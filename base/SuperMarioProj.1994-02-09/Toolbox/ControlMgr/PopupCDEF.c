/*
	File:		PopupCDEF.c

	Contains:	Source code for Popup Menu CDEF

	Written by:	Byron Han

	Copyright:	© 1987-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM4>	11/19/92	RB		Set ROMMapInsert to mapTRue to look for resources in ROM first.
		<43>	11/13/92	JDR		Updated QuickDraw.h to not use the dangerous pattern type.
		<42>	 11/7/92	JDR		Removed the PopupPrivateData record, which is now a public
									structure.  Change occurances of the usage too,
		<41>	 11/3/92	DTY		Strip out unnecessary includes.
		<40>	 5/21/92	gbm		#1030284,<dty>: Stop leaving the menu handle in the menu list
									all the time. This causes conflicts if two popups have the same
									ID.
		<39>	 5/10/92	BH		some applications stuff the window manager and color window
									manager ports with text size 12 (font is still set to 0).
									normally, these values are 0 and 0 - this causes the menu
									manager to barf since it tries to draw using the font and size
									of the window mgr ports.  if a non zero value is specified in
									these ports, the appropriate values in sysFontFam and
									sysFontSize are not	used (which is where we stuff the
									appropriate owning window's font and size	information.

									this is the infamous geneva 12 popup problem.

									and I solved it.  (Byronus Maximus)

									we use the routines SaveAndResetWMPorts() and RestoreWMPorts()
									[see mods in PopupCdef.h]

									All under the TheFuture conditional
									• REALPOPUP() when saving port state, also save and reset the
									window manager port and color port font and size to 0, 0
									• DoDraw() fix bug where triangles for items after the 31st in
									the menu would be dimmed
									• DoUseWFont() fix bug where if the owning window font was the
									system font we would not set the size for the control properly.
									• DoUseWFont() always invalidate the font manager cache (was
									checking to see if it was 0, 0 but it may be different because
									of window manager port/color port may have been set to 0, C
									instead) [fontfam, fontsize]
									• SaveAndResetWMPorts() added
									• RestoreWMPorts() added
		<38>	 2/19/92	BH		fixed comments for <37>
		<37>	 1/24/92	BH		Fixed problem with triangles for items after item 31 were always
									dimmed.  Also fixed the infamous 12 point geneva popup (now
									always invalidate font mgr cache when changing system font and
									size)
		<36>	 8/22/91	KSM		PKE,#85693-Bruges: Don't use hard-coded bullet char for menu.
									Don't use sysFontFam=0, get smSysScript.
		<35>	  4/5/91	JDR		dba, DTY, kaz, BRC#86187: If the menu has been disposed and then
									re-added to the menu list, call GetMHandle to refresh the menu
									handle in the control's data storage.
		<34>	  4/3/91	DTY		ngk, kaz, #SIAC: The preflighting calls _HNoPurge on the menu
									handle for all messages except for initCntlMsg. In low memory
									conditions, the menu can get purged inside of CalcAndDrawMenu
									which is called via ResizeControl which in turn is called during
									the initCntlMsg. Add lines to _HNoPurge the menu for the
									initCtlMsg case also.
		<33>	 3/27/91	kaz		jng, AEK-038: Fix last fix — dispCntl will dispose what dataP
									points to, so dataP->theMenu is an unknown. Instead we check the
									menuList w/ _GetMHandle to see if the menu is still around &
									then restore the state.
		<32>	 3/27/91	DFH		JWM,WS#DFH-910320a: Fixed so that we don't try to HSetState the
									menu handle if the menu has been disposed.
		<31>	 3/20/91	VL		DFH, WS#DFH-910320a: theMenu is made non-purgeable during the
									CDEF routines. This fixes many problems with PopupCDEF during
									low-memory situations.
		<30>	 3/15/91	kaz		jng, b6-ngk-002: Fixing bug in _TestControl — removing -2 fudge
									factor from rectangle test
		<29>	  2/7/91	kaz		BBH, #81082: ResizeControl() - if the popup gets too big and
									only the triangle is showing (Views CDEV - size menu), we will
									draw the frame at the contrlRect boundary.
		<28>	  2/4/91	kaz		jng, #74915: DrawTriangle() will dim the triangle if the control
									is dimmed, the whole menu is dimmed, or the menuItem is dimmed.
		<27>	 1/23/91	kaz		<jng>,#81486,#81487: Fix ResEdit crash by exiting from the CDEF
									if the MENU is really gone. If the menu is not preloaded, use
									the contrlMin field as a 'MENU' resID and get the new menuID
									after GetMenu(resID).
		<26>	 1/16/91	kaz		Fixing Script Mgr bug: DoUseWFont() will not stuff SysFontFam if
									txFont is systemFont or applFont.
		<25>	 1/14/91	kaz		<jng>  Fixing DrawTitle for B&W classic machines.
		<24>	12/13/90	kaz		Copying the pnVis field if we have to create a color port. <jng>
		<23>	 12/7/90	kaz		Hierarchial Menu support: added popupResult field to private
									record to save the last value of PopupMenuSelect. Won't check
									hierarchial items, and draws the unpopped item without hier
									arrow. Added FindHMenu() to return the parent if a hier menu
									item is tracked; Fixed bug in right just of title: not sign
									extending. <jng>
		<22>	11/12/90	kaz		Added StandardFile support: draws the box pinned to the
									contrlRect if needed;  Shrinks the box width to be the current
									itemwidth. Smarter drawing: only redraws the control if heights
									or widths have changed, the control has moved, etc.  Some
									cleanup. <jng>
		<21>	10/30/90	kaz		Triangle is now drawn using _FillPoly instead of _DrawPicture;
									Will only dehilite the title if the user tracks the same value;
									Removed PaintMyGrayRect(). <BBH>
		<20>	 9/22/90	gbm		(with kaz) Changing DoDraw to properly dispose of PixPats (by
									calling PenNormal!) instead of leaving them around in the app
									heap
		<19>	 9/16/90	PWD		Grays the title on classic machines properly. <kaz>
		<18>	 9/15/90	PWD		Reimplemented AddResMenu variant; popupUseCQD is now reserved.
									<kaz> <crz>
		<17>	 9/14/90	kaz		Always tries to use CQD; Clips properly, even if we roll our own
									color port; Erase control if the new menu item height < old (for
									icons).
		<16>	 9/13/90	kaz		DoDraw(): no longer calls _DisposPixPat after _PenPixPat (let QD
									dispose it when we reset the PenState); calls _PenPixPat after
									sending drawItemMsg in case MDEF has changed it.
		<15>	 9/11/90	kaz		If the menu is cleared after InitMsg, we reload it and delete it
									at the end.
		<14>	  9/5/90	kaz		Properly bracketing calls for BeginGray/EndGray
		<13>	  9/4/90	kaz		Changing gray text _PenMode to _TextMode.
		<12>	  8/5/90	kaz		Added new graying algorithm; Spun out DrawTitle().
		<11>	  8/2/90	kaz		Fixed shadow dimensions. Moved triangle in a little, and center
									it for popups with not enough space. Only erases the old
									location if the control has moved. Only redraws the title when
									mouse hit. Fixed right justified bugs. Temporary gray algorithm.
									More code shrinkage.
		<10>	 7/10/90	dba		get rid of C warnings
		 <9>	 6/12/90	kaz		Mega changes to decrease code size. JustLeft is now the default.
									Does not grow the contrlRect. Clips to conrtlRect. Draws the
									popup nicely if the menu is empty. ItemMark will be a checkmark
									if the useWFont is a SystemFont.
		 <8>	  6/4/90	kaz		Temporarily removed the min/max FixedWidth stuff as Finder uses
									the refCon. Resort to old fixedWidth. Locks theData before
									calling ResizeControl() from DoInit().
		 <7>	  6/4/90	kaz		Control is now vertically centered; Will not normally change the
									control rectangle, except to grow it. Will center the control if
									titleWidth == 0 and titleJust = center (for Standard File).
									FixedWidth will calc a minimum width from the refCon, and
									maxWidth from the controlRect. AddResMenu temporarily #ifdef'ed.
		 <6>	 5/31/90	kaz		popupFixedWidth expands the menu in DoTrack() if the menuWidth <
									boxWidth.
		 <5>	 5/29/90	kaz		Title textFace is set to grafPort's for useWFont; Removed SKANKY
									#ifdef; Tweaked traingle drawing.
		 <4>	 3/20/90	EMT		Changed CQDLives() function to HaveCQD() macro used by
									Notification and Layer managers (to save code).
		 <3>	 3/16/90	BBH		removed code that checked if mdefproc bailed on the
									mCalcItemMsg; also stopped diddling with the wmgrport and
									wmgrcport's font and fontsize since we are already diddling with
									sysFontFam and sysFontSize
		 <2>	  3/5/90	BBH		fix problem with drawing the popup label in right justification
									mode
		 <1>	  3/5/90	BBH		first checked in to BBS

	To Do:
		• Add our own truncString for < System 7

*/

//	Pre-BBS Modification History
//		WHEN		WHO				WHAT
//		2/6/90		BBH				Changed drawing of triangle via polygon to use a pict
//									renamed hasColor to hasCQD in myData data structure
//									renamed color to useCQD in myData data structure
//
//		12/7/89		BBH		Fixed calculation of size for reduced icons
//									Now tries to use mdef to calculate item size and
//										standard message for drawing an item.
//
//		12/6/89		BBH		Finished port to C
//									Better accuray in redraw, smaller code
//		12/5/89		BBH		Started
//////////Ported to C
/*	old revision notes
		12/4/89		BBH		Use two custom MDEF's
									One that calls the mdef and widens the width
									Other really isn't an MDEF - just handles mDrawMsg
										differently (draws item in whichItem in the
										rectangle provided) - but handles icons,
										sicns, and reduced icons
									Added triangle support
									Consolidation of calculation routines
		11/30/89	BBH		Trying to get it up
		10/4/89		BBH		Minimum safe title width is 1 pixel.  We now check.
		10/3/89		BBH		If menu preexists in the menu list, we do not a GetMenu, InsertMenu
										upon creation, and a DeleteMenu, DisposMenu upon disposal
										(SAFEMENUS set to TRUE for this feature)
									Fixed bug in TruncateString if space given is insufficient for even
										an ellipsis caused infinite loop in truncation.
									TruncateString routine has been modified to return empty string
										if there is insuffient space for 1 character (can be multibyte)
										plus the ellipsis.
		7/25/89		BBH		Always recalculate the height regardless of popupFixedWidth bit.
		6/19/89		BBH		Added checks for the popup menu trap and the script manager trap.
									If the popup menu trap is NOT available, then draw
										the control as if disabled and do not perform tracking.
									If the script manager trap is NOT available, then truncate the
										string one byte at a time (do not test for multibute chars)
		6/18/89		BBH		Instead of setting clip to the control rectangle, set to the intersection
										of the existing clipRgn and the control rectangle.
									Removed scripts field from private data structure.  Was not being used.
									Reformatted modification history.  Added more generous comments.
									Also use CTBUtils.p pascal interface file for the variant code
										constants.
									Removed some dead code.  Did a personal review of the code.
		6/16/89		BBH		TruncateString is now script manager compatible - uses CharByte.
									Also, when drawing the control, we clip to the control rectangle.
									This prevents a problem with descenders in geneva 9pt, shadow being
										drawing outside of the control rectangle.
		6/14/89		BBH		Added support for calcCntlRgn and calcThumbRgn messages for System 7.0
										compatibility.  See TN 212
		6/8/89		BBH		Fixed bug that Rich Hoiberg was having that if you have a control
										attached to a dialog item list and after creating the dialog
										(and hence creating the control) you change the dialog font and
										dialog size, the height of the popup is not recalculated properly.
										fixed by calling GetFontInfo in DoResize and manually adjusting
										the control height if the variation code for fixed size is not being
										used.
									Removed info: FontInfo and height: INTEGER from myData data structure
		6/7/89		BBH		Changed EraseRect of the control rectangle to a EraseControl call
										which will only erase the parts of the control that get drawn
										which means if the window is filled with a pattern, no annoying
										whitespace gets left behind.
		6/4/89		BBH		PreflightFont is called after resetting the window manager ports
										in DoResize.
		5/31/89		BBH		popup value has the justification stored as a signed Byte
										in the low byte and stores the popup title style in
										the high byte if the high bit is clear
									Defined new constants in CTBUtils.r, .p, and .h
									If high bit of the popup value is set, ignore style (this is for
										backwards compatibility with old CNTL templates which use -1 for
										the popup title justification)
		5/3/89		BBH		Changed HLock and HUnlock of the control handle to HGetState,
										HLock, HSetState
		4/26/89 	BBH		Changed all BitAnd's to BANDs
									Changed drawing to not subtract 3.  This fixes shifting of quiescent and
										active menu items with system 6.0.4
		3/22/89 	BBH		use checkmark if using system font
									use • if using window font instead of checkMark
									we now call preflightFont to load in font info if using window font
										prior to calling CalcMenuSize in the DoResize routine.
									Also indentation when drawing popup box is using the max char width
										of the font minus two.
		3/21/89 	kaz		in DoTrack() check to see if variant is UseWFont
										then save the old WMgrPort text info, set it to the
										current window text info, then reset it.  This
										should paint the menus in the right font/size
									Changed Pop-up to use a bullet mark • for the
										checked menu item instead of a checkmark
		3/14/89 	BBH		use System Font size 0.
									also added support for variant codes.
									moved refcon/AddResMenu stuff to variant code 4.
									added color quickdraw stuff with support for mctb.
										opens a CGrafPort is necessary.
									added support for sys just of teJustRight to draw
										title to the RIGHT of the pop up box.
		12/23/88 	BBH		Drawing of the ow done with TextBox.  Is using GetSysJust
									Also, instead of using System Font size 12, we get font and
										size information from window manager port.
									The test routine will return 0 or myPartCode which is 1.
									Caveats
										The CDEF installs and deinstalls the menu from the menu list
											when the control is created/destroyed.
										If the menu does not exist, you are at your own risk.
		12/22/88 	BBH		value is justification of the popup menu title.  It is
										reset to min after control creation
									application must reset the value then.

		9/88		BBH		Second major rewrite to add AddResMenu capability
									refCon is NIL - nothing
									refCon is NOT nil - do a AddResMenu(ResType(refcon));
									menuID is MIN
									popuptitlewidth is MAX
		7/88		BBH		First major rewrite.  Use refCon to specify menuID and popup title
										width.  Use flag bit in refCon to determine if application
										is performing InsertMenu/DeleteMenu.
									Refcon values
										HiWord contains ID of menu
										LoWord contains width of popup title
									If the hi bit of the low word of the refcon is set, then the calling
									applciation must install/deinstall the menu from the menu list.
									Rect is rect.
		11/2/87		BBH		Second version
		10/20/87	BBH		First version

	Things to be done:
		TruncateString should be internationalized.  Test it.
		Why does TrackControl always return non zero even if we track out of the control?

		Make popup return different part codes
*/

#include <Types.h>
#include <Controls.h>
#include <Fonts.h>
#include <Memory.h>
#include <Menus.h>
#include <QuickDraw.h>
#include <QuickDrawText.h>
#include <Resources.h>
#include <Script.h>
#include <SysEqu.h>
#include <TextEdit.h>
#include <ToolUtils.h>

#include "PopupCDEF.h"

#define	HaveCQD()	((*(unsigned short *) ROM85) <= 0x3FFF)


/*************************************************************
	REALPOPUP		- DefProc control

		hControl	- the popup
		theVar		- CDEF variant
**************************************************************/

pascal long REALPOPUP(short theVar, ControlHandle hControl, short msg, long theParam)
{
	PenState		savedPen;
	TextState		savedText,
					myText;
	WMPortState		savedWMPortState;

	GrafPtr			savedPort;
	unsigned char 	savedState;
	long			rval = 0;			// return value
	GrafPtr			owner;
	myDataP			dataP;				// moved this declaration out from if-statement 	<31>
	unsigned char	menuSavedState;		// state for theMenu								<31>

	GetPort(&savedPort);			// set to the owner
	owner = (**hControl).contrlOwner;
	SetPort(owner);

	savedState = HGetState((Handle) hControl);
	HLock((Handle) hControl);		// lock the control down

	/* We need to lock theData 'cuz we use dangling ptrs to save space		*/
	/* It's NIL at init time, but HLock doesn't care, so we don't either	*/
	HLock((Handle) (**hControl).contrlData);

	GetTextState(&savedText);
	GetPenState(&savedPen);
	SaveAndResetWMPorts(&savedWMPortState);

	PenNormal();
	if ( !(theVar & popupUseWFont) ) {
		myText.theFont = systemFont;
		myText.theSize = 0;
		myText.theFace = 0;
		myText.theMode = srcOr;
		SetTextState(&myText);
	}

	dataP = *((myDataH) (**hControl).contrlData);				// moved this from the if-statement below <31>

	// if the menu's gone, reload it & mark it for deletion later
	if (msg != initCntl) {
		dataP->public.mHandle = GetMHandle(dataP->public.mID);		// <35>
		if (dataP->public.mHandle == nil) {
			dataP->preinstalled = false;						// delete at dispose time
			dataP->public.mHandle = GetMenu(dataP->resID);
			if (dataP->public.mHandle == nil)
				goto restoreState;								// if it doesn't work, get out
			dataP->public.mID = (**dataP->public.mHandle).menuID;
			InsertMenu(dataP->public.mHandle, hierMenu);

			menuSavedState = HGetState((Handle) dataP->public.mHandle);
			HNoPurge((Handle) dataP->public.mHandle);
			} else
				dataP->preinstalled = true;

		// <31> The following segment of code is added to ensure that the menu is not purgeable
		// 		during the course of PopupCDEF routines.

	}


	switch (msg) {
		case drawCntl:
			if ( (**hControl).contrlVis )
				rval = DoDraw(hControl, theVar, (short) theParam);
			break;

		case testCntl:
			rval = DoTest(hControl, theParam);
			break;

		case calcCRgns:
#ifdef calcCntlRgn
		case calcCntlRgn:
#endif calcCntlRgn
#ifdef calcThumbRgn
		case calcThumbRgn:
#endif calcThumbRgn
			DoCalc(hControl, theVar, &theParam);
			break;

		case initCntl:
			DoInit(hControl, theVar);
			break;

		case dispCntl:
			DoDispose(hControl);
			break;

		case autoTrack:
			rval = DoTrack(hControl, theVar);
			break;
	}

	// <31> The following line is used to restore the state of public.mHandle so that public.mHandle can
	//		become purgeable again. (maybe)
	//	we inserted the menu in the list on entry, and now we’re taking it out again
	if ((msg != initCntl) && (msg !=dispCntl)) {
		if (!dataP->preinstalled) {
			DeleteMenu( dataP->public.mID );
			HSetState((Handle)dataP->public.mHandle, menuSavedState );
		}
	}

restoreState:
	SetTextState(&savedText);
	SetPenState(&savedPen);			// restore control handle
	RestoreWMPorts(&savedWMPortState);

	HUnlock((Handle) (**hControl).contrlData);
	HSetState((Handle) hControl, savedState);

	SetPort(savedPort);
	return(rval);					// bye bye
}

/////////////////////////////////////////////////////////////////////////////////////////////////

/*************************************************************
	DoInit			- Gets the values from the control record,
					and puts them in our private data struct

		hControl	- the popup
		theVar		- CDEF variant
**************************************************************/

void DoInit(ControlHandle hControl, short theVar)
{
	myDataP			dataP;
	ControlPtr		controlP;
	myDataH			theData;
	MenuHandle		hMenu;
	unsigned char	menuSavedState;						// Save the state of the menu handle <34>

	controlP = *hControl;

	theData = (myDataH) NewHandleClear(sizeof(myData));
	controlP->contrlData = (Handle) theData;
	if (theData == nil)
		goto error;

	HLock((Handle) theData);							// will be unlocked in REALPOPUP

	dataP = *theData;									// ptr for smaller code

	// deal with environment
	dataP->useColor = HaveCQD();						// always try to use CQD
	if (dataP->useColor)
		dataP->makeCPort = ((dataP->useColor) &&
						(!(controlP->contrlOwner->portBits.rowBytes & cGrafSignature)));

	// deal with creation parameters
	dataP->resID = dataP->public.mID = controlP->contrlMin;		// get the menu ID

	if (controlP->contrlMax > 1)
		dataP->titleWidth = controlP->contrlMax;

	dataP->titleJust = (char) (controlP->contrlValue & popupTitleJustMask);
	if (!(controlP->contrlValue & popupTitleNoStyle))
		dataP->titleStyle = (Style) ((controlP->contrlValue & popupTitleStyleMask ) >> 8);

	// Add the grafport's style if useWFont
	if (theVar & popupUseWFont)
		dataP->titleStyle |= controlP->contrlOwner->txFace;

	// get the menu; if it isn't pre-installed, then we need to build it
	hMenu = GetMHandle(dataP->public.mID);
	dataP->preinstalled = (hMenu != nil);
	if (! dataP->preinstalled) {
		hMenu = GetMenu(dataP->resID);
		if (hMenu == nil)
			goto error;
		menuSavedState = HGetState((Handle) hMenu);			// Save the state 			<34>
		HNoPurge((Handle) hMenu);							// Make it non-purgeable	<34>
		dataP->public.mID = (**hMenu).menuID;
		InsertMenu(hMenu, hierMenu);						// stick it in
	}
	dataP->public.mHandle = hMenu;

	// Use the refCon as the resource type to add
	if ( (theVar & popupUseAddResMenu) )
		AddResMenu(hMenu, (ResType) controlP->contrlRfCon);

	// Get the widths for fixedWidth controls.  Note that if the refCon = 0, this is
	// the old fixedWidth control setting to maintain compatibility w/ 1.0
	dataP->maxWidth = controlP->contrlRect.right - controlP->contrlRect.left;

	if (theVar & popupFixedWidth)
		dataP->minWidth = dataP->maxWidth;

	// set up max/min/val
	controlP->contrlMin = 1;
	controlP->contrlMax = CountMItems(hMenu);
	if (controlP->contrlMax == 0 )
		controlP->contrlMin = 0;							// nothing around
	controlP->contrlValue = controlP->contrlMin;

	dataP->popupResult = (dataP->public.mID << 16) + controlP->contrlValue;

	dataP->oldContrlRect = controlP->contrlRect;
	dataP->oldMax = controlP->contrlMax;
	dataP->changedVal = true;								// forces a draw of the control

	ResizeControl(hControl, theVar);						// calculate dynamic values

	controlP->contrlAction = (ProcPtr) (-1);				// handle own autotrack
	if (!dataP->preinstalled) {								// take this back out of the menu list when 		<gbm>
		DeleteMenu(dataP->public.mID);						// we’re done, but only if we put it in there 		<gbm>
		HSetState((Handle) dataP->public.mHandle, menuSavedState);		// Restore the state			<34>
	}
	goto exit;
error:														// clean up
	DoDispose(hControl);
	controlP->contrlData = nil;								// nil it out please

exit:
	;
}


/*************************************************************
	DoDraw			- Draws the entire popup control

		hControl	- the popup
		theVar		- CDEF variant
		theParam	- part code to draw:
						0 			=> all parts
						myPartCode 	=> control hasn't moved
						129			=> control has moved
		returns		- 0
**************************************************************/

long DoDraw(ControlHandle hControl, short theVar, short theParam)
{
	myDataP			dataP;
	ControlPtr		controlP;
	CGrafPtr		myCPort = nil;
	GrafPtr			owner;									// owner of control
	Rect			boxRect,
					theRect;
	RgnHandle		savedClip,
					newClip;								// for setting clip to control frame
	RGBColor		oldCQDFore,								// CQD variables
					oldCQDBack;
	Boolean			dimmedOut;
	Boolean			useColor;
	PixPatHandle	myPixPat = nil;
	long			oldFlags;
	Boolean			majorClean = false;
	short			cmdChar = 0,
					markChar = 0;

	controlP = *hControl;
	dataP = *((myDataH) controlP->contrlData);

	useColor = dataP->useColor;								// local vars for smaller code
	owner = controlP->contrlOwner;

	// We dim the control if the hilite is set, or the menu is empty
	dimmedOut = ((controlP->contrlHilite == 255) ||
								((**dataP->public.mHandle).menuWidth == 0));

	// get new menu item height
	SetRect(&theRect, 0, 0, 0, 0);
	CalcAndDrawItem(hControl, theVar, mCalcItemMsg, &theRect);	// theRect.top = 0 => ignore it
	GetItemCmd(dataP->public.mHandle,controlP->contrlValue,&cmdChar);
	GetItemMark(dataP->public.mHandle,controlP->contrlValue,&markChar);

	majorClean = ((theParam == movedPartCode) ||
				(controlP->contrlMax != dataP->oldMax) ||
				(theRect.bottom != dataP->oldHeight) ||
				(cmdChar == 0x001A) ||
				(dataP->oldWidth != (**dataP->public.mHandle).menuWidth) ||
				(! EqualRect(&dataP->oldContrlRect, &controlP->contrlRect)));

	if (majorClean) {
		dataP->oldMax = controlP->contrlMax;
		dataP->oldHeight = theRect.bottom;						// update flags
		dataP->oldWidth = (**dataP->public.mHandle).menuWidth;
		dataP->oldContrlRect = controlP->contrlRect;

		EraseRect(&dataP->myCtlRect);
		ResizeControl(hControl, theVar);					// recalc & erase new loc
		EraseRect(&dataP->myCtlRect);
	}

	// If the value has changed, we just need to dehilite the title
	if (! dataP->changedVal)
		if (! majorClean)
			if (dataP->titleWidth <= 0)
				goto done;

PortNClip:
	if (dataP->makeCPort)
		myCPort = MakeColorPort(owner);
															// save off color quickdraw stuff
	if (useColor) {
		GetForeColor(&oldCQDFore);
		GetBackColor(&oldCQDBack);
	}

	savedClip = NewRgn();
	newClip = NewRgn();
	GetClip(savedClip);
	RectRgn(newClip, &controlP->contrlRect);
	SectRgn(newClip, savedClip, newClip);
	SetClip(newClip);

	if (dataP->titleWidth > 0) {
		DrawTitle(controlP, dataP);

		if (! dataP->changedVal)
			if (! majorClean)
				goto cleanup;
	}

	// Don't need to redraw the following for hilited state as
	// menu will popup on top and save bits behind
	if ((controlP->contrlHilite == myPartCode) && (! majorClean))
		goto cleanup;

	// now erase the popup box if CQD to get colors right
	CalcBoxArea(dataP, &boxRect);
	if (useColor) {
		SetColors(dataP->public.mID, controlP->contrlValue, false);
		EraseRect(&boxRect);
	}

	// Draw the Menu Item
	// If we're dimmed, we trick the Menu Mgr into dimming for us
	// Also, we don't want any check marks, hier arrows drawn for us, so we zap 'em
	CalcMenuArea(dataP, &theRect);
	if (dimmedOut) {
		oldFlags = (**dataP->public.mHandle).enableFlags;
		if (controlP->contrlValue <= 31)
			DisableItem(dataP->public.mHandle, controlP->contrlValue);
	}
	if (cmdChar == 0x001B)
		SetItemCmd(dataP->public.mHandle,controlP->contrlValue,0);
	SetItemMark(dataP->public.mHandle,controlP->contrlValue,0);

	CalcAndDrawItem(hControl, theVar, mDrawItemMsg, &theRect);

	// Now put everything back
	SetItemCmd(dataP->public.mHandle,controlP->contrlValue,cmdChar);
	SetItemMark(dataP->public.mHandle,controlP->contrlValue,markChar);
	if (dimmedOut)
		(**dataP->public.mHandle).enableFlags = oldFlags;

	// Triangle will be dimmed if control is dimmed OR the menu OR current item is dimmed
	// so we reuse oldFlags to see if the menu is dimmed, else if the item is dimmed

	oldFlags = (**dataP->public.mHandle).enableFlags & 1;		// bit 0 -- whole menu is dimmed
#if CubeE
	if (oldFlags == 1)
		oldFlags = (**dataP->public.mHandle).enableFlags & (1 << controlP->contrlValue);
#endif
#if TheFuture
	if (oldFlags == 1) {
		if (controlP->contrlValue <= 31)					// <37>
			oldFlags = (**dataP->public.mHandle).enableFlags & (1 << controlP->contrlValue);
	}
#endif
	DrawTriangle(dataP,(dimmedOut || (oldFlags == 0)));

	if (useColor)
		SetColors(dataP->public.mID, -1, false);				// reset to default colors

	// set it in case menu mgr changed it
	if (dimmedOut)
		myPixPat = BeginMyGray(dataP);

	// Frame & Shadow
	FrameRect(&boxRect);
	CalcShadowArea(dataP, &boxRect);
	MoveTo(--boxRect.right, boxRect.top);					// move the pen INSIDE the rect
	LineTo(boxRect.right, --boxRect.bottom);
	LineTo(boxRect.left, boxRect.bottom);

	// Graying over the triangle
	if (dimmedOut)
		if (myPixPat == nil) {								// gray the title if no color avail.
			CalcTitleArea(dataP, &theRect);
			PenMode(patBic);
			PaintRect(&theRect);
		}

	PenNormal();
	if (myPixPat != nil)
		DisposPixPat(myPixPat);

cleanup:
	// _SetPenState will be reset it main

	if (useColor) {
		RGBForeColor(&oldCQDFore);
		RGBBackColor(&oldCQDBack);
	}

	SetClip(savedClip);
	DisposeRgn(savedClip);
	DisposeRgn(newClip);

	if (myCPort != nil) {
		CloseCPort(myCPort);
		DisposPtr((Ptr) myCPort);
		SetPort(owner);
	}

done:
	dataP->changedVal = true;
	return(0);
}


/*************************************************************
	DoTest			- Determines if a point is in my control
					and returns which part.

		hControl	- the popup
		theParam	- point location

		returns		- control part code
**************************************************************/

long DoTest(ControlHandle hControl, long theParam)
{
	myDataP			dataP;
	Point			thePoint;
	long			rval = 0;
	Rect			theRect;

	*((long *) &thePoint) = theParam;

	dataP = *((myDataH) (**hControl).contrlData);

	theRect = dataP->myCtlRect;

	if (PtInRect(thePoint, &theRect))
		if (((**hControl).contrlHilite != 255) &&
						((**dataP->public.mHandle).menuWidth != 0))
			rval = myPartCode;

	return (rval);
}


/*************************************************************
	DoDispose		- Returns the control area as a region
					for Control Mgr calculations

		hControl	- the popup
**************************************************************/

void DoDispose(ControlHandle hControl)
{
	myDataH			theData;

	theData = (myDataH) (**hControl).contrlData;
	if (theData != nil) {
		if (!(**theData).preinstalled) {
			DeleteMenu( (**theData).public.mID);
			DisposeMenu( (**theData).public.mHandle );
			(**theData).public.mHandle = nil;
		}

		DisposHandle((Handle) theData);
		(**hControl).contrlData = 0;
	}
}


/*************************************************************
	DoCalc			- Returns the control area as a region
					for Control Mgr calculations

		hControl	- the popup
		theVar		- CDEF variant
		theParam	- a VAR RgnHandle to return
**************************************************************/

void DoCalc(ControlHandle hControl, short theVar, long* theParam)
{
	ResizeControl(hControl, theVar);
	RectRgn((RgnHandle) (*theParam), &((**((myDataH) (**hControl).contrlData)).myCtlRect));
}



/*************************************************************
	DoTrack			- Gets the SpandexMDEF if necessary, and
					calls _PopUpMenuSelect.

		hControl	- the popup
		theVar		- CDEF variant
**************************************************************/

long DoTrack(ControlHandle hControl, short theVar)
{
	myDataP			dataP;
	MenuHandle		hMenu;
	TextState		savedInfo;								// saved wmgr/cwmgr port font/size
	SpandexH		theHandle = nil;
	Rect			theRect;
	Point			thePoint;
	Boolean			useSpandex = true;
	short			growWidth;
	short			curWidth;
	char			myMark;
	long			rval = 0;
	short			cmdChar = 0;
	short			thisMenuID;

	dataP = *((myDataH) (**hControl).contrlData);

	hMenu = dataP->public.mHandle;

	CalcBoxArea(dataP, &theRect);
	thePoint.h = ++theRect.left;
	thePoint.v = ++theRect.top;
	LocalToGlobal(&thePoint);

	if (theVar & popupUseWFont)
		DoUseWFont(&savedInfo,(**hControl).contrlOwner,true);

	/* We want to check the current item but won't for StdFile ($1A) or Hier ($1B)	*/
	/* Also, use checkmark unless we're not using System Font						*/
	GetItemCmd(hMenu,(**hControl).contrlValue,&cmdChar);
	if ((cmdChar != 0x001A) && (cmdChar != 0x001B)) {
		myMark = (char) checkMark;
		if (theVar & popupUseWFont)
		{
			short sysFont = (*((short *) SysFontFam));
			if (sysFont != GetScript(GetEnvirons(smSysScript), smScriptSysFond))
				myMark = GetIntlTokenChar(tokenCenterDot, Font2Script(sysFont), BulletMark);							// no checkmark available
		}
		SetItemMark(hMenu, (**hControl).contrlValue, myMark);
	}

	/* We want to grow the menu for non-fixed width and for	*/
	/* fixed width where the menuWidth < controlWidth		*/
	CalcBoxArea(dataP, &theRect);
															// +1 for the shadow
	growWidth = theRect.right - theRect.left - (**hMenu).menuWidth - popupSlop + 1;
	useSpandex = (growWidth > 0);

	if (useSpandex) {										// invoke spandex MDEF
		*(short*) RomMapInsert = mapTrue;												// <SM4> rb
		theHandle = (SpandexH) GetResource(SpandexResType, SpandexMDEF);				// <SM4> rb
		if (theHandle != nil) {								// load in spandex MDEF
			LoadResource( (Handle) theHandle );				// load in case of purged -- now unpurgeable so remove this
			HLock( (Handle) theHandle );
			if ( * (Handle) theHandle == nil)				// was it loaded properly?
				theHandle = nil;							// nope
		}

		if (theHandle != nil) {							// splice old mdef into spandex
			(**theHandle).defProc = (**hMenu).menuProc;		// store old mdefproc

			/* If the menu is in a fixed state, then we expand to the max	*/
			/* otherwise we just grow to the triangle size					*/

			curWidth = dataP->myCtlRect.right - dataP->myCtlRect.left;
			if (!((theVar & popupFixedWidth) &&
							((curWidth == dataP->minWidth) ||
											(curWidth == dataP->maxWidth))))
				growWidth = dataP->info.widMax;

			(**theHandle).dH = growWidth;					// amount to widen by...

			(**hMenu).menuProc = (Handle) theHandle;		// splice it in
			CalcMenuSize(hMenu);							// recalculate the menu width
		}
	}

	rval = PopUpMenuSelect(hMenu, thePoint.v, thePoint.h, (**hControl).contrlValue);

	dataP->popupResult = rval;								// track menuID & value for hier menus

	if (useSpandex)											// take off spandex
		if (theHandle != nil) {
			(**hMenu).menuProc = (**theHandle).defProc;		// restore mdefproc
			HUnlock( (Handle) theHandle );
		}

	if ((cmdChar != 0x001A) && (cmdChar != 0x001B))
		CheckItem(hMenu, (**hControl).contrlValue, false);		// uncheck the item

	if ((theVar & popupUseWFont))
		DoUseWFont(&savedInfo,nil,false);

	dataP->changedVal = false;

	/*
	To determine the control value we normally check the low word of rval.
	For hierarchial menus, the ctl value should be the parent item of the selected item.
	We match the menuHandles instead of the menu IDs, 'cuz MacApp adds a menuHandle
	from their 'cmnu' stuff
	*/

	thisMenuID = (rval & 0xFFFF0000) >> 16;
	if (thisMenuID == 0)
		rval = 0;
	else {
		if (GetMHandle(thisMenuID) != dataP->public.mHandle)
			rval = FindHItem(dataP->public.mHandle,thisMenuID);
		else
			rval &= 0x0000FFFF;
															// set the flag for changed val
		dataP->changedVal = ((**hControl).contrlValue != rval);
		(**hControl).contrlValue = rval;

		rval = myPartCode;									// useless as DoTrack is a void()
	}

	return(rval);
}



/*************************************************************
	ResizeControl	- Calculates the Rect to draw in.
					Grows contrlRect if needed.

		hControl	- the popup
		theVar		- CDEF variant
**************************************************************/

void ResizeControl(hControl, theVar)
	ControlHandle 		hControl;
	short 				theVar;
{
	myDataP				dataP;
	myDataH				theData;
	TextState			savedInfo;
	MenuHandle			hMenu;
	short				height;									// also used for menu width
	Rect				theRect;
	short				curWidth;
	short				lineHeight;
	short				cmdChar = 0;			// for StdFile presence
	short				limit;
	Rect				controlRect,
						mRect;

	theData = (myDataH) (**hControl).contrlData;

	dataP = *theData;
	hMenu = dataP->public.mHandle;

	// recalculate dynamic properties
	dataP->sysJust = GetSysJust();

	if ((theVar & popupUseWFont))
		DoUseWFont(&savedInfo,(**hControl).contrlOwner,true);

	CalcMenuSize(hMenu);										// recalculate menusize
	GetFontInfo(&dataP->info);									// get font information

	// Set my control area
	controlRect = (**hControl).contrlRect;
	dataP->myCtlRect = controlRect;

	//	calculate height of the control
	SetRect(&theRect, 0, 0, 0, 0);
	CalcAndDrawItem(hControl, theVar, mCalcItemMsg, &theRect);
	dataP->oldHeight = theRect.bottom;
	dataP->oldWidth = (**hMenu).menuWidth;

	// If the menu is empty, get the height from the font info
	lineHeight = theRect.bottom;								// - top, but top == 0
	if (lineHeight == 0)
		lineHeight = dataP->info.ascent + dataP->info.descent + dataP->info.leading;

	// We now center the control on the height
	height = ((controlRect.bottom - controlRect.top) - lineHeight - popupSlopV) >> 1;
	dataP->myCtlRect.top += height;
	dataP->myCtlRect.bottom = dataP->myCtlRect.top + lineHeight + popupSlopV;

	// If the box gets too tall, we shorten the popup to the controlRect for 2 cases:
	// StandardFile and the case where only the triangle is displayed

	GetItemCmd(hMenu,(**hControl).contrlValue,&cmdChar);
	CalcMenuArea(dataP, &mRect);
													// small menurect => no menuitem drawn
	if ((cmdChar == 0x001A) || ((mRect.right - mRect.left) <= (popupSlop << 1))) {
		limit = controlRect.top;
		if (dataP->myCtlRect.top < limit)
			dataP->myCtlRect.top = limit;
		limit = controlRect.bottom;
		if (dataP->myCtlRect.bottom > limit)
			dataP->myCtlRect.bottom = limit;
	}

	// Calc the width of the control.  If the menu's empty, we set to the max width
	// If StandardFile, the width is the current item, otherwise use menuWidth
	if ((**hMenu).menuWidth == 0)
		height = dataP->maxWidth;
	else {
		height = dataP->info.widMax + dataP->titleWidth + popupSlop;
		if (cmdChar == 0x001A)
			height += theRect.right;
		else
			height += (**hMenu).menuWidth;
	}

	// Sanity check for new fixed width, both min & max
	if (height > dataP->maxWidth)
		height = dataP->maxWidth;

	if (height < dataP->minWidth)
		height = dataP->minWidth;

	/* For popups w/ no title (i.e. StandardFile), we can center the control	*/
	if ((dataP->titleWidth == 0) &&
						(dataP->titleJust == popupTitleCenterJust)) {
		curWidth = controlRect.right - controlRect.left;
		dataP->myCtlRect.left -= (height - curWidth) >> 1;
		dataP->myCtlRect.right = dataP->myCtlRect.left + height;
	}
	else if (dataP->sysJust == teJustRight)
		dataP->myCtlRect.left = dataP->myCtlRect.right - height;
	else
		dataP->myCtlRect.right = dataP->myCtlRect.left + height;

	// restore font information
	if ((theVar & popupUseWFont))
		DoUseWFont(&savedInfo,nil,false);

}


///////////////////////////////////////////////////////////////////////////////////////////
// Drawing Routines
///////////////////////////////////////////////////////////////////////////////////////////

/*************************************************************
	DrawTriangle		- Draws the popup triangle

		dataP		- control info
		dimmed		- gray the triangle using fore & back blend
**************************************************************/

void DrawTriangle(myDataP dataP, Boolean dimmed)
{
	short			tWidth;								// width of triangle
	short			tHeight;							// height of triangle
	short			sWidth;								// width of rectangle
	short			sHeight;							// height of tectangle
	Rect			trigRect,
					boxRect;
	short			theWidth;							// max width of the triangle
	PolyHandle		trigPoly;
	PixPatHandle	myPixPat = nil;

	if (dimmed)
		myPixPat = BeginMyGray(dataP);

	CalcTriangleRect(dataP, &trigRect);
	CalcBoxArea(dataP, &boxRect);

	theWidth = dataP->info.widMax;
	sWidth = boxRect.right - boxRect.left;
	sHeight = trigRect.bottom - trigRect.top;

	// We need a width that's 75% of original and even
	tWidth = theWidth;
	tWidth -= (tWidth >> 2);
	if (tWidth & 0x0001)
		tWidth++;

	// Minimum size sanity checking
	if (tWidth < 6) {
		tWidth = 6;
		if (tWidth > theWidth) {
			if (theWidth < 4)
				return;
			tWidth = 4;
		}
	}

	tHeight = tWidth >> 1;

	/* Horizontally center the triangle if the control 	*/
	/* isn't wide enough for the menu item				*/
	if (sWidth < (theWidth + (popupSlop << 1)))
		trigRect.left = boxRect.left + ((sWidth - tWidth) >> 1);
	else if (dataP->sysJust == teJustRight)
		trigRect.left = trigRect.right - tWidth;

	trigRect.right = trigRect.left + tWidth;
	trigRect.top += ((sHeight - tHeight) >> 1);			// move top by half of vertical whitespace
	trigRect.bottom = trigRect.top + tHeight;

	trigPoly = OpenPoly();
		MoveTo(trigRect.left, trigRect.top);
		LineTo(trigRect.right, trigRect.top);
		LineTo((trigRect.right + trigRect.left) >> 1, trigRect.bottom);
		LineTo(trigRect.left, trigRect.top);
	ClosePoly();

	PaintPoly(trigPoly);
	KillPoly(trigPoly);

	PenNormal();
	if (myPixPat != nil)
		DisposPixPat(myPixPat);
}



/*************************************************************
	DrawTitle		- Draws the popup title

		controlP	- control
		dataP		- control info
**************************************************************/

void DrawTitle(ControlPtr controlP, myDataP dataP)
{
	Rect			titleRect,
					theRect;
	Style			oldStyle;
	short			oldMode;
	Str255			theString;
	short			width;

	CalcTitleArea(dataP, &titleRect);
	InsetRect(&titleRect,0,1);

	// Erase the title & swap colors for color inverse
	if (dataP->useColor)
		SetColors(dataP->public.mID, 0, (controlP->contrlHilite == myPartCode));							// use menu table

	EraseRect(&titleRect);

	oldStyle = (controlP->contrlOwner)->txFace;
	oldMode = (controlP->contrlOwner)->txMode;
	TextFace(dataP->titleStyle);

	// We dim the control if the hilite is set, or the menu is empty
	if ((controlP->contrlHilite == 255) ||
								((**dataP->public.mHandle).menuWidth == 0))
		TextMode(grayishTextOr);

	BlockMove((Ptr) controlP->contrlTitle, (Ptr) theString,
										Length(controlP->contrlTitle) + 1);

	// textbox rect
	theRect = titleRect;
	InsetRect(&theRect, popupSlop, 0);

	// turn on condensing if we can't fit
	width = theRect.right - theRect.left - 1;	// 1 is for indent
	if (StringWidth(theString) > width) {
		TextFace(dataP->titleStyle | condense);
		TruncString(width, theString, smTruncEnd);
	}
	TextBox((Ptr) (theString + 1), Length(theString), &theRect, dataP->titleJust );
	TextFace(oldStyle);
	TextMode(oldMode);

	// now hilite the title if necessary and if not CQD (CQD already done above)
	if (controlP->contrlHilite == myPartCode)
		if (! dataP->useColor)
			InvertRect(&titleRect);
}



/*************************************************************
	MakeColorPort		- Returns a color grafport

		owner			- where this will reside
**************************************************************/

CGrafPtr MakeColorPort(GrafPtr owner)
{
	Rect		theRect;
	Point		tempPt;
	TextState	myText;
	CGrafPtr	myCPort;

	myCPort = (CGrafPtr) NewPtr(sizeof(CGrafPort));		// create color graf port
	if (myCPort != nil) {
		theRect = owner->portRect;

		tempPt.h = theRect.left;
		tempPt.v = theRect.top;

		LocalToGlobal(&tempPt);
		OpenCPort(myCPort);								// initialize the port (and set to it)
		MovePortTo(tempPt.h, tempPt.v);					// and size it properly
		PortSize( theRect.right - theRect.left, theRect.bottom - theRect.top);

		// now copy quickdraw stuff
		myText.theFont = owner->txFont;
		myText.theSize = owner->txSize;
		myText.theFace = owner->txFace;
		myText.theMode = owner->txMode;
		SetTextState(&myText);

		PenNormal();									// Isn't this done for us?
		CopyRgn(owner->clipRgn, myCPort->clipRgn);
		CopyRgn(owner->visRgn, myCPort->visRgn);
		if (owner->pnVis < 0)
			HidePen();
	}

	return(myCPort);
}



/*************************************************************
	SetColors		- Sets the RGB Fore & Back colors to
					the appropriate values

		menuID		- for the menu color table entry
		menuItem	- 	> 0 	--> item color table entry
						= 0 	--> use menu CT
						= -1 	--> set to default colors
		isHilited	- reverse the colors if we're hilited
**************************************************************/

void SetColors(short menuID, short menuItem, Boolean isHilited)
{
	RGBColor	newCQDFore, newCQDBack,
				*foreCP, *backCP;
	MCEntryPtr	theMenuColor;

	newCQDFore.red = 								// load up initial values
	newCQDFore.green =
	newCQDFore.blue = 0;

	newCQDBack.red =
	newCQDBack.green =
	newCQDBack.blue = 0xFFFF;

	/* We try to get the colors asked for.				*/
	/* Failing that, we keep trying 'till default mctb	*/

	if (menuItem == -1)								// just set to default color
		goto setColor;

	if (menuItem == 0)								// default item color
		goto getMenuColor;

	/* We have to make local copies of the menu colors, 'cuz the 	*/
	/* table is relocatable, and RGBForColor may move it			*/

													// use mctb for menu/item
	if ((theMenuColor = GetMCEntry(menuID,menuItem)) != nil) {
		newCQDFore = theMenuColor->mctRGB2;			// item color
		newCQDBack = theMenuColor->mctRGB4;			// item background color
		goto setColor;
	}

getMenuColor:										// use mctb for menu
	if ((theMenuColor = GetMCEntry(menuID,0)) != nil) {
		newCQDFore = theMenuColor->mctRGB3;			// default item color
		newCQDBack = theMenuColor->mctRGB4;			// default item background color
		goto setColor;
	}

getSystemColor:										// use default mctb
	if ((theMenuColor = GetMCEntry(0,0)) != nil) {
		newCQDFore = theMenuColor->mctRGB3;			// default title color
		newCQDBack = theMenuColor->mctRGB2;			// default menubar background color
	}

setColor:
	foreCP = &newCQDFore;					// ptrs for easy swap & small size
	backCP = &newCQDBack;

	if (isHilited) {
		foreCP = &newCQDBack;				// swap values if we're highlighted
		backCP = &newCQDFore;
	}

	RGBForeColor(foreCP);
	RGBBackColor(backCP);
}


///////////////////////////////////////////////////////////////////////////////////////////
// Calculation Routines
///////////////////////////////////////////////////////////////////////////////////////////

/*************************************************************
	CalcAndDrawItem	- passes mCalcMsg or mDrawMsg to the
					menu defProc

		hControl	- the popup
		theVar		- CDEF variant
		theRect		- what to return
**************************************************************/

void CalcAndDrawItem(ControlHandle hControl, short theVar, short theMsg, Rect* theRect)
{
	myDataH			theData;
	MDEFHandle		theHandle;
	short			whichItem;
	TextState		savedInfo;
	unsigned char	theState;

	theData = (myDataH) (**hControl).contrlData;

	theHandle = (MDEFHandle) (**((**theData).public.mHandle)).menuProc;
	if (theHandle == nil)
		return;

	LoadResource((Handle) theHandle);
	if (*((Handle) theHandle) == nil)
		return;

	// Set up the grafport info
	if (theVar & popupUseWFont)
		DoUseWFont(&savedInfo,(**hControl).contrlOwner,true);

	whichItem = (**hControl).contrlValue;

	// Is the lock really needed??
	theState = HGetState( (Handle) theHandle);
	HLock((Handle) theHandle);
		(**theHandle) (theMsg, (**theData).public.mHandle, theRect, * (Point*) theRect, &whichItem);
	HSetState((Handle) theHandle, theState);

	// Reset the grafport info
	if (theVar & popupUseWFont)
		DoUseWFont(&savedInfo,nil,false);
}


/*************************************************************
	CalcTriangleRect	- Calcs triangle rect for pict

		dataP		- the popup
		theRect		- what to return
**************************************************************/

void CalcTriangleRect(myDataP dataP, Rect *theRect)
{
	short			tWidth;

	CalcBoxArea(dataP, theRect);

	tWidth = dataP->info.widMax + popupSlop;

	InsetRect(theRect,1,1);
	if (dataP->sysJust == teJustRight)
		theRect->right = theRect->left + tWidth;
	else
		theRect->left = theRect->right - tWidth;
}


/*************************************************************
	CalcTitleArea	- Calcs enclosing title rect

		dataP		- the popup
		theRect		- what to return
**************************************************************/

void CalcTitleArea(myDataP dataP,Rect *theRect)
{
	short		tWidth;

	*theRect = dataP->myCtlRect;
	tWidth = dataP->titleWidth;

	if (dataP->sysJust == teJustRight)
		theRect->left = theRect->right - tWidth;
	else
		theRect->right = theRect->left + tWidth;

	--theRect->bottom;
}


/*************************************************************
	CalcBoxArea	- Calcs popup box without shadow

		dataP		- the popup
		theRect		- what to return
**************************************************************/
void CalcBoxArea(myDataP dataP, Rect *theRect)
{
	short			tWidth;

	*theRect = dataP->myCtlRect;
	tWidth = dataP->titleWidth;

	if (dataP->sysJust == teJustRight)
		theRect->right -= tWidth;
	else
		theRect->left += tWidth;

	--theRect->bottom;
	--theRect->right;
}


/*************************************************************
	CalcMenuArea	- Calcs popup box without shadow, triangle

		dataP		- the popup
		theRect		- what to return
**************************************************************/

void CalcMenuArea(myDataP dataP, Rect *theRect)
{
	short			tWidth;

	tWidth = dataP->info.widMax + popupSlop;
	CalcBoxArea(dataP, theRect);

	if (dataP->sysJust == teJustRight) {
		theRect->left += tWidth;
		--theRect->right;
	}
	else {
		theRect->right -= tWidth;
		++theRect->left;
	}

	++theRect->top;
}



/*************************************************************
	CalcShadowArea	- Calc rect for popup shadow

		dataP		- the popup
		theRect		- what to return
**************************************************************/

void CalcShadowArea(myDataP dataP, Rect *theRect)
{
	CalcBoxArea(dataP, theRect);
	theRect->left += shadowLeft;
	theRect->top += shadowTop;
	++theRect->right;
	++theRect->bottom;
}



/*************************************************************
	GetTextState	- Loads the current grafport text info

		txState		-	Like penState
**************************************************************/

void GetTextState(TextState *txState)
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

void SetTextState(TextState *txState)
{
	TextFont(txState->theFont);
	TextSize(txState->theSize);
	TextFace(txState->theFace);
	TextMode(txState->theMode);
}

/*************************************************************
	SaveAndResetWMPorts	-
						saves font and size for window manager
						port and color window manager port
						then sets them to zero
		WMPortState	-	Like penState
**************************************************************/

	void
	SaveAndResetWMPorts(WMPortState *pWMPortState)
	{
		GrafPtr		tempPort;
		CGrafPtr	tempCPort;

		tempPort = * (GrafPtr *) WMgrPort;
		pWMPortState->savedFont = tempPort->txFont;
		pWMPortState->savedSize = tempPort->txSize;
		tempPort->txFont = tempPort->txSize = 0;

		if (HaveCQD()) {
			tempCPort = * (CGrafPtr *) WMgrCPort;
			pWMPortState->savedCFont = tempCPort->txFont;
			pWMPortState->savedCSize = tempCPort->txSize;
			tempCPort->txFont = tempCPort->txSize = 0;
		}
	}

/*************************************************************
	RestoreWMPorts	- 	sets the wmgr and color wmgr ports
		WMPortState	-	Like penState
**************************************************************/
	void
	RestoreWMPorts(WMPortState *pWMPortState)
	{
		GrafPtr		tempPort;
		CGrafPtr	tempCPort;

		tempPort = * (GrafPtr *) WMgrPort;
		tempPort->txFont = pWMPortState->savedFont;
		tempPort->txSize = pWMPortState->savedSize;

		if (HaveCQD()) {
			tempCPort = * (CGrafPtr *) WMgrCPort;
			tempCPort->txFont = pWMPortState->savedCFont;
			tempCPort->txSize = pWMPortState->savedCSize;
		}
	}

/*************************************************************
	DoUseWFont		- Sets the font mgr low mem globals so
						we can have Geneva 9 popups

		savedInfo	- Fills it in if saveIt = true, else
						it sets the port to those values
		owner		- Where to get the original values
		saveIt		- true at the start of the
**************************************************************/

void DoUseWFont(TextState *savedInfo, WindowPtr owner,  Boolean saveIt)
{
	TextState		myState,
					*theState;
	short			aFont;

	theState = savedInfo;

	if (saveIt) {
		savedInfo->theFont = *((short *) SysFontFam);	// save low memory globals
		savedInfo->theSize = *((short *) SysFontSize);

		myState.theFont = owner->txFont;
		myState.theSize = owner->txSize;
		theState = &myState;

		// if we stuff systemFont, it will screw up Script Mgr
		if (owner->txFont == systemFont)
			goto dosizestuff;
	}

	// if we stuff applFont, this will also screw up Script Mgr
	// instead we get the actual font

	aFont = theState->theFont;
	if (saveIt)
		if (owner->txFont == applFont)
			aFont = *((short *) ApFontID);
	*((short *) SysFontFam) = aFont;					// set/restore low memory globals

dosizestuff:
	*((short *) SysFontSize) = theState->theSize;

	*((long *) CurFMInput) = 0xFFFFFFFF;
}



/*************************************************************
	BeginMyGray		- Sets up the pen, etc for disabled
						drawing

		returns		- A colorQD pattern
**************************************************************/

PixPatHandle BeginMyGray(myDataP dataP)
{
	Pattern			pat;
	RGBColor		oldFore, oldAft;
	PixPatHandle	myPixPat = nil;

	if (dataP->useColor) {
		// do color QD stuff here
		GetForeColor(&oldFore);
		GetBackColor(&oldAft);

		// gimme average RGB.  Why does MenuMgr do a ROXR instead of an ASR??
		oldFore.red = (oldFore.red + oldAft.red) >> 1;
		oldFore.green = (oldFore.green + oldAft.green) >> 1;
		oldFore.blue = (oldFore.blue + oldAft.blue) >> 1;

		myPixPat = NewPixPat();

		if (myPixPat != nil) {
			MakeRGBPat(myPixPat,&oldFore);
			PenPixPat(myPixPat);
		}

		TextMode(grayishTextOr);	//grayish text copy
	}

	if (myPixPat == nil) {
		GetIndPattern(&pat,sysPatListID,grayPatternID);
		PenPat(&pat);
	}

	return(myPixPat);
}


/*************************************************************
	FindHItem		- Tries to find a hierarchial item in the
					menu with the given hier menu id

		returns		- The parent item number
**************************************************************/

short FindHItem(MenuHandle hMenu, short hMenuID)
{
	short		i;
	short		cmdChar = 0,
				markChar = 0;

	for (i = 1;i <= CountMItems(hMenu);i++) {
		GetItemCmd(hMenu, i, &cmdChar);
		if (cmdChar == 0x001B) {
			GetItemMark(hMenu, i, &markChar);
			if (markChar == hMenuID)
				return(i);
		}
	}

	return(1);
}


/*************************************************************
	GetIntlTokenChar - Get the token for the given script

		returns		- The token or the default if error
**************************************************************/

char GetIntlTokenChar(short whichToken, short whichScript, char defaultChar)
{
	Handle	itl4H;
	long	offset, len;

	// Look up the untoken table -- bail if we can’t get it
	IUGetItlTable(whichScript, iuUnTokenTable, &itl4H, &offset, &len);
	if (itl4H && (offset > 0) && (len >= 0))
	{
		char *sp = (*itl4H + offset);				// Point to start of untoken table
		if (whichToken <= ((short *)sp)[1])			// Check if token has valid index
		{
			sp += ((short *)sp)[2+whichToken];		// Add the string offset and voliá!
			if (*sp == 1) defaultChar = sp[1];		// Might be 2-byte, so check length byte
		}
	}
	return(defaultChar);
}