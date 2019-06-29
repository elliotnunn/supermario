/*
	File:		PictUtil.h

	Contains:	C-Style headers for the picture utilities package

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

*/


#ifndef __PICTUTIL__
#define __PICTUTIL__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __PALETTES__
#include <Palettes.h>
#endif

/*----------------------------------------------------------------------------------------------------------*/

/* verbs for the GetPictInfo, GetPixMapInfo, and NewPictInfo calls */

#define returnColorTable			( (short) 0x0001 )
#define	returnPalette				( (short) 0x0002 )
#define	recordComments				( (short) 0x0004 )
#define	recordFontInfo				( (short) 0x0008 )
#define	suppressBlackAndWhite		( (short) 0x0010 )


/* color pick methods */

#define systemMethod				0
#define	popularMethod				1
#define medianMethod				2


/* color bank types */

#define ColorBankIsCustom			-1
#define ColorBankIsExactAnd555		0
#define ColorBankIs555				1


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Data Structures
*/

struct CommentSpec {

	short			count;						/* number of occurrances of this comment ID */
	short			ID;							/* ID for the comment in the picture */

};
typedef	struct CommentSpec CommentSpec;
typedef	CommentSpec *CommentSpecPtr, **CommentSpecHandle;


struct FontSpec {

	short			pictFontID;					/* ID of the font in the picture */
	short			sysFontID;					/* ID of the same font in the current system file */
	long			size[4];					/* bit array of all the sizes found (1..127) (bit 0 means > 127) */
	short			style;						/* combined style of all occurrances of the font */
	long			nameOffset;					/* offset into the fontNamesHdl handle for the font’s name */

};
typedef	struct FontSpec FontSpec;
typedef	FontSpec *FontSpecPtr, **FontSpecHandle;



struct  PictInfo {

	short				version;				/* this is always zero, for now */
	long				uniqueColors;			/* the number of actual colors in the picture(s)/pixmap(s) */
	PaletteHandle		thePalette;				/* handle to the palette information */
	CTabHandle			theColorTable;			/* handle to the color table */
	Fixed				hRes;					/* maximum horizontal resolution for all the pixmaps */
	Fixed				vRes;					/* maximum vertical resolution for all the pixmaps */
	short				depth;					/* maximum depth for all the pixmaps (in the picture) */
	Rect				sourceRect;				/* the picture frame rectangle (this contains the entire picture) */
	long				textCount;				/* total number of text strings in the picture */
	long				lineCount;				/* total number of lines in the picture */
	long				rectCount;				/* total number of rectangles in the picture */
	long				rRectCount;				/* total number of round rectangles in the picture */
	long				ovalCount;				/* total number of ovals in the picture */
	long				arcCount;				/* total number of arcs in the picture */
	long				polyCount;				/* total number of polygons in the picture */
	long				regionCount;			/* total number of regions in the picture */
	long				bitMapCount;			/* total number of bitmaps in the picture */
	long				pixMapCount;			/* total number of pixmaps in the picture */
	long				commentCount;			/* total number of comments in the picture */
	long				uniqueComments;			/* the number of unique comments in the picture */
	CommentSpecHandle	commentHandle;			/* handle to all the comment information */
	long				uniqueFonts;			/* the number of unique fonts in the picture */
	FontSpecHandle		fontHandle;				/* handle to the FontSpec information */
	Handle				fontNamesHandle;		/* handle to the font names */
	long				reserved1;
	long				reserved2;

};
typedef	struct PictInfo PictInfo;
typedef	PictInfo *PictInfoPtr,**PictInfoHandle;

typedef	long PictInfoID;


#ifdef __cplusplus
extern "C" {
#endif

pascal OSErr GetPictInfo		(	PicHandle		thePictHandle, 
									PictInfo		*thePictInfo,
									short			verb,
									short			colorsRequested,
									short			colorPickMethod,
									short			version
								) = { 0x303C, 0x0800, 0xA831 };


pascal OSErr GetPixMapInfo		(	PixMapHandle	thePixMapHandle, 
									PictInfo		*thePictInfo,
									short			verb,
									short			colorsRequested,
									short			colorPickMethod,
									short			version
								) = { 0x303C, 0x0801, 0xA831 };


pascal OSErr NewPictInfo		(	PictInfoID		*thePictInfoID,
									short			verb,
									short			colorsRequested,
									short			colorPickMethod,
									short			version
								) = { 0x303C, 0x0602, 0xA831 };


pascal OSErr RecordPictInfo		(	PictInfoID		thePictInfoID,
									PicHandle		thePictHandle
								) = { 0x303C, 0x0403, 0xA831 };


pascal OSErr RecordPixMapInfo	(	PictInfoID		thePictInfoID,
									PixMapHandle	thePixMapHandle
								) = { 0x303C, 0x0404, 0xA831 };


pascal OSErr RetrievePictInfo	(	PictInfoID		thePictInfoID,
									PictInfo		*thePictInfo,
									short			colorsRequested
								) = { 0x303C, 0x0505, 0xA831 };


pascal OSErr DisposPictInfo		(	PictInfoID		thePictInfoID
								) = { 0x303C, 0x0206, 0xA831 };


#ifdef __cplusplus
}
#endif

#endif
