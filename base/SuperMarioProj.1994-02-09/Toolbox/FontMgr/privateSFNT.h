/*
	File:		private sfnt.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 7/18/90	MR		Change return types to int for computemapping and readsfnt
		 <3>	 7/14/90	MR		changed SQRT to conditional FIXEDSQRT2
		 <2>	 7/13/90	MR		Change parameters to ReadSFNT and ComputeMapping
		<1+>	 4/18/90	CL		
		 <1>	 3/21/90	EMT		First checked in with Mike Reed's blessing.

	To Do:
*/

/* PUBLIC PROTOTYPE CALLS */

/*
 * Below we have private stuff
 * This has nothing to do with the file format.
 */
 extern void* sfnt_GetTablePtr (fsg_SplineKey *key, sfnt_tableIndex n, boolean mustHaveTable);

/*
 * Creates mapping for finding offset table
 */
extern void sfnt_DoOffsetTableMap(fsg_SplineKey *key);

/* perfect spot size (Fixed) */
#ifndef FIXEDSQRT2
#define FIXEDSQRT2 0x00016A0A
#endif

/*
 * Returns offset and length for table n
 */
extern void sfnt_GetOffsetAndLength(fsg_SplineKey *key, int32 *offsetT, unsigned *lengthT, sfnt_tableIndex n);

extern int sfnt_ComputeMapping(fsg_SplineKey *key, uint16 platformID, uint16 specificID);

extern void sfnt_ReadSFNTMetrics(fsg_SplineKey *key, uint16 glyphIndex);

extern int sfnt_ReadSFNT(fsg_SplineKey *key, int16 *elementCount, uint16 gIndex, boolean useHints, voidFunc traceFunc);
