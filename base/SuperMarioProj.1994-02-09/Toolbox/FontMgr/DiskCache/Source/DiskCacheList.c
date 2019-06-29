/*
	File:		DiskCacheList.c

	Contains:	Code for maintaining the disk cache lists. Caches are maintained in two
				linked lists. The offset list is sorted in physical file offset order.
				The date list is stored in most recently used to least recently used
				order.
				
				An empty array of disk cache entries is maintained inside of the disk
				cache map for disk cache storage. The array isn't dynamically allocated
				since we can't allocate memory if called during a purge procedure (the
				array is tiny compared to the size of the font cache file for all you
				chronic-byte-twiddling-weenies).
				
				The routines in this file are cake so I'm going to spare myself the pain
				documenting them.
	
	Written by:	John Farmer

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 3/27/92	DTY		#1024868: Delete inclusion of Assertion.h, since this file
									doesn’t use these routines.
		 <2>	 1/29/92	JSM		Fix bug #1020958.  In InsertDiskCache(), if we were inserting a
									new cache just before one we were about to delete because
									FindFirstFreeCache() didn't find a free cache, fNextOffsetIndex
									for the new entry would point to itself, causing the linked list
									to loop back on itself.
		 <1>	 7/17/91	jlf		Split these routines out of DiskCacheExtensions.c

*/

// Include Statements


	#include	"BaseIncludes.h"

	#include	"DiskCacheList.proto"
	
	
/*	--------------------------------------------------------------------------------------

	Routine:	cacheIndex = FindFirstFreeCache( diskCacheMapHandle )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


DiskCacheIndex FindFirstFreeCache( DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		Boolean									freeCacheNotFound;
		DiskCacheIndex							cacheIndex, cacheCount;
		
	cacheIndex = 0;
	freeCacheNotFound = true;
	cacheCount = (*diskCacheMapHandle)->fMaximumDiskCacheCount;
	while ( freeCacheNotFound && (cacheIndex < cacheCount) ) {
	
		if ( (*diskCacheMapHandle)->fDiskCache[cacheIndex].fCacheIsFree ) {
		
			freeCacheNotFound = false;
		
		} else {
		
			cacheIndex++;
		
		}
	
	}
		
	return ( freeCacheNotFound ? kDiskCacheNotFound : cacheIndex );
		
}


/*	--------------------------------------------------------------------------------------

	Routine:	InsertDiskCacheIntoOffsetList( cacheIndex, nextCacheIndex, diskCacheMapHandle )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */



void InsertDiskCacheIntoOffsetList( DiskCacheIndex cacheIndex, DiskCacheIndex nextCacheIndex, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		DiskCacheIndex							previousCacheIndex;
		DiskCacheEntryStructure					*cachePointer;
		
	if ( nextCacheIndex == kDiskCacheNotFound ) {
	
		previousCacheIndex = (*diskCacheMapHandle)->fLastOffsetIndex;
		(*diskCacheMapHandle)->fLastOffsetIndex = cacheIndex;
		
	} else {
	
		previousCacheIndex = (*diskCacheMapHandle)->fDiskCache[nextCacheIndex].fPreviousOffsetIndex;
		(*diskCacheMapHandle)->fDiskCache[nextCacheIndex].fPreviousOffsetIndex = cacheIndex;
		
	}
	
	if ( previousCacheIndex == kDiskCacheNotFound ) {
	
		(*diskCacheMapHandle)->fFirstOffsetIndex = cacheIndex;
		
	} else {
	
		(*diskCacheMapHandle)->fDiskCache[previousCacheIndex].fNextOffsetIndex = cacheIndex;
	
	}
	
	cachePointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];	
	cachePointer->fPreviousOffsetIndex = previousCacheIndex;
	cachePointer->fNextOffsetIndex = nextCacheIndex;

}


/*	--------------------------------------------------------------------------------------

	Routine:	InsertDiskCacheIntoDateList( cacheIndex, nextCacheIndex, diskCacheMapHandle )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */



void InsertDiskCacheIntoDateList( DiskCacheIndex cacheIndex, DiskCacheIndex nextCacheIndex, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		DiskCacheIndex							previousCacheIndex;
		DiskCacheEntryStructure					*cachePointer;
		
	if ( nextCacheIndex == kDiskCacheNotFound ) {
	
		previousCacheIndex = (*diskCacheMapHandle)->fLastDateIndex;
		(*diskCacheMapHandle)->fLastDateIndex = cacheIndex;
		
	} else {
	
		previousCacheIndex = (*diskCacheMapHandle)->fDiskCache[nextCacheIndex].fPreviousDateIndex;
		(*diskCacheMapHandle)->fDiskCache[nextCacheIndex].fPreviousDateIndex = cacheIndex;
		
	}
	
	if ( previousCacheIndex == kDiskCacheNotFound ) {
	
		(*diskCacheMapHandle)->fFirstDateIndex = cacheIndex;
		
	} else {
	
		(*diskCacheMapHandle)->fDiskCache[previousCacheIndex].fNextDateIndex = cacheIndex;
	
	}
	
	cachePointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];	
	cachePointer->fPreviousDateIndex = previousCacheIndex;
	cachePointer->fNextDateIndex = nextCacheIndex;

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = InsertDiskCache( &cacheIndex, nextCacheIndex, diskCache, diskCacheMapHandle )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr InsertDiskCache( DiskCacheIndex *cacheIndexPointer, DiskCacheIndex nextCacheIndex, DiskCacheEntryStructure sourceCacheEntry, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		OSErr									error;
		
	error = noErr;
	
	*cacheIndexPointer = FindFirstFreeCache( diskCacheMapHandle );
	if ( *cacheIndexPointer == kDiskCacheNotFound ) {
		
		*cacheIndexPointer = (*diskCacheMapHandle)->fLastDateIndex;

		/* <2> */
		
		if (*cacheIndexPointer == nextCacheIndex)
		{
			// we're deleting the cache after the one we're about to insert, so we need to update nextCacheIndex
			nextCacheIndex = (*diskCacheMapHandle)->fDiskCache[nextCacheIndex].fNextOffsetIndex;
		}
		
		/* <2> */
		
		error = RemoveDiskCache( *cacheIndexPointer, diskCacheMapHandle );
		
	}
	
	if ( error == noErr ) {
	
		sourceCacheEntry.fCacheIsFree = false;
		(*diskCacheMapHandle)->fDiskCache[*cacheIndexPointer] = sourceCacheEntry;
		(*diskCacheMapHandle)->fDiskCacheCount++;
		
		InsertDiskCacheIntoOffsetList( *cacheIndexPointer, nextCacheIndex, diskCacheMapHandle );
		InsertDiskCacheIntoDateList( *cacheIndexPointer, (*diskCacheMapHandle)->fFirstDateIndex, diskCacheMapHandle );
		
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	RemoveDiskCacheFromOffsetList( cacheIndex, diskCacheMapHandle )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void RemoveDiskCacheFromOffsetList( DiskCacheIndex cacheIndex, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		DiskCacheIndex							previousCacheIndex, nextCacheIndex;
		DiskCacheEntryStructurePointer			cacheEntryPointer;
		
	cacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
	previousCacheIndex = cacheEntryPointer->fPreviousOffsetIndex;
	nextCacheIndex = cacheEntryPointer->fNextOffsetIndex;
	
	if ( previousCacheIndex == kDiskCacheNotFound ) {
	
		(*diskCacheMapHandle)->fFirstOffsetIndex = nextCacheIndex;
	
	} else {
	
		(*diskCacheMapHandle)->fDiskCache[previousCacheIndex].fNextOffsetIndex = nextCacheIndex;
	
	}
	
	if ( nextCacheIndex == kDiskCacheNotFound ) {
	
		(*diskCacheMapHandle)->fLastOffsetIndex = previousCacheIndex;
	
	} else {
		
		(*diskCacheMapHandle)->fDiskCache[nextCacheIndex].fPreviousOffsetIndex = previousCacheIndex;
		
	}
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	RemoveDiskCacheFromDateList( cacheIndex, diskCacheMapHandle )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void RemoveDiskCacheFromDateList( DiskCacheIndex cacheIndex, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		DiskCacheIndex							previousCacheIndex, nextCacheIndex;
		DiskCacheEntryStructurePointer			cacheEntryPointer;
		
	cacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
	previousCacheIndex = cacheEntryPointer->fPreviousDateIndex;
	nextCacheIndex = cacheEntryPointer->fNextDateIndex;
	
	if ( previousCacheIndex == kDiskCacheNotFound ) {
	
		(*diskCacheMapHandle)->fFirstDateIndex = nextCacheIndex;
	
	} else {
	
		(*diskCacheMapHandle)->fDiskCache[previousCacheIndex].fNextDateIndex = nextCacheIndex;
	
	}
	
	if ( nextCacheIndex == kDiskCacheNotFound ) {
	
		(*diskCacheMapHandle)->fLastDateIndex = previousCacheIndex;
	
	} else {
		
		(*diskCacheMapHandle)->fDiskCache[nextCacheIndex].fPreviousDateIndex = previousCacheIndex;
		
	}
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = RemoveDiskCache( cacheIndex, diskCacheMapHandle )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr RemoveDiskCache( DiskCacheIndex cacheIndex, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		OSErr									error;
		DiskCacheEntryStructurePointer			destinationCacheEntryPointer;
		
	error = noErr;
	
	destinationCacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
	destinationCacheEntryPointer->fCacheIsFree = true;
	(*diskCacheMapHandle)->fDiskCacheCount--;
	
	RemoveDiskCacheFromOffsetList( cacheIndex, diskCacheMapHandle );
	RemoveDiskCacheFromDateList( cacheIndex, diskCacheMapHandle );
		
	return( error );
	
}