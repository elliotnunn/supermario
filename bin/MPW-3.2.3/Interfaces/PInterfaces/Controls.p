{
Created: Sunday, January 6, 1991 at 10:25 PM
    Controls.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc. 1985-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Controls;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingControls}
{$SETC UsingControls := 1}

{$I+}
{$SETC ControlsIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingQuickdraw}
{$I $$Shell(PInterfaces)Quickdraw.p}
{$ENDC}
{$SETC UsingIncludes := ControlsIncludes}

CONST
pushButProc = 0;
checkBoxProc = 1;
radioButProc = 2;
useWFont = 8;
scrollBarProc = 16;
inButton = 10;
inCheckBox = 11;
inUpButton = 20;
inDownButton = 21;
inPageUp = 22;
inPageDown = 23;
inThumb = 129;

popupMenuProc = 1008;               { 63 * 16 }
popupFixedWidth = $0001;            { popup menu CDEF variation codes }
popupUseAddResMenu = $0004;
popupUseWFont = $0008;
popupTitleBold = $00000100;         {    Popup Title characteristics }
popupTitleItalic = $00000200;
popupTitleUnderline = $00000400;
popupTitleOutline = $00000800;
popupTitleShadow = $00001000;
popupTitleCondense = $00002000;
popupTitleExtend = $00004000;
popupTitleNoStyle = $00008000;
popupTitleLeftJust = $00000000;
popupTitleCenterJust = $00000001;
popupTitleRightJust = $000000FF;

{
axis constraints for DragGrayRgn call}
noConstraint = 0;
hAxisOnly = 1;
vAxisOnly = 2;

{
control messages}
drawCntl = 0;
testCntl = 1;
calcCRgns = 2;
initCntl = 3;
dispCntl = 4;
posCntl = 5;
thumbCntl = 6;
dragCntl = 7;
autoTrack = 8;
calcCntlRgn = 10;
calcThumbRgn = 11;

cFrameColor = 0;
cBodyColor = 1;
cTextColor = 2;
cThumbColor = 3;
popupMenuCDEFproc = popupMenuProc;  { synonym for compatibility }

TYPE
ControlPtr = ^ControlRecord;
ControlHandle = ^ControlPtr;
ControlRecord = PACKED RECORD
    nextControl: ControlHandle;
    contrlOwner: WindowPtr;
    contrlRect: Rect;
    contrlVis: Byte;
    contrlHilite: Byte;
    contrlValue: INTEGER;
    contrlMin: INTEGER;
    contrlMax: INTEGER;
    contrlDefProc: Handle;
    contrlData: Handle;
    contrlAction: ProcPtr;
    contrlRfCon: LONGINT;
    contrlTitle: Str255;
    END;

CCTabPtr = ^CtlCTab;
CCTabHandle = ^CCTabPtr;
CtlCTab = RECORD
    ccSeed: LONGINT;                {reserved}
    ccRider: INTEGER;               {see what you have done - reserved}
    ctSize: INTEGER;                {usually 3 for controls}
    ctTable: ARRAY [0..3] OF ColorSpec;
    END;

AuxCtlPtr = ^AuxCtlRec;
AuxCtlHandle = ^AuxCtlPtr;
AuxCtlRec = RECORD
    acNext: AuxCtlHandle;           {handle to next AuxCtlRec}
    acOwner: ControlHandle;         {handle for aux record's control}
    acCTable: CCTabHandle;          {color table for this control}
    acFlags: INTEGER;               {misc flag byte}
    acReserved: LONGINT;            {reserved for use by Apple}
    acRefCon: LONGINT;              {for use by application}
    END;


FUNCTION NewControl(theWindow: WindowPtr;boundsRect: Rect;title: Str255;
    visible: BOOLEAN;value: INTEGER;min: INTEGER;max: INTEGER;procID: INTEGER;
    refCon: LONGINT): ControlHandle;
    INLINE $A954;
PROCEDURE SetCTitle(theControl: ControlHandle;title: Str255);
    INLINE $A95F;
PROCEDURE GetCTitle(theControl: ControlHandle;VAR title: Str255);
    INLINE $A95E;
FUNCTION GetNewControl(controlID: INTEGER;owner: WindowPtr): ControlHandle;
    INLINE $A9BE;
PROCEDURE DisposeControl(theControl: ControlHandle);
    INLINE $A955;
PROCEDURE KillControls(theWindow: WindowPtr);
    INLINE $A956;
PROCEDURE HideControl(theControl: ControlHandle);
    INLINE $A958;
PROCEDURE ShowControl(theControl: ControlHandle);
    INLINE $A957;
PROCEDURE DrawControls(theWindow: WindowPtr);
    INLINE $A969;
PROCEDURE Draw1Control(theControl: ControlHandle);
    INLINE $A96D;
PROCEDURE HiliteControl(theControl: ControlHandle;hiliteState: INTEGER);
    INLINE $A95D;
PROCEDURE UpdtControl(theWindow: WindowPtr;updateRgn: RgnHandle);
    INLINE $A953;
PROCEDURE UpdateControls(theWindow: WindowPtr;updateRgn: RgnHandle);
    INLINE $A953;
PROCEDURE MoveControl(theControl: ControlHandle;h: INTEGER;v: INTEGER);
    INLINE $A959;
PROCEDURE SizeControl(theControl: ControlHandle;w: INTEGER;h: INTEGER);
    INLINE $A95C;
PROCEDURE SetCtlValue(theControl: ControlHandle;theValue: INTEGER);
    INLINE $A963;
FUNCTION GetCtlValue(theControl: ControlHandle): INTEGER;
    INLINE $A960;
PROCEDURE SetCtlMin(theControl: ControlHandle;minValue: INTEGER);
    INLINE $A964;
FUNCTION GetCtlMin(theControl: ControlHandle): INTEGER;
    INLINE $A961;
PROCEDURE SetCtlMax(theControl: ControlHandle;maxValue: INTEGER);
    INLINE $A965;
FUNCTION GetCtlMax(theControl: ControlHandle): INTEGER;
    INLINE $A962;
PROCEDURE SetCRefCon(theControl: ControlHandle;data: LONGINT);
    INLINE $A95B;
FUNCTION GetCRefCon(theControl: ControlHandle): LONGINT;
    INLINE $A95A;
PROCEDURE SetCtlAction(theControl: ControlHandle;actionProc: ProcPtr);
    INLINE $A96B;
FUNCTION GetCtlAction(theControl: ControlHandle): ProcPtr;
    INLINE $A96A;
PROCEDURE DragControl(theControl: ControlHandle;startPt: Point;limitRect: Rect;
    slopRect: Rect;axis: INTEGER);
    INLINE $A967;
FUNCTION TestControl(theControl: ControlHandle;thePt: Point): INTEGER;
    INLINE $A966;
FUNCTION TrackControl(theControl: ControlHandle;thePoint: Point;actionProc: ProcPtr): INTEGER;
    INLINE $A968;
FUNCTION FindControl(thePoint: Point;theWindow: WindowPtr;VAR theControl: ControlHandle): INTEGER;
    INLINE $A96C;
PROCEDURE SetCtlColor(theControl: ControlHandle;newColorTable: CCTabHandle);
    INLINE $AA43;
FUNCTION GetAuxCtl(theControl: ControlHandle;VAR acHndl: AuxCtlHandle): BOOLEAN;
    INLINE $AA44;
FUNCTION GetCVariant(theControl: ControlHandle): INTEGER;
    INLINE $A809;


{$ENDC}    { UsingControls }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

