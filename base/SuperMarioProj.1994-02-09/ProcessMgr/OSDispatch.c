/*
	File:		OSDispatch.c

	Contains:	Selected Process Mgr dispatch routines.

	Written by:	Phil Goldman and Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<18>	  9/4/92	DRF		#1041525,<EA> Changed SameProcess() to handle the case where
									there is no current process.
		<17>	 8/26/92	DTY		Change ModSquad’s funky conditional to use #ifdefs like everyone
									else.
		<16>	 8/26/92	DTY		Roll in latest changes for A/UX.
		<15>	 8/17/92	hjcr	Adding support for OCE's "defer drag" feature. Conditionally
									compiled under MODSQUAD
		<14>	 4/10/92	JSM		Get rid of compiler warning from last change.
		<13>	  4/9/92	KST		#1025131,<JH>: Changed SWM from a background app to a driver so
									that input method can do dialog, change cursor, access the menu,
									and type into the dialog window.
		<12>	 3/25/92	DTY		#1024114,<pvh>: Added c_PEntryFromProcessSerialNumber. This is
									identical to the PEntryFromPSN routine in Processes.c, except
									that this follows Pascal calling conventions and is accessed as
									a selector off of OSDispatch. Cube-E Finder needs this routine
									so that it doesn’t have to rely on hard coded offsets into
									Process Manager globals for the application assassin code. The
									ultimate solution would be to move the assassin into Launch and
									remove this selector.
		<11>	 1/11/92	YK		Added NewTSMLayerOwner and RouteEventToSWM for TSM.
		<10>	10/16/91	YK		Added c_InlineAware for supporting the Text Services Manager.
		 <9>	 10/9/91	DTY		Add #include "OSDispatchPrivate.h" for new definition of
									FindAppLayer(). The build was broken because FindAppLayer() was
									changed to FindAppBaseLayer(), and FindAppLayer() became a macro
									which called the new name. This file wasn’t updated to reflect
									the new scheme of things.
		 <8>	 1/14/91	DFH		Fix checkin comment.
		 <7>	 1/14/91	DFH		(dba) Make system mode per process.
		 <6>	 1/14/91	DFH		(JDR) Conditionalize out AUX support.
		 <4>	  1/8/91	DFH		(rdd) Changed c_SetFrontProcess to allow for case where
									destination process is already frontmost, but suspended for
									high-level debugging.
		 <3>	12/20/90	DFH		(dba) Remove obsolete dispatch routine TWKill.
		 <2>	 12/5/90	DFH		Integrate AUX support.
		 <0>	 x/xx/86	PYG		New Today.

*/ 

#include <types.h>
#include <memory.h>
#include <files.h>
#include <quickdraw.h>
#include <windows.h>
#include <menus.h>
#include <events.h>
#include <resources.h>
#include <osutils.h>
#include <toolutils.h>
#include <retrace.h>
#include <desk.h>
#include <dialogs.h>
#include <errors.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Aux.h"
#include "Puppet.h"
#include "OSDispatchPrivate.h"

/*------------------------------------------------------------------

	Here are the exported (via Trap) task management calls.
	
  ------------------------------------------------------------------*/

#pragma segment	kernel_segment

/* c_KernelLaunch.  Launch a task into a new partition. */ 
pascal OSErr
c_KernelLaunch(InternalLaunchPBPtr pParams)
	{
	unsigned long	olda5;
	OSErr			err, NewProcess(InternalLaunchPBPtr);
	void			DisposeProcessEntry(PEntryPtr);
		
	/* Get access to our globals, and switch to launchee's volume */
	olda5 = ProcessMgrA5Setup();
	
	/* Create a new process.  Let AUX have the first chance at it. */
#ifdef HAS_AUX_PROCESSMGR
	if (AUXIsPresent) 
		{
		err = AUX_Launch(pParams);	
		if ((err == permErr))
			err = NewProcess(pParams);
		}
	else
#endif HAS_AUX_PROCESSMGR
		err = NewProcess(pParams);
					
	A5Restore(olda5);
	return(err);
	}

/* TWFGSwitch. This routine arranges for a foreground switch from the current application
 * to the given one.  Note: It is OK for a task to switch to itself, as long as it's not
 * currently the foreground task.  In this way a background app could force itself to the
 * front.
 * NOTE: This is the old call, so it just defers to the current one (SetFrontProcess).
 */
pascal long
c_TWFGSWitch(ProcessID pid)
	{
	PEntryPtr	pProc;
	
	/* Locate the process. */
	if ((pProc = PEntryFromPID(pid)) == nil)
		return(procNotFound);
	
	return(SetFrontProcess(&pProc->p_serialNumber));
	}

/* KillProcess.  Terminate the given process.  Actually, this just puts the process
 * in a state where the next switch in will make it ExitToShell.  This way, the
 * low memory/trap patch/VBL/etc context is correct for the cleanup.
 */
pascal OSErr
c_KillProcess(ProcessSerialNumberPtr pPSN)
	{
	PEntryPtr		pDeadMeat;
	unsigned long	olda5;
	void			PushOnStateList(PEntryPtr, PEntryPtr *);

	/* Locate the poor thing */
	if ((pDeadMeat = PEntryFromPSN(pPSN)) == nil)
		return(procNotFound);
	
	/* Special case! */
	olda5 = ProcessMgrA5SimpleSetup();
	if (pDeadMeat == pCurrentProcess)
		ExitToShell();
	
	/* Rouse him from his slumber */
	CancelSleep(pDeadMeat);
	
	/* And throw him into the chute */
	if (pDeadMeat->p_condemned == wFalse)
		{
		pDeadMeat->p_condemned = wTrue;
		PushOnStateList(pDeadMeat, &pDyingStateList);
		}

	A5SimpleRestore(olda5);
	return(noErr);
	}

/* TWGetPID. Return the currently running process ID. */
pascal ProcessID
c_TWGetPID(void)
	{
	unsigned long	olda5;
	ProcessID		retval;

	olda5 = ProcessMgrA5SimpleSetup();
	retval = (pCurrentProcess != nil) ? pCurrentProcess->p_mypid : 0;
	A5SimpleRestore(olda5);
	return(retval);
	}

/* TWGetProcInfo. This routine fills in the data of the passed info record for the
 * given process ID. Returns noErr for real processes, or procNotFound for a bogus
 * ProcessID.
 */
pascal long
c_TWGetProcInfo(ProcessID pid, struct procinfo *inforec)
	{
	PEntryPtr		p, pDadProc;
	PCB				*pc;
	THz				savezone;
	unsigned long	olda5;

	if ((p = PEntryFromPID(pid)) == nil)
		return(procNotFound);
	
	olda5 = ProcessMgrA5SimpleSetup();
	inforec->p_state = p->p_state;
	inforec->p_mypid = p->p_mypid;
	inforec->p_type = p->p_type;
	inforec->p_signature = p->p_signature;
	inforec->p_version = p->p_version;
	inforec->p_zone = p->p_zone;
	inforec->p_taskmode = p->p_taskmode;
	inforec->p_needsuspresevts = ((p->p_taskmode & modeNeedSuspendResume) != 0);
	inforec->p_back = ((p->p_taskmode & modeCanBackground) != 0);
	/* NOTE: Add this when next Finder created:
	 * inforec->p_isfirstbackevt = p->p_isfirstbackevt;
	 */
	inforec->p_activateonresume = ((p->p_taskmode & modeDoesActivateOnFGSwitch) != 0);
	inforec->p_dad = ((pDadProc = p->p_dadProcess) != nil) ? pDadProc->p_mypid : 0;
	inforec->p_size = p->p_size;
	inforec->p_ssize = p->p_ssize;
	inforec->p_slices = p->p_slices;
	
	assert(p->p_pcb != nil);
	pc = *p->p_pcb;
	BlockMove(&pc->curapname, &inforec->p_name, 32);
	inforec->p_vrefnum = pc->p_vrefnum;
	
	/* NOTE: Assumes FreeMem can not cause ProcessMgrZone to move, since we restore the
	 * saved zone unconditionally, instead of using SafeSetZone and RestoreZone.
	 */
	savezone = THEZONE;
	THEZONE = p->p_zone;
	inforec->p_freemem = (unsigned long)(*p->p_pcb)->appllimit
		- (unsigned long)((p->p_zone)->bkLim) + FreeMem();
	THEZONE = savezone;
	A5SimpleRestore(olda5);
	return(noErr);	
	}

/* c_GetCurrentProcess.  Return the serial number of the currently running process.
 * NOTE: Must allow for times when there is no current process (e.g. during a switch).
 */
pascal OSErr
c_GetCurrentProcess(ProcessSerialNumberPtr psnStorage)
	{
	PEntryPtr		pCurrProc;
	u_long			oldA5;
	
	oldA5 = ProcessMgrA5SimpleSetup();
	pCurrProc = pCurrentProcess;
	if (pCurrProc != nil)
		*psnStorage = pCurrProc->p_serialNumber;
	else
		SetPSN(kNoProcess, psnStorage);
		
	A5SimpleRestore(oldA5);
	return(noErr);
	}

/* c_GetSystemClientProcess.  Return a process serial number to identify the party
 * responsible for current system activity.  This will be either "the system" or
 * the current application.
 */
pascal OSErr
c_GetSystemClientProcess(ProcessSerialNumberPtr psnStorage)
	{
	OSErr		retval;
	u_long		oldA5;
	
	oldA5 = ProcessMgrA5SimpleSetup();
	
	/* Check depth counter */
	if ((pCurrentProcess != nil) && (pCurrentProcess->p_systemModeLevel == 0))
		retval = GetCurrentProcess(psnStorage);
	else
		{
		SetPSN(kSystemProcess, psnStorage);
		retval = noErr;
		}
		
	A5SimpleRestore(oldA5);
	return(retval);
	}

/* c_GetNextProcess.  Return the next highest process serial number in use.
 * NOTE: Assumes that the process list is in descending order of p_serialNumber.
 */
#define IS_GTE_PSN(src, dst)							\
	(((src)->highLongOfPSN != (dst)->highLongOfPSN) ? 	\
		((src)->highLongOfPSN >= (dst)->highLongOfPSN)	\
		: ((src)->lowLongOfPSN >= (dst)->lowLongOfPSN))
pascal OSErr
c_GetNextProcess(ProcessSerialNumberPtr pParamPSN)
	{
	register PEntryPtr		pProc, pPrev;
	ProcessSerialNumberPtr	pCheckPSN;
	OSErr					retVal = noErr;
	u_long					oldA5;
	
	oldA5 = ProcessMgrA5SimpleSetup();
	
	/* Look for special values */
	if (EqualPSNConst(kCurrentProcess, pParamPSN) && (pCurrentProcess != nil))
		pCheckPSN = &pCurrentProcess->p_serialNumber;
	else
		pCheckPSN = pParamPSN;
	
	/* Search the process list */
	pPrev = nil;
	pProc = pProcessList;
	while (pProc != nil)
		{
		if (IS_GTE_PSN(pCheckPSN, &(pProc->p_serialNumber)))
			break;
		pPrev = pProc;
		pProc = pProc->p_NextProcess;
		}
	
	/* If we actually found something, pProc points to the item after the desired
	 * one (nil if the desired one is last in the list).  pPrev points to the desired
	 * one itself (nil if the desired one is first in the list).  Handle these cases.
	 */
	if (pPrev != nil)
		*pParamPSN = pPrev->p_serialNumber;
	else
		{
		SetPSN(kNoProcess, pParamPSN);
		retVal = procNotFound;
		}
			
	A5SimpleRestore(oldA5);
	return (retVal);
	}

/* c_GetProcessFromLayer.  Identify the process ultimately responsible for the
 * given layer.  If pLayer is -1, caller is asking for the frontmost process.
 */
pascal OSErr
c_GetProcessFromLayer(ProcessSerialNumberPtr pPSN, LayerPtr pLayer)
	{
	LayerPtr		pAppLayer;
	PEntryPtr		pOwnerProc;
	unsigned long	olda5;
	OSErr			retval;
	
	olda5 = ProcessMgrA5SimpleSetup();
	
	/* Set up probable return value */
	retval = noErr;

	/* Weed out some bad cases */
	if ((pLayer == nil) || (pPSN == nil))
		retval = paramErr;
	else
		{
		pAppLayer = (pLayer == (LayerPtr) -1) ? GetFrontAppLayer() : FindAppLayer((WindowPtr) pLayer);
		if ( (pAppLayer != nil) && ((pOwnerProc = LayerOwner(pAppLayer)) != nil) )
			*pPSN = pOwnerProc->p_serialNumber;
		else
			retval = procNotFound;
		}
	
	/* Return kNoProcess if caller goofed, or there is no front process */
	if (retval != noErr)
		SetPSN(kNoProcess, pPSN);
		
	A5SimpleRestore(olda5);
	return(retval);
	}

/* c_SameProcess.  Determine whether two ProcessSerialNumbers refer to the
 * exact same process.  Provided partially for convenience, but mainly so that
 * we can keep the format of a PSN private (e.g. might want PSN to have subfields
 * with abstract values, like machine ID).
 */
pascal OSErr
c_SameProcess(ProcessSerialNumberPtr pPSN1, ProcessSerialNumberPtr pPSN2, Boolean *pResult)
	{
	PEntryPtr					pCurrProc;
	ProcessSerialNumberPtr		pCheckPSN1, pCheckPSN2;
	u_long						oldA5;
	
	/* Weed out the hose heads */
	if ( pPSN1 == nil || pPSN2 == nil || pResult == nil )
		return(paramErr);
		
	oldA5 = ProcessMgrA5SimpleSetup();
	
	/* Convert special values to real PSNs */
	pCheckPSN1 = pPSN1;
	pCheckPSN2 = pPSN2;
	if ((pCurrProc = pCurrentProcess) != nil)
		{
		if (EqualPSNConst(kCurrentProcess, pPSN1))
			pCheckPSN1 = &pCurrProc->p_serialNumber;
			
		if (EqualPSNConst(kCurrentProcess, pPSN2))
			pCheckPSN2 = &pCurrProc->p_serialNumber;
		}
#if	TheFuture
	else	//	DRF #1041525: Handle case where there is no current process
		{
		if (EqualPSNConst(kNoProcess, pPSN1))
			SetPSN(kNoProcess,pCheckPSN1);

		if (EqualPSNConst(kNoProcess, pPSN2))
			SetPSN(kNoProcess,pCheckPSN2);
		}
#endif
	
	/* Compare them */
	*pResult = EqualPSN(pCheckPSN1, pCheckPSN2);
	
	A5SimpleRestore(oldA5);
	return(noErr);
	}

/* c_GetProcessInformation.  Fill caller's record with data about the specified
 * process.
 */

/* Size of first ProcessInfoRec we ever defined */
#define FIRSTPROCESSINFORECLENGTH	(11*sizeof(long) + 2*sizeof(ProcessSerialNumber))

pascal OSErr
c_GetProcessInformation(ProcessSerialNumberPtr pPSN, ProcessInfoRecPtr pStorage)
	{
	PEntryPtr		pProc, pDadProc;
	PCB				*pPCB;
	OSErr			err = noErr;
	FSSpecPtr		pFileSpec;
	FCBPBRec		fcbInfo;
	THz				savezone;
	u_long			oldA5;
	
	/* check for dupes */
	if ((pStorage == nil) || (pStorage->processInfoLength < FIRSTPROCESSINFORECLENGTH))
		return(paramErr);
	
	/* Can't do anything if there is no such process */
	if ((pProc = PEntryFromPSN(pPSN)) == nil)
		return(procNotFound);
	
	/* Fill in the record. */
	oldA5 = ProcessMgrA5SimpleSetup();
	assert(pProc->p_pcb != nil);
	pPCB = *pProc->p_pcb;
	
	if (pStorage->processName != nil)
		BlockMove(&pPCB->curapname, pStorage->processName, 32);
	
	pStorage->processNumber = pProc->p_serialNumber;
	pStorage->processSignature = pProc->p_signature;
	pStorage->processType = pProc->p_type;
	pStorage->processMode = pProc->p_taskmode;

	/* Get process location, size, and amount of free memory.
	 * NOTE: Locates PCB via handle so that we are independant of evaluation order (in
	 * case FreeMem() moves memory).
	 * NOTE: Assumes FreeMem can not cause ProcessMgrZone to move, since we restore saved
	 * zone unconditionally, instead of using SafeSetZone and RestoreZone.
	 */
	pStorage->processLocation = (Ptr) pProc->p_zone;
	pStorage->processSize = pProc->p_size;
	savezone = THEZONE;
	THEZONE = pProc->p_zone;
	pStorage->processFreeMem = (unsigned long) (*pProc->p_pcb)->appllimit 
		- (unsigned long)((pProc->p_zone)->bkLim) + FreeMem();
	THEZONE = savezone;
	pPCB = *pProc->p_pcb;

	/* Get information about launcher */
	if ((pDadProc = pProc->p_dadProcess) != nil)
		pStorage->processLauncher = pDadProc->p_serialNumber;
	else
		SetPSN(kNoProcess, &pStorage->processLauncher);
	pStorage->processLaunchDate = pPCB->launchDate;
	
	/* Get scheduling information */
	pStorage->processActiveTime = pProc->p_activeTime;
	
	/* Get the specification for the application or DA file. */

	if ((pFileSpec = pStorage->processAppSpec) != nil)
		{
		fcbInfo.ioFCBIndx = 0;
		fcbInfo.ioRefNum = pPCB->p_fileRefNum;
		fcbInfo.ioNamePtr = &pFileSpec->name;
		fcbInfo.ioCompletion = nil;
		if ((err = PBGetFCBInfo(&fcbInfo, SyncHFS)) == noErr)
			{
			pFileSpec->vRefNum = fcbInfo.ioFCBVRefNum;
			pFileSpec->parID = fcbInfo.ioFCBParID;
			}
		}
		
	A5SimpleRestore(oldA5);
	return(err);
	}

/* c_SetFrontProcess.  Cause the specified process to become the frontmost. */
pascal OSErr
c_SetFrontProcess(ProcessSerialNumberPtr pPSN)
	{
	PEntryPtr		pProc, pFront;
	u_long			oldA5;
	
	/* Locate the process */
	if ((pProc = PEntryFromPSN(pPSN)) == nil)
		return(procNotFound);
	
	/* Can't switch background-only process to the front! */
	if ((pProc->p_taskmode & modeOnlyBackground) != 0)
		return(appIsDaemon);
	
	oldA5 = ProcessMgrA5SimpleSetup();
	pFront = pFrontProcess;

	/* Is specified process suspended for high-level debugging? */	
	if (pProc->p_state == PRNULL)
		{
		/* Don't allow switch to debuggee unless current process is the debugger. */
		if (pCurrentProcess != pDebugProcess)
			{
			A5SimpleRestore(oldA5);
			return(procNotFound);
			}
			
		/* If debuggee is still in front, all we need to do is allow it to run */
		if (pProc == pFront)
			{
			pProc->p_state = PRREADY;
			A5SimpleRestore(oldA5);
			return(noErr);
			}
		}

	/* Don't switch to fg guy! */
	if (pProc != pFront)
		{		  
		/* Since this may be done from a background app, we wakeup the current
		 * front process so it can start the coercion.
		 */
		CancelSleep(pFront);

		/* Herd him in */
		AddToFrontProcessQueue(pProc);
		}
	
	A5SimpleRestore(oldA5);
	return(noErr);
	}

/* Wakeup.  Wake up the given task, or at least tell MF you'd like to.  We
 * don't adjust the list right now, because this call could be being made by
 * an interrupt routine while we are otherwise looking at the list.  Instead,
 * we flag the list, then do the move at a more synchronous time.
 * NOTE: Should we dump this now that WakeupProcess is the public trap?
 */
short
c_Wakeup(ProcessID pid)
	{
	PEntryPtr		pPEntry;
	
	dbmsg("Current app using unpublished TWWakeup trap!");
	if ((pPEntry = PEntryFromPID(pid)) == nil)
		return(procNotFound);
	
	return(WakeUpProcess(&pPEntry->p_serialNumber));
	}

/* c_WakeUpProcess.  Rouse the specified process from the sleep induced by waiting
 * for an event.  We don't adjust the list right now, because this call could be
 * being made by an interrupt routine while we are otherwise looking at the list.
 * Instead, we flag the list, then do the move at a more synchronous time.
 * NOTE: We zero the p_wakeuptime even if p_state != PRSLEEPING.  This keeps the
 * the app from sleeping at the end of its current event call in some intermediate
 * state like PRUPDATE.  Of course, this only works if the app is in the event call
 * that would sleep him.  If instead, it is in the middle of a series (like you might
 * see for PRUPDATE), the effect is lost.
 */
pascal OSErr
c_WakeUpProcess(ProcessSerialNumberPtr pPSN)
	{
	PEntryPtr		pPEntry;
	u_long			oldA5;

	/* Can't wake someone who isn't sleeping */
	if ((pPEntry = PEntryFromPSN(pPSN)) == nil)
		return(procNotFound);
	
	/* Set flag we'll see in scheduler and/or event calls. */
	oldA5 = ProcessMgrA5SimpleSetup();
	if (pPEntry->p_state == PRSLEEPING)
		napOver = true;
	pPEntry->p_wakeuptime = 0;
#ifdef HAS_AUX_PROCESSMGR
	pPEntry->wakeup_flag++;
#endif HAS_AUX_PROCESSMGR
	A5SimpleRestore(oldA5);

	return(noErr);
	}

/* c_BeginSystemMode.  Switch to a mode where GetSystemClientProcess will indicate
 * "the system".  This is so that the ROM/system can obtain files, memory, etc.
 * that will not be tracked by application death.
 */
pascal OSErr
c_BeginSystemMode(void)
	{
	u_long		oldA5;

	oldA5 = ProcessMgrA5SimpleSetup();
	if (pCurrentProcess != nil)
		pCurrentProcess->p_systemModeLevel += 1;
	A5SimpleRestore(oldA5);
	return(noErr);
	}
	
/* c_EndSystemMode.  Potentially switch out of "system mode".  Is not really out
 * if caller is nested in mode.
 */
pascal OSErr
c_EndSystemMode(void)
	{
	u_long		oldA5;

	oldA5 = ProcessMgrA5SimpleSetup();
	if ((pCurrentProcess != nil) && (pCurrentProcess->p_systemModeLevel != 0))
		pCurrentProcess->p_systemModeLevel -= 1;
	A5SimpleRestore(oldA5);
	return(noErr);
	}


/* NewDesktopLayerOwner.  The shell (Finder) calls this trap to gain ownership of the
 * desktop appearance.  We hand back the DeskPort we found earlier.  The shell will
 * copy the updateRgn into its own window.
 */
pascal OSErr
c_newdesktoplayerowner(LayerPtr *ppNewLayer)
	{
	u_long		olda5;
	OSErr		retval;

	olda5 = ProcessMgrA5SimpleSetup();
	if (desklayerowner == nil)
		{
		*ppNewLayer = desklayer;
		desklayerowner = pCurrentProcess;
		retval = noErr;
		}
	else
		retval = protocolErr;

	A5SimpleRestore(olda5);
	return(retval);
	}

// <12> PEntryFromProcessSerialNumber.  Locate existing process having given ProcessSerialNumber.
//		This routine is a duplicate of PEntryFromPSN in Processes.c so that the Cube-E Finder doesn’t
//		have hard coded offsets to Process Manager globals to do the application
//		assassin stuff.  In the long run, we should get rid of this routine, and
//		implement the assassin as a new launch flag.

pascal PEntryPtr
c_PEntryFromProcessSerialNumber(ProcessSerialNumberPtr pPSN)
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

/* DisableSwitching. This routine arranges such that the calling process
 * will not be switched for any reason.
 */
pascal void
c_DisableSwitching(void)
	{
	unsigned long	olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	dont_switch++;
	A5SimpleRestore(olda5);
	}

/* EnableSwitching. This routine arranges such that the calling process
 * will be switched for any reason.
 */
pascal void
c_EnableSwitching(void)
	{
	unsigned long	olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	if (dont_switch > 0)
		dont_switch--;
	A5SimpleRestore(olda5);
	}

PrTypeString			globalPrTypeString;

/* [GS]etPrTypeStrings. These routines read and write the name of the current
 * printer type for a given process and the current global printer -- the real one.
 * NOTE: The interface should be changed to copy the strings because *pLocalStrPtr
 * is in the middle of the pcb and thus can move.
 */
pascal char
c_GetPrTypeStrings(PrTypeStringPtr *pLocalStrPtr, PrTypeStringPtr *pGlobalStrPtr)
	{
	unsigned long		olda5;
	PrTypeStringHandle	prTypeStringHdl;
	PCB					*pPCB;
	char				enablePrTypeChanges;

	olda5 = ProcessMgrA5SimpleSetup();
#ifndef CHOOSE_CALLS_SETPRTYPESTRINGS	
	Length(&globalPrTypeString) = 0;
	
	/* Make sure ResLoad is true, so we get the data */
	{
	Boolean				oldResLoad = RESLOAD;
	
	RESLOAD = true;
	prTypeStringHdl = GetResource('STR ',-8192);
	RESLOAD = oldResLoad;
	}
	
	if (prTypeStringHdl != nil)
		{
		BlockMove(*prTypeStringHdl, &globalPrTypeString, PRTYPE_STRING_SIZE);
		ReleaseResource(prTypeStringHdl);
		}
#endif
	*pGlobalStrPtr = &globalPrTypeString;
	
	assert(pCurrentProcess->p_pcb != nil);
	pPCB = *pCurrentProcess->p_pcb;
	*pLocalStrPtr = &(pPCB->localPrTypeString);
	if (Length(&(pPCB->localPrTypeString)) == 0)				/* Is there a local one yet? */
		BlockMove(&globalPrTypeString, *pLocalStrPtr, PRTYPE_STRING_SIZE);

	enablePrTypeChanges = pPCB->enablePrTypeChanges;
	
	A5SimpleRestore(olda5);
	return enablePrTypeChanges;
	}
	
pascal void
c_SetPrTypeStrings(PrTypeStringPtr localStrPtr, PrTypeStringPtr globalStrPtr, char enablePrTypeChanges)
	{
	unsigned long		olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	
	if (globalStrPtr != nil)
		BlockMove(globalStrPtr, &globalPrTypeString, PRTYPE_STRING_SIZE);
	
	if (localStrPtr != nil)
		BlockMove(localStrPtr, &((*pCurrentProcess->p_pcb)->localPrTypeString), PRTYPE_STRING_SIZE);

	assert(PRTYPE_CHANGESTATUS_SAME == (-1))
	if (enablePrTypeChanges >= 0)
		(*pCurrentProcess->p_pcb)->enablePrTypeChanges = enablePrTypeChanges;
		
	A5SimpleRestore(olda5);
	}

/* c_TWPostAllPseudoEvent.  Don't need anymore.  SysError instead. (August 90) */
pascal short
c_TWPostAllPseudoEvent(long signature, long tyype, EventRecord *pEvent, u_short priority)
	{
#pragma unused (signature, tyype, pEvent, priority)
	SysError(dsCoreErr);
	return(noErr);
	}

/* c_TWPostPseudoEvent.  Don't need anymore.  SysError instead. (August 90) */
pascal void	
c_TWPostPseudoEvent(ProcessID pid, EventRecord *pEvent, u_short priority)
	{
#pragma unused (pid, pEvent, priority)
	SysError(dsCoreErr);
	}

/* c_SetStdFileDir. Don't support anymore.  SysError instead. (August 90) */
pascal void
c_SetStdFileDir(ProcessID pid, short sfSaveDisk, unsigned long curDirID)
	{
#pragma unused (pid, sfSaveDisk, curDirID)
	SysError(dsCoreErr);
	}

/* c_BackgroundNotify. Can't support anymore.  SysError instead. (November 89) */
pascal void
c_BackgroundNotify(StringPtr msgStringPtr)
	{
#pragma unused (msgStringPtr)
	SysError(dsCoreErr);
	}

#ifdef MODSQUAD
pascal OSErr c_AcceptDrag(void)
	{
	u_long			oldA5;
	OSErr			retval = noErr;

	//Debugger();
	oldA5 = ProcessMgrA5SimpleSetup();
	if (pCurrentProcess && pCurrentProcess->p_state == PRSTARTDRAG)
		gDragState = DRAG_DONE;
	else
		retval = protocolErr;	//	What should I return here?
	A5SimpleRestore(oldA5);
	return(retval);
	}
#endif

/************************************************************************************
 *						Text Services Manager supporting routine.
 ************************************************************************************/
pascal Boolean
c_InlineAware(ProcessSerialNumberPtr pPSN, Boolean aware)
	{
	Boolean			prevValue;
	PEntryPtr		pPEntry;
	u_long			oldA5;

	if ((pPEntry = PEntryFromPSN(pPSN)) == nil)
		return(procNotFound);
	
	oldA5 = ProcessMgrA5SimpleSetup();
	prevValue = pPEntry->p_inlineAware;
	pPEntry->p_inlineAware = aware;
	WakeUpProcess(&pFrontProcess->p_serialNumber);
	A5SimpleRestore(oldA5);

	return(prevValue);
	}

pascal OSErr
c_NewTSMLayerOwner( const ProcessSerialNumber *pPSN)
	{
	OSErr		retval;
	u_long		olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	if (tsmLayerOwner == nil)
		{
		tsmLayerOwner = (PEntryPtr)(pPSN->lowLongOfPSN);		// owner is the driver refnum
		retval = noErr;
		}
	else
		retval = protocolErr;

	A5SimpleRestore(olda5);
	return(retval);
	}

pascal Boolean
c_RouteEventToSWM( Boolean doRoute)
	{
	Boolean			prevValue;
	u_long			oldA5;

	oldA5 = ProcessMgrA5SimpleSetup();
	prevValue = routeEvent;
	routeEvent = doRoute;
	A5SimpleRestore(oldA5);

	return(prevValue);
	}
