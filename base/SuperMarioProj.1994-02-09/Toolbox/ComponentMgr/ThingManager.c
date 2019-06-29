/*
	File:		Thing Manager.c

	Copyright:	© 1990-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	  <SM21>	11/29/93	JDR		When cloning a component, need to preserve the flag determining
									if it's a PowerPC code fragment.
	  <SM20>	 9/23/93	JDR		If ResolveCodeFragment fails, return a nil address. When
									searching the ComponentPlatformInfo, need to increment the
									pointer to the next entry. __LoadComponent needs to test for a
									nil rtRoutineHandle.
	  <SM19>	  8/9/93	JDR		Warnings were not available in SuperMario builds, so after
									fixing that I found the Gestalt proc didn't set the OSErr
									result. Removed some Cyclone ROM hacks from
									RegisterROMComponents.
	  <SM18>	  8/5/93	JDR		integrate component mgr from reality
	   <20+>	 7/26/93	JDR		fix problems
		<20>	 7/21/93	JDR		Needed to move the #if ForROM to always allow the
									_RegisterRomComponents symbol to be compiled into the object
									file (for the LinkPatchTool).
		<19>	 7/21/93	JDR		Added _RegisterRomComponents. Corrected Gestalt interfaces. Used
									kComponentResourceType constant.
		<18>	 7/20/93	DTY		Get rid of some unused variables.
		<17>	 7/16/93	JDR		Added SupportNativeComponents. Fixed InitComponentManager to
									ignore the cut back address.
	  <SM16>	 7/13/93	CSS		Fix conditionals involving POWER_PC_NATIVE_QT since this is a
									feature and will be defined if the feature is off (but will be
									=0)
	  <SM15>	  7/7/93	RC		moved around a couple of the #ifdefs for the native component
									support to kill some compile time warnings
		<14>	  7/2/93	IH		Reality Update: Change Gestalt call to use
									SelectorFunctionProcPtr. Correct prototype of ThingyGestalt to
									take long*.
	  <SM13>	 6/23/93	CSS		Conditionalized RegisterHandleTrashNotification call
	  								out until we figure out why the trap isn't available.
									Update from Reality:
	  								<16>	 6/13/93	JDR		Rolled in the ComponentMgr sources from QuickTime 1.6.
									<15>	 1/20/93	DTY		MFPrivate is now ProcessesPriv.
	  <SM12>	 6/14/93	kc		Roll in Ludwig.
	   <LW3>	  1/4/93	fau		Modified RegisterFromROMResource to set a flag in slot 0 PRAM so
									that components could check whether they are being opened just
									to be registered.  This flag should really be in ExpandMem or
									something but...
	  <SM11>	12/10/92	DH		Updated RegisterComponentROMResource routine  to call
									GetSystemThenROMRes routine to search in gibbley then rom for
									thng associated resources.
	  <SM11> 11/16/92	CSS			Update from Reality:
	  								<14>	11/13/92	JDR		I misspelled kComponentInitialTableEntries.
									<13>	11/12/92	JDR		<PH> rolled in QuickTime 1.5 bug fixes.  Fixed a problem with
																LoadComponent under low memory conditions to not rob the system
																heap beyond the emMinSysHeapFreeSpace amount.  Fixed a
																deferenced handle problem in RegisterComponent.  Created
																kComponentInitialTableEntries which was set to 32 entries in
																System 7.1, but I cut it back to only 16 to help save some RAM
																for the general user.
	  <SM10> 11/10/92	CSS			Update from Reality:
									<12>	10/28/92	DTY		Use new Get/Set macro pairs for ExpandMem access.
	  <SM9>	  7/29/92	CSS			On the last change I forgot to remove a procedure, CheckSysHeapSlop(),
	  								that is no longer called, and other cleanup.
	  <SM8>	  7/14/92	CSS			I missed some of the changes for <11>.
	  <SM7>	  7/7/92	CSS			Update from Reality:
	  								<11>	  7/1/92	JH		#1033507,1033505 <KsT>: Remove check for sysheap slop so real
																components can be opened in the application heap. We will move
																this check to TSM which opens unreal components that like to do
																things like allocate 200K of the system heap for themselves.
	  <SM6>	  7/6/92	fau			Blew away <SM4> to put in the two new routines that Bruce Leak
									wrote to search components in ROM:  RegisterRomComponents
									and RegisterComponentRomResource.  This probably affected <SM3>
									changes.
	   <SM4>	  7/1/92	RB		Make the routine LoadComponent look for resources also in ROM.
									Let resources be found in ROM when the resource file being
									scanned is 0, meaning the system file.
	   <SM3>	 6/29/92	RB		Allow component resources to be in ROM by setting MapTrue in
									ROMMapInsert
		<10>	  2/6/92	JH		#1018985, RegisterComponentResource locks the component resource
									registers it, and then unlocks it. Unfortunately, the
									dereferenced handle is used down below and some memory calls are
									made before it gets used. Changing the code so it just uses the
									handle rather than the dereferenced handle.
		 <9>	12/12/91	DTY		Get rid of the spaces in the file names
		 <8>	 12/6/91	YK		Removed System6 conditionals. Object compare identical to <7>.
		 <7>	 12/5/91	YK		Rolled in QuickTime GM:
										BAL	make it compile
										BAL	return the componentID in the componentFlagsMask field in order
											to guarantee uniqueness from GetComponentInfo
										BAL	When growing the component table, make sure the component
											instance table is at least as big as the component table so that
											if a component ID is used to index into the instance table it
											will look at known signature values which will never match
											instead of random heap blocks which may.
										KIP	put strings and icons in app heap for local components.  also
											start with 32 entry table.
										KIP	don't check for system heap slop if getresource failed.
										KIP	move component high in app heap before locking
										KIP	make sure that rtRoutine is zero when the component is not
											loaded
										BAL	now zeros rtRoutine pointer when the component is unloaded to
											prevent spurious unregistrations.
		 <6>	 12/4/91	YK		Do more reliable checking for the System heap slop.
		 <5>	11/23/91	YK		Check the System heap free space after every open call.
		 <4>	10/30/91	YK		Moved CompopnentSearch call to ComponentMgrInstall proc so that
									InitComponentManager will not search files.
		 <3>	10/29/91	YK		Added missing ComponentSearch call so that 'thng' files in the
									Extensions folder will be registered at startup.  It was lost
									when rolled in.
		 <2>	10/29/91	YK		Conditionalized code for System 6.  Replace them with System 7
									traps.
		<46>	10/15/91	MD		install gestalt proc
		<45>	10/15/91	MD		don't dereference ResLoad twice in RegisterComponentResourceFile
		<44>	10/11/91	BAL		Patch into to RsrcZoneInit to know when zones are going away at
									init time.  Multifinder doesn't call this through the trap.
		<43>	 10/2/91	BAL		Torch components which are registered locally when their a5
									world is destroyed even if they aren't loaded.
		<42>	 10/1/91	BAL		cast everything so peter doesn't have to.
		<41>	 10/1/91	BAL		Added RegisterComponentResourceFile and its support stuff.
		<40>	 9/29/91	PH		stop DestroyComponent from looping forever
		<39>	 9/25/91	BAL		Now happy about loading components in the app heap if there
									isn't enough system heap.
		<38>	 9/21/91	BAL		Deals with low or no system heap better.  Now leaves a free
									space cushion in the system heap.
		<37>	  9/2/91	BAL		Grow default size of instance table to avoid the statistically
									possible crash which festers mark.
		<36>	 8/28/91	BL		make it compile
		<35>	 8/28/91	BAL		Added OpenDefaultComponent for your convenience.
		<34>	 7/24/91	BAL		Change open to set flag for fast dispatch things.
		<33>	 7/10/91	JW		return appropriate errors
		<32>	  7/7/91	BL		Remove compiler warnings
		<31>	  7/2/91	PH		to compile again
		<30>	  7/2/91	JW		Change result of OpenComponentResFile from OSErr to refnum
		<29>	 5/18/91	dvb		Thing->Component
		<28>	  5/6/91	BAL		add a valid thing check to set default thing
		<27>	  5/3/91	MK		fix flag masking in findnextmatchingthing
		<26>	  5/2/91	BAL		Send register message to those who request it.  If loading in
									system heap fails then try app heap--if that succeeds clone off
									a local copy of the thing.
		<25>	 4/24/91	BAL		conditionalized debugger breaks
		<24>	 4/19/91	BAL		fix setDefaultThing return value and
									FindNextCommonThingInstance(0,x,x).
		<23>	 4/18/91	BAL		Got rid of when and self from thingParams.  Now pass self as a
									param to Open and Close.
		<22>	 4/16/91	JB		Moved param defs into proc call so that they will be used as
									prototypes
		<21>	 4/15/91	JB		Fixed testing of flags within SetDefaultThing
		<20>	 4/15/91	JB		SetDefaultThing was passing a struct instead of a ptr to an
									internal routine
		<19>	 4/14/91	BAL		reordered the flags to SetDefaultThing.
		<18>	 4/14/91	BAL
		<18>	 4/14/91	BAL		Replaced some of the Debugger's with DebugStr's.
		<17>	 4/14/91	BAL		Fixed countThings so that it actually works.
		<16>	 4/11/91	MK		fix countthings and do loadresource
		<15>	 4/11/91	MK		fix countthings and do loadresource
		<14>	  4/3/91	dvb		CallThing needed return value to avoid warning.
		<13>	  4/1/91	MK		put rtRefNum and flags mask back to be separate
		<12>	 3/28/91	PH		fixing closething again...
		<11>	 3/28/91	PH		CloseThing was not calling the Thing with storage
		<10>	 3/28/91	KD		Add the dummy proc CallThing{}.
		 <9>	 3/28/91	BAL		Do some file management.  Support Capture, Chain, and Default.
		 <8>	 2/23/91	BAL		Close all things at Re-install time.
		 <7>	 2/21/91	MK		slime some globals for codec manager
		 <6>	 2/18/91	BAL		Fix KillHeapThingInstances to check to see if the instance is
									active first.
		 <5>	 2/16/91	BAL		Added InitThingManager call.
		 <4>	 2/11/91	BAL		Fixed bug in ValidThingInstance which compared against
									#instances used not available.
		 <3>	  2/5/91	MK		fix destroy thing to not remove from list
		 <2>	 1/31/91	MK		fix heap zone in register thing
		 <3>	 12/30/90	bal		changed name to thing manager
		 <2>	 10/06/90	dvb		Make it work

	To Do:
		Proper use of warhol globals
*/

/*   Movies Thing Manager

	by Bruce Leak, David Van Brink, Jim Batson, Mark Krueger, Peter Hoddie
	Copyright 1990 Apple Computer, Inc. All rights reserved.
*/

	#include <Types.h>
	#include <Memory.h>
	#include <OSUtils.h>
	#include <Resources.h>
	#include <Files.h>
	#include <Errors.h>
	#include <GestaltEqu.h>
	#include <SysEqu.h>
	#include <Traps.h>
	#include <MemoryMgrPriv.h>
	#include <FigmentMemory.h>
	#include <ExpandMemPriv.h>
	#include <TrapsPrivate.h>
	#include <LowMem.h>


#ifdef applec
	// #define tLINKED
#endif
#include "PrivateThingManager.h"


#ifdef SupportNativeComponents
#include "FragLoad.h"
#endif


#ifdef applec
extern long OLDPMGR;
extern char PMANAGEREXITCHAIN;
extern long OLDRSRC;
extern char RSRCZONEINITCHAIN;
extern pascal short ThingyGestalt(OSType selector, long* responsePtr);
#endif

#pragma parameter __D0 CoolReplaceGestalt(__D0,__A0)
pascal OSErr CoolReplaceGestalt(OSType selector,SelectorFunctionProcPtr selectorFunction)
    = {0xA5AD};

#pragma parameter __A0 rHandToHand(__A0)
Handle rHandToHand(Handle theHndl)
	= {0xA9E1,0x31c0,0x0220};

pascal void		__InitComponentManager(Ptr *cutBackAddress);
pascal long		__ComponentManagerVersion(void);
pascal OSErr	__GrowRegisteredComponentInstanceTable( short growCount );
pascal OSErr	__GrowRegisteredComponentTable( short growCount );
pascal long		__GetComponentListModSeed(void);
long			RegisteredComponentPointerToComponentID(RegisteredComponent *rt);
RegisteredComponent	*ComponentIDtoRegisteredComponentPointer(long t);
RegisteredComponent	*FindMatchingComponent(ComponentDescription *td, RegisteredComponent *rtList);
pascal long		__CountComponents(ComponentDescription *td);
pascal long		CountSame(RegisteredComponent *new);
RegisteredComponent	*FindPreviousMatchingComponent(ComponentDescription *td,
					RegisteredComponent *rtList);
pascal Component	__RegisterComponent(ComponentDescription *td,
					ComponentRoutine ComponentEntryPoint, short global, Handle ComponentName,
					Handle ComponentInfo, Handle ComponentIcon);
pascal Component	__RegisterComponentResource(ComponentResource **htr, short global);
pascal long		__RegisterComponentResourceFile(short resRefNum, short global);
pascal RegisteredComponentInstancePtr __ValidComponentInstance(ComponentInstance aComponentInstance);
pascal RegisteredComponentPtr	__ValidComponent(Component aComponent);
int				RemoveRTFromList(RegisteredComponent *rt, RegisteredComponent **pRTList);
pascal OSErr	__UnregisterComponent(Component aComponent);
pascal OSErr	__SetDefaultComponent(Component aComponent, short flags);
pascal Component	__CaptureComponent(Component slave, Component master);
pascal OSErr	__UncaptureComponent(Component aComponent);
pascal short	__OpenComponentResFile(Component aComponent);
pascal OSErr	__CloseComponentResFile(short refnum);
RegisteredComponentInstance	*ComponentInstanceIDToComponentInstancePointer(long t);
long			ComponentInstancePointerToComponentInstanceID(RegisteredComponentInstance *rti);
pascal long		__CountComponentInstances(Component aComponent);
pascal RegisteredComponent	*CloneComponent(long componentID, short global);
pascal long		__LoadComponent(long componentID);
pascal OSErr	__UnloadComponent(long componentID);
pascal ComponentInstance	FindNextCommonComponentInstance(ComponentInstance aComponentInstance,
					Component aComponent, ComponentRoutine entryPoint);
pascal OSErr	__DestroyComponent(Component aComponent);
pascal RegisteredComponent *DestroyOlder( register RegisteredComponent *new, long version );
void			KillHeapComponentInstances(Ptr heapStart, Ptr heapEnd);
void			KillHeapComponents(Ptr heapStart, Ptr heapEnd);
pascal void		__CleanUpApplicationComponents(void);
pascal Component	__FindNextComponent(long previousComponent, ComponentDescription *td);
void			HandToXHand(Handle srcHandle, Handle dstHandle);
pascal OSErr	__GetComponentInfoPrivate( Component aComponent,
					Handle  nameHand, Handle infoHand, Handle iconHand );
pascal OSErr	__GetComponentInfo(Component aComponent, ComponentDescription *td,
					Handle nameHand, Handle infoHand, Handle iconHand);
pascal long		__GetComponentRefcon(long aComponent);
pascal void		__SetComponentRefcon(long aComponent, long theRefcon);
pascal OSErr	__GetComponentInstanceError(long aComponentInstance);
pascal void		__SetComponentInstanceError(long aComponentInstance, OSErr theError);
pascal Handle	__GetComponentInstanceStorage(long aComponentInstance);
pascal void		__SetComponentInstanceStorage(long aComponentInstance, Handle theStorage);
pascal long		__GetComponentInstanceA5(long aComponentInstance);
pascal void		__SetComponentInstanceA5(long aComponentInstance, long theA5);
pascal ComponentInstance	__OpenComponent(long componentID);
pascal OSErr	__CloseComponent(ComponentInstance componentInstanceID);
pascal ComponentInstance	__OpenDefaultComponent(OSType componentType, OSType componentSubType);
pascal ComponentResult	__CallComponent(void);
pascal void		__ComponentSearch(void);
RegisteredComponentInstance *FindNextComponentInstance( register RegisteredComponentInstance *rti,  register RegisteredComponent *rt );

static pascal void ThingMemoryTrashed(Ptr startAddr, Ptr stopAddr);

pascal long goComponentOpen( long theComponent, long self )	= ComponentCallNow( kComponentOpenSelect, 4);
pascal long goComponentClose(long theComponent, long self ) = ComponentCallNow( kComponentCloseSelect,4);
pascal long goComponentRegister(long theComponent ) = ComponentCallNow( kComponentRegisterSelect,0);
pascal long goComponentUnregister(long theComponent ) = ComponentCallNow( kComponentUnregisterSelect,0);


// Gestalt proc for returning the version of the installed Component Mgr
pascal OSErr GestaltComponentMgrVersion(OSType selector, long* responsePtr)
{
#pragma unused (selector)

	*responsePtr = ComponentManagerVersion();
	return (noErr);
}

//#ifdef tLINKED

void KillHeapComponents(Ptr heapStart, Ptr heapEnd);

pascal long __ComponentManagerVersion(void)
	{
	return 3;
	}

Handle fixHandle(register Handle h)
{
	if (h == 1) h = 0;
	return h;
}

#ifdef SupportNativeComponents

pascal void * ResolveCodeFragment(RegisteredComponentPtr rt)
{
	Str255			errStr;
	Ptr				mainAddr;
	THz				oldZone;
	OSErr			err;

#ifdef WARHOL_DEBUG
	Handle			nameHand;

	nameHand = NewHandle(0);
	if (nameHand == nil)
		return (0);

	err = GetComponentInfo(RegisteredComponentPointerToComponentID(rt), nil, nameHand, nil, nil);
	if (err != noErr) {
		DebugStr("\pno info");
		DisposeHandle(nameHand);
		goto bail;
	}
	HLock(nameHand);
#endif

	oldZone = GetZone();
	SetZone(SystemZone());

#ifdef WARHOL_DEBUG
	err = GetMemFragment(*rt->rtRoutineHandle,0,*nameHand,kLoadNewCopy,&rt->rtConnectionID,&mainAddr,errStr);
#else
	err = GetMemFragment(*rt->rtRoutineHandle,0,"\pComponent",kLoadNewCopy,&rt->rtConnectionID,&mainAddr,errStr);
#endif

	SetZone(oldZone);

#ifdef WARHOL_DEBUG
	DisposeHandle(nameHand);
#endif

	if (err != noErr) {
#ifdef WARHOL_DEBUG
		DebugStr(errStr);
#endif
		return (nil);
	}

bail:
	return (mainAddr);
}

#endif


pascal OSErr __GrowRegisteredComponentTable( short growCount )
/*
 * Grow the table of registered components.  String the new entries onto the free list.
 * Since the table moved, all absolute references to entries in the registered
 * component table must be relocated--including the rtNext chaining links.
 * Interrupts need not be disabled as long as the old table is kept intact and used until
 * the new one is finished.  If the table were a handle and SetHandleSize were used then
 * interrupts would have to be disabled across the bulk of this routine.
 */
	{
	register RegisteredComponent *aTable;
	RegisteredComponent *oldTable, *newTable, *freeChain;
	long oldTableSize;

	oldTableSize = ComponentManagerGlobals->rtTableEntryCount * sizeof(RegisteredComponent);
	oldTable = ComponentManagerGlobals->rtTable;
	newTable = aTable = (RegisteredComponent *)NewPtrSysClear( oldTableSize  + (long)growCount * sizeof(RegisteredComponent));
#ifdef	WARHOL_DEBUG
	if (!aTable) DebugStr("\pCould not expand component table");
#endif
	if (!aTable)
		return MemError();

	BlockMove((Ptr)oldTable, (Ptr)aTable, oldTableSize);

	{
	register short i;
	register long offset;

		offset = (char *)newTable - (char *)oldTable;
		for(i = 0; i<ComponentManagerGlobals->rtTableEntryCount; i++)	/* relocate old entries' next & parent links */
			{
			if (aTable->rtNext)
				aTable->rtNext = (RegisteredComponent *)((char *)aTable->rtNext + offset);
			if (aTable->rtParent)
				aTable->rtParent = (RegisteredComponent *)((char *)aTable->rtParent + offset);
			aTable++;
			}

		/* Relocate the back pointers in the componentInstanceList as well */
		{
		register RegisteredComponentInstance *rti;

		rti = ComponentManagerGlobals->rtInstanceTable;
		i = ComponentManagerGlobals->rtInstanceTableTotalCount;
		for(--i; i >= 0; --i)
			{
			if (rti->rtiEntry)
				rti->rtiEntry = (RegisteredComponent *)((char *)rti->rtiEntry + offset);
			rti++;
			}
		}

	if (ComponentManagerGlobals->rtUsedChain) 				/* relocate used chain */
		ComponentManagerGlobals->rtUsedChain = (RegisteredComponent *)((char *)ComponentManagerGlobals->rtUsedChain + offset);

	freeChain = ComponentManagerGlobals->rtFreeChain;		/* relocate and save current free chain */
	if (freeChain) freeChain = (RegisteredComponent *)((char *)freeChain + offset);
	ComponentManagerGlobals->rtFreeChain = aTable;			/* start free chain with new entries */

	for(i = 1; i<growCount; i++)							/* chain new entries-1 together on free list */
		{
		aTable->rtNext = aTable+1;
		aTable++;
		}
	aTable->rtNext = freeChain;								/* hook previous free chain on to the end */
	}

	ComponentManagerGlobals->rtTable = newTable;			/* begin using new table */
	ComponentManagerGlobals->rtTableEntryCount += growCount;
	if (oldTable)
		DisposPtr((Ptr)oldTable);							/* dump the old one */

	if (ComponentManagerGlobals->rtTableEntryCount > ComponentManagerGlobals->rtInstanceTableTotalCount)
		GrowRegisteredComponentInstanceTable( ComponentManagerGlobals->rtTableEntryCount -  ComponentManagerGlobals->rtInstanceTableTotalCount);

	return 0;
	}



pascal OSErr __GrowRegisteredComponentInstanceTable( short growCount )
/*
 * Grow the table of registered component instances.
 * Interrupts need not be disabled as long as the old table is kept intact and used until
 * the new one is finished.  If the table were a handle and SetHandleSize were used then
 * interrupts would have to be disabled across the bulk of this routine.
 */
	{
	register RegisteredComponentInstance *aTable;
	RegisteredComponentInstance *oldTable, *newTable;
	long oldTableSize;

	oldTableSize = ComponentManagerGlobals->rtInstanceTableTotalCount * sizeof(RegisteredComponentInstance);
	oldTable = ComponentManagerGlobals->rtInstanceTable;
	newTable = aTable = (RegisteredComponentInstance *)NewPtrSysClear( oldTableSize  + (long)growCount * sizeof(RegisteredComponentInstance));
#ifdef	WARHOL_DEBUG
	if (!aTable) DebugStr("\pCould not expand component instance table");
#endif
	if (!aTable)
		return MemError();

	BlockMove((Ptr)oldTable, (Ptr)aTable, oldTableSize);

	if (!newTable->rtiEntryUseCount) newTable->rtiEntryUseCount = 0x81; /* force nil componentInstances invalid */
	ComponentManagerGlobals->rtInstanceTable = newTable;			/* begin using new table */
	ComponentManagerGlobals->rtInstanceTableTotalCount += growCount;
	if (oldTable)
		DisposPtr((Ptr)oldTable);									/* dump the old one */
	return 0;
	}


#define BumpModificationSeed( pSeed )  (*(pSeed))++

/*
void BumpModificationSeed( register long *pSeed )
	{
	register long ticks;

	ticks = TickCount();
	if (*pSeed >= ticks )
		*pSeed++;
	else
		*pSeed = ticks;
	}
*/

pascal long __GetComponentListModSeed( )
	{
	return ComponentManagerGlobals->rtModSeed;
	}


long RegisteredComponentPointerToComponentID(register RegisteredComponent *rt)		/* should be a macro */
	{																				/* don't let valid components be negative */
	return rt ? ( rt - ComponentManagerGlobals->rtTable + ((long)rt->rtEntryUseCount<<16) ) : 0;
	}


#if 1
	#define ComponentIDtoRegisteredComponentPointer(t) ((RegisteredComponent *)( ComponentManagerGlobals->rtTable + (short)(t & 0xffff)))
#else
	RegisteredComponent *ComponentIDtoRegisteredComponentPointer( register long t )			/* should be a macro */
	{
	return ( ComponentManagerGlobals->rtTable + (short)(t & 0xffff) );
	}
#endif


RegisteredComponent *FindMatchingComponent( ComponentDescription *td, register RegisteredComponent *rtList )
/*
 * Find the RegisteredComponent that matches the ComponentDescription provided;
 * If, in the "looking" record, a zero is passed
 * for Type, subType, Manufacturer or flags, then they are wildcards.
 *
 * Returns componentID or nil if not found.
 */
	{
	 register long mask, flags;

	 mask = td->componentFlagsMask;
	 flags = td->componentFlags & mask;

	while ((rtList)	&& (
				 ((rtList->rtDesc.componentType != td->componentType)  && td->componentType)
				|| (td->componentSubType 		&& (rtList->rtDesc.componentSubType != td->componentSubType))
				|| (td->componentManufacturer 	&& (rtList->rtDesc.componentManufacturer != td->componentManufacturer))
				|| ((rtList->rtDesc.componentFlags & mask) != flags)
				|| (rtList->rtLocalA5			&& (rtList->rtLocalA5 != *(long *)CurrentA5))
				|| (rtList->rtFlags & (rtfCaptured+rtfChild))  )  )
		{
		rtList = rtList->rtNext;
		}

	return rtList;
	}



pascal long __CountComponents( register ComponentDescription *td )
/*
  */
	{
	register RegisteredComponent *rt = ComponentManagerGlobals->rtUsedChain;
	register long count = 0;

	while ( (rt=FindMatchingComponent(td, rt)) ) {
		rt = rt->rtNext;
		count++;
	}

	return count;
	}


pascal long CountSame( register RegisteredComponent *new )
/*
  */
	{
	register RegisteredComponent *rtList = ComponentManagerGlobals->rtUsedChain;
	register long count = 0;

	while (rtList)
		{
		if (
			(rtList->rtDesc.componentType			== new->rtDesc.componentType) &&
			(rtList->rtDesc.componentSubType		== new->rtDesc.componentSubType) &&
			(rtList->rtDesc.componentManufacturer	== new->rtDesc.componentManufacturer) &&
			(rtList->rtDesc.componentFlags			== new->rtDesc.componentFlags) &&
	//		(rtList->rtFileNumber					== new->rtFileNumber) &&
			(rtList->rtResourceID					== new->rtResourceID) &&
			(rtList->rtResourceType					== new->rtResourceType) &&
			(rtList->rtLocalA5						== new->rtLocalA5)
		   )
		   count++;

		rtList = rtList->rtNext;
		}

	return count;
	}




RegisteredComponent *FindPreviousMatchingComponent( register ComponentDescription *td, register RegisteredComponent *rtList )
/*
 * Find the RegisteredComponent in the rtList that logically precedes the ComponentDescription provided;
 *
 */
	{
	 register RegisteredComponent *prev = nil;

	while ((rtList) && ((rtList->rtDesc.componentType < td->componentType)
						|| ((rtList->rtDesc.componentType == td->componentType)
							&& ((rtList->rtDesc.componentSubType < td->componentSubType)
								|| ((rtList->rtDesc.componentSubType == td->componentSubType)
						&&  ((rtList->rtDesc.componentManufacturer < td->componentManufacturer)
		  )                   )   )   )   )   )
		{
		prev = rtList;
		rtList = rtList->rtNext;
		}

	return prev;		/* return pointer to one before where it belongs */
	}


static Handle cmHandToHand(register Handle h)
{
	if ((long)(h == 0) || ((long)h == 1))
		return h;
	else {
#ifdef WARHOL_DEBUG
	if (!*h) DebugStr("\pyou suck.");
#endif
		return rHandToHand(h);
	}
}

pascal short __ResolveComponentPlatform(ComponentResource **htr, ComponentDescription *cd, ResourceSpec *rs)
{
	ComponentResourceExtension *cre;
	long response = gestalt68k;
	ComponentPlatformInfoArray *cpia;
	ComponentPlatformInfo *cpi;
	short count;
	short platformType = 0;

	// set up the default return stuff
	if (cd) *cd = (**htr).cd;
	if (rs) *rs = (**htr).component;

	// bail if no extension present
	if (GetHandleSize((Handle)htr) < (sizeof(ComponentResource) + sizeof(ComponentResourceExtension)))
		goto bail;

	cre = (ComponentResourceExtension *)((long)*htr + sizeof(ComponentResource));

	// bail if no platform information
	if (!(cre->componentRegisterFlags & componentHasMutliplePlatforms))
		goto bail;

	// bail if no way to determine system architecture
	if (Gestalt(gestaltSysArchitecture, &response) != noErr)
		goto bail;

	// cycle through the array of platform descriptions looking for a match
	cpia = (ComponentPlatformInfoArray *)((long)*htr + sizeof(ComponentResource) + sizeof(ComponentResourceExtension));
	count = cpia->count;
	cpi = cpia->platformArray;

	while (count--)
		{
		if (cpi->platformType == response)
			{
			if (cd)
				cd->componentFlags = cpi->componentFlags;
			if (rs)
				*rs = cpi->component;
			platformType = response;
			goto bail;
			}
		cpi++;
		}

bail:
	// If the platform type is still unknown, then assume this means the 68k.
	// But if there is a resType for the component's code, then it means it
	// has 68k type code available and we need to update the platform type.

	if (platformType == 0)
		{
		if ((**htr).component.resType != 0)
			platformType = gestalt68k;
		}
	return (platformType);
}

pascal Component __RegisterComponent(ComponentDescription *td, ComponentRoutine ComponentEntryPoint, short global,
			 Handle ComponentName, Handle ComponentInfo, Handle ComponentIcon)
/*
 * Register a new Component with the Component manager.
 * Returns index of RegisteredComponent or zero if registration failed.
 */
	{
	THz	saveZone;
	register RegisteredComponent *new, *previous;
	ComponentDescription atd = *td;									// duplicate so that if it moves (or gets trashed...) we still have it

	if (!ComponentManagerGlobals->rtFreeChain) {					/* no free entries available */
		if (GrowRegisteredComponentTable( kComponentAllocationSize))	/* expand (and MOVE…) the table */
			return 0;												/* out of memory */
	}

	new = ComponentManagerGlobals->rtFreeChain;
	ComponentManagerGlobals->rtFreeChain = new->rtNext;
	new->rtDesc = atd;
	new->rtRoutine = (ComponentRoutine *)StripAddress((Ptr)ComponentEntryPoint);
	new->rtLocalA5 = (long)((global & registerComponentGlobal) ? 0 : *(long *)CurrentA5 );

	saveZone = (THz)GetZone();

	if (global & registerComponentGlobal)
		SetZone(*(THz *)SysZone);
	new->rtNameHand = cmHandToHand( ComponentName );
	new->rtInfoHand = cmHandToHand( ComponentInfo );
	new->rtIconHand = cmHandToHand( ComponentIcon );

	SetZone((THz)saveZone);
	if (global & registerComponentAfterExisting)			/* register after other similar ones */
		atd.componentType++;
														/* find the entry ordered before this one */
	if  ( previous = FindPreviousMatchingComponent( &atd, ComponentManagerGlobals->rtUsedChain ) )
		{
		new->rtNext = previous->rtNext;						/* add this one to the ordered chain */
		previous->rtNext = new;
		}
	else
		{													/* no one before this one */
		new->rtNext = ComponentManagerGlobals->rtUsedChain;	/* chain it in at the head of the list */
		ComponentManagerGlobals->rtUsedChain = new;
		}

	if (global & registerComponentAfterExisting)			/* restore component type */
		atd.componentType--;

	BumpModificationSeed( &ComponentManagerGlobals->rtModSeed );
	if (!new->rtEntryUseCount)
		new->rtEntryUseCount++;	/* zero use count not valid */

	new->rtParent = 0;
	new->rtRoutineHandle = 0;
	new->rtFlags = 0;

	// the upper byte is the platform type, but 0 means unknown and assumed to be 68k
	if (((unsigned short)global >> 8) > gestalt68k)
		new->rtFlags |= rtfCodeFragment;
	new->rtDesc.componentFlagsMask = 0;						/* re-use this field: rt->rtRefcon */
	new->rtFileNumber = -1;									/* flag as not a resource */
	new->rtResourceType = 0;							/* flag as not a resource even more */
	return RegisteredComponentPointerToComponentID( new );	/* return  index with useCount in highword */
	}




pascal Component __RegisterComponentResource(ComponentResource **htr, short global)
/*
 * Register a new Component based on component resource with the Component manager.
 * Returns index of RegisteredComponent or zero if registration failed.
 */
	{
	long		result = 0;
	char		saveState;
	register 	ComponentResource *tr;
	Handle		componentRoutine;
	short	resID;
	OSType	resType;
	Str255	wastedName;
	long	version = 0;
	short	platformType;

	GetResInfo((Handle)htr, &resID, &resType, wastedName);			// don't need the name under system 7
	if (result = ResError()) return 0;

	saveState = HGetState((Handle)htr);
	HLock((Handle)htr);
	tr = *htr;

	platformType = ResolveComponentPlatform(htr, &tr->cd, nil);
	if (platformType == 0)
		{
		HSetState((Handle)htr, saveState);
		return componentDontRegister;
		}

	result = RegisterComponent(&tr->cd,  (ComponentRoutine )0L,  (global & 0x00FF) | (platformType << 8),
						nil, nil, nil);

	HSetState((Handle)htr, saveState);

	if (result)
		{
		register RegisteredComponent *new = ComponentIDtoRegisteredComponentPointer(result);

		new->rtNameHand = (Handle)(tr->componentName.resType != 0);
		new->rtInfoHand = (Handle)(tr->componentInfo.resType != 0);
		new->rtIconHand = (Handle)(tr->componentIcon.resType != 0);

		// deal with the compoent resource extension, if present
		//	note: this is done before any calls that might move memory and therefore purge the resource
		if (GetHandleSize((Handle)htr) >= (sizeof(ComponentResource) + sizeof(ComponentResourceExtension)))
			{
			ComponentResourceExtension *cre = (long *)((long)*htr + sizeof(ComponentResource));

			version = cre->componentVersion;

			if (cre->componentRegisterFlags & componentDoAutoVersion) {
				new->rtFlags |= rtfAutoVersion;
				if (cre->componentRegisterFlags & componentAutoVersionIncludeFlags)
					new->rtFlags |= rtfAutoVersionUseFlags;
			}

			if (cre->componentRegisterFlags & componentWantsUnregister)
				new->rtFlags |= rtfWantsUnregister;

			if (cre->componentIconFamily)
				new->rtFlags |= rtfHasIconSuite;
			}

		new->rtFileNumber = AddComponentResFile( (Handle)htr );
		if ( new->rtFileNumber < 0)
			{
#ifdef	WARHOL_DEBUG
			Debugger();						/* should never fail (can fail if no memory and couldn't get a file id)*/
#endif
			UnregisterComponent(result);
			return 0;
			}
		componentRoutine = NewHandleSys(0);
		EmptyHandle(componentRoutine);

		new->rtResourceID = resID;
		new->rtResourceType = resType;
		new->rtRoutineHandle = componentRoutine;
		new->rtRoutine = 0L;

		/* check for duplicates if necessary */

		if ( (global & registerComponentNoDuplicates) && (CountSame(new)>1) )
			{
			UnregisterComponent(result);
			return 0;
			}

		// check for register message

		if (( new->rtDesc.componentFlags & kComponentNeedsRegisterKiss ) )
			if (goComponentRegister((ComponentInstance)result))
				{
				UnregisterComponent(result);
				return 0;
				}

		// scan the Component table for possible duplicates and kill the older one(s) (if auto versioning on)
		// (can't do this before the register message since we might call GetComponentVersion on the component,
		//	which it might not be able to deal with before the register)

		if (DestroyOlder(new, version) == nil)
			{
			// we are too old. don't register.
			UnregisterComponent(result);
			return 0;
			}
		}

	return	result;
	}


pascal long __RegisterComponentResourceFile(short resRefNum, short global)
/*
 * Register components found in this resource file.
 */
	{
	short saveRef;
	short err;
	Boolean oldResLoad;
	Handle hdl;

	oldResLoad = *(Boolean *)ResLoad;
	saveRef = CurResFile();

	SetResLoad(true);
	UseResFile(resRefNum);

	if ((err=ResError()) == noErr)
	{
		register short count, i;

		if ((count = Count1Resources(kComponentResourceType)) > 0)
		{
			for (i=1; i<=count; i++)
			{
				hdl = Get1IndResource(kComponentResourceType, i);
				if (hdl)
					{
					if (RegisterComponentResource((ComponentResource **)hdl, global))
						err++;						/* we registered something */
					}
			}
		}
	}

	UseResFile(saveRef);
	SetResLoad(oldResLoad);

	return err;
	}





pascal RegisteredComponentInstancePtr __ValidComponentInstance(register ComponentInstance aComponentInstance)
	{
	register long signature;
	register RegisteredComponentInstance *result;

	signature = aComponentInstance>>16;

	if ( (aComponentInstance <=0) || (!signature) ) 		/* valid ComponentID's are > 0 */
		return false;

	aComponentInstance &= 0xffff;

	if ( aComponentInstance < 0 || aComponentInstance > ComponentManagerGlobals->rtInstanceTableTotalCount
			|| signature != (result = ComponentManagerGlobals->rtInstanceTable + aComponentInstance)->rtiEntryUseCount )
		return 0;
	else
		return result;
	}



pascal RegisteredComponentPtr __ValidComponent(register Component aComponent)
	{
	register long signature;
	register RegisteredComponent *result;
	register RegisteredComponentInstance *vti;

	signature = aComponent>>16;

	if ( ( aComponent <=0 ) || (!signature) ) 				/* valid ComponentID's are > 0 */
		return 0;

	aComponent &= 0xffff;

	if ( aComponent >= 0 && aComponent < ComponentManagerGlobals->rtTableEntryCount
			&& signature == (result = ComponentManagerGlobals->rtTable + (short)aComponent)->rtEntryUseCount )
		return result;
	else
		{
		if (vti = ValidComponentInstance(aComponent + (signature<<16)))
			return vti->rtiEntry;
		}
	return 0;
	}



int RemoveRTFromList( register RegisteredComponent *rt, RegisteredComponent **pRTList )
	{
	register RegisteredComponent *rtList;
	register RegisteredComponent *prev = nil;

	if (!(rtList = *pRTList))
		return -1;							/* error:  empty list */
	while ((rtList) && (rtList != rt))
		{
		prev = rtList;
		rtList = rtList->rtNext;
		}

	if (!rtList)
		return -1;							/* error:  not found in list */

	if (!prev)
		*pRTList = rt->rtNext;				/* we are removing the head */
	else
		prev->rtNext = rt->rtNext;			/* we are removing from the middle */

	return 0;
	}



pascal OSErr __UnregisterComponent(Component aComponent)
/*
  *	Returns an error if aComponent is currently open
  *
  */
	{
	register RegisteredComponent *rt;

	if( ! (rt = ValidComponent(aComponent)) )
		return invalidComponentID;						/* error : not a valid component ID */

	if ( rt->rtReferenceCount )							/* error: valid instances exist */
		return validInstancesExist;

	if ( (rt->rtFlags & rtfWantsUnregister) && (rt->rtFlags & rtfHasBeenOpened) )
		goComponentUnregister(aComponent);

	if ( RemoveRTFromList( rt, &ComponentManagerGlobals->rtUsedChain ) )
		return invalidComponentID;						/* error: not found in list */

	rt->rtNext = ComponentManagerGlobals->rtFreeChain;	/* add this entry to the free list */
	ComponentManagerGlobals->rtFreeChain = rt;

	if (rt->rtNameHand != 1) DisposHandle(rt->rtNameHand);
	if (rt->rtInfoHand != 1) DisposHandle(rt->rtInfoHand);
	if (rt->rtIconHand != 1) DisposHandle(rt->rtIconHand);

		DisposHandle(rt->rtRoutineHandle);

	RemoveComponentResFile( rt->rtFileNumber );			/* decrement the use count */

	if (rt->rtParent)									/* if child, unchain from parent */
														/* note: this also unchains a parent from a child. this can happen, if
															the component is loaded into the application heap for the
															register message, and the component doesn't want to be
															registered. */
		{
		register RegisteredComponent *previous;

		previous = rt->rtParent;
		while (previous->rtParent != rt)
			previous = previous->rtParent;				/* find previous node */

		if (previous == rt->rtParent)
			previous->rtParent = 0;						/* no more children */
		else
			previous->rtParent = rt->rtParent;			/* take us out of chain */
		}

	BumpModificationSeed( &ComponentManagerGlobals->rtModSeed );	/* even bump for children in case someone cached it */

	rt->rtEntryUseCount++;
	if (!(rt->rtEntryUseCount &= 0x7f)) 	rt->rtEntryUseCount++;	/* ensure entry counts wrap and are nonzero*/
	return 0;
	}



pascal OSErr __SetDefaultComponent( Component aComponent, register short flags )
	{
	register RegisteredComponent *rt;
	register RegisteredComponent *previous;
	ComponentDescription td;

	if( ! (rt = ValidComponent(aComponent)) )
		return invalidComponentID;							/* error : not a valid component ID */

	GetComponentInfo( aComponent, &td, 0L, 0L, 0L );

	if (flags & defaultComponentAnySubType)
		td.componentSubType = 0;
	if (flags & defaultComponentAnyManufacturer)
		td.componentManufacturer = 0;
	if (flags & defaultComponentAnyFlags)
		td.componentFlags = 0;
	td.componentFlagsMask = 0;

	if ( RemoveRTFromList( rt, &ComponentManagerGlobals->rtUsedChain ) )
		{
#ifdef	WARHOL_DEBUG
		Debugger();											/* error: should not happen */
#endif
		return invalidComponentID;
		}

															/* find the entry ordered before this one */
	if  ( previous = FindPreviousMatchingComponent( &td, ComponentManagerGlobals->rtUsedChain ) )
		{
		rt->rtNext = previous->rtNext;						/* add this one to the ordered chain */
		previous->rtNext = rt;
		}
	else
		{													/* no one before this one */
		rt->rtNext = ComponentManagerGlobals->rtUsedChain;	/* chain it in at the head of the list */
		ComponentManagerGlobals->rtUsedChain = rt;
		}
	return 0;
	}


pascal Component __CaptureComponent( Component slave, Component master )
	{
	register RegisteredComponent *s, *m;

	if( ! (s = ValidComponent(slave)) )
		return invalidComponentID;							/* error : not a valid component ID */

	if( ! (m = ValidComponent(master)) )
		return invalidComponentID;							/* error : not a valid component ID */

	if (s->rtFlags & rtfCaptured)
		{
#ifdef	WARHOL_DEBUG
		DebugStr("\pComponent has already been captured");	/* already captured !!! */
#endif
		return 0;
		}

	s->rtFlags |= rtfCaptured;
	BumpModificationSeed( &ComponentManagerGlobals->rtModSeed );
	return slave;
	}


pascal OSErr __UncaptureComponent( Component aComponent )
	{
	register RegisteredComponent *rt;

	if( ! (rt = ValidComponent(aComponent)) )
		return invalidComponentID;							/* error : not a valid component ID */

	if (!(rt->rtFlags & rtfCaptured)) 					/* not captured !!! */
		return componentNotCaptured;

	rt->rtFlags &= ~rtfCaptured;
	BumpModificationSeed( &ComponentManagerGlobals->rtModSeed );
	return 0;
	}


pascal short __OpenComponentResFile( Component aComponent )
	{
	register RegisteredComponent *rt;
	register short fn;
	short	 saveResLoad;
	short	 result;
	rtFile	*rtFileList;
	FSSpec	theFile;

	if( ! (rt = ValidComponent(aComponent)) )
		return -1;									/* error : not a valid component ID */

	fn = rt->rtFileNumber;
	if ((fn < 0) || (fn >= ComponentManagerGlobals->rtFileTotalCount))
		return -1;									/* no file here */

	// figure out where the file is
	rtFileList = ComponentManagerGlobals->rtFileTable + fn;
	if (rtFileList->vRefNum == 0)
		theFile = **(FSSpec **)rtFileList->fileID;
	else
		{
		HParamBlockRec pb;

		theFile.name[0] = 0;
		pb.fidParam.ioNamePtr = theFile.name;
		pb.fidParam.ioVRefNum = rtFileList->vRefNum;
		pb.fidParam.ioFileID = rtFileList->fileID;
		if (PBResolveFileIDRefSync(&pb) != noErr)
			return -1;
		theFile.vRefNum = rtFileList->vRefNum;
		theFile.parID = pb.fidParam.ioSrcDirID;
		}

	saveResLoad = *(Boolean *)ResLoad;
	SetResLoad(false);
	result  = FSpOpenResFile( &theFile, fsRdPerm );
	SetResLoad(saveResLoad);
	return result;

	}


pascal OSErr __CloseComponentResFile( short refnum )
	{
	if (refnum>0) {
		CloseResFile( refnum );
		return ResError();
		}
	return 0;

	}




RegisteredComponentInstance *ComponentInstanceIDToComponentInstancePointer( register long t )		/* should be a macro */
	{
	return ( ComponentManagerGlobals->rtInstanceTable + (t & 0xffff) );
	}




long ComponentInstancePointerToComponentInstanceID(register RegisteredComponentInstance *rti)		/* should be a macro */
	{
	return rti ? ( rti - ComponentManagerGlobals->rtInstanceTable + ((long)rti->rtiEntryUseCount<<16) ) : 0;
	}




RegisteredComponentInstance *FindNextComponentInstance( register RegisteredComponentInstance *rti,  register RegisteredComponent *rt )
	{
	register short i;

		i = ComponentManagerGlobals->rtInstanceTableTotalCount;

	if (!rti)
		rti = ComponentManagerGlobals->rtInstanceTable;
	else
		{
		rti++;
		i -= rti - ComponentManagerGlobals->rtInstanceTable;
		}

	for(--i; i >= 0; --i)
		{
		if (rt == rti->rtiEntry)
			return rti;
		rti++;
		}

	return 0;										/* not found */
	}


pascal long __CountComponentInstances(Component aComponent)
/*
  *
  */
	{
	register RegisteredComponent *rt;

	if( ! (rt = ValidComponent(aComponent)) )
		return 0;									/* error : not a valid component ID */

	return (rt->rtReferenceCount);
	}



pascal RegisteredComponent* CloneComponent( long componentID, short global )
/*
 */
	{
	register RegisteredComponent *rt, *nt;
	long	newComponent;
	Handle componentRoutine;
	long saveSeed;

	if( ! (rt = ValidComponent(componentID)) )
		return 0;									/* error : not a valid component ID */

	saveSeed = ComponentManagerGlobals->rtModSeed;

	if (rt->rtFileNumber >= 0)
		newComponent = RegisterComponent(&rt->rtDesc, rt->rtRoutine, global, (Handle)(rt->rtNameHand != 0), (Handle)(rt->rtInfoHand != 0), (Handle)(rt->rtIconHand != 0));
	else
		newComponent = RegisterComponent(&rt->rtDesc, rt->rtRoutine, global, rt->rtNameHand, rt->rtInfoHand, rt->rtIconHand);

	if (newComponent)
		{
		rt = ComponentIDtoRegisteredComponentPointer(componentID);		/* it may have moved !!! */
		nt = ComponentIDtoRegisteredComponentPointer(newComponent);
		nt->rtResourceID = rt->rtResourceID;
		nt->rtResourceType = rt->rtResourceType;
		nt->rtFileNumber = rt->rtFileNumber;

		(ComponentManagerGlobals->rtFileTable[rt->rtFileNumber]).referenceCount++;

		componentRoutine = NewHandle(0);
		EmptyHandle(componentRoutine);
		nt->rtRoutineHandle = componentRoutine;
		nt->rtRoutine = 0L;

		if (!global)
			{
			nt->rtFlags |= rtfChild;						/* mark as child -- hide from findnext */
			nt->rtFlags |= (rt->rtFlags & (rtfAutoVersion|rtfHasIconSuite|rtfWantsUnregister|rtfAutoVersionUseFlags|rtfCodeFragment));	// transfer obscure flags
			nt->rtLocalA5 = *(long *)CurrentA5;				/* mark for this app only */
			ComponentManagerGlobals->rtModSeed = saveSeed;	/* seed must not change */
			if (rt->rtParent)
				{
				nt->rtParent = rt->rtParent;				/* insert the new one into the list */
				rt->rtParent = nt;
				}
			else
				{
				rt->rtParent = nt;							/* chain them together */
				nt->rtParent = rt;
				}
			}
		}
	else
		nt = 0;

	return  nt;
	}



pascal long __LoadComponent( long componentID )
/*
 */
	{
	register RegisteredComponent *rt, *child = 0;
	Handle	 entryPoint = nil;
	THz		saveZone;
	short	refnum;

	if( ! (rt = ValidComponent(componentID)) )
		return 0;									/* error : not a valid component ID */

	if (rt->rtParent)								/* check for children in this app */
		{
		child = rt->rtParent;
		while ( (child!=rt) && (child->rtLocalA5 != *(long *)CurrentA5) )
			child = child->rtParent;

		if (child==rt)
			child = 0;
		else if (*child->rtRoutineHandle)
			rt = child;								/* we found a child and it is loaded */
		}

	if (rt->rtFileNumber>=0)
		{											/* resource file exists */
		if (!rt->rtReferenceCount)					/* don't bump count until success */
			{
			if (!rt->rtRoutineHandle || !*rt->rtRoutineHandle)		/* code is purged, so go get it */
				{
				short saveCurRes = CurResFile();
				ResourceSpec resSpec;
				ComponentResource **htr;

				refnum = OpenComponentResFile( componentID );
				if (refnum == -1)
					return 0;						/* could not open the file */

				// load the thing resource so that we can figure out where the code resource lives
				htr = (ComponentResource **)Get1Resource(rt->rtResourceType, rt->rtResourceID);
				if (!htr) goto closeDownFile;
				LoadResource((Handle)htr);
				if (!*htr) goto closeDownFile;

				// we ignore the result, since it has already be determined to be correct
				ResolveComponentPlatform(htr, nil, &resSpec);
				ReleaseResource((Handle)htr);

				saveZone = (THz)GetZone();
				if (!rt->rtLocalA5)
					SetZone(*(THz *)SysZone);
				if (entryPoint = Get1Resource(resSpec.resType, resSpec.resID))
					{
					LoadResource(entryPoint);
					DetachResource(entryPoint);
					if (!rt->rtLocalA5)
						{
						// try to keep components from being cloned at boot time so they can set up their
						//	globals in the system heap cleanly. this is somewhat misguided and mostly
						//	intended to keep ill-behaved components from dying miserably.

						if (GetExpandMemProcessMgrExists()) {
							if (!EnoughSystemHeapSlop())
								EmptyHandle(entryPoint);					/* not enough space, so dump our handle */
							}
						}
					}
				SetZone(saveZone);

#ifdef	WARHOL_DEBUG
				if (!entryPoint || !*entryPoint)  DebugStr("\pFailed to load component in sys heap.;g");
#endif
				/* If we couldn't put it in the System heap, try the App heap */

				if ((!entryPoint || !*entryPoint) && (!rt->rtLocalA5))
					{
					entryPoint = Get1Resource(resSpec.resType, resSpec.resID);
					LoadResource(entryPoint);
					DetachResource(entryPoint);
					if (entryPoint && *entryPoint)						/* we got it */
						{
						HNoPurge( entryPoint );							/* done let clone purge it */
						if (child)
							rt = child;									/* refill the existing private copy */
						else
							rt = CloneComponent(componentID, false); 	/* make a private copy */

						if (!rt)
							{
#ifdef	WARHOL_DEBUG
						if (!rt)  DebugStr("\pClone component failed.");
#endif
							DisposHandle(entryPoint);
							entryPoint = nil;						/* dump it, we'll exit soon */
						}
					}
					}
closeDownFile:
				CloseComponentResFile(refnum);
				UseResFile(saveCurRes);

				if (!entryPoint || !*entryPoint)
					return 0;											/* not enough memory */

				DisposHandle( rt->rtRoutineHandle );					/* dump the purged handle */
				rt->rtRoutineHandle = entryPoint;

				}
			if (rt->rtLocalA5)
				MoveHHi( rt->rtRoutineHandle );					/* if it's in the app heap then force it high */
			else
				{
				if (ComponentManagerGlobals->hasMoveHLo)
					MoveHLow(rt->rtRoutineHandle);				// if it's in the system heap, force it low if possible
				}
			HLock( rt->rtRoutineHandle );							/* lock it down */
#ifdef SupportNativeComponents
			if(rt->rtFlags & rtfCodeFragment)
			{
				rt->rtRoutine = ResolveCodeFragment(rt);
				if (rt->rtRoutine == nil)
				{
					DisposHandle( rt->rtRoutineHandle );		// error, bail out
					rt->rtRoutineHandle = nil;					// forget about this
					return 0;
				}
			}
			else
#endif
			rt->rtRoutine = (void *)StripAddress( *rt->rtRoutineHandle );
			}
		}

	rt->rtReferenceCount++;												/* one more client */
	return	(long)rt;
	}

pascal OSErr __UnloadComponent( long componentID )
/*
 */
	{
	register RegisteredComponent *rt;
#ifdef SupportNativeComponents
	THz oldZone;
#endif
	OSErr result;

	result = noErr;
	if( ! (rt = ValidComponent(componentID)) )
		return invalidComponentID;				/* error : not a valid component ID */

	rt->rtReferenceCount--;						/* one fewer client */
	if (rt->rtFileNumber>=0)
		{										/* resource file exists */
		if (!rt->rtReferenceCount)
			{									/* recover handle and make it purgeable */
			rt->rtRoutine = 0L;					/* no longer valid */

#ifdef SupportNativeComponents
			if (rt->rtFlags & rtfCodeFragment)
				{
				oldZone = GetZone();
				SetZone(SystemZone());
				result = CloseConnection(&rt->rtConnectionID);
				SetZone(oldZone);
#ifdef WARHOL_DEBUG
				if (result != noErr) DebugStr("\pCloseConnection failed");
#endif
				}
#endif
			HUnlock(rt->rtRoutineHandle);
			HPurge(rt->rtRoutineHandle);
			}
		}
	return	result;
	}


#if 0

pascal ComponentInstance FindNextCommonComponentInstance(ComponentInstance aComponentInstance, Component aComponent, ComponentRoutine entryPoint )
/*
  *
  */
	{
	register RegisteredComponentInstance *rti = 0;
	register RegisteredComponent *rt;

	if (!(rt = ValidComponent(aComponent)) || (rt->rtRoutine != (void *)StripAddress((Ptr)entryPoint)) )
		return 0;

	if (aComponentInstance)
		{
		if ( !(rti = ValidComponentInstance(aComponentInstance)) || ( rti->rtiEntry->rtRoutine != (void *)StripAddress((Ptr)entryPoint) ))
			return 0;						/* error : not a valid component ID or entry point doesn't match */
		}

	return ComponentInstancePointerToComponentInstanceID(FindNextComponentInstance(rti, rt));
	}

#endif


pascal OSErr __DestroyComponent(Component aComponent)
/*
  *	semi-Private routine for unregistering a component that is currently open
  *
  */
	{
	register RegisteredComponent *rt;
	register RegisteredComponentInstance *rti;

	if( ! (rt = ValidComponent(aComponent)) )
		return invalidComponentID;					/* error : not a valid component ID */


	while (rt->rtReferenceCount)
		{
		rti = FindNextComponentInstance( 0L, rt );	/* start at beginning */
		if (!rti) {
#ifdef	WARHOL_DEBUG
			Debugger();								/* should never happen */
#endif
			rt->rtReferenceCount = 0;				/* force all instances to be gone.... */
			break;
			}
		CloseComponent( ComponentInstancePointerToComponentInstanceID(rti) );
		}

	return UnregisterComponent(aComponent);
	}


long getComponentVersion(register RegisteredComponent *rt)
	{
	long version = 0;
	short fref;
	Component t = RegisteredComponentPointerToComponentID(rt);

	fref = OpenComponentResFile(t);
	if (fref != -1)
		{
		ComponentResource **htr;

		// load the component resource to see if the version number is available
		htr = (ComponentResource **)Get1Resource(rt->rtResourceType, rt->rtResourceID);
		if (htr)
			{
			LoadResource((Handle)htr);
			if (*htr)
				{
				if ( GetHandleSize((Handle)htr) >= (sizeof(ComponentResource) + sizeof(ComponentResourceExtension)) )
					{
					ComponentResourceExtension *cre = (long *)((long)*htr + sizeof(ComponentResource));
					version = cre->componentVersion;
					}
				}
			}
		CloseComponentResFile(fref);
		if (version)
			return version;
		}

	// that failed, so just ask the Component
	version = GetComponentVersion(t);
	if (version < 0)
		version = 0;

	return version;
	}


pascal RegisteredComponent *DestroyOlder( register RegisteredComponent *new, long version )
/*
  */
	{
	register RegisteredComponent *rtList = ComponentManagerGlobals->rtUsedChain;

	while (rtList)
		{
		if (
			(rtList->rtDesc.componentType			== new->rtDesc.componentType) &&
			(rtList->rtDesc.componentSubType		== new->rtDesc.componentSubType) &&
			(rtList->rtDesc.componentManufacturer	== new->rtDesc.componentManufacturer) &&
			(rtList->rtLocalA5						== new->rtLocalA5) &&
			((rtList->rtFlags & rtfAutoVersion) || (new->rtFlags & rtfAutoVersion)) &&
			(rtList != new)
		   )
		   {
		   long thisVersion;
		   Component c;

			if (
				((rtList->rtFlags & rtfAutoVersionUseFlags) || (new->rtFlags & rtfAutoVersionUseFlags)) &&
				(rtList->rtDesc.componentFlags	!= new->rtDesc.componentFlags)
			   )
				goto next;

			// found one that's the same
			thisVersion = getComponentVersion(rtList);
			c = RegisteredComponentPointerToComponentID(rtList);

			rtList = rtList->rtNext;				// get the next one (assumes UnregisterComponent never kills more than one Component)

			if (!version)
				version = getComponentVersion(new);

#ifdef WARHOL_DEBUG
			DebugStr("\pDeath to the old.");
#endif
			if (thisVersion <= version)
				{
				// kill the other one, let us live
				DestroyComponent(c);
				}
			else
				{
				// we are older than this one, so kill us
				return nil;
				}
		   }
		   else
		   	{
next:
			rtList = rtList->rtNext;
			}
		}

	return new;
	}


void KillHeapComponentInstances(register Ptr heapStart, register Ptr heapEnd)
	{
	register RegisteredComponentInstance *rti;
	register short i;

	rti = ComponentManagerGlobals->rtInstanceTable;
	i = ComponentManagerGlobals->rtInstanceTableTotalCount;

	for(--i; i >= 0; --i)
		{
		if ( (rti->rtiEntry) && ((Ptr)rti->rtiStorage >= heapStart) && ((Ptr)rti->rtiStorage <= heapEnd) )
			CloseComponent( ComponentInstancePointerToComponentInstanceID(rti) );
		rti++;
		}
	}



void KillHeapComponents(register Ptr heapStart, register Ptr heapEnd)
	{
	register RegisteredComponent *rt = ComponentManagerGlobals->rtUsedChain;
	register RegisteredComponent *rt2;

	while (rt)
		{
		rt2 = rt;
		rt=rt->rtNext;
		if ((((Ptr)rt2->rtRoutine >= heapStart) && ((Ptr)rt2->rtRoutine <= heapEnd)) ||
			((Ptr)rt2->rtRoutineHandle >= heapStart) && ((Ptr)rt2->rtRoutineHandle <= heapEnd) ||
			(((Ptr)rt2->rtLocalA5 >= heapStart) && ((Ptr)rt2->rtLocalA5 <= heapEnd)))
			DestroyComponent( RegisteredComponentPointerToComponentID(rt2) );
		}
	}

pascal void ThingMemoryTrashed(Ptr startAddr, Ptr stopAddr)
{
	KillHeapComponentInstances(startAddr, stopAddr);
	KillHeapComponents(startAddr, stopAddr);
}


pascal void __CleanUpApplicationComponents(void)
	{
#if 0
	// if we use the strange MemoryTrashed zone notification stuff, then this is not necessary
	Ptr zoneBegin, zoneEnd;

	GetApplZoneBounds(&zoneBegin, &zoneEnd);
	KillHeapComponentInstances(zoneBegin, zoneEnd);
	KillHeapComponents(zoneBegin, zoneEnd);
#endif
	}


pascal Component __FindNextComponent( long previousComponent, ComponentDescription *td )
/*
  * If, in the "looking" record, a zero is passed
  * for Type, subType, Manufacturer or flags, then they are wildcards.
  *
  * If previousComponent is nil then find the first such component.
  */
	{
	register RegisteredComponent *rt;


	if( previousComponent && (rt = ValidComponent(previousComponent)) )
		rt = rt->rtNext;								/* valid component ID, so start with successor */
	else
		rt = ComponentManagerGlobals->rtUsedChain;		/* start from beginning */


	return RegisteredComponentPointerToComponentID(FindMatchingComponent( td,  rt ));	/* return ID or nil */
	}



void HandToXHand(register Handle srcHandle, register Handle dstHandle)
	{
	register long size;

	if (dstHandle)
		{
		srcHandle = fixHandle(srcHandle);
		if (srcHandle && *srcHandle)
			{
			size = GetHandleSize(srcHandle);
			SetHandleSize( dstHandle, size );
			if (MemError() == -109)
				ReallocHandle(dstHandle, size);			// we return empty handles, so let the pass them to us
			if (! MemError())
				{
				BlockMove( *srcHandle, *dstHandle, size );
				return;
				}
			}
		EmptyHandle( dstHandle );				/* return empty handle if src is nil or empty */
		}
	}



pascal OSErr __GetComponentInfoPrivate( Component aComponent, Handle  nameHand, Handle infoHand, Handle iconHand )
	{
	register RegisteredComponent *rt = ValidComponent(aComponent);

	if ( rt->rtResourceType) {
		Boolean force = false;

		if (nameHand && (rt->rtNameHand == 1))	{rt->rtNameHand = 0; force = true;}
		if (infoHand && (rt->rtInfoHand == 1))	{rt->rtInfoHand = 0; force = true;}
		if (iconHand && (rt->rtIconHand == 1))	{rt->rtIconHand = 0; force = true;}

		 if  ( force ||
			(nameHand && rt->rtNameHand && !*rt->rtNameHand) ||
			  (infoHand && rt->rtInfoHand && !*rt->rtInfoHand) ||
			   (iconHand && rt->rtIconHand && !*rt->rtIconHand) ) {
				// reload the whole lot
			THz saveZone = (THz)GetZone();
			short saveRes = CurResFile();
			short resRef;

			SetZone(*(THz *)SysZone);

			resRef = OpenComponentResFile(aComponent);
			if (resRef != -1) {
				Boolean saveResLoad = *(Boolean *)ResLoad;
				register RegisteredComponent *thisRT = ComponentManagerGlobals->rtUsedChain;

				SetResLoad(true);

				for (thisRT = ComponentManagerGlobals->rtUsedChain; thisRT && (thisRT->rtDesc.componentType <= rt->rtDesc.componentType); thisRT = thisRT->rtNext) {
					Handle h;

					if ((thisRT->rtFileNumber != rt->rtFileNumber) ||
						(thisRT->rtDesc.componentType != rt->rtDesc.componentType))
						continue;

					h = Get1Resource(thisRT->rtResourceType, thisRT->rtResourceID);
					if (h) {
						ComponentResource cr = **(ComponentResource **)h;

						ReleaseResource(h);

						if (nameHand && cr.componentName.resType) {
							DisposHandle(fixHandle(thisRT->rtNameHand));
							if (thisRT->rtNameHand = Get1Resource(cr.componentName.resType, cr.componentName.resID)) {
								HNoPurge(thisRT->rtNameHand);
								DetachResource(thisRT->rtNameHand);
							}
						}
						if (infoHand && cr.componentInfo.resType) {
							DisposHandle(fixHandle(thisRT->rtInfoHand));
							if (thisRT->rtInfoHand = Get1Resource(cr.componentInfo.resType, cr.componentInfo.resID)) {
								HNoPurge(thisRT->rtInfoHand);
								DetachResource(thisRT->rtInfoHand);
							}
						}
						if (iconHand && cr.componentIcon.resType) {
							DisposHandle(fixHandle(thisRT->rtIconHand));
							if (thisRT->rtIconHand = Get1Resource(cr.componentIcon.resType, cr.componentIcon.resID)) {
								HNoPurge(thisRT->rtIconHand);
								DetachResource(thisRT->rtIconHand);
							}
						}

						if (thisRT != rt) {
							HPurge(fixHandle(thisRT->rtNameHand));
							HPurge(fixHandle(thisRT->rtInfoHand));
							HPurge(fixHandle(thisRT->rtIconHand));
						}
					}
				}

				SetResLoad(saveResLoad);
				CloseComponentResFile(resRef);
			}

			SetZone(saveZone);
			UseResFile(saveRes);
		}
	}

	HandToXHand(rt->rtNameHand, nameHand);
	HandToXHand(rt->rtInfoHand, infoHand);
	HandToXHand(rt->rtIconHand, iconHand);

	if (rt->rtResourceType) {
		HPurge(fixHandle(rt->rtNameHand));
		HPurge(fixHandle(rt->rtInfoHand));
		HPurge(fixHandle(rt->rtIconHand));
	}

	return noErr;
}

pascal void ClearD0(void)
	= 0x7000;

pascal OSErr __GetComponentInfo( Component aComponent, register ComponentDescription *td, Handle  nameHand, Handle infoHand, Handle iconHand )
/*
  */
	{
	register RegisteredComponent *rt;
	OSErr err;

	if ( rt = ValidComponent(aComponent))
		{
		if (td)
			{
			register RegisteredComponent *rtTemp = rt;

			*td = rt->rtDesc;					/* copy type, subtype, manufacturer, flags */
			while (rtTemp->rtFlags & rtfChild)
				{
				rtTemp = rtTemp->rtParent;
#ifdef WARHOL_DEBUG
				if (!rtTemp)
					DebugStr("\pClonedComponentHell");
#endif
				}

			td->componentFlagsMask = RegisteredComponentPointerToComponentID(rtTemp);
			}								/* return component ID for uniqueness */


		if (nameHand || infoHand || iconHand)
			err = GetComponentInfoPrivate(aComponent, nameHand, infoHand, iconHand);
		else
			err = noErr;
		}
	else
		err = invalidComponentID;								/* error : not a valid component ID */

	ClearD0();						//•• this makes the 7.1 Keyboard menu code not crash (fun, huh?)
	return err;
	}



pascal long __GetComponentRefcon( long aComponent )
/*
 */
	{
	register RegisteredComponent *rt;

	if (rt = ValidComponent(aComponent))
		{
		return rt->rtDesc.componentFlagsMask;	/* re-use this field: rt->rtRefcon */
		}
	else
		return 0;								/* error : not a valid component ID */
	}



pascal void __SetComponentRefcon( long aComponent, long theRefcon )
/*
 */
	{
	register RegisteredComponent *rt;

	if( rt = ValidComponent(aComponent) )
		rt->rtDesc.componentFlagsMask = theRefcon;		/* re-use this field: rt->rtRefcon */
	}



pascal OSErr __GetComponentInstanceError( long aComponentInstance )
/*
 */
	{
	register RegisteredComponentInstance *rti;
	short err;

	if( ! (rti = ValidComponentInstance(aComponentInstance)) )
		return invalidComponentID;				/* error : not a valid component ID */

	err = rti->rtiError;
	rti->rtiError=0;
	return err;
	}


pascal void __SetComponentInstanceError( long aComponentInstance, OSErr theError )
/*
 */
	{
	register RegisteredComponentInstance *rti;

	if( rti = ValidComponentInstance(aComponentInstance) )
		rti->rtiError = theError;
	}


pascal Handle __GetComponentInstanceStorage( long aComponentInstance )
/*
 */
	{
	register RegisteredComponentInstance *rti;

	if( ! (rti = ValidComponentInstance(aComponentInstance)) )
		return 0;								/* error : not a valid component ID */

	return rti->rtiStorage;
	}


pascal void __SetComponentInstanceStorage( long aComponentInstance, Handle theStorage )
/*
 */
	{
	register RegisteredComponentInstance *rti;

	if( rti = ValidComponentInstance(aComponentInstance) )
		rti->rtiStorage = theStorage;
	}


pascal long __GetComponentInstanceA5( long aComponentInstance )
/*
 */
	{
	register RegisteredComponentInstance *rti;

	if( ! (rti = ValidComponentInstance(aComponentInstance)) )
		return 0;								/* error : not a valid component ID */

	return rti->rtiSwitchedA5;
	}


pascal void __SetComponentInstanceA5( long aComponentInstance, long theA5 )
/*
 */
	{
	register RegisteredComponentInstance *rti;

	if( rti = ValidComponentInstance(aComponentInstance) )
		rti->rtiSwitchedA5 = theA5;
	}


pascal ComponentInstance __OpenComponent( long componentID )
/*
 */
	{
	register RegisteredComponent *rt;
	register RegisteredComponentInstance *rti;
	ComponentInstance ti;
	long	result;

	if( ! (rt = (RegisteredComponent*)LoadComponent(componentID) ) )
		{
#ifdef	WARHOL_DEBUG
		DebugStr("\pFailed to load component.  Not enough memory.;g");
#endif
		return 0;
		}

	if (( ComponentManagerGlobals->rtInstanceTableUsedCount >= ComponentManagerGlobals->rtInstanceTableTotalCount )
	     && (GrowRegisteredComponentInstanceTable( kComponentInstanceAllocationSize )))
		return 0;									/* error: could not allocate an instance record */

	if (! (rti = FindNextComponentInstance( 0L, 0L)))/* start at beginning and look for a nil component ptr */
		return 0;									/* error: should never happen */

	ComponentManagerGlobals->rtInstanceTableUsedCount++;

	rt->rtFlags |= rtfHasBeenOpened;			// remember that this one has been used

	rti->rtiEntry = rt;								/* store pointer to the parent registered component */
	rti->rtiStorage = 0;							/* set high bit of flags with component flags  bit 30  */
	rti->rtiFlags = (((~rt->rtDesc.componentFlags) >> 23) & 0x80);
	rti->rtiError = 0;
	rti->rtiSwitchedA5 = rt->rtLocalA5;
	if (!rti->rtiEntryUseCount)
		rti->rtiEntryUseCount = 0x81;				/* distinguish between componentID's and componentInstances */

	/***** Call the Component with the OPEN message Here *******/

	ti = ComponentInstancePointerToComponentInstanceID(rti);
	result = goComponentOpen( ti, ti );				/* 4 bytes of params, selector open = -1 */

	if (result)									/* error returned from open */
		{
#ifdef	WARHOL_DEBUG
		DebugStr("\pOpen component failed.");
#endif
		CloseComponent(ti);
		return 0;
		}
	return ti;

	}


pascal OSErr __CloseComponent( ComponentInstance componentInstanceID )
/*
 */
	{
	register RegisteredComponent *rt;
	register RegisteredComponentInstance *rti;
	ComponentInstance ti;
	long	result;

	if( ! (rti = ValidComponentInstance(componentInstanceID)) )
		return invalidComponentID;						/* error : not a valid component ID */

	/***** Call the Component with the CLOSE message Here *******/
	ti = ComponentInstancePointerToComponentInstanceID(rti);
	result = goComponentClose( ti , ti);				/* 4 bytes of params, selector close = -2 */

	UnloadComponent( componentInstanceID );				/* Drops reference count */
	rt = rti->rtiEntry;
	ComponentManagerGlobals->rtInstanceTableUsedCount--;

	rti->rtiEntry = 0;									/* Invalidate entry; kill pointer to the parent registered component */
	rti->rtiEntryUseCount++;							/* invalidate all stale references */
	if (!(rti->rtiEntryUseCount &= 0x7f))	rti->rtiEntryUseCount++;	/* ensure entry counts wrap and are nonzero*/
	rti->rtiEntryUseCount |= 0x80;						/* flag as a componentInstance */

	return result;

	}



pascal ComponentInstance __OpenDefaultComponent( OSType componentType, OSType componentSubType )
/*
 */
	{
	Component cmp = 0;
	ComponentInstance result = 0;
	ComponentDescription look;

	look.componentType = componentType;
	look.componentSubType = componentSubType;
	look.componentManufacturer = 0;
	look.componentFlags = 0;
	look.componentFlagsMask = 0;

	while (!result) {
		if (!(cmp = FindNextComponent(cmp, &look)))
			return 0;
		result = OpenComponent(cmp);
		}
	return result;
	}



// this is only here because of CubeE which links it in…
pascal void __ComponentSearch (void)
{
	doComponentSearch();
}
//#endif

/********************************

	everything below here is cut back
	by after InitComponentManager is called.

********************************/

pascal void __InitComponentManager(Ptr *cutBackAddress)	// Called only once at install time
	{
#pragma unused (cutBackAddress)

	long saveCodecManagerGlobals = -1;

#if 0
	*cutBackAddress = (Ptr)&_InitComponentManager;		// for QuickTime use only
#endif

	RegisterHandleTrashNotification(ThingMemoryTrashed, kComponentManagerPriority, 0);

	if ( (long)ComponentManagerGlobals != -1) {			// don't install if already there (but always return cutback address)
		// 7.1 calls GetComponentManagerVersion via JSR, not a trap
		CoolReplaceGestalt(gestaltComponentMgr,GestaltComponentMgrVersion);
		goto bail;
	}

	NewGestalt(gestaltComponentMgr, GestaltComponentMgrVersion);

	ComponentManagerGlobals = (ComponentManagerGlobalsRecord *)NewPtrSysClear( sizeof(ComponentManagerGlobalsRecord) );
	ComponentManagerGlobals->rtReservedForCodecManager = saveCodecManagerGlobals;
	GrowRegisteredComponentTable( 32 );
bail:
	ComponentManagerGlobals->hasMoveHLo = (GetOSTrapAddress(_MoveHLow) != GetToolboxTrapAddress(_Unimplemented));
	}


pascal ComponentResult __CallComponent(void) 		/* referenced by dispatch table--but never called */
	{
	return 0;
	}

Handle GetSystemThenROMRes(ResType theType, short resIndex)
{
	Handle 		H;
	short		saveResFile;

	/*Do a quick check to see if we are looking for a valid resource*/
		if (theType == 0 ) return(nil);

	/*First look for the resource in the system file*/
	saveResFile = CurResFile();
	UseResFile(0); /*set to the system resource file*/
	H = Get1Resource(theType, resIndex);
	UseResFile(saveResFile); /*restore resource file */
	if (H != nil) return(H); /*If we got a resource then just return with it*/

	/*If we didnt find a resource, then try looking for it in ROM*/
	LMSetROMMapInsert(mapTrue);
	H = Get1Resource(theType, resIndex);

	return(H); /*return system resource file if found */

}

pascal Component RegisterComponentRomResource(ComponentResource **htr, short global)
/*
 * Register a new Component based on component resource.
 * Returns index of RegisteredComponent or zero if registration failed.
 * Resource must refer to Component Code and associated resources in ROM.
 */
	{
	long		result = 0;
	char		saveState;
	ComponentResource	*tr;
	Handle		H1, H2, H3, H4;

	saveState = HGetState((Handle)htr);
	HLock((Handle)htr);
	tr = *htr;


	H1 = GetSystemThenROMRes(tr->componentName.resType, tr->componentName.resID);
	H2 = GetSystemThenROMRes(tr->componentInfo.resType, tr->componentInfo.resID);
	H3 = GetSystemThenROMRes(tr->componentIcon.resType, tr->componentIcon.resID);
	H4 = GetSystemThenROMRes(tr->component.resType, tr->component.resID);
	result = RegisterComponent(&tr->cd,  (ComponentRoutine )*H4,  global, H1, H2, H3);
	HSetState((Handle)htr, saveState);
	ReleaseResource(H1);
	ReleaseResource(H2);
	ReleaseResource(H3);
	ReleaseResource(H4);	/* The code is actually still around since it's in ROM */

	if (result)
		{
		if (( tr->cd.componentFlags & kComponentNeedsRegisterKiss ) )
			if (goComponentRegister((ComponentInstance)result))
				{
				UnregisterComponent(result);
				return 0;
				}
		}
	}

pascal void __RegisterRomComponents(void)
/*
 * Register components found in the ROM resource file.
 */
	{
#if ForROM
// don't need this code after booting, so the system or init versions shouldn't need
// to carry it around. But this entry point does need to be here.

	Boolean oldResLoad;
	Handle hdl;
	short		count, i;

	oldResLoad = ResLoad;
	SetResLoad(true);

	LMSetROMMapInsert(mapTrue);
	if ((count = Count1Resources(kComponentResourceType)) > 0)
	{
		for (i=1; i<=count; i++)
		{
			LMSetROMMapInsert(mapTrue);
			hdl = Get1IndResource(kComponentResourceType, i);
			if ( hdl )
				RegisterComponentRomResource((ComponentResource **)hdl, true);
		}
	}

	SetResLoad(oldResLoad);
#endif
}

