/*	--------------------------------------------------------------------------------------

	Copyright © 1990-1991, Apple Computer, Inc., All Rights Reserved.
	
	File:		BaseTypes.h
		
	Author:		John Farmer
	
	Contains:	This file contains alternative type definitions for the C base types.
				Use these types to define your structure specific types and never
				use base types directly.  For example, if you need an unsigned two
				byte integer as an index in your Foobar structure, define the following:
				
					typedef UnsignedInteger FoobarIndex, *FoobarIndexPointer;
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
		<1>		11/16/90	John Farmer - Created the file and it's original contents.
														
	-----------------------------------------------------------------------------------  */


#ifndef			__BaseTypes__
#define			__BaseTypes__


// Enumerations and Type Statements


	typedef char				*BytePointer, **ByteHandle;
	typedef unsigned char		UnsignedByte, *UnsignedBytePointer, **UnsignedByteHandle;

	typedef char				Character, *CharacterPointer, **CharacterHandle;
	typedef unsigned char		UnsignedCharacter, *UnsignedCharacterPointer, **UnsignedCharacterHandle;
	
	typedef short				Integer, *IntegerPointer, **IntegerHandle;
	typedef unsigned short		UnsignedInteger, *UnsignedIntegerPointer, **UnsignedIntegerHandle;
	
	typedef long				LongInteger, *LongIntegerPointer, **LongIntegerHandle;
	typedef unsigned long		UnsignedLongInteger, *UnsignedLongIntegerPointer, **UnsignedLongIntegerHandle;
		
	typedef pascal void (*StdTextProcedurePointer)( Integer byteCount, CharacterPointer textBuffer, Point numerator, Point denominator );


#endif			__BaseTypes__