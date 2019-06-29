{
Created: Sunday, January 6, 1991 at 10:55 PM
    PPCToolBox.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1989-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT PPCToolBox;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingPPCToolBox}
{$SETC UsingPPCToolBox := 1}

{$I+}
{$SETC PPCToolBoxIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingAppleTalk}
{$I $$Shell(PInterfaces)AppleTalk.p}
{$ENDC}
{$IFC UNDEFINED UsingMemory}
{$I $$Shell(PInterfaces)Memory.p}
{$ENDC}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$SETC UsingIncludes := PPCToolBoxIncludes}

TYPE
PPCServiceType = SignedByte;

CONST

{  service Type  }
ppcServiceRealTime = 1;

TYPE
PPCLocationKind = INTEGER;

CONST

{ lookup Type }
ppcNoLocation = 0;                          { There is no PPCLocName }
ppcNBPLocation = 1;                         { Use AppleTalk NBP      }
ppcNBPTypeLocation = 2;                     { Used for specifying a location name type during PPCOpen only }

TYPE
PPCPortKinds = INTEGER;

CONST

{ port Types }

ppcByCreatorAndType = 1;                    { PortType is specified as colloquial Mac creator and type }
ppcByString = 2;                            { Port Type is in pascal string format }

TYPE
PPCSessionOrigin = SignedByte;              { Values returned for request field in PPCInform call }

CONST

{ Values returned for requestType field in PPCInform call }
ppcLocalOrigin = 1;                         { session originated from this machine }
ppcRemoteOrigin = 2;                        { session originated from remote machine }

TYPE
PPCPortRefNum = INTEGER;
PPCSessRefNum = LONGINT;

PPCPortPtr = ^PPCPortRec;
PPCPortRec = RECORD
    nameScript: ScriptCode;                 { script of name }
    name: Str32;                            { name of port as seen in browser }
    portKindSelector: PPCPortKinds;         { which variant }
    CASE PPCPortKinds OF
      ppcByString:
        (portTypeStr: Str32);
      ppcByCreatorAndType:
        (portCreator: OSType;
        portType: OSType);
    END;

LocationNamePtr = ^LocationNameRec;
LocationNameRec = RECORD
    locationKindSelector: PPCLocationKind;  { which variant }
    CASE PPCLocationKind OF
      ppcNBPLocation:
        (nbpEntity: EntityName);            { NBP name entity }
      ppcNBPTypeLocation:
        (nbpType: Str32);                   { just the NBP type string, for PPCOpen }
    END;

PortInfoPtr = ^PortInfoRec;
PortInfoRec = RECORD
    filler1: SignedByte;
    authRequired: BOOLEAN;
    name: PPCPortRec;
    END;


PortInfoArrayPtr = ^PortInfoArray;
PortInfoArray = ARRAY [0..0] OF PortInfoRec;
PPCFilterProcPtr = ProcPtr;
{Procedures you will need to write 
 ex: void MyCompletionRoutine(PPCParamBlkPtr pb) 
 ex: pascal Boolean    MyPortFilter(LocationNamePtr locationName, PortInfoPtr 
 thePortInfo)}

PPCCompProcPtr = ProcPtr;

PPCOpenPBPtr = ^PPCOpenPBRec;
PPCOpenPBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;      { reserved }
    portRefNum: PPCPortRefNum;              { Port Reference }
    filler1: LONGINT;
    serviceType: PPCServiceType;
    resFlag: SignedByte ;
    portName: PPCPortPtr;                   { PortName for PPC }
    locationName: LocationNamePtr;
    networkVisible: BOOLEAN;
    nbpRegistered: BOOLEAN;
    END;

PPCInformPBPtr = ^PPCInformPBRec;
PPCInformPBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;
    portRefNum: PPCPortRefNum;
    sessRefNum: PPCSessRefNum;
    serviceType: PPCServiceType;
    autoAccept: BOOLEAN;
    portName: PPCPortPtr;
    locationName: LocationNamePtr;
    userName: StringPtr;
    userData: LONGINT;
    requestType: PPCSessionOrigin;
    END;

PPCStartPBPtr = ^PPCStartPBRec;
PPCStartPBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;
    portRefNum: PPCPortRefNum;
    sessRefNum: PPCSessRefNum;
    serviceType: PPCServiceType;
    resFlag: SignedByte;
    portName: PPCPortPtr;
    locationName: LocationNamePtr;
    rejectInfo: LONGINT;
    userData: LONGINT;
    userRefNum: LONGINT;
    END;

PPCAcceptPBPtr = ^PPCAcceptPBRec;
PPCAcceptPBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;
    filler1: INTEGER;
    sessRefNum: PPCSessRefNum;
    END;

PPCRejectPBPtr = ^PPCRejectPBRec;
PPCRejectPBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;
    filler1: INTEGER;
    sessRefNum: PPCSessRefNum;
    filler2: INTEGER;
    filler3: LONGINT;
    filler4: LONGINT;
    rejectInfo: LONGINT;
    END;

PPCWritePBPtr = ^PPCWritePBRec;
PPCWritePBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;
    filler1: INTEGER;
    sessRefNum: PPCSessRefNum;
    bufferLength: Size;
    actualLength: Size;
    bufferPtr: Ptr;
    more: BOOLEAN;
    filler2: SignedByte;
    userData: LONGINT;
    blockCreator: OSType;
    blockType: OSType;
    END;

PPCReadPBPtr = ^PPCReadPBRec;
PPCReadPBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;
    filler1: INTEGER;
    sessRefNum: PPCSessRefNum;
    bufferLength: Size;
    actualLength: Size;
    bufferPtr: Ptr;
    more: BOOLEAN;
    filler2: SignedByte;
    userData: LONGINT;
    blockCreator: OSType;
    blockType: OSType;
    END;

PPCEndPBPtr = ^PPCEndPBRec;
PPCEndPBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;
    filler1: INTEGER;
    sessRefNum: PPCSessRefNum;
    END;

PPCClosePBPtr = ^PPCClosePBRec;
PPCClosePBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;
    portRefNum: PPCPortRefNum;
    END;

IPCListPortsPBPtr = ^IPCListPortsPBRec;
IPCListPortsPBRec = RECORD
    qLink: Ptr;
    csCode: INTEGER;
    intUse: INTEGER;
    intUsePtr: Ptr;
    ioCompletion: PPCCompProcPtr;
    ioResult: OSErr;
    reserved: ARRAY [1..5] OF LONGINT;
    filler1: INTEGER;
    startIndex: INTEGER;
    requestCount: INTEGER;
    actualCount: INTEGER;
    portName: PPCPortPtr;
    locationName: LocationNamePtr;
    bufferPtr: PortInfoArrayPtr;
    END;

    PPCParamBlockPtr = ^PPCParamBlockRec;
    PPCParamBlockRec = RECORD
                        CASE Integer OF
                            0: (openParam:         PPCOpenPBRec);
                            1: (informParam:    PPCInformPBRec);
                            2: (startParam:        PPCStartPBRec);
                            3: (acceptParam:    PPCAcceptPBRec);
                            4: (rejectParam:    PPCRejectPBRec);
                            5: (writeParam:        PPCWritePBRec);
                            6: (readParam:         PPCReadPBRec);
                            7: (endParam:        PPCEndPBRec);
                            8: (closeParam:        PPCClosePBRec);
                            9: (listPortsParam: IPCListPortsPBRec);
                    END;



{  PPC Calling Conventions  }
FUNCTION PPCInit: OSErr;
    INLINE $7000,$A0DD,$3E80;
FUNCTION PPCOpen(pb: PPCOpenPBPtr;async: BOOLEAN): OSErr;
FUNCTION PPCOpenSync(pb: PPCOpenPBPtr): OSErr;
    INLINE $205F,$7001,$A0DD,$3E80;
FUNCTION PPCOpenAsync(pb: PPCOpenPBPtr): OSErr;
    INLINE $205F,$7001,$A4DD,$3E80;
FUNCTION PPCInform(pb: PPCInformPBPtr;async: BOOLEAN): OSErr;
FUNCTION PPCInformSync(pb: PPCInformPBPtr): OSErr;
    INLINE $205F,$7003,$A0DD,$3E80;
FUNCTION PPCInformAsync(pb: PPCInformPBPtr): OSErr;
    INLINE $205F,$7003,$A4DD,$3E80;
FUNCTION PPCStart(pb: PPCStartPBPtr;async: BOOLEAN): OSErr;
FUNCTION PPCStartSync(pb: PPCStartPBPtr): OSErr;
    INLINE $205F,$7002,$A0DD,$3E80;
FUNCTION PPCStartAsync(pb: PPCStartPBPtr): OSErr;
    INLINE $205F,$7002,$A4DD,$3E80;
FUNCTION PPCAccept(pb: PPCAcceptPBPtr;async: BOOLEAN): OSErr;
FUNCTION PPCAcceptSync(pb: PPCAcceptPBPtr): OSErr;
    INLINE $205F,$7004,$A0DD,$3E80;
FUNCTION PPCAcceptAsync(pb: PPCAcceptPBPtr): OSErr;
    INLINE $205F,$7004,$A4DD,$3E80;
FUNCTION PPCReject(pb: PPCRejectPBPtr;async: BOOLEAN): OSErr;
FUNCTION PPCRejectSync(pb: PPCRejectPBPtr): OSErr;
    INLINE $205F,$7005,$A0DD,$3E80;
FUNCTION PPCRejectAsync(pb: PPCRejectPBPtr): OSErr;
    INLINE $205F,$7005,$A4DD,$3E80;
FUNCTION PPCWrite(pb: PPCWritePBPtr;async: BOOLEAN): OSErr;
FUNCTION PPCWriteSync(pb: PPCWritePBPtr): OSErr;
    INLINE $205F,$7006,$A0DD,$3E80;
FUNCTION PPCWriteAsync(pb: PPCWritePBPtr): OSErr;
    INLINE $205F,$7006,$A4DD,$3E80;
FUNCTION PPCRead(pb: PPCReadPBPtr;async: BOOLEAN): OSErr;
FUNCTION PPCReadSync(pb: PPCReadPBPtr): OSErr;
    INLINE $205F,$7007,$A0DD,$3E80;
FUNCTION PPCReadAsync(pb: PPCReadPBPtr): OSErr;
    INLINE $205F,$7007,$A4DD,$3E80;
FUNCTION PPCEnd(pb: PPCEndPBPtr;async: BOOLEAN): OSErr;
FUNCTION PPCEndSync(pb: PPCEndPBPtr): OSErr;
    INLINE $205F,$7008,$A0DD,$3E80;
FUNCTION PPCEndAsync(pb: PPCEndPBPtr): OSErr;
    INLINE $205F,$7008,$A4DD,$3E80;
FUNCTION PPCClose(pb: PPCClosePBPtr;async: BOOLEAN): OSErr;
FUNCTION PPCCloseSync(pb: PPCClosePBPtr): OSErr;
    INLINE $205F,$7009,$A0DD,$3E80;
FUNCTION PPCCloseAsync(pb: PPCClosePBPtr): OSErr;
    INLINE $205F,$7009,$A4DD,$3E80;
FUNCTION IPCListPorts(pb: IPCListPortsPBPtr;async: BOOLEAN): OSErr;
FUNCTION IPCListPortsSync(pb: IPCListPortsPBPtr): OSErr;
    INLINE $205F,$700A,$A0DD,$3E80;
FUNCTION IPCListPortsAsync(pb: IPCListPortsPBPtr): OSErr;
    INLINE $205F,$700A,$A4DD,$3E80;
FUNCTION DeleteUserIdentity(userRef: LONGINT): OSErr;
FUNCTION GetDefaultUser(VAR userRef: LONGINT;
                        VAR userName: Str32): OSErr;
FUNCTION StartSecureSession(pb: PPCStartPBPtr;
                            VAR userName: Str32;
                            useDefault: BOOLEAN;
                            allowGuest: BOOLEAN;
                            VAR guestSelected: BOOLEAN;
                            prompt: Str255): OSErr;
FUNCTION PPCBrowser(prompt: Str255;
                    applListLabel: Str255;
                    defaultSpecified: BOOLEAN;
                    VAR theLocation: LocationNameRec;
                    VAR thePortInfo: PortInfoRec;
                    portFilter: PPCFilterProcPtr;
                    theLocNBPType: Str32): OSErr;
    INLINE $303C,$0D00,$A82B;


{$ENDC}    { UsingPPCToolBox }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

