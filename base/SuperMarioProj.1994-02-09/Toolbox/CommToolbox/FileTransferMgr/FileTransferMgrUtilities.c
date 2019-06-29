/*
	File:		FileTransferMgrUtilities.c

	Contains:	Utility routines for the File Transfer Manager and glue

	Written by:	Byron Han

	Copyright:	© 1988-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	  2/7/92	BH		fixed problem with calling FDEFExecuteFDEF with old calling
									conventions for CubeE (we are now aligning all of file transfer
									mgr code to use conventions introduced in <4> by darin,
									essentially	reversing <7> <6> and <5>
		 <7>	11/18/91	DTY		Make the ROM build use the CubeE version of FDEFExecuteFDEF.
		 <6>	 10/4/91	JSM		Change PsychoticFarmerOrLater conditionals to TheFuture.
		 <5>	 10/2/91	DTY		Conditionalise <2> and <4> out of CubeE.
		 <4>	 7/10/91	dba		use a parameter of type FTHandle and figure out the defProc
									prarameter automatically
		 <3>	 7/10/91	BH		rename to FDEFExecuteFDEF
		 <2>	 7/10/91	BH		made FDEFCallFDEF a pascal long instead of untyped routine
		 <1>	 3/14/90	BBH		first checked in

*/

//	This file is INCLUDED by either the manager or the glue code
//	it does NOT stand alone

pascal long
FDEFExecuteFDEF(FTHandle theHandle, short msg, long p1, long p2, long p3)
{
	long			rval;
	CRMToolContext	context;
	pascal long		(*callProc) (FTHandle, short, long, long, long);

	CRMToolPreflight((**theHandle).procID, &context);
	(ProcPtr) callProc = (**theHandle).defProc;
	rval = (*callProc) (theHandle, msg, p1, p2, p3);		/* and away she goes */
	CRMToolPostflight(&context);
	return(rval);
}
