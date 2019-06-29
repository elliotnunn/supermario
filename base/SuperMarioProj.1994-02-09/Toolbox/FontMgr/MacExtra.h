/*
	File:		MacExtra.h

	Contains:	xxx put contents here (or delete the whole line) xxx

	Written by:	xxx put name of writer here (or delete the whole line) xxx

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	10/30/90	CL		(MR)Taking out fs_getsfntinfo and putting in new call
									fs_lowestppem
		 <6>	 8/22/90	MR		Change fillWidthTable to take sfnt handle and rsrc id
		 <5>	 8/20/90	MR		Add lowestRecPPEM to GetSfntInfo
		 <4>	  5/3/90	RB		Nothing known
		 <3>	 3/20/90	CL		support for tag format
		 <2>	 2/27/90	CL		getting bbs headers
	   <1.1>	11/14/89	CEL		Adding in device metrics for width table.
	   <1.0>	 9/25/89	CEL		Adding file into EASE for the first time.

	To Do:
*/

/***************************************************************************************************
**											MacExtra.h
**
**	File:			MacExtra.h
**	Language:		"C"
**
**	Date: 			September 21, 1989
**	Revision:		1.0
**	Author:			Charlton E. Lui
**
**	Usage: Used for Macintosh specific defines in regards to the FontScaler package
**
**	Make sure that this include is after the FontScaler.h
**	
**
**	
**
***************************************************************************************************/
typedef struct {
	Fixed	ascent;
	Fixed	descent;
	Fixed	widMax;
	Fixed	leading;
	Fixed	lOverHMax;
	Fixed	rOverHMax;
	Fixed	yMax;
	Fixed	yMin;
} fs_LayoutInfo;

extern int32 fs_FillWidthTable( fs_GlyphInputType*, Fixed* wt, Handle sfnt, Fixed styleExtra, short sfntID, Boolean round );
extern int32 fs_FillLayoutInfo( fs_GlyphInputType*, fs_GlyphInfoType*, fs_LayoutInfo*);
pascal short fs_LowestPPEM(Handle fontHandle);
