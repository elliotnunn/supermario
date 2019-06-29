/*
	File:		FontMath.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 11/9/90	MR		Fix CompDiv when numer and denom have zero hi longs. [rb]
		 <2>	 11/5/90	MR		Remove Types.h from include list, rename FixMulDiv to LongMulDiv
									[rb]
		 <1>	10/20/90	MR		Math routines for font scaler. [rj]

	To Do:
*/

#include "FSCdefs.h"
#include "FontMath.h"

void CompMul(long src1, long src2, long dst[2]);
long CompDiv(long src1, long src2[2]);


static void CompMul(long src1, long src2, long dst[2])
{
	int negative = (src1 ^ src2) < 0;
	register unsigned long dsthi, dstlo;

	if (src1 < 0)
		src1 = -src1;
	if (src2 < 0)
		src2 = -src2;
	{	unsigned short src1hi, src1lo;
		register unsigned short src2hi, src2lo;
		register unsigned long temp;
		src1hi = src1 >> 16;
		src1lo = src1;
		src2hi = src2 >> 16;
		src2lo = src2;
		temp = (unsigned long)src1hi * src2lo + (unsigned long)src1lo * src2hi;
		dsthi = (unsigned long)src1hi * src2hi + (temp >> 16);
		dstlo = (unsigned long)src1lo * src2lo;
		temp <<= 16;
		dsthi += (dstlo += temp) < temp;
		dst[0] = dsthi;
		dst[1] = dstlo;
	}
	if (negative)
		if (dstlo = -dstlo)
			dsthi = ~dsthi;
		else
			dsthi = -dsthi;
	dst[0] = dsthi;
	dst[1] = dstlo;
}

static long CompDiv(long src1, long src2[2])
{
	register unsigned long src2hi = src2[0], src2lo = src2[1];
	int negative = (long)(src2hi ^ src1) < 0;

	if ((long)src2hi < 0)
		if (src2lo = -src2lo)
			src2hi = ~src2hi;
		else
			src2hi = -src2hi;
	if (src1 < 0)
		src1 = -src1;
	{	register unsigned long src1hi, src1lo;
		unsigned long result = 0, place = 0x40000000;

		if ((src1hi = src1) & 1)
			src1lo = 0x80000000;
		else
			src1lo = 0;

		src1hi >>= 1;
		src2hi += (src2lo += src1hi) < src1hi;		/* round the result */

		if (src2hi > src1hi || src2hi == src1hi && src2lo >= src1lo)
			if (negative)
				return NEGINFINITY;
			else
				return POSINFINITY;
		while (place && src2hi)
		{	src1lo >>= 1;
			if (src1hi & 1)
				src1lo += 0x80000000;
			src1hi >>= 1;
			if (src1hi < src2hi)
			{	src2hi -= src1hi;
				src2hi -= src1lo > src2lo;
				src2lo -= src1lo;
				result += place;
			}
			else if (src1hi == src2hi && src1lo <= src2lo)
			{	src2hi = 0;
				src2lo -= src1lo;
				result += place;
			}
			place >>= 1;
		}
		if (src2lo >= src1)
			result += src2lo/src1;
		if (negative)
			return -result;
		else
			return result;
	}
}

/*
 *	a*b/c
 */
long LongMulDiv(long a, long b, long c)
{
	long temp[2];

	CompMul(a, b, temp);
	return CompDiv(c, temp);
}

#ifdef NOT_ON_MOTOROLA
long ShortFracMul(long a, ShortFrac b)
{
	int negative = false;
	uint16 al, ah;
	uint32 lowlong, midlong, hilong;

	if (a < 0) { a = -a; negative = true; }
	if (b < 0) { b = -b; negative ^= true; }

	al = LOWORD(a); ah = HIWORD(a);

	midlong = USHORTMUL(ah, b);
	hilong = midlong & 0xFFFF0000;
	midlong <<= 16;
	midlong += 1 << 13;
	lowlong = USHORTMUL(al, b) + midlong;
	if (lowlong < midlong)
		hilong += ONEFIX;

	midlong = (lowlong >> 14) | (hilong << 2);
	return negative ? -midlong : midlong;
}

int32 ShortMulDiv(int32 a, int16 b, int16 c)
{
	return LongMulDiv(a, b, c);
}
#endif

ShortFrac ShortFracDot(ShortFrac a, ShortFrac b)
{
	return SHORTMUL(a,b) + (1 << 13) >> 14;
}

#define FASTMUL26LIMIT		46340
/*
 *	Total precision routine to multiply two 26.6 numbers		<3>
 */
F26Dot6 Mul26Dot6(F26Dot6 a, F26Dot6 b)
{
	int negative = false;
	uint16 al, bl, ah, bh;
	uint32 lowlong, midlong, hilong;

	if ((a <= FASTMUL26LIMIT) && (b <= FASTMUL26LIMIT) && (a >= -FASTMUL26LIMIT) && (b >= -FASTMUL26LIMIT))
		return a * b + (1 << 5) >> 6;						/* fast case */

	if (a < 0) { a = -a; negative = true; }
	if (b < 0) { b = -b; negative ^= true; }

	al = LOWORD(a); ah = HIWORD(a);
	bl = LOWORD(b); bh = HIWORD(b);

	midlong = USHORTMUL(al, bh) + USHORTMUL(ah, bl);
	hilong = USHORTMUL(ah, bh) + HIWORD(midlong);
	midlong <<= 16;
	midlong += 1 << 5;
	lowlong = USHORTMUL(al, bl) + midlong;
	hilong += lowlong < midlong;

	midlong = (lowlong >> 6) | (hilong << 26);
	return negative ? -midlong : midlong;
}

#define FASTDIV26LIMIT	(1L << 25)
/*
 *	Total precision routine to divide two 26.6 numbers			<3>
 */
F26Dot6 Div26Dot6(F26Dot6 num, F26Dot6 den)
{
	int negative = false;
	register uint32 hinum, lownum, hiden, lowden, result, place;

	if (den == 0) return (num < 0 ) ? NEGINFINITY : POSINFINITY;

	if ( (num <= FASTDIV26LIMIT) && (num >= -FASTDIV26LIMIT) )			/* fast case */
		return (num << 6) / den;

	if (num < 0) { num = -num; negative = true; }
	if (den < 0) { den = -den; negative ^= true; }

	hinum = ((uint32)num >> 26);
	lownum = ((uint32)num << 6);
	hiden = den;
	lowden = 0;
	result = 0;
	place = HIBITSET;

	if (hinum >= hiden) return negative ? NEGINFINITY : POSINFINITY;

	while (place)
	{
		lowden >>= 1;
		if (hiden & 1) lowden += HIBITSET;
		hiden >>= 1;
		if (hiden < hinum)
		{
			hinum -= hiden;
			hinum -= lowden > lownum;
			lownum -= lowden;
			result += place;
		}
		else if (hiden == hinum && lowden <= lownum)
		{
			hinum = 0;
			lownum -= lowden;
			result += place;
		}
		place >>= 1;
	}

	return negative ? -result : result;
}

