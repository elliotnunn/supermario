/*
	File:		ZoneHandle.c

	Contains:	Routines for manipulating handles in a specific heap zone.

	Written by:	John Farmer

	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 3/27/92	DTY		#1024868: Remove calls to Assert since it doesn’t do anything
									any more. Also added checks to make sure the zone parameter is
									not nil, since the partial font zone is not created for Roman
									systems.

		System 6.1.0 and 7.0 Changes:
		
		<2>		04/02/91	John Farmer - Corrected incorrect assert function result test
							in ResizeZoneHandle. Added SetZoneHandleSize routine.
							
		<1>		03/23/91	John Farmer - Created file and documentation.
														
*/


// Include Statements


	#include	"BaseIncludes.h"
	#include	"Assertion.h"
	#include	"Errors.h"
	#include	"Memory.h"
	#include	"ZoneHandle.proto"
	
			
/*	--------------------------------------------------------------------------------------

	Routine:	savedZone = SwapZone( zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
THz SwapZone( THz zone )

{

	// Locals
	
		THz									savedZone;
		
	savedZone = GetZone( );
	SetZone( zone );
	
	return( savedZone );

}

	
/*	--------------------------------------------------------------------------------------

	Routine:	error = NewZonePtr( &thePointer, pointerSize, zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
OSErr NewZonePtr( Ptr* ptrPointer, Size pointerSize, THz zone )

{

	// Locals
	
		OSErr								error;
		THz									savedZone;
	
	if (zone != nil) {
		savedZone = SwapZone( zone );
		
		*ptrPointer = NewPtr( pointerSize );
		error = MemError( );
		
		SwapZone( savedZone );
	}
	else
		error = memAZErr;
		
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = NewZoneHandle( &theHandle, handleSize, zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
OSErr NewZoneHandle( Handle* handlePointer, Size handleSize, THz zone )

{

	// Locals
	
		OSErr								error;
		THz									savedZone;
		
	if (zone != nil) {
		savedZone = SwapZone( zone );
		
		*handlePointer = NewHandle( handleSize );
		error = MemError( );
		
		SwapZone( savedZone );
	}
	else
		error = memAZErr;

	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = DisposeZoneHandle( theHandle, zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
OSErr DisposeZoneHandle( Handle theHandle, THz zone )

{

	// Locals
	
		OSErr								error;
		THz									savedZone;
		
	if (zone != nil) {
		savedZone = SwapZone( zone );
		
		DisposHandle( theHandle );
		error = MemError( );
		
		SwapZone( savedZone );
	}
	else
		error = memAZErr;
		
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = GetZoneHandleSize( &theSize, theHandle, zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
OSErr GetZoneHandleSize( Size* sizePointer, Handle theHandle, THz zone )

{

	// Locals
	
		OSErr								error;
		THz									savedZone;
		
	if (zone != nil) {
		savedZone = SwapZone( zone );
		
		*sizePointer = GetHandleSize( theHandle );
		error = MemError( );
		
		SwapZone( savedZone );
	}
	else
		error = memAZErr;
		
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = SetZoneHandleSize( theHandle, theSize, zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
OSErr SetZoneHandleSize( Handle theHandle, Size theSize, THz zone )

{

	// Locals
	
		OSErr								error;
		THz									savedZone;
		
	if (zone != nil) {
		savedZone = SwapZone( zone );
		
		SetHandleSize( theHandle, theSize );
		error = MemError( );
		
		SwapZone( savedZone );
	}
	else
		error = memAZErr;
		
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = ResizeZoneHandle( theHandle, delta, zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
OSErr ResizeZoneHandle( Handle theHandle, Size delta, THz zone )

{

	// Locals
	
		OSErr								error;
		Size								oldSize;
		THz									savedZone;
		
	if (zone != nil) {
		savedZone = SwapZone( zone );
		
		oldSize = GetHandleSize( theHandle );
		if ( (error = MemError( )) == noErr ) {
		
			SetHandleSize( theHandle, oldSize + delta );
			error = MemError( );
		
		}
		
		SwapZone( savedZone );
	}
	else
		error = memAZErr;
		
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = MoveZoneHandleHigh( theHandle, zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
OSErr MoveZoneHandleHigh( Handle theHandle, THz zone )

{

	// Locals
	
		OSErr								error;
		THz									savedZone;
	
	if (zone != nil) {
		savedZone = SwapZone( zone );
		
		MoveHHi( theHandle );
		error = MemError( );
		
		SwapZone( savedZone );
	}
	else
		error = memAZErr;

	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = LockZoneHandle( theHandle, zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
OSErr LockZoneHandle( Handle theHandle, THz zone )

{

	// Locals
	
		OSErr								error;
		THz									savedZone;
		
	if (zone != nil) {
		savedZone = SwapZone( zone );
		
		HLock( theHandle );
		error = MemError( );
		
		SwapZone( savedZone );
	}
	else
		error = memAZErr;
		
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = UnlockZoneHandle( theHandle, zone )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */
	
	
OSErr UnlockZoneHandle( Handle theHandle, THz zone )

{

	// Locals
	
		OSErr								error;
		THz									savedZone;
		
	if (zone != nil) {
		savedZone = SwapZone( zone );
		
		HUnlock( theHandle );
		error = MemError( );
		
		SwapZone( savedZone );
	}
	else
		error = memAZErr;

	return( error );
	
}