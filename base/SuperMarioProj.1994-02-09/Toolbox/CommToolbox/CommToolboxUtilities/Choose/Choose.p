{
	File:		Choose.p

	Contains:	Puts up the standard "Settings" dialog for Communication Tools

	Written by:	Byron Han

	Copyright:	© 1988-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 8/12/93	kc		<mb>: I deleted all of the "TheFuture" conditionals in all of
									the CommToolbox files per Mike Bell's instructions. I also had
									to delete some old code that was compiled under the "CubeE"
									conditional.
		<28>	 11/7/92	JDR		Removed the PopupPrivateData record, which is now a public
									structure.  Change occurances of the usage too.
		<27>	  6/1/92	DTY		#1031085: Do a quick checkout/checkin to get some release notes
									for change <25>. (See Bonnie, I told you I write release notes!)
		<26>	 5/29/92	BH		Problem with dates (don't have any - ha ha)
		<25>	 5/29/92	BH		Fixed problem with getting  menu handle resource BRC #1031085
									(BH,AMR)
		<24>	 10/4/91	JSM		Change PsychoticFarmerOrLater conditionals to TheFuture.
		<23>	 10/2/91	DTY		Conditionalise <20> through <22> out of CubeE.
		<22>	 6/18/91	BH		made special always true, merged old modification history into
									current one with appropriate chronological order
		<21>	 6/17/91	BH		change chooseNoChange to ctbChooseNoChangeHandle
									change ctbBackdoor to ctbChooseNoDialog
									added ctbChooseNoChangeTool flag
									always delete items from popup before building it to avoid
									trailing X's after a crash
									added backdoor and filtering to forPost70
									always do a reserverf and releaserf when about to call the
									application filterproc
		<20>	 2/28/91	kaz		jng, DTG-068: Adding cursor tracking for I-Beams over edit text
									fields.
		<19>	  2/8/91	kaz		jng, #82583: fixed MacPad compatibility bug: CTBSetupXCleanup
									was always passing FALSE for whether the OK button was hit --
									modded ClearDITL() to take that Boolean parameter.
		<18>	  2/4/91	kaz		jng, #82081: Fixing event filter proc to handle update events
									for windows behind the Choose dialog.
		<17>	 12/6/90	JNG		ARGH!!! I Hate aliases. Revalidate the procID after ClearDITL
									<kaz>
		<16>	 12/4/90	JNG		Fix bug in DoPopup that closes the new tool instead of the
									oldtool. <kaz>
		<15>	10/29/90	ngk		Updated to new way StdFilterProc works
		<14>	 9/10/90	JNG		Put in alias resolution/display
		<13>	 8/15/90	kaz		Uses StdFilterProc() where possible;  Cut/Copy/Paste for Sys6;
									Centers ShutDown alert; Checks for NIL in _DisposPtr;  Compiled
									out _xxPChoose stuff;  Mega shrinkage
		<12>	  8/6/90	kaz		Changing GoodDrawOutlineButton() to DoSolidOutline().
		<11>	  7/1/90	kaz		DoNewConn() was locking the wrong handle.
		<10>	 5/24/90	kaz		Calling utility routine Getflst()
		 <9>	 5/22/90	kaz		Updating to use TextState rec for finf
		 <8>	 5/15/90	DC		Made "Point(0)" Coercions 3.1+ compatible
		<6+>	 4/12/90	kaz		Major cleanup
									- Broke ChooseEntry into multiple procedures
									- Removed Debugging conditions
									- Removed code to check for option-click on popup
									- Uses utility routines in CommToolboxUtilityRoutines.c
									- Uses CmdPeriod() from TechNotes
									- Cleaned up DoNewTerm(), etc
									- Cleaned up drawing of dynamic items
		 <6>	 4/11/90	BBH		removed diddling with ClipRgn directly - now use
									HidePen/ShowPen; also if preflight returns NIL for item list
									handle, this is considered an abort condition
		 <5>	 3/21/90	EMT		Added AppleTalk USES for Str32 in CommToolboxPriv.p.
		 <4>	 3/18/90	BBH		remove unneeded memory checks
		 <3>	 3/16/90	BBH		urned debugging off
		 <2>	 3/16/90	BBH		make it compile please
		 <1>	 3/14/90	BBH		first checked in

	Old Modifications:
		2/6/90		KAZ		fix build for specialk
		2/2/90		kaz		Set oldSize.h to GetMinWidth() before every _AppendDITL.
								_AppendDITL looks at the size of the original window
								not the items in it when it determines the new size.
								So we set the original size to the min.
		2/1/90		kaz		Made the tool popup a resCtrl DITL item and removed the
								GetNewControl.
							Changed HideDynamicItems() to use a minimum threshhold
								instead of _HideDItem.  The minimum is set to whatever
								the width of the tool menu is + space for the OK button
		1/31/90		kaz		Added HideDynamicItems() and ShowDynamicItems() that move
								the OK, Cancel, Outline, and Solid Line Items around
								depending on the window position.  This means that
								the dialog will shrink to fit the tool, then add the
								dynamic items in at the right places.

		11/7/89		BBH		Fixed problem with not disposing of tempConfig
		10/30/89	BBH		Revamped update event processing
							When snarfing update events, we keep track of updateRgn's
								in global coordinates and then use PaintBehind to
								generate all the requisite update events

							Took out old maxExtents code which kept track of the largest
								rectangle that the choose dialog grew to...

							(old code is in BADDOGGY)
		9/17/89		BBH		BufferSizes renamed CMBufferSizes
		9/15/89		BBH		Moved MiniModalDialog filter proc to SpecialUnit.p
		9/14/89		BBH		Fixed location of reallyshutdown dialog.  also added beep to it.
							Also added filterProc to handle command-. and escape.

							Added Delay to hilighing of OK/Cancel buttons in main dialog.
		9/5/89		BBH		Now set control minimum to 1 explicitly to keep Shimmer happy.
		8/29/89		JNG		Esc now equal to Cmd '.'
							Fixed final update bug created with my first attempted fix.  I was
								getting the strucRgn before the dialog was shown.  Unfortunately,
								the strucRgn isn't setup yet.  So, we now get the original size
								after the ShowWindow.
							Removed oldName - use tempString where needed, and use popup ctl
								value to keep track of the original tool
		8/25/89		JNG		Changed AND and OR to shortcircuit & and |
							We only get the version info if the user option-clicks the popup
		8/24/89		JNG		Removed 8/32/89 change by BBH
		8/23/89		BBH		In OKMajor, we copy new config into oldConfig as well as config.
		8/18/89		JNG		When we are accumulating the maximum size of the dialog (so we can
							force update events), we were unioning the portRects.  But this
							doesn't include the window frame. So, now we union the
							WindowPeek(theDialog)^.strucRgn^^.rgnBBox, to the maximum
							extent of the dialog.
		8/17/89		JNG		Removed handling of return/enter/cmd '.' before calling the tools
							   filterproc
							Put in error check if CTBDefault fails when user changes tool.
							No longer save MLU in newConn and newTerm
		7/28/89		BBH		Eliminated extra redraw of the window frame.  AppendDITL does its
								own SizeWindow so the explicit SizeWindow in our code can
								be removed.
		7/27/89		BBH		Eliminate MyDebugStr.
							Try to speed up the goddamn thing.  Succeeded a little bit by
								immediately doing explicit erases after shortening.  Also,
								after appending, invalidate the entire bottom portion of
								the dialog.  This way, redraw is faster since we have a
								much less complex region (one big rectangle instead of lots
								of disjoint little rectangles)

							Also added arrow and watch cursors.

							The compiler switches Cursing and SpeedyBeeDeeBeeDee control these
								options.  Set them to TRUE for proper behaviour.
		7/20/89		BBH		Try to use the toolname passed in chooseRec if theHandle is NIL.
							If cannot find, then use first tool available.

							Also, added extra debugging calls to CRMDumpResChain.
		7/19/89		BBH		The very first GetVersResource will fail since we are not
								yet stuffed into the resource chain.  So bracket first
								call to GetVersResource with CRMToolPreflight/CRMToolPostflight
		7/12/89		BBH		Idle proc was only called when items were hit.  Now
								idleProc is always called. Also call eventProc in
								response to idles, activates, and updates on windows
								other than dialog window.
		6/30/89		BBH		Changed to use CTBSetupXCleanup
		6/21/89		BBH		Major rework.
								Common exit code.
								Managing case of theHandle = NIL
								Handling chooseNochange message - return toolName and config in
									choose record.
								CTBChoose() now creates the chooseRec which is passed in.
								Manage use of the filter procedure, newID, and newConfig.
		6/15/89		JNG		Fixed JED-290.  Renaming a tool no longer crashes choose.  We were
							getting a new procID after the user had selected OK or CANCEL, but
							we already had this procID sitting around.
							Somewhat related, when a user changes tools from the popup, we check
							to see if they are going back to the original tool.  If so, we don't call
							GetProcID, but rather just pull the procID back from the original
							handle.
		6/12/89		BBH		If ditl to append is NIL then hide the dotted line otherwise show it
		6/2/89		JNG		New CTB core interface
		5/19/89		BBH		Changed hardcode constants for 1 and 2 to ChooseItemOK and ChooseItemCancel
		 		 	JNG		Changed CMStatus info to be sets.  Changed Data types to reflect new interfaces.
		5/17/89		BBH		Problem with Choose if DoNewConn was aborted that we would still
							do a blockmove of the new config stuff over the old config in the
							conn handle.
		5/4/89		kaz		Moved GetIndfinf() to SpecialUnit.p
		5/2/89		BBH		Changed ID from 9990 to -3991
		4/28/89		BBH		Changed FTNewConn to use new FTNew routine
		4/27/89		JNG		Strangeness due to fluid intstallation of tools.  If the user moves a
							tool that they are using from the communications folder, we were having
							problems.  So, now we have to special case this.  The tool is added to the
							pop-up after all of the other tools are added.  It is then removed if the user
							selects another tool.  Cancel still works because the tool isn't closed
							until after the user selects ok/cancel.

					BBH		Got more BitAnds
		4/26/89		BBH		Changed dotted line at bottom of choose dialog to solid
							When changing dialog font/size info, need to stuff lineHeight,
								and fontAscent.
							Stop calling CMGetProcID - use CTBGetProcID.

							In ChooseFilter, we need to preprocess Return, Enter, or Command-. before
								calling CTBFilter...
		4/24/89		BBH		In DoNewTerm/DoNewConn/DoNewFT, save the reserved and reserved2 fields.
							Actually, also save environsProc for Term and FT.
							If cannot create new procedure , return ChooseDisaster.

							DoNewConn also checks to see if the connection is still open and attempts
							to confirm closing the connection down.

							Change BitAnds to BANDs

							Added outline button to confirmation dialog
		4/19/89		BBH		Fixed bug in that we were using theHandle as a corehandle but were
							still using hTerm in places.  so, removed hConn, hFT local
							vars.  assign hTerm properly.  this is especially true when
							validating after selecting the appropriate tool.

							Fixed sanity checking bug when looking at msg.
		4/17/89		BBH		Use CTBPrivate.
							Added some error handling.
							Changed drawing of version string to use 'vers' 1 resource from the tool
							file.

							eliminate tempString local variable saving 256 bytes.

							Start using CTBCore to save space.

							Fixed up memory management to conserve.  Still need to implement
								ChooseNochange backdoor.
		4/13/89		BBH		Take out case sensitivity when checking names of tools.  Changed FindMenuItem
							in SpecialUnit.p and check at end of main routine where oldName <> theName
							to use the EqualString routine, case insensitive, diacrit sensitive.
		4/12/89		BBH		Moved disposal and cleanup of the dialog to be BEFORE we call DoNewConn/Term/FT
							to cut down on peak memory usage.

							••••• NEED TO ADD ERROR CHECKING GALORE •••••

							Added call to TECalText in setting up dialog font/size.
		4/9/89		BBH		When adding tools to the menu, we were calling CRMGetIndFile.  This routine
							returns an error condition if the requested file cannot be found so.  There is
							no need to abort Choose if the file cannot be found.  Also added check for no
							tools found after building tool menu.  No tools will cause abort with
							return value 0.
		4/7/89		JNG		Changed CMGetConnName to CMGetToolName
							Changed TMGetTermName to TMGetToolName
							Changed FTGetName to FTGetToolName

	To Do:
		• Low mem problems for preflighting
		• Why can't a tool's setup call xxSetup(): Stattenfield
		• Make DrawLine() a PICT
		7/27/89		BBH		Arrow and watch cursors where appropriate
							SPEED THIS GODDAMN THING UP
		6/23/89		BBH		What happens if the user has published their boot volume over
								Killer Rabbit or Tops and another user renames a tool
								while the dialog is up?????
}


UNIT Choose;
INTERFACE

USES
	MemTypes, QuickDraw, OSIntf, ToolIntf, PackIntf, AppleTalk,
	SysEqu, Files, Aliases,
	CTBUtilities,
	Terminals,
	CommResources,
	Connections,
	FileTransfers,
	Dialogs,
	Controls,

	{ private CommToolbox routines }
	CommToolboxPriv;

FUNCTION ChooseEntry(msg:INTEGER; VAR theHandle:CoreHandle; where : Point; VAR cRec: ChooseRec) : INTEGER;

IMPLEMENTATION

CONST
	ChooseItemOK = 1;
	ChooseItemCancel = 2;
	ChooseItemOutline = 3;
	ChooseItemTitle = 4;
	ChooseItemVersion = 5;
	ChooseItemDottedLine = 6;
	ChooseItemPopup = 7;

	OUTLINEWIDTH	= 4;		{ in pixels	}
	ITEMSLOP		= 5;

	fAlias			= 32768;
	validScript		= 128;
	ScriptMask		= -129;

TYPE
	dialogInfoP = ^dialogInfo;		{ storate private to the configuration dialog }
	dialogInfo = RECORD
		tempProcID 	: INTEGER;	{ this MUST be the first item in the record for all dialogs }
		magicCookie : LONGINT;	{ this MUST be the second item in the record for all dialogs }
		msg 		: INTEGER;	{ the rest of the data structure is private to the particular }
		theType		: ResType;	{ toolClass	}
		tempConfig 	: Ptr;		{ configuration dialog being used (standard or custom) }
		count 		: INTEGER;
		title		: STR255;	{ tool being displayed }
		updateProc	: ProcPtr;	{ if NIL, we need to save updates... }
		idleProc	: ProcPtr;
		myWatch		: Cursor;
		toUpdate	: RgnHandle;
		hMenu		: MenuHandle;		{ For tool list popup menu		}
		theControl	: ControlHandle;
		oldOutline	: Rect;				{ saved rect for OK outline for later erasure		}
		chDialog	: DialogPtr;
	END;

{	forward declaration }

FUNCTION 	DoBackDoor(msg: INTEGER; VAR theHandle: CoreHandle;
												VAR cRec: ChooseRec): INTEGER; FORWARD;
FUNCTION 	DoAppFilter(tempTool: Ptr; msg: INTEGER;theHandle: CoreHandle;
												filterProc: ProcPtr): BOOLEAN; FORWARD;
FUNCTION 	CallFilter(VAR theHandle: CoreHandle; toolName: STR255;
									toolID: INTEGER; theProc: ProcPtr): BOOLEAN; EXTERNAL;

FUNCTION	GetCurrentTool(infoP: DialogInfoP;theHandle: CoreHandle;cRec: ChooseRec;
												VAR theConfig: Ptr): INTEGER; FORWARD;
FUNCTION 	DoInit(infoP: dialogInfoP; theMsg: INTEGER;
								theDialog: DialogPtr; cRec: ChooseRec): BOOLEAN; FORWARD;
PROCEDURE 	BuildPopup(infoP: DialogInfoP;theHandle: CoreHandle;cRec: ChooseRec;
										toolName: Str255;VAR oldItem: INTEGER);FORWARD;
PROCEDURE 	ClearDITL(infoP: DialogInfoP; done: BOOLEAN; OKed: BOOLEAN; oldSize: Point); FORWARD;
FUNCTION 	ShowDITL(infoP: DialogInfoP; oldSize: Point): BOOLEAN; FORWARD;
FUNCTION 	DoPopup(infoP: DialogInfoP;isOriginalTool: BOOLEAN;
						firstProcID: INTEGER; oldItem: INTEGER;
								VAR oldVal: INTEGER; VAR oldSize: Point): BOOLEAN; FORWARD;
FUNCTION 	ChangeTools(infoP: DialogInfoP;VAR theHandle: CoreHandle;
							tempString: Str255): INTEGER; FORWARD;

FUNCTION 	DoNewConn(VAR hConn:ConnHandle; oldTool:ConnHandle; tempProcID:INTEGER; quiet: BOOLEAN; theDialog: DialogPtr): INTEGER; FORWARD;
FUNCTION 	DoNewTerm(VAR hTerm:TermHandle; oldTool:TermHandle; tempProcID:INTEGER): INTEGER; FORWARD;
FUNCTION 	DoNewFT(VAR hFT:FTHandle; oldTool:FTHandle; tempProcID:INTEGER): INTEGER; FORWARD;

PROCEDURE 	DrawLine(theDialog : DialogPtr; itemNo : INTEGER); FORWARD;
PROCEDURE 	DrawTitle(theDialog : DialogPtr; itemNo : INTEGER); FORWARD;

PROCEDURE 	SetDTextInfo(theDialog : DialogPtr; procID: INTEGER); FORWARD;

PROCEDURE 	CallIdle(idleProc:ProcPtr); EXTERNAL;
PROCEDURE 	CallEventProc(VAR theEvent: EventRecord; updateProc: ProcPtr); EXTERNAL;

PROCEDURE 	HideDynamicItems(theDialog: DialogPtr); FORWARD;
PROCEDURE 	ShowDynamicItems(theDialog: DialogPtr); FORWARD;
FUNCTION	GetMinWidth(theDialog: DialogPtr): INTEGER; FORWARD;

PROCEDURE 	AdjustControl(theDialog: DialogPtr; theItem: INTEGER; hiliteVal: INTEGER); FORWARD;
FUNCTION 	ChooseFilter(theDialog : DialogPtr; VAR theEvent:EventRecord;
					VAR theItem:INTEGER) : BOOLEAN; FORWARD;
PROCEDURE UnavailableTool(theParent: DialogPtr); FORWARD;


{===================================================================================================}
{
	This is the main entry point to the choo resource.  The choose resource is the standard
	selection dialog for selection and configuration of a communication tool.

	msg selects which manager is calling the choo resource.
	theHandle is a (var) parameter which is the connection/terminal/file transfer handle.
	where is the upper left hand corner of the selection dialog.
	idleProc is a procedure pointer (with no parameters) that is to be called from the
		dialog idle loop.
}
FUNCTION 	ChooseEntry(msg:INTEGER; VAR theHandle: CoreHandle;
				where: Point; VAR cRec: ChooseRec): INTEGER;

LABEL
	1;

VAR
	savedPort	:	GrafPtr;		{ saved port }
	theWindow	:	WindowPtr;		{ used to set up invalid regions after dialog disappears }
	theDialog	:	DialogPtr;		{ the choose dialog }
	tempString	:	STR255;			{ temporary placeholder }
	theItem		:	INTEGER;		{ for manipulating dialog items }
	itemKind	:	INTEGER;
	itemHandle	:	Handle;
	itemRect	:	Rect;
	oldSize		:	Point;			{ old size of choose dialog before resizing }
	thePtr		:	Ptr;			{ pointer to temporary configuration record }
	oldVal		:	INTEGER;		{ old control (popup menu) value }
	newVal		:	INTEGER;		{ current control value }
	oldItem		:	integer;		{ original popup item value - i.e. original tool }
	configSize	:	LONGINT;		{ size of the configuration record }
	infoP		:	dialogInfoP;	{ pointer to dialog data }
	hDITL		:	Handle;			{ handle to DITL to append }
	tempPort	:	GrafPtr;
	firstProcID	: 	INTEGER;			{ first procID of the first tool }
	includeIt	: 	BOOLEAN;			{ do we include tool in the menu? }
	savedRes	:	INTEGER;		{ saved ResFile reference  <AMR 25> }

BEGIN
	InitCursor;

	ChooseEntry := ChooseFailed;						{ pessimistic happening }
	IF (msg <> ChooseCM) &  (msg <> ChooseTM) & (msg <> ChooseFT) THEN		{ are we out of range? }
		Exit(ChooseEntry);								{ sanity chex }

	{
		ctbChooseNoChangelHandle means bring up dialog to select tool or configure tool,
			but only return the toolName and the new configPtr (i.e. do not dispose of
			old handle or create new one

		ctbChooseNoDialog means change to the toolName and config sent in without bringing
			up the dialog

		ctbChooseNoChangleTool means popup only contains current tool (cannot change tool)
	}

	IF theHandle = NIL THEN
		cRec.msg := BOR(cRec.msg, ctbChooseNoChangeHandle);		{ app needs to allocate records}

	IF (BAND(cRec.msg, ctbChooseNoDialog) = ctbChooseNoDialog) THEN			{ check for special case }
	BEGIN
		ChooseEntry := DoBackDoor(msg, theHandle, cRec);
		Exit(ChooseEntry);
	END;

	savedRes := CurResFile;								{ avoid application resources <AMR 25> }
	UseResFile(0);										{ <AMR 25> }
														{ preflight values }
	savedPort := NIL;									{ mark these variables as uninitialized }
	theDialog := NIL;									{ i.e. unallocated so that errorexit will }
	infoP := NIL;

	theDialog := GetNewDialog(CTB_ChooseDLOGID, NIL, POINTER(-1));
	IF theDialog = NIL THEN								{ perform safe dialog }
		GOTO 1;

	GetPort(savedPort);									{ always the boyscout }
	SetPort(theDialog);									{ at 'em boy }

	infoP := dialogInfoP(NewPtrClear(SIZEOF(dialogInfo)));	{ allocate internal data space }
	IF infoP = NIL THEN									{ no memory }
		GOTO 1;

	SetWRefCon(theDialog, LONGINT(infoP));				{ set the dialog refcon to infoP }

	IF NOT DoInit(infoP, msg, theDialog, cRec) THEN			{ Initialize the struct	}
		GOTO 1;

	SetUserItemProc(theDialog, ChooseItemTitle,@DrawTitle);
	SetUserItemProc(theDialog, ChooseItemDottedLine,@DrawLine);
	SetUserItemProc(theDialog, ChooseItemOutLine,@DoSolidOutline);

	{ we need to tell the StdFilterProc to handle the default and cancel buttons }
	IF SetDialogDefaultItem(theDialog, ChooseItemOK) <> noErr THEN;
	IF SetDialogCancelItem(theDialog, ChooseItemCancel) <> noErr THEN;
	IF SetDialogTracksCursor(theDialog, true) <> noErr THEN;

	WITH infoP^ DO BEGIN
		SetCursor(myWatch);

		GetIndString(tempString, CTB_ChooseSTRsID, 4+msg);		{ title }
		SetCTitle(theControl, tempString);						{ get the appropriate title }

		{ Get the first tool's procID & name	}
		firstProcID := GetCurrentTool(infoP, theHandle, cRec, tempConfig);
		IF firstProcID = -1 THEN
			GOTO 1;

		tempProcID := firstProcID;
		CTBGetToolName(tempProcID, tempString, msg);

		{ Creates the popup of tools and returns the current tool in oldItem	}
		BuildPopup(infoP,theHandle,cRec,tempString,oldItem);

		oldVal := oldItem;										{ Save the original popup value }
		oldSize := theDialog^.portRect.botRight;				{ old size of the dialog }
		newVal := oldVal;

		IF NOT ShowDITL(infoP, oldSize) THEN					{ Put up the new DITL	}
			GOTO 1;

		MoveWindow(theDialog, where.h, where.v, TRUE);				{ move dialog to appropriate place }
		ShowWindow(theDialog);										{ unveiling… }

		InitCursor;

		theItem := 0;												{ for now… }
		WHILE (theItem <> ChooseItemOK) & (theItem <> ChooseItemCancel) DO BEGIN
			ModalDialog(@ChooseFilter, theItem);

			IF theItem = ChooseItemPopup THEN						{ did the popup get hit? }
				{ Handles tool choice.  Returns the tool value & size of the window	}
				IF NOT DoPopup(infoP,(theHandle = NIL), firstProcID,
														oldItem,oldVal,oldSize) THEN
					GOTO 1;

			IF theItem >= count THEN									{ item hit in the tool's part }
				CTBSetupItem(tempProcID, tempConfig, count, theDialog, theItem, magicCookie, msg);

		END; {while theItem NOT OK or cancel}

		newVal := GetCtlValue(theControl);
		GetItem(hMenu, newVal, tempString);

		HideWindow(theDialog);									{ hide the dialog }

		ClearDITL(infoP, TRUE, (theItem = ChooseItemOK), oldSize);	{ get rid of the DITL	}

		DisposDialog(theDialog);
		theDialog := NIL;
		SetPort(savedPort);

		{ If the user hit OK, we either have the same tool or a new one	}
		IF theItem = ChooseItemOK THEN BEGIN

			{ Special Filter stuff	}
			IF  (BAND(cRec.msg, ctbChooseNoChangeHandle) = ctbChooseNoChangeHandle) THEN BEGIN
				cRec.newTool := tempString;
				cRec.newConfig := tempConfig;
				tempConfig := NIL;
				if newVal <> oldItem then
					ChooseEntry := ChooseOKMajor
				else
					ChooseEntry := ChooseOKMinor;
			END {noChange}
			ELSE IF newVal <> oldItem THEN					{ new tool	}
				ChooseEntry := ChangeTools(infoP,theHandle,tempString)
			ELSE BEGIN
				ChooseEntry := ChooseOKMinor;				{ same tool	}
				configSize := GetPtrSize(tempConfig);
				BlockMove(tempConfig, theHandle^^.config, configSize);
				IF CTBValidate(theHandle, msg) THEN
					;
			END; { name has changed }

		END {user hit OK }
		ELSE
			ChooseEntry := ChooseCancel;

		SetCursor(myWatch);

		{ Update all the windows behind us that got erased by our dynamic sizing	}
1:
		{ App's not dealing w/ events	}
		IF (cRec.eventProc = NIL) THEN BEGIN
			theWindow := FrontWindow;
			IF theWindow <> NIL THEN
				IF infoP <> NIL THEN
					IF infoP^.toUpdate <> NIL THEN
						PaintBehind(WindowPeek(theWindow), infoP^.toUpdate);
		END;

		SetPort(savedPort);
	END;	 {with }

	IF infoP <> NIL THEN BEGIN
		WITH infoP^ DO BEGIN
			IF infoP^.toUpdate <> NIL THEN
				DisposeRgn(infoP^.toUpdate);
			IF (tempConfig <> NIL) THEN
				DisposPtr(tempConfig);					{ DisposPtr does not check for NIL }
		END;
		DisposPtr(Ptr(infoP));
	END;

	IF theDialog <> NIL THEN
		DisposDialog(theDialog);

	IF (savedPort <> NIL) THEN
		SetPort(savedPort);

	UseResFile(savedRes);								{ restore <AMR 25> }
	InitCursor;
END;



{ -----------------------------------------------------------------------
	DoInit		-	Fills in the our data struct

		infoP		-	The target
		theMsg		- 	What tool class
		theDialog	-	DialogPtr

		returns		-	true if successful
------------------------------------------------------------------------ }
FUNCTION DoInit(infoP: dialogInfoP; theMsg: INTEGER;
								theDialog: DialogPtr; cRec: ChooseRec): BOOLEAN;
VAR
	itemKind		: INTEGER;
	itemRect		: Rect;

BEGIN
	DoInit := TRUE;

	WITH infoP^ DO BEGIN
		msg := theMsg;
		GetIndString(infoP^.title, CTB_ChooseSTRsID, msg + 1);

		IF msg = ChooseCM THEN
			theType := ClassCM;			{ store for use elsewhere	}
		IF msg = ChooseTM THEN
			theType := ClassTM;
		IF msg = ChooseFT THEN
			theType := ClassFT;

		chDialog := theDialog;

		myWatch := CursHandle(GetCursor(watchCursor))^^;		{ get the watch }
		toUpdate := NewRgn;

		updateProc := cRec.eventProc;
		idleProc := cRec.idleProc;
		count := CountDITL(theDialog) + 1;

		GetDItem(theDialog, ChooseItemPopup, itemKind, Handle(theControl), itemRect);
		DoInit := (theControl <> NIL);

		hMenu := MenuHandle(PopupPrivateDataPtr(ControlHandle(theControl)^^.contrlData^)^.mHandle);	{ Get handle directly <AMR 25> }
	{	hMenu := GetMHandle(CTB_ChooseMenuID); <AMR 25> }
		DoInit := (hMenu <> NIL);
	END; {with}

END;



{ -----------------------------------------------------------------------
	ChangeTools	-	Changes from one tool to another after the
					user hits OK

		returns		-	Choose Error Condition

------------------------------------------------------------------------ }
FUNCTION ChangeTools(infoP: DialogInfoP;VAR theHandle: CoreHandle;
							tempString: Str255): INTEGER;
VAR
	rVal		: INTEGER;
	configSize	: LONGINT;
	savedPort	: GrafPtr;
	hTerm		: TermHandle;
	tempPort	: GrafPtr;
	tempTool	: Handle;

LABEL
	1;

BEGIN
	rVal := ChooseOKMajor;

	WITH infoP^ DO BEGIN
		tempProcID := CTBGetProcID(tempString, msg);

		{ Create a new tool	}
		IF tempProcID = -1 THEN BEGIN
			rVal := ChooseAborted;
			GOTO 1;
		END;

		{ duplicate the old tool handle	}
		tempTool := Handle(theHandle);
		IF (HandToHand(tempTool) <> noErr) THEN BEGIN
			rVal := ChooseAborted;
			GOTO 1;
		END;

		HLock(tempTool);

		{ Create a new tool 	}
		IF msg = ChooseCM THEN
			rVal := DoNewConn(ConnHandle(theHandle), ConnHandle(tempTool), tempProcID, FALSE, infoP^.chDialog);
		IF msg = ChooseTM THEN
			rVal := DoNewTerm(TermHandle(theHandle), TermHandle(tempTool), tempProcID);
		IF msg = ChooseFT THEN
			rval := DoNewFT(FTHandle(theHandle), FTHandle(tempTool), tempProcID);

		HUnlock(tempTool);
		DisposHandle(tempTool);

		{ If sucessful, validate it	}
		IF rval = ChooseOKMajor THEN BEGIN
			configSize := GetPtrSize(tempConfig);
			BlockMove(tempConfig, theHandle^^.config, configSize);

			{ Need to set the port before calling TMValidate	}
			{ Should be done in DoNewTerm						}
			IF msg = ChooseTM THEN BEGIN
				hTerm := TermHandle(theHandle);
				tempPort := hTerm^^.owner;
				GetPort(savedPort);
				SetPort(tempPort);
			END;

			IF CTBValidate(theHandle,msg) THEN
				;							{ validate }

			IF msg = ChooseTM THEN BEGIN
				InvalRect(tempPort^.portRect);	{ invalrect }
				SetPort(savedPort);				{ back to our regular station }
			END;

		END; {rval = ChooseOKMajor}

	END; { with	}

1:
	ChangeTools := rVal;
END;


{ -----------------------------------------------------------------------
	ClearDITL	-	Removes the tool's DITL

		infoP		-	our info
		done		-	true => done w/ dialog; false => changing tools

------------------------------------------------------------------------ }
PROCEDURE ClearDITL(infoP: DialogInfoP; done: BOOLEAN; OKed: BOOLEAN; oldSize: Point);
VAR
	itemRect	: Rect;

BEGIN
	WITH infoP^ DO BEGIN
		SetCursor(myWatch);

		IF (done) THEN
			HideWindow(chDialog);

		CTBSetupXCleanup(tempProcID, tempConfig, count, chDialog, OKed, magicCookie, msg);

		IF NOT done THEN
			WITH itemRect DO BEGIN
				botRight := chDialog^.portRect.botRight;
				left := 0;
				top := oldSize.v;
				EraseRect(itemRect);
			END;

		{ Prevents erasing of controls	}
		HidePen;
		ShortenDITL(chDialog, CountDITL(chDialog) - count + 1);
		ShowPen;

		CTBSetupPostflight(tempProcID, msg);			{ decrement usecount of tool 	}
	END;
END;


{ -----------------------------------------------------------------------
	ShowDITL		-	Preflights the new tool and shows the DITL

		infoP		-	our info

		returns		- 	True if successful, false if no DITL returned
------------------------------------------------------------------------ }
FUNCTION ShowDITL(infoP: DialogInfoP; oldSize: Point): BOOLEAN;
VAR
	hDITL		: Handle;

BEGIN
	ShowDITL := FALSE;

	WITH infoP^ DO BEGIN
		HideDynamicItems(chDialog);

		hDITL := CTBSetupPreflight(tempProcID, magicCookie, msg);
		IF (hDITL = NIL) THEN
			EXIT(ShowDITL);

		SetDTextInfo(chDialog,tempProcID);

		oldSize.h := GetMinWidth(chDialog);
		chDialog^.portRect.bottom := oldSize.v;		{ don't actually resize yet }
		chDialog^.portRect.right := oldSize.h;

		HidePen;
		AppendDITL(chDialog, hDITL, appendDITLBottom);
		ShowPen;

		ShowDynamicItems(chDialog);
		DisposHandle(hDITL);

		HidePen;
		CTBSetupSetup(tempProcID, tempConfig, count, chDialog, magicCookie, msg);
		ShowPen;

		ShowDITL := TRUE;
	END;
END;



{ -----------------------------------------------------------------------
	DoPopup	-	Handles the creation & diposing of the tools at the
				users request

		returns		-	True if successful

------------------------------------------------------------------------ }
FUNCTION DoPopup(infoP: DialogInfoP;isOriginalTool: BOOLEAN; firstProcID: INTEGER;
					oldItem: INTEGER; VAR oldVal: INTEGER; VAR oldSize: Point): BOOLEAN;
VAR
	newVal		: INTEGER;
	tempString	: Str255;
	hDITL		: Handle;
	itemKind	: INTEGER;
	itemRect	: Rect;
	err			: INTEGER;
	newProcID	: Integer;
	theErr		: OSErr;

BEGIN
	DoPopup := TRUE;
	newVal := GetCtlValue(infoP^.theControl);					{ what is the current value? }

	IF newVal <> oldVal THEN
		WITH infoP^ DO BEGIN

			{ Get the new tool name & procID	}

			GetItem(hMenu, newVal, tempString);

			{ Note that if we're going back to the original tool - either newVal = oldVal,
			{ or originalTool (core handle was nil) we use the original procID, }
			{ 'cuz the user may have changed the tool name in the Finder between }
			{ open & choose	(This would cause GetProcID to return a new ProcID since the }
			{ name is one of the matching criteria for open files }

			newProcID := firstProcID;					{ use the original tools procID }
			IF (newVal <> oldItem) OR (isOriginalTool) THEN
				newProcID := CTBGetProcID(tempString, msg);		 { Get a new ProcID }

			IF (newProcID = -1) THEN							 { Alias doesn't resolve }
			BEGIN
				UnavailableTool(infoP^.chDialog);
				SetCtlValue(infoP^.theControl,oldVal);
				EXIT(DoPopup);
			END;

			ClearDITL(infoP, FALSE, FALSE, oldSize);			{ get rid of the DITL	}

			{ We need to do another GetProcID in case the we're picking an alias to the
			  current tool.  In this case, the first GetProcID would return the refNum
			  of the open tool, but then the ClearDITL would close the tool - so we need
			  to verify the procID }

			tempProcID := firstProcID;					{ use the original tools procID }
			IF (newVal <> oldItem) OR (isOriginalTool) THEN
				tempProcID := CTBGetProcID(tempString, msg);		 { Get a new ProcID }

			{ Get rid of the old config	}

			IF (tempConfig <> NIL) THEN BEGIN
				DisposPtr(tempConfig);
				tempConfig := NIL;
			END;

			CTBDefault(tempConfig, tempProcID, TRUE, msg);	{ and get a new one }

			IF tempConfig = NIL THEN BEGIN
				err := CRMReleaseRF(tempProcID);			{ decrement the useCount }
				DoPopup := FALSE;
				EXIT(DoPopup);
			END;

			IF NOT ShowDITL(InfoP, oldSize) THEN BEGIN
				err := CRMReleaseRF(tempProcID);			{ decrement the useCount }
				DoPopup := FALSE;
				EXIT(DoPopup);
			END;
															{ set up the items }
			oldVal := newVal;								{ update value to detect toolchange }

			{ Simplifies the update Rgn	}
			itemRect := chDialog^.portRect;
			itemRect.left := 0;
			itemRect.top := oldSize.v;

			InvalRect(itemRect);							{ This is in global coords }

		End; {with}

	InitCursor;
END;


{ -----------------------------------------------------------------------
	BuildPopup	-	Scans thru the tool files and builds the
					tool popup

		infoP		-	Our internal info
		theHandle	-	Handle to the current tool
		cRec		-	Choose Special filtering
		toolName	-	The original tool Name
		oldVal		-	Returns the currently selected tool

------------------------------------------------------------------------ }
PROCEDURE BuildPopup(infoP: DialogInfoP;theHandle: CoreHandle;cRec: ChooseRec;
							toolName: Str255;VAR oldItem: INTEGER);
TYPE
	IntPtr	= ^Integer;

VAR
	maxTools,
	theItem		: INTEGER;				{ tool index		}
	vRefNum		: INTEGER;
	dirID		: LONGINT;
	theErr		: OSErr;
	tempTool	: STR63;				{ currently selected tool name }
	includeIt	: BOOLEAN;
	itemKind	: INTEGER;
	itemRect	: Rect;
	itemHandle	: Handle;
	thePB		: CInfoPBRec;			{ Cat info PB to resolve aliases }
	alias		: AliasHandle;			{ Alias info }
	resLoadState: Integer;
	aliasResFile: Integer;				{ Ref num of a finder alias file }
	theScript	: Integer;				{ Script of a file }


BEGIN
	{ Get the volume to index thru		}
	IF CRMFindCommunications(vRefNum, dirID) <> noErr THEN
		EXIT(BuildPopup);

	{ Start index loop	}

	maxTools := 0;						{ # of items in the menu	}
	oldItem := 0;
	theItem := 1;

	theErr := noErr;

	WITH infoP^ DO BEGIN
{begin addition <21>}
{	this makes sure that the menu is empty so that after a crash we don't get the trailing X's }
		maxTools := CountMItems(hMenu);
		for theItem := 1 TO maxTools do
			DelMenuItem(hMenu, theItem);
		maxTools := 0;
		theItem := 1;
{end addition <21>}

		WHILE theErr = noErr DO BEGIN
			theErr := CRMGetIndFile(tempTool,theType, vRefNum, dirID, theItem);

			IF theErr = noErr THEN

				IF LENGTH(tempTool) <> 0 THEN BEGIN

					{ Call the App's filter proc	}
					includeIt := TRUE;							{ include it }

					IF cRec.filterProc <> NIL THEN
						includeIt:= DoAppFilter(@tempTool,msg,theHandle,cRec.filterProc);

					IF includeIt THEN BEGIN
						{ Is this the current tool? }
						IF EqualString(tempTool, toolName, FALSE, TRUE) THEN
							oldItem := theItem;

						{ AppenMenu pays attention to special characters, which we don't want	}
						{ So we pre-append it with a dummy string, then add the real McCoy		}

						maxTools := maxTools + 1;					{ new menu item }
						AppendMenu(hMenu, 'X');
						SetItem(hMenu, maxTools, StringPtr(@tempTool)^);

						{ Is this tool really an alias? }
						WITH thePB DO
						BEGIN
							ioNamePtr := StringPtr(@tempTool);
							ioVRefNum := vRefNum;
							ioFDirIndex := 0;
							ioDirID := dirID;
						END;
						theErr := PBGetCatInfo(@thePB,false);

						IF ((theErr = 0) & (BAND(thePB.ioFlFndrInfo.fdFlags,fAlias) <> 0)) THEN
						BEGIN
							{ We are now dealing w/ an alias file }

							if (oldItem = 0) THEN
							BEGIN
								{We haven't matched a tool yet, this alias could possibly be
									the current tool.  The strategy we're currently using is:
									1 - Get the alias handle from the resource file itself.
									2 - Use the Alias Manager to extract the target file name of the alias
									3 - If this name matches, consider it the path used to get to this tool.
									If the real file is found later on, it will override the alias - however no more aliases will be matched.

									We don't want to resolve the alias at this time, because this would force the user to deal w/ inserting disks while
									the choose dialog is being built.  So, we're trying to guess how the user got to the current state
								}
								resLoadState := IntPtr(ResLoad)^;
								SetResLoad(FALSE);
								aliasResFile := HOpenResFile(vRefNum, dirID, tempTool, fsCurPerm);

								IF (aliasResFile > 0) THEN
								BEGIN
									SetResLoad(true);

									UseResFile(aliasResFile);
									alias := AliasHandle(Get1IndResource(rAliasType, 1));
									if (alias <> nil) then
									BEGIN
										theErr := GetAliasInfo(alias,asiAliasName,tempTool);
										IF ((theErr = 0) & (EqualString(tempTool, toolName, FALSE, TRUE))) THEN
										BEGIN
											oldItem := theItem; { Alias matches name w/ current tool }
										END;
									END;
									CloseResFile(aliasResFile);
								END;
								SetResLoad(resLoadState<>0);
							End;

							{ Set the appropriate style for the alias in the menu  - The FXInfo contains the script for the filename.  If this field is not valid, use
							  the system script
							}
							if (BAND(thePB.ioFlxFndrInfo.fdScript,validScript) <> 0) THEN
								theScript :=  BAND(thePB.ioFlxFndrInfo.fdScript,ScriptMask)
							ELSE
								theScript := LoWrd(GetEnvirons(smSysScript));

							{ Set the style correctly - asking the script for it's alias style }
							SetItemStyle(hMenu,maxTools,Style(GetScript(theScript,smScriptAliasStyle)));
						END;

					END;

					theItem := theItem + 1;						{ get the next tool please }

				END; {if tempTool <> ''}

		END; {while}

		{ If the user has moved the file out of the comm directory we can	}
		{ show the name, but disable it										}

		{ Note that we have to insert it at the top 'cuz we can only		}
		{ disable the first 31 items (Is this still true??)					}

		IF oldItem = 0 THEN	BEGIN
			maxTools := maxTools + 1;
			oldItem := 1;
			InsMenuItem(hMenu, 'X', 0);
			SetItem(hMenu,1,toolName);
			DisableItem(hMenu, 1);
		END;

		SetCtlMin(theControl, 1);
		SetCtlMax(theControl, maxTools);

		SetCtlValue(theControl, oldItem);				{ set up popup value }
	END; {with}
END;


{ -----------------------------------------------------------------------
	GetCurrentTool	-	Determines which tool is currently selected
						and returns a copy of the configPtr

		infoP		-	Our internal info
		theHandle	-	Handle to the current tool
		theConfig	-	A copy of the tool configPtr

		returns		-	the tool procID
------------------------------------------------------------------------ }

FUNCTION GetCurrentTool(infoP: DialogInfoP;theHandle: CoreHandle;cRec: ChooseRec;
												VAR theConfig: Ptr): INTEGER;
LABEL
	1;

VAR
	firstProcID		: INTEGER;
	err				: INTEGER;
	tempString		: Str255;
	configSize		: LONGINT;
	thePtr			: Ptr;

BEGIN
	firstProcID := -1;

	WITH infoP^ DO BEGIN

		{ If there is no tool handle, we get the first tool	}
		IF theHandle = NIL THEN BEGIN
			{ Handle app's request for a specific tool name	}
			IF LENGTH(cRec.newTool) <> 0 THEN
				BlockMove(@cRec.newTool, @tempString, LENGTH(cRec.newTool) + 1)
			ELSE IF CRMGetIndToolName(theType,1,tempString) <> noErr THEN
				GOTO 1;

			{ get the procID	}
			firstProcID := CTBGetProcID(tempString, msg);
			IF firstProcID = -1 THEN
				GOTO 1;

			{ Try to get a configPtr.  If failed, then cleanup	}
			CTBDefault(thePtr, firstProcID, TRUE, msg);

			IF thePtr = NIL THEN BEGIN
				err := CRMReleaseRF(firstProcID);			{ decrement the useCount }
				firstProcID := -1;
				GOTO 1;
			END;

		END
		ELSE BEGIN
			{ Get info from the tool	}
			firstProcID := theHandle^^.procID;
			thePtr := theHandle^^.config;
		END;

		{ Copy the config Ptr	}
		configSize := GetPtrSize(thePtr);

		theConfig := NewPtr(configSize);
		IF theConfig = NIL THEN
			firstProcID := -1
		ELSE
			BlockMove(thePtr, theConfig, configSize);

	END; { with}

1:
	GetCurrentTool := firstProcID;
END;

FUNCTION ReallyShutdown(theParent: DialogPtr): BOOLEAN;
VAR
	theDialog	: DialogPtr;
	theItem		: INTEGER;
	savedPort	: GrafPtr;

BEGIN
	InitCursor;

	ReallyShutdown := TRUE;							{ reckless }
	GetPort(savedPort);								{ save the port }
	theDialog := GetNewDialog(CTB_ChooseConfirmDLOGID, NIL, WindowPtr(-1));
	IF theDialog = NIL THEN							{ no dialog }
		Exit(ReallyShutdown);

	{ In System 7 the standard filter proc will handle the outline	}
	{ as well as the OK & Cancel hilite and key equivalents			}
	{ Also, the DLOG will be auto-cenetered							}

	SysBeep(5);
	ShowWindow(theDialog);							{ show it }

	ModalDialog(NIL, theItem);

	ReallyShutdown := (theItem = 1);				{ 1 = ok, 2 = cancel }
	DisposDialog(theDialog);						{ rid o the dialog }
	SetPort(savedPort);								{ restore the port }
END;

PROCEDURE UnavailableTool(theParent: DialogPtr);
VAR
	theDialog	: DialogPtr;
	theItem		: INTEGER;
	savedPort	: GrafPtr;

BEGIN
	InitCursor;

	GetPort(savedPort);								{ save the port }
	theDialog := GetNewDialog(CTB_ChooseUnavailableDLOGID, NIL, WindowPtr(-1));
	IF theDialog = NIL THEN							{ no dialog }
		Exit(UnavailableTool);

	{ In System 7 the standard filter proc will handle the outline	}
	{ as well as the OK & Cancel hilite and key equivalents			}
	{ Also, the DLOG will be auto-cenetered							}

	SysBeep(5);
	ShowWindow(theDialog);							{ show it }

	ModalDialog(NIL, theItem);

	DisposDialog(theDialog);						{ rid o the dialog }
	SetPort(savedPort);								{ restore the port }
END;

{	change from one connection type to another }
FUNCTION DoNewConn(VAR hConn:ConnHandle; oldTool: ConnHandle; tempProcID:INTEGER; quiet: BOOLEAN; theDialog: DialogPtr): INTEGER;
VAR
	status 		: CMStatFlags;
	sizes 		: CMBufferSizes;

BEGIN
	DoNewConn := ChooseOKMajor;
	IF NOT quiet THEN BEGIN
		{ Checks to see if the connection is opened to warn the user	}
		IF CMStatus(hConn, sizes, status) = noErr THEN
			IF (BAND(CMStatusOpen+CMStatusOpening,status)) <> 0 THEN
				IF NOT ReallyShutdown(theDialog) THEN BEGIN
					DoNewConn := ChooseAborted;				{ so beat a quick retreat }
					Exit(DoNewConn);
				END;
	END;

	CMDispose(hConn);								{ get rid of old connection }

	WITH oldTool^^ DO BEGIN
		hConn := CMNew(tempProcID, flags, bufSizes, refCon, userData);
		IF (hConn = NIL) THEN BEGIN
			DoNewConn := ChooseDisaster;
			EXIT(DoNewConn);
		END;

	{	hConn^^.reserved0 := reserved0;		}	{ now asyncEOM which does not need to be saved }
		hConn^^.reserved1 := reserved1;
		hConn^^.reserved2 := reserved2;
	END; {with}

END;



FUNCTION DoNewTerm(VAR hTerm:TermHandle; oldTool: TermHandle; tempProcID:INTEGER) : INTEGER;
VAR
	savedPort	:	GrafPtr;

LABEL
	1;

BEGIN
	DoNewTerm := ChooseOKMajor;

	GetPort(savedPort);

	TMDispose(hTerm);

	WITH oldTool^^ DO BEGIN
		SetPort(owner);
		hTerm := TMNew(termRect, viewRect, flags, tempProcID, owner, sendProc,
						cacheProc, breakProc, clikLoop, environsProc, refCon, userData);
		IF (hTerm = NIL) THEN BEGIN
			DoNewTerm := ChooseDisaster;
			GOTO 1;
		END;

		{ Need to save these as well	}
		hTerm^^.reserved1 := reserved1;
		hTerm^^.reserved2 := reserved2;
	END;

1:
	SetPort(savedPort);
END;

FUNCTION DoNewFT(VAR hFT:FTHandle; oldTool: FTHandle; tempProcID:INTEGER) : INTEGER;
BEGIN
	DoNewFT := ChooseOKMajor;

	FTDispose(hFT);

	WITH oldTool^^ DO BEGIN
		hFT := FTNew(tempProcID, flags, sendProc, recvProc,readProc, writeProc,
												environsProc, owner, refCon,userData);
		IF (hFT = NIL) THEN BEGIN
			DoNewFT := ChooseDisaster;
			EXIT(DoNewFT);
		END;

		hFT^^.reserved1 := reserved1;
		hFT^^.reserved2 := reserved2;
	END;
END;


PROCEDURE AdjustControl(theDialog: DialogPtr; theItem: INTEGER; hiliteVal: INTEGER);

VAR
	itemKind 		: INTEGER;							{ for manipulating items 	}
	itemHandle 		: ControlHandle;
	itemRect 		: Rect;

BEGIN
	GetDItem(theDialog,theItem,itemKind,Handle(itemHandle),itemRect);
	IF (itemHandle^^.contrlHilite <> hiliteVal) THEN
		HiliteControl(itemHandle,hiliteVal);
END;


FUNCTION ChooseFilter(theDialog : DialogPtr; VAR theEvent:EventRecord;
												VAR theItem:INTEGER) : BOOLEAN;
VAR
	result			: BOOLEAN;
	infoP 			: DialogInfoP;						{ dialog private data	 	}
	filterIt		: BOOLEAN;

LABEL
	1;
LABEL
	2;

BEGIN
	infoP := DialogInfoP(GetWRefCon(theDialog));

	{ Call the app's idle proc	}
	WITH infoP^ DO
		IF idleProc <> NIL THEN
			CallIdle(idleProc);

	filterIt := FALSE;
	result := FALSE;

	{ The filter procs are called in the following order:		}
	{ Tool's Filter, App's Filter, StdFilterProc, then us		}
1:
	WITH infoP^ DO BEGIN
		result := CTBSetupFilter(tempProcID, tempConfig, count, theDialog,
											theEvent, theItem, magicCookie, msg);
		IF (result) THEN
			GOTO 2;

		IF updateProc <> NIL THEN					{ CMPChoose stuff	}
			CallEventProc(theEvent, updateProc);

		result := StdFilterProc(theDialog, theEvent, theItem);
		IF (result) THEN
			GOTO 2;
	END;

	{ 2nd time thru => we filter the sucker & leave	}
	IF (filterIt) THEN
		result := TRUE;

	{ We need to clear the update flood that occurs with a pending	}
	{ update event for a window behind us.  We translate the event	}
	{ to a nullEvent and call the filter routines again				}

	IF (theEvent.what = updateEvt) OR (theEvent.what = activateEvt) THEN BEGIN

		IF (WindowPtr(theEvent.message) <> theDialog) THEN BEGIN
			theEvent.what := nullEvent;
			theItem := 0;
			filterIt := TRUE;
			GOTO 1
		END;

	END;

2:
	ChooseFilter := result;
END;


{	draw title of the user item }
PROCEDURE DrawTitle(theDialog : DialogPtr; itemNo : INTEGER);
VAR
	infoP			:	dialogInfoP;
	itemRect 		: 	Rect;
	savedFont		:	INTEGER;
	savedSize		:	INTEGER;

BEGIN
	infoP := DialogInfoP(GetWRefCon(theDialog));
	WITH infoP^ DO BEGIN
		savedFont := theDialog^.txFont;
		savedSize := theDialog^.txSize;

		TextFont(0);				{ system font and size please }
		TextSize(0);

		GetItemRect(theDialog,itemNo,itemRect);
		EraseRect(itemRect);		{ erase it please }
		TextBox( Ptr( ORD4(@title) + 1), LENGTH(title), itemRect, teJustLeft);

		TextFont(savedFont);		{ restore font and size information }
		TextSize(savedSize);
	END;
END;

{ 	useritem to draw line }
PROCEDURE DrawLine(theDialog : DialogPtr; itemNo : INTEGER);
VAR
	itemRect 		: 	Rect;
	savedPen		:	PenState;

BEGIN
	GetPenState(savedPen);
	PenNormal;

	GetItemRect(theDialog,itemNo,itemRect);
	FrameRect(itemRect);

	SetPenState(savedPen);
END;


{	this routine will set the dialog font and size information based upon the finf resource }
PROCEDURE SetDTextInfo(theDialog: DialogPtr; procID: INTEGER);
VAR
	savedPort		: GrafPtr;
	thefinf			: TextState;
	myPeek			: DialogPeek;
	savedState		: SignedByte;
	info			: FontInfo;

BEGIN
	GetPort(savedPort);									{ safe porting }
	SetPort(theDialog);

	Getflst(procID, CTB_ChoosefinfID, 1, thefinf);

	myPeek := DialogPeek(theDialog);

	savedState := HGetState(Handle(myPeek^.textH));		{ get handle state }
	HLock(Handle(myPeek^.textH));						{ get the dialog text edit handle }

	WITH thefinf DO BEGIN
		SetTextState(thefinf);

		GetFontInfo(info);								{ thanks scott douglass 		}

		WITH myPeek^.textH^^ DO BEGIN					{ Load in the text edit info	}
			txFont := theFont;
			txFace := theFace;
			txSize := theSize;
			txMode := theMode;

			WITH info DO BEGIN							{ need to stuff fields in text edit record }
				lineHeight := ascent + descent + leading;
				fontAscent := ascent;					{ I-378 }
			END;
		END;
	END;

	HSetState(Handle(myPeek^.textH), savedState);		{ restore dialog }

	SetPort(savedPort);									{ and now back to our regular station }
END; { SetDTextInfo }


{
	This will move the dynamic items back to the minimum threshhold
}
PROCEDURE HideDynamicItems(theDialog: DialogPtr);
VAR
	itemKind	: INTEGER;			{ DITL stuff			}
	itemHandle	: Handle;
	itemRect	: Rect;
	delta		: INTEGER;
	popupRect	: Rect;

BEGIN
	{ Calculate minimum window width	}
	GetItemRect(theDialog, ChooseItemPopup,popupRect);

	{ Item outline									}
	{ Save this to be erased in ShowDynamicItems	}
	GetDItem(theDialog, ChooseItemOutline, itemKind, itemHandle, itemRect);
	DialogInfoP(GetWRefCon(theDialog))^.oldOutline := itemRect;
	delta := (popupRect.right + ITEMSLOP) - itemRect.left;
	OffsetRect(itemRect, delta, 0);
	SetDItem(theDialog, ChooseItemOutline, itemKind, itemHandle, itemRect);

	{ DividingLine: Don't erase for better aesthetics	}
	GetDItem(theDialog, ChooseItemDottedLine, itemKind, itemHandle, itemRect);
	itemRect.right := itemRect.left + GetMinWidth(theDialog);
	SetDItem(theDialog, ChooseItemDottedLine, itemKind, itemHandle, itemRect);

	GetDItem(theDialog, ChooseItemOK, itemKind, itemHandle, itemRect);
	delta := (popupRect.right + ITEMSLOP) - itemRect.left;
	OffsetRect(itemRect, delta, 0);
	SetDItem(theDialog, ChooseItemOK, itemKind, itemHandle, itemRect);

	GetDItem(theDialog, ChooseItemCancel, itemKind, itemHandle, itemRect);
	delta := (popupRect.right + ITEMSLOP) - itemRect.left;
	OffsetRect(itemRect, delta, 0);
	SetDItem(theDialog, ChooseItemCancel, itemKind, itemHandle, itemRect);
END;


{	This will set the itemRects for the dynamic items
	after an AppendDITL so that the OK & Cancel buttons
	are in the top-right corner
}
PROCEDURE ShowDynamicItems(theDialog: DialogPtr);
VAR
	itemKind	: INTEGER;			{ DITL stuff			}
	itemHandle	: Handle;
	itemRect	: Rect;
	delta		: INTEGER;			{ We love to fly…		}
	validMe		: Rect;				{ simplify updateRgn	}

BEGIN
	LONGINT(validMe.topLeft) := 0;
	LONGINT(validMe.botRight) := 0;

	{ We erase the old location of the outline	}
	EraseRect(DialogInfoP(GetWRefCon(theDialog))^.oldOutline);

	{ For all 4 items we draw it explicitly instead of waiting for an update	}
	{ Then we need to get these areas out of the update rgn						}

	{ Dividing Line: Pin to right edge of window	}
	GetDItem(theDialog, ChooseItemDottedLine, itemKind, itemHandle, itemRect);
	itemRect.right := theDialog^.portRect.right;

	SetDItem(theDialog, ChooseItemDottedLine, itemKind, itemHandle, itemRect);
	DrawLine(theDialog, ChooseItemDottedLine);

	UnionRect(validMe,itemRect,validMe);

	{ The following execution order is important!!	}

	{ OK Button: Move the puppy						}
	{ _SetDItem does not call _MoveControl, so we do this ourselves	}
	{ Also, _MoveControl erases, invals, then redraws				}
	GetDItem(theDialog, ChooseItemOK, itemKind, itemHandle, itemRect);
	delta := theDialog^.portRect.right - ITEMSLOP - itemRect.right;
	OffsetRect(itemRect,delta,0);

	MoveControl(ControlHandle(itemHandle),itemRect.left, itemRect.top);
	SetDItem(theDialog, ChooseItemOK, itemKind, itemHandle, itemRect);

	UnionRect(validMe,itemRect,validMe);

	{ OK Outline: Moves with the OK button						}
	{ This must come after the OK button as MoveControl erases	}
	GetDItem(theDialog, ChooseItemOutline, itemKind, itemHandle, itemRect);
									{ Compensate for the outline				}
	delta := theDialog^.portRect.right - (ITEMSLOP - OUTLINEWIDTH) - itemRect.right;
	OffsetRect(itemRect,delta,0);

	SetDItem(theDialog, ChooseItemOutline, itemKind, itemHandle, itemRect);
	DoSolidOutline(theDialog, ChooseItemOutline);

	UnionRect(validMe,itemRect,validMe);


	{ Cancel Button: Moves under the OK button		}
	GetDItem(theDialog, ChooseItemCancel, itemKind, itemHandle, itemRect);
	delta := theDialog^.portRect.right - ITEMSLOP - itemRect.right;
	OffsetRect(itemRect,delta,0);

	MoveControl(ControlHandle(itemHandle),itemRect.left, itemRect.top);
	SetDItem(theDialog, ChooseItemCancel, itemKind, itemHandle, itemRect);

	UnionRect(validMe,itemRect,validMe);

	{ We want a simple updateRgn so we validate essentially the whole top part	}
	ValidRect(validMe);
END; { ShowDynamicItems }

{	Determines the minimum width of the dialog
}
FUNCTION GetMinWidth(theDialog: DialogPtr): INTEGER;
VAR
	itemRect,
	popupRect	: Rect;

BEGIN
	GetItemRect(theDialog,ChooseItemPopup,popupRect);
	GetItemRect(theDialog,ChooseItemOutline,itemRect);
	GetMinWidth := popupRect.right + ITEMSLOP + (itemRect.right - itemRect.left);
END; { GetMinWidth }

{	=======================================================================	}
{							Special Stuff follows							}
{	=======================================================================	}

{
	This used to be the Special conditional, which was taken out after 7.0.
	Special was false, for 7.0, so this code can be just activated for
	TheFuture
}


{ -----------------------------------------------------------------------
	DoAppFilter	-	Calls the application filter

		tempTool	-	Ptr to the tool string
		msg			- 	What tool class
		theHandle	-	Handle to the current tool
		filterProc	-	Routine to call

		returns		-	false if the tool should be filtered
------------------------------------------------------------------------ }
FUNCTION DoAppFilter(tempTool: Ptr; msg: INTEGER;theHandle: CoreHandle;
												filterProc: ProcPtr): BOOLEAN;
VAR
	itemKind	: INTEGER;
	err			: INTEGER;
	theContext	: CRMToolContext;

BEGIN
{  	NEWCOMMENT 5 march 1991
	we always do a reserverf and releaserf regardless if handle is nil or not }

{	OLD COMMENT
	Above, if there was a nil Handle, we do a getProcID,
	  but we aren't doing a ReserveRF.  Therefore, if we fall into this
	  code, and do a reserve, we will only set the index to 1, the
	  release would then close the file.  This is ungood. }

	itemKind := CTBGetProcID(StringPtr(tempTool)^, msg);

	DoAppFilter := FALSE;
	IF itemKind <> -1 THEN BEGIN
															{ firstProcID is already open }
		err := CRMReserveRF(itemKind);

		{ We put the tool into the resChain temporarily	}
		{ so the app can get some resources				}

		CRMToolPreflight(itemKind, theContext);

		DoAppFilter := CallFilter(theHandle, StringPtr(tempTool)^,
														itemKind, filterProc);
		CRMToolPostflight(theContext);
		err := CRMReleaseRF(itemKind);
	END;
END;

{
	This back door allows script to change tools from one type to another using a standard call
}
FUNCTION DoBackDoor(msg: INTEGER; VAR theHandle: CoreHandle; VAR cRec: ChooseRec): INTEGER;
VAR
	theConfig:	Ptr;
	newID: INTEGER;
	noChange: BOOLEAN;
	theDialog: DialogPtr;		{ ignored by DoNewConn because quiet is TRUE }
	tempTool	: Handle;

BEGIN
	DoBackDoor := ChooseOKMajor;							{ optimism }
	IF theHandle = NIL THEN									{ be happy }
	BEGIN
		DoBackDoor := ChooseFailed;							{ bye bye }
		Exit(DoBackDoor);									{ taps would be appropriate here }
	END;

	newID := CTBGetProcID(StringPtr(@cRec.newTool)^, msg);
	IF newID = -1 THEN BEGIN
		DoBackDoor := ChooseFailed;							{ bye bye }
		Exit(DoBackDoor);									{ taps would be appropriate here }
	END;

	IF cRec.newConfig = NIL THEN							{ did we specify a new config? }
		CTBDefault(theConfig, newID, TRUE, msg)				{ call core default routine }
	ELSE
		theConfig := cRec.newConfig;						{ yup! }

	{ But we don't seem to use theConfig later???!!!	}

	IF theConfig = NIL THEN									{ could not default }
	BEGIN
		DoBackDoor := ChooseFailed;							{ bye bye }
		Exit(DoBackDoor);									{ taps would be appropriate here }
	END
	ELSE BEGIN
		{ duplicate the old tool handle	}
		tempTool := Handle(theHandle);
		IF (HandToHand(tempTool) <> noErr) THEN BEGIN
			DoBackDoor := ChooseAborted;
			EXIT(DoBackDoor);
		END;

		HLock(tempTool);

		IF msg = ChooseCM THEN
			DoBackDoor := DoNewConn(ConnHandle(theHandle), ConnHandle(tempTool), newID, TRUE, theDialog);

		IF msg = ChooseTM THEN
			DoBackDoor := DoNewTerm(TermHandle(theHandle), TermHandle(tempTool), newID);

		IF msg = ChooseFT THEN
			DoBackDoor := DoNewFT(FTHandle(theHandle), FTHandle(tempTool), newID);

		HUnlock(tempTool);
		DisposHandle(tempTool);
	END;

END;


END.
