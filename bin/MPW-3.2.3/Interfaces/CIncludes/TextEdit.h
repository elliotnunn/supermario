/************************************************************

Created: Sunday, January 6, 1991 at 10:11 PM
    TextEdit.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved

************************************************************/


#ifndef __TEXTEDIT__
#define __TEXTEDIT__

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


enum {


/* Justification styles */
    teJustLeft = 0,
    teJustCenter = 1,
    teJustRight = -1,
    teForceLeft = -2,

/* new names for the Justification styles */
    teFlushDefault = 0,                                 /*flush according to the line direction */
    teCenter = 1,                                       /*center justify */
    teFlushRight = -1,                                  /*flush right for all scripts */
    teFlushLeft = -2,                                   /*flush left for all scripts */

/* Set/Replace style modes */
    fontBit = 0,                                        /*set font*/
    faceBit = 1,                                        /*set face*/
    sizeBit = 2,                                        /*set size*/
    clrBit = 3,                                         /*set color*/
    addSizeBit = 4,                                     /*add size mode*/
    toglBit = 5,                                        /*set faces in toggle mode*/

/* TESetStyle/TEContinuousStyle modes */
    doFont = 1,                                         /* set font (family) number*/
    doFace = 2,                                         /*set character style*/
    doSize = 4,                                         /*set type size*/
    doColor = 8,                                        /*set color*/
    doAll = 15,                                         /*set all attributes*/
    addSize = 16                                        /*adjust type size*/
};
enum {
    doToggle = 32,                                      /*toggle mode for TESetStyle & TEContinuousStyle*/

/* offsets into TEDispatchRec */
    EOLHook = 0,                                        /*[ProcPtr] TEEOLHook*/
    DRAWHook = 4,                                       /*[ProcPtr] TEWidthHook*/
    WIDTHHook = 8,                                      /*[ProcPtr] TEDrawHook*/
    HITTESTHook = 12,                                   /*[ProcPtr] TEHitTestHook*/
    nWIDTHHook = 24,                                    /*[ProcPtr] nTEWidthHook*/
    TextWidthHook = 28,                                 /*[ProcPtr] TETextWidthHook*/

/* selectors for TECustomHook */
    intEOLHook = 0,                                     /*TEIntHook value*/
    intDrawHook = 1,                                    /*TEIntHook value*/
    intWidthHook = 2,                                   /*TEIntHook value*/
    intHitTestHook = 3,                                 /*TEIntHook value*/
    intNWidthHook = 6,                                  /*TEIntHook value for new version of WidthHook*/
    intTextWidthHook = 7,                               /*TEIntHook value for new TextWidthHook*/

/* feature or bit definitions for TEFeatureFlag */
    teFAutoScr = 0,                                     /*00000001b*/
    teFTextBuffering = 1,                               /*00000010b*/
    teFOutlineHilite = 2,                               /*00000100b*/
    teFInlineInput = 3,                                 /*00001000b*/
    teFUseTextServices = 4,                             /*00010000b*/

/* action for the new "bit (un)set" interface, TEFeatureFlag */
    TEBitClear = 0,
    TEBitSet = 1                                        /*set the selector bit*/
};
enum {
    TEBitTest = -1,                                     /*no change; just return the current setting*/

/*constants for identifying the routine that called FindWord */
    teWordSelect = 4,                                   /*clickExpand to select word*/
    teWordDrag = 8,                                     /*clickExpand to drag new word*/
    teFromFind = 12,                                    /*FindLine called it ($0C)*/
    teFromRecal = 16                                    /*RecalLines called it ($10)*/
};

typedef pascal Boolean (*WordBreakProcPtr)(Ptr text, short charPos);
typedef pascal Boolean (*ClikLoopProcPtr)(void);

struct TERec {
    Rect destRect;
    Rect viewRect;
    Rect selRect;
    short lineHeight;
    short fontAscent;
    Point selPoint;
    short selStart;
    short selEnd;
    short active;
    WordBreakProcPtr wordBreak;
    ClikLoopProcPtr clikLoop;
    long clickTime;
    short clickLoc;
    long caretTime;
    short caretState;
    short just;
    short teLength;
    Handle hText;
    short recalBack;
    short recalLines;
    short clikStuff;
    short crOnly;
    short txFont;
    Style txFace;                                       /*txFace is unpacked byte*/
    char filler;
    short txMode;
    short txSize;
    GrafPtr inPort;
    ProcPtr highHook;
    ProcPtr caretHook;
    short nLines;
    short lineStarts[16001];
};

typedef struct TERec TERec;
typedef TERec *TEPtr, **TEHandle;

typedef char Chars[32001];
typedef Chars *CharsPtr,**CharsHandle;

struct StyleRun {
    short startChar;                                    /*starting character position*/
    short styleIndex;                                   /*index in style table*/
};

typedef struct StyleRun StyleRun;

struct STElement {
    short stCount;                                      /*number of runs in this style*/
    short stHeight;                                     /*line height*/
    short stAscent;                                     /*font ascent*/
    short stFont;                                       /*font (family) number*/
    Style stFace;                                       /*character Style*/
    char filler;                                        /*stFace is unpacked byte*/
    short stSize;                                       /*size in points*/
    RGBColor stColor;                                   /*absolute (RGB) color*/
};

typedef struct STElement STElement;

typedef STElement TEStyleTable[1777], *STPtr, **STHandle;

struct LHElement {
    short lhHeight;                                     /*maximum height in line*/
    short lhAscent;                                     /*maximum ascent in line*/
};

typedef struct LHElement LHElement;

typedef LHElement LHTable[8001], *LHPtr, **LHHandle;    /* ARRAY [0..8000] OF LHElement */

struct ScrpSTElement {
    long scrpStartChar;                                 /*starting character position*/
    short scrpHeight;                                   /*starting character position*/
    short scrpAscent;
    short scrpFont;
    Style scrpFace;                                     /*unpacked byte*/
    char filler;                                        /*scrpFace is unpacked byte*/
    short scrpSize;
    RGBColor scrpColor;
};

typedef struct ScrpSTElement ScrpSTElement;

typedef ScrpSTElement ScrpSTTable[1601];                /* ARRAY [0..1600] OF ScrpSTElement */

struct StScrpRec {
    short scrpNStyles;                                  /*number of styles in scrap*/
    ScrpSTTable scrpStyleTab;                           /*table of styles for scrap*/
};

typedef struct StScrpRec StScrpRec;
typedef StScrpRec *StScrpPtr, **StScrpHandle;

struct NullStRec {
    long teReserved;                                    /*reserved for future expansion*/
    StScrpHandle nullScrap;                             /*handle to scrap style table*/
};

typedef struct NullStRec NullStRec;
typedef NullStRec *NullStPtr, **NullStHandle;

struct TEStyleRec {
    short nRuns;                                        /*number of style runs*/
    short nStyles;                                      /*size of style table*/
    STHandle styleTab;                                  /*handle to style table*/
    LHHandle lhTab;                                     /*handle to line-height table*/
    long teRefCon;                                      /*reserved for application use*/
    NullStHandle nullStyle;                             /*Handle to style set at null selection*/
    StyleRun runs[8001];                                /*ARRAY [0..8000] OF StyleRun*/
};

typedef struct TEStyleRec TEStyleRec;
typedef TEStyleRec *TEStylePtr, **TEStyleHandle;

struct TextStyle {
    short tsFont;                                       /*font (family) number*/
    Style tsFace;                                       /*character Style*/
    char filler;                                        /*tsFace is unpacked byte*/
    short tsSize;                                       /*size in point*/
    RGBColor tsColor;                                   /*absolute (RGB) color*/
};

typedef struct TextStyle TextStyle;
typedef TextStyle *TextStylePtr, **TextStyleHandle;


typedef short TEIntHook;

#ifdef __cplusplus
extern "C" {
#endif
pascal void TEInit(void)
    = 0xA9CC; 
pascal TEHandle TENew(const Rect *destRect,const Rect *viewRect)
    = 0xA9D2; 
pascal void TEDispose(TEHandle hTE)
    = 0xA9CD; 
pascal void TESetText(const void *text,long length,TEHandle hTE)
    = 0xA9CF; 
pascal CharsHandle TEGetText(TEHandle hTE)
    = 0xA9CB; 
pascal void TEIdle(TEHandle hTE)
    = 0xA9DA; 
pascal void TESetSelect(long selStart,long selEnd,TEHandle hTE)
    = 0xA9D1; 
pascal void TEActivate(TEHandle hTE)
    = 0xA9D8; 
pascal void TEDeactivate(TEHandle hTE)
    = 0xA9D9; 
pascal void TEKey(short key,TEHandle hTE)
    = 0xA9DC; 
pascal void TECut(TEHandle hTE)
    = 0xA9D6; 
pascal void TECopy(TEHandle hTE)
    = 0xA9D5; 
pascal void TEPaste(TEHandle hTE)
    = 0xA9DB; 
pascal void TEDelete(TEHandle hTE)
    = 0xA9D7; 
pascal void TEInsert(const void *text,long length,TEHandle hTE)
    = 0xA9DE; 
pascal void TESetJust(short just,TEHandle hTE)
    = 0xA9DF; 
pascal void TEUpdate(const Rect *rUpdate,TEHandle hTE)
    = 0xA9D3; 
pascal void TextBox(const void *text,long length,const Rect *box,short just)
    = 0xA9CE; 
pascal void TEScroll(short dh,short dv,TEHandle hTE)
    = 0xA9DD; 
pascal void TESelView(TEHandle hTE)
    = 0xA811; 
pascal void TEPinScroll(short dh,short dv,TEHandle hTE)
    = 0xA812; 
pascal void TEAutoView(Boolean fAuto,TEHandle hTE)
    = 0xA813; 
#define TEScrapHandle() (* (Handle*) 0xAB4)
pascal void TECalText(TEHandle hTE)
    = 0xA9D0; 
pascal short TEGetOffset(Point pt,TEHandle hTE)
    = 0xA83C; 
pascal Point TEGetPoint(short offset,TEHandle hTE)
    = {0x3F3C,0x0008,0xA83D}; 
pascal void TEClick(Point pt,Boolean fExtend,TEHandle h)
    = 0xA9D4; 
pascal TEHandle TEStylNew(const Rect *destRect,const Rect *viewRect)
    = 0xA83E; 
pascal void SetStylHandle(TEStyleHandle theHandle,TEHandle hTE)
    = {0x3F3C,0x0005,0xA83D}; 
pascal TEStyleHandle GetStylHandle(TEHandle hTE)
    = {0x3F3C,0x0004,0xA83D}; 
pascal void TEGetStyle(short offset,TextStyle *theStyle,short *lineHeight,
    short *fontAscent,TEHandle hTE)
    = {0x3F3C,0x0003,0xA83D}; 
pascal void TEStylPaste(TEHandle hTE)
    = {0x3F3C,0x0000,0xA83D}; 
pascal void TESetStyle(short mode,const TextStyle *newStyle,Boolean redraw,
    TEHandle hTE)
    = {0x3F3C,0x0001,0xA83D}; 
pascal void TEReplaceStyle(short mode,const TextStyle *oldStyle,const TextStyle *newStyle,
    Boolean redraw,TEHandle hTE)
    = {0x3F3C,0x0002,0xA83D}; 
pascal StScrpHandle GetStylScrap(TEHandle hTE)
    = {0x3F3C,0x0006,0xA83D}; 
pascal void TEStylInsert(const void *text,long length,StScrpHandle hST,
    TEHandle hTE)
    = {0x3F3C,0x0007,0xA83D}; 
pascal long TEGetHeight(long endLine,long startLine,TEHandle hTE)
    = {0x3F3C,0x0009,0xA83D}; 
pascal Boolean TEContinuousStyle(short *mode,TextStyle *aStyle,TEHandle hTE)
    = {0x3F3C,0x000A,0xA83D}; 
pascal void SetStylScrap(long rangeStart,long rangeEnd,StScrpHandle newStyles,
    Boolean redraw,TEHandle hTE)
    = {0x3F3C,0x000B,0xA83D}; 
pascal void TECustomHook(TEIntHook which,ProcPtr *addr,TEHandle hTE)
    = {0x3F3C,0x000C,0xA83D}; 
pascal long TENumStyles(long rangeStart,long rangeEnd,TEHandle hTE)
    = {0x3F3C,0x000D,0xA83D}; 
pascal short TEFeatureFlag(short feature,short action,TEHandle hTE)
    = {0x3F3C,0x000E,0xA83D}; 
#define TEGetScrapLen() ((long) * (unsigned short *) 0x0AB0)
pascal void TESetScrapLen(long length); 
pascal OSErr TEFromScrap(void); 
pascal OSErr TEToScrap(void); 
pascal void SetClikLoop(ClikLoopProcPtr clikProc,TEHandle hTE); 
pascal void SetWordBreak(WordBreakProcPtr wBrkProc,TEHandle hTE); 
void teclick(Point *pt,Boolean fExtend,TEHandle h); 
#ifdef __cplusplus
}
#endif

#endif
