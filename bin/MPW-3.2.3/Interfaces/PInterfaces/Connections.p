{
	Connections.p
	Pascal Interface to the Connection Manager
	
	Copyright © Apple Computer, Inc. 1988-90
	All rights reserved
}
{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT Connections;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingConnections}
{$SETC UsingConnections := 1}

{$I+}
{$SETC ConnectionsIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingDialogs}
{$I $$Shell(PInterfaces)Dialogs.p}
{$ENDC}

{$IFC UNDEFINED UsingCTBUtilities}
{$I $$Shell(PInterfaces)CTBUtilities.p}
{$ENDC}

{$SETC UsingIncludes := ConnectionsIncludes}

CONST
{ current Connection Manager version }
	curCMVersion		=	2;

{ current Connection Manager Environment Record version }
	curConnEnvRecVers	=	0;
	
{ error codes }
	cmGenericError		=	-1;
	cmNoErr				=	0;
	cmRejected 			=	1;
	cmFailed			=	2;
	cmTimeOut			=	3;
	cmNotOpen			=	4;
	cmNotClosed			=	5;
	cmNoRequestPending	=	6;
	cmNotSupported		=	7;
	cmNoTools			=	8;
	cmUserCancel		=	9;
	cmUnknownError		=	11;

{ CMRecFlags / CMChannel}
{	Low word of CMRecFlags is same as CMChannel }
	cmData				= 	$00000001;
	cmCntl				= 	$00000002;
	cmAttn				= 	$00000004;

	cmDataNoTimeout		= 	$00000010;  
	cmCntlNoTimeout		= 	$00000020;
	cmAttnNoTimeout		= 	$00000040;

	cmDataClean			= 	$00000100;  
	cmCntlClean			= 	$00000200;
	cmAttnClean			= 	$00000400;

										{	only for CMRecFlags (not CMChannel) below this point }
	cmNoMenus			= 	$00010000;
	cmQuiet				= 	$00020000;

{ CMStatFlags}
	cmStatusOpening		= 	$00000001;
	cmStatusOpen		= 	$00000002;
	cmStatusClosing		= 	$00000004;
	cmStatusDataAvail	= 	$00000008;
	cmStatusCntlAvail	= 	$00000010;
	cmStatusAttnAvail	= 	$00000020;

	cmStatusDRPend		= 	$00000040;		{data read pending}
	cmStatusDWPend		= 	$00000080;		{data write pending}
	cmStatusCRPend		= 	$00000100;		{cntl read pending}
	cmStatusCWPend		= 	$00000200;		{cntl write pending}
	cmStatusARPend		= 	$00000400;		{attn read pending}
	cmStatusAWPend		= 	$00000800;		{attn write pending}

	cmStatusBreakPend 	=	$00001000;
	cmStatusListenPend 	= 	$00002000;
	cmStatusIncomingCallPresent	
						=	$00004000;

	cmStatusReserved0	=	$00008000;

{ CMSearchFlags}
	cmSearchSevenBit	= 	$0001;
	
{ CMFlags}
	cmFlagsEOM			= 	$0001;
	
TYPE
	CMErr				=	OSErr;
	
	CMBufFields = (
		cmDataIn,
		cmDataOut,
	
		cmCntlIn,
		cmCntlOut,
	
		cmAttnIn,
		cmAttnOut,
		
		cmRsrvIn,
		cmRsrvOut
	);
	
	CMBuffers		=	ARRAY[CMBufFields] OF Ptr;
	CMBufferSizes	=	ARRAY[CMBufFields] OF LONGINT;
	
	CMStatFlags 	= 	LONGINT;
	CMRecFlags 		= 	LONGINT;
	CMChannel 		= 	INTEGER;
	CMSearchFlags 	= 	INTEGER;
	CMFlags 		= 	INTEGER;

	ConnEnvironRecPtr 
					= 	^ConnEnvironRec;
	ConnEnvironRec 	=	RECORD
		version			:	INTEGER;
		baudRate		:	LONGINT;
		dataBits		:	INTEGER;
		channels		:	CMChannel;
		swFlowControl	:	BOOLEAN;
		hwFlowControl	:	BOOLEAN;
		flags			:	CMFlags;
	END;
	
	ConnHandle		=	^ConnPtr;
	ConnPtr			=	^ConnRecord;
	ConnRecord		=	RECORD
		procID		:	INTEGER;
		
		flags		:	CMRecFlags;
		errCode		:	CMErr;
		
		refCon		:	LONGINT;
		userData	:	LONGINT;

		defProc		:	ProcPtr;

		config		:	Ptr;
		oldConfig	:	Ptr;

		asyncEOM	:	LONGINT;
		
		reserved1	:	LONGINT;
		reserved2	:	LONGINT;

		cmPrivate	:	Ptr;
		
		bufferArray	:	CMBuffers;
		bufSizes	:	CMBufferSizes;
				
		mluField	:	LONGINT;
		
		asyncCount 	:	CMBufferSizes;
	END;
	
	

FUNCTION 	InitCM: CMErr;
FUNCTION 	CMGetVersion(hConn: ConnHandle): Handle;
FUNCTION 	CMGetCMVersion: INTEGER;

FUNCTION  	CMNew(procID: INTEGER; flags: CMRecFlags; 
					desiredSizes: CMBufferSizes; refCon: LONGINT; 
					userData: LONGINT): ConnHandle;
PROCEDURE 	CMDispose(hConn: ConnHandle);

FUNCTION 	CMListen(hConn: ConnHandle; async: BOOLEAN; completor: ProcPTr; 
					timeout: LONGINT): CMErr;
FUNCTION 	CMAccept(hConn: ConnHandle; accept: BOOLEAN): CMErr;

FUNCTION 	CMOpen(hConn: ConnHandle; async: BOOLEAN; completor: ProcPtr; 
					timeout: LONGINT): CMErr;
FUNCTION 	CMClose(hConn: ConnHandle; async: BOOLEAN; completor: ProcPtr; 
					timeout: LONGINT; now: BOOLEAN): CMErr;

FUNCTION 	CMAbort(hConn: ConnHandle): CMErr;

FUNCTION 	CMStatus(hConn: ConnHandle; VAR sizes: CMBufferSizes; 
					VAR flags: CMStatFlags): CMErr;
PROCEDURE 	CMIdle(hConn: ConnHandle);

PROCEDURE 	CMReset(hConn: ConnHandle);

PROCEDURE 	CMBreak(hConn: ConnHandle; duration: LONGINT; async: BOOLEAN;  
					completor: ProcPtr);

FUNCTION 	CMRead(hConn: ConnHandle; theBuffer: Ptr; VAR toRead: LONGINT; 
					theChannel: CMChannel; async: BOOLEAN;  
					completor: ProcPtr; timeout: LONGINT; VAR flags: CMFlags): CMErr;
FUNCTION 	CMWrite(hConn: ConnHandle; theBuffer: Ptr; VAR toWrite: LONGINT; 
					theChannel: CMChannel; async: BOOLEAN; 
					completor: ProcPtr; timeout: LONGINT; flags: CMFlags): CMErr;
FUNCTION 	CMIOKill(hConn: ConnHandle; which: INTEGER): CMErr;

PROCEDURE 	CMActivate(hConn: ConnHandle; activate: BOOLEAN);
PROCEDURE 	CMResume(hConn: ConnHandle; resume: BOOLEAN);
FUNCTION 	CMMenu(hConn: ConnHandle; menuID: INTEGER; item: INTEGER): BOOLEAN;

FUNCTION 	CMValidate(hConn: ConnHandle): BOOLEAN;
PROCEDURE 	CMDefault(VAR theConfig: Ptr ; procID: INTEGER; allocate: BOOLEAN);

FUNCTION 	CMSetupPreflight(procID: INTEGER; VAR magicCookie: LONGINT):Handle;
PROCEDURE 	CMSetupSetup(procID: INTEGER; theConfig: Ptr; count: INTEGER; theDialog: DialogPtr;
						VAR magicCookie:LONGINT);
FUNCTION 	CMSetupFilter(procID: INTEGER; theConfig: Ptr; count: INTEGER; 
						theDialog: DialogPtr; VAR theEvent: EventRecord; VAR theItem: INTEGER;
						VAR magicCookie: LONGINT): BOOLEAN;
PROCEDURE 	CMSetupItem(procID: INTEGER; theConfig: Ptr; count: INTEGER; theDialog: DialogPtr;
						VAR theItem: INTEGER; VAR magicCookie: LONGINT);
PROCEDURE 	CMSetupCleanup(procID: INTEGER; theConfig: Ptr; count: INTEGER; theDialog: DialogPtr;
						VAR magicCookie: LONGINT);
PROCEDURE	CMSetupPostflight(procID: INTEGER);

FUNCTION 	CMGetConfig(hConn: ConnHandle): Ptr;
FUNCTION 	CMSetConfig(hConn: ConnHandle; thePtr: Ptr): INTEGER;

FUNCTION 	CMIntlToEnglish(hConn: ConnHandle; inputPtr: Ptr; VAR outputPtr: Ptr; 
					language: INTEGER): OSErr;
FUNCTION 	CMEnglishToIntl(hConn: ConnHandle; inputPtr: Ptr; VAR outputPtr: Ptr; 
					language: INTEGER): OSErr;

FUNCTION 	CMAddSearch(hConn: ConnHandle; theString: Str255; flags: CMSearchFlags;
					callBack: ProcPtr): LONGINT;
PROCEDURE 	CMRemoveSearch(hConn: ConnHandle; refnum: LONGINT);
PROCEDURE 	CMClearSearch(hConn: ConnHandle);

FUNCTION 	CMGetConnEnvirons(hConn: ConnHandle; VAR theEnvirons: ConnEnvironRec): CMErr;

FUNCTION 	CMChoose(VAR hConn: ConnHandle; where: Point; idleProc: ProcPtr): INTEGER;

PROCEDURE 	CMEvent(hConn: ConnHandle; theEvent: EventRecord);
					
PROCEDURE 	CMGetToolName(procID: INTEGER; VAR name: Str255);
FUNCTION  	CMGetProcID(name: Str255): INTEGER;

PROCEDURE 	CMSetRefCon(hConn: ConnHandle; refCon: LONGINT);
FUNCTION 	CMGetRefCon(hConn: ConnHandle): LONGINT;

PROCEDURE 	CMSetUserData(hConn: ConnHandle; userData: LONGINT);
FUNCTION 	CMGetUserData(hConn: ConnHandle): LONGINT;


{$ENDC} {UsingConnections}

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

