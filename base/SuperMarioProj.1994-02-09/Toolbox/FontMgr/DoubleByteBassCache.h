/*	--------------------------------------------------------------------------------------

	Copyright © 1990-1991, Apple Computer, Inc, All Rights Reserved.
	
	File:		DoubleByteBassCache.h
		
	Author:		John Farmer
	
	Contains:	Header file information for the file DoubleByteBassCache.c
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
	System 6.1.0 Changes:
	
		<2>		03/04/91	John Farmer - Transferred routine prototypes to BassCache.c
							to get rid of nasty header file dependencies and to remove
							dependencies on FontMgr project from KanjiTalk610.
							
		<1>		11/29/90	John Farmer - Create file and it's original contents.
														
	-----------------------------------------------------------------------------------  */


#ifndef			__DoubleByteBassCache__
#define			__DoubleByteBassCache__


// Include Directives

	
	#include	"Memory.h"
	

// Constants


	#define		kSingleByte								0
	#define		kLowByteGlyphOffsetTableSize			(256 * sizeof( unsigned long ))
	
	
// Structures


	typedef struct {
	
		unsigned char						fHighByte[256];
		unsigned char						fLowByte[256];
		unsigned char						fDefaultLowByte;
		unsigned char						fPad;
		
	} EncodingTable, *EncodingTablePointer;
	
	
#endif			__DoubleByteBassCache__
