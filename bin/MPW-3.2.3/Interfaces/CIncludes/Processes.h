/************************************************************

Created: Sunday, January 6, 1991 at 9:42 PM
    Processes.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1989-1990
        All rights reserved

************************************************************/


#ifndef __PROCESSES__
#define __PROCESSES__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif


/* type for unique process identifier */
struct ProcessSerialNumber {
    unsigned long highLongOfPSN;
    unsigned long lowLongOfPSN;
};

typedef struct ProcessSerialNumber ProcessSerialNumber;
typedef ProcessSerialNumber *ProcessSerialNumberPtr;


enum {


/*************************************************************************
 *                            Process identifier.
 ************************************************************************
 Various reserved process serial numbers. */

    kNoProcess = 0,
    kSystemProcess = 1,
    kCurrentProcess = 2
};

/**********************************************************************************************************************************************
 *        Definition of the parameter block passed to _Launch.
 *************************************************************************

* Typedef and flags for launchControlFlags field */
typedef unsigned short LaunchFlags;

enum {


/*************************************************************************
 *        Definition of the parameter block passed to _Launch.
 *************************************************************************/

    launchContinue = 0x4000,
    launchNoFileFlags = 0x0800,
    launchUseMinimum = 0x0400,
    launchDontSwitch = 0x0200,
    launchAllow24Bit = 0x0100,
    launchInhibitDaemon = 0x0080
};

/* Format for first AppleEvent to pass to new process.  The size of the overall
 * buffer variable: the message body immediately follows the messageLength.
 */
struct AppParameters {
    EventRecord theMsgEvent;
    unsigned long eventRefCon;
    unsigned long messageLength;
};

typedef struct AppParameters AppParameters;
typedef AppParameters *AppParametersPtr;

/* Parameter block to _Launch */
struct LaunchParamBlockRec {
    unsigned long reserved1;
    unsigned short reserved2;
    unsigned short launchBlockID;
    unsigned long launchEPBLength;
    unsigned short launchFileFlags;
    LaunchFlags launchControlFlags;
    FSSpecPtr launchAppSpec;
    ProcessSerialNumber launchProcessSN;
    unsigned long launchPreferredSize;
    unsigned long launchMinimumSize;
    unsigned long launchAvailableSize;
    AppParametersPtr launchAppParameters;
};

typedef struct LaunchParamBlockRec LaunchParamBlockRec;
typedef LaunchParamBlockRec *LaunchPBPtr;


enum {


/* Set launchBlockID to extendedBlock to specify that extensions exist.
* Set launchEPBLength to extendedBlockLen for compatibility.*/


#define extendedBlock ((unsigned short)'LC')
#define extendedBlockLen (sizeof(LaunchParamBlockRec) - 12)

/*************************************************************************
 * Definition of the information block returned by GetProcessInformation
 ************************************************************************
 Bits in the processMode field */


    modeDeskAccessory = 0x00020000,
    modeMultiLaunch = 0x00010000,
    modeNeedSuspendResume = 0x00004000,
    modeCanBackground = 0x00001000,
    modeDoesActivateOnFGSwitch = 0x00000800,
    modeOnlyBackground = 0x00000400,
    modeGetFrontClicks = 0x00000200,
    modeGetAppDiedMsg = 0x00000100,
    mode32BitCompatible = 0x00000080,
    modeHighLevelEventAware = 0x00000040,
    modeLocalAndRemoteHLEvents = 0x00000020,
    modeStationeryAware = 0x00000010,
    modeUseTextEditServices = 0x00000008
};

/* Record returned by GetProcessInformation */
struct ProcessInfoRec {
    unsigned long processInfoLength;
    StringPtr processName;
    ProcessSerialNumber processNumber;
    unsigned long processType;
    OSType processSignature;
    unsigned long processMode;
    Ptr processLocation;
    unsigned long processSize;
    unsigned long processFreeMem;
    ProcessSerialNumber processLauncher;
    unsigned long processLaunchDate;
    unsigned long processActiveTime;
    FSSpecPtr processAppSpec;
};

typedef struct ProcessInfoRec ProcessInfoRec;
typedef ProcessInfoRec *ProcessInfoRecPtr;


#ifdef __cplusplus
extern "C" {
#endif
#pragma parameter __D0 LaunchApplication(__A0)
pascal OSErr LaunchApplication(const LaunchParamBlockRec *LaunchParams)
    = 0xA9F2; 
pascal OSErr LaunchDeskAccessory(const FSSpec *pFileSpec,ConstStr255Param pDAName)
    = {0x3F3C,0x0036,0xA88F}; 
pascal OSErr GetCurrentProcess(ProcessSerialNumber *PSN)
    = {0x3F3C,0x0037,0xA88F}; 
pascal OSErr GetFrontProcess(ProcessSerialNumber *PSN)
    = {0x70FF,0x2F00,0x3F3C,0x0039,0xA88F}; 
pascal OSErr GetNextProcess(ProcessSerialNumber *PSN)
    = {0x3F3C,0x0038,0xA88F}; 
pascal OSErr GetProcessInformation(const ProcessSerialNumber *PSN,ProcessInfoRecPtr info)
    = {0x3F3C,0x003A,0xA88F}; 
pascal OSErr SetFrontProcess(const ProcessSerialNumber *PSN)
    = {0x3F3C,0x003B,0xA88F}; 
pascal OSErr WakeUpProcess(const ProcessSerialNumber *PSN)
    = {0x3F3C,0x003C,0xA88F}; 
pascal OSErr SameProcess(const ProcessSerialNumber *PSN1,const ProcessSerialNumber *PSN2,
    Boolean *result)
    = {0x3F3C,0x003D,0xA88F}; 
#ifdef __cplusplus
}
#endif

#endif
