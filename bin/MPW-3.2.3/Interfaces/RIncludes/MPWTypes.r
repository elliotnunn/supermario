/*
	MPWTypes.r -- Type Declarations for Rez and DeRez
	
	Copyright Apple Computer, Inc. 1986-1989
	All rights reserved.

	August 15, 1986
*/

#ifndef __MPWTYPES.R__
#define __MPWTYPES.R__

/*
 * Macintosh Programmer's Workshop
 * DRVR resource type 'DRVR' (requires DRVRRuntime.o)
 */

# define mDownMask		   2		/* Event mask values for drvrEMask */
# define mUpMask		   4
# define keyDownMask	   8
# define keyUpMask		  16
# define autoKeyMask	  32
# define updateMask 	  64
# define diskMask		 128
# define activMask		 256
# define networkMask	1024
# define driverMask 	2048
# define everyEvent 	  -1

/*
 * When building a DRVR resource, insert the line:
 *
 *		type 'DRVR' as 'DRVW';
 *
 * in your resource file before your "resource 'DRVR' ..." declaration.
 */

type 'DRVW' {
	boolean = 0;							/* unused */
	boolean dontNeedLock,	 needLock;		/* lock drvr in memory	*/
	boolean dontNeedTime,	 needTime;		/* for periodic action	*/
	boolean dontNeedGoodbye, needGoodbye;	/* call before heap reinit*/
	boolean noStatusEnable,  statusEnable;	/* responds to status	*/
	boolean noCtlEnable,	 ctlEnable; 	/* responds to control	*/
	boolean noWriteEnable,	 writeEnable;	/* responds to write	*/
	boolean noReadEnable,	 readEnable;	/* responds to read 	*/

	byte = 0;					/* low byte of drvrFlags word unused */

	unsigned integer;			/* driver delay (ticks) */
	integer;					/* desk acc event mask	*/
	integer;					/* driver menu ID		*/

	unsigned integer = 50;		/* offset to DRVRRuntime open	*/
	unsigned integer = 54;		/* offset to DRVRRuntime prime	*/
	unsigned integer = 58;		/* offset to DRVRRuntime control*/
	unsigned integer = 62;		/* offset to DRVRRuntime status */
	unsigned integer = 66;		/* offset to DRVRRuntime close	*/

	pstring[31];				/* driver name */

	hex string; 				/* driver code (use $$resource(...)) */
};

#endif __MPWTYPES.R__
