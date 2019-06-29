/*
	File:		FontMath.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	11/27/90	MR		make pascal declaration a macro, conditionalize traps -vs-
									externs for Fix/Frac math routines. [ph]
		 <3>	 11/5/90	MR		Move [U]SHORTMUL into FSCdefs.h Rename FixMulDiv to LongMulDiv.
									[rb]
		 <2>	10/20/90	MR		Add some new math routines (stolen from skia). [rj]
		 <1>	 4/11/90	dba		first checked in

	To Do:
*/

#define HIBITSET			0x80000000
#define POSINFINITY			0x7FFFFFFF
#define NEGINFINITY			0x80000000
#define HIWORDMASK			0xffff0000
#define LOWORDMASK			0x0000ffff
#define FIXONEHALF			0x00008000
#define ONESHORTFRAC		(1 << 14)

#define FIXROUND( x )		(int16)(((x) + FIXONEHALF) >> 16)
#define ROUNDFIXED( x )		(((x) + FIXONEHALF) & HIWORDMASK)
#define DOT6TOFIX(n)		((n) << 10)
#define HIWORD(n)			((uint16)((uint32)(n) >> 16))
#define LOWORD(n)			((uint16)(n))
#define LOWSIXBITS			63

typedef short ShortFrac;			/* 2.14 */

#ifdef NOT_ON_THE_MAC
	PASCAL Fixed FixRatio(short numer,short denom);
	PASCAL Fixed FixMul(Fixed a,Fixed b);
	PASCAL Fixed FixDiv(Fixed x,Fixed y);
	PASCAL Fract FracMul(Fract x,Fract y);
	PASCAL Fract FracDiv(Fract x,Fract y);
	PASCAL Fract FracSqrt(Fract x);
#else
#ifndef __TOOLUTILS__
	PASCAL Fixed FixRatio(short numer,short denom) = 0xA869;
	PASCAL Fixed FixMul(Fixed a,Fixed b) = 0xA868;
#endif
#ifndef __FIXMATH__
	PASCAL Fixed FixDiv(Fixed x,Fixed y) = 0xA84D;
	PASCAL Fract FracMul(Fract x,Fract y) = 0xA84A;
	PASCAL Fract FracDiv(Fract x,Fract y) = 0xA84B;
	PASCAL Fract FracSqrt(Fract x) = 0xA849;
#endif
#endif

long LongMulDiv(long a, long b, long c);		/* (a*b)/c */
long ShortFracMul(long a, ShortFrac b);
ShortFrac ShortFracDot(ShortFrac a, ShortFrac b);
int32 ShortMulDiv(int32 a, int16 b, int16 c);	/* (a*b)/c */
F26Dot6 Mul26Dot6(F26Dot6 a, F26Dot6 b);
F26Dot6 Div26Dot6(F26Dot6 a, F26Dot6 b);
