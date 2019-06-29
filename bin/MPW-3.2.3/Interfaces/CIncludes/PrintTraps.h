/************************************************************

Created: Thursday, September 7, 1989 at 5:45 PM
	PrintTraps.h
	C Interface to the Macintosh Libraries


	Copyright Apple Computer, Inc.	1985-1989
	All rights reserved

************************************************************/


#ifndef __PRINTTRAPS__
#define __PRINTTRAPS__

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif

enum {bDraftLoop = 0};
enum {bSpoolLoop = 1};
enum {iPFMaxPgs = 128};             /*Max number of pages in a print file.*/
enum {iPrPgFract = 120};
enum {iPrPgFst = 1};                /*Page range constants*/
enum {iPrPgMax = 9999};
enum {iPrRelease = 2};              /*Current version number of the code.*/
enum {iPrSavPFil = -1};
enum {iIOAbort = -27};
enum {pPrGlobals = 0x00000944};
enum {bUser1Loop = 2};
enum {bUser2Loop = 3};
enum {fNewRunBit = 2};              /*Bit 2 (3rd bit) in bDocLoop is new JobRun indicator.*/
enum {fHiResOK = 3};                /*Bit 3 (4th bit) in bDocLoop is hi res indicator for paint.*/
enum {fWeOpenedRF = 4};             /*Bit 4 (5th bit) in bDocLoop is set if driver opened the pr res file.*/
enum {iPrAbort = 128};
enum {iPrDevCtl = 7};               /*The PrDevCtl Proc's ctl number*/
enum {lPrReset = 0x00010000};       /*The PrDevCtl Proc's CParam for reset*/
enum {lPrLineFeed = 0x00030000};
enum {lPrLFStd = 0x0003FFFF};       /*The PrDevCtl Proc's CParam for std paper advance*/
enum {lPrLFSixth = 0x0003FFFF};
enum {lPrPageEnd = 0x00020000};     /*The PrDevCtl Proc's CParam for end page*/
enum {lPrDocOpen = 0x00010000};     /*note: same as lPrReset low order byte indicates number of copies to print*/
enum {lPrPageOpen = 0x00040000};
enum {lPrPageClose = 0x00020000};   /*note: same as lPrPageEnd*/
enum {lPrDocClose = 0x00050000};
enum {iFMgrCtl = 8};                /*The FMgr's Tail-hook Proc's ctl number*/
enum {iMscCtl = 9};                 /*Msc Text state / Drvr State ctl number*/
enum {iPvtCtl = 10};                /*Private ctls start here*/
enum {iMemFullErr = -108};

/*
Driver constants */

enum {iPrBitsCtl = 4};              /*The Bitmap Print Proc's ctl number*/
enum {lScreenBits = 0};             /*The Bitmap Print Proc's Screen Bitmap param*/
enum {lPaintBits = 1};              /*The Bitmap Print Proc's Paint [sq pix] param*/
enum {lHiScreenBits = 0x00000002};  /*The Bitmap Print Proc's Screen Bitmap param*/
enum {lHiPaintBits = 0x00000003};   /*The Bitmap Print Proc's Paint [sq pix] param*/
enum {iPrIOCtl = 5};                /*The Raw Byte IO Proc's ctl number*/
enum {iPrEvtCtl = 6};               /*The PrEvent Proc's ctl number*/
enum {lPrEvtAll = 0x0002FFFD};      /*The PrEvent Proc's CParam for the entire screen*/
enum {lPrEvtTop = 0x0001FFFD};      /*The PrEvent Proc's CParam for the top folder*/
#define sPrDrvr ".Print"
enum {iPrDrvrRef = -3};
enum {getRslDataOp = 4};            /*PrGeneral Cs*/
enum {setRslOp = 5};                /*PrGeneral Cs*/
enum {draftBitsOp = 6};             /*PrGeneral Cs*/
enum {noDraftBitsOp = 7};           /*PrGeneral Cs*/
enum {getRotnOp = 8};               /*PrGeneral Cs*/
enum {NoSuchRsl = 1};               /*PrGeneral Cs*/
enum {RgType1 = 1};                 /*PrGeneral Cs*/

enum {feedCut,feedFanfold,feedMechCut,feedOther};
typedef unsigned char TFeed;

enum {scanTB,scanBT,scanLR,scanRL};
typedef unsigned char TScan;

typedef Rect *TPRect;

typedef pascal void (*PrIdleProcPtr)(void);
typedef pascal void (*PItemProcPtr)(DialogPtr theDialog, short item);

struct TPrPort {
	GrafPort gPort; 				/*The Printer's graf port.*/
	QDProcs gProcs; 				/*..and its procs*/
	long lGParam1;					/*16 bytes for private parameter storage.*/
	long lGParam2;
	long lGParam3;
	long lGParam4;
	Boolean fOurPtr;				/*Whether the PrPort allocation was done by us.*/
	Boolean fOurBits;				/*Whether the BitMap allocation was done by us.*/
};

typedef struct TPrPort TPrPort;
typedef TPrPort *TPPrPort;

/* Printing Graf Port. All printer imaging, whether spooling, banding, etc, happens "thru" a GrafPort.
This is the "PrPeek" record. */
struct TPrInfo {
	short iDev; 					/*Font mgr/QuickDraw device code*/
	short iVRes;					/*Resolution of device, in device coordinates*/
	short iHRes;					/*..note: V before H => compatable with Point.*/
	Rect rPage; 					/*The page (printable) rectangle in device coordinates.*/
};

typedef struct TPrInfo TPrInfo;
typedef TPrInfo *TPPrInfo;

/* Print Info Record: The parameters needed for page composition. */
struct TPrStl {
	short wDev; 					/*The device (driver) number. Hi byte=RefNum, Lo byte=variant. f0 = fHiRes f1 = fPortrait, f2 = fSqPix, f3 = f2xZoom, f4 = fScroll.*/
	short iPageV;					/*paper size in units of 1/iPrPgFract*/
	short iPageH;					/* ..note: V before H => compatable with Point.*/
	char bPort; 					/*The IO port number. Refnum?*/
	TFeed feed; 					/*paper feeder type.*/
};

typedef struct TPrStl TPrStl;
typedef TPrStl *TPPrStl;

/* Printer Style: The printer configuration and usage information. */
struct TPrXInfo {
	short iRowBytes;
	short iBandV;
	short iBandH;
	short iDevBytes;
	short iBands;
	char bPatScale;
	char bUlThick;
	char bUlOffset;
	char bUlShadow;
	TScan scan;
	char bXInfoX;
};

typedef struct TPrXInfo TPrXInfo;
typedef TPrXInfo *TPPrXInfo;

struct TPrJob {
	short iFstPage;
	short iLstPage;
	short iCopies;
	char bJDocLoop;
	Boolean fFromUsr;
	PrIdleProcPtr pIdleProc;
	StringPtr pFileName;
	short iFileVol;
	char bFileVers;
	char bJobX;
};

typedef struct TPrJob TPrJob;
typedef TPrJob *TPPrJob;

struct TPrint {
	short iPrVersion;				/*(2) Printing software version*/
	TPrInfo prInfo; 				/*(14) the PrInfo data associated with the current style.*/
	Rect rPaper;					/*(8) The paper rectangle [offset from rPage]*/
	TPrStl prStl;					/*(8)  This print request's style.*/
	TPrInfo prInfoPT;				/*(14)	Print Time Imaging metrics*/
	TPrXInfo prXInfo;				/*(16)	Print-time (expanded) Print info record.*/
	TPrJob prJob;					/*(20) The Print Job request (82)  Total of the above; 120-82 = 38 bytes needed to fill 120*/
	short printX[19];				/*Spare to fill to 120 bytes!*/
};

typedef struct TPrint TPrint;
typedef TPrint *TPPrint, **THPrint;

/* The universal 120 byte printing record */
struct TPrStatus {
	short iTotPages;				/*Total pages in Print File.*/
	short iCurPage; 				/*Current page number*/
	short iTotCopies;				/*Total copies requested*/
	short iCurCopy; 				/*Current copy number*/
	short iTotBands;				/*Total bands per page.*/
	short iCurBand; 				/*Current band number*/
	Boolean fPgDirty;				/*True if current page has been written to.*/
	Boolean fImaging;				/*Set while in band's DrawPic call.*/
	THPrint hPrint; 				/*Handle to the active Printer record*/
	TPPrPort pPrPort;				/*Ptr to the active PrPort*/
	PicHandle hPic; 				/*Handle to the active Picture*/
};

typedef struct TPrStatus TPrStatus;
typedef TPrStatus *TPPrStatus;

/* Print Status: Print information during printing. */
struct TPfPgDir {
	short iPages;
	long iPgPos[129];				/*ARRAY [0..iPfMaxPgs] OF LONGINT*/
};

typedef struct TPfPgDir TPfPgDir;
typedef TPfPgDir *TPPfPgDir, **THPfPgDir;

/* PicFile = a TPfHeader followed by n QuickDraw Pics (whose PicSize is invalid!) */
struct TPrDlg {
	DialogRecord Dlg;				/*The Dialog window*/
	ModalFilterProcPtr pFltrProc;	/*The Filter Proc.*/
	PItemProcPtr pItemProc; 		/*The Item evaluating proc.*/
	THPrint hPrintUsr;				/*The user's print record.*/
	Boolean fDoIt;
	Boolean fDone;
	long lUser1;					/*Four longs for user's to hang global data.*/
	long lUser2;					/*...Plus more stuff needed by the particular printing dialog.*/
	long lUser3;
	long lUser4;
};

typedef struct TPrDlg TPrDlg;
typedef TPrDlg *TPPrDlg;

typedef pascal TPPrDlg (*PDlgInitProcPtr)(THPrint hPrint);

/* This is the Printing Dialog Record. Only used by folks appending their own dialogs.
Print Dialog: The Dialog Stream object. */


struct TGnlData {
	short iOpCode;
	short iError;
	long lReserved; 				/*more fields here depending on call*/
};

typedef struct TGnlData TGnlData;
struct TRslRg {
	short iMin;
	short iMax;
};

typedef struct TRslRg TRslRg;
struct TRslRec {
	short iXRsl;
	short iYRsl;
};

typedef struct TRslRec TRslRec;
struct TGetRslBlk {
	short iOpCode;
	short iError;
	long lReserved;
	short iRgType;
	TRslRg xRslRg;
	TRslRg yRslRg;
	short iRslRecCnt;
	TRslRec rgRslRec[27];
};

typedef struct TGetRslBlk TGetRslBlk;
struct TSetRslBlk {
	short iOpCode;
	short iError;
	long lReserved;
	THPrint hPrint;
	short iXRsl;
	short iYRsl;
};

typedef struct TSetRslBlk TSetRslBlk;
struct TDftBitsBlk {
	short iOpCode;
	short iError;
	long lReserved;
	THPrint hPrint;
};

typedef struct TDftBitsBlk TDftBitsBlk;
struct TGetRotnBlk {
	short iOpCode;
	short iError;
	long lReserved;
	THPrint hPrint;
	Boolean fLandscape;
	char bXtra;
};

typedef struct TGetRotnBlk TGetRotnBlk;
#ifdef __cplusplus
extern "C" {
#endif
pascal void PrPurge(void)
	= {0x2F3C,0xA800,0x0000,0xA8FD};
pascal void PrNoPurge(void)
	= {0x2F3C,0xB000,0x0000,0xA8FD};
pascal Handle PrDrvrDCE(void)
	= {0x2F3C,0x9400,0x0000,0xA8FD};
pascal short PrDrvrVers(void)
	= {0x2F3C,0x9A00,0x0000,0xA8FD};
pascal void PrOpen(void)
	= {0x2F3C,0xC800,0x0000,0xA8FD};
pascal void PrClose(void)
	= {0x2F3C,0xD000,0x0000,0xA8FD};
pascal void PrintDefault(THPrint hPrint)
	= {0x2F3C,0x2004,0x0480,0xA8FD};
pascal Boolean PrValidate(THPrint hPrint)
	= {0x2F3C,0x5204,0x0498,0xA8FD};
pascal Boolean PrStlDialog(THPrint hPrint)
	= {0x2F3C,0x2A04,0x0484,0xA8FD};
pascal Boolean PrJobDialog(THPrint hPrint)
	= {0x2F3C,0x3204,0x0488,0xA8FD};
pascal void PrJobMerge(THPrint hPrintSrc,THPrint hPrintDst)
	= {0x2F3C,0x5804,0x089C,0xA8FD};
pascal TPPrPort PrOpenDoc(THPrint hPrint,TPPrPort pPrPort,Ptr pIOBuf)
	= {0x2F3C,0x0400,0x0C00,0xA8FD};
pascal void PrCloseDoc(TPPrPort pPrPort)
	= {0x2F3C,0x0800,0x0484,0xA8FD};
pascal void PrOpenPage(TPPrPort pPrPort,TPRect pPageFrame)
	= {0x2F3C,0x1000,0x0808,0xA8FD};
pascal void PrClosePage(TPPrPort pPrPort)
	= {0x2F3C,0x1800,0x040C,0xA8FD};
pascal void PrPicFile(THPrint hPrint,TPPrPort pPrPort,Ptr pIOBuf,Ptr pDevBuf,
	TPrStatus *prStatus)
	= {0x2F3C,0x6005,0x1480,0xA8FD};
pascal short PrError(void)
	= {0x2F3C,0xBA00,0x0000,0xA8FD};
pascal void PrSetError(short iErr)
	= {0x2F3C,0xC000,0x0200,0xA8FD};
pascal void PrGeneral(Ptr pData)
	= {0x2F3C,0x7007,0x0480,0xA8FD};
pascal void PrDrvrOpen(void)
	= {0x2F3C,0x8000,0x0000,0xA8FD};
pascal Boolean PrDlgMain(THPrint hPrint,PDlgInitProcPtr pDlgInit)
	= {0x2F3C,0x4A04,0x0894,0xA8FD};
pascal void PrDrvrClose(void)
	= {0x2F3C,0x8800,0x0000,0xA8FD};
pascal TPPrDlg PrJobInit(THPrint hPrint)
	= {0x2F3C,0x4404,0x0410,0xA8FD};
pascal void PrCtlCall(short iWhichCtl,long lParam1,long lParam2,long lParam3)
	= {0x2F3C,0xA000,0x0E00,0xA8FD};
pascal TPPrDlg PrStlInit(THPrint hPrint)
	= {0x2F3C,0x3C04,0x040C,0xA8FD};
#ifdef __cplusplus
}
#endif

#endif
