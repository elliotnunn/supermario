/*
	File:		puMedian.c

	Contains:	This is the median color finding algorithm for the picture utilities package.

	Written by:	Dave Good. Some ideas stolen from Konstantin Othmer and Bruce Leak. Algorithm by Keith McGreggor.

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<11>	  2/6/91	SMC		DDG,#81681: Removed un-uncommented code in box splitting
									routine, since it is never executed (or necessary).
		<10>	  2/4/91	SMC		KON,#81681: Un-uncommented code in box splitting routine to
									deal with a box filled with more than copy of the same color.
		 <9>	  2/4/91	SMC		KON,#81589: Fixed the median box splitting code. It didn't
									seem to want to break a box on the blue axis.
		 <8>	12/17/90	KON		<with DDG> Made the median color algorithm return better results
									by splitting the box with the largest spread, instead of
									splitting every box that we find.
		 <7>	 9/21/90	DDG		Made changes from code review. Fixed a possible stack overflow
									problem, by allocating the boxInfo array as a buffer, instead of
									a stack array. Minor changes to reflect the new interface to the
									buffering routines.
		 <6>	 8/16/90	DDG		Cleaned up the comments.
		 <5>	  8/5/90	DDG		Added a parameter to the RecordMedianColors routine. This
									routine should still never be called.
		 <4>	 7/31/90	DDG		Fixed a bug in the RecordMedianColors routine. Note that this
									routine should never be called, but better safe than sorry.
		 <3>	 7/30/90	DDG		Added init, record, and kill routines to support the generic
									colorPickMethod model. Also fixed a few bugs.
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

/*
|	Private Defines
*/

#define noBoxToSplit	-1


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Private Data Structures
*/

typedef struct BoxInfo
{
	unsigned short	count;

	unsigned short	minRed;
	unsigned short	maxRed;
	unsigned short	minGreen;
	unsigned short	maxGreen;
	unsigned short	minBlue;
	unsigned short	maxBlue;

	unsigned long	totalRed;
	unsigned long	totalGreen;
	unsigned long	totalBlue;

	unsigned long	padding1;
	unsigned short	padding2;

} BoxInfo;


typedef struct RangeInfo
{
	unsigned short	spread;
	unsigned short	start;
	unsigned short	end;
	unsigned short	step;

} RangeInfo;


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Private Function Declarations
*/

void	SplitBox( BoxInfo *, short *, short, short );

void	FillMedianTable( BoxInfo *, ColorSpec *, short );


/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************/
/****												Functions Follow									 ****/
/************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine is called to initialize the median method. It doesn’t really do anything, except tell
|	the main picture utilities package that it should store the colors in the ExactAnd555 format.
*/

pascal OSErr InitMedianMethod(
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
|	This routine is called to kill the median method. It doesn’t do anything.
*/

pascal OSErr KillMedianMethod(
long	dataHandle	)
{
	#pragma unused( dataHandle )

	return noErr;
}

/*----------------------------------------------------------------------------------------------------------*/

/*
|	This routine should never be called, so it returns an error.
*/

pascal OSErr RecordMedianColors(
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
|	This returns the colors that cover the widest range in the histogram.
*/

pascal OSErr CalcMedianTable(
long		dataHandle,
short		colorsRequested,
short		*colorBankPtr,
ColorSpec	*resultPtr	)
{
	#pragma unused( dataHandle )

	register BoxInfo	*boxInfoPtr;
	register short		*boxIDPtr;
	register short		index;
	Buffer		boxIDBuffer;
	Buffer		boxInfoBuffer;
	short		colorsToFill;
	OSErr		error;

/*
|	Allocate two buffers, one to hold a box ID number for each color in the histogram and the second one to hold
|	the actual box information (component range, etc). Then make “boxInfoPtr” a pointer to the first boxInfo in
|	the boxInfo buffer. This variable won’t get reset until after the main loop.
*/

	if( error = NewBufferClear( &boxIDBuffer, HistogramTableSize, bufferFixedType ) )
		return error;

	if( error = NewBufferClear( &boxInfoBuffer, 256 * sizeof(BoxInfo), bufferFixedType ) )
		return error;

	boxInfoPtr = (BoxInfo *)(boxInfoBuffer.ptr);

/*
|	Mark all the entries in the histogram as either being in box zero (if they have at least one color) or -1 if
|	they had no colors. This will also give us the total number of colors in the histogram, which we must make
|	sure to clip to the number of colors requested to get the number of colors to fill.
*/

	{
		register long	histogramEnd;
		short			*histogramPtr;

		boxIDPtr	 = (short *)(boxIDBuffer.ptr);
		histogramPtr = colorBankPtr;
		histogramEnd = (long)colorBankPtr + HistogramTableSize;

		while( histogramPtr < (char *)histogramEnd )
		{
			if( *(histogramPtr++) > 0 ) 
			{
				*(boxIDPtr++) = 0;
				boxInfoPtr->count++;
			}
			else
				*(boxIDPtr++) = -1;
		}
	}

	colorsToFill = (boxInfoPtr->count > colorsRequested) ? colorsRequested : boxInfoPtr->count;

/*
|	Find the minimum and maximum range for each component of all the colors in box zero. Since all the colors are
|	in box zero to start with, this finds the range for all the colors in the histogram. Note that we must
|	initialize the minimum values for each box, but not the maximum ones. This is because we have cleared the
|	boxInfo buffer, so that our maximum values are already initialized to zero for us. boxInfoPtr continues to
|	point to the BoxInfo structure for block zero.
|
|	Note that we declare a separate block to allow us to registerize the our component indexes.
*/

	boxIDPtr = (short *)(boxIDBuffer.ptr);

	{
		register unsigned short	red, green, blue;

		boxInfoPtr->minRed = boxInfoPtr->minGreen = boxInfoPtr->minBlue = 0xFFFF;

		for(red = 0; red < 32; red++)
		{
			for(green = 0; green < 32; green++)
			{
				for(blue = 0; blue < 32; blue++)
				{
					if( *(boxIDPtr++) == 0 )
					{
						if(   red < boxInfoPtr->minRed )	boxInfoPtr->minRed	 = red;
						if( green < boxInfoPtr->minGreen )	boxInfoPtr->minGreen = green;
						if(  blue < boxInfoPtr->minBlue )	boxInfoPtr->minBlue	 = blue;

						if(   red > boxInfoPtr->maxRed )	boxInfoPtr->maxRed	 = red;
						if( green > boxInfoPtr->maxGreen )	boxInfoPtr->maxGreen = green;
						if(  blue > boxInfoPtr->maxBlue )	boxInfoPtr->maxBlue	 = blue;
					}
				}
			}
		}
	}

/*
|	We scan thru all the filled boxes multiple times, each time looking for the one that has the largest spread
|	in either red, green or blue. Note that we only look at boxes that have more than one color, because we are
|	going to split the box that we find into two sub-boxes. After we split the box, we loop back and scan thru
|	all the filled boxes again (this time there is one more filled box than last time. We terminate both loops,
|	when we have reached the number of colors to fill. Note that since we clipped colorsToFill to the total number
|	of colors in the histogram, we will never be in a situation where we can’t split a box, but we haven’t used
|	all our colors yet.
|
|	Note that we declare a separate block to allow us to registerize some other variables.
*/

	{
		register short availableBox;
		register short filledBoxes;

		filledBoxes = availableBox = 1;

		while( true /* filledBoxes < 256 */ )
		{
			register short theSpread;
			register short spreadMax;
			short maxIndex;

			if( availableBox >= colorsToFill )
				goto exitMain;

			spreadMax = maxIndex = 0;
			for(index = 0; (index < filledBoxes) && (spreadMax != 0x1F); index++)
			{
				boxInfoPtr = (BoxInfo *)( boxInfoBuffer.ptr + (index * sizeof(BoxInfo)) );

				if( boxInfoPtr->count > 1 )
				{
					theSpread = boxInfoPtr->maxRed - boxInfoPtr->minRed;
					if( theSpread > spreadMax )
					{
						spreadMax = theSpread;
						maxIndex = index;
					}
					theSpread = boxInfoPtr->maxGreen - boxInfoPtr->minGreen;
					if( theSpread > spreadMax )
					{
						spreadMax = theSpread;
						maxIndex = index;
					}
					theSpread = boxInfoPtr->maxBlue - boxInfoPtr->minBlue;
					if( theSpread > spreadMax )
					{
						spreadMax = theSpread;
						maxIndex = index;
					}
				}
			}

			SplitBox((BoxInfo *)(boxInfoBuffer.ptr), (short *)(boxIDBuffer.ptr), maxIndex, availableBox++);
			filledBoxes = availableBox;
		}
	}

exitMain:

/*
|	Calculate the totals for each color component for all colors contained in each of the boxes. We first loop
|	thru each color checking to see which box it belongs to. We then get a pointer to the BoxInfo for that
|	particular box and for each component, we add the color to the appropriate total.
|
|	Note that we declare a separate block to allow us to registerize the our component indexes.
*/

	boxIDPtr = (short *)(boxIDBuffer.ptr);

	{
		register unsigned short	red, green, blue;

		for(red = 0; red < 32; red++)
		{
			for(green = 0; green < 32; green++)
			{
				for(blue = 0; blue < 32; blue++)
				{
					if( (index = *(boxIDPtr++)) >= 0 )
					{
						boxInfoPtr = (BoxInfo *)( boxInfoBuffer.ptr + (index * sizeof(BoxInfo)) );
						boxInfoPtr->totalRed	+= red;
						boxInfoPtr->totalGreen	+= green;
						boxInfoPtr->totalBlue	+= blue;
					}
				}
			}
		}
	}

/*
|	Here is where we actually fill the color table that we are returning. We scan thru all the boxes in the
|	boxInfo buffer, checking to see if their count is greater than zero. If it is, then we have a color to return,
|	so for each component, we divide the total by the number of colors in this box to get the average component,
|	then we smear the component out to fill a full 16-bit range for the color table. If the count for a particular
|	color is zero, then we return black for that entry in the color table. Note that this will never happen until
|	we have reached the end of the filled boxes, so at that point all we really want to do is fill the rest of the
|	returned color table with black.
*/

	FillMedianTable( (BoxInfo *)(boxInfoBuffer.ptr), resultPtr, colorsRequested );

/*
|	Kill the buffers for the boxes and the boxIDs
*/

	KillBuffer( &boxInfoBuffer );
	KillBuffer( &boxIDBuffer );

	return noErr;
}

/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/************************************************************************************************************/
/****										Internal Functions Follow									 ****/
/************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/

static void SplitBox(
BoxInfo		*boxInfoPtr,
short		*mainBoxIDPtr,
short		srcBox,
short		dstBox	)
{
	RangeInfo	first, second, third;
	BoxInfo		*slowSrcPtr, *slowDstPtr;
	register unsigned short	oneHalfCount;

/*
|	Make pointers to the two boxes; the source box that we are taking colors from and the dest box
|	that we are adding the taken colors to. These pointers are stored in non register locals, so
|	that we can reload fast register pointers quickly.
*/

	slowSrcPtr = boxInfoPtr + srcBox;
	slowDstPtr = boxInfoPtr + dstBox;

/*
|	The second box has a count that is equal to one half of the source box. Note that we subtract
|	the number of colors we removed from the source box’s count to ensure that even if the count is
|	odd, we will not lose a color.
|
|	We use a separate block in order to get register variables for our two box pointers (srcBoxPtr
|	and dstBoxPtr).
*/

	{
		register BoxInfo	*srcBoxPtr = slowSrcPtr;
		register BoxInfo	*dstBoxPtr = slowDstPtr;

		dstBoxPtr->count = oneHalfCount = srcBoxPtr->count >> 1;
		srcBoxPtr->count -= oneHalfCount;

	/*
	|	Here we setup the three range blocks. This is some very tricky code that allows us to sort the
	|	component range blocks in order of the spread between the minimum and maximum values of the
	|	components. The variables that we need for each of the range blocks are the spread between the
	|	values of the components, the starting index (the minimum component value shifted over by the
	|	proper amount), the ending index (the maximum component value shifted over by the proper amount),
	|	and the step (the amount to increment the component index by each time thru the loop.
	*/

		first.spread	= srcBoxPtr->maxRed - srcBoxPtr->minRed;
		first.start		= (srcBoxPtr->minRed) << 10;
		first.end		= (srcBoxPtr->maxRed) << 10;
		first.step		= 1024;
	
		second.spread	= srcBoxPtr->maxGreen - srcBoxPtr->minGreen;
		second.start	= (srcBoxPtr->minGreen) << 5;
		second.end		= (srcBoxPtr->maxGreen) << 5;
		second.step		= 32;
	
		third.spread	= srcBoxPtr->maxBlue - srcBoxPtr->minBlue;
		third.start		= srcBoxPtr->minBlue;
		third.end		= srcBoxPtr->maxBlue;
		third.step		= 1;
	}

/*
|	Here is where we sort the range blocks in order of the spread between the components. Notice that
|	since we have a RangeInfo structure for each, we only need to swap pointers to swap two elements
|	in this structure. Also notice that we don’t have to actually swap the pointers, since we know
|	the value of at least one of them at all times.
|
|	We need a separate code block again so that we can registerize the pointers to each of our range
|	info structures. We also registerize some other frequently used variables.
*/

	{
		register RangeInfo	*firstPtr, *secondPtr, *thirdPtr;
				 RangeInfo	*swapPtr;							/*<9>*/
		register unsigned short	firstIndex, secondIndex, index;

		firstPtr	= &first;
		secondPtr	= &second;
		thirdPtr	= &third;

		if( secondPtr->spread > firstPtr->spread )
		{
			secondPtr	= firstPtr;
			firstPtr	= &second;
		}
	
		if( thirdPtr->spread > secondPtr->spread )
		{
			thirdPtr	= secondPtr;
			secondPtr	= &third;
	
			if( secondPtr->spread > firstPtr->spread )
			{
				swapPtr		= firstPtr;							/*<9>*/
				firstPtr	= secondPtr;						/*<9>*/
				secondPtr	= swapPtr;							/*<9>*/
			}
		}

	/*
	|	This is where we actually use these range blocks that we set up and sorted . We look through
	|	all the colors that are within this defined range and we set half of them to be in the dest
	|	box instead of the src box. Note that mainBoxIDPtr is not a register; it would be nice if
	|	it was, but it is actually less important than the other variables.
	*/
	
		firstIndex = firstPtr->start;
		while( firstIndex <= firstPtr->end )
		{
			secondIndex = secondPtr->start;
			while( secondIndex <= secondPtr->end )
			{
				index = firstIndex + secondIndex + thirdPtr->start;
				while( index <= (firstIndex + secondIndex + thirdPtr->end) )
				{
					if( mainBoxIDPtr[index] == srcBox )
					{
						mainBoxIDPtr[index] = dstBox;
						if( --oneHalfCount == 0 )
							goto exitLoop;
					}
					index += thirdPtr->step;
				}
				secondIndex += secondPtr->step;
			}
			firstIndex += firstPtr->step;
		}
	}

exitLoop:

/*
|	We have now divided the original box into two parts. Now all we need to do is find the range of
|	each of the components for both the original source box and the new destination box.
|
|	Again, we have a new code block so that we can declare a completely different set of register
|	variables. This time the src and dst box pointers are very important. The indexes for our loops
|	are also important.
*/

	{
		BoxInfo	initialBox;
		register BoxInfo		*srcBoxPtr = slowSrcPtr;
		register BoxInfo		*dstBoxPtr = slowDstPtr;
		register short			*boxIDPtr;
		register unsigned short	red, green, blue;			/*<9>*/
		register short			boxIndex;

	/*
	|	Copy the initial box structure (particularly the range information) into our local structure.
	|	This is because we need the initial values in the source box structure for our loop ending
	|	conditions, but we want to be able to fill the new source box structure directly in the loop.
	*/

		initialBox = *srcBoxPtr;

	/*
	|	Initialize the minimum and maximum fields of the src and dst box structures.
	*/

		srcBoxPtr->minRed = srcBoxPtr->minGreen = srcBoxPtr->minBlue = 0xFFFF;
		dstBoxPtr->minRed = dstBoxPtr->minGreen = dstBoxPtr->minBlue = 0xFFFF;

		srcBoxPtr->maxRed = srcBoxPtr->maxGreen = srcBoxPtr->maxBlue = 0x0000;
		dstBoxPtr->maxRed = dstBoxPtr->maxGreen = dstBoxPtr->maxBlue = 0x0000;

	/*
	|	This is where we loop thru all the colors in the old range, checking them against either the
	|	source or destination box structure’s minimums and maximums. Note that boxIDPtr is a pointer
	|	into the boxID buffer that tells which box each color belongs to. We can increment it by one
	|	as we move thru the blue component, but before each blue loop, we need to set it up properly.
	*/

		for( red = initialBox.minRed; red <= initialBox.maxRed; red++ )
		{
			for( green = initialBox.minGreen; green <= initialBox.maxGreen; green++ )
			{
				boxIDPtr = mainBoxIDPtr + (red << 10) + (green << 5) + initialBox.minBlue;
				for( blue = initialBox.minBlue; blue <= initialBox.maxBlue; blue++ )
				{
					boxIndex = *(boxIDPtr++);
					if( boxIndex == srcBox )
					{
						if(   red < srcBoxPtr->minRed )		srcBoxPtr->minRed	= red;
						if( green < srcBoxPtr->minGreen )	srcBoxPtr->minGreen	= green;
						if(  blue < srcBoxPtr->minBlue )	srcBoxPtr->minBlue	= blue;

						if(   red > srcBoxPtr->maxRed )		srcBoxPtr->maxRed	= red;
						if( green > srcBoxPtr->maxGreen )	srcBoxPtr->maxGreen	= green;
						if(  blue > srcBoxPtr->maxBlue )	srcBoxPtr->maxBlue	= blue;
					}
					else if( boxIndex == dstBox )
					{
						if(   red < dstBoxPtr->minRed )		dstBoxPtr->minRed	= red;
						if( green < dstBoxPtr->minGreen )	dstBoxPtr->minGreen	= green;
						if(  blue < dstBoxPtr->minBlue )	dstBoxPtr->minBlue	= blue;
	
						if(   red > dstBoxPtr->maxRed )		dstBoxPtr->maxRed	= red;
						if( green > dstBoxPtr->maxGreen )	dstBoxPtr->maxGreen	= green;
						if(  blue > dstBoxPtr->maxBlue )	dstBoxPtr->maxBlue	= blue;
					}
				}
			}
		}
	}
}

/*----------------------------------------------------------------------------------------------------------*/						
