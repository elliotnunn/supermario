/*
	File:		Private Thing Manager.h

	Copyright:	© 1990-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM5>	  8/5/93	JDR		integrate component mgr from reality
	   <SM4>	 7/21/93	RC		House Cleaning from Reality
		 <6>	 7/16/93	JDR		Added SupportNativeComponents and kComponentIsNative.
		 <5>	 6/13/93	JDR		Rolled in the ComponentMgr sources from QuickTime 1.6.
		 <4>	11/12/92	JDR		Created kComponentInitialTableEntries which was set to 32
									entries in System 7.1, but I cut it back to only 16 to help save
									some RAM for the general user.
		 <3>	12/12/91	DTY		Get rid of the spaces in the file names
		 <2>	 12/5/91	YK		Rolled in QuickTime GM:
										grow cmpnt table by smaller steps to preserve system heap
		 <8>	 9/25/91	BAL		Added an internal flag to mark cloned app-heap compononents as
									subordinate to their sys heap parents.
		 <7>	 5/18/91	dvb		Thing->Component
		 <6>	  5/2/91	BAL		overlap rtDesc.FlagsMask and rtRefcon again.
		 <5>	  4/1/91	MK		make rtRefcon and rtFlagsMask separate again
		 <4>	 3/28/91	BAL		Do some file management.  Added parentThing and Captured flag.
		 <3>	 2/21/91	MK		slime some globals for codec manager
		 <2>	 2/16/91	BAL		Added InitThingManager call.
		 <3>	 12/30/90	bal		changed name to thing manager
		 <2>	 10/06/90	dvb		Make it work

	To Do:
*/

/* Private Thing Manager.h
	Internal Interface to the Movies Thing Manager

	by Jim Batson, Bruce Leak, David Van Brink
	Copyright 1990 Apple Computer, Inc. All rights reserved.
*/

#include <files.h>
#ifdef SupportNativeComponents
#include "FragLoad.h"
#endif

#ifndef _PrivateThingManager_
#define _PrivateThingManager_

typedef long Component;
typedef long ComponentInstance;

typedef short rtFileNum;

rtFileNum AddComponentResFile( Handle h );
OSErr RemoveComponentResFile( rtFileNum fn );
pascal short _FSpOpenResFile ( FSSpec* spec, char permission );

typedef enum
	{
	rtfCaptured = 1,
	rtfChild = 2,
	rtfAutoVersion = 4,
	rtfHasIconSuite = 8,					// this flag could be done away with if necessary
	rtfWantsUnregister = 16,
	rtfHasBeenOpened = 32,					// for optimizing unregister messages
	rtfAutoVersionUseFlags = 64,			// flags matter during auto version stuff
	rtfCodeFragment = 128					// this is a code fragment for PowerPC
	} rtFlagsType;

#define kComponentNeedsRegisterKiss (1L<<31)

typedef struct
	{
	OSType componentType;					/* A unique 4-byte code indentifying the command set */
	OSType componentSubType;				/* Particular flavor of this instance */
	OSType componentManufacturer;			/* Vendor indentification */
	unsigned long componentFlags;			/* 8 each for Component,Type,SubType,Manuf/revision */
	unsigned long componentFlagsMask;		/* Mask for specifying which flags to consider in search, zero during registration */
	} ComponentDescription;

typedef struct RegisteredComponent
	{
	void	*rtRoutine;
	ComponentDescription rtDesc;
	/*long rtRefcon; */						/* overlaps previous struct */
	struct RegisteredComponent *rtParent;
	Handle rtNameHand;
	Handle rtInfoHand;
	Handle rtIconHand;
	unsigned char rtFlags;
	unsigned char rtEntryUseCount;
	short rtReferenceCount;
	short rtFileNumber;
	short rtResourceID;
	long rtResourceType;
	long rtLocalA5;
	struct RegisteredComponent *rtNext;
	Handle rtRoutineHandle;				/* don't trust RecoverHandle */
#ifdef SupportNativeComponents
	ConnectionID rtConnectionID;
#endif
	} RegisteredComponent, *RegisteredComponentPtr;


typedef struct RegisteredComponentInstance
	{
	RegisteredComponentPtr rtiEntry;
	unsigned char rtiFlags;
	unsigned char rtiEntryUseCount;
	short rtiError;
	Handle rtiStorage;
	/*long  rtiQueuedCalls;*/
	long  rtiSwitchedA5;
	} RegisteredComponentInstance, *RegisteredComponentInstancePtr;


#ifndef __COMPONENTS__
	#include "Components.h"
#endif __COMPONENTS__

typedef struct rtFileOld
	{
	short	referenceCount;
	FSSpec	spec;
	} rtFileOld;

typedef struct rtFile
	{
	short	referenceCount;
	short	vRefNum;
	long	fileID;
	} rtFile;


typedef struct
	{
	RegisteredComponentInstance *rtInstanceTable;
	long	rtModSeed;
	long	rtReservedForCodecManager;
	short	rtInstanceTableUsedCount;
	short	rtInstanceTableTotalCount;
	short	rtFileUsedCount;
	short	rtFileTotalCount;
	short	rtTableEntryCount;
	Boolean	hasMoveHLo;
	Boolean	rtReserved;
	RegisteredComponent *rtTable;
	RegisteredComponent *rtUsedChain;
	RegisteredComponent *rtFreeChain;
	rtFile *rtFileTable;
	} ComponentManagerGlobalsRecord;

#define ComponentManagerLowMem	0x028A
#define ComponentManagerGlobals (*(ComponentManagerGlobalsRecord**)ComponentManagerLowMem)

#define kComponentAllocationSize 8
#define kComponentInstanceAllocationSize 32
#define kComponentFileAllocationSize 1

void doComponentSearch (void);

#endif _PrivateThingManager_