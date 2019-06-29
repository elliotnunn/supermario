/*
	File:		TSMFunctions.c

	Contains:	Text Services Manager functions.

	Written by:	Kenny SC. Tung

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<56>	10/30/92	DTY		Use Get/Set macros to access ExpandMem.
		<55>	  8/4/92	DCL		Changed spelling of tsmUnSupportedTypeErr to
									tsmUnsupportedTypeErr (lowercased the s)
		<54>	 7/16/92	KST		#1034952 <Kda>: Clear the 'kTSMEventBeenCalled' bit in
									TSMEvent().
		<53>	 7/13/92	KST		#1035594 <JH>: In xActivateTSMDocument(), I assumed when a TSM
									aware application calls ActivateTSMDocument(), the other
									document has been deactivated (as the ERS said). This is
									apparently not a wise assumption because application writer
									could be as dumb as me and will make mistake. (Like the one they
									do to produce this bug).
		<52>	  7/9/92	KST		#1035067 <JH>: When there is no TSM document in a TSM aware
									application, we call UseInputWindow (nil, true). But that fix
									did not check re-entrant case. So this bug happens. The fix is
									to check the flag, if it is already set, then we know we have
									been called again and just return.
		<51>	  7/8/92	KST		#1033475 <JH>: Changed the routine name utHasNoInputMethod() to
									utScriptIsNotCCJK() to be more descriptive.
		<50>	  7/6/92	KST		#1033507,1033505 <JH>: Clear the kIMJustSetCursor bit before the
									SetTextServiceCursor() call so that IM can change cursor from
									inside of the input area.
		<49>	 6/26/92	KST		<JH>: In case application opens the input method as a text
									service (event we documented not to do so), do not remove/delete
									its menu.
		<48>	 6/26/92	KST		#1033450,1033456,1031316 <JH>: When creating a new TSM document,
									we just open an input method's instance but don't activate it.
		<47>	 6/23/92	KST		Added an important comment for DeleteTSMDocument. (Just
									documentation changes only).
		<46>	 6/17/92	KST		#1032783 <JH>: Checking for interface type must use '&&' not
									'||'. All right, John, stop laughing.
		<45>	 6/17/92	KST		#1032783 <JH>: The refcon was putting in the AE record with the
									wrong keyword, it should be keyAETSMDocumentRefcon.
		<44>	 6/17/92	KST		#1032643 <JH>: When an application deletes all its TSM
									documents, TSM will save some states info and turn global
									UseInputWindow on for that app. This will allow user to use IM's
									pallete and menu. Also when switch to Finder and then switch
									back, the pencil will be correct.
		<43>	 6/15/92	KST		Removed debugger statements.
		<42>	 6/14/92	KST		#M. Sang, <JH>: 1. If a document is using input window and we
									deactivate that document, FixTSMDocument was called in the wrong
									place. 2. When DeleteTSMDocument calls utCloseIM4Document, we
									remove IM's menu only from system menu list but keep it in app's
									menu so that when a TSM aware application (using Akiko in Roman
									promary script) deletes all the TSM documents, Akiko's menu will
									still stays in the menu bar.
		<41>	 6/14/92	KST		#1032466 <JH>: Fixed the bug when running a TSM aware
									application in Roman primary script, select SJip and then Akiko,
									Akiko is not activated.
		<40>	 6/10/92	KST		#1031142,1030881 <JH>: Since we removed the docID param from
									TSMEvent, UseInputWindow has been broken because keydown event
									is routed to SWM recursively. Fix it with a TSM global keeping
									track of which doc is using the input window. And added a new
									private call for SWM -- TSMEventFromSWM.
		<39>	  6/2/92	JH		#1028635,1030481,1030631,1030634,1030647 <KST>: Added calls to
									the new routine in InternalUtils to add input method menus as
									system menus.
		<38>	 5/18/92	KST		#1029814 <JH>: NewTSMDocument should check the supported
									interface type and number of interfaces.
		<37>	  5/6/92	KST		#1028642,<JH>: Fixed the bug: "floating pallete stays when
									switch from Akiko to Roman in TSM aware application".
		<36>	  5/1/92	KST		#1027482,<JH>: If input method changed cursor, then we set a
									flag in TSM globals to indicate so.
		<35>	  5/1/92	JH		Use the result of SetTextServiceCursor to set a flag telling our
									SetCursor patch whether it should let SetCursor calls go through
									or not.
		<34>	 4/20/92	YK		#1026761: Before calling InsertMenu and DrawMenuBar, make sure
									that the menu handle is not NIL.
		<33>	 4/10/92	KST		JH,Fixed a bug when we launch a TSM aware app, the menu is not
									displayed. Fix in "utTryRemoveMenu".
		<32>	  4/9/92	KST		JH,modifying utNewTSMDocument to support the fact that SWM is no
									longer an application but a driver.
		<31>	 3/30/92	KST		JH, 1025008, 1025009. First attempt to solve the pallete/menu
									bar flickering problem. xDeactivateTSMDocument will not remove
									IM's pallete and menu. We now also change keyboard menu to
									synchronize the keyscript with the TSM document.
		<30>	 3/23/92	KST		Code changed according to the comments from the code review.
		<29>	 3/20/92	KST		JH, In "utNewTSMDocument", I should dereference the handle after
									the utRecordDocumentID call.
		<28>	 3/18/92	KST		Pass menu select call to text services only if the menu ID
									matches with the menu ID created by that text services.
		<27>	 3/17/92	KST		Allow creating TSM document in any scripts, not restricted to
									only CJK script.
		<26>	 3/16/92	KST		Removed DocumentID parameter from TSMEvent, TSMMenuSelect, and
									SetTSMCursor calls. It applies to the current active TSM
									document.
		<25>	 3/12/92	KST		We store a refcon in each document record. NewTSMDocument now
									takes one more parameter. In "xSendAEtoClient", we return that
									refcon in Apple event record.
		<24>	  3/6/92	KST		Added documentation for change #22. Also released memory for
									some objects in AESend.
		<23>	  3/2/92	KST		Added a private TSM call "NewTSMDocument4SWM". Also
									"utOpenDefaultIM" now takes a Boolean flag.
		<22>	 2/29/92	KST		In xActivateTSMDocument, if there is currently an active
									document which belongs to SWM, then send an AE to ask her to
									close the input window.
		<21>	 2/28/92	KST		FixTSMDocument and SetTSMCursor didn't pass the call to the
									current Input Method.
		<20>	 2/27/92	KST		Cannot call "DeleteMenu" when the TSM aware application is
									exiting to shell. Use our new call "utDeleteMenu".
		<19>	 2/26/92	KST		Took out debugger break.
		<18>	 2/22/92	KST		When sending event to input method Component, we set the system
									to system mode.
		<17>	 2/11/92	DCL		Changed the name of TSMEqu.[aph] to TextServices.[aph] for
									better read-ability.
		<16>	 2/10/92	KST		Moved all internal routines to TSMInternalUtil.c file.
		<15>	  2/1/92	KST		We don't have a global flag in TSM globals to indicate using
									bottom window. Change the byte to indicate the number of active
									text services for ease of debugging.
		<14>	 1/30/92	KST		Removed Debugger code.
		<13>	 1/29/92	KST		Open Input method only when the correct environment has set up.
									Now we open one input method for each document (not shared).
		<12>	 1/16/92	KST		Renamed error code.
		<11>	 1/13/92	KST		In "utOpenDefaultIM", if the default input method is already
									open, we return noErr. We also call Initiate and Activate
									TextService after successful open. Currently IM is never
									deactivated.
		<10>	 1/11/92	KST		Adding code to support old Japanese IM and use input window.
		 <9>	  1/9/92	KST		Adding code to support input method synchronization.
		 <8>	  1/8/92	KST		In NewTSMDocument, the default input method for the current
									script is automatically opened if there is one.
		 <7>	  1/4/92	KST		Add a new routine "CloseTextService". Also "OpenTextService" now
									returns component instance to the caller.
		 <6>	12/31/91	KST		We now record all open DocumentIDs so that it can be cleaned up
									upon process termination.
		 <5>	12/20/91	KST		Making progress toward Alpha. Changed "inline" to "TSM", and
									added new calls.
		 <4>	12/10/91	KST		Adding a new "InlineAwareRegister" call.
		 <3>	11/26/91	KST		Low level TSM routines are now dispatched directly to the
									Component Manager.
		 <2>	11/23/91	KST		Check in the first time for Cube-E.

*/


/*	********************************************************************************
 *	File:	"TSMFunctions.c"
 *
 *	Written  by Kenny SC. Tung
 *
 *	Modification History:
 *	18Nov91		KSCT	New today.
 *
 *	External routines:
 *		xNewTSMDocument
 *		xDeleteTSMDocument
 *		xDeactivateTSMDocument
 *		xActivateTSMDocument
 *		xTSMEvent
 *		xTSMMenuSelect
 *		xFixTSMDocument
 *		xSetTSMCursor
 *		xGetServiceList
 *		xOpenTextService
 *		xCloseTextService
 *		xSendAEtoClient
 *	******************************************************************************** */

#include	<Script.h>
#include	<MFPrivate.h>
#include	<TextServices.h>
#include	<TSMPrivate.h>
#ifndef __DEVICES__
	#include	<Devices.h>
#endif




/* ************************************ *
 *										*
 *	••  TSM routines start from here ••	*
 *										*
 * ************************************ */
 
/* High level TSM routines ... */



/*	**********************************************************************	*
 *	Function:	utNewTSMDocument											*
 *				Code shared by xNewTSMDocument4SWM and xNewTSMDocument.		*
 *				ActivateP is false when called by SWM and SWM only!!!		*
 *				Because SWM is not an app now, so we need to fake a PSN.	*
 *	Input:		activateP = false means called by SWM !						*
 *																			*
 *	Modification History:													*
 *	02Mar92	KST	New today. SWM creates the document in the background, if	*
 *				it uses xNewTSMDocument, then input method's pallete will	*
 *				be visible on the Desktop just after booting the system.	*
 *	06Mar92	KST	We now store an application's refcon in each document.		*
 *	26Mar92	KST	We now keep track of the script used by this document.		*
 *	07Apr92	KST	If the current script is not one of CJK, then need to set	*
 *				iDocUseThisScript to kNoSuchScript. Otherwise the keyscript	*
 *				will be changed to the wrong script when this document is	*
 *				activated. (this fix is basically for SWM).					*
 *	09Apr92	KST	If activateP is false, then we are called by SWM, fabricate	*
 *				a PSN using SWM's drive refnum.								*
 *	18May92	KST	I should check the supported interface type and the number	*
 *				of interfaces. (only 1 now)									*
 *	**********************************************************************	*/
OSErr
utNewTSMDocument(	short			numOfInterface, 
					OSType			supportedInterfaceTypes[],
					TSMDocumentID	*idocIDPtr,
					long			refcon,
					Boolean			activateP)
{
	Handle				aHandle;
	long				tempLong = sizeof(TSMDocRecord);
	short				i;
	OSErr				err;
	ScriptCode			currentScript;
	TSMDocRecord		*idocRecPtr;
	ProcessSerialNumber	PSNRecord;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();



	if (numOfInterface != 1)		return tsmUnsupportedTypeErr;			/* support only one type now */
	//	When we define a new type in the future, we need to calculate the handle size like this ...
	//	tempLong += sizeof(OSType) * (numOfInterface - 1);					/* record size is NOT  fixed */

	if (supportedInterfaceTypes[0] != kTextService 		&&					/* type has to be 'tsvc' */
		supportedInterfaceTypes[0] != TSMTEInterfaceType )					/* .......	   or 'tmTE' */
		return tsmUnsupportedTypeErr;

	aHandle = NewHandleClear(tempLong);
	if (aHandle == nil)				return memFullErr;						/* no memory */
	
	if ( activateP )														/* called by a TSM aware app */
		(void) GetCurrentProcess(&PSNRecord);								/* get current process's PSN */
	else {																	/* this is our dummied up SWM PSN */
		PSNRecord.highLongOfPSN = TSMVarsPtr->tsmSWMDrvrRefNum << 16;		/* so dummy one up */
		PSNRecord.lowLongOfPSN  = TSMVarsPtr->tsmSWMDrvrRefNum << 16;
	}

	
	err = utaRecordDocumentID(aHandle, &PSNRecord);		/* record the ID */
	if (err) {											/* app is not TSM aware */
		DisposeHandle(aHandle);							/* release the handle */
		*idocIDPtr = nil;
		return err;										/* and say goodbye */
	}
	
	/* initialize the TSM document record ... */
	currentScript = TSMVarsPtr->tsmCurrentSLRec.fScript;
	idocRecPtr = (TSMDocRecord *) *aHandle;				/* dereference the handle after the call	<#29> */
	idocRecPtr->iDocPSNRecord.highLongOfPSN = PSNRecord.highLongOfPSN;		/* owner of this TSM document */
	idocRecPtr->iDocPSNRecord.lowLongOfPSN  = PSNRecord.lowLongOfPSN;

	idocRecPtr->iDocSignature		= kTSMDocumentSignature;
	idocRecPtr->iDocOpenTextServiceN= 0;
	idocRecPtr->iDocCIHead			= nil;
	idocRecPtr->iDocActiveP			= false;
	idocRecPtr->iDocUseInputWindowP	= false;			/* local flag only */
	idocRecPtr->iDocRefcon			= refcon;			/* private storage for application		<#25> */
	idocRecPtr->iDocInterfaceN		= numOfInterface;


	if ( activateP )											/* called by a TSM aware application		<#37> */
			idocRecPtr->iDocUseThisScript	= currentScript;	/* inline-aware doc uses current script		<#37> */
	else	idocRecPtr->iDocUseThisScript	= kNoSuchScript;	/* SWM's doc has no initial script, neutral	<#37> */

	for (i=0; i<numOfInterface; i++)
		idocRecPtr->iDocOSTypeArray[i] = supportedInterfaceTypes[i];

	*idocIDPtr = aHandle;										/* return the handle as ID */
	if (utScriptIsCCJKAndNeedsIM(currentScript))				/* If the current script needs IM */
		err = utOpenActivateDefaultIM(*idocIDPtr, false);		/* open the default IM but don't activate it !! */
	else
		err = noErr;											/* no input method */

	return err;
};	/* utNewTSMDocument */



/*	**********************************************************************	*
 *	Function:	xNewTSMDocument4SWM											*
 *				SWM creates a new TSM aware Document.						*
 *	Input:		numOfInterface = number of supported interfaces,			*
 *				supportedInterfaceTypes	= array of OSTypes.					*
 *	Output:		If no error, return the handle as idocID.					*
 *				Error code: GetCurrentProcess,								*
 *				tsmNeverRegisteredErr = app is not TSM aware,				*
 *	Side effect:Allocate a handle from application heap.					*
 *				This document ID is also recorded in TSM's PSN table.		*
 *				The default input method for the current script is opened	*
 *				if there is one, BUT it is not activated.					*
 *																			*
 *	Modification History:													*
 *	02Mar92	KST	New today. SWM creates the document in the background, if	*
 *				it uses xNewTSMDocument, then input method's pallete will	*
 *				be visible on the Desktop just after booting the system.	*
 *	**********************************************************************	*/



pascal OSErr xNewTSMDocument4SWM(short numOfInterface, OSType supportedInterfaceTypes[], TSMDocumentID *idocID)
{
	/* Do not activate the input method, and refcon = 0 ... */
	return utNewTSMDocument(numOfInterface, supportedInterfaceTypes, idocID, 0, false);
}



/*	**********************************************************************	*
 *	Function:	xNewTSMDocument												*
 *				Application creates a new TSM aware Document.				*
 *	Input:		numOfInterface = number of supported interfaces,			*
 *				supportedInterfaceTypes	= array of OSTypes.					*
 *	Output:		If no error, return the handle as idocID.					*
 *				Error code: GetCurrentProcess,								*
 *				tsmNeverRegisteredErr = app is not TSM aware,				*
 *	Side effect:Allocate a handle from application heap.					*
 *				This document ID is also recorded in TSM's PSN table.		*
 *				The default input method for the current script is opened	*
 *				if there is one, and it is always activated.				*
 *																			*
 *	Modification History:													*
 *	10Jan92	KST	The default input method is not opened for each document.	*
 *				Instead, it is always open and saved in TSM globals.		*
 *	06Mar92	KST	We now have a new parameter -- refcon.						*
 *	**********************************************************************	*/

pascal OSErr xNewTSMDocument(short numOfInterface, OSType supportedInterfaceTypes[], TSMDocumentID *idocID, long refcon)
{
	/* Do activate the input method ... */
	return utNewTSMDocument(numOfInterface, supportedInterfaceTypes, idocID, refcon, true);
}



/*	***********************************************************************	*
 *	Function:	xDeleteTSMDocument											*
 *				Application wants to close the TSM aware Document.			*
 *				Release all memory used by this doc and text services.		*
 *	Input:		idocID = TSM Document ID									*
 *	Output:		noErr				= no error,								*
 *				tsmInvalidDocIDErr	= invalid document ID,					*
 *				tsmTSMDocBusyErr	= document is still active,				*
 *	Side effect: 1.	Will call "xDeactivateTSMDocument" for the document ID.	*
 *					All component instances are closed, and memory freed.	*
 *					The ID is also removed from TSM's PSN table.			*
 *				 2.	Since we don't want to remove IM's pallete/menu, but	*
 *					now we are deleting the document, what can we do to		*
 *					remember the info for this document? We create a copy	*
 *					of the document and give it a special signature. Keep	*
 *					this special doc. in the tsmPreviousDocID field.		*
 * The meaning of a shadow ID are:											*
 *	1. The real document ID has been deleted.								*
 *	2. This handle is from system heap. And is just a copy of the real ID.	*
 *	3. Instance has been closed, openedDefIMCInstance does not mean			*
 *	   anything even it is not nil !!!										*
 *	4. The size of the handle is always the sizeof(TSMDocRecord).			*
 *																			*
 * Modification history:													*
 *	30Mar92	KST	Before we dispose of the ID Handle, or before closing the	*
 *				input methods, to be precise. We need to check if the doc	*
 *				ID is saved in the TSMglobal's tsmPreviousDocID field. If	*
 *				it is, then we have to do something special. (make a copy)	*
 *	***********************************************************************	*/
pascal OSErr xDeleteTSMDocument(TSMDocumentID	idocID)
{	OSErr				err;
	short				i, count;
	CIListRec			*aListRecPtr;
	TSMDocRecord		*idocRecPtr;
	Handle				anewHandle;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	TSMAppsRecord		*appRecordPtr;


	err = utVerifyTSMDocID(idocID);
	if (err != noErr)										/* invalid id */
		return err;



	err =  xDeactivateTSMDocument(idocID);					/* deactivate the document, and ...  */
	/* this will set TSMVarsPtr->tsmPreviousDocID, because we		*
	 * don't want to hide the pallete when we delete the document.	*/


	if (TSMVarsPtr->tsmPreviousDocID == idocID) {			/* new 30Mar92 #31 */

		anewHandle = NewHandleSysClear(sizeof(TSMDocRecord));	/* from system heap */
		if (anewHandle != nil) {							/* has memory */
			BlockMove(*((Handle) idocID), *anewHandle, sizeof(TSMDocRecord));	/* copy the handle and change its signature */
			((TSMDocRecord *) *anewHandle)->iDocSignature = kTSMShadowDocumentSig;
		}
		TSMVarsPtr->tsmPreviousDocID = anewHandle;			/* replace it with a temp doc ID or nil */
	}

	/* .. close all opened input methods .... */
	(void) utCloseIM4Document(idocID, smJapanese,	false);		/* ignore error (it might not be open)  */
	(void) utCloseIM4Document(idocID, smTradChinese,false);
	(void) utCloseIM4Document(idocID, smSimpChinese,false);
	(void) utCloseIM4Document(idocID, smKorean,		false);


	HLock((Handle) idocID);
	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */

	aListRecPtr = idocRecPtr->iDocCIHead;
	for (i=0; i<idocRecPtr->iDocOpenTextServiceN; i++) {	/* close all opened components */
		if (aListRecPtr) {									/* just to be safe */
			if (aListRecPtr->ciServiceMenu	&&				/* if the text service has a menu */
				(*(aListRecPtr->ciServiceMenu))->menuID != -kTSMSystemMenuID) {			/* and this is not a system menu */
					if (aListRecPtr->ciMenuVisibleP) {	/* if it is visible, then remove it */
						utDeleteMenu((*(aListRecPtr->ciServiceMenu))->menuID);			/* call our delete menu routine */
						aListRecPtr->ciMenuVisibleP = false;							/* in case some stupid guy open the input method as a text service */
					}

				DisposeHandle((Handle) aListRecPtr->ciServiceMenu);						/* release the menu */
				aListRecPtr->ciServiceMenu = nil;			/* no menu now */
			}

			CloseComponent(aListRecPtr->ciTS);				/* ignore error */
			aListRecPtr = aListRecPtr->ciNextCIListRec;
		}
		else	break;
	}
	
	HUnlock((Handle) idocID);
	err = utaRemoveDocumentID((Handle) idocID, &count, &appRecordPtr);	/* remove the ID from TSM internal table */
	DisposeHandle((Handle) idocID);							/* release the handle */


	if (err == noErr && count == 0) {						/* alert: the TSM aware application has no more document */
		if (!appRecordPtr->psnTSMCalledUseIWP) {			/* if this is a re-entrant call, don't do it again <#52> */
			appRecordPtr->psnOldUseIWGlobalState = appRecordPtr->psnAppUseInputWindowP;		/* save the previous state   */
			err = xUseInputWindow(nil, true);				/* turn use input window ON */
			if (err == noErr) {
				appRecordPtr->psnTSMCalledUseIWP = true;	/* mark TSM did this */
			}
			else {
				appRecordPtr->psnTSMCalledUseIWP = false;
				appRecordPtr->psnAppUseInputWindowP = appRecordPtr->psnOldUseIWGlobalState;	/* restore state */
			}
		}
	}

	return err;
};


/*	**************************************************************************	*
 *	Function:	xDeactivateTSMDocument											*
 *				Deactivate an TSM aware Document.								*
 *	Input:		idocID = TSM Document ID										*
 *	Output:		noErr If no error,												*
 *				tsmInvalidDocIDErr = invalid id,								*
 *				tsmDocNotActiveErr = the document is not activated,				*
 *	Side effect:Set active falg to false.										*
 *				Deactivate all opened Text Services.							*
 *				Deactivate the Input Method of the current script.				*
 *				After the call, tsmUseInputWindowDocID = nil,					*
 *								tsmActiveDocID = nil.							*
 *																				*
 * Modification history:														*
 *	24Mar92	KST	Need to call utSoftDeactivateIM4Document, not utDeactivateIM4Document	*
 *				UtSoftDeactivateIM4Document will not remove IM's pallete and menu.		*
 *	28Mar92	KST	For SWM, once we deactivate the document, the iDocUseThisScript			*
 *				field should be nullified 'cause SWM's doc is shared by 4 CCJK			*
 *				scripts. This makes the whole thing so completed.						*
 *	05Jun92	KST	If tsmActiveDocID == tsmUseInputWindowDocID, then deactivate the ID.	*
 *				If tsmActiveDocID != tsmUseInputWindowDocID, then tsmActiveDocID has	*
 *				to be SWM's doc. I'll deactivate both two documents !					*
 *	**********************************************************************************	*/
pascal OSErr xDeactivateTSMDocument(TSMDocumentID idocID)
{	OSErr					err, appUseIWError;
	short					i;
	ScriptCode				currentScript;
	CIListRec				*aListRecPtr;
	TSMDocRecord			*idocRecPtr;
	ComponentResult			cmResult;
	ProcessSerialNumber		PSNRecord;
	Boolean					deactivateSWMDocP;
	TSMDocumentID			currentTSMActiveDocID;
	DefIMInstanceRecord		*openedDefIMRecordPtr;
	TSMVarsRec				*TSMVarsPtr = GetExpandMemTSMGlobals();


	err = utVerifyTSMDocID(idocID);
	if (err != noErr)											/* invalid id */
		return err;

	currentScript = TSMVarsPtr->tsmCurrentSLRec.fScript;

	if (! ((TSMDocRecordPtr) *(Handle) idocID)->iDocActiveP ) {	/* if doc is not active  */
		/* If user just create and then delete the document, it is not active but
		 * there could be an open input method .................   <26Jun92 #48> */
		HLock((Handle) idocID);									/* lock the handle */
		idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
		for (i=0; i<kSavedDefaultIMCount; i++) {
			openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[i]);

			if (openedDefIMRecordPtr->openedDefIMCInstance	&&	/* if there is one open */
				openedDefIMRecordPtr->openedDefIMActiveP) {		/* and the input method is active */
					err = DeactivateTextService(openedDefIMRecordPtr->openedDefIMCInstance);	/* what the app can do on error ??? */
					if (err == noErr)
						openedDefIMRecordPtr->openedDefIMActiveP = false;	/* not active anymore */
			}
		}
		HUnlock((Handle) idocID);								/* unlock the handle */
		return tsmDocNotActiveErr;								/* stop right now */
	}

	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);			/* dereference the handle */
	if (idocRecPtr->iDocPSNRecord == TSMVarsPtr->tsmSWMsPSNRecord)	/* doc belongs to SWM */
			deactivateSWMDocP = true;
	else	deactivateSWMDocP = false;
	

	(void) GetCurrentProcess(&PSNRecord);						/* get current process's PSN */
	appUseIWError = utaIsAppTSMAwareAndNotUsingIW(&PSNRecord);	/* for TSM aware app */
	
	/* don't fix text 'cause it might send to another application ..... */
	//if (appUseIWError == tsmUseInputWindowErr)				/* use input window */
	//	(void) xFixTSMDocument( TSMVarsPtr->tsmActiveDocID );	/* then fix its input area ...	<14Jun93 #42> */

	/* deactivate the default input method of the current keyscript		-- <27Jan92> */
	if ( utDocNeedsInputMethod(currentScript, &idocRecPtr->iDocPSNRecord) ) {
	//	if ( deactivateSWMDocP )								/* if this is SWM */
	//		(void) xFixTSMDocument( idocID );					/* then fix its input area ...	<14Jun93 #42> */

		(void) utSoftDeactivateIM4Document(idocID, currentScript );
		TSMVarsPtr->tsmActiveTSCount--;							/* one less	active TS	--		<01Feb92 #15> */
	}
	else
		if (appUseIWError == tsmUseInputWindowErr ) {			/* use input window, just deactivate the IM */
			/* Don't hide the pallete/menu, just save the document ID we are deactivating */
			(void) utJustDeactivateIM4Document(idocID, currentScript, false );		/* <#48> */
			
			TSMVarsPtr->tsmPreviousDocID  = idocID;				/* save this ID		<#13> */
			TSMVarsPtr->tsmPreviousScript = currentScript;		/* save this script <#13> */
		}


	err = noErr;
	HLock((Handle) idocID);										/* lock the handle */
	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);			/* dereference the handle */
	aListRecPtr = idocRecPtr->iDocCIHead;
	for (i=0; i<idocRecPtr->iDocOpenTextServiceN; i++) {		/* call each open text service */
		if (aListRecPtr) {										/* just to be safe */
			cmResult = DeactivateTextService(aListRecPtr->ciTS);	/* what the app can do on error ??? */
			if (cmResult) {
				err = cmResult;									/* change the error to short */
				break;
			}
			
			TSMVarsPtr->tsmActiveTSCount--;						/* one less	active TS	-- <01Feb92 #15> */
			/* if the text service has a menu, remove it from menu bar */
			if (aListRecPtr->ciServiceMenu) {					/* if the text service has a menu */
				if (aListRecPtr->ciMenuVisibleP) {				/* and is visible */
					if ((*(aListRecPtr->ciServiceMenu))->menuID != -kTSMSystemMenuID) {	/* if this is not a system menu */
						utDeleteMenu((*(aListRecPtr->ciServiceMenu))->menuID);			/* call our delete menu routine */
						aListRecPtr->ciMenuVisibleP = false;							/* in case some stupid guy open the input method as a text service */
					}
				}
			}
			aListRecPtr = aListRecPtr->ciNextCIListRec;
		}
	}
	
	if (err == noErr) {
		idocRecPtr->iDocActiveP = false;
		
		currentTSMActiveDocID = TSMVarsPtr->tsmUseInputWindowDocID;
		if ( currentTSMActiveDocID == idocID )					/* app will deactivate this one first */
			TSMVarsPtr->tsmUseInputWindowDocID = nil;


#ifdef BuildTSMInit
		else
			if ( currentTSMActiveDocID )			Debugger();
#endif

		currentTSMActiveDocID = TSMVarsPtr->tsmActiveDocID;		/* then look at this current document */
		if ( currentTSMActiveDocID ) {
			if (currentTSMActiveDocID != idocID) {				/* clear the global ...		*/
				if ( ((TSMDocRecordPtr) *(Handle) currentTSMActiveDocID)->iDocPSNRecord == TSMVarsPtr->tsmSWMsPSNRecord) {	/* current doc belongs to SWM */
					/* this is because the doc is using input window, so we have 2 active documents */
					err = utJustDeactivateIM4Document(currentTSMActiveDocID, TSMVarsPtr->tsmCurrentSLRec.fScript, false );
					if (err == noErr) {
						utCloseSWMWindow(false);				/* ask SWM to close her input window */
						((TSMDocRecord *) *((Handle) currentTSMActiveDocID))->iDocUseThisScript = kNoSuchScript;
					}
				}

#ifdef BuildTSMInit
				else		Debugger();
#endif

			}
			TSMVarsPtr->tsmActiveDocID = nil;					/* no current document now	*/
			TSMVarsPtr->tsmActiveTSCount = 0;
		}
	}
	HUnlock((Handle) idocID);
	return err;
};


/*	***********************************************************	*
 *	Because there is no way to remove the menu after the TSM	*
 *	aware app has been swapped out, we need to remove all other	*
 *	IM's menu before we put out ours.							*
 *	***********************************************************	*/
void utTryRemoveMenu( TSMVarsRec	*TSMVarsPtr )
{
	short				pos;
	short				currentIndex;
	ComponentMenuRec	*aCMRecordPtr;
	
	currentIndex  = utGetScriptIndex(TSMVarsPtr->tsmCurrentSLRec.fScript);
	
	for (pos=0; pos<kSavedDefaultIMCount; pos++) {
		if (pos != currentIndex) {									/* leave alone the current one */
			aCMRecordPtr = &(TSMVarsPtr->tsmDefIMInfoTable[pos]);	/* #33 */
			if (aCMRecordPtr->gMenuHandle)							/* if it has a menu */
				utDeleteMenu((*aCMRecordPtr->gMenuHandle)->menuID);
		}
	}
}


/*	**************************************************************************	*
 *	Function:	xActivateTSMDocument											*
 *				Activate an TSM aware Document.									*
 *	Input:		idocID = TSM Document ID										*
 *	Output:		noErr If no error,												*
 *				tsmInvalidDocIDErr = invalid id,								*
 *	Side effect:Set active falg to true,										*
 *				deactivate any currently active TSM aware document,				*
 *				activate all opened Text Services too.							*
 *				Update the 'UseInputWindow' flag in the PSN table.	 			*
 *																				*
 *	Modification History:														*
 *	04Mar92	KST	If there is currently an active document which belongs to SWM,	*
 *				then send an AE to ask her to close the input window if it is	*
 *				visible. This happens when user switches from non-TSM aware		*
 *				application to a TSM document.									*
 *	27Mar92	KST	We now change keyboard menu to synchronize the keyscript		*
 *				with the TSM document.											*
 *	09Apr92	KST	Because there is no way to remove the menu after the TSM		*
 *				aware app has been swapped out, we need to remove all other		*
 *				IM's menu before we put out ours.								*
 *	07Jun92	KST	To make UseInputWindow to work for each document, we have to	*
 *				maintain a parallel active documentID -- tsmUseInputWindowDoc.	*
 *	08Jun92	KST	If the app is using input window, then we need to delay SWM's	*
 *				ActivateTSMDocument call until the app activates its document.	*
 *	10Jul92	KST	Do not assume when a TSM aware application calls				*
 *				ActivateTSMDocument(), the other document has been deactivated.	*
 *	**************************************************************************	*/
pascal OSErr xActivateTSMDocument(TSMDocumentID idocID)
{	OSErr				err;
	short				i;
	Boolean				swmsDocIDp = false,					/* true if the current active doc belongs to SWM */
						activatingSWMsDocP = false,			/* true if we are activating SWM's document */
						useInputWindowCalledByTSMP = false,	/* true if use InputWindow is called by TSM 'cause app has no document */
						useInputWindowSpecialCase  = false;
	CIListRec			*aListRecPtr;
	TSMDocRecord		*idocRecPtr;
	ComponentResult		cmResult;
	ProcessSerialNumber	PSNRecord;
	TSMDocumentID		theCurrentActiveDocID, theUseInputWindowDocID;			/* <#53> */
	ScriptCode			theCurrentScript;
	TSMAppsRecord		*appRecordPtr;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();


	err = utVerifyTSMDocID(idocID);
	if (err != noErr)										/* invalid id */
		return err;

	if ( ((TSMDocRecordPtr) *(Handle) idocID)->iDocActiveP) {	/* if we are currently active already ... */
		return noErr;										/* if you want to call it, I'll make you happy */
	}


	/* For a TSM aware application, usually tsmUseInputWindowDocID = tsmActiveDocID = current active document,
	 * but if this app calls UseInputWindow, then start typing, SWM will activate her document.
	 * In this case, I will:
	 *		1. keep tsmUseInputWindowDocID but change tsmActiveDocID = SWM's document, 
	 *		2. deactivate the input method used by tsmUseInputWindowDocID, but keep TSs active!
	 *		3. activate SWM's document.
	 */

	
	theCurrentActiveDocID	= TSMVarsPtr->tsmActiveDocID;			/* optimize it */
	theUseInputWindowDocID	= TSMVarsPtr->tsmUseInputWindowDocID;	/* too   <#53> */
	theCurrentScript		= TSMVarsPtr->tsmCurrentSLRec.fScript;

	if ( ((TSMDocRecordPtr) *(Handle) idocID)->iDocPSNRecord == TSMVarsPtr->tsmSWMsPSNRecord )
		activatingSWMsDocP = true;							/* true if we are activating SWM's document */

	(void) GetCurrentProcess(&PSNRecord);					/* get current process's PSN */
	(void) utaGetTSMAppRecordPtr(&PSNRecord, &appRecordPtr);/* get the app record */

	err = utaIsAppTSMAwareAndNotUsingIW(&PSNRecord);		/* 2 calls should be combined */
	//	if (err == tsmNeverRegisteredErr)					/* if the current application is not TSM aware */
	//		return err;										/* this is OK, SWM is activating her doc */

	if (err == tsmUseInputWindowErr) {						/* if the current application is using Input Window */
		if ( activatingSWMsDocP ) {							/* if we are activating SWM's document */
			/*	If we are activating SWM's document and the document/app is using Input Window, then		*
			 *	we need to do this special check. Since SWM calls ActivateTSMDocument at every				*
			 *	'doAccRun' time if the current app is using Input Window. We have to delay activating		*
			 *	SWM's document if the TSM aware app has not activated its own document. If we don't			*
			 *	delay SWM's call, then there will be 2 problems:											*
			 *	1.	We'll activate SWM document twice. If the two documents are using the same script,		*
			 *		then this is only a minor inefficiency for the first time is unnecessary.				*
			 *	2.	If the two documents are not using the same script, and the two documents are both		*
			 *		using Input Window, then the first time will mess up the state information kept by		*
			 *		TSM. ie., If you switch from Akiko to KIM, Akiko's pallete will stay on the screen.		*/

			if (theCurrentActiveDocID == nil) {				/* should yield to the app */
				/* We should yield to the TSM aware app, but now we want to solve the NO TSM docuemnt bug	*
				 * by always turning global UseInputWindow on. Soooooo, now we have to allow SWM activates	*
				 * its document in this special case. Oh, I am sick and tired of this project !! <17Jun92>	*/
				
				
				if ( !appRecordPtr->psnTSMCalledUseIWP )	/* TSM didn't call use input window */
						return noErr;						/* I hope the explanation above is clear enough */
															/* if not, don't feel bad. I don't either ..... */
				useInputWindowCalledByTSMP = true;			/* now we know this is because TSM forced us .. */
			}
			
			if (theUseInputWindowDocID		 &&				/* this means the current app is TSM aware */
				theCurrentActiveDocID == theUseInputWindowDocID ) {
					useInputWindowSpecialCase = true;		/* TSM aware app uses SWM's document, app's document is already activated */
			}
		}
	}

	if (theCurrentActiveDocID != nil) {						/* another document is currently active ? */
		if (useInputWindowSpecialCase) {					/* do not deactivate the current document (it's using input window) */
			/*	Application activated its document, now tsmActiveDocID == tsmUseInputWindowDoc, if the app or	*
			 *	the document is using input window, then SWM will activate her document at 'doAccRun' time,		*
			 *	in this case, we don't want to deactivate the tsmActiveDocID, instead, we keep 2 active IDs.	*
			 *	When the application calls xUseInputWindow for one document, I know TSM already called			*
			 *	"utJustDeactivateIM4Document". But if the application is using Input Window globally, then the	*
			 *	document's Input Method has not been deactivated. So we have to call it here again.				*
			 *	utJustDeactivateIM4Document can be called more than once for the same document.					*/

			// The current active document is not changed after this call but its current IM is deactivated ....
			(void) utJustDeactivateIM4Document(theCurrentActiveDocID, theCurrentScript, true);	/* so deactivate the input method but keep active */
		}
		else {
			/* deactivate any currently active TSM aware document, there can be only one. */
	
			if ( ((TSMDocRecordPtr) *(Handle) theCurrentActiveDocID)->iDocPSNRecord == TSMVarsPtr->tsmSWMsPSNRecord)	/* doc belongs to SWM	<#22> */
					swmsDocIDp = true;						/* true if the doc belongs to SWM */
			else	swmsDocIDp = false;

			/* can't fix text ... */
			(void) xDeactivateTSMDocument( theCurrentActiveDocID );		 	 /* ignore error */
			if (swmsDocIDp)									/* doc belongs to SWM	<#22> */
				utCloseSWMWindow(false);					/* ask SWM to close her input window	<#22> */

			if ( theUseInputWindowDocID		&&								/* <#53> */
				 theCurrentActiveDocID != theUseInputWindowDocID ) {		/* <#53> */
				(void) xDeactivateTSMDocument( theUseInputWindowDocID );	/* <#53> */
				TSMVarsPtr->tsmUseInputWindowDocID = nil;					/* <#53> */
			}																/* <#53> */
		}	/* no current active document now */
		TSMVarsPtr->tsmActiveDocID	 = nil;					/* no active document now */
	}
	

	HLock((Handle) idocID);									/* lock the handle */
	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */

	/* Now, We want to change keyboard menu to synchronize the keyscript with this TSM document */
	if (idocRecPtr->iDocUseThisScript != kNoSuchScript &&
		idocRecPtr->iDocUseThisScript != theCurrentScript ) {

		/* We have to be careful here because KeyScript will end up calling SetTextServiceLanguage.	*
		 * Are we reentrant? SetTextServiceLanguage should never call xActivateTSMDocument, I hope!	*
		 */
		theCurrentScript = idocRecPtr->iDocUseThisScript;	/* current script will be changed */
		KeyScript(theCurrentScript);						/* change it to this document's keyscript */
	}

	err = noErr;
	if ( !useInputWindowSpecialCase) {						/* SWM will never open text services */
		/* If there is a menu belongs to other IM, then remove it. */
		// utTryRemoveMenu(TSMVarsPtr);

		/* activate all the text services associated with the document */
		aListRecPtr = idocRecPtr->iDocCIHead;
		for (i=0; i<idocRecPtr->iDocOpenTextServiceN; i++) {	/* call each open text service */
			if (aListRecPtr) {									/* just to be safe */
				cmResult = ActivateTextService(aListRecPtr->ciTS);	/* what the app can do on error ??? */
				if (cmResult) {									/* some error */
					err = cmResult;								/* change the error to short */
					break;
				}
				
				TSMVarsPtr->tsmActiveTSCount++;					/* bump the counter		-- <01Feb92 #15> */
				if (aListRecPtr->ciServiceMenu) {				/* if it has a menu */
					InsertMenu(aListRecPtr->ciServiceMenu, 0);
					DrawMenuBar();
					aListRecPtr->ciMenuVisibleP = true;
				}
				aListRecPtr = aListRecPtr->ciNextCIListRec;
			}
		}
	}
	
	if (!useInputWindowCalledByTSMP) {						/* if this is not a forced activate call by TSM */
		if (appRecordPtr->psnTSMCalledUseIWP) {				/* then restore the whole bloody mess */
			appRecordPtr->psnTSMCalledUseIWP = false;
			appRecordPtr->psnAppUseInputWindowP = appRecordPtr->psnOldUseIWGlobalState;	/* restore state */
		}
	}
	
	if (err == noErr) {
		idocRecPtr->iDocActiveP		= true;					/* document is active */
		TSMVarsPtr->tsmActiveDocID	= idocID;				/* save the current active doc id */
		if ( !useInputWindowSpecialCase )					/* doc does not belongs to SWM */
			TSMVarsPtr->tsmUseInputWindowDocID = idocID;	/* then keep it as parallel ID */

		utaUpdateDocFlagInPSNTable(idocID);					/* update the PSN table use input window flag for the document */
		
		if (utDocNeedsInputMethod(theCurrentScript, &idocRecPtr->iDocPSNRecord)) {
			err = utActivateThisDocsInputMethod(idocID);
			if (err == noErr)
				TSMVarsPtr->tsmActiveTSCount++;				/* bump the counter		-- <01Feb92 #15> */
		}
		else {
			/*	We switch from doc1 (Japanese) to doc2 (Roman), here we have changed the current script to	*
			 *	Roman, so we are not going to activate any IM dor doc2. But now Akiko's pallete is still	*
			 *	hanging around because the previous doc has not been cleaned up !!			-- <#37>		*/
			if (TSMVarsPtr->tsmPreviousDocID)
				utCleanUpPreviousDoc(TSMVarsPtr->tsmPreviousDocID, TSMVarsPtr->tsmCurrentSLRec.fScript);
		}
	}
	HUnlock((Handle) idocID);
	return err;
};	/* end xActivateTSMDocument */


/*	**************************************************************************	*
 *	Function:	utTSMEvent2InputMethod -- Send events to input method.			*
 *	Input:		Pointer to a EventRecord and documentation id.					*
 *	Output:		Return false if input method doesn't process the event.			*
 *	Side effect:None.															*
 *	Modification History:														*
 *	03Jun92	KST	New today, need this call to solve the UseInputWindow problem.	*
 *	**************************************************************************	*/
Boolean utTSMEvent2InputMethod(	EventRecord		*eventPtr,
								ScriptCode		currentScript,
								TSMDocumentID	idocID)				/* ID already verified */
{
	TSMDocRecord			*idocRecPtr;
	TSMVarsRec				*TSMVarsPtr = GetExpandMemTSMGlobals();
	DefIMInstanceRecord		*openedDefIMRecordPtr;
	ComponentResult			cmResult = 0;


	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */

	if (idocRecPtr->iDocActiveP == false)					/* if not active  */
		return false;										/* stop right now */

	/* Now first pass the event to the default input method of the current key script if we're using one */
	/* Check if the document is using input window or old JIM, if it is, don't pass the event on */			

	openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(currentScript)]);
	if (openedDefIMRecordPtr->openedDefIMCInstance) {		/* has open input method */

		// ???????
		if (!openedDefIMRecordPtr->openedDefIMActiveP)		/* if the input method is NOT active */
			return false;									/* stop right now */


		TSMVarsPtr->tsmPatchFlags |= kInTSMEventCall;		/* say we are inside of the TextServiceEvent call	<#40> */
		(void) BeginSystemMode();							/* input method is treated as system service		<#18> */

		cmResult = TextServiceEvent(openedDefIMRecordPtr->openedDefIMCInstance, 1, eventPtr);

		(void) EndSystemMode();								/* close the window									<#18> */
		TSMVarsPtr->tsmPatchFlags &= ~kInTSMEventCall;		/* say we are out of the TextServiceEvent call		<#40> */

		if (cmResult)										/* input method handled the call */
			return true;									/* so break now ... */
	}
	return false;											/* input method does NOT handled the call */
}



/*	**************************************************************************	*
 *	Function:	TSMEventFromSWM -- private call for SWM only.					*
 *				SWM passes all events to TSM.									*
 *				(This is actually the original TSMEvent code).					*
 *	Input:		Pointer to a EventRecord and documentation id.					*
 *	Output:		Return false if input method doesn't process the event.			*
 *	Side effect:None.															*
 *	Note:		SWM only handles input, it does not deal with text services.	*
 *	Modification History:														*
 *	03Jun92	KST	New today, need this call to solve the UseInputWindow problem.	*
 *	**************************************************************************	*/
pascal Boolean xTSMEventFromSWM(EventRecord *eventPtr)		// pass all events to TSM
{
	TSMVarsRec		*TSMVarsPtr = GetExpandMemTSMGlobals();
	ScriptCode		currentScript = TSMVarsPtr->tsmCurrentSLRec.fScript;
	TSMDocumentID	idocID;

	
	idocID = TSMVarsPtr->tsmActiveDocID;					/* use current TSM document ID as default	<#26> */
	if (utDocNeedsInputMethod(currentScript, &TSMVarsPtr->tsmSWMsPSNRecord)) {
		if (utVerifyTSMDocID(idocID) != noErr)				/* invalid id */
			return false;									/* nobody handled the event */
		return	utTSMEvent2InputMethod( eventPtr, currentScript, idocID );	/* use common code -- for input method */
	}
	return false ;					/* if does not need input method, return false */

};



/*	******************************************************************	*
 *	Function:	xTSMEvent												*
 *				Application passes all events to TSM.					*
 *				We will call all the open text services until one 		*
 *				handles it or no more TS to call.						*
 *																		*
 *				Special case for Input Method: only one IM will be		*
 *				called which matches the current keyboard script.		*
 *	Input:		Pointer to a EventRecord and documentation id.			*
 *	Output:		Return false if text service doesn't process the event.	*
 *	Side effect:None.													*
 *																		*
 *	Modification History:												*
 *	10Jan92	KST	If the system is using old Japanese input method, or	*
 *				if the document is using input window, then don't pass	*
 *				the event to text services.								*
 *	10Jan92	KST	The default input method is not opened for each.		*
 *				document, it is always open and saved in TSM globals.	*
 *	16Mar92	KST	Removed DocumentID parameter. It applies to the current	*
 *				active TSM document. Will return false if there is no	*
 *				TSM document, or the document is not active.	 <#26>	*
 *	03Jun92	KST	Changed this code to be called only by TSM aware apps	*
 *				so that we can handle if the doc/app is using the input	*
 *				window. SWM will no longer use this call.				*
 *	******************************************************************	*/
pascal Boolean xTSMEvent(EventRecord *eventPtr)				/* pass all events to TSM by application */
{															/* SWM shall never call this routine !!! */
	TSMVarsRec			*TSMVarsPtr		= GetExpandMemTSMGlobals();
	ScriptCode			currentScript	= TSMVarsPtr->tsmCurrentSLRec.fScript;
	Boolean				result;
	TSMDocumentID		idocID;
	OSErr				err;
	short				numOfEvents = 1;					/* always sending 1 event */
	CIListRec			*aListRecPtr;
	ComponentResult		cmResult = 0;
	TSMDocRecord		*idocRecPtr;
	ProcessSerialNumber	PSNRecord;
	

	// This flag is set in SWM's _ModalDialogPatch, and we reset this flag when TSMEvent is called.
	TSMVarsPtr->tsmPatchFlags &= ~kTSMEventBeenCalled;		/* <#54> */

	/* For a TSM aware application, usually tsmUseInputWindowDocID = tsmActiveDocID = current active document,
	 * but if this app calls UseInputWindow, then tsmUseInputWindowDocID = current active document, and
	 * tsmActiveDocID = SWM's document. When app opens a text service, the text service is recorded with
	 * the app's document, and therefore when event is sent to text services, we should use tsmUseInputWindowDocID.
	 */
	idocID = TSMVarsPtr->tsmUseInputWindowDocID;			/* use tsmUseInputWindowDocID document ID as default	<#40> */
	// I know if the app is using input window, then the IM with tsmUseInputWindowDocID is deactivated.
	// But this is all right becuase in this case, we will not send event to IM any way ....

	if (utVerifyTSMDocID(idocID) != noErr)					/* invalid id */
		return false;										/* nobody handled the event */
	
	err = GetCurrentProcess(&PSNRecord);
	if (err)			return false;						/* what is this error? */
	
	/*	now is the tricky part, return false if:
		1. the current application is not TSM aware, or
		2. the app is TSM aware, but is using input window, or
		3. the app is TSM aware, but is using old JIM.	*/
	if (utDocNeedsInputMethod(currentScript, &PSNRecord)) {	/* if app needs input method */
		result = utTSMEvent2InputMethod( eventPtr, currentScript, idocID );	/* use common code -- for input method */
		if (result)											/* event handled by IM */
			return true;									/* so reutnr now */
	}
	
	/* If input method didn't handle the event, pass the event to other open text services ... */
	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
	if (idocRecPtr->iDocOpenTextServiceN == 0)				/* if no open text services */
		return false;										/* stop right now */

	HLock((Handle) idocID);									/* lock the handle */
	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
	aListRecPtr = idocRecPtr->iDocCIHead;
	
	while (aListRecPtr) {									/* call each open TS */
		TSMVarsPtr->tsmPatchFlags |= kInTSMEventCall;		/* say we are inside of the TextServiceEvent call	<#40> */
		cmResult = TextServiceEvent(aListRecPtr->ciTS, numOfEvents, eventPtr);
		TSMVarsPtr->tsmPatchFlags &= ~kInTSMEventCall;		/* say we are out of the TextServiceEvent call		<#40> */

		if (cmResult)										/* text service handled the call */
			break;											/* so break now ... */
		aListRecPtr = aListRecPtr->ciNextCIListRec;
	}
	
	HUnlock((Handle) idocID);
	if (cmResult)											/* if text service handled the event */
			return true;									/* and return true */
	else	return false;									/* didn't handle the event */
};




/*	******************************************************************	*
 *	Function:	xTSMMenuSelect											*
 *				User chooses a menu item, pass it to text service.		*
 *				We will call all the open text services until one 		*
 *				handles it or no more TS to call.						*
 *	Input:		idocID													*
 *				menuResult												*
 *	Output:		false if text service doesn't handle the menu selection	*
 *				true  if text service handles the menu selection.		*
 *																		*
 *	Side effect:None.													*
 *																		*
 *	Modification History:												*
 *	10Jan92	KST	If the system is using old Japanese input method, or	*
 *				if the document is using input window, then don't pass	*
 *				the event to text services.								*
 *	16Mar92	KST	Removed DocumentID parameter. It applies to the current	*
 *				active TSM document. Will return false if there is no	*
 *				TSM document, or the document is not active.	 <#26>	*
 *	******************************************************************	*/
pascal Boolean xTSMMenuSelect(long	menuResult)	// pass menu selection to TSM
{
	short				menuItem, menuID, i;
	CIListRec			*aListRecPtr;
	ComponentResult		cmResult = 0;
	TSMDocRecord		*idocRecPtr;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	TSMDocumentID		idocID;


	idocID = TSMVarsPtr->tsmActiveDocID;					/* use current TSM document ID as default	<#26> */
	if (utVerifyTSMDocID(idocID) != noErr)					/* invalid id */
		return false;

	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
	if (idocRecPtr->iDocActiveP == false)					/* if not active  */
		return false;										/* stop right now */

	menuID	 = menuResult >> 16;
	menuItem = (short) menuResult & 0xFFFF;

	/* Now first pass the event to the default input method of the current key script */
	/* Check if the document is using input window or old JIM, if it is, don't pass the event on */			

	if (utDocNeedsInputMethod(TSMVarsPtr->tsmCurrentSLRec.fScript, &idocRecPtr->iDocPSNRecord)) {
		/* Note: "utDocNeedsInputMethod" does not move memory */
		openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(TSMVarsPtr->tsmCurrentSLRec.fScript)]);

		if (openedDefIMRecordPtr->openedDefIMCInstance) {	/* double check if it is open */

#ifdef BuildTSMInit
			if (!openedDefIMRecordPtr->openedDefIMActiveP)	/* if the input method is NOT active */
				Debugger();									/* let me know */
#endif

			if (menuID == -kTSMSystemMenuID ) {				/* do I need to check this ??? */
				(void) BeginSystemMode();					/* input method is treated as system service	<#18> */
				cmResult = TextServiceMenuSelect(openedDefIMRecordPtr->openedDefIMCInstance, openedDefIMRecordPtr->openedDefIMServiceMenuHdl, menuItem);
				(void) EndSystemMode();						/* close the window really quick				<#18> */
				if (cmResult)								/* input method handled the selection */
					return true;							/* so break now ... */
			}
		}
	}


	HLock((Handle) idocID);									/* lock the handle */
	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
	aListRecPtr = idocRecPtr->iDocCIHead;

	for (i=0; i<idocRecPtr->iDocOpenTextServiceN; i++) {	/* call each open text service */
		if (aListRecPtr && aListRecPtr->ciServiceMenu) {	/* call if it has a menu */
			if ((*(aListRecPtr->ciServiceMenu))->menuID == menuID) {	/* for the right menu */
				cmResult = TextServiceMenuSelect(aListRecPtr->ciTS, aListRecPtr->ciServiceMenu, menuItem);
				if (cmResult)								/* text service handled the call */
					break;									/* so break now ... */
			}
			aListRecPtr = aListRecPtr->ciNextCIListRec;
		}
	}
	
	HUnlock((Handle) idocID);
	if (cmResult)											/* if text service handled the event */
			return true;									/* and return true */
	else	return false;									/* didn't handle the event */
};


/*	******************************************************************	*
 *	Function:	xFixTSMDocument											*
 *				Confirm the current active input area.					*
 *	Input:		idocID													*
 *	Output:		noErr If no error,										*
 *				tsmInvalidDocIDErr = invalid doc id,					*
 *				tsmDocNotActiveErr = the document is not activated,		*
 *				tsmNoOpenTSErr	   = no open text service.				*
 *	Side effect:None.													*
 *																		*
 *	******************************************************************	*/
pascal OSErr xFixTSMDocument(TSMDocumentID idocID)
{	OSErr				err;
	TSMDocRecord		*idocRecPtr;
	ComponentResult		cmResult = 0;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	DefIMInstanceRecord	*openedDefIMRecordPtr;

	err = utVerifyTSMDocID(idocID);
	if (err != noErr)										/* invalid id */
		return err;


	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
	if (!idocRecPtr->iDocActiveP)							/* if not active, can not continue */
		return tsmDocNotActiveErr;

	/* Now first pass the event to the default input method of the current key script if we're using one */
	/* Check if the document is using input window or old JIM, if it is, don't pass the event on */			

	if (utDocNeedsInputMethod(TSMVarsPtr->tsmCurrentSLRec.fScript, &idocRecPtr->iDocPSNRecord)) {
		/* Note: "utDocNeedsInputMethod" does not move memory */
		openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(TSMVarsPtr->tsmCurrentSLRec.fScript)]);
		if (openedDefIMRecordPtr->openedDefIMCInstance) {	/* already open */
		
			if (!openedDefIMRecordPtr->openedDefIMActiveP)	/* if the input method is NOT active */
				return tsmTSNotOpenErr;						/* let me know */

			(void) BeginSystemMode();						/* input method is treated as system service	<#18> */
			cmResult = FixTextService(openedDefIMRecordPtr->openedDefIMCInstance);
			(void) EndSystemMode();							/* close the window really quick				<#18> */
		}
		else	cmResult = tsmNoOpenTSErr;
	}

	return ((OSErr) cmResult);
};




/*	******************************************************************	*
 *	Function:	xSetTSMCursor											*
 *				Give each text service a chance to set its cursor.		*
 *				We will call all the open text services until one 		*
 *				handles it or no more TS to call.						*
 *	Input:		idocID													*
 *				mousePos												*
 *	Output:		false if text service doesn't set the cursor,			*
 *				true  if text service set the cursor.					*
 *																		*
 *	Side effect:None.													*
 *																		*
 *	Modification History:												*
 *	16Mar92	KST	Removed DocumentID parameter. It applies to the current	*
 *				active TSM document. Will return false if there is no	*
 *				TSM document, or the document is not active.	 <#26>	*
 *	******************************************************************	*/
pascal Boolean xSetTSMCursor(Point mousePos)
{

	short				i;
	ComponentResult		cmResult = 0;
	CIListRec			*aListRecPtr;
	TSMDocRecord		*idocRecPtr;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	TSMDocumentID		idocID;

	
	idocID = TSMVarsPtr->tsmActiveDocID;					/* use current TSM document ID as default	<#26> */
	if ( utVerifyTSMDocID(idocID) != noErr )				/* invalid id */
		return false;

	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
	if (idocRecPtr->iDocActiveP == false)					/* if not active  */
		return false;										/* stop right now */

	/* Now first pass the event to the default input method of the current key script if we're using one */
	/* Check if the document is using input window or old JIM, if it is, don't pass the event on */			

	if (utDocNeedsInputMethod(TSMVarsPtr->tsmCurrentSLRec.fScript, &idocRecPtr->iDocPSNRecord)) {
		/* Note: "utDocNeedsInputMethod" does not move memory */
		openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(TSMVarsPtr->tsmCurrentSLRec.fScript)]);
		if (openedDefIMRecordPtr->openedDefIMCInstance) {	/* already open */

#ifdef BuildTSMInit
			if (!openedDefIMRecordPtr->openedDefIMActiveP)	/* if the input method is NOT active */
				Debugger();									/* let me know */
#endif

			(void) BeginSystemMode();						/* input method is treated as system service	<#18> */
			TSMVarsPtr->tsmPatchFlags &= ~kIMJustSetCursor;	/* clear the flag so that IM can change the cursor again	<#50> */
			
			cmResult = SetTextServiceCursor(openedDefIMRecordPtr->openedDefIMCInstance, mousePos);
			/* if cmResult then SetCursor becomes a Noop until Flag is cleared by a 0 cmResult*/
			cmResult ? TSMVarsPtr->tsmPatchFlags |= kIMJustSetCursor : TSMVarsPtr->tsmPatchFlags &= ~kIMJustSetCursor;				
			
			(void) EndSystemMode();							/* close the window really quick				<#18> */
			if (cmResult != 0)								/* input method handled the call */
				return true;								/* so return true */
		}
	}



	HLock((Handle) idocID);									/* lock the handle */
	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
	aListRecPtr = idocRecPtr->iDocCIHead;
	for (i=0; i<idocRecPtr->iDocOpenTextServiceN; i++) {	/* call each open text service */
		if (aListRecPtr) {									/* to be safe */
			
			cmResult = SetTextServiceCursor(aListRecPtr->ciTS, mousePos);
			if (cmResult)									/* text service handled the call */
				break;										/* so break now ... */
			aListRecPtr = aListRecPtr->ciNextCIListRec;
		}
		else	break;
	}
	
	HUnlock((Handle) idocID);
	if (cmResult)			return true;
	else					return false;

};





/*	******************************************************************	*
 *	Function:	xGetServiceList											*
 *				Get all text services.									*
 *	Input:		numOfInterface											*
 *				supportedInterfaceTypes									*
 *	Output:		false if text service doesn't set the cursor,			*
 *				true  if text service set the cursor.					*
 *
 *	Side effect:If serviceInfo is nil, we will allocate the memory.		*
 *	******************************************************************	*/
pascal OSErr xGetServiceList(short numOfInterface, OSType supportedInterfaceTypes[], TextServiceListHandle *serviceInfo, long *seedValue)
{

	ComponentDescription	cd, dummyCD;
	Component				aComp;
	short					pos, index;
	long					count;
	OSErr					err;
	Handle					componentName = NewHandle(kComponentNameLength);


	if (componentName == nil)						/* no memory */
			return memFullErr;

	if (numOfInterface == 0)						/* can't be 0 */
		return paramErr;

	if (*serviceInfo == nil) {						/* allocate the handle */
		*serviceInfo = (TextServiceListHandle) NewHandle(sizeof(TextServiceList));
		if (*serviceInfo == nil)					/* no memory */
			return memFullErr;
	}

	(**serviceInfo)->fTextServiceCount = 0;


	
	for (pos=0; pos<numOfInterface; pos++) {
		cd.componentType			= supportedInterfaceTypes[pos];
		cd.componentSubType			=
		cd.componentManufacturer	=
		cd.componentFlags			=
		cd.componentFlagsMask		= 0;			/* don't care */
		
			
		count = CountComponents(&cd);
		if (count > 0) {							/* have TS component registered */
			aComp = FindNextComponent(nil, &cd);	/* find the first component */
			if (aComp == nil)						return tsmTextServiceNotFoundErr;
			index = 0;

			while (aComp) {
				if (count == 0) {
#ifdef BuildTSMInit
					Debugger();						/* debug for now */
#endif
					break;
				}

				err = GetComponentInfo(aComp, &dummyCD, componentName, nil, nil);
				if (err == noErr) {					/* got the info */
					/* check if the serviceInfo handle is big enough to receive this info */
					err = utMakeRoom4SInfoHandle(*serviceInfo, index);
					if (err != noErr)
						return err;
					
					(**serviceInfo)->fServices[index].fComponent = aComp;			/* save the component */
					BlockMove(*componentName, (**serviceInfo)->fServices[index].fItemName, kComponentNameLength);
					(**serviceInfo)->fTextServiceCount++;
					index++;
				}
				count--;
				aComp = FindNextComponent(aComp, &cd);
			}	/* end while */
		}		/* end if */
	}			/* end for */
	
	*seedValue = GetComponentListModSeed();
	return noErr;

};



/*	**********************************************************************	*
 *	Function:	xOpenTextService											*
 *				User chooses to open a text service, add this text 			*
 *				service to the TSM document record.							*
 *	Note: IM is not opened thru this code ! (we use utOpenIM4Document)		*
 *	Input:		idocID														*
 *				aComp = the component to open.								*
 *				aCompInstance = a pointer to a Component Instance.			*
 *	Output:		noErr if no error,											*
 *																			*
 *	Side effect:A CIListRec record is created if the TS is opened the		*
 *				first time and is save in a linked list in idocID.			*
 *				"InitiateTextService" is always called.						*
 *	Issues:		If the TS is already open, we return noErr???				*
 *				Does the document has to be active when this call is made?	*
 *																			*
 *	Modification History:													*
 *	04Jan92	KSCT	We now return the component instance to the caller,		*
 *					so that it can be closed later.							*
 *	**********************************************************************	*/
pascal OSErr xOpenTextService(TSMDocumentID idocID, Component aComp, ComponentInstance *aCompInstance)
{	ComponentResult			cmResult = 0;
	CIListRec				*aListRecPtr;
	TSMDocRecord			*idocRecPtr;
	Boolean					alreadyOpenedP = false;
	OSErr					err;
	short					i;
	ComponentInstance		ts = nil;
	MenuHandle				serviceMenuHandle = nil;
	ComponentDescription	cdRec;


	*aCompInstance = nil;									/* return nil if failed	-- <04Jan92 #7> */
	err = utVerifyTSMDocID(idocID);
	if (err != noErr)										/* invalid id */
		return err;

	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */

	aListRecPtr = idocRecPtr->iDocCIHead;					/* get the first open TS */
	for (i=0; i<idocRecPtr->iDocOpenTextServiceN; i++) {	/* search each open text service */
		if (aListRecPtr) {									/* just to be safe */
			if (aListRecPtr->ciTheComponent == aComp) {		/* already opened (according to Peter H. I can test for component equality */
				alreadyOpenedP = true;						/* set flag to true */
				ts = aListRecPtr->ciTS;						/* keep its ComponentInstance */
				break;										/* so break now ... */
			}
			aListRecPtr = aListRecPtr->ciNextCIListRec;
		}
	}	/* end for */


	if (!alreadyOpenedP) {									/* need to open the first time */
		err = GetComponentInfo(aComp, &cdRec, nil, nil, nil);
		if (err)											/* if can't get info, stop all the nonsense right here */
			return err;

		ts = OpenComponent(aComp);
		if (ts == nil)										/* can't open it */
			return tsmCantOpenComponentErr;					/* Component Manager never tells us what's wrong -- too bad */

		/* Component is open now ... */
		aListRecPtr = (CIListRec *) NewPtr(sizeof(CIListRec));
		if (aListRecPtr == nil) {
			CloseComponent(ts);
			return memFullErr;								/* sorry */
		}		

		/* fill in the fields of the record */
		aListRecPtr->ciTheComponent		= aComp;			/* save the component */
		aListRecPtr->ciTS				= ts;				/* save the instance  */
		aListRecPtr->ciSLRecord.fScript   = mGetScriptCode(cdRec);
		aListRecPtr->ciSLRecord.fLanguage = mGetLanguageCode(cdRec);
		aListRecPtr->ciTakeActiveInputP = (cdRec.componentFlags & bTakeActiveEvent) ? true : false;
		aListRecPtr->ciMenuVisibleP		= false;
		aListRecPtr->ciServiceMenu		= nil;

		cmResult = GetTextServiceMenu( ts, &serviceMenuHandle);
		if (cmResult == tsmComponentNoErr) {								
			aListRecPtr->ciServiceMenu = serviceMenuHandle;
			if ( serviceMenuHandle!=nil ) {
				InsertMenu(serviceMenuHandle, 0);
				DrawMenuBar();
				aListRecPtr->ciMenuVisibleP	= true;
			}
		}
		/* the call will not move memory */
		utAppendTS2Document((TSMDocRecordPtr) *((Handle) idocID), aListRecPtr);	/* add to the list */
	}
	
	*aCompInstance = ts;									/* return the instance	-- <04Jan92 #7> */
	cmResult = InitiateTextService(ts);						/* inform Component to start servicing  */
	/* what to do if there is an error ???? */
	cmResult = ActivateTextService(ts);						/* and activate the service */
	/* what to do if there is an error ???? */

	if (alreadyOpenedP)										/* here, cmResult should = noErr */
		cmResult = tsmComponentAlreadyOpenErr;				/* return a warning */

	return ((OSErr) cmResult);								/* return this as result */
};


/*	**************************************************************************	*
 *	Function:	xCloseTextService												*
 *				User chooses to close a text service, remove this text			*
 *				service from the TSM document record, and release its memory.	*
 *	Input:		idocID															*
 *				aCompInstance = the component instance to close.				*
 *	Output:		noErr if no error,												*
 *
 *	Side effect:The CIListRec record is removed from the linked list in idocID,	*
 *				the open text services counter is decremented by one,			*
 *				and if the text service has a menu, the menu is released and	*
 *				removed from the menu bar.										*
 *	Modification History:														*
 *	04Jan92	KSCT	New today.													*
 *	**************************************************************************	*/
pascal OSErr xCloseTextService(TSMDocumentID idocID, ComponentInstance aCompInstance)
{	ComponentResult		cmResult = 0;
	CIListRec			*aListRecPtr=nil, *prevRecPtr=nil;
	TSMDocRecord		*idocRecPtr;
	OSErr				err;


	err = utVerifyTSMDocID(idocID);
	if (err != noErr)										/* invalid id */
		return err;

	HLock((Handle) idocID);
	idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
	if (idocRecPtr->iDocOpenTextServiceN) {					/* if we have open text services */
		aListRecPtr = idocRecPtr->iDocCIHead;				/* get the first open TS record in the linked list */
		while (aListRecPtr) {								/* find the match */
			if (aListRecPtr->ciTS == aCompInstance) {		/* found, remove it from the list */

				if (aListRecPtr == idocRecPtr->iDocCIHead)	/* this is the first one in the list */
					idocRecPtr->iDocCIHead = aListRecPtr->ciNextCIListRec;
				else
					prevRecPtr->ciNextCIListRec = aListRecPtr->ciNextCIListRec;
				
				if (aListRecPtr->ciServiceMenu	&&			/* if the text service has a menu */
					(*(aListRecPtr->ciServiceMenu))->menuID != -kTSMSystemMenuID) {			/* and this is not a system menu */
						if (aListRecPtr->ciMenuVisibleP) {	/* if it is visible, then remove it */
							utDeleteMenu((*(aListRecPtr->ciServiceMenu))->menuID);			/* call our delete menu routine */
							aListRecPtr->ciMenuVisibleP = false;							/* in case some stupid guy open the input method as a text service */
						}

					DisposeHandle((Handle) aListRecPtr->ciServiceMenu);						/* release the menu */
					aListRecPtr->ciServiceMenu = nil;		/* no menu now */
				}
			
				idocRecPtr->iDocOpenTextServiceN--;			/* decrement the counter of open instances */
				DisposePtr((Ptr) aListRecPtr);				/* release its memory  */
				HUnlock((Handle) idocID);
				return CloseComponent(aCompInstance);		/* close the component and return */
			}
			prevRecPtr  = aListRecPtr;
			aListRecPtr = aListRecPtr->ciNextCIListRec;
		}
	}
	/* no text service to close, or the instance is not found */
	HUnlock((Handle) idocID);
	return tsmNoOpenTSErr;
	
}


/* used only by "xSendAEtoClient" */

OSErr utMapInstance2Refcon(ComponentInstance		aCompInstance,
							DocumentTable			*docTablePtr,
							long					*refcon)
{
	short				i, total;
	TSMDocumentID		idocID;
	TSMDocRecord		*idocRecPtr;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	CIListRec			*aListRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;


	if (TSMVarsPtr->tsmCacheInstance == aCompInstance) {		/* in the cache */
		*refcon = TSMVarsPtr->tsmCacheRefcon;					/* don't need to search */
		return noErr;
	}

	total = docTablePtr->docTableEntryN;						/* number of IDs recorded */
	
	for (i=0; i<total; i++) {
		idocID = docTablePtr->docIDStart[i];
		if (idocID == nil)		continue;						/* empty slot doesn't count */
		
		idocRecPtr  = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
#ifdef TSMDebug
		if (idocRecPtr->iDocSignature != kTSMDocumentSignature) {
			DebugStr("\pTSM: Map refcon failed");
			break;
		}
#endif
		/* first check if this instance is an input method */
		openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(TSMVarsPtr->tsmCurrentSLRec.fScript)]);
		if (openedDefIMRecordPtr->openedDefIMCInstance == aCompInstance) {
			*refcon = idocRecPtr->iDocRefcon;					/* return refcon */
			TSMVarsPtr->tsmCacheInstance = aCompInstance;		/* save the mapping in the cache */
			TSMVarsPtr->tsmCacheRefcon	 = *refcon;
			return noErr;
		}
		/* if this instance is not an input method, check other text services ... */
		if (idocRecPtr->iDocOpenTextServiceN) {					/* if we have open text services */
			aListRecPtr = idocRecPtr->iDocCIHead;				/* get the first open TS record in the linked list */
			while (aListRecPtr) {								/* find the match */
				if (aListRecPtr->ciTS == aCompInstance) {		/* found */
					*refcon = idocRecPtr->iDocRefcon;			/* return refcon */
					TSMVarsPtr->tsmCacheInstance = aCompInstance;	/* save the mapping in the cache */
					TSMVarsPtr->tsmCacheRefcon	 = *refcon;
					return noErr;
				}
				aListRecPtr = aListRecPtr->ciNextCIListRec;
			}
		}
	}	/* end for */
	
	return tsmInputMethodNotFoundErr;							/* return any non-zero error */
}


/*	******************************************************************	*
 *	Function:	xSendAEtoClient											*
 *				Sending callback AppleEvents to the application.		*
 *	Input:		theAppleEvent, ......									*
 *	Output:		????.													*
 *																		*
 *	Side effect:If the target app is not TSM aware, send the AE to SWM	*
 *				background application.									*
 *	Modification history:												*
 *	06Mar92	KST	Map the instance to document ID and return the refcon	*
 *				in each event record.									*
 *	06Mar92	KST	If the doc is using input window, events were routed	*
 *				to SWM. But if the event was sent from TS, then the		*
 *				event should be routed to the Application.				*
 *	******************************************************************	*/
pascal OSErr xSendAEtoClient(AppleEvent			*theAppleEvent,
							AppleEvent			*reply, 
							AESendMode			sendMode,
							AESendPriority		sendPriority,
							long				timeOutInTicks,
							IdleProcPtr			idleProc,
							EventFilterProcPtr	filterProc )
{
	
	OSErr					err;
	ProcessSerialNumber		PSNRecord;
	TSMVarsRec				*TSMVarsPtr = GetExpandMemTSMGlobals();
	AEAddressDesc			addrDescriptor;
	ComponentInstance		aComponentInstance = nil;
	ComponentDescription	cdRec;
	DescType				returnedType;
	long					tempLong;
	DocumentTable			*docTablePtr;
	Boolean					eventFromTextServiceP = false;	/* true = AE sent from IM, false = sent from TS */
	#pragma unused (addrDescriptor)


	err = GetCurrentProcess(&PSNRecord);
	if (err)				return tsmUnknownErr;			/* what is this ? */


	err = AEGetParamPtr(theAppleEvent, keyAEServerInstance, typeComponentInstance, &returnedType,
				(Ptr) &aComponentInstance, sizeof(ComponentInstance), &tempLong);		/* get ComponentInstance */
	if (err == noErr) {										/* we got the Component instance */
		err = GetComponentInfo( (Component) aComponentInstance, &cdRec, nil, nil, nil);
		if (err == noErr) {									/* got the info */
			eventFromTextServiceP = cdRec.componentSubType == kInputMethodService ? false : true;
		}
	}


	if (eventFromTextServiceP ||							/* if the AE is sent from Text Service, then send it the application */
		(utaGetTSMAwareDocTable(&PSNRecord, &docTablePtr) == noErr) ) {		/* or TSM aware app and is not using input window */
		if (aComponentInstance) {							/* we got the Component instance */
			err = utMapInstance2Refcon(aComponentInstance, docTablePtr, &tempLong);
			if (err == noErr) {								/* mapped the instance into refcon */
				err = AEPutParamPtr(theAppleEvent, keyAETSMDocumentRefcon, typeLongInteger, (Ptr) &tempLong, 4);
#ifdef TSMDebug
				if (err)
					DebugStr ( "\pPut refcon failed" );
#endif
			}
		}		/* endif we have the instance */

		return AESend(theAppleEvent, reply, sendMode, sendPriority, timeOutInTicks, idleProc, filterProc );
	}
	else {		/* send this AE to SWM, SWM doesn't need refcon 'cause it has only one document */
		return TSMVarsPtr->tsmSendAEProc( TSMVarsPtr->tsmSWMDrvrRefNum, theAppleEvent, reply );
	}
};


