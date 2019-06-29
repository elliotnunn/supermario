/*
	File:		NotificationMgr.c

	Contains:	Notification Manager

	Written by:	Ed Tecot

	Copyright:	© 1987-1994 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM6>	  1/8/94	kc		Initialize SndChannelPtr before calling SndNewChannel in
									DoSound. This fixes bug found in MacWorld Compatability Lab.
	   <SM5>	12/21/93	SAM		Changed DoSound to manually get a new sound channel and set its
									volume to the "alert" volume level before calling SndPlay.
	   <SM4>	 5/18/93	RC		perfomance/bug fix (seed up WaitNextEvent by 20%)
	   <SM3>	 12/3/92	RB		Renamed Call to avoid a conflict.
		<14>	 5/29/92	DCL		Included TextUtils.p. EqualString was moved for the New Inside
									Mac.
		<13>	10/28/91	SAM/KSM	Rolled in Regatta changes.
	
	Regatta Change History:

		 <3>	 6/14/91	SAM		(HJR) Added a call to IdleUpdate() on _NMInstall calls to wake
									the the system up when NM procs want to run.
		 <1>	 5/22/91	SAM		Split off from 7.0 GM sources.

	7.0 Change History:

		<12>	  4/3/91	dba		kill all the fun
		<11>	  3/4/91	dba		dty: get rid of SysVers conditional
		<10>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
		 <9>	10/10/90	JL		Add Errors.h to Include List for nmTypErr
		 <8>	 9/21/90	VL		Juggled the fields in QHdr because we are patching some routines
									in ci and portable ROMs. Preserving the order of the fields in
									the original record allows us not to patch all of the routines.
		 <7>	 8/18/90	dba		get rid of code that does dialog centering for 7.0, since the
									Window Mgr. will do it for us
		 <6>	 7/17/90	dba		use Memory.h all the time (MemUtils.h is obsolete); get rid of C
									warning from unused parameter
		 <5>	 7/16/90	VL		See "Changes for 7.0".
		 <4>	 7/10/90	dba		add prototypes to avoid warnings
		 <3>	  7/7/90	dba		use standard Memory.h and OSUtils.h for system disk version
		 <2>	 3/20/90	EMT		Rewrote HaveCQD() macro to generate less code. Removed
									RealMBarHeight stuff - MBarHeight is sufficient.
	   <1.2>	11/17/89	dba		got rid of check for 64K ROMs
	   <1.1>	 11/9/88	CCH		Fixed file header.
	   <1.0>	 11/9/88	CCH		Adding to EASE.
	   <1.7>	 11/1/88	GGD		Added even more type coercions so that it compiles under MPW
									3.0b1.
	   <1.6>	10/10/88	MSH		Added type coercions so that it compiles under MPW 3.0b1.
	   <1.5>	 9/16/88	cv		Added type coersion so that it compiles with MPW3.0a2
	   <1.4>	  8/4/88	BBM		Changed FLUSHAPPLNM to uppercase for numac build
	   <1.3>	 7/13/88	EMT		Rolled in S474: Fixed bug so response procedure gets called.
	   <1.2>	 5/23/88	BBM		Changed NMInit to uppercase.
	   <1.1>	 5/19/88	BBM		Converted to MPW C 3.0.
	   <1.0>	 3/22/88	EMT		First time adding Notification Manager sources

*/

/*********************************************************************************************
*
* Changes for 7.0
*
* User-interface changes:
* 1) If a notification is coming from a process which is listed under the Application Menu,
*	 the blinking icon is put above the Application Menu. Otherwise, the blinking icon will be
*	 put above the Apple Menu. The latter case applies to DAs and system-level tasks.
* 2) The marks (or the diamonds) are put on the left of the menu items under the Application Menu.
*	 No mark will be put in the Apple Menu.
* 3) The NM can now blink color icon. The handle to the color icon suite is passed in the
*	 nmIcon field (previously nmSIcon field). The NM distinguishes the 2 kinds of icons by
*	 the size of the handle in nmIcon.
*
* Engineering changes:
* 1) In order to deal with blinking icons on both the Apple Menu and the Application Menu, an
*	 extra queue is added to NMHdr. amIcons and pmIcons will contain IconRec's for the Apple
*	 Menu and the Application Menu respectively. curAMIconRec and curPMIconRec contain the current
*	 icons for the Apple Menu and Application Menu.
* 2) An extra boolean is added to NMHdr. It shows whether NM should put up a new icon on the
*	 Apple Menu or the Application Menu. The icons above the 2 menus blink alternately.
* 3) In order to deal with color icon, a new Boolean iconSuite is added to IconRec so that
*	 the NM will know which kind of icon to plot.
* 4) GetApplicationMenu now gets the Application Menu for the 7.0 system while it gets the
*	 Apple menu in the 6.0 system.
* 5) New MBDF messages are added to deal with drawing above the Application Menu and color icon
*	 suites.
* 6) WaitSThresh is no longer a constant. It becomes a macro and returns the long or short
*	 wait time according to the state of the icon queues. The long wait time is used when
*	 icons are only blinking on one side while the short wait time is used when icons are
*	 blinking on both sides.
*
********************************************************************************************/

#include <Types.h>
#include <Errors.h>
#include <Memory.h>
#include <OSUtils.h>
#include <TextUtils.h>
#include <Devices.h>
#include <Events.h>
#include <Resources.h>
#include <Controls.h>
#include <Dialogs.h>
#include <Menus.h>
#include <Sound.h>
#include <IconUtilsPriv.h>
#include <MenuMgrPriv.h>
#include <MFPrivate.h>
#include <Power.h>
#include "kernel.h"
#include "task.h"
#include "Notification.h"

int disable();
void spl(int level);

#define	MoveSDist	0		/* Argument to CallMBarProc message */
#define	MoveSThresh	1		/* Number of ticks between moves */

#define	AlarmSICN	0xBFFF	/* ID of the alarm clock SICN */
#define	AppleMark	0x0114	/* Pascal string containing apple character */

#define	SICNsize	32		/* Size of (single) SICN */

typedef struct {
	MenuHandle	menuOH;
	short		menuLeft;
} MenuRec;

typedef struct {
	short	lastMenu, lastRight, mbResID;
	MenuRec	menu[0];
} MenuL, *MenuLPtr, **MenuLHdl;

#define	MapTRUE		-1

#define	BufPtr			(*(Ptr *) 0x10C)
#define	UnitTable		(*(DCtlHandle **) 0x11C)
#define	Ticks			(*(long *) 0x16A)
#define	AlarmState		(*(char *) 0x21F)
#define	ROM85			(*(unsigned short *) 0x28E)
#define	SysZone			(*(THz *) 0x2A6)
#define	ApplZone		(*(THz *) 0x2AA)
#define WMgrPort		(*(GrafPtr *) 0x9DE)
#define MenuList		(*(MenuLHdl *) 0xA1C)
#define	ROMMapInsert	(*(short *) 0xB9E)
#define	MBarHeight		(*(short *) 0xBAA)
#define WMgrCPort		(*(GrafPtr *) 0xD2C)

#define	HaveCQD()		(ROM85 <= 0x3FFF)

/* True iff alarm has gone off */
#define	Alarm()		(!(AlarmState & 0x3))

/* MBDF Messages */
#define MoveICSToPM		16
#define MoveICSToAM		17
#define	MoveSICNToPM	18
#define MoveSICNToAM	13

/* Private NMRec nmFlags */
#define	fItemMarked		0x0001
#define	fIconinRot		0x0002
#define	fSndPlayed		0x0008
#define	fDlogDisp		0x0010
#define	fDlogCompl		0x0020
#define	fRespCompl		0x0040
#define	fReqLocked		0x0080
#define	fReqDone		0x8000

/* All auxiliary records start with this */
typedef struct AuxRec {
	QElemPtr		qLink;
	short			qType;
	short			aRefCount;
	struct AuxRec	*aLink;
} AuxRec, *AuxRecPtr;

#define	nmMarkChar	0x13

/* Auxiliary record for menu item marks */
typedef struct MarkRec {
	AuxRec		hdr;
	short		mPID;
	short		mMarkID;
} MarkRec, *MarkRecPtr;

/* Auxiliary record for icons */
typedef struct IconRec {
	AuxRec		hdr;
	Handle		iSIcon;
	Boolean		iconSuite;
} IconRec, *IconRecPtr;

/* Notification Manager's globals */
typedef struct NMHdr {
	QHdr		requests, marks, amIcons;	/* amIcons was icons */
	NMRecPtr	alarmRec;
	MenuHandle	lastMenuUsed;				/* was lastAppleMenu */
	IconRecPtr	curAMIconRec;				/* was curIconRec */
	Handle		copyIcon;
	long		lastMoveTime;
	Boolean		useNewIcon;
	/* the following fields were added after the Portable and IIci ROMs */
	IconRecPtr	curPMIconRec;
	QHdr		pmIcons;
	Boolean		appleMenu;
} NMHdr, *NMHdrPtr;

#define	NMQHdr	(*(NMHdrPtr *) 0xB60)

/* Private NMHdr qFlags */
#define	fInDoDlog		0x0001
#define	fDlogInval		0x0002

#define		NMDialog	1
#define		buttonItem	1
#define		defButItem	2
#define		textItem	3
#define		iconItem	4

#define	EmptyIconQueue(iconQueue) (((IconRecPtr) iconQueue.qHead)->hdr.qLink == nil)
#define WaitSThresh(amIconQueue, pmIconQueue) ((!EmptyIconQueue(amIconQueue) && !EmptyIconQueue(pmIconQueue)) ? 30 : 60)

/* The MPW GetDCtlEntry has a bug.  This is my substitute.  It returns a handle
 * to the DCE of the driver having the given refnum.
 */
#define	MyGetDCtlEntry(refNum)	UnitTable[~refNum]

/* Returns true iff theAddr is in the range specified by low & high.  Uses StripAddress. */
Boolean
AddrInRange(Ptr theAddr, Ptr low, Ptr high)
{
	theAddr = StripAddress(theAddr);
	return (StripAddress(low) <= theAddr) && (theAddr <= StripAddress(high));
}
#define	InSysZone(theAddr)	AddrInRange(theAddr, (Ptr) SysZone, SysZone->bkLim)
#define	InApplZone(theAddr)	AddrInRange(theAddr, (Ptr) ApplZone, BufPtr)


/* Gets a process ID, or returns -1 if none is available (MultiFinder not active) */
short
GetPID(void)
{
	if (NGetTrapAddress(0x8F, ToolTrap) != NGetTrapAddress(0x9F, ToolTrap)) {
		/* MultiFinder active */
		return TWGetPID();
	}
	else return -1;
}

/* Given a menu handle and a string, finds the item number, if any.  Returns
 * zero if it is not in the menu.
 */
short
FindItem(MenuHandle theMenu, StringPtr itemString)
{
	register short	i = CountMItems(theMenu);
	Str255			menuString;

	while (i > 0) {
		GetItem(theMenu, i, menuString);
		if (EqualString(itemString, menuString, false, true)) return i;
		i--;
	}
	return 0;
}

MenuHandle
GetApplicationMenu(void)
{
	return ((MenuHandle) GetResource('MENU',kApplicationMenuID));
}

short InApplicationMenu(short thePID, short theMarkID)
{
	register MenuHandle	theApplicationMenu = GetApplicationMenu();
	register DCtlHandle	hDCE = (theMarkID < 0) ? MyGetDCtlEntry(theMarkID) : nil;
	short	theItem = 0;

	if (theApplicationMenu) {
		if (thePID != -1) {
			/* MultiFinder is here */
			if (hDCE && !InSysZone((*hDCE)->dCtlDriver) && (thePID != GetPID())) {
				/* The driver is not in the system heap and the PID is not mine.
				 * Therefore, the driver is not to be found, flag the app instead.
				 */
				hDCE = nil;
			}
			if (!hDCE) {
				/* Mark the application */
				struct procinfo	procInfoRec;

				TWGetProcInfo(thePID, &procInfoRec);
				theItem = FindItem(theApplicationMenu, procInfoRec.p_name);
			}
		}

		if (hDCE) {
			/* It's a driver */
			short			dID;
			ResType			dType;
			Str255			dName;

			ROMMapInsert = MapTRUE;
			GetResInfo(GetResource('DRVR', ~theMarkID), &dID, &dType, dName);
			theItem = FindItem(theApplicationMenu, dName);
		}
	}
	return theItem;
}

/* Gets the free auxiliary record, if available */
AuxRecPtr
GetFreeAux(QHdrPtr theQueue)
{
	register AuxRecPtr	theAuxRec = theQueue->qTail;

	if (theAuxRec && !theAuxRec->qType) return theAuxRec;
	else return nil;
}

/* Ensures that a free auxiliary record is available */
void
AddAuxRec(Size theSize, QHdrPtr theQueue)
{
	if (!GetFreeAux(theQueue)) {
		register AuxRecPtr	theAuxRec = NewPtrSysClear(theSize);

		Enqueue((QElemPtr)theAuxRec, theQueue);
	}
}

#define	AddMarkRec()	AddAuxRec(sizeof(MarkRec), &NMQHdr->marks)
void
AddIconRec(void)
{
	AddAuxRec(sizeof(IconRec), &NMQHdr->amIcons);
	AddAuxRec(sizeof(IconRec), &NMQHdr->pmIcons);
}

/* Links the auxiliary record to theNMRec */
void
LinkAuxRec(NMRecPtr theNMRec, AuxRecPtr theAuxRec)
{
	theAuxRec->qType = -1;
	theAuxRec->aRefCount++;
	theAuxRec->aLink = (AuxRecPtr) theNMRec->nmPrivate;
	(AuxRecPtr) theNMRec->nmPrivate = theAuxRec;
}

/* Removes the auxiliary record links from theNMRec */
void
UnLinkAuxRecs(NMRecPtr theNMRec)
{
	register AuxRecPtr	theAuxRec = (AuxRecPtr) theNMRec->nmPrivate;

	(AuxRecPtr) theNMRec->nmPrivate = nil;
	while (theAuxRec) {
		theAuxRec->aRefCount--;
		theAuxRec = theAuxRec->aLink;
	}
}

/* Inserts the MarkRec associated with theNMRec in the queue.
 * Note that since MarkRecs have multiple references, it is important that
 * these be the first auxiliary records in. (aLink should always be nil)
 */
MarkRecPtr
InsertMarkRec(NMRecPtr theNMRec)
{
	register MarkRecPtr	theMarkRec = NMQHdr->marks.qHead;
	register short		thePID = theNMRec->nmReserved;
	register short		theMarkID = theNMRec->nmMark;

	while (theMarkRec &&
			((theMarkID != theMarkRec->mMarkID) || (thePID != theMarkRec->mPID)) &&
			theMarkRec->hdr.qType) theMarkRec = (MarkRec *) theMarkRec->hdr.qLink;	/* 1.5 type coersion */

	if (!theMarkRec) theMarkRec = (MarkRec *) GetFreeAux(&NMQHdr->marks);			/* 1.5 type coersion */

	if (theMarkRec) {
		theMarkRec->mPID = thePID;
		theMarkRec->mMarkID = theMarkID;
		LinkAuxRec(theNMRec, (AuxRecPtr) theMarkRec);
	}

	return theMarkRec;
}

/* Inserts the IconRec associated with theNMRec in the queue. */
IconRecPtr
InsertIconRec(NMRecPtr theNMRec)
{
	register IconRecPtr theIconRec = nil;

	if ((GetHandleSize(theNMRec->nmIcon) == sizeof(IconSuite)) ||
		(GetHandleSize(theNMRec->nmIcon) == SICNsize)) {
		theIconRec = (IconRecPtr) GetFreeAux((InApplicationMenu(theNMRec->nmReserved, theNMRec->nmMark) && (theNMRec->nmMark)) ? &NMQHdr->pmIcons : &NMQHdr->amIcons);
		if (theIconRec) {
			theIconRec->iSIcon = theNMRec->nmIcon;
			theIconRec->iconSuite = (GetHandleSize(theNMRec->nmIcon) == sizeof(IconSuite));
			LinkAuxRec(theNMRec, (AuxRecPtr) theIconRec);
		}
	}
	return theIconRec;
}

/* Finds the next valid IconRec in the queue, deleting all unused entries along
 * the way, starting with theIconRec.
 */
IconRecPtr
ValidIconRec(IconRecPtr theIconRec, QHdrPtr queue)
{
	if (theIconRec)
		theIconRec = (IconRecPtr) theIconRec->hdr.qLink;
	else
		theIconRec = (IconRecPtr) queue->qHead;
	while (theIconRec && !theIconRec->hdr.aRefCount) {
		register IconRecPtr	tempIconRec = theIconRec->hdr.qLink;

		if (theIconRec->hdr.qType) {
			Dequeue((QElemPtr)theIconRec, queue);
			DisposPtr((Ptr)theIconRec);
		}
		theIconRec = tempIconRec;
	}

	return theIconRec;
}

/* This is run immediately after the code is installed in the system heap. */
void
NMINIT(void)
{
	NMQHdr = (NMHdrPtr) NewPtrSysClear(sizeof(NMHdr));
	NMQHdr->useNewIcon = true;
	NMQHdr->appleMenu = true;
	NMQHdr->copyIcon = NewHandleSys(sizeof(IconSuite));
}

/* This is the NMInstall trap, $A05E.  The underscore is because NMInstall() calls
 * the trap.  This routine installs the request into the queue.  It is guaranteed
 * to succeed if theNMRec is valid.
 */
OSErr
_NMInstall(NMRecPtr theNMRec)
{
	if (theNMRec->qType != nmType) return nmTypErr;
	if (NGetTrapAddress(0xA085,OSTrap) != NGetTrapAddress(0xA89F,ToolTrap)) 
		IdleUpdate();
	theNMRec->nmFlags = 0;
	theNMRec->nmPrivate = 0;
	theNMRec->nmReserved = GetPID();
	Enqueue((QElemPtr)theNMRec, &NMQHdr->requests);
	return noErr;
}

/* This is the NMRemove trap, $A05F.  The underscore is because NMRemove() calls
 * the trap.  This routine removes the request from the queue.  It will remove
 * the dialog if one is up, and will abort any icon rotation that is in progress.
 * It is guaranteed to succeed if theNMRec is valid and in the queue.
 */
OSErr
_NMRemove(NMRecPtr theNMRec)
{
	register OSErr	rValue;

	if (theNMRec->qType != nmType) return nmTypErr;

	if ((rValue = Dequeue((QElemPtr)theNMRec, &NMQHdr->requests)) == noErr) {
		/* See if we have a dialog up */
		if ((theNMRec->nmFlags & (fDlogDisp | fDlogCompl)) == fDlogDisp) NMQHdr->requests.qFlags |= fDlogInval;
		UnLinkAuxRecs(theNMRec);
	}

	return rValue;
}

/* Copies Pascal style strings. */
pascal void
CopyString(StringPtr srcString, StringPtr dstString)
{
	BlockMove(srcString, dstString, *srcString + 1);
}

/* Called by InitApplZone via the IAZNotify vector.  Removes all requests that
 * are in the application zone.
 */
void
FLUSHAPPLNM(void)
{
	if (SysZone != ApplZone) {
		register int		procLevel;
		register NMRecPtr	theNMRec;

		procLevel = disable();

		theNMRec = (NMRecPtr) NMQHdr->requests.qHead;			/* 1.5 type coersion */
		while (theNMRec != nil) {
			register NMRecPtr	nextNMRec = theNMRec->qLink;

			if (InApplZone((Ptr) theNMRec)) NMRemove(theNMRec);
			theNMRec = nextNMRec;
		}

		spl(procLevel);
	}
}

/* This dialog filter proc is almost identical to the default, except that it will
 * also return if aborted by NMRemove.
 */
pascal Boolean
NMFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit)
{
	if (theEvent->what == keyDown) {
		register unsigned char	theChar = theEvent->message & 0xFF;

		if ((theChar == 3)  || (theChar == 0xD)) {
			/* It's a return or enter. */
			short	itemType;
			ControlHandle	item;
			Rect	junk;

			GetDItem(theDialog, buttonItem, &itemType, (Handle*)&item, &junk);
			if (itemType == (ctrlItem+btnCtrl)) {
				/* It's an enabled button - highlight it */
				HiliteControl(item, inButton);
				Delay(8, (long*)&junk);
				HiliteControl(item, 0);
			}
			*itemHit = buttonItem;
			return true;
		}
	}
	if (NMQHdr->requests.qFlags & fDlogInval) {
		/* The request was removed - punt the dialog */
		return true;
	}
	return false;
}

/* This is the user item procedure which displays the default button roundRect. */
pascal void
DefButProc(DialogPtr theDialog, short itemNo)
{
	PenState	savePen;
	short				itemType;
	Handle				item;
	Rect				box;

	GetPenState(&savePen);
	PenSize(3,3);
	GetDItem(theDialog, itemNo, &itemType, &item, &box);
	FrameRoundRect(&box, 16, 16);
	SetPenState(&savePen);
}

typedef pascal long (**MBarProcHandle)(short, short, short, long);

/* This routine calls the Menu Bar Proc.  May be replaced by a trap at a later date.
 */
long
NMCallMBarProc(short message, short param1, long param2)
{
	register MenuL			**menuLHdl, *menuLPtr;
	register short			mbResID;
	register MBarProcHandle	MBDFHndl;


	/* This mess just forces code optimization */
	if ((menuLHdl = MenuList) && (menuLPtr = *menuLHdl)) {
		mbResID = menuLPtr->mbResID;
	}
	else return 0;

	ROMMapInsert = MapTRUE;
	if (MBDFHndl = (MBarProcHandle) GetResource('MBDF', mbResID)) {
		register short	flags = HGetState((Handle)MBDFHndl);
		register long	result;

		HLock((Handle)MBDFHndl);
		result = (**MBDFHndl)(mbResID & 7, message, param1, param2);
		HSetState((Handle)MBDFHndl, flags);
		return result;
	}
	else return 0;
}

/* theItem of theMenu is marked with newMarkChar iff it was previously marked with
 * oldMarkChar.
 */
pascal void		/* Declared pascal for efficiency */
SwapItemMarks(MenuHandle theMenu, short theItem, short oldMarkChar, short newMarkChar)
{
	short	theMarkChar;

	GetItemMark(theMenu, theItem, &theMarkChar);
	if (theMarkChar == oldMarkChar) SetItemMark(theMenu, theItem, newMarkChar);
}

/* Clears out all the Notification Manager marks from theMenu */
void
ClearMarks(MenuHandle theMenu)
{
	register short	i = CountMItems(theMenu);

	while (i > 0) SwapItemMarks(theMenu, i--, nmMarkChar, noMark);
}

/* Exchanges oldMarkChar for newMarkChar in the menu item referred to by
 * theMarkRec.  If the current mark is not oldMarkChar, nothing happens.
 */
pascal void		/* Declared pascal for efficiency */
SwapAppMark(MarkRecPtr theMarkRec, short oldMarkChar, short newMarkChar)
{
	register MenuHandle	theApplicationMenu = GetApplicationMenu();

	if (theApplicationMenu) {
		register short		thePID = theMarkRec->mPID;
		register short		theMarkID = theMarkRec->mMarkID;
		register short		theItem = 0;

		theItem = InApplicationMenu(thePID, theMarkID);
		if (theItem) SwapItemMarks(theApplicationMenu, theItem, oldMarkChar, newMarkChar);
	}
}

#define MarkApp(theMarkRec)		SwapAppMark(theMarkRec, noMark, nmMarkChar)
#define UnMarkApp(theMarkRec)	SwapAppMark(theMarkRec, nmMarkChar, noMark)

/* Mark an item in the apple or Application Menu */
void
MarkItem(NMRecPtr theNMRec, int procLevel)
{
	if (theNMRec->nmMark != 0) {
		register MarkRecPtr	theMarkRec = InsertMarkRec(theNMRec);

		if (theMarkRec) {
			theNMRec->nmFlags |= fItemMarked;
			spl(procLevel);
			MarkApp(theMarkRec);
		}
	}
	else theNMRec->nmFlags |= fItemMarked;
}

/* Start an icon rotating */
void
RotateIcon(NMRecPtr theNMRec, int /*procLevel*/)
{
	if (theNMRec->nmIcon != 0) {
		if (InsertIconRec(theNMRec)) theNMRec->nmFlags |= fIconinRot;
	}
	else theNMRec->nmFlags |= fIconinRot;
}

/* Play a sound */
void
DoSound(NMRecPtr theNMRec, int procLevel)
{
	register Handle	theSound = theNMRec->nmSound;
	SndChannelPtr		chan = nil;	// <SM6>
	long				sysBeepVolume = 0;
	SndCommand			cmd;

	theNMRec->nmFlags |= fSndPlayed;
	if (theSound) 
	{
		spl(procLevel);
		if (theSound == (Handle) -1) 
			SysBeep(3);
		else 
		{
			if ( SndNewChannel(&chan, 0, 0, nil) == noErr )
			{
				GetSysBeepVolume(&sysBeepVolume);
					cmd.cmd = volumeCmd;
					cmd.param2 = sysBeepVolume;
					SndDoImmediate(chan, &cmd);
				
				SndPlay(chan, theSound, false);
				SndDisposeChannel(chan, true);
			}
		}
	}
}

/* Put up a dialog.  Currently, this procedure waits for the user to respond, or
 * the request to be removed.  This should be fixed in the future.
 */
void
DoDialog(NMRecPtr theNMRec, int procLevel)
{
	if (theNMRec->nmStr == nil) theNMRec->nmFlags |= fDlogDisp | fDlogCompl;
	/* DoDialog not reentrant, temporarily lock out this request */
	else if (NMQHdr->requests.qFlags & fInDoDlog) theNMRec->nmFlags |= fReqLocked;
	else {
		register DialogPtr	theDialog;
		register short		saveResFile;
		GrafPtr				savePort;
		Str255				alertStr;
		short				itemType, itemHit;
		Handle				item;
		Rect				box;

		/* Mark that we're in here for reentrancy test */
		NMQHdr->requests.qFlags |= fInDoDlog;

		/* Flag that we are going to bring up a dialog */
		/* Also lock it until the user responds */
		theNMRec->nmFlags |= fDlogDisp | fReqLocked;

		/* Copy the string */
		CopyString(theNMRec->nmStr, alertStr);

		spl(procLevel);

		InitCursor();

		GetPort(&savePort);

		saveResFile = CurResFile();
		UseResFile(0);
		theDialog = GetNewDialog(NMDialog, nil, (WindowPtr) -1);
		UseResFile(saveResFile);

		GetDItem(theDialog, defButItem, &itemType, &item, &box);
		SetDItem(theDialog, defButItem, itemType, (Handle) DefButProc, &box);	/* 1.5 type coersion */

		GetDItem(theDialog, textItem, &itemType, &item, &box);
		SetIText(item, alertStr);

		ShowWindow(theDialog);
		ModalDialog((ModalFilterProcPtr) NMFilter, &itemHit);		/* 1.5, 1.7 type coersion */

		/* If the queue entry is intact, mark the dialog as displayed and unlock the request.
		 * Disable queue access during this time to prevent removal underneath.
		 */
		procLevel = disable();
		if (!(NMQHdr->requests.qFlags & fDlogInval)) {
			/* Queue entry is intact */
			theNMRec->nmFlags |= fDlogCompl;
			theNMRec->nmFlags &= ~fReqLocked;
		}
		else NMQHdr->requests.qFlags &= ~fDlogInval;

		/* Also unlock the next pending dialog. */
		theNMRec = (NMRecPtr) NMQHdr->requests.qHead;		/* 1.5 type coersion */
		while (theNMRec != nil) {
			if ((theNMRec->nmFlags & (fDlogDisp | fReqLocked)) == fReqLocked) {
				theNMRec->nmFlags &= ~fReqLocked;
				break;
			}
			theNMRec = (NMRecPtr) theNMRec->qLink;			/* 1.5 type coersion */
		}

		spl(procLevel);

		DisposDialog(theDialog);

		SetPort(savePort);
		NMQHdr->requests.qFlags &= ~fInDoDlog;
	}
}

/* Call the response procedure */
void
DoRespProc(NMRecPtr theNMRec, int procLevel)
{
	register NMProcPtr	respProc = theNMRec->nmResp;

	theNMRec->nmFlags |= fRespCompl;
	if (respProc) {
		if (respProc == (NMProcPtr) -1) NMRemove(theNMRec);
		else {
			spl(procLevel);
			(*respProc)(theNMRec);
		}
	}
}

/* Called by GetNextEvent.  Checks to see if the alarm has gone off. */
pascal void
NMGNEFilter(void)
{
	register NMRecPtr	theAlarmRec = NMQHdr->alarmRec;

	if (Alarm()) {
		/* Alarm should be on */
		if (!theAlarmRec) {
			/* It's not, turn it on */
			register Handle	theSIcon;

			ROMMapInsert = MapTRUE;
			if (theSIcon = GetResource('SICN', AlarmSICN)) {
				/* Build theAlarmRec and copy the alarm icon into the sys heap */
				register Size	iconSize = GetHandleSize(theSIcon);

				if ((theAlarmRec = (NMRec *) NewPtrSysClear(sizeof(NMRec))) &&
					(theAlarmRec->nmIcon = NewHandleSys(iconSize))) {
					BlockMove(*theSIcon, *(theAlarmRec->nmIcon), iconSize);
					theAlarmRec->qType = nmType;
					theAlarmRec->nmSound = (Handle) -1;		/* Use SysBeep */
					NMInstall(theAlarmRec);
					NMQHdr->alarmRec = theAlarmRec;
				}
				ReleaseResource(theSIcon);
			}
		}
	}
	else {
		/* Alarm should be off */
		if (theAlarmRec) {
			/* It's not, turn it off */
			NMRemove(theAlarmRec);
			DisposHandle(theAlarmRec->nmIcon);
			NMQHdr->alarmRec = nil;
		}
	}
}

/* Called by SystemTask in the topmost layer */
pascal void
NMTask(void)
{
	register int	procLevel;
	short	selector[4];
	
	/* NOTE: If this code seems buggy, take this 'if' statement and move it below after the call to AddIconRec() */
	if (NMQHdr->requests.qHead == nil)
		return;

	selector[0] = MoveSICNToPM;
	selector[1] = MoveICSToPM;
	selector[2] = MoveSICNToAM;
	selector[3] = MoveICSToAM;
	
	procLevel = disable();
 
	{
		/* Find the first queue element that needs attention.
		 * Note that the response proc is the last thing done.
		 */
		register NMRecPtr	theNMRec = NMQHdr->requests.qHead;
		while ((theNMRec != nil) && (theNMRec->nmFlags & fReqLocked)) theNMRec = (NMRecPtr) theNMRec->qLink;	/* 1.5 type coersion */

		/* Determine the next thing to be done and call the procedure to do it */
		if (theNMRec != nil) {
			register short	nmFlags = theNMRec->nmFlags;

			if (!(nmFlags & fItemMarked)) MarkItem(theNMRec, procLevel);
			else if (!(nmFlags & fIconinRot)) RotateIcon(theNMRec, procLevel);
			else if (!(nmFlags & fSndPlayed)) DoSound(theNMRec, procLevel);
			else if (!(nmFlags & fDlogDisp)) DoDialog(theNMRec, procLevel);
			else if (!(nmFlags & fRespCompl)) DoRespProc(theNMRec, procLevel);
			else theNMRec->nmFlags |= fReqLocked | fReqDone;
		}
	}

	spl(procLevel);

	AddMarkRec();
	AddIconRec();

	/* Now go through the marks queue, deleting entries and updating menu marks
	 * as needed.
	 */
	{
		register MenuHandle	theApplicationMenu = GetApplicationMenu();
		register MarkRecPtr	theMarkRec = NMQHdr->marks.qHead;

		if (theApplicationMenu && (NMQHdr->lastMenuUsed != theApplicationMenu)) ClearMarks(theApplicationMenu);

		while (theMarkRec != nil) {
			register MarkRecPtr	nextMarkRec = theMarkRec->hdr.qLink;

			if (theMarkRec->hdr.qType) {
				if (!theMarkRec->hdr.aRefCount) {
					/* Remove the mark and delete this record */
					UnMarkApp(theMarkRec);
					Dequeue((QElemPtr)theMarkRec, &NMQHdr->marks);
					DisposPtr((Ptr)theMarkRec);
				}
				else if (theApplicationMenu && NMQHdr->lastMenuUsed != theApplicationMenu) MarkApp(theMarkRec);
			}
			theMarkRec = nextMarkRec;
		}
		NMQHdr->lastMenuUsed = theApplicationMenu;
	}

	{
		/* Get the icon to be displayed for the menu bar defproc */
		register long	curTime = Ticks, timeSince = curTime - NMQHdr->lastMoveTime;
		register IconRecPtr	*curIconRec;

		curIconRec = (NMQHdr->appleMenu ? &NMQHdr->curAMIconRec : &NMQHdr->curPMIconRec);

		if (timeSince >= MoveSThresh) {
			register IconRecPtr	theIconRec = *curIconRec;

			if (NMQHdr->useNewIcon) {
				/* Get the next icon from the queue.  If not enough time has elapsed,
				 * or the new icon is the same as the old, get out.
				 */
				if (timeSince >= WaitSThresh(NMQHdr->amIcons, NMQHdr->pmIcons)) {
					register IconRecPtr	newIconRec = ValidIconRec(theIconRec,  (NMQHdr->appleMenu ? &NMQHdr->amIcons : &NMQHdr->pmIcons));

					if (newIconRec == theIconRec) {
						NMQHdr->appleMenu = !(NMQHdr->appleMenu);
						return;
					}
					else {
						*curIconRec = theIconRec = newIconRec;
						if (theIconRec)
							BlockMove(*(theIconRec->iSIcon), *(NMQHdr->copyIcon), (theIconRec->iconSuite ? sizeof(IconSuite) : SICNsize));
					}
				}
				else return;
			}

			NMQHdr->lastMoveTime = curTime;
			NMQHdr->useNewIcon = NMCallMBarProc(selector[(NMQHdr->appleMenu << 1) + (theIconRec ? theIconRec->iconSuite : 0)],
													MoveSDist,
													(long) (theIconRec ? NMQHdr->copyIcon: nil));
			NMQHdr->appleMenu = !(NMQHdr->appleMenu);
		}
	}
}
