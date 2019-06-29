/*
	File:		embeddedBitmap.c

	Contains:	Code to read embedded bitmaps in 'bloc' & 'bdat' tables in sfnts.

	Written by:	Richard Becker
	
	Copyright:	© 1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	10/23/91	RB		first checked in
*/

#if TheFuture

#include "setjmp.h"

#include "FSError.h"
#include "FSCdefs.h"
#include "FontMath.h"
#include "sfnt.h"
#include "sc.h"
#include "fnt.h"
#include "FontScaler.h"
#include "FSGlue.h"
#include "privateSFNT.h"
#include "embeddedBitmap.h"

// private prototypes
static uint8 bm_availFontBits(uint8 hPpem, uint8 vPpem, uint16 size, uint8* p, int16* jTable );
static uint8 bm_bestScalableGlyph( fsg_SplineKey* key );
static uint8 bm_calcMethod( uint8 method, uint8 bitmapPreferences, uint8 usefulOutlines );
static uint16 bm_bDatOffset1( uint16* p, uint8 shift, uint8 n, uint16 glyph, uint32* offsetP );
static uint16 bm_bDatOffset( fsg_SplineKey* key );
uint8* MAGICREED( fsg_SplineKey* key , sfnt_tableIndex n, uint32 offset, uint32 length );
static void stretchRow( uint8* sP, uint8* dP, int sWide, int dWide, Fixed r, int sOffset, int8 xSide );

/* rwb 7/19/91
 * key contains a target hPpem, vPpem, ps, and glyph index
 * key contains a pointer to a bloc table
 * Find the best bitmap table that actually has data for that glyph.
 * Fill out parameters specifying hPpem, vPpem, & ps of best match
 * Also fill out offset of glyph bitmap data
 * Fill out output record fields.
 */
void bm_bestMatchGlyph(fs_GlyphInputType *inputPtr, fsg_SplineKey* key, fs_GlyphInfoType *outputPtr )
{
uint8 success;
if( inputPtr->param.newglyph.bitmapMethodPreferences != key->bitmapPreferences )
{
	key->bitmapPreferences = inputPtr->param.newglyph.bitmapMethodPreferences;
	key->methodToBeUsed =  bm_calcMethod( key->availMatchFont, key->bitmapPreferences, key->usefulOutlines );
}
else key->methodToBeUsed = key->methodToBeUsedFont;
if( key->methodToBeUsed == 2 || key->methodToBeUsed == 3)
{
 	key->bestHorPpemGlyph = key->bestHorPpemFont;
 	key->bestVerPpemGlyph = key->bestVerPpemFont;
 	key->bestPointSizeGlyph = key->bestPointSizeFont;
	key->indexShiftGlyph = key->indexShiftFont;
	key->indexNbaseGlyph = key->indexNbaseFont;
	key->indexOffsetGlyph = key->indexOffsetFont;
	key->lengthBitData = bm_bDatOffset( key );
	if( key->lengthBitData == 0 )
	{
		switch( key->bitmapPreferences )
		{
			case 0: key->methodToBeUsed = key->usefulOutlines ? 0 : 1;
					break;
			case 1: key->methodToBeUsed = 1;
					break;
			case 2: key->methodToBeUsed = 1;
					break;
		// case 3 can't happen
			case 4: key->methodToBeUsed = 1;
					break;
			case 5: key->methodToBeUsed = 4;
					break;
		}
	}
}
if( key->methodToBeUsed == 1 )
{
	success = bm_bestScalableGlyph( key );
	if( success == 4 && key->bitmapPreferences == 4 ) key->methodToBeUsed = 4;
	else if( success == 4 && key->usefulOutlines == 0 ) key->methodToBeUsed = 4;
	else if( success == 4 ) key->methodToBeUsed = 0;
}
if( key->methodToBeUsed == 4 || key->methodToBeUsed == 0)
{
	key->bestHorPpemGlyph = 0;
	key->bestVerPpemGlyph = 0;
}
		
/* now fill out the output record */
outputPtr->bitmapMethod = key->methodToBeUsed;
outputPtr->bitmapHorizontalPPEM = key->bestHorPpemGlyph;
outputPtr->bitmapVerticalPPEM = key->bestVerPpemGlyph;
}

/* rwb 7/25/91
 * Attempt to find best scalable glyph data; (assumes method == 1)
 * Fill out bestHorPpemGlyph, bestVerPpemGlyph,	bestPointSizeGlyph,	indexShiftGlyph,
 * indexNbaseGlyph, offsetBitData in key.
 * Set method to 4 if unsuccessful.
 */
 static uint8 bm_bestScalableGlyph( fsg_SplineKey* key )
 {
 	uint8 avail;
	uint8* pt;
	int16 table;
	
 	key->bestHorPpemGlyph = key->bestHorPpemFont;
 	key->bestVerPpemGlyph = key->bestVerPpemFont + 1;
 	key->bestPointSizeGlyph = key->bestPointSizeFont;

	do
	{
		--key->bestVerPpemGlyph;
		avail = bm_availFontBits( key->bestHorPpemGlyph, key->bestVerPpemGlyph, key->desiredPointSize,
						key->blocPointer, &table );
		if( avail )
		{
			pt = key->blocPointer + 8 + (SIZESUBTABLESIZE * table); //address of size-subtable
			key->bestHorPpemGlyph = *pt;
			key->bestVerPpemGlyph = *(pt+1);
			key->bestPointSizeGlyph = *((uint16*)(pt+2));
			key->indexShiftGlyph = *(pt+14);
			key->indexNbaseGlyph = *(pt+15);
			key->indexOffsetGlyph = *((uint32*)(pt+16));
			key->lengthBitData = bm_bDatOffset( key );
		}
		else return 4;
	} while( key->lengthBitData == 0 );
	return 1;
}
		

/* rwb 7/19/91
 * Given a target hPpem, vPpem, & ps
 * Given pointer to a bloc table. (set when new_sfnt looked for bitmaps)
 * Find the best bitmap table match in this font.
 * Fill out parameters specifying hPpem, vPpem, & ps of best match
 * Also fill out offset of index subtable for the match.
 * Fillout indicator of match quality = 
 * 	0 - no bitmap match available
 *	1 - scalable match available
 *	2 - exact match available
 *	3 - exact match available except for pointsize
 * Fill out output record fields.
 */
void bm_bestMatchFont( fsg_SplineKey* key, fs_GlyphInfoType *outputPtr )
{
	uint8* pt;
	int16 table;
	uint8 method = bm_availFontBits( key->desiredHorPpem, key->desiredVerPpem, key->desiredPointSize,
					key->blocPointer, &table );
	key->indexPtrGlyph = 0;
	if( method )
	{
		key->availMatchFont = method;
		pt = key->blocPointer;
		key->numberGlyphs = *((uint16*)(pt+4));
		pt += ( 8 + (SIZESUBTABLESIZE * table)); //address of size-subtable
		key->bestHorPpemFont = *pt;
		key->bestVerPpemFont = *(pt+1);
		key->bestPointSizeFont = *((uint16*)(pt+2));
		key->horAscent = *(pt+4);
		key->horDescent = *(pt+5);
		key->horLineGap = *(pt+6);
		key->horAdvanceMax = *(pt+7);
		key->verAscent = *(pt+8);
		key->verDescent = *(pt+9);
		key->verLineGap = *(pt+10);
		key->verAdvanceMax = *(pt+11);
		key->indexShiftFont = *(pt+14);
		key->indexNbaseFont = *(pt+15);
		key->indexOffsetFont = *((uint32*)(pt+16));
		key->indexFormat = *(pt+20);
		key->dataFormat = *(pt+21);
		key->methodToBeUsedFont =  bm_calcMethod( method, key->bitmapPreferences, key->usefulOutlines );
	}
	else
	{
		key->availMatchFont = 0;
		key->methodToBeUsedFont = key->usefulOutlines ? 0 : 4;
		key->bestHorPpemFont = 0;
		key->bestVerPpemFont = 0;
	}
	/* now fill out the output record */
	outputPtr->bitmapMethod = key->methodToBeUsedFont;
	outputPtr->bitmapHorizontalPPEM = key->bestHorPpemFont;
	outputPtr->bitmapVerticalPPEM = key->bestVerPpemFont;
}
 
/* rwb 7/1/91
 * Given a target horiz ppem, vert ppem & point-size.
 * Given a pointer to a bloc table.
 * Return an indicator = 
 *	0 - no match available.
 *	1 - exact match available.
 *	2 - exact match except for point-size available.
 *	3 - scalable resolution available.
 * Use x res + y res as decision criteria, providing that both x & Y are smaller than target.
 * If bitmap is available, set *jTable to which table provides the best match.
 */
static uint8 bm_availFontBits(uint8 hPpem, uint8 vPpem, uint16 size, uint8* p, int16* jTable )
{
int distance = 1025;
uint16 n = *((uint16*)(p+6)); // number of size-subtables
int dSize = 0;
int table = 0;
uint16 s;
int j;
p += 8; //start of subtables
for(j=0; j<n; ++j)
{
	int d = (*p > hPpem) ? 512 : hPpem - *p;
	d += (*(p+1) > vPpem) ? 512 : vPpem - *(p+1);
	if( d < distance )
	{ 
		distance = d;
		table = j;
		s = *((uint16*)(p+2));
		dSize = ( (s == size) || (s == 0)) ? 0 : 1;
	}
	p += SIZESUBTABLESIZE; //size of a size-subtable
}
if(distance > 511)
{
	*jTable = 0;
	return 0;
}
*jTable = table;
if(distance == 0 && dSize == 0) return 2;
if(distance == 0) return 3;
return 1;
}

/* rwb 7/23/91
 * Given bitmap availability, bitmapPreferences, and outline availability
 * Calculate closest match to preferences
 */
static uint8 bm_calcMethod( uint8 method, uint8 bitmapPreferences, uint8 usefulOutlines )
{
	if( method == 0 )
	{
		if( (bitmapPreferences <= 3 && !usefulOutlines)
		|| (bitmapPreferences >= 4 )) method = 4;
	}
	else switch( bitmapPreferences )
	{
	case 0:	if( usefulOutlines && method == 1 ) method = 0;
			break;
	case 2: if( usefulOutlines ) method = 0;
			break;
	case 3:	method =  usefulOutlines ? 0 : 4;
			break;
	case 5:	if( method == 1) method = 4;
			break;
// cases 1 & 4 leave method unchanged
	}
	return method;
}
/* rwb 7/26/91
 * Given a bitmap Method and an Offset into the bdat table.
 * Read the bdat table, cache the pointer.
 * Fill out the sc_BitMapData structure with metrics (may be scaled) info.
 * ONLY Formats 2 & 4
 */
void bm_fillOutBMdata( sc_BitMapData* dest, metricsType* metric, fsg_SplineKey* key )
{
#define SCALE( num, den) ((ShortMulDiv( temp<<16, num, den ) + 0x8000) >> 16)

uint8 pixelWidth, pixelHeight,  destWidth;
uint8 format = key->dataFormat;
uint8* source = MAGICREED( key, sfnt_bitmapData, key->offsetBitData, key->lengthBitData);
uint8* begin = source;
int	scaleB = ( key->methodToBeUsed == 1 );
int16 xNum = key->desiredHorPpem;
int16 xDen = key->bestHorPpemGlyph;
int16 yNum = key->desiredVerPpem;
int16 yDen = key->bestVerPpemGlyph;
Fixed temp;

temp = key->pixHeightSource = *source++;
pixelHeight = scaleB ? (uint8)SCALE( yNum, yDen ) : temp;
temp = key->pixWidthSource = *source++;
pixelWidth =  scaleB ? (uint8)SCALE( xNum, xDen ) : temp;
if( format == 1 || format == 4 || format == 6 ) // read vertical bearings & advance
{
	if( format == 1 ) source += 4;						// skip horizontal
	else if( format == 6 ) source += 3;
	temp = key->vBearingXsource = *source++;
	dest->xMin = scaleB ? (uint8)SCALE( xNum, xDen ) : temp;
	metric->devLeftSideBearing.x = metric->leftSideBearing.x = metric->leftSideBearingLine.x \
		= metric->devLeftSideBearingLine.x = dest->xMin << 16;
	temp = key->vBearingYsource = *source++;
	dest->yMax = scaleB ? (uint8)SCALE( yNum, yDen ) : temp;
	metric->devLeftSideBearing.y = metric->leftSideBearing.y = metric->leftSideBearingLine.y \
		= metric->devLeftSideBearingLine.y = dest->yMax << 16;
	if( format == 1 )
	{
		temp = *((int16*)source);
		source += 2;
	}
	else temp = *source++;
	key->vAdvanceSource = temp;
	metric->advanceWidth.x = metric->devAdvanceWidth.x = scaleB ? (uint8)SCALE( xNum, xDen ) : *source << 16;
	metric->advanceWidth.y = metric->devAdvanceWidth.y = 0;	
}
if( format == 1 || format == 2 || format == 6 ) // now overwrite with horizontal stuff
{
	if( format == 1 ) source -= 8; 				// go back to horizontal
	else if( format == 6 ) source -= 6;
	temp = key->hBearingXsource =*source++;
	dest->xMin = scaleB ? (uint8)SCALE( xNum, xDen ) : temp;
	metric->devLeftSideBearing.x = metric->leftSideBearing.x = metric->leftSideBearingLine.x \
		= metric->devLeftSideBearingLine.x = dest->xMin << 16;
	temp = key->hBearingYsource = *source++;
	dest->yMax = scaleB ? (uint8)SCALE( yNum, yDen ) : temp;
	metric->devLeftSideBearing.y = metric->leftSideBearing.y = metric->leftSideBearingLine.y \
		= metric->devLeftSideBearingLine.y = dest->yMax << 16;
	if( format == 1 )
	{
		temp = *((int16*)source);
		source += 2;
	}
	else temp = *source++;
	key->hAdvanceSource = temp;
	metric->advanceWidth.x = metric->devAdvanceWidth.x = scaleB ? (uint8)SCALE( xNum, xDen ) : *source << 16;
	metric->advanceWidth.y = metric->devAdvanceWidth.y = 0;
	if( format == 1 ) source += 4;				// skip verticals
	else if( format == 6 ) source += 3;
}
if( format == 1 )
{
	key->numAttachments = *source++;
	key->numComponents = *source++;
	source += 2;										// skip attachments offset
	key->bitmapPtr = begin + *((uint16*)source);		// might be pointer to components
}
else
{
	key->bitmapPtr = source;
	key->numAttachments = 0;
	key->numComponents = 0;
}
dest->high	= pixelHeight;
dest->yMin	= dest->yMax - pixelHeight;

destWidth	= (pixelWidth + 31) & ~31;		//round up to a long
if( !(pixelWidth & 31)) destWidth += 32; 	//to be backward compatible with outlines
dest->wide = destWidth;
dest->xMax	= dest->xMin + pixelWidth;
#undef SCALE
}

/* rwb 7/29/91
 * Given the client preferences in the input record
 * Given the current device specifications
 * Fill out key->bitmapPreferences, scalingPreference, desiredHorPpem, desiredVerPpem,
 * 	desiredPointSize,
 * Return false for bitmaps not requested, else true.
 */
boolean bm_requestForBitmaps( fsg_SplineKey* key, fs_GlyphInputType *inputPtr )
{
register Fixed xppem, yppem;
if( inputPtr->param.newtrans.bitmapMethodPreferences == 3 ) return false;
if( key->currentTMatrix.transform[0][1] != 0 || 
	key->currentTMatrix.transform[1][0] != 0) return false; // don't do rotated bitmaps
	
key->bitmapPreferences = inputPtr->param.newtrans.bitmapMethodPreferences;
key->scalingPreference = inputPtr->param.newtrans.scalingPreference;
xppem = ShortMulDiv( key->fixedPointSize, inputPtr->param.newtrans.xResolution, POINTSPERINCH );
yppem = ShortMulDiv( key->fixedPointSize, inputPtr->param.newtrans.yResolution, POINTSPERINCH );
xppem = FixMul( xppem, key->currentTMatrix.transform[0][0] );
yppem = FixMul( yppem, key->currentTMatrix.transform[1][1] );
key->desiredPointSize = (key->fixedPointSize + 0x8000) >> 16;
key->desiredHorPpem = (xppem + 0x8000) >> 16;
key->desiredVerPpem = (yppem + 0x8000) >> 16;
}

/* rwb 7/18/91
 * Given a spline-key containing a copy of the size-subtable for the best match
 * for the current transformation and a desired glyph.
 * If necessary, read in the index-subtable, look up the glyph.
 * Set offset to the required glyph-bitmap in key.
 * Return the length of the glyph-bitmap data.
 */
static uint16 bm_bDatOffset( fsg_SplineKey* key )
{
	int16 	length, j;
	uint8 	indexF 	= key->indexFormat;
	uint16*	indexP 	= key->indexPtrGlyph;
	uint8 	nbase 	= key->indexNbaseGlyph;
	uint16 	glyph	= key->glyphIndex;
	int32 	offset1, offset2;
	
	
	
	if( indexF == 1 ) length = 4 * key->numberGlyphs + 4;
		else if( indexF == 2 ) length = 2 * key->numberGlyphs + 2 + 4*key->indexNbaseGlyph;
			else return 0;
 	if( indexP == 0 ) 
		key->indexPtrGlyph = indexP =
		(uint16*)MAGICREED( key, sfnt_bitmapLocation, key->indexOffsetGlyph, length);
	if( indexF == 1)
	{
		offset1 = *((uint32*)indexP + glyph );
		offset2 = *((uint32*)indexP + glyph + 1);
		length = offset2 - offset1;
		if( length == 0 ) return 0;
	}
	else
	{
		j = glyph + nbase + nbase;			// skip base addresses
		offset1 = *(indexP + j);
		offset2 = *(indexP + j + 1);
		length = offset2 - offset1;
		if( length == 0 ) return 0;
		j = glyph >> key->indexShiftGlyph;	// calculate base address to be added
		j  <<= 1;							// base addresses are longs
		offset1 += (*(indexP+j) << 16);
		offset1 +=  *(indexP+j+1);
		if( length < 0 )					// 16-bit offsets are reversed, must have crossed base address
		{
			j += 2;
			offset2 += (*(indexP+j) << 16);
			offset2 +=  *(indexP+j+1);
			length = offset2 - offset1;
		}
	}
	key->offsetBitData = offset1;
	return length;
}

/* rwb 7/17/91 
 * source is byte aligned, destination is long aligned. sourceWide will generally
 * be less than 4 bytes, so moving longs or words is not justified.
 * Accomodate banding, to be consistent with outline rendering.
 */
void bm_copyExactBitMap( uint8* source, uint8 sourceWide, int16 lowBand, int16 highBand,
	uint8* bitMap, uint8 destWide, uint8 destHigh ) 
{
uint8* destBegin = bitMap;
uint8 *destNext, *sourceNext;
int16 nRows = highBand - lowBand + 1;
while( destHigh-- > highBand )
{
	destBegin += destWide;
	source += sourceWide;
}
destNext = destBegin + destWide;
sourceNext = source + sourceWide;
while( nRows-- )
{
	while( source < sourceNext ) *destBegin++ = *source++;
	while( destBegin < destNext ) *destBegin++ = 0;
	sourceNext += sourceWide;
	destNext += destWide;
}
}

uint8* MAGICREED( fsg_SplineKey* key , sfnt_tableIndex n, uint32 offset, uint32 length )
{
#pragma unused( length )
	uint8* p = sfnt_GetTablePtr( key, n, false );
	p += offset;
	return p;
}

void bm_clear( uint8* p, int32 n )
{
while( n-- ) *p++ = 0;
}

/* rwb 8/14/91
 * Or a component glyph into the bitmap cache for a compound glyph.
 * Read the data from the bdat table.
 * Offset the component (including surrounding whitespace) by xGOff & yOff.
 * If component is itself compound, call this routine recursively.
 * Components only exist in data format 1. 
 * Component offsets are based on horizontal bearings (even if layout is vertical).
 * yGoff combines component offsets and vert component of left sidebearing of dest glyph.
 */
void bm_orComponent(fsg_SplineKey* key, uint32 offSet, uint16 length,
	int16 lowBand, int16 highBand,
	uint8* bitMap, uint8 destWide, uint8 destHigh, int8 xGOff, int8 yGOff, int8 ySide )
{
uint8* 	cP 		= MAGICREED( key, sfnt_bitmapData, offSet, length);
uint8*	subP 	= cP + *((int16*)(cP+14));
int		nComp	= *(cP + 11);

if( nComp == 0 )
{
	int8 xBOff = xGOff + *(cP+2);
	int8 yBOff = yGOff + *(cP+3);
	bm_orSimpleBitMap( subP, *(cP+1), lowBand, highBand,
									bitMap, destWide, destHigh, xBOff, yBOff, *cP, ySide );
}
else
{
	while( nComp-- != 0 )
	{
		uint32 oj = *((uint32*)subP);
		uint16 lj = *((uint16*)subP+3);
		int8 ojx = xGOff + *(subP+4);
		int8 ojy = yGOff + *(subP+5);
 		bm_orComponent(key, oj, lj, lowBand, highBand,
						bitMap, destWide, destHigh, ojx, ojy, ySide );	
		subP += 8;
	}
}
}

/* rwb 7/17/91
 * OR a simple bitmap into destination cache.
 * Assumes glyph format 1.
 * Offset the source bitmap by xOff and yOff.
 * source is byte aligned, destination is long aligned. sourceWide will generally
 * be less than 4 bytes, so moving longs or words is not justified.
 * Accomodate banding, to be consistent with outline rendering.
 * yOff combines vertical component of left SideBearing for source glyph and vertical
 *   component of left SideBearing for destination glyph with any component offsets. 
 */
void bm_orSimpleBitMap( uint8* source, uint8 sourceWide, int16 lowBand, int16 highBand,
	uint8* bitMap, uint8 destWide, uint8 destHigh, int8 xOff, int8 yOff, uint8 sourceHigh, int8 ySide ) 
{
uint8* destBegin = bitMap;
uint8 *destNext = destBegin + destWide;
uint8* sourceNext = source + sourceWide;
int16 dRow = ySide; 
int16 sRow = ySide + sourceHigh - yOff;
while( dRow > ySide - destHigh )
{
	if( dRow <= highBand && dRow >= lowBand )
	{
		if( sRow <= sourceHigh && sRow >= 1)
		{
			int temp = *source;
			int tt;
			while( source++ < sourceNext )
			{
				temp <<= 8;
				if(source < sourceNext ) temp |= *source;
				tt = (xOff >= 0) ? temp >> xOff : temp << xOff;
				*destBegin++ |= (tt >> 8);
			}		
			sourceNext += sourceWide;
		}
		destBegin = destNext;
		destNext += destWide;
	}
	--dRow;
	--sRow;
}
}

/* rwb 8/19/91
 * Stretch Or a component glyph into the bitmap cache for a compound glyph.
 * Read the data from the bdat table, and stretch it by the ratio 1/r.
 * Offset the component (including surrounding whitespace) by xGOff & yOff.
 * If component is itself compound, call this routine recursively.
 * Components only exist in data format 1. 
 * Component offsets are based on horizontal bearings (even if layout is vertical).
 */
void bm_orStretchComponent(fsg_SplineKey* key, uint32 offSet, uint16 length,
	int16 lowBand, int16 highBand,
	uint8* bitMap, uint8 destWidePix, uint8 destHigh, int8 xGOff, int8 yGOff,
	Fixed r, Fixed q, int8 ySide, int8 xSide )
{
uint8* 	cP 		= MAGICREED( key, sfnt_bitmapData, offSet, length);
uint8*	subP 	= cP + *((int16*)(cP+14));
int		nComp	= *(cP + 11);

if( nComp == 0 )
{
	int8 xBOff = xGOff + *(cP+2);
	int8 yBOff = yGOff + *(cP+3);
	bm_orStretchSimpleBitMap( subP, *(cP+1), lowBand, highBand,
				bitMap, destWidePix, destHigh, xBOff, yBOff, *cP, r, q, ySide, xSide );
}
else
{
	while( nComp-- != 0 )
	{
		uint32 oj = *((uint32*)subP);
		uint16 lj = *((uint16*)subP+3);
		int8 ojx = xGOff + *(subP+4);
		int8 ojy = yGOff + *(subP+5);
 		bm_orStretchComponent(key, oj, lj, lowBand, highBand,
						bitMap, destWidePix, destHigh, ojx, ojy, r, q, ySide, xSide);	
		subP += 8;
	}
}
}

/* rwb 9/14/91
 *  STRETCH OR a simple bitmap into destination cache.
 * Assumes glyph format 1.
 * Offset the source bitmap by xOff and yOff.
 * source is byte aligned, destination is long aligned. sourceWide will generally
 * be less than 4 bytes, so moving longs or words is not justified.
 * Accomodate banding, to be consistent with outline rendering.
 * 1/r is horizontal stretch factor
 * 1/q is vertical stretch factor
 * ySide is vertical component of left SideBearing for destination glyph
 * yOff combines vertical component of left SideBearing for source glyph with any component offsets. 
 */
void bm_orStretchSimpleBitMap( uint8* source, uint8 sourceWidePix,
		int16 lowBand, int16 highBand, uint8* bitMap, uint8 destWidePix, uint8 destHigh,
		int8 xOff, int8 yOff, uint8 sourceHigh, Fixed r, Fixed q, int8 ySide, int8 xSide ) 

{
uint8* destBegin = bitMap;
int16 sourceWideBytes = sourceWidePix > 0 ? ((sourceWidePix-1)>>3)+1 : 0;
int16 destWideBytes = destWidePix > 0 ? ((destWidePix-1)>>3)+1 : 0;
int16 dRow = ySide;
Fixed dr = ySide << 16;
Fixed sr = (FixMul( dr, q ) + 0x8000 ) >> 16;
int16 sRow = sr - yOff + sourceHigh;
int16 first = 1;
int16 sOld;
while( dRow > ySide-destHigh )
{
	if( dRow <= highBand && dRow >= lowBand )
	{
		dr = dRow << 16;
		sr = (FixMul( dr, q ) + 0x8000 ) >> 16;
		sRow = sr -yOff + sourceHigh;
		if( sRow <= sourceHigh && sRow >= 1)
		{
			if( first )
			{
				first = 0;
				sOld = sRow;
			}
			else if( sRow != sOld )
			{
				sOld = sRow;
				source += sourceWideBytes;
			}
			stretchRow( source, destBegin, sourceWidePix, destWidePix, r, xOff, xSide );
		}
		destBegin += destWideBytes;
	}
	--dRow;
}
}

/* rwb 8-9-91
 * Stretch one row of a bitmap by a ratio (1/r) = (n/m)
 *	m is the ppem of the source bitmap. 
 * 	n is the ppem of the destination bitmap ( n >= m ).
 * The source row consists of sWide pixels beginning at sP
 *	and sOffset pixels of left-sidebearing
 * The destination row consists of dWide pixels beginning at dP
 *	and dOffset pixels of left-siedbearing. (Calculate dOffset).
 * r is a Fixed (16.16) point number = (m/n);
 * assume destination has been previously filled with zeros
 */
 
void stretchRow( uint8* sP, uint8* dP, int sWide, int dWide, Fixed r, int sOffset, int8 xSide)
{
uint8 mask[8] = {128,64,32,16,8,4,2,1};
int sMask, dMask, k;
Fixed valS;
int32 oldS, newS, offS;

offS = sOffset << 16;
sMask = dMask = 0;

/* first do left-sidebearing */
valS = r>>1;
for(k=0; k<xSide; ++k) valS += r;
oldS = valS & 0x7FFF0000;

/* next bump dest bitmap until component offset + source-left-sidebearing offset are covered
 */
while( oldS < offS ) 
{
	valS += r;
	oldS = valS & 0x7FFF0000;
	if( ++dMask == 8 )
	{
		dMask = 0;
		++dP;
	}
}
offS += (sWide << 16);	
for( k=0; k< dWide; ++k)
{
	newS = valS & 0x7FFF0000;
	if( newS > oldS )
	{
		if( newS >= offS ) break;
		oldS = newS;
		if( ++sMask == 8 )
		{
			++sP;
			sMask = 0;
		}
	}
	if( *sP & *(mask + sMask) ) *dP |= *(mask + dMask);
	if( ++dMask == 8 )
	{
		dMask = 0;
		++dP;
	}
	valS += r;
}
}

#endif