/*
	File:		WindowMgrPatches.c

	Contains:	Routines which patch the Window Manager traps.

	Written by:	Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	11/15/92	JDR		Changed QuickDraw.h to not use the dangerous pattern, which
									defines a pattern as a struct. This code was calling BlockMove
									to copy the 8 byte pattern, but patterns are structs so Pattern
									= Pattern works fine. I fixed the includes a bit too, and the
									dependencies were wrong by the way so I fixed them as well.
		 <6>	 5/19/92	YK		#1030028: Remove <4> and <5> since SWM is not an application
									now.    Roll back to <3>.
		 <5>	 1/14/92	YK		Added checking code for TSM. Removed the include statement that
									was added at <4>.
		 <4>	 1/10/92	YK		Added support for the special-hacked-background app for the Text
									Services Manager in c_initwindows.
									Include MFPrivate.h to do that.
		 <3>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
		 <0>	  9/2/86	ELR		New Today.

*/

#include <QuickDraw.h>
#include <Windows.h>
#include <Resources.h>
#include <Memory.h>
#include <Menus.h>
#include <Palettes.h>
#include <TextUtils.h>
#include <ToolUtils.h>

#include "Data.h"
#include "Glue.h"
#include "Lomem.h"
#include "SysMisc.h"


/* Some function prototypes that should be in (yet another) header file */
void		InsertAppInApplicationMenu(void);
void		AdjustApplicationMenu(PEntryPtr);

/* This routine handles the call to InitWindows() made by the application. */
pascal void
c_initwindows(void)
	{
	unsigned long	olda5;
	Boolean			hascolors;

#ifdef DEBUG
	/* catch dummies that are calling InitWindows but have declared themselves BG-only */
	olda5 = ProcessMgrA5SimpleSetup();
	if ((pCurrentProcess->p_taskmode & modeOnlyBackground) != 0)
		dbmsg("This app lied that it's onlyBackground: it has called InitWindows!!!");
	A5SimpleRestore(olda5);
#endif DEBUG

	if (WWEXIST != (char) 0)
		{
		LayerInit();

		WWEXIST = (char) 0;
		SAVEUPDATE = (short) -1;
		PAINTWHITE = (short) -1;
		ALARMSTATE |= AlrmParity;
		DSWNDUPDATE |= ((char)0x80);

		SetPort(WMGRPORT);
		DESKPATTERN = **GetPattern(deskPatID);
		ShowCursor();

		olda5 = ProcessMgrA5SimpleSetup();
		hascolors = Colorized;
		A5SimpleRestore(olda5);

		GHOSTWINDOW = nil;
		CURACTIVATE = nil;
		CURDEACTIVATE = nil;

		/* Set up the auxiliary window lists for color machines */
		if (hascolors)
			{
			AUXWINHEAD = (AuxWinHandle) NewHandleClear(sizeof(AuxWinRec));
			HLock(AUXWINHEAD);
			((AuxWinPtr) (*AUXWINHEAD))->awCTable = RGetResource('wctb', 0);
			HUnlock(AUXWINHEAD);

			AUXCTLHEAD = (AuxWinHandle) NewHandleClear(sizeof(AuxCtlRec));
			HLock(AUXCTLHEAD);
			((AuxCtlPtr) (*AUXCTLHEAD))->acCTable = RGetResource('cctb', 0);
			HUnlock(AUXCTLHEAD);
			}

		/* Don't do menus for MF itself */
		if (PROCESSMGRGLOBALS != CURRENTA5)
			{
			InitMenus();
			olda5 = ProcessMgrA5SimpleSetup();
			InsertAppInApplicationMenu();
			AdjustApplicationMenu(nil);
			A5SimpleRestore(olda5);
			DrawMenuBar();
			}

		if (hascolors)
			InitPalettes();
		}
	}

struct portlist {
	short		portcount;
	GrafPtr		ports[];
};

/* PortListKill.  Dump all grafports associated with the given process */
void
PortListKill(PEntryPtr pProc)
	{
	GrafPtr			*gp;
	struct portlist	**plist;
	short			n;
	u_long			base, bound;

	/* Do nothing if PORTLIST not initialized */
	if ((long)PORTLIST == -1)
		return;

	/* get process partition limits for check */
	base = (unsigned long) pProc->p_zone;
	bound = base + pProc->p_size;

top:
	plist = (struct portlist **)PORTLIST;
	n = (*plist)->portcount;
	gp = (*plist)->ports;

	while (n--)
		{
		if ( ((unsigned long) *gp >= base) && ((unsigned long) *gp <= bound) )
			{
			--(*plist)->portcount;
			Munger(plist, 2, gp, 4, (Ptr)-1, 0);
			goto top;
			}
		else
			gp++;
		}
	}

/* QDKill. Kill all quickdraw related resources for the current process.
 * NOTE: Assumes A5 = PROCESSMGRGLOBALS, and THEZONE = pCurrentProcess->p_zone
 */
void
QDKill(void)
	{
	if (Colorized)
		PortListKill(pCurrentProcess);
	}
