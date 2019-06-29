/************************************************************

Created: Sunday, January 6, 1991 at 9:34 PM
    OSUtils.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved

************************************************************/


#ifndef __OSUTILS__
#define __OSUTILS__

#ifndef __TYPES__
#include <Types.h>
#endif


enum {

    useFree = 0,
    useATalk = 1,
    useAsync = 2,
    useExtClk = 3,      /*Externally clocked*/
    useMIDI = 4,

/**** Environs Equates ****/
    curSysEnvVers = 2,  /*Updated to equal latest SysEnvirons version*/

/* Machine Types */
    envMac = -1,
    envXL = -2,
    envMachUnknown = 0,
    env512KE = 1,
    envMacPlus = 2,
    envSE = 3,
    envMacII = 4,
    envMacIIx = 5,
    envMacIIcx = 6,
    envSE30 = 7,
    envPortable = 8,
    envMacIIci = 9,
    envMacIIfx = 11,

/* CPU types */
    envCPUUnknown = 0
};
enum {
    env68000 = 1,
    env68010 = 2,
    env68020 = 3,
    env68030 = 4,
    env68040 = 5,

/* Keyboard types */
    envUnknownKbd = 0,
    envMacKbd = 1,
    envMacAndPad = 2,
    envMacPlusKbd = 3,
    envAExtendKbd = 4,
    envStandADBKbd = 5,
    envPrtblADBKbd = 6,
    envPrtblISOKbd = 7,
    envStdISOADBKbd = 8,
    envExtISOADBKbd = 9,
    false32b = 0,       /*24 bit addressing error*/
    true32b = 1,        /*32 bit addressing error*/

/* result types for RelString Call */
    sortsBefore = -1,   /*first string < second string*/
    sortsEqual = 0,     /*first string = second string*/
    sortsAfter = 1      /*first string > second string*/
};


enum {dummyType,vType,ioQType,drvQType,evType,fsQType,sIQType,dtQType};
typedef unsigned short QTypes;

enum {OSTrap,ToolTrap};
typedef unsigned char TrapType;


struct SysParmType {
    char valid;
    char aTalkA;
    char aTalkB;
    char config;
    short portA;
    short portB;
    long alarm;
    short font;
    short kbdPrint;
    short volClik;
    short misc;
};

typedef struct SysParmType SysParmType;
typedef SysParmType *SysPPtr;

struct QElem {
    struct QElem *qLink;
    short qType;
    short qData[1];
};

typedef struct QElem QElem;
typedef QElem *QElemPtr;

struct QHdr {
    short qFlags;
    QElemPtr qHead;
    QElemPtr qTail;
};

typedef struct QHdr QHdr;
typedef QHdr *QHdrPtr;

struct DateTimeRec {
    short year;
    short month;
    short day;
    short hour;
    short minute;
    short second;
    short dayOfWeek;
};

typedef struct DateTimeRec DateTimeRec;

struct SysEnvRec {
    short environsVersion;
    short machineType;
    short systemVersion;
    short processor;
    Boolean hasFPU;
    Boolean hasColorQD;
    short keyBoardType;
    short atDrvrVersNum;
    short sysVRefNum;
};

typedef struct SysEnvRec SysEnvRec;


#ifdef __cplusplus
extern "C" {
#endif
#define GetSysPPtr() ((SysPPtr) 0x01F8)
pascal void SysBeep(short duration)
    = 0xA9C8; 
pascal long KeyTrans(const void *transData,short keycode,long *state)
    = 0xA9C3; 
#pragma parameter __D0 DTInstall(__A0)
pascal OSErr DTInstall(QElemPtr dtTaskPtr)
    = 0xA082; 
#define GetMMUMode() (* (char*) 0x0CB2)
#pragma parameter SwapMMUMode(__A0)
pascal void SwapMMUMode(char *mode)
    = {0x1010,0xA05D,0x1080}; 
#if SystemSixOrLater
#pragma parameter __D0 SysEnvirons(__D0,__A0)
pascal OSErr SysEnvirons(short versionRequested,SysEnvRec *theWorld)
    = 0xA090; 
#else
pascal OSErr SysEnvirons(short versionRequested,SysEnvRec *theWorld);
#endif
#pragma parameter __D0 ReadDateTime(__A0)
pascal OSErr ReadDateTime(unsigned long *time)
    = 0xA039; 
#pragma parameter GetDateTime(__A0)
pascal void GetDateTime(unsigned long *secs)
    = {0x20B8,0x020C}; 
#pragma parameter __D0 SetDateTime(__D0)
pascal OSErr SetDateTime(unsigned long time)
    = 0xA03A; 
#pragma parameter SetTime(__A0)
pascal void SetTime(const DateTimeRec *d)
    = {0xA9C7,0xA03A}; 
#pragma parameter GetTime(__A0)
pascal void GetTime(DateTimeRec *d)
    = {0x2038,0x020C,0xA9C6}; 
pascal void Date2Secs(const DateTimeRec *d,unsigned long *secs); 
#pragma parameter Secs2Date(__D0,__A0)
pascal void Secs2Date(unsigned long secs,DateTimeRec *d)
    = 0xA9C6; 
#pragma parameter Delay(__A0,__A1)
pascal void Delay(long numTicks,long *finalTicks)
    = {0xA03B,0x2280}; 
#pragma parameter __A0 GetTrapAddress(__D0)
pascal long GetTrapAddress(short trapNum)
    = 0xA146; 
#pragma parameter SetTrapAddress(__A0,__D0)
pascal void SetTrapAddress(long trapAddr,short trapNum)
    = 0xA047; 
pascal long NGetTrapAddress(short trapNum,TrapType tTyp); 
pascal void NSetTrapAddress(long trapAddr,short trapNum,TrapType tTyp); 
#pragma parameter __A0 GetOSTrapAddress(__D0)
pascal long GetOSTrapAddress(short trapNum)
    = 0xA346; 
#pragma parameter SetOSTrapAddress(__A0,__D0)
pascal void SetOSTrapAddress(long trapAddr,short trapNum)
    = 0xA247; 
#pragma parameter __A0 GetToolTrapAddress(__D0)
pascal long GetToolTrapAddress(short trapNum)
    = 0xA746; 
#pragma parameter SetToolTrapAddress(__A0,__D0)
pascal void SetToolTrapAddress(long trapAddr,short trapNum)
    = 0xA647; 
#pragma parameter __A0 GetToolboxTrapAddress(__D0)
pascal long GetToolboxTrapAddress(short trapNum)
    = 0xA746; 
#pragma parameter SetToolboxTrapAddress(__A0,__D0)
pascal void SetToolboxTrapAddress(long trapAddr,short trapNum)
    = 0xA647; 
pascal OSErr WriteParam(void); 
pascal Boolean EqualString(ConstStr255Param str1,ConstStr255Param str2,
    Boolean caseSens,Boolean diacSens); 
Boolean equalstring(char *str1,char *str2,Boolean caseSens,Boolean diacSens); 
pascal void UprString(Str255 theString,Boolean diacSens); 
void uprstring(char *theString,Boolean diacSens); 
#pragma parameter Enqueue(__A0,__A1)
pascal void Enqueue(QElemPtr qElement,QHdrPtr qHeader)
    = 0xA96F; 
#pragma parameter __D0 Dequeue(__A0,__A1)
pascal OSErr Dequeue(QElemPtr qElement,QHdrPtr qHeader)
    = 0xA96E; 
long SetCurrentA5(void)
    = {0x200D,0x2A78,0x0904}; 
#pragma parameter __D0 SetA5(__D0)
long SetA5(long newA5)
    = 0xC18D; 
pascal void Environs(short *rom,short *machine); 
pascal short RelString(ConstStr255Param str1,ConstStr255Param str2,Boolean caseSens,
    Boolean diacSens); 
short relstring(char *str1,char *str2,Boolean caseSens,Boolean diacSens); 
pascal OSErr HandToHand(Handle *theHndl); 
#pragma parameter __D0 PtrToXHand(__A0,__A1,__D0)
pascal OSErr PtrToXHand(const void *srcPtr,Handle dstHndl,long size)
    = 0xA9E2; 
pascal OSErr PtrToHand(const void *srcPtr,Handle *dstHndl,long size); 
#pragma parameter __D0 HandAndHand(__A0,__A1)
pascal OSErr HandAndHand(Handle hand1,Handle hand2)
    = 0xA9E4; 
#pragma parameter __D0 PtrAndHand(__A0,__A1,__D0)
pascal OSErr PtrAndHand(const void *ptr1,Handle hand2,long size)
    = 0xA9EF; 
#pragma parameter __D0 InitUtil
pascal OSErr InitUtil(void)
    = 0xA03F; 
pascal Boolean SwapInstructionCache(Boolean cacheEnable); 
pascal void FlushInstructionCache(void); 
pascal Boolean SwapDataCache(Boolean cacheEnable); 
pascal void FlushDataCache(void); 
#ifdef __cplusplus
}
#endif

#endif
