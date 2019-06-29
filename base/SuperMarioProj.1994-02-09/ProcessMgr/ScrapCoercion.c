/*
	File:		ScrapCoercion.c

	Contains:	Routines that implement scrap coercion on foreground process switches.

	Written by:	Phil Goldman and Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<17>	  4/9/92	KST		#1027029,<JH>: Inform TSM when a non-TSM aware application is
									getting suspended so TSM can pull down input method's menu.
		<16>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		<15>	 2/25/91	DFH		DC,WS#DFH-910225a: Fixed so GetScrap works even if called before
									first event call. IsScrapOwnedByCurrentProcess did not allow for
									case where MoveScrap() has not been called on the current app.
		<14>	 1/28/91	DFH		JSM,#81718:Fixed OrphanScrapIntoPartition to deal with case
									where SCRAPHANDLE is nil because scrap is on the disk.
		<12>	  1/8/91	DFH		(rdd) Changed StartForegroundSwitch to allow for case where
									current front process is suspended for high-level debugging.
		<11>	12/21/90	DFH		Fixed SetFirstDstCoercionState to set srcMyScrapInfo.pScrapOwner
									= pCurrentProcess when dst scrap is already up to date.
		<10>	12/18/90	DFH		Added GetScrap patch support routine.
		 <9>	12/18/90	DFH		Removed fgAppsUnexecuted mechanism, since front switch queue
									works better.
		 <8>	12/17/90	DFH		Fixed scrap retention devices.
		 <7>	12/15/90	DFH		Propagate correct scrap, even if some destination process
									doesn't have enough memory for it. True even if real scrap owner
									ExitToShell's.
		 <6>	12/11/90	DFH		CancelQuitAll now called only when FG switching out of intended
									victim. Other switches can be normal (e.g. when intended victim
									is a BG-only application).
		 <5>	12/10/90	DFH		Compiler bug workaround in MoveScrap.  Use SCRAPCOUNT += 1
									instead of SCRAPCOUNT++.
		 <4>	11/14/90	DFH		Change ScrapIO to use PBOpenDF instead of PBOpen.
		 <3>	 11/6/90	DFH		Use structure defs for menu bar access in CS_INIT.
		 <0>	 x/xx/86	PYG		New Today.

*/ 

#include <types.h>
#include <memory.h>
#include <desk.h>
#include <menus.h>
#include <quickdraw.h>
#include <palette.h>
#include <osutils.h>
#include <events.h>
#include <windows.h>
#include <files.h>
#include <errors.h>

#include <TSMprivate.h>
#include <TextServices.h>


#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Patches.h"
#include "Puppet.h"
#include "AppleEventExtensions.h"



#pragma segment	kernel_segment

static long		resume_message;					/* the message field of the resume event */
static short	eatMouseTimeout;				/* counter of gne tries to eat mouse-down in CS_EATMOUSE */
static short	clickDAWindTimeout;				/* counter of gne tries to force click in fake DA window */


/* Function prototypes that shoule be in (yet another) header file */
void		BringProcessToFront(PEntryPtr);

/* Function prototypes defined and used in this file */
void		SetThirdSrcCoercionState(void);
void		SetFirstDstCoercionState(void);
short		FindCutCopyItems(MenuHandle);
void		MoveScrap(void);
void		ScrapIO(short, StringPtr, Ptr, u_size);
void		ActivateTopWindow(short);
void		DisposeOrphanedScrap(void);

/* ActivateTopWindow.  Invert the hiliting the front window when the current application
 * is the  kind that does the rest of the deactivate/activate itself.
 * NOTE:  Assumes A5 = PROCESSMGRGLOBALS
 */
void
ActivateTopWindow(short activateMods)
	{
	WindowPtr			frontWindow;
	EventRecord			dummyEvent;

	assert(pCurrentProcess->p_layer != nil);
		
	if ((frontWindow = FrontWindowIn(pCurrentProcess->p_layer)) != nil)
		{
		/* Highlight the window and make sure the palette agrees. */
		HiliteWindow(frontWindow, (activateMods != 0));
		if (Colorized && ((activateMods & activeFlag) != 0))
			ActivatePalette(frontWindow);
		
		/* If it's a DA window and activate events are allowed, give the DA an activate event */
		if (((WindowPeek) frontWindow)->windowKind < 0 && SEVTENB != 0)
			{
			dummyEvent.what = activateEvt;
			dummyEvent.message = (unsigned long)frontWindow;
			dummyEvent.modifiers = activateMods;
			(void) SystemEvent(&dummyEvent);
			}
		}
	}
	
/* PutUpFakeWindow.  Puts up a window way offscreen to generate de-activate events.
 * NOTE: Have to have at least one character in name string just in case someone
 * tries to stick this name in a menu (null strings are not allowed there).
 * An app that does this:  Insight Accounts Receivable
 * Assumes A5 == PROCESSMGRGLOBALS
 */
void
PutUpFakeWindow(void)
	{
	WindowPtr		fakeWindowPtr;
	Rect			fakeWindowRect;
	short			nullski = TWO_CHARS_TO_SHORT('\01',' ');
	
	/* NOTE: Should use global here */
	SET_RECT(&fakeWindowRect,16000,16000,16004,16004);
	
	fakeWindowPtr = NewWindow(NULL, &fakeWindowRect,
		&nullski, true, (short) 0, (WindowPtr) -1, true, 0L);
		
	/* Tag it as a fake window by sticking a funny value in the windowKind field */
	((WindowPeek) fakeWindowPtr)->windowKind = SCRAP_COERCION_FAKE_DA_WINDOW_ID;
	pCurrentProcess->p_wptr = fakeWindowPtr;
	}
	
/* TakeDownFakeWindow.  Takes down the fake DA window we put up earlier.  Nothing fancy.
 * Assumes A5 == PROCESSMGRGLOBALS
 */
void
TakeDownFakeWindow(void)
	{
	register WindowPeek		fakeWindowPtr;
	register PEntryPtr		pCurrProc;
		
	/* Does the window exist?  If so, get rid of it. */
	pCurrProc = pCurrentProcess;
	if ((fakeWindowPtr = pCurrProc->p_wptr) != nil)
		{
		DisposeWindow(fakeWindowPtr);
		pCurrProc->p_wptr = nil;
		}
	}

#define SCRAP_TRANSFER_SIZE				1024
#define DO_SCRAP_READ					((short) 0)
#define DO_SCRAP_WRITE					((short) 1)

/* MoveScrap.  Moves the scrap from the source task to the destination task.  The scrap
 * of the source task is described by srcMyScrapInfo.  The scrap of the destination
 * task is described in lomem.
 */
void
MoveScrap(void)
	{
	u_size		size;
	
	/* Say we've tried at least once to give this process a scrap.  It means that the
	 * scrap copied over during LaunchApplication is no longer to be used.
	 */
	pCurrentProcess->p_scrapIsNotFromLaunch = true;

	/* Get/Set scrap size */
	size = SCRAPINFO = srcMyScrapInfo.size;
	
	/* See if the dst task scrap is in memory */
	if (SCRAPHANDLE != nil)
		{
		/* Check whether the src task scrap is in mem */
		if (srcMyScrapInfo.memHandle != nil)
			{
			if (PtrToXHand(*srcMyScrapInfo.memHandle, SCRAPHANDLE, size) != noErr)
				{
				SCRAPINFO = 0;
				SetHandleSize(SCRAPHANDLE, 0);
				}
			}

		/* The src task scrap is on disk */
		else
			{
			SetHandleSize(SCRAPHANDLE, size);
			if (MEMERROR == noErr)
				ScrapIO(DO_SCRAP_READ, srcMyScrapInfo.fileName, *SCRAPHANDLE, size);
			else
				{
				SCRAPINFO = 0;
				SetHandleSize(SCRAPHANDLE, 0);
				}
			}
		}
	
	/* The dst task scrap is on disk */
	else
		{
		/* See whether the src task scrap is in memory */
		if (srcMyScrapInfo.memHandle != nil)
			ScrapIO(DO_SCRAP_WRITE, SCRAPNAME, *srcMyScrapInfo.memHandle, size);
#ifdef DISK_TO_DISK_SCRAP
		
		/* The src task scrap is on disk.  I give up!
		 * NOTE: srcMyScrapInfo.vRefNum is never set.  ScrapCopyOverVolumes doesn't exist.
		 */
		else
			{
			short		vRefNumDst = BOOTVOL;
			
			/* Do I/O if on separate volumes, otherwise just change size */
			if (srcMyScrapInfo.vRefNum != vRefNumDst)
				{
				/* Files on different volumes */
				ScrapCopyOverVolumes(srcMyScrapInfo.vRefNum,vRefNumDst);
				}
			
			}
#endif DISK_TO_DISK_SCRAP
		}
	
	/* If it worked, indicate that the current process' scrap is good, and get rid
	 * of the orphaned scrap (its job is done!).
	 */
	if (size == SCRAPINFO)
		{
		srcMyScrapInfo.pScrapOwner = pCurrentProcess;
		DisposeOrphanedScrap();
		}
		
	/* Invalidate the scrap, and sync up local counter with global. */
	SCRAPCOUNT += 1;
	pCurrentProcess->p_cutcopycount = cutCopyCount;
	}

/* IsScrapOwnedByCurrentProcess.  Return whether we think that the current SCRAPINFO
 * is valid.  Essentially, the check is whether the most recent MoveScrap() failed
 * and no new scrap has been made.
 */
Boolean
IsScrapOwnedByCurrentProcess(void)
	{
	PEntryPtr		pCurrProc;
	
	pCurrProc = pCurrentProcess;
	return ((pCurrProc == srcMyScrapInfo.pScrapOwner) || 
			(pCurrProc->p_cutcopycount != cutCopyCount) ||
			(pCurrProc->p_scrapIsNotFromLaunch == false));
	}

/* ScrapIO.  Generic scrap file I/O based on the first parameter passed in */
void	
ScrapIO(short ioMode, StringPtr scrapFileName, Ptr scrapPtr, u_size size)
	{
	IOParam			IOParamLocal;
	
	/* Do the open */
	IOParamLocal.ioNamePtr = scrapFileName;
	IOParamLocal.ioVRefNum = BOOTVOL;	/* scrap is always on boot volume */
	IOParamLocal.ioVersNum = (char) 0;
	IOParamLocal.ioMisc = NULL;
	IOParamLocal.ioPermssn = (ioMode == DO_SCRAP_READ) ? fsRdPerm : fsWrPerm;
	(void) PBOpenDF((ParmBlkPtr) &IOParamLocal, SyncHFS);
	
	/* Do the I/O */
	IOParamLocal.ioBuffer = scrapPtr;
	IOParamLocal.ioReqCount = size;
	IOParamLocal.ioPosMode = fsFromStart;
	IOParamLocal.ioPosOffset = 0L;
	if (ioMode == DO_SCRAP_READ)
		(void) PBRead((ParmBlkPtr) &IOParamLocal, SyncHFS);
	else
		(void) PBWrite((ParmBlkPtr) &IOParamLocal, SyncHFS);
		
	/* Do the close */
	(void) PBClose((ParmBlkPtr) &IOParamLocal, SyncHFS);
	}

/* Coercion_State_Engine.  This is the engine that drives the scrap coercion that
 * brackets user-initiated switches between a source task (src) and a destination task
 * (dst).  It is procedurally driven, rather than table driven, based on the variable
 * coercionState.  The sequence of values that coercionState will have is a function of
 * the PEntry of the src and dst tasks, in specific the modeNeedSuspendResume (in
 * p_taskMode) and the p_cutcopycount field.  These two values are used to determine new
 * values of coercionState at certain key points in the scrap coercion.  In between
 * these key points new coercionState values are predetermined from the old ones.
 *
 * See the flowchart in file 'Scrap Coercion Flowchart' for a more visual explanation.
 *
 * The return value of the function is a word-sized boolean indicating whether the
 * given EventRecord has been filled in with a non-nullEvent (i.e. like an event call).
 *
 * NOTE:  Assumes A5 = PROCESSMGRGLOBALS
 */

#define APP_EVENTS_ONLY_MASK (app1Mask+app2Mask+app3Mask)

short
Coercion_State_Engine(unsigned short eventmask, EventRecord *theevent, Ptr oldtrap, Boolean pullevent)
	{
	short	retval;
	
	switch(coercionState)
		{			
		/* First state seen by the incoming process */
		case CS_ENTERING:
			{
			SetFirstDstCoercionState();
			retval = wFalse;
			break;
			}
			
		/* Outgoing app has finished coercion.  We're ready to switch */
		case CS_EXITING:
			{
			PEntryPtr		pProc;
							
			src_scrap_setup();
			coercionState = CS_ENTERING;
			pProc = pNewFrontProcess;
			pNewFrontProcess = nil;
			
			/* Make pProc frontmost, and switch to it */
			BringProcessToFront(pProc);
			
			retval = wFalse;
			break;
			}
			
		/* Generate the deactivate, activate events caused by putting up window */
		case	CS_GENERATE_DEACTIVATE1:
		case	CS_GENERATE_DEACTIVATE2:
			{
			if (pullevent)
				SetThirdSrcCoercionState();
			retval = CALL_FNC_PTR(Eventcommon_ptr,oldtrap,(eventmask & activMask, theevent));
			break;
			}

		case	CS_MOVE_SCRAP:
			{
			MoveScrap();
			
			nullTimeout = NULL_TIMEOUT_INIT;
			coercionState = ((pCurrentProcess->p_taskmode & modeNeedSuspendResume) != 0)
								? CS_RESUME : CS_GENERATE_NULLS_THEN_CMD_C;

			/* sync up next time thru */
			retval = wFalse;
			break;
			}
			
		case	CS_GENERATE_NULLS_THEN_CMD_V:
		case	CS_GENERATE_NULLS_THEN_CMD_C:
			{
#define CMD_V	(0x976)
#define CMD_C	(0x863)
			retval = wFalse;
			
			/* Generating a few null events to make apps like Microsoft File happy */
			if (--nullTimeout > NULL_TIMEOUT_CUTOFF)
				break;
			
			/* If it's OK to, convert the event to a cmd-C or cmd-V.  First off,
			 * system events aren't allowed inside GNE if SEVTENB != 0.
			 */
			if ((pullevent == false) || (SEVTENB == (char) 0 && nullTimeout > 0))
				{
				/* Must have caller that wants key-downs */
				if ((eventmask & keyDownMask) != 0)
					{
					/* Try to get outgoing app to post its scrap */
					if (coercionState == CS_GENERATE_NULLS_THEN_CMD_V)
						{
						if (pullevent)
							coercionState = CS_EXITING;
						theevent->message = CMD_V;
						}
						
					/* Try to get new app to load the scrap. */
					else
						{
						if (pullevent)
							{
							clickDAWindTimeout = CLICK_DA_WIND_TIMEOUT_INIT;
							coercionState = CS_CLICK_IN_WINDOW;
							}
	
						theevent->message = CMD_C;
						}
					
					/* force a cmd-<something>
					 * NOTE: Why do we also say the mouse button is down?
					 */
					theevent->what = keyDown;
					theevent->modifiers = cmdKey+btnState;
					retval = wTrue;
					}
				}
			
			/* We timed out waiting for SEVTENB to be clear.  Abandon the attempt. */
			else
				{
				/* See if we were in destination trying to load the scrap. */
				if (coercionState == CS_GENERATE_NULLS_THEN_CMD_C)
					{
					coercionState = CS_CLICK_IN_WINDOW;
					clickDAWindTimeout = CLICK_DA_WIND_TIMEOUT_INIT;
					}
				
				/* We were trying to get the outgoing app to post the scrap. */
				else
					coercionState = CS_EXITING;
				}

			break;
			}
		
		/* Fabricate a mouse-down in the fake DA window so we can close the window when
		 * the app calls _SystemClick (i.e. as if the user clicked the close box).
		 */
		case	CS_CLICK_IN_WINDOW:
			{
			WindowPtr 		fakeDAWindowPtr;
			
			retval = wFalse;
			
			/* Must have caller that wants mousedown.  Timeout if necessary.  */
			if ((eventmask & mDownMask) == 0)
				{
				if (--clickDAWindTimeout == 0)
					coercionState = CS_TAKE_DOWN_WINDOW;
				break;
				}
		
			/* Return the event */
			coercionState = CS_TAKE_DOWN_WINDOW;
			if ((fakeDAWindowPtr = pCurrentProcess->p_wptr) != nil)
				{
				theevent->what = mouseDown;
				PT_TO_PT(&(*(((WindowPeek) fakeDAWindowPtr)->strucRgn))->rgnBBox.top, &theevent->where);
				theevent->modifiers = 0;
				retval = wTrue;
				}
			
			break;
			}
		
		/* We just gave a mousedown in the DA window.  Either, the app called SystemClick
		 * and we closed the window, or he has made another event call and we can take
		 * down the window now.  Look for an activate event on another window.
		 * NOTE: If we DO give back an activate, we'll come through here again, since we
		 * don't change the coercionState.
		 */
		case	CS_TAKE_DOWN_WINDOW:
			{
			/* Just in case the app never calls _SystemClick */
			TakeDownFakeWindow();
			
			/* Try and force thru the activate event that should be generated since we just
			 * took down the fake window
			 */
			retval = CALL_FNC_PTR(Eventcommon_ptr,oldtrap,(eventmask & activMask, theevent));

			/* Either there was no activate to get (e.g. no windows left) or we got it.
			 * In both cases forge ahead.
			 * NOTE: The 32-bit dirty check of the CURACTIVATE high bit checks whether the
			 * window update was canceled (i.e. <= 0 means no window update in progress).
			 */
			if ((long)CURACTIVATE <= 0)
				{
				eatMouseTimeout = EAT_MOUSE_TIMEOUT;
				coercionState = CS_EATMOUSE;
				}
			
			break;
			}
			
		case	CS_PRESUSPEND_DEACTIVATE:
			/* Deactivate the top window and return a suspend event.  This also
			 * results in a deactivate event later on.
			 */
			ActivateTopWindow(0);
				
			/* and fall thru */

		case	CS_SUSPEND:
			{
			retval = wFalse;
			
			/* fabricate a suspend event */
			if ((eventmask & app4Mask) != 0)
				{
				theevent->what = app4Evt;
				theevent->message = SUSPEND_MESSAGE;
				retval = wTrue;
				}

			/* actually switch next time */
			if (pullevent)
				coercionState = CS_EXITING;
			
			break;
			}
		
		case	CS_RESUME:
			{
			/* Make transition to next state */
			if (pullevent)
				{
				if ((pCurrentProcess->p_taskmode & modeDoesActivateOnFGSwitch) != 0)
					{
					/* treat resume as a activate evt too */
					ActivateTopWindow(activeFlag);
	
					eatMouseTimeout = EAT_MOUSE_TIMEOUT;
					coercionState = CS_EATMOUSE;
					}
				else
					coercionState = CS_TAKE_DOWN_WINDOW;
				}
				
			/* Generate a resume event for the app.  Added bonus: Throw in scrap
			 * coercion flags and puppet string results into the .message field.
			 */
			retval = wFalse;
			if ((eventmask & app4Mask) != 0)
				{
				theevent->what = app4Evt;
				theevent->message = resume_message;
				
				retval = wTrue;
				}
				
			break;
			}

		case	CS_EATMOUSE:
			{
			/* About to leave scrap coercion so check if a mouse event
			 * should be flushed.  Flush if mouse down in content rgn,
			 * close box, or zoom box of front window.  This is most
			 * likely the mouse click that caused the FG switch so long ago,
			 * but not necessarily (since the FG switch might be programmatic
			 * or via Apple menu process list item).
			 * NOTE: Technically, w->hilited check should be w == FrontWindow(),
			 * but since many apps (e.g. SuperPaint) have tool windows, and
			 * highlight a menu farther along in the list, we do this hack.
			 * Question:  What does this do in PageMaker, which has 2
			 * active windows?
			 */
			
			short			windowPart;
			WindowPtr		pWindow;
			Boolean			allDone = false;
			
			/* If there is a mousedown, process it */
			if (retval = CALL_FNC_PTR(Eventcommon_ptr, oldtrap,(eventmask & mDownMask, theevent)))
				{
				windowPart = FindWindow(theevent->where, &pWindow);
				if ((pWindow != nil)
					&& (windowPart != inDrag)
					&& (windowPart != inDesk)
					&& (((WindowPeek) pWindow)->hilited)
					&& (((windowPart == inGoAway) || (windowPart == inZoomIn) || (windowPart == inZoomOut))
						|| (pCurrentProcess->p_taskmode & modeGetFrontClicks) == 0) )
						{
						if (pullevent == false)
							FlushEvents(mDownMask, mUpMask);
						FlushEvents(mUpMask, everyEvent-mUpMask);
						retval = wFalse;
						}
						
				/* Indicate that this click brought the app to the front */
				else
					theevent->modifiers |= activeFlag;
				
				/* That's all folks! */
				allDone = pullevent;		
				}

			/* We're done if this was GetNextEvent, or we timed out with EventAvails */
			else
				allDone = (pullevent || ((eventmask & mDownMask) != 0) || (--eatMouseTimeout == 0));
			
			/* High-level switch is complete.  If we've switched into a scratchpad task,
			 * now is a good time to set its state to receive the puppet strings.
			 */
			if (allDone)
				{
				/* Let process see real events for a change */
				coercionState = CS_DONE;
				
				/* Well… maybe a few more fake ones! */
				if (pCurrentProcess->p_puppetvars.puppetMasterProc != nil)
					pCurrentProcess->p_state = PRPUPPET;
				
				/* Are we in the middle of a system QuitAll? */
				if (pSystemQuitAllMsg != nil)
					{
					PEntryPtr	pTargetProc;
					
					/* Can continue the quitall if we are switching back to the
					 * originator after a victim has died.  Allow initial switch to
					 * intended victim.
					 */
					pTargetProc = PEntryFromPSN(&(pSystemQuitAllMsg->targetmfid.localPSN));
					if ( (pTargetProc == nil) && EqualPSN(&(pSystemQuitAllMsg->sendermfid.localPSN), &pCurrentProcess->p_serialNumber) )
						ContinueQuitAll();
					}
				
				}
			break;
			}
					
		/* Initial coercion state of a process.  This code should only be reached the
		 * very first time an app makes an event call in the foreground.
		 */
		case	CS_INIT:
			{
			register MenuPair 		*pThisMenuPair;
			register Ptr			limitPtr;
			MenuHandle				menuHdl;
			

			/* Check if process can do cut or copies, but only if there is a menu list. */
			pCurrentProcess->p_haseditcmdkeys = 0;
			if ((MENULIST != nil) && (*MENULIST != nil))
				{
				/* Check likely edit menu first.  Scan remainder only on no-find. */
				if ((menuHdl = GetMHandle(3)) && FindCutCopyItems(menuHdl))
					pCurrentProcess->p_haseditcmdkeys = 1;
				else
					{
					pThisMenuPair = &((MenuListPtr) (*MENULIST))->menuPairs[0];
					limitPtr = *MENULIST + ((MenuListPtr) (*MENULIST))->lastMenu;
					while (pThisMenuPair <= limitPtr)
						{
						/* Check this menu */
						if (FindCutCopyItems(pThisMenuPair->menuHdl))
								{
								pCurrentProcess->p_haseditcmdkeys = 1;
								break;
								}
						
						/* Look at next menu */
						pThisMenuPair += 1;
						}
					}
				}
			
			/* Set up process with the current scrap. */
			MoveScrap();

			/* Put process in state to receive scratchpad events if the lad has
			 * already acquired a master.  Poor guy.
			 */
			coercionState = CS_DONE;
			if (pCurrentProcess->p_puppetvars.puppetMasterProc != nil)
				pCurrentProcess->p_state = PRPUPPET;
			retval = CALL_FNC_PTR(Eventcommon_ptr,oldtrap,(eventmask, theevent));
			break;
			}
		}
	
	return(retval);
	}

#pragma segment Main

/* AddToFrontProcessQueue.  Puts pProc at the end of the waiting list for being switched
 * to the foreground.  Uses a pretty crude queue, on the assumption that we're not going
 * to have a very long list.
 * NOTE: This could be made more VM friendly, by a) skipping RemoveFromFrontProcessQueue
 * if we could tell that pProc wasn't in the queue, and b) using a tail pointer.
 */
void
AddToFrontProcessQueue(PEntryPtr pProc)
	{
	PEntryPtr	pCurr, pPrev;
	
	/* Remove pProc if it is already queued */
	RemoveFromFrontProcessQueue(pProc);
	
	/* Scan the list to find the end */
	pPrev = nil;
	pCurr = pFrontProcessQueue;
	while (pCurr != nil)
		{
		pPrev = pCurr;
		pCurr = pCurr->p_nextQueuedFrontProcess;
		}
	
	/* Now, append to the list */
	pProc->p_nextQueuedFrontProcess = nil;
	if (pPrev == nil)
		pFrontProcessQueue = pProc;
	else
		pPrev->p_nextQueuedFrontProcess = pProc;
	}

/* PopFrontProcessQueue.  Extract and return the first process to bring to the front. */
PEntryPtr
PopFrontProcessQueue(void)
	{
	PEntryPtr	pProc;
	
	/* Look at the queue */
	if ((pProc = pFrontProcessQueue) != nil)
		{
		pFrontProcessQueue = pProc->p_nextQueuedFrontProcess;
		pProc->p_nextQueuedFrontProcess = nil;
		}
		
	return(pProc);
	}

/* RemoveFromFrontProcessQueue. "Cancels" pending foreground switch to pProc. */
void
RemoveFromFrontProcessQueue(PEntryPtr pProc)
	{
	PEntryPtr	pCurr, pPrev;
	
	pPrev = nil;
	pCurr = pFrontProcessQueue;
	while (pCurr != nil)
		{
		if (pCurr == pProc)
			{
			PEntryPtr	pNext;
			
			pNext = pProc->p_nextQueuedFrontProcess;
			if (pPrev == nil)
				pFrontProcessQueue = pNext;
			else
				pPrev->p_nextQueuedFrontProcess = pNext;
			pProc->p_nextQueuedFrontProcess = nil;
			break;
			}
		
		pPrev = pCurr;
		pCurr = pCurr->p_nextQueuedFrontProcess;
		}
	}

/* OrphanScrapIntoPartition.  This is a great one!  When the APPLZONE is about to be
 * nuked, we make sure that the SCRAPHANDLE is not taken with it, if that scrap is one
 * that we'll want to pass on to other processes (i.e. realScrapOwner is true).  We save
 * it off by copying it to the beginning of the application partition, then resizing the
 * partition to the correct length.  This avoids excessive stack or heap use.  We
 * indicate that SCRAPHANDLE is an orphan, so that it gets disposed when the copying
 * (MoveScrap()) is complete.
 * If realScrapOwner is false, we make sure the scrap gets disposed even if it is not
 * inside the partition that is going away.
 * NOTE: Requires valid APPLZONE, but then invalidates it!  Yea!
 */
void
OrphanScrapIntoPartition(void)
	{
	register Handle		scrapHdl, procHdl;
	unsigned long		scrapSize;
	Boolean				scrapInZone;
	
	/* Nothing to do if scrap is purged.
	 * NOTE: This has side-effect that a purged scrap handle from heap other than
	 * APPLZONE or one of its subzones will not get disposed.  Too risky to try, tho!
	 */
	if ((scrapHdl = SCRAPHANDLE) != nil)
		if (*scrapHdl == nil)
			return;
	
	/* Remember whether handle is from APPLZONE.
	 * NOTE: Will be “false” for nil handle, which is what we want.
	 */
	scrapInZone = ( (scrapHdl >= APPLZONE) && (scrapHdl < APPLZONE->bkLim) );

	/* Don't save/copy the scrap if dying process doesn't have the scrap of interest.
	 * Dispose the scrap if it is not going to be implicitly disposed when the
	 * partition is.  E.g. The DA Handler's scrap is in the system heap.
	 */
	if ((pCurrentProcess != srcMyScrapInfo.pScrapOwner) && (pCurrentProcess->p_cutcopycount == cutCopyCount))
		{
		if ((scrapInZone == false) && (scrapHdl != nil))
			DisposHandle(scrapHdl);
		return;
		}
		
	/* Copy scrap to beginning of partition, then size down the partition handle.
	 * We nil out APPLZONE since we are putting it out of commission.
	 * NOTE: We know that scrapSize < GetHandleSize(procHdl) since the SCRAPHANDLE
	 * is fully contained by a heap within the partition.
	 */
	if (scrapInZone)
		{
		procHdl = pCurrentProcess->p_zoneh;
		scrapSize = GetHandleSize(scrapHdl);
		APPLZONE = nil;
		BlockMove(*scrapHdl, *procHdl, scrapSize);
		SetHandleSize(procHdl, scrapSize);
	
		/* Now replace the lomem, and forget that handle was the partition */
		pCurrentProcess->p_zoneh = nil;
		HUnlock(procHdl);
		SCRAPHANDLE = procHdl;
		}
	
	/* Set up coercion info, and remember to chuck the scrap when we're done.  This needs to
	 * be done even if scrapHdl is nil, since that just means the scrap is on disk.
	 */
	src_scrap_setup();
	srcMyScrapInfo.pScrapOwner = nil;
	srcMyScrapInfo.orphanedScrap = true;	
	}

/* DisposeOrphanedScrap.  Dispose the source scrap if it was meant to be.  Zero
 * memHandle field for safety.
 * NOTE: The orphaned scrap exists until a new scrap is made.  A new scrap is made
 * (or at least committed to) when a) an application calls ZeroScrap, PutScrap,
 * SysEdit (cut or copy), or MenuSelect (cut or copy), or b) we call MoveScrap()
 * toward the end of a foreground switch.  At these times, DisposeOrphanedScrap is
 * called because we'll no longer need the orphan.  In (a) cases, the orphan was
 * never needed (oh well).  In (b), the orphan was used to give the new front app
 * a scrap, so the orphan's job is complete.  Give 'im an extra lump of coal!
 */
void
DisposeOrphanedScrap(void)
	{
	if (srcMyScrapInfo.orphanedScrap)
		{
		srcMyScrapInfo.orphanedScrap = false;
		if (srcMyScrapInfo.memHandle != nil)
			{
			assert(*(srcMyScrapInfo.memHandle) != 0);
			DisposHandle(srcMyScrapInfo.memHandle);
			srcMyScrapInfo.memHandle = nil;
			}
		}
	}

/* src_scrap_setup.  Save the outgoing app's scrap state information.  It will be
 * used later when MoveScrap is called in the context of the new foreground
 * application.
 */
void
src_scrap_setup(void)
	{
	assert (pCurrentProcess != nil);
	
	/* Don't change scrap info if the current process has an out-of-date scrap.
	 * Decrement the p_cutcopycount so we'll try again to copy the scrap when
	 * twitching this process back in.
	 */
	if ((pCurrentProcess != srcMyScrapInfo.pScrapOwner) && (pCurrentProcess->p_cutcopycount == cutCopyCount))
		{
		pCurrentProcess->p_cutcopycount--;
		return;
		}
	
	/* Dump orphan, if he's around */
	DisposeOrphanedScrap();
	
	/* Set up the src scrap record */
	srcMyScrapInfo.size = SCRAPINFO;

	/* if SCRAPHANDLE == nil, src scrap is on disk */
	if ((srcMyScrapInfo.memHandle = SCRAPHANDLE) == nil)
		srcMyScrapInfo.fileName = SCRAPNAME;
	
	/* Assume this scrap is attached still. Caller can change this, if he wants. */
	srcMyScrapInfo.orphanedScrap = false;
	
	/* Claim scrap and synch the cutcopycount since global was just taken from the app */
	srcMyScrapInfo.pScrapOwner = pCurrentProcess;
	pCurrentProcess->p_cutcopycount = cutCopyCount;
	}

/* StartForegroundSwitch.  Initiate foreground switch. */
void
StartForegroundSwitch(PEntryPtr pProc)
	{
	register PEntryPtr	pFront, pTargetProc;
	
	assert(coercionState == CS_DONE);
	pNewFrontProcess = pProc;
	pFront = pFrontProcess;

	/* Are we in the middle of a system QuitAll?  Cancel it if we're switching out of
	 * the currently intended victim.
	 * NOTE: This assumes that StartForegroundSwitch is not called when switching to
	 * a new front process after the current front process has ExitToShell'ed.  Otherwise,
	 * we would be canceling the aeQuitAll just because we got someone to quit!  Pretty
	 * safe assumption, though, since StartForegroundSwitch deals with pCurrentProcess,
	 * which is completely invalid after ExitToShell.
	 */
	if ( (pSystemQuitAllMsg != nil)
		&& (pFront == (pTargetProc = PEntryFromPSN(&(pSystemQuitAllMsg->targetmfid.localPSN)))) )
		CancelQuitAll(&pTargetProc->p_serialNumber);
	
	/* Check whether FG switching out of a process that has been suspended for
	 * high-level debugging.  If so, we must forego the source's half of the state
	 * machine.  We go straight to the new FG process.
	 */
	if (pFront->p_state == PRNULL)
		{
		coercionState = CS_ENTERING;
		BringProcessToFront(pProc);
		
		// Inform TSM that a non-TSM aware app got swapped to the front ...
		//if ( !pFront->p_inlineAware &&  (tsmLayerOwner != nil))
		//	InformTSM(kMsgResumeApp, nil);
		return;
		}
	
	/* Start the ball rolling… */
	if ((pFront->p_taskmode & (modeNeedSuspendResume | modeDoesActivateOnFGSwitch))
		== (modeNeedSuspendResume | modeDoesActivateOnFGSwitch))
		{
		coercionState = CS_PRESUSPEND_DEACTIVATE;
		}
	else
		{
		coercionState = (WINDOWLIST != nil) ? CS_GENERATE_DEACTIVATE1 : CS_GENERATE_DEACTIVATE2;
		PutUpFakeWindow();
		}
	

	// Inform TSM that a non-TSM aware app got suspended ...
	
	if ( !pFront->p_inlineAware &&  (tsmLayerOwner != nil))
			InformTSM(kMsgSuspendApp, nil);
	}
	
/* SetThirdSrcCoercionState.  Decide where to go after CS_GENERATE_DEACTIVATE1
 * or CS_GENERATE_DEACTIVATE2.  Being in CS_GENERATE_DEACTIVATE2 means that
 * we're expecting to see an activate event for the fake DA window.  It's time
 * to put the outgoing app on ice.
 */
void
SetThirdSrcCoercionState(void)
 	{
	/* See if we're expecting deactivate-then-activate (i.e. whether the fake window
	 * was not the only window in the WINDOWLIST).
	 */
	if (coercionState == CS_GENERATE_DEACTIVATE1)
		{
		coercionState = CS_GENERATE_DEACTIVATE2;
		return;
		}
	
	/* If we can't see cut/copies, assume it happened */
	if (pCurrentProcess->p_haseditcmdkeys == false)
		cutCopyCount++;
	
	/* Set the next state.  This will determine how the outgoing app will be told/forced
	 * to post its scrap and get switched out.
	 */
	 
	/* Try to use modern suspend event that tells app to do it all */
	if ((pCurrentProcess->p_taskmode & modeNeedSuspendResume) != 0)
		coercionState = CS_SUSPEND;
	
	/* We have dumb app that doesn't understand suspend events. */
	else
		{
		/* Force scrap coercion if necessary */
		if (pCurrentProcess->p_cutcopycount != cutCopyCount)
			{
			nullTimeout = NULL_TIMEOUT_INIT;
			coercionState = CS_GENERATE_NULLS_THEN_CMD_V;
			}
		
		/* Otherwise, start the move to the new app. */
		else
			coercionState = CS_EXITING;
		}
	}

/* SetFirstDstCoercionState.  Start the the track that the dst task will take through
 * coercion state space.  Called only by engine in CS_ENTERING state, this can be
 * pulled back inline for optimization.
 */
void
SetFirstDstCoercionState(void)
 	{
	PEntryPtr		pCurrProc = pCurrentProcess;
	
	if (pCurrProc->p_slices == 1)
		{
		coercionState = CS_INIT;
		return;
		}
		
	/* NOTE:  Have to set bit 1 dynamically */
	resume_message = RESUME_MESSAGE;
	
	/* Check whether returning from scratchpad switch */
	if (pCurrProc->p_puppetvars.masterPuppetProc != nil)
		{
		resume_message |= (1 << bitResumeScratch);
		*((char *) &resume_message + 1) = pCurrProc->p_puppetvars.masterResumeMsg;
		pCurrProc->p_puppetvars.masterResumeMsg = resumeFlag;
		pCurrProc->p_puppetvars.masterPuppetProc = nil;
		}

	/* Check whether we need to copy clipboard. If so, the process needs also to
	 * coerce the clipboard into its scrap when it gets its resume event.
	 */
	if (pCurrProc->p_cutcopycount != cutCopyCount)
		{
		resume_message |= convertClipboardFlag;
		coercionState = CS_MOVE_SCRAP;
		}
	
	/* No clipboard copying needed.  Take the easy way out. */
	else
		{
		/* Pass the honor of scrap ownership to the new front process */
		srcMyScrapInfo.pScrapOwner = pCurrentProcess;
		
		/* Remove orphaned scrap, if that's the case */
		DisposeOrphanedScrap();
		
		/* Shortcut to resume event for the bright, young apps */
		if ((pCurrProc->p_taskmode & modeNeedSuspendResume) != 0)
			coercionState = CS_RESUME;
		else
			{
			coercionState = CS_CLICK_IN_WINDOW;
			clickDAWindTimeout = CLICK_DA_WIND_TIMEOUT_INIT;
			}
		}
	}
	
/* CheckForCutCopySysEditCmd.  Given the edit command passed to _SysEdit, determine whether
 * it is a "cut" or "copy" type of command.  If it is, increment the "cutcopycount"
 * addressed by pCutCopyCount.  If said count is the global cutCopyCount, call
 * DisposeOrphanScrap to free the memory now that we won't need the orphan.
 */
void
CheckForCutCopySysEditCmd(short theEditCmd, short *pCutCopyCount)
	{
	/* Edit command passed to _SysEdit has not yet been biased by accUndo */ 
	if ( (theEditCmd != (accCut-accUndo)) && (theEditCmd != (accCopy-accUndo)) )
		return;
		
	(*pCutCopyCount)++;
	if (pCutCopyCount == &cutCopyCount)
		DisposeOrphanedScrap();
	}
	
/* CheckForCutCopyMenuKey.  Given the character that is the key equivalent for a menu
 * item, determine whether the item is a "cut" or "copy" type of command.
 * If it is, increment the "cutcopycount" addressed by pCutCopyCount.  If said count is
 * the global cutCopyCount, call DisposeOrphanScrap to free the memory now that we won't
 * need the orphan.
 */
void
CheckForCutCopyMenuKey(unsigned char ch, short *pCutCopyCount)
	{	
	/* Is it a cut or copy char? */
	switch(ch)
		{
		case (unsigned char)'X':
		case (unsigned char)'x':
		case (unsigned char)'≈':		/* Option-x */
		case (unsigned char)'C':
		case (unsigned char)'c':
		case (unsigned char)'ç':		/* Option-c */
		case (unsigned char)'Z':
		case (unsigned char)'z':
		case (unsigned char)'Ω':		/* Option-z */
			{
			(*pCutCopyCount)++;
			if (pCutCopyCount == &cutCopyCount)
				DisposeOrphanedScrap();
			}
		}
	}

/* FindCutCopyItems.  Check the menu with Handle menuHdl to see if it has any cut
 * or copy items that we can find.  Return result of search.  This does nothing to
 * relocate menuHdl so it needn't be locked.
 * NOTE: Uses speedy but structure-dependant search through menu data.
 */
short	
FindCutCopyItems(MenuHandle menuHdl)
	{
	MenuPtr			menuPtr;
	register Ptr	chPtr;
	char			nameLen;
	
	/* Assume menu is useless if no menu info available */
	if (menuHdl == nil || (menuPtr = *menuHdl) == nil)
		return(wFalse);
	
	/* Jump over menu title */
	chPtr = &(menuPtr->menuData);
	nameLen = *chPtr++;
	chPtr += nameLen;
	if (*chPtr == (char) 0)
		return(wFalse);
		
	for (;;)
		{
		nameLen = *chPtr++;			/* get length of item name */
		if (nameLen == (char) 0)
			return(wFalse);			/* ran off end of list */
		chPtr += nameLen + 4;		/* skip over name and flags */
		
		/* Is it a cut or copy char? */
		switch(chPtr[-3])
			{
			case 'X':
			case 'x':
			case 'C':
			case 'c':
				return(wTrue);
			}
		}
	}
