/*
	File:		Puppet.c

	Contains:	Routines which handle puppet strings.
				NOTE:  This is extremely ugly code, as much of it is transliterated
				from the Switcher 5.1 asm code.  However, it is readable.
				Feel free to clean it up.  Don't feel free to break it (it is fragile).

	Written by:	Phil Goldman

	Copyright:	© 1986-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <6>	 4/12/91	DFH		ewa, WS#DFH-910412a : Fix GetNextPuppetEvent to clear the
									puppet's puppetMasterProc when event queue is emptied. Leaving
									it non-nil caused puppet string escape mechanism in fg_resched
									to fire and crash when the user types cmd-comma, even long after
									the stringing was done.
		 <5>	 3/14/91	DFH		ngk,WS#DFH-910314a: Fixed GetNextPuppetEvent to
									generate_mousedown_in_menubar even for EventAvail, the way it
									used to be. Bug was code left over from partial back out of
									change made in Sep 90 for puppet string timeouts. Caused hang in
									MicroSoft Word when double-clicking Word documents.
		 <4>	  2/9/91	JWM		DFH,#79583:Restart and Shutdown don't work in low memory
									conditions. This involved several of the messages which are sent
									during the processing of a QuitAll message. The messages could
									be translated into puppet strings, hence changes in the memory
									allocation scheme changed in this module.The fix was to make a
									new version of ProcessMgrHiNewHandle which on failing to get
									memory from the Process Manager's heap would try to get memory
									from the system heap.
		 <0>	 11/9/86	PYG		New Today.

*/ 
 
#include <types.h>
#include <errors.h>
#include <resources.h>
#include <events.h>
#include <memory.h>
#include <osutils.h>
#include <toolutils.h>
#include <menus.h>
#include <Processes.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Patches.h"
#include "Puppet.h"

Handle			defaultPuppetStringHdl;

/* Function prototypes */
Boolean			CheckLocalPt(Point *, short);
void			SetMousePosition(Point);
void			generate_mousedown_in_menubar(EventRecord *);
short			MyRelString(StringPtr, StringPtr);
short			MyPrefixRelString(StringPtr, StringPtr);
short			GetMenuStrings(short, StringPtr *);
long			MenuSelectionGivenName(MenuHandle, StringPtr);

/* Puppet String related globals */
Point			buttonUpPt;					/* point where mouse button comes up */
long			forcedPupStrMenuSelection;	/* menu selection we are stuffing */
short			timeoutMaskYield;			/* number of times to wait for a legal event mask */
short			timeoutButtonDown;			/* number of times to wait for a mouse up event */
	
#pragma segment INIT
void
PuppetStringsInit(void)
	{	
	timeoutMaskYield = TIMEOUT_MASKYIELD;
	defaultPuppetStringHdl = GetResource(SYS_PUPPET_STRING_RSRC_TYPE, SYS_PUPPET_STRING_RSRC_ID);
	assert(defaultPuppetStringHdl != nil);
	}

#pragma segment Main

/* InitPuppets.  Set up initial state of puppeting for the given process.
 * NOTE: Assumes that pProc->p_puppetvars was zeroed when PEntry was created.
 * NOTE: Assumes that a) p_signature has been set, b) the application file the current
 * resource file, and c) that the PCB is available.
 */
void
InitPuppets(PEntryPtr pProc)
	{
	Handle		pupStrHdl;
	
	/* Locate resource that we can use to convert scratchpad instructions to puppet
	 * string event records.  Tries first in current resource file, then in the system
	 * file.  If neither of these works, it uses the default we picked up earlier.  If
	 * we didn't use the default, we detach the resource in case multiple guys are
	 * running (each needs its own copy since data is stored these weirdos).
	 */
	if (((pupStrHdl = Get1Resource(pProc->p_signature, APP_PUPPET_STRING_RSRC_ID)) == nil)
	&&	((pupStrHdl = GetResource(pProc->p_signature, SYS_PUPPET_STRING_RSRC_ID)) == nil))
		pupStrHdl = defaultPuppetStringHdl;
	
	/* Got a resource, so detach it */
	else
		DetachResource(pupStrHdl);

	(*pProc->p_pcb)->puppetstringhdl = pupStrHdl;
	}

/* PuppetKill.  Clean up puppet stringing for current process, since it is dying. */
void
PuppetKill(void)
	{
	CancelSwitch();
	}
	
/* CoreRequestScratchSwitchTask.  The meat of the work for sending a puppet string.
 * The caller passes an array of event records encoded as "instructions".  We tack
 * it onto the victim's process info for later consumption in the event loop.  The
 * event calls convert the instructions into "user" events like mousedown, menu
 * selections, etc.
 */
OSErr
CoreRequestScratchSwitchTask(PEntryPtr pDestProc, EventListHdl eventListInHdl, EventListHdl eventListOutHdl, short timeout, MFmsgBlkPtr pMsgBlk)
	{
	PuppetVars		*puppetVarsPtr;
	PEntryPtr		pSrcProc;
	OSErr			retval;

	/* Install the instructions */
	retval = (pMsgBlk != nil)
		? AddInstrsHdl(pDestProc, eventListInHdl)
		: PushInstrsHdl(pDestProc, eventListInHdl, evtMouseGlobal);
	if (retval != noErr)
		return(retval);
		
	SetOutInstrsHdl(pDestProc, (eventListOutHdl != nil) ? eventListOutHdl : (-1));
	
	/* Set up puppet master's information */
	pSrcProc = pCurrentProcess;
	puppetVarsPtr = &pSrcProc->p_puppetvars;
	puppetVarsPtr->masterResumeMsg = resumeFlag;
	puppetVarsPtr->masterPuppetProc = pDestProc;
	
	/* Set up victim puppet's information */
	puppetVarsPtr = &(pDestProc->p_puppetvars);
	puppetVarsPtr->puppetTimeout = timeout;
	puppetVarsPtr->puppetMasterProc = pSrcProc;
	puppetVarsPtr->puppetEppcMsgBlk = pMsgBlk;
	return(noErr);
	}
	
/* DoRequestScratchSwitchTask.  This trap is called by an application that wants
 * the specified process to receive puppet strings for some purpose.
 */
pascal void
DoRequestScratchSwitchTask(ProcessID pid, EventListHdl eventListInHdl, EventListHdl eventListOutHdl, short timeout)
	{
	PEntryPtr		pProc;
	unsigned long	olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	
	/* Arrange for foreground switch, then call core routine. */
	if (((pProc = PEntryFromPID(pid)) != nil) && (SetFrontProcess(&pProc->p_serialNumber) == noErr))
		(void) CoreRequestScratchSwitchTask(pProc, eventListInHdl, eventListOutHdl, timeout, nil);
	
	/* Force full scrap coercion */
	cutCopyCount++;
		
	A5SimpleRestore(olda5);
	}

/* DoGetActiveTaskInfo.  This trap returns process information and the count of
 * existing processes.  Caller passes the address and dimension of a TaskInfoRec
 * array.  We fill as many slots as possible, without overflow, and make sure to
 * give the correct process count, even if the array size is 0 (in which case,
 * taskInfo is undefined).  Omits background-only processes and DA Handlers.
 */
pascal short
DoGetActiveTaskInfo(TaskInfoRec taskInfo[1], short numSlots)
	{
	PEntryPtr		pPEntry;
	short			tasksFound, slotsFilled;
	Boolean			doFillTable;
	unsigned long	olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	
	/* See if caller just wants the process count */
	doFillTable = (numSlots != 0);
	
	/* Traverse list of processes */
	tasksFound = 0;
	slotsFilled = 0;
	pPEntry = pProcessList;
	while (pPEntry != nil)
		{
		if ((pPEntry->p_taskmode & (modeDeskAccessory + modeOnlyBackground)) == 0)
			{
			if (doFillTable && (slotsFilled < numSlots))
				{
				taskInfo->taskDescriptor = pPEntry->p_mypid;
				taskInfo->signature = pPEntry->p_signature;
				taskInfo->version = pPEntry->p_version;
				taskInfo++; slotsFilled++;
				}
			tasksFound++;
			}
		pPEntry = pPEntry->p_NextProcess;
		}
		
	A5SimpleRestore(olda5);
	return(tasksFound);
	}

/* DoGetSwitchInfo.  Return our own signature and version. */
pascal void
DoGetSwitchInfo(SwitchInfoRec switchInfo[1])
	{
	PEntryPtr		pNullProc;
	unsigned long	olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	pNullProc = pNullProcess;
	switchInfo->signature = pNullProc->p_signature;
	switchInfo->version = pNullProc->p_version;
	A5SimpleRestore(olda5);
	}

/* DoCancelSwitch.  Code for _CancelSwitch dispatch trap. */
pascal void
DoCancelSwitch(void)
	{
	PuppetVars		*myPuppetVarsPtr, *hisPuppetVarsPtr;
	PEntryPtr		pProc;
	unsigned long	olda5;

	olda5 = ProcessMgrA5Setup();

	myPuppetVarsPtr = &(pCurrentProcess->p_puppetvars);

	/* See if I am puppet master canceling a switch I just started to him */
	if ((pProc = myPuppetVarsPtr->masterPuppetProc) != nil)
		{
		hisPuppetVarsPtr = &(pProc->p_puppetvars);
		hisPuppetVarsPtr->puppetMasterProc = nil;
		hisPuppetVarsPtr->puppetTimeout = 0;
		myPuppetVarsPtr->masterPuppetProc = nil;
		
		coercionState = CS_DONE;
		
		/* Just in case we got this far in the scrap coercion */
		TakeDownFakeWindow();
		} 
	
	/* See if I am scratchpad task canceling from a switch he has made to me. */
	else if ((pProc = myPuppetVarsPtr->puppetMasterProc) != nil)
		{
		ClearInstrsQueue();
		
		/* If this was a translation, tell the sender the bad news.  Otherwise,
		 * set result we'll include in the puppet master's resume event.  Bring
		 * master back to the foreground.
		 */
		hisPuppetVarsPtr = &(pProc->p_puppetvars);
		if (myPuppetVarsPtr->puppetEppcMsgBlk != nil)
			{
			postTranslationResult(myPuppetVarsPtr->puppetEppcMsgBlk, reqAborted);
			myPuppetVarsPtr->puppetEppcMsgBlk = nil;
			hisPuppetVarsPtr->masterPuppetProc = nil;
			}
		else
			hisPuppetVarsPtr->masterResumeMsg = ResumeFromCancel;
		
		/* Start FG switch back to master */
		(void) SetFrontProcess(&pProc->p_serialNumber);
		
		/* Take current process out of PRPUPPET state so he can run */
		pCurrentProcess->p_state = PRRUN;
		}
	
	A5Restore(olda5);
	}

/* CancelSwitchWithError.  Cancel switch and set puppetmaster's resume event result code.
 * NOTE:  This should probably be made a trap.
 * NOTE:  Assumes A5 == PROCESSMGRGLOBALS
 */
void
CancelSwitchWithError(void)
	{
	PEntryPtr	pProc;
	
	CancelSwitch();
	if ((pProc = pCurrentProcess->p_puppetvars.puppetMasterProc) != nil)
		pProc->p_puppetvars.masterResumeMsg =
			IsOutEventListActive() ? ResumeEventListOutErr : ResumeEventListInErr;
	}

/* ---------------------------------------------------------------------
 *
 * Below this line are the support routines for the puppet stuff.
 *
 * ---------------------------------------------------------------------
 */

/* AddInstrsHdl.  Append event list to end of queue for specified process.
 * NOTE: Unconditionally sets instrModBits to a safe value since this field is
 * not needed in the lists for which we make this call.
 */
OSErr
AddInstrsHdl(PEntryPtr pProc, EventListHdl eventListHdl)
	{
	register PuppetInstrElemPtr		pPupElem; 
	register PuppetInstrElemHdl		hCurrPupElem;
	PuppetInstrElemHdl				hPupElem, hNextPupElem;
	
	if ((hPupElem = (PuppetInstrElemHdl) ProcessMgrNewHandle(sizeof(PuppetInstrElem))) == nil)
		return(memFullErr);
	
	pPupElem = *hPupElem;
	pPupElem->instrsHdl = (Handle) (StripAddress(eventListHdl));
	pPupElem->instrModBits = evtMouseGlobal;
	pPupElem->nextPuppetInstrElem = nil;

	if ((hCurrPupElem = pProc->p_puppetvars.puppetInstrQueue) == nil)
		pProc->p_puppetvars.puppetInstrQueue = hPupElem;
	else
		{
		while ((hNextPupElem = (*hCurrPupElem)->nextPuppetInstrElem) != nil)
			hCurrPupElem = hNextPupElem;
		(*hCurrPupElem)->nextPuppetInstrElem = hPupElem;
		}
		
	return(noErr);
	}

/* PushInstrsHdl.  Insert the given instruction list as the first in the queue for
 * the specified process.
 */
OSErr
PushInstrsHdl(PEntryPtr pProc, EventListHdl eventListHdl, short locModBits)
	{
	register PuppetInstrElemPtr		pPupElem; 
	PuppetInstrElemHdl				hPupElem, hFirstElem;
	short							oldLocModBits;
	OSErr							error;

	if ((hPupElem = (PuppetInstrElemHdl) GMBlk(sizeof(PuppetInstrElem), &error)) == nil)
		return(memFullErr);
	
	oldLocModBits = ((hFirstElem = pProc->p_puppetvars.puppetInstrQueue) != nil)
		? (*hFirstElem)->instrModBits : evtMouseGlobal;

	pPupElem = *hPupElem;
	pPupElem->instrsHdl = (Handle) (StripAddress(eventListHdl));
	pPupElem->nextPuppetInstrElem = hFirstElem;
	pProc->p_puppetvars.puppetModBits = evtMouseGlobal;
	if ( ((locModBits & evtMouseLocal) != 0)
		|| (((locModBits & evtMouseEither) != 0) && ((oldLocModBits & evtMouseLocal) != 0)) )
		pProc->p_puppetvars.puppetModBits = evtMouseLocal;
	pProc->p_puppetvars.puppetInstrQueue = hPupElem;
		
	return(noErr);
	}

/* DumpFirstInstr.  Remove and free the first puppet element in the current process.
 * Returns handle to next element in list, if any.
 */
PuppetInstrElemHdl
DumpFirstInstr(void)
	{
	register PuppetInstrElemHdl		hPupElem, retval;
	register EventListHdl			hEvtList;
	
	if ((hPupElem = pCurrentProcess->p_puppetvars.puppetInstrQueue) == nil)
		return(nil);
		
	pCurrentProcess->p_puppetvars.puppetInstrQueue = retval = (*hPupElem)->nextPuppetInstrElem;
	if ((hEvtList = (*hPupElem)->instrsHdl) != nil)
		DisposHandle(hEvtList);
	DisposHandle(hPupElem);
	
	/* Return next in list */
	return(retval);
	}

EventListHdl
GetActiveInstrsHdl(void)
	{
	register PuppetInstrElemHdl		hPupElem;

	return ((hPupElem = pCurrentProcess->p_puppetvars.puppetInstrQueue) != nil)
		? (*hPupElem)->instrsHdl : nil;
	}

short 
GetNextPuppetEvent(u_short eventmask, EventRecord *theevent, Ptr oldtrap, Boolean pullRealEvent)
	{
	EventListHdl	activeInstrsHdl;
	EventListPtr	activeInstrsPtr;
	EventRecord		*evtPtr;
	Handle			puppetStringHdl;
	Ptr				puppetStringPtr, puppetStringBasePtr;
	u_char			entryIndex;
	short			retval;
	Boolean			pullEvent;
	PuppetVars		*myPuppetVarsPtr;
	PEntryPtr		pProc;

	/* Initial setup */
	puppetStringHdl = nil;
	pullEvent = true;
	retval = wFalse;
	
	/* Check for activates only, and only if caller wants them.  Also fills in
	 * miscellaneous event record fields.
	 */
	if (CALL_FNC_PTR(Eventcommon_ptr,oldtrap,(eventmask & activMask, theevent)))
		return(wTrue);

	/* Deal with dummies that put a nil handle in the stack */
	if ((activeInstrsHdl = GetActiveInstrsHdl()) == nil)
		goto label_pop_stack;
	
	activeInstrsPtr = *activeInstrsHdl;
	if (activeInstrsPtr->count != 0)
		{
		evtPtr = ((Ptr) activeInstrsPtr) + (u_long) activeInstrsPtr->offset;

		/* Treat non-highlevel event as normal.
		 * NOTE: Therefore puppetStringHdl never set up, so be careful.
		 */
		if (*((char *) &evtPtr->what + 1) != app4Evt)
			goto label_instr_evt;
			
		/* High level event */
		puppetStringHdl = (*pCurrentProcess->p_pcb)->puppetstringhdl;
		
		for (;;)
			{
			/* Try up this sequence with given puppet string, then default if necessary */
			puppetStringPtr = puppetStringBasePtr = *puppetStringHdl;
			entryIndex = *((u_char *) &evtPtr->message);

			/* Check for out of bounds.  If too small then it must be a mistake.
			 * If too big then assume it is an event the app can handle itself.
			 */
			if (entryIndex < ((u_char) instrReset))
				goto label_instr_illegal;
			entryIndex -= ((u_char) instrReset);
			if (entryIndex >= *((u_char *) puppetStringPtr))
				{
				if (puppetStringHdl == defaultPuppetStringHdl)
					goto label_instr_illegal;
				else
					{
					puppetStringHdl = defaultPuppetStringHdl;
					continue;
					}
				}
			break;
			}
			
		HLock(puppetStringHdl);
		HLock(activeInstrsHdl);
		
		/* Point at entry */
		puppetStringPtr += 2 + (entryIndex * 6);
		
		switch (*((short *) puppetStringPtr))
			{
			case pstNormalEvent:
label_instr_evt:
				{
				theevent->what = (evtPtr->what & 0x000F);

				/* Can we pass it through? */
				if (((eventmask & (1<<theevent->what))) != 0)
					timeoutMaskYield = TIMEOUT_MASKYIELD;
				else
					{						
					if (--timeoutMaskYield <= 0)
						{
						timeoutMaskYield = TIMEOUT_MASKYIELD;
						if ((evtPtr->what & evtIsUnimportant) != 0)
							{
							/* Timed out on it so just skip it */
							pullEvent = true;
							retval = wFalse;
							break;
							}
						else
							goto label_instr_illegal;
						}

					/* Unlock handles we used.  Careful with puppetStringHdl since it
					 * was not set up if instruction was low level event.
					 */
					if (puppetStringHdl != nil)
						HUnlock(puppetStringHdl);
					HUnlock(activeInstrsHdl);
					return(wFalse);
					}
				
				/* If we got here the event is passable */
				
				/* copy rest of src to dst record, updating the tick count */
				theevent->message = evtPtr->message;
				theevent->when = TICKS;
				PT_TO_PT(&evtPtr->where,&theevent->where);
				theevent->modifiers = evtPtr->modifiers;
				
				if (theevent->what == mouseDown)
					{
					if (*((char *) &(evtPtr + 1)->what + 1) != mouseUp)
						goto label_instr_illegal;
					if (CheckLocalPt(&theevent->where, evtPtr->what) == false)
						goto label_instr_illegal;
					SetMousePosition(theevent->where);
					if (pullRealEvent)
						timeoutButtonDown = TIMEOUT_BUTTONDOWN;	/* Initiate drag */
					PT_TO_PT(&(evtPtr + 1)->where, &buttonUpPt);
					if (CheckLocalPt(&buttonUpPt, (evtPtr + 1)->what) == false)
						goto label_instr_illegal;
					}
				else
					{
					if (CheckLocalPt(&theevent->where, evtPtr->what) == false)
						goto label_instr_illegal;
					timeoutButtonDown = 0;
					SET_PT(&buttonUpPt,0,0);
					}
				
				/* Event should be dequeued if the caller asked for that, or the event
				 * is not a physical event.
				 */
				pullEvent = (pullRealEvent || (theevent->what >= updateEvt));
				retval = wTrue;

				break;
				}

/*			case pstNullInstr:
 *			case pstUnknownInstr:
 *					break;
 */					
			/*	This instruction will select a menu item
			 *		titleID: 	look for menu title which matches GetResource(menuStringResType, titleID)
			 *		itemID:  	look for menu title which matches GetResource(menuStringResType, itemID)
			 */		
			case pstMenuString:
				{
				register MenuPair	*menuRecPtr;
				MenuHandle			menuHdl;
				MenuListPtr			menuListPtr;
				Ptr					stringPtr;
				short				cMenuStrings;

				puppetStringPtr += sizeof(short);
				
				/* fail if no menus */
				if (MENULIST == nil)
					goto label_instr_illegal;
					
				/* fail if can't get menu title string */
				if ((cMenuStrings = GetMenuStrings(*((short *)puppetStringPtr)++, &stringPtr)) == 0)
					goto label_instr_illegal;
			
				/* search for menu having desired title */
				menuListPtr = (*(MenuListHandle) MENULIST);
				assert(menuListPtr != nil);
				menuRecPtr = (Ptr) menuListPtr + menuListPtr->lastMenu;
				while (menuRecPtr > menuListPtr)
					{
					menuHdl = menuRecPtr->menuHdl;
					if (StrInStrList(&(*menuHdl)->menuData, stringPtr, cMenuStrings, MyRelString) != nil)
						break;
					--menuRecPtr;
					}
				
				/* fail if couldn't find menu title */
				if (menuRecPtr == menuListPtr)
					goto label_instr_illegal;
					
				/* fail if can't get menu item text */
				if ((cMenuStrings = GetMenuStrings(*((short *)puppetStringPtr), &stringPtr)) == 0)
					goto label_instr_illegal;
		
				/* search for menu item having desired text */
				while (--cMenuStrings >= 0)
					{
					if ((forcedPupStrMenuSelection = MenuSelectionGivenName(menuHdl, stringPtr)) != 0)
						break;
					stringPtr += Length(stringPtr) + 1;
					}
	
				/* fail if couldn't find menu item */
				if (forcedPupStrMenuSelection == 0)
					goto label_instr_illegal;
			
				/* NOTE:  Should find a way to turn this into an event list of length 1 */
				pullEvent = false;
				if ((eventmask & mDownMask) != 0)
					{
					pullEvent = pullRealEvent;	/* This is a low-level event, after all */
					generate_mousedown_in_menubar(theevent);
					retval = wTrue;
					timeoutMaskYield = TIMEOUT_MASKYIELD;
					}
				else if (--timeoutMaskYield <= 0)
					{
					/* Got fed up waiting */
					timeoutMaskYield = TIMEOUT_MASKYIELD;
					forcedPupStrMenuSelection = 0;
					goto label_instr_illegal;
					}
				
				break;
				}

			case pstMenuItem:
				{			
				forcedPupStrMenuSelection = *((long *) (puppetStringPtr+sizeof(short)));
				pullEvent = false;
				if ((eventmask & mDownMask) != 0)
					{
					pullEvent = pullRealEvent;
					generate_mousedown_in_menubar(theevent);
					retval = wTrue;
					}
					
				break;
				}
				
			case pstEventList:
				{
				short			paramCount;
				Ptr				paramBlkPtr;
				EventListHdl	eventListHdl;
				
				puppetStringPtr += sizeof(short);	/* skip word */
				puppetStringPtr = puppetStringBasePtr + *((u_long *) puppetStringPtr);
				
				paramCount = *((short *) puppetStringPtr)++;
				
				/* Get the parameter list pointer.  The AND w/LO3BYTES is a field
				 * extraction operation, not an address masking, so don't use
				 * StripAddress.
				 * NOTE: This is NOT 32-bit clean!  We can not change it, tho, without
				 * wrecking old apps that send these (MicroSoft Word).  Instead, those
				 * apps will have to use AppleEvents (and do away with puppet strings) 
				 * to become 32-bit clean.
				 */
				paramBlkPtr = (Ptr) ((*(u_long *)(evtPtr->message)) & LO3BYTES);
				
				/* Substitute in the parameters */
				while (paramCount--)
					{
					short		len;
					Ptr			dstPtr;
					
					dstPtr = &puppetStringBasePtr[*((u_long *) puppetStringPtr)++];
					len = *((short *) puppetStringPtr)++;
					BlockMove(paramBlkPtr, dstPtr, len);
					paramBlkPtr += len;
					}
					
				/* puppetStringPtr now points at an event list */
				if (PtrToHand(puppetStringPtr, &eventListHdl, 6 + ((EventListPtr) puppetStringPtr)->count * sizeof(EventRecord)) == noErr)
					PushInstrsHdl(pCurrentProcess, eventListHdl, evtPtr->what);

				break;
				}

			case pstEventCode:
				{
				EventListHdl	eventListHdl;
				short			locModBits = evtPtr->what;
				
				puppetStringPtr += sizeof(short);
				puppetStringPtr = puppetStringBasePtr + *((u_long *) puppetStringPtr);
				eventListHdl = CALL_FNC_PTR(Puppetstringevtcode_ptr,puppetStringPtr,(evtPtr));
				
				if (eventListHdl == nil)
					goto label_instr_illegal;
				PushInstrsHdl(pCurrentProcess, eventListHdl, locModBits);
				activeInstrsPtr = *activeInstrsHdl;

				break;
				}

			case pstImplementCode:
				{
				puppetStringPtr += sizeof(short);
				puppetStringPtr = puppetStringBasePtr + *((u_long *) puppetStringPtr);
				if (CALL_FNC_PTR(Puppetstringimplcode_ptr, puppetStringPtr, (evtPtr)) == 0)
					goto label_instr_illegal;
				
				/* we're done if we canceled inside */
				if (InstrsQueueIsEmpty())
					return(wFalse);

				activeInstrsPtr = *activeInstrsHdl;

				break;
				}

			case pstIllegalInstr:
label_instr_illegal:
				{
				PEntryPtr		pMaster;
				
				/* Dump remaining instructions, and cause switch back to puppet master
				 * (puts future trap error code in p_puppetvars.masterResumeMsg).
				 */
				ClearInstrsQueue();
				
				myPuppetVarsPtr = &pCurrentProcess->p_puppetvars;
				pMaster = myPuppetVarsPtr->puppetMasterProc;
				if (myPuppetVarsPtr->puppetEppcMsgBlk != nil)
					{
					pMaster->p_puppetvars.masterPuppetProc = nil;
					postTranslationResult(myPuppetVarsPtr->puppetEppcMsgBlk, paramErr);
					myPuppetVarsPtr->puppetEppcMsgBlk = nil;
					}				
				else
					pMaster->p_puppetvars.masterResumeMsg =
						IsOutEventListActive() ? ResumeEventListOutErr : ResumeEventListInErr;
					
				/* Unlock handles we used.  Careful with puppetStringHdl since it
				 * was not set up if instruction was low level event.
				 */
				HUnlock(activeInstrsHdl);
				if (puppetStringHdl != nil)
					HUnlock(puppetStringHdl);

				/* Start FG switch back to master */
				(void) SetFrontProcess(&pMaster->p_serialNumber);
			
				pCurrentProcess->p_state = PRRUN;
				
				return(wFalse);
				}
				
			}  /* end of mega SWITCH/CASE statement */
		
		/* Out of switch statement now.  Unlock handles we used.  Careful with
		 * puppetStringHdl since it was not set up if instruction was low level event.
		 */
		HUnlock(activeInstrsHdl);
		if (puppetStringHdl != nil)
			HUnlock(puppetStringHdl);
		
		if (pullEvent)
			activeInstrsPtr->count -= 1;
		
		}
		
	/* What to do now? */
	if (pullEvent)
		{
		if (activeInstrsPtr->count != 0)
			activeInstrsPtr->offset += sizeof(EventRecord);
		else if (GetActiveInstrsHdl() == activeInstrsHdl)
			{
			/* Pop the instruction stack.  If this leaves an empty list, and the puppet
			 * string was an EPPC translation, tell the good news to the EPPC sender
			 * (checks activeInstrsHdl != nil because this guarantees that we just went
			 * from non-empty to empty).
			 */
label_pop_stack:
			if (DumpFirstInstr() == nil)
				{
				myPuppetVarsPtr = &pCurrentProcess->p_puppetvars;
				if ((activeInstrsHdl != nil) && (myPuppetVarsPtr->puppetEppcMsgBlk != nil))
					{
					pProc = myPuppetVarsPtr->puppetMasterProc;
					assert(IsProcessEntry(pProc));
					pProc->p_puppetvars.masterPuppetProc = nil;
					postTranslationResult(myPuppetVarsPtr->puppetEppcMsgBlk, noErr);
					myPuppetVarsPtr->puppetEppcMsgBlk = nil;
					}
				myPuppetVarsPtr->puppetMasterProc = nil;
				pCurrentProcess->p_state = PRRUN;
				}
			}
		}
	
	return(retval);
	}

/*------------------------------------------------------------------*/

/* Get1MenuStrings.  Look in current resource file for puppet menu table. */
short
Get1MenuStrings(short id, StringPtr *pStrPtr)
	{
	Handle		stringHdl;
	Ptr			stringPtr;
	short		cMenuItems = 0;

	if ((stringHdl = Get1Resource(menuStringListResType,id)) != nil)
		{
		stringPtr = *stringHdl;
		cMenuItems = *((short *)stringPtr)++;
		}
	else
		{
		if ((stringHdl = Get1Resource(menuStringResType, id)) != nil)
			{
			stringPtr = *stringHdl;
			cMenuItems = 1;
			}
		}
		
	*pStrPtr = stringPtr;
	return(cMenuItems);
	}

/* GetMenuStrings.  Locate the table of menu item names we will use when generating
 * menu selection events.  First choice is a custom table from the app file.  Falls
 * back to generic table in the system file.
 * NOTE:  This routine depends on ResSysHeap(MENU_STRING_RES_TYPE, id) == true if from
 * system file.
 * NOTE: Handle is not locked so no sys heap mem movement can go on during use
 * of **pStrPtr.
 */
short
GetMenuStrings(short id, StringPtr *pStrPtr)
	{
	short		saveCurMap, cMenuItems;

	saveCurMap = CURMAP;
	CURMAP = CURAPREFNUM;
	if ((cMenuItems = Get1MenuStrings(id, pStrPtr)) == 0)
		{
		CURMAP = SYSMAP;
		cMenuItems = Get1MenuStrings(id, pStrPtr);
		}
		
	CURMAP = saveCurMap;	
	return(cMenuItems);
	}

/* CheckLocalPt.  Given a point and the mod bits for a particular mouse puppet event,
 * convert the point to local coordinates if it is appropriate.  Function result comes
 * back false if the conversion was required, but could not be done.
 * The conversion must be done if either a) the event's mods say evtMouseLocal, or
 * b) the event's mods say evtMouseEither and the instruction list's mods say
 * evtMouseLocal.
 */
Boolean	
CheckLocalPt(Point *wherePtr, short locModBits)
	{
	WindowPeek		topVisibleWindowPtr;
	RgnHandle		contRgn;
	Rect			*pRgnBBox;
	
	/* Bail out if conversion not appropriate */
	if ( ((locModBits & evtMouseLocal) == 0)
		&& ( ((locModBits & evtMouseEither) == 0)
			|| ((pCurrentProcess->p_puppetvars.puppetModBits & evtMouseLocal) == 0) ) )
		return(true);

	/* Make the point local.  Look for first non-Ghost, visible
	 * window w/ a content rgn.
	 */
	topVisibleWindowPtr = WINDOWLIST;
	for(;;)
		{
		/* couldn't be found if we're now off list end */
		if (topVisibleWindowPtr == nil)
			return(false);
		
		if (topVisibleWindowPtr->visible
			&& ((contRgn = topVisibleWindowPtr->contRgn) != nil)
			&& (topVisibleWindowPtr != GHOSTWINDOW))
				break;
		
		topVisibleWindowPtr = topVisibleWindowPtr->nextWindow;
		}
		
	/* If we go to this point we got a valid window.  Get the bounding box. */
	pRgnBBox = &((*contRgn)->rgnBBox);
	if ((wherePtr->v += pRgnBBox->top) > pRgnBBox->bottom)
		return(false);
	if ((wherePtr->h += pRgnBBox->left) > pRgnBBox->right)
		return(false);

	return(true);
	}

void
SetMousePosition(Point mousePosInLongword)
	{
	short	ps;
	
	assert(sizeof(long) == sizeof(Point));

	/* Don't want interference from mouse, cursor handlers */
	ps = disable();
	
	/* Force cursor task to deal with it */
	CRSRNEW = true;
	
	MOUSE = mousePosInLongword;
	RAWMOUSE = mousePosInLongword;
	MTEMP = mousePosInLongword;
	
	/* Turn interrupts back on */
	spl(ps);
	}
		

/* PuppetForceButtonDown.  Return whether we are puppeting a new mouse position.  And if
 * we are, set the mouse to the prescribed position.
 * NOTE: Assumes A5 == PROCESSMGRGLOBALS.
 */
Boolean
PuppetForceButtonDown(void)
	{
	Boolean		retVal;
	
	if (retVal = (timeoutButtonDown != 0))
		{
		timeoutButtonDown--;
		SetMousePosition(buttonUpPt);
		}
	
	return(retVal);
	}

/* generate_mousedown_in_menubar.  Fabricate an event record to make it look like the
 * user has held the mouse button down while the cursor is over the menu bar.
 * NOTE: Does this work well when someone has set MENUBARHEIGHT to 0?
 * NOTE: Do we need cmd-key down?  Switcher does it.
 */
#define X_PT_IN_MENUBAR	32
#define Y_PT_IN_MENUBAR	8
void
generate_mousedown_in_menubar(EventRecord *theevent)
	{
	theevent->what = mouseDown;
	theevent->message = 0;
	SET_PT(&theevent->where, X_PT_IN_MENUBAR, Y_PT_IN_MENUBAR);
	theevent->modifiers = 0;
	}
