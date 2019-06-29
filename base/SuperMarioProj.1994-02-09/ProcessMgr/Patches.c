/*
	File:		Patches.c

	Contains:	Routines which monitor/maintain/switch patches made by applications.

	Written by:	Phil Goldman

	Copyright:	© 1986-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <4>	 1/14/91	DFH		(JDR) Fixed latent bug in c_settrapaddress that mishandled
									SetTrapWordAddress of am OSTrap.
		 <3>	 1/10/91	DFH		(JDR) Add hack to prevent HyperCard 1.2.5 patch to SysBeep.
		 <0>	10/26/86	PYG		New Today.

*/ 
 
#include <errors.h>
#include <memory.h>
#include <menus.h>
#include <osutils.h>
#include <retrace.h>
#include <segload.h>
#include <traps.h>
#include <types.h>
#include <MFPrivate.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Patches.h"

#define OS_TRAPTABLE_MASK					((short) (0x0FF))
#define TB_TRAPTABLE_MASK					((short) (0x3FF))
#define HIGHEST_CONTIG_MACPLUS_OSTRAPNUM	((short) (0x04F))

#define TRAPNUM_NEWOSSETTRAPADDRESS			(_SetTrapAddress + (1<<9))
#define TRAPNUM_NEWTOOLSETTRAPADDRESS		(_SetTrapAddress + (1<<9) + (1<<10))
#define TRAPNUM_SETTRAPWORDADDRESS			(_SetTrapAddress + (1<<10))
#define SETTRAPWORDADDRESS_TYPE_MASK		(0x0800)

#define NullTrapType				3
#define PATCH_TABLE_COUNT_INIT		10

Ptr						blockTrapAddr;

/* c_GetProcessTrapAddress.  Implements GetProcessTrapAddress selector of OSDispatch. */
pascal OSErr
c_GetProcessTrapAddress(ProcessSerialNumberPtr pPSN, Ptr *pTrapAddr, TrapType trapType, unsigned short trapNum)
	{
	register PEntryPtr	pProc;
	PatchEntryHdl		hPatch;
	PatchEntryPtr		pPatch;
	unsigned long		olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	if ((pProc = PEntryFromPSN(pPSN)) == nil)
		{
		A5SimpleRestore(olda5);
		return(procNotFound);
		}

	/* If target is current process, a simple NGetTrapAddress will do */
	if (pProc == pCurrentProcess)
		{
		*pTrapAddr = (Ptr) NGetTrapAddress(trapNum, trapType);
		A5SimpleRestore(olda5);
		return(noErr);
		}
	
	/* Otherwise, we must look through the process' patch registry */
	hPatch = (*pProc->p_pcb)->applPatchTable;
	trapNum &= (trapType == ToolTrap) ? TB_TRAPTABLE_MASK : OS_TRAPTABLE_MASK;
	*pTrapAddr = 
		((pPatch = GetApplPatch(hPatch, trapNum, (unsigned short) trapType)) != nil)
			? (Ptr) pPatch->newtrap
			: GetRealTrapAddr(trapNum, trapType);

	A5SimpleRestore(olda5);
	return(noErr);
	}

/* c_SetProcessTrapAddress.  Implements SetProcessTrapAddress selector of OSDispatch. */
pascal OSErr
c_SetProcessTrapAddress(ProcessSerialNumberPtr pPSN, Ptr trapAddr, TrapType trapType, unsigned short trapNum)
	{
	PEntryPtr			pProc;
	unsigned long		olda5;
			
	olda5 = ProcessMgrA5SimpleSetup();
	if ((pProc = PEntryFromPSN(pPSN)) == nil)
		{
		A5SimpleRestore(olda5);
		return(procNotFound);
		}
	
	trapNum &= (trapType == ToolTrap) ? TB_TRAPTABLE_MASK : OS_TRAPTABLE_MASK;
	UpdatePatchRegistry(pProc, trapAddr, trapType, trapNum);
	if (pProc == pCurrentProcess)
		settrapaddress_glue(trapAddr, trapNum, _SetTrapAddress, patchtraps[SETTRAPADDRESS].oldtrap);

	A5SimpleRestore(olda5);
	return(noErr);
	}

/* c_settrapaddress.  Patch to SetTrapAddress to record patches made by applications.
 * App patches get a) removed on minor switch out or app death, b) re-implanted on
 * minor switch in.
 *
 * Although SetTrapAddress is one trap, there are three variants based on bits 9 & 10
 * of the trap word.  Both off is the original SetTrapAddress, which is mostly out of
 * date since the trap explosion in the Mac II resulted in an overlap between OS and
 * toolbox trap numbers.  The newer SetTrapAddresses cope by having the caller specify
 * in the trap word (i.e. bits 9 & 10) whether the trap is an OS trap or a toolbox trap.
 * Both on means ToolTrap.  Bit 9 on and bit 10 off means OSTrap.  Our table records
 * both the trap number and trap type, so we can make the correct calls later on.  The
 * trap number is masked to the significant part so we can compare them.
 * NOTE: This contains a hack to prevent HyperCard 1.2.5 from patching SysBeep.  Said
 * patch breaks horribly under System 7.0 and later.  We go to pains to prevent just
 * the first patch attempt, because XCMDs and sub-pieces may have their own patches.
 */
void
c_settrapaddress(Ptr trapAddr, unsigned short trapNum, short staTrapWord)
	{
	register unsigned short		trapType, trapNumCopy;
	unsigned long				olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	
	/* Assert this since we assume that masking to OS_TRAPTABLE_MASK after masking to
	 * TB_TRAPTABLE_MASK will not lose any bits from the OS_TRAPTABLE_MASK.
	 */
	assert((TB_TRAPTABLE_MASK & OS_TRAPTABLE_MASK) == OS_TRAPTABLE_MASK);
	
	/* Assume TB until proven OS */
	trapType = ToolTrap;
	trapNumCopy = trapNum;
	trapNum &= TB_TRAPTABLE_MASK;
	
	/* Check whether trap is OSTrap.  Inhibit HyperCard 1.2.5 SysBeep patch. */
	if ( (staTrapWord == (short) TRAPNUM_NEWOSSETTRAPADDRESS) ||
		((staTrapWord == (short) TRAPNUM_SETTRAPWORDADDRESS) && ((trapNumCopy & SETTRAPWORDADDRESS_TYPE_MASK) == 0)) ||
		((staTrapWord == (short) _SetTrapAddress) && 
			((trapNum <= HIGHEST_CONTIG_MACPLUS_OSTRAPNUM) ||
			(trapNum == (_UprString & TB_TRAPTABLE_MASK)) ||
			(trapNum == (_SetApplBase & TB_TRAPTABLE_MASK)))) )
		{
		trapType = OSTrap;
		trapNum &= OS_TRAPTABLE_MASK;
		}
	else if ( (trapNum == (_SysBeep & TB_TRAPTABLE_MASK)) &&
			(pCurrentProcess->p_preventFirstSysBeepPatch) )
		{
		pCurrentProcess->p_preventFirstSysBeepPatch = false;
		A5SimpleRestore(olda5);
		return;
		}
	
	/* Record it.
	 * NOTE: Gee, great… if pCurrentProcess is nil (like during switch_task) the trap
	 * doesn't get recorded.  This means it becomes the "original" for new apps or any
	 * existing ones that haven't patched it.  Gross.  Hopefully, apps are the only
	 * things calling SetTrapAddress after INIT time.
	 */
	if (pCurrentProcess != nil)
		UpdatePatchRegistry(pCurrentProcess, trapAddr, trapType, trapNum);

	/* Call old routine */
	settrapaddress_glue(trapAddr, trapNum, staTrapWord, patchtraps[SETTRAPADDRESS].oldtrap);

	A5SimpleRestore(olda5);
	}

/* GetApplPatch.  Get ptr to application patch entry for given trapNum, or nil. */
PatchEntryPtr
GetApplPatch(PatchEntryHdl hPatch, register unsigned short trapNum, register unsigned short trapType)
	{
	register PatchEntryPtr		pPatch;
	PatchEntryPtr				pPatchMax;

	/* Get base and bound of table */	
	pPatch = (PatchEntryPtr) StripAddress(*hPatch);
	pPatchMax = ((Ptr) pPatch + GetHandleSize(hPatch));
	
	/* Search for a matching entry */
	while (pPatch < pPatchMax)
		{
		if (pPatch->trapnum == trapNum && pPatch->traptype == trapType)
			return(pPatch);
		pPatch++;
		}
	
	/* exiting loop means no-find */
	return(nil);
	}

/* UpdatePatchRegistry.  Add, remove, or change the patch registry entry for the
 * given patch and process.
 */
void
UpdatePatchRegistry(PEntryPtr pProc, Ptr trapAddr, TrapType trapType, unsigned short trapNum)
	{
	PatchEntryHdl		hPatch;
	PatchEntryPtr		pPatch;
	PCB					**pcbHdl;
	Ptr					currTrapAddr;
#ifdef WRITENOW_FIX
	ProcPtr				SetOverlaySystemEvent(ProcPtr);
#endif WRITENOW_FIX
#ifdef MSWORKS_FIX
	ProcPtr				SetOverlayRecoverHandle(ProcPtr);
#endif MSWORKS_FIX

	/* Get handle to table and size.
	 * NOTE: The extraordinary case of a nil pProc->p_pcb happens if someone patches
	 * while pCurrentProcess == pNullProcess.  Not impossible!  An INIT can patch
	 * a trap we call while launching Finder.  If that patch makes a patch, we're there!
	 */
	if ((pcbHdl = pProc->p_pcb) == nil)
		return;
	hPatch = (*pcbHdl)->applPatchTable;
	
	/* See if app is modifying an existing patch (i.e. whether entry already exists). */
	if ((pPatch = GetApplPatch(hPatch, trapNum, (unsigned short) trapType)) != nil)
		{
		/* Application is unpatching if new address same as its original */
		if (pPatch->oldtrap == trapAddr)
			{
			RemoveApplPatch(pPatch);
			(*pcbHdl)->applPatchCount--;
			}
		
		/* Application is re-adjusting a patch.  Don't touch pPatch->oldtrap! */
		else
			pPatch->newtrap = trapAddr;
		}
		
	/* Application is making a new patch */
	else
		{
#ifdef WRITENOW_FIX
		/* NOTE:  Special hack for WriteNow -- see DeskMgrPatches.c */
		if ( (trapNum == (_SystemEvent & TB_TRAPTABLE_MASK))
			&& trapType == ToolTrap
			&& pCurrentProcess->p_signature == 'nX^n' )
			{
			(ProcPtr)trapAddr = SetOverlaySystemEvent(trapAddr);
			}
#endif WRITENOW_FIX

#ifdef MSWORKS_FIX
		/* NOTE:  Special hack for Works -- see MiscPatches.a and Utilities.c */
		else if ( (trapNum == (_RecoverHandle & OS_TRAPTABLE_MASK))
					&& (trapType == OSTrap)
					&& (pCurrentProcess->p_signature == 'PSIP') )
			{
			(ProcPtr)trapAddr = SetOverlayRecoverHandle(trapAddr);
			}
#endif MSWORKS_FIX

		currTrapAddr = (pProc == pCurrentProcess)
			? (Ptr) NGetTrapAddress(trapNum, trapType) : GetRealTrapAddr(trapNum, trapType);
		AddApplPatch(hPatch, currTrapAddr, trapAddr, trapNum, trapType);
		(*pcbHdl)->applPatchCount++;
		}
	}
	
/* AddApplPatch.  Add trap to application's trap table */
void
AddApplPatch(PatchEntryHdl hPatch, Ptr currTrapAddr, Ptr trapAddr, unsigned short trapNum, unsigned short trapType)
	{
	PatchEntryPtr		pPatch, pPatchMax;
	u_long				trapTabSize, newTrapTabSize;
	
	/* Find size, base, and bound of trap table */
	trapTabSize = GetHandleSize(hPatch);
	pPatch = *hPatch;
	pPatchMax = ((Ptr) pPatch + trapTabSize);
	
	/* Find an empty entry */
	while (pPatch < pPatchMax)
		{
		if (pPatch->traptype == NullTrapType)
			break;
		pPatch++;
		}
		
	/* Did we reach end of table? */
	if (pPatch >= pPatchMax)
		{
		/* If so, grow by one entry.  Reset pPatch since table handle might move. */
		newTrapTabSize = trapTabSize + sizeof(PatchEntry);
		SetHandleSize(hPatch, newTrapTabSize);
		pPatch = (PatchEntryPtr) (((Ptr) *hPatch) + trapTabSize);
		}

	/* Initialize the patch entry. */		
	pPatch->trapnum = trapNum;
	pPatch->traptype = trapType;
	pPatch->oldtrap = currTrapAddr;
	pPatch->newtrap = trapAddr;
	}

/* GetRealTrapAddr.  Get the trap address underneath the application patch level.
 * NOTE: This does not work well if called while we are in the parts of switch_task
 * where pCurrentProcess == nil, but either app's patches are still installed.
 */
Ptr
GetRealTrapAddr(unsigned short trapNum, unsigned short trapType)
	{
	register PatchEntryPtr	pPatch;
	register PatchEntryHdl	hPatch;
	
	pPatch = nil;
	if (pCurrentProcess != nil)
		{
		hPatch = (*pCurrentProcess->p_pcb)->applPatchTable;
		pPatch = GetApplPatch(hPatch, trapNum, (unsigned short) trapType);
		}
		
	return (pPatch != nil) ? (Ptr) pPatch->oldtrap : (Ptr) NGetTrapAddress(trapNum, trapType);
	}

/* save_or_restore_dispatch.  Switch in or out the patches made by the 
 * specified process.  Patches should be switched out only after all the
 * executable objects that need these patches have been switched out.
 * Patches should be switched in before switching in any executable
 * objects that need them.  These executables include the VBLs, drivers,
 * and "hooks".
 */
void
save_or_restore_dispatch(PCB *PCBPtr, int do_restore)
	{
	PatchEntryHdl			hPatch;
	register PatchEntryPtr	pPatch, pPatchMax;

	/* Check if valid */
	if (PCBPtr == nil
		|| PCBPtr->applPatchCount == 0
		|| (hPatch = PCBPtr->applPatchTable) == nil)
			return;

	pPatch = *hPatch;
	pPatchMax = ((Ptr) pPatch + GetHandleSize(hPatch));
	
	while (pPatch < pPatchMax)
		{
		if (pPatch->traptype != NullTrapType)
			{
			/* (Un)patch this trap */
			settrapaddress_glue(
				(do_restore != SAVE_DISPATCH) ? pPatch->newtrap : pPatch->oldtrap,
				pPatch->trapnum,
				(pPatch->traptype ? TRAPNUM_NEWTOOLSETTRAPADDRESS : TRAPNUM_NEWOSSETTRAPADDRESS),
				patchtraps[SETTRAPADDRESS].oldtrap);
			}
		pPatch++;
		}
	}

/* kill_dispatch.  Remove app's patches, and the registry. */
void
kill_dispatch(PCB *PCBPtr)
	{
	if (PCBPtr->applPatchTable != nil)
		{
		save_or_restore_dispatch(PCBPtr, SAVE_DISPATCH);
		PCBPtr->applPatchCount = 0;
		DisposHandle(PCBPtr->applPatchTable);
		PCBPtr->applPatchTable = nil;
		}
	}

/* InitApplPatchTable.  Initialize specified trap table and counter. */
void
InitApplPatchTable(PatchEntryHdl *pApplPatchTab, short *applPatchCountPtr)
	{
	PatchEntryPtr	pPatch, pPatchMax;
	long			size = PATCH_TABLE_COUNT_INIT * sizeof(PatchEntry);
	PatchEntryHdl	hPatch;

	/* Initialize the parameters */
	*applPatchCountPtr = 0;
	*pApplPatchTab = hPatch = NewHandle(size);
	if (hPatch == nil)
		return;
	
	/* Initialize the table */
	pPatch = *hPatch;
	pPatchMax = ((Ptr) pPatch + size);
	while(pPatch < pPatchMax)
		(pPatch++)->traptype = NullTrapType;
	}

#pragma segment INIT

/* InstallPatches. Install the Process Mgr's patches. */
void
InstallPatches(void)
	{
	register PatchEntryPtr		pPatch;
	unsigned long				isNewTrap;
	Ptr							unimplTrapAddr;

	blockTrapAddr = (Ptr) NGetTrapAddress(_BlockMove, OSTrap);
	unimplTrapAddr = (Ptr) NGetTrapAddress(_Unimplemented, ToolTrap);
	
	pPatch = (IS_NUMAC_ROMS(ROM85)) ? &patchtraps[FIRST_ROM78_TRAP] : &patchtraps[FIRST_ROM75_TRAP];
		
	while ( (unsigned long) pPatch->newtrap != 0)
		{
		isNewTrap = (long) pPatch->oldtrap;	/* Marked by -1 in this field */
		pPatch->oldtrap = (void (*)()) NGetTrapAddress(pPatch->trapnum, pPatch->traptype);
		if (isNewTrap || pPatch->oldtrap != unimplTrapAddr)
			{
			NSetTrapAddress((long) ROUTINE_ADDR(pPatch->newtrap), pPatch->trapnum, pPatch->traptype);
			}
		pPatch++;
		}
	
	/* Cache address of CheckLoad */
	SetOldCheckLoad(patchtraps[CHECKLOAD].oldtrap);
	}
	
#pragma segment Main
