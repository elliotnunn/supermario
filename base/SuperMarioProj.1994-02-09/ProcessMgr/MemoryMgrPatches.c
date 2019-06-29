/*
	File:		MemoryMgrPatches.c

	Contains:	Process Mgr heap routines that manipulate the heap without direct
				knowledge of whether the heap is 24-bit or 32-bit.  MemoryMgr24Patches.c
				and MemoryMgr32Patches.c are virtually identical in source code.  The
				header files each uses, though, determines the how the block header
				access macros are expanded.  This (hopefully) makes it easier to
				maintain given the 24/32 duality imposed on us.

	Written by:	Phil Goldman

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<13>	10/28/92	DTY		Use new Get/Set macros to access ExpandMem.
		<12>	 6/22/92	DTY		#1033275 <csd>: In ProcessManagerNewHiHandle, quad align the
									blocks if we’re running on an 040 machine.  The alignment takes
									the block header size into account, then subtracts it since
									ChipOffRelocFromFree will add the block header size back on, so
									the fudging is necessary to keep the block aligned properly.
		<11>	 4/15/91	DFH		dba, WS#DFH-910415a : ReclaimSpaceIfSysHeapLow now checks
									GetProcessMgrLoFreeBytes to see if enough is available there
									without having to scan the system heap. This is often true, so
									Process Mgr and Temp Memory allocations will be faster. We
									noticed this with EPPC.
		<10>	  4/9/91	DFH		VL, #83672 : Fixed ProcessMgrMaxBlock to use our own
									GetProcessMgrHiMaxBlock, instead of MaxBlock, since we need to
									avoid the straddle block area. That memory is accounted for in
									StraddleBlockSize() and so a) the could be a block we could use
									above the straddle, even if system heap preservation makes
									StraddleBlockSize return 0, or b) if there is no such block
									above the straddle, we would return MaxBlock() even though we
									need to save it for the system.
		 <9>	  4/7/91	DFH		gbm, WS#JC-PM-005 : Fix StraddleBlockSize to not return a number
									less than the header size, since that memory can not really be
									allocated.  Caused ProcessMgrMaxBlock (and therefore TempMaxMem)
									to return negative number, since they subtract the header size
									to convert to logical size.
		 <8>	  4/4/91	DFH		VL, WS#JC-PM-005 : Fixed ProcessMgrMaxBlock to calculate correct
									size. Was incorrect, sometimes negative (!), when there was no
									room left in the Process Mgr zone.
		 <7>	  4/3/91	DFH		kst, WS#DFH-910403a : Removed shortcut in ProcessMgrHiNewHandle
									about whether to call ReclaimSpaceIfSysHeapLow. It was supposed
									to do it only if the grow zone was NOT called, but the condition
									was reversed. This meant we allowed ProcessMgrHiNewHandle to eat
									too much of the System heap, resulting in needless shortages.
									Checking bytesGrown was a bad idea anyway, since it ends up zero
									even if the growzone succeeded. Removed the shortcut altogether
									just to avoid problems.
		 <6>	  4/2/91	DFH		ewa, #86189 : Added VM_HoldSystemHeap call in
									LowerProcessMgrZoneBottom. This was being done by VM's patch to
									the ProcessMgrGZProc. Unfortunately, it is impossible for such a
									patch to work, since it needs to a) not eat any volatile
									registers, b) not use any stack (since our glue crawls the
									caller's stack to find the memory manager's A6 register), and c)
									be reentrant.
		 <5>	 3/14/91	DFH		gbm,#82504, #82681, #83168, #83182, #83207: Use new ExpandMem
									variable for sys heap contiguity factor.
		 <4>	 2/21/91	DFH		dba,#82504, #82681, #83168, #83182, #83207: Process Mgr heap
									allocation now makes sure there is sufficient memory in the
									system heap even when ProcessMgr zone doesn't need to grow.
		 <2>	11/21/90	DFH		(with BBM) Fixed GZ proc parameter adjustments.  Fixed
									ProcessMgrSysGZProc to pay attention to the result of chaining
									through to the previous SysZone gzProc.  Also, use ROUTINE_ADDR
									when stting gzProc, to bypass jump table.
		 <0>	 x/xx/86	PYG		New Today.

*/ 
 
#include <types.h>
#include <memory.h>
#include <menus.h>
#include <files.h>
#include <quickdraw.h>
#include <windows.h>
#include <osutils.h>
#include <devices.h>
#include <retrace.h>
#include <resources.h>
#include <segload.h>
#include <errors.h>
#include <GestaltEqu.h>
#include <VMCallsPriv.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Patches.h"
#include "Zone.h"

/* Data internal to this file */
static Ptr		oldSysGZProc;		/* old system heap grow zone procedure */ 

/* Function prototypes internal to this file */
void			InitMPBlock(Ptr, u_size);
void			InitHeapRoutines(void);
long			ProcessMgrHMakeMoreMasters(void);

#pragma segment INIT

/* InitHeapRoutines.  Both the 24-bit and 32-bit heap management segments are pre-loaded.
 * Now that we know the MMU mode, we can set up the routine vectors based on the
 * appropriate segment, and release the other segment.
 * NOTE: We preload both because the system heap is not growable when this routine is
 * called, so we might not have room to load the needed segment.  Otherwise, it is a
 * Catch-22 because these are the very routines that make the sysheap growable again!
 */
void
InitHeapRoutines(void)
	{
	CodeEntry			*pCodeEntry;

	/* Pick the correct segment to set up the vectors, and the one to release */
	if (In32BitMode)
		{
		InitVectorTable32();
		pCodeEntry = InitVectorTable24;
		}
	else
		{
		InitVectorTable24();
		pCodeEntry = InitVectorTable32;
		}

	/* Release the segment that was NOT used to set up the vectors */
	ReleaseResource(GET_SCOD_HDL_FROM_FNC_PTR(pCodeEntry));
	}

/* InitHeaps.  Initialize the system and Process Mgr heaps.  The fundamental change we
 * implement is a flexible boundary between the system zone and the Process Mgr zone.
 * To do this, we ensure that non-relocatable or locked blocks occur low in the
 * system heap, or high in the Process Mgr heap.  We start the ball rolling here by
 * a) setting a handy flag to force the SYSZONE allocations low, b) moving existing
 * relocatable blocks out of the Process Mgr heap, c) relocating the Process Mgr master
 * pointer block high, and d) taking over the grow zone procedures of the two heaps.
 * The original Process Mgr MP block is turned into a minimum size pointer block at the
 * bottom of the heap.  The system grow zone procedure will use a SetPtrSize on this
 * block to get enough space to extend its zone into the bottom of the Process Mgr zone.
 *
 * NOTE: Error checking should be done in here!
 * A5 world needed:  PROCESSMGRGLOBALS.
 */
void
InitHeaps(void)
	{
	Handle			newMasterBlockHdl;
	unsigned short	cMoreMasters;
	u_size			masterBlockSize;
		
	/* Install vectored heap routines */
	InitHeapRoutines();

	/* Turn on the FNoRvrAlloc flag in the system heap so that all blocks
	 * are allocated low from now on.
	 */
	SYSZONE->flags |= FNoRvrAlloc;
	
	/* Create new master pointer block for Process Mgr heap.  First, adjust the MP
	 * growth increment to be at least enough to allocate the entire zone given
	 * an average block sizes of ZONE_BYTES_PER_MP.  Then, allocate a block of
	 * corresponding size, move it high in the heap, and replace the free master
	 * pointer list with the contents of the new block.  This gets the master
	 * pointers up high like everyone else.  Note that at this point there should
	 * be no master pointers in use from the old set.
	 */
	cMoreMasters = (APPLZONE->bkLim - (Ptr)APPLZONE)/ZONE_BYTES_PER_MP;
	if ((unsigned short) (APPLZONE->moreMast) < cMoreMasters)
		APPLZONE->moreMast = cMoreMasters;
	else
		cMoreMasters = APPLZONE->moreMast;
		
	newMasterBlockHdl = NewHandle(masterBlockSize = cMoreMasters * sizeof(Ptr));
	MoveHHi(newMasterBlockHdl);
	InitMPBlock(*newMasterBlockHdl,masterBlockSize);
	
	/* Make the old master block just a token block.  Assumes block shrinks downward.
	 * This block becomes the "dummyBlock" we SetPtrSize on when we want to grow or
	 * shrink ProcessMgrZone.
	 */
	SetPtrSize(ZoneMPBlockPtr(APPLZONE), SIZEOF_SMALLEST_PTR_DATUM);

	/* The size of the system heap is only indirectly dependent on the size of memory.
	 * It is more a function of how many resources can be brought in from the sys res
	 * file.  However, the current amount is definitely not enough.  Therefore, we
	 * should add on only a fraction of the old cMoreMasters.
	 */
	if (SYSZONE->moreMast <= MAX_TOO_FEW_SYS_MPS)
		{
		SYSZONE->moreMast += (cMoreMasters >> 2);
		THEZONE = SYSZONE;
		MoreMasters();
		THEZONE = APPLZONE;
		}
		
	/* Set the grow zone procedures for the system and Process Mgr heaps.  Saves
	 * the previous setting for the system heap so we can chain to it.
	 */
	oldSysGZProc = SYSZONE->gzProc;
	SYSZONE->gzProc = ROUTINE_ADDR(ProcessMgrSysGZProc);
	APPLZONE->gzProc = ROUTINE_ADDR(ProcessMgr_GZProc_Glue);
	
	/* And set the pertinent globals */
	ProcessMgrZone = APPLZONE;
	}
 
#pragma segment zone_tools

/* Initialize the non-relocatable block to be all MPs on the free list of THEZONE.
 * Must use mode check instead of vector for MakeNonRelocatable call, because during
 * startup the zone tool segment can not be loaded until after we're called.
 * Assumes a5 == PROCESSMGRGLOBALS.
 */
void
InitMPBlock(Ptr masterBlock, u_size masterBlockSize)
	{
	Ptr		*pMPCurrent, *pMPPrev;
	
	/* Point zone free list at last long word in block */
	pMPPrev = pMPCurrent = (Ptr *) (masterBlock + masterBlockSize);
	--pMPPrev;
	THEZONE->hFstFree = (Ptr)pMPPrev;

	/* queue all MPs in block */
	while (pMPPrev > (Ptr *)masterBlock)
		*--pMPCurrent = --pMPPrev;
		
	/* zero out last MP (lowest long in block) */
	*pMPPrev = nil;
	
	/* Turn block into a non-relocatable one. */
	if (In32BitMode)
		MakeNonRelocatable32(pMPPrev);
	else
		MakeNonRelocatable24(pMPPrev);
	}
	
#ifdef MM_DEBUG
typedef pascal void (*PurgeProcPtr) (Handle hdl);
Ptr			oldPurgeProc;

pascal void
DebugNotifyPurge(Handle hdl)
	{
	unsigned long	olda5;
	
	olda5 = ProcessMgrA5SimpleSetup();
	
	*((Handle *) 0xF2) = hdl;
	dbmsg("Purging hdl at $F2...");
	
	if (oldPurgeProc != nil)
		(*((PurgeProcPtr)oldPurgeProc))(hdl);
	
	A5SimpleRestore(olda5);
	}
#endif MM_DEBUG

/* ProcessMgrSysGZProc.  Grow zone procedure for system heap.  Call the old proc and if
 * that fails try shrinking the Process Mgr heap and then growing the system heap.
 * Attempts to get a little more than needed in case someone is doing a series of
 * allocations.
 * A5 world needed:  None (Sets up PROCESSMGRGLOBALS where needed).
 */
pascal long
ProcessMgrSysGZProc(unsigned long cbNeeded)
	{
	Ptr				localOldSysGZProc;
	long			retval;
	unsigned long	firstStab, olda5;
	
	/* Make sure cbNeeded is big enough and longword aligned.  If cbNeeded is too large
	 * (0xFFFFFFFD to 0xFFFFFFFF), the result of LONG_ALIGN will be 0.  We quickly
	 * return 0 because we know it can't be got.  Weird, nearly 0xFFFFFFFF, values
	 * usually happen when somebody subtracts a larger value from a smaller value
	 * to calculate the size to request.
	 */
	if (cbNeeded < MINGROWTH)
		cbNeeded = MINGROWTH;
	else if (LONG_ALIGN(cbNeeded) == 0)
		return(0);
		
	/* Get old grow zone proc */
	retval = 0;
	olda5 = ProcessMgrA5SimpleSetup();
	localOldSysGZProc = oldSysGZProc;
	A5SimpleRestore(olda5);
	
	/* Call old routine w/ olda5 */
	if (localOldSysGZProc != nil)
		retval = CALL_FNC_PTR(GZProcPtr, localOldSysGZProc, (cbNeeded));
	
	/* If that didn't get enough, try to shrink the Process Mgr heap. */
	if (retval == 0)
		{
		olda5 = ProcessMgrA5SimpleSetup();
		
		/* Try for a little extra, but fall back to required amount */
		if ( ((firstStab = (cbNeeded + EXTRA_SLOP_FOR_NEXT_TIME)) < cbNeeded) ||
			((retval = ShrinkProcessMgrZone(firstStab)) == 0) )
			retval = ShrinkProcessMgrZone(cbNeeded);
			
		/* If it worked, grow the system heap by the amount we got */
		if (retval != 0)
			ExtendZone(SYSZONE, retval);
			
		A5SimpleRestore(olda5);
		}
	
	return(retval);
	}

/* ShrinkSysHeap.  Try to shrink the system heap by exactly cbNeeded bytes, but
 * ensure we don't leave less than a minimum sized free block at the top.
 * NOTE:  Have to disable the sys gz proc to stop it from trying to shrink the
 * Process Mgr heap, leading to an unwanted recursion.  This should be changed so
 * that the old sys gz proc would still be called.
 *
 * Returns the amount we were able to get (0 on failure).
 * Assumes A5 == PROCESSMGRGLOBALS
 */
unsigned long
ShrinkSysHeap(unsigned long cbNeeded)
	{
	register u_size	topSize;
	register THz	sysZone;
	FreeBlockAddr	pTopBlock;
	THz				theZone;
	Ptr				saveGZProc;
	unsigned long	realCBNeeded;

	/* Don't even try if the amount is so big we overflow when trying to leave space */
	if ((realCBNeeded = cbNeeded + GetExpandMemMinSysHeapFreeSpace()) < cbNeeded)
		return(0);
	
	/* Move to our zone and disable the growzone procedure
	 * NOTE: Assumes that this can not move ProcessMgrZone, or that THEZONE != ProcessMgrZone,
	 * since THEZONE is restored unconditionally, instead of using SafeSetZone and
	 * SafeRestoreZone.
	 */
	theZone = THEZONE;
	THEZONE = sysZone = SYSZONE;
	saveGZProc = sysZone->gzProc;
	sysZone->gzProc = nil;
	
	/* Torture the system heap to get a large enough free block on top */
	(void)CompactMem(compactAll);
	
	/* Did we get enough at the very top? */
	topSize = GetTopBlockSize(pTopBlock = GetHighestLargeFreeBlock(0, nil));
	if (topSize <= realCBNeeded)
		{
		long		purgeResult;
		
		/* Now try purging (then recompact to coelesce space that was freed up). */
		(void)MaxMem(&purgeResult);
		(void)CompactMem(compactAll);
	
		/* Again, did we get enough at the very top? */
		topSize = GetTopBlockSize(pTopBlock = GetHighestLargeFreeBlock(0, nil));
		if (topSize <= realCBNeeded)
			{
			sysZone->gzProc = saveGZProc;
			THEZONE = theZone;
			return(0);
			}
		}
	
	/* If we're here, pTopBlock points to a free block at the very top of the system
	 * zone, and the block size is at least as big as caller asked for.  Reduce that
	 * free block by the surplus, and create a new zone trailer block immediately
	 * following it.  Update zone header appropriately.
	 */
	sysZone->bkLim = (Ptr) FreeTop(pTopBlock, topSize - cbNeeded);
	sysZone->zcbFree -= cbNeeded;
	
	/* Cleanup and return */
	sysZone->gzProc = saveGZProc;
	THEZONE = theZone;
	
	return(cbNeeded);
	}

/* Globals to communicate between ProcessMgr_GZProc_Glue() and LowerProcessMgrZoneBottom(). */
THz				oldProcessMgrZone, newProcessMgrZone;

/* Lower the bottom of the Process Mgr heap by cbLower bytes, where
 * cbLower >= MINGROWTH.
 * Assumes A5 = PROCESSMGRGLOBALS
 * NOTE:  Have to handle case where old ptr had size correction.
 */
void	
LowerProcessMgrZoneBottom(unsigned long cbLower)
	{
	long	VMResult;
	
	/* Set globals do glue can see them */
	oldProcessMgrZone = ProcessMgrZone;
	newProcessMgrZone = (Ptr)oldProcessMgrZone - cbLower;
	
	/* Move the zone */
	MoveZone(oldProcessMgrZone, newProcessMgrZone);
	
	/* Let VM know what happened.  VM needs to "hold down" the entire system heap.  Since
	 * we just shrank that heap, VM can reassess the heap, most likely unholding the area
	 * we removed.
	 */
	 if ((Gestalt(gestaltVMAttr,&VMResult) == noErr) && (VMResult & (1 << gestaltVMPresent)))
	 	(void) VM_HoldSystemHeap();
	}

/* ProcessMgrGZProc.  Grow zone procedure for Process Mgr heap.  Try shrinking the
 * system heap from the top so we can lower the bottom of the ProcessMgrZone.  The actual
 * amount grown might be more than asked for, because we add extra slop to forestall
 * doing this again.
 * NOTE:  This changes the bottom of the heap!!  After this is called, any pointers to
 * the bottom of the heap must be reset to THEZONE.  In fact, the rudest example of
 * this is the gzProc glue must reset the memory manager's A6 on the stack.  The memory
 * manager uses A6 to hold the zone pointer relevant to the current call.
 * A5 world needed:  None (Sets up PROCESSMGRGLOBALS where needed).
 */
pascal unsigned long
ProcessMgrGZProc(unsigned long cbNeeded)
	{
	unsigned long	olda5, firstStab, retval;

	/* Make sure cbNeeded is big enough and longword aligned.  If cbNeeded is too large
	 * (0xFFFFFFFD to 0xFFFFFFFF), the result of LONG_ALIGN will be 0.  We quickly
	 * return 0 because we know it can't be got.  Weird, nearly 0xFFFFFFFF, values
	 * usually happen when somebody subtracts a larger value from a smaller value
	 * to calculate the size to request.
	 */
	if (cbNeeded < MINGROWTH)
		cbNeeded = MINGROWTH;
	else if (LONG_ALIGN(cbNeeded) == 0)
		return(0);
	
	/* Squeeze blood from a stone, return 0 if it can't be done */
	olda5 = ProcessMgrA5SimpleSetup();
	if ( ((firstStab = (cbNeeded + EXTRA_SLOP_FOR_NEXT_TIME)) < cbNeeded) ||
		((retval = ShrinkSysHeap(firstStab)) == 0) )
		retval = ShrinkSysHeap(cbNeeded);
	
	/* If it worked, move our zone header down and free up the difference. */
	if (retval != 0)
		LowerProcessMgrZoneBottom(retval);

	A5SimpleRestore(olda5);	
	return(retval);
	}

/* ReclaimSpaceIfSysHeapLow.  Check whether, after the allocation of the specified handle,
 * the space available to the system heap is below our predefined threshold.  If it is,
 * we try to grow the system heap to get the emMinSysHeapFreeSpace, or at least as much
 * as we can.
 */
void
ReclaimSpaceIfSysHeapLow(Handle *theNewHandle)
	{
	Boolean		handleInStraddle;
	u_size		lowPM, highSys, roomInPM;

	/* Make dirtcheap check first to avoid expensive scan of the more popular heap. */
	if (GetProcessMgrLoFreeBytes(nil, nil) >= GetExpandMemMinSysHeapFreeSpace())
		return;

	/* Just leave if there is sufficient memory or if the handle is not in the bottom
	 * part of the Process Mgr heap where it can be usefully reclaimed.
	 */
	if ( (StraddleBlockSize(&lowPM, &highSys, theNewHandle, &handleInStraddle) != 0) ||
		(handleInStraddle == false) )
		return;
		
	/* We can get more if we dispose the new handle first.  Bound sys heap growth to the
	 * emMinSysHeapFreeSpace, since freeing the block may give us more than we need.
	 */
	roomInPM = lowPM + (u_size) GetHandleSize(theNewHandle);
	LogicalToPhysicalSize(roomInPM);
	if (highSys + roomInPM > GetExpandMemMinSysHeapFreeSpace())
		roomInPM = GetExpandMemMinSysHeapFreeSpace() - highSys;
		
	DisposHandle(theNewHandle);
	if ((roomInPM = ShrinkProcessMgrZone(roomInPM)) != 0)
		ExtendZone(SYSZONE, roomInPM);
	MEMERROR = memFullErr;
	*theNewHandle = nil;
	}

/* ProcessMgrHiNewHandle.  Allocate a new handle high up in the heap.  A handle is allocated as
 * high in the heap as possible, except that it will be kept below any non-relocatable
 * blocks (this does not mean locked handles) not including the first block in the
 * heap.  This last part is done to allow for Excel's ceiling pointer.
 * Assumes A5 = PROCESSMGRGLOBALS
 * NOTE: Funky resursion when we call ProcessMgrHMakeMoreMasters and it calls us.
 */

#define MAX_ALLOC_TRIES 4	/* Limit of purge-growzone-compact cycle */

Handle
ProcessMgrHiNewHandle(u_size size, Ptr lockPtr)
	{
	FreeBlockAddr	freeAddr;
	u_size			largestFreeBlock;
	u_size			blockHeaderSize;
	unsigned long	bytesNeeded, bytesGrown, purgeResult;
	short			sizeCorrection, allocTries;
	Handle			theHandle;
	THz				theZone;

	/* Switch to the appropriate zone */
	SafeSetZone(theZone, ProcessMgrZone);		
	
	/* Check for enough MPs, but don't get one until after the heap has settled. */
	theHandle = nil;
	if ((THEZONE->hFstFree == nil) && ((MEMERROR = ProcessMgrHMakeMoreMasters()) < 0))
		goto Return_theHandle;

	/* Return error if requested size is larger than Memory Mgr allows */
	if (size > MAX_LOGICAL_SIZE)
		{
		MEMERROR = paramErr;
		goto Return_theHandle;
		}
	
	/* Enforce size minimum and alignment, set size correction appropriately. */

	// 68040 memory manager (Terror ROM) has been patched to quadword align
	// all blocks.  we have to do this in our blocks too so that we're
	// consistent and also remove the possibility of physical size of the block
	// being > 16 bytes beyond logical end of block. 

	bytesNeeded = size;
	if (size < MIN_LOGICAL_SIZE)
		size = MIN_LOGICAL_SIZE;
	else {
		if (CPUFLAG == 4) {													// <12>
			blockHeaderSize = In32BitMode ? SIZEOF_32BIT_BLOCK_HEADER : SIZEOF_24BIT_BLOCK_HEADER;	// <12> Use the right block header size depending on the memory mode we’re in.
			size += blockHeaderSize;										// <12> Add in the size of the block header, so that
			QUAD_ALIGN(size);												// <12> when the size of the block header is added in
			size -= blockHeaderSize;										// <12> ChipOffRelocFromFree, the block will stay quad aligned.
		}																	// <12>
		else																// <12>
			LONG_ALIGN(size);
	}

	sizeCorrection = size - bytesNeeded;
	
	/* Compact memory before anything else. */
	(void)CompactMem(compactAll);

	/* Loop a limited amount to allocate block.
	 * NOTE: bytesNeeded has a different meaning inside this loop.  Above, it was used
	 * as a temp to figure sizeCorrection.  Inside here, it is used to figure the amount
	 * to ask the gzProc for.  It's convenient that above use provides a valid initial
	 * value of bytesNeeded for the (bytesGrown >= bytesNeeded) check.
	 */
	bytesGrown = 0;
	for (allocTries = MAX_ALLOC_TRIES+1; allocTries; --allocTries)
		{
		/* Compact memory if GZ got a worthwhile amount. */
		if (bytesGrown >= bytesNeeded)
			{
			bytesGrown = 0;
			(void)CompactMem(compactAll);
			}
		
		/* Is there a big enough block right now? */
		if ((freeAddr = GetHighestLargeFreeBlock(size, lockPtr)) != nil)
			break;

		/* Allocation try failed, so try purging. Get out if it worked.  Otherwise,
		 * ask for remainder from grow zone procedure.
		 * NOTE: Of course, growing the zone won't help much unless that biggest block
		 * can be merged with the added zone area (i.e. there are no intervening locked
		 * or non-relocatable blocks).  Should we be smarter here (like look for the
		 * free block closest to the end of the zone that grows)?
		 * NOTE: Don't trust MaxMem return value if we have additional requirement
		 * that memory be allocated below a ceiling.  Unconditionally ask the GZ proc
		 * for the full amount.
		 */
		if (size <= (largestFreeBlock = MaxMem(&purgeResult)))
			{
			if (lockPtr == nil)
				continue;
			bytesNeeded = size;
			}
		else
			bytesNeeded = size - largestFreeBlock;
		
		/* At this point we were able to get a block that isn't quite big enough.
		 * Call grow zone procedure.  Give up if we get nothing out of it.
		 */
		if ( (THEZONE->gzProc == nil)
			|| ((bytesGrown = CALL_FNC_PTR(GZProcPtr, THEZONE->gzProc, (bytesNeeded))) == 0) )
			break;
		}

	/* Could not find a big enough block */
	if (freeAddr == nil)
		{
		MEMERROR = memFullErr;
		goto Return_theHandle;
		}
		
	/* freeAddr is a FreeBlockAddr of a block that's at least big enough (probably
	 * a lot bigger!).  Take what we need from the high end of it.
	 */
	theHandle = (Handle) THEZONE->hFstFree;
	assert(theHandle != nil);
	THEZONE->hFstFree = *theHandle;
	ChipOffRelocFromFree(freeAddr, theHandle, size, sizeCorrection);

	/* Make sure we're not taking too much. */
	ReclaimSpaceIfSysHeapLow(&theHandle);

Return_theHandle:
	SafeRestoreZone(theZone);
	return(theHandle);
	}
	
/* ProcessMgrHMakeMoreMasters.  Allocate a new master ptr block for THEZONE.
 * NOTE: Uses a fake master pointer (on the stack) in order for ProcessMgrHiNewHandle to allocate
 * the MP block.  Goes out of use when block is converted to non-relocateble.  Might be
 * better to have a HiFreeBlock routine that would be called here and by ProcessMgrHiNewHandle.
 * The free block could then be converted to either a relocatble or non-relocatable.
 * Assumes A5 == PROCESSMGRGLOBALS
 */
long
ProcessMgrHMakeMoreMasters(void)
	{
	u_size			mpBlockSize;
	Handle			mpBlockHdl;
	Ptr				fakeMP = nil;

	assert(THEZONE->hFstFree == nil);
	
	/* Allocate the MP block high and lock it */
	mpBlockSize = THEZONE->moreMast * sizeof(Ptr);
	THEZONE->hFstFree = &fakeMP;
	mpBlockHdl = ProcessMgrHiNewHandle(mpBlockSize, nil);
	if (MEMERROR != noErr)
		return(MEMERROR);
	
	/* Convert block data to free MP list, and make the block unrelocatable
	 * (with the implication that fakeMP will no longer be needed).
	 */
	InitMPBlock(*mpBlockHdl, mpBlockSize);
	
	return(noErr);
	}

#pragma segment Main

/* In order to set the growzone proc for everybody except Write 4.5 and 4.6,
 * which both have a serious Process Mgr bug which causes the gz proc to ALWAYS
 * return non-0.
 * NOTE:  This travesty should be taken out as soon as Write is rev'ed
 */
#define IS_MACWRITE_46_OR_45(pProc) ((pProc)->p_signature == 'MACA' && (pProc)->p_version <= '0460')
		
/* Grow zone procedure for application heaps.  Checks for special case of running
 * on Process Mgr stack and if so tries to grow the heap itself since the memory
 * manager looks at HEAPEND instead of APPLZONE->bkLim against APPLLIMIT as the
 * grow constraint.
 * A5 world needed:  None (Sets up own to get Process Mgr globals).
 */
pascal long 
MyGrowZone(unsigned long cbNeeded)
	{
	THz				applZone;
	ProcPtr			gzProc;
	Ptr				oldHeapEnd;
	long			retval;
	PEntryPtr		pProc;
	unsigned long	olda5;
	extern Boolean	inLauncheesWorld;	

	/* Make sure cbNeeded is big enough and longword aligned.  If cbNeeded is too large
	 * (0xFFFFFFFD to 0xFFFFFFFF), the result of LONG_ALIGN will be 0.  We quickly
	 * return 0 because we know it can't be got.  Weird, nearly 0xFFFFFFFF, values
	 * usually happen when somebody subtracts a larger value from a smaller value
	 * to calculate the size to request.
	 */
	if (cbNeeded < MINGROWTH)
		cbNeeded = MINGROWTH;
	else if (LONG_ALIGN(cbNeeded) == 0)
		return(0);
	
	retval = 0;
	olda5 = ProcessMgrA5SimpleSetup();
	applZone = APPLZONE;
	
	/* Check global indicating we are using the kernelstack.  If it's non-zero,
	 * assume we are the one causing the heap to grow.  Do our cheap extension
	 * bounded by APPLLIMIT.
	 */
	if ((kernelbusy != 0) && ((cbNeeded + (unsigned long) applZone->bkLim) < APPLLIMIT))
		{
		ExtendZone(applZone, cbNeeded);
		A5SimpleRestore(olda5);
		retval = 1;
		}
	else
		{
		/* Don't call gz proc if launching another (or ever for Write <= 4.6) */
		pProc = pCurrentProcess;
		gzProc = (inLauncheesWorld || IS_MACWRITE_46_OR_45(pProc))
				? nil : (*(pProc->p_pcb))->p_gzproc;
		A5SimpleRestore(olda5);
	
		/* Call user routine.  We restore HEAPEND in case we are running off the
		 * ProcessMgr stack (which means HEAPEND has been modified).
		 */
		if (gzProc != nil)
			{
			oldHeapEnd = HEAPEND;
			HEAPEND = applZone->bkLim;
			retval = CALL_FNC_PTR(GZProcPtr, gzProc, (cbNeeded));
			HEAPEND = oldHeapEnd;
			}
		}

	return(retval);
	}

/* SafeSetGrowZone.  The C language part of our patch to SetGrowZone.  Record caller's
 * gzProc so MyGrowZone can chain to it, if necessary.  Also, don't let applications set
 * the growzone for the system heap or the Process Mgr heap, since those cases are too
 * tricky for them to do right.  Returns chain-through address so assembly-langyage part
 * of the trap can call it with proper register setup.  Returns nil if chain-through
 * should not be done.
 * NOTE: Ensures that the "user" gzProc isn't our own!  This happens if an application
 * tries to save/restore the gzProc by a direct fetch (since no GetGrowZone exists!)
 * and a later SetGrowZone with what he found.  Letting this happen would result in
 * an infinite loop as our growzone chains to p_gzproc.
 * NOTE: Assumes that there is no such thing as _SetGrowZone, SYS.
 */
Ptr
SafeSetGrowZone(ProcPtr gzProc)
	{
	register THz	theZone;
	Ptr				retval;
	unsigned long	olda5;
	
	olda5 = ProcessMgrA5SimpleSetup();
	
	/* Record app's APPLZONE gzProc */
	retval = nil;
	if ((theZone = THEZONE) == APPLZONE)
		{
		if ((ProcPtr)(StripAddress(gzProc)) != MyGrowZone)
			(*(pCurrentProcess->p_pcb))->p_gzproc = gzProc;
		}
	
	/* Prevent meddling in the system and Process Mgr heaps */
	else if ((theZone != SYSZONE) && (theZone != ProcessMgrZone))
		retval = (Ptr) patchtraps[SETGROWZONE].oldtrap;

	A5SimpleRestore(olda5);
	return(retval);
	}

/* StraddleBlockSize.  Calculate the number of free/purgeable bytes in the area of
 * memory shared by the System and Process Mgr heaps.  This is a physical count.
 */
u_size
StraddleBlockSize(u_size *lowPMStorage, u_size *highSysStorage, Handle blockOfInterest, Boolean *sawInterestingBlock)
	{
	u_size	blockSize;
	
	assert(lowPMStorage != nil);

	/* Figure out how much we can get from the top of the system heap, and
	 * add in what we can get from the bottom of the Process Mgr heap.
	 */
	blockSize = GetSystemHiFreeBytes();
	if (highSysStorage != nil)
		*highSysStorage = blockSize;
	blockSize += *lowPMStorage = GetProcessMgrLoFreeBytes(blockOfInterest, sawInterestingBlock);

	/* Allow for the fact that we must leave space at the top of the system heap */
	if (blockSize < GetExpandMemMinSysHeapFreeSpace() + MIN_PHYSICAL_SIZE_EITHER_MODE)
		blockSize = 0;
	else
		blockSize -= GetExpandMemMinSysHeapFreeSpace();
	
	return(blockSize);
	}

/* ProcessMgrMaxBlock.  Act like a MaxBlock() on the Process Mgr heap, except that we
 * count purgeable space.
 */
u_size
ProcessMgrMaxBlock(void)
	{
	u_size		straddleBlockSize, hiMaxBlock, junk, headerBytes;

	/* Calculate largest block above the straddle area. */
	hiMaxBlock = GetProcessMgrHiMaxBlock();

	/* Calculate amount that we can get by combining the top of the system heap
	 * with the bottom of the Process Mgr zone.  StraddleBlockSize() result is
	 * physical block size.  ProcessMgrMaxBlock returns the maximum *logical* block size
	 * that could be requested of TempNewHandle.  Since TempNewHandle actually leaves
	 * us with two blocks, one at the top of the system heap, and one block
	 * at the bottom of the Process Mgr heap, we subtract two header sizes from the
	 * physical size.
	 */
	straddleBlockSize = StraddleBlockSize(&junk, nil, nil, nil);
	headerBytes = (In32BitMode) ? 2*SIZEOF_32BIT_BLOCK_HEADER : 2*SIZEOF_24BIT_BLOCK_HEADER;
	if (straddleBlockSize > headerBytes)
		straddleBlockSize -= headerBytes;
	else
		straddleBlockSize = 0;
		
	/* Result is the larger of the two. */
	return((straddleBlockSize > hiMaxBlock) ? straddleBlockSize : hiMaxBlock);
	}
