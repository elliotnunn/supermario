/*
	File:		FSglue.h

	Contains:	xxx put contents here (or delete the whole line) xxx

	Written by:	xxx put name of writer here (or delete the whole line) xxx

	Copyright:	© 1988-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<10>	10/22/91	RB		Add support for sfnt-embedded-bitmaps
		 <9>	 9/25/91	jlf		Moved sfnt_PackedSplineFormat from sfnt.c to this file and
		 							rolled in mr's SOFTRELEASESFNTFRAG modifications.
		 <8>	12/11/90	MR		Add use-my-metrics support for devMetrics in component glyphs.
									[rb]
		 <7>	 12/5/90	MR		Remove unneeded leftSideBearing and advanceWidth fields. [rb]
		 <6>	 12/5/90	RB		Change reverseContours to unused1, since we don't use it, since
									the scan converter now uses non-zero winding number fill. [mr]
		 <5>	11/27/90	MR		Need two scalars: one for (possibly rounded) outlines and cvt,
									and one (always fractional) metrics. [rb]
		 <4>	 11/5/90	MR		Add Release macro
		 <3>	10/31/90	MR		Add fontFlags field to key (copy of header.flag) [rb]
		 <2>	10/20/90	MR		Change to new scaling routines/parameters, removed scaleFunc
									from key. [rb]
		<12>	 7/18/90	MR		Change error return type to int
		<11>	 7/13/90	MR		Declared function pointer prototypes, Debug fields for runtime
									range checking
		 <8>	 6/21/90	MR		Add field for ReleaseSfntFrag
		 <7>	  6/5/90	MR		remove vectorMappingF
		 <6>	  6/4/90	MR		Remove MVT
		 <5>	  6/1/90	MR		Thus endeth the too-brief life of the MVT...
		 <4>	  5/3/90	RB		adding support for new scan converter and decryption.
		 <3>	 3/20/90	CL		Added function pointer for vector mapping
		 							Removed devRes field
									Added fpem field
		 <2>	 2/27/90	CL		Change: The scaler handles both the old and new format
									simultaneously! It reconfigures itself during runtime !  Changed
									transformed width calculation.  Fixed transformed component bug.
	   <3.1>	11/14/89	CEL		Left Side Bearing should work right for any transformation. The
									phantom points are in, even for components in a composite glyph.
									They should also work for transformations. Device metric are
									passed out in the output data structure. This should also work
									with transformations. Another leftsidebearing along the advance
									width vector is also passed out. whatever the metrics are for
									the component at it's level. Instructions are legal in
									components. Now it is legal to pass in zero as the address of
									memory when a piece of the sfnt is requested by the scaler. If
									this happens the scaler will simply exit with an error code !
	   <3.0>	 8/28/89	sjk		Cleanup and one transformation bugfix
	   <2.2>	 8/14/89	sjk		1 point contours now OK
	   <2.1>	  8/8/89	sjk		Improved encryption handling
	   <2.0>	  8/2/89	sjk		Just fixed EASE comment
	   <1.5>	  8/1/89	sjk		Added composites and encryption. Plus some enhancements…
	   <1.4>	 6/13/89	SJK		Comment
	   <1.3>	  6/2/89	CEL		16.16 scaling of metrics, minimum recommended ppem, point size 0
									bug, correct transformed integralized ppem behavior, pretty much
									so
	   <1.2>	 5/26/89	CEL		EASE messed up on “c” comments
	  <•1.1>	 5/26/89	CEL		Integrated the new Font Scaler 1.0 into Spline Fonts
	   <1.0>	 5/25/89	CEL		Integrated 1.0 Font scaler into Bass code for the first time…

	To Do:
*/
/*		<3+>	 3/20/90	mrr		Added flag executeFontPgm, set in fs_NewSFNT
*/
#define POINTSPERINCH				72
#define MAX_ELEMENTS				2
#define MAX_TWILIGHT_CONTOURS		1

#define TWILIGHTZONE 0 /* The point storage */
#define GLYPHELEMENT 1 /* The actual glyph */



/* use the lower ones for public phantom points */
/* public phantom points start here */
#define LEFTSIDEBEARING 0
#define RIGHTSIDEBEARING 1
/* private phantom points start here */
#define ORIGINPOINT 2
#define LEFTEDGEPOINT 3
/* total number of phantom points */
#define PHANTOMCOUNT 4


/*** Memory shared between all fonts and sizes and transformations ***/
#define KEY_PTR_BASE				0 /* Constant Size ! */
#define VOID_FUNC_PTR_BASE			1 /* Constant Size ! */
#define SCAN_PTR_BASE				2 /* Constant Size ! */
#define WORK_SPACE_BASE				3 /* size is sfnt dependent, can't be shared between grid-fitting and scan-conversion */
/*** Memory that can not be shared between fonts and different sizes, can not dissappear after InitPreProgram() ***/
#define PRIVATE_FONT_SPACE_BASE		4 /* size is sfnt dependent */
/* Only needs to exist when ContourScan is called, and it can be shared */
#define BITMAP_PTR_1				5 /* the bitmap - size is glyph size dependent */
#define BITMAP_PTR_2				6 /* size is proportional to number of rows */
#define BITMAP_PTR_3				7 /* used for dropout control - glyph size dependent */
#define MAX_MEMORY_AREAS			8 /* this index is not used for memory */

#ifdef RELEASE_MEM_FRAG
	#define RELEASESFNTFRAG(key,data)	(key)->ReleaseSfntFrag((key)->clientID, (void*)(data))
	#define SOFTRELEASESFNTFRAG(key,data)	if (data) (key)->ReleaseSfntFrag((key)->clientID, (void*)(data))
#else
	#define RELEASESFNTFRAG(key,data)
	#define SOFTRELEASESFNTFRAG(key,data)
#endif


typedef struct {
	Fixed x;
	Fixed y;
} point;

/*** Offset table ***/
typedef struct {
	int32			interpreterFlagsOffset;
	int32			startPointOffset;
	int32			endPointOffset;
	int32			oldXOffset;
	int32			oldYOffset;
	int32			scaledXOffset;
	int32			scaledYOffset;
	int32			newXOffset;
	int32			newYOffset;
	int32			onCurveOffset;
	int32			nextOffset; /* New for components */
} fsg_OffsetInfo;


/*  #define COMPSTUFF  */

/*** Element Information ***/
typedef struct {
	int32				missingCharInstructionOffset;
	int32				stackBaseOffset;
#ifdef COMPSTUFF
	fsg_OffsetInfo 		*offsets;
	fnt_ElementType  	*interpreterElements;
#else
	fsg_OffsetInfo		offsets[MAX_ELEMENTS];
	fnt_ElementType 	interpreterElements[MAX_ELEMENTS];
#endif COMPSTUFF
} fsg_ElementInfo;

typedef struct {
	Fixed xScale;
	Fixed yScale;
} fsg_transformationMagic;

/*** The Internal Key ***/
typedef struct fsg_SplineKey {
	int32				clientID;
	GetSFNTFunc			GetSfntFragmentPtr;	/* User function to eat sfnt */
	ReleaseSFNTFunc		ReleaseSfntFrag;	/* User function to relase sfnt */
	uint16				(*mappingF)(struct fsg_SplineKey*, uint16);			/* mapping function */
	int32				mappOffset;			/* Offset to platform mapping data */
	int16				glyphIndex;			/* */
	uint16				elementNumber;		/* Character Element */
	sfnt_OffsetTable*	sfntDirectory;		/* Points to sfnt */
	
	char**				memoryBases;		/* array of memory Areas */

	fsg_ElementInfo		elementInfoRec;		/* element info structure */
	sc_BitMapData		bitMapInfo;			/* bitmap info structure */

	uint16			numberOfBytesTaken;		/* from the character code */
	uint16			emResolution;			/* used to be int32 <4> */

	Fixed			fixedPointSize;			/* user point size */
	Fixed			interpScalar;			/* scalar for instructable things */
	Fixed			metricScalar;			/* scalar for metric things */

	transMatrix		currentTMatrix; 		/* Current Transform Matrix */
	transMatrix		localTMatrix; 			/* Local Transform Matrix */
	int8			localTIsIdentity;
	int8			phaseShift;				/* 45 degrees flag <4> */
	int8			identityTransformation;
	int8			unused1;				/* reverseContours; */
	int8			outlineIsCached;
	int8			pad0;

	uint16			fontFlags;				/* copy of header.flags */

	Fixed			pixelDiameter;
	uint16			nonScaledAW;
	int16			nonScaledLSB;
	
	int32			state;					/* for error checking purposes */
	int32			scanControl;			/* flags for dropout control etc.  */
	
	/* for key->memoryBases[PRIVATE_FONT_SPACE_BASE] */
	int32			offset_storage;
	int32			offset_functions;
	int32			offset_instrDefs;		/* <4> */
	int32			offset_controlValues;
	int32			offset_globalGS;
	int32			offset_inverseTable;
	int32			offset_memTable;
	int32			offset_preTable;
	int32			offset_offsets;
	int32			offset_interpreterElements;

	int32			glyphLength;
	
	/* copy of profile */
	sfnt_maxProfileTable	maxProfile;

#ifdef DEBUG
	int32	cvtCount;
#endif

	int16			offsetTableMap[sfnt_NUMTABLEINDEX];
	uint16			numberOf_LongHorMetrics;
	
	uint16			totalContours; /* for components */
	uint16			totalComponents; /* for components */
	uint16			weGotComponents; /* for components */
	uint16			compFlags;
	int16			arg1, arg2;
	point			devLSB, devRSB;
	
	fs_GlyphInfoType *outputPtr; 				/* So Cary can do his styles */
	void	 		(*styleFunc)(fs_GlyphInfoType*);
	
	fsg_transformationMagic tInfo;
	fsg_transformationMagic globalTInfo;
	
	int32			instructControl;	/* set to inhibit execution of instructions */	
	int32			imageState;			/* is glyph rotated, stretched, etc. */
	
	uint16			numberOfRealPointsInComponent;
	uint16			lastGlyph;
	uint8			executePrePgm;
	uint8			executeFontPgm;		/* <4> */
	uint8			useMyMetrics;
	uint8			padByte;
	jmp_buf			env;
	
#if TheFuture /* <10 > EBITMAP */						
	uint8		bitmapPreferences;	/* 0 = exactBM > outline > scaledBM; 1 = exactBM > scaledBM > outline;
									   2 = outline > exactBM > scaledBM; 3 = only outline - else error;
									   4 = only bitmap - else error; 	 5 = only exact bitmap - else error; */
	uint8		scalingPreference;	// 0 = scaledBM > metricsScaledBM;  1 = metricsScaledBM > scaledBM;
	uint8		usefulOutlines;		// based on 'glyf' format.(in 'head')  1 means this is bitmaps only.
	uint8		usefulBitmaps;		// based on existence of 'bloc' table
	uint8		desiredHorPpem;
	uint8		desiredVerPpem;
	uint16		desiredPointSize;
	uint8*		blocPointer;		// set by new_sfnt
	uint8		availMatchFont;		/* 0 no bitmap data, 1 scaled bitmap data,
									 * 2 exact bitmap data, 3 exact-except for pointsize */ 
	uint8		methodToBeUsedFont;	/* 0 = outlines; 1 = scaled bitmap; 2 = exact bitmap;
	 								 * 3 = exact bitmap except for pointsize; 4 = error */	
	uint8		bestHorPpemFont;	// beginning of size subtable for best bitmap data in font
	uint8		bestVerPpemFont;
	uint16		bestPointSizeFont;
	uint8		maxPixWidthFont;	// may not be needed - but components ?
	uint8		maxPixHeightFont;
	uint8		indexShiftFont;
	uint8		indexNbaseFont;
	uint8		indexFormat;		// 1 = longs, 2 = compressed short
	uint8		dataFormat;	
	uint32		indexOffsetFont;
	int8		horAscent; 		
	int8		horDescent;		
	int8		horLineGap; 		
	uint8		horAdvanceMax; 		
	int8		verAscent; 		
	int8		verDescent; 		
	int8		verLineGap; 		
	uint8		verAdvanceMax;		

	uint8		bestHorPpemGlyph;		//beginning of size subtable for best bitmap data for current glyph
	uint8		bestVerPpemGlyph;
	uint16		bestPointSizeGlyph;
	uint32		indexOffsetGlyph;
	uint32		offsetBitData;
	uint16		lengthBitData;
	uint8		indexShiftGlyph;
	uint8		indexNbaseGlyph;
	uint16*		indexPtrGlyph;
	uint8*		bitmapPtr;
	uint16		numberGlyphs;
	uint8		pixHeightSource;
	uint8		pixWidthSource;
	int8		hBearingXsource;
	int8		hBearingYsource;
	int16		hAdvanceSource;
	int8		vBearingXsource;
	int8		vBearingYsource;
	int16		vAdvanceSource;
	uint8		numAttachments;
	uint8		numComponents;
	uint8		methodToBeUsed;		/* 0 = outlines; 1 = scaled bitmap; 2 = exact bitmap;
	 								 * 3 = exact bitmap except for pointsize; 4 = error */	

#endif
} fsg_SplineKey;

#define VALID 0x1234

/* Change this if the format for cached outlines change. */
/* Someone might be caching old stuff for years on a disk */
#define OUTLINESTAMP 0xA1986688
#define OUTLINESTAMP2 0xA5


/* for the key->state field */
#define INITIALIZED 0x0001
#define NEWSFNT 	0x0002
#define NEWTRANS	0x0004
#define GOTINDEX	0x0008
#define GOTGLYPH	0x0010
#define SIZEKNOWN	0x0020

/* fo the key->imageState field */
#define ROTATED		0x400
#define STRETCHED 	0x1000

/**********************/
/** FOR MISSING CHAR **/
/**********************/
#define NPUSHB			0x40
#define MDAP_1			0x2f
#define MDRP_01101		0xcd
#define MDRP_11101		0xdd
#define IUP_0			0x30
#define IUP_1			0x31
#define SVTCA_0			0x00
/**********************/


/***************/
/** INTERFACE **/
/***************/
extern unsigned fsg_KeySize(void);
extern unsigned fsg_InterPreterDataSize(void);
extern unsigned fsg_ScanDataSize(void);
extern unsigned fsg_PrivateFontSpaceSize(fsg_SplineKey *key);
extern int fsg_GridFit(fsg_SplineKey *key, voidFunc traceFunc, boolean useHints);


/***************/

/* Private Data Types */
typedef struct {
	int16 xMin;
	int16 yMin;
	int16 xMax;
	int16 yMax;
} sfnt_BBox;

/** SFNT Packed format **/
typedef struct {
	int16 		numberContours;
	int16 		*endPoints;				/** vector: indexes into x[], y[] **/
	int16 		numberInstructions;
	uint8  		*instructions;			/** vector **/
	uint8 		*flags;					/** vector **/
	sfnt_BBox	bbox;
} sfnt_PackedSplineFormat;

/* matrix routines */

/*
 * ( x1 y1 1 ) = ( x0 y0 1 ) * matrix;
 */
extern void fsg_Dot6XYMul( F26Dot6* x, F26Dot6* y, transMatrix* matrix );
extern void fsg_FixXYMul( Fixed* x, Fixed* y, transMatrix* matrix );
extern void fsg_FixVectorMul( vectorType* v, transMatrix* matrix );

/*
 *   B = A * B;		<4>
 *
 *         | a  b  0  |
 *    B =  | c  d  0  | * B;
 *         | 0  0  1  |
 */
extern void fsg_MxConcat2x2(transMatrix* matrixA, transMatrix* matrixB);

/*
 * scales a matrix by sx and sy.
 *
 *              | sx 0  0  |
 *    matrix =  | 0  sy 0  | * matrix;
 *              | 0  0  1  |
 */
extern void fsg_MxScaleAB(Fixed sx, Fixed sy, transMatrix *matrixB);

extern void fsg_ReduceMatrix(fsg_SplineKey* key);

/*
 *	Used in FontScaler.c and MacExtra.c, lives in FontScaler.c
 */
int fsg_RunFontProgram( fsg_SplineKey* key, voidFunc traceFunc );


/* 
** Other externally called functions.  Prototype calls added on 4/5/90
*/
void fsg_IncrementElement(fsg_SplineKey *key, int32 n, register int32 numPoints, register int32 numContours);

void fsg_InitInterpreterTrans( register fsg_SplineKey *key  );

int fsg_InnerGridFit(register fsg_SplineKey *key, int16 useHints, voidFunc traceFunc, 
	sfnt_BBox *bbox, int32 sizeOfInstructions, uint8 *instructionPtr, int finalCompositePass);

int fsg_NewTransformation(register fsg_SplineKey *key, voidFunc traceFunc);

void fsg_SetUpElement(fsg_SplineKey *key, int32 n); 

unsigned fsg_WorkSpaceSetOffsets(fsg_SplineKey *key); 

int fsg_SetDefaults( fsg_SplineKey* key );

