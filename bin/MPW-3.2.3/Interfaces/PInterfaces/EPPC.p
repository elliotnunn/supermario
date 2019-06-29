{
Created: Sunday, January 6, 1991 at 10:32 PM
    EPPC.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1988-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT EPPC;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingEPPC}
{$SETC UsingEPPC := 1}

{$I+}
{$SETC EPPCIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingPPCToolbox}
{$I $$Shell(PInterfaces)PPCToolbox.p}
{$ENDC}
{$IFC UNDEFINED UsingProcesses}
{$I $$Shell(PInterfaces)Processes.p}
{$ENDC}
{$IFC UNDEFINED UsingEvents}
{$I $$Shell(PInterfaces)Events.p}
{$ENDC}
{$SETC UsingIncludes := EPPCIncludes}

CONST
kHighLevelEvent = 23;

{ postOptions currently supported }
receiverIDMask = $0000F000;
receiverIDisPSN = $00008000;
receiverIDisSignature = $00007000;
receiverIDisSessionID = $00006000;
receiverIDisTargetID = $00005000;

systemOptionsMask = $00000F00;
nReturnReceipt = $00000200;

priorityMask = $000000FF;
nAttnMsg = $00000001;

{ error returns from Post and Accept }

bufferIsSmall = -607;
noOutstandingHLE = -608;
connectionInvalid = -609;
noUserInteractionAllowed = -610;        { no user interaction allowed }

{ constant for return receipts }

HighLevelEventMsgClass = 'jaym';
rtrnReceiptMsgID = 'rtrn';
msgWasPartiallyAccepted = 2;
msgWasFullyAccepted = 1;
msgWasNotAccepted = 0;

TYPE
TargetIDPtr = ^TargetID;
TargetIDHdl = ^TargetIDPtr;
TargetID = RECORD
    sessionID: LONGINT;
    name: PPCPortRec;
    location: LocationNameRec;
    recvrName: PPCPortRec;
    END;


SenderID = TargetID;
SenderIDPtr = ^SenderID;

HighLevelEventMsgPtr = ^HighLevelEventMsg;
HighLevelEventMsgHdl = ^HighLevelEventMsgPtr;
HighLevelEventMsg = RECORD
    HighLevelEventMsgHeaderLength: INTEGER;
    version: INTEGER;
    reserved1: LONGINT;
    theMsgEvent: EventRecord;
    userRefcon: LONGINT;
    postingOptions: LONGINT;
    msgLength: LONGINT;
    END;


FUNCTION PostHighLevelEvent(theEvent: EventRecord;
                            receiverID: Ptr;
                            msgRefcon: LONGINT;
                            msgBuff: Ptr;
                            msgLen: LONGINT;
                            postingOptions: LONGINT): OSErr;
    INLINE $3F3C,$0034,$A88F;
FUNCTION AcceptHighLevelEvent(VAR sender: TargetID;
                              VAR msgRefcon: LONGINT;
                              msgBuff: Ptr;
                              VAR msgLen: LONGINT): OSErr;
    INLINE $3F3C,$0033,$A88F;
FUNCTION GetProcessSerialNumberFromPortName(portName: PPCPortRec;VAR PSN: ProcessSerialNumber): OSErr;
    INLINE $3F3C,$0035,$A88F;
FUNCTION GetPortNameFromProcessSerialNumber(VAR portName: PPCPortRec;PSN: ProcessSerialNumber): OSErr;
    INLINE $3F3C,$0046,$A88F;

TYPE
GetSpecificFilterProcPtr = ProcPtr;     { FUNCTION MyFilter(yourDataPtr: Ptr;                }
                                        {                    msgBuff: HighLevelEventMsgPtr;    }
                                        {                    sender: TargetID): Boolean;        }

FUNCTION GetSpecificHighLevelEvent(aFilter: GetSpecificFilterProcPtr;yourDataPtr: UNIV Ptr;
    VAR err: OSErr): BOOLEAN;
    INLINE $3F3C,$0045,$A88F;


{$ENDC}    { UsingEPPC }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

