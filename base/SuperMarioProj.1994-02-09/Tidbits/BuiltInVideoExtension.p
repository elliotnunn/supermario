{
	File:		BuiltInVideoExtension.p

	Contains:	Built-in Video Monitors (cdev) Extension for IIci and IIsi.

	Written by:	Jim Straus/Mike Puckett

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: Mac32 BigBang Sys606

	Change History (most recent first):

		 <4>	 1/23/91	stb		BBM: change spParamData in SpBlock type from Ptr to a LONGINT;
		 <3>	  1/7/91	JDR		(dba) Devices.p now has the Monitors extensions, so I've removed
									them from this file.
		 <2>	12/14/90	dba		<JDR> get rid of redundant RectPtr declaration, now in Types.p
		 <1>	 10/6/90	jmp		Name change from VideoExtensionIIci.p to
									BuiltInVideoExtension.p.  That’s all!
		<11>	 10/6/90	jmp		Cleaned up changes I made on 10/2/90 -- no functionality
									changes; changed name of file from VideoExtensionIIci.p to
									BuiltInVideoExtension.p to reflect that it is used for more than
									just the IIci.
		 <10>	 10/4/90	JAL		changed spStackPtr to spParamData to match change in Slots.p
		 							interface file.
		 <9>	 10/2/90	jmp		NEEDED FOR 7.0: In version 4.4b2 of Monitors, I effectively
									removed the CodeValueOK code so that developers could have the
									entire LONGINT range of values. However, several third-party
									extensions were relying on Monitors reserving values 1-255 as
									error return values. So, in Monitors 4.4b3, I put the
									CodeValueOK code back in. This change, then, was made so that
									superUser vs. normalUser modes can be remembered across calls in
									the built-in extension (i.e., I added a call to NewHandle at
									startupMsg time, and a call to DisposHandle at okMsg/cancelMsg
									time).
		 <8>	 9/29/90	jmp		NEEDED FOR 7.0:  For version 4.4b2 of Monitors and later, this
									extension now always comes up; changing memory allocation for
									RBV is no longer considered an “expert” feature -- especially
									with the memory constraints imposed by System 7.0.  This
									extension works on 6.0.x Systems as well.
		 <7>	 8/23/90	jmp		Added MPW3.0 conditional for C2PStr. I’m not sure which MPW
									objects SixPack or 7.0 are built with; I use MPW 3.2 myself.
		 <6>	 8/18/90	jmp		NEEDED FOR SIXPACK:  Cleaned up changes that were made on July
									3rd -- Get1Resource vs GetResource problem.  Also, replaced the
									call of WeirdStatus to PBStatus, and brought GetTypesRsrc
									in-line (i.e., there is now no dependecy of this file on
									VideoExtensionIIci.a).  I also fixed a problem in GetCardName
									that only occured when 32-bit addressing is activated (same
									change that was made to the NuMonitors.p source); this problem
									caused the built-in video radio buttons dialog to not show up.
									Finally, I fixed the window updating problems that were showing
									up under System 7.0.
		 <5>	  7/3/90	jmp		I added to the comments for the previous fix (number <4>).  No
									code changes were made. See the <5> comment block in the EDialog
									routine.
		 <4>	  7/3/90	DDG		NEEDED FOR SIXPACK: Fixed resource reference problem.
		 <3>	 6/18/90	JBS		NEEDED FOR SIXPACK-Merged ci/erickson extension with elsie
									resources
		 <2>	  1/3/90	CCH		Change name of Gestalt interface file.
	   <1.0>	  8/3/89	JBS		FOR 6.0.4 BUILD - Mac IIci Monitors extension code added to
									build.

	To Do:
}
UNIT MNTR;

INTERFACE

USES
	GestaltEqu,
	Errors,
	Devices,
	Slots,
	Video,
	Start,
	QuickDraw,
	OSUtils,
	Desk,
	SysEqu,
	Menus,
	Windows,
	Dialogs,
	Lists,
	Fonts,
	Memory,
	Packages,
	PaletteMgr,
	Resources,
	ROMDefs,
	ToolUtils,
	Strings,
	PasLibIntf;


{$D-}	{No debug checking}
{$R-}	{Zap range checking again, in case USEd units set it}

CONST
	SlotCount		= 15;	{Maximum number of slots we deal with}

	MonitorsMem		= $C2E;	{Monitors low memory global}

	divLineItem		= 1;	{the items in our DITL}
	bwOnlyItem		= 2;	{the four radio buttons.  Must be together and in order}
	fourItem		= 3;
	sixteenItem		= 4;
	twoFiftySixItem	= 5;
	rect1Item		= 6;	{the various rectangles for max depths -- there’s got to be}
	rect2Item		= 7;	{	a better way!}
	rect3Item		= 8;
	titleItem		= 9;
	memoryItem		= 10;

	buttonDistance	= 20;		{how far the buttons are apart.  Used to shrink the RECT resource}
	memRoundFactor	= $8000;	{size to round memory amounts by}

	spFlagAll		= 1;		{flags that can be set for spParamData}
	spFlagOneSlot	= 2;
	spFlagNext		= 4;

	cscSetDefault 	= 9;		{csCode to the driver to get and set the video default}

	rExtensionId	= -4096;	{use localID and baseID as calculated in code instead}
	rLocalId		= -4080;

	Mac2ID			= $86;
	PortraitID		= $81;
	IIGSID			= $82;
	SEID			= $87;

	IDMask			= $E7;		{????}

	numShow			= 2;		{localID to localID+numShow are the cards on which we should be}
								{visible; i.e. we won't show up ever on an Elsie or Eclipse.}

	errorValue		= 1;		{returned in monitorValue when the extension can’t be started up}

TYPE
	LongPtr = ^LONGINT;
	LongHdl = ^LongPtr;

	QDGlobals = RECORD
			randSeed:	LONGINT;
			screenBits:	BitMap;
			arrow:		Cursor;
			dkGray:		Pattern;
			ltGray:		Pattern;
			gray:		Pattern;
			black:		Pattern;
			white:		Pattern;
			thePort:	GrafPtr;
		END;
	QDGlobalsPtr = ^QDGlobals;

	{
		August 15, 1990 jmp mod -- added this definition for ease of use with PBStatus,
		which is replacing the calls to WeirdStatus.  (I.e., There’s nothing weird
		about _Status except in the high-level glue, and who needs it anyway.)
	}
	VideoParamBlock = RECORD
		qLink:			QElemPtr;
		qType:			INTEGER;
		ioTrap:			INTEGER;
		ioCmdAddr:		Ptr;
		ioCompletion:	ProcPtr;
		ioResult:		OSErr;
		ioNamePtr:  	StringPtr;
		ioVRefNum:		INTEGER;
		ioRefNum:		INTEGER;
		csCode:			INTEGER;
		csParam:		Ptr;
	END;
	VideoParamBlockPtr = ^VideoParamBlock;

	ScrnRecord = RECORD		{'scrn' info for each screen}
		srDrvrHW:		INTEGER;	{spDrvrHW from slot decl mgr's device type}
		srSlot:			INTEGER;	{Slot number for the screen's video card}
		srDCtlDevBase:	LONGINT;	{Base address of card's memory}
		srMode:			INTEGER;	{sRsrcID for desired mode}
		srFlagMask:		INTEGER;	{$77FE}
		srFlags:		INTEGER;	{active, main screen, B/W or color}
		srColorTable:	INTEGER;	{Resource ID of desired 'clut'}
		srGammaTable:	INTEGER;	{Resource ID of desired 'gama'}
		srRect:			Rect;		{Device's rectangle, global coordinates}
		srCtlCount:		INTEGER;	{Number of control calls}
	END;
	ScrnRecordPtr = ^ScrnRecord;
	ScrnRecordHandle = ^ScrnRecordPtr;

	ScrnRsrc = RECORD				{Complete 'scrn' resource}
		count:		INTEGER;		{Number of screens configured here}
		scrnRecs:	ARRAY[1..SlotCount] OF ScrnRecord;	{Config for @ one}
	END;
	ScrnRsrcPtr = ^ScrnRsrc;
	ScrnRsrcHandle = ^ScrnRsrcPtr;

	RectHandle = 	^RectPtr;

	VDDefaultRecord = RECORD
		csID:	SignedByte;
		pad:	SignedByte;
	END;
	GDDefaultRecPtr = ^VDDefaultRecord;

	SPRAMRecord = RECORD
			boardID:	INTEGER;	{Apple-defined card identification}
			vendorUse1:	SignedByte;	{Byte with the default mode for screen devices}
			vendorUse2:	SignedByte;	{Unused by us}
			vendorUse3:	SignedByte;	{Unused by us}
			vendorUse4:	SignedByte;	{Unused by us}
			vendorUse5:	SignedByte;	{Unused by us}
			vendorUse6:	SignedByte;	{Unused by us}
		END;

FUNCTION Entry(message,					{the action to handle}
		Item,							{the DITL item or 0}
		numItems:		INTEGER;		{the number of the systems last item}
		monitorValue:	LONGINT;		{the value returned last time.  defaults to zero}
		mDialog:		DialogPtr;		{the dialog pointer}
		theEvent:		EventRecord;	{the event that caused the message to happen}
		ScreenNum:		INTEGER;		{the number of the screen to affect}
		VAR Screens:	ScrnRsrcHandle;	{the screen resource to change if desired}
		VAR ScrnChanged:	BOOLEAN		{TRUE if we should clean up Screens}
		): LONGINT;						{value to pass back in or error if negative}

IMPLEMENTATION

FUNCTION EDialog(message,				{the action to handle}
		Item,							{the DITL item or 0}
		numItems:		INTEGER;		{the number of the systems last item}
		monitorValue:	LONGINT;		{the value returned last time.  defaults to zero}
		mDialog:		DialogPtr;		{the dialog pointer}
		theEvent:		EventRecord;	{the event that caused the message to happen}
		ScreenNum:		INTEGER;		{the number of the screen to affect}
		VAR Screens:	ScrnRsrcHandle;	{the screen resource to change if desired}
		VAR ScrnChanged:	BOOLEAN		{TRUE if we should clean up Screens}
		): LONGINT; FORWARD;			{value to pass back in or error if negative}

FUNCTION Entry(message,					{the action to handle}
		Item,							{the DITL item or 0}
		numItems:		INTEGER;		{the number of the systems last item}
		monitorValue:	LONGINT;		{the value returned last time.  defaults to zero}
		mDialog:		DialogPtr;		{the dialog pointer}
		theEvent:		EventRecord;	{the event that caused the message to happen}
		ScreenNum:		INTEGER;		{the number of the screen to affect}
		VAR Screens:	ScrnRsrcHandle;	{the screen resource to change if desired}
		VAR ScrnChanged:	BOOLEAN		{TRUE if we should clean up Screens}
		): LONGINT;						{value to pass back in or error if negative}
BEGIN
	Entry := EDialog(message, Item, numItems, monitorValue, mDialog, theEvent,
					ScreenNum, Screens, ScrnChanged);
END;

{--------------------------------------------------------------------------------
 GetCardName - returns the name of the driver for a particular slot.
}
FUNCTION GetCardName(slotNum: INTEGER): STR255;
CONST
	board		= 0;
	drvrHWMask	= 1;
VAR
	sPB:		SpBlock;
	name:		Str255;
BEGIN
	WITH sPB DO
		BEGIN
			spSlot		:= slotNum;			{ Start the sResource search from the top of the list }
			spID		:= 0;				{	(i.e, from 0+1 since 0 is invalid).				  }
			spExtDev	:= 0;

			spTBMask	:= 0;				{ Look for board sResources [catBoard,typeBoard,0,0]  }
			spCategory	:= catBoard;		{	only, set set mask to 0.						  }
			spCType		:= typeBoard;
			spDrvrSW	:= 0;
			spDrvrHW	:= 0;

			spHwDev		:= 0;
		END;

	name := 'XXX';							{ Assume we will fail. }

	IF SNextTypeSRsrc(@sPB) = noErr THEN
		BEGIN
			sPB.spID := sRsrcName;

			IF SGetCString(@sPB) = noErr THEN
				name := C2PStr(Ptr(sPB.spResult))^
		END;

	GetCardName := name;
END;

{---------------------------------------------------------------------------------
 GetQDGlobals - return pointer to quickdraw globals
}
FUNCTION GetQDGlobals: QDGlobalsPtr;
BEGIN
	{
		Get pointer to thePort, which is at offset zero from A5, and adjust
		it to point at the base of the quickdraw globals.  We have to do
		this to reference quickdraw globals, because as a desk accessory
		we can't have any globals of our own.
	}

	GetQDGlobals := QDGlobalsPtr(
		LongHdl(CurrentA5)^^ + Sizeof(GrafPtr) - Sizeof(QDGlobals));
END;

{---------------------------------------------------------------------------------
 DrawDivLine draws the line separating the extension from the rest of the options
 dialog.  DrawLine, below, just draws a solid line.
}
PROCEDURE DrawDivLine(theWindow: WindowPtr; itemNo: INTEGER);
VAR
	itemType:		INTEGER;
	itemHandle:		Handle;
	itemBox:		Rect;
	savedPenState:	PenState;
BEGIN
	GetDItem(DialogPtr(theWindow), itemNo, itemType, itemHandle, itemBox);

	GetPenState(savedPenState);
		PenNormal;
		FrameRect(itemBox);

		PenMode(notPatBic);
		PenPat(GetQDGlobals^.gray);
		PaintRect(itemBox);

	SetPenState(savedPenState);
END;

PROCEDURE DrawLine(theWindow: WindowPTr; itemNo: INTEGER);
VAR
	itemType:		INTEGER;
	itemHandle:		Handle;
	itemBox:		Rect;
BEGIN
	GetDItem(DialogPtr(theWindow), itemNo, itemType, itemHandle, itemBox);

	PenNormal;
	FrameRect(itemBox);
END;


{---------------------------------------------------------------------------------
 ID2Item converts a family member id into the control item in the dialog.
 There’s got to be a better way to do this!
}
FUNCTION ID2Item(id: SignedByte): INTEGER;
VAR
	temp:	INTEGER;
BEGIN
	ID2Item := BWOnlyItem;		{a default value}
	temp := BAnd(id, $FF);		{turn the signed byte into an integer}
	CASE temp OF
		$8E, $89, $8A, $8F:		{all the 1 bit IDs for the various monitors supported}
			ID2Item := BWOnlyItem;
		$9E, $91, $9A, $9F:		{all the 2 bit IDs for the various monitors supported}
			ID2Item := FourItem;
		$96, $81, $92, $97:		{all the 4 bit IDs for the various monitors supported}
			ID2Item := SixteenItem;
		$86, $82, $87:			{all the 8 bit IDs for the various monitors supported}
			ID2Item := TwoFiftySixItem;
	END;
END;

{---------------------------------------------------------------------------------
 Item2ID converts a control item into the family member id.
 There’s got to be a better way to do this.
}
FUNCTION Item2ID(Item: INTEGER; oldID: SignedByte): SignedByte;
VAR
	base:	INTEGER;
BEGIN
	base := BAnd(oldID, IDMask);	{mask to the base ID}
	IF base = $00 THEN
		base := $86;				{if we have no base, assume High-Res Display}
	IF base = $81 THEN				{handle Page Display specially.  It can't have 8 bit mode}
		CASE Item OF
			BWOnlyItem:
				base := BOr(base, $08);
			FourItem:
				base := BOr(base, $10);
			SixteenItem:
				base := BOr(base, $00);
			TwoFiftySixItem:
				base := BOr(base, $00);	{just in case}
		END
	ELSE
		CASE Item OF
			BWOnlyItem:
				base := BOr(base, $08);
			FourItem:
				base := BOr(base, $18);
			SixteenItem:
				base := BOr(base, $10);
			TwoFiftySixItem:
				base := BOr(base, $00);
		END;
	Item2ID := base;
END;

{---------------------------------------------------------------------------------
 SetControl deselects all the radio buttons except for the item which is selected.
}
PROCEDURE SetControl(mDialog: DialogPtr; Item: INTEGER; numItems: INTEGER);
VAR
	i:			INTEGER;
	iType:		INTEGER;
	iHandle:	HANDLE;
	iBox:		Rect;
BEGIN
	Item := Item - numItems;						{convert into our range of item numbers}
	FOR i := BWOnlyItem TO TwoFiftySixItem DO
		BEGIN
			GetDItem(mDialog, i + numItems, iType, iHandle, iBox);
			SetCtlValue(ControlHandle(iHandle), ord(i = Item));		{set to 1 if selected item, else 0}
		END;
END;

{---------------------------------------------------------------------------------
 GetControl returns the item number of the radio button which is selected.
}
FUNCTION GetControl(mDialog: DialogPtr; numItems: INTEGER): INTEGER;
VAR
	i:			INTEGER;
	iType:		INTEGER;
	iHandle:	HANDLE;
	iBox:		Rect;
BEGIN
	GetControl := 0;							{default case}
	FOR i := BWOnlyItem TO TwoFiftySixItem DO
		BEGIN
			GetDItem(mDialog, i + numItems, iType, iHandle, iBox);
			IF GetCtlValue(ControlHandle(iHandle)) <> 0 THEN
				GetControl := i;
		END;
END;

FUNCTION GetDCEHand(gdHand: gdHandle): AuxDCEHandle;
BEGIN
	GetDCEHand := AuxDCEHandle(GetDCtlEntry(gdHand^^.gdRefNum));
END;

{---------------------------------------------------------------------------------
 GetRefNum returns a slot number’s driver refNum.
}
FUNCTION GetRefNum(Slot: INTEGER): INTEGER;
VAR
	curGD:		GDHandle;
BEGIN
	GetRefNum := 0;									{default case}
	curGD := GetDeviceList;
	WHILE (curGD <> NIL) DO
		BEGIN
			IF GetDCEHand(curGD)^^.dCtlSlot = Slot THEN
				BEGIN
					GetRefNum := curGD^^.gdRefNum;
					curGD := NIL;					{to force us out of the loop}
				END
			ELSE
				curGD := GetNextDevice(curGD);
		END;
END;

{---------------------------------------------------------------------------------
 GetVidDefault returns the family id for the currently selected family.
}
FUNCTION GetVidDefault(VAR Id: SignedByte; slot: INTEGER): OSErr;
CONST
	cscGetDefaultMode = 9; {This should be removed when the Pascal Equates are updated.}
VAR
	err:			OSErr;
	defaultRec:		VDDefaultRecord;
	vPB:			VideoParamBlock;
BEGIN
	{err := WeirdStatus(GetRefNum(slot), cscSetDefault, @defaultRec);}

	vPB.ioRefNum := GetRefNum(slot);
	vPB.csCode   := cscGetDefaultMode;
	vPB.csParam  := Ptr(@defaultRec);

	err := PBStatus(ParmBlkPtr(@vPB),false);

	IF err = noErr THEN
		Id := defaultRec.csID
	ELSE
		Id := 0;

	GetVidDefault := err;
END;

{---------------------------------------------------------------------------------
 GetMemoryUsed determines the amount of memory used by video for a particular max
 depth.  There’s got to be a better way to do this!
}
FUNCTION GetMemoryUsed(item: INTEGER; slot: INTEGER): LONGINT;
VAR
	id:			SignedByte;
	size:		LONGINT;
	factor:		LONGINT;
	pageSize:	LONGINT;
BEGIN
	size := 0;
	GetMemoryUsed := 0;
	IF Gestalt(gestaltLogicalPageSize, pagesize) <> NoErr THEN
		pagesize := $00008000;			{if no page size, assume 32k}
	IF GetVidDefault(id, slot) <> NoErr THEN
		Exit(GetMemoryUsed);
	CASE BAnd(id, IDMask) OF
		Mac2ID:							{Mac II 12" monitor-640x480}
			CASE item OF
				BWOnlyItem:
					size := $00009600;
				FourItem:
					size := $00012C00;
				SixteenItem:
					size := $00025800;
				TwoFiftySixItem:
					size := $0004B000;
			END;
		PortraitID:						{Portrait display-640x780}
			CASE item OF
				BWOnlyItem:
					size := $00010FE0;
				FourItem:
					size := $00021FC0;
				SixteenItem:
					size := $00043F80;
				TwoFiftySixItem:
					size := $00043F80;
			END;
		IIGSID:							{IIGS 12" monitor-512x384}
			CASE item OF
				BWOnlyItem:
					size := $00006000;
				FourItem:
					size := $0000C000;
				SixteenItem:
					size := $00018000;
				TwoFiftySixItem:
					size := $00030000;
			END;
		SEID:							{SE 9" monitor-512x342}
			CASE item OF
				BWOnlyItem:
					size := $00005580;
				FourItem:
					size := $0000A600;
				SixteenItem:
					size := $00015600;
				TwoFiftySixItem:
					size := $0002AC00;
			END;
	END;
	factor := size MOD pagesize;
	GetMemoryUsed := size + (pagesize - factor);
END;

{sets the id for the selected family.  Also sets the mode for restart}
FUNCTION SetVidDefault(Id: SignedByte; depth: INTEGER; slot: INTEGER; VAR Screens: ScrnRsrcHandle; ScreenNum: INTEGER): OSErr;
VAR
	err:			OSErr;
	defaultRec:		VDDefaultRecord;
	csParamPtr:		Ptr;
	spBlk:			spBlock;
	PRAMRec:		SPRAMRecord;
BEGIN
	defaultRec.csID := Id;
	csParamPtr := @defaultRec;
	err := Control(GetRefNum(slot), cscSetDefault, @csParamPtr);
	SetVidDefault := err;

	spBlk.spSlot := slot;
	spBlk.spResult := LongInt(@PRAMRec);
	err := sReadPRAMRec(@spBlk);
	spBlk.spsPointer := @PRAMRec;
	PRAMRec.vendorUse1 := depth;						{force to default mode}
	err := sPutPRAMRec(@spBlk);

	Screens^^.scrnRecs[ScreenNum].srMode := depth;		{force to default mode in scrn resource}
END;

{---------------------------------------------------------------------------------
 FindSlotStructfinds the video slot structure using the flags passed in
 (either all or only active).
}
FUNCTION FindSlotStruct(slot: INTEGER; VAR spBlk: spBlock; dataFlags: LongInt): OSErr;
VAR
	err:		OSErr;
	value:		Ptr;
BEGIN
	WITH spBlk DO							{set up the fields for the slot manager call}
		BEGIN
			spParamData := dataFlags;
			spSlot := slot;
			spExtDev := 0;
			spCategory := catDisplay;
			spCType := typeVideo;
			spDrvrSW := drSwApple;
			spHWDev := 0;
			spTBMask := 1;					{mask off hardware type field; match any vendor}
		END;
	err := SGetTypeSRsrc(@spBlk);

	FindSlotStruct := err;
END;

{---------------------------------------------------------------------------------
 NumFamilies finds the number of active and inactive video sResources.
}
FUNCTION NumFamilies(slot: INTEGER): INTEGER;
VAR
	count:		INTEGER;
	spBlk:		spBlock;
	err:		OSErr;
BEGIN
	count := 0;
	err := NoErr;
	spBlk.spId := 0;
	WHILE (err = NoErr) DO
		BEGIN
			err := FindSlotStruct(slot, spBlk, spFlagAll+spFlagNext+spFlagOneSlot);	{force to find invalid sRsrcs}
			IF err = NoErr THEN
				count := count + 1;
		END;
	IF count = 0 THEN						{if we didn't find any, then there MUST be the active one}
		count := 1;

	NumFamilies := count;
END;

{---------------------------------------------------------------------------------
 GetActiveFamily gets the active member of the video family.
}
FUNCTION GetActiveFamily(slot: INTEGER): SignedByte;
VAR
	spBlk:		spBlock;
	err:		OSErr;
BEGIN
	GetActiveFamily := 0;
	spBlk.spId := 0;
	err := FindSlotStruct(slot, spBlk, spFlagNext+spFlagOneSlot);
	IF err = NoErr THEN
		GetActiveFamily := spBlk.spID;
END;

{---------------------------------------------------------------------------------
 SetMemUsed sets the string indicating the amount of memory that will be used/freed
 for a given selection.
}
PROCEDURE SetMemUsed(numItems, item: INTEGER; mDialog: DialogPtr; slot, localID: INTEGER);
VAR
	iType:		INTEGER;
	iHandle:	HANDLE;
	iBox:		Rect;
	defaultID:	SignedByte;
	size:		LONGINT;
	newText:	STR255;
	titleText:	STR255;
BEGIN
	GetDItem(mDialog, MemoryItem + numItems, iType, iHandle, iBox);
	defaultID := GetActiveFamily(slot);
	size := GetMemoryUsed(Id2Item(defaultID), slot) - GetMemoryUsed(item - numItems, slot);
	IF size = 0 THEN
		newText := ''
	ELSE
		BEGIN
			NumToString(abs(size) DIV 1024, newText);
			IF size < 0 THEN
				BEGIN
					GetIndString(titleText, localID, 1);
					NewText := concat(titleText, newText);
					GetIndString(titleText, localID, 2);
					NewText := concat(newText, titleText);
				END
			ELSE
				BEGIN
					GetIndString(titleText, localID, 3);
					NewText := concat(titleText, newText);
					GetIndString(titleText, localID, 4);
					NewText := concat(newText, titleText);
				END;
		END;
	SetIText(iHandle, newText);
END;

{---------------------------------------------------------------------------------
 SetLine sets the item to our line drawing routine.
}
PROCEDURE SetLine(item, numItems: INTEGER; superUser: LONGINT; mDialog: DialogPtr);
VAR
	iType:		INTEGER;
	iHandle:	HANDLE;
	iBox:		Rect;
BEGIN
	GetDItem(mDialog, item + numItems, iType, iHandle, iBox);	{put in our line}

	{
		Draw the divider line in gray (when in superuser mode) and the other lines
		solid.
	}
	IF item = divLineItem THEN
		BEGIN
		IF superUser = 1 THEN
			SetDItem(mDialog, item + numItems, iType, @DrawDivLine, iBox)
		END
	ELSE
		SetDItem(mDialog, item + numItems, iType, @DrawLine, iBox);
END;

{---------------------------------------------------------------------------------
 CenterRect centers a rectangle on another rectangle.
}
PROCEDURE CenterRect(VAR rectToCenter: Rect; rectToCenterOn: Rect);
VAR
	ptToCenterTo:	Point;
	oldCenter:		Point;
BEGIN
	WITH rectToCenterOn DO
		BEGIN
			ptToCenterTo.v := top + (bottom - top) DIV 2;
			ptToCenterTo.h := left + (right - left) DIV 2;
		END;
	WITH rectToCenter DO
		BEGIN
			oldCenter.v := top + (bottom - top) DIV 2;
			oldCenter.h := left + (right - left) DIV 2;
		END;
	OffsetRect(rectToCenter, ptToCenterTo.h - oldCenter.h, ptToCenterTo.v - oldCenter.v);
END;

{---------------------------------------------------------------------------------
 MyAlert performs an alert after moving the window over the control panel
 (front window).
}
FUNCTION MyAlert(alertID: INTEGER; filterProc: ProcPtr): INTEGER;
VAR
	alertHand:	AlertTHndl;
	r:			Rect;
	mWindow:	WindowPtr;
	savedPort:	GrafPtr;
BEGIN
	mWindow := FrontWindow;

	InitCursor;
	alertHand := AlertTHndl(GetResource('ALRT', alertID));
	r := mWindow^.portRect;
	LocalToGlobal(r.topLeft);
	LocalToGlobal(r.botRight);
	CenterRect(alertHand^^.boundsRect, r);
	MyAlert := NoteAlert(alertID, filterProc);

	{
	  If I don’t do the following, then the Monitors window doesn’t get updated properly
	  under System 7.0.  I should probably clip around the area where the NoteAlert
	  came up, but I’m hoping that this code can be dumped. -- jmp
	 }
	GetPort(savedPort);
	SetPort(mWindow);

	InvalRect(mWindow^.portRect);
	SetPort(savedPort);
END;

{---------------------------------------------------------------------------------
 EDialog is the main code.
}
FUNCTION EDialog(message,					{the action to handle}
		item,								{the DITL item or 0}
		numItems:			INTEGER;		{the number of the systems last item}
		monitorValue:		LONGINT;		{the value returned last time.  defaults to zero}
		mDialog:			DialogPtr;		{the dialog pointer}
		theEvent:			EventRecord;	{the event that caused the message to happen}
		ScreenNum:			INTEGER;		{the number of the screen to affect}
		VAR Screens:		ScrnRsrcHandle;	{the screen resource to change if desired}
		VAR ScrnChanged:	BOOLEAN			{TRUE if we should clean up Screens}
		): LONGINT;							{value to pass back in or error if negative}
VAR
		iType:			INTEGER;
		iHandle:		HANDLE;
		iBox:			Rect;
		err:			OSErr;
		defaultID:		SignedByte;
		itemFound:		INTEGER;
		id:				SignedByte;
		depth:			INTEGER;
		num:			INTEGER;
		rectHdl:		RectHandle;
		theName:		STR255;
		found:			BOOLEAN;
		baseID:			INTEGER;
		localID:		INTEGER;
		theType:		ResType;
		i:				INTEGER;

		disposeStorage:	BOOLEAN;			{true when time to dispose private storage}
		savedResFile:	INTEGER;			{for saving/restoring the current resFile}
		codeHandle:		Handle;				{for remembering handle to mntr resource}

BEGIN
	{
		Remember our entry point…
	}
	codeHandle := Recoverhandle(@Entry);

	{
		Scary stuff here!
	}
	GetResInfo(codeHandle, baseID, theType, theName);
	baseID  := BAND(baseID, $FFE0);
	localID := baseID + $10;

	{
		We’ll determine dynamically whether storage needs to be thrown away
		this time or not.
	}
	disposeStorage := FALSE;

	{
		Handle the messages…
	}
	CASE message OF
		STARTUPMSG:
			BEGIN
				theName	:= GetCardName(Screens^^.scrnRecs[ScreenNum].srSlot);
				found	:= FALSE;

				savedResFile := CurResFile;
				UseResFile(HomeResFile(codeHandle));

				FOR i := localID TO (localID + numShow) DO
					BEGIN
						iHandle := Get1Resource('card',i);
						IF iHandle <> NIL THEN
							IF theName = StringHandle(iHandle)^^ THEN
								found := TRUE;
					END;
				UseResFile(savedResFile);

				monitorValue := LONGINT(NewHandle(sizeof(INTEGER)));

				IF monitorValue <> LONGINT(NIL) THEN
					BEGIN
						IF NOT found THEN
							disposeStorage := TRUE
						ELSE
							BEGIN
								num := NumFamilies(Screens^^.scrnRecs[ScreenNum].srSlot);
								rectHdl := RectHandle(GetResource('RECT', baseID));
								IF rectHdl <> NIL THEN
									BEGIN
										HNoPurge(Handle(rectHdl));	{so the resource won't be purged}
										rectHdl^^.top := rectHdl^^.top + (ButtonDistance * (4 - num));

										Handle(monitorValue)^^ := item;
									END
								ELSE
									disposeStorage := TRUE;
							END;
					END;
			END;

		INITMSG:
			BEGIN
				{
					Set the line separating us from the default controls.
				 }
				SetLine(divLineItem, numItems, Handle(monitorValue)^^, mDialog);

				{
					Find how many controls are showing and select the correct border rect.
				}
				CASE NumFamilies(Screens^^.scrnRecs[ScreenNum].srSlot) OF
					2:
						SetLine(rect1Item, numItems, Handle(monitorValue)^^, mDialog);
					3:
						SetLine(rect2Item, numItems, Handle(monitorValue)^^, mDialog);
					OTHERWISE
						SetLine(rect3Item, numItems, Handle(monitorValue)^^, mDialog);
				END;

				{
				 	Select the approriate control.  If we can't get the default, try for the active
					family member.
				}
				err := GetVidDefault(defaultID, Screens^^.scrnRecs[ScreenNum].srSlot);
				IF err <> noErr THEN
					defaultID := GetActiveFamily(Screens^^.scrnRecs[ScreenNum].srSlot);

				itemFound := ID2Item(defaultID) + numItems;
				GetDItem(mDialog, itemFound, iType, iHandle, iBox);
				SetCtlValue(ControlHandle(iHandle), 1);

				{
				 	Define the default control by appending message to the name of the active
					control.
				}
				defaultID := GetActiveFamily(Screens^^.scrnRecs[ScreenNum].srSlot);
				itemFound := ID2Item(defaultID) + numItems;
				GetDItem(mDialog, itemFound, iType, iHandle, iBox);
				GetCTitle(ControlHandle(iHandle), theName);
				theName := concat(theName, GetString(localID)^^);
				SetCTitle(ControlHandle(iHandle), theName);
			END;

		HITMSG:
			BEGIN
				SetControl(mDialog, Item, numItems);
				SetMemUsed(numItems, Item, mDialog, Screens^^.scrnRecs[ScreenNum].srSlot, localID);
			END;

		OKMSG:
			BEGIN
				itemFound := GetControl(mDialog, numItems);
				IF itemFound <> 0 THEN
					BEGIN
						err := GetVidDefault(defaultID, Screens^^.scrnRecs[ScreenNum].srSlot);
						id := Item2ID(itemFound, defaultID);
						IF (err = noErr) AND (id <> defaultID) THEN
							BEGIN
								depth := $80 + (itemFound - BWOnlyItem); {This is ugly, but still works.}
								err := SetVidDefault(id, depth, Screens^^.scrnRecs[ScreenNum].srSlot, Screens, ScreenNum);
							END;
						IF (err = noErr) AND (id <> GetActiveFamily(Screens^^.scrnRecs[ScreenNum].srSlot)) THEN
							BEGIN
								ScrnChanged := TRUE;
								Ptr(MonitorsMem)^ := BAnd(Ptr(MonitorsMem)^, BNot(BSL(1, ScreenNum)));
								err := MyAlert(localID, NIL);
							END;
					END;

				disposeStorage := TRUE;
			END;

		CANCELMSG:
			disposeStorage := TRUE;
	END;

	{
		Common area for disposing of storage for “superUser” vs. “normalUser” flag.  Note
		that if disposeStorage is TRUE at start, the monitorValue being set to errorValue
		is the right thing to do.  Also, since the okMsg and cancelMsg are “then end,” setting
		monitorValue to an errorValue doesn’t matter.
	}
	IF disposeStorage THEN
		BEGIN
			DisposHandle(Handle(monitorValue));
			monitorValue := errorValue;
		END;

	{
		Return the result…
	}
	EDialog := monitorValue;
END;

END.


