/*
	File:		TSMUtility.c

	Contains:	Text Services Manager Utility routines.

	Written by:	Kenny SC. Tung & Kurita san.

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<60>	10/30/92	DTY		Use Get/Set macros to access ExpandMem.
		<59>	  8/4/92	KST		#1038311 <gbm>: <HA>: Undo the previous fix and put the fix in
									ScriptMgr:ScriptMgrKeyGetSet.a
		<58>	  8/1/92	KST		#1038311 <JH>: Documentation change.
		<57>	  8/1/92	KST		#1038311 <JH>: Can't call FixTSMDocument() to the app. in the
									SetDefaultInputMethod() either.
		<56>	 7/31/92	KST		#1038311 <JH>: In xCloseServiceWindow(), make sure the window
									that we are going to close is not nil and it is truely a
									floating window.
		<55>	 7/27/92	KST		#1037539 <JH>: Return tsmScriptHasNoIMErr instead of
									tsmUnknownErr in SetDefaultInputMethod() and
									GetDefaultInputMethod() to match the ERS.
		<54>	  7/8/92	KST		#1033475 <JH>: In SetTextServiceLanguage(), when we activate an
									input method, make sure we are not using old JIM.
		<53>	  7/6/92	KST		#1033507,1033505 <JH>: Don't change the tsmScript/Language if
									utOpenIM4Document() failed.
		<52>	 6/30/92	KST		#1030903,1033475 <smb>: We had an agreement with the keyboard
									menu code that TSM will call SetScript(false) in
									SetDefaultInputMethod().
		<51>	 6/30/92	KST		#1033456 <JH>: If utJustDeactivateIM4Document() returns
									DocNotActiveErr, then ignore the error.
		<50>	 6/26/92	KST		#1033450,1033456,1031316 <JH>: utCloseIM4Document() will now
									take an extra parameter to indicate if it should send
									HidePallete() command to input method. Only set it to true when
									called in SetDefaultInputMethod().
		<49>	 6/22/92	KST		#1030843 <JH>: When there is no current active document, and
									switch from old JIM to TSM-styled input method, need to reset
									UseOldJIM flag.
		<48>	 6/17/92	KST		#1030843 <JH>: For the previous fix, in xUseInputWindow, there
									is a possibility that the current active document could be nil.
									Need to check that.
		<47>	 6/17/92	KST		#1030843 <JH>: Changed the function name from
									utaIsApplicationTSMAware to utaIsAppTSMAwareAndNotUsingIW to be
									more precise and descriptive.
		<46>	 6/14/92	KST		#1032466 <JH>: Fixed the bug when running a TSM aware
									application in Roman primary script, select SJip and then Akiko,
									Akiko is not activated.
		<45>	 6/13/92	KST		#1031142,1030881 <JH>: When user turns UseInputWindow off and
									the current active document is SWM's, then need to call
									FixTSMDocument before deactivating that document.
		<44>	 6/10/92	YK		#1031298 <JH>: Make sure that the default input method exists.
		<43>	 6/10/92	KST		#1031142,1030881 <JH>: Since we removed the docID param from
									TSMEvent, UseInputWindow has been broken because keydown event
									is routed to SWM recursively. Fix it with a TSM global keeping
									track of which doc is using the input window. This involved many
									many changes.
		<42>	 6/10/92	KST		#1031634, #1031924 <JH>: Fixed the bug -- "Balloon Help is broken
									causing the system to crash as menus are pulled." in FindServiceWindow.
		<41>	  6/2/92	JH		#1028635,1030481,1030631,1030634,1030647 <KST>: Adding a check
									in FindServiceWindow for a nil menulist handle in low memory.
									FindServiceWindow gets called a lot now by input methods at odd
									times, and if the menulist low-mem has nil in it (yes, it can)
									the normal FindWindow call crashes trying to jump to an MDEF
									proc referenced off of nil. So we check here. If it is nil we
									just return indesk.
		<40>	 5/20/92	KST		Ooops, I added a comment which had syntax error.
		<39>	 5/20/92	KST		#1030422 <JH>: Fixed a bug in SetDefaultInputMethod.
		<38>	 5/19/92	KST		#1029247 <hsK>: GetDefaultInputMethod, SetDefaultInputMethod,
									and SetTextServiceLanguage routines should check if user is
									passing the correct language code corresponding to the script
									code.
		<37>	 5/13/92	JH		<KST>: Fixing NewServiceWindow so that it marks floating windows
									with the proper windowkind.
		<36>	  5/8/92	KST		When we switch from non-CJK script to TSM-styled IM, we must
									clear the ScriptMgr's secret "UseOldJapaneseIM" flag.
		<35>	  5/6/92	KST		#1028642,<JH>: Fixed the bug: "floating pallete stays when
									switch from Akiko to Roman in TSM aware application".
		<34>	  5/1/92	KST		#1028301,<JH>: SetTSMDialogState, RestoreTSMDialogState,
									TSMChangeCursor, TSMRestoreCursor calls are no longer needed and
									should be removed from the source code.
		<33>	  4/9/92	KST		JH,modifying xNewServiceWindow to mark a falg in refcon. Adding
									stuff to support SWM as a driver. We don't need
									xSetTSMDialogState and xRestoreTSMDialogState call anymore. Will
									remove them in the next release.
		<32>	 3/30/92	KST		JH, 1025008, 1025009. First attempt to solve the pallete/menu
									bar flickering problem. Also, we synchronize the keyboard menu
									with the script of the TSM document.
		<31>	 3/23/92	KST		Code changed according to the comments from the code review.
		<30>	 3/18/92	KST		In "xSetTextServiceLanguage", if user switches key script, then
									I'll call FixTSMDocument.
		<29>	 3/12/92	KST		Make sure handles are locked properly.
		<28>	  3/6/92	KST		In "xSetTextServiceLanguage", when user switches input method,
									if there is currently an active document which belongs to SWM,
									then send an AE to ask her to close the input window.
		<27>	  3/2/92	KST		In "utOpenActivateDefaultIM" and "utOpenIM4Document", it now
									takes a Boolean flag to indicate if we should activate the input
									method.
		<26>	 2/28/92	KST		Added 2 new calls "SetTSMDialogState", and
									"RestoreTSMDialogState".
		<25>	 2/11/92	DCL		Changed the name of TSMEqu.[aph] to TextServices.[aph] for
									better read-ability.
		<24>	 2/10/92	KST		Moved all internal routines to TSMInternalUtil.c file.
		<23>	 2/10/92	KST		Change the default input method resource format, we don't store
									script code now. Script is implied by array index. Instead, we
									store the old IM FEP ID in the resource record.
		<22>	  2/3/92	KST		If we switch from old to new input method, need to check if we
									are coming from old JIM and need to inform Process Manager.
		<21>	  2/1/92	KST		If there is no active document, we still need to inform PM
									whether to use SWM or not.
		<20>	  2/1/92	KST		We don't have a global flag in TSM globals to indicate using
									bottom-line window.
		<19>	 1/31/92	YK		with KST: Added xFindServiceWindow.
		<18>	 1/31/92	KST		Update the input method table in the TSM global in
									"xGetDefaultInputMethod".
		<17>	 1/31/92	KST		Activate SWM only if we are not using old Japanese Input Method.
		<16>	 1/29/92	KST		Input method synchronized with the script/language only when the
									correct environment has been set up in SetTextServiceLanguage.
		<15>	 1/16/92	KST		When we search for a default input method, need to set the mask
									field to search for the correct script.
		<14>	 1/15/92	KST		GetDefaultInputMethod should not open the input method.
		<13>	 1/13/92	YK		Put comments. Call GetToolTrapAddress instead of GetTrapAddress.
		<12>	 1/13/92	KST		Activate SWM only if we can open the default input method.
		<11>	 1/11/92	KST		Call Process Manager's "RouteEventToSWM" routine only if 
									P.M. has been loaded.
		<10>	 1/11/92	KST		Adding code to support SWM Background application.
		 <9>	 1/10/92	KST		Call "LoadResource" if the resource handle points to nil.
		 <8>	  1/9/92	KST		In "UseInputWindow", If idocID ==  0, it affects only one (••
									current ••) application, not the whole system.
		 <7>	  1/8/92	KST		In xGetDefaultInputMethod, if the default input method is not
									loaded in the system, another input method is opened by query
									Component Manager.
		 <6>	12/31/91	KST		We now save default input method and "UseInputWindow" flag in
									resource file across reboot. A new scheme is adopted to allow a
									single document to use input window.
		 <5>	12/20/91	KST		Making progress toward Alpha. Changed "inline" to "TSM".
		 <4>	12/10/91	KST		Adding new code for the D5 build.
		 <3>	11/26/91	YK		Added code for the floating window.
		 <2>	11/23/91	KST		Check in the first time for Cube-E.

*/



#include	<Types.h>
#include	<Memory.h>
#include	<Windows.h>
#include	<Layers.h>
#include	<Script.h>
#include	<Resources.h>
#include	<Traps.h>

#include	<MFPrivate.h>
#include	<TextServices.h>				/*25*/
#include	<TSMPrivate.h>


/*	********************************************************************************
 *	File:	"TSMUtility.c"
 *
 *	External routines:
 *		xSetDefaultInputMethod
 *		xGetDefaultInputMethod
 *		xSetTextServiceLanguage
 *		xGetTextServiceLanguage
 *		xUseInputWindow
 *		xNewServiceWindow
 *		xCloseServiceWindow
 *		xGetFrontServiceWindow
 *		xFindServiceWindow
 *		
 *	******************************************************************************** */

/*
Note: ••	The "TSMVarsPtr->tsmUseOldJInputMethodP" flag is meaningful only when the current
	  ••	script is Japanese. This means only test/change the flag in Japanese script !!
	  
	  ••	Thru out this and other files, I used 'Akiko' to refer to a TSM styled new Input Method.
*/

/*
 *	returns true if the old Input Method whose ID is fepID is installed.  (implememted in TSMDispatch.a)
 */
extern pascal Boolean OldJapaneseInputMethodExists( short fepID);




/*	***************************************************************************	*
 *	Function:	xSetDefaultInputMethod											*
 *				Set the default Input Method (saved across reboot).				*
 *				Called when user changes input method from the keyboard menu	*
 *				or by application.												*
 *				Assume we'll not be called if user switches to old Japanese IM.	*
 *																				*
 *	Input:		Pointer to an Input Method component,							*
 *				Pointer to a ScriptLanguageRecord buffer.						*
 *	Output:		noErr = if successful,											*
 *				tsmUnsupScriptLanguageErr = if no IM supports that script.		*
 *	Side effect:The (new) Default Input Method is opened in here.				*
 *				And the change is write to disk (in system resource file).		*
 *	Note:		In this routine, we do not change tsmCurrentSLRec.fScript !		*
 *																				*
 *	Modification History:														*
 *	10Jan92	KST	When we switch from one default input method to another in the	*
 *				same script, the old one is closed and the new one is opened.	*
 *	23Jan92	KST	When we switch input method, we might switch the keyscript too.	*
 *	24Mar92	KST	Need to call utForceDeactivateIM4Document instead of			*
 *				utDeactivateIM4Document.										*
 *	26Mar92	KST	We now keep track of the script used by this document.			*
 *				If we switch script, then change the script information for		*
 *				the current active document.									*
 *	19May92	KST	We should check if user is passing the correct language code	*
 *				corresponding to the script code. If not, return paramErr.<#38>	*
 *	19May92	KST	We had an agreement with the keyboard menu code that TSM will	*
 *				call SetScript(kUseOldJapaneseIM,false).						*
 *	01Aug92	KST	When we switch from CIM1 to CIM2 to Akiko and back to CIM1, TSM	*
 *				still keeps the menuhandle of CIM2. Need to get the menu from	*
 *				CIM1 by just clearing the menuHandle in TSM globals. Will get	*
 *				the menu from CIM1 in utOpenIM4Document().						*
 *	***************************************************************************	*/
pascal OSErr xSetDefaultInputMethod ( Component aComp, ScriptLanguageRecord *slRecPtr )
{

	OSErr					err = noErr;
	Boolean					need2UpdateDefIMp = false;			/* false = do not need to update default I.M. setting */
	Boolean					need2UpdateScript = false;			/* false = do not need to update current key script */
	short					imIndex;
	ComponentDescription	cdRec;
	ScriptCode				newScript, currentScript;
	TSMVarsRec				*TSMVarsPtr = GetExpandMemTSMGlobals();
	//TSMDocRecord			*currentActiveDocPtr;
	MenuHandle				serviceMenuHandle = nil;
	TSMDocumentID			currentTSMDocument;
	Boolean					forceUpdateResource = false;


	/* First, verify if the script/language code are correct ...	<#38> */
	if (utBadSandL(slRecPtr))	return paramErr;				 /* <#38> */
		
	/* see if there is such an Input Method supports the script/language  */
	err = GetComponentInfo(aComp, &cdRec, nil, nil, nil);
	if (err)					return err;
	
	
	/* There are serveral cases to consider:					-- 23Jan92
		1. Default IM and current script are not changed. In this case, this is just a NOOP.
		2. Script is not changed, just IM is changed. I need to close the old IM, set the default and open the new one.
		   In this case, don't call "xSetTextServiceLanguage" because it does nothing when script is the same.
		3. Script has changed -- there are two possible cases for that script:
			a). I.M. is the same:	User just wants to switch from one I.M to the other, no default setting is affected.
									I need to deactivate the I.M for the current script, and then call "xSetTextServiceLanguage".
			b). I.M. is different:	User wants to switch to a different script and also changes the default I.M.
									I need to close the old I.M. for that script, set the default,
									and then call "xSetTextServiceLanguage".
		4. And of course for each case, we need to consider if the Japanese is using old input method.
	*/


	
	newScript = slRecPtr->fScript;								/* new setting */
	if (utScriptIsNotCCJK(newScript))							/* does the script needs IM? */
		return tsmScriptHasNoIMErr;								/* only CJK uses IM */

	currentScript = TSMVarsPtr->tsmCurrentSLRec.fScript;		/* current script */
	currentTSMDocument = TSMVarsPtr->tsmActiveDocID;			/* current active document */
	
	if (currentScript == smJapanese || currentScript == smTradChinese ||
		currentScript == smKorean   || currentScript == smSimpChinese) {	/* if currently in CCJK script */

		imIndex = utGetScriptIndex(currentScript);				/* index for the current script */
		/* First check if the current open input method is the same as the new one, */
		if (TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent == aComp && currentScript == newScript) {	/* same component and same script */
			/* When we use old JIM, tsmDefIMInfoTable[smJapanese].gComponent could still points to Akiko's	*/
			/* So we need to special case for Japanese script to activate its IM, oh I hate this ....		*/
			/* For otherwise, this is just a NOOP */
			if (currentScript == smJapanese) {					/* Japanese could switch from old to new IM */
				if (TSMVarsPtr->tsmUseOldJInputMethodP) {		/* if we are switching   from old to new IM */
					(void) RouteEventToSWM(true);				/* start sending events to SWM */
					TSMVarsPtr->tsmUseOldJInputMethodP = false;	/* falg = use the new JIM now  */
					SetScript(smJapanese, kUseOldJapaneseIM, (long) false);		/* then disable the secret flag !!	<#52> */
					/* need to activate the current open input method ...		<27Mar92> */
					if (currentTSMDocument) {					/* if there is a current active document */
						err = utActivateIM4Document(currentTSMDocument, smJapanese);
						if (err == tsmTSNotOpenErr) {
							/* It works until we have a TSM aware application, if we create a document in Roman	*
							 * script and the japanese default IM is Akiko. The document will have no Component	*
							 * and no Instance, utActivateIM4Document will fail if we switch from Roman to SJip	*
							 * first and then switch to Akiko, for there is no Component to activate ......		*/
							
							err = utOpenIM4Document(currentTSMDocument, aComp, smJapanese, true);	/* open and active */
						}
					}
					err = utUpdateDefaultInputMethodResource(aComp, slRecPtr);	/* need to update the resource <08Feb92 #23> */
				}
			}
			return err;											/* and treat this as a noop */
		}

		/* else, script or IM changed */
		/* If script has not changed, then check if we are switching form old to new IM ...  */
		if (newScript == currentScript) {					/* script is the same ...  <#39> */
			/* First, to check if the current input method has been set up in the TSM globals ... */
			/* This happens when we switch from old JIM to new JIM for the first time, oh no, not again !! */
			/* ie., I mean if you boot with SJIP as the default input method. */
			/* or input method changed, ie., oldJIM -> newJIM1 -> oldJIM -> newJIM2.	<#49> */
			if (TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent == nil || TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent != aComp) {	/* <#49> */
				if (currentScript == smJapanese) {					/* Component changed */
					if (TSMVarsPtr->tsmUseOldJInputMethodP) {		/* if we are switching from old to new */
						(void) RouteEventToSWM(true);				/* start sending events to SWM */
						TSMVarsPtr->tsmUseOldJInputMethodP = false;	/* use the new JIM */
						SetScript(smJapanese, kUseOldJapaneseIM, (long) false);		/* then disable the secret flag !!	<#52> */
					}
				}
//				else {
//					/* this happens when system script is CJK with no coresponding IM at boot time */
//					DebugStr("\pSetDefaultIM: current tsmDefIMInfoTable is nil");
//				}
				TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent  = aComp;		/* save this new comp */
				TSMVarsPtr->tsmDefIMInfoTable[imIndex].gMenuHandle = nil;		/* no menu for now .. */
			}
		}

	}

	if (currentScript != newScript) {									/* script is not the same, special check if we are switching to Japanese now ... */
		imIndex = utGetScriptIndex(newScript);							/* index for the new script */
		/* In the case of SJip -> CJK/Roman -> Akiko, the default IM resource is still saying we are using old JIM !!	*/
		if (newScript == smJapanese) {									/* for Japanese again !!!! */
			/* if this tsmUseOldJInputMethodP was set, then the resource file must be updated! */
			forceUpdateResource = TSMVarsPtr->tsmUseOldJInputMethodP;	/* <#36> */
			if (forceUpdateResource) {									/* if we were using the old JIM	<#39> */
				SetScript(smJapanese, kUseOldJapaneseIM, (long) false);	/* then disable the secret flag !!	<#52> */
				TSMVarsPtr->tsmUseOldJInputMethodP = false;				/* so reset the flag -- use the new JIM now <#39> */
			}
		}
	}



	/* If Script and input method Component are the same, we already returned.		*/
	/* So here, we must have switched to a new input method OR to a new script ...	*/
	if (currentScript == newScript) {								/* same script but different IM */
		/* For the current document: need to close the old IM. and open a new one */
		/* And need to set and save the default I.M. in system resource file. */
		if (currentTSMDocument) {									/* if there is a current doc */
		//	currentActiveDocPtr = (TSMDocRecord *) *((Handle) currentTSMDocument);			/* dereference it	<01Aug92 #57> */
		//	if (currentActiveDocPtr->iDocPSNRecord == TSMVarsPtr->tsmSWMsPSNRecord) {		/* doc belongs to SWM		<#57> */
				/* Cannot send AE to application because A5 might not be correct, but we must fix text for SWM.	<01Aug92 #57> *
				 * To ask SWM fixing the text is OK because SWM does not have A5 and the posted text is sent to the		 #57> *
				 * application in the correct context -- our changes to the Process Manager is doing the right thing.	 #57> */
				(void) xFixTSMDocument(currentTSMDocument);			/* fix current input area before closing the input method */
		//	}
			utCloseIM4Document(currentTSMDocument, currentScript, true);	/* close the old one and hide its pallete */

#ifdef 0	// code should be deleted
			// apparently, Akiko has solved this problem, and this code can be removed now. */
			currentActiveDocPtr = (TSMDocRecord *) *((Handle) currentTSMDocument);			/* <27Mar92 #32> */
			if (currentActiveDocPtr->iDocPSNRecord == TSMVarsPtr->tsmSWMsPSNRecord) {		/* doc belongs to SWM	<#32> */
				/* this is another long story. The current application is apparently non-TSM aware,		*
				 * (doc belongs to SWM), if we open the input method now, input method such as Akiko	*
				 * will save the A5 of the current non-TSM aware application. When Akiko send AE to SWM	*
				 * it tries to restore the A5 -- which is not SWM's A5. So the system bumb!!	<#32>	*/
				(void) xDeactivateTSMDocument(currentTSMDocument);	/* tsmActiveDocID will be cleared ....  */
			}
			else
#endif

			err = utOpenIM4Document(currentTSMDocument, aComp, currentScript, true);	/* and open the new one		<#27> */
			if (err)		return err;								/* how to handle this error? */
		}															/* end if (currentTSMDocument) */
		TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent = aComp;	/* save this new Component */
		need2UpdateDefIMp = true;									/* need to update default I.M. */
		need2UpdateScript = false;									/* script has not changed */
	}		/* end if same script */
	else {	/* else, different script -- must disable the IM for the current script.			*
			 * you may wonder where do I activate the new script's IM for the current document,	*
			 * it's done in xSetTextServiceLanguage. It will change iDocUseThisScript too.		*/
		if ( currentTSMDocument &&									/* if there is a current document */
			 utScriptIsCCJKAndNeedsIM( currentScript ) ) {			/* and the current script is CCJK	<#45> */
			/* cannot send AE to application because A5 might not be correct when this is called by KBMenu.		<01Aug92 #57> */
			err = xFixTSMDocument(currentTSMDocument);			/* fix the current input area		<#45> */
			utForceDeactivateIM4Document(currentTSMDocument, currentScript);	/* deactivate the IM for the current script */
		}
		need2UpdateScript = true;									/* script has changed, always update TSM's internal state */
		
		imIndex = utGetScriptIndex(newScript);						/* index for the new script */
		if (TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent == aComp) {		/* same IM for the new script */
			need2UpdateDefIMp = false;								/* do not need to update default I.M. */
		}
		else {														/* new script and new IM */
			need2UpdateDefIMp = true;								/* need to update default I.M. */
			if (currentTSMDocument)									/* if there is a current doc */
				utCloseIM4Document(currentTSMDocument, newScript, true);		/* close the old IM and hide its pallete */
			TSMVarsPtr->tsmDefIMInfoTable[imIndex].gComponent  = aComp;			/* save this new comp */
			TSMVarsPtr->tsmDefIMInfoTable[imIndex].gMenuHandle = nil;			/* no menu for now .. */
		}
		
		if (forceUpdateResource)									/* SJip => Roman => Akiko		<#36> */
			need2UpdateDefIMp = true;								/* need to update default I.M.	<#36> */
	}



	/* Save the resource in System file, as defined in TSMPrivate.H */

	if (need2UpdateDefIMp) {									/* need to update the resource first */
		err = utUpdateDefaultInputMethodResource(aComp, slRecPtr);
		if (err)		return err;
	}

	if (need2UpdateScript)										/* this will open or activate the default IM */
		err = xSetTextServiceLanguage(slRecPtr);				/* and it will change iDocUseThisScript too. */

	return err;
}	/* end xSetDefaultInputMethod */



/*	**********************************************************************	*
 *	Function:	xGetDefaultInputMethod										*
 *				Get the default Input Method saved by TSM which supports	*
 *				the script/language specified in slRecPtr.					*
 *	Input:		Pointer to a ScriptLanguageRecord buffer.					*
 *	Output:		Return noErr and the default component in "ts" if found.	*
 *	Side effect:If the default Input method has never been set, this call	*
 *				will ask the CM for a default and save it with TSM Mgr.		*
 *	•• Note:																*
 *			Even we are using old JIM, this call still returns the new		*
 *			default	Japanese Input Method Component saved in TSM.			*
 *																			*
 *	Modification History:													*
 *	19May92	KST	We should check if user is passing the correct language		*
 *				code corresponding to the script code. If not, returns		*
 *				paramErr. 										<#38>		*
 *	**********************************************************************	*/
pascal OSErr xGetDefaultInputMethod ( Component *aComp, ScriptLanguageRecord *slRecPtr )
{
	OSErr					err;
	short					index;
	ComponentDescription	cdRec;
	TSMVarsRec				*TSMVarsPtr = GetExpandMemTSMGlobals();
	ScriptIDRecordPtr		scriptIDRecPtr;


	/* First, verify if the script/language code are correct ...	<#38> */
	if (utBadSandL(slRecPtr))	return paramErr;				/*  <#38> */
	
	if (utScriptIsNotCCJK(slRecPtr->fScript))					/* does the script have an IM ? */
		return tsmScriptHasNoIMErr;								/* return if no (don't check old JIM) */

	err = utGetTSMDefaultInputMethod(&cdRec, slRecPtr);			/* search from resource */
	if (err == noErr) {											/* we found the default saved by TSM */
		/* but verify if the component is currently registered in the system */
		cdRec.componentFlagsMask = bScriptLanguageMask;			/* verify the script/language code		<16Jan92 #15> */
		*aComp = FindNextComponent(nil, &cdRec);				/* can we find the component? */
		if (*aComp) {											/* if we find the Component */
			index = utGetScriptIndex(slRecPtr->fScript);		/* set global		-- <31Jan92 #18> */
			TSMVarsPtr->tsmDefIMInfoTable[index].gComponent = *aComp;		/* update the in memory copy */
			return noErr;										/* yes, cool */
		}
	}
	else if ( (err == tsmInputMethodIsOldErr)
				&& OldJapaneseInputMethodExists((short) cdRec.componentSubType) ) {		/* default is the old IM */
			scriptIDRecPtr = (ScriptIDRecordPtr) aComp;							/* cast it into special record	<06Feb92 #23> */
			scriptIDRecPtr->sidScript	= (ScriptCode) cdRec.componentType;		/* return script				<06Feb92 #23> */
			scriptIDRecPtr->sidFEPID	= (short)	   cdRec.componentSubType;	/* 		and FEP ID				<06Feb92 #23> */
			TSMVarsPtr->tsmUseOldJInputMethodP = true;							/* set the flag	 				<06Feb92 #23> */
			return tsmInputMethodIsOldErr;										/* return special error			<06Feb92 #23> */
		 }

	/* default has not been set, or it was set but can not be found now, ask CM for a default */
	cdRec.componentType			= kTextService;
	cdRec.componentSubType		= kInputMethodService;
	cdRec.componentManufacturer	= 0;							/* don't care */
	
	cdRec.componentFlags		= utSLRec2CompFlag(slRecPtr);
	cdRec.componentFlagsMask	= bScriptLanguageMask;
	
	*aComp = FindNextComponent(nil, &cdRec);					/* find the default component */
	if (*aComp == nil)			return tsmInputMethodNotFoundErr;

	/* use this component as the default, but before return, set our default */
	return utUpdateDefaultInputMethodResource (*aComp, slRecPtr);

}


/*	******************************************************************************	*
 *	Function:	xSetTextServiceLanguage												*
 *				Set the current text service script/language.						*
 *				Called by ScriptMgr:ScriptMgrKeyGetSet and ScriptMgrSysMenuPatch.a	*
 *	Input:		Pointer to a ScriptLanguageRecord buffer.							*
 *	Output:		tsmUnsupScriptLanguageErr if S/L is not supported by the system.	*
 *				noErr = OK															*
 *	Side effect:1. Input method synchronized with the script/language, if and		*
 *				   only if the script has changed.									*
 *				2. If the script has changed and there is a tsmPreviousDocument,	*
 *				   then the previous document will be cleared. (take down pallete)	*
 *	•• Note ••	If script/language is not changed, this call is a NOOP.				*
 *																					*
 *	Modification History:															*
 *	04Mar92	KST	If there is currently an active document which belongs to SWM, then	*
 *				send an AE to ask her to close the input window if it is visible.	*
 *				This happens when user is using a non-TSM aware application and		*
 *				switches keyscript/IM.										<28>	*
 *	26Mar92	KST	We now keep track of the script used by this document. If we switch	*
 *				script and there is a current active document, then change the		*
 *				script information saved for the current active document.			*
 *	05May92	KST	When we switch to non-CJK script system, need to check if there is	*
 *				previous document needs to be cleaned up. For now floating pallete	*
 *				stays when switch from Akiko to Roman in TSM aware application".<35>*
 *	19May92	KST	We should check if user is passing the correct language code		*
 *				corresponding to the script code. If not, returns paramErr. <#38>	*
 *	******************************************************************************	*/
pascal OSErr xSetTextServiceLanguage( ScriptLanguageRecord *slRecPtr )
{
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	ScriptCode			oldScript, newScript;
	LangCode			oldLanguage;
	OSErr				err = noErr;							/* make sure it's initialized */
	ProcessSerialNumber	PSNRecord;
	TSMDocRecord		*currentActiveDocPtr;



	/* First, verify if the script/language code are correct ...	<#38> */
	if (utBadSandL(slRecPtr))	return paramErr;				/*  <#38> */


	newScript = slRecPtr->fScript;								/* to generate better code on MPW C */
	oldScript = TSMVarsPtr->tsmCurrentSLRec.fScript;
	oldLanguage = TSMVarsPtr->tsmCurrentSLRec.fLanguage;
	if (oldScript == newScript && oldLanguage == slRecPtr->fLanguage)
		return noErr;											/* nothing changed, return right now */


	/* Switching to a new script ... */

	if (utSupportedSL(slRecPtr)) {								/* check if the system supports this script */
	

		if (TSMVarsPtr->tsmActiveDocID) {						/* if currently has active document		<01Feb92 #21> */
			utFixDocumentAndCloseSWMWindow(oldScript);			/* fix text, deactivate IM, and deactivate SWM's document */
		}

		TSMVarsPtr->tsmCurrentSLRec.fScript		= newScript;
		TSMVarsPtr->tsmCurrentSLRec.fLanguage	= slRecPtr->fLanguage;

		/* I should not do this. Someone should call InformTSM ... */
		if (newScript == smJapanese) {							/* only check for Japanese script */
			if (GetScript(smJapanese, kUseOldJapaneseIM)) {		/* are we using the old script? */
				TSMVarsPtr->tsmUseOldJInputMethodP = true;		/* yes, use the old JIM */
			}
		}
		
		if (!TSMVarsPtr->tsmPMgrActiveP) {						/* if PM not active */
			if	( GetExpandMemProcessMgrExists() )				/* PM is active now */
				TSMVarsPtr->tsmPMgrActiveP = true;				/* this should only excute once */
			else												/* if PM is still not active, return now */
				return noErr;									/* we are still in the early booting process */
		}		/* now Process Manager has boot */


		/* If the new script uses imput method then synchronize the Input method with the script. */
		/* Open the default input method of the current script (for the current active document)		<23Jan92 #16> */
		/* If the system is using old Japanese input method, then don't activate SWM.					<12Jan92 #12> */

		if (TSMVarsPtr->tsmActiveDocID) {						/* if currently has active document		<01Feb92 #21> */
			/* this document must belongs to a TSM aware application, we need to open a new IM for it	<05Jun92 #43> */
			/* then open or activate the new default IM for the document.								<23Jan92 #16> */

			/* first, I need to change the script info saved for this document ...						<28Mar92 #32> */
			currentActiveDocPtr = (TSMDocRecord *) *((Handle) TSMVarsPtr->tsmActiveDocID);			/*  <28Mar92 #32> */
			currentActiveDocPtr->iDocUseThisScript	= newScript;	/* now change to use this script	<28Mar92 #32> */
			
			/* second, I need to deactivate the old input method of the previous script ...	*/
			(void) utForceDeactivateIM4Document(TSMVarsPtr->tsmActiveDocID, oldScript);
			
			/* third, I need to activate/open the new input method of the current script .. */
			/* if the current script is one of CCJK, we know we're not using the old JIM.	*/
			if (utScriptIsCCJKAndNeedsIM(newScript)) {			/* if this is one of CCJK script			<02Jul92 #53> */
				err = utOpenActivateDefaultIM(TSMVarsPtr->tsmActiveDocID, true);		 /* activate flag = true	<#53> */
				if (err) {										/* no memory, for example	<#53> */
					if (err != tsmInputMethodNotFoundErr) {		
					/*	If no IM, KBMenu will call us and change the script without checking	*/
					/*	for any error, so I can't restore any state info but return ...			*/
#ifdef BuildTSMInit
						DebugStr("\pCan't open the new IM");	/* restore and leave		<#53> */
#endif
						TSMVarsPtr->tsmCurrentSLRec.fScript		= oldScript;			/*	<#53> */
						TSMVarsPtr->tsmCurrentSLRec.fLanguage	= oldLanguage;			/*	<#53> */
						currentActiveDocPtr = (TSMDocRecord *) *((Handle) TSMVarsPtr->tsmActiveDocID);				/*  <#53> */
						currentActiveDocPtr->iDocUseThisScript	= oldScript;			/* restore to old script	<#53> */
						/* and now, activate the input method for the old script ...		<#53> */
						utActivateIM4Document(TSMVarsPtr->tsmActiveDocID, oldScript);	/*	<#53> */
					}
					return err;															/*	<#53> */
				}
			}
		}
		/* if there is no current active document, say we are in the Finder now, then the				<01Feb92 #21> */
		/* default IM will be opened when a document becomes active, or is been created.				<01Feb92 #21> */

		if (utScriptIsNotCCJK(newScript))						/* if this is not CCJK script			<01Feb92 #21> */
			(void) RouteEventToSWM(false);						/* do not send events to SWM */
		else {													/* else, this is one of CCJK */
			if (newScript == smJapanese) {						/* only check for Japanese script */
				if (GetScript(smJapanese, kUseOldJapaneseIM)) {	/* are we using the old script? */
					(void) RouteEventToSWM(false);				/* yes, don't send events to SWM */
					TSMVarsPtr->tsmUseOldJInputMethodP = true;	/* use the old JIM */
					return err;
				}
			}
			
			/* Current script is one of CJK, and requires TSM input method */
			/* then inform process Manager to start sending events to SWM  */
			
			(void) GetCurrentProcess(&PSNRecord);					/* get current process's PSN */
			if (utaIsAppTSMAwareAndNotUsingIW(&PSNRecord) == noErr)	/* if the current application is TSM aware and not using IW */
				(void) RouteEventToSWM(false);						/* then do not send events to SWM */
			else													/* if the current application is not TSM aware */
				(void) RouteEventToSWM(true);						/* then always sending events to SWM */
		}

		/* if we switch from a TSM doc using Chinese to Finder (Roman) */
		if (TSMVarsPtr->tsmPreviousDocID)						/* clean it up		-- <#35> */
				utCleanUpPreviousDoc(TSMVarsPtr->tsmPreviousDocID, newScript);

		return err;
	}
	return tsmUnsupScriptLanguageErr;
}


/*	***********************************************************	*
 *	Function:	xGetTextServiceLanguage							*
 *				Get the current text service script/language.	*
 *	Input:		Pointer to a ScriptLanguageRecord buffer.		*
 *	Output:		Always succeed.									*
 *				At boot time, Script Manager set this up.		*
 *	Side effect:none.											*
 *	***********************************************************	*/
pascal OSErr xGetTextServiceLanguage( ScriptLanguageRecord *slRecPtr )
{
	TSMVarsRec	*TSMVarsPtr = GetExpandMemTSMGlobals();

	slRecPtr->fScript	= TSMVarsPtr->tsmCurrentSLRec.fScript;
	slRecPtr->fLanguage = TSMVarsPtr->tsmCurrentSLRec.fLanguage;

	return noErr;
}


/*	***********************************************************************	*
 *	Function:	xUseInputWindow												*
 *																			*
 *	Input:		If idocID not nil, then affects only this document,			*
 *				else, affects global setting.								*
 *				useFloatWindow = Boolean flag.								*
 *	Output:		noErr if all OK,											*
 *				tsmInvalidDocIDErr = invalid id,							*
 *																			*
 *	Side effect:															*
 *			1. Process Manager is informed if the current app is using		*
 *			   input window or not.											*
 *			2. If useFloatWindow is false, then the default input method	*
 *			   will be opened/activated for the decument.					*
 *	07Jan91	Design changes:													*
 *	If idocID ==  0, it affects only one (•• current ••) application.		*
 *	If idocID == valid doc id, it applies to that document only.			*
 *	•• SWM should never call this routine. Otherwise ... ••					*
 *																			*
 * Modification history:													*
 *	24Mar92	KST	Need to call utSoftDeactivateIM4Document instead of			*
 *				utDeactivateIM4Document. UtSoftDeactivateIM4Document		*
 *				will not remove IM's pallete and menu.						*
 *	04Jun92	KST	We keep two active documents when using input window.		*
 *				Change the call from utSoftDeactivateIM4Document to			*
 *				utJustDeactivateIM4Document. This call will not change the	*
 *				state info in the TSM global.						<#43>	*
 *	**********************************************************************	*/
pascal OSErr xUseInputWindow( TSMDocumentID idocID, Boolean useFloatWindow )
{
	TSMVarsRec			*TSMVarsPtr = GetExpandMemTSMGlobals();
	TSMDocRecord		*idocRecPtr;
	OSErr				err = noErr;
	ProcessSerialNumber	PSNRecord;
	ScriptCode			currentScript;
	TSMDocumentID		currentTSMActiveDocID;
	Boolean				currentTSMDocIsSWM = false;
	Boolean				appsGlobalFlag, stillActiveP=false;


	currentScript			= TSMVarsPtr->tsmCurrentSLRec.fScript;
	currentTSMActiveDocID	= TSMVarsPtr->tsmActiveDocID;
	if ( currentTSMActiveDocID 		&&							/* it could be nil		<#48> */
		((TSMDocRecordPtr) *(Handle) currentTSMActiveDocID)->iDocPSNRecord == TSMVarsPtr->tsmSWMsPSNRecord )
			currentTSMDocIsSWM = true;							/* current doc belongs to SWM */


	if (idocID) {												/* toggle this document's status */
		if (idocID == -1)										/* global effect */
			return paramErr;									/* changed the design -- no system wide flag */

		// else, this document only ...
		err = utVerifyTSMDocID(idocID);							/* I assume this is the current active document */
		if (err != noErr)										/* invalid id */
			return err;

		idocRecPtr = (TSMDocRecord *) *((Handle) idocID);			/* dereference the handle */
		/* Is this document using input window now?? If not why bother calling me ?? */
		if ( idocRecPtr->iDocUseInputWindowP == useFloatWindow )	/* state never changed */
			return noErr;											/* get out of here ! */

		idocRecPtr->iDocUseInputWindowP = useFloatWindow;			/* local flag is set */
		utaUpdateDocFlagInPSNTable(idocID);							/* update the UseInputWindow flag in the PSN table */
																	/* and will inform Process Manager about this */

		/* application could call this routine with any document, might not be the current one !!! */
		if (useFloatWindow) {										/* this document is using input window now */

			if (TSMVarsPtr->tsmUseInputWindowDocID == idocID)		/* it is the current active document */
				// The current active document is not changed after this call but its current IM is deactivated ....
				stillActiveP = true;								/* we need to keep it active because it might have other text services open */
			else
				stillActiveP = false;								/* this is another doc */
			
			(void) utJustDeactivateIM4Document(idocID, currentScript, stillActiveP);	/* so deactivate the input method but keep active */
		}
		else {														/* this document is NOT using input window now */
			/* if the document/script needs an input method, then we open the default IM for this document */
			/* But first check if there is current active document which is SWM's doc. If it is then need to deactivate it.	*/
			/* The tsmActiveDocID got to be either SWM's document or this idocID. */
			if (currentTSMActiveDocID != nil		&&				/* if there is a current active document */
				TSMVarsPtr->tsmUseInputWindowDocID == idocID) {		/* and we're changing the state of it .. */					
				if ( currentTSMDocIsSWM ) {							/* doc belongs to SWM */
					(void) xFixTSMDocument( currentTSMActiveDocID );/* fix text if any		<#45> */
					err = utJustDeactivateIM4Document(currentTSMActiveDocID, currentScript, false);		/* shut it down */

					if (err == noErr) {
						utCloseSWMWindow(true);						/* ask SWM to close her input window and flush the buffer */
						((TSMDocRecord *) *((Handle) currentTSMActiveDocID))->iDocUseThisScript = kNoSuchScript;
					}
				}
				
				if (utDocNeedsInputMethod(currentScript, &idocRecPtr->iDocPSNRecord))
					(void) utActivateThisDocsInputMethod(idocID);	/* activate the input method */
	
				TSMVarsPtr->tsmActiveDocID = idocID;				/* this one will become the current active doc */
			}

#ifdef BuildTSMInit
			else	Debugger();										/* can't be nil ?? */

			// Debugging .......
			idocRecPtr = (TSMDocRecord *) *((Handle) idocID);		/* dereference the handle */
			if ( !idocRecPtr->iDocActiveP )		Debugger();			/* not active ?? */
#endif
		}
	}
	else {					/* If idocID ==  0, it affects the (current) application, all documents. */
		(void) GetCurrentProcess(&PSNRecord);							/* get current process's PSN */

		// If the global flag is the same, then this is a noop ....
		err = utaGetAppsUseInputWindowFlag( &PSNRecord, &appsGlobalFlag );
		if (err == tsmNeverRegisteredErr )		return err;				/* not TSM aware */

		if ( appsGlobalFlag == useFloatWindow )							/* state is the same */
			return noErr;												/* get out of here */

		(void) utaUpdateAppFlagInPSNTable(&PSNRecord, useFloatWindow);	/* affect the application */

		if ( currentTSMActiveDocID ) {									/* if there is a current active document, do something */

			/*	DocumentX		Global state	Current Doc		Action
			
				use IW			use IW			SWM's			none
				use IW			use IW			documentX		utJustDeactivateIM4Document
				not use IW		use IW			SWM's			can not happen (for a non-TSM aware application)
				not use IW		use IW			documentX		utJustDeactivateIM4Document
				
				use IW			not use IW		SWM's			none
				use IW			not use IW		documentX		none
				not use IW		not use IW		SWM's			utJustDeactivateIM4Document and ask SWM to take down its window (global)
				not use IW		not use IW		documentX		none
			*/

			if (useFloatWindow) {										/* ON: change this document to use input window now */
				stillActiveP = true;
				if ( !currentTSMDocIsSWM )								/* doc does not belong to SWM */
					err = utJustDeactivateIM4Document( currentTSMActiveDocID, currentScript, true );
					// If it is SWM's document, (the document is already using input window) then leave it alone ......
			}
			else {														/* OFF: this document is NOT using input window now */
				if ( currentTSMDocIsSWM ) {								/* doc belongs to SWM */
					idocRecPtr = ((TSMDocRecordPtr) *(Handle) TSMVarsPtr->tsmUseInputWindowDocID);		/* app's document */
					if ( !idocRecPtr->iDocUseInputWindowP ) {			/* the document is not using IW, then must be the global effect */
						(void) xFixTSMDocument( currentTSMActiveDocID );/* fix text if any		<#45> */
						err = utJustDeactivateIM4Document(currentTSMActiveDocID, currentScript, false);
						utCloseSWMWindow(true);							/* ask SWM to close her input window and flush the buffer */
						
						if (utDocNeedsInputMethod(currentScript, &idocRecPtr->iDocPSNRecord))
							err = utActivateThisDocsInputMethod(TSMVarsPtr->tsmUseInputWindowDocID);	/* activate the input method */
#ifdef BuildTSMInit
						if (err)		Debugger();
#endif
						TSMVarsPtr->tsmActiveDocID = TSMVarsPtr->tsmUseInputWindowDocID;
					}
				}
#ifdef BuildTSMInit
				else	Debugger();										/* where is SWM ? */
#endif
			}
		}
	}
	
	if ( stillActiveP )													/* always inform Process Manager to	*/
			(void) RouteEventToSWM(true);								/* start sending events to SWM ...	*/

	if (err == tsmDocNotActiveErr)										/* ignore this error */
			return noErr;
	else	return err;
}


/*	*******************************************************************	*
 *	Function:	xNewServiceWindow										*	
 *	Input:		lots of parameters to create a window					*
 *				and a ComponentInstance which owns this window.			*
 *	Output:		returns an OSErr and the WindowPtr.						*
 *																		*
 *	Side effect: Creates a floting layer by calling the Process 		*
 *				 Manager if the layer has never been created.			*
 *																		*
 *	•• Note: If ts == kCurrentProcess, then we assume the window is		*
 *		 	 created for the application, else it's for Text Services,	*
 *			 and ts should be a Component Instance.						*
 *	*******************************************************************	*/
pascal OSErr xNewServiceWindow(	void				*wStorage,
								const Rect			*boundsRect,
								ConstStr255Param	title,
								Boolean				visible,
								short				theProcID,
								WindowPtr			behind,
								Boolean				goAwayFlag,
								ComponentInstance	ts,
								WindowPtr			*window )
{
	LayerPtr			floatLayer;
	LayerPtr			curLayer;
	WindowPeek			theWindow;
	OSErr				err;
	//ProcessSerialNumber	PSNRecord;
	THz					aZone;


	theWindow = nil;
	err = NewFloatLayer( &floatLayer);					/* Call the Process Manager		*/
	if ( err==noErr) {									/*  to create OR locate a floating layer	*/

		aZone = utSwapZone( SystemZone() );				// create the window in system zone
		// •• need to fix 4 the next release ......
		curLayer = SwapCurLayer( floatLayer);			/* New window in the floating layer		*/
		theWindow = (WindowPeek) NewWindow(	wStorage,
											boundsRect,
											title,
											visible,
											theProcID,
											behind,
											goAwayFlag,
											(long)ts	);
		if ( theWindow != nil ) {
		
			if ((long)ts == kCurrentProcess ) {
				theWindow->windowKind  = applicationFloatKind;
				theWindow->refCon = 0;
			}
			else {
				theWindow->windowKind  = systemFloatKind;
				theWindow->refCon = (long)ts;				/* window belongs to SWM driver */
			}

		} else {
			err = MemError();
		}

		SetCurLayer( curLayer);							/* Reset the current layer */
		(void)utSwapZone(aZone);
	}
	*window = (WindowPtr)theWindow;						/* Return this floating window */
	return err;
}


/*	***********************************************************	*
 *	Function:	xCloseServiceWindow								*
 *	Input:		the floating window to close.					*
 *	Output:		returns an OSErr.								*
 *																*
 *	Side effect: probably none.									*
 *																*
 *	Modification history:										*
 *	31Jul92	KST	Make sure the window that we are going to close	*
 *				is not nil and it is truely a floating window.	*
 *	***********************************************************	*/
pascal OSErr xCloseServiceWindow(WindowPtr window )
{
	LayerPtr	floatLayer;
	LayerPtr	curLayer;
	OSErr		err;

	// First make sure we are closing the right kind of window ......
	if (window													&&		/* window is not nil and ... */
		( ((WindowPeek) window)->windowKind == systemFloatKind	||		/* ....it is my type of baby */
		  ((WindowPeek) window)->windowKind == applicationFloatKind) ) {
		err = NewFloatLayer( &floatLayer);								/* Call the Process Manager		*/
		if ( err==noErr) {												/*  to get the floating layer	*/
	
			curLayer = SwapCurLayer( floatLayer);						/* Close the window			*/
			CloseWindow( window);										/*  in the floating layer	*/
			SetCurLayer( curLayer);										/* Reset the current layer	*/
	
		}
		return err;
	}
	return	paramErr;													/* not a floating window */
}


/*	***********************************************************	*
 *	Function:	xGetFrontServiceWindow							*
 *	Input:		the pointer to the WindowPtr to get an window.	*
 *	Output:		the front most window in the floting layer.		*
 *																*
 *	Side effect: A floating layer will be created if it is not	*
 *				 already been created -- fix in next release.	*
 *	***********************************************************	*/
pascal OSErr xGetFrontServiceWindow(WindowPtr *window )
{
	LayerPtr	floatLayer;
	LayerPtr	curLayer;
	OSErr		err;

	*window = nil;
	err = NewFloatLayer( &floatLayer);					/* Call the Process Manager		*/
	if ( err==noErr) {									/*  to get the floating layer	*/

		curLayer = SwapCurLayer( floatLayer);			/* Get the front window		*/
		*window = FrontWindow();						/*  in the floating layer	*/
		SetCurLayer( curLayer);							/* Reset the current layer	*/

	}
	return err;
}


/*	******************************************************************************	*
 *	Function:	xFindServiceWindow													*
 *	Input:		the pointer to the WindowPtr to get an window.						*
 *				thePoint = point of interest.										*
 *	Output:		which part of which window, if any, the 'thePoint'					*
 *				was pressed in.	the window is returned in theWindow.				*
 *																					*
 * Side effect: Side effect: A floating layer will be created if it is not			*
 *				already been created -- will fix this in the next release.			*
 * Modification history:															*
 *	10Jun92	KST	We patched HMGetBalloon to call FindServiceWindow for supporting	*
 *				floating window. FindWindow will call MBDF when the mouse is pressed*
 *				However, MBDF will call HMGetBalloon when Help is on and the mouse	*
 *				is pressed in the menu bar, HMGetBalloon will call MBDF because of	*
 *				our patch. The MBDF code lock the menulist handle on entry, but 	*
 *				unlock it on exit. So after the HMGetBalloon call, the handle is not*
 *				locked but MBDF keeps a dereferenced pointer, and ..... system will *
 *				eventually crash. The fix is to save the handle state before and	*
 *				restore it after the FindWindow call.						<#42>	*
 *	*******************************************************************************	*/
 
#define 		MenuList 		0xA1C

pascal short xFindServiceWindow(Point thePoint,WindowPtr *theWindow)
{
	LayerPtr	floatLayer;
	LayerPtr	curLayer;
	short		retValue;
	OSErr		err;
	long		*menulistPtr=MenuList;
	char		handleState;

	*theWindow	= nil;
	
	if (*menulistPtr == 0) {								/* if no menulist, then can't call FindWindow,		*/
		return inDesk;										/* ROM tries to get MBDF resource	<26May92 Tokyo>	*/
	}														/* and it does not check for error if can't get it	*/

	retValue	= inDesk;
	err = NewFloatLayer( &floatLayer);						/* Call the Process Manager ...		*/
	if ( err == noErr) {									/* ... to get the floating layer	*/

		curLayer = SwapCurLayer( floatLayer);				/* Find the window	*/
		
		/* When MBDF returns, it unlocks the handle, this makes it not reentrant. */
		handleState = HGetState( (Handle) *menulistPtr );	/* save the handle state		<#42> */
		retValue = FindWindow( thePoint, theWindow);		/* in the floating layer */
		HSetState( (Handle) *menulistPtr, handleState);		/* restore the handle state		<#42> */

		SetCurLayer( curLayer);								/* Reset the current layer	*/

	}
	return retValue;
}

