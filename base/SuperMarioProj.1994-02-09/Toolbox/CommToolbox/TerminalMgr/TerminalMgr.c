/*
	File:		TerminalMgr.c

	Contains:	Guts of the Terminal Manager

	Written by:	Byron Han, Jerry Godes, Mary Chan

	Copyright:	© 1988-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 6/25/91	JNG		Change TerminalsPriv.h to TerminalsExtensions.h
		 <6>	 8/28/90	kaz		Change TermPointer to TermPtr.
		 <5>	 8/23/90	kaz		Saving/Restoring MemErr, ResErr on a _TMNew so that app has a
									clue as to why it failed; Code shrinkage in __TMNew.
		 <4>	 7/10/90	dba		add __ to routine names to prevent conflict with interfaces to
									the same routines
		 <3>	 3/17/90	BBH		added include of ToolUtils.h to make it compile
		 <2>	 3/15/90	BBH		fixed includes files
		 <1>	 3/14/90	BBH		first checked in to BBS
		 
	ToDo:
		

*/

/*
	Pre-BBS Modification History

		9/17/89		BBH		SearchBlockPtr renamed TMSearchBlockPtr
							SearchBlock renamed TMSearchBlock

							SetupPtr renamed TMSetupPtr
							SetupStruct renamed TMSetupStruct
		7/31/89		BBH		InitTM no longer calls InitCTBUtilities thus paralleling behaviour in
								InitFT and InitCM.
		7/14/89		BBH		Call TMClearSearch in TMDispose
		7/7/89		MC		returned error for invalid searchType in TMAddSearch().
		7/5/89		BBH		Preflighted TMPeekLine by nilling out the LineDataBLock structure
							Removed debugs
		6/26/89		BBH		Added TMPeekLine which is like old TMGetLine before TermDataBlock
							change
		6/10/89		JNG		New interfaces.
		6/7/89		kaz		Changed TMGetLine to be a proc that passes a TermDataBlock
								instead of a LineInfoRec.
		6/2/89		JNG		New CTB core interface
		5/25/89		kaz		Mod to TMPaint() to take the new TermDataBlock for pix
		4/6/89		BBH		InitTM - check error codes in InitCRM/InitCTBUtilities
							TMNew - check err code from CRMReserveRF
								  - remove assignment to resrefnum
								  - call TMValidate at end of TMNew
							TMKey - remove differentiation of tmKeyMsg and TMAutoKeyMsgs
									since tools have access to entire event record.  This will
									require changes to the tools.
							TMDefault - check if allocate boolean is TRUE before
										assigning NIL to theConfig upon failure
							TMSetupPreflight - check err from CRMReserveRF
							TMSetupSetup,
							TMSetupItem,
							TMSetupFilter - change call to CRMGetAResource to CTBGetAResource
											(don't register with the CRM) and remove
											balancing call to CRMReleaseResource
							TMSetupCleanup - change call to CRMGetAResource to TMGetAResource
											 (don't register with the CRM) and have only
											 one balancing call to CRMReleaseResource

							TMSetConfig - add call to TMValidate after setting the
										  terminal configuration string
							TMGetVersion - behaves like CMGetVersion in that we go for
										   the vers 1 resource in the tool file by doing
										   a 1 deep call.
							TMAddSearch - add error checking when doing NewPtr
							TMGetTermName - renamed to TMGetToolName
							TMGetProcID - was returning an error code if couldn't find
										  the communications file.  now returns -1.
		4/3/89		JG		Converted to separate file stuff.  Modified InitTM in
							checking if no tools installed, changed TMGetProcID,
							TMGetTermName, added preflighting and postflighting of
							calls to the tool, etc.  Based on connection manager stuff.
		2/21/89		MC		Changed TMIntlToEnglish and TMEnglishToIntl to pass in hTerm instead
							of just theConfig so that the tool does not need to hardcode procID's
		3/2/89		BBH		1.0A1
		8/1/88		BBH		1.0D1


		5/2/89		BBH		When oldConfig allocation fails, now we
								dispose of config
							Bug in TMRemoveSearch if removing now first item in list
								cause serious brain damage (dispose of wrong stuff

							Converted to MPW C 3.0

					JNG		Check after tmInitMsg changed back to 0 - this call returns
								the success/failure, not a ptr.
							TMDispose - should call CRMReleaseResource
							TMAddSearch - Disposes string if error allocating SearchBlock.
		4/27/89		JNG		Removed all of the theProc := hTerm^^.defProc and
								just put it into the procedure call.
								Removed any result local variables in functions.
								TMNew got some more error checking that didn't
								make it in after the code review.
		4/24/89		JNG		Move TMResume TMMenu TMEvent TMActivate TMReset TMIdle to CTBCore
		4/17/89		BBH		Use CTBPrivate.p for ctb private declarations
		4/17/89		JNG		Move Validate and Default to Core (Change internals
								from TM	to CTB)
		4/12/89		JNG		TMGetToolName now in CTBCore
		4/11/89		JNG		Changed TMGetAResource to CTBGetAResource, that and
								CRMGetAResource	are now part of the CTBGlue
								routines.
							TMGetVersion is now part of the CTBCore
							TMSetRefCon, TMGetRefCon, TMSetUserData, TMGetUserData
								are now part of the CTBCore
		4/10/89		JNG		Changes to termrecord for core routines.
		4/7/89		BBH		TMAddSearch - was not returning anything.  now returns refnum of
										  the searc, -1 if invalid.


		4/3/89		BBH		Changed TMGetSelect to pass back length as function result


	Review History
		4/6/89				Santa Cruz.  BBH,CL,MC,AK,JG
*/


#include <Memory.h>
#include <Resources.h>
#include <ToolUtils.h>
#include <SysEqu.h>

#include "CTBUtilities.h"					/* toolbox interface files				*/
#include "CommResources.h"
#include "CommToolboxPriv.h"

#include "Terminals.h"
#include "TerminalsExtensions.h"
#include "TerminalTools.h"
#include "TerminalToolsPriv.h"

#include "TerminalMgrUtilities.c"				// for TDEFExecutedef

#define  __MANAGERCODE__
#include "TerminalMgrExtensions.c"				// for __TMGetErrorString
#undef	__MANAGERCODE__


pascal void __TMClearSearch(TermHandle);

/*
	InitTM - initializes the terminal manager
 */
pascal TMErr __InitTM()
{
	return(InitManager(TMSel));							/* do core initialization */
}

/*
	TMNew - creates terminal record
 */
pascal TermHandle __TMNew(termRect, viewRect, flags, procID, owner, sendProc,
				cacheProc, breakProc, clikLoop, environsProc, refCon, userData)
	const Rect		*termRect;
	const Rect		*viewRect;
	TMFlags			flags;
	short			procID;
	WindowPtr		owner;
	ProcPtr			sendProc,
					cacheProc,
					breakProc,
					clikLoop,
					environsProc;
	long			refCon,
					userData;
{
	TermHandle		hTerm;
	TermPtr			pTerm;
	long			result;
	Handle			theHandle;
	Ptr				thePtr;
	CRMToolContext	theContext;
	short			savedMem = GetMemErr(),			// saved values for MemErr, ResErr
					savedRes = GetResErr();			// if _TMNew returns NIL

	/* open tool file or increment useCount */
	if (CRMReserveRF(procID) != noErr)
		goto err0;

	hTerm = (TermHandle) NewHandleClear(sizeof(TermRecord));	/* allocate the hTerm */
	if (hTerm == nil) {
		savedMem = GetMemErr();
		goto err1;
	}
	
	/* Done by _NewHandleClear	*/
	/*
		(**hTerm).defProc = nil;
		(**hTerm).lastIdle = 0;
		SetRect(&(**hTerm).selection.selRect, 0, 0, 0, 0);
		(**hTerm).selType = 0;
		(TMSearchBlockPtr)(**hTerm).mluField = nil;
		(**hTerm).tmPrivate = (**hTerm).config = (**hTerm).oldConfig = nil;
	*/
	
	pTerm = *hTerm;
	pTerm->flags = flags;
	pTerm->procID = procID;
	pTerm->termRect = *termRect;
	pTerm->viewRect = *viewRect;
	pTerm->owner = owner;

	pTerm->sendProc = sendProc;
	pTerm->breakProc = breakProc;
	pTerm->cacheProc = cacheProc;
	pTerm->clikLoop = clikLoop;
	pTerm->environsProc = environsProc;

	pTerm->refCon = refCon;
	pTerm->userData = userData;

	theHandle = CRMGetAResource(tdefType, procID);	/* get tdef from tool file */
	if (theHandle == nil) {
		savedRes = GetResErr();
		goto err2;
	}

	MoveHHi(theHandle);							/* should be locked down to load low */
	HLock(theHandle);							/* but just in case… */
	(**hTerm).defProc = (ProcPtr) (*theHandle);

	CTBDefault(&thePtr, procID, true, TMSel);	/* get config allocated */
	if (thePtr == nil) {
		savedMem = GetMemErr();
		savedRes = GetResErr();
		goto err3;								/* oops */
	}
	(**hTerm).config = thePtr;

	CTBDefault(&thePtr, procID, true, TMSel);	/* get oldConfig */
	if (thePtr == nil) {
		savedMem = GetMemErr();
		savedRes = GetResErr();
		goto err4;
	}
	(**hTerm).oldConfig = thePtr;

	result = TDEFExecuteDef((long) hTerm, (long) (**hTerm).defProc, tmInitMsg, 0, 0, 0);
	if (result != 0) {
		savedMem = GetMemErr();
		savedRes = GetResErr();
		goto err5;								/* INIT message failed */
	}

	if (CTBValidate((CoreHandle) hTerm, TMSel))
		;
	
	return(hTerm);

/***********************************************************************************/
err5:
	DisposPtr( (**hTerm).oldConfig);	/* release the oldConfig */
err4:
	DisposPtr( (**hTerm).config);		/* release the config */
err3:
	CRMToolPreflight(procID, &theContext);
	CRMReleaseResource(theHandle);		/* release the tdef resource */
	CRMToolPostflight(&theContext);
err2:
	DisposHandle( (Handle) hTerm);		/* get rid of terminal record */
err1:
	CRMReleaseRF(procID);				/* close tool file or decrement useCount */
	GetMemErr() = savedMem;				// restore the error values
	GetResErr() = savedRes;
err0:
	return(nil);						/* send back nothing */
}

/*
	TMDispose - disposes of terminal record
 */
pascal void __TMDispose(hTerm)
TermHandle hTerm;
{
	Handle	theHandle;
	short	procID;
	CRMToolContext	theContext;

	__TMClearSearch(hTerm);				/* clear all searches ??? go through CTB dispatcher */
	procID = (**hTerm).procID;			/* save for now */

	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmDisposeMsg, 0, 0, 0);

	theHandle = RecoverHandle( (Ptr) (**hTerm).defProc );	/* recover tdef resource */
	if ( theHandle != nil)
	{
		CRMToolPreflight(procID, &theContext);
		CRMReleaseResource(theHandle);							/* got it so release it */
		CRMToolPostflight(&theContext);
	}

	DisposPtr( (Ptr) (**hTerm).oldConfig);					/* get rid of oldConfig */
	DisposPtr( (Ptr) (**hTerm).config);						/* get rid of oldConfig */
	DisposHandle( (Handle) hTerm);							/* get rid of hTerm */

	CRMReleaseRF(procID);
}

/*
 	TMKey - called in response to autoKey or keyDown or keyUp message
 */
pascal void __TMKey(hTerm, theEvent)
TermHandle	hTerm;
const EventRecord *theEvent;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmKeyMsg, (long)theEvent, 0, 0);
}

/*
 	TMUpdate - called in response to update message
 */
pascal void __TMUpdate(hTerm, visRgn)
TermHandle	hTerm;
RgnHandle	visRgn;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmUpdateMsg, (long)visRgn, 0, 0);
}

/*
 	TMPaint - called to paint a line on the screen
 */
pascal void __TMPaint(hTerm, theTermData, theRect)
TermHandle			hTerm;
const TermDataBlock *theTermData;
const Rect 			*theRect;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmPaintMsg, (long)theTermData, (long)theRect, 0);
}

/*
 	TMClick - called in response to click message
 */
pascal void __TMClick(hTerm, theEvent)
TermHandle	hTerm;
const EventRecord *theEvent;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmClickMsg, (long)theEvent, 0, 0);
}

/*
 	TMStream - called in response to incoming data
 */
pascal long __TMStream(hTerm, theBuffer, length, flags)
TermHandle	hTerm;
Ptr theBuffer;
long length;
short flags;
{
	return(TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmStreamMsg, (long)theBuffer,
				length, flags));
}

/*
 	TMClear - called in response to click message
 */
pascal void __TMClear(hTerm)
TermHandle	hTerm;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmClearMsg, 0, 0, 0);
}

/*
 	TMResize - called to resize the terminal area
 */
pascal void __TMResize(hTerm, newViewRect)
TermHandle	hTerm;
const Rect *newViewRect;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmResizeMsg, (long)newViewRect, 0, 0);
}

/*
 	TMGetSelect - called to get selection
 */
pascal long __TMGetSelect(hTerm, theData, theType)
TermHandle	hTerm;
Handle 		theData;
ResType		*theType;
{
	return(TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmGetSelectionMsg,
				(long)theData, (long)theType, 0) );
}

/*
 	TMGetLine - called to get line
 */
pascal void __TMGetLine(hTerm, lineNo, theTermData)
TermHandle		hTerm;
short			lineNo;
TermDataBlock	*theTermData;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmGetLineMsg, lineNo,
				(long)theTermData, 0);
}

/*
 	TMPeekLine - called to peek at a line
 */
pascal void __TMPeekLine(hTerm, lineNo, theLineData)
TermHandle		hTerm;
short			lineNo;
LineDataBlock	*theLineData;
{
	theLineData->dataPtr = nil;			/* preflight the call */
	theLineData->dataSize = 0;
	theLineData->attrPtr = nil;
	theLineData->attrSize = 0;
	theLineData->reserved = 0;

	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmPeekLineMsg, lineNo,
				(long)theLineData, 0);
}

/*
 	TMSetSelection - called to set selection
 */
pascal void __TMSetSelection(hTerm, theSelection, selType)
TermHandle	hTerm;
TMSelection *theSelection;
TMSelTypes	selType;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmSetSelectionMsg, (long)theSelection,
				selType, 0);
}

/*
 	TMScroll - called in response to scrolling
 */
pascal void __TMScroll(hTerm, dh, dv)
TermHandle	hTerm;
short		dh, dv;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmScrollMsg, dh, dv, 0);
}

/*
 	TMGetCursor - returns cursor position
 */
pascal Point __TMGetCursor(hTerm, cursType)
TermHandle	hTerm;
short		cursType;
{
	long	result;

	result =  TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmCursorMsg, cursType, 0, 0);
	return( * ((Point *) &result) );
}

/*
 	TMGetTermEnvirons - returns terminal environment
 */
pascal short __TMGetTermEnvirons(hTerm, theEnvirons)
TermHandle	hTerm;
TermEnvironRec *theEnvirons;
{
	return (TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmGetEnvironsMsg,
			(long)theEnvirons, 0, 0));
}

/*
 	TMDoTermKey - performs terminal key operation
 */
pascal Boolean __TMDoTermKey(hTerm, theKey)
TermHandle	hTerm;
const Str255 theKey;
{
	long result;

	result = TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmDoTermKeyMsg,
				(long)theKey, 0, 0);
	return ( result == 0 ? true : false);
}

/*
 	TMCountTermKeys - counts # of terminal keys
 */
pascal short __TMCountTermKeys(hTerm)
TermHandle	hTerm;
{
	return(TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmCountTermKeysMsg, 0, 0, 0));
}

/*
 	TMGetIndTermKey - return id-th terminal key
 */
pascal void __TMGetIndTermKey(hTerm, id, theKey)
TermHandle	hTerm;
short id;
Str255 theKey;
{
	TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmGetIndTermKeyMsg, id,
						(long)theKey, 0);
}

/*
	TMAddSearch - adds a search to the terminal record
 */
pascal short __TMAddSearch(hTerm, theString, where, searchType, callBack)
TermHandle hTerm;
const Str255 theString;
const Rect *where;
TMSearchTypes searchType;
ProcPtr callBack;
{
	short	nextIndex;
	TMSearchBlockPtr pSearchBlock,
					oldSearchBlock;
	StringHandle	theHandle;

	theHandle = NewString(theString);						/* allocate the string handle */
	if (theHandle == nil)
		return( -1 );										/* failed */

	pSearchBlock = (TMSearchBlockPtr) (NewPtr(sizeof(TMSearchBlock)));
	if (pSearchBlock == nil) {								/* no can do */
		DisposHandle(theHandle);
		return( -1 );
	}

	oldSearchBlock = (TMSearchBlockPtr)(**hTerm).mluField;					/* get list of searches */
	nextIndex = (oldSearchBlock == nil ? 0 : oldSearchBlock->refnum + 1 );

	pSearchBlock->next = oldSearchBlock;					/* put into list */
	(TMSearchBlockPtr)(**hTerm).mluField = pSearchBlock;

	pSearchBlock->where = *where;
	pSearchBlock->refnum = nextIndex;
	pSearchBlock->callBack = callBack;
	pSearchBlock->searchType = searchType;
	pSearchBlock->theString = theHandle;

	return(nextIndex);										/* and return */
}

/*
	TMRemoveSearch - removes a search
 */
pascal void __TMRemoveSearch(hTerm, refNum)
TermHandle hTerm;
short		refNum;
{
	TMSearchBlockPtr	oldSearchBlock;
	TMSearchBlockPtr	pSearchBlock;

	oldSearchBlock = (TMSearchBlockPtr)(**hTerm).mluField;

	if (oldSearchBlock == nil)
		return;

	if (oldSearchBlock->refnum == refNum)				/* we are first */
	{
		(TMSearchBlockPtr)(**hTerm).mluField = oldSearchBlock->next;
remove1:
		DisposHandle( (Handle) oldSearchBlock->theString);
		DisposPtr( (Ptr) oldSearchBlock);
		return;
	}

	pSearchBlock = oldSearchBlock->next;
	while (pSearchBlock != nil)
	{
		if (pSearchBlock->refnum == refNum)
		{
			oldSearchBlock->next = pSearchBlock->next;
			oldSearchBlock = pSearchBlock;				/* go to common exit code */
			goto remove1;
		}

		oldSearchBlock = pSearchBlock;
		pSearchBlock = pSearchBlock->next;
	}
}

/*
	TMClearSearch - clears all searches
 */
pascal void __TMClearSearch(TermHandle hTerm)
{
	TMSearchBlockPtr	pSearchBlock;
	TMSearchBlockPtr	oldSearchBlock;

	pSearchBlock = (TMSearchBlockPtr)(**hTerm).mluField;					/* start at top */

	while (pSearchBlock != nil)
	{
		oldSearchBlock = pSearchBlock;
		pSearchBlock = pSearchBlock->next;

		DisposHandle( (Handle) oldSearchBlock->theString );
		DisposPtr( (Ptr) oldSearchBlock );
	}
	(TMSearchBlockPtr)(**hTerm).mluField = nil;							/* nil it out */
}
