/************************************************************

Created: Sunday, January 6, 1991 at 9:51 PM
    Retrace.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved

************************************************************/


#ifndef __RETRACE__
#define __RETRACE__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


typedef pascal void (*VBLProcPtr)(void);

struct VBLTask {
    QElemPtr qLink;
    short qType;
    VBLProcPtr vblAddr;
    short vblCount;
    short vblPhase;
};

typedef struct VBLTask VBLTask;


#ifdef __cplusplus
extern "C" {
#endif
#define GetVBLQHdr() ((QHdrPtr) 0x0160)
#pragma parameter __D0 SlotVInstall(__A0,__D0)
pascal OSErr SlotVInstall(QElemPtr vblBlockPtr,short theSlot)
    = 0xA06F; 
#pragma parameter __D0 SlotVRemove(__A0,__D0)
pascal OSErr SlotVRemove(QElemPtr vblBlockPtr,short theSlot)
    = 0xA070; 
#pragma parameter __D0 AttachVBL(__D0)
pascal OSErr AttachVBL(short theSlot)
    = 0xA071; 
#pragma parameter __D0 DoVBLTask(__D0)
pascal OSErr DoVBLTask(short theSlot)
    = 0xA072; 
#pragma parameter __D0 VInstall(__A0)
pascal OSErr VInstall(QElemPtr vblTaskPtr)
    = 0xA033; 
#pragma parameter __D0 VRemove(__A0)
pascal OSErr VRemove(QElemPtr vblTaskPtr)
    = 0xA034; 
#ifdef __cplusplus
}
#endif

#endif
