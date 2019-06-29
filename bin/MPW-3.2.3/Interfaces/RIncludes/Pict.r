/*
	Pict.r -- Type Declaration for Rez and DeRez
	
	Copyright Apple Computer, Inc. 1986-1989
	All rights reserved.

	This PICT resource definition should probably used
	for DeRez’ing purposes only.
	
*/

#ifndef __PICT.R__
#define __PICT.R__

type 'PICT' {
	startOfPict:
		unsigned integer = (endOfPict - startOfPict) >> 3;
		Rect;									/* Bounds of picture */
		switch {
			case VersionOne:
				key integer = $1101;
				array OpCodes {
					switch {
						case NOP:
							key byte = $00;

						case clipRgn:
							key byte = $01;
	clipRgnStart:			integer = (clipRgnEnd[$$ArrayIndex(OpCodes)] - clipRgnStart[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(clipRgnStart[$$ArrayIndex(OpCodes)]) - 10];
	clipRgnEnd:
						case bkPat:
							key byte = $02;
							hex string [8];
						
						case txFont:
							key byte = $03;
							integer;			/* font */
						
						case txFace:
							key byte = $04;
							unsigned hex byte;	/* text face */
						
						case txMode:
							key byte = $05;
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic;
						
						case spExtra:
							key byte = $06;
							unsigned hex longint;
						
						case pnSize:
							key byte = $07;
							Point;
						
						case pnMode:
							key byte = $08;
							unsigned hex integer;
						
						case pnPat:
							key byte = $09;
							hex string [8];
						
						case thePat:
							key byte = $0A;
							hex string [8];
						
						case ovSize:
							key byte = $0B;
							Point;
						
						case origin:
							key byte = $0C;
							integer;			/* dh */
							integer;			/* dv */
						
						case txSize:
							key byte = $0D;
							integer;
						
						case fgColor:
							key byte = $0E;
							longint;
						
						case bkColor:
							key byte = $0F;
							longint;
						
						case txRatio:
							key byte = $10;
							Point;				/* numer */
							Point;				/* denom */
						
						case picVersion:
							key byte = $11;
							byte;
						
						case line:
							key byte = $20;
							Point;				/* pnLoc */
							Point;				/* newPt */
						
						case lineFrom:
							key byte = $21;
							Point;				/* newPt */
						
						case shortLine:
							key byte = $22;
							Point;				/* pnLoc */
							byte;				/* dh */
							byte;				/* dv */
						
						case shortLineFrom:
							key byte = $23;
							byte;				/* dh */
							byte;				/* dv */
						
						case longText:
							key byte = $28;
							Point;				/* txLoc */
							pstring;
						
						case dhText:
							key byte = $29;
							byte;				/* dh */
							pstring;
							
						case dvText:
							key byte = $2A;
							byte;				/* dvh */
							pstring;
							
						case dhdvText:
							key byte = $2B;
							byte;				/* dh */
							byte;				/* dv */
							pstring;
							
						case frameRect:
							key byte = $30;
							Rect;
							
						case paintRect:
							key byte = $31;
							Rect;
							
						case eraseRect:
							key byte = $32;
							Rect;
							
						case invertRect:
							key byte = $33;
							Rect;
							
						case fillRect:
							key byte = $34;
							Rect;
						
						case frameSameRect:
							key byte = $38;
							
						case paintSameRect:
							key byte = $39;
							
						case eraseSameRect:
							key byte = $3A;
							
						case invertSameRect:
							key byte = $3B;
							
						case fillSameRect:
							key byte = $3C;
						
						case frameRRect:
							key byte = $40;
							Rect;
							
						case paintRRect:
							key byte = $41;
							Rect;
							
						case eraseRRect:
							key byte = $42;
							Rect;
							
						case invertRRect:
							key byte = $43;
							Rect;
							
						case fillRRect:
							key byte = $44;
							Rect;
							
						case frameSameRRect:
							key byte = $48;
							
						case paintSameRRect:
							key byte = $49;
							
						case eraseSameRRect:
							key byte = $4A;
							
						case invertSameRRect:
							key byte = $4B;
							
						case fillSameRRect:
							key byte = $4C;
							
						case frameOval:
							key byte = $50;
							Rect;
							
						case paintOval:
							key byte = $51;
							Rect;
							
						case eraseOval:
							key byte = $52;
							Rect;
							
						case invertOval:
							key byte = $53;
							Rect;
							
						case fillOval:
							key byte = $54;
							Rect;
						
						case frameSameOval:
							key byte = $58;
							
						case paintSameOval:
							key byte = $59;
							
						case eraseSameOval:
							key byte = $5A;
							
						case invertSameOval:
							key byte = $5B;
							
						case fillSameOval:
							key byte = $5C;
						
						case frameArc:
							key byte = $60;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case paintArc:
							key byte = $61;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case eraseArc:
							key byte = $62;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case invertArc:
							key byte = $63;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case fillArc:
							key byte = $64;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case frameSameArc:
							key byte = $68;
							integer;				/* startAngle */
							integer;				/* arcAngle */

						case paintSameArc:
							key byte = $69;
							integer;				/* startAngle */
							integer;				/* arcAngle */

						case eraseSameArc:
							key byte = $6A;
							integer;				/* startAngle */
							integer;				/* arcAngle */

						case invertSameArc:
							key byte = $6B;
							integer;				/* startAngle */
							integer;				/* arcAngle */

						case fillSameArc:
							key byte = $6C;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case framePoly:
							key byte = $70;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case paintPoly:
							key byte = $71;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case erasePoly:
							key byte = $72;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case invertPoly:
							key byte = $73;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case fillPoly:
							key byte = $74;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case frameSamePoly:
							key byte = $78;
	
						case paintSamePoly:
							key byte = $79;
	
						case eraseSamePoly:
							key byte = $7A;
	
						case invertSamePoly:
							key byte = $7B;
	
						case fillSamePoly:
							key byte = $7C;
	
						case frameRgn:
							key byte = $80;
	frameRgnStart:			integer = (frameRgnEnd[$$ArrayIndex(OpCodes)] - frameRgnStart[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(frameRgnStart[$$ArrayIndex(OpCodes)]) - 10];
	frameRgnEnd:
						case paintRgn:
							key byte = $81;
	paintRgnStart:			integer = (paintRgnEnd[$$ArrayIndex(OpCodes)] - paintRgnStart[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(paintRgnStart[$$ArrayIndex(OpCodes)]) - 10];
	paintRgnEnd:
						case eraseRgn:
							key byte = $82;
	eraseRgnStart:			integer = (eraseRgnEnd[$$ArrayIndex(OpCodes)] - eraseRgnStart[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(eraseRgnStart[$$ArrayIndex(OpCodes)]) - 10];
	eraseRgnEnd:
						case invertRgn:
							key byte = $83;
	invertRgnStart:			integer = (invertRgnEnd[$$ArrayIndex(OpCodes)] - invertRgnStart[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(invertRgnStart[$$ArrayIndex(OpCodes)]) - 10];
	invertRgnEnd:
						case fillRgn:
							key byte = $84;
	fillRgnStart:			integer = (fillRgnEnd[$$ArrayIndex(OpCodes)] - fillRgnStart[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(fillRgnStart[$$ArrayIndex(OpCodes)]) - 10];
	fillRgnEnd:
						case frameSameRgn:
							key byte = $88;

						case paintSameRgn:
							key byte = $89;

						case eraseSameRgn:
							key byte = $8A;

						case invertSameRgn:
							key byte = $8B;

						case fillSameRgn:
							key byte = $8C;

						case bitsRect:
							key byte = $90;
	BitsRectRowBytes:		integer;					/* row bytes */
	BitsRectBounds:			Rect;						/* bounds */
							Rect;						/* srcRect */
							Rect;						/* destRect */
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic;
							hex string [($$BitField(BitsRectBounds[$$ArrayIndex(OpCodes)],32,16)
										- $$BitField(BitsRectBounds[$$ArrayIndex(OpCodes)],0,16))
										* $$Word(BitsRectRowBytes[$$ArrayIndex(OpCodes)])];

						case bitsRgn:
							key byte = $91;
	BitsRgnRowBytes:		integer;					/* row bytes */
	BitsRgnBounds:			Rect;						/* bounds */
							Rect;						/* srcRect */
							Rect;						/* destRect */
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic;
	bitsRgnStart:			integer = (bitsRgnEnd[$$ArrayIndex(OpCodes)] - bitsRgnStart[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(bitsRgnStart[$$ArrayIndex(OpCodes)]) - 10];
	bitsRgnEnd:
							hex string [($$BitField(BitsRgnBounds[$$ArrayIndex(OpCodes)],32,16)
										- $$BitField(BitsRgnBounds[$$ArrayIndex(OpCodes)],0,16))
										* $$Word(BitsRgnRowBytes[$$ArrayIndex(OpCodes)])];

						case packBitsRect:
							key byte = $98;
	PackBitsRectRowBytes:	integer;					/* row bytes */
	PackBitsRectBounds:		Rect;						/* bounds */
							Rect;						/* srcRect */
							Rect;						/* destRect */
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic;
	PackBitsBits:			hex string [$$PackedSize(PackBitsBits[$$ArrayIndex(OpCodes)],
										$$Word(PackBitsRectRowBytes[$$ArrayIndex(OpCodes)]),
										($$BitField(PackBitsRectBounds[$$ArrayIndex(OpCodes)],32,16)
										- $$BitField(PackBitsRectBounds[$$ArrayIndex(OpCodes)],0,16)))];

						case packBitsRgn:
							key byte = $99;
	packBitsRgnRowBytes:	integer;					/* row bytes */
	packBitsRgnBounds:		Rect;						/* bounds */
							Rect;						/* srcRect */
							Rect;						/* destRect */
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic;
	packBitsRgnStart:		integer = (packBitsRgnEnd[$$ArrayIndex(OpCodes)] - packBitsRgnStart[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(packBitsRgnStart[$$ArrayIndex(OpCodes)]) - 10];
	packBitsRgnEnd:			hex string [$$PackedSize(packBitsRgnEnd[$$ArrayIndex(OpCodes)],
										$$Word(packBitsRgnRowBytes[$$ArrayIndex(OpCodes)]),
										($$BitField(packBitsRgnBounds[$$ArrayIndex(OpCodes)],32,16)
										- $$BitField(packBitsRgnBounds[$$ArrayIndex(OpCodes)],0,16)))];
	
						case shortComment:
							key byte = $A0;
							integer;			/* kind */

						case longComment:
							key byte = $A1;
							integer;			/* kind */
	longCommentStart:		integer = (longCommentEnd[$$ArrayIndex(OpCodes)] - longCommentStart[$$ArrayIndex(OpCodes)] - 16) >> 3;
							hex string [$$Word(longCommentStart[$$ArrayIndex(OpCodes)])];
	longCommentEnd:
					};
				};
				byte = $FF; 	/* End of picture */
				
			case VersionTwo:
				key longint = $001102ff;		/* Version op + version */
				array OpCodes {
					switch {
						case Clip:
							key integer = $0001;
	ClipStart:				integer = (ClipEnd[$$ArrayIndex(OpCodes)] - ClipStart[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(ClipStart[$$ArrayIndex(OpCodes)]) - 10];
	ClipEnd:
						case BkPat:
							key integer = $0002;
							hex string [8];
						
						case TxFont:
							key integer = $0003;
							integer;			/* font */
						
						case TxFace:
							key integer = $0004;
							unsigned hex byte;	/* text face */
						
						case TxMode:
							key integer = $0005;
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic;
						
						case SpExtra:
							key integer = $0006;
							unsigned hex longint;
						
						case PnSize:
							key integer = $0007;
							Point;
						
						case PnMode:
							key integer = $0008;
							unsigned hex integer;
						
						case PnPat:
							key integer = $0009;
							hex string [8];
						
						case FillPat:
							key integer = $000A;
							hex string [8];
						
						case OvSize:
							key integer = $000B;
							Point;
						
						case Origin:
							key integer = $000C;
							integer;			/* dh */
							integer;			/* dv */
						
						case TxSize:
							key integer = $000D;
							integer;
						
						case FgColor:
							key integer = $000E;
							longint;
						
						case BkColor:
							key integer = $000F;
							longint;
						
						case TxRatio:
							key integer = $0010;
							Point;				/* numer */
							Point;				/* denom */
						
						case Version:
							key integer = $0011;
							byte;

						case BkPixPat:
							key integer = $0012;
							switch {
								case newPattern:
									key integer = 1;
									hex string [8];			/* old-style pattern	*/
									unsigned bitstring[1] = 1;								/* New pixMap flag		*/
									unsigned bitstring[2] = 0;								/* Must be 0			*/
	BkPixPatpMapRowBytes:			unsigned bitstring[13];									/* Offset to next row	*/
	BkPixPatBounds:					rect;													/* Bitmap bounds		*/
									integer;												/* pixMap vers number	*/
									integer	unpacked;										/* Packing format		*/
									unsigned longint;										/* Size of pixel data	*/
									unsigned hex longint;									/* h. resolution (ppi) (fixed) */
									unsigned hex longint;									/* v. resolution (ppi) (fixed) */
									integer			chunky, chunkyPlanar, planar;			/* Pixel storage format	*/
									integer;												/* # bits in pixel		*/
									integer;												/* # components in pixel*/
									integer;												/* # bits per field		*/
									unsigned longint;										/* Offset to next plane	*/
									unsigned longint;										/* Offset to color table*/
									fill long;												/* Reserved				*/
									/* Color Table */
									unsigned hex longint;									/* ctSeed				*/
									integer;												/* ctFlags				*/
									integer = $$Countof(ColorSpec) - 1;						/* ctSize				*/
									wide array ColorSpec {
											integer;										/* value				*/
											unsigned integer;								/* RGB:	red				*/
											unsigned integer;								/*		green			*/
											unsigned integer;								/*		blue			*/
									};
									/* If rowBytes < 8 then data is unpacked,
									   if >= 8, then data is packed */
	BkPixPatBits:					hex string [($$BitField(BkPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) < 8) *
												$$BitField(BkPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) +
												($$BitField(BkPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) >= 8) *
												$$PackedSize(BkPixPatBits[$$ArrayIndex(OpCodes)],
												$$BitField(BkPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13),
												($$BitField(BkPixPatBounds[$$ArrayIndex(OpCodes)],32,16)
												- $$BitField(BkPixPatBounds[$$ArrayIndex(OpCodes)],0,16)))];
								
								case ditherPattern:
									key integer = 2;
									hex string [8];			/* old-style pattern	*/
									unsigned integer;		/* RGB:	red				*/
									unsigned integer;		/*		green			*/
									unsigned integer;		/*		blue			*/
							};

						case PnPixPat:
							key integer = $0013;
							switch {
								case newPattern:
									key integer = 1;
									hex string [8];			/* old-style pattern	*/
									unsigned bitstring[1] = 1;								/* New pixMap flag		*/
									unsigned bitstring[2] = 0;								/* Must be 0			*/
	PnPixPatpMapRowBytes:			unsigned bitstring[13];									/* Offset to next row	*/
	PnPixPatBounds:					rect;													/* Bitmap bounds		*/
									integer;												/* pixMap vers number	*/
									integer	unpacked;										/* Packing format		*/
									unsigned longint;										/* Size of pixel data	*/
									unsigned hex longint;									/* h. resolution (ppi) (fixed) */
									unsigned hex longint;									/* v. resolution (ppi) (fixed) */
									integer			chunky, chunkyPlanar, planar;			/* Pixel storage format	*/
									integer;												/* # bits in pixel		*/
									integer;												/* # components in pixel*/
									integer;												/* # bits per field		*/
									unsigned longint;										/* Offset to next plane	*/
									unsigned longint;										/* Offset to color table*/
									fill long;												/* Reserved				*/
									/* Color Table */
									unsigned hex longint;									/* ctSeed				*/
									integer;												/* ctFlags				*/
									integer = $$Countof(ColorSpec) - 1;						/* ctSize				*/
									wide array ColorSpec {
											integer;										/* value				*/
											unsigned integer;								/* RGB:	red				*/
											unsigned integer;								/*		green			*/
											unsigned integer;								/*		blue			*/
									};
									/* If rowBytes < 8 then data is unpacked,
									   if >= 8, then data is packed */
	PnPixPatBits:					hex string [($$BitField(PnPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) < 8) *
												$$BitField(PnPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) +
												($$BitField(PnPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) >= 8) *
												$$PackedSize(PnPixPatBits[$$ArrayIndex(OpCodes)],
												$$BitField(PnPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13),
												($$BitField(PnPixPatBounds[$$ArrayIndex(OpCodes)],32,16)
												- $$BitField(PnPixPatBounds[$$ArrayIndex(OpCodes)],0,16)))];
								
								case ditherPattern:
									key integer = 2;
									hex string [8];			/* old-style pattern	*/
									unsigned integer;		/* RGB:	red				*/
									unsigned integer;		/*		green			*/
									unsigned integer;		/*		blue			*/
							};

						case FillPixPat:
							key integer = $0014;
							switch {
								case newPattern:
									key integer = 1;
									hex string [8];			/* old-style pattern	*/
									unsigned bitstring[1] = 1;								/* New pixMap flag		*/
									unsigned bitstring[2] = 0;								/* Must be 0			*/
	FillPixPatpMapRowBytes:			unsigned bitstring[13];									/* Offset to next row	*/
	FillPixPatBounds:					rect;													/* Bitmap bounds		*/
									integer;												/* pixMap vers number	*/
									integer	unpacked;										/* Packing format		*/
									unsigned longint;										/* Size of pixel data	*/
									unsigned hex longint;									/* h. resolution (ppi) (fixed) */
									unsigned hex longint;									/* v. resolution (ppi) (fixed) */
									integer			chunky, chunkyPlanar, planar;			/* Pixel storage format	*/
									integer;												/* # bits in pixel		*/
									integer;												/* # components in pixel*/
									integer;												/* # bits per field		*/
									unsigned longint;										/* Offset to next plane	*/
									unsigned longint;										/* Offset to color table*/
									fill long;												/* Reserved				*/
									/* Color Table */
									unsigned hex longint;									/* ctSeed				*/
									integer;												/* ctFlags				*/
									integer = $$Countof(ColorSpec) - 1;						/* ctSize				*/
									wide array ColorSpec {
											integer;										/* value				*/
											unsigned integer;								/* RGB:	red				*/
											unsigned integer;								/*		green			*/
											unsigned integer;								/*		blue			*/
									};
									/* If rowBytes < 8 then data is unpacked,
									   if >= 8, then data is packed */
	FillPixPatBits:					hex string [($$BitField(FillPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) < 8) *
												$$BitField(FillPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) +
												($$BitField(FillPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) >= 8) *
												$$PackedSize(FillPixPatBits[$$ArrayIndex(OpCodes)],
												$$BitField(FillPixPatpMapRowBytes[$$ArrayIndex(OpCodes)],0,13),
												($$BitField(FillPixPatBounds[$$ArrayIndex(OpCodes)],32,16)
												- $$BitField(FillPixPatBounds[$$ArrayIndex(OpCodes)],0,16)))];
								
								case ditherPattern:
									key integer = 2;
									hex string [8];			/* old-style pattern	*/
									unsigned integer;		/* RGB:	red				*/
									unsigned integer;		/*		green			*/
									unsigned integer;		/*		blue			*/
							};
						
						case PnLocHFrac:
							key integer = $0015;
							unsigned hex integer;
						
						case ChExtra:
							key integer = $0016;
							integer;						/* extra for each character */
						
						case AppleReserved0017:
							key integer = $0017;
							
						case AppleReserved0018:
							key integer = $0018;
							
						case AppleReserved0019:
							key integer = $0019;
						
						case RGBFgCol:
							key integer = $001A;
							unsigned integer;		/* RGB:	red				*/
							unsigned integer;		/*		green			*/
							unsigned integer;		/*		blue			*/
							
						case RGBBkCol:
							key integer = $001B;
							unsigned integer;		/* RGB:	red				*/
							unsigned integer;		/*		green			*/
							unsigned integer;		/*		blue			*/
							
						case HiliteMode:
							key integer = $001C;
						
						case HiliteColor:
							key integer = $001D;
							unsigned integer;		/* RGB:	red				*/
							unsigned integer;		/*		green			*/
							unsigned integer;		/*		blue			*/
						
						case DefHilite:
							key integer = $001E;
						
						case OpColor:
							key integer = $001F;
							unsigned integer;		/* RGB:	red				*/
							unsigned integer;		/*		green			*/
							unsigned integer;		/*		blue			*/
						
						case Line:
							key integer = $0020;
							Point;					/* pnLoc */
							Point;					/* newPt */
							
						case LineFrom:
							key integer = $0021;
							Point;					/* newPt */
						
						case ShortLine:
							key integer = $0022;
							Point;					/* pnLoc */
							byte;					/* dh */
							byte;					/* dv */
						
						case ShortLineFrom:
							key integer = $0023;
							byte;					/* dh */
							byte;					/* dv */
						
						case AppleReserved0024:
							key integer = $0024;
				AR24:		unsigned integer = (AR24End - AR24) / 8 - 2;
							hex string [$$Word(AR24)];
				AR24End:
						
						case AppleReserved0025:
							key integer = $0025;
				AR25:		unsigned integer = (AR25End - AR25) / 8 - 2;
							hex string [$$Word(AR25)];
				AR25End:
						
						case AppleReserved0026:
							key integer = $0026;
				AR26:		unsigned integer = (AR26End - AR26) / 8 - 2;
							hex string [$$Word(AR26)];
				AR26End:
						
						case AppleReserved0027:
							key integer = $0027;
				AR27:		unsigned integer = (AR27End - AR27) / 8 - 2;
							hex string [$$Word(AR27)];
				AR27End:
						
						case LongText:
							key integer = $0028;
							Point;				/* txLoc */
							pstring;			/* text */
						
						case DHText:
							key integer = $0029;
							byte;				/* dh */
							pstring;			/* text */

						case DVText:
							key integer = $002A;
							byte;				/* dv */
							pstring;			/* text */

						case DHDVText:
							key integer = $002A;
							byte;				/* dh */
							byte;				/* dv */
							pstring;			/* text */

						case AppleReserved002C:
							key integer = $002C;
				AR2C:		unsigned integer = (AR2CEnd - AR2C) / 8 - 2;
							hex string [$$Word(AR2C)];
				AR2CEnd:
						
						case AppleReserved002D:
							key integer = $002D;
				AR2D:		unsigned integer = (AR2DEnd - AR2D) / 8 - 2;
							hex string [$$Word(AR2D)];
				AR2DEnd:
						
						case AppleReserved002E:
							key integer = $002E;
				AR2E:		unsigned integer = (AR2EEnd - AR2E) / 8 - 2;
							hex string [$$Word(AR2E)];
				AR2EEnd:
						
						case AppleReserved002F:
							key integer = $002F;
				AR2F:		unsigned integer = (AR2FEnd - AR2F) / 8 - 2;
							hex string [$$Word(AR2F)];
				AR2FEnd:
						
						case FrameRect:
							key integer = $0030;
							Rect;
							
						case PaintRect:
							key integer = $0031;
							Rect;
							
						case EraseRect:
							key integer = $0032;
							Rect;
							
						case InvertRect:
							key integer = $0033;
							Rect;
							
						case FillRect:
							key integer = $0034;
							Rect;
							
						case AppleReserved0035:
							key integer = $0035;
							Rect;
							
						case AppleReserved0036:
							key integer = $0036;
							Rect;
							
						case AppleReserved0037:
							key integer = $0037;
							Rect;
						
						case FrameSameRect:
							key integer = $0038;
						
						case PaintSameRect:
							key integer = $0039;
						
						case EraseSameRect:
							key integer = $003A;
						
						case InvertSameRect:
							key integer = $003B;
						
						case FillSameRect:
							key integer = $003C;
						
						case AppleReserved003D:
							key integer = $003D;
						
						case AppleReserved003E:
							key integer = $003E;
						
						case AppleReserved003F:
							key integer = $003F;
						
						case FrameRRect:
							key integer = $0040;
							Rect;
							
						case PaintRRect:
							key integer = $0041;
							Rect;
							
						case EraseRRect:
							key integer = $0042;
							Rect;
							
						case InvertRRect:
							key integer = $0043;
							Rect;
							
						case FillRRect:
							key integer = $0044;
							Rect;
							
						case AppleReserved0045:
							key integer = $0045;
							Rect;
							
						case AppleReserved0046:
							key integer = $0046;
							Rect;
							
						case AppleReserved0047:
							key integer = $0047;
							Rect;
						
						case FrameSameRRect:
							key integer = $0048;
						
						case PaintSameRRect:
							key integer = $0049;
						
						case EraseSameRRect:
							key integer = $004A;
						
						case InvertSameRRect:
							key integer = $004B;
						
						case FillSameRRect:
							key integer = $004C;
						
						case AppleReserved004D:
							key integer = $004D;
						
						case AppleReserved004E:
							key integer = $004E;
						
						case AppleReserved004F:
							key integer = $004F;
						
						case FrameOval:
							key integer = $0050;
							Rect;
							
						case PaintOval:
							key integer = $0051;
							Rect;
							
						case EraseOval:
							key integer = $0052;
							Rect;
							
						case InvertOval:
							key integer = $0053;
							Rect;
							
						case FillOval:
							key integer = $0054;
							Rect;
							
						case AppleReserved0055:
							key integer = $0055;
							Rect;
							
						case AppleReserved0056:
							key integer = $0056;
							Rect;
							
						case AppleReserved0057:
							key integer = $0057;
							Rect;
						
						case FrameSameOval:
							key integer = $0058;
						
						case PaintSameOval:
							key integer = $0059;
						
						case EraseSameOval:
							key integer = $005A;
						
						case InvertSameOval:
							key integer = $005B;
						
						case FillSameOval:
							key integer = $005C;
						
						case AppleReserved005D:
							key integer = $005D;
						
						case AppleReserved005E:
							key integer = $005E;
						
						case frameArc:
							key integer = $0060;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case paintArc:
							key integer = $0061;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case eraseArc:
							key integer = $0062;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case invertArc:
							key integer = $0063;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case fillArc:
							key integer = $0064;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case AppleReserved0065:
							key integer = $0065;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case AppleReserved0066:
							key integer = $0066;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case AppleReserved0067:
							key integer = $0067;
							Rect;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case frameSameArc:
							key integer = $0068;
							integer;				/* startAngle */
							integer;				/* arcAngle */

						case paintSameArc:
							key integer = $0069;
							integer;				/* startAngle */
							integer;				/* arcAngle */

						case eraseSameArc:
							key integer = $006A;
							integer;				/* startAngle */
							integer;				/* arcAngle */

						case invertSameArc:
							key integer = $006B;
							integer;				/* startAngle */
							integer;				/* arcAngle */

						case fillSameArc:
							key integer = $006C;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case AppleReserved006D:
							key integer = $006D;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case AppleReserved006E:
							key integer = $006E;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case AppleReserved006F:
							key integer = $006F;
							integer;				/* startAngle */
							integer;				/* arcAngle */
						
						case framePoly:
							key integer = $0070;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case paintPoly:
							key integer = $0071;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case erasePoly:
							key integer = $0072;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case invertPoly:
							key integer = $0073;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case fillPoly:
							key integer = $0074;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case AppleReserved0075:
							key integer = $0075;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case AppleReserved0076:
							key integer = $0076;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case AppleReserved0077:
							key integer = $0077;
							integer = $$CountOf(framePoly) * 4 + 10;
							Rect;					/* enclosing rectangle */
							array framePoly {
								Point;
							};
						
						case frameSamePoly:
							key integer = $0078;
	
						case paintSamePoly:
							key integer = $0079;
	
						case eraseSamePoly:
							key integer = $007A;
	
						case invertSamePoly:
							key integer = $007B;
	
						case fillSamePoly:
							key integer = $007C;
	
						case AppleReserved007D:
							key integer = $007D;
	
						case AppleReserved007E:
							key integer = $007E;
	
						case AppleReserved007F:
							key integer = $007F;
	
						case frameRgn:
							key integer = $0080;
	frameRgnStart2:			integer = (frameRgnEnd2[$$ArrayIndex(OpCodes)] - frameRgnStart2[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(frameRgnStart2[$$ArrayIndex(OpCodes)]) - 10];
	frameRgnEnd2:
						case paintRgn:
							key integer = $0081;
	paintRgnStart2:			integer = (paintRgnEnd2[$$ArrayIndex(OpCodes)] - paintRgnStart2[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(paintRgnStart2[$$ArrayIndex(OpCodes)]) - 10];
	paintRgnEnd2:
						case eraseRgn:
							key integer = $0082;
	eraseRgnStart2:			integer = (eraseRgnEnd2[$$ArrayIndex(OpCodes)] - eraseRgnStart2[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(eraseRgnStart2[$$ArrayIndex(OpCodes)]) - 10];
	eraseRgnEnd2:
						case invertRgn:
							key integer = $0083;
	invertRgnStart2:		integer = (invertRgnEnd2[$$ArrayIndex(OpCodes)] - invertRgnStart2[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(invertRgnStart2[$$ArrayIndex(OpCodes)]) - 10];
	invertRgnEnd2:
						case fillRgn:
							key integer = $0084;
	fillRgnStart2:			integer = (fillRgnEnd2[$$ArrayIndex(OpCodes)] - fillRgnStart2[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(fillRgnStart2[$$ArrayIndex(OpCodes)]) - 10];
	fillRgnEnd2:
						case AppleReserved0085:
							key integer = $0085;
	AppleReserved0085Start:integer = (AppleReserved0085End[$$ArrayIndex(OpCodes)] - AppleReserved0085Start[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(AppleReserved0085Start[$$ArrayIndex(OpCodes)]) - 10];
	AppleReserved0085End:
						case AppleReserved0086:
							key integer = $0086;
	AppleReserved0086Start:	integer = (AppleReserved0086End[$$ArrayIndex(OpCodes)] - AppleReserved0086Start[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(AppleReserved0086Start[$$ArrayIndex(OpCodes)]) - 10];
	AppleReserved0086End:
						case AppleReserved0087:
							key integer = $0087;
	AppleReserved0087Start:	integer = (AppleReserved0087End[$$ArrayIndex(OpCodes)] - AppleReserved0087Start[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(AppleReserved0087Start[$$ArrayIndex(OpCodes)]) - 10];
	AppleReserved0087End:
						case frameSameRgn:
							key integer = $0088;

						case paintSameRgn:
							key integer = $0089;

						case eraseSameRgn:
							key integer = $008A;

						case invertSameRgn:
							key integer = $008B;

						case fillSameRgn:
							key integer = $008C;

						case AppleReserved008D:
							key integer = $008D;

						case AppleReserved008E:
							key integer = $008E;

						case AppleReserved008F:
							key integer = $008F;

						case BitsRect:
							key integer = $0090;
							unsigned bitstring[1] = 1;								/* New pixMap flag		*/
							unsigned bitstring[2] = 0;								/* Must be 0			*/
	BitsRectpMapRowBytes:	unsigned bitstring[13];									/* Offset to next row	*/
	BitsRectBounds2:		rect;													/* Bitmap bounds		*/
							integer;												/* pixMap vers number	*/
							integer	unpacked;										/* Packing format		*/
							unsigned longint;										/* Size of pixel data	*/
							unsigned hex longint;									/* h. resolution (ppi) (fixed) */
							unsigned hex longint;									/* v. resolution (ppi) (fixed) */
							integer			chunky, chunkyPlanar, planar;			/* Pixel storage format	*/
							integer;												/* # bits in pixel		*/
							integer;												/* # components in pixel*/
							integer;												/* # bits per field		*/
							unsigned longint;										/* Offset to next plane	*/
							unsigned longint;										/* Offset to color table*/
							fill long;												/* Reserved				*/
							/* Color Table */
							unsigned hex longint;									/* ctSeed				*/
							integer;												/* ctFlags				*/
							integer = $$Countof(ColorSpec) - 1;						/* ctSize				*/
							wide array ColorSpec {
									integer;										/* value				*/
									unsigned integer;								/* RGB:	red				*/
									unsigned integer;								/*		green			*/
									unsigned integer;								/*		blue			*/
							};
							Rect;						/* srcRect */
							Rect;						/* destRect */
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic,
										blend = 32, addPin, addOver, subPin,
										adMax = 37, subOver, adMin;
	BitsRectBits:			hex string [$$BitField(BitsRectpMapRowBytes[$$ArrayIndex(OpCodes)],0,13) *
										($$BitField(BitsRectBounds2[$$ArrayIndex(OpCodes)],32,16)
										- $$BitField(BitsRectBounds2[$$ArrayIndex(OpCodes)],0,16))];
						
						
						case BitsRgn:
							key integer = $0091;
							unsigned bitstring[1] = 1;								/* New pixMap flag		*/
							unsigned bitstring[2] = 0;								/* Must be 0			*/
	BitsRgnpMapRowBytes:	unsigned bitstring[13];									/* Offset to next row	*/
	BitsRgnBounds2:			rect;													/* Bitmap bounds		*/
							integer;												/* pixMap vers number	*/
							integer	unpacked;										/* Packing format		*/
							unsigned longint;										/* Size of pixel data	*/
							unsigned hex longint;									/* h. resolution (ppi) (fixed) */
							unsigned hex longint;									/* v. resolution (ppi) (fixed) */
							integer			chunky, chunkyPlanar, planar;			/* Pixel storage format	*/
							integer;												/* # bits in pixel		*/
							integer;												/* # components in pixel*/
							integer;												/* # bits per field		*/
							unsigned longint;										/* Offset to next plane	*/
							unsigned longint;										/* Offset to color table*/
							fill long;												/* Reserved				*/
							/* Color Table */
							unsigned hex longint;									/* ctSeed				*/
							integer;												/* ctFlags				*/
							integer = $$Countof(ColorSpec) - 1;						/* ctSize				*/
							wide array ColorSpec {
									integer;										/* value				*/
									unsigned integer;								/* RGB:	red				*/
									unsigned integer;								/*		green			*/
									unsigned integer;								/*		blue			*/
							};
							Rect;						/* srcRect */
							Rect;						/* destRect */
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic,
										blend = 32, addPin, addOver, subPin,
										adMax = 37, subOver, adMin;
	BitsRgnStart2:			integer = (BitsRgnEnd2[$$ArrayIndex(OpCodes)] - BitsRgnStart2[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(BitsRgnStart2[$$ArrayIndex(OpCodes)]) - 10];
	BitsRgnEnd2:			hex string [$$PackedSize(BitsRgnEnd2[$$ArrayIndex(OpCodes)],
										$$BitField(BitsRgnpMapRowBytes[$$ArrayIndex(OpCodes)],0,13),
										($$BitField(BitsRgnBounds2[$$ArrayIndex(OpCodes)],32,16)
										- $$BitField(BitsRgnBounds2[$$ArrayIndex(OpCodes)],0,16)))];

						case AppleReserved0092:
							key integer = $0092;
	AppleReserved0092Start:	unsigned integer = (AppleReserved0092End[$$ArrayIndex(OpCodes)] - AppleReserved0092Start[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved0092Start[$$ArrayIndex(OpCodes)])];
	AppleReserved0092End:
						case AppleReserved0093:
							key integer = $0093;
	AppleReserved0093Start:	unsigned integer = (AppleReserved0093End[$$ArrayIndex(OpCodes)] - AppleReserved0093Start[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved0093Start[$$ArrayIndex(OpCodes)])];
	AppleReserved0093End:
						case AppleReserved0094:
							key integer = $0094;
	AppleReserved0094Start:	unsigned integer = (AppleReserved0094End[$$ArrayIndex(OpCodes)] - AppleReserved0094Start[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved0094Start[$$ArrayIndex(OpCodes)])];
	AppleReserved0094End:
						case AppleReserved0095:
							key integer = $0095;
	AppleReserved0095Start:	unsigned integer = (AppleReserved0095End[$$ArrayIndex(OpCodes)] - AppleReserved0095Start[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved0095Start[$$ArrayIndex(OpCodes)])];
	AppleReserved0095End:
						case AppleReserved0096:
							key integer = $0096;
	AppleReserved0096Start:	unsigned integer = (AppleReserved0096End[$$ArrayIndex(OpCodes)] - AppleReserved0096Start[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved0096Start[$$ArrayIndex(OpCodes)])];
	AppleReserved0096End:
						case AppleReserved0097:
							key integer = $0097;
	AppleReserved0097Start:	unsigned integer = (AppleReserved0097End[$$ArrayIndex(OpCodes)] - AppleReserved0097Start[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved0097Start[$$ArrayIndex(OpCodes)])];
	AppleReserved0097End:
						case PackBitsRect:
							key integer = $0098;
							unsigned bitstring[1] = 1;								/* New pixMap flag		*/
							unsigned bitstring[2] = 0;								/* Must be 0			*/
	PackBitsRectpMapRowBytes:unsigned bitstring[13];									/* Offset to next row	*/
	PackBitsRectBounds2:	rect;													/* Bitmap bounds		*/
							integer;												/* pixMap vers number	*/
							integer	unpacked;										/* Packing format		*/
							unsigned longint;										/* Size of pixel data	*/
							unsigned hex longint;									/* h. resolution (ppi) (fixed) */
							unsigned hex longint;									/* v. resolution (ppi) (fixed) */
							integer			chunky, chunkyPlanar, planar;			/* Pixel storage format	*/
							integer;												/* # bits in pixel		*/
							integer;												/* # components in pixel*/
							integer;												/* # bits per field		*/
							unsigned longint;										/* Offset to next plane	*/
							unsigned longint;										/* Offset to color table*/
							fill long;												/* Reserved				*/
							/* Color Table */
							unsigned hex longint;									/* ctSeed				*/
							integer;												/* ctFlags				*/
							integer = $$Countof(ColorSpec) - 1;						/* ctSize				*/
							wide array ColorSpec {
									integer;										/* value				*/
									unsigned integer;								/* RGB:	red				*/
									unsigned integer;								/*		green			*/
									unsigned integer;								/*		blue			*/
							};
							Rect;						/* srcRect */
							Rect;						/* destRect */
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic,
										blend = 32, addPin, addOver, subPin,
										adMax = 37, subOver, adMin;
	PackBitsRectBits:		hex string [$$PackedSize(PackBitsRectBits[$$ArrayIndex(OpCodes)],
										$$BitField(PackBitsRectpMapRowBytes[$$ArrayIndex(OpCodes)],0,13),
										($$BitField(PackBitsRectBounds2[$$ArrayIndex(OpCodes)],32,16)
										- $$BitField(PackBitsRectBounds2[$$ArrayIndex(OpCodes)],0,16)))];
						
						case PackBitsRgn:
							key integer = $0099;
							unsigned bitstring[1] = 1;								/* New pixMap flag		*/
							unsigned bitstring[2] = 0;								/* Must be 0			*/
	PackBitsRgnpMapRowBytes:unsigned bitstring[13];									/* Offset to next row	*/
	PackBitsRgnBounds2:	rect;														/* Bitmap bounds		*/
							integer;												/* pixMap vers number	*/
							integer	unpacked;										/* Packing format		*/
							unsigned longint;										/* Size of pixel data	*/
							unsigned hex longint;									/* h. resolution (ppi) (fixed) */
							unsigned hex longint;									/* v. resolution (ppi) (fixed) */
							integer			chunky, chunkyPlanar, planar;			/* Pixel storage format	*/
							integer;												/* # bits in pixel		*/
							integer;												/* # components in pixel*/
							integer;												/* # bits per field		*/
							unsigned longint;										/* Offset to next plane	*/
							unsigned longint;										/* Offset to color table*/
							fill long;												/* Reserved				*/
							/* Color Table */
							unsigned hex longint;									/* ctSeed				*/
							integer;												/* ctFlags				*/
							integer = $$Countof(ColorSpec) - 1;						/* ctSize				*/
							wide array ColorSpec {
									integer;										/* value				*/
									unsigned integer;								/* RGB:	red				*/
									unsigned integer;								/*		green			*/
									unsigned integer;								/*		blue			*/
							};
							Rect;						/* srcRect */
							Rect;						/* destRect */
							integer		srcCopy, srcOr, srcXor, srcBic,	/* mode */
										notSrcCopy, notSrcOr, notSrcXor,
										notSrcBic, patCopy, patOr, patXor,
										patBic, notPatCopy, notPatOr, notPatBic,
										blend = 32, addPin, addOver, subPin,
										adMax = 37, subOver, adMin;
	packBitsRgnStart2:		integer = (packBitsRgnEnd2[$$ArrayIndex(OpCodes)] - packBitsRgnStart2[$$ArrayIndex(OpCodes)]) >> 3;
							Rect;				/* region's enclosing rect */
							hex string [$$Word(packBitsRgnStart2[$$ArrayIndex(OpCodes)]) - 10];
	packBitsRgnEnd2:		hex string [$$PackedSize(packBitsRgnEnd2[$$ArrayIndex(OpCodes)],
										$$BitField(PackBitsRgnpMapRowBytes[$$ArrayIndex(OpCodes)],0,13),
										($$BitField(PackBitsRgnBounds2[$$ArrayIndex(OpCodes)],32,16)
										- $$BitField(PackBitsRgnBounds2[$$ArrayIndex(OpCodes)],0,16)))];

						case AppleReserved009A:
							key integer = $009A;
	AppleReserved009AStart:	unsigned integer = (AppleReserved009AEnd[$$ArrayIndex(OpCodes)] - AppleReserved009AStart[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved009AStart[$$ArrayIndex(OpCodes)])];
	AppleReserved009AEnd:
						case AppleReserved009B:
							key integer = $009B;
	AppleReserved009BStart:	unsigned integer = (AppleReserved009BEnd[$$ArrayIndex(OpCodes)] - AppleReserved009BStart[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved009BStart[$$ArrayIndex(OpCodes)])];
	AppleReserved009BEnd:
						case AppleReserved009C:
							key integer = $009C;
	AppleReserved009CStart:	unsigned integer = (AppleReserved009CEnd[$$ArrayIndex(OpCodes)] - AppleReserved009CStart[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved009CStart[$$ArrayIndex(OpCodes)])];
	AppleReserved009CEnd:
						case AppleReserved009D:
							key integer = $009D;
	AppleReserved009DStart:	unsigned integer = (AppleReserved009DEnd[$$ArrayIndex(OpCodes)] - AppleReserved009DStart[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved009DStart[$$ArrayIndex(OpCodes)])];
	AppleReserved009DEnd:
						case AppleReserved009E:
							key integer = $009E;
	AppleReserved009EStart:	unsigned integer = (AppleReserved009EEnd[$$ArrayIndex(OpCodes)] - AppleReserved009EStart[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved009EStart[$$ArrayIndex(OpCodes)])];
	AppleReserved009EEnd:
						case AppleReserved009F:
							key integer = $009F;
	AppleReserved009FStart:	unsigned integer = (AppleReserved009FEnd[$$ArrayIndex(OpCodes)] - AppleReserved009FStart[$$ArrayIndex(OpCodes)]) >> 3 - 2;
							hex string [$$Word(AppleReserved009FStart[$$ArrayIndex(OpCodes)])];
	AppleReserved009FEnd:
						case ShortComment:
							key integer = $00A0;
							integer;							/* comment kind */
						
						case LongComment:
							key integer = $00A1;
							integer;							/* comment kind */
	longCommentStart2:		integer = (longCommentEnd2[$$ArrayIndex(OpCodes)] - longCommentStart2[$$ArrayIndex(OpCodes)] - 16) >> 3;
							hex string [$$Word(longCommentStart2[$$ArrayIndex(OpCodes)])];
	longCommentEnd2:
						case OpEndPic:
							key integer = $00FF;
						
						case HeaderOp:
							key integer = $0C00;
							longint		Version2 = -1;
							/* Fixed point bounding box */
							unsigned hex longint;
							unsigned hex longint;
							unsigned hex longint;
							unsigned hex longint;
							longint		Version2 = -1;		/* Reserved */
						
					};
					align word;
				};
		};
	endOfPict:
};

#endif __PICT.R__
