{
Created: Sunday, January 6, 1991 at 10:57 PM
    Printing.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1985-1989
        All rights reserved

}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Printing;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingPrinting}
{$SETC UsingPrinting := 1}

{$I+}
{$SETC PrintingIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingQuickdraw}
{$I $$Shell(PInterfaces)Quickdraw.p}
{$ENDC}
{$IFC UNDEFINED UsingDialogs}
{$I $$Shell(PInterfaces)Dialogs.p}
{$ENDC}
{$SETC UsingIncludes := PrintingIncludes}

CONST
iPFMaxPgs = 128;
iPrPgFract = 120;                       {Page scale factor. ptPgSize (below) is in units of 1/iPrPgFract}
iPrPgFst = 1;                           {Page range constants}
iPrPgMax = 9999;
iPrRelease = 3;                         {Current version number of the code.}
iPrSavPFil = -1;
iPrAbort = $0080;
iPrDevCtl = 7;                          {The PrDevCtl Proc's ctl number}
lPrReset = $00010000;                   {The PrDevCtl Proc's CParam for reset}
lPrLineFeed = $00030000;
lPrLFStd = $0003FFFF;                   {The PrDevCtl Proc's CParam for std paper advance}
lPrLFSixth = $0003FFFF;
lPrPageEnd = $00020000;                 {The PrDevCtl Proc's CParam for end page}
lPrDocOpen = $00010000;
lPrPageOpen = $00040000;
lPrPageClose = $00020000;
lPrDocClose = $00050000;
iFMgrCtl = 8;                           {The FMgr's Tail-hook Proc's ctl number}
iMemFullErr = -108;
iIOAbort = -27;
pPrGlobals = $00000944;                 {The PrVars lo mem area:}
bDraftLoop = 0;
bSpoolLoop = 1;
bUser1Loop = 2;
bUser2Loop = 3;
iPrBitsCtl = 4;
lScreenBits = 0;
lPaintBits = 1;
lHiScreenBits = $00000002;              {The Bitmap Print Proc's Screen Bitmap param}
lHiPaintBits = $00000003;               {The Bitmap Print Proc's Paint [sq pix] param}
iPrIOCtl = 5;
iPrEvtCtl = 6;                          {The PrEvent Proc's ctl number}
lPrEvtAll = $0002FFFD;                  {The PrEvent Proc's CParam for the entire screen}
lPrEvtTop = $0001FFFD;                  {The PrEvent Proc's CParam for the top folder}
iPrDrvrRef = -3;
getRslDataOp = 4;
setRslOp = 5;
draftBitsOp = 6;
noDraftBitsOp = 7;
getRotnOp = 8;
NoSuchRsl = 1;
RgType1 = 1;

TYPE
TFeed = (feedCut,feedFanfold,feedMechCut,feedOther);

TScan = (scanTB,scanBT,scanLR,scanRL);


TPRect = ^Rect;                         { A Rect Ptr }


PrIdleProcPtr = ProcPtr;
PItemProcPtr = ProcPtr;

TPPrPort = ^TPrPort;
TPrPort = RECORD
    gPort: GrafPort;                    {The Printer's graf port.}
    gProcs: QDProcs;                    {..and its procs}
    lGParam1: LONGINT;                  {16 bytes for private parameter storage.}
    lGParam2: LONGINT;
    lGParam3: LONGINT;
    lGParam4: LONGINT;
    fOurPtr: BOOLEAN;                   {Whether the PrPort allocation was done by us.}
    fOurBits: BOOLEAN;                  {Whether the BitMap allocation was done by us.}
    END;

{ Printing Graf Port. All printer imaging, whether spooling, banding, etc, happens "thru" a GrafPort.
  This is the "PrPeek" record. }
TPPrInfo = ^TPrInfo;
TPrInfo = RECORD
    iDev: INTEGER;                      {Font mgr/QuickDraw device code}
    iVRes: INTEGER;                     {Resolution of device, in device coordinates}
    iHRes: INTEGER;                     {..note: V before H => compatable with Point.}
    rPage: Rect;                        {The page (printable) rectangle in device coordinates.}
    END;

{ Print Info Record: The parameters needed for page composition. }
TPPrStl = ^TPrStl;
TPrStl = RECORD
    wDev: INTEGER;
    iPageV: INTEGER;
    iPageH: INTEGER;
    bPort: SignedByte;
    feed: TFeed;
    END;

TPPrXInfo = ^TPrXInfo;
TPrXInfo = RECORD
    iRowBytes: INTEGER;
    iBandV: INTEGER;
    iBandH: INTEGER;
    iDevBytes: INTEGER;
    iBands: INTEGER;
    bPatScale: SignedByte;
    bUlThick: SignedByte;
    bUlOffset: SignedByte;
    bUlShadow: SignedByte;
    scan: TScan;
    bXInfoX: SignedByte;
    END;

TPPrJob = ^TPrJob;
TPrJob = RECORD
    iFstPage: INTEGER;                  {Page Range.}
    iLstPage: INTEGER;
    iCopies: INTEGER;                   {No. copies.}
    bJDocLoop: SignedByte;              {The Doc style: Draft, Spool, .., and ..}
    fFromUsr: BOOLEAN;                  {Printing from an User's App (not PrApp) flag}
    pIdleProc: PrIdleProcPtr;           {The Proc called while waiting on IO etc.}
    pFileName: StringPtr;               {Spool File Name: NIL for default.}
    iFileVol: INTEGER;                  {Spool File vol, set to 0 initially}
    bFileVers: SignedByte;              {Spool File version, set to 0 initially}
    bJobX: SignedByte;                  {An eXtra byte.}
    END;

{ Print Job: Print "form" for a single print request. }
TPPrint = ^TPrint;
THPrint = ^TPPrint;
TPrint = RECORD
    iPrVersion: INTEGER;                {(2) Printing software version}
    prInfo: TPrInfo;                    {(14) the PrInfo data associated with the current style.}
    rPaper: Rect;                       {(8) The paper rectangle [offset from rPage]}
    prStl: TPrStl;                      {(8)  This print request's style.}
    prInfoPT: TPrInfo;                  {(14)  Print Time Imaging metrics}
    prXInfo: TPrXInfo;                  {(16)  Print-time (expanded) Print info record.}
    prJob: TPrJob;                      {(20) The Print Job request (82)  Total of the above; 120-82 = 38 bytes needed to fill 120}
    printX: ARRAY [1..19] OF INTEGER;   {Spare to fill to 120 bytes!}
    END;

{ The universal 120 byte printing record }
TPPrStatus = ^TPrStatus;
TPrStatus = RECORD
    iTotPages: INTEGER;                 {Total pages in Print File.}
    iCurPage: INTEGER;                  {Current page number}
    iTotCopies: INTEGER;                {Total copies requested}
    iCurCopy: INTEGER;                  {Current copy number}
    iTotBands: INTEGER;                 {Total bands per page.}
    iCurBand: INTEGER;                  {Current band number}
    fPgDirty: BOOLEAN;                  {True if current page has been written to.}
    fImaging: BOOLEAN;                  {Set while in band's DrawPic call.}
    hPrint: THPrint;                    {Handle to the active Printer record}
    pPrPort: TPPrPort;                  {Ptr to the active PrPort}
    hPic: PicHandle;                    {Handle to the active Picture}
    END;

{ Print Status: Print information during printing. }
TPPfPgDir = ^TPfPgDir;
THPfPgDir = ^TPPfPgDir;
TPfPgDir = RECORD
    iPages: INTEGER;
    iPgPos: ARRAY [0..128] OF LONGINT;  {ARRAY [0..iPfMaxPgs] OF LONGINT}
    END;

{ PicFile = a TPfHeader followed by n QuickDraw Pics (whose PicSize is invalid!) }
TPPrDlg = ^TPrDlg;
TPrDlg = RECORD
    Dlg: DialogRecord;                  {The Dialog window}
    pFltrProc: ModalFilterProcPtr;      {The Filter Proc.}
    pItemProc: PItemProcPtr;            {The Item evaluating proc.}
    hPrintUsr: THPrint;                 {The user's print record.}
    fDoIt: BOOLEAN;
    fDone: BOOLEAN;
    lUser1: LONGINT;                    {Four longs for user's to hang global data.}
    lUser2: LONGINT;                    {...Plus more stuff needed by the particular printing dialog.}
    lUser3: LONGINT;
    lUser4: LONGINT;
    END;


PDlgInitProcPtr = ProcPtr;

TGnlData = RECORD
    iOpCode: INTEGER;
    iError: INTEGER;
    lReserved: LONGINT;                 {more fields here depending on call}
    END;

TRslRg = RECORD
    iMin: INTEGER;
    iMax: INTEGER;
    END;

TRslRec = RECORD
    iXRsl: INTEGER;
    iYRsl: INTEGER;
    END;

TGetRslBlk = RECORD
    iOpCode: INTEGER;
    iError: INTEGER;
    lReserved: LONGINT;
    iRgType: INTEGER;
    xRslRg: TRslRg;
    yRslRg: TRslRg;
    iRslRecCnt: INTEGER;
    rgRslRec: ARRAY [1..27] OF TRslRec;
    END;

TSetRslBlk = RECORD
    iOpCode: INTEGER;
    iError: INTEGER;
    lReserved: LONGINT;
    hPrint: THPrint;
    iXRsl: INTEGER;
    iYRsl: INTEGER;
    END;

TDftBitsBlk = RECORD
    iOpCode: INTEGER;
    iError: INTEGER;
    lReserved: LONGINT;
    hPrint: THPrint;
    END;

TGetRotnBlk = RECORD
    iOpCode: INTEGER;
    iError: INTEGER;
    lReserved: LONGINT;
    hPrint: THPrint;
    fLandscape: BOOLEAN;
    bXtra: SignedByte;
    END;

TPBitMap = ^BitMap;                     { A BitMap Ptr }

TN = 0..15;                             { a Nibble }

TPWord = ^TWord;
THWord = ^TPWord;
TWord = PACKED RECORD
    CASE INTEGER OF
      0:
        (c1,c0: CHAR);
      1:
        (b1,b0: SignedByte);
      2:
        (usb1,usb0: Byte);
      3:
        (n3,n2,n1,n0: TN);
      4:
        (f15,f14,f13,f12,f11,f10,f9,f8,f7,f6,f5,f4,f3,f2,f1,f0: BOOLEAN);
      5:
        (i0: INTEGER);
    END;

TPLong = ^TLong;
THLong = ^TPLong;
TLong = RECORD
    CASE INTEGER OF
      0:
        (w1,w0: TWord);
      1:
        (b1,b0: LONGINT);
      2:
        (p0: Ptr);
      3:
        (h0: Handle);
      4:
        (pt: Point);
    END;


PROCEDURE PrPurge;
PROCEDURE PrNoPurge;
PROCEDURE PrOpen;
PROCEDURE PrClose;
PROCEDURE PrintDefault(hPrint: THPrint);
FUNCTION PrValidate(hPrint: THPrint): BOOLEAN;
FUNCTION PrStlDialog(hPrint: THPrint): BOOLEAN;
FUNCTION PrJobDialog(hPrint: THPrint): BOOLEAN;
FUNCTION PrStlInit(hPrint: THPrint): TPPrDlg;
FUNCTION PrJobInit(hPrint: THPrint): TPPrDlg;
PROCEDURE PrJobMerge(hPrintSrc: THPrint;hPrintDst: THPrint);
FUNCTION PrDlgMain(hPrint: THPrint;pDlgInit: PDlgInitProcPtr): BOOLEAN;
FUNCTION PrOpenDoc(hPrint: THPrint;pPrPort: TPPrPort;pIOBuf: Ptr): TPPrPort;
PROCEDURE PrCloseDoc(pPrPort: TPPrPort);
PROCEDURE PrOpenPage(pPrPort: TPPrPort;pPageFrame: TPRect);
PROCEDURE PrClosePage(pPrPort: TPPrPort);
PROCEDURE PrPicFile(hPrint: THPrint;pPrPort: TPPrPort;pIOBuf: Ptr;pDevBuf: Ptr;
    VAR prStatus: TPrStatus);
FUNCTION PrError: INTEGER;
PROCEDURE PrSetError(iErr: INTEGER);
PROCEDURE PrGeneral(pData: Ptr);
PROCEDURE PrDrvrOpen;
PROCEDURE PrDrvrClose;
PROCEDURE PrCtlCall(iWhichCtl: INTEGER;lParam1: LONGINT;lParam2: LONGINT;
    lParam3: LONGINT);
FUNCTION PrDrvrDCE: Handle;
FUNCTION PrDrvrVers: INTEGER;


{$ENDC}    { UsingPrinting }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

