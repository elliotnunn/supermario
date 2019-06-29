/*
	File:		PartialFontExtensions.c

	Contains:	Partial font extension entry points for the scaler and cache code to
				read and cache outline font data from the disk.

				Three main structures are necessary for the partial font code:
				
					Fragment
					FragmentTable
					FragmentTableCache
					
				A fragment is a handle that contains a small header followed by a range
				of data read from an outline font resource. The header identifies where
				the data started in the outline font, it's length, and other attributes.
				
				A fragment table is a doubly linked list of fragments for one outline
				font.
				
				A fragment table cache is a doubly linked list of fragment tables for
				the entire system.
				
				The two most important routines in this file are QDGetFontFrag and
				QDUngetFontFrag. These entry points are used as the GetSfntFragmentPtr
				and ReleaseSfntFrag fields of an input parameter block used to
				communicate with the TrueType scaler.
				
	Written by:	John Farmer based on work originally written by Mike Reed

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<SM4>	  7/7/92	CSS		Roll-in Reality Changes:
				 					<7> 7/4/92 DTY 	#1034875 <csd>: Allow InitializePartialFonts to be called more
													than once.  On successive calls, if the requested zone size is
													larger than the existing zone size, blow the smaller one away.
													If the requested size is smaller than the existing one, keep the
													existing one.
		<SM3>	  6/8/92	CSS		Roll-in Reality Changes:
									<6> 4/2/92 DTY 	#1025909: Commented out calls to VerifyPartialFontStructures and
													the routine itself since it just returns.  This will save a few
													bytes of our ever precious memory.
									<5> 4/1/92 YK	#1025684: Calculate the number of master pointers initially 
													created in the partial font zone, instead of hard-codeing the 
													value.
									<4> 4/1/92 YK 	#1023997: InitializePartialFonts should be a pascal function.
	   		 						<3> 3/30/92	DTY	#1025684: Make InitializePartialFonts take a Size parameter so
													that the caller can specify how large to make the partial font
													sub-zone.
		 							<2> 3/27/92	DTY	#1024868: Take out calls to assertion routines since they don’t
													do anything anymore, and the assertion messages are wasting
													memory.
		<SM2>	 4/17/92	PN		Change InitZone to InitZoneGlue to do ROM build with Bass fonts

		System 6.1.0 Changes:
		
		<3>		08/27/91	John Farmer - Added documentation.
		
		<2>		06/03/91	John Farmer - Dispose of at least kMinimumFreeHeapSpace in
							the GrowZone procedure.
							
		<1>		03/23/91	John Farmer - Created file and documentation.
														
*/


// Include Statements


	#include	"BaseIncludes.h"
	#include	"Assertion.h"
	#include	"Fragment.proto"
	#include	"FragmentTable.proto"
	#include	"FSCdefs.h"	
	#include	"MapString.h"
	#include	"Memory.h"
	#include	"PartialFontExtensions.proto"
	#include	"sfnt.h"
	#include	"sfnt_enum.h"
	#include	"ZoneHandle.proto"
	
	#include	"FontScaler.h"
	#include	"Bass_Cache.h"

//••••• Temporary hack for INitZone
extern pascal void INITZONEGLUE (GrowZoneProcPtr pgrowZone,short cmoreMasters,void *limitPtr,
 void *startPtr);

/*	--------------------------------------------------------------------------------------

	Routine:	partialFontZone = GetPartialFontZone( )
		
	Purpose:	Return pointer to partial font zone from TrueType globals.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


THz GetPartialFontZone( void )

{

	// Locals
	
		sb_SplineKeyHdl							splineKey = SPLINE_KEY_HAND;
		
	return( (*splineKey)->fPartialFontZone );

}


/*	--------------------------------------------------------------------------------------

	Routine:	fragmentTableCache = GetGlobalFontFragmentTableCache( )
		
	Purpose:	Return handle to fragment table cache.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


FragmentTableCacheStructureHandle GetGlobalFontFragmentTableCache( void )

{

	// Locals
	
		sb_SplineKeyHdl							splineKey = SPLINE_KEY_HAND;
		
	return( (*splineKey)->fFragmentTableCache );

}


/*	--------------------------------------------------------------------------------------

	Routine:	VerifyPartialFontStructures
		
	Purpose:	Inspect partial font data structures for bogus values. Remove the return
				statement at the top of the routine to enable verification.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


#if 0											// <SM3> CSS <6>
void VerifyPartialFontStructures( void )

{

	// Locals
	
		Boolean									valid;
		OSErr									error;
		Size									fragmentSize;
		FragmentStructureHandle					fragment;
		FragmentTableStructureHandle			fragmentTable;
		FragmentTableCacheStructureHandle		fragmentTableCache;
		THz										partialFontZone, savedZone;
		
	return;
	
	error = noErr;
	
	partialFontZone = GetPartialFontZone( );
	if ( partialFontZone ) {
	
		savedZone = SwapZone( GetPartialFontZone( ) );
		
		fragmentTableCache = GetGlobalFontFragmentTableCache( );
		if ( AssertHandle( (Handle) fragmentTableCache ) ) {
		
			valid = true;
			fragmentTable = (*fragmentTableCache)->fFirstFragmentTable;
			while ( valid && (fragmentTable != nil) ) {
			
				if ( valid = (AssertHandle( (Handle) fragmentTable ) && AssertHandle( (*fragmentTable)->fFontHandle )) ) {
				
					fragment = (*fragmentTable)->fFirstFragment;
					while ( valid && (fragment != nil) ) {
					
						if ( valid = AssertHandle( (Handle) fragment ) ) {
						
							fragmentSize = GetHandleSize( (Handle) fragment );
							if ( valid = ((error = MemError( )) == noErr) ) {
							
								if ( valid = (fragmentSize == sizeof( FragmentStructure ) + (*fragment)->fLength) ) {
								
									if ( valid = ( (*fragment)->fLockCount >= 0) ) {
									
										if ( valid = ( (*fragment)->fOffset >= 0) ) {
										
											valid = ( (*fragment)->fLength >= 0);
										
										}
									
									}
								
								}
							
							}
						
						}
					
						fragment = (*fragment)->fNextFragment;
						
					}
					
				}
			
				fragmentTable = (*fragmentTable)->fNextFragmentTable;
				
			}
								
		}
		
		SwapZone( savedZone );
		
	}

}
#endif												// <SM3> CSS <6>

/*	--------------------------------------------------------------------------------------

	Routine:	PartialFontZoneGrowZoneProcedure( )
		
	Purpose:	Throw away the least recently used fragments until memoryNeeded bytes is
				obtained. MemoryNeeded is pinned to at least kMinimumFreeHeapSize to limit
				Memory Manager heap compaction and frequent visits to this routine. Only
				unlocked fragments are thrown away.
				
	Warnings:	If this routine ever fails it means that some very large fragment requests
				are being made by the scaler. The best thing to do is reduce the size of
				the request or increase the size of the partial font heap.
	
	-----------------------------------------------------------------------------------  */


pascal Size PartialFontZoneGrowZoneProcedure( Size memoryNeeded )

{

	// Locals

		OSErr									error;
		Size									memoryDeallocated, fragmentSize;
		THz										savedZone;
		Handle									handleFromHell;
		FragmentStructureHandle					fragment, previousFragment;
		FragmentTableStructureHandle			fragmentTable;
		FragmentTableCacheStructureHandle		fragmentTableCache;
	
//	VerifyPartialFontStructures( );

	error = noErr;
	memoryDeallocated = 0;
	fragmentTableCache = GetGlobalFontFragmentTableCache( );
	
	handleFromHell = GZSaveHnd( );
	savedZone = SwapZone( GetPartialFontZone( ) );
	
	if ( memoryNeeded < kMinimumFreeHeapSpace )
		memoryNeeded = kMinimumFreeHeapSpace;
		
	fragmentTable = (*fragmentTableCache)->fLastFragmentTable;
	while ( (fragmentTable != nil) && (memoryDeallocated < memoryNeeded) ) {
	
		fragment = (*fragmentTable)->fLastFragment;
		while ( (error == noErr) && (fragment != nil) && (memoryDeallocated < memoryNeeded) ) {
		
			previousFragment = (*fragment)->fPreviousFragment;
			if ( ((Handle) fragment != handleFromHell) && ((*fragment)->fLockCount == 0) ) {
				
				RemoveFragmentFromFragmentTable( fragment, fragmentTable );
				fragmentSize = GetHandleSize( (Handle) fragment );
				if ( (error = MemError( )) == noErr ) {
				
					DisposHandle( (Handle) fragment );
					memoryDeallocated += fragmentSize;
				
				}
				
			}
		
			fragment = previousFragment;
		
		}
		
		fragmentTable = (*fragmentTable)->fPreviousFragmentTable;
		
	}
	
	SwapZone( savedZone );
	
//	VerifyPartialFontStructures( );
	return( memoryDeallocated );
	
}



/*	--------------------------------------------------------------------------------------

	Routine:	InitializePartialFonts( Size partialFontZoneSize )
		
	Purpose:	Initialize the partial font data structures. Mainly responsible for
				embedding the partial font heap within the system heap, creating an
				empty fragment table cache within the partial font heap, and initializing
				key data structures.
				
	Warnings:	This routine should be called as earily as possible in the boot process
				to ensure that we do not fragment the system heap.
	
	-----------------------------------------------------------------------------------  */


pascal OSErr InitializePartialFonts( Size partialFontZoneSize )

{


	// Locals
	
		OSErr									error = noErr;
		THz										savedZone;
		sb_SplineKeyHdl							splineKey = SPLINE_KEY_HAND;
		Ptr										partialFontZone = (Ptr) ((*splineKey)->fPartialFontZone);
		Size									existingPartialFontZoneSize;
		FragmentTableCacheStructureHandle		fragmentTableCache;
		
	// <7> Check to see if a partial font zone already exists.  If one does, and it’s larger
	//		than the size that is being requested, return immediately and keep using the old
	//		one.  If the existing zone is smaller than the requested size, blow it away
	//		and create a new one.
	
	if ( partialFontZone != nil ) {
		existingPartialFontZoneSize = GetPtrSize(partialFontZone);				// <7> Get the size of the existing partial font zone
		if (existingPartialFontZoneSize >= partialFontZoneSize)					// <7> Is the existing one larger than the requested one?
			return (noErr);														// <7> Yeah.  Go ahead and use what’s there

		DisposPtr(partialFontZone);												// <7> Otherwise, blow it away.
		(*splineKey)->fPartialFontZone = nil;									// <7> Remove the reference to the old zone in case the new one fails to allocate
		(*splineKey)->fFragmentTableCache = nil;								// <7> We blew away the fragment table cache too.
	}
	
	savedZone = GetZone( );

	error = NewZonePtr( &partialFontZone, partialFontZoneSize, SystemZone( ) );
	if ( error == noErr ) {
//••••Temporary hack
			INITZONEGLUE( (GrowZoneProcPtr) PartialFontZoneGrowZoneProcedure, partialFontZoneSize/kPartialFontBytesPerMasterPointer, partialFontZone + partialFontZoneSize, partialFontZone ); //<SM3> <SM4>
			if ( (error = MemError( )) == noErr ) {
			
				error = NewZoneHandle( (Handle*) &fragmentTableCache, (Size) sizeof( FragmentTableCacheStructure ), (THz) partialFontZone );
				if ( error == noErr ) {
				
					(*fragmentTableCache)->fFirstFragmentTable = nil;
					(*fragmentTableCache)->fLastFragmentTable = nil;
					(*splineKey)->fPartialFontZone = (THz) partialFontZone;
					(*splineKey)->fFragmentTableCache = fragmentTableCache;
					
				}
				
			}
		
		}
		
	SetZone( savedZone );
	
			
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	QDGetFontFrag
		
	Purpose:	Return a pointer to font data starting at 'offset' bytes into a 'sfnt'
				resource. The data is 'length' bytes in size and the data pointer must remain
				valid until a corresponding QDUngetFontFrag call is made.
				
				The splineFont is assumed to be loaded into memory if the master pointer of
				the splineFont handle is not nil. In this case, we act like the Roman system
				and add the offset parameter to the splineFont master pointer.
				
				If the master pointer is nil, we try to lookup a fragmentTable for the font
				identified by the 'splineFont' parameter. If we find a fragmentTable it
				becomes the current fragmentTable and we attempt to find or create a fragment
				within the fragmentTable.
				
				If we do not find a fragmentTable we have to create a new one, add it to
				the fragmentTableCache (which is just a list of fragmentTables in MRU order),
				and call the FindOrCreateFragment routine, which, in this case, simply
				creates the fragment and adds it to the list of fragments in the newly
				created fragmentTable.
				
				Once a fragment has been found or created we can return a pointer to the
				data. FindOrCreateFragment is responsible for returning a locked fragment
				handle so we're guaranteed that the data won't move. The returned fragment
				may have a fragment offset less than or equal to the requested fragment
				offset. Therefore, we must offset the data pointer by the different between
				the requested offset and fragment's offset (a picture would be great here).
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void* QDGetFontFrag( Handle splineFont, long offset, long length )

{

	// Locals

		OSErr										error;
		void*										dataPointer;
		Ptr											splineMasterPointer;
		FragmentStructureHandle						fragment;
		FragmentTableStructureHandle				fragmentTable;
		FragmentTableCacheStructureHandle			fragmentTableCache;
		
//	VerifyPartialFontStructures( );

	if ( (splineMasterPointer = StripAddress( *splineFont )) != nil ) {
	
		dataPointer = splineMasterPointer + offset;
	
	} else {
	
		error = noErr;
		dataPointer = nil;
		fragment = nil;
		fragmentTableCache = GetGlobalFontFragmentTableCache( );
	
		if ( AssertHandle( splineFont ) && AssertHandle((Handle) fragmentTableCache) && (offset >= 0) && (length >= 0) ) {
		
			FindFragmentTable( &fragmentTable, splineFont, fragmentTableCache );
			if ( fragmentTable != nil ) {
			
				SetCurrentFragmentTable( fragmentTable, fragmentTableCache );
				error = FindOrCreateFragment( &fragment, offset, length, fragmentTable );
				
			} else {
	
				error = CreateFragmentTable( &fragmentTable, splineFont );
				if ( error == noErr ) {
				
					AddFragmentTableToCache( fragmentTable, (*fragmentTableCache)->fFirstFragmentTable, fragmentTableCache );
					error = FindOrCreateFragment( &fragment, offset, length, fragmentTable );
					
				}
	
			}
				
		}
	
		if ((error == noErr) && (fragment != nil))
			dataPointer = StripAddress( (*fragment)->fData + (offset - (*fragment)->fOffset) );
	
	}
	
//	VerifyPartialFontStructures( );
	return( dataPointer );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	QDUngetFontFrag
		
	Purpose:	Unlock a fragment that has been locked through a QDGetFontFrag call.
				After finding the fragmentTable associated with the splineFont parameter
				we have to search through the first few entries of the fragment list
				for the fragmentTable. This is necessary since QDGetFontFrag may return
				a pointer to the middle of a locked handle, which, does no allow us to
				call recover handle. The fragment being released is almost always the
				current fragment in the fragment list since the scaler nests QDGetFontFrag
				and QDUngetFontFrag pairs.
				
				The data parameter is the pointer returned by QDGetFontFrag.
				
	Warnings:	None.
	
	-----------------------------------------------------------------------------------  */


void QDUngetFontFrag( Handle splineFont, void* data )

{

	// Locals
	
		FragmentStructureHandle						fragment;
		FragmentTableStructureHandle				fragmentTable;
		FragmentTableCacheStructureHandle			fragmentTableCache;

//	VerifyPartialFontStructures( );

	if ( (data != nil) && (StripAddress( *splineFont ) == nil) && AssertHandle( splineFont) ) {
		
		fragmentTableCache = GetGlobalFontFragmentTableCache( );
		FindFragmentTable( &fragmentTable, splineFont, fragmentTableCache );
		if ( fragmentTable != nil ) {
		
			FindFragmentHandle( &fragment, (Ptr) data, fragmentTable );
			if ( fragment != nil ) {
			
				UnlockFragment( fragment, fragmentTable );
				
			}
			
		}
	
	}
	
//	VerifyPartialFontStructures( );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	KillApplicationHeapFragmentTableCaches( startAddress, endAddress )
		
	Purpose:	Search through all fragmentTables in the fragmentTable cache. If a fragmentTable
				is associated with a spline font handle contained within startAddress..endAddress,
				remove the fragmentTable from the fragmentTableCache, remove and dispose of
				each fragment contained within the fragmentTable (ouch), and dispose of the
				fragmentTable.
				
				This routine is normally called when an application terminates. If an
				outline font resource has been loaded into the application heap all
				fragments associated with that font will be disposed when the application
				quits. Pacific outline fonts will not have this problem since they are always
				allocated in the system heap as empty handles.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void KillApplicationHeapFragmentTableCaches( Handle heapStart, Handle heapEnd )

{

	// Locals
	
		OSErr									error;
		FragmentStructureHandle					fragment, nextFragment;
		FragmentTableStructureHandle			fragmentTable, nextFragmentTable;
		FragmentTableCacheStructureHandle		fragmentTableCache;
		THz										partialFontZone;
		
	error = noErr;
	
	fragmentTableCache = GetGlobalFontFragmentTableCache( );
	if ( fragmentTableCache != nil ) {
	
		partialFontZone = GetPartialFontZone( );
		
		fragmentTable = (*fragmentTableCache)->fFirstFragmentTable;
		while ( fragmentTable != nil ) {
		
			nextFragmentTable = (*fragmentTable)->fNextFragmentTable;
			
			if ( ((*fragmentTable)->fFontHandle >= heapStart) && ((*fragmentTable)->fFontHandle < heapEnd) ) {
			
				RemoveFragmentTableFromCache( fragmentTable, fragmentTableCache );
				
				fragment = (*fragmentTable)->fFirstFragment;
				while ( fragment != nil ) {
				
					nextFragment = (*fragment)->fNextFragment;
					
					RemoveFragmentFromFragmentTable( fragment, fragmentTable );
					DisposeZoneHandle( (Handle) fragment, partialFontZone );
					
					fragment = nextFragment;
				
				}
				
				DisposeZoneHandle( (Handle) fragmentTable, partialFontZone );
			
			}
			
			fragmentTable = nextFragmentTable;
			
		}
	
	}
	
}
	
	
/*	--------------------------------------------------------------------------------------

	Routine:	PackStringClass( platformID, scriptID, languageID )
		
	Purpose:	Pack platformID, scriptID, and languageID into a four byte integer.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


long PackStringClass( short platformID, short scriptID, short languageID )

{

	return( ~((platformID << 16L) | (scriptID << 8L) | languageID) );

}


/*	--------------------------------------------------------------------------------------

	Routine:	UnpackStringClass( class, &platformID, &scriptID, &languageID )
		
	Purpose:	Extract a platformID, scriptID, and languageID from a four byte integer.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void UnpackStringClass( long class, short* platformIDPointer, short *scriptIDPointer, short *languageIDPointer )

{

	class = ~class;
	if ( languageIDPointer != nil )
		*languageIDPointer = class & 0xff;
		
	class >>= 8;
	if ( scriptIDPointer != nil )
		*scriptIDPointer = class & 0xff;
		
	class >>= 8;
	if ( platformIDPointer != nil )
		*platformIDPointer = class & 0xff;

}


/*	--------------------------------------------------------------------------------------

	Routine:	LookupSplineTagTableIndex( splineTag, splineDirectoryPointer )
		
	Purpose:	Try to find a directory entry with a tag field equal to the splineTag
				parameter. If found, return the directory table index. Otherwise,
				return zero.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


long LookupSplineTagTableIndex( long splineTag, sfnt_OffsetTable* splineDirectoryPointer )

{

	// Locals
	
		Boolean										found;
		long										tableIndex, tableCount;
		sfnt_DirectoryEntry*						tablePointer;
		
	found = false;	
	tableIndex = 1;
	tablePointer = splineDirectoryPointer->table;
	tableCount = splineDirectoryPointer->numOffsets;
	
	while ( (found == false) && (tableIndex <= tableCount) ) {
	
		if ( tablePointer->tag == splineTag ) {
		
			found = true;
		
		} else {
		
			tableIndex++;
			tablePointer++;
	
		}
		
	}
	
	return( found ? tableIndex : 0 );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	GetSplineDirectoryFragmentPointer
		
	Purpose:	Return a pointer to the splineDirectory for the outline font resource
				specified by the splineFont parameter. Two reads are necessary to
				load the spline directory. The first read loads the directory header.
				The second read loads the entire directory using the numOffsets field
				of the spline directory to determine the directory size.
				
	Warnings:	QDUngetFontFrag must called using the splineFont parameter and this
				function's result as it's parameters. Not doing so will result in handles
				that can't be purged from the partialFontZone and will eventually lead
				to an out of memory error.
	
	-----------------------------------------------------------------------------------  */


void* GetSplineDirectoryFragmentPointer( Handle splineFont )

{

	// Locals
	
		Size									splineDirectorySize;
		sfnt_OffsetTable*						splineDirectoryPointer;
		
	splineDirectoryPointer = (sfnt_OffsetTable*) QDGetFontFrag( splineFont, 0, OFFSETTABLESIZE );
	if ( splineDirectoryPointer != nil ) {
	
		splineDirectorySize = OFFSETTABLESIZE + (splineDirectoryPointer->numOffsets * sizeof(sfnt_DirectoryEntry));
		QDUngetFontFrag( splineFont, splineDirectoryPointer );
		
		splineDirectoryPointer = (sfnt_OffsetTable*) QDGetFontFrag( splineFont, 0, splineDirectorySize );
	}
	
	return( splineDirectoryPointer );

}


/*	--------------------------------------------------------------------------------------

	Routine:	PartialGetFontTablePtr
		
	Purpose:	Return a pointer to a piece of data within a table. The splineFont parameter
				identifies the outline font resource. The offset parameter is relative to
				the start of the table and specifies where to read the data from. Length
				specifies how many bytes to read. If length is -1 a pointer to the entire
				table is returned.
				
				Either tableIndex or splineTag can be used to specify the table to read
				from. If tableIndex is zero, the splineTag parameter is used to lookup
				the directory tableIndex. Otherwise, tableIndex is used directly.
				
	Warnings:	QDUngetFontFrag must called using the splineFont parameter and this
				function's result as it's parameters. Not doing so will result in handles
				that can't be purged from the partialFontZone and will eventually lead
				to an out of memory error.
	
	-----------------------------------------------------------------------------------  */


void* PartialGetFontTablePtr( Handle splineFont, long offset, long length, long tableIndex, long splineTag )

{

	// Locals
	
		void*									dataPointer;
		long									readOffset, readLength;
		sfnt_OffsetTable*						splineDirectoryPointer;
		sfnt_DirectoryEntry*					splineTableEntryPointer;
		
	dataPointer = nil;
	
	splineDirectoryPointer = (sfnt_OffsetTable*) GetSplineDirectoryFragmentPointer( splineFont );
	if ( splineDirectoryPointer != nil ) {
		
		if ( tableIndex == 0 )
			tableIndex = LookupSplineTagTableIndex( splineTag, splineDirectoryPointer );

		if ( tableIndex != 0 ) {
		
			splineTableEntryPointer = &splineDirectoryPointer->table[tableIndex - 1];
			readOffset = splineTableEntryPointer->offset + offset;
			readLength = (length == -1) ? splineTableEntryPointer->length : length;		
			dataPointer = QDGetFontFrag( splineFont, readOffset, readLength );
	
		}
		
		QDUngetFontFrag( splineFont, splineDirectoryPointer );

	}
	
	return( dataPointer );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	GetFontTablePtr
		
	Purpose:	Bozo routine to load an entire table into memory.
				
	Warnings:	QDUngetFontFrag must called using the splineFont parameter and this
				function's result as it's parameters. Not doing so will result in handles
				that can't be purged from the partialFontZone and will eventually lead
				to an out of memory error.
	
	-----------------------------------------------------------------------------------  */


void* GetFontTablePtr( Handle splineFont, long tableIndex, long splineTag )

{

	return( PartialGetFontTablePtr( splineFont, 0, -1, tableIndex, splineTag ) );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	GetFontMapInfo( splineFont, index, &glyphs[0], count, &stream[0], &streamLength )
		
	Purpose:	Function used by MacExtra.c
	
				Return bizarre function result. Two different function results are returned
				based on the platformIndex parameter. If platformIndex is non-zero a packed
				string class is returned by looking up the platformID and specificID from
				the cmap's platformArray. If platformIndex is zero the routine attempts to
				find a platform entry with a platformID and specificID equal to the platformID
				and specificID values contained in the class parameter. If a match is found
				the platformIndex is returned. Otherwise zero is returned.
				
				Function Result Summary:
				
					-1 if an error occured
					zero or positive integer if platformIndex parameter is zero
					packed string class if platformIndex is non-zero
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


long GetFontMapInfo( Handle splineFont, long platformIndex, long class )

{

	// Locals
	
		short									platformCount, platformID, scriptID;
		long									platformIndexOrClass;
		sfnt_char2IndexDirectory*				char2IndexTablePointer;
		sfnt_platformEntry*						platformPointer;
		
	platformIndexOrClass = -1;
	
	char2IndexTablePointer = (sfnt_char2IndexDirectory*) GetFontTablePtr( splineFont, 0, tag_CharToIndexMap );
	if ( char2IndexTablePointer != nil) {
	
		if ( platformIndex != 0 ) {
		
			platformPointer = char2IndexTablePointer->platform + platformIndex - 1;
			platformIndexOrClass = PackStringClass( platformPointer->platformID, platformPointer->specificID, 0 );
		
		} else {
		
			platformIndexOrClass = 0;
			platformCount = char2IndexTablePointer->numTables;
			platformPointer = char2IndexTablePointer->platform;
			UnpackStringClass( class, &platformID, &scriptID, 0 );
			
			platformIndex = 1;
			while ( (platformIndexOrClass == 0) && (platformIndex <= platformCount) ) {
			
				if ( (platformPointer->platformID == platformID) && (platformPointer->specificID == scriptID) ) {
				
					platformIndexOrClass = platformIndex;
				
				} else {
				
					platformIndex++;
					platformPointer++;
				
				}
			
			}
		
		}
	
		QDUngetFontFrag( splineFont, char2IndexTablePointer );
	
	}
	
	return( platformIndexOrClass );

}


/*	--------------------------------------------------------------------------------------

	Routine:	ApplyFontMap( splineFont, index, &glyphs[0], count, &stream[0], &streamLength )
		
	Purpose:	Function used by MacExtra.c.
	
				Call the appropriate cmap string mapping function based on the platformIndex
				parameter.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


long ApplyFontMap( Handle splineFont, long platformIndex, short* glyphPointer, long glyphCount, char* streamPointer, long* streamLengthPointer )

{

	// Locals
	
		long									glyphsProcessed;
		sfnt_char2IndexDirectory*				charToIndexTablePointer;
		sfnt_platformEntry*						platformPointer;
		sfnt_mappingTable*						mappingTablePointer;
		uint16*									mappingDataPointer;
		
	glyphsProcessed = 0;
	
	charToIndexTablePointer = (sfnt_char2IndexDirectory*) GetFontTablePtr( splineFont, 0, tag_CharToIndexMap );
	if ( charToIndexTablePointer != nil ) {
	
		platformPointer = charToIndexTablePointer->platform + platformIndex - 1;
		mappingTablePointer = (sfnt_mappingTable*) ((char*) charToIndexTablePointer + platformPointer->offset);
		mappingDataPointer = (uint16*)(mappingTablePointer + 1);
		
		switch ( mappingTablePointer->format ) {
		
			case 0:
				glyphsProcessed = MapString0( (uint8*) mappingDataPointer, (uint8*) streamPointer, glyphPointer, glyphCount );
				break;
				
			case 2:
				glyphsProcessed = MapString2( mappingDataPointer, streamPointer, glyphPointer, glyphCount, streamLengthPointer );
				break;
				
			case 4:
				if ( platformPointer->platformID == plat_Macintosh )
					glyphsProcessed = MapString4_8( mappingDataPointer, (uint8*) streamPointer, glyphPointer, glyphCount );
				else
					glyphsProcessed = MapString4_16( mappingDataPointer, (uint16*) streamPointer, glyphPointer, glyphCount );
				break;
				
			case 6:
				if ( platformPointer->platformID == plat_Macintosh )
					glyphsProcessed = MapString6_8( mappingDataPointer, (uint8*) streamPointer, glyphPointer, glyphCount );
				else
					glyphsProcessed = MapString6_16( mappingDataPointer, (uint16*) streamPointer, glyphPointer, glyphCount );
				break;
				
			default:
				break;
		
		}
	
		QDUngetFontFrag( splineFont, charToIndexTablePointer );
	
	}
	
	return( glyphsProcessed );

}
