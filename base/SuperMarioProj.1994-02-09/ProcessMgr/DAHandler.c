/*
	File:		DAHandler.c

	Contains:	The System Desk Accessory Handler application

	Written by:	Phil Goldman

	Copyright:	© 1986-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
	  <SM18>	 7/21/93	joe		Back out <SM17>.
		<16>	 9/25/92	DRF		Get rid of MyGestalt
		<15>	 4/10/92	JSM		Undo revision 14, we fixed the problem someplace else.
		<14>	  4/4/92	DTY		#1019369,<FM>: For each desk accessory, remember the current
									keyboard state, and the keyboard state after the driver has been
									opened. On a resume, set the keyboard to the state the driver
									left it, and on a suspend, set it to the original state.
		<13>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		<12>	  1/9/92	DTY		In CleanupAndExit, check to see if there are any more desk
									accessories running. If there aren’t, unload the DAHandler
									segment. ThereAreMoreDeskAccessories ripped off from scott’s 7
									Up code.  Also unload Init and %A5Init in main() after DAHandler
									is done with them.
		<11>	 11/1/91	DTY		TOPMAPHANDLE should be set to the top system override map in
									SetupMenus so DA’s can enjoy overridden system resources too.
									Also, use the generic desk accessory icon if the desk accessory
									is in a system override map.
		<10>	  4/4/91	DFH		VL, WS#DFH-910404a : Fixed CloseDACleanupAndExit to be more
									careful about a non-existent driver table entry.
		 <9>	 1/10/91	DFH		(dba) Got rid of InForeground, since it was just left over from
									the sleep time calculation that got removed.
		 <7>	  1/8/91	DFH		(dba) Removed stack shrinkage from StartupCode() since stack
									requirement is greater now that modal dialogs can call menu
									select.
		 <6>	  1/3/91	fjs		(DFH) handle edit function keys
		 <5>	  1/3/91	DFH		(fjs) OpenTheDeskAccessory() was setting the myDriverRefnum
									global to the OpenDeskAcc result. This is bad in the case that
									the result was not a real refnum, since our cleanup/exit code
									relies on it being -1 or a refnum. This resulted in wiping out
									random memory as we tried to MemClear() the DCE in
									CloseDACleanupAndExit().
		 <4>	12/10/90	DFH		Added custom icon support.
		 <3>	 12/5/90	DFH		Remove sleep time calculation.  Scheduler will do that for us
									now.
		 <2>	11/19/90	DFH		Robustified ExitToShell patch. Was same as Quit command, which
									is not good in crash cases. Now sets myUnitNumber and
									myDriverRefNum as early as possible, so we can deal with
									ExitToShell in OpenDeskAcc (need to close driver if it got
									open).
		 <0>	 x/xx/86	PYG		New Today.

*/ 

#include <types.h>
#include <memory.h>
#include <resources.h>
#include <quickdraw.h>
#include <fonts.h>
#include <events.h>
#include <windows.h>
#include <dialogs.h>
#include <controls.h>
#include <menus.h>
#include <textedit.h>
#include <devices.h>
#include <desk.h>
#include <toolutils.h>
#include <errors.h>
#include <osutils.h>
#include <segload.h>
#include <diskinit.h>
#include <sound.h>
#include <GestaltEqu.h>
#include <Palettes.h>
#include <Icons.h>
#include <IconUtilsPriv.h>
#include <traps.h>
#include <Processes.h>
#include <Layers.h>
#include <ResourceMgrPriv.h>

#include "Lomem.h"
#include "SysMisc.h"
#include "Glue.h"

#undef assert
#ifdef DEBUG
#define assert(exp) {if (!(exp)) SysError(dsCoreErr);}
#else
#define assert(exp)
#endif DEBUG
	
/*
 * Menu ID constants.  These are the positive counterparts of the resource IDs.
 */
#define					appleID				16473
#define					fileID 				16472
#define					editID 				16471

/*
 * Menu item constants.
 */
#define					appleMenu			0
#define						aboutMeCommand		1
#define					fileMenu			1
#define						closeCommand		1
#define						quitCommand			2
#define					editMenu			2
#define						undoCommand 		1
#define						cutCommand			3
#define						copyCommand 		4
#define						pasteCommand		5
#define						clearCommand		6

#define					menuCount	 3

/*
 * For the About… DLOG
 */
#define					aboutMeDLOG			(-16473)
#define						okButton			1
#define						daIcon				2
#define						daNameItem			3
#define						firstAboutString	4
#define						secondAboutString	5
#define					AlertID_OutOfMem	(-16472)
#define					DitlID_OutOfMem		(-16472)
#define					AlertID_NotFound	(-16471)
#define					DitlID_NotFound		(-16471)
#define					StrID_InSysFile		(-16471)

/* Extra slop to get up alert:  Window, icon, etc. */
#define ALERT_SLOP_SIZE		0x800

/* Masks and a macro to ensure alignment of counts and addresses.  Word alignment is
 * required by the 68000 for non-byte pointer refereneces.  Note that the result of
 * WORD_ALIGN is the same as the original if the original was already word-aligned.
 * Long alignment is for 68020 (and up) efficiency in general, and is a requirement
 * of blocks in a 32-bit mode heap.
 */
#define LONGALIGN_MASK		0xFFFFFFFC
#define WORDALIGN_MASK		0xFFFFFFFE
#define LONG_ALIGN(value)	(value = ((value + sizeof(long) - 1) & LONGALIGN_MASK))
#define WORD_ALIGN(value)	(value = ((value + sizeof(short) - 1) & WORDALIGN_MASK))
#define LONG_ALIGNED(value)	(((value) & (sizeof(long) - 1)) == 0)
#define WORD_ALIGNED(value)	(((value) & (sizeof(short) - 1)) == 0)

/* HIWORD and LOWORD macros, for readability. */
#define					HIWORD(aLong)			(((aLong) >> 16) & 0xFFFF)
#define					LOWORD(aLong)			((short)(aLong))

/* Macro to look up a particular byte in a pascal string */
#define StringByte(pStr, byteIndex)			(*((char *) pStr + byteIndex + 1))

/* Macros for Resource Mgr peeking */
#define	isNotDARefNum(refNum)	(((refNum) >= 0) || ((refNum) <= -UNITNTRYCNT))
#define MapRefNumOffset			20
#define TopResFile()			(*((short *) (*TOPMAPHANDLE + MapRefNumOffset)))
#define IsSystemRefNum(refNum)	((refNum == SYSMAP) || (refNum == 0))

/* Bit assignments in the dCtlFlags of a driver control block */
#define DNeedLock			(1<<(8+6))		/* Driver will be locked when opened? */
#define DNeedTime			(1<<(8+5))		/* Does driver need periodic control calls? */
#define DNeedGoodbye		(1<<(8+4))		/* Does driver need a goodbye kiss? */
#define DStatEnable			(1<<(8+3))		/* Can driver respond to Status calls? */
#define DCtlEnable			(1<<(8+2))		/* Can driver respond to Control calls? */
#define DWritEnable			(1<<(8+1))		/* Can driver respond to Write calls? */
#define DReadEnable			(1<<(8+0))		/* Can driver respond to Read calls? */
#define DActive				(1<<7)			/* Is driver currently executing? */
#define DRamBased			(1<<6)			/* Is driver RAM-based? */
#define DOpened				(1<<5)			/* Is driver already open? */

/* Symbols for values of the asych parameter of HFS calls */
#define AsyncHFS			true
#define SyncHFS				false

/* Function prototypes */
void			main(void);
void			StartupCode(void);
void			SetupMenus(void);
void			DoCommand(long);
void			OpenTheDeskAccessory(void);
OSErr			PreflightDA(void);
void			HandleCloseDA(short);
void			CloseDACleanupAndExit(Boolean);
void			ExitToShellPatch(void);
void			MemClear(Ptr, unsigned long);
void			PutUpDeathAlert(OSErr);
Boolean			IsDrvrOpen(void);

/* Functions imported from DAHandler.a */
void			CloseFrontDeskWindow(void);
void			ClosePatch(void);
void			SystemEventPatch(void);
StringPtr		GetSysHeapString(void);
Handle			GetMapHdlFromRefNum(short, Handle);
void			InitAboutMeDialog(void);

/* From %A5Init */
extern void _DataInit();

/*
 * Global Data objects.
 */
MenuHandle				MyMenus[menuCount]; 	/* The menu handles */
Ptr						oldCloseAddress;		/* Address of old _Close routine */
Ptr						oldSystemEventAddress;	/* Address of old _SystemEvent routine */ 
Ptr						oldExitToShellAddress;	/* Address of old _ExitToShell routine */
short					doneFlag;				/* Time to quit yet? */
Boolean					Colorized;				/* Whether this machine has color capability */
Boolean					Has32BitQD;				/* Whether this machine has 32-bit QuickDraw */
short					myFileRefNum;			/* Refnum of file holding the DRVR */
short					myDriverRefNum;			/* Refnum of driver resource */
short					myUnitNumber;			/* the UNITTABLE index of my DA */
short					myDisplayIconID;		/* Resource ID of icon and info strings */
short					wdShrinkOfAboutMe, htOfAboutMeStrings, htOfFirstAboutString, htOfSecondAboutString;
StringHandle			hFirstAboutMeString, hSecondAboutMeString;
#define SetPSN(X, dst)		{(dst)->highLongOfPSN = 0; (dst)->lowLongOfPSN = (X);}

/* ThereAreMoreDeskAccessories.  Returns true if there are other desk accessories running. */
Boolean
ThereAreMoreDeskAccessories()
	{
	int count = 0;
	ProcessSerialNumber psn = { 0, kNoProcess };
	ProcessInfoRec info;

	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName = nil;
	info.processAppSpec = nil;

	while (GetNextProcess(&psn) == noErr)
		{
		if (GetProcessInformation(&psn, &info) == noErr)
			{
			if (info.processMode & modeDeskAccessory)
				{
				++count;
				if (1 < count)
					break;
				}
			}
		}

	return (1 < count);
	}

/* CleanupAndExit.  Remove any trace of our existence. */
void
CleanupAndExit(void)
	{	
	/* Restore zone (prudent) and remove our patches */
	THEZONE = APPLZONE;
	NSetTrapAddress((long)oldCloseAddress, (short) _Close, OSTrap);
	NSetTrapAddress((long)oldSystemEventAddress, (short) _SystemEvent, ToolTrap);
	NSetTrapAddress((long)oldExitToShellAddress, (short) _ExitToShell, ToolTrap);
	
	// <12> If there are no other desk accessories running, unload DAHandler.
	
	if (!ThereAreMoreDeskAccessories())
		UnloadSeg((Ptr) main);
		
	/* Tell OS to destroy us altogether */
	ExitToShell();
	}

/* GiveGoodByeKiss.  Give this driver a goodbye kiss if it wants one. */
GiveGoodByeKiss(DCtlPtr dCtlPtr)
	{
	CntrlParam			goodByeParamBlock;
	
	/* Give him a goodbye kiss, if necessary */
	if ((dCtlPtr->dCtlFlags & DNeedGoodbye) != 0)
		{
		goodByeParamBlock.csCode = csCodeGoodBye;
		goodByeParamBlock.ioCRefNum = dCtlPtr->dCtlRefNum;
		PBControl(&goodByeParamBlock, SyncHFS);					
		}
	}
	
/* MemClear.  Two-bit routine to clear 'bytes' bytes starting at *'ptr' */
void	
MemClear(register Ptr ptr, register unsigned long bytes)
	{
	do
		{
		*ptr++ = (char) 0x00;
		}
	while (--bytes != 0);
	
	}

/* ExitToShellPatch.  Our patch to ExitToShell.  Give the driver a fighting chance at
 * cleaning up
 * NOTE: The etsDepth counter is to keep us from additional flailing if we are
 * forced to ExitToShell even as we are closing the DA.
 */
void
ExitToShellPatch(void)
	{
	static short		etsDepth = 0;
	
	/* Try to do it nice.  Then just do it. */
	A5Restore(CURRENTA5);
	if (etsDepth++ == 0)
		CloseDACleanupAndExit(false);
	else
		CleanupAndExit();
	}

/* CloseDACleanupAndExit.  Patch for _ExitToShell.  Gives the DA a chance to
 * cancel (i.e. by leaving windows up).
 */
void
CloseDACleanupAndExit(Boolean beNice)
	{
	register DCtlPtr	dCtlPtr;
	register DCtlHandle	dCtlHdl;

	A5Restore(CURRENTA5);
	if (myDriverRefNum != 0)
		CloseDeskAcc(myDriverRefNum);
	
	/* Quit if the DA did */
	if ((beNice == false) || (FrontWindow() == nil))
		{
		if (myUnitNumber != 0)
			{			
			dCtlHdl = ((DCtlHandle) UNITTABLE[myUnitNumber]);
			if ((dCtlHdl != nil) && (dCtlPtr = *dCtlHdl) != nil)
				{
				if ((dCtlPtr->dCtlFlags & (DNeedGoodbye | DOpened)) == (DNeedGoodbye | DOpened))
					GiveGoodByeKiss(dCtlPtr);						
				MemClear(dCtlPtr, sizeof(DCtlEntry));
				dCtlPtr->dCtlRefNum = myDriverRefNum;
				}
			}
			
		CleanupAndExit();
		}
	}

typedef struct MP
	{
	struct MP					*pNextMP;
	} MP;

/* Determine whether the current zone has at least the given number
 * of free master pointers.
 */
Boolean
ThisManyMPs(unsigned short cMinAcceptableMPs)
	{
	MP							*pCurMP;
	
	for (pCurMP = (MP *)THEZONE->hFstFree; pCurMP != nil; pCurMP = pCurMP->pNextMP)
		if (--cMinAcceptableMPs == 0)
			return true;
			
	return false;
	}
	
#define							MIN_ACCEPTABLE_MPS		32

#define kFunctionKey	0x10
#define kUndo			0x7A
#define kCut			0x78
#define kCopy			0x63
#define kPaste			0x76

void
main(void)
	{
	EventRecord 			myEvent;
	WindowPtr				whichWindow;
	static Point			bmPt = {100,100};	/* ULH pt of Disk Err dialog */
#define MAXSLEEPTIME		(0xFFFFFFFF)		/* largest value of WaitNextEvent sleep */

	StartupCode();
	
	/* Force all into system heap */
	THEZONE = SYSZONE;
	
	/* Because we are about to use this heap heavily */
	if (ThisManyMPs(MIN_ACCEPTABLE_MPS) == false)
		MoreMasters();
	
	/* Get the sucker open */
	OpenTheDeskAccessory();

	// <12> Now that DAHandler is up and running, it’s Init and %A5Init segments can be
	//		unloaded.
	
	UnloadSeg((Ptr) StartupCode);			// <12> Unload Init
	UnloadSeg((Ptr) _DataInit);				// <12> Unload %A5Init
	
	/* The event loop */
	for (;;)
		{
		short				thereIsAnEvent;

		if (doneFlag)
			CloseDACleanupAndExit(true);

		/*
		 * Handle the next event.
		 */
		
		/* Anybody out there? */
		thereIsAnEvent = WaitNextEvent(everyEvent, &myEvent, MAXSLEEPTIME, nil);
		 
		/* NOTE:  This should be done after _WNE also, in case of DAs like Intermail
		 * that close themselves on the fly (in _SystemTask).  We do the processing
		 * after waking up, which is now.
		 */
		if (doneFlag)
			CloseDACleanupAndExit(true);

		/* check for timeout */
		if (!thereIsAnEvent || myEvent.what == nullEvent)
			continue;

		switch (myEvent.what)
			{
			case mouseDown:
				switch (FindWindow(myEvent.where, &whichWindow))
					{
					case inSysWindow:
						SystemClick(&myEvent, whichWindow);
						break;
					case inMenuBar:
						DoCommand(MenuSelect(myEvent.where));
						break;
					}
				break;
				
			case keyDown:
				if (myEvent.modifiers & cmdKey)
					DoCommand(MenuKey((char)myEvent.message));
				else
					{
					unsigned char key;
					unsigned char virtualKey;
					
					key = (char) myEvent.message;
					virtualKey = (myEvent.message & keyCodeMask) >> 8;
					
					if (key == kFunctionKey)
						{
						switch (virtualKey)
							{
							case kUndo:
								key = 'Z';
								break;
								
							case kCut:
								key = 'X';
								break;
								
							case kCopy:
								key = 'C';
								break;
								
							case kPaste:
								key = 'V';
								break;
								
							default:
								key = 0;
								break;
							}
							
						if (key)			// if we mapped it then menukey
							DoCommand(MenuKey(key));
						}
					}
				break;
				
			case diskEvt:
				if (*((short *)&myEvent.message) != noErr)
					(void)DIBadMount(bmPt, myEvent.message);
				break;
			
			case app4Evt:
				break;
				
			/* default:
			 * 		break;
			 */
			}
		}
	}	
	
/*
 * Initialization code:  unload this segment when done with it.
 */
#pragma segment Init
		
/* InitConfig.  Find out what sort of software/hardware setup we're dealing with.
 * NOTE: This is the place to make any costly determinations about our environment
 * for later time savings.  None such, yet.
 */
void
InitConfig(void)
	{
	long	qdInfo;

	Colorized = false;
	Has32BitQD = false;
	
	if (Gestalt(gestaltQuickdrawVersion,&qdInfo) == noErr)
		{
		qdInfo &= 0xFFFF;
		Colorized = (qdInfo >= gestalt8BitQD);
		Has32BitQD = (qdInfo >= gestalt32BitQD);
		}
	}
	
/* SetupMenus.  Set up the Apple, and Edit menus.
 * NOTE: If the MENU resources are missing, we die.
 */
void
SetupMenus(void)
	{
	extern MenuHandle		MyMenus[];
	register MenuHandle		*pMenu;
	Handle					prevTop;
	short					topFile;
	
	/* Set up the desk accessories menu.  The "About…" item, followed by a grey
	 * line, is presumed to be already in the resource.  We then append the desk
	 * accessory names from the 'DRVR' resources.  We copy the menu because
	 * each DA Handle needs its own version of the common menu resource.  HandToHand
	 * into the application heap means that dispose is automatic, even on crash.
	 * NOTE: We have to sidestep the DA file itself, since it has DRVR resource(s) and
	 * AddResMenu always uses the whole chain.
	 */
	MyMenus[appleMenu] = GetMenu(-appleID);
	HandToHand(&MyMenus[appleMenu]);
	
	topFile = CURMAP;
	prevTop = TOPMAPHANDLE;
	CURMAP = SYSMAP;
	TOPMAPHANDLE = GetOverrideMap(SYSMAPHANDLE);
	AddResMenu(MyMenus[appleMenu], (ResType) 'DRVR');
	TOPMAPHANDLE = prevTop;
	CURMAP = topFile;
	
	/*
	 * Now File, Edit menus.
	 */
	MyMenus[fileMenu] = GetMenu(-fileID);
	MyMenus[editMenu] = GetMenu(-editID);

	/*
	 * Now insert all of the application menus in the menu bar.
	 */
	for (pMenu = &MyMenus[0]; pMenu < &MyMenus[menuCount]; ++pMenu)
		InsertMenu(*pMenu, 0);

	DrawMenuBar();

	return;
	}
	
void
StartupCode(void)
	{
	/* Clear global flag that tells us when to quit */
	doneFlag = 0;

	/* Initialization traps, pretty much required */
	MaxApplZone();
	InitConfig();
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
	FlushEvents(everyEvent, 0);
	myDriverRefNum = 0;
	myUnitNumber = 0;

	SetupMenus();

	/* Patch _Close to check for closing desk accessories */
	oldCloseAddress = (Ptr)NGetTrapAddress((short) _Close, OSTrap);
	NSetTrapAddress((long)ClosePatch, (short) _Close, OSTrap);
	
	/* Patch _SystemEvent so we can handle cmd-Q and cmd-W */
	oldSystemEventAddress = (Ptr)NGetTrapAddress((short) _SystemEvent, ToolTrap);
	NSetTrapAddress((long)SystemEventPatch, (short) _SystemEvent, ToolTrap);
	
	/* Patch _ExitToShell to check for closing desk accessories, so that
	 * we can handle ES from MacsBug.
	 */
	oldExitToShellAddress = (Ptr)NGetTrapAddress((short) _ExitToShell, ToolTrap);
	NSetTrapAddress((long)ExitToShellPatch, (short) _ExitToShell, ToolTrap);		
	}

/* GetRsrcHdlMatchingFirstLongword.  Find the first resource of specified type that
 * has the given value in its first long word.  Scope is current file.
 */
Handle
GetRsrcHdlMatchingFirstLongword(long resType, long firstLong)
	{
	Handle		hdl;
	short		resIndex, resLast;
	
	resLast = Count1Resources(resType);
	for (resIndex = 1; resIndex <= resLast; resIndex++)
		{
		hdl = Get1IndResource(resType, resIndex);
		if (hdl != nil)
			{
			if (**((long **) hdl) == firstLong)
				return (hdl);
			ReleaseResource(hdl);
			}
		}
		
	return (nil);
	}	

/* GetIconIDFromBundle.  Figure out the icon family resource ID for the given type and
 * creator.  Starts at the current resource map.  Uses the same search path that Finder
 * does (FREF -> BNDL -> icon resource).
 */
short
GetIconIDFromBundle(OSType theType, unsigned long theCreator)
	{
	Ptr					pRsrc;
	short				typeIndex, icnIndex;
	Handle				rsrcHdl;
	short				localID, globalID;
		
	/* Find the FREF of the type */
	rsrcHdl = GetRsrcHdlMatchingFirstLongword('FREF', theType);
	if (rsrcHdl == nil)
		return(0);

	pRsrc = *rsrcHdl + 4;
	localID = *((short *) pRsrc);
	ReleaseResource(rsrcHdl);
	
	/* Get the BNDL */
	rsrcHdl = GetRsrcHdlMatchingFirstLongword('BNDL', theCreator);
	if (rsrcHdl == nil)
		return(0);
		
	pRsrc = *rsrcHdl + 6;
	
	/* Major loop -- find global id of icon in bundle */
	globalID = 0;
	for (typeIndex = *((short *) pRsrc)++; typeIndex >= 0; typeIndex--)
		{
		/* Is this type ICN# ? */
		if (*((long *) pRsrc)++ == 'ICN#')
			{
			/* Minor loop - find a match for our given local ID and get it */
			for (icnIndex = *((short *) pRsrc)++; icnIndex >= 0; icnIndex--)
				{
				if (*((short *) pRsrc)++ == localID)
					{
					/* Found it! */
					globalID = *((short *) pRsrc);
					break;
					}
					
				pRsrc += 2;
				}
			
			/* Either we found it or we looked at all icons */
			break;
			}
		else
			{
			/* Skip over all of this type's resources, carefully because count
			 * is zero-based.
			 */
			((long *) pRsrc) += *((short *) pRsrc)++;
			((long *) pRsrc)++;
			}
		}
	
	/* Clean up and go */
	ReleaseResource(rsrcHdl);
	return(globalID);
	}

/* GetDisplayIconID.  Return the resource ID of the DA's icon family. */
short
GetDisplayIconID(void)
	{
	ProcessInfoRec	procInfo;
	short			retval;
	FInfo			finderInfo;
	FSSpec			fileSpec;

	/* Nothing special if in the system file or an override map */
	if (IsThisASystemResourceMap(TOPMAPHANDLE))
		return(genericDeskAccessoryIconResource);

	/* Get process info for search */
	procInfo.processInfoLength = sizeof(ProcessInfoRec);
	procInfo.processName = nil;
	procInfo.processAppSpec = &fileSpec;
	SetPSN(kCurrentProcess, &(procInfo.processNumber));
	if (GetProcessInformation(&procInfo.processNumber, &procInfo) != noErr)
		return(genericDeskAccessoryIconResource);

	/* Try for custom icon first.  If not custom, use our type and signature in
	 * BNDL lookup.
	 */
	if ((FSpGetFInfo(&fileSpec, &finderInfo) == noErr) && ((finderInfo.fdFlags & FILE_FLAG_HAS_CUSTOM_ICON) != 0))
		retval = kCustomIconID;
	else if ((retval = GetIconIDFromBundle(procInfo.processType, procInfo.processSignature)) == 0)
		retval = genericDeskAccessoryIconResource;

	return(retval);
	}

/* ID of owned resources for the "about me" dialog */
#define OWN_ABOUT_ME_ID			(1)
#define OWN_DLOG_PROTO			(OWNED_BY_DRVR | OWN_ABOUT_ME_ID)

#define STANDARD_DLOG_WIDTH		(260)

/* InitAboutMeDialog.  Figure out the strings and offsets we'll need to set up the
 * "about me" dialog.  The dialog has (among other things) two static text items
 * positioned one atop the other.  The texts are supplied by resources, and both
 * are optional.  If just one is missing, the other goes in the top position.  Sets
 * the hFirstAboutMeString, hSecondAboutMeString, wdShrinkOfAboutMe, htOfAboutMeStrings,
 * htOfFirstAboutString, and htOfSecondAboutString.
 */
void
InitAboutMeDialog(void)
	{
	DialogRecord	dialogRecord;
	
	THEZONE = APPLZONE;

	/* Set defaults in case we don't want or can't get everything */
	wdShrinkOfAboutMe = 0;
	htOfAboutMeStrings = 0;
	htOfFirstAboutString = 0;
	htOfSecondAboutString = 0;
	hFirstAboutMeString = nil;
	hSecondAboutMeString = nil;
	
	/* Determine resource ID of display icon.  Use it to locate the info strings. */
	if ((myDisplayIconID = GetDisplayIconID()) != genericDeskAccessoryIconResource)
		hFirstAboutMeString = Get1Resource('dast', myDisplayIconID);
		
	/* If DA is from the system file, get the informative string */
	if ((hSecondAboutMeString = (IsSystemRefNum(myFileRefNum)) ? GetResource('STR ', StrID_InSysFile) : nil) != nil)
		{
		DetachResource(hSecondAboutMeString);
		HNoPurge(hSecondAboutMeString);
		}
	
	/* Determine the lineup */
	if (hFirstAboutMeString == nil)
		{
		hFirstAboutMeString = hSecondAboutMeString;
		hSecondAboutMeString = nil;
		}
	else	
		HandToHand(&hFirstAboutMeString);		

	if (GetNewDialog(aboutMeDLOG, &dialogRecord, (WindowPtr) -1) != nil)
		{
		if (hFirstAboutMeString != nil)
			{
			short					itemType;
			Handle					itemHdl;
			Rect					itemRect1, itemRect2;
			short					lineHeight;
			register StringHandle	hStr;
			GrafPtr					savePort;
			
			GetPort(&savePort);
			SetPort(&dialogRecord);
			GetDItem(&dialogRecord, secondAboutString, &itemType, &itemHdl, &itemRect2);
			GetDItem(&dialogRecord, firstAboutString, &itemType, &itemHdl, &itemRect1);
			itemRect1.bottom = itemRect1.top+100;
			SetDItem(&dialogRecord, firstAboutString, editText, itemHdl, &itemRect1);
			lineHeight = (*dialogRecord.textH)->lineHeight;
		
			hStr = hFirstAboutMeString;
			do
				{
				short		numLines;
				
				HLock(hStr);
				SetIText(itemHdl, *hStr);
				HUnlock(hStr);
				numLines = (*dialogRecord.textH)->nLines;
				if (hStr != hFirstAboutMeString)
					{
					htOfSecondAboutString = numLines * lineHeight;
					htOfAboutMeStrings += (itemRect2.top - itemRect1.top) + htOfSecondAboutString;	
					break;
					}
				htOfAboutMeStrings = htOfFirstAboutString = numLines * lineHeight;
				hStr = hSecondAboutMeString;
				} while (hStr != nil);
		
			SetPort(savePort);
			}
		wdShrinkOfAboutMe = (htOfAboutMeStrings != 0) ? 0 : dialogRecord.window.port.portRect.right - dialogRecord.window.port.portRect.left - STANDARD_DLOG_WIDTH;
		CloseDialog(&dialogRecord);
		}
	else
		{
		hFirstAboutMeString = nil;
		hSecondAboutMeString = nil;
		}
	
	THEZONE = SYSZONE;
	}

/* PutUpDeathAlert.  Put up an alert, if there's enough memory.  Try to get the memory
 * out of the DA Handler app heap, which should be unaffected by the ravages of what is
 * now a very small sys heap.
 */
void	
PutUpDeathAlert(OSErr err)
	{
	Handle			slopHdl;
	short			alertID;

	THEZONE = APPLZONE;
	alertID = (err == memFullErr) ? AlertID_OutOfMem : AlertID_NotFound;	
	if (GetResource('ALRT', alertID) == nil
		|| GetResource('DITL', alertID) == nil
		|| (slopHdl = NewHandle(ALERT_SLOP_SIZE)) == nil)
			{
			SysBeep(20);
			return;
			}
		
	DisposHandle(slopHdl);
	ParamText(CURAPNAME, nil, nil, nil);
	(void)StopAlert(alertID, nil);
	THEZONE = SYSZONE;
	}

typedef pascal unsigned long (*GzProcPtr)(unsigned long cbNeeded);

/* Without doing actual purging, guarantee there are cbNeeded free or purgeable bytes
 * in THEZONE, including attempts to grow the heap.
 */ 
Boolean
CanGetThisMuchFreeOrPurgeSpace(long cbNeeded)
	{
	GzProcPtr			gzProcPtr = SYSZONE->gzProc;
	
	for (;;)
		{
		/* Quit now if enough contiguous space already exists */
		if (SysContigPurgeSpace() >= cbNeeded)
			return true;

		/* See how much the growzone procedure buys us */
		if (gzProcPtr == nil || (*gzProcPtr)(cbNeeded) == 0)
			return false;
		}
	}

/* IsDrvrOpen.  Return boolean whether our driver is open.  Assumes myUnitNumber. */
Boolean
IsDrvrOpen(void)
	{
	DCtlEntry		**ppDCtlEntry;
	
	ppDCtlEntry = ((DCtlEntry ***)UNITTABLE)[myUnitNumber];
	return (ppDCtlEntry != nil && (((*ppDCtlEntry)->dCtlFlags & DOpened) != 0));
	}	


#define					DA_SLOP_SIZE 			0x2000	/* Min memory a DA need to operate in (a guess) */
#define					TOOLBOX_SLOP_BASE		0x5000	/* Always require at least this much */
#define					TOOLBOX_SLOP_32QD	 	0x4000	/* Add this much if toolbox has 32-bit QuickDraw */
#define					TOOLBOX_SLOP_COLOR	 	0x5000	/* Add this much if toolbox has non-32-bit color */

/* EstimatedToolboxHeap.  Because the DA will be calling toolbox routines using the
 * system heap, if the toolbox takes too much memory, other DAs and/or applications
 * will die.  We therefore preflight the system heap.  The required amount is hard to
 * predict, but is strongly related to the toolbox capabalitities.  This routine makes
 * its estimate by starting at a value for the fundamental toolbox, then adding a weight
 * representative of each capability we have found to be a memory hog.
 *   For now, it is felt that color quickdraw is the only thing that greatly
 * contributes.  As more hogs are born, check for them here and add an appropriate
 * amount if they exist.
 */
long		
EstimatedToolboxHeap(void)
	{
	long	requirement;
	
	requirement = TOOLBOX_SLOP_BASE;
	
	/* Color always requires more, but 32-bit QuickDraw is lesser of two evils */
	if (Has32BitQD)
		requirement += TOOLBOX_SLOP_32QD;
	else
		{
		if (Colorized)
			requirement += TOOLBOX_SLOP_COLOR;
		}
		
	return requirement;
	}

/* PreflightDA.  Determine if there is enough room for a DA in the system heap. */
OSErr
PreflightDA(void)
	{
	Handle			drvrHdl;
	Handle			slopHdl;
	long			flags;
	Boolean			gotSpace;

	/* Find the resource.  Return error if not found, or not enough room for it. */
	if ((drvrHdl = GetNamedResource('DRVR',CURAPNAME)) == nil)
		return RESERR;
	
	/* Take a whack at one of our globals */
	myFileRefNum = HomeResFile(drvrHdl);
	
	/* Found the resource, so determine unit table index from resource ID.
	 * NOTE:  Assumes the best even if we couldn't get resource info.
	 */
	ROMMAPINSERT = -1;
	GetResInfo(drvrHdl, &myUnitNumber, nil, nil);
	if (RESERR != noErr)
		return noErr;
	
	/* Base the preliminary settings of the unit number and refnum globals on the
	 * resource ID returned by GetResInfo.  These will get reset after OpenDeskAcc.  We
	 * need them now so that we can handle an ExitToShell from inside OpenDeskAcc.  If
	 * the OpenDeskAcc got far enough, the driver will be open and need to be closed.
	 */
	myDriverRefNum = (-(myUnitNumber + 1));

	/* No space problem if the driver is already open */
	if (IsDrvrOpen())
		return(noErr);
	
	/* Error if there wasn't space for driver. */
	if (*drvrHdl == nil)
		return(memFullErr);
	
	/* Make sure there's enough room for everything else, too. But don't let the
	 * driver get purged by the check.
	 */
	gotSpace = false;
	flags = HGetState(drvrHdl);
	HNoPurge(drvrHdl);
	if ((slopHdl = NewHandle(DA_SLOP_SIZE)) != nil)
		{
		gotSpace = CanGetThisMuchFreeOrPurgeSpace(EstimatedToolboxHeap());
		DisposHandle(slopHdl);
		}
	HSetState(drvrHdl, flags);
	
	return (gotSpace ? noErr : memFullErr);
	}
		
/* OpenTheDeskAccessory.  Routine to call when we're ready to open the DA. */
void
OpenTheDeskAccessory(void)
	{
	GrafPtr				savePort;
	short				odaResult;
	MenuHandle			appleMenuHdl;
	StringHandle		tempStrHdl;
	unsigned char 		ParamZeroStr[3] = {2, '^','0'};
	StringPtr			ptr1, ptr2;
	unsigned long		len1, len2;
	OSErr				err;
	Str255				strItemName;
	
	/* Go to the top of the chain */
	CURMAP = TopResFile();

	/* Cancel operation if there is not enough memory to work with. */
	if ((err = PreflightDA()) != noErr)
		{
		PutUpDeathAlert(err);
		doneFlag = true;
		return;
		}
	
	/* Put DA name in the place of honor */
	appleMenuHdl = MyMenus[appleMenu];
	GetItem(appleMenuHdl, aboutMeCommand, &strItemName);
	if ((tempStrHdl = NewString(strItemName)) != nil)
		{
		ptr1 = (StringPtr) &ParamZeroStr;
		len1 = (unsigned long) Length(ptr1);
		ptr1 = &StringByte(ptr1, 0);
		ptr2 = &StringByte(CURAPNAME, 1);
		len2 = (unsigned long) Length(CURAPNAME) - 1;
		(void) Munger(tempStrHdl, 0, ptr1, len1, ptr2, len2);
		Length(*tempStrHdl) += len2 - len1;
		if (MEMERROR == noErr)
			{
			HLock(tempStrHdl);
			SetItem(appleMenuHdl, aboutMeCommand, *tempStrHdl);
			HUnlock(tempStrHdl);
			}
		DisposHandle(tempStrHdl);
		}	
		
	/* Do actual open */	
	GetPort(&savePort);
	odaResult = OpenDeskAcc(CURAPNAME);
	SetPort(savePort);
	
	/* Check whether open failed. */
	if (isNotDARefNum(odaResult))
		{
		doneFlag = true;
		return;
		}

	/* Base the permanent settings of the unit number and refnum globals on the
	 * OpenDeskAcc function result.
	 */
	myDriverRefNum = odaResult;
	myUnitNumber = (-(myDriverRefNum + 1));

	/* Check whether DA completed already. */
	if (IsDrvrOpen() == false)
		{
		doneFlag = true;
		return;
		}
	
	/* Set up the about box dialog. */
	InitAboutMeDialog();	
	}
		
#pragma segment Main

pascal void
MyItemProc(WindowPtr theWindow, short itemNum)
	{
	short 		typ;
	Handle		hdl;
	Rect		theRect;
	GrafPtr 	savePort;

	/* Frame the OK button */
	GetDItem(theWindow, okButton, &typ, &hdl, &theRect);
	PenSize(3, 3);
	InsetRect(&theRect, -4, -4);
	FrameRoundRect(&theRect, 16, 16);
	
	/* Plot the icon */
	GetPort(&savePort);
	SetPort(theWindow);
	GetDItem(theWindow, itemNum, &typ, &hdl, &theRect);
	PlotIconID(&theRect, atAbsoluteCenter, ttNone, myDisplayIconID);
	SetPort(savePort);
	}

/* showAboutMeDialog.  Display the DAHandler Application dialog. */
void
showAboutMeDialog(void)
	{
	GrafPtr 		savePort;
	DialogRecord	dialogRecord;
	Handle			hDLOG;
	short			itemType;
	Handle			itemHdl;
	Rect			itemRect;
	short			itemHit;
	
	THEZONE = APPLZONE;

	GetPort(&savePort);
	
	/* Pre-adjust dialog size for optional parts */
	{
	Rect		*pRect;
	
	if ((hDLOG = GetResource('DLOG', aboutMeDLOG)) == nil)
		return;
	HNoPurge(hDLOG);
	pRect = (Rect *) *hDLOG;
	pRect->right -= wdShrinkOfAboutMe;
	pRect->bottom += htOfAboutMeStrings;
	}
		
	/* Allocate a dialog record */
	if (GetNewDialog(aboutMeDLOG, &dialogRecord, (WindowPtr) -1) == nil)
		return;

	/* Release the resource so that resizing change will not be remembered */
	ReleaseResource(hDLOG);

	/* Display dialog if there's still enough room */
	if ((hDLOG = NewHandle(ALERT_SLOP_SIZE)) != nil)
		{
		short			pixelsNeeded;

		DisposHandle(hDLOG);

		SetPort(&dialogRecord);
		AutoPositionWindow((WindowPtr) &dialogRecord, lcMainScreen, hcCenter, vcAlertCenter);
		
		/* Set the user item proc address into the icon item */
		GetDItem(&dialogRecord, daIcon, &itemType, &itemHdl, &itemRect);
		SetDItem(&dialogRecord, daIcon, itemType, (Handle) &MyItemProc, &itemRect);

		/* Set the DA name */
		GetDItem(&dialogRecord, daNameItem, &itemType, &itemHdl, &itemRect);
		SetIText(itemHdl, (StringPtr)CURAPNAME);

		/* Adjust the first text item length */
		if ((pixelsNeeded = htOfAboutMeStrings) != 0)
			{
			GetDItem(&dialogRecord, firstAboutString, &itemType, &itemHdl, &itemRect);
			itemRect.bottom = itemRect.top + htOfFirstAboutString;
			SetDItem(&dialogRecord, firstAboutString, itemType, itemHdl, &itemRect);
			HLock(hFirstAboutMeString);
			SetIText(itemHdl, *hFirstAboutMeString);
			HUnlock(hFirstAboutMeString);

			/* Adjust the second text item vertical position and length */
			if (hSecondAboutMeString != nil)
				{
				GetDItem(&dialogRecord, secondAboutString, &itemType, &itemHdl, &itemRect);
				itemRect.top += htOfFirstAboutString;
				itemRect.bottom = itemRect.top + htOfSecondAboutString;
				SetDItem(&dialogRecord, secondAboutString, itemType, itemHdl, &itemRect);
				HLock(hSecondAboutMeString);
				SetIText(itemHdl, *hSecondAboutMeString);
				HUnlock(hSecondAboutMeString);
				}
			}
			
		/* Adjust the position of the OK button */
		GetDItem(&dialogRecord, okButton, &itemType, &itemHdl, &itemRect);
		itemRect.bottom += pixelsNeeded;
		itemRect.left -= wdShrinkOfAboutMe;
		itemRect.top += pixelsNeeded;
		itemRect.right -= wdShrinkOfAboutMe;
		MoveControl(itemHdl, itemRect.left, itemRect.top);
		SetDItem(&dialogRecord, okButton, itemType, itemHdl, &itemRect);

		/* Get this show on the road!
		 * NOTE: Must switch back to system heap since ModalDialog and/or WDEF may load
		 * resources or allocate memory.
		 */
		ShowWindow(&dialogRecord);
		THEZONE = SYSZONE;
		ModalDialog(nil, &itemHit);
		THEZONE = APPLZONE;
		
		SetPort(savePort);
		}

	CloseDialog(&dialogRecord);
	if (dialogRecord.items != nil)
		DisposHandle(dialogRecord.items);

	/* Back to the DA zone */
	THEZONE = SYSZONE;
	return;
	}

/*
 * Process mouse clicks in menu bar
 */
void
DoCommand(long mResult)
	{
	extern MenuHandle	MyMenus[];
	short 				theMenu, theItem;
	Str255				daName;

	theItem = LOWORD(mResult);
	theMenu = HIWORD(mResult);		/* This is the resource ID */

	switch (theMenu)
		{
		case appleID:
			if (theItem == aboutMeCommand)
				showAboutMeDialog();
			else
				{
				GrafPtr		savePort;
				
				GetItem(MyMenus[appleMenu], theItem, &daName);
				GetPort(&savePort);
				(void) OpenDeskAcc(&daName);
				SetPort(savePort);
				}
			break;

		case fileID:
			switch (theItem)
				{
				case closeCommand:
					CloseFrontDeskWindow();
					break;
				
				case quitCommand:
					CloseDACleanupAndExit(true);
					break;
				}
			break;
			
		case editID:
			/*
			 * If this is for a 'standard' edit item,
			 * run it through SystemEdit first.
			 * SystemEdit will return FALSE if it's not a system window.
			 */
			SystemEdit(theItem-1);
		}

	HiliteMenu(0);
	}

/* HandleCloseDA.  Routine to be called from our Close patch.  Shuts down the
 * DA if the DA is asking to close its driver code.
 */
void
HandleCloseDA(short daRefNum)
	{
	ProcessSerialNumber		myPSN;
		
	if (daRefNum == myDriverRefNum)
		{
		SetPSN(kCurrentProcess, &myPSN);
		(void) WakeUpProcess(&myPSN);
		doneFlag = true;
		}
	}
