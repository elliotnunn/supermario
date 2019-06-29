/************************************************************

Created: Sunday, January 6, 1991 at 9:10 PM
    EPPC.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1988-1990
        All rights reserved

************************************************************/


#ifndef __EPPC__
#define __EPPC__

#ifndef __PPCTOOLBOX__
#include <PPCToolbox.h>
#endif

#ifndef __PROCESSES__
#include <Processes.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif


enum {

    kHighLevelEvent = 23,

/* postOptions currently supported */
    receiverIDMask = 0x0000F000,
    receiverIDisPSN = 0x00008000,
    receiverIDisSignature = 0x00007000,
    receiverIDisSessionID = 0x00006000,
    receiverIDisTargetID = 0x00005000,

    systemOptionsMask = 0x00000F00,
    nReturnReceipt = 0x00000200,

    priorityMask = 0x000000FF,
    nAttnMsg = 0x00000001,


/* error returns from Post and Accept */

    bufferIsSmall = -607,
    noOutstandingHLE = -608,
    connectionInvalid = -609,
    noUserInteractionAllowed = -610,    /* no user interaction allowed */

/* constant for return receipts */


#define HighLevelEventMsgClass 'jaym'
#define rtrnReceiptMsgID 'rtrn'

    msgWasPartiallyAccepted = 2,
    msgWasFullyAccepted = 1,
    msgWasNotAccepted = 0
};

struct TargetID {
    long sessionID;
    PPCPortRec name;
    LocationNameRec location;
    PPCPortRec recvrName;
};

typedef struct TargetID TargetID;
typedef TargetID *TargetIDPtr, **TargetIDHdl;


typedef TargetID SenderID;
typedef SenderID *SenderIDPtr;

struct HighLevelEventMsg {
    unsigned short HighLevelEventMsgHeaderLength;
    unsigned short version;
    unsigned long reserved1;
    EventRecord theMsgEvent;
    unsigned long userRefcon;
    unsigned long postingOptions;
    unsigned long msgLength;
};

typedef struct HighLevelEventMsg HighLevelEventMsg;
typedef HighLevelEventMsg *HighLevelEventMsgPtr, **HighLevelEventMsgHdl;


#ifdef __cplusplus
extern "C" {
#endif
pascal OSErr PostHighLevelEvent(const EventRecord *theEvent,
                                unsigned long receiverID,
                                unsigned long msgRefcon,
                                Ptr msgBuff,
                                unsigned long msgLen,
                                unsigned long postingOptions)
    = {0x3F3C,0x0034,0xA88F}; 
pascal OSErr AcceptHighLevelEvent(TargetID *sender,
                                  unsigned long *msgRefcon,
                                  Ptr msgBuff,
                                  unsigned long *msgLen)
    = {0x3F3C,0x0033,0xA88F}; 
pascal OSErr GetProcessSerialNumberFromPortName(const PPCPortPtr portName,
    ProcessSerialNumberPtr pPSN)
    = {0x3F3C,0x0035,0xA88F}; 
pascal OSErr GetPortNameFromProcessSerialNumber(PPCPortPtr portName,const ProcessSerialNumberPtr pPSN)
    = {0x3F3C,0x0046,0xA88F}; 
#ifdef __cplusplus
}
#endif
typedef pascal Boolean (*GetSpecificFilterProcPtr) (void *yourDataPtr,
     HighLevelEventMsgPtr msgBuff,
     const TargetID *sender);

#ifdef __cplusplus
extern "C" {
#endif
pascal Boolean GetSpecificHighLevelEvent(GetSpecificFilterProcPtr aFilter,
    void *yourDataPtr,OSErr *err)
    = {0x3F3C,0x0045,0xA88F}; 
#ifdef __cplusplus
}
#endif

#endif
