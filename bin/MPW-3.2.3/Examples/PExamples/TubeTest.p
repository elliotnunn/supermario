{------------------------------------------------------------------------------
#
#	Macintosh Developer Technical Support
#
#	Simple Color QuickDraw Animation Sample Application
#
#	TubeTest
#
#	TubeTest.p	-	Pascal Source
#
#	Copyright © Apple Computer, Inc. 1988-1990
#	All rights reserved.
#
#	Versions:	1.1	(MPW 3.0 Final)	8/88
#
#	Components:	TubeTest.p			Feb.   1, 1990
#				TubeTest.r			Feb.   1, 1990
#				TubeTest.make		Feb.   1, 1990
#
#	The TubeTest program is a simple demonstration of how to use the Palette 
#	Manager in a color program.  It has a special color palette that is associated
#	with the main window.  The colors are animated using the Palette Manager 
#	to give a flowing tube effect.  The program is very simple, and the Palette
#	Manager and drawing parts are put in separate subroutines to make it easier
#	to figure out what is happening.
#	
#	The program is still a complete Macintosh application with a Main Event Loop,
#	so there is the extra code to run the MEL.  
#	
#	There is a resource file that is necessary as well, to define the Menus, Window,
#	Dialog, and Palette resources used in the program.  
#
#	See Sample and TESample for the general structure and MultiFinder techniques that
#	we recommend that you use when building a new application.
#
------------------------------------------------------------------------------}

PROGRAM TubeTest;
 (*
	File TubeTest.p

	Version 1.0: 6/2/88
	
	4/19/88:
	TubeTest -- A small sample application written by Bo3b Johnson.
	The idea is to draw two circles in varying colors in the window, then
	animate the colors when the menu is chosen.	This is a complete program
	with event loop and everything.	It is intended to be a simple example of
	how to use the Palette Manager to do some minor color animation, and
	how to use the PM to set up the colors desired in a window.
	
	Also see the resource file that goes with this to see how the Palette 
	itself is layed out.
	
	Could be built with something like this:
		rez TubeTest.r -o TubeTest
		pascal TubeTest.p
		Link TubeTest.p.o ∂
			"{Libraries}"Interface.o ∂
			"{Libraries}"Runtime.o ∂
			"{PLibraries}"Paslib.o ∂
			-o TubeTest
		TubeTest
 *)
 
 {Where does it fit:
	This is a series of sample programs for those doing development
	using Color QuickDraw.  Since the whole color problem depends
	upon the exact effect desired, there are a number of answers
	to how to use colors, from the simple to the radically complex.
	These programs try to cover the gamut, so you should use 
	which ever seems appropriate.  In most cases, use the simplest
	one that will give the desired results.  The compatibility
	rating is from 0..9 where low is better.  The more known risks 
	there are the higher the rating.
	
	
	The programs (in order of compatibility):
	
		SillyBalls:
			This is the simplest use of Color QuickDraw, and does
			not use the Palette Manager.  It draws randomly colored
			balls in a color window.  This is intended to give you
			the absolute minimum required to get color on the screen.
			Written in straight Pascal code.
			Compatibility rating = 0, no known risks.
		
		FracAppPalette:
			This is a version of FracApp that uses only the Palette
			Manager.  It does not support color table animation
			since that part of the Palette Manager is not sufficient.
			The program demonstrates a full color palette that is
			used to display the Mandelbrot set.  It uses an offscreen
			gDevice w/ Port to handle the data, using CopyBits to
			draw to the window.  The Palette is automatically 
			associated with each window.  The PICT files are read
			and written using the bottlenecks, to save on memory
			useage.
			Written in MacApp Object Pascal code.
			Compatibility rating = 0, no known risks.
		
		TubeTest:	(***)
			This is a small demo program that demonstrates using the
			Palette Manager for color table animation.  It uses a 
			color palette with animating entries, and draws using the
			Palette Manager.  There are two circles of animating colors
			which gives a flowing tube effect.  This is a valid case
			for using the animating colors aspect of the Palette Manager,
			since the image is being drawn directly.
			Written in straight Pascal code.
			Compatibility rating = 0, no known risks.
		
		FracApp:
			This is the ‘commercial quality’ version of FracApp.  This
			version supports color table animation, using an offscreen
			gDevice w/ Port, and handles multiple documents.  The
			CopyBits updates to the screen are as fast as possible.  The
			program does not use the Palette Manager, except to
			provide for the system palette, or color modes with less than
			255 colors.  For color table animation using an offscreen
			gDevice w/ Port, it uses the Color Manager and handles the
			colors itself.  Strict compatibility was relaxed to allow for
			a higher performance program.  This is the most ‘real’ of the
			sample programs.
			Written in MacApp Object Pascal code.
			Compatibility rating = 2.  (nothing will break, but it may not
			always look correct.)
		
		FracApp300:
			This doesn't support colors, but demonstrates how to create and
			use a 300 dpi bitmap w/ Port.  The bitmap is printed at full
			resolution on LaserWriters, and clipped on other printers (but
			they still print).  It demonstrates how to use a high resolution
			image as a PICT file, and how to print them out.
			Written in MacApp Object Pascal code.
			Compatibility rating = 1.  (The use of PrGeneral is slightly 
			out of the ordinary, although supported.)
}

 {$R-}	{ No range checking.}
 {$D+}	{ Debugging labels on. }
 
 
	{ Interface files with all the happy Macintosh stuff in them. }
USES 
	MemTypes, QuickDraw, OsIntf, ToolIntf, PaletteMgr;
	
	
CONST
	appleID					= 1000;		{ resource IDs/menu IDs for Apple, File and Edit menus }
	fileID					= 1001;
	editID					= 1002;
	
	appleM					= 1;		{ Index for each menu in myMenus (array of menu handles) }
	fileM					= 2;
	editM					= 3;

	menuCount				= 3;		{ Total number of menus }

	windowID				= 1000;		{ Resource ID for main window }
	aboutMeDLOG 			= 1000;		{ And Resource ID for About box dialog. }

	tubularItem				= 1;		{ When checked, animation of colors. }
	quitItem				= 3;		{ Quit in the menu of course. }

	aboutMeCommand			= 1;		{ Menu item in apple menu for About TubeTest item }
	
	totalColors				= 152;		{ use 150 colors in our palette for drawing eyes. }
	numColors				= 150;		{ to skip black and white. }
	
	
 
VAR
	myMenus:		ARRAY [1..menuCount] OF MenuHandle;
	dragRect:		Rect;				{ Rectangle used to mark bounds for dragging window }
	doneFlag:		BOOLEAN;			{ TRUE if user has chosen Quit command }
	myEvent:		EventRecord;
	myWindow:		WindowPtr;
	whichWindow:	WindowPtr;
	tubeCheck:		Boolean;			{ if true, the menu is checked, and we animate. }
	theChar:		Char;
	error: 			OSErr;
	theWorld: 		SysEnvRec;
	
	{ This routine will update the window when required by update events.	It
		will draw two circular dudes that are indexed in colors through the colors
		we are using. 0 and 1 are skipped, since those are white and black in the
		palette. }
PROCEDURE DrawEyes;
	
	VAR		TempRect:	Rect;
			I:			Integer;
	
BEGIN
	SetRect(TempRect, numColors, numColors, numColors, numColors);
	For I := 2 To totalColors Do Begin
		PmForeColor(I);
		FrameOval (TempRect);
		InsetRect (TempRect, -1, -1);
	END;
	
	SetRect(TempRect, numColors*3, numColors, numColors*3, numColors);
	For I := totalColors DownTo 2 Do Begin
		PmForeColor(I);
		FrameOval (TempRect);
		InsetRect (TempRect, -1, -1);
	END;
END;	{ DrawEyes }


PROCEDURE SetUpMenus;

	VAR		I:	INTEGER;

BEGIN
	{ Read menu descriptions from resource file into memory and store handles
	in myMenus array }
	myMenus[appleM] := GetMenu(appleID);	{read Apple menu from resource file}
	AddResMenu(myMenus[appleM], 'DRVR');	{add desk accessory names to Apple menu}
	myMenus[fileM] := GetMenu(fileID);		{read file menu from resource file}
	myMenus[editM] := GetMenu(editID);		{read edit menu from resource file}

	FOR I := 1 TO menuCount DO InsertMenu(myMenus[I], 0); {install menus in menu bar}
	
	DrawMenuBar;							{and draw menu bar}
END; { SetUpMenus }


	{ Use the Palette currently attached to the main window to animate the colors 
	in the circular eye shapes.  This will rotate them around to give the flowing 
	tube effect. We make the palette into a color table so we can move entries 
	around.	We have to skip the first two entries since those are black and white. 
	(entries 0 and 1) }
PROCEDURE ShiftyColors;
	
	VAR		currPalette:	PaletteHandle;
			destCTab:		CTabHandle;
			lastCSpec:		ColorSpec;
	
BEGIN
	SetPort (myWindow);
	
	currPalette := GetPalette(myWindow);
	destCTab := CTabHandle (NewHandle (SIZEOF (ColorTable)+(totalColors*SIZEOF(ColorSpec))));
	IF destCTab = NIL  THEN Exit (ShiftyColors);
	Palette2CTab(currPalette, destCTab);
	
		{ Move the colors around in the color table, skipping 0 and 1, and moving
		all the elements down by one, and copying the element at 2 back to the 
		end of the table. The effect is to rotate the colors in the table.	}
	lastCSpec := destCTab^^.ctTable[2];							{ pull first one off. }
	BlockMove (@destCTab^^.ctTable[3], @destCTab^^.ctTable[2], 
				(numColors) * SIZEOF (ColorSpec) );				{ copy all one entry down. }
	destCTab^^.ctTable[totalColors-1] := lastCSpec;				{ put last color back on front. }
		
	AnimatePalette(myWindow, destCTab, 2, 2, numColors);
	
	DisposHandle (Handle (destCTab));
END;	{ ShiftyColors }


	{ Display the dialog box in response to the 'About TubeTest' menu item. }
PROCEDURE ShowAboutMeDialog;

	VAR		theDialog:	DialogPtr;
			itemHit:	Integer;

BEGIN
	theDialog := GetNewDialog(aboutMeDLOG, NIL, WindowPtr( - 1));
	ModalDialog(NIL, itemHit);
	DisposDialog(theDialog);
END; { ShowAboutMeDialog }


	{ Execute menu command specified by mResult, the result of MenuSelect }
PROCEDURE DoCommand(mResult: LONGINT);

	VAR		theItem:	INTEGER;				{ menu item number from mResult low-order word }
			theMenu:	INTEGER;				{ menu number from mResult high-order word }
			name: 		Str255;					{ desk accessory name }
			temp: 		INTEGER;
			dummy:		Boolean;

BEGIN
	theItem := LoWord(mResult);					{ call Toolbox Utility routines to }
	theMenu := HiWord(mResult);					{ set menu item number and menu }

	CASE theMenu OF								{ case on menu ID }

		appleID:
			IF (theItem = aboutMeCommand) THEN  ShowAboutMeDialog
			ELSE
				BEGIN							{ call Menu Manager to get desk acc.}
					GetItem(myMenus[appleM], theItem, name);
					temp := OpenDeskAcc(name);
					SetPort(myWindow);
				END; { appleID }

		fileID:
			BEGIN
				IF theItem = quitItem THEN doneFlag := TRUE;
				
				IF theItem = tubularItem THEN BEGIN
					tubeCheck := NOT tubeCheck;
					CheckItem(myMenus[fileM], tubularItem, tubeCheck);
				END;
			END;	{ fileID }

		editID:
				dummy := SystemEdit(theItem - 1);{ Pass the command on to the Desk Manager.}

	END; {of menu CASE}

	HiliteMenu(0);						{ Unhighlight menu title(highlighted by MenuSelect) }
END; {of DoCommand}


BEGIN  { Main }
	{ Test the computer to be sure we can do color.  If not we would crash, which
	would be bad.  If we can’t run, just beep and exit. }
	error := SysEnvirons(1, theWorld);
	IF NOT theWorld.hasColorQD THEN BEGIN
		SysBeep (50);
		ExitToShell;				{ If no color QD, we must leave. }
	END;

	InitGraf(@thePort);
	InitFonts;
	InitWindows;
	InitMenus;
	TEInit;
	InitDialogs(NIL);
	InitCursor;

	WITH screenBits.bounds DO
		SetRect(dragRect, 4, 24, right - 4, bottom - 4);
	doneFlag := FALSE;				{ flag to detect when Quit command is chosen}
	tubeCheck := FALSE;				{ flag for animating color is initially off. }

		{ Open the color window. }
	myWindow := GetNewCWindow(windowID, NIL, POINTER(-1));
	SetPort(myWindow);

		{ Set up menus last, since the menu drawing can then use the palette we have for ou
		window. Makes the Apple look better, in particular. }
	SetUpMenus;
	
	{ Main Event Loop }
	REPEAT
		SystemTask;

		IF GetNextEvent(everyEvent, myEvent) THEN
			CASE myEvent.what OF			{ case on event type}

				mouseDown:
					CASE FindWindow(myEvent.where, whichWindow) OF

						inSysWindow:				{ desk accessory window: call Desk Manager to handle it}
								SystemClick(myEvent, whichWindow);

						inMenuBar:					{ Menu bar: learn which command, then execute it. }
								DoCommand(MenuSelect(myEvent.where));

						inDrag:						{ title bar: call Window Manager to drag}
								DragWindow(whichWindow, myEvent.where, dragRect);

						inContent:					{ body of application window: }
								IF whichWindow <> frontWindow THEN
									SelectWindow(whichWindow); {and make it active if not}
					END; {of mouseDown}

				updateEvt:							{ Update the eyes in window. }
					IF WindowPtr(myEvent.message) = myWindow THEN
						BEGIN
							BeginUpdate(WindowPtr(myEvent.message));
								DrawEyes;
							EndUpdate(WindowPtr(myEvent.message));
						END; {of updateEvt}
							
				keyDown, autoKey:					{ key pressed once or held down to repeat }
					IF myWindow = frontWindow THEN
						BEGIN
							theChar := CHR(BAnd(myEvent.message, charCodeMask)); { get the char }
								{ If Command key down, do it as a Menu Command. }
							IF BAnd(myEvent.modifiers, cmdKey) <> 0 THEN DoCommand(MenuKey(theChar));
						END; {of keyDown and autoKey}

			END; {of event CASE}

			{ If we have menu item checked, go ahead and animate colors. }
		IF tubeCheck THEN ShiftyColors;
		
	UNTIL doneFlag;

		{ clean up after palette manager, so he can chuck the palette in use. }
	DisposeWindow (myWindow);
	
END.
