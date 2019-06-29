/*
 File Memory.c
 Copyright Apple Computer, Inc. 1985-1987
 All rights reserved.

 * Memory - report the amount of free space in the
 * application and system heap, and on the boot volume.
 *
 * This is the sample C desk accessory.  The desk accessory does not
 * use any global variables.  Instead, it allocates a handle to a
 * structure that holds some "global" variables.  This sample program
 * could be written without having to use this structure, but then it
 * wouldn't be as informative...
 */
#include	<types.h>
#include	<osutils.h>
#include	<memory.h>
#include	<devices.h>
#include	<events.h>
#include	<quickdraw.h>
#include	<fonts.h>
#include	<windows.h>
#include	<files.h>
#include	<errors.h>
#include	<toolutils.h>
#include	<packages.h>

/*
 * Macro to compute owned resource id
 */

#define OWNEDRSRCID(id)	(0xC000 | (((-(id)) - 1) << 5))

/*
 * String constant indexes for STR# resource
 */

#define	APPHEAP		1
#define	SYSHEAP		2
#define	DISK		3
#define	FREEON		4

#define	ACCEVENT	64
#define	ACCRUN		65

/* This structure type holds the global variables used by this desk accessory */

typedef struct {
	int 	rsrcID; 		/* Computed rsrc id of STR# and WIND resources */
	Str255	strBuf;			/* Buffer to read strings into */
} Globals;


pascal short DRVROpen(CntrlParam *ctlPB, DCtlPtr dCtl)
{
	#pragma	unused (ctlPB)
	GrafPtr		savePort;
	WindowPeek	myWindow;
	long		heapGrow;

	/*
	 * If the windowPtr is non-nil, we already have a window open.
	 * This desk accessory ignores multiple opens.
	 */
	if (dCtl->dCtlWindow != nil)
		return noErr;

	GetPort(&savePort);
	
	/*
	 * Get a handle to some storage that will hold our pseudo-global
	 * variables.  Save the handle in a location accessible by
	 * all the driver routines.
	 */
	dCtl->dCtlStorage = NewHandle(sizeof(Globals));
	/*
	 * Compute the resource id of the owned 'STR#' resource that
	 * contains all of the program's text strings.  The id is saved
	 * in one place that can be accessed by all the driver routines.
	 */
	((Globals *)(*dCtl->dCtlStorage))->rsrcID = OWNEDRSRCID(dCtl->dCtlRefNum);
	
	/*
	 * wStorage = nil (allocate on the heap)
	 * visible = false, behind = -1, goAway = true, refCon = 0
	 */
	myWindow = (WindowPeek)GetNewWindow(((Globals *)(*dCtl->dCtlStorage))->rsrcID, nil, (WindowPtr) -1);
	/*
	 * Set windowKind to the DA refNum, which is negative.
	 */
	myWindow->windowKind = dCtl->dCtlRefNum;
	/*
	 * Store the windowPtr in the Device Control Entry
	 */
	dCtl->dCtlWindow = (WindowPtr)myWindow;
	/*
	 * Now compact the heap in the most violent way.
	 * Purge whatever's purgeable.
	 */
	(void) MaxMem(&heapGrow);

	SetPort(savePort);
	return noErr;
}



pascal short DRVRPrime(CntrlParam *ctlPB, DCtlPtr dCtl)
{
	#pragma	unused (ctlPB, dCtl)
	return noErr;			/* Not used in this desk accessory */
}

pascal short DRVRStatus(CntrlParam *ctlPB, DCtlPtr dCtl)
{
	#pragma	unused (ctlPB, dCtl)
	return noErr;			/* Not used in this desk accessory */
}

pascal short DRVRControl(CntrlParam *ctlPB, DCtlPtr dCtl)
{
	extern	void	doCtlEvent();
	extern	void	doPeriodic();

	/*
	 * The current grafPort is saved & restored by the Desk Manager
	 */
	switch (ctlPB->csCode) {
		case ACCEVENT:				/* accEvent */
			HLock(dCtl->dCtlStorage);	/* Lock handle since it will be dereferenced */
			doCtlEvent( *((EventRecord **) &ctlPB->csParam[0]),
					(Globals *)(*dCtl->dCtlStorage));
			HUnlock(dCtl->dCtlStorage);
			break;

		case ACCRUN:				/* periodicEvent */
			doPeriodic(dCtl);
			break;

		default:
			break;
	}
	return 0;
}

static void doCtlEvent(register EventRecord *theEvent, Globals *globals)
{
	register	WindowPtr	myWindow;
	extern		void		drawWindow();

	if (theEvent->what == updateEvt) {
		myWindow = (WindowPtr) theEvent->message;
		SetPort(myWindow);
		BeginUpdate(myWindow);
		drawWindow(myWindow, globals);
		EndUpdate(myWindow);
	}
}

static void doPeriodic(DCtlPtr dCtl)
{
	extern void drawWindow();

	SetPort(dCtl->dCtlWindow);
	HLock(dCtl->dCtlStorage);	/* Lock handle since it will be dereferenced */
	drawWindow(dCtl->dCtlWindow, (Globals *)(*dCtl->dCtlStorage));
	HUnlock(dCtl->dCtlStorage);
}

/*
 * Display the contents of the window.
 * The current port is assumed to be set to the window.
 */
static void drawWindow(WindowPtr window, Globals *globals)
{
	THz 				saveZone;
	Str27				volName;
	HVolumeParam		myParamBlk; 
	void				printNum(unsigned long);
	static	StringPtr	text(int index, Globals *globals);

	if (window == nil)
		return;		/* "can't happen" */

	TextMode(srcCopy);
	TextFont(monaco);
	TextSize(9);

	MoveTo(6, 10);
	TextFace(bold);

	saveZone = GetZone();

	DrawString(text(APPHEAP, globals));
	SetZone(ApplicZone());
	printNum(FreeMem());

	DrawString(text(SYSHEAP, globals));
	SetZone(SystemZone());
	printNum(FreeMem());

	SetZone(saveZone);

	DrawString(text(DISK, globals));
	myParamBlk.ioNamePtr = volName;
	myParamBlk.ioVRefNum = 0;		/* Boot volume */
	myParamBlk.ioVolIndex = 0;
	(void) PBHGetVInfo((HParmBlkPtr)&myParamBlk, false);
	printNum((unsigned long)myParamBlk.ioVAlBlkSiz * myParamBlk.ioVFrBlk);

	DrawString(text(FREEON, globals));
	TextFace(underline);
	DrawString(volName);
}

static void printNum(unsigned long num)
{
	unsigned	char	numStr[32];

	TextFace(normal);
	NumToString(num, numStr);		/* Its possible that a large unsigned
									   will come back negative! */
	DrawString(numStr);
	TextFace(bold);
}

pascal short DRVRClose(char *ctlPB, DCtlPtr dCtl)
{						/* Save & Restore current grafPort? */
	#pragma	unused (ctlPB)
	WindowPtr window;

	window = (WindowPtr) dCtl->dCtlWindow;
	if ( window != nil) {
		dCtl->dCtlWindow = nil;
		DisposHandle(dCtl->dCtlStorage);
		DisposeWindow(window);
	}

	return 0;
}

static StringPtr text(int index, Globals *globals)
{
	GetIndString(globals->strBuf, globals->rsrcID, index);
	return(globals->strBuf);
}
