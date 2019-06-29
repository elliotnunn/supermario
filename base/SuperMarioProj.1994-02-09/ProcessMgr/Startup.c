/*
	File:		Startup.c

	Contains:	Main startup for the Process Manager.

	Written by:	Erich Ringewald

	Copyright:	© 1986-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	  <SM28>	 7/21/93	joe		Back out <SM27>.
		<27>	10/28/92	DTY		Use new Get/Set macros to access ExpandMem.
		<26>	 9/25/92	DRF		Get rid of MyGestalt, since inline glue exists
		<25>	 8/26/92	DTY		Change ModSquad’s funky conditional to use #ifdefs like everyone
									else.
		<24>	 8/26/92	DTY		Roll in latest changes for A/UX.
		<23>	 8/17/92	hjcr	Adding support for OCE's "defer drag" feature.  Conditionally
									compiled under MODSQUAD
		<22>	 5/11/92	JSM		#1029437 <DC>: Set emProcessMgrExists to true after calling
									MyNewGestalt().
		<21>	 3/30/92	DTY		#1025416,<DDG>: Look for Skia in InitConfig and set skiaExists
									accordingly.
		<20>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		<19>	 1/11/92	YK		Initialize new globals for the TSM.
		<18>	11/22/91	DTY		Initialize allocateGMBlkInSysHeap to be false. GMBlk normally
									tries to allocate it’s block in the Process Manager zone first,
									except when an 'obit' event is being sent.
		<17>	 11/1/91	DTY		Set TOPMAPHANDLE to point to the first system override map
									instead of SYSMAPHANDLE in main().
		<16>	10/15/91	YK		Added initializing code for the Text Service Manager.
		<15>	 9/16/91	DFH		Set initialProcessSR in InitializeScheduler. Needed for
									NuKernel.
		<14>	 5/23/91	dba		do a style of coercion that MPW 3.2 C likes
		<13>	 1/28/91	DFH		JSM,#81425: Removed inhibit_DAs mechanism.
		<12>	 1/15/91	DFH		(VL) Conditionalize out FloatAUXUp.
		<11>	 1/14/91	DFH		(dba) Make system mode per process.
		<10>	 1/14/91	DFH		(JDR) Conditionalize out AUX support.
		 <8>	  1/8/91	DFH		(rdd) Got rid of pErrantProcess since it is no longer needed.
		 <7>	12/15/90	DFH		Main now initializes srcMyScrapInfo.pScrapOwner.
		 <6>	12/14/90	DFH		InitializeScheduler now initializes kernelMode.
		 <5>	12/13/90	JWM		For 7.0B3, sending the AppleEvent message to the network
									extension (Network Setup) in the shell program (Finder 7.0)
									about the PPCToolbox not initializing was removed.
		 <4>	 12/5/90	DFH		Integrated AUX support.
		 <3>	11/26/90	DFH		Change trespass() to dbmsg().
		 <0>	  9/2/86	ELR		New Today.

*/

#pragma segment INIT

#include <types.h>
#include <memory.h>
#include <menus.h>
#include <files.h>
#include <quickdraw.h>
#include <events.h>
#include <resources.h>
#include <scrap.h>
#include <osutils.h>
#include <retrace.h>
#include <fonts.h>
#include <dialogs.h>
#include <textedit.h>
#include <segload.h>
#include <errors.h>
#include <GestaltEqu.h>
#include <EPPC.h>
#include <AppleEventsInternal.h>
#include <MFPrivate.h>
#include <ResourceMgrPriv.h>

#include "Lomem.h"
#include "SysMisc.h"
#include "Glue.h"
#include "Data.h"
#include "OSDispatchPrivate.h"
#include "Patches.h"
#include "Aux.h"
#include "Puppet.h"
#include "HList.h"
#include "Zone.h"
#include "EppcPrivate.h"
#include "AppleEventExtensions.h"

/* Some function prototypes that should be in (yet another) header file */
void			MyNewGestalt(void);
void			InitExceptions(void);
void			GetSwitchTab(void);
void			InitSystemMenus(void);
void			VNInit(void);
void			FMKill(Boolean);

/* Function prototypes internal to this file */
void			InitializeEnvironment(void);
void			InitializeScheduler(void);
void			InitConfig(void);
void			FloatMainUp(void);
void			FloatKernelUp(void);
void			FloatEppcUp(void);
void			FloatAUXUp(void);

main(void)
	{
	LaunchResults			finderLaunchResults;
	Handle					ourScrapHdl;
	HighLevelEventRecord	theEvent;
	FSSpec					finderSpec;

	_DataInit();							/* Initialize our global data */
	InitializeEnvironment();				/* Adjust machine environment to our liking */
	InstallPatches();						/* Install our patches */
	InitializeScheduler();					/* Initialize our global structures */

	/* Locate Process Mgr's scrap handle BEFORE launches */
	ourScrapHdl = SCRAPHANDLE;

	/* In case BG app dies before we launch FINDERNAME */
	pShellProcess = ((Ptr) -1);

	/* Launch startup background applications */
	if ((EXTENSIONSENABLEBYTE & (1 << ExtensionsEnabledBit)) != 0)
		LaunchFacelessTasks();

	THEZONE = ProcessMgrZone;
	if ((finderLaunchResults.LaunchError = FSMakeFSSpec(BOOTVOL, 0, FINDERNAME, &finderSpec)) == noErr)
		StandardLaunch(&finderSpec, launchUseMinimum, 0, nil, &finderLaunchResults, nil);

	/* Quick Restore:  Have to set zone to sys to do _UnloadSeg correctly */
	THEZONE = SYSZONE; CURMAP = SYSMAP; TOPMAPHANDLE = GetOverrideMap(SYSMAPHANDLE);

	/* Bomb box if the Finder launch failed */
	if (finderLaunchResults.LaunchError != noErr)
		SysError(dsFinderErr);

	/* Remember who we launched from this special place of honor */
	pShellProcess = finderLaunchResults.pNewProcess;
	pShellProcess->p_dadProcess = nil;
	ShellWasFinderType = IS_FINDER(pShellProcess);

	/* Dump scrap since we don't use it */
	if (ourScrapHdl != nil)
		DisposHandle(ourScrapHdl);

	/* Next Process Mgr call should use Process Mgr stack as kernel stack */
	kernelstack = initCurStackBase;

	/* Prepare to execute Finder */
	coercionState = CS_INIT;
	srcMyScrapInfo.pScrapOwner = pFrontProcess = pShellProcess;
	pFrontProcess->p_state = PRRUN;

	/* Unload initialization code before leaving, and also
	 * remove the handle from the system res map.
	 * NOTE: This leaves us running in a dead code segment.  But the current
	 * Resource Manager and Memory Manager let us get away with this
	 * for now.  All we need to do is make the switch_task call, since its
	 * code is in another segment, and will never return.
	 */
	{
	CodeEntry		*pCodeEntry;	/* Needed explicitly because of C compiler bug */

	pCodeEntry = InitializeEnvironment;
	UnloadSeg(pCodeEntry);
	ReleaseResource(GET_SCOD_HDL_FROM_FNC_PTR(pCodeEntry));
	}

	/* Send Finder AppleEvents a) to say that machine was just booted, and b) to give a
	 * message to the network extension if PPC wouldn't initialize.
	 */
	SetAppleEvent(aeStartup, &theEvent);
	(void) BeginSystemMode();
	(void) PostHighLevelEvent((EventRecord *) &theEvent, (u_long) &pShellProcess->p_serialNumber, 0, nil, 0, receiverIDisPSN);

	/* For 7.0B3 the message to the network extension in the shell program about the
	 * PPCToolbox not initializing was removed.  No one to support Responder, so it
	 * was rewritten.
	 */

	(void) EndSystemMode();

	/* Switch to the Finder we launched, never to return (hopefully) */
	switch_task(pFrontProcess);
	dbmsg("Illegal return to the Process Manager.");
	}


/* Dummy routines to force load of CODE segments */
#pragma segment Main

void
FloatMainUp(void)
	{
	}

#pragma segment kernel_segment

void
FloatKernelUp(void)
	{
	}

#pragma segment eppc_segment

void
FloatEppcUp(void)
	{
	}

#ifdef HAS_AUX_PROCESSMGR
#pragma segment AUX_SEGMENT

/* null procedure so that segment can get loaded appropriately */
void FloatAUXUp(void)
	{
	}
#endif HAS_AUX_PROCESSMGR

#pragma segment INIT

/* This is the size we give the system heap at startup.  It's exact value is somewhat
 * unimportant.  It mainly determines how many times we'll have to call the System
 * zone or Process Mgr zone GrowZone procedures before things settle out.  Make sure
 * it's small enough that the initial Finder launch goes smoothly.
 */
#define INIT_SYS_HEAP_SIZE	(96*1024)

/* InitializeEnvironment.  Set up the initial world (i.e. heap, qd globals, etc.). */
void
InitializeEnvironment(void)
	{
	CodeEntry					*pCodeEntry;
	register OSDispatchEntry	*pDispatchEntry;

	/* Set up our own lomem A5 ptr */
	PROCESSMGRGLOBALS = CURRENTA5;

	/* Save initial location zero before we call any traps that might change it! */
	initLocationZero = LOCATIONZERO;

	/* Get configuration information */
	InitConfig();

	/* Get table of switchable low memory locations (in System Heap) */
	GetSwitchTab();

	/* Expand the zone up to the APPLLIMIT */
	MaxApplZone();

	/* initialize the scrap, if necessary */
	THEZONE = SYSZONE;
	if (SCRAPSTATE == (-1))
		(void) ZeroScrap();
	THEZONE = APPLZONE;

	/* Unload data init seg and release its CODE resource (to get it out of map). */
	pCodeEntry = _DataInit;
	UnloadSeg(pCodeEntry);
	ReleaseResource(GET_SCOD_HDL_FROM_FNC_PTR(pCodeEntry));

	/* Set up dynamic heaps */
	InitHeaps();

	THEZONE = SYSZONE;

	/* Load in the code segments now that zone expansion code is in place */
	FloatMainUp();
	FloatKernelUp();
	FloatEppcUp();
#ifdef HAS_AUX_PROCESSMGR
	if (AUXIsPresent)
		FloatAUXUp();
#endif HAS_AUX_PROCESSMGR

	/* Alter dispatch table to go directly to routine, bypassing the jump table */
	pDispatchEntry = &OSDispatchTable[0];
	while (pDispatchEntry->dispatchAddr != nil)
		{
		pDispatchEntry->dispatchAddr = ROUTINE_ADDR(pDispatchEntry->dispatchAddr);
		pDispatchEntry += 1;
		};

	/* Make sure PROCESSMGRGLOBALS world is reasonable */
	InitGraf(&qd.thePort);
#ifdef MM_DEBUG
	{
	pascal void			DebugNotifyPurge(void);
	extern Ptr			oldPurgeProc;

	oldPurgeProc = (Ptr)SYSZONE->purgeProc;
	SYSZONE->purgeProc = (ProcPtr)DebugNotifyPurge;
	}
#endif MM_DEBUG

	/* Initialize window manager in system heap ('cause it's shared in our patch)
	 * NOTE: Calls InitFonts for InitWindow's sake on older machines.
	 */
	InitFonts();
	InitWindows();
	MFLayer = GetCurLayer();
	IMLayer	= nil;
	THEZONE = APPLZONE;

#ifdef HAS_AUX_PROCESSMGR
	/* Adjust ourselves to running under AUX */
	if (AUXIsPresent)
		AUX_Setup();
#endif HAS_AUX_PROCESSMGR

	/* We want to run from the boot volume. */
	SetVol(nil, BOOTVOL);

	/* Set up volume notification mechanism */
	VNInit();
	}

/* GetKernelMode.  Inline to fetch the processor's SR and save it sans all bits
 * except the supervisor/user mode one.
 */
#pragma parameter GetKernelMode(__A0)
void
GetKernelMode(short *storage)
	= {0x40C0, 0x0240, 0x2000, 0x3080};
/*	MOVE       SR,D0
 *	ANDI.W     #$2000,D0
 *	MOVE.W     D0,(A0)
 */

/* InitializeScheduler.  Now that our machine environment looks good, prepare our data
 * structures so we can run applications.
 */
void
InitializeScheduler(void)
	{
	PEntryPtr			pProc;
	Handle				hdl;
	u_size				currSysHeapSize;
	extern StringPtr	SSSPromptpStr, MacOSPortNamepStr;

	/* Initialize these before calling InitExceptions() because the exception
	 * handlers will want to look at them.
	 */
	pProc = pCurrentProcess = pNullProcess;

	/* Install our exception handlers */
	InitExceptions();

	/* initialize the process list globals */
	ProcessNumberSource = LowestNonSystemPSN;
	ProcessIDSource = MAX_PROCESSID;
	pProcessList = nil;
	pFrontProcess = pNullProcess;

	/* we are taking over the nullprocess */
	MemClear(pProc, sizeof(PEntry));
	pProc->p_state = PRNULL;
	pProc->p_signature = NULLPROC_SIGNATURE;
	pProc->p_version = SYS_VERSION;
	SetPSN(kSystemProcess, &pProc->p_serialNumber);
	pProc->p_zone = THEZONE;
	pProc->p_size = (unsigned long) BUFPTR - (unsigned long) THEZONE;

	/* Save useful startup values */
	initialProcessSR = GetCPUStatusRegister() & (kCPUSupervisorMode | kCPUMasterStack);
	initMemTop = MEMTOP;
	initFSQueueHook = FSQUEUEHOOK;

	/* Get the stack for kernel use set up. */
	initCurStackBase = CURSTACKBASE;
	kernelstack = nil;
	kernelstackbase = (unsigned long) APPLZONE->bkLim;
	GetKernelMode(&kernelMode);

 	/* Because app zone can't grow anymore */
	kernelbusy = 0;

	/* initialize the state lists */
	pSleepingStateList = nil;
	pDyingStateList = nil;

	/* Now init some of the more mundane things. */
	pDebugProcess = nil;
	debugKeyTryCount = -1;			/* Based on -1 as start */
	pSystemQuitAllMsg = nil;
	pLastBGProcess = nil;
 	nbacks = 0;
	cutCopyCount = 0;
	dont_switch = 0;
	napOver = false;
	(void) GetDeskPort((GrafPtr *) &desklayer);
	desklayerowner = nil;
	SystemTempMemHList = CreateHList(0);
	
	/* Allocate handle for supporting the Text Service Manager */
	fakeKeyHandle = (FakeKeyHandle)NewHandle( 0);
	fakeKeyCount = 0;
	dontGetFakeKey = 0;
	tsmLayerOwner = nil;
	routeEvent = false;

	/* Get all the resources we need.  Do the Process Mgr ones first.  Wait to do system
	 * ones until we have THEZONE == SYSZONE.  See note below.
	 */
	THEZONE = SYSZONE;

	/* Get menu strings for scrap coercion */
	hdl = Get1Resource('STR#',-16414);
	pStrListCopyNamePrefix = (hdl == nil) ? nil : *((StrListHdl)hdl);

	/* Get the strings for ePPC  */
	SSSPromptpStr = nil;
	MacOSPortNamepStr = nil;
	if ((hdl = Get1Resource('STR#',-16457)) != nil)
		{
		SSSPromptpStr = *hdl+(Ptr)2;
		MacOSPortNamepStr = (Ptr)SSSPromptpStr + (Ptr)(*(char *)SSSPromptpStr);
		MacOSPortNamepStr += (Ptr)1;
		}

	/* <18> This semaphore is only true when an 'obit' event is being sent from
		SendDeathNotice.  Set it to false to start off. */
	allocateGMBlkInSysHeap = false;	
	
	/* Make sure there is some minimum sys heap size so we won't call
	 * the grow zone proc so many times to launch Finder.
	 */
	currSysHeapSize = (Ptr)SYSZONE->bkLim - (Ptr)SYSZONE;
	if (currSysHeapSize < INIT_SYS_HEAP_SIZE)
		(void)ProcessMgrSysGZProc(INIT_SYS_HEAP_SIZE - currSysHeapSize);

	/* Initialize the puppet strings */
	PuppetStringsInit();

	/* Get rid of old system resources that were in the system heap */
	THEZONE = APPLZONE;
	RsrcZoneInit();

	/* Switch to SYSZONE because any of the _GetResource calls below could move
	 * memory within the call, which is a problem for the rsrc mgr routine
	 * CheckLoad().  It saves and restores THEZONE, but our heap can move so restoring
	 * the "original" value is incorrect.
	 */
	THEZONE = SYSZONE;
	{
	/* Get the standard MDEF for the system menus.  We must do this because some apps
	 * (like Finder!) aren't picky about what they get.  We need a good version!
	 */
	long				mdef = 'MDEF';
	short				mdefIndex = 0;
	long				mdefVersionID = 0;
	Handle				realMDefHdl;

#define SMALLEST_GOOD_MDEF_ID (13)

	while (mdefVersionID < SMALLEST_GOOD_MDEF_ID)
		{
		if ((realMDefHdl = GetResource(mdef, mdefIndex)) == nil)
			{
			/* Couldn't find any tables */
			assert(RESERR == memFullErr);
			SysError(dsMemFullErr);
			}

		/* NOTE:  Create struct for this */
		mdefVersionID = *((unsigned long *)(*((Handle)realMDefHdl) + 8));
		mdefIndex++;
		}

	standardMenuDefProcRsrc = (Handle) StripAddress(realMDefHdl);
	}

	/* Insert the system menus now that we have the MDEF */
	InitSystemMenus();

	/* Get rid of useless font width tables, etc.  Must be done after InitSystemMenus,
	 * because menu installers need font info set up.
	 */
	FMKill(false);

	/* Announce our arrival */
	MyNewGestalt();
	
	/* Let other parts of system software know we are here without having to call the gestalt routine we just installed */
	SetExpandMemProcessMgrExists((short)true);

	/* Initialize the Event Program to Program Communication (can load drivers) */
	ePPCInit();

	/* End of sys resources */
	THEZONE = ProcessMgrZone;
	}

/* InitConfig.  Set up hardware/software configuration information, based on the
 * word of our wonderboy, Gestalt.  Only gets the info we actually need.  Sets
 * defaults if selector not installed.  The defaults are somewhat impossible to
 * know for FPU and keyboard types, but their selectors should always be safe.
 */
void
InitConfig(void)
	{
	long			gestaltResult;

	/* set defaults in case selector undefined */
	In32BitMode = false;
	Colorized = false;
	MachineHasFPU = false;
	MachineHasMacPlusKbd = false;
#ifdef HAS_AUX_PROCESSMGR
	AUXIsPresent = false;
#endif HAS_AUX_PROCESSMGR

	/* Check processor addressing mode */
	if (Gestalt(gestaltAddressingModeAttr,&gestaltResult) == noErr)
		In32BitMode = ((gestaltResult & (1 << gestalt32BitAddressing)) != 0);

	/* Check QuickDraw version */
	if (Gestalt(gestaltQuickdrawVersion,&gestaltResult) == noErr)
		Colorized = ((gestaltResult & 0xFFFF) >= gestalt8BitQD);

	/* Does this machine have an FPU? */
	if (Gestalt(gestaltFPUType,&gestaltResult) == noErr)
		MachineHasFPU = (gestaltResult != gestaltNoFPU);

	/* Does this machine have an older style keyboard (without an escape key)? */
	if (Gestalt(gestaltKeyboardType,&gestaltResult) == noErr)
		MachineHasMacPlusKbd = (gestaltResult <= gestaltMacPlusKbd);

	if (Gestalt(gestaltGraphicsVersion,&gestaltResult) == noErr)
		skiaExists = true;
	else
		skiaExists = false;
		
#ifdef MODSQUAD
	/* Check whether drag manager is installed. */
	if (Gestalt(gestaltDragMgrVersion, &gestaltResult) == noErr)
		gDragMgrIsAvailable = true;
#endif

#ifdef HAS_AUX_PROCESSMGR
	/* Check whether we're running under AUX. */
	if (Gestalt(gestaltAUXVersion, &gestaltResult) == noErr	&& AUX_EnableCoffLaunch())
		AUXIsPresent = true;
#endif HAS_AUX_PROCESSMGR
	}
