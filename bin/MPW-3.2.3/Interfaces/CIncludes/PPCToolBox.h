/************************************************************

Created: Sunday, January 6, 1991 at 9:38 PM
    PPCToolBox.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1989-1990
        All rights reserved

************************************************************/


#ifndef __PPCTOOLBOX__
#define __PPCTOOLBOX__

#ifndef __APPLETALK__
#include <AppleTalk.h>
#endif

#ifndef __MEMORY__
#include <Memory.h>
#endif

#ifndef __TYPES__
#include <Types.h>
#endif


typedef unsigned char PPCServiceType;

enum {


/*  service Type  */
    ppcServiceRealTime = 1
};

typedef short PPCLocationKind;

enum {


/* lookup Type */
    ppcNoLocation = 0,                          /* There is no PPCLocName */
    ppcNBPLocation = 1,                         /* Use AppleTalk NBP      */
    ppcNBPTypeLocation = 2                      /* Used for specifying a location name type during PPCOpen only */
};

typedef short PPCPortKinds;

enum {


/* port Types */

    ppcByCreatorAndType = 1,                    /* PortType is specified as colloquial Mac creator and type */
    ppcByString = 2                             /* Port Type is in pascal string format */
};

typedef unsigned char PPCSessionOrigin;         /* Values returned for request field in PPCInform call */

enum {


/* Values returned for requestType field in PPCInform call */
    ppcLocalOrigin = 1,                         /* session originated from this machine */
    ppcRemoteOrigin = 2                         /* session originated from remote machine */
};

typedef short PPCPortRefNum;
typedef long PPCSessRefNum;

struct PPCPortRec {
    ScriptCode nameScript;                      /* script of name */
    Str32 name;                                 /* name of port as seen in browser */
    PPCPortKinds portKindSelector;              /* which variant */
    union {
        Str32 portTypeStr;                      /* pascal type string */
        struct {
            OSType creator;
            OSType type;
            } port;
        } u;
};

typedef struct PPCPortRec PPCPortRec;
typedef PPCPortRec *PPCPortPtr;

struct LocationNameRec {
    PPCLocationKind locationKindSelector;       /* which variant */
    union {
        EntityName nbpEntity;                   /* NBP name entity */
        Str32 nbpType;                          /* just the NBP type string, for PPCOpen */
        } u;
};

typedef struct LocationNameRec LocationNameRec;
typedef LocationNameRec *LocationNamePtr;

struct PortInfoRec {
    unsigned char filler1;
    Boolean authRequired;
    PPCPortRec name;
};

typedef struct PortInfoRec PortInfoRec;
typedef PortInfoRec *PortInfoPtr;


typedef PortInfoRec *PortInfoArrayPtr;
typedef pascal Boolean (*PPCFilterProcPtr)(LocationNamePtr, PortInfoPtr);
/*Procedures you will need to write 
 ex: void MyCompletionRoutine(PPCParamBlkPtr pb) 
 ex: pascal Boolean    MyPortFilter(LocationNamePtr locationName, PortInfoPtr 
 thePortInfo)*/

typedef ProcPtr PPCCompProcPtr;
#define PPCHeader \
     Ptr               qLink;                   /* PPC's Internal Use */\
     unsigned short    csCode;                  /* Requested PPC command */\
     unsigned short    intUse;                  /* Internal Use */\
     Ptr               intUsePtr;               /* Internal Use */\
     PPCCompProcPtr       ioCompletion;         /* Completion Routine    */\
     OSErr               ioResult;              /* Command Result Code */\
     unsigned long       Reserved[5];           /* Reserved for PPC, Don't use */

struct PPCOpenPBRec {
    PPCHeader 
    PPCPortRefNum portRefNum;                   /* Port Reference */
    long filler1;
    PPCServiceType serviceType;                 /*  Bit field describing the requested port service    */
    unsigned char resFlag;                      /* Must be set to 0 */
    PPCPortPtr portName;                        /* PortName for PPC */
    LocationNamePtr locationName;               /* If NBP Registration is required */
    Boolean networkVisible;                     /* make this network visible on network */
    Boolean nbpRegistered;                      /* The given location name was registered on the network */
};

typedef struct PPCOpenPBRec PPCOpenPBRec;
typedef PPCOpenPBRec *PPCOpenPBPtr;

struct PPCInformPBRec {
 PPCHeader
 PPCPortRefNum portRefNum;
    PPCSessRefNum sessRefNum;
    PPCServiceType serviceType;
    Boolean autoAccept;
    PPCPortPtr portName;
    LocationNamePtr locationName;
    StringPtr userName;
    unsigned long userData;
    PPCSessionOrigin requestType;
};

typedef struct PPCInformPBRec PPCInformPBRec;
typedef PPCInformPBRec *PPCInformPBPtr;

struct PPCStartPBRec {
 PPCHeader
    PPCPortRefNum portRefNum;
    PPCSessRefNum sessRefNum;
    PPCServiceType serviceType;
    unsigned char resFlag;
    PPCPortPtr portName;
    LocationNamePtr locationName;
    unsigned long rejectInfo;
    unsigned long userData;
    unsigned long userRefNum;
};

typedef struct PPCStartPBRec PPCStartPBRec;
typedef PPCStartPBRec *PPCStartPBPtr;

struct PPCAcceptPBRec {
 PPCHeader
    short filler1;
    PPCSessRefNum sessRefNum;
};

typedef struct PPCAcceptPBRec PPCAcceptPBRec;
typedef PPCAcceptPBRec *PPCAcceptPBPtr;

struct PPCRejectPBRec {
 PPCHeader
    short filler1;
    PPCSessRefNum sessRefNum;
    short filler2;
    long filler3;
    long filler4;
    unsigned long rejectInfo;
};

typedef struct PPCRejectPBRec PPCRejectPBRec;
typedef PPCRejectPBRec *PPCRejectPBPtr;

struct PPCWritePBRec {
 PPCHeader
    short filler1;
    PPCSessRefNum sessRefNum;
    Size bufferLength;
    Size actualLength;
    Ptr bufferPtr;
    Boolean more;
    unsigned char filler2;
    unsigned long userData;
    OSType blockCreator;
    OSType blockType;
};

typedef struct PPCWritePBRec PPCWritePBRec;
typedef PPCWritePBRec *PPCWritePBPtr;

struct PPCReadPBRec {
 PPCHeader
    short filler1;
    PPCSessRefNum sessRefNum;
    Size bufferLength;
    Size actualLength;
    Ptr bufferPtr;
    Boolean more;
    unsigned char filler2;
    unsigned long userData;
    OSType blockCreator;
    OSType blockType;
};

typedef struct PPCReadPBRec PPCReadPBRec;
typedef PPCReadPBRec *PPCReadPBPtr;

struct PPCEndPBRec {
 PPCHeader
    short filler1;
    PPCSessRefNum sessRefNum;
};

typedef struct PPCEndPBRec PPCEndPBRec;
typedef PPCEndPBRec *PPCEndPBPtr;

struct PPCClosePBRec {
 PPCHeader
    PPCPortRefNum portRefNum;
};

typedef struct PPCClosePBRec PPCClosePBRec;
typedef PPCClosePBRec *PPCClosePBPtr;

struct IPCListPortsPBRec {
 PPCHeader
    short filler1;
    unsigned short startIndex;
    unsigned short requestCount;
    unsigned short actualCount;
    PPCPortPtr portName;
    LocationNamePtr locationName;
    PortInfoArrayPtr bufferPtr;
};

typedef struct IPCListPortsPBRec IPCListPortsPBRec;
typedef IPCListPortsPBRec *IPCListPortsPBPtr;

union PPCParamBlockRec
{
        PPCOpenPBRec        openParam;
        PPCInformPBRec           informParam;
        PPCStartPBRec        startParam;
        PPCAcceptPBRec         acceptParam;
        PPCRejectPBRec         rejectParam;
        PPCWritePBRec        writeParam;
        PPCReadPBRec        readParam;
        PPCEndPBRec              endParam;
        PPCClosePBRec        closeParam;
        IPCListPortsPBRec    listPortsParam;
};
typedef union PPCParamBlockRec PPCParamBlockRec;
typedef  PPCParamBlockRec  *PPCParamBlockPtr;


#ifdef __cplusplus
extern "C" {
#endif
/*  PPC Calling Conventions  */
#pragma parameter __D0 PPCInit
pascal OSErr PPCInit(void)
    = {0x7000,0xA0DD}; 
pascal OSErr PPCOpen(PPCOpenPBPtr pb,Boolean async); 
#pragma parameter __D0 PPCOpenSync(__A0)
pascal OSErr PPCOpenSync(PPCOpenPBPtr pb)
    = {0x7001,0xA0DD}; 
#pragma parameter __D0 PPCOpenAsync(__A0)
pascal OSErr PPCOpenAsync(PPCOpenPBPtr pb)
    = {0x7001,0xA4DD}; 
pascal OSErr PPCInform(PPCInformPBPtr pb,Boolean async); 
#pragma parameter __D0 PPCInformSync(__A0)
pascal OSErr PPCInformSync(PPCInformPBPtr pb)
    = {0x7003,0xA0DD}; 
#pragma parameter __D0 PPCInformAsync(__A0)
pascal OSErr PPCInformAsync(PPCInformPBPtr pb)
    = {0x7003,0xA4DD}; 
pascal OSErr PPCStart(PPCStartPBPtr pb,Boolean async); 
#pragma parameter __D0 PPCStartSync(__A0)
pascal OSErr PPCStartSync(PPCStartPBPtr pb)
    = {0x7002,0xA0DD}; 
#pragma parameter __D0 PPCStartAsync(__A0)
pascal OSErr PPCStartAsync(PPCStartPBPtr pb)
    = {0x7002,0xA4DD}; 
pascal OSErr PPCAccept(PPCAcceptPBPtr pb,Boolean async); 
#pragma parameter __D0 PPCAcceptSync(__A0)
pascal OSErr PPCAcceptSync(PPCAcceptPBPtr pb)
    = {0x7004,0xA0DD}; 
#pragma parameter __D0 PPCAcceptAsync(__A0)
pascal OSErr PPCAcceptAsync(PPCAcceptPBPtr pb)
    = {0x7004,0xA4DD}; 
pascal OSErr PPCReject(PPCRejectPBPtr pb,Boolean async); 
#pragma parameter __D0 PPCRejectSync(__A0)
pascal OSErr PPCRejectSync(PPCRejectPBPtr pb)
    = {0x7005,0xA0DD}; 
#pragma parameter __D0 PPCRejectAsync(__A0)
pascal OSErr PPCRejectAsync(PPCRejectPBPtr pb)
    = {0x7005,0xA4DD}; 
pascal OSErr PPCWrite(PPCWritePBPtr pb,Boolean async); 
#pragma parameter __D0 PPCWriteSync(__A0)
pascal OSErr PPCWriteSync(PPCWritePBPtr pb)
    = {0x7006,0xA0DD}; 
#pragma parameter __D0 PPCWriteAsync(__A0)
pascal OSErr PPCWriteAsync(PPCWritePBPtr pb)
    = {0x7006,0xA4DD}; 
pascal OSErr PPCRead(PPCReadPBPtr pb,Boolean async); 
#pragma parameter __D0 PPCReadSync(__A0)
pascal OSErr PPCReadSync(PPCReadPBPtr pb)
    = {0x7007,0xA0DD}; 
#pragma parameter __D0 PPCReadAsync(__A0)
pascal OSErr PPCReadAsync(PPCReadPBPtr pb)
    = {0x7007,0xA4DD}; 
pascal OSErr PPCEnd(PPCEndPBPtr pb,Boolean async); 
#pragma parameter __D0 PPCEndSync(__A0)
pascal OSErr PPCEndSync(PPCEndPBPtr pb)
    = {0x7008,0xA0DD}; 
#pragma parameter __D0 PPCEndAsync(__A0)
pascal OSErr PPCEndAsync(PPCEndPBPtr pb)
    = {0x7008,0xA4DD}; 
pascal OSErr PPCClose(PPCClosePBPtr pb,Boolean async); 
#pragma parameter __D0 PPCCloseSync(__A0)
pascal OSErr PPCCloseSync(PPCClosePBPtr pb)
    = {0x7009,0xA0DD}; 
#pragma parameter __D0 PPCCloseAsync(__A0)
pascal OSErr PPCCloseAsync(PPCClosePBPtr pb)
    = {0x7009,0xA4DD}; 
pascal OSErr IPCListPorts(IPCListPortsPBPtr pb,Boolean async); 
#pragma parameter __D0 IPCListPortsSync(__A0)
pascal OSErr IPCListPortsSync(IPCListPortsPBPtr pb)
    = {0x700A,0xA0DD}; 
#pragma parameter __D0 IPCListPortsAsync(__A0)
pascal OSErr IPCListPortsAsync(IPCListPortsPBPtr pb)
    = {0x700A,0xA4DD}; 
pascal OSErr DeleteUserIdentity(unsigned long userRef); 
pascal OSErr GetDefaultUser(unsigned long *userRef,
                            Str32 userName); 
pascal OSErr StartSecureSession(PPCStartPBPtr pb,
                                Str32 userName,
                                Boolean useDefault,
                                Boolean allowGuest,
                                Boolean *guestSelected,
                                ConstStr255Param prompt); 
pascal OSErr PPCBrowser(ConstStr255Param prompt,
                        ConstStr255Param applListLabel,
                        Boolean defaultSpecified,
                        LocationNameRec *theLocation,
                        PortInfoRec *thePortInfo,
                        PPCFilterProcPtr portFilter,
                        ConstStr32Param theLocNBPType)
    = {0x303C,0x0D00,0xA82B}; 
#ifdef __cplusplus
}
#endif

#endif
