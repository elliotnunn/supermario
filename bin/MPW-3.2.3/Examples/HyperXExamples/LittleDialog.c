/*

	© 1988 by Apple Computer, Inc.
	All Rights Reserved

	LittleDialog -- a Hypercard XCMD that displays a modal dialog. This example uses
	callbacks to HyperCard provided by the HyperXLib.o library, introduced with MPW 3.0.
	The library provides a consistent set of routines to call HyperCard internal routines
	from any language. This is the first public demonstration of its use.

	
	Form: LittleDialog DisplayText
	
	Example: LittleDialog "Display this very long text string that wouldn't fit in Answer."
	
	DisplayText: The text to display in the window. Up to 255 characters may be displayed.
	

	To compile and link this file using MPW C, select the following lines and
	press ENTER:
	
	make LittleDialog.c
	
	or:
	
	C -b LittleDialog.c -mbg off
	Link -w -rt XCMD=10002 ∂
		-m ENTRYPOINT ∂
		-sg LittleDialog ∂
		LittleDialog.c.o ∂
		"{Libraries}HyperXLib.o" ∂
		"{Libraries}Interface.o" ∂
		"{CLibraries}StdCLib.o" ∂
		"{Libraries}Runtime.o" ∂
		-o "HyperXExample Stack"
	Rez LittleDialog.r -o "HyperXExample Stack" -append
	
*/

#include <Types.h>
#include <Dialogs.h>
#include <Memory.h>
#include <OSEvents.h>
#include <QuickDraw.h>
#include <Resources.h>
#include <HyperXCmd.h>

#define	dialogID	(short) 10002
#define DLOG			(long)	0x444C4F47
#define DITL			(long)	0x4449544C		
#define	gap				(int)		-4
#define	defaultOK	(short) 2


pascal void EntryPoint(XCmdPtr paramPtr)
{

	pascal void DrawOKDefault();
	void				ErrAbort();
	GrafPtr			savePort;
	Str255			displayStr;
	DialogPtr		myDialogPtr;
	Rect				itemRect;
	short				itemType;
	Handle			item;
	short				itemHit;
	
	/*
		Save the current port; good defensive programming practice.
		NOTE: this port will always be the card window's grafport.
	*/
	GetPort(&savePort);
												
	/*	
		It is always a good idea to check for the correct number of parameters.
		Another nice idea is to return the proper syntax of the call if it has
		been called improperly.
	*/
	if(paramPtr->paramCount != 1)
		{
			ErrAbort(paramPtr,"Correct usage is: 'LittleDialog fieldName'");
			return;
		}
		
	/*		
		This routine will use DLOG and DITL resources that must be available.
		Since XCMDs may be moved by ResEdit without knowledge of those resources,
		we must check for their availability. To suggest that our DLOG, DITL, and
		XCMD all belong together, we have numbered them all the same: 10002.
	*/
	if((GetResource((ResType)DLOG,dialogID) == nil) ||
		(GetResource((ResType)DITL,dialogID) == nil))
		{
			ErrAbort(paramPtr,"XCMD requires resources: DLOG 10002 & DITL 10002.");
			return;
		}

	/*
		Convert the zero-terminated string that was the parameter to the XCMD
		into a pascal string.  Set it up to be the string that will be displayed.
		DLOG 10002 is already set up to display a text string.
	*/
	HLock(paramPtr->params[0]);
	ZeroToPas(paramPtr,*(paramPtr->params[0]),displayStr); 
	HUnlock(paramPtr->params[0]);
	ParamText(displayStr,"","",""); 
	SetDAFont(0);										/* possibly redundant, but safe nonetheless */

	/*
		Get a pointer to the not yet visible dialog.
		Abort with error message if lacking memory.
	*/
	myDialogPtr = GetNewDialog(dialogID,nil,(WindowPtr)-1);
	if (myDialogPtr == nil) 
		{
			ErrAbort(paramPtr,"Not enough memory for dialog.");
			return;
		}
		
	/*
		Get the rect of the OK button and enlarge it slightly.
		Use it to surround user item #2 to make a default (ringed) button.
		The function DrawOKDefault will draw the button.
	*/
	GetDItem(myDialogPtr,ok,&itemType,&item,&itemRect); 
	InsetRect(&itemRect,gap,gap); 							
	SetDItem(myDialogPtr,defaultOK,(userItem | itemDisable),(Handle)&DrawOKDefault,&itemRect);
		
	/* Make the dialog visible. */
	
	ShowWindow(myDialogPtr);

	/* Set the cursor to the arrow for use with the dialog box. */
		
	InitCursor();
	
	/* Flush all events to be safe. */		

	FlushEvents(everyEvent,0); 
	
	/*
		Make the modal dialog handle events.
		Display the dialog until the user clicks OK.
		Then get rid of dialog and dialog handle.
		And restore the port.
	*/
	do
		{
			ModalDialog(nil,&itemHit);
		}
	while (itemHit != ok);

		/*
			Let HyperCard set the cursor to a known state, so the next "idle" message
			after the dialog goes away will reset it to the correct cursor. (HyperCard
			doesn't know we did an InitCursor.)
		*/
			
	SendHCMessage(paramPtr,"set cursor to 4");	/* 4 = the watch cursor */

	/*
		Then get rid of dialog and dialog handle.
		And restore the port.
	*/
	DisposDialog(myDialogPtr); 	
	SetPort(savePort); 				
	
}


void ErrAbort(XCmdPtr paramPtr, char *str)
{
	Handle  nuHndl;

	/*
		Allocate space for an error message.
		Copy the string into it.
		Return the handle to HyperCard.
	*/
	nuHndl = NewHandle((long)(strlen(str)+1));
	if (nuHndl == nil) return;
	strcpy((char *)*nuHndl,str);
	paramPtr->returnValue = nuHndl;
}


#define curveRad	(short) 16
#define	nuPenSize	(short) 3

pascal void DrawOKDefault(DialogPtr theDialog, short theItem)
{
	PenState	savePen;
	short			itemType;
	Handle		item;
	Rect			itemRect;
	
	/*
		Save the pen state.
		Draw an outline with a larger pen around our default OK button.
		Then restore the pen state.
	*/
	GetPenState(&savePen);
	GetDItem(theDialog,theItem,&itemType,&item,&itemRect);
	PenSize(nuPenSize,nuPenSize);
	FrameRoundRect(&itemRect,curveRad,curveRad);
	SetPenState(&savePen);
}

