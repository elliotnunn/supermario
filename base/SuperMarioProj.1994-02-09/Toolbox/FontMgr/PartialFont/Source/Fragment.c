/*
	File:		Fragment.c

	Contains:	Fragment routines

	Written by:	John Farmer

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 3/27/92	DTY		#1024868: Remove calls to Assert since it doesn’t do anything
									any more.

		System 6.1.0 Changes:
		
		<2>		06/03/91	John Farmer - Added cache blocking factor code. Fixed
							block merging bug in FindFragment.
							
		<1>		03/23/91	John Farmer - Created file and documentation.
														
*/

// Include Statements


	#include	"BaseIncludes.h"
	#include	"Assertion.h"
	#include	"Fragment.proto"
	#include	"Memory.h"
	#include	"PartialFontExtensions.proto"
	#include	"Resources.h"
	#include	"ZoneHandle.proto"
	
			
/*	--------------------------------------------------------------------------------------

	Routine:	AddFragmentToFragmentTable( fragment, nextFragment, fragmentTable )
		
	Purpose:	Insert fragment before nextFragment in the linked list contained
				in fragmentTable.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void AddFragmentToFragmentTable( FragmentStructureHandle fragment, FragmentStructureHandle nextFragment, FragmentTableStructureHandle fragmentTable )

{

	// Locals
	
		FragmentStructureHandle					previousFragment;
		
	
	if ( nextFragment == nil ) {
	
		previousFragment = (*fragmentTable)->fLastFragment;
		(*fragmentTable)->fLastFragment = fragment;
	
	} else {
	
		previousFragment = (*nextFragment)->fPreviousFragment;
		(*nextFragment)->fPreviousFragment = fragment;
		
	}

	if ( previousFragment == nil ) {
	
		(*fragmentTable)->fFirstFragment = fragment;
	
	} else {
	
		(*previousFragment)->fNextFragment = fragment;
		
	}
			
	(*fragment)->fPreviousFragment = previousFragment;
	(*fragment)->fNextFragment = nextFragment;

}


/*	--------------------------------------------------------------------------------------

	Routine:	RemoveFragmentFromFragmentTable( fragment, fragmentTable )
		
	Purpose:	Remove fragment from the linked list contained in fragmentTable.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void RemoveFragmentFromFragmentTable( FragmentStructureHandle fragment, FragmentTableStructureHandle fragmentTable )

{

	// Locals
	
		FragmentStructureHandle					previousFragment, nextFragment;
		
	previousFragment = (*fragment)->fPreviousFragment;
	nextFragment = (*fragment)->fNextFragment;
	
	if ( previousFragment == nil ) {
	
		(*fragmentTable)->fFirstFragment = nextFragment;
	
	} else {
	
		(*previousFragment)->fNextFragment = nextFragment;
	
	}

	if ( nextFragment == nil ) {
	
		(*fragmentTable)->fLastFragment = previousFragment;
	
	} else {
	
		(*nextFragment)->fPreviousFragment = previousFragment;
	
	}
	
	(*fragment)->fPreviousFragment = nil;
	(*fragment)->fNextFragment = nil;
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	SetCurrentFragment( fragment, fragmentTable )
		
	Purpose:	Move fragment from it's current position to the front of the list.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void SetCurrentFragment( FragmentStructureHandle fragment, FragmentTableStructureHandle fragmentTable )

{

	RemoveFragmentFromFragmentTable( fragment, fragmentTable );
	AddFragmentToFragmentTable( fragment, (*fragmentTable)->fFirstFragment, fragmentTable );

}


/*	--------------------------------------------------------------------------------------

	Routine:	LockFragment( fragment, fragmentTable )
		
	Purpose:	Lock the fragment down if the fragment lock count is zero. The fragment handle
				is not moved high for performance reasons. Fragments are normally locked and
				unlocked in stack order by the scaler; heap fragmentation is temporary.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void LockFragment( FragmentStructureHandle fragment, FragmentTableStructureHandle fragmentTable )

{

	#pragma unused (fragmentTable)
	
	// Locals
	
		THz									partialFontZone;
		
	if ( (*fragment)->fLockCount++ == 0 ) {
	
		partialFontZone = GetPartialFontZone( );
		LockZoneHandle( (Handle) fragment, partialFontZone );
		
	
	}

}


/*	--------------------------------------------------------------------------------------

	Routine:	UnlockFragment( fragment, fragmentTable )
		
	Purpose:	Unlock the fragment if the fragment has only been locked once. If the fragment
				is a duplicate (See ExpandAndLockFragment) we delete.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void UnlockFragment( FragmentStructureHandle fragment, FragmentTableStructureHandle fragmentTable )

{

	if ( --(*fragment)->fLockCount == 0 ) {
	
		UnlockZoneHandle( (Handle) fragment, GetPartialFontZone( ) );		
	
		if ( (*fragment)->fDuplicate == true ) {
		
			RemoveFragmentFromFragmentTable( fragment, fragmentTable );
			DisposeZoneHandle( (Handle) fragment, GetPartialFontZone( ) );
		
		}
		
	}

}


/*	--------------------------------------------------------------------------------------

	Routine:	cacheLength = ComputeFragmentCacheLength( length )
		
	Purpose:	Given a logical fragment size, return the physical fragment size rounded
				up to the nearest blocking factor.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


FragmentOffset ComputeFragmentCacheLength( FragmentOffset length )

{

	return( (((sizeof( FragmentStructure ) + length) / kFragmentBlockingFactor) + 1) * kFragmentBlockingFactor );

}


/*	--------------------------------------------------------------------------------------

	Routine:	CreateFragment( &fragment, offset, length, splineFont )
		
	Purpose:	Create and initialize a new fragment at offset for length bytes in the
				splineFont. After allocating memory and initializing the fields the
				data is read into memory.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr CreateFragment( FragmentStructureHandle* fragmentPointer, FragmentOffset offset, FragmentOffset length, Handle splineFont )

{

	// Locals
	
		OSErr								error;
		FragmentOffset						cacheLength;
		FragmentStructureHandle				fragment;
		
	*fragmentPointer = nil;
	
	cacheLength = ComputeFragmentCacheLength( length );
	
	error = NewZoneHandle( (Handle*) &fragment, cacheLength, GetPartialFontZone( ) );
	if ( error == noErr ) {
	
		(*fragment)->fOffset = offset;
		(*fragment)->fLength = length;
		(*fragment)->fCacheLength = cacheLength;
		(*fragment)->fLockCount = 0;
		(*fragment)->fDuplicate = false;
		(*fragment)->fFragmentIsFull = false;
		*fragmentPointer = fragment;
		
		ReadPartialResource( splineFont, offset, (*fragment)->fData, length );
		error = ResError( );
		
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	ExpandFragment( fragment, positiveDelta, fragmentTable )
		
	Purpose:	Expand an unlocked fragment (if necessary) and read the expanded area
				into memory. If the fragment is already large enough to hold the expanded
				data the resizing step is skipped to improve performance.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr ExpandFragment( FragmentStructureHandle fragment, FragmentOffset positiveDelta, FragmentTableStructureHandle fragmentTable )

{

	// Locals
	
		OSErr								error;	
		FragmentOffset						newLength, newCacheLength;
		
	error = noErr;
	
	newLength = (*fragment)->fLength + positiveDelta;
	if ( newLength + sizeof( FragmentStructure ) > (*fragment)->fCacheLength ) {
	
		newCacheLength = ComputeFragmentCacheLength( newLength );
		error = SetZoneHandleSize( (Handle) fragment, newCacheLength, GetPartialFontZone( ) );
		if ( error == noErr ) {
		
			(*fragment)->fCacheLength = newCacheLength;
		
		}
		
	}
	
	if ( error == noErr ) {
	
		ReadPartialResource( (*fragmentTable)->fFontHandle, (*fragment)->fOffset + (*fragment)->fLength, (*fragment)->fData + (*fragment)->fLength, positiveDelta );
		if ( (error = ResError( )) == noErr ) {
		
			(*fragment)->fLength = newLength;
		
		}
		
	}
			
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	ExpandAndLockFragment( fragmentPointer, offset, length, fragmentTable )
		
	Purpose:	Expand a fragment and lock it down. The fragment must be removed from the
				fragment chain in case our grow zone procedure is called since the grow
				zone proc disposes of fragments. A point to the fragment is passed in
				case we have to create a duplicate fragment.
				
				Duplicates are created whenever there is an attempt to expand a locked
				fragment. The current fragment is marked as a duplicate, a new fragment
				is created for the requested range, and the new fragment handle is
				returned in the fragmentPointer parameter.
				
				If a fragment's physical length is greater than a specified limit the
				fragment is marked as full to prevent further fragment expansion. Failure
				to do so could result in one huge fragment and performance degradation.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr ExpandAndLockFragment( FragmentStructureHandle *fragmentPointer, FragmentOffset offset, FragmentOffset length, FragmentTableStructureHandle fragmentTable )

{

	// Locals
	
		OSErr									error;
		FragmentOffset							insertEnd, fragmentEnd;
		FragmentStructureHandle					fragment;
		
	error = noErr;
	fragment = *fragmentPointer;
	RemoveFragmentFromFragmentTable( fragment, fragmentTable );

	insertEnd = offset + length;
	fragmentEnd = (*fragment)->fOffset + (*fragment)->fLength;
	if ( (insertEnd > fragmentEnd) && ( (*fragment)->fFragmentIsFull == false) ) {
	
		if ( (*fragment)->fLockCount > 0 ) {
		
			(*fragment)->fDuplicate = true;
			AddFragmentToFragmentTable( fragment, (*fragmentTable)->fFirstFragment, fragmentTable );
			error = CreateFragment( &fragment, (*fragment)->fOffset, length + (offset - (*fragment)->fOffset), (*fragmentTable)->fFontHandle );
			
		} else {
		
			error = ExpandFragment( fragment, insertEnd - fragmentEnd, fragmentTable );
			if ( error == noErr ) {
			
				(*fragment)->fFragmentIsFull = (*fragment)->fCacheLength > kMaximumFragmentLength;
			
			}
			
		}
		
	}
	
	if ( error == noErr ) {
	
		LockFragment( fragment, fragmentTable );
		AddFragmentToFragmentTable( fragment, (*fragmentTable)->fFirstFragment, fragmentTable );
		*fragmentPointer = fragment;
		
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	FindFragment( &fragment, offset, length, fragmentTable )
		
	Purpose:	Search through a fragment table for a fragment offset that falls within
				an existing fragment. If we find a fragment match and the fragment is
				not full we return the fragment. If the fragment is full the fragment
				cannot be expanded and we only return the fragment if the offset + length
				request would not cause fragment expansion.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void FindFragment( FragmentStructureHandle* fragmentPointer, FragmentOffset offset, FragmentOffset length, FragmentTableStructureHandle fragmentTable )

{

	// Locals
	
		FragmentOffset						compareStart, compareEnd;
		FragmentStructureHandle				fragment;
			
	*fragmentPointer = nil;
	fragment = (*fragmentTable)->fFirstFragment;
	
	while ( (*fragmentPointer == nil) && (fragment != nil) ) {
	
		compareStart = (*fragment)->fOffset;
		compareEnd = compareStart + (*fragment)->fLength;
		
		if ( (offset >= compareStart) && (offset <= compareEnd) ) {
		
			if ( ((*fragment)->fFragmentIsFull == false) || (offset + length <= compareEnd) ) {
			
				*fragmentPointer = fragment;
		
			}
			
		}
		
		fragment = (*fragment)->fNextFragment;
		
	}
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	FindOrCreateFragment( &fragment, offset, length, fragmentTable )
		
	Purpose:	Find or create a fragment at offset for length bytes in the fragmentTable.
				If we find the fragment it is expanded (if necessary) and locked down.
				Otherwise, we create a new fragment, add it to the fragment table and
				lock it down.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr FindOrCreateFragment( FragmentStructureHandle* fragmentPointer, FragmentOffset offset, FragmentOffset length, FragmentTableStructureHandle fragmentTable )

{

	// Locals
	
		OSErr										error;
		
	error = noErr;
	
	FindFragment( fragmentPointer, offset, length, fragmentTable );
	if ( *fragmentPointer != nil ) {

		error = ExpandAndLockFragment( fragmentPointer, offset, length, fragmentTable );				
		
	} else {

		error = CreateFragment( fragmentPointer, offset, length, (*fragmentTable)->fFontHandle );
		if ( error == noErr ) {
		
			AddFragmentToFragmentTable( *fragmentPointer, (*fragmentTable)->fFirstFragment, fragmentTable );
			LockFragment( *fragmentPointer, fragmentTable );
		
		}

	}

	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	FindFragmentHandle( &fragment, dataPointer, fragmentTable )
		
	Purpose:	Find fragment handle searches for a fragment handle in the fragmentTable that
				dataPointer is contained within. This routine is similar to RecoverHandle
				except that it is fragment specific and the dataPointer may be contained
				anywhere within the handle.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void FindFragmentHandle( FragmentStructureHandle* fragmentPointer, Ptr dataPointer, FragmentTableStructureHandle fragmentTable )

{

	// Locals
	
		Ptr									handleStart, handleEnd;
		FragmentStructureHandle				fragment;
		
	*fragmentPointer = nil;
	fragment = (*fragmentTable)->fFirstFragment;
	
	while ( (*fragmentPointer == nil) && (fragment != nil) ) {
	
		handleStart = (Ptr) StripAddress( *fragment );
		handleEnd = handleStart + sizeof( FragmentStructure ) + (*fragment)->fLength;
		
		if ( (dataPointer >= handleStart) && (dataPointer < handleEnd) ) {
		
			*fragmentPointer = fragment;
		
		}
		
		fragment = (*fragment)->fNextFragment;
	
	}

}


