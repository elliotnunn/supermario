{
Created: Sunday, January 6, 1991 at 10:50 PM
    Notification.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1989-90
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Notification;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingNotification}
{$SETC UsingNotification := 1}

{$I+}
{$SETC NotificationIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingOSUtils}
{$I $$Shell(PInterfaces)OSUtils.p}
{$ENDC}
{$SETC UsingIncludes := NotificationIncludes}

CONST
nmType = 8;

TYPE
NMRecPtr = ^NMRec;
NMRec = RECORD
    qLink: QElemPtr;        {next queue entry}
    qType: INTEGER;         {queue type -- ORD(nmType) = 8}
    nmFlags: INTEGER;       {reserved}
    nmPrivate: LONGINT;     {reserved}
    nmReserved: INTEGER;    {reserved}
    nmMark: INTEGER;        {item to mark in Apple menu}
    nmIcon: Handle;         {handle to small icon}
    nmSound: Handle;        {handle to sound record}
    nmStr: StringPtr;       {string to appear in alert}
    nmResp: ProcPtr;        {pointer to response routine}
    nmRefCon: LONGINT;      {for application use}
    END;


FUNCTION NMInstall(nmReqPtr: NMRecPtr): OSErr;
    INLINE $205F,$A05E,$3E80;
FUNCTION NMRemove(nmReqPtr: NMRecPtr): OSErr;
    INLINE $205F,$A05F,$3E80;


{$ENDC}    { UsingNotification }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

