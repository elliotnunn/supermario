/************************************************************

Created: Sunday, January 6, 1991 at 9:37 PM
    Power.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1989-1990
        All rights reserved

************************************************************/


#ifndef __POWER__
#define __POWER__

#ifndef __TYPES__
#include <Types.h>
#endif


enum {


/* Bit positions for ModemByte */
    modemOnBit = 0,
    ringWakeUpBit = 2,
    modemInstalledBit = 3,
    ringDetectBit = 4,
    modemOnHookBit = 5,

/* masks for ModemByte */
    modemOnMask = 0x1,
    ringWakeUpMask = 0x4,
    modemInstalledMask = 0x8,
    ringDetectMask = 0x10,
    modemOnHookMask = 0x20,

/* bit positions for BatteryByte */
    chargerConnBit = 0,
    hiChargeBit = 1,
    chargeOverFlowBit = 2,
    batteryDeadBit = 3,
    batteryLowBit = 4,
    connChangedBit = 5,

/* masks for BatteryByte */
    chargerConnMask = 0x1,
    hiChargeMask = 0x2,
    chargeOverFlowMask = 0x4,
    batteryDeadMask = 0x8
};
enum {
    batteryLowMask = 0x10,
    connChangedMask = 0x20,

/* commands to SleepQRec sleepQProc */
    sleepRequest = 1,
    sleepDemand = 2,
    sleepWakeUp = 3,
    sleepRevoke = 4,

/* SleepQRec.sleepQFlags */
    noCalls = 1,
    noRequest = 2,

    slpQType = 16,
    sleepQType = 16
};

typedef char ModemByte;
typedef char BatteryByte;

typedef long PMResultCode;

struct SleepQRec {
    struct SleepQRec *sleepQLink;
    short sleepQType;       /*type = 16*/
    ProcPtr sleepQProc;     /*Pointer to sleep routine*/
    short sleepQFlags;
};

typedef struct SleepQRec SleepQRec;
typedef SleepQRec *SleepQRecPtr;


#ifdef __cplusplus
extern "C" {
#endif
pascal OSErr DisableWUTime(void); 
pascal OSErr GetWUTime(long *WUTime,Byte *WUFlag); 
pascal OSErr SetWUTime(long WUTime); 
pascal OSErr BatteryStatus(Byte *Status,Byte *Power); 
pascal OSErr ModemStatus(Byte *Status); 
pascal long IdleUpdate(void)
    = 0xA285; 
pascal long GetCPUSpeed(void)
    = {0x70FF,0xA485}; 
pascal void EnableIdle(void)
    = {0x7000,0xA485}; 
pascal void DisableIdle(void)
    = {0x7001,0xA485}; 
pascal void SleepQInstall(SleepQRecPtr qRecPtr)
    = {0x205F,0xA28A}; 
pascal void SleepQRemove(SleepQRecPtr qRecPtr)
    = {0x205F,0xA48A}; 
pascal void AOn(void)
    = {0x7004,0xA685}; 
pascal void AOnIgnoreModem(void)
    = {0x7005,0xA685}; 
pascal void BOn(void)
    = {0x7000,0xA685}; 
pascal void AOff(void)
    = {0x7084,0xA685}; 
pascal void BOff(void)
    = {0x7080,0xA685}; 
#ifdef __cplusplus
}
#endif

#endif
