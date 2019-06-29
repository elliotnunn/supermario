/*
 * File Memory.r
 *
 * Copyright Apple Computer, Inc. 1985-1987
 * All rights reserved.
 *
 * Sample desk accessory resource file.
 * This incorporates the DRVR header information (defined here)
 * with the linked code (stored as a 'DRVW' resource in the link command)
 */

#include "Types.r"				/* To get system types */
#include "MPWTypes.r"			/* To get 'DRVW' type */

#define DriverID	12

#ifdef NOASM_BUILD
/*
 * This will produce a DRVR resource from the special DRVW type.
 * (this eliminates the need for using the Assembler to create
 *	the DA header, but makes it impossible to use SADE, oops!)
 *
 * Note that the ID 12 is irrelevant, since the Font/DA Mover
 * will renumber it to something else when installing it anyway.
 *
 * The leading NUL in the resource name is required to
 * conform to the desk accessory naming convention.
 *
 * The resource is declared purgeable.	If the code were to
 * do funky things like SetTrapAddress calls (requiring the code to
 * be around at all times), we would have to set it nonpurgeable.
 */

type 'DRVR' as 'DRVW';			/* Map 'DRVW' => 'DRVR' */

resource 'DRVR' (DriverID, "\0x00Memory", purgeable) {
	/*
	 * DRVR flags
	 */
	dontNeedLock,			/* OK to float around, not saving ProcPtrs */
	needTime,				/* Yes, give us periodic Control calls */
	dontNeedGoodbye,		/* No special requirements */
	noStatusEnable,
	ctlEnable,				/* Desk accessories only do Control calls */
	noWriteEnable,
	noReadEnable,
	5*60,					/* drvrDelay - Wake up every 5 seconds */
	updateMask, 			/* drvrEMask - This DA only handles update events */
	0,						/* drvrMenu - This DA has no menu */
	"Memory",				/* drvrName - This isn't used by the DA */
	/*
	 * This directive inserts the contents of the DRVW resource
	 * produced by linking DRVRRuntime.o with our DA code
	 */
	$$resource("Memory.DRVW", 'DRVW', 0)
};
#endif

/*
 * Since desk accessories cannot use global data (and the C compiler
 * considers string constants to be global data) and we really don't
 * want to hard-code strings in our source, the strings used by the 
 * DA are stored in the resource file. Note the expression used to
 * figure out the resource id.
 */

resource 'STR#' (0xC000 | (DriverID << 5), "Memory's Strings") {
	{
		"AppHeap: ";
		"  SysHeap: ";
		"  Disk: ";
		" free on "
	};
};


resource 'WIND' (0xC000 | (DriverID << 5), "Memory's Window") {
	{322, 10, 338, 500},
	noGrowDocProc,
	visible,
	goAway,
	0x0,
	"Free Memory (# Bytes)"
};
