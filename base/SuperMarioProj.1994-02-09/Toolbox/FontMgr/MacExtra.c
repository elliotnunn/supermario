/*
	File:		MacExtra.c

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<13>	 6/22/92	JH		#1033107 <AMW>: In Map256Chars, GetEncodingTable is called to
									get the script ID and then GetFontMapInfo is called to get the
									index to the cmap table. GetFontMapInfo uses the script ID
									returned by GetEncodingTable, which now returns the right script
									ID. However, with the old roman coded cmaps for Arabic using the
									right script ID means these fonts won't get a proper widthtable
									and will draw very, very poorly. To try and fix that if the
									script ID isn't Roman and GetFontMapInfo returned 0, we call
									GetFontMapInfo again using smRoman has the script ID the second
									time around.
		<12>	 9/25/91	jlf		Rolled in Pacific TrueType modifications.
									Removed all 'sfnt' functions at bottom of file; functions have
									been rewritten for partial font support. Modified fs_FillWidthTable:
									doesn't load entire hmtx into memory now (necessary for Pacific
									fonts); replaced references to sfnt_GetTablePtr with GetFontTablePtr;
									replaced references to RELEASESFNTFRAG with QDUngetFontFrag. Added
									doublebyte support to Map256Chars - doublebyte character widths are
									mapped to the width of the missing glyph.
		<11>	  4/4/91	KON		mrr, WRKSHT# RBN-BS-005:  Outline condensed text was producing
									negative width tables because styleExtra was negative.  The fix
									is if styleExtra is negative, pin the values in the width table
									to zero.
		<10>	12/15/90	PKE		(with KSM) Delete include of OutlineCalls.h, which is now
									obsolete since Darin moved everything in it into Fonts.h.
		 <9>	12/11/90	MR		Undo "intelligent" rounding of descent and leading. [rb]
		 <8>	 12/5/90	RB		Detect case where scaled ymax-ymin is greater than QD coordinate
									space. Return error.[CEL]
		 <7>	 12/5/90	MR		Device metrics now always fails if ppem > 255. Add SLOP to ymax
									and ymin so quickdraw doesn't clip tall or low glyphs. [rb]
		 <6>	11/27/90	MR		Be sure to use metricScalar for widthtable, corrected call to
									FillDeviceWidths, changed Ceiling to ROUND, and added fancy
									calculation for ascent and descent to improve accuracy. [rb]
		 <5>	11/13/90	MR		New behavior: Never scale styleExtra, simply add it to the
									(scaled) widths. [cl]
		 <4>	 11/5/90	MR		Look for device widths, even if the text is stretched. [rb]
		 <3>	10/30/90	CL		(MR)Adding pascal glue for lowestRecPPEM.
		 <2>	10/20/90	MR		Rev to use new math/scaling routines, simplify loops in
									FillWidthTable, merge needed routines in from FontTools.c. [rb]
		<20>	 8/22/90	MR		Change FillWidthTable to take sfnt handle and rsrc id and not
									take fsInfoType
		<19>	 8/20/90	MR		Add line-of-death to GetCheckSumAdj (which is now
									fs_GetSfntInfo)
		<18>	 8/10/90	MR		Rev. to new interface to ApplyFontMapping
		<17>	 7/23/90	MR		Moved fonttool calls to fonttools.c
		<16>	 7/19/90	dba		get rid of C warnings
		<15>	 7/18/90	MR		Add call to Font2Script
		<14>	 7/14/90	MR		undefined RELEASE_FRAG, changed DOUBLE_BYTE code, fillWidthTable
									now builds glyph array up front instead of inline, cleaned up
									parameters to charCode mapping functions in font toolbox
		<13>	 7/13/90	MR		Add GetFontGlyph, use union in FSInput
		<12>	 6/26/90	MR		Revise Toolbox calls, change script enums
		<11>	 6/21/90	MR		Handle suffixIndex == 0 in GetPSFontName, add calls to
									ReleaseSfntFrag
		<10>	 6/19/90	MR		Add GetPSFontName and fixed bug in GetFontStringInfo and
									GetFontMapInfo
		 <9>	  6/5/90	MR		Try adding FontUtilities
		 <8>	  6/4/90	MR		Clean up device metrics, simplify all loops
		 <7>	  6/1/90	MR		The MVT is dead! Let no one utter its foul name again.
		 <6>	 5/29/90	CL		Fixed double byte variables.
		 <5>	  5/3/90	RB		Mike added fs_GetCheckSumAdj.
		 <4>	 4/10/90	CL		Added script manager codes for Japanese.
		 							mrr	- Removed reference to fsinfo.error
		 <3>	 3/20/90	CL		Masks and macros for width table loops Added many greased loops
									Modified to use fractional ppem (fpem) Added routine to return
									CheckSumAdj Put x,y transform inline in FillLayoutInfo Add style
									extra before stretch in rounded-xformed loop Changed c to INT
									and use --count >= 0 in widthtable loops
		 <2>	 2/27/90	CL		New error code for missing but needed table. (0x1409 ).  Fixed
									transformed component bug.
	   <3.3>	11/14/89	CEL		Adding in device metrics for width table.
	   <3.2>	 9/25/89	CEL		Added rounding of integer capability back in. This fixes the
									smearing problem when fractenable is off.
	   <3.1>	 9/15/89	CEL		Bug that caused text not to appear… The font scaler state was
									set up correctly but the sfnt was purged. It was reloaded and
									the clientid changed but was still the same font. Under the
									rules of the FontScaler fs_newsfnt should not have to be called
									again to reset the state. The extra checks sent back a
									BAD_CLIENTID_ERROR so QuickDraw would think it was a bad font
									and not continue to draw.
	   <3.0>	 8/28/89	sjk		Cleanup and one transformation bugfix
	   <2.3>	 8/14/89	sjk		1 point contours now OK
	   <2.2>	  8/8/89	sjk		Improved encryption handling
	   <2.0>	  8/2/89	sjk		Just fixed EASE comment
	   <1.6>	  8/1/89	sjk		Added composites and encryption. Plus some enhancements…
	   <1.5>	 6/13/89	sjk		updated for new mapping stuff
	   <1.4>	  6/5/89	sjk		Fixed stretching problem, with respect to some widthtable crap
	   <1.3>	  6/2/89	CEL		16.16 scaling of metrics, minimum recommended ppem, point size 0
									bug, correct transformed integralized ppem behavior, pretty much
									so
	   <1.2>	 5/26/89	CEL		EASE messed up on “c” comments
	  <•1.1>	 5/26/89	CEL		Integrated the new Font Scaler 1.0 into Spline Fonts
	   <1.0>	 5/25/89	CEL		Integrated 1.0 Font scaler into Bass code for the first time…

	To Do:
*/
/** System Includes **/
#include <setjmp.h>
#include <FixMath.h>
#include <ToolUtils.h>
#include <Memory.h>
#include <Fonts.h>
#include <Script.h>

/** FontScaler’s Includes **/
#include "FSError.h"
#include "FSCdefs.h"
#include "FontMath.h"
#include "sfnt.h"
#include "sc.h"
#include "fnt.h"
#include "FontScaler.h"
#include "FSglue.h"
#include "privateSfnt.h"
#include "PartialFontExtensions.proto"
#include "MacExtra.h"
#include "Bass_Cache.h"
#include "MapString.h"
/* #include "OutlineCalls.h" */		/* <10> */
#include	"DoubleByteBassCache.h"

extern pascal EncodingTablePointer		GetEncodingTable( short fondID, uint16 *scriptCodePointer );
#define	kMissingGlyph					1			/* character code 1 maps to the missing glyph */

#define YMAX_SLOP		(3L << 16)
#define YMIN_SLOP		(3L << 16)

#define FIXONE			0x00010000
#define FIXONEHALF		0x00008000
#define HIGHWORDMASK	0xFFFF0000

#define FIXROUND(n)		FixRound(n)
#define ROUND(n)		(((n) + FIXONEHALF) & HIGHWORDMASK)

#define SETJUMP(key, error)	if ( error = setjmp(key->env) ) return( error )

/*** This guy is defined in FontScaler.c *****/
extern fsg_SplineKey* fs_SetUpKey(fs_GlyphInputType* inptr, int32 stateBits, int32* error);

static Boolean FillDeviceWidths(sfnt_DeviceMetrics* devMetricsPtr, Fixed* wt, int16* glyphs, Fixed styleExtra, Fixed fixedPpem)
{
	uint8* devWidths = (uint8*)(devMetricsPtr + 1);
	short numRecords = devMetricsPtr->numRecords;
	long recordSize = devMetricsPtr->recordSize;
	int ppem = FIXROUND(fixedPpem);

	if (ppem > 255) return false;

	for (--numRecords; numRecords >= 0; --numRecords)
	{
		if (*devWidths > ppem)		/* widths are sorted small to large */
			break;
		if (*devWidths == ppem)
		{
			register int16* shortw = (int16*)wt;
			register int16 shortStyleExtra = FIXROUND(styleExtra);
			register int16 charCode;

			devWidths += DEVEXTRA;			/* skip the size and max byte */
			for (charCode = 255; charCode >= 0; --charCode)
			{
				*shortw++ = devWidths[*glyphs++] + shortStyleExtra;
				*shortw++ = 0;
			}
			return true;
		}
		devWidths += recordSize;
	}
	return false;
}

static int32 Map256Chars(Handle sfnt, short* glyphs, short fondID)
{
	short platformCode = plat_Macintosh;
	uint16 scriptCode;
	long characterMapIndex;
	EncodingTable *encodingPtr;
	
	encodingPtr = GetEncodingTable(fondID, &scriptCode);
	characterMapIndex = GetFontMapInfo( sfnt, 0, PackStringClass( platformCode, scriptCode, 0 ) );
	
		//
		// 	if GetFontMapInfo failed (i.e. returned 0 for the index) and the scriptCode for
		// 	this font is > Roman, this could be one of the old Arabic or Hebrew fonts 
		//	that were released with a roman cmap. so try asking for a Roman cmap before
		//	bailing completely.
		//
	if ( characterMapIndex == 0 && scriptCode > smRoman )
		characterMapIndex = GetFontMapInfo( sfnt, 0, PackStringClass( platformCode, smRoman, 0 ) );

	
	if (characterMapIndex) {
		unsigned short byteIndex;
		unsigned char byteList[512];
		unsigned char *bytePtr = byteList;

		if (encodingPtr) {
			for (byteIndex = 0; byteIndex < 256; byteIndex++) {
				*bytePtr++ = byteIndex;
				if (encodingPtr->fHighByte[byteIndex]) {
					*(bytePtr - 1) = kMissingGlyph;
				}
			}
		} else {
			for (byteIndex = 0; byteIndex < 256; byteIndex++) {
				*bytePtr++ = byteIndex;
			}
		}

		
		ApplyFontMap(sfnt, characterMapIndex, glyphs, 256, byteList, 0);
		return 0;
	} else {
		return -1;
	}
}


/*
 *	fs_FillWidthTable:  returns non-zero error code
 *
 */
long fs_FillWidthTable(fs_GlyphInputType* inputPtr, Fixed* origWidthTable, Handle sfnt, Fixed styleExtra, short fondID, Boolean round)
{
	register Fixed scaledLastAdvanceWidthPlusStyle;
	register sfnt_HorizontalMetrics* metrics, *lastHorizontalMetricsPointer;
	unsigned short numberOf_LongHorMetrics;
	short lastAdvanceWidth, numGlyphs;
	short glyphStorage[256];
	short* glyphs = glyphStorage;
	Fixed* wt = origWidthTable;
	int32 error;
	fsg_SplineKey* key = (fsg_SplineKey*)fs_SetUpKey(inputPtr, INITIALIZED | NEWSFNT | NEWTRANS, &error);

	if (error) return error;
	SETJUMP(key, error);

	if (error = Map256Chars(sfnt, glyphs, fondID))
		return error;

	{
		sfnt_HorizontalHeader* horiHead = (sfnt_HorizontalHeader*)GetFontTablePtr( sfnt, 0, tag_HoriHeader);
		numberOf_LongHorMetrics = horiHead->numberOf_LongHorMetrics;
		QDUngetFontFrag(sfnt, horiHead);
	}

	{
		sfnt_maxProfileTable* profile = (sfnt_maxProfileTable*)GetFontTablePtr( sfnt, 0, tag_MaxProfile );
		numGlyphs = profile->numGlyphs;
		QDUngetFontFrag(sfnt, profile);
	}

/*	Precache the first 256 glyph entries of the 'hmtx' table.  We're assuming that the first 256 entries
	map, more or less, to the 256 characters in the ascii chart. This is mainly for the benefit of huge
	double-byte fonts since we don't really want to load in an 88k htmx table into our 100k partial font
	heap. For most single-byte fonts, this should have the same affect as loading in the entire table.
	Character codes in the 0..255 range that map to glyph index greater than 255 will result in a
	partial font call (read: this is painful).
*/

	metrics = (sfnt_HorizontalMetrics*) PartialGetFontTablePtr( sfnt, 0, 256 * sizeof( sfnt_HorizontalMetrics ), 0, tag_HorizontalMetrics );
	lastHorizontalMetricsPointer = PartialGetFontTablePtr( sfnt, (numberOf_LongHorMetrics - 1) * sizeof( sfnt_HorizontalMetrics ), sizeof( sfnt_HorizontalMetrics ), 0, tag_HorizontalMetrics );
	lastAdvanceWidth = lastHorizontalMetricsPointer->advanceWidth;
	scaledLastAdvanceWidthPlusStyle = ShortMulDiv( key->metricScalar, lastAdvanceWidth, key->emResolution ) + styleExtra;
/*
 *	Since we NEVER have perspective, and widths only look at x-component,
 *	all we need to use is the xScale.
 */
 	{
		register short charCode;
		short upem = key->emResolution;
		Fixed xScale = FixMul(key->currentTMatrix.transform[0][0], key->metricScalar);

		if (numberOf_LongHorMetrics < numGlyphs)
			scaledLastAdvanceWidthPlusStyle = ShortMulDiv( xScale, lastAdvanceWidth, upem ) + styleExtra;

	/*
	 *	FractEnable OFF
	 *	Try to find cached device metrics. If that fails, round the scaled widths.
	 */
		if (round)
		{
			sfnt_DeviceMetrics* devMetricsPtr;
			if (devMetricsPtr = (sfnt_DeviceMetrics*)GetFontTablePtr( sfnt, 0, tag_HoriDeviceMetrics))
			{
				Boolean success = FillDeviceWidths(devMetricsPtr, wt, glyphs, styleExtra, xScale);
				QDUngetFontFrag(sfnt, devMetricsPtr);
				if (success)
					goto EXIT;
			}

			styleExtra += FIXONEHALF;
			scaledLastAdvanceWidthPlusStyle = ROUND(scaledLastAdvanceWidthPlusStyle);
			for (charCode = 255; charCode >= 0; --charCode)
			{
				int16 index = *glyphs++;
				if ( index >= numberOf_LongHorMetrics ) {
				
					*wt++ = scaledLastAdvanceWidthPlusStyle;
					
				} else {
				
					if ( index < 256 ) {
					
						*wt++ = (ShortMulDiv( xScale, metrics[index].advanceWidth, upem ) + styleExtra) & HIWORDMASK;
						
					} else {
					
						sfnt_HorizontalMetrics*		glyphMetricPointer;
						
						glyphMetricPointer = PartialGetFontTablePtr( sfnt, index * sizeof( sfnt_HorizontalMetrics ), sizeof( sfnt_HorizontalMetrics ), 0, tag_HorizontalMetrics );
						*wt++ = (ShortMulDiv( xScale, glyphMetricPointer->advanceWidth, upem ) + styleExtra) & HIWORDMASK;
						QDUngetFontFrag( sfnt, glyphMetricPointer );
						
					}
				}
			}
		}
		else	/* FractEnable ON */
		{
			for (charCode = 255; charCode >= 0; --charCode)
			{
				int16 index = *glyphs++;
				if ( index >= numberOf_LongHorMetrics ) {
				
					*wt++ = scaledLastAdvanceWidthPlusStyle;
					
				} else {
				
					if ( index < 256 ) {
					
						*wt++ = ShortMulDiv( xScale, metrics[index].advanceWidth, upem ) + styleExtra;

					} else {
					
						sfnt_HorizontalMetrics*		glyphMetricPointer;
						
						glyphMetricPointer = PartialGetFontTablePtr( sfnt, index * sizeof( sfnt_HorizontalMetrics ), sizeof( sfnt_HorizontalMetrics ), 0, tag_HorizontalMetrics );
						*wt++ = ShortMulDiv( xScale, glyphMetricPointer->advanceWidth, upem ) + styleExtra;
						QDUngetFontFrag( sfnt, glyphMetricPointer );
					
					}
					
				}
			}
		}
	}
EXIT:
	QDUngetFontFrag( sfnt, lastHorizontalMetricsPointer );
	QDUngetFontFrag(sfnt, metrics);

/*	Make sure a negative styleExtra doesn't result in a negative width
 *	Condense style is a good candidate.
 */
	if (styleExtra < 0) {
		short charCode;
		Fixed* wt = origWidthTable;
		for (charCode = 255; charCode >= 0; --charCode) {
			if (*wt < 0)
				*wt = 0;
			wt++;
		}
	}

	return NO_ERR;
}

/*
 * fs_FillLayoutInfo
 *
 * Return error code like GlyphInfoType.error.
 */
 #define 	OVERFLOW_ERR		0x00AA
int32 fs_FillLayoutInfo( fs_GlyphInputType* inputPtr, fs_GlyphInfoType*, fs_LayoutInfo* info)
{
	register sfnt_FontHeader*	fontHead;
	register sfnt_HorizontalHeader*	horiHead;
	int32 error;
	int16 upem;
	register fsg_SplineKey* key = fs_SetUpKey(inputPtr, INITIALIZED | NEWSFNT | NEWTRANS, &error);
	Handle sfnt;

	if (error) return error;
	SETJUMP(key, error);

	sfnt = (Handle)key->clientID;
	upem = key->emResolution;
	fontHead = (sfnt_FontHeader*)GetFontTablePtr( sfnt, 0, tag_FontHeader );
	horiHead = (sfnt_HorizontalHeader*)GetFontTablePtr( sfnt, 0, tag_HoriHeader );

	{
		Fixed xScale = FixMul( key->metricScalar, key->currentTMatrix.transform[0][0]);

		info->widMax = ShortMulDiv( xScale, horiHead->advanceWidthMax, upem );
		info->lOverHMax = - ROUND(ShortMulDiv( xScale, horiHead->minLeftSideBearing, upem ));
		info->rOverHMax = - ROUND(ShortMulDiv( xScale, horiHead->minRightSideBearing, upem ));
	}
	{
		Fixed yScale = FixMul( key->metricScalar, key->currentTMatrix.transform[1][1]);

		info->ascent  = ROUND(ShortMulDiv( yScale, horiHead->yAscender, upem ));
		info->descent  = ROUND(ShortMulDiv( yScale, horiHead->yDescender, upem ));
		info->leading  = ROUND(ShortMulDiv( yScale, horiHead->yLineGap, upem ));
		info->yMax = ROUND(ShortMulDiv( yScale, fontHead->yMax, upem )) + YMAX_SLOP;
		info->yMin = ROUND(ShortMulDiv( yScale, fontHead->yMin, upem )) - YMIN_SLOP;
	}

	if ( info->lOverHMax < 0 ) info->lOverHMax = 0;
	if ( info->rOverHMax < 0 ) info->rOverHMax = 0;

	QDUngetFontFrag(sfnt, horiHead);
	QDUngetFontFrag(sfnt, fontHead);
	if (info->yMax - info->yMin < 0)		return OVERFLOW_ERR;

	return NO_ERR;
}

/*
 *	Extra routines for Chazzvetica				<3><4>
 */
pascal short fs_LowestPPEM(Handle fontHandle)
{
	sfnt_FontHeader* fontHead = GetFontTablePtr( fontHandle, 0, tag_FontHeader);
	short lowest = fontHead->lowestRecPPEM;
	QDUngetFontFrag(fontHandle, fontHead);
	return lowest;
}

