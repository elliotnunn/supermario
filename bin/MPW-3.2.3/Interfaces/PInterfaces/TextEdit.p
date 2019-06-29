{
Created: Sunday, January 6, 1991 at 11:23 PM
    TextEdit.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT TextEdit;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingTextEdit}
{$SETC UsingTextEdit := 1}

{$I+}
{$SETC TextEditIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingQuickdraw}
{$I $$Shell(PInterfaces)Quickdraw.p}
{$ENDC}
{$SETC UsingIncludes := TextEditIncludes}

CONST

{ Justification styles }
teJustLeft = 0;
teJustCenter = 1;
teJustRight = -1;
teForceLeft = -2;

{ new names for the Justification styles }
teFlushDefault = 0;                     {flush according to the line direction }
teCenter = 1;                           {center justify }
teFlushRight = -1;                      {flush right for all scripts }
teFlushLeft = -2;                       {flush left for all scripts }

{ Set/Replace style modes }
fontBit = 0;                            {set font}
faceBit = 1;                            {set face}
sizeBit = 2;                            {set size}
clrBit = 3;                             {set color}
addSizeBit = 4;                         {add size mode}
toglBit = 5;                            {set faces in toggle mode}

{ TESetStyle/TEContinuousStyle modes }
doFont = 1;                             { set font (family) number}
doFace = 2;                             {set character style}
doSize = 4;                             {set type size}
doColor = 8;                            {set color}
doAll = 15;                             {set all attributes}
addSize = 16;                           {adjust type size}
doToggle = 32;                          {toggle mode for TESetStyle & TEContinuousStyle}

{ offsets into TEDispatchRec }
EOLHook = 0;                            {[ProcPtr] TEEOLHook}
DRAWHook = 4;                           {[ProcPtr] TEWidthHook}
WIDTHHook = 8;                          {[ProcPtr] TEDrawHook}
HITTESTHook = 12;                       {[ProcPtr] TEHitTestHook}
nWIDTHHook = 24;                        {[ProcPtr] nTEWidthHook}
TextWidthHook = 28;                     {[ProcPtr] TETextWidthHook}

{ selectors for TECustomHook }
intEOLHook = 0;                         {TEIntHook value}
intDrawHook = 1;                        {TEIntHook value}
intWidthHook = 2;                       {TEIntHook value}
intHitTestHook = 3;                     {TEIntHook value}
intNWidthHook = 6;                      {TEIntHook value for new version of WidthHook}
intTextWidthHook = 7;                   {TEIntHook value for new TextWidthHook}

{ feature or bit definitions for TEFeatureFlag }
teFAutoScr = 0;                         {00000001b}
teFTextBuffering = 1;                   {00000010b}
teFOutlineHilite = 2;                   {00000100b}
teFInlineInput = 3;                     {00001000b}
teFUseTextServices = 4;                 {00010000b}

{ action for the new "bit (un)set" interface, TEFeatureFlag }
TEBitClear = 0;
TEBitSet = 1;                           {set the selector bit}
TEBitTest = -1;                         {no change; just return the current setting}

{constants for identifying the routine that called FindWord }
teWordSelect = 4;                       {clickExpand to select word}
teWordDrag = 8;                         {clickExpand to drag new word}
teFromFind = 12;                        {FindLine called it ($0C)}
teFromRecal = 16;                       {RecalLines called it ($10)}

TYPE
TEPtr = ^TERec;
TEHandle = ^TEPtr;
TERec = RECORD
    destRect: Rect;
    viewRect: Rect;
    selRect: Rect;
    lineHeight: INTEGER;
    fontAscent: INTEGER;
    selPoint: Point;
    selStart: INTEGER;
    selEnd: INTEGER;
    active: INTEGER;
    wordBreak: ProcPtr;
    clikLoop: ProcPtr;
    clickTime: LONGINT;
    clickLoc: INTEGER;
    caretTime: LONGINT;
    caretState: INTEGER;
    just: INTEGER;
    teLength: INTEGER;
    hText: Handle;
    recalBack: INTEGER;
    recalLines: INTEGER;
    clikStuff: INTEGER;
    crOnly: INTEGER;
    txFont: INTEGER;
    txFace: Style;                      {txFace is unpacked byte}
    txMode: INTEGER;
    txSize: INTEGER;
    inPort: GrafPtr;
    highHook: ProcPtr;
    caretHook: ProcPtr;
    nLines: INTEGER;
    lineStarts: ARRAY [0..16000] OF INTEGER;
    END;

CharsPtr = ^Chars;
CharsHandle = ^CharsPtr;

Chars = PACKED ARRAY [0..32000] OF CHAR;

StyleRun = RECORD
    startChar: INTEGER;                 {starting character position}
    styleIndex: INTEGER;                {index in style table}
    END;

STElement = RECORD
    stCount: INTEGER;                   {number of runs in this style}
    stHeight: INTEGER;                  {line height}
    stAscent: INTEGER;                  {font ascent}
    stFont: INTEGER;                    {font (family) number}
    stFace: Style;                      {character Style}
    stSize: INTEGER;                    {size in points}
    stColor: RGBColor;                  {absolute (RGB) color}
    END;

STPtr = ^TEStyleTable;
STHandle = ^STPtr;

TEStyleTable = ARRAY [0..1776] OF STElement;

LHElement = RECORD
    lhHeight: INTEGER;                  {maximum height in line}
    lhAscent: INTEGER;                  {maximum ascent in line}
    END;

LHPtr = ^LHTable;
LHHandle = ^LHPtr;

LHTable = ARRAY [0..8000] OF LHElement;

ScrpSTElement = RECORD
    scrpStartChar: LONGINT;             {starting character position}
    scrpHeight: INTEGER;                {starting character position}
    scrpAscent: INTEGER;
    scrpFont: INTEGER;
    scrpFace: Style;                    {unpacked byte}
    scrpSize: INTEGER;
    scrpColor: RGBColor;
    END;

ScrpSTTable = ARRAY[0..1600] OF ScrpSTElement;

StScrpPtr = ^StScrpRec;
StScrpHandle = ^StScrpPtr;
StScrpRec = RECORD
    scrpNStyles: INTEGER;               {number of styles in scrap}
    scrpStyleTab: ScrpSTTable;          {table of styles for scrap}
    END;

NullStPtr = ^NullStRec;
NullStHandle = ^NullStPtr;
NullStRec = RECORD
    teReserved: LONGINT;                {reserved for future expansion}
    nullScrap: StScrpHandle;            {handle to scrap style table}
    END;

TEStylePtr = ^TEStyleRec;
TEStyleHandle = ^TEStylePtr;
TEStyleRec = RECORD
    nRuns: INTEGER;                     {number of style runs}
    nStyles: INTEGER;                   {size of style table}
    styleTab: STHandle;                 {handle to style table}
    lhTab: LHHandle;                    {handle to line-height table}
    teRefCon: LONGINT;                  {reserved for application use}
    nullStyle: NullStHandle;            {Handle to style set at null selection}
    runs: ARRAY [0..8000] OF StyleRun;  {ARRAY [0..8000] OF StyleRun}
    END;

TextStylePtr = ^TextStyle;
TextStyleHandle = ^TextStylePtr;
TextStyle = RECORD
    tsFont: INTEGER;                    {font (family) number}
    tsFace: Style;                      {character Style}
    tsSize: INTEGER;                    {size in point}
    tsColor: RGBColor;                  {absolute (RGB) color}
    END;


TEIntHook = INTEGER;

PROCEDURE TEInit;
    INLINE $A9CC;
FUNCTION TENew(destRect: Rect;viewRect: Rect): TEHandle;
    INLINE $A9D2;
PROCEDURE TEDispose(hTE: TEHandle);
    INLINE $A9CD;
PROCEDURE TESetText(text: Ptr;length: LONGINT;hTE: TEHandle);
    INLINE $A9CF;
FUNCTION TEGetText(hTE: TEHandle): CharsHandle;
    INLINE $A9CB;
PROCEDURE TEIdle(hTE: TEHandle);
    INLINE $A9DA;
PROCEDURE TESetSelect(selStart: LONGINT;selEnd: LONGINT;hTE: TEHandle);
    INLINE $A9D1;
PROCEDURE TEActivate(hTE: TEHandle);
    INLINE $A9D8;
PROCEDURE TEDeactivate(hTE: TEHandle);
    INLINE $A9D9;
PROCEDURE TEKey(key: CHAR;hTE: TEHandle);
    INLINE $A9DC;
PROCEDURE TECut(hTE: TEHandle);
    INLINE $A9D6;
PROCEDURE TECopy(hTE: TEHandle);
    INLINE $A9D5;
PROCEDURE TEPaste(hTE: TEHandle);
    INLINE $A9DB;
PROCEDURE TEDelete(hTE: TEHandle);
    INLINE $A9D7;
PROCEDURE TEInsert(text: Ptr;length: LONGINT;hTE: TEHandle);
    INLINE $A9DE;
PROCEDURE TESetJust(just: INTEGER;hTE: TEHandle);
    INLINE $A9DF;
PROCEDURE TEUpdate(rUpdate: Rect;hTE: TEHandle);
    INLINE $A9D3;
PROCEDURE TextBox(text: Ptr;length: LONGINT;box: Rect;just: INTEGER);
    INLINE $A9CE;
PROCEDURE TEScroll(dh: INTEGER;dv: INTEGER;hTE: TEHandle);
    INLINE $A9DD;
PROCEDURE TESelView(hTE: TEHandle);
    INLINE $A811;
PROCEDURE TEPinScroll(dh: INTEGER;dv: INTEGER;hTE: TEHandle);
    INLINE $A812;
PROCEDURE TEAutoView(fAuto: BOOLEAN;hTE: TEHandle);
    INLINE $A813;
FUNCTION TEScrapHandle: Handle;
    INLINE $2EB8,$0AB4;
PROCEDURE TECalText(hTE: TEHandle);
    INLINE $A9D0;
FUNCTION TEGetOffset(pt: Point;hTE: TEHandle): INTEGER;
    INLINE $A83C;
FUNCTION TEGetPoint(offset: INTEGER;hTE: TEHandle): Point;
    INLINE $3F3C,$0008,$A83D;
PROCEDURE TEClick(pt: Point;fExtend: BOOLEAN;h: TEHandle);
    INLINE $A9D4;
FUNCTION TEStylNew(destRect: Rect;viewRect: Rect): TEHandle;
    INLINE $A83E;
PROCEDURE SetStylHandle(theHandle: TEStyleHandle;hTE: TEHandle);
    INLINE $3F3C,$0005,$A83D;
FUNCTION GetStylHandle(hTE: TEHandle): TEStyleHandle;
    INLINE $3F3C,$0004,$A83D;
PROCEDURE TEGetStyle(offset: INTEGER;VAR theStyle: TextStyle;VAR lineHeight: INTEGER;
    VAR fontAscent: INTEGER;hTE: TEHandle);
    INLINE $3F3C,$0003,$A83D;
PROCEDURE TEStylPaste(hTE: TEHandle);
    INLINE $3F3C,$0000,$A83D;
PROCEDURE TESetStyle(mode: INTEGER;newStyle: TextStyle;redraw: BOOLEAN;
    hTE: TEHandle);
    INLINE $3F3C,$0001,$A83D;
PROCEDURE TEReplaceStyle(mode: INTEGER;oldStyle: TextStyle;newStyle: TextStyle;
    redraw: BOOLEAN;hTE: TEHandle);
    INLINE $3F3C,$0002,$A83D;
FUNCTION GetStylScrap(hTE: TEHandle): StScrpHandle;
    INLINE $3F3C,$0006,$A83D;
PROCEDURE TEStylInsert(text: Ptr;length: LONGINT;hST: StScrpHandle;hTE: TEHandle);
    INLINE $3F3C,$0007,$A83D;
FUNCTION TEGetHeight(endLine: LONGINT;startLine: LONGINT;hTE: TEHandle): LONGINT;
    INLINE $3F3C,$0009,$A83D;
FUNCTION TEContinuousStyle(VAR mode: INTEGER;VAR aStyle: TextStyle;hTE: TEHandle): BOOLEAN;
    INLINE $3F3C,$000A,$A83D;
PROCEDURE SetStylScrap(rangeStart: LONGINT;rangeEnd: LONGINT;newStyles: StScrpHandle;
    redraw: BOOLEAN;hTE: TEHandle);
    INLINE $3F3C,$000B,$A83D;
PROCEDURE TECustomHook(which: TEIntHook;VAR addr: ProcPtr;hTE: TEHandle);
    INLINE $3F3C,$000C,$A83D;
FUNCTION TENumStyles(rangeStart: LONGINT;rangeEnd: LONGINT;hTE: TEHandle): LONGINT;
    INLINE $3F3C,$000D,$A83D;
FUNCTION TEFeatureFlag(feature: INTEGER;action: INTEGER;hTE: TEHandle): INTEGER;
    INLINE $3F3C,$000E,$A83D;
FUNCTION TEGetScrapLen: LONGINT;
PROCEDURE TESetScrapLen(length: LONGINT);
FUNCTION TEFromScrap: OSErr;
FUNCTION TEToScrap: OSErr;
PROCEDURE SetClikLoop(clikProc: ProcPtr;hTE: TEHandle);
PROCEDURE SetWordBreak(wBrkProc: ProcPtr;hTE: TEHandle);


{$ENDC}    { UsingTextEdit }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

