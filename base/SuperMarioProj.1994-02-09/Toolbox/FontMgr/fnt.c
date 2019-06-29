/*
	File:		fnt.c

	Copyright:	© 1987-1990, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<25>	 7/20/92	DTY		Revert fnt_IP back to 7.0 code.
		<24>	 7/23/91	RB		Bumped version number returned by GetInfo to 4 for 6.1J TT. Make
									it 5 for Kirin 1. Make it 6 for CubeE.
		<23>	  5/7/91	RB		Cancelled bugfix made in <18>.  When freedom and projection
									vectors are orthogonal, the projection vector is set to the
									freedom vector.  However, fonts have already been shipped that
									rely on this bad behavior, so we're stuck with it. Echh!!
		<22>	 4/26/91	RB		Add DEBUG assertion checks for jump instructions.
		<21>	 4/25/91	RB		no changes
		<20>	 4/11/91	RB		Change to New Mnemonics for OPCodes
		<19>	 4/10/91	RB		Finish obsoleting AA and SANGW
		<18>	  4/9/91	RB		Change orthogonal freedom and projection vector fix so
									that cosine of angle is pinned at 1/16 rather than set to 1.0.
		<17>	  4/9/91	RB		Fix IP when original range is 0.
		<16>	  4/5/91	RB		Bugfix to fnt_Normalize; check for 0 length;
									if so, set vector to x axis. 
		<15>	  3/25/91	RB		Change Kanji Adjust Instruction
		<13>	  3/7/91	RB		***********  POST 7.0 ****************
									- Add Adjust Instruction for Kanji.
									**************************************
		<14>	 3/11/91	RB		Make AA and SANGW obsolete.
		<12>	 2/15/91	RB		MR, #82806: Bump version number returned by GETINFO instruction
									so 7.0 TT (version 2) can be distinguished from 6.0 TT (version
									1).
		<11>	 2/11/91	RB		MR, CL,CC,#82409: Add initials to previous change. Also
									rearrange order to be more like original.
		<10>	  2/8/91	RB		Fix instructions MIRP and MSIRP to correctly move point in
									twilight zone.
		 <9>	12/20/90	RB		Add mr initials to previous change comment. [mr]
		 <8>	12/20/90	RB		Fix bug in INSTCTRL so unselected flags are not changed.[mr]
		 <7>	 12/5/90	MR		Change RAW to use the phantom points.
		 <6>	11/27/90	MR		Fix bogus debugging in Check_ElementPtr. [rb]
		 <5>	11/16/90	MR		More debugging code [rb]
		 <4>	 11/9/90	MR		Fix non-portable C (dup, depth, pushsomestuff) [rb]
		 <3>	 11/5/90	MR		Use proper types in fnt_Normalize, change globalGS.ppemDot6 to
									globalGS.fpem. Make instrPtrs all uint8*. Removed conditional
									code for long vectors. [rb]
		 <2>	10/20/90	MR		Restore changes since project died. Converting to 2.14 vectors,
									smart math routines. [rb]
		<24>	 8/16/90	RB		Fix IP to use oo domain
		<22>	 8/11/90	MR		Add Print debugging function
		<21>	 8/10/90	MR		Make SFVTL check like SPVTL does
		<20>	  8/1/90	MR		remove call to fnt_NextPt1 macro in fnt_SHC
		<19>	 7/26/90	MR		Fix bug in SHC
		<18>	 7/19/90	dba		get rid of C warnings
		<17>	 7/18/90	MR		What else, more Ansi-C fixes
		<16>	 7/13/90	MR		Added runtime range checking, various ansi-fixes
		<15>	  7/2/90	RB		combine variables into parameter block.
		<12>	 6/26/90	RB		bugfix in divide instruction
		<11>	 6/21/90	RB		bugfix in scantype
		<10>	 6/12/90	RB		add scantype instruction, add selector variable to getinfo
									instruction
		 <9>	 6/11/90	CL		Using the debug call.
		 <8>	  6/4/90	MR		Remove MVT
		 <7>	  6/3/90	RB		no change
		 <6>	  5/8/90	RB		revert to version 4
		 <5>	  5/4/90	RB		mrr-more optimization, errorchecking
		 <4>	  5/3/90	RB		more optimization. decreased code size.
		 <4>	  5/2/90	MR		mrr - added support for IDEF mrr - combined multiple small
									instructions into switchs e.g. BinaryOperands, UnaryOperand,
									etc. (to save space) mrr - added twilightzone support to fnt_WC,
									added fnt_ROTATE, fnt_MAX and fnt_MIN. Max and Min are in
									fnt_BinaryOperand. Optimized various functions for size.
									Optimized loops in push statements and alignrp for speed.
									gs->loop now is base-0. so gs->loop == 4 means do it 5 times.
		 <3>	 3/20/90	MR		Added support for multiple preprograms. This touched function
									calls, definitions. Fractional ppem (called ppemDot6 in globalGS
									Add new instructions ELSE, JMPR Experimenting with RMVT, WMVT
									Removed lots of the MR_MAC #ifdefs, GOSLOW, Added MFPPEM, MFPS
									as experiments (fractional versions) Added high precision
									multiply and divide for MUL and DIV Changed fnt_MD to use oox
									instead of ox when it can (more precise) fnt_Init: Initialize
									instruction jump table with *p++ instead of p[index] Changed
									fnt_AngleInfo into fnt_FractPoint and int16 for speed and to
									maintain long alignment Switch to Loop in PUSHB and PUSHW for
									size reduction Speed up GetCVTScale to avoid FracMul(1.0,
									transScale) (sampo)
		 <2>	 2/27/90	CL		Added DSPVTL[] instruction.  Dropout control scanconverter and
									SCANCTRL[] instruction.  BugFix in SFVTL[], and SFVTOPV[].
									Fixed bug in fnt_ODD[] and fnt_EVEN[]. Fixed bug in
									fnt_Normalize
	   <3.4>	11/16/89	CEL		Added new functions fnt_FLIPPT, fnt_FLIPRGON and fnt_FLIPRGOFF.
	   <3.3>	11/15/89	CEL		Put function array check in ifndef so printer folks could
									exclude the check.
	   <3.2>	11/14/89	CEL		Fixed two small bugs/feature in RTHG, and RUTG. Added SROUND &
									S45ROUND.
	   <3.1>	 9/27/89	CEL		GetSingleWidth slow was set to incorrect value. Changed rounding
									routines, so that the sign flip check only apply if the input
									value is nonzero.
	   <3.0>	 8/28/89	sjk		Cleanup and one transformation bugfix
	   <2.3>	 8/14/89	sjk		1 point contours now OK
	   <2.2>	  8/8/89	sjk		Now allow the system to muck with high bytes of addresses
	   <2.1>	  8/8/89	sjk		Improved encryption handling
	   <2.0>	  8/2/89	sjk		Just fixed EASE comment
	   <1.8>	  8/1/89	sjk		Added in composites and encryption. Plus other enhancements…
	   <1.7>	 6/13/89	sjk		fixed broken delta instruction
	   <1.6>	 6/13/89	SJK		Comment
	   <1.5>	  6/2/89	CEL		16.16 scaling of metrics, minimum recommended ppem, point size 0
									bug, correct transformed integralized ppem behavior, pretty much
									so
	   <1.4>	 5/26/89	CEL		EASE messed up on “c” comments
	  <•1.3>	 5/26/89	CEL		Integrated the new Font Scaler 1.0 into Spline Fonts

	To Do:
	<24>  Added string debug support.  NewFDebugstr.
					 
		 
		 
rwb 4/24/90 - changed scanctrl instruction to take 16 bit argument
*/
/******* MIKE PLEASE FIX up these comments so they fit in the header above with the same
		 FORMAT!!!
*/
/*
 * File: fnt.c
 *
 * This module contains the interpreter that executes font instructions
 *
 * The BASS Project Interpreter and Font Instruction Set sub ERS contains
 * relevant information.
 *
 *  © Apple Computer Inc. 1987, 1988, 1989, 1990.
 *
 * History:
 * Work on this module began in the fall of 1987
 *
 * Written June 23, 1988 by Sampo Kaasila
 *
 * Rewritten October 18, 1988 by Sampo Kaasila. Added a jump table instead of the switch statement.
 *		Also added CALL(), LOOPCALL(), FDEF(), ENDF(), and replaced WX(),WY(), RX(), RY(), MD()
 *		with new WC(), RC(), MD(). Reimplemented IUP(). Also optimized the code somewhat.
 *		Cast code into a more digestable form using a local and global graphics state.
 *
 * December 20, 1988. Added DELTA(), SDB(), SDS(), and deleted ONTON(), and ONTOFF(). ---Sampo.
 *
 * January 17, 1989 Added DEBUG(), RAW(), RLSB(), WLSB(), ALIGNPTS(), SANGW(), AA().
 *			    Brought up this module to an alpha ready state. ---Sampo
 *
 * January 31, 1989 Added RDTG(), and RUTG().
 *
 * Feb 16, 1989 Completed final bug fixes for the alpha release.
 *
 * March 21, 1989 Fixed a small Twilight Zone bug in MIAP(), MSIRP(), and MIRP().
 *
 * March 28, 1989 Took away the need and reason for directional switches in the control value table.
 *		          However, this forced a modification of all the code that reads or writes to the control
 *				  value table and the single width. Also WCVT was replaced by WCVTFOL, WCVTFOD, and
 *				  WCVTFCVT. ---Sampo
 *
 * April 17, 1989 Modified RPV(), RFV(), WPV(), WFV() to work with an x & y pair instead of just x. --- Sampo
 *
 * April 25, 1989 Fixed bugs in CEILING(), RUTG(), FDEF(), and IF(). Made MPPEM() a function of
 *                the projection vector. ---Sampo
 *
 * June 7, 1989 Made ALIGNRP() dependent on the loop variable, and also made it blissfully
 *              ignorant of the twilight zone.
 * 				Also, added ROFF(), removed WCVTFCVT(), renamed WCVTFOL() to WCVT(), made MIRP() and
 *				MDRP() compensate for the engine even when there is no rounding. --- Sampo
 *
 * June 8, 1989 Made DELTA() dependent on the Transformation. ---Sampo
 *
 * June 19, 1989 Added JROF() and JROT(). ---Sampo
 *
 * July 14, 1989 Forced a pretty tame behaviour when abs((projection vector) * (freedoom vector)) < 1/16.
 *			The old behaviour was to grossly blow up the outline. This situation may happen for low pointsizes
 *			when the character is severly distorted due to the gridfitting ---Sampo
 *
 * July 17, 1989 Prevented the rounding routines from changing the sign of a quantity due to the engine compensation. ---Sampo
 *
 * July 19, 1989 Increased nummerical precision of fnt_MovePoint by 8 times. ---Sampo
 *
 * July 28, 1989 Introduced 5 more Delta instructions. (Now 3 are for points and 3 for the cvt.) ---Sampo
 *
 * Aug 24, 1989 fixed fnt_GetCVTEntrySlow and fnt_GetSingleWidthSlow bug ---Sampo
 *
 * Sep 26, 1989 changed rounding routines, so that the sign flip check only apply if the input value is nonzero. ---Sampo
 *
 * Oct 26, 1989 Fixed small bugs/features in fnt_RoundUpToGrid() and fnt_RoundToHalfGrid. Added SROUND() and S45ROUND(). ---Sampo
 *
 * Oct 31, 1989 Fixed transformation bug in fnt_MPPEM, fnt_DeltaEngine, fnt_GetCvtEntrySlow, fnt_GetSingleWidthSlow. ---Sampo
 *
 * Nov 3, 1989 Added FLIPPT(), FLIPRGON(), FLIPRGOFF(). ---Sampo
 *
 * Nov 16, 1989 Merged back in lost Nov 3 changes.---Sampo
 *
 * Dec 2, 1989 Added READMETRICS() aand WRITEMETRICS(). --- Sampo
 *
 * Jan 8, 1990 Added SDPVTL(). --- Sampo
 *
 * Jan 8, 1990 Eliminated bug in SFVTPV[] ( old bug ) and SFVTL[] (showed up because of SDPVTL[]). --- Sampo
 *
 * Jan 9, 1990 Added the SCANCTRL[] instruction. --- Sampo
 *
 * Jan 24, 1990 Fixed bug in fnt_ODD and fnt_EVEN. ---Sampo
 *
 * Jan 28, 1990 Fixed bug in fnt_Normalize. --- Sampo
 *
 *	2/9/90	mrr	ReFixed Normalize bug, added ELSE and JMPR.  Added pgmList[] to globalGS
 				in preparation for 3 preprograms.  affected CALL, LOOPCALL, FDEF
 * 2/21/90	mrr	Added RMVT, WMVT.
 * 3/7/90	mrr		put in high precision versions of MUL and DIV.
 */

#include <setjmp.h>

/** FontScaler’s Includes **/
#include "FSCdefs.h"
#include "FontMath.h"
#include "sc.h"
#include "fnt.h"
#include "FSError.h"

/****** Macros *******/
#define POP( p )     ( *(--p) )
#define PUSH( p, x ) ( *(p)++ = (x) )

#define BADCOMPILER

#ifdef BADCOMPILER
#define BOOLEANPUSH( p, x ) PUSH( p, ((x) ? 1 : 0) ) /* MPW 3.0 */
#else
#define BOOLEANPUSH( p, x ) PUSH( p, x )
#endif

#define MAX(a,b)	((a) > (b) ? (a) : (b))

/* <24>  Cal added printing debug support for developer init */
#ifdef DEBUG
/*  FDebugStrNew routines CKL 6/25/1991  */

/* 	reverse string s in place <24> */
void reverse (char *s); 
void reverse (char *s) 
{
	char c;
	int i, j;
	j=0;
	while (s[j] != 0) ++j; /* find length of string */
	for (i=0, j = j-1; i<j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/* convert n to characters in s <24> */	
char *itoa (int n, char *s);
char *itoa (int n, char *s) 
{
	int i, sign;
	
	if ((sign = n) <0) 
	{
		n=-n;
	}
	
	i=0;
	do
	{
		s[i++] = n%10+'0';
	} while ((n/=10)>0);
	
	if (sign<0) 
	{
		s[i++] = '-';
	}
	s[i] = 0;
	reverse(s) ;
	return (s) ;
}

/* concatenate two strings <24> */	
void catstrings (char *str1, char* str2);
void catstrings (char *str1, char* str2)
{
	while (*str1 != 0) ++str1;				/* find end of str1 */
	while ((*str1++ = *str2++) != 0);	/* copy str2 */
}

/*  make c string into pascal string - rwb <24> */	
void c2pas (char *str );
void c2pas (char *str )
{
	int i;
	int j = 0;
	while (str[j] != 0) ++j;				/* find end of str */
	for( i = j; i>0; --i) str[i] = str[i-1];
	str[0] = j;
}

void FDebugStrNew(char *str1, int n);	
void FDebugStrNew(char *str1, int n)
{
	char str0[80], str2[10];

	str0[0] = 0;
	catstrings (str0, str1);
	itoa(n,str2);				/* convert index to ASCII value */
	catstrings (str0, str2) ;	/* concatenate the two strings */
	c2pas( str0);
	DebugStr(str0);
}
	
void CHECK_RANGE(int32 n, int32 min, int32 max);
void CHECK_RANGE(int32 n, int32 min, int32 max)
{
	if (n > max || n < min)
		Debugger();
}
void CHECK_RANGE2(int32 n, int32 min, int32 max, fnt_GlobalGraphicStateType* gs);
void CHECK_RANGE2(int32 n, int32 min, int32 max, fnt_GlobalGraphicStateType* gs)
{
	if (n > max || n < min)
		FDebugStrNew("glyph id = ", gs->glyphIndex);
		//FDebugStr("glyph id = 0x%x",gs->glyphIndex);
}
void CHECK_ASSERTION( int expression );
void CHECK_ASSERTION( int expression )
{
	if (!expression)
		Debugger();
}
void CHECK_CVT(fnt_LocalGraphicStateType* gs, int cvt);
void CHECK_CVT(fnt_LocalGraphicStateType* gs, int cvt)
{
	CHECK_RANGE(cvt, 0, gs->globalGS->cvtCount-1);
}
void CHECK_CVT2(fnt_LocalGraphicStateType* gs, int cvt);
void CHECK_CVT2(fnt_LocalGraphicStateType* gs, int cvt)
{
	CHECK_RANGE2(cvt, 0, gs->globalGS->cvtCount-1, gs->globalGS);
}
void CHECK_FDEF(fnt_LocalGraphicStateType* gs, int fdef);
void CHECK_FDEF(fnt_LocalGraphicStateType* gs, int fdef)
{
	CHECK_RANGE(fdef, 0, gs->globalGS->maxp->maxFunctionDefs-1);
}
void CHECK_PROGRAM(int pgmIndex);
void CHECK_PROGRAM(int pgmIndex)
{
	CHECK_RANGE(pgmIndex, 0, MAXPREPROGRAMS-1);
}
void CHECK_ELEMENT(fnt_LocalGraphicStateType* gs, int elem);
void CHECK_ELEMENT(fnt_LocalGraphicStateType* gs, int elem)
{
	CHECK_RANGE(elem, 0, gs->globalGS->maxp->maxElements-1);
}
void CHECK_ELEMENTPTR(fnt_LocalGraphicStateType* gs, fnt_ElementType* elem);
void CHECK_ELEMENTPTR(fnt_LocalGraphicStateType* gs, fnt_ElementType* elem)
{	
	if (elem == &gs->elements[1])
	{
		int maxctrs, maxpts;

		maxctrs = MAX(gs->globalGS->maxp->maxContours, gs->globalGS->maxp->maxCompositeContours);
		maxpts  = MAX(gs->globalGS->maxp->maxPoints, gs->globalGS->maxp->maxCompositePoints);

		CHECK_RANGE(elem->nc, 1, maxctrs);
		CHECK_RANGE(elem->ep[elem->nc-1], 0, maxpts-1);
	}
	else if (elem != &gs->elements[0])
	{
		Debugger();
	}
}
void CHECK_STORAGE(fnt_LocalGraphicStateType* gs, int index);
void CHECK_STORAGE(fnt_LocalGraphicStateType* gs, int index)
{
	CHECK_RANGE(index, 0, gs->globalGS->maxp->maxStorage-1);
}
void CHECK_STACK(fnt_LocalGraphicStateType* gs);
void CHECK_STACK(fnt_LocalGraphicStateType* gs)
{
	CHECK_RANGE(gs->stackPointer - gs->globalGS->stackBase, 0, gs->globalGS->maxp->maxStackElements-1);
}
void CHECK_POINT(fnt_LocalGraphicStateType* gs, fnt_ElementType* elem, int pt);
void CHECK_POINT(fnt_LocalGraphicStateType* gs, fnt_ElementType* elem, int pt)
{
	CHECK_ELEMENTPTR(gs, elem);
	if (gs->elements == elem)
		CHECK_RANGE(pt, 0, gs->globalGS->maxp->maxTwilightPoints - 1);
	else
		CHECK_RANGE(pt, 0, elem->ep[elem->nc-1] + 2);	/* phantom points */
}
void CHECK_CONTOUR(fnt_LocalGraphicStateType* gs, fnt_ElementType* elem, int ctr);
void CHECK_CONTOUR(fnt_LocalGraphicStateType* gs, fnt_ElementType* elem, int ctr)
{
	CHECK_ELEMENTPTR(gs, elem);
	CHECK_RANGE(ctr, 0, elem->nc - 1);
}
#define CHECK_POP(gs, s)		POP(s)
#define CHECK_PUSH(gs, s, v)	PUSH(s, v)
#else
#define CHECK_RANGE(a,b,c)
#define CHECK_RANGE2(a,b,c)
#define CHECK_ASSERTION(a)
#define CHECK_CVT(a,b)
#define CHECK_CVT2(a,b)
#define CHECK_POINT(a,b,c)
#define CHECK_CONTOUR(a,b,c)
#define CHECK_FDEF(a,b)
#define CHECK_PROGRAM(a)
#define CHECK_ELEMENT(a,b)
#define CHECK_ELEMENTPTR(a,b)
#define CHECK_STORAGE(a,b)
#define CHECK_STACK(a)
#define CHECK_POP(gs, s)		POP(s)
#define CHECK_PUSH(gs, s, v)	PUSH(s, v)
#endif

#define GETBYTE(ptr)	( (uint8)*ptr++ )
#define MABS(x)			( (x) < 0 ? (-(x)) : (x) )

#define BIT0( t ) ( (t) & 0x01 )
#define BIT1( t ) ( (t) & 0x02 )
#define BIT2( t ) ( (t) & 0x04 )
#define BIT3( t ) ( (t) & 0x08 )
#define BIT4( t ) ( (t) & 0x10 )
#define BIT5( t ) ( (t) & 0x20 )
#define BIT6( t ) ( (t) & 0x40 )
#define BIT7( t ) ( (t) & 0x80 )

/******** 12 BinaryOperators **********/
#define LT_CODE		0x50
#define LTEQ_CODE	0x51
#define GT_CODE		0x52
#define GTEQ_CODE	0x53
#define EQ_CODE		0x54
#define NEQ_CODE	0x55
#define AND_CODE	0x5A
#define OR_CODE		0x5B
#define ADD_CODE	0x60
#define SUB_CODE	0x61
#define DIV_CODE	0x62
#define MUL_CODE	0x63
#define MAX_CODE	0x8b
#define MIN_CODE	0x8c

/******** 9 UnaryOperators **********/
#define ODD_CODE		0x56
#define EVEN_CODE		0x57
#define NOT_CODE		0x5C
#define ABS_CODE		0x64
#define NEG_CODE		0x65
#define FLOOR_CODE		0x66
#define CEILING_CODE	0x67

/******** 6 RoundState Codes **********/
#define RTG_CODE		0x18
#define RTHG_CODE		0x19
#define RTDG_CODE		0x3D
#define ROFF_CODE		0x7A
#define RUTG_CODE		0x7C
#define RDTG_CODE		0x7D

/****** LocalGS Codes *********/
#define POP_CODE	0x21
#define SRP0_CODE	0x10
#define SRP1_CODE	0x11
#define SRP2_CODE	0x12
#define SLOOP_CODE	0x17
#define LMD_CODE	0x1A

/****** Element Codes *********/
#define SCE0_CODE	0x13
#define SCE1_CODE	0x14
#define SCE2_CODE	0x15
#define SCES_CODE	0x16

/****** Control Codes *********/
#define IF_CODE		0x58
#define ELSE_CODE	0x1B
#define EIF_CODE	0x59
#define ENDF_CODE	0x2d
#define MD_CODE		0x49

/* flags for UTP, IUP, MovePoint */
#define XMOVED 0x01
#define YMOVED 0x02

#ifdef SEGMENT_LINK
#pragma segment FNT_C
#endif

/* <13> defines for kanji adjust from here*/
#define ADJUSTBASE 0x8F							/* <15> base opcode for adjust instruction */
#define PIXEL 64
#define LG2PIXEL 6
#define HALF (PIXEL >> 1)
#define HALFM (HALF-1)
#define HALFP (HALF+1)
#define floor( x ) ((x) & ~0x3F )

typedef int pixel;
typedef int subPixel;
typedef short index;
/* <13> defines for kanji adjust to here */

/* Private function prototypes */

void fnt_Panic(fnt_LocalGraphicStateType *gs, int error);
void fnt_IllegalInstruction(fnt_LocalGraphicStateType *gs);
void fnt_Normalize(F26Dot6 x, F26Dot6 y, VECTOR*);
void fnt_MovePoint(fnt_LocalGraphicStateType *gs, fnt_ElementType *element, ArrayIndex point, F26Dot6 delta);
void fnt_XMovePoint(fnt_LocalGraphicStateType *, fnt_ElementType *element, ArrayIndex point, F26Dot6 delta) ;
void fnt_YMovePoint(fnt_LocalGraphicStateType *, fnt_ElementType *element, ArrayIndex point, F26Dot6 delta) ;
F26Dot6 fnt_Project(fnt_LocalGraphicStateType *gs, F26Dot6 x, F26Dot6 y);
F26Dot6 fnt_OldProject(fnt_LocalGraphicStateType *gs, F26Dot6 x, F26Dot6 y);
F26Dot6 fnt_XProject(fnt_LocalGraphicStateType *gs, F26Dot6 x, F26Dot6 y);
F26Dot6 fnt_YProject(fnt_LocalGraphicStateType *gs, F26Dot6 x, F26Dot6 y);
Fixed fnt_GetCVTScale(fnt_LocalGraphicStateType *gs);
F26Dot6 fnt_GetCVTEntryFast(fnt_LocalGraphicStateType *gs, ArrayIndex n);
F26Dot6 fnt_GetCVTEntrySlow(fnt_LocalGraphicStateType *gs, ArrayIndex n);
F26Dot6 fnt_GetSingleWidthFast(fnt_LocalGraphicStateType *gs);
F26Dot6 fnt_GetSingleWidthSlow(fnt_LocalGraphicStateType *gs);
void fnt_ChangeCvt(fnt_LocalGraphicStateType *gs, fnt_ElementType *element, ArrayIndex number, F26Dot6 delta);
void fnt_InnerTraceExecute(fnt_LocalGraphicStateType *gs, uint8 *ptr, uint8 *eptr);
void fnt_InnerExecute(fnt_LocalGraphicStateType *gs, uint8 *ptr, uint8 *eptr);
void fnt_Check_PF_Proj(fnt_LocalGraphicStateType *gs);
void fnt_ComputeAndCheck_PF_Proj(fnt_LocalGraphicStateType *gs);
Fract fnt_QuickDist(Fract dx, Fract dy);
void fnt_SetRoundValues(fnt_LocalGraphicStateType *gs, int arg1, int normalRound);
F26Dot6 fnt_CheckSingleWidth(F26Dot6 value, fnt_LocalGraphicStateType *gs);
fnt_instrDef* fnt_FindIDef(fnt_LocalGraphicStateType* gs, uint8 opCode);
void fnt_DeltaEngine(fnt_LocalGraphicStateType *gs, FntMoveFunc doIt, int16 base, int16 shift);
void fnt_DefaultJumpTable( voidFunc* function );

/* Actual instructions for the jump table */
void fnt_SVTCA_0(fnt_LocalGraphicStateType *gs);
void fnt_SVTCA_1(fnt_LocalGraphicStateType *gs);
void fnt_SPVTCA(fnt_LocalGraphicStateType *gs);
void fnt_SFVTCA(fnt_LocalGraphicStateType *gs);
void fnt_SPVTL(fnt_LocalGraphicStateType *gs);
void fnt_SDPVTL(fnt_LocalGraphicStateType *gs);
void fnt_SFVTL(fnt_LocalGraphicStateType *gs);
void fnt_SPVFS(fnt_LocalGraphicStateType *gs);
void fnt_SFVFS(fnt_LocalGraphicStateType *gs);
void fnt_GPV(fnt_LocalGraphicStateType *gs);
void fnt_GFV(fnt_LocalGraphicStateType *gs);
void fnt_SFVTPV(fnt_LocalGraphicStateType *gs);
void fnt_ISECT(fnt_LocalGraphicStateType *gs);
void fnt_SetLocalGraphicState(fnt_LocalGraphicStateType *gs);
void fnt_SetElementPtr(fnt_LocalGraphicStateType *gs);
void fnt_SetRoundState(fnt_LocalGraphicStateType *gs);
void fnt_SROUND(fnt_LocalGraphicStateType *gs);
void fnt_S45ROUND(fnt_LocalGraphicStateType *gs);
void fnt_SMD(fnt_LocalGraphicStateType *gs);
void fnt_RAW(fnt_LocalGraphicStateType *gs);
void fnt_WLSB(fnt_LocalGraphicStateType *gs);
void fnt_SCVTCI(fnt_LocalGraphicStateType *gs);
void fnt_SSWCI(fnt_LocalGraphicStateType *gs);
void fnt_SSW(fnt_LocalGraphicStateType *gs);
void fnt_DUP(fnt_LocalGraphicStateType *gs);
void fnt_POP(fnt_LocalGraphicStateType *gs);
void fnt_CLEAR(fnt_LocalGraphicStateType *gs);
void fnt_SWAP(fnt_LocalGraphicStateType *gs);
void fnt_DEPTH(fnt_LocalGraphicStateType *gs);
void fnt_CINDEX(fnt_LocalGraphicStateType *gs);
void fnt_MINDEX(fnt_LocalGraphicStateType *gs);
void fnt_ROLL( fnt_LocalGraphicStateType* gs );
void fnt_MDAP(fnt_LocalGraphicStateType *gs);
void fnt_MIAP(fnt_LocalGraphicStateType *gs);
void fnt_IUP(fnt_LocalGraphicStateType *gs);
void fnt_SHP(fnt_LocalGraphicStateType *gs);
void fnt_SHC(fnt_LocalGraphicStateType *gs);
void fnt_SHZ(fnt_LocalGraphicStateType *gs);
void fnt_SHPIX(fnt_LocalGraphicStateType *gs);
void fnt_IP(fnt_LocalGraphicStateType *gs);
void fnt_MSIRP(fnt_LocalGraphicStateType *gs);
void fnt_ALIGNRP(fnt_LocalGraphicStateType *gs);
void fnt_ALIGNPTS(fnt_LocalGraphicStateType *gs);
void fnt_SANGW(fnt_LocalGraphicStateType *gs);
void fnt_FLIPPT(fnt_LocalGraphicStateType *gs);
void fnt_FLIPRGON(fnt_LocalGraphicStateType *gs);
void fnt_FLIPRGOFF(fnt_LocalGraphicStateType *gs);
void fnt_SCANCTRL(fnt_LocalGraphicStateType *gs);
void fnt_SCANTYPE(fnt_LocalGraphicStateType *gs);
void fnt_INSTCTRL(fnt_LocalGraphicStateType *gs);
void fnt_AA(fnt_LocalGraphicStateType *gs);
void fnt_NPUSHB(fnt_LocalGraphicStateType *gs);
void fnt_NPUSHW(fnt_LocalGraphicStateType *gs);
void fnt_WS(fnt_LocalGraphicStateType *gs);
void fnt_RS(fnt_LocalGraphicStateType *gs);
void fnt_WCVTP(fnt_LocalGraphicStateType *gs);
void fnt_WCVTF(fnt_LocalGraphicStateType *gs);
void fnt_RCVT(fnt_LocalGraphicStateType *gs);
void fnt_GC(fnt_LocalGraphicStateType *gs);
void fnt_SCFS(fnt_LocalGraphicStateType *gs);
void fnt_MD(fnt_LocalGraphicStateType *gs);
void fnt_MPPEM(fnt_LocalGraphicStateType *gs);
void fnt_MPS(fnt_LocalGraphicStateType *gs);
void fnt_GETINFO(fnt_LocalGraphicStateType* gs);
void fnt_FLIPON(fnt_LocalGraphicStateType *gs);
void fnt_FLIPOFF(fnt_LocalGraphicStateType *gs);
#ifndef NOT_ON_THE_MAC
#ifdef DEBUG
void fnt_DDT(int8 c, int32 n);
#endif
#endif
void fnt_DEBUG(fnt_LocalGraphicStateType *gs);
void fnt_SkipPushCrap(fnt_LocalGraphicStateType *gs);
void fnt_IF(fnt_LocalGraphicStateType *gs);
void fnt_ELSE( fnt_LocalGraphicStateType* gs );
void fnt_EIF(fnt_LocalGraphicStateType *gs);
void fnt_JMPR( fnt_LocalGraphicStateType* gs );
void fnt_JROT(fnt_LocalGraphicStateType *gs);
void fnt_JROF(fnt_LocalGraphicStateType *gs);
void fnt_BinaryOperand(fnt_LocalGraphicStateType*);
void fnt_UnaryOperand(fnt_LocalGraphicStateType*);
void fnt_ROUND(fnt_LocalGraphicStateType *gs);
void fnt_NROUND(fnt_LocalGraphicStateType *gs);
void fnt_PUSHB(fnt_LocalGraphicStateType *gs);
void fnt_PUSHW(fnt_LocalGraphicStateType *gs);
void fnt_MDRP(fnt_LocalGraphicStateType *gs);
void fnt_MIRP(fnt_LocalGraphicStateType *gs);
void fnt_CALL(fnt_LocalGraphicStateType *gs);
void fnt_FDEF(fnt_LocalGraphicStateType *gs);
void fnt_LOOPCALL(fnt_LocalGraphicStateType *gs);
void fnt_IDefPatch( fnt_LocalGraphicStateType* gs );
void fnt_IDEF( fnt_LocalGraphicStateType* gs );
void fnt_UTP(fnt_LocalGraphicStateType *gs);
void fnt_SDB(fnt_LocalGraphicStateType *gs);
void fnt_SDS(fnt_LocalGraphicStateType *gs);
void fnt_DELTAP1(fnt_LocalGraphicStateType *gs);
void fnt_DELTAP2(fnt_LocalGraphicStateType *gs);
void fnt_DELTAP3(fnt_LocalGraphicStateType *gs);
void fnt_DELTAC1(fnt_LocalGraphicStateType *gs);
void fnt_DELTAC2(fnt_LocalGraphicStateType *gs);
void fnt_DELTAC3(fnt_LocalGraphicStateType *gs);
void fnt_ADJUST( fnt_LocalGraphicStateType *gs );	/* <13> */

/*
 * We exit through here, when we detect serious errors.
 */
void fnt_Panic( fnt_LocalGraphicStateType* gs, int error )
{
	longjmp( gs->env, error ); /* Do a gracefull recovery  */
}


/***************************/



#define fnt_NextPt1( pt, elem, ctr )	( (pt) == elem->ep[(ctr)] ? elem->sp[(ctr)] : (pt)+1 )


/*
 * Illegal instruction panic
 */
static void fnt_IllegalInstruction(register fnt_LocalGraphicStateType *gs)
{
	fnt_Panic( gs, UNDEFINED_INSTRUCTION_ERR );
}

#define bitcount(a, count) \
{ \
	count = 0; \
	while (a) \
	{ \
		a >>= 1; \
		count++; \
	} \
}

static void fnt_Normalize(F26Dot6 x, F26Dot6 y, VECTOR* v)
{
	Fract x1, y1;
	/*
	 *	Since x and y are 26.6, and currently that means they are really 16.6,
	 *	when treated as Fract, they are 0.[8]22, so shift up to 0.30 for accuracy
	 */

	CHECK_RANGE(x, -32768L << 6, 32767L << 6);
	CHECK_RANGE(y, -32768L << 6, 32767L << 6);

	{
		int shift, count;
		F26Dot6 xx = x;
		F26Dot6 yy = y;
		
		if (xx < 0)	xx = -xx;
		if (yy < 0) yy = -yy;
		if (xx < yy) xx = yy;
		/*
		 *	0.5 <= max(x,y) < 1
		 */
		bitcount(xx, count); 
		shift = 8 * sizeof(Fract) - 2 - count;
		x1 = (Fract) x << shift;
		y1 = (Fract) y << shift;
	}
	{
		Fract length;
		if (length = FracSqrt( FracMul( x1, x1 ) + FracMul( y1, y1 ) ))	/*<16>*/
		{
			v->x = FIXROUND( FracDiv( x1, length ) );
			v->y = FIXROUND( FracDiv( y1, length ) );
		}
		else
			/* length is 0 set up the projection vector along the x axis <16>*/		
		{
			v->x = ONEVECTOR;
			v->y = 0;
		}
	}
}

/******************** BEGIN Rounding Routines ***************************/

/*
 * Internal rounding routine
 */
F26Dot6 fnt_RoundToDoubleGrid(register F26Dot6 xin, F26Dot6 engine, fnt_LocalGraphicStateType* gs)
{
#pragma unused(gs)
	register F26Dot6 x = xin;

    if ( x >= 0 ) {
	    x += engine;
		x += fnt_pixelSize/4;
		x &= ~(fnt_pixelSize/2-1);
	} else {
	    x = -x;
	    x += engine;
		x += fnt_pixelSize/4;
		x &= ~(fnt_pixelSize/2-1);
		x = -x;
	}
	if ( ((int32)(xin ^ x)) < 0 && xin ) {
		x = 0; /* The sign flipped, make zero */
	}
	return x;
}

/*
 * Internal rounding routine
 */
F26Dot6 fnt_RoundDownToGrid(register F26Dot6 xin, F26Dot6 engine, fnt_LocalGraphicStateType* gs)
{
#pragma unused(gs)
	register F26Dot6 x = xin;

    if ( x >= 0 ) {
	    x += engine;
		x &= ~(fnt_pixelSize-1);
	} else {
	    x = -x;
	    x += engine;
		x &= ~(fnt_pixelSize-1);
		x = -x;
	}
	if ( ((int32)(xin ^ x)) < 0 && xin ) {
		x = 0; /* The sign flipped, make zero */
	}
	return x;
}

/*
 * Internal rounding routine
 */
F26Dot6 fnt_RoundUpToGrid(register F26Dot6 xin, F26Dot6 engine, fnt_LocalGraphicStateType* gs)
{
#pragma unused(gs)
	register F26Dot6 x = xin;

    if ( x >= 0 ) {
	    x += engine;
		x += fnt_pixelSize - 1;
		x &= ~(fnt_pixelSize-1);
	} else {
	    x = -x;
	    x += engine;
		x += fnt_pixelSize - 1;
		x &= ~(fnt_pixelSize-1);
		x = -x;
	}
	if ( ((int32)(xin ^ x)) < 0 && xin ) {
		x = 0; /* The sign flipped, make zero */
	}
	return x;
}

/*
 * Internal rounding routine
 */
F26Dot6 fnt_RoundToGrid(register F26Dot6 xin, F26Dot6 engine, fnt_LocalGraphicStateType* gs)
{
#pragma unused(gs)
	register F26Dot6 x = xin;

    if ( x >= 0 ) {
	    x += engine;
		x += fnt_pixelSize/2;
		x &= ~(fnt_pixelSize-1);
	} else {
	    x = -x;
	    x += engine;
		x += fnt_pixelSize/2;
		x &= ~(fnt_pixelSize-1);
		x = -x;
	}
	if ( ((int32)(xin ^ x)) < 0 && xin ) {
		x = 0; /* The sign flipped, make zero */
	}
	return x;
}


/*
 * Internal rounding routine
 */
F26Dot6 fnt_RoundToHalfGrid(register F26Dot6 xin, F26Dot6 engine, fnt_LocalGraphicStateType* gs)
{
#pragma unused(gs)
	register F26Dot6 x = xin;

    if ( x >= 0 ) {
	    x += engine;
		x &= ~(fnt_pixelSize-1);
	    x += fnt_pixelSize/2;
	} else {
	    x = -x;
	    x += engine;
		x &= ~(fnt_pixelSize-1);
	    x += fnt_pixelSize/2;
		x = -x;
	}
	if ( ((int32)(xin ^ x)) < 0 && xin ) {
		x = xin > 0 ? fnt_pixelSize/2 : -fnt_pixelSize/2; /* The sign flipped, make equal to smallest valid value */
	}
	return x;
}

/*
 * Internal rounding routine
 */
F26Dot6 fnt_RoundOff(register F26Dot6 xin, F26Dot6 engine, fnt_LocalGraphicStateType* gs)
{
#pragma unused(gs)
	register F26Dot6 x = xin;

    if ( x >= 0 ) {
	    x += engine;
	} else {
	    x -= engine;
	}
	if ( ((int32)(xin ^ x)) < 0 && xin) {
		x = 0; /* The sign flipped, make zero */
	}
	return x;
}

/*
 * Internal rounding routine
 */
F26Dot6 fnt_SuperRound(register F26Dot6 xin, F26Dot6 engine, register fnt_LocalGraphicStateType *gs)
{
	register F26Dot6 x = xin;
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;

    if ( x >= 0 ) {
	    x += engine;
		x += pb->threshold - pb->phase;
		x &= pb->periodMask;
		x += pb->phase;
	} else {
	    x = -x;
	    x += engine;
		x += pb->threshold - pb->phase;
		x &= pb->periodMask;
		x += pb->phase;
		x = -x;
	}
	if ( ((int32)(xin ^ x)) < 0 && xin ) {
		x = xin > 0 ? pb->phase : -pb->phase; /* The sign flipped, make equal to smallest phase */
	}
	return x;
}

/*
 * Internal rounding routine
 */
F26Dot6 fnt_Super45Round(register F26Dot6 xin, F26Dot6 engine, register fnt_LocalGraphicStateType *gs)
{
	register F26Dot6 x = xin;
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;

    if ( x >= 0 ) {
	    x += engine;
		x += pb->threshold - pb->phase;
		x = FracDiv( x, pb->period45 );
		x  &= ~(fnt_pixelSize-1);
		x = FracMul( x, pb->period45 );
		x += pb->phase;
	} else {
	    x = -x;
	    x += engine;
		x += pb->threshold - pb->phase;
		x = FracDiv( x, pb->period45 );
		x  &= ~(fnt_pixelSize-1);
		x = FracMul( x, pb->period45 );
		x += pb->phase;
		x = -x;
	}
	if ( ((int32)(xin ^ x)) < 0 && xin ) {
		x = xin > 0 ? pb->phase : -pb->phase; /* The sign flipped, make equal to smallest phase */
	}
	return x;
}


/******************** END Rounding Routines ***************************/


/* 3-versions ************************************************************************/

/*
 * Moves the point in element by delta (measured against the projection vector)
 * along the freedom vector.
 */
static void fnt_MovePoint(register fnt_LocalGraphicStateType *gs,
							register fnt_ElementType *element,
							register ArrayIndex point,
							register F26Dot6 delta)
{
    register VECTORTYPE pfProj = gs->pfProj;
	register VECTORTYPE fx = gs->free.x;
	register VECTORTYPE fy = gs->free.y;

	CHECK_POINT( gs, element, point );
	
	if ( pfProj != ONEVECTOR )
	{
		if ( fx ) {
			element->x[point] += VECTORMULDIV( delta, fx, pfProj );
			element->f[point] |= XMOVED;
		}
		if ( fy ) {
			element->y[point] += VECTORMULDIV( delta, fy, pfProj );
			element->f[point] |= YMOVED;
		}
	}
	else
	{
		if ( fx ) {
			element->x[point] += VECTORMUL( delta, fx );
			element->f[point] |= XMOVED;
		}
		if ( fy ) {
			element->y[point] += VECTORMUL( delta, fy );
			element->f[point] |= YMOVED;
		}
	}
}

/*
 * For use when the projection and freedom vectors coincide along the x-axis.
 */
static void fnt_XMovePoint( fnt_LocalGraphicStateType* gs, fnt_ElementType* element, ArrayIndex point, register F26Dot6 delta )
{
#ifndef DEBUG
#pragma unused(gs)
#endif
	CHECK_POINT( gs, element, point );
	element->x[point] += delta;
	element->f[point] |= XMOVED;
}

/*
 * For use when the projection and freedom vectors coincide along the y-axis.
 */
static void fnt_YMovePoint( fnt_LocalGraphicStateType* gs, register fnt_ElementType *element, ArrayIndex point, F26Dot6 delta )
{
#ifndef DEBUG
#pragma unused(gs)
#endif
	CHECK_POINT( gs, element, point );
	element->y[point] += delta;
	element->f[point] |= YMOVED;
}

/*
 * projects x and y into the projection vector.
 */
static F26Dot6 fnt_Project(fnt_LocalGraphicStateType* gs, F26Dot6 x, F26Dot6 y)
{
    return( VECTORMUL( x, gs->proj.x ) + VECTORMUL( y, gs->proj.y ) );
}

/*
 * projects x and y into the old projection vector.
 */
static F26Dot6 fnt_OldProject(fnt_LocalGraphicStateType* gs, F26Dot6 x, F26Dot6 y)
{
    return( VECTORMUL( x, gs->oldProj.x ) + VECTORMUL( y, gs->oldProj.y ) );
}

/*
 * Projects when the projection vector is along the x-axis
 */
static F26Dot6 fnt_XProject(fnt_LocalGraphicStateType* gs, F26Dot6 x, F26Dot6 y)
{
#pragma unused(gs,y)
    return( x );
}

/*
 * Projects when the projection vector is along the y-axis
 */
static F26Dot6 fnt_YProject(fnt_LocalGraphicStateType* gs, F26Dot6 x, F26Dot6 y)
{
#pragma unused(gs,x)
    return( y );
}
/*************************************************************************/

/*** Compensation for Transformations ***/

/*
 * Internal support routine, keep this guy FAST!!!!!!!		<3>
 */
static Fixed fnt_GetCVTScale(register fnt_LocalGraphicStateType* gs)
{
	register VECTORTYPE pvx, pvy;
	register Fixed scale;
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	/* Do as few Math routines as possible to gain speed */

	pvx = gs->proj.x;
	pvy = gs->proj.y;
	if ( pvy ) {
		if ( pvx )
		{
			pvy = VECTORDOT( pvy, pvy );
			scale = VECTORMUL( globalGS->yStretch, pvy );
			pvx = VECTORDOT( pvx, pvx );
			return scale + VECTORMUL( globalGS->xStretch, pvx );
		}
		else	/* pvy == +1 or -1 */
			return globalGS->yStretch;
	}
	else	/* pvx == +1 or -1 */
		return globalGS->xStretch;
}


/*	Functions for function pointer in local graphic state
*/
static F26Dot6 fnt_GetCVTEntryFast(fnt_LocalGraphicStateType* gs, ArrayIndex n)
{
	CHECK_CVT2( gs, n );
 	return gs->globalGS->controlValueTable[ n ];
}

static F26Dot6 fnt_GetCVTEntrySlow(register fnt_LocalGraphicStateType *gs, ArrayIndex n)
{
	register Fixed scale;

	CHECK_CVT2( gs, n );
	scale = fnt_GetCVTScale( gs );
	return ( FixMul( gs->globalGS->controlValueTable[ n ], scale ) );
}

static F26Dot6 fnt_GetSingleWidthFast(register fnt_LocalGraphicStateType *gs)
{
 	return gs->globalGS->localParBlock.scaledSW;
}

/*
 *
 */
static F26Dot6 fnt_GetSingleWidthSlow(register fnt_LocalGraphicStateType *gs)
{
	register Fixed scale;

	scale = fnt_GetCVTScale( gs );
	return ( FixMul( gs->globalGS->localParBlock.scaledSW, scale ) );
}



/*************************************************************************/

static void fnt_ChangeCvt(fnt_LocalGraphicStateType* gs, fnt_ElementType* elem,
							ArrayIndex number, F26Dot6 delta)
{
#pragma unused(elem)
	CHECK_CVT( gs, number );
	gs->globalGS->controlValueTable[ number ] += delta;
}

/*
 * This is the tracing interpreter.
 */
static void fnt_InnerTraceExecute(register fnt_LocalGraphicStateType *gs, uint8 *ptr, register uint8 *eptr)
{
    register FntFunc* function;
	register uint8 *oldInsPtr;
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;
#ifdef DEBUG
	uint8 *oldInsBase = gs->globalGS->instructBase; /*<22>*/
#endif
	oldInsPtr = gs->insPtr;
	gs->insPtr = ptr;
	function = gs->globalGS->function;

	if ( !gs->TraceFunc ) return; /* so we exit properly out of CALL() */
#ifdef DEBUG
	gs->globalGS->instructBase = gs->insPtr; /*<22>*/
#endif

	while ( gs->insPtr < eptr ) {
		/* The interpreter does not use gs->roundToGrid, so set it here */
		if ( pb->RoundValue == fnt_RoundToGrid )
			gs->roundToGrid = 1;
		else if ( pb->RoundValue == fnt_RoundToHalfGrid )
			gs->roundToGrid = 0;
		else if ( pb->RoundValue == fnt_RoundToDoubleGrid )
			gs->roundToGrid = 2;
		else if ( pb->RoundValue == fnt_RoundDownToGrid )
			gs->roundToGrid = 3;
		else if ( pb->RoundValue == fnt_RoundUpToGrid )
			gs->roundToGrid = 4;
		else if ( pb->RoundValue == fnt_RoundOff )
			gs->roundToGrid = 5;
		else if ( pb->RoundValue == fnt_SuperRound )
			gs->roundToGrid = 6;
		else if ( pb->RoundValue == fnt_Super45Round )
			gs->roundToGrid = 7;
		else
			gs->roundToGrid = -1;

		gs->TraceFunc( gs );

		if ( !gs->TraceFunc ) break; /* in case the editor wants to exit */

		function[ gs->opCode = *gs->insPtr++ ]( gs );
	}
#ifdef DEBUG
	gs->globalGS->instructBase = oldInsBase; /*<22>*/
#endif
	gs->insPtr = oldInsPtr;
}

#ifdef DEBUG
#define LIMIT		65536L*64L

void CHECK_STATE( fnt_LocalGraphicStateType* );
void CHECK_STATE( fnt_LocalGraphicStateType *gs )
{
	fnt_ElementType* elem;
	F26Dot6* x;
	F26Dot6* y;
	int16 count;
	F26Dot6 xmin, xmax, ymin, ymax;

	if (!gs->globalGS->glyphProgram) return;

	elem = &gs->elements[1];
	x = elem->x;
	y = elem->y;
	count = elem->ep[elem->nc - 1];
	xmin = xmax = *x;
	ymin = ymax = *y;

	for (; count >= 0; --count)
	{
		if (*x < xmin)
			xmin = *x;
		else if (*x > xmax)
			xmax = *x;
		if (*y < ymin)
			ymin = *y;
		else if (*y > ymax)
			ymax = *y;
		x++, y++;
	}
	if (xmin < -LIMIT || xmax > LIMIT || ymin < -LIMIT || ymax > LIMIT)
		Debugger();
}
#else
	#define CHECK_STATE(gs)
#endif

/*
 * This is the fast non-tracing interpreter.
 */
static void fnt_InnerExecute(register fnt_LocalGraphicStateType *gs, uint8 *ptr, uint8 *eptr)
{
    register FntFunc* function;
	uint8 *oldInsPtr = gs->insPtr;
#ifdef DEBUG
	uint8 *oldInsBase = gs->globalGS->instructBase; /*<22>*/
#endif
	gs->insPtr = ptr;
#ifdef DEBUG
	gs->globalGS->instructBase = gs->insPtr; /*<22>*/
#endif
	function = gs->globalGS->function;

	CHECK_STATE( gs );
	while ( gs->insPtr < eptr )
	{
		function[ gs->opCode = *gs->insPtr++ ]( gs );
		CHECK_STATE( gs );
	}

	gs->insPtr = oldInsPtr;
#ifdef DEBUG
	gs->globalGS->instructBase = oldInsBase; /*<22>*/
#endif
}

extern void fnt_SVTCA_0();

#ifdef DEBUG
static int32 fnt_NilFunction(void);
static int32 fnt_NilFunction()
{
	Debugger();
	return 0;
}
#endif


/*** <13> support macros for Kanji adjust from here  ***/ 

/* Decrease the number of covered scanlines between yLo and yHi by 1 pixel.
If jMove is already set to jLo or jHi, then calculate the amount that edge
must be moved and put it in *amounP.  If jMove == -1, find the edge that
must be moved least and set the minimum movement to accomplish this in *amountP. 
*/
#define COMPRESS( yLo, yHi, amountP, jLo, jHi, jMove )			\
{																\
	subPixel moveHi = yHi - (floor( yHi - HALF ) + HALFM);		\
	subPixel moveLo = (floor( yLo + HALFM ) + HALFP) - yLo;		\
	if( jMove == jLo ) *amountP = moveLo;						\
	else if( jMove == jHi ) *amountP = -moveHi;					\
	else if( moveLo <= moveHi )									\
	{															\
		*amountP = moveLo;										\
		jMove = jLo;											\
	}															\
	else 														\
	{															\
		*amountP = -moveHi;										\
		jMove = jHi;											\
	}															\
}																\

/* Increase the number of covered scanlines between yLo and yHi by 1 pixel.
If jMove is already set to jLo or jHi, then calculate the amount that edge
must be moved and put it in *amounP.  If jMove == -1, find the edge that
must be moved least and set the minimum movement to accomplish this in *amountP. 
*/
#define EXPAND( yLo, yHi, amountP, jLo, jHi, jMove )			\
{																\
	subPixel moveHi = floor( yHi - HALF ) + HALF + PIXEL - yHi;	\
	subPixel moveLo = yLo - (floor( yLo + HALFM ) - HALF);		\
	if( jMove == jLo ) *amountP = -moveLo;						\
	else if( jMove == jHi ) *amountP = moveHi;					\
	else if( moveLo <= moveHi )									\
	{															\
		*amountP = -moveLo;										\
		jMove = jLo;											\
	}															\
	else 														\
	{															\
		*amountP = moveHi;										\
		jMove = jHi;											\
	}															\
}																\

/* Figure out whether width needs to be increased or decreased and which
 * edge to move to accomplish the task.  If jMove is already set, only figure
 * out how much to move it.
 */
#define CALCEDGE( target, j1, j2, coord, moveP, jMove )						\
{																			\
	subPixel scanHi, scanLo;												\
	pixel nPixels;															\
	subPixel yLo = coord[j1];												\
	subPixel yHi = coord[j2];												\
	index	 jLo = j1;														\
	index	 jHi = j2;														\
	if( yHi < yLo )															\
	{																		\
		subPixel temp = yHi;												\
		yHi = yLo;															\
		yLo = temp;															\
		jLo = j2;															\
		jHi = j1;															\
	}																		\
	scanHi = floor( yHi - HALF ) + HALF;									\
	scanLo = floor( yLo + HALFM ) + HALF;									\
	nPixels = (scanHi - scanLo + PIXEL) >> LG2PIXEL;						\
	if( nPixels == target ) jMove = -1;										\
	else if( nPixels > target )												\
		COMPRESS( yLo, yHi, moveP, jLo, jHi, jMove )						\
	else  EXPAND( yLo, yHi, moveP, jLo, jHi, jMove )						\
}																			\

/* Move all the points on the same stroke edge as jMove by movement. Also do
serif adjustment; move adjoining points that have coordinates between original
edge and moved edge to moved edge.
*/
#define MOVEEDGE( jMove, movement, coord, sp, ep, nContours, flags, direction )		\
{																	\
int nPoints;														\
int nc = nContours;													\
index start, finish, j;												\
register subPixel coordMin, coordMax;								\
register subPixel coordEdge = coord[jMove];							\
subPixel coordNew = coordEdge + movement;							\
if( coordEdge < coordNew )											\
{																	\
	coordMin = coordEdge;											\
	coordMax = coordNew;											\
}																	\
else 																\
{																	\
	coordMin = coordNew;											\
	coordMax = coordEdge;											\
}																	\
																	\
--nc;																\
while( jMove < sp[nc] ) --nc;										\
finish = ep[nc];													\
start  = sp[nc];													\
																	\
j = jMove;															\
nPoints = finish - start;											\
while( coord[j] >= coordMin && coord[j] <= coordMax && nPoints-- )	\
{																	\
	coord[j] = coordNew;											\
	flags[j] |= direction;											\
	if( ++j > finish ) j = start;									\
}																	\
																	\
j = jMove;															\
if( --j < start )	j = finish;										\
nPoints = finish - start;											\
while( coord[j] >= coordMin && coord[j] <= coordMax && nPoints-- )	\
{																	\
	coord[j] = coordNew;											\
	flags[j] |= direction;											\
	if( --j < start )	j = finish;									\
}																	\
}																	\
/*** <13> support macros for Kanji adjust to here  ***/ 


/*
 * Executes the font instructions.
 * This is the external interface to the interpreter.
 *
 * Parameter Description
 *
 * elements points to the character elements. Element 0 is always
 * reserved and not used by the actual character.
 *
 * ptr points at the first instruction.
 * eptr points to right after the last instruction
 *
 * globalGS points at the global graphics state
 *
 * TraceFunc is pointer to a callback functioned called with a pointer to the
 *		local graphics state if TraceFunc is not null.
 *
 * Note: The stuff globalGS is pointing at must remain intact
 *       between calls to this function.
 */
int fnt_Execute(fnt_ElementType *elements, uint8 *ptr, register uint8 *eptr,
							fnt_GlobalGraphicStateType *globalGS, voidFunc TraceFunc)
{
    fnt_LocalGraphicStateType GS;
	register fnt_LocalGraphicStateType *gs; /* the local graphics state pointer */
	register int result;

	gs = &GS;
	gs->globalGS = globalGS;

	gs->elements = elements;
	gs->Pt0 = gs->Pt1 = gs->Pt2 = 0;
	gs->CE0 = gs->CE1 = gs->CE2 = &elements[1];
    gs->free.x = gs->proj.x = gs->oldProj.x = ONEVECTOR;
    gs->free.y = gs->proj.y = gs->oldProj.y = 0;
	gs->pfProj = ONEVECTOR;
	gs->MovePoint = fnt_XMovePoint;
	gs->Project   = fnt_XProject;
	gs->OldProject = fnt_XProject;
	gs->loop = 0;		/* 1 less than count for faster loops. mrr */

	if ( globalGS->pgmIndex == FONTPROGRAM )
	{
#ifdef DEBUG
		gs->GetCVTEntry = fnt_NilFunction;
		gs->GetSingleWidth = fnt_NilFunction;
#endif
		goto ASSIGN_POINTERS;
	}

	if ( globalGS->pixelsPerEm <= 1 )
		return NO_ERR;
	if ( globalGS->identityTransformation ) {
		gs->GetCVTEntry = fnt_GetCVTEntryFast;
		gs->GetSingleWidth = fnt_GetSingleWidthFast;
	} else {
		gs->GetCVTEntry = fnt_GetCVTEntrySlow;
		gs->GetSingleWidth = fnt_GetSingleWidthSlow;
		if ( FixMul( globalGS->fpem, globalGS->xStretch ) <= ONEFIX ||
			 FixMul( globalGS->fpem, globalGS->yStretch ) <= ONEFIX )
			return NO_ERR;
	}

	if ( globalGS->init ) {
#ifndef NO_CHECK_TRASHED_MEMORY
#ifdef CLEANMACHINE
		if ( globalGS->function[ 0x00 ] != fnt_SVTCA_0 ) {
#else
		/* Clean off high byte for checking .... */
		if ( ((int32)globalGS->function[ 0x00 ] & 0x00ffffff) != ((int32)fnt_SVTCA_0 & 0x00ffffff) ) {
#endif
			/* Who trashed my memory ???? */
			return( TRASHED_MEM_ERR  );
		}
#endif
	} else if ( globalGS->localParBlock.sW ) {
	    /* We need to scale the single width for this size  */
		globalGS->localParBlock.scaledSW = globalGS->ScaleFunc( globalGS, globalGS->localParBlock.sW );
	}

ASSIGN_POINTERS:

	gs->stackPointer = globalGS->stackBase;
	if ( result = setjmp(gs->env) )		return( result );	/* got an error */

	/* remove obsolete check for angle info being in right place <19> */

    (gs->Interpreter = (gs->TraceFunc = TraceFunc) ?
		fnt_InnerTraceExecute : fnt_InnerExecute)( gs, ptr, eptr );

	return NO_ERR;
}


/*************************************************************************/

/*** 2 internal gs->pfProj computation support routines ***/

/*
 * Only does the check of gs->pfProj
 */
static void fnt_Check_PF_Proj(fnt_LocalGraphicStateType *gs)
{
	register VECTORTYPE pfProj = gs->pfProj;

	if ( pfProj > -ONESIXTEENTHVECTOR && pfProj < ONESIXTEENTHVECTOR) {
		gs->pfProj = pfProj < 0 ? -ONEVECTOR : ONEVECTOR; /* Prevent divide by small number */
	}
} 									/*<23>*/


/*
 * Computes gs->pfProj and then does the check
 */
static void fnt_ComputeAndCheck_PF_Proj(register fnt_LocalGraphicStateType *gs)
{
	register VECTORTYPE pfProj;

	pfProj = VECTORDOT( gs->proj.x, gs->free.x ) + VECTORDOT( gs->proj.y, gs->free.y );
	if ( pfProj > -ONESIXTEENTHVECTOR && pfProj < ONESIXTEENTHVECTOR) {
		pfProj = pfProj < 0 ? -ONEVECTOR : ONEVECTOR; /* Prevent divide by small number */
	}
	gs->pfProj = pfProj;
}                                /*<23>*/



/******************************************/
/******** The Actual Instructions *********/
/******************************************/

/*
 * Set Vectors To Coordinate Axis - Y
 */
static void fnt_SVTCA_0( register fnt_LocalGraphicStateType* gs )
{
	gs->free.x = gs->proj.x = 0;
	gs->free.y = gs->proj.y = ONEVECTOR;
	gs->MovePoint = fnt_YMovePoint;
	gs->Project = fnt_YProject;
	gs->OldProject = fnt_YProject;
	gs->pfProj = ONEVECTOR;
}

/*
 * Set Vectors To Coordinate Axis - X
 */
static void fnt_SVTCA_1( register fnt_LocalGraphicStateType* gs )
{
	gs->free.x = gs->proj.x = ONEVECTOR;
	gs->free.y = gs->proj.y = 0;
	gs->MovePoint = fnt_XMovePoint;
	gs->Project = fnt_XProject;
	gs->OldProject = fnt_XProject;
	gs->pfProj = ONEVECTOR;
}

/*
 * Set Projection Vector To Coordinate Axis
 */
static void fnt_SPVTCA( register fnt_LocalGraphicStateType* gs )
{
	if ( BIT0( gs->opCode )  ) {
		gs->proj.x = ONEVECTOR;
		gs->proj.y = 0;
		gs->Project = fnt_XProject;
		gs->pfProj = gs->free.x;
	} else {
		gs->proj.x = 0;
		gs->proj.y = ONEVECTOR;
		gs->Project = fnt_YProject;
		gs->pfProj = gs->free.y;
	}
	fnt_Check_PF_Proj( gs );
	gs->MovePoint = fnt_MovePoint;
	gs->OldProject = gs->Project;
}

/*
 * Set Freedom Vector to Coordinate Axis
 */
static void fnt_SFVTCA( register fnt_LocalGraphicStateType* gs )
{
	if ( BIT0( gs->opCode )  ) {
		gs->free.x = ONEVECTOR;
		gs->free.y = 0;
		gs->pfProj = gs->proj.x;
	} else {
		gs->free.x = 0;
		gs->free.y = ONEVECTOR;
		gs->pfProj = gs->proj.y;
	}
	fnt_Check_PF_Proj( gs );
	gs->MovePoint = fnt_MovePoint;
}

/*
 * Set Projection Vector To Line
 */
static void fnt_SPVTL(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex arg1, arg2;

	arg2 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	arg1 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_POINT( gs, gs->CE2, arg2 );
	CHECK_POINT( gs, gs->CE1, arg1 );

	fnt_Normalize( gs->CE1->x[arg1] - gs->CE2->x[arg2], gs->CE1->y[arg1] - gs->CE2->y[arg2], &gs->proj );
	if ( BIT0( gs->opCode ) ) {
		/* rotate 90 degrees */
		VECTORTYPE tmp	= gs->proj.y;
		gs->proj.y		= gs->proj.x;
		gs->proj.x		= -tmp;
	}
	fnt_ComputeAndCheck_PF_Proj( gs );
	gs->MovePoint = fnt_MovePoint;
	gs->Project = fnt_Project;
	gs->OldProject = gs->Project;
}


/*
 * Set Dual Projection Vector To Line
 */
static void fnt_SDPVTL(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex arg1, arg2;

	arg2 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	arg1 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_POINT( gs, gs->CE2, arg2 );
	CHECK_POINT( gs, gs->CE1, arg1 );

	/* Do the current domain */
	fnt_Normalize( gs->CE1->x[arg1] - gs->CE2->x[arg2], gs->CE1->y[arg1] - gs->CE2->y[arg2], &gs->proj );

	/* Do the old domain */
	fnt_Normalize( gs->CE1->ox[arg1] - gs->CE2->ox[arg2], gs->CE1->oy[arg1] - gs->CE2->oy[arg2], &gs->oldProj );

	if ( BIT0( gs->opCode ) ) {
		/* rotate 90 degrees */
		VECTORTYPE tmp	= gs->proj.y;
		gs->proj.y		= gs->proj.x;
		gs->proj.x		= -tmp;

		tmp				= gs->oldProj.y;
		gs->oldProj.y	= gs->oldProj.x;
		gs->oldProj.x	= -tmp;
	}
	fnt_ComputeAndCheck_PF_Proj( gs );

	gs->MovePoint = fnt_MovePoint;
	gs->Project = fnt_Project;
	gs->OldProject = fnt_OldProject;
}

/*
 * Set Freedom Vector To Line
 */
static void fnt_SFVTL(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex arg1, arg2;

	arg2 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	arg1 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_POINT( gs, gs->CE2, arg2 );
	CHECK_POINT( gs, gs->CE1, arg1 );

	fnt_Normalize( gs->CE1->x[arg1] - gs->CE2->x[arg2], gs->CE1->y[arg1] - gs->CE2->y[arg2], &gs->free );
	if ( BIT0( gs->opCode ) ) {
		/* rotate 90 degrees */
		VECTORTYPE tmp	= gs->free.y;
		gs->free.y		= gs->free.x;
		gs->free.x		= -tmp;
	}
	fnt_ComputeAndCheck_PF_Proj( gs );
	gs->MovePoint = fnt_MovePoint;
}


/*
 * Write Projection Vector
 */
static void fnt_SPVFS(register fnt_LocalGraphicStateType *gs)
{
	gs->proj.y = (VECTORTYPE)CHECK_POP(gs, gs->stackPointer);
	gs->proj.x = (VECTORTYPE)CHECK_POP(gs, gs->stackPointer);

	fnt_ComputeAndCheck_PF_Proj( gs );

	gs->MovePoint = fnt_MovePoint;
	gs->Project = fnt_Project;
	gs->OldProject = gs->Project;
}

/*
 * Write Freedom vector
 */
static void fnt_SFVFS(register fnt_LocalGraphicStateType *gs)
{
	gs->free.y = (VECTORTYPE)CHECK_POP(gs, gs->stackPointer);
	gs->free.x = (VECTORTYPE)CHECK_POP(gs, gs->stackPointer);

	fnt_ComputeAndCheck_PF_Proj( gs );

	gs->MovePoint = fnt_MovePoint;
}

/*
 * Read Projection Vector
 */
static void fnt_GPV(register fnt_LocalGraphicStateType *gs)
{
	CHECK_PUSH( gs, gs->stackPointer, gs->proj.x );
	CHECK_PUSH( gs, gs->stackPointer, gs->proj.y );
}

/*
 * Read Freedom Vector
 */
static void fnt_GFV(register fnt_LocalGraphicStateType *gs)
{
	CHECK_PUSH( gs, gs->stackPointer, gs->free.x );
	CHECK_PUSH( gs, gs->stackPointer, gs->free.y );
}

/*
 * Set Freedom Vector To Projection Vector
 */
static void fnt_SFVTPV(register fnt_LocalGraphicStateType *gs)
{
	gs->free = gs->proj;
	gs->pfProj = ONEVECTOR;
	gs->MovePoint = fnt_MovePoint;
}

/*
 * fnt_ISECT()
 *
 * Computes the intersection of two lines without using floating point!!
 *
 * (1) Bx + dBx * t0 = Ax + dAx * t1
 * (2) By + dBy * t0 = Ay + dAy * t1
 *
 *  1  =>  (t1 = Bx - Ax + dBx * t0 ) / dAx
 *  +2 =>   By + dBy * t0 = Ay + dAy/dAx * [ Bx - Ax + dBx * t0 ]
 *     => t0 * [dAy/dAx * dBx - dBy = By - Ay - dAy/dAx*(Bx-Ax)
 *     => t0(dAy*DBx - dBy*dAx) = dAx(By - Ay) + dAy(Ax-Bx)
 *     => t0 = [dAx(By-Ay) + dAy(Ax-Bx)] / [dAy*dBx - dBy*dAx]
 *     => t0 = [dAx(By-Ay) - dAy(Bx-Ax)] / [dBx*dAy - dBy*dAx]
 *     t0 = N/D
 *     =>
 *	    N = (By - Ay) * dAx - (Bx - Ax) * dAy;
 *		D = dBx * dAy - dBy * dAx;
 *      A simple floating point implementation would only need this, and
 *      the check to see if D is zero.
 *		But to gain speed we do some tricks and avoid floating point.
 *
 */
static void fnt_ISECT(fnt_LocalGraphicStateType *gs)
{
	register F26Dot6 N, D;
	register Fract t;
	register ArrayIndex arg1, arg2;
	F26Dot6 Bx, By, Ax, Ay;
	F26Dot6 dBx, dBy, dAx, dAy;

	{
		register fnt_ElementType* element = gs->CE0;
		register F26Dot6* stack = gs->stackPointer;

		arg2 = (ArrayIndex)CHECK_POP(gs, stack ); /* get one line */
		arg1 = (ArrayIndex)CHECK_POP(gs, stack );
		dAx = element->x[arg2] - (Ax = element->x[arg1]);
		dAy = element->y[arg2] - (Ay = element->y[arg1]);

		element = gs->CE1;
		arg2 = (ArrayIndex)CHECK_POP(gs, stack ); /* get the other line */
		arg1 = (ArrayIndex)CHECK_POP(gs, stack );
		dBx = element->x[arg2] - (Bx = element->x[arg1]);
		dBy = element->y[arg2] - (By = element->y[arg1]);

		arg1 = (ArrayIndex)CHECK_POP(gs, stack ); /* get the point number */
		gs->stackPointer = stack;
	}
	gs->CE2->f[arg1] |= XMOVED | YMOVED;
	{
		register F26Dot6* elementx = gs->CE2->x;
		register F26Dot6* elementy = gs->CE2->y;
		if ( dAy == 0 ) {
			if ( dBx == 0 ) {
				elementx[arg1] = Bx;
				elementy[arg1] = Ay;
				return;
			}
			N = By - Ay;
			D = -dBy;
		} else if ( dAx == 0 ) {
			if ( dBy == 0 ) {
				elementx[arg1] = Ax;
				elementy[arg1] = By;
				return;
			}
			N = Bx - Ax;
			D = -dBx;
		} else if ( MABS( dAx ) > MABS( dAy ) ) {
			/* To prevent out of range problems divide both N and D with the max */
			t = FracDiv( dAy, dAx );
			N = (By - Ay) - FracMul( (Bx - Ax), t );
			D = FracMul( dBx, t ) - dBy;
		} else {
			t = FracDiv( dAx, dAy );
			N = FracMul( (By - Ay), t ) - (Bx - Ax);
			D = dBx - FracMul( dBy, t );
		}

		if ( D ) {
			if ( MABS( N ) < MABS( D ) ) {
				/* this is the normal case */
				t = FracDiv( N, D );
				elementx[arg1] = Bx + FracMul( dBx, t );
				elementy[arg1] = By + FracMul( dBy, t );
			} else {
				if ( N ) {
					/* Oh well, invert t and use it instead */
					t = FracDiv( D, N );
					elementx[arg1] = Bx + FracDiv( dBx, t );
					elementy[arg1] = By + FracDiv( dBy, t );
				} else {
					elementx[arg1] = Bx;
					elementy[arg1] = By;
				}
			}
		} else {
			/* degenerate case: parallell lines, put point in the middle */
			elementx[arg1] = (Bx + (dBx>>1) + Ax + (dAx>>1)) >> 1;
			elementy[arg1] = (By + (dBy>>1) + Ay + (dAy>>1)) >> 1;
		}
	}
}

/*
 * Load Minimum Distanc
 */
static void fnt_SMD(register fnt_LocalGraphicStateType *gs)
{
	gs->globalGS->localParBlock.minimumDistance = CHECK_POP(gs, gs->stackPointer );
}

/*
 * Load Control Value Table Cut In
 */
static void fnt_SCVTCI(register fnt_LocalGraphicStateType *gs)
{
	gs->globalGS->localParBlock.wTCI = CHECK_POP(gs, gs->stackPointer );
}

/*
 * Load Single Width Cut In
 */
static void fnt_SSWCI(register fnt_LocalGraphicStateType *gs)
{
	gs->globalGS->localParBlock.sWCI = CHECK_POP(gs, gs->stackPointer );
}

/*
 * Load Single Width , assumes value comes from the original domain, not the cvt or outline
 */
static void fnt_SSW(register fnt_LocalGraphicStateType *gs)
{
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	register fnt_ParameterBlock *pb = &globalGS->localParBlock;

	pb->sW = (int16)CHECK_POP(gs, gs->stackPointer );

	pb->scaledSW = globalGS->ScaleFunc( globalGS, pb->sW ); /* measurement should not come from the outline */
}

static void fnt_SetLocalGraphicState(register fnt_LocalGraphicStateType *gs)
{
	int arg = (int)CHECK_POP(gs, gs->stackPointer );

	switch (gs->opCode) {
	case SRP0_CODE:	 gs->Pt0 = (ArrayIndex)arg; break;
	case SRP1_CODE:	 gs->Pt1 = (ArrayIndex)arg; break;
	case SRP2_CODE:	 gs->Pt2 = (ArrayIndex)arg; break;

	case SLOOP_CODE: gs->loop = (LoopCount)arg - 1; break;

	case POP_CODE: break;
#ifdef DEBUG
	default:
		Debugger();
		break;
#endif
	}
}

static void fnt_SetElementPtr(register fnt_LocalGraphicStateType *gs)
{
	ArrayIndex arg = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	fnt_ElementType* element = &gs->elements[ arg ];

	CHECK_ELEMENT( gs, arg );

	switch (gs->opCode) {
	case SCES_CODE: gs->CE2 = element;
					gs->CE1 = element;
	case SCE0_CODE: gs->CE0 = element; break;
	case SCE1_CODE: gs->CE1 = element; break;
	case SCE2_CODE: gs->CE2 = element; break;
#ifdef DEBUG
	default:
		Debugger();
		break;
#endif
	}
}

/*
 * Super Round
 */
static void fnt_SROUND(register fnt_LocalGraphicStateType *gs)
{
	register int arg1 = (int)CHECK_POP(gs, gs->stackPointer );
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;

	fnt_SetRoundValues( gs, arg1, true );
	pb->RoundValue = fnt_SuperRound;
}

/*
 * Super Round
 */
static void fnt_S45ROUND(register fnt_LocalGraphicStateType *gs)
{
	register int arg1 = (int)CHECK_POP(gs, gs->stackPointer );
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;

	fnt_SetRoundValues( gs, arg1, false );
	pb->RoundValue = fnt_Super45Round;
}

/*
 *	These functions just set a field of the graphics state
 *	They pop no arguments
 */
static void fnt_SetRoundState(register fnt_LocalGraphicStateType *gs)
{
	register FntRoundFunc *rndFunc = &gs->globalGS->localParBlock.RoundValue;

	switch (gs->opCode) {
	case RTG_CODE:  *rndFunc = fnt_RoundToGrid; break;
	case RTHG_CODE: *rndFunc = fnt_RoundToHalfGrid; break;
	case RTDG_CODE: *rndFunc = fnt_RoundToDoubleGrid; break;
	case ROFF_CODE: *rndFunc = fnt_RoundOff; break;
	case RDTG_CODE: *rndFunc = fnt_RoundDownToGrid; break;
	case RUTG_CODE: *rndFunc = fnt_RoundUpToGrid; break;
#ifdef DEBUG
	default:
		Debugger();
		break;
#endif
	}
}


#define FRACSQRT2DIV2 759250125
/*
 * Internal support routine for the super rounding routines
 */
static void fnt_SetRoundValues(register fnt_LocalGraphicStateType *gs,
								register int arg1, register int normalRound)
{
	register int tmp;
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;

	tmp = arg1 & 0xC0;

	if ( normalRound ) {
		switch ( tmp ) {
		case 0x00:
			pb->period = fnt_pixelSize/2;
			break;
		case 0x40:
			pb->period = fnt_pixelSize;
			break;
		case 0x80:
			pb->period = fnt_pixelSize*2;
			break;
		default:
			pb->period = 999; /* Illegal */
		}
		pb->periodMask = ~(pb->period-1);
	} else {
		pb->period45 = FRACSQRT2DIV2;
		switch ( tmp ) {
		case 0x00:
			pb->period45 >>= 1;
			break;
		case 0x40:
			break;
		case 0x80:
			pb->period45 <<= 1;
			break;
		default:
			pb->period45 = 999; /* Illegal */
		}
		tmp = (sizeof(Fract) * 8 - 2 - fnt_pixelShift);
		pb->period = (int16)((pb->period45 + (1L<<(tmp-1))) >> tmp); /*convert from 2.30 to 26.6 */
	}

	tmp = arg1 & 0x30;
	switch ( tmp ) {
	case 0x00:
		pb->phase = 0;
		break;
	case 0x10:
		pb->phase = (pb->period + 2) >> 2;
		break;
	case 0x20:
		pb->phase = (pb->period + 1) >> 1;
		break;
	case 0x30:
		pb->phase = (pb->period + pb->period + pb->period + 2) >> 2;
		break;
	}
	tmp = arg1 & 0x0f;
	if ( tmp == 0 ) {
		pb->threshold = pb->period-1;
	} else {
		pb->threshold = ((tmp - 4) * pb->period + 4) >> 3;
	}
}

/*
 * Read Advance Width
 */
static void fnt_RAW(register fnt_LocalGraphicStateType *gs)
{
	fnt_ElementType* elem = &gs->elements[1];
	F26Dot6* ox = elem->ox;
	ArrayIndex index = elem->ep[elem->nc - 1] + 1;		/* lsb point */

	CHECK_PUSH( gs, gs->stackPointer, ox[index+1] - ox[index] );
}

/*
 * DUPlicate
 */
static void fnt_DUP(register fnt_LocalGraphicStateType *gs)
{
	F26Dot6 top = gs->stackPointer[-1];
	CHECK_PUSH( gs, gs->stackPointer, top);
}

/*
 * CLEAR stack
 */
static void fnt_CLEAR(register fnt_LocalGraphicStateType *gs)
{
	gs->stackPointer = gs->globalGS->stackBase;
}

/*
 * SWAP
 */
static void fnt_SWAP(register fnt_LocalGraphicStateType *gs)
{
	register F26Dot6* stack = gs->stackPointer;
	register F26Dot6 arg2 = CHECK_POP(gs, stack );
	register F26Dot6 arg1 = CHECK_POP(gs, stack );

	CHECK_PUSH( gs, stack, arg2 );
	CHECK_PUSH( gs, stack, arg1 );
}

/*
 * DEPTH
 */
static void fnt_DEPTH(register fnt_LocalGraphicStateType *gs)
{
	F26Dot6 depth = gs->stackPointer - gs->globalGS->stackBase;
	CHECK_PUSH( gs, gs->stackPointer, depth);
}

/*
 * Copy INDEXed value
 */
static void fnt_CINDEX(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex arg1;
	register F26Dot6 tmp;
	register F26Dot6* stack = gs->stackPointer;

	arg1 = (ArrayIndex)CHECK_POP(gs, stack );
	tmp = *(stack - arg1 );
	CHECK_PUSH( gs, stack , tmp );
}

/*
 * Move INDEXed value
 */
static void fnt_MINDEX(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex arg1;
	register F26Dot6 tmp, *p;
	register F26Dot6* stack = gs->stackPointer;

	arg1 = (ArrayIndex)CHECK_POP(gs, stack );
	tmp = *(p = (stack - arg1));
	if ( arg1 ) {
		do {
			*p = *(p + 1); p++;
		} while ( --arg1 );
		CHECK_POP(gs, stack );
	}
	CHECK_PUSH( gs, stack, tmp );
	gs->stackPointer = stack;
}

/*
 *	Rotate element 3 to the top of the stack			<4>
 *	Thanks to Oliver for the obscure code.
 */
static void fnt_ROLL( register fnt_LocalGraphicStateType* gs )
{
	register F26Dot6 *stack = gs->stackPointer;
	register F26Dot6 element1 = *--stack;
	register F26Dot6 element2 = *--stack;
	*stack = element1;
	element1 = *--stack;
	*stack = element2;
	*(stack + 2) = element1;
}

/*
 * Move Direct Absolute Point
 */
static void fnt_MDAP(register fnt_LocalGraphicStateType *gs)
{
    register F26Dot6 proj;
	register fnt_ElementType* ce0 = gs->CE0;
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;
	register ArrayIndex ptNum;

	ptNum = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	gs->Pt0 = gs->Pt1 = ptNum;

	if ( BIT0( gs->opCode ) )
	{
		proj = gs->Project( gs, ce0->x[ptNum], ce0->y[ptNum] );
		proj = pb->RoundValue( proj, gs->globalGS->engine[0], gs ) - proj;
	}
	else
		proj = 0;		/* mark the point as touched */

	gs->MovePoint( gs, ce0, ptNum, proj );
}

/*
 * Move Indirect Absolute Point
 */
static void fnt_MIAP(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex ptNum;
	register F26Dot6 newProj, origProj;
	register fnt_ElementType* ce0 = gs->CE0;
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;

	newProj = gs->GetCVTEntry( gs, CHECK_POP(gs, gs->stackPointer ) );
	ptNum = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_POINT(gs, ce0, ptNum);
	gs->Pt0 = gs->Pt1 = ptNum;

	if ( ce0 == gs->elements )		/* twilightzone */
	{
		ce0->x[ptNum] = ce0->ox[ptNum] = VECTORMUL( newProj, gs->proj.x );
		ce0->y[ptNum] = ce0->oy[ptNum] = VECTORMUL( newProj, gs->proj.y );
	}

	origProj = gs->Project( gs, ce0->x[ptNum], ce0->y[ptNum] );

	if ( BIT0( gs->opCode ) )
	{
		register F26Dot6 tmp = newProj - origProj;
		if ( tmp < 0 )
			tmp = -tmp;
		if ( tmp > pb->wTCI )
			newProj = origProj;
		newProj = pb->RoundValue( newProj, gs->globalGS->engine[0], gs );
	}

	newProj -= origProj;
	gs->MovePoint( gs, ce0, ptNum, newProj );
}

/*
 * Interpolate Untouched Points
 */
static void fnt_IUP(fnt_LocalGraphicStateType *gs)
{
	register fnt_ElementType* CE2 = gs->CE2;
	register int32 tmp32B;
	F26Dot6 *coord, *oCoord, *ooCoord;
    LoopCount ctr;
	F26Dot6 dx, dx1, dx2;
	F26Dot6 dlow, dhigh;
	F26Dot6 tmp32, high, low;
	int mask;
	ArrayIndex tmp16B;

	if ( gs->opCode & 0x01 ) {
		/* do x */
		coord = CE2->x;
		oCoord = CE2->ox;
		ooCoord = CE2->oox;
		mask = XMOVED;
	} else {
		/* do y */
		coord = CE2->y;
		oCoord = CE2->oy;
		ooCoord = CE2->ooy;
		mask = YMOVED;
	}
	for ( ctr = 0; ctr < CE2->nc; ctr++ )
	{
		ArrayIndex start = CE2->sp[ctr];
		int16 tmp16 = CE2->ep[ctr];
		while( !(CE2->f[start] & mask) && start <= tmp16 )
			start++;
		if ( start > tmp16 )
			continue;
		tmp16B = start;
		do {
			ArrayIndex end;
			tmp16 = end = fnt_NextPt1( start, CE2, ctr );
			while( !(CE2->f[end] & mask) ) {
				end = fnt_NextPt1( end, CE2, ctr );
				if ( start == end )
					break;
			}

			if ( ooCoord[start] < ooCoord[end] ) {
				dx  = coord[start];
				dx1 = oCoord[start];
				dx2 = ooCoord[start];
				high = oCoord[end];
				dhigh = coord[end] - high;
				tmp32  = coord[end] - dx;
				tmp32B = ooCoord[end] - dx2;
			} else {
				dx  = coord[end];
				dx1 = oCoord[end];
				dx2 = ooCoord[end];
				high = oCoord[start];
				dhigh = coord[start] - high;
				tmp32  = coord[start] - dx;
				tmp32B = ooCoord[start] - dx2;
			}
			low = dx1;
			dlow = dx - dx1;

			if ( tmp32B ) {
				if ( tmp32B < 32768 && tmp32 < 32768 )
				{
					F26Dot6 corr = tmp32B >> 1;
					while ( tmp16 != end )
					{
						F26Dot6 tmp32C = oCoord[tmp16];
						if ( tmp32C <= low )
							tmp32C += dlow;
						else if ( tmp32C >= high )
							tmp32C += dhigh;
						else
						{
							tmp32C = ooCoord[tmp16];
							tmp32C -= dx2;
							tmp32C  = SHORTMUL(tmp32C, tmp32);
							tmp32C += corr;
							if ( tmp32C < 32768 )
							    tmp32C = SHORTDIV(tmp32C, tmp32B);
							else
							    tmp32C /= (int16)tmp32B;
							tmp32C += dx;
						}
						coord[tmp16] = tmp32C;
						tmp16 = fnt_NextPt1( tmp16, CE2, ctr);
					}
				}
				else
				{
					Fixed ratio;
				    int firstTime = true;
					while ( tmp16 != end )
					{
						F26Dot6 tmp32C = oCoord[tmp16];
						if ( tmp32C <= low )
							tmp32C += dlow;
						else if ( tmp32C >= high )
							tmp32C += dhigh;
						else
						{
						    if ( firstTime )
							{
						        ratio = FixDiv( tmp32, tmp32B );
								firstTime = 0;
						    }
							tmp32C = ooCoord[tmp16];
							tmp32C -= dx2;
							tmp32C = FixMul( tmp32C, ratio );
							tmp32C += dx;
						}
						coord[tmp16] = tmp32C;
						tmp16 = fnt_NextPt1( tmp16, CE2, ctr);
					}
				}
			} else {
				while ( tmp16 != end ) {
					coord[tmp16] += dx - dx1;
					tmp16 = fnt_NextPt1( tmp16, CE2, ctr);
				}
			}
			start = end;
		} while ( start != tmp16B );
	}

}

static fnt_ElementType* fnt_SH_Common(fnt_LocalGraphicStateType* gs, F26Dot6* dx, F26Dot6* dy, ArrayIndex* point)
{
	F26Dot6 proj;
	ArrayIndex pt;
	fnt_ElementType* element;

	if ( BIT0( gs->opCode ) ) {
		pt = gs->Pt1;
		element = gs->CE0;
	} else {
		pt = gs->Pt2;
		element = gs->CE1;
	}
	proj = gs->Project( gs, element->x[pt] - element->ox[pt],
                            element->y[pt] - element->oy[pt] );

	if ( gs->pfProj != ONEVECTOR )
	{
		if ( gs->free.x )
			*dx = VECTORMULDIV( proj, gs->free.x, gs->pfProj );
		if ( gs->free.y )
			*dy = VECTORMULDIV( proj, gs->free.y, gs->pfProj );
	}
	else
	{
		if ( gs->free.x )
			*dx = VECTORMUL( proj, gs->free.x );
		if ( gs->free.y )
			*dy = VECTORMUL( proj, gs->free.y );
	}
	*point = pt;
	return element;
}

static void fnt_SHP_Common(fnt_LocalGraphicStateType *gs, F26Dot6 dx, F26Dot6 dy)
{
	register fnt_ElementType* CE2 = gs->CE2;
	register LoopCount count = gs->loop;
	for (; count >= 0; --count)
	{
		ArrayIndex point = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
		if ( gs->free.x ) {
			CE2->x[point] += dx;
			CE2->f[point] |= XMOVED;
		}
		if ( gs->free.y ) {
			CE2->y[point] += dy;
			CE2->f[point] |= YMOVED;
		}
	}
	gs->loop = 0;
}

/*
 * SHift Point
 */
static void fnt_SHP(register fnt_LocalGraphicStateType *gs)
{
	F26Dot6 dx, dy;
	ArrayIndex point;

	fnt_SH_Common(gs, &dx, &dy, &point);
	fnt_SHP_Common(gs, dx, dy);
}

/*
 * SHift Contour
 */
static void fnt_SHC(register fnt_LocalGraphicStateType *gs)
{
    register fnt_ElementType *element;
	register F26Dot6 dx, dy;
	register ArrayIndex contour, point;

	{
		F26Dot6 x, y;
		ArrayIndex pt;
		element = fnt_SH_Common(gs, &x, &y, &pt);
		point = pt;
		dx = x;
		dy = y;
	}
    contour = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_CONTOUR(gs, gs->CE2, contour);

	{
		VECTORTYPE fvx = gs->free.x;
		VECTORTYPE fvy = gs->free.y;
		register fnt_ElementType* CE2 = gs->CE2;
		ArrayIndex currPt = CE2->sp[contour];
		LoopCount count = CE2->ep[contour] - currPt;
		CHECK_POINT(gs, CE2, currPt + count);
		for (; count >= 0; --count)
		{
			if ( currPt != point || element != CE2 )
			{
				if ( fvx ) {
					CE2->x[currPt] += dx;
					CE2->f[currPt] |= XMOVED;
				}
				if ( fvy ) {
					CE2->y[currPt] += dy;
					CE2->f[currPt] |= YMOVED;
				}
			}
			currPt++;
		}
	}
}

/*
 * SHift Element			<4>
 */
static void fnt_SHZ(register fnt_LocalGraphicStateType *gs)
{
    register fnt_ElementType *element;
	register F26Dot6 dx, dy;
	ArrayIndex firstPoint, origPoint, lastPoint, arg1;

	{
		F26Dot6 x, y;
		element = fnt_SH_Common(gs, &x, &y, &origPoint);
		dx = x;
		dy = y;
	}

	arg1 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	CHECK_ELEMENT(gs, arg1);

	lastPoint = gs->elements[arg1].ep[gs->elements[arg1].nc - 1];
	CHECK_POINT(gs, &gs->elements[arg1], lastPoint);
	firstPoint  = gs->elements[arg1].sp[0];
	CHECK_POINT(gs, &gs->elements[arg1], firstPoint);

/*** changed this			<4>
	do {
		if ( origPoint != firstPoint || element != &gs->elements[arg1] ) {
			if ( gs->free.x ) {
				gs->elements[ arg1 ].x[firstPoint] += dx;
				gs->elements[ arg1 ].f[firstPoint] |= XMOVED;
			}
			if ( gs->free.y ) {
				gs->elements[ arg1 ].y[firstPoint] += dy;
				gs->elements[ arg1 ].f[firstPoint] |= YMOVED;
			}
		}
		firstPoint++;
	} while ( firstPoint <= lastPoint );
***** To this ? *********/

	if (element != &gs->elements[arg1])		/* we're in different zones */
		origPoint = -1;						/* no need to skip orig point */
	{
		register int8 mask = 0;
		if ( gs->free.x )
		{
			register F26Dot6 deltaX = dx;
			register F26Dot6* x = &gs->elements[ arg1 ].x[firstPoint];
			register LoopCount count = origPoint - firstPoint - 1;
			for (; count >= 0; --count )
				*x++ += deltaX;
			if (origPoint == -1)
				count = lastPoint - firstPoint;
			else
			{
				count = lastPoint - origPoint - 1;
				x++;							/* skip origPoint */
			}
			for (; count >= 0; --count )
				*x++ += deltaX;
			mask = XMOVED;
		}
		if ( gs->free.y )		/* fix me semore */
		{
			register F26Dot6 deltaY = dy;
			register F26Dot6* y = &gs->elements[ arg1 ].y[firstPoint];
			register uint8* f = &gs->elements[ arg1 ].f[firstPoint];
			register LoopCount count = origPoint - firstPoint - 1;
			for (; count >= 0; --count )
			{
				*y++ += deltaY;
				*f++ |= mask;
			}
			if (origPoint == -1)
				count = lastPoint - firstPoint;
			else
			{
				count = lastPoint - origPoint - 1;
				y++, f++;						/* skip origPoint */
			}
			mask |= YMOVED;
			for (; count >= 0; --count )
			{
				*y++ += deltaY;
				*f++ |= mask;
			}
		}
	}
}

/*
 * SHift point by PIXel amount
 */
static void fnt_SHPIX(register fnt_LocalGraphicStateType *gs)
{
    register F26Dot6 proj, dx, dy;

	proj = CHECK_POP(gs, gs->stackPointer );
	if ( gs->free.x )
		dx = VECTORMUL( proj, gs->free.x );
	if ( gs->free.y )
		dy = VECTORMUL( proj, gs->free.y );

	fnt_SHP_Common(gs, dx, dy);
}

/*
 * Interpolate Point
 */
static void fnt_IP(register fnt_LocalGraphicStateType *gs)
{
	F26Dot6 oldRange, currentRange;
	register ArrayIndex RP1 = gs->Pt1;
	register ArrayIndex pt2 = gs->Pt2;
	register fnt_ElementType* CE0 = gs->CE0;
	boolean twilight = CE0 == gs->elements || gs->CE1 == gs->elements || gs->CE2 == gs->elements;

	{
		currentRange = gs->Project( gs, gs->CE1->x[pt2] - CE0->x[RP1],
										gs->CE1->y[pt2] - CE0->y[RP1] );
		if ( twilight )
			oldRange = gs->OldProject( gs, gs->CE1->ox[pt2] - CE0->ox[RP1],
										   gs->CE1->oy[pt2] - CE0->oy[RP1] );
		else
			oldRange = gs->OldProject( gs, gs->CE1->oox[pt2] - CE0->oox[RP1],
										   gs->CE1->ooy[pt2] - CE0->ooy[RP1] );
	}
	for (; gs->loop >= 0; --gs->loop)
	{
		register ArrayIndex arg1 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
		register F26Dot6 tmp;
		if ( twilight )
			tmp = gs->OldProject( gs, gs->CE2->ox[arg1] - CE0->ox[RP1],
									  gs->CE2->oy[arg1] - CE0->oy[RP1] );
		else
			tmp = gs->OldProject( gs, gs->CE2->oox[arg1] - CE0->oox[RP1],
									  gs->CE2->ooy[arg1] - CE0->ooy[RP1] );

		if ( oldRange )
			tmp = LongMulDiv( currentRange, tmp, oldRange );

		tmp -= gs->Project( gs, gs->CE2->x[arg1] - CE0->x[RP1],
							    gs->CE2->y[arg1] - CE0->y[RP1] ); /* delta = desired projection - current projection */
		gs->MovePoint( gs, gs->CE2, arg1, tmp );
	}
	gs->loop = 0;
}

/*
 * Move Stack Indirect Relative Point
 */
void fnt_MSIRP( fnt_LocalGraphicStateType* gs )
{
	register fnt_ElementType* CE0 = gs->CE0;
	register fnt_ElementType* CE1 = gs->CE1;
	register ArrayIndex Pt0 = gs->Pt0;
	register F26Dot6 dist = CHECK_POP(gs, gs->stackPointer ); /* distance   */
	register ArrayIndex pt2 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer ); /* point #    */

	if ( CE1 == gs->elements )
	{
		CE1->ox[pt2] = CE0->ox[Pt0] + VECTORMUL( dist, gs->proj.x );
		CE1->oy[pt2] = CE0->oy[Pt0] + VECTORMUL( dist, gs->proj.y );
		CE1->x[pt2] = CE0->x[Pt0];										/* <10> */
		CE1->y[pt2] = CE0->y[Pt0];										/* <10> */
	}
	dist -= gs->Project( gs, CE1->x[pt2] - CE0->x[Pt0],
							  CE1->y[pt2] - CE0->y[Pt0] );
	gs->MovePoint( gs, CE1, pt2, dist );
	gs->Pt1 = Pt0;
	gs->Pt2 = pt2;
	if ( BIT0( gs->opCode ) ) {
		gs->Pt0 = pt2; /* move the reference point */
	}
}

/*
 * Align Relative Point
 */
static void fnt_ALIGNRP(register fnt_LocalGraphicStateType *gs)
{
	register fnt_ElementType* ce1 = gs->CE1;
	register F26Dot6 pt0x = gs->CE0->x[gs->Pt0];
	register F26Dot6 pt0y = gs->CE0->y[gs->Pt0];

	for (; gs->loop >= 0; --gs->loop)
	{
		register ArrayIndex ptNum = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
		register F26Dot6 proj = -gs->Project( gs, ce1->x[ptNum] - pt0x, ce1->y[ptNum] - pt0y );
		gs->MovePoint( gs, ce1, ptNum, proj );
	}
	gs->loop = 0;
}


/*
 * Align Two Points ( by moving both of them )
 */
static void fnt_ALIGNPTS(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex pt1, pt2;
	register F26Dot6 move1, dist;

	pt2  = (ArrayIndex)CHECK_POP(gs, gs->stackPointer ); /* point # 2   */
	pt1  = (ArrayIndex)CHECK_POP(gs, gs->stackPointer ); /* point # 1   */
	/* We do not have to check if we are in character element zero (the twilight zone)
	   since both points already have to have defined values before we execute this instruction */
	dist = gs->Project( gs, gs->CE0->x[pt2] - gs->CE1->x[pt1],
							 gs->CE0->y[pt2] - gs->CE1->y[pt1] );

	move1 = dist >> 1;
	gs->MovePoint( gs, gs->CE0, pt1, move1 );
	gs->MovePoint( gs, gs->CE1, pt2, move1 - dist ); /* make sure the total movement equals tmp32 */
}

/*
 * Set Angle Weight
 */
static void fnt_SANGW(register fnt_LocalGraphicStateType *gs)
{
	CHECK_POP(gs, gs->stackPointer );
}

/*
 * Does a cheap approximation of Euclidian distance.
 */
static Fract fnt_QuickDist(register Fract dx, register Fract dy)
{
	if ( dx < 0 ) dx = -dx;
	if ( dy < 0 ) dy = -dy;

	return( dx > dy ? dx + ( dy >> 1 ) : dy + ( dx >> 1 ) );
}

/*
 * Flip Point
 */
static void fnt_FLIPPT(fnt_LocalGraphicStateType *gs)
{
	register uint8 *onCurve = gs->CE0->onCurve;
	register F26Dot6* stack = gs->stackPointer;
	register LoopCount count = gs->loop;

	for (; count >= 0; --count)
	{
		register ArrayIndex point = (ArrayIndex)CHECK_POP(gs, stack );
		onCurve[ point ] ^= ONCURVE;
	}
	gs->loop = 0;

	gs->stackPointer = stack;
}

/*
 * Flip On a Range
 */
static void fnt_FLIPRGON(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex lo, hi;
	register LoopCount count;
	register uint8 *onCurve = gs->CE0->onCurve;
	register F26Dot6* stack = gs->stackPointer;

	hi = (ArrayIndex)CHECK_POP(gs, stack );
	CHECK_POINT( gs, gs->CE0, hi );
	lo = (ArrayIndex)CHECK_POP(gs, stack );
	CHECK_POINT( gs, gs->CE0, lo );

	onCurve += lo;
	for (count = (LoopCount)(hi - lo); count >= 0; --count)
		*onCurve++ |= ONCURVE;
	gs->stackPointer = stack;
}

/*
 * Flip On a Range
 */
static void fnt_FLIPRGOFF(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex lo, hi;
	register LoopCount count;
	register uint8 *onCurve = gs->CE0->onCurve;

	hi = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	CHECK_POINT( gs, gs->CE0, hi );
	lo = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	CHECK_POINT( gs, gs->CE0, lo );

	onCurve += lo;
	for (count = (LoopCount)(hi - lo); count >= 0; --count)
		*onCurve++ &= ~ONCURVE;
}

/* 4/22/90 rwb - made more general
 * Sets lower 16 flag bits of ScanControl variable.  Sets scanContolIn if we are in one
 * of the preprograms; else sets scanControlOut.
 *
 * stack: value => -;
 *
 */
static void fnt_SCANCTRL(register fnt_LocalGraphicStateType *gs)
{
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	register fnt_ParameterBlock *pb = &globalGS->localParBlock;

	pb->scanControl = (pb->scanControl & 0xFFFF0000) | CHECK_POP(gs, gs->stackPointer );
}

/* 5/24/90 rwb
 * Sets upper 16 bits of ScanControl variable. Sets scanContolIn if we are in one
 * of the preprograms; else sets scanControlOut.
 */

static void fnt_SCANTYPE(register fnt_LocalGraphicStateType *gs)
{
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	register fnt_ParameterBlock *pb = &globalGS->localParBlock;
	register int value = (int)CHECK_POP(gs, gs->stackPointer );
	register int32 *scanPtr = &(pb->scanControl);
	if		( value == 0 )  *scanPtr &= 0xFFFF;
	else if ( value == 1 )	*scanPtr = (*scanPtr & 0xFFFF) | STUBCONTROL;
	else if ( value == 2 )	*scanPtr = (*scanPtr & 0xFFFF) | NODOCONTROL;
}

/* 6/28/90 rwb
 * Sets instructControl flags in global graphic state.  Only legal in pre program.
 * A selector is used to choose the flag to be set.
 * Bit0 - NOGRIDFITFLAG - if set, then truetype instructions are not executed.
 * 		A font may want to use the preprogram to check if the glyph is rotated or
 * 	 	transformed in such a way that it is better to not gridfit the glyphs.
 * Bit1 - DEFAULTFLAG - if set, then changes in localParameterBlock variables in the
 *		globalGraphics state made in the CVT preprogram are not copied back into
 *		the defaultParameterBlock.  So, the original default values are the starting
 *		values for each glyph.
 *
 * stack: value, selector => -;
 *
 */
static void fnt_INSTCTRL(register fnt_LocalGraphicStateType *gs)  /* old<13> */
{
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	register int32 *ic = &globalGS->localParBlock.instructControl;
	int selector 	= (int)CHECK_POP(gs, gs->stackPointer );
	uint32 value 	= (uint32)CHECK_POP(gs, gs->stackPointer );
	if( globalGS->init )
	{
		if( selector == 1 )
		{
			*ic &= ~NOGRIDFITFLAG;
			*ic |= (value & NOGRIDFITFLAG);
		}
		else if( selector == 2 )
		{
			*ic &= ~DEFAULTFLAG;
			*ic |= (value & DEFAULTFLAG);
		}
	}
}

/*
 * AdjustAngle         <4>    		OBSOLETE as of TT version 3     <19> 
 */
static void fnt_AA(register fnt_LocalGraphicStateType *gs)
{
CHECK_POP(gs, gs->stackPointer );
}

/*
 *	Called by fnt_PUSHB and fnt_NPUSHB
 */
static void fnt_PushSomeStuff(fnt_LocalGraphicStateType *gs, register LoopCount count, boolean pushBytes)
{
	register F26Dot6* stack = gs->stackPointer;
	register uint8* instr = gs->insPtr;
	if (pushBytes)
		for (--count; count >= 0; --count)
			CHECK_PUSH( gs, stack, GETBYTE( instr ));
	else
	{
		for (--count; count >= 0; --count)
		{
			int16 word = *instr++;
			CHECK_PUSH( gs, stack, (int16)((word << 8) + *instr++));
		}
	}
	gs->stackPointer = stack;
	gs->insPtr = instr;
}

/*
 * PUSH Bytes		<3>
 */
static void fnt_PUSHB(fnt_LocalGraphicStateType *gs)
{
	fnt_PushSomeStuff(gs, gs->opCode - 0xb0 + 1, true);
}

/*
 * N PUSH Bytes
 */
static void fnt_NPUSHB(register fnt_LocalGraphicStateType *gs)
{
	fnt_PushSomeStuff(gs, GETBYTE( gs->insPtr ), true);
}

/*
 * PUSH Words		<3>
 */
static void fnt_PUSHW(register fnt_LocalGraphicStateType *gs)
{
	fnt_PushSomeStuff(gs, gs->opCode - 0xb8 + 1, false);
}

/*
 * N PUSH Words
 */
static void fnt_NPUSHW(register fnt_LocalGraphicStateType *gs)
{
	fnt_PushSomeStuff(gs, GETBYTE( gs->insPtr ), false);
}

/*
 * Write Store
 */
static void fnt_WS(register fnt_LocalGraphicStateType *gs)
{
    register F26Dot6 storage;
	register ArrayIndex storeIndex;

	storage = CHECK_POP(gs, gs->stackPointer );
	storeIndex = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_STORAGE( gs, storeIndex );

	gs->globalGS->store[ storeIndex ] = storage;
}

/*
 * Read Store
 */
static void fnt_RS(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex storeIndex;

	storeIndex = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	CHECK_STORAGE( gs, storeIndex );
	CHECK_PUSH( gs, gs->stackPointer, gs->globalGS->store[storeIndex] );
}

/*
 * Write Control Value Table from outLine, assumes the value comes form the outline domain
 */
static void fnt_WCVTP(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex cvtIndex;
	register F26Dot6 cvtValue;

	cvtValue = CHECK_POP(gs, gs->stackPointer );
	cvtIndex = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_CVT( gs, cvtIndex );

	gs->globalGS->controlValueTable[ cvtIndex ] = cvtValue;

	/* The BASS outline is in the transformed domain but the cvt is not so apply the inverse transform */
	if ( cvtValue ) {
		register F26Dot6 tmpCvt;
		if ( (tmpCvt = gs->GetCVTEntry( gs, cvtIndex )) && tmpCvt != cvtValue ) {
			gs->globalGS->controlValueTable[ cvtIndex ] = FixMul( cvtValue,  FixDiv( cvtValue, tmpCvt ) );
		}
	}
}

/*
 * Write Control Value Table From Original Domain, assumes the value comes from the original domain, not the cvt or outline
 */
static void fnt_WCVTF(fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex cvtIndex;
	register F26Dot6 cvtValue;
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;

	cvtValue = CHECK_POP(gs, gs->stackPointer );
	cvtIndex = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	CHECK_CVT( gs, cvtIndex );
	globalGS->controlValueTable[ cvtIndex ] = globalGS->ScaleFunc( globalGS, cvtValue );
}



/*
 * Read Control Value Table
 */
static void fnt_RCVT(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex cvtIndex;

	cvtIndex = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	CHECK_PUSH( gs, gs->stackPointer, gs->GetCVTEntry( gs, cvtIndex ) );
}

/*
 * Read Coordinate
 */
static void fnt_GC(register fnt_LocalGraphicStateType *gs)
{
    ArrayIndex pt;
	fnt_ElementType *element;
	register F26Dot6 proj;

	pt = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	element = gs->CE2;

    if ( BIT0( gs->opCode ) )
	    proj = gs->OldProject( gs, element->ox[pt], element->oy[pt] );
	else
	    proj = gs->Project( gs, element->x[pt], element->y[pt] );

	CHECK_PUSH( gs, gs->stackPointer, proj );
}

/*
 * Write Coordinate
 */
static void fnt_SCFS(register fnt_LocalGraphicStateType *gs)
{
    register F26Dot6 proj, coord;
	register ArrayIndex pt;
	register fnt_ElementType *element;

	coord = CHECK_POP(gs, gs->stackPointer );/* value */
	pt = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );/* point */
	element = gs->CE2;

	proj = gs->Project( gs, element->x[pt],  element->y[pt] );
	proj = coord - proj;

	gs->MovePoint( gs, element, pt, proj );

	if (element == gs->elements)		/* twilightzone */
	{
		element->ox[pt] = element->x[pt];
		element->oy[pt] = element->y[pt];
	}
}


/*
 * Measure Distance
 */
static void fnt_MD(register fnt_LocalGraphicStateType *gs)
{
    register ArrayIndex pt1, pt2;
	register F26Dot6 proj, *stack = gs->stackPointer;
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;

	pt2 = (ArrayIndex)CHECK_POP(gs, stack );
	pt1 = (ArrayIndex)CHECK_POP(gs, stack );
	if ( BIT0( gs->opCode - MD_CODE ) )
	{
		proj  = gs->OldProject( gs, gs->CE0->oox[pt1] - gs->CE1->oox[pt2],
								     gs->CE0->ooy[pt1] - gs->CE1->ooy[pt2] );
	    proj = globalGS->ScaleFunc( globalGS, proj );
	}								 
	else
		proj  = gs->Project( gs, gs->CE0->x[pt1] - gs->CE1->x[pt2],
								  gs->CE0->y[pt1] - gs->CE1->y[pt2] );
	CHECK_PUSH( gs, stack, proj );
	gs->stackPointer = stack;
}

/*
 * Measure Pixels Per EM
 */
static void fnt_MPPEM(register fnt_LocalGraphicStateType *gs)
{
    register uint16 ppem;
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;

	ppem = globalGS->pixelsPerEm;

	if ( !globalGS->identityTransformation )
		ppem = (uint16)FixMul( ppem, fnt_GetCVTScale( gs ) );

	CHECK_PUSH( gs, gs->stackPointer, ppem );
}

/*
 * Measure Point Size
 */
static void fnt_MPS(register fnt_LocalGraphicStateType *gs)
{
	CHECK_PUSH( gs, gs->stackPointer, gs->globalGS->pointSize );
}

/*
 * Get Miscellaneous info: version number, rotated, stretched 	<6>
 * Version number is 8 bits.  This is version 0x01 : 5/1/90
 *
 */

static void fnt_GETINFO( register fnt_LocalGraphicStateType* gs )
{
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	register int selector = (int)CHECK_POP(gs, gs->stackPointer );
	register int info = 0;

	if( selector & 1)			/* version 1 = 6.0 TT; 2 = 7.0 TT;
	                             * 3 = Windows 3.1 TT;  4 = 6.1J TT;
								 * 5 = Kirin 1.0 TT; 6 = CubeE */
		info |= 6;											/*<12><24>*/
	if( (selector & 2) && (globalGS->non90DegreeTransformation & 0x1) )
		info |= ROTATEDGLYPH;
	if( (selector & 4) &&  (globalGS->non90DegreeTransformation & 0x2))
		info |= STRETCHEDGLYPH;
	CHECK_PUSH( gs, gs->stackPointer, info );
}

/*
 * FLIP ON
 */
static void fnt_FLIPON(register fnt_LocalGraphicStateType *gs)
{
	gs->globalGS->localParBlock.autoFlip = true;
}

/*
 * FLIP OFF
 */
static void fnt_FLIPOFF(register fnt_LocalGraphicStateType *gs)
{
	gs->globalGS->localParBlock.autoFlip = false;
}

#ifndef NOT_ON_THE_MAC
#ifdef DEBUG
/*
 * DEBUG
 */
static void fnt_DEBUG(register fnt_LocalGraphicStateType *gs)
{
	register int32 arg;
	int8 buffer[24];

	arg = CHECK_POP(gs, gs->stackPointer );

	buffer[1] = 'D';
	buffer[2] = 'E';
	buffer[3] = 'B';
	buffer[4] = 'U';
	buffer[5] = 'G';
	buffer[6] = ' ';
	if ( arg >= 0 ) {
		buffer[7] = '+';
	} else {
		arg = -arg;
		buffer[7] = '-';
	}

	buffer[13] = arg % 10 + '0'; arg /= 10;
	buffer[12] = arg % 10 + '0'; arg /= 10;
	buffer[11] = arg % 10 + '0'; arg /= 10;
	buffer[10] = arg % 10 + '0'; arg /= 10;
	buffer[ 9] = arg % 10 + '0'; arg /= 10;
	buffer[ 8] = arg % 10 + '0'; arg /= 10;

	buffer[14] = arg ? '*' : ' ';


	buffer[0] = 14; /* convert to pascal */
	DebugStr( buffer );
}

#else		/* debug */

static void fnt_DEBUG(register fnt_LocalGraphicStateType* gs)
{
	CHECK_POP(gs, gs->stackPointer );
}

#endif		/* debug */
#else

static void fnt_DEBUG(register fnt_LocalGraphicStateType* gs)
{
	CHECK_POP(gs, gs->stackPointer );
}

#endif		/* ! not on the mac */

/*
 *	This guy is here to save space for simple insructions
 *	that pop two arguments and push one back on.
 */
static void fnt_BinaryOperand(fnt_LocalGraphicStateType* gs)
{
	F26Dot6* stack = gs->stackPointer;
	F26Dot6 arg2 = CHECK_POP(gs, stack );
	F26Dot6 arg1 = CHECK_POP(gs, stack );

	switch (gs->opCode) {
	case LT_CODE:	BOOLEANPUSH( stack, arg1 < arg2 );  break;
	case LTEQ_CODE:	BOOLEANPUSH( stack, arg1 <= arg2 ); break;
	case GT_CODE:	BOOLEANPUSH( stack, arg1 > arg2 );  break;
	case GTEQ_CODE:	BOOLEANPUSH( stack, arg1 >= arg2 ); break;
	case EQ_CODE:	BOOLEANPUSH( stack, arg1 == arg2 ); break;
	case NEQ_CODE:	BOOLEANPUSH( stack, arg1 != arg2 ); break;

	case AND_CODE:	BOOLEANPUSH( stack, arg1 && arg2 ); break;
	case OR_CODE:	BOOLEANPUSH( stack, arg1 || arg2 ); break;

	case ADD_CODE:	CHECK_PUSH( gs, stack, arg1 + arg2 ); break;
	case SUB_CODE:	CHECK_PUSH( gs, stack, arg1 - arg2 ); break;
	case MUL_CODE:	CHECK_PUSH( gs, stack, Mul26Dot6( arg1, arg2 )); break;
	case DIV_CODE:	CHECK_PUSH( gs, stack, Div26Dot6( arg1, arg2 )); break;
	case MAX_CODE:	if (arg1 < arg2) arg1 = arg2; CHECK_PUSH( gs, stack, arg1 ); break;
	case MIN_CODE:	if (arg1 > arg2) arg1 = arg2; CHECK_PUSH( gs, stack, arg1 ); break;
#ifdef DEBUG
	default:
		Debugger();
#endif
	}
	gs->stackPointer = stack;
	CHECK_STACK(gs);
}

static void fnt_UnaryOperand(fnt_LocalGraphicStateType* gs)
{
	F26Dot6* stack = gs->stackPointer;
	F26Dot6 arg = CHECK_POP(gs, stack );
	uint8 opCode = gs->opCode;

	switch (opCode) {
	case ODD_CODE:
	case EVEN_CODE:
		arg = fnt_RoundToGrid( arg, 0L, 0 );
		arg >>= fnt_pixelShift;
		if ( opCode == ODD_CODE )
			arg++;
		BOOLEANPUSH( stack, (arg & 1) == 0 );
		break;
	case NOT_CODE:	BOOLEANPUSH( stack, !arg );  break;

	case ABS_CODE:	CHECK_PUSH( gs, stack, arg > 0 ? arg : -arg ); break;
	case NEG_CODE:	CHECK_PUSH( gs, stack, -arg ); break;

	case CEILING_CODE:
		arg += fnt_pixelSize - 1;
	case FLOOR_CODE:
		arg &= ~(fnt_pixelSize-1);
		CHECK_PUSH( gs, stack, arg );
		break;
#ifdef DEBUG
	default:
		Debugger();
#endif
	}
	gs->stackPointer = stack;
	CHECK_STACK(gs);
}

#define NPUSHB_CODE 0x40
#define NPUSHW_CODE 0x41

#define PUSHB_START 0xb0
#define PUSHB_END 	0xb7
#define PUSHW_START 0xb8
#define PUSHW_END 	0xbf

/*
 * Internal function for fnt_IF(), and fnt_FDEF()
 */
static void fnt_SkipPushCrap(register fnt_LocalGraphicStateType *gs)
{
	register uint8 opCode = gs->opCode;
	register uint8* instr = gs->insPtr;
	register ArrayIndex count;

	if ( opCode == NPUSHB_CODE ) {
		count = (ArrayIndex)*instr++;
		instr += count;
	} else if ( opCode == NPUSHW_CODE ) {
		count = (ArrayIndex)*instr++;
		instr += count + count;
	} else if ( opCode >= PUSHB_START && opCode <= PUSHB_END ) {
		count = (ArrayIndex)(opCode - PUSHB_START + 1);
		instr += count;
	} else if ( opCode >= PUSHW_START && opCode <= PUSHW_END ) {
		count = (ArrayIndex)(opCode - PUSHW_START + 1);
		instr += count + count;
	}
	gs->insPtr = instr;
}

/*
 * IF
 */
static void fnt_IF(register fnt_LocalGraphicStateType *gs)
{
    register int level;
	register uint8 opCode;

	if ( ! CHECK_POP(gs, gs->stackPointer ) ) {
		/* Now skip instructions */
		for ( level = 1; level; ) {
			/* level = # of "ifs" minus # of "endifs" */
			if ( (gs->opCode = opCode = *gs->insPtr++) == EIF_CODE ) {
				level--;
			} else if ( opCode == IF_CODE ) {
				level++;
			} else if ( opCode == ELSE_CODE ) {
				if ( level == 1 ) break;
			} else
				fnt_SkipPushCrap( gs );
		}
	}
}

/*
 *	ELSE for the IF
 */
static void fnt_ELSE( fnt_LocalGraphicStateType* gs )
{
    register int level;
	register uint8 opCode;

	for ( level = 1; level; ) {
		/* level = # of "ifs" minus # of "endifs" */
		if ( (gs->opCode = opCode = *gs->insPtr++) == EIF_CODE ) { /* EIF */
			level--;
		} else if ( opCode == IF_CODE ) {
			level++;
		} else
			fnt_SkipPushCrap( gs );
	}
}

/*
 * End IF
 */
static void fnt_EIF( fnt_LocalGraphicStateType* gs)
{
#pragma unused(gs)
}

/*
 * Jump Relative
 */
static void fnt_JMPR( register fnt_LocalGraphicStateType* gs )
{
	register ArrayIndex offset;

	offset = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	offset--; /* since the interpreter post-increments the IP */
	CHECK_ASSERTION( gs->insPtr > gs->globalGS->instructBase );  /*<22>*/
	gs->insPtr += offset;
	CHECK_ASSERTION( gs->insPtr > gs->globalGS->instructBase ); /*<22>*/

}

/*
 * Jump Relative On True
 */
static void fnt_JROT(register fnt_LocalGraphicStateType *gs)
{
	register ArrayIndex offset;
	register F26Dot6* stack = gs->stackPointer;

	if ( CHECK_POP(gs, stack ) ) {
		offset = (ArrayIndex)CHECK_POP(gs, stack );
		--offset; /* since the interpreter post-increments the IP */
		CHECK_ASSERTION( gs->insPtr > gs->globalGS->instructBase ); /*<22>*/
		gs->insPtr += offset;
	} else {
		--stack;/* same as POP */
	}
	CHECK_ASSERTION( gs->insPtr > gs->globalGS->instructBase ); /*<22>*/
	gs->stackPointer = stack;
}

/*
 * Jump Relative On False
 */
static void fnt_JROF(register fnt_LocalGraphicStateType *gs)
{
	register ArrayIndex offset;
	register F26Dot6* stack = gs->stackPointer;

	if ( CHECK_POP(gs, stack ) ) {
		--stack;/* same as POP */
	} else {
		offset = (ArrayIndex)CHECK_POP(gs, stack );
		offset--; /* since the interpreter post-increments the IP */
		CHECK_ASSERTION( gs->insPtr > gs->globalGS->instructBase ); /*<22>*/
		gs->insPtr += offset;
	}
	CHECK_ASSERTION( gs->insPtr > gs->globalGS->instructBase ); /*<22>*/
	gs->stackPointer = stack;
}

/*
 * ROUND
 */
static void fnt_ROUND(register fnt_LocalGraphicStateType *gs)
{
    register F26Dot6 arg1;
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;

	arg1 = CHECK_POP(gs, gs->stackPointer );

	CHECK_RANGE( gs->opCode, 0x68, 0x6B );

	arg1 = pb->RoundValue( arg1, gs->globalGS->engine[gs->opCode - 0x68], gs);
	CHECK_PUSH( gs, gs->stackPointer , arg1 );
}

/*
 * No ROUND
 */
static void fnt_NROUND(register fnt_LocalGraphicStateType *gs)
{
    register F26Dot6 arg1;

	arg1 = CHECK_POP(gs, gs->stackPointer );

	CHECK_RANGE( gs->opCode, 0x6C, 0x6F );

	arg1 = fnt_RoundOff( arg1, gs->globalGS->engine[gs->opCode - 0x6c], 0 );
	CHECK_PUSH( gs, gs->stackPointer , arg1 );
}

/*
 * An internal function used by MIRP an MDRP.
 */
static F26Dot6 fnt_CheckSingleWidth(register F26Dot6 value, register fnt_LocalGraphicStateType *gs)
{
	register F26Dot6 delta, scaledSW;
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;

	scaledSW = gs->GetSingleWidth( gs );

	if ( value >= 0 ) {
		delta = value - scaledSW;
		if ( delta < 0 )    delta = -delta;
		if ( delta < pb->sWCI )    value = scaledSW;
	} else {
		value = -value;
		delta = value - scaledSW;
		if ( delta < 0 )    delta = -delta;
		if ( delta < pb->sWCI )    value = scaledSW;
		value = -value;
	}
	return value;
}


/*
 * Move Direct Relative Point
 */
static void fnt_MDRP(register fnt_LocalGraphicStateType *gs)
{
	register ArrayIndex pt1, pt0 = gs->Pt0;
	register F26Dot6 tmp, tmpC;
    register fnt_ElementType *element = gs->CE1;
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	register fnt_ParameterBlock *pb = &globalGS->localParBlock;

	pt1 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_POINT(gs, gs->CE0, pt0);
	CHECK_POINT(gs, element, pt1);

	if ( gs->CE0 == gs->elements || element == gs->elements ) {
		tmp  = gs->OldProject( gs, element->ox[pt1] - gs->CE0->ox[pt0],
								     element->oy[pt1] - gs->CE0->oy[pt0] );
	} else {
		tmp  = gs->OldProject( gs, element->oox[pt1] - gs->CE0->oox[pt0],
								     element->ooy[pt1] - gs->CE0->ooy[pt0] );
	    tmp = globalGS->ScaleFunc( globalGS, tmp );
	}

	if ( pb->sWCI ) {
		tmp = fnt_CheckSingleWidth( tmp, gs );
	}

	tmpC = tmp;
	if ( BIT2( gs->opCode )  ) {
		tmp = pb->RoundValue( tmp, globalGS->engine[gs->opCode & 0x03], gs );
	} else {
		tmp = fnt_RoundOff( tmp, globalGS->engine[gs->opCode & 0x03], 0 );
	}


	if ( BIT3( gs->opCode ) )
	{
		F26Dot6 tmpB = pb->minimumDistance;
		if ( tmpC >= 0 ) {
			if ( tmp < tmpB ) {
				tmp = tmpB;
			}
		} else {
			tmpB = -tmpB;
			if ( tmp > tmpB ) {
				tmp = tmpB;
			}
		}
	}

	tmpC = gs->Project( gs, element->x[pt1] - gs->CE0->x[pt0],
							element->y[pt1] - gs->CE0->y[pt0] );
	tmp -= tmpC;
	gs->MovePoint( gs, element, pt1, tmp );
	gs->Pt1 = pt0;
	gs->Pt2 = pt1;
	if ( BIT4( gs->opCode ) ) {
		gs->Pt0 = pt1; /* move the reference point */
	}
}

/*
 * Move Indirect Relative Point
 */
static void fnt_MIRP(register fnt_LocalGraphicStateType *gs)
{
	register ArrayIndex ptNum;
	register F26Dot6 tmp, tmpB, tmpC;
	register fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;

	tmp = gs->GetCVTEntry( gs, (ArrayIndex)CHECK_POP(gs, gs->stackPointer ) );
	ptNum = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	if ( pb->sWCI ) {
		tmp = fnt_CheckSingleWidth( tmp, gs );
	}

	if ( gs->CE1 == gs->elements )
	{
		gs->CE1->ox[ptNum] = gs->CE0->ox[gs->Pt0];
		gs->CE1->oy[ptNum] = gs->CE0->oy[gs->Pt0];
		gs->CE1->ox[ptNum] += VECTORMUL( tmp, gs->proj.x );
		gs->CE1->oy[ptNum] += VECTORMUL( tmp, gs->proj.y );
		gs->CE1->x[ptNum] = gs->CE0->x[gs->Pt0];			/* <10> */
		gs->CE1->y[ptNum] = gs->CE0->y[gs->Pt0];			/* <10> */
	}

	tmpC  = gs->OldProject( gs, gs->CE1->ox[ptNum] - gs->CE0->ox[gs->Pt0],
							      gs->CE1->oy[ptNum] - gs->CE0->oy[gs->Pt0] );
	if ( pb->autoFlip ) {
		if ( ((int32)(tmpC ^ tmp)) < 0 ) {
			tmp = -tmp; /* Do the auto flip */
		}
	}

	if ( BIT2( gs->opCode )  ) {
		tmpB = tmp - tmpC;
		if ( tmpB < 0 )    tmpB = -tmpB;
		if ( tmpB > pb->wTCI )    tmp = tmpC;
		tmp = pb->RoundValue( tmp, globalGS->engine[gs->opCode & 0x03], gs );
	} else {
		tmp = fnt_RoundOff( tmp, globalGS->engine[gs->opCode & 0x03], 0 );
	}


	if ( BIT3( gs->opCode ) ) {
		tmpB = gs->globalGS->localParBlock.minimumDistance;
		if ( tmpC >= 0 ) {
			if ( tmp < tmpB ) {
				tmp = tmpB;
			}
		} else {
			tmpB = -tmpB;
			if ( tmp > tmpB ) {
				tmp = tmpB;
			}
		}
	}

	tmpC  = gs->Project( gs, gs->CE1->x[ptNum] - gs->CE0->x[gs->Pt0],
							   gs->CE1->y[ptNum] - gs->CE0->y[gs->Pt0] );

	tmp  -= tmpC;

	gs->MovePoint( gs, gs->CE1, ptNum, tmp );
	gs->Pt1 = gs->Pt0;
	gs->Pt2 = ptNum;
	if ( BIT4( gs->opCode ) ) {
		gs->Pt0 = ptNum; /* move the reference point */
	}
}

/*
 * CALL a function
 */
static void fnt_CALL(register fnt_LocalGraphicStateType *gs)
{
	register fnt_funcDef *funcDef;
	uint8 *ins;
	fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	ArrayIndex arg = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_ASSERTION( globalGS->funcDef != 0 );
	CHECK_FDEF( gs, arg );
    funcDef = &globalGS->funcDef[ arg ];

	CHECK_PROGRAM(funcDef->pgmIndex);
	ins     = globalGS->pgmList[ funcDef->pgmIndex ];

	CHECK_ASSERTION( ins != 0 );

	ins += funcDef->start;
    gs->Interpreter( gs, ins, ins + funcDef->length);
}

/*
 * Function DEFinition
 */
static void fnt_FDEF(register fnt_LocalGraphicStateType *gs)
{
	register fnt_funcDef *funcDef;
	uint8* program, *funcStart;
	fnt_GlobalGraphicStateType *globalGS = gs->globalGS;
	ArrayIndex arg = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );

	CHECK_FDEF( gs, arg );

    funcDef = &globalGS->funcDef[ arg ];
	program = globalGS->pgmList[ funcDef->pgmIndex = globalGS->pgmIndex ];

	CHECK_PROGRAM(funcDef->pgmIndex);
	CHECK_ASSERTION( globalGS->funcDef != 0 );
	CHECK_ASSERTION( globalGS->pgmList[funcDef->pgmIndex] != 0 );

	funcDef->start = gs->insPtr - program;
	funcStart = gs->insPtr;
	while ( (gs->opCode = *gs->insPtr++) != ENDF_CODE )
		fnt_SkipPushCrap( gs );

	funcDef->length = gs->insPtr - funcStart - 1; /* don't execute ENDF */
}

/*
 * LOOP while CALLing a function
 */
static void fnt_LOOPCALL(register fnt_LocalGraphicStateType *gs)
{
	register uint8 *start, *stop;
	register InterpreterFunc Interpreter;
	register fnt_funcDef *funcDef;
	ArrayIndex arg = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	register LoopCount loop;

	CHECK_FDEF( gs, arg );

    funcDef	= &(gs->globalGS->funcDef[ arg ]);
	{
		uint8* ins;
		
		CHECK_PROGRAM(funcDef->pgmIndex);
		ins = gs->globalGS->pgmList[ funcDef->pgmIndex ];

		start		= &ins[funcDef->start];
		stop		= &ins[funcDef->start + funcDef->length];	/* funcDef->end -> funcDef->length <4> */
	}
	Interpreter = gs->Interpreter;
	loop = (LoopCount)CHECK_POP(gs, gs->stackPointer );
    for (--loop; loop >= 0; --loop )
        Interpreter( gs, start, stop );
}

/*
 *	This guy returns the index of the given opCode, or 0 if not found <4>
 */
static fnt_instrDef* fnt_FindIDef(fnt_LocalGraphicStateType* gs, register uint8 opCode)
{
	register fnt_GlobalGraphicStateType* globalGS = gs->globalGS;
	register LoopCount count = globalGS->instrDefCount;
	register fnt_instrDef* instrDef = globalGS->instrDef;
	for (--count; count >= 0; instrDef++, --count)
		if (instrDef->opCode == opCode)
			return instrDef;
	return 0;
}

/*
 *	This guy gets called for opCodes that has been patch by the font's IDEF	<4>
 *	or if they have never been defined.  If there is no corresponding IDEF,
 *	flag it as an illegal instruction.
 */
static void fnt_IDefPatch( register fnt_LocalGraphicStateType* gs )
{
	register fnt_instrDef* instrDef = fnt_FindIDef(gs, gs->opCode);
	if (instrDef == 0)
		fnt_IllegalInstruction( gs );
	else
	{
		register uint8* program;

		CHECK_PROGRAM(instrDef->pgmIndex);
		program = gs->globalGS->pgmList[ instrDef->pgmIndex ];

		program += instrDef->start;
	    gs->Interpreter( gs, program, program + instrDef->length);
	}
}

/*
 * Instruction DEFinition	<4>
 */
static void fnt_IDEF( register fnt_LocalGraphicStateType* gs )
{
	register uint8 opCode = (uint8)CHECK_POP(gs, gs->stackPointer );
	register fnt_instrDef* instrDef = fnt_FindIDef(gs, opCode);
	register ArrayIndex pgmIndex = (ArrayIndex)gs->globalGS->pgmIndex;
	uint8* program = gs->globalGS->pgmList[ pgmIndex ];
	uint8* instrStart = gs->insPtr;

	CHECK_PROGRAM(pgmIndex);

	if (!instrDef)
		instrDef = gs->globalGS->instrDef + gs->globalGS->instrDefCount++;

	instrDef->pgmIndex = pgmIndex;
	instrDef->opCode = opCode;		/* this may or may not have been set */
	instrDef->start = gs->insPtr - program;

	while ( (gs->opCode = *gs->insPtr++) != ENDF_CODE )
		fnt_SkipPushCrap( gs );

	instrDef->length = gs->insPtr - instrStart - 1; /* don't execute ENDF */
}

/*
 * UnTouch Point
 */
static void fnt_UTP(register fnt_LocalGraphicStateType *gs)
{
	register ArrayIndex point = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
	register uint8* f = gs->CE0->f;

	if ( gs->free.x ) {
		f[point] &= ~XMOVED;
	}
	if ( gs->free.y ) {
		f[point] &= ~YMOVED;
	}
}

/*
 * Set Delta Base
 */
static void fnt_SDB(register fnt_LocalGraphicStateType *gs)
{
	gs->globalGS->localParBlock.deltaBase = (int16)CHECK_POP(gs, gs->stackPointer );
}

/*
 * Set Delta Shift
 */
static void fnt_SDS(register fnt_LocalGraphicStateType *gs)
{
	gs->globalGS->localParBlock.deltaShift = (int16)CHECK_POP(gs, gs->stackPointer );
}

/*
 * DeltaEngine, internal support routine
 */
static void fnt_DeltaEngine(register fnt_LocalGraphicStateType *gs, FntMoveFunc doIt,
								int16 base, int16 shift)
{
	register int32 tmp;
	register int32 fakePixelsPerEm, ppem;
	register int32 aim, high;
	register int32 tmp32;

	/* Find the beginning of data pairs for this particular size */
	high = (int32)CHECK_POP(gs, gs->stackPointer ) << 1; /* -= number of pops required */
	gs->stackPointer -= high;

	/* same as fnt_MPPEM() */
	tmp32 = gs->globalGS->pixelsPerEm;

	if ( !gs->globalGS->identityTransformation ) {
		Fixed scale;

		scale = fnt_GetCVTScale( gs );
		tmp32 = FixMul( tmp32, scale );
	}

	fakePixelsPerEm = tmp32 - base;



	if ( fakePixelsPerEm >= 16 ||fakePixelsPerEm < 0 ) return; /* Not within exception range */
	fakePixelsPerEm <<= 4;

	aim = 0;
	tmp = high >> 1; tmp &= ~1;
	while ( tmp > 2 ) {
		ppem  = gs->stackPointer[ aim + tmp ]; /* [ ppem << 4 | exception ] */
		if ( (ppem & ~0x0f) < fakePixelsPerEm ) {
			aim += tmp;
		}
		tmp >>= 1; tmp &= ~1;
	}

	while ( aim < high ) {
		ppem  = gs->stackPointer[ aim ]; /* [ ppem << 4 | exception ] */
		if ( (tmp = (ppem & ~0x0f)) == fakePixelsPerEm ) {
			/* We found an exception, go ahead and apply it */
			tmp  = ppem & 0xf; /* 0 ... 15 */
			tmp -= tmp >= 8 ? 7 : 8; /* -8 ... -1, 1 ... 8 */
			tmp <<= fnt_pixelShift; /* convert to pixels */
			tmp >>= shift; /* scale to right size */
			doIt( gs, gs->CE0, gs->stackPointer[aim+1] /* point number */, tmp /* the delta */ );
		} else if ( tmp > fakePixelsPerEm ) {
			break; /* we passed the data */
		}
		aim += 2;
	}
}



/*
 * DELTAP1
 */
static void fnt_DELTAP1(register fnt_LocalGraphicStateType *gs)
{
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;
	fnt_DeltaEngine( gs, gs->MovePoint, pb->deltaBase, pb->deltaShift );
}

/*
 * DELTAP2
 */
static void fnt_DELTAP2(register fnt_LocalGraphicStateType *gs)
{
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;
	fnt_DeltaEngine( gs, gs->MovePoint, pb->deltaBase+16, pb->deltaShift );
}

/*
 * DELTAP3
 */
static void fnt_DELTAP3(register fnt_LocalGraphicStateType *gs)
{
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;
	fnt_DeltaEngine( gs, gs->MovePoint, pb->deltaBase+32, pb->deltaShift );
}

/*
 * DELTAC1
 */
static void fnt_DELTAC1(register fnt_LocalGraphicStateType *gs)
{
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;
	fnt_DeltaEngine( gs, fnt_ChangeCvt, pb->deltaBase, pb->deltaShift );
}

/*
 * DELTAC2
 */
static void fnt_DELTAC2(register fnt_LocalGraphicStateType *gs)
{
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;
	fnt_DeltaEngine( gs, fnt_ChangeCvt, pb->deltaBase+16, pb->deltaShift );
}

/*
 * DELTAC3
 */
static void fnt_DELTAC3(register fnt_LocalGraphicStateType *gs)
{
	register fnt_ParameterBlock *pb = &gs->globalGS->localParBlock;
	fnt_DeltaEngine( gs, fnt_ChangeCvt, pb->deltaBase+32, pb->deltaShift );
}

/* This code adjusts strokes so that their width is closer to the target width.
 * Only 1 bit is added or subtracted.
 * If the boolean part of the opCode is set, the edge that is moved is specified
 * the first argument; otherwise the edge that needs to move the least is moved.  
 */

static void fnt_ADJUST( fnt_LocalGraphicStateType *gs )  /* <13> */
{
	index j1, j2, jMove;
	pixel target;
	subPixel movement, *coord;
	uint8 direction;
	boolean moveFirstOne =  gs->opCode == (ADJUSTBASE + 1);

	if( gs->free.x == 0 )
	{
		coord = gs->CE0->y;
		direction = YMOVED;
	}		
	else
	{
		coord = gs->CE0->x;
		direction = XMOVED;
	}
	target = gs->GetCVTEntry( gs, (ArrayIndex)CHECK_POP(gs, gs->stackPointer ) );
	CHECK_RANGE(target, 0, 32767);
	target += HALF;
	target >>= LG2PIXEL;
	if( target <= 0 ) target = 1;
	for (; gs->loop >= 0; --gs->loop)
	{
		j1 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
		j2 = (ArrayIndex)CHECK_POP(gs, gs->stackPointer );
		CHECK_POINT( gs, gs->CE0, j1 );
		CHECK_POINT( gs, gs->CE0, j2 );
		jMove = moveFirstOne ? j1 : -1; 
		CALCEDGE(target, j1, j2, coord, &movement, jMove );
		if( jMove >= 0 )
			MOVEEDGE( jMove, movement, coord, gs->CE0->sp,
					gs->CE0->ep, gs->CE0->nc, gs->CE0->f, direction );
	}
	gs->loop = 0;
}

/*
 *	Rebuild the jump table		<4>
 */
static void fnt_DefaultJumpTable( register voidFunc* function )
{
	register LoopCount i;

	/***** 0x00 - 0x0f *****/
    *function++ = fnt_SVTCA_0;
	*function++ = fnt_SVTCA_1;
	*function++ = fnt_SPVTCA;
	*function++ = fnt_SPVTCA;
	*function++ = fnt_SFVTCA;
	*function++ = fnt_SFVTCA;
	*function++ = fnt_SPVTL;
	*function++ = fnt_SPVTL;
	*function++ = fnt_SFVTL;
	*function++ = fnt_SFVTL;
	*function++ = fnt_SPVFS;					/* previously WPV <20> */
	*function++ = fnt_SFVFS;					/* previously WFV <20> */
	*function++ = fnt_GPV;						/* previously RPV <20> */
	*function++ = fnt_GFV;						/* previously RFV <20> */
	*function++ = fnt_SFVTPV;
	*function++ = fnt_ISECT;

	/***** 0x10 - 0x1f *****/
	*function++ = fnt_SetLocalGraphicState;		/* fnt_SRP0; */
	*function++ = fnt_SetLocalGraphicState;		/* fnt_SRP1; */
	*function++ = fnt_SetLocalGraphicState;		/* fnt_SRP2; */
	*function++ = fnt_SetElementPtr;			/* fnt_SCE0; */
	*function++ = fnt_SetElementPtr;			/* fnt_SCE1; */
	*function++ = fnt_SetElementPtr;			/* fnt_SCE2; */
	*function++ = fnt_SetElementPtr;			/* fnt_SCES; */
	*function++ = fnt_SetLocalGraphicState;		/* fnt_SLOOP previously fnt_LLOOP <20> */
	*function++ = fnt_SetRoundState;			/* fnt_RTG; */
	*function++ = fnt_SetRoundState;			/* fnt_RTHG; */
	*function++ = fnt_SMD;						/* fnt_SMD previously fnt_LMD <20> */
	*function++ = fnt_ELSE;						/* used to be fnt_RLSB */
	*function++ = fnt_JMPR;						/* used to be fnt_WLSB */
	*function++ = fnt_SCVTCI;					/* previously LWTCI <20> */
	*function++ = fnt_SSWCI;					/* previously LSWCI <20> */
	*function++ = fnt_SSW;						/* previously LSW <20> */

	/***** 0x20 - 0x2f *****/
	*function++ = fnt_DUP;
	*function++ = fnt_SetLocalGraphicState;		/* fnt_POP; */
	*function++ = fnt_CLEAR;
	*function++ = fnt_SWAP;
	*function++ = fnt_DEPTH;
	*function++ = fnt_CINDEX;
	*function++ = fnt_MINDEX;
	*function++ = fnt_ALIGNPTS;
	*function++ = fnt_RAW;
	*function++ = fnt_UTP;
	*function++ = fnt_LOOPCALL;
	*function++ = fnt_CALL;
	*function++ = fnt_FDEF;
	*function++ = fnt_IllegalInstruction; 		/* fnt_ENDF; used for FDEF and IDEF */
	*function++ = fnt_MDAP;
	*function++ = fnt_MDAP;


	/***** 0x30 - 0x3f *****/
	*function++ = fnt_IUP;
	*function++ = fnt_IUP;
	*function++ = fnt_SHP;
	*function++ = fnt_SHP;
	*function++ = fnt_SHC;
	*function++ = fnt_SHC;
	*function++ = fnt_SHZ;						/* previously SHE <20> */
	*function++ = fnt_SHZ;						/* previously SHE <20> */
	*function++ = fnt_SHPIX;
	*function++ = fnt_IP;
	*function++ = fnt_MSIRP;
	*function++ = fnt_MSIRP;
	*function++ = fnt_ALIGNRP;
	*function++ = fnt_SetRoundState;	/* fnt_RTDG; */
	*function++ = fnt_MIAP;
	*function++ = fnt_MIAP;

	/***** 0x40 - 0x4f *****/
	*function++ = fnt_NPUSHB;
	*function++ = fnt_NPUSHW;
	*function++ = fnt_WS;
	*function++ = fnt_RS;
	*function++ = fnt_WCVTP;			/* previously WCVT <20> */
	*function++ = fnt_RCVT;
	*function++ = fnt_GC;				/* previously RC <20> */
	*function++ = fnt_GC;				/* previously RC <20> */
	*function++ = fnt_SCFS;				/* previously WC <20> */
	*function++ = fnt_MD;
	*function++ = fnt_MD;
	*function++ = fnt_MPPEM;
	*function++ = fnt_MPS;
	*function++ = fnt_FLIPON;
	*function++ = fnt_FLIPOFF;
	*function++ = fnt_DEBUG;

	/***** 0x50 - 0x5f *****/
	*function++ = fnt_BinaryOperand;	/* fnt_LT; */
	*function++ = fnt_BinaryOperand;	/* fnt_LTEQ; */
	*function++ = fnt_BinaryOperand;	/* fnt_GT; */
	*function++ = fnt_BinaryOperand;	/* fnt_GTEQ; */
	*function++ = fnt_BinaryOperand;	/* fnt_EQ; */
	*function++ = fnt_BinaryOperand;	/* fnt_NEQ; */
	*function++ = fnt_UnaryOperand;		/* fnt_ODD; */
	*function++ = fnt_UnaryOperand;		/* fnt_EVEN; */
	*function++ = fnt_IF;
	*function++ = fnt_EIF;		/* should this guy be an illegal instruction??? */
	*function++ = fnt_BinaryOperand;	/* fnt_AND; */
	*function++ = fnt_BinaryOperand;	/* fnt_OR; */
	*function++ = fnt_UnaryOperand;		/* fnt_NOT; */
	*function++ = fnt_DELTAP1;
	*function++ = fnt_SDB;
	*function++ = fnt_SDS;

	/***** 0x60 - 0x6f *****/
	*function++ = fnt_BinaryOperand;	/* fnt_ADD; */
	*function++ = fnt_BinaryOperand;	/* fnt_SUB; */
	*function++ = fnt_BinaryOperand;	/* fnt_DIV;  */
	*function++ = fnt_BinaryOperand;	/* fnt_MUL; */
	*function++ = fnt_UnaryOperand;		/* fnt_ABS; */
	*function++ = fnt_UnaryOperand;		/* fnt_NEG; */
	*function++ = fnt_UnaryOperand;		/* fnt_FLOOR; */
	*function++ = fnt_UnaryOperand;		/* fnt_CEILING */
	*function++ = fnt_ROUND;
	*function++ = fnt_ROUND;
	*function++ = fnt_ROUND;
	*function++ = fnt_ROUND;
	*function++ = fnt_NROUND;
	*function++ = fnt_NROUND;
	*function++ = fnt_NROUND;
	*function++ = fnt_NROUND;

	/***** 0x70 - 0x7f *****/
	*function++ = fnt_WCVTF;			/* previously WCVTFOD <20> */
	*function++ = fnt_DELTAP2;
	*function++ = fnt_DELTAP3;
	*function++ = fnt_DELTAC1;
	*function++ = fnt_DELTAC2;
	*function++ = fnt_DELTAC3;
	*function++ = fnt_SROUND;
	*function++ = fnt_S45ROUND;
	*function++ = fnt_JROT;
	*function++ = fnt_JROF;
 	*function++ = fnt_SetRoundState;	/* fnt_ROFF; */
	*function++ = fnt_IllegalInstruction;/* 0x7b reserved for data compression */
	*function++ = fnt_SetRoundState;	/* fnt_RUTG; */
	*function++ = fnt_SetRoundState;	/* fnt_RDTG; */
	*function++ = fnt_SANGW;			/* <14> opcode for obsolete SANGW instruction */
	*function++ = fnt_AA;				/* <14> opcode for obsolete AA instruction */

	/***** 0x80 - 0x8f *****/
	*function++ = fnt_FLIPPT;
	*function++ = fnt_FLIPRGON;
	*function++ = fnt_FLIPRGOFF;
	*function++ = fnt_IDefPatch;		/* fnt_RMVT, this space for rent */
	*function++ = fnt_IDefPatch;		/* fnt_WMVT, this space for rent */
	*function++ = fnt_SCANCTRL;
	*function++ = fnt_SDPVTL;
	*function++ = fnt_SDPVTL;
	*function++ = fnt_GETINFO;			/* <7> */
	*function++ = fnt_IDEF;
	*function++ = fnt_ROLL;				/* previously ROTATE <20> */
	*function++ = fnt_BinaryOperand;	/* fnt_MAX; */
	*function++ = fnt_BinaryOperand;	/* fnt_MIN; */
	*function++ = fnt_SCANTYPE;			/* <7> */
	*function++ = fnt_INSTCTRL;			/* old<13> */

	/***** 0x8F - 0x90 *****/
	*function++ = fnt_ADJUST;			/* <15> */
	*function++ = fnt_ADJUST;			/* <15> */
		
	/***** 0x91 - 0xaf *****/
	for ( i = 30; i >= 0; --i )	
	    *function++ = fnt_IDefPatch;	/* potentially fnt_IllegalInstruction  <4> */

	/***** 0xb0 - 0xb7 *****/
	for ( i = 7; i >= 0; --i )
	    *function++ = fnt_PUSHB;

	/***** 0xb8 - 0xbf *****/
	for ( i = 7; i >= 0; --i )
	    *function++ = fnt_PUSHW;

	/***** 0xc0 - 0xdf *****/
	for ( i = 31; i >= 0; --i )
	    *function++ = fnt_MDRP;

	/***** 0xe0 - 0xff *****/
	for ( i = 31; i >= 0; --i )
	    *function++ = fnt_MIRP;
}

/*
 *	Init routine, to be called at boot time.
 *	globalGS->function has to be set up when this function is called.
 *	rewrite initialization from p[] to *p++							<3>
 *	restructure fnt_AngleInfo into fnt_FractPoint and int16			<3>
 *
 *	Only gs->function is valid at this time.
 *  Remove fnt_AngleInfor information and constant setup			<19>
 */
void fnt_Init( fnt_GlobalGraphicStateType* globalGS )
{
	fnt_DefaultJumpTable( globalGS->function );

}

/* END OF fnt.c */
