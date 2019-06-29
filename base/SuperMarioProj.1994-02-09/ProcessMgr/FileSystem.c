/*
	File:		FileSystem.c

	Contains:	Process Mgr file system replacement routines.

	Written by: Erich Ringewald

	Copyright:	© 1987-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <3>	  3/5/91	DFH		dnf, #84115: Added support for new forced _Unmount (ignore
									result of notification proc).
		 <0>	  6/1/87	ELR		New Today.

*/ 
 
#include <types.h>
#include <memory.h>
#include <menus.h>
#include <osutils.h>
#include <errors.h>
#include <retrace.h>
#include <segload.h>
#include <files.h>
#include <resources.h>
#include <MFPrivate.h>
#include <Folders.h>

#include "Glue.h"
#include "SysMisc.h"
#include "Lomem.h"
#include "Data.h"
#include "Patches.h"

/************************************************************************************
 * Here are some interesting file system routines that we use to elevate the
 * status of certain volume and disk operations to the system level.  This is
 * because there are system entities (such as Finder and its desktop database)
 * that are really high-level extensions of HFS.  They at least need to be told
 * about, and sometime need to help out with, the comings and goings on.
 ************************************************************************************/

typedef struct VNEntry {
	struct vnentry			**VNNextRequest;
	ProcPtr					VNRoutine;
	unsigned long			VNRefcon;
	ProcessSerialNumber		VNInstaller;
	} VNEntry, *VNPointer, **VNHandle;

/* registry for volume aficionados */
VNHandle		VolumeNotificationList;

/* Function prototypes internal to this file */
void		NotifyVolumeAction(short, VolumeNotice, OSErr, VNHandle);
void		ResetStandardFileVol(short);

/* c_RequestVolumeNotification.  Enlist to receive a call whenever significant
 * disk or volume actions are occurring.
 */
pascal OSErr
c_RequestVolumeNotification(ProcPtr pRoutine, unsigned long refCon)
	{
	ProcessSerialNumber		installer;
	VNHandle				hNewElem;
	u_long					oldA5;
	OSErr					retVal;

	retVal = noErr;
	oldA5 = ProcessMgrA5SimpleSetup();
	
	/* identify the client */
	(void)GetSystemClientProcess(&installer);
	
	/* install new entry */
	if ((hNewElem = (VNHandle) ProcessMgrNewHandle(sizeof(VNEntry))) != nil)
		{
		VNPointer	pNewElem;
		
		pNewElem = *hNewElem;
		pNewElem->VNRoutine = pRoutine;
		pNewElem->VNRefcon = refCon;
		pNewElem->VNInstaller = installer;
		pNewElem->VNNextRequest = VolumeNotificationList;
		VolumeNotificationList = hNewElem;
		}
	else
		retVal = MEMERROR;
	
	A5SimpleRestore(oldA5);
	return(retVal);
	}
		
/* c_DeclineVolumeNotification.  Resign from volume/disk notifications.  */
pascal OSErr
c_DeclineVolumeNotification(ProcPtr pRoutine, unsigned long refCon)
	{
	VNHandle				hBefore, hCurr, hAfter;
	VNPointer				pCurr;
	ProcessSerialNumber		remover;
	u_long					oldA5;

	oldA5 = ProcessMgrA5SimpleSetup();
	
	(void)GetCurrentProcess(&remover);
	hBefore = nil;
	hCurr = VolumeNotificationList;
	while (hCurr != nil)
		{
		pCurr = *hCurr;
		if ((pCurr->VNRoutine == pRoutine) &&
			(pCurr->VNRefcon == refCon) &&
			((EqualPSN(&pCurr->VNInstaller, &remover))
			|| (EqualPSNConst(kSystemProcess, &pCurr->VNInstaller))))
			{
			hAfter = pCurr->VNNextRequest;
			DisposHandle(hCurr);
			if (hBefore == nil)
				VolumeNotificationList = hAfter;
			else
				(*hBefore)->VNNextRequest = hAfter;
			break;
			}
		
		hBefore = hCurr;
		hCurr = (*hCurr)->VNNextRequest;
		}
	
	A5SimpleRestore(oldA5);
	return(noErr);
	}

/* VNKill.  Un-register the dead process from volume notification. */
void
VNKill(void)
	{
	ProcessSerialNumberPtr	pDeadPSN;
	register VNHandle		hBefore, hCurr, hAfter;
	
	pDeadPSN = &pCurrentProcess->p_serialNumber;
	hBefore = nil;
	hCurr = VolumeNotificationList;
	while (hCurr != nil)
		{
		/* Grab the forward link *before* disposing the element */
		hAfter = (*hCurr)->VNNextRequest;
		
		/* Match up the element to the calling process */
		if (EqualPSN(&((*hCurr)->VNInstaller), pDeadPSN))
			{
			DisposHandle(hCurr);
			assert(MEMERROR == noErr);
			if (hBefore == nil)
				VolumeNotificationList = hAfter;
			else
				(*hBefore)->VNNextRequest = hAfter;
			}
		else
			hBefore = hCurr;

		/* Forge ahead */
		hCurr = hAfter;
		}
	}

#pragma segment Main

/* ResetStandardFileVol.  Reset the StandardFile lomems for each process that has its
 * StandardFile pointed at the given volume.
 * NOTE: It's this way for now to be efficient.  When "putting the Process Mgr in ROM",
 * you should probably make it a volume notification routine installed by PACK3.  It
 * would use an OSDispatch call to set the SFSaveDisk and CurDirStore of a process.
 * NOTE: Assumes a5 == PROCESSMGRGLOBALS
 */
void
ResetStandardFileVol(short vRefNum)
	{
	register PCB	*pPCB;
	PEntryPtr		pProc;
		
	/* Turn vRefNum to volume index because that's what StandardFile uses */
	vRefNum = -(vRefNum);

	/* Fix up contexts for all processes.  Effect will be on next minor switch in. */
	pProc = pProcessList;
	while (pProc != nil)
		{
		pPCB = *(pProc->p_pcb);
		if (pPCB->sfsavedisk == vRefNum)
			{
			pPCB->sfsavedisk = 1;
			pPCB->curdirstore = fsRtDirID;
			}
		pProc = pProc->p_NextProcess;
		}
	
	/* Make sure effect is immediate for current process */
	if (SFSAVEDISK == vRefNum)
		{
		SFSAVEDISK = 1;
		CURDIRSTORE = fsRtDirID;
		}
	}

/* NotifyVolumeAction.  Notify all interested parties of a volume action that
 * has just occurred.  A non-nil hEnd can be used to stop the traversal early
 * (notification proceeds from the first element up to, but not including, the
 * one addressed by hEnd).
 * NOTE: Assumes the list does not change in the notification routine!!
 * NOTE: Assumes a5 == PROCESSMGRGLOBALS
 */
void
NotifyVolumeAction(short theVolume, VolumeNotice theNotice, OSErr theResult, VNHandle hEnd)
	{
	VNHandle			hCurr;
	VolumeNoticeBlk		vnBlock;
	OSErr				result;

	/* Reset StandardFile lomem when volume successfully unmounted. */
	if (theNotice == VNUnmount && theResult == noErr)
		ResetStandardFileVol(theVolume);
		
	/* Fill in the parameter block */
	vnBlock.VNBNotice = theNotice;
	vnBlock.VNBVolume = theVolume;
	vnBlock.VNBResult = theResult;
	vnBlock.VNBLength = sizeof(VolumeNoticeBlk);
	
	/* Tell everyone */
	hEnd = (VNHandle) StripAddress(hEnd);
	hCurr = (VNHandle) StripAddress(VolumeNotificationList);
	while (hCurr != hEnd)
		{
		vnBlock.VNBRefCon = (*hCurr)->VNRefcon;
		result = CALL_FNC_PTR(VNProcPtr, (*hCurr)->VNRoutine, (&vnBlock));
		hCurr = (VNHandle) StripAddress((*hCurr)->VNNextRequest);
		}
	}

/* NotifyVolumeGoodbye.  Notify all interested parties that the given volume is
 * about to be ejected or unmounted.  If anyone objects, we abort and pass the
 * error back to the caller (this action can be overriden by specifying the
 * forced parameter as true).
 * NOTE: Assumes the list does not change in the notification routine!!
 * NOTE: Assumes a5 == PROCESSMGRGLOBALS
 */
OSErr
NotifyVolumeGoodbye(short theVolume, VolumeNotice theNotice, VolumeNotice theAbortNotice, Boolean forced)
	{
	VNHandle			hCurr;
	VolumeNoticeBlk		vnBlock;
	OSErr				retval;
	
	/* Fill in the parameter block */
	vnBlock.VNBNotice = theNotice;
	vnBlock.VNBVolume = theVolume;
	vnBlock.VNBResult = noErr;
	vnBlock.VNBLength = sizeof(VolumeNoticeBlk);
	
	/* Tell everyone */
	hCurr = VolumeNotificationList;
	while (hCurr != nil)
		{
		vnBlock.VNBRefCon = (*hCurr)->VNRefcon;
		retval = CALL_FNC_PTR(VNProcPtr, (*hCurr)->VNRoutine, (&vnBlock));
		if ((retval != noErr) && (forced == false))
			{
			NotifyVolumeAction(theVolume, theAbortNotice, retval, hCurr);
			return(retval);
			}
		
		hCurr = (*hCurr)->VNNextRequest;
		}

	return(noErr);
	}

#pragma segment INIT

/* VNInit.  Initialize list volume registration. */
void
VNInit(void)
	{
	VolumeNotificationList = nil;
	}

/************************************************************************************
 * Useful utilities related to HFS.
 ************************************************************************************/
	
#pragma segment	Main

/* DecomposeWD.  Simple utility routine to extract the vRefNum and dirID components
 * of the specified working directory.
 */
OSErr
DecomposeWD(short srcWD, short *dstVolume, unsigned long *dstDirID)
	{
	WDPBRec			wdInfoRec;
	OSErr			result;

	wdInfoRec.ioNamePtr = nil;
	wdInfoRec.ioVRefNum = srcWD;
	wdInfoRec.ioWDIndex = 0;
	wdInfoRec.ioWDProcID = 0;
	wdInfoRec.ioWDVRefNum = 0;
	wdInfoRec.ioCompletion = nil;
	if ((result = PBGetWDInfo(&wdInfoRec, SyncHFS)) == noErr)
		{
		*dstVolume = wdInfoRec.ioWDVRefNum;
		*dstDirID = wdInfoRec.ioWDDirID;
		}
	else
		{
		*dstVolume = 0;
		*dstDirID = 0;
		}
	
	return(result);
	}

/* CheckIfBackingWD.  Check whether the given WD is the one into which the current
 * application was launched.
 */
pascal Boolean
CheckIfBackingWD(short wdRefNum)
	{
	PCB			**hPCB;
	
	return(((hPCB = pCurrentProcess->p_pcb) != nil) && (wdRefNum == (*hPCB)->p_vrefnum));
	}
