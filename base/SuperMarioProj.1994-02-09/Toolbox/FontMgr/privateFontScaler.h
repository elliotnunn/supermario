/*
	File:		privateFontScaler.h

	Contains:	Nothing Important

	Written by:	Charlton E. Lui

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 7/18/90	MR		change parameters of SetupKey to be input*, unsigned and int
		 <2>	  5/3/90	RB		nothing new
		 <1>	 4/16/90	HJR		first checked in
		 <1>	 4/10/90	CL		first checked in

	To Do:
*/

/* ****************************************************
**
** CKL	02/20/1990	Added ANSI-C prototypes
**
** ****************************************************
*/
 
/*
 *
 *  © Apple Computer Inc. 1988, 1989, 1990.
 *
 *	The file defines private sfnt stuff
 *
 * History:
 * 
 */




extern pascal int32 fs_SetMem(register fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr); 
extern fsg_SplineKey* fs_SetUpKey(fs_GlyphInputType* inptr, unsigned stateBits, int* error);
