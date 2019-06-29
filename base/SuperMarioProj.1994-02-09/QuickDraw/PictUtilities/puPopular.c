/*
	File:		puPopular.c

	Contains:	This is the popular color finding algorithm for the picture utilities package.

	Written by:	Dave Good. Some ideas stolen from Konstantin Othmer and Bruce Leak. Algorithm by Keith McGreggor.

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 9/21/90	DDG		Made changes from code review. Minor changes to reflect the new
									interface to the buffering routines.
		 <6>	 8/16/90	DDG		Cleaned up the comments and optomized the CalcPopularTable
									routine to save some bytes.
		 <5>	  8/5/90	DDG		Added a parameter to the RecordPopularColors routine. This
									routine should still never be called.
		 <4>	 7/31/90	DDG		Fixed a bug in the RecordPopularColors routine. Note that this
									routine should never be called, but better safe than sorry.
		 <3>	 7/30/90	DDG		Added stub init, record, and kill routines to support the new
									extensible colorPickerMethods. Also fixed a few bugs.
		 <2>	 7/29/90	DDG		Fixed header.
		 <1>	 7/29/90	DDG		First checked in using new structure.

	To Do:
*/


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Includes
*/

#include "puPrivate.h"


/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************/
/****												Functions Follow									 ****/
/************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine is called to initialize the popular method. It doesn’t really do anything, except tell
|	the main picture utilities package that it should store the colors in the ExactAnd555 format.
*/

pascal OSErr InitPopularMethod(
short	colorsRequested,
long	*dataHandlePtr,
short	*colorBankTypePtr	)
{
	#pragma unused( colorsRequested )

	*dataHandlePtr		= nil;
	*colorBankTypePtr	= ColorBankIsExactAnd555;

	return noErr;
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine is called to kill the popular method. It doesn’t do anything.
*/

pascal OSErr KillPopularMethod(
long	dataHandle	)
{
	#pragma unused( dataHandle )

	return noErr;
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine should never be called, so it returns an error.
*/

pascal OSErr RecordPopularColors(
long		dataHandle,
RGBColor	*colorPtr,
long		colorCount,
long		*uniqueColorsPtr	)
{
	#pragma unused( dataHandle, colorPtr, colorCount, uniqueColorsPtr )

	return cantLoadPickMethodErr;
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This returns the colors that occur most frequently in the histogram. In the case of a tie, the color with
|	the higher RGB value is arbitrarily chosen.
*/

pascal OSErr CalcPopularTable(
long				dataHandle,
short				colorsRequested,
short				*colorBankPtr,
register ColorSpec	*resultPtr	)
{
	#pragma unused( dataHandle )

	register short			*histogramPtr;
	register unsigned short	index;
	register short			threshold;
	register long			ignoreCount;
	short					*countBufferPtr;
	Buffer					countBuffer;
	unsigned short			value = 0;
	short					uniqueColors;
	OSErr					error;

/*
|	Allocate a buffer to hold a count of our counts. If we get the buffer, then clear it.
*/

	if( error = NewBufferClear( &countBuffer, HistogramTableSize, bufferFixedType ) )
		return error;

	countBufferPtr = (short *)countBuffer.ptr;

/*
|	Scan thru the histogram getting a "count of counts". Thus if there were two colors that each had 20 pixels
|	in the image using them, we would put 2 in word number 20 of our count buffer. The variable uniqueColors ends
|	up as the total number of UNIQUE colors in the histogram.
*/

	uniqueColors	= 0;
	histogramPtr	= (short *)colorBankPtr;
	for(index = 0; index <= 32767; index++) 
	{
		register unsigned short	colorCount;

		if( (colorCount = *(histogramPtr++)) != 0 )
		{
			uniqueColors++;
			countBufferPtr[colorCount]++;
		}
	}

/*
|	If, there are more colors in the table than we want to return, so do a very clever trick. We scan backwards
|	thru our count buffer, totaling up the occurrances of each of the color counts. Thus if two colors had a count
|	of 32767, and three colors had a count of 32766, then the countSum would start at 2 and then go to 5.
|
|	As soon as the countSum is greater than the number of requested colors, then we have found our threshold.
|
|	We know that there will be at least colorsRequested unique colors that have a equal or higher count than our
|	count threshold. This means that we can ignore all colors in the histogram that have a lower count than our
|	threshold. We can always put colors that have a count higher than our threshold into the returned color table.
|	If the color has a count that is exactly equal to our threshold, then we skip over a certain number of such
|	colors before putting them into the returned color table.
|
|	The number of colors that we skip over is equal to the total number of colors with counts that are greater
|	than or equal to our threshold MINUS the number of requested colors. This prevents us from overflowing the
|	return table and returning too many colors.
|
|	Note that we don’t need to check the index for underflowing, since we know that there are more unique colors
|	than were requested.
|
|	Before we check this, we set the threshold to one and the ignoreCount to zero, so that we can use the same
|	loop, even if we are requesting more colors than there are.
*/

	threshold	= 1;
	ignoreCount	= 0;

	if( uniqueColors > colorsRequested )
	{
		register long	countSum;

		index			= 32767;
		threshold		= index;
		countSum		= 0;

		while( countSum < colorsRequested )
		{
			countSum	+= countBufferPtr[ index ];
			threshold	= index--;
		}

		ignoreCount = countSum - colorsRequested;
	}

/*
|	This is where we actually pull the colors out of the histogram. We make colors the same way as quickdraw does,
|	as described below.
|
|	To make a full 48-bit color, we spread the bits for each of the components evenly over the 16 bit range. For
|	each component, we move the 5 bits to the top of the 16 bit range, and save this value. Then we shift a copy
|	of this value over 5 bits and or it back into the main value. Then we shift a copy of the new main value over
|	10 bits (not 5) and or it with the main value again. The reason why we shift 10 bits instead of five is that
|	we need to fill the remaining bit (bit 0) and we always have to shift in multiples of five so that oring the
|	colors together doesn’t change any of the bits.
|
|	Every time we add a color into the table, we set the color’s value field and increment our value counter.
*/

	histogramPtr = (short *)colorBankPtr;
	for(index = 0; index <= (unsigned short)32767; index++) 
	{
		register unsigned short	component;
		short					count;

		if( (count = *(histogramPtr++)) >= threshold ) 
		{
			if( (count == threshold) && (ignoreCount != 0) )
			{
				ignoreCount--;
				continue;
			}

			resultPtr->value = value++;

			component  = (index & 0x7C00) << 1;
			component |= component >>  5;
			component |= component >> 10;
			resultPtr->rgb.red = component;

			component  = (index & 0x03E0) << 6;
			component |= component >>  5;
			component |= component >> 10;
			resultPtr->rgb.green = component;

			component  = (index & 0x001F) << 11;
			component |= component >>  5;
			component |= component >> 10;
			resultPtr->rgb.blue = component;

			resultPtr++;
		}
	}

/*
|	After we have returned all the colors in the histogram, we need to clear the remaining entries in the table.
|	Note that the loop terminating condition will ensure that we don’t execute this loop if we don’t need to.
*/

	colorsRequested -= uniqueColors;
	while( --colorsRequested >= 0 )
	{
		resultPtr->value		= value++;
		resultPtr->rgb.red		= 0;
		resultPtr->rgb.green	= 0;
		resultPtr->rgb.blue		= 0;
		resultPtr++;
	}

/*
|	Kill the count buffer.
*/

	KillBuffer( &countBuffer );
	return noErr;
}

/*----------------------------------------------------------------------------------------------------------*/
