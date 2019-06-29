/*	--------------------------------------------------------------------------------------

	Copyright © 1990-1991, Apple Computer, Inc, All Rights Reserved.
	
	File:		LowMemoryGlobals.h
		
	Author:		John Farmer
	
	Contains:	Definitions for low memory globals that are normally only defined in the
				assembly equates.
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
	System 6.1.0 and 7.0 Changes:
	
		<1>		11/29/90	John Farmer - Create file and it's original contents.
														
	-----------------------------------------------------------------------------------  */


#ifndef			__LowMemoryGlobals__
#define			__LowMemoryGlobals__


// Include Directives


	#include	"Fonts.h"
	

// Constants
	

	#define		gIntlSpec							((SMgrRecord**)		0x00000BA0)
	#define		gWidthTabHandle						((WidthTable***)	0x00000B2A)
	#define		gFMDefaultSize						((UnsignedByte*)	0x00000987)
	#define		gSysFontSize						((UnsignedInteger*)	0x00000BA8)
		
	
#endif			__LowMemoryGlobals__