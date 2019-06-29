/*
	File:		Debugger.c

	Contains:	Routines to assist the registered high-level debugger.

	Written by:	Russ Daniels

	Copyright:	© 1987-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <6>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		 <5>	  2/4/91	DFH		gbm,WhiteBoard:Fixed CoreRegisterDebugger's check for initial
									registration vs re-registration. Kept InitExceptions from being
									called during registration. This check was incorrect even before
									change <4>.
		 <4>	 1/21/91	DFH		(ksm) Fixed CoreRegisterDebugger so that it can be called to
									change attributes.
		 <0>	 7/10/87	RDD		New Today.

*/ 

#include <types.h>
#include <errors.h>
#include <memory.h>
#include <files.h>
#include <quickdraw.h>
#include <windows.h>
#include <menus.h>
#include <events.h>
#include <resources.h>
#include <osutils.h>
#include <retrace.h>
#include <segload.h>
#include <desk.h>
#include <MFPrivate.h>
#include <sysequ.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Puppet.h"

#pragma segment Debugger

/* Some function prototypes that should be in (yet another) header file */
void			InitDebuggerExceptions(void);
void			UnplugDebuggerExceptions(void);

/* CoreRegisterDebugger.  Register process as the current debugger.  Passing kNoProcess
 * unregisters the debugger.
 * NOTE: Assumes a5 == PROCESSMGRGLOBALS.
 */
OSErr 
CoreRegisterDebugger(ProcessSerialNumberPtr pPSN, Ptr pEntryRoutine, Ptr pExitRoutine, Ptr pToAppRoutine, unsigned short newDebugControlKeyCode)
	{
	CodeEntry		*pCodeEntry;
	PEntryPtr		pProc;

	/* Are we are unregistering the current debugger? */
	if (EqualPSNConst(kNoProcess, pPSN))
		{
		/* Don't unregister unless there is one */
		if (pDebugProcess != nil)
			{
			assert(IsProcessEntry(pDebugProcess));
			
			/* Kill of all his debuggees first. */
			pProc = pProcessList;
			while (pProc != nil)
				{
				PEntryPtr		pNextProc;
				
				assert(IsProcessEntry(pProc));
				pNextProc = pProc->p_NextProcess;
				if (pProc->p_state == PRNULL)
					{
					KillProcess(&pProc->p_serialNumber);
					}
				pProc = pNextProc;
				}
				
			UnplugDebuggerExceptions();
			
			/* And get rid of the Debugger code segment, too */
			pCodeEntry = (CodeEntry *)CoreRegisterDebugger;
			UnloadSeg((Ptr)pCodeEntry);
			ReleaseResource(GET_SCOD_HDL_FROM_FNC_PTR(pCodeEntry));
			
			pDebugProcess = nil;
			}
		}

	/* We are registering or changing the debugger attributes */
	else
		{		
		/* locate PEntry associated with given process */
		if ((pProc = PEntryFromPSN(pPSN)) == nil)
			return(procNotFound);
		
		/* if registering, rather than changing, put in the exception handlers */
		if (pDebugProcess == nil)
			InitDebuggerExceptions();
	
		/* set the procptrs in global variables */
		pDebugProcess = pProc;
		debugEntryRoutine = pEntryRoutine;
		debugExitRoutine = pExitRoutine;
		debugToAppRoutine = pToAppRoutine;
		debugControlKeyCode = newDebugControlKeyCode;
		}

	return noErr;
	}

/* swap_state.  This is an attenuated copy of switch_task.  It changes all of the mapped
 * memory, but doesn't actually switch to the "new" task.
 */
unsigned short
swap_state(PEntryPtr pInProc, PEntryPtr pOutProc)
	{
	void				save_state(PEntryPtr);
	unsigned short		restore_state(PEntryPtr, Boolean);

	/* Don't need to disable here, but don't know valid processor state! */
	if (pInProc == pOutProc) 
		return(disable());
	
	/* save outgoers state, then switch to incomer's */
	save_state(pOutProc);
	return(restore_state(pInProc, false));
	}

/* CoreReadWriteProcessMemory.  To read the contents of memory for a given task, we
 * switch in that task's memory (by calling swap_state), move the memory, then
 * switch the caller's memory back in.  Interrupts are disabled during the process.
 */
OSErr 
CoreReadWriteProcessMemory(ProcessSerialNumberPtr pPSN, Ptr procMemPtr, u_long cBytes, Ptr callerMemPtr, Boolean doRead)
	{
	Ptr					tmpPtr;
	PEntryPtr			pProc;
	unsigned short		ps;

	if ((pProc = PEntryFromPSN(pPSN)) == nil)
		return(procNotFound);

	/* Assume readin' 'till proven it's writin' */
	if (!doRead)
		{
		/* Do the big el swapo */
		tmpPtr = procMemPtr;
		procMemPtr = callerMemPtr;
		callerMemPtr = tmpPtr;
		}
		
	ps = swap_state(pProc, pCurrentProcess);
	
	BlockMove (procMemPtr, callerMemPtr, cBytes);
	
	(void)swap_state(pCurrentProcess, pProc);
	spl(ps);

	return(noErr);
	}

/* CoreReadWriteProcessFPURegs.  To read the or write the Floating Point Unit context
 * for a process, we go straight to where we saved them when we last switched the
 * process out.  Of course, the caller's FPU registers can be sampled directly.
 * NOTE: Extremely dependant on the order in which the registers are saved.
 * NOTE: Assumes a5 == PROCESSMGRGLOBALS
 */
OSErr 
CoreReadWriteProcessFPURegs(ProcessSerialNumberPtr pPSN, Ptr callerMemPtr, Boolean doRead)
	{
	PEntryPtr			pProc;
	AllRegs				*pAllRegs;
	Ptr					srcPtr, dstPtr;

	/* Can do nothing if there is no FPU! */
	if (MachineHasFPU == false)
		return(hardwareConfigErr);

	/* Locate the victim */
	if ((pProc = PEntryFromPSN(pPSN)) == nil)
		return(procNotFound);

	if (pProc == pCurrentProcess)
		{
		if (doRead)
			GetLiveFPURegs(callerMemPtr);
		else
			SetLiveFPURegs(callerMemPtr);
		}
	else
		{
		pAllRegs = (AllRegs *)pProc->p_sp;

		/* If not a full FPU context, create one by munging the stack to give the
		 * values an FRESTORE would. 
		 */
		if (!pAllRegs->fpuFrame.isFullFPUFrame)
			{
			BlockMove(pAllRegs, (Ptr)pAllRegs - sizeof(FPUFrame), sizeof(SaveRegs));
			pProc->p_sp = (Ptr)pAllRegs -= sizeof(FPUFrame);		/* Lower the stack */
			MemClear(&pAllRegs->fpuFrame, sizeof(FPUFrame));		/* Do whole frame for alignment speed, hopefully */
			pAllRegs->fpuFrame.isFullFPUFrame = true;
			}
		
		/* Move data the right direction */
		srcPtr = (doRead) ? &pAllRegs->fpuFrame.fpuUserRegs : callerMemPtr;
		dstPtr = (doRead) ? callerMemPtr : &pAllRegs->fpuFrame.fpuUserRegs;
		BlockMove(srcPtr, dstPtr, sizeof(FPUUserRegs));
		}
		
	return(noErr);
}
	
#pragma segment	kernel_segment

/* c_RegisterDebugger. Verify caller, then call core routine in Debugger segment.
 * NOTE: Assumes THEZONE != ProcessMgrZone, in that it restores THEZONE unconditionally,
 * instead of using SafeSetZone and SafeRestoreZone.
 */
pascal OSErr 
c_RegisterDebugger(ProcessSerialNumberPtr pPSN, Ptr pEntryRoutine, Ptr pExitRoutine, Ptr pToAppRoutine, unsigned short newDebugControlKeyCode)
	{
	short			appCurMap;
	THz				appTheZone;
	
	unsigned long	olda5;
	short			retval;
	CodeEntry		*pCodeEntry;
	
	olda5 = ProcessMgrA5SimpleSetup();
	assert(THEZONE != ProcessMgrZone);
	appCurMap = CURMAP; CURMAP = SYSMAP;
	appTheZone = THEZONE; THEZONE = SYSZONE;
	
	pCodeEntry = CoreRegisterDebugger;
	if (CODE_ENTRY_UNLOADED(pCodeEntry)
		&& GET_SCOD_HDL_FROM_FNC_PTR(pCodeEntry) == nil)
		{
		if ((retval = RESERR) == noErr)
			retval = resNotFound;
		}
	else
		retval = CoreRegisterDebugger(pPSN, pEntryRoutine, pExitRoutine, pToAppRoutine, newDebugControlKeyCode);

	THEZONE = appTheZone;
	CURMAP = appCurMap;
	A5SimpleRestore(olda5);

	return retval;
	}

/* c_ReadWriteProcessMemory. Verify caller, then call core routine in Debugger segment. */
pascal OSErr 
c_ReadWriteProcessMemory(ProcessSerialNumberPtr pPSN, Ptr procMemPtr, unsigned long cBytes, Ptr callerMemPtr, Boolean doRead)
	{
	OSErr				retval;
	PEntryPtr			pProc;
	unsigned long		olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	retval = noErr;

	/* Pay special favor to TMON.  Mr. Horwat needs to be able get the topmaphandle
	 * of other processes in a reentrant way (since, unlike SADE, he'd like to be
	 * able to step through context switching code).
	 * NOTE: This should be taken care of by better debugger support in general.
	 */
	if (doRead && (cBytes == (-1)) && (StripAddress(procMemPtr) == TopMapHndl))
		{
		if ((pProc = PEntryFromPSN(pPSN)) == nil)
			retval = procNotFound;
		else
			BlockMove(&((*pProc->p_pcb)->topmaphandle), callerMemPtr, sizeof(Handle));

		A5SimpleRestore(olda5);
		return(retval);
		}
	
	/* Require that the caller be preregistered.  Also guarantees that the Debugger
	 * segment is loaded.
	 */
	if (pCurrentProcess == pDebugProcess)
		CoreReadWriteProcessMemory(pPSN, procMemPtr, cBytes, callerMemPtr, doRead);
	else
		retval = protocolErr;
	
	A5SimpleRestore(olda5);
	return(retval);
	}

/* c_ReadWriteProcessFPURegs. Verify caller, then call core routine in Debugger segment. */
pascal OSErr 
c_ReadWriteProcessFPURegs(ProcessSerialNumberPtr pPSN, Ptr callerMemPtr, Boolean doRead)
	{
	OSErr				retval;
	unsigned long		olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	
	/* Require that the caller be preregistered.  Also guarantees that the Debugger
	 * segment is loaded.
	 */
	retval = (pCurrentProcess == pDebugProcess)
		? CoreReadWriteProcessFPURegs(pPSN, callerMemPtr, doRead)
		: protocolErr;
	
	A5SimpleRestore(olda5);
	return(retval);
	}
