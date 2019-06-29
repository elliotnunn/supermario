/*
	File:		ProcessMgrINIT.c

	Contains:	Code for an INIT which creates a resource override of the system.
				This code is only used to build an experimental Process Manager
				for testing, it SHOULD NOT be part of the normal system build!

	Written by:	Dave Falkenburg

	Copyright:	© 1980-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
	   <SM3>	 7/21/93	joe		Backed out <SM2>.
		 <1>	11/25/92	DRF		first checked in

*/ 

#include <Types.h>
#include <Resources.h>
#include <Memory.h>
#include <ResourceMgrPriv.h>
#include <SysEqu.h>
#include <Traps.h>

//	Functions defined in this file…

pascal Handle MyGet1IndResource(ResType, short);

void
main(void)
	{
	Handle myResourceMap;
	Handle systemResourceMap;
	Handle myNewResourceMap;
	SignedByte currentAttributes;
	Size myResourceMapSize;
	long oldTrap;

#if	PsychicTV
	//	Only install PsychicTV ProcessMgr on a RISC machine
	//	we check this by looking to see if GoNative has already installed
	//	trap interfaces to CodeFragmentManager…
	
	//	We really should check specifically for a PowerPC box here,
	//	but there is no Gestalt selector for this purpose yet…

	if (GetToolboxTrapAddress(0xaa5a) == GetToolboxTrapAddress(_Unimplemented))
		return;
#endif

	oldTrap = GetToolboxTrapAddress(_Get1IxResource);
	SetToolboxTrapAddress((long) MyGet1IndResource, _Get1IxResource);
	*(long*)ApplScratch = oldTrap;

	myResourceMap = GetMap(CurResFile());
	if(ResError())
		DebugStr("\pGetMap failed");

	// Special code to put my resource map into the system heap.
	myResourceMapSize = GetHandleSize(myResourceMap);
	myNewResourceMap = NewHandleSys(myResourceMapSize);
	BlockMove(*myResourceMap,*myNewResourceMap,myResourceMapSize);

	*(Handle *)TopMapHndl = myNewResourceMap;
	DisposHandle(myResourceMap);
	
	systemResourceMap = GetOverrideMap(GetMap(0));
	if(ResError())
		DebugStr("\pGetOverrideMap failed");

	InsertOverrideMap(myNewResourceMap,systemResourceMap);
	if(ResError())
		DebugStr("\pInsertOverridMap");
		
	currentAttributes = GetOverrideAttributes(myNewResourceMap);
	if(ResError())
		DebugStr("\pGetOverrideAttributes failed");
		
	currentAttributes |= (kDontCountOrIndexDuplicatesMask |
						  kTwoDeepMask |
						  kPreventFileFromBeingClosedMask |
						  kOverrideNextMapMask);
						  
	SetOverrideAttributes(myNewResourceMap,currentAttributes);
	if(ResError())
		DebugStr("\pSetOverrideAttributes failed");
	}


//	This GetIndResource patch prevents loading of additional resources from the
//	System file.  It is basically a hack to prevent the Start Manager from loading
//	the wrong INIT resources, or accidentally running our extension twice!

pascal Handle MyGet1IndResource(ResType, short)
	{
	long oldTrap;
	oldTrap = *(long*)ApplScratch;
	NSetTrapAddress(oldTrap, _Get1IxResource, ToolTrap);
	return NULL;
	}
