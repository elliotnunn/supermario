/*
	File:		MemoryMgr32Patches.c

	Contains:	Process Mgr heap routines that manipulate data blocks according to the
				32-bit memory manager structures.  MemoryMgr24Patches.c and
				MemoryMgr32Patches.c are virtually identical in source code.  The header
				files each uses, though, determines the how the block header access
				macros are expanded.  This (hopefully) makes it easier to maintain given
				the 24/32 duality imposed on us.

	Written by:	David Harrison

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<12>	 6/22/92	DTY		#1033275 <csd>: Remove last change.  The quad alignment is now
									done in MemoryMgrPatches.c.
		<11>	 6/12/92	pvh		#1032359: Added quad word alignment code in ChipFromFreeBlock
									function so that we are properly aligned with the rest of the
									Memory manager on 68040 Macs.
		<10>	11/22/91	DTY		Fix terminating condition in GetProcessMgrHiMaxBlock32. It
									should stop at ProcessMgrHeap->bkLim instead of SYSZONE->bkLim.
		 <9>	10/28/91	SAM/KSM	Rolled in Regatta change:
	 									(RMP) Fixed MoveHeap bug where slop wasn't taken into account when
		 								calculating zcbFree.
		 <8>	 5/23/91	dba		do a style of coercion that MPW 3.2 C likes
		 <7>	  4/9/91	DFH		VL, #83672 : Added GetProcessMgrHiMaxBlock.
		 <6>	 3/15/91	DFH		sad,WS#DFH-910315a: Fixed GetProcessMgrLoFreeBytes to not
									dereference sawInterestingBlock parameter if handleOfInterest is
									nil.
		 <5>	  3/4/91	DFH		JWM,WS#DFH-910226b: Fixed GetSystemHiFreeBytes loop termination.
									Was counting the zone trailer.
		 <4>	 2/21/91	DFH		dba,#82504, #82681, #83168, #83182, #83207: TempNewHandle now
									makes sure there is sufficient memory in the system heap even
									when ProcessMgr zone doesn't need to grow.
		 <2>	11/21/90	DFH		(with BBM) Fixed GZ parameter adjustments to deal correctly with
									boundary conditions. Also, use ROUTINE_ADDR when setting gzProc,
									to bypass jumptable.
		 <0>	 x/xx/89	DFH		New Today.

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
#include <errors.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Zone.h"
#include "Zone32.h"

#pragma segment zone32_tools

/* ZoneMPBlockPtr32.  Locate the master pointer block for the given zone.
 * Only needed at startup.
 */
Ptr
ZoneMPBlockPtr32(THz pZone)
	{
	return((Ptr)pZone + SIZEOF_ZONE_HEADER + SIZEOF_BLOCK_HEADER);
	}

/* MakeNonRelocatable32.  Given a pointer to a block's data, convert the block type
 * to "nonrelocatable".
 * Assumes block is from THEZONE.
 */
void
MakeNonRelocatable32(Ptr pBlock)
	{
	register char		sizeCorr;

	pBlock -= SIZEOF_BLOCK_HEADER;
	sizeCorr = SIZE_CORR((HeapBlockPtr)pBlock);
	CLEAR_BLOCK_ATTRS(pBlock);
	TAGBYTE((HeapBlockPtr)pBlock) = nRelTagVal;
	SIZE_CORR((HeapBlockPtr)pBlock) = sizeCorr;
	BLOCK_HANDLE((HeapBlockPtr)pBlock) = (Handle)THEZONE;
	}

/* GetProcessMgrLoFreeBytes32.  Find out how many free or purgeable bytes there are below
 * the lowest immovable block in the Process Mgr heap.  Size returned is physical byte
 * count.  As a side benefit, this routine also tells the caller whether the specified
 * handle was encountered in this low range.  Finding the block implies that freeing or
 * shrinking it would donate free space area of the Process Mgr heap into which the
 * system heap can grow.
 */
u_size
GetProcessMgrLoFreeBytes32(Handle handleOfInterest, Boolean *sawInterestingBlock)
	{
	HeapBlockPtr	heapBlockPtr, blockOfInterest;
	u_size			blockSize, loFreeBytes;
	unsigned int	blockTags;
	unsigned int	blockFlags;

	/* Convert Handle to physical block address.  This works fine even if the handle
	 * has been purged (physical address will not be found in the loop), but of course
	 * we are never called with a purged handle, anyway.
	 */
	if (handleOfInterest != nil)
		{
		blockOfInterest = (HeapBlockPtr) (*handleOfInterest - SIZEOF_BLOCK_HEADER);
		*sawInterestingBlock = false;
		}
	else
		blockOfInterest = nil;

	/* Skip dummy block */
	heapBlockPtr = &ProcessMgrZone->heapData;
	(Ptr)heapBlockPtr += BLOCK_SIZE(heapBlockPtr);
	loFreeBytes = 0;

	/* Walk the heap. Use infinite for loop instead of while loop, since we are guaranteed
	 * to find at least one non-relocatable block (e.g. a master pointer block).
	 */
	for (;;)
		{
		blockTags = (unsigned int) TAGBYTE(heapBlockPtr);
		blockSize = BLOCK_SIZE(heapBlockPtr);

		/* Are we looking at the interesting block? */
		if (heapBlockPtr == blockOfInterest)
			*sawInterestingBlock = true;

		/* Count this block in if it is free or purgeable.  Else, stop walking. */
		if (blockTags == freeTagVal)
			loFreeBytes += blockSize;
		else if (blockTags == relTagVal)
			{
			blockFlags = (unsigned int) MP_TAGBYTE(heapBlockPtr);
			if ((blockFlags & lockedMPTagFlag) != 0)
				break;
			else if ((blockFlags & purgeableMPTagFlag) != 0)
				loFreeBytes += blockSize;
			}
		else
			break;

		(Ptr)heapBlockPtr += blockSize;
		}

	return(loFreeBytes);
	}

/* GetSystemHiFreeBytes32.  Find out how many free or purgeable bytes there are above
 * the highest immovable block in the system heap.  Size returned is physical byte count.
 */
u_size
GetSystemHiFreeBytes32(void)
	{
	HeapBlockPtr	heapBlockPtr;
	u_size			blockSize, hiFreeBytes;
	unsigned int	blockTags;
	unsigned int	blockFlags;

	hiFreeBytes = 0;

	/* Begin at the beginning */
	heapBlockPtr = &SYSZONE->heapData;

	/* Walk the heap */
	while (heapBlockPtr < SYSZONE->bkLim)
		{
		blockTags = (unsigned int) TAGBYTE(heapBlockPtr);
		blockSize = BLOCK_SIZE(heapBlockPtr);

		/* Count this block in if it is free or purgeable.  Reset the counter if the
		 * if the block is non-relocatable, relocatable but locked, or of some unknown
		 * new type, since we require memory above all such blocks.
		 */
		if (blockTags == freeTagVal)
			hiFreeBytes += blockSize;
		else if (blockTags == relTagVal)
			{
			blockFlags = (unsigned int) MP_TAGBYTE(heapBlockPtr);
			if ((blockFlags & lockedMPTagFlag) != 0)
				hiFreeBytes = 0;
			else if ((blockFlags & purgeableMPTagFlag) != 0)
				hiFreeBytes += blockSize;
			}
		else
			hiFreeBytes = 0;

		(Ptr)heapBlockPtr += blockSize;
		}

	return(hiFreeBytes);
	}

/* ExtendZone32.  Extend the given zone upwards cbNeeded bytes by changing the
 * trailer block into a free block and creating a new block right after it.  This
 * parallels procedure ZoneAdjustEnd() in ROM.
 * NOTE:  This must be extended in blocks of at least MIN_PHYSICAL_SIZE, as we need
 * to create a new free block underneath the current trailer block.
 * A5 world needed:  None
 */
void
ExtendZone32(THz theZone, u_size cbNeeded)
	{
	HeapBlockPtr	oldBkLim,newBkLim;

	assert(cbNeeded >= MIN_PHYSICAL_SIZE);

	/* Get current zone trailer block and calc address of the new one */
	(Ptr)oldBkLim = theZone->bkLim;
	(Ptr)newBkLim = (Ptr)oldBkLim + cbNeeded;

	/* Enlarge the zone trailer to the amount of the extension */
	CLEAR_BLOCK_ATTRS(oldBkLim);
	BLOCK_SIZE(oldBkLim) = cbNeeded;
	TAGBYTE(oldBkLim) = freeTagVal;

	/* Init the block header of the new zone trailer */
	CLEAR_BLOCK_ATTRS(newBkLim);
	TAGBYTE(newBkLim) = freeTagVal;
	BLOCK_SIZE(newBkLim) = MIN_PHYSICAL_SIZE;

	/* Adjust zone header to reflect the added free bytes */
	theZone->zcbFree += cbNeeded;
	theZone->bkLim = (Ptr)newBkLim;
	}

/* MoveZone32.  Move the Process Mgr zone from here to there.
 * Assumes A5 == PROCESSMGRGLOBALS
 */
void
MoveZone32(THz oldZone, THz newZone)
	{
	HeapBlockPtr	pBlock, pMaxBlock;
	u_size			cbDelta, oldDummyBlockSize;
	Boolean			zoneWentUp;

	/* Update appropriate locations to reflect new location */
	if (THEZONE == oldZone)
		THEZONE = newZone;
	if (APPLZONE == oldZone)
		APPLZONE = newZone;
	pNullProcess->p_zone = ProcessMgrZone = newZone;

	/* remember size of current dummy ptr (big if shrinking Process Mgr zone) */
	pBlock = &oldZone->heapData;
	oldDummyBlockSize = BLOCK_SIZE(pBlock);

	/* Invalidate the rover and purgeptr */
	oldZone->purgePtr = nil;
	oldZone->allocPtr = nil;

	/* Move the zone header */
	BlockMove(oldZone, newZone, SIZEOF_ZONE_HEADER);

	/* Set up new dummy nonrelocatable block */
	pBlock = &newZone->heapData;
	CLEAR_BLOCK_ATTRS(pBlock);
	BLOCK_SIZE(pBlock) = MIN_PHYSICAL_SIZE;
	BLOCK_HANDLE(pBlock) = (Handle) newZone;
	TAGBYTE(pBlock) = nRelTagVal;

	/* Need to know sign and magnitude of move */
	zoneWentUp = (newZone > oldZone);
	cbDelta = (zoneWentUp) ? (unsigned long)newZone - (unsigned long)oldZone : (unsigned long)oldZone - (unsigned long)newZone;

	/* If the zone was lowered, there is room to make a free block above
	 * the dummy block.  Have to add on extra slop if old size > MIN_PHYSICAL_SIZE.
	 * Also, keep the zone free count in sync with this addition.
	 */
	if (zoneWentUp == false)
		{
		(Ptr)pBlock += MIN_PHYSICAL_SIZE;
		BLOCK_SIZE(pBlock) = cbDelta + (oldDummyBlockSize - MIN_PHYSICAL_SIZE);
		CLEAR_BLOCK_ATTRS(pBlock);
		BLOCK_HANDLE(pBlock) = (Handle) nil;
		TAGBYTE(pBlock) = freeTagVal;
		newZone->zcbFree += BLOCK_SIZE(pBlock);							/* <1> was += cbDelta */
		}

	/* Traverse zone to adjust handle field in the block headers */
	(Ptr)pBlock += BLOCK_SIZE(pBlock);
	pMaxBlock = newZone->bkLim;
	while (pBlock < pMaxBlock)
		{
		if (IS_RELOC_BLOCK(pBlock))
			{
			if (zoneWentUp)
				(Ptr)(BLOCK_HANDLE(pBlock)) -= cbDelta;
			else
				(Ptr)(BLOCK_HANDLE(pBlock)) += cbDelta;
			}
		else
			if (IS_NRELOC_BLOCK(pBlock))
				BLOCK_HANDLE(pBlock) = newZone;

		/* go to next block */
		(Ptr)pBlock += BLOCK_SIZE(pBlock);
		}
	}

/* ShrinkProcessMgrZone32.  Try to shrink the Process Mgr heap by cbNeeded bytes.  Works
 * by growing the dummy block up (with the GZ proc disabled!), then moving the zone
 * header up to the end of it.
 * NOTE: Does not adjust the system zone!  When we return to caller, there is a
 * vacuum between the two zones that should caller should fix with ExtendZone.
 *
 * Returns cbActual (where cbActual >= cbNeeded) on success, zero on failure.
 * A5 world needed:  PROCESSMGRGLOBALS.
 */
unsigned long
ShrinkProcessMgrZone32(u_size cbNeeded)
	{
	THz				origZone;
	HeapBlockPtr	dummyHdrPtr;
	Ptr				dummyPtr;
	unsigned long	sizeToGrow;

	/* check param */
	assert((cbNeeded != 0) && (WORD_ALIGNED(cbNeeded)));

	/* Calculate size that dummy block should grow.  Allow for overflow. */
	if ((sizeToGrow = cbNeeded + SIZEOF_SMALLEST_PTR_DATUM) < cbNeeded)
		return(0);

	/* Try to grow the dummy block to desired size, making sure that
	 * the ProcessMgrZone GZ proc is temporarily disabled.
	 */
	origZone = ProcessMgrZone;
	dummyHdrPtr = (HeapBlockPtr)(&origZone->heapData);
	dummyPtr = (Ptr)dummyHdrPtr + SIZEOF_BLOCK_HEADER;
	origZone->gzProc = nil;
	SetPtrSize(dummyPtr, sizeToGrow);
	origZone->gzProc = ROUTINE_ADDR(ProcessMgr_GZProc_Glue);

	/* Not enough memory ... oh well! */
	if (MEMERROR != noErr)
		return(0);

	/* Find out how much we actually were given (could be more if filling a large
	 * free block), then subtract overhead, to determine the true offset of the new
	 * zone header.  Save result in cbNeeded.
	 */
	cbNeeded = BLOCK_SIZE(dummyHdrPtr) - (SIZEOF_BLOCK_HEADER + SIZEOF_SMALLEST_PTR_DATUM);
	assert((cbNeeded != 0) && (WORD_ALIGNED(cbNeeded)));

	/* Calculate new start of zone, and move it there */
	MoveZone(origZone, (Ptr) origZone + cbNeeded);

	/* tell caller how much we got */
	return(cbNeeded);
	}

/* GetLockPtr32.  Get a non-relocatable block as close to (but below) ceilingPtr as
 * possible, and stamp it with the signature 'LOCK'.
 * Lock pointer not needed in 32-bit world because need for it means the app is
 * not 32-bit clean.
 */
Ptr
GetLockPtr32(Ptr)
	{
	return(nil);
	}

/* GetProcessMgrHiMaxBlock32.  Do like MaxBlock() on the Process Mgr zone, but ignore
 * the straddle area part.
 */
u_size
GetProcessMgrHiMaxBlock32(void)
	{
	HeapBlockPtr	heapBlockPtr;
	u_size			blockSize, hiMaxBlock, thisRangeFree;
	unsigned int	blockTags;
	unsigned int	blockFlags;
	Boolean			aboveStraddle;

	aboveStraddle = false;
	thisRangeFree = 0;
	hiMaxBlock = 0;

	/* Skip dummy block */
	heapBlockPtr = &ProcessMgrZone->heapData;
	(Ptr)heapBlockPtr += BLOCK_SIZE(heapBlockPtr);

	/* Walk the heap */
	while (heapBlockPtr < ProcessMgrZone->bkLim)
		{
		blockTags = (unsigned int) TAGBYTE(heapBlockPtr);
		blockSize = BLOCK_SIZE(heapBlockPtr);

		/* Count this block in if it is free or purgeable.  Reset the counter if the
		 * if the block is non-relocatable, relocatable but locked, or of some unknown
		 * new type, since we're looking for memory *between* such blocks.
		 */
		if (blockTags == freeTagVal)
			thisRangeFree += blockSize;
		else if (blockTags == relTagVal)
			{
			blockFlags = (unsigned int) MP_TAGBYTE(heapBlockPtr);
			if ((blockFlags & lockedMPTagFlag) != 0)
				{
				if ((thisRangeFree > hiMaxBlock) && (aboveStraddle))
					hiMaxBlock = thisRangeFree;
				aboveStraddle = true;
				thisRangeFree = 0;
				}
			else if ((blockFlags & purgeableMPTagFlag) != 0)
				thisRangeFree += blockSize;
			}
		else
			{
			if ((thisRangeFree > hiMaxBlock) && (aboveStraddle))
				hiMaxBlock = thisRangeFree;
			aboveStraddle = true;
			thisRangeFree = 0;
			}

		(Ptr)heapBlockPtr += blockSize;
		}

	/* Allow for block header we'd need */
	if (hiMaxBlock != 0)
		PhysicalToLogicalSize(hiMaxBlock);
	return(hiMaxBlock);
	}

/* GetHighestLargeFreeBlock32.  This function returns a pointer to a block that
 * is >= size, or nil if one couldn't be located.  It will not pass any
 * non-relocatable blocks during it's search.  Does not use the lockPtr parameter
 * since ceilings are not supported in 32-bit heaps.
 */
FreeBlockAddr
GetHighestLargeFreeBlock32(u_size size, Ptr lockPtr)
	{
#pragma unused	(lockPtr)
	HeapBlockPtr	blockPtr, highestLargeFreeBlock, trailerBlockPtr;
	u_size			blockSize;

	/* Looking for physical blocks now */
	size += SIZEOF_BLOCK_HEADER;

	/* Start at bottom of heap, and go up to trailerBlock */
	blockPtr = (HeapBlockPtr)&THEZONE->heapData;
	trailerBlockPtr = (HeapBlockPtr)THEZONE->bkLim;
	highestLargeFreeBlock = nil;
	while (blockPtr < trailerBlockPtr)
		{
		blockSize = BLOCK_SIZE(blockPtr);

		/* Check whether this is a valid block for our purposes */
		if ((IS_FREE_BLOCK(blockPtr) && (blockSize >= size)))
			highestLargeFreeBlock = blockPtr;

		/* Advance to next block */
		(Ptr)blockPtr += blockSize;
		}

	return((FreeBlockAddr) highestLargeFreeBlock);
	}

/* GetTopBlockSize32.  Extract the size of the specified block.  Return 0 if address
 * is nil or the block is not the absolute highest in the zone.
 */
unsigned long
GetTopBlockSize32(FreeBlockAddr pTopBlock)
	{
	u_size	blockSize;

	if ((pTopBlock == nil)
		|| ((Ptr)pTopBlock + (blockSize = BLOCK_SIZE((HeapBlockPtr) pTopBlock)) != THEZONE->bkLim) )
			return(0);

	return(blockSize);
	}

/* FreeTop32.  Shrink the size of the given free block to the specified value, and
 * create a zone trailer block after the result.  Returns address of trailer block.
 */
FreeBlockAddr
FreeTop32(FreeBlockAddr pBlockHdr, u_size newSize)
	{
	/* Resize the free block: only the size field needs to change */
	BLOCK_SIZE((HeapBlockPtr)pBlockHdr) = newSize;

	/* Advance pBlockHdr to header of block to follow, and init the header */
	*(Ptr*)&pBlockHdr += newSize;
	CLEAR_BLOCK_ATTRS(pBlockHdr);
	TAGBYTE((HeapBlockPtr)pBlockHdr) = freeTagVal;
	BLOCK_SIZE((HeapBlockPtr)pBlockHdr) = MIN_PHYSICAL_SIZE;
	return(pBlockHdr);
	}


/* ChipOffRelocFromFree32.  Allocate a relocatable block at the high end of the given
 * free block.
 * Assumes that the free block length is at least sizeNeeded.
 * NOTE: The size correction will be incorrect if the free block is bigger
 * than sizeNeeded, but not big enough to be cleaved.  The resulting logical
 * size will be slightly larger than requested.
 */
void
ChipOffRelocFromFree32(FreeBlockAddr pBlock, Handle theHandle, u_size sizeNeeded, short sizeCorrection)
	{
	HeapBlockPtr	hbPtr = (HeapBlockPtr) pBlock;
	u_size			unusedBytes;

	sizeNeeded += SIZEOF_BLOCK_HEADER;

	if ((unusedBytes = BLOCK_SIZE(hbPtr) - sizeNeeded) >= MIN_PHYSICAL_SIZE)
		{
		SIZE_CORR(hbPtr) = 0;
		BLOCK_SIZE(hbPtr) = unusedBytes;
		(Ptr)hbPtr += unusedBytes;
		BLOCK_SIZE(hbPtr) = sizeNeeded;
		}
	else
		{
		sizeNeeded = BLOCK_SIZE(hbPtr);
		sizeCorrection += unusedBytes;
		assert(sizeCorrection <= 15);
		}

	/* Convert block to relocatable, and set master pointer and size correction */
	CLEAR_BLOCK_ATTRS(hbPtr);
	TAGBYTE(hbPtr) = relTagVal;
	BLOCK_HANDLE(hbPtr) = (Ptr)(theHandle) - (long)THEZONE;
	SIZE_CORR(hbPtr) = sizeCorrection;
	*theHandle = BLOCK_DATA(hbPtr);

	/* Adjust heap for the loss */
	HEAP_FREE_SPACE(THEZONE) -= sizeNeeded;
	}

/* InitVectorTable32.  Stuff the function vectors with our values. */
void
InitVectorTable32(void)
	{
	vrGetProcessMgrLoFreeBytes = (Ptr) ROUTINE_ADDR(GetProcessMgrLoFreeBytes32);
	vrExtendZone = (Ptr) ROUTINE_ADDR(ExtendZone32);
	vrMoveZone = (Ptr) ROUTINE_ADDR(MoveZone32);
	vrShrinkProcessMgrZone = (Ptr) ROUTINE_ADDR(ShrinkProcessMgrZone32);
	vrGetLockPtr = (Ptr) ROUTINE_ADDR(GetLockPtr32);
	vrGetHighestLargeFreeBlock = (Ptr) ROUTINE_ADDR(GetHighestLargeFreeBlock32);
	vrGetTopBlockSize = (Ptr) ROUTINE_ADDR(GetTopBlockSize32);
	vrFreeTop = (Ptr) ROUTINE_ADDR(FreeTop32);
	vrChipOffRelocFromFree = (Ptr) ROUTINE_ADDR(ChipOffRelocFromFree32);
	vrZoneMPBlockPtr = (Ptr) ROUTINE_ADDR(ZoneMPBlockPtr32);
	vrGetSystemHiFreeBytes = (Ptr) ROUTINE_ADDR(GetSystemHiFreeBytes32);
	vrGetProcessMgrHiMaxBlock = (Ptr) ROUTINE_ADDR(GetProcessMgrHiMaxBlock32);
	}
