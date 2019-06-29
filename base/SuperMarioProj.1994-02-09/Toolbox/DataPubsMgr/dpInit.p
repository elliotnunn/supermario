{
	File:		dpInit.p

	Contains:	Code for Initializing each part of the Edition Manager 
				and cleaning up up after applications quit
				
	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 5/31/90	ngk		use MFprivate
		 <4>	  4/7/90	ngk		update USES
		 <3>	 3/10/90	ngk		Fixed order of USES files.
		 <2>	  1/6/90	ngk		Change USES to use Editions.p and EditionPrivate.p Added
									dp_GetEditionOpenerProc and dp_SetEditionOpenerProc Added
									dp_GetLastEditionContainerUsed Renamed dp_InitEditionPack and
									dp_QuitEditionPack
	   <1.7>	11/13/89	ngk		Cleaned up dependencies
	   <1.6>	10/25/89	ngk		Now USEs Files
	   <1.5>	10/13/89	ngk		Now USEs Resources
	   <1.4>	 10/2/89	ngk		EASE sucks
	  <•1.3>	 10/2/89	ngk		nothing
	   <1.2>	  8/8/89	ngk		Changed Quit to return an OSErr.
	   <1.1>	 5/29/89	ngk		changed InitDP to check package version instead of the glue
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}

UNIT dpInit;


INTERFACE

USES	
	{ • public Interface files }
	StandardEqu, Editions, Aliases,
		
	{ • private Interface files }
	EditionsPrivate, MFprivate,
	
	{ • other Edition Manager Units }
	dpMisc, dpPubControlBlock;
	
	
	FUNCTION dp_InitEditionPack(versionAppKnows:INTEGER): OSErr; { called once by each app at start }
	
	FUNCTION dp_QuitEditionPack: OSErr; 	{ called by MultiFinder each time an app terminates }
	
	FUNCTION dp_GetEditionOpenerProc(VAR appsEmulator: EditionOpenerProcPtr): OSErr;
	
	FUNCTION dp_SetEditionOpenerProc(appsEmulator: EditionOpenerProcPtr): OSErr;
	

	
IMPLEMENTATION

{$I dpCompileOptions.inc.p }
{$I dpInit.inc.p }

END. { dpInit }


