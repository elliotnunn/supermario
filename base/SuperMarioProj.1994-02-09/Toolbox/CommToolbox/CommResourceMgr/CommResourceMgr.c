/*
	File:		CommResourceMgr.c

	Contains:	Communications Resource Manager

	Written by:	Byron Han

	Copyright:	© 1988-1990, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<13>	 4/28/92	DTY		Remove hasAliasMgr conditional. Always use the Folder Manager.
		<12>	  2/7/92	BH		changed define for MANAGERCODE to be TRUE or FALSE to allow
									CommResourceMgrExtensions.c to properly conditionalize for
									TheFuture and for CubeE and for building the glue file
		<11>	  7/2/91	BH		added __CRMGetHeader
		<10>	 8/15/90	kaz		Fixed C3.0 bug in _CRMResource calls that would do a SUBQ.L
									instead of a CLR.L for a function result => garbage returned.
		 <9>	 7/10/90	dba		add __ to distinguish routines from glue
		 <8>	  7/2/90	kaz		Code shrink/cleanup.
		 <7>	  4/9/90	BBH		* Moving CRMFindCommunications here from
									CommResourceMgrUtilities.a
									• Use Folder Mgr if available (instead of manually creating
									Communications Folder use the Extensions folder) this is
									controlled by a conditional compile (if hasAliasMgr)
		 <6>	 3/22/90	BBH		moved getglobals macro to CommToolboxPriv.h
		 <5>	 3/16/90	BBH		fixed problem where deviceID's never got above 2 (after first
									match of deviceType, we would immediately break)
		 <4>	 3/16/90	BBH		fixed GetDeviceID.  when trying to match deviceID's was looking
									at garbage.
		 <3>	 3/16/90	BBH		create a macro to get CommToolbox globals
		 <2>	 3/15/90	BBH		Fixed GetList to properly return pointer to commtoolbox globals
		 <1>	 3/14/90	BBH		first checked in

	To Do:
*/

#include <SysEqu.h>
#include <Types.h>
#include <Memory.h>
#include <Resources.h>
#include <OSUtils.h>
#include <Errors.h>

#include <CommResources.h>
#include <CommToolboxPriv.h>

#include "CommResourceMgr.h"

//	<12>	changed to do TRUE and FALSE for the define
#define __MANAGERCODE__		TRUE
#include "CommResourceMgrExtensions.c"

#include <Folders.h>

// Should be moved to CommToolboxPriv.h
#define	IsResError()	((*(short *) ResErr) != noErr)

//	initializes the Communications Resource Manager
pascal CRMErr __InitCRM()
{
	/* Shouldn't we return an error if CTB Globals didn't get allocated	*/
	/*

	if CRMGetGlobals & CRMGetGlobals->crmQueue != nil

	*/

	return(noErr);
}

//	Folder stuff for 7.0
pascal OSErr __CRMFindCommunications(vRefNum, dirID)
	short 	*vRefNum;
	long 	*dirID;
{
	return(FindFolder(kOnSystemDisk, kExtensionFolderType, kCreateFolder, vRefNum, dirID));
}

pascal OSErr __CRMCreateCommunications(vRefNum, dirID)
	short 	*vRefNum;
	long 	*dirID;
{
	return(FindFolder(kOnSystemDisk, kExtensionFolderType, kCreateFolder, vRefNum, dirID));
}

//••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
//	Routines for Device Management
//••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••

//	adds element to CRM queue
pascal void __CRMInstall(crmReqPtr)
	QElemPtr		crmReqPtr;
{
	CRMRecPtr		pMatch = (CRMRecPtr) crmReqPtr;

	pMatch->qType = crmType;
	pMatch->crmVersion = crmRecVersion;
	pMatch->crmDeviceID = GetDeviceID(pMatch->crmDeviceType);
	Enqueue(crmReqPtr, CRMGetHeader);
}

//	this removes an entry from the CRM queue
pascal OSErr __CRMRemove(crmReqPtr)
	QElemPtr		crmReqPtr;
{
	return( Dequeue(crmReqPtr, CRMGetHeader) );
}

pascal QHdrPtr __CRMGetHeader(void)
{
	return CRMGetHeader;
}

//	this searches for an entry with the same device type and with a
//	device ID bigger than the one specified.  since we keep the entry list
//	sorted by device ID, we can do a simple search.  maybe we should make this a
//	binary search eventually…
pascal QElemPtr __CRMSearch(crmReqPtr)
	QElemPtr	crmReqPtr;
{
	CRMRecPtr	pMatch = (CRMRecPtr) crmReqPtr;
	CRMRecPtr	pWalker = (CRMRecPtr) CRMGetHeader->qHead;

	for (; pWalker != nil; pWalker = (CRMRecPtr) pWalker->qLink)
		if (pWalker->crmDeviceType == pMatch->crmDeviceType)
			if (pWalker->crmDeviceID > pMatch->crmDeviceID)
				return((QElemPtr) pWalker);					// we're outta here

	return(nil);
}

///////////////////////////////////////////////////////////////////////////////////
//		Utilities Routines
///////////////////////////////////////////////////////////////////////////////////

//	this returns the next free device ID for the given device type in the
//	crm queue.
pascal long GetDeviceID(dType)
	long		dType;
{
	CRMRecPtr	pWalker = (CRMRecPtr)CRMGetHeader->qHead;
	long		rval = 0;

	for (; pWalker != nil; pWalker = (CRMRecPtr) pWalker->qLink)
		if (pWalker->crmDeviceType == dType)							// <4>
			if (pWalker->crmDeviceID > rval)
				rval = pWalker->crmDeviceID;							// remove break <5>

	return(++rval);
}

//••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••
//	Routines for Resource Management
//••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••

pascal long __CRMGetIndex(hResource)
	Handle	hResource;
{
	CRMResourceCountPtr		pMatch = FindResource(hResource);
	long					rval = 0;

	if (pMatch != nil) {
		if (pMatch->useCount <= 0)
			RemoveEntry(pMatch);				// cleanup please…
		else
			rval = pMatch->useCount;
	}

	return(rval);
}

pascal Handle CRMGet10Resource(theType, theID, deep)
	ResType		theType;
	short		theID;
	short		deep;
{
	Handle		theHandle;

	theHandle = (deep == 1 ? Get1Resource(theType, theID) : GetResource(theType, theID));
	// Hopefully we can remove this workaround w/ C3.2
	if (IsResError())
		theHandle = nil;
	HandleResource(theHandle);
	return(theHandle);
}

pascal Handle CRMGet10IndResource(theType, index, deep)
	ResType		theType;
	short		index;
	short		deep;
{
	Handle	theHandle;

	theHandle = (deep == 1 ? Get1IndResource(theType, index) : GetIndResource(theType, index));
	if (IsResError())
		theHandle = nil;
	HandleResource(theHandle);
	return(theHandle);
}

pascal Handle CRMGet10NamedResource(theType, name, deep)
	ResType			theType;
	const Str255 	name;
	short			deep;
{
	Handle	theHandle;

	theHandle = (deep == 1 ? Get1NamedResource(theType, name) : GetNamedResource(theType, name));
	if (IsResError())
		theHandle = nil;
	HandleResource(theHandle);
	return(theHandle);
}

pascal void __CRMReleaseResource(Handle pResource)
{
	CRMResourceCountPtr		pMatch = FindResource(pResource);

	if (pMatch != nil)							// huh?
		if (--(pMatch->useCount) <= 0) {
			RemoveEntry(pMatch);				// pull from list
			pMatch = nil;
		}

	if (pMatch == nil)
		ReleaseResource(pResource);
}

///////////////////////////////////////////////////////////////////////////////////
//		Utilities Routines
///////////////////////////////////////////////////////////////////////////////////

//	this finds the resource count entry for the given resource
CRMResourceCountPtr FindResource(hResource)
	Handle	hResource;
{
	CRMResourceCountPtr		pCRMResourceCount = GetList;

	for (; pCRMResourceCount != nil; pCRMResourceCount = pCRMResourceCount->next)
		if (pCRMResourceCount->theHandle == hResource)
			return(pCRMResourceCount);

	return(nil);
}

//	this clears the resource counts for the file with the refnum specified
//	used for cleanup purposes…
void ClearResource(refnum)
	short	refnum;
{
	CRMResourceCountPtr		curCRM = GetList,
							tempCRM;

	while (curCRM != nil) {
		tempCRM = curCRM->next;
		if (curCRM->refnum == refnum)
			RemoveEntry(curCRM);
		curCRM = tempCRM;
	}
}

//	this routine adds a resource to the resource count list
CRMResourceCountPtr RegisterDevice(hResource)
	Handle					hResource;
{
	CRMResourceCountPtr		pCRMResourceCount =
									(CRMResourceCountPtr) NewPtrSys(sizeof(CRMResourceCount));

	if (pCRMResourceCount != nil) {
		pCRMResourceCount->next = GetList;
		pCRMResourceCount->useCount = 0;
		pCRMResourceCount->theHandle = hResource;

		GetList = pCRMResourceCount;			// put at start of list
												// GetList is a macro
	}

	return(pCRMResourceCount);
}


//	this routine removes an entry from the resource count list
void RemoveEntry(pMatch)
	CRMResourceCountPtr	pMatch;
{
	CRMResourceCountPtr		curCRM = GetList,
							prevCRM = nil;

	for (; curCRM != nil && pMatch != curCRM;
			prevCRM = curCRM, curCRM = curCRM->next)
		;

	if (curCRM == nil)
		return;

	if (prevCRM == nil) 				// eliminating first element
		GetList = curCRM->next;			// GetList is a macro
	else
		prevCRM->next = curCRM->next;

	DisposPtr((Ptr) curCRM);
}

//	handle a resource by adding it to the resource count list if necessary
//		and bumping the useCount if needed

void HandleResource(hResource)
	Handle		hResource;
{
	CRMResourceCountPtr		pMatch;

	if (hResource == nil)
		return;

	pMatch = FindResource(hResource);			// is it there?
	if (pMatch == nil)
		pMatch = RegisterDevice(hResource);		// add to list

	if (pMatch == nil)
		return;									// we are F*CKED!

	pMatch->useCount++;
	pMatch->refnum = HomeResFile(hResource);	// for cleanup purposes
}
