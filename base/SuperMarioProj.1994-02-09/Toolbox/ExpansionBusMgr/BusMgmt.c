/*
	File:		BusMgmt.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	11/11/93	fau		first checked in

*/

/*
 * File:		BusMgmt.c
 *
 * Contains:	Bus Management Functions
 *
 * Written by:	Al Kossow
 *
 * Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.
 *
 * Change History (most recent first):
 *
 * <1>			10/20/93	aek		Created
 *
 *
 * These routines are used to register and remove handlers for bus segments
 * from the rest of the system.
 *
 * Note: It is possible that more than one bus manager may be required on a
 *       system for the same bus type. An example of this would be two PCI
 *		 segments with bridge chips that have different programming models.
 *
 */

#include "ExpansionMgrInternal.h"

/*
 * AddBusManager()
 *
 * Register a new bus segment with the system
 *
 */

AddBusManager()
{
}

/*
 * RmvBusManager()
 *
 * Remove a bus and all associated devices from the system
 *
 */
 
RmvBusManager()
{
}
