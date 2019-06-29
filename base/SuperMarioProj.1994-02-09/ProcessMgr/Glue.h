/*
	File:		Glue.h

	Contains:	Assembly glue header for Process Manager.

	Written by:	Phil Goldman

	Copyright:	© 1988-1991  by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: System

	Change History (most recent first):
	 
		 <3>	 7/26/91	stb		remove glue for HOpenResFile. Inline is used now when
									SystemSevenOrLater is defined.
		 <0>	 5/20/88	PYG		New Today.

*/ 

#ifndef __GLUE__
#define __GLUE__

#ifndef __FILES__
#include 	"Files.h"
#endif

/* Symbols for values of the asynch parameter of HFS calls */
#define AsyncHFS		true
#define SyncHFS			false

/* First come the routines that will change the a5 register to either the
 * Process Mgr value or the CURRENTA5 value, and back again.  ProcessMgrA5Setup and
 * CurrentA5Setup also copy the long word that the outgoing a5 points to.  This
 * long word is a pointer to the QuickDraw global data.  This gives the incoming
 * a5 world some decent graphics information, and is required if we intend to use
 * QuickDraw or its clients while we have the new a5 set up.
 */

/* ProcessMgrA5Setup. Set up PROCESSMGRGLOBALS and copy in the graphics global pointer */
unsigned long
ProcessMgrA5Setup(void)
	= {0x200D, 0x2215, 0x2A78, 0x0B7C, 0x2a81};
/*		move.l	a5,d0
 *		move.l	(a5),d1
 *		move.l	PROCESSMGRGLOBALS,a5
 *		move.l	d1,(a5)
 */
 
/* CurrentA5Setup. Set up CURRENTA5 and copy in the graphics global pointer.
 * NOTE: When is this useful, since the CURRENTA5 graphics world is
 * undoubtedly OK to use?
 */
unsigned long
CurrentA5Setup(void)
	= {0x200D, 0x2215, 0x2A78, 0x0904, 0x2a81};
/*		move.l	a5,d0
 *		move.l	(a5),d1
 *		move.l	$904,a5
 *		move.l	d1,(a5)
 */

/* A5Restore.  Set a5 from parameter.  Does not restore the graphics global pointer. */
#pragma parameter A5Restore(__D0)
void
A5Restore(unsigned long myA5)
	= {0x2A40};
/*		move.l	d0,a5
 */

/* ProcessMgrA5SimpleSetup. Set up PROCESSMGRGLOBALS, but don't copy the graphics global
 * pointer.
 */
unsigned long
ProcessMgrA5SimpleSetup(void)
	= {0x200D, 0x2A78, 0x0B7C};
/*		move.l	a5,d0
 *		move.l	PROCESSMGRGLOBALS,a5
 */
 
/* CurrentA5SimpleSetup. Set up CURRENTA5, but don't copy the graphics global pointer */
unsigned long
CurrentA5SimpleSetup(void)
	= {0x200D, 0x2A78, 0x0904};
/*		move.l	a5,d0
 *		move.l	$904,a5
 */
 
/* A5SimpleRestore.  Set a5 from parameter.  Does not restore the graphics global pointer. */
#pragma parameter A5SimpleRestore(__D0)
void
A5SimpleRestore(unsigned long myA5)
	= {0x2A40};
/*		move.l	d0,a5
 */

/* Access to assembler */
void
GetLiveFPURegs(Ptr dstPtr)
	= {0x2057, 0xF218, 0xBC00, 0xF210, 0xF0FF};
/*		move.l	(sp),a0
 *		fmovem	fpcr/fpsr/fpiar,(a0)+
 *		fmovem	fp0-fp7,(a0)
 */

void
SetLiveFPURegs(Ptr srcPtr)
	= {0x2057, 0xF218, 0x9C00, 0xF210, 0xD0FF};
/*		move.l	(sp),a0
 *		fmovem	(a0)+,fpcr/fpsr/fpiar
 *		fmovem	(a0),fp0-fp7
 */

unsigned long
GetVBR(void)
	= {0x4E7A, 0x0801};
	
/*		move.l	vbr,d0
 */

/* Direct access to traps */

/* SysContigPurgeSpace.  This "customizes" the trap by specifying SYS, and
 * by moving the value we care about from a0 to d0.
 */
#pragma parameter __D0 SysContigPurgeSpace
pascal long
SysContigPurgeSpace(void)
	= {0xA562, 0x2008};
/*	_PurgeSpace	,SYS
 *	move.l		a0,d0
 */

#pragma parameter ControlImmed(__A0)
void
ControlImmed(CntrlParam *)
	= {0xA204};
/*		_Control	,Immed
 */

pascal void
ShutDwnUserChoice(void)
    = {0x3F3C,0x0005,0xA895}; 
/*	move.w	#$0005,-(A7)
 *	_ShutDown
 */

pascal void
debugger(StringPtr message)
	= {0xABFF};
/*	_Debugger
 */
 
#endif __GLUE__
