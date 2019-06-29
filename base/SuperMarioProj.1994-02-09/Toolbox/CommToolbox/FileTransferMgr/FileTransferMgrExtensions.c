/*
	File:		FileTransferMgrExtensions.c

	Contains:	Extensions to the File Transfer Manager

	Written by:	Byron Han

	Copyright:	© 1990-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 8/12/93	kc		<mb>: I deleted all of the "TheFuture" conditionals in all of
									the CommToolbox files per Mike Bell's instructions. I also had
									to delete some old code that was compiled under the "CubeE"
									conditional.
		<11>	  2/7/92	BH		changed define for MANAGERCODE to be TRUE or FALSE to
																		allow FileTransferManagerExtensions.c to properly
									conditionalize	for TheFuture and for CubeE and for building the
									glue file
									
									also fixed problem with calling FDEFExecuteFDEF with old calling
									conventions for CubeE (we are now aligning all of file transfer
									mgr code to use conventions introduced in <7> by darin
		<10>	 11/5/91	DTY		Rename the CubeE version of FTGetErrorString to
									__FTGetErrorString to keep the interface build warningless.
		 <9>	 10/4/91	JSM		Change PsychoticFarmerOrLater to TheFuture.
		 <8>	 10/2/91	DTY		Conditionalise <2> through <4>, and <7> out of CubeE.
		 <7>	 7/10/91	dba		change calling conventions for FDEFExecuteFDEF
		 <6>	 7/10/91	BH		remove #define garbage to avoid duplicate wierd symbols in
									object file - now use Lib in SysObj.make
		 <5>	  7/2/91	BH		FileTransferNotificationProc was renamed
									FileTransferNotificationProcPtr
		 <4>	  7/2/91	BH		FTReceive was sending wrong message to the tool.  Now fixed
		 <3>	  7/1/91	BH		changed __FTGetErrorString error check from if (err != ftNoErr)
									to if (err)
		 <2>	 6/26/91	BH		added FTSend and FTReceive
		 <1>	 3/14/90	BBH		first checked in

*/

//	<11>	changed to do TRUE and FALSE for the define
#ifndef __MANAGERCODE__
#define __MANAGERCODE__		FALSE
#endif __MANAGERCODE__

#if !__MANAGERCODE__
#include "CTBUtilities.h"					/* toolbox interface files				*/

#include "FileTransfers.h"
#include "FileTransferTools.h"
#include "CommResources.h"
#include "CommToolboxPriv.h"

//#define FDEFExecuteFDEF		FT_PROC0
//#define __FTGetErrorString 	FT_PROC1
//#define __FTSend				FT_PROC2
//#define __FTReceive		 	FT_PROC3

#include "FileTransferMgrUtilities.c"		// for FDEFExecuteDef

#endif __MANAGERCODE__

// Get a single resource from the specified tool's resource fork.
// Returns NIL if not found.



//	These routines are INCLUDED by either the manager or the glue code
//	it does NOT stand alone

pascal FTErr
__FTSend(FTHandle hFT, short numFiles, FSSpecArrayPtr pFSpec,
					FileTransferNotificationProcPtr notifyProc)
{
	FTErr	rval;

	rval = (FTErr)FDEFExecuteFDEF(hFT, ftSendMsg,
				numFiles, (long) pFSpec, (long)notifyProc);

	return rval;

}

pascal FTErr
__FTReceive(FTHandle hFT, FSSpecPtr pFSpec,
					FileTransferNotificationProcPtr notifyProc)
{
	FTErr	rval;

	rval = (FTErr)FDEFExecuteFDEF(hFT, ftReceiveMsg,
				(long) pFSpec, (long)notifyProc, 0);

	return rval;

}
