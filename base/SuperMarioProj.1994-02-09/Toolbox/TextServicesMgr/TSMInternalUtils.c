/*
	File:		TSMInternalUtils.c

	Contains:	TSM internal utility routines.

	Written by:	Kenny SC. Tung

	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<48>	10/30/92	DTY		Use Get/Set macros to access ExpandMem.
		<47>	  8/4/92	KST		#1038606 <JH>: Apparently, when I ran the compare the crucial
									code change wasn't copied over. Trying again.
		<46>	  8/4/92	DCL		Changed spelling of Pallete to Palette (for HidePaletteWindow
									and all others)
		<45>	  8/4/92	KST		#1038311 <gbm>: Undo the previous fix. We want to call
									FixTSMDocument(). The right fix is in ScriptMgrKeyGetSet.a
		<44>	 7/31/92	KST		#1038311 <JH>: We can't call application to fix input area when
									script changes because the SetTextServiceLanguage() call is
									issued inside of the GetOSEventPatch of Script Manager code.
									When we are running inside of GetOSEvent(), the A5 points to
									Process Manager's global. If we call FixTSMDocument(), input
									method will send AE to the application, and very bad thing could
									happen when the app accesses its globals.
		<43>	 7/31/92	KST		#1037798 <GM>: <JH> Fixed a memory leak bug in
									utCleanUpPreviousDoc().
		<42>	 7/22/92	JH		#1031828 <KT>: On Rubiks when tsmFinder launches a application
									they deactivate the Finder's TSMDocuments. Then if a non-TSM
									aware calls InitMenus and then calls GetNextEvent, SWM will
									activate its document when it gets the accRun call (as a result
									of the GetNextEvent call). When the swm document is activated
									TSM will insert the input methods menu in the menulist. The
									trouble with this is that if there are no menus in the list
									(i.e. InitMenus was called but no insertMenu yet), the FEP menu
									will be the only system menu in the menulist. This apparently
									prevents the other system menus from being added. We fixed this
									by checking to see if there are any menus in the menulist before
									we insert the FEP menu. Change is in utCreateSystemVersionOfMenu.
		<41>	  7/8/92	KST		#1033475 <JH>: Changed the routine name utHasNoInputMethod() to
									utScriptIsNotCCJK() to be more descriptive.
		<40>	  7/6/92	KST		#1033507,1033505 <JH>: For the fix 38, when we close the
									instance, need to call HidePaletteWindows().
		<39>	  7/2/92	KST		#1034430 <LP>: When handling input methods, in some cases I
									forgot to check if the script is one of CCJK. If it is not we
									should not call utGetScriptIndex() because later on when
									accessing the array, we'll write to memory that does not belong
									to us and trashes the heap.
		<38>	  7/1/92	JH		#1033507,1033505 <KsT>: Adding Kurita's CheckHeapSlop call to
									here. Fromerly resided in ComponentMgr.
		<37>	 6/26/92	KST		#1033450,1033456,1031316 <JH>: utCloseIM4Document() will now
									take an extra parameter to indicate if it should send
									HidePalette() command to input method. Only set it to true when
									called in SetDefaultInputMethod().
		<36>	 6/24/92	KST		#1033456 <JH>: When we close an input method, we better take
									down its palette too. Added a new call
									utDeleteCleanUpTSMDocument().
		<35>	 6/22/92	KST		#1032492 <JH>: (For the #33 fix) When switching input methods,
									we need to remove the old input method's menu from the system
									menulist I was using the wrong routine, I should use
									'utDeleteInputMethodsPreviousMenu', not 'utDeleteMenu'.
		<34>	 6/22/92	KST		#1030843 <JH>: Close the instance event it is not active so that
									memory can be released. Undo change #28.
		<33>	 6/19/92	KST		#1032492 <JH>: When switching input methods within the same
									script, need to delete the previous menu and then insert the new
									one. In the case when input method does not have a menu, need to
									make sure no previous menu left over in the menu bar.
		<32>	 6/17/92	KST		#1030843 <JH>: Changed the function name from
									utaIsApplicationTSMAware to utaIsAppTSMAwareAndNotUsingIW to be
									more precise and descriptive.
		<31>	 6/16/92	KST		#1032645 <JH>: In utJustDeactivateIM4Document, need to set the
									document activep flag when there is no Component.
		<30>	 6/14/92	KST		#M. Sang 2, <JH>: When DeleteTSMDocument calls
									utCloseIM4Document, we remove IM's menu only from system menu
									list but keep it in app's menu so that when a TSM aware
									application (using Akiko in Roman promary script) deletes all
									the TSM documents, Akiko's menu will still stays in the menu
									bar.
		<29>	 6/14/92	KST		#M. Sang 1, <JH>: If we open the input method the first time, need
									to inform Process Manager if it should route events to SWM.
									(This is a new bug only happens when we use a TSM aware
									application in Roman as a primary script).
		<28>	 6/14/92	KST		in utCloseIM4Document: Check the flag and close the text service
									only if it is active.
		<27>	 6/13/92	KST		#1032463 <JH>: when switch from a TSM aware application, but is
									using Input Window, to Finder, we need to keep the pencil in
									that app's menu but we don't want to see it when we launch other
									applications.
		<26>	 6/10/92	KST		#1031142,1030881 <JH>: Since we removed the docID param from
									TSMEvent, UseInputWindow has been broken because keydown event
									is routed to SWM recursively. Fix it with a TSM global keeping
									track of which doc is using the input window. Changed many
									routines such as: utFixDocumentAndCloseSWMWindow,
									utJustDeactivateIM4Document in this file.
		<25>	  6/4/92	KST		#1031578 <JH>: Need to remove IM's menu from the system menulist
									when the primary script is not CJK.
		<24>	  6/2/92	pvh		Change MenuList to ApplicationMenuList.
		<23>	  6/2/92	JH		#1028635,1030481,1030631,1030634,1030647 <KST>: Added routine to
									turn input method windows into system menus (sorry can't
									remember the name and this is a dialog so I can't look)
		<22>	 5/20/92	KST		In utDeactivateIMforDocument, validate the document signature.
									If not valid, return tsmInvalidDocIDErr error.
		<21>	 5/19/92	KST		#1029247 <hsK>: Added a new routine: utBadSandL to verify the
									script and language code.
		<20>	  5/8/92	KST		Added documentation only.
		<19>	  5/1/92	KST		#1028881,<HSK>: If open IM for SWM's document, set the zone to
									system zone so that the instance will not get closed when the
									application exits (in utOpenIM4Document).
		<18>	 4/30/92	KST		Add some documentation describing the calls that activate Input
									Method for document.
		<17>	 4/20/92	YK		#1026761: Avoid HUnlocking of the disposed handle.
		<16>	 4/20/92	YK		#1026761: Before calling InsertMenu and DrawMenuBar, make sure
									that the menu handle is not NIL.
		<15>	 4/10/92	KST		JH,When switch from a TSM aware app to a non-TSM aware app, and
									both use the same IM, we didn't inform PM that it should
									start/stop sending events to SWM.
		<14>	  4/9/92	KST		JH,modifying utCloseSWMWindow to support the fact that SWM is no
									longer an application but a driver. Adding a new call
									"utSuspendResumeApp".
		<13>	 3/30/92	KST		JH, 1025008, 1025009. First attempt to solve the palette/menu
									bar flickering problem. Also, we synchronize the keyboard menu
									with the script of the TSM document.
		<12>	 3/23/92	KST		Code changed according to the comments from the code review.
		<11>	 3/12/92	KST		"utActivateThisDocsInputMethod" takes only a doc ID parameter.
		<10>	 3/10/92	KST		Need to lock document ID handle.
		 <9>	  3/6/92	KST		In "utCloseSWMWindow", send Apple Event only if we got SWM's
									process number.
		 <8>	  3/6/92	KST		Removed debugger code.
		 <7>	  3/2/92	KST		In "utOpenActivateDefaultIM" and "utOpenIM4Document", it now
									takes a Boolean flag to indicate if we should activate the input
									method.
		 <6>	 2/27/92	KST		In "utCloseSWMWindow", send AE failed because the app is not
									high level event aware.
		 <5>	 2/27/92	KST		Added 2 new calls -- "utCloseSWMWindow", and "utDeleteMenu".
									Remove the menu from menu bar only if tsmKillApplicationP flag
									is false.
		 <4>	 2/18/92	KST		In "utOpenIM4Document", When openning input method Component, we
									set the system to system mode so that files openned get charged
									to the system.
		 <3>	 2/11/92	DCL		Changed the name of TSMEqu.[aph] to TextServices.[aph] for
									better read-ability.
		 <2>	 2/10/92	KST		Moved all internal routines from TSMUtility.c and TSMFunction.c
									to TSMInternalUtil.c file.

*/



/*	********************************************************************************
 *	File:	"TSMInternalUtils.c"
 *
 *	Written  by Kenny SC. Tung
 *
 *	Modification History:
 *	09Feb92		KSCT	New today.
 *
 *	TSM internal utility routines:
 *
 *		utJustDeactivateIM4Document
 *		utDeactivateIMforDocument
 *		utDeleteCleanUpTSMDocument
 *
 *		utSupportedSL
 *		utAppendTS2Document
 *		utMakeRoom4SInfoHandle
 *		utHidePaletteAndMenu
 *		utCleanUpPreviousDoc
 *		utCreateSystemVersionOfMenu
 *		utActivateIM4Document
 *		utCloseIM4Document
 *
 *		utDeleteInputMethodsPreviousMenu
 *		utOpenIM4Document
 *		utOpenActivateDefaultIM
 *		utActivateThisDocsInputMethod
 *
 *		utScriptIsNotCCJK
 *		utScriptIsCCJKAndNeedsIM
 *		utGetScriptIndex
 *		utGetTSMDefaultInputMethod
 *		utSLRec2CompFlag
 *		utChangeToOldJIM
 *		utSuspendResumeApp
 *		utAddTSMResource
 *		utUpdateDefaultInputMethodResource
 *		utCloseSWMWindow						<#5>
 *		utBadSandL								<#21>
 *
 *
 *	utActivateIM4Document:			<< •• already open, just activate >>
 *		Activate the input method of the current script for this document.
 *		Add its menu and activate the instance.	The instance must be open already.
 *	utOpenIM4Document:				<< •• open an IM only >>
 *		Open the Component for the document as its default input method of the current script.
 *		Assume no input method of this script opened for this document !!
 *		This is the only path where I open an input method for the document.
 *	utOpenActivateDefaultIM:		<< •• open or activate >>
 *		Open the default input method for the current keyscript if it is not already open,
 *		-- with utOpenIM4Document.
 *		If the default input method is already open, then activate it,
 *		-- with utActivateIM4Document.
 *	utActivateThisDocsInputMethod:	<< •• synchronize IM used by the document >>
 *		This document was deactivated and is now being activated, if user is still using
 *		the same IM, then just activate it; -- with utActivateIM4Document.
 *		else, close the previous Input Method and open the new one.
 *		
 *	******************************************************************************** */

#include	<Traps.h>
#include	<OSUtils.h>
#include	<Script.h>
#include	<Devices.h>
#include	<MFPrivate.h>
#include	<Resources.h>
#include	<TextServices.h>
#include	<TSMPrivate.h>
#include 	<Menus.h>




/*	***************************************************************	*
 *	Function:	utJustDeactivateIM4Document							*
 *				Deactivate the IM for the Document only. 			*
 *				stillActiveP flag tells if the document should keep	*
 *				active or not.										*
 *				If the IM is already deactivated, return noErr.		*
 *	Input:		docID, script code, and stillActiveP flag.			*
 *	***************************************************************	*/
 
OSErr utJustDeactivateIM4Document( TSMDocumentID theDocID,	ScriptCode	theScript, Boolean	stillActiveP )
{
	OSErr				err = noErr;
	TSMDocRecord		*idocRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;


	idocRecPtr  = (TSMDocRecord *) *((Handle) theDocID);				/* dereference the handle */
	
	if (! idocRecPtr->iDocActiveP)										/* if not active  */
		return tsmDocNotActiveErr;										/* stop right now */

	if (!utScriptIsNotCCJK(theScript)) {								/* if the script needs IM */
		/* Deactivate the default input method of the current script if it is open */
		openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(theScript)]);
	
		if (openedDefIMRecordPtr->openedDefIMCInstance) {				/* if there is one open */
			if (openedDefIMRecordPtr->openedDefIMActiveP) {				/* if the input method is active */
				err = DeactivateTextService(openedDefIMRecordPtr->openedDefIMCInstance);	/* what the app can do on error ??? */
				if (err == noErr) {
					idocRecPtr  = (TSMDocRecord *) *((Handle) theDocID);/* dereference the handle */
					openedDefIMRecordPtr->openedDefIMActiveP = false;	/* not active anymore */
				}
			}
		}
	}
	idocRecPtr->iDocActiveP = stillActiveP;								/* keep it active option */
	return err;

}


/* **************************************************** *
 * utBadSandL:											*
 *	Verify the Language code is correct corresponding	*
 *	to the script code., if NOT valid return true.		*
 *	( Congress will bail out the bad S&L. )				*
 * **************************************************** */
Boolean utBadSandL(ScriptLanguageRecord	*slRecPtr)
{	LangCode	languageCode;

	languageCode = (LangCode) GetScript(slRecPtr->fScript, smScriptLang);
	return (languageCode != slRecPtr->fLanguage ? true : false );
}


/* ******************************************** *
 * Verify the idocID, if valid return true.		*
 * ******************************************** */

OSErr	utVerifyTSMDocID(TSMDocumentID	idocID)
{
	
	if (idocID == nil ||
		((TSMDocRecordPtr) *(Handle) idocID)->iDocSignature != kTSMDocumentSignature )
			return tsmInvalidDocIDErr;
	else	return noErr;									/* ID verified */
}


/* ******************************************** *
 * append the object to the linked list of open	*
 * text services in the document record.		*
 * Assume: this routine does not move memory	*
 * ******************************************** */
 
void	utAppendTS2Document(TSMDocRecord	*idocRecPtr,
							CIListRec		*theListRecPtrToAppend)
{
	CIListRec	*aListRecPtr;

	if (idocRecPtr->iDocOpenTextServiceN) {				/* if we have open text services */
		aListRecPtr = idocRecPtr->iDocCIHead;			/* get the first open TS record in the linked list */
		while (aListRecPtr->ciNextCIListRec)			/* find the end of the list */
			aListRecPtr = aListRecPtr->ciNextCIListRec;

		aListRecPtr->ciNextCIListRec = theListRecPtrToAppend;
	}
	else {		/* this is the first one open */
		idocRecPtr->iDocCIHead = theListRecPtrToAppend;
	}
	theListRecPtrToAppend->ciNextCIListRec = nil;		/* mark the end */
	idocRecPtr->iDocOpenTextServiceN++;					/* bump the counter of open instances */
}


/* ************************************************************************
 *	Check if the handle has memory for the Nth record, N specified in pos,
 *	Return noErr if there is enough room ...
 *	Recall the structure is:
 *		typedef struct {
 *				short				fTextServiceCount;
 *				TextServiceInfo		fServices[fScriptLanguageCount];
 *		} TextServiceList;
 *	***********************************************************************/

OSErr	utMakeRoom4SInfoHandle(
			TextServiceListHandle	serviceInfoHandle,
			int						pos)
{
	Size	requestSize, handleSize;
	OSErr	err = noErr;
	
	handleSize  = GetHandleSize((Handle) serviceInfoHandle);
	requestSize = 2 + sizeof(TextServiceInfo) * (pos+1);
	if (handleSize < requestSize) {					/* not enough space */
		SetHandleSize((Handle) serviceInfoHandle, requestSize);
		err = MemError();
	}
	return err;
}


/* *******************************************************************************	*
 * Function: utHidePaletteAndMenu													*
 *	Hide input method's palette, if it has a menu, remove it too.					*
 *	Used only in this file by utCleanUpPreviousDoc and utForceDeactivateIM4Document	*
 *	Input:	openedDefIMRecordPtr = the input method record to clean up.				*
 * *******************************************************************************	*/
void	utHidePaletteAndMenu(DefIMInstanceRecord	*openedDefIMRecordPtr)
{
	/* if it has a menu and the menu is displayed in the menu bar, then remove the menu */
	if (openedDefIMRecordPtr->openedDefIMServiceMenuHdl && openedDefIMRecordPtr->openedDefIMMenuVisibleP) {	
		utDeleteMenu((*(openedDefIMRecordPtr->openedDefIMServiceMenuHdl))->menuID);
		openedDefIMRecordPtr->openedDefIMMenuVisibleP = false;				/* not visible anymore */
	}
	(void) HidePaletteWindows(openedDefIMRecordPtr->openedDefIMCInstance);
}


/* ************************************************************************************	*
 * Function: utCleanUpPreviousDoc														*
 *	we deactivated the previous TSM document without taking down its palette now we are	*
 *	activating another TSM document, if scripts are different or input methods are		*
 *	different, then need to ask the previous IM to hide its	palette.					*
 *																						*
 *	Input:	theNewDocID = new document to activate, it could be nil when called			*
 *			from xSetTextServiceLanguage where there is no new document to activate.	*
 *			(this is not the previous document ID) !!									*
 * Modification history:																*
 *	30Mar92	KST	The tsmPreviousDocID could contain a shadow ID handle because the real	*
 *				ID has been deleted (xDeleteTSMDocument). Check iDocSignature.			*	
 * ************************************************************************************	*/
void utCleanUpPreviousDoc(TSMDocumentID		theNewDocID,			/* new document to activate */
						ScriptCode			theNewScript)
{
	Boolean				need2HidePalette = false;
	short				imIndex;
	Component			currentComponent = nil;
	ScriptCode			previousScript;
	TSMDocRecord		*idocRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();


	previousScript = TSMVarsPtr->tsmPreviousScript;
	if (utScriptIsNotCCJK(previousScript)) {						/* if the script doesn't need IM, such as Arabic or Roman */
		TSMVarsPtr->tsmPreviousDocID = nil;							/* done			<12Jun92 #27> */
		return;
	}


	if (utScriptIsCCJKAndNeedsIM(theNewScript)) {					/* this is one of CJK */
		
		imIndex = utGetScriptIndex(theNewScript);					/* index for the current script */
		if (theNewDocID) {
			idocRecPtr  = (TSMDocRecordPtr) *((Handle) theNewDocID);				/* dereference the current ID */
			openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[imIndex]);	/* the default IM we will use */
			currentComponent = openedDefIMRecordPtr->openedDefIMComponent;			/* the current Component */
		}
		//else
		//	currentComponent = nil;									/* if script is the same, then this will take down the palette */
	}

	/* dereference here so that openedDefIMRecordPtr can be used when need2HidePalette is true */
	HLock((Handle) TSMVarsPtr->tsmPreviousDocID);
	idocRecPtr = (TSMDocRecordPtr) *((Handle) TSMVarsPtr->tsmPreviousDocID);	/* dereference the previous ID */
	openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex( previousScript )]);
	if (idocRecPtr->iDocSignature == kTSMShadowDocumentSig)			/* a special ID */
		openedDefIMRecordPtr->openedDefIMCInstance = nil;			/* make sure it is nil */

	/* if there is any instance, then hide the Palette ... */
	/* else, the Palette should be taken down by the IM. when the last instance is closed.	<#45> */
	if ( CountComponentInstances( openedDefIMRecordPtr->openedDefIMComponent ) > 0 ) {
		if (previousScript == theNewScript) {						/* same script, check if IM changed */
			if (currentComponent != openedDefIMRecordPtr->openedDefIMComponent)		/* IM switched */
				need2HidePalette  = true;							/* then need to clean up the previous IM stuff */
		}
		else														/* different scripts */
			need2HidePalette  = true;								/* input method must be also changed */
	}

	if (need2HidePalette) {											/* if we need to hide the Palette, then need to open the Component */
		if (idocRecPtr->iDocSignature == kTSMShadowDocumentSig) {	/* a special ID */
			openedDefIMRecordPtr->openedDefIMServiceMenuHdl = nil;	/* shadow document does not have menu */
			openedDefIMRecordPtr->openedDefIMCInstance = OpenComponent(openedDefIMRecordPtr->openedDefIMComponent);
		}															/* open the component for the next call */
		if ( openedDefIMRecordPtr->openedDefIMCInstance )			/* if we have open Component */
			utHidePaletteAndMenu(openedDefIMRecordPtr);				/* then talk to it */
	}

	if (idocRecPtr->iDocSignature == kTSMShadowDocumentSig) {		/* a special ID */
		if (openedDefIMRecordPtr->openedDefIMCInstance)				/* need to close the Instance, */
			(void) CloseComponent(openedDefIMRecordPtr->openedDefIMCInstance);
		DisposeHandle((Handle) TSMVarsPtr->tsmPreviousDocID);		/* and release the handle */
		TSMVarsPtr->tsmPreviousDocID = nil;							/* avoid HUnlocking to tsmPreviousDocID <17> */
	}
		
	HUnlock((Handle) TSMVarsPtr->tsmPreviousDocID);
	TSMVarsPtr->tsmPreviousDocID = nil;								/* done */
}

/********************************************************************
*	take the menuresult and change the ID to the input method
*	then send it to the input method
**********************************************************************/
pascal void ChangeMenuResultForInputMethod( long *MenuResult )
{
	register TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	short				imIndex;
	TSMDocRecord		*idocRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	long				cmResult;
	
	imIndex = utGetScriptIndex(TSMVarsPtr->tsmCurrentSLRec.fScript);
	idocRecPtr = (TSMDocRecordPtr) *((Handle) TSMVarsPtr->tsmActiveDocID);	/* dereference the previous ID */
	openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[imIndex]);

	if ( openedDefIMRecordPtr->openedDefIMServiceMenuHdl ) {
	
		if ( *MenuResult >> 16 == -kTSMSystemMenuID  ) {
			
			(void) BeginSystemMode();					/* input method is treated as system service	<#18> */
			cmResult = TextServiceMenuSelect(openedDefIMRecordPtr->openedDefIMCInstance, openedDefIMRecordPtr->openedDefIMServiceMenuHdl, (short)*MenuResult);
			(void) EndSystemMode();						/* close the window really quick				<#18> */
		
		}

	}
	
}


	
/*******************************************************************************
* Function utCreateSystemVersionOfMenu
*		Take a menu that belongs to an input method, make a copy of it,			
*		change the ID of our copy to be our special -16500 menuID, and stick it
*		in the system Menu
********************************************************************************/
pascal void SetRomMapInsertToTrue() =
{
	0x31FC, 0xFFFF, 0x0B9E         		//move.w	#MapTrue,RomMapInsert
};

Boolean utCreateSystemVersionOfMenu( MenuHandle mHdl, TSMVarsRec	*TSMVarsPtr)
{
	#define MBDFHndl			(0x0B58)
	#define kKeyboardMenuID		(-16491)	// -16491: The keyboard menu ID
	
	typedef struct MListRecord {
		short	lastMenu;				//Offset to last regular menu in the MenuList }
		short	lastRight;				//Pixel location of rgt edge of rgtmost menu }
		short	mbResID					//High 13 bits = RSRC ID of MBDF, Low 3 = MBVariant }
	} MListRecord;
	
	typedef pascal long (*CallMBARProc)( short selector, short message, short param1, long param2);
	
	long	theMenuList = ( *(long *)ApplicationMenuList );
	long	theSystemMenuList = ( *(long *)SystemMenuList );
	Handle	mbarHandle;
	long	hstate;
	short	numberOfMenusInList;
	
	if ( theMenuList == 0L )
		return false;
		
	numberOfMenusInList = **(short **)(theMenuList);
	numberOfMenusInList /= 6;
	
	if ( numberOfMenusInList == 0 )
		return false;
		
	
	
	//
	//remember the old menuID and Change the menuID of the menu to -16500
	//
	TSMVarsPtr->tsmRealSystemMenuID = (*mHdl)->menuID;
	(*mHdl)->menuID = -kTSMSystemMenuID;
	

	//
	//now load the iconsuite for the component(maybe)
	//
	if ( theMenuList != nil  ) {
	
		InsertMenu(mHdl,kKeyboardMenuID);
		if ( theSystemMenuList != nil ) {			//change menulist to be the systemmenulist
			
			( *(long *)SystemMenuList ) = theMenuList;
			InsertMenu(mHdl,kKeyboardMenuID);
			( *(long *)SystemMenuList ) = theSystemMenuList;
			
		}
		
		//
		//call the MBDF proc to update
		//
		SetRomMapInsertToTrue();
		mbarHandle = GetResource( 'MBDF',(*(MListRecord **)theMenuList)->mbResID & (short)0xFFF8);
		if (mbarHandle) {							// check for error
			
			(* (Handle *)MBDFHndl) = mbarHandle;
			hstate = HGetState(mbarHandle);
			HLock(mbarHandle);
			(void)((CallMBARProc)(*mbarHandle))((*(MListRecord **)theMenuList)->mbResID & (short)0x0007,2,0,0);
			HSetState(mbarHandle, hstate);
		}
		InvalMenuBar();
		return true;
	}
	return false;
}
		

/* ***************************************************************************	*
 * Function:  utActivateIM4Document												*
 *		Activate the input method of the current script for this document.		*
 *		Add its menu and activate the instance.	The instance is not opened anew	*
 *		If the instance is not open, this call returns an error.				*
 *		•• Warning:	1. No error checking on the doc ID !!						*
 *																				*
 * Modification history:														*
 *	25Mar92	KST		If there is a previous document ID saved in the TSM global	*
 *					then need to make check if we need to remove its palette.	*
 * ***************************************************************************	*/
OSErr	utActivateIM4Document(
						TSMDocumentID		theDocID,
						ScriptCode			theScript)
{
	TSMDocRecord		*idocRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	MenuHandle			serviceMenuHandle = nil;
	ComponentResult		cmResult = 0;


	if (TSMVarsPtr->tsmPreviousDocID) {								/* something to clean up */
		utCleanUpPreviousDoc(theDocID, theScript);
	}

	idocRecPtr  = (TSMDocRecordPtr) *((Handle) theDocID);			/* dereference the handle */
	
	/* Activate the default input method of the current script if it is open */
	openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(theScript)]);

	if (openedDefIMRecordPtr->openedDefIMCInstance) {				/* if there is one open */
		if (openedDefIMRecordPtr->openedDefIMActiveP) 				/* if the input method is already active */
			return noErr;


		HLock((Handle) theDocID);									/* <10Mar92 #10> */
		cmResult = InitiateTextService(openedDefIMRecordPtr->openedDefIMCInstance);
		cmResult = ActivateTextService(openedDefIMRecordPtr->openedDefIMCInstance);	/* what the app can do on error ??? */
		if (cmResult == tsmComponentNoErr) {
			openedDefIMRecordPtr->openedDefIMActiveP = true;		/* active now */
	
			if (openedDefIMRecordPtr->openedDefIMServiceMenuHdl) {	/* if the IM has menu */
				openedDefIMRecordPtr->openedDefIMMenuVisibleP = utCreateSystemVersionOfMenu(openedDefIMRecordPtr->openedDefIMServiceMenuHdl, TSMVarsPtr);
				//InsertMenu(openedDefIMRecordPtr->openedDefIMServiceMenuHdl, 0);
				//DrawMenuBar();
				//openedDefIMRecordPtr->openedDefIMMenuVisibleP = true;/* menu is visible */
			}
		}
		HUnlock((Handle) theDocID);									/* <10Mar92 #10> */
		return cmResult;
	}
	else	return tsmTSNotOpenErr;									/* IM has never opened */
}



/* ********************************************************************************	*
 * Function:  utDeactivateIMforDocument												*
 *		Deactivate the input method of the current script for this document.		*
 *		Only deactivate the instance. The instance is not closed.					*
 *		If the instance is not open, this call is just a noop.						*
 *		If theHideFlag == true, then remove its menu and call utHidePaletteAndMenu.	*
 *		If theHideFlag == false, then don't remove its menu.						*
 *		%% This routine has no public entry point, it is used only internally by	*
 *		%% utForceDeactivateIM4Document, and utSoftDeactivateIM4Document.			*
 * ••	Warning:1. Assume theScript is one of the CCJK scripts.						*
 *				2. The document is still active after the call.						*
 *																					*
 * Modification history:															*
 * 27Feb92	KST	If tsmKillApplicationP flag is set, then the menu is 				*
 *				already killed by Process Manager, don't do "DeleteMenu" !!			*
 * 24Mar92	KST	When we deactivate the input method, now TSM will not				*
 *				remove the menu from the menu bar. Instead, the ID is saved			*
 *				in TSM globals, and delay the operation later in Activate.			*
 * 24Mar92	KST	Added 2 new calls:													*
 *				utForceDeactivateIM4Document (theHideFlag = true),					*
 *				utSoftDeactivateIM4Document  (theHideFlag = false).					*
 * ********************************************************************************	*/
OSErr	utDeactivateIMforDocument(
						TSMDocumentID	theDocID,
						ScriptCode		theScript,
						Boolean			theHideFlag)
{
	TSMDocRecord		*idocRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	MenuHandle			serviceMenuHandle = nil;
	OSErr				err = noErr;


	/* if a TSM aware app. exits without calling CloseTSMAwareApp, its docID is invalid */
	/* this check will prevent from accessing garbage doc ID.					<#22>	*/
	if (utVerifyTSMDocID(theDocID))									return tsmInvalidDocIDErr;

	/* For a TSM aware app. its docID can be valid, but the script is changed to Arabic. */
	if ( utScriptIsNotCCJK( theScript ) )							/* if not one of CJK */
		return noErr;
	
	idocRecPtr  = (TSMDocRecordPtr) *(Handle) theDocID;				/* dereference the handle */
	
	/* Deactivate the default input method of the current script if it is open */
	openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(theScript)]);

	if (openedDefIMRecordPtr->openedDefIMCInstance) {				/* if there is one open */
		if (!openedDefIMRecordPtr->openedDefIMActiveP)				/* if the input method is NOT active */
			return noErr;											/* nothing to do then */

		HLock((Handle) theDocID);									/* <10Mar92 #10> */

		if (theHideFlag) {											/* force to hide the palette */
			utHidePaletteAndMenu(openedDefIMRecordPtr);				/*		<29Mar92 #13> */
			TSMVarsPtr->tsmPreviousDocID  = nil;					/* clear		<#13> */
			TSMVarsPtr->tsmPreviousScript = kNoSuchScript;			/* don't care	<#13> */
		}
		else {
			/* Don't hide the palette/menu, just save the document ID we are deactivating */
			TSMVarsPtr->tsmPreviousDocID  = theDocID;				/* save this ID		<#13> */
			TSMVarsPtr->tsmPreviousScript = theScript;				/* save this script <#13> */

			/* don't remove the menu now (not done yet) ????? */
			/* if we do this, this means input method can have only one global menu handle */
//			if (openedDefIMRecordPtr->openedDefIMServiceMenuHdl && openedDefIMRecordPtr->openedDefIMMenuVisibleP) {		/* if the menu is displayed in the menu bar */
//				utDeleteMenu((*(openedDefIMRecordPtr->openedDefIMServiceMenuHdl))->menuID);
//				openedDefIMRecordPtr->openedDefIMMenuVisibleP = false;	/* not visible anymore */
//			}
		}

		if (openedDefIMRecordPtr->openedDefIMActiveP ) {			/* we know the input method is active */
			(void) DeactivateTextService(openedDefIMRecordPtr->openedDefIMCInstance);	/* what the app can do on error ??? */
			openedDefIMRecordPtr->openedDefIMActiveP = false;		/* not active anymore */
		}
		HUnlock((Handle) theDocID);									/* <10Mar92 #10> */
	}
	else	err = tsmTSNotOpenErr;

	return err;
}


/* ****************************************************	*
 * Function:  utDeleteCleanUpTSMDocument				*
 *		Delete the document and then clean it up.		*
 *		This will remove the palette when app crashes.	*
 *		Used in XCLOSETSMAWAREAPPLICATION() only.		*
 * Modification history:								*
 *	23Jun92	KST	New today								*
 * ****************************************************	*/
void utDeleteCleanUpTSMDocument(TSMDocumentID	theDocID)
{	OSErr			err;
	TSMVarsRec		*TSMVarsPtr = GetExpandMemTSMGlobals();


	err = xDeleteTSMDocument(theDocID);							/* delete the document */
	if (err == noErr) {
		if (TSMVarsPtr->tsmPreviousDocID)						/* something to clean up */
			utCleanUpPreviousDoc(nil, TSMVarsPtr->tsmPreviousScript);
	}
}


/* ************************************************************************	*
 * Function: utCloseIM4Document												*
 *		Close the input method of the current script for this document.	 	*
 *		If there is no Input Method open, then return noErr.				*
 * •• Warning:	1. No error checking on the doc ID !!						*
 * 09Apr92	KST	IM's menu handle is never released by TSM. IM should		*
 *				use one menu and keep track of it itself.					*
 * 14Jun92	KST	Delete from system menu list but keep in app's menu so that	*
 *				when a TSM aware application (using Akiko in Roman promary	*
 *				script) deletes all the TSM documents, Akiko's menu will	*
 *				still stays in the menu bar.								*
 * 18Jun92	KST	After closing the input method, reset TSM globals.	<#33>	*
 * 23Jun92	KST	Take down the palette as well when hidePaletteP is true.	*
 * ************************************************************************	*/
OSErr	utCloseIM4Document(
	TSMDocumentID		theDocID,
	ScriptCode			theScript,
	Boolean				hidePaletteP)									/* only true when called by SetDefaultInputMethod */
{
	TSMDocRecord		*idocRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	MenuHandle			serviceMenuHandle = nil;
	short				imIndex;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();


	idocRecPtr  = (TSMDocRecordPtr) *((Handle) theDocID);			/* dereference the handle */
	
	/* close the default input method of the current script if it is open */
	openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(theScript)]);

	if (openedDefIMRecordPtr->openedDefIMCInstance) {					/* if there is one open */
		HLock((Handle) theDocID);										/* <10Mar92 #10> */
		if (openedDefIMRecordPtr->openedDefIMServiceMenuHdl) {			/* if it has a menu */
			if (openedDefIMRecordPtr->openedDefIMMenuVisibleP) {		/* and if menu is visible */

				if (CountComponentInstances( openedDefIMRecordPtr->openedDefIMComponent ) == 1) {
				/* If this is the last instance, remove the menu from the menulist before closing it,		*
				 * because when closing the last instance, input method will dispose the menu handle! ...	*/

					utDeleteMenu((*(openedDefIMRecordPtr->openedDefIMServiceMenuHdl))->menuID);
					imIndex = utGetScriptIndex(theScript);								/* index for the new script	 <#45> */
					TSMVarsPtr->tsmDefIMInfoTable[imIndex].gMenuHandle	= nil;			/* no menu handle now		 <#45> */
					
					/* If we delete the last instance because we switch to another input method within the same script,	*/
					/* utDeleteMenu() will not delete the menu from the system menulist, so we have to do it explicitly */
					utDeleteSysMenu((*(openedDefIMRecordPtr->openedDefIMServiceMenuHdl))->menuID, 0);		/* <14Jun92 #30> */
				}
		
				//systemLong = GetEnvirons(smSysScript);			/* need to remove it for non-CJK primary script<14Jun92 #30> */
				//utDeleteSysMenu((*(openedDefIMRecordPtr->openedDefIMServiceMenuHdl))->menuID, systemLong);		/* <14Jun92 #30> */
			}
			// DisposeHandle((Handle) openedDefIMRecordPtr->openedDefIMServiceMenuHdl);	/* release the menu handle */
			// never release the menu ....
		}
		
		if (hidePaletteP)
			(void) HidePaletteWindows(openedDefIMRecordPtr->openedDefIMCInstance);		/* hide it please	<#36> */
		
		if (openedDefIMRecordPtr->openedDefIMActiveP)					/* if it is active	<24Jun92 #37> */
			(void) DeactivateTextService(openedDefIMRecordPtr->openedDefIMCInstance);	/* what the app can do on error ??? */

		CloseComponent(openedDefIMRecordPtr->openedDefIMCInstance);		/* and then close it */

		/* reset all flags and globals ... */
		/* close IM doesn't change this information. Open IM will update it */
		// imIndex = utGetScriptIndex(theScript);						/* index for the new script	 <#33> */
		// TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent	= nil;	/* save this new comp		 <#33> */
		// TSMVarsPtr->tsmDefIMInfoTable[imIndex].gMenuHandle	= nil;	/* no menu handle now		 <#33> */

		openedDefIMRecordPtr->openedDefIMCInstance		= nil;
		openedDefIMRecordPtr->openedDefIMComponent		= nil;
		openedDefIMRecordPtr->openedDefIMServiceMenuHdl = nil;
		openedDefIMRecordPtr->openedDefIMMenuVisibleP	= false;
		openedDefIMRecordPtr->openedDefIMActiveP		= false;		/* clear every thing */
		HUnlock((Handle) theDocID);										/* <10Mar92 #10> */
	}
	return noErr;
}



/* **************************************************************** *
 * Remove the previous menu from menu bar because the input method	*
 * has no menu.	Called only by 'utOpenIM4Document'.			<#33>	*
 *																	*
 * 18Jun92	KST	New today.											*
 * **************************************************************** */
void utDeleteInputMethodsPreviousMenu(short menuID)
{
	long		theMenuList;
	long		theSystemMenuList = ( *(long *)SystemMenuList );


	DeleteMenu(menuID);
	if ( theSystemMenuList != nil ) {							/* change menulist to be the systemmenulist */
		theMenuList = ( *(long *) ApplicationMenuList );		/* save app's menulist */
		( *(long *) ApplicationMenuList ) = theSystemMenuList;
		DeleteMenu(menuID);
		( *(long *) ApplicationMenuList ) = theMenuList;		/* restore */
	}
	InvalMenuBar();			/* this does not redraw */
}



// avoid linking in evil glue.	we don't have PurgeSpaceSys, anyway.
#pragma parameter __A0 PurgeSpaceSysContig
pascal unsigned long
PurgeSpaceSysContig(void)	= {0xA562};
/*	_PurgeSpace	,SYS
 *	On exit	A0:	contig	(long word)
 *			D0:	total	(long word)
 */


Boolean	utCheckSysHeapSlop()						/*	for Cube-E by YK. */
{
	unsigned long	minSysHeapFreeSpace;
	unsigned long	hiSysRoom, lowTempRoom;
	Handle			tempHandle;

	minSysHeapFreeSpace = GetExpandMemMinSysHeapFreeSpace();

	if ( GetExpandMemProcessMgrExists() )			//	if the Process Manager is there,
		{											//	the bottom part of the Process Mgr heap can be considered potential System Heap.

		SysZoneFloatSizes( nil, &lowTempRoom);		//	try fairly cheap call to get free space at bottom of PM heap.
		if ( lowTempRoom>=minSysHeapFreeSpace )
			return true;

		SysZoneFloatSizes( &hiSysRoom, nil);		//	try more expensive call for free space at top of System heap.
		if ( (hiSysRoom+lowTempRoom)>=minSysHeapFreeSpace )
			return true;

		if ( PurgeSpaceSysContig()>=minSysHeapFreeSpace )	//	take purgeable space into account.
			return true;

		}
	else
		{

		if ( PurgeSpaceSysContig()>=minSysHeapFreeSpace )
			return true;

		tempHandle = NewHandle( minSysHeapFreeSpace);		//	hope System heap grows.
		if ( tempHandle!=nil )
			{
			DisposeHandle( tempHandle);
			return true;
			}

		}

	return false;
}



/* ****************************************************************************************	*
 * Function: utOpenIM4Document (Just open)													*
 * Open this Component for the document as its default input method of the current script.	*
 * •• Warning:	1. Assume no input method of this script opened for this document !!		*
 *				2. No error checking on the doc ID !!										*
 *				3. We are not using the old Japanese input method.							*
 *																							*
 * Modification history:																	*
 *	18Feb92	KST	Call BeginSystemMode before OpenComponent and BeginSystemMode after. <#4>	*
 *	02Mar92	KST	It now takes a Boolean flag to indicate if we should activate the input		*
 *				method. (SWM creates new TSM document but don't want IM activated.)			*
 *				If the flag is false, then we will not activate the input method, and will	*
 *				not insert its menu.														*
 *	30Apr92	KST	If open IM for SWM's document, set the zone to system zone so that the		*
 *				instance will not get closed when the application exits.			<#19>	*
 * 18Jun92	KST	After openning the input method, save the Component and menu handle!<#33>	*
 * ****************************************************************************************	*/
OSErr	utOpenIM4Document(
	TSMDocumentID		theDocID,
	Component			theComponent,
	ScriptCode			theScript,
	Boolean				activateP)
{
	TSMDocRecord		*idocRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	ComponentInstance	aCompInstance;
	MenuHandle			serviceMenuHandle = nil;
	ComponentResult		cmResult = 0;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	short				imIndex;
	THz					aZone = nil;
	

	imIndex = utGetScriptIndex(theScript);
	HLock((Handle) theDocID);									/* <10Mar92 #10> */
	idocRecPtr  = (TSMDocRecordPtr) *((Handle) theDocID);		/* dereference the handle */
	
	/* open the default input method of the current script if it is not already opened */
	openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[imIndex]);

	/* open the IM Component in system mode ... */
	(void) BeginSystemMode();									/* so that files will be openned in system mode <# 4>	*/
	if (idocRecPtr->iDocPSNRecord == TSMVarsPtr->tsmSWMsPSNRecord)			/* only if doc belongs to SWM		<#19>	*/
		aZone = utSwapZone( SystemZone() );						/* open the Component in system zone			<#19>	*/
	aCompInstance = OpenComponent(theComponent);
	(void) EndSystemMode();										/* and use MF temp memory ... <#4> */

	if (aCompInstance == nil) {									/* can't open it */
		HUnlock((Handle) theDocID);								/* <10Mar92 #10> */
		if (aZone)		(void) utSwapZone(aZone);				/* restore the zone			<#19> */
		return tsmCantOpenComponentErr;							/* Component Manager never tells us what's wrong -- too bad */
	}
	
	if ( !utCheckSysHeapSlop() ) {								/* Ok if it is opened, check the sysheap slop, if not enough close it */
		(void) HidePaletteWindows(aCompInstance);				/* thank you */
		(void) CloseComponent(aCompInstance);					/* close the instance */
		HUnlock((Handle) theDocID);							
		if (aZone)		(void) utSwapZone(aZone);				/* restore the zone		*/
		return memFullErr;										/* return the MemFull error in this case */
	}

	
	/* associate the instance with the document */
	openedDefIMRecordPtr->openedDefIMCInstance			= aCompInstance;
	openedDefIMRecordPtr->openedDefIMComponent			= theComponent;
	
	TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent	= theComponent;				/* save this new component	 <#33> */
	
	if (activateP) {											/* activate the input method only if flag is true	<#7> */
		cmResult = InitiateTextService(aCompInstance);			/* inform Component to start servicing  */
		
		cmResult = ActivateTextService(aCompInstance);			/* and activate the service */
		
		if (cmResult == tsmComponentNoErr)
			openedDefIMRecordPtr->openedDefIMActiveP  = true;	/* I.M. is active  */
		else
			return cmResult;
	}

	cmResult = GetTextServiceMenu(aCompInstance, &serviceMenuHandle);
	if (cmResult == tsmComponentNoErr) {						/* it has menu */
		openedDefIMRecordPtr->openedDefIMServiceMenuHdl	= serviceMenuHandle;
		
		if (TSMVarsPtr->tsmDefIMInfoTable[imIndex].gMenuHandle != serviceMenuHandle) {		/* menu has changed				<#33> */
			if (activateP)		utDeleteInputMethodsPreviousMenu( -kTSMSystemMenuID );		/* delete the previous menu		<#35> */
			TSMVarsPtr->tsmDefIMInfoTable[imIndex].gMenuHandle	= serviceMenuHandle;		/* keep in the global too		<#33> */		
		}
		if (activateP) {										/* insert the menu only if 'activate' flag is true	<#7> */
			if ( serviceMenuHandle != nil )
				openedDefIMRecordPtr->openedDefIMMenuVisibleP = utCreateSystemVersionOfMenu(openedDefIMRecordPtr->openedDefIMServiceMenuHdl, TSMVarsPtr);
		}
	}
	else if (cmResult == tsmTSHasNoMenuErr) {					/* if it has no menu, then make sure no left over in the system menu	<#33> */
			utDeleteInputMethodsPreviousMenu( -kTSMSystemMenuID );		/* delete the previous menu	<#33> */
			TSMVarsPtr->tsmDefIMInfoTable[imIndex].gMenuHandle	= nil;	/* no menu now				<#33> */
		 }
	HUnlock((Handle) theDocID);									/* <10Mar92 #10> */
	if (aZone)		(void) utSwapZone(aZone);					/* restore the zone			<#19> */
	return noErr;
}


/*************************************************************************************	*
 * Function: utOpenActivateDefaultIM													*
 * Open the default input method for the current keyscript if it is not already open.	*
 * If the default input method is already open, we return noErr also to signal the		*
 * presence of a default input method. And we will also make sure it is activated.		*
 * If the current script does not need input method, then this routine will return		*
 * tsmScriptHasNoIMErr error.															*
 *																						*
 * Output:	tsmScriptHasNoIMErr = if we are using the old Japanese IM, or				*
 *								  if the current keyscript does not need an IM,			*
 *			tsmInvalidDocIDErr  = if the activeDocID is not a valid ID,					*
 *																						*
 * This function is called by:	NewTSMDocument, SetTextServiceLanguage.					*
 * I assume the default input method has been set up before this call is made.			*
 * •• What to do if we cannot find or cannot open the default input method ??????????	*
 *																						*
 * Modification history:																*
 *	23Jan92	KST	Changed to open Input method for the document. One instance for each	*
 *				open document. Not shared in a global sense.							*
 *	02Mar92	KST	It now takes a Boolean flag to indicate if we should activate the input	*
 *				method. (SWM creates new TSM document but don't want IM activated.)		*
 *				This flag only applies when we want to open, not activate, the IM.		*
 *				The flag will be true except when called from NewTSMDocument by SWM.	*
 *				The flag is passed down to "utOpenIM4Document" and is used there. <#7>	*
 *************************************************************************************	*/
OSErr	utOpenActivateDefaultIM(TSMDocumentID	activeDocID,
								Boolean			activateP)
{
	OSErr					err;
	long					tempLong;
	TSMVarsRec				*TSMVarsPtr = GetExpandMemTSMGlobals();
	DefIMInstanceRecord		*openedDefIMRecordPtr;
	TSMDocRecord			*idocRecPtr;
	Component				aComponent;
	ScriptCode				currentScript;
	ComponentResult			cmResult = 0;

#ifdef TSMDebug

	if (TSMVarsPtr->tsmDebugSignature != kDebugSignature) {
		utTSMDebug(1);
		return -1;
	}
#endif

	currentScript = TSMVarsPtr->tsmCurrentSLRec.fScript;
	
	/* but first check if user is using an old Japanese input method */
	if (currentScript == smJapanese) {								/* only check for Japanese script */
		tempLong = GetScript(smJapanese, kUseOldJapaneseIM);		/* use old script? */
		if (tempLong)		return tsmScriptHasNoIMErr;				/* Yes, so return. */
	}

	if (utScriptIsNotCCJK(currentScript))							/* does the script need IM? */
		return tsmScriptHasNoIMErr;									/* no, only CJK uses IM */


	err = utVerifyTSMDocID(activeDocID);							/* is this a valid TSM ID? */
	if (err != noErr)												/* invalid id */
		return err;

	idocRecPtr  = (TSMDocRecordPtr) *((Handle) activeDocID);		/* dereference the handle */
	
	/* open the default input method of the current script if it is not already opened. */
	/* we might called from "SetTextServiceLanguage", where we just want to activate an input method. */

	openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[utGetScriptIndex(currentScript)]);
	if (openedDefIMRecordPtr->openedDefIMCInstance) {				/* already opened */
		return utActivateIM4Document(activeDocID, currentScript);	/* activate and return */
	}

	/* IM is not open, find the default input method from TSM ... */
	err = xGetDefaultInputMethod(&aComponent, &TSMVarsPtr->tsmCurrentSLRec);
	if (err == noErr) {												/* if a default input method is found */

		err = utOpenIM4Document(activeDocID, aComponent, currentScript, activateP);		/* then open it */

	}
	return err;
}


/* ********************************************************************	*
 * utActivateThisDocsInputMethod:										*
 *		This document was deactivated and is now being activated, if	*
 *		user is still using the same IM, then just activate it;	 else,	*
 *		close the previous Input Method and open the new one.			*
 *		Used in "xActivateTSMDocument" and "xUseInputWindow".			*
 *		Note: We checked that we are not using Old JIM.					*
 * Input:	idocID = document ID										*
 * •• This document could be active and the input method's is not in	*
 *	  the case when user turn UseInputWindow to false for this doc.		*
 * Modification history:												*
 *	10Apr92	KST	When switch from a TSM aware app to a non-TSM aware app	*
 *				and both use the same IM, we didn't inform PM that it	*
 *				should start/stop sending events to SWM.				*
 *				Note that this happens when script and IM are not		*
 *				changed. If they do changed, PM will be notified then.	*
 *	08Jun92	KST	If we open the input method for the document, then we	*
 *				need to check if the app is currently using Input		*
 *				Window, if yes then tell PM start sending events to SWM	*
 * ********************************************************************	*/
 
OSErr	utActivateThisDocsInputMethod(TSMDocumentID	idocID)
{
	short				imIndex;
	OSErr				err;
	Boolean				need2InformPM = false;
	TSMDocRecord		*idocRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	ProcessSerialNumber	PSNRecord;


#ifdef TSMDebug

	if (TSMVarsPtr->tsmDebugSignature != kDebugSignature) {
		utTSMDebug(1);
		return -1;
	}
#endif


	/* Check if the default IM has been switched after this document is deactivated ... *
	 * The keyboard script might have switched as well, but we only care about the IM	*
	 * of the current script. Because all IM should be deactivated at this state.		*/

	imIndex = utGetScriptIndex(TSMVarsPtr->tsmCurrentSLRec.fScript);						/* index for the current script */

#ifdef TSMDebug
	if (TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent == nil) {
		DebugStr("\pNo input method for the current script to activate");
	}
#endif

	idocRecPtr  = (TSMDocRecordPtr) *((Handle) idocID);										/* dereference the handle */
	openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[imIndex]);					/* the default IM we were using */
	if (TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent == openedDefIMRecordPtr->openedDefIMComponent) {	/* IM never switched */
		err = utActivateIM4Document(idocID, TSMVarsPtr->tsmCurrentSLRec.fScript);

		/* Current script is one of CJK, and requires TSM input method then inform			*/
		/* process Manager to start or stop sending events to SWM							*/
		/* We are switching between a TSM aware and a non-TSM aware app using the same IM.	*/
		/* We need to do this because we don't know when a process has swapped in.			*/
		if (err == noErr)		need2InformPM = true;			/* so set the flag */
	}
	else {								/* IM or script switched while we are deactivated. OR we have never opened the IM */
		/* The problem with a TSM aware application in Roman script. Select Akiko and then click in TeachText,	*
		 * now the script is still in Japanese, but you not type into SWM because RouteEvent is still false.... */
		if (openedDefIMRecordPtr->openedDefIMComponent == nil) {
			err =  utOpenIM4Document (idocID, TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent, TSMVarsPtr->tsmCurrentSLRec.fScript, true);
			if (err == noErr)	need2InformPM = true;			/* so set the flag */
		}
		else {
			(void) utCloseIM4Document(idocID, TSMVarsPtr->tsmCurrentSLRec.fScript, false);		/* close the old one if open  */
			err =  utOpenIM4Document (idocID, TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent, TSMVarsPtr->tsmCurrentSLRec.fScript, true);
		}
	}
	
	if ( need2InformPM ) {
		(void) GetCurrentProcess(&PSNRecord);					/* get current process's PSN */
		if (utaIsAppTSMAwareAndNotUsingIW(&PSNRecord) == noErr)	/* if the current application is TSM aware */
			(void) RouteEventToSWM(false);						/* then do not send events to SWM */
		else													/* if the current application is not TSM aware or is using input window */
			(void) RouteEventToSWM(true);						/* then always sending events to SWM */
	}
	
	return err;
}



									/* •• not complete yet (is this necessary?) */
Boolean								/* return true if the script is installed in the System */
utSupportedSL(ScriptLanguageRecord	*slRecPtr)
{	#pragma unused (slRecPtr)

	return true;
}



/* ********************************************************	*
 * utScriptIsNotCCJK:										*
 *	return true if the script has no input method (not CJK)	*
 * 															*
 * Input:	script -- the script to check.					*
 * Output:	true (non-zero) if script is not one of CJK.	*
 * Modification History:									*
 *	Changed the name from utHasNoInputMethod() to new one.	*
 * ********************************************************	*/
Boolean
utScriptIsNotCCJK( ScriptCode	script)
{
	switch (script) {
		case smTradChinese:
		case smSimpChinese:
		case smJapanese:
		case smKorean:				return false;			/* we all need input method */
									break;

		default:					return true;
	}
}


/* ********************************************************	*
 * utScriptIsCCJKAndNeedsIM:								*
 *	return true if the script (CCJK) needs input method. 	*
 * 															*
 * Input:	script -- the script to check.					*
 * Output:	true (non-zero) if script is one of CJK and		*
 *			need TSM-styled input method.					*
 * ********************************************************	*/
Boolean	utScriptIsCCJKAndNeedsIM(ScriptCode		script)
{
	TSMVarsRec	*TSMVarsPtr = GetExpandMemTSMGlobals();

	switch (script) {
		case smTradChinese:
		case smSimpChinese:
		case smKorean:		return true;								/* we all need input method */

		case smJapanese:	if (TSMVarsPtr->tsmUseOldJInputMethodP)		/* are we using the old JIM? */
								return false;							/* yes, so return false */
							return true;								/* else we need input method */

		default:			return false;
	}
}


/* ****************************************************	*
 * utInformPMgr:										*
 *	If Process Manager is active, then inform it if we	*
 *	should start sending events to SWM.					*
 * 														*
 * Called by SWM when it is launched. This is the first	*
 * chance we have to sync'nize the script with PM.		*
 * ****************************************************	*/
void utInformPMgr()
{
	TSMVarsRec	*TSMVarsPtr = GetExpandMemTSMGlobals();
	long		addr1, addr2;


#ifdef TSMDebug

	if (TSMVarsPtr->tsmDebugSignature != kDebugSignature) {
		utTSMDebug(1);
		return -1;
	}
#endif

	if (!TSMVarsPtr->tsmPMgrActiveP) {						/* if PM not active */
		addr1 = GetToolTrapAddress(_Unimplemented);			/* check again */
		addr2 = GetToolTrapAddress(_OSDispatch);
		if (addr1 != addr2)									/* active now. this should only excute once */
			TSMVarsPtr->tsmPMgrActiveP = true;
		else												/* if not, return now */
			return;
	}


	/* Here, PM is activated ....  */
	/* If the system is using old Japanese input method, then don't activate SWM.				<12Jan92 #12> */
	/* If current script is one of CJK, then inform process Manager to start sending events to SWM */

	/* Activate SWM only if we are not using old Japanese Input Method.					--	<31Jan92 #17> */
	if (TSMVarsPtr->tsmCurrentSLRec.fScript == smJapanese) {		/* only check for Japanese script */
		if (GetScript(smJapanese, kUseOldJapaneseIM)) {				/* use old script? */
			(void) RouteEventToSWM(false);							/* don't send events to SWM */
			return;													/* Yes, so return. */
		}
	}
	if (!utScriptIsNotCCJK(TSMVarsPtr->tsmCurrentSLRec.fScript))	/* if current script needs input method */
		(void) RouteEventToSWM(true);								/* then send events to SWM */
	else
		(void) RouteEventToSWM(false);								/* else, don't send events to SWM */

}



/* **************************************************************************************************** *
 * Note: "utDocNeedsInputMethod" does not and can not move memory !										*
 * Return true if the document/application needs input method. (not in any of the following states).	*
 *	1. If the script is not CJK, then it doesn't need input method.										*
 *	2. If the script is Japanese and it's using old JIM, then it doesn't need input method. 			*
 *	3. If the document is using Input Window (locally or globally), then it doesn't need input method.	*
 * **************************************************************************************************** */

Boolean								
utDocNeedsInputMethod(	ScriptCode			script,
						ProcessSerialNumber	*PSNRecordPtr)
{
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();

	if (utaIsAppTSMAwareAndNotUsingIW(PSNRecordPtr) != noErr)
		return false;													/* if using bottom window then return false */

	switch (script) {
		case smTradChinese:
		case smSimpChinese:
		case smKorean:		return true;								/* we all need input method */
							break;
		case smJapanese:	if (TSMVarsPtr->tsmUseOldJInputMethodP)		/* are we using the old JIM? */
								return false;							/* yes, so return false */
							return true;								/* we need input method */

		default:			return false;
	}
}


/* ********************************************	*
 * return array index based on script code.		*
 * Warning: no error checking on the script!	*
 * ********************************************	*/
int
utGetScriptIndex(ScriptCode	script)
{
	//	4 input methods for the following scripts ...
	
	//	smJapanese		= 1,		/* map to 1 => Japanese				*/
	//	smTradChinese	= 2,		/* map to 2 => Traditional Chinese	*/
	//	smKorean		= 3,		/* map to 3 => Korean				*/
	//	smSimpChinese	= 25,		/* map to 0 => Simplified  Chinese	*/

#ifdef BuildTSMInit
	switch (script) {
		case smTradChinese:
		case smSimpChinese:
		case smKorean:
		case smJapanese:			break;				/* these scripts are OK */
		default:					DebugStr("\pIllegal script!");
	}
#endif

	if (script == smSimpChinese)	return 0;
	else							return script;		/* use the script as index */
}



/* ***************************************************************************************	*
 * Search the System resource for the default input method according to script. If we have  *
 * saved the default IM, then return noErr and return the Component description in "cdPtr".	*
 * •• Note: Assume "utScriptIsNotCCJK" has been called before using this function.			*
 *																							*
 * Modification history:																	*
 *	06Feb92	KST		We now save old input method info in the same 'inpm' resource. If the	*
 *					default is old IM, we return a special error - "tsmInputMethodIsOldErr"	*
 *					and return the script code in componentType, FEP ID in componentSubType	*
 * ***************************************************************************************	*/
 
OSErr utGetTSMDefaultInputMethod(	ComponentDescription	*cdPtr,
									ScriptLanguageRecord	*slRecPtr)
{

	DefIMRecordHandle	defIMInfoHandle;
	IMSRecord			*imRecordPtr;
	Handle				defIMResource = nil;
	OSErr				err;
	int					index;


	/* "utScriptIsNotCCJK" has been called by "xGetDefaultInputMethod" */
	index = utGetScriptIndex(slRecPtr->fScript);

	/* get the resource from System file, defined in TSMPrivate.H */
	defIMResource = GetResource( kTSMDefInputMethodType, kTSMDefInputMethodID);
	err = ResError();				/* this will return noErr -- BAD */
	if (err)						return err;
	if (defIMResource == nil)		return resNotFound;


	if (*defIMResource == nil) {										/* if handle is empty */
		LoadResource(defIMResource);
		if (*defIMResource == nil)										/* still cannot load it */
			return resNotFound;
	}

	defIMInfoHandle = (DefIMRecordHandle) defIMResource;
	if ((*defIMInfoHandle)->defRsrcSignature != kINPMresourceSig)		/* is this the right TSM resource? */
		return resNotFound;												/* better stop now */
	
	if ((*defIMInfoHandle)->defIMCount != kSavedDefaultIMCount)			/* if we saved any, then search for it */
		return resNotFound;												/* this might overkill */


	imRecordPtr = &((*defIMInfoHandle)->defIMRecordArray[index]);		/* index set above */
	/* Verify if the record is valid and script is the same */
	if (imRecordPtr->imValidP) {										/* don't check script code		<06Feb92 #23> */
		if (imRecordPtr->imUseOldInputMethodP) {						/* if the default is old one	<06Feb92 #23> */
		
			cdPtr->componentType	= (OSType) slRecPtr->fScript;		/* return script and FEP ID		<06Feb92 #23> */
			cdPtr->componentSubType = (OSType) imRecordPtr->imOldInputMethodFEPID;					 /* <06Feb92 #23> */
			err		= tsmInputMethodIsOldErr;							/* return special error			<06Feb92 #23> */
		}
		else {															/* default is TSM-styled */
			*cdPtr	= imRecordPtr->imCDRec;								/* copy the structure */
			err		= noErr;
		}
	}
	else	err = tsmInputMethodNotFoundErr;							/* no valid info saved */

	
	return err;
}


/* compose Component flag from Script/Language record pointer ... */
 unsigned long
 utSLRec2CompFlag(ScriptLanguageRecord *slRecPtr)
 {
 	unsigned long	flag = slRecPtr->fScript;
	
	return ((flag << 8) + slRecPtr->fLanguage);
 
 }




/* **************************************************************************** *
 * User wants to switch to old input method (for Japanese script only)			*
 * If the current document is using a new JIM, then we need to deactivate it	*
 * This call implies we are switching to Japanese keyscript too .....			*
 *																				*
 * Modification history:														*
 *	24Mar92	KST		Need to call utForceDeactivateIM4Document instead of		*
 *					utDeactivateIM4Document.									*
 * **************************************************************************** */

OSErr utChangeToOldJIM(ScriptIDRecordPtr sidrecptr)
{
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	int					index;
	OSErr				err;
	ScriptCode			oldScript, deactivateScript;
	DefIMRecordHandle	defIMHandle;
	IMSRecord			*imRecordPtr;
	

	/* script is smJapanese, I hope the caller is correct */

	if (sidrecptr->sidScript != smJapanese) {

#ifdef TSMDebug
		DebugStr("\putChangeToOldJIM: Script is not Japanese");
#endif

		return tsmScriptHasNoIMErr;
	}
	
	if (TSMVarsPtr->tsmPMgrActiveP)							/* if PM has booted */
		(void) RouteEventToSWM(false);						/* don't send events to SWM any more */

	TSMVarsPtr->tsmUseOldJInputMethodP = true;				/* set the flag -- <#22> */
	oldScript = TSMVarsPtr->tsmCurrentSLRec.fScript;		/* current script - we might switch from Roman to Japanese */

	/* need to update the resource */
	index = utGetScriptIndex(smJapanese);

	/* Save the resource in System file, as defined in TSMPrivate.H */
	/* first let's see if we have created the resource ... */
	defIMHandle = (DefIMRecordHandle) GetResource( kTSMDefInputMethodType, kTSMDefInputMethodID);

	/* if not, create it now ... */
	if (defIMHandle == nil) {									/* if there is no such resource */
		err = utAddTSMResource(&defIMHandle);					/* then allocate and add one */
		if (defIMHandle == nil)			return err;				/* probably no memory */
	}

	if (*defIMHandle == nil) {									/* if handle is empty */
		LoadResource((Handle) defIMHandle);						/* then load it */
		if (*defIMHandle == nil) {								/* still cannot load it */
			return resNotFound;
		}
	}

	if ((*defIMHandle)->defRsrcSignature != kINPMresourceSig)	/* is this the right TSM IM resource? */
		return resNotFound;										/* better stop now */
	
	/* now we have the resource, update the default */
	imRecordPtr = &((*defIMHandle)->defIMRecordArray[index]);	/* index set above */

	imRecordPtr->imValidP				= true;					/* it's valid now  */
	imRecordPtr->imUseOldInputMethodP	= true;					/* default is old input method */
	imRecordPtr->imOldInputMethodFEPID	= sidrecptr->sidFEPID;	/* this is the FEP id */
	
	HNoPurge((Handle) defIMHandle);								/* make it no purgeable before we write to it */
	ChangedResource((Handle) defIMHandle);						/* save current resfile */
	WriteResource(  (Handle) defIMHandle);						/* write it back to System resource */
	HPurge((Handle) defIMHandle);

	/* *********************** END processing resource file **************************/



	if (TSMVarsPtr->tsmActiveDocID == nil)						/* if there is no active document */
		return noErr;											/* then just bow and quit */

	/* If there is no active document, and the current script is not CJK,	*
	 * then we don't change the current TSM script ...						*/
	TSMVarsPtr->tsmCurrentSLRec.fScript = smJapanese;			/* set current new script */

	/* Here we got an active current document, since old Japanese input method		*
	 * does not need TSM document, so we need to deactivate it and its TSM style IM */
	
//	deactivateScript = kNoSuchScript;
//	if (oldScript != smJapanese) {								/* if we switch from C. or K. to old Japanese IM */
//		if (oldScript==smTradChinese || oldScript==smKorean || oldScript==smSimpChinese)
//			deactivateScript = oldScript;
//	}
//	else	deactivateScript = smJapanese;						/* else we switch from new JIM to old JIM */
	
	if ( utScriptIsNotCCJK(oldScript) )							/* if not one of CJK */
			deactivateScript = kNoSuchScript;
	else
			deactivateScript = oldScript;


	
	
	if (deactivateScript != kNoSuchScript) {					/* if we switch from new (CJK) IM to old Japanese IM */
		utFixDocumentAndCloseSWMWindow(deactivateScript);		/* then deactivate IM of the current doc and close SWM's window */
	}
	return noErr;
}

/* ******************************************************************** *
 * utSuspendResumeApp: Suspend or resume a non-TSM aware application.	*
 *			Used by InformTSM.											*
 *			Called by PM in StartForegroundSwitch (ScrapCoercion.c)		*
 *			We are sure: 1. SWM is around, and							*
 *						 2. The app is not TSM aware.					*
 * Output:	always returns noErr.										*
 * Modification history:												*
 *	09Apr92	KST		New today.											*
 * ******************************************************************** */
OSErr	utSuspendResumeApp( short	flag )
{
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	TSMDocRecord		*idocRecPtr;
	DefIMInstanceRecord	*openedDefIMRecordPtr;
	long				systemScript;
	short				imIndex;
	char				handleState;
	ScriptCode			currentScript;
	TSMDocumentID		currentTSMActiveDocID;
	
	
	systemScript = GetEnvirons(smSysScript);
	currentTSMActiveDocID = TSMVarsPtr->tsmActiveDocID;
	
	if ( currentTSMActiveDocID ) {			/* if we have current active doc -- must be SWM's */
		currentScript = TSMVarsPtr->tsmCurrentSLRec.fScript;

		handleState = HGetState( (Handle) currentTSMActiveDocID );				/* save the handle state		<#27> */
		HLock( (Handle) currentTSMActiveDocID );
		idocRecPtr  = (TSMDocRecordPtr) *((Handle) currentTSMActiveDocID );		/* dereference the current ID */
		
		if (utScriptIsNotCCJK(currentScript)) {									/* if the script has no input method */
			if (idocRecPtr->iDocActiveP) {										/* if it is active */
				/* We take the advantage that xDeactivateTSMDocument() will check and deactivate	*
				 * each input method when it is called while the document is not active !!!			*/
				idocRecPtr->iDocActiveP = false;								/* like I said above*/
				(void) xDeactivateTSMDocument( currentTSMActiveDocID );			/* ignore error */
			}
			HSetState( (Handle) currentTSMActiveDocID, handleState);			/* restore the handle state		<#27> */
			return noErr;
		}


		imIndex = utGetScriptIndex( currentScript );							/* index for the last  script */
		openedDefIMRecordPtr = &(idocRecPtr->iDocDefIMInstanceArray[imIndex]);	/* the default IM we were using */

		if (flag == kTSMSuspend) {
			/* This application is not TSM aware. But it can be because of 2 reasons:	*
			 *	a. The app is really non-TSM aware,										*
			 *	b. The app is TSM aware but is using Input Window.						*
			 *	Case a: Need to deactivate the document, otherwise the doc is active.	*
			 *	Case b: Don't do anything because the app should make deactivate call.	*
			 *
			 *	
			 *	TSM aware		Roman				DeactivateTSMDoc, SetTextServiceLanguage
			 *	non-TSM			non-TSM		CJK		Leave SWM document active, don't take down its window and menu.
			 *										Script is not changed.
			 *	non-TSM			switch to	Roman	utSuspendResumeApp, take down its window and menu from app's menulist.
			 *								Finder	If you don't take down the menu and switch from Roman back to the app,
			 *										the pencil shows up in Roman script. Document has to be deactivated,
			 *										otherwise, pencil will not show up in the menu bar when switch to Akiko.
			 *										And need to take down SWM's window as well.
			 *	non-TSM			quit to		Roman	SetTextServiceLanguage, since there is no utSuspendResumeApp call,
			 *								Finder	the active document is non-nil. SetTextServiceLanguage will clean up
			 *										palette, and SWM's window. (Note, Finder doesn't have IM's menu.
			 */

			if (currentTSMActiveDocID == TSMVarsPtr->tsmUseInputWindowDocID) {	/* app is not TSM aware, and this must be SWM's doc */
			
				//if ( utDocNeedsInputMethod(currentScript, &idocRecPtr->iDocPSNRecord) ) {
					//(void) xDeactivateTSMDocument( currentTSMActiveDocID );	/* ignore error */
					//(void) utSoftDeactivateIM4Document(currentTSMActiveDocID, currentScript );
					// utJustDeactivateIM4Document( TSMVarsPtr->tsmActiveDocID, TSMVarsPtr->tsmCurrentSLRec.fScript, false );
					//TSMVarsPtr->tsmActiveDocID			= nil;
					//TSMVarsPtr->tsmUseInputWindowDocID	= nil;
					//TSMVarsPtr->tsmActiveTSCount		= 0;					/* no active TS	*/
				//}
			}

			if (systemScript != currentScript &&								/* only take down menu when they don't match */
				utScriptIsNotCCJK((ScriptCode) systemScript)) {					/* and the system script is not CJK */
				/* If the system script is CJK, then we don't take down the menu because there will be	*
				 * an Input method and therefore a menu, What if no menu?? So we keep the menu and keep	*
				 * SWM's document active. If we switch to non-TSM aware application, then SWM needs to	*
				 * be active anyway. If we switch to a TSM aware application, then that application will*
				 * have to activate a document, we will deactivate SWM's document and clean up menu and	*
				 * synchronize everything when that magic moment happens.								*/

				if (currentTSMActiveDocID == TSMVarsPtr->tsmUseInputWindowDocID) {	/* app is not TSM aware, and this must be SWM's doc */
					// can't fix text, because when this is called, the app is being suspended,
					// if we fix text, then text will be posted to the next application.
					//(void) xFixTSMDocument( currentTSMActiveDocID );				/* fix the current input area */
					(void) xDeactivateTSMDocument( currentTSMActiveDocID );			/* ignore error */
					utCloseSWMWindow(false);										/* close window if visible */
					if (openedDefIMRecordPtr->openedDefIMServiceMenuHdl ) {			/* if IM has a menu */
						/* remove IM's menu from the application menulist so it will not reappear again .....	*
						 * and remove it from the system menu too so it will not appear in other applications	*
						 * TSM aware app will not go thru this path, but the menu will be removed when			*
						 * SetTextServiceLanguage is called when we switch back to Finder.						*/
						utDeleteMenu((*(openedDefIMRecordPtr->openedDefIMServiceMenuHdl))->menuID);
						openedDefIMRecordPtr->openedDefIMMenuVisibleP = false;		/* not visible */
					}
				}
				else	/* use input window case */
					if (openedDefIMRecordPtr->openedDefIMServiceMenuHdl ) {			/* if IM has a menu */
						/* remove IM's menu from the application menulist so it will not reappear again .....	*
						 * and remove it from the system menu too so it will not appear in other applications	*
						 * TSM aware app will not go thru this path, but the menu will be removed when			*
						 * SetTextServiceLanguage is called when we switch back to Finder.						*/
						utDeleteSysMenu((*(openedDefIMRecordPtr->openedDefIMServiceMenuHdl))->menuID, systemScript);
						openedDefIMRecordPtr->openedDefIMMenuVisibleP = false;		/* not visible */
					}

			}
		}
		else {		/* not used for now */
			if (systemScript == TSMVarsPtr->tsmCurrentSLRec.fScript) {			/* only add menu when they match */
				if (openedDefIMRecordPtr->openedDefIMServiceMenuHdl ) {			/* if the IM has menu */
					InsertMenu(openedDefIMRecordPtr->openedDefIMServiceMenuHdl, 0);
					DrawMenuBar();
					//openedDefIMRecordPtr->openedDefIMMenuVisibleP = true;		/* menu is visible */
				}
			}
		}
		
		HSetState( (Handle) currentTSMActiveDocID, handleState);				/* restore the handle state		<#27> */
	}
	return noErr;
}


/* ******************************************************************************** *
 * utAddTSMResource: allocate and add a resource to system resource file for TSM.	*
 * Note: we only add the resource without writing it to disk !!						*
 * Modification history:															*
 *	29Jan92	KST	Set sysheap and purgeable bits.										*
 * ******************************************************************************** */

OSErr utAddTSMResource( DefIMRecordHandle	*theHandlePtr )
{
	short				curResfile;
	DefIMRecordHandle	defIMHandle;
	IMSRecord			*imRecordPtr;

	*theHandlePtr = nil;										/* default is nil */
	defIMHandle = (DefIMRecordHandle) NewHandleClear(sizeof(DefIMRecord));
	if (defIMHandle == nil)				return memFullErr;		/* no memory */
	
	(*defIMHandle)->defRsrcSignature	= kINPMresourceSig;		/* TSM resource */
	(*defIMHandle)->defUseFloatWindow	= false;				/* don't use floating input window */
	(*defIMHandle)->defIMCount			= kSavedDefaultIMCount;	/* size of the array */


	//	4 input methods for the following scripts ...
	
	//	smJapanese = 1,					/* Japanese */
	//	smTradChinese = 2,				/* Traditional Chinese */
	//	smKorean = 3,					/* Korean   */
	//	smSimpChinese = 25,				/* Simplified Chinese */

 
	imRecordPtr = &((*defIMHandle)->defIMRecordArray[0]);	/* 0 = Simplified Chinese */
	imRecordPtr->imValidP				= false;			/* not valid until we set it */
	imRecordPtr->imUseOldInputMethodP	= false;			/* default is to use TSM-style input method */
	imRecordPtr->imOldInputMethodFEPID	= 0;				/* FEP id = 0 */

	imRecordPtr = &((*defIMHandle)->defIMRecordArray[1]);	/* 1 = Japanese */
	imRecordPtr->imValidP				= false;			/* not valid until we set it */
	imRecordPtr->imUseOldInputMethodP	= false;			/* default is to use TSM-style input method */
	imRecordPtr->imOldInputMethodFEPID	= 0;				/* FEP id = 0 */

	imRecordPtr = &((*defIMHandle)->defIMRecordArray[2]);	/* 2 = Traditional Chinese */
	imRecordPtr->imValidP				= false;			/* not valid until we set it */
	imRecordPtr->imUseOldInputMethodP	= false;			/* default is to use TSM-style input method */
	imRecordPtr->imOldInputMethodFEPID	= 0;				/* FEP id = 0 */

	imRecordPtr = &((*defIMHandle)->defIMRecordArray[3]);	/* 3 = Korean */
	imRecordPtr->imValidP				= false;			/* not valid until we set it */
	imRecordPtr->imUseOldInputMethodP	= false;			/* default is to use TSM-style input method */
	imRecordPtr->imOldInputMethodFEPID	= 0;				/* FEP id = 0 */
	
	curResfile = CurResFile();								/* save current resfile */
	UseResFile(0);											/* use System resource  */
	AddResource((Handle) defIMHandle, kTSMDefInputMethodType, kTSMDefInputMethodID, "\pTSM Resource");
	SetResAttrs((Handle) defIMHandle, resSysHeap+resPurgeable);
	UseResFile(curResfile);
	
	*theHandlePtr = defIMHandle;							/* return the handle */
	return noErr;
}


/* ******************************************************************************** *
 * utUpdateDefaultInputMethodResource:												*
 *		Update the default input method in the system resource for the script.		*
 *		Only used when we switch between TSM-styled input methods. That's why I		*
 *		always set imUseOldInputMethodP to false.									*
 * Note: We don't open or close any instances ...									*
 * Side effect: The resource will be changed in the System file.					*
 * ******************************************************************************** */

OSErr utUpdateDefaultInputMethodResource (	Component				aComp,
											ScriptLanguageRecord	*slRecPtr)
{
	OSErr					err;
	int						index;
	ComponentDescription	cdRec;
	DefIMRecordHandle		defIMHandle;
	IMSRecord				*imRecordPtr;
	TSMVarsRec				*TSMVarsPtr = GetExpandMemTSMGlobals();


	/* see if there is such an Input Method supports the script/language */

	err = GetComponentInfo(aComp, &cdRec, nil, nil, nil);		/* fill in CD */
	if (err)				return err;
	
	if (utScriptIsNotCCJK(slRecPtr->fScript))					/* does the script needs IM? */
		return tsmUnknownErr;									/* only CJK uses IM */
	
	index = utGetScriptIndex(slRecPtr->fScript);

	/* Save the resource in System file, as defined in TSMPrivate.H */
	/* first let's see if we have created the resource ... */
	defIMHandle = (DefIMRecordHandle) GetResource( kTSMDefInputMethodType, kTSMDefInputMethodID);

	/* if not, create it now ... */
	if (defIMHandle == nil) {									/* if there is no such resource */
		err = utAddTSMResource(&defIMHandle);					/* then allocate and add one */
		if (defIMHandle == nil)			return err;				/* probably no memory */
	}

	if (*defIMHandle == nil) {									/* if handle is empty */
		LoadResource((Handle) defIMHandle);						/* then load it */
		if (*defIMHandle == nil) {								/* still cannot load it */
			return resNotFound;
		}
	}

	if ((*defIMHandle)->defRsrcSignature != kINPMresourceSig)	/* is this the right TSM resource? */
		return resNotFound;										/* better stop now */

	TSMVarsPtr->tsmDefIMInfoTable[index].gComponent = aComp;	/* update the in memory copy */
	
	/* now we have the resource, update the default */
	imRecordPtr = &((*defIMHandle)->defIMRecordArray[index]);	/* index set above */

	imRecordPtr->imValidP		= true;							/* it's valid now  */
	imRecordPtr->imUseOldInputMethodP  = false;					/* set to false */
	imRecordPtr->imOldInputMethodFEPID = 0;
	imRecordPtr->imLanguage		= slRecPtr->fLanguage;			/* language */
	imRecordPtr->imCDRec		= cdRec;						/* can we copy the structure? -> Yes ! */
	
	HNoPurge((Handle) defIMHandle);								/* make it no purgeable before we write to it */
	ChangedResource((Handle) defIMHandle);						/* save current resfile */
	WriteResource(  (Handle) defIMHandle);						/* write it back System resource */
	HPurge((Handle) defIMHandle);
	err = ResError();
	

}



/* **************************************************************************** *
 * Function: utFixDocumentAndCloseSWMWindow:								 	*
 *			 If there IS a current active TSM document, then:					*
 *				1. fix the input area,											*
 *				2. Force deactivate the input method, and						*
 *				3. If this document belongs to SWM, then						*
 *				   a. close SWM's input window by sending Apple Event to SWM,	*
 *				   b. and then deactivate SWM's document. (SWM's document has	*
 *				      to be deactivated because it has been decided that SWM's	*
 *					  window should go away. It will be activated again when	*
 *					  user starts typing.										*
 *			•• Note that we don't deactivate TSM aware app's document.			*
 * Side effect: Text in the inline hole will be lost.							*
 * Called by "xActivateTSMDocument", and "xSetTextServiceLanguage".				*
 *																				*
 * Modification history:														*
 *	24Mar92	KST	Since we have 2 active documents now, need to be careful as		*
 *				which document to deactivate here.								*
 *				If tsmActiveDocID == tsmUseInputWindowDocID == SWM's doc, then	*
 *				deactivate SWM's doc. Otherwise we make tsmActiveDocID equals	*
 *				to tsmUseInputWindowDocID because when we return from this code,*
 *				xSetTextServiceLanguage will open the new IM for this TSM aware	*
 *				application's document based on tsmActiveDocID.					*
 *	31Jul92	KST	Do not fix TSMDocument since the A5 world might not be set up	*
 *				correctly in a keydown (cmd-space) event.						*
 * **************************************************************************** */
void utFixDocumentAndCloseSWMWindow(ScriptCode	deactivateScript)
{
	TSMVarsRec		*TSMVarsPtr = GetExpandMemTSMGlobals();
	TSMDocRecord	*currentActiveDocPtr;
	TSMDocumentID	currentTSMActiveDocID;


	currentTSMActiveDocID = TSMVarsPtr->tsmActiveDocID;
	if ( currentTSMActiveDocID ) {					/* if currently has active document		<01Feb92 #21> */

		currentActiveDocPtr = (TSMDocRecord *) *((Handle) currentTSMActiveDocID);		/*  <04Mar92 #28> */

		if (currentActiveDocPtr->iDocPSNRecord == TSMVarsPtr->tsmSWMsPSNRecord) {		/* doc belongs to SWM	<#28> */
			/* •• must call DeactivateTSMDocument and utCloseSWMWindow before we change tsmCurrentSLRec *
			 * •• because DeactivateTSMDocument relies on current script and current IM are in sync !	*
			 * •• also, I'll have to call DeactivateTSMDocument here, because when SWM receive the AE,	*
			 * •• it is after xSetTextServiceLanguage routine already been executed ...					*/

			(void) xFixTSMDocument( currentTSMActiveDocID );			// Akiko crashes if we do this??? works now <09Jun92 #26>
			
			/* <4/5/92>	John, we have a problem here. utForceDeactivateIM4Document will remove IM's menu
			 *			and hide the Palette (utHidePaletteAndMenu). However, it might be too late to
			 *			remove the menu in the following situation:
			 *			Use Roman as the primary script. Launch any non-TSM aware application then select Akiko, 
			 *			switch to finder and switch back to that application. Notice that Akiko's menu stays.
			 *
			 * <4/6/92>	Hmmm, I think we'll have the same problem in trying to fix the input area as well.
			 *			By the time we do all these, the application already swapped out. And Akiko crashes.
			 *
			 *	The solution is to send "suspend" event to SWM and SWM call DeactivateTSMDocument4SWM.
			 */
			
			(void) utForceDeactivateIM4Document( currentTSMActiveDocID, deactivateScript);	/* need to deactivate the IM */
			if ( currentTSMActiveDocID == TSMVarsPtr->tsmUseInputWindowDocID ) {			/* SWM is the sole player */
				xDeactivateTSMDocument( currentTSMActiveDocID );							/* tsmActiveDocID will be cleared ....  */
			}
			else {																			/* if using Input Window */
				TSMVarsPtr->tsmActiveDocID = TSMVarsPtr->tsmUseInputWindowDocID;			/* they must be match */
				currentActiveDocPtr = (TSMDocRecord *) *((Handle) currentTSMActiveDocID);	/* dereference <09Jun92 #26> */
				currentActiveDocPtr->iDocActiveP = false;									/* not active  <09Jun92 #26> */
			}
			
			/* SWM has to call xFixTSMDocument, and DeactivateTSMDocument in its own context */
			/* remove DeactivateTSMDocument when we switch SWM to a driver, and add fixText! */
			utCloseSWMWindow(false);								/* ask SWM to close her input window	<04Mar92 #28> */
		}			/* end if */
		else {		/* if not SWM, then I can call xFixTSMDocument in TSM aware application's context */
			(void) xFixTSMDocument( currentTSMActiveDocID );		/* fix current input area ...			<18Mar92 #30> */
			(void) utForceDeactivateIM4Document( currentTSMActiveDocID, deactivateScript );
		}
	}
}


/* **************************************************************************** *
 * Function: utCloseSWMWindow:											  <#5>	*
 *			 Close SWM's input window by sending Apple Event to SWM.			*
 * Side effect: Text in the inline hole will be lost.							*
 * Called by "xActivateTSMDocument", and "xSetTextServiceLanguage".				*
 * Modification History:														*
 *	25Jun92	KST	Added one new param FlushBufferP to SWM. true = flush TE buffer	*
 * **************************************************************************** */
void utCloseSWMWindow(Boolean	FlushBufferP)
{
	ProcessSerialNumber	PSNRecord;
	OSErr				err;
	AEAddressDesc		addrDescriptor;
	AppleEvent			theAEvent, theReply;
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	#pragma unused (theReply,theAEvent,addrDescriptor,err,PSNRecord)

#ifdef TSMDebug
	if (TSMVarsPtr->tsmDebugSignature != kDebugSignature) {
		utTSMDebug(1);
		return;
	}
#endif


	if (TSMVarsPtr->tsmSWMsPSNRecord.highLongOfPSN==0 && TSMVarsPtr->tsmSWMsPSNRecord.lowLongOfPSN==0)
		return;										/* return if no PSN */

	(void) TSMVarsPtr->tsmCloseSWMWindowProc( TSMVarsPtr->tsmSWMDrvrRefNum, FlushBufferP );

}


/* ************************************************************************ *
 * Remove the menu from menu bar only if tsmKillApplicationP flag is false.	*
 * If tsmKillApplicationP flag is set, then the menu is already killed by	*
 * Process Manager, so don't do "DeleteMenu" !! (trust me).					*
 *																			*
 * 27Feb92	KST	New today.													*
 * 27Feb92	KST	Need to remove IM's menu from the system menulist if we		*
 *				are running in a non-CJK primary system.					*
 * ************************************************************************ */
void utDeleteMenu(short menuID)
{
	TSMVarsRec	*TSMVarsPtr = GetExpandMemTSMGlobals();
	long		theMenuList;
	long		theSystemMenuList = ( *(long *)SystemMenuList );
	long		systemLong;


#ifdef TSMDebug

	if (TSMVarsPtr->tsmDebugSignature != kDebugSignature) {
		utTSMDebug(1);
		return;
	}

#endif


	if (!TSMVarsPtr->tsmKillApplicationP) {							/* if app is not exiting to shell */
		DeleteMenu(menuID);
		if (menuID == -kTSMSystemMenuID) {							/* if this is a system menu */
			systemLong = GetEnvirons(smSysScript);					/* need to remove it for non-CJK system */
			if (!utScriptIsNotCCJK((ScriptCode) systemLong)) {		/* if current script needs input method */
				if (systemLong == smJapanese) {						/* special case for Japanese */
					if ( !TSMVarsPtr->tsmUseOldJInputMethodP )		/* if not use old JIM */
						return;										/* return right now   */
				}
				else return;										/* leave the menu in system menu */
			}
			if ( theSystemMenuList != nil ) {						/* change menulist to be the systemmenulist */
				theMenuList = ( *(long *) ApplicationMenuList );	/* save app's menulist */
				( *(long *) ApplicationMenuList ) = theSystemMenuList;
				DeleteMenu(menuID);
				( *(long *) ApplicationMenuList ) = theMenuList;	/* restore */
			}
		}
		InvalMenuBar();
	}
}


void utDeleteAppsMenu(short menuID)
{
	TSMVarsRec	*TSMVarsPtr = GetExpandMemTSMGlobals();

	if (!TSMVarsPtr->tsmKillApplicationP) {							/* if app is not exiting to shell */
		DeleteMenu(menuID);											/* delete it from the app's menulist */
		InvalMenuBar();
	}
}


/* ***********************************************************************	*
 * remove the menu from the system menulist, so that it will not show up	*
 * in other application's menu bar. The menu is not removed from the		*
 * current application's menulist. So it always stays with the application.	*
 * Useful when switch from a TSM aware application to Finder, we need to	*
 * keep the pencil in that app's menu but we don't want to see it when we	*
 * launch other applications. Called by 'utSuspendResumeApp'.				*
 * Note: we only remove the menu if the script is one of CCJK.				*
 * ***********************************************************************	*/
void utDeleteSysMenu(short menuID,	long	systemScript)
{
	TSMVarsRec	*TSMVarsPtr = GetExpandMemTSMGlobals();
	long		theMenuList;
	long		theSystemMenuList = ( *(long *)SystemMenuList );
	

	if (!TSMVarsPtr->tsmKillApplicationP) {							/* if app is not exiting to shell */
		if (menuID == -kTSMSystemMenuID) {							/* if this is a system menu */
			if (!utScriptIsNotCCJK((ScriptCode) systemScript)) {	/* if current script needs input method */
				if (systemScript == smJapanese) {					/* special case for Japanese */
					if ( !TSMVarsPtr->tsmUseOldJInputMethodP )		/* if not using old JIM */
						return;										/* return right now   */
				}
				else return;										/* leave the menu in system menu */
			}
			if ( theSystemMenuList != nil ) {						/* change menulist to be the systemmenulist */
				theMenuList = ( *(long *) ApplicationMenuList );	/* save app's menulist */
				( *(long *) ApplicationMenuList ) = theSystemMenuList;
				DeleteMenu(menuID);
				( *(long *) ApplicationMenuList ) = theMenuList;	/* restore */
			}
		}
	}
}


/* *********************************************************************** *
 * set current zone to theZone, and return the current zone before setting *
 * *********************************************************************** */
THz utSwapZone( register THz theZone )

{
	register THz	savedZone;
		
	savedZone = GetZone( );
	SetZone( theZone );
	
	return( savedZone );

}






#ifdef TSMDebug

void utTSMDebug(short whocalled)
{
	switch (whocalled) {
	case 1:
		DebugStr("\pNot TSM globals");
		break;
	default:
	}
}

#endif