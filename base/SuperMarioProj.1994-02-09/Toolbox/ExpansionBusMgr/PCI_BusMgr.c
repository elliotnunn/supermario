/*
	File:		PCI_BusMgr.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	11/11/93	fau		first checked in

*/

/*
 * File:		PCI_BusMgr.c
 *
 * Contains:	Expansion Mgr PCI Specific Routines
 *
 * Written by:	Al Kossow
 *
 * Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.
 *
 * Change History (most recent first):
 *
 * <1>			10/20/93	aek		Created
 *
 * Handle calls specific to the PCI expansion bus for systems with a Bandit
 * ARBus to PCI bridge chip, and Grand Central Slot Interrupts
 *
 * Functions supported:
 *
 * PCI Device Configuration Register Accesses
 *
 * Generating Config and I/O Cycles (crit section..)
 *
 *
 */

#include "ExpansionMgrInternal.h"
