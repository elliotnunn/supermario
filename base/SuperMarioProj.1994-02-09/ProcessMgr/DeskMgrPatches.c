/*
	File:		DeskMgrPatches.c

	Contains:	Routines that patch the Desk Manager

	Written by:	Erich Ringewald

	Copyright:	© 1986-1991, 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
	  <SM21>	 7/21/93	joe		Backed out <SM20>.
		<19>	11/17/91	YK		Modify code for the Text Services Manager in c_systemevent to
									support apps which set SEvtEnb flag off.
		<18>	 11/1/91	DTY		Use the system override map in FILE_NOT_IN_SYSTEM_MAP instead of
									SYSMAPHANDLE.
		<17>	10/15/91	YK		Added code for the Text Services Manager in c_systemevent.
		<16>	  3/7/91	DFH		stb, #84365: Fixed CoreLaunchDeskAccessory to deal with file
									spec ptr == nil and driver name ptr == nil. Was calling
									OpenDeskAcc ROM with nil name pointer.
		<15>	  3/7/91	DFH		stb, #84195: Tuned sleep limiters to have a floor driver delay.
									Greedy drivers were zeroing out the sleep period, causing the
									front application to hog the CPU from the other applications.
		<14>	  3/5/91	DFH		dnf, WS#DFH-910305a: Fixed LaunchDeskAccessory so that it can be
									called when the DA itself is the current process.
		<13>	 1/28/91	DFH		JSM,#81425: Removed inhibit_DAs mechanism that used to prevent
									SystemEvent and SystemTask call-throughs while in MenuSelect.
									There was no need for it, and it caused problems for drivers
									during prolonged SystemMenu (e.g. Help menu's ModalDialog).
		<12>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
		<10>	  1/8/91	DFH		(stb) Fixed LimitFrontSleepForDriversSake and
									LimitBackSleepForDriversSake to work correctly when dCtlDelay is
									0.
		 <9>	  1/8/91	DFH		(ewa) Fixed CoreLaunchDeskAccessory() bug where retval was not
									being set.
		 <8>	12/18/90	DFH		Changed SystemEvent patch to give high-level debugger key even
									if that debugger is pCurrentProcess.
		 <7>	12/17/90	DFH		Rename CheckForCutCopy to CheckForCutCopyMenuKey.
		 <6>	12/14/90	DFH		Added SynchIdleTime call in c_systemtask.
		 <5>	12/10/90	DFH		Compiler bug workaround in c_sysedit.  Use SCRAPCOUNT += 1
									instead of SCRAPCOUNT++.
		 <4>	 12/5/90	DFH		Driver scheduling now affects application sleep time, not vice
									versa.
		 <3>	11/30/90	DFH		Fixed PEntryFromFileSpec call in CoreLaunchDeskAccessory to pass
									pDASpec->daFileSpec instead of &fileSpec.  This bug prevented
									LaunchDeskAccessory from bringing forward an already open DA.
		 <0>	  9/2/86	ELR		New Today.

*/ 
  
#include <types.h>
#include <errors.h>
#include <memory.h>
#include <osutils.h>
#include <toolutils.h>
#include <quickdraw.h>
#include <fonts.h>
#include <events.h>
#include <desk.h>
#include <resources.h>
#include <retrace.h>
#include <menus.h>
#include <windows.h>
#include <dialogs.h>
#include <files.h>
#include <segload.h>
#include <devices.h>
#include <sound.h>
#include <MFPrivate.h>
#include <MenuMgrPriv.h>
#include <ResourceMgrPriv.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Patches.h"
#include "Puppet.h"
#include "OSDispatchPrivate.h"

/* Some function prototypes that should be in (yet another) header file */
Boolean			OptionKeyIsDown(void);
Handle			GetMapHdlFromRefNum(short, Handle);
pascal OSErr	CoreLaunchDeskAccessory(DASpecBlock *, short *, Boolean);
AppleItemHdl	StringToAppleItemHdl(StringPtr);
void			SendAppleMenuSelectMsg(AppleItemHdl);
PEntryPtr		PEntryFromApplicationMenuItem(unsigned short);
void			AdjustApplicationMenu(PEntryPtr);
void			GiveDriversTime(void);
OSErr			GetDrvrResourceAndInfo(StringPtr, Handle *, short *, Str255 *);
Boolean			imLayerKeyDown(EventRecord *, Boolean);

/* Function prototypes internal to this file */
void			GiveLocalDriversTime(void);

/* The macro below is used because Suitcase sticks maps below the system */
#define FILE_NOT_IN_SYSTEM_MAP(refNum) (GetMapHdlFromRefNum(refNum, GetOverrideMap(SYSMAPHANDLE)) == nil)

/* Used as floor value when adjusting application sleep time based on driver needs.
 * Smaller values cause the application to sleep less, which robs time from the
 * other applications.  Larger values may impinge on real driver requirements.
 * NOTE: The initial value of this (7 Mar 91) is just an educated guess.
 */
#define MIN_DRIVER_DELAY		(5)

/* GetDrvrResourceAndInfo.  Utility to get information about DA in current chain.
 * Assumes a5 == PROCESSMGRGLOBALS
 */
OSErr
GetDrvrResourceAndInfo(StringPtr pDAName, Handle *phRsrc, short *theID, Str255 *theName)
	{
	register Handle		drvrHdl;
	
	/* Locate the DRVR resource */
	drvrHdl = (pDAName != nil)
		? CALL_FNC_PTR(Getnamedresource_ptr,patchtraps[GETNAMEDRESOURCE].oldtrap,('DRVR', pDAName))
		: Get1IndResource('DRVR', 1);
		
	/* Determine the file and resource refnums */
	if (drvrHdl != nil)
		{	
		ROMMAPINSERT = (-1);
		GetResInfo(drvrHdl, theID, nil, theName);
		}
	
	*phRsrc = drvrHdl;
	return(RESERR);
	}

/* CheckUnitTableEntry.  See if the UNITTABLE entry that this driver would occupy
 * is acceptable.  For now, just checks whether entry is already in use.
 */
OSErr
CheckUnitTableEntry(short resourceID)
	{
	register DCtlPtr	dCtlPtr;
	DCtlHandle			dCtlHdl;
	OSErr				result;
	
	result = noErr;
	dCtlHdl = (DCtlHandle) UNITTABLE[resourceID];
	if ( (dCtlHdl != nil) && ((dCtlPtr = *dCtlHdl) != nil) && ((dCtlPtr->dCtlFlags & DOpened) != 0) )
		result = portInUse;
		
	return(result);
	}

pascal OSErr
CoreLaunchDeskAccessory(DASpecBlock *daSpecAddr, short *pDrvrNum, Boolean fromOpenDeskAcc)
	{
	register DASpecBlock	*pDASpec;
	register StringPtr		pDAName;
	register OSErr			retval;
	short					daFile;
	short					daResourceNum;
	Handle					drvrHdl;
	Boolean					alreadyLaunched;
	unsigned long			olda5;
	FSSpec					fileSpec;
	FCBPBRec				fcbInfo;
	LaunchResults			dahLaunchResults;
	Str255					rsrcName;
	pascal void				(*oldtrap)();
#define daOwnerPtr			(dahLaunchResults.pNewProcess)
	
	retval = noErr;
	olda5 = ProcessMgrA5Setup();
 	daOwnerPtr = nil;
	pDASpec = daSpecAddr;
	pDAName = pDASpec->daDrvrName;
	daResourceNum = 0;
	
	/* daFileSpec == nil means that the resource MUST be in the existing resource chain.
	 * Locate the resource to determine the full file specification.  Allow user to
	 * open the DA in the current layer if the option key is down.
	 */
	if (pDASpec->daFileSpec == nil)
		{		
		/* Locate the DRVR resource */
		if ((retval = GetDrvrResourceAndInfo(pDAName, &drvrHdl, &daResourceNum, &rsrcName)) != noErr)
			goto ErrorRecovery;
	
		/* Get file spec of resource file */
		if ((daFile = HomeResFile(drvrHdl)) == 0)
			daFile = SYSMAP;
		fcbInfo.ioFCBIndx = 0;
		fcbInfo.ioRefNum = daFile;
		fcbInfo.ioNamePtr = &fileSpec.name;
		fcbInfo.ioCompletion = nil;
		if ((retval = PBGetFCBInfo(&fcbInfo, SyncHFS)) != noErr)
			goto ErrorRecovery;
		
		fileSpec.vRefNum = fcbInfo.ioFCBVRefNum;
		fileSpec.parID = fcbInfo.ioFCBParID;		
		pDASpec->daFileSpec = &fileSpec;
	
		/* See whether current process should get it */
		if ( (FILE_NOT_IN_SYSTEM_MAP(daFile) || OptionKeyIsDown() ) &&
			((pCurrentProcess->p_taskmode & modeDeskAccessory) == 0) )
			{
			if (pCurrentProcess->p_layer == nil)
				{
				retval = appIsDaemon;
				goto ErrorRecovery;
				}
			else
				daOwnerPtr = pCurrentProcess;
			}
		}

	/* See if handler already launched.  Must do this with a possibly nil pDAName,
	 * since we can't look in the file to find the name if the file is busy!
	 */
	if (daOwnerPtr == nil)
		daOwnerPtr = PEntryFromFileSpec(pDASpec->daFileSpec, pDAName);
		
	if ((alreadyLaunched = (daOwnerPtr != nil)) == false)
		{			
		/* Try to launch the DA Handler (implicitly setting daOwnerPtr) */
		StandardLaunch(nil, 0, modeDeskAccessory+FILE_FLAG_MULTILAUNCH, nil, &dahLaunchResults, pDASpec);
		if ((retval = dahLaunchResults.LaunchError) != noErr)
			goto ErrorRecovery;
		
		/* Nuke dad if opening the DA was just a knee-jerk reaction by the
		 * application, since the relationship is accidental.
		 */
		if (fromOpenDeskAcc)
			daOwnerPtr->p_dadProcess = nil;
		}

	/* If DA belongs to someone else: bring it forward.  If it's brand new, make it sleep
	 * until it can come forward (avoids flash).
	 * NOTE: Assumes that DA is not using this call to bring itself forward (since we'll just
	 * _OpenDeskAcc).  OK, tho.  DA could use SetFrontProcess instead.
	 */
	if (daOwnerPtr != pCurrentProcess)
		{
		if ((SetFrontProcess(&daOwnerPtr->p_serialNumber) == noErr) && (alreadyLaunched == false))
			PutOnSleepQueue(daOwnerPtr, MAXVALUE);
		}
	
	/* Owner is current process: load the DA!
	 * NOTE: Must set the p_daResource before call through, in case the call through
	 * results in a context switch (I've seen DAs put up StandardFile here!).
	 */
	else
		{
		if ((coercionState == CS_DONE) || (coercionState == CS_ENTERING))
			{
			short	callThroughResult;
			
			if (((pCurrentProcess->p_taskmode & modeDeskAccessory) != 0) && (daResourceNum != 0))
				(*daOwnerPtr->p_pcb)->p_daResource = daResourceNum;
			oldtrap = patchtraps[OPENDESKACC].oldtrap;
			callThroughResult = CALL_FNC_PTR(Opendeskacc_ptr,oldtrap,((pDAName != nil) ? pDAName : &rsrcName));
			if (pDrvrNum != nil)
				*pDrvrNum = callThroughResult;
			}
		else
			{
			dbmsg("Bad time to call LaunchDeskAccessory/_OpenDeskAcc");
			}
		}

ErrorRecovery:
	A5Restore(olda5);
	return (retval);
#undef daOwnerPtr	
	}

/* c_opendeskacc.  Our patch to OpenDeskAcc.  This trap used to mean "open
 * desk accessory".  Now it means "I don't recognize the Apple menu item the
 * user selected, what do you make of it?".  What we make of it is one of
 * about a half dozen things that we stick in the Apple menu: a phantom item to
 * aid scrap coercion, a process name to switch to, a command, our about box,
 * a speciality item added by an application, or (last but not least) a DA.
 */
pascal short
c_opendeskacc(StringPtr stringPtrDAName)
	{
	short				retval;
	unsigned long		olda5;
	AppleItemHdl		hAppleItem;
	
	olda5 = ProcessMgrA5Setup();
	
	/* In the middle of scrap coercion?  Handle this state from here.  Install a fake
	 * DA window.  Set coercionState so app does the proper activation (i.e. if there
	 * is already a window, expect a deactivate on it before an activate on ours).
	 */
	retval = FAKE_OPENDA_RETVAL;

	if ((hAppleItem = StringToAppleItemHdl(stringPtrDAName)) != nil)
		{
		SendAppleMenuSelectMsg(hAppleItem);
		}
	
	/* Item is nothing we know as special.  Try to open as a desk accessory. */
	else
		{
		DASpecBlock		daSpec;
		
		daSpec.daFileSpec = nil;
		daSpec.daDrvrName = stringPtrDAName;
		if (CoreLaunchDeskAccessory(&daSpec, &retval, true) != noErr)
			SysBeep(20);
		}
		
	A5Restore(olda5);
	return retval;
	}

/* c_LaunchDeskAccessory.  Cause the specified DA to be launched and/or frontmost.
 * If the file name is not specified, assume that DA is in the current chain.  If
 * the file name is specified, but the resource name is not, use the first DRVR
 * resource in the file.
 */
pascal OSErr
c_LaunchDeskAccessory(FSSpec *pFileSpec, StringPtr pDAName)
	{
	DASpecBlock		daSpec;
	
	daSpec.daFileSpec = pFileSpec;
	daSpec.daDrvrName = pDAName;
	return(CoreLaunchDeskAccessory(&daSpec, nil, false));
	}

/* c_sysedit.  Patch to increment SCRAPCOUNT if the fake window is on top. */
pascal short
c_sysedit(short editCmd)
	{
	unsigned long 	olda5;
	WindowPeek		windowPeek;
	pascal void		(*oldtrap)();
	short			retval;
	
	/* Locate top window */
	windowPeek = WINDOWLIST;
	
	/* Check the windowKind field to see if it's mine */
	if ((windowPeek != nil)
		&& (windowPeek->windowKind == SCRAP_COERCION_FAKE_DA_WINDOW_ID))
			{
			/* If cut or copy bump the scrapcount.  Pretend we accepted it
			 * in any case.
			 */
			CheckForCutCopySysEditCmd(editCmd, &SCRAPCOUNT);
			retval = wTrue;
			}
	else
		{
		olda5 = ProcessMgrA5Setup();
	
		/* Check if we came in here for a cut or copy */
		CheckForCutCopySysEditCmd(editCmd, &cutCopyCount);
		
		oldtrap = patchtraps[SYSEDIT].oldtrap;	
		A5Restore(olda5);
		retval = CALL_FNC_PTR(Sysedit_ptr,oldtrap,(editCmd));
		}
		
	return retval;
	}

/* c_systemmenu.  Our chance to have real menus! */
pascal void
c_systemmenu(long menuResult)
	{
	PEntryPtr		pProc;
	LayerPeek		pNextLayer;
	Boolean			hideFront;
	unsigned long 	olda5;
	pascal void		(*oldtrap)();
	
	olda5 = ProcessMgrA5Setup();
	oldtrap = patchtraps[SYSTEMMENU].oldtrap;	

	/* Was it our menu (versus a DA or other system menu)? */
	if (MENU_ID_OF_SELECTION(menuResult) != kApplicationMenuID)
		{
		A5Restore(olda5);
		CALL_FNC_PTR(Systemmenu_ptr,oldtrap,(menuResult));
		return;
		}

	/* Our menu was selected from.  Do the right thing. */
	pProc = nil;
	switch (ITEM_ID_OF_SELECTION(menuResult))
		{			
		case HIDE_APP_ITEM:
			hideFront = true;
			if ((pNextLayer = LGetNextLayer(GetLayer(), true)) != nil)
				pProc = LayerOwner(pNextLayer);
			break;

		case HIDE_OTHERS_ITEM:
		case SHOW_ALL_ITEM:
			ShowHideOthers(ITEM_ID_OF_SELECTION(menuResult) == SHOW_ALL_ITEM);
			AdjustApplicationMenu(nil);
			break;
		
		/* Process name was selected */
		default:
			{
			/* Sample option key promptly */
			hideFront = OptionKeyIsDown();
			
			/* Initiate foreground switch to the selected process */
			if ((pProc = PEntryFromApplicationMenuItem(ITEM_ID_OF_SELECTION(menuResult))) == pFrontProcess)
				pProc = nil;
			break;
			}
		};
	
	/* Handle foreground switch and optional hiding */
	if ((pProc != nil) && (SetFrontProcess(&pProc->p_serialNumber) == noErr) && hideFront)
		HideLayer(GetFrontAppLayer(), false);

	/* Disemblazon the menu */
	HiliteMenu(0);
	A5Restore(olda5);
	}

/* c_systemclick.  Yet another step in scrap coercion. */
pascal void
c_systemclick(EventRecord *evtPtr, WindowPeek *windowPtr)
	{
	unsigned long 	olda5;
	pascal void		(*oldtrap)();
	
	olda5 = ProcessMgrA5Setup();
	
	/* Coercion state CS_CLICK_IN_WINDOW said mouse down in fake window, and
	 * sets state to CS_TAKE_DOWN_WINDOW.  App responds by calling _SystemClick,
	 * so now take down fake window to simulate a _CloseDeskAcc.  Next call(s)
	 * to _WaitNextEvent, _GetNextEvent, or _EventAvail should return the activate
	 * events for the (new) top window.
	 */
	if (coercionState == CS_TAKE_DOWN_WINDOW)
		{
		TakeDownFakeWindow();
		}
	else
		{	
		oldtrap = patchtraps[SYSTEMCLICK].oldtrap;	
		CALL_FNC_PTR(Systemclick_ptr,oldtrap,(evtPtr,windowPtr));
		}
		
	A5Restore(olda5);
	}

#define IS_UPDATE_OR_ACTIVATE_EVENT(eCode) ( (eCode == updateEvt) || (eCode == activateEvt) )

/* c_systemevent.  If its a key or any event with our window in the message field,
 * bump SCRAPCOUNT and return true.
 * NOTE:  Like much of the desk mgr, we don't understand completely why we can't
 * can't call the old routine inside of _MenuSelect.  There is no apparent reason
 * why we should, except to pass on an update event that would invalidate the menu
 * saved-behind bits (but this should never happen).  Right now we always pass thru
 * disk-insert events since we need to always get them mounted.
 */
pascal short
c_systemevent(EventRecord *theevent)
	{
	unsigned long 	olda5;
	WindowPeek		windowPeek;
	LayerPtr		pLayer;
	short			eventCode;
	Boolean			isCmdKeyDown;
	short			dontPassThru;
	pascal void		(*oldtrap)();
	short			retval;
	
	eventCode = theevent->what;
	isCmdKeyDown = ((eventCode == keyDown) && ((theevent->modifiers & cmdKey) != 0));
	
	/* Get the window we're going to deal with, based on the event handling table in ROM _SystemEvent */
	windowPeek = IS_UPDATE_OR_ACTIVATE_EVENT(eventCode) ? (WindowPeek)theevent->message : (WindowPeek) FrontWindow();

	/* If it is our fake DA window, swallow the events, possibly looking for cut or copies */
	if (windowPeek != nil && windowPeek->windowKind == SCRAP_COERCION_FAKE_DA_WINDOW_ID)
		{
		if (isCmdKeyDown)
			CheckForCutCopyMenuKey((char)theevent->message, &SCRAPCOUNT);

		/* Have to return false for pseudoevents */
		return (theevent->what != app4Evt);
		}
		
	olda5 = ProcessMgrA5SimpleSetup();
	oldtrap = patchtraps[SYSTEMEVENT].oldtrap;
	dontPassThru = wFalse;
	retval = wFalse;

	if ( (coercionState == CS_DONE) && imLayerKeyDown( theevent, true) )
		{
		theevent->what = nullEvent;
		dontPassThru = wTrue;
		retval = wTrue;
		}
	dontGetFakeKey = 0;

	if (isCmdKeyDown && coercionState == CS_DONE)
		CheckForCutCopyMenuKey((char)theevent->message, &cutCopyCount);
		
	/* Is the event an update to a window in another app's layer?  If so, we need to
	 * return wTrue (so app doesn't try to handle event for window it doesn't own), AND
	 * not call through (preventing SystemEvent from trying to give the update to a DA
	 * that is currently switched out.
	 */
	if (eventCode == updateEvt)
		{
		if ( ((pLayer = FindAppLayer(windowPeek)) != nil) &&
			(LayerOwner(pLayer) != pCurrentProcess) )
			{
			dontPassThru = wTrue;
			retval = wTrue;
			}
		}

	/* No?  Check for the debugger key */
	else if ( (pDebugProcess != nil)
		&&	(theevent->what == keyDown)
		&& (*((char *) &theevent->message + 2) == debugControlKeyCode)
		&& ((theevent->modifiers & (cmdKey | optionKey)) == (cmdKey | optionKey))	)
			{
			/* Saw the debug key from a "safe" level.  Reset counter because we
			 * succeeded getting from PostEvent to GNE to SystemEvent.  Make a
			 * system error that the debugger will interpret as "user hit the
			 * debugger key".  This SysError will return!
			 */			
			debugKeyTryCount = -1;
			SysError(enterDebugger);
			
			dontPassThru = wTrue;
			}

	A5SimpleRestore(olda5);

	if ((dontPassThru == wFalse) || theevent->what == diskEvt)
		retval = CALL_FNC_PTR(Systemevent_ptr,oldtrap,(theevent));

	return retval;
	}

#ifdef WRITENOW_FIX
/* ********** NOTE:  The following 2 routines are done for WriteNow to work. ********* */	

/* OverlaySystemEvent.  Set a5 to CURRENTA5, then call app's _SystemEvent.
 * NOTE:  This is a hack for WriteNow that should be taken out when it is rev'ed
 * (or at least check for 'VERS').
 */
pascal short
OverlaySystemEvent(EventRecord *theevent)
	{
	unsigned long 	olda5;
	pascal void		(*apptrap)();
	short			retval;
	
	/* Get app's patch routine addr */
	olda5 = ProcessMgrA5SimpleSetup();
	(ProcPtr)apptrap = (*pCurrentProcess->p_pcb)->appSystemEventPatch;
	A5SimpleRestore(olda5);
	
	/* Call it using CURRENTA5, but SystemEvent caller's graphics pointer */
	olda5 = CurrentA5Setup();
	retval = CALL_FNC_PTR(Systemevent_ptr,apptrap,(theevent));
	A5Restore(olda5);
	
	return retval;
	}
	
/* SetOverlaySystemEvent.  Set up SystemEvent patch address in current process, and
 * return the address of our overlay.
 * NOTE:  Assumes a5 == PROCESSMGRGLOBALS.
 */
ProcPtr
SetOverlaySystemEvent(ProcPtr apptrap)
	{
	(*pCurrentProcess->p_pcb)->appSystemEventPatch = apptrap;
	return ((ProcPtr) OverlaySystemEvent);
	}

/* **************************** */
#endif WRITENOW_FIX

/* LimitFrontSleepForDriversSake.  Calculate a desired maximum sleep time based on
 * how soon the next driver needs to be called (drivers are called via SystemTask which
 * is called by old apps and by event_common).
 * NOTE: Assumes that pCurrentProcess == pFrontProcess, since drivers installed in
 * the UNITTABLE but local to the current (non-FG) process are irrelevant.  Furthermore,
 * this must be called *before* the process has actually started to sleep, since we don't
 * monkey around with the sleep queue.
 * NOTE: Best called after SystemTask, since SystemTask may satisfy current need and
 * advance dCtlDelay.
 */
void
LimitFrontSleepForDriversSake(void)
	{
	register unsigned long	nextCall, minSoFar;
	register DCtlPtr		dCtlPtr;
	DCtlHandle				*pDCtlHdl;
	short					loopCount;
	
	/* Shortcut if process is not planning on sleeping anyway */
	if (pCurrentProcess->p_wakeuptime == 0)
		return;

	/* DA Handler should not sleep in the foreground, since it has a driver that
	 * probably needs cursor calls.
	 */
	if ((pCurrentProcess->p_taskmode & modeDeskAccessory) != 0)
		minSoFar = 0;
	else
		{
		minSoFar = 0xFFFFFFFF;
		pDCtlHdl = UNITTABLE;
		loopCount = UNITNTRYCNT;
		while ((--loopCount >= 0) && (minSoFar != 0))
			{
			if ( (*pDCtlHdl != nil) && ((dCtlPtr = **pDCtlHdl) != nil) && WantsTime(dCtlPtr) ) 
				{
				/* Check when driver should be called, handling dCtlDelay < MIN_DRIVER_DELAY
				 * and instances where we're late giving the driver time.  Also checks
				 * for wraparound (in which case minSoFar should be min'd with 0xFFFFFFFF,
				 * a NOP we can avoid!).
				 */
				if ((nextCall = (unsigned long) dCtlPtr->dCtlCurTicks + dCtlPtr->dCtlDelay) >= (unsigned long) dCtlPtr->dCtlCurTicks)
					{
					if (nextCall <= TICKS)
						nextCall = (dCtlPtr->dCtlDelay < MIN_DRIVER_DELAY) ? TICKS + MIN_DRIVER_DELAY : 0;
					
					if (nextCall < minSoFar)
						minSoFar = nextCall;
					}
				}
				
			pDCtlHdl++;
			}
		}
	
	/* Limit sleep time to calculated value. */
	if (pCurrentProcess->p_wakeuptime > minSoFar)
		pCurrentProcess->p_wakeuptime = minSoFar;
	}

/* LimitBackSleepForDriversSake.  Calculate a desired maximum sleep time based on
 * how soon the next driver needs to be called (drivers are called via SystemTask which
 * is called by old apps and by event_common).
 * NOTE: This must be called *before* the process has actually started to sleep, since
 * we don't monkey around with the sleep queue.
 * NOTE: Best called after SystemTask, since SystemTask may satisfy current need and
 * advance dCtlDelay.
 */
void
LimitBackSleepForDriversSake(void)
	{
	register DCtlPtr		dCtlPtr;
	register unsigned long	nextCall, minSoFar;
	DCEDescHdl				ppDCEDesc;
	DCEDescPtr				pDCEDesc;
	unsigned short			cDCE;
	
	/* Shortcut if process is not planning on sleeping anyway */
	if (pCurrentProcess->p_wakeuptime == 0)
		return;

	/* Get handle to DCE switch table, and find out how many local drivers there are.
	 * Leave now if there are nonesuch.
	 */
	if ((cDCE = GetHandleSize((ppDCEDesc = (*(pCurrentProcess->p_pcb))->dces))) == 0)
		return;

	/* Loop through the DCE switch table. */
	pDCEDesc = *ppDCEDesc;
	cDCE /= sizeof(**ppDCEDesc);		
	minSoFar = 0xFFFFFFFF;
	do
		{		
		if ( ((dCtlPtr = *((DCtlHandle) UNITTABLE[pDCEDesc->d_unit])) == nil) || (WantsTime(dCtlPtr) == false) )
			continue;
		
		/* Check when driver should be called, handling dCtlDelay < MIN_DRIVER_DELAY
		 * and instances where we're late giving the driver time.  Also checks
		 * for wraparound (in which case minSoFar should be min'd with 0xFFFFFFFF,
		 * a NOP we can avoid!).
		 */
		if ((nextCall = (unsigned long) dCtlPtr->dCtlCurTicks + dCtlPtr->dCtlDelay) >= (unsigned long) dCtlPtr->dCtlCurTicks)
			{
			if (nextCall <= TICKS)
				nextCall = (dCtlPtr->dCtlDelay < MIN_DRIVER_DELAY) ? TICKS + MIN_DRIVER_DELAY : 0;
			
			if (nextCall < minSoFar)
				minSoFar = nextCall;
			}
		
		/* Advance to next descriptor (let the counter catch us going off the end) */
		pDCEDesc += 1;
	} while ((--cDCE > 0) && (minSoFar != 0));
	
	
	/* Limit sleep time to calculated value. */
	if (pCurrentProcess->p_wakeuptime > minSoFar)
		pCurrentProcess->p_wakeuptime = minSoFar;
	}

/* GiveLocalDriversTime.  Give accRun calls to drivers local to the current process.
 * NOTE: It only gives time to drivers that were switched out of the unit table
 * when this guy was switched out, so if this guy opened one after the switch, he'll
 * have to wait for the next go-around to get the guy some time.
 * NOTE:  Routine optimized for case where task has no local drivers.
 */
void
GiveLocalDriversTime(void)
	{
	register DCtlPtr	dCtlPtr;
	DCEDescHdl			ppDCEDesc;
	DCEDescPtr			pDCEDesc;
	unsigned short		cDCE;
	unsigned long		tickDelay;
	CntrlParam			ctlPB;
	
	/* Get handle to DCE switch table, and find out how many local drivers there are.
	 * Leave now if there are nonesuch.
	 */
	if ((cDCE = GetHandleSize((ppDCEDesc = (*(pCurrentProcess->p_pcb))->dces))) == 0)
		return;
		
	/* Lock ppDCEDesc, because a control call could conceivably cause movement in
	 * the Process Mgr heap, most likely by causing sys heap growth.
	 */
	HLock(ppDCEDesc);

	/* Loop through the DCE switch table.  Calling each driver noted therein.
	 * NOTE:  dCtlPtr could come directly from *ppDCEDesc->d_handle, but then we lose
	 * the guarantee that it is actually in the unit table.  A misbehaving app might
	 * pull the guy out of the unit table, but leave him intact between the time the
	 * app was switched in, and the time SystemTask was called.
	 */
	pDCEDesc = *ppDCEDesc;
	cDCE /= sizeof(**ppDCEDesc);		
	do
		{		
		if ( ((dCtlPtr = *((DCtlHandle) UNITTABLE[pDCEDesc->d_unit])) == nil) || (WantsTime(dCtlPtr) == false) )
			continue;
		
		tickDelay = dCtlPtr->dCtlDelay;
		if (tickDelay == 0 || (unsigned long)dCtlPtr->dCtlCurTicks + tickDelay < TICKS)
			{
			/* Update him to show that he got time */
			dCtlPtr->dCtlCurTicks = TICKS;
			
			/* Give him some time */
			ctlPB.ioCRefNum = dCtlPtr->dCtlRefNum;
			ctlPB.csCode = accRun;
			ControlImmed(&ctlPB);
		
			/* Result can be positive since this is an immediate call */
			assert(ctlPB.ioResult >= 0);
			}
		
		/* Advance to next descriptor (let the counter catch us going off the end) */
		pDCEDesc += 1;
	} while (--cDCE > 0);
		
	HUnlock(ppDCEDesc);
	}

/* c_systemtask. A patch for SystemTask. Do nothing unless the calling process and
 * coercion are in a generic state, and the fake DA window is not up (can the window
 * be even be up when coercionState == CS_DONE?).  We can call the original routine
 * only if the current app is the frontmost, since the cursor handling and notification
 * manager rely on the WINDOWLIST to contain the active window.  If the current app is
 * not frontmost, we do the remaining part of the original routine.  Currently, this
 * only means giving control calls to the needs-time drivers.
 * NOTE: We disallow switching so we don't get screwed up by DA's that make event calls
 * (we could end up back here, and call them again, resulting in an infinite loop).
 * NOTE:  We bracket the call with BeginSystemMode/EndSystemMode so that files opened
 * by permanent system drivers are not closed when an app quits.  Unfortunately, this
 * is not quite bombproof because some "permanent drivers" (like QuickMail) are really
 * DAs that make event calls, which gives another app the chance to die.  ExitToShell
 * necessarily resets the SystemModeLevel to 0.  A file opened later in the same
 * control call would be tracked.  This shouldn't screw up legitimate users of system
 * mode, tho, because of the rule that it not be in effect over an event call.
 * NOTE: Have to set up CURRENTA5 because of case where a DA might bring up an
 * alert.  The problem is that if the alert causes any kind of event for one
 * of the underlying app's dialog windows, which might have a user item
 * installed and the user item might expect A5 == CURRENTA5.
 */
pascal void
c_systemtask(void)
	{
	register PEntryPtr	pCurPEntry;
	pascal void			(*oldtrap)();
	unsigned long		olda5;

	/* Give the system/toolbox some time */
	SynchIdleTime();
	
	olda5 = ProcessMgrA5Setup();
	pCurPEntry = pCurrentProcess;
	
	/* In FG: do old trap if not in the middle of menu activity or switching */
	if (pCurPEntry->p_state == PRRUN)
		{
		if ((coercionState == CS_DONE) &&
			(WINDOWLIST == nil || WINDOWLIST->windowKind != SCRAP_COERCION_FAKE_DA_WINDOW_ID))
			{
			(void) BeginSystemMode();
			oldtrap = patchtraps[SYSTEMTASK].oldtrap;
			A5Restore(CURRENTA5);

			CALL_FNC_PTR(Systemtask_ptr, oldtrap, ());

			A5Restore(PROCESSMGRGLOBALS);
			(void) EndSystemMode();
			}
		}
	
	/* In BG: restrict activity to control calls for current drivers. */
	else if (pCurPEntry->p_state == PRBACKRUN)
		GiveLocalDriversTime();

	A5Restore(olda5);
	}
