{------------------------------------------------------------------------------
#
#	Apple Macintosh Developer Technical Support
#
#	MultiFinder-Aware Simple TextEdit Sample Application
#
#	TESample
#
#	TESample.p	-	Pascal Source
#
#	Copyright © Apple Computer, Inc. 1989-1990
#	All rights reserved.
#
#	Versions:	
#				1.00				08/88
#				1.01				11/88
#				1.02				04/89	MPW 3.1
#				1.03				02/90	MPW 3.2
#
#	Components:
#				TESample.p			Feb.  1, 1990
#				TESampleGlue.a		Feb.  1, 1990
#				TESample.r			Feb.  1, 1990
#				TESample.h			Feb.  1, 1990
#				TESample.make		Feb.  1, 1990
#
#	TESample is an example application that demonstrates how 
#	to initialize the commonly used toolbox managers, operate 
#	successfully under MultiFinder, handle desk accessories and 
#	create, grow, and zoom windows. The fundamental TextEdit 
#	toolbox calls and TextEdit autoscroll are demonstrated. It 
#	also shows how to create and maintain scrollbar controls.
#
#	It does not by any means demonstrate all the techniques you 
#	need for a large application. In particular, Sample does not 
#	cover exception handling, multiple windows/documents, 
#	sophisticated memory management, printing, or undo. All of 
#	these are vital parts of a normal full-sized application.
#
#	This application is an example of the form of a Macintosh 
#	application; it is NOT a template. It is NOT intended to be 
#	used as a foundation for the next world-class, best-selling, 
#	600K application. A stick figure drawing of the human body may 
#	be a good example of the form for a painting, but that does not 
#	mean it should be used as the basis for the next Mona Lisa.
#
#	We recommend that you review this program or Sample before 
#	beginning a new application. Sample is a simple app. which doesn’t 
#	use TextEdit or the Control Manager.
#
------------------------------------------------------------------------------}


PROGRAM TESample;


{Segmentation strategy:

 This program consists of three segments. Main contains most of the code,
 including the MPW libraries, and the main program. Initialize contains
 code that is only used once, during startup, and can be unloaded after the
 program starts. %A5Init is automatically created by the Linker to initialize
 globals for the MPW libraries and is unloaded right away.}


{SetPort strategy:

 Toolbox routines do not change the current port. In spite of this, in this
 program we use a strategy of calling SetPort whenever we want to draw or
 make calls which depend on the current port. This makes us less vulnerable
 to bugs in other software which might alter the current port (such as the
 bug (feature?) in many desk accessories which change the port on OpenDeskAcc).
 Hopefully, this also makes the routines from this program more self-contained,
 since they don't depend on the current port setting.}


{Clipboard strategy:

 This program does not maintain a private scrap. Whenever a cut, copy, or paste
 occurs, we import/export from the public scrap to TextEdit's scrap right away,
 using the TEToScrap and TEFromScrap routines. If we did use a private scrap,
 the import/export would be in the activate/deactivate event and suspend/resume
 event routines.}

{$D+}

USES
	MemTypes, QuickDraw, OSIntf, ToolIntf, PackIntf, Traps;


CONST
	{MPW 3.0 will include a Traps.p interface file that includes constants for trap numbers.
	 These constants are from that file.}
	{1.02 - Uses Traps.p to obtain trap numbers.}

	{1.01 - changed constants to begin with 'k' for consistency, except for resource IDs}
	{kTextMargin is the number of pixels we leave blank at the edge of the window.}
	kTextMargin				= 2;

	{kMaxOpenDocuments is used to determine whether a new document can be opened
	 or created. We keep track of the number of open documents, and disable the
	 menu items that create a new document when the maximum is reached. If the
	 number of documents falls below the maximum, the items are enabled again.}
	kMaxOpenDocuments		= 1;
	
	{kMaxDocWidth is an arbitrary number used to specify the width of the TERec's
	destination rectangle so that word wrap and horizontal scrolling can be
	demonstrated.}
	kMaxDocWidth			= 576;
	
	{kMinDocDim is used to limit the minimum dimension of a window when GrowWindow
	is called.}
	kMinDocDim				= 64;
	
	{kControlInvisible is used to 'turn off' controls (i.e., cause the control not
	to be redrawn as a result of some Control Manager call such as SetCtlValue)
	by being put into the contrlVis field of the record. kControlVisible is used
	the same way to 'turn on' the control.}
	kControlInvisible		= 0;
	kControlVisible			= $FF;
	
	{kScrollBarAdjust and kScrollBarWidth are used in calculating
	values for control positioning and sizing.}
	kScrollbarWidth			= 16;
	kScrollbarAdjust		= kScrollbarWidth - 1;
	
	{kScrollTweek compensates for off-by-one requirements of the scrollbars
 	to have borders coincide with the growbox.}
	kScrollTweek			= 2;
	
	{kCrChar is used to match with a carriage return when calculating the
	number of lines in the TextEdit record. kDelChar is used to check for
	delete in keyDowns.}
	kCRChar					= 13;
	kDelChar				= 8;
	
	{kButtonScroll is how many pixels to scroll horizontally when the button part
	of the horizontal scrollbar is pressed.}
	kButtonScroll			= 4;
	
	{kMaxTELength is an arbitrary number used to limit the length of text in the TERec
	so that various errors won't occur from too many characters in the text.}
	kMaxTELength			= 32000;

	{kSysEnvironsVersion is passed to SysEnvirons to tell it which version of the
	 SysEnvRec we understand.}
	kSysEnvironsVersion		= 1;

	{kOSEvent is the event number of the suspend/resume and mouse-moved events sent
	 by MultiFinder. Once we determine that an event is an osEvent, we look at the
	 high byte of the message sent to determine which kind it is. To differentiate
	 suspend and resume events we check the resumeMask bit.}
	kOSEvent				= app4Evt;	{event used by MultiFinder}
	kSuspendResumeMessage	= 1;		{high byte of suspend/resume event message}
	kResumeMask				= 1;		{bit of message field for resume vs. suspend}
	kMouseMovedMessage		= $FA;		{high byte of mouse-moved event message}
	kNoEvents				= 0;		{no events mask}

	{1.01 - kMinHeap - This is the minimum result from the following
	 equation:
			
			ORD(GetApplLimit) - ORD(ApplicZone)
			
	 for the application to run. It will insure that enough memory will
	 be around for reasonable-sized scraps, FKEYs, etc. to exist with the
	 application, and still give the application some 'breathing room'.
	 To derive this number, we ran under a MultiFinder partition that was
	 our requested minimum size, as given in the 'SIZE' resource.}
	 
	kMinHeap	= 29 * 1024;
	
	{1.01 - kMinSpace - This is the minimum result from PurgeSpace, when called
	 at initialization time, for the application to run. This number acts
	 as a double-check to insure that there really is enough memory for the
	 application to run, including what has been taken up already by
	 pre-loaded resources, the scrap, code, and other sundry memory blocks.}
	 
	kMinSpace	= 20 * 1024;
	
	{kExtremeNeg and kExtremePos are used to set up wide open rectangles and regions.}
	kExtremeNeg		= -32768;
	kExtremePos		= 32767 - 1;	{required for old region bug}
	
	{kTESlop provides some extra security when pre-flighting edit commands.}
	kTESlop			= 1024;

	{kErrStrings is the resource ID for the error strings STR# resource.}
	kErrStrings		= 128;

	{The following are indicies into STR# resources.}
	eWrongMachine	= 1;
	eSmallSize		= 2;
	eNoMemory		= 3;
	eNoSpaceCut		= 4;
	eNoCut			= 5;
	eNoCopy			= 6;
	eExceedPaste	= 7;
	eNoSpacePaste	= 8;
	eNoWindow		= 9;
	eExceedChar		= 10;
	eNoPaste		= 11;
	
	{The following constants are all resource IDs, corresponding to resources in Sample.r.}
	rMenuBar	= 128;					{application's menu bar}
	rAboutAlert	= 128;					{about alert}
	rUserAlert	= 129;					{user error alert}
	rDocWindow	= 128;					{application's window}
	rVScroll	= 128;					{vertical scrollbar control}
	rHScroll	= 129;					{horizontal scrollbar control}

	{The following constants are used to identify menus and their items. The menu IDs
	 have an "m" prefix and the item numbers within each menu have an "i" prefix.}
	mApple		= 128;					{Apple menu}
	iAbout		= 1;

	mFile		= 129;					{File menu}
	iNew		= 1;
	iClose		= 4;
	iQuit		= 12;

	mEdit		= 130;					{Edit menu}
	iUndo		= 1;
	iCut		= 3;
	iCopy		= 4;
	iPaste		= 5;
	iClear		= 6;

	{1.01 - kDITop and kDILeft are used to locate the Disk Initialization dialogs.}
	kDITop		= $0050;
	kDILeft		= $0070;


TYPE
	{A DocumentRecord contains the WindowRecord for one of our document windows,
	 as well as the TEHandle for the text we are editing. We have added fields to
	 hold the ControlHandles to the vertical and horizontal scrollbars and to hold
	 the address of the default clikLoop that gets attached to a TERec when you call
	 TEAutoView. Other document fields can be added to this record as needed. For
	 a similar example, see how the Window Manager and Dialog Manager add fields
	 after the GrafPort.}
	DocumentRecord	= RECORD
		docWindow	: WindowRecord;
		docTE		: TEHandle;
		docVScroll	: ControlHandle;
		docHScroll	: ControlHandle;
		docClik		: ProcPtr;
	END;
	DocumentPeek	= ^DocumentRecord;


VAR
	{The "g" prefix is used to emphasize that a variable is global.}

	{GMac is used to hold the result of a SysEnvirons call. This makes
	 it convenient for any routine to check the environment. It is
	 global information, anyway.}
	gMac				: SysEnvRec;	{set up by Initialize}

	{GHasWaitNextEvent is set at startup, and tells whether the WaitNextEvent
	 trap is available. If it is false, we know that we must call GetNextEvent.}
	gHasWaitNextEvent	: BOOLEAN;		{set up by Initialize}

	{GInBackground is maintained by our OSEvent handling routines. Any part of
	 the program can check it to find out if it is currently in the background.}
	gInBackground		: BOOLEAN;		{maintained by Initialize and DoEvent}

	{GNumDocuments is used to keep track of how many open documents there are
	 at any time. It is maintained by the routines that open and close documents.}
	gNumDocuments		: INTEGER;		{maintained by Initialize, DoNew, and DoCloseWindow}

	
{$S Initialize}
FUNCTION TrapAvailable(tNumber: INTEGER; tType: TrapType): BOOLEAN;

{Check to see if a given trap is implemented. This is only used by the
 Initialize routine in this program, so we put it in the Initialize segment.
 The recommended approach to see if a trap is implemented is to see if
 the address of the trap routine is the same as the address of the
 Unimplemented trap.}
{1.02 - Needs to be called after call to SysEnvirons so that it can check
 if a ToolTrap is out of range of a pre-MacII ROM.}

BEGIN
	IF (tType = ToolTrap) &
		(gMac.machineType > envMachUnknown) &
		(gMac.machineType < envMacII) THEN BEGIN		{it's a 512KE, Plus, or SE}
		tNumber := BAND(tNumber, $03FF);
		IF tNumber > $01FF THEN							{which means the tool traps}
			tNumber := _Unimplemented;					{only go to $01FF}
	END;
	TrapAvailable := NGetTrapAddress(tNumber, tType) <>
						GetTrapAddress(_Unimplemented);
END; {TrapAvailable}


{$S Main}
FUNCTION IsDAWindow(window: WindowPtr): BOOLEAN;

{Check if a window belongs to a desk accessory.}

BEGIN
	IF window = NIL THEN
		IsDAWindow := FALSE
	ELSE	{DA windows have negative windowKinds}
		IsDAWindow := WindowPeek(window)^.windowKind < 0;
END; {IsDAWindow}


{$S Main}
FUNCTION IsAppWindow(window: WindowPtr): BOOLEAN;

{Check to see if a window belongs to the application. If the window pointer
 passed was NIL, then it could not be an application window. WindowKinds
 that are negative belong to the system and windowKinds less than userKind
 are reserved by Apple except for windowKinds equal to dialogKind, which
 mean it is a dialog.
 1.02 - In order to reduce the chance of accidentally treating some window
 as an AppWindow that shouldn't be, we'll only return true if the windowkind
 is userKind. If you add different kinds of windows to Sample you'll need
 to change how this all works.}

BEGIN
	IF window = NIL THEN
		IsAppWindow := FALSE
	ELSE	{application windows have windowKinds = userKind (8)}
		WITH WindowPeek(window)^ DO
			IsAppWindow := (windowKind = userKind);
END; {IsAppWindow}


{$S Main}
PROCEDURE AlertUser(error: INTEGER);

{Display an alert that tells the user an error occurred, then exit the program.
 This routine is used as an ultimate bail-out for serious errors that prohibit
 the continuation of the application. Errors that do not require the termination
 of the application should be handled in a different manner. Error checking and
 reporting has a place even in the simplest application. The error number is used
 to index an 'STR#' resource so that a relevant message can be displayed.}
 
VAR
	itemHit	: INTEGER;
	message	: Str255;
BEGIN
	SetCursor(arrow);
	GetIndString(message, kErrStrings, error);
	ParamText(message, '', '', '');
	itemHit := Alert(rUserAlert, NIL);
END; {AlertUser}


{$S Main}
PROCEDURE GetTERect(window: WindowPtr; VAR teRect: Rect);

{return a rectangle that is inset from the portRect by the size of
the scrollbars and a little extra margin.}

BEGIN
	teRect := window^.portRect;
	InsetRect(teRect, kTextMargin, kTextMargin);			{adjust for margin}
	teRect.bottom := teRect.bottom - kScrollbarAdjust;	{and for the scrollbars}
	teRect.right := teRect.right - kScrollbarAdjust;
END; {GetTERect}


{$S Main}
FUNCTION DoCloseWindow(window: WindowPtr) : BOOLEAN;

{Close a window. This handles desk accessory and application windows.}

{1.01 - At this point, if there was a document associated with a
 window, you could do any document saving processing if it is 'dirty'.
 DoCloseWindow would return TRUE if the window actually closed, i.e.,
 the user didn’t cancel from a save dialog. This result is handy when
 the user quits an application, but then cancels the save of a document
 associated with a window.}

BEGIN
	DoCloseWindow := TRUE;
	IF IsDAWindow(window) THEN
		CloseDeskAcc(WindowPeek(window)^.windowKind)
	ELSE IF IsAppWindow(window) THEN BEGIN
		WITH DocumentPeek(window)^ DO
			IF docTE <> NIL THEN
				TEDispose(docTE);		{dispose the TEHandle}
		{1.01 - We used to call DisposeWindow, but that was technically
		 incorrect, even though we allocated storage for the window on
		 the heap. We should instead call CloseWindow to have the structures
		 taken care of and then dispose of the storage ourselves.}
		CloseWindow(window);
		DisposPtr(Ptr(window));
		gNumDocuments := gNumDocuments - 1;
	END;
END; {DoCloseWindow}


{$S Main}
PROCEDURE AdjustViewRect(docTE: TEHandle);

{Update the TERec's view rect so that it is the greatest multiple of
the lineHeight and still fits in the old viewRect.}

BEGIN
	WITH docTE^^ DO BEGIN
		viewRect.bottom := (((viewRect.bottom - viewRect.top) DIV lineHeight) *
							lineHeight) + viewRect.top;
	END;
END; {AdjustViewRect}


{$S Main}
PROCEDURE AdjustTE(window: WindowPtr);

{Scroll the TERec around to match up to the potentially updated scrollbar
values. This is really useful when the window resizes such that the
scrollbars become inactive and the TERec had been previously scrolled.}

VAR
	value	: INTEGER;
BEGIN
	WITH DocumentPeek(window)^ DO BEGIN
		TEScroll((docTE^^.viewRect.left - docTE^^.destRect.left) - GetCtlValue(docHScroll),
				(docTE^^.viewRect.top - docTE^^.destRect.top) -
					(GetCtlValue(docVScroll) * docTE^^.lineHeight),
				docTE);
	END;
END; {AdjustTE}


{$S Main}
PROCEDURE AdjustHV(isVert: BOOLEAN; control: ControlHandle; docTE: TEHandle; canRedraw: BOOLEAN);

{Calculate the new control maximum value and current value, whether it is the horizontal or
vertical scrollbar. The vertical max is calculated by comparing the number of lines to the
vertical size of the viewRect. The horizontal max is calculated by comparing the maximum document
width to the width of the viewRect. The current values are set by comparing the offset between
the view and destination rects. If necessary and we canRedraw, have the control be re-drawn by
calling ShowControl.}

VAR
	value, lines, max	: INTEGER;
	oldValue, oldMax	: INTEGER;
BEGIN
	oldValue := GetCtlValue(control);
	oldMax := GetCtlMax(control);
	IF isVert THEN BEGIN
		lines := docTE^^.nLines;
		{since nLines isn’t right if the last character is a return, check for that case}
		IF Ptr(ORD(docTE^^.hText^) + docTE^^.teLength - 1)^ = kCRChar THEN
			lines := lines + 1;
		max := lines - ((docTE^^.viewRect.bottom - docTE^^.viewRect.top) DIV docTE^^.lineHeight);
	END ELSE
		max := kMaxDocWidth - (docTE^^.viewRect.right - docTE^^.viewRect.left);
	IF max < 0 THEN max := 0;			{check for negative values}
	SetCtlMax(control, max);
	IF isVert THEN
		value := (docTE^^.viewRect.top - docTE^^.destRect.top) DIV docTE^^.lineHeight
	ELSE
		value := docTE^^.viewRect.left - docTE^^.destRect.left;
	IF value < 0 THEN
		value := 0
	ELSE IF value > max THEN
		value := max;					{pin the value to within range}
	SetCtlValue(control, value);
	IF canRedraw & ( (max <> oldMax) | (value <> oldValue) ) THEN
		ShowControl(control);			{check to see if the control can be re-drawn}
END; {AdjustHV}


{$S Main}
PROCEDURE AdjustScrollValues(window: WindowPtr; canRedraw: BOOLEAN);

{Simply call the common adjust routine for the vertical and horizontal scrollbars.}

BEGIN
	WITH DocumentPeek(window)^ DO BEGIN
		AdjustHV(TRUE, docVScroll, docTE, canRedraw);
		AdjustHV(FALSE, docHScroll, docTE, canRedraw);
	END;
END; {AdjustScrollValues}


{$S Main}
PROCEDURE AdjustScrollSizes(window: WindowPtr);

{Re-calculate the position and size of the viewRect and the scrollbars.
 kScrollTweek compensates for off-by-one requirements of the scrollbars
 to have borders coincide with the growbox.}

VAR
	teRect	: Rect;
BEGIN
	GetTERect(window, teRect);										{start with teRect}
	WITH DocumentPeek(window)^, window^.portRect DO BEGIN
		docTE^^.viewRect := teRect;
		AdjustViewRect(docTE);										{snap to nearest line}
		MoveControl(docVScroll, right - kScrollbarAdjust, -1);
		SizeControl(docVScroll, kScrollbarWidth, (bottom - top) -
						(kScrollbarAdjust - kScrollTweek));
		MoveControl(docHScroll, -1, bottom - kScrollbarAdjust);
		SizeControl(docHScroll, (right - left) - (kScrollbarAdjust -
						kScrollTweek), kScrollbarWidth);
	END;
END; {AdjustScrollSizes}


{$S Main}
PROCEDURE AdjustScrollbars(window: WindowPtr; needsResize: BOOLEAN);

{Turn off the controls by jamming a zero into their contrlVis fields (HideControl erases them
and we don't want that). If the controls are to be resized as well, call the procedure to do that,
then call the procedure to adjust the maximum and current values. Finally re-enable the controls
by jamming a $FF in their contrlVis fields.}

VAR
	oldMax, oldVal	: INTEGER;
BEGIN
	WITH DocumentPeek(window)^ DO BEGIN
		{First, turn visibility of scrollbars off so we won’t get unwanted redrawing}
		docVScroll^^.contrlVis := kControlInvisible;	{turn them off}
		docHScroll^^.contrlVis := kControlInvisible;
		IF needsResize THEN								{move and size if needed}
			AdjustScrollSizes(window);
		AdjustScrollValues(window, NOT needsResize);	{fool with max and current value}
		{Now, restore visibility in case we never had to ShowControl during adjustment}
		docVScroll^^.contrlVis := kControlVisible;		{turn them on}
		docHScroll^^.contrlVis := kControlVisible;
	END;
END; {AdjustScrollbars}


{$S Main}
{$PUSH} {$Z+}
PROCEDURE PascalClikLoop;

{Gets called from our assembly language routine, AsmClikLoop, which is in
 turn called by the TEClick toolbox routine. Saves the windows clip region,
 sets it to the portRect, adjusts the scrollbar values to match the TE scroll
 amount, then restores the clip region.}

VAR
	window	: WindowPtr;
	region	: RgnHandle;
BEGIN
	window := FrontWindow;
	region := NewRgn;
	GetClip(region);					{save the old clip}
	ClipRect(window^.portRect);			{set the new clip}
	AdjustScrollValues(window, TRUE);	{pass TRUE for canRedraw}
	SetClip(region);					{restore the old clip}
	DisposeRgn(region);
END; {PascalClikLoop}
{$POP}


{$S Main}
{$PUSH} {$Z+}
FUNCTION GetOldClikLoop : ProcPtr;

{Gets called from our assembly language routine, AsmClikLoop, which is in
turn called by the TEClick toolbox routine. It returns the address of the
default clikLoop routine that was put into the TERec by TEAutoView to
AsmClikLoop so that it can call it.}

BEGIN
	GetOldClikLoop := DocumentPeek(FrontWindow)^.docClik;
END; {GetOldClikLoop}
{$POP}


PROCEDURE AsmClikLoop; EXTERNAL;

{A reference to our assembly language routine that gets attached to the clikLoop
field of our TE record.}


{$S Main}
PROCEDURE DoNew;

{Create a new document and window.}

VAR
	good, ignore		: BOOLEAN;
	storage				: Ptr;
	window				: WindowPtr;
	destRect, viewRect	: Rect;

BEGIN
	storage := NewPtr(SIZEOF(DocumentRecord));
	IF storage <> NIL THEN BEGIN
		window := GetNewWindow(rDocWindow, storage, WindowPtr(-1));
		IF window <> NIL THEN BEGIN
			gNumDocuments := gNumDocuments + 1;	{this will be decremented when we call DoCloseWindow}
			good := FALSE;
			SetPort(window);
			WITH window^, DocumentPeek(window)^ DO BEGIN
				GetTERect(window, viewRect);
				destRect := viewRect;
				destRect.right := destRect.left + kMaxDocWidth;
				docTE := TENew(destRect, viewRect);
				IF docTE <> NIL THEN BEGIN		{1.02 - moved}
					good := TRUE;				{if TENew succeeded, we have a good document}
					AdjustViewRect(docTE);
					TEAutoView(TRUE, docTE);
					docClik := docTE^^.clikLoop;
					docTE^^.clikLoop := @AsmClikLoop;
				END;
				IF good THEN BEGIN
					docVScroll := GetNewControl(rVScroll, window);
					good := (docVScroll <> NIL);
				END;
				IF good THEN BEGIN
					docHScroll := GetNewControl(rHScroll, window);
					good := (docHScroll <> NIL);
				END;
				IF good THEN BEGIN
					{FALSE to AdjustScrollValues means musn’t redraw; technically, of course,
					the window is hidden so it wouldn’t matter whether we called ShowControl or not.}
					AdjustScrollValues(window, FALSE);
					ShowWindow(window);			{if the document is good, make the window visible}
				END ELSE BEGIN
					ignore := DoCloseWindow(window);	{otherwise regret we ever created it...}
					AlertUser(eNoWindow);				{and tell user}
				END
			END;
		END ELSE
			DisposPtr(storage);					{get rid of the storage if it is never used}
	END;
END; {DoNew}


{$S Main}
PROCEDURE BigBadError(error: INTEGER);
BEGIN
	AlertUser(error);
	ExitToShell;
END;


{$S Initialize}
PROCEDURE Initialize;

{Set up the whole world, including global variables, Toolbox managers,
 menus, and a single blank document.}

{1.01 - The code that used to be part of ForceEnvirons has been moved into
 this module. If an error is detected, instead of merely doing an ExitToShell,
 which leaves the user without much to go on, we call AlertUser, which puts
 up a simple alert that just says an error occurred and then calls ExitToShell.
 Since there is no other cleanup needed at this point if an error is detected,
 this form of error- handling is acceptable. If more sophisticated error recovery
 is needed, an exception mechanism, such as is provided by Signals, can be used.}

VAR
	menuBar				: Handle;
	total, contig		: LongInt;
	ignoreResult		: BOOLEAN;
	event				: EventRecord;
	count, ignoreError	: INTEGER;
	
	PROCEDURE BigBadError(error: INTEGER);
	BEGIN
		AlertUser(error);
		ExitToShell;
	END;

BEGIN
	gInBackground := FALSE;

	InitGraf(@thePort);
	InitFonts;
	InitWindows;
	InitMenus;
	TEInit;
	InitDialogs(NIL);
	InitCursor;

	{Call MPPOpen and ATPLoad at this point to initialize AppleTalk,
	 if you are using it.}
	{NOTE -- It is no longer necessary, and actually unhealthy, to check
	 PortBUse and SPConfig before opening AppleTalk. The drivers are capable
	 of checking for port availability themselves.}
	
	{This next bit of code is necessary to allow the default button of our
	 alert be outlined.
	 1.02 - Changed to call EventAvail so that we don't lose some important
	 events.}
	 
	FOR count := 1 TO 3 DO
		ignoreResult := EventAvail(everyEvent, event);
	
	{Ignore the error returned from SysEnvirons; even if an error occurred,
	 the SysEnvirons glue will fill in the SysEnvRec. You can save a redundant
	 call to SysEnvirons by calling it after initializing AppleTalk.}
	 
	ignoreError := SysEnvirons(kSysEnvironsVersion, gMac);
	
	{Make sure that the machine has at least 128K ROMs. If it doesn't, exit.}
	
	IF gMac.machineType < 0 THEN BigBadError(eWrongMachine);
	
	{1.02 - Move TrapAvailable call to after SysEnvirons so that we can tell
	 in TrapAvailable if a tool trap value is out of range.}
	 
	gHasWaitNextEvent := TrapAvailable(_WaitNextEvent, ToolTrap);

	{1.01 - We used to make a check for memory at this point by examining ApplLimit,
	 ApplicZone, and StackSpace and comparing that to the minimum size we told
	 MultiFinder we needed. This did not work well because it assumed too much about
	 the relationship between what we asked MultiFinder for and what we would actually
	 get back, as well as how to measure it. Instead, we will use an alternate
	 method comprised of two steps.}
	 
	{It is better to first check the size of the application heap against a value
	 that you have determined is the smallest heap the application can reasonably
	 work in. This number should be derived by examining the size of the heap that
	 is actually provided by MultiFinder when the minimum size requested is used.
	 The derivation of the minimum size requested from MultiFinder is described
	 in Sample.h. The check should be made because the preferred size can end up
	 being set smaller than the minimum size by the user. This extra check acts to
	 insure that your application is starting from a solid memory foundation.}
	 
	IF ORD(GetApplLimit) - ORD(ApplicZone) < kMinHeap THEN BigBadError(eSmallSize);
	
	{Next, make sure that enough memory is free for your application to run. It
	 is possible for a situation to arise where the heap may have been of required
	 size, but a large scrap was loaded which left too little memory. To check for
	 this, call PurgeSpace and compare the result with a value that you have determined
	 is the minimum amount of free memory your application needs at initialization.
	 This number can be derived several different ways. One way that is fairly
	 straightforward is to run the application in the minimum size configuration
	 as described previously. Call PurgeSpace at initialization and examine the value
	 returned. However, you should make sure that this result is not being modified
	 by the scrap's presence. You can do that by calling ZeroScrap before calling
	 PurgeSpace. Make sure to remove that call before shipping, though.}
	 
	{* ZeroScrap; *}
	PurgeSpace(total, contig);
	IF total < kMinSpace THEN
		IF UnloadScrap <> noErr THEN
			BigBadError(eNoMemory)
		ELSE BEGIN
			PurgeSpace(total, contig);
			IF total < kMinSpace THEN
				BigBadError(eNoMemory);
		END;

	{The extra benefit to waiting until after the Toolbox Managers have been initialized
	 to check memory is that we can now give the user an alert to tell her/him what
	 happened. Although it is possible that the memory situation could be worsened by
	 displaying an alert, MultiFinder would gracefully exit the application with
	 an informative alert if memory became critical. Here we are acting more
	 in a preventative manner to avoid future disaster from low-memory problems.}

	menuBar := GetNewMBar(rMenuBar);		{read menus into menu bar}
	IF menuBar = NIL THEN
		BigBadError(eNoMemory);
	SetMenuBar(menuBar);					{install menus}
	DisposHandle(menuBar);
	AddResMenu(GetMHandle(mApple), 'DRVR');	{add DA names to Apple menu}
	DrawMenuBar;

	gNumDocuments := 0;

	{do other initialization here}

	DoNew;									{create a single empty document}
END; {Initialize}


(**************************************************************************************
1.01 - PROCEDURE DoCloseBehind(window: WindowPtr); was removed.

{1.01 - DoCloseBehind was a good idea for closing windows when quitting
 and not having to worry about updating the windows, but it suffered
 from a fatal flaw. If a desk accessory owned two windows, it would
 close both those windows when CloseDeskAcc was called. When DoCloseBehind
 got around to calling DoCloseWindow for that other window that was already
 closed, things would go very poorly. Another option would be to have a
 procedure, GetRearWindow, that would go through the window list and return
 the last window. Instead, we decided to present the standard approach
 of getting and closing FrontWindow until FrontWindow returns NIL. This
 has a potential benefit in that the window whose document needs to be saved
 may be visible since it is the front window, therefore decreasing the
 chance of user confusion. For aesthetic reasons, the windows in the
 application should be checked for updates periodically and have the
 updates serviced.}
**************************************************************************************)


{$S Main}
PROCEDURE Terminate;

{Clean up the application and exit. We close all of the windows so that
 they can update their documents, if any.}
 
{1.01 - If we find out that a cancel has occurred, we won't exit to the
 shell, but will return instead.}
 
VAR
	aWindow	: WindowPtr;
	closed	: BOOLEAN;

BEGIN
	closed := TRUE;
	REPEAT
		aWindow := FrontWindow;					{get the current front window}
		IF aWindow <> NIL THEN
			closed := DoCloseWindow(aWindow);	{close this window}
	UNTIL (NOT closed) | (aWindow = NIL);		{do all windows}
	IF closed THEN
		ExitToShell;							{exit if no cancellation}
END; {Terminate}


{$S Main}
PROCEDURE AdjustMenus;

{Enable and disable menus based on the current state.
 The user can only select enabled menu items. We set up all the menu items
 before calling MenuSelect or MenuKey, since these are the only times that
 a menu item can be selected. Note that MenuSelect is also the only time
 the user will see menu items. This approach to deciding what enable/
 disable state a menu item has the advantage of concentrating all the decision-
 making in one routine, as opposed to being spread throughout the application.
 Other application designs may take a different approach that may or may not be
 as valid.}

VAR
	window			: WindowPtr;
	menu			: MenuHandle;
	offset			: LONGINT;
	undo			: BOOLEAN;
	cutCopyClear	: BOOLEAN;
	paste			: BOOLEAN;

BEGIN
	window := FrontWindow;

	menu := GetMHandle(mFile);
	IF gNumDocuments < kMaxOpenDocuments THEN
		EnableItem(menu, iNew)		{New is enabled when we can open more documents}
	ELSE
		DisableItem(menu, iNew);
	IF window <> NIL THEN			{Close is enabled when there is a window to close}
		EnableItem(menu, iClose)
	ELSE
		DisableItem(menu, iClose);

	menu := GetMHandle(mEdit);
	undo := FALSE;
	cutCopyClear := FALSE;
	paste := FALSE;
	IF IsDAWindow(window) THEN BEGIN
		undo := TRUE;				{all editing is enabled for DA windows}
		cutCopyClear := TRUE;
		paste := TRUE;
	END ELSE IF IsAppWindow(window) THEN BEGIN
		WITH DocumentPeek(window)^.docTE^^ DO
			IF selStart < selEnd THEN
				cutCopyClear := TRUE;
				{Cut, Copy, and Clear is enabled for app. windows with selections}
		IF GetScrap(NIL, 'TEXT', offset) > 0 THEN
			paste := TRUE;			{Paste is enabled for app. windows}
	END;
	IF undo THEN
		EnableItem(menu, iUndo)
	ELSE
		DisableItem(menu, iUndo);
	IF cutCopyClear THEN BEGIN
		EnableItem(menu, iCut);
		EnableItem(menu, iCopy);
		EnableItem(menu, iClear);
	END ELSE BEGIN
		DisableItem(menu, iCut);
		DisableItem(menu, iCopy);
		DisableItem(menu, iClear);
	END;
	IF paste THEN
		EnableItem(menu, iPaste)
	ELSE
		DisableItem(menu, iPaste);
END; {AdjustMenus}


{$S Main}
PROCEDURE DoMenuCommand(menuResult: LONGINT);

{This is called when an item is chosen from the menu bar (after calling
 MenuSelect or MenuKey). It does the right thing for each command.}
 
VAR
	menuID, menuItem		: INTEGER;
	itemHit, daRefNum		: INTEGER;
	daName					: Str255;
	ignoreResult, saveErr	: OSErr;
	handledByDA				: BOOLEAN;
	te						: TEHandle;
	window					: WindowPtr;
	ignore					: BOOLEAN;
	aHandle					: Handle;
	oldSize, newSize		: LongInt;
	total, contig			: LongInt;

BEGIN
	window := FrontWindow;
	menuID := HiWrd(menuResult);	{use built-ins (for efficiency)...}
	menuItem := LoWrd(menuResult);	{to get menu item number and menu number}
	CASE menuID OF
	
		mApple:
			CASE menuItem OF
				iAbout:				{bring up alert for About}
					itemHit := Alert(rAboutAlert, NIL);
				OTHERWISE BEGIN		{all non-About items in this menu are DAs}
					GetItem(GetMHandle(mApple), menuItem, daName);
					daRefNum := OpenDeskAcc(daName);
				END;
			END;
			
		mFile:
			CASE menuItem OF
				iNew:
					DoNew;
				iClose:
					ignore := DoCloseWindow(window); {we don't care if cancelled}
				iQuit:
					Terminate;
			END;
			
		mEdit: BEGIN				{call SystemEdit for DA editing & MultiFinder}
			IF NOT SystemEdit(menuItem-1) THEN BEGIN
				te := DocumentPeek(window)^.docTE;
				CASE menuItem OF
				
					iCut: BEGIN		{after cutting, export the TE scrap}
						IF ZeroScrap = noErr THEN BEGIN
							PurgeSpace(total, contig);
							IF (te^^.selEnd - te^^.selStart) + kTESlop > contig THEN
								AlertUser(eNoSpaceCut)
							ELSE BEGIN
								TECut(te);
								IF TEToScrap <> noErr THEN BEGIN
									AlertUser(eNoCut);
									ignoreResult := ZeroScrap;
								END;
							END;
						END;
					END;
					
					iCopy: BEGIN	{after copying, export the TE scrap}
						IF ZeroScrap = noErr THEN BEGIN
							TECopy(te);
							IF TEToScrap <> noErr THEN BEGIN
								AlertUser(eNoCopy);
								ignoreResult := ZeroScrap;
							END;
						END;
					END;
					
					iPaste: BEGIN	{import the TE scrap before pasting}
						IF TEFromScrap = noErr THEN BEGIN
							IF TEGetScrapLen + (te^^.teLength -
								(te^^.selEnd - te^^.selStart)) > kMaxTELength THEN
								AlertUser(eExceedPaste)
							ELSE BEGIN
									aHandle := Handle(TEGetText(te));
									oldSize := GetHandleSize(aHandle);
									newSize := oldSize + TEGetScrapLen + kTESlop;
									SetHandleSize(aHandle, newSize);
									saveErr := MemError;
									SetHandleSize(aHandle, oldSize);
									IF saveErr <> noErr THEN
										AlertUser(eNoSpacePaste)
									ELSE
										TEPaste(te);
								END;
						END ELSE
							AlertUser(eNoPaste);
					END;
					
					iClear:
						TEDelete(te);
						
				END;
				AdjustScrollbars(window, FALSE);
				AdjustTE(window);
			END;
		END;
	END;
	HiliteMenu(0);					{unhighlight what MenuSelect (or MenuKey) hilited}
END; {DoMenuCommand}


{$S Main}
PROCEDURE DrawWindow(window: WindowPtr);

{Draw the contents of an application window.}

BEGIN
	SetPort(window);
	WITH window^ DO BEGIN
		EraseRect(portRect);		{as per TextEdit chapter of Inside Macintosh}
		DrawControls(window);		{this ordering makes for a better appearance}
		DrawGrowIcon(window);
		TEUpdate(portRect, DocumentPeek(window)^.docTE);
	END;
END; {DrawWindow}


{$S Main}
FUNCTION GetSleep: LONGINT;

{Calculate a sleep value for WaitNextEvent. This takes into account the things
 that DoIdle does with idle time.}

VAR
	sleep	: LONGINT;
	window	: WindowPtr;

BEGIN
	sleep := MAXLONGINT;					{default value for sleep}
	IF NOT gInBackground THEN BEGIN			{if we are in front...}
		window := FrontWindow;			{and the front window is ours...}
		IF IsAppWindow(window) THEN BEGIN
			WITH DocumentPeek(window)^.docTE^^ DO
				IF selStart = selEnd THEN	{and the selection is an insertion point...}
					sleep := GetCaretTime;	{we need to blink the insertion point}
		END;
	END;
	GetSleep := sleep;
END; {GetSleep}


{$S Main}
PROCEDURE CommonAction(control: ControlHandle; VAR amount: INTEGER);

{Common algorithm for setting the new value of a control. It returns the actual amount
the value of the control changed. Note the pinning is done for the sake of returning
the amount the control value changed.}

VAR
	value, max	: INTEGER;
	window		: WindowPtr;
BEGIN
	value := GetCtlValue(control);	{get current value}
	max := GetCtlMax(control);		{and max value}
	amount := value - amount;
	IF amount < 0 THEN
		amount := 0
	ELSE IF amount > max THEN
		amount := max;
	SetCtlValue(control, amount);
	amount := value - amount;		{calculate true change}
END; {CommonAction}


{$S Main}
PROCEDURE VActionProc(control: ControlHandle; part: INTEGER);

{Determines how much to change the value of the vertical scrollbar by and how
much to scroll the TE record.}

VAR
	amount	: INTEGER;
	window	: WindowPtr;
BEGIN
	IF part <> 0 THEN BEGIN
		window := control^^.contrlOwner;
		WITH DocumentPeek(window)^, DocumentPeek(window)^.docTE^^ DO BEGIN
			CASE part OF
				inUpButton, inDownButton:
					amount := 1;												{one line}
				inPageUp, inPageDown:
					amount := (viewRect.bottom - viewRect.top) DIV lineHeight;	{one page}
			END;
			IF (part = inDownButton) | (part = inPageDown) THEN
				amount := -amount;												{reverse direction}
			CommonAction(control, amount);
			IF amount <> 0 THEN
				TEScroll(0, amount * docTE^^.lineHeight, docTE);
		END;
	END;
END; {VActionProc}


{$S Main}
PROCEDURE HActionProc(control: ControlHandle; part: INTEGER);

{Determines how much to change the value of the horizontal scrollbar by and how
much to scroll the TE record.}

VAR
	amount	: INTEGER;
	window	: WindowPtr;
BEGIN
	IF part <> 0 THEN BEGIN
		window := control^^.contrlOwner;
		WITH DocumentPeek(window)^, DocumentPeek(window)^.docTE^^ DO BEGIN
			CASE part OF
				inUpButton, inDownButton:										{a few pixels}
					amount := kButtonScroll;
				inPageUp, inPageDown:
					amount := viewRect.right - viewRect.left;					{a page}
			END;
			IF (part = inDownButton) | (part = inPageDown) THEN
				amount := -amount;												{reverse direction}
			CommonAction(control, amount);
			IF amount <> 0 THEN
				TEScroll(amount, 0, docTE);
		END;
	END;
END; {HActionProc}


{$S Main}
PROCEDURE DoIdle;

{This is called whenever we get an null event or a mouse-moved event.
 It takes care of necessary periodic actions. For this program, it calls TEIdle.}

VAR
	window	: WindowPtr;

BEGIN
	window := FrontWindow;
	IF IsAppWindow(window) THEN
		TEIdle(DocumentPeek(window)^.docTE);
END; {DoIdle}


{$S Main}
PROCEDURE DoKeyDown(event: EventRecord);

{This is called for any keyDown or autoKey events, except when the
 Command key is held down. It looks at the frontmost window to decide what
 to do with the key typed.}

VAR
	window	: WindowPtr;
	key		: CHAR;
	te		: TEHandle;

BEGIN
	window := FrontWindow;
	IF IsAppWindow(window) THEN BEGIN
		te := DocumentPeek(window)^.docTE;
		key := CHR(BAnd(event.message, charCodeMask));
		IF (key = CHR(kDelChar)) |							{don't count deletes}
			(te^^.teLength - (te^^.selEnd - te^^.selStart)
							+ 1 < kMaxTELength) THEN BEGIN	{but check haven't gone past}
			TEKey(key, te);
			AdjustScrollbars(window, FALSE);
			AdjustTE(window);
		END ELSE
			AlertUser(eExceedChar);
	END;
END; {DoKeyDown}


{$S Main}
PROCEDURE DoContentClick(window: WindowPtr; event: EventRecord);

{Called when a mouseDown occurs in the content of a window.}

VAR
	mouse		: Point;
	control		: ControlHandle;
	part, value	: INTEGER;
	shiftDown	: BOOLEAN;
	teRect		: Rect;

BEGIN
	IF IsAppWindow(window) THEN BEGIN
		SetPort(window);
		mouse := event.where;											{get the click position}
		GlobalToLocal(mouse);											{convert to local coordinates}
		{see if we are in the viewRect. if so, we won’t check the controls}
		GetTERect(window, teRect);
		IF PtInRect(mouse, teRect) THEN BEGIN
			shiftDown := BAnd(event.modifiers, shiftKey) <> 0;	{extend if Shift is down}
			TEClick(mouse, shiftDown, DocumentPeek(window)^.docTE);
		END ELSE BEGIN
			part := FindControl(mouse, window, control);
			WITH DocumentPeek(window)^ DO
				CASE part OF
					0:;											{do nothing for viewRect case}
					inThumb: BEGIN
						value := GetCtlValue(control);
						part := TrackControl(control, mouse, NIL);
						IF part <> 0 THEN BEGIN
							value := value - GetCtlValue(control);
							IF value <> 0 THEN
								IF control = docVScroll THEN
									TEScroll(0, value * docTE^^.lineHeight, docTE)
								ELSE
									TEScroll(value, 0, docTE);
						END;
					END;
					OTHERWISE									{must be page or button}
						IF control = docVScroll THEN
							value := TrackControl(control, mouse, @VActionProc)
						ELSE
							value := TrackControl(control, mouse, @HActionProc);
				END;
		END;
	END;
END; {DoContentClick}


{$S Main}
PROCEDURE ResizeWindow(window: WindowPtr);

{Called when the window has been resized to fix up the controls and content}

BEGIN
	WITH window^ DO BEGIN
		AdjustScrollbars(window, TRUE);
		AdjustTE(window);
		InvalRect(portRect);
	END;
END; {ResizeWindow}


{$S Main}
PROCEDURE GetLocalUpdateRgn(window: WindowPtr; localRgn: RgnHandle);

{Returns the update region in local coordinates}

BEGIN
	CopyRgn(WindowPeek(window)^.updateRgn, localRgn);						{save old update region}
	WITH window^.portBits.bounds DO
		OffsetRgn(localRgn, left, top);										{convert to local coords}
END; {GetLocalUpdateRgn}


{$S Main}
PROCEDURE DoGrowWindow(window: WindowPtr; event: EventRecord);

{Called when a mouseDown occurs in the grow box of an active window. In
 order to eliminate any 'flicker', we want to invalidate only what is
 necessary. Since ResizeWindow invalidates the whole portRect, we save
 the old TE viewRect, intersect it with the new TE viewRect, and
 remove the result from the update region. However, we must make sure
 that any old update region that might have been around gets put back.}

VAR
	growResult		: LONGINT;
	tempRect		: Rect;
	tempRgn			: RgnHandle;
	ignoreResult	: BOOLEAN;

BEGIN
	WITH screenBits.bounds DO
		SetRect(tempRect, kMinDocDim, kMinDocDim, right, bottom);			{set up limiting values}
	growResult := GrowWindow(window, event.where, tempRect);
	IF growResult <> 0 THEN 												{see if changed size}
		WITH DocumentPeek(window)^, window^ DO BEGIN
			tempRect := docTE^^.viewRect;									{save old text box}
			tempRgn := NewRgn;
			GetLocalUpdateRgn(window, tempRgn);								{get localized update region}
			SizeWindow(window, LoWrd(growResult), HiWrd(growResult), TRUE);
			ResizeWindow(window);
			ignoreResult := SectRect(tempRect, docTE^^.viewRect, tempRect);	{find what stayed same}
			ValidRect(tempRect);											{take it out of update}
			InvalRgn(tempRgn);												{put back any prior update}
			DisposeRgn(tempRgn);
		END;
END; {DoGrowWindow}


{$S Main}
PROCEDURE DoZoomWindow(window: WindowPtr; part: INTEGER);

{Called when a mouseClick occurs in the zoom box of an active window.
 Everything has to get re-drawn here, so we don't mind that
 ResizeWindow invalidates the whole portRect.}

BEGIN
	WITH window^ DO BEGIN
		EraseRect(portRect);
		ZoomWindow(window, part, (window = FrontWindow));
		ResizeWindow(window);
	END;
END; {DoZoomWindow}


{$S Main}
PROCEDURE DoUpdate(window: WindowPtr);

{This is called when an update event is received for a window.
 It calls DrawWindow to draw the contents of an application window,
 but only if the visRgn is non-empty; for efficiency reasons,
 not because it is required.}

BEGIN
	IF IsAppWindow(window) THEN BEGIN
		BeginUpdate(window);					{this sets up the visRgn}
		IF NOT EmptyRgn(window^.visRgn) THEN	{draw if updating needs to be done}
			DrawWindow(window);
		EndUpdate(window);
	END;
END; {DoUpdate}


{$S Main}
PROCEDURE DoActivate(window: WindowPtr; becomingActive: BOOLEAN);

{This is called when a window is activated or deactivated.}

VAR
	tempRgn, clipRgn	: RgnHandle;
	growRect			: Rect;

BEGIN
	IF IsAppWindow(window) THEN
		WITH DocumentPeek(window)^ DO
			IF becomingActive THEN BEGIN
				{since we don’t want TEActivate to draw a selection in an area where
				 we’re going to erase and redraw, we’ll clip out the update region
				 before calling it.}
				tempRgn := NewRgn;
				clipRgn := NewRgn;
				GetLocalUpdateRgn(window, tempRgn);			{get localized update region}
				GetClip(clipRgn);
				DiffRgn(clipRgn, tempRgn, tempRgn);			{subtract updateRgn from clipRgn}
				SetClip(tempRgn);
				TEActivate(docTE);							{let TE do its thing}
				SetClip(clipRgn);							{restore the full-blown clipRgn}
				DisposeRgn(tempRgn);
				DisposeRgn(clipRgn);

				{the controls need to be redrawn on activation:}
				docVScroll^^.contrlVis := kControlVisible;
				docHScroll^^.contrlVis := kControlVisible;
				InvalRect(docVScroll^^.contrlRect);
				InvalRect(docHScroll^^.contrlRect);
				{the growbox needs to be redrawn on activation:}
				growRect := window^.portRect;
				WITH growRect DO BEGIN
					top := bottom - kScrollbarAdjust;		{adjust for the scrollbars}
					left := right - kScrollbarAdjust;
					END;
				InvalRect(growRect);
			END ELSE BEGIN
				TEDeactivate(docTE);
				{the controls should be hidden immediately on deactivation:}
				HideControl(docVScroll);
				HideControl(docHScroll);
				{the growbox should be changed immediately on deactivation:}
				DrawGrowIcon(window);
			END;
END; {DoActivate}


{$S Main}
PROCEDURE GetGlobalMouse(VAR mouse: Point);

{Get the global coordinates of the mouse. When you call OSEventAvail
 it will return either a pending event or a null event. In either case,
 the where field of the event record will contain the current position
 of the mouse in global coordinates and the modifiers field will reflect
 the current state of the modifiers. Another way to get the global
 coordinates is to call GetMouse and LocalToGlobal, but that requires
 being sure that thePort is set to a valid port.}

VAR
	event	: EventRecord;
	
BEGIN
	IF OSEventAvail(kNoEvents, event) THEN;	{we aren't interested in any events}
	mouse := event.where;					{just the mouse position}
END;


{$S Main}
PROCEDURE AdjustCursor(mouse: Point; region: RgnHandle);

{Change the cursor's shape, depending on its position. This also calculates the region
 where the current cursor resides (for WaitNextEvent). If the mouse is ever outside of
 that region, an event is generated, causing this routine to be called. This
 allows us to change the region to the region the mouse is currently in. If
 there is more to the event than just “the mouse moved”, we get called before the
 event is processed to make sure the cursor is the right one. In any (ahem) event,
 this is called again before we fall back into WNE.}


VAR
	window		: WindowPtr;
	arrowRgn	: RgnHandle;
	iBeamRgn	: RgnHandle;
	iBeamRect	: Rect;

BEGIN
	window := FrontWindow;	{we only adjust the cursor when we are in front}
	IF (NOT gInBackground) AND (NOT IsDAWindow(window)) THEN BEGIN
		{calculate regions for different cursor shapes}
		arrowRgn := NewRgn;
		iBeamRgn := NewRgn;

		{start with a big, big rectangular region}
		SetRectRgn(arrowRgn, kExtremeNeg, kExtremeNeg, kExtremePos, kExtremePos);

		{calculate iBeamRgn}
		IF IsAppWindow(window) THEN BEGIN
			iBeamRect := DocumentPeek(window)^.docTE^^.viewRect;
			SetPort(window);					{make a global version of the viewRect}
			WITH iBeamRect DO BEGIN
				LocalToGlobal(topLeft);
				LocalToGlobal(botRight);
			END;
			RectRgn(iBeamRgn, iBeamRect);
			WITH window^.portBits.bounds DO
				SetOrigin(-left, -top);
			SectRgn(iBeamRgn, window^.visRgn, iBeamRgn);
			SetOrigin(0, 0);
		END;

		{subtract other regions from arrowRgn}
		DiffRgn(arrowRgn, iBeamRgn, arrowRgn);
		
		{change the cursor and the region parameter}
		IF PtInRgn(mouse, iBeamRgn) THEN BEGIN
			SetCursor(GetCursor(iBeamCursor)^^);
			CopyRgn(iBeamRgn, region);
		END ELSE BEGIN
			SetCursor(arrow);
			CopyRgn(arrowRgn, region);
		END;

		{get rid of our local regions}
		DisposeRgn(arrowRgn);
		DisposeRgn(iBeamRgn);
	END;
END; {AdjustCursor}


{$S Main}
PROCEDURE DoEvent(event: EventRecord);

{Do the right thing for an event. Determine what kind of event it is, and call
 the appropriate routines.}

VAR
	part, err	: INTEGER;
	window		: WindowPtr;
	key			: CHAR;
	ignore		: BOOLEAN;
	aPoint		: Point;

BEGIN
	CASE event.what OF
		nullEvent:
			DoIdle;
		mouseDown: BEGIN
			part := FindWindow(event.where, window);
			CASE part OF
				inMenuBar: BEGIN
					AdjustMenus;
					DoMenuCommand(MenuSelect(event.where));
				END;
				inSysWindow:
					SystemClick(event, window);
				inContent:
					IF window <> FrontWindow THEN BEGIN
						SelectWindow(window);
						{DoEvent(event);}	{use this line for "do first click"}
					END ELSE
						DoContentClick(window, event);
				inDrag:
					DragWindow(window, event.where, screenBits.bounds);
				inGrow:
					DoGrowWindow(window, event);
				inGoAway:
					IF TrackGoAway(window, event.where) THEN
						ignore := DoCloseWindow(window);		{we don't care if cancelled}
				inZoomIn, inZoomOut:
					IF TrackBox(window, event.where, part) THEN
						DoZoomWindow(window, part);
			END;
		END;
		keyDown, autoKey: BEGIN
			key := CHR(BAnd(event.message, charCodeMask));
			IF BAnd(event.modifiers, cmdKey) <> 0 THEN BEGIN	{Command key down}
				IF event.what = keyDown THEN BEGIN
					AdjustMenus;			{enable/disable/check menu items properly}
					DoMenuCommand(MenuKey(key));
				END;
			END ELSE
				DoKeyDown(event);
		END;								{call DoActivate with the window and...}
		activateEvt:						{TRUE for activate, FALSE for deactivate}
			DoActivate(WindowPtr(event.message), BAnd(event.modifiers, activeFlag) <> 0);
		updateEvt:							{call DoUpdate with the window to update}
			DoUpdate(WindowPtr(event.message));
		{1.01 - It is not a bad idea to at least call DIBadMount in response
		 to a diskEvt, so that the user can format a floppy.}
		diskEvt:
			IF HiWrd(event.message) <> noErr THEN BEGIN
				SetPt(aPoint, kDILeft, kDITop);
				err := DIBadMount(aPoint, event.message);
			END;
		kOSEvent:
			CASE BAnd(BRotL(event.message, 8), $FF) OF	{high byte of message}
				kMouseMovedMessage:
					DoIdle;					{mouse moved is also an idle event}
				kSuspendResumeMessage: BEGIN
					gInBackground := BAnd(event.message, kResumeMask) = 0;
					DoActivate(FrontWindow, NOT gInBackground);
				END;
			END;
	END;
END; {DoEvent}


{$S Main}
PROCEDURE EventLoop;

{Get events forever, and handle them by calling DoEvent.
 Also call AdjustCursor each time through the loop.}

VAR
	cursorRgn	: RgnHandle;
	gotEvent	: BOOLEAN;
	event		: EventRecord;
	mouse		: Point;

BEGIN
	cursorRgn := NewRgn;		{we'll pass an empty region to WNE the first time thru}
	REPEAT
		IF gHasWaitNextEvent THEN BEGIN	{put us 'asleep' forever under MultiFinder}
			GetGlobalMouse(mouse);		{since we might go to sleep}
			AdjustCursor(mouse, cursorRgn);
			gotEvent := WaitNextEvent(everyEvent, event, GetSleep, cursorRgn);
		END ELSE BEGIN
			SystemTask;					{must be called if using GetNextEvent}
			gotEvent := GetNextEvent(everyEvent, event);
		END;
		IF gotEvent THEN BEGIN
			AdjustCursor(event.where, cursorRgn);	{make sure we have the right cursor}
			DoEvent(event);				{Handle the event only if for us.}
			END
		ELSE
			DoIdle;
		{If you are using modeless dialogs that have editText items,
		 you will want to call IsDialogEvent to give the caret a chance
		 to blink, even if WNE/GNE returned FALSE. However, check FrontWindow
		 for a non-NIL value before calling IsDialogEvent.}
	UNTIL FALSE;	{loop forever}
END; {EventLoop}


PROCEDURE _DataInit; EXTERNAL;

{This routine is automatically linked in by the MPW Linker. This external
 reference to it is done so that we can unload its segment, %A5Init.}


{$S Main}
BEGIN
	UnloadSeg(@_DataInit);	{note that _DataInit must not be in Main!}
	
	{1.01 - call to ForceEnvirons removed}
	{If you have stack requirements that differ from the default,
	 then you could use SetApplLimit to increase StackSpace at 
	 this point, before calling MaxApplZone.}
	 
	MaxApplZone;			{expand the heap so code segments load at the top}

	Initialize;				{initialize the program}
	UnloadSeg(@Initialize);	{note that Initialize must not be in Main!}

	EventLoop;				{call the main event loop}
END.
