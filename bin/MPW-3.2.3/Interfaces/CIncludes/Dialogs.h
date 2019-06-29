/************************************************************

Created: Tuesday, January 8, 1991 at 1:03 PM
    Dialogs.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.     1985-1990
        All rights reserved

************************************************************/


#ifndef __DIALOGS__
#define __DIALOGS__

#ifndef __WINDOWS__
#include <Windows.h>
#endif

#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif


enum {

    ctrlItem = 4,
    btnCtrl = 0,
    chkCtrl = 1,
    radCtrl = 2,
    resCtrl = 3,
    statText = 8,
    editText = 16,
    iconItem = 32,
    picItem = 64,
    userItem = 0,
    itemDisable = 128,
    ok = 1,
    cancel = 2,
    stopIcon = 0,
    noteIcon = 1,
    cautionIcon = 2
};

typedef short StageList;
typedef WindowPtr DialogPtr;
typedef pascal void (*ResumeProcPtr)(void);
typedef pascal void (*SoundProcPtr)(void);
typedef pascal Boolean (*ModalFilterProcPtr)(DialogPtr theDialog, EventRecord *theEvent, short *itemHit);

struct DialogRecord {
    WindowRecord window;
    Handle items;
    TEHandle textH;
    short editField;
    short editOpen;
    short aDefItem;
};

typedef struct DialogRecord DialogRecord;
typedef DialogRecord *DialogPeek;

struct DialogTemplate {
    Rect boundsRect;
    short procID;
    Boolean visible;
    Boolean filler1;
    Boolean goAwayFlag;
    Boolean filler2;
    long refCon;
    short itemsID;
    Str255 title;
};

typedef struct DialogTemplate DialogTemplate;
typedef DialogTemplate *DialogTPtr, **DialogTHndl;

struct AlertTemplate {
    Rect boundsRect;
    short itemsID;
    StageList stages;
};

typedef struct AlertTemplate AlertTemplate;
typedef AlertTemplate *AlertTPtr, **AlertTHndl;


#ifdef __cplusplus
extern "C" {
#endif
pascal void InitDialogs(ResumeProcPtr resumeProc)
    = 0xA97B; 
pascal void ErrorSound(SoundProcPtr soundProc)
    = 0xA98C; 
pascal DialogPtr NewDialog(void *wStorage,const Rect *boundsRect,ConstStr255Param title,
    Boolean visible,short procID,WindowPtr behind,Boolean goAwayFlag,long refCon,
    Handle itmLstHndl)
    = 0xA97D; 
DialogPtr newdialog(void *wStorage,const Rect *boundsRect,char *title,Boolean visible,
    short procID,WindowPtr behind,Boolean goAwayFlag,long refCon,Handle itmLstHndl); 
pascal DialogPtr GetNewDialog(short dialogID,void *dStorage,WindowPtr behind)
    = 0xA97C; 
pascal void CloseDialog(DialogPtr theDialog)
    = 0xA982; 
pascal void DisposDialog(DialogPtr theDialog)
    = 0xA983; 
pascal void DisposeDialog(DialogPtr theDialog)
    = 0xA983; 
pascal void CouldDialog(short dialogID)
    = 0xA979; 
pascal void FreeDialog(short dialogID)
    = 0xA97A; 
pascal void ParamText(ConstStr255Param param0,ConstStr255Param param1,ConstStr255Param param2,
    ConstStr255Param param3)
    = 0xA98B; 
pascal void ModalDialog(ModalFilterProcPtr filterProc,short *itemHit)
    = 0xA991; 
pascal Boolean IsDialogEvent(const EventRecord *theEvent)
    = 0xA97F; 
pascal Boolean DialogSelect(const EventRecord *theEvent,DialogPtr *theDialog,
    short *itemHit)
    = 0xA980; 
pascal void DrawDialog(DialogPtr theDialog)
    = 0xA981; 
pascal void UpdtDialog(DialogPtr theDialog,RgnHandle updateRgn)
    = 0xA978; 
pascal void UpdateDialog(DialogPtr theDialog,RgnHandle updateRgn)
    = 0xA978; 
pascal short Alert(short alertID,ModalFilterProcPtr filterProc)
    = 0xA985; 
pascal short StopAlert(short alertID,ModalFilterProcPtr filterProc)
    = 0xA986; 
pascal short NoteAlert(short alertID,ModalFilterProcPtr filterProc)
    = 0xA987; 
pascal short CautionAlert(short alertID,ModalFilterProcPtr filterProc)
    = 0xA988; 
pascal void CouldAlert(short alertID)
    = 0xA989; 
pascal void FreeAlert(short alertID)
    = 0xA98A; 
pascal void GetDItem(DialogPtr theDialog,short itemNo,short *itemType,Handle *item,
    Rect *box)
    = 0xA98D; 
pascal void SetDItem(DialogPtr theDialog,short itemNo,short itemType,Handle item,
    const Rect *box)
    = 0xA98E; 
pascal void HideDItem(DialogPtr theDialog,short itemNo)
    = 0xA827; 
pascal void ShowDItem(DialogPtr theDialog,short itemNo)
    = 0xA828; 
pascal void SelIText(DialogPtr theDialog,short itemNo,short strtSel,short endSel)
    = 0xA97E; 
pascal void GetIText(Handle item,Str255 text)
    = 0xA990; 
pascal void SetIText(Handle item,ConstStr255Param text)
    = 0xA98F; 
pascal short FindDItem(DialogPtr theDialog,Point thePt)
    = 0xA984; 
pascal DialogPtr NewCDialog(void *dStorage,const Rect *boundsRect,ConstStr255Param title,
    Boolean visible,short procID,WindowPtr behind,Boolean goAwayFlag,long refCon,
    Handle items)
    = 0xAA4B; 
DialogPtr newcdialog(void *dStorage,const Rect *boundsRect,char *title,
    Boolean visible,short procID,WindowPtr behind,Boolean goAwayFlag,long refCon,
    Handle items); 
#define GetAlrtStage() (* (short*) 0x0A9A)
pascal void ResetAlrtStage(void)
    = {0x4278,0x0A9A}; 
pascal void DlgCut(DialogPtr theDialog); 
pascal void DlgPaste(DialogPtr theDialog); 
pascal void DlgCopy(DialogPtr theDialog); 
pascal void DlgDelete(DialogPtr theDialog); 
pascal void SetDAFont(short fontNum)
    = {0x31DF,0x0AFA}; 
void paramtext(char *param0,char *param1,char *param2,char *param3); 
void getitext(Handle item,char *text); 
void setitext(Handle item,char *text); 
short findditem(DialogPtr theDialog,Point *thePt); 
#ifdef __cplusplus
}
#endif

#endif
