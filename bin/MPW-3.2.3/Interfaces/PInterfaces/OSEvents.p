{
Created: Sunday, January 6, 1991 at 10:51 PM
    OSEvents.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT OSEvents;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingOSEvents}
{$SETC UsingOSEvents := 1}

{$I+}
{$SETC OSEventsIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingEvents}
{$I $$Shell(PInterfaces)Events.p}
{$ENDC}
{$IFC UNDEFINED UsingOSUtils}
{$I $$Shell(PInterfaces)OSUtils.p}
{$ENDC}
{$SETC UsingIncludes := OSEventsIncludes}

FUNCTION PostEvent(eventNum: INTEGER;eventMsg: LONGINT): OSErr;
FUNCTION PPostEvent(eventCode: INTEGER;eventMsg: LONGINT;VAR qEl: EvQElPtr): OSErr;
FUNCTION OSEventAvail(mask: INTEGER;VAR theEvent: EventRecord): BOOLEAN;
FUNCTION GetOSEvent(mask: INTEGER;VAR theEvent: EventRecord): BOOLEAN;
PROCEDURE FlushEvents(whichMask: INTEGER;stopMask: INTEGER);
    INLINE $201F,$A032;
PROCEDURE SetEventMask(theMask: INTEGER);
    INLINE $31DF,$0144;
FUNCTION GetEvQHdr: QHdrPtr;
    INLINE $2EBC,$0000,$014A;


{$ENDC}    { UsingOSEvents }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

