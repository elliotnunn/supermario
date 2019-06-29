/************************************************************

Created: Thursday, September 7, 1989 at 8:55 PM
	FixMath.h
	C Interface to the Macintosh Libraries


	Copyright Apple Computer, Inc.	1985-1990
	All rights reserved

CHANGE LOG:
	19 Oct 90  JPO  Made Frac2x, Fix2x, X2Fix, and X2Frac
					  in-line trap calls in non-mc68881 mode.
					  
************************************************************/


#ifndef __FIXMATH__
#define __FIXMATH__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
pascal Fract Fix2Frac(Fixed x)
	= 0xA841;
pascal long Fix2Long(Fixed x)
	= 0xA840;
pascal Fixed FixATan2(long x,long y)
	= 0xA818;
pascal Fixed Long2Fix(long x)
	= 0xA83F;
pascal Fixed Frac2Fix(Fract x)
	= 0xA842;
	
#ifdef mc68881

pascal extended Frac2X(Fract x);
pascal extended Fix2X(Fixed x); 
pascal Fixed X2Fix(extended x); 
pascal Fract X2Frac(extended x);

#else

pascal extended Frac2X(Fract x)
	= 0xA845;
pascal extended Fix2X(Fixed x)
	= 0xA843; 
pascal Fixed X2Fix(extended x)
	= 0xA844; 
pascal Fract X2Frac(extended x)
	= 0xA846;

#endif

pascal Fract FracMul(Fract x,Fract y)
	= 0xA84A;
pascal Fixed FixDiv(Fixed x,Fixed y)
	= 0xA84D;
pascal Fract FracDiv(Fract x,Fract y)
	= 0xA84B;
pascal Fract FracSqrt(Fract x)
	= 0xA849;
pascal Fract FracSin(Fixed x)
	= 0xA848;
pascal Fract FracCos(Fixed x)
	= 0xA847;
#ifdef __cplusplus
}
#endif

#endif
