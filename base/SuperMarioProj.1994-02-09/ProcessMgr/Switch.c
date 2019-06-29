/*
	File:		Switch.c

	Contains:	Process switching routines.

	Written by:	Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<22>	10/28/92	DTY		Use new Get/Set macros to access ExpandMem.
		<21>	10/27/92	DTY		Change BlockMoves to BlockMoveData.
		<20>	 9/11/92	DRF		Take out “while(FSBUSY);” in “TheFuture” to allow for faster
									switching.
		<19>	 8/26/92	DTY		Roll in latest changes for A/UX.
		<18>	 3/30/92	DTY		#1025416,<DDG>: Tell Skia to switch it’s globals on a context
									switch.
		<17>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		<16>	 2/18/92	DTY		#1021226: Switch emMessageManagerGlobals again.
		<15>	 10/4/91	JSM		Change PsychoticFarmerOrLater conditionals to TheFuture.
		<14>	 9/22/91	DTY		Change PsychoticFarmerAndLater to PsychoticFarmerOrLater.
		<13>	 9/13/91	DTY		Conditionalise previous change so it doesn’t get built for
									CubeE. (It’ll get built for PsychoticFarmerAndLater.)
		<12>	  6/4/91	DFH		Switch emMessageManagerGlobals.
		<11>	 1/28/91	DFH		JSM,#81425:Include MFPrivate.h, since that's where
									DisableSwitching and EnableSwitching are now.
		<10>	 1/18/91	DFH		(ewa) Fix kill_vbl check of VBL record address. Was checking the
									wrong element.
		 <9>	 1/15/91	DFH		(VL) Conditionalize out the AUX switch prototypes.
		 <8>	 1/14/91	DFH		(JDR) Conditionalize out AUX support.
		 <6>	12/19/90	gbm		(dba) Fix off-by-one bug in an A/UX change. Also kill VBLs
		 							whose queue entries are in an application’s heap as well
									as those whose task pointers are.
		 <5>	 12/5/90	DFH		Integrated AUX support.
		 <4>	 11/6/90	DFH		Renamed emAppleEventsGlobal to emAppleEvents.
		 <3>	 11/1/90	DFH		Unconditionalized edition mgr lomem switch.
		 <0>	  9/2/86	ELR		New Today.

*/ 
 
#include <types.h>
#include <memory.h>
#include <menus.h>
#include <osutils.h>
#include <quickdraw.h>
#include <events.h>
#include <resources.h>
#include <retrace.h>
#include <osutils.h>
#include <fonts.h>
#include <devices.h>
#include <errors.h>
#include <ExpandMemPriv.h>
#include <MFPrivate.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Patches.h"
#include "Aux.h"

#ifdef MORE_SEGMENTS
#pragma segment	kernel_segment
#endif MORE_SEGMENTS

void				SwitchSkiaGlobals(ProcessSerialNumber theNewProcess)
 = {0x303C, 0xFFFE, 0xA832};
 
/* Some function prototypes that should be in (yet another) header file */
void				save_or_restore_dispatch(PCB *, int);
void				kill_dispatch(PCB *);
void				SwitchAllRegs(unsigned long *, unsigned long);
void				SwitchCPURegs(unsigned long *, unsigned long);
#ifdef HAS_AUX_PROCESSMGR
void				aux_ctxsw (unsigned long *, unsigned long, long);
void				aux_fpxsw (unsigned long *, unsigned long, long);
#endif HAS_AUX_PROCESSMGR
void				save_lomem(PEntryPtr);
void				restore_lomem(PEntryPtr, Boolean);
void				save_lmemtab(Ptr);
void				restore_lmemtab(Ptr);

/* Function prototypes internal to this file */
void				save_state(PEntryPtr);
void				save_dce(PEntryPtr);
void				save_vbl(PEntryPtr);
void				AtomicVBLSave(Handle, unsigned long, unsigned long);
void				save_hooks(PEntryPtr);
unsigned short		restore_state(PEntryPtr, Boolean);
void				restore_dce(PEntryPtr);
void				restore_vbl(PEntryPtr);
void				restore_hooks(PEntryPtr);
void				kill_dce(THz);
void				GiveGoodByeKiss(DCtlPtr);
void				kill_vbl(PEntryPtr);
void				kill_hooks(void);

/* Scheduling constant to cause applications to yield the CPU even if there is 
 * is no other reason to switch.  For foreground processes, this is checked
 * only when there are no pending events.  For background processes, it is
 * checked before events are.  Of course, the process can continue with the
 * processor (get a new quantum right away), if there are no other processes
 * ready to be scheduled.
 */ 
#define QUANTUM		(5)

/* switch_task.  Switch to the specified process.  The outgoing process,
 * pCurrentProcess, can be nil.  It happens when switching out of the dead
 * app in c_exittoshell().
 */
void 
switch_task(register PEntryPtr pNewProc)
	{
	register PEntryPtr		pOldProc;
	unsigned short			ps;
#ifdef HAS_AUX_PROCESSMGR
	long					auxPID;
#endif HAS_AUX_PROCESSMGR
	unsigned long			olda5;
	Ptr						ourQDGlobals;
	unsigned long			*oldStackSave;
	
	assert((pNewProc != nil) && (IsProcessEntry(pNewProc))
			&& (pNewProc->p_state != PRSLEEPING));
	
	/* Save Process Mgr a5 and proc globals */
	olda5 = ProcessMgrA5Setup();
	ourQDGlobals = *((Ptr *) olda5);
	
	/* Do nothing if this would all be a NOP */
	if ((pOldProc = pCurrentProcess) == pNewProc)
		{
		A5Restore(olda5);
		return;
		}
	
	/* Let the error handling routines know we can't handle switching now,
	 * but only do it if we'll see the code on the other side of SwitchxxxRegs().
	 */
	if (pNewProc->p_slices > 0)
		DisableSwitching();

#if	TheFuture

//	We don't do anything special for old apps anymore

#else
	/* Wait until file system and AppleShare async calls are done.
	 * NOTE: This is done for the sole reason that MacWrite (4.5??) has an
	 * HFS completion routine that doesn't set up A5.  If we can workaround
	 * (or forget) them, we should.  This would let switches go faster, and
	 * allow apps to sleep pending I/O completion (i.e. WNE with big sleep,
	 * and an I/O completion routine that calls WakeupProcess).
	 */
#ifdef HAS_AUX_PROCESSMGR
	/* No need to wait for synchronous FM since A/UX does it asynchronously */
	if (! AUXIsPresent)
#endif HAS_AUX_PROCESSMGR
	while (FSBUSY)
		;

#endif	TheFuture

	// <18> Call Skia to let it know that a context switch is occuring
	
	if (skiaExists)
		SwitchSkiaGlobals(pNewProc->p_serialNumber);
	
	/* Update count of switched-in time for outgoing process.
	 * NOTE: On TICKS rollover we don't bother to update… who'll miss 'em?
	 * Assume we never have to worry about p_activeTime rollover.
	 */
	if ((lastswitch < TICKS) && (pOldProc != nil))
		pOldProc->p_activeTime += (TICKS - lastswitch);
	lastswitch = TICKS;
	
	/* Update count of slices for incoming process */
	pNewProc->p_slices++;
		
	/* Set time of next quantum cut (even works if TICKS is about to roll over) */
	nextswitch = TICKS + QUANTUM;

	/* Invalidate current process information during switch */
	pCurrentProcess = nil;
	
	/* Save outgoing state, set up incoming state */
	save_state(pOldProc);
	ps = restore_state(pNewProc, false);

	/* Everything is in place.  It's time for the final act of switching the CPU
	 * context from the old to the new.  The SwitchAllRegs and SwitchCPURegs routines
	 * are entered by the old process, but are returned from by the new one.  The most
	 * interesting part is that this switches the stack pointer: all local variables
	 * used after this use the values that the incomer saved when it was the outgoer.
	 */
	pCurrentProcess = pNewProc;
	oldStackSave = (pOldProc != nil) ? &pOldProc->p_sp : nil;
#ifdef HAS_AUX_PROCESSMGR
	if (AUXIsPresent)
		{		
		auxPID = ((pOldProc == nil) || (pNewProc->aux_realpid != pOldProc->aux_realpid)) ? pNewProc->aux_realpid : 0;
		if (MachineHasFPU)
			aux_fpxsw(oldStackSave, pNewProc->p_sp, auxPID);
		else
			aux_ctxsw(oldStackSave, pNewProc->p_sp, auxPID);
		}
	else
		{
#endif HAS_AUX_PROCESSMGR
		if (MachineHasFPU)
			SwitchAllRegs(oldStackSave, pNewProc->p_sp);
		else
			SwitchCPURegs(oldStackSave, pNewProc->p_sp);
#ifdef HAS_AUX_PROCESSMGR
		}
#endif HAS_AUX_PROCESSMGR

	/* Restore interrupt level to what old guy saved before passing the torch */
	spl(ps);
		
	/* let the error handling routines know we can handle switching again */
	EnableSwitching();

	/* Restore his A5 world and his quickdraw globals */
	A5Restore(olda5);
	*((Ptr *) olda5) = ourQDGlobals;
	
	/* Now kill him! */
	if (pCurrentProcess->p_condemned)
		{
		DSERRCODE = noErr;
		ExitToShell();
		}
}

#pragma segment	kernel_segment

/* save_state.  Save all process specific stuff in the process state records.
 * We make an effort to remove all executable things (drivers, vbls, etc)
 * before switching the patches or lomem.  This guarantees that those
 * executables never run without the context the need.
 * NOTE: Should we switch timer tasks?
 * NOTE: VBLs, drivers, and DAs in the system heap are not switched out by
 * this code, so they better not rely on low memory and patches being
 * fair game.  Not only that, but they shouldn't rely on TWGetPID to tell
 * them, because they'll see an incorrect value during the switch.
 */
void
save_state(PEntryPtr pPEntry)
	{
	PCB			**pcbHdl;
	
	/* Weed out the degenerates */
	if ( (pPEntry == nil) || (pcbHdl = pPEntry->p_pcb) == nil)
		return;
	
	/* Disarm executables */
	save_dce(pPEntry);
	save_vbl(pPEntry);
	save_hooks(pPEntry);
	
	/* Remove the guy's patches and save off low memory */
	save_or_restore_dispatch(*pcbHdl, SAVE_DISPATCH);
	save_lomem(pPEntry);
	}

/* restore_state.  Restores all process specific stuff from the state records.
 * NOTE:  Interrupts must be turned off while restoring volatile portions of
 * low memory (e.g. STKLOWPT), because they may be affected by the interrupts
 * and VBLs that execute even as we speak.
 */
unsigned short 
restore_state(register PEntryPtr pPEntry, Boolean shouldRestoreVolatile)
	{
	u_short		ps;

	if (pPEntry->p_pcb == nil)
		return(0);
	
	/* Restore data context (lomem) */
	restore_lomem(pPEntry, shouldRestoreVolatile);

	/* Now restore executable context */
	save_or_restore_dispatch(*pPEntry->p_pcb, RESTORE_DISPATCH);
	restore_dce(pPEntry);
	restore_vbl(pPEntry);
	restore_hooks(pPEntry);
	
	/* Restore volatile state with interrupts off.  These were deferred by
	 * restore_lomem() until we got to this section with interrupts off.
	 * Please excuse the lack of layering.
	 * NOTE:  To leave interrupts off even less we could lift this into
	 * switch_task() itself.
	 */
	if (shouldRestoreVolatile == false)
		{
		ps = disable();
		STKLOWPT = (*pPEntry->p_pcb)->stklowpt;
		}
		
	return(ps);
	}

/* kill_state.  Chuck the switched context of the specified process.  The idea here is
 * a) neutralize the context, and b) make sure that the info is lost so that another
 * save_state/restore_state would not re-install any of the custom context.  This is
 * important when calling StandardLaunch in HandleShellDeath.  We want the restore_state
 * at the end of NewProcess to not re-install patches, dces, etc.  The dces, vbls, and
 * hooks are naturally handled, since the save_state will see genericness.   The dispatch
 * table, tho, must be explicitly removed now.
 */
void
kill_state(PEntryPtr pPEntry)
	{
	PCB				**pcbHdl;

	/* Do nothing if app had no state info */
	if ((pcbHdl = pPEntry->p_pcb) == nil)
		return;
	
	/* Neutralize! */
	kill_dispatch(*pcbHdl);
	kill_dce((*pcbHdl)->applzone);
	kill_vbl(pPEntry);
	kill_hooks();
	}
	
/* Save_lomem copies the current low memory state for the process described
 * by `pp'.
 *
 * Low memory locations can be divided into 4 classes:
 *	(1) Undefined Memory:  Doesn't matter what we do with these
 *	(2) Application Specific Memory:  Must be switched.
 *	(3) Static System Memory:  Must be switched.
 *		Only examples so far: BOOTVOL (SWITCHDATA?).
 *	(4) Dynamic System Memory: Must not be switched.
 *
 * Low memory should be switched out only after all the executable objects that
 * use the low memory have been switched out.  These executables include VBLs,
 * drivers, and "hooks".
 *
 * NOTE: Assumes A5 = PROCESSMGRGLOBALS
 */
void
save_lomem(PEntryPtr pp)
	{
	PCB			*pc;

	assert(pp->p_pcb != nil && pp->p_lmemtool != nil);

	pc = *pp->p_pcb;
		
	/* Save ExpandMem switchables */
	pc->emScriptAppGlobals = GetExpandMemScriptAppGlobals();
	pc->emAppleEvents = GetExpandMemAppleEvents();
	pc->emEditionMgrPerApp = GetExpandMemEditionMgrPerApp();
	pc->emMessageManagerGlobals = GetExpandMemMessageManagerGlobals(0);

	/* Save OS lomem */
	pc->bootvol = BOOTVOL;
	pc->switchdata = SWITCHDATA;
	pc->defvcbptr = DEFVCBPTR;
	pc->defaultwdcb = *((WDCBPtr) (WDCBSPTR + 2));
	pc->defvrefnum = DEFVREFNUM;
	pc->dserrcode = DSERRCODE;
	
	/* Save toolbox lomem */
	save_lmemtab(*pp->p_lmemtool);
	
	/* Save randomly accessed toolbox lomem */
	pc->stklowpt = STKLOWPT;
	pc->appllimit = APPLLIMIT;
	pc->applzone = APPLZONE;
	pc->currenta5 = (unsigned long)CURRENTA5;
	BlockMoveData(CURAPNAME, &pc->curapname, *((unsigned char *)CURAPNAME) + 1);
	pc->curlayer = GetCurLayer();
	pc->topmaphandle = TOPMAPHANDLE;
	pc->curmap = CURMAP;
	if (Colorized)
		pc->menucinfo = MENUCINFO;
	pc->sfsavedisk = SFSAVEDISK;
	pc->curdirstore = CURDIRSTORE;
	}

/* restore_lomem.  Switches in the low memory settings of the given process.
 * Low memory should be switched in before any executable objects that
 * use the low memory have been switched in.  These executables include VBLs,
 * drivers, and "hooks".
 */
void
restore_lomem(PEntryPtr pp, Boolean shouldRestoreVolatile)
	{
	PCB					*pc;
	unsigned short		ps;

	assert(pp->p_pcb != nil && pp->p_lmemtool != nil);

	pc = *pp->p_pcb;

	/* Restore location zero to it's startup time value */
	LOCATIONZERO = initLocationZero;
	
	/* Restore ExpandMem switchables */
	SetExpandMemScriptAppGlobals(pc->emScriptAppGlobals);
	SetExpandMemAppleEvents(pc->emAppleEvents);
	SetExpandMemEditionMgrPerApp(pc->emEditionMgrPerApp);
	SetExpandMemMessageManagerGlobals(0, pc->emMessageManagerGlobals);

	/* Restore OS lomem */
	BOOTVOL = pc->bootvol;
	SWITCHDATA = pc->switchdata;
	DEFVCBPTR = pc->defvcbptr;
	*((WDCBPtr) (WDCBSPTR + 2)) = pc->defaultwdcb;
	DEFVREFNUM = pc->defvrefnum;
	DSERRCODE = pc->dserrcode;
	
	/* Restore toolbox lomem */
	restore_lmemtab(*pp->p_lmemtool);

	/* Restore interrupt sensitive lomem. */
	if (shouldRestoreVolatile)
		{
		ps = disable();
		STKLOWPT = pc->stklowpt;
		APPLZONE = pc->applzone;
		spl(ps);
		}
	else
		{
		STKLOWPT = 0;
		APPLZONE = pc->applzone;
		}
	
	/* Restore randomly accessed toolbox lomem.
	 * NOTE: These must be done after the other toolbox entries since we may
	 * explicitly store values here (e.g. newproc()).
	 */
	APPLLIMIT = pc->appllimit;
	CURRENTA5 = pc->currenta5;
	BlockMoveData(&pc->curapname, CURAPNAME, Length(&(pc->curapname)) + 1);
	SetCurLayer(pc->curlayer);
	TOPMAPHANDLE = pc->topmaphandle;
	CURMAP = pc->curmap;
	if (Colorized)
		MENUCINFO = pc->menucinfo;
	SFSAVEDISK = pc->sfsavedisk;
	CURDIRSTORE = pc->curdirstore;
	}
	
/* save_vbl.  Go through all of the current VBL tasks and replace the receiver
 * addresses with dummies, and set all counts to a large number, making the VBL
 * a NOP.
 * This must be called before save_or_restore_dispatch and save_lomem, to ensure
 * that no VBLs run without their required context.
 */
void
save_vbl(PEntryPtr pp)
	{
	unsigned long		base, bound;
	
	base = (u_long) pp->p_zone;
	bound = base + pp->p_size;
	AtomicVBLSave((*pp->p_pcb)->vblvars, base, bound);
	}

/* restore_vbl.  Reinstate the VBLs that were swapped out by previous save_vbl. */
void
restore_vbl(PEntryPtr pp)
	{
	VBLTask			*pVBL;
	VBLDescPtr		pVBLSave;
	VBLDescHdl		hVBLSave;
	short			vblDescCount;
	unsigned short	ps;

	assert(pp->p_pcb != nil);
		
	hVBLSave = (*pp->p_pcb)->vblvars;
	vblDescCount = (u_long) GetHandleSize((Handle) hVBLSave) / sizeof(VBLDesc);
	
	pVBLSave = *hVBLSave;
	while (--vblDescCount >= 0) // <6>
		{
		pVBL = pVBLSave->v_eladdr;
		assert(pVBL->qType == vType);
		
		ps = disable();
		pVBL->vblCount = pVBLSave->v_count;
		pVBL->vblAddr = pVBLSave->v_addr;
		spl(ps);
		
		pVBLSave++;
		}
	}
	
/* kill_vbl.  Remove all VBL tasks local to the given process.
 * Note that under A/UX tasks above PhysMemTop should be killed as well.
 */
void 
kill_vbl(PEntryPtr pProc)
	{
	register VBLTask			*pThisVBL, *pNextVBL;
	register unsigned long		vblAddr, base, bound;
#ifdef HAS_AUX_PROCESSMGR
	register unsigned long		secondBound;
#endif HAS_AUX_PROCESSMGR

	base = (unsigned long) pProc->p_zone;
	bound = base + pProc->p_size;
#ifdef HAS_AUX_PROCESSMGR
	secondBound = (AUXIsPresent) ? PHYSMEMTOP : (0xFFFFFFFF);
#endif HAS_AUX_PROCESSMGR
	
	pNextVBL = VBLQHDR->qHead;
	while (pNextVBL != nil)
		{
		pThisVBL = pNextVBL;
		pNextVBL = pNextVBL->qLink;
		vblAddr = StripAddress((Ptr) pThisVBL->vblAddr);
		if (((((unsigned long) pThisVBL) >= base) && (((unsigned long) pThisVBL) <= bound)) ||
			((vblAddr >= base) && (vblAddr <= bound  
#ifdef HAS_AUX_PROCESSMGR
			|| ( AUXIsPresent && vblAddr > secondBound)
#endif
			)))
				(void) VRemove(pThisVBL);
		}
	}

/* save_dce.  Switch out all drivers/DAs associated the specified process.
 * This should be called before save_or_restore_dispatch and save_lomem.
 */
void
save_dce(PEntryPtr pp)
	{
	DCtlHandle		*h;
	u_short			i, n, daNumber;
	u_long			crit, base, bound;
	DCEDescPtr		del;

	assert(pp->p_pcb != nil);
	daNumber = (*(pp->p_pcb))->p_daResource;
	base = (unsigned long) pp->p_zone;
	bound = base + pp->p_size;
		
	/* tally up the DAs/drivers we'll be switching */
	h = UNITTABLE;
	i=0; n=0;
	while (n < UNITNTRYCNT)
		{
		if (*h != nil)
			{
			/* Is it a DA handler one? */
			if (n == daNumber)
				i++;
			
			/* swap local drivers */
			else
				{
				if ( (((crit = (unsigned long) (**h)->dCtlWindow) == 0)
						&& ((crit = (unsigned long) (**h)->dCtlStorage) == 0)) )
					crit = (unsigned long) (**h)->dCtlDriver;
				crit = (u_long) StripAddress((Ptr) crit);
				
				if ( (crit >= base) && (crit < bound) )
					i++;
				}
			}
		h++;
		n++;
	}

	/* Make handle correct size (back to 0 if drivers all went away) */
	SetHandleSize((*pp->p_pcb)->dces, i*sizeof(DCEDesc));

	/* No use going on */
	if (i == 0)
		return;
	
	/*  Do the save now that we have storage for it. */
	del = *(*pp->p_pcb)->dces;
	n = 0;
	h = UNITTABLE;
	while (n < UNITNTRYCNT)
		{
		Boolean		removeIt;
		
		if (*h != nil)
			{
			/* Check whether current DCE is local driver or DA from DA Handler */
			if ((removeIt = (n == daNumber)) == false)
				{
				if ( ((crit = (unsigned long) (**h)->dCtlWindow) == 0)
						&& ((crit = (unsigned long) (**h)->dCtlStorage) == 0) )
					crit = (unsigned long) (**h)->dCtlDriver;
				crit = (u_long) StripAddress((Ptr) crit);
				removeIt = ((crit >= base) && (crit < bound));
				}
			
			/* Act on our findings */
			if (removeIt)
				{
				assert(i-- != 0);
				del->d_unit = n;
				del++->d_handle = *h;
				*h = nil;
				}
			}
		h++;
		n++;
		}
	}

/* restore_dce.  Replace all the DCEs we switched out in save_dce. */
void
restore_dce(PEntryPtr pProc)
	{
	register DCEDescHdl		hDCEDesc;
	register DCEDescPtr		pDCEDesc;
	short					dceDescCount;

	assert(pProc->p_pcb != nil);

	hDCEDesc = (*pProc->p_pcb)->dces;
	pDCEDesc = *hDCEDesc;
	dceDescCount = GetHandleSize(hDCEDesc) / sizeof(DCEDesc);
	while (--dceDescCount >= 0)
		{
		*(UNITTABLE + pDCEDesc->d_unit) = pDCEDesc->d_handle;
		pDCEDesc++;
		}
	}

/* kill_dce.  Fix up the unit table when the given appl zone is nuked.
 * This contains the basic functionality of the ROM routine InstallRDrivers().
 * Zeroes out the DCE except for dCtlRefNum (restored because _DrvrInstall won't be
 * called again on an allocated DCE).  Also, gives appropriate GBKisses to drivers.
 * NOTE: A known bug is that a system DA opened locally with no window or storage but
 * a menu id will not be cleaned up or get a gb kiss.
 * NOTE: There should be no need to StripAddress handles, only the MPs.
 * NOTE: It is somewhat dangerous to not _ReleaseResource on the drivers if there are
 * misbehaved drivers (such as the "Windows" DA) that store data in their 'DRVR' handles
 * and depend on the data being reinitialized from disk since they assume they are app
 * heap resource handles and thus will be removed from memory by _RsrcZoneInit.
 */
void
kill_dce(THz applZone)
	{
	u_long				tmp;
	Ptr					baseCurAppHeap, boundsCurAppHeap;
	DCtlHandle			*unitEntry;
	short				unitNTryCnt;
	DCtlPtr				dCtlPtr;
	short				dRefNum;
	
	baseCurAppHeap = (Ptr)applZone;
	boundsCurAppHeap = ((THz) baseCurAppHeap)->bkLim;
	unitEntry = UNITTABLE;
	dRefNum = FIRSTDREFNUM+1;
	unitNTryCnt = UNITNTRYCNT;
	
	while (--unitNTryCnt >= 0)
		{
		/* Keep dRefNum and unitNTryCnt in synch */
		dRefNum -= 1;
		
		/* Get table entry (handle to driver) */
		if ((tmp = (u_long) *unitEntry++) == nil)
			continue;
		
		/* Dereference it to get pointer to driver, can do nothing if it was purged */
		if ((tmp = (u_long) *((DCtlHandle) tmp)) == nil)
			continue;

		/* Convert type just once */
		dCtlPtr = (DCtlPtr) tmp;

		/* Can do nothing if driver isn't available. */
		if ((tmp = (u_long) StripAddress((Ptr) dCtlPtr->dCtlDriver)) == nil)
			continue;
		
		/* Look for driver based in application heap.  Wipe it out! */
		if (tmp > baseCurAppHeap && tmp < boundsCurAppHeap)
			{
			GiveGoodByeKiss(dCtlPtr);						
			MemClear(dCtlPtr,sizeof(DCtlEntry));
			dCtlPtr->dCtlRefNum = dRefNum;
			continue;
			}
		
		/* Look for driver based in system heap who has storage in the application heap. */
		if (tmp < (u_long)SYSZONE->bkLim)
			{
			/* Is the storage handle in current application heap? */
			tmp = (u_long) StripAddress((Ptr) dCtlPtr->dCtlStorage);
			if (tmp > baseCurAppHeap && tmp < boundsCurAppHeap)
				{
				OSErr				resErr;
				Str255				driverNameString;
				
				/* Get driver name now, since goodbye kiss may prompt the driver
				 * to nuke dCtlPtr->dCtlDriver.
				 */
				GetResInfo(dCtlPtr->dCtlDriver, nil, nil, &driverNameString);
				resErr = RESERR;
				
				/* Smooch! */
				GiveGoodByeKiss(dCtlPtr);
				
				/* Look for DA vs regular driver.  If it's a DA, wipe it out altogether.
				 * If a driver, just nil the handle into the application heap (driver
				 * can later simple restore it before re-using).  Do any drivers take
				 * advantage of this?
				 * NOTE:  This assumes that all non-DA drivers start with a '.'.
				 */
				if ((resErr != noErr) ||
					((Length(&driverNameString) != 0) && (StringByte(&driverNameString, 0) == '\0')))
					{
					HPurge(dCtlPtr->dCtlDriver);
					MemClear(dCtlPtr, sizeof(DCtlEntry));
					dCtlPtr->dCtlRefNum = dRefNum;
					}
				else
					dCtlPtr->dCtlStorage = nil;
				continue;
				}
			}
			
		/* At this point we have a valid driver w/ last tries for GBKiss if it has
		 * a window in the current heap.  This is done primarily for the benefit of
		 * Calculator, which allocates its storage in the sys heap, but never removes it.
		 */
		tmp = (u_long) StripAddress((Ptr) dCtlPtr->dCtlWindow);
		if (tmp > baseCurAppHeap && tmp < boundsCurAppHeap)
			{
			GiveGoodByeKiss(dCtlPtr);
			HPurge(dCtlPtr->dCtlDriver);
			MemClear(dCtlPtr,sizeof(DCtlEntry));
			dCtlPtr->dCtlRefNum = dRefNum;
			}
		}
	}

/* GiveGoodByeKiss.  Give this driver a goodbye kiss if it wants one. */
void
GiveGoodByeKiss(DCtlPtr dCtlPtr)
	{
	CntrlParam			goodByeParamBlock;
	
	/* Give him a goodbye kiss, if necessary */
	if ((dCtlPtr->dCtlFlags & (DNeedGoodbye | DOpened)) == (DNeedGoodbye | DOpened))
		{
		goodByeParamBlock.csCode = csCodeGoodBye;
		goodByeParamBlock.ioCRefNum = dCtlPtr->dCtlRefNum;
		PBControl(&goodByeParamBlock, SyncHFS);					
		}
	}

/* save_hooks.  Switch out any "hook" that may depend on the application's patches
 * or low memory.  This should be called before save_or_restore_dispatch and
 * save_lomem.
 */
void
save_hooks(PEntryPtr  pProc)
	{
	PCB		*pc;
	
	pc = *pProc->p_pcb;
	pc->fsqueuehook = FSQUEUEHOOK;
	pc->pmsphook = *((Ptr *)(PMSPPTR+PMSPHOOKINDEX));
	kill_hooks();
	}

/* restore_hooks.  Switch in the hooks that may depend on the application's patches
 * or low memory.  This should be called before save_or_restore_dispatch and
 * restore_lomem.
 */
void
restore_hooks(PEntryPtr  pProc)
	{
	PCB		*pc;
	
	pc = *pProc->p_pcb;
	FSQUEUEHOOK = pc->fsqueuehook;
	*((Ptr *)(PMSPPTR+PMSPHOOKINDEX)) = pc->pmsphook;
	}

/* kill_hooks.  Restore hooks to their neutral values. */
void
kill_hooks(void)
	{
	FSQUEUEHOOK = (void (**)())initFSQueueHook;
	*((Ptr *)(PMSPPTR+PMSPHOOKINDEX)) = nil;
	}

#pragma segment	INIT

/* GetSwitchTab.  Get table of toolbox switch locations and set up related globals */
void
GetSwitchTab(void)
	{
	short			*pLen, len;
	Handle			switchTabHdl;
	short			lmemID;
	
	/* Which table depends on whether the machine has color */
	lmemID = (Colorized) ? COLOR_LOMEMTAB_ID : BW_LOMEMTAB_ID;
	if ((switchTabHdl = GetResource(LOMEM_TAB_TYPE, lmemID)) == nil)
		{
		assert(RESERR == memFullErr);
		SysError(dsMemFullErr);
		}
	
	/* Remember table in globals (resource must be marked resLocked) */
	switchTabPtr = *switchTabHdl;
	
	/* NOTE:  This assumes only data addresses in the table */
	for (pLen = switchTabPtr, lmemToolDataSize = 0; (len = *pLen) != 0; (Ptr)pLen += 6)
		lmemToolDataSize += len;
	}
#pragma segment	kernel_segment
