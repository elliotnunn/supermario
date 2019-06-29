/*
	File:		LayerMgr.c

	Contains:	Layer Manager primitives

	Written by:	Ed Tecot

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<SM3>	 11/10/92	CSS		Update from Reality:
									<74>	10/28/92	DTY		Use Get/Set macros to access ExpandMem.
									<73>	 6/29/92	DC		added a condition to the conditional "theFuture" to eliminate
																this change when compiling the INIT so as to be CubeE clean.
									<72>	 5/31/92	DC		Added a call to ShowHide to __CloseWindow when it is called on a
																layer, since the new implementation of ShowHide for a layer will
																crash in the middle of a close.
									<71>	 5/15/92	DC		Fixes to make ShowHide work for layers to provide application
																hiding.  Still not working.
									<70>	 5/14/92	DC		Fixed more bugs in layerless apps (again all changes are
																conditionalized to not affect CubeE)
									<69>	 4/20/92	DC		Fixed bugs in layerless apps mods (all conditionalized to
																protect CubeE and System builds)
									<68>	 4/10/92	DC		Put in conditional changes to WindowMgrPatches.a and LayerMgr.c
																for implementing layerless apps.
		<SM2>	 5/22/92	FM		
		<SM2>	  5/7/92	FM		renamed the romXXX routines as XXXXGlue since thats what they are highlevel interfaces for
									register based routines. The windowmgrpatches file in reality will need to be changed to reflect this.

									renamed oldXXXX routines as XXXXGuts. These are assembly core routines that the
									LayerMgr is building on (originally in the form of a tail patch.)

									renamed __BeginUpdate as BeginUpdateOfLayersChildren to reflect the funstion of this
									routine. It is now called from WindowMgr.a in BeginUpdate. I did this to replace the 
									sick, fragmented jumping around that used to be done in the patch file.

		<67>	 10/4/91	JSM		Change PsychoticFarmerOrLater conditionals to TheFuture.
		<66>	 9/22/91	DTY		Change PsychoticFarmerAndLater to PsychoticFarmerOrLater.
		<65>	 9/13/91	dba		Fix the bug where a window which is created with a nil
									titleHandle gets an empty string for its handle, even though
									layers don’t use the titleHandle field.
		<64>	 9/12/91	DTY		This is the last time I listen to Greg. The title handle needs
									to be checked because LayerRecords get hosed without it.
		<63>	 8/30/91	DTY		Always create a title handle. (For PsychoticFarmerAndLater.)
		<62>	 7/15/91	dba		get rid of MemErr
		<61>	  4/3/91	dba		kill enjoyable part of the code
		<60>	 3/26/91	dba		BAL: add a selector for VisRgnChanged so we don’t burn a whole
									trap
		<59>	 3/14/91	dba		dty, #b6q8crash: check for nil titleHandle before dereferencing
		<58>	  3/4/91	dba		gbm: put in SetWTitle workaround
		<57>	 1/30/91	VL		RLC, #VL003: Invalidate balloon saved bits when creating a new
									layer. Also, add HMGetBalloons check before
									HMInvalidateSavedBits for better performance.
		<56>	 1/30/91	VL		dba, #Whiteboard7.0b4q6HowDoYouLikeThisBugNumberChris?: Use a
									flag for checking whether ancestor regions have already been
									recalc’ed.
		<55>	 1/17/91	VL		(dba) Changed PaintOne to PaintOneLite and PaintBehind to
									PaintBehindLite. These routines do not recalculate ancestor
									rgns.
		<54>	 1/14/91	VL		(stb) Added kIsLayer for FastIsLayer. FastIsLayer used to check
									for negative values but some apps set txSize to negative and
									totally hosed us.
		<53>	  1/3/91	VL		(RLC) Provide a new call RedrawAll for redrawing the whole
									screen.
		<52>	 12/7/90	DC		& dba; Put a sanity check on calls to AutoPositionWindow when
									the where option is lcParentWindow to reposition a partially
									obscured window on the lcParentWindowScreen.
		<51>	 12/6/90	VL		& dba; Added 3 new routines (PreShowHide, ShowWindowAction and
									HideWindowAction). These 3 routines together maintain the
									regions of layers when they become visible and invisible. When a
									layer is becoming invisible, all its children will have their
									structRgn, contRgn and visRgn cleared out. When a layer is
									becoming visible, the regions of its children will be
									recalculated.
		<50>	 12/3/90	VL		& dba; Changed PaintWhite from a Boolean to a short (to match
									what MultiFinder did).
		<49>	 11/1/90	csd		& dba; Add CalcDeskPortVisRgn and call it after creating a new
									window.
		<48>	10/30/90	dba		special case for Finder; don’t calculate visRgns if the visRgn
									is shared with the DeskPort
		<47>	10/24/90	DC		VL - Fix a bug in my inline divide (ShortDiv) that was
									introduced in the conversion to C3.2
		<46>	10/21/90	gbm		(with dba) Made compatible with 3.2 C compiler (and incompatible
									with previous compilers).
		<46>	 10/2/90	JAL		Changing #pragmas to 3.2 version.
		<45>	 9/23/90	dba		Add an ActivatePalette in NewWindowCommon so that newly created
									windows with default palettes will work properly. Maintain the
									DeskPort’s visRgn when painting the desk so that the Finder can
									use it and avoid extraneous region calculations. Also tighten
									code and check it agains the 3.2 C compiler (optimize register
									use for locals).
		<44>	 9/14/90	gbm		Mark layers with the txSize field negative instead of using the
									stringWidth field. This field exists for grafports, too, so we
									no longer mistakenly return true for some grafport owners of
									controls (control owners are supposed to be windows, but tell
									that to SuperCard). Change SetWindowState to no longer accept
									window state handles that are too small, or that have a version
									number of 0 or negative. Get rid of the DeviceLoop declaration,
									and change the call a bit to correspond to the new declaration
									in QuickDraw.h.
		<43>	 9/10/90	dba		move allocation of deskPort (in __InitWindows) before call to
									oldInitWindows
		<42>	 8/31/90	dba		Got rid of locked layers. This allows us to get rid of a lot of
									code, and use FastIsLayer, instead of (slow) WindowType. Since
									the Process Mgr. used a locked layer for the desktop, we have
									created a replacement, the DeskPort. Optimized some of the
									window centering code, including fixing a bug where a region
									would be orphaned.
		<41>	 8/30/90	dba		Extensive changes to speed up the Layer Manager.
		<40>	 8/17/90	RLC		Change interface call to private HMInvalidateSavedBits() in
									used __PaintOne patch.
		<39>	 8/16/90	dba		get rid of extraneous CalcAncestorRgns in PaintBehind
		<38>	  8/2/90	RLC		Insert code in __PaintOne to invalidate bits behind balloon if
									window environment changes.
		<37>	 7/23/90	EMT		Changed IsLayerLocked to WindowType.
		<36>	 7/20/90	dba		get rid of a C warning
		<35>	 7/19/90	EMT		Returned to older style paint and calcvis loops.
		<34>	 7/16/90	EMT		Fixed PaintOnePal accidentally erasing too much. Fixed hiding
									layers in EmptyVisAction.
		<33>	 7/13/90	KSM		Since WindowMgr has a comefrom patch on CheckUpdate, we need to
									make this call the ValidateMenuBar from InvalMenubarPatch.
		<32>	 7/11/90	EMT		Fixed BeginUpdate/EndUpdate reentrancy problem.
		<31>	 7/10/90	dba		get rid of C warnings
		<30>	  7/2/90	EMT		Miscellaneous size and performance improvements: Rewrite
									__NewWindow, __NewCWindow and __DrawNew in 68000. Don’t Call
									PaintOne or CalcVisBehind for invisible windows. Content region
									of layers is now empty instead of "wide open".
		<29>	 6/15/90	DC		Improved time complexity of stagger algorithm
		<28>	 5/30/90	DC		Exported GlobalPortRect
		<27>	 5/22/90	DC		Removed some redundant code in GetNewStructRect. Fixed a bug in
	   								NewWindowCommon caused by calling AutoPositionWindow on a window
									in a half-made state.
		<26>	 4/20/90	dba		make ClipAbove(root) work so that PaintOne(root) will work; note
									that ClipAbove(root) doesn’t need to do anything at all
		<25>	 4/18/90	DC		Change the Dialog centering constant from 3 to 5
		<24>	  4/3/90	DC		Fixed MoveWindow bug in SetWindowState
		<23>	  4/2/90	EMT		Made friends with Palette Manager.
		<22>	 3/20/90	EMT		Rewrote HaveCQD() macro to generate less code.
		<21>	 3/16/90	DC		Added CheckWindow
		<20>	 3/15/90	EMT		Fixed Activate bug when creating layers in NewWindowCommon.
		<19>	 3/14/90	DC		Completed staggering algorithm for PositionWindow
		<18>	 3/14/90	EMT		NewLayer has problems with MakeWindowRecord. Combined to form
									NewWindowCommon.
		<11>	  3/2/90	DC		Merged in Window Positioning code.  Set conditional
									hasWindowPositioning to in line.
		<10>	  3/2/90	JSM		No changes, just check in new version to force BBS to rebuild
									new object.
		 <9>	 2/27/90	EMT		No more AppLayer.
		 <8>	 2/26/90	EMT		Moved bracketed routines to LayerMgrPatchTail.a to save space.
		 <7>	 2/20/90	EMT		Speeded up ClipAbove. Fixed bug in CalcVisAction on invisible
									layers.  Fixed bug in EachWindow when parent->nextWindow == nil.
		 <6>	 2/15/90	EMT		Changed AuxCtlHndl to AuxCtlHandle to match I.M.
		 <5>	 2/15/90	JAL		Changed AuxWinHndl to AuxWinHandle to match I.M.
		 <4>	 2/14/90	EMT		Fix CalcVisAction with embedded layers and speed up
									CalcVisAction and PaintAction to boot! Moved reserved2 bits to
									Layers.h. Added SyncCurLayer. SetCurLayer(nil) now sets the
									current layer to the root. GetParent(nil) now returns CurLayer.
		 <3>	 1/18/90	EMT		Call SetCurLayer(nil) in FrontWindowIn to make sure hierarchy
									remains intact.
		 <2>	12/21/89	EMT		Put back change to CheckUpdate. CheckUpdateIn now checks the
									layer it is passed as well as its children. Rewrote
									FrontWindowIn to stay "in bounds".
	   <2.3>	12/12/89	EMT		Back out change to CheckUpdate until MultiFinder’s ready for it.
	   <2.2>	12/11/89	EMT		Fixed bug in IsLayer when a "window" has no defProc. Added
									CheckUpdateIn, FrontWindowIn and ActiveWindow. Changed
									CheckUpdate to only look in application’s windows.
	   <2.1>	 12/7/89	JSM		Replace code accidentally deleted from __EachWindow.
	   <2.0>	 12/6/89	DAM		Added UNTESTED window positioning code, compiles with
									'hasWindowPositioning' defined.
	   <1.9>	 9/15/89	EMT		Ripped out centering code until it can be done properly. Made
									GetRootLayer survive during early initialization.
	   <1.8>	 8/15/89	EMT		Fixed bug in GetParent when CurLayer is not correct.
	   <1.7>	  7/7/89	EMT		Forgot to check for color in one place…
	   <1.6>	 6/27/89	EMT		Rewrite NewWindow, NewCWindow, NewLayer to use common code and
									splice window into the list before making any window manager or
									defproc calls. Swap AuxWinHead and AuxCtlHead with layers. Added
									Chris Derossi’s centering code.
	   <1.5>	 6/12/89	EMT		Use CallWindowCalc in ROM. Blast PaintWhite in PaintBehind for
									compatibility.
	   <1.4>	  6/9/89	EMT		Enforce window hierarchy in CalcAncestorRgns.
	   <1.3>	  6/5/89	EMT		CloseWindow clears subWindows for layers to prevent KillPicture
									from being called. A better fix than 1.1 for CalcVis and
									CalcVisBehind.
	   <1.2>	 5/31/89	EMT		CheckUpdate now looks at all windows.
	   <1.1>	 5/31/89	EMT		Fixed CalcVisBehind for invisible layers.
	   <1.0>	 5/13/89	EMT		Added to EASE.

	To Do:
*/

#include <Memory.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <Resources.h>
#include <Fonts.h>
#include <Palettes.h>
#include <PalettesPriv.h>
#include <Layers.h>
#include <Traps.h>
#include <Errors.h>
#include <Menus.h>
#include <Balloons.h>
#include <ExpandMemPriv.h>

#ifndef hasLayerlessAppsINIT
#define hasLayerlessAppsINIT 0
#endif

pascal short HMInvalidateSavedBits(WindowPtr)
	= { 0x303C, 0x02FA, _Pack14 };	/* selector = -6, 2 words of parameters */

#pragma parameter VisRgnChanged(__A0)
pascal void VisRgnChanged(WindowPtr)
	= { 0x7000, 0xA0A5 };

#define	ROM85			(* (unsigned short*) 0x28E)
#define	WindowList		(* (WindowPeek*) 0x9D6)
#define	PaintWhite		(* (short*) 0x9DC)
#define	WMgrPort		(* (GrafPtr*) 0x9DE)
#define	DeskPort		(* (GrafPtr*) 0x9E2)
#define	GrayRgn			(* (RgnHandle*) 0x9EE)
#define	SaveVisRgn		(* (RgnHandle*) 0x9F2)
#define	CurActivate		(* (WindowPeek*) 0xA64)
#define	GhostWindow		(* (WindowPeek*) 0xA84)
#define	CurLayer		(* (LayerPeek*) 0xA90)
#define	WMgrUpdate		(* (WindowPeek*) 0xAFC)
#define	ROMMapInsert	(* (short*) 0xB9E)
#define	MBarHeight		(* (short*) 0xBAA)
#define	MMU32Bit		(* (Boolean*) 0xCB2)
#define	AuxWinHead		(* (AuxWinHandle*) 0xCD0)
#define	AuxCtlHead		(* (AuxCtlHandle*) 0xCD4)
#define	WMgrCPort		(* (CGrafPtr*) 0xD2C)
#define MainDevice		(* (GDHandle*) 0x8A4)

#define	HaveCQD()		(ROM85 <= 0x3FFF)

#define	dsWDEFNotFound	87
#define	kDoPaintWhite	-1
#define kDontPaintWhite	0

// These are machine language multiply and divide routines, which are
// used to avoid using CRuntime.o

#pragma parameter __D0 ShortMul(__D1, __D0)
long ShortMul(short, short)
	= 0xC1C1;

#pragma parameter __D0 ShortDiv(__D0, __D1)
short ShortDiv(short, short)
	= 0x81C1;

#define TOPLEFT(aRect) (*((Point *)(&(aRect))))

#if DEBUG
#define	assert(what) do { if (!(what)) DebugStr("\pLayers: Assertion failed!"); } while (0)
#else
#define assert(what)
#endif

/* Philosophy:
 *
 * We can get the ROM Window Manager to do most of the work by carefully maintaining the globals
 * WindowList and GrayRgn.  It is important for all calls to "do the right thing".  The way
 * we can achieve this is to never make assumptions about CurLayer; whenever an operation is
 * performed on a window, great pains are taken to ensure that these globals are correct.
 * The notable exception are a few routines which implicitly take CurLayer as a parameter.
 *
 * The following is a table of routines and their use of these globals.  Routines which
 * implicitly use CurLayer as a parameter are marked with a + following their name.
 * Traps not listed either do not use these globals or call one of the routines below.
 *
 * 					reads WindowList	writes WindowList		reads GrayRgn	writes GrayRgn
 * BringToFront				x					x
 * CalcVis					x											x
 * CheckUpdate+				x
 * ClipAbove				x											x
 * CloseWindow				x					x
 * DragWindow				x(with patch)											x
 * FindWindow+				x
 * FrontWindow+				x
 * GrowWindow															x
 * InitWindows									x									x
 * MoveWindow				x					x						x
 * NewCWindow+				x					x
 * NewWindow+				x					x
 * PaintBehind															x
 * PaintOne																x
 * SelectWindow				x					x
 * SendBehind				x					x
 * SetDeskCPat															x
 * ShowWindow				x
 * ZoomWindow				x					x
 *
 * A simple application using layers might be organized as follows:
 *
 *							_________________________________________________
 *							|	Application Layer (created by InitWindows)	|
 *							|_______________________________________________|
 *										 /					\
 *										/					 \
 *				_________________________________		_____________________________
 *				|	Float Layer (neverActive)	|		|		Document Layer		|
 *				|_______________________________|		|___________________________|
 *						 /			\						 /		  |			\
 *						/			 \						/		  |			 \
 *					Window			Window				Window		Window		Window
 *														^
 *														|
 *														FrontWindow
 */

#define notPaletteUpdate	((void *) -1)

#define	layerWDEF	127
#define	layerProc	layerWDEF*16

#if	!hasLayerlessAppsINIT
pascal void			InitWindowsGuts(void);
#endif
pascal void			ClipAboveGuts(WindowPeek);
pascal void			PaintOneGuts(WindowPeek, RgnHandle);
pascal void			CloseWindowGuts(WindowPtr);
pascal void			InsertWindow(WindowPtr, WindowPtr);
pascal void			MakeDeactiveGlue(WindowPtr);
pascal void			CallWindowGlue(WindowPeek, short, long);
pascal void			CallWindowDrawGlue(WindowPeek);
pascal void			CallWindowCalcGlue(WindowPeek);

pascal void			PaintDesk(void);
pascal void			ValidateMenuBar(void);

#if hasLayerlessApps
pascal LayerPeek	__patchGetParent(WindowPtr);
#define __GetParent	__patchGetParent
#else
pascal void			CalcAncestorRgns(WindowPtr);
pascal LayerPeek	__GetParent(WindowPtr);
#endif

pascal LayerPtr		__GetRootLayer(void);

void				CalcDeskPortVisRgn(void);

struct AutoWindowPosition {
	unsigned short	screenControl:2;
	unsigned short	hPosition:1;
	unsigned short	vPosition:2;
	unsigned short	check:11;
};

typedef struct {
	short	infoVersion;
	Rect	userState;
	Boolean	zoomed;
} WindowStateInfo;
typedef WindowStateInfo *WindowStateInfoPtr;

#define autoPositionTopCookie 0x7FFC


//	ScreenWithMostOF() returns the bounding rectangle of the screen which
//	contains the largest amount of area of the rectangle theRefRect.

struct procData
{
	Rect		theRect,		//	The winning device rect;
				theRefRect;		//	The reference Rect to test against
	long		theArea;		//	High-water mark for eclipsed area
};

pascal void CheckProc(short, short, GDHandle targetDevice, struct procData *theData)
{
	if (targetDevice) {
		Rect	testRect,
				intersectRect;

		testRect = (*targetDevice)->gdRect;

		if (targetDevice == MainDevice)
			testRect.top += MBarHeight;

		if (SectRect(&testRect, &(theData->theRefRect), &intersectRect)) {
			long testArea = ShortMul(intersectRect.right - intersectRect.left,
									intersectRect.bottom - intersectRect.top);

			if (testArea > theData->theArea) {
				theData->theRect = testRect;
				theData->theArea = testArea;
			}
		}
	} else {
		theData->theRect = WMgrPort->portRect;
		theData->theRect.top += MBarHeight;
	}
}

void ScreenWithMostOf(Rect *theRefRect)
{
	struct procData theData;

	theData.theArea = 0;
	theData.theRefRect = *theRefRect;

#ifndef leaveOutDeviceLoop
	DeviceLoop(nil, (DeviceLoopDrawingProcPtr) CheckProc, (long) &theData, singleDevices | allDevices);
#endif

	*theRefRect = theData.theRect;
}

//	TestContent() tests if a the rectangle contentRect has at least 75% of
//	its area on the screen with rectangle screenRect

Boolean TestContent(const Rect *contentRect, const Rect *screenRect)
{
	Rect	sRect;

	if (!SectRect(contentRect, screenRect, &sRect))
		return false;

	{
		unsigned long sArea = ShortMul(sRect.right - sRect.left, sRect.bottom - sRect.top);
		unsigned long cArea = ShortMul(contentRect->right - contentRect->left,
			contentRect->bottom - contentRect->top);

		// is the area that intersects screenRect greater than 3/4 of the content area?
		return (sArea > (cArea - (cArea >> 2)));
	}
}

//	GetNewStructRect takes a WindowPtr and a rectangle and returns the bounding
//	rectangle of the window if its content region were in the area specified by
//	thePort.  It must work even if the window is invisible, which means that the
//	WDEF must be faked out to produce the desired information.

void GetNewStructRect(Rect *newStruct, WindowPeek whichWindow, Rect *thePort)
{
	Rect		aRect;

	Point		oldPos,
				oldSize;

	GrafPtr		curPort;

	Boolean		oldVisibility;

	//	Save the current port and install the window’s (in case they’re not the
	//	same (more than likely))

	GetPort(&curPort);
	SetPort((GrafPtr) whichWindow);

	//	Save the current condition of the window’s GrafPort	because we’re going
	//	to doctor it up considerably to fool the WDEF into giving us its strucRgn.

	oldVisibility = whichWindow->visible;
	whichWindow->visible = true;

	aRect = ((WindowPtr) whichWindow)->portRect;
	oldPos = TOPLEFT(aRect);					//	Get old topLeft;
	LocalToGlobal(&oldPos);						//	Make it global;
	oldSize.h = aRect.right - aRect.left;		//	Get old size;
	oldSize.v = aRect.bottom - aRect.top;

	//	Move the window’s GrafPort to the new location. thePort is already global.

	MovePortTo(thePort->left, thePort->top);

	//	Make it the new size.

	PortSize(thePort->right - thePort->left, thePort->bottom - thePort->top);

	//	call the WDEF with the wCalcRgns message in this new faked up state.

	CallWindowCalcGlue(whichWindow);

	//	get the bounding box of the structure rgn.

	*newStruct = (*(whichWindow->strucRgn))->rgnBBox;

	//	restore things to their old condition.

	MovePortTo(oldPos.h, oldPos.v);
	PortSize(oldSize.h, oldSize.v);
	whichWindow->visible = oldVisibility;

	SetPort(curPort);

	CallWindowCalcGlue(whichWindow);
}

#define staggerIncrement 20
#define initialWindowListSize 100
#define blockSize	(initialWindowListSize * sizeof(WindowPtr))

typedef struct
{
	Rect		checkRect;

	WindowPeek	**backList,
				windowToAvoid;

	int			listSize,
				noOfWindows;
} GetNextPrivateData;

#define slopConstant 3		// amount of slack in deciding whether a window is in position

//	Creates a rectangular around a point.  the area is used to check a window’s position
//	against the point, taking a few pixels of slack into account.

void CreateCheckRect(Rect *checkRect, Point thePoint)
{
	TOPLEFT(*checkRect) = thePoint;
	TOPLEFT(checkRect->bottom) = thePoint;
	InsetRect(checkRect, -slopConstant, -slopConstant);
}

//	returns true if the window is visible and is located within the checkrect contained
//	in privateData (called by FindFirst and GetNextStaggerPos).

int	WindowAtPos(WindowPeek window, GetNextPrivateData *privateData)
{
	if (window->visible)
		return PtInRect(TOPLEFT((*(window->contRgn))->rgnBBox), &(privateData->checkRect));
	else
		return false;
}

//	FindFirst is an ActionProc passed into EachWindow to find the first window that occupies the first
//	stagger position.  FindFirst keeps a "trail" of windows it has traversed in trying to find
//	the first window.  We will use this record to retrace our steps if the first and subsequent
//	stagger positions are occupied.

pascal OSErr FindFirst(WindowPeek window, LayerPeek /* parent */, GetNextPrivateData *privateData)
{
	//	if this is the window we’re trying to position, just ignore it.

	if (window == privateData->windowToAvoid)
		return noErr;

	//	check if this window is in the first stagger position.

	if (WindowAtPos(window, privateData))
		return haltIterator;

	//	if necessary, burst the size of the backward list of windows.

	if (privateData->noOfWindows == privateData->listSize) {
		SetHandleSize((Handle) privateData->backList, privateData->listSize + initialWindowListSize);
		if (MemError() != noErr)
			return haltIterator;
		privateData->listSize += initialWindowListSize;
	}

	//	record the window in our list.

	(*(privateData->backList))[privateData->noOfWindows++] = window;

	return noErr;
}

//	NextPosToCheck moves windCont to the next stagger position in the sequence.  If
//	the window dosen’t fit in any possible next step, NextPosToCheck returns false.

int	NextPosToCheck(Point *curStaggerPos, Rect *windCont, Rect *theScreen)
{
	OffsetRect(windCont, windCont->right - windCont->left + staggerIncrement, 0);	// Try to move one window-width to the right

	if (TestContent(windCont, theScreen))
		return true;

	// if that doesn’t work, move down to the beginning of another row.

	curStaggerPos->h += staggerIncrement;
	curStaggerPos->v += staggerIncrement;
	OffsetRect(windCont, curStaggerPos->h - windCont->left, curStaggerPos->v - windCont->top);

	// if that doesn’t work, we’ve occupied all possible locations and need to start over.

	return TestContent(windCont, theScreen);
}

//	GetNextStaggerPos returns the stagger position into which the newly created window
//	should be placed.

Point GetNextStaggerPos(WindowPeek whichWindow,
					Point firstStaggerPos,
					Rect *theScreen,
					Rect *windCont,
					Point structOffset)
{
	GetNextPrivateData	privateData;		//	structure for use by FindFirst.
	int					noOfWindows;
	Point				curStaggerPos;
	WindowPeek			*backPtr;

	AddPt(structOffset, &firstStaggerPos);	//	adjust upper left of stagger world to
											//	account for the difference between the
											//	content and structure region.

	curStaggerPos = firstStaggerPos;		//	curStaggerPos is the place we are currently
											//	testing for validity.

	//	Move windCont to the first stagger position.

	OffsetRect(windCont, firstStaggerPos.h - windCont->left,
		firstStaggerPos.v - windCont->top);

	//	If it doesn’t fit in the first stagger position, it won’t fit any better anywhere
	//	else.  Might as well place it the beginning.

	if (!TestContent(windCont, theScreen))
		goto out;

	//	create space for the backward list of window ptr generated by FindFirst.

	privateData.backList = (WindowPeek **)NewHandleClear(blockSize);

	//	if we’re real low on memory, we’re stuck with the first stagger position.

	if (MemError() != noErr)
		goto out;

	//	Initialize privateData for call to EachWindow

	privateData.listSize = initialWindowListSize,
	privateData.noOfWindows = 0;
	privateData.windowToAvoid = whichWindow;

	//	Create an area for testing the window’s position.

	CreateCheckRect(&(privateData.checkRect), TOPLEFT(*windCont));

	//	Search all windows in the layer for one that occupies the first stagger position,
	//	keeping a record of all the foreward windows you covered along the way.  If
	//	EachWindow returns noErr, it means there is no window in the first location, so
	//	we’re going to occupy that prime piece of real-estate.

	if (EachWindow(firstWindow, afterLastWindow, nil,
					(WindowAction *)FindFirst, (void *)(&privateData)) == noErr)
		goto cleanup;


	//	Get the results of the search for the window in the first stagger position.
	//	noOfWindows is the number of windows we walked over in trying to find
	//	a window in the first stagger position (minus the one we’re trying to
	//	position and the one in the first position).  backPtr points to the
	//	rear-most member of this list, i.e. the window in front of the first window.

	noOfWindows = privateData.noOfWindows;
	backPtr = *(privateData.backList) + noOfWindows;


	//	The following loop tests each position after the first to see if any window
	//	in our stored path ocupies it.  The first unoccupied position it finds, it
	//	takes.  If no unoccupied positions are found, we assume the screen is full
	//	of windows and we must start at the top of the order.

topOfLoop:
	if (!NextPosToCheck(&curStaggerPos, windCont, theScreen))
		TOPLEFT(*windCont) = firstStaggerPos;
	else {
		CreateCheckRect(&(privateData.checkRect), TOPLEFT(*windCont));
		while (noOfWindows) {
			--backPtr;
			--noOfWindows;
			if (WindowAtPos(*backPtr, &privateData))
				goto topOfLoop;
		}
	}

cleanup:
	DisposHandle((Handle)privateData.backList);		//	clean up the backward list.
out:
	return TOPLEFT(*windCont);						//	return the point to use
													//	in positioning the
													//	window.
}

//	This routine will return a point which can be used to position a window relative to the
//	supplied reference rectangle (in global coords).  The params horizControl and vertControl
//	determine how the window will be positioned.  If 'findScreen' is true, then the rectange
//	is not used directly.  Instead, we find what graphics device contains most of the
//	rectangle, and use the boundary of this screen as the reference rect.

pascal void __PositionWindow(WindowPtr whichWindow,
								const Rect *referenceRect,
								Boolean findScreen,
								HorizontalControlValues horizontalControl,
								VerticalControlValues verticalControl)
{
	Rect	windStruct,			//	bounding box of the window’s structure region
			windCont,			//	bounding box of the window’s content region
			theRefRect;			//	adjusted reference rect.

	Point	thePoint,			//	the position to move the structure region of the window
			structOffset;		//	to offset of structure region from contentregion

	//	get the boundary rect of the windows content region.  We don’t care about what
	//	coordinate system its in.
	windCont = whichWindow->portRect;

	// get the boundary rect of a window with windCont for a portRect
	GetNewStructRect(&windStruct, (WindowPeek)whichWindow, &windCont);

	structOffset = TOPLEFT(windCont);
	SubPt(TOPLEFT(windStruct), &structOffset);

	//	adjust referenceRect for findScreen

	theRefRect = *referenceRect;

	if (findScreen)
		ScreenWithMostOf(&theRefRect);

	thePoint = TOPLEFT(theRefRect);		//	Set default value for thePoint

	if (verticalControl != vcStagger) {
		AddPt(structOffset, &thePoint);

		//	center vertically, if necessary

		if (verticalControl != vcNoVerticalControl) {
			short leftovery = (theRefRect.bottom - theRefRect.top) -
									(windStruct.bottom - windStruct.top);

			if (leftovery < 0)
				leftovery = 0;

			if (verticalControl == vcAlertCenter)
				thePoint.v += ShortDiv(leftovery, lmDialogConstant);
			else
				thePoint.v += (leftovery >> 1);
		}

		//	center horizontally, if necessary

		if (horizontalControl == hcCenter) {
			short leftoverx = (theRefRect.right - theRefRect.left) -
									(windStruct.right - windStruct.left);

			if (leftoverx < 0)
				leftoverx = 0;

			thePoint.h += (leftoverx >> 1);
		}
	} else
		thePoint = GetNextStaggerPos((WindowPeek)whichWindow, thePoint, &theRefRect, &windCont, structOffset);

	// Do the actual positioning

	MoveWindow(whichWindow, thePoint.h,  thePoint.v, false);
}

//	This routine gets the rectangle of the main screen with the menubar chopped off the top

void GetMainScreenRect(Rect *theMainScreenRect)
{
	if (HaveCQD()) {
		GDHandle theGDHand = MainDevice;
		*theMainScreenRect = (*theGDHand)->gdRect;
	} else
		*theMainScreenRect = WMgrPort->portRect;

	theMainScreenRect->top += MBarHeight;
}

//	This routine will position a window in various ways, relative to a rectangle that
//	it computes.  The postion location for where the window should be moved to is
//	returned as a point.  The 'locationControl' param determines how it computes the
//	reference rectangle.  The horizontalControl, and verticalControl params determine
//	how the window will be positioned.

pascal void __AutoPositionWindow(WindowPtr whichWindow, LocationControlValues where,
								HorizontalControlValues horizontalControl,
								VerticalControlValues verticalControl)
{
	Rect		theRect;
	Boolean		lookup;

again:

	lookup = false;

	// location control determines what we send to __PositionWindow as reference Rect

	if (where == lcMainScreen)
		GetMainScreenRect(&theRect);
	else {
		//	Find the window behind the one being positioned.

		WindowPtr parentWind = whichWindow;

		// starting at the one behind and going "toward the desktop", find first visible window.

		while ((parentWind = (WindowPtr) ((WindowPeek)parentWind)->nextWindow) != nil)
			if (((WindowPeek) parentWind)->visible)
				break;

		//	if there is such a window use the bounding box of its structure region

		if (parentWind) {
			theRect = (*(((WindowPeek) parentWind)->strucRgn))->rgnBBox;
			if (where == lcParentWindowScreen)	// use containing screen
				lookup = true;
		} else
			GetMainScreenRect(&theRect);		// if no good windows, use the main screen
	}

	//	Do the positioning

	PositionWindow(whichWindow, &theRect, lookup, horizontalControl, verticalControl);

	//	If the parent option is being used, a sanity check is done
	//	for the caller.

	if ((where == lcParentWindow) && (!CheckWindow(whichWindow, ccContent, true))) {
		where = lcParentWindowScreen;
		goto again;
	}
}

//	This routine returns the portRect of whichWindow in global coordinates.

pascal void __GlobalPortRect(WindowPtr whichWindow, Rect *theRect)
{
	/* Adjust to global coordinates. */
	BitMap* theMap;

	if ((theMap = &whichWindow->portBits)->rowBytes < 0)
		/* It’s a CGrafPort. */
		theMap = *((BitMap**) theMap->baseAddr);

	*theRect = whichWindow->portRect;
	OffsetRect(theRect, -theMap->bounds.left, -theMap->bounds.top);
}


//	IsZoomed determines if the window is zoomable, and if it is, checks whether or not it is in the
//	zoomed state in much the same way that the standard WDEF does it.

Boolean IsZoomed(WindowPtr window)
{
	if (((WindowPeek) window)->spareFlag) {		// check that window has concept of zooming
		Rect	zoomedRect,
				curRect,
				testRect;

		zoomedRect = ((WStateData*) (*((WindowPeek) window)->dataHandle))->stdState;
		GlobalPortRect(window, &curRect);

		InsetRect(&zoomedRect, -7, -7);			// create largest rect that is considered zoomed.

		if (SectRect(&zoomedRect, &curRect, &testRect) && EqualRect(&testRect, &curRect)) {
			// continue if all of the port is within largest rect

			InsetRect(&zoomedRect, 14, 14);		// create smallest rect that is considered zoomed.

			if (SectRect(&zoomedRect, &curRect, &testRect) && EqualRect(&testRect, &zoomedRect))
				return true;
				// if all of small rect is inside currect, return true
		}
	}
	return false;
}


//	GetWindowState returns a handle to data which records the current size of the window as
//	a Rect in global coordinates.  If the window is zoomable and it is zoomed this information
//	is stored and the size recorded is the window’s stored userState.

pascal OSErr __GetWindowState(WindowPtr whichWindow, Handle windState, Size *infoSize)
{
	SetHandleSize(windState, sizeof(WindowStateInfo));
	{
		OSErr e;
		if ((e = MemError()) != noErr)
			return e;
	}

	{
		WindowStateInfoPtr windStatePtr = (WindowStateInfoPtr) *windState;
		char savedState = HGetState(windState);
		HLock(windState);

		windStatePtr->infoVersion = 1;

		if (windStatePtr->zoomed = IsZoomed(whichWindow))
			windStatePtr->userState = ((WStateData*) (*((WindowPeek) whichWindow)->dataHandle))->userState;
		else
			GlobalPortRect(whichWindow, &(windStatePtr->userState));

		if (infoSize)
			*infoSize = sizeof(WindowStateInfo);

		HSetState(windState, savedState);
	}

	return noErr;
}

//	This should be generalized and made public (or at least exported for internal use).  There should be a way of legally
//	querying a window about its nature, i.e. is it draggable, sizeable, modal.  Better still would be a new message for the
//	window def which asks the window to give an account of itself.

pascal short IsDraggable(WindowPtr whichWindow)
{
	short draggable = false;
	short id;

	GetResInfo(((WindowPeek) whichWindow)->windowDefProc, &id, nil, nil);

	//	if this was a hand-stuffed, non-resource-based WDEF, assume it is not draggable

	if (ResError() == noErr) {

		//	if the id of the WDEF was 0 and the the variant was not 1, 2 or 3, we assume that the window is draggable
		//	if the id was 1, then its our old friend, the rounded WDEF and it is draggable.
		//	we assume all other WDEFs to be non-draggable.

		if (id == 0) {
			register short variant = GetWVariant(whichWindow);
			if ((variant < 1) || (variant > 3))
				draggable = true;
		} else if (id == 1)
			draggable = true;
	}

	return draggable;
}

//	SetWindowState takes a handle produced by GetWindowState adjusts a window accordingly.  The window
//	must be valid, but not necessarily visible.  If invisible when the routine is called, the window will
//	be invisible after.  A valid dataHandle field with a valid stdState must available before this routine
//	is called.  If the window was zoomed and the userState is not valid for the current configuration, an
//	error will be generated.

pascal OSErr __SetWindowState(WindowPtr whichWindow, Handle windState)
{
	WindowStateInfo		windStateRec;
	Rect				testRect;

	// Check for valid handle

	if ((whichWindow == nil) || (GetHandleSize(windState) < sizeof(WindowStateInfo)))
		return paramErr;

	windStateRec = * (WindowStateInfoPtr) *windState;

	// Check if version is one we don’t know about

	if (windStateRec.infoVersion != 1)
		return badVersNo;

	// if the saved info indicates a zoomed window and the window isn’t zoomable, its an error

	if (windStateRec.zoomed && !((WindowPeek) whichWindow)->spareFlag)
		return badWindowType;

	//	Test the saved position (or userState if zoomed) of the window to see if it is valid
	//	with the current screen configuration.

	GetNewStructRect(&testRect, (WindowPeek) whichWindow, &windStateRec.userState);		// get bounds of proposed structure region.

	{
		RgnHandle rgn = NewRgn();	// create region for testing visibility on the screen.

		//	if the window is draggable, we’re only testing the drag bar for a valid location

		if (IsDraggable(whichWindow))
			testRect.bottom = testRect.top + 20;	// assume a drag bar in the top 20 pixels of the structure region;

		//	check the structure region (or drag bar) against the screen configuration.

		RectRgn(rgn, &testRect);
		DiffRgn(rgn, GrayRgn, rgn);

		// if 100% of windows structure region or dragbar is not on-screen, return an error

		{
			Boolean empty = EmptyRgn(rgn);
			DisposeRgn(rgn);
			if (!empty)
				return badWindPos;
		}
	}

	//	if the window was in the zoomed state when the windStateInfo was generated, it should be restored
	//	to a zoomed state and the old userState restored from the windStateInfo.  OtherWise, it should simply
	//	be moved to the userState in the windStateInfo.

	if (windStateRec.zoomed) {
		((WStateData *)(*(((WindowPeek)whichWindow)->dataHandle)))->userState = windStateRec.userState;
		testRect = ((WStateData *)(*(((WindowPeek)whichWindow)->dataHandle)))->stdState;
	} else
		testRect = windStateRec.userState;

	MoveWindow(whichWindow, testRect.left, testRect.top, false);
	SizeWindow(whichWindow, testRect.right - testRect.left, testRect.bottom - testRect.top, true);

	return noErr;
}

//	Checks the position of a window according to the checkControl parameter (assumes a rectangular window):
//		ccFrame - checks the entire window, including the structure region.
//		ccDragBar - assumes a dragbar in the top 20 pixels of the structure region bounding box
//		ccContent - checks the content region of the window.
//
//	If entirelyOnScreen is true, CheckWindow will return true only if the entire portion (structure,
//	dragbar or content) is on the screen.  Otherwise, CheckWindow will return true if the portion
//	intersects the visible screen at all.

pascal Boolean __CheckWindow(WindowPtr whichWindow, CheckControlValues checkControl,
	Boolean entirelyOnScreen)
{
	Rect theRect;
	RgnHandle theRgn;
	Boolean rval;

	//	Get the rectangle of the portion of the window we’re concerned about

	if (checkControl < ccDragBar)	// If the we are checking the structure or dragbar, …
	{
		//	Get the bounding box of the structure region.

		if (((WindowPeek)whichWindow)->visible)
			theRect = (*(((WindowPeek)whichWindow)->strucRgn))->rgnBBox;
		else {
			GlobalPortRect(whichWindow, &theRect);
			GetNewStructRect(&theRect, (WindowPeek)whichWindow, &theRect);
		}

		//	if we are checking the dragbar, chop off all but the top 20 pixels.

		if (checkControl == ccDragBar)
			theRect.bottom = theRect.top + 20;
	} else
		GlobalPortRect(whichWindow, &theRect);	// else, get the content.

	theRgn = NewRgn();					//	turn the check rect into a region.
	RectRgn(theRgn, &theRect);

	//	compare the check region to the GrayRgn to determine the return value.

	if (entirelyOnScreen) {
		DiffRgn(theRgn, GrayRgn, theRgn);
		rval = EmptyRgn(theRgn);
	} else {
		SectRgn(theRgn, GrayRgn, theRgn);
		rval = !EmptyRgn(theRgn);
	}

	DisposeRgn(theRgn);		//	Clean up and go…

	return rval;
}

#define kIsLayer	((short) 0xDEAD)
#define FastIsLayer(window) (((GrafPtr) window)->txSize == kIsLayer)

/* Returns true iff window is really a layer. */
pascal Boolean
__IsLayer(WindowPtr window)
{
	return (window != nil) && FastIsLayer(window);
}


/* Returns the window’s "type". */
pascal short
__WindowType(WindowPtr window)
{
	return ((window != nil) && FastIsLayer(window)) ? isLayer : isWindow;
}

/* Creates a new window or layer.  This is common code used by NewWindow, NewCWindow and NewLayer. */
pascal WindowPtr
NewWindowCommon(register Ptr wStorage, register const Rect *boundsRect, const Str255 title,
	register Boolean visible, short theProc, WindowPtr behind, Boolean goAwayFlag, long refCon,
	Boolean isColor)
{
#if hasLayerlessApps
	pascal void HashWindow(WindowPeek theWindow, WindowPeek behind, Boolean visible);
#endif
	GrafPtr		savePort;

	GetPort(&savePort);

	/* Be sure to allocate the storage */
	wStorage = (wStorage == nil) ? NewPtr(sizeof(WindowRecord)) : StripAddress(wStorage);

	/* Zero out all the fields in the window record.
	 * Since WindowRecord is 0x9C long, we can use a fast longword clear.
	 */
	{
		register int countDown;
		register long* longPtr = (long *) wStorage;
		for (countDown = sizeof(WindowRecord) >> 2; countDown > 0; countDown--)
			*(longPtr++) = 0;
	}

	/* Open the correct style port. */
	if (isColor && HaveCQD())
		OpenCPort((CGrafPtr) wStorage);
	else
		OpenPort((GrafPtr) wStorage);

	/* Set the default font. */
	TextFont(applFont);

	{
		/* Get the active window before we splice into the window list. */
		WindowPtr oldFront = ActiveWindow();

		/* Insert it into the window list. */
		if (behind == (WindowPtr) -1) {
			/* Special case - insert it first. */
			((WindowPeek) wStorage)->nextWindow = WindowList;
			WindowList = ((WindowPeek) wStorage);
		} else
			InsertWindow((WindowPtr) wStorage, behind);

		/* Get the WDEF.  If the one we’re looking for doesn’t exist, get id = 0. */
		{
			register short wdefID = theProc >> 4;
			register Handle wdefHandle;

			ROMMapInsert = mapTrue;
			if ((wdefHandle = GetResource('WDEF', wdefID)) == nil) {
				ROMMapInsert = mapTrue;
				if ((wdefHandle = GetResource('WDEF', 0)) == nil)
					SysError(dsWDEFNotFound);
			}
			((WindowPeek) wStorage)->windowDefProc = wdefHandle;

			if (wdefID == layerWDEF) {
				/* Initialize layer-specific fields. */
				((LayerPeek) wStorage)->parent = CurLayer;
				if (HaveCQD()) {
					GetAuxWin(nil, &((LayerPeek) wStorage)->auxWinHead);
					GetAuxCtl(nil, &((LayerPeek) wStorage)->auxCtlHead);
				}
				((GrafPtr) wStorage)->txSize = kIsLayer;	/* kIsLayer text size is the mark of being a layer */
			}
#if TheFuture && !hasLayerlessAppsINIT
			else {
				/* All windows have titles. */
				if (title == nil)
					title = "\p";	/* empty string is the default title */
			}
#endif
		}

#if hasLayerlessApps
		if (((GrafPtr) wStorage)->txSize != kIsLayer)
			HashWindow((WindowPeek)wStorage, (WindowPeek)behind, visible);
#endif

		/* Put the variant in the right place. */
		{
			register unsigned char variant = theProc & 0xF;
			if (HaveCQD() && MMU32Bit) {
				AuxWinHandle awHndl;

				/* Auxiliary window record is mandatory in 32 bit mode. */
				SetWinColor((WindowPtr) wStorage, (WCTabHandle) -1);
				(void) GetAuxWin((WindowPtr) wStorage, &awHndl);
				(*(unsigned char *) &((**awHndl).awFlags)) = variant;
			} else
				(*(unsigned char *) &(((WindowPeek) wStorage)->windowDefProc)) = variant;
		}

		/* Initialize windowKind, visible, goAwayFlag, refCon */
		((WindowPeek) wStorage)->windowKind = userKind;
		((WindowPeek) wStorage)->visible = visible;
		((WindowPeek) wStorage)->goAwayFlag = goAwayFlag;
		((WindowPeek) wStorage)->refCon = refCon;

		/* Allocate 3 regions. */
		((WindowPeek) wStorage)->strucRgn = NewRgn();
		((WindowPeek) wStorage)->contRgn = NewRgn();
		((WindowPeek) wStorage)->updateRgn = NewRgn();

		/* Get the new active window, and make activate/deactivate events if necessary. */
		{
			WindowPtr newFront = ActiveWindow();
			if (newFront != oldFront) {
				if (oldFront != nil)
					MakeDeactiveGlue(oldFront);
				if (newFront != nil)
					((WindowPeek) newFront)->hilited = true;
					CurActivate = (WindowPeek) newFront;
			}
		}
	}

	{
		/* Finish setting up the port before doing anything else. */
		Boolean	autoPosition = false;
		if (boundsRect != nil) {
			/* Adjust the portRect first. */
			if ((boundsRect->top == autoPositionTopCookie) && ((boundsRect->left & 0x7FF) == 0xA)) {
				autoPosition = true;
				MovePortTo(0, 0);
				PortSize(boundsRect->right, boundsRect->bottom);
			} else {
				MovePortTo(boundsRect->left, boundsRect->top);
				PortSize(boundsRect->right - boundsRect->left, boundsRect->bottom - boundsRect->top);
			}
		}

		/* Go to the window manager port for string width calculation. */
		SetPort(WMgrPort);

		if (title != nil) {
			/* Do the title. */
			((WindowPeek) wStorage)->titleHandle = NewString(title);
			((WindowPeek) wStorage)->titleWidth = StringWidth(title);
		}

		CallWindowGlue((WindowPeek) wStorage, wNew, 0);

		if (autoPosition) {
			struct	AutoWindowPosition *info = (struct AutoWindowPosition *) &(boundsRect->left);

			//	This window is actually invisible since its regions are zeroed out and it has
			//	not been displayed yet.  Thus we make the visible flag reflect this fact for
			//	so that AutoPositionWindow (which does not expect a window to be in a limbo
			//	state) will do the right thing.

			((WindowPeek) wStorage)->visible = false;

			AutoPositionWindow((WindowPtr) wStorage, info->screenControl,
				info->hPosition, info->vPosition);

			//	Restore the flag to its "correct" state.

			((WindowPeek) wStorage)->visible = visible;
		}
	}

	/* Calculate the structure and content of this window or layer.
	 * If it is a visible window, it can also affect the structure of the layers it is contained in
	 * and needs to be drawn and obscure other windows.
	 */
	CallWindowCalcGlue((WindowPeek) wStorage);
	if (visible && !FastIsLayer(wStorage)) {
		ActivatePalette((WindowPtr) wStorage);
		PaintOne((WindowPeek) wStorage, ((WindowPeek) wStorage)->strucRgn);
		CalcVisBehind((WindowPeek) wStorage, ((WindowPeek) wStorage)->strucRgn);
		CalcDeskPortVisRgn();
	}

	SetPort(savePort);
	
	return (WindowPtr) wStorage;
}


/* Creates a layer. */
pascal OSErr
__NewLayer(LayerPtr *layer, Boolean visible, Boolean neverActive, WindowPtr behind, long refCon)
{
	*layer = NewWindowCommon((Ptr) *layer, nil, nil, visible, layerProc, behind, neverActive, refCon, true);
	if (HMGetBalloons())
		HMInvalidateSavedBits((WindowPtr) *layer);
	return noErr;
}


/* Gets the current layer. */
pascal LayerPtr
__GetCurLayer(void)
{
	return (LayerPtr) CurLayer;
}


/* Synchronizes the old Window Manager globals with the current layer. */
pascal void
SyncCurLayer(void)
{
	LayerPeek curLayer;

	if ((curLayer = CurLayer) != nil) {
		if (HaveCQD()) {
			curLayer->auxWinHead = AuxWinHead;
			curLayer->auxCtlHead = AuxCtlHead;
		}
		curLayer->subWindows = WindowList;
	}
}


/* Sets the current layer.  Note that WindowList always contains the children of the current layer. */
pascal void
__SetCurLayer(LayerPtr layer)
{
	register LayerPeek newLayer;

	/* Reassert child pointer and auxiliary lists in case they changed. */
	SyncCurLayer();

	if ((newLayer = (LayerPeek) layer) == nil)
		newLayer = (LayerPeek) GetRootLayer();

	if (newLayer != nil) {		/* Needed because GetRootLayer() returns nil during InitWindows. */
		assert(IsLayer((LayerPtr) layer));

		/* Change the current layer. */
		CurLayer = newLayer;

		/* Set up the window and auxiliary lists. */
		if (HaveCQD()) {
			AuxWinHead = newLayer->auxWinHead;
			AuxCtlHead = newLayer->auxCtlHead;
		}
		WindowList = newLayer->subWindows;
	}
}


/* Sets the current layer.  Returns the previous current layer.  Does not use traps for speed. */
pascal LayerPtr
__SwapCurLayer(LayerPtr layer)
{
	LayerPtr saveLayer = (LayerPtr) CurLayer;

	/* Replace with old value. */
	__SetCurLayer(layer);

	return saveLayer;
}


/* Gets the desk port. */
pascal OSErr
__GetDeskPort(GrafPtr* result)
{
	*result = DeskPort;
	return noErr;
}


/* Gets the children of a layer. */
pascal WindowPeek
__GetSubWindows(LayerPtr layer)
{
	if (IsLayer(layer))
		return ((LayerPeek) layer)->subWindows;
	else
		return nil;
}


/* This iterates over the hierarchy. It starts with start and calls the action procedure for each
 * object (layer or window), up to but not including stop in the layer denoted by parent.
 * Nil and -1 are valid for start and stop, nil meaning after the last window in parent, and
 * -1 meaning the first window in parent.    If stop is -1, the iteration "wraps", causing the
 * iteration to continue to the end of the hierarchy (useful for xxxBehind traps).  If parent is
 * nil, EachWindow will use start’s parent, or CurLayer if start is nil or -1.
 */
pascal OSErr
__EachWindow(WindowPeek start, WindowPeek stop, LayerPeek parent, WindowAction *action, void *privateData)
{
	/* Since EachWindow requires that the hierarchy remains intact, reassert the
	 * child pointer in case it changed.
	 */
	SyncCurLayer();

	/* Check for nil parent. */
	if (parent == nil) {
		if ((start == afterLastWindow) || (start == firstWindow))
			parent = CurLayer;
		else
			parent = __GetParent((WindowPtr) start);
	}
	assert(IsLayer(parent));

	/* Check for firstWindow start. */
	if (start == firstWindow)
		start = __GetSubWindows((LayerPtr) parent);

	/* Check for afterLastWindow or afterLastLayer stop. */
	if (stop == afterLastWindow)
		stop = (WindowPeek) parent;
	else if (stop == afterLastLayer)
		stop = nil;

	for (;;) {
		while ((start == nil) && (parent != nil)) {
			/* We’ve just finished searching a family with no luck.  Pop up a level.
			 * The next child to be searched is Dad’s little sister.
			 */
			if ((WindowPeek) parent == stop)
				/* We’re done. */
				return noErr;
			start = parent->nextWindow;
			parent = parent->parent;
		}

		if ((start == stop) || (start == nil))
			/* We’re done looking. */
			return noErr;

		{
			/* If we found the little critter, reunite the family. */
			OSErr result = (*action)(start, parent, privateData);
			if ((result != noErr) && (result != skipChildren))
				return result;

			/* If the kid is a layer, he might have kids of his own.
			 * Search his family unless he has the plague.
			 */
			if ((result != skipChildren) && FastIsLayer(start)) {
				parent = (LayerPeek) start;
				start = parent->subWindows;
				/* Note that we are now searching a new family. */
			} else {
				/* Advance to the next sibling. */
				start = start->nextWindow;
			}
		}
	}

	/* Not reached due to for (;;) above. */
}


#if !hasLayerlessApps

/* ParentSearch is an action procedure that finds a specific window’s parent.
 * It always descends into sub-trees.
 */
pascal OSErr
ParentSearch(WindowPeek window, LayerPeek parent, WindowPtr *target)
{
	if (window == (WindowPeek) *target) {
		*target = (WindowPtr) parent;
		return haltIterator;
	} else
		return noErr;
}

/* Finds the parent of the given window. */
pascal LayerPeek
__GetParent(WindowPtr window)
{
	if (window == nil)
		/* By Imperial decree, the parent of nil is CurLayer. */
		return CurLayer;
	else if (FastIsLayer(window)) {
		/* Layers and Earth girls are easy. */
		return ((LayerPeek) window)->parent;
	} else {
		/* Look high and low... */
		LayerPeek	parent = (LayerPeek) window;

		/* Start looking in Curlayer. */
		if (EachWindow(firstWindow, afterLastLayer, nil, ParentSearch, &parent) == noErr)
			/* If not there, try the rest of the hierarchy. */
			if (EachWindow(firstWindow, (WindowPeek) CurLayer, (LayerPeek) __GetRootLayer(), ParentSearch, &parent) == noErr)
				/* If not there, give up. */
				return nil;

		return parent;
	}
}
#endif

/* Finds the root layer.  Returns nil if there is none (during initialization). */
pascal LayerPtr
__GetRootLayer(void)
{
	LayerPtr	layer, parent;

	/* Start with the current layer. */
	if ((layer = (LayerPtr) CurLayer) == nil)
		return nil;

	/* The root is the only layer that doesn’t have a parent. */
	while ((parent = (LayerPtr) ((LayerPeek) layer)->parent) != nil)
		layer = parent;

	return layer;
}


/* Returns true iff artifact is an ancestor of or equal to child. */
Boolean
IsAncestor(WindowPtr artifact, LayerPtr child)
{
	assert(IsLayer(child));

	do
		if (child == artifact)
			return true;
	while ((child = (WindowPtr) ((LayerPeek) child)->parent) != nil);

	return false;
}
  
/* Initializes the layer and window managers. */
pascal OSErr
__InitLayers(void)
{
	CurLayer = nil;

	/* Create the DeskPort, which is a port that has an updateRgn. */
	{
		register GrafPtr deskPort = (GrafPtr) NewPtrClear(sizeof(WindowRecord));
		OpenPort(deskPort);
		((WindowPeek) deskPort)->updateRgn = NewRgn();
		DeskPort = deskPort;
	}

	/* Call the old InitWindows. */
#if hasLayerlessAppsINIT
	InitWindows();
#else
	InitWindowsGuts();
#endif

	/* Create the two layers (one for system stuff). */
	{
		short i;
		for (i = 0; i < 2; i++) {
			LayerPtr layer = nil;
			(void) __NewLayer(&layer, true, false, (WindowPtr) -1, 0);
			SetCurLayer(layer);
		}
	}

	return noErr;
}

/* DrawWindowPic draws the window picture in a window. */
pascal void
DrawWindowPic(WindowPeek window)
{
	PicHandle	windowPic = window->windowPic;
	Rect		picFrame = (**windowPic).picFrame;
	GrafPtr		savePort;
	RgnHandle	saveClip = NewRgn();

	GetPort(&savePort);
	SetPort(&window->port);
	GetClip(saveClip);
	ClipRect(&window->port.portRect);

	BeginUpdate((WindowPtr) window);
	DrawPicture(windowPic, &picFrame);
	EndUpdate((WindowPtr) window);

	SetClip(saveClip);
	DisposeRgn(saveClip);
	SetPort(savePort);
}

#if !hasLayerlessApps
/* Tells all ancestor layers to calculate their regions. */
pascal void
CalcAncestorRgns(WindowPtr window)
{
	/* Since CalcAncestorRgns requires that the hierarchy remains intact, reassert the
	 * child pointer in case it changed.
	 */
	SyncCurLayer();

	{
		LayerPeek layer = __GetParent(window);
		while (layer != nil) {
			CallWindowCalcGlue((WindowPeek) layer);
			layer = layer->parent;
		}
	}
}
#endif


/* CheckUpdateSearch is an action procedure used by CheckUpdate that finds the frontmost
 * window or layer that needs updating.  It will update any windowPics automatically.
 * It always descends into sub-trees, but skips invisible layers and windows.
 */
pascal OSErr
CheckUpdateSearch(WindowPeek window, LayerPeek, WindowPeek *updateWindow)
{
	if (!window->visible)
		return skipChildren;

	if (EmptyRgn(window->updateRgn))
		return noErr;

	if (!FastIsLayer((WindowPtr) window) && (window->windowPic != nil)) {
		DrawWindowPic(window);
		return noErr;
	}

	*updateWindow = window;
	return haltIterator;
}


/* Called by the Event Manager to look for update events.  Will fill out event record and
 * return true if a window is found that needs to be updated.  Checks for updates in the
 * layer given and all its children.
 */
pascal Boolean
__CheckUpdateIn(EventRecord *event, LayerPtr inLayer)
{
	register Boolean result = false;

	ValidateMenuBar();

	if ((EachWindow((WindowPeek) inLayer, ((WindowPeek) inLayer)->nextWindow,
		((LayerPeek) inLayer)->parent, CheckUpdateSearch, &event->message) != noErr)) {
		/* Make an update event and return true. */
		event->what = updateEvt;
		result = true;
	}

	return result;
}


/* Called by others to look for update events.  Replaces the routine in ROM.
 * Changes include looking for layers and only examining WindowPic if it is not one.
 * CheckUpdate only searches in the application layer.
 */
pascal Boolean
__CheckUpdate(EventRecord *event)
{
	return __CheckUpdateIn(event, (LayerPtr) CurLayer);
}


/* If window is a layer, BeginUpdate will correctly paint and calculate visRgns for its children. */
pascal void
BeginUpdateOfLayersChildren(WindowPtr window)
{
	if (FastIsLayer(window)) {
		PaintOne((WindowPeek) window, ((WindowPeek) window)->updateRgn);
		/* Note that only the children of window really need CalcVis. */
		CalcVis((WindowPeek) window);
	}
}

#if !hasLayerlessApps
/* Sets the clipRgn of the Window Manager port to be the desktop intersected with the current
 * clipRgn, minus the structure regions of all the windows in front of the given window.  If
 * window is nil, all windows in the current layer are "in front".  It makes use of the old code
 * in ROM.
 */
pascal void
__ClipAbove(WindowPeek window)
{
	LayerPtr	saveLayer = (LayerPtr) CurLayer;
	LayerPtr	parent = (LayerPtr) __GetParent((WindowPtr) window);

	while (parent != nil) {
		if (!((LayerPeek) parent)->visible) {
			/* Parent is invisible, set the clip to the empty region and get out! */
			SetEmptyRgn(WMgrPort->clipRgn);
			break;
		}

		/* Call the old ClipAbove (set the current layer for it). */
		__SetCurLayer(parent);
		ClipAboveGuts(window);

		/* Jump up a layer. */
		window = (WindowPeek) parent;
		parent = (LayerPtr) ((LayerPeek) parent)->parent;
	}

	/* Restore the current layer. */
	__SetCurLayer(saveLayer);
}
#endif

#if !hasLayerlessApps
/* Recalculate the visRgn of the DeskPort. */
void
CalcDeskPortVisRgn(void)
{
	register GrafPtr wMgrPort = WMgrPort;
	register RgnHandle deskPortVisRgn = (long) DeskPort->visRgn;
	RgnHandle wMgrPortClipRgn = wMgrPort->clipRgn;

	/* Recalculate the visible part of the desktop.
	 * Note that we use ClipAbove, which works on the WMgrPort’s clipRgn directly.
	 * Start by opening the region wide, and putting it in the WMgrPort’s clipRgn.
	 */
	CopyRgn(GrayRgn, deskPortVisRgn);
	wMgrPort->clipRgn = deskPortVisRgn;
	/* Now call ClipAbove(nil) at the root. */
	{
		register long saveLayer = (long) SwapCurLayer(nil);	/* get a D register (SLIME) */
		ClipAbove(nil);
		SetCurLayer((LayerPtr) saveLayer);
	}
	/* Finally, restore the WMgrPort’s clipRgn.. */
	wMgrPort->clipRgn = wMgrPortClipRgn;
}
#endif

#if hasLayerlessApps
void PaintTheDesk(RgnHandle clobberedRgn);
#else
/* Call this to update the desktop when part of the desktop is clobbered. */
void
PaintTheDesk(RgnHandle clobberedRgn)
{
	register GrafPtr wMgrPort = WMgrPort;
	register GrafPtr deskPort = DeskPort;
	register RgnHandle wMgrPortClipRgn = wMgrPort->clipRgn;

	CalcDeskPortVisRgn();
	/* Set up clipping of the WMgrPort to intersection of what was clobbered and visible. */
	SectRgn(clobberedRgn, deskPort->visRgn, wMgrPortClipRgn);
	/* Accumulate visible clobbered areas into the DeskPort’s updateRgn. */
	{
		register RgnHandle deskUpdate = ((WindowPeek) deskPort)->updateRgn;
		UnionRgn(deskUpdate, wMgrPortClipRgn, deskUpdate);
	}
	/* Draw gray, and leave the clipRgn set, for compatibility with old apps. that expect it.
	 * Use the WMgrCPort on CQD machines, so that we can draw color patterns.
	 */
	if (HaveCQD())
		wMgrPort = (GrafPtr) WMgrCPort;
	SetPort(wMgrPort);
	PaintDesk();
}
#endif

typedef struct {
	WindowPtr	frontWindow;
	RgnHandle	clobberedRgn;
} PaintInfoRec, *PaintInfoPtr;

/* PaintAction is an action procedure that doesn’t find anything.
 * It paints the window, clipped to clobberedRgn.
 */
pascal OSErr
PaintAction(WindowPeek window, LayerPeek, PaintInfoPtr paintInfo)
{
	if (!window->visible)
		return skipChildren;

	if ((!FastIsLayer(window)) &&
		((paintInfo->frontWindow == notPaletteUpdate) ||
		 ((*WhatPal((WindowPtr) window))->pmPrivate &
		  ((window == paintInfo->frontWindow) ? CForeMask : CBackMask)))) {

		/* Call the old PaintOne for real windows only in normal loops and when a palette update
		 * is necessary.
		 */
		PaintOneGuts(window, paintInfo->clobberedRgn);
	}

	return noErr;
}

#if !hasLayerlessApps
/* <56> CalcAncestorRgns for the normal case. If the one-shot in ExpandMem is set, that
 * means that some other call has already calculated the ancestor regions, so we can be
 * faster by not recalculating them.
 */
void CalcAncestorRgnsForPaintActions(WindowPtr window)
{
	ExpandMemRec *emRec = ExpandMem;
	if (GetExpandMemAncestorRgnsCalced() == 0)
		CalcAncestorRgns(window);		// calculate the regions if the one-shot is off
	else
		SetExpandMemAncestorRgnsCalced(0);		// reset the one-shot
}
#endif

/* Paints the given window, and any children, clipped to clobberedRgn. */
/* <55> VL 	PaintOne calls CalcAncestorRgns before calling PaintOneLite. */
pascal void
__PaintOne(WindowPeek window, RgnHandle clobberedRgn)
{
#if !hasLayerlessApps
	CalcAncestorRgnsForPaintActions((WindowPtr) window);
#endif

	if (window == nil) {
		PaintTheDesk(clobberedRgn);
	} else {
		PaintInfoRec	paintInfo;

		if (HMGetBalloons())
			HMInvalidateSavedBits((WindowPtr) window);

		paintInfo.frontWindow = notPaletteUpdate;
		paintInfo.clobberedRgn = clobberedRgn;
		(void) EachWindow(window, window->nextWindow, nil, PaintAction, &paintInfo);
	}
}

#if !hasLayerlessApps
/* Paints the given window, and any children, and all windows behind the given window, even
 * in layers behind its parent, and the desktop, clipped to clobberedRgn.  If startWindow is
 * nil, it paints all windows behind CurLayer.
 */
/* <55> VL   PaintBehind calls CalcAncestorRgns before calling PaintBehindLite. */
pascal void
__PaintBehind(WindowPeek startWindow, RgnHandle clobberedRgn)
{
	PaintInfoRec	paintInfo;

	CalcAncestorRgnsForPaintActions((WindowPtr) startWindow);

	PaintTheDesk(clobberedRgn);

	paintInfo.frontWindow = notPaletteUpdate;
	paintInfo.clobberedRgn = clobberedRgn;
	(void) EachWindow(startWindow, afterLastLayer, nil, PaintAction, &paintInfo);

	/* Blast PaintWhite to be compatible. */
	PaintWhite = kDoPaintWhite;
}
#endif


/* Similar to PaintOne, but only paints windows that have the proper Palette Manager flags.
 */
pascal void
__PaintOnePal(WindowPeek window, RgnHandle clobberedRgn)
{
	PaintInfoRec	paintInfo;
	short			savePaintWhite = PaintWhite;

	if (window == nil) {
		/* In this case, we’re painting everything. */
		PaintTheDesk(clobberedRgn);
		window = (WindowPeek) GetRootLayer();
	}

	/* Never PaintWhite in palette update. */
	PaintWhite = kDontPaintWhite;
	paintInfo.frontWindow = ActiveWindow();
	paintInfo.clobberedRgn = clobberedRgn;
	(void) EachWindow(window, window->nextWindow, nil, PaintAction, &paintInfo);
	PaintWhite = savePaintWhite;
}


/* UpdatedVisRgn is called whenever a visRgn has been changed. */
pascal void
UpdatedVisRgn(WindowPeek window)
{
	VisRgnChanged((WindowPtr) window);

	if (window == WMgrUpdate) {
		/* We’re changing the visRgn of a window inside BeginUpdate/EndUpdate.
		 * Invalidate SaveVisRgn.
		 */
		WMgrUpdate = nil;
		SetEmptyRgn(SaveVisRgn);
	}
}

#if !hasLayerlessApps

/* EmptyVisAction is an action procedure that doesn’t find anything.  It sets the visRgn
 * of all its windows to be empty.  It skips the children of all layers whose visRgn is
 * already empty as an optimization.
 */
pascal OSErr
EmptyVisAction(WindowPeek window, LayerPeek, void*)
{
	if (!FastIsLayer(window)) {
		RgnHandle myVis = window->port.visRgn;
		if (myVis != DeskPort->visRgn) {
			SetEmptyRgn(myVis);
			UpdatedVisRgn(window);
		}
	}

	return noErr;
}

/* CalcVisAction is an action procedure that doesn’t find anything.  It recalculates the visRgn
 * for any window that intersects the clobberedRgn.
 */
pascal OSErr
CalcVisAction(WindowPeek window, LayerPeek parent, RgnHandle clobberedRgn)
{
	if (!window->visible) {
		/* If it’s invisible, don’t waste time, just make all the visRgns empty. */
		(void) EachWindow(window, window->nextWindow, parent, EmptyVisAction, nil);
		return skipChildren;
	}

	if ((clobberedRgn == nil) || RectInRgn(&(**window->contRgn).rgnBBox, clobberedRgn)) {
		/* If the clobbered region intersects then recalculate the visRgn. */
		register long myVis = (long) window->port.visRgn;			/* get a D register (SLIME) */

		if ((RgnHandle) myVis != DeskPort->visRgn) {
			{
				/* Borrow ClipAbove technology to calculate the visRgn. */
				register RgnHandle* wMgrPortClipRgnPtr = &WMgrPort->clipRgn;
				register long saveClip = (long) *wMgrPortClipRgnPtr;	/* get a D register (SLIME) */
				CopyRgn(window->contRgn, (RgnHandle) myVis);
				*wMgrPortClipRgnPtr = (RgnHandle) myVis;
				ClipAbove(window);
				*wMgrPortClipRgnPtr = (RgnHandle) saveClip;
			}

			{
				/* Adjust to local coordinates. */
				BitMap* theMap;
				if ((theMap = &window->port.portBits)->rowBytes < 0)
					/* It’s a CGrafPort. */
					theMap = *((BitMap **) theMap->baseAddr);
				OffsetRgn((RgnHandle) myVis, theMap->bounds.left, theMap->bounds.top);
			}

			UpdatedVisRgn(window);
		}
	}
	else if (FastIsLayer(window) && !RectInRgn(&(**window->strucRgn).rgnBBox, clobberedRgn))
		/* Don’t waste time looking at children if structure doesn’t intersect. */
		return skipChildren;

	return noErr;
}

/* Calculates the visRgn for window and any children. */
pascal void
__CalcVis(WindowPeek window)
{
	if (window != nil)
		(void) EachWindow(window, window->nextWindow, nil, CalcVisAction, nil);
}


/* Calculates the visRgn for window and all windows behind, even in layers behind its parent
 * and any children that intersect clobberedRgn.  If startWindow is nil, it calculates the
 * visRgn for all windows behind CurLayer that intersect.
 */
pascal void
__CalcVisBehind(WindowPeek startWindow, RgnHandle clobberedRgn)
{
	if (startWindow != nil) {
		/* Always CalcVis the startWindow and go to next one. */
		__CalcVis(startWindow);
		startWindow = startWindow->nextWindow;
	}

	(void) EachWindow(startWindow, afterLastLayer, nil, CalcVisAction, clobberedRgn);
}

#endif

#if !hasLayerlessApps
/* Determines which region of which window a point lies inside.  Descends layers until a window is
 * found.  Starts with the current subtree.
 */
pascal short
__FindWindow(Point point, WindowPtr *window)
{
	/* Save the current layer. */
	LayerPtr	saveLayer = (LayerPtr) CurLayer;
	short		result;

	/* Loop indefinitely until break is used. */
	for (;;) {
		/* Call the old FindWindow. */
		result = FindLayer(point, window);

		/* We’re done if it’s not in a layer. */
		if ((result != inContent) || !FastIsLayer(*window))
			break;

		/* It’s a layer, go down and try again. */
		__SetCurLayer((LayerPtr) *window);
	}

	/* Restore the current layer. */
	__SetCurLayer(saveLayer);

	return result;
}
#endif

/* Returns the frontmost active window in the given layer.  Note that FrontWindowIn, like
 * FindWindow, only descends into subtrees, and never leaves them; so EachWindow is not usable here.
 */
pascal WindowPtr
__FrontWindowIn(LayerPtr layer)
{
	/* Since FrontWindowIn requires that the hierarchy remains intact, reassert the
	 * child pointer in case it changed.
	 */
	SyncCurLayer();

	/* Check the window list, descending into each candidate.  If we find a window, break
	 * out of the loop.
	 */
	{
		WindowPeek window = __GetSubWindows(layer);
		while (window != nil) {
			if (window->visible && (window != GhostWindow)) {
				if (!FastIsLayer((WindowPtr) window))
					/* It’s not a layer, so it must be the right one. */
					break;
				if (!((LayerPeek) window)->neverActive) {
					/* It’s a normal layer (not neverActive), so descend and continue. */
					window = __GetSubWindows((LayerPtr) window);
					continue;
				}
			}
			/* Otherwise, just go on to the next window. */
			window = window->nextWindow;
		}

		return (WindowPtr) window;
	}
}

/* Returns the frontmost active window in the application layer. */
pascal WindowPtr
__FrontWindow(void)
{
	return __FrontWindowIn((LayerPtr) CurLayer);
}

/* Returns the frontmost active window. */
pascal WindowPtr
__ActiveWindow(void)
{
	return __FrontWindowIn(GetRootLayer());
}

/* Bracketed to ensure layer is correct.  If this window is a layer, its children will
 * be orphaned.
 */
pascal void
__CloseWindow(WindowPtr window)
{
	/* Save the current layer. */
	LayerPtr saveLayer = __SwapCurLayer((LayerPtr) __GetParent(window));

	if (FastIsLayer(window)) {
#if hasLayerlessApps
		//	We do this now because the old CloseWindow will do it eventually and when
		//	it dose it, auxWindHead will be NULL etc., which will cause crashes in
		//	ShowHideLayer.
		
		ShowHide(window, false);
#endif
		/* CloseWindow can orphan saveLayer.  If this is about to happen, correct, by not
		 * restoring the current layer.
		 */
		if (IsAncestor(window, saveLayer))
			saveLayer = (LayerPtr) CurLayer;

		/* Clear out title handle so we don’t crash.  Layers use this as auxWinHead. */
		((WindowPeek) window)->titleHandle = nil;

		/* Clear out control list so we don’t crash.  Layers use this as auxCtlHead. */
		((WindowPeek) window)->controlList = nil;

#if !hasLayerlessApps
		//	we will do this in our come-from patch DisposeRgn in the rom CloseWindow
		
		/* Set subWindows to nil so that KillPicture isn’t called.
		 * If any of the children are also layers, their parent pointers will be invalid,
		 * but you’re not supposed to call CloseWindow on a layer in this case anyway.
		 */
		((LayerPeek) window)->subWindows = nil;
#endif
	}

	/* Call the old CloseWindow. */
	CloseWindowGuts(window);

	/* Restore the current layer. */
	__SetCurLayer(saveLayer);
}

#if !hasLayerlessApps

/* Empty out the structRgn, contRgn and visRgn as the layer to which the window belongs is becoming invisible. */
pascal OSErr
HideWindowAction(WindowPeek window, LayerPeek, PaintInfoPtr)
{
	SetEmptyRgn(window->strucRgn);
	SetEmptyRgn(window->contRgn);
	SetEmptyRgn(((GrafPtr) window)->visRgn);

	return noErr;
}

/* Calculate the structRgn and contRgn of the window as the layer to which it belongs is becoming visible.	*/
/* The visRgn is not calculated here.  But it will be calculated in ShowHide where CalcVBehind is called 	*/
pascal OSErr
ShowWindowAction(WindowPeek window, LayerPeek layer, PaintInfoPtr)
{
	if (!window->visible)
		return skipChildren;
	if (!FastIsLayer(window)) {
		CallWindowCalcGlue(window);
		if (window->nextWindow == nil)					/* calculate parent only after the last child of a layer */
			CallWindowCalcGlue((WindowPeek) layer);
	}
	return noErr;
}

/* This routine is called by the ShowHide patch to fix up the regions if the object to be shown/hidden is a layer */
pascal void
PreShowHide(WindowPtr window, Boolean showFlag)
{
	if (FastIsLayer(window)) {
		EachWindow(firstWindow,afterLastWindow,(LayerPeek) window,(showFlag ? ShowWindowAction : HideWindowAction),nil);
		CalcAncestorRgns(window);
	}
}

#endif

/* This routine is a local routine used to redraw a region. All the windows from the root layer will be refreshed. */
pascal void
Redraw(RgnHandle region)
{
	GrafPtr				origPort;
	register GrafPtr	wmgrPort = WMgrPort;
	EventRecord			discardEvent;
	Rect				menuBarRect;

	PaintBehind((WindowPeek) __GetRootLayer(), region);

	GetPort(&origPort);
	SetPort(wmgrPort);

	SetRect(&menuBarRect, wmgrPort->portRect.left, wmgrPort->portRect.top,  wmgrPort->portRect.right, wmgrPort->portRect.top+MBarHeight);
	if (SectRect(&((**region).rgnBBox), &menuBarRect, &menuBarRect))
		InvalMenuBar();

	/* Here is a hack to get help windows updated. Since Process Mgr. uses CheckUpdate only on the appl layer, 	*/
	/* it never passes updates to the system windows (e.g. help windows).										*/
	/* This CheckUpdateIn doesn’t really work unless the system windows use the WindowPic updating scheme. 		*/
	(void) __CheckUpdateIn(&discardEvent, __GetRootLayer());

	SetPort(origPort);
}

/* This routine redraws all the windows in the layer structure. */
pascal void
__RedrawAll(void)
{
	RgnHandle	wideOpenArea = NewRgn();
	SetRectRgn(wideOpenArea, -32768,  -32768,  32767, 32767);
	Redraw(wideOpenArea);
	DisposeRgn(wideOpenArea);
}
