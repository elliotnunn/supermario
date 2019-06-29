/*
	File:		ProcessMgr.r

	Contains:	Resources for the Process Mgr proper.

	Written by:	Erich Ringewald

	Copyright:	© 1986-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):
	 
		 <2>	10/22/90	DFH		Add include for types.r
		 <0>	 x/xx/86	PYG		New Today.

*/ 
 
#define FOR_REZ 1 
#include "types.r"
#include "puppet.h"

/************************************************************************************
 * Template for the Application Menu.
 * NOTE: Title is irrelevant 5-character string that just reserves space for the
 * icon cache handle we will put there when we actually insert the menu.
 ************************************************************************************/
resource 'MENU' (-16489, sysHeap) {
	-16489, textMenuProc, allEnabled, enabled, "Maura",
	{
		"Hide ^0",
			noicon, nokey, nomark, plain;
		"Hide Others",
			noicon, nokey, nomark, plain;
		"Show All",
			noicon, nokey, nomark, plain;
		"-",
			noicon, nokey, nomark, plain
	}
};

/************************************************************************************
 * Resources for EPPC.
 ************************************************************************************/
resource 'STR#' (-16415, locked, sysHeap)
	{
		{
		"May you code in interesting times.",
		"MacOS",
		"ZSYSeppc"
		}
	};

/************************************************************************************
 * Resources for scrap coercion.
 ************************************************************************************/
/* These better be the prefixes of any item in the edit menu that alters the local scrap (e.g. "Copy Chart…" in Excel). */
resource 'STR#' (-16414, locked, sysHeap)
	{
		{
		"Copy"
		}
	};

/************************************************************************************
 * Resources for puppet instruction to event sequence conversion.
 ************************************************************************************/

/* Strings for puppet strings */
type menuStringResType as 'STR ';
type menuStringListResType as 'STR#';

/* Resources for QUIT puppet string */
resource menuStringResType (strIDQuitMenu, sysHeap)
	{
	"File"
	};
resource menuStringResType (strIDQuitItem, sysHeap)
	{
	"Quit"
	};

/* Resources for OPEN puppet string */
resource menuStringResType (strIDOpenMenu, sysHeap)
	{
	"File"
	};
resource menuStringListResType (strIDOpenItem, sysHeap)
	{
		{
		"Open…",
		"Open...",
		"Open ...",
		"Open Stack..."
		}
	};
