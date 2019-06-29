/*
	File:		FileTransferMgr.c

	Contains:	Guts of the File Transfer Manager

	Written by:	Carol Lee

	Copyright:	© 1988-1990, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	  2/7/92	BH		changed define for MANAGERCODE to be TRUE or FALSE to allow
									FileTransferManagerExtensions.c to properly conditionalize for
									TheFuture and for CubeE and for building the glue file
		 <6>	 7/10/91	dba		change calling conventions for FDEFExecuteFDEF
		 <5>	 7/10/91	BH		rename FDEFCallFDEF to FDEFExecuteFDEF
		 <4>	 8/27/90	kaz		Saving/Restoring MemErr, ResErr if _FTNew returns nil; Code
									shrinkage.
		 <3>	 7/10/90	dba		add __ to distinguish routines from glue
		 <2>	 3/15/90	BBH		updated include files
		 <1>	 3/14/90	BBH		first checked in

	To Do:
*/

/* ----------------------------------------------------------------------------------------------

 	Pre-BBS History

		8/18/89		BBH		Include FileTransferToolPrivate for ftCleanupMsg
		7/14/89		JNG		Removed FTGetProcID (we're calling core now).
		6/2/89		JNG		New CTB core routine interface
		5/18/89		JNG		Just removed an unused variable from FTNew. (I was tired of the warning).
		5/5/89		BBH		common exit code for FTNew.  bracked CRMReleaseResource calls with
								CRMToolPreflight/CRMToolPostflight
		5/4/89		CL		copy new flags - FTNoMenu & FTQuit
		4/30/89 	CL		Move file transfer buffer allocation to tools.
		4/30/89		BBH		Modified file Transfer Record to remove fDefProc and pDefProc and other
								assorted flotsam and jetsam and moved into private.
							Also, when FTNew was cleaning up from an error, it did a DisposHandle
								on the main resource.  Should do a ReleaseResource
		4/28/89		BBH		Added flags to FTNew routine.
		4/24/89		JNG		Moved FTResume FTEvent FTMenu FTAbort to ctbcore
		4/24/89 	BBH		Modified FTNew and file transfer record to add environsProc
		4/17/89		JNG		Moved FTValidate and FTDefault into CTBCore (changed internal calls to CTBxxx)
							This also forced out a discrepancy between documentation, the other mgrs, and
							the FTMgr - FTDefault was passing the configptr in the FTHandle field instead
							of the p1 field as documented and as the other mgrs do.  Our two tools have
							now been revved to actually use the data from the correct parameters.
		4/13/89		BBH		Added FTMenu routine.
		4/12/89		JNG		FTGetTooName is now in CTBCore
		4/11/89		JNG		Changed FTGetAResource to CTBGetAResource, that and CRMGetAResource
								are now part of the CTBGlue routines.
							FTGetVersion is now part of the CTBCore
							FTGetRefCon, FTSetRefCon, FTGetUserData, FTSetUserData are now
								part of the CTBCore
		4/7/89		JNG		Changed FTGetName to FTGetToolName
							Made changes discovered by Code Review / Offsite (mostly
							making FTMgr consistent with other mgrs
							Include FTIntf.h (consistency w/ CM)
		4/4/89		CL		Changed to use separate resource files for tools
		3/17/89		CL		Add direction and ReplyPtr as paramaters of FTStart.
		10/31/88	CL		in ftevent change the eventrecord to ptr.
		10/27/88	CL		only set IsFTMode & ftSucc bits in flags field if the ftStartMsg returns
								ftNoErr.
		10/26/88	CL		cooperate get/release resource with CRM.
		10/25/88	CL		make room in stack for FTGetFTVersion in ftentry.a to fixed bus error problem.
		9/29/88		CL		Change the FTRec to add SameCircuit & move the internal use flags to
							global.
		9/29/88		CL		Initiate (**hFT).errCode in init & start.
		9/20/88 	CL		Change the FTValidate to pass the hFT instead of (**hFT).Config
							for 1st param when calling the CallfDef. **CL**
--------------------------------------------------------------------------------------------------*/

#include <Types.h>
#include <Memory.h>
#include <Resources.h>
#include <SysEqu.h>

#include "CommResources.h"				/* Communications Resource Manager */
#include "CTBUtilities.h"
#include "CommToolboxPriv.h"

#include "FileTransfers.h"
#include "FileTransferTools.h"
#include "FileTransferToolsPriv.h"

#include "FileTransferMgrUtilities.c"			// for FDEFExecuteFDEF

//	<7>	changed to do TRUE and FALSE for the define
#define __MANAGERCODE__		TRUE
#include "FileTransferMgrExtensions.c"			// for FTGetErrorString

/*
 *	InitFT - the real meat here
 *
 *	This routine will initialize the File Transfer Manager and will also initialize
 *	the Communications Toolbox Utilities and the Communications Resource Manager.
 *
 *	Under MultiFinder patch on demand, the trap dispatch code will automatically load in
 *	the Connection Manager code into the application heap and load the appropriate
 * 	pointers into the internal dispatch table
 */
pascal short __InitFT(void)
{
	return (InitManager(FTSel));
}

/* allocate a new file xfer record and storage */
pascal FTHandle __FTNew(short procID, long flags, ProcPtr sendProc, ProcPtr recvProc,
						ProcPtr readProc, ProcPtr writeProc, ProcPtr environsProc,
						WindowPtr theOwner, long refCon, long userData)
{
	FTHandle 			hFT;
	FTPtr				pFT;
	short				result;
	Handle				tempH;
	Ptr					aConfig;
	CRMToolContext		theContext;
	short				savedMem = GetMemErr(),		// saved in case _FTNew => nil
						savedRes = GetResErr();

	if (CRMReserveRF(procID) != noErr)
		goto err0;

	hFT = (FTHandle) NewHandleClear(sizeof(FTRecord));
	if (hFT == nil) {
		savedMem = GetMemErr();
		goto err1;
	}

	/* Done by _NewHandleClear	*/
	/*
	(**hFT).errCode = 0;
	(**hFT).ftPrivate = nil;
	(**hFT).oldConfig = nil;
	(**hFT).config = nil;
	(**hFT).attributes = 0;
	(**hFT).autoRec[0] = (unsigned char) (0x00);
	*/

	/* get the version from resource */
	pFT = *hFT;
	pFT->procID = procID;
	pFT->flags = flags | ftSucc;
	pFT->sendProc = sendProc;
	pFT->recvProc = recvProc;
	pFT->readProc = readProc;
	pFT->writeProc = writeProc;
	pFT->environsProc = environsProc;
	pFT->owner = theOwner;
	pFT->refCon = refCon;
	pFT->userData = userData;

	/* control DEF */
	tempH = CRMGetAResource(fdefType, procID);
	if (tempH == nil) {
		savedRes = GetResErr();
		goto err2;
	}

	MoveHHi(tempH);
	HLock(tempH);									/* keep it low, did have a MoveHHi */
	(**hFT).defProc = (ProcPtr) *tempH;

	CTBDefault(&aConfig, procID, true, FTSel);
	if (aConfig == nil) {
		savedMem = GetMemErr();
		savedRes = GetResErr();
		goto err3;
	}
	(**hFT).config = aConfig;


	/* duplicate config */
	CTBDefault(&aConfig, procID, true, FTSel);
	if (aConfig == nil) {
		savedMem = GetMemErr();
		savedRes = GetResErr();
		goto err4;
	}
	(**hFT).oldConfig = aConfig;


	/* init the file xfer tool */
	result = FDEFExecuteFDEF(hFT, ftInitMsg, 0, 0, 0);
	if (result != 0) {
		savedMem = GetMemErr();
		savedRes = GetResErr();
		goto err5;
	}

	/* validate for the record */
	if (CTBValidate((CoreHandle) hFT, FTSel))
		;

	return(hFT);

err5:
	DisposPtr((**hFT).oldConfig);
err4:
	DisposPtr((**hFT).config);
err3:
	CRMToolPreflight(procID, &theContext);
	CRMReleaseResource(tempH);
	CRMToolPostflight(&theContext);
err2:
	DisposHandle( (Handle) hFT);
err1:
	CRMReleaseRF(procID);
	GetMemErr() = savedMem;				// restore the error values
	GetResErr() = savedRes;
err0:
	return(nil);
}

/* start the file transfer process */
pascal FTErr __FTStart(FTHandle hFT, short direction, SFReply* fileInfo)
{
	FTErr	result;

	(**hFT).errCode = 0;
	/* reset ftSucc and ftIsFTMode */
	(**hFT).flags &= ~(ftSucc | ftIsFTMode);
	(**hFT).direction = direction;
	BlockMove( (Ptr) fileInfo, (Ptr) &(**hFT).theReply, sizeof(SFReply) );

	result = FDEFExecuteFDEF(hFT, ftStartMsg, 0, 0, 0);

	/* assume file transfer will success, enable file transfer */
	if (result == ftNoErr)
		(**hFT).flags |= (ftSucc | ftIsFTMode);

	return(result);
}


/* file transfer's */
pascal void __FTExec(FTHandle hFT)
{
	/* just call the  scheduler in control DEF for next task */
	FDEFExecuteFDEF(hFT,ftExecMsg, 0,  0, 0);
}

/* dispose the  file xfer record */
pascal void __FTDispose(FTHandle hFT)
{
	long	result;
	Handle theHand;
	short	procID;
	CRMToolContext	theContext;

	procID = (**hFT).procID;

	result = FDEFExecuteFDEF(hFT, ftDisposeMsg, 0, 0, 0);

	theHand = RecoverHandle( (Ptr) (**hFT).defProc );
	if (theHand != nil && MemError() == noErr)
	{
		CRMToolPreflight(procID, &theContext);
		CRMReleaseResource( theHand );
		CRMToolPostflight(&theContext);
	}

	DisposPtr((**hFT).config);
	DisposPtr((**hFT).oldConfig);
	DisposHandle( (Handle) hFT);

	CRMReleaseRF(procID);
}


/* CLEANUP the file xfer */
pascal FTErr __FTCleanup(FTHandle hFT, Boolean now)
{	long	flags;

	flags = 0;
	if (now) flags = 1;
	return(FDEFExecuteFDEF(hFT, ftCleanupMsg, 0, flags, 0));
}
