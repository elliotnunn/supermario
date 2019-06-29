/*
	File:		Aux.c

	Contains:	Process Mgr support for A/UX.
				NOTE: None of the code in this file is referenced unless the build
				defines the HAS_AUX_PROCESSMGR condition.

	Written by:	David Harrison

	Copyright:	© 1980-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <5>	 8/26/92	DTY		Roll in latest changes for A/UX.
		 <4>	  4/1/91	DFH		(with John Iarocci) Bring this file up to date.
		 <2>	 12/5/90	DFH		Fixed warning in AUX_Block re: unused param.
		 <0>	 12/4/90	DFH		New Today.

*/ 

#include <Files.h>
#include <QuickDraw.h>
#include <Resources.h>
#include <Retrace.h>
#include <Errors.h>
#include <MFPrivate.h>

#include "Data.h"
#include "OSDispatchPrivate.h"
#include "Glue.h"
#include "SysMisc.h"
#include "Aux.h"
#include "Patches.h"
#include "Lomem.h"

/* Variables local to this module */
static short				auxMaxSelector = -1;
static long					ProcessMgrTaskID = -1;
static Getnextevent_ptr		realGNE;
static Eventavail_ptr		realEA;
static AUXDispatch_ptr		realAD;

/* Some function prototypes that should be in (yet another) header file */
OSErr	NewProcess(InternalLaunchPBPtr pParams);
short	GetNextPseudoEvent(short, EventRecord *, Boolean);
void	OpenAppWDs(PEntryPtr);

/* Handy macro to determine whether given event is best handled by AUX. */
#define IsAUXEvent(pEvent)	((pEvent->what >= FIRST_AUX_EVENT) && (pEvent->what <= LAST_AUX_EVENT))

#pragma segment AUX_SEGMENT

/* AUX_Setup.  Set things up as necessary for A/UX. */
void
AUX_Setup(void)
	{
	/* Determine level of functionality.  This check can probably go away once
	 * we settle on a common version of A/UX
	 */
	auxMaxSelector = AUX_Highest();
	
	/* Set extra patches so we can fool ourselves */
	realGNE = (Getnextevent_ptr) NGetTrapAddress(0xa970, ToolTrap);
	NSetTrapAddress((long) AUX_GetNextEvent, 0xa970, ToolTrap);
	
	realEA = (Eventavail_ptr) NGetTrapAddress(0xa971, ToolTrap);
	NSetTrapAddress((long) AUX_EventAvail, 0xa971, ToolTrap);

	realAD = (AUXDispatch_ptr) NGetTrapAddress(0xabf9, ToolTrap);
	NSetTrapAddress((long) AUX_Dispatch_Patch, 0xabf9, ToolTrap);
	}

/* AUX_EventAvail.  Similar to our replacement for getnextevent, we also replace
 * EventAvail for usage by Process Mgr only.
 */
pascal short
AUX_EventAvail(short evtMask, EventRecord *theEvent)
	{
	Boolean			oldMask;
	EventRecord		anEvent;
	
	oldMask = AUX_SetAUXMask(true);
	for (;;)
		{	
		(void) (*realEA)(evtMask, theEvent);
		
		if (IsAUXEvent(theEvent) == false)
			break;
		
		/* if an A/UX event is available, remove it from the queue and deal with it */
		AUX_GetAnyEventGlue(0, &anEvent, 0, 0, true);
		(void) AUX_HandleEvent(&anEvent);
		};
	
	(void) AUX_SetAUXMask(oldMask);
	
	return theEvent->what == nullEvent ? wFalse : wTrue;
	}

/* AUX_Dispatch_Patch. Patch AUX_Dispatch so we can look for a AUX_BEGINAPPLICATION
 * selector and execute within the ProcessManagers context
 */
 pascal long
 AUX_Dispatch_Patch(short selector, char *p)
 {
	unsigned long	olda5;
	long retval;
	
	olda5 = ProcessMgrA5SimpleSetup();
 	
	if (selector != AUX_BEGINAPPLICATION) 
		retval = (*realAD)(selector, p);
	else 
		/* Fix up working directories for home directory and files in APPPARMHANDLE */
		OpenAppWDs(pCurrentProcess);

	A5SimpleRestore(olda5);

	return(retval);
}

/* AUX_Block.  */
void
AUX_Block(PEntryPtr)
	{
	}

/* AUX_CoffLaunch.  If a coff binary toolbox program is executed, it will first issue a
 * UI_ATTACH ioctl, which will send us an attachEvt.  Upon seeing the attach event, we
 * use AUX_MakeLayer to create a minimal process which can then be switched to using the
 * normal context switching code.  The coff binary will then call (via OSDispatch) this
 * routine to really fill the process out.  All this is necessary because the coff
 * program must execute in a separate A/UX process and it's accesses to the shared data
 * areas must be synchronized.
 */
pascal short
AUX_CoffLaunch(u_long reqSize, u_long taskmode, StringPtr procName, short vrefnum, u_long ssize)
	{
#pragma unused (reqSize, taskmode, procName, vrefnum, ssize)
		
	return pCurrentProcess->p_mypid;
	}

/* AUX_FindProc.  Find the process associated with taskId. */
PEntryPtr
AUX_FindProc(taskId)
	{
	PEntryPtr	pProc;

	pProc = pProcessList;
	while (pProc != nil)
		{
		if (pProc->aux_realpid == taskId)
			break;
		
		pProc = pProc->p_NextProcess;
		}
		
	return pProc;
	}

/* AUX_ForkExec.  Fork and exec a new coff binary. */
short
AUX_ForkExecGlue(short vrefnum, char *name)
	{
	ForkExecRec		params;
	
	assert(AUX_FORKEXEC <= auxMaxSelector);
	
	params.vrefnum = vrefnum;
	params.name = name;
	if (AUX_ForkExec(&params) != 0)
		return permErr;
	else
		return params.pid;
	}

/* AUX_GetAnyEventGlue.  Handy routine for calling AUX_GetAnyEvent, since latter is
 * rather awkward.
 */
short
AUX_GetAnyEventGlue(short evtMask, EventRecord *theEvent, long timeout, RgnHandle bounds, Boolean pullit)
	{
	GetAnyEventRec		params;
	
	assert(AUX_GETANYEVENT <= auxMaxSelector);
	
	params.mask = evtMask;
	params.timeout = timeout;
	params.mouseBounds = bounds;
	params.pullIt = pullit;
	
	(void) AUX_GetAnyEvent(&params);
	
	*theEvent = params.event;
	
	return theEvent->what != nullEvent ? -1 : 0;
	}

/* AUX_GetMinTimeout.  Compute the minimum timeout necessary, we use this so we can
 * sleep in the kernel instead of spinning in user mode.
 */
long
AUX_GetMinTimeout(void)
	{
	extern Boolean napOver;
	
	if ((pFrontProcess->p_waitnexteventlevel > 0)	/* GetNextEvent never blocks */
		&& (coercionState == CS_DONE) 				/* if coercing, don't want physical events */
		&& (napOver == 0)							/* somebody needs attention */
		&& (pSleepingStateList != nil))				/* somebody is sleeping */
		return (pSleepingStateList->p_wakeuptime - TICKS);
	else
		return 0;
	}

/* AUX_GetMouseBounds.  Compute the rectangle for mouse moved events.  Once again, this
 * is so we can sleep in the kernel instead of user mode spinning.
 */
RgnHandle
AUX_GetMouseBounds(void)
	{
	extern RgnHandle mousebox;
	
	return mousebox;
	}

/* AUX_GetNextEvent.  Our replacement for the real GetNextEvent.  This isn't the one
 * seen by the user program, but is the one we will use to actually get events out of
 * the queue.  Ours is different because we can get some funky events back from the
 * kernel and mere mortals shouldn't ever get them.
 *
 * NOTE: ProcessManager overpatches this trap, but calls us in event_common when
 *		the process state is PRRUN.
 */
pascal short
AUX_GetNextEvent(short evtMask, register EventRecord *theEvent)
	{
	Boolean		oldMask;
	short		realRetVal;
	
	oldMask = AUX_SetAUXMask(true);
	
	while ((realRetVal = (*realGNE)(evtMask, theEvent)) && AUX_HandleEvent(theEvent))
		{
		/* Just loop.  All the work is done in the loop control. */
		}

	(void) AUX_SetAUXMask(oldMask);
	
	return realRetVal;
	}

/* AUX_GetTaskGlue.  Handy utility to return the A/UX style task identifier of this
 * process so we can do a real context switch to it later.
 */
long
AUX_GetTaskGlue(void)
	{
	long		taskId;
	
	assert(AUX_GETTASK <= auxMaxSelector);
	AUX_GetTask(&taskId);
	return taskId;
	}

/* AUX_HandleEvent.  Handle an A/UX specific event. */
short
AUX_HandleEvent(EventRecord *theEvent)
	{
	short		retVal;

	retVal = wTrue;
	if (IsAUXEvent(theEvent) == false)
		return wFalse;
	
	switch(theEvent->what)
		{
		/* somebody's UI_SETSELECT broke, make 'em runnable. */
		case	auxSelectEvent:
			AUX_MakeRunnable(theEvent->message);
			break;
		
		/* a new layer was asynchronously created, fit it in to the scheduling */
		case	auxAttachEvent:
			AUX_MakeLayer(theEvent->message, theEvent->modifiers, theEvent->when);
			break;
		
		/* a layer went away by mistake (kill(9...) for instance, clean it up */
		case	auxExitEvent:
			AUX_KillLayer(theEvent->message);
			break;
		}
	theEvent->what = nullEvent;

	return retVal;
	}

#define LAUNCH_DELAY		(60*60)						/* one minute */

/* AUX_IdleProc.  */
void
AUX_IdleProc(void)
	{
	register PEntryPtr	myProc;
	unsigned long		olda5;
	EventRecord			anEvent;

	olda5 = ProcessMgrA5Setup();
	myProc = pCurrentProcess;
	for (;;)
		{
		assert(myProc->aux_waitproc);
		
		(void) AUX_GetAnyEventGlue(0, &anEvent, LAUNCH_DELAY, 0, true);
		
		/* Handle the attach.  aux_restore_ctx should get us out of here, never to return. */
		if ((anEvent.what == auxAttachEvent) && (anEvent.message == myProc->aux_waitproc))
			{
			myProc->aux_realpid = myProc->aux_waitproc;
			myProc->aux_waitproc = 0;
			AUX_SwitchGlue(myProc->aux_realpid);
			aux_restore_ctx(pCurrentProcess->p_sp);
			dbmsg("How the ≈ did I get here?");
			}
		
		/* AUX event was not the usual attach, or an attach came from an incorrect
		 * AUX process.  Feebly try to handle the event.  Then die.
		 */
		(void) AUX_HandleEvent(&anEvent);
		AUX_Kill(myProc->aux_waitproc);
		ExitToShell();
		}
	}

/* AUX_KernelEvents.  */
short
AUX_KernelEvents(EventRecord *theEvent)
	{
	short		foundSome;
	
	foundSome = 0;
	while (AUX_GetAnyEventGlue(0, theEvent, 0, 0, 1) && IsAUXEvent(theEvent) != 0)
		{
		(void) AUX_HandleEvent(theEvent);
		foundSome++;
		}
	
	return foundSome;
	}

/* AUX_KillLayer.  Get rid of an abnormally terminated process */
void
AUX_KillLayer(long taskId)
	{
	register PEntryPtr		pProc;
	
	if ((pProc = AUX_FindProc(taskId)) != nil)
		(void) KillProcess(&pProc->p_serialNumber);
	}

/* AUX_Launch.  */
OSErr
AUX_Launch(InternalLaunchPBPtr pParams)
	{
	short		realPid;
	PEntryPtr	pProc;
	OSErr		err;
	WDPBRec     wd;

	/* setup a WDPBRec to pass to AUX_ForkExec through the AUXDispatch */
	wd.ioCompletion = nil;
	wd.ioNamePtr = nil;
	wd.ioWDProcID = 0;
	wd.ioVRefNum = pParams->ilAppSpecPtr->vRefNum;
	wd.ioWDDirID = pParams->ilAppSpecPtr->parID;
	PBOpenWD(&wd, true);
    
	realPid = AUX_ForkExecGlue(wd.ioVRefNum, pParams->ilAppSpecPtr->name);

	/* Close the WD, if we opened it */
	if (wd.ioWDCreated != 0)
		PBCloseWD(&wd, SyncHFS);

	/* Negative result is an OSErr.  Result should never be 0.  Positive result is AUX pid. */
	if (realPid < 0)
		return((OSErr) realPid);

	pParams->ilTaskMode |= COFF_BINARY;
	if ((err = NewProcess(pParams)) != noErr)
		{
		dbmsg("AUX_Launch: NewProcess failed");
		AUX_Kill(realPid);
		return(err);
		}
	
	pProc = pParams->ilResultProc;
	pProc->aux_waitproc = realPid;
	pProc->aux_realpid = AUX_ProcessMgrTask();

	return(noErr);
	}

/* AUX_MakeLayer.  Create an empty process.  After the A/UX process switch it will be
 * filled out by whomever did the UI_ATTACH.
 */
void
AUX_MakeLayer(long taskId, unsigned short flags, long size)
	{
	register PEntryPtr	pProc;
	InternalLaunchPB	launchPB;
	CoffName			coffname;

	coffname.taskid = taskId;
	if ((pProc = AUX_Unblock(taskId)) != nil)
		return;

	if (AUX_COFF_FSSpec(&coffname) != noErr)
		return;
	
	launchPB.ilAppSpecPtr = &coffname.fsspec;
	launchPB.ilTaskMode = COFF_BINARY | ASYNC_LAUNCH | (unsigned long) flags;
	launchPB.ilDAInfoPtr = nil;
	launchPB.ilPartitionSize = size + DEFLTSTACK - MACPLUS_DEFLTSTACK;
	launchPB.ilStackSize = DEFLTSTACK;
	launchPB.ilAppParameters = nil;
	launchPB.ilAppParamHdl = nil;
	if (NewProcess(&launchPB) != noErr)
		{
		dbmsg("AUX_MakeLayer: NewProcess failed");
		AUX_Kill(taskId);
		return;
		}
	
	pProc = launchPB.ilResultProc;
	pProc->aux_realpid = taskId;
	pProc->aux_waitproc = 0;
	
	(void) WakeUpProcess(&pFrontProcess->p_serialNumber);
	}

/* AUX_MakeRunnable.  Make an application contained in a different task runnable.  taskId
 * refers to the kernel event layer identifier we got back from AUX_GetTask and stowed
 * away in auxInfo.
 */
void
AUX_MakeRunnable(long taskId)
	{
	PEntryPtr		pProc;
	
	if ((pProc = AUX_FindProc(taskId)) != nil)
		(void) WakeUpProcess(&pProc->p_serialNumber);
	}

/* AUX_ProcessMgrTask.  Return the task descriptor for Macintosh Applications */
long
AUX_ProcessMgrTask(void)
	{
	if (ProcessMgrTaskID < 0)
		AUX_GetTask(&ProcessMgrTaskID);
		
	return ProcessMgrTaskID;
	}

/* AUX_SwitchGlue.  Force an A/UX kernel context switch. */
void
AUX_SwitchGlue(long taskId)
	{
	long			rval;
	EventRecord		anEvent;
	
	assert(AUX_SWITCH <= auxMaxSelector);

	/* If the Process Mgr process this task is associated with is gone, kill this task
	 * after the switch, if this is the original Process Mgr process we have to
	 * leave it laying around anyway.
	 * NOTE: Inverting taskID tells AUX_Switch to kill the originating process after
	 * enabling the new one.
	 */
	if (((rval = AUX_GetTaskGlue()) != AUX_ProcessMgrTask()) && AUX_FindProc(rval) == 0)
		taskId = (-(taskId));

	for (;;)
		{
		/* Try switch.  On error we try to destroy the layer (AUX_KillLayer won't
		 * return).  If it does, we aren't in a valid process, so we pick a new one.
		 */
		if (AUX_Switch(taskId) != 0)
			{
			AUX_KillLayer(taskId);
			cpu_resched();
			}
		
		/* Normally, when we get back from the AUX_Switch, we will have already been
		 * made the current process.  If the task id of this a/ux process is not the
		 * same as where we think we ought to be, we got woken up because the current
		 * process died (most likely, perhaps theres something else, so we're pretending
		 * to be paranoid) so we reap and handle any a/ux events.  If it's true that
		 * the current process really did die, we will eventually receive an exitEvent,
		 * which will result in the associated Process Mgr process being killed and the
		 * processor reschedule, in which case we won't ever return to this routine.
		 */
		if (pCurrentProcess->aux_realpid == AUX_GetTaskGlue())
			break;
			
		/* AUX_HandleEvent won't return if the process we're switching to has exited,
		 * thus giving us an exitEvent
		 */
		do
		{
			(void) AUX_GetAnyEventGlue(0, &anEvent, 0l, 0l, true);
		} while (AUX_HandleEvent(&anEvent));
		
		dbmsg("AUX_SwitchGlue: probably shouldn't ever get here");
		
		/* having handled whatever broke us out of our interminable sleep, try again */
		}
	}

/* AUX_Unblock.  Find the nacent Process Mgr process associated with the given taskID,
 * and declare it alive and well.
 */
PEntryPtr
AUX_Unblock(long taskId)
	{
	PEntryPtr	pProc;

	pProc = pProcessList;
	while (pProc != nil)
		{
		if (pProc->aux_waitproc == taskId)
			{
			pProc->aux_waitproc = 0;
			pProc->aux_realpid = taskId;
			break;
			}
		
		pProc = pProc->p_NextProcess;
		}
		
	return pProc;
	}

/* AUX_WaitForEvent.  */
short
AUX_WaitForEvent(short mask, EventRecord *theEvent)
	{
	if (pCurrentProcess)
		if (GetNextPseudoEvent(mask, theEvent, false))
			return wTrue;

	(void) AUX_GetAnyEventGlue(mask, theEvent, AUX_GetMinTimeout(), AUX_GetMouseBounds(), false);
	
	return theEvent->what == nullEvent ? wFalse : wTrue;
	}

/* AUX_WaitNextEvent.  */
short
AUX_WaitNextEvent(short mask, EventRecord *theEvent, long timeout, RgnHandle mbox)
	{
	short		rval;
	
	AUX_SetTimeOut(timeout);
	AUX_SetBounds(mbox);
	
	rval = GetNextEvent(mask, theEvent);
	
	AUX_SetTimeOut(0);
	AUX_SetBounds(nil);
	
	return rval;
	}
