/*
	File:		FSCdefs.h

	Contains:	xxx put contents here (or delete the whole line) xxx

	Written by:	xxx put name of writer here (or delete the whole line) xxx

	Copyright:	© 1988-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 9/25/91	jlf		Corrected definition of ReleaseSFNTFunc by adding a long parameter
		 							before the void* param. Defined RELEASE_MEM_FRAG conditional.
		 <3>	11/27/90	MR		Add #define for PASCAL. [ph]
		 <2>	 11/5/90	MR		Move USHORTMUL from FontMath.h, add Debug definition [rb]
		 <7>	 7/18/90	MR		Add byte swapping macros for INTEL, moved rounding macros from
									fnt.h to here
		 <6>	 7/14/90	MR		changed defines to typedefs for int[8,16,32] and others
		 <5>	 7/13/90	MR		Declared ReleaseSFNTFunc and GetSFNTFunc
		 <4>	  5/3/90	RB		cant remember any changes
		 <3>	 3/20/90	CL		type changes for Microsoft
		 <2>	 2/27/90	CL		getting bbs headers
	   <3.0>	 8/28/89	sjk		Cleanup and one transformation bugfix
	   <2.2>	 8/14/89	sjk		1 point contours now OK
	   <2.1>	  8/8/89	sjk		Improved encryption handling
	   <2.0>	  8/2/89	sjk		Just fixed EASE comment
	   <1.5>	  8/1/89	sjk		Added composites and encryption. Plus some enhancements…
	   <1.4>	 6/13/89	SJK		Comment
	   <1.3>	  6/2/89	CEL		16.16 scaling of metrics, minimum recommended ppem, point size 0
									bug, correct transformed integralized ppem behavior, pretty much
									so
	   <1.2>	 5/26/89	CEL		EASE messed up on “c” comments
	  <•1.1>	 5/26/89	CEL		Integrated the new Font Scaler 1.0 into Spline Fonts
	   <1.0>	 5/25/89	CEL		Integrated 1.0 Font scaler into Bass code for the first time…

	To Do:
*/

/* #define DEBUG			*/
#define RELEASE_MEM_FRAG					/* <4-jlf> */
#define PASCAL		pascal

#define true 1
#define false 0
#define ONEFIX 		( 1L << 16 )
#define ONEFRAC 	( 1L << 30 )
#define ONEHALFFIX	0x8000L
#define ONEVECSHIFT	16
#define HALFVECDIV	(1L << (ONEVECSHIFT-1))

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

typedef short FWord;
typedef unsigned short uFWord;
typedef long F26Dot6;

#ifndef Fixed
#define Fixed long
#endif

#ifndef Fract
#define Fract long
#endif

#ifndef boolean
#define boolean int
#endif

typedef void (*voidFunc) ();

typedef void*	VoidPtr;

typedef void	(*ReleaseSFNTFunc)(long, void*);				/* <4-jlf> */
typedef void*	(*GetSFNTFunc)(long, long, long);

#ifdef INTEL

#define SWAPW(a)		(unsigned short)(((unsigned char)((a) >> 8)) | ((unsigned char)(a) << 8))
#define SWAPWINC(a)		SWAPW(*(a)); a++
#define SWAPL(a)		((((a)&0xff) << 24) | (((a)&0xff00) << 8) | (((a)&0xff0000) >> 8) | ((a) >> 24))

typedef int	LoopCount;
typedef int	ArrayIndex;

#define USHORTMUL(a, b)	((uint32)((uint32)(a)*(uint32)(b)))
#define SHORTMUL(a,b)	(int32)((int32)(a) * (int32)(b))
#define SHORTDIV(a,b)	(int32)((int32)(a) / (int32)(b))

#else

#define SWAPW(a)		(a)
#define SWAPWINC(a)		(*(a)++)
#define SWAPL(a)		(a)

typedef int16	LoopCount;		/* short gives us a DBF */
typedef int32	ArrayIndex;		/* avoids EXT.L */

#define USHORTMUL(a, b)	((uint32)((uint16)(a)*(uint16)(b)))
#define SHORTMUL(a,b)	(int32)((int16)(a) * (int16)(b))
#define SHORTDIV(a,b)	(int32)((int16)(a) / (int16)(b))

#endif	/* intel */

/* d is half of the denumerator */
#define FROUND( x, n, d, s ) \
	    x = SHORTMUL(x, n); x += d; x >>= s;

/* <3> */
#define SROUND( x, n, d, halfd ) \
    if ( x < 0 ) { \
	    x = -x; x = SHORTMUL(x, n); x += halfd; x /= d; x = -x; \
	} else { \
	    x = SHORTMUL(x, n); x += halfd; x /= d; \
	}

#ifdef DEBUG
#ifndef NOT_ON_THE_MAC
#ifndef __TYPES__
	pascal void Debugger(void) = 0xA9FF; 
	pascal void DebugStr(char* aStr) = 0xABFF; 
#endif
#endif
#endif