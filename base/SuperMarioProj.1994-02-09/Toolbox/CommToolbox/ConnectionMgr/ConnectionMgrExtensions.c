/*
	File:		ConnectionMgrExtensions.c

	Contains:	Extensions to the Connection Manager

	Written by:	Byron Han

	Copyright:	© 1988-1991, 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/12/93	kc		<mb>: I deleted all of the "TheFuture" conditionals in all of
									the CommToolbox files per Mike Bell's instructions. I also had
									to delete some old code that was compiled under the "CubeE"
									conditional.
		<13>	 11/5/91	DTY		Add __ to the front of the routines brought back for CubeE to
									keep the build warningless.
		<12>	 10/4/91	JSM		Change PsychoticFarmerOrLater to TheFuture.
		<11>	 10/2/91	DTY		Conditionalise <7> and <9> out of CubeE.
		<10>	 7/10/91	BH		remove #define garbage to avoid duplicate wierd symbols in
									object file - now use Lib in SysObj.make
		 <9>	  7/1/91	BH		changed error check in __CMGetErrorString to if (err) instead of
									if (err == cmNotSupported)
		 <8>	 6/26/91	BH		added __ to distinguish from glue
		 <7>	 6/18/91	BH		removed wierd new Open/Listen calls
		 <6>	 1/14/91	CP		;Removed CMConnectPrep and added CMListenPrep and CMOpenPrep.
									;Renamed CMFreeIORec to CMDisposeIOPB.
									;Renamed CMAllocateIORec to CMNewIOPB.
									;Save lock status of connection handle in flags field and then
									lock the connection
									 handle in CMListenPrep and CMOpenPrep.
									;Unlock connection handle in CMConnectCleanup if not locked
									before call to CMListenPrep or CMOpenPrep.
									;Removed param block from CMPBAccept, CMPBListen, CMPBClose,
									CMPBOpen, and CMConnectCleanup.
									;Added async, completor, and timeout parameters to CMListenPrep
									and CMOpenPrep.
									;Renamed CMIORec to CMIOPB <BBH>
		 <5>	11/27/90	kaz		Renaming CMHCalls to CMPBCalls; added CMPBOpen, etc.  <jng>
		 <4>	  6/4/90	BBH		include new ConnectionsExtensions.h file
		 <3>	 3/20/90	BBH		make CMGetErrorString ONLY nil out the return string if
									cmNotSupported is returned
		 <2>	 3/14/90	BBH		correct typo in inclusion of ConnectionMgrExtensions
		 <1>	 3/14/90	BBH		first checked in

*/

#ifdef __MANAGERCODE__
#else

#include "CTBUtilities.h"					/* toolbox interface files				*/

#include "Connections.h"
#include "ConnectionsExtensions.h"
#include "ConnectionTools.h"
#include "CommResources.h"
#include "ConnectionsPriv.h"					/* Connection Manager private includes 	*/
#include "CommToolboxPriv.h"

//#define CDEFExecuteDef		CM_PROC0
//#define __CMGetErrorString 	CM_PROC1
//#define __CMNewIOPB		 	CM_PROC2
//#define __CMDisposeIOPB		CM_PROC3
//#define __CMPBRead			CM_PROC4
//#define __CMPBWrite			CM_PROC5
//#define __CMPBIOKill		 	CM_PROC6

#include "ConnectionMgrUtilities.c"		// for CDEFExecuteDef

#endif __MANAGERCODE__

// Get a single resource from the specified tool's resource fork.
// Returns NIL if not found.
pascal void __CMGetErrorString (hConn, id, errMsg)
	ConnHandle hConn;
	short id;
	Str255 errMsg;
{	
	CMErr	rval;
	
	rval = (CMErr)CDEFExecuteDef( (long)hConn, (long)(**hConn).defProc, cmGetErrorStringMsg, 
				id, (long) errMsg, 0 );

	if (rval) 									// was != cmNoErr
		errMsg[0] = (char) 0;					// oops not supported
}

pascal CMErr  __CMNewIOPB(hConn, theIOPB)
	ConnHandle 	hConn;
	CMIOPBPtr	*theIOPB;
{
	CMErr			rval;
	
	rval = (CMErr)CDEFExecuteDef( (long)hConn, (long)(**hConn).defProc, cmNewIOPBMsg, 
			(long) theIOPB, 0, 0 );
	return(rval);
}

pascal CMErr  __CMDisposeIOPB(hConn, theIOPB)
	ConnHandle 	hConn;
	CMIOPBPtr	theIOPB;
{
	CMErr			rval;
	
	rval = (CMErr)CDEFExecuteDef( (long)hConn, (long)(**hConn).defProc, cmDisposeIOPBMsg, 
			(long) theIOPB, 0, 0 );
	return(rval);
}

pascal CMErr  __CMPBRead(hConn, theIOPB, async)
	ConnHandle 	hConn;
	CMIOPBPtr	theIOPB;
	Boolean		async;
{
	CMErr			rval;
	pascal long		(*callProc) (ConnHandle, short, long, long, long);

	(ProcPtr) callProc = (**hConn).defProc;
	rval = (CMErr)(*callProc) (hConn, cmPBReadMsg, 
			(long) theIOPB, (async ? 1 : 0), 0 );

	return(rval);
}

pascal CMErr  __CMPBWrite(hConn, theIOPB, async)
	ConnHandle 	hConn;
	CMIOPBPtr	theIOPB;
	Boolean 	async;
{
	CMErr			rval;
	pascal long		(*callProc) (ConnHandle, short, long, long, long);
	
	(ProcPtr) callProc = (**hConn).defProc;
	rval = (CMErr)(*callProc) ( hConn, cmPBWriteMsg, 
			(long) theIOPB, (async ? 1 : 0), 0 );
	return(rval);
}

pascal CMErr  __CMPBIOKill(hConn, theIOPB)
	ConnHandle 	hConn;
	CMIOPBPtr	theIOPB;
{
	CMErr			rval;
	pascal long		(*callProc) (ConnHandle, short, long, long, long);
	
	(ProcPtr) callProc = (**hConn).defProc;
	rval = (CMErr)(*callProc) ( hConn,  cmPBIOKillMsg, 
			(long) theIOPB, 0, 0 );
	return(rval);
}
