/************************************************************

Created: Monday, January 14, 1991 at 4:39 PM
    Events.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1989
        All rights reserved

************************************************************/


#ifndef __EVENTS__
#define __EVENTS__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


enum {

    nullEvent = 0,
    mouseDown = 1,
    mouseUp = 2,
    keyDown = 3,
    keyUp = 4,
    autoKey = 5,
    updateEvt = 6,
    diskEvt = 7,
    activateEvt = 8,
    osEvt = 15,

/* event mask equates */
    mDownMask = 2,
    mUpMask = 4,
    keyDownMask = 8,
    keyUpMask = 16,
    autoKeyMask = 32,
    updateMask = 64,
    diskMask = 128,
    activMask = 256,
    highLevelEventMask = 1024,
    osMask = -32768
};
enum {
    everyEvent = -1,

/* event message equates */
    charCodeMask = 0x000000FF,
    keyCodeMask = 0x0000FF00,
    adbAddrMask = 0x00FF0000,
    osEvtMessageMask = 0xFF000000,

/* OSEvent Messages */
    mouseMovedMessage = 0xFA,
    suspendResumeMessage = 0x01,
    resumeFlag = 1,             /* bit 0 of message indicates resume vs suspend */
    convertClipboardFlag = 2,   /* bit 1 in resume message indicates clipboard change */

/* modifiers */
    activeFlag = 1,             /*bit 0 of modifiers for activate event*/
    btnState = 128,             /*Bit 7 of low byte is mouse button state*/
    cmdKey = 256,               /*Bit 0*/
    shiftKey = 512,             /*Bit 1*/
    alphaLock = 1024,           /*Bit 2 */
    optionKey = 2048,           /*Bit 3 of high byte*/
    controlKey = 4096,

/* obsolete equates*/
    networkEvt = 10,
    driverEvt = 11,
    app1Evt = 12,
    app2Evt = 13
};
enum {
    app3Evt = 14,
    app4Evt = 15,
    networkMask = 1024,
    driverMask = 2048,
    app1Mask = 4096,
    app2Mask = 8192,
    app3Mask = 16384,
    app4Mask = -32768
};

struct EventRecord {
    short what;
    long message;
    long when;
    Point where;
    short modifiers;
};

typedef struct EventRecord EventRecord;


typedef long KeyMap[4];

#ifdef __cplusplus
extern "C" {
#endif
pascal Boolean GetNextEvent(short eventMask,EventRecord *theEvent)
    = 0xA970; 
pascal Boolean WaitNextEvent(short eventMask,EventRecord *theEvent,unsigned long sleep,
    RgnHandle mouseRgn)
    = 0xA860; 
pascal Boolean EventAvail(short eventMask,EventRecord *theEvent)
    = 0xA971; 
pascal void GetMouse(Point *mouseLoc)
    = 0xA972; 
pascal Boolean Button(void)
    = 0xA974; 
pascal Boolean StillDown(void)
    = 0xA973; 
pascal Boolean WaitMouseUp(void)
    = 0xA977; 
pascal void GetKeys(KeyMap theKeys)
    = 0xA976; 
pascal unsigned long TickCount(void)
    = 0xA975; 
#define GetDblTime() (* (unsigned long*) 0x02F0)
#define GetCaretTime() (* (unsigned long*) 0x02F4)
#ifdef __cplusplus
}
#endif

#endif
