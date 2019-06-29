/*
	File:		PartialFontExtensions.h

	Contains:	Header information for PartialFontExtensions.c

	Written by:	John Farmer

	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	  4/2/92	JH		Need to include #ifndef rez here so we don't screw up the
									DoubleTalk build.
		 <5>	  4/1/92	YK		#1025684: Add kPartialFontZoneBytesPerMasterPointer.  Rename
									kPartialFontZoneSize and kPartialFontZoneMasterPointerCount
									since the size is given by a parameter now.
		 <4>	 3/24/92	YK		#1025323: Sorry, Dean. We need 150K, which came from
									outer space.
		 <3>	 3/16/92	YK		Conditionalize some lines by ‘rez’ variable since DoubleTalk.r
									now needs to include this file. System build will not be
									affected by this change.
		 <2>	 3/13/92	DTY		I’m not convinced that a 150K heap is necessary. Reduce the heap
									down to about 32K.

*/

/*	--------------------------------------------------------------------------------------

	Copyright © 1990-1991, Apple Computer, Inc, All Rights Reserved.
	
	File:		PartialFontExtensions.h
		
	Author:		John Farmer
	
	Contains:	Header file information for the file PartialFontExtensions.c
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
		<2>		06/03/91	John Farmer - Added kMinimumFreeHeapSpace constant.
		
		<1>		03/23/91	John Farmer - Created file.
														
	-----------------------------------------------------------------------------------  */


#ifndef			__PartialFontExtensions__
#define			__PartialFontExtensions__


// Include Statements


	#include	"FragmentTable.h"
	

// Constants


	#define	kPartialFontRecommendedZoneSize					(150*1024)
	#define	kPartialFontRecommendedZoneMasterPointerCount	(128)
	#define	kPartialFontBytesPerMasterPointer				(kPartialFontRecommendedZoneSize/kPartialFontRecommendedZoneMasterPointerCount)
	#define	kMinimumFreeHeapSpace							(15*1024)
	

// Structures

#ifndef rez
	typedef struct {
	
		FragmentTableStructureHandle			fFirstFragmentTable;
		FragmentTableStructureHandle			fLastFragmentTable;
		
	} FragmentTableCacheStructure, *FragmentTableCacheStructurePointer, **FragmentTableCacheStructureHandle;
	
#endif rez	
	
#endif			__PartialFontExtensions__
