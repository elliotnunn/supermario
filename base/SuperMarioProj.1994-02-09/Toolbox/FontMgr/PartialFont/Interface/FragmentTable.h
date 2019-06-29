/*
	File:		FragmentTable.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	  4/2/92	JH		Added #ifndef rez so we could include some partial font stuff in
									our DoubleTalk resource file.

*/

/*	--------------------------------------------------------------------------------------

	Copyright © 1991, Apple Computer, Inc, All Rights Reserved.
	
	File:		FragmentTable.h
		
	Author:		John Farmer
	
	Contains:	Header file information for the file FragmentTable.c
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
		<1>		03/23/91	John Farmer - Created file.
														
	-----------------------------------------------------------------------------------  */


#ifndef			__FragmentTable__
#define			__FragmentTable__


// Inclusion Statements


	#include	"Fragment.h"
	
	
// Structures

#ifndef	rez
	typedef struct FragmentTableStructure {
	
		struct FragmentTableStructure**			fPreviousFragmentTable;
		struct FragmentTableStructure**			fNextFragmentTable;

		Handle									fFontHandle;
		FragmentStructureHandle					fFirstFragment;
		FragmentStructureHandle					fLastFragment;
		
	} FragmentTableStructure, *FragmentTableStructurePointer, **FragmentTableStructureHandle;

#endif
	
#endif			__FragmentTable__