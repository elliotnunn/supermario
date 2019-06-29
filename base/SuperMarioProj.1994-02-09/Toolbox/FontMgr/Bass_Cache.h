/*
	File:		Bass_Cache.h

	Contains: 	This is the main define file for the Bass caching mechanism.

	Written by:	Charlton E. Lui

	Date:		May 8, 1988

	Revision:	1.0

	Copyright:	© 1988-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/28/92	DCL		Fixed enums with commas before the closing brace. MPW now
									complains.
		 <8>	  5/4/92	DTY		There really was a good reason for me to stick fondCache in the
									middle of SplineKey, honest! But since I don’t remember what it
									is, I’m moving it to the end of the structure to keep existing
									offsets the same.
		 <7>	11/15/91	DTY		Remember which resource map a font was loaded from.
		 <6>	10/11/91	DTY		Building the candidate FOND list takes too long. Cache two lists
									in SplineKey.
		 <5>	 9/25/91	jlf		Rolled in modifications from Pacific TrueType project:
		 							Added prototypes for routines used by the partial font,
									disk caching, and international support routines. Added
									typedef for the memory manager purge procedure. Added
									fSplineDirectory field to the WidthTable structure - the
									spline directory of a TrueType font is now appended to it's
									global width table for partial font support. Added
									fEncodingTablePointer and fScriptNumber fields to the
									sb_CacheHead structure for international support.
		 <4>	 5/23/91	dba		take out second copy of ZoneHandleOptions
		 <3>	  1/9/91	RB		(CEL/PKE/GM/DA) Added in safeCount variable to splineKey globals
									to fix the long string bug caused in stdtxmeas overflow.
		 <2>	10/22/90	CL		(BKR) Adding support for minimum memory mode to band and run in
									low cache conditions.
	    <20>	 10/9/90	CL		Added support for preflight routine.
		<19>	 10/8/90	CL		Adding loadSpline flag to the widthTable
	    <18>	 10/1/90	CL		Adding in memoryPatch vector for memory manager fix on the
									growzone.
		<17>	 9/27/90	CL		Made INITIAL_CHAR_COUNT start out smaller and increment cache
									increases by a smaller amount.
	    <16>	 9/24/90	CL		Added FM data to the cacheheader.
	    <15>	 9/20/90	CL		Adding shrink options for the memory calls.
	   	<14>	 9/16/90	CL		Cleaned up splinekeyRec… Got rid of disk cache defines.
	   								Added in heap defines into splineKey.  Added mfExists flag.
									Took out old prototypes.
	    <13>	 8/29/90	CL		Added cachezone to splinekey
		<12>	 8/28/90	MR		Add bitmapzone to splinekey
		<11>	 8/20/90	MR		Change CacheListHandle to handle of CacheReference {Handle and
									zone} and add fsWorkZone. Both changes record the zone a
									multiheap object lives in.
		<10>	  8/1/90	gbm		changes here to fix warnings in Bass_Cache.c
		 <9>	 7/18/90	MR		Conditionalize typedef of transformMatrix and remove
									redeclaration of FMOutput
		 <8>	 7/16/90	CL		Adding a vector for after cleaning up the cache.
		 <7>	 6/29/90	CL		Doing work…
		 <7>	 6/20/90	CL		Added INCREASE_CACHE constant for the caching code.
		 <6>	 5/29/90	CL		Took out cache references from the width table.
		 <5>	  5/3/90	CL		Added new support for new memory area for scan converter.
		 							and encryption.
		 <4>	 4/10/90	CL		Added in support for double byte codes. Work on disk caching.
									Fixed retrieval masks.
		 <3>	 3/20/90	CL		Adding disk cache… Support for tag format
		 <2>	 2/27/90	CL		Added in lots of support for optimizations of DrText.
	   <1.7>	11/15/89	CEL		Moved WidthTabTempHandle up.
	   <1.6>	11/14/89	CEL		Added device metrics. Added squash capabilities.
	   <1.5>	 9/25/89	CEL		Added in boolean param for sb_FillWidthTab.
	   <1.4>	 9/15/89	CEL		Took out the tempAscent and tempDescent vars in splinekey.
	   <1.3>	 8/28/89	CEL		Using transform matrix in widthtable and splinekeyrec instead of
									numer and denom.
	   <1.2>	 8/14/89	CEL		Added some vars for mfexists & machinetype. These are used for
									mem checks. Added in extra handle for temp memory that is not in
									system heap. Updated widthtable to include encryption var.
	   <1.1>	  8/1/89	CEL		Fixed EASE’s “C” comment brackets. On check in they were wrong!
	   <1.0>	  8/1/89	CEL		Adding into Bass build for the first time…
	   <1.5>	 6/12/89	CEL		Re-Structured sp_Glyph data. Got rid of some unnecessary
									constants.
	   <1.4>	 6/12/89	CEL		Needed extra check for sfnt state (face)…
	   <1.3>	 5/26/89	CEL		Integrated the new Font Scaler 1.0 into Spline Fonts
	   <1.2>	  5/3/89	CEL		Comment brackets were previously messed up from override
	  <•1.1>	  5/3/89	CEL		All kinks worked out, Blasting in bass files…
	   <1.0>	  5/2/89	CEL		Rolling in Bass for first time…

	To Do:
*/

/*—————————————————————————————
	Expandmem Define
*/
#define EXPAND_MEM_PTR (*(long *) 0x02B6)
#define SPLINE_KEY_HAND (*(sb_SplineKeyHdl **) ((char *)EXPAND_MEM_PTR + 0x0068))

/*—————————————————————————————
	WidthListHandle
*/
#define FMOUTPUT_PTR 		(*(Ptr *) 0x0998)
#define WIDTH_LISTHANDLE 	(*(Handle **) 0x08E4)
#define WIDTH_TABNUM		12			/* Currently there are 12 width tables.  If this should */
										/* change then change this also */
#define LASTSPEXTRA 		0x0B4C
#define HILITEMODE 			0x0938
#define PRESERVE_GLYPH		0x10

/*—————————————————————————————
	Resource Manager low mem
*/
#define TopMapHndl 			(*(long *) 0x0A50)			/* topmost map in list [handle] */
#define SysMapHndl 			(*(long *) 0x0A54)			/* system map [handle] */
#define SysMap 				(*(short *) 0x0A58)			/* reference number of system map [word] */
#define CurMap				(*(short *) 0x0A5A)			/* reference number of current map [word] */
#define UPDATE_RESOURCE_COUNT	20						/* Write resource when number of new entries equal */

/*—————————————————————————————
	CDEFS
*/
#ifndef int16
#define int16 short
#endif

#ifndef uint16
#define uint16 unsigned short
#endif

#ifndef int32
#define int32 int
#endif

#ifndef uint32
#define uint32 unsigned long
#endif

#ifndef Fixed
#define Fixed long
#endif

/*—————————————————————————————
	MACROS
*/
#define MAKE_EVEN( n ) if ( n & 1 ) n++
#define GetByte( p ) *(((char  *)p)++)
#define GetAlignedWord( p ) *(((short *)p)++)

/*—————————————————————————————
	Transform struct
*/
#define OBLIQUE_BIT		2
#define OUTLINE_BIT		8
#define SHADOW_BIT		16
#define HEIGHT			3
#define WIDTH			3

#ifndef transformMatrix
typedef struct {
	Fixed		transform[HEIGHT][WIDTH];
} transformMatrix;
#endif

#define POINTSPERINCH				72
#define	INITIAL_BITMAP_SIZE			256
#define MULTI_ZONE_FLAG				0xFFFFFFFF

/*—————————————————————————————
	CACHE DEFINES
*/
#define MAX_PPEM_CACHED				127
#define CONTOUR_ENTRY_BIT			0x80000000
#define	ERROR_ENTRY_BIT				0xC0000000
#define ENTRY_MASK					0x3FFFFFFF

/*—————————————————————————————
	NEW CACHE
*/
#define INITIAL_CACHES				5			/* Amount of allowed caches at once */
#define GROW_CACHE_NUM				5			/* Amount of allowed caches at once */
#define INITIAL_CHAR_COUNT			10			/* Amount of initial characters to alloc for */
#define ADD_CHAR_NUM				10			/* Add the character numbers */
#define INCREASE_CACHE				256			/* Amount to increase cache */
#define GLYPH_COUNT					256			/* Number of possible entries into cache */
#define AVG_CONTOUR_SIZE			400			/* Say 600 bytes for average contour */
#define EMPTY						0			/* Empty cell */

#define	ADD_ENTRY_COUNT				128			/* Number of entries to add to cache */
#define	SKIP_ENTRY_COUNT			1			/* Number of entries to skip on collision */
#define	FULL_ENTRY_PERCENT			60			/* Percentage of entries used in full table */



/*—————————————————————————————————————————————————————————————————————————————————————————
	$$$ CHANGE ASSEMBLY IF BELOW IS MODIFIED $$$
	For QuickDraw - DrawText.a
*/
typedef struct {
	uint16		StrikeID;						/* character to blit */
	uint16		glyphID;						/* Code of Glyph */
	uint16		ptSize;							/* Size of character */
	uint16		destDepth;						/* Depth of device */
	Boolean		clipVert;						/* Vertical character clip flag */
	Boolean		clipHorz;						/* Horizontal character clip flag */
	int16		botClip;						/* bottom clip off baseline */
	int16		topClip;						/* top clip off baseline */
	int16		yMin;							/* Y min off baseline */
	int16		yMax;							/* Y max off baseline */
	Fixed		lsb;							/* Left side bearing */
	short		devLSB;							/* Device Left side bearing */
	int16		scan;							/* # of scan lines */
	int16		bandScan;						/* size of band for next band */
	int16		nextBotBand;					/* next bot character band */
	int16		nextTopBand;					/* next top character band */
	Boolean		nextBand;						/* Another character band? */
	Boolean		FILLBYTE;						/* */
	long		entryOffset;					/* entry offset */
	uint16		bitWidth;						/* glyph bit width */
	uint32		byteWidth;						/* row bytes of character */
	uint32		*bitMapPtr;						/* Points to the BitMap */
	uint32		bufStart;						/* Start of destination bits buffer */
}	sb_Glyph;

/*—————————————————————————————
	$$$ UPDATE THE ASSEMBLY WHEN CHANGING THIS $$$
	BitMap Header
*/
typedef struct	{
	short			yMin;						/* Y min off baseline to place BitMap */
	short			yMax;						/* Y max off baseline */
	long			lsb;						/* Left side bearing */
	short			devLSB;						/* Device left side bearing */
	short			adjustTop;					/* Adjust this char from ascent */
	short			rError;						/* Error in case of huge size || bad font */
	short			bitWidth;					/* Width of char bits */
	long			byteWidth;					/* number of byte width */
	short			xMin;						/* Save xMin and also use to calc xMax */
	short			scan;						/* # of scan lines */
	short			scanByteWidth;				/* actual need byteWidth for scan converter */
	uint16			glyphID;					/* id of glyph */

	/** DO NOT PLACE ANYTHING AT END, THIS IS DATA TERRITORY **/
	long			cacheData[];				/* Data starts here */
} sb_GlyphData;
#define GLYPH_HEAD_BYTES	((sizeof(sb_GlyphData) + 3) & ~3)

/*—————————————————————————————
	$$$ UPDATE THE ASSEMBLY WHEN CHANGING THIS $$$
	CACHE HEADER
*/
typedef struct	{
	/* Match the font with these */
	short 				StrikeID;				/* id of sfnt */
	short 				ptSize;					/* point size */
	short 				face;					/* style of request */
	short 				newEntries;				/* Number of new entries added */
	long				trans00;				/* transform for x */
	long				trans11;				/* transform for y */
	long				trans10;				/* transform for skew */

	Fixed				ascent;					/* Ascent of the font */
	Fixed				descent;				/* Descent of the font */
	Fixed				widMax;					/* Widmax of the Font */
	Fixed				leading;				/* Leading of the Font */
	Fixed				lOverHMax;				/* Left over hang max */
	Fixed				rOverHMax;				/* Right over hang max */
	Fixed				yMax;					/* YMax of the font */
	Fixed				yMin;					/* YMin of the font */
	short				height;					/* Height of the font */
	short				filler;					/*  */
	Fixed				rightItalicSlop;		/* Right Italic slop */
	Fixed				leftItalicSlop;			/* Left italic slop */

	unsigned char 		fmAscent;				/* FMOutput ascent */
	unsigned char 		fmDescent;				/* FMOutput descent */
	unsigned char 		fmWidMax;				/* FMOutput widmax */
	char 				fmLeading;				/* FMOutput leading */

	long				addEntryOffset;			/* Offset to next spot to add */
	char				bitsCached;				/* Set if bitmaps are cached */
	char				squashed;				/* A squashed cached if true */
	short				familyID;				/* Family id for international checks */
	
	Ptr                 fEncodingTablePointer;  /* pointer to double byte encoding table or nil for single byte scripts */
	uint16              fScriptNumber;          /* script associated with this cache */
	uint16				fLongWordAlign;			/* long word align */	
	uint32				glyphArray[];			/* Array of offsets starts here */
} sb_CacheHead, *sb_CacheHeadPtr, **sb_CacheHeadHdl;



/*—————————————————————————————————————————————————————————————————————————————————————
								WIDTH TABLE
*/
typedef struct {
	Fixed 			tabData[256];
	Handle 			tabFont;
	long 			sExtra;
	Fixed 			style;
	short 			fID;
	short 			fSize;
	short 			face;
	short 			device;
	Point 			inNumer;
	Point 			inDenom;
	short 			aFID;
	Handle 			fHand;
	unsigned char	usedFam;
	unsigned char 	aFace;
	short 			vOutput;
	short 			hOutput;
	short 			vFactor;
	short 			hFactor;
	short 			aSize;

	short			NFNT;
	short			StrikeID;
	short			Hint;
	short			devRes;

	Boolean			IsSpline;
	Boolean			notFast;				/* flag of whats cached */
	short			badFontFlag;			/* If the preprogram is corrupt use the missing character */
	Boolean			loadSpline;				/* set if loading splines */
	Boolean			FREEBYTE;				/*  */

	long			trans00;				/* transform for x */
	long			trans11;				/* transform for y */
	long			trans10;				/* transform for skew */
	short			fKeepSplineOnDisk;		/* non-zero means don't load the spline into memory */
	short			fSplineDirectory[];		/* copy of spline directory is appended here */
	
} widthTable, *widthTablePtr, **widthTableHdl;

/*——————————————————————————————————————————————————————————————————————————————————————————————————
							Font Scaler Storage
*/
typedef struct {
	long				xOffset, yOffset, startPointOffset, endPointOffset, onCurveOffset;
	long				*workSpacePtr;		/* workSpace that will work on */
	short				contours;			/* number of contours */
} splineData;

typedef struct {
	Handle	cache;
	THz		zone;
} CacheReference;

typedef enum {
	kNoOptions		= 0,
	kKeepContents	= 1
} ZoneHandleOptions;

typedef enum {
	noShrink		= 0,
	doShrink		= 1
} ShrinkOptions;


typedef enum {
	useApp		= 0,
	useSys		= 1
} whichHeap;

typedef pascal void (*PurgeProcPtr)( Handle purgeableHandle );

/*——————————————————————————————
	$$$ WARNING UPDATE. make parallel changes in fontPrivate.a $$$
	Memory off Spline Low-Mem
*/
typedef struct {
	long						callDebug;					/* Drop into debugger if set and error */
	Ptr							vFontCache;					/* Dispatch table for QuickDraw caching */
	Ptr							vFontScaler;				/* Dispatch table for Font scaling routines */
	Handle						bitmapHand;					/* Bitmap Handle - Number 5 in Font Scaler */
	THz							bitmapZone;					/* Zone the bitmapHand is allocated in */
	CacheReference**			cacheListHandle;			/* List of Cache Handles */
	sb_CacheHeadHdl				cacheHand;					/* Global cache hand */
	THz							cacheZone;					/* zone for the current cache */
	short						cachePlace;					/* spot where we got the cache */
	short						fontCount;					/* Set to Zero on increment each time a new cache exists */
	THz							curAppZone;					/* current application zone */
	Boolean						sysFull;					/* System is full */
	Boolean						appFull;					/* App is full */
	char						useHeap;					/* which heap to use */
	Boolean						mfExists;					/* flags whether multiFinder exists */
	short						glyphID;					/* last glyph id */
	Boolean						is32Bit;					/* Does 32bit QD exist */
	Boolean						squashed;					/* Current font is squashed */
	short						StrikeID;					/* check if still same sfnt */
	short						ptSize;						/* Check Point Size the same */
	short						device;						/* check Device resolution the same */
	short						face;						/* check Face the same */
	long						trans00;					/* transform for x */
	long						trans11;					/* transform for y */
	long						trans10;					/* transform for skew */
	widthTableHdl				tempWidthTabHand;			/* Used temporarily to get printer widths */
	Ptr							vGrowZone;					/* vector for growzone patch */
	short						safeCount;					/* safe count in case of length overflow */
	short						EXTRAWORD;
															/* ————FONT SCALER MEMORY———— */
	Handle						fsWorkHand;					/* work area for spline data */
	THz							fsWorkZone;					/* heap this guy lives in */
	fs_GlyphInputType 			*inPtr;						/* pointer to input struct */
	fs_GlyphInfoType  			*outPtr;					/* pointer to info struct */
	long						inputOffset;				/* Constant memory */
	long						infoOffset;					/* Constant memory */
	long						globalOffset0;				/* Constant memory */
	long						globalOffset1;				/* Constant memory */
	long						globalOffset2;				/* Constant memory */
	long						sfntDataOffset3;			/* Variable size memory */
	long						sfntDataOffset4;			/* Variable size memory */
	long						rowArrayOffset6;			/* Variable size memory */
	long						columnArrayOffset7;			/* Variable size memory */
	THz							fPartialFontZone;			/* Zone for partial font fragments */
	FragmentTableCacheStructureHandle fFragmentTableCache;	/* list of fragment tables  */
	short						fLastSwapFontFamilyID;		/* FMInput family ide passed in at last SwapFont call */
	Handle						fDiskCacheMap;					/* handle to disk cache map */
	PurgeProcPtr				fResourceManagerPurgeProcedure; /* saved resource manager purge procedure */
	Boolean						fInPurgeProcedure;			/* true if currently in purge procedure */
	Boolean						fFontCacheExists;			/* true if the font cache exists */
	short						lastFontMap;				/* Reference number of resource map last font was fetched from */
	Handle						fondCache[2];				/* Cache for candidate FOND list */
	
} sb_SplineKey, *sb_SplineKeyPtr, **sb_SplineKeyHdl;

/*——————————————————————————————————————————————————————————————————————————————————————————
									Interfaces
*/
#define SB_KILLSOMECACHES			0x0002
#define SB_FILLPRINTER_WIDTHS		0x0003
#define	SB_RETRIEVE_GLYPH			0x0004
#define	SB_FILL_WIDTHTAB			0x0005
#define	SB_SFNTMatchSIZE			0x000d

/*——————————————————————————————————————————————————————————————————————————————————————————
								Outside system client calls
*/
/* Private calls */
extern pascal long FillPrinterWidths(Fixed *widthsArrayPtr, Handle sfntHand)
	= {0x7000 + SB_FILLPRINTER_WIDTHS,0xA854};

extern pascal long KillSomeCaches( unsigned long startAddr, unsigned long endAddr )
	= {0x7000 + SB_KILLSOMECACHES,0xA854};

extern pascal long sb__RetrieveGlyph( widthTableHdl widthTabHand, register sb_Glyph *glyphPtr )
	= {0x303C,SB_RETRIEVE_GLYPH,0xA854};

extern pascal long sb__FillWidthTab(widthTableHdl widthTabHand, Boolean fractEnable, Boolean needWidths)
	= {0x303C,SB_FILL_WIDTHTAB,0xA854};

extern pascal Boolean sb__SFNTMatchSize(Handle sfntHand, short matchSize)
	= {0x303C,SB_SFNTMatchSIZE,0xA854};


long		sb_ShrinkCacheToMin(sb_CacheHeadHdl cacheHand, long addEntryOffset);
Handle		sb_SizeTheBlock(sb_SplineKeyHdl splineKey, Handle inputH, THz* inputZone, long inputSize, ZoneHandleOptions options, short cachePlace, ShrinkOptions shrinkTest);
void		sb_DisposeZoneHandle(Handle h, THz zonePtr);
Handle		sb_NewZoneHandle(sb_SplineKeyHdl splineKey, long size, THz* zone);
short		IntrinsicStyle(widthTablePtr w);
