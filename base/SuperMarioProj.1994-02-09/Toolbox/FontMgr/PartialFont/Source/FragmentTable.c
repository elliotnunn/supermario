/*
	File:		FragmentTable.c

	Contains:	Fragment table routine

	Written by:	John Farmer

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 3/27/92	DTY		#1024868: Remove inclusion of Assertion.h, since this file
									doesn’t make any of those calls.

		System 6.1.0 Changes:
		
		<1>		03/23/91	John Farmer - Created file and documentation.
														
*/

// Include Statements


	#include	"BaseIncludes.h"
	#include	"FragmentTable.proto"
	#include	"Memory.h"
	#include	"PartialFontExtensions.proto"
	#include	"Types.h"	
	#include	"ZoneHandle.proto"
	
			
/*	--------------------------------------------------------------------------------------

	Routine:	FindFragmentTable( &fragmentTable, splineFont, fragmentTableCache )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void FindFragmentTable( FragmentTableStructureHandle* tablePointer, Handle splineFont, FragmentTableCacheStructureHandle fragmentTableCache )

{

	// Locals
	
		Boolean								found;
		FragmentTableStructureHandle		fragmentTable;
		
	*tablePointer = nil;
	
	found = false;

	fragmentTable = (*fragmentTableCache)->fFirstFragmentTable;	
	while ( (found == false) && (fragmentTable != nil) ) {
	
		if ( found = (*fragmentTable)->fFontHandle == splineFont ) {
		
			found = true;
			*tablePointer = fragmentTable;
			
		}
		
		fragmentTable = (*fragmentTable)->fNextFragmentTable;
		
	}
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	CreateFragmentTable( &fragmentTable, splineFont )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr CreateFragmentTable( FragmentTableStructureHandle* tablePointer, Handle splineFont )

{

	// Locals
	
		OSErr									error;
		FragmentTableStructureHandle			fragmentTable;
		
	*tablePointer = nil;
	
	error = NewZoneHandle( (Handle*) &fragmentTable, (Size) sizeof( FragmentTableStructure ), GetPartialFontZone( ) );
	if ( error == noErr ) {
	
		(*fragmentTable)->fPreviousFragmentTable = nil;
		(*fragmentTable)->fNextFragmentTable = nil;
		(*fragmentTable)->fFontHandle = splineFont;
		(*fragmentTable)->fFirstFragment = nil;
		(*fragmentTable)->fLastFragment = nil;
		
		*tablePointer = fragmentTable;
		
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	AddFragmentTableToCache( fragmentTable, nextFragmentTable, fragmentTableCache )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void AddFragmentTableToCache( FragmentTableStructureHandle fragmentTable, FragmentTableStructureHandle nextFragmentTable, FragmentTableCacheStructureHandle fragmentTableCache )

{

	// Locals
	
		FragmentTableStructureHandle			previousFragmentTable;
		
	
	if ( nextFragmentTable == nil ) {
	
		previousFragmentTable = (*fragmentTableCache)->fLastFragmentTable;
		(*fragmentTableCache)->fLastFragmentTable = fragmentTable;
	
	} else {
	
		previousFragmentTable = (*nextFragmentTable)->fPreviousFragmentTable;
		(*nextFragmentTable)->fPreviousFragmentTable = fragmentTable;
		
	}

	if ( previousFragmentTable == nil ) {
	
		(*fragmentTableCache)->fFirstFragmentTable = fragmentTable;
	
	} else {
	
		(*previousFragmentTable)->fNextFragmentTable = fragmentTable;
		
	}
			
	(*fragmentTable)->fPreviousFragmentTable = previousFragmentTable;
	(*fragmentTable)->fNextFragmentTable = nextFragmentTable;

}


/*	--------------------------------------------------------------------------------------

	Routine:	RemoveFragmentTableFromCache( fragmentTable, fragmentTableCache )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void RemoveFragmentTableFromCache( FragmentTableStructureHandle fragmentTable, FragmentTableCacheStructureHandle fragmentTableCache )

{

	// Locals
	
		FragmentTableStructureHandle			previousFragmentTable, nextFragmentTable;
		
	previousFragmentTable = (*fragmentTable)->fPreviousFragmentTable;
	nextFragmentTable = (*fragmentTable)->fNextFragmentTable;
	
	if ( previousFragmentTable == nil ) {
	
		(*fragmentTableCache)->fFirstFragmentTable = nextFragmentTable;
	
	} else {
	
		(*previousFragmentTable)->fNextFragmentTable = nextFragmentTable;
	
	}

	if ( nextFragmentTable == nil ) {
	
		(*fragmentTableCache)->fLastFragmentTable = previousFragmentTable;
	
	} else {
	
		(*nextFragmentTable)->fPreviousFragmentTable = previousFragmentTable;
	
	}
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	SetCurrentFragmentTable( fragmentTable, fragmentTableCache )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void SetCurrentFragmentTable( FragmentTableStructureHandle fragmentTable, FragmentTableCacheStructureHandle fragmentTableCache )

{

	RemoveFragmentTableFromCache( fragmentTable, fragmentTableCache );
	AddFragmentTableToCache( fragmentTable, (*fragmentTableCache)->fFirstFragmentTable, fragmentTableCache );

}