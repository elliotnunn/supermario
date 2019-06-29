/*	--------------------------------------------------------------------------------------

	Copyright © 1990-1991, Apple Computer, Inc, All Rights Reserved.
	
	File:		StringUtility.c
		
	Author:		John Farmer
	
	Contains:	Miscellaneous string utility routines.
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
		System 6.1.0 Changes:
	
		<1>		07/17/91	John Farmer - Created file.
														
	-----------------------------------------------------------------------------------  */


// Include Statements


	#include	"BaseIncludes.h"
	#include	"StringUtility.proto"
	

/*	--------------------------------------------------------------------------------------

	Routine:	isEqual = ComparePascalString( firstString, secondString )
		
	Purpose:	
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Boolean ComparePascalString( StringPtr firstStringPointer, StringPtr secondStringPointer )

{

	// Locals
	
		Integer									stringIndex, stringCount;
		
	stringIndex = 0;
	stringCount = *firstStringPointer;
	while ( (*firstStringPointer++ == *secondStringPointer++) && (stringIndex < stringCount) )
		stringIndex++;
		
	return( stringIndex == stringCount );

}
