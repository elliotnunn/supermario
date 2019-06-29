/*
	File:		sc.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<17>	 10/1/91	RB		Updated so CubeE has speed-up changes included in Kirin code.
		<16>	  3/6/91	RB		OS,RJ,#84368: Change sc_DrawParabola so that it does not create
									zero-length vectors, which potentially cause problems later on
									in sc_mark.
		<15>	 2/11/91	RB		MR,CL,CC,#82410: Add comment about slop in veccount and initials
									for previous change.(date changed to be compatible
									with Reality Sources)
		<14>	 1/24/91	RB		(MR) #External Reported Bug. DropoutControl excluding stubs did
									not work properly on the corner of a glyph bounding box. Changed
									nUpperCrossings and nLowerCrossings.(date changed to be compatible
									with Reality Sources)
		<13>	12/20/90	RB			Add ZERO macro to include 0 degrees in definition of interior
									angle.  Set oncol in MarkRows when vector does not start on row
									but does start on column. [mr]
		<12>	12/18/90	RB		Revert to old definition of interior to not include 0 and 180
									degrees.[mr]
		<11>	12/10/90	RB		Change findextrema to return error if min,max are outside of
									-32768,32767 bounds. Redefine INTERIOR macro to include 0
									degrees and 180 degrees as true. Optimized fillonerow. [cel]
		<10>	 12/5/90	RB		Fix fill routine to agree with spec and do non-zero winding
									number fill rather than positive winding number fill.[mr]
		 <9>	11/21/90	RB		A few optimizations in findextrema, sortRows, sortCols,
									orSomeBits, invpixsegX, invpixsegY. [This is a reversion to <7>
									so mr's initials are added by proxy]
		 <8>	11/13/90	MR		(dnf) Revert back one revision to fix a memmory-trashing bug (we
									hope).
		 <7>	11/13/90	RB		Fix banding so that we can band down to one row, using only
									enough bitmap memory and auxillary memory for one row.[mr]
		 <6>	 11/5/90	MR		type cast NEGONE constant, remove QD includes [rb]
		 <5>	10/31/90	RB		No Change
		 <4>	10/31/90	RB		[MR]Fix weitek bug of long<<32 undefined
		 <3>	10/30/90	RB		[MR] Projector screwup - starting over with new version. Code
									Review Changes speeded up scanconverter by up to 33%. Made
									macros from functions INTERIOR, FILLONEROW, ISORT Unrecursed
									compare and put inline. Renamed sort3rows->sortrows,
									sort3cols->sortcols Undid fix <14>, because fs_contourscan now
									inhibits dropoutcontrol when banding

	To Do:
*/

/*
	File:		sc.c

	Contains:	xxx put contents here (or delete the whole line) xxx

	Written by:	xxx put name of writer here (or delete the whole line) xxx

	Copyright:	© 1987-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
		<15>	 9/26/90	CL		Messed up on adding in the version comment. Needed some comment
									brackets!!!
		<14>	 9/26/90	CL		A bug in sc_orsomebits caused memory trashing.  This routine did
									not take into account that banding could occur and would write
									outside of the bitmap band.  This fix checks to see if we are
									banding and skips this routine.
		<13>	 8/22/90	RB		fixed nUppercrossings and nLowercrossings to not access out of
									range scan lines.  Moved functions around in module to be more
									readable.
		<12>	 8/18/90	RB		new sort routine, took out multlong(),
		<11>	 7/18/90	RB		fixed bug in invpixyseg - stray pixels undone
		<10>	 7/18/90	MR		sc_ScanChar now returns error code as int
		 <9>	 7/13/90	MR		Ansi-C stuff, added some static prototypes
		 <8>	 6/22/90	RB		added stubcontrol option to orsomebits
		 <7>	  6/1/90	RB		fix banding/dropout bug
		 <6>	 5/10/90	RB		fix overlapping points that lie on scanlines
		<4+>	  5/8/90	RB		fix scanning of glyphs that collapse to line and overlapping
									points that are on scanlines.
		 <4>	  5/3/90	RB		  Almost completely new scanconverter.  Winding number fill,
									dropout control.
									  Took out Fixupper requirement on dropout control.
		 <3>	 2/27/90	CL		Dropoutcontrol scanconverter and SCANCTRL[] instruction
	   <3.0>	 8/28/89	sjk		Cleanup and one transformation bugfix
	   <2.2>	 8/14/89	sjk		1 point contours now OK
	   <2.1>	  8/8/89	sjk		Improved encryption handling
	   <2.0>	  8/2/89	sjk		Just fixed EASE comment
	   <1.7>	  8/1/89	sjk		Added composites and encryption. Plus some enhancements…
	   <1.6>	 6/13/89	SJK		Comment
	   <1.5>	  6/2/89	CEL		16.16 scaling of metrics, minimum recommended ppem, point size 0
									bug, correct transformed integralized ppem behavior, pretty much
									so
	   <1.4>	 5/26/89	CEL		EASE messed up on “c” comments
	  <•1.3>	 5/26/89	CEL		Integrated the new Font Scaler 1.0 into Spline Fonts

	To Do:
*/
/*
 * File: sc.c
 *
 * This module scanconverts a shape defined by quadratic B-splines
 *
 * The BASS project scan converter sub ERS describes the workings of this module.
 *
 *
 *  © Apple Computer Inc. 1987, 1988, 1989, 1990.
 *
 * History:
 * Work on this module began in the fall of 1987.
 * Written June 14, 1988 by Sampo Kaasila.
 *
 * Released for alpha on January 31, 1989.
 * 
 * Added experimental non breaking scan-conversion feature, Jan 9, 1990. ---Sampo
 * 
 */

#include    "FSCdefs.h"
#include    "sc.h"
#include    "FSError.h"


#ifdef SEGMENT_LINK
#pragma segment SC_C
#endif

/* Private prototypes */
				
static void sc_mark( int32 x0, int32 y0, int32 x1, int32 y1, int32 bx, int32 by,
				int16 **xBase, int16 **yBase, sc_BitMapData *bbox );
				
static void sc_markRows( int32 x0, int32 y0, int32 x1, int32 y1, int32 xb, int32 yb,
	 int16 **yBase, sc_BitMapData *bbox, int16** lowRowP, int16** highRowP );
	
static void sc_lineGen(sc_BitMapData *bbox, int32 *px, int32 *py, int32 *pend,
						int16 **xBase, int16 **yBase );
static void sc_lineGenRows(sc_BitMapData *bbox, int32 *px, int32 *py, int32 *pend,
					 int16 **yBase, int16** lowRowP, int16** highRowP );
static void sortRows( sc_BitMapData *bbox, int16** lowRowP, int16** highRowP );

static void sortCols( sc_BitMapData *bbox );

static uint32* compare( int16 coord, int16* lastBitP, uint32* tempP, uint32* row, int16 windNbr);

static int32 sc_DrawParabola(int32 Ax, int32 Ay, int32 Bx, int32 By, int32 Cx, int32 Cy, int32 **hX, int32 **hY, int32 *count, int32 inGY);

static void sc_wnNrowFill( int32 rowM, int16 nRows, sc_BitMapData *bbox );

static void sc_orSomeBits( sc_BitMapData *bbox, int32 scanKind);

static int16** sc_lineInit( int16* arrayBase, int16** rowBase, int16 nScanlines, int16 maxCrossings,
							int16 minScanline );
static int32 nOnOff( int16** base, int32 k, int16 val, int32 nChanges );

static int32 nUpperXings(int16** lineBase, int16** valBase, int16 line, int16 val,
						int16 lineChanges, int16 valChanges, int16 valMin, int16 valMax, int16 lineMax); /*<14>*/
static int32 nLowerXings(int16** lineBase, int16** valBase, int16 line, int16 val,
						int16 lineChanges, int16 valChanges, int16 valMin, int16 valMax, int16 lineMin); /*<14>*/
static void invpixSegY(int16 llx, uint16 k, uint32* bitmapP );

static void invpixSegX(int16 llx, uint16 k, uint32* bitmapP );

static void invpixOn(int16 llx, uint16 k, uint32* bitmapP );

/*
 * Returns the bitmap
 * This is the top level call to the scan converter.
 *
 * Assumes that (*handle)->bbox.xmin,...xmax,...ymin,...ymax
 * are already set by sc_FindExtrema()
 *
 * PARAMETERS:
 *
 *
 * glyphPtr is a pointer to sc_CharDataType
 * scPtr is a pointer to sc_GlobalData.
 * lowBand   is lowest scan line to be included in the band.
 * highBand  is one greater than the highest scan line to be included in the band. <7>
 * scanKind contains flags that specify whether to do dropout control and what kind
 * 	0 -> no dropout control
 *	bits 0-15 not equal 0 -> do dropout control
 *	if bit 16 is also on, do not do dropout control on 'stubs' 
*/
int sc_ScanChar(sc_CharDataType *glyphPtr, sc_GlobalData *scPtr, sc_BitMapData *bbox,
			int16 lowBand, int16 highBand, int32 scanKind) 
{
	register F26Dot6 *x = glyphPtr->x;
	register F26Dot6 *y = glyphPtr->y;
    register ArrayIndex i, endPt, nextPt;
	register char *onCurve = glyphPtr->onC;
	ArrayIndex startPt, j;
	LoopCount ctr;
	sc_GlobalData *p;
    F26Dot6 *xp, *yp, *x0p, *y0p;
    register F26Dot6 xx, yy, xx0, yy0;
	int quit;
	int16 **lowRowP, **highRowP;
	int32 vecCount;
			
	if( scanKind )
	{
		bbox->xBase = sc_lineInit( bbox->xLines, bbox->xBase, bbox->xMax - bbox->xMin, bbox->nXchanges,
									bbox->xMin );	
		bbox->yBase = sc_lineInit( bbox->yLines, bbox->yBase, bbox->yMax - bbox->yMin, bbox->nYchanges,
									bbox->yMin);
	}
	else
	{
		bbox->yBase = sc_lineInit( bbox->yLines, bbox->yBase, highBand - lowBand, bbox->nYchanges,
									lowBand );
	}
	lowRowP = bbox->yBase + lowBand;			
	highRowP = bbox->yBase + highBand - 1;
	
	if( glyphPtr->ctrs == 0 ) return NO_ERR;
	p = scPtr;
	for ( ctr = 0; ctr < glyphPtr->ctrs; ctr++ ) {
		x0p = xp = p->xPoints;
        y0p = yp = p->yPoints;
	    startPt = i = glyphPtr->sp[ctr];		
        endPt = glyphPtr->ep[ctr];
		
		if ( startPt == endPt ) continue; 
		quit = 0;
		vecCount = 1;
		if ( onCurve[i] & ONCURVE) {
 		    *xp++ = xx = x[i];
		    *yp++ = yy = y[i++];
		} else {
		    if ( onCurve[endPt] & ONCURVE ) {
			    startPt = endPt--;
				*xp++ = xx = x[startPt];
				*yp++ = yy = y[startPt];
			} else {
				*xp++ = xx = (x[i] + x[endPt] + 1) >> 1;
				*yp++ = yy = (y[i] + y[endPt] + 1) >> 1;
    			goto Offcurve;
			}
		}
	    while ( true ) {
		    while ( onCurve[i] & ONCURVE )
			{
				if( ++vecCount > MAXVECTORS ) 
				{ /*Ran out of local memory. Consume data and continue. */
					if ( scanKind )
							sc_lineGen(bbox, x0p, y0p, yp-1, bbox->xBase, bbox->yBase );
					else sc_lineGenRows(bbox, x0p, y0p, yp-1, bbox->yBase, lowRowP, highRowP );
					x0p = p->xPoints + 2;		/* save data in points 0 and 1 for final */
					y0p = p->yPoints + 2;
					*x0p++ = *(xp-2);			/* save last vector to be future previous vector */
					*x0p++ = *(xp-1);
					*y0p++ = *(yp-2);
					*y0p++ = *(yp-1);
					xp = x0p;					/* start next processing with last vector */
					x0p = p->xPoints + 2;
					yp = y0p;
					y0p = p->yPoints + 2;
					vecCount = 5;				/* probably could be 4 <15> */
				}
			    *xp++ = xx = x[i];
		        *yp++ = yy = y[i];
				if ( quit ) {
					goto sc_exit;
				} else {
   	                i = i == endPt ? quit = 1, startPt : i + 1;
				}
			}

			do {
Offcurve:		xx0 = xx;
				yy0 = yy;
			    /* nextPt = (j = i) + 1; */
				j = i;
                nextPt = i == endPt ? quit = 1, startPt : i + 1;
				if ( onCurve[nextPt] & ONCURVE ) {
				    xx = x[nextPt];
					yy = y[nextPt];
					i = nextPt;
				} else {
					xx = (x[i] + x[nextPt] + 1) >> 1;
					yy = (y[i] + y[nextPt] + 1) >> 1;
				}
                if ( sc_DrawParabola( xx0, yy0, x[j], y[j], xx, yy,
				                      &xp, &yp, &vecCount, -1) )
				{ /* not enough room to create parabola vectors  */
					if ( scanKind )
							sc_lineGen(bbox, x0p, y0p, yp-1, bbox->xBase, bbox->yBase );
					else sc_lineGenRows(bbox, x0p, y0p, yp-1, bbox->yBase, lowRowP, highRowP );
					
					x0p = p->xPoints + 2;
					y0p = p->yPoints + 2;
					*x0p++ = *(xp-2);
					*x0p++ = *(xp-1);
					*y0p++ = *(yp-2);
					*y0p++ = *(yp-1);
					xp = x0p;
					x0p = p->xPoints + 2;
					yp = y0p;
					y0p = p->yPoints + 2;
					vecCount = 5;
					/* recaptured some memory, try again, if still wont work, MAXVEC is too small */
                	if ( sc_DrawParabola( xx0, yy0, x[j], y[j], xx, yy,
				                      &xp, &yp, &vecCount, -1) ) return SCAN_ERR;
				}
				if ( quit ) {
					goto sc_exit;
				} else {
   	                i = i == endPt ? quit = 1, startPt : i + 1;
				}
            } while ( ! (onCurve[i] & ONCURVE) );

    	}
sc_exit:

		if ( scanKind )
		{
			sc_lineGen(bbox, x0p, y0p, yp-1, bbox->xBase, bbox->yBase );
			sc_mark( *(p->xPoints), *(p->yPoints), *(p->xPoints+1), *(p->yPoints+1),
				*(xp-2), *(yp-2), bbox->xBase, bbox->yBase, bbox);
		}
		else
		{
			sc_lineGenRows(bbox, x0p, y0p, yp-1, bbox->yBase, lowRowP, highRowP );
			sc_markRows( *(p->xPoints), *(p->yPoints), *(p->xPoints+1), *(p->yPoints+1),
				*(xp-2), *(yp-2), bbox->yBase, bbox, lowRowP, highRowP );
		}
	}
	
	sortRows( bbox, lowRowP, highRowP ); 
	if ( scanKind ) sortCols( bbox );
	
/* Take care of problem of very small thin glyphs - always fill at least one pixel
   Should this only be turned on if dropout control ?? 
 */
	if( highRowP < lowRowP )
	{
		register int16 *p = *lowRowP;
		register int16 *s = p + bbox->nYchanges+1;
		++*p; *(p+*p) = bbox->xMin;
		++*s; *(s-*s) = bbox->xMax == bbox->xMin ? bbox->xMin+1 : bbox->xMax;
		highBand = lowBand+1;
	}
	else if( bbox->xMin == bbox->xMax )
	{
		register int16 *p;
		register int16 inc = bbox->nYchanges;
		for( p = *lowRowP; p <= *highRowP; p += inc+1 )
		{
			*p=1;
			*(p+inc) = bbox->xMin+1; 
			*(++p) = bbox->xMin;
			*(p+inc) = 1;
		}
	}

	sc_wnNrowFill( lowBand, highBand - lowBand , bbox );
	
 	if ( scanKind ) 
 		sc_orSomeBits( bbox, scanKind );

	return NO_ERR;
}
/* rwb 11/29/90 - modify the old positive winding number fill to be 
 * a non-zero winding number fill to be compatible with skia, postscript,
 * and our documentation.
 */

#define NEGONE 		((uint32)0xFFFFFFFF)

/* x is pixel position, where 0 is leftmost pixel in scanline. 
 * if x is not in the long pointed at by row, set row to the value of temp, clear
 * temp, and clear all longs up to the one containing x.  Then set the bits
 * from x mod 32 through 31 in temp.
 */
#define CLEARUpToAndSetLoOrder( x, lastBit, row, temp )		\
{															\
	if( x >= lastBit )										\
	{														\
		*row++ = temp;										\
		temp = 0;											\
		lastBit += 32;										\
	}														\
	while( x >= lastBit )									\
	{														\
		*row++ = 0;											\
		lastBit += 32;										\
	}														\
	temp |= (NEGONE >> (32 + x - lastBit)); 				\
}

/* x is pixel position, where 0 is leftmost pixel in scanline. 
 * if x is not in the long pointed at by row, set row to the value of temp, set
 * all bits in temp, and set all bits in all longs up to the one containing x.
 * Then clear the bits from x mod 32 through 31 in temp.
 */
#define SETUpToAndClearLoOrder( x, lastBit, row, temp )     \
{															\
	if( x > lastBit )		/*<4>*/							\
	{														\
		*row++ = temp;										\
		temp = NEGONE;										\
		lastBit += 32;										\
	}														\
	while( x > lastBit )	/*<4>*/							\
	{														\
		*row++ = NEGONE;									\
		lastBit += 32;										\
	}														\
	temp &= (NEGONE << (lastBit - x)); 						\
}

#define FILLONEROW( row, longsWide, line, lineWide, xMin )  													\
/* do a winding number fill of one row of a bitmap from two sorted arrays \
of onTransitions and offTransitions. 								\
*/																	\
{																	\
register int16 moreOns, moreOffs;									\
register int16 *onTp, *offTp;										\
register uint32 temp;												\
uint32 *rowEnd = row + longsWide;									\
int32  windNbr, lastBit, on, off;									\
																	\
lastBit = 32 + xMin;												\
windNbr  = 0;														\
temp = 0;															\
moreOns = *line;													\
onTp = line+1;														\
offTp = line + lineWide - 1;										\
moreOffs = *offTp;													\
offTp -= moreOffs;													\
																	\
while( moreOns || moreOffs )										\
{																	\
	if( moreOns )													\
	{																\
		on = *onTp;													\
		if( moreOffs )												\
		{															\
			off = *offTp;											\
			if( on < off )											\
			{														\
				--moreOns;											\
				++onTp;												\
				++windNbr;											\
				if( windNbr == 1)									\
					CLEARUpToAndSetLoOrder( on, lastBit, row, temp )\
				else if( windNbr == 0 )								\
					SETUpToAndClearLoOrder( on, lastBit, row, temp )\
			}														\
			else if( on > off )										\
			{														\
				--moreOffs;											\
				++offTp;											\
				--windNbr;											\
				if( windNbr == 0)									\
					SETUpToAndClearLoOrder( off, lastBit, row, temp)\
				else if( windNbr == -1 )							\
					CLEARUpToAndSetLoOrder( off, lastBit, row, temp)\
			}														\
			else													\
			{														\
				--moreOns;											\
				++onTp;												\
				--moreOffs;											\
				++offTp;											\
			}														\
		}															\
		else    						/* no more offs left */		\
		{															\
			--moreOns;												\
			++onTp;													\
			++windNbr;												\
			if( windNbr == 1)										\
				CLEARUpToAndSetLoOrder( on, lastBit, row, temp )	\
			else if( windNbr == 0 )									\
				SETUpToAndClearLoOrder( on, lastBit, row, temp )	\
		}															\
	}																\
	else								/* no more ons left */		\
	{																\
		off = *offTp;												\
		--moreOffs;													\
		++offTp;													\
		--windNbr;													\
		if( windNbr == 0)											\
			SETUpToAndClearLoOrder( off, lastBit, row, temp)		\
		else if( windNbr == -1 )									\
			CLEARUpToAndSetLoOrder( off, lastBit, row, temp)		\
	}																\
}																	\
*row = temp;														\
while( ++row < rowEnd ) *row = 0; 									\
}																	\
/* Winding number fill of nRows of a glyph beginning at rowM, using two sorted 
arrays of onTransitions and offTransitions. 
*/

static void sc_wnNrowFill( int32 rowM, int16 nRows, sc_BitMapData *bbox )
{
	uint32  longsWide = bbox->wide>>5;
	uint32  lineWide = bbox->nYchanges + 2;
	uint32 *rowB = bbox->bitMap;
	int16  *lineB = *(bbox->yBase + rowM + nRows - 1);
	int32   xMin = bbox->xMin;
	while( nRows-- > 0 )
	{
		uint32 *row = rowB;
		int16 *line = lineB;
		FILLONEROW( row, longsWide, line, lineWide, xMin )
		rowB += longsWide;
		lineB -= lineWide;
	}
}
#undef NEGONE


/* Sort the values stored in locations pBeg to pBeg+nVal in ascending order
*/
#define ISORT( pBeg, pVal )								\
{														\
	register int16 *pj = pBeg;							\
	register int16 nVal = *pVal - 2;					\
	for( ; nVal >= 0; --nVal )							\
	{													\
		register int16 v;								\
		register int16 *pk, *pi;						\
														\
		pk = pj;										\
		pi = ++pj;										\
		v = *pj;										\
		while( *pk > v && pk >= pBeg ) *pi-- = *pk--;	\
		*pi = v;										\
	}													\
}														\

/* rwb 4/5/90 Sort OnTransition and OffTransitions in Xlines arrays */
static void sortCols( sc_BitMapData *bbox )
{
register int16 nrows = bbox->xMax - bbox->xMin - 1;
register int16 *p = bbox->xLines;
register uint32 n = bbox->nXchanges + 1; 			/*<9>*/

	for(; nrows >= 0; --nrows )
	{
		ISORT( p+1, p );
		p += n;										/*<9>*/
		ISORT( p-*p, p );
		++p;
	}
}

/* rwb 4/5/90 Sort OnTransition and OffTransitions in Ylines arrays */
static void sortRows( sc_BitMapData *bbox, int16** lowRowP, int16** highRowP )
{
register uint32 n = bbox->nYchanges + 1; 			/*<9>*/
int16 *p, *pend;

if( highRowP < lowRowP ) return;
p = *lowRowP;
pend = *highRowP;
do
{
	ISORT( p+1, p );
	p += n;											/*<9>*/
	ISORT( p-*p, p );
	++p; 
}
while(p <= pend);
}

/* Generate scan line intersections from series of vectors.  Always need previous
vector to properly generate data for current vector.  pend typically points
one beyond last valid vector endpoint.  4/6/90
*/
static void sc_lineGen(sc_BitMapData *bbox, int32 *px, int32 *py, int32 *pend,
		int16 **xBase, int16 **yBase )
{
int32 x0, x1, y0, y1, xb, yb;
x0 = *px++;
y0 = *py++;
x1 = *px++;
y1 = *py++;
while (py <= pend)
{
	xb = x0;
	yb = y0;
	x0 = x1;
	y0 = y1;
	x1 = *px++;
	y1 = *py++;
	sc_mark( x0, y0, x1, y1, xb, yb, xBase, yBase, bbox);
}
}

/* Generate row scan line intersections from series of vectors.  Always need previous
vector to properly generate data for current vector.  pend typically points
one beyond last valid vector endpoint.  4/6/90
*/
static void sc_lineGenRows(sc_BitMapData *bbox, int32 *px, int32 *py, int32 *pend,
	 int16 **yBase, int16** lowRowP, int16** highRowP )
{
int32 x0, x1, y0, y1, xb, yb;

if( highRowP < lowRowP ) return;

x0 = *px++;
y0 = *py++;
x1 = *px++;
y1 = *py++;
while (py <= pend)
{
	xb = x0;
	yb = y0;
	x0 = x1;
	y0 = y1;
	x1 = *px++;
	y1 = *py++;
	sc_markRows( x0, y0, x1, y1, xb, yb, yBase, bbox, lowRowP, highRowP);
}
}


/* 4/4/90 Version that distinguishes between On transitions and Off transitions.
*/
/* 3/23/90 
* 	A procedure to find and mark all of the scan lines (both row and column ) that are
* crossed by a vector.  Many different cases must be considered according to the direction
* of the vector, whether it is vertical or slanted, etc.  In each case, the vector is first
* examined to see if it starts on a scan-line.  If so, special markings are made and the
* starting conditions are adjusted.  If the vector ends on a scan line, the ending 
* conditions must be adjusted.  Then the body of the case is done.
* 	Special adjustments must be made when a vector starts or ends on a scan line. Whenever
* one vector starts on a scan line, the previous vector must have ended on a scan line.
* Generally, this should result in the line being marked as crossed only once (conceptually
* by the vector that starts on the line.  But, if the two lines form a vertex that
* includes the vertex in a colored region, the line should be marked twice.  If the 
* vertex is also on a perpendicular scan line, the marked scan line should be marked once
* on each side of the perpendicular line.  If the vertex defines a point that is jutting
* into a colored region, then the line should not be marked at all. In order to make
* these vertex crossing decisions, the previous vector must be examined.
*	
*	Because many vectors are short with respect to the grid for small resolutions, the
* procedure first looks for simple cases in which no lines are crossed.
*
* xb, x0, and x1 are x coordinates of previous point, current point and next point
* similaryly yb, y0 and y1
* 	ybase points to an array of pointers, each of which points to an array containing
* information about the glyph contour crossings of a horizontal scan-line.  The first
* entry in these arrays is the number of ON-transition crossings, followed by the y
* coordinates of each of those crossings.  The last entry in each array is the number of
* OFF-transtion crossings, preceded by the Y coordinates for each of these crossings.
* 	xBase contains the same information for the column scan lines.
*/
#define DROUND(a) ((a + HALFM) & INTPART)
#define RSH(a) (int16)(a>>PIXSHIFT)
#define LSH(a) (a<<PIXSHIFT)
#define LINE(a)   ( !((a & FRACPART) ^ HALF))
#define SET(p,val) {register int16 *row = *p; ++*row; *(row+*row)=val;}
#define OFFX(val) {register int16 *s = *px+wideX; ++*s; *(s-*s) = val;}
#define OFFY(val) {register int16 *s = *py+wideY; ++*s; *(s-*s) = val;}
#define BETWEEN(a,b,c) (a < b && b < c )
#define INTERIOR  ((x0-xb)*dy < (y0-yb)*dx)
#define ZERO( prev, mid, next, more, less ) (prev == mid && next == mid && more > less )

static void sc_mark( int32 x0, int32 y0, int32 x1, int32 y1, int32 xb, int32 yb,
int16 **xBase, int16 **yBase, sc_BitMapData *bbox )
{
int16 jx, jy, endx, endy, onrow, oncol, wideX, wideY;
register int16 **px, **py;
int16 **pend;
int32 rx0, ry0, rx1, ry1, dy, dx, rhi, rlo;
register int32  r, incX, incY;

rx0 = DROUND( x0 );
jx = RSH( rx0 );
ry0 = DROUND( y0 );
jy = RSH( ry0 );
rx1 = DROUND( x1 );
endx = RSH( rx1 );
ry1 = DROUND( y1 );
endy = RSH( ry1 );
py = yBase + jy;
px = xBase + jx;
dy = y1 - y0;
dx = x1 - x0;
onrow = false;
oncol = false;
wideX = bbox->nXchanges+1;
wideY = bbox->nYchanges+1;

if( dy >= 0 && dx > 0 )					/* FIRST QUADRANT CASE include ---> */
{
	if LINE(y0)								/* vector starts on row scan line */
	{				
		onrow = true;
		if LINE(x0)							/* can also start on column scan line */
		{
			oncol = true;
			if( INTERIOR || ZERO( yb, y0, y1, xb, x0 ) )
			{
				SET( py, jx )
				OFFX( jy+1 )
				if( xb > x0 ) SET( px, jy )
				if( yb > y0 ) OFFY( jx+1 )
			}
			else
			{
				if BETWEEN(xb, x0, x1) OFFX( jy+1 )
				if BETWEEN(yb, y0, y1) SET( py, jx )
			}
		}
		else								/* starts on row scan but not col */
		{
			if( INTERIOR || ZERO( yb, y0, y1, xb, x0 ))
			{
				SET( py, jx )
				if( yb > y0 ) OFFY( jx )
			}
			else if BETWEEN( yb, y0, y1 ) SET( py, jx )
		}
				
	}
	else if LINE(x0)						/* starts on col scan line but not row */
	{
		oncol = true;
		if( INTERIOR || ZERO( yb, y0, y1, xb, x0 ))
		{
			OFFX( jy )
			if( xb > x0 ) SET( px, jy )		/* cross extremum vertex twice */
		}
		else if BETWEEN( xb, x0, x1 ) OFFX( jy )
	}
	
	if( endy == jy )						/* horizontal line */
	{
		if( endx == jx ) return;
		if( onrow ) ++jy;
		if( oncol ) ++px;
		pend = xBase + endx;
		while( px < pend )
		{
			OFFX( jy )
			++px;
		}
		return;
	}
	
	if( endx == jx )
	{
		pend = yBase + endy;
		if( onrow ) ++py;
		while( py < pend )					/* note oncol can't be true */
		{
			SET( py, jx )
			++py;
		}
		return;
	}
	
	if(! onrow ) rhi = (ry0 - y0 + HALF) * dx;
	else
	{
		rhi = LSH( dx );
		++jy;
		++py;
	}
	if(! oncol ) rlo =  (rx0 - x0 + HALF) * dy;
	else
	{
		rlo = LSH( dy );
		++jx;
		++px;
	}
	r = rhi - rlo;
	incX = LSH( dx );
	incY = LSH( dy );
	do 
	{
		if( r > 0) 
		{
			if( jx == endx ) break;
			OFFX( jy );
			++px;
			++jx;
			r -= incY;
		}
		else
		{
			if( jy == endy ) break;
			SET( py, jx );
			++py;
			++jy;
			r += incX;
		}
	} while (true );
}	
else if( dy > 0 && dx <= 0 )					/* SECOND QUADRANT CASE include vertical up*/
	{
		if LINE(y0)								/* vector starts on row scan line */
		{				
			onrow = true;
			if LINE(x0)							/* can also start on column scan line */
			{
				oncol = true;
				if (INTERIOR || ZERO( xb, x0, x1, yb, y0 ))
				{
					SET( py, jx )
					SET( px, jy )
					if( xb < x0 ) OFFX( jy+1 )
					if( yb > y0 ) OFFY( jx+1 )
				}
				else
				{
					if BETWEEN(yb, y0, y1) SET( py, jx );
					if BETWEEN(x1, x0, xb) SET( px, jy );
				}
			}
			else								/* starts on row scan but not col */
			{
				if( INTERIOR || ZERO( xb, x0, x1, yb, y0 ))
				{
					SET( py, jx )
					if( yb > y0 ) OFFY( jx )
				}
				else if BETWEEN(yb, y0, y1) SET( py, jx)
			}
		}
		else if LINE(x0)						/* starts on col scan line but not row */
		{
			oncol = true;
			if( INTERIOR || ZERO( xb, x0, x1, yb, y0 ))
			{
				SET( px, jy )
				if( xb < x0 ) OFFX( jy )		/* cross extremum vertex twice  */ 
			}
			else if BETWEEN( x1, x0, xb ) SET( px, jy )
		}
		
		if( endy == jy )
		{
			if( endx == jx ) return;
			if LINE(x1) ++endx;
			pend = xBase + endx;
			--px;
			while( px >= pend )					
			{
				SET( px, jy )
				--px;
			}
			return;
		}
		
		if( endx == jx )
		{
			pend = yBase + endy;
			if( onrow ) ++py;
			while( py < pend )					
			{
				SET( py, jx )
				++py;
			}
			return;
		}
		
		if(! onrow ) rhi = (ry0 - y0 + HALF) * dx;
		else
		{
			rhi = LSH( dx );
			++jy;
			++py;
		}
		if(! oncol ) rlo = (rx0 - x0 - HALF) * dy;
			else rlo = -LSH( dy );
			
		r = rhi - rlo;
		incX = LSH( dx );
		incY = LSH( dy );
		if LINE(x1) ++endx;						/* ends on scan line but dont mark it */
		do 
		{
			if( r <= 0) 
			{
				--jx;
				--px;
				if( jx < endx ) break;
				SET( px, jy );
				r += incY;
			}
			else
			{
				if( jy == endy ) break;
				SET( py, jx );
				++py;
				++jy;
				r += incX;
			}
		} while (true );
	}	
	else if( dy <= 0 && dx < 0 )				/* THIRD QUADRANT CASE includes <--- */
	{
		if LINE(y0)								/* vector starts on row scan line */
		{				
			onrow = true;
			if LINE(x0)							/* can also start on column scan line */
			{
				oncol = true;
				if( INTERIOR || ZERO( yb, y0, y1, x0, xb ))
				{
					OFFY( jx+1 )
					SET( px, jy )
					if( xb < x0 ) OFFX( jy+1 )
					if( yb < y0 ) SET( py, jx )
				}
				else
				{
					if BETWEEN(y1, y0, yb) OFFY( jx+1 );
					if BETWEEN(x1, x0, xb) SET( px, jy );
				}
			}
			else								/* starts on row scan but not col */
			{
				if( INTERIOR  || ZERO( yb, y0, y1, x0, xb ))
				{
					OFFY( jx )
					if( yb < y0 ) SET( py, jx )
				}
				else if BETWEEN(y1, y0, yb) OFFY( jx)
			}
		}
		else if LINE(x0)						/* starts on col scan line but not row */
		{
			oncol = true;
			if( INTERIOR  || ZERO( yb, y0, y1, x0, xb ))
			{
				SET( px, jy )
				if( xb < x0 ) OFFX( jy )		/* cross extremum vertex twice  */ 
			}
			else if BETWEEN( x1, x0, xb ) SET( px, jy )
		}
		
		if( endy == jy )
		{
			if( endx == jx ) return;
			if LINE(x1) ++endx;
			pend = xBase + endx;
			--px;
			while( px >= pend )					
			{
				SET( px, jy )
				--px;
			}
			return;
		}
		
		if( endx == jx )
		{
			if LINE( y1 ) ++endy;
			pend = yBase + endy;
			--py;
			while( py >= pend )					
			{
				OFFY( jx )
				--py;
			}
			return;
		}
		
		if(! onrow ) rhi = (ry0 - y0 - HALF) * dx;
			else rhi = -LSH( dx );
		
		if(! oncol ) rlo = (rx0 - x0 - HALF) * dy;
			else rlo = -LSH( dy );
		
		r = rhi - rlo;
		incX = LSH( dx );
		incY = LSH( dy );
		if LINE(y1) ++endy;						/* ends on scan line but dont mark it */
		if LINE(x1) ++endx;						/* ends on scan line but dont mark it */
		do 
		{
			if( r > 0) 
			{
				--jx;
				--px;
				if( jx < endx ) break;
				SET( px, jy );
				r += incY;
			}
			else
			{
				--jy;
				--py;
				if( jy < endy ) break;
				OFFY( jx );
				r -= incX;
			}
		} while (true );
	}	
	else if( dy < 0 && dx >= 0 )					/* FOURTH QUADRANT CASE includes vertical down */
	{
		if LINE(y0)								/* vector starts on row scan line */
		{				
			onrow = true;
			if LINE(x0)							/* can also start on column scan line */
			{
				oncol = true;
				if( INTERIOR || ZERO( xb, x0, x1, y0, yb ))
				{
					OFFY( jx+1 )
					OFFX( jy+1 )
					if( xb > x0 ) SET( px, jy )
					if( yb < y0 ) SET( py, jx )
				}
				else
				{
					if BETWEEN(y1, y0, yb) OFFY( jx+1 );
					if BETWEEN(xb, x0, x1) OFFX( jy+1 );
				}
			}
			else								/* starts on row scan but not col */
			{
				if( INTERIOR || ZERO( xb, x0, x1, y0, yb ))
				{
					OFFY( jx )
					if( yb < y0 ) SET( py, jx )
				}
				else if BETWEEN(y1, y0, yb) OFFY( jx)
			}
		}
		else if LINE(x0)						/* starts on col scan line but not row */
		{
			oncol = true;
			if( INTERIOR || ZERO( xb, x0, x1, y0, yb ))
			{
				OFFX( jy )
				if( xb > x0 ) SET( px, jy )		/* cross extremum vertex twice  */ 
			}
			else if BETWEEN( xb, x0, x1 ) OFFX( jy )
		}
		
		if( endy == jy )
		{
			if( endx == jx ) return;
			pend = xBase + endx;
			if( oncol ) ++px;
			while( px < pend )
			{
				OFFX( jy )
				++px;
			}
			return;
		}
		
		if( endx == jx )
		{
			if( oncol ) ++jx;					/* straight down line on col scan line */
			if LINE( y1 ) ++endy;
			pend = yBase + endy;
			--py;
			while( py >= pend )					
			{
				OFFY( jx )
				--py;
			}
			return;
		}
		if(! onrow ) rhi = (ry0 - y0 - HALF) * dx;
			else rhi = -LSH( dx );
		
		if(! oncol ) rlo = (rx0 - x0 + HALF) * dy;
		else
		{
			rlo = LSH( dy );
			++jx;
			++px;
		}

		r = rhi - rlo;
		incX = LSH( dx );
		incY = LSH( dy );
		if LINE(y1) ++endy;						/* ends on scan line but dont mark it */
		do 
		{
			if( r <= 0) 
			{
				if( jx == endx ) break;
				OFFX( jy );
				++px;
				++jx;
				r -= incY;
			}
			else
			{
				--jy;
				--py;
				if( jy < endy ) break;
				OFFY( jx );
				r -= incX;
			}
		} while (true );
	}		
}

/* 4/6/90 Version for banding and no dropout control 
* Same Routine as sc_mark,
* EXCEPT No column scan lines are marked and only rows within band are marked
* To do banding and dropout control, use sc_mark to mark all the crossings, even
*	though only one band of the bit map will be filled.
*
* x0,y0 is the starting point of the vector.
* x1,y1 is the ending point of the vector.
* xb,yb is the starting point of the previous connecting vector.
* xBase is not used in the rows only version of this routine.
* yBase points at the array of pointers to the glyph scan intersection arrays.
* bbox points at the bitmap struture that has all the relevant pointers.
* lowRowP is the pointer to the lowest glyph scan row to be processed by this call.
* highRowP is the pointer to the highest glyph scan row to be processed by this call.
* 
*/

#undef	DROUND
#undef	RSH
#undef	LSH
#undef	LINE
#undef	SET
#undef	OFFY
#undef	BETWEEN
#undef	INTERIOR
#undef  ZERO

#define DROUND(a) ((a + HALFM) & INTPART)
#define RSH(a) (int16)(a>>PIXSHIFT)
#define LSH(a) (a<<PIXSHIFT)
#define LINE(a)   ( !((a & FRACPART) ^ HALF))
#define SET(p,val) {register int16 *t = *p; ++*t; *(t+*t)=val;}
#define OFFY(val) {register int16 *s = *py+wideY; ++*s; *(s-*s) = val;}
#define BETWEEN(a,b,c) (a < b && b < c )
#define INTERIOR  ((x0-xb)*dy < (y0-yb)*dx)
#define ZERO( prev, mid, next, more, less ) (prev == mid && next == mid && more > less )

static void sc_markRows( int32 x0, int32 y0, int32 x1, int32 y1, int32 xb, int32 yb,
int16 **yBase, sc_BitMapData *bbox, int16** lowRowP, int16** highRowP )
{
int16 jx, jy, endx, endy, onrow, oncol, wideX, wideY;
register int16 **py, *row;
int16 **pend;
int32 rx0, ry0, rx1, ry1, dy, dx, rhi, rlo;
register int32  r, incX, incY;

rx0 = DROUND( x0 );
jx = RSH( rx0 );
ry0 = DROUND( y0 );
jy = RSH( ry0 );
rx1 = DROUND( x1 );
endx = RSH( rx1 );
ry1 = DROUND( y1 );
endy = RSH( ry1 );
py = yBase + jy;
pend = yBase + endy;
dy = y1 - y0;
dx = x1 - x0;
onrow = false;
oncol = false;
wideX = bbox->nXchanges+1;
wideY = bbox->nYchanges+1;

if( dy >= 0 && dx > 0 )					/* FIRST QUADRANT CASE include ---> */
{
	if( py > highRowP || pend < lowRowP ) return;
	if( py >= lowRowP )
	{
		if LINE(y0)								/* vector starts on row scan line */
		{				
			onrow = true;
			if LINE(x0)							/* can also start on column scan line */
			{
				oncol = true;
				if( INTERIOR || ZERO( yb, y0, y1, xb, x0 ))
				{
					SET( py, jx )
					if( yb > y0 ) OFFY( jx+1 )
				}
				else
				{
					if BETWEEN(yb, y0, y1) SET( py, jx )
				}
			}
			else								/* starts on row scan but not col */
			{
				if( INTERIOR || ZERO( yb, y0, y1, xb, x0 ))
				{
					SET( py, jx )
					if( yb > y0 ) OFFY( jx )
				}
				else if BETWEEN( yb, y0, y1 ) SET( py, jx )
			}
					
		}
		else if LINE(x0) oncol = true;				/* starts on col scan line but not row */
	}
	if( endy == jy ) return;						/* horizontal line */
	
	if( endx == jx )
	{
		if( pend > highRowP ) pend = highRowP+1;
		if( onrow ) ++py;
		while( py < pend )					/* note oncol can't be true */
		{
			if( py >= lowRowP )
			{
				row = *py;
				++*row;
				*(row+*row) = jx;
			}
			++py;
		}
		return;
	}
	
	if(! onrow ) rhi = (ry0 - y0 + HALF) * dx;
	else
	{
		rhi = LSH( dx );
		++jy;
		++py;
	}
	if(! oncol ) rlo = (rx0 - x0 + HALF) * dy;
	else
	{
		rlo = LSH( dy );
		++jx;
	}
	r = rhi - rlo;
	incX = LSH( dx );
	incY = LSH( dy );
	do 
	{
		if( r > 0) 
		{
			if( jx == endx ) return;
			++jx;
			r -= incY;
		}
		else
		{
			if( jy == endy ) return;
			if( py > highRowP ) return;
			if( py >= lowRowP )
			{
				row = *py;
				++*row;
				*(row+*row) = jx;
			}
			++py;
			++jy;
			r += incX;
		}
	} while (true );
}	
else if( dy > 0 && dx <= 0 )					/* SECOND QUADRANT CASE include vertical up*/
	{
		if( py > highRowP || pend < lowRowP ) return;
		if( py >= lowRowP )
		{
			if LINE(y0)								/* vector starts on row scan line */
			{
				onrow = true;
				if LINE(x0)							/* can also start on column scan line */
				{
					oncol = true;
					if( INTERIOR || ZERO( xb, x0, x1, yb, y0 ))
					{
						SET( py, jx )
						if( yb > y0 ) OFFY( jx+1 )
					}
					else
					{
						if BETWEEN(yb, y0, y1) SET( py, jx );
					}
				}
				else								/* starts on row scan but not col */
				{
					if( INTERIOR || ZERO( xb, x0, x1, yb, y0 ))
					{
						SET( py, jx )
						if( yb > y0 ) OFFY( jx )
					}
					else if BETWEEN(yb, y0, y1) SET( py, jx)
				}
			}
			else if LINE(x0) oncol = true;				/* starts on col scan line but not row */
		}
		if( endy == jy ) return;
		
		if( endx == jx )
		{
			if( pend > highRowP ) pend = highRowP+1;
			if( onrow ) ++py;
			while( py < pend )					
			{
				if( py >= lowRowP )
				{
					row = *py;
					++*row;
					*(row+*row) = jx;
				}
				++py;
			}
			return;
		}
		
		if(! onrow ) rhi = (ry0 - y0 + HALF) * dx;
		else
		{
			rhi = LSH( dx );
			++jy;
			++py;
		}
		if(! oncol ) rlo = (rx0 - x0 - HALF) * dy;
			else rlo = -LSH( dy );
			
		r = rhi - rlo;
		incX = LSH( dx );
		incY = LSH( dy );
		if LINE(x1) ++endx;						/* ends on scan line but dont mark it */
		do 
		{
			if( r <= 0) 
			{
				--jx;
				if( jx < endx ) return;
				r += incY;
			}
			else
			{
				if( jy == endy ) return;
				if( py > highRowP ) return;
				if( py >= lowRowP )
				{
					row = *py;
					++*row;
					*(row+*row) = jx;
				}
				++py;
				++jy;
				r += incX;
			}
		} while (true );
	}	
	else if( dy <= 0 && dx < 0 )				/* THIRD QUADRANT CASE includes <--- */
	{
		if( py < lowRowP || pend > highRowP ) return;
		if( py <= highRowP )
		{
			if LINE(y0)								/* vector starts on row scan line */
			{				
				onrow = true;
				if LINE(x0)							/* can also start on column scan line */
				{
					oncol = true;
					if( INTERIOR || ZERO( yb, y0, y1, x0, xb ))
					{
						OFFY( jx+1 )
						if( yb < y0 ) SET( py, jx )
					}
					else
					{
						if BETWEEN(y1, y0, yb) OFFY( jx+1 );
					}
				}
				else								/* starts on row scan but not col */
				{
					if( INTERIOR || ZERO( yb, y0, y1, x0, xb ))
					{
						OFFY( jx )
						if( yb < y0 ) SET( py, jx )
					}
					else if BETWEEN(y1, y0, yb) OFFY( jx)
				}
			}
			else if LINE(x0) oncol = true;				/* starts on col scan line but not row */
		}
		if( endy == jy ) return;

		if( endx == jx )
		{
			if LINE( y1 ) ++endy;
			pend = yBase + endy;
			if( pend < lowRowP ) pend = lowRowP;
			--py;
			while( py >= pend )					
			{
				if( py <= highRowP ) OFFY( jx )
				--py;
			}
			return;
		}
		
		if(! onrow ) rhi = (ry0 - y0 - HALF) * dx;
			else rhi = -LSH( dx );
		
		if(! oncol ) rlo = (rx0 - x0 - HALF) * dy;
			else rlo = -LSH( dy );
		
		r = rhi - rlo;
		incX = LSH( dx );
		incY = LSH( dy );
		if LINE(y1) ++endy;						/* ends on scan line but dont mark it */
		if LINE(x1) ++endx;						/* ends on scan line but dont mark it */
		do 
		{
			if( r > 0) 
			{
				--jx;
				if( jx < endx ) return;
				r += incY;
			}
			else
			{
				--jy;
				--py;
				if( jy < endy ) return;
				if( py < lowRowP ) return;
				if( py <= highRowP ) OFFY( jx );
				r -= incX;
			}
		} while (true );
	}	
	else if( dy < 0 && dx >= 0 )					/* FOURTH QUADRANT CASE includes vertical down */
	{
		if( py < lowRowP || pend > highRowP ) return;
		if( py <= highRowP )
		{
			if LINE(y0)								/* vector starts on row scan line */
			{				
				onrow = true;
				if LINE(x0)							/* can also start on column scan line */
				{
					oncol = true;
					if( INTERIOR || ZERO( xb, x0, x1, y0, yb ))
					{
						OFFY( jx+1 )
						if( yb < y0 ) SET( py, jx )
					}
					else
					{
						if BETWEEN(y1, y0, yb) OFFY( jx+1 );
					}
				}
				else								/* starts on row scan but not col */
				{
					if( INTERIOR || ZERO( xb, x0, x1, y0, yb ))
					{
						OFFY( jx )
						if( yb < y0 ) SET( py, jx )
					}
					else if BETWEEN(y1, y0, yb) OFFY( jx)
				}
			}
			else if LINE(x0) oncol = true;				/* starts on col scan line but not row */
		}
		
		if( endy == jy ) return;
		
		if( endx == jx )
		{
			if( oncol ) ++jx;					/* straight down line on col scan line */
			if LINE( y1 ) ++endy;
			pend = yBase + endy;
			if( pend < lowRowP ) pend = lowRowP;
			--py;
			while( py >= pend )					
			{
				if( py <= highRowP ) OFFY( jx )
				--py;
			}
			return;
		}
		
		if(! onrow ) rhi = (ry0 - y0 - HALF) * dx;
			else rhi = -LSH( dx );
		
		if(! oncol ) rlo = (rx0 - x0 + HALF) * dy;
		else
		{
			rlo = LSH( dy );
			++jx;
		}

		r = rhi - rlo;
		incX = LSH( dx );
		incY = LSH( dy );
		if LINE(y1) ++endy;						/* ends on scan line but dont mark it */
		do 
		{
			if( r <= 0) 
			{
				if( jx == endx ) break;
				++jx;
				r -= incY;
			}
			else
			{
				--jy;
				--py;
				if( jy < endy ) break;
				if( py < lowRowP ) return;
				if( py <= highRowP ) OFFY( jx );
				r -= incX;
			}
		} while (true );
	}		
}
#undef	DROUND
#undef	RSH
#undef	LSH
#undef	LINE
#undef	SET
#undef	OFFY
#undef	BETWEEN
#undef	INTERIOR
#undef  ZERO


/* new version 4/4/90 - winding number version assumes that the On transitions are
int the first half of the array, and the Off transitions are in the second half.  Also
assumes that the number of on transitions is in array[0] and the number of off transitions
is in array[n].
*/

/* New version 3/10/90 
Using the crossing information, look for segments that are crossed twice.  First
do Y lines, then do X lines.  For each found segment, look at the three lines in
the more positive adjoining segments.  If there are at least two crossings
of these lines, there is a dropout that needs to be fixed, so fix it.  If the bit on 
either side of the segment is on, quit; else turn the leastmost of the two pixels on.
*/ 

static void sc_orSomeBits( sc_BitMapData *bbox, int32 scanKind)
{
int16 ymin, ymax, xmin, xmax;
register int16 **yBase, **xBase;											/*<9>*/
register int16 scanline, coordOn, coordOff, nIntOn, nIntOff;				/*<9>*/
uint32 *bitmapP, *scanP;
int16  *rowPt, longsWide, *pOn, *pOff, *pOff2;
int16 index, incY, incX;
int32 upper, lower;

scanKind &= STUBCONTROL;
ymin = bbox->yMin;
ymax = bbox->yMax-1;
xmin = bbox->xMin;
xmax = bbox->xMax-1;
xBase = bbox->xBase;
yBase = bbox->yBase;
longsWide = bbox->wide >> 5;
if( longsWide == 1 ) bitmapP = bbox->bitMap + bbox->high - 1;
else bitmapP = bbox->bitMap + longsWide*(bbox->high-1);

/* First do Y scanlines
*/
scanP = bitmapP;
incY = bbox->nYchanges + 2;
incX = bbox->nXchanges + 2;
rowPt = *(yBase + ymin );
for (scanline = ymin; scanline <= ymax; ++scanline)
{
	nIntOn = *rowPt;
	nIntOff = *(rowPt + incY - 1);
	pOn = rowPt+1;
	pOff = rowPt + incY - 1 - nIntOff;
	while(nIntOn--)
	{
		coordOn = *pOn++;
		index = nIntOff;
		pOff2 = pOff;
		while (index-- && ((coordOff = *pOff2++) < coordOn));
		
		if( coordOn == coordOff )  /* This segment was crossed twice  */
		{
			if( scanKind )
			{
				upper = nUpperXings(yBase, xBase, scanline, coordOn,
					incY-2, incX-2,  xmin, xmax+1, ymax ); /*<14>*/ 
				lower = nLowerXings(yBase, xBase, scanline, coordOn,
					incY-2, incX-2,  xmin, xmax+1, ymin ); /*<14>*/
				if( upper < 2 || lower < 2 ) continue;
			}
			if(coordOn > xmax  ) invpixOn( xmax-xmin, longsWide, scanP );
			else if(coordOn == xmin  ) invpixOn( 0, longsWide, scanP );
			else invpixSegY( coordOn-xmin-1, longsWide, scanP );
		}
	}
	rowPt += incY;
	scanP -= longsWide;
}
/* Next do X scanlines */
rowPt = *(xBase + xmin);
for (scanline = xmin ; scanline <= xmax; ++scanline) 
{
	nIntOn = *rowPt;
	nIntOff = *(rowPt + incX - 1);
	pOn = rowPt+1;
	pOff = rowPt + incX - 1 - nIntOff;
	while(nIntOn--)
	{
		coordOn = *pOn++;
		index = nIntOff;
		pOff2 = pOff;
		while (index-- && ((coordOff = *pOff2++) < coordOn));
		if( coordOn == coordOff )
		{
			if( scanKind )
			{
				upper = nUpperXings(xBase, yBase, scanline, coordOn,
					incX-2, incY-2, ymin, ymax+1, xmax ); /*<14>*/
				lower = nLowerXings(xBase, yBase, scanline, coordOn,
					incX-2, incY-2,  ymin, ymax+1, xmin); /*<14>*/
				if( upper < 2 || lower < 2 ) continue;
			}
			if(coordOn > ymax  ) invpixOn( scanline-xmin, longsWide, bitmapP - longsWide*(ymax-ymin) );
			else if(coordOn == ymin ) invpixOn( scanline-xmin, longsWide, bitmapP );
			else invpixSegX( scanline-xmin, longsWide, bitmapP - longsWide*(coordOn-ymin-1) );
		}
	}
	rowPt += incX;
}
}

/* Pixel oring to fix dropouts   *** inverted bitmap version ***
See if the bit on either side of the Y line segment is on, if so return,
else turn on the leftmost bit.

Bitmap array is always K longs wide by H rows high.

Bit locations are numbered 0 to H-1 from top to bottom
and from 0 to 32*K-1 from left to right; bitmap pointer points to 0,0, and
all of the columns for one row are stored adjacently.
*/

static void invpixSegY(int16 llx, uint16 k, uint32* bitmapP )
{
register uint32 maskL, maskR;								/*<9>*/
	if (k == 1)
	{
		maskL = 0x40000000;
		maskL >>= llx;
		if( *bitmapP & maskL) return;
		maskL <<= 1;
		*bitmapP |= maskL;
	}
	else
	{
		register uint8  nBits = (uint8)(llx & 0x1f);		/*<9>*/
		maskL = 0x80000000;
		maskL >>= nBits; 
		bitmapP += (llx >>5);
		if( *bitmapP & maskL) return;
		if(nBits < 31)
		{
			maskR = maskL >> 1;
			if(*bitmapP & maskR) return;	
			*bitmapP |= maskL;	
		}
		else
		{
			maskR = 0x80000000;
			++bitmapP;
			if(*bitmapP & maskR) return;	
			--bitmapP;	
			*bitmapP |= maskL;	
		}
	}
}
/* Pixel oring to fix dropouts   *** inverted bitmap version ***
See if the bit on either side of the X line segment is on, if so return,
else turn on the bottommost bit.

Temporarily assume bitmap is set up as in Sampo Converter. 
Bitmap array is always K longs wide by H rows high.

For now, assume bit locations are numbered 0 to H-1 from top to bottom
and from 0 to 32*K-1 from left to right; and that bitmap pointer points to 0,0, and
all of the columns for one row are stored adjacently.
*/

static void invpixSegX(int16 llx, uint16 k, uint32* bitmapP )
{
register uint32 maskL = 0x80000000;						/*<9>*/
	bitmapP -= k;
	if (k == 1)	maskL >>= llx;
	else
	{
		maskL >>= (unsigned)(llx & 0x1f); 	/*<9>*/
		bitmapP += (llx >>5);
	}
	if( *bitmapP & maskL) return;
	bitmapP += k;
	*bitmapP |= maskL;
}

/* Pixel oring to fix dropouts    ***inverted bitmap version ***
This code is used to orin dropouts when we are on the boundary of the bitmap.
The bit at llx, lly is colored.

Temporarily assume bitmap is set up as in Sampo Converter. 
Bitmap array is always K longs wide by H rows high.

For now, assume bit locations are numbered 0 to H-1 from top to bottom
and from 0 to 32*K-1 from left to right; and that bitmap pointer points to 0,0, and
all of the columns for one row are stored adjacently.
*/
static void invpixOn(int16 llx, uint16 k, uint32* bitmapP )
{
register uint32 maskL = 0x80000000;						/*<9>*/
	if (k == 1)	maskL >>= llx;
	else
	{
		maskL >>= (unsigned)(llx & 0x1f);				/*<9>*/
		bitmapP += (llx >>5);
	}
	*bitmapP |= maskL;
}


/* Initialize a two dimensional array that will contain the coordinates of 
line segments that are intersected by scan lines for a simple glyph.  Return
a biased pointer to the array containing the row pointers, so that they can
be accessed without subtracting a minimum value.
	Always reserve room for at least 1 scanline and 2 crossings 
*/
static int16** sc_lineInit( int16* arrayBase, int16** rowBase, int16 nScanlines, int16 maxCrossings,
	int16 minScanline )
{
int16** bias;
register short count = nScanlines;
	if( count ) --count;
	bias = rowBase - minScanline;
	maxCrossings += 1;
	for(; count>=0; --count)
	{
	*rowBase++ = arrayBase;
	*arrayBase = 0;
	arrayBase += maxCrossings;
	*arrayBase++ = 0;
	}
return bias;
}

/* Check the kth scanline (indexed from base) and count the number of onTransition and
 * offTransition contour crossings at the line segment val.  Count only one of each
 * kind of transition, so maximum return value is two. 
 */ 				
static int32 nOnOff( int16** base, int32 k, int16 val, int32 nChanges )
{
register int16* rowP = *(base + k);
register int16* endP = (rowP + *rowP + 1);
register int32 count = 0;
register int16 v;

while(++rowP < endP)
{
	if( (v = *rowP) == val)
	{
		++count;
		break;
	}
	if( v > val ) break;
}
rowP = *(base + k) + nChanges + 1;
endP = (rowP - *rowP - 1);
while(--rowP > endP)
{
	if( (v = *rowP) == val) return ++count;
	if( v < val ) break;
}
return count;
}
/* 8/22/90 - added valMin and valMax checks */
/* See if the 3 line segments on the edge of the more positive quadrant are cut by at
 * least 2 contour lines.
 */

static int32 nUpperXings(int16** lineBase, int16** valBase, int16 line, int16 val,
						int16 lineChanges, int16 valChanges, int16 valMin, int16 valMax, int16 lineMax) /*<14>*/
{
register int32 count = 0;

if( line < lineMax ) count += nOnOff( lineBase, line+1, val, lineChanges ); 	/*<14>*/
if( count > 1 ) return count;
	else if( val > valMin ) count += nOnOff( valBase, val-1, line+1, valChanges );
if( count > 1 ) return count;
	else if( val < valMax ) count += nOnOff( valBase, val, line+1, valChanges );
return count;
}

/* See if the 3 line segments on the edge of the more negative quadrant are cut by at
 * least 2 contour lines.
 */

static int32 nLowerXings(int16** lineBase, int16** valBase, int16 line, int16 val,
						int16 lineChanges, int16 valChanges, int16 valMin, int16 valMax, int16 lineMin) /*<14>*/
{
register int32 count = 0;

if( line > lineMin ) count += nOnOff( lineBase, line-1, val, lineChanges );		/*<14>*/
if( count > 1 ) return count;
if( val > valMin ) count += nOnOff( valBase, val-1, line, valChanges );
if( count > 1 ) return count;
if( val < valMax ) count += nOnOff( valBase, val, line, valChanges );
return count;
}

/*
 * Finds the extrema of a character.
 *
 * PARAMETERS:
 *
 * bbox is the output of this function and it contains the bounding box.

*/
/* revised for new scan converter 4/90 rwb */
int sc_FindExtrema(sc_CharDataType *glyphPtr, sc_BitMapData *bbox) 
{
	register F26Dot6 *x, *y;									/*<9>*/
	register F26Dot6 tx, ty, prevx, prevy;
	F26Dot6  xmin, xmax, ymin, ymax;
	ArrayIndex	point, endPoint, startPoint;
	LoopCount 	ctr;
	uint16 	nYchanges, nXchanges, nx;
	int 	posY, posX, firstTime = true;
	
	nYchanges = nXchanges = 0;
	xmin = xmax = ymin = ymax = 0;

	for ( ctr = 0; ctr < glyphPtr->ctrs; ctr++ ) {
		endPoint = glyphPtr->ep[ctr];
		startPoint = glyphPtr->sp[ctr];
		x = &(glyphPtr->x[startPoint]); 						/*<9>*/
		y = &(glyphPtr->y[startPoint]); 						/*<9>*/
		if ( startPoint == endPoint ) continue; /* We need to do this for anchor points for composites */
		if( firstTime )
		{
			xmin = xmax = *x;									/* <9>*/
			ymin = ymax = *y;									/* <9>*/
			firstTime = false;
		}
		posY = (int)(*y >= (ty = *(y+endPoint-startPoint)));	/* <9>*/
		posX = (int)(*x >= (tx = *(x+endPoint-startPoint)));	/* <9>*/		
		for( point = startPoint; point <= endPoint; ++point )
		{
			prevx = tx;
			prevy = ty;
			tx = *x++;											/* <9>*/
			ty = *y++;											/* <9>*/
			if ( tx > prevx ) {
				if (!posX) {
					++nXchanges;
					posX = true;
				}
			}
			else if (tx < prevx ) {
				if (posX) {
					++nXchanges;
					posX = false;
				}
			}
			else if( ty == prevy )
			{													/*faster <9>*/
				LoopCount j = point-2-startPoint;
				register F26Dot6 *newx = x-3;  
				register F26Dot6 *oldx = newx++;  
				register F26Dot6 *newy = y-3;  
				register F26Dot6 *oldy = newy++;  
				register int8 *newC = &(glyphPtr->onC[point-2]);  
				register int8 *oldC = newC++; 
				*(newC+1) |= ONCURVE;
				for( ; j>=0; --j)
				{
					*newx-- = *oldx--;
					*newy-- = *oldy--;
					*newC-- = *oldC--;
				}
				++startPoint;
			}
	
			if ( ty > prevy ) {
				if (!posY) {
					++nYchanges;
					posY = true;
				}
			}
			else if (ty < prevy ) {
				if (posY) {
					++nYchanges;
					posY = false;
				}
			}
			if (tx > xmax ) xmax = tx;
			else if (tx < xmin ) xmin = tx;
			if (ty > ymax ) ymax = ty;
			else if (ty < ymin ) ymin = ty;
		}
		glyphPtr->sp[ctr] = startPoint < endPoint ? startPoint : endPoint;
		if( nXchanges & 1) ++nXchanges;
		if( nYchanges & 1) ++nYchanges; /* make even */
	}

	xmax += HALF;  xmax >>= PIXSHIFT;
	ymax += HALF;  ymax >>= PIXSHIFT;
	xmin += HALFM; xmin >>= PIXSHIFT; 
	ymin += HALFM; ymin >>= PIXSHIFT;
	
	if( xmin <= -32768 || ymin <= -32768 || xmax > 32767 || ymax > 32767 )  /*<10>*/
		return POINT_MIGRATION_ERR;

	bbox->xMax = (int16)xmax; /* quickdraw bitmap boundaries  */
	bbox->xMin = (int16)xmin;  
	bbox->yMax = (int16)ymax;
	bbox->yMin = (int16)ymin;

	bbox->high = (int16)ymax - (int16)ymin;
	nx = (int16)xmax - (int16)xmin;		/*  width is rounded up to be a long multiple*/
	bbox->wide = ( nx + 31) & ~31;		/* also add 1 when already an exact long multiple*/
	if( !( nx & 31) ) bbox->wide += 32;
	if(nXchanges == 0) nXchanges = 2;
	if(nYchanges == 0) nYchanges = 2;
	bbox->nXchanges = nXchanges;
	bbox->nYchanges = nYchanges;
	return false;
}


/*
 * This function break up a parabola defined by three points (A,B,C) and breaks it
 * up into straight line vectors given a maximium error. The maximum error is
 * 1/resolution * 1/ERRDIV. ERRDIV is defined in sc.h.
 *
 *           
 *         B *-_
 *          /   `-_
 *         /       `-_
 *        /           `-_ 
 *       /               `-_
 *      /                   `* C
 *   A *
 *
 * PARAMETERS:
 *
 * Ax, Ay contains the x and y coordinates for point A. 
 * Bx, By contains the x and y coordinates for point B. 
 * Cx, Cy contains the x and y coordinates for point C.
 * hX, hY are handles to the areas where the straight line vectors are going to be put.
 * count is pointer to a count of how much data has been put into *hX, and *hY.
 *
 * F(t) = (1-t)^2 * A + 2 * t * (1-t) * B + t * t * C, t = 0... 1 =>
 * F(t) = t * t * (A - 2B + C) + t * (2B - 2A) + A  =>
 * F(t) = alfa * t * t + beta * t + A
 * Now say that s goes from 0...N, => t = s/N
 * set: G(s) = N * N * F(s/N)
 * G(s) = s * s * (A - 2B + C) + s * N * 2 * (B - A) + N * N * A
 * => G(0) = N * N * A
 * => G(1) = (A - 2B + C) + N * 2 * (B - A) + G(0)
 * => G(2) = 4 * (A - 2B + C) + N * 4 * (B - A) + G(0) =
 *           3 * (A - 2B + C) + 2 * N * (B - A) + G(1)
 *
 * D(G(0)) = G(1) - G(0) = (A - 2B + C) + 2 * N * (B - A)
 * D(G(1)) = G(2) - G(1) = 3 * (A - 2B + C) + 2 * N * (B - A)
 * DD(G)   = D(G(1)) - D(G(0)) = 2 * (A - 2B + C)
 * Also, error = DD(G) / 8 .
 * Also, a subdivided DD = old DD/4.
 */
static int32  sc_DrawParabola(	int32 Ax,
								int32 Ay,
								int32 Bx,
								int32 By,
								int32 Cx,
								int32 Cy,
								int32 **hX,
								int32 **hY,
								int32 *count,
								int32 inGY )
{
	register int32 GX, GY, DX, DY, DDX, DDY, nsqs;
	register int32 *xp, *yp;
	register int32 tmp;
	int32 i;

    /* Start calculating the first and 2nd order differences */
    GX  = Bx; /* GX = Bx */
	DDX = (DX = (Ax - GX)) - GX + Cx; /* = alfa-x = half of ddx, DX = Ax - Bx */
	GY  = By; /* GY = By */
	DDY = (DY = (Ay - GY)) - GY + Cy; /* = alfa-y = half of ddx, DY = Ay - By */
	/* The calculation is not finished but these intermediate results are useful */

    if ( inGY < 0 ) {
		/* calculate amount of steps necessary = 1 << GY */
		/* calculate the error, GX and GY used a temporaries */
		GX  = DDX < 0 ? -DDX : DDX;
		GY  = DDY < 0 ? -DDY : DDY;
		/* approximate GX = sqrt( ddx * ddx + ddy * ddy ) = Euclididan distance, DDX = ddx/2 here */
		GX += GX > GY ? GX + GY : GY + GY; /* GX = 2*distance = error = GX/8 */
	
		/* error = GX/8, but since GY = 1 below, error = GX/8/4 = GX >> 5, => GX = error << 5 */
#ifdef ERRSHIFT
		for ( GY = 1; GX > (PIXELSIZE << (5 - ERRSHIFT)); GX >>= 2  )  {
#else
		for ( GY = 1; GX > (PIXELSIZE << 5 ) / ERRDIV; GX >>= 2  ) {
#endif
			GY++; /* GY used for temporary purposes */
		}
		/* Now GY contains the amount of subdivisions necessary, number of vectors == (1 << GY )*/
		if ( GY > MAXMAXGY ) GY = MAXMAXGY; /* Out of range => Set to maximum possible. */
		i = 1 << GY;
		if ( (*count = *count + i )  > MAXVECTORS ) {	
		   /* Overflow, not enough space => return */
		   return( 1 );
		}
	} else {
	    GY = inGY;
		i = 1 << GY;
	}

    if ( GY > MAXGY ) {
	    DDX = GY-1; /* DDX used as a temporary */
	    /* Subdivide, this is nummerically stable. */
#define MIDX GX
#define MIDY GY
		MIDX = (Ax + Bx + Bx + Cx + 2) >> 2;
		MIDY = (Ay + By + By + Cy + 2) >> 2;
		DX   = (Ax + Bx + 1) >> 1;
		DY   = (Ay + By + 1) >> 1;
		sc_DrawParabola( Ax, Ay, DX, DY, MIDX, MIDY, hX, hY, count, DDX );
		DX = (Cx + Bx + 1) >> 1;
		DY = (Cy + By + 1) >> 1;
		sc_DrawParabola( MIDX, MIDY, DX, DY, Cx, Cy, hX, hY, count, DDX );
	    return 0;
	}
	nsqs = GY + GY; /* GY = n shift, nsqs = n*n shift */

	/* Finish calculations of 1st and 2nd order differences */
	DX   = DDX - (DX << ++GY); /* alfa + beta * n */
	DDX += DDX;
	DY   = DDY - (DY <<   GY);
	DDY += DDY;

    xp = *hX;
	yp = *hY;

	tmp = 1L << (nsqs-1);								/*<17> factor out of loop below */
	GY = (Ay << nsqs) + tmp; /*  Ay * (n*n) */			/*<17> and add in here */
	GX = (Ax << nsqs) + tmp; /*  Ax * (n*n) */ /* GX and GY used for real now */ /*<16> and here*/
	
	/* OK, now we have the 1st and 2nd order differences,
	   so we go ahead and do the forward differencing loop. */
	do {
	    GX += DX;  /* Add first order difference to x coordinate */
		DX += DDX; /* Add 2nd order difference to first order difference. */
		GY += DY;  /* Do the same thing for y. */
		DY += DDY;
		*xp = GX >> nsqs;								/*<17> postpone increment */
		*yp = GY >> nsqs;								/*<17> postpone increment */
		if( *xp == *(xp-1) && *yp == *(yp-1) ) --*count;/*<16> Eliminate zero length vectors */
		else											/*<16>*/
		{												/*<16>*/
			++xp;										/*<16>*/
			++yp;										/*<16>*/
		}												/*<16>*/
	} while ( --i );
	*hX = xp; /* Done, update pointers, so that caller will know how much data we put in. */
	*hY = yp;
	return 0;
}
#undef MIDX
#undef MIDY
