/*
	File:		Sleep.c

	Contains:	Routines which facilitate sleeping processes.

	Written by:	Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <5>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		 <4>	 3/16/92	YK		(for TSM) In clkint, If a physical event is not for the front
									app, don’t switch to the front app.
		 <2>	11/27/90	DFH		Removed unneeded parameter from cpu_resched.
		 <0>	 3/13/86	ELR		New Today.

*/ 
 
#include <types.h>
#include <memory.h>
#include <osutils.h>
#include <files.h>
#include <quickdraw.h>
#include <windows.h>
#include <menus.h>
#include <events.h>
#include <resources.h>
#include <retrace.h>
#include <segload.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Puppet.h"

/* Some function prototypes that should be in (yet another) header file */
void		RemoveFromStateList(PEntryPtr, PEntryPtr *);
void		CancelSleep(PEntryPtr);
Boolean		OSEventAvailForFront(short, EventRecord *);

/* Function prototypes internal to this file */
void		RoustSleepers(void);
void		PutOnSleepQueue(PEntryPtr, unsigned long);
void		RoustSleepers(void);

/* PutOnSleepQueue.  Put the specified process on the sleep queue with the designated
 * sleep time.
 */
void
PutOnSleepQueue(PEntryPtr pProc, unsigned long sleepTime)
	{
	register PEntryPtr	pPrev, pNext;	
	unsigned long		thenTicks;

	if (sleepTime == 0)
		return;
		
		/* Figure out time to wakeup.  Pin overflow to highest TICKS value. */
		thenTicks = TICKS + sleepTime;
		if (thenTicks < sleepTime)
			thenTicks = MAXVALUE;
		
		/* reflect process state in PEntry */
		pProc->p_state = PRSLEEPING;
		pProc->p_wakeuptime = thenTicks;
		
		/* Place process in the (ordered) sleep state list */
		pPrev = nil;
		pNext = pSleepingStateList;
		while (pNext != nil)
			{
			unsigned long		tempTicks;

			if ( ((tempTicks = pNext->p_wakeuptime) != 0) && (tempTicks > thenTicks) )
				break;
			pPrev = pNext;
			pNext = pNext->p_NextProcessInState;
			}
		
		/* Have insertion point.  Fix up newcomer's links, then insert. */
		assert((pProc != pPrev) && (pProc != pNext));
		pProc->p_PrevProcessInState = pPrev;
		pProc->p_NextProcessInState = pNext;
		
		if (pPrev == nil)
			pSleepingStateList = pProc;
		else
			pPrev->p_NextProcessInState = pProc;
		
		if (pNext != nil)
			pNext->p_PrevProcessInState = pProc;
	}

/* SleepProcess. put the calling process to sleep. Call resched to arrange for
 * another process to use the processor.
 */
pascal void
c_SleepProcess(u_long napTime)
	{
	PEntryPtr	pCurr;
	u_long		olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	
	/* can't sleep if not running */
	pCurr = pCurrentProcess;
	if ( (pCurr->p_state != PRRUN) &&
		 (pCurr->p_state != PRBACKRUN) )
		{
		A5SimpleRestore(olda5);
		return;
		}
	
	/* can't sleep if coercing */
	if (coercionState != CS_DONE)
		{
		A5SimpleRestore(olda5);
		return;
		}
	
	PutOnSleepQueue(pCurr, napTime);
	
	/* offer up the machine */
	cpu_resched();
	
	A5SimpleRestore(olda5);
	}

/* clkint. This routine simulates processing on a clock interrupt. This is silly,
 * and for now we call this thing from the idle loop.
 */
void
clkint(void)
	{
	EventRecord			evtRec;
	unsigned long		olda5;

	olda5 = ProcessMgrA5SimpleSetup();
	
	/* See if anyone's time is up */
	RoustSleepers();

	/* Get frontmost app going if there's a physical event queued */
	assert(coercionState == CS_DONE);
	if ( (pFrontProcess->p_state == PRSLEEPING) && OSEventAvailForFront(pFrontProcess->p_eventmask, &evtRec) ) 
		CancelSleep(pFrontProcess);
			
	A5Restore(olda5);
	}

/* PushOnStateList.  Links process into head of given state list */
void
PushOnStateList(register PEntryPtr pProc, PEntryPtr *ppList)
	{
	PEntryPtr	pFirstProc;
	
	pFirstProc = *ppList;
	pProc->p_NextProcessInState = pFirstProc;
	pProc->p_PrevProcessInState = nil;
	pFirstProc->p_PrevProcessInState = pProc;
	*ppList = pProc;
	}

/* RemoveFromStateList.  Unlinks process from given state list */
void
RemoveFromStateList(register PEntryPtr pProc, PEntryPtr *ppList)
	{
	PEntryPtr	pOtherProc;
	
	if ((pOtherProc = pProc->p_PrevProcessInState) != nil)
		pOtherProc->p_NextProcessInState = pProc->p_NextProcessInState;
	else
		*ppList = pProc->p_NextProcessInState;
	
	if ((pOtherProc = pProc->p_NextProcessInState) != nil)
		pOtherProc->p_PrevProcessInState = pProc->p_PrevProcessInState;
	
	pProc->p_PrevProcessInState = nil;
	pProc->p_NextProcessInState = nil;
	}

/* RoustSleepers. This fellow is called by the 'clock interrupt'. It dequeues all
 * members that have expired.  The queue is ordered by p_wakeuptime, so normally
 * we go no farther than a PEntry that should not be woken.  The exception to this,
 * of course, is when napOver == true, since that means that there are processes
 * that are to be woken up prematurely (their p_wakeuptime == 0).
 * NOTE: We turn interrupts off to protect this critical region against interference
 * from WakeupProcess calls by interrupt routines.
 */
void
RoustSleepers(void)
	{
	register PEntryPtr	pProc, pNextProc;
	short				ps;
	
	ps = disable();
	pProc = pSleepingStateList;
	while (pProc != nil)
		{
		/* must get this now, since RemoveFromStateList() changes it */
		pNextProc = pProc->p_NextProcessInState;

		/* see if this guy should wake up now */
		if (pProc->p_wakeuptime <= (unsigned long) TICKS)
			{
			RemoveFromStateList(pProc, &pSleepingStateList);
			pProc->p_state = PRREADY;
			}
		else if (napOver == false)
			break;
	
		/* move on */
		pProc = pNextProc;
		}

	napOver = false;
	spl(ps);
	}

/* CancelSleep.  Ensures that pProc gets out of its current event call. */
void
CancelSleep(PEntryPtr pProc)
	{
	/* make sure the process is awake */
	if (pProc->p_state == PRSLEEPING)
		{
		RemoveFromStateList(pProc, &pSleepingStateList);
		pProc->p_state = PRREADY;
		}

	/* make sure the process stays awake.  A zeroed p_wakeuptime means that the app
	 * will not go to sleep on the way out of its current event call.  We must do this
	 * even if app is not asleep to begin with.
	 */
	pProc->p_wakeuptime = 0;
	}
