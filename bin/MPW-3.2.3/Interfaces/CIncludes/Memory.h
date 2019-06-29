/************************************************************

Created: Sunday, January 6, 1991 at 9:29 PM
    Memory.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved

************************************************************/


#ifndef __MEMORY__
#define __MEMORY__

#ifndef __TYPES__
#include <Types.h>
#endif


enum {

    maxSize = 0x800000,     /*Max data block size is 8 megabytes*/
    defaultPhysicalEntryCount = 8,

/* values returned from the GetPageState function */
    kPageInMemory = 0,
    kPageOnDisk = 1,
    kNotPaged = 2
};

typedef long Size;          /* size of a block in bytes */
typedef pascal long (*GrowZoneProcPtr)(Size cbNeeded);

struct Zone {
    Ptr bkLim;
    Ptr purgePtr;
    Ptr hFstFree;
    long zcbFree;
    GrowZoneProcPtr gzProc;
    short moreMast;
    short flags;
    short cntRel;
    short maxRel;
    short cntNRel;
    short maxNRel;
    short cntEmpty;
    short cntHandles;
    long minCBFree;
    ProcPtr purgeProc;
    Ptr sparePtr;
    Ptr allocPtr;
    short heapData;
};

typedef struct Zone Zone;
typedef Zone *THz;

struct MemoryBlock {
    void * address;
    unsigned long count;
};

typedef struct MemoryBlock MemoryBlock;

struct LogicalToPhysicalTable {
    MemoryBlock logical;
    MemoryBlock physical[defaultPhysicalEntryCount];
};

typedef struct LogicalToPhysicalTable LogicalToPhysicalTable;


typedef short PageState;
typedef short StatusRegisterContents;

#ifdef __cplusplus
extern "C" {
#endif
#define GetApplLimit() (* (Ptr*) 0x0130)
#pragma parameter __A0 GetZone
pascal THz GetZone(void)
    = 0xA11A; 
#define SystemZone() (* (THz*) 0x02A6)
#define ApplicZone() (* (THz*) 0x02AA)
#define ApplicationZone() (* (THz*) 0x02AA)
#pragma parameter __A0 NewHandle(__D0)
pascal Handle NewHandle(Size byteCount)
    = 0xA122; 
#pragma parameter __A0 NewHandleSys(__D0)
pascal Handle NewHandleSys(Size byteCount)
    = 0xA522; 
#pragma parameter __A0 NewHandleClear(__D0)
pascal Handle NewHandleClear(Size byteCount)
    = 0xA322; 
#pragma parameter __A0 NewHandleSysClear(__D0)
pascal Handle NewHandleSysClear(Size byteCount)
    = 0xA722; 
#pragma parameter __A0 HandleZone(__A0)
pascal THz HandleZone(Handle h)
    = 0xA126; 
#pragma parameter __A0 RecoverHandle(__A0)
pascal Handle RecoverHandle(Ptr p)
    = 0xA128; 
#pragma parameter __A0 RecoverHandleSys(__A0)
pascal Handle RecoverHandleSys(Ptr p)
    = 0xA528; 
#pragma parameter __A0 NewPtr(__D0)
pascal Ptr NewPtr(Size byteCount)
    = 0xA11E; 
#pragma parameter __A0 NewPtrSys(__D0)
pascal Ptr NewPtrSys(Size byteCount)
    = 0xA51E; 
#pragma parameter __A0 NewPtrClear(__D0)
pascal Ptr NewPtrClear(Size byteCount)
    = 0xA31E; 
#pragma parameter __A0 NewPtrSysClear(__D0)
pascal Ptr NewPtrSysClear(Size byteCount)
    = 0xA71E; 
#pragma parameter __A0 PtrZone(__A0)
pascal THz PtrZone(Ptr p)
    = 0xA148; 
#define GZSaveHnd() (* (Handle*) 0x0328)
#define TopMem() (* (Ptr*) 0x0108)
#pragma parameter __D0 MaxBlock
pascal long MaxBlock(void)
    = 0xA061; 
#pragma parameter __D0 MaxBlockSys
pascal long MaxBlockSys(void)
    = 0xA461; 
#pragma parameter __D0 StackSpace
pascal long StackSpace(void)
    = 0xA065; 
#pragma parameter __A0 NewEmptyHandle
pascal Handle NewEmptyHandle(void)
    = 0xA166; 
#pragma parameter __A0 NewEmptyHandleSys
pascal Handle NewEmptyHandleSys(void)
    = 0xA566; 
#pragma parameter HLock(__A0)
pascal void HLock(Handle h)
    = 0xA029; 
#pragma parameter HUnlock(__A0)
pascal void HUnlock(Handle h)
    = 0xA02A; 
#pragma parameter HPurge(__A0)
pascal void HPurge(Handle h)
    = 0xA049; 
#pragma parameter HNoPurge(__A0)
pascal void HNoPurge(Handle h)
    = 0xA04A; 
#pragma parameter HLockHi(__A0)
pascal void HLockHi(Handle h)
    = {0xA064,0xA029}; 
#if SystemSixOrLater
#pragma parameter __D0 StripAddress(__D0)
pascal Ptr StripAddress(void *theAddress)
    = 0xA055; 
#else
pascal Ptr StripAddress(void *theAddress);
#endif
#pragma parameter __D0 Translate24To32(__D0)
pascal Ptr Translate24To32(void *addr24)
    = 0xA091; 
pascal Handle TempNewHandle(Size logicalSize,OSErr *resultCode)
    = {0x3F3C,0x001D,0xA88F}; 
pascal Size TempMaxMem(Size *grow)
    = {0x3F3C,0x0015,0xA88F}; 
pascal long TempFreeMem(void)
    = {0x3F3C,0x0018,0xA88F}; 
/*  Temporary Memory routines renamed, but obsolete, in System 7.0 and later.  */
pascal void TempHLock(Handle h,OSErr *resultCode)
    = {0x3F3C,0x001E,0xA88F}; 
pascal void TempHUnlock(Handle h,OSErr *resultCode)
    = {0x3F3C,0x001F,0xA88F}; 
pascal void TempDisposeHandle(Handle h,OSErr *resultCode)
    = {0x3F3C,0x0020,0xA88F}; 
pascal Ptr TempTopMem(void)
    = {0x3F3C,0x0016,0xA88F}; 
/*  Temporary Memory routines as they were known before System 7.0.  */
pascal Size MFMaxMem(Size *grow)
    = {0x3F3C,0x0015,0xA88F}; 
pascal long MFFreeMem(void)
    = {0x3F3C,0x0018,0xA88F}; 
pascal Handle MFTempNewHandle(Size logicalSize,OSErr *resultCode)
    = {0x3F3C,0x001D,0xA88F}; 
pascal void MFTempHLock(Handle h,OSErr *resultCode)
    = {0x3F3C,0x001E,0xA88F}; 
pascal void MFTempHUnlock(Handle h,OSErr *resultCode)
    = {0x3F3C,0x001F,0xA88F}; 
pascal void MFTempDisposHandle(Handle h,OSErr *resultCode)
    = {0x3F3C,0x0020,0xA88F}; 
pascal Ptr MFTopMem(void)
    = {0x3F3C,0x0016,0xA88F}; 
pascal void InitApplZone(void)
    = 0xA02C; 
pascal void InitZone(GrowZoneProcPtr pgrowZone,short cmoreMasters,void *limitPtr,
    void *startPtr); 
#pragma parameter SetZone(__A0)
pascal void SetZone(THz hz)
    = 0xA01B; 
#pragma parameter __D0 CompactMem(__D0)
pascal Size CompactMem(Size cbNeeded)
    = 0xA04C; 
#pragma parameter __D0 CompactMemSys(__D0)
pascal Size CompactMemSys(Size cbNeeded)
    = 0xA44C; 
#pragma parameter PurgeMem(__D0)
pascal void PurgeMem(Size cbNeeded)
    = 0xA04D; 
#pragma parameter PurgeMemSys(__D0)
pascal void PurgeMemSys(Size cbNeeded)
    = 0xA44D; 
#pragma parameter __D0 FreeMem
pascal long FreeMem(void)
    = 0xA01C; 
#pragma parameter __D0 FreeMemSys
pascal long FreeMemSys(void)
    = 0xA41C; 
#pragma parameter ResrvMem(__D0)
pascal void ResrvMem(Size cbNeeded)
    = 0xA040; 
#pragma parameter ReserveMem(__D0)
pascal void ReserveMem(Size cbNeeded)
    = 0xA040; 
#pragma parameter ReserveMemSys(__D0)
pascal void ReserveMemSys(Size cbNeeded)
    = 0xA440; 
#pragma parameter __D0 MaxMem(__A1)
pascal Size MaxMem(Size *grow)
    = {0xA11D,0x2288}; 
#pragma parameter __D0 MaxMemSys(__A1)
pascal Size MaxMemSys(Size *grow)
    = {0xA51D,0x2288}; 
#pragma parameter SetGrowZone(__A0)
pascal void SetGrowZone(GrowZoneProcPtr growZone)
    = 0xA04B; 
#pragma parameter SetApplLimit(__A0)
pascal void SetApplLimit(void *zoneLimit)
    = 0xA02D; 
#pragma parameter MoveHHi(__A0)
pascal void MoveHHi(Handle h)
    = 0xA064; 
#pragma parameter DisposPtr(__A0)
pascal void DisposPtr(Ptr p)
    = 0xA01F; 
#pragma parameter DisposePtr(__A0)
pascal void DisposePtr(Ptr p)
    = 0xA01F; 
pascal Size GetPtrSize(Ptr p); 
#pragma parameter SetPtrSize(__A0,__D0)
pascal void SetPtrSize(Ptr p,Size newSize)
    = 0xA020; 
#pragma parameter DisposHandle(__A0)
pascal void DisposHandle(Handle h)
    = 0xA023; 
#pragma parameter DisposeHandle(__A0)
pascal void DisposeHandle(Handle h)
    = 0xA023; 
pascal Size GetHandleSize(Handle h); 
#pragma parameter SetHandleSize(__A0,__D0)
pascal void SetHandleSize(Handle h,Size newSize)
    = 0xA024; 
#pragma parameter EmptyHandle(__A0)
pascal void EmptyHandle(Handle h)
    = 0xA02B; 
#pragma parameter ReallocHandle(__A0,__D0)
pascal void ReallocHandle(Handle h,Size byteCount)
    = 0xA027; 
#pragma parameter ReallocateHandle(__A0,__D0)
pascal void ReallocateHandle(Handle h,Size byteCount)
    = 0xA027; 
#pragma parameter HSetRBit(__A0)
pascal void HSetRBit(Handle h)
    = 0xA067; 
#pragma parameter HClrRBit(__A0)
pascal void HClrRBit(Handle h)
    = 0xA068; 
pascal void MoreMasters(void)
    = 0xA036; 
#pragma parameter BlockMove(__A0,__A1,__D0)
pascal void BlockMove(const void *srcPtr,void *destPtr,Size byteCount)
    = 0xA02E; 
#define MemError() (* (OSErr*) 0x0220)
pascal void PurgeSpace(long *total,long *contig); 
#pragma parameter __D0 HGetState(__A0)
pascal char HGetState(Handle h)
    = 0xA069; 
#pragma parameter HSetState(__A0,__D0)
pascal void HSetState(Handle h,char flags)
    = 0xA06A; 
#pragma parameter SetApplBase(__A0)
pascal void SetApplBase(void *startPtr)
    = 0xA057; 
pascal void MaxApplZone(void)
    = 0xA063; 
#pragma parameter __D0 HoldMemory(__A0,__A1)
pascal OSErr HoldMemory(void *address,unsigned long count)
    = {0x7000,0xA05C}; 
#pragma parameter __D0 UnholdMemory(__A0,__A1)
pascal OSErr UnholdMemory(void *address,unsigned long count)
    = {0x7001,0xA05C}; 
#pragma parameter __D0 LockMemory(__A0,__A1)
pascal OSErr LockMemory(void *address,unsigned long count)
    = {0x7002,0xA05C}; 
#pragma parameter __D0 LockMemoryContiguous(__A0,__A1)
pascal OSErr LockMemoryContiguous(void *address,unsigned long count)
    = {0x7004,0xA05C}; 
#pragma parameter __D0 UnlockMemory(__A0,__A1)
pascal OSErr UnlockMemory(void *address,unsigned long count)
    = {0x7003,0xA05C}; 
pascal OSErr GetPhysical(LogicalToPhysicalTable *addresses,unsigned long *physicalEntryCount); 
#pragma parameter __D0 DeferUserFn(__A0,__D0)
pascal OSErr DeferUserFn(ProcPtr userFunction,void *argument)
    = 0xA08F; 
#pragma parameter __D0 DebuggerGetMax
pascal long DebuggerGetMax(void)
    = {0x7000,0xA08D}; 
pascal void DebuggerEnter(void)
    = {0x7001,0xA08D}; 
pascal void DebuggerExit(void)
    = {0x7002,0xA08D}; 
pascal void DebuggerPoll(void)
    = {0x7003,0xA08D}; 
#pragma parameter __D0 GetPageState(__A0)
pascal PageState GetPageState(const void *address)
    = {0x7004,0xA08D}; 
#pragma parameter __D0 PageFaultFatal
pascal Boolean PageFaultFatal(void)
    = {0x7005,0xA08D}; 
#pragma parameter __D0 DebuggerLockMemory(__A0,__A1)
pascal OSErr DebuggerLockMemory(void *address,unsigned long count)
    = {0x7006,0xA08D}; 
#pragma parameter __D0 DebuggerUnlockMemory(__A0,__A1)
pascal OSErr DebuggerUnlockMemory(void *address,unsigned long count)
    = {0x7007,0xA08D}; 
#pragma parameter __D0 EnterSupervisorMode
pascal StatusRegisterContents EnterSupervisorMode(void)
    = {0x7008,0xA08D}; 
#ifdef __cplusplus
}
#endif

#endif
