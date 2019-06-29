/*	--------------------------------------------------------------------------------------

	Copyright © 1991, Apple Computer, Inc, All Rights Reserved.
	
	File:		Fragment.h
		
	Author:		John Farmer
	
	Contains:	Header file information for the file Fragment.c
	
	Revisions: (most recent first):
	
		ID		Date		Description
		
		<2>		06/03/91	John Farmer - Added constants and fields for a cache blocking
							factor and maximum cache size.
							
		<1>		03/23/91	John Farmer - Created file.
														
	-----------------------------------------------------------------------------------  */


#ifndef			__Fragment__
#define			__Fragment__


// Include Statements

#ifndef	rez

	#include	"Types.h"


// Type definitions and enumerations


	typedef long								FragmentOffset;
	typedef long								FragmentLockCount;

#endif rez
// Constants


	#define	kFragmentBlockingFactor				128
	#define	kMaximumFragmentLength				(5*1024)
	

// Structures

#ifndef	rez

	typedef struct FragmentStructure {
	
		struct FragmentStructure**				fPreviousFragment;
		struct FragmentStructure**				fNextFragment;
		
		Boolean									fDuplicate;
		Boolean									fFragmentIsFull;
		FragmentOffset							fOffset;
		FragmentOffset							fLength;
		FragmentOffset							fCacheLength;
		FragmentLockCount						fLockCount;
		
		SignedByte								fData[];
	
	} FragmentStructure, *FragmentStructurePointer, **FragmentStructureHandle;
	
#endif rez	
#endif			__Fragment__