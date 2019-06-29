/*	--------------------------------------------------------------------------------------

	Copyright © 1990-1991, Apple Computer, Inc, All Rights Reserved.
	
	File:		BaseConstants.h
		
	Author:		John Farmer
	
	Contains:	Constants shared throughout all source files.
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
		<2>		3/6/91		Hae-sung Kim - Added more general character constants.
		<1>		11/29/90	John Farmer - Create file and it's original contents.
		
	-----------------------------------------------------------------------------------  */


#ifndef			__BaseConstants__
#define			__BaseConstants__


// <2> Define some useful non-printable charactors.
typedef	enum {

	kNull = 0x00,				// Ascii Null character.
	kEnter = 0x03,				// Ascii Enter character.
	kHTab = 0x09,				// Ascii Horizontal Tab character.
	kReturn = 0x0D,				// Ascii Caridge Return character.
	kSpace = 0x20,				// Ascii Space character.
	kKoreanSpace = 0xA1A1,		// DoubleByte Korean space character.
	kJapaneseSpace = 0x8140,	// DoubleByte Japanese space character.
	kTradChineseSpace = 0xA140,	// DoubleByte TradChinese space character.
	kSimpChineseSpace =	0xA1A1	// DoubleByte SimpChinese space character.
	
} NonPrintableChars;
	
#define		kFixZero							((Fixed) 0x00000000)
#define		kFixOneHalf							((Fixed) 0x00008000)
#define		kFixOne								((Fixed) 0x00010000)
#define		kFixTwo								((Fixed) 0x00020000)

	
#endif			__BaseConstants__