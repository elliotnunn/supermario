/*
	File:		HList.c

	Contains:	Operations on HLists.  An HList is a (handle to) a list of handle
				elements, or nil entries.

	Written by:	Phil Goldman

	Copyright:	© 1988-1991  by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <0>	 5/12/88	PYG		New Today.

*/ 

#pragma segment kernel_segment

#include <types.h>
#include <errors.h>
#include <memory.h>
#include <toolutils.h>
#include <windows.h>

#include "Glue.h"
#include "Data.h"
#include "Lomem.h"
#include "HList.h"

/* Macro to return HListPtr of element at offset bytes into the given HList */ 
#define HLISTPTR_AT_OFFSET(hListHdl, offset)	((HListPtr)((Ptr)(*(hListHdl)) + (offset)))

/* AddToHList.  Add hListElem to the list of which hListHdl is the handle.  Returns
 * memFullErr if there is not enough room for hListHdl to be grown by sizeof(hListElem).
 */
OSErr
AddToHList(HListHdl hListHdl, HListElem hListElem)
	{
	u_size				hListHdlSize = GetHandleSize(hListHdl);
	register HListPtr	pHListElem, pHListElemMax;
	OSErr				osErr;
	
	assert(hListHdl != nil && *hListHdl != nil);
	
	/* Keep only stripped handles */
	hListElem = (HListElem) StripAddress((Ptr) hListElem);
	
	/* First look for an empty slot in the existing list */
	pHListElem = *hListHdl;
	pHListElemMax = (HListPtr)((Ptr)pHListElem + hListHdlSize);
	while (pHListElem < pHListElemMax)
		{
		if (*pHListElem == nil)
			{
			/* Found an empty, so set it */
			*pHListElem = hListElem;
			return noErr;
			}
			
		pHListElem++;
		}
			
	/* If we got here there is no empty one, so make one */
	SetHandleSize(hListHdl, hListHdlSize + sizeof(HListElem));
	if ((osErr = MEMERROR) == noErr)
		*HLISTPTR_AT_OFFSET(hListHdl, hListHdlSize) = hListElem;

	return osErr;
	}
	
/* InHList.  Determine whether hListElem in the list of which hListHdl is the handle.
 * Returns qErr if not.
 */
OSErr
InHList(HListHdl hListHdl, HListElem hListElem)
	{
	HListPtr		pHListElem;
	u_size			hListHdlSize;
	
	/* can do nothing if there is no HList! */
	if (hListHdl == nil)
		return qErr;
	assert(*hListHdl != nil);

	/* compare stripped target against the each element */
	hListElem = (HListElem) StripAddress((Ptr) hListElem);
	hListHdlSize = GetHandleSize(hListHdl);
	pHListElem = HLISTPTR_AT_OFFSET(hListHdl, hListHdlSize);
	while (hListHdlSize != 0)
		{
		hListHdlSize -= sizeof(HListElem);
		if (*--pHListElem == hListElem)
			return noErr;
		}
		
	/* fall through means no-find */
	return qErr;
	}
	
/* RecoverHListElem.  Determine whether hListElemMP is a master pointer for a
 * handle stored as an HListElem in hListHdl.  Return the handle, or nil if not
 * found.
 */
HListElem
RecoverHListElem(HListHdl hListHdl, Ptr hListElemMP)
	{
	HListPtr		pHListElem;
	u_size			hListHdlSize;
	
	/* can do nothing if there is no HList! */
	if (hListHdl == nil)
		return nil;
	assert(*hListHdl != nil);

	/* compare stripped target against the value pointed to by each element */
	hListElemMP = (Ptr) StripAddress((Ptr) hListElemMP);
	hListHdlSize = GetHandleSize(hListHdl);
	pHListElem = HLISTPTR_AT_OFFSET(hListHdl, hListHdlSize);
	while (hListHdlSize != 0)
		{
		if (hListElemMP == StripAddress((Ptr) *((Handle)*--pHListElem)))
			return *pHListElem;
		hListHdlSize -= sizeof(HListElem);
		}
	
	/* fall through means no-find */
	return nil;
	}
	
/* RemoveFromHList.  Remove hListElem to the list of which hListHdl is the handle.
 * Returns qErr if hListElem not found in the list.  Will only delete one occurrence
 * from the list.
 * NOTE:  Does not move memory if _Munger doesn't.
 */
OSErr
RemoveFromHList(HListHdl hListHdl, HListElem hListElem)
	{
	HListPtr		pHListElem;
	u_size			hListHdlSize;
	
	/* can do nothing if there is no HList! */
	if (hListHdl == nil)
		return qErr;
	assert(*hListHdl != nil);

	/* compare stripped target against the value pointed to by each element */
	hListElem = (HListElem) StripAddress((Ptr) hListElem);
	hListHdlSize = GetHandleSize(hListHdl);
	pHListElem = HLISTPTR_AT_OFFSET(hListHdl, hListHdlSize);
	while (hListHdlSize != 0)
		{
		hListHdlSize -= sizeof(HListElem);
		if (*--pHListElem == hListElem)
			{
			(void)Munger(hListHdl, hListHdlSize, &hListElem, sizeof(hListElem), (Ptr)-1, 0);
			return noErr;
			}
		}
	
	/* fall through means no-find */
	return qErr;
	}
		
/* GetFirstHListElem.  Find the first HListElem in the list to which hListHdl is
 * the handle.
 */
HListElem
GetFirstHListElem(HListHdl hListHdl)
	{
	assert(hListHdl != nil && *hListHdl != nil);
	
	return (GetHandleSize(hListHdl) >= sizeof(HListElem)) ? **hListHdl : nil;
	}

/* ApplyToHListElems.  Traverse the HList indirectly pointed to by hListHdl and
 * apply (*pHListElemFnc)() to each element.
 */
void
ApplyToHListElems(HListHdl hListHdl, HListElemFnc *pHListElemFnc)
	{
	register HListPtr		pHListElem;
	HListElem				hListElem;
	
	assert(hListHdl != nil && *hListHdl != nil);

	HLock(hListHdl);

	/* Call routine on each element */
	pHListElem = HLISTPTR_AT_OFFSET(hListHdl, GetHandleSize(hListHdl));
	while (pHListElem > *hListHdl)
		{
		/* extract next element down */
		hListElem = *--pHListElem;
		
		/* Call routine on this element */
		if (hListElem != nil)
			(*pHListElemFnc)(hListElem);
		}
		
	HUnlock(hListHdl);
	}
	
/* DestroyHList.  Disposes each element in the list and then disposes the list itself.
 * Sets THEZONE to ProcessMgrZone for the duration of the disposes.  This is so that
 * purged handles end up in the right place (we assume that all handles in the HList,
 * and the HList block itself, are in ProcessMgrZone).
 * NOTE: Assumes that DisposHandle does not move memory (HList block).
 * NOTE: Assumes that if disposes can result in moving the ProcessMgrZone, the current
 * zone is not ProcessMgrZone (otherwise, our restore at the end would be inconsistent
 * with the new zone location).  Could use SafeSetZone and SafeRestoreZone to be safe.
 */
void
DestroyHList(HListHdl hListHdl)
	{
	HListElem			hListElem;
	register HListPtr	pHListElem;
	THz					saveZone;

	assert(hListHdl != nil && *hListHdl != nil);

	/* Set up ProcessMgrZone so MPs of purged handles go in its free list */
	saveZone = THEZONE;
	THEZONE = ProcessMgrZone;
	
	/* Call disposal function on each element */
	pHListElem = HLISTPTR_AT_OFFSET(hListHdl, GetHandleSize(hListHdl));
	while (pHListElem > *hListHdl)
		{
		/* extract next element down */
		hListElem = *--pHListElem;
		
		/* dispose of it */
		if (hListElem != nil)
			{
			DisposHandle((Handle) hListElem);
			assert(MEMERROR == noErr);
			}
		}
	
	/* Dispose HList */
	DisposHandle(hListHdl);
	assert(MEMERROR == noErr);
	
	/* Restore the zone and leave */
	THEZONE = saveZone;
	}
