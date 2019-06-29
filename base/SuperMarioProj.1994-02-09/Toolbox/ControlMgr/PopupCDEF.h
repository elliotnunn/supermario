/*
	File:		PopupCDEF.h

	Contains:	Header for Popup CDEF

	Written by:	Byron Han

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<20>	 11/7/92	JDR		Removed the PopupPrivateData record, which is really publicly
									known about.
		<19>	 5/21/92	gbm		#1030284,<dty>: Take popup font size change for Cube-E
		<18>	 5/10/92	BH		added definition for structure WMgrPortState and prototypes for
									SaveAndResetWMPorts() and ResetoreWMPorts() under TheFuture
									conditional
		<17>	 8/22/91	KSM		PKE,#85693-Bruges: Added utility routine header to get a
									tokenized bullet (GetIntlTokenChar).
		<16>	  2/4/91	kaz		jng, #74915: Adding a boolean to DrawTriangle() to determine
									whether to dim the triangle.
		<15>	 1/23/91	kaz		<jng>, #81486: Adding resID field to private struct to
									differentiate between menuID and 'MENU' resID.
		<14>	 1/16/91	kaz		Adding ApFontID global location for Popup access
		<13>	12/14/90	kaz		Added comments for private record.
		<12>	 12/7/90	kaz		Hierarchial Menu support: adding popupResult and FindHMenu()
									proto. <jng>
		<11>	11/12/90	kaz		Adding flags to track changes for smarter drawing; removed
									hasColor as redundant. <jng>
		<10>	10/30/90	kaz		Adding changedVal to flag a change in the control's value; added
									popupSlopV constant; Removed PaintMyGrayRect() prototype. < BBH>
		 <9>	 9/14/90	kaz		Adding old menu height field
		 <8>	  8/5/90	kaz		Added new prototypes for gray drawing.
		 <7>	  8/3/90	kaz		Prototype & constant mods
		 <6>	 6/12/90	kaz		Prototype changes galore. Replaced TextInfo w/ TextState.
		 <5>	  6/4/90	kaz		Added fields for new drawing and new fixedWidth mentality.
		 <4>	 5/29/90	kaz		Removed SKANKY #ifdef
		 <3>	 3/16/90	BBH		stop diddling with wmgrport and wmgrcport font and size so
									removed appropriate entries from the TextInfo data structure
									(compiler directive SKANKY is undef-ed to turn off the wmgrport
									diddling)
		 <2>	  3/5/90	BBH		change spandex MDEF to be spandex proc resource type
		 <1>	  3/5/90	BBH		first checked in

	To Do:
*/

#include	<Controls.h>
#include	"ControlPriv.h"					// private control manager equates (for triangle ID)

//	the constants
////////////////////////////////////////////////////////////////////////////////

//	low memory globals used
#define			SysFontFam			0x0BA6
#define			SysFontSize			0x0BA8
#define			CurFMInput			0x0988
#define			ApFontID 			0x0984

//	characters
#define			BulletMark			0xA5

//	color grafport signature
#define			cGrafSignature		0x0000C000

//	patterns to be gotten from the system file
#define			blackPatternID		1
#define			grayPatternID		4

//	part code
#define			myPartCode			1
#define			movedPartCode		129

//	menu command key special values
#define			useReducedIcon		0x001D
#define			useSmallIcon		0x001E

//	MDEF's owned by me (CDEF 63)
#define			SpandexResType		'proc'
#define			SpandexMDEF			0xDFE0

#ifndef mDrawItemMsg
#define			mDrawItemMsg		4
#endif mDrawItemMsg
#ifndef mCalcItemMsg
#define			mCalcItemMsg		5
#endif mCalcItemMsg

#ifndef calcCntlRgn
#define			calcCntlRgn			10
#endif calcCntlRgn

#ifndef calcThumbRgn
#define			calcThumbRgn		11
#endif calcThumbRgn

//	internal constants
#define			popupTitleJustMask	0x00FF
#define			popupTitleStyleMask	0x7F00

#define			popupSlop			3
#define			popupSlopV			3

#define			shadowTop			3
#define			shadowLeft			3

//	the data structures
////////////////////////////////////////////////////////////////////////////////

typedef struct myData {			// private data structure from contrlData
	// the following structure MUST NOT be moved
	// it's documented in Inside Mac as being here
	// but we can have our private data following this

	PopupPrivateData	public;	// publicly known data structure

	long		popupResult;	// MacApp: hierarchial popups (hiword == menuId)
	Rect		myCtlRect;		// StdFile: area where the control is drawn

	// Everything below this is private
	Rect		oldContrlRect;	// in case control moves
	short		minWidth;		// min width for fixedWidth CNTLs
	short		maxWidth;		// contrlRect.right - contrlRect.left
	short		oldHeight;		// last menu item height
	short		oldWidth;		// last menu width
	short		oldMax;			// last # of menu items
	short		resID;			// 'MENU' resID can be differnt from menuID
	Boolean		changedVal;		// if the user has TRACKED a new value

	// other characteristics
	Boolean		useColor;		// are we going to use CQD?
	Boolean		makeCPort;		// do we need to create a cGrafPort?
	Style		titleStyle;		// style of the popup label
	short		titleWidth;		// width of popup label area
	short		titleJust;		// justification of the popup label
	short		preinstalled;	// menu preinstalled in menulist?

	// below here is dynamic recalculated when we draw
	short		sysJust;
	FontInfo	info;
} myData, *myDataP, **myDataH;

// This is also defined in CommToolboxPriv.h
typedef struct TextState {
	short		theFont;
	Style		theFace;
	short		theSize;
	short		theMode;
} TextState, *TextStatePtr;

typedef struct WMgrPortState {
	short		savedFont;
	short		savedSize;

	short		savedCFont;
	short		savedCSize;
} WMPortState, *WMPortStatePtr;

typedef struct Spandex {		// header for spandex MDEF
	short		branch;			// BRA.S @0
	short		flags;
	ResType		rType;
	short		procID;
	short		version;

	// this is important below
	Handle		defProc;		// saved mdefproc to be called
	short		dH;				// amount to expand menuWidth by
} Spandex, *SpandexP, **SpandexH;


//	functional prototypes
pascal long nupopup(short theVar, ControlHandle hControl, short msg, long theParam);

//	action procedures
void DoInit(ControlHandle hControl, short theVar);
long DoDraw(ControlHandle hControl, short theVar, short theParam);
long DoTest(ControlHandle hControl, long theParam);
void DoCalc(ControlHandle hControl, short theVar, long *theParam);
void DoDispose(ControlHandle hControl);
long DoTrack(ControlHandle hControl, short theVar);

typedef pascal void (**MDEFHandle)(short msg, MenuHandle hMenu, Rect *mRect, Point hitPt,
		short *whichItem);

//	drawing routines
void ResizeControl(ControlHandle hControl, short theVar);
void EraseControl(ControlHandle hControl);
void DrawTriangle(myDataP dataP, Boolean dimmed);
void DrawTitle(ControlPtr controlP, myDataP dataP);
void SetColors(short menuID, short menuItem, Boolean isHilited);
CGrafPtr MakeColorPort(GrafPtr owner);
PixPatHandle BeginMyGray(myDataP dataP);

//	calculate routines
void CalcTitleArea(myDataP dataP, Rect *theRect);
void CalcBoxArea(myDataP dataP, Rect *theRect);
void CalcMenuArea(myDataP dataP, Rect *theRect);
void CalcShadowArea(myDataP dataP, Rect *theRect);
void CalcTriangleRect(myDataP dataP, Rect *theRect);
void CalcAndDrawItem(ControlHandle hControl, short theVar,
									short theMsg, Rect *theRect);

// more utils
void GetTextState(TextState *txState);
void SetTextState(TextState *txState);
void SaveAndResetWMPorts(WMPortState *savedWMPortState);
void RestoreWMPorts(WMPortState *savedWMPortState);

void DoUseWFont(TextState *savedInfo, WindowPtr owner, Boolean saveIt);
short FindHItem(MenuHandle hMenu, short hMenuID);
char GetIntlTokenChar(short whichToken, short whichScript, char defaultChar);