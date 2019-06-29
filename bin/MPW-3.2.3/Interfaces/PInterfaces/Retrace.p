{
Created: Sunday, January 6, 1991 at 11:06 PM
    Retrace.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Retrace;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingRetrace}
{$SETC UsingRetrace := 1}

{$I+}
{$SETC RetraceIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingOSUtils}
{$I $$Shell(PInterfaces)OSUtils.p}
{$ENDC}
{$SETC UsingIncludes := RetraceIncludes}

FUNCTION GetVBLQHdr: QHdrPtr;
    INLINE $2EBC,$0000,$0160;
FUNCTION SlotVInstall(vblBlockPtr: QElemPtr;theSlot: INTEGER): OSErr;
    INLINE $301F,$205F,$A06F,$3E80;
FUNCTION SlotVRemove(vblBlockPtr: QElemPtr;theSlot: INTEGER): OSErr;
    INLINE $301F,$205F,$A070,$3E80;
FUNCTION AttachVBL(theSlot: INTEGER): OSErr;
    INLINE $301F,$A071,$3E80;
FUNCTION DoVBLTask(theSlot: INTEGER): OSErr;
    INLINE $301F,$A072,$3E80;
FUNCTION VInstall(vblTaskPtr: QElemPtr): OSErr;
    INLINE $205F,$A033,$3E80;
FUNCTION VRemove(vblTaskPtr: QElemPtr): OSErr;
    INLINE $205F,$A034,$3E80;


{$ENDC}    { UsingRetrace }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

