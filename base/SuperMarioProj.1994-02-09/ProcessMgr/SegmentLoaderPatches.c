/*
	File:		SegmentLoaderPatches.c

	Contains:	Routines which patch the segment loader traps.

	Written by:	Erich Ringewald and Phil Goldman

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: System

	Change History (most recent first):
	 
		<18>	 5/21/92	gbm		#1030303,<dty>: Start using the new (and improved!) 'SIZE'(1)
									for launching, if it’s present.
		<17>	 1/14/92	YK		Take out <15>,<16> since we found a better way.
		<16>	 1/10/92	YK		Oops, I didn't close the comment.
		<15>	 1/10/92	YK		Check if the app is the Service Window Manager app (for TSM) in
									GetSizeInfo, then set the flag. (this is a tentative solution)
		<14>	 7/26/91	stb		use HOpenResFile instead of old glue
		<13>	 4/18/91	DFH		TED, WS#DFH-910418b : Fixed LaunchApplication to restore the
									zone in the error patch where the file could not be opened.
		<12>	  4/1/91	DFH		(with John Iarocci) Merged in latest AUX support code. NOTE: No
									AUX support code is referenced unless the build defines the
									HAS_AUX_PROCESSMGR condition.
		<11>	 3/25/91	DFH		JSM,#DFH-910325a: Fix SetAppParameters to deal with the fact
									that the fabricated AppleEvent is in an unlocked handle.
		<10>	 2/28/91	DFH		VL,#83781: Removed 32-bit clean check from LaunchApplication. It
									is deemed no longer relevant, since Finder always overrode it.
		 <9>	 2/21/91	DFH		dba,#82504, #82681, #83168, #83182, #83207: Available memory
									calc in StandardLaunch no longer needs special param for
									ProcessMgrMaxBlock to be accurate.
		 <8>	 1/31/91	DFH		NGK,WS#DFH-910131a:Changed SetAppParameters so that launchee
									gets a saeLaunchedWithoutDocs AppleEvent when the launcher
									passed launchAppParameters == nil.
		 <7>	 1/14/91	DFH		(JDR) Conditionalize out AUX support.
		 <5>	 12/5/90	DFH		Integrated AUX support.
		 <4>	11/26/90	DFH		Change trespass() to dbmsg().
		 <2>	10/30/90	csd		Fixed some identifiers to match the newest interfaces.
		 <0>	 x/xx/86	PYG		New Today.

*/ 
 
#include <types.h>
#include <memory.h>
#include <toolutils.h>
#include <osutils.h>
#include <quickdraw.h>
#include <fonts.h>
#include <events.h>
#include <devices.h>
#include <desk.h>
#include <resources.h>
#include <retrace.h>
#include <menus.h>
#include <windows.h>
#include <files.h>
#include <segload.h>
#include <errors.h>
#include <Aliases.h>
#include <folders.h>
#include <MFPrivate.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Aux.h"
#include "Puppet.h"
#include "Zone.h"
#include "AppleEventExtensions.h"

/* Function prototypes internal to this file */
void				OpenAppWDs(PEntryPtr);
void				CopyAppParmHandle(AppParmRecHdl, PCB **);
void				GetSizeInfo(swParmPtr, short);
void				SyncSIZEInfo(void);

/* Some function prototypes that should be in (yet another) header file */
AppParmRecHdl		AEParmToAppParmHandle(AppParametersPtr, PCB **);
Handle				AppParmHandleToAEParm(AppParmRecHdl);
OSErr				DecomposeWD(short, short *, unsigned long *);
void				FPInit(void);
void				NextAppFile(AppFile **);
unsigned long		SizeOfThisAppFile(AppFile *);
void				kill_dce(THz);

/* Macros for easy access to the _Launch parameters */
#define	IS_EXTENDED_BLOCK(pLaunchBlock)	((pLaunchBlock)->launchBlockID == extendedBlock)
#define FIRST_EXTENSION_SIZE			(2*sizeof(short))
#define SECOND_EXTENSION_SIZE			( FIRST_EXTENSION_SIZE + sizeof(ProcessSerialNumber) + 5*sizeof(long) )

/* This is roughly the memory overhead to handle an application that will have
 * half a chance of running.  It includes PCBs, HLists, 'lmem' resources, AppleEvents,
 * etc. that are made in either the system heap or the Process Mgr heap.
 */
#define APPSIZESLOP (0x1000)

/* SyncSIZEInfo.  Sync the SIZE information in case a pre-7.1 system has touched the SIZE(0)
 * information but the SIZE(1) is not yet updated.  We would prefer to use the SIZE(1) resource
 * to do the launch, but only after it is up to date.  Data is updated as follows:  if the
 * psize (preferred size) in the SIZE(0) is different from the same field in SIZE(1), then
 * copy that size into SIZE(1).  Then, if the msize (minimum size) in SIZE(1) is greater than
 * the new psize, set msize the same as psize.  If either SIZE(1) or SIZE(0) are
 * missing, then bail.
 */
void
SyncSIZEInfo()
	{
	swParmHdl		h0, h1;
	
	/* Get the resources */
	h0 = Get1Resource('SIZE', 0);
	h1 = Get1Resource('SIZE', 1);
	if ( h0 && h1 ) // if we have both handles, then do more stuff
		{
		if ( (**h0).psize != (**h1).psize )
			{
			(**h1).psize = (**h0).psize;
			if ( (**h1).msize > (**h1).psize )
				(**h1).msize = (**h1).psize;
			ChangedResource( (Handle) h1 );		// if this fails, we don’t care...
			WriteResource( (Handle) h1 );		// ...the in-memory copy will still be right
			}
		}
	}
	
/* GetSizeInfo.  Extract SIZE information from current file.  Supplies defaults
 * in no-find case.  The defaults were chosen with the assumption that the app
 * is capable of running on a MacPlus.  We also assume that the sizes (default or
 * otherwise) were chosen with the MacPlus stack size in mind, and adjust them
 * according to the stack size we'll actually use for this launch.
 */
void
GetSizeInfo(swParmPtr pSizeInfo, short firstChoice)
	{
	swParmHdl		h;

	/* Make sure the SIZE resources are correct */
	SyncSIZEInfo();

	/* First, try the preferred resource ID.  This will either be a 1 for applications or
	 * a weird negative ID for DAs.  If it’s for a DA and it’s unavailable, give up.  If it’s for
	 * an application and we’re looking for ID 1 and it’s unavailable, then try ID 0.  If neither
	 * ID 0 or ID 1 (made by Finder) are present, try one more time for ID -1.  If ID -1 can’t be
	 * found, then just give up.
	*/
	if ( ((h = Get1Resource('SIZE', firstChoice)) == nil) && (firstChoice == 1) )
		if ( (h = Get1Resource('SIZE', 0)) == nil )
			h = Get1Resource('SIZE', -1);

	/* Copy info if available.  Otherwise, assume the app has old requirements. */
	if (h != nil)
		BlockMove(*h,pSizeInfo,sizeof(swParms));
	else
		{
		pSizeInfo->flags = 0;
		pSizeInfo->psize = (512*1024);
		pSizeInfo->msize = (512*1024);
		}
	
	/* Normalize to MacPlus stack size if not a daemon */
	if (((pSizeInfo->flags & modeOnlyBackground) == 0) && (DEFLTSTACK > MACPLUS_DEFLTSTACK))
		{
		unsigned long		normalization;
		
		normalization = DEFLTSTACK - MACPLUS_DEFLTSTACK;
		pSizeInfo->psize += normalization;
		pSizeInfo->msize += normalization;
		}
	}

/* StandardLaunch.  Subroutinizes most of what it takes to launch an application
 * into a process.  Name and vRefNum specify the application resource file.  Assumes
 * that any necessary working directories have already been opened.
 * Called internally, we may have extra flags on in the taskMode (such as to 
 * indicate a Desk Accessory launch).
 * NOTE: Callers should be able to rely on launchResults->pNewProcess == nil if the result != noErr.
 * NOTE: assumes a5 == PROCESSMGRGLOBALS
 */
void
StandardLaunch(FSSpecPtr pFileSpec, u_short launchFlags, u_long taskMode, Ptr pAppParms, LaunchResultsPtr launchResults, DASpecBlockPtr pDAInfo)
	{
	InternalLaunchPB	ILParams;
	swParms				sizeInfo;
	OSErr				retVal;
	short				rfn, saveCurMap;
	u_size				appSize;
	THz					saveZone;
	
	/* Fill in param block for lower routines */
	ILParams.ilAppSpecPtr = pFileSpec;
	ILParams.ilDAInfoPtr = pDAInfo;
	ILParams.ilStackSize = DEFLTSTACK;
	ILParams.ilAppParameters = pAppParms;
	ILParams.ilAppParamHdl = nil;
	
	/* In case of error */
	launchResults->pNewProcess = nil;
	appSize = 0;
		
	/* Get access to the application resource file SIZE resource.  Switch to the
	 * system heap, since the caller may not have enough room to even create the
	 * resource map when we open the file.
	 */
	saveCurMap = CURMAP;
	SafeSetZone(saveZone, SYSZONE);
		
	if ((taskMode & modeDeskAccessory) != 0)
		{
		UseResFile(0);
		GetSizeInfo(&sizeInfo, DAH_SIZE_RESOURCE_ID);
		}
	else
		{
		SetResLoad(false);
		rfn = HOpenResFile(pFileSpec->vRefNum, pFileSpec->parID, &pFileSpec->name,
			((taskMode & modeMultiLaunch) != 0) ? fsRdPerm : fsCurPerm);
		launchResults->LaunchError = RESERR;
		SetResLoad(true);
		
		/* Fail if we couldn't open the resource file */
		if (rfn == (-1))
			{
			SafeRestoreZone(saveZone);
			return;
			}
		
		/* Set the taskMode.  OR in the MultiLaunch flag, and replace the low word
		 * with control information from the SIZE resource.
		 */
		if ((taskMode & FILE_FLAG_MULTILAUNCH) != 0)
			taskMode |= modeMultiLaunch;
		GetSizeInfo(&sizeInfo, 1); // <18>
	
		/* Don't close yet so that we get opWrErr later on.
		 * NOTE: Closing the app file (we do it here in the non-continue case)
		 * has the potential for screwing up VBLs (and others?) that will still
		 * be running out of its code segments.  Mostly a problem only if the
		 * heap moves before the VBLKILL in the launcher's ExitToShell.
		 */
		if ( ((launchFlags & launchContinue) == 0) || (rfn != CURAPREFNUM) )
			CloseResFile(rfn);
		}
		
	/* Have to restore map, since app file might not have been at top of chain */
	SafeRestoreZone(saveZone);
	CURMAP = saveCurMap;
	
	/* Copy the flags into the task mode */
	ILParams.ilTaskMode = ((taskMode & 0xFFFF0000) | (u_long) sizeInfo.flags);

	/* If it's a BG-only app, is that OK? */
	if (((sizeInfo.flags & modeOnlyBackground) != 0)
		&& ((launchFlags & launchInhibitDaemon) != 0))
		{
		launchResults->LaunchError = appIsDaemon;
		return;
		}
		
	/* Try to launch in the preferred size */
	ILParams.ilPartitionSize = sizeInfo.psize;
	retVal = KernelLaunch(&ILParams);
	
	/* If launch failed because there was not enough room, retry in smaller space
	 * if the user wants to, and the space is available.  Use the largest available
	 * size, as long as it meets the application's minimum size requirement.  Try to
	 * leave enough memory in ProcessMgrZone to allocate subordinate structures, like
	 * the PCB, HLists, etc.
	 * NOTE: Assumes that the partition is the next block allocated from the
	 * ProcessMgrZone (otherwise, we are not guaranteed to get full appSize).
	 */
	if (retVal == memFullErr)
		{
		appSize = ProcessMgrMaxBlock();
		if (appSize < APPSIZESLOP)
			appSize = 0;
		else
			appSize -= APPSIZESLOP;
		
		/* Do we have enough to retry?
		 * NOTE: Assumes that the first KernelLaunch didn't change anything in ILParams.
		 */
		if ( (appSize >= sizeInfo.msize)
			&& ((launchFlags & launchUseMinimum) != 0) )
			{
			ILParams.ilPartitionSize = appSize;
			retVal = KernelLaunch(&ILParams);
			appSize = 0;
			}
		}

	if (retVal == noErr)
		{
		/* Synchronize count for scrap coercion */
		(ILParams.ilResultProc)->p_cutcopycount = cutCopyCount;

		/* Launching an app cancels an saeQuitAll */
		if (pSystemQuitAllMsg != nil)
			CancelQuitAll(&((ILParams.ilResultProc)->p_serialNumber));
		}

	/* Return output information */
	launchResults->pNewProcess = ILParams.ilResultProc;
	launchResults->AvailableSize = appSize;
	launchResults->PreferredSize = sizeInfo.psize;
	launchResults->MinimumSize = sizeInfo.msize;
	launchResults->LaunchError = retVal;
}

#pragma segment INIT

/* LaunchFacelessTasks.  Find and launch all background applications kept in the
 * "extensions" folder.
 * NOTE:  Theoretically, FindFolder or HOpenResFile could move memory by calling
 * _GetResource, which is a problem for the rsrc mgr routine CheckLoad(), which
 * will save and restore TheZone for resources w/ resSysHeap set.  Growing the
 * system heap means moving ProcessMgrZone, so the resource manager save/restore would
 * restore an obsolete value.
 * NOTE: assumes a5 == PROCESSMGRGLOBALS
 * NOTE: assumes THEZONE == ProcessMgrZone since we specifically restore it
 */
void
LaunchFacelessTasks(void)
	{
	LaunchResults		bgLaunchResults;
	swParms				sizeInfo;
	short				rfn, saveCurMap;
	FSSpec				fileSpec;
	CInfoPBRec			cInfoPBRec;

	/* Locate the startup folder. */
	assert(THEZONE == ProcessMgrZone);
	THEZONE = SYSZONE;
	if (FindFolder(kOnSystemDisk, kExtensionFolderType, kDontCreateFolder,
		&fileSpec.vRefNum, &fileSpec.parID) != noErr)
		{
		THEZONE = ProcessMgrZone;
		return;
		}

	/* Now iterate over all the files in the directory */
	cInfoPBRec.hFileInfo.ioNamePtr = &fileSpec.name;
	cInfoPBRec.hFileInfo.ioVRefNum = fileSpec.vRefNum;
	cInfoPBRec.hFileInfo.ioFDirIndex = 0;
	cInfoPBRec.hFileInfo.ioCompletion = nil;
	for(;;)
		{
		/* Reset parameters and get next file.  Quit if directory went bad. */
		cInfoPBRec.hFileInfo.ioDirID = fileSpec.parID;
		cInfoPBRec.hFileInfo.ioFDirIndex++;
		if (PBGetCatInfo(&cInfoPBRec, SyncHFS) != noErr)
			break;
		
		/* Don't try launch if file isn't an application */
		if ( ((cInfoPBRec.hFileInfo.ioFlAttrib & FILE_ATTRIB_ISDIR) != 0)
			|| (cInfoPBRec.hFileInfo.ioFlFndrInfo.fdType != 'appe') )
			continue;
		
		/* Don't launch if it is not a background-only system extension */
		saveCurMap = CURMAP;
		SetResLoad(false);
		rfn = HOpenResFile(fileSpec.vRefNum, fileSpec.parID, &fileSpec.name, fsCurPerm);
		SetResLoad(true);
		if (rfn == (-1))
			continue;
		GetSizeInfo(&sizeInfo, 1); // <18>
		CloseResFile(rfn);
		CURMAP = saveCurMap;
		if ((sizeInfo.flags & modeOnlyBackground) == 0)
			continue;
		
		/* Try the launch */
		StandardLaunch(&fileSpec, 0, 0, nil, &bgLaunchResults, nil);
		}

	THEZONE = ProcessMgrZone;
	}
	
#pragma segment Main

/* c_launch.  Our patch to the _Launch trap.  It has takes a parameter block that
 * may be any of three different sizes.  To make things worse, there are flag
 * fields that can tell us to perform optional extra work so that the caller
 * doesn't have to.  Please excuse my GOTOs, but they were the most effective
 * way to bail out of error conditions without some extremely unwieldy coding.
 * This is essentially a wrapper for our internal routine StandardLaunch.
 */

#define APPPARMS_NOT_SPECIFIABLE		(-1)
pascal long 
c_launch(LaunchPBPtr pLaunchBlock)
	{
	u_short				launchOpts, wdRefNumDst;
	u_long				blockLen, olda5;
	Boolean				alreadyLaunched;
	FInfo				fndrInfo;
	unsigned short		fileFlags;
	Ptr					appParms;
	LaunchResults		appLaunchResults;
	FSSpec				fileSpec, *pFileSpec;
#define pProc		(appLaunchResults.pNewProcess)
#define retVal		(appLaunchResults.LaunchError)
	
	olda5 = ProcessMgrA5SimpleSetup();
	
	/* Clear retVal, pProc, and SIZE information */
	MemClear(&appLaunchResults, sizeof(LaunchResults));

	/* Fill in default values for parameters that might not be there */
	blockLen = 0;
	pFileSpec = nil;
	fileFlags = 0;
	launchOpts = 0;
	appParms = (Ptr) APPPARMS_NOT_SPECIFIABLE;

	/* Get the parameters that were given */
	blockLen = (IS_EXTENDED_BLOCK(pLaunchBlock)) ? pLaunchBlock->launchEPBLength : 0;
	if (blockLen >= FIRST_EXTENSION_SIZE)
		{
		fileFlags = pLaunchBlock->launchFileFlags;
		launchOpts = pLaunchBlock->launchControlFlags;
		}

	if (blockLen >= SECOND_EXTENSION_SIZE)
		{
		pFileSpec = pLaunchBlock->launchAppSpec;
		pLaunchBlock->launchPreferredSize = 0;
		pLaunchBlock->launchMinimumSize = 0;
		pLaunchBlock->launchAvailableSize = 0;
		if ((appParms = pLaunchBlock->launchAppParameters) == APPPARMS_NOT_SPECIFIABLE)
			appParms = nil;
		}	
	else
		{
		GetVol(0, &wdRefNumDst);
		if ((retVal = FSMakeFSSpec(wdRefNumDst, 0, (StringPtr) pLaunchBlock->reserved1, &fileSpec)) != noErr)
				goto AfterLaunch;
		pFileSpec = &fileSpec;
		}
	
	/* Launch the application into a process.  Relaunch existing application only if
	 * it is the caller himself and he is not trying to get a second copy!.
	 */
	alreadyLaunched = ((pProc = PEntryFromFileSpec(pFileSpec, nil)) != nil);
	if ( (alreadyLaunched == false) ||
		((pProc == pCurrentProcess) && ((launchOpts & launchContinue) == 0)) )
		{
		/* Get file's flags if the caller wants us to */
		if ((launchOpts & launchNoFileFlags) != 0)
			{
			if ((retVal = FSpGetFInfo(pFileSpec, &fndrInfo)) != noErr)
				goto AfterLaunch;
			fileFlags = fndrInfo.fdFlags;
			}
		
		/* Try the launch */
		StandardLaunch(pFileSpec, launchOpts, (u_long) fileFlags, appParms, &appLaunchResults, nil);
		}
	
	/* Return output parameters */
	if (blockLen >= SECOND_EXTENSION_SIZE)
		{
		if (retVal == noErr)
			pLaunchBlock->launchProcessSN = pProc->p_serialNumber;
		else
			SetPSN(kNoProcess, &pLaunchBlock->launchProcessSN);

		pLaunchBlock->launchPreferredSize = appLaunchResults.PreferredSize;
		pLaunchBlock->launchMinimumSize = appLaunchResults.MinimumSize;
		pLaunchBlock->launchAvailableSize = appLaunchResults.AvailableSize;
		}
		
AfterLaunch:
	
	/* Arrange for the new app layer to be created in front, unless overridden.  We then
	 * put the newborn to sleep until fg_resched decides to schedule it.  This ensures
	 * that the app gets its first time in the foreground, and when there are no modals
	 * up.  A "don't switcher" will be created in the very back (it doesn't matter
	 * whether there's a modal in this case).
	 * If the app was already launched, a simple SetFrontProcess will do.
	 */
	if (retVal == noErr)
		{
		if ( ((launchOpts & launchDontSwitch) == 0) &&
			((pProc->p_taskmode & modeOnlyBackground) == 0) )
			{
			if ((SetFrontProcess(&pProc->p_serialNumber) == noErr)
				&& (alreadyLaunched == false))
				PutOnSleepQueue(pProc, MAXVALUE);
			}
		}

	/* Old style launch causes the launcher to quit.  This is true even if the new
	 * application fails to launch (no error return possible, so just beep).
	 */
	if ((launchOpts & launchContinue) == 0)
		{
		if (retVal != noErr)
			SysBeep(20);
		
		A5SimpleRestore(olda5);
		ExitToShell();
		dbmsg("ExitToShell in LaunchApplication failed.");
		}
	
	A5SimpleRestore(olda5);
		
	/* Return short process descriptor or error to caller */
	return ( ((retVal == noErr) && (blockLen < SECOND_EXTENSION_SIZE)) ? pProc->p_mypid : retVal);
#undef pProc
#undef retVal
	}

#pragma segment kernel_segment

/* NewAppParmHandle.  Create a blank AppParmRecHdl of the specified size.
 * NOTE: Locking the AppParmHandle is done explicitly for MacWrite 4.5 (and
 * 4.6?) and should someday be removed.
 */
AppParmRecHdl
NewAppParmHandle(Size theSize)
	{
	AppParmRecHdl		retHdl;
	
	ResrvMem(theSize);
	if ((retHdl = (AppParmRecHdl) NewHandleClear(theSize)) != nil)
		HLock(retHdl);
	return(retHdl);
	}

/* SizeOfThisAppFile.  Figure out the size of the given AppFile structure.  Allows
 * for even alignment.
 */
unsigned long
SizeOfThisAppFile(AppFile *pAppFile)
	{
	register unsigned long	theSize;
	
	theSize = 2*sizeof(short) + sizeof(OSType) + Length(&(pAppFile->fName))+ 1;
	WORD_ALIGN(theSize);	
	return(theSize);
	}

/* NextAppFile.  Moves the specified AppFile pointer past its data. */
void
NextAppFile(AppFile **ppAppFile)
	{
	AppFile			*pAppFile;
	
	/* Bump pointer by size of data */
	pAppFile = *ppAppFile;
	(Ptr) pAppFile += SizeOfThisAppFile(pAppFile);
	
	*ppAppFile = pAppFile;
	}

/* CopyAppParmHandle.  Copy the specified AppParmRecHdl into the current zone, and
 * set up the p_appWDList for later consumption by OpenAppWDs.
 */
void
CopyAppParmHandle(AppParmRecHdl sourceAPH, PCB **hPCB)
	{
	AppWDRecPtr		dstLoc;
	AppWDListPtr	pWDList;
	AppWDListHdl	hWDList;
	AppParmRecPtr	pAPH;
	AppFile			*parmLoc;
	short			parmCount = 0, sizeOfStruct;

	/* In case of error */
	(*hPCB)->p_appWDList = nil;

	/* Copy the APPPARMHANDLE into new zone.  Allocate it low so we can lock it.
	 * NOTE: Locking the AppParmHandle is done explicitly for MacWrite 4.5 (and
	 * 4.6?) and should someday be removed.
	 */
	ResrvMem(GetHandleSize(sourceAPH));
	HandToHand(&sourceAPH);
	if (MEMERROR != noErr)
		return;
		
	HLock(sourceAPH);
	APPPARMHANDLE = sourceAPH;
	pAPH = *sourceAPH;

	/* See how many files there are.  Get out if there are none. */
	parmCount = pAPH->count;
	if (parmCount == 0)
		return;
		
	/* Allocate the list in a relocatable block in the current heap */
	sizeOfStruct = sizeof(AppWDList) + (parmCount * sizeof(AppWDRec));
	if ( (hWDList = (AppWDListHdl) NewHandleClear(sizeOfStruct)) == nil )
		{
		DisposHandle(sourceAPH);
		return;
		}
	
	/* Traverse the APPPARMHANDLE to save off the WD information therein */
	pWDList = *hWDList;
	pWDList->wdlCount = parmCount;
	dstLoc = &(pWDList->wdlDirectories);	
	parmLoc = &(pAPH->appFiles[0]);
	while (parmCount-- != 0)
		{
		(void) DecomposeWD(parmLoc->vRefNum, &dstLoc->awdVolume, &dstLoc->awdDirID);
		parmLoc->vRefNum = 0;
		NextAppFile(&parmLoc);
		dstLoc++;
		}
	
	(*hPCB)->p_appWDList = hWDList;
	}

/* SetAppParameters.  Ensures that the new app gets the intended parameters in a
 * form it can understand.  This can mean translating from an AppleEvent to an
 * APPPARMHANDLE, or vice versa.  It definitely means saving the necessary working
 * directory info.
 */
void
SetAppParameters(PCB **hPCB, unsigned long currentA5, register InternalLaunchPBPtr pParams, Boolean appDoesEppc)
	{
	PCB					*pPCB;
	DASpecBlockPtr		pDAInfo;
	AppParmRecHdl		aph;
	struct appparmarea	*parm;
	
	/* Get the resource file directory.  If this is DA Handler being launched,
	 * the resource file directory is the one containing the Desk Accessory file.
	 */
	pPCB = *hPCB;
	if ((pDAInfo = pParams->ilDAInfoPtr) == nil)
		{
		pPCB->p_appVolume = (pParams->ilAppSpecPtr)->vRefNum;
		pPCB->p_appDirID = (pParams->ilAppSpecPtr)->parID;
		}
	else
		{
		pPCB->p_appVolume = (pDAInfo->daFileSpec)->vRefNum;
		pPCB->p_appDirID = (pDAInfo->daFileSpec)->parID;
		}

	/* Convert DocList to AppParmRecHdl or vice-versa.  Do nothing for the thoroughly
	 * modern case of the launcher giving an AppleEvent to an AppleEvent-aware launchee,
	 * except to ensure that the launchee gets an saeLaunchedWithoutDocs ('oapp') in
	 * case ilAppParameters is nil (AppleEvent-aware applications count on seeing an
	 * AppleEvent at startup, knowing that it came from the launcher and not sometime
	 * after they were launched).
	 */
	aph = APPPARMHANDLE;
	APPPARMHANDLE = nil;
	if (pParams->ilAppParameters != APPPARMS_NOT_SPECIFIABLE)
		{
		if (appDoesEppc == false)
			{
			APPPARMHANDLE = AEParmToAppParmHandle(pParams->ilAppParameters, hPCB);
			pParams->ilAppParameters = nil;
			}
		else if (pParams->ilAppParameters == nil)
			pParams->ilAppParamHdl = AppParmHandleToAEParm(nil);
		}
	else
		{
		pParams->ilAppParameters = nil;
		if (appDoesEppc)
			pParams->ilAppParamHdl = AppParmHandleToAEParm(aph);
		else if ((aph != nil) && (*aph != nil))
			CopyAppParmHandle(aph, hPCB);
		}
		
	/* fix up application parameter area */
	parm = (struct appparmarea *) ((Ptr)currentA5 + 8);
	parm->stdin = 0;
	parm->stdout = 0;
	parm->physexec = 0;
	parm->parmhandle = APPPARMHANDLE;
	}

/* OpenAppWDs.  Open the working directories needed by a new application.  Also
 * sets CURDIRSTORE and SFSAVEDISK according to the last file in APPPARMHANDLE
 * (if there is no APPPARMHANDLE, it uses the resource file directory).
 */
void
OpenAppWDs(PEntryPtr pProc)
	{
	AppWDListHdl	hWDList;
	AppWDRecPtr		pWDInfo;
	short			wdCount;
	WDPBRec			wdpbrec;
	AppFile			*pAppFile;
	PCB				*pPCB;

	/* Open home working directory */
	wdpbrec.ioNamePtr = nil;
	wdpbrec.ioCompletion = nil;
	pPCB = *(pProc->p_pcb);
	wdpbrec.ioVRefNum = pPCB->p_appVolume;
	wdpbrec.ioWDDirID = pPCB->p_appDirID;
	wdpbrec.ioWDProcID = 0;
	PBOpenWD(&wdpbrec, SyncHFS);
	(*pProc->p_pcb)->p_vrefnum = wdpbrec.ioVRefNum;
	SetVol(nil, wdpbrec.ioVRefNum);

	/* Get list of parameter WDs to open, if any. */
	if ((hWDList = (*pProc->p_pcb)->p_appWDList) == nil)
		{
		SFSAVEDISK = -(pPCB->p_appVolume);
		CURDIRSTORE = pPCB->p_appDirID;
		return;
		}
	
	/* Traverse said list, opening a WD from the component info.  Note that the
	 * volume/dirid that ultimately get used for SFSAVEDISK and CURDIRSTORE will
	 * be from the last AppFile in APPPARMHANDLE.
	 */
	HLock(hWDList);
	wdCount = (*hWDList)->wdlCount;
	pWDInfo = &((*hWDList)->wdlDirectories);
	if (APPPARMHANDLE != nil)
		pAppFile = &((*(AppParmRecHdl) APPPARMHANDLE)->appFiles[0]);
	
	while (wdCount-- != 0)
		{
		wdpbrec.ioVRefNum = pWDInfo->awdVolume;
		wdpbrec.ioWDDirID = pWDInfo->awdDirID;
		wdpbrec.ioWDProcID = 0;
		PBOpenWD(&wdpbrec, SyncHFS);
		
		pAppFile->vRefNum = wdpbrec.ioVRefNum;
		SFSAVEDISK = -(pWDInfo->awdVolume);
		CURDIRSTORE = pWDInfo->awdDirID;
		NextAppFile(&pAppFile);
		pWDInfo++;
		}
	
	/* Free the memory */
	DisposHandle(hWDList);
	(*pProc->p_pcb)->p_appWDList = nil;
	}

/* BeginApplication.  Code executed by a newborn process after it is switched in for
 * the very first time.  Set up the toolbox and OS worlds.
 */
void
BeginApplication(void)
	{
#ifdef HAS_AUX_PROCESSMGR
	void			(*auxidleprocptr)();
#endif HAS_AUX_PROCESSMGR
	unsigned long	olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	if (MachineHasFPU)
		FPInit();

	/* Fix up working directories for home directory and files in APPPARMHANDLE */
	OpenAppWDs(pCurrentProcess);
#ifdef HAS_AUX_PROCESSMGR
	auxidleprocptr = ((pCurrentProcess->p_taskmode & COFF_BINARY) != 0) ? AUX_IdleProc : nil;
#endif HAS_AUX_PROCESSMGR
	A5SimpleRestore(olda5);
	
	/* now call toolbox/system initializing trap */
	InitApplication();
	
#ifdef HAS_AUX_PROCESSMGR
	/* Let coff binary be initialized and scheduled by AUX.
	 * NOTE: AUX context switch will probably occur here…
	 */
	if (auxidleprocptr != nil)
		(*auxidleprocptr)();
#endif HAS_AUX_PROCESSMGR
	}

#pragma segment kernel_segment

/* c_rdrvrinstall.  Fix up the unit table when the application zone is nuked.
 * NOTE:  Also, gives appropriate GBKisses to drivers, which is not done in the ROM
 * routine (Done inline in _ExitToShell/_Launch) but no one except _ExitToShell should
 * be calling this anyway.
 * NOTE: Needs to be same segment as kill_dce.
 */
pascal void
c_rdrvrinstall(void)
	{
	kill_dce(APPLZONE);
	}
