/************************************************************

Created: Sunday, January 6, 1991 at 9:06 PM
    Devices.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.     1985-1990
        All rights reserved

************************************************************/


#ifndef __DEVICES__
#define __DEVICES__

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


enum {

    newSelMsg = 12,
    fillListMsg = 13,
    getSelMsg = 14,
    selectMsg = 15,
    deselectMsg = 16,
    terminateMsg = 17,
    buttonMsg = 19,
    chooserID = 1,
    initDev = 0,        /*Time for cdev to initialize itself*/
    hitDev = 1,         /*Hit on one of my items*/
    closeDev = 2,       /*Close yourself*/
    nulDev = 3,         /*Null event*/
    updateDev = 4,      /*Update event*/
    activDev = 5,       /*Activate event*/
    deactivDev = 6,     /*Deactivate event*/
    keyEvtDev = 7,      /*Key down/auto key*/
    macDev = 8,         /*Decide whether or not to show up*/
    undoDev = 9,
    cutDev = 10,
    copyDev = 11
};
enum {
    pasteDev = 12,
    clearDev = 13,
    cursorDev = 14,
    cdevGenErr = -1,    /*General error; gray cdev w/o alert*/
    cdevMemErr = 0,     /*Memory shortfall; alert user please*/
    cdevResErr = 1,     /*Couldn't get a needed resource; alert*/
    cdevUnset = 3,      /* cdevValue is initialized to this*/

/* Monitors control panel messages */
    initMsg = 1,        /*initialization*/
    okMsg = 2,          /*user clicked OK button*/
    cancelMsg = 3,      /*user clicked Cancel button*/
    hitMsg = 4,         /*user clicked control in Options dialog*/
    nulMsg = 5,         /*periodic event*/
    updateMsg = 6,      /*update event*/
    activateMsg = 7,    /*not used*/
    deactivateMsg = 8,  /*not used*/
    keyEvtMsg = 9,      /*keyboard event*/
    superMsg = 10,      /*show superuser controls*/
    normalMsg = 11,     /*show only normal controls*/
    startupMsg = 12     /*code has been loaded*/
};

struct DCtlEntry {
    Ptr dCtlDriver;
    short dCtlFlags;
    QHdr dCtlQHdr;
    long dCtlPosition;
    Handle dCtlStorage;
    short dCtlRefNum;
    long dCtlCurTicks;
    WindowPtr dCtlWindow;
    short dCtlDelay;
    short dCtlEMask;
    short dCtlMenu;
};

typedef struct DCtlEntry DCtlEntry;
typedef DCtlEntry *DCtlPtr, **DCtlHandle;

struct AuxDCE {
    Ptr dCtlDriver;
    short dCtlFlags;
    QHdr dCtlQHdr;
    long dCtlPosition;
    Handle dCtlStorage;
    short dCtlRefNum;
    long dCtlCurTicks;
    GrafPtr dCtlWindow;
    short dCtlDelay;
    short dCtlEMask;
    short dCtlMenu;
    char dCtlSlot;
    char dCtlSlotId;
    long dCtlDevBase;
    Ptr dCtlOwner;
    char dCtlExtDev;
    char fillByte;
};

typedef struct AuxDCE AuxDCE;
typedef AuxDCE *AuxDCEPtr, **AuxDCEHandle;


#ifdef __cplusplus
extern "C" {
#endif
pascal DCtlHandle GetDCtlEntry(short refNum); 
pascal Boolean SetChooserAlert(Boolean f); 
pascal OSErr OpenDriver(ConstStr255Param name,short *drvrRefNum); 
OSErr opendriver(char *driverName,short *refNum); 
pascal OSErr CloseDriver(short refNum); 
pascal OSErr Control(short refNum,short csCode,const void *csParamPtr); 
pascal OSErr Status(short refNum,short csCode,void *csParamPtr); 
pascal OSErr KillIO(short refNum); 
pascal OSErr PBControl(ParmBlkPtr paramBlock,Boolean async); 
#pragma parameter __D0 PBControlSync(__A0)
pascal OSErr PBControlSync(ParmBlkPtr paramBlock)
    = 0xA004; 
#pragma parameter __D0 PBControlAsync(__A0)
pascal OSErr PBControlAsync(ParmBlkPtr paramBlock)
    = 0xA404; 
pascal OSErr PBStatus(ParmBlkPtr paramBlock,Boolean async); 
#pragma parameter __D0 PBStatusSync(__A0)
pascal OSErr PBStatusSync(ParmBlkPtr paramBlock)
    = 0xA005; 
#pragma parameter __D0 PBStatusAsync(__A0)
pascal OSErr PBStatusAsync(ParmBlkPtr paramBlock)
    = 0xA405; 
pascal OSErr PBKillIO(ParmBlkPtr paramBlock,Boolean async); 
#pragma parameter __D0 PBKillIOSync(__A0)
pascal OSErr PBKillIOSync(ParmBlkPtr paramBlock)
    = 0xA006; 
#pragma parameter __D0 PBKillIOAsync(__A0)
pascal OSErr PBKillIOAsync(ParmBlkPtr paramBlock)
    = 0xA406; 
#ifdef __cplusplus
}
#endif

#endif
