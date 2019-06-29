/*
	File:		DiskCacheExtensions.c

	Contains:	All of the entry points for reading and writing TrueType font caches
				from and to the disk. The main entry points are:
				
					Routine											Called By
					
					InitializeDiskCache								Font Manager
					DiskCachePurgeProcedure							Memory Manager
					DiskCacheResourceManagerPurgeProcedure			Memory Manager
					DiskCacheShutDownProcedure						Shutdown Manager
					LoadFontCacheFromDisk							Bass Cache Code
					CheckForMaximumDiskCacheSize					Bass Cache Code
					DumpFontCacheToDiskIfNotTooBig					Bass Cache Code
					GetFontCacheFileSize							Font Cache Control Panel
					SetFontCacheFileSize							Font Cache Control Panel
					GetFontCacheFileStatus							Font Cache Control Panel
					SetFontCacheFileStatus							Font Cache Control Panel
					FontCacheFileExists								Font Cache Control Panel

	Written by:	John Farmer

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	10/28/92	DTY		Use Get/Set macros to access ExpandMem.
		 <4>	  4/1/92	YK		#1023997: InitializeDiskCashe should be a pascal function.
		 <3>	 3/27/92	DTY		#1024868: Remove calls to Assert since it doesn’t do anything
									any more.
		 <2>	  1/3/92	JSM		Roll-in changes from Sakura: Get name of disk cache file from a
									resource instead of hard-coding it in InitializeDiskCache()
									(BRC# 1017240). Add code to (a) minimize the window of
									opportunity for losing the integrity of the font cache file, and
									(b) indicate the file is dirty so we can (c) rebuild the map if
									the file is opened and the map is inconsistent with the cache
									blocks. FindOrPurgeSpaceForDiskCache() is now
									MakeSpaceInDiskCache(), add DumpFontCacheToDiskIfNotTooBig().
									Add more asserts and use squashed in LoadFontCacheFromDisk().

		System 6.1.0 Changes:
		
		 <3>	 7/17/91	jlf		Split file into smaller pieces.
		
		 <2>	 7/14/91	jlf		Redesigned since we can't allocate memory
									when called from a purge proc. The disk cache map is
									no longer dynamically allocated and buffer space for
									compressing the cache file is retrieved from a separate
									heap.
							
		 <1>	 6/29/91	jlf		Created file and documentation.

*/

// Include Statements


	#include	"BaseIncludes.h"
	#include	"Assertion.h"
	#include	"Errors.h"
	#include	"ExpandMemPriv.h"
	#include	"Files.h"
	#include	"Memory.h"
	#include	"OSUtils.h"
	#include	"Shutdown.h"
	#include	"Resources.h"
	
	#include	"DoubleByteBassCache.h"
	#include	"FSCdefs.h"
	#include	"MapString.h"
	#include	"PartialFontExtensions.proto"
	#include	"sfnt.h"
	#include	"sfnt_enum.h"
	#include	"FontScaler.h"
	#include	"Bass_Cache.h"
	
	#include	"AddDiskCache.proto"
	#include	"DiskCacheExtensions.proto"
	#include	"DiskCacheMap.proto"
	#include	"DiskCacheList.proto"
	
			
// Prototypes


	extern pascal EncodingTablePointer		GetEncodingTable( Integer fondID, Integer *scriptCodePointer );


/*	--------------------------------------------------------------------------------------

	Routine:	error = InitializeDiskCache( )
		
	Purpose:	Initialize private data structures for the disk based font caches,
				attempt to open a font cache file in the system folder, load the disk
				cache map, install a diskcache shutdown procedure, and place a disk cache
				purge procedure in the system heap.
				
				If the map is marked dirty, create a new, empty map and write it out.
				
				This does not attempt to repair files which have a damaged, non-regulation-sized map.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


pascal OSErr InitializeDiskCache( void )

{

	// Locals
	
		OSErr									error;
		THz										systemZonePointer;
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
		DiskCacheMapStructureHandle				diskCacheMapHandle;
		SysEnvRec								environment;
		ParamBlockRec							parameterBlock;
		Handle									diskCacheFileName;
	
	error = noErr;
	
	(*splineKeyHandle)->fDiskCacheMap = nil;
	(*splineKeyHandle)->fInPurgeProcedure = false;
	(*splineKeyHandle)->fFontCacheExists = false;
	
	error = SysEnvirons( 1, &environment );
	if ( error == noErr ) {
	
		// get the name of the disk cache file
		diskCacheFileName = GetResource('STR ', kDiskCacheFileNameResID);
		if (diskCacheFileName) {
		
			HLock(diskCacheFileName);
			
			parameterBlock.ioParam.ioNamePtr = *diskCacheFileName;
			parameterBlock.ioParam.ioVRefNum = environment.sysVRefNum;
			parameterBlock.ioParam.ioVersNum = 0;
			parameterBlock.ioParam.ioPermssn = fsRdWrPerm;
			parameterBlock.ioParam.ioMisc = nil;
			error = PBOpen( &parameterBlock, false );
			
			HUnlock(diskCacheFileName);
			ReleaseResource(diskCacheFileName);
			
			if ( error != fnfErr ) {
			
				if ( error == noErr ) {
				
					error = LoadDiskCacheMap( &diskCacheMapHandle, parameterBlock.ioParam.ioRefNum );
					if ( error != noErr ) {
					
						PBClose( &parameterBlock, false );
						
					} 
					else
					{
						if ((*diskCacheMapHandle)->fMapDirty)
							// Assume that we got back a reasonably-sized handle from LoadDiskCacheMap.
							// Fill in values to make this a good, empty map.  It’s entirely likely that
							// the map is mostly good, but we’ll do the whole job.  This code not entirely
							// unlike CreateDiskCacheMap in the disk cache builder project.
						{
							// Sub-locals
								DiskCacheOffset						diskCacheArraySize, diskCacheMapSize;
								DiskCacheMapStructurePointer		diskCacheMapPointer;
								StringListStructure					*stringListPointer;
								DiskCacheIndex						cacheIndex;
								DiskCacheEntryStructurePointer		cacheEntryPointer;
	
							diskCacheArraySize = sizeof( DiskCacheMapStructure) + (kMaximumDiskCacheCount * sizeof( DiskCacheEntryStructure ));
							diskCacheMapSize = diskCacheArraySize + kFontNameListSize;
							
							diskCacheMapPointer = *diskCacheMapHandle;
							diskCacheMapPointer->fFileVersion = kCurrentDiskCacheFileVersion;
							diskCacheMapPointer->fFileRefNum = parameterBlock.ioParam.ioRefNum;
		
							diskCacheMapPointer->fMapDirty = !kMapDirty;
							diskCacheMapPointer->fMapRebuildCount++;	// here for anyone interested in how many times this has happened
		
							diskCacheMapPointer->fFontCacheIsOn = true;
							
							diskCacheMapPointer->fFirstOffsetIndex = kDiskCacheNotFound;
							diskCacheMapPointer->fLastOffsetIndex = kDiskCacheNotFound;
							diskCacheMapPointer->fFirstDateIndex = kDiskCacheNotFound;
							diskCacheMapPointer->fLastDateIndex = kDiskCacheNotFound;
							
							diskCacheMapPointer->fFontNameListOffset = diskCacheArraySize;
							diskCacheMapPointer->fDiskCacheMapSize = diskCacheMapSize;
							diskCacheMapPointer->fMaximumDiskCacheCount = kMaximumDiskCacheCount;
							diskCacheMapPointer->fDiskCacheCount = 0;
							
							stringListPointer = (StringListStructure*) ((Ptr) diskCacheMapPointer + diskCacheArraySize);
							stringListPointer->fStringCount = 0;
							
							// Mark all entries unused
							cacheEntryPointer = &diskCacheMapPointer->fDiskCache[0];
							for ( cacheIndex = 0; cacheIndex < kMaximumDiskCacheCount; cacheIndex++ )
							{
								cacheEntryPointer->fCacheIsFree = true;
								cacheEntryPointer++;
							}
														
							error = DumpDiskCacheMap( diskCacheMapHandle );	//put it on disk
						}
						
						ShutDwnInstall( (ShutDwnProcPtr) DiskCacheShutDownProcedure, sdRestartOrPower + sdOnUnmount );
						
						systemZonePointer = SystemZone( );
						systemZonePointer->purgeProc = (ProcPtr) DiskCachePurgeProcedure;
						(*splineKeyHandle)->fDiskCacheMap = (Handle) diskCacheMapHandle;
						(*splineKeyHandle)->fFontCacheExists = true;
						
					}
					
				}
				
			}
		
		}
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	FindCacheInCacheList( purgeableHandle, splineKeyHandle )
		
	Purpose:	Search through the TrueType cache list looking for purgeableHandle. If
				purgeableHandle is in the cache return the cache's list index. Otherwise,
				return kCacheNotFound.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Integer FindCacheInCacheList( Handle purgeableHandle, sb_SplineKeyHdl splineKeyHandle )

{

	// Locals
	
		Boolean									found;
		Integer									cacheListIndex, cacheListCount;
		CacheReference							*cacheReferencePointer, **cacheListHandle;
		
	found = false;
	cacheListHandle = (*splineKeyHandle)->cacheListHandle;
	
	if ( cacheListHandle != nil ) {

		cacheListIndex = 0;
		cacheListCount = (*splineKeyHandle)->fontCount;
		cacheReferencePointer = *cacheListHandle;
		
		while ( (found == false) && (cacheListIndex < cacheListCount) ) {
		
			if ( cacheReferencePointer->cache == purgeableHandle ) {
			
				found = true;
			
			} else {
			
				cacheListIndex++;
				cacheReferencePointer++;
				
			}
		
		}
	
	}
	
	return( found ? cacheListIndex : kCacheNotFound );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	DiskCachePurgeProcedure( purgeableHandle )
		
	Purpose:	If purgeableHandle is a TrueType font cache call the DumpFontCacheToDiskIfNotTooBig
				routine. This routine is called by the Memory Manager during it's purge
				sequence - re-entering the memory manager with this heap would be a
				really bad idea at this point. Manipulation of the current heap should
				be avoided.
				
				The splineKey global fInPurgeProcedure is a simple reentrancy check. We
				wouldn't want to allocate memory in another heap in order to dump a cache
				in the current heap only to have the purge proc of the other heap invoked.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


pascal void DiskCachePurgeProcedure( Handle purgeableHandle )

{

	// Locals
	
		Integer									cacheListIndex;
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
	
	if ( (*splineKeyHandle)->fInPurgeProcedure == false ) {
	
		(*splineKeyHandle)->fInPurgeProcedure = true;
		
		cacheListIndex = FindCacheInCacheList( purgeableHandle, splineKeyHandle );
		if ( cacheListIndex != kCacheNotFound ) {
		
			// only dump cache to disk if it's smaller than the maximum size
			DumpFontCacheToDiskIfNotTooBig( (sb_CacheHeadHdl) purgeableHandle );
		
		}

		(*splineKeyHandle)->fInPurgeProcedure = false;
	
	}
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	DiskCacheResourceManagerPurgeProcedure( purgeableHandle )
		
	Purpose:	Almost the same as the DiskCachePurgeProcedure routine. The main difference
				is that we call a saved resource manager purge procedure if the handle
				passed in is not a TrueType font cache.
				
				The resource manager purge procedure is saved into the spline key globals
				during the SetResPurge call and the address is placed into the purge proc
				field of the current heap instead.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


pascal void DiskCacheResourceManagerPurgeProcedure( Handle purgeableHandle )

{

	// Locals
	
		Integer									cacheListIndex;
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
		PurgeProcPtr							resourceManagerPurgeProcedure;
		
	resourceManagerPurgeProcedure = (*splineKeyHandle)->fResourceManagerPurgeProcedure;
	
	if ( (*splineKeyHandle)->fInPurgeProcedure ) {
	
		if ( resourceManagerPurgeProcedure != nil ) {
		
			(*resourceManagerPurgeProcedure)( purgeableHandle );
	
		}
		
	} else {

		(*splineKeyHandle)->fInPurgeProcedure = true;
		
		cacheListIndex = FindCacheInCacheList( purgeableHandle, splineKeyHandle );
		if ( cacheListIndex == kCacheNotFound ) {
		
			if ( resourceManagerPurgeProcedure != nil ) {
			
				(*resourceManagerPurgeProcedure)( purgeableHandle );
			
			}
		
		} else {
		
			// only dump cache to disk if it's smaller than the maximum size
			DumpFontCacheToDiskIfNotTooBig( (sb_CacheHeadHdl) purgeableHandle );
		
		}
		
		(*splineKeyHandle)->fInPurgeProcedure = false;
		
	}
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	DiskCacheShutDownProcedure( )
		
	Purpose:	Call DumpFontCacheToDiskIfNotTooBig for each cache in the global cache list followed
				by a call to DumpDiskCacheMap and close call for the font cache file.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


pascal void DiskCacheShutDownProcedure( void )

{

	// Locals
	
		Integer									cacheListIndex, cacheListCount;
		THz										savedZone;
		CacheReference							*cacheReferencePointer, **cacheListHandle;
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
		DiskCacheMapStructureHandle				diskCacheMapHandle;
		
	diskCacheMapHandle = (DiskCacheMapStructureHandle) (*splineKeyHandle)->fDiskCacheMap;
	if ( (*splineKeyHandle)->fFontCacheExists ) {
	
		if ( (*diskCacheMapHandle)->fFontCacheIsOn ) {
		
			savedZone = GetZone( );
			
			cacheListHandle = (*splineKeyHandle)->cacheListHandle;
			if ( cacheListHandle != nil ) {
		
				cacheListCount = (*splineKeyHandle)->fontCount;
				for ( cacheListIndex = 0; cacheListIndex < cacheListCount; cacheListIndex++ ) {
				
					cacheReferencePointer = *cacheListHandle + cacheListIndex;
					if ( cacheReferencePointer->cache != nil ) {
					
						SetZone( cacheReferencePointer->zone );

						// only dump cache to disk if it's smaller than the maximum size
						DumpFontCacheToDiskIfNotTooBig( (sb_CacheHeadHdl) cacheReferencePointer->cache );
					
					}
								
				}
			
			}
			
			SetZone( savedZone );
	
		}
			
		DumpDiskCacheMap( diskCacheMapHandle );
		FSClose( (*diskCacheMapHandle)->fFileRefNum );
					
	}
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = LoadFontCacheFromDisk( &cacheHandle, &cacheZone, widthTableHandle )
		
	Purpose:	To search for, and load if found, a TrueType font cache from the font cache
				file. The font cache file must exist and the font cache file must be on in
				order for this routine to do anything.
				
				The cache is searched for using information in the widthTableHandle parameter.
				If the cache is found a new handle is allocated, the cache data is read from
				the disk, volatile fields of the cache are initialized, the cache is
				moved to the front of the date list, and the cacheHandle/cacheZone parameters
				are assigned the proper values.
				
				sb_NewZoneHandle is a Bass_Cache memory allocation routine that is shared
				by this routine.
				
	Warnings:	None
	
	Assumes:	This doesn’t get called if this cache block is already in memory.
	
	-----------------------------------------------------------------------------------  */


OSErr LoadFontCacheFromDisk( sb_CacheHeadHdl* cacheHandlePointer, THz* cacheZonePointer, widthTableHdl widthTableHandle, Boolean squashed )

{

	// Locals
	
		OSErr									error;
		Integer									cacheFileRefNum, scriptNumber;
		DiskCacheIndex							cacheIndex;
		DiskCacheOffset							cacheSize, cacheOffset;
		sb_CacheHeadPtr							cachePointer;
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
		DiskCacheMapStructureHandle				diskCacheMapHandle;

	error = noErr;
	*cacheHandlePointer = nil;
	
	diskCacheMapHandle = (DiskCacheMapStructureHandle) (*splineKeyHandle)->fDiskCacheMap;
	if ( (*splineKeyHandle)->fFontCacheExists && (*diskCacheMapHandle)->fFontCacheIsOn ) {
		
		error = SearchForDiskCacheUsingWidthTable( &cacheIndex, widthTableHandle, diskCacheMapHandle, squashed );
		if ( (error == noErr) && (cacheIndex != kDiskCacheNotFound) ) {
		
			cacheSize = (*diskCacheMapHandle)->fDiskCache[cacheIndex].fLogicalSize;
			*cacheHandlePointer = (sb_CacheHeadHdl) sb_NewZoneHandle( splineKeyHandle, cacheSize, cacheZonePointer );
			if ( *cacheHandlePointer == nil ) {
			
				error = memFullErr;
			
			} else {
			
				cacheFileRefNum = (*diskCacheMapHandle)->fFileRefNum;
				cacheOffset = (*diskCacheMapHandle)->fDiskCache[cacheIndex].fLogicalOffset;
				error = SetFPos( cacheFileRefNum, fsFromStart, cacheOffset );
				if ( error == noErr ) {
				
					error = FSRead( cacheFileRefNum, &cacheSize, (Ptr) **cacheHandlePointer );
					if ( error == noErr  ) {
					
						cachePointer = (sb_CacheHeadPtr) StripAddress( (Ptr) **cacheHandlePointer );
						cachePointer->familyID = (*widthTableHandle)->fID;
						cachePointer->StrikeID = (*widthTableHandle)->StrikeID;
						cachePointer->fEncodingTablePointer = (Ptr) GetEncodingTable( cachePointer->familyID, &scriptNumber );

						RemoveDiskCacheFromDateList( cacheIndex, diskCacheMapHandle );
						InsertDiskCacheIntoDateList( cacheIndex, (*diskCacheMapHandle)->fFirstDateIndex, diskCacheMapHandle );
						
					}
					
				}
				
			}
			
		}
	
	}
	
	if ( (error != noErr) && (*cacheHandlePointer != nil) ) {
	
		sb_DisposeZoneHandle( (Handle) *cacheHandlePointer, *cacheZonePointer );
		*cacheHandlePointer = nil;
	
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = DumpFontCacheToDiskIfNotTooBig( cacheHandle )
		
	Purpose:	Calls DumpFontCacheToDisk if the size of the cache isn't too big.
								
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr DumpFontCacheToDiskIfNotTooBig( sb_CacheHeadHdl cacheHandle )

{

	// Locals
	
		OSErr									error;

	error = noErr;
	

	// only dump cache to disk if it's smaller than the maximum size
	if ( GetHandleSize( (Handle) cacheHandle ) <= kMaximumDiskCacheSize ) {
	
		error = DumpFontCacheToDisk( cacheHandle );
	
	}

	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = DumpFontCacheToDisk( cacheHandle )
		
	Purpose:	To write a font cache to the font cache file. The font cache file must
				exist and the font cache file must be on in order for this routine to
				do anything.
				
				The cache is searched for using information in the cacheHandle parameter.
				If the cache is not found a call to AddDiskCache is made. Otherwise, if the
				cache is not locked (necessary for Pacific caches where regenerating a cache
				for the system font is too painful), the cache is less than the maximum
				disk cache size, and the memory cache size is greater than the the disk
				cache size, the cache is written to the font cache file. If there is enough
				expansion space after the cache to hold the extra cache information the cache
				is written to it's current disk position. Otherwise, the cache is removed and
				readded to the font cache file (similar to a memory compaction in the memory
				manager).
				
				This marks the map dirty before writing the cache block, and writes the map
				afterwards (also clearing the dirty flag).  This adds a little time, but it
				reduces the opportunity for the map to be inconsistent, and gives us the
				chance to notice it if it is and repair it by installing an empty map.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr DumpFontCacheToDisk( sb_CacheHeadHdl cacheHandle )

{

	// Locals
	
		OSErr									error;
		DiskCacheIndex							cacheIndex;
		DiskCacheOffset							cacheSize, expansionSize, cacheOffset,
												nextCacheOffset;
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
		DiskCacheEntryStructurePointer			cacheEntryPointer;
		DiskCacheMapStructureHandle				diskCacheMapHandle;
		
	error = noErr;
	
	diskCacheMapHandle = (DiskCacheMapStructureHandle) (*splineKeyHandle)->fDiskCacheMap;
	if ( (*splineKeyHandle)->fFontCacheExists && (*diskCacheMapHandle)->fFontCacheIsOn && (StripAddress( (Ptr) *cacheHandle ) != nil) && ((*cacheHandle)->bitsCached == true) ) {
	
		error = SearchForDiskCacheUsingCache( &cacheIndex, cacheHandle, diskCacheMapHandle );
		if ( error == noErr ) {
		
			if ( cacheIndex == kDiskCacheNotFound ) {
			
				error = AddDiskCache( &cacheIndex, cacheHandle, diskCacheMapHandle );
			
			} else {
			
				cacheSize = GetHandleSize( (Handle) cacheHandle );
				cacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
				if ( (cacheEntryPointer->fCacheIsLocked == false) && (cacheEntryPointer->fCacheIsMaximumSize == false) ) {
				
					if ( cacheSize > cacheEntryPointer->fLogicalSize ) {
					
						cacheEntryPointer->fCacheIsMaximumSize = (cacheSize >= kMaximumDiskCacheSize);
						error = GetCacheExpansionInformation( &expansionSize, &cacheOffset, &nextCacheOffset, cacheIndex, diskCacheMapHandle );
						if ( error == noErr ) {
						
							if ( cacheSize <= cacheEntryPointer->fLogicalSize + expansionSize ) {
							
								cacheEntryPointer->fLogicalSize = cacheSize;
								
								//write the block and the map
								error = WriteDiskCache( cacheIndex, cacheHandle, diskCacheMapHandle );
							
							} else {
							
								error = RemoveDiskCache( cacheIndex, diskCacheMapHandle );
								if ( error == noErr ) {
								
									error = AddDiskCache( &cacheIndex, cacheHandle, diskCacheMapHandle );
								
								}
							
							}
					
						}
						
					}
				
				}
			
			}
		
		}
		
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = CheckForMaximumDiskCacheSize( cacheHandle )
		
	Purpose:	If the cache size is greater than the maximum cache size dump it to disk.
				Called by sb_RetrieveGlyph, so the cache will be at most the size of the
				bits of one glyph over the maximum.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr CheckForMaximumDiskCacheSize( sb_CacheHeadHdl cacheHandle )

{

	// Locals
	
		OSErr									error;
		DiskCacheOffset							cacheSize;
		
	error = noErr;
	
	cacheSize = GetHandleSize( (Handle) cacheHandle );
	if ( cacheSize >= kMaximumDiskCacheSize ) {
	
		error = DumpFontCacheToDisk( cacheHandle );
	
	}
	
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = GetFontCacheFileSize( &fileSize )
		
	Purpose:	Return the size of the font cache file if it exists.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr GetFontCacheFileSize( DiskCacheOffset* fileSizePointer )

{

	// Locals
	
		OSErr									error;
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
		DiskCacheMapStructureHandle				diskCacheMapHandle;
		
	error = noErr;
	*fileSizePointer = 0;
	
	diskCacheMapHandle = (DiskCacheMapStructureHandle) (*splineKeyHandle)->fDiskCacheMap;
	if ( (*splineKeyHandle)->fFontCacheExists ) {
	
		error = GetEOF( (*diskCacheMapHandle)->fFileRefNum, fileSizePointer );
		
	}
	
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = SetFontCacheFileSize( fileSize )
		
	Purpose:	Resize the font cache file, purging and compressing as necessary to obtain
				the requested file size. CompressDiskCacheFile must be called after
				MakeSpaceInDiskCache since MakeSpaceInDiskCache may find the requested
				space between two caches and not compress the file.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr SetFontCacheFileSize( DiskCacheOffset newFileSize )

{

	// Locals
	
		OSErr									error, error2;
		Integer									fileRefNum;
		DiskCacheIndex							cacheIndex;
		DiskCacheOffset							oldFileSize, logicalOffset;
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
		DiskCacheMapStructureHandle				diskCacheMapHandle;
		
	error = noErr;
	
	diskCacheMapHandle = (DiskCacheMapStructureHandle) (*splineKeyHandle)->fDiskCacheMap;
	if ( (*splineKeyHandle)->fFontCacheExists ) {
	
		fileRefNum = (*diskCacheMapHandle)->fFileRefNum;
		error = GetEOF( fileRefNum, &oldFileSize );
		if ( (error == noErr) && (newFileSize != oldFileSize) ) {
		
			if ( newFileSize < oldFileSize ) {
			
				error = MakeSpaceInDiskCache( &cacheIndex, &logicalOffset, oldFileSize - newFileSize, diskCacheMapHandle );
				if ( error == noErr ) {
				
					error = CompressDiskCacheFile( diskCacheMapHandle );
				
				}
				
				error2 = DumpDiskCacheMap( diskCacheMapHandle );
				if ( error == noErr )
					error = error2;
				
			}
		
			if ( error == noErr ) {
			
				error = SetEOF( fileRefNum, newFileSize );
				
			}
			
		}
		
	}
	
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	cacheExists = GetFontCacheFileStatus( )
		
	Purpose:	Return true if the font cache file exists and is currently turned on.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Boolean GetFontCacheFileStatus( void )

{

	// Locals
	
		Boolean									fontCacheIsOn;
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
		DiskCacheMapStructureHandle				diskCacheMapHandle;
		
	fontCacheIsOn = false;
	if ( (*splineKeyHandle)->fFontCacheExists ) {
	
		diskCacheMapHandle = (DiskCacheMapStructureHandle) (*splineKeyHandle)->fDiskCacheMap;
		fontCacheIsOn = (*diskCacheMapHandle)->fFontCacheIsOn;
	
	}
	
	return( fontCacheIsOn );

}


/*	--------------------------------------------------------------------------------------

	Routine:	SetFontCacheFileStatus( cacheStatus )
		
	Purpose:	Turn the font cache on or off based on cacheStatus. No action is taken if
				the font cache file doesn't exist.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void SetFontCacheFileStatus( Boolean cacheStatus )

{

	// Locals
	
		sb_SplineKeyHdl							splineKeyHandle = SPLINE_KEY_HAND;
		DiskCacheMapStructureHandle				diskCacheMapHandle;

	if ( (*splineKeyHandle)->fFontCacheExists ) {

		diskCacheMapHandle = (DiskCacheMapStructureHandle) (*splineKeyHandle)->fDiskCacheMap;
		(*diskCacheMapHandle)->fFontCacheIsOn = cacheStatus;
	
	}

}


/*	--------------------------------------------------------------------------------------

	Routine:	cacheExists = FontCacheFileExists( )
		
	Purpose:	Return true if expandMem is present, the emSplineKey field is within the
				expandMem block (a nasty test in my opinion), and the fFontCacheExists
				field is true.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Boolean FontCacheFileExists( void )

{

	// Locals
		
		Boolean									fontCacheFileExists;
		Size									splineKeyExpandMemFieldOffset;
		sb_SplineKeyHdl							splineKeyHandle;
		
	fontCacheFileExists = false;
	
	splineKeyExpandMemFieldOffset = (Ptr) &(GetExpandMemSplineKey()) - (Ptr) GetExpandMem();
	if ( splineKeyExpandMemFieldOffset + sizeof( Handle) <= GetExpandMemSize() ) {
	
		splineKeyHandle = (sb_SplineKeyHdl) GetExpandMemSplineKey();
		fontCacheFileExists = (*splineKeyHandle)->fFontCacheExists;
		
	}
			
	
	return( fontCacheFileExists );

}