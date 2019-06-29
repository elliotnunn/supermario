{
Created: Sunday, January 6, 1991 at 11:20 PM
    Start.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1987-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Start;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingStart}
{$SETC UsingStart := 1}

{$I+}
{$SETC StartIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$SETC UsingIncludes := StartIncludes}

TYPE
DefStartType = (slotDev,scsiDev);


DefStartPtr = ^DefStartRec;
DefStartRec = RECORD
    CASE DefStartType OF
      slotDev:
        (sdExtDevID: SignedByte;
        sdPartition: SignedByte;
        sdSlotNum: SignedByte;
        sdSRsrcID: SignedByte);
      scsiDev:
        (sdReserved1: SignedByte;
        sdReserved2: SignedByte;
        sdRefNum: INTEGER);
    END;

DefVideoPtr = ^DefVideoRec;
DefVideoRec = RECORD
    sdSlot: SignedByte;
    sdsResource: SignedByte;
    END;

DefOSPtr = ^DefOSRec;
DefOSRec = RECORD
    sdReserved: SignedByte;
    sdOSType: SignedByte;
    END;


PROCEDURE GetDefaultStartup(paramBlock: DefStartPtr);
    INLINE $205F,$A07D;
PROCEDURE SetDefaultStartup(paramBlock: DefStartPtr);
    INLINE $205F,$A07E;
PROCEDURE GetVideoDefault(paramBlock: DefVideoPtr);
    INLINE $205F,$A080;
PROCEDURE SetVideoDefault(paramBlock: DefVideoPtr);
    INLINE $205F,$A081;
PROCEDURE GetOSDefault(paramBlock: DefOSPtr);
    INLINE $205F,$A084;
PROCEDURE SetOSDefault(paramBlock: DefOSPtr);
    INLINE $205F,$A083;
PROCEDURE SetTimeout(count: INTEGER);
PROCEDURE GetTimeout(VAR count: INTEGER);


{$ENDC}    { UsingStart }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

