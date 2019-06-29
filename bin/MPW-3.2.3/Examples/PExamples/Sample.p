{------------------------------------------------------------------------------
#
#	Apple Macintosh Developer Technical Support
#
#	MultiFinder-Aware Simple Sample Application
#
#	Sample
#
#	Sample.p	-	Pascal Source
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
#				Sample.p			Feb.  1, 1990
#				Sample.r			Feb.  1, 1990
#				Sample.h			Feb.  1, 1990
#				Sample.make			Feb.  1, 1990
#
#	Sample is an example application that demonstrates how to
#	initialize the commonly used toolbox managers, operate 
#	successfully under MultiFinder, handle desk accessories, 
#	and create, grow, and zoom windows.
#
#	It does not by any means demonstrate all the techniques 
#	you need for a large application. In particular, Sample 
#	does not cover exception handling, multiple windows/documents, 
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
#	We recommend that you review this program or TESample before 
#	beginning a new application.
#
------------------------------------------------------------------------------}


PROGRAM Sample;


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


USES
	MemTypes, QuickDraw, OSIntf, ToolIntf, PackIntf, Traps;

CONST
	{MPW 3.0 will include a Traps.p interface file that includes constants for trap numbers.
	 These constants are from that file.}
	{1.02 - since using MPW 3.0 only, we include Traps.p, so we now have trap numbers.}

	{1.01 - changed constants to begin with 'k' for consistency, except for resource IDs}
	{SysEnvironsVersion is passed to SysEnvirons to tell it which version of the
	 SysEnvRec we understand.}
	kSysEnvironsVersion		= 1;

	{OSEvent is the event number of the suspend/resume and mouse-moved events sent
	 by MultiFinder. Once we determine that an event is an osEvent, we look at the
	 high byte of the message sent to determine which kind it is. To differentiate
	 suspend and resume events we check the resumeMask bit.}
	kOSEvent				= app4Evt;	{event used by MultiFinder}
	kSuspendResumeMessage	= 1;		{high byte of suspend/resume event message}
	kResumeMask				= 1;		{bit of message field for resume vs. suspend}
	kNoEvents				= 0;		{no events mask}

	{1.01 - kMinHeap - This is the minimum result from the following
	 equation:
			
			ORD(GetApplLimit) - ORD(ApplicZone)
			
	 for the application to run. It will insure that enough memory will
	 be around for reasonable-sized scraps, FKEYs, etc. to exist with the
	 application, and still give the application some 'breathing room'.
	 To derive this number, we ran under a MultiFinder partition that was
	 our requested minimum size, as given in the 'SIZE' resource.}
	 
	kMinHeap	= 21 * 1024;
	
	{1.01 - kMinSpace - This is the minimum result from PurgeSpace, when called
	 at initialization time, for the application to run. This number acts
	 as a double-check to insure that there really is enough memory for the
	 application to run, including what has been taken up already by
	 pre-loaded resources, the scrap, code, and other sundry memory blocks.}
	 
	kMinSpace	= 8 * 1024;
	
	{kExtremeNeg and kExtremePos are used to set up wide open rectangles and regions.}
	kExtremeNeg	= -32768;
	kExtremePos	= 32767 - 1;			{required for old region bug}
	
	{The following constants are all resource IDs, corresponding to resources in Sample.r.}
	rMenuBar	= 128;					{application's menu bar}
	rAboutAlert	= 128;					{about alert}
	rUserAlert	= 129;					{error user alert}
	rWindow		= 128;					{application's window}
	rStopRect	= 128;					{rectangle for Stop light}
	rGoRect		= 129;					{rectangle for Go light}

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

	mLight		= 131;					{Light menu}
	iStop		= 1;
	iGo			= 2;
	
	{1.01 - kDITop and kDILeft are used to locate the Disk Initialization dialogs.}
	kDITop		= $0050;
	kDILeft		= $0070;


VAR
	{The "g" prefix is used to emphasize that a variable is global.}

	{GMac is used to hold the result of a SysEnvirons call. This makes
	 it convenient for any routine to check the environment. It is
	 global information, anyway.}
	gMac				: SysEnvRec;	{set up by Initialize}

	{GHasWaitNextEvent is set at startup, and tells whether the WaitNextEvent
	 trap is available. If it is false, we know that we must call GetNextEvent.}
	gHasWaitNextEvent	: BOOLEAN;		{set up by Initialize}

	{GInBackground is maintained by our osEvent handling routines. Any part of
	 the program can check it to find out if it is currently in the background.}
	gInBackground		: BOOLEAN;		{maintained by Initialize and DoEvent}


	{The following globals are the state of the window. If we supported more than
	 one window, they would be attatched to each document, rather than globals.}

	{GStopped tells whether the stop light is currently on stop or go.}
	gStopped			: BOOLEAN;		{maintained by Initialize and SetLight}

	{GStopRect and gGoRect are the rectangles of the two stop lights in the window.}
	gStopRect			: Rect;			{set up by Initialize}
	gGoRect				: Rect;			{set up by Initialize}


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
PROCEDURE AlertUser;

{Display an alert that tells the user an error occurred, then exit the program.
 This routine is used as an ultimate bail-out for serious errors that prohibit
 the continuation of the application. Errors that do not require the termination
 of the application should be handled in a different manner. Error checking and
 reporting has a place even in the simplest application. For simplicity, the alert
 displayed here only says that an error occurred, but not what it was. There are
 various methods available for being more specific.}
 
VAR
	itemHit	: INTEGER;
BEGIN
	SetCursor(arrow);
	itemHit := Alert(rUserAlert, NIL);
	ExitToShell;
END; {AlertUser}


{$S Main}
FUNCTION DoCloseWindow(window: WindowPtr) : BOOLEAN;

{Close a window.}

{1.01 - At this point, if there was a document associated with a
 window, you could do any document saving processing if it is 'dirty'.
 DoCloseWindow would return TRUE if the window actually closes, i.e.,
 the user does not cancel from a save dialog. This result is handy when
 the user quits an application, but then cancels a save of a document
 associated with a window. We also added code to close the application
 window since otherwise, the termination routines would never stop looping,
 waiting for FrontWindow to return NIL.}

BEGIN
	DoCloseWindow := TRUE;
	IF IsDAWindow(window) THEN
		CloseDeskAcc(WindowPeek(window)^.windowKind);
	IF IsAppWindow(window) THEN
		CloseWindow(window);
END; {DoCloseWindow}


{$S Initialize}
FUNCTION GoGetRect(rectID: INTEGER; VAR theRect: Rect) : BOOLEAN;
 
{This utility loads the global rectangles that are used by the window
 drawing routines. It shows how the resource manager can be used to hold
 values in a convenient manner. These values are then easily altered without
 having to re-compile the source code. In this particular case, we know
 that this routine is being called at initialization time. Therefore,
 if this returns FALSE, we will assume that the application is in such
 bad shape that we should just exit. Your error handling may differ, but
 the check should still be made.}

{1.01 - Changed GoGetRect to return a BOOLEAN that indicates if it was successful
 in getting the rectangle rather than just doing ExitToShell.}
 
TYPE
	RectPtr		= ^Rect;
	RectHnd		= ^RectPtr;
VAR
	resource	: Handle;
BEGIN
	resource := GetResource('RECT', rectID);
	IF resource <> NIL THEN BEGIN
		GoGetRect := TRUE;
		theRect := RectHnd(resource)^^;
		END
	ELSE
		GoGetRect := FALSE;
END; {GoGetRect}


{$S Initialize}
PROCEDURE Initialize;

{Set up the whole world, including global variables, Toolbox managers,
 and menus. We also create our one application window at this time.
 Since window storage is non-relocateable, how and when to allocate space
 for windows is very important so that heap fragmentation does not occur.
 Because Sample has only one window and it is only disposed when the application
 quits, we will allocate its space here, before anything that might be a locked
 relocatable object gets into the heap. This way, we can force its storage to be
 in the lowest memory available in the heap. Window storage can differ widely
 amongst applications depending on how many windows are created and disposed.
 If a failure occurs here, we will consider that the application is in such
 bad shape that we should just exit. Your error handling may differ, but
 the checks should still be made.}
 
{1.01 - The code that used to be part of ForceEnvirons has been moved into
 this module. If an error is detected, instead of merely doing an ExitToShell,
 which leaves the user without much to go on, we call AlertUser, which puts
 up a simple alert that just says an error occurred and then calls ExitToShell.
 In the interests of keeping things simple, the alert does not state the specific
 cause of the error, but a more informative alert would be appropriate for more
 sophisticated applications. Since there is no other cleanup needed at this point
 if an error is detected, this form of error- handling is acceptable. If more
 sophisticated error recovery is needed, a signal mechanism, such as is provided
 by Signals, can be used.}

VAR
	menuBar			: Handle;
	window			: WindowPtr;
	ignoreError		: OSErr;
	total, contig	: LongInt;
	ignoreResult	: BOOLEAN;
	event			: EventRecord;
	count			: INTEGER;

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
	
	IF gMac.machineType < 0 THEN AlertUser;
	
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
	 
	IF ORD(GetApplLimit) - ORD(ApplicZone) < kMinHeap THEN AlertUser;
	
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
	 
	PurgeSpace(total, contig);
	IF total < kMinSpace THEN AlertUser;

	{The extra benefit to waitng until after the Toolbox Managers have been initialized
	 before checking memory is that we can now give the user an alert to tell him what
	 happened. Although it is possible that the memory situation could be worsened by
	 displaying an alert, MultiFinder would gracefully exit the application with
	 an informative alert if memory became critical. Here we are acting more
	 in a preventative manner to avoid future disaster from low-memory problems.}
	 
	{We will allocate our own window storage instead of letting the Window
	 Manager for two reasons. One, GetNewWindow locks the 'WIND' resource
	 handle before calling NewWindow and this can lead to heap fragmentation.
	 Two, it takes just as much time for NewWindow to get the memory as it
	 does for us to get it.}

	window := WindowPtr(NewPtr(SIZEOF(WindowRecord)));
	IF window = NIL THEN AlertUser;
	window := GetNewWindow(rWindow, Ptr(window), WindowPtr(-1));

	menuBar := GetNewMBar(rMenuBar);		{read menus into menu bar}
	IF menuBar = NIL THEN AlertUser;
	SetMenuBar(menuBar);					{install menus}
	DisposHandle(menuBar);
	AddResMenu(GetMHandle(mApple), 'DRVR');	{add DA names to Apple menu}
	DrawMenuBar;

	gStopped := TRUE;
	IF NOT GoGetRect(rStopRect, gStopRect) THEN
		AlertUser;							{the stop light rectangle}
	IF NOT GoGetRect(rGoRect, gGoRect) THEN
		AlertUser;							{the go light rectangle}
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

{Clean up the application and exits. We close all of the windows so that
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
PROCEDURE SetLight(window: WindowPtr; newStopped: BOOLEAN);

{Change the setting of the light.}

BEGIN
	IF newStopped <> gStopped THEN BEGIN
		gStopped := newStopped;
		SetPort(window);
		InvalRect(window^.portRect);
	END;
END; {SetLight}


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
 just as valid.}

VAR
	window			: WindowPtr;
	menu			: MenuHandle;

BEGIN
	window := FrontWindow;

	menu := GetMHandle(mFile);
	IF IsDAWindow(window) THEN				{we can allow desk accessories to be closed from the menu}
		EnableItem(menu, iClose)
	ELSE
		DisableItem(menu, iClose);			{but not our traffic light window}

	menu := GetMHandle(mEdit);
	IF IsDAWindow(window) THEN BEGIN		{a desk accessory might need the edit menu}
		EnableItem(menu, iUndo);
		EnableItem(menu, iCut);
		EnableItem(menu, iCopy);
		EnableItem(menu, iPaste);
		EnableItem(menu, iClear);
	END ELSE BEGIN							{but we know we do not}
		DisableItem(menu, iUndo);
		DisableItem(menu, iCut);
		DisableItem(menu, iCopy);
		DisableItem(menu, iClear);
		DisableItem(menu, iPaste);
	END;

	menu := GetMHandle(mLight);
	IF IsAppWindow(window) THEN BEGIN		{we know that it must be the traffic light}
		EnableItem(menu, iStop);
		EnableItem(menu, iGo);
	END ELSE BEGIN
		DisableItem(menu, iStop);
		DisableItem(menu, iGo);
	END;
	CheckItem(menu, iStop, gStopped);		{we can also determine check/uncheck state, too}
	CheckItem(menu, iGo, NOT gStopped);
END; {AdjustMenus}


{$S Main}
PROCEDURE DoMenuCommand(menuResult: LONGINT);

{This is called when an item is chosen from the menu bar (after calling
 MenuSelect or MenuKey). It performs the right operation for each command.
 It is good to have both the result of MenuSelect and MenuKey go to
 one routine like this to keep everything organized.}

VAR
	menuID			: INTEGER;		{the resource ID of the selected menu}
	menuItem		: INTEGER;		{the item number of the selected menu}
	itemHit			: INTEGER;
	daName			: Str255;
	daRefNum		: INTEGER;
	handledByDA		: BOOLEAN;
	ignore			: BOOLEAN;

BEGIN
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
				iClose:
					ignore := DoCloseWindow(FrontWindow); {we don't care if cancelled}
				iQuit:
					Terminate;
			END;
		mEdit:						{call SystemEdit for DA editing & MultiFinder}
			handledByDA := SystemEdit(menuItem-1);	{since we don't do any editing}
		mLight:
			CASE menuItem OF
				iStop:
					SetLight(FrontWindow, TRUE);
				iGo:
					SetLight(FrontWindow, FALSE);
			END;
	END;
	HiliteMenu(0);					{unhighlight what MenuSelect (or MenuKey) hilited}
END; {DoMenuCommand}


{$S Main}
PROCEDURE DrawWindow(window: WindowPtr);

{Draw the contents of the application window. We do some drawing in color, using
 Classic QuickDraw's color capabilities. This will be black and white on old
 machines, but color on color machines. At this point, the window's visRgn is
 set to allow drawing only where it needs to be done.}

BEGIN
	SetPort(window);

	EraseRect(window^.portRect);	{clear out any garbage that might be left behind}
	IF gStopped THEN				{draw a red (or white) stop light}
		ForeColor(redColor)
	ELSE
		ForeColor(whiteColor);
	PaintOval(gStopRect);
	ForeColor(blackColor);
	FrameOval(gStopRect);
	IF NOT gStopped THEN			{draw a green (or white) go light}
		ForeColor(greenColor)
	ELSE
		ForeColor(whiteColor);
	PaintOval(gGoRect);
	ForeColor(blackColor);
	FrameOval(gGoRect);
END; {DrawWindow}


{$S Main}
PROCEDURE DoContentClick(window: WindowPtr; event: EventRecord);

{This is called when a mouse-down event occurs in the content of a window.
 Other applications might want to call FindControl, TEClick, etc., to
 further process the click.}

BEGIN
	SetLight(window, NOT gStopped);
END; {DoContentClick}


{$S Main}
PROCEDURE DoUpdate(window: WindowPtr);

{This is called when an update event is received for a window.
 It calls DrawWindow to draw the contents of an application window.
 As an effeciency measure that does not have to be followed, it
 calls the drawing routine only if the visRgn is non-empty. This
 will handle situations where calculations for drawing or drawing
 itself is very time-consuming.}

BEGIN
	IF IsAppWindow(window) THEN BEGIN
		BeginUpdate(window);					{sets up the visRgn, clears updateRgn}
		IF NOT EmptyRgn(window^.visRgn) THEN	{draw if updating needs to be done}
			DrawWindow(window);
		EndUpdate(window);						{restores the visRgn}
	END;
END; {DoUpdate}


{$S Main}
PROCEDURE DoActivate(window: WindowPtr; becomingActive: BOOLEAN);

{This is called when a window is activated or deactivated.
 In Sample, the Window Manager's handling of activate and
 deactivate events is sufficient. Other applications may have
 TextEdit records, controls, lists, etc., to activate/deactivate.}

BEGIN
	IF IsAppWindow(window) THEN
		IF becomingActive THEN
			{do whatever you need to at activation}
		ELSE
			{do whatever you need to at deactivation};
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
	window				: WindowPtr;
	arrowRgn			: RgnHandle;
	plusRgn				: RgnHandle;
	globalPortRect		: Rect;
	

BEGIN
	window := FrontWindow;	{we only adjust the cursor when we are in front}
	IF (NOT gInBackground) AND (NOT IsDAWindow(window)) THEN BEGIN
		{calculate regions for different cursor shapes}
		arrowRgn := NewRgn;
		plusRgn := NewRgn;

		{start with a big, big rectangular region}
		{1.01 - changed to kExtremeNeg and kExtremePos for consistency}
		SetRectRgn(arrowRgn, kExtremeNeg, kExtremeNeg,
							kExtremePos, kExtremePos);

		{calculate plusRgn}
		IF IsAppWindow(window) THEN BEGIN
			SetPort(window);			{make a global version of the portRect}
			SetOrigin(-window^.portBits.bounds.left, -window^.portBits.bounds.top);
			globalPortRect := window^.portRect;
			RectRgn(plusRgn, globalPortRect);
			SectRgn(plusRgn, window^.visRgn, plusRgn);
			SetOrigin(0, 0);
		END;

		{subtract other regions from arrowRgn}
		DiffRgn(arrowRgn, plusRgn, arrowRgn);

		{change the cursor and the region parameter}
		IF PtInRgn(mouse, plusRgn) THEN BEGIN
			SetCursor(GetCursor(plusCursor)^^);
			CopyRgn(plusRgn, region);
		END ELSE BEGIN
			SetCursor(arrow);
			CopyRgn(arrowRgn, region);
		END;

		{get rid of our local regions}
		DisposeRgn(arrowRgn);
		DisposeRgn(plusRgn);
	END;
END; {AdjustCursor}


{$S Main}
PROCEDURE DoEvent(event: EventRecord);

{Do the right thing for an event. Determine what kind of event it is, and call
 the appropriate routines.}

VAR
	part, err	: INTEGER;
	window		: WindowPtr;
	hit			: BOOLEAN;
	key			: CHAR;
	aPoint		: Point;

BEGIN
	CASE event.what OF
		mouseDown: BEGIN
			part := FindWindow(event.where, window);
			CASE part OF
				inMenuBar: BEGIN			{process the menu command}
					AdjustMenus;
					DoMenuCommand(MenuSelect(event.where));
				END;
				inSysWindow:				{let the system handle the mouseDown}
					SystemClick(event, window);
				inContent:
					IF window <> FrontWindow THEN BEGIN
						SelectWindow(window);
						{DoEvent(event);}	{use this line for "do first click"}
					END ELSE
						DoContentClick(window, event);
				inDrag:						{pass screenBits.bounds to get all gDevices}
					DragWindow(window, event.where, screenBits.bounds);
				inGrow:;
				inZoomIn, inZoomOut:;
			END;
		END;
		keyDown, autoKey: BEGIN				{check for menukey equivalents}
			key := CHR(BAnd(event.message, charCodeMask));
			IF BAnd(event.modifiers, cmdKey) <> 0 THEN	{Command key down}
				IF event.what = keyDown THEN BEGIN
					AdjustMenus;			{enable/disable/check menu items properly}
					DoMenuCommand(MenuKey(key));
				END;
		END;                                {call DoActivate with the window and...}
		activateEvt:                        {TRUE for activate, FALSE for deactivate}
			DoActivate(WindowPtr(event.message), BAnd(event.modifiers, activeFlag) <> 0);
		updateEvt:                          {call DoUpdate with the window to update}
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
 Get the events by calling WaitNextEvent, if it's available, otherwise
 by calling GetNextEvent. Also call AdjustCursor each time through the loop.}

VAR
	cursorRgn	: RgnHandle;
	gotEvent	: BOOLEAN;
	event		: EventRecord;
	mouse		: Point;

BEGIN
	cursorRgn := NewRgn;				{we’ll pass WNE an empty region the 1st time thru}
	REPEAT
		IF gHasWaitNextEvent THEN BEGIN	{put us 'asleep' forever under MultiFinder}
			GetGlobalMouse(mouse);		{since we might go to sleep}
			AdjustCursor(mouse, cursorRgn);
			gotEvent := WaitNextEvent(everyEvent, event, MAXLONGINT, cursorRgn);
		END ELSE BEGIN
			SystemTask;					{must be called if using GetNextEvent}
			gotEvent := GetNextEvent(everyEvent, event);
		END;
		IF gotEvent THEN BEGIN
			AdjustCursor(event.where, cursorRgn);	{make sure we have the right cursor}
			DoEvent(event);
		END;
		{If you are using modeless dialogs that have editText items,
		 you will want to call IsDialogEvent to give the caret a chance
		 to blink, even if WNE/GNE returned FALSE. However, check FrontWindow
		 for a non-NIL value before calling IsDialogEvent.}
	UNTIL FALSE;						{loop forever; we quit through an ExitToShell}
END; {EventLoop}


PROCEDURE _DataInit; EXTERNAL;

{This routine is part of the MPW runtime library. This external
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
