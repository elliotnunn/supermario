/*
	File:		Error.c

	Contains:	Process error recovery routines.

	Written by:	Erich Ringewald and his bestest buddy Philip York Goldman

	Copyright:	© 1987-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <5>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
		 <3>	  1/8/91	DFH		(rdd) Changed CallDebugger to do just a minor switch to
									pDebugProcess. Moved aeDebuggerNotify sending here from
									BringProcessToFront(). Exception vectors now get stuffed with
									the real subroutine address, rather than the address in the jump
									table.
		 <0>	 1/23/87	PYG		New Today.

*/ 
 
#include <types.h>
#include <memory.h>
#include <menus.h>
#include <osutils.h>
#include <errors.h>
#include <retrace.h>
#include <segload.h>
#include <AppleEventsInternal.h>
#include <MFPrivate.h>

#include "Glue.h"
#include "SysMisc.h"
#include "Lomem.h"
#include "Data.h"
#include "AppleEventExtensions.h"

/* Function prototypes that should be in (yet another) header file */
Boolean			IsNonFatalSysErr(short);
unsigned short	ApplicationMenuItemFromPEntry(PEntryPtr);

void	buserr(void), addrerr(void), ilglerr(void), zeroerr(void), traceerr(void), trap5err(void);
void	trap6err(void), line1111err(void), chkerr(void), trapverr(void), priverr(void), unass(void);
void	coprocerr(void), uniniterr(void), spurint(void);

/* MacsBug 5.5 and earlier...they grab the exception vectors */
#define IsOldDebugger()		(XCEPTIONVECTORS[ID_LINE1010] < ROMBASE)

#pragma segment INIT
void
InitExceptions(void)
/* Set up the low memory exception vectors that we wish to trap.
 * NOTE:  This MUST be called only after the Process Mgr traps are patched, but as soon
 * after as possible.
 */
	{
	register Ptr	*biasedOldExceptionVectors;
	
#ifdef VBR_BIAS
	Ptr				*xceptionVectors = XCEPTIONVECTORS;
	
	/* Add in vector base register (VBR) on '020 and above */
	if (CPUFLAG >= 2)
		(Ptr)xceptionVectors += GetVBR();
#endif VBR_BIAS
	
	/* Need to fall thru to old vectors if old debugger exists */
	IsOldDebugger = IsOldDebugger();

	biasedOldExceptionVectors = oldExceptionVectors - 1;
	biasedOldExceptionVectors[ID_BUSERR] = *(XCEPTIONVECTORS + ID_BUSERR);			/* Save old bus error exception vector */
	biasedOldExceptionVectors[ID_ADDRERR] = *(XCEPTIONVECTORS + ID_ADDRERR);		/* Save old address error exception vector */
	biasedOldExceptionVectors[ID_ILGLERR] = *(XCEPTIONVECTORS + ID_ILGLERR);		/* Save old illegal error exception vector */
	biasedOldExceptionVectors[ID_ZEROERR] = *(XCEPTIONVECTORS + ID_ZEROERR);		/* Save old zero divide error exception vector */
		
	*(XCEPTIONVECTORS + ID_BUSERR) = ROUTINE_ADDR(buserr);							/* Set bus error exception vector */
	*(XCEPTIONVECTORS + ID_ADDRERR) = ROUTINE_ADDR(addrerr);						/* Set address error exception vector */
	*(XCEPTIONVECTORS + ID_ILGLERR) = ROUTINE_ADDR(ilglerr);						/* Set illegal error exception vector */
	*(XCEPTIONVECTORS + ID_ZEROERR) = ROUTINE_ADDR(zeroerr);						/* Set zero divide error exception vector */
	}
	
#pragma segment Debugger

void
InitDebuggerExceptions(void)
	{
	register Ptr	*biasedOldExceptionVectors;

	biasedOldExceptionVectors = oldExceptionVectors - 1;
	biasedOldExceptionVectors[ID_TRACE] = *(XCEPTIONVECTORS + ID_TRACE);	 		/* Save old trace error exception vector */
	biasedOldExceptionVectors[ID_TRAP5] = *(XCEPTIONVECTORS + ID_TRAP5);			/* Save old trap5 error exception vector */
	biasedOldExceptionVectors[ID_TRAP6] = *(XCEPTIONVECTORS + ID_TRAP6);	 		/* Save old trap6 error exception vector */
	biasedOldExceptionVectors[ID_LINE1111] = *(XCEPTIONVECTORS + ID_LINE1111);	 	/* Save old FTrap error exception vector */
	biasedOldExceptionVectors[ID_CHKERROR] = *(XCEPTIONVECTORS + ID_CHKERROR);	 	/* Save old FTrap error exception vector */
	biasedOldExceptionVectors[ID_TRAPVERR] = *(XCEPTIONVECTORS + ID_TRAPVERR);		/* Save old FTrap error exception vector */
	biasedOldExceptionVectors[ID_PRIVILEG] = *(XCEPTIONVECTORS + ID_PRIVILEG);	 	/* Save old FTrap error exception vector */
	biasedOldExceptionVectors[ID_MISCERR] = *(XCEPTIONVECTORS + ID_MISCERR);	 	/* Save old FTrap error exception vector */
	biasedOldExceptionVectors[ID_COPRERR] = *(XCEPTIONVECTORS + ID_COPRERR);		/* Save old FTrap error exception vector */
	biasedOldExceptionVectors[ID_UNINITINT] = *(XCEPTIONVECTORS + ID_UNINITINT);	/* Save old FTrap error exception vector */
	biasedOldExceptionVectors[ID_SPURINT] = *(XCEPTIONVECTORS + ID_SPURINT);	 	/* Save old FTrap error exception vector */

	*(XCEPTIONVECTORS + ID_TRACE) = ROUTINE_ADDR(traceerr);							/* Set trap 5 error exception vector */
	*(XCEPTIONVECTORS + ID_TRAP5) = ROUTINE_ADDR(trap5err);							/* Set trap 5 error exception vector */
	*(XCEPTIONVECTORS + ID_TRAP6) = ROUTINE_ADDR(trap6err);							/* Set trap 6 error exception vector */
	*(XCEPTIONVECTORS + ID_LINE1111) = ROUTINE_ADDR(line1111err);					/* Set FTrap error exception vector */
	*(XCEPTIONVECTORS + ID_CHKERROR) = ROUTINE_ADDR(chkerr);						/* Set chk error exception vector */
	*(XCEPTIONVECTORS + ID_TRAPVERR) = ROUTINE_ADDR(trapverr);						/* Set trapV error exception vector */
	*(XCEPTIONVECTORS + ID_PRIVILEG) = ROUTINE_ADDR(priverr);						/* Set privilege error exception vector */
	*(XCEPTIONVECTORS + ID_MISCERR) = ROUTINE_ADDR(unass);							/* Set unassigned error exception vector */
	*(XCEPTIONVECTORS + ID_COPRERR) = ROUTINE_ADDR(coprocerr);						/* Set coprocessor error exception vector */
	*(XCEPTIONVECTORS + ID_UNINITINT) = ROUTINE_ADDR(uniniterr);					/* Set unitialized error exception vector */
	*(XCEPTIONVECTORS + ID_SPURINT) = ROUTINE_ADDR(spurint);						/* Set spurious error exception vector */
	}

void	
UnplugDebuggerExceptions(void)
	{
	register Ptr	*biasedOldExceptionVectors = oldExceptionVectors - 1;

	*(XCEPTIONVECTORS + ID_TRACE) = biasedOldExceptionVectors[ID_TRACE];			/* Restore trap 5 error exception vector */
	*(XCEPTIONVECTORS + ID_TRAP5) = biasedOldExceptionVectors[ID_TRAP5];			/* Restore trap 5 error exception vector */
	*(XCEPTIONVECTORS + ID_TRAP6) = biasedOldExceptionVectors[ID_TRAP6];			/* Restore trap 6 error exception vector */
	*(XCEPTIONVECTORS + ID_LINE1111) = biasedOldExceptionVectors[ID_LINE1111];		/* Restore FTrap error exception vector */
	*(XCEPTIONVECTORS + ID_CHKERROR) = biasedOldExceptionVectors[ID_CHKERROR];		/* Restore chk error exception vector */
	*(XCEPTIONVECTORS + ID_TRAPVERR) = biasedOldExceptionVectors[ID_TRAPVERR];		/* Restore trapV error exception vector */
	*(XCEPTIONVECTORS + ID_PRIVILEG) = biasedOldExceptionVectors[ID_PRIVILEG];		/* Restore privilege error exception vector */
	*(XCEPTIONVECTORS + ID_MISCERR) = biasedOldExceptionVectors[ID_MISCERR];		/* Restore unassigned error exception vector */
	*(XCEPTIONVECTORS + ID_COPRERR) = biasedOldExceptionVectors[ID_COPRERR];		/* Restore coprocessor error exception vector */
	*(XCEPTIONVECTORS + ID_UNINITINT) = biasedOldExceptionVectors[ID_UNINITINT];	/* Restore unitialized error exception vector */
	*(XCEPTIONVECTORS + ID_SPURINT) = biasedOldExceptionVectors[ID_SPURINT];		/* Restore spurious error exception vector */
	}
	
#pragma segment Main

/* IsMacJmpDebugger.  Return whether a system debugger is currently installed. */
Boolean
IsMacJmpDebugger(void)
	{
	return (MACJMPFLAG != NotImplemented) ? (*((char *)&MACJMPFLAG) & DEBUGGER_INITIALIZED) : (*((char *)&MACJMP) & DEBUGGER_INITIALIZED);
	}
	
/* SysErrRecover.  Handle System errors, by either destroying the current process, or
 * by calling the old (single-tasking) error handler, depending on how deep we guess we
 * are in the Process Mgr routines.  This routine is called either via the low memory
 * exception vectors, or by a call to _SysError.
 * NOTE: Some of this logic should also be in c_exittoshell(), since the user may drop
 * into MacsBug and do an ES from there(?).
 * NOTE:  This routine has been changed to return the old _SysErr routine addr, rather
 * than calling it.  This is necessary because there is currently a system patch to
 * _SysErr to fix a bug in the menu mgr.  This old routine has a case where it need to
 * read/write the stack and assumes the stack (and the registers as well) is unchanged
 * from the point at which _MenuKey called _SysErr.  Therefore, we must return to the
 * assembly routine, which will jump to the old routine.
 */
Ptr
SysErrRecover(short  errID)
	{
	u_long		olda5;
	Ptr			retval = nil;
	
	/* Try to use the graphics world of the CURRENTA5.  If _InitGraf has not been
	 * called, use whatever graphics world PROCESSMGRGLOBALS happens to have.
	 */
	if (QDEXIST == (char) 0)
		{
		olda5 = CurrentA5SimpleSetup();
		(void) ProcessMgrA5Setup();
		}
	else
		olda5 = ProcessMgrA5SimpleSetup();
	
	/* Two choices: return the old trap, or quit the app right now.  The old trap
	 * should be called if a) there is a system debugger (like MacsBug) installed,
	 * b) the error is not catastrophic, c) the error occurred with no distinct process
	 * in (e.g. during boot or a switch_task).  Otherwise, it should be safe and
	 * effective to just force the current process to quit.
	 * NOTE: Have to call through for old menu mgr errors so that sys patch to SysError
	 * can either handle them (popups for MacII) or at least change them to the correct
	 * (i.e. positive ID) errors by recalling _SysError.
	 */
	if ( (IsMacJmpDebugger()) || IsNonFatalSysErr(errID) ||
		(pCurrentProcess == nil) || (pCurrentProcess == pNullProcess) )
			{
			retval = patchtraps[SYSERROR].oldtrap;
			}
	else
		{
		DSERRCODE = errID;
		ExitToShell();
		/* It shouldn't ever get past ExitToShell, but we can handle it if does */
		}

	A5Restore(olda5);
	return retval;
	}

#pragma segment	Debugger

/* Message format for aeDebuggerNotify we send when scheduling a debuggee */
typedef struct DebuggerNotifyMsg {
	AETFHeader				messageHeader;
	KeyWord					metaDataMark;
	AETFParameter			processHdr;
	ProcessSerialNumber		process;
} DebuggerNotifyMsg;

/* CallDebugger.  Do a minor switch into the high-level debugger.  Note that
 * this avoids any of the normal work by the outgoing process, since that would
 * involve forbidden execution time.
 */
void
CallDebugger(void)
	{
	register PEntryPtr		pDBProc;
	OSErr					result;
	HighLevelEventRecord	theEvent;
	DebuggerNotifyMsg		msgStorage;

	pDBProc = pDebugProcess;
	assert (pDBProc != nil);

	/* Bring new front process out of its slumber so it can be running */
	CancelSleep(pDBProc);

	/* Send aeDebuggerNotify AppleEvent to outgoer's debugger */
	SetAppleEvent(aeDebuggerNotify, &theEvent);	
	GenericAEMessage(&msgStorage.messageHeader);
	msgStorage.metaDataMark = aeEndOfMetaDataKeyword;
	SetAEParmHeader(&msgStorage.processHdr, aeProcessKeyword, aeProcessParamType, sizeof(ProcessSerialNumber));
	msgStorage.process = pCurrentProcess->p_serialNumber;
	
	/* Post the event */
	(void) BeginSystemMode();
	result = PostHighLevelEvent((EventRecord *) &theEvent, (u_long) &pDBProc->p_serialNumber, 0, &msgStorage, sizeof(DebuggerNotifyMsg), receiverIDisPSN);
	(void) EndSystemMode();
	assert(result == noErr);

	/* Disable process list item */
	if (pCurrentProcess->p_layer != nil)
		DisableItem(ApplicationMenuHdl, ApplicationMenuItemFromPEntry(pCurrentProcess));
	pCurrentProcess->p_state = PRNULL;
	coercionState = CS_DONE;

	pDBProc->p_state = (pDBProc == pFrontProcess) ? PRRUN : PRBACKRUN;
	switch_task(pDBProc);
	}
