/*	--------------------------------------------------------------------------------------

	Copyright © 1990-1991, Apple Computer, Inc, All Rights Reserved.
	
	File:		Assertion.h
		
	Author:		John Farmer
	
	Contains:	Header file information for the file Assertion.c.
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
	System 6.1.0 and 7.0 Changes:
	
		<2>		04/02/91	John Farmer - Changed kAssertionFailed to false and renamed
							assertionFailed parameter to assertion.
							
		<1>		11/29/90	John Farmer - Create file and it's original contents.
														
	-----------------------------------------------------------------------------------  */


#ifndef			__Assertion__
#define			__Assertion__


// Constants


	#define		kAssertionFailed				false


// Prototypes

	
	Boolean Assert( Boolean assertion, StringPtr assertionString );
	Boolean AssertCharacter( Character value, Character minimum, Character maximum );
	Boolean AssertInteger( LongInteger value, LongInteger minimum, LongInteger maximum );
	Boolean AssertUnsignedInteger( UnsignedLongInteger value, UnsignedLongInteger minimum, UnsignedLongInteger maximum );
	Boolean AssertPointer( Ptr thePointer );
	Boolean AssertHandle( Handle theHandle );
	

#endif			__Assertion__
