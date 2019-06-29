/*------------------------------------------------------------------------------
#
#	Macintosh Developer Technical Support
#
#	EditText Sample Control Panel Device
#
#	EditCdev
#
#	EditCdev.c	-	C Source
#
#	Copyright © Apple Computer, Inc. 1988-1990
#	All rights reserved.
#
#	Versions:	1.1					7/88
#				1.0					6/88
#
#	Components:	EditCdev.c			Feb.  1, 1990
#				EditCdev.r			Feb.  1, 1990
#				EditCdev.make		Feb.  1, 1990
#
#	EditCdev demonstrates how to implement an editText item
#	in a Control Panel Device.  It utilizes the new undo, cut, copy,
#	paste, and delete messages that are sent to cdevs in
#	response to user menu selections.  How to handle private
#	storage is also covered.
#
------------------------------------------------------------------------------*/

#include <Types.h>
#include <Memory.h>
#include <Quickdraw.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Devices.h>
#include <Scrap.h>

/* Constants */
#define	textItm		1			/* first editTExt item in cdev */


/* Types */
typedef struct CDEVRec {
	TEHandle	myTE;
} CDEVRec, *CDEVPtr, **CDEVHnd;
	
/* Prototypes */
void DoEditCommand (short message, DialogPtr CPDialog);

/* This is the main dispatcher. It must be the first code in the cdev.
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
 the text.*/
 
pascal Handle
TextCDEV(short message, short item, short numItems, short CPanelID,
		 EventRecord *theEvent, Handle cdevStorage, DialogPtr CPDialog)
{												
	#pragma unused (item, CPanelID)		/* unused formal parameters */
	
	char		tempChar;

	if (message == macDev) return((Handle) 1);				/* we work on every machine */
	else if (cdevStorage != nil) {
		switch (message) {
			case initDev:									/* initialize cdev */
				cdevStorage = NewHandle(sizeof(CDEVRec));	/* create provate storage */
				SelIText(CPDialog, numItems + textItm, 0, 999); /* make caret show up */
				break;
				
			case hitDev:									/* handle hit on item */
			case closeDev:									/* clean up and dispose */
			case nulDev:
			case updateDev:									/* handle any update drawing */
			case activDev:									/* activate any needed items */
			case deactivDev:								/* deactivate any needed items */
				break;
				
			case keyEvtDev:									/* respond to keydown */
				tempChar = theEvent->message & charCodeMask;/* get the character, and check */
				if (theEvent->modifiers & cmdKey) {			/*  status of command key */
					message = nulDev;						/* start with no message */
					theEvent->what = nullEvent;				/* and empty event type */
					
					switch (tempChar) {						/* set appropriate message */
						
						case 'X':
						case 'x':
							message = cutDev;
							break;
						case 'C':
						case 'c':
							message = copyDev;
							break;
						case 'V':
						case 'v':
							message = pasteDev;
							break;
					}
					DoEditCommand(message, CPDialog);		/* Let edit command handler take it */
				}
				break;
				
			case macDev:
			case undoDev:
				break;
				
			case cutDev:
			case copyDev:
			case pasteDev:
			case clearDev:
				DoEditCommand(message, CPDialog);			/* respond to edit command */
				break;
		}

		return (cdevStorage);
	}  /* cdevStorage != nil */
	
	/* 
	**	if cdevStorage = NIL then ControlPanel 
	**  will put up memory error
	*/
	return (nil);
}

/* Call the appropriate Dialog Manager routine to handle an edit command for
 	an editText item. It will do all the work regarding the TEScrap. */
void 
DoEditCommand (short message, DialogPtr CPDialog)
{
	switch (message) {
	case cutDev:
		DlgCut(CPDialog);
		break;
	case copyDev:
		DlgCopy(CPDialog);
		break;
	case pasteDev:
		DlgPaste(CPDialog);
		break;
	case clearDev:
		DlgDelete(CPDialog);
		break;
	}
}

