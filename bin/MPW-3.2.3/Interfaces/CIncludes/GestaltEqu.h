/************************************************************

Created: Sunday, January 6, 1991 at 9:27 PM
    GestaltEqu.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1988-1990
        All rights reserved

************************************************************/


#ifndef __GESTALTEQU__
#define __GESTALTEQU__

#ifndef __TYPES__
#include <Types.h>
#endif


enum {


/************************
 *    Gestalt error codes
 ************************/
    gestaltUnknownErr = -5550,              /* value returned if Gestalt doesn't know the answer */
    gestaltUndefSelectorErr = -5551,        /* undefined selector was passed to Gestalt */
    gestaltDupSelectorErr = -5552,          /* tried to add an entry that already existed */
    gestaltLocationErr = -5553,             /* gestalt function ptr wasn't in sysheap */

/**************************
 *    Environment Selectors
 **************************/

#define gestaltVersion 'vers'               /* gestalt version */
#define gestaltAddressingModeAttr 'addr'    /* addressing mode attributes */

    gestalt32BitAddressing = 0,             /* using 32-bit addressing mode */
    gestalt32BitSysZone = 1,                /* 32-bit compatible system zone */
    gestalt32BitCapable = 2,                /* Machine is 32-bit capable */

#define gestaltAliasMgrAttr 'alis'          /* Alias Mgr Attributes */

    gestaltAliasMgrPresent = 0,             /* True if the Alias Mgr is present */

#define gestaltAppleTalkVersion 'atlk'      /* appletalk version */
#define gestaltAUXVersion 'a/ux'            /*a/ux version, if present */
#define gestaltConnMgrAttr 'conn'           /* connection mgr attributes    */

    gestaltConnMgrPresent = 0,
    gestaltConnMgrCMSearchFix = 1,          /* Fix to CMAddSearch?     */

#define gestaltCRMAttr 'crm '               /* comm resource mgr attributes */

    gestaltCRMPresent = 0,

#define gestaltCTBVersion 'ctbv'            /* CommToolbox version */
#define gestaltDBAccessMgrAttr 'dbac'       /* Database Access Mgr attributes */

    gestaltDBAccessMgrPresent = 0,          /* True if Database Access Mgr present */

#define gestaltDITLExtAttr 'ditl'           /* AppenDITL, etc. calls from CTB */

    gestaltDITLExtPresent = 0,              /* True if calls are present */

#define gestaltEasyAccessAttr 'easy'        /* Easy Access attributes */

    gestaltEasyAccessOff = 0,               /* if Easy Access present, but off (no icon) */
    gestaltEasyAccessOn = 1,                /* if Easy Access "On" */
    gestaltEasyAccessSticky = 2,            /* if Easy Access "Sticky" */
    gestaltEasyAccessLocked = 3,            /* if Easy Access "Locked" */

#define gestaltEditionMgrAttr 'edtn'        /* Edition Mgr attributes */

    gestaltEditionMgrPresent = 0,           /* True if Edition Mgr present */

#define gestaltAppleEventsAttr 'evnt'       /* Apple Events attributes */

    gestaltAppleEventsPresent = 0,          /* True if Apple Events present */

#define gestaltFindFolderAttr 'fold'        /* Folder Mgr attributes */

    gestaltFindFolderPresent = 0            /* True if Folder Mgr present */
};
enum {

#define gestaltFontMgrAttr 'font'           /* Font Mgr attributes */

    gestaltOutlineFonts = 0,                /* True if Outline Fonts supported */

#define gestaltFPUType 'fpu '               /* fpu type */

    gestaltNoFPU = 0,                       /* no FPU */
    gestalt68881 = 1,                       /* 68881 FPU */
    gestalt68882 = 2,                       /* 68882 FPU */
    gestalt68040FPU = 3,                    /* 68040 built-in FPU */

#define gestaltFSAttr 'fs  '                /* file system attributes */

    gestaltFullExtFSDispatching = 0,        /* has really cool new HFSDispatch dispatcher */
    gestaltHasFSSpecCalls = 1,              /* has FSSpec calls    */
    gestaltHasFileSystemManager = 2,        /* has a file system manager */

#define gestaltFXfrMgrAttr 'fxfr'           /* file transfer manager attributes */

    gestaltFXfrMgrPresent = 0,

#define gestaltHardwareAttr 'hdwr'          /* hardware attributes */

    gestaltHasVIA1 = 0,                     /* VIA1 exists */
    gestaltHasVIA2 = 1,                     /* VIA2 exists */
    gestaltHasASC = 3,                      /* Apple Sound Chip exists */
    gestaltHasSCC = 4,                      /* SCC exists */
    gestaltHasSCSI = 7,                     /* SCSI exists */
    gestaltHasSoftPowerOff = 19,            /* Capable of software power off */

#define gestaltHelpMgrAttr 'help'           /* Help Mgr Attributes */

    gestaltHelpMgrPresent = 0,              /* true if help mgr is present */

#define gestaltKeyboardType 'kbd '          /* keyboard type */

    gestaltMacKbd = 1,
    gestaltMacAndPad = 2,
    gestaltMacPlusKbd = 3,
    gestaltExtADBKbd = 4
};
enum {
    gestaltStdADBKbd = 5,
    gestaltPrtblADBKbd = 6,
    gestaltPrtblISOKbd = 7,
    gestaltStdISOADBKbd = 8,
    gestaltExtISOADBKbd = 9,
    gestaltADBKbdII = 10,
    gestaltADBISOKbdII = 11,

#define gestaltLowMemorySize 'lmem'         /* size of low memory area */
#define gestaltLogicalRAMSize 'lram'        /* logical ram size */
#define gestaltMiscAttr 'misc'              /* miscellaneous attributes */

    gestaltScrollingThrottle = 0,           /* true if scrolling throttle on */
    gestaltSquareMenuBar = 2,               /* true if menu bar is square */

#define gestaltMMUType 'mmu '               /* mmu type */

    gestaltNoMMU = 0,                       /* no MMU */
    gestaltAMU = 1,                         /* address management unit */
    gestalt68851 = 2,                       /* 68851 PMMU */
    gestalt68030MMU = 3,                    /* 68030 built-in MMU */
    gestalt68040MMU = 4,                    /* 68040 built-in MMU */

#define gestaltStdNBPAttr 'nlup'            /* standard nbp attributes */

    gestaltStdNBPPresent = 0,

#define gestaltNotificationMgrAttr 'nmgr'   /* notification manager attributes */

    gestaltNotificationPresent = 0,         /* notification manager exists */

#define gestaltNuBusConnectors 'sltc'       /* bitmap of NuBus connectors*/
#define gestaltOSAttr 'os  '                /* o/s attributes */

    gestaltSysZoneGrowable = 0,             /* system heap is growable */
    gestaltLaunchCanReturn = 1,             /* can return from launch */
    gestaltLaunchFullFileSpec = 2,          /* can launch from full file spec */
    gestaltLaunchControl = 3                /* launch control support available */
};
enum {
    gestaltTempMemSupport = 4,              /* temp memory support */
    gestaltRealTempMemory = 5,              /* temp memory handles are real */
    gestaltTempMemTracked = 6,              /* temporary memory handles are tracked */
    gestaltIPCSupport = 7,                  /* IPC support is present */
    gestaltSysDebuggerSupport = 8,          /* system debugger support is present */

#define gestaltOSTable 'ostt'               /*  OS trap table base  */
#define gestaltToolboxTable 'tbtt'          /*  OS trap table base  */
#define gestaltExtToolboxTable 'xttt'       /* Extended Toolbox trap table base */
#define gestaltLogicalPageSize 'pgsz'       /* logical page size */
#define gestaltPowerMgrAttr 'powr'          /* power manager attributes */

    gestaltPMgrExists = 0,
    gestaltPMgrCPUIdle = 1,
    gestaltPMgrSCC = 2,
    gestaltPMgrSound = 3,

#define gestaltPPCToolboxAttr 'ppc '        /* PPC toolbox attributes */

/*
 * PPC will return the combination of following bit fields.
 * e.g. gestaltPPCSupportsRealTime +gestaltPPCSupportsIncoming + gestaltPPCSupportsOutGoing
 * indicates PPC is cuurently is only supports real time delivery
 * and both incoming and outgoing network sessions are allowed.
 * By default local real time delivery is supported as long as PPCInit has been called.*/


    gestaltPPCToolboxPresent = 0x0000,      /* PPC Toolbox is present  Requires PPCInit to be called */
    gestaltPPCSupportsRealTime = 0x1000,    /* PPC Supports real-time delivery */
    gestaltPPCSupportsIncoming = 0x0001,    /* PPC will deny incoming network requests */
    gestaltPPCSupportsOutGoing = 0x0002,    /* PPC will deny outgoing network requests */

#define gestaltProcessorType 'proc'         /* processor type */

    gestalt68000 = 1,
    gestalt68010 = 2,
    gestalt68020 = 3,
    gestalt68030 = 4,
    gestalt68040 = 5,

#define gestaltParityAttr 'prty'            /* parity attributes */

    gestaltHasParityCapability = 0,         /* has ability to check parity */
    gestaltParityEnabled = 1                /* parity checking enabled */
};
enum {

#define gestaltQuickdrawVersion 'qd  '      /* quickdraw version */

    gestaltOriginalQD = 0x000,              /* original 1-bit QD */
    gestalt8BitQD = 0x100,                  /* 8-bit color QD */
    gestalt32BitQD = 0x200,                 /* 32-bit color QD */
    gestalt32BitQD11 = 0x210,               /* 32-bit color QDv1.1 */
    gestalt32BitQD12 = 0x220,               /* 32-bit color QDv1.2 */
    gestalt32BitQD13 = 0x230,               /* 32-bit color QDv1.3 */

#define gestaltQuickdrawFeatures 'qdrw'     /* quickdraw features */

    gestaltHasColor = 0,                    /* color quickdraw present */
    gestaltHasDeepGWorlds = 1,              /* GWorlds can be deeper than 1-bit */
    gestaltHasDirectPixMaps = 2,            /* PixMaps can be direct (16 or 32 bit) */
    gestaltHasGrayishTextOr = 3,            /* supports text mode grayishTextOr */

#define gestaltPhysicalRAMSize 'ram '       /* physical RAM size */
#define gestaltPopupAttr 'pop!'             /* popup cdef attributes */

    gestaltPopupPresent = 0,

#define gestaltResourceMgrAttr 'rsrc'       /* Resource Mgr attributes */

    gestaltPartialRsrcs = 0,                /* True if partial resources exist */

#define gestaltScriptMgrVersion 'scri'      /* Script Manager version number    <08/05/89 pke> */
#define gestaltScriptCount 'scr#'           /* number of active script systems    <08/05/89 pke> */
#define gestaltSerialAttr 'ser '            /* Serial attributes */

    gestaltHasGPIaToDCDa = 0,               /* GPIa connected to DCDa*/
    gestaltHasGPIaToRTxCa = 1,              /* GPIa connected to RTxCa clock input*/
    gestaltHasGPIbToDCDb = 2,               /* GPIb connected to DCDb */

#define gestaltSoundAttr 'snd '             /* sound attributes */

    gestaltStereoCapability = 0,            /* sound hardware has stereo capability */
    gestaltStereoMixing = 1,                /* stereo mixing on external speaker */
    gestaltSoundIOMgrPresent = 3,           /* The Sound I/O Manager is present */
    gestaltBuiltInSoundInput = 4            /* built-in Sound Input hardware is present */
};
enum {
    gestaltHasSoundInputDevice = 5,         /* Sound Input device available */

#define gestaltStandardFileAttr 'stdf'      /* Standard File attributes */

    gestaltStandardFile58 = 0,              /* True if selectors 5-8 (StandardPutFile-CustomGetFile) are supported */

#define gestaltTextEditVersion 'te  '       /* TextEdit version number            <08/05/89 pke> */

    gestaltTE1 = 1,                         /* TextEdit in MacIIci ROM <8Aug89smb> */
    gestaltTE2 = 2,                         /* TextEdit with 6.0.4 Script Systems on MacIIci (Script bug fixes for MacIIci) <8Aug89smb> */
    gestaltTE3 = 3,                         /* TextEdit with 6.0.4 Script Systems all but MacIIci <8Aug89smb> */
    gestaltTE4 = 4,                         /* TextEdit in System 7.0 */
    gestaltTE5 = 5,                         /* TextWidthHook available in TextEdit */

#define gestaltTermMgrAttr 'term'           /* terminal mgr attributes */

    gestaltTermMgrPresent = 0,

#define gestaltTimeMgrVersion 'tmgr'        /* time mgr version */

    gestaltStandardTimeMgr = 1,             /* standard time mgr is present */
    gestaltRevisedTimeMgr = 2,              /* revised time mgr is present */
    gestaltExtendedTimeMgr = 3,             /* extended time mgr is present */

#define gestaltVMAttr 'vm  '                /* virtual memory attributes */

    gestaltVMPresent = 0,                   /* true if virtual memory is present */

/*************************
 *    Info-only selectors
 ************************/

#define gestaltMachineType 'mach'           /* machine type */

    kMachineNameStrID = -16395,
    gestaltClassic = 1,
    gestaltMacXL = 2,
    gestaltMac512KE = 3,
    gestaltMacPlus = 4,
    gestaltMacSE = 5,
    gestaltMacII = 6,
    gestaltMacIIx = 7
};
enum {
    gestaltMacIIcx = 8,
    gestaltMacSE030 = 9,
    gestaltPortable = 10,
    gestaltMacIIci = 11,
    gestaltMacIIfx = 13,
    gestaltMacClassic = 17,
    gestaltMacIIsi = 18,
    gestaltMacLC = 19

#define gestaltMachineIcon 'micn'           /* machine icon */
#define gestaltROMSize 'rom '               /* rom size */
#define gestaltROMVersion 'romv'            /* rom version */
#define gestaltSystemVersion 'sysv'         /* system version*/
};

#ifdef __cplusplus
extern "C" {
#endif
#if SystemSevenOrLater
#pragma parameter __D0 Gestalt(__D0,__A1)
pascal OSErr Gestalt(OSType selector,long *response)
    = {0xA1AD,0x2288}; 
#else
pascal OSErr Gestalt(OSType selector,long *response);
#endif
pascal OSErr NewGestalt(OSType selector,ProcPtr gestaltFunction); 
pascal OSErr ReplaceGestalt(OSType selector,ProcPtr gestaltFunction,ProcPtr *oldGestaltFunction); 
#ifdef __cplusplus
}
#endif

#endif
