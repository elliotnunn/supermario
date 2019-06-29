/*
	File:		puPixMaps.c

	Contains:	All the routines that work on pixMaps for the picture utilities package. There is one routine
				(DoCopyBits) that does things with pixMaps that is not in this file, but that is because it
				only extracts pixMaps out of a picture and then passes them to RecordPixMapGuts (in this file).

	Written by:	Dave Good. Some ideas stolen from Konstantin Othmer and Bruce Leak. Prototyped by Cris Rys.

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<18>	 2/21/91	SMC		KON,#(to be announced): Fixed case of CLUT with less than
									2^pixelsize entries getting padded with too many extra colors
									and writing off the end of the buffer.
		<17>	 2/14/91	SMC		KON,#82895: Suppress black and white in ConvertBankTo555 so that
									colors near black or white don't show up later.
		<16>	  2/6/91	SMC		DDG,#81681: Fixed ConvertBankTo555 to reset the count of unique
									colors and to properly keep track of the number of pixels of a
									specified color (it used to “forget” about some of the pixels
									that corresponded to an index).
		<15>	  2/4/91	SMC		KON,#82191: Fixed the way pixmaps were getting colorized. The
									context of the fore and back colors was switched.
		<14>	 1/16/91	dba		(DDG) calculate a source rectangle in RecordPixMapInfo
		<13>	 9/25/90	DDG		Saved a few bytes throughout various routines.
		<12>	 9/25/90	DDG		Fixed a bug in “RecordPixMapGuts”, where the
									“MakeTrueColorTable” routine was being called with a bogus color
									table pointer in the case of bitMaps. Also fixed two bugs in the
									“MakeTrueColorTable” routine when it generates relative colors.
									The first one was that the dest pointer was not being set right
									when the relative color loop started. The second was that colors
									used to be able to wrap around to black, sometimes giving two
									instances of black in the color table. They are now clipped to
									white.
		<11>	 9/21/90	DDG		Made changes from code review. Moved several routines to
									assembly language (“puLowLevel.a”).
		<10>	  8/5/90	DDG		Fixed several bugs with recording custom colors.
		 <9>	  8/3/90	DDG		Cleaned up the code some and added the CalcExactTable routine.
		 <8>	  8/2/90	DDG		Added support for recording colors in custom color procs. Also
									incorporated Sean Callahan’s colorizing algorithm.
		 <7>	  8/1/90	DDG		All sorts of changes to support relative entries in the pixmap’s
									color table.
		 <6>	 7/31/90	DDG		Made RecordRGBColor respect the suppressBlackAndWhite verb.
		 <5>	 7/31/90	DDG		Speeded up the suppressBlackAndWhite case for recording indexed
									pixMaps into exact color banks.
		 <4>	 7/30/90	DDG		Added code to support the suppressBlackAndWhite verb. Also
									cleaned up the MakeIndexCache routine and the entire path thru
									adding colors to the color bank.
		 <3>	 7/30/90	DDG		Added all sorts of routines to support exact color banks. Also
									fixed a bug in 32-bit pixMaps.
		 <2>	 7/29/90	DDG		Fixed header.
		 <1>	 7/29/90	DDG		First checked in using new structure.

	To Do:
*/

/*----------------------------------------------------------------------------------------------------------*/

/*
|	Includes
*/

#include "puPrivate.h"
#include <ToolUtils.h>


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Structures
*/

typedef struct directPixel32 {

	unsigned char	filler;
	unsigned char	red;
	unsigned char	green;
	unsigned char	blue;

} directPixel32, *directPixel32Ptr;



/*----------------------------------------------------------------------------------------------------------*/

/*
|	Declarations
*/

void	MakeIndexCache( InternalPictInfoPtr );
void	ConvertBankTo555( InternalPictInfoPtr );
void	MakeTrueColorTable( InternalPictInfoPtr, CTabHandle, short );



/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************/
/****												Functions Follow									 ****/
/************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/

pascal OSErr __RecordPixMapInfo(
register PictInfoID	thePictInfoID,
PixMapHandle		thePixMapHandle )
{
	register InternalPictInfoPtr	theInfoPtr;
	register PixMapPtr				thePixMapPtr;
	register OSErr	error;
	char			oldInfoState;
	CGrafPtr		thePortPtr;

/*
|	Save the locked state of the information handle, then move it high and lock it down. After this,
|	then check the version number of the picture information record to make sure that it is valid.
*/

	if( (theInfoPtr = DerefPictInfoID(thePictInfoID, &oldInfoState)) == 0 )
		return pictInfoIDErr;

/*
|	Increment the proper count in the structure, depending on whether the “PixMap” is really a
|	PixMap or a BitMap.
*/

	thePixMapPtr = *thePixMapHandle;

	if( IsPixMap(thePixMapPtr->rowBytes) )
	{
		++theInfoPtr->ext.pixMapCount;
		theInfoPtr->ext.hRes = thePixMapPtr->hRes;
		theInfoPtr->ext.vRes = thePixMapPtr->vRes;
	}
	else
	{
		++theInfoPtr->ext.bitMapCount;
		theInfoPtr->ext.hRes = 72 << 16;
		theInfoPtr->ext.vRes = 72 << 16;
	}
	theInfoPtr->ext.sourceRect.bottom	= thePixMapPtr->bounds.bottom - thePixMapPtr->bounds.top;
	theInfoPtr->ext.sourceRect.right	= thePixMapPtr->bounds.right - thePixMapPtr->bounds.left;
	theInfoPtr->ext.sourceRect.top		= 0;
	theInfoPtr->ext.sourceRect.left		= 0;

/*
|	Check the current port’s version. If the port is a color port, then set the foreground and background
|	colors to black and white respectively.
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
|	Now, call the main “guts” routine that records the real information for the pixMap
*/

	error = RecordPixMapGuts(theInfoPtr, thePixMapHandle);

/*
|	Restore the state of the PictInfoID and return an error if there is one.
*/

returnError:

	HSetState( (Handle)thePictInfoID, oldInfoState );
	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

OSErr RecordPixMapGuts(
register InternalPictInfoPtr	theInfoPtr,
PixMapHandle					thePixMapHandle )
{
	register PixMapPtr		thePixMapPtr;
	register short			width, height;
	register unsigned short	white = 0xFFFF;
	long			theIndexTable[256];
	OSErr			error = noErr;
	short			thePixelSize;
	unsigned short	rowBytes;
	Boolean			isPixMap;
	char			oldPMState, oldCTState;

/*
|	Lock down the pixMap handle after first moving it high.
*/

	oldPMState = HGetState( (Handle)thePixMapHandle );
	if( error = MemError() )
		return error;

	MoveHHi( (Handle)thePixMapHandle );
	HLock( (Handle)thePixMapHandle );

	thePixMapPtr = *thePixMapHandle;

/*
|	Set the “isPixMap” and “thePixelSize” variables and set the maximum depth field is the information handle.
*/

	thePixelSize = 1;
	if( isPixMap = IsPixMap(thePixMapPtr->rowBytes) )
		thePixelSize = thePixMapPtr->pixelSize;

	if( thePixelSize > theInfoPtr->ext.depth )
		theInfoPtr->ext.depth = thePixelSize;

/*
|	Check to see if we are returning color information in any form. If we are not, then
|	exit.
*/

	if( !(theInfoPtr->verb & (returnPalette | returnColorTable)) )
		goto exit;

/*
|	Setup a the variables needed to loop thru each pixel
*/

	height		= thePixMapPtr->bounds.bottom - thePixMapPtr->bounds.top;
	width		= thePixMapPtr->bounds.right  - thePixMapPtr->bounds.left;
	rowBytes	= thePixMapPtr->rowBytes & 0x3FFF;

/*
|	If we have a pixMap, then lock down the color table after moving it high. Note that
|	direct pixMaps do not have a color table, so we don’t lock them down.
*/

	if( isPixMap && (thePixMapPtr->pmTable) )
	{
		oldCTState = HGetState( (Handle)(thePixMapPtr->pmTable) );
		if( error = MemError() )
			goto exit;

		MoveHHi( (Handle)(thePixMapPtr->pmTable) );
		HLock( (Handle)(thePixMapPtr->pmTable) );
	}

/*
|	If the foreground color is white and the background color is black, then don’t colorize. Otherwise, colorize
|	each RGBColor, using the stored foreground and background color.
*/

	theInfoPtr->colorizing = true;
	if( (theInfoPtr->foreColor.red == 0x0000) && (theInfoPtr->foreColor.green == 0x0000) && (theInfoPtr->foreColor.blue == 0x0000) &&
		(theInfoPtr->backColor.red == white) && (theInfoPtr->backColor.green == white) && (theInfoPtr->backColor.blue == white) )
	{
		theInfoPtr->colorizing = false;
	}

/*
|	If the pixMap is indexed, then make an index cache that stores the offset into the histogram for each value
|	of the index. Then call an assembly routine that records all the pixels into the color bank.
*/

	if( thePixelSize <= 8 )
	{
		theInfoPtr->colorTableEntries	= 1 << thePixelSize;
		theInfoPtr->indexCachePtr		= theIndexTable;
		MakeTrueColorTable(theInfoPtr, ( (isPixMap) ? thePixMapPtr->pmTable : 0), thePixelSize);

		MakeIndexCache(theInfoPtr);
	}
	else
	{
		theInfoPtr->indexCachePtr = nil;
		ConvertBankTo555(theInfoPtr);
	}

	RecordPixels(theInfoPtr, width, height, rowBytes, (long)(thePixMapPtr->baseAddr), thePixelSize);

	theInfoPtr->indexCachePtr = nil;

	if( (theInfoPtr->colorBankType == ColorBankIsCustom) && (theInfoPtr->colorBankIndex > 0) )
	{
		RecordColors( theInfoPtr->pickProcData, (RGBColor *)(theInfoPtr->colorBankBuffer.ptr), (long)(theInfoPtr->colorBankIndex), &(theInfoPtr->ext.uniqueColors), theInfoPtr->colorPickMethod );
		theInfoPtr->colorBankIndex = 0;
	}

/*
|	Restore the state of the locked handles and return the error.
*/

	if( isPixMap && (thePixMapPtr->pmTable) )
		HSetState( (Handle)thePixMapPtr->pmTable, oldCTState );

exit:
	HSetState( (Handle)thePixMapHandle, oldPMState );
	return error;
}

/*----------------------------------------------------------------------------------------------------------*/

void ConvertBankTo555(
register InternalPictInfoPtr	theInfoPtr )
{
	register short		index;
	register ColorSpec	*theColorPtr;
	register short		*countPtr;
	ColorSpec			*maxColorPtr;
	ColorSpec			tempBuffer[maximumExactColors];

	if( theInfoPtr->colorBankType == ColorBankIsExactAnd555 )
	{
		theInfoPtr->colorBankType = ColorBankIs555;

		BlockMove( theInfoPtr->colorBankBuffer.ptr, (char *)tempBuffer, (sizeof(ColorSpec) * maximumExactColors) );
		ClearMemoryBytes( theInfoPtr->colorBankBuffer.ptr, HistogramTableSize );

		theColorPtr = tempBuffer;
		maxColorPtr = theColorPtr + theInfoPtr->ext.uniqueColors;
		theInfoPtr->ext.uniqueColors = 0;
		while( theColorPtr < maxColorPtr )
		{
			index  = (theColorPtr->rgb.red   >>  1) & 0x7C00;
			index += (theColorPtr->rgb.green >>  6) & 0x03E0;
			index += (theColorPtr->rgb.blue  >> 11);

			countPtr = (short *)(theInfoPtr->colorBankBuffer.ptr) + index;
			if( *countPtr == 0 )
				theInfoPtr->ext.uniqueColors++;
			*countPtr += theColorPtr->value;

			++theColorPtr;
		}
		if (theInfoPtr->verb & suppressBlackAndWhite) {
			countPtr = (short *)(theInfoPtr->colorBankBuffer.ptr);
			if (*countPtr != 0) {
				*countPtr = 0;
				theInfoPtr->ext.uniqueColors--;
			}
			countPtr += 32767;
			if (*countPtr != 0) {
				*countPtr = 0;
				theInfoPtr->ext.uniqueColors--;
			}
		}
	}
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	Note that this routine will never be called with colorsRequested less than uniqueColors.
*/

void CalcExactTable(
register short		colorsRequested,
register short		uniqueColors,
register ColorSpec	*colorBankPtr,
register ColorSpec	*resultPtr )
{
	register short	value = 0;

	colorsRequested -= uniqueColors;

	while( --uniqueColors >= 0 )
	{
		resultPtr->value	= value++;
		resultPtr->rgb		= colorBankPtr->rgb;

		++colorBankPtr;
		++resultPtr;
	}

	while( --colorsRequested >= 0 )
	{
		resultPtr->value		= value++;
		resultPtr->rgb.red		= 0;
		resultPtr->rgb.green	= 0;
		resultPtr->rgb.blue		= 0;

		++resultPtr;
	}
}

/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************/
/****										Internal Functions Follow									 ****/
/************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/

static void MakeTrueColorTable(
register InternalPictInfoPtr	theInfoPtr,
CTabHandle		theCTHandle,
short			thePixelSize )
{
	ColorSpec	*srcColorPtr;
	register ColorSpec	*dstColorPtr;
	register short		index;
	register short		entryCount;
	register short		colorCount = 0;
	short				value = 0;
	RGBColor			combo;
	CTabPtr				theCTPtr;
	short				relativeEntries;
	char				oldCTState;

	entryCount		= 1 << thePixelSize;

	dstColorPtr = (ColorSpec *)(theInfoPtr->colorTableBuffer.ptr);
	for(index = 0; index < entryCount; ++index)
	{
		dstColorPtr->value = -1;
		++dstColorPtr;
	}

	if( theCTHandle )
	{
		oldCTState = HGetState( (Handle)theCTHandle );
		HLock( (Handle)theCTHandle );
		theCTPtr = *theCTHandle;

		colorCount	= theCTPtr->ctSize + 1;
		srcColorPtr	= &(theCTPtr->ctTable);

		combo.red	= theInfoPtr->foreColor.red ^ theInfoPtr->backColor.red;
		combo.green	= theInfoPtr->foreColor.green ^ theInfoPtr->backColor.green;
		combo.blue	= theInfoPtr->foreColor.blue ^ theInfoPtr->backColor.blue;

		while( colorCount-- )
		{
		/*
		|	The index into the color table should default to the value in the ColorSpec. However, if the
		|	color table flag word has it’s highest bit set, then we need to ignore the value field and
		|	use a constantly incrementing index.
		*/

			index = srcColorPtr->value;
			if( theCTPtr->ctFlags < 0 )
				index = value++;

			dstColorPtr = (ColorSpec *)(theInfoPtr->colorTableBuffer.ptr);
			dstColorPtr += index;

			dstColorPtr->value	= 0;

			if( theInfoPtr->colorizing )
			{
				dstColorPtr->rgb.red	= (srcColorPtr->rgb.red & combo.red) ^ theInfoPtr->foreColor.red;		/*<15>*/
				dstColorPtr->rgb.green	= (srcColorPtr->rgb.green & combo.green) ^ theInfoPtr->foreColor.green;	/*<15>*/
				dstColorPtr->rgb.blue	= (srcColorPtr->rgb.blue & combo.blue) ^ theInfoPtr->foreColor.blue;	/*<15>*/
			}
			else
				dstColorPtr->rgb = srcColorPtr->rgb;

			++srcColorPtr;
		}

		colorCount = theCTPtr->ctSize + 1;
		HSetState( (Handle)theCTHandle, oldCTState );
	}

	if( colorCount < entryCount )
	{
		Fixed	theRed, theGreen, theBlue;
		Fixed	incRed, incGreen, incBlue;

		relativeEntries = entryCount - colorCount;

		theRed = theInfoPtr->backColor.red;
		incRed = theInfoPtr->foreColor.red - theRed;
		if( incRed < 0 )
		{
			theRed = theInfoPtr->foreColor.red;
			incRed = -incRed;
		}

		theGreen = theInfoPtr->backColor.green;
		incGreen = theInfoPtr->foreColor.green - theGreen;
		if( incGreen < 0 )
		{
			theGreen = theInfoPtr->foreColor.green;
			incGreen = -incGreen;
		}

		theBlue = theInfoPtr->backColor.blue;
		incBlue = theInfoPtr->foreColor.blue - theBlue;
		if( incBlue < 0 )
		{
			theBlue = theInfoPtr->foreColor.blue;
			incBlue = -incBlue;
		}

		theRed	 = (theRed << 16) + 0x8000;
		theGreen = (theGreen << 16) + 0x8000;
		theBlue	 = (theBlue << 16) + 0x8000;

		incRed	 = FixRatio( (short)incRed, relativeEntries );
		incGreen = FixRatio( (short)incGreen, relativeEntries );
		incBlue	 = FixRatio( (short)incBlue, relativeEntries );

		dstColorPtr = (ColorSpec *)(theInfoPtr->colorTableBuffer.ptr);
		for(index = 0; index < relativeEntries; ++index)				/* <18> relativeEntries was entryCount */
		{
			register unsigned long	temp;

			if( dstColorPtr->value )
			{
				dstColorPtr->rgb.red	= theRed >> 16;
				dstColorPtr->rgb.green	= theGreen >> 16;
				dstColorPtr->rgb.blue	= theBlue >> 16;

				temp = (unsigned long)theRed;
				theRed += incRed;
				if( (unsigned long)theRed < temp )
					theRed = 0xFFFF0000;

				temp = (unsigned long)theGreen;
				theGreen += incGreen;
				if( (unsigned long)theGreen < temp )
					theGreen = 0xFFFF0000;

				temp = (unsigned long)theBlue;
				theBlue += incBlue;
				if( (unsigned long)theBlue < temp )
					theBlue = 0xFFFF0000;
			}
			++dstColorPtr;
		}
	}
}

/*----------------------------------------------------------------------------------------------------------*/

void MakeIndexCache(
register InternalPictInfoPtr	theInfoPtr )
{
	register ColorSpec		*theColorPtr;
	register long			*indexCachePtr;
	register unsigned short	index;
	register short			colorCount;
	register short			exactBankFlag;
	register short			customBankFlag;

/*
|	Setup the variables that we are going to need.
*/

	indexCachePtr	= theInfoPtr->indexCachePtr;
	colorCount		= theInfoPtr->colorTableEntries;
	theColorPtr		= (ColorSpec *)(theInfoPtr->colorTableBuffer.ptr);

	exactBankFlag	= theInfoPtr->colorBankType == ColorBankIsExactAnd555;
	customBankFlag	= theInfoPtr->colorBankType == ColorBankIsCustom;

/*
|	Loop thru the entire color table, making an entry in the index cache for each color.
*/

	while( colorCount-- )
	{
		if(	(theInfoPtr->verb & suppressBlackAndWhite) &&
			(theColorPtr->rgb.red == theColorPtr->rgb.green) &&
			(theColorPtr->rgb.red == theColorPtr->rgb.blue) &&
			((theColorPtr->rgb.red == 0x0000) || (theColorPtr->rgb.red == 0xFFFF)) )
		{
			*(indexCachePtr++) = -2;
		}
		else
		{
			if( exactBankFlag )
			{
				ColorSpec	*colorBankPtr;

			/*
			|	If the color bank is an array of exact colors, then scan thru all these colors and see if any of them
			|	match the color we are trying to cache. If there is already a color in the bank, then set the index
			|	cache entry to the offset into the color bank of this color. Otherwise, set the cache entry to -1.
			*/

				colorBankPtr = (ColorSpec *)(theInfoPtr->colorBankBuffer.ptr);

				for(index = theInfoPtr->ext.uniqueColors; index > 0; index--)
				{
					if( colorBankPtr->rgb == theColorPtr->rgb )
					{
						*(indexCachePtr++) = (long)colorBankPtr - (long)(theInfoPtr->colorBankBuffer.ptr);
						break;
					}

					++colorBankPtr;
				}

				if( index == 0 )
					*(indexCachePtr++) = -1;
			}
			else if( customBankFlag )
			{
				*(indexCachePtr++) = -3;
			}
			else
			{
			/*
			|	If the color bank is a 555 histogram, then calculate the index into the histogram for this color.
			|	Then check to see if we are suppressing black and white, and if we are AND the color is black
			|	or white, then set the index cache index to -2.
			*/

				index  = (theColorPtr->rgb.red   >>  1) & 0x7C00;
				index += (theColorPtr->rgb.green >>  6) & 0x03E0;
				index += (theColorPtr->rgb.blue  >> 11);

				*(indexCachePtr++) = (unsigned long)(index) << 1;
			}
		}

		++theColorPtr;
	}
}

/*----------------------------------------------------------------------------------------------------------*/
