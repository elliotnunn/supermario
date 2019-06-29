(*

	© 1988 by Apple Computer, Inc.
	All Rights Reserved

	LittleDialog -- a Hypercard XCMD that displays a modal dialog. This example uses
	callbacks to HyperCard provided by the HyperXLib.o library, introduced with MPW 3.0.
	The library provides a consistent set of routines to call HyperCard internal routines
	from any language. This is the first public demonstration of its use.

	
	Form: LittleDialog DisplayText
	
	Example: LittleDialog "Display this very long text string that wouldn't fit in Answer."
	
	DisplayText: The text to display in the window. Up to 255 characters may be displayed.
	

	To compile and link this file using MPW Pascal, select the following lines and
	press ENTER:
	
	make LittleDialog.p
	
	or:
	
	Pascal LittleDialog.p -mbg off
	Link -w -rt XCMD=10002 ∂
		-m ENTRYPOINT ∂
		-sg LittleDialog ∂
		LittleDialog.p.o ∂
		"{Libraries}HyperXLib.o" ∂
		"{Libraries}Interface.o" ∂
		"{PLibraries}PasLib.o" ∂
		-o "HyperXExample Stack"
	Rez LittleDialog.r -o "HyperXExample Stack" -append

*)

{$R-}
{$Z+}

UNIT DummyUnit;


INTERFACE

USES
	Types,
	Memory,
	QuickDraw,
	OSEvents,
	Dialogs,
	Resources,
	PasLibIntf,
	HyperXCmd;


IMPLEMENTATION


PROCEDURE LittleDialog(paramPtr: XCmdPtr);		FORWARD;

PROCEDURE ENTRYPOINT(paramPtr: XCmdPtr);
  
	BEGIN
		LittleDialog(paramPtr);
	END;


	{*
	 * Draw the ring around the OK button, the way that
	 * alert boxes get it. This procedure is only called 
	 * by the Dialog Manager for update events.
	 *
•••• NOTE: In Pascal, code referenced in an XCMD using the @ operator
••••       must reside outside of the main body of the XCMD code.
	 *}
	 
PROCEDURE DrawOKDefault(theDialog: DialogPtr; theItem: INTEGER);
	CONST
		curveRad = 16;

	VAR
		savePen:					PenState;
		tempType:					Integer;
		tempHandle:				Handle;
		tempRect:					Rect;

	BEGIN
		GetPenState(savePen); {save the old pen state}
		
		{ Draw an outline around our default button.}
		
		GetDItem(theDialog, theItem, tempType, tempHandle, tempRect); {get the item’s rect}
		PenSize(3,3); {make the pen fatter}
		FrameRoundRect(tempRect,curveRad,curveRad); {draw the ring}

		SetPenState(savePen); {restore the pen state}
	END; {DrawOKDefault}


PROCEDURE LittleDialog(paramPtr: XCmdPtr);

	CONST
				dialogID = 10002;
				iDefOKRing = OK+1;		{ number of user item for ring around default button }
				gap = -4;

	VAR
  			myDialogPtr:	 		DialogPtr;
				savePort:					GrafPtr;
				itemHit:					INTEGER;
				tempType:					INTEGER;
				tempHandle:				Handle;
				ctlRect:					Rect;
				displayStr:				Str255;


	PROCEDURE ErrAbort(errMsg: Str255); {Abort execution returning an error message}
		BEGIN
			SetPort(savePort); {restore the port}
			paramPtr^.returnValue := PasToZero(paramPtr,errMsg); {XCMDs can return values}
			EXIT(LittleDialog); {leave the XCMD}
		END;


	BEGIN {Main}

		{ Save the current port; good defensive programming practice.
		NOTE: this port will always be the card window's grafport. }
		
		GetPort(savePort);
		
		{ It is always a good idea to check for the correct number of parameters.}
		{ Another nice idea is to return the proper syntax of the call if it has
			been called improperly.}
		
		IF (paramPtr^.paramCount <> 1) THEN
			ErrAbort('Correct usage is: "LittleDialog fieldName"');
		
		{ This routine will use DLOG and DITL resources that must be available.
			Since XCMDs may be moved by ResEdit without knowledge of those resources,
			we must check for their availability. To suggest that our DLOG, DITL, and
			XCMD all belong together, we have numbered them all the same: 10002.}
		
		IF (GetResource('DLOG',dialogID) = NIL) OR
			 (GetResource('DITL',dialogID) = NIL) THEN
			ErrAbort('XCMD requires resources: DLOG 10002 & DITL 10002.');

		ZeroToPas(paramPtr,paramPtr^.params[1]^,displayStr); {the string to display}
		ParamText(displayStr,'','',''); {DLOG 10002 already set up for this call}
		SetDAFont(0);		{ * possibly redundant, but safe nonetheless}

		myDialogPtr := GetNewDialog(dialogID,NIL,POINTER(-1)); {get ptr to invisible dialog}
		IF myDialogPtr = NIL THEN
			ErrAbort('Not enough memory for dialog.');

		{Move our default-OK-button userItem to around the OK button, and set its
			item handle to be a pointer to our other drawing procedure}

		GetDItem(myDialogPtr,OK,tempType,tempHandle,ctlRect); {get rect of OK button}
		InsetRect(ctlRect,gap,gap); {make the rect a little bigger}
			{set the same old type, our procptr, and the new box}
		SetDItem(myDialogPtr,iDefOKRing,userItem+itemDisable,@DrawOKDefault,ctlRect);

		ShowWindow(myDialogPtr); {Make the dialog visible}
			
		{ Set the cursor to the arrow for use with the dialog box.}
			
		InitCursor;
		
		{ Make the modal dialog handle events. }
		
		FlushEvents(everyEvent,0); {flush all events to be safe}
		REPEAT
			ModalDialog(NIL,itemHit); {stay modal until the user clicks OK}
		UNTIL itemHit = OK;
		
		{ Let HyperCard set the cursor to a known state, so the next "idle" message
			after the dialog goes away will reset it to the correct cursor. (HyperCard
			doesn't know we did an InitCursor.)}
			
		SendHCMessage(paramPtr,'set cursor to 4');	{4 = the watch cursor}

		DisposDialog(myDialogPtr); {get rid of dialog and dialog handle}
		SetPort(savePort); {restore the port}

	END; {main}
	
END. {LittleDialog}
