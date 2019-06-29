{
Created: Sunday, January 6, 1991 at 10:28 PM
    DeskBus.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1987 -1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT DeskBus;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingDeskBus}
{$SETC UsingDeskBus := 1}

{$I+}
{$SETC DeskBusIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$SETC UsingIncludes := DeskBusIncludes}

TYPE
ADBOpBPtr = ^ADBOpBlock;
ADBOpBlock = RECORD
    dataBuffPtr: Ptr;           {address of data buffer}
    opServiceRtPtr: Ptr;        {service routine pointer}
    opDataAreaPtr: Ptr;         {optional data area address}
    END;

ADBDBlkPtr = ^ADBDataBlock;
ADBDataBlock = PACKED RECORD
    devType: SignedByte;        {device type}
    origADBAddr: SignedByte;    {original ADB Address}
    dbServiceRtPtr: Ptr;        {service routine pointer}
    dbDataAreaAddr: Ptr;        {data area address}
    END;

ADBSInfoPtr = ^ADBSetInfoBlock;
ADBSetInfoBlock = RECORD
    siServiceRtPtr: Ptr;        {service routine pointer}
    siDataAreaAddr: Ptr;        {data area address}
    END;

ADBAddress = SignedByte;


PROCEDURE ADBReInit;
    INLINE $A07B;
FUNCTION ADBOp(data: Ptr;compRout: ProcPtr;buffer: Ptr;commandNum: INTEGER): OSErr;
FUNCTION CountADBs: INTEGER;
    INLINE $A077,$3E80;
FUNCTION GetIndADB(VAR info: ADBDataBlock;devTableIndex: INTEGER): ADBAddress;
FUNCTION GetADBInfo(VAR info: ADBDataBlock;adbAddr: ADBAddress): OSErr;
FUNCTION SetADBInfo(VAR info: ADBSetInfoBlock;adbAddr: ADBAddress): OSErr;


{$ENDC}    { UsingDeskBus }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

