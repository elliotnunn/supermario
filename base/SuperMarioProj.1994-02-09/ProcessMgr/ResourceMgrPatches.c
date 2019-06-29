/*
	File:		ResourceMgrPatches.c

	Contains:	Routines which patch the Resource Manager traps.

	Written by:	Erich Ringewald and Phil Goldman

	Copyright:	© 1986-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
	  <SM10>	 7/21/93	joe		Back out <SM9>.
		 <8>	  7/8/92	gbm		#1035120,<csd>: Add a check to the ReleaseResource patch that
									sees if LastFOND is being released. If it is, then set LastFOND
									to nil so that the Font Manager will fill it in again later with
									another 'FOND' handle.
		 <7>	 6/10/92	DTY		Remove structures definitions for TypeEntry and RefEntry, now
									that they’re both defined in ResourceMgrPriv.h.
		 <6>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		 <5>	 11/1/91	DTY		Change ClearSharingBits to check to see if the map handle is not
									a system override map as well as the system resource map.
		 <4>	10/10/91	stb		bbm; add some comments about why print drivers go in the system
									heap
		 <0>	 x/xx/86	PYG		New Today.

*/  
 
#include <types.h>
#include <memory.h>
#include <toolutils.h>
#include <osutils.h>
#include <quickdraw.h>
#include <fonts.h>
#include <events.h>
#include <desk.h>
#include <resources.h>
#include <retrace.h>
#include <menus.h>
#include <windows.h>
#include <files.h>
#include <segload.h>
#include <errors.h>
#include <MFPrivate.h>
#include <ResourceMgrPriv.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Patches.h"
#include "Puppet.h"

/*
 * There are several reasons that the Process Mgr patches the Resource Mgr:
 *
 * 		1) _OpenRF and _Close are patched to implement shared printer driver files.
 *			OpenRF succeeds when it would otherwise get opWrErr if the resource map
 *			has the mapForceSysHeap attribute.  Close skips the close if the map has
 *			that bit set, and the file is still open in someone's chain.
 *
 *		2) CheckLoad is patched to force resources from shared printer driver files
 *			and from the system file to load in the system heap.  The system file 
 *			resources must go in the system heap because they may be used by several
 *			processes, and need to stay intact even if one or more processes go away.
 *			Print driver resources need to go in the sys heap because they may be shared.
 *			It would be bad to have them in an application zone which went away when
 *			the resources were in use by another application.
 *
 *		3) GetResAttrs and UpdateResFile are patched to cover up the implementation of
 *			the CheckLoad patch.  The CheckLoad patch sets the resSysHeap bit if it was
 *			not already set, and sets an extra bit, ResSysRef to indicate that it has
 *			done this.  GetResAttrs and UpdateResFile make sure that the fake ResSysHeap
 *			setting is not found or written to the file.
 *			NOTE: An improvement here would be to have the CheckLoad patch just set &
 *			restore THEZONE = SYSZONE around calling through to the original CheckLoad.
 *			This way you could avoid setting and hiding attribute bits.  It is rumored
 *			that system pieces besides the Resource Mgr use the resSysHeap bit to figure
 *			out whether the resource is in the system heap.  Haven't seen it myself.
 *			RsrcZoneInit does it, but RsrcZoneInit is not called under Process Mgr.
 *
 *		4) GetNamedResource and SizeRsrc are patched to deal with oddities of FullPaint,
 *			MacDraw II, and other applications where they pre-flight OpenDeskAcc by
 *			calling GetNamedResource/SizeRsrc in response to MenuSelect.  This is gross!
 *
 *		5) ReleaseResource is patched to a) not release font and cursor related resources,
 *			and b) to deregister Apple menus registered by c_addresmenu. 
 *
 *		6) GetResource is patched as part of the grand "global/local printer" scheme.  It
 *			opens the correct driver before loading the PDEF from it.  
 *
 */
 
#define mapForceSysHeap	(1)
#define resSysRef		(1 << 7)

#define MAP_OFFSET_HDL_TO_NEXT_MAP	16	
#define MAP_OFFSET_REFNUM			20	
#define MAP_OFFSET_RESFILE_ATTRS	22	
#define MAP_OFFSET_TYPELIST_OFFSET	24
#define GET_REFNUM_FROM_MAP_HDL(mapHdl) (((short *)(*(mapHdl)))[MAP_OFFSET_REFNUM/sizeof(short)])

/* Some function prototypes that should be in (yet another) header file */
Handle				GetMapHdlFromRefNum(short, Handle);
AppleItemHdl		StringToAppleItemHdl(StringPtr);
void				RemoveFromAppleMenuList(Handle hdl);
	
typedef void (*RefEntryFncPtr)(ReferenceEntry *pRefEntry);

/* ApplyToAllRefEntries.  Call (*refEntryFncPtr)() on each entry in resource
 * map handled to by mapHdl
 * A5 world needed:  None (but need to ensure correct address of fnc passed by
 * the caller).
 */
void
ApplyToAllRefEntries(Handle mapHdl,RefEntryFncPtr refEntryFncPtr)
	{
	short			*pTypeList;
	TypeEntry		*pCurType;
	ReferenceEntry	*pCurRef;
	short			cTypes, cResources;

	/* Locate type list by using the offset to it stored in the map */
	pTypeList = *mapHdl + MAP_OFFSET_TYPELIST_OFFSET;
	pTypeList = *mapHdl + *pTypeList;

	/* Locate first type entry and the number of entries - 1 */
	pCurType = pTypeList + 1;
	cTypes = *pTypeList;
	
	/* Major loop -- go through all types in the list */
	while (cTypes >= 0)
		{
		/* locate first resource entry and the number of entries - 1 */
		pCurRef = (Ptr)pTypeList + pCurType->tOffset;
		cResources = pCurType->tCount;
		
		/* minor loop -- apply function to each resource entry of current type */
		while (cResources >= 0)
			{
			(*refEntryFncPtr)(pCurRef);
			pCurRef++;
			--cResources;
			}
		
		/* Advance to next type in major loop */
		pCurType++;
		--cTypes;
		}
	}

/* ClearSharingBitFromEntry.  For the given resource map entry, clear both the 
 * resSysRef and the resSysHeap if the resSysRef indicates that WE moved the
 * resource to the system heap (set by CheckLoad patch).
 */
void	
ClearSharingBitFromEntry(ReferenceEntry *pRefEntry)
	{
#if (resSysRef == (1<<7))
	if ((char)pRefEntry->rAttrData.rAttr < (char)0)
#else
	if ((pRefEntry->rAttrData.rAttr & (unsigned char) resSysRef) != 0)
#endif
		pRefEntry->rAttrData.rAttr &= (unsigned char) (~(resSysRef | resSysHeap));
	}

/* ClearSharingBits.  Before calling old routine, make sure to clear out resSysHeap
 * bit of any guy who also has the resSysRef bit set.  We are using the latter to
 * signify that we marked a resource as belonging in the system heap, and the app
 * did not.
 * Return value:	MAP_CHANGE: Map has changed, but sharing bit is not set
 * 					MAP_CHANGE_SHARED: Map has changed and has sharing bit set
 * 					MAP_NO_CHANGE: Map has not changed
 * A5 world needed:  None.
 */

#define MAP_NO_CHANGE		0
#define MAP_CHANGE			1
#define MAP_CHANGE_SHARED	2

char
ClearSharingBits(Handle mapHdl)
	{
	char			mapAttrs;
	unsigned long	olda5;

	/* just let old routine choke if there's no map */
	if (mapHdl == nil)
		return(MAP_CHANGE);
	
	/* map not changed if resource mgr doesn't think so */
	mapAttrs = (*mapHdl)[MAP_OFFSET_RESFILE_ATTRS];
	if ((mapAttrs & mapChanged) == 0)
		return(MAP_NO_CHANGE);

	/* No need to change bits on a readonly map; it can't get updated
	 * anyway.  Likewise, if it's not the sys map and it doesn't have
	 * the sharing bit set, there won't be any resSysRefs set (so return).
	 */
	if ( ((mapAttrs & mapReadOnly) != 0) ||
		((!IsThisASystemResourceMap(mapHdl)) && ((mapAttrs & mapForceSysHeap) == 0)) )
			return(MAP_CHANGE);
	
	/* Clear bits all across the map */
	olda5 = ProcessMgrA5SimpleSetup();
	ApplyToAllRefEntries(mapHdl, ClearSharingBitFromEntry);
	A5SimpleRestore(olda5);
	return(MAP_CHANGE_SHARED);
	}
	
/* ResetSharedBitForEntry.  For the given resource map entry, set both the
 * resSysRef and the resSysHeap in its resAttrs if the resource is not
 * already in the system heap.
 */
void	
ResetSharedBitForEntry(ReferenceEntry *pRefEntry)
	{
	if ((pRefEntry->rAttrData.rAttr & (unsigned char)resSysHeap) == 0)
		pRefEntry->rAttrData.rAttr |= (unsigned char)(resSysRef | resSysHeap);
	}

/* c_updateresfile.  Patch to ensure that resource is map is not written out with the
 * attribute bits we use.
 */
pascal void
c_updateresfile(short refNum)
	{
	unsigned long 	olda5;
	Handle			mapHdl;
	char			mapStatus;

	/* Clear out ResError, because we might not fall thru to old routine */
	RESERR = 0;
	
	/* Clear out transitory resource attributes bits, checking whether write is needed */
	mapStatus = ClearSharingBits(mapHdl = GetMapHdlFromRefNum(refNum, TOPMAPHANDLE));
	
	/* Call old routine if map needs updating.  If it was a shared map, restore the
	 * sharing bits in each entry.
	 */
	if (mapStatus != MAP_NO_CHANGE)
		{
		olda5 = ProcessMgrA5SimpleSetup();
		CALL_FNC_PTR(Updateresfile_ptr,patchtraps[UPDATERESFILE].oldtrap,(refNum));
		if (mapStatus == MAP_CHANGE_SHARED)
			ApplyToAllRefEntries((mapHdl), ResetSharedBitForEntry);
		A5SimpleRestore(olda5);
		}
	}

/* c_releaseresource.  Deregister Apple menus.  Prevent release of fonts and cursors. */
pascal void	
c_releaseresource(Handle resHdl)
	{
	unsigned long	olda5;
	short			saveCurMap;
	Handle			saveTopMapHandle;
	long			theType = 0, fastTheType;
	
	olda5 = ProcessMgrA5SimpleSetup();

	/* Figure out resource type.  We need to invalidate the cache so we can know
	 * whether the resource truly came from system file.
	 */
	saveCurMap = CURMAP; CURMAP = SYSMAP;
	saveTopMapHandle = TOPMAPHANDLE; TOPMAPHANDLE = GetOverrideMap(SYSMAPHANDLE);
	RMGRCACHE = (Handle)0;
	GetResInfo(resHdl, nil, &theType, nil);
	CURMAP = saveCurMap;
	TOPMAPHANDLE = saveTopMapHandle;

	/* Call through if resource not at or below system file, or it is not font/cursor */
	if (RESERR == resNotFound
		|| ((fastTheType = theType) != 'FONT'
			&& (fastTheType != 'FOND')
			&& (fastTheType != 'NFNT')
			&& (fastTheType != 'CURS')) )
				{
				/* Deregister Apple menu */
				RemoveFromAppleMenuList(resHdl);

				// See if we’re releasing LastFOND.
			   
				if (resHdl == LASTFOND)								// <8>
					LASTFOND = nil;									// <8>
			   
				/* Call through */
				CALL_FNC_PTR(Releaseresource_ptr,patchtraps[RELEASERESOURCE].oldtrap,(resHdl));
				}

	A5SimpleRestore(olda5);
	}

/* c_getresattrs.  If resSysRef is set, then clear both it and resSysHeap */
pascal short
c_getresattrs(Handle resHdl)	
	{
	unsigned long	olda5;
	Ptr				oldtrap;
	unsigned short	resAttrs;
	
	olda5 = ProcessMgrA5SimpleSetup();
	oldtrap = patchtraps[GETRESATTRS].oldtrap;
	A5SimpleRestore(olda5);

	resAttrs = CALL_FNC_PTR(Getresattrs_ptr, oldtrap, (resHdl));
	if ((resAttrs & ((unsigned short) resSysRef)) != 0)
		resAttrs &= (unsigned short) (~(resSysRef | resSysHeap));

	return(resAttrs);
	}

/* GetMapHdlInOtherChainFromRefNum.  Return whether the map of the resource file of
 * given ref num is in the chain of any task besides pCurrentProcess.
 * Assumes A5 == PROCESSMGRGLOBALS
 */
Handle
GetMapHdlInOtherChainFromRefNum(short refNum)
	{
	PEntryPtr	pPEntry;
	Handle		mapHdl;
	
	/* Traverse process list */
	mapHdl = nil;
	pPEntry = pProcessList;
	while (pPEntry != nil)
		{
		if (pPEntry != pCurrentProcess)
			{
			/* Does this other process have a map of same file? */
			if ((mapHdl = GetMapHdlFromRefNum(refNum, (*pPEntry->p_pcb)->topmaphandle)) != nil)
				break;
			}
		pPEntry = pPEntry->p_NextProcess;
		}
	
	/* Return what we found, maybe nil */
	return(mapHdl);
	}

/* c_getnamedresource.  This is necessary because FullPaint and MacDraw II call this to
 * make sure a desk accessory really exists before they call OpenDeskAcc.  Unknown others
 * might do this: pre-flighting the DRVR resource before OpenDeskAcc was in the original
 * sample app code.
 * The problem is that the Process Mgr puts many things in the Apple Menu that are not
 * DAs.  We return a fake handle, which we also cache in the PEntry so we only have to
 * waste one.  We patch SizeRsrc to return a bogus value if passed the fake handle (have
 * to do this since the handle is actually a resource).
 */
pascal Handle
c_getnamedresource(long thetype, Ptr name)
	{
	unsigned long	olda5;
	Handle			retval;
	
	olda5 = ProcessMgrA5SimpleSetup();

	/* Call through to get the resource handle */
	retval = CALL_FNC_PTR(Getnamedresource_ptr,patchtraps[GETNAMEDRESOURCE].oldtrap,(thetype, name));

	/* If not found, and caller is asking for DRVR by a name that is really an
	 * Apple Menu item, return a fake handle.  Save for later call to SizeRsrc.
	 */
	if ((retval == nil) && (thetype == 'DRVR') && (StringToAppleItemHdl(name) != nil))
		{
		RESERR = noErr;
		if ((retval = pCurrentProcess->p_fakeDAHandle) == nil)
			{
			retval = pCurrentProcess->p_fakeDAHandle = NewHandle(0);
			RESERR = MEMERROR;
			}
		}

	A5Restore(olda5);
	return(retval);
	}

/* c_sizersrc.  Patch to SizeRsrc to go along with GetNamedResource patch. */
#define FAKE_DA_RESOURCE_SIZE	4
pascal long	
c_sizersrc(Handle resHdl)
	{
	unsigned long	olda5;
	long			retval;
	
	olda5 = ProcessMgrA5Setup();

	retval = ((resHdl != nil) && (resHdl == pCurrentProcess->p_fakeDAHandle))
		? FAKE_DA_RESOURCE_SIZE
		: CALL_FNC_PTR(Sizersrc_ptr,patchtraps[SIZERSRC].oldtrap,(resHdl));

	A5Restore(olda5);

	return(retval);
	}
