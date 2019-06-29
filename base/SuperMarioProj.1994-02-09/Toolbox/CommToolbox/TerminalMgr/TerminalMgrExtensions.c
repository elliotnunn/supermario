/*
	File:		TerminalMgrExtensions.c

	Contains:	Extensions to the Terminal Manager

	Written by:	Byron Han

	Copyright:	© 1990-1991, 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/12/93	kc		<mb>: I deleted all of the "TheFuture" conditionals in all of
									the CommToolbox files per Mike Bell's instructions. I also had
									to delete some old code that was compiled under the "CubeE"
									conditional.
		 <5>	 10/4/91	JSM		Change PsychoticFarmerOrLater conditionals to TheFuture.
		 <4>	 10/2/91	DTY		Conditionalise <2> out of CubeE.
		 <3>	 7/10/91	BH		remove #define garbage to avoid duplicate wierd symbols in
									object file - now use Lib in SysObj.make
		 <2>	  7/1/91	BH		Renamed TMGetErrorString to __TMGetErrorString to avoid
									conflicts with glue.  also changed error check from if (err !=
									tmNoErr) to if (err)
		 <1>	 3/14/90	BBH		first checked in

*/

#ifndef __MANAGERCODE__

#include "CTBUtilities.h"					/* toolbox interface files				*/

#include "Terminals.h"
#include "TerminalTools.h"
#include "CommResources.h"
#include "CommToolboxPriv.h"

//#define TDEFExecuteDef		TM_PROC0
//#define __TMGetErrorString 	TM_PROC1

#include "TerminalMgrUtilities.c"			// for TDEFExecuteDef
#endif __MANAGERCODE__

// Get a single resource from the specified tool's resource fork.
// Returns NIL if not found.
pascal void 
__TMGetErrorString (hTerm, id, errMsg)
	TermHandle hTerm;
	short id;
	Str255 errMsg;
{	
	TMErr	rval;
	
	rval = (TMErr)TDEFExecuteDef( (long)hTerm, (long)(**hTerm).defProc, tmGetErrorStringMsg, 
				id, (long) errMsg, 0 );

	if (rval) 
		errMsg[0] = (char) 0;					// oops not supported
}
