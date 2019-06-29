{------------------------------------------------------------------------------
#
#	Macintosh Developer Technical Support
#
#	EditText Sample Control Panel Device
#
#	EditCdev
#
#	EditCdev.p	-	Pascal Source
#
#	Copyright © Apple Computer, Inc. 1988-1990
#	All rights reserved.
#
#	Versions:	1.1					7/88
#				1.0					6/88
#
#	Components:	EditCdev.p			Feb.  1, 1990
#				EditCdev.r			Feb.  1, 1990
#				EditCdev.make		Feb.  1, 1990
#
#	EditCdev demonstrates how to implement an editText item
#	in a Control Panel Device.  It utilizes the new undo, cut, copy,
#	paste, and delete messages that are sent to cdevs in
#	response to user menu selections.  How to handle private
#	storage is also covered.
#
------------------------------------------------------------------------------}


UNIT EditTextCDEV;

INTERFACE

USES

	MemTypes, QuickDraw, OSIntf, ToolIntf, PackIntf;

CONST

	textItm		= 1;			{first editTExt item in cdev}

TYPE

	CDEVRec	= RECORD
				myHandle	: Handle;
				END;
	CDEVPtr	= ^CDEVRec;
	CDEVHnd	= ^CDEVPtr;
	
FUNCTION TextCDEV (message, item, numItems, CPanelID: INTEGER;
											VAR theEvent: EventRecord;
											cdevStorage: Handle;
											CPDialog: DialogPtr) : Handle;
												
IMPLEMENTATION

PROCEDURE DoEditCommand (message: INTEGER; CPDialog: DialogPtr); FORWARD;

FUNCTION TextCDEV (message, item, numItems, CPanelID: INTEGER;
											VAR theEvent: EventRecord;
											cdevStorage: Handle;
											CPDialog: DialogPtr) : Handle;
{This is the main dispatcher. It must be the first code in the cdev.
 EditCdev's dispatcher responds only to the following messages from
 the Control Panel:
 	
	macDev		- To indicate what machines it is available on.
	initDev		- To set up some temporary storage and get the caret started.
	keyEvtDev	- To check for an edit command and do the appropriate action.
	cutDev		- To cut the current selection.
	copyDev		- To copy the current selection.
	pasteDev	- To paste the contents of the clipboard.
	clearDev	- To delete the current selection.

 The Dialog Manager's services are used to handle entry of text, selection
 of text, editing of text, and moving between the editText items via the
 tab key. Since the Dialog Manager handles the selection of text, we do not
 have to be concerned with hitDev messages for the editText items. The only
 things we have to take care of are calling the Dialog Manager editing
 routines in response to an edit command, and getting the caret to show up
 at the beginning. In response to an edit command that was the result of
 a command-key equivalent, we must also eliminate the event so that it does
 not get processed as a keyDown by the Dialog Manager. Otherwise, an 'x'
 would show up in the editText item when the user did a command-x to cut
 the text.}
 
VAR
	tempChar	: CHAR;
BEGIN
IF message = macDev THEN TextCDEV := Handle(1)			{we work on every machine}
ELSE IF cdevStorage <> NIL THEN BEGIN
	CASE message OF
	initDev:											{initialize cdev}
		BEGIN
		cdevStorage := NewHandle(SIZEOF(CDEVRec));		{create private storage}
		SelIText(CPDialog, numItems + textItm, 0, 999);	{make caret show up}
		END;
	hitDev:;
	closeDev:;
	nulDev:;
	updateDev:;
	activDev:;
	deActivDev:;
	keyEvtDev:											{respond to key down}
		BEGIN
		{first, get the character}
		tempChar := CHR(BAnd(theEvent.message, charCodeMask));
		{then see if the command key was down}
		IF BAnd(theEvent.modifiers, cmdKey) <> 0 THEN BEGIN
			message := nulDev;							{start off with no message}
			theEvent.what := nullEvent;					{wipe out event}
			CASE tempChar OF							{set appropriate message}
				'X','x':
					message := cutDev;
				'C','c':
					message := copyDev;
				'V','v':
					message := pasteDev;
				END;
			DoEditCommand(message, CPDialog);			{let edit command handler take it}
			END;
		END;
	macDev:;
	undoDev:;
	cutDev, copyDev, pasteDev, clearDev:
		DoEditCommand(message, CPDialog);				{respond to edit command}
	END;  {CASE message}
	TextCDEV := cdevStorage;
	{if cdevStorage = NIL then ControlPanel will put up memory error}
	END;  {cdevStorage <> NIL}
END; {TextCDEV}

PROCEDURE DoEditCommand (message: INTEGER; CPDialog: DialogPtr);

{Call the apprpriate Dialog Manager routine to handle an edit command for
 an editText item. It will do all the work regarding the TEScrap.}
 
BEGIN
CASE message OF
	cutDev:
		DlgCut(CPDialog);
	copyDev:
		DlgCopy(CPDialog);
	pasteDev:
		DlgPaste(CPDialog);
	clearDev:
		DlgDelete(CPDialog);
	END;
END; {DoEditCommand}

END.  {EditTextCDEV}
