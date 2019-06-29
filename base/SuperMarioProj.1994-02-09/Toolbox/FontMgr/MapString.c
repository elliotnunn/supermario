/*
	File:		MapString.c

	Contains:	Character to glyph mapping functions

	Written by:	Mike Reed

	Copyright:	© 1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 9/25/91	jlf		Rolled in fix to MapString2.
		 <2>	11/16/90	MR		Fix range field of cmap-4 (thanks Eric) [rb]
		 <3>	 9/27/90	MR		Change selector in ComputeIndex4 to be log2(range) instead of
									2*log2(range), and fixed up comments
		 <2>	 8/10/90	MR		Add textLength field to MapString2
		 <1>	 7/23/90	MR		first checked in
				 7/23/90	MR		xxx put comment here xxx

	To Do:
*/


#include "FSCDefs.h"
#include "MapString.h"

/*
 *	These next 4 MapString functions map the charCodes into glyphs,
 *	returning the number of glyphs processed.
 */
long MapString0(uint8* map, register uint8* charCodes, register int16* glyphs, long glyphCount)
{
	register int16 count = glyphCount;

	for (--count; count >= 0; --count)
		*glyphs++ = map[*charCodes++];

	return glyphCount;
}

/*
 * High byte mapping through table
 *
 * Useful for the national standards for Japanese, Chinese, and Korean characters.
 *
 * Dedicated in spirit and logic to Mark Davis and the International group.
 *
 *	Algorithm: (I think)
 *		First byte indexes into KeyOffset table.  If the offset is 0, keep going, else use second byte.
 *		That offset is from beginning of data into subHeader, which has 4 words per entry.
 *			entry, extent, delta, range
 *
 *	textLength is optional.  If it is nil, it is ignored.
 */
long MapString2(register uint16* map, uint8* charCodes, int16* glyphs, long glyphCount, long* textLength)
{
	register short count = glyphCount;
	register uint8* codeP = charCodes;
	register uint16 mapMe;
	int16* origGlyphs = glyphs;

	for (--count; count >= 0; --count)
	{
		uint16 *temporaryMap = map;
		uint16 highByte = *codeP++;
		
		if ( temporaryMap[highByte] != 0 )
			mapMe = *codeP++;
		else
			mapMe = highByte;

		if (textLength && *textLength < codeP - charCodes)
		{
			--codeP;
			if ( temporaryMap[highByte] )
				--codeP;
			break;
		}

		temporaryMap = ((uint16*)((int8*)temporaryMap + temporaryMap[highByte])) + 256;	/* <4> bad mapping */
		mapMe -= *temporaryMap++; 		/* Subtract first code. */

		if ( mapMe < *temporaryMap++ ) {	/* See if within range. */
			uint16 idDelta;

			idDelta = *temporaryMap++;
			mapMe += mapMe; /* turn into word offset */

			temporaryMap = (uint16*)((int32)temporaryMap + *temporaryMap + mapMe );

			if ( *temporaryMap )
				*glyphs++ = *temporaryMap + idDelta;
			else
				*glyphs++ = 0;		/* missing */
		} else
			*glyphs++ = 0;			/* missing */					/* <4> bad mapping */
	}
	if (textLength)
		*textLength = codeP - charCodes;		/* report # bytes eaten */

	return glyphs - origGlyphs;				/* return # glyphs processed */
}

#define maxLinearX2 16
#define BinaryIteration \
		newP = (uint16 *) ((int8 *)tableP + (range >>= 1)); \
		if ( charCode > *newP ) tableP = newP;

/*
 * Segment mapping to delta values, Yack.. !
 *
 * In memory of Peter Edberg. Initial code taken from code example supplied by Peter.
 */
static uint16 ComputeIndex4(uint16* tableP, uint16 charCode)
{
	register uint16 idDelta;
	register uint16 offset, segCountX2, index;

	index = 0; /* assume missing initially */
	segCountX2 = *tableP++;

	if ( segCountX2 < maxLinearX2 ) {
		tableP += 3; /* skip binary search parameters */
	} else {
		/* start with unrolled binary search */
		register uint16 *newP;
		register int16  range; 		/* size of current search range */
		register uint16 selector; 	/* where to jump into unrolled binary search */
		register uint16 shift; 		/* for shifting of range */

		range 		= *tableP++; 	/* == 2*(2**floor(log2(segCount))) == 2*largest power of two <= segCount */
		selector 	= *tableP++;	/* == log2(range/2) */
		shift 		= *tableP++; 	/* == 2*segCount-range */
		/* tableP points at endCount[] */

		if ( charCode >= *((uint16 *)((int8 *)tableP + range)))
			tableP = (uint16 *) ((int8 *)tableP + shift); /* range to low shift it up */
		switch ( selector )
		{
		case 15: BinaryIteration;
		case 14: BinaryIteration;
		case 13: BinaryIteration;
		case 12: BinaryIteration;
		case 11: BinaryIteration;
		case 10: BinaryIteration;
		case  9: BinaryIteration;
		case  8: BinaryIteration;
		case  7: BinaryIteration;
		case  6: BinaryIteration;
		case  5: BinaryIteration;
		case  4: BinaryIteration;
		case  3:
		case  2:  /* drop through */
		case  1:
		case  0:
			break;
		}
	}
	/* Now do linear search */
	while ( charCode > *tableP++) /* goes one to far, so we can skip the reservedPad */
		;

	/* End of search, now do mapping */

	tableP = (uint16 *) ((int8 *)tableP + segCountX2); /* point at startCount[] */
	if ( charCode >= *tableP ) {
		offset = charCode - *tableP;
		tableP = (uint16 *) ((int8 *)tableP + segCountX2); /* point to idDelta[] */
		idDelta = *tableP;
		tableP = (uint16 *) ((int8 *)tableP + segCountX2); /* point to idRangeOffset[] */
		if ( *tableP == 0 ) {
			index	= charCode + idDelta;
		} else {
			offset += offset; /* make word offset */
			tableP 	= (uint16 *) ((int8 *)tableP + *tableP + offset ); /* point to glyphIndexArray[] */
			if (index = *tableP)
				index += idDelta;
		}
	}
	return index;
}

long MapString4_8(uint16* map, uint8* charCodes, int16* glyphs, long glyphCount)
{
	register short count = glyphCount;

	for (--count; count >= 0; --count)
		*glyphs++ = ComputeIndex4( map, *charCodes++ );

	return glyphCount;
}

long MapString4_16(uint16* map, uint16* charCodes, int16* glyphs, long glyphCount)
{
	register short count = glyphCount;

	for (--count; count >= 0; --count)
		*glyphs++ = ComputeIndex4( map, *charCodes++ );

	return glyphCount << 1;
}

/*
 * Trimmed Table Mapping - 8 bit character codes
 */
long MapString6_8(register uint16* map, uint8* charCodes, register int16* glyphs, long glyphCount)
{
	register short count = glyphCount - 1;
	uint16 firstCode = *map++;
	uint16 entryCount = *map++;
	int16* origGlyphs = glyphs;

	for (; count >= 0; --count)
	{
		uint16 charCode = *charCodes++ - firstCode;
		if ( charCode < entryCount )
			*glyphs++ = map[ charCode ];
		else
			*glyphs++ = 0; /* missing char */
	}
	return glyphs - origGlyphs;
}

/*
 * Trimmed Table Mapping - 16 bit character codes
 */
long MapString6_16(register uint16* map, uint16* charCodes, register int16* glyphs, long glyphCount)
{
	register short count = glyphCount - 1;
	uint16 firstCode = *map++;
	uint16 entryCount = *map++;
	int16* origGlyphs = glyphs;

	for (; count >= 0; --count)
	{
		uint16 charCode = *charCodes++ - firstCode;
		if ( charCode < entryCount )
			*glyphs++ = map[ charCode ];
		else
			*glyphs++ = 0; /* missing char */
	}
	return glyphs - origGlyphs;
}

