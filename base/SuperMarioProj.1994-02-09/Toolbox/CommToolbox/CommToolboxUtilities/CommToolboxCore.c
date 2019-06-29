/*
	File:		CommToolboxCore.c

	Contains:	These routines are the core routines shared between the three major managers
				The glue routines for the managers end up passing the correct selector into
				the utilities manager.

	Written by:	Jerry Godes (from code written by Byron Han, Carol Lee, Dean Wong,
				Tom Dowdy, Alex Kazim, and Mary Chan

	Copyright:	© 1988-1991, 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/12/93	kc		<mb>: I deleted all of the "TheFuture" conditionals in all of
									the CommToolbox files per Mike Bell's instructions. I also had
									to delete some old code that was compiled under the "CubeE"
									conditional.
		<11>	 10/4/91	JSM		Change PsychoticFarmerOrLater to TheFuture.
		<10>	 10/2/91	DTY		Conditionalise <9> out of CubeE.
		 <9>	  7/1/91	BH		fix CTBGetErrorString to return empty string if the tool returns
									and error condition
		 <8>	11/27/90	kaz		Adding ConnectionsExtensions.h for include file <jng>
		 <7>	 10/5/90	kaz		Code shrinking: Added GetType(), CallToolProc(), GetToolProc().
									<jng>
		 <6>	 10/2/90	kaz		Fixin' case of CTB messages to reflect parity in private
									interface files. <jng>
		 <5>	 9/14/90	kaz		Stop using _ReleaseResource on _xxChoose 'proc' since it's
									already purgeable.
		 <4>	 7/10/90	dba		add __ to distinguish routines from glue
		 <3>	 4/24/90	BBH		CTBGetErrorString was incorrectly declaring MgrSel to be a long
									- it really is a short and we were screwing up the stack
		 <2>	 3/15/90	BBH		fix constants
		 <1>	 3/14/90	BBH		first checked in
		 
	Pre BBS History
	
				2/20/90		BBH		use constants for ChooseResource
				2/15/90		BBH		Call LoadResource for setup, script, validation, and localization resources
									Fix SetConfig to return Validate error as appropriate
				1/4/90		BBH		Took out special hack for special about box
				7/14/89		JNG		Raided debugstr
				7/14/89		BBH		If xval defproc returns 0, CTBValidate returns FALSE, otherwise TRUE
				7/12/89		BBH		Added eventProc to ChooseRec
				6/30/89		BBH		Added CTBSetupXCleanup with OKed BOOLEAN
				6/2/89		JNG		New CTB core interface (mgrsel now last parameter).
				6/1/89		BBH		CTBSetConfig was calling CTBValidate but was passing CMSel for the
										manager selector instead of MgrSel.  So CMValidate worked but
										TMValidate and FTValidate did not.
				5/17/89		BBH		Changed CTBDefault to pass procID as p3
				5/8/89		BBH		CTBChoose routine now defines choose entry to return short instead of long
									This fixes problem of not detecting proper return result from
										Choose routines.
				5/4/89		BBH		Changed to new resource map scheme
									Modified SetupPreflight and SetupPostflight to jam the
										resource map for the tool into the resource chain
									Takes advantage of backdoor to CRMToolPreflight (-1 for
										procID) and CRMToolPostflight(0 for oldProcID in context)
				4/29/89		JNG		CTBGetVersion now resets resfile to whatever was the resfile before.
									CTBSetupPreflight - I put the MoveHHi back in.  This way, if a tool
										hasn't set the locked bit, we will do the proper thing, and
										move the tool out of the way.
									Cleanup of various routines.
				4/27/89		JNG		Changed everything to have callproc be long.
									Changed CTBGetProcID to check for match in tool name and
										tool type.
							BBH		CTBSetupItem still had wrong prototype for calling the code rsrc
									Changed from void to long return
				4/24/89		JNG		Added CTBResume CTBEvent CTBMenu CTBAbort CTBActivate
									CTBReset to ctbcore
				4/20/89		BBH		Changed CTBSetConfig to return integer instead of boolean.
										0 means no error, -1 is generic error, >0 is character position
										that error occured at.
				4/18/89		BBH		Added CTBSetupSetup, Cleanup, Preflight, Postflight, Item, Filter
									Added CTBIntlToEnglish, EnglishToIntl, GetConfig, SetConfig
									Added CTBGetProcID.
				4/17/89 	JNG		Added CTBDefault
				4/11/89 	JNG		Added CTBGetVersion
				4/8/89		JNG 	Pulling routines from the managers.

	To Do:
		• Why is 'xset' loaded twice and released twice?  Why not once at start & end?

*/

#include "CommToolboxPriv.h"
#include "CommResources.h"
#include "CTBUtilities.h"
#include "ConnectionsExtensions.h"
#include "ConnectionTools.h"

ResType		GetType(short mgrID, short resID);
long 		CallToolProc(Handle theProc, CoreHandle hCore, short theMessage, 
								long p3, long p4, long p5, SetupStruct *rSetup);
Handle 		GetToolProc(short procID, ResType theType, Boolean moveHi);

/* NOTE:  InitXX has to trap into the appropriate manager because this is the
			signal for Patch On Demand to load in the manager.  It then traps
			into this core routine.  The double trap isn't all that bad because
			this call is not made often

	This routine will initialize the appropriate Manager return if there are
	any tools installed or not

*/
pascal short __InitManager(short MgrSel)
{
	short	rval;
	short 	vRefNum;
	long	dirID;
	OSErr	err;
	ResType theType;

	err = CRMFindCommunications(&vRefNum, &dirID);	/* find the Communications folder */
	if (err != noErr)
		return(err);
	
	theType = GetType(MgrSel, BndType);

	rval = CRMCountFiles(theType, vRefNum, dirID);	/* are we home free?					*/

	return(rval == 0 ? CTBNoTools : noErr);			/* if we found any tools, we are ok		*/
													/* otherwise, we need to return error	*/
													/* if no tools were found				*/
}


/*
 *	FUNCTION	CTBGetVersion(hCore: CoreHandle;mgrsel: INTEGER): Handle;
 *
 *	This routine will return the handle to the vers 1 resource in the tool.
 */
pascal Handle __CTBGetVersion(CoreHandle hCore,short MgrSel)
{
#pragma unused(MgrSel)

	Handle			theHandle;
	short			oldRF;
	CRMToolContext	theContext;

	CRMToolPreflight((**hCore).procID, &theContext);
	oldRF = CurResFile();										/* get current resource file	*/
	UseResFile((**hCore).procID);								/* use the tool	*/

	theHandle = Get1Resource('vers', 1);						/* get the vers=1 resource		*/

	if (theHandle != nil)										/* if not nil detach it			*/
		DetachResource(theHandle);

	UseResFile(oldRF);											/* set the world back */

	CRMToolPostflight(&theContext);
	return( theHandle );										/* beam me up scotty			*/
}


pascal void __CTBDefault(Ptr *config,short procID,Boolean allocate,short MgrSel)
{
	Handle 			theHand;
	ResType			theType;

	theType = GetType(MgrSel,ValType);
	
	theHand = GetToolProc(procID,theType,true);		// true => _MoveHHi
	
	if (theHand != nil)
		CallToolProc(theHand,nil,CTBDefaultMsg,(long) config,
										(long) allocate,(long) procID,nil);
	else
		if (allocate)
			*config = nil;					/* didn't get it */
}


pascal Boolean	__CTBValidate(CoreHandle hCore,short MgrSel)
{
	Handle 			theHand;
	ResType			theType;
	long			rval = 0;		// optimistic return value
	
	theType = GetType(MgrSel,ValType);
	
	theHand = GetToolProc((**hCore).procID,theType,true);			// true => _MoveHHi
	
	if (theHand != nil)
		rval = CallToolProc(theHand, hCore, CTBValidateMsg, 0, 0, 0, nil);

	return(rval == 0 ? false : true);					/* xval return 0 is TRUE, != 0 is FALSE */
}



/* This routine should check and make sure CRMOpenRFPerm has found a file with the correct name
	*AND* filetype.  This was you can't get a "Serial" terminal tool */
pascal short __CTBGetProcID(Str255 name,short MgrSel)
{
	short		vRefNum;
	long		dirID;
	OSType 		theType;

	if (CRMFindCommunications(&vRefNum, &dirID) != noErr)
		return(-1);

	theType = GetType(MgrSel,BndType);
	if (! CRMCheckTool(name,theType, vRefNum, dirID))
		return (-1);										/* Not found */

	return(CRMOpenRFPerm(name, vRefNum, dirID));
}


/*	This routine is the core SetupPreflight routine which will be called by all three
	main managers.  It will return a dialog item list to be appended to the
	dialog.
*/
pascal Handle __CTBSetupPreflight(short procID,long *magicCookie,short MgrSel)
{
	Handle 			theHand; 					/* handle to the resource */
	Handle			result;
	SetupStruct 	rSetup;						/* to be sent to tool */

	OSErr			err;						/* reserve error */
	CRMToolContext	theContext;	
	ResType			theType;					/* resource type */

	theType = GetType(MgrSel,SetType);

	err = CRMReserveRF(procID);				/* increment usecount for the tool */
	if (err != noErr) {
		return (nil);						/* there was this little problem… */
	}

	result = 0;								/* pessimistic */
	theHand = CRMGetAResource( theType, procID );
	theHand = CRMGetAResource( theType, procID );
											/* Get setup proc and register with CRM */
	if ( theHand != nil)
	{
		LoadResource(theHand);									// <1>

		MoveHHi( theHand);					/* Hopefully - this will error because the tool
												has locked this resource down.  But, just
												in case they haven't, we'll move it out of
												the way */
		HLock( theHand );

		rSetup.theDialog = nil;
		rSetup.count = 0;
		rSetup.theConfig = nil;
		rSetup.procID = procID;

		theContext.oldProcID = 0;				/* special invalid message */
		theContext.toolProcID = procID;
		CRMToolPreflight(-1, &theContext);	/* so that res map is avail during duration */

		result = (Handle) CallToolProc(theHand,nil,CTBPreflightMsg,0,0,(long) magicCookie, &rSetup);

	}
	return( result );
}

/*	This routine is the core SetupSetup routine which will be called by all three
	main managers.  It will setup the dialog item list that was appended to the
	dialog.
*/
pascal void __CTBSetupSetup(short procID,Ptr theConfig,short count,
						DialogPtr theDialog,long  *magicCookie,short MgrSel)
{
	Handle 			theHand;					/* handle to the resource */
	SetupStruct 	rSetup;						/* to be sent to tool */
	ResType			theType;					/* resource type */

	theType = GetType(MgrSel,SetType);
	
	theHand = GetToolProc(procID,theType,false);	// false => No _MoveHHi
	if (theHand != nil) {
		rSetup.theDialog = theDialog;
		rSetup.count = count;
		rSetup.theConfig = theConfig;
		rSetup.procID = procID;
		
		CallToolProc(theHand,nil,CTBSetupMsg,0,0,(long) magicCookie,&rSetup);
	}
}

/*	This routine is the core SetupItem routine which will be called by all three
	main managers.  It will process items hit in the tool's part of the dialog item list
*/
pascal void __CTBSetupItem(short procID,Ptr theConfig,short count,
						DialogPtr theDialog,short *item,long *magicCookie,short MgrSel)
{
	Handle 			theHand;					/* handle to the resource */
	SetupStruct 	rSetup;						/* to be sent to tool */
	ResType			theType;					/* resource type */

	theType = GetType(MgrSel,SetType);

	theHand = GetToolProc(procID,theType,false);	// false => No _MoveHHi
	if (theHand != nil) {
		rSetup.theDialog = theDialog;
		rSetup.count = count;
		rSetup.theConfig = theConfig;
		rSetup.procID = procID;

		CallToolProc(theHand,nil,CTBItemMsg,(long) item,0,(long) magicCookie,&rSetup);
	}
}

/*	This routine is the core SetupFilter routine which will be called by all three
	main managers.  It will process filter events in the setup dialog filter procedure
*/
pascal Boolean __CTBSetupFilter(short procID,Ptr theConfig,short count,
									DialogPtr theDialog,EventRecord *theEvent,
										short *theItem,long *magicCookie,short MgrSel)
{
	Handle 			theHand;					/* handle to the resource */
	SetupStruct 	rSetup;						/* to be sent to tool */
	long			result = false;
	ResType			theType;					/* resource type */

	theType = GetType(MgrSel,SetType);
	
	theHand = GetToolProc(procID,theType,false);	// false => No _MoveHHi
	if (theHand != nil) {
		rSetup.theDialog = theDialog;
		rSetup.count = count;
		rSetup.theConfig = theConfig;
		rSetup.procID = procID;

		result = CallToolProc(theHand,nil,CTBFilterMsg,(long) theEvent,
												(long) theItem,(long) magicCookie,&rSetup);
	}
	
	return(result != 0);			/* 1 means handled so return TRUE, 0 means FALSE */
}

/*	This routine is the core SetupCleanup routine which will be called by all three
	main managers.  It will process perform any cleanup of the setup dialog before it
	is shortened or disposed of.
*/
pascal void __CTBSetupXCleanup(short procID,Ptr theConfig,short count,
							DialogPtr theDialog,Boolean OKed,long *magicCookie,short MgrSel)
{
	Handle 			theHand;					/* handle to the resource */
	SetupStruct 	rSetup;						/* to be sent to tool */
	CRMToolContext	context;					/* resource map context */
	ResType			theType;					/* resource type */
	pascal long		(*callProc) (SetupStruct *, short, long, long, long);

	theType = GetType(MgrSel,SetType);

	theHand = GetToolProc(procID,theType,false);	// false => No _MoveHHi
	if (theHand != nil) {
		rSetup.theDialog = theDialog;
		rSetup.count = count;
		rSetup.theConfig = theConfig;
		rSetup.procID = procID;

		CRMToolPreflight(procID, &context);
			(Ptr) callProc = *theHand;
			(*callProc) ( &rSetup, CTBCleanupMsg, 0, (OKed ? 1 : 0), (long)magicCookie);
			CRMReleaseResource(theHand);		/* Release and deregister with CRM (registered in
													SetupPreflight)*/
			CRMReleaseResource(theHand);		/* Release and deregister with CRM (registered in
												SetupPreflight)*/
		CRMToolPostflight(&context);

	}
}

pascal void __CTBSetupCleanup(short procID,Ptr theConfig,short count,
								DialogPtr theDialog,long *magicCookie,short MgrSel)
{
	__CTBSetupXCleanup(procID, theConfig, count, theDialog, false, magicCookie, MgrSel);
}


/*	This routine is the core SetupPostflight routine which will be called by all three
	main managers.  It will process perform any postflighting of the setup dialog after it
	is shortened or disposed of.  This may involve closing the tool file.
*/
pascal void __CTBSetupPostflight(short procID,short MgrSel)
{
#pragma unused(MgrSel)

	OSErr			err;
	CRMToolContext	theContext;

	theContext.oldProcID = 0;				/* special invalid message */
	theContext.toolProcID = procID;
	CRMToolPostflight(&theContext);
	err = CRMReleaseRF(procID);				/* decrement usecount for the tool */
}


/*	This routine is the core GetConfig routine which will be called by all three
	main managers.  It will return the configuration string for the tool referenced by
	hCore.
*/
pascal Ptr __CTBGetConfig(CoreHandle hCore,short  MgrSel)
{
	Ptr 			result = nil;							/* value to return */
	Handle 			theHand;						/* the resource handle */
	ResType			theType;					/* resource type */

	theType = GetType(MgrSel,ScrType);

	theHand = GetToolProc((**hCore).procID,theType,true);	// true => _MoveHHi
	if (theHand != nil)
		result = (Ptr) CallToolProc(theHand,hCore,CTBGetMsg,0,0,0,nil);

	return(result);
}

/*	This routine is the core SetConfig routine which will be called by all three
	main managers.  It will take the configuration string given and send it to the
	tool referenced by hCore.  CTBValidate will be called.
*/
pascal short __CTBSetConfig(CoreHandle hCore,Ptr thePtr,short MgrSel)
{
	long 		result = -1;				/* value to return */
	Handle 		theHand;					/* the resource handle */
	ResType		theType;					/* resource type */

	theType = GetType(MgrSel,ScrType);
	
	theHand = GetToolProc((**hCore).procID,theType,true);	// true => _MoveHHi
	if (theHand != nil) {
		result = CallToolProc(theHand,hCore,CTBSetMsg,(long) thePtr,0,0,nil);
		
		// Validate the result  <1.1>
		// If there was no scripting error but Validate fails, return -1
		// If there WAS a scripting error, return this error regardless
		// If validate succeeds, return whatever error was going to be returned
		//
		if (__CTBValidate(hCore, MgrSel))
			if (result == 0)
				result = -1;					/* validate any changes made */
												/* fixed 6/1/89 BBH changed CMSel to MgrSel */
	}

	return( result );					/* 0 means no problem, -1 means generic error,
											positive integer is the character position
											that the setconfig failed upon */
}


/*	This routine is the core IntlToEnglish routine which will be called by all three
	main managers.  It will take the configuration string given and send it to the
	tool referenced by hCore.  The tool will convert the configuration string to english.
*/
pascal short __CTBIntlToEnglish(CoreHandle hCore,Ptr input,Ptr *output,short language,short MgrSel)
{
	Handle 		theHand;
	long 		result;
	ResType		theType;					/* resource type */

	theType = GetType(MgrSel,LocType);

	*output = nil;							/* pessimism */

	theHand = GetToolProc((**hCore).procID,theType,true);	// true => _MoveHHi
	result = ResError();
	
	if (theHand != nil)
		result = CallToolProc(theHand,hCore,CTB2EnglishMsg,(long) input,
												(long) output,(long) language,nil);
	return( result );
}

/*	This routine is the core EnglishToIntl routine which will be called by all three
	main managers.  It will take the configuration string given and send it to the
	tool referenced by hCore.  The tool will convert the configuration string to the
	specified language.
*/
pascal short __CTBEnglishToIntl(CoreHandle hCore,Ptr input,
									Ptr *output,short language,short MgrSel)
{
	Handle 		theHand;
	long 		result;
	ResType		theType;					/* resource type */
	
	theType = GetType(MgrSel,LocType);

	*output = nil;							/* pessimism */

	theHand = GetToolProc((**hCore).procID,theType,true);	// true => _MoveHHi
	result = ResError();
	
	if (theHand != nil)
		result = CallToolProc(theHand,hCore,CTB2IntlMsg,(long) input,
											(long) output,(long) language,nil);
	return(result);
}


/*	This routine is the core Choose routine which will be called by all three
	main managers.  It will bring up the standard tool choosing dialog.
*/
pascal short __CTBChoose(CoreHandle *hCore,Point where,ProcPtr idleProc,short MgrSel)
{
	long 			result;
	Handle			theProc;
	pascal 	short	(*callProc) (short, CoreHandle *, long, ChooseRec *);
	ChooseRec		cRec;

	cRec.msg = 0;													/* default message is 0 */
	cRec.idleProc = idleProc;
	cRec.filterProc = nil;
	cRec.newTool[0] = (char) 0x00;
	cRec.newConfig = nil;
	cRec.reserved = 0;
	cRec.eventProc = nil;

	result = 0;															/* pessimistic */
	
	// Resource is SysHeap (i.e. no _MoveHHi), Purgeable
	theProc = GetResource(CTB_ChooseType, CTB_ChooseID);
	if (theProc != nil) {
		LoadResource(theProc);											// why???
		HLock(theProc);

		(Ptr) callProc = *theProc;
		result = (*callProc) (MgrSel, hCore, *((long *)(&where)), &cRec);

		HUnlock(theProc);
	}
	
	return(result);
}

/*	This routine is the core Choose routine which will be called by all three
	main managers.  It will bring up the standard tool choosing dialog.
*/
pascal short __CTBPChoose(CoreHandle *hCore,Point where,ChooseRec *cRec,short MgrSel)
{
	long 			result;
	Handle			theProc;
	pascal short	(*callProc) (short, CoreHandle *, long, ChooseRec *);

	cRec->reserved = 0;

	result = 0;							

	theProc = GetResource(CTB_ChooseType, CTB_ChooseID);
	if (theProc != nil) {
		LoadResource(theProc);											// why??
		HLock(theProc);

		(Ptr) callProc = *theProc;
		result = (*callProc) (MgrSel, hCore, *((long *)(&where)), cRec);

		HUnlock(theProc);
	}
	
	return(result);
}

pascal void __CTBResume(CoreHandle hCore,Boolean res)
{
	CallToolProc(nil,hCore,(res ? CTBResumeMsg : CTBSuspendMsg),0,0,0,nil);
}

pascal Boolean __CTBMenu(CoreHandle hCore,short menuid,short item )
{
	return(CallToolProc(nil,hCore,CTBMenuMsg,menuid,item,0,nil) == 0 ? false : true);
}

pascal void __CTBGetErrorString(CoreHandle hCore,short id,Str255 errMsg,short MgrSel)
{
	long rval;
	
	rval = CallToolProc(nil,hCore,(MgrSel == CMSel ? cmGetErrorStringMsg : CTBGetErrorStringMsg),
	 																	id,(long) errMsg,0,nil);
	if (rval) errMsg[0] = 0;						// return empty string if there is a problem
}

pascal void __CTBEvent(CoreHandle hCore,EventRecord *theEvent)
{
	 CallToolProc(nil,hCore,CTBEventMsg,(long) theEvent,0,0,nil);
}

pascal void __CTBActivate(CoreHandle hCore,Boolean act)
{
	 CallToolProc(nil,hCore,(act ? CTBActivateMsg : CTBDeactivateMsg),0,0,0,nil);
}

pascal OSErr __CTBAbort(CoreHandle hCore)
{	 
	return(CallToolProc(nil,hCore,CTBAbortMsg,0,0,0,nil));
}

pascal void __CTBReset(CoreHandle hCore)
{
	CallToolProc(nil,hCore,CTBResetMsg,0,0,0,nil);
}


/*******************************************************
	GetType		- Returns the tool resType for the given
					mgr and type index
					
		mgrID	- CMSel, TMSel, or FTSel
		resID	- bndType..locType
	
	returns		- 'cbnd', 'fdef', etc
********************************************************/
ResType GetType(short mgrID, short resID)
{
	ResType	IDTable[6][3] = {	{'cbnd','tbnd','fbnd'},
								{'cdef','tdef','fdef'},
								{'cval','tval','fval'},
								{'cset','tset','fset'},
								{'cscr','tscr','fscr'},
								{'cloc','tloc','floc'}	};
	
	return(IDTable[resID][mgrID]);
}

/********************************************************************
	CallToolProc	- Jumps to a tool routine and returns the result
						
		theProc		- tool code resource to jump to. If it's nil, 
						we assume we want the defProc and will not 
						release it.
		hCore		- tool handle;  if = nil, auxInfo holds the procID
		theMessage	- message to pass
		p3,p4,p5	- params for the message
		rSetup		- SetupStruct * for stup messages
	
		returns		- function result
**********************************************************************/
long CallToolProc(Handle theProc, CoreHandle hCore, short theMessage, 
								long p3, long p4, long p5, SetupStruct *rSetup)
{
	long			result;
	CRMToolContext	context;
	long			p1;			
	pascal long		(*callProc) (long, short, long, long, long);
	short			procID;
	
	// Try to get the procID.  For CTBDefaultMsg & setup hCore == nil
	// so we get them from other places
	
	if (hCore == nil) {
		if (theMessage == CTBDefaultMsg)
			procID = p5;
		else if (rSetup != nil)
			procID = rSetup->procID;
	}
	else
		procID = (**hCore).procID;
		
	// Special case 2: CTBSetupMsgs
	p1 = (long) hCore;
	if (rSetup != nil)
		p1 = (long) rSetup;
		
	CRMToolPreflight(procID, &context);			// put the tool in the chain
		
		// Get the routine to jump to.  Assume defProc if nil is passed
		if (theProc != nil)
			(ProcPtr) callProc = (ProcPtr) *theProc;
		else
			(ProcPtr) callProc = (**hCore).defProc;
		
		result = (*callProc) (p1, theMessage, p3, p4, p5);
		
		// we release non defProc resources thru CRM
		if (theProc != nil)
			CRMReleaseResource(theProc);
			
	CRMToolPostflight(&context);
	
	return(result);
}

/*******************************************************
	GetToolProc		- Gets the tool code resource, loads
					it high, and locks it down
					
		theID	- tool res refnum
		theType	- tool resource type
	
	returns		- the resource handle
********************************************************/
Handle GetToolProc(short procID, ResType theType, Boolean moveHi)
{
	Handle	theCode = CRMGetAResource(theType, procID);
	
	if (theCode != nil) {
		LoadResource(theCode);	
		if (moveHi)
			MoveHHi(theCode);
		HLock(theCode);
	}
	
	return(theCode);
}