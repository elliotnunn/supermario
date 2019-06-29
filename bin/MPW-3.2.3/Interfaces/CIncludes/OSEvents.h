/************************************************************

Created: Sunday, January 6, 1991 at 9:34 PM
    OSEvents.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved

************************************************************/


#ifndef __OSEVENTS__
#define __OSEVENTS__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


struct EvQEl {
    QElemPtr qLink;
    short qType;
    short evtQWhat;     /*this part is identical to the EventRecord as...*/
    long evtQMessage;   /*defined in ToolIntf*/
    long evtQWhen;
    Point evtQWhere;
    short evtQModifiers;
};

typedef struct EvQEl EvQEl;
typedef EvQEl *EvQElPtr;


#ifdef __cplusplus
extern "C" {
#endif
#pragma parameter __D0 PostEvent(__A0,__D0)
pascal OSErr PostEvent(short eventNum,long eventMsg)
    = 0xA02F; 
#pragma parameter __D0 PPostEvent(__A0,__D0,__A1)
pascal OSErr PPostEvent(short eventCode,long eventMsg,EvQElPtr *qEl)
    = {0xA12F,0x2288}; 
#pragma parameter __D0 OSEventAvail(__D0,__A0)
pascal Boolean OSEventAvail(short mask,EventRecord *theEvent)
    = {0xA030,0x5240}; 
#pragma parameter __D0 GetOSEvent(__D0,__A0)
pascal Boolean GetOSEvent(short mask,EventRecord *theEvent)
    = {0xA031,0x5240}; 
pascal void FlushEvents(short whichMask,short stopMask)
    = {0x201F,0xA032}; 
#pragma parameter SetEventMask(__D0)
pascal void SetEventMask(short theMask)
    = {0x31C0,0x0144}; 
#define GetEvQHdr() ((QHdrPtr) 0x014A)
#ifdef __cplusplus
}
#endif

#endif
