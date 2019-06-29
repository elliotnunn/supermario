{
	File:		DITL.p

	Contains:	DITL Management unit

	Written by:	Byron Han

	Copyright:	© 1987-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <7>	12/13/90	kaz		Stuff the new rect before appending resCtrls. <jng>
		 <6>	  5/9/90	kaz		Adjusted the rects of resCtrl items on Append
		 <5>	 3/29/90	BBH		fix handling of resControl's (was using uninitialized control ID
									variable)
		 <4>	 3/22/90	BBH		Implement (latest) Help interface stuff thrashed out with Randy
									(new constant HMScanAppendhdlg with the hdlg ID and the dialog
									item list offset stored in the ditl itself)
		 <3>	 3/21/90	BBH		Take out SetClip's
									Use WITH where possible
									Use ShowDItem/HideDItem to handle text edit item
									appending/removal
		 <2>	 3/15/90	BBH		renamed some routines
		 <1>	 3/14/90	BBH		first checked in

	To Do:

	Pre-BBS History
		1/30/90		BBH		Changed handling of helpitems to reflect reality.
		8/22/89		BBH		In the Save/Restore of the control template rectangle, we assumed
								that GetNewControl does not release the CNTL resource.
								This is, unfortunately, only true on the Mac II.  On SE and Plus
								the CNTL resource is released by the control manager.
		8/21/89		BBH		Eliminate call to ReleaseResource when handling resControl's
		
		8/19/89		BBH		In fixing things, forgot to lock down theNewDITL when appending
								to end of old one....
								
							Revised handling of resource controls to truly allow use of cctb's
								in resource controls.
							Also there was a screw up in the handling of the rectangles for
								resource controls which is now fixed. 	The strategy is the
								load the CNTL template and offset the template rectangle
								appropriately.  Then after control creation via GetNewControl
								(which automagically handles cctb's), the control is moved
								to the topLeft of the itemRect (as per dmgr.a)
		8/18/89		BBH		Changed BitOr to BOR's
							Use WITH's statements
							
							In processing helpItem, we now stuff helpInvalidID into the
							ID field of the dialog item list if we are unable to load the
							hdlg resource.
		
							Added error recovery from failing to append the new item list
								to the old item list.
								
							Moved calls to InvalRect in Append outside of CASE structure
								to cut down on codesize.
								
							Changed constants 5, 3, 8192 to named constants 
		8/7/89		BBH		Added balloon help support per conversations with Randy Carr
							'hdlg' resource has first two bytes version, second two being
							the item number (0 based) of the first dialog item managed by the
							'hdlg' resource.
							
							Renamed some data structure elements to be clearer and renamed
							some local variables to be clearer.
		8/4/89		BBH		Removed "fix" to resControl handling.  Now explicitly read in the
							CNTL template and handle it manually.
	
		8/2/89		BBH		Added support for HelpItems ?
							Handle resControl items with call to GetNewControl instead of
								manually ditling with the CNTL template (no pun intended)
		7/28/89		MPR		Added support for color icons
		4/25/89		BBH		Changed BitAnd to BANDs
							Added some memory error checking
		4/9/89		BBH		Added sanity check to AppendDITL to see if theDITL is NIL.
							Added sanity check to ShortenDITL to see if numberItems is <= 0
}

UNIT DITL;
INTERFACE

USES
	MemTypes, 
	QuickDraw, 
	OSIntf, 
	ToolIntf, 
	PackIntf, 
	AppleTalk,
	CTBUtilities,
	CommToolboxPriv;

FUNCTION DITLEntry(theDialog: DialogPtr; theDITL: Handle; method: INTEGER;
				numberItems: INTEGER; msg: INTEGER): LONGINT;

IMPLEMENTATION
CONST
{	INTERNAL CONSTANTS }

	dialogSlop			=	5;	{ after growing dialog, this is whitespace at bottom/right }
	teSlop				=	3;	{ slop around text edit boxes }
	hiddenThreshold		=	8192;	
	
	low7Bits			=	$0000007F;			
	
	buttonMask			=	$03;					{ 	to get var code for buttons 		}
	
	growNothing			=	0;				
	growRight			=	1;						{ 	internal msgs for dialgo growing	}
	growBottom			=	2;						{ 	to the right or to the bottom 		}
	
{	new item types to be included someday with Dialogs.p }
	helpItem			=	1;
	reservedItem		=	2;
	
{	more help stuff }
	helpResource		=	'hdlg';
	helpInvalidID		=	-1;
	HMScanAppendhdlg	=	8;
	
TYPE
{	DITL data structures	}
	IntPtr 				= 	^INTEGER;
	IntHandle 			= 	^IntPtr;
	
	hHelpItem			=	^pHelpItem;				{ 	per discussions with Randy Carr 	}
	pHelpItem			=	^rHelpItem;
	rHelpItem			=	RECORD
		hVersion	:	INTEGER;
		hFirstItem	:	INTEGER;
		hItemData	:	ARRAY[0..0] OF SignedByte;
	END;
	
	hDITLItem 			= 	^pDITLItem;
	pDITLItem 			= 	^DITLItem;
	DITLItem 			= 	RECORD
		dItemHandle	: Handle;
		dItemRect 	: Rect;
		dItemType 	: SignedByte;
		dItemData 	: Array[0..0] OF SignedByte;
	END;
	
	hItemList 			= 	^pItemList;
	pItemList 			= 	^ItemList;
	ItemList 			= 	RECORD
		iNumberItems: INTEGER;
		iDITLItems 	: ARRAY[0..0] OF DITLItem;
	END;
	

{	forward declaration of routines }
{	These routines are the externally visible routines which are accessed via the
	single entry point and selector }

PROCEDURE DoAppendDITL( theDialog:DialogPtr; theDITL:Handle; method:INTEGER); FORWARD;
FUNCTION  DoCountDITL( theDialog:DialogPtr ) : INTEGER; FORWARD;
PROCEDURE DoShortenDITL( theDialog:DialogPtr; numberItems:INTEGER); FORWARD;

{	utility routines } 
FUNCTION IsCGrafPort(theCPort: CGrafPtr): BOOLEAN; FORWARD;
FUNCTION GetFirstTEItem(theDialog: DialogPtr) : INTEGER; FORWARD;
FUNCTION GetUSB(a : SignedByte) : INTEGER; FORWARD;
FUNCTION EvenInt(a : INTEGER) : INTEGER; FORWARD;
	

{	Main Entry Point }
{------------------------------------------------------------------------------------------}
FUNCTION	DITLEntry(theDialog: DialogPtr; theDITL: Handle; method: INTEGER;
				numberItems: INTEGER; msg: INTEGER): LONGINT;
				
BEGIN
	DITLEntry := 0;
	CASE msg OF
		AppendMSG:
			DoAppendDITL(theDialog, theDITL, method);
		CountMSG:
			DITLEntry := DoCountDITL(theDialog);
		ShortenMSG:
			DoShortenDITL(theDialog, numberItems);
		OTHERWISE
			DITLEntry := -1;											{ 	out of range 	}
	END; {case}
END;

PROCEDURE DoAppendDITL( theDialog:DialogPtr; theDITL:Handle; method:INTEGER);
VAR
	offset 			: Point;			{	offset each appended item from this				}
	
	theDialogRect 	: Rect;				{	dialog original portRect						}	
	theNewDITL 		: hItemList;		{	to append (should be detached from rsrc map)	}
	theOldDITL		: hItemList;		{	the current DITL								}
	theType 		: INTEGER;			{	type of item being worked on					}
		
	firstItem 		: INTEGER;			{	item number of first appended item (1 based)	}
	newItems 		: INTEGER;			{	number of new items	to append					}
	dataSize 		: LONGINT;			{	size of data associated with item				}
	theCurrentItem 	: pDITLItem;		{	current item in appended list being processed	}
	i 				: INTEGER;			{	a counter										}
	err 			: OSErr;			{	fuckup notifier									}
	
	firstTEItem 	: INTEGER;			{	first TE item.  need to special case adding		}
										{	a TE item where none were before				}
	
	theID			: INTEGER;			{	ID for creating control							}
	theIDPtr		: IntPtr;
	
	theHelpHandle	: hHelpItem;		{	for 'hdlg' resource								}
	
	myItemKind		: INTEGER;			{	itemKind										}
	myItemHandle	: Handle;			{	itemBox											}
	myItemBox		: Rect;				{	itemBox											}
	
	expanded		: INTEGER;			{	expanded right or bottom?						}
	tempRect		: Rect;				{	place holder									}
	
	savedPort		: GrafPtr;			{	saved port										}
	
BEGIN
	IF theDITL = NIL THEN
		Exit(DoAppendDITL);						{ abort abort abort 						}

	GetPort(savedPort);							{	sanity									}
	SetPort(theDialog);
	HidePen;
	
	theDialogRect := theDialog^.portRect;		{	current dialog portRect					}
	expanded := growNothing;					{	did not grow rectangle					}
	
	IF method < 0 THEN							{	relative to topLeft of item |method|	}
	BEGIN									
		GetDItem(theDialog, -method, myItemKind, myItemHandle, myItemBox);
		offset := myItemBox.topLeft;
	END
	ELSE
	BEGIN
		offset := theDialog^.portRect.topLeft;		{ 	append rel to topLeft 	}
		CASE method OF
			appendDITLRight:
				offset.h := theDialogRect.right;	{ relative to topRight of portRect		}
			appendDITLBottom:
				offset.v := theDialogRect.bottom;	{ relative to botLeft of portRect		}
		END; {case}
	END;
			
	theOldDITL := hItemList( DialogPeek(theDialog)^.items );	{ 	current item list		}
	theNewDITL := hItemList(theDITL);							{	list to be appended		}
	
	firstItem := theOldDITL^^.iNumberItems + 2;			{ 	item # of first appended item	}
	newItems := theNewDITL^^.iNumberItems + 1;			{	# items to append				}
														{	this are 1 based				}
														
	firstTEItem := GetFirstTEItem(theDialog);			{	first TE item in old list		}
														{	this is 1 based 				}
														
	HLock(Handle(theNewDITL));							{	lock new list					}
	theCurrentItem := @theNewDITL^^.iDITLItems;			{	go through items in new list	}
	FOR i := 1 TO newItems DO 
	BEGIN												{	loop de loop					}
		WITH theCurrentItem^ DO 
		BEGIN
			myItemBox := dItemRect;						{	get item rectangle				}
			OffsetRect(myItemBox, offset.h, offset.v);	{	move it							}
			InvalRect(myItemBox);						{	Invalidate the rectangle		}
	
			theType := BAND(dItemType, low7Bits);		{	what kind of item is it			}
			dataSize := GetUSB(dItemData[0]);			{	and how big is the data			}
			
			dItemHandle := NIL;							{ 	pessimism						}
			
			CASE theType OF
			
				helpItem:
				BEGIN
					theID := IntPtr(@dItemData[1])^;	{	get the help item type			}
					IF theID = HMScanAppendhdlg THEN	
					BEGIN
						theIDPtr := IntPtr(@dItemData[5]);	{	stuff offset into ditl		}
						theIDPtr^ := firstItem - 1;			{ 	stuff invalid ID marker		}
					END;				
				END;
				
				ctrlItem + btnCtrl,	
				ctrlItem + chkCtrl,		
				ctrlItem + radCtrl: 		
				BEGIN		
					dItemHandle := 
						Handle(NewControl(theDialog, 
										myItemBox, 
										StringPtr(@dItemData[0])^, 
										TRUE, 			{ 	visible, val, min, max			}
										0, 0, 1, 		{ 	procID, refCon					}
										BAND(theType, buttonMask), 0));
					
					{ We get the new contrlRect 'cuz NewControl may have changed the contrlRect	}
					IF dItemHandle <> NIL THEN
					BEGIN
						myItemBox := ControlHandle(dItemHandle)^^.contrlRect;
						InvalRect(myItemBox);			{	invalidate it since we are not	}
					END;
					
				END;
				
				ctrlItem + resCtrl:
				BEGIN
					{ We stuff the new rect in the CNTL resource before NewControl	}
					{ so that the control won't get confused about drawing/erasing	}
					{ in the old location											}
					
					dItemHandle := GetResource('CNTL', IntPtr(@dItemData[1])^);
					IF (dItemHandle <> NIL) THEN
						BlockMove(@myItemBox,Ptr(dItemHandle^),SIZEOF(Rect));
					
					dItemHandle := Handle(GetNewControl(IntPtr(@dItemData[1])^, theDialog));
					IF dItemHandle <> NIL THEN
					BEGIN
						myItemBox := ControlHandle(dItemHandle)^^.contrlRect;
						InvalRect(myItemBox);			{	didn't really draw it so...	}
					END;
				END;
				
				statText: 									{	static text item			}
					err := PtrToHand(@dItemData[1], dItemHandle, dataSize);	{	copy data	}
				
				editText: 								{	yummy.  a text edit item		}
				BEGIN
					err := PtrToHand(@dItemData[1], dItemHandle, dataSize);	{	copy data	}
					InsetRect(myItemBox, -teSlop, -teSlop);		
														{	expand to take care of outline	}
					InvalRect(myItemBox);				{	box.  so inval it				}
					InsetRect(myItemBox, teSlop, teSlop);
				END;
				
				iconItem: 								{	an icon!						}
				BEGIN
					IF IsCGrafPort(CGrafPtr(theDialog)) THEN{	check for a color dialog	}
						dItemHandle := Handle(GetCIcon(IntPtr(@dItemData[1])^));	
														{	try for a cicn	}
					IF dItemHandle = NIL THEN							{	nope, get BW	}
						dItemHandle := GetIcon(IntPtr(@dItemData[1])^);	{	load the icon	}
											
				END;
				
				picItem: 								{	a picture (of vanna white)		}
					dItemHandle := Handle(GetPicture((IntPtr(@dItemData[1])^)));
					
			END; {case}
			
			dItemRect := myItemBox;						{	in case itemBox changed			}
		END; {with theCurrentItem}

		UnionRect(myItemBox, theDialogRect, tempRect);	{	do we need to grow dlog rect?	}
		IF tempRect.right > theDialogRect.right THEN	{	grown to the right				}
			expanded := BOR(expanded, growRight);		{	set right grow bit				}
		IF tempRect.bottom > theDialogRect.bottom THEN	{	grown to the bottom				}
			expanded := BOR(expanded, growBottom);	{	set bottom grow bit				}
		theDialogRect := tempRect;						{	set dlog rect to grown rect		}
		
		dataSize := EvenInt(dataSize);					{	item data is padded to even len	}
		theCurrentItem := pDITLItem(Ptr(ORD4(@theCurrentItem^) + dataSize + sizeof(DITLItem)));
														{	move to next item				}
	END; {for i}
	HUnlock(Handle(theNewDITL));

														{	append new list to old			}
	HLock(Handle(theNewDITL));
	theCurrentItem := @theNewDITL^^.iDITLItems;
	err := PtrAndHand(Ptr(theCurrentItem), 				{	-2 is to skip numItems field	}
							Handle(theOldDITL), 
							GetHandleSize(Handle(theNewDITL)) -2);
	HUnlock(Handle(theNewDITL));
	
	IF err <> noErr THEN								{ 	cannot add items to DITL		}
	BEGIN
		{	cleanup handles and/or resources allocated 	}
		HLock(Handle(theNewDITL));							{	lock new list				}
		theCurrentItem := @theNewDITL^^.iDITLItems;			{	go thru items in new list	}
		FOR i := 1 TO newItems DO 
		BEGIN												{	loop de loop				}
			WITH theCurrentItem^ DO 
			BEGIN
				theType := BAND(dItemType, low7Bits);		{	what kind of item is it		}
				dataSize := EvenInt(GetUSB(dItemData[0]));			
															{	and how big is the data		}
				IF dItemHandle <> NIL THEN
				BEGIN
					CASE theType OF
						ctrlItem + btnCtrl,						{	dispose of it				}
						ctrlItem + chkCtrl,						
						ctrlItem + radCtrl, 					
						ctrlItem + resCtrl : 					
							DisposeControl( ControlHandle(dItemHandle) );
								
						statText, 								{	static text item			}
						editText:
							DisposHandle(dItemHandle);
								
						iconItem, 								{	an icon!					}
						picItem: 								{	a picture (of vanna white)	}
							ReleaseResource(dItemHandle);
					END; {case}
				END;
				
			dItemHandle := NIL;			
			END; {with theCurrentItem}
			theCurrentItem := pDITLItem(Ptr(ORD4(@theCurrentItem^) + dataSize + sizeof(DITLItem)));
		END; {for}
		HUnlock(Handle(theNewDITL));
		
		{	now cleanup local variables that indicate state of appending operation }
		
		newItems := 0;									{	no new items 					}
		expanded := growNothing;						{ 	no growing of the dialog 		}
		theDialogRect := theDialog^.portRect;			{	mark no size change of dialog	}
	END; {if err <> noErr}
	
	WITH theOldDITL^^ DO
		iNumberItems := iNumberItems + newItems; 		{ 	add itemCount	}
		
	IF BAND(expanded, growBottom) = growBottom THEN		{	add an extra 5 if growing		}
		theDialogRect.bottom := theDialogRect.bottom + dialogSlop;
	IF BAND(expanded, growRight) = growRight THEN		{	add an extra 5 if growing		}
		theDialogRect.right := theDialogRect.right + dialogSlop;

													{ only redraw the rectangle if size has changed }
	IF NOT EqualPt(theDialogRect.botRight, theDialog^.portRect.botRight) THEN
		SizeWindow(theDialog, theDialogRect.right, theDialogRect.bottom, FALSE);
														{	resize the window if needed		}
	IF firstTEItem = 0 THEN								{ 	no TE items before… 			}
	BEGIN
		firstTEItem := GetFirstTEItem(theDialog);		{ 	got one now?					}
		IF firstTEItem > 0 THEN
		BEGIN
			HideDItem(theDialog, firstTEItem);			{ 	select it please				}
			ShowDItem(theDialog, firstTEItem);
			SelIText(theDialog, firstTEItem, 0, 32767);
		END;
	END;
	ShowPen;
	SetPort(savedPort);
END;


{	returns the number of dialog items in the dialog item list }
{	this is 1 based }
FUNCTION DoCountDITL( theDialog:DialogPtr) : INTEGER;
BEGIN
	DoCountDITL := 0;
	IF theDialog <> NIL THEN
		DoCountDITL := IntHandle(DialogPeek(theDialog)^.items)^^ + 1;
END;

PROCEDURE DoShortenDITL( theDialog:DialogPtr; numberItems:INTEGER);
{ 	
	removes "number of items" from end of DITL. 
	
	need to remember that if we have a editText item in the area being deleted,
	we must be sure that we are not in it.  if we are, we need to make sure
	we move the current text edit item to one that is available.  it there are
	none, we need to set editField to -1
}
VAR
	theOldDITL		: hItemList;		
	lastItem 		: INTEGER;
	lastSavedItem 	: INTEGER;
	dataSize 		: LONGINT;
	
	theCurrentItem 	: pDITLItem;
	i 				: INTEGER;
	newDITLSize 	: LONGINT;
	
	tempKind 		: INTEGER;
	
	tempRect 		: Rect;
	tempHandle 		: Handle;
	
	theState1		:INTEGER;

	savedPort		: GrafPtr;

	killCurrent		: BOOLEAN;						{ are we disposing of the current TE item }
	
	itemKind: INTEGER;
	itemHandle: Handle;
	itemRect: Rect;
	
	changeIt: BOOLEAN;
	
BEGIN
	IF numberItems <= 0 THEN
		Exit(DoShortenDITL);						{ abort abort abort 					}
		
	GetPort(savedPort);								{ sanity								}
	SetPort(theDialog);
	
	theOldDITL := hItemList( DialogPeek(theDialog)^.items );	{ the current item list		}

	newDITLSize := 2;								{	how many bytes to keep in item list	}
	lastItem := theOldDITL^^.iNumberItems + 1;		{	last item in DITL (1 based)			}
	lastSavedItem := lastItem - numberItems;		{	itemNumber of last to save (1 base)	}
	
	killCurrent :=  (DialogPeek(theDialog)^.editField+1 > lastSavedItem);
	
	FOR i := lastSavedItem + 1 TO lastItem DO
	BEGIN
		GetDItem(theDialog, i, itemKind, itemHandle, itemRect);
		itemKind := BAND(BNOT(itemDisable), itemKind);	{ clear disable bit					}

		IF itemKind = editText THEN
			InsetRect(itemRect, -teSlop, -teSlop);
		EraseRect(itemRect);
		InvalRect(itemRect);
		
		IF itemKind = editText THEN
			HideDItem(theDialog, i);
		
		IF itemHandle <> NIL THEN
		BEGIN
			changeIt := TRUE;
			CASE itemKind OF
				statText,
				editText:
					DisposHandle( itemHandle );
					
				ctrlItem + btnCtrl,				{	a control, eh?						}
				ctrlItem + chkCtrl,
				ctrlItem + radCtrl,
				ctrlItem + resCtrl : 			{	a resource control, eh?				}
					DisposeControl( ControlHandle(itemHandle) );
					
				iconItem,
				picItem: 
					ReleaseResource(itemHandle);
			
				OTHERWISE
					changeIt := FALSE;
			END; {case}
			
			IF changeIt THEN
				SetDItem(theDialog, i, userItem, NIL, itemRect);
		END; {if itemHandle <> NIL }
	END;
		
	theState1 := HGetState(Handle(theOldDITL));		{	get state of DITL					}
	HLock(Handle(theOldDITL));						{	lock it down						}
	theCurrentItem := @theOldDITL^^.iDITLItems;		{	the current item to look at			}

	FOR i := 1 TO lastSavedItem DO 						{ 	and a 1, and a 2, and a 3, 4, 5, 6	}
	BEGIN
		dataSize := EvenInt(GetUSB(theCurrentItem^.dItemData[0]));	
		newDITLSize := newDITLSize + dataSize + sizeof(DITLItem);	{	yup				}
			
		theCurrentItem := pDITLItem(Ptr(ORD4(@theCurrentItem^) + dataSize + sizeof(DITLItem)));
	END; {for }
	
	SetHandleSize(Handle(theOldDITL), newDITLSize);		{	reduce datasize of DITL			}
	WITH theOldDITL^^ DO
		iNumberItems := iNumberItems - numberItems;		{	reduce # items in DITL			}
	HSetState(Handle(theOldDITL), theState1);			{	restore state of DITL			}
	
	IF killCurrent THEN
	BEGIN
		i := DialogPeek(theDialog)^.editField+1;
		IF i > 0 THEN
			SelIText(theDialog, i, 0, 32767);
	END;
	SetPort(savedPort);									{	go on home						}
END;

{	This function takes a grafPtr and returns TRUE if it is a color grafPort }
FUNCTION IsCGrafPort(theCPort: CGrafPtr): BOOLEAN;
BEGIN
	IsCGrafPort := (BAND(theCPort^.portVersion, $0000C000) = $0000C000);
END;

{	
	This is a utility routine which returns the item number (1 based) of the
	first text edit item in the dialog item list, 0 if none is found
}
FUNCTION	GetFirstTEItem(theDialog: DialogPtr) : INTEGER;
VAR
	theOldDITL		: hItemList;		
	lastItem 		: INTEGER;
	dataSize 		: LONGINT;
	
	theCurrentItem 	: pDITLItem;
	i 				: INTEGER;
	theType			: INTEGER;
	
	firstTEItem 	: INTEGER;

BEGIN
	theOldDITL := hItemList( DialogPeek(theDialog)^.items );
	
	WITH theOldDITL^^ DO
	BEGIN
		lastItem := iNumberItems;
		theCurrentItem := @iDITLItems;
	END;

	GetFirstTEItem := 0;
	FOR i := 0 TO lastItem DO
	BEGIN
		WITH theCurrentItem^ DO
		BEGIN
			theType := BAND(dItemType, low7Bits);
			dataSize := EvenInt(GetUSB(dItemData[0]));
			
			IF theType = editText THEN
				IF dItemRect.left < hiddenThreshold THEN	{ hidden? }
				BEGIN
					GetFirstTEItem := i+1;					{ return 1 based value }
					Exit(GetFirstTEItem);					{ exit please }
				END;
		END;		
		theCurrentItem := pDITLItem(Ptr(ORD4(@theCurrentItem^) + dataSize + sizeof(DITLItem)));
	END; {for }
END; {function}
			
{	this function casts a Signed Byte into an integer }
FUNCTION GetUSB(a : SignedByte) : INTEGER;	{ cast signed byte into unsigned byte }
BEGIN
	GetUSB := BAND($000000FF, a);
END;
	
{	this function evens out an integer (rounding up) }
FUNCTION EvenInt(a : INTEGER) : INTEGER;
BEGIN
	EvenInt := BAND(a+1, $FFFFFFFE);
END;

END.