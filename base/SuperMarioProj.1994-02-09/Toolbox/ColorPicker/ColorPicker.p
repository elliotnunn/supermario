{
	File:		ColorPicker.p

	Contains:	Color Picker package

				This is the heart of the ColorPicker package, the part which presents the
				color selection dialog, animates it, and lets the user ultimately select
				a color.  It changes the color table, if there is one, so that the user
				sees the real color, not just the current "best match" returned by Quickdraw.
				The conversion routines for the various color models, the assembly dispatcher
				for the package, and the glue code that unlocks the package upon exit are
				all elsewhere.

	Copyright:	© 1987-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang Sys606

	Change History (most recent first):

		<13>	 5/21/91	gbm		Stop using "empty" units like Memtypes...
		<12>	 1/18/91	gbm		(csd) Change prompt string so it will be truncated instead of
									wrapping around.
		<11>	 1/10/91	SMC		Added code to ignore strings with non-numeric characters when
									pasting into a number field.  With KON.
		<10>	12/14/90	dvb		Eyedropper
		 <9>	12/14/90	gbm		& JDR; Dispose the palette after closing the window, instead of
									setting the palette to nil and disposing it first. This has the
									side effect of causing more updates in some cases, but avoids
									the unsightly color table change while the color wheel is still
									visible. I am not completely sure that this is the right thing.
									An alternative is to return to the old way, but to erase the
									contents of the color picker dialog before changing the palette
									(and indirectly, the color table).
		 <8>	10/12/90	dvb		Remove color arrows
		 <7>	  9/4/90	dvb		Make whizzier.
		 <6>	 8/28/90	dvb		Numbering mixed up because project was corrupted and restored.


		 <8>	 8/27/90	dvb		Fix sample draw on non-clut devices
		 <7>	 8/27/90	dvb		NEEDED FOR SIXPACK: Remove calls to PixPatChanged and
									CTabChanged... does this break 8•24 GC?
		 <6>	 8/15/90	dvb		NEEDED FOR SIXPACK: Fix to work an b/w machines
		 <5>	 6/12/90	DVB		Make main-screen preferred in case of depth tie.
		 <4>	 5/30/90	DVB		Streamline: remove cWhite and cBlack. Use colors for input and
									output in 2-bit mode.
		 <3>	 5/23/90	DVB		Make it use palettes instead of RestoreEntries, fix cursor bug
									(7.0 only)
		 <2>	12/28/89	dba		got rid of obsolete compile-time options (options are passed in
									by the Makefile)
	   <1.4>	12/14/89	dvb		NEEDED for 6.0.5: Change default roundrect to conform to Inside
									Mac recommendations, make 'esc' key the same as cmd-. .
	   <1.3>	  8/3/89	CSD		NEEDED For 6.0.4: Set Cursor to an arrow when dialog opens (BRC
									#30423). Also, changed the way that the scroll bar CDEF was
									called to make it safer for 32-Bit Memory Manager.
	   <1.2>	  8/3/89	CCH		Changed references to "transIndex" to "ctFlags".
	  <•1.1>	 7/24/89	CSD		Forced submission of the Color Picker from 32-Bit QuickDraw. Too
									many changes, too long ago to merge without causing havoc.

	older Change History (oldest first, Darin is too lazy to convert it):

	1.0d7	amy		Updated PickDlgClose to use the new color manager proc DelSearch.
						Note that 1.0d6 won't run on >B3 ROMs, since it does a DisposPtr
						on the proc queue element, which is now a handle.

	1.0b1	amy		Made the model dialog inaccessible by moving the button offscreen.

	1.0b2	amy		Corrected colorOk test.  We now test that the machine has a Mac II
						or later ROM, rather than checking that CPUFlag = 2 (68020).
					Paint the final "tint" on the wheel (no saturation) with FillOval,
						rather than filling a separate arc for each color.
					Added SetPort to the beginning of PickDlgDrawItem, to correct a bug
						that shows up iff you're on a non-clutType device (wheel and new
						sample were being drawn in the wrong port initially).
					Changed SampleDraw to use lines, not FrameRect, so there's less
						flicker while updating iNewSample on a non-clutType device.
					Rearranged order of colors in PColor, to alter the priority with
						which colors are matched as they are swiped from the color table.
						Old order was colors, white, black, gray, input & output samples.
						New order is nearly the same, but colors come last.  This insures
						that black and white are always substituted correctly, that the
						color being picked, if on screen, is more likely to animate, and
						that the colors in the wheel are lowest priority for matching.
					When resolving which CLUT entries to swipe, come down from the top
						of the color table, not up from the bottom.
					When resolving which CLUT entries to swipe, avoid the hilight color,
						and any grays (except black and white), since they may be used in
						gray scale fonts.
					Removed MakeITable calls, which were making it impossible to avoid the
						hilight color if the input color was close to any of the entries
						Color Picker installs in the color table.  This should affect no
						one (and should make the package less of a memory hog, and safer),
						because Color Picker uses a search proc to get any of its colors.
					Changed CTabSearch to return FALSE if gdType <> clutType, since we'd
						rather have the default search proc get the best match for fixed
						devices.
					Don't artifically spread the colors before GetSubTable by upping the
						brightness, and don't invert inColor either.
					Turned off debugging and (unused) remodel dialog to save space.  Turned
						off skip gray also, and removed remodel resources from the .r file.

	1.1a1	amy		Updated to use MPW 2.0 interface files.
					Removed $LOAD file to help out SCM.

	1.2d1	amy		Iff we're in 8 bit mode or more, on a clut device, use more colors in
						the wheel (36 for now).  This does NOT slow use down by 36/6,
						because of a clever idea of David's for using FrameOval for the
						saturation levels, which eliminates ((wheelTints - 1) * maxColors)
						PaintArc calls to (wheelTints - 1) FrameOvals.

	1.2b1	amy		Do a SetPort after GetNewDialog, in case there is no port at start.
					Handle autoKey events just like keydown events to get validation.
					Increased size of editText fields by 2 each to get rid of jumping.
					Art added NewPalette and SetPalette to render ActivatePalette harmless,
						so that the color environment won't change (esp. between the time
						we chose which entries to animate and the dialog appears).

	1.2b2	amy		Preserve caller's port around calls to GetColor.

	1.3b1	amy		Fix HSL2RGB once and for all by replacing HiWrd, which is broken in tests.

	1.4a2	CSD		Added constants for resources STR# $E980, ppat $E980, wedg $E980-$E981, and
						CURS $E980
					Used STR# for drawing wedge names in 1 and 2 bit modes.
					Make a where parameter of (-1, -1) place the dialog on the deepest screen.

	1.4a3	CSD		Accept arrow keys as well as digits and backspace.
					Handle CMD-X, CMD-C, CMD-V as cut, copy, paste.
					Handle CMD-. as cancel with a brief hilite of the cancel button.
					Briefly hilite Ok button for return and enter.
					Option-click in up and down arrows sets value to max and min.

	1.4a4	CSD		With a where parameter of (-1, -1), use the "best" screen, not neccessarily
						the deepest screen. (i.e. color 4-bit is better than mono 8-bit)
					Erase the hot-spot before redrawing the wheel to prevent trash from being
						left outside the wheel.
					Test for required memory and show an alert if we don't think we can
						run safely.
					If possible, animate the colors while the user is dragging the thumb
						in the brightness scroll bar.
	1.4a5	CSD		Reduced the amount of memory that guarantees we can run. It used to have
						about 9K of slop, now it has about 2K.
	1.4a6	CSD		Instead of testing for required memory by calling PurgeSpace, actually
						allocate a handle and then dispos it. This will cause the heap to
						grow if it can, giving us a more correct result.
					When looking for the 'best' device, don't count inactive screens.
			dvb		17 April 1989 - Fixed searchproc to work in 16-bit mode, made 8-color
						wheel less interesting and much faster.
	1.4b2	CSD		Fixed test to see if we should allocate a palette for the window. It
						used to test colorOk which wasn't setup yet.
					Fixed bug where RGB/HSV values wouldn't update if the thumb was
						dragged to one of the control extremes on an 8-bit display.

	To Do:
		Add error handling for pickDlg = NIL
		Handle CLUTFixed, CLUTDirect better
		Nicer looking cursor and hot spot
		Use dithering to smooth color wheel appearance
		Test for wheel hits via radius from center, not wheelRgn?
		Build custom brightness control?
		Beef up error handling throughout
		Make model selection dialog work
		Add YIQ conversion routines
		Get Gerard's sqrt & get rid of SANE

		What if calling app has protected lots of CLUT entries?
		SetDAFont(applFont), and make prompt a user item in System font
		How to check that we aren't swiping the CLUT entry for a desktop color?
		Whenever editField is used, watch out for editField < 0?
		Put scaling in model selection dialog?
		Add graphical component legends?
		Help button and text?
		Keyboard equivalents?

		Change setting of colorOk to use SysEnvirons

}

{$SETC HSV := TRUE}				{Use HSV color model (FALSE=HSL)}
{$SETC GRUNGY := TRUE}			{Grungy behavior that needs to be cleaner}
{$SETC UsePalette := TRUE}

UNIT ColorPicker;

INTERFACE

USES
	Types, Memory, QuickDraw, Palettes, Resources, Fonts, Dialogs, 
	Packages, GestaltEqu, FixMath, ToolUtils, OSUtils, SysEqu,
	ColorConvert;

FUNCTION  GetColor(where: Point; prompt: Str255; inColor: RGBColor;
					VAR outColor: RGBColor): BOOLEAN;

IMPLEMENTATION

CONST
	arrowDeltaLow =		1;		{Change in component value for inc/dec}
	arrowDeltaMed =		10;		{1st level of inc/dec acceleration}
	arrowDeltaHi =		100;	{2nd level of inc/dec acceleration}
	arrowSlop =			2;		{Slop around each arrow rect for tracking}
	inputSlop =			4;		{Slop around input sample for tracking}
	lineBright =		$100;	{Change in bright control for scroll arrow}
	maxColors =			36;		{Wheel wedges if 8 bit mode w/ animation}
	minColors =			6;		{Wheel wedges otherwise}
	pageBright =		$10;	{Change in bright control for scroll page}
	pickClientID =		12;		{Client ID used with picker's search proc}
	slopRim =			10;		{Rim outside wheel that still tracks}
	twoPi =				$0006487F;	{Two times Pi, in Fixed format}
	wheelTints =		5;		{Number of saturation levels in wheel}

	{Resource IDs}
	rPickDlg =			$E980;	{Color picker dialog}
	rArrowPic =			$E980;	{Inc/dec control picture}
	rRemodelDlg =		$E981;	{Remodel dialog}
	rColorNamesStr =	$E980;	{STR# of color slice names}
	rGreenWedge =		$E980;	{Green wedge bits for wheel}
	rRedWedge =			$E981;	{Red wedge bits for wheel}
	rCursor =			$E980;	{A good looking cursor}
	rMemShortageID =	$E982;	{Not enough memory alert}

	{Memory Requirements}
	kDirectMemReq =		55600;	{RAM needed on 16/32 bit devices. Actually ≈53608}
	kClutMemReq =		24000;	{RAM needed on clut devices. Actually ≈22200}

	{Items in the dialog}
	{ok = 				1;}		{Defined for us by the Toolbox}
	{cancel = 			2;}		{Defined for us by the Toolbox}
	iRemodel =			3;		{Button to bring up remodel dialog}
	iPrompt =			4;		{User-supplied prompt string}
	iBrightCtl =		5;		{Brightness control}
		{Items iFirstUI to iLastUI are drawn by DrawUserItem}
	iFirstUI =			6;		{First of user items in dialog}
	iOkOutline =		6;		{Outline for OK button}
	iWheel =			7;		{Hue/saturation wheel}
	iNewSample =		8;		{Sample square of current color}
	iOldSample =		9;		{Sample square of input color}
	iVersion =			10;		{Color picker version number, writ small}
	iFirstArrow =		11;		{First of up/down controls}
	iHSxArrows =		11;		{First of three HSx up/down controls}
	iRGBArrows =		14;		{First of three RGB up/down controls}
	iLastArrow =		16;		{Last of up/down controls}
	iLastUI =			16;		{Last of user items in dialog}
		{Color component labels are items 17-22, edit fields are below}
	iFirstComponent	=	23;		{First color component text string}
	iHue =				23;
	iSaturation =		24;
	iLightness =		25;
	iRed =				26;
	iGreen =			27;
	iBlue =				28;
	iLastComponent =	28;		{Last color component text string}

TYPE
	ColorItem = (rgbText, hsText, lText, hsWheel, lWheel, bright);
	ColorItemSet = SET OF ColorItem;

	IntPtr = ^INTEGER;
	LongPtr = ^LONGINT;
	RGBColorPtr =	^RGBColor;

	PColor = (cInput, cSample, cGray, cSlice1);

	PickColor = RECORD CASE INTEGER OF
		0: (hsx : HSLColor;
			rgb : RGBColor);
		1: (components : ARRAY [0..5] OF INTEGER);
		END;

	ReqListPtr = ^ReqListRec;
	ReqListHandle = ^ReqListPtr;

	PickInfo = RECORD			{Picker's "global" vars stick with dialog}
		dlgRec: DialogRecord;	{Dialog record must be first}
		rgbInput: RGBColor;		{Color the caller supplied as input}
		theColor: PickColor;	{Color being picked, in all req'd transforms}
		showModel: BOOLEAN;		{Show primary color model?}
		showAltModel: BOOLEAN;	{Show alternate color model?}
		slopRgn: RgnHandle;		{Region slightly larger than the wheel}
		wheelRgn: RgnHandle;	{Region of the wheel itself}
		wheelCenter: Point;		{Center of the color wheel}
		wheelRadius: Fixed;		{Radius of the color wheel}
		wheelColors: INTEGER;	{Number of wedges in the color wheel}
		lastColor: PColor;		{Color table index of last color in wheel}
		whichPart: INTEGER;		{Part of control we started tracking in}
		ourCursor: BOOLEAN;		{Is our cursor set (or the usual arrow)?}
		cursorRgn: RgnHandle;	{Composite of item rects in which we want ourCursor}
		cursorHand: CursHandle;	{Handle to our cursor}
		hotSpot: Point;			{Displayed hot spot (HS) on wheel, if any)}
		pixSize: INTEGER;		{pixelsize of screen we are on}
		gdType: INTEGER;			{type of gDevice we are on}
		colorOK: BOOLEAN;		{Not ok if: 128k ROM or one bit per pixel}
		itsGDev: GDHandle;		{deepest device the dlog intersects}
		pickCTab: CTabHandle;	{Table of colors picker borrows, or nil}
	{$IFC UsePalette}
		pickPal: PaletteHandle;	{Handle to a palette full of animated entries}
	{$ELSEC}
		whichColor: PColor;		{Which entry in pickCTab search proc should return}
		saveCTab: CTabHandle;	{Table of pre-picker CLUT entries, or nil}
		saveIRes: INTEGER;		{Caller's inverse table resolution value}
		pickReqList: ReqListHandle;	{List of stolen CLUT entries, or nil}
	{$ENDC}
		END;
	PickInfoPtr = ^PickInfo;	{Ptr to our info, and the dialog}

	QDGlobals = RECORD
		randSeed: LONGINT;
		screenBits: BitMap;
		arrow: Cursor;
		dkGray: Pattern;
		ltGray: Pattern;
		gray: Pattern;
		black: Pattern;
		white: Pattern;
		thePort: GrafPtr;
		END;
	QDGlobalsPtr = ^QDGlobals;

	ControlThumbInfo = RECORD
		limitRect:	Rect;
		slopRect:	Rect;
		axis:		INTEGER;
		startPoint:	Point;
		END;
	ControlThumbInfoPtr = ^ControlThumbInfo;

{---------------------------------------------------------------------------------
 Declare some EXTERNALS that are in ColorPicker.a.
}
FUNCTION NibbleUnpack(src,dst:LONGINT):LONGINT; EXTERNAL;
PROCEDURE SetGray25Pat; EXTERNAL;
PROCEDURE SetGray50Pat; EXTERNAL;
PROCEDURE SetGray75Pat; EXTERNAL;
PROCEDURE SetGray50BPat; EXTERNAL;


{---------------------------------------------------------------------------------
 Declare everything FORWARD, so we can order the source alphabetically.  Paired
 routines are together.  GetColor, the only one that will (eventually) be PACKaged,
 is last.
}

 FUNCTION  ArrowTrack(theDialog: DialogPtr; where: Point; option : Boolean): BOOLEAN; FORWARD;

 PROCEDURE BrightAdjust(pickDlg: DialogPtr; ctlHand: ControlHandle; forceIt : Boolean); FORWARD;
 PROCEDURE BrightCtlGet(ctlHand: ControlHandle; VAR lightness: SmallFract); FORWARD;
 PROCEDURE BrightCtlSet(ctlHand: ControlHandle; lightness: SmallFract); FORWARD;
 FUNCTION  BrightTrack(theDialog: DialogPtr; where: Point): BOOLEAN; FORWARD;
 PROCEDURE BrightTrackScroll(theControl: ControlHandle; partCode: INTEGER); FORWARD;

 FUNCTION  CallControl(theControl : ControlHandle; message : Integer;
 						param : LongInt) : LongInt; FORWARD;
 PROCEDURE CTabAdjust(pickDlg: DialogPtr; newBright: BOOLEAN); FORWARD;
 PROCEDURE CTabBuild(VAR pickData: PickInfo; inColor: RGBColor); FORWARD;
 PROCEDURE CTabInstall(pickDlg: DialogPtr; newBright: BOOLEAN); FORWARD;
 PROCEDURE CTabRestore(pickPtr: PickInfoPtr); FORWARD;
 PROCEDURE CTabSave(pickPtr: PickInfoPtr); FORWARD;
 FUNCTION  CTabSearch(myColor: RGBColor; VAR index: LONGINT): BOOLEAN; FORWARD;

 FUNCTION  DeviceCTab: CTabHandle; FORWARD;
 FUNCTION  DeviceITab: ITabHandle; FORWARD;

 PROCEDURE RGBNormal; FORWARD;

 FUNCTION  GetIHand(theDialog: DialogPtr; item: INTEGER): Handle; FORWARD;
 PROCEDURE GetIRect(theDialog: DialogPtr; item: INTEGER; VAR itemRect: Rect); FORWARD;
 FUNCTION  GetQDGlobals: QDGlobalsPtr; FORWARD;

 PROCEDURE HSx2RGB(hsx: HSLColor; VAR rgb: RGBColor); FORWARD;

 PROCEDURE MakeWedges32(wheelRect:Rect; brite:INTEGER); FORWARD;

 PROCEDURE PickDlgClose(infoPtr: PickInfoPtr); FORWARD;
 PROCEDURE PickDlgDrawItem(theWindow: WindowPtr; item: INTEGER); FORWARD;
 FUNCTION  PickDlgFilter(theDialog: DialogPtr; VAR theEvent: EventRecord; VAR itemHit: INTEGER): BOOLEAN; FORWARD;
 FUNCTION  PickDlgOpen(infoPtr: PickInfoPtr; where: Point; prompt: Str255): DialogPtr; FORWARD;

 PROCEDURE RemodelDlg(pickRect: Rect; VAR showModel, showAltModel: BOOLEAN); FORWARD;
 PROCEDURE RemodelDlgDrawItem(theWindow: WindowPtr; item: INTEGER); FORWARD;
 PROCEDURE RGB2HSx(rgb: RGBColor; VAR hsx: HSLColor); FORWARD;

 PROCEDURE SampleDraw(pickDlg: DialogPtr; item: INTEGER; sampleRect: Rect); FORWARD;
 FUNCTION  SampleTrack(theDialog: DialogPtr; where: Point): BOOLEAN; FORWARD;

 PROCEDURE ShowAffected(pickDlg: DialogPtr; item: INTEGER); FORWARD;
 PROCEDURE ShowHotSpot(where: Point); FORWARD;
 PROCEDURE ShowNewColor(pickDlg: DialogPtr; affected: ColorItemSet); FORWARD;
 PROCEDURE ShowNewValues(pickDlg: DialogPtr; newRGB: BOOLEAN; affected: ColorItemSet); FORWARD;
 PROCEDURE SnareNewComponents(pickDlg: DialogPtr); FORWARD;
 PROCEDURE StuffIt(pickDlg: DialogPtr; theItem: INTEGER; newVal: SmallFract); FORWARD;

 PROCEDURE WheelDraw(pickDlg: DialogPtr; wheelRect: Rect); FORWARD;
 PROCEDURE WheelPos2HS(pickDlg: DialogPtr; where: Point); FORWARD;
 PROCEDURE HS2WheelPos(pickDlg: DialogPtr; VAR where: Point); FORWARD;
 PROCEDURE WheelTintPattern(whichTint: INTEGER; VAR pat: Pattern); FORWARD;
 FUNCTION  WheelTrack(theDialog: DialogPtr; where: Point): BOOLEAN; FORWARD;


 FUNCTION  CallCDEFProc(varCode : Integer; theControl : ControlHandle; message : Integer;
 						param : LongInt; defProcPtr : ProcPtr) : LongInt;
	INLINE	$205F,	{MOVE.L     (A7)+, A0	;Get procPtr}
			$4E90;	{JSR        (A0)		;Call the defProc}

PROCEDURE CopyPix(src:PixMap;dst:BitMap; srcRect, dstRect: Rect;
					   mode: INTEGER; maskRgn: RgnHandle);
	  INLINE $A8EC;



{--------------------------------------------------------------------------------
 ArrowTrack - track mouse-down in component plus/minus controls.  All of the
 control accelerate, but they purposely do NOT accelerate to really great
 speed.  The maximum speed they attain is one that takes the user through the
 entire range of a given component (like hue or saturation) in a slightly
 leisurely fashion, so they can see everything.
 1.4a3	If the user option-clicks in an arrow, set the value to its upper or
 		lower limit and don't bother to keep tracking.
}

FUNCTION ArrowTrack(theDialog: DialogPtr; where: Point; option : Boolean): BOOLEAN;
VAR
	c, i:		INTEGER;
	delta:		Fixed;
	doDelay:	BOOLEAN;
	finalTicks:	LONGINT;
	nextDelta:	Fixed;
	goingUp:	BOOLEAN;
	item:		INTEGER;
	lastKick:	Fixed;
	midLine:	INTEGER;
	slopRect:	Rect;
	temp:		Fixed;
	up:			BOOLEAN;
BEGIN
	ArrowTrack := FALSE;
	item := -1;									{ signal if an arrow found }
	WITH PickInfoPtr(theDialog)^, theColor DO
			BEGIN
			{The only areas in cursorRgn besides the wheel are the inc/dec
				controls.  Since WheelTrack has already had a shot at this
				event, if it's in cursorRgn, it's ours.  First, figure out
				which control we're in.}
			FOR i := iFirstArrow TO iLastArrow DO
				BEGIN
				GetIRect(theDialog, i, slopRect);
				IF PtInRect(where, slopRect) THEN
					BEGIN
					ArrowTrack := TRUE;
					c := i - iFirstArrow;
					item := c + iFirstComponent;
					InsetRect(slopRect, -arrowSlop, -arrowSlop);
					WITH slopRect DO
						midLine := (bottom + top) DIV 2;
					LEAVE;	{exit FOR}
					END;
				END;

				IF item = -1 THEN EXIT(ArrowTrack);

			{Now we know which component we're in. If the user held the option
				key while clicking, slam the value to the upper or lower limit
				as appropriate.}
			IF option THEN
				BEGIN
				IF where.v < midLine THEN
					components[c] := Fix2SMallFract(MaxSmallFract)
				ELSE
					components[c] := Fix2SMallFract(0);
				ShowAffected(theDialog, item);
				END
			ELSE
				BEGIN

				{Now that we know which component the user is changing, track it.
					Like WheelTrack, we use repeat here so that the block executes
					at least once, and quick mouse clicks are seen.}
				doDelay := TRUE;
				REPEAT
					GetMouse(where);
					IF PtInRect(where, slopRect) THEN
						BEGIN
						up := (where.v < midLine);
						temp := SmallFract2Fix(components[c]);

						{Before inc/dec-ing the value, see if it's time to
							accelerate, or to quit accelerating because of a
							change in direction.}
						IF up <> goingUp THEN
							BEGIN
							goingUp := up;
							lastKick := temp;
							delta := arrowDeltaLow;
							nextDelta := arrowDeltaMed;
							END
						ELSE IF (Abs(temp-lastKick) >= nextDelta) &
							(temp MOD nextDelta = 0) THEN
							BEGIN
							lastKick := temp;
							delta := nextDelta;
							IF delta = arrowDeltaMed
								THEN nextDelta := arrowDeltaHi
								ELSE nextDelta := MaxSmallFract;
							END;

						{If there's still room to move in the current direction,
							do so.}
						IF (up & (temp < MaxSmallFract)) |
							(NOT up & (temp > 0)) THEN
							BEGIN
							IF up
								THEN temp := Min(MaxSmallFract, temp + delta)
								ELSE temp := Max(0, temp - delta);
							components[c] := Fix2SmallFract(temp);
							ShowAffected(theDialog, item);
							END

						{If the component we're tracking is hue, let the user
							wrap around, so they can have fun circling the wheel.
							lastKick is set so that the difference between it and
							temp will remain < MaxSmallFract, and delta is set so
							that the user keeps moving in the same sort of jumps
							that they were before passing zero hue.}
						ELSE IF (item = iHue) THEN
							BEGIN
							IF up THEN
								BEGIN
								lastKick := 1;
								temp := delta;
								END
							ELSE
								BEGIN
								lastKick := MaxSmallFract - 1;
								temp := MaxSmallFract - (MaxSmallFract MOD delta);
								END;
							temp := BAnd(temp, MaxSmallFract);
							components[c] := Fix2SmallFract(temp);
							ShowAffected(theDialog, item);
							END;
						END;

					{Now that we've reflected the new value, check to see if
						this is the first time 'round the loop.  If so, we do
						a small delay, to make life easier for slow clickers.}
					IF doDelay THEN
						BEGIN
						Delay(3, finalTicks);
						doDelay := FALSE;
						END;
				UNTIL NOT WaitMouseUp;
				END;

			{Regardless of where the user moused up, we've handled the mouse
				down; let the filter proc know so ModalDialog will ignore it.}
			END;
END;

{---------------------------------------------------------------------------------
 BrightAdjust - Adjust to new value in brightness control.  This routine
 is called by both of the scroll bar tracking action procedures (BrightTrack
 and BrightCtlTrack) once the new control value has been set.
}

PROCEDURE BrightAdjust(pickDlg: DialogPtr; ctlHand: ControlHandle; forceIt : Boolean);
VAR
	newBright:	SmallFract;
BEGIN
	{See what lightness value the new control value translates to.  If it's
		the same as the current lightness, none of picker's values have
		changed; don't bother updating them unless forceIt is set.}
	BrightCtlGet(ctlHand, newBright);
	WITH PickInfoPtr(pickDlg)^.theColor.hsx DO
		IF (newBright <> lightness) OR forceIt THEN
			BEGIN
			lightness := newBright;
			ShowNewValues(pickDlg, FALSE, [rgbText, lText, lWheel]);
			END;
END;

{--------------------------------------------------------------------------------
 BrightCtlGet - get the brightness control value into lightness range
 BrightCtlSet - set the brightness control value from lightness

 The following procedures convert between the brightness scroll bar control
 value (which ranges from -32768 to 32767), and lightness/value (which ranges
 from 0 to 1).  In doing so, they also invert the value, because the scroll
 bar minimum is at the top, where we want the lightness/value maximum to be.
}

PROCEDURE BrightCtlGet(ctlHand: ControlHandle; VAR lightness: SmallFract);
BEGIN
	lightness := Fix2SmallFract(MaxSmallFract - MAXINT - 1 - GetCtlValue(ctlHand));
END;

PROCEDURE BrightCtlSet(ctlHand: ControlHandle; lightness: SmallFract);
BEGIN
	SetCtlValue(ctlHand, MaxSmallFract - MAXINT - 1 - SmallFract2Fix(lightness));
END;

{--------------------------------------------------------------------------------
 BrightThumbTrack - update the colors while dragging the brightness thumb
}

PROCEDURE BrightThumbTrack;
CONST
	kArrowHeight	=	15;	{Height in pixels of a single scroll bar arrow}
	kThumbHeight	=	16;	{Height in pixels of the scroll bar thumb}
VAR
	dlgPtr:		DialogPtr;
	ctlHand:	ControlHandle;
	startValue:	LongInt;
	pThumbData:	ControlThumbInfoPtr;
	curMouse:	Point;
	deltaV:		LongInt;
	pixelRange:	LongInt;
	newValue:	LongInt;
	newBright:	SmallFract;
	oldPenState:PenState;
	gdType:		INTEGER;
BEGIN
	dlgPtr := DialogPtr(FrontWindow);
	gdType := PickInfoPtr(dlgPtr)^.gdType;
	ctlHand := ControlHandle(GetIHand(dlgPtr, iBrightCtl));
	startValue := GetCtlValue(ctlHand);
	pThumbData := ControlThumbInfoPtr(GetCRefCon(ctlHand));
	GetMouse(curMouse);

	IF PtInRect(curMouse, pThumbData^.slopRect)
	THEN
		{Calculate what the new control value would be if the user released
			the mouse button at this point. Too bad this isn't done for us.}
	BEGIN
		deltaV := curMouse.v - pThumbData^.startPoint.v;
		WITH ctlHand^^.contrlRect
		DO
			pixelRange := bottom - top - 2 * kArrowHeight - kThumbHeight;
		newValue := startValue + (deltaV * 65535) DIV pixelRange;
		IF newValue < -32768
		THEN
			newValue := -32768
		ELSE IF newValue > 32767
		THEN
			newValue := 32767;
	END
	ELSE
		newValue := startValue;

	newBright := Fix2SmallFract(MaxSmallFract - MAXINT - 1 - newValue);
	WITH PickInfoPtr(dlgPtr)^.theColor.hsx DO
		IF newBright <> lightness
		THEN
		BEGIN
			lightness := newBright;
			GetPenState(oldPenState);		{ control mgr uses weird xor mode }
			PenNormal;
			IF gdType = clutType											{ <8> }
			THEN
				ShowNewValues(dlgPtr, FALSE, [lText,rgbText,lWheel])		{ <8> }
			ELSE															{ <8> }
				ShowNewValues(dlgPtr, FALSE, [lText,rgbText]);				{ <8> }
			SetPenState(oldPenState);
		END

END;

{--------------------------------------------------------------------------------
 BrightTrack - track mouse-down inside brightness scroll bar
 				1.4a4	If we're on a clut device, do our color table animation
						of the wheel and selection color while the user is
						dragging the thumb in the scroll bar.
}

FUNCTION BrightTrack(theDialog: DialogPtr; where: Point): BOOLEAN;
VAR
	ctlHand:	ControlHandle;
	itemHand:	Handle;
	partCode:	INTEGER;
	upCode:		INTEGER;
	thumbData:	ControlThumbInfo;
	dummyResult:	LongInt;
BEGIN
	BrightTrack := FALSE;
	partCode := FindControl(where, WindowPtr(theDialog), ctlHand);
	itemHand := GetIHand(theDialog, iBrightCtl);

	IF (partCode <> 0) AND (ctlHand = ControlHandle(itemHand)) THEN
		BEGIN
		IF partCode = inThumb THEN
			BEGIN
				thumbData.startPoint := where;
				thumbData.limitRect.topLeft := where;
				{Ask the scroll bar defProc to calculate the slopRect we'll need in the
					action proc.}
				dummyResult := CallControl(ctlHand, thumbCntl, LongInt(@thumbData));
				SetCRefCon(ctlHand, LongInt(@thumbData));
				upCode := TrackControl(ctlHand, where, @BrightThumbTrack);
				BrightAdjust(theDialog, ctlHand, TRUE);
			END
		ELSE
			BEGIN
			PickInfoPtr(theDialog)^.whichPart := partCode;
			upCode := TrackControl(ctlHand, where, @BrightTrackScroll);
			END;

		{Regardless of where the user moused up, we've handled the mouse
			down; let the filter proc know so ModalDialog will ignore it.}
		BrightTrack := TRUE;
		END;
END;

{---------------------------------------------------------------------------------
 BrightTrackScroll - Adjust the value of the scroll bar for page/line up/down.
}

PROCEDURE BrightTrackScroll(theControl: ControlHandle; partCode: INTEGER);
VAR
	ctlVal: 	INTEGER;
	delta:		INTEGER;
	newVal:		LONGINT;
	pickDlg:	DialogPtr;
	up:			BOOLEAN;
BEGIN
	pickDlg := FrontWindow;
	WITH PickInfoPtr(pickDlg)^ DO
		IF (whichPart = partCode) THEN
			BEGIN
			{Figure out whether the user wants to scroll up or down.}
			IF partCode IN [inUpButton, inPageUp]
				THEN delta := -lineBright
				ELSE delta := lineBright;

			{Change movement to a page if appropriate.}
			IF partCode IN [inPageUp, inPageDown] THEN
				delta := delta * pageBright;

			{Hooray!  Now do the scroll.}
			ctlVal := GetCtlValue(theControl);
			newVal := ORD4(ctlVal) + delta;
			IF newVal > MAXINT THEN
				newVal := MAXINT
			ELSE IF newVal < (-MAXINT - 1) THEN
				newVal := (-MAXINT - 1);

			SetCtlValue(theControl, newVal);
			BrightAdjust(pickDlg, theControl, FALSE);
			END
END;

{---------------------------------------------------------------------------------
 CallControl - Safely get the control variant and definition procedure and call the
               control with the given message and parameter.
}

FUNCTION  CallControl(theControl : ControlHandle; message : Integer;
						param : LongInt) : LongInt;

VAR
	savedPort			:	GrafPtr;
	ctlProcHandle		:	Handle;
	savedState			:	SignedByte;

BEGIN
	CallControl := 0;	{This is the default, in case of error.}

	GetPort(savedPort);
	SetPort(theControl^^.contrlOwner);


	ctlProcHandle := Handle(StripAddress(Ptr(theControl^^.contrlDefProc)));
	IF ctlProcHandle^ = NIL THEN
		LoadResource(ctlProcHandle);

	IF ctlProcHandle^ <> NIL THEN BEGIN
		savedState := HGetState(ctlProcHandle);
		HLock(ctlProcHandle);
		CallControl := CallCDEFProc(GetCVariant(theControl), theControl, message, param, ctlProcHandle^);
		HSetState(ctlProcHandle, savedState);
	END;

	SetPort(savedPort);
END;

{---------------------------------------------------------------------------------
 CTabAdjust - adjust one or more of the colors in picker's color table
}

PROCEDURE CTabAdjust(pickDlg: DialogPtr; newBright: BOOLEAN);
VAR
	c:			PColor;
	hsx:		HSLColor;
	update:		BOOLEAN;
BEGIN
	WITH PickInfoPtr(pickDlg)^
	DO
	BEGIN
		IF colorOK
		THEN
		BEGIN
			WITH pickCTab^^, hsx
			DO
			BEGIN
				IF newBright
				THEN
					FOR c := cInput TO lastColor
					DO
					BEGIN
						update := TRUE;
						CASE c OF
							cSample:
								hsx := theColor.hsx;
							cInput:
								update := FALSE;
							cGray:
							BEGIN
								hue := 0;
								saturation := 0;
							END;
							OTHERWISE	{wheel colors}
							BEGIN
								IF wheelColors > minColors
								THEN
								BEGIN
									hue := Fix2SmallFract(FixRatio(ORD(c) - ORD(cSlice1), wheelColors div 2)mod 65536);
									IF(ORD(c) - ORD(cSlice1) >= wheelColors div 2)
									THEN
										saturation := 32767
									ELSE
										saturation := 65535;
								END
								ELSE
								BEGIN
									hue := Fix2SmallFract(FixRatio(ORD(c) - ORD(cSlice1), wheelColors));
									saturation := 65535;
								END
							END
						END; {case}
		
						IF update
						THEN
						BEGIN
							lightness := theColor.hsx.lightness;
							HSx2RGB(hsx, ctTable[ORD(c)].RGB);
						END;
					END
		
				{If the lightness is the same, only the color in the sample
					swatch (the one the user is picking) needs adjustment,
					and the new value is already computed.}
				ELSE
					ctTable[ORD(cSample)].RGB := theColor.rgb;
			END;  {with pickdlgptr}
{$IFC UsePalette}
			AnimatePalette(pickDlg,pickCTab,0,0,pickCTab^^.ctSize+1);
{$ENDC}
		END;  {if colorOK}
	END;
END;

{---------------------------------------------------------------------------------
 CTabBuild - build picker's color table, and the list of CLUT entries to borrow
}

{$IFC UsePalette}
PROCEDURE CTabBuild(VAR pickData: PickInfo; inColor: RGBColor);
VAR
	globRect: Rect;

BEGIN
	WITH pickData
	DO
	BEGIN
		{
		First, make sure we have color.  If not, we'll make do with black
		and white, avoid calling any of color quickdraw, and building
		the table is a null operation:  we don't need it

		Figure out whether or not we have color quickdraw, and enough
		bits per pixel to really use color.  For now, we check that the
		machine has a Mac II or later ROM to see if color Quickdraw
		is available.  There should be a better way to do this
		}

		wheelColors := minColors;
		pixSize := 1;
		colorOK := (BAnd(IntPtr(ROM85)^, BNot($3FFF)) = 0);

		IF colorOK
		THEN
		BEGIN
			globRect := dlgRec.window.port.portRect; { figure global rect }
			LocalToGlobal(globRect.topLeft);
			LocalToGlobal(globRect.botRight);

			itsGDev := GetMaxDevice(globRect);		{ find deepest device sect }

			IF itsGDev = NIL						{ if no sect, fake it }
			THEN
				itsGDev := GetGDevice;

			pixSize := PixMapHandle(itsGDev^^.gdPMap)^^.pixelSize;
			gdType := itsGDev^^.gdType;
			IF (gdType = clutType)
			THEN
				IF (pixSize >= 8)
				THEN
					wheelColors := maxColors
				ELSE IF (pixSize = 2)
				THEN
					wheelColors := -1				{ so all we get is input and sample }
		END;

		colorOK := colorOK AND (pixSize >= 2);
		lastColor := PColor(ORD(cSlice1) + wheelColors - 1);

		IF NOT colorOK
		THEN
		BEGIN
			pickCTab := NIL;
			pickPal := NIL;
			lastColor := PColor(0);
			EXIT (CTabBuild);
		END;

{Allocate our color table.}
		pickCTab := CTabHandle(NewHandle(
			Sizeof(ColorTable) + (Sizeof(ColorSpec) * ORD(lastColor))));

{Set up our color table.}
		WITH pickCTab^^
		DO
		BEGIN
			ctSize := ORD(lastColor);
			ctSeed := 0;
			ctFlags := 0;									{ 1.2 }
		END;

		{Next, figure out which CLUT entries to borrow.  For now, we get
			the widest possible distribution (try to avoid conflicts) by
			trying to match the hues at maximum brightness, the gray at
			medium range, and the input and current sample as complements.
			Surely there is better way to distribute the colors?}
		pickCTab^^.ctTable[ORD(cInput)].rgb := inColor;
		pickPal := NewPalette(pickCTab^^.ctSize+1,pickCTab,pmAnimated,0);
		SetEntryUsage(pickPal,ORD(cInput),pmTolerant,0);
		SetPalette(WindowPtr(@dlgRec),pickPal,TRUE);
		CTabAdjust(DialogPtr(@pickData), TRUE);
	END;
END;


{$ELSEC}  {UsePalette}
PROCEDURE CTabBuild(VAR pickData: PickInfo; inColor: RGBColor);
CONST
	HiliteRGB =		$DA0;
VAR
	c:				PColor;
	hiliteColor:	INTEGER;
	nextTry:		INTEGER;
	saveLight:		SmallFract;
	skipsAllowed:	INTEGER;
	tempColor:		RGBColor;
	saveGDev:		GDHandle;
	globRect:		Rect;			{ rect to globalize port in, for maxDev }

	FUNCTION BadChoice(index: INTEGER; c: PColor; doSkip: BOOLEAN): BOOLEAN;
	BEGIN
		IF index = hiliteColor THEN
			BadChoice := TRUE
		ELSE
			BadChoice := FALSE;
	END;

	FUNCTION Conflict(index: INTEGER; c: PColor): BOOLEAN;
	VAR
		p:		PColor;
	BEGIN
		{Return TRUE iff index is used in any earlier table entry.}
		IF BadChoice(index, c, FALSE) THEN
			Conflict := TRUE
		ELSE
			BEGIN
			Conflict := FALSE;
			WITH pickData.pickReqList^^ DO
				FOR p := cInput TO PRED(c) DO
					IF reqLData[ORD(p)] = index THEN
						BEGIN
						Conflict := TRUE;
						EXIT (Conflict);
						END;
			END;
	END;

	FUNCTION Taken(index: INTEGER; c: PColor): BOOLEAN;
	VAR
		p:		PColor;
	BEGIN
		{Return TRUE iff index, an entry we're considering stealing, is
			already in the table, and taking it would cause a conflict.}
		IF BadChoice(index, c, TRUE) THEN
			Taken := TRUE
		ELSE
			BEGIN
			Taken := FALSE;
			WITH pickData, pickReqList^^ DO
				FOR p := cInput TO lastColor DO
					IF reqLData[ORD(p)] = index THEN
						BEGIN
						Taken := TRUE;
						EXIT (Taken);
						END;
			END;
	END;

BEGIN	{CTabBuild}
	WITH pickData DO
		BEGIN
		{First, make sure we have color.  If not, we'll make do with black
			and white, avoid calling any of color quickdraw, and building
			the table is a null operation:  we don't need it.}
{$IFC GRUNGY}
		{Figure out whether or not we have color quickdraw, and enough
			bits per pixel to really use color.  For now, we check that the
			machine has a Mac II or later ROM to see if color Quickdraw
			is available.  There should be a better way to do this.}
		wheelColors := minColors;
		pixSize := 1;
		colorOK := (BAnd(IntPtr(ROM85)^, BNot($3FFF)) = 0);
		IF colorOK THEN
			BEGIN

			globRect := dlgRec.window.port.portRect; { figure global rect }
			LocalToGlobal(globRect.topLeft);
			LocalToGlobal(globRect.botRight);

			itsGDev := GetMaxDevice(globRect);		{ find deepest device sect }

			IF itsGDev = NIL						{ if no sect, fake it }
			THEN
				itsGDev := GetGDevice;

			pixSize := PixMapHandle(itsGDev^^.gdPMap)^^.pixelSize;
			gdType := itsGDev^^.gdType;
			IF (gdType = clutType) AND (pixSize >= 8) THEN
				wheelColors := maxColors;
			END;
		colorOK := colorOK AND (pixSize >= 4);
		lastColor := PColor(ORD(cSlice1) + wheelColors - 1);
{$ENDC}
		IF NOT colorOK THEN
			BEGIN
			pickCTab := NIL;
			pickReqList := NIL;
			EXIT (CTabBuild);
			END;

{For a time, work in the GDev containing the picker...}
		saveGDev := GetGDevice;
		SetGDevice(itsGDev);
		itsGDev^^.gdPMap^^.pmTable^^.ctSeed := GetCTSeed;

{Allocate our color table.}
		pickCTab := CTabHandle(NewHandle(
			Sizeof(ColorTable) + (Sizeof(ColorSpec) * ORD(lastColor))));

{Set up our color table.}
		WITH pickCTab^^ DO
			BEGIN
			ctSize := ORD(lastColor);
			ctSeed := 0;
			ctFlags := 0;									{ 1.2 }

		{Next, figure out which CLUT entries to borrow.  For now, we get
			the widest possible distribution (try to avoid conflicts) by
			trying to match the hues at maximum brightness, the gray at
			medium range, and the input and current sample as complements.
			Surely there is better way to distribute the colors?}
		WITH pickCTab^^ DO
			BEGIN
			CTabAdjust(DialogPtr(@pickData), TRUE);
			ctTable[ORD(cInput)].rgb := inColor;
			END;

		{Now that we've distributed the colors, call GetSubTable to fill in
			the indices of the nearest matches among current CLUT entries.}
		GetSubTable(pickCTab, 4, NIL);

		{Allocate the request list.}
		pickReqList := ReqListHandle(NewHandle(
			Sizeof(ReqListRec) + (Sizeof(INTEGER) * ORD(lastColor))));

		{Set up the request list, which we'll need to save and restore
			the colors set when we were called, and those we borrow.}
		WITH pickCTab^^, pickReqList^^ DO
			BEGIN
			reqLSize := ORD(lastColor);
			FOR c := cInput TO lastColor DO
				reqLData[ORD(c)] := ctTable[ORD(c)].value;

			{See if the CLUT indices returned by GetSubTable are unique.
				If not, resolve the conflicts by finding other unmatched
				entries.}
			tempColor := RGBColorPtr(HiliteRGB)^;
			hiliteColor := Color2Index(tempColor);

			{Figure out how many colors we can pass up as poor choices for
				conflict resolution.  The value computed below is:

						Number of entries in the color table
					-	Number of entries we require
					+2	(Entries for black and white, which we must skip)
					-1	(One held in reserve for skipping the hilite color)
				------------------------------------------------------------
						skips we can make before we run out

			The entries we skip over (in Conflict and Taken) are the current
				hilight color and, if possible, grays.  Grays are skipped
				because they may be used in gray scale fonts, which may be
				on the screen, and look remarkable ugly animated.}
			skipsAllowed := BSL(1, pixSize) - (ORD(lastColor) + 1) + 1;

			nextTry := BSL(1, pixSize) - 1;
			FOR c := cInput TO lastColor DO
				BEGIN
				IF Conflict(reqLData[ORD(c)], c) THEN
					BEGIN
					WHILE Taken(nextTry, c) DO
						nextTry := nextTry - 1;
					reqLData[ORD(c)] := nextTry;
					ctTable[ORD(c)].value := nextTry;
					nextTry := nextTry - 1;
					END;
				END;
			END;

		SetGDevice(saveGDev);				{restore old GDev}

		END; {with pickData}
END; {CTabBuild}
{$ENDC}		{ UsePalette }

{---------------------------------------------------------------------------------
 CTabInstall - (re)install color picker's color table to get new colors
}

PROCEDURE CTabInstall(pickDlg: DialogPtr; newBright: BOOLEAN);
VAR
	loneReq:	ReqListRec;
	loneColor:	CTabHandle;
	saveGDev:	GDHandle;
BEGIN
{$IFC NOT UsePalette}
	WITH PickInfoPtr(pickDlg)^ DO
		IF colorOK THEN
			IF newBright THEN
				BEGIN
				HLock(Handle(pickReqList));
				saveGDev := GetGDevice;
				SetGDevice(itsGDev);
				RestoreEntries(pickCTab, NIL, pickReqList^^);
				SetGDevice(saveGDev);

				HUnlock(Handle(pickReqList));
				END
			ELSE
				BEGIN
				loneColor := CTabHandle (NewHandle (SIZEOF (ColorTable)));
				WITH loneColor^^ DO
					BEGIN
					ctSeed := pickCTab^^.ctSeed;
					ctFlags := pickCTab^^.ctFlags;					{ 1.2 }
					ctSize := 0;
					ctTable[0] := pickCTab^^.ctTable[ORD(cSample)];
					END;
				WITH loneReq DO
					BEGIN
					reqLSize := 0;
					reqLData[0] := pickReqList^^.reqLData[ORD(cSample)];
					END;

				saveGDev := GetGDevice;
				SetGDevice(itsGDev);
				RestoreEntries(loneColor, NIL, loneReq);
				SetGDevice(saveGDev);

				DisposHandle (Handle (loneColor));
				END;
{$ENDC}
END;

{---------------------------------------------------------------------------------
 CTabRestore - restore color table to pre-picker condition
}

PROCEDURE CTabRestore(pickPtr: PickInfoPtr);
VAR
	saveGDev: GDHandle;
BEGIN
{$IFC NOT UsePalette}
	WITH pickPtr^ DO
		IF colorOK THEN
			BEGIN
			HLock(Handle(pickReqList));

			saveGDev := GetGDevice;
			SetGDevice(itsGDev);
			RestoreEntries(saveCTab, NIL, pickReqList^^);
			SetGDevice(saveGDev);

			HUnlock(Handle(pickReqList));
			DisposHandle(Handle(saveCTab));
			END;
{$ENDC}
END;

{---------------------------------------------------------------------------------
 CTabSave - save pre-picker color table
}

PROCEDURE CTabSave(pickPtr: PickInfoPtr);
VAR
	saveGDev:	GDHandle;
BEGIN
{$IFC NOT UsePalette}
	{We know which CLUT slots we're going to borrow, and what colors to
		replace them with.  All that remains is to save the current entries,
		so we can restore them before leaving.}
	WITH pickPtr^ DO
		IF colorOK THEN
			BEGIN
			saveCTab := CTabHandle(NewHandle(0));
			HLock(Handle(pickReqList));
			saveGDev := GetGDevice;
			SetGDevice(itsGDev);
			SaveEntries(NIL, saveCTab, pickReqList^^);
			SetGDevice(saveGDev);
			HUnlock(Handle(pickReqList));
			END
		ELSE
			saveCTab := NIL;
{$ENDC}
END;

{---------------------------------------------------------------------------------
 CTabSearch - return index of CLUT entry we requested
}

FUNCTION CTabSearch(myColor: RGBColor; VAR index: LONGINT): BOOLEAN;
BEGIN

	{Return true (and a CLUT index) iff the main color picker dialog is
		the one requesting the color.  If so, the client ID will be set
		and we can get at picker's vars via FrontWindow.
	We don't really do any searching, since the table may have duplicate
		entries; instead we return the index CLUT represented in whichColor.
		We've known that all along, but without color quickdraw routines
		IndexForeColor and IndexBackColor, the search proc is the only way
		to coerce the CLUT entries we want.
	We don't need to worry about checking colorOK here, since this routine
		is called iff it's true.
	Finally, do nothing unless we're on a CLUT device; on a fixed device, we
		want the default search proc to get the closest current match.}

	CTabSearch := FALSE;
	WITH GetGDevice^^
	DO
		IF (gdID = pickClientID) AND
			(myColor.red = 12) AND
			(myColor.green = 34)
		THEN
			WITH PickInfoPtr(FrontWindow)^.pickCTab^^.ctTable[myColor.blue]
			DO
			BEGIN
				CTabSearch := TRUE;
				IF gdType = clutType
				THEN
					index := value
				ELSE
					IF (gdPMap^^.pixelSize = 32)
					THEN
						index := (BAnd(LONGINT(rgb.red),$0000FFFF) div 256) * 256*256 +
								(BAnd(LONGINT(rgb.green),$0000FFFF) div 256) * 256 +
								(BAnd(LONGINT(rgb.blue),$0000FFFF) div 256)
					ELSE
						index := (BAnd(LONGINT(rgb.red),$0000FFFF) div 2048) * 32*32 +
								(BAnd(LONGINT(rgb.green),$0000FFFF) div 2048) * 32 +
								(BAnd(LONGINT(rgb.blue),$0000FFFF) div 2048);
			END;
END;

{---------------------------------------------------------------------------------
 Dither1, Dither50 - set up a dither pattern
}

PROCEDURE Dither50(which1, which2: pColor);
	{
	Set the foreground color to which1, and the
	background color to which2.
	Set the pattern to 50% gray.
	}
BEGIN
{$IFC UsePalette}
	PmForeColor(ORD(which1));
	PmBackColor(ORD(which2));
{$ELSEC}
	anRGB.red := 12;
	anRGB.green := 34;
	anRGB.blue := ORD(which1);
	RGBForeColor(anRGB);
	anRGB.blue := ORD(which2);
	RGBNormal;
{$ENDC}
	PenPat (GetQDGlobals^.gray);
END;


PROCEDURE Dither1(which1: pColor);
VAR
	anRGB:		RGBColor;
BEGIN
{$IFC UsePalette}
	PmForeColor(ORD(which1));
{$ELSEC}
	anRGB.red := 12;
	anRGB.green := 34;
	anRGB.blue := ORD(which1);
	RGBForeColor(anRGB);
{$ENDC}
	PenPat (GetQDGlobals^.black);
END;

{---------------------------------------------------------------------------------
 DeviceCTab - return handle to the current graphics device's CLUT, if any
}

FUNCTION  DeviceCTab: CTabHandle;
BEGIN
	DeviceCTab := PixMapHandle(GetGDevice^^.gdPMap)^^.pmTable;
END;

{---------------------------------------------------------------------------------
 DeviceITab - return handle to the current graphics device's inverse table, if any
}

FUNCTION  DeviceITab: ITabHandle;
BEGIN
	DeviceITab := GetGDevice^^.gdITable;
END;

{---------------------------------------------------------------------------------
 RGBNormal - Fore and back to black and white, please
}

PROCEDURE  RGBNormal;
BEGIN

	IF (BAnd(IntPtr(ROM85)^, BNot($3FFF)) = 0)
	THEN
	BEGIN
		RGBForeColor(RGBColorPtr(RGBBlack)^);
		RGBBackColor(RGBColorPtr(RGBWhite)^);
	END;
END;

{--------------------------------------------------------------------------------
 GetIHand - get handle for given dialog item
 GetIRect - get rectangle for given dialog item
}

FUNCTION GetIHand(theDialog: DialogPtr; item: INTEGER): Handle;
VAR
	itemHand:	Handle;
	itemRect:	Rect;
	itemType:	INTEGER;
BEGIN
	GetDItem(theDialog, item, itemType, itemHand, itemRect);
	GetIHand := itemHand;
END;

PROCEDURE GetIRect(theDialog: DialogPtr; item: INTEGER; VAR itemRect: Rect);
VAR
	itemType:	INTEGER;
	itemHand:	Handle;
BEGIN
	GetDItem(theDialog, item, itemType, itemHand, itemRect);
END;

{---------------------------------------------------------------------------------
 GetQDGlobals - return pointer to quickdraw globals
}

FUNCTION  GetQDGlobals: QDGlobalsPtr;
TYPE
	LPtr = ^LONGINT;
	LPtrPtr = ^LPtr;
BEGIN
	{Get pointer to thePort, which is at offset zero from A5, and adjust
		it to point at the base of the quickdraw globals.  We have to do
		this to reference those globals from a PACK.  $904 is the low
		memory variable CurrentA5.}
	GetQDGlobals := QDGlobalsPtr(
		LPtrPtr($904)^^ + Sizeof(GrafPtr) - Sizeof(QDGlobals));
END;

{---------------------------------------------------------------------------------
 HSx2RGB - convert HSL/HSV color to RGB, based on HSV compile time flag
 RGB2HSx - convert RGB color to HSL/HSV, based on HSV compile time flag
}

PROCEDURE HSx2RGB(hsx: HSLColor; VAR rgb: RGBColor);
BEGIN
{$IFC HSV}
		HSV2RGB(HSVColor(hsx), rgb);
{$ELSEC}
		HSL2RGB(hsx, rgb);
{$ENDC}
END;

PROCEDURE RGB2HSx(rgb: RGBColor; VAR hsx: HSLColor);
BEGIN
{$IFC HSV}
		RGB2HSV(rgb, HSVColor(hsx));
{$ELSEC}
		RGB2HSL(rgb, hsx);
{$ENDC}
END;

{--------------------------------------------------------------------------------
 PickDlgClose - close down the color picker dialog
}

PROCEDURE PickDlgClose(infoPtr: PickInfoPtr);
VAR
	itemList:		Handle;
	saveGDev:		GDHandle;
BEGIN
	WITH infoPtr^ DO
		BEGIN
		DisposeRgn(wheelRgn);
		DisposeRgn(slopRgn);
		DisposeRgn(cursorRgn);
		ReleaseResource(Handle(cursorHand));

		itemList := dlgRec.items;

		IF colorOK THEN
			BEGIN
			{Restore pre-picker colors, and dispose of related memory.}
			CTabRestore(infoPtr);
			DisposHandle(Handle(pickCTab));
{$IFC NOT UsePalette}
			DisposHandle(Handle(pickReqList));
			{Get rid of the search proc we added.}

			saveGDev := GetGDevice;
			SetGDevice(itsGDev);
			DelSearch(@CTabSearch);
			SetGDevice(saveGDev);
{$ENDC}
			{ADD: Use QDErr here, to check that the rebuild succeeded.}
			END;

		CloseDialog(DialogPtr(infoPtr));

		IF colorOK THEN 				{<9>}
			DisposePalette(pickPal);	{<9>}
		END;

		DisposHandle(itemList);

		InitCursor;
END;

{--------------------------------------------------------------------------------
 PickDlgDrawItem - draw requested user item in picker dialog
}

PROCEDURE PickDlgDrawItem(theWindow: WindowPtr; item: INTEGER);
VAR
	fontStuff:	FontInfo;
	itemRect: 	Rect;
	version:	Str255;
	arrowPic:	PicHandle;
BEGIN
	SetPort(GrafPtr(theWindow));
	GetIRect(DialogPtr(theWindow), item, itemRect);

	CASE item OF
		iOkOutline:
			BEGIN
			PenNormal;
			PenSize(3,3);
			FrameRoundRect(itemRect, 16, 16);
			END;

		iWheel:
			WheelDraw(DialogPtr(theWindow), itemRect);

		iOldSample,
		iNewSample:
			SampleDraw(DialogPtr(theWindow), item, itemRect);

		iVersion:
			BEGIN
			TextFont(applFont);
			TextSize(9);
			GetWTitle(theWindow, version);
			GetFontInfo(fontStuff);
			WITH itemRect DO
				MoveTo(left, top+fontStuff.ascent);
			DrawString(version);
			END;

		OTHERWISE
			BEGIN
				arrowPic := GetPicture(rArrowPic);
				DrawPicture(arrowPic,itemRect);
			END;
		END;

	RGBNormal;
	PenNormal;
	TextFont(systemFont);
	TextSize(0);
END;

{--------------------------------------------------------------------------------
 PickDlgFilter - respond to mousing around in color wheel, etc
}

FUNCTION PickDlgFilter(theDialog: DialogPtr; VAR theEvent: EventRecord; VAR itemHit: INTEGER): BOOLEAN;
CONST
	kUpArrow	=	$1E;	{Character codes (NOT key codes) for the arrow keys}
	kDnArrow	=	$1F;
	kLtArrow	=	$1C;
	kRtArrow	=	$1D;

VAR
	key:			INTEGER;
	itemHand:		Handle;
	s:				Str255;
	savePort:		GrafPtr;
	wantOurCursor:	BOOLEAN;
	where:			Point;
	btnControl:		ControlHandle;
	dummyTicks:		LongInt;

	thatPixel:		RGBColor;
	oldColor:		RGBColor;
BEGIN
	PickDlgFilter := FALSE;
	GetPort(savePort);
	SetPort(theDialog);

	{Handle mouse commands in the color wheel and the increment/decrement
		arrows for the color specs.  Also handle keyboard shortcuts for
		fine cursor control, and mimic the standard filter's behavior for
		the return and enter keys.}

	GetMouse(where);
	WITH PickInfoPtr(theDialog)^				{ This used to be in the nullevent action, }
	DO											{ but System 7 didn't give us nulls! -dvb  }
	BEGIN
		wantOurCursor := PtInRgn(where, cursorRgn);
		IF wantOurCursor <> ourCursor
		THEN
		BEGIN
			IF ourCursor
			THEN
				SetCursor(GetQDGlobals^.arrow)
			ELSE
				SetCursor(cursorHand^^);
			ourCursor := wantOurCursor;
		END;
	END;

	where := theEvent.where;
	GlobalToLocal(where);

	CASE theEvent.what OF
		autoKey,															{1.2b1}
		keyDown:
			BEGIN
			WITH theEvent DO
				key := BAnd(message, charCodeMask);

			IF (BAnd(theEvent.modifiers, cmdKey) <> 0) OR (key=27) THEN
			{Handle pseudo key equivalents for Cut, Copy, and Paste}
				BEGIN
					CASE Chr(key) OF
						'x', 'X' :
							BEGIN
							DlgCut(theDialog);
							SnareNewComponents(theDialog);
							END;
						'c', 'C' :
							BEGIN
							DlgCopy(theDialog);
							SnareNewComponents(theDialog);
							END;
						'v', 'V' :
							BEGIN
							DlgPaste(theDialog);
							SnareNewComponents(theDialog);
							END;
						'.', chr(27) :
							BEGIN
							btnControl := ControlHandle(GetIHand(theDialog, cancel));
							HiliteControl(btnControl, inButton);
							Delay(8, dummyTicks);
							HiliteControl(btnControl, 0);
							itemHit := cancel;
							END;
						OTHERWISE
							SysBeep(1);
						END;
					PickDlgFilter := TRUE;
				END

			ELSE IF (key = $03) OR (key = $0D) THEN
			{Make return and enter the same as pushing the OK button.}
				BEGIN
				SnareNewComponents(theDialog);
				btnControl := ControlHandle(GetIHand(theDialog, ok));
				HiliteControl(btnControl, inButton);
				Delay(8, dummyTicks);
				HiliteControl(btnControl, 0);
				itemHit := OK;
				PickDlgFilter := TRUE;
				END

			{If the user is tabbing, get any new value in field they're
				leaving.  Note that PickDlgFilter remains FALSE, so that
				ModalDialog will change fields in response to the tab.}
			ELSE IF (key = $09) THEN
				SnareNewComponents(theDialog)

			{If the key isn't a digit or backspace or one of the arrow keys,
				ignore it, and indicate to ModalDialog that we've handled
				the (unwanted) event.}
			ELSE IF (key < ORD('0')) OR (key > ORD('9')) THEN
				BEGIN
				IF (key <> $08) AND (key <> kUpArrow) AND (key <> kDnArrow) AND
				   (key <> kLtArrow) AND (key <> kRtArrow) THEN
					BEGIN
					SysBeep(1);
					PickDlgFilter := TRUE;
					END;
				END

			{The key is a digit.  But if the field less the current selection
				is already five digits wide, we don't even want a digit.}
			ELSE WITH DialogPeek(theDialog)^ DO
				BEGIN
				itemHand := GetIHand(theDialog, editField + 1);
				GetIText(itemHand, s);
				WITH textH^^ DO
					IF (Length(s) - (selEnd - selStart) >= 5) THEN
						BEGIN
						SysBeep(1);
						PickDlgFilter := TRUE;
						END;
				END;
			END;

		mouseDown:
			BEGIN

				IF ( (NOT PtInRect(where,theDialog^.portRect)) AND
						(BAnd(theEvent.modifiers, optionKey) <> 0) )
				THEN
				BEGIN
					oldColor := PickInfoPtr(theDialog)^.theColor.rgb; 
					REPEAT
						IF (PtInRect(where,theDialog^.portRect))
						THEN
							thatPixel := oldColor
						ELSE
							GetCPixel(where.h,where.v,thatPixel);
						PickInfoPtr(theDialog)^.theColor.rgb := thatPixel;
						ShowNewValues(theDialog, TRUE, [rgbText..bright]);
						GetMouse(where);
					UNTIL NOT WaitMouseUp;
	
					PickDlgFilter := TRUE;
				END
				ELSE
				BEGIN
					SnareNewComponents(theDialog);
					PickDlgFilter :=
							BrightTrack(theDialog, where) |
							WheelTrack(theDialog, where) |
							SampleTrack(theDialog, where) |
							ArrowTrack(theDialog, where, BAnd(theEvent.modifiers, optionKey) <> 0);
				END;
			END;
		END; {case}

	SetPort(savePort);
END;

{--------------------------------------------------------------------------------
 PickDlgOpen - get the picker dialog and set it up
 				If where is (-1, -1), then put the dialog on the best screen, with
				the measure of "goodness" coming from this ordering:
either 1-bit, mono 2-bit, color 2-bit, mono 4-bit, mono 8-bit, color >= 4
}

FUNCTION PickDlgOpen(infoPtr: PickInfoPtr; where: Point; prompt: Str255): DialogPtr;
VAR
	useBest:		BOOLEAN;		{true if v=h=-1}
	i:				INTEGER;
	truncResult:	INTEGER;		{Result of TruncString call... ignored <12>}
	itemHand:		Handle;			{Dialog item handle got by GetDItem}
	itemRect:		Rect;			{Dialog item rectangle got by GetDItem}
	itemType:		INTEGER;		{Dialog item type got by GetDItem}
	pickDlg:		DialogPtr;		{Dialog pointer we'll return eventually}
	pickPltt:		PaletteHandle;	{Dialog palette to avoid the ActivatePalette}
	aDevice:		GDHandle;		{Handle for traversing device list}
	bestDevice:		GDHandle;		{Handle to the "best" device}
	devRating:		Integer;		{Value of device's "goodness"}
	bestDevRating:	Integer;		{Value of best device's "goodness"}
BEGIN
	{Get the dialog from the system resource file.}
	pickDlg := GetNewDialog(rPickDlg, Pointer(infoPtr), WindowPtr(-1));
	SetPort (pickDlg);														{1.2b1}
{$IFC NOT UsePalette}
	IF (BAnd(IntPtr(ROM85)^, BNot($3FFF)) = 0) THEN	{Same test as info^.colorOk}											{1.2b1}
		BEGIN
		pickPltt := NewPalette(1, NIL, pmCourteous, 0);
		SetPalette(pickDlg, pickPltt, TRUE)
		END;
{$ENDC}
	{Make the user items reference the procedures to draw them.}
	FOR i := iFirstUI TO iLastUI DO
		BEGIN
		GetDItem(pickDlg, i, itemType, itemHand, itemRect);
		SetDItem(pickDlg, i, itemType, Handle(@PickDlgDrawItem), itemRect);

		{For the wheel, use the itemRect to determine the region of the
			enclosed circle, which we'll need for hit testing later.}
		IF i = iWheel THEN
			WITH infoPtr^ DO
				BEGIN
				wheelRgn := NewRgn;
				OpenRgn;
				FrameOval(itemRect);
				CloseRgn(wheelRgn);
				WITH itemRect DO
					BEGIN
					wheelRadius := FixRatio(right-left, 2);
					WITH wheelCenter DO
						BEGIN
						h := (left + right) DIV 2;
						v := (top + bottom) DIV 2;
						END;
					END;
				slopRgn := NewRgn;
				OpenRgn;
				InsetRect(itemRect, -slopRim, -slopRim);
				FrameOval(itemRect);
				CloseRgn(slopRgn);
				END;
		END;

	{Set up cursor variables.}
	WITH infoPtr^ DO
		BEGIN
		cursorRgn := NewRgn;
		OpenRgn;
	{	FOR i := iFirstArrow TO iLastArrow DO
			BEGIN
			GetIRect(pickDlg, i, itemRect);
			FrameRect(itemRect);
			END; }
		GetIRect(pickDlg, iOldSample, itemRect);
		FrameRect(itemRect);
		CloseRgn(cursorRgn);
		UnionRgn(wheelRgn, cursorRgn, cursorRgn);
		cursorHand := GetCursor(rCursor);
		SetCursor(GetQDGlobals^.arrow);  {08Aug89 CSD Vers 1.3 -- Fixes BRC 30423}
		ourCursor := FALSE;

		HS2WheelPos(pickDlg, hotSpot);
		END;

	{Position the dialog.  If where is (0,0), position the dialog neatly
		for the user: center it horizontally, and leave one third of the
		margin above and two thirds below vertically.}
	WITH where DO
		BEGIN
		useBest := (h = -1) AND (v = -1);
		IF ((h = 0) AND (v = 0)) OR (useBest AND
				NOT (BAnd(IntPtr(ROM85)^, BNot($3FFF)) = 0)) THEN
			BEGIN
			WITH GetQDGlobals^.screenBits.bounds DO
				BEGIN
				h := right - left;
				v := bottom - top - IntPtr(mBarHeight)^;
				END;
{$IFC GRUNGY}
			{Using the portRect here looks wrong on the Mac+, because the
				portRect does not include the (outset) border of the dialog.
				We could get the rectangle, including the border, from the
				structure region's bounding box, but really need a cleaner
				way to do this…}
			WITH pickDlg^.portRect DO
				BEGIN
				h := (h - (right - left)) DIV 2;
				v := (v - (bottom - top)) DIV 3 + IntPtr(mBarHeight)^;
				END;
{$ENDC}
			END
		ELSE IF useBest THEN
			BEGIN
			bestDevRating := 0;
			aDevice := GetDeviceList;
			WHILE aDevice <> NIL DO
				BEGIN
				IF NOT TestDeviceAttribute(aDevice, screenActive) THEN
					devRating := 0
				ELSE IF aDevice^^.gdPMap^^.pixelSize >= 16 THEN
					devRating := 8 + aDevice^^.gdPMap^^.pixelSize
				ELSE
					CASE BAnd(aDevice^^.gdFlags, 1) * 128 + aDevice^^.gdPMap^^.pixelSize OF
						1	:	devRating := 1;
						129	:	devRating := 2;
						2	:	devRating := 3;
						130	:	devRating := 4;
						4	:	devRating := 5;
						8	:	devRating := 6;
						132	:	devRating := 7;
						136	:	devRating := 8;
						END;
				IF (devRating > bestDevRating) OR
						((devRating = bestDevRating) AND (aDevice = GetMainDevice)) {settle tie}
				THEN
					BEGIN
					bestDevRating := devRating;
					bestDevice := aDevice;
					END;
				aDevice := GetNextDevice(aDevice);
				END;

			WITH bestDevice^^.gdRect DO
				BEGIN
				h := right - left;
				IF bestDevice = GetMainDevice THEN
					v := bottom - top - IntPtr(mBarHeight)^
				ELSE
					v := bottom - top;
				END;
{$IFC GRUNGY}
			{Using the portRect here looks wrong on the Mac+, because the
				portRect does not include the (outset) border of the dialog.
				We could get the rectangle, including the border, from the
				structure region's bounding box, but really need a cleaner
				way to do this…}
			WITH pickDlg^.portRect DO
				BEGIN
				h := (h - (right - left)) DIV 2 + bestDevice^^.gdRect.left;
				IF bestDevice = GetMainDevice THEN
					v := (v - (bottom - top)) DIV 3 + IntPtr(mBarHeight)^ + bestDevice^^.gdRect.top
				ELSE
					v := (v - (bottom - top)) DIV 3 + bestDevice^^.gdRect.top;
				END;
{$ENDC}
			END;
		MoveWindow(WindowPtr(pickDlg), h, v, FALSE);

	END;

	{Stuff the user's prompt.}
	itemHand := GetIHand(pickDlg, iPrompt);
	GetIRect(pickDlg, iPrompt, itemRect);												{<12>}
	truncResult := TruncString( (itemRect.right - itemRect.left) - 1, prompt, smTruncEnd );	{<12>}
	SetIText(itemHand, prompt);

	{Convert the two color specs to numbers that the user can edit.}

	PickDlgOpen := pickDlg;
END;

{--------------------------------------------------------------------------------
 RemodelDlg - display and respond to remodel dialog
}

PROCEDURE RemodelDlg(pickRect: Rect; VAR showModel, showAltModel: BOOLEAN);
CONST
	iOkRim =		3;
	iRmTitle =		4;
	iShowModel =	5;
	iShowAltModel =	6;
	iFirstAltModel =7;
	iRGBModel =		7;
	iCMYModel =		8;
	iHSLModel =		9;
	iLastAltModel =	9;
VAR
	ctlHand:		ControlHandle;
	dlgPtr:			DialogPtr;
	dlgRec:			DialogRecord;
	h, v:			INTEGER;
	itemHand:		Handle;
	itemHit:		INTEGER;
	itemList:		Handle;
	itemRect:		Rect;
	itemType:		INTEGER;
	newCtlValue:	INTEGER;
	whichRadio:		INTEGER;


BEGIN
END;

{--------------------------------------------------------------------------------
 RemodelDlgDrawItem - draw user items in remodel dialog
}

PROCEDURE RemodelDlgDrawItem(theWindow: WindowPtr; item: INTEGER);
VAR
	itemRect: Rect;
BEGIN
END;

{--------------------------------------------------------------------------------
 SampleDraw - draw one of the color samples in the dialog
}

PROCEDURE SampleDraw(pickDlg: DialogPtr; item: INTEGER; sampleRect: Rect);
VAR
	height:			INTEGER;
	width:			INTEGER;
	whichSample:	PColor;
	saveID:			INTEGER;
	saveGDev:		GDHandle;
BEGIN
	WITH sampleRect DO
		BEGIN
		height := bottom - top;
		width := right - left;

		IF item = iNewSample THEN
			BEGIN
			MoveTo(left, bottom);
			height := -height;		{flip height so open side of box border is down}
			whichSample := cSample;
			bottom := bottom + 1;
			END
		ELSE
			BEGIN
			whichSample := cInput;
			top := top - 1;
			MoveTo(left, top);		{after top-1 so that pen hanging down & right works}
			END;
		END;
	{Draw the open sided box around the sample first.}
	Line(0, height);
	Line(width-1, 0);
	Line(0, -height);

	InsetRect(sampleRect, 1, 1);

	WITH pickInfoPtr(pickDlg)^
	DO
	BEGIN
		IF colorOK
		THEN
			IF gdType <> clutType					{ a fixed device? <8> dvb }
			THEN											{ <8> dvb }
			BEGIN											{ <8> dvb }
				IF whichSample = cInput						{ <8> dvb }
				THEN										{ <8> dvb }
					RGBForeColor(rgbInput)					{ <8> dvb }
				ELSE										{ <8> dvb }
					RGBForeColor(theColor.rgb);				{ <8> dvb }
				PaintRect(sampleRect);						{ <8> dvb }
			END												{ <8> dvb }
			ELSE											{ <8> dvb }
			BEGIN											{ <8> dvb }
{$IFC UsePalette}											{ <8> dvb }
				Dither1(whichSample);						{ <8> dvb }
				PaintRect(sampleRect);						{ <8> dvb }
{$ELSC}														{ <8> dvb }
				saveGDev := GetGDevice;						{ save current gDevice }
				SetGDevice(itsGDev);						{ set to our gDev }
				saveID := itsGDev^^.gdID;					{ save old client }
				SetClientID(pickClientID);					{ set client to us, for matching }
				SetGDevice(saveGDev);						{ set back to primary gDev }
				Dither1(whichSample);
				PaintRect(sampleRect);						{ paint the rect }
				SetGDevice(itsGDev);						{ set to our GDev }
				SetClientID(saveID);						{ restore client ID }
				SetGDevice(saveGDev);						{ set to real gDev }
{$ENDC}														{ <8> dvb }
			END
		ELSE		{colorOK}
			PaintRect(sampleRect);						{ paint the rect black }
	END;
END;

{---------------------------------------------------------------------------------
 SampleTrack - During mouse down in input sample, reset output to that color
}

FUNCTION SampleTrack(theDialog: DialogPtr; where: Point): BOOLEAN;
VAR
	inputShown:	BOOLEAN;
	newPt:		Point;
	saveRGB:	RGBColor;
	slopRect:	Rect;
	wantInput:	BOOLEAN;
BEGIN
	SampleTrack := FALSE;
	GetIRect(theDialog, iOldSample, slopRect);

	WITH PickInfoPtr(theDialog)^, theColor DO
		IF PtInRect(where, slopRect) THEN
			BEGIN
			{Save the current color, in case the user drags out of the
				sample and mouses up there.  Fudge where so that it and newPt
				aren't equal the first time through the loop, so that the
				color at mouseDown gets set.}
			saveRGB := rgb;
			InsetRect(slopRect, -inputSlop, -inputSlop);
			SetPt(where, 0, 0);
			inputShown := FALSE;

			{While the user remains in the input swatch, reset the current
				color to it.  Revert to the saved value if user drifts out.
				Like WheelTrack, we use repeat here so that the block runs
				at least once, and quick mouse clicks are seen.}
			REPEAT
				GetMouse(newPt);
				IF NOT EqualPt(where, newPt) THEN
					BEGIN
					where := newPt;
					wantInput := PtInRect(newPt, slopRect);
					IF wantInput <> inputShown THEN
						BEGIN
						IF wantInput
							THEN rgb := rgbInput
							ELSE rgb := saveRGB;
						ShowNewValues(theDialog, TRUE, [rgbText..bright]);
						inputShown := wantInput;
						END;
					END;
			UNTIL NOT WaitMouseUp;

			{Regardless of where the user moused up, we've handled the mouse
				down; let the filter proc know so ModalDialog will ignore it.}
			SampleTrack := TRUE;
			END;
END;

{--------------------------------------------------------------------------------
 ShowAffected - call ShowNewValues, as appropriate for changed color component
}

PROCEDURE ShowAffected(pickDlg: DialogPtr; item: INTEGER);
VAR
	affected:		ColorItemSet;
BEGIN
	CASE item OF
		iRed, iBlue, iGreen:
			affected := [rgbText, hsText, lText, hsWheel, lWheel, bright];
		iHue, iSaturation:
			affected := [hsText, rgbText, hsWheel];
		iLightness:
			affected := [lText, rgbText, lWheel, bright];
		END;

	{Changing the saturation to zero has a side effect on the hue: it
		becomes undefined.  Force it to zero for consistency, rather
		than leaving it at the old value.}
	IF (item = iSaturation) THEN
		WITH PickInfoPtr(pickDlg)^.theColor.hsx DO
		IF (saturation = 0) THEN
			hue := 0;

	ShowNewValues(pickDlg, (item >= iRed), affected);
END;

{--------------------------------------------------------------------------------
 ShowHotSpot - invert the area around the given "hot spot" (HS) on the wheel
}

PROCEDURE ShowHotSpot(where: Point);
VAR
	r:		Rect;
BEGIN
	WITH r DO
		BEGIN
		topLeft := where;
		botRight := where;
		END;

	InsetRect(r, -3, -3);		{ let r surround the hot point }

	{Make the hot spot (in)visible by an oval frame 1 pixel wide.}
	InvertOval(r);
	InsetRect(r,1,1);
	InvertOval(r);
END;

{--------------------------------------------------------------------------------
 ShowNewColor - update the colored dialog items
}

PROCEDURE ShowNewColor(pickDlg: DialogPtr; affected: ColorItemSet);
VAR
	newBright:		BOOLEAN;
BEGIN
	{If any of the items affected by lightness has changed, we have a new
		lightness value; our color table must be adjusted accordingly.}
	newBright := lWheel IN affected;

	{Adjust picker's colors as necessary, then reinstall the color table
		so that they show up.}
	CTabAdjust(pickDlg, newBright);
	CTabInstall(pickDlg, newBright);

	{If user has no color table, we can't "animate" with it; our color
		object must be redrawn to show up in the new colors.}
	IF PickInfoPtr(pickDlg)^.colorOK THEN
		IF PickInfoPtr(pickDlg)^.gdType <> clutType THEN
			BEGIN
			PickDlgDrawItem(WindowPtr(pickDlg), iNewSample);
			IF lWheel IN affected THEN
				BEGIN
				{Erase the hotspot before redrawing the wheel. The wheel draw
					routine will re-show it.}
				ShowHotSpot(PickInfoPtr(pickDlg)^.hotSpot);	{1.4a4}
				PickDlgDrawItem(WindowPtr(pickDlg), iWheel);
				END;
			END;
END;

{--------------------------------------------------------------------------------
 ShowNewValues - update the color component text items and controls
}

PROCEDURE ShowNewValues(pickDlg: DialogPtr; newRGB: BOOLEAN; affected: ColorItemSet);
VAR
	ctlHand:	ControlHandle;
	item:		ColorItem;
	newHotSpot:	Point;
	oldLight:	SmallFract;
BEGIN
	WITH PickInfoPtr(pickDlg)^, theColor DO
		BEGIN
		IF newRGB
			THEN RGB2HSx(rgb, hsx)
			ELSE HSx2RGB(hsx, rgb);

		{Update the colors on the screen first, since the user is likely to
			be more intent on them than the controls and text values.}
		ShowNewColor(pickDlg, affected);

		{Update the component text items and other non-color information
			(like the brightness scroll bar and the wheel hot spot).}
		FOR item := rgbText TO bright DO
			IF item IN affected THEN
				CASE item OF
					rgbText:
						WITH rgb DO
						BEGIN
						StuffIt(pickDlg, iRed, red);
						StuffIt(pickDlg, iGreen, green);
						StuffIt(pickDlg, iBlue, blue);
						END;
					hsText:
						WITH hsx DO
						BEGIN
						StuffIt(pickDlg, iHue, hue);
						StuffIt(pickDlg, iSaturation, saturation);
						END;
					lText:
						StuffIt(pickDlg, iLightness, hsx.lightness);
					hsWheel:
						BEGIN
						{Compute hot spot based on new color.  Update iff
							the location has really changed.}
						HS2WheelPos(pickDlg, newHotSpot);
						IF NOT EqualPt(hotSpot, newHotSpot) THEN
							BEGIN
							{Erase old hot spot by re-inverting it.}
							ShowHotSpot(hotSpot);
							ShowHotSpot(newHotSpot);
							hotSpot := newHotSpot;
							END;
						END;
					bright:
						BEGIN
						{Again, to reduce flicker, check that the lightness
							has really changed before updating it.  We should
							perhaps check that it has changed by an amount
							that will be discernible on screen.}
						ctlHand := ControlHandle(GetIHand(pickDlg, iBrightCtl));
						BrightCtlGet(ctlHand, oldLight);
						IF oldLight <> hsx.lightness THEN
							BrightCtlSet(ctlHand, hsx.lightness);
						END;
				END; {case}
		END;
END;

{--------------------------------------------------------------------------------
 SnareNewComponents - adjust to any quiet (no tab) changes in component values
}

PROCEDURE SnareNewComponents(pickDlg: DialogPtr);
VAR
	c:			INTEGER;
	item:		INTEGER;
	itemHand:	Handle;
	n:			INTEGER;
	newVal:		LONGINT;
	oldVal:		LONGINT;
	s:			Str255;
BEGIN
	{Get the value in the text field the user is currently editing, in case
		s/he changed it without tabbing out of the field.}
	item := DialogPeek(pickDlg)^.editField + 1;
	itemHand := GetIHand(pickDlg, item);
	GetIText(itemHand, s);
	StringToNum(s, newVal);

	{If the new value is too big, pin it at the maximum.  ShowAffected
		will redisplay it, because the newVal and the item text differ.}
	IF newVal > MaxSmallFract THEN
		newVal := MaxSmallFract;

	{See if the selected component has changed; update if so.}
	WITH PickInfoPtr(pickDlg)^.theColor DO
		BEGIN
		c := item - iFirstComponent;
		oldVal := SmallFract2Fix(components[c]);
		IF s[0] = chr(0) THEN
			newVal := oldVal
		ELSE
			FOR n := 1 TO ORD(s[0]) DO
				IF (s[n] < '0') | (s[n] > '9') THEN
					BEGIN
						newVal := oldVal;
						SysBeep(1);
					END;

		IF oldVal <> newVal THEN
			BEGIN
			components[c] := Fix2SmallFract(newVal);
			ShowAffected(pickDlg, item);
			END
		ELSE
			{Regularize values that are numerically the same, but have
				been typed in quaintly (get rid of leading zeroes, make
				'' into '0', etc.).}
			StuffIt(pickDlg, item, newVal);
		END;
END;

{--------------------------------------------------------------------------------
 StuffIt - update text item's text iff necessary
}

PROCEDURE StuffIt(pickDlg: DialogPtr; theItem: INTEGER; newVal: SmallFract);
VAR
	itemHand:	Handle;
	s, sOld:	Str255;
BEGIN
	itemHand := GetIHand(pickDlg, theItem);
	GetIText(itemHand, sOld);
	NumToString(SmallFract2Fix(newVal), s);

	{To reduce flicker, check that the value we're stuffing is different
		than the one already there.  We check the string, not the number,
		so that values like '' and '00' are regularized to '0'.}
	IF s <> sOld THEN
		BEGIN
		SetIText(itemHand, s);
		{If the item being stuffed is the currently selected one,
			make sure that the entire field is selected.}
		IF (DialogPeek(pickDlg)^.editField + 1) = theItem THEN
			SelIText(pickDlg, theItem, 0, 32767);
		END;
END;

{---------------------------------------------------------------------------------
 WheelDraw - draw the hue/saturation wheel
}

PROCEDURE WheelDraw(pickDlg: DialogPtr; wheelRect: Rect);
VAR
	color: PColor;				{Color wheel wedge being drawn}
	nextColor: PColor;			{Next color in wheel, for dithering}
	halfColor: pColor;			{half saturation edition of wedge being drawn}
	nextHalfColor: pColor;		{half saturation of next wedge}
	inset: INTEGER;				{Amount to inset each saturation level}
	savePen: PenState;			{Saved pen state}
	sliceAngle: INTEGER;		{Number of degrees of arc in each wheel wedge}
	startAngle: INTEGER;		{Angle at which to start drawing wheel wedge}
	tint: INTEGER;				{Index of color wheel saturation level}
	HalfColors: INTEGER;		{Offset to the half-bright elements}
	ring2,ring3,ring4,ring5: Rect;
	saveGDev:	GDHandle;			{Old GDevice}
	saveID: INTEGER;			{Old client ID}
	RSpot, GSpot, BSpot: Point;	{For b/w wheel, where to draw letters R, G, and B}
	wheelCenter: Point;
	colorNames: Handle;			{Handle to STR# with color names}
	colorString: StringPtr;


	PROCEDURE WedgeName(angSin,angCos:LONGINT; n:String);
{ angSin and angCos are the sine and cosine times one hundred for where on the perimeter }
{ of the wheel to erase a circle and draw the wedge name, n. }
	VAR
		r:Rect;
		i2,w:INTEGER;
	BEGIN
		WITH PickInfoPtr(pickDlg)^
		DO
		BEGIN
			i2 := inset div 2;
			w := (wheelRadius div 65536 - i2);
			r.top := wheelCenter.v + angSin * w div 100;
			r.left := wheelCenter.h + angCos * w div 100;
			r.botRight := r.topLeft;
			MoveTo(r.left - StringWidth(n) div 2,r.top + 5);
			i2:=2-i2;
			InsetRect(r,i2,i2);
			EraseOval(r);
			DrawString(n);
		END;
	END;


BEGIN

	RGBNormal;

{Draw the wheel outline}
	InsetRect(wheelRect, -1, -1);
	FrameOval(wheelRect);
	InsetRect(wheelRect, 1, 1);
	inset := (wheelRect.right - wheelRect.left) DIV (wheelTints * 2-1 );
	GetPenState (savePen);

	ring2 := wheelRect;
	InsetRect(ring2,inset,inset);
	ring3 := ring2;
	InsetRect(ring3,inset,inset);
	ring4 := ring3;
	InsetRect(ring4,inset,inset);
	ring5 := ring4;
	InsetRect(ring5,inset,inset);

	WITH PickInfoPtr(pickDlg)^ DO
	BEGIN

{$IFC NOT UsePalette}
	IF ColorOK
	THEN
	BEGIN
		saveID := itsGDev^^.gdID;
		saveGDev := GetGDevice;
		SetGDevice(itsGDev);
		SetClientID(pickClientID);
		SetGDevice(saveGDev);
	END;
{$ENDC}

	IF (pixSize <= 2)
	THEN
{ Wheel drawing for 1 and 2 bit mode }
		WITH GetQDGlobals^,wheelRect DO
		BEGIN
			PenSize(inset,inset);
			PenPat(black);
			FrameOval(wheelRect);
			PenPat(dkGray);
			FrameOval(ring2);
			PenPat(gray);
			FrameOval(ring3);
			PenPat(ltGray);
			FrameOval(ring4);

			colorNames := GetResource('STR#',rColorNamesStr);
			IF (LONGINT(colorNames) <> 0)
			THEN
			BEGIN
				HLock(colorNames);
				ColorString := StringPtr(Ord4(colorNames^) + 2);
				WedgeName(0,100,ColorString^);		{'R'}
				ColorString := StringPtr(Ord4(ColorString) + Length(ColorString^) + 1);
				WedgeName(-86,-50,ColorString^);	{'G'}
				ColorString := StringPtr(Ord4(ColorString) + Length(ColorString^) + 1);
				WedgeName(86,-50,ColorString^);		{'B'}
				ColorString := StringPtr(Ord4(ColorString) + Length(ColorString^) + 1);
				WedgeName(0,-100,ColorString^);		{'C'}
				ColorString := StringPtr(Ord4(ColorString) + Length(ColorString^) + 1);
				WedgeName(86,50,ColorString^);		{'M'}
				ColorString := StringPtr(Ord4(ColorString) + Length(ColorString^) + 1);
				WedgeName(-86,50,ColorString^);		{'Y'}
				ReleaseResource(colorNames);
			END;
		END

	ELSE IF gdType <> clutType
	THEN
		MakeWedges32(wheelRect,PickInfoPtr(pickDlg)^.theColor.hsx.lightness) {!!!}
	ELSE IF (pixSize = 8)
	THEN
	BEGIN
{ Wheel drawing for 8 bit mode}

{Paint the entire wheel, over which we'll dither in saturation levels.
		Start at 3 o'clock, which is zero, zero degrees, and red on the HSx
		scale, and run counter-clockwise.  Quickdraw puts zero degrees at
		12 o'clock and runs clockwise.  The extra (sliceAngle/2) insures
		that the first hue (red) will be centered at zero degrees, not
		begin there.}

		sliceAngle := 720 DIV wheelColors;
		HalfColors := wheelColors DIV 2;
		startAngle := 90 + (sliceAngle DIV 2);

		PenSize(inset,inset);

		FOR color := cSlice1 TO pColor(ord(cSlice1)+HalfColors-1)
		DO
		BEGIN
			halfColor := pColor(ord(color)+halfColors);

			PenMode(srcCopy);
			Dither1(color);
			PaintArc(wheelRect, startAngle, -sliceAngle);
			Dither1(halfcolor);
			PaintArc(ring3, startAngle, -sliceAngle);
			PenMode(srcOr);
			PenPat(GetQDGlobals^.gray);
			FrameArc(ring2, startAngle, -sliceAngle);
			startAngle := startAngle - sliceAngle;
		END;


{Set RGB foreground color to current intensity gray}
		Dither1(cGray);
		PenMode(srcOr);
		PenPat(GetQDGlobals^.gray);
		FrameOval(ring4);
		Dither1(cGray);
		PaintOval(ring5);
	END
	ELSE
	BEGIN { it must be four bits!}

{4 bit wheeldraw}
{Paint the entire wheel, over which we'll dither in saturation levels.
		Start at 3 o'clock, which is zero, zero degrees, and red on the HSx
		scale, and run counter-clockwise.  Quickdraw puts zero degrees at
		12 o'clock and runs clockwise.  The extra (sliceAngle/2) insures
		that the first hue (red) will be centered at zero degrees, not
		begin there.}

		sliceAngle := 90 DIV wheelColors;
		startAngle := 90 + (sliceAngle DIV 2);

		PenSize(2*inset,2*inset);

		FOR color := cSlice1 TO lastColor
		DO
		BEGIN
			IF color = lastColor
			THEN
				nextColor := cSlice1
			ELSE
				nextColor := succ(color);

			Dither1(color);
			FrameArc(wheelRect, startAngle, -sliceAngle);
			startAngle := startAngle - sliceAngle;

			Dither50(nextColor,color);
			SetGray25Pat;
			FrameArc(wheelRect, startAngle, -sliceAngle);
			startAngle := startAngle - sliceAngle;

			Dither50(color,nextColor);
			FrameArc(wheelRect, startAngle, -sliceAngle);
			startAngle := startAngle - sliceAngle;

			Dither50(nextColor,color);
			SetGray75Pat;
			FrameArc(wheelRect, startAngle, -sliceAngle);

			startAngle := startAngle +3*sliceAngle + (sliceAngle div 2);

			Dither1(color);
			FrameArc(ring3, startAngle, -2*sliceAngle);
			startAngle := startAngle - (2*sliceAngle);
			Dither50(color,nextColor);
			SetGray50BPat;
			FrameArc(ring3, startAngle, -2*sliceAngle);
			startAngle := startAngle - (2*sliceAngle) - (sliceAngle div 2);

		END;

{Set RGB foreground color to current intensity gray, back to white}
		RGBNormal;
		Dither1(cGray);
		PenMode(srcOr);
		SetGray50Pat;				{ stamp the inner ring with white checkers }
		FrameOval(ring3);
		PenNormal;
		PaintOval(ring5);
	END;

{$IFC NOT UsePalette}
	IF ColorOK
	THEN
	BEGIN
		SetGDevice(itsGDev);
		SetClientID(saveID);
		SetGDevice(saveGDev);
	END;
{$ENDC}

	SetPenState (savePen);

{Draw the hot spot, which we've just erased	by redrawing the wheel over it.}
	ShowHotSpot(PickInfoPtr(pickDlg)^.hotSpot);

	END;	{with pickdlg^}

END;

PROCEDURE TwoComponentCTab(rPart,gPart,bPart, brite:INTEGER; theCTab:CTabHandle);
{ A color table is constructed with 16 levels each of two colors. Which
  two colors are specified by passing numbers 1,2, or 3 into rPart, gPart,
  and cPart. 1: ctTable[i].rgb.x = i div 16,
             2: ctTable[i].rgb.x = i mod 16,
			 3: ctTable[i].rgb.x = 65535.
  all values are scaled by brite, which can range 0 to 65535.
}

VAR
	parts:	ARRAY [1..3] OF LONGINT;
	lBrite:	LONGINT;
	i: INTEGER;

BEGIN
	lBrite := brite;
	lBrite := (BAnd(lBrite, $0000FFFF)+1) div 256;	{ treat as 0 to 256 }

	parts[3] := brite;								{ fixed component }

	SetHandleSize(Handle(theCTab),2056);			{ size for 256 entries }

	WITH theCTab^^
	DO
	BEGIN
		ctSeed := GetCTSeed;
		ctFlags := 0;								{ 1.2 }
		ctSize := 255;
		FOR i:=0 TO 255
		DO
			WITH ctTable[i]
			DO
			BEGIN
				parts[1] := (i div 16) * LONGINT(4369) * lBrite div 256;
				parts[2] := (i mod 16) * LONGINT(4369) * lBrite div 256;
				value := i;

				rgb.red := parts[rPart];
				rgb.green := parts[gPart];
				rgb.blue := parts[bPart];
			END;
	END;
END;

PROCEDURE UnpackRect(srcStart, dstStart: LONGINT; dstStep, height: INTEGER);
{	Use NibbleUnpack to unpack a section of the colorful wedges.
	srcStart is a pointer to packed data, dstStart is the target
	bitmap's 1st address. dstStep is how much to add to the address
	AFTER each unpacked line. height is how many lines to unpack.}
VAR
	i,h:	INTEGER;
	dummy:	INTEGER;

BEGIN
	h := height div 2;
	FOR i:=1 TO h
	DO
	BEGIN
		dummy :=  NibbleUnpack(srcStart,dstStart);
		dstStart := dstStart + LONGINT(dstStep);
		srcStart := srcStart + NibbleUnpack(srcStart,dstStart);
		dstStart := dstStart + LONGINT(dstStep);
	END;
END;



PROCEDURE MakeWedges32(wheelRect:Rect; brite:INTEGER);

{
This diagram shows the points used to construct the clipping regions
for the three wedges drawn, and the names of the points describing it.
The center lines are cx,cy, and the corners and edges are wheelRect.
	        x1
	 -------+----
	|       |    |__y1
	|      /     |
	+-----+      +
	|      \     |__y2
	|       |    |
	 -------+----

Also, this routine is full of hardcoded 200-bits diameter slime. This is
because the data in the Wedg resrouces are 200-bit diameter wedges.
}

TYPE
	bmptr = ^BitMap;

VAR
	wedge1Bits:	PixMap;
	wedge1Handle:Handle;
	wedge2Handle:Handle;
	wedgeClip,
	wheelClip:	RgnHandle;
	srcWalker:	LONGINT;
	dstWalker:	LONGINT;
	i,cx,cy,
	x1,y1,y2:	INTEGER;
	square:		Rect;
	qdVersion:	LONGINT;
	copyMode:	INTEGER;

BEGIN
	RGBNormal;

	copyMode := srcCopy;
	IF Gestalt(gestaltQuickdrawVersion,qdVersion) = noErr
	THEN
		IF qdVersion >= gestalt32BitQD
		THEN
			copyMode := ditherCopy;

	SetRect(square,0,0,200,200);		{ Hard coded: a no-no, I know }

	WITH wedge1Bits
	DO
	BEGIN
		baseAddr := NewPtr(40000);
		rowBytes := 200;
		bounds := square;
		pmVersion := 0;
		packType := 0;
		pixelType := 0;
		pixelSize := 8;
		cmpCount := 1;
		cmpSize := 8;
		pmTable := CTabHandle(NewHandle(2056));

{Build a wheel shaped clipping region}
		wheelClip := NewRgn;
		OpenRgn;
		FrameOval(wheelRect);
		CloseRgn(wheelClip);

		cx := (wheelRect.left + wheelRect.right) div 2;
		cy := (wheelRect.top + wheelRect.bottom) div 2;
		x1 := cx+ (wheelRect.right-cx) div 2-1;
		y1 := cy+ LONGINT(wheelRect.top-cy)*866 div 1000 -4;
		y2 := cy+cy-y1;
		wedgeClip := NewRgn;

{Do the upper red wedge}
		TwoComponentCTab(3,1,2,brite,pmTable);
		wedge1Handle := GetResource('wedg',rRedWedge);

		UnpackRect(LONGINT(wedge1Handle^),LONGINT(baseAddr)+100,
				rowBytes,100);


{Upper red wedge clip region }
		WITH wheelRect
		DO
		BEGIN
			OpenRgn;
			MoveTo(cx,cy);
			LineTo(x1,y1);
			LineTo(x1,top);
			LineTo(right,top);
			LineTo(right,cy);
			LineTo(cx,cy);
			CloseRgn(wedgeClip);
		END;
		SectRgn(wedgeClip,wheelClip,wedgeClip);

		rowBytes := LONGINT(rowBytes)+LONGINT(32768);
		CopyPix(wedge1Bits,GetQDGlobals^.thePort^.portBits,
				square,wheelRect,
				copyMode,wedgeClip);
		rowBytes := LONGINT(rowBytes)-LONGINT(32768);

{Do the green wedge}
		TwoComponentCTab(1,3,2,brite,pmTable);
		wedge2Handle := GetResource('wedg',rGreenWedge);
		UnpackRect(LONGINT(wedge2Handle^),LONGINT(baseAddr),
				rowBytes,100);

{ Green wedge clip region }
		WITH wheelRect
		DO
		BEGIN
			OpenRgn;
			MoveTo(left,cy);
			LineTo(cx,cy);
			LineTo(x1,y1);
			LineTo(x1,top);
			LineTo(left,top);
			LineTo(left,cy);
			CloseRgn(wedgeClip);
		END;
		SectRgn(wedgeClip,wheelClip,wedgeClip);

		rowBytes := LONGINT(rowBytes)+LONGINT(32768);
		CopyPix(wedge1Bits,GetQDGlobals^.thePort^.portBits,
				square,wheelRect,
				copyMode,wedgeClip);
		rowBytes := LONGINT(rowBytes)-LONGINT(32768);

{ Move the source bitmap to the lower half of the wheel }
		square.top := square.top - 0; {!!!}

{ Do the blue wedge }
		TwoComponentCTab(1,2,3,brite,pmTable);
		wedge2Handle := GetResource('wedg',rGreenWedge);
		UnpackRect(LONGINT(wedge2Handle^),
				LONGINT(baseAddr)+LONGINT(rowBytes)*199,
				-rowBytes,100);

{ Blue wedge clip region }
		WITH wheelRect
		DO
		BEGIN
			OpenRgn;
			MoveTo(left,cy);
			LineTo(cx,cy);
			LineTo(x1,y2);
			LineTo(x1,bottom);
			LineTo(left,bottom);
			LineTo(left,cy);
			CloseRgn(wedgeClip);
		END;
		SectRgn(wedgeClip,wheelClip,wedgeClip);

		rowBytes := LONGINT(rowBytes)+LONGINT(32768);
		CopyPix(wedge1Bits,GetQDGlobals^.thePort^.portBits,
				square,wheelRect,copyMode,wedgeClip);
		rowBytes := LONGINT(rowBytes)-LONGINT(32768);

{Do the Lower red wedge}
		TwoComponentCTab(3,2,1,brite,pmTable);
		wedge1Handle := GetResource('wedg',rRedWedge);

		UnpackRect(LONGINT(wedge1Handle^),LONGINT(baseAddr)+LONGINT(rowBytes)*199+100,
				-rowBytes,100);

{Lower red wedge clip region }
		WITH wheelRect
		DO
		BEGIN
			OpenRgn;
			MoveTo(cx,cy);
			LineTo(x1,y2);
			LineTo(x1,bottom);
			LineTo(right,bottom);
			LineTo(right,cy);
			LineTo(cx,cy);
			CloseRgn(wedgeClip);
		END;
		SectRgn(wedgeClip,wheelClip,wedgeClip);

		rowBytes := LONGINT(rowBytes)+LONGINT(32768);
		CopyPix(wedge1Bits,GetQDGlobals^.thePort^.portBits,
				square,wheelRect,
				copyMode,wedgeClip);
		rowBytes := LONGINT(rowBytes)-LONGINT(32768);

{Clean up}
		DisposeRgn(wedgeClip);
		DisposeRgn(wheelClip);
		ReleaseResource(wedge1Handle);
		ReleaseResource(wedge2Handle);
		DisposHandle(Handle(pmTable));
		DisposPtr(baseAddr);

	END; {with wedge1bits}
END;


{--------------------------------------------------------------------------------
 WheelPos2HS - convert mouse position within wheel to new H and S values
 HS2WheelPos - convert H and S values to mouse position within wheel
}

PROCEDURE WheelPos2HS(pickDlg: DialogPtr; where: Point);
VAR
	x,y:		LONGINT;
	r:			Fixed;
	radians:	Fixed;
BEGIN
	{Get the difference between center and where into normal cartesian
		coordinates (if where is down and to the right of center, x is
		positive and y is negative, etc.}
	WITH PickInfoPtr(pickDlg)^ DO
		BEGIN
		WITH where DO
			BEGIN
			x := h - wheelCenter.h;
			y := wheelCenter.v - v;
			END;

		WITH theColor.hsx DO
			BEGIN
			{Calculate the distance from where to the wheel center, and pin
				that value at wheelRadius (in case they're in the rim of the
				slop region that's actually outside the wheel).}
			r := X2Fix(sqrt(x*x + y*y));
			IF r >= wheelRadius
				THEN saturation := Fix2SmallFract(MaxSmallFract)
				ELSE saturation := Fix2SmallFract(FixDiv(r, wheelRadius));

			radians := FixATan2(x, y);
			hue := Fix2SmallFract(FixDiv(radians, twoPi));
			END;
		END;
END;

PROCEDURE HS2WheelPos(pickDlg: DialogPtr; VAR where: Point);
VAR
	x,y:		INTEGER;
	r:			Fixed;
	radians:	Fixed;
BEGIN
	{Given an angle (hue * 2Pi) and radius (saturation), find x and y in
		normal cartesian coordinates.  Then change to quickdraw coordinates
		while moving (x,y) from the center of the wheel.}
	WITH PickInfoPtr(pickDlg)^ DO
		BEGIN
		WITH theColor.hsx DO
			BEGIN
			radians := FixMul(SmallFract2Fix(hue), twoPi);
			r := FixMul(SmallFract2Fix(saturation), wheelRadius);
			x := HiWrd(FixMul(Frac2Fix(FracCos(radians)), r));
			y := HiWrd(FixMul(Frac2Fix(FracSin(radians)), r));
			END;
		WITH where DO
			BEGIN
			h := wheelCenter.h + x;
			v := wheelCenter.v - y;
			END;
		END;
END;

{--------------------------------------------------------------------------------
 WheelTintPattern - get b/w pattern to dither manually for a given saturation.
 When filling with a black and white pattern, quickdraw uses the foreground
 color for bits that are set, and the background pattern for those that are
 not. So the amount of color in the combination we're dithering corresponds
 to the amount of black (we get pure color for tint level one).  The higher
 the tint level, the closer we are to white and the lower the saturation.
}
PROCEDURE WheelTintPattern(whichTint: INTEGER; VAR pat: Pattern);
BEGIN
	WITH GetQDGlobals^ DO
		CASE whichTint OF
			5:	pat := black;
			4:	pat := DkGray;
			3:	pat := Gray;
			2:	pat := LtGray;
			1:	pat := white;
			END;
END;

{--------------------------------------------------------------------------------
 WheelTrack - track mouse-down inside the color wheel
}

FUNCTION WheelTrack(theDialog: DialogPtr; where: Point): BOOLEAN;
VAR
	newPt:		Point;
	reverted:	BOOLEAN;
	saveHSx:	HSLColor;
BEGIN
	WheelTrack := FALSE;
	WITH PickInfoPtr(theDialog)^ DO
		IF PtInRgn(where, wheelRgn) THEN
			BEGIN
			{Save the current HSx color, in case the user drags out of the
				wheel and mouses up there.  Fudge where so that it and newPt
				aren't equal the first time through the loop, so that the
				color at mouseDown gets set.}
			saveHSx := theColor.hsx;
			SetPt(where, 0, 0);
			reverted := FALSE;

			{Use repeat so that this always executes at least once.  Otherwise,
				if the use clicks quickly (and we're slow to get here because of
				checking for component changes…), the new position is never shown,
				because WaitMouseUp is already TRUE.}
			REPEAT
				GetMouse(newPt);
				IF NOT EqualPt(where, newPt) THEN
					BEGIN
					where := newPt;
					IF PtInRgn(newPt, slopRgn) THEN
						BEGIN
						WheelPos2HS(theDialog, newPt);
						{Don't update hsWheel; hot spot fixed at end of track.}
						ShowNewValues(theDialog, FALSE, [rgbText, hsText]);
						reverted := FALSE;
						END
					ELSE IF NOT reverted THEN
						BEGIN
						theColor.hsx := saveHSx;
						ShowNewValues(theDialog, FALSE, [rgbText, hsText]);
						reverted := TRUE;
						END;
					END;
			UNTIL NOT WaitMouseUp;

			{Now that we're done tracking, move the hot spot.}
			IF NOT reverted THEN
				ShowNewValues(theDialog, FALSE, [hsWheel]);

			{Regardless of where the user moused up, we've handled the mouse
				down; let the filter proc know so ModalDialog will ignore it.}
			WheelTrack := TRUE;
			END;
END;

{================================================================================
 GetColor - present the color picker dialog; let the user choose a color
}

FUNCTION GetColor(where: Point; prompt: Str255; inColor: RGBColor; VAR outColor: RGBColor): BOOLEAN;
VAR
	itemHit:	INTEGER;
	pickData:	PickInfo;		{Our dialog, with color values tacked on}
	pickDlg:	DialogPtr;		{Pointer to our dialog}
	savePort:	GrafPtr;		{Caller's port}
	tabHand:	CTabHandle;
	saveGDev:	GDHandle;
	memNeeded:	LongInt;		{How much free (or purgeable) memory we need}
	tempHandle:	Handle;
BEGIN
	GetColor := FALSE;		{Prepare for possible memory failure}

	GetPort (savePort);

	{Set up pickData.}
	WITH pickData, theColor
	DO
	BEGIN
		rgb := inColor;
		RGB2HSx(inColor, hsx);
		rgbInput := inColor;
		showModel := TRUE;
		showAltModel := TRUE;
	END;

	{Build picker's color table and save the current one, then swap in
		picker's colors.  Note that CTabBuild expects both versions of
		theColor to be set.}

	{Get the dialog, and set up all of its items.}
	pickDlg := PickDlgOpen(@pickData, where, prompt);

	CTabBuild(pickData, inColor);
{$IFC UsePalette}
	CTabSave(@pickData);
{$ENDC}
	CTabInstall(DialogPtr(@pickData), TRUE);

	{1.4a4	We've done the initial setup and know about our environment. It's
			time to see if we have enough memory to continue. The memory requirement
			values were determined empirically, so I've purposely made them a little
			bit conservative.}

	IF pickData.pixSize >= 16
	THEN
		memNeeded := kDirectMemReq
	ELSE
		memNeeded := kClutMemReq;
	tempHandle := NewHandle(memNeeded);

	IF tempHandle <> NIL THEN
		BEGIN
		DisposHandle(tempHandle);

		ShowNewValues(pickDlg, TRUE, [rgbText..bright]);	{ first values }

		{Now that we're capable of drawing the dialog, and all the controls
			are set up, make the dialog visible; the resource is invisible.}
		ShowWindow(WindowPtr(@pickData));

{$IFC NOT UsePalette}
		IF pickData.colorOK THEN
		BEGIN
			saveGDev := GetGDevice;
			SetGDevice(pickData.itsGDev);
			AddSearch(@CTabSearch);
			SetGDevice(saveGDev);
		END;
{$ENDC}

		REPEAT
			ModalDialog(@PickDlgFilter, itemHit);
			IF itemHit = iRemodel THEN
				WITH pickData DO
					RemodelDlg(pickDlg^.portRect, showModel, showAltModel);
		UNTIL (itemHit = ok) OR (itemHit = cancel);

		END
	ELSE
		BEGIN
		itemHit := StopAlert(rMemShortageID, NIL);
		itemHit := cancel;
		END;

	{Close up shop: close the dialog, restore the color table, get rid
		of the search proc we installed, and free any memory we allocated.}
	PickDlgClose(@pickData);

	IF (itemHit = ok) THEN
		BEGIN
		outColor := pickData.theColor.rgb;
		GetColor := TRUE;
		END
	ELSE
		GetColor := FALSE;

	SetPort (savePort)
END;

END.
