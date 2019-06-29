/*
	File:		Processes.c

	Contains:	Process Mgr creation/deletion routines.

	Written by:	Phil Goldman and Erich Ringewald

	Copyright:	© 1986-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	  <SM55>	 7/21/93	joe		Back out <SM54>.
		<55>	 12/4/92	DRF		Only support stub 'cfri' resource consisting of a single 32-bit
									zero. This special case indicates launch a PowerPC main code
									fragment living in the datafork. In the future, we will support
									more complicated 'cfri' structures indicated by a non-zero value
									in the first 32-bit word.
		<54>	11/25/92	DRF		Roll-in changes for PsychicTV. CreateProcess() now calls CFM to
									create a native runtime context for every process. Native
									applications are also detected via the presence of a 'cfri' (0)
									resource, and launched with a mini-A5 world. In ExitToShell(),
									any connections to system fragments (DLLs), or native resources
									are released. All changes are conditionalized under "PsychicTV".
		<53>	10/28/92	DTY		Use new Get/Set macros to access ExpandMem.
		<52>	 8/26/92	DTY		Roll in latest changes for A/UX.
		<51>	 5/29/92	DCL		Included Script.h. iuSystemScript moved there for the New Inside
									Mac.
		<50>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		<49>	 2/18/92	DTY		#1021226: Switch emMessageManagerGlobals again.
		<48>	 1/14/92	YK		Take out <46> since we found a better way. But <47> is still
									there.
		<47>	 1/11/92	YK		Make tsmLayerOwner nil if the dying process is it. (for TSM)
		<46>	 1/10/92	YK		Allocate enough stack space to the Service Window Manager app
									(for TSM) even though it is the background only app. (because it
									owns windows.)
		<45>	11/22/91	DTY		Set the allocateGMBlkInSysHeap semaphore around the
									PostHighLevelEvent in SendDeathNotice to force the event to be
									allocated in the system heap instead of the Process Manager
									heap. This is a fix from 7-Up.
		<44>	10/23/91	DTY		Use new _GetOverrideMap call instead of calling
									GetTopSystemOverrideMap.
		<43>	10/20/91	DTY		New resource chains should start at the topmost System Override
									map, not SysMapHndl.  Call GetTopSystemOverrideMap to get this
									map.
		<42>	 10/9/91	YK		Added code to dispose float layer in c_exittoshell.
		<41>	 10/4/91	JSM		Change PsychoticFarmerOrLater to TheFuture.
		<40>	 9/22/91	DTY		Change PsychoticFarmerAndLater to PsychoticFarmerOrLater.
		<39>	 9/20/91	DFH		Conditionalize initialization of emMessageManagerGlobals based
									on PsychoticFarmerAndLater.
		<38>	 9/16/91	DFH		Changed CreateProcess to initialize new process' SR from the SR
									we found at startup, rather than a hard-coded 0x2000. Needed for
									NuKernel.
		<37>	 7/26/91	stb		replace HOpenResFileTrap with HOpenResFile.  No glue is used
									when SystemSevenOrLater.
		<36>	  6/4/91	DFH		Zero emMessageManagerGlobals in InitLowMemory.
		<35>	 5/23/91	dba		do a style of coercion that MPW 3.2 C likes
		<34>	  4/9/91	DFH		VL, WS#BB-KA-034 : ExitToShell now checks QDExist before calling
									InitCursor. Caused system crash when running application that
									quits before InitGraf (such as Giffer). application on a MacIIci
									or MacIIfx.
		<33>	  4/2/91	DFH		(with John Iarocci) Added version check for MicroSoft Word
									workaround. NOTE: This only affects the code when it is built
									with the HAS_AUX_PROCESSMGR condition defined.
		<32>	  4/2/91	DFH		VL,#86163: Fixed CreateProcess to not be vulnerable to
									purgeability of the DA Handler CODE 0 resource. If purged,
									LaunchDeskAccessory gets appMemFullErr, since we get garbage
									when trying to fetch a5 world info from the resource.
		<31>	 3/27/91	DFH		JWM,#84862: Fixed CreateProcess to look at result of
									HOpenResFile, rather than at RESERR, to deceide whether the call
									succeeded. RESERR can be set even if file got opened (e.g. when
									there is not enough room for the preload resources).
		<30>	 3/25/91	DFH		JSM,#DFH-910325a: Fix CreateProcess to deal with the fact that
									fabricated Apple Event is in an unlocked handle.
		<29>	 3/21/91	DFH		tes,#84989: Fixed pointer math in FMKill change #28.
		<28>	 3/20/91	DFH		gbm,#84989: Added MakeSynStrikesPurgeable.
		<27>	  3/1/91	DFH		KST,WS#DFH-910301a: Fixed GetFinderInfoForFile to extract script
									code correctly. Was setting the high byte to 0xFF.
		<26>	 2/26/91	DFH		JWM,WS#DFH-910226c: Fixed NewProcess to make sure pBabyProc is
									set before jumping to ErrorRecovery after failed
									NewProcessEntryClear.
		<25>	 2/21/91	DFH		dba,#82504, #82681, #83168, #83182, #83207: HiNewHandle rolled
									into ProcessMgrHiNewHandle. CreateProcess now calls the latter
									instead of the former.
		<24>	 2/18/91	DFH		BAC,#83089, #82873:Fix CreateProcess so that DA Handler's CODE 0
									can remain unlocked, but is not susceptible to movement.
									Currently, HOpenResFile is called after we deref the handle for
									later use.
		<23>	  2/1/91	DFH		BAC,WS#DFH-910131c:aeApplicationDied keyword parameter with the
									OSErr is now the 'errn', rather than 'errv'.
		<22>	 1/31/91	DFH		NGK,WS#DFH-910131b:Changed ExitToShell to adjust the application
									menu for the incoming front process. Copes with applications
									that call DrawMenuBar before we fix up the menu at the end of
									the foreground switch.
		<21>	 1/28/91	DFH		JSM,#81425: Removed inhibit_DAs mechanism.
		<20>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
	   	<19>	 1/14/91	DFH		(dba) Make system mode per process.
		<18>	 1/14/91	DFH		(JDR) Conditionalize out AUX support.
		<17>	 1/10/91	DFH		(JDR) Add hack to prevent HyperCard 1.2.5 patch to SysBeep.
		<16>	  1/8/91	DFH		(rdd) Got rid of pErrantProcess since it is no longer needed.
		<15>	12/18/90	DFH		Removed fgAppsUnexecuted mechanism, since front switch queue
									works better.
		<14>	12/17/90	DFH		Change NewProcess to always initialize the SCRAPHANDLE to an
									zero length handle, and let CS_INIT give the process the
									correct, up-to-date scrap.
		<13>	12/15/90	DFH		OrphanScrapIntoPartition no longer has a parameter.
		<12>	12/14/90	DFH		(SMC) Remove PMgrExit workaround in ExitToShell. Was stuffing
									HEAPEND around CleanupApplication, since PMgrExit used it to
									figure out which palettes to dispose. Now, PMgrExit uses
									ApplZone->bkLim like it should.
		<11>	12/10/90	DFH		Adding support for custom icons (i.e. the ones the user can
									specify).
		<10>	 12/5/90	DFH		Integrated AUX support.
		 <9>	11/27/90	DFH		Removed unneeded parameter from cpu_resched.
		 <8>	11/26/90	DFH		Change trespass() to dbmsg().
		 <7>	11/21/90	DFH		Changed InitHeapZone to use ROUTINE_ADDR of MyGrowZone so that
									jump table is bypassed.
		 <6>	 11/9/90	DFH		Added CancelSleep() to c_exittoshell in case ExitToShell was
									called when process was sleeping, e.g. by dsForcedQuit handler
									or MacsBug.  Otherwise, sleep queue is hosed.
		 <5>	 11/6/90	DFH		Renamed emAppleEventsGlobal to emAppleEvents.
		 <4>	 11/1/90	DFH		Put workaround in ExitToShell for PMgrExit using HEAPEND for
									limit check.  Remove when PMgrExit fixed.  Unconditionalized
									edition mgr lomem switch.
		 <2>	10/30/90	csd		Fixed some identifiers to match the newest interfaces.
		 <0>	  9/2/86	PYG		New Today.

*/

#pragma segment	kernel_segment

#include <types.h>
#include <memory.h>
#include <files.h>
#include <quickdraw.h>
#include <windows.h>
#include <menus.h>
#include <events.h>
#include <resources.h>
#include <osutils.h>
#include <script.h>
#include <Notification.h>
#include <retrace.h>
#include <segload.h>
#include <desk.h>
#include <dialogs.h>
#include <errors.h>
#include <shutdown.h>
#include <EPPC.h>
#include <MFPrivate.h>
#include <Icons.h>
#include <AppleEvents.h>
#include <AppleEventsInternal.h>
#include <ExpandMemPriv.h>
#include <ResourceMgrPriv.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Patches.h"
#include "HList.h"
#include "Aux.h"
#include "Puppet.h"
#include "Zone.h"
#include "OSDispatchPrivate.h"
#include "AppleEventExtensions.h"

#if	PsychicTV
#include	"MixedMode.h"			/*	<54> Some new interfaces from NativeProj	*/
#include	"CodeFragments.h"
#include	"CodeFragmentsTrap.h"
#endif

/* Some function prototypes that should be in (yet another) header file */
void				GetProcessIconCache(PEntryPtr);
void				AdjustApplicationMenu(PEntryPtr pTitleProc);
void				DeleteAppFromApplicationMenu(PEntryPtr);
void				AppleMenuCleanup(PEntryPtr);
Handle				GetMapHdlFromRefNum(short, Handle);
OSErr				GetDrvrResourceAndInfo(StringPtr, Handle *, short *, Str255 *);
OSErr				CheckUnitTableEntry(short);
OSErr				MakeHidingString(PEntryPtr, StringPtr);
void				InitPuppets(PEntryProc);
void				SetAppParameters(PCB **, unsigned long, InternalLaunchPBPtr, Boolean);
void				kill_state(PEntryPtr);
void				PuppetKill(void), QDKill(void), VNKill(void);
void				save_lomem(PEntryPtr);

/* Function prototypes internal to this file */
void				InitLowMemory(FSSpecPtr);
Ptr					GetCeilingPtr(PEntryPtr pPEntry);
OSErr				NewProcessEntryClear(PEntryPtr *);
void				DisposeProcessEntry(PEntryPtr);
OSErr				GetProcessMemory(PEntryPtr, Ptr);
void				DisposeProcessMemory(PEntryPtr);
OSErr				CreateProcess(InternalLaunchPBPtr, PEntryPtr, PEntryPtr, u_long *);
OSErr				InitHeapZone(PEntryPtr);
void				GetFinderInfoForFile(FSSpec *, PEntryPtr);
void				HandleShellDeath(PEntryPtr, OSErr, Boolean);
void				MenuKill(void), FMKill(Boolean), NMKill(void);

#if	PsychicTV
/*	<54> Function Prototypes for CodeFragmentMgr/MixedMode Specific Stuff	*/

Boolean				FindFragmentEntryPoint(FSSpec *theAppFSSpec,
										FragmentLocatorPtr mainFragment,
										OSType *architecture);
OSErr				SetupPsychicTVProcess(PEntryPtr pNewProc,
										Boolean launchingFragment,
										FragmentLocatorPtr mainFragment,
										OSType archictecture,
										RoutineDescriptor *nativeEntryPoint);
void				TearDownPsychicTVProcess(PEntryPtr theProc);
#endif

/*---------------------------------------------------------------------------------*
							Process Creation Routines
 *---------------------------------------------------------------------------------*/

/* Process heap and stack control */
#define TOPROOM				(0x0400)			/* APPLLIMIT during launch */
#define APPZONESIZE			(0x1800)			/* minimum app heap size at launch */
#define DAEMON_APPZONESIZE	(0x1000)			/* minimum BG only app heap size at launch */
#define DAEMON_STACKSIZE	(0x800)				/* max stack size for BG only app */

/* NewProcessEntryClear.  Make a new process desriptor. Zeroes out the entire entry, then
 * allocates a ProcessID and ProcessSerialNumber.
 */
OSErr
NewProcessEntryClear(PEntryPtr *ppProc)
	{
	register ProcessID	pid;
	PEntryPtr			retProc, *hProc;

	/* safety first! */
	*ppProc = nil;

	/* Allocate the PEntry high and lock it down */
	if ((hProc = ProcessMgrHiNewHandle(sizeof(PEntry), nil)) == nil)
		return(memFullErr);
	HLock(hProc);

	/* Clear PEntry, then fill in preliminary information */
	retProc = (char *) StripAddress(*hProc);
	MemClear(retProc, sizeof(PEntry));
	retProc->p_HandleForThisPEntry = hProc;

	/* Allocate a ProcessID (handles wrap-around gracefully). */
	pid = ProcessIDSource;
	do
		{
		if ((--pid) <= 0)
			pid = MAX_PROCESSID;
		}
	while (PEntryFromPID(pid) != nil);

	retProc->p_mypid = ProcessIDSource = pid;
	retProc->p_serialNumber.lowLongOfPSN = ProcessNumberSource++;
	retProc->p_serialNumber.highLongOfPSN = 0;
#ifdef DEBUG
	retProc->p_tattoo = BRAND;
#endif DEBUG

	/* Set output param and return value */
	*ppProc = retProc;
	return(noErr);
	}

/* DisposeProcessEntry.  Opposite of NewProcessEntryClear. */
void
DisposeProcessEntry(PEntryPtr pDeadProc)
	{
#ifdef DEBUG
	pDeadProc->p_tattoo = 0;
#endif DEBUG
	DisposHandle(pDeadProc->p_HandleForThisPEntry);
	}

/* Flag to disable app gz proc of launcher while launchees lomem is current */
Boolean inLauncheesWorld = false;

/* NewProcess.  Create the new process with a world size of reqSize bytes, and a
 * stack size of ssize bytes.
 */
OSErr
NewProcess(InternalLaunchPBPtr pParams)
	{
	register PEntryPtr	pBabyProc, pLauncherProc;
	FSSpecPtr			pFileSpec;
	PEntryPtr			pProcOnStack;
	PCB					*pPCB, **hOldPCB;
	Handle				rsrcHdl;
	short				err;
	u_long				init_appzonesize, currenta5;
	AppParametersPtr	pAEParm;
	Ptr					msgAddr;

#ifdef HAS_AUX_PROCESSMGR
	/* Sanity check */
	if ((pParams->ilTaskMode & COFF_BINARY) != 0)
		assert(AUXIsPresent);
#endif HAS_AUX_PROCESSMGR

	/* Don't call gz proc of launcher (pCurrentProcess) */
	inLauncheesWorld = true;
	pLauncherProc = pCurrentProcess;

	/* allocate a process list entry */
	err = NewProcessEntryClear(&pProcOnStack);
	if ((pBabyProc = pProcOnStack) == nil)
		goto ErrorRecovery;

	/* Determine process dimensions */
	pBabyProc->p_size = pParams->ilPartitionSize;
	LONG_ALIGN(pBabyProc->p_size);
	if ((pParams->ilTaskMode & modeOnlyBackground) != 0)
		{
		init_appzonesize = DAEMON_APPZONESIZE;
		pBabyProc->p_ssize = DAEMON_STACKSIZE;
		}
	else
		{
		init_appzonesize = APPZONESIZE;
		pBabyProc->p_ssize = pParams->ilStackSize;
		}

	pBabyProc->p_taskmode = pParams->ilTaskMode;
	pBabyProc->p_haseditcmdkeys = (-1);				/* Have not yet gone through CS_INIT */
	pBabyProc->p_wakeuptime = TICKS;				/* Reset the wakeup placeholder */

#ifdef HAS_AUX_PROCESSMGR
	/* Initialize osInfo to prevent random switches for real Macintosh applications */
	if (AUXIsPresent)
		pBabyProc->aux_realpid = AUX_ProcessMgrTask();
#endif HAS_AUX_PROCESSMGR

	/* Set signature, type */
	pFileSpec = pParams->ilAppSpecPtr;
	if ((pBabyProc->p_taskmode & modeDeskAccessory) == 0)
		GetFinderInfoForFile(pFileSpec, pBabyProc);

	/* Save the launcher's state so we can restore after generating launchee's state */
	save_state(pLauncherProc);

	/* Go get Excel's version number to determine whether we have to kludge
	 * the partition allocation.
	 * NOTE: Sets THEZONE = SYSZONE so map goes into impartial heap.
	 * NOTE: Assumes that the PEntry was zeroed, since we haven't set the p_signature
	 * (or p_type) if we're launching a DA.
	 * NOTE: Assumes that the resource chain changes resulting from HOpenResFile and
	 * CloseResFile will not be reflected in the caller's state, since our save_state
	 * (which just happened) and restore_state will take care of the relevant info.
	 */
	if (pBabyProc->p_signature == 'XCEL' || pBabyProc->p_signature == 'nX^n')
		{
		THEZONE = SYSZONE;
		SetResLoad(false);
		if (HOpenResFile(pFileSpec->vRefNum, pFileSpec->parID, &pFileSpec->name, fsRdPerm) != (-1))
			{
			SetResLoad(true);
			if ((rsrcHdl = Get1Resource('VERS', 0)) != nil)
				pBabyProc->p_version = **((unsigned long **) rsrcHdl);
			CloseResFile(CurResFile());
			}
		SetResLoad(true);
		/* At this point THEZONE == SYSZONE */
		}

	/* Big enough zone requested? */
	if (pParams->ilPartitionSize < (init_appzonesize + pBabyProc->p_ssize))
		{
		err = appMemFullErr;
		goto ErrorRecovery;
		}

	/* try allocating the physical memory partition for this process
	 * NOTE: Side effect is to set THEZONE to ProcessMgrZone.
	 */
	if ((err = GetProcessMemory(pBabyProc, GetCeilingPtr(pBabyProc)) ) != noErr)
		goto ErrorRecovery;

	/* At this point THEZONE == ProcessMgrZone */

	/* initialize the portions of low memory which need it.
	 * NOTE: CURAPNAME for a DA will be set later when we know for sure what the
	 * the driver name is.
	 */
	InitLowMemory(pFileSpec);

	/* create the application heap zone low in the partition (sets THEZONE) */
	if ((err = InitHeapZone(pBabyProc)) != noErr)
		goto ErrorRecovery;

	/* At this point THEZONE == pBabyProc->p_zone */

	/* Set the top of the resource chain back down to the system file.  This causes
	 * subsequent resource file opens to create a new chain.
	 */
	TOPMAPHANDLE = GetOverrideMap(SYSMAPHANDLE);

	/* At this point, an ExitToShell should (safely) cleanup launchee, not launcher.  So,
	 * set pCurrentProcess to new PEntry.  This means that any tracking will be against
	 * launchee (such as the FCB for CURAPREFNUM).
	 */
	pCurrentProcess = pBabyProc;

	InitAllPacks();

	/* Initialize auxiliary information in the PCB */
	pPCB = *pBabyProc->p_pcb;
	pPCB->p_vrefnum = 0;
	pPCB->launchDate = TICKS;

	InitApplPatchTable(&pPCB->applPatchTable, &pPCB->applPatchCount);
	if ((err = MEMERROR) != noErr)
		goto ErrorRecovery;

	/* load resources, set up A5 world and stack, and fill in the PEntry some more. */
	if ((err = CreateProcess(pParams, pBabyProc, pLauncherProc, &currenta5)) != noErr)
		goto ErrorRecovery;

	/* Create custom string for hide item of Application Menu */
	if ((err = MakeHidingString(pBabyProc, CURAPNAME)) != noErr)
		goto ErrorRecovery;

	/********************************************************************************
	 * NOTE: There is no error handling from here on out.
	 ********************************************************************************/

	/* Get the application icon */
	GetProcessIconCache(pBabyProc);

	/* Set up initial scrap info.  The correct scrap will get moved in later during
	 * CS_INIT in Coercion_State_Engine when the process is brought to the front.
	 * NOTE: This HandToHand's the current SCRAPHANDLE from the launcher to the
	 * launchee because there any app combos that depend on this direct inheritance
	 * as a covert means of communication.  However, they don't need it after the
	 * launchee's first event call (since the scrap can change there anyway).  Strictly
	 * a compatibility thing.
	 */
	if ((pBabyProc->p_taskmode & modeDeskAccessory) == 0)
		{
		if (SCRAPHANDLE != nil)
			(void) HandToHand(&SCRAPHANDLE);
		}
	else
		{
		SCRAPINFO = 0;
		SCRAPSTATE = 1;
		SCRAPHANDLE = NewHandleSys(0);
		}

	/* Get version longword and release it.  This non-standard version resource is
	 * a special deal for MicroSoft applications so they can call TWGetProcInfo.
	 */
	if ((rsrcHdl = (long **) GetResource('VERS', 0)) != nil)
		{
		pBabyProc->p_version = **((long **) rsrcHdl);
		ReleaseResource(rsrcHdl);
		}
	else
		pBabyProc->p_version = 0L;

	/* Check whether this application is HyperCard 1.2.5.  If so, we need to prevent
	 * it from patching SysBeep, since its patch breaks horribly under System 7.0 and
	 * later.  Remove this when HyperCard 1.2.5 is no longer supported.
	 */
	if ((pBabyProc->p_signature == 'WILD') &&
		((rsrcHdl = (long **) Get1Resource('vers', 1)) != nil))
		{
		if (**((short **) rsrcHdl) <= 0x125)
			pBabyProc->p_preventFirstSysBeepPatch = true;
		ReleaseResource(rsrcHdl);
		}

	/* Set up puppet string handling */
	InitPuppets(pBabyProc);

	/* Get Event PPC resource and create message queues */
	CreateMsgQ(pBabyProc);

	/* Handle any necessary APPPARMHANDLE/ilAppParameters conversion. */
	SetAppParameters(pBabyProc->p_pcb, currenta5, pParams, pBabyProc->eppcBlk.portID != 0);

#ifdef HAS_AUX_PROCESSMGR
	/* Fix bug in Microsoft Word.  If A/UX is running they don't try to grow
	 * the zone and then don't believe they have any memory available.  Historical
	 * braindamage that's really because they don't use the MaxApplZone, but
	 * instead manually scribble a 24-bit style heap block at the end to artificially
	 * grow the zone.  That doesn't work on A/UX.
	 */
	if (AUXIsPresent && pBabyProc->p_signature == 'MSWD' && pBabyProc->p_version <= '0400')
		MaxApplZone();
#endif HAS_AUX_PROCESSMGR

	/* fork caller's low memory into createe's PCB. */
	save_lomem(pBabyProc);

	/* Copy the following info into PCB directly, rather than through low memory, so
	 * that VBL tasks of the calling process will not ever access the wrong values.
	 * Stklowpt is set with knowledge that return address INITRET is already on the stack.
	 * NOTE: The PMSPHOOK lomem is NIL'd by the call to save_state(pLauncherProc), to
	 * make sure the hook is not used while lomem is in transition.  For the launchee to
	 * correctly inherit the launcher's value, we must copy the saved value from the
	 * launcher's PCB.  This is normally fine.  It gets tricky when the caller has no
	 * PCB (!), such as pNullProcess launching Finder.  The solution is to leave PMSPHOOK
	 * alone in this case.  Since there is no PCB, the context save didn't have to alter
	 * the lomem.  Therefore, the save_lomem(pBabyProc) just above copied the correct value.
	 */
	pPCB = *pBabyProc->p_pcb;
	pPCB->currenta5 = currenta5;
	pPCB->stklowpt = CURSTACKBASE - sizeof(Ptr);
	pPCB->fsqueuehook = (void (**)())initFSQueueHook;
	if ((hOldPCB = pLauncherProc->p_pcb) != nil)
		{
		assert(pLauncherProc != pNullProcess);
		pPCB->pmsphook = (*hOldPCB)->pmsphook;
		}

	/* Restore caller's state */
	pCurrentProcess = nil;
	(void)restore_state(pLauncherProc, true);
	pCurrentProcess = pLauncherProc;
	inLauncheesWorld = false;

	/* Send any necessary AppleEvents.  Free the message if SetAppParameters made it. */
	pAEParm = pParams->ilAppParameters;
	if (pParams->ilAppParamHdl != nil)
		{
		HLock(pParams->ilAppParamHdl);
		pAEParm = *pParams->ilAppParamHdl;
		}
	if (pAEParm != nil)
		{
		msgAddr = (pAEParm->messageLength == 0) ? nil : ((Ptr) pAEParm + sizeof(AppParameters));
		(void) BeginSystemMode();
		(void) PostHighLevelEvent(&(pAEParm->theMsgEvent), (u_long) &pBabyProc->p_serialNumber, pAEParm->eventRefCon, msgAddr, pAEParm->messageLength, receiverIDisPSN);
		(void) EndSystemMode();
		}
	if (pParams->ilAppParamHdl != nil)
		DisposHandle(pParams->ilAppParamHdl);

	/* Return output parameters to caller */
	pParams->ilResultProc = pBabyProc;
	return(noErr);

/* Common code to do the cleanup after a fatal error occurred (err==reason) */
ErrorRecovery:

	if (pBabyProc != nil)
		{
		pCurrentProcess = nil;
		DisposeProcessMemory(pBabyProc);
		(void)restore_state(pLauncherProc, true);
		pCurrentProcess = pLauncherProc;
		DisposeProcessEntry(pBabyProc);
		}

	inLauncheesWorld = false;
	pParams->ilResultProc = nil;
	return(err);
}

/* userret. The address of this routine is placed on the process' stack as the return
 * address of the main routine, so it is entered when a process exits that routine.
 */
void
userret(void)
	{
	ExitToShell();
	}

/* CreateProcess. Given a name and a PEntryPtr describing a valid address space, make
 * the given process ready to run.
 * NOTE: Assumes that PEntry and PCB were zeroed when created, so fields that are
 * supposed to be init'd to zero need no attention.
 */
OSErr
CreateProcess(InternalLaunchPBPtr pParams, PEntryPtr pNewProc, PEntryPtr pLauncherProc, u_long *pCurrenta5)
	{
	OSErr					err;
	u_long					currenta5;
	Ptr						curStackEnd, saveApplLimit;
	u_size					minappheapsize;
	struct code0			*pCode0;
	FSSpecPtr				pFileSpec;
	Handle					drvrHdl;
	short					resourceID;
	short					daFileRefNum;
#ifdef HAS_AUX_PROCESSMGR
	static struct code0 coffSegInfo =
		{
		32,					/* above a5 */
		sizeof(qd),			/* below a5 */
		0,					/* jtSize */
		32					/* jtOffset */
		};
#endif HAS_AUX_PROCESSMGR
#if	PsychicTV
	static struct code0 fakeCode0 =				/*	<54> This should be combined with A/UX stuff, 	*/
		{										/*	but the #ifdefs don’t seem to be working out…	*/
		32,					/* above a5 */
		sizeof(qd),			/* below a5 */
		0,					/* jtSize */
		32					/* jtOffset */
		};
	Boolean					launchingFragment = false;
	FragmentLocator			mainFragLocation;
	OSType					architecture;
	RoutineDescriptor		nativeEntryPoint;
#endif

	if ((pNewProc->p_taskmode & modeDeskAccessory) != 0)
		{
		/* Get the DA resource file.
		 * NOTE: We turn off ResLoad because we want the DA Handler to load the
		 * resource.  We set THEZONE to SYSZONE so that pre-load resources will
		 * get empty handles in the SYSZONE.
		 */
		SetResLoad(false);
		THEZONE = SYSZONE;
		pFileSpec = (pParams->ilDAInfoPtr)->daFileSpec;
		(*pNewProc->p_pcb)->p_fileRefNum = daFileRefNum = HOpenResFile(pFileSpec->vRefNum,
			pFileSpec->parID, &pFileSpec->name, fsCurPerm);

		/* Fix up CURAPNAME, since we know it now.  Make sure table entry is OK to use. */
		err = RESERR;
		if (daFileRefNum != (-1))
			if ((err = GetDrvrResourceAndInfo((pParams->ilDAInfoPtr)->daDrvrName, &drvrHdl, &resourceID, CURAPNAME)) == noErr)
				err = CheckUnitTableEntry(resourceID);

		THEZONE = APPLZONE;
		SetResLoad(true);
		UseResFile(0);

		/* Try for DAHandler's segment loader information resource */
		if (err == noErr)
			{
			CURAPREFNUM = SYSMAP;
			if ((SAVESEGHANDLE = GetResource(SYS_SEGMENT_TYPE, DAH_SEGMENT_ZERO)) == nil)
				if ((err = RESERR) == noErr)
					err = resNotFound;
			}

		/* Handle error opening the file, getting the driver, or checking the DCE.  We
		 * can close the file only if it was opened above the system file.
		 */
		if (err != noErr)
			{
			if ( (daFileRefNum != (-1)) && (GetMapHdlFromRefNum(daFileRefNum, SYSMAPHANDLE) == nil) )
				CloseResFile(daFileRefNum);
			return(err);
			}

		/* Get address of segment loader info, since we got it. */
		pCode0 = *SAVESEGHANDLE;

		/* Set signature, type, and name script code */
		GetFinderInfoForFile(pFileSpec, pNewProc);
		}
	else
		{
		/* Open the app file (read-only if the app is being multilaunched) */
		pFileSpec = pParams->ilAppSpecPtr;
		CURAPREFNUM = HOpenResFile(pFileSpec->vRefNum, pFileSpec->parID, &pFileSpec->name,
			((pNewProc->p_taskmode & modeMultiLaunch) != 0) ? fsRdPerm : fsCurPerm);
		if (((*pNewProc->p_pcb)->p_fileRefNum = CURAPREFNUM) == (-1))
			return(RESERR);

#ifdef HAS_AUX_PROCESSMGR
		/* Try to load the initial code segment.  Not available or needed for coff binaries. */
		if (AUXIsPresent && (pNewProc->p_taskmode & COFF_BINARY) != 0)
			{
			pCode0 = &coffSegInfo;
			SAVESEGHANDLE = nil;
			}
		else
#endif HAS_AUX_PROCESSMGR

#if	PsychicTV
		/*	<54>	Find out if there is a main code fragment to launch	*/
		if ((launchingFragment = FindFragmentEntryPoint(pFileSpec,&mainFragLocation,&architecture)))
			{
			pCode0 = &fakeCode0;
			SAVESEGHANDLE = nil;
			}
		else
#endif
			{
			if ((SAVESEGHANDLE = GetResource('CODE', 0)) == nil)
				{
				if ((err = RESERR) == noErr)
					err = resNotFound;
				CloseResFile(CURAPREFNUM);
				return(err);
				}

			pCode0 = *SAVESEGHANDLE;
			}
		}

	/* really enough room for min heap, a5 world, and stack, at least? */
	minappheapsize = ((pNewProc->p_taskmode & modeOnlyBackground) != 0) ? DAEMON_APPZONESIZE : APPZONESIZE;
	if (pNewProc->p_size < (minappheapsize + pCode0->abovea5 + pCode0->belowa5 + pNewProc->p_ssize) )
		{
		if (CURAPREFNUM != SYSMAP)
			CloseResFile(CURAPREFNUM);
		if ((pNewProc->p_taskmode & modeDeskAccessory) != 0)
			CloseResFile(daFileRefNum);
		return(appMemFullErr);
		}
			
#if	PsychicTV
	if ((err = SetupPsychicTVProcess(pNewProc,launchingFragment,&mainFragLocation,architecture,&nativeEntryPoint)) != noErr)
		{
		DebugStr("\pSetup Failed");
		CloseResFile(CURAPREFNUM);
		return(err);
		}
#endif

	/* Setup a5, curjtoffset, stackbase, and lowpt based on CODE 0 info. */
	currenta5 = (unsigned long)pNewProc->p_zone + pNewProc->p_size - pCode0->abovea5;
	CURJTOFFSET = (unsigned short)pCode0->jtoffset;
	CURSTACKBASE = curStackEnd = (Ptr) ((unsigned long)currenta5 - pCode0->belowa5);

	/* Push return address onto his stack a la JSR, so that an RTS from his main
	 * routine dumps him into our handler.  Then, push the real first routine, so
	 * that InitializeProcess() returns to it.
	 */
	*--(Ptr *)curStackEnd = INITRET;
#if	PsychicTV
	/*	<54> If we’re launching a fragment, push it’s main entry point instead of jumptable entry	*/
	
	if (launchingFragment)
		*--(Ptr *)curStackEnd = nativeEntryPoint;	//	a pointer to a private_RD that points to the native entry point
	else
#endif
		*--(Ptr *)curStackEnd = (Ptr)(currenta5 + pCode0->jtoffset + 2);	//	first jump table entry

	/* copy jump table from resource to its home in the app's a5 world */
	BlockMove(pCode0->jt, (Ptr)currenta5 + pCode0->jtoffset, pCode0->jtsize);

	/* Now that things are set up, we can release segment 0 */
	if (SAVESEGHANDLE != nil)
		ReleaseResource(SAVESEGHANDLE);

	/* Try to set APPLLIMIT to ssize below current stack.  We get an error if this
	 * would be below the current HEAPEND.  This can happen if the application heap
	 * had to be expanded after we initialized it.  Essentially, it means that the
	 * specified partition was not big enough.  We could give the guy a substandard
	 * stack size (i.e. a higher APPLLIMIT), but prefer not to.
	 * NOTE: SetApplLimit has a bug wherein APPLLIMIT is set even if an error is
	 * returned, causing the heap globals to be bad.  We therefore try to restore the
	 * value if the set failed.  That should always work.  Even if it doesn't we can
	 * probably still back out OK, since all we'll do in the heap is dispose blocks.
	 */
	saveApplLimit = APPLLIMIT;
	SetApplLimit((Ptr) ((u_long) curStackEnd - pNewProc->p_ssize));
	if (MEMERROR != noErr)
		{
		SetApplLimit(saveApplLimit);
		if (CURAPREFNUM != SYSMAP)
			CloseResFile(CURAPREFNUM);
		return(appMemFullErr);
		}

	/* Set up registers for first context switch in */
	{
	curStackEnd -= sizeof(SaveRegs);		/* To leave room for regs on the stack */

	((SaveRegs *)curStackEnd)->d2 = 0;				/* Because MS Works assumes this to be so */
	((SaveRegs *)curStackEnd)->a6 = (unsigned long) SCRATCH20;
													/* NOTE:
													 *	This is always supposed to be a safe value.
													 *	MacWrite has the following code at init:
													 *		link a6,#$fff0	; make room for buffer
													 *		move.l (a6),a0	; should be move.l a6,a0
													 *		...
													 *		_SysEnvirons	; a0 supposed to point to buffer
													 */
	((SaveRegs *)curStackEnd)->a5 = currenta5;
	((SaveRegs *)curStackEnd)->sr = initialProcessSR;
	((SaveRegs *)curStackEnd)->pc = InitializeProcess;
	pNewProc->p_sp = (unsigned long)curStackEnd;
	}

	pNewProc->p_dadProcess = pLauncherProc;
	if ((pNewProc->p_taskmode & modeCanBackground) != 0)
		{
		if ((pNewProc->p_taskmode & (modeNeedSuspendResume | modeDoesActivateOnFGSwitch))
			== (modeNeedSuspendResume | modeDoesActivateOnFGSwitch))
			pNewProc->p_isfirstbackevt = true;

		if (nbacks++ == 0)
			pLastBGProcess = pNewProc;
		}

	/* Now that the entire thing is set up, claim the PEntry */
	pNewProc->p_state = PRREADY;
	pNewProc->p_NextProcess = pProcessList;
	pProcessList = pNewProc;

	DSERRCODE = noErr;

	/* Success!  Set the output parameter and leave. */
	*pCurrenta5 = currenta5;
	return(noErr);
	}

/* InitHeapZone. This routine will initialize the heap zone for the given process.
 * NOTE: A side effect is to set THEZONE == pProc->p_zone.
 */
OSErr
InitHeapZone(PEntryPtr pProc)
	{
	/* turn his allocated world into a zone. */
	InitZone(ROUTINE_ADDR(MyGrowZone), 64, (Ptr)pProc->p_zone + (((pProc->p_taskmode & modeOnlyBackground) != 0) ? DAEMON_APPZONESIZE : APPZONESIZE), pProc->p_zone);
	if (MEMERROR != noErr)
		return(MEMERROR);

	/* Mark heap delimiters in lomem, set applzone there.  Our new zone will be
	 * the `application zone,' so we can perform memory manager calls which
	 * manipulate it.
	 */
	HIHEAPMARK = HEAPEND = (APPLZONE = THEZONE)->bkLim;

	/* Heap end is now the nominal APPZONESIZE (approx 6K, 4k for daemons).
	 * Set a reasonable appl limit (Top of world less 'TOPROOM').  This
	 * gives us a provisional heap until we get the CODE 0 resource loaded
	 * to determine where the real APPLLIMIT should go.
	 * NOTE: Launch will fail if our allocations in the app heap cause the
	 * HEAPEND to move higher than the real APPLLIMIT we set later.
	 */
	SetApplLimit((Ptr)pProc->p_zone + pProc->p_size - TOPROOM);
	if (MEMERROR != noErr)
		return (MEMERROR);

	/* Set application's memtop as if partition were located low in memory
	 * NOTE: Since Process Mgr makes SYSZONE->bklim a *variable*, the exact
	 * value we give MEMTOP is somewhat irrelevant.
	 */
	MEMTOP = SYSZONE->bkLim + pProc->p_size;

	return(noErr);
}

/* InitLowMemory. This routine initializes the low memory for a fledgling process for
 * those portions that cannot be inherited from one's parent.
 */
void
InitLowMemory(FSSpecPtr pAppFileSpec)
	{
	register long		zero = 0, minusOne = (-1);

	/* turn off stack sniffing whilst hacking on heaps... */
	STKLOWPT = (char *)zero;

	SEVTENB = minusOne;

	DSDRAWPROC = (void (**)())minusOne;
	EJECTNOTIFY = (void (**)())zero;
	IAZNOTIFY = (void (**)())zero;

	/* Set current layer using inherited window mgr and qd state */
	SetCurLayer(MFLayer);
	WWEXIST = minusOne;
	QDEXIST = minusOne;

	/* Set CURAPNAME, but make sure to limit the size (MFS names can be bigger) */
	if (pAppFileSpec != nil)
		{
		BlockMove(&pAppFileSpec->name, CURAPNAME, sizeof(Str31));
		if (Length(CURAPNAME) > sizeof(Str31) - 1)
			Length(CURAPNAME) = sizeof(Str31) - 1;
		}

	SetExpandMemAppleEvents(zero);
	SetExpandMemScriptAppGlobals((Ptr) zero);
	SetExpandMemEditionMgrPerApp((Ptr) zero);
	SetExpandMemMessageManagerGlobals(0, zero);

	DESKHOOK = (void (**)())zero;
	CLOSEORNHOOK = (void (**)())zero;
	DRAGHOOK = (void (**)())zero;

	RESUMEPROC = (void (**)())zero;

	TASKLOCK = zero;
	FSCALEDISABLE = zero;

	RESERRPROC = (void (**)())zero;
	MENULIST = (Handle)zero;

	LASTSPEXTRA = minusOne;
	WIDTHTABHANDLE = (Handle)zero;
	WIDTHLISTHAND = (Handle)zero;
	if (Colorized)
		{
		FMEXIST = minusOne;
		SYNLISTHANDLE = (Handle)zero;
		MENUCINFO = (Handle)minusOne;
		LAYERPALETTE = (Handle)zero;
		}

	MACPGM = zero;				/* Don't confuse the runtime systems */
	SWITCHDATA = (Ptr)zero;		/* Convince every one that switcher isn't running */
	SWITCHEDBITS = (char)minusOne;
	}

#define				ONE_K							(1024)
#define				ONE_MEG_PTR						((Ptr)0x100000)
#define				MIN_EXCEL_K_ABOVE_1MEG			(100)				/* In K units */
#define				MAX_EXCEL_K_ABOVE_1MEG			(525)				/* In K units */
#define				MIN_EXCEL_K_PARTITION_SIZE		(224)				/* In K units */
#define				MAX_EXCEL_K_PARTITION_SIZE		(1385)				/* In K units */
#define				MAX_REASONABLE_SYS_HEAP_TOP		((Ptr)(167*ONE_K))	/* In bytes */

/* GetCeilingPtr.  Return the address that bounds the process from above.  This is
 * necessary because certain applications make assumptions about loading in the 1st
 * meg.  In particular, make sure that WriteNow 1.00 never reaches higher than
 * 1M and calculate the ceiling for Excel 1.00-1.999... based on the empirical given
 * below (linear interpolation based on partition size).
 * NOTE: We don't care if p_size > MAX_EXCEL_K_PARTITION_SIZE: it won't be gotten and
 * GetProcessMemory will return memFragErr.
 * NOTE:  We should actually check for the case in which the user has set the Excel
 * size so large that the multiplication overflows.  However, as long as the constants
 * stay hardwired this could only happen if the size is set to 2**31/(525-127) > 10M.
 * It's not worth the extra code to handle this.  If the constants get bigger, just
 * change the units to make the product smaller.  The goal here is to waste as little
 * code as possible on this.
 */
Ptr
GetCeilingPtr(PEntryPtr pPEntry)
	{
	long 	cbDiffActualFromMin, cbAboveAsk;
	Ptr		minCeilingPtr, ceilingPtr;

	ceilingPtr = nil;
	if (pPEntry->p_signature == 'nX^n' && pPEntry->p_version <= '0100')
		ceilingPtr = ONE_MEG_PTR;
	else if (pPEntry->p_signature == 'XCEL' && pPEntry->p_version < '0200')
		{
		/* Find difference between requested and minimum partitions (might be negative). */
		cbDiffActualFromMin = pPEntry->p_size - (MIN_EXCEL_K_PARTITION_SIZE * ONE_K);

		/* Adjust ceiling pointer.  Use default if requested size was too small. */
		ceilingPtr = ONE_MEG_PTR;
		if (cbDiffActualFromMin > 0)
			{
			cbAboveAsk = (cbDiffActualFromMin
						* (MAX_EXCEL_K_ABOVE_1MEG - MIN_EXCEL_K_ABOVE_1MEG)
						/ (MAX_EXCEL_K_PARTITION_SIZE - MIN_EXCEL_K_PARTITION_SIZE))
						+ (MIN_EXCEL_K_ABOVE_1MEG * ONE_K);

			ceilingPtr += cbAboveAsk;
			}

		/* Now bound ceiling from the bottom */
		minCeilingPtr = MAX_REASONABLE_SYS_HEAP_TOP + (Ptr) pPEntry->p_size;
		if (ceilingPtr < minCeilingPtr)
			ceilingPtr = minCeilingPtr;
		}

	return ceilingPtr;
	}

/* GetProcessMemory. Get all of the physical memory required to run this process.
 * NOTE: Has unique ability to force application to be loaded completely
 * below a given ceiling.  The hokey method is to allocate a specially-marked
 * block at the ceiling point, then mangle the ProcessMgrHiNewHandle to not look higher
 * than said block.  This is to kow tow to Microsoft Excel 1.5's and WriteNow 1.0's
 * crippled designs.
 * NOTE: Assumes a5 == PROCESSMGRGLOBALS
 * NOTE: Side effect is to set THEZONE to ProcessMgrZone.
 */
static Ptr			myRoverPtr = nil;

OSErr
GetProcessMemory(PEntryPtr pProc, Ptr ceilingPtr)
	{
	Ptr			lockPtr;
	OSErr		retVal;
	PCB			*pPCB;

	lockPtr = nil;
	retVal = memFullErr;

	THEZONE = ProcessMgrZone;
	assert(APPLZONE != THEZONE || STKLOWPT == nil);

	/* If a ceiling-application in then load on top of it */
	if (myRoverPtr != nil)
		THEZONE->allocPtr = myRoverPtr;

	/* Punt if the ceiling is below the Process Mgr zone, because we can't
	 * allocate a "lock block" there.  On the other hand, a ceiling above
	 * the Process Mgr zone is taken care of naturally (because we never go
	 * higher anyway).
	 */
	if (ceilingPtr != nil)
		{
		retVal = memFragErr;
		if ( (ceilingPtr >= &THEZONE->heapData) && (ceilingPtr < THEZONE->bkLim) )
			lockPtr = GetLockPtr(ceilingPtr);
		if (lockPtr == nil)
			goto ReturnRetVal;
		}

	/* Allocate the application partition.  The error code for failure should
	 * be memFragErr if ceilingPtr != nil, memFullErr if ceilingPtr == nil.
	 */
	if ( (pProc->p_zoneh = ProcessMgrHiNewHandle(pProc->p_size, lockPtr)) == nil)
		goto ReturnRetVal;

	/* Switch back to plain error once ceiling-application partition has been made */
	retVal = memFullErr;

	/* lock the physical zone, and strip its tags. */
	HLock(pProc->p_zoneh);
	pProc->p_zone = (char *)(StripAddress(*pProc->p_zoneh));

	/* Set rover for next guy */
	if (lockPtr != nil)
		{
		THEZONE->allocPtr = myRoverPtr = (Ptr)pProc->p_zone + pProc->p_size;
		}

	/* Get the block in which to record lomem context */
	if ( (pProc->p_lmemtool = NewHandle(lmemToolDataSize)) == nil)
		goto ReturnRetVal;

	/* NOTE:  PCB must be initialized to 0's, because much of the data
	 * needs to be initialized to zero, so this is a fast, convenient way.
	 */
	if ( (pProc->p_pcb = (PCB **) NewHandleClear(sizeof(PCB))) == nil)
		goto ReturnRetVal;

	/* Well. Now we have a process control block, and can continue to build
	 * the rest of the dynamic data structures for this process in the Process Mgr
	 * heap. When all are allocated, we make sure all really exist and have one
	 * last chance to back out.
	 */

	if ( ((*pProc->p_pcb)->vblvars = NewHandle(0)) == nil)
		goto ReturnRetVal;
	if ( ((*pProc->p_pcb)->dces = NewHandle(0)) == nil)
		goto ReturnRetVal;
	if ( ((*pProc->p_pcb)->tempMemHListHdl = CreateHList(0)) == nil)
		goto ReturnRetVal;

	/* Set up the PCB. */
	pPCB = *pProc->p_pcb;
	pPCB->sysevtmask = everyEvent - keyUpMask;
	pPCB->enablePrTypeChanges = PRTYPE_CHANGESTATUS_ENABLED;
	retVal = noErr;

ReturnRetVal:
	if (lockPtr != nil)
		DisposPtr(lockPtr);
	THEZONE->allocPtr = nil;
	return(retVal);
	}

/* GetFinderInfoForFile.  Handy routine to set a bunch of app file info we need. */
void
GetFinderInfoForFile(FSSpec *pFileSpec, PEntryPtr pProc)
	{
	register ScriptCode	scriptCode;
	CInfoPBRec			ioPB;

	pProc->p_nameScript = iuSystemScript;
	ioPB.hFileInfo.ioNamePtr = &pFileSpec->name;
	ioPB.hFileInfo.ioVRefNum = pFileSpec->vRefNum;
	ioPB.hFileInfo.ioDirID = pFileSpec->parID;
	ioPB.hFileInfo.ioFDirIndex = 0;
	ioPB.hFileInfo.ioCompletion = nil;
	if (PBGetCatInfo(&ioPB.hFileInfo, SyncHFS) == noErr)
		{
		pProc->p_type = ioPB.hFileInfo.ioFlFndrInfo.fdType;
		pProc->p_signature = ioPB.hFileInfo.ioFlFndrInfo.fdCreator;
		scriptCode = (ScriptCode) ((FXInfo *) &ioPB.hFileInfo.ioFlXFndrInfo)->fdScript;
		if ((scriptCode & (1 << kScriptValidBit)) != 0)
			pProc->p_nameScript = (scriptCode & xfiScriptCodeMask);
		if ((((FInfo *) &ioPB.hFileInfo.ioFlFndrInfo)->fdFlags & FILE_FLAG_HAS_CUSTOM_ICON) != 0)
			pProc->iconResourceID = kCustomIconID;
		}
	}

/*---------------------------------------------------------------------------------*
							Process Liquidation Routines
 *---------------------------------------------------------------------------------*/

/* HandleShellDeath.  Determine the significance of the current death.  Relaunch
 * Finder if we Deep Shat from the it or else tried to _ExitToShell from the last
 * layer.
 * NOTE:  What we really want to do is switch into pNullProcess and have it just
 * constantly spawn new Finders.  The problem is, we can't really switch to it because
 * right now pNullProcess has no context other than registers (and not even these
 * if we don't change switch_task()).
 * NOTE: Assumes a5 == PROCESSMGRGLOBALS
 */
void
HandleShellDeath(PEntryPtr pDeadProc, OSErr status, Boolean isLastLayer)
	{
	register PEntryPtr	pProc;
	Boolean				relaunch;
	FSSpec				finderSpec;
	LaunchResults		finderLaunchResults;

	/* Set likely result */
	relaunch = false;

	/* Weed out case of crash during LaunchFacelessTasks() */
	if (pShellProcess == ((Ptr) -1))
		return;

	/* At the very least, relaunch Finder if it crashed */
	if (pDeadProc == pShellProcess)
		{
		pShellProcess = nil;
		if ((ShellWasFinderType) && (status != noErr))
			relaunch = true;
		}

	/* Handle death of last app on the system.  If process we launched from FINDERNAME
	 * is Finder or Finder-replacement, relaunch it.  Otherwise, call for a restart as
	 * soon as the corpse is buried.
	 * NOTE: The SysError handler needs the fonts set up still.
	 */
	if (pProcessList == nil)
		{
		if (ShellWasFinderType)
			relaunch = true;
		else
			ShutDwnUserChoice();
		}

	/* Handle last faceful app.  If process we launched from FINDERNAME is Finder or a
	 * Finder-replacement, relaunch it.  Otherwise, kill all the remaining apps (i.e.
	 * daemons).  When the last of them dies, we'll be called again and say to shutdown.
	 */
	else if (isLastLayer)
		{
		if (ShellWasFinderType)
			relaunch = true;
		else
			{
			pProc = pProcessList;
			while (pProc != nil)
				{
				if (pProc != pDeadProc)
					KillProcess(&pProc->p_serialNumber);
				pProc = pProc->p_NextProcess;
				}
			}
		}

	/* Well, do we take the initiative about Finder? */
	if (relaunch)
		{
		if ((finderLaunchResults.LaunchError = FSMakeFSSpec(BOOTVOL, 0, FINDERNAME, &finderSpec)) == noErr)
			StandardLaunch(&finderSpec, launchUseMinimum, 0, nil, &finderLaunchResults, nil);

		if (finderLaunchResults.LaunchError != noErr)
			SysError(dsFinderErr);

		/* Wait to do this 'til we're sure. */
		pShellProcess = finderLaunchResults.pNewProcess;
		pShellProcess->p_dadProcess = nil;
		ShellWasFinderType = IS_FINDER(pShellProcess);
		(void) SetFrontProcess(&pShellProcess->p_serialNumber);
		}
	}

/* DisposeProcessMemory. Dispose of the physical memory occupied by the specified process.
 * Doesn't assume structures exist, because this may be called as error recovery
 * from launch.
 * NOTE: Does not explicitly dispose things that are known to reside in the application
 * heap, since these are implicitly freed by freeing or resizing the app partition.
 */
void
DisposeProcessMemory(PEntryPtr pProc)
	{
	PCB				*pPCB, **hPCB;

	/* Must invalidate rover if it was set for this app's partition */
	if (myRoverPtr == ((Ptr)pProc->p_zone + pProc->p_size))
		myRoverPtr = nil;

	/* Free the partition */
	if (pProc->p_zoneh != nil)
		{
		DisposHandle(pProc->p_zoneh);
		pProc->p_zoneh = nil;
		}

	/* free structures hanging off of PEntry proper */
	if (pProc->p_lmemtool != nil)
		{
		DisposHandle(pProc->p_lmemtool);
		pProc->p_lmemtool = nil;
		}

	/* now free structures hanging off PCB, then PCB itself */
	if ((hPCB = pProc->p_pcb) == nil)
		return;

	pProc->p_pcb = nil;
	HLock(hPCB);
	pPCB = *hPCB;

	if (pPCB->vblvars != nil)
		DisposHandle(pPCB->vblvars);

	if (pPCB->dces != nil)
		DisposHandle(pPCB->dces);

	if (HListExists(pPCB->p_PseudoEvtQHList))
		DestroyHList(pPCB->p_PseudoEvtQHList);

	if (HListExists(pPCB->tempMemHListHdl))
		DestroyHList(pPCB->tempMemHListHdl);

	/* puppetstringhdl is either a) in app heap (dispose is not needed), or b) a shared
	 * resource (dispose would be bad for others).  Don't dispose it.
	 */
	if (pPCB->puppetstringhdl != nil)
		{
	 	/* DisposHandle(pPCB->puppetstringhdl); */
		}

	DisposHandle(hPCB);
	}

/* MenuKill.  Delete Apple menu items defined by this app, then delete application
 * name itself. */
void
MenuKill(void)
	{
	/* Nuke MENULIST because app resource files will be closed, which releases all the
	 * menu resources, leaving disposed handles in the menulist.
	 */
	MENULIST = nil;

	/* Remove our record of this app having an Apple menu */
	AppleMenuCleanup(pCurrentProcess);

	/* Update the Application Menu.  Must reset the enable state of the hiding items if
	 * this process is dying in the background.
	 */
	DeleteAppFromApplicationMenu(pCurrentProcess);
	if (pCurrentProcess != LayerOwner(GetFrontAppLayer()))
		AdjustApplicationMenu(nil);
	}

/* NMKill.  Remove all notification manager requests for the current process.
 * Criteria is that request memory lies within the application's partition.
 * NOTE:  This should be a trap call to NM mgr.
 */
void
NMKill(void)
	{
	register NMRecPtr	curNMRec;
	NMRecPtr			oldNMRec;
	Ptr					base, bound;
	unsigned short		ps;

	/* Bail out if NM doesn't exist or there are no outstanding requests */
	if (NMQHDRPTR == nil || NMQHDRPTR == ((Ptr) -1))
		return;

	/* Cache the application boundaries */
	base = pCurrentProcess->p_zone;
	bound = base + pCurrentProcess->p_size;

	/* Scan the list.  Disables interrupts to avoid collisions. */
	curNMRec = NMQHDRPTR->qHead;
	ps = disable();
	while (curNMRec != nil)
		{
		oldNMRec = curNMRec; curNMRec = curNMRec->qLink;
		if ((oldNMRec >= base) && (oldNMRec <= bound))
			(void) NMRemove(oldNMRec);
		}
	spl(ps);
	}

/* FMKill.  Remove all font handles for the current process.  If the location of these
 * handles ever changes to another zone, so should this.
 * NOTE: WIDTHTABHANDLE, WIDTHLISTHAND, and SYNLISTHANDLE are nil'd out in case the
 * lomem may be used again (esp. for sys startup caller).
 * NOTE: Assumes THEZONE != ProcessMgrZone, in that it restores THEZONE unconditionally,
 * instead of using SafeSetZone and RestoreZone.
 */
void
FMKill(Boolean disposeSynStrikes)
	{
	short				iTable;
	Ptr					sysZoneEnd, tableEnd;
	Handle				widthTabHdl, *widthTabHdlPtr;
	SynEntryPtr			synEntryPtr;
	THz					saveZone;

	/* All the action here occurs in the system heap */
	saveZone = THEZONE;
	THEZONE = SYSZONE;
	sysZoneEnd = SYSZONE->bkLim;

	/* Deallocate the current WIDTHTABHANDLE */
	if (WIDTHTABHANDLE != nil)
		{
		DisposHandle(WIDTHTABHANDLE);
		WIDTHTABHANDLE = nil;
		}

	/* Get rid of any system handle in WIDTHLISTHAND, then dispose the list itself */
	if (WIDTHLISTHAND != nil)
		{
		assert(*WIDTHLISTHAND != nil);
		HLock(WIDTHLISTHAND);

		widthTabHdlPtr = *WIDTHLISTHAND;
		iTable = NUMITABLES - 1;
		do
			{
			/* Table may prematurely end with a 0 */
			if ((widthTabHdl = (Handle) StripAddress(*widthTabHdlPtr++)) == nil)
				break;

			/* Get rid of sys heap handles */
			if (widthTabHdl < sysZoneEnd)
				DisposHandle(widthTabHdl);
			}
		while (--iTable >= 0);

		/* Dispose of the list itself */
		DisposHandle(WIDTHLISTHAND);
		WIDTHLISTHAND = nil;
		}

	/* Get rid of any system handle in SYNLISTHANDLE, then dispose the list itself.
	 * NOTE: We are guaranteed that when we are called by InitializeEnvironment(), that
	 * only the first entry could be in the sys heap.  Worse yet, a following entry
	 * might appear to be in the sys heap because it was from a MP block at the
	 * bottom of the app heap, and a subsequent _SetAppBase has raised the top of
	 * the sys heap over it.  We therefore check that entry is below sysZoneEnd.
	 */
	if (Colorized && (SYNLISTHANDLE != nil) && ((synEntryPtr = StripAddress(*SYNLISTHANDLE)) != nil))
		{
		if (disposeSynStrikes)
			{
			HLock(SYNLISTHANDLE);
			tableEnd = (Ptr) synEntryPtr + GetHandleSize(SYNLISTHANDLE);

			/* Get rid of sys heap handles */
			while ((Ptr) synEntryPtr < tableEnd)
				{
				if ( (synEntryPtr->synNFNT != (-1)) &&
					(StripAddress(synEntryPtr->synStrikeHdl) < sysZoneEnd) )
						DisposHandle(synEntryPtr->synStrikeHdl);
				synEntryPtr += 1;
				}
			}

		/* Dispose of the list itself */
		DisposHandle(SYNLISTHANDLE);
		SYNLISTHANDLE = nil;
		}

	THEZONE = saveZone;
	}

/* MakeSynStrikesPurgeable.  The Font Manager (spec. _FMSwapFont) always leaves one of
 * the synthetic strikes non-purgeable.  It stays that way indefinitely, even though the
 * Font Manager can easily cope with a purged strike.  Since the memory for the strike
 * (10's of Kb) comes from the system heap, we would like to see the strike purgeable.
 * There is no great time in QuickDraw or the Font Manager we can do this, since the
 * caller of _FMSwapFont is a public call (app may assume strike is non-purgeable in
 * places the system doesn't).  We figure, tho, that by the time the app is making a
 * ToolBox event call it can cope with a purgeable strike.
 */
void
MakeSynStrikesPurgeable(void)
	{
	SynEntryPtr		synEntryPtr;
	Ptr				tableEnd;

	if (Colorized && (SYNLISTHANDLE != nil) && ((synEntryPtr = StripAddress(*SYNLISTHANDLE)) != nil))
		{
		tableEnd = (Ptr) synEntryPtr + GetHandleSize(SYNLISTHANDLE);
		while ((Ptr) synEntryPtr < tableEnd)
			{
			if (synEntryPtr->synNFNT != (-1))
				HPurge(synEntryPtr->synStrikeHdl);
			synEntryPtr += 1;
			}
		}
	}

/* Message format for aeApplicationDied we send */
typedef struct ApplicationDiedMsg {
	AETFHeader				messageHeader;
	KeyWord					metaDataMark;
	AETFParameter			appDiedErrorHdr;
	long					appDiedErrorNumber;
	AETFParameter			appDiedProcessHdr;
	ProcessSerialNumber		appDiedProcessNumber;
} ApplicationDiedMsg;

/* SendDeathNotice.  Notify the specified process that its pal is dying. */
void
SendDeathNotice(PEntryPtr pTargetProc, PEntryPtr pProc, OSErr status)
	{
	ApplicationDiedMsg		msgStorage;
	HighLevelEventRecord	theEvent;

	/* Do nothing if not there, or doesn't want the notification */
	if ((pTargetProc == nil) || ((pTargetProc->p_taskmode & modeGetAppDiedMsg) == 0))
		return;

	/* Set up the high-level event record */
	SetAppleEvent(aeApplicationDied, &theEvent);

	/* Set up the message buffer */
	GenericAEMessage(&msgStorage.messageHeader);
	msgStorage.metaDataMark = aeEndOfMetaDataKeyword;
	SetAEParmHeader(&msgStorage.appDiedErrorHdr, keyErrorNumber, aeLongintParamType, sizeof(long));
	msgStorage.appDiedErrorNumber = (long) status;

	SetAEParmHeader(&msgStorage.appDiedProcessHdr, aeProcessKeyword, aeProcessParamType, sizeof(ProcessSerialNumber));
	msgStorage.appDiedProcessNumber = pProc->p_serialNumber;

	/* Post the event.  Do it in system mode since the dying process is incapable of EPPC. */
	(void) BeginSystemMode();
	allocateGMBlkInSysHeap = true;								// <45> Force event to be created in the System heap
	(void) PostHighLevelEvent((EventRecord *) &theEvent, (u_long) &pTargetProc->p_serialNumber, 0, &msgStorage, sizeof(ApplicationDiedMsg), receiverIDisPSN);
	allocateGMBlkInSysHeap = false;								// <45> Go back to trying to allocate from the PM heap first
	(void) EndSystemMode();
	}

/* c_exittoshell.  Here is our patch for ExitToShell().  We don't want this to run as a
 * kernel routine!!!  Either the application called this in the normal course of events,
 * or else some type of exception was fielded by the debugger and the user did an
 * exit (e.g. ES in MacsBug) from there, thus setting the DSErrCode.  This status is
 * passed on to the bereaved parent.
 * This is the last subroutine a process ever calls.
 */
pascal void
c_exittoshell(void)
	{
	PEntryPtr		pDeadProc, pOtherProc;
	LayerPtr		pLayer;
	Boolean			frontmost, isLastLayer, deathByQuitAll;
	PCB				**pcbHdl;
	OSErr			exitStatus;
	unsigned long	oldA5;

	/* Use our A5, but swipe the graphics globals from CURRENTA5 as long as InitGraf has
	 * been called (i.e. QDEXIST == 0).
	 */
	if (QDEXIST == (char) 0)
		{
		A5SimpleRestore(CURRENTA5);
		oldA5 = ProcessMgrA5Setup();
		}
	else
		oldA5 = ProcessMgrA5SimpleSetup();

	/* Disk switches don't count as sys errors */
	if ((exitStatus = (OSErr) DSERRCODE) == dsReinsert)
		exitStatus = noErr;

	/* Switch to a neutral zone, in case we call something that allocates memory */
	THEZONE = SYSZONE;

#if	PsychicTV
	/*	<54>	Tear down any CFM-loaded system DLLs or fat resources	*/
	
	TearDownPsychicTVProcess(pCurrentProcess);
#endif

	/* Collect some useful information for later */
	pDeadProc = pCurrentProcess;
	pcbHdl = pDeadProc->p_pcb;

	/* Reset this so that cleanup won't wipe out files/data acquired in system mode */
	pDeadProc->p_systemModeLevel = 0;

	/* Clean up executable stuff so it doesn't interfere with our cleanup (for example,
	 * the dead app might have patches to traps we call).
	 */
	kill_state(pDeadProc);

	/* Dump any pending events if app crashed.
	 * NOTE: Do this only for the foreground process?
	 */
	if (exitStatus != noErr)
		FlushEvents(everyEvent, 0);

	/* See if process died in its sleep.  Something other than the app code may be
	 * calling _ExitToShell (like MacsBug or the dsForcedQuit handler).
	 */
	CancelSleep(pDeadProc);

	/* Reset other process' entries */
 	pOtherProc = pProcessList;
 	while (pOtherProc != nil)
		{
		if (pOtherProc->p_dadProcess == pDeadProc)
			pOtherProc->p_dadProcess = nil;
		pOtherProc = pOtherProc->p_NextProcess;
		}

	/* Remove from St. Peter's waiting list */
	if (pDeadProc->p_condemned)
		{
		RemoveFromStateList(pDeadProc, &pDyingStateList);
		pDeadProc->p_condemned = wFalse;
		}

	/* Make sure the registered debugger checks out (and takes his debuggees with him) */
	if (pDeadProc == pDebugProcess)
		{
		ProcessSerialNumber		noProcess;

		SetPSN(kNoProcess, &noProcess);
		RegisterDebugger(&noProcess, nil, nil, nil, 0);
		}

	/* Cancel system quit if the dead app is the one that started the whole thing. */
	frontmost = (pDeadProc == pFrontProcess);
	deathByQuitAll = false;
	if (pSystemQuitAllMsg != nil)
		{
		if (EqualPSN(&(pSystemQuitAllMsg->sendermfid.localPSN), &pDeadProc->p_serialNumber))
			CancelQuitAll(nil);
		else
			deathByQuitAll = (EqualPSN(&(pSystemQuitAllMsg->targetmfid.localPSN), &pDeadProc->p_serialNumber));
		}

	/* Cancel puppet stringing. */
	PuppetKill();

	/* Nuke EPPC and PPC stuff. */
	DestroyMsgQ(&pDeadProc->eppcBlk);

	/* Cleanup volume notification */
	VNKill();

	/* Make sure cursor is visible */
	if ((frontmost) && (QDEXIST == (char) 0))
		InitCursor();

	/* Menu manager cleanup */
	MenuKill();

	/* Re-enable CloseWD for home working directory (p_vrefnum) */
	(*pcbHdl)->p_vrefnum = 0;

	/* Get system and toolbox to cleanup.  Set the zone and A5 since some need them
	 * to know which structures to wipe.
	 */
	THEZONE = APPLZONE;
	A5SimpleRestore(oldA5);
	CleanupApplication();
	(void) ProcessMgrA5SimpleSetup();
	THEZONE = SYSZONE;

	/* Close down the toolbox for the dead process (needs our help) */
	if ((pLayer = pDeadProc->p_layer) != nil)
		LayerClose(pLayer);
	if (pDeadProc->p_floatLayer != nil)
		DisposeWindow(pDeadProc->p_floatLayer);
	if (pDeadProc == tsmLayerOwner)
		tsmLayerOwner = nil;
	QDKill();
	NMKill();

	/* Remember whether this is the last visible layer going away */
	isLastLayer = (GetFrontAppLayer() == nil);

	/* Reallow switching in case he died in the middle. */
	while (dont_switch > 0)
		EnableSwitching();

	/* Remove from count of background apps, if he is one */
	if ((pDeadProc->p_taskmode & modeCanBackground) != 0)
		nbacks--;

	{
	/* unlink pDeadProc from list of active entries */
	PEntryPtr		pPrev, pNext;

	pPrev = pNullProcess;
	while ((pNext = pPrev->p_NextProcess) != pDeadProc)
		{
		assert(pNext != nil);
		pPrev = pNext;
		}
	pPrev->p_NextProcess = pDeadProc->p_NextProcess;
	}

	/* Adjust BG round-robin placeholder */
	if (pDeadProc == pLastBGProcess)
		{
		if ((pLastBGProcess = pDeadProc->p_NextProcess) == nil)
			pLastBGProcess = pProcessList;
		}

	/* Take out of queue if dead guy was waiting to come to the front */
	RemoveFromFrontProcessQueue(pDeadProc);

	/* Reset global so we don't end up switching to same guy! */
	if (pDeadProc == pNewFrontProcess)
		pNewFrontProcess = nil;

	/* Let the world know of the tragedy that has beset us */
	SendDeathNotice(pDeadProc->p_dadProcess, pDeadProc, exitStatus);
	if (pDebugProcess != pDeadProc->p_dadProcess)
		SendDeathNotice(pDebugProcess, pDeadProc, exitStatus);

	/* Handle shell quit or crash, or death of the last layer.
	 * NOTE: We wait to FMKill until after HandleShellDeath, because fonts will be
	 * needed if a SysError is generated (like dsFinderErr, or inside ShutDwnUserChoice).
	 */
	HandleShellDeath(pDeadProc, exitStatus, isLastLayer);
	FMKill(true);

	/* Remove the process' memory.  Be very careful for awhile, since some of the basic
	 * structures are about to become invalid!
	 * NOTE: OrphanScrapIntoPartition commandeers the partition to hold the scrap until
	 * it coercion is satisfied.  This means we'd better be done with everything we need
	 * in the partition!
	 */
	OrphanScrapIntoPartition();
	pCurrentProcess = nil;
	DisposeProcessMemory(pDeadProc);
	DisposeProcessEntry(pDeadProc);

	/* Set pFrontProcess to new front process now that current guy is dead (in the
	 * event we don't call cpu_resched() below).
	 * Candidates:
	 *	• current frontmost if that isn't the dead app (pFrontProcess)
	 *	• the process from SetFrontProcess() or canceled switch (pNewFrontProcess)
	 *	• process waiting to come to the front (PopFrontProcessQueue())
	 *	• process that initiated a system quit (from pSystemQuitAllMsg)
	 *	• process controlling the top layer
	 */

	/* See if we need to select new foreground app */
	if (frontmost)
		{
		if ( ((pOtherProc = pNewFrontProcess) == nil) &&
			((pOtherProc = PopFrontProcessQueue()) == nil) )
			{
			if (pSystemQuitAllMsg != nil)
				pOtherProc = PEntryFromPSN(&(pSystemQuitAllMsg->sendermfid.localPSN));
			else
				{
				if ((pLayer = LGetNextLayer(nil, true)) == nil)
					pLayer = LGetNextLayer(nil, false);
				assert(pLayer != nil);
				pOtherProc = LayerOwner(pLayer);
				}
			}

		assert(IsProcessEntry(pOtherProc));

		pFrontProcess = pOtherProc;
		}

	/* User deaths require full scrap-in switches.  Background deaths cause control to
	 * go the frontmost, unless generalized rescheduling was requested.  Otherwise, it
	 * was a simple murder of a non-frontmost process.
	 */

	/* Check for death of foreground process with other facefuls available */
	if (frontmost && (isLastLayer == false) && (pFrontProcess != nil))
		{
		/* Jump into end of coercion (scrap info set by OrphanScrapIntoPartition) */
		coercionState = CS_ENTERING;

		/* So that new front process sees valid menu bar */
		AdjustApplicationMenu(pFrontProcess);

		/* Switch to front process we located earlier */
		pNewFrontProcess = nil;
		CancelSleep(pFrontProcess);
		pFrontProcess->p_state = PRMOVING;

		/* Reset OSDispatch depth indicator.  This is positioned such that we have
		 * already made all the OSDispatch calls we need before the switch.
		 */
		kernelbusy = 0;

		switch_task(pFrontProcess);
		}

	/* Death of BG process or last foreground process */
	else
		{
		/* Continue system quit, if it's happening */
		if (deathByQuitAll)
			ContinueQuitAll();

		/* Reset OSDispatch depth indicator.  This is positioned such that we have
		 * already made all the OSDispatch calls we need before the switch.
		 */
		kernelbusy = 0;

		cpu_resched();
		}

	dbmsg("ExitToShell failed to schedule a different application.");
	}

/*----------------------------------------------------------------------------------*
 *							Process Access Routines									*
 *----------------------------------------------------------------------------------*/

/* PEntryFromPID.  Locate existing process having given ProcessID. */
PEntryPtr
PEntryFromPID(ProcessID pid)
	{
	register PEntryPtr		pProc;
	unsigned long			oldA5;

	/* Loop until ProcessID matches or end of list */
	oldA5 = ProcessMgrA5SimpleSetup();
	pProc = pProcessList;
	while (pProc != nil)
		{
		if ((pProc->p_mypid) == pid)
			break;

		pProc = pProc->p_NextProcess;
		}

	A5SimpleRestore(oldA5);
	return(pProc);
	}

/* EqualPSN.  Return whether the two PSNs are identical. */
Boolean
EqualPSN(ProcessSerialNumberPtr pFirstPSN, ProcessSerialNumberPtr pSecondPSN)
	{
	return( (pFirstPSN->lowLongOfPSN == pSecondPSN->lowLongOfPSN) &&
			(pFirstPSN->highLongOfPSN == pSecondPSN->highLongOfPSN) );
	}

/* PEntryFromPSN.  Locate existing process having given ProcessSerialNumber.
 * NOTE: Caller may be passing in PSN address from heap block, so we should
 * be careful to not move memory in this call.
 */
PEntryPtr
PEntryFromPSN(ProcessSerialNumberPtr pPSN)
	{
	register PEntryPtr		pProc;
	unsigned long			oldA5;

	/* Weed out the junkies */
	if (pPSN == nil)
		return(nil);

	/* Look first for special value */
	oldA5 = ProcessMgrA5SimpleSetup();
	if ((pPSN->lowLongOfPSN == kCurrentProcess) && (pPSN->highLongOfPSN == 0))
		pProc = pCurrentProcess;
	else
		{
		/* Loop until ProcessSerialNumber matches or end of list */
		pProc = pProcessList;
		while (pProc != nil)
			{
			if (EqualPSN(&pProc->p_serialNumber, pPSN))
				break;

			pProc = pProc->p_NextProcess;
			}
		}

	A5SimpleRestore(oldA5);
	return(pProc);
	}

/* PEntryFromFileSpec.  See if we can find a process that is running off the
 * the specified file.  Include the pResName if you're looking for a DA.
 * NOTE: Does PBGetFCBInfo return appropriate ioFCBParID on MFS volume?
 */
PEntryPtr
PEntryFromFileSpec(FSSpecPtr pSpec, StringPtr pResName)
	{
	PEntryPtr	pProc;
	FCBPBRec	fcbInfoRec;
	Str255		rsrcFileName;

	/* Iterate through the process list */
	pProc = pProcessList;
	fcbInfoRec.ioCompletion = nil;
	while (pProc != nil)
		{
		/* Get resource file spec for this process */
		fcbInfoRec.ioFCBIndx = 0;
		fcbInfoRec.ioRefNum = (*pProc->p_pcb)->p_fileRefNum;
		fcbInfoRec.ioNamePtr = &rsrcFileName;
		if (PBGetFCBInfo(&fcbInfoRec, SyncHFS) == noErr)
			{
			/* See if spec matches */
			if ((pSpec->vRefNum == fcbInfoRec.ioFCBVRefNum) && (pSpec->parID == fcbInfoRec.ioFCBParID))
				{
				if ( (MyRelString(&pSpec->name, &rsrcFileName) == sortsEqual) &&
					((pResName == nil) || (MyRelString(pResName, &((*pProc->p_pcb)->curapname)) == sortsEqual)) )
					break;
				}
			}

		pProc = pProc->p_NextProcess;
		}

	return (pProc);
	}

#if	PsychicTV

/*----------------------------------------------------------------------------------*
 *	<54>					PsychicTV Support Routines								*
 *----------------------------------------------------------------------------------*/

/*	Routines added to support launching of Native applications under RISC System Software	*/
/*	These routines can also be generalized to support Code Fragment Manager on 68K			*/


Boolean
FindFragmentEntryPoint(FSSpec *theAppFSSpec,FragmentLocatorPtr mainFragment,OSType	*architecture)
	{
	// <55> we’re looking for a longword version for now, and we only support version 0.
	//		(PowerPC code in the datafork).
	
	long	**codeFragIndex;	
	
	if ((codeFragIndex = (long **) Get1Resource('cfri',0)) && (**codeFragIndex == 0))
		{
		mainFragment->where = kOnDiskFlat;			//	In the data fork
		mainFragment->u.onDisk.fileSpec = theAppFSSpec;
		mainFragment->u.onDisk.offset = 0;			//	the whole data fork is the container
		mainFragment->u.onDisk.length = 0;			//	another for the whole data fork

		*architecture = kPowerPC;					//	this is all we have now…

		ReleaseResource(codeFragIndex);
		return(true);
		}
	else
		return(false);
	}

OSErr
SetupPsychicTVProcess(PEntryPtr pNewProc,Boolean launchingFragment,FragmentLocatorPtr mainFragment,
								OSType architecture,RoutineDescriptor *nativeEntryPoint)
	{
	OSErr				err;
	FragContextID 		fragContext;
	FragConnectionID	fragConnection;
	LogicalAddress		entryPoint;
	Str255				badLibName;
	
	err = FragCreateContext(&fragContext,
							pNewProc->p_zone,
							kDefaultAddrSpaceID,
							kDefaultTeamID,
							kDefaultTaskID,
							0);
	if (err != noErr)
		return(err);
		
	(*pNewProc->p_pcb)->fragContext = fragContext;		// Save contextID for later use
	
	if (launchingFragment)
		{
		if (architecture != kPowerPC)
			return(-1);

		err = FragPrepare(	fragContext,
							mainFragment,
							kNoLibName,
							kAttachStatic,
							architecture,		//	usually PowerPC
							&fragConnection,
							&entryPoint,
							badLibName);		//	Is the errorString useful to "Joe User"?
		if (err == noErr)
			*nativeEntryPoint = NewRoutineDescriptor((ProcPtr) entryPoint,kCStackBased+kNoReturnValue,kCodeTypePower);
		else
			{
			//	We couldn’t load a fragment, so destroy the context, and return the
			//	error code.  We could pass the badLibName up a level, but I'm not sure
			//	this is useful to the average user.  Maybe a debugging version could…
			
			DebugStr("\pPrepare Failed!");
			
			(void) FragRelease(fragContext,kAllConnections,kDefaultRelease,kNormalTerm);
			(*pNewProc->p_pcb)->fragContext = 0;	// so we don’t double-release
			return (err);
			}
		}

	return(noErr);
	}

void
TearDownPsychicTVProcess(PEntryPtr pProc)
	{
	FragErr			err;
	FragContextID	fragContext;
	
	fragContext = (*pProc->p_pcb)->fragContext;
	
	if (fragContext)
		{
		err = FragRelease(fragContext,kAllConnections,kDefaultRelease,kNormalTerm);
		if (err != noErr)
			FragRelease(fragContext,kAllConnections,kDefaultRelease,kAbnormalTerm);
		}
	}

#endif	PsychicTV
