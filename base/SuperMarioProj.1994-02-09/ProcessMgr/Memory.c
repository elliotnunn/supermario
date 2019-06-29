/*
	File:		Memory.c

	Contains:	Routines manage the Process Mgr's memory resources.

	Written by:	Erich Ringewald and his bestest buddy Philip York Goldman.

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<12>	10/28/92	DTY		Use new Get/Set macros to access ExpandMem.
		<11>	 3/11/92	DTY		7 Up’s patch to GMBlk doesn’t respect the slop space in the
									system heap, so that the death notice session record will always
									be allocated. We do and we need to stop.
		<10>	11/22/91	DTY		Check allocateGMBlkInSysHeap in GMBlk before trying to allocate
									the block in the Process Manager heap. This is one of the fixes
									from 7-Up.
		 <9>	  4/5/91	JWM		DtY,#86193: GMBlk was fragmenting the system heap and did not
									honor emMinSysHeapFreeSpace. Added a ReserveMemSys call to help
									with the fragmentation problem. Added a check for leaving a
									minimum amount of space in the system heap.
		 <8>	 2/25/91	dba		pm: set up A5 in SysZoneFloatSizes
		 <7>	 2/21/91	DFH		dba,#82504, #82681, #83168, #83182, #83207: Temp mem heap calls
									are more accurate, now that we crawl the system heap to
									calculate the high free memory.
		 <6>	  2/9/91	JWM		DFH,#79583:Restart and Shutdown don't work in low memory
									conditions. A new routine was defined -- GMBlk. It is modeled
									after ProcessMgrHiNewHandle but checks the system heap if
									ProcessMgrHiNewHandle returns a null handle.
		 <5>	 1/14/91	DFH		(dba) Make system mode per process.
		 <3>	 11/5/90	DFH		Correct names for TEMPTOPMEM and TEMPDISPOSEHANDLE.
		 <0>	 1/23/87	PYG		New Today.

*/ 

#include <types.h>
#include <errors.h>
#include <memory.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "Zone.h"
#include "HList.h"

#ifdef MORE_SEGMENTS
#pragma segment kernel_segment
#endif MORE_SEGMENTS

/* The following routines implement selected Memory Manager functions
 * for the Process Mgr Zone.
 *
 * NOTE: Originally, the handle manipulation routines really did something
 * different from their "normal" counterparts.  Now, only the allocation
 * routine (new handle) is special, and the normal call set has been
 * made to work with temp blocks. 
 */

/* c_TempMaxMem.  This routine tries to calculate the size of the largest block we
 * could allocate in the Process Mgr zone.  Unfortunately, it is inaccurate
 * because it does not determine how much of the SYSHEAP space can actually be
 * used (it assumes all free/purgeable space is at the top).
 */
pascal u_size
c_TempMaxMem(u_size *pGrowSize)
	{
	u_size			retval;
	unsigned long	olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	retval = ProcessMgrMaxBlock();
	
	/* Spec sez this is always zero if heap is not APPLZONE */
	if (pGrowSize != nil)
		*pGrowSize = 0;
	
	A5SimpleRestore(olda5);
	return(retval);
	}

/* c_TempFreeMem.  Potential free memory for Process Mgr heap also includes memory from
 * the top of the system heap.  Fairly useless, really.
 * NOTE: Assumes that FreeMem can not cause ProcessMgrZone to move, in that it restores
 * THEZONE unconditionally, instead of using SafeSetZone and SafeRestoreZone.
 */
pascal u_size
c_TempFreeMem(void)
	{
	u_size			retval, lowPM;
	THz				theZone;
	unsigned long	olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	theZone = THEZONE;
	THEZONE = ProcessMgrZone;
	
	/* Initially, calculate space as straddle block (top of system heap and bottom of
	 * Process Mgr heap) plus the free memory in the Process Mgr heap.  This is too much,
	 * since the bottom space in the Process Mgr heap is counted twice, so we finish by
	 * subtracting that much off.  StraddleBlockSize returns 0 if the amount available
	 * in the straddle is less than our threshold, but lowPM can still be non-zero.
	 * NOTE: lowPM can be greater than FreeMem(), since the former includes purgeable
	 * space but the latter does not.  This is why we are careful when subtracting.
	 */
	if ((retval = StraddleBlockSize(&lowPM, nil, nil, nil) + FreeMem()) > lowPM)
		retval -= lowPM;
	else
		retval = 0;
		
	THEZONE = theZone;
	A5SimpleRestore(olda5);
	return(retval);
	}

/* c_TempTopMem.  Return the value of MEMTOP we saw at startup. This call is
 * obsolete in Process Mgr 7.0 and later, because Gestalt is better.
 */
pascal Ptr
c_TempTopMem(void)
	{
	unsigned long	olda5;
	Ptr				retval;

	olda5 = ProcessMgrA5SimpleSetup();
	retval = initMemTop;
	A5SimpleRestore(olda5);
	return(retval);
	}

/* c_SysZoneFloatSizes.  Find the sizes of the blocks that form the straddle block
 * across the top of the system heap and bottom of the Temp Memory heap.  This is
 * the area where the bottom of the Temp Memory heap can float when allocations from
 * one heap require abducting memory from the other heap.
 */
pascal OSErr
c_SysZoneFloatSizes(unsigned long *hiSysRoom, unsigned long *lowTempRoom)
	{
	/* Set up A5 so that we can get at Process Mgr. globals */
	unsigned long savedA5 = ProcessMgrA5SimpleSetup();

	/* Find space available at top of system zone */
	if (hiSysRoom != nil)
		*hiSysRoom = GetSystemHiFreeBytes();

	/* Find space available at bottom of Process Mgr (aka Temp Memory) zone */
	if (lowTempRoom != nil)
		*lowTempRoom = GetProcessMgrLoFreeBytes(nil, nil);
	
	/* For now, no error is possible */
	A5SimpleRestore(savedA5);
	return(noErr);
	}

/* GMBlk.  Attempts to allocate a handle high in the ProcessMgrZone.  If there is no
 * memory in the ProcessMgrZone, check the system zone for the space requested.
 * GMBlk pays attention to emMinSysHeapFreeSpace (denotes how much free we should
 * keep in the system zone.)  If there is space in sysZone we call ReserveMemSys
 * (forces next block allocated to be low in the system heap), then NewHandleSys
 * for the memory.  NOTE:  FreeMemSys() - logicalSize is conservative.  The ReserveMemSys
 * will rearrange the heap.
 * GMBlk is called by some puppet string routines, some routines in AppleEventExtensions 
 * and most memory allocation paths in ePPC.  Most of the memory will be locked due to being
 * accessed from interrupt level by the PPCToolbox.
 */
Handle
GMBlk(u_size logicalSize, OSErr *error)
	{
	Handle	aHdl;

	// <11> If allocateGMBlkInSysHeap is set, it means that an application is quitting, and we’re trying to allocate
	//		a session record for the appDiedEvent.  In this case, always create this session record, even if it means
	//		taking the system heap down below the gauranteed minimum free space.  To save some
	//		code, we’re going to do this with a skanky goto.  This saves two tests of allocateGMBlkInSysHeap.
		

	if (allocateGMBlkInSysHeap)													// <11>
		goto allocateBlock;														// <11>
		
	aHdl = ProcessMgrHiNewHandle(logicalSize, nil);
	if ((*error = MEMERROR) != noErr)
		{
		if ((FreeMemSys() - logicalSize) > GetExpandMemMinSysHeapFreeSpace())
			{	
allocateBlock:
			ReserveMemSys(logicalSize);
			if ((*error = MEMERROR) == noErr)
				{
				aHdl = NewHandleSys(logicalSize);
				*error = MEMERROR;
				}
			}
		else
			*error = memFullErr;
		}
	return(aHdl);
	}

/* ProcessMgrNewHandle.  Allocate a handle in the ProcessMgrZone.
 * Assumes A5 == PROCESSMGRGLOBALS
 */
Handle
ProcessMgrNewHandle(u_size logicalSize)
	{
	Handle		retVal;
	THz			theZone;
	
	SafeSetZone(theZone, ProcessMgrZone);		
	retVal = NewHandle(logicalSize);
	SafeRestoreZone(theZone);

	/* Be kind to the system heap and make sure we're not taking too much. */
	ReclaimSpaceIfSysHeapLow(&retVal);

	return(retVal);
	}

/* c_TempNewHandle.  Allocates a relocateable block from our zone for general use
 * by an application.  We record the allocation so all such can be deallocated when
 * the application terminates.  Allocates the block high in case the caller decides
 * to lock it without moving it high.
 */
pascal Handle								
c_TempNewHandle(u_size logicalSize, OSErr *pResultCode)
	{
	Handle			retVal;
	HListHdl		hList;
	unsigned long	olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	assert((pCurrentProcess->p_pcb != nil) && ((*pCurrentProcess->p_pcb)->tempMemHListHdl != nil));

	retVal = ProcessMgrHiNewHandle(logicalSize, nil);
	if ((*pResultCode = MEMERROR) == noErr)
		{
		hList = (pCurrentProcess->p_systemModeLevel == 0) ? (*pCurrentProcess->p_pcb)->tempMemHListHdl : SystemTempMemHList;
		if (AddToHList(hList,(HListElem)retVal) != noErr)
			{
			*pResultCode = MEMERROR;
			DisposHandle(retVal);
			retVal = nil;
			}
		}

	A5SimpleRestore(olda5);
	return(retVal);
	}
	
/* LikelyTempMemHList.  Before we leap right into a semi-expensive scan of a guy's
 * temp memory lists, we can make couple of cheap checks to eliminate values that
 * can not be found.  First, if the value is lower than the beginning of the Process Mgr
 * zone, it is out of bounds (like nil or in the system heap).  Second, we make sure
 * the value is not within the application partition (this works even if its from a
 * zone-within-a-zone in the application).
 * IMPORTANT: Assumes the tElem has already been stripped, if necessary.
 * NOTE: The elimination checks should stay fairly modest, because many processes
 * won't have any temporary memory at all, meaning the list search is cheap anyway.
 * NOTE:  Assumes a5 == PROCESSMGRGLOBALS
 */
Boolean
LikelyTempMemHList(HListElem tElem, HListHdl *hList)
	{
	Boolean			retVal;
	PEntryPtr		theProc;

	retVal = false;
	theProc = pCurrentProcess;
	if ( ((Ptr) tElem > ProcessMgrZone)
		&& (theProc != nil)
		&& (((Ptr)tElem < theProc->p_zone)
			|| ((Ptr)tElem >= ((Ptr)theProc->p_zone + theProc->p_size))) )
		{
		*hList = (*theProc->p_pcb)->tempMemHListHdl;
		retVal = true;
		}
	
	return(retVal);
	}

/* InCurrTempMem.  Takes a potential HListElem from the temporary memory allocations
 * of the current process, and verifies it.  Returns an OSErr if we couldn't spot it. 
 * NOTE:  Assumes a5 == PROCESSMGRGLOBALS
 */
OSErr
InCurrTempMem(HListElem hElem)
	{
	OSErr			retval=qErr;
	HListHdl		hList;
	
	hElem = (HListElem) StripAddress((Ptr) hElem);
	if (LikelyTempMemHList(hElem,&hList))
		{
		if (HListExists(hList))
			retval = InHList(hList,hElem);
		
		if (retval != noErr)
			retval = InHList(SystemTempMemHList, hElem);
		}
		
	return(retval);	
	}

/* RemoveCurrTempMem.  Takes a potential HListElem from the temporary memory allocations
 * of the current process, and de-registers the allocation.  Returns an OSErr if we didn't
 * deregister anything. 
 * NOTE:  Assumes a5 == PROCESSMGRGLOBALS
 */
OSErr
RemoveCurrTempMem(HListElem hElem)
	{
	Boolean			retval=qErr;
	HListHdl		hList;
	
	hElem = (HListElem) StripAddress((Ptr) hElem);
	if (LikelyTempMemHList(hElem, &hList))
		{
		if (HListExists(hList))
			retval = RemoveFromHList(hList, hElem);
		
		if (retval != noErr)
			retval = RemoveFromHList(SystemTempMemHList, hElem);
		}
		
	return retval;	
	}

/* RecoverCurrTempHdl.  Takes a potential master pointer of a temporary memory handle, and
 * returns the handle.  It's similar to RecoverHandle.
 * NOTE:  Assumes a5 == PROCESSMGRGLOBALS
 */
Handle
RecoverCurrTempHdl(Ptr targetPtr)
	{
	Handle			retVal;
	HListHdl		hList;
	
	retVal = nil;
	targetPtr = StripAddress((Ptr) targetPtr);
	if (LikelyTempMemHList((HListElem) targetPtr, &hList))
		{
		if (HListExists(hList))
			retVal = (Handle) RecoverHListElem(hList, targetPtr);
		
		if (retVal == nil)
			retVal = (Handle) RecoverHListElem(SystemTempMemHList, targetPtr);
		}
	
	return(retVal);
	}
	
/* These have to stay in seg Main since they call glue but don't set up a5 */
#ifdef MORE_SEGMENTS
#pragma segment Main
#endif MORE_SEGMENTS

/* TempHLock.  Lock the relocatable block, preventing it from being moved
 * within its heap zone.
 */
pascal void
c_TempHLock(Handle hdl, OSErr *pResultCode)
	{
	HLock(hdl);
	*pResultCode = MEMERROR;
	}


/* TempHUnLock.  UnLock the relocatable block, allowing it to be moved within
 * its heap zone.
 */
pascal void
c_TempHUnLock(Handle hdl, OSErr *pResultCode)
	{
	HUnlock(hdl);
	*pResultCode = MEMERROR;
	}


/* TempDisposeHandle.  Release the memory occupied by the relocatable block whose
 * handle is h.
 */
pascal void
c_TempDisposeHandle(Handle hdl, OSErr *pResultCode)
	{
	DisposHandle(hdl);
	*pResultCode = MEMERROR;
	}
	
#ifdef MM_DEBUG
/* Check for entries in the system resource map that have MPs in an application heap.
 * The check is enabled by holding down the command key (by itself).
 */
Boolean
ToggleDoCheck(Boolean alreadyChecking)
	{
	EventRecord		doCheckEventRec;
	Boolean			doToggle;

#define noEventMask 0
#define CHECK_SKIP_MODIFIERS (cmdKey+shiftKey+optionKey)
#define DONT_SKIP_MODIFIERS (cmdKey+optionKey)
	(void)OSEventAvail(noEventMask, &doCheckEventRec);
	
	if ( (doCheckEventRec.modifiers & CHECK_SKIP_MODIFIERS) == DONT_SKIP_MODIFIERS )
		{
		if (alreadyChecking = !alreadyChecking)
			dbmsg("Turning on sys map and MP checking...");
		else
			dbmsg("Turning off sys map and MP checking...");
		}
	return alreadyChecking;
	}

static Boolean	doCheck = true;
static Boolean	isSafeNow = false;

void
CheckSysMapForBadMPs(void)
	{
	short			*pTypeList;
	TypeEntry		*pCurType;
	RefEntry		*pCurRef;
	short			cTypes, cResources;
	u_long			sysLimit;
	
	isSafeNow = true;
	
	if ((doCheck = ToggleDoCheck(doCheck)) == false)
			return;
		
	pTypeList = *SYSMAPHANDLE + 24;
	pTypeList = *SYSMAPHANDLE + *pTypeList;
	pCurType = pTypeList + 1;
	cTypes = *pTypeList;
	sysLimit = StripAddress((Ptr) SYSZONE->bkLim);
	while (cTypes >= 0)
		{
		pCurRef = (Ptr)pTypeList + pCurType->refListOffset;
		
		cResources = pCurType->cResources;
		while (cResources >= 0)
			{
			if ((StripAddress((Ptr) pCurRef) > sysLimit)
				dbmsg("Bad Resource!");
			pCurRef++;
			--cResources;
			}
		
		pCurType++;
		--cTypes;
		}
	}
	
#define IS_HANDLE_IN_ZONE(theHdl, theZone) (((Ptr)(theHdl) > (Ptr)(theZone)) && ((Ptr)(theHdl) < (theZone)->bkLim))

/* Check that all (free) MPs in this zone fall within this zone */
void	
CheckZoneForBadFreeMPs(THz theZone)
	{
	Ptr					*pMP;
	
	if (!doCheck)
		return;
		
	theZone = StripAddress((Ptr) theZone);
		
	for (pMP = theZone->hFstFree; pMP != nil; pMP = (Ptr *)*pMP)
		{
		pMP = StripAddress((Ptr) pMP);
		if (!(IS_HANDLE_IN_ZONE(pMP, theZone)))
			{
			dbmsg("Bad free MP list in given zone");
			}
		}
	}
	
/* Return the correct zone for the purged handle.
 * A5 world needed:  PROCESSMGRGLOBALS
 */
THz
GetCorrectZone(Handle purgedHdl)
	{
	PEntryPtr	pProc;
	THz			theZone;
	
	if ((Ptr)purgedHdl < SYSZONE || (Ptr)purgedHdl > ProcessMgrZone->bkLim)
		return nil;			/* Out of all bounds */
	
	if ((Ptr)purgedHdl < SYSZONE->bkLim)
		return SYSZONE;
		
	pProc = pProcessList;
	while (pProc != nil)
		{
		assert(IsProcessEntry(pProc));
		theZone = (THz)(StripAddess(pProc->p_zone));
			
		if (IS_HANDLE_IN_ZONE(purgedHdl, theZone))
			return theZone;
		pProc = pProc->p_NextProcess;
		}
	
	/* Couldn't find in any of the app zones so must be in Process Mgr zone */
	return ProcessMgrZone;
	}
	
/* Check if given handle is "bad" */
void
CheckBadHandle(Handle hdl)
	{
	unsigned long		olda5;
	THz					correctZone, GetCorrectZone(Handle);
	THz					pZone = &correctZone;
	
	olda5 = ProcessMgrA5SimpleSetup();
	
	hdl = (Handle)(StripAddress((Ptr) hdl));		/* Strip! */
	
	if (isSafeNow && doCheck)
		{
		if (hdl == nil)
			{
			dbmsg("Operating on nil handle");
			}
		else if (*hdl == nil)
			{
			/* Purged */
			if ((correctZone = GetCorrectZone(hdl)) == nil)
				dbmsg("Couldn't find a zone for purged handle");
			else if (correctZone != THEZONE)
				{
				if (THEZONE == ProcessMgrZone)
					{
					dbmsg("Purged handle op possibly trashing ProcessMgrZone MP list");
					}
				else if (THEZONE == SYSZONE)
					{
					dbmsg("Purged handle op possibly trashing sys zone MP list");
					}
				else if (!(THEZONE == APPLZONE && correctZone == SYSZONE))
					{
					dbmsg("Bad zone for operation on purged handle");
					}
				}
			}
		}
		
	A5SimpleRestore(olda5);
	}
			
#endif MM_DEBUG