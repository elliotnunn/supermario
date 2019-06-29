/*	--------------------------------------------------------------------------------------

	Copyright © 1990-1991, Apple Computer, Inc, All Rights Reserved.
	
	File:		QuickDrawUtility.h
		
	Author:		John Farmer
	
	Contains:	Header file information for the QuickDrawUtility.c file.
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
		System 6.1.0 and 7.0 Changes:
	
		<1>		01/30/91	John Farmer - Created file.
														
	-----------------------------------------------------------------------------------  */


// Include Directives


	#include		"QuickDrawPrivate.h"
	

// Prototypes

	
	Fixed GetFixedFontSize( void );
	Boolean IsColorPort( GrafPtr port );
	Fixed GetFixedHorizontalPen( void );
	void MoveFixedHorizontalPen( Fixed horizontalOffset );
	Fixed FixedStdTxMeas( Integer textLength, UnsignedCharacterPointer textBuffer, Point *numeratorPointer, Point *denominatorPointer, FontInfo *fontInformationPointer );
	
	extern pascal Fixed GetFontAdj( void );
	extern pascal Integer GetPnLocFixed( void );
	extern pascal Fixed GetQDRunSlop( void );
	extern pascal Fixed GetQDChExtra( void );
	extern pascal void SetPnLocFixed( Integer pnLocFixed );
	extern pascal void SetQDChExtra( Fixed qdChExtra );
	extern pascal void SetQDRunSlop( Fixed qdRunSlop );