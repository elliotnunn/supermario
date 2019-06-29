/*
	File:		embeddedBitmap.h

	Contains:	Header info to read embedded bitmaps in 'bloc' & 'bdat' tables in sfnts.

	Written by:	Richard Becker
	
	Copyright:	© 1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	10/23/91	RB		first checked in
*/

void bm_bestMatchGlyph(fs_GlyphInputType *inputPtr, fsg_SplineKey* key, fs_GlyphInfoType *outputPtr );
void bm_bestMatchFont( fsg_SplineKey* key, fs_GlyphInfoType *outputPtr );
boolean bm_requestForBitmaps( fsg_SplineKey* key, fs_GlyphInputType *inputPtr );
void bm_copyExactBitMap( uint8* source, uint8 sourceWide, int16 lowBand, int16 highBand, uint8* bitMap, uint8 destWide, uint8 destHigh );
void bm_fillOutBMdata( sc_BitMapData* dest, metricsType* metric, fsg_SplineKey* key );
void bm_clear( uint8* p, int32 n );
void bm_orComponent(fsg_SplineKey* key, uint32 offSet, uint16 length,
	int16 lowBand, int16 highBand,
	uint8* bitMap, uint8 destWide, uint8 destHigh, int8 xGOff, int8 yGOff, int8 ySide );
void bm_orSimpleBitMap( uint8* source, uint8 sourceWide, int16 lowBand, int16 highBand,
	uint8* bitMap, uint8 destWide, uint8 destHigh, int8 xOff, int8 yOff, uint8 sourceHigh, int8 ySide ); 
void bm_orStretchComponent(fsg_SplineKey* key, uint32 offSet, uint16 length,
	int16 lowBand, int16 highBand,
	uint8* bitMap, uint8 destWide, uint8 destHigh, int8 xGOff, int8 yGOff, Fixed r, Fixed q, int8 ySide, int8 xSide );
void bm_orStretchSimpleBitMap( uint8* source, uint8 sourceWide,
		int16 lowBand, int16 highBand, uint8* bitMap, uint8 destWide, uint8 destHigh,
		int8 xOff, int8 yOff, uint8 sourceHigh, Fixed r, Fixed q, int8 ySide, int8 xSide); 

#define EBITMAP
#define SIZESUBTABLESIZE 24
