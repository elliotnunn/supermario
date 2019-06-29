{
Created: Sunday, January 6, 1991 at 10:58 PM
    Processes.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1989-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Processes;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingProcesses}
{$SETC UsingProcesses := 1}

{$I+}
{$SETC ProcessesIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingEvents}
{$I $$Shell(PInterfaces)Events.p}
{$ENDC}
{$IFC UNDEFINED UsingFiles}
{$I $$Shell(PInterfaces)Files.p}
{$ENDC}
{$SETC UsingIncludes := ProcessesIncludes}

TYPE
{ type for unique process identifier }
ProcessSerialNumberPtr = ^ProcessSerialNumber;
ProcessSerialNumber = RECORD
    highLongOfPSN: LONGINT;
    lowLongOfPSN: LONGINT;
    END;


CONST

{************************************************************************
 *                            Process identifier.
 ************************************************************************
 Various reserved process serial numbers. }

kNoProcess = 0;
kSystemProcess = 1;
kCurrentProcess = 2;

TYPE
{*********************************************************************************************************************************************
 *        Definition of the parameter block passed to _Launch.
 *************************************************************************

* Typedef and flags for launchControlFlags field }
LaunchFlags = INTEGER;

CONST

{************************************************************************
 *        Definition of the parameter block passed to _Launch.
 ************************************************************************}

launchContinue = $4000;
launchNoFileFlags = $0800;
launchUseMinimum = $0400;
launchDontSwitch = $0200;
launchAllow24Bit = $0100;
launchInhibitDaemon = $0080;

TYPE
{ Format for first AppleEvent to pass to new process.  The size of the overall
 * buffer variable: the message body immediately follows the messageLength.
 }
AppParametersPtr = ^AppParameters;
AppParameters = RECORD
    theMsgEvent: EventRecord;
    eventRefCon: LONGINT;
    messageLength: LONGINT;
    messageBuffer: ARRAY [0..0] OF SignedByte;
    END;

{ Parameter block to _Launch }
LaunchPBPtr = ^LaunchParamBlockRec;
LaunchParamBlockRec = RECORD
    reserved1: LONGINT;
    reserved2: INTEGER;
    launchBlockID: INTEGER;
    launchEPBLength: LONGINT;
    launchFileFlags: INTEGER;
    launchControlFlags: LaunchFlags;
    launchAppSpec: FSSpecPtr;
    launchProcessSN: ProcessSerialNumber;
    launchPreferredSize: LONGINT;
    launchMinimumSize: LONGINT;
    launchAvailableSize: LONGINT;
    launchAppParameters: AppParametersPtr;
    END;


CONST
extendedBlock = $4C43;  { 'LC' }
extendedBlockLen = (sizeof(LaunchParamBlockRec) - 12);

{************************************************************************
 * Definition of the information block returned by GetProcessInformation
 ************************************************************************
 Bits in the processMode field }

modeDeskAccessory = $00020000;
modeMultiLaunch = $00010000;
modeNeedSuspendResume = $00004000;
modeCanBackground = $00001000;
modeDoesActivateOnFGSwitch = $00000800;
modeOnlyBackground = $00000400;
modeGetFrontClicks = $00000200;
modeGetAppDiedMsg = $00000100;
mode32BitCompatible = $00000080;
modeHighLevelEventAware = $00000040;
modeLocalAndRemoteHLEvents = $00000020;
modeStationeryAware = $00000010;
modeUseTextEditServices = $00000008;

TYPE
{ Record returned by GetProcessInformation }
ProcessInfoRecPtr = ^ProcessInfoRec;
ProcessInfoRec = RECORD
    processInfoLength: LONGINT;
    processName: StringPtr;
    processNumber: ProcessSerialNumber;
    processType: LONGINT;
    processSignature: OSType;
    processMode: LONGINT;
    processLocation: Ptr;
    processSize: LONGINT;
    processFreeMem: LONGINT;
    processLauncher: ProcessSerialNumber;
    processLaunchDate: LONGINT;
    processActiveTime: LONGINT;
    processAppSpec: FSSpecPtr;
    END;


FUNCTION LaunchApplication(LaunchParams:LaunchPBPtr):OSErr;
    INLINE $205F,$A9F2,$3E80;
FUNCTION LaunchDeskAccessory(pFileSpec: FSSpecPtr;pDAName: StringPtr): OSErr;
    INLINE $3F3C,$0036,$A88F;
FUNCTION GetCurrentProcess(VAR PSN: ProcessSerialNumber): OSErr;
    INLINE $3F3C,$0037,$A88F;
FUNCTION GetFrontProcess(VAR PSN: ProcessSerialNumber): OSErr;
    INLINE $70FF,$2F00,$3F3C,$0039,$A88F;
FUNCTION GetNextProcess(VAR PSN: ProcessSerialNumber): OSErr;
    INLINE $3F3C,$0038,$A88F;
FUNCTION GetProcessInformation(PSN: ProcessSerialNumber;VAR info: ProcessInfoRec): OSErr;
    INLINE $3F3C,$003A,$A88F;
FUNCTION SetFrontProcess(PSN: ProcessSerialNumber): OSErr;
    INLINE $3F3C,$003B,$A88F;
FUNCTION WakeUpProcess(PSN: ProcessSerialNumber): OSErr;
    INLINE $3F3C,$003C,$A88F;
FUNCTION SameProcess(PSN1: ProcessSerialNumber;PSN2: ProcessSerialNumber;
    VAR result: BOOLEAN): OSErr;
    INLINE $3F3C,$003D,$A88F;


{$ENDC}    { UsingProcesses }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

