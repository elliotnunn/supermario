{
	FileTransfers.p
	Pascal Interface to the File Transfer Manager
	
	Copyright © Apple Computer, Inc. 1988-90
	All rights reserved
}
{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT FileTransfers;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingFileTransfers}
{$SETC UsingFileTransfers := 1}

{$I+}
{$SETC FileTransfersIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingPackages}
{$I $$Shell(PInterfaces)Packages.p}
{$ENDC}
{$IFC UNDEFINED UsingCTBUtilities}
{$I $$Shell(PInterfaces)CTBUtilities.p}
{$ENDC}

{$SETC UsingIncludes := FileTransfersIncludes}

CONST
{ current file transfer manager version }
	curFTVersion		=	2;
	
{ FTErr }
	ftGenericError		=	-1;
	ftNoErr				=	0;
 	ftRejected			=	1;
 	ftFailed			=	2;
 	ftTimeOut			=	3;
	ftTooManyRetry		=	4;
	ftNotEnoughDSpace	=	5;
	ftRemoteCancel		=	6;
	ftWrongFormat		=	7;
	ftNoTools			=	8;
	ftUserCancel		=	9;
	ftNotSupported		=	10;
	
{ FTFlags }
	ftIsFTMode			=	$00000001;
	ftNoMenus			=	$00000002;
	ftQuiet				=	$00000004;
	ftSucc				=	$00000080;
	
{ FTAttributes }
	ftSameCircuit		=	$0001;
	ftSendDisable 		=	$0002;
	ftReceiveDisable	=	$0004;
	ftTextOnly			=	$0008;
	ftNoStdFile			=	$0010;
	
{ FTDirection }
	ftReceiving			=	0;
	ftTransmitting		=	1;
	ftFullDuplex		=	2;
	
{ FTChooseRec messages	}
	ftChooseNochange	=	1;
	ftChooseBackdoor	=	2;

TYPE
	FTErr				=	OSErr;
	FTFlags				= 	LONGINT;
	FTAttributes		=	INTEGER;
	FTDirection			=	INTEGER;
				
	{ FTChoose data structures }
	FTChooseRecPtr 	=	^FTChooseRec;
	FTChooseRec		=	RECORD
		reserved	:	LONGINT;
		msg			:	LONGINT;
		idleProc	:	ProcPtr;
		filterProc	:	ProcPtr;
		newTool		:	Str63;
		newConfig	:	Ptr;
		eventProc	:	ProcPtr;
	END;
	
	FTHandle			=	^FTPtr;
	FTPtr				=	^FTRecord;
	FTRecord			=	PACKED RECORD
		procID			:	INTEGER;
		
		flags			:	FTFlags;
		errCode			:	FTErr;
		
		refCon			:	LONGINT;
		userData		:	LONGINT;
	
		defProc			:	ProcPtr;
		
		config			:	Ptr;
		oldConfig 		:	Ptr;

		environsProc	:	ProcPtr;
		reserved1		:	LONGINT;		
		reserved2		:	LONGINT;		
		
		ftPrivate		:	Ptr;
	
		sendProc		:  	ProcPtr;
		recvProc		:	ProcPtr;
		writeProc		:	ProcPtr;
		readProc		:	ProcPtr;

		owner			:	WindowPtr;

		direction		:	FTDirection;
		theReply		: 	SFReply;

		writePtr		:	LONGINT;
		readPtr			:	LONGINT;
		theBuf			:	^char;
		bufSize			:	LONGINT;
		autoRec			:	Str255;
		attributes		:	FTAttributes;
	END;
	
FUNCTION 	InitFT: FTErr;
FUNCTION 	FTGetVersion(hFT : FTHandle) : Handle;
FUNCTION 	FTGetFTVersion : INTEGER;
 
FUNCTION 	FTNew(procID: INTEGER; flags: FTFlags; 
				sendProc: ProcPtr; recvProc: ProcPtr;
				readProc: ProcPtr; writeProc: ProcPtr; environsProc: ProcPtr;
				owner: WindowPtr; refCon: LONGINT; userData: LONGINT) : FTHandle;
				
PROCEDURE 	FTDispose(hFT: FTHandle);

FUNCTION 	FTStart(hFT: FTHandle; direction: FTDirection; fileInfo: SFReply) : FTErr;
FUNCTION 	FTAbort(hFT: FTHandle) : FTErr;

PROCEDURE 	FTExec(hFT : FTHandle);

PROCEDURE 	FTActivate(hFT : FTHandle; activate: BOOLEAN);
PROCEDURE 	FTResume(hFT : FTHandle; resume: BOOLEAN);
FUNCTION	FTMenu(hFT: FTHandle; menuID: INTEGER; item: INTEGER): BOOLEAN;

FUNCTION 	FTChoose(VAR hFT: FTHandle; where: Point; idleProc: ProcPtr) : INTEGER;
FUNCTION 	FTPChoose(VAR hFT:FTHandle; where: Point; VAR cRec: FTChooseRec) : INTEGER;
PROCEDURE 	FTEvent(hFT: FTHandle; theEvent: EventRecord);

FUNCTION  	FTValidate(hFT: FTHandle) : BOOLEAN;
PROCEDURE 	FTDefault(VAR theConfig: Ptr; procID: INTEGER; allocate: BOOLEAN);

FUNCTION  	FTSetupPreflight(procID: INTEGER; VAR magicCookie: LONGINT): Handle;
PROCEDURE 	FTSetupSetup(procID: INTEGER; theConfig: Ptr; count: INTEGER; theDialog: DialogPtr;
				VAR magicCookie: LONGINT);
FUNCTION  	FTSetupFilter(procID: INTEGER; theConfig: Ptr;  count: INTEGER; theDialog: DialogPtr;  
				VAR theEvent: EventRecord; VAR theItem: INTEGER; VAR magicCookie: LONGINT) : BOOLEAN;
PROCEDURE 	FTSetupItem(procID: INTEGER; theConfig: Ptr; count: INTEGER; theDialog: DialogPtr; 
				VAR theItem: INTEGER; VAR magicCookie: LONGINT);
PROCEDURE 	FTSetupCleanup(procID: INTEGER; theConfig: Ptr; count: INTEGER; theDialog: DialogPtr;  
				VAR magicCookie: LONGINT);
PROCEDURE 	FTSetupXCleanup(procID:INTEGER; theConfig:Ptr; count:INTEGER; 
				theDialog:DialogPtr; OKed: BOOLEAN; VAR magicCookie:LONGINT);
	
PROCEDURE 	FTSetupPostflight(procID: INTEGER);

FUNCTION  	FTGetConfig(hFT: FTHandle) : Ptr; 
FUNCTION  	FTSetConfig(hFT: FTHandle; thePtr : Ptr) : INTEGER;

FUNCTION 	FTIntlToEnglish(hFT: FTHandle; inputPtr: Ptr; VAR outputPtr: Ptr;
					language: INTEGER) : OSErr; 
FUNCTION 	FTEnglishToIntl(hFT: FTHandle; inputPtr: Ptr; VAR outputPtr: Ptr; 
					language: INTEGER) : OSErr;

PROCEDURE 	FTGetToolName(procID: INTEGER; VAR name: Str255);
FUNCTION  	FTGetProcID(name: Str255) : INTEGER;

PROCEDURE 	FTSetRefCon(hFT: FTHandle; refCon: LONGINT);
FUNCTION  	FTGetRefCon(hFT: FTHandle) : LONGINT;

PROCEDURE 	FTSetUserData(hFT: FTHandle; userData: LONGINT);
FUNCTION  	FTGetUserData(hFT: FTHandle) : LONGINT;

PROCEDURE	FTGetErrorString(hFT: FTHandle; id: INTEGER; VAR errMsg: Str255);
{$ENDC} {UsingFileTransfers}

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

