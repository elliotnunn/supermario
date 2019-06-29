/*
	File:		Schedule.c

	Contains:	Process Mgr scheduling routines.

	Written by:	Phil Goldman and Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<27>	11/11/92	FS		Modify the process manager to send events to the shell process
									if a window in the TSM layer has the magic window kind (13)
		<26>	 8/26/92	DTY		Change ModSquad’s funky conditional to use #ifdefs like everyone
									else.
		<25>	 8/17/92	hjcr	Adding support for OCE's "defer drag" feature.  Conditionally
									compiled under MODSQUAD
		<24>	 7/28/92	DCL		Included the private Layers.h because applicationFloatKind &
									systemFloatKind constants were moved out of the public eye and
									into the private Layers header file.
		<23>	 6/22/92	YK		#1033306 <JH>: Always remove the mouse down event for a floating
									window from the event queue.
		<22>	  6/8/92	YK		<JH>: A boolean variable may not be either 0 or 1.  (Probably
									$FF)   So ‘==true’ should be ‘!=false’.
		<21>	  6/8/92	JH		Removing PtInRgn from imLayerMouseDown. This is a superfluous
									test since the swapCurLayer, FindWindow, SetCurLayer will always
									do the right thing. In addition, for the PtInRgn call to work
									the layer manager will always have to maintain the structrgn
									state as it is maintained today. This assumption will break
									(does break?) the shuffle window manager changes done by Dave
									Collins for O.C.E.
		<20>	 5/11/92	JH		#1029361 <KST>: Need to determine whether to send event to SWM
									driver via the WindowKind and not the refcon. imLayerMouseDown
									changed to look at the WindowKind.
		<19>	 4/29/92	YK		#1028427 <JH>: Pass mouse down events to the front application
									when the front app is inline aware.
		<18>	 4/25/92	YK		#1027486: Move the code, which checks update regions for TSM
									windows, from nexttask() to event_common() in EventMgrPatches.c.
		<17>	 4/22/92	JH		#1027915,<YK>: YK,#1027915,If the an input method opens a
									floating window in the context of a TSM unaware application that
									window gets assigned to the swmdriver. However, if the user then
									switches to a TSM aware application any mousedown events will be
									routed to the swmdriver rather than the TSM aware application.
									That means that the input method gets an A5 world that belongs
									to the Process Manager and not the TSM aware application. When
									the input method subsequently tries to callback to the
									application a5 belongs to the process manager. If the
									application depends on being able to access globals in the
									AppleEvent handlers we get some fancy crashes. To fix this
									problem we make a last minute check to see if the current
									application is TSM aware before call SendEventToSWMDriver. If
									the app is TSM aware we change the pTarg to route to the current
									application.
		<16>	  4/9/92	KST		#1025131,<JH>: Changed SWM from a background app to a driver so
									that input method can do dialog, change cursor, access the menu,
									and type into the dialog window.
		<15>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		<14>	 10/9/91	YK		Added imLayerMouseDown to get mouse down events in Text Service
									Floating window.
		<13>	  2/6/91	DFH		ewa,WS#DFH-910205a:Changed BringProcessToFront to call
									MakeTopLayer if the destination is a high-level debuggee being
									resumed.
		<12>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
		<11>	 1/14/91	DFH		(JDR) Conditionalize out AUX support.
		 <9>	  1/8/91	DFH		(rdd) Simplified BringProcessToFront() since some of its work is
									now done by CallDebugger(), and pErrantProcess is no longer
									needed.
		 <8>	12/18/90	DFH		Removed fgAppsUnexecuted mechanism, since front switch queue
									works better.
		 <7>	12/17/90	DFH		Fixing fg_resched to not consider new apps and front switches
									when the event is not a null event.
		 <6>	12/14/90	DFH		Added SynchIdleTime call in nexttask.
		 <5>	 12/5/90	DFH		Integrate AUX support. Also, remove GiveDriversTime         call
									from nexttask, since sleep is now based on drivers'
		 <4>	 12/3/90	RLC		<ksm> Change call to IsFrontWindowModal() to call the newer
									GetFrontWindowModalClass instead.
		 <3>	11/27/90	DFH		Removed unneeded parameter from cpu_resched.
		 <0>	 x/xx/86	PYG		New Today.

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
#include <retrace.h>
#include <segload.h>
#include <desk.h>
#include <dialogs.h>
#include <DialogsPriv.h>
#include <errors.h>
#include <EPPC.h>
#include <MFPrivate.h>
#include <Files.h>
#include <Devices.h>
#include <TextServices.h>
#include <Layers.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Aux.h"
#include "Puppet.h"
#include "OSDispatchPrivate.h"


/* Some function prototypes that should be in (yet another) header file */
void			pollmouse(void);
void			clkint(void);
unsigned short	ApplicationMenuItemFromPEntry(PEntryPtr);
void			PostPseudoEvent(PEntryPtr pProc, EventRecord *pEvent);

/* Function prototypes internal to this file */
PEntryPtr		nexttask(void);
PEntryPtr		nextback(void);
void			PrepareForUpdate(PEntryPtr);

/************************************************************************************
 *																					*
 *							Process scheduling routines								*	
 *																					*
 ************************************************************************************/

/* BringProcessToFront. This fellow switches the given process to the frontmost,
 * foremost, frontmost user application.
 */
void
BringProcessToFront(PEntryPtr pNewProc)
	{
	PEntryPtr		localCurrProc;

	/* Accept no substitutes.  This may happen, for example, if pNewFrontProcess gets
	 * cleared out because the app we are switching to croaked while foreground switching.
	 */
	if (pNewProc == nil)
		return;
	
	localCurrProc = pCurrentProcess;
	if (pNewProc->p_state == PRNULL)
		{
		/* Only the debugger can switch into debuggees */
		if (localCurrProc != pDebugProcess)
			return;
		
		/* Re-enable process list item.  Bring the application layer forward right now,
		 * on the assumption that this is where it was when the process was suspended.
		 * Otherwise, it would not come forward until the next event call.  Window ops
		 * in the interim would go awry.
		 * NOTE:  We *do* wait 'til the next event call to draw the menu bar, since we
		 * need to be in that app's context when we call DrawMenuBar().
		 */
		if (pNewProc->p_layer != nil)
			{
			EnableItem(ApplicationMenuHdl, ApplicationMenuItemFromPEntry(pNewProc));
			MakeTopLayer(pNewProc->p_layer);
			}
		
		/* Have to run through initial stuff, possibly. */
		coercionState = (pNewProc->p_haseditcmdkeys == (-1)) ? CS_INIT : CS_DONE;
		}
		
	/* Any time we do a high-level switch we are no longer a scratchpad task */
	localCurrProc->p_puppetvars.puppetMasterProc = nil;
	
	/* Save off his system event mask */
	(*localCurrProc->p_pcb)->sysevtmask = SYSEVTMASK;
	
	pFrontProcess = pNewProc;
	localCurrProc->p_state = PRREADY;
	
	/* Bring new front process out of its slumber so it can be running */
	CancelSleep(pNewProc);
	
	pNewProc->p_state = PRMOVING;
	switch_task(pNewProc);
	}

Boolean
imLayerMouseDown( EventRecord *theEvent, Boolean eventInQueue)
	{
	//PEntryPtr			pTarg;
	EventRecord			evtRecord;
	WindowPtr			floatWindow;
	LayerPtr			currLayer;
#ifdef MODSQUAD
	short				floatWindowKind;
#endif
	
	if ( (theEvent->what == mouseDown) && (IMLayer!=nil) && (((LayerPeek)IMLayer)->visible)	 )
		{
		currLayer = SwapCurLayer( IMLayer);
		FindWindow( theEvent->where, &floatWindow);
		SetCurLayer( currLayer);
		if ( floatWindow==nil ) {
			return false;
		}
		
		if ( eventInQueue ) {							// remove the event before we send it to SWM
			GetOSEvent( mDownMask, &evtRecord);
		}

		
#ifdef MODSQUAD
#define shellFloatKind 13
		if (!routeEvent || pFrontProcess->p_inlineAware!=false) 			// before we go off half-cocked lets make double sure we go the right place
			{
			floatWindowKind = ((WindowPeek)(floatWindow))->windowKind;
			if (floatWindowKind == shellFloatKind)
				PostPseudoEvent(pShellProcess, theEvent);					// route to the shell application
			else if (floatWindowKind == applicationFloatKind)
				PostPseudoEvent(pFrontProcess, theEvent);					// route to the front application	
			}
#else
		if ( !routeEvent || pFrontProcess->p_inlineAware!=false || ((WindowPeek)(floatWindow))->windowKind == applicationFloatKind ) 				// before we go off half-cocked lets make double sure we go the right place
			PostPseudoEvent( pFrontProcess, theEvent);							// route to the front application						
#endif
		else 
		if ( ((WindowPeek)(floatWindow))->windowKind == systemFloatKind ) 		// this layer belongs to the SWM driver
			SendEventToSWMDriver( theEvent );									// so route the event to driver
		


		theEvent->what = nullEvent;
		return true;
		}
	else
		return false;
	}



/*	SendEventToSWMDriver:
		Send event to SWM driver, and TSM's input methods.
		Be careful with the deadlock situation because we are using sync. control call .....
		Changed to use PBControlDirect to call the driver so that ModalDialog can use
		SWM driver in a recursive way.

	Called by:
		1. "imLayerMouseDown" when user click in SWM or IM's floating window. (this file)
		2. "imLayerKeyDown" when user press any key. (EventMgrPatches.c)
*/


#pragma parameter __D0 PBControlDirect(__A0)			// has to make direct call to SWM
pascal OSErr PBControlDirect(ParmBlkPtr paramBlock)
 = 0xA204; 


void SendEventToSWMDriver( EventRecord  *theEvent )
{
	CntrlParam				cp;
	unsigned long			oldA5;
	
	oldA5 = ProcessMgrA5SimpleSetup();
	
	if ( tsmLayerOwner!=nil )
		{
		cp.csCode = accEvent; 
		(*(EventRecord **)(cp.csParam)) = theEvent;
		cp.ioCRefNum = (short) (((int) tsmLayerOwner) >> 16);
		(void)PBControlDirect( (ParmBlkPtr)&cp);			/* nothing we could do about an error so ignore it. */
		}
	A5SimpleRestore(oldA5);
}


/* fg_resched.  Called by the foreground application during an event call.
 * It returns a pointer to the application that should be frontmost.
 */
PEntryPtr
fg_resched(register EventRecord *pOfficialEvent, Boolean pullEvent)
	{
	WindowPeek				pWindow;
	register LayerPtr		pLayer;
	register PEntryPtr		pProc;
	register PEntryPtr		pCurrProc;
	EventRecord				evtRecord;
	short					part;
	Boolean					foundMouseDown;
	char					scratchpadResume;
	short					modalDialogActive;
	EventRecord				*pGivenEvent = pOfficialEvent;
	Boolean					foundInQueue;

#ifdef MODSQUAD
	Boolean					doRepost = false;
#endif
	pCurrProc = pCurrentProcess;

	/* Disallow foreground switches if frontmost window is a modal dialog.
	 * NOTE: This check does not halt a foreground switch that is already in
	 * progress (this is correct, since the switch MUST have been initiated
	 * when there was not a modal up, and we'd like to finish the switch).
	 * No one should be putting up modals during a foreground switch anyway.
	 */
	if ((GetFrontWindowModalClass(&modalDialogActive) == noErr) && (modalDialogActive == dBoxProc))
			{
			imLayerMouseDown( pOfficialEvent, !pullEvent);	//	
			return(pCurrProc);
			}
				
	/* Check whether there are foreground switches waiting to happen.
	 * NOTE: CURACTIVATE <= 0 means no window activation in progress.  The 32-bit dirty
	 * check of the high bit checks whether a window activation was canceled.
	 */
	if ( (pOfficialEvent->what == nullEvent) && ((long) CURACTIVATE <= 0) )
		{		
		/* Any pending programmatic switches? */
		if ((pProc = PopFrontProcessQueue()) != nil)
			{
			CancelSleep(pProc);
			return(pProc);
			}
		}
		
	/* Now check for the existence of a mouse click in the event queue (or just dequeued).
	 * If there is one, and it belongs to another layer, we set the wheels in motion to
	 * bring that layer to the foreground (i.e., we return its PEntryPtr and let the
	 * caller worry about this).
	 */
	
	/* The mouse is down if either a) the passed event was a mouseDown, or b) the *first*
	 * physical event is a mousedown (i.e. passed is a nullEvent, and the first available
	 * is mouseDown).
	 */
#define everyPhysicalEvent (mDownMask+mUpMask+keyDownMask+keyUpMask)	
	foundMouseDown = false;
	foundInQueue = false;
	if (pOfficialEvent->what == mouseDown)
	 	foundMouseDown = true;
	else if (pOfficialEvent->what == nullEvent)
	 	{
		pOfficialEvent = &evtRecord;
		foundMouseDown = OSEventAvail(everyPhysicalEvent, pOfficialEvent) && (pOfficialEvent->what == mouseDown);
		foundInQueue = foundMouseDown;
		}
	
	if ( foundMouseDown && imLayerMouseDown( pOfficialEvent, (!pullEvent) || foundInQueue) )
		{
		pGivenEvent->what	 = nullEvent;
		foundMouseDown = false;
		}

	/* Well, is the mouse down? */
	if (foundMouseDown)
		{		
		/* Which way did it go, George? */
		if ( ((part = FindWindow(pOfficialEvent->where, &pWindow)) == inDesk) ||
			 ((part == inSysWindow) && (pWindow == nil)) )
			{
			pProc = ((pLayer = LayerFind(pOfficialEvent->where)) != nil)
					? LayerOwner(pLayer) : desklayerowner;
			
			if ((pProc != nil) && (pProc != pCurrProc) && (pProc->p_state != PRNULL))
				{
#ifdef MODSQUAD
				/* Hide the mousedown from the current application */
				pOfficialEvent->what = nullEvent;
#endif
					
				if ((pOfficialEvent->modifiers & optionKey) != 0)
					HideLayer(GetLayer(), false);
#ifdef MODSQUAD
				else if (gDragMgrIsAvailable)
					{
					LayerPtr currLayer;
					WindowPtr bgWindow;
					
					currLayer = SwapCurLayer(pLayer);
					FindWindow(pOfficialEvent->where, &bgWindow);
					SetCurLayer(currLayer);
					if (IsDragAware(bgWindow, &pProc->p_serialNumber))
						{
						short saveState;
						if (pOfficialEvent == &evtRecord || !pullEvent)
							GetOSEvent(mouseDown, &evtRecord);	//	remove event from queue
						gDragState = DRAG_INIT;
						gDragEvent = *pOfficialEvent;
						//gStartDragTime = pOfficialEvent->message;
						//gStartDragPosition = pOfficialEvent->where;
						saveState = pCurrProc->p_state;
						pCurrProc->p_state = PRREADY;
						gFrontDragProcess = pCurrProc;
						CancelSleep(pProc);
						if (pProc->p_state == PRREADY)
							pProc->p_state = PRBACKRUN;
						else
							Debugger();
						switch_task(pProc);
						pCurrProc->p_state = saveState;
						if (gDragState == DRAG_DONE || gDragState == DRAG_INIT)
							{
							if (gDragState == DRAG_INIT)
								{
								//	Weird.  The app died I guess. ignore mouseup/down pair
								GetOSEvent(mUpMask, &evtRecord);
								gDragState = DRAG_DONE;
								}
							/* Hide the mousedown from the current application */
							pOfficialEvent->what = nullEvent;
							return pCurrProc;
							}
						assert(gDragState == DRAG_IGNORED);
						gDragState = DRAG_DONE;
						doRepost = true;
						}
					}
#else
//		I moved this code before the previous if
				/* Hide the mousedown from the current application */
				pOfficialEvent->what = nullEvent;
#endif

				/* Re-post event if it was pulled, so next app sees them.  We
				 * use PPostEvent so we can pass on the mouse position of the
				 * original event.  PPostEvent and PostEvent always use the
				 * current mouse position, which will have changed if the user
				 * is clicking on the run.
				 */
#ifdef MODSQUAD
				if (((pOfficialEvent != &evtRecord) && (pullEvent)) || doRepost)
#else
				if ((pOfficialEvent != &evtRecord) && (pullEvent))
#endif
					{
					EvQElPtr		pEvtQEl;
					
					if (PPostEvent(mouseDown, pOfficialEvent->message, &pEvtQEl) == noErr)
						pEvtQEl->evtQWhere = pOfficialEvent->where;
						
					if ( (GetOSEvent(mUpMask, &evtRecord)) &&
						(PPostEvent(mouseUp, evtRecord.message, &pEvtQEl) == noErr) )
						pEvtQEl->evtQWhere = evtRecord.where;
					}
				
				return(pProc);
				}
			}
		}

	/* End of normal checks.  Now check briefly for any puppet string activity.
	 * puppetMasterProc == nil means no puppeting at all, puppetEppcMsgBlk != nil
	 * means the puppeting is being done for AppleEvent translation (where there
	 * are no "special" ways out).
	 */
	if ( (pCurrProc->p_puppetvars.puppetMasterProc == nil)
		|| (pCurrProc->p_puppetvars.puppetEppcMsgBlk != nil) )
		return(pCurrProc);
	
	/* If we get to this point we are in a scratchpad task.  There are two
	 * "special" ways out of this: timeout, and hitting cmd-comma.
	 */
	if ( (pCurrProc->p_puppetvars.puppetTimeout != 0)
		&& (--(pCurrProc->p_puppetvars.puppetTimeout) == 0) )
		scratchpadResume = ResumeTimeout;
	
	else if ( (pOfficialEvent->what == keyDown)
		&& (*((char *) &pOfficialEvent->message + 2) == SCRATCHPADRETURNKEYCODE)
		&& ((pOfficialEvent->modifiers & cmdKey) != 0) )
			{
			/* Don't let app see it */
			pOfficialEvent->what = nullEvent;
			
			/* To force clipboard coercion */
			cutCopyCount++;
			
			/* Set return value */
			scratchpadResume = ResumeFromScratchpad;
			pCurrProc->p_state = PRPUPPET;
			}
	
	else
		return(pCurrProc);
	
	/* We're escaping!  Make the "switch out" instruction list active */
	PushInstrsHdl(pCurrProc, pCurrProc->p_puppetvars.puppetOutInstrsHdl, evtMouseGlobal);
	pCurrProc->p_puppetvars.puppetOutInstrsHdl = (Handle) -1;
	pProc = pCurrProc->p_puppetvars.puppetMasterProc;
	pProc->p_puppetvars.masterResumeMsg = scratchpadResume;
	return(pProc);
	}
	
/* cpu_resched. This routine is called when there is a chance to reschedule the
 * CPU. This comes when:
 *	1) the foreground task has received enough NULL events
 *	2) an updating background task has completed, and is returning the CPU.
 * If appropriate, the calling process is switched out for a while.
 */
void
cpu_resched(void)
	{
	register PEntryPtr		pProc, pNewProc;

	/* processes which are already running are re-eligible for execution */
	pProc = pCurrentProcess;
	if ( (pProc != nil) &&
		((pProc->p_state == PRRUN) || (pProc->p_state == PRBACKRUN) || (pProc->p_state == PRUPDATE)) )
			pProc->p_state = PRREADY;
		
	/* switch to a different process, if appropriate */
	if ((pNewProc = nexttask()) != pProc)
		switch_task(pNewProc);
	}

/* nexttask.  This routine is called by resched to determine who the next task to run is.
 * In the current world, the view is:
 *	1) the first guy with remaining updates
 *	2) A desk layer with pending updates
 *	3) alternately foreground and round robin background processes.
 *
 * NOTE: Assumes that current app's state is PRREADY, not PRRUN, PRBACKRUN, etc.  This
 * is why cpu_resched() changes the current apps state to PRREADY.
 *
 * NOTE: You found it!  This routine houses the "main loop" of the Process Mgr where the
 * PC spins when there are no processes that need time.
 */ 

PEntryPtr 
nexttask(void)
	{
	EventRecord		theEvent;
	PEntryPtr		pProc;
	short			fgstate;

	/* Look everywhere for an update.  Normally, desk updates will be found by the
	 * CheckUpdateIn(&theEvent, MFLayer), since the owner (Finder) has a window
	 * in its windowlist that shares the updateRgn handle.  It will not be found, tho,
	 * if the owner is currently hidden (i.e. because its layer is invisible).  We
	 * make the check ourselves.
	 */
	pProc = nil;
	if (CheckUpdateIn(&theEvent, MFLayer))
		pProc = LayerOwner(FindAppLayer((WindowPtr) theEvent.message));
	else if (EmptyRgn(((WindowPeek) desklayer)->updateRgn) == false)
		pProc = desklayerowner;


	/* Found one.  pProc is the handler.  Check pProc's viability, then schedule it
	 * to get time.
	 */
	if (pProc != nil)
		{
		/* Don't use this update if the controlling process is suspended */
		if (pProc->p_state == PRNULL)
			pProc = nil;

		/* Check for the current process ending its quantum without even wanting
		 * to handle the update.  Don't let him hog the machine forever.
		 */
		else if ( (pProc == pCurrentProcess)
				&& ((pProc->p_eventmask & updateMask) == 0)
				&& (TICKS >= nextswitch) )
			pProc = nil;
	
		/* Did we unearth any necessary updates? */
		if (pProc != nil)
			PrepareForUpdate(pProc);
		}
		
	/* No updates happening.  Look for general reasons to reschedule. If there is
	 * absolutely nothing to do, idle here, polling various sources, until an app
	 * needs time.
	 */
	while (pProc == nil)
		{
		/* Because we use pFrontProcess, which is guaranteed valid only if CS_DONE */
		assert(coercionState == CS_DONE);

		/* wake up sleeping processes whose naptime is over */
		clkint();
		
		/* generate mouse moved... */
		pollmouse();
				
		/* See if any notifications have come in */
		if (pFrontProcess->p_state == PRSLEEPING
			&& (NMQHDRPTR != nil && NMQHDRPTR != (Ptr)(-1) && NMQHDRPTR->qHead != nil) )
			CancelSleep(pFrontProcess);
		
		/* see if it's time to switch to the frontmost task */
		fgstate = pFrontProcess->p_state;
		if (pCurrentProcess != pFrontProcess)
			{
			/* Well? */
			if (fgstate == PRREADY)
				{
				pProc = pFrontProcess;
				pProc->p_state = PRRUN;
				break;
				}
			
			/* See if any puppet strings have come in from a BG process */
			if (fgstate == PRPUPPET)
				{
				pProc = pFrontProcess;
				break;
				}
			}

		/* See if there are any pending deaths */
		if ((pProc = pDyingStateList) != nil)
			break;
		
		/* see if it's time to switch to a background task */
		if (nbacks > 0)
			{
			if ( (pProc = nextback()) != pFrontProcess)
				{
				pProc->p_state = PRBACKRUN;
				break;
				}
			else
				pProc = nil;
			}
		
		/* else, continue to run frontmost process */
		if (fgstate == PRREADY)
			{
			pProc = pFrontProcess;
			pProc->p_state = PRRUN;
			break;
			}
			
		/* Give the system/toolbox some time */
		SynchIdleTime();

#ifdef HAS_AUX_PROCESSMGR
		/* If we got here, there's nothing that wants to run, if this is A/UX we can
		 * go block in the kernel waiting on something interesting to happen, this will
		 * also break if the mouse leaves mousebox or as soon as the first timeout expires
		 * NOTE: We make sure to do this after SyncIdleTime(), since we will not get
		 * control back for a while.
		 */
		if (AUXIsPresent)
			{
		 	EventRecord		anEvent;
			
			if (AUX_WaitForEvent(-1, &anEvent))
				{
				/* If an event is waiting to be processed, wake up the frontmost layer,
				 * hopefully it's some sort of user event which needs to be processed
				 * by the front layer, otherwise it's a kernel event which can be
				 * processed by anybody.
				 */
				pProc = pFrontProcess;
				if (fgstate == PRSLEEPING)
					CancelSleep(pProc);
				pProc->p_state = PRRUN;
				break;
				}
			}
#endif HAS_AUX_PROCESSMGR
		}
		
	return(pProc);
	}

/* nextback.  Return the next available background task to run.  Uses a placeholder
 * to affect a roundrobin selection of background tasks.
 */
PEntryPtr
nextback(void)
	{
	PEntryPtr			pThisPEntry, pStartEntry;
	
	/* recover placeholder, and start the search */
	pThisPEntry = pStartEntry = pLastBGProcess;
	assert(IsProcessEntry(pThisPEntry));
	do
		{
		/* Move on. Wraparound to start of list, if necessary. */
		if ((pThisPEntry = pThisPEntry->p_NextProcess) == nil)
			pThisPEntry = pProcessList;
		
		/* check eligibility of this process */
		if (((pThisPEntry->p_taskmode & modeCanBackground) != 0)
			&& (pThisPEntry->p_state == PRREADY))
			return (pLastBGProcess = pThisPEntry);
		}
	while (pThisPEntry != pStartEntry);
	
	/* Couldn't find any so stay in foreground */
	return(pFrontProcess);
	}

/* Limit on number of event calls allowed while switched in to do updates in the
 * background (p_state == PRUPDATE).  We impose this because some apps are unable
 * to handle background updates.  Timing out is the only recourse.
 * NOTE: This timeout should really be per update.  What if there are many windows
 * to be updated?  Only the first MAX_UPDATE_CHANCES could be done, the remainder
 * would be lost (via CancelPendingUpdates).  Also, some programmer may decide to
 * do a slow update in pieces, with interspersed event calls to increase user response
 * time (e.g. to cancel the draw).  Would it make sense to patch BeginUpdate or
 * EndUpdate to reset the counter?
 */
#define MAX_UPDATE_CHANCES	(50)

/* PrepareForUpdate.  Given a random process that has an update pending, set up the
 * necessary state information to let it run.
 */
void
PrepareForUpdate(PEntryPtr pProc)
	{
	CancelSleep(pProc);
	
	/* The foreground application take updates without any special effort */
	if (pProc == pFrontProcess)
		pProc->p_state = PRRUN;
	
	/* Background applications should be scheduled just long enough to do the
	 * update(s) before we return to our normal scheduling.  Impose limit on
	 * number of update events we'll actually allow.
	 */
	else
		{
		/* NOTE: Why don't we set remnulls here? */
		pProc->p_state = PRUPDATE;
		pProc->p_updatechances = MAX_UPDATE_CHANCES;
		}
	}
