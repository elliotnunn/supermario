/************************************************************

Created: Sunday, January 6, 1991 at 9:05 PM
    Desk.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1989
        All rights reserved

************************************************************/


#ifndef __DESK__
#define __DESK__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif


enum {

    accEvent = 64,
    accRun = 65,
    accCursor = 66,
    accMenu = 67,
    accUndo = 68,
    accCut = 70,
    accCopy = 71,
    accPaste = 72,
    accClear = 73,
    goodbye = -1    /*goodbye message*/
};

#ifdef __cplusplus
extern "C" {
#endif
pascal short OpenDeskAcc(ConstStr255Param deskAccName)
    = 0xA9B6; 
pascal void CloseDeskAcc(short refNum)
    = 0xA9B7; 
pascal void SystemClick(const EventRecord *theEvent,WindowPtr theWindow)
    = 0xA9B3; 
pascal Boolean SystemEdit(short editCmd)
    = 0xA9C2; 
pascal void SystemTask(void)
    = 0xA9B4; 
pascal Boolean SystemEvent(const EventRecord *theEvent)
    = 0xA9B2; 
pascal void SystemMenu(long menuResult)
    = 0xA9B5; 
short opendeskacc(char *deskAccName); 
#ifdef __cplusplus
}
#endif

#endif
