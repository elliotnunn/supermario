/*
	File:		Assertion.c

	Contains:	This file contains routines for performing assertions.  These routines
				should be used on all parameters passed to a function.

	Written by:	John Farmer

	Copyright:	© 1990-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 3/31/92	YK		Simplify <4>.
		 <4>	 3/31/92	YK		#1024868: The jlf defines ‘The Empty Handle is a valid Handle.’
		 <3>	 3/27/92	DTY		#1024868: Skip calls to Assert since it doesn’t do anything any
									more.
		 <2>	 9/30/91	JSM		Kill a warning.
		 <1>	 9/25/91	jlf		checking in file for first time.

	System 6.1.0 and 7.0 changes:
	
		<2>		04/02/91	John Farmer - Assertion routines now return true if the assertion
							is true and don't drop into the debugger unless the assertion
							is false. Changed the name of this file from Assert.c to Assertion.c
							
		<1>		10/11/90	John Farmer - Created the new file and added routines

*/

// Include Directives


	#include	"BaseIncludes.h"
	#include	"Assertion.h"


/*	--------------------------------------------------------------------------------------

	Routine:	Assert
		
	Purpose:	Drop the user into macsbug if an assertion fails.
				
	Warnings:	Macsbug needs to be installed when this routine is called.
	
	-----------------------------------------------------------------------------------  */


Boolean Assert( Boolean assertion, StringPtr /* failureString */ )

{

/*	if ( assertion == false )
		DebugStr( failureString );
*/		
	return( assertion );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	AssertCharacter
		
	Purpose:	Make sure the value parameter is within the bounds of the minimum and
				maximum parameters.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Boolean AssertCharacter( Character value, Character minimum, Character maximum )

{
	
	Locals
	
		Boolean						assertion;
		
	assertion = (value >= minimum) && (value <= maximum);
	return ( assertion );
		
}


/*	--------------------------------------------------------------------------------------

	Routine:	AssertInteger
		
	Purpose:	Make sure the value parameter is within the bounds of the minimum and
				maximum parameters.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Boolean AssertInteger( LongInteger value, LongInteger minimum, LongInteger maximum )

{
	
	Locals
	
		Boolean						assertion;
		
	assertion = (value >= minimum) && (value <= maximum);
	return ( assertion );
		
}


/*	--------------------------------------------------------------------------------------

	Routine:	AssertUnsignedInteger
		
	Purpose:	Make sure the value parameter is within the bounds of the minimum and
				maximum parameters.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Boolean AssertUnsignedInteger( UnsignedLongInteger value, UnsignedLongInteger minimum,
							   UnsignedLongInteger maximum )
							   
{
	
	Locals
	
		Boolean						assertion;
		
	assertion = (value >= minimum) && (value <= maximum);
	return ( assertion );
		
}


/*	--------------------------------------------------------------------------------------

	Routine:	AssertPointer
		
	Purpose:	Make sure thePointer parameter is not nil or odd.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Boolean AssertPointer( Ptr thePointer )

{
	
	Locals
	
		Boolean						assertion;
		
	assertion = (thePointer != nil) && (((LongInteger) thePointer & 1) == 0);
	return ( assertion );
		
}


/*	--------------------------------------------------------------------------------------

	Routine:	AssertHandle
		
	Purpose:	Make sure theHandle parameter is not nil or odd and the handle's master
				pointer is not odd.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Boolean AssertHandle( Handle theHandle )

{

	Locals
	
		Boolean						assertion;
	
	assertion = (theHandle != nil) && (((LongInteger) theHandle & 1) == 0);
	if ( assertion )
		assertion = (((LongInteger) *theHandle & 1) == 0);
	
	return( assertion );
	
}
