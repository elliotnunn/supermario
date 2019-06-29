/*
	File:		puPictures.c

	Contains:	High level routines for the picture utilities package. This file contains all of the external
				routines except for RecordPixMapInfo, which is in puPixMaps.c. It also contains all the higher
				level routines that work with pictures.

	Written by:	Dave Good. Some ideas stolen from Konstantin Othmer and Bruce Leak. Prototyped by Cris Rys.

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<18>	 2/21/91	SMC		KON,#83577: FgColor and BkColor color information from pictures
									was not getting recorded.
		<17>	  2/6/91	SMC		DDG,#81681: Updated unique color count after call to
									ConvertBankTo555.
		<16>	 1/16/91	JDR		(dba) Renamed METHOD_SYSTEM to systemMethod, METHOD_POPULAR to
									popularMethod, and METHOD_MEDIAN to medianMethod
		<15>	 1/14/91	dba		(csd) skip regions correctly in DoCopyBits; we were skipping two
									bytes too many
		<14>	 12/7/90	SMC		Fixed picture spooling problem where length and rectangle of
									picture was getting skipped twice. With DDG.
		<13>	 10/3/90	DDG		Fixed bug with regard to running on black and white machines. We
									now check for the presence of color quickdraw before we call
									GetCTSeed.
		<12>	 9/25/90	DDG		Fixed a bug in “DoCopyBits”, where the source and dest pointers
									for unpacking pack type two were reversed.
		<11>	 9/21/90	DDG		Made changes from code review. Fixed a bug, where font sizes
									greater than 127 would trash memory. They now set bit zero of
									the font size array. Numerous small changes to save bytes and
									cleanup the code.
		<10>	 8/16/90	DDG		Fixed a bug in GetPixMapInfo, where it wasn’t clearing the
									return record, when an error occurred.
		 <9>	 8/16/90	DDG		Fixed a bug in adding fonts to the font cache. Fixed a bug where
									recording a font with an emtpy name cache, would record garbage.
									Fixed a bug that would crash while recording dither patterns.
		 <8>	  8/5/90	DDG		Unknown changes...(probably fixed several small bugs).
		 <7>	  8/3/90	DDG		Cleaned up code some and fixed some bugs in deallocating memory.
									I also made the code the calculates the exact color table into a
									subroutine in the puPixMaps.c file.
		 <6>	  8/2/90	DDG		I did font names right; this involved allocating and maintaining
									a font (name <-> ID) cache handle. I also reordered the
									parameters for two of the internal routines in order to be more
									consistant with the other internal routines.
		 <5>	  8/1/90	DDG		Added a break statement to the end of type four decompression to
									prevent memory trashing. Fixed dither patterns to record the RGB
									color that they are trying to approximate. Now we copy the
									comment, font, and font name handles and give the COPY to the
									application. Fixed a bug in returning exact color tables. Added
									allocation and deallocation of a new global buffer
									(colorTableBuffer). Fixed a few bugs in returning font names and
									information.
		 <4>	 7/30/90	DDG		Removed debugger call.
		 <3>	 7/30/90	DDG		Added support for colorizing and moved the routine that records
									an RGB color to puPixMaps.c, so that it can support exact color
									banks.
		 <2>	 7/29/90	DDG		Fixed header.
		 <1>	 7/29/90	DDG		First checked in using new structure.

	To Do:
*/


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Includes
*/

#include <Fonts.h>
#include <GestaltEqu.h>
#include <OSUtils.h>
#include <Resources.h>
#include <ToolUtils.h>
#include <Traps.h>

#include "puPrivate.h"


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Defines
*/

/* Define this here until quickdraw.h defines them */

#define ditherPat	2


/* Define these here until palettes.h defines them */

#define	pmInhibitG2	0x0100
#define	pmInhibitG4	0x0400
#define	pmInhibitG8	0x1000


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Structures
*/

typedef struct CurrentFont {
	short				size;
	short				style;
	char				name[256];
	short				fontID;
} CurrentFont;


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Declarations
*/

OSErr		RecordComment( InternalPictInfoPtr, unsigned short );
OSErr		RecordFont( InternalPictInfoPtr, CurrentFont *, Handle );
OSErr		AddFontToCache( Handle, CurrentFont * );
long		FindFontName( Handle, CurrentFont * );
OSErr		DoCopyBits( InternalPictInfoPtr, unsigned short, Boolean );
void		ConvertBankTo555( InternalPictInfoPtr );
void		DisposeInternalMemory( InternalPictInfoPtr );

short		DivideULongByShort( unsigned long, unsigned short ) =
			{
				0x2017,						// move.l	(A7),D0
				0x222F, 0x0004,				// move.l	$0004(A7),D1
				0x80C1,						// divu.w	D1,D0
				0x0280, 0x0000, 0xFFFF		// andi.l	#$0000FFFF,D0
			};

/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************/
/****												Functions Follow									 ****/
/************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/

extern pascal OSErr __GetPictInfo(
PicHandle		thePictHand,
PictInfo		*thePictInfoPtr,
short			verb,
short			colorsRequested,
short			colorPickMethod,
short			version )
{
	PictInfoID		thePictInfoID;
	register OSErr	error;

	if( error = __NewPictInfo(&thePictInfoID, verb, colorsRequested, colorPickMethod, version) )
		goto returnError2;

	if( error = __RecordPictInfo(thePictInfoID, thePictHand) )
		goto returnError1;

	if( error = __RetrievePictInfo(thePictInfoID, thePictInfoPtr, colorsRequested) )
		goto returnError1;

	return( __DisposPictInfo(thePictInfoID) );

returnError1:
	__DisposPictInfo( thePictInfoID );
returnError2:
	ClearMemoryBytes( (Ptr)thePictInfoPtr, sizeof(PictInfo) );
	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

extern pascal OSErr __GetPixMapInfo(
PixMapHandle	thePixMapHand,
PictInfo		*thePictInfoPtr,
short			verb,
short			colorsRequested,
short			colorPickMethod,
short			version )
{
	PictInfoID		thePictInfoID;
	register OSErr	error;

	if( verb & (recordComments | recordFontInfo) )
	{
		error = pictInfoVerbErr;
		goto returnError2;
	}

	if( error = __NewPictInfo(&thePictInfoID, verb, colorsRequested, colorPickMethod, version) )
		goto returnError2;

	if( error = __RecordPixMapInfo(thePictInfoID, thePixMapHand) )
		goto returnError1;

	if( error = __RetrievePictInfo(thePictInfoID, thePictInfoPtr, colorsRequested) )
		goto returnError1;

	return( __DisposPictInfo(thePictInfoID) );

returnError1:
	__DisposPictInfo( thePictInfoID );
returnError2:
	ClearMemoryBytes( (Ptr)thePictInfoPtr, sizeof(PictInfo) );
	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

pascal OSErr __NewPictInfo(
long			*theIDPtr,
register short	verb,
register short	colorsRequested,
short			colorPickMethod,
short			version	)
{
	InternalPictInfoHandle			theInfoHand;
	register InternalPictInfoPtr	theInfoPtr;
	register OSErr					error;
	register short					pickMethodInited = false;

/*
|	Check the version number to make sure that it is valid. Also check the verb to make sure that it makes
|	sense. Bits beyond the 5th one must all be zero and you can’t set suppress B/W when you aren’t returning
|	color information.
*/

	if( version != CURRENT_VERSION )
		return pictInfoVersionErr;

	if( verb & ~allVerbMask )
		return pictInfoVerbErr;

	if( (verb & suppressBlackAndWhite) & !(verb & (returnPalette | returnColorTable)) )
		return pictInfoVerbErr;

/*
|	Allocate the main PictInfo structure. Note that we allocate it as clear, so that all our internal buffers
|	and counts are initialized to nil.
*/

	*theIDPtr = (long)theInfoHand = NewHandleClear( sizeof(InternalPictInfo) );
	if( !theInfoHand )
		return MemError();

/*
|	Move our new PictInfo handle hi, (so that we can allocate memory easier) then lock it down and dereference
|	it. We must lock it so that we can do buffer calls (they take a pointer into the info structure as a
|	parameter). Then save the verb and the info handle itself in the info structure.
*/

	MoveHHi( (Handle)theInfoHand );
	HLock( (Handle)theInfoHand );

	theInfoPtr = *theInfoHand;

	theInfoPtr->verb			= verb;
	theInfoPtr->thePictInfoID	= (long)theInfoHand;

/*
|	If we are capable of returning color information, then call the color pick method’s initialization routine.
|	Note that we check to make sure that colorsRequested is valid before continuing.
*/

	if( verb & (returnPalette | returnColorTable) )
	{
		if( (colorsRequested > 256) || (colorsRequested < 1) )
		{
			error = colorsRequestedErr;
			goto returnError;
		}

		theInfoPtr->colorPickMethod	= colorPickMethod;
		theInfoPtr->colorsRequested	= colorsRequested;

		if( error = InitPickMethod( colorsRequested, &(theInfoPtr->pickProcData), &(theInfoPtr->colorBankType), colorPickMethod ) )
			goto returnError;

		pickMethodInited = true;
	}

/*
|	If we are recording comments, then allocate some space for our comment info. Note that we allocate comment
|	information in chuncks of 256 comment specs.
*/

	if( verb & recordComments )
	{
		if( (theInfoPtr->ext.commentHandle = (CommentSpecHandle)NewHandleClear(256 * sizeof(CommentSpec))) == nil )
			goto returnMemError;
	}

/*
|	If we are recording fonts, then allocate some space for the font spec information and a separate handle for
|	the font names. Note that we allocate font specs in multiples of eight and font names in chunks of 256 chars.
*/

	if( verb & recordFontInfo )
	{
		if( (theInfoPtr->ext.fontHandle = (FontSpecHandle)NewHandleClear(8 * sizeof(FontSpec))) == nil )
			goto returnMemError;
		if( (theInfoPtr->ext.fontNamesHandle = NewHandleClear(256)) == nil )
			goto returnMemError;
	}

/*
|	If we are returning color information, then allocate space for our “true” color table and then for our color
|	bank. Note that we MUST clear the color bank, before using it.
*/

	if( (verb & (returnPalette | returnColorTable)) )
	{
		if( error = NewBuffer(&(theInfoPtr->colorTableBuffer), (256 * sizeof(ColorSpec)), bufferFixedLastingType) )
			goto returnError;

		if( error = NewBufferClear(&(theInfoPtr->colorBankBuffer), HistogramTableSize, bufferFixedLastingType) )
			goto returnError;
	}

/*
|	Unlock the info handle and return no error.
*/

	HUnlock( (Handle)theInfoHand );
	return noErr;

/*
|	We got a memory error here, so set our error code to the current memory error and fall thru to the error
|	return section.
*/

returnMemError:
	error = MemError();

/*
|	We got some sort of error here, so dispose of everything we allocated and return the error code along with
|	a nil PictInfoID.
*/

returnError:
	if( pickMethodInited )
		KillPickMethod( theInfoPtr->pickProcData, theInfoPtr->colorPickMethod );

	DisposeInternalMemory(theInfoPtr);
	DisposHandle( (Handle)theInfoHand );

	*theIDPtr = nil;
	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	If we are returning color information, then kill the color pick method. Note that this really only matters
|	if there is a custom color pick PROC.
*/

pascal OSErr __DisposPictInfo(
register PictInfoID	thePictInfoID )
{
	register InternalPictInfoPtr	theInfoPtr;
	register OSErr	error = noErr;
	char			theState;

	if( (theInfoPtr = DerefPictInfoID(thePictInfoID, &theState)) == 0 )
		return pictInfoIDErr;

	if( theInfoPtr->verb & (returnPalette | returnColorTable) )
		error = KillPickMethod( theInfoPtr->pickProcData, theInfoPtr->colorPickMethod );

	DisposeInternalMemory(theInfoPtr);
	DisposHandle( (Handle)thePictInfoID );

	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

pascal OSErr __RetrievePictInfo(
PictInfoID			thePictInfoID,
register PictInfo	*destinationPtr,
register short		colorsRequested )
{
	register InternalPictInfoPtr theInfoPtr;
	register OSErr		error = noErr;
	register long		theSize;
	CTabHandle			theCTHandle;
	PaletteHandle		thePaletteHandle;
	char				oldInfoState;

/*
|	Dereference the PictInfoID and pre-check the parameters that we can to make sure that there are no obvious
|	problems.
*/

	if( (theInfoPtr = DerefPictInfoID(thePictInfoID, &oldInfoState)) == 0 )
		return pictInfoIDErr;

	if( (theInfoPtr->verb & (returnPalette | returnColorTable)) && ( (colorsRequested > 256) || (colorsRequested < 1) ) )
	{
		HSetState( (Handle)thePictInfoID, oldInfoState );
		return colorsRequestedErr;
	}

/*
|	Return the base picture information. Then fill the handle fields with nil, in case we get an error. This
|	allows us to dispose of the ones that were allocated, before we encountered the error. Note that we need to
|	copy the handles for the comments, fonts, and font names.
*/

	BlockMove( (Ptr)(&(theInfoPtr->ext)), (Ptr)destinationPtr, sizeof(PictInfo) );

	destinationPtr->thePalette		= nil;
	destinationPtr->theColorTable	= nil;

	destinationPtr->commentHandle	= nil;
	destinationPtr->fontHandle		= nil;
	destinationPtr->fontNamesHandle	= nil;

	theCTHandle = nil;

	if( theInfoPtr->ext.commentHandle )
	{
		theSize = theInfoPtr->ext.uniqueComments * sizeof(CommentSpec);

		destinationPtr->commentHandle = (CommentSpecHandle) NewHandle( theSize );
		if( error = MemError() )
			goto returnError;

		BlockMove( (Ptr)(*(theInfoPtr->ext.commentHandle)), (Ptr)(*(destinationPtr->commentHandle)), theSize);
	}

	if( theInfoPtr->ext.fontHandle )
	{
		theSize = theInfoPtr->ext.uniqueFonts * sizeof(FontSpec);

		destinationPtr->fontHandle = (FontSpecHandle) NewHandle( theSize );
		if( error = MemError() )
			goto returnError;

		BlockMove( (Ptr)(*(theInfoPtr->ext.fontHandle)), (Ptr)(*(destinationPtr->fontHandle)), theSize);
	}

	if( theInfoPtr->ext.fontNamesHandle )
	{
		destinationPtr->fontNamesHandle = theInfoPtr->ext.fontNamesHandle;
		HandToHand( &(destinationPtr->fontNamesHandle) );
	}

/*
|	If we are returning either a color table or a palette, we need to create the correct color table. If we are
|	just returning a palette, then we will use this color table to create a palette and then dispose it later.
|	If we ARE returning a color table, then we will set the field in the info structure only after we are sure
|	that no errors have occurred.
*/

	if( theInfoPtr->verb & (returnColorTable | returnPalette) )
	{
		long	qdVersion;

	/*
	|	Allocate a color table and lock it down, so that we can fill it. Note that we do NOT
	|	clear it, so the CalcColorTable routines must take care of that for us.
	*/

		if( (theCTHandle = (CTabHandle)NewHandle(sizeof(ColorTable) + (colorsRequested - 1) * sizeof(ColorSpec))) )
		{
			register CTabPtr	theCTPtr;
			register long		uniqueColors;
			register short		pickMethod;

			uniqueColors = theInfoPtr->ext.uniqueColors;

			HLock( (Handle)theCTHandle );
			theCTPtr = *theCTHandle;

			if( (Gestalt(gestaltQuickdrawVersion, &qdVersion) == noErr) && (qdVersion >= gestalt8BitQD) )
				theCTPtr->ctSeed	= GetCTSeed();
			else
				theCTPtr->ctSeed	= 0;

			theCTPtr->ctFlags	= 0x0000;
			theCTPtr->ctSize	= colorsRequested - 1;

			if( theInfoPtr->colorBankType == ColorBankIsExactAnd555 )
			{
				if( uniqueColors <= colorsRequested )
				{
					CalcExactTable( colorsRequested, uniqueColors, (ColorSpec *)(theInfoPtr->colorBankBuffer.ptr), &(theCTPtr->ctTable) );
					goto unlockTable;
				}
				else
				{
					ConvertBankTo555( theInfoPtr );
					uniqueColors = theInfoPtr->ext.uniqueColors;
					destinationPtr->uniqueColors = uniqueColors;
				}
			}

			if( (pickMethod = theInfoPtr->colorPickMethod) == systemMethod )
			{
			/*
			|	if the 3/4 times the number of unique colors is greater than the number of colors that the user
			|	requested, then there are a lot more colors than we can return, so we should merge them together
			|	(use the median method). Otherwise, we are returning most of the colors, so we should just use
			|	the popular method.
			*/

				if( (uniqueColors - (uniqueColors >> 2)) > colorsRequested )
					pickMethod = medianMethod;
				else
					pickMethod = popularMethod;
			}

			if( error = CalcColorTable(theInfoPtr->pickProcData, colorsRequested, theInfoPtr->colorBankBuffer.ptr, &(theCTPtr->ctTable), pickMethod) )
				goto returnError;

unlockTable: HUnlock( (Handle)theCTHandle );
		}
		else
		{
			error = MemError();
			goto returnError;
		}

	/*
	|	If we need to return a palette, then use the color table to create one and If we don’t
	|	get any errors, then store it in the info structure. Note that we clip the number of
	|	requested colors to the true number of colors in the pixMap.
	*/

		if( theInfoPtr->verb & returnPalette )
		{
			if( colorsRequested > theInfoPtr->ext.uniqueColors )
				colorsRequested = theInfoPtr->ext.uniqueColors;

			thePaletteHandle = nil;
			if( Gestalt(gestaltQuickdrawVersion, &qdVersion) == noErr )
			{
				if( qdVersion >= gestalt8BitQD )
				{
					thePaletteHandle = NewPalette( colorsRequested, theCTHandle, (pmTolerant + pmInhibitG2 + pmInhibitG4 + pmInhibitG8), 0 );
					if( error = MemError() )
						goto returnError;
				}
			}
			destinationPtr->thePalette = thePaletteHandle;
		}

	/*
	|	If we should return a color table handle, then set the field in the info structure.
	|	Otherwise, check to see if we created a color table (in order to make a palette), and
	|	if we did, then dispose of it.
	*/

		if( theInfoPtr->verb & returnColorTable )
			destinationPtr->theColorTable = theCTHandle;
		else if( theCTHandle )
			DisposHandle( (Handle)theCTHandle );
	}

	HSetState( (Handle)thePictInfoID, oldInfoState );
	return noErr;

returnError:

	DisposHandle( (Handle)theCTHandle );
	DisposHandle( (Handle)(destinationPtr->thePalette) );
	DisposHandle( (Handle)(destinationPtr->theColorTable) );
	DisposHandle( (Handle)(destinationPtr->commentHandle) );
	DisposHandle( (Handle)(destinationPtr->fontHandle) );
	DisposHandle( (Handle)(destinationPtr->fontNamesHandle) );

	HSetState( (Handle)thePictInfoID, oldInfoState );
	ClearMemoryBytes( (Ptr)destinationPtr, sizeof(PictInfo) );
	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This function scans the picture opcode by opcode counting the objects drawn. It calls
|	RecordPixMapGuts for all bitmaps in the picture.
*/

pascal OSErr __RecordPictInfo(
PictInfoID	thePictInfoID,
PicHandle	thePictHandle )
{
	register InternalPictInfoPtr	theInfoPtr;
	register unsigned short			hiByte, loNib;
	unsigned short		hiNib;
	unsigned short		length, commentType;
	unsigned long		longLength;
	GetPicDataProcPtr	theGetPicProc;
	CurrentFont			currentFont;
	Handle				fontNameCache = nil;
	unsigned short		code;
	short				pictVersion;
	Boolean				endOfPict;
	CGrafPtr			thePortPtr;
	OSErr				error = noErr;
	char				oldInfoState;
	RGBColor			theColor;
	short				patternType;

/*
|	Dereference the PictInfoID and check its version and ID tag.
*/

	if( (theInfoPtr = DerefPictInfoID(thePictInfoID, &oldInfoState)) == 0 )
		return pictInfoIDErr;

/*
|	Initialize all the variables that we are going to need.
*/

	pictVersion					= 1;
	endOfPict					= false;

	currentFont.size			= 12;
	currentFont.style			= 0;
	currentFont.name[0]			= 0;
	currentFont.fontID			= 0;

	theInfoPtr->spoolPtr		= nil;
	theInfoPtr->spoolMax		= nil;
	theInfoPtr->spoolPictHandle	= thePictHandle;
	theInfoPtr->spoolPictOffset	= 0;
	theInfoPtr->spoolPictLength	= GetHandleSize( (Handle)thePictHandle );

	theInfoPtr->ext.hRes		= 72 << 16;
	theInfoPtr->ext.vRes		= 72 << 16;

	theInfoPtr->ext.sourceRect	= (*thePictHandle)->picFrame;

/*
|	If the current port is a color port, then set the foreground and background colors to the fields in
|	the port record. Otherwise, the foreground and background colors become black and white, respectively.
*/

	GetPort( (GrafPtr *)(&thePortPtr) );

	if( theInfoPtr->verb & (returnPalette | returnColorTable) )
	{
		theInfoPtr->foreColor.red = theInfoPtr->foreColor.green = theInfoPtr->foreColor.blue = 0x0000;
		theInfoPtr->backColor.red = theInfoPtr->backColor.green = theInfoPtr->backColor.blue = 0xFFFF;

		if( IsColorPort(thePortPtr->portVersion) )
		{
			theInfoPtr->foreColor	= thePortPtr->rgbFgColor;
			theInfoPtr->backColor	= thePortPtr->rgbBkColor;
		}
	}

/*
|	Check the verb to see if we are returning font information. If we are, then allocate a handle to hold
|	the name/ID cache. We start with an empty handle, that we will add font names to as we encounter them.
*/

	if( theInfoPtr->verb & recordFontInfo )
	{
		fontNameCache = NewHandle(0);
		if( error = MemError() )
			goto returnError;
	}

/*
|	Allocate new, empty buffers for the spoolBuffer, lineBuffer, and bandBuffer. This is just in case we
|	actually need to use them
*/

	NewEmptyBuffer( &(theInfoPtr->spoolBuffer) );
	NewEmptyBuffer( &(theInfoPtr->bandBuffer) );
	NewEmptyBuffer( &(theInfoPtr->lineBuffer) );

/*
|	Check to see if we need to spool this picture. If we do, then allocate a buffer for spooling.
|	If we don't, skip over the picture length and bounds rect (when spooling, the length and bounds
|	of the pict are in the handle passed to __RecordPictInfo).
*/

	theGetPicProc = (GetPicDataProcPtr)( (thePortPtr->grafProcs)->getPicProc );
	if( thePortPtr->grafProcs && theGetPicProc && (theGetPicProc != NGetTrapAddress(_StdGetPic, ToolTrap)) )
	{
		if( error = SetBufferSize(&(theInfoPtr->spoolBuffer), 0L, bufferVariableType) )
			goto returnError;

		theInfoPtr->spoolPtr	= theInfoPtr->spoolBuffer.ptr;
		theInfoPtr->spoolMax	= theInfoPtr->spoolBuffer.ptr + theInfoPtr->spoolBuffer.size;
		theInfoPtr->spoolProc	= theGetPicProc;
		(theGetPicProc)( theInfoPtr->spoolBuffer.ptr, theInfoPtr->spoolBuffer.size );
	} else
		SkipPictData( theInfoPtr, sizeof(short) + sizeof(Rect) );

/*
|	Scan through the picture, parsing each opCode, until we reach the “end-of-picture” opCode.
*/

	while( !endOfPict )
	{
	/*
	|	If we generated an error, either from the spooling routines or otherwise, then exit this loop
	*/

		if( error || (error = theInfoPtr->spoolPictError) )
			goto returnError;

	/*
	|	version 1 pictures have a byte opCode
	*/

		if( pictVersion == 1 )
			code = GetPictByte( theInfoPtr );
		else
		{
		/*
		|	Align the picture data stream to an even offset.
		*/

			if( theInfoPtr->spoolPictOffset & 1 )
				SkipPictByte( theInfoPtr );

		/*
		|	version 2 pictures have a word opCode
		*/

			code = GetPictWord( theInfoPtr );
		}

	/*
	|	Convert the opCode into a high byte, the high nibble of the low byte and the low nibble
	|	of the low byte.
	*/

		hiByte	= code >> 8;				// hiByte	= code / 256
		hiNib	= (code >> 4) & (16 - 1);	// hiNib	= (code / 16) % 16
		loNib	= code & (16 - 1);			// loNib	= code % 16

		if( hiByte == 0 )
		{
			switch( hiNib )
			{
			case  0:
				switch( loNib )
				{
					case  1:	/* skip region */
						goto skipRegion;

					case  9:	/* skip pattern (old-style) */
					case 10:
					case  2:
						SkipPictData( theInfoPtr, sizeof(Pattern) );
						break;

					case  8:	/* skip two bytes */
					case  5:
						SkipPictWord( theInfoPtr );
						break;

					case  6:	/* skip four bytes */
					case  7:
					case 11:
					case 12:
						SkipPictLong( theInfoPtr );
						break;
					
					case 14:	/* FgColor */						/* <18> - Used to just skip opcodes 14,15 */
					case 15:	/* BkColor */
						{
							long *tcp = &theColor.red;
							switch ( GetPictLong( theInfoPtr ) ) {
								case blackColor:
									*tcp = 0x00000000;			/* stuff red and green simultaneously */
									theColor.blue	= 0x0000;
									break;
								case yellowColor:
									*tcp = 0xFC00F37D;
									theColor.blue	= 0x052F;
									break;
								case magentaColor:
									*tcp = 0xF2D70856;
									theColor.blue	= 0x84EC;
									break;
								case redColor:
									*tcp = 0xDD6B08C2;
									theColor.blue	= 0x06A2;
									break;
								case cyanColor:
									*tcp = 0x0241AB54;
									theColor.blue	= 0xEAFF;
									break;
								case greenColor:
									*tcp = 0x00008000;
									theColor.blue	= 0x11B0;
									break;
								case blueColor:
									*tcp = 0x00000000;
									theColor.blue	= 0xD400;
									break;
								case whiteColor:
									*tcp = 0xFFFFFFFF;
									theColor.blue	= 0xFFFF;
									break;
								default:
									goto unknownColor;
							}
	
							if( loNib == 14 )
								theInfoPtr->foreColor = theColor;
							else
								theInfoPtr->backColor = theColor;
	
							if( theInfoPtr->verb & (returnPalette | returnColorTable) )
								RecordRGBColor( theInfoPtr, &theColor );
						}
unknownColor:			break;

					case  3:	/* TxFont */
						currentFont.fontID	= GetPictWord( theInfoPtr );
						break;

					case  4:	/* TxFace */
						currentFont.style	= GetPictByte( theInfoPtr );
						break;

					case 13:	/* TxSize */
						currentFont.size	= GetPictWord( theInfoPtr );
						break;
				}
				break;

			case  1:
				switch( loNib )
				{
					case  0:	/* skip eight bytes */
						SkipPictData( theInfoPtr, 8 );
						break;

					case  1:	/* version */
						pictVersion = GetPictByte( theInfoPtr );
						break;

					case  2:	/* pixel patterns */
					case  3:
					case  4:
						patternType = GetPictWord(theInfoPtr);

						SkipPictData(theInfoPtr, sizeof(Pattern));

						if( patternType == ditherPat )
							goto doRGBColor;

						error = DoCopyBits(theInfoPtr, 8, true);
						break;

					case  5:	/* skip two bytes */
					case  6:
						SkipPictWord( theInfoPtr );
						break;

					case 10:	/* RGB colors */
					case 11:
					case 13:
					case 15:
doRGBColor:				theColor.red	= GetPictWord(theInfoPtr);
						theColor.green	= GetPictWord(theInfoPtr);
						theColor.blue	= GetPictWord(theInfoPtr);

						if( loNib == 10 )
							theInfoPtr->foreColor = theColor;
						else if( loNib == 11 )
							theInfoPtr->backColor = theColor;

						if( theInfoPtr->verb & (returnPalette | returnColorTable) )
							RecordRGBColor( theInfoPtr, &theColor );
						break;
				}
				break;

			case  2:
				switch( loNib )
				{
					case  0:	/* lines */
						SkipPictData( theInfoPtr, 8 );
						goto lineCount;
					case  1:
						SkipPictLong( theInfoPtr );
						goto lineCount;
					case  2:
						SkipPictLong( theInfoPtr );			/* fall thru into “3” to skip another word */
					case  3:
						SkipPictWord( theInfoPtr );
lineCount:				theInfoPtr->ext.lineCount++;
						break;

					case  4:	/* skip reserved data */
					case  5:
					case  6:
					case  7:
					case 13:
					case 14:
					case 15:
						goto skipWordLength;

					case  8:	/* text */
						SkipPictLong( theInfoPtr );
						goto textCount;
					case  9:
					case 10:
						SkipPictByte( theInfoPtr );
						goto textCount;
					case 11:
						SkipPictWord( theInfoPtr );
textCount:				theInfoPtr->ext.textCount++;
						length = GetPictByte( theInfoPtr );			/* skip over the text data */
						SkipPictData( theInfoPtr, (unsigned long)(length) );
						if( theInfoPtr->verb & recordFontInfo )
							error = RecordFont( theInfoPtr, &currentFont,fontNameCache );
						break;

					case 12:
						SkipPictWord( theInfoPtr );
						currentFont.fontID	= GetPictWord( theInfoPtr );
						length = GetPictByte( theInfoPtr );
						currentFont.name[0] = length;
						GetPictData( theInfoPtr, &(currentFont.name[1]), (unsigned long)(length) );
						if( theInfoPtr->verb & recordFontInfo )
						{
							if( (error = theInfoPtr->spoolPictError) == noErr )
								error = AddFontToCache(fontNameCache, &currentFont);
						}
						break;
				}
				break;

			case  3:	/* rect */
					theInfoPtr->ext.rectCount++;
					goto skipRect;

			case  4:	/* rrect */
					theInfoPtr->ext.rRectCount++;
					goto skipRect;

			case  5:	/* oval */
					theInfoPtr->ext.ovalCount++;
skipRect:			if( loNib < 8 )
						SkipPictData( theInfoPtr, 8 );
					break;

			case  6:	/* arcs */
					theInfoPtr->ext.arcCount++;
					if( loNib < 8 )
						SkipPictData( theInfoPtr, 8 );
					SkipPictLong( theInfoPtr );
					break;

			case  7:	/* polygons */
					theInfoPtr->ext.polyCount++;
					goto skipRegion;

			case  8:	/* regions */
					theInfoPtr->ext.regionCount++;
skipRegion:			if( loNib < 8 )
					{
						length = GetPictWord( theInfoPtr ) - 2;		/* we have already moved past the length */
						SkipPictData( theInfoPtr, (unsigned long)(length) );
					}
					break;

			case 9:
				switch( loNib )
				{
					case  0:	/* unpacked */
					case  1:
					case  8:	/* packed */
					case  9:
					case 10:	/* direct, no CLUT */
					case 11:
						error = DoCopyBits(theInfoPtr, loNib, false);
						break;

					default:
						goto skipWordLength;
				}
				break;

			case 10:
				theInfoPtr->ext.commentCount++;

				commentType = GetPictWord( theInfoPtr );
				if( theInfoPtr->verb & recordComments )
					error = RecordComment( theInfoPtr, commentType );

				if( loNib == 0 )
					break;

				if( loNib != 1 )
				{
					SkipPictData( theInfoPtr, (unsigned long)(commentType) );
					break;
				}

skipWordLength:	length = GetPictWord( theInfoPtr );
				SkipPictData( theInfoPtr, (unsigned long)(length) );
				break;

			/* case 11 and 12 are blank */

			case 13:
			case 14:
				goto skipLongLength;

			case 15:
				if( loNib == 15 )
					endOfPict = true;
				else
					goto skipLongLength;
				break;
			}
		}
		else	/* hiByte != 0 */
		{
			if( code == 0x0C00 )
			{
				if( (short)GetPictWord(theInfoPtr) == -2 )		/* get the header version number */
				{
					SkipPictWord( theInfoPtr );			/* skip reserved word */

					theInfoPtr->ext.hRes				= GetPictLong( theInfoPtr );
					theInfoPtr->ext.vRes				= GetPictLong( theInfoPtr );

					theInfoPtr->ext.sourceRect.top		= GetPictWord( theInfoPtr );
					theInfoPtr->ext.sourceRect.left		= GetPictWord( theInfoPtr );
					theInfoPtr->ext.sourceRect.bottom	= GetPictWord( theInfoPtr );
					theInfoPtr->ext.sourceRect.right	= GetPictWord( theInfoPtr );

					SkipPictLong( theInfoPtr );			/* skip reserved long */
				}
				else
				{
					SkipPictData( theInfoPtr, 22 );		/* skip header */
				}
			}
			else if( hiByte < 0x80 )
					SkipPictData( theInfoPtr, (unsigned long)(2 * hiByte) );
			else if( hiByte >= 0x81 )
			{
skipLongLength:	longLength = GetPictLong( theInfoPtr );
				SkipPictData( theInfoPtr, longLength );
			}
		}
	}

/*
|	Offset the sourceRect to have a topLeft corner of (0,0), then dispose of our buffers, restore the info
|	handle’s state and return the error.
*/

returnError:

	theInfoPtr->ext.sourceRect.bottom	-= theInfoPtr->ext.sourceRect.top;
	theInfoPtr->ext.sourceRect.right	-= theInfoPtr->ext.sourceRect.left;
	theInfoPtr->ext.sourceRect.top		= 0;
	theInfoPtr->ext.sourceRect.left		= 0;

	DisposHandle( fontNameCache );

	KillBuffer( &(theInfoPtr->spoolBuffer) );
	KillBuffer( &(theInfoPtr->bandBuffer) );
	KillBuffer( &(theInfoPtr->lineBuffer) );

	HSetState( (Handle)thePictInfoID, oldInfoState );

	return error;
}

/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************/
/****										Internal Functions Follow									 ****/
/************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/

static OSErr RecordComment(
InternalPictInfoPtr		theInfoPtr,
register unsigned short	commentType )
{
	register CommentSpec	**theCommentHand;
	register CommentSpec	*theCommentPtr;
	register long			index, commentCount;

/*
|	Check to see if an error has already occurred in the low level parsing routines. If one did, then return
|	this error.
*/

	if( theInfoPtr->spoolPictError )
		return theInfoPtr->spoolPictError;

/*
|	Setup our local variables.
*/

	commentCount	= theInfoPtr->ext.uniqueComments;
	theCommentHand	= theInfoPtr->ext.commentHandle;
	theCommentPtr	= *theCommentHand;

/*
|	See if we already a comment of this type. If we do, then increment it’s counter and return.
*/

	for(index = commentCount; index > 0; index--)
	{
		if( commentType == theCommentPtr->ID )
		{
			theCommentPtr->count++;
			return noErr;
		}

		theCommentPtr++;
	}

/*
|	If we got here, then we have a new comment. First check to see if we already have space
|	for this comment. If we don’t then allocate another 256 comment specs and continue. Then,
|	point to the next free entry in the comment spec handle and fill it with (1, commentType).
|	Finally, update the count of unique comments.
*/

	if( (char)commentCount == (char)0xFF )
	{
		SetHandleSize( (Handle)theCommentHand, ((commentCount  & 0xFFFFFF00) + 512) * sizeof(CommentSpec) );
		if( MemError() )
			return MemError();
	}

	theCommentPtr = *theCommentHand + commentCount;
	theCommentPtr->count = 1;
	theCommentPtr->ID	 = commentType;

	theInfoPtr->ext.uniqueComments++;
	return noErr;
}

/*----------------------------------------------------------------------------------------------------------*/

static OSErr RecordFont(
InternalPictInfoPtr		theInfoPtr,
register CurrentFont	*inputFontPtr,
Handle					fontNameCache )
{
	register FontSpec	*theFontSpecPtr;
	register long		fontCount;
	register long		theSize;
	register long		theNameOffset;
	FontSpec	**theFontSpecHand;
	char		**theFontNamesHand;
	short		theSysFontID;

/*
|	Check to see if an error has already occurred in the low level parsing routines. If one did, then return
|	this error.
*/

	if( theInfoPtr->spoolPictError )
		return theInfoPtr->spoolPictError;

/*
|	Copy the two font handles out of our main data handle into local variables.
*/

	theFontSpecHand		= theInfoPtr->ext.fontHandle;
	theFontNamesHand	= theInfoPtr->ext.fontNamesHandle;

/*
|	If the size is greater than 127, then set bit zero in the FontSpec.
*/

	if( inputFontPtr->size > 127 )
		inputFontPtr->size = 0;

/*
|	If we have seen this font before, then update the bit array of the sizes and update the
|	style.
*/

	theFontSpecPtr	= *theFontSpecHand;
	for(fontCount = theInfoPtr->ext.uniqueFonts; fontCount > 0; fontCount--)
	{
		if( theFontSpecPtr->pictFontID == inputFontPtr->fontID )
		{
			BitSet( (Ptr)&(theFontSpecPtr->size[0]), inputFontPtr->size );
			theFontSpecPtr->style |= inputFontPtr->style;
			return noErr;
		}

		theFontSpecPtr++;
	}

/*
|	If we got here, then we haven’t seen this font before, so we must add it to the list. First,
|	check to see if there are no more empty entries in the handle. If that is so, then we need
|	more memory, so allocate enough for 8 more font specs and return an error if there is one.
*/

	fontCount = theInfoPtr->ext.uniqueFonts;

	if( ((char)fontCount & (char)7) == (char)7 )
	{
		SetHandleSize( (Handle)theFontSpecHand, ((fontCount & 0xFFFFFFF8) + 16) * sizeof(FontSpec) );
		if( MemError() )
			return MemError();
	}

/*
|	We have the space for our font spec now, so first find the real number for the input font,
|	and then fill out all the fields in the font spec except for nameOffset.
*/

	theSysFontID = inputFontPtr->fontID;

	inputFontPtr->name[0] = 0;
	if( (theNameOffset = FindFontName(fontNameCache, inputFontPtr)) != 0 )
		BlockMove( (*fontNameCache + theNameOffset), inputFontPtr->name, (long)( *((*fontNameCache + theNameOffset)) ) );

	if( (inputFontPtr->name[0]) )
		GetFNum(inputFontPtr->name, &theSysFontID);

	theFontSpecPtr = *theFontSpecHand + fontCount;
	theFontSpecPtr->sysFontID	= theSysFontID;
	theFontSpecPtr->pictFontID	= inputFontPtr->fontID;
	theFontSpecPtr->style		= inputFontPtr->style;
	theFontSpecPtr->size[0]		= 0;
	theFontSpecPtr->size[1]		= 0;
	theFontSpecPtr->size[2]		= 0;
	theFontSpecPtr->size[3]		= 0;
	BitSet( (Ptr)&(theFontSpecPtr->size[0]), inputFontPtr->size );

/*
|	Add the font name to the name handle. Otherwise, get an offset to the end of the old name list and set the
|	nameOffset field in the font spec record to be this ending offset. Then we check to see if there is still
|	space left in the names handle for our new name. If there isn’t then, we allocate 256 more bytes for the
|	name handle. Finally, we block move the new name into the name handle at the correct spot.
*/

	theSize			= GetHandleSize(theFontNamesHand);
	theNameOffset	= 0;
	if( fontCount != 0 )
	{
		theNameOffset = (theFontSpecPtr - 1)->nameOffset;
		theNameOffset += *(*theFontNamesHand + theNameOffset) + 1;
	}
	theFontSpecPtr->nameOffset = theNameOffset;

	if( theSize < (theNameOffset + inputFontPtr->name[0] + 1) )
	{
		SetHandleSize(theFontNamesHand, (theNameOffset + 256));
		if( MemError() )
			return MemError();

		theFontSpecPtr = *theFontSpecHand + fontCount;
	}

	BlockMove( inputFontPtr->name, (*theFontNamesHand + theNameOffset), (inputFontPtr->name[0] + 1) );

	theInfoPtr->ext.uniqueFonts++;
	return noErr;
}

/*----------------------------------------------------------------------------------------------------------*/

static long FindFontName(
Handle			fontNameCache,
CurrentFont		*inputFontPtr )
{
	register short	*shortPtr, *maxPtr;
	register unsigned short	nameLength;

	shortPtr = (short *)(*fontNameCache);
	maxPtr	 = (short *)((char *)shortPtr + GetHandleSize(fontNameCache));

	while( shortPtr < maxPtr )
	{
		if( *(shortPtr++) == inputFontPtr->fontID )
			return( (long)shortPtr - (long)(*fontNameCache) );

		nameLength = *( (unsigned char *)shortPtr ) + 1;
		nameLength = (nameLength + 1) & ~1;

		shortPtr = (short *)( (char *)shortPtr + nameLength );
	}

	return( 0 );
}

/*----------------------------------------------------------------------------------------------------------*/

static OSErr AddFontToCache(
Handle					fontNameCache,
register CurrentFont	*inputFontPtr )
{
	register short	*shortPtr;
	register long	oldSize;
	register unsigned short	nameLength;

	if( FindFontName(fontNameCache, inputFontPtr) )
		return noErr;

	oldSize  = GetHandleSize(fontNameCache);
	nameLength	= (unsigned char)(inputFontPtr->name[0]) + 1;

	SetHandleSize( fontNameCache, ((oldSize + nameLength + 3) & ~1) );
	if( MemError() )
		return MemError();

	shortPtr		= (short *)(*fontNameCache + oldSize);
	*(shortPtr++)	= inputFontPtr->fontID;

	BlockMove( (char *)(inputFontPtr->name), (char *)shortPtr, nameLength);

	return noErr;
}

/*----------------------------------------------------------------------------------------------------------*/

static OSErr DoCopyBits(
register InternalPictInfoPtr	theInfoPtr,
volatile unsigned short			loNib,
volatile Boolean				ppatFlag )
{
	register PixMapPtr		thePixMapPtr;
	register unsigned short	index;
	CTabPtr			theCTPtr;
	PixMapHandle	thePixMapHandle;
	CTabHandle		theCTHandle = nil;
	unsigned short	rowBytes;
	unsigned short	length;
	unsigned short	scanLines, linesPerBand;
	unsigned short	packType;
	OSErr			error;
	Boolean			hasFourComponents;

/*
|	Check to see if an error has already occurred in the low level parsing routines. If one did, then return
|	this error.
*/

	if( theInfoPtr->spoolPictError )
		return theInfoPtr->spoolPictError;

/*
|	Allocate space for our pixMap structure. Note that we must do this, even if the data
|	is actually a bitMap. We allocate the handle as “clear”, so that we don’t have to fill
|	in all the fields that default to zero.
*/

	if( (thePixMapHandle = (PixMapHandle)NewHandleClear(sizeof(PixMap))) == nil )
		return MemError();

/*
|	Lock down the pixMap handle and start reading data from the picture into it. Also
|	allocate a color table (if there is one) and put the handle to it in the pmTable
|	field of the pixMap handle.
*/

	MoveHHi( (Handle)thePixMapHandle );
	HLock( (Handle)thePixMapHandle );
	thePixMapPtr = *thePixMapHandle;

/*
|	Skip the baseAddr field in the case of direct pixMaps. Non-Direct pixMaps do not store this field in the
|	picture. Then move the rowBytes and boundsRect into the pixMap with one GetPictData call.
*/

	if( loNib >= 0x0A )
		SkipPictLong(theInfoPtr);

	GetPictData( theInfoPtr, (char *)&(thePixMapPtr->rowBytes), (sizeof(short) + sizeof(Rect)) );

	rowBytes	= thePixMapPtr->rowBytes;
	scanLines	= thePixMapPtr->bounds.bottom - thePixMapPtr->bounds.top;

	if( IsPixMap(rowBytes) )
	{
	/*
	|	The high bit of rowBytes being SET means that the data that follows is a true pixMap. Read the following
	|	fields into the pixMap with one call to GetPictData: pmVersion, packType, packSize, hRes, vRes, pixelType,
	|	pixelSize, cmpCount, cmpSize, and planeBytes. Then skip the pmTable and pmReserved fields.
	*/

		theInfoPtr->ext.pixMapCount++;

		GetPictData( theInfoPtr, (char *)&(thePixMapPtr->pmVersion), 28 );
		SkipPictData(theInfoPtr, 8);

	/*
	|	If the pixMap is not “direct” (16 or 32 bit), then a color table will follow the pixMap. Get the color
	|	table here. Note that we have to get the first three fields separate from the rest, so that we can use
	|	theCTSize to allocate the handle and know how much data to read.
	*/

		if( loNib < 0x0A )
		{
			long	theCTSeed;
			short	theCTFlags;
			short	theCTSize;

			theCTSeed	= GetPictLong(theInfoPtr);
			theCTFlags	= GetPictWord(theInfoPtr);
			theCTSize	= GetPictWord(theInfoPtr);

			if( (theCTHandle = (CTabHandle)NewHandleClear(sizeof(ColorTable) + (theCTSize * sizeof(ColorSpec)))) == nil )
			{
				error = MemError();
				goto returnError;
			}

			HLock( (Handle)theCTHandle );
			theCTPtr = *theCTHandle;

			theCTPtr->ctSeed	= theCTSeed;
			theCTPtr->ctFlags	= theCTFlags;
			theCTPtr->ctSize	= theCTSize;
			GetPictData(theInfoPtr, (unsigned char *)(theCTPtr->ctTable), ((theCTSize + 1) * sizeof(ColorSpec)));

			HUnlock( (Handle)theCTHandle );
			thePixMapPtr->pmTable = theCTHandle;
		}
	}
	else
	{
	/*
	|	The high bit of rowBytes being CLEAR means that the data that follows is actually a bitMap.
	|	In this case, there is no more data that pertains to the bitMap and there is no color table.
	*/

		theInfoPtr->ext.bitMapCount++;

		thePixMapPtr->hRes			= 0x00480000;
		thePixMapPtr->vRes			= 0x00480000;
		thePixMapPtr->pixelSize		= 1;
		thePixMapPtr->cmpCount		= 1;
		thePixMapPtr->cmpSize		= 1;

		/* all other fields are zero, for a standard pixMap */
	}

/*
|	Check to see if an error has already occurred in the low level parsing routines. If one did, then return
|	this error.
*/

	if( error = theInfoPtr->spoolPictError )
		goto returnError;

/*
|	If the rowBytes is less than eigth, then the data is already unpacked. This, strangely
|	enough is indicated by a packType of one. If rowBytes >= 8, then we check to see if
|	the pixMap is a bitMap or a non-direct pixMap. If this is the case, then the data is
|	packed in the format used by _PackBits and we indicate this with a packType of zero!
|	The other cases (direct pixMaps with a rowBytes greater than eight), use the packType
|	of the bitMap to determine the packing scheme. Finally, if this is the case, then I
|	use a gross hack to make sure that my “fake” packType of zero, is never falsely set.
|	It turns out that quickdraw treats a packType of zero exactly the same as a packType
|	of two, so I take advantage of this.
*/

	rowBytes &= 0x3FFF;
	hasFourComponents	= ( thePixMapPtr->cmpCount == 4 );

	if( rowBytes < 8 )
		packType = 1;
	else if( thePixMapPtr->pixelType != 16 )
		packType = 0;
	else if( (packType = thePixMapPtr->packType) == 0 )
		packType = 2;

	HUnlock( (Handle)thePixMapHandle );

/*
|	If the “pixMap” is really a pixPat, then there are no srcRect, dstRect, and mode fields. In
|	all other cases (bitMap or pixMap), we must skip over these fields.
*/

	if( !ppatFlag )
		SkipPictData(theInfoPtr, sizeof(short) + (2 * sizeof(Rect)));

/*
|	If the opCode is $91 or $99 or $9B, then there is a mask region in the picture that we
|	need to skip. The format of a region is the data length (word) followed by the data.
*/

	if( loNib & 1 )
	{
		length = GetPictWord(theInfoPtr) - 2;	/* we have already moved past the length <15> */
		SkipPictData(theInfoPtr, (unsigned long)(length));
	}

/*
|	Check to see if our buffers are already allocated. If they aren’t then allocate them.
|	Note that we allocate the line buffer only if we need to. Also note that we must allocate
|	some extra slop in the line buffer, in case the compressed data is actually bigger than
|	the uncompressed data.
*/

	if( (packType == 0) || (packType == 3) || (packType == 4) )
	{
		short	slop;

		slop = rowBytes >> 4;
		if( slop < 2 )
			slop = 2;

		if( error = SetBufferSize(&(theInfoPtr->lineBuffer), (rowBytes + slop), bufferFixedType) )
			goto returnError;
	}

	if( error = SetBufferSize(&(theInfoPtr->bandBuffer), 0L, bufferVariableType) )
		goto returnError;

/*
|	Now scan through all the data for the pixMap, copying it ito the banding buffer, and
|	uncompressing it if we need to. For each band, we call RecordPixMapGuts to concatenate
|	the color information for the band to our collected total.
*/

	(*thePixMapHandle)->bounds.bottom = (*thePixMapHandle)->bounds.top;
	linesPerBand = DivideULongByShort(theInfoPtr->bandBuffer.size, rowBytes);

	for( ; scanLines > 0; scanLines -= linesPerBand )
	{
	/*
	|	If we are on the last band, make sure that we don’t go beyond the end of the picture.
	*/

		if( linesPerBand > scanLines )
			linesPerBand = scanLines;

	/*
	|	Determine the packing type for the scan lines and unpack them into the banding buffer.
	*/

		switch( packType )
		{
			case 0:
			/*
			|	This packType indicates that the data was packed by run-length encoding
			|	bytes of data with _PackBits. This packing scheme requires us to use the
			|	scan line buffer.
			*/
				{
					unsigned char	*srcPtr, *dstPtr;

					dstPtr = theInfoPtr->bandBuffer.ptr;
					for(index=0; index < linesPerBand; index++)
					{
						length = (rowBytes > 250) ? GetPictWord(theInfoPtr) : GetPictByte(theInfoPtr);
						srcPtr = theInfoPtr->lineBuffer.ptr;

						GetPictData(theInfoPtr, srcPtr, (unsigned long)(length));
						if( error = theInfoPtr->spoolPictError )
							goto returnError;

						UnpackBits(&srcPtr, &dstPtr, rowBytes);
					}
				}
				break;

			case 1:
			/*
			|	This packType indicates that the data was not packed at all, so just copy it
			|	directly into the banding buffer.
			*/
				GetPictData(theInfoPtr, theInfoPtr->bandBuffer.ptr, (unsigned long)(linesPerBand * rowBytes));
				if( error = theInfoPtr->spoolPictError )
					goto returnError;

				break;

			case 2:
			/*
			|	This packType indicates that the data was packed by removing the alpha
			|	channel from each pixel. Unpack this by reading the data for an entire
			|	band into the end of the banding buffer and then unpacking it in place.
			|	The AddPadBytes routine does the actual “unpacking”.
			*/
				{
					register unsigned char	*srcPtr, *dstPtr;
					register unsigned long	longLength, pixelCount;

					longLength	= linesPerBand * rowBytes;
					pixelCount	= longLength >> 2;
					dstPtr		= theInfoPtr->bandBuffer.ptr;
					srcPtr		= dstPtr + pixelCount;

					GetPictData(theInfoPtr, srcPtr, (longLength - pixelCount));
					if( error = theInfoPtr->spoolPictError )
						goto returnError;

					AddPadBytes(srcPtr, dstPtr, pixelCount);
				}
				break;

			case 3:
			/*
			|	This packType indicates that the data was packed by run-length encoding
			|	words of data. This is just like type 0, but the “atom” of data is a word
			|	instead of a byte. This packing scheme requires us to use the scan line
			|	buffer.
			*/
				{
					unsigned char	*dstPtr;

					dstPtr = theInfoPtr->bandBuffer.ptr;
					for(index=0; index < linesPerBand; index++)
					{
						length = (rowBytes > 250) ? GetPictWord(theInfoPtr) : GetPictByte(theInfoPtr);

						GetPictData(theInfoPtr, theInfoPtr->lineBuffer.ptr, (unsigned long)(length));
						if( error = theInfoPtr->spoolPictError )
							goto returnError;

						UnpackWords(theInfoPtr->lineBuffer.ptr, &dstPtr, rowBytes);
					}
				}
				break;

			case 4:
			/*
			|	This packType indicates that the data was packed by extracting out the
			|	components of each pixel, concatenating them together and then run-length
			|	encoding this entire block as bytes of data. We first get the compressed
			|	data into the line buffer, then we unpack it into the banding buffer. After
			|	this, we copy the data back to the line buffer and merge the components
			|	of all the pixels. Then we move on to the next row.
			*/
				{
					register unsigned char	*srcPtr, *dstPtr;
					register unsigned short	compressedRowBytes, pixelCount;
					unsigned char	*tempSrc, *tempDst;

					pixelCount			= rowBytes >> 2;
					compressedRowBytes	= ( hasFourComponents ) ? rowBytes : rowBytes - pixelCount;
					srcPtr				= theInfoPtr->lineBuffer.ptr;

					dstPtr = theInfoPtr->bandBuffer.ptr;
					for(index=0; index < linesPerBand; index++)
					{
						length	 = (rowBytes > 250) ? GetPictWord(theInfoPtr) : GetPictByte(theInfoPtr);
						GetPictData(theInfoPtr, srcPtr, (unsigned long)(length));
						if( error = theInfoPtr->spoolPictError )
							goto returnError;

						tempSrc = srcPtr;
						tempDst = dstPtr;
						UnpackBits(&tempSrc, &tempDst, compressedRowBytes);

						BlockMove(dstPtr, srcPtr, compressedRowBytes);
						MergeRGBData( srcPtr, dstPtr, pixelCount, hasFourComponents );

						dstPtr += rowBytes;
					}
				}
				break;

			default:
			/*
			|	If we don’t recognize the packing type then skip over all the data, a scan
			|	line at a time and then “continue” the main loop, thus skipping the pixMap
			|	record.
			*/
				for(index=0; index < linesPerBand; index++)
				{
					length = (rowBytes > 250) ? GetPictWord(theInfoPtr) : GetPictByte(theInfoPtr);
					SkipPictData(theInfoPtr, (unsigned long)(length));
				}
				continue;
		}

		thePixMapPtr = *thePixMapHandle;
		thePixMapPtr->bounds.top	= thePixMapPtr->bounds.bottom;
		thePixMapPtr->bounds.bottom	+=  linesPerBand;
		thePixMapPtr->baseAddr		= theInfoPtr->bandBuffer.ptr;

		if( error = RecordPixMapGuts(theInfoPtr, thePixMapHandle) )
			goto returnError;
	}

/*
|	Dispose of our temporary memory here (the pixMap handle and the color table handle).
|	Note that we do NOT dispose of our buffers, since we might do another pixMap and we
|	don’t want to waste time allocating memory twice.
*/

returnError:

	DisposHandle( (Handle)theCTHandle );
	DisposHandle( (Handle)thePixMapHandle );

	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

static void DisposeInternalMemory(
register InternalPictInfoPtr theInfoPtr )
{
	KillBuffer( &(theInfoPtr->colorBankBuffer) );
	KillBuffer( &(theInfoPtr->colorTableBuffer) );

	DisposHandle( (Handle)(theInfoPtr->ext.fontNamesHandle) );
	DisposHandle( (Handle)(theInfoPtr->ext.fontHandle) );
	DisposHandle( (Handle)(theInfoPtr->ext.commentHandle) );
}

/*----------------------------------------------------------------------------------------------------------*/
