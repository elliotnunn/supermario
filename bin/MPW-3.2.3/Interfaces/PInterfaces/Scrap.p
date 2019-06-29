{
Created: Sunday, January 6, 1991 at 11:07 PM
    Scrap.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1985-1989
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Scrap;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingScrap}
{$SETC UsingScrap := 1}

{$I+}
{$SETC ScrapIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$SETC UsingIncludes := ScrapIncludes}

TYPE
PScrapStuff = ^ScrapStuff;
ScrapStuff = RECORD
    scrapSize: LONGINT;
    scrapHandle: Handle;
    scrapCount: INTEGER;
    scrapState: INTEGER;
    scrapName: StringPtr;
    END;


FUNCTION InfoScrap: PScrapStuff;
    INLINE $A9F9;
FUNCTION UnloadScrap: LONGINT;
    INLINE $A9FA;
FUNCTION LoadScrap: LONGINT;
    INLINE $A9FB;
FUNCTION GetScrap(hDest: Handle;theType: ResType;VAR offset: LONGINT): LONGINT;
    INLINE $A9FD;
FUNCTION ZeroScrap: LONGINT;
    INLINE $A9FC;
FUNCTION PutScrap(length: LONGINT;theType: ResType;source: Ptr): LONGINT;
    INLINE $A9FE;


{$ENDC}    { UsingScrap }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

