{
	File:		StandardNBP.p

	Contains:	StandardNBP - network lookup package

	Written by:	Byron Han, Jerry Godes, and Alex Kazim

	Copyright:	© 1988-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<17>	 1/28/91	kaz		BBH, #81369: Fix update flood bug so null events will let
									StdFilterProc draw the OK outline properly.
		<16>	 1/25/91	stb		JDR: rename LookupName to avoid conflict with AppleTalk.p
		<15>	 11/6/90	kaz		Uses StdFilterProc to handle key events, button outlining &
									hiliting; Uses an EventRecord instead of separate fieds; Uses
									TypeSelect Utilities for keyboard type ahead. <jng>
		<14>	 9/16/90	gbm		change name of UNIT to not conflict with a call name
		<13>	 8/22/90	kaz		Fixed a bug that erases the item list after some time; Also
									fixed one that stopped accepting keyboard input for itemList
									navigation.
		<12>	 8/15/90	kaz		Added NIL checking for _DisposPtr. Will be fixed in system
									later, but I need it here for System 6 anyway.
		<11>	  8/6/90	kaz		Used utility routines for OK outline. New gray algorithm for OK
									outline.
		<10>	 5/24/90	kaz		Added Boolean param to GetIndflst
		 <9>	 5/22/90	kaz		Moved GetTextState, SetTextState to utilities; Moved TextState
									to Private intf files
		 <8>	 5/17/90	kaz		Fixing Point Coercions for MPW 3.1+.  Removed 'parm' resource.
									Gets font info from 'flst'.
		 <7>	 5/15/90	kaz		Putting _ResizeDialog on a conditional compile Version string is
									now a 'PICT' hookVersion is converted from a userItem to a
									picItem on the fly to save code & maintain compatibility. Fixed
									select flicker at list boundaries
		 <6>	 4/18/90	kaz		Added ResizeDialog() which will try to resize/arrange the
										dialog if no zones are availabkle and we're called
										using the original dialog box.
									GotoXXXFocus() will no longer focus on a hidden item
									Put all the focusable items into an activeList along
										with some routines to build and access it.  This
										makes us similar to new StandardFile
		 <5>	 4/11/90	kaz		Fixed PixelWidth of the Focus frame
		 <4>	 4/11/90	kaz		Code Shrink/Cleanup 
		 							- Removed remaining Conditional compiles 
									- No longer frames EditText items 
									- Checks for CmdPeriod() using Tech Note utility 
									- DoChangeZone() & DoChangeItem now share code
									- Uses common code to create both lists 
									- Tweaked all the routines 
									- Added GetTextState(0 and SetTextState() 
									- No checking for NIL on _DisposPtr,_DisposHandle 
									- Checked for unneeded assignments for _NewPtrClear, etc
		 <3>	 3/20/90	kaz		Merging in code shrink/cleanup from 1.0
									- Locked DataH before __LookupName() and ItemsToList()
									- Changing ZonesExist() to return the bridge, net &
	   									result
									- Fixing DrawFocus() and EraseFocus() to abandon 
	   									color & regions to shrink the code
									- Uses GetItemRect() instead of GetDItem
									- Uses SetUserItemProc() instead of GetDItem/SetDItem
									- Get pattern from QD globals rather than GetIndPattern
									- Cleaned up magic numbers
									- Used direct string index access rather than relying
										on Pascal's string copying/checking
									- Got rid of some unused conditional compiles
									- Used PCopyString instead of Pascal aStr := bStr;
		 <2>	 3/16/90	BBH		made it compile
		 <1>	 3/14/90	BBH		first checked in to BBS

	To Do:
		• Change _GetDblTime to _GetKeyTime or equivalent
		• StandardFilter() should call UserFilterProc() first
		• Use RelString instead of IUEqualString?
		• Use TypeSelectUtils after 7.0a10.  Keep the old stuff for 6.0
		• Don't focus if only 1 list item is visible


	Pre-BBS Modification History
		<64>	 2/20/90	BBH		change parm to ctbr
		1/21/88		BBH		Started.
		3/5/88		BBH		More work
		10/27/88	BBH		Changed into NuLookup
		4/12/89 	JNG		Major Cleanup work started.  This code needs much work to 
								be easier to read and less buggy.
		4/24/89 	JNG		More cleanup.
							Fixed bug in aging entries.
							Fixed bug in search WHILE typing.
		4/27/89		JNG		Added support for default lookup - pass numtypes = 0.
								typeList is now a VAR parameter internally.
		5/3/89		JNG		Changed numtypes = 0 for all to numtypes = -1 for all (oops, I
								listened to Glenn).
		5/17/89		JNG		Check for error in Adding items to List now doesn't break when
								AppleTalk isn't active.
							The zone list is always created because it looks ugly without the 
								empty list.
		5/24/89		JNG		Default parms wasn't setting interval.  Also changed to match
								parms stored in resource.
							Fixed cleanup a bit
		5/31/89		JNG		DoMoreSearch now doesn't THEN dimmed items.  AddSorted now checks
								for duplicates, so we don't have to.  SortedList interface changes.
								Passing DataH in places where we were passing many fields.
		7/7/89		JNG		Fixed a bug where IF the item selected was the first item, and THEN
								the user turned off selection (or IF there was no selection, and 
								the user picked the first item).  The problem was in the check to
								see IF the user picked a new cell, or just reselected the old one.
							Also IF the user specifies a default zone, and that zone is not 
								present, THEN don't THEN anything (was selecting the home zone)
		8/13/89		BBH		Changed ICON to hIcon in NLTypeEntry
							Changed use of DrawOutlineButton to GoodDrawOutlineButton which
								does not DO a InsetRect(-3, -3).  (NuLookup.r was modified to
								have the itemRect for the outline already outset 3 pixels)
								
		8/25/89		BBH		Changed Longint to LONGINT and Integer to INTEGER
							If user tries to select dimmed zone, then revert to previously selected
								zone (if any)
							Moved MaxLim to parm resource
							Added version # fo parms resource
							
							Enter/Return/Command-. now cause controls to flash appropriately.
							
							When typing to select an item, we now will jump to next undimmed item
								if the item that otherwise would be selected
							Also if typing to select an item and we exceed 32 characters we will now
								start beeping at the user.
							
							Version dialog comes up pinned to bottom of the nulookup dialog so as to not
								disturb anything outside of the nulookup dialog.
								
							If bottommost item in itemList is dimmed and we have no current selection
								and we hit the upArrow key we still select bottommost item
							
							Consolidate DoNextZone/PrevZone and DoNextItem/PrevItem
		8/29/89		BBH		Added support for different fonts/sizes for the lists.
							Also better calculate size of the list rView as well as cellSizes.
		
		8/31/89		BBH		Prevent List Manager boil over (too many cells…)
							Also dynamically grow the NBP response buffer as needed
		9/2/89		BBH		Fix up error detection in SetupBuffers					
							Integrated calling userHookProc into standardHookProc
								and changed calls for hookPreflight and hookPostflight to
								simply call standardHook which in turn calls userHookProc
							When changing zones with DoChangeZone (in resp to command-Arrow Up/Down,
								we now generate a hookItemRefresh hookProc message)
							Also a hookZoneRefresh will generate a hookItemRefresh message
		9/11/89		BBH		Now flash hilite ON and then OFF for the OK/Cancel buttons 
								instead of just turning hilite ON.
							
							Fixed problem with versColor.
		9/14/89		BBH		Redo calculation of version box placement/sizing
							When setting up the item list, do NOT select if the item to be selected
								is dimmed.
		
							Eliminate flicker in cursor around the scrollbar in the itemList.
							This was because we were calling LDoDraw(TRUE) always in CheckProcess.
							Now only call when it was set to false.	( thanks to Mike Radovancevich )

							Fixed aging algorithm - would have skipped entry immediately after an item
								that was deleted.
								
		9/18/89		BBH		Work around List Manager bug in arrowing down causing LAutoScroll jump
							(manually detect being last cell in list and then do a LScroll ourselves
								only for going down on the list with the old selection the bottommost
								visible cell)
							
							Also fixed problem of slow clearing of the itemList when changing zones
								(call LDelRow outside of the LDoDraw FALSE/TRUE pair)
								
		10/2				DrawVersion now draws using teJustLeft
		10/13/89	BBH		Added Focus for WHEATIES compilerdirective
		
		2/5/90		BBH		Changed focus to be 3 pixel border like Chooser
		2/12/90		kaz		Focus is now drawn using a region rather than rects
							Focus frame is drawn in the hilite color
							Sets a flag (isZoning) if there's typing in the zone
								list to delay the lookup
							StandardHook will check isZoning & the time before
								doing a lookup
							DoChangeZones will set the delay flag & timer for
								zone lookups so cursor keys will work well
		
		
	Algorithm for Hiliting of the OK/Cancel buttons
	
	1.	If Enter/Return is hit, hilite the OK button
	2.	If Command-./Escape is hit, hilite the CANCEL button
	3.	If double click in the list window, hilite the OK button
	
	4. 	If user's filter procedure returns item hit of hookOK or hookCancel
	and the event (after filtering) is not a mouseDown then
	hiliting will occur on the OK or Cancel button as appropriate.

	5.	If user's filter procedure returns item hit of hookOK or hookCancel
	and the event (after filtering) is a mouseDown then if the
	theEvent.where (after filtering) is converted to local coordinates
	and found to not be in the item rectangles for hookOK and hookCancel
	then we will hilite the controls as appropriate.
	
	(do the user filter procedure)
	
	where := theEvent.where;
	GlobalToLocal(where);
	IF FindDItem(theDialog, where)+1 = hookOK OR hookCancel THEN
		(the original mouseDown was already in the OK/Cancel button
			so no need to do additional hilighting)
	ELSE
		IF theItem = hookOK or hookCancel (theItem is returned from filterProc)
			do hilighting
	
	
	6.	If the hook procedure changes an itemHit into hookOK or hookCancel then
	we will perform hilighting of the appropriate control.
	
	
	Hilighting of the control consists of
		HiliteControl(theControl, 1);		( turn it on )
		Delay( 8, fooTicks );				( emprical delay from ModalDialog sources )
		HiliteControl(theControl, 0);		( turn it off )
}

Unit StandardNBPUNIT;

INTERFACE
USES
	MemTypes, 
	QuickDraw, 
	OSIntf, 
	ToolIntf, 
	PackIntf, 
	AppleTalk,
	Script,	
	Traps,
{$IFC NOT PRE70}
	 DialogsPriv,		{ StdFilterProc stuff 	}
	 IntlUtilsPriv,		{ Type ahead stuff		}
{$ENDC}
	CTBUtilities,
	CommToolboxPriv,
	StandardNBPStructures,
	ListUtilities,
	NBPUtilities,
	ZIPUtilities;
	
{$SETC RESIZE := FALSE		}		{ Code for dynamic size if no zones		}
	
{
    ***********NOTE*******WARNING*******WATCHOUT**********HACK*HACK*HACK*******************
    ***********NOTE*******WARNING*******WATCHOUT**********HACK*HACK*HACK*******************
    ***********NOTE*******WARNING*******WATCHOUT**********HACK*HACK*HACK*******************
    typeList as defined here is a VAR parameter - Pascal calling convention causes this array
    to be passed by address, and the calling PROCEDURE makes a local copy.  However, since we 
    don't change this we don't need a copy of it.  We also don't want a copy because this array
    can be of any size.  The compiler won't copy the correct amount IF it's passed by value
    ***********NOTE*******WARNING*******WATCHOUT**********HACK*HACK*HACK*******************
    ***********NOTE*******WARNING*******WATCHOUT**********HACK*HACK*HACK*******************
    ***********NOTE*******WARNING*******WATCHOUT**********HACK*HACK*HACK*******************
}



{	Main entry point	}
FUNCTION NuLookupEntry(where : Point; prompt : Str255; numTypes : INTEGER;
				   VAR typeList: NLType; nameFilter : ProcPtr; zoneFilter : ProcPtr;
				   hookProc : ProcPtr; userData : LONGINT; dialogID : INTEGER;
				   filterProc : ProcPtr; VAR theReply : NBPReply) : INTEGER;
				   
{	Sets up local storage, dialogs, lists, and starts the first NBP lookups }
FUNCTION NuLookupPreflight(where : Point; VAR prompt : Str255; _numTypes : INTEGER;
				   typeList: NLTypePtr; _nameFilter : ProcPtr; _zoneFilter : ProcPtr;
				   _hookProc : ProcPtr; _userData : LONGINT; dialogID : INTEGER;
				   _filterProc : ProcPtr; VAR theReply : NBPReply ; VAR theData : DataH)
				   	: INTEGER;
				   
FUNCTION StandardFilter(theDialog: DialogPtr; VAR theEvent: EventRecord; 
						 VAR itemHit: INTEGER): Boolean;
FUNCTION StandardHook(theItem : INTEGER; theData : DataH) : INTEGER;

{	useritem procedures }
PROCEDURE DrawList(theDialog : DialogPtr; itemNo : INTEGER);
PROCEDURE DrawLine(theDialog : DialogPtr; itemNo : INTEGER);
{$IFC NOT PRE70}
PROCEDURE DrawNothing(theDialog : DialogPtr; itemNo : INTEGER);	
{$ENDC}

{	called from NuLookupPreflight }
FUNCTION CreateList(theData: DataH; whichList: INTEGER): ListHandle;
FUNCTION  SetupDialog(theData : DataH; VAR prompt : Str255; dialogID : INTEGER) : INTEGER;
PROCEDURE SetupZones(theData : DataH); 
PROCEDURE SetupItems(theData : DataH);

{$IFC RESIZE	}
PROCEDURE ResizeDialog(theData: DataH);
{$ENDC RESIZE	}

{	Called to check on status of asynchronous NBP/ZIP lookups }
PROCEDURE CheckZone(theData : DataH);
PROCEDURE CheckProcess(theData : DataH);
PROCEDURE StartNextProcess(theData: DataH);
PROCEDURE StopProcess(theData : DataH);

FUNCTION SetupBuffers(theData: DataH): OSErr;					{ dynamically setup NBP Buffers }

PROCEDURE NuLookupCleanup(theData : DataH);

{	Called in response to arrows/command-arrows/typing to select items }
FUNCTION ChangeInList(theList: ListHandle; direction: BOOLEAN): BOOLEAN;
PROCEDURE DoChangeZone(theData: DataH; direction: BOOLEAN);
PROCEDURE DoChangeItem(theData: DataH; direction: BOOLEAN);

{$IFC PRE70}
PROCEDURE DoMoreSearch(theData : DataH; theChar : CHAR);
{$ELSEC}
PROCEDURE DoMoreSearch(theData : DataH);
{$ENDC}

{	Utilty routine that checks to see IF currently selected item is dimmed }
FUNCTION CheckCell(theList : ListHandle) : BOOLEAN;

{	set cursor to watch }
PROCEDURE DoWatch;

{	fill in data structures with currently selected items }
PROCEDURE FillReply(theData : DataH ) ;

FUNCTION  MaxEntities: INTEGER;				{ returns max # of entities supported }
PROCEDURE RedrawVersion(theData: DataH);	{ invalidates version box }
			
FUNCTION	GetItemKind(theDialog: DialogPtr; itemNo: INTEGER): INTEGER;

{ FOCUS management	}
PROCEDURE	GotoNextFocus(theData: DataH);
PROCEDURE	GotoPrevFocus(theData: DataH);

FUNCTION	FocusIsTEItem(theData: DataH): BOOLEAN;

PROCEDURE 	DrawFocus(theDialog: DialogPtr; itemNo: INTEGER);
PROCEDURE 	EraseFocus(theDialog: DialogPtr; itemNo: INTEGER);

PROCEDURE	ChangeFocus(theData: DataH; newFocus: INTEGER; noSelect: BOOLEAN);

PROCEDURE 	SetupFocusUserItem(theData: DataH);
PROCEDURE 	HideFocusUserItem(theData: DataH);

FUNCTION 	GetMaxFocus(activeList: Ptr): INTEGER;
FUNCTION 	GetFocusItem(activeList: Ptr; theOffset: INTEGER): INTEGER;
FUNCTION 	GetFocusEntry(activeList: Ptr; theItem: INTEGER): INTEGER;
PROCEDURE	SetFocusEntry(activeList: Ptr; theEntry, theItem: INTEGER);
FUNCTION 	BuildFocusList(theData: DataH): Ptr;

IMPLEMENTATION

CONST	
	kFocusSize		=	3;				{ inset of the focus border 		}
	kFocusWidth		=	kFocusSize - 1;	{ width of the frame				}
	kCellWithIcon	=	18;				{ pixel height for icons			}
	kScrollBarInset	=	15;				{ scroll bar width in pixels		}
	kOutlineWidth	=	3;				{ pixel width of OK outline			}
	kEditTextWidth	=	3;				{ pixel outset of edit text box		}
	
	kEntityHead		= 	9;				{ wish I knew what this did…		}
	kLookupDelay	=	20;				{ for keydowns in the lists			}
	kNBPMaxSize		=	32767;			{ Max size of NBP Response			}

	
	getNextThingy	= TRUE;
	getPrevThingy	= FALSE;
		
	entityLowerLimit	=	10;			{ minimum # of entities to look for }

TYPE
	IntPtr 			= 	^INTEGER;
	IntHandle 		= 	^IntPtr;


FUNCTION CallHook(hookProc: ProcPtr; itemHit: INTEGER; 
					theDialog: DialogPtr): INTEGER; EXTERNAL;
FUNCTION CallFilter(filterProc: ProcPtr; entity: EntityName): INTEGER; EXTERNAL;
FUNCTION CallDFilter(filterProc: ProcPtr; theDialog: DialogPtr; VAR theEvent: EventRecord; 
					VAR ItemHit: INTEGER): Boolean; EXTERNAL;


{$R-} {Turn off range checking - It has to be off so that we can loop through the type list }
	 	{	without generating an error }
					
FUNCTION NuLookupEntry(where: Point; prompt: Str255; numTypes: INTEGER;
				   VAR typeList: NLType; nameFilter: ProcPtr; zoneFilter: ProcPtr;
				   hookProc: ProcPtr; userData: LONGINT; dialogID: INTEGER;
				   filterProc: ProcPtr; VAR theReply: NBPReply) : INTEGER;
VAR
	result		: OSErr;
	theControl	: ControlHandle;		{	hControl for the OK button	}
	theData		: DataH;				{ 	NuLookup globals 			}
	theList		: ListHandle;	
	savedPort	: GrafPtr;
	oldItemHit	: INTEGER;
	itemHit		: INTEGER;				{ 	GetDItem stuff				}
	itemKind	: INTEGER;
	itemHandle	: Handle;
	itemRect	: Rect;
	theCell,
	oldCell		: Point;				{	Cell manipulation			}	
	quit 		: Boolean;				{	Are we done yet?			}
	fooTicks	: LONGINT;
	theDialog	: DialogPtr;
	evtWhere	: Point;
	
	
LABEL
	1;									{	Common error exit point		}
	
BEGIN
	GetPort(savedPort);
	DoWatch;							{	gimme a watch cursor		}
	
{	Preflight allocates local storage, loads in the parameter resource, initializes
	the dialogs, the lists, and starts the NBP lookups…
}
	result := NuLookupPreflight(where, prompt, numTypes, @typeList, nameFilter,
					zoneFilter, hookProc, userData, dialogID, filterProc, theReply,
					theData);

	IF result <> nlOk THEN									{	did we successfully complete? 		}
	BEGIN
		NuLookupEntry := result;										{	nope								}
		GOTO 1;
	END;
	
	{ Since we derefernce this everywhere, let's lock 'till cleanup	}
	HLock(Handle(theData));
	
	WITH theData^^ DO BEGIN
		theDialog := dialog;
		InitCursor;												{ 	we are OK now						}
		quit := FALSE;
		
		WHILE NOT quit DO BEGIN
			GetDItem(theDialog, hookOK, itemKind, 				{	Get OK button for later				}
									Handle(theControl), itemRect); 
			
			LONGINT(theCell) := 0;								{	Hilite/dim OK button				}
			itemHit := theControl^^.contrlHilite;
			itemHit := BAND(itemHit, $000000FF);				{	give me low byte only				}
			
			IF LGetSelect(TRUE,theCell,itemList) THEN	{ 	there is a selection so undim OK	}
			BEGIN
				IF itemHit <> 0 THEN							{ 	only change if need to				}
				BEGIN
					HiliteControl(theControl,0);
					
					{ StdFilterProc will outline for us	}
{$IFC PRE70}
					SetUserItemProc(theDialog, hookOutline, ProcPtr(@DoSolidOutline));
					DoSolidOutline(theDialog, hookOutline);
{$ENDC}
				END;
			END
			ELSE IF itemHit <> 255 THEN BEGIN					{	no selection so dim OK				}
				HiliteControl(theControl,255);
{$IFC PRE70}
				SetUserItemProc(theDialog, hookOutline, ProcPtr(@DoGrayOutline));
				DoGrayOutline(theDialog, hookOutline);
{$ENDC}
			END;
	
			CheckProcess(theData);								{	what's going on with Async Lookups	}
				
			FillReply(theData);
			shouldHilite := FALSE;					{	reset the shouldHilite bit		}	
			
			ModalDialog(@StandardFilter,itemHit);
		
			evtWhere := myEvent.where;				{ local the mouse hit	}
			GlobalToLocal(evtWhere);
			
			IF itemHit = hookItemList THEN BEGIN	
				ChangeFocus(theData, itemHit, TRUE);
				theList := itemList;
				
				IF LClick(evtWhere, myEvent.modifiers, theList) THEN BEGIN
																{ double click 						}
					IF CheckCell(theList) THEN BEGIN			{ 	Is the cell dimmed?				}
						itemHit := hookOK;						{ 	Nope - we're done!				}
						shouldHilite := TRUE;			{	let's hilite the control		}
					END;
				END
				ELSE											{ single click 						}
					IF CheckCell(theList) THEN					{	Is the cell dimmed?				}
						;
			END
			ELSE IF itemHit = hookZoneList THEN	BEGIN				{ 	Change in zones 				}
			
				ChangeFocus(theData, itemHit, TRUE);
				{ We now want to check IF the zone has changed so we can generate the HookItemRefresh 	}
				theList := zoneList;
				
				LONGINT(oldCell) := 0;								{ 	Get old selection 				}
				IF NOT LGetSelect(TRUE, oldCell, theList) THEN
					LONGINT(oldCell) := -1;							{ 	no old selection - wierd - 
																			shouldn't happen, eh		}
					
				{Handle the click - it doesn't matter IF it is a double click }
				IF LClick(evtWhere, myEvent.modifiers, theList) THEN
					;
					
				LONGINT(theCell) := 0; 								{ 	Get new selection 				}
				IF NOT LGetSelect(TRUE, theCell, theList) THEN
					LONGINT(theCell) := -1;							{ 	user didn't select anything 	}
					
				{Note : Once someone selects a dimmed zone, there is no selection, so if they
				 select that zone again, there will be a new selection, and we will fall into this 
				 section of code, and turn it off }
				
				{ 	Change in selection				}
				IF NOT EqualPt(theCell, oldCell) THEN BEGIN
					itemHit := hookItemRefresh;
					
					IF NOT CheckCell(theList) THEN BEGIN					
						IF LONGINT(oldCell) <> -1 THEN		
							LSetSelect(TRUE, oldCell, theList);	
						itemHit := hookZoneList;			
					END;
				END;
				
			END; { Else zone change	}
				
			IF GetItemKind(theDialog, itemHit) = editText THEN
				ChangeFocus(theData, itemHit, TRUE);
				
			FillReply(theData);			{Make the reply up to date with above selection.  This is also
										needed so that the hookProc can correctly update the
										list IF the zone has changed}
	
	{==============================================================================================================
		explicitly calling users hookProc is collapsed into StandardHook (like StandardFilter will call
		user's filterProc if specified
		
	==============================================================================================================
			IF hookProc <> nil THEN
				itemHit := CallHook(theData^^.hookProc, itemHit, theDialog);
	==============================================================================================================}
	
			
			oldItemHit := itemHit;
			itemHit := StandardHook(oldItemHit,theData);				{ this WILL call user hookProc				}	
			
			IF (itemHit = hookOK) THEN
				IF (oldItemHit <> hookOK) THEN
					shouldHilite := TRUE;							{ hookProc made us to a hookOK/hookCancel	}
				
			IF (itemHit = hookCancel) THEN
				IF (oldItemHit <> hookCancel) THEN
					shouldHilite := TRUE;							{ hookProc made us to a hookOK/hookCancel	}
			
			case itemHit of
				HookOK: 
					BEGIN
						LONGINT(theCell) := 0;						{	Can't OK IF there's no selected item	}
						IF LGetSelect(TRUE, theCell, itemList) THEN BEGIN
							quit := TRUE;
							NuLookupEntry := NLok;
						END
						ELSE	{	dim the control - this should never really happen }
							HiliteControl(theControl, 255);
					END;
					
				HookCancel:
					BEGIN
						quit := TRUE;
						NuLookupEntry := NLcancel;					
					END;
					
				HookEject:
					BEGIN
						quit := TRUE;
						NuLookupEntry := NLeject;
					END;
				OTHERWISE
					;
			END;
		END;
			
		IF ((itemHit = hookOK) OR (itemHit = hookCancel)) AND 
											(shouldHilite) THEN BEGIN
			GetDItem(theDialog, itemHit, itemKind, itemHandle, itemRect);
			HiliteControl(ControlHandle(itemHandle), 1);
			Delay(buttonHiliteDelay, fooTicks);		{ 	see ListItems.p for explanation of buttonHiliteDelay }
			HiliteControl(ControlHandle(itemHandle), 0);
		END;
		
	END; {with}
	
	{ We're out of here - start cleaning up } 
	FillReply(theData);
	
	{This needs to be done before Cleanup - userData needs to be accessible to hookProc}
	itemHit := StandardHook(hookPostflight, theData);		{	 	this WILL call user hookProc			}
	
	HUnlock(Handle(theData));

1:
	NuLookupCleanUp(theData);		{ Dispose the world }
	SetPort(savedPort);				{ Set the port back like a nice routine}
	InitCursor;						{ back to reality Jim }
END; {NuLookupEntry}


{ 
	CheckCell checks the selection in the specified list.  IF the cell is dimmed, the selection
		is turned off, and false is returned (i.e. no selection in the list)
}
FUNCTION CheckCell(theList : ListHandle) : BOOLEAN;
VAR
	theLItem	: ListItem;
	theCell		: Cell;
	
BEGIN
	CheckCell := FALSE;
	IF GetSortedSelection(theLItem, theList, theCell) THEN BEGIN
		IF theLItem.dimmed THEN 	{	It's dimmed - turn it off	}
			LSetSelect(FALSE,theCell,theList)
		ELSE
			CheckCell := true;
	END;
END;

{	Setup the NBP Lookup Buffers }

FUNCTION SetupBuffers(theData: DataH): OSErr;
VAR
	err			: OSErr;
	tempHandle	: Handle;
	tempSize	: Size;
	tempValue	: LONGINT;
	maxAvail	: LONGINT;

BEGIN
	err := noErr;
	
	WITH theData^^, parms^^ DO BEGIN
		{	sanity check on NBPLimit }
		tempValue := MaxEntities;		
		IF NBPLimit > tempValue THEN
			NBPLimit := tempValue;
			
		IF NBPLimit < entityLowerLimit THEN
			NBPLimit := entityLowerLimit;

		
		tempSize := (SIZEOF(EntityName) + kEntityHead) * NBPLimit;	{ get new size }

		tempHandle := theBuffer;
				
		{ Buffer is already allocated.  Try to resize	}
		IF tempHandle <> NIL THEN BEGIN
			HUnlock(tempHandle);
			SetHandleSize(tempHandle, tempSize);
			
			{ Can't resize -> destroy & make a new one	}
			IF MemError <> noErr THEN BEGIN
				DisposHandle(tempHandle);				
				tempHandle := NIL;							
			END;
		END;
		
		{ Make a new buffer								}
		IF tempHandle = NIL THEN BEGIN
			maxAvail := MaxBlock;
			
			{ calc even multiples	}
			tempValue := maxAvail DIV (SIZEOF(EntityName) + kEntityHead);
			
			IF (tempValue < entityLowerLimit) THEN						{ no space			}
				NBPLimit := 0;
			
			IF (tempValue >= entityLowerLimit) THEN BEGIN
			
				{ Try to get the best size possible	}
				IF (maxAvail < tempSize) THEN BEGIN
					NBPLimit := tempValue;
					tempSize := (SIZEOF(EntityName) + kEntityHead) * tempValue;
				END;
				
				tempHandle := NewHandleClear(tempSize);					{ Why is this a clear?	}
				err := MemError;
			END;				

		END;
		
		IF tempHandle <> NIL THEN BEGIN
			err := 0;											{	no errors to report										}
			myBufferSize := GetHandleSize(tempHandle);
			theBuffer := tempHandle;
			HLock(theBuffer);									{ lock it down }
			
			WITH thePBptr^ DO BEGIN
				retBuffPtr := theBuffer^;
				retBuffSize := myBufferSize;
				maxToGet := NBPLimit;
			END;			
		END;
		
		IF tempHandle = NIL THEN BEGIN			{ better code than if-then-else	}
			myBufferSize := 0;
			theBuffer := NIL;
			
			WITH thePBptr^ DO BEGIN
				retBuffPtr := NIL;
				retBuffSize := 0;
				maxToGet := 0;
			END;
		END;
		
	END; {with}
	
	SetupBuffers := err;
END;

FUNCTION NuLookupPreflight(where : Point; VAR prompt : Str255; _numTypes : INTEGER;
				   typeList: NLTypePtr; _nameFilter : ProcPtr; _zoneFilter : ProcPtr;
				   _hookProc : ProcPtr; _userData : LONGINT; dialogID : INTEGER;
				   _filterProc : ProcPtr; VAR theReply : NBPReply ; VAR theData : DataH)
				   	: INTEGER;
VAR
	theParms	: ParmH;									{ 	Timeout/Retry parameters 	}
	tempPtr		: Ptr;
	result 		: INTEGER;
	listsH		: SecretListHandle;
	hInfo		: Handle;									{ to 'flst' stuff	}
	oldResRef	: INTEGER;

LABEL
	1;														{	exit code					}
	
BEGIN
	NuLookupPreflight := nlEject;							{ 	Pessimism					}
	
	theData := DataH(NewHandleClear(SIZEOF(DataR)));		{ 	Get Data Record 			}
	IF theData = nil THEN 
		Goto 1;

	theParms := ParmH(NewHandleClear(SIZEOF(ParmData)));	{	get a default parm block				}
	IF (theParms = nil) THEN
		Goto 1;
	
	WITH theParms^^ DO BEGIN
		maxlim := entityLowerLimit;							{ add this many at a time	}
		retryCount := 8;
		retryInterval := 8;
		
		result := MaxEntities;	
		
		NBPLimit := 32;
		IF NBPLimit > result THEN
			NBPLimit := result;
			
		IF NBPLimit < entityLowerLimit THEN
			NBPLimit := entityLowerLimit;
		
		AgeLimit := 5;
	END;
	
	HLock(Handle(theData));
	
	WITH theData^^ DO BEGIN
		parms := theParms;
					
		{	setup secret list protocol to communicate information like
			is there a script manager, are we using icons in the list,
		}
		listsH := SecretListHandle(NewHandleClear(SIZEOF(SecretListRecord)));
		IF listsH = NIL THEN
			GOTO 1;
	
		HLock(Handle(listsH));
		listsH^^.dialog := dialog;
		
		WITH listsH^^ DO BEGIN
			
			{ We need to load the info from the 'flst'	}
			oldResRef := CurResFile;
			UseResFile(0);
			hInfo := Get1Resource(CTB_NuLookupParmsType, CTB_NuLookupParmsID);
			UseResFile(oldResRef);
			
			{ FALSE -> 'flst' resource					}
			IF (hInfo <> NIL) THEN
				IF GetIndflst(hInfo, FALSE, 1, theFontInfo) <> noErr THEN
					;
			
			hasSM := (NGetTrapAddress(_Unimplemented, OSTrap) = NGetTrapAddress(_ScriptUtil, ToolTrap));
			hasIcons := FALSE;									{ 	we will check in SetupDialog  }
			itemNo := hookItemList;
		END;
		
		HUnlock(Handle(listsH));
		iLists := listsH;
		
		IF HandToHand(Handle(listsH)) <> noErr THEN
			GOTO 1;
			
		listsH^^.itemNo := hookZoneList;
		zLists := listsH;
			
		{	setup the typelists; -1 is a wildcard }	
		IF _numTypes = -1 THEN BEGIN
			typeList := NLTypePtr(NewPtrClear(SIZEOF(NLType) div 4));	{	Need a fake typelist - one 
																			elt long					}
			IF typeList = nil THEN
				Goto 1;
			
			(*
			typeList^[0].hIcon := nil;			{ done by NewPtrClear	}
			*)
			typeList^[0].typeStr[0] := CHR(1);
			typeList^[0].typeStr[1] := CHR($3D);				{ equals	}
			numTypes := 1;
			fake := true;
		END
		ELSE BEGIN
			numTypes := _numTypes;						{	use their type list			}
			fake := false;
		END;
		
		types := typeList;

		{ 	Fill in the parms 			}
		userData := _userData;						
		nameFilter := _nameFilter;
		zoneFilter := _zoneFilter;
		FilterProc := _FilterProc;
		HookProc := _HookProc;
		theRPtr := @theReply;
		CurrentNBP := -1;									{ 	No NBP request outstanding	}
		itemable := TRUE;
		
		searchString[0] := CHR(0);							{ 	Keyboard searching info		}
		lastKey := TickCount - GetDblTime*2;
	
	{	SETUP NBP BUFFERS }
	{	Setup the NBP PArameter BLocks }
		tempPtr := NewPtrClear(SIZEOF(MPPParamBlock));			{	Setup NBP Parameter Block								}
		IF tempPtr = nil THEN
			Goto 1;
			
		thePBptr := MPPpbPtr(tempPtr);
	
		tempPtr := NewPtrClear(SIZEOF(EntityName));
		IF tempPtr = nil THEN
			Goto 1;
			
		WITH thePBptr^ DO								{	These fields only need to be set once					}
		BEGIN
			entityPtr := tempPtr;
			{ Done by NewPtrClear	}
			(*
			retBuffPtr := NIL;									{ 	filled in below }
			retBuffSize := 0;									{	filled in below }
			maxToGet := 0;										{	filled in below }
			*)
			interval := parms^^.retryInterval;		{We don't really need to fill this in }
		END;	
			
		result := SetupBuffers(theData);
		IF result <> noErr THEN
			Goto 1;
	
	{	Miscellaneous setup }
		zonable := false;					{Force CheckZone to look for the zones}	
		CheckZone(theData);

		{	setup the rest of the dialog }
		result := SetUpDialog(theData, prompt, dialogID);		{ 	get dialog, lists and 
																	setup userItems 			}
		IF result <> nlOK THEN
			GOTO 1;
			
		IF zonable THEN
			SetupZones(theData);
		
		SetUpItems(theData);
			
		FillReply(theData);							{Select whatever was passed in if possible}
	
		result := StandardHook(hookPreflight, theData);			{ 	this WILL call user hookProc			}
		IF (result = HookEject)  THEN 							{ 	Bail if preflight tells you to eject	}
			Goto 1;
	
		MoveWindow(dialog, where.h, where.v, TRUE);	{	Get window to specified spot			}
		ShowWindow(dialog);
		
		fUserList := FALSE;
		activeList := BuildFocusList(theData);
		
		ChangeFocus(theData, hookItemList, TRUE);
		isZoning := FALSE;
		
{$IFC NOT PRE70}
		TypeSelectClear(tsr);								{ clear the type ahead struct	}
{$ENDC}

	END; {with}
	
	HUnlock(Handle(theData));
	
	StartNextProcess(theData);								{	Start up async search					}

	NuLookupPreflight := nlOk;
1:
	;
END;


{$IFC RESIZE	}
{ Tries to rearrange the dialog if no zones are available		}
{ Must be called after the dialog exists, and before the lists	}
{ are created													}
PROCEDURE ResizeDialog(theData: DataH);
VAR
	newRect,
	sexRect,
	zoneLRect	: Rect;
	numItems	: INTEGER;		{ # items in the dialog	}
	i			: INTEGER;		{ loop counter			}
	coastClear	: BOOLEAN;
	itemKind	: INTEGER;
	itemHandle	: Handle;
	itemRect	: Rect;
	
BEGIN
	WITH theData^^ DO BEGIN
		
		GetItemRect(dialog, hookItemList, newRect);
		GetItemRect(dialog, hookZoneList, zoneLRect);
		
		{ Is everything the way we expect it	}
		IF (zoneLRect.left < newRect.left) THEN BEGIN
			newRect := dialog^.portRect;
			newRect.left := zoneLRect.right;	{ the proposed rect		}
			
			numItems := CountDITL(dialog) + 1;			
			coastClear := TRUE;
						
			{ For the other items, make sure these items do not get obscured	}
			FOR i:= 1 TO numItems DO BEGIN
				IF (i in [hookOK, hookCancel, hookOutline, hookTitle]) THEN BEGIN
					GetItemRect(dialog, i, itemRect);
					
					IF NOT SectRect(itemRect, newRect, sexRect) THEN
						EXIT(ResizeDialog)
					ELSE IF NOT EqualRect(itemRect, sexRect) THEN BEGIN
						EXIT(ResizeDialog);
					END;
				END;
				
			END;
			
			{ Rearrange the DITL				}
			IF coastClear THEN BEGIN
				{ Hide the old stuff	}
				HideDItem(dialog,hookZoneTitle);
				HideDItem(dialog,hookLine);
				HideDItem(dialog,hookZoneList);	
				
				{ Move the version string	}
				GetDItem(dialog, hookVersion, itemKind, itemHandle, itemRect);
				OffsetRect(itemRect,newRect.left - dialog^.portRect.left,0);
				SetDItem(dialog, hookVersion, itemKind, itemHandle, itemRect);
				
				{ Size the window				}
				{ Yuch!! This is truly skanky	}
				dialog^.portRect := newRect;
			END;
				
		END; { zones are to the left	}
			
	END; { with }
END;
{$ENDC RESIZE	}


PROCEDURE CheckZone(theData : DataH);
VAR
	result 		: INTEGER;
	bridge,
	net			: INTEGER;				{ Dummy vars; all we want is the boolean	}
	
BEGIN	
	WITH theData^^,theRPtr^.theEntity  DO BEGIN
		zonable := ZonesExist(bridge, net, result);			{	Get Zone information			}
		
		IF zonable THEN BEGIN
			result := GetZoneName(homeZone);
			
			IF (LENGTH(zoneStr) = 0) OR
					((LENGTH(zoneStr) = 1) AND (ORD(zoneStr[1]) = $2A)) THEN
				PCopyString(homeZone,zoneStr);			
		END;
		
		IF NOT zonable THEN BEGIN			{	 No Zones - use * for zone name }
			zoneStr[0] := CHR(1);
			zoneStr[1] := CHR($2A);
		END;
	
	END; {with}
END;

{	cleanup after ourselves 
	This involes terminating all outstanding NBP lookups
	deallocating lists and data structures allocated
	and the dialog
}
PROCEDURE NuLookupCleanup(theData : DataH);
BEGIN
	IF theData <> nil THEN BEGIN
		HLock(Handle(theData));
		
		{ Only DisposHandle check for NIL right now	}
		
		WITH theData^^ DO BEGIN
			IF thePBPtr <> nil THEN BEGIN
				StopProcess(theData);
				IF (thePBPtr^.entityPtr <> NIL) THEN
					DisposPtr(thePBPtr^.entityPtr);
				DisposPtr(Ptr(thePBPtr));			
			END;
			
			DisposHandle(Handle(iLists));
			DisposHandle(Handle(zLists));
				
			IF (fake) THEN
				IF (types <> NIL) THEN
					DisposPtr(Ptr(types));
			
			IF NOT fUserList THEN			{ Don't throw away the users	}
				IF (activeList <> NIL) THEN
					DisposPtr(activeList);
				
			DisposHandle(Handle(parms));
			
			IF ItemList <> nil THEN	
				LDispose(itemList);
			IF zoneList <> nil THEN 
				LDispose(zoneList);
				
			DisposHandle(theBuffer);	
			
			IF dialog <> nil THEN
				DisposDialog(dialog);	
		END;		
		
		HUnlock(Handle(theData));
		DisposHandle(Handle(theData));
	END;
	
	HPurge(Handle(GetPicture(CTB_NulookupID)));		{ clean up version PICT	}
END;

{ Fill in the current selections 	}
{ Assumes theData is LOCKED			}

PROCEDURE FillReply(theData : DataH);
VAR
	theCell 	: Cell;
	myItem 		: ListItem;
	len 		: INTEGER;
	
BEGIN
	WITH theData^^,theRPtr^  DO BEGIN
	
		{Default to nothing on}
		LONGINT(theAddr) := 0;

		theEntity.objStr[0] := CHR(0);
		theEntity.typeStr[0] := CHR(0);
		theEntity.zoneStr[0] := CHR(0);
	
		IF zonable THEN BEGIN
			LONGINT(theCell) := 0;
			
			{Start WITH the zone}
			IF LGetSelect(TRUE,theCell,zoneList) THEN BEGIN	
				len := SIZEOF (ListItem);
				LGetCell(PTR(@myItem),len,theCell,zoneList);	{For the zonelist - objStr is the zone name}
				PCopyString(myItem.objStr,theEntity.zoneStr);
			END;
			
		END
		ELSE BEGIN
			theEntity.zoneStr[0] := CHR(1);
			theEntity.zoneStr[1] := CHR($2A);
		END;
	
		
		LONGINT(theCell) := 0;										{THEN fill in the item}
		IF LGetSelect(TRUE,theCell,itemList) THEN BEGIN
			len := SIZEOF (ListItem);
			LGetCell(Ptr(@myItem),len,theCell,itemList);
			PCopyString(myItem.objStr,theEntity.objStr);
			PCopyString(myItem.typeStr,theEntity.typeStr);
			theAddr := myItem.Addr;
		END;
		
	END; {with}
END;

{$IFC PRE70}
{Transmogrify events }
FUNCTION StandardFilter(theDialog:DialogPtr; VAR theEvent: EventRecord; 
						 VAR itemHit: INTEGER): Boolean;	
VAR
	theData: 	DataH;
	theCell: 	Point;
	theChar: 	CHAR;
	result: 	BOOLEAN;
	oldItem:	INTEGER;
	where	: Point;
	hitItem: INTEGER;
	savedPort: GrafPtr;
	theWindow: WindowPtr;
	theFocus:	INTEGER;


BEGIN
	theData := DataH(GetWRefCon(theDialog));
	itemHit := 0;											{ nothing hit (yet) }
	
	result := FALSE;
	
	WITH theData^^ DO BEGIN
		evtWhere := theEvent.where;							{ Store info needed later}
		GlobalToLocal(evtWhere);							{ Does not move mem}
		modifiers := theEvent.modifiers;
		when := theEvent.when;
	
		CASE theEvent.what OF
			keyDown, autoKey:
				BEGIN
					theChar := CHAR(BAND(theEvent.message, charCodeMask));
					
					{ Use the Script Happy CmdPeriod() from Tech Notes	}
					IF CmdPeriod(theEvent) THEN BEGIN
						itemHit := hookCancel;
						shouldHilite := TRUE;		{ let's hilite the OK button… }
						result := TRUE;
					END
					ELSE
						CASE theChar OF
							CHR($03),
							CHR($0D):		{Enter/Return}
								BEGIN
									result := TRUE;
									{ IF something is selected THEN we can return the OK button }
									LONGINT(theCell) := 0;
									IF LGetSelect(TRUE, theCell, itemList) THEN BEGIN
										itemHit := hookOK;
										shouldHilite := TRUE;		{ let's hilite the OK button… }
									END
									ELSE 
										itemHit := hookNull;
								END;
									
							CHR($1B):			{ esc	}
								BEGIN
									itemHit := hookCancel;
									shouldHilite := TRUE;		{ let's hilite the OK button… }
									result := TRUE;
								END;
								
							CHR($09):			{ tab! }
								BEGIN
									result := TRUE;
									ItemHit := hookKeyBase + INTEGER(theChar);
								END;
								
							OTHERWISE
								IF NOT FocusIsTEItem(theData) THEN BEGIN
									result := TRUE;
									ItemHit := hookKeyBase + INTEGER(theChar);
								END;
								
						END; {case}		
		
				END; { keydown	}
				
			activateEvt:
				;
				
			mouseDown, 
			mouseUp:										{ Don't eat mousedown or activateevents }
				HideFocusUserItem(theData);					{ reset rectangle to 0000 }
	
			updateEvt: 	{ This is here to let our updates through }
				BEGIN	{ here we eat other windows' update events }

					theWindow := DialogPtr(theEvent.message);
					
					IF theWindow <> theDialog THEN BEGIN
						result := true;
						itemHit := hookNull;
					END
					ELSE
						{ false to do a TEDeactivate if focus is not editText item }				
						IF (GetItemKind(theDialog, focus) <> editText) THEN BEGIN
							ChangeFocus(theData, focus, FALSE);
							SetupFocusUserItem(theData);		{ setup the rectangle }				
							DrawFocus(theDialog, hookFocusItem);
							HideFocusUserItem(theData);
						END;
				END;
				
			nullEvent:
				BEGIN
					itemHit := FindDItem(theDialog, evtWhere)+1;
					result := true;
					itemHit := hookNull;
				END;
				
			OTHERWISE
				BEGIN
					result := true;
					itemHit := hookNull;
				END;
				
		END; {case}
	
		{Call users filterProc}
		IF (filterProc <> nil) THEN BEGIN
			oldItem := itemHit;
			StandardFilter := (CallDFilter(filterproc, theDialog, 
													TheEvent, ItemHit) OR result);
			
			{ Hilite controls if app has set the hooks	}
			IF theEvent.what = mouseDown THEN BEGIN	
				where := theEvent.where;				
				GlobalToLocal(where);
				hitItem := FindDItem(theDialog, where) + 1;
				
				IF (hitItem <> hookOK) AND (itemHit = hookOK) THEN
					shouldHilite := TRUE;
				IF (hitItem <> hookCancel) AND (itemHit = hookCancel) THEN
					shouldHilite := TRUE;
			END
			ELSE BEGIN										
				IF (ItemHit = hookOK) THEN
					shouldHilite := TRUE;
				IF (ItemHit = hookCancel) THEN
					shouldHilite := TRUE;
			END;
		END
		ELSE
			StandardFilter := result;	
	
		{ take care of clicks in editText fields }
		IF theEvent.what = mouseDown THEN BEGIN
			where := theEvent.where;							{ then we still need to hilite the controls				}
			GlobalToLocal(where);
			hitItem := FindDItem(theDialog, where) + 1;
			
			theFocus := focus;
			
			{ Need to activate if old wasn't and new is	}
			IF (GetItemKind(theDialog, theFocus) <> editText) THEN 
				IF(GetItemKind(theDialog, hitItem) = editText) THEN
					TEActivate(DialogPeek(theDialog)^.textH);
		END; { if theEvent.what is mouseDown }
	
	END; {with}

END; {standardFilter}

{$ELSEC}

{Transmogrify events }
FUNCTION StandardFilter(theDialog:DialogPtr; VAR theEvent: EventRecord; 
						 					VAR itemHit: INTEGER): BOOLEAN;	
VAR
	theData		: DataH;
	theChar		: CHAR;
	result		: BOOLEAN;
	where		: Point;
	hitItem		: INTEGER;
	theFocus	: INTEGER;

LABEL
	1;

BEGIN
	theData := DataH(GetWRefCon(theDialog));
	itemHit := 0;											{ nothing hit (yet) }
	result := FALSE;
	
	WITH theData^^ DO BEGIN		
		myEvent := theEvent;								{ used elsewhere	}
		
		{ Call the app's filter proc	}
		IF (filterProc <> NIL) THEN
			result := CallDFilter(filterProc, theDialog, theEvent, itemHit);
		IF result THEN										{ exit if the app eats it	}
			GOTO 1;
		
		{ Ask Dialog Mgr to take care of OK, etc	}
		result := StdFilterProc(theDialog, theEvent, itemHit);		
		IF result THEN
			GOTO 1;
		
		CASE theEvent.what OF
			keyDown, autoKey:
				BEGIN
					theChar := CHAR(BAND(theEvent.message, charCodeMask));
					IF (theChar = CHR($09))	OR (NOT FocusIsTEItem(theData)) THEN BEGIN
						result := TRUE;
						itemHit := hookKeyBase + INTEGER(theChar);
					END;
				END; { keydown	}
			
			updateEvt:
					{ false to do a TEDeactivate if focus is not editText item }
					IF (theDialog = WindowPtr(theEvent.message)) THEN BEGIN
						IF (GetItemKind(theDialog, focus) <> editText) THEN BEGIN
							ChangeFocus(theData, focus, FALSE);
							SetupFocusUserItem(theData);		{ setup the rectangle }				
							DrawFocus(theDialog, hookFocusItem);
							HideFocusUserItem(theData);
						END;
					END
					ELSE BEGIN
						{ Update Flood: if the window behind doesn't handle the	}
						{ update event ModalDialog won't return so we make it 	}
						{ a null and let StdFilterProc take a stab at it		}
						theEvent.what := nullEvent;
						result := StdFilterProc(theDialog, theEvent, itemHit);
						result := TRUE;
						itemHit := hookNull;
					END;							
				
			activateEvt:
				;
				
			mouseDown, 
			mouseUp:			
				BEGIN
					TypeSelectClear(tsr);
					HideFocusUserItem(theData);					{ reset rectangle to 0000 }
										
					IF NOT result THEN BEGIN
						{ take care of clicks in editText fields }
						where := theEvent.where;
						GlobalToLocal(where);
						hitItem := FindDItem(theDialog, where) + 1;
						
						theFocus := focus;
						
						{ Need to activate if old wasn't and new is	}
						IF (GetItemKind(theDialog, theFocus) <> editText) THEN 
							IF(GetItemKind(theDialog, hitItem) = editText) THEN
								TEActivate(DialogPeek(theDialog)^.textH);
					END;
				END; {mousedown}
				
			OTHERWISE
				BEGIN
					result := true;
					itemHit := hookNull;			{ gives time to do item lookup	}
				END; {null}
				
		END; {case}

1:
		StandardFilter := result;	
	
	END; {with}

END; {standardFilter}

{$ENDC}



{Deal with keystrokes, user items, pseudo items}
FUNCTION StandardHook(theItem : INTEGER; theData : DataH) : INTEGER;
VAR 
	theChar: 	CHAR;
	result:		INTEGER;
	theFocus:	INTEGER;
	theDialog:	DialogPtr;
	eaten:		BOOLEAN;	
	theList:	ListHandle;
	toScroll:	INTEGER;
	direction	: Boolean;
	
BEGIN
	theList := NIL;

	WITH theData^^ DO BEGIN
		theFocus := focus;
		theDialog := dialog;
		
		IF theFocus = hookItemList THEN
			theList := itemList;
			
		IF theFocus = hookZoneList THEN
			theList := zoneList;
			
		IF hookProc <> nil THEN										{ Call the user's hookproc }
			theItem := CallHook(hookProc, theItem, dialog);
	
		StandardHook :=  theItem;
		
		IF theItem > hookKeyBase THEN BEGIN
		
			theChar := CHAR(theItem - HookKeyBase);
			eaten := TRUE;
			
			CASE theChar OF
				CHR($03),												{ CR or ETX	}
				CHR($0D),
				CHR($1C),												{ right	}
				CHR($1D):												{ left 	}
					;
					
				CHR($09):												{tab!	}
					IF BAND(myEvent.modifiers, shiftKey) = 0 THEN
						GotoNextFocus(theData)
					ELSE
						GotoPrevFocus(theData);
				
				CHR($1E),												{ up	}
				CHR($1F):												{ down	}
					BEGIN
						direction := GetPrevThingy;
						IF theChar = CHR($1F) THEN
							direction := GetNextThingy;
							
						IF theFocus = hookItemList THEN
							DoChangeItem(theData, direction);
							
						IF theFocus = hookZoneList THEN
							DoChangeZone(theData, direction);
					END;

				CHR($01),												{ home }
				CHR($04):												{ end }
					BEGIN
						toScroll := theList^^.dataBounds.bottom;
						IF theChar = CHR($01) THEN
							toScroll := -toScroll;						{ going home }
							
						IF theList <> NIL THEN
							LScroll(0, toScroll, theList);
					END;
					
				CHR($0B),												{ page up }
				CHR($0C):												{ page down }
					IF theList <> NIL THEN BEGIN
						WITH theList^^ DO
							toScroll := (rView.bottom - rView.top) DIV cellSize.v;

						IF theChar = CHR($0B) THEN
							toScroll := -toScroll;					{ going up }
							
						LScroll(0, toScroll, theList);
					END;
					
				OTHERWISE
						eaten := FALSE;
				END; {case}
{$IFC PRE70}
				IF NOT eaten THEN BEGIN
					DoMoreSearch(theData,theChar);						{Some other key}
					result := StandardHook(theFocus, theData);
				END;
{$ELSEC}
				IF eaten THEN
					TypeSelectClear(tsr)				
				ELSE BEGIN
					DoMoreSearch(theData);						{Some other key}
					result := StandardHook(theFocus, theData);
				END;
{$ENDC}				
		END
		ELSE 
		BEGIN
			CASE theItem OF
				hookVersion:;
	
				hookNull:	{ if we're zoning then wait until time's up	}
					IF (isZoning) AND
								((TickCount - lastKey) > kLookupDelay) THEN BEGIN
						result := StandardHook(hookItemRefresh, theData);
						lastKey := TickCount;
						isZoning := FALSE;		{ clear the delay flag	}
					END;
	
				hookItemRefresh:
					BEGIN
						StopProcess(theData);		{Stop the current search}
						SetupItems(theData);		{Fill in the list from scratch}
						StartNextProcess(theData);	{Start it up again}
					END;
					
				hookZoneRefresh:
					IF zonable THEN BEGIN
						StopProcess(theData);	{Stop the current search}
						SetupZones(theData);	{Find the zone lists again}
						result := StandardHook(hookItemRefresh, theData);
					END;
			END; {Case}
	
			IF (theItem = hookItemList) OR (theItem = hookZoneList) OR 
							(GetItemKind(theDialog, theItem) = editText) THEN
				ChangeFocus(theData, theItem, TRUE);
				
		END;
		
	END; {with}
END;


{	useritem procedures }
PROCEDURE DrawList(theDialog : DialogPtr; itemNo : INTEGER);
VAR
	itemRect	: Rect;
	theList		: ListHandle;
	theData		: DataH;
	
BEGIN
	theData := DataH(GetWRefCon(theDialog));

	GetItemRect(theDialog, itemNo, itemRect);
	
	IF itemNo = hookItemList THEN
		theList := theData^^.itemList
	ELSE
		theList := theData^^.zoneList;
	
	IF theList <> nil THEN BEGIN
		EraseRect(theList^^.rView);
		LUpdate(theDialog^.visRgn, theList);
		FrameRect(itemRect);
	end;
END;

PROCEDURE DrawLine(theDialog : DialogPtr; itemNo : INTEGER);
VAR
	itemRect : Rect;
	
BEGIN
	GetItemRect(theDialog, itemNo, itemRect);
	FrameRect(itemRect);
END;

{$IFC NOT PRE70}
{ User Item Proc Stub	}
PROCEDURE DrawNothing(theDialog : DialogPtr; itemNo : INTEGER);	
BEGIN
END;
{$ENDC}

{ Creates either the zone list or the item list		}
{ Assumes the port is valid							}

FUNCTION CreateList(theData: DataH; whichList: INTEGER): ListHandle;
VAR
	savedText	: TextState;
	info		: FontInfo;
	cSize		: Point;
	dataBounds	: Rect;
	theList		: ListHandle;
	i			: INTEGER;
	rView		: Rect;
	itemKind	: INTEGER;
	itemHandle	: Handle;
	itemRect	: Rect;
	aLists		: SecretListHandle;
	
BEGIN	
	WITH theData^^ DO BEGIN
		theList := NIL;
		
		IF (whichList = hookItemList) THEN
			aLists := iLists
		ELSE
			aLists := zLists;
		
		GetTextState(savedText);						{ store for later		}
		SetTextState(aLists^^.theFontInfo);
		
		GetFontInfo(info);
	
		SetTextState(savedText);						{ reset to saved state	}
	
		{ Set up Item List }
		GetDItem(dialog, whichList, itemKind, itemHandle, itemRect);
		SetRect(dataBounds, 0, 0, 1, 0);
	
		{ set up cell size }
		WITH itemRect DO
			cSize.h := right - left - kScrollBarInset;
			
		WITH info DO
			cSize.v := ascent + descent + leading;
		
		{ Handle icons for the Item List	}
		IF (whichList = hookItemList) THEN
			WITH iLists^^ DO BEGIN
				hasIcons := FALSE;
				
				FOR i := 0 TO numTypes - 1 DO
					IF types^[i].hIcon <> NIL THEN BEGIN
						hasIcons := TRUE;
						
						{ leave enough room for icons 	}
						IF (cSize.v < kCellWithIcon) THEN
							cSize.v := kCellWithIcon;
						LEAVE;
					END;
			END;
	
		{	set up list viewing bounds }
		rView := itemRect;
		InsetRect(rView, 1, 1);
		
		{ Make the rect an even multiple of the cellHeight	}
		WITH rView DO BEGIN
			right := right - kScrollBarInset;
			bottom := top + (((bottom - top) DIV cSize.v) * cSize.v);
			itemRect.bottom := bottom + 1;
		END;
		
		SetDItem(dialog, whichList, itemKind, itemHandle, itemRect);
		
		theList := LNew(rView, dataBounds, cSize, CTB_NulookupLDEFID, dialog,
														TRUE, FALSE, FALSE, TRUE);
			
		IF (theList <> NIL) THEN BEGIN
			theList^^.selFlags := lOnlyOne;
			SetDItem(dialog, whichList, itemKind, @DrawList, itemRect);	
		END;
	
	END; {with}
		
	CreateList := theList;
END;


FUNCTION SetupDialog(theData : DataH; VAR prompt : Str255; dialogID : INTEGER) : INTEGER;
VAR
	itemHandle 		: Handle;
	itemRect 		: Rect;
	itemKind 		: INTEGER;
	theList 		: ListHandle;
	theDialog	 	: DialogPtr;	

BEGIN
	SetupDialog := nlOk;
	
	theDialog := GetNewDialog(dialogID, NIL, Pointer(-1));
	IF theDialog = nil THEN BEGIN
		SetupDialog := nlEject;
		EXIT (SetupDialog);
	END;
	
	SetPort(theDialog);

	SetWRefCon(theDialog, LONGINT(theData));		{ Store global into the dialogs refcon }
	theData^^.dialog := theDialog;					{ Fill in the fields }
	
{$IFC RESIZE	}
	{ If we can, we'll grow the item list 	}
	IF NOT theData^^.zonable AND (dialogID = CTB_NulookupID) THEN
		ResizeDialog(theData);
{$ENDC RESIZE	}		
	
	{ Create the ITEM List	}
	theList := CreateList(theData, hookItemList);
	
	IF theList = NIL THEN BEGIN
		SetupDialog := nlEject;
		EXIT(SetupDialog);
	END
	ELSE BEGIN
		theList^^.refCon := LONGINT(theData^^.iLists);
		theData^^.itemList := theList;
	END;
	
	{ Set up Zone List }
	theList := CreateList(theData, hookZoneList);
	
	IF theList = NIL THEN BEGIN
		SetupDialog := nlEject;
		EXIT(SetupDialog);
	END
	ELSE BEGIN
		theList^^.refCon := LONGINT(theData^^.zLists);
		theData^^.zoneList := theList;
	END;
	
	SetUserItemProc(theDialog, hookLine, ProcPtr(@DrawLine));
	
{$IFC PRE70}
	SetUserItemProc(theDialog, hookOutline, ProcPtr(@DoSolidOutline));
{$ELSEC}
	{ we need to tell the StdFilterProc to handle the default and cancel buttons }
	IF SetDialogDefaultItem(theDialog, hookOK) <> noErr THEN;
	IF SetDialogCancelItem(theDialog, hookCancel) <> noErr THEN;
	SetUserItemProc(theDialog, hookOutline, ProcPtr(@DrawNothing));
{$ENDC}

	{ hookVersion has already been published as a userItem, so we	}
	{ make the hookVersion item a PICT & use DialogMgr to save code	}
	
	GetDItem(theDialog, hookVersion, itemKind, itemHandle, itemRect);
	itemHandle := Handle(GetPicture(CTB_NulookupID));
	HNoPurge(itemHandle);
	
	WITH PicHandle(itemHandle)^^.picFrame DO BEGIN
		itemRect.right := itemRect.left + right - left;	{ don't scale		}
		itemRect.bottom := itemRect.top + bottom - top;
	END;
	SetDItem(theDialog, hookVersion, picItem, itemHandle, itemRect);
		
	GetDItem(theDialog, hookTitle, itemKind, itemHandle, itemRect);
	SetIText(itemHandle, prompt);		
END;



PROCEDURE SetupZones(theData : DataH);
VAR
	itemHandle 	: Handle;
	itemRect 	: Rect;
	itemKind 	: INTEGER;
	itemNo 		: INTEGER;
	theCell		: Point;	
	result 		: OSErr;
	myItem 		: ListItem;
	noGo		: Boolean;
	
BEGIN	
	WITH theData^^ DO BEGIN
		
		DoWatch;
		
		IF zonable THEN BEGIN
		
			result := ZonesToList(theData);		{ Create the list}
			
			IF result <> noErr THEN				
				zonable := FALSE;				{ Problem creating the list}
				
			IF result = noErr THEN BEGIN
				PCopyString(theRPtr^.theEntity.zoneStr,myItem.objStr);
				myItem.typeStr[0] := CHR(0);
				
				noGo := TRUE;
				
				IF SelectSortedItem(myItem, zoneList) THEN	{ 	try to select appropriate zone	}
					IF CheckCell(zoneList) THEN				{ 	Is the cell dimmed?				}
						noGo := FALSE;
	
				IF noGo THEN								{	no selection					}
					FillReply(theData);						{ just changed zone - update reply	}
				
				LAutoScroll(zoneList);						{	scroll into view				}
			END;
			
		END; { if zones	}
	
	END; {with}
		
	InitCursor;												{	and reset cursor				}
END;

PROCEDURE SetupItems(theData : DataH);
VAR
	
	result : OSErr;
		
	myItem : ListItem;
	i : INTEGER;
	theList : ListHandle;
	tempRect: Rect;
	
BEGIN
	DoWatch;
		
	WITH theData^^ DO
		IF itemable THEN BEGIN
			theList := itemList;
	
			LDelRow(0, 0, theList);			{ immediate clearing please }
			LDoDraw (FALSE, theList);		
			
			IF (LENGTH(theRPtr^.theEntity.zoneStr) > 0) THEN BEGIN	{Have they selected a zone?}
	
				FOR i := 0 to (numTypes - 1) DO BEGIN
					result := ItemsToList(theData, i);
				
					IF result = noErr THEN		{Problem with the list}
					BEGIN
						WITH theRPtr^.theEntity DO BEGIN
							PCopyString(objStr,myItem.objStr);
							PCopyString(typeStr,myItem.typeStr);
						END;
						
						IF SelectSortedItem(myItem, theList) THEN	{ try to select current item }
							IF CheckCell(theList) THEN				{ make sure it is not dimmed }
								;
					
						FillReply(theData);							{ update incase nothing was selected }
							
						LAutoScroll(theList);
						itemable := TRUE;
					END
					ELSE BEGIN
					{*******  FIX THIS SOMETIME ******}
						IF result <> extractErr THEN (* Extract Error OK????*)
						BEGIN
							{Not Open Error means appletalk not active}
							IF result <> notOpenErr THEN 
								;
	
					(*		theData^^.itemable := FALSE; ignore for now *)
						END;
					END;
				END;
			END; { good length	}
	
			LDoDraw(TRUE, theList);
			tempRect := theList^^.rView;
			invalRect(tempRect);
			
		END; { itemable	}
			
	InitCursor;	
END;

PROCEDURE CheckProcess(theData : DataH);
VAR
	i,j			: INTEGER;
	theAddr		: AddrBlock;
	myEntity	: EntityName;
	result 		: OSErr;
	theCell 	: Cell;
	located 	: BOOLEAN;
	theItem 	: ListItem;
	len 		: INTEGER;
	addit 		: Boolean;
	include 	: INTEGER;
	found 		: INTEGER;
	theList		: ListHandle;
	tempRect	: Rect;
	
	changed		: BOOLEAN;
	
	itemsToCheck: INTEGER;
	
BEGIN
	changed := FALSE;
	
	WITH theData^^, parms^^ DO
		IF itemable THEN BEGIN
			theList := itemList;
			
			found := thePBptr^.numGotten;
			IF (thePBptr^.numGotten > LastIChecked + maxLim) THEN
				found := LastIChecked + maxLim;				

			FOR i := LastIChecked + 1 TO found DO BEGIN

				result := NBPExtract(thePBptr^.retBuffPtr, NBPLimit,
															i, myEntity, theAddr);
							 
				IF result = noErr THEN BEGIN
					PCopyString(myEntity.objStr,theItem.objStr);
					PCopyString(myEntity.typeStr,theItem.typeStr);
				
					theItem.addr := theAddr;
					theItem.dimmed := false;
				
					{ Check so we don't flash the list	}
					IF NOT FindSortedItem(theItem, theList, theCell) THEN BEGIN	
						addit := (nameFilter = nil);
						IF not addit THEN BEGIN	
							PCopyString(theRPtr^.theEntity.zoneStr,myEntity.zoneStr);
							include := CallFilter(nameFilter,myEntity);
							addit := (include < nameReject);
							theItem.dimmed := (include = nameDisable);
						END;
							
						WITH theList^^.dataBounds DO
							IF ((bottom - top) >= maxint DIV SIZEOF(ListItem)) THEN			{ awooga awoooga					}
								addIt := FALSE;
	
						IF addit THEN BEGIN						
							LDoDraw(FALSE, theList);
							changed := TRUE;	{ list has changed }
							theItem.age := ageLimit;
							theItem.hIcon := types^[CurrentNBP].hIcon;

							AddSortedToList(theItem, theList);
						END;	
					END;
											
				END; { result = 0	}
				
			END; {for i = }

			LastIChecked := found;
			
			 { if finished then check for losers	}
			IF thePBptr^.ioResult <= 0 THEN BEGIN
				itemsToCheck := theList^^.dataBounds.bottom - 1;
				
				FOR j := itemsToCheck DOWNTO 0 DO BEGIN
					LDoDraw(FALSE, theList);				{ avoid flicker when we update ages and stuff }
					
					theCell.h := 0;
					theCell.v := j;
					
					len := SIZEOF(ListItem);
					LGetCell(Ptr(@theItem),len, theCell, theList);

					IF IUEqualString(theItem.typeStr,types^[currentNBP].typeStr) = 0 THEN BEGIN
						i := 1;
						located := FALSE;
						
						WHILE (i <= found) AND (NOT located) DO BEGIN
							result := NBPExtract(thePBptr^.retBuffPtr,NBPLimit,
																i,myEntity,theAddr);
							i := i + 1;
							IF result = noErr THEN
								IF IUEqualString(myEntity.objStr,theItem.objStr) = 0 THEN
									located := TRUE;
		
						END; {WHILE i <= found and not located }
							
						IF (NOT located) THEN BEGIN
							{ Age THEN delete the item IF too old}
							theItem.age := theItem.age - 1;
							IF (theItem.age <= 0) THEN BEGIN
								changed := TRUE;						{ list has changed }
								LDelRow(1, j, theList);					{delete the item }
							END
							ELSE
								LSetCell(ptr(@theItem),len,theCell, theList);
						END {IF not located}
						ELSE BEGIN 											{They have responded - set age to max}
							theItem.age := ageLimit;
							LSetCell(ptr(@theItem),len,theCell, theList);
						END;
						
					END; { typeStr match	}
					
				END; {for j }
				
				IF itemsToCheck >= 0 THEN		{ we turned off drawing to avoid flicker when we update ages and stuff }
					LDoDraw(TRUE, theList);		{ so turn it on again }
	
				result := NBPLimit;
				IF thePBptr^.numGotten >= result THEN BEGIN
					result := result * 2;
					NBPLimit := result;
					result := SetupBuffers(theData);
					IF result <> noErr THEN
						IF itemsToCheck <= 0 THEN
							itemable := FALSE;		{ I lost the edge and am turning in my wings }
				END;	
	
				StartNextProcess(theData); {Restart the Process}
			END; {the call completed }
			
			IF changed THEN					{ did we add/delete items? }
			BEGIN
				LAutoScroll(theList);		{ scroll selection back into view }
				LDoDraw(TRUE, theList);		{ turn drawing back on }
				tempRect := theList^^.rView;	
				InvalRect(tempRect);		{But, invalidate so will draw later}
			END;
		
		END; {Itemable}
END;

{	start another NBP asynchronous lookup }
PROCEDURE StartNextProcess(theData: DataH);
VAR
	result 		: OSErr;
	found  		: INTEGER;
	
BEGIN	
	WITH theData^^ DO
		IF itemable THEN BEGIN
			CurrentNBP := (CurrentNBP + 1) MOD numtypes;
			LastIChecked := 1;
			thePBptr^.numGotten := 0; 	{	This is the only important field to zero out 	}
										{	used in CheckProcess							}
			
			IF (LENGTH(theRPtr^.theEntity.zoneStr) > 0) THEN	{Is there a zone selected?}		
				result := __LookupName(theData, CurrentNBP, found, parms^^.retryCount, TRUE);
		END;
END;

{	terminal outstanding NBP lookups }
PROCEDURE StopProcess(theData : DataH);
VAR
	result		: OSErr;
	myPBptr		: MPPpbPtr;
	
BEGIN
	WITH theData^^ DO BEGIN
		myPBptr := MPPpbPtr(NewPtrClear(SIZEOF(MPPParamBlock)));
		
		IF MemError <> noErr THEN
			itemable := FALSE
		ELSE BEGIN
			IF itemable THEN
				IF thePBptr^.count <> 0 THEN
				BEGIN
					myPBptr^.nKillQEl := Ptr(thePBptr);
					result := PKillNBP(myPBptr, FALSE);
					CurrentNBP := -1; 	{ Nothing Outstanding		}
				END;
		END;
		
		IF (myPBPtr <> NIL) THEN
			DisposPtr(Ptr(myPBPtr));		{ DisposPtr does not checks for nil	yet}
	END;
END;


{	Get the next undimmed item starting at theCell, go through the list "step" cells at a time

	returns -1 in theCell if there is no more 
}
PROCEDURE GetUndimmed(theList : ListHandle; step : INTEGER; VAR theCell : cell ; VAR MyItem : ListItem);
VAR 
	maxIndex : INTEGER;
	len : INTEGER;
	
BEGIN
	theCell.v := theCell.v + step;
	maxIndex := theList^^.dataBounds.bottom - 1;
	myItem.dimmed := true;
	len := SIZEOF(ListItem);
	WHILE ((theCell.v <= MaxIndex) AND (theCell.v >= 0) AND (myItem.dimmed)) DO
	BEGIN
		LGetCell(PTR(@myItem),len,theCell,theList);
		IF MyItem.dimmed THEN 
			theCell.v := theCell.v + step
		ELSE
			Exit(GetUndimmed);								{ found it		}
	END;
	
	LONGINT(theCell) := -1;									{ no selection	}
END;



{---------------------------------------------------------------
	ChangeInList	-	Core stuff for navigating a list
	
		theList		-	Which list, eh
		direction	-	Up or Down
		
		returns		-	TRUE if the selection has changed
----------------------------------------------------------------}

FUNCTION ChangeInList(theList: ListHandle; direction: BOOLEAN): BOOLEAN;
VAR
	oldCell,
	theCell,
	tempPt		: Point;
	stepper		: INTEGER;
	cRect		: Rect;
	myItem		: ListItem;			{ unused		}

BEGIN
	ChangeInList := TRUE;
	
	LONGINT(oldCell) := 0;
		
	IF LGetSelect(TRUE, oldCell, theList) THEN
		theCell := oldCell
	ELSE BEGIN
		LONGINT(oldCell) := -1;		{ mark no original selection				}

		theCell.h := 0;
		IF direction = GetNextThingy THEN
			theCell.v :=  -1
		ELSE
			theCell.v := theList^^.dataBounds.bottom
	END;
	
	stepper := -1;
	IF direction = GetNextThingy THEN
		stepper := 1;
		
	GetUndimmed (theList, stepper, theCell, myItem);	

	{ We only change the selection if we have to		}
	IF LONGINT(theCell) = -1 THEN
		theCell := oldCell
	ELSE IF theCell.v >= 0 THEN BEGIN
		IF (oldCell.v >= 0) THEN
			LSetSelect(FALSE, oldCell, theList);
		
		LSetSelect(TRUE, theCell, theList);

		tempPt := oldCell;
		IF direction = GetPrevThingy THEN
			LONGINT(tempPt) := -1;				{ we only special case going DOWN on the list	}
			
		IF (LONGINT(tempPt) <> -1) THEN BEGIN	{ only if there was a previous selection		}
			LRect(cRect, tempPt, theList);
			
			IF theList^^.rView.bottom = cRect.bottom THEN	{ we are at the bottom 						}
				LScroll(0, 1, theList)			{ scroll one down please						}
			ELSE
				LONGINT(tempPt) := -1;			{ force LAutoScroll call						}
		END;
		
		IF (LONGINT(tempPt) = -1) THEN			{ only if there was no previous selection do we	}
			LAutoScroll(theList);				{ auto scroll }
		
	END;
	
	IF LONGINT(theCell) = LONGINT(oldCell) THEN	{ Has selection changed?		}
		ChangeInList := FALSE;
END;

{
	
	this routine handles user using the arrows to move one item up or down
	in the ZONE list
	
	properly handles cases of no current selection, dimmed items, etc.
	
	will stop outstanding NBP lookup and restart new one
}
PROCEDURE DoChangeZone(theData: DataH; direction: BOOLEAN);
VAR
	theItem: INTEGER;

BEGIN	
	WITH theData^^ DO
		IF zonable THEN BEGIN			
			IF NOT ChangeInList(zoneList, direction) THEN	{ selection has NOT changed			}
				theItem := StandardHook(hookZoneList, theData)	
			ELSE BEGIN										{ selection HAS changed				}
				FillReply(theData);							{ just changed zone - update reply	}
				isZoning := TRUE;
				lastKey := myEvent.when;							{ set the delay flag & timer}
				theItem := StandardHook(hookNull, theData);
			END;
		END;
END;

{	
	this routine handles user using the arrows to move one item up or down
	in the item list
	
	properly handles cases of no current selection, dimmed items, etc.
}
PROCEDURE DoChangeItem(theData : DataH; direction: BOOLEAN);
BEGIN	
	WITH theData^^ DO
		IF Itemable THEN
			IF ChangeInList(itemList, direction) THEN ;		{ selection changed??	}
END;

{
	this handles searching the item list for items that match characters
	typed in by the user.
	
	keystrokes must fall within DoubleClick time.  if more than 32 keystrokes
	are received, we start beeping.
	
	also, if the item that would ordinarily be selected is dimmed, we try to go to the next one.
}

{$IFC PRE70}

PROCEDURE DoMoreSearch(theData : DataH; theChar : CHAR);
VAR
	len			: INTEGER;
	softCell,
	theCell,
	oldCell		: Cell;
	myItem 		: ListItem;
	theList		: ListHandle;
	theItem		: INTEGER;				{ used when focus changes }

BEGIN	
	WITH theData^^ DO
		IF itemable THEN BEGIN
			IF focus = hookItemList THEN
				theList := itemList
			ELSE IF focus = hookZoneList THEN
				theList := zoneList
			ELSE
				Exit(DoMoreSearch);
	
			IF myEvent.when - lastKey > GetDblTime THEN	BEGIN							{ user is typing in a new search string }
				IF (theChar = CHR($08)) OR (theChar = CHR($7F)) THEN			{ BS/DEL }
					searchString[0] := CHR(0)
				ELSE BEGIN
					searchString[0] := CHR(1);
					searchString[1] := theChar;
				END;
			END
			ELSE BEGIN	{ user is continuing old search string	}
				IF (theChar = CHR($08)) OR (theChar = CHR($7F)) THEN BEGIN		{ BS/DEL }
					len := LENGTH(searchString) - 1;
					IF len < 0 THEN
						len := 0;
					searchString[0] := CHAR(len);	{ trim one please }
				END
				ELSE BEGIN
					len := LENGTH(searchString);
					IF (len < SIZEOF(Str32)) THEN BEGIN
						len := len + 1;
						searchString[0] := CHAR(len);
						searchString[len] := theChar;
					END
					ELSE
						SysBeep(5);													{ user has typed in too much so beep 	}
				END;
			END;
			
			{ Now, look for it }
			LONGINT(oldCell) := 0;
			IF NOT LGetSelect(TRUE, oldCell, theList) THEN
				LONGINT(oldCell) := -1;											{ no current selection					}
				
			PCopyString(searchString,myItem.objStr);
			myItem.typeStr[0] := CHR(0);
			
			IF FindSortedItem(myItem,  theList, theCell) THEN
				;																{ try and find what we are looking for	}

			IF theCell.v >= theList^^.dataBounds.bottom THEN				{ sanity check please					}
				theCell.v := theList^^.dataBounds.bottom - 1;
			
			IF NOT EqualPt(theCell, oldCell) THEN								{ did we find something?				}
			BEGIN
			
				IF theCell.v >= 0 THEN											{	there is a new selection			}
				BEGIN															{	but we need to see if it is dimmed	}
				
					len := SIZEOF(listItem);
					LGetCell(@myItem, len, theCell, theList);
					
					IF myItem.dimmed THEN BEGIN									{ item is dimmed so go on to next one 	}
						softCell := theCell;									{ save off cell we are trying to select	}
						
						GetUndimmed(theList, 1, theCell, myItem);				{ try and get the next one in list		}
						IF LONGINT(theCell) = -1 THEN BEGIN						{ no go going down, try UP 				}
							theCell := softCell;								{ save off cell we are trying to select	}
							GetUndimmed(theList, -1, theCell, myItem);			{ try and get the next one in list		}
						END;
					END;
				
					IF (theCell.v >= 0) THEN BEGIN
						IF (oldCell.v >= 0) THEN								{ there was an old selection and there	}
							LSetSelect(FALSE, oldCell, theList);				{ and there is a new selection			}
						LSetSelect(TRUE, theCell, theList);
					END;
						
					LAutoScroll(theList);										{ and make sure we are visible to all	}
				END;
				
				FillReply(theData);							{ just changed zone - update reply	}
				IF (focus = hookZoneList) THEN BEGIN
					isZoning := TRUE;						{ flag to delay lookups	}
					lastKey := myEvent.when;				{ timer on the lookup	}
					theItem := StandardHook(hookNull, theData);	{ null event		}
				END;
	
			END;
				
			lastKey := myEvent.when;								{ remember last keystroke time			}
		END; { if items	}	
END;

{$ELSEC}

PROCEDURE DoMoreSearch(theData : DataH);
VAR
	theList		: ListHandle;
	i			: INTEGER;
	len			: INTEGER;
	myItem		: ListItem;
	oldCell,
	theCell,
	softCell	: Cell;
	theItem		: INTEGER;
	maxLim		: INTEGER;

BEGIN
	WITH theData^^ DO
		IF (itemable) OR (zonable) THEN BEGIN
		
			IF focus = hookItemList THEN
				theList := itemList
			ELSE IF focus = hookZoneList THEN
				theList := zoneList
			ELSE
				Exit(DoMoreSearch);
				
			maxLim := theList^^.dataBounds.bottom - 1;
			
			LONGINT(oldCell) := 0;
			IF NOT LGetSelect(TRUE, oldCell, theList) THEN
				LONGINT(oldCell) := -1;
								
			IF TypeSelectNewKey(myEvent, tsr) THEN
			
				{ Walk thru the list to find the first match	}
				FOR i := 0 TO maxLim DO BEGIN
					theCell.h := 0;
					theCell.v := i;
					len := SIZEOF(ListItem);
					LGetCell(@myItem, len, theCell, theList);
					
					{ Select the new cell if we have a match or at the bottom of the list	}
					IF (TypeSelectCompare(tsr, tsr.tsrScript, @myItem.objStr) < 1) OR
																	(i = maxLim) THEN BEGIN
						
						{ if the item is dimmed, get the next below it or above it	}
						IF myItem.dimmed THEN BEGIN
							softCell := theCell;					{ save it	}
							
							GetUndimmed(theList, 1, theCell, myItem);			
							IF LONGINT(theCell) = -1 THEN BEGIN				
								theCell := softCell;								
								GetUndimmed(theList, -1, theCell, myItem);		
							END;
						END;
						
						IF (oldCell.v <> theCell.v) THEN BEGIN
							IF (oldCell.v >= 0) THEN
								LSetSelect(FALSE, oldCell, theList);
							LSetSelect(TRUE, theCell, theList);
							
							LAutoScroll(theList);
							
							{ now update the items for a change in zones	}
							FillReply(theData);						
							IF (focus = hookZoneList) THEN BEGIN
								isZoning := TRUE;		
								lastKey := myEvent.when;	
								theItem := StandardHook(hookNull, theData);
							END;
							
						END; {new selection}
						
						LEAVE;
					END; { have a match }
							
				END; {for every cell}
			
		END; {have items}
END;

{$ENDC}

{	set cursor to a watch }
PROCEDURE DoWatch;
VAR
	theCurs : CursHandle;
	
BEGIN
	theCurs := GetCursor(watchCursor);
	
	IF theCurs <> NIL THEN
		SetCursor(theCurs^^);
END;

{	
	this returns the maximum number of entities that can be returned from an NBP lookup response packet
	as well as the maximum number of cells that the list manager will be able to manage.  This places
	an upper bound on the # of NBP entities that can be returned from the NBP lookup…
	
	Typically, the upper bound will be controlled by the size of the NBP response buffer…
	
	And in reality, we can actually manage more items in the list (SIZEOF(ListItem) < SIZEOF(EntityName)+kEntityHead)
		that can be returned in a single NBP response buffer/packet.
		
	Someday, of course, NBP will be less brain dead and will allow multipacket responses
}
FUNCTION 	MaxEntities: INTEGER;
VAR
	result1: INTEGER;
	result2: INTEGER;
	
BEGIN
	result1 := kNBPMaxSize DIV (SIZEOF(NamesTableEntry));	{ 32767 is max NBP response size }
	result2 := maxint DIV (SIZEOF(ListItem));				{ list manager limitations }
	
	MaxEntities := MIN(result1, result2);
	
END;

{	causes version box to be redrawn }
PROCEDURE RedrawVersion(theData: DataH);
VAR
	itemRect: Rect;
	theDialog: DialogPtr;
	savedPort: GrafPtr;
	
BEGIN
	theDialog := theData^^.dialog;
	GetItemRect(theDialog, hookVersion, itemRect);
	
	GetPort(savedPort);
	SetPort(theDialog);
	InvalRect(itemRect);
	SetPort(savedPort);
END;

{	this routine returns the item type of the given item number in the dialog specified }
{	after stripping disabled bit }
FUNCTION GetItemKind(theDialog: DialogPtr; itemNo: INTEGER): INTEGER;
VAR
	itemKind : INTEGER;
	itemRect : Rect;
	itemHandle : Handle;

BEGIN
	GetItemKind := 0;											{ placeholder }
	
	IF (itemNo > CountDITL(theDialog) + 1) OR (itemNo < 1) THEN
		EXIT(GetItemKind);
	
	GetDItem(theDialog, itemNo, itemKind, itemHandle, itemRect);
	GetItemKind := BAND(itemKind, BNOT(itemDisable));
END;


PROCEDURE	GotoXXXXFocus(theData: DataH; nextOne: BOOLEAN);
VAR
	itemNo		: INTEGER;
	itemRect	: Rect;
	oldFocus,
	theFocus	: INTEGER;
	maxItems	: INTEGER;
	
BEGIN
	WITH theData^^ DO BEGIN
		theFocus := focus;
		{ Convert to an entry into the active list	}
		theFocus := GetFocusEntry(activeList, theFocus);
		IF (theFocus = -1) THEN
			EXIT(GotoXXXXFocus);
		
		oldFocus := theFocus;
		maxItems := GetMaxFocus(activeList);
		
		{ Search for the next valid, visible item	}
		REPEAT
			IF nextOne THEN
				theFocus := theFocus + 1
			ELSE
				theFocus := theFocus - 1;
				
			IF (theFocus > maxItems) THEN			{ sanity checking	}
				theFocus := 1;
				
			IF (theFocus < 1) THEN
				theFocus := maxItems;
			
			{ Don't focus on hidden ones	}
			IF (theFocus <> oldFocus) THEN BEGIN
				itemNo := GetFocusItem(activeList, theFocus);
				GetItemRect(dialog, itemNo, itemRect);
				
				IF (itemRect.left < 16384) THEN BEGIN
					ChangeFocus(theData, itemNo, FALSE);
					Exit(GotoXXXXFocus);
				END;
			END;
			
		UNTIL theFocus = oldFocus;
	END; {with}
END;

{	given current focus, go "forwards" }
PROCEDURE	GotoNextFocus(theData: DataH);
BEGIN
	GotoXXXXFocus(theData, TRUE);
END;

{	given current focus, go "backwards" }
PROCEDURE	GotoPrevFocus(theData: DataH);
BEGIN
	GotoXXXXFocus(theData, FALSE);
END;

{	is the current focus a te item? }
FUNCTION	FocusIsTEItem(theData: DataH): BOOLEAN;
BEGIN	
	WITH theData^^ DO
		FocusIsTEItem := (GetItemKind(dialog, focus) = editText);
END;

{ --------------------------------------------------------------
	DrawFocus	-	Draws the rect around each list given the
					focus user Item
		
		theDialog	Where I am
		itemNo		Focus User Item
--------------------------------------------------------------	}

PROCEDURE 	DrawFocus(theDialog: DialogPtr; itemNo: INTEGER);	
VAR
	itemRect	: Rect;
	savedPen	: PenState;
	qdWorld		: GrafWorldPtr;			{ QD globals				}
	
BEGIN
	GetPenState(savedPen);
	PenNormal;
	PenSize(kFocusWidth, kFocusWidth);
										{ Get the rect				}
	GetItemRect(theDialog,itemNo,itemRect);
	qdWorld := GetQDGlobals;
	PenPat(qdWorld^.black);				{ tuxedo junction			}
	
	FrameRect(itemRect);
	
	SetPenState(savedPen);				{ back in black				}
END;

{ --------------------------------------------------------------
	EraseFocus	-	Erases the rect around each list given the
					focus user Item.  We need to get the rect
					for each leg of the frame to erase.
		
		theDialog	Where I am
		itemNo		Focus User Item
--------------------------------------------------------------	}
PROCEDURE EraseFocus(theDialog: DialogPtr; itemNo: INTEGER);
VAR
	itemRect	: Rect;
	savedPen	: PenState;
	eraseIt		: Rect;
	
BEGIN
	GetPenState(savedPen);				{ save for later		}
	PenNormal;

	GetItemRect(theDialog, itemNo, itemRect);
	
	eraseIt := itemRect;
	
	{ I can't use FrameRect(white) 'cuz the backPat might be something else		}
	{ I can't do an EraseRect(itemRect), so I erase the 4 legs					}
	
	WITH eraseIt DO BEGIN
		bottom := top + kFocusWidth;	{ top 			}
		EraseRect(eraseIt);
	
		bottom := itemRect.bottom;		{ bottom 		}
		top := bottom - kFocusWidth;
		EraseRect(eraseIt);
	
		top := itemRect.top;			{ left			}
		right := left + kFocusWidth;
		EraseRect(eraseIt);
		
		right := itemRect.right;		{ right			}
		left := right - kFocusWidth;
		EraseRect(eraseIt);
	END;
	
	SetPenState(savedPen);
END;

{	redraw focus hilighting }
{	IF noSelect is FALSE, then we will not do a set select on the text edit item
	(should only be FALSE from tabbing operations
	
	also noSelect is FALSE from updateEvt processing  - this is to turn off
	text edit blinking and selection via TEDeactivate if the focus is not
	an editText item 
	
	otherwise should always be passed in TRUE for noSelect
}
PROCEDURE	ChangeFocus(theData: DataH; newFocus: INTEGER; noSelect: BOOLEAN);
VAR
	itemKind		: INTEGER;
	itemRect		: Rect;
	itemHandle		: Handle;
	theDialog		: DialogPtr;
	theFocus		: INTEGER;
	tempRect		: Rect;
	hTE				: TEHandle;	
	newIsEditText,
	oldWasEditText	: BOOLEAN;
	
BEGIN
	theDialog := theData^^.dialog;
	theFocus := theData^^.focus;
	hTE := DialogPeek(theDialog)^.textH;

	{	sanity chex }
	IF (newFocus > CountDITL(theDialog) + 1) THEN
		EXIT(ChangeFocus);
	
	{ Not in the active list	}
	IF GetFocusEntry(theData^^.activeList,newFocus) = -1 THEN
		EXIT(ChangeFocus);					
		
	oldWasEditText := (GetItemKind(theDialog, theFocus) = editText);
	newIsEditText := (GetItemKind(theDialog, newFocus) = editText);
	
{	turn off text edit if current focus is not text edit		}
{	(just to be sure)											}
	IF NOT oldWasEditText THEN
		IF NOT ((noSelect) AND (newIsEditText)) THEN
			TEDeactivate(hTE);		
	
{	optimization chex }
	IF theFocus = newFocus THEN
		Exit(ChangeFocus);
	
	{ We don't DRAW (or ERASE) the focus on editText items		}
	
	{ erase the old }
	IF theFocus > 0 THEN
		IF oldWasEditText THEN
			TEDeactivate(hTE)
		ELSE BEGIN
			SetupFocusUserItem(theData);
			EraseFocus(theDialog, hookFocusItem);
			HideFocusUserItem(theData);
		END;
	
	{ draw the new }
	theData^^.focus := newFocus;
	theFocus := newFocus;
	
	IF newIsEditText THEN BEGIN
		GetDItem(theDialog, theFocus, itemKind, itemHandle, itemRect);
		DialogPeek(theDialog)^.editField := theFocus - 1;
		WITH hTE^^ DO BEGIN
			viewRect := itemRect;		{	so convince dmgr		}
			destRect := itemRect;		{	do viewRect, destRect	}
			hText := itemHandle;		{	do the text				}
		END;
		TEAutoView(TRUE, hTE);			{	enable autoScroll		}
		TECalText(hTE);					{	recalculate				}
										
		IF NOT noSelect THEN
			TESetSelect(0, maxint, hTE);	{	select all				}
		
		TEActivate(hTE);				{	activate shields		}

		InsetRect(itemRect, -kEditTextWidth, -kEditTextWidth);	{ for outside box }
	END
	ELSE BEGIN
		TESetSelect(0, 0, hTE);			{ 	deselect since we are no longer in a text edit field }

		SetupFocusUserItem(theData);
		DrawFocus(theDialog, hookFocusItem);
		HideFocusUserItem(theData);
	END;
	
	theData^^.lastKey := TickCount - GetDblTime*2;				{ reset timer }
END;

{
	This sets up the focus userItem rectangle so that
	it is focused upon the correct item rectangle
	
	When we process update events, we call SetupFocusUserItem to
	setup the rectangle properly
}
PROCEDURE SetupFocusUserItem(theData: DataH);
VAR
	itemKind: 	INTEGER;
	itemHandle: Handle;
	itemRect: 	Rect;
	savedRect: 	Rect;
	
BEGIN
	WITH theData^^ DO BEGIN
	
		GetDItem(dialog, focus, itemKind, itemHandle, savedRect);
		IF BAND(itemKind, BNOT(itemDisable)) = editText THEN
			InsetRect(savedRect, -kEditTextWidth, -kEditTextWidth);
			
		InsetRect(savedRect, -(kFocusSize), -(kFocusSize));
		
		GetDItem(dialog, hookFocusItem, itemKind, itemHandle, itemRect);
		SetDitem(dialog, hookFocusItem, itemKind, itemHandle, savedRect);
	END;
END;

{
	This resets the focus userItem rectangle to 0 0 0 0 so that
	it does not get "in the way" of the item it is focused upon
	
	When we process update events, we call SetupFocusUserItem to
	setup the rectangle properly
}
PROCEDURE HideFocusUserItem(theData: DataH);
VAR
	itemKind: INTEGER;
	itemHandle: Handle;
	itemRect: Rect;
	
BEGIN
	WITH theData^^ DO BEGIN
		GetDItem(dialog, hookFocusItem, itemKind, itemHandle, itemRect);
		SetRect(itemRect, 0, 0, 0, 0);
		SetDitem(dialog, hookFocusItem, itemKind, itemHandle, itemRect);
	END;
END;



{ ---------------------------------------------------------------
	GetMaxFocus	-	Gets the # of items in the focus list
						
		activeList	-	the focus list
	
		returns		-	that number
----------------------------------------------------------------- }
FUNCTION GetMaxFocus(activeList: Ptr): INTEGER;	
BEGIN
	GetMaxFocus := IntPtr(ORD4(activeList))^;
END;


{ ---------------------------------------------------------------
	GetFocusItem	-	Gets the focus item number given an offset
						into the list of items
						
		theOffset	-	NO sanity checking!!!
	
		returns		-	a DITL item number
----------------------------------------------------------------- }
FUNCTION GetFocusItem(activeList: Ptr; theOffset: INTEGER): INTEGER;
BEGIN
	GetFocusItem := IntPtr(ORD4(activeList) + 
									(theOffset * SIZEOF(INTEGER)))^;
END;


{ ---------------------------------------------------------------
	GetFocusEntry	-	Gets the offset into the Focus List given
						a dialog item number
						
		theItem		-	no sanity checking is done here!!!!
	
		returns		-	an offset, or -1 for not found
----------------------------------------------------------------- }
FUNCTION GetFocusEntry(activeList: Ptr; theItem: INTEGER): INTEGER;
VAR	
	anItem		: INTEGER;
	maxItems	: INTEGER;
	i			: INTEGER;
	
BEGIN
	maxItems := GetMaxFocus(activeList);
	
	{ Walk the list until found or at end	}
	FOR i := 1 TO maxItems DO BEGIN
		anItem := GetFocusItem(activeList,i);
		IF anItem = theItem THEN BEGIN
			GetFocusEntry := i;
			EXIT(GetFocusEntry);
		END;
	END;	
	
	GetFocusEntry := -1;		{ not found	}
END;

{ ---------------------------------------------------------------
	SetFocusEntry	-	Sets the offset into the Focus List given
						a dialog item number
						
		theEntry	-	Where to put the value
		theItem		-	no sanity checking is done here!!!!
----------------------------------------------------------------- }
PROCEDURE SetFocusEntry(activeList: Ptr; theEntry, theItem: INTEGER);
VAR	
	thePtr	: Ptr;
	
BEGIN
	thePtr := Ptr(ORD4(activeList) + (theEntry * SIZEOF(INTEGER)));
	IntPtr(thePtr)^ := theItem;
END;


{ ---------------------------------------------------------------
	BuildFocusList	-	Builds the default list of focusable
						items incl. all found editText ones
						
		theData		-	The important stuff
	
		returns		-	a ptr to list of INTEGERs, where the
						first entry is the # of items, and the
						rest are item numbers for focusing
----------------------------------------------------------------- }						
FUNCTION BuildFocusList(theData: DataH): Ptr;
VAR
	thePtr 		: Ptr;
	maxFocus	: INTEGER;		{ max items to focus	}
	i			: INTEGER;		{ loop counter			}
	maxItems	: INTEGER;
	theDialog	: DialogPtr;
	
BEGIN
	thePtr := NIL;
	theDialog := theData^^.dialog;
	
	{ Calc the # of focusable items		}
	maxFocus := 2;				{ Item List & Zone List		}
	
	maxItems := CountDITL(theDialog);
	FOR i := 1 TO maxItems DO
		IF (GetItemKind(theDialog, i) = editText) THEN
			maxFocus := maxFocus + 1;
	
	{ Allocate the space for it			}
	thePtr := NewPtr((maxFocus + 1) * SIZEOF(INTEGER));
	
	{ Fill in the known entities		}
	IF MemError = noErr THEN BEGIN
		SetFocusEntry(thePtr, 0, maxFocus);			{ size		}
		SetFocusEntry(thePtr, 1, hookItemList);		{ lists		}
		SetFocusEntry(thePtr, 2, hookZoneList);
		
		maxFocus := 2;
		FOR i := 1 TO maxItems DO						{ all edit text	}
			IF (GetItemKind(theDialog, i) = editText) THEN BEGIN
				maxFocus := maxFocus + 1;
				SetFocusEntry(thePtr, maxFocus, i);
			END;
	END;
		
	BuildFocusList := thePtr;
END;


END.