/*
	File:		AddDiskCache.c

	Contains:	Code for dumping font caches to disk.

	Written by:	John Farmer

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 3/27/92	DTY		#1024868: Remove calls to Assert since it doesn’t do anything
									any more.
		 <2>	  1/3/92	JSM		Roll-in changes from Sakura: Add code to (a) minimize the window
									of opportunity for losing the integrity of the font cache file,
									and (b) indicate the file is dirty so we can (c) rebuild the map
									if the file is opened and the map is inconsistent with the cache
									blocks. Add some error checking, comments, and clarify some
									names. Remove code which cleared and restored the purgeProc
									during an allocation. Set fSquashed in AddDiskCache(). Add more
									asserts in MoveFileDataDown().

		System 6.1.0 Changes:
		
		 <1>	 7/17/91	jlf		Split these routines out of DiskCacheExtensions.c
*/

// Include Statements


	#include	"BaseIncludes.h"
	#include	"Assertion.h"

	#include	"Errors.h"
	#include	"Fonts.h"
	#include	"Files.h"

	#include	"DoubleByteBassCache.h"
	#include	"FSCdefs.h"
	#include	"MapString.h"
	#include	"PartialFontExtensions.proto"
	#include	"sfnt.h"
	#include	"sfnt_enum.h"
	#include	"FontScaler.h"
	#include	"Bass_Cache.h"

	#include	"DiskCacheList.proto"
	#include	"DiskCacheMap.proto"
	

/*	--------------------------------------------------------------------------------------

	Routine:	GetCacheExpansionInformation( &expansionSize, &expansionOffset, &nextCacheOffset, cacheIndex, 
											  diskCacheMapHandle )
		
	Purpose:	Given a cacheIndex and a diskCacheMapHandle, return:
	
					expansionSize   - the number of bytes the cache can expand on disk before
									  bumping into the next cache.
									
					expansionOffset	- an offset to the end of the cache specified by cacheIndex.
									  Could also be viewed as a starting pointer for a new cache
									  if expansionSize is big enough.

					nextCacheOffset	- an offset to the start of the next cache on disk.
					
				The nextCacheOffset for the last cache is always the logical end of the file.
				
				If there are no caches the expansionOffsetPointer is set to the end of
				the diskCacheMap and the nextCacheOffset is set to the end of the file.
				
				ExpansionSize is always the difference between the start of the next cache
				and the end of the requested cache.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr GetCacheExpansionInformation( DiskCacheOffset* expansionSizePointer, 
									DiskCacheOffset* expansionOffsetPointer, 
									DiskCacheOffset* nextCacheOffsetPointer, 
									DiskCacheIndex cacheIndex, 
									DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		OSErr									error;
		DiskCacheIndex							nextCacheIndex;
		DiskCacheEntryStructurePointer			cacheEntryPointer;
		
	error = noErr;
	
	if ( (*diskCacheMapHandle)->fDiskCacheCount == 0 ) {//no blocks in the file, just the header	
	
		*expansionOffsetPointer = (*diskCacheMapHandle)->fDiskCacheMapSize;
		error = GetEOF( (*diskCacheMapHandle)->fFileRefNum, nextCacheOffsetPointer );
		
	} else {
	
		cacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
		*expansionOffsetPointer = cacheEntryPointer->fLogicalOffset + cacheEntryPointer->fLogicalSize;

		nextCacheIndex = (*diskCacheMapHandle)->fDiskCache[cacheIndex].fNextOffsetIndex;
		if ( nextCacheIndex == kDiskCacheNotFound ) {//last block is special case
		
			error = GetEOF( (*diskCacheMapHandle)->fFileRefNum, nextCacheOffsetPointer );
		
		} else {//get offset of the next block in file order
		
			*nextCacheOffsetPointer = (*diskCacheMapHandle)->fDiskCache[nextCacheIndex].fLogicalOffset;

		}
		
	}

	*expansionSizePointer = *nextCacheOffsetPointer - *expansionOffsetPointer;

	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = MarkMapDirty( dirtyOrNot, diskCacheMapHandle )
		
	Purpose:	Mark the map as kMapDirty or not by setting a field and writing the map.
				
				The kMapDirty flag is used by InitializeDiskCache to determine whether
				the map is consistent with the file contents.  If the answer is no, 
				InitializeDiskCache destroys the contents of the file and implants a new map.
				
				Call this whenever the map no longer matches the blocks in either size
				or offset.
				
				Do not mark the map dirty when you change the sort-order of the blocks.
				The consequences of the map reflecting a different sort order is of
				little significance.
				
				Altering a list happens frequently (and should actually happen whenever
				a glyph is referenced as opposed to when a strike is read).  Marking the
				map dirty causes the map to be written out.  A _Write for every glyph 
				reference would be bad.  
				
	Warnings:	Always follow a MarkMapDirty( kMapDirty ) with a MarkMapDirty( !MarkMapDirty ).
				
	Assumes:	diskCacheMapHandle is locked
	-----------------------------------------------------------------------------------  */


OSErr MarkMapDirty( Boolean dirtyOrNot, DiskCacheMapStructureHandle diskCacheMapHandle )

{
	// Locals
	
		OSErr	error;

		(*diskCacheMapHandle)->fMapDirty = dirtyOrNot;
		error = DumpDiskCacheMap( diskCacheMapHandle );
		
	return( error );
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = PurgeDiskCaches( spaceNeeded, diskCacheMapHandle )
		
	Purpose:	Purge spaceNeeded bytes out of the disk cache specified by diskCacheMapHandle.
				A two pass approach is used to purge space: the first pass cycles through
				the cache date list from least-used to most-used to determine if it's possible
				to obtain spaceNeeded bytes. The second pass actually removes the data. This
				allows outrageous requests to be processed without destroying any data.
				
				The space may be made piecemeal and noncontiguous.
				
				If it's not possible to obtain spaceNeeded bytes, kShrinkFailedError is
				returned.
				
				Mark the map dirty before starting, then mark it clean when done, writing
				the map in the process.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr PurgeDiskCaches( DiskCacheOffset spaceNeeded, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals

		Boolean									spaceNotFound;
		OSErr									error;
		DiskCacheIndex							cacheIndex, previousCacheIndex;
		DiskCacheOffset							purgeableSpace;
		DiskCacheEntryStructurePointer			cacheEntryPointer;
		
	error = noErr;
	
	purgeableSpace = 0;
	spaceNotFound = true;

	//walk the list from oldest to newest
	cacheIndex = (*diskCacheMapHandle)->fLastDateIndex;
	while ( spaceNotFound && (cacheIndex != kDiskCacheNotFound) ) {//not enough yet and not at the end of the list
	
		cacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
		if ( cacheEntryPointer->fCacheIsLocked == false ) {
		
			purgeableSpace += cacheEntryPointer->fLogicalSize;
			spaceNotFound = (purgeableSpace < spaceNeeded);
		
		}
		
		cacheIndex = cacheEntryPointer->fPreviousDateIndex;	//next item in the list
	
	}
	
	if ( spaceNotFound ) {
	
		error = kShrinkFailedError;
	
	} else {//go purge blocks until the request is satisfied
		
		error = MarkMapDirty( kMapDirty, diskCacheMapHandle );
		
		purgeableSpace = 0;
		cacheIndex = (*diskCacheMapHandle)->fLastDateIndex;
		while ( (error == noErr) && (purgeableSpace < spaceNeeded) && (cacheIndex != kDiskCacheNotFound) ) {
		
			cacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
			previousCacheIndex = cacheEntryPointer->fPreviousDateIndex;
			
			if ( cacheEntryPointer->fCacheIsLocked == false ) {
				
				//get block size from the cache entry, then remove the cache entry
				purgeableSpace += cacheEntryPointer->fLogicalSize;
				
				error = RemoveDiskCache( cacheIndex, diskCacheMapHandle );

			}
			
			cacheIndex = previousCacheIndex; //next, please

		}
	
	}
	
	if ( error == noErr )
		error = MarkMapDirty( !kMapDirty, diskCacheMapHandle );

	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = AllocateLockedBufferForCompression( &bufferSize, &bufferHandle, &bufferZone )
		
	Purpose:	Allocate a buffer that can be used to compress the font cache file. This
				routine normally gets called in total deep shit situations (see the
				CompressDiskCacheFile routine below).  The block allocated here uses up
				all available space in the “other” zone, but that’s ok since we’re modal
				until our caller disposes of this block.
				
				bufferSizePointer is assigned the largest contiguous block size of the
				opposite heap (opposite heap is defined as the application heap if the
				current heap is the system heap and vice versa; see the warnings section
				below).
				
				bufferHandlePointer is assigned the allocated, locked handle.
				
				bufferZone is assigned the opposite heap.
				
				The routine will fail if kMinimumDiskCacheBufferSize bytes cannot be allocated.
				Less than this and a compression will take so long that it’s not worth doing.
				
	Warnings:	Memory cannot be allocated in the current zone (we may have been called from
				the memory manager through a purge procedure).
				
				The growzone procedure of any heap cannot be invoked since, under MultiFinder,
				it could end up resizing the current zone (the growzone proc in the multi heap
				resizes the system heap and the growzone proc of the system heap resizes the
				multi heap).
	
	-----------------------------------------------------------------------------------  */


OSErr AllocateLockedBufferForCompression( Size* bufferSizePointer, Handle* bufferHandlePointer, 
										  THz* bufferZonePointer )
{

	// Locals
	
		OSErr									error;
		GrowZoneProcPtr							savedGrowZoneProcedure;
		THz										savedZone;
		
	error = noErr;
	
	savedZone = GetZone( );

	//find out which zone is not the current zone?
	*bufferZonePointer = (savedZone == SystemZone( )) ? ApplicZone( ) : SystemZone( );

	savedGrowZoneProcedure = (*bufferZonePointer)->gzProc;
	
	(*bufferZonePointer)->gzProc = nil;
	
	SetZone( *bufferZonePointer );
	*bufferSizePointer = CompactMem( maxSize );
	
	if (kMinimumDiskCacheBufferSize <= *bufferSizePointer) //there’s enough for a buffer
	{	
		*bufferHandlePointer = NewHandle( *bufferSizePointer );
		
		//Since we’ve been so careful to ensure that we allocate only what we know we can get,
		//there is no good reason to expect that an error could occur.
		//One exception might be a patch to NewHandle which causes heap movement or allocation.
		if ( (error = MemError( )) == noErr ) {
			HLock( *bufferHandlePointer );
		}
	} else {
		error = memFullErr;
	}
	(*bufferZonePointer)->gzProc = savedGrowZoneProcedure;
	SetZone( savedZone );
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	DisposeOfCompressionBuffer( bufferHandle, bufferZone )
		
	Purpose:	Dispose of the compression buffer allocated by AllocateLockedBufferForCompression.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void DisposeOfCompressionBuffer( Handle bufferHandle, THz bufferZone )

{

	// Locals
	
		THz										savedZone;
		
	savedZone = GetZone( );
	SetZone( bufferZone );
	DisposHandle( bufferHandle );
	SetZone( savedZone );

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = MoveFileDataDown( fileRefNum, sourceOffset, destinationOffset, dataSize, 
										  bufferSize, bufferHandle )
		
	Purpose:	Sort-of a special case disk-based blockmove. Move dataSize bytes from
				sourceOffset to destinationOffset. SourceOffset and destinationOffset
				may overlap but sourceOffset must be greater than destinationOffset.
				BufferHandle is used to store the data between reads and writes; bufferSize
				specifies the size of bufferHandle.

				A multipass approach is used to copy the data, copying bufferSize bytes
				until dataSize bytes have been copied. If the number of bytes left to copy
				during the last pass is less than bufferSize, bytesLeftToCopy bytes is copied
				instead of bufferSize bytes.
				
				In the case that something weird happens and FSRead returns a different number
				of bytes read that the amount requested, the routine should still work.
				
	Note:		sourceOffset should be greater than destinationOffset.
	
				This routine must not move memory. Mainly because I'm lazy and don't want
				to update pointers after calling this routine in CompressDiskCacheFile.
	
	-----------------------------------------------------------------------------------  */


OSErr MoveFileDataDown( Integer fileRefNum, DiskCacheOffset sourceOffset, DiskCacheOffset destinationOffset, 
						DiskCacheOffset dataSize, Size bufferSize, Handle bufferHandle )

{

	// Locals
	
		OSErr								error;
		DiskCacheOffset						bytesCopied, bytesLeftToCopy, copyCount;
		
	error = noErr;
	
	if ( sourceOffset > destinationOffset ) {//short-circuits if zero-length or out-of-order
	
		bytesCopied = 0;
		bytesLeftToCopy = dataSize;
		while ( (error == noErr) && (bytesLeftToCopy > 0) ) {
		
			copyCount = (bytesLeftToCopy > bufferSize) ? bufferSize : bytesLeftToCopy;
			
			error = SetFPos( fileRefNum, fsFromStart, sourceOffset + bytesCopied );
			if ( error == noErr ) {
			
				error = FSRead( fileRefNum, &copyCount, *bufferHandle );
				if ( error == noErr ) {
				
					error = SetFPos( fileRefNum, fsFromStart, destinationOffset + bytesCopied );
					if ( error == noErr ) {
					
						error = FSWrite( fileRefNum, &copyCount, *bufferHandle );
					
					}
				
				}
			
			}
			
			bytesCopied += copyCount;
			bytesLeftToCopy -= copyCount;
	
		}
		
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = CompressDiskCacheFile( diskCacheMapHandle )
		
	Purpose:	Total deep shit routine. This routine gets called when the font cache file
				needs to be compressed in order to satisfy an allocation request.
				
				After allocating a locked compression buffer, the first step is to make the
				first cache adjacent with the disk cache map. This frees up any fragmented
				space between the disk cache map and the first cache that may have occured
				through expanding or purging caches.
				
				After this step the routine cycles through each cache in file offset order
				checking for fragmented space between the end of the cache and the next
				cache. If there is free space the next cache is moved down so that it is
				adjacent to the current cache and the process is repeated.
				
				We mark the map dirty before we begin, then clear it when done compressing,
				forcing the map to be consistent by writing it out.
				
	Warnings:	Allocating memory at this stage is tricky - see the AllocateLockedBufferForCompression
				routine for more information.
	
	Assumes:	diskCacheMapHandle is locked
	-----------------------------------------------------------------------------------  */


OSErr CompressDiskCacheFile( DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		OSErr									error;
		Integer									fileRefNum;
		Size									bufferSize;
		DiskCacheOffset							endOfPreviousCache;
		Handle									bufferHandle;
		THz										bufferZone;
		DiskCacheEntryStructure					*cachePointer, *previousCachePointer;
		
	if ( (*diskCacheMapHandle)->fDiskCacheCount > 0 ) {//at least one block exists
	
		error = MarkMapDirty( kMapDirty, diskCacheMapHandle );
		
		error = AllocateLockedBufferForCompression( &bufferSize, &bufferHandle, &bufferZone );
		if ( error == noErr ) {//got the buffer, so let’s move blocks around
		
			fileRefNum = (*diskCacheMapHandle)->fFileRefNum;			

			//slide the first block down to be adjacent to the map
			cachePointer = &(*diskCacheMapHandle)->fDiskCache[(*diskCacheMapHandle)->fFirstOffsetIndex];
			error = MoveFileDataDown( fileRefNum, cachePointer->fLogicalOffset, 
									  (*diskCacheMapHandle)->fDiskCacheMapSize, cachePointer->fLogicalSize, 
									  bufferSize, bufferHandle );
			if ( error == noErr ) {//move the rest of the blocks
			
				//update the first block’s base address
				cachePointer->fLogicalOffset = (*diskCacheMapHandle)->fDiskCacheMapSize;
				
				while ( (error == noErr) && (cachePointer->fNextOffsetIndex != kDiskCacheNotFound) ) {
						//no problems    and   not at the end of the list				
					previousCachePointer = cachePointer;
					endOfPreviousCache = previousCachePointer->fLogicalOffset 
									   + previousCachePointer->fLogicalSize;
					
					cachePointer = &(*diskCacheMapHandle)->fDiskCache[previousCachePointer->fNextOffsetIndex];
					if ( cachePointer->fLogicalOffset > endOfPreviousCache ) {
					
						error = MoveFileDataDown( fileRefNum, cachePointer->fLogicalOffset,
												  endOfPreviousCache, cachePointer->fLogicalSize, 
												  bufferSize, bufferHandle );					
						cachePointer->fLogicalOffset = endOfPreviousCache;
					
					}
				}
			}
			DisposeOfCompressionBuffer( bufferHandle, bufferZone );
			error = MarkMapDirty( !kMapDirty, diskCacheMapHandle );
		}
	}
	return( error );
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = MakeSpaceInDiskCache( &cacheIndex, &logicalOffset, sizeRequested, 
											  diskCacheMapHandle )
		
	Purpose:	This routine is the heart of the disk based cache allocator. It obtains a
				contiguous block of space by:
		
					1) finding expansion space between caches.
					
					2) compressing the font cache file to obtain a contiguous block
					   starting from the end of the last cache.
					   
					3) purging caches starting from the least used caches and then
					   compressing the font cache file to obtain a contiguous block
					   starting from the end of the last cache.
				
				The cacheIndex parameter is assigned an index that a new cache should be
				inserted after.
				
				The logicalOffset parameter is assigned the font cache file offset of
				where the new cache data should be written.
				
				Purging is a last resort; this routine will attempt to compress the
				file before having to purge cache data. If a purge does become necessary
				the amount of inter-cache compression space is factored into the purge
				size allowing fragmentation to be considered before purging unnecessarily.
				
				The font cache file is first searched to see if space between a
				cache is large enough to fulfill the sizeRequested request. If
				there is a large enough expansion space cacheIndex is set to the cacheIndex
				being inspected and logicalOffset is set to the end of cache. The amount
				of expansion space is accumulated during this process to determine
				requested contiguous block size through compression.
				
				If a large enough contiguous space cannot be found between caches the
				routine next checks to see if a contiguous block can be found starting
				from the end of the last cache and ending at the end of file. If enough
				space is found the cacheIndex parameter is set to the last cache index,
				signaling that the cache should be inserted after the lastmost cache,
				and the logicalOffsetPointer is set to the end of the last cache.
				
				If space still cannot be found the routine looks at the amount of space
				available from the end of the last cache to the end of file and decides
				if compressing the file will obtain the requested contiguous block. If
				it will, the file is compressed forming a large contiguous block starting
				from the end of the last cache and ending at the end of file. Parameters
				are then set as in the previous case.
				
				If compressing won't obtain the requested space the routine determines
				the number of bytes that will have to be purged after compressing to
				fulfill the requested block, purges that many bytes, and compresses the
				file to obtain a contiguous block starting from the end of the last
				cache and ending at the end of file. Parameters are then set as in the
				previous case.
				
				After purging it is necessary to recalculate the last cache index from
				the disk cache map handle since purging may have removed what was
				previously the last most cache.
				
				After compressing it is necessary to recalculate the logicalOffset pointer
				with GetCacheExpansionInformation since compressing may physically move the
				last cache.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr MakeSpaceInDiskCache( DiskCacheIndex* cacheIndexPointer, 
									DiskCacheOffset* logicalOffsetPointer, 
									DiskCacheOffset sizeRequested, 
									DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals

		OSErr									error;
		DiskCacheIndex							cacheIndex;
		DiskCacheOffset							totalFreeSpaceBeforePurging, expansionSize,
												compressionSize, nextCacheOffset;
		DiskCacheEntryStructurePointer			cacheEntryPointer;
		
	error = noErr;
	compressionSize = 0;
	*cacheIndexPointer = kDiskCacheNotFound;
	
	if ( (*diskCacheMapHandle)->fDiskCacheCount > 0 ) {//there are blocks in the file
	
		cacheIndex = (*diskCacheMapHandle)->fFirstOffsetIndex;
		cacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
		while ( (*cacheIndexPointer == kDiskCacheNotFound) 							//haven’t found a big enough hole yet
				&& (cacheEntryPointer->fNextOffsetIndex != kDiskCacheNotFound) )	//not at end of list
		{
		
			GetCacheExpansionInformation( &expansionSize, logicalOffsetPointer, &nextCacheOffset, cacheIndex, 
										  diskCacheMapHandle );
			if ( sizeRequested <= expansionSize )	//there’s room, so we’ve found it and we’re done
			{
				*cacheIndexPointer = cacheEntryPointer->fNextOffsetIndex;
			
			} else {
			
				compressionSize += expansionSize;
				cacheIndex = cacheEntryPointer->fNextOffsetIndex;
				cacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];//next, please
			}
		}
	}
	
	if ( *cacheIndexPointer == kDiskCacheNotFound ) {	//didn’t find a suitable hole,
														//but is there room at the end?
		//cacheIndex==kDiskCacheNotFound
		GetCacheExpansionInformation( &expansionSize, logicalOffsetPointer, &nextCacheOffset, 
									  cacheIndex, diskCacheMapHandle );			
		if ( sizeRequested > expansionSize ) {
		
			totalFreeSpaceBeforePurging = expansionSize + compressionSize;
			if ( sizeRequested <= totalFreeSpaceBeforePurging ) { //not enough room at eof
			
				error = CompressDiskCacheFile( diskCacheMapHandle );
			
			} else {
			
				error = PurgeDiskCaches( sizeRequested - totalFreeSpaceBeforePurging, diskCacheMapHandle );
				if ( error == noErr )
				{
					error = CompressDiskCacheFile( diskCacheMapHandle );
				}
			}
			//reset logicalOffsetPointer, which may have been changed by purging or compressing
			GetCacheExpansionInformation( &expansionSize, logicalOffsetPointer, &nextCacheOffset, (*diskCacheMapHandle)->fLastOffsetIndex, diskCacheMapHandle );			
		}
	}
	
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = WriteDiskCache( cacheIndex, cacheHandle, diskCacheMapHandle )
		
	Purpose:	Given a cacheIndex this routine writes the TrueType cache identified by
				cacheHandle to the disk using information stored in the diskCacheMapHandle.
				
				After writing the cache the routine then dumps the disk cache map to
				ensure the cache data's integrity in the event of a power failure or
				spurious interruption.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr WriteDiskCache( DiskCacheIndex cacheIndex, sb_CacheHeadHdl cacheHandle, 
					  DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals

		OSErr									error;
		Integer									fileRefNum;
		DiskCacheOffset							logicalOffset, logicalSize;
		DiskCacheEntryStructurePointer			cacheEntryPointer;
		
	error = noErr;
	
	cacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
	logicalOffset = cacheEntryPointer->fLogicalOffset;
	logicalSize = cacheEntryPointer->fLogicalSize;
	
	fileRefNum = (*diskCacheMapHandle)->fFileRefNum;
	error = SetFPos( fileRefNum, fsFromStart, logicalOffset );
	if ( error == noErr ) {
	
		error = FSWrite( fileRefNum, &logicalSize, (Ptr) *cacheHandle );
		if ( error == noErr ) {
		
			error = DumpDiskCacheMap( diskCacheMapHandle );
		
		}
		
	}

	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = AddDiskCache( &cacheIndex, cacheHandle, diskCacheMapHandle )
		
	Purpose:	Find space in the font cache file for the TrueType cache specified by
				cacheHandle, initialize a new disk cache entry, insert the disk cache
				into the date and offset lists, and write it's data to disk.
				
	Warnings:	The cacheHandle must be marked as non-purgeable during the entry allocation.
	
				Nil and purged cacheHandle parameters are frowned upon.
	
	-----------------------------------------------------------------------------------  */


OSErr AddDiskCache( DiskCacheIndex* cacheIndexPointer, sb_CacheHeadHdl cacheHandle,
					DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		SignedByte								cacheHandleState;
		OSErr									error;
		Integer									fontNameIndex;
		DiskCacheIndex							insertionIndex;
		DiskCacheOffset							logicalOffset, logicalSize;
		sb_CacheHeadPtr							cachePointer;
		DiskCacheEntryStructure					diskCacheEntry;
		Str255									fontName;
		
	error = noErr;
	*cacheIndexPointer = kDiskCacheNotFound;
	
	if ( cacheHandle == nil ) {
	
		error = memFullErr;
		
	} else if ( StripAddress( (Ptr) *cacheHandle ) == nil ) {
	
		error = nilHandleErr;
	
	} else {
	
		cacheHandleState = HGetState( (Handle) cacheHandle );
		HNoPurge( (Handle) cacheHandle );
		
		logicalSize = GetHandleSize( (Handle) cacheHandle );
		error = MakeSpaceInDiskCache( &insertionIndex, &logicalOffset, logicalSize, diskCacheMapHandle );
		if ( error == noErr ) {
				
			GetFontName( (*cacheHandle)->familyID, fontName );
			error = FontNameToFontNameIndex( &fontNameIndex, fontName, diskCacheMapHandle );
			if ( error == noErr ) {
			
				cachePointer = *cacheHandle;
				diskCacheEntry.fFontNameIndex = fontNameIndex;
				diskCacheEntry.fPointSize = cachePointer->ptSize;
				diskCacheEntry.fStyle = cachePointer->face;
				diskCacheEntry.fHorizontalScale = cachePointer->trans00;
				diskCacheEntry.fVerticalScale = cachePointer->trans11;
				diskCacheEntry.fSkewTransform = cachePointer->trans10;
				diskCacheEntry.fSquashed = cachePointer->squashed;
				diskCacheEntry.fLogicalOffset = logicalOffset;
				diskCacheEntry.fLogicalSize = logicalSize;
				diskCacheEntry.fCacheIsLocked = false;
				diskCacheEntry.fCacheIsMaximumSize = (logicalSize >= kMaximumDiskCacheSize);
				diskCacheEntry.fCacheIsFree = false;
				
				error = InsertDiskCache( cacheIndexPointer, insertionIndex, diskCacheEntry, diskCacheMapHandle );
				if ( error == noErr ) {
				
					//write the block and the map
					error = WriteDiskCache( *cacheIndexPointer, cacheHandle, diskCacheMapHandle );
				
				}
			
			}
			
		}
		
		HSetState( (Handle) cacheHandle, cacheHandleState );
	
	}
	
	return( error );
	
}


