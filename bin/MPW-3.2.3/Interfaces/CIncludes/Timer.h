/************************************************************

Created: Sunday, January 6, 1991 at 10:13 PM
    Timer.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved

************************************************************/


#ifndef __TIMER__
#define __TIMER__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


typedef pascal void (*TimerProcPtr)(void);

struct TMTask {
    QElemPtr qLink;
    short qType;
    TimerProcPtr tmAddr;
    long tmCount;
    long tmWakeUp;
    long tmReserved;
};

typedef struct TMTask TMTask;
typedef TMTask *TMTaskPtr;


#ifdef __cplusplus
extern "C" {
#endif
#pragma parameter InsTime(__A0)
pascal void InsTime(QElemPtr tmTaskPtr)
    = 0xA058; 
#pragma parameter InsXTime(__A0)
pascal void InsXTime(QElemPtr tmTaskPtr)
    = 0xA458; 
#pragma parameter PrimeTime(__A0,__D0)
pascal void PrimeTime(QElemPtr tmTaskPtr,long count)
    = 0xA05A; 
#pragma parameter RmvTime(__A0)
pascal void RmvTime(QElemPtr tmTaskPtr)
    = 0xA059; 
#ifdef __cplusplus
}
#endif

#endif
