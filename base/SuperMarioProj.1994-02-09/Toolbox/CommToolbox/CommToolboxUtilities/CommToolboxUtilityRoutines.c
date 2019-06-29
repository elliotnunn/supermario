/*
	File:		CommToolboxUtilityRoutines.c

	Contains:	Various routines

	Written by:	Byron Han
				Alex Kazim

	Copyright:	© 1990, 1992 by Apple Computer, Inc., all rights reserved.


	Change History (most recent first):

		<16>	11/13/92	JDR		Updated QuickDraw.h to not use the dangerous pattern type.
		<15>	 5/29/92	DCL		Included TextUtils.h and Script.h. EqualString and GetScript
									moved for the New Inside Mac.
		<14>	  2/8/91	JNG		kaz,#82584:Getflst needs to make sure the system file is in the
									resource chain before calling GetIndFlst.
		<13>	 8/19/90	kaz		Fixed gray OK outline to check for ColorQD and a color port.
		<12>	 8/15/90	kaz		Added PlaceWindow() routine to perform 2/3rds centering.
		<11>	  8/6/90	kaz		Added grayOutline button routine, HaveColorQD() macro.
		<10>	  7/2/90	kaz		GetIndflst will compensate for odd font names.
		 <9>	 5/24/90	kaz		Combined GetIndfinf & GetIndflst; Added Getflst()
		 <8>	 5/22/90	kaz		Renamed Parse_finf, Parse_flst. Using new TextState data struct.
									Adding GetTextState, SetTextState
		 <7>	 4/18/90	kaz		DoTheIcon() now uses only 1 BitMap
		 <6>	 4/11/90	kaz		Added International CmdPeriod() from TechNotes
		 <5>	 3/20/90	kaz		Merging utility routines from 1.0
									- Fetch QD Globals
									- Get just the DITL ItemRect
									- Set the UserItem Proc
									- PCopyString instead of Pascal aStr := bStr
		 <4>	 3/16/90	BBH		added MIN and MAX routines
		 <3>	 3/16/90	BBH		change GetDitem to GetDItem
		 <2>	 3/16/90	BBH		added DoTheIcon, parse_finf, parse_flst, FindMenuItem,
									GoodDrawOutlineButton
		 <1>	 3/14/90	BBH		first checked in

	To Do:
*/

//	These are utility routines used by CommToolboxCore.c and the various managers

//	this routine returns the first resource of the given type from the
//	tool with the procID specified.  This makes no assumptions about the
//	tool already being in the resource chain

#include <Menus.h>
#include <Dialogs.h>
#include <QuickDraw.h>
#include <Fonts.h>
#include <SysEqu.h>
#include <TextUtils.h>
#include <Script.h>

#include "CTBUtilities.h"
#include "CommResources.h"
#include "CommToolboxPriv.h"

/* For CmdPeriod()	*/
#define		kMaskModifiers	0xFE00				// strip the cmd key
#define		kMaskVirtualKey	0x0000FF00			// virtual key from event
#define		kUpKeyMask		0x0080
#define		kShiftWord		8					// virtual key -> keycode
#define		kMaskASCII1		0x00FF0000			// getting the key
#define		kMaskASCII2		0x000000FF
#define		kPeriod			0x2E				// ascii for a '.'

#define		HaveCQD()	((*(unsigned short *) ROM85) <= 0x3FFF)

pascal long MIN(a, b)
long	a, b;
{
	return( a < b ? a : b);
}

pascal long MAX(a, b)
long	a, b;
{
	return( a > b ? a : b);
}

pascal Handle 	CRMGetAResource(theType, procID)
ResType theType;
short procID;
{
	CRMToolContext		theContext;
	short				cur;
	Handle				theHandle;

	CRMToolPreflight(procID, &theContext);
	cur = CurResFile();
	UseResFile(procID);
	theHandle = CRMGet1IndResource(theType, 1);
	UseResFile(cur);
	CRMToolPostflight(&theContext);

	return(theHandle);
}

pascal void DoTheIcon(theIcon, destRect, selected)
	Ptr	theIcon;
	const Rect *destRect;
	Boolean selected;
{

#define		ICONSIZE	32

typedef struct IconsRecord {
	long		icon[ICONSIZE];
	long		mask[ICONSIZE];
} IconsRecord, *IconsRecordPtr;

	BitMap	mBitMap;
	GrafPtr	myPort;
	IconsRecordPtr iP;

	GetPort(&myPort);

	SetRect(&mBitMap.bounds, 0, 0, ICONSIZE, ICONSIZE);
	mBitMap.rowBytes = 4;

	iP = (IconsRecordPtr) theIcon;

	mBitMap.baseAddr = (Ptr) &iP->mask;
	CopyBits(&mBitMap, &myPort->portBits, &mBitMap.bounds, destRect, selected ? srcOr : srcBic, nil);

	mBitMap.baseAddr = (Ptr) &iP->icon;
	CopyBits(&mBitMap, &myPort->portBits, &mBitMap.bounds, destRect, srcXor, nil);
}


/*************************************************************
	DoSolidOutline	- Draws a solid outline
								Used for default buttons.

		pDialog		-	Where we are
		theItem		-	The DITL item #
**************************************************************/
pascal void DoSolidOutline(DialogPtr pDialog, short theItem)
{
	Rect		itemRect;
	PenState	ps;

	GetPenState(&ps);
	PenNormal();
	PenSize(3,3);
	GetItemRect(pDialog, theItem, &itemRect);
	FrameRoundRect(&itemRect, 16, 16);
	SetPenState(&ps);
}

/*************************************************************
	DoGrayOutline	- Draws a gray outline
							Used for default buttons.

		pDialog		-	Where we are
		theItem		-	The DITL item #
**************************************************************/
pascal void DoGrayOutline(DialogPtr pDialog, short theItem)
{
	Rect			itemRect;
	PenState		ps;
	GrafWorldPtr	qdWorld;
	PixPatHandle	myPix = nil;
	RGBColor		oldFore,
					oldAft;

	GetPenState(&ps);
	PenNormal();
	PenSize(3,3);
	GetItemRect(pDialog, theItem, &itemRect);

	if (HaveCQD())
		if (((CGrafPtr) pDialog)->portVersion & 0xC000) {
			GetForeColor(&oldFore);
			GetBackColor(&oldAft);

			// gimme average RGB.  Why does MenuMgr do a ROXR instead of an ASR??
			oldFore.red = (oldFore.red + oldAft.red) >> 1;
			oldFore.green = (oldFore.green + oldAft.green) >> 1;
			oldFore.blue = (oldFore.blue + oldAft.blue) >> 1;

			myPix = NewPixPat();

			if (myPix != nil) {
				MakeRGBPat(myPix,&oldFore);
				PenPixPat(myPix);
			}
		}


	if (myPix == nil) {
		qdWorld = GetQDGlobals();
		PenPat((ConstPatternParam)&(qdWorld->gray));
	}

	FrameRoundRect(&itemRect, 16, 16);

	if (myPix != nil)
		DisposPixPat(myPix);

	SetPenState(&ps);
}


/*************************************************************
	GetIndflst	- Walks the font info template ('finf' or
					'flst' and tries to get the request

		hInfo		-	A handle to the data from GetResource
		oldStyle	-	True if a 'finf', else 'flst'
		index		-	Which font info do you want
		finfInfo	-	Info to return

		returns		-	-1 if it didn't work
**************************************************************/

pascal OSErr GetIndflst(hInfo, oldStyle, index, finfInfo)
	Handle 		hInfo;
	Boolean		oldStyle;
	short 		index;
	TextState 	*finfInfo;
{
	short		numEntries;
	long		offset;
	Str255		fontName;
	short		i;
	short		aFont;

	finfInfo->theFont = 0;						// resort to defaults
	finfInfo->theSize = 0;
	finfInfo->theFace = 0;
	finfInfo->theMode = srcOr;

	numEntries = *((short *) (*hInfo));
	offset = (long) (*hInfo) + sizeof(numEntries);

	/* Have to walk this 'cuz 'flst' is variable length */
	for (i = 1; i <= numEntries; i++) {
		if (oldStyle) {
			aFont = *((short *) offset);
			offset += sizeof(short);
		}
		else {
			BlockMove((Ptr) offset, fontName, *((unsigned char *) offset) + 1);
			offset += Length(fontName) + 1;			// add length + length-byte
			if (offset & 0x0001)					// even it out
				offset++;
		}

		/* Grab the rest of the data	*/
		if (i == index) {
			if (oldStyle)
				finfInfo->theFont = aFont;
			else
				GetFNum(fontName, &finfInfo->theFont);
			finfInfo->theFace = *((short *) offset);
			offset += sizeof(short);
			finfInfo->theSize = *((short *) offset);
			offset += sizeof(short);
			if (! oldStyle)							// mode does not exist in 'finf'
				finfInfo->theMode = *((short *) offset);
			return(noErr);
		}

		offset += sizeof(short);			// face
		offset += sizeof(short);			// size
		if (! oldStyle)
			offset += sizeof(short);		// mode
	}
	return(-1);
}


/*************************************************************
	Getflst		- Tries to get the 'flst' resource from the
				specified file, then tries to get a 'finf',
				finally resorts to System 'flst'. Assumes that
				the resource will exist in the System.

		procID		-	The file to get it from
		resourceID	-	yes
		finfIndex	-	Index to get
		finfInfo	-	Info to return

**************************************************************/

#define		toolFINF	1
#define		sysFLST		2

pascal void Getflst(procID, resourceID, finfIndex, finfInfo)
	short 		procID;
	short		resourceID;
	short		finfIndex;
	TextState 	*finfInfo;
{
	short		savedResRef;
	Handle		theResource;
	short		tries;
	ResType		theType;

	savedResRef = CurResFile();


	for (tries = 0; tries < 3; tries++) {


		theType = 'flst';
		if (tries == toolFINF)		// we try the 'finf' to maintain compatibility
			theType = 'finf';

		if (tries == sysFLST)
			UseResFile(0);
		else
			UseResFile(procID);

		theResource = Get1Resource(theType, resourceID);

		if (theResource == nil)
			continue;

		UseResFile(0);				// Needs to be the system so GetIndflst can find font names

		if (GetIndflst(theResource, (tries == toolFINF), finfIndex, finfInfo) == noErr)
			break;
	}

	UseResFile(savedResRef);
}



/*************************************************************
	GetTextState	- Loads the current grafport text info

		txState		-	Like penState
**************************************************************/

pascal void GetTextState(TextState *txState)
{
	GrafPtr		thePort;

	GetPort(&thePort);

	txState->theFont = thePort->txFont;
	txState->theFace = thePort->txFace;
	txState->theSize = thePort->txSize;
	txState->theMode = thePort->txMode;
}



/*************************************************************
	SetTextState	- Sets the current grafport text info

		txState		-	Like penState
**************************************************************/

pascal void SetTextState(TextState *txState)
{
	TextFont(txState->theFont);
	TextSize(txState->theSize);
	TextFace(txState->theFace);
	TextMode(txState->theMode);
}



pascal short FindMenuItem(hMenu, theItem)
MenuHandle hMenu;
const Str255 theItem;
{
	short 	i;
	Str255 	theString;
	short	limit;

	limit = CountMItems(hMenu);
	for (i = 1; i <= limit; i++) {
		GetItem(hMenu, i, &theString);
		if (EqualString(theString, theItem, false, true) )
			return(i);
	}
	return(0);
}

/*************************************************************
	GetQDGlobals	-	Gets a pointer to the start of
						the QD Globals given that A5 is valid

		oneWorld	-	The stuff we need

**************************************************************/

pascal GrafWorldPtr GetQDGlobals()
{
	GrafWorldPtr	oneWorld;

	/* A5 points to thePort of QD Global structure, so we add	*/
	/* the GrafPtr to make oneWorld point past the bottom		*/
	/* Then we make oneWorld point to the top					*/

	oneWorld = (GrafWorldPtr) ((**((long **) CurrentA5)) + sizeof(GrafPtr));
	return(--oneWorld);
}


/*************************************************************
	GetItemRect	-	Gets a rectangle for a dialog item
					so I don't have to pass all those damn
					parameters all the time.

		theDialog	-	The place
		theItem		-	The DITL number
		theRect		-	Thar she blows

**************************************************************/

pascal void GetItemRect(DialogPtr theDialog, short theItem, Rect *theRect)
{
	short		itemKind;
	Handle		itemHandle;

	GetDItem(theDialog, theItem, &itemKind, &itemHandle, theRect);
}


/*************************************************************
	SetUserItemProc	-	Sets the DITL user Item Proc

		theDialog	-	The place
		theItem		-	The DITL number
		theProc		-	What to set it to

**************************************************************/

pascal void SetUserItemProc(DialogPtr theDialog, short theItem, ProcPtr theProc)
{
	short		itemKind;
	Handle		itemHandle;
	Rect		itemRect;

	GetDItem(theDialog, theItem, &itemKind, &itemHandle, &itemRect);
	SetDItem(theDialog, theItem, itemKind, (Handle) theProc, &itemRect);
}

/*************************************************************
	PCopyString		-	Copies the Pascal string using
							BlockMove src -> dest

		srcStr		-	The source & scourge
		destStr		-	Unsuspecting bystander

**************************************************************/

pascal void PCopyString(Str255 srcStr, Str255 destStr)
{
	BlockMove((Ptr) srcStr,(Ptr) destStr,Length(srcStr) + 1);
}

/*************************************************************
	CmdPeriod	-	Determines if the cmd-. was hit in an
						international friendly way

		theEvent	- what to look at; not a VAR param

**************************************************************/

pascal Boolean CmdPeriod(EventRecord *theEvent)
{
	Boolean		fTimeToQuit;
	short		keyCode;
	long		virtualKey, keyInfo, lowChar,
				highChar, state, keyCId;
	Handle		hKCHR;

	fTimeToQuit = false;

	if ((theEvent->what == keyDown) || (theEvent->what == autoKey)) {

		/* If CmdKey is down, get the ascii equiv of the keydown	*/
		if (theEvent->modifiers & cmdKey) {
			virtualKey = (theEvent->message & kMaskVirtualKey) >> kShiftWord;

			/* get rid of the cmd key and add the virtual	*/
			keyCode = (theEvent->modifiers & kMaskModifiers) | virtualKey;
			state = 0;

			keyCId = GetScript(GetEnvirons(smKeyScript), smScriptKeys);
			hKCHR = GetResource('KCHR', keyCId);

			if (hKCHR != nil) {
				keyInfo = KeyTrans(*hKCHR, keyCode, &state);
				ReleaseResource(hKCHR);
			}
			else
				keyInfo = theEvent->message;

			lowChar = keyInfo & kMaskASCII2;
			highChar = (keyInfo & kMaskASCII1) >> 16;

			if ((lowChar == kPeriod) || (highChar == kPeriod))
				fTimeToQuit = true;
		}	// cmdKey down

	}	// keyEvent

	return(fTimeToQuit);
}

/****************************************************************************
*	PlaceWindow	-	Determines where to put the topLeft corner
*					of a window given a relative window
*
*	Inputs:
*		theParent	- place the child relative to it
*		childHeight	- the size of the child window.  We use a rect 'cuz the
*		childWidth	- dialog may be opened/closed by a ROM routine (SFGetFile)
******************************************************************************/
pascal Point PlaceWindow(WindowPtr theParent,short childHeight,short childWidth)
{
	GDHandle	theScreen,
				maxScreen;			// How many faces
	RgnHandle	screenRgn;
	long		areaTaken,
				maxAreaTaken = 0;	// The amt the parent occupies per screen
	Point		offset;				// Relative to the GDevice
	Rect		screenRect;
	Boolean		addMenu = false;
	Point		returnVal;

	if (! HaveCQD()) {							// old fashioned way
		screenRect = (**(GetGrayRgn())).rgnBBox;
		goto setLoc;
	}

	if (theParent == nil) {					// Put it on the main screen
		screenRect = (**(GetMainDevice())).gdRect;
		addMenu = true;
		goto setLoc;
	}
									// Put it on that screen
	screenRgn = NewRgn();

	/* We will walk the screen list to decide which one	*/
	/* the parent window is most on.					*/

	theScreen = GetDeviceList();		// Scan thru the GDevices
	maxScreen = theScreen;
	maxAreaTaken = 0;

	while (theScreen != nil) {
		RectRgn(screenRgn,&(**theScreen).gdRect);
		SectRgn(screenRgn,((WindowPeek) theParent)->contRgn,screenRgn);

		screenRect = (**screenRgn).rgnBBox;
		areaTaken = (screenRect.right - screenRect.left) *
								(screenRect.bottom - screenRect.top);

		if (areaTaken > maxAreaTaken) {			// This be the big one
			maxScreen = theScreen;
			maxAreaTaken = areaTaken;
		}

		theScreen = GetNextDevice(theScreen);	// NeXT, please
	}	// in the list

	DisposeRgn(screenRgn);

	screenRect = (**maxScreen).gdRect;
	addMenu = (maxScreen == GetMainDevice());

setLoc:
	offset.h = ((screenRect.right - screenRect.left) - childWidth) >> 1;
	offset.v = ((screenRect.bottom - screenRect.top) - childHeight) / 3;
	SetPt(&returnVal, screenRect.left + offset.h, screenRect.top + offset.v);
	if (addMenu)
		returnVal.v += (GetMBarHeight() << 1) / 3;

	return(returnVal);
}