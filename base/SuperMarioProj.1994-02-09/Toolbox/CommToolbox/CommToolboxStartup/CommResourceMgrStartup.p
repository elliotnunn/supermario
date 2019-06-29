{
	File:		CommResourceMgrStartup.p

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 10/2/90	kaz		Changing reserved to appList for Sys 7 in SetupBlock(). <jng>
		 <2>	 3/16/90	BBH		made it compile
		 <1>	 3/14/90	BBH		first checked in

	To Do:
}

UNIT CRMInit;
{	
	Communications Resource Manager INIT
	
	allocates the memory off of our low memory location ($BB4)
	
	Copyright © 1988-1990 by Apple Computer, Inc.  All rights reserved.
	
	Byron Han, esquire.  6 September 1988.
	
	First version ready for rolling:			19 September 1988
	Thanks to The Art of Noise.
	
	MODIFICATION HISTORY:
	7/18/89		JNG		Moved check of CTBtrap to before the calls to InitCRM & InitCTBUtils
}

INTERFACE
USES
	MemTypes, QuickDraw, OSIntf, ToolIntf, PackIntf,
	Traps, SysEqu,
	CTBUtilities,
	CommToolboxPriv,
	CommResources;

PROCEDURE CRMInitEntry;

IMPLEMENTATION

FUNCTION Installed : BOOLEAN;			FORWARD;
FUNCTION Allocated : BOOLEAN;			FORWARD;
FUNCTION ObsoleteVersion : BOOLEAN;		FORWARD;
PROCEDURE AllocateBlock;				FORWARD;
PROCEDURE SetupBlock;					FORWARD;
PROCEDURE DoFiles;						FORWARD;

PROCEDURE CRMInitEntry;
VAR
	theHandle : Handle;
	
	masterPtr : CTBBlockPtr;
	err:	CRMErr;
	
BEGIN
	theHandle := RecoverHandle(@CRMInitEntry);
	HLock(theHandle);

	IF NOT Installed THEN			{Is this call necessary???  Well, it doesn't do any harm}
		Exit(CRMInitEntry);

	err := InitCTBUtilities;
	err := InitCRM;
			
	IF NOT Allocated THEN
		BEGIN
		AllocateBlock;
		SetupBlock;
		IF NOT Allocated THEN
			Exit(CRMInitEntry);
		END
	ELSE
		BEGIN
		IF ObsoleteVersion THEN
			BEGIN
			BlockMove(Ptr(CommToolboxGlobals), Ptr(@masterPtr), SIZEOF(Ptr));
			DisposPtr(Ptr(masterPtr));
			
			AllocateBlock;
			SetupBlock; 
			END;
		END;
	
	DoFiles;
END;

PROCEDURE DoFiles;
VAR
	err:		OSErr;
	vRefNum:	INTEGER;
	dirID:		LONGINT;
	
BEGIN
	err := CRMFindCommunications(vRefNum, dirID);
	IF err <> noErr THEN
		err := CRMCreateCommunications(vRefNum, dirID);
END;

FUNCTION Allocated : BOOLEAN;
TYPE
	LongPtr = ^LONGINT;
	
VAR
	thePtr  : LONGINT;
	
BEGIN
	thePtr := LongPtr(CommToolboxGlobals)^;
	Allocated := (thePtr <> -1);
END;

FUNCTION ObsoleteVersion : BOOLEAN;
VAR
	thePtr : CTBBlockPtr;
	
BEGIN
	thePtr := CTBGlobalPtr(CommToolboxGlobals)^;
	WITH thePtr^ DO
		BEGIN
		ObsoleteVersion := (version < CTBBlockVersion);
		END;
END;

PROCEDURE AllocateBlock;
VAR
	thePtr : CTBBlockPtr;
	
BEGIN
	thePtr := CTBBlockPtr(NewPtrSys(SIZEOF(CTBBlock)));
	WITH thePtr^ DO
		BEGIN
		version := CTBBlockVersion;
		END; {block}
	BlockMove(Ptr(@thePtr), Ptr(CommToolboxGlobals), SIZEOF(Ptr));
END;

PROCEDURE SetupBlock;							{ this sets up the global block for the commtoolbox }
VAR
	thePtr : CTBBlockPtr;
	theQueueHeader : QHdrPtr;
	
BEGIN
	thePtr := CTBGlobalPtr(CommToolboxGlobals)^;
	WITH thePtr^ DO
		BEGIN
		version := CTBBlockVersion;
		CRMQueue := QHdrPtr(NewPtrSys(SIZEOF(QHdr)));
		resList := 0;
		WITH CRMQueue^ DO BEGIN
			QFlags := 0;
			QHead := NIL;
			QTail := NIL;
		END;
		
		private := 0;
{$IFC PRE70}
		reserved := 0;
{$ELSEC}
		appList := NIL;
{$ENDC}
		resFiles := NIL;
		toolResChain := NIL;	
		END; {block}
END;

FUNCTION Installed : BOOLEAN;
CONST
	UnimplementedTrapNumber = $9F;
	
BEGIN
	Installed := (NGetTrapAddress(UnimplementedTrapNumber, OSTrap) <> NGetTrapAddress(_CommToolboxDispatch, OSTrap));
END;

END.
