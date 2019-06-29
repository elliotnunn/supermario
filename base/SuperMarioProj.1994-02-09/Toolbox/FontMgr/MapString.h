/*
	File:		MapString.h

	Contains:	Char2glyph calls

	Written by:	Mike Reed

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/10/90	MR		Add textLength to MapString2
		 <1>	 7/23/90	MR		first checked in
				 7/23/90	MR		xxx put comment here xxx

	To Do:
*/

long MapString0(uint8* map, uint8* charCodes, int16* glyphs, long glyphCount);
long MapString2(uint16* map, uint8* charCodes, int16* glyphs, long glyphCount, long* textLength);
long MapString4_8(uint16* map, uint8* charCodes, int16* glyphs, long glyphCount);
long MapString4_16(uint16* map, uint16* charCodes, int16* glyphs, long glyphCount);
long MapString6_8(uint16* map, uint8* charCodes, int16* glyphs, long glyphCount);
long MapString6_16(uint16* map, uint16* charCodes, int16* glyphs, long glyphCount);
