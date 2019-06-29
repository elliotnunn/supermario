/************************************************************

Created: Sunday, January 6, 1991 at 9:03 PM
    Controls.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc. 1985-1990
        All rights reserved

************************************************************/


#ifndef __CONTROLS__
#define __CONTROLS__

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


enum {

    pushButProc = 0,
    checkBoxProc = 1,
    radioButProc = 2,
    useWFont = 8,
    scrollBarProc = 16,
    inButton = 10,
    inCheckBox = 11,
    inUpButton = 20,
    inDownButton = 21,
    inPageUp = 22,
    inPageDown = 23,
    inThumb = 129,

    popupMenuProc = 1008,   /* 63 * 16 */
    inLabel = 1,
    inMenu = 2,
    inTriangle = 4
};

enum {    popupFixedWidth        = 1 << 0,    popupReserved            = 1 << 1,    popupUseAddResMenu    = 1 << 2,
        popupUseWFont        = 1 << 3};

enum {    popupTitleBold        = 1 << 8,    popupTitleItalic    = 1 << 9,    popupTitleUnderline    = 1 << 10,
        popupTitleOutline    = 1 << 11,    popupTitleShadow    = 1 << 12,    popupTitleCondense    = 1 << 13,
        popupTitleExtend    = 1 << 14,    popupTitleNoStyle    = 1 << 15};


enum {

    popupTitleLeftJust = 0x00000000,
    popupTitleCenterJust = 0x00000001,
    popupTitleRightJust = 0x000000FF,

/*
axis constraints for DragGrayRgn call*/
    noConstraint = 0,
    hAxisOnly = 1,
    vAxisOnly = 2,

/*
control messages*/
    drawCntl = 0,
    testCntl = 1,
    calcCRgns = 2,
    initCntl = 3,
    dispCntl = 4,
    posCntl = 5,
    thumbCntl = 6,
    dragCntl = 7,
    autoTrack = 8,
    calcCntlRgn = 10,
    calcThumbRgn = 11,

    cFrameColor = 0,
    cBodyColor = 1,
    cTextColor = 2
};
enum {
    cThumbColor = 3
};

struct ControlRecord {
    struct ControlRecord **nextControl;
    WindowPtr contrlOwner;
    Rect contrlRect;
    unsigned char contrlVis;
    unsigned char contrlHilite;
    short contrlValue;
    short contrlMin;
    short contrlMax;
    Handle contrlDefProc;
    Handle contrlData;
    ProcPtr contrlAction;
    long contrlRfCon;
    Str255 contrlTitle;
};

typedef struct ControlRecord ControlRecord;
typedef ControlRecord *ControlPtr, **ControlHandle;

struct CtlCTab {
    long ccSeed;            /*reserved*/
    short ccRider;          /*see what you have done - reserved*/
    short ctSize;           /*usually 3 for controls*/
    ColorSpec ctTable[4];
};

typedef struct CtlCTab CtlCTab;
typedef CtlCTab *CCTabPtr, **CCTabHandle;

struct AuxCtlRec {
    Handle acNext;          /*handle to next AuxCtlRec*/
    ControlHandle acOwner;  /*handle for aux record's control*/
    CCTabHandle acCTable;   /*color table for this control*/
    short acFlags;          /*misc flag byte*/
    long acReserved;        /*reserved for use by Apple*/
    long acRefCon;          /*for use by application*/
};

typedef struct AuxCtlRec AuxCtlRec;
typedef AuxCtlRec *AuxCtlPtr, **AuxCtlHandle;


#ifdef __cplusplus
extern "C" {
#endif
pascal ControlHandle NewControl(WindowPtr theWindow,const Rect *boundsRect,
    ConstStr255Param title,Boolean visible,short value,short min,short max,
    short procID,long refCon)
    = 0xA954; 
pascal void SetCTitle(ControlHandle theControl,ConstStr255Param title)
    = 0xA95F; 
pascal void GetCTitle(ControlHandle theControl,Str255 title)
    = 0xA95E; 
pascal ControlHandle GetNewControl(short controlID,WindowPtr owner)
    = 0xA9BE; 
pascal void DisposeControl(ControlHandle theControl)
    = 0xA955; 
pascal void KillControls(WindowPtr theWindow)
    = 0xA956; 
pascal void HideControl(ControlHandle theControl)
    = 0xA958; 
pascal void ShowControl(ControlHandle theControl)
    = 0xA957; 
pascal void DrawControls(WindowPtr theWindow)
    = 0xA969; 
pascal void Draw1Control(ControlHandle theControl)
    = 0xA96D; 
pascal void HiliteControl(ControlHandle theControl,short hiliteState)
    = 0xA95D; 
pascal void UpdtControl(WindowPtr theWindow,RgnHandle updateRgn)
    = 0xA953; 
pascal void UpdateControls(WindowPtr theWindow,RgnHandle updateRgn)
    = 0xA953; 
pascal void MoveControl(ControlHandle theControl,short h,short v)
    = 0xA959; 
pascal void SizeControl(ControlHandle theControl,short w,short h)
    = 0xA95C; 
pascal void SetCtlValue(ControlHandle theControl,short theValue)
    = 0xA963; 
pascal short GetCtlValue(ControlHandle theControl)
    = 0xA960; 
pascal void SetCtlMin(ControlHandle theControl,short minValue)
    = 0xA964; 
pascal short GetCtlMin(ControlHandle theControl)
    = 0xA961; 
pascal void SetCtlMax(ControlHandle theControl,short maxValue)
    = 0xA965; 
pascal short GetCtlMax(ControlHandle theControl)
    = 0xA962; 
pascal void SetCRefCon(ControlHandle theControl,long data)
    = 0xA95B; 
pascal long GetCRefCon(ControlHandle theControl)
    = 0xA95A; 
pascal void SetCtlAction(ControlHandle theControl,ProcPtr actionProc)
    = 0xA96B; 
pascal ProcPtr GetCtlAction(ControlHandle theControl)
    = 0xA96A; 
pascal void DragControl(ControlHandle theControl,Point startPt,const Rect *limitRect,
    const Rect *slopRect,short axis)
    = 0xA967; 
pascal short TestControl(ControlHandle theControl,Point thePt)
    = 0xA966; 
pascal short TrackControl(ControlHandle theControl,Point thePoint,ProcPtr actionProc)
    = 0xA968; 
pascal short FindControl(Point thePoint,WindowPtr theWindow,ControlHandle *theControl)
    = 0xA96C; 
pascal void SetCtlColor(ControlHandle theControl,CCTabHandle newColorTable)
    = 0xAA43; 
pascal Boolean GetAuxCtl(ControlHandle theControl,AuxCtlHandle *acHndl)
    = 0xAA44; 
pascal short GetCVariant(ControlHandle theControl)
    = 0xA809; 
void dragcontrol(ControlHandle theControl,Point *startPt,const Rect *limitRect,
    const Rect *slopRect,short axis); 
ControlHandle newcontrol(WindowPtr theWindow,const Rect *boundsRect,char *title,
    Boolean visible,short value,short min,short max,short procID,long refCon); 
short findcontrol(Point *thePoint,WindowPtr theWindow,ControlHandle *theControl); 
void getctitle(ControlHandle theControl,char *title); 
void setctitle(ControlHandle theControl,char *title); 
short trackcontrol(ControlHandle theControl,Point *thePoint,ProcPtr actionProc); 
short testcontrol(ControlHandle theControl,Point *thePt); 
#ifdef __cplusplus
}
#endif

#endif
