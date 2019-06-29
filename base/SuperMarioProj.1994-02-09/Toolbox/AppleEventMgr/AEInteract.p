{
	File:		AEInteract.p

	Copyright:	© 1990-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <2>	 5/21/91	gbm		Stop using the "empty" units (MemTypes, etc...)
		 <1>	12/13/90	Lai		first checked in

	To Do:
}

UNIT AEInteract;

INTERFACE

USES
	Types, Memory, Resources,
	AppleEvents, AEHashTable, AEUtil, AEDFWrapper, AEDF;

	FUNCTION AE_InteractWithUser(timeOutInTicks: LONGINT; { how long are you willing to wait }
								 nmReqPtr: NMRecPtr; { your own custom notification }
								 idleProc: IdleProcPtr { your own idle procedure }
								 ): OSErr; { return the error }

	
	FUNCTION AE_ResetTimer(VAR reply:AppleEvent): OSErr;{ VAR is for efficiency only }
	{ Convience routine.  Create and send a 'wait' message for the }
	{ current reply. }

IMPLEMENTATION

{$I AEInteract.inc1.p}

END. { AEMisc }