/************************************************************

Created: Sunday, January 6, 1991 at 9:33 PM
    Notification.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1989-90
        All rights reserved

************************************************************/


#ifndef __NOTIFICATION__
#define __NOTIFICATION__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


enum {

    nmType = 8
};

typedef pascal void (*NMProcPtr)(struct NMRec *);

struct NMRec {
    QElemPtr qLink;     /*next queue entry*/
    short qType;        /*queue type -- ORD(nmType) = 8*/
    short nmFlags;      /*reserved*/
    long nmPrivate;     /*reserved*/
    short nmReserved;   /*reserved*/
    short nmMark;       /*item to mark in Apple menu*/
    Handle nmIcon;      /*handle to small icon*/
    Handle nmSound;     /*handle to sound record*/
    StringPtr nmStr;    /*string to appear in alert*/
    NMProcPtr nmResp;   /*pointer to response routine*/
    long nmRefCon;      /*for application use*/
};

typedef struct NMRec NMRec;
typedef NMRec *NMRecPtr;


#ifdef __cplusplus
extern "C" {
#endif
pascal OSErr NMInstall(NMRecPtr nmReqPtr)
    = {0x205F,0xA05E,0x3E80}; 
pascal OSErr NMRemove(NMRecPtr nmReqPtr)
    = {0x205F,0xA05F,0x3E80}; 
#ifdef __cplusplus
}
#endif

#endif
