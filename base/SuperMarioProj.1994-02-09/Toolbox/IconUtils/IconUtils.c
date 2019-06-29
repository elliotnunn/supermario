/*
	File:		IconUtils.c

	Contains:	C laguage portions of color icon plotting utilities

	Written by:	David Collins

	Copyright:	© 1990-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM7>	 12/3/92	RB		Renamed GetClut to IconGetClut to avoid a conflict.
		<78>	11/24/92	KSM		Fix ROMMapInsert setting in CubeE version (77+74 problem).
		<77>	11/23/92	DTY		Make the CubeE part of this file build too.
		<76>	11/13/92	JDR		Updated QuickDraw.h to not use the dangerous pattern type.
		<75>	10/26/92	DTY		Replace explicit references to ExpandMem globals with macro API.
		<74>	 9/03/92	JDR		Changed noMaskFound to noMaskFoundErr. Removed private inline
									version of myGetHandleSize, which must be glue because the
									Memory Manager may return MemErr in D0. Removed a duplicate
									define for RomMapInsert. Removed private inline to NewGestalt.
		<73>	  7/2/92	DC		Sigh, rolled back the cool changes I made to the 7.0 versions of
									IconUtils because the fix I made makes Finder Printing work
									differently.
		<72>	  7/1/92	DC		#1032893 <csd>: Changed the onScreen field of the ParamBlock
									structure to useCopyMask.  The flag is now true if the current
									port has user-defined bottlenecks or is recording to a PICT.  No
									longer overload this flag to mean "always plot 8-bits to a color
									port".  The world is now safe for Tsunami and the bug is fixed.
		<71>	  6/5/92	DC		Added registry of the Gestalt selector. (conditionalized for
									theFuture)
		<70>	 3/20/92	DC		Added UpdateIconGlobals to reinitialize the icon globals in
									response to dynamic changes in the make up of the device list.
									Used GetExpandMem to replace the GetGlobalPtr.
		<69>	 3/19/92	DC		Changed references to printerPortCache in the globalIconData
									structure to a separate ExpandMem reference.
		<68>	10/29/91	DC		Rolled out all post-7.0 changes by one huge conditional for
									Cube-E.
		<67>	 8/30/91	DC		Changed globals from a handle to a ptr (makes code more readable
									and avoids locking and unlocking). Changed the way printing is
									detected to avoid screen plotting problems during background
									printing. Fixed maskRgn to scale back the region to a rectangle
									under low memory conditions.
		<66>	 8/28/91	DC		changed name of ParamBlock to IconParamBlock to avoid confusion
									with HFS, fixed problem with alignment of shrunken icons.  Fixed
									end condition in ApplyPattern loop.
		<65>	 7/15/91	dba		get rid of MemErr, TheGDevice, and fixed up the ShortMul and
									ShortDiv functions
		<64>	 3/11/91	DC		sad, #84447: took out conditionalization so that the bug fix
									will go into 7.0
		<63>	  3/7/91	DC		ngk, #84447: completely factored out 4-bit selection from 4-bit
									labeling in MakeClut. Conditionalized with forPost70
		<62>	  3/5/91	DC		csd,#84160:  Fixed memory moves in the middle of purging icon
									cluts in SetLabel
		<61>	 2/27/91	DC		dba,#DCC004:  Icon cluts made purgeable when built as opposed to
									when the purged handles are created
		<60>	 2/22/91	DC		dba - added initials and bug number reference: worksheet number
									DCC003
		<59>	 2/21/91	DC		Changed transform and hit-test code to use offscreen bitmaps
									instead of region operations. Fixed IconToRgn type code to use
									less stack space.
		<58>	 1/30/91	DC		KSM - Fixed Memory moving bug in DoTransform
		<57>	 1/19/91	DC		KSM - In the case of an SICN without a mask (where I synthesize
									one) the wrong rowBytes is used on the data when generating a
									boundary rectangle. The fix is to use maskMap.baseAddr as the
									source data for boundary generation.
		<56>	 1/16/91	DC		Fix Greg and Kip's previous fix to my potential divide-by-zero
									error
		<55>	 1/14/91	KIP		<gbm> Fix greg's boneheaded mistake.
		<54>	 1/14/91	DC		KSM - Minor performance shore-ups to improve Finder behavior on
									Plus's and SE's
		<53>	 1/11/91	DC		KSM - Fixed 2 bugs in PlotCIconHandle which prevented correct
									drawing of CIcons with no one-bit data (which is legal) and/or
									pixel values outside the range of specified clut entries (also
									legal)
		<52>	12/14/90	DC		KSM - Use true grays wherever possible in 2-bit mode
		<51>	12/13/90	ngk		need to include PalettesPriv to get CheckColors (used to be in
									Palettes)
		<50>	 12/6/90	DC		ngk - Change one bit to color plots to use darken when plotting
									labeled icons. Rolled some color back into 4-bits. (Went a
									little overboard on change <47>)
		<49>	11/29/90	DC		ngk - Fix the crashing bug when a mask with no data is present.
		<48>	11/27/90	DC		KSM - Create some routines needed for menu manager
		<47>	10/30/90	DC		ngk - Fix labeling algorithm to look good in 4-bits and
									gray-scale (if I can)
		<46>	10/24/90	DC		VL - Add a routine to brighten colors before using them to tint
									labeled icons. Rewrite Darken() to be correct and faster.
									Changed MakeClut to cause all colors in a clut to be disabled,
									rather than just Paulien's colors.
		<45>	10/21/90	gbm		(with dba) Made compatible with 3.2 C compiler (and incompatible
									with previous compilers).
		<45>	 10/2/90	JAL		Changing #pragmas to 3.2 version.
		<44>	 9/25/90	DC		Made two fixes for revision 42. One fixed a bug with saving and
									restoring handle states which caused a locked handle not to
									plot. This was stopping Alex Kazim. This change was reviewed by
									Scott Boyd. The other was a minor change to the way black and
									white systems plotted non-transformed icons in an attempt to
									boost performance on the plus and SE. This change was refined in
									in revision 43, but time-tests showed that it was not a win.
									This revision rolls out the change to the black and white draw,
									but keeps the necessary change to the way icon handle states are
									saved and restored (by merging 42 with 41)
		<43>	 9/24/90	DC		Fixing plotting to Black and White Macs that I broke on my last
									fix.
		<42>	 9/23/90	DC		<ngk> Fixed bug with get and SetState using sign extended data
		<41>	 9/21/90	DC		<ksm>Fixed clip region bug.
		<40>	 9/15/90	DC		Rewrote callbacks as assembly, used GetGray in one-bit color
									disabled case
		<39>	 9/15/90	csd		Removed the declaration of DeviceLoop and fixed the call to
									match the interfaces.
		<38>	 9/10/90	DC		Fixed minor bug in 1-bit color quickdraw opened case
		<37>	  9/7/90	DC		Fix up appearance of disabled state
		<36>	  9/6/90	JSM		SetLabel was setting the length of the string one too many.
		<35>	  9/6/90	DC		Merge in new labeling/darkening algorithm
		<34>	  9/1/90	dvb		Add CIconToIconSuite routine
		<33>	 8/29/90	DC		allow one-bit versions without masks.  I.e. ICON
		<32>	 8/22/90	DC		fixed some low-memory condition bugs
		<31>	  8/9/90	DC		Fixed to prevent hanging in icons that have a mask with empty
									data
		<30>	  8/7/90	DC		Temporary fix for bug that caused icons not to plot in low
									memory conditions
		<29>	  8/6/90	DC		added a true gray disabled state for black and white icons on
									color systems (will not degrade correctly yet)
		<28>	  8/2/90	DC		Changed the transfer mode in CopyBits in Render() from srcCopy
									to srcOr (makes 'PICT''s look better)
		<27>	 7/26/90	DC		Fixed some dumb bugs that caused malfunction
		<26>	 7/25/90	DC		Converted some routines to assembly language for efficiency
		<25>	 7/23/90	DC		Added CheckColors, fixed black and white         transforms
		<24>	 7/13/90	dba		fixed C warnings
		<23>	 7/12/90	DC		Fixed bug in 4-bit mode.  Added support for printing and
									offscreen cases.  Started to add support for graceful
									degradation.
		<22>	  7/5/90	DC		Added hit-testing and region-producing
		<21>	 6/29/90	DC		Fix bad resolution values in SetUpPixMap.  Added set up of
									global data and colorized labeling.
		<20>	 6/15/90	DC		Get my cluts from a global now (although its none of your
									business where I get my cluts from)
		<19>	 6/13/90	DC		Fixed 4-bit clut in MakeIconData
		<18>	 6/13/90	DC		Added MakeIconData and associated types
		<17>	  6/7/90	ngk		auto-include quickdraw
	   	<16>	 5/31/90	DC		Make some fixes to icon itteration
		<15>	 5/30/90	DC		Removed DisposeIconCache and changed name of interface from
									IconUtils.h to Icons.h
		<14>	 5/22/90	DC		Made IconCache a subclass of IconSuite.
		<13>	  5/9/90	DC		Use srcOr mode in stead of transparent mode in color plotting
									(faster, more correct)
		<12>	  5/8/90	DC		Fixed alignment problem in icon caches.  improved efficiency of
									loading heuristic
		<11>	  5/4/90	DC		Added many new routines NewIconSuite, ForEachIconDo, etc.
		<10>	 4/18/90	DC		Use typeTable to map IconType values to corresponding ResType
		 <9>	 4/18/90	DC		Fixed drawing in 4-bits and small size
		 <7>	 4/11/90	DC		Changed for interface fixes
		 <6>	 4/10/90	DC		Fixed bug in ploticoncache - might crash if no icon was
									available
		 <5>	 4/10/90	DC		Merged PlotIconCache and PlotIconSuite into one routine -
									PlotBestIcon
		 <4>	  4/9/90	DC		IconCache stuff now is working - isn't life strange and
									wonderful
		 <1>	  4/7/90	DC		first checked in - IconCache stuff not yet working

	To Do:
*/

//	this is a big conditional used to protect Cube-E from all my post-7.0 changes.  I thought
//	this was the safest way to roll back completely to 7.0.  The code in the else clause of this
//	conditional is the entirety (minus header comments) of version 64 of IconUtils.c, which is
//	the version that went into 7.0 final.

#if TheFuture

#ifndef __ICONS__
#include <Icons.h>
#endif

#ifndef __EXPANDMEMPRIV__
#include <ExpandMemPriv.h>
#endif

#ifndef __ERRORS__
#include <Errors.h>
#endif

#ifndef __QUICKDRAW__
#include <QuickDraw.h>
#endif

#ifndef __PALETTES__
#include <Palettes.h>
#endif

#include <IconUtilsPriv.h>
#include <PalettesPriv.h>

#ifndef __MEMORY__
#include <Memory.h>
#endif

#ifndef __RESOURCES__
#include <Resources.h>
#endif

#ifndef __TRAPS__
#include <Traps.h>
#endif

#ifndef __SYSEQU__
#include <SysEqu.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#ifndef __QDOFFSCREEN__
#include <QDOffScreen.h>
#endif

#ifndef __GESTALTEQU__
#include <GestaltEqu.h>
#endif

GrafPtr myGetPort() =
{
	0x2255,				//		MOVEA.L		(A5), A1	;	Get Pointer to globals
	0x2011				//		MOVE.L		(A1), D0	;	Return current grafport
};

ConstPatternParam GetBlack() =
{
	0x70F0,			//	MOVEQ.L		black, D0
	0xD095			//	ADD.L		GrafGlobals(A5), D0
};

ConstPatternParam GetHalfGray() =
{
	0x70E8,			//	MOVEQ.L		gray, D0
	0xD095			//	ADD.L		GrafGlobals(A5), D0
};

ConstPatternParam GetLightGray() =
{
	0x70E0,			//	MOVEQ.L		ltGray, D0
	0xD095			//	ADD.L		GrafGlobals(A5), D0
};

#define ResErr			(*((short *)0xA60))
#define DeviceList		(*((GDHandle *)0x8A8))
#define MainDevice		(*((GDHandle *)0x8A4))
#define	ROM85			(*(unsigned short *) 0x28E)
#define	HaveCQD()		(ROM85 <= 0x3fff)

// These are machine language multiply and divide routines, which are
// used to avoid using CRuntime.o

#pragma parameter __D0 ShortMul(__D1, __D0)
long ShortMul(short, short)
	= 0xC1C1;

#pragma parameter __D0 ShortDiv(__D0, __D1)
short ShortDiv(short, short)
	= 0x81C1;

#define MakeLong(aThing) (*((long *)(&(aThing))))

//	This external table is defined in IconUtils.a

extern ResType typeTable[IconSpace];

//	typedef for call-back used internally for routines like PlotIconGeneric, HitTestIconGeneric, etc.
typedef Handle (*IconRetriever)(long data, IconType theIcon);

//	the folowing is the definition of the parameter block used throughout the code

typedef struct
{
	PixMap			dataMap;		//	PixMap (or BitMap) structure used for icon image data
	BitMap			maskMap;		//	BitMap used for icon mask data
	BitMapPtr		dstMap;			//	Pointer to the destination BitMap
	Handle			theMask,		//	Handle to one-bit data (usually with mask appended)
					theData;		//	Handle to 4 or 8 bit icon image data
	Rect			theRect,		//	destination rectangle (as modified by alignment)
					boundary;		//	boundary of non-zero data within the maskMap bounds.
	short			rowBytes;		//	basic rowBytes of data (2 or 4)
	unsigned short	theTransform,	//	transform broken out from transform code
					theLabel,		//	label broken out from transform code
					selected,		//	selected state broken out from transform code
					dontDraw,		//	flag which is set to true for LoadIconCache, cuts operation short before drawing
					useCopyMask,		//	true if the icon does not need to go through bottlenecks.
					screenDepth,	//	used to record the depth of the device we'll be drawing to
					dataDepth,		//	used to record the depth of the data we'll be drawing
					clutNumber;		//	index into an array of CTabHandles, specifies CLUT we will use with dataMap
	Boolean			useRegions;		//	true if we will use regions to perform transforms, false normally
	IconType		theGroup;		//	size category of icon we are using (mini, small or large) also used to indicate a cicn
	long			userData;		//	data for icon-data-getting call back
	IconRetriever	userProc;		//	ProcPtr to icon-data-getting call back
	CTabHandle		theClut;		//	handle to CLUT we will use with dataMap
	GDHandle		theDevice;		//	device we are currently drawing to or NULL if black and white or if single device
	unsigned long	maskBuffer[32];	//	buffer in which we store masks we create for one reason or another
} IconParamBlock;

#define HorizontalCodes 0xC			//	mask for horizontal positioning codes
#define VerticalCodes 0x3			//	mask for vertical positioning codes

short DivByTwoShort(short theValue)
{
	if (theValue < 0)
		++theValue;

	theValue >>= 1;

	return theValue;
}

//	PerformAlignment offsets theBlock->theRect to cause the icon to have the alignment specified by the caller

pascal void PerformAlignment(IconParamBlock *theBlock, short align)
{
	short	offsetx,
			offsety;

	Rect	*theRect = &(theBlock->theRect),
			theBoundary = theBlock->boundary;

	MapRect(&theBoundary, &(theBlock->dataMap.bounds), theRect);

	switch (align & HorizontalCodes)
	{
		case atHorizontalCenter:
			offsetx = DivByTwoShort((theRect->right + theRect->left) - (theBoundary.right + theBoundary.left));
			break;

		case atLeft:
			offsetx = theRect->left - theBoundary.left;
			break;

		case atRight:
			offsetx = theRect->right - theBoundary.right;
			break;

		default:
			offsetx = 0;
	}

	switch (align & VerticalCodes)
	{
		case atVerticalCenter:
			offsety = DivByTwoShort((theRect->bottom + theRect->top) - (theBoundary.bottom + theBoundary.top));
			break;

		case atTop:
			offsety = theRect->top - theBoundary.top;
			break;

		case atBottom:
			offsety = theRect->bottom - theBoundary.bottom;
			break;

		default:
			offsety = 0;
	}

	OffsetRect(theRect, offsetx, offsety);
}

//	The following routines are used to create regions from mask BitMap's for IconToRgn-type calls and
//	for hit-testing and transforms when an icon is draw larger than 32-bits.  They are faster than the
//	more general BitMapRgn code because they are optimized for a bitMap no greater that a longword in
//	width.

#define TempRgnBufferSize 100

pascal Boolean AddBufferToRgn(RgnHandle theRgn, unsigned short *rgnBuffer, unsigned short *regionPtr)
{
	unsigned long	incrementSize = (unsigned long)regionPtr - (unsigned long)rgnBuffer;
	short			rgnSize = (*theRgn)->rgnSize;

	SetHandleSize((Handle)theRgn, rgnSize + incrementSize);

	if (MemError() != noErr)
		return false;

	BlockMoveData((Ptr)rgnBuffer, (Ptr)(((unsigned char *)(*theRgn)) + rgnSize), incrementSize);

	(*theRgn)->rgnSize  += incrementSize;

	return true;
}

pascal void MaskRgn(RgnHandle		theRgn,
					IconParamBlock	*theBlock)
{
	unsigned char	*data = theBlock->maskMap.baseAddr;		//	This may or may not be locked down

	unsigned short	rowBytes = theBlock->maskMap.rowBytes,
					rowIndex,
					rgnBuffer[TempRgnBufferSize],
					*regionPtr = rgnBuffer,
					spaceLeftInBuffer = TempRgnBufferSize;

	unsigned long	sizeMask = (rowBytes > 2) ? 0xFFFFFFFF : 0xFFFF0000,
					lastRow = 0;

	//	initialize output region
	(*theRgn)->rgnSize = sizeof(Region);
	(*theRgn)->rgnBBox = theBlock->boundary;

	data += ShortMul(rowBytes, theBlock->boundary.top);	//	Point to the top of real data

	//	find the inversion points row-by-row

	for (rowIndex = theBlock->boundary.top;
			rowIndex <= theBlock->boundary.bottom;
			++rowIndex, data += rowBytes)
	{
		unsigned long	thisRow,
						inversion;

		if (rowIndex != theBlock->boundary.bottom)
		{
			thisRow = *((unsigned long *)data);
			inversion = thisRow ^ lastRow;			//	inversion is a bit-map difference between this row and the last
			lastRow = thisRow;
		}
		else
			inversion = lastRow;					//	To finish the region, we compare the last row to nothing (all zeros)

		inversion &= sizeMask;						//	Take only the significant data.

		//	if there is any difference between this row and the last, we describe this difference as a set of inversion points

		if (inversion)
		{
			short columnIndex = 0;

			//	I'm about to add two words to the region (the row index and the first column index, so I make one check to see
			//	if I've got two words left in my buffer.  If I don't, I dump my temporary buffer to the output region and free
			//	up the temp buffer.

			if (spaceLeftInBuffer < 2)
			{
				if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
					goto error;
				regionPtr = rgnBuffer;
				spaceLeftInBuffer = TempRgnBufferSize;
			}

			*regionPtr++ = rowIndex;

			//	find the first inversion point (may be the first point in the row)

			while (!(inversion & 0x80000000))
			{
				++columnIndex;
				inversion <<= 1;
			}

			//	record the first inversion point.

			*regionPtr++ = columnIndex++;

			//	update our gas guage.
			spaceLeftInBuffer -= 2;

			//	find and record all subsequent inversion points

			do
			{
				unsigned long check = inversion & 0xC0000000;

				if (check && (check != 0xC0000000))
				{
					//	check if I need to flush my temp buffer

					if (!spaceLeftInBuffer)
					{
						if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
							goto error;
						regionPtr = rgnBuffer;
						spaceLeftInBuffer = TempRgnBufferSize;
					}

					*regionPtr++ = columnIndex;
					--spaceLeftInBuffer;
				}

				columnIndex++;
				inversion <<= 1;
			} while (inversion);

			//	cap off the row with a row-capping marker.

			if (!spaceLeftInBuffer)
			{
				if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
					goto error;
				regionPtr = rgnBuffer;
				spaceLeftInBuffer = TempRgnBufferSize;
			}

			*regionPtr++ = 0x7FFF;
			--spaceLeftInBuffer;
		}
	}

	if (!spaceLeftInBuffer)
	{
		if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
			goto error;
		regionPtr = rgnBuffer;
		spaceLeftInBuffer = TempRgnBufferSize;
	}

	*regionPtr++ = 0x7FFF;	//	cap off the rgn

	//	If the region is purely rectangular, minimize it and return it.

	if (((*theRgn)->rgnSize == sizeof(Region)) &&
		(((unsigned long)regionPtr - (unsigned long)rgnBuffer) == 18) &&
			(rgnBuffer[1] == rgnBuffer[5]) &&
			(rgnBuffer[2] == rgnBuffer[6]))
	{
		SetHandleSize((Handle)theRgn, sizeof(Region));
		return;
	}

	//	Flush the remainder of the buffer to the region.

	if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
		goto error;

	return;

error:
	//	This handle had to be at least sizeof(Region) to start with and the error occurred
	//	white trying to size it up.  Therefore, SetHandleSize to sizeof(Region) can only
	//	be sizing it down or leaving it unchanged.  It should always work.

	SetHandleSize((Handle)theRgn, sizeof(Region));
	(*theRgn)->rgnSize = sizeof(Region);
}

//-------------------------------------------------------------------------------------
//	Search utilities.
//-------------------------------------------------------------------------------------

//	GetBestData finds the best available data that is appropriate for the destination device.
//	Initially, theBlock->dataDepth is set equal to the screen depth (by SetUpParamBlock).  On
//	exit, theData is set to the best available color data, theClut is set to the appropriate
//	CLUT to use, and dataDepth is updated to reflect the depth of the output data.  If no color
//	data is available or appropriate, theData and theClut are set NULL and dataDepth is set to 1,
//	all of which means that this is a one-bit plot.  GetClut calls CheckClut to see if the clut
//	of the specified depth fits into the current color environments.  It returns false if it is not,
//	causing us to degrade to a smaller bit-depth.

pascal void GetBestData(IconParamBlock *theBlock)
{
	long			theData = theBlock->userData,
					IconGetClut(IconParamBlock *, short);

	short			checkDepth = 8,
					whichIcon = theBlock->theGroup + largeIcon8;	//	whichIcon is initialized to the code
																	//	for the deepest data in our size group

	long			IconGetClut(IconParamBlock *, short);

	IconRetriever	theProc = theBlock->userProc;
	void			MakeClut(IconParamBlock *);

	do {
		if ((theBlock->dataDepth >= checkDepth)
			&& IconGetClut(theBlock, checkDepth)
			&& (theBlock->theData = (*theProc)(theData, whichIcon)))
		{
			theBlock->dataDepth = checkDepth;

			//	if the the clut returned by IconGetClut is purged, build a new one

			if (!(*(theBlock->theClut)))
				MakeClut(theBlock);

			return;
		}

		checkDepth -= 4;
		--whichIcon;
	} while (checkDepth);

	theBlock->dataDepth = 1;
	theBlock->theClut = NULL;
}

//	ChooseBestSize maps a destination rectangle size to an icon size category.

short ChooseBestSize(const Rect *theRect)
{
	int			width = theRect->right - theRect->left,
				height = theRect->bottom - theRect->top;

	if ((width >= 32) || (height >= 32))
		return largeIcon1;

	if (height > 12)
		return smallIcon1;

	return miniIcon1;
}

#define FIXED(a)		(long)(((a)<<16))
#define THEPIXMAP		((PixMapPtr)theBlock)	// only works if the pixmap is the first element

//	SetUpPixMap initializes the necessary fields of dataMap for a color plot.

pascal void SetUpPixMap(IconParamBlock *theBlock)
{
	unsigned short		depth = theBlock->dataDepth;

	THEPIXMAP->baseAddr = *(theBlock->theData);

	//	we use the pixelSize field to indicate whether or not we already have the dataMap
	//	initialized for a color plot, and if so to what depth.

	if (THEPIXMAP->pixelSize == depth)
		return;

	THEPIXMAP->pmVersion = 0;
	THEPIXMAP->packType = 0;
	THEPIXMAP->packSize = 0;
	THEPIXMAP->hRes = FIXED(72);
	THEPIXMAP->vRes = FIXED(72);
	THEPIXMAP->pixelType = 0;
	THEPIXMAP->cmpCount = 1;
	THEPIXMAP->planeBytes = 0;

	THEPIXMAP->pixelSize = depth;
	THEPIXMAP->cmpSize = depth;

	//	if the icon is a true CIcon, the rowBytes could be any arbitrary value.  Otherwise,
	//	we can calculate the proper PixMap rowBytes value from the basic rowBytes and the
	//	depth.

	if (theBlock->theGroup != aCIcon)
		THEPIXMAP->rowBytes = ShortMul(theBlock->rowBytes, depth) | 0x8000;
	else
		THEPIXMAP->rowBytes = ((CIconPtr)(theBlock->userProc))->iconPMap.rowBytes;

	THEPIXMAP->pmTable = theBlock->theClut;
}

//-------------------------------------------------------------------------------------
//	IconSuite Manipulation routines.
//-------------------------------------------------------------------------------------

//	__NewIconSuite is called directly by the dispatcher to implement the NewIconSuite call.

pascal OSErr __NewIconSuite(IconSuiteHandle *theSuite)
{
	*theSuite = (IconSuiteHandle)NewHandleClear(sizeof(IconSuite));
	return MemError();
}

//	__MakeIconCache is called directly by the dispatcher to implement the MakeIconCache call.

pascal OSErr __MakeIconCache(IconCacheHandle *theHandle,
						IconGetter makeIcon,
						void *yourDataPtr)
{
	IconCacheHandle	theCache;

	if (!(theCache = (IconCacheHandle)NewHandleClear(sizeof(IconCache))))
		return MemError();

	(*theCache)->theSuite.type = CACHE;		//	set the suite type to indicate that this is a cache
	(*theCache)->userPtr = yourDataPtr;		//	install the call-back and the data.
	(*theCache)->userMethod = makeIcon;

	*theHandle = theCache;

	return noErr;
}

//	EditIconSuite is called by __AddIconToSuite and __GetIconFromSuite (defined in IconUtils.a)
//	If isAdd is false, EditIconSuite assumes refCon is a pointer to a Handle and returns the
//	entry of type theType from theSuite (possibly NULL).  If isAdd is true, the value in refcon
//	is added to theSuite in the slot for data of type theType.

pascal OSErr EditIconSuite(long refCon,		//	Handle or referenct to handle of icon data
							IconSuiteHandle theSuite,
							ResType theType,
							long isAdd)
{
	Handle	*theSlot = (*theSuite)->table;
	long	theEnd = (long)(theSlot + IconSpace);
	ResType	*theTable = typeTable;

	do
	{
		if (theType == *theTable++)
		{
			if (isAdd)
				*theSlot = (Handle)refCon;
			else
				*((Handle *)refCon) = *theSlot;

			return noErr;
		}
	} while ((long)(++theSlot) < theEnd);

	return paramErr;
}

//	__ForEachIconDo is called by the trap dispatcher to implement the ForEachIconDo call.  The action
//	call-back is called for every member of theSuite specified by selector (See ERS for selector values).

pascal OSErr __ForEachIconDo(IconSuiteHandle theSuite,
							unsigned long selector,
							IconAction action,
							void *yourDataPtr)
{
	Handle 	*theTable = (Handle *)((*theSuite)->table);
	long	end = (long)(theTable + IconSpace);		// there are more data registers than address
	ResType *theTypes = typeTable;
	OSErr	rval = noErr;

	char theState = HGetState((Handle)theSuite);
	HLock((Handle) theSuite);

	do
	{
		short i = IconSizeSpace;

		do
		{
			if (selector & 0x00000001)
				if (rval = (*action)(*theTypes, theTable, yourDataPtr))
					goto out;

			theTable++;
			theTypes++;
			selector >>= 1;
		} while (--i);

		selector >>= (8 - IconSizeSpace);
	} while (theTable < (Handle *)end);

out:
	HSetState((Handle) theSuite, theState);

	return rval;
}

//	This is the call back used with ForEachIconDo to implement the __GetIconSuite
//	call below.  A more sophisticated call-back could be used to implement a
//	Get1IconSuite or to ensure that GetIconSuite only got icons from a single
//	file.  A user could do this kind of thing by rewriting GetIconSuite with
//	their own call-back.

pascal OSErr GetIconFromResource(ResType theType, Handle *theIcon, void *yourDataPtr)
{
	*theIcon = GetResource(theType, (short)yourDataPtr);
	return noErr;
}

//	Implements the GetIconSuite call.  The selector allows only specific members of the
//	suite to be asked for.

pascal OSErr __GetIconSuite(Handle *theHandle, short theID, unsigned long selector)
{
	if (__NewIconSuite((IconSuiteHandle *)theHandle))
		return MemError();

	__ForEachIconDo((IconSuiteHandle)(*theHandle), selector, GetIconFromResource, (void *)theID);

	return noErr;
}

//	Implements the SetSuiteLabel call.  This specifies a default label for a suite
//	which is used if no label is specified by the transform.  Yes, this is currently used.
//	Namely, by the process manager for the Apple Menu.

pascal OSErr __SetSuiteLabel(IconSuiteHandle theSuite, unsigned short theLabel)
{
	if (theLabel > 7)
		return paramErr;

	theLabel <<= labelShift;
	(*theSuite)->label = theLabel;
	return noErr;
}

//	Implements the GetSuiteLabel.  Just for completeness.

pascal short __GetSuiteLabel(IconSuiteHandle theSuite)
{
	return (((*theSuite)->label) >> labelShift);
}

//	This is the call back used with ForEachIconDo to implement DisposIconSuite.  An icon
//	is only disposed if it is not a resource.

pascal OSErr Disposer(ResType , Handle *theIcon, void *)
{
	if (*theIcon && (HomeResFile(*theIcon) == -1))
		DisposHandle(*theIcon);

	*theIcon = NULL;

	return noErr;
}

//	Implements the DisposeIconSuite call.  disposeData indicates whether an attempt should
//	be made to dispose of the data in the suite.  Only non-resource handles will be disposed.

pascal OSErr __DisposeIconSuite(IconSuiteHandle theSuite, Boolean disposeData)
{
	if (disposeData)
		(void)__ForEachIconDo(theSuite, svAllAvailableData, Disposer, NULL);

	DisposHandle((Handle)theSuite);

	return MemError();
}

//	These are the call-backs used with all the generic routines (PlotIconGeneric, HitTestIconGeneric and
//	IconToRgnGeneric) to implement PlotIconID, PlotIconMethod, PlotIconSuite, HitTestIconID, etc. etc.
//	FromMethod and FromResource are defined in IconUtils.a

Handle FromMethod(long data, IconType theIcon);

Handle FromResource(long data, IconType theIcon);

Handle FromSuite(long data, IconType theIcon)
{
	IconCachePtr	theCache = *((IconCacheHandle)data);
	Handle			rval = theCache->theSuite.table[theIcon];

	//	if a suite is a cache (See Cache routines below) and we don't have any data, we call the user's
	//	call back to get some data.

	if (!rval && theCache->theSuite.type)
	{
		IconGetter theProc = theCache->userMethod;

		if (theProc)
		{
			rval = (*theProc)(typeTable[theIcon], theCache->userPtr);
			(*((IconSuiteHandle)data))->table[theIcon] = rval;
		}
	}

	return rval;
}

//	AdjustRgn checks the from and to rects and decides whether the region needs mapping,
//	offsetting or nothing.  It is defined in IconUtils.a and is called by DoRegionTransform
//	and HitTestIconGeneric.

extern pascal void AdjustRgn(RgnHandle maskRgn, Rect *from, Rect *to);

//	DoRegionTransform performs a transform using region operations.  This will be called
//	when the useRegions flag is true.

pascal void DoRegionTransform(IconParamBlock *theBlock, short theTransform)
{
	RgnHandle	patRgn;
	Boolean		isASuite = (theBlock->theGroup != aCIcon);
	PenState	saveState;
	char		maskState;

	GetPenState(&saveState);

	//	If we are not a true CIcon, we need to generate a region from the icon mask.  The
	//	locking of theMask is redundant in the color case and extraneous in the case where
	//	we synthesize a mask but the complexity of determining exactly whether it needs to be done
	//	dosen't buy that much.

	if (isASuite)
	{
		maskState = HGetState(theBlock->theMask);
		HLock(theBlock->theMask);
		patRgn = NewRgn();
		MaskRgn(patRgn, theBlock);
		AdjustRgn(patRgn, &(theBlock->maskMap.bounds), &(theBlock->theRect));
	}
	else
		patRgn = (RgnHandle)theBlock->userData;			//	This is used for true CIcon to hold a region equivalent
														//	to its mask (See SetupParamBlockForCIcon)
	switch (theTransform)
	{
		//	makes an outline and fills it with a 25% gray pattern

		case ttOpen:
			FillRgn(patRgn, GetBlack());
			InsetRgn(patRgn, 1, 1);
			FillRgn(patRgn, GetLightGray());
			break;

		//	Or's over the data (already drawn) with a 25% gray pattern

		case ttOffline:
			PenPat(GetLightGray());
			PenMode(patOr);
			PaintRgn(patRgn);
			break;

		//	Bit-clears the already drawn data with a 50% pattern

		case ttDisabled:
			PenPat(GetHalfGray());
			PenMode(patBic);
			PaintRgn(patRgn);
			break;
	}

	SetPenState(&saveState);

	if (isASuite)
	{
		DisposeRgn(patRgn);
		HSetState(theBlock->theMask, maskState);
	}
}

//	The following routines are used by DoBitMapTransform to accomplish the various
//	transform effects.  They rely on the fact that the theBlock->maskMap describes
//	exactly the countours of the shape they are transforming.  This is guaranteed
//	by PrescaleMask (See below)

//	DoOutline takes the pre-scaled maskMap and OR's an outline of the icon
//	over destination.  It is used to implement the open transform.

void DoOutline(BitMapPtr maskMap, unsigned long *destination)
{
	unsigned char *source = (unsigned char *)(maskMap->baseAddr);

	unsigned long	dataMask = (unsigned long)(-1),
					lastRow,
					row,
					nextRow,
					destRow,
					checkRow,
					setMask;

	short			end = maskMap->bounds.bottom - 1,
					i;

	//	data mask is used to mask off all but the bits belonging to one row of the mask map.

	dataMask <<= (32 - maskMap->bounds.right);

	//	last row represents the row of mask data above the one we're analyzing.  It is initialized
	//	to zero because logicly, there are zero bits outside the bounds of our maskMap.

	lastRow = 0L;

	//	row is the row of mask data we are currently trying to hollow out.

	row = *((unsigned long *)source) & dataMask;
	source += maskMap->rowBytes;

	//	we march through the rows of our mask, hollowing our rows as we go.  We only go as far
	//	as the row before the bottom-most row, since we are only interested in examining rows
	//	that are samwiched between other rows with data.

	for (i = 0; i < end; i++)
	{
		//	destRow will be our hollowed out row.  It is initialized to row since we don't know if it
		//	needs hollowing.

		destRow = row;

		//	nextRow is the row below the one we are examining.  We advance our source pointer here.  We
		//	won't be advancing off the icon because we're stopping one row before the last.

		nextRow = *((unsigned long *)source) & dataMask;
		source += maskMap->rowBytes;

		//	if row has data above and below it, we examine its bits left-to-right.  If any one bit has
		//	a one-bit to left of it, to the right of it, on top of it and below it, we turn that bit
		//	off in destRow.  It works, really.

		if (destRow)
		{
			if (lastRow && nextRow)
			{
				setMask = 0x40000000;

				for (checkRow = row; checkRow; checkRow <<= 1, setMask >>= 1)
					if (((checkRow & 0xE0000000) == 0xE0000000) && (setMask & lastRow) && (setMask & nextRow))
						destRow ^= setMask;
			}

			//	here we OR the destRow with the destination data

			*destination |= destRow;
		}

		//	advance the destination pointer as well as lastRow and row.

		++destination;
		lastRow = row;
		row = nextRow;
	}

	//	nextRow now contains the data from the last row in the icon.  It either has data, in which case
	//	it gets OR'd in its entirety, or it dosen't and we do nothing.

	if (nextRow)
		*destination |= nextRow;
}

//	ApplyPattern is used to either bitClear the data in theMap with a 50% pattern (for the disabled transform)
//	or to OR the data with a 25% pattern (for offline and open).

pascal void ApplyPattern(BitMapPtr theMap, BitMapPtr theMask, Boolean bitClear)
{
	unsigned long	*mapData = (unsigned long *)(theMap->baseAddr);

	unsigned char	*maskData = (unsigned char *)(theMask->baseAddr);	// we know how wide this buffer is
	short			rowBytes = theMask->rowBytes,
					end = theMask->bounds.bottom;

	do {
		if (bitClear)
		{
			*mapData++ &= 0xAAAAAAAA;
			*mapData++ &= 0x55555555;
		}
		else
		{
			*mapData++ |= (0x88888888 & *((unsigned long *)maskData));
			maskData += rowBytes;
			*mapData++ |= (0x22222222 & *((unsigned long *)maskData));
			maskData += rowBytes;
		}

		end -= 2;
	} while (end > 0);
}


//	DoBitMapTransform causes an icon to appear transformed (open, offline or disabled).  If onePass is true,
//	It means that we are plotting a one-bit icon and we want to combine the transform patterns with the
//	icon data off-screen and plot it all at once.  If onePass is false, it means we are plotting a color
//	icon and that the color data has already been plotted.

pascal void DoBitMapTransform(IconParamBlock *theBlock, short theTransform, Boolean onePass)
{
	BitMap			transformMap;				//	BitMap used for transform data.
	unsigned long	transformBuffer[32],		//	offscreen area where we will create transform data
					*buffPtr = transformBuffer,
					*endPtr = buffPtr + 32;

	//	IsNotPreScaled is true if we did not have to scale the mask prior to this point.  We need to know this to decide
	//	whether to scale the data for one pass attempt and whether we should use CopyMask.

	Boolean			IsNotPreScaled = (MakeLong(theBlock->maskMap.bounds.bottom) == MakeLong(theBlock->dataMap.bounds.bottom));

	//	empty out the transformBuffer, since we are OR'ing data over it.

	do {
		*buffPtr++ = 0L;
	} while (buffPtr < endPtr);

	//	initialize transform BitMap

	transformMap.baseAddr = (Ptr)transformBuffer;
	transformMap.rowBytes = 4;
	transformMap.bounds = theBlock->maskMap.bounds;

	//	if we are trying to do this plot in one pass, we copy the one-bit data into the transformBuffer.  If the mask has
	//	not been prescaled, the image data data will not need scaling and we can just copy it into the buffer.  (this will
	//	always be the case in the Finder since they prescale their old-style icons into different size categories)  Otherwise,
	//	we will have to stretch the data into transform buffer;

	if (onePass && (theTransform != ttOpen))
	{
		if (IsNotPreScaled)
		{
			short			height = theBlock->dataMap.bounds.bottom;
			unsigned char	*theData = *(theBlock->theMask);

			buffPtr = transformBuffer;

			do {
				*buffPtr++ = *((unsigned long *)theData);
				theData += theBlock->rowBytes;
			} while (--height);
		}
		else	//	use srcXor because we do not know the foreColor or backColor but we know the destination is all zeros
			CopyBits((BitMapPtr)(&(theBlock->dataMap)), &transformMap, &(theBlock->dataMap.bounds), &(transformMap.bounds), srcXor, NULL);
	}

	//	OR in an outline of the mask if the transform is ttOpen

	if (theTransform == ttOpen)
		DoOutline(&(theBlock->maskMap), transformBuffer);

	//	if the transform is ttDisabled, bit clear the data with a 50% gray.  If it is ttOffline or ttOpen, OR the data with 25% gray

	if (theTransform == ttDisabled)
		ApplyPattern(&transformMap, &(theBlock->maskMap), true);
	else
		ApplyPattern(&transformMap, &(theBlock->maskMap), false);

	//	I'm putting this in a block to take advantage of register re-use

	{
		Rect	*maskRect = &(theBlock->maskMap.bounds);

		//	if we're plotting a one-bit, transformed icon or if we're plotting an opened icon, we have all the
		//	data we need in the transform map and we'll try to render that in one shot (although we may have
		//	to put down the mask separately.  See comments below).  Otherwise, we have a color transform and
		//	we assume that the basic icon data has already been rendered and that we're just going to OR over it
		//	with our transform pattern.

		if (onePass || (theTransform == ttOpen))
		{
			//	The choice of which rendering technique to use is based on three things.  If you have don't have color
			//	QuickDraw, CopyMask dosen't stretch bits so you generally can't use it.  If you're printing or plotting
			//	to an open PICT, again CopyMask won't work.  If you're mask is pre-scaled, and therefore has different
			//	bounds than your data, CopyMask won't work (a bug).  In general you want to try to use CopyMask because
			//	it goes down in one-pass and avoids any possible flashing.  If you can't use it, you BitClear the mask
			//	and OR over it with data, which works regardless of fore and backGround colors.

			if (HaveCQD() && (theBlock->useCopyMask) && IsNotPreScaled)
				CopyMask(&transformMap, &(theBlock->maskMap), theBlock->dstMap, maskRect, maskRect, &(theBlock->theRect));
			else
			{
				CopyBits(&(theBlock->maskMap), theBlock->dstMap, maskRect, &(theBlock->theRect), srcBic, NULL);
				CopyBits(&transformMap, theBlock->dstMap, maskRect, &(theBlock->theRect), srcOr, NULL);
			}
		}
		else
			CopyBits(&transformMap, theBlock->dstMap, maskRect, &(theBlock->theRect), srcOr, NULL);
	}
}

//	Render is called by PlotDeep, PlotShallowBlackAndWhite and PlotShallowColor.  It uses the same criterion described above
//	to decide which rendering technique to use with the main icon image data.

pascal void Render(IconParamBlock *theBlock)
{
	Rect	*destRect = &(theBlock->theRect);

	if (HaveCQD() && (theBlock->useCopyMask) && (MakeLong(theBlock->dataMap.bounds.bottom) == MakeLong(theBlock->maskMap.bounds.bottom)))
		CopyMask((BitMapPtr)(&(theBlock->dataMap)), &(theBlock->maskMap), theBlock->dstMap,
													&(theBlock->dataMap.bounds), &(theBlock->maskMap.bounds), destRect);
	else
	{
		CopyBits(&(theBlock->maskMap), theBlock->dstMap, &(theBlock->maskMap.bounds), destRect, srcBic, NULL);
		CopyBits((BitMapPtr)(&(theBlock->dataMap)), theBlock->dstMap, &(theBlock->dataMap.bounds), destRect, srcOr, NULL);
	}
}

//	PlotDeep, PlotShallowBlackAndWhite and PlotShallowColor are all called by PlotTheIcon, once it has been decided what kind of data
//	we will be plotting.  When these routines are called, theBlock has theDevice, theClut and theData set up.  The clipRgn has also been
//	set up to whatever extent is necessary.

//	PlotDeep is used to plot color data to a deep color screen.

pascal void PlotDeep(IconParamBlock *theBlock)
{
	ColorSpec	foreGround,
				backGround;

	Handle		theData = theBlock->theData;
	short		theTransform = theBlock->theTransform;
	char		dataState;

	//	the CLUT is purgeable and rendering may move memory.

	HNoPurge((Handle)(theBlock->theClut));

	//	if we've gotten as far as calling PlotDeep we know that there's a good, disabled clut for this screen.
	//	Setting the local copy theTransform to none indicates that we should not attempt a bit cleared disabled
	//	transform.

	if (theTransform == ttDisabled)
		theTransform = ttNone;

	//	if our data is unloaded, we attempt to load it.

	if (!(*theData))
		LoadResource(theData);

	//	lock down the image data because CopyBits or CopyMask may move memory (if only rarely)

	dataState = HGetState(theData);
	HLock(theData);

	SetUpPixMap(theBlock);	//	initialize dataMap with additional fields needed for a PixMap

	//	Save the color environment

	SaveFore(&foreGround);
	SaveBack(&backGround);

	//	Ensure black and white for fore and backGround color

	ForeColor(blackColor);
	BackColor(whiteColor);

	//	theTransform is ttOpen, the transform code will do all the work.  Otherwise, we render
	//	the main color image.

	if (theTransform != ttOpen)
		Render(theBlock);

	//	If there's a transform, we OR or bit clear over the image in an appropriate way.

	if (theTransform)
	{
		if (theBlock->useRegions)
			DoRegionTransform(theBlock, theTransform);
		else
			DoBitMapTransform(theBlock, theTransform, false);
	}

	//	restore the color environment

	RestoreFore(&foreGround);
	RestoreBack(&backGround);

	HPurge((Handle)(theBlock->dataMap.pmTable));
	HSetState(theData, dataState);
}

//	PlotShallowBlackAndWhite is used to plot to a one-bit screen (Either a black and white Mac or
//	a monitor set to one-bit)

pascal void PlotShallowBlackAndWhite(IconParamBlock	*theBlock)
{
	short		theTransform = theBlock->theTransform;
	ColorSpec	foreGround,
				backGround;

	theBlock->dataMap.baseAddr = *(theBlock->theMask);

	//	If the icon is selected, we invert the current fore and backGround color

	if (theBlock->selected)
	{
		if (HaveCQD())
		{
			RGBColor	RGBFore,
						RGBBack;

			SaveFore(&foreGround);
			SaveBack(&backGround);

			GetForeColor(&RGBFore);
			GetBackColor(&RGBBack);

			RGBForeColor(&RGBBack);
			RGBBackColor(&RGBFore);
		}
		else
		{
			GrafPtr curPort = myGetPort();
			*((long *)(&foreGround)) = curPort->fgColor;
			*((long *)(&backGround)) = curPort->bkColor;
			ForeColor(*((long *)(&backGround)));
			BackColor(*((long *)(&foreGround)));
		}
	}

	//	If we have a transform, use the transform code to render the icon and the transform.  Otherwise,
	//	just render the data.

	if (theTransform)
	{
		//	If we have to use regions to generate the transformed image, render the icon and modify it
		//	by the transform.  Otherwise, do the operation in one pass with DoBitMapTransform.

		if (theBlock->useRegions)
		{
			if (theTransform != ttOpen)
				Render(theBlock);

			DoRegionTransform(theBlock, theTransform);
		}
		else
			DoBitMapTransform(theBlock, theTransform, true);
	}
	else
		Render(theBlock);

	//	If we mucked with the color environment, restore it.

	if (theBlock->selected)
	{
		if (HaveCQD())
		{
			RestoreFore(&foreGround);
			RestoreBack(&backGround);
		}
		else
		{
			ForeColor(*((long *)(&foreGround)));
			BackColor(*((long *)(&backGround)));
		}
	}
}

//	PlotShallowColor is for plotting one-bit data to a deep screen

pascal void PlotShallowColor(IconParamBlock	*theBlock)
{
	RGBColor		foreColor,
					backColor;

	ColorSpec		foreGround,
					backGround;

	short			screenDepth = theBlock->screenDepth,
					theTransform = theBlock->theTransform,
					theLabel;

	Boolean			isDeep = (screenDepth > 4);	//	Are we deeper than 4-bits?

	//	this check makes sure that labeling does not happen in gray-scale or in 2-bit mode

	if ((screenDepth <= 2) || (theBlock->theDevice && !((*(theBlock->theDevice))->gdFlags & 0x1)))
		theLabel = 0;
	else
		theLabel = theBlock->theLabel;

	if (theLabel)
		(void)GetLabel(theLabel, &foreColor, NULL);
	else
		GetForeColor(&foreColor);

	if (theTransform == ttOpen)
	{
		backColor.red = 52266;			//	It seems strange to hard-code this color.
		backColor.green = 52266;		//	Should it maybe be function of the label
		backColor.blue = 65322;			//	or some other factor?  HMMMM?
	}
	else
		GetBackColor(&backColor);

	theBlock->dataMap.baseAddr = *(theBlock->theMask);

	//	This check allows for a pure disabled color to be used on a screen that has one.
	//	Otherwise, a disabled icon will be Bit-Cleared with a 50% pattern.  GetGray has
	//	the side-effect of setting foreColor to the average of backColor and foreColor.
	//	It returns false if no good average could be found.  The condition should read
	//	something like "If we're on a an 8-bit monitor or we're on a 4-bit monitor and
	//	there's no label, try to find a disabled color for this icon.  If you can find
	//	one, use that color as the foreground color and treat the icon as not transformed."

	if ((theTransform == ttDisabled) && (isDeep || (!theLabel)) &&
			GetGray(theBlock->theDevice, &backColor, &foreColor))
		theTransform = ttNone;

	//	If the icon is selected, set up the color environment to make it appear darkened.

	if (theBlock->selected)
	{
		void Darken(RGBColor *aColor);

		//	on a four-bit screen, there is almost never an appropriate, darkened color available,
		//	 so we don't try.

		if (isDeep)
			Darken(&foreColor);

		Darken(&backColor);
	}

	SaveFore(&foreGround);
	SaveBack(&backGround);

	//	Note:  There is no guarantee that a good approximation of foreColor and backColor exist
	//	on the destination device.  This is a hole in the whole color protection scheme in this code.
	//	These colors should probably be validated in some way before they are used, falling back on a
	//	black & white image.

	RGBForeColor(&foreColor);
	RGBBackColor(&backColor);

	//	Use the same criterion as in PlotShallowBlackAndWhite for render the image.

	if (theTransform)
	{
		if (theBlock->useRegions)
		{
			if (theTransform != ttOpen)
				Render(theBlock);

			DoRegionTransform(theBlock, theTransform);
		}
		else
			DoBitMapTransform(theBlock, theTransform, true);
	}
	else
		Render(theBlock);

	RestoreFore(&foreGround);
	RestoreBack(&backGround);
}

//	This routine simply decides which of the two PlotShallow flavors to use.  It exists for
//	historical reasons and should be folded into PlotTheIcon in a future version.

pascal void PlotShallow(IconParamBlock *theBlock)
{
	theBlock->dataMap.rowBytes = theBlock->rowBytes;
	theBlock->dataMap.pixelSize = 1;

	if (theBlock->screenDepth >= 2)
		PlotShallowColor(theBlock);
	else
		PlotShallowBlackAndWhite(theBlock);
}

//	PlotTheIcon sets up the IconParamBlock for plotting the icon to the particular screen and sets up
//	the clip region as appropriate.  It is either called directly by PlotIconGeneric or indirectly
//	by way of DeviceLoop.  On entry, theBlock has been set up by SetupParamBlock, MakeBoundary has
//	been called, alignment has been done and the mask has been prescaled if necessary.  PlotTheIcon
//	gets the best available data for the screen depth and calls one of the plotting routines above
//	to render the data with its transform.

pascal void PlotTheIcon(short			screenDepth,
						short			flags,
						GDHandle		theDevice,
						IconParamBlock	*theBlock)
{
	Rect				boundRect,
						sectRect;

	RgnHandle			saveClip = NULL;

	short				theTransform = theBlock->theTransform;

	//	flags will be non-zero if PlotTheIcon was called from DeviceLoop.  Flags will be zero if it
	//	was called directly from PlotIconGeneric which implies a single target device.

	if (theDevice && flags)
	{
		//	the following code generates a global rectangle which bounds the icon in its destination
		//	size.

		boundRect = theBlock->boundary;
		MapRect(&boundRect, &(theBlock->dataMap.bounds), &(theBlock->theRect));
		LocalToGlobal((Point *)&boundRect);
		LocalToGlobal((Point *)(&(boundRect.bottom)));

		//	If this global rectangle dosen't intersect the GDevice were checking, then we won't plot to it.
		//	Otherwise, we will check if it is only partially on the screen.  If it is, we will adjust the
		//	clipRgn so that only the portion of the icon that appears on the screen will be drawn.

		if (SectRect(&((*theDevice)->gdRect), &boundRect, &sectRect))
		{
			if ((!EqualRect(&sectRect, &boundRect)) && (!theBlock->dontDraw))
			{
				RgnHandle realClip = myGetPort()->clipRgn;

				saveClip = NewRgn();
				GetClip(saveClip);
				GlobalToLocal((Point *)(&sectRect));
				GlobalToLocal((Point *)(&(sectRect.bottom)));
				ClipRect(&sectRect);
				SectRgn(saveClip, realClip, realClip);
			}
		}
		else
			return;
	}

	theBlock->screenDepth = screenDepth;
	theBlock->theDevice = theDevice;

	//	The following conditional reads - "try to plot the icon as color if A) we are plotting to an 8-bit
	//	screen and the transform is not ttOpen or B) we are plotting to a 4-bit screen, the icon is not
	//	selected, and the transform is neither ttOpen nor ttDisabled.  Otherwise, treat it as one-bit"

	if (((screenDepth > 4) && (theTransform != ttOpen)) ||
		((screenDepth == 4) && (!(theBlock->selected)) && (!(theTransform & ttDisabled))))
	{
		theBlock->dataDepth = screenDepth;		//	Initialize data depth to screen depth.  GetBestData may
		GetBestData(theBlock);					//	change this value.
	}
	else
	{
		theBlock->dataDepth = 1;
		theBlock->theClut = NULL;
	}

	//	PlotTheIcon is being called from LoadIconCache, we're done loading the data and we run away.

	if (theBlock->dontDraw)
		return;

	if (theBlock->dataDepth >= 4)
		PlotDeep(theBlock);
	else
		PlotShallow(theBlock);

	if (saveClip)
	{
		SetClip(saveClip);
		DisposeRgn(saveClip);
	}
}

//	MakeBoundary creates a rectangle within the bounds of the maskMap (before it is pre-scaled) which
//	bounds the non-zero bits of the mask.  It returns false if the mask is all zeros.

Boolean MakeBoundary(IconParamBlock *theBlock)
{
	unsigned char	*ptr = (unsigned char *)(theBlock->maskMap.baseAddr); 			// <57>

	unsigned short	mark,
					side,
					rowBytes = theBlock->maskMap.rowBytes,
					height = theBlock->maskMap.bounds.bottom;

	unsigned long	temp,
					checkSum,
					sizeMask = (theBlock->rowBytes < 4) ? 0xFFFF0000 : 0xFFFFFFFF;

	//	Find the first row that has data and call it the top.
	for (side = 0; side < height; ++side, ptr += rowBytes)
		if (*((unsigned long *)ptr) & sizeMask)
			break;

	//	If no data in the mask return false
	if (side == height)
		return false;

	theBlock->boundary.top = side;

	checkSum = 0;

	//	mark is set equal to the row number of the last row with data.  checkSum
	//	is set to the OR of all the rows.
	for (; side < height; ++side, ptr += rowBytes)
		if (temp = (*((unsigned long *)ptr) & sizeMask))
		{
			mark = side;
			checkSum |= temp;
		}

	//	the bottom is equal to the row number of the last row with data plus one.
	theBlock->boundary.bottom = mark + 1;

	//	checkSum is shifted left until the most significant bit is one.
	for (side = 0; !(checkSum & 0x80000000); checkSum <<= 1, ++side);

	//	the number of shifts will be the left boundary
	theBlock->boundary.left = side;

	//	checkSum is further shifted left until all the data is gone.
	for (; checkSum; checkSum <<= 1, ++side);

	//	the number of shifts plus the left offset equals the right boundary.
	theBlock->boundary.right = side;

	return true;
}

//	CreateMask generates a mask from some data.  It is generated into a buffer
//	at the end of theBlock.  CalcMask is used to generate the data.

void CreateMask(IconParamBlock *theBlock, long maskByteLength)
{
	Ptr		maskBuffer = (Ptr)(theBlock->maskBuffer);
	short	rowBytes = theBlock->maskMap.rowBytes,
			height = ShortDiv(maskByteLength, rowBytes);

	//	This catches the odd case in which an icon is larger than an icon
	//	but smaller than an icon with a mask.  This makes sure we only use
	//	32 bits of height so as not to overrun our buffer.

	if (height > 32)
		height = 32;

	CalcMask((Ptr)(*(theBlock->theMask)),
			maskBuffer, rowBytes, 4,
			height, rowBytes >> 1);

	theBlock->maskMap.baseAddr = maskBuffer;
	theBlock->maskMap.rowBytes = 4;
}

//	SetupParamBlock finds the appropriate size category of icon to be dealing
//	with, gets (or synthesizes) the mask for the data, locks all necessary data
//	down, sets up the maskMap and some parameters that don't change throughout
//	the course of the plot (or make region) call.  It returns FALSE if no mask
//	(1-bit data) could be found in any size category.

short SetupParamBlock(IconParamBlock *theBlock)
{
	short	size,
			rowBytes,
			desired = ChooseBestSize(&(theBlock->theRect));

	long	maskByteLength;

	Handle	theMask;

	//	Find a mask of the right size category

	for (size = desired; size <= miniIcon1; size += IconDepthSpace)
		if (theMask = (*(theBlock->userProc))(theBlock->userData, size))
			goto done;

	for (size = desired - IconDepthSpace; size >= largeIcon1; size -= IconDepthSpace)
		if (theMask = (*(theBlock->userProc))(theBlock->userData, size))
			goto done;

	return false;

	done:

	theBlock->theMask = theMask;
	theBlock->theGroup = size;
	theBlock->theData = NULL;

	//	size here means the basic dimension (height) of the icon of specific size
	//	category.  rowBytes here is a basic rowBytes of the one bit data.

	if (size == largeIcon1)
	{
		rowBytes = 4;
		size = 32;
	}
	else
	{
		rowBytes = 2;

		if (size == smallIcon1)
			size = 16;
		else
			size = 12;
	}

	//	the basic rowBytes is stored because maskMap.rowBytes might be changed to 4
	//	arbitrarily if the mask is synthesized.
	theBlock->rowBytes = rowBytes;

	//	maskMap is setup
	theBlock->maskMap.rowBytes = rowBytes;
	MakeLong(theBlock->maskMap.bounds) = 0L;
	theBlock->maskMap.bounds.bottom = size;
	theBlock->maskMap.bounds.right = rowBytes << 3;	// this insures that a mini icon is 12X16

	//	lock down the mask member and remember its state.

	if (!(*theMask))
		LoadResource(theMask);	//	theoretically its cool to let LoadResource decide whether the resource is loaded or not
								//	but LoadResource has been back-patched thrice at the time of this writing and all kinds
								//	of annoying stuff goes before the simple check of the master pointer.  sigh.

	//	The following does a size check to guess whether an icon has a mask or not.  If not,
	//	a probable mask is synthesized.

	rowBytes *= size;		//	I'm hijacking rowbytes to use as a temp
	rowBytes <<= 1;
	maskByteLength = GetHandleSize(theMask);

	//	if there is no mask data appended to the one-bit version of this icon (i.e. its not a proper ICN#, ics#, icm#)
	//	synthesize a mask.  Otherwise, we just point the maskMap baseAddr at the mask data in the one-bit icon.

	if (maskByteLength < rowBytes)
	{
		rowBytes >>= 1;
		CreateMask(theBlock, maskByteLength);
	}
	else
		theBlock->maskMap.baseAddr = (*theMask) + (maskByteLength >> 1);

	//	This is just to allow the Finder to use some SICN's as minis without special-casing (Grrrr!)

	if ((theBlock->theGroup == miniIcon1) && (maskByteLength > rowBytes))
	{
		long center = ((theBlock->theRect.bottom + theBlock->theRect.top) >> 1);

		theBlock->theRect.top = center - 8;
		theBlock->theRect.bottom = center + 8;
		theBlock->maskMap.bounds.bottom = 16;
	}

	theBlock->dataMap.bounds = theBlock->maskMap.bounds;	//	the mask and data have the same source bounds (may change later)

	theBlock->dataMap.pixelSize = 0;		//	functions as flag saying "the data map is not yet initialized as a color grafport"

	theBlock->useRegions = false;			//	Initially assume that we will not use regions for transforms and hit testing.
											//	PrescaleMask may change this value.

	return true;
}

//	PrescaleMask is used after SetupParamBlock.  If the destination rectangle of an icon is
//	less than or equal to 32 in both dimensions, and if it is different in height or width
//	to maskMap.bounds, we do the stretching that will take place off-screen.  The result ends
//	up in the maskBuffer and maskMap is modified to point to it.  After calling PrescaleMask,
//	you are guaranteed that the maskMap describes the countours of the icon's mask as it will
//	be drawn (which is important for doing transforms and for hit-testing without doing region
//	operations).  If an icon is larger than our off-screen buffer (never under 7.0), we use
//	region operations for hit-testing and transforms.  PrescaleMask is called by HitTestIconGeneric
//	and by PlotIconGeneric (only if there is a transform).

pascal void PrescaleMask(IconParamBlock *theBlock)
{
	short	width = theBlock->theRect.right - theBlock->theRect.left,
			height = theBlock->theRect.bottom - theBlock->theRect.top;

	if ((width > 32) || (height > 32))
	{
		theBlock->useRegions = true;
		return;
	}

	//	This checks if any stretching will be done… we know that maskMap.bounds has (0,0) for its
	//	upper left coordinate.

	if ((width != theBlock->maskMap.bounds.right) || (height != theBlock->maskMap.bounds.bottom))
	{
		unsigned long	tempBuffer[32];
		BitMap			tempMap;
		Rect			newBounds;

		MakeLong(newBounds) = 0;
		newBounds.right = width;
		newBounds.bottom = height;

		MakeLong(tempMap.bounds) = 0;
		MakeLong(tempMap.bounds.bottom) = 0x00200020;
		tempMap.rowBytes = 4;
		tempMap.baseAddr = (Ptr)theBlock->maskBuffer;

		//	if maskBuffer is currently being used, as in the case of a missing mask we have synthesized,
		//	we dump it temporarily to an intermediate buffer.

		if (theBlock->maskMap.baseAddr == theBlock->maskBuffer)
		{
			theBlock->maskMap.baseAddr = (Ptr)tempBuffer;
			BlockMoveData(theBlock->maskBuffer, tempBuffer, 32 * sizeof(unsigned long));
		}

		CopyBits(&(theBlock->maskMap), &tempMap, &(theBlock->maskMap.bounds), &newBounds, srcCopy, NULL);

		MakeLong(theBlock->maskMap.bounds.bottom) = MakeLong(newBounds.bottom);
		theBlock->maskMap.baseAddr = (Ptr)theBlock->maskBuffer;
		theBlock->maskMap.rowBytes = 4;
	}
}

//	This is the routine that all plotting goes through.  By the time we get to PlotIconGeneric, we
//	have a handler routine and some data pushed that identifies how we are going to get suite
//	members.  For example, in the case of PlotIconID, the refCon is equal to the resource ID and
//	the IconRetriever is set to a function call FromResource which calls GetResource for an icon
//	type and the given ID.  The dontDraw parameter allows for LoadIconCache to act as if it were
//	drawing, but in stead just load the data.
//
//	PlotIconGeneric is called from assembly language wrappers in IconUtils.a that Implement
//	PlotIconID, PlotIconSuite, PlotIconMethod and LoadIconCache.

#define PrintVars (*((short *)0x948))
pascal OSErr PlotIconGeneric(const Rect *theRect,
						short align,
						short transform,
						long refCon,
						IconRetriever iconProc,
						short dontDraw)
{
	GrafPort			*curPort;
	IconParamBlock		theBlock;
	GlobalIconDataPtr	theGlobals = (GlobalIconDataPtr) GetExpandMemIconCluts();

	//	if this is a call to LoadIconCache and the input is not an iconcache,
	//	exit with a paramErr

	if (dontDraw && !((*((IconSuiteHandle)refCon))->type))
		return paramErr;

	//	save some basic information in the parameter block
	theBlock.theRect = *theRect;
	theBlock.userData = refCon;
	theBlock.userProc = iconProc;
	theBlock.theTransform = transform & transformMask;
	theBlock.selected = transform & ttSelected;
	theBlock.dontDraw = dontDraw;

	curPort = myGetPort();

	//	useCopyMask is true if the icon does not need to go through bottlenecks
	theBlock.useCopyMask = !((curPort->grafProcs) || (curPort->picSave));

	theBlock.dstMap = &(curPort->portBits);

	if (!SetupParamBlock(&theBlock))
		return noMaskFoundErr;

	//	See MakeBoundary above.  If no data was found in the mask portion of the
	//	one-bit data, then clearly CopyMask would not plot anything.  So why bother
	//	doing any more work?  This is not the same case as no one-bit data available.

	if (!MakeBoundary(&theBlock))
		goto out;

	//	boundary-making and alignment have to take place before PrescaleMask, because they
	//	make certain assumptions about the mask which in general are no longer valid after the
	//	pre-scale.

	if (align)
		PerformAlignment(&theBlock, align);

	//	this line performs an off-screen stretching of the mask data if there is a
	//	a difference between the size of the destination rect and the mask size.  Thus,
	//	the maskMap field will always have a correctly scaled mask of the data which
	//	can be used for creating transform effects etc.  The one exception would be if the
	//	destination rect was so large that the pre-scaled mask would not fit in the 32 X 32
	//	buffer in the parameter block.  In this case, the useRegions flag would be set and
	//	regions would be used for transforms, hit-testing etc.

	if (theBlock.theTransform)
		PrescaleMask(&theBlock);

	//	If we are plotting to a PixMap, we lock down the mask (because color CopyBits/CopyMask
	//	can move memory), get label information and decide how to plot based on our screen setup.

	if (curPort->portBits.rowBytes < 0)
	{
		GDHandle	theScreen;
		Handle		theMask = theBlock.theMask;
		char		maskState = HGetState(theMask);

		HLock(theMask);

		theBlock.theLabel = (transform & labelMask) >> labelShift;

		if (theScreen = theGlobals->virtualScreen)
			PlotTheIcon((*((*theScreen)->gdPMap))->pixelSize, 0, theScreen, &theBlock);
		else
			DeviceLoop(NULL, (DeviceLoopDrawingProcPtr)PlotTheIcon, (long)(&theBlock),
					dontMatchSeeds | singleDevices | allDevices);

		HSetState(theMask, maskState);
	}
	else
	{
		theBlock.theLabel = 0;					//	If we're not plotting to a CGrafPort, we don't use a label.
		PlotTheIcon(1, 0, NULL, &theBlock);
	}

out:
	return noErr;
}

//	IconToRgnGeneric is called by a bunch of assembly language wrappers in IconUtils.a to implement
//	IconIDToRgn, IconSuiteToRgn and IconMethodToRgn.  It is also called from HitTestIconGeneric if regions
//	will be used to perform hit-testing.  It takes a valid region, the rect to which the theoretical icon was drawn, the
//	alignment used and the same kind of call-back used by PlotIconGeneric above.  theRgn is changed to be
//	equivalent to the icon's mask when aligned and stretched to the destination rect.

pascal OSErr IconToRgnGeneric(RgnHandle		theRgn,
						Rect				*theRect,
						IconAlignmentType	align,
						long				refCon,
						IconRetriever		iconProc)
{
	IconParamBlock theBlock;

	//	minimal set up for call to SetupParamBlock (I know, its a little squirly that SetupParamBlock needs set up but…)

	theBlock.theRect = *theRect;
	theBlock.userData = refCon;
	theBlock.userProc = iconProc;

	//	we set up the IconParamBlock, which finds the right-sized mask for the destination rect and loads it.  It also
	//	may synthesize a mask if a SICN or ICON is used for a one-bit icon.

	if (!SetupParamBlock(&theBlock))
		return noMaskFoundErr;

	//	if there is no data in the mask, we make an empty region.  Makes sense.

	if (MakeBoundary(&theBlock))
	{
		//	MaskRgn moves memory so we lock down our mask.  Note for future change:  a check should be made here
		//	to see if the mask is in maskBuffer, in which case this lock and restore is extraneous.

		Handle theMask = theBlock.theMask;
		char maskState = HGetState(theMask);
		HLock(theMask);

		//	If there's an alignment specified, we do it.

		if (align)
			PerformAlignment(&theBlock, align);

		//	make the region and	stretch it appropriately

		MaskRgn(theRgn, &theBlock);
		AdjustRgn(theRgn, &(theBlock.maskMap.bounds), &(theBlock.theRect));

		HSetState(theMask, maskState);
	}
	else
		SetEmptyRgn(theRgn);

	return noErr;
}

//	HitTestIconMaskGeneric is called from HitTestIconGeneric (See Below) if regions are not used for the
//	hit-testing.  It takes the same input as HitTestIconGeneric.  The test is performed by stretching the
//	mask data off-screen (if necessary) and comparing the point or rect to this data arithmeticly.

#define	isAPoint	0
#define isARect		1

pascal Boolean HitTestIconMaskGeneric(unsigned long pointOrRect,
										short IsRect,
										Rect *theRect,
										IconAlignmentType align,
										long refCon,
										IconRetriever iconProc)
{
	IconParamBlock	theBlock;
	Rect			sectRect;
	unsigned long	testMap;
	short			theRow,
					theCount;

	//	Set up for call to SetupParamBlock (yeah yeah, I know)

	theBlock.theRect = *theRect;
	theBlock.userData = refCon;
	theBlock.userProc = iconProc;

	//	If there is no one-bit data or the mask contains nothing but zeros, we return false

	if (!SetupParamBlock(&theBlock) || !MakeBoundary(&theBlock))
		return false;

	//	If there is alignment, we do it.

	if (align)
		PerformAlignment(&theBlock, align);

	theRect = &(theBlock.theRect);	//	we hijak the parameter theRect to point to the possibly shifted destination rectangle
									//	(NOTE:  This is very silly.  The idea was to make references to theRect smaller and faster.
									//	Why not make a new variable, or not do this at all and refer to the rect as theBlock.theRect??
									//	We luck out under the 3.2 compiler, which sees that we refer to theRect a lot and allocates A3
									//	for this.  If we were out of address registers, this would make larger and slower code)

	PrescaleMask(&theBlock);		//	if the icon is to be shrunken, this routine will do the shrinking offscreen and place the
									//	result in maskMap

	//	we set up theRow, theCount and testMap to test against the pre-scaled mask data.  theRow is equal to the first row of the maskMap
	//	that we will test.  theCount is equal to the number of rows we will test.  testMap is a long-word bitMap which we will intersect
	//	with each row that we test.  If any row has intersection with testMap, then we have a hit.

	if (IsRect)
	{
		//	we do the intersection with the possibly shifted rectangle to see if alignment has moved us
		//	out of the test rect.  (if there is no alignment, this is redundant with the SectRect we did
		//	in HitTestIconGeneric below.  This probably needs restructuring in a new version.)

		if (!SectRect((Rect *)pointOrRect, theRect, &sectRect))
			return false;

		//	we set test map equal to one scan-line of the rectangle intersection of the test rect and the destination

		testMap = 0xffffffff;
		testMap <<= (32 - (sectRect.right - sectRect.left));
		testMap >>= (sectRect.left - theRect->left);

		//	theRow is the top of the intersection rectangle in destination co-ordinates.

		theRow = (sectRect.top - theRect->top);

		//	theCount is equal to the height of the intersection

		theCount = (sectRect.bottom - sectRect.top);
	}
	else
	{
		//	testMap is set to a one bit at the horizontal location of the test point in destination co-ordinates

		testMap = 1 << (31 - ((short)(pointOrRect & 0xffff) - theRect->left));

		//	theRow is set equal to the vertical location of the test point in destination co-ordinates

		theRow = ((short)(pointOrRect >> 16)) - theRect->top;

		//	there is only one row to test

		theCount = 1;
	}

	//	Here we take theRow, theCount and testMap and compare it to the mask data.

	{
		short			rowBytes = theBlock.maskMap.rowBytes;

		//	since we treat rows that are sometimes only a word in width as long words, we need
		//	to mask off the right most data that is significant.

		unsigned long	significantMask = -1L << (32 - theBlock.maskMap.bounds.right);

		unsigned char	*maskData = theBlock.maskMap.baseAddr + ShortMul(rowBytes, theRow),
						*endData = maskData + ShortMul(rowBytes, theCount);

		//	start at theRow and loop through theCount rows, testing for itersection with testMap

		do {
			if (*((unsigned long *)maskData) & significantMask & testMap)
				return true;

			maskData += rowBytes;
		} while (maskData < endData);
	}

	return false;
}

//	HitTestIconGeneric is called by assembly language wrappers in IconUtils.a to implement PtInIconID,
//	PtInIconSuite, PtInIconMethod, RectInIconID, RectInIconSuite and RectInIconMethod.  It takes a value
//	which is either a point or pointer to a rect along with a flag indicating which one it is.  It also
//	takes a rect which is where the icon being hit-test was drawn and the alignment that was used.  Finally,
//	the generic call-back and refcon used in all the generic routines (PlotIconGeneric, IconToRgnGeneric, etc.)
//	It returns true if the mask data as aligned and stretched intersects either the rect or the pixel to
//	the lower right of the point.  It returns false if there is no intersection or if any kind of error was
//	encountered.

pascal Boolean HitTestIconGeneric(unsigned long		pointOrRect,
								Rect				*theRect,
								IconAlignmentType	align,
								long				refCon,
								IconRetriever		iconProc,
								short				IsRect)
{
	Boolean			rval = false;
	RgnHandle		testRgn = NULL;
	Rect			sectRect;

	if (IsRect)
	{
		//	if the test rectangle does not even touch the destination rect, clearly there is no hit

		if (!SectRect((Rect *)pointOrRect, theRect, &sectRect))
			goto out;

		//	if the test rectangle completely engulfs the destination rectangle, clearly there is a
		//	hit and no further testing is required.

		if (EqualRect(theRect, &sectRect))
			goto trueOut;
	}
	else
		if (!PtInRect(*((Point *)(&pointOrRect)), theRect))		//	If our point is not in the ball-bark, we bail
			goto out;

	//	The only time we would use IconToRgnGeneric to test pointOrRect would be if our maskMap needed scaling
	//	and we could not do the scaling off-screen with PrescaleMask.  This is only true if the height or
	//	width of the destination rect is greater than 32 (the size of our off-screen buffer).  Otherwise, we
	//	use HitTestIconmaskGeneric which is faster and dosen't move memory as much.

	if (((theRect->right - theRect->left) > 32) || ((theRect->bottom - theRect->top) > 32))
	{
		testRgn = NewRgn();

		if (IconToRgnGeneric(testRgn, theRect, align, refCon, iconProc))
			goto out;

		if (IsRect)
		{
			if (!RectInRgn((Rect *)pointOrRect, testRgn))
				goto out;
		}
		else
			if (!PtInRgn(*((Point *)(&pointOrRect)), testRgn))
				goto out;
	}
	else if (!HitTestIconMaskGeneric(pointOrRect, IsRect, theRect, align, refCon, iconProc))
		goto out;

trueOut:

	rval = true;

out:
	if (testRgn)
		DisposeRgn(testRgn);

	return rval;
}

//	MakeIconData is called after DeviceList is initialized to set up the ExpandMem global structure used by the icon utilities.
//	The structure is returned in the variable parameter iconCluts.  (Named for historical reasons and not changed lately)
//
//	This is the layout of the structure:
//
//	#define Labels 		8
//	#define Sets		8
//
//	typedef struct
//	{
//		Strings				labelStrings;					//	array of eight string handles to label strings.  On a black and
//															//	white Mac, the rest of the structure after this point is not allocated.
//		long				seeds[Labels * Sets];			//	array of seeds for all the cluts (transformed and labeled)
//		CTabHandle			colorTables[Labels * Sets],		//	array of CTabHandles for all cluts
//							originals[2];					//	handles to the two original rom cluts (clut 4 and clut 8)
//		RGBColor			labelColors[Labels];			//	array of 8 label colors
//		unsigned char		**indexLists[2];				//	two handles to arrays of indices used in checking colors
//		GDHandle			virtualScreen;					//	a GDHandle used by SetIconDevice and GetIconDevice
//		long				deviceListSize;					//	number of GDevices at boot time
//		DeviceCheck			deviceCheckList[0];				//	a variable number of DeviceCheck structures used to cache
//															//	the results of CheckColors for all GDevices
//	} GlobalIconData;
//
//	typedef Handle Strings[Labels];
//
//	typedef struct
//	{
//		GDHandle		theDevice;				//	Handle to the GDevice that we're caching the information about
//		long			theSeed;				//	the seed of its GDPmap's pmTable at last caching
//		unsigned long	checkMask,				//	a bit-map specifying which cluts we actually tried and cached results for
//						checkResults,			//	results of caching as a bit-map
//						disabledCheckMask,		//	the other long word of the CheckMask bit-map needed when we added disabled cluts
//						disabledCheckResults;	//	the other long word of the CheckResults bit-map needed when we added disabled cluts
//	} DeviceCheck;

//	SetUpDeviceCaches initializes the DeviceCheck structures in the GlobalIconData deviceCheckList array

pascal void SetUpDeviceCaches(GlobalIconData *temp)
{
	GDHandle	aGDHandle = DeviceList;
	DeviceCheck	*checkPtr = temp->deviceCheckList;

	do {
		checkPtr->theDevice = aGDHandle;
		checkPtr->theSeed = (*((*((*aGDHandle)->gdPMap))->pmTable))->ctSeed;
		checkPtr->checkMask = 0L;
		checkPtr->disabledCheckMask = 0L;
		++checkPtr;
		aGDHandle = (GDHandle)((*aGDHandle)->gdNextGD);
	} while (aGDHandle);
}

pascal OSErr __UpdateIconGlobals(void)
{
	GlobalIconDataPtr	theOldData = (GlobalIconDataPtr) GetExpandMemIconCluts();
	GlobalIconDataPtr	theNewData;

	GDHandle			aGDHandle = DeviceList;

	short				devices = 0;

	long				size = sizeof(GlobalIconData);

	do {
		++devices;
		size += sizeof(DeviceCheck);
		aGDHandle = (GDHandle)((*aGDHandle)->gdNextGD);
	} while (aGDHandle);

	if (!(theNewData = (GlobalIconDataPtr)NewPtrSysClear(size)))
		return MemError();

	BlockMoveData((Ptr)theOldData, (Ptr)theNewData, sizeof(GlobalIconData));

	DisposPtr((Ptr)theOldData);

	theNewData->deviceListSize = devices;

	SetUpDeviceCaches(theNewData);

	SetExpandMemIconCluts((Ptr) theNewData);

	return noErr;
}

pascal OSErr iconGestaltProc(OSType, long *response)
{
	*response = 1;
	return noErr;
}

pascal void MakeIconData(GlobalIconDataPtr *iconCluts)
{
	long					size,
							devices;
	GlobalIconDataPtr		temp;
	short					curResFile, i;
	Handle					aHandle,
							*strPtr;
	RGBColor				*colorPtr;
	GDHandle				aGDHandle;

	(void)NewGestalt(gestaltIconUtilities, (ProcPtr)iconGestaltProc);

	//	If we're running on a color machine, allocate the whole structure.  Otherwise, just allocate the
	//	label string list.

	if (HaveCQD())
	{
		aGDHandle = DeviceList;
		size = sizeof(GlobalIconData);

		devices = 0;

		do {
			++devices;
			size += sizeof(DeviceCheck);
			aGDHandle = (GDHandle)((*aGDHandle)->gdNextGD);
		} while (aGDHandle);
	}
	else
		size = sizeof(Strings);

	temp = (GlobalIconDataPtr)NewPtrSysClear(size);

	*iconCluts = temp;			//	return the structure in the variable parameter

	strPtr = &(temp->labelStrings);	//	point to the beginning of the label string handles

	//	Get the label strings (Should they be unloaded?)

	for (i = labelColorBase; i < (labelColorBase + Labels); ++i)
		*strPtr++ = GetResource('lstr', i);

	//	If we're not on a color machine, we're done.

	if (!HaveCQD())
		return;

	//	virtualScreen is initially NULL, indicating that we will traverse the list of devices normally.

	temp->virtualScreen = NULL;

	//	set deviceListSize with the count we generated for allocation

	temp->deviceListSize = devices;

	//	Run through the list of devices, initializing our cache of device information.

	SetUpDeviceCaches(temp);

	//	Save the current resource file and set to the System file.

	curResFile = CurResFile();
	UseResFile(0);

	//	Get	the two lists of color indices to check at CheckColors time

	strPtr = (Handle *)(&(temp->indexLists));
	*strPtr++ = GetResource('indl', labelColorBase);
	*strPtr = GetResource('indl', labelColorBase + 1);

	//	Get the two standard system clut's for 8 and 4 bits from ROM

	strPtr = (Handle *)(&(temp->originals));
	*((short *)RomMapInsert) = mapTrue;
	*strPtr++ = GetResource('clut',8);
	*((short *)RomMapInsert) = mapTrue;
	*strPtr++ = GetResource('clut',4);

	//	Set the first label color to black.

	colorPtr = &(temp->labelColors);
	*((long *)(colorPtr)) = 0L;
	colorPtr->blue = 0;
	++colorPtr;

	//	Get the other seven label colors from resources

	for (i = (labelColorBase + 1); i < (labelColorBase + Labels); ++i)
		*colorPtr++ = **((RGBColor **)GetResource('rgb ', i));

	//	Restore the resource file

	UseResFile(curResFile);

	//	Get the seed for the 8-bit system clut

	strPtr = (Handle *)(&(temp->seeds));
	*strPtr++ = (Handle)((*(temp->originals[0]))->ctSeed);

	//	Generate seeds for the other 7 labeled 8-bit cluts and the
	//	selected clut and the 7 labeled selected 8-bit cluts

	for (i = 1; i < (Labels * 2); ++i)
		*strPtr++ = (Handle)GetCTSeed();

	//	Get the see for the 4-bit system clut

	*strPtr++ = (Handle)((*(temp->originals[1]))->ctSeed);

	//	Generate seeds for the other 7 labeled 4-bit cluts and the
	//	selected 4-bit clut and the 7 labeled selected 4-bit cluts and
	//	all the other combinations of labeled, selected and disabled cluts

	for (i = 1; i < (Labels * 6); ++i)
		*strPtr++ = (Handle)GetCTSeed();

	//	Allocate empty handles for all the cluts

	strPtr = (Handle *)(&(temp->colorTables));

	for (i = 0; i < (Labels * Sets); ++i)
	{
		aHandle = NewEmptyHandleSys();
		*strPtr++ = aHandle;
	}
}

#define acceptableDelta (2 * 256)

//	CompareComponent takes two color components and returns 0 if the two are equal within
//	the tolerance specified by acceptableDelta, a negative number if the first is less than
//	the second by acceptableDelta and a positive number if the first is greater than the
//	second by acceptableDelta.

long CompareComponent(unsigned short comp1, unsigned short comp2)
{
	long absoluteDifference = (unsigned long)comp1 - (unsigned long)comp2;

	if ((absoluteDifference < acceptableDelta) && (absoluteDifference > -acceptableDelta))
		return 0;
	else
		return absoluteDifference;
}

//	Darken is used to reduce the brightness of an RGBColor to create selected clut's.  The intention
//	is to take a color, halve all the components and then increase the saturation of the result by
//	halving again all but the strongest component.  It is necessary to increase the saturation of
//	a color when you halve its components because dividing all the components by two causes
//	the difference in component values to become much smaller, decreasing the saturation.  The alogrithm
//	implemented below causes the HSV value of the color to be halved, while approximately maintaining
//	the color's original saturation.

void Darken(RGBColor *aColor)
{
	unsigned short	*smallest = &(aColor->red),
					*notSmallest1 = &(aColor->green),
					*notSmallest2 = &(aColor->blue),
					*temp = smallest;

	long			compareValue;

	//	halve all values

	aColor->red >>= 1;
	aColor->green >>= 1;
	aColor->blue >>= 1;

	//	Gray has zero saturation, so it can't lose anything in
	//	the darkening process.  Thus we are done in the grayish case.

	if (!CompareComponent(*smallest, *notSmallest1) && !CompareComponent(*smallest, *notSmallest2))
		return;

	//	Find the smallest color component.

	if (CompareComponent(*smallest, *notSmallest1) > 0)
	{
		temp = smallest;
		smallest = notSmallest1;
		notSmallest1 = temp;
	}

	if (CompareComponent(*smallest, *notSmallest2) > 0)
	{
		temp = smallest;
		smallest = notSmallest2;
		notSmallest2 = temp;
	}

	//	Divide its value by two again, so that the larger components are emphasized.

	(*smallest) >>= 1;

	//	take the runner-up in the race for first place and divide it by two again so that
	//	the largest component is emphasized.  If its a tie, we're done.

	compareValue = CompareComponent(*notSmallest1, *notSmallest2);

	if (compareValue < 0)
		(*notSmallest1) >>= 1;
	else if (compareValue > 0)
		(*notSmallest2) >>= 1;
}

//	Luminance takes an RGBColor and generates a value from 0 to 0xffff equal to the percieved
//	gray-scale brightness of the color.  The algorithm (which is the same one used by the QuickDraw
//	to make such conversions) is to take a weighted average of all the components, where the weights
//	are 1/8 of the blue component, 5/16 of the red component and 9/16 of the green component.  The routine
//	is used by Brighten to decide how much to brighten a color.

unsigned long Luminance(RGBColor *aColor)
{
	unsigned long	temp;

	temp = ((unsigned long)aColor->red) << 2;		//	five times the red component
	temp += aColor->red;

	temp += ((unsigned long)aColor->green) << 3;	//	plus nine times the green component
	temp += aColor->green;

	temp += ((unsigned long)aColor->blue) << 1;		//	plus two times the blue component

	return (temp >> 4);								//	divide the sum by 16 to get the weighted average
}

//	Brighten takes an RGBColor, reduces its saturation and increases its brightness (virtually the inverse
//	of Darken).  It is used by MakeClut to take a raw label color and convert it into a color which can be
//	applied to a clut and used to label 8-bit icons.  Brightened colors will not seriously affect the
//	brightness of the icon's unselected appearance, but will retain much of the character of the original color.
//
//	A little background:  A label color is applied to a clut color by multiplying each component by the
//	percentage strength of the corresponding label color component.  For example, a label color of
//	{0x7fff, 0x3fff, 0xffff} would be interpreted as 50% red, 25% green, 100% blue and each clut color
//	would have its components multiplied by the corresponding fraction.  Since the standard label colors
//	vary greatly in their effect on a clut color and since a label color can be customized by the user to
//	any value, a label color must be processed so that it does not reduce the brightness of an affected
//	icon to the point where it looks selected or unreadably dark.  An ideal label color has low HSV saturation
//	so that no color component is so attenuated that it becomes very dark.  A pure blue for example is
//	100% saturated and would reduce all components except blue to zero, making for a very dark icon.  A low
//	saturation color has a lot of gray in it, thus allowing some representation from all colors in the icon.
//	A good label color should also have a value (in the HSV sense of the word value) of 100%; that is, it should
//	have at least one component that is 100%, so that the overall value of the resultant color is not greatly reduced.
//	Finally, whatever process is used to turn a bad label color into a good label color should preserve the HSV hue
//	of the color, since that is what gives the color its "character".  A further constraint on the algorithm to make
//	any label color usable is that it be sensitive to the hue.  For example, an unprocessed blue label color of a
//	certain saturation and value will make an icon look darker than an unprocessed green label color of the same
//	saturation and value, because the human eye is many times more sensitive to green than to blue.  Thus a blue
//	label color needs to have its saturation reduced more than a green label color.
//
//	The algorithm:  The processing of any arbitrary color into a good label color takes place conceptually in three
//	steps (although it it implemented in one pass through the components).  The first step is to increase the HSV
//	value of the color to 100%, while nearly maintaining the hue.  This is done by multiplying each component by a
//	factor which takes the maximum component to 0xffff, thus "stretching" the color to its maximum value.
//	The second step multiplies the color by a fraction related to the original luminance of the label color.  This
//	makes the color closer to a gray (i.e. lowers its saturation) and incidentally lowers its over all value (brightness).
//	The third step compensates for the incidental loss of brightness by adding in a constant value, which is the complement
//	of the compression factor.  The maximum component in the resulting color is 100%, the saturation has been lowered to
//	an extent related to the luminance and the hue is maintained.

void Brighten(RGBColor *labelColor)
{
	unsigned short	*component = (unsigned short *)labelColor,
					*end = component + 3;

	unsigned long	temp,
					luminance = Luminance(labelColor);

	unsigned short	basis,
					max = labelColor->red;

	//	Find the maximum component

	if (labelColor->green > max)
		max = labelColor->green;

	if (labelColor->blue > max)
		max = labelColor->blue;

	//	This is the ultimate brightening.  Black gets mapped to white.  This implements the "feature"
	//	that specifying black as a label color means "label this with a text only.  Don't apply a color
	//	to this icon."  White will in no way affect the appearance of the icon.  This was the lowest impact
	//	change to implement this feature.  A more optimal solution in terms of performance would be to
	//	bail in PlotIconGeneric when we see a black label color.  This implies that we should store the
	//	label color in the parameter block (since we're going to all the trouble of getting it so early on
	//	in the plot process).  This involves some broad changes in labeling code which did not seem warranted
	//	at B4 time.

	if (!max)
	{
		*((long *)labelColor) = -1;
		labelColor->blue = 0xffff;

		return;
	}

	//	here we calculate the parameters basis and luminance.  luminance is related to the gray-scale brightness
	//	of the color being brightened, but is compressed to range from 0x6000 to 0xffff.  Basis is the complement
	//	of luminance and ranges from 0x9fff to 0.  Calculating the values in this way makes the process of
	//	brightening sensitive to the color being brightened.  If the luminance of the color is low, the basis
	//	will be large and their will be a greater reduction in saturation.
	//
	//	NOTE:  the extent to which luminance is compressed into a smaller range ensures that there is a minimum
	//	saturation decrease.  In the code below, luminance is multiplied by 5/8 and 3/8 of 100% (0x6000) is added
	//	on to it to achieve the compression.  If a fraction smaller the 5/8 is used, the overall saturation of all
	//	labeled colors will increase.  Multiplying luminance by 1/2 and adding 0x8000 produces labeled icons which
	//	are slightly to dark.  Some observers have noted that the current settings produce icons whose labeling
	//	does not sufficiently suggest the color chosen from the Finder labels menu.  This is an area that bears
	//	some further investigation.

	luminance >>= 1;				//	halve luminance
	luminance += (luminance >> 2);	//	add a quarter of the halved luminance to the halved luminance to get 5/8 luminance
	luminance += 0x6000;			//	add about 3/8 of the whole range to the number
	basis = 0xffff - luminance;

	//	This loop uses the calculated parameters luminance and basic to perform the brightening.

	do {
		temp = (unsigned long)(*component);

		//	stretch the color.  If the maximum component is already 100%, skip this step for all components

		if (max < 0xffff)
		{
			temp *= 0xffff;
			temp /= max;	//	max will only be zero if the label color is black in which case we will not reach this point
		}

		//	compress the color

		temp *= luminance;
		temp /= 0xffff;

		//	add in a gray component

		*component++ = basis + (unsigned short)temp;
	} while (component < end);

	//	NOTE:  for a future version, the following loop body would have been much better.  On the other hand, the current
	//	version makes the underlying model more clear and performance is not particularly critical in this routine.
	//
	//	temp = (unsigned long)(*component);
	//
	//	if (temp < 0xffff)
	//	{
	//		temp *= luminance;
	//		temp /= max;
	//		*component = basis + (unsigned short)temp;
	//	}
	//
	//	++component;
}

//	FindBlack is called by MakeClut to find the lowest luminance color in a clut.  Why?  It's for the 4-bit labeled true CIcon
//	(don't laugh).  When such an icon is labeled, the rule is that I turn the black color to the label color.  In an Icon
//	suite, the black color we want is well-defined as the 15 entry.  In a CIcon, there may be no real black, so I'll have to
//	choose something to label.  Might as well be the closest thing to black.  NOTE:  a next version of this code should check
//	the last entry in the clut to see if it is black, and if so it should return clutSize.  If the last entry is black, that's
//	the one we want.  A good way to accomplish this would be to do the search from the last entry backward, bailing if any
//	color is black (something this code should do anyway), taking the minimum luminance value otherwise.

long FindBlack(CTabHandle theClut, long clutSize)
{
	ColorSpec		*specList = (*theClut)->ctTable;
	unsigned long	luminance,
					minLuminance = 0xffff;
	long			rval = 0,
					index = 0;

	do {
		luminance = Luminance(&(specList->rgb));

		if (luminance < minLuminance)
		{
			minLuminance = luminance;
			rval = clutSize;
		}

		++index;
		++specList;
	} while (index <= clutSize);

	return rval;
}

//	MakeClut is called by CheckClut if IconGetClut passes it a purged clut or by GetBestData if IconGetClut returns a purged clut.
//	The clut is reallocated, a copy of the ROM clut of the appropriate bit-depth (4 or 8) is copied into it, it is given
//	its old seed back and is then processed according to the transform and label.  MakeClut is also called by
//	SetupParamBlockForCIcon to process a copy of the CIcon's clut to do transforms.

void MakeClut(IconParamBlock *theBlock)
{
	short				labelNumber = theBlock->clutNumber;		//	0-7, which label, if any, we are using

	CTabHandle			theClut = theBlock->theClut;			//	the purged clut handle we are rebuilding

	short				sourceNumber;			//	0 or 1, specifies which of the ROM clut's we will use for a source

	unsigned char		*listPtr;				//	pointer to the list of clut indices to be protected with CheckColors

	long				blackEntry,				//	index of the color we will consider black for this clut (see FindBlack above)
						entryNo,
						clutSize;				//	number of entries in the processed clut

	ColorSpec			*specList;

	RGBColor			aColor,
						labelColor;

	//	If we're recreating a purged IconSuite clut, we will use the clut database in our ExpandMem
	//	globals for the source.  If we are processing a copy of a CIcon clut, we get the clut from
	//	our parameter block.

	if (theBlock->theGroup != aCIcon)
	{
		GlobalIconDataPtr	theGlobals = (GlobalIconDataPtr) GetExpandMemIconCluts();
		long				theSeed = theGlobals->seeds[labelNumber];
		CTabHandle			source;
		unsigned char		**theList;
		unsigned long		size;

		//	this convoluted arithmetic of getting a labelNumber and sourceNumber from the
		//	clutNumber (as opposed to just looking at the theLabel field) comes from the fact
		//	that theLabel specifies what label the user asked for, not necessarily the one
		//	we're going to give him.  For example, if the plotting takes place on a gray screen,
		//	the label is ignored.  The clutNumber reflects this fact, and theLabel does not.
		//	I could have had more fields in the IconParamBlock to figure all this out, but they would
		//	have to be set up by IconGetClut.  In general, it is more important for IconGetClut to be fast,
		//	since in most cases, the clut will be already created and cached.  We now return you
		//	to your regularly scheduled programming.

		//	If the transform is ttDisabled, labelNumber will be in the range of disabled clut numbers.
		//	This line brings it back into the lower range.

		if (theBlock->theTransform == ttDisabled)
			labelNumber -= ((Labels * Sets) >> 1);

		//	If the clut is in the 4-bit range, bring it into the lower range and record the fact that
		//	we will be building a 4-bit clut.

		if (labelNumber >= (Labels << 1))
		{
			labelNumber -= (Labels << 1);
			sourceNumber = 1;
			blackEntry = 15;	//	Only used if we are processing 4-bit cluts.  For non-CIcons blackEntry is always 15
		}
		else
			sourceNumber = 0;

		labelNumber >>= 1;		//	There is a selected and non-selected version of every clut.  Dividing by two puts
								//	labelNumber into the range 0-7.

		source = theGlobals->originals[sourceNumber];	//	get a handle to the source clut
		theList = theGlobals->indexLists[sourceNumber];	//	get a handle to the list of indices to protect
		size = GetHandleSize((Handle)source);			//	get the size to reallocate the purged clut
		clutSize = (*source)->ctSize;					//	get the number of entries in the clut

		ReallocHandle((Handle)theClut, size);			//	reallocate the clut
		HPurge((Handle)theClut);						//	make clut purgable

		BlockMoveData((Ptr)(*source), (Ptr)(*theClut), size);	//	copy in the source clut
		(*theClut)->ctSeed = theSeed;						//	restore its unique seed
		listPtr = *theList;									//	dereference the index list handle
	}
	else
	{
		//	a CIcon's clut is copied and processed each time it is plotted (to each screen if on multiple monitors)  Thus,
		//	the values used to determine the parameters used in processing are determined differently.

		listPtr = NULL;						//	CIcon cluts are never checked with CheckColors so listPtr is irrelevant
		clutSize = (*theClut)->ctSize;		//	theClut is already a copy so whatever we take whatever size it is

		//	For a CIcon, sourceNumber is used only to mean "Shall we process this clut in the 4-bit style (1) or the
		//	8-bit style (0)"

		if (theBlock->screenDepth < 8)
		{
			sourceNumber = 1;
			blackEntry = FindBlack(theClut, clutSize);
		}
		else
			sourceNumber = 0;
	}

	//	If there's a label get the RGBColor

	if (labelNumber)
	{
		GetLabel(labelNumber, &labelColor, NULL);

		//	If we're processing an 8-bit clut, we Brighten the color.  We don't do it in four bits because there's very little
		//	hope of achieving the subtle color effects that we can in 8-bits.  The labeling algorithm in 4-bits is different
		//	anyway and does not require the brightening process.  (See Brighten above)

		if (!sourceNumber)
			Brighten(&labelColor);
	}

	specList = (*theClut)->ctTable;		//	point to the beginning of the list of ColorSpecs in theClut

	//	Loop through all the clut entries, doing various kinds of color arithmetic to achieve the
	//	transformed clut effects.

	for (entryNo = 0; entryNo <= clutSize; ++entryNo)
	{
		//	we only do labeling and selection processing on clut entries in the index list.  That way,
		//	an icon designer could decide to use some colors for detail outside this list and not have
		//	them labeled or selected.  Thus, if we have a listPtr, we check if this entry is in the list
		//	before processing.  We process all colors otherwise (for CIcons).

		if ((!listPtr) || (entryNo == *listPtr))
		{
			//	If we have a listPtr, increment it.

			if (listPtr)
				++listPtr;

			aColor = specList->rgb;	//	make a copy of the color to process

			//	if there is a label color, apply it to this clut entry.

			if (labelNumber)
			{
				//	if the clut is 4-bit, set the black entry of the clut to the label color.
				//	Otherwise, tint the entire clut to the brightened label color by multiplying
				//	each component of each color by the fractional strength of the corresponding
				//	component of the label color.  The net effect of this process is to "filter"
				//	the clut entry through the label color.

				if (sourceNumber)
				{
					if (entryNo == blackEntry)
						aColor = labelColor;
				}
				else
				{
					if (entryNo)
					{
						//  multiplying the components together in this fashion has the effect of "filtering"
						//	the entry color through the label color.

						aColor.red = (aColor.red * (unsigned long)(labelColor.red)) >> 16;
						aColor.green = (aColor.green * (unsigned long)(labelColor.green)) >> 16;
						aColor.blue = (aColor.blue * (unsigned long)(labelColor.blue)) >> 16;
					}
					else
					{
						//	if possible, the white entry has to be "brighter" than any other color, so instead of
						//	multiplying white by the label color (which would simply produce the label color),
						//	we bias the label color toward white and use that.

						aColor.red = ((unsigned long)(aColor.red) + labelColor.red) >> 1;
						aColor.green = ((unsigned long)(aColor.green) + labelColor.green) >> 1;
						aColor.blue = ((unsigned long)(aColor.blue) + labelColor.blue) >> 1;
					}
				}
			}

			//	If this clut is for a selected icon, we apply the darkening algorithm to every affected entry

			if (theBlock->selected)
				Darken(&aColor);

			specList->rgb = aColor;		// record the processed color
		}
		else
			specList->value |= 4;	//	If this color is not in the protected list, we set the inhibit bit which causes
									//	CheckColors to ignore this entry.

		//	If the transform is ttDisabled, we average the color with white.  This has the affect of "bleaching" the color,
		//	making it seem "grayed" in the same sense that disabled text is shown.

		if (theBlock->theTransform == ttDisabled)
		{
			aColor = specList->rgb;
			aColor.red = ((unsigned long)(aColor.red) + 0xffff) >> 1;
			aColor.green = ((unsigned long)(aColor.green) + 0xffff) >> 1;
			aColor.blue = ((unsigned long)(aColor.blue) + 0xffff) >> 1;
			specList->rgb = aColor;
		}

		++specList;			//	Get the next entry in the clut
	}
}

//	GetDeviceCheck is called by IconGetClut to get the DeviceCheck structure corresponding to theDevice.
//	The DeviceCheck structure is used to cache information about which cluts have been checked against
//	a certain GDevice's available colors.
//
//	A little background:  CheckColors is a palette manager routine that takes a CTabHandle, a GDevice and some
//	tolerance values and returns a boolean, indicating whether the clut's colors can be represented on that device
//	to the specified tolerance.  To avoid calling CheckColors every time we plot, we cache the results of the
//	check in a list of structures stored at the end of our ExpandMem globals.  There is one structure for every
//	GDevice in the DeviceList.  Each structure contains a GDHandle to the corresponding device, the seed of the
//	device's color table at the time of the last check and two 64-bit bit-maps, each corresponding to the 64
//	different cluts used in plotting color icons.  One of the bit-maps indicates whether the clut has been
//	checked.  The other bit-map indicates the result (good or bad) of the check.  Before a clut is returned by
//	IconGetClut, the DeviceCheck for the given GDevice is consulted to see if the clut has already been checked
//	for that device, and if so, what the result was.  If the clut has never before been checked, CheckColors
//	is called, the DeviceCheck structure is updated to indicate that the clut has been checked and the
//	result is cached.

DeviceCheck *GetDeviceCheck(GDHandle theDevice, GlobalIconDataPtr theGlobals)
{
	DeviceCheck	*theCheck = &(theGlobals->deviceCheckList[0]);			//	prime the pointer to the beginning of deviceCheckList
	long		listSize = theGlobals->deviceListSize,
				theSeed = (*((*((*theDevice)->gdPMap))->pmTable))->ctSeed;	//	Get the device's ColorTable's seed

	//	Loop through all the DeviceCheck structures in the list, looking for the one corresponding to theDevice

	do {
		if (theCheck->theDevice == theDevice)
		{
			//	if we find a device, check if its seed has changed since the last check we did.  If it has, none of
			//	the cached check values are valid, so we invalidate the cache and update theSeed.

			if (theCheck->theSeed != theSeed)
			{
				theCheck->theSeed = theSeed;
				theCheck->checkMask = 0L;
				theCheck->disabledCheckMask = 0L;
			}

			return theCheck;
		}
		--listSize;
		++theCheck;
	} while (listSize);

	return NULL;
}

#define iconMaxTol 0x4000
#define iconMaxAveTol 0x2000

//	CheckClut takes a parameter block and calls CheckColors on theClut.  If theClut is purged,
//	we rebuild it first.  (NOTE:  I don't know why this routine returns a long.  Should be a Boolean)

long CheckClut(IconParamBlock *theBlock)
{
	//	if the clut dosen't exist, make it

	if (!(*(theBlock->theClut)))
		MakeClut(theBlock);

	//	the tolerances used here mean that any given color can be represented if the device has a
	//	color within 25% of	it.  The average tolerance of all colors in the clut must be within
	//	12.5%.  These values are empirically derived, but otherwise arbitrary.

	return CheckColors(theBlock->theDevice, theBlock->theClut, iconMaxTol, iconMaxAveTol);
}

//	IconGetClut gets the clut that should be used to plot an icon described by theBlock using data
//	of the theDepth depth.  On entry, theBlock has theDevice, theTransform, selected and theLabel
//	fields set up.  On exit, theBlock has theClut and clutNumber set up.  IconGetClut returns false if
//	the clut for the icon plotted to theDepth does not pass the CheckColors test for the given
//	device.  (See GetDeviceCheck above).  The basic procedure for finding the clut is to generate
//	an index into the array of cluts in the ExpandMem globals from the information in theBlock and
//	theDepth.  The clut is then checked, using the cached information in the deviceCheckList (See
//	GetDeviceCheck above) or CheckClut (see above) if necessary.  If the clut passes the test we
//	return true, otherwise we return false.  (NOTE:  again this function should return a boolean
//	in stead of a long)

long IconGetClut(IconParamBlock *theBlock, short theDepth)
{
	GlobalIconDataPtr	theGlobals = (GlobalIconDataPtr) GetExpandMemIconCluts();
	short				clutNumber;			//	index into clut array
	unsigned long		clutMask,
						*theCheckMap,
						*theResult;
	DeviceCheck			*theCheck;
	GDHandle			theDevice = theBlock->theDevice;

	//	if we are not on a gray-scale monitor, clutNumber is set to twice the label number (because
	//	there are two cluts for each label, selected and not selected).  No labeling is attempted
	//	on a gray-scale monitor.

	if ((theDevice && (!((*theDevice)->gdFlags & 0x1))))
		clutNumber = 0;
	else
		clutNumber = theBlock->theLabel << 1;

	//	Even numbered cluts are unselected, odd numbered cluts are selected.

	if (theBlock->selected)
		++clutNumber;

	//	The second half of the array contains transformed cluts

	if (theBlock->theTransform == ttDisabled)
		clutNumber += ((Labels * Sets) >> 1);

	//	the 4-bit cluts follow the 8-bit cluts in the transformed and untransformed groups

	if (theDepth < 8)
		clutNumber += (Labels << 1);

	//	Get the clut and record it and its index in theBlock.

	theBlock->theClut = theGlobals->colorTables[clutNumber];
	theBlock->clutNumber = clutNumber;

	//	theDevice will be NULL if we are plotting to a printer or to pict.  If so,
	//	we don't do any checks.

	if (!theDevice)
		return true;

	//	We get the DeviceCheck structure corresponding to the device we're checking.  If GetDeviceCheck
	//	returns NULL, it means that the caller is plotting to an off-screen GDevice (which is OK and will
	//	work).  We won't have any information cached for such a device so we call CheckClut immediately.

	if (theCheck = GetDeviceCheck(theDevice, theGlobals))
	{
		//	get clutNumber into the 0-31 range and set theCheckMap and theResult to either the disabled
		//	or non-disabled check bit-maps (See GetDeviceCheck above for a general description of
		//	of these bit-maps)

		if (clutNumber >= ((Labels * Sets) >> 1))
		{
			clutNumber -= ((Labels * Sets) >> 1);
			theCheckMap = &(theCheck->disabledCheckMask);
			theResult = &(theCheck->disabledCheckResults);
		}
		else
		{
			theCheckMap = &(theCheck->checkMask);
			theResult = &(theCheck->checkResults);
		}

		//	create a one-bit mask to check against the bit-maps

		clutMask = 1 << clutNumber;

		//	if the clut has never been checked against this device, update theCheckMap, call CheckClut
		//	and record the result.

		if (!((*theCheckMap) & clutMask))
		{
			(*theCheckMap) |= clutMask;

			if (CheckClut(theBlock))
				(*theResult) |= clutMask;
			else
				(*theResult) &= ~clutMask;
		}

		//	if the result of the check was that this clut cannot be well-represented on this device,
		//	return false

		if (!((*theResult) & clutMask))
			return false;
	}
	else
		return CheckClut(theBlock);

	return true;
}

//	__GetLabel is used to get information about the system labels.  It is called by the
//	trap dispatcher to implement the public GetLabel call.  labelNumber specifies which
//	label we are querying.  labelColor and labelString are buffers into which store the
//	label's RGB value and text, respectively.  Either labelColor or labelString can be
//	NULL to indicate that no information is required about the color or the text.

pascal OSErr __GetLabel(short labelNumber, RGBColor *labelColor,
						Str255 labelString)
{
	GlobalIconDataPtr	theGlobals;

	//	if labelNumber is out of range we return an error

	if ((labelNumber < 0) || (labelNumber > 7))
		return paramErr;

	//	Get a pointer to the global parameter block hanging off of ExpandMem

	theGlobals = (GlobalIconDataPtr) GetExpandMemIconCluts();

	//	If we don't have color QuickDraw, the global parameter block will not
	//	contain any label color information.  (Should this return an error?)

	if (labelColor && HaveCQD())
		*labelColor = theGlobals->labelColors[labelNumber];

	if (labelString)
	{
		Handle	aHandle = theGlobals->labelStrings[labelNumber];
		LoadResource(aHandle);

		if (ResErr)
			return ResErr;

		BlockMoveData((Ptr)(*aHandle), (Ptr)labelString, (long)(**aHandle) + 1);
	}

	return noErr;
}

//	InvalLabelCluts is called by SetLabel to purge all the icon-plotting cluts associated with a
//	label whose color has been changed

#define EightBitNonSelected 0
#define EightBitSelected 1
#define FourBitNonSelected (Labels * 2)
#define FourBitSelected (FourBitNonSelected + 1)
#define DisabledEightBitNonSelected ((Labels * Sets) / 2)
#define DisabledEightBitSelected (DisabledEightBitNonSelected + 1)
#define DisabledFourBitNonSelected (DisabledEightBitNonSelected + (Labels * 2))
#define DisabledFourBitSelected (DisabledFourBitNonSelected + 1)

pascal void InvalLabelCluts(short labelNumber)
{
	CTabHandle			*clutPtr;

	//	Get a pointer to the unselected, non-disabled, 8-bit CTabHandle for this label in the
	//	global array of icon-plotting cluts and purge it.
	clutPtr = &(((GlobalIconDataPtr) GetExpandMemIconCluts())->colorTables[labelNumber << 1]);
	EmptyHandle((Handle)(*clutPtr + EightBitNonSelected));

	//	purge the 8-bit, selected clut
	EmptyHandle((Handle)(*(clutPtr + EightBitSelected)));

	//	purge the 4-bit non-selected clut
	EmptyHandle((Handle)(*(clutPtr + FourBitNonSelected)));

	//	purge the 4-bit selected clut
	EmptyHandle((Handle)(*(clutPtr + FourBitSelected)));

	//	purge the 8-bit, non-selected, disabled clut
	EmptyHandle((Handle)(*(clutPtr + DisabledEightBitNonSelected)));

	//	purge the 8-bit, selected, disabled clut
	EmptyHandle((Handle)(*(clutPtr + DisabledEightBitSelected)));

	//	purge the 4-bit, non-selected, disabled clut
	EmptyHandle((Handle)(*(clutPtr + DisabledFourBitNonSelected)));

	//	purge the 4-bit, selected, disabled clut
	EmptyHandle((Handle)(*(clutPtr + DisabledFourBitSelected)));
}

//	__SetLabel is used to get information about the system labels.  It is called through the
//	trap dispatcher to implement the GetLabel call, which is exported but private.  labelNumber
//	specifies which label we are modifying.  labelColor and labelString are buffers containing the
//	label's RGB value and text, respectively.  Either labelColor or labelString can be
//	NULL to indicate that the color or text is not to be changed.

pascal OSErr __SetLabel(short labelNumber, RGBColor *labelColor,
					Str255 labelString)
{
	GlobalIconDataPtr	theGlobals;
	Handle				aHandle;

	//	If the labelNumber is out of range, return a paramErr.  Note that the range of valid
	//	label numbers is different for __SetLabel than for __GetLabel.  Label 0 cannot be
	//	changed and is always black with text "None".

	if ((labelNumber < 1) || (labelNumber > 7))
		return paramErr;

	//	If labelColor is specified and the global data structure contains color information,
	//	set the label value.

	if (labelColor && HaveCQD())
	{
		//	Update the list of label colors in memory

		theGlobals = (GlobalIconDataPtr) GetExpandMemIconCluts();
		theGlobals->labelColors[labelNumber] = *labelColor;

		//	Update the label color in the system file

		aHandle = GetResource('rgb ', labelColorBase + labelNumber);
		*((RGBColor *)(*aHandle)) = *labelColor;

		HNoPurge(aHandle);
		ChangedResource(aHandle);
		WriteResource(aHandle);
		HPurge(aHandle);

		//	Invalidate cluts associated with labelNumber
		InvalLabelCluts(labelNumber);
	}

	//	if there is a label string to set…

	if (labelString)
	{
		unsigned char size = *labelString + 1;

		//	if the string is larger than a Str32, truncate it

		if (size > 32)
			size = 32;

		//	update the string in memory.

		theGlobals = (GlobalIconDataPtr) GetExpandMemIconCluts();
		aHandle = theGlobals->labelStrings[labelNumber];
		LoadResource(aHandle);
		BlockMoveData((Ptr)labelString, *aHandle, (long)size);
		**aHandle = size-1;

		//	update the system file

		HNoPurge(aHandle);
		ChangedResource(aHandle);
		WriteResource(aHandle);
		HPurge(aHandle);
	}

	UpdateResFile(0);

	return noErr;
}

//	These two routines are used as call-backs for PlotIconGeneric to implement
//	PlotIconHandle, PlotSICNHandle

Handle FromICON(long data, IconType theIcon)
{
	if (theIcon == largeIcon1)
		return ((Handle)data);
	else
		return NULL;
}

Handle FromSICN(long data, IconType theIcon)
{
	if ((theIcon == smallIcon1) || (theIcon == miniIcon1))
		return ((Handle)data);
	else
		return NULL;
}

//	These two routines are called through the trap dispatcher to implement the public
//	calls PlotIconHandle and PlotSICNHandle.  NOTE:  It was my intention to move these
//	and their respective call-backs to IconUtils.a for speed and size efficiency.

pascal OSErr __PlotIconHandle(const Rect *theRect,
						short align,
						short transform,
						Handle theIcon)
{
	return PlotIconGeneric(theRect, align, transform, (long)theIcon, FromICON, false);
}

pascal OSErr __PlotSICNHandle(const Rect *theRect,
						short align,
						short transform,
						Handle theIcon)
{
	return PlotIconGeneric(theRect, align, transform, (long)theIcon, FromSICN, false);
}

//	The QuickDraw.h definition of a CIcon includes a one-byte array at the end as a place-holder
//	for the variable-sized mask data.  This is a CIcon definition that includes only the fixed-size
//	data for a sizeof() operator.

typedef struct
{
	PixMap iconPMap;				/*the icon's pixMap*/
	BitMap iconMask;				/*the icon's mask*/
	BitMap iconBMap;				/*the icon's bitMap*/
	Handle iconData;				/*the icon's data*/
} myCIcon;

//	SetupParamBlockForCIcon initializes a parameter block for a true CIcon.  It is called by
//	__PlotCIconHandle after the theBlock has its theRect, userProc, theTransform, theLabel
//	selected and dontDraw fields initialized.  On exit theBlock is setup to be called by
//	PlotTheCIcon (See below).

void SetupParamBlockForCIcon(IconParamBlock *theBlock)
{
	//	the userProc field is used to hold a pointer to the locked CIcon structure
	CIconPtr		iconPtr = (CIconPtr)(theBlock->userProc);

	short			height = iconPtr->iconMask.bounds.bottom - iconPtr->iconMask.bounds.top;
	unsigned long	maskDataSize = (iconPtr->iconMask.rowBytes * height);

	//	oneBitImageHandle is a Handle which we will set up to take the place of the one-bit
	//	member of an icon suite.  We will be setting up maskMap to point to the mask data
	//	within the CIcon itself, so we only need to make oneBitImageHandle as big as the
	//	one-bit image data.
	Handle			oneBitImageHandle = NewHandle(maskDataSize);

	//	theOneBitData points to the one-bit data (one-bit image and mask) for this CIcon.
	//	oneBitPtr is the beginning of the one-bit image data.  It is initialized to theOneBitData
	//	because it is legal to omit the one-bit image data from a CIcon, in which case we use
	//	the mask as the one-bit data.
	unsigned char	*theOneBitData = (unsigned char *)(iconPtr->iconMaskData),
					*oneBitPtr = theOneBitData;

	RgnHandle		maskRgn = NewRgn();
	GrafPtr			curPort = myGetPort();

	HLock(oneBitImageHandle);

	//	If there is one bit-data, we point oneBitPtr to it.  Otherwise, we leave it pointing
	//	to the mask data.

	if (theBlock->userData > (sizeof(myCIcon) + maskDataSize))
		oneBitPtr += maskDataSize;

	//	copy the one-bit image data to the simulated one-bit suite entry and point the theMask
	//	field in the IconParamBlock to point at it.

	BlockMoveData((Ptr)oneBitPtr, *oneBitImageHandle, maskDataSize);
	theBlock->theMask = oneBitImageHandle;

	//	copy the bounds and rowBytes information from the CIcon's iconMask into theBlock->maskMap
	//	and point the baseAddr field to the mask data within the CIcon

	BlockMoveData((Ptr)(&(iconPtr->iconMask)), (Ptr)(&(theBlock->maskMap)), sizeof(BitMap));
	theBlock->maskMap.baseAddr = (Ptr)theOneBitData;

	//	copy all the PixMap information from the CIcon into the PixMap within theBlock and set
	//	the theData field to point to the color data from the CIcon.

	BlockMoveData((Ptr)(&(iconPtr->iconPMap)), (Ptr)(&(theBlock->dataMap)), sizeof(PixMap));
	theBlock->theData = iconPtr->iconData;

	//	We use BitMapToRegion to generate the mask region used in doing transforms, etc.
	//	From this region we also derive our boundary information.  userData is overloaded for
	//	CIcons to contain the handle to the mask region, since unlike icon suites, this region
	//	is generated once as opposed to when needed.

	BitMapToRegion(maskRgn, &(theBlock->maskMap));
	theBlock->boundary = (*maskRgn)->rgnBBox;
	theBlock->userData = (long)maskRgn;

	//	Initialize miscelaneous fields for plotting

	theBlock->useCopyMask = !((curPort->grafProcs) || (curPort->picSave));
	theBlock->dstMap = &(curPort->portBits);
	theBlock->dataMap.pixelSize = 0;
	theBlock->theClut = NULL;
	theBlock->rowBytes = theBlock->maskMap.rowBytes;

	theBlock->useRegions = true;	//	we don't have any choice with CIcon's
}

//	myColorTable is a ColorTable with only the fixed-sized portion for a sizeof() operator

typedef struct
{
	long ctSeed;					/*unique identifier for table*/
	short ctFlags;					/*high bit: 0 = PixMap; 1 = device*/
	short ctSize;					/*number of entries in CTTable*/
} myColorTable;

//	CopyClut is called by GetCIconClut to create a clut that is the same as input clut but
//	padded to the depth of the pixMap data to which it belongs.  Thus a 5-entry clut for a
//	4-bit data pixMap would be padded to 16 entries.  If a clut needs padding, the entries
//	not specified are set to a ramp between white and black.  This is to simulate the fact
//	that relative colors in a CIcon are assigned to a ramp between foreGround and backGround.
//	Since we always set foreGround to black and backGround to white, this will have the desired effect.

CTabHandle CopyClut(CTabHandle theClut, short depth)
{
	long		specArraySize = sizeof(ColorSpec) << depth;		//	size of the CSpecArray in new clut
	CTabHandle	theNewClut = (CTabHandle)NewHandle(sizeof(myColorTable) + specArraySize);
	CTabPtr		oldClutPtr = *theClut,
				newClutPtr = *theNewClut;
	short		newClutEntries = (1 << depth) - 1,		//	ctSize of new clut
				oldClutEntries = oldClutPtr->ctSize;	//	ctSize of old clut

	newClutPtr->ctSeed = GetCTSeed();					//	get unique seed
	newClutPtr->ctSize = newClutEntries;				//	initialize ctSize entry
	newClutPtr->ctFlags = oldClutPtr->ctFlags;			//	copy flags from old clut to new

	//	copy common data between the two cluts (yeah, and some garbage as well)
	BlockMoveData((Ptr)(oldClutPtr->ctTable), (Ptr)(newClutPtr->ctTable), specArraySize);

	//	if there is no difference in CSpecArray size, we are done now.
	if (newClutEntries > oldClutEntries)
	{
		ColorSpec	tempColor,
					*theTable = newClutPtr->ctTable;

		*((long *)(&(tempColor.rgb))) = 0;			//	set the last entry to black
		tempColor.rgb.blue = 0;
		tempColor.value = newClutEntries;
		theTable[newClutEntries] = tempColor;

		++oldClutEntries;							//	we are now looking one entry beyond the
													//	last entry in the old clut

		if (newClutEntries > oldClutEntries)		//	Is there more than one extra spot to fill?
		{
			long			index;
			unsigned short	increment = 0xffff / (newClutEntries - oldClutEntries),
							value = 0;

			*((long *)(&(tempColor.rgb))) = -1;		//	set entry after the last specified to white
			tempColor.rgb.blue = 0xffff;
			tempColor.value = oldClutEntries;
			theTable[oldClutEntries] = tempColor;

			//	fill all intermediate entries with an evenly spaced ramp from white to black

			for (index = newClutEntries - 1; index > oldClutEntries; --index)
			{
				value += increment;
				tempColor.rgb.red = value;
				tempColor.rgb.green = value;
				tempColor.rgb.blue = value;
				tempColor.value = index;
				theTable[index] = tempColor;
			}
		}
	}

	return theNewClut;
}

//	This is the CIcon couterpart to IconGetClut.  It is called by PlotTheCIcon (Below) to get
//	the appropriate clut to use for plotting to deep devices.

pascal void GetCIconClut(IconParamBlock *theBlock, GDHandle theDevice, short flags)
{
	CTabHandle	theClut = ((CIconPtr)(theBlock->userProc))->iconPMap.pmTable;
	short		clutDepth = ((CIconPtr)(theBlock->userProc))->iconPMap.pixelSize,
				clutNumber;

	//	copy the CIcon's clut and pad if necessary

	theBlock->theClut = CopyClut(theClut, clutDepth);

	//	clutNumber is either set to the label number or zero if	we're on a gray-scale device.
	//	MakeClut (See above) uses this value differently for CIcons than for icon suites

	if (theDevice && (!(flags & 0x1)))
		clutNumber = 0;
	else
		clutNumber = theBlock->theLabel;

	theBlock->clutNumber = clutNumber;

	//	process the copied clut appropriately for our device, transform and label

	MakeClut(theBlock);
}

//	PlotTheCIcon is called directly by __PlotCIconHandle (Below) or indirectly via DeviceLoop.
//	It is responsible for deciding whether the color icon data will be plotted or the one-bit data.
//	When PlotTheCIcon is called from DeviceLoop, we make use of DeviceLoop's setting up of visRgn to
//	clip the image appropriately.  (In PlotTheIcon, we do our own clipping and only use DeviceLoop
//	to iterate through the active devices.)

pascal void PlotTheCIcon(short			screenDepth,
						short			flags,
						GDHandle		theDevice,
						IconParamBlock	*theBlock)
{
	theBlock->screenDepth = screenDepth;
	theBlock->theDevice = theDevice;

	//	if our monitor is deeper than four bits, or if our monitor is four bits and our icon is
	//	not selected or disabled, then we can plot the deep data.  Otherwise we plot the one-bit
	//	data.

	if (((screenDepth > 4) && (theBlock->theTransform != ttOpen)) ||
		((screenDepth == 4) && (!(theBlock->selected)) && (!(theBlock->theTransform & ttDisabled))))
	{
		theBlock->dataDepth = ((CIconPtr)(theBlock->userProc))->iconPMap.pixelSize;	//	set up dataDepth for PlotDeep
		GetCIconClut(theBlock, theDevice, flags);									//	get a processed copy of the clut
		PlotDeep(theBlock);															//	plot the icon
		DisposHandle((Handle)(theBlock->theClut));									//	dispose of our copy of the clut
		theBlock->theClut = NULL;
	}
	else
	{
		theBlock->dataDepth = 1;
		theBlock->theClut = NULL;
		PlotShallow(theBlock);
	}
}

//	__PlotCIconHandle is called through the trap dispatcher to implement the PlotCIconHandle call.  It allows
//	a CIcon to be plotted, using the modifiers for an icon suite.

pascal OSErr __PlotCIconHandle(const Rect *theRect,
							short align,
							short transform,
							CIconHandle theIcon)
{
	IconParamBlock	theBlock;
	char			iconState;

	//	CIcon's don't exist on non-color machines.

	if (!HaveCQD())
		return paramErr;

	//	Since PlotCIconHandle is slower than PlotCIcon, we fall through to PlotCIcon when there is no alignment
	//	or transform.  For visual consistancy, we set fore and back colors to black and white, because PlotDeep
	//	(See above) does that before plotting deep data.

	if ((!align) && (!transform))
	{
		ColorSpec	foreGround,
					backGround;

		SaveFore(&foreGround);
		SaveBack(&backGround);

		ForeColor(blackColor);
		BackColor(whiteColor);

		PlotCIcon(theRect, theIcon);

		RestoreFore(&foreGround);
		RestoreBack(&backGround);
		return noErr;
	}

	//	Lock down the CIcon while we plot it.

	iconState = HGetState((Handle)theIcon);
	HLock((Handle)theIcon);

	//	set up for call to SetupParamBlockForCIcon

	theBlock.theGroup = aCIcon;
	theBlock.userData = GetHandleSize((Handle)theIcon);		//	Going into SetupParamBlockForCIcon, userData is the size of theIcon,
															//	coming out, it is the mask region.
	theBlock.theRect = *theRect;
	theBlock.userProc = (IconRetriever)(*theIcon);
	theBlock.theTransform = transform & transformMask;
	theBlock.theLabel = (transform & labelMask) >> labelShift;
	theBlock.selected = transform & ttSelected;
	theBlock.dontDraw = false;

	SetupParamBlockForCIcon(&theBlock);

	//	do alignment, if any

	if (align)
		PerformAlignment(&theBlock, align);

	//	map the region to the size of the destination rect

	AdjustRgn((RgnHandle)(theBlock.userData),
			&(theBlock.maskMap.bounds),
			&(theBlock.theRect));

	//	If we're plotting to a BitMap as opposed to a PixMap, we call PlotTheCIcon directly with a depth of 1.
	//	Otherwise, we need to to decide more carefully how to plot.

	if (theBlock.dstMap->rowBytes < 0)
	{
		GlobalIconDataPtr	theGlobals = (GlobalIconDataPtr) GetExpandMemIconCluts();
		GDHandle			theScreen;

		//	If we're assuming we're plotting to a given monitor, we call PlotTheCIcon directly for that
		//	monitor.  Otherwise, we call DeviceLoop and let PlotTheCIcon decide for itself how to plot
		//	the data on monitor-by-monitor basis.

		if (theScreen = theGlobals->virtualScreen)
			PlotTheCIcon((*((*theScreen)->gdPMap))->pixelSize, 0, theScreen, &theBlock);
		else
			DeviceLoop((RgnHandle)(theBlock.userData),
					(DeviceLoopDrawingProcPtr)PlotTheCIcon,
					(long)(&theBlock), 0);
	}
	else
		PlotTheCIcon(1, 1, NULL, &theBlock);

	//	restore the handle state of the CIconHandle

	HSetState((Handle)theIcon, iconState);

	//	dispose of the mask region

	DisposeRgn((RgnHandle)(theBlock.userData));

	//	dispose of the one-bit data handle we created in SetupParamBlockForCIcon

	DisposHandle((Handle)(theBlock.theMask));
}

#else

#ifndef __ICONS__
#include <Icons.h>
#endif

#ifndef __ERRORS__
#include <Errors.h>
#endif

#ifndef __QUICKDRAW__
#include <QuickDraw.h>
#endif

#ifndef __PALETTES__
#include <Palettes.h>
#endif

#include <IconUtilsPriv.h>
#include <PalettesPriv.h>

#ifndef __MEMORY__
#include <Memory.h>
#endif

#ifndef __RESOURCES__
#include <Resources.h>
#endif

#ifndef __TRAPS__
#include <Traps.h>
#endif

#ifndef __SYSEQU__
#include <SysEqu.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#ifndef __QDOFFSCREEN__
#include <QDOffScreen.h>
#endif

GrafPtr myGetPort() =
{
	0x2255,				//		MOVEA.L		(A5), A1	;	Get Pointer to globals
	0x2011				//		MOVE.L		(A1), D0	;	Return current grafport
};

ConstPatternParam GetBlack() =
{
	0x70F0,			//	MOVEQ.L		black, D0
	0xD095			//	ADD.L		GrafGlobals(A5), D0
};

ConstPatternParam GetHalfGray() =
{
	0x70E8,			//	MOVEQ.L		gray, D0
	0xD095			//	ADD.L		GrafGlobals(A5), D0
};

ConstPatternParam GetLightGray() =
{
	0x70E0,			//	MOVEQ.L		ltGray, D0
	0xD095			//	ADD.L		GrafGlobals(A5), D0
};

#pragma parameter __D0 myGetHandleSize (__A0)
long myGetHandleSize(Handle theHandle) = _GetHandleSize;

#define MemErr			(*((short *)0x220))
#define ResErr			(*((short *)0xA60))
#define DeviceList		(*((GDHandle *)0x8A8))
#define TheGDevice		(*((GDHandle *)0xCC8))
#define	ROM85			(*(unsigned short *) 0x28E)
#define	HaveCQD()		(ROM85 <= 0x3fff)

#pragma parameter __D0 ShortMul (__D1,__D0)
long ShortMul(unsigned short x, unsigned short y) = 0xc0c1;

#pragma parameter __D0 ShortDiv (__D0, __D1)
short ShortDiv(unsigned long x, unsigned short y) = 0x80c1;

#define MakeLong(aThing) (*((long *)(&(aThing))))

//	This external table is defined in IconUtils.a

extern ResType typeTable[IconSpace];

//	typedef for call-back used internally for routines like PlotIconGeneric, HitTestIconGeneric, etc.
typedef Handle (*IconRetriever)(long data, IconType theIcon);

GlobalIconDataPtr GetGlobalPtr();
GlobalIconDataHandle GetGlobalHandle();

//	the folowing is the definition of the parameter block used throughout the code

typedef struct
{
	PixMap			dataMap;		//	PixMap (or BitMap) structure used for icon image data
	BitMap			maskMap;		//	BitMap used for icon mask data
	BitMapPtr		dstMap;			//	Pointer to the destination BitMap
	Handle			theMask,		//	Handle to one-bit data (usually with mask appended)
					theData;		//	Handle to 4 or 8 bit icon image data
	Rect			theRect,		//	destination rectangle (as modified by alignment)
					boundary;		//	boundary of non-zero data within the maskMap bounds.
	short			rowBytes;		//	basic rowBytes of data (2 or 4)
	unsigned short	theTransform,	//	transform broken out from transform code
					theLabel,		//	label broken out from transform code
					selected,		//	selected state broken out from transform code
					dontDraw,		//	flag which is set to true for LoadIconCache, cuts operation short before drawing
					onScreen,		//	true if the icon is being drawn to a video device, false if printing or offscreen destination
					screenDepth,	//	used to record the depth of the device we'll be drawing to
					dataDepth,		//	used to record the depth of the data we'll be drawing
					clutNumber;		//	index into an array of CTabHandles, specifies CLUT we will use with dataMap
	Boolean			useRegions;		//	true if we will use regions to perform transforms, false normally
	IconType		theGroup;		//	size category of icon we are using (mini, small or large) also used to indicate a cicn
	long			userData;		//	data for icon-data-getting call back
	IconRetriever	userProc;		//	ProcPtr to icon-data-getting call back
	CTabHandle		theClut;		//	handle to CLUT we will use with dataMap
	GDHandle		theDevice;		//	device we are currently drawing to or NULL if black and white or if single device
	unsigned long	maskBuffer[32];	//	buffer in which we store masks we create for one reason or another
} ParamBlock;

//	PerformAlignment offsets theBlock->theRect to cause the icon to have the alignment specified by the caller

pascal void PerformAlignment(ParamBlock *theBlock, short align)
{
	short	offsetx,
			offsety;

	Rect	*theRect = &(theBlock->theRect),
			theBoundary = theBlock->boundary;

	MapRect(&theBoundary, &(theBlock->dataMap.bounds), theRect);

	switch (align & 0xC)
	{
		case atHorizontalCenter:
			offsetx = ((theRect->right + theRect->left) - (theBoundary.right + theBoundary.left)) >> 1;
			break;

		case atLeft:
			offsetx = theRect->left - theBoundary.left;
			break;

		case atRight:
			offsetx = theRect->right - theBoundary.right;
			break;

		default:
			offsetx = 0;
	}

	switch (align & 0x3)
	{
		case atVerticalCenter:
			offsety = ((theRect->bottom + theRect->top) - (theBoundary.bottom + theBoundary.top)) >> 1;
			break;

		case atTop:
			offsety = theRect->top - theBoundary.top;
			break;

		case atBottom:
			offsety = theRect->bottom - theBoundary.bottom;
			break;

		default:
			offsety = 0;
	}

	OffsetRect(theRect, offsetx, offsety);
}

//	The following routines are used to create regions from mask BitMap's for IconToRgn-type calls and
//	for hit-testing and transforms when an icon is draw larger than 32-bits.  They are faster than the
//	more general BitMapRgn code because they are optimized for a bitMap no greater that a longword in
//	width.

#define TempRgnBufferSize 100

pascal Boolean AddBufferToRgn(RgnHandle theRgn, unsigned short *rgnBuffer, unsigned short *regionPtr)
{
	unsigned long	incrementSize = (unsigned long)regionPtr - (unsigned long)rgnBuffer;
	short			rgnSize = (*theRgn)->rgnSize;

	SetHandleSize((Handle)theRgn, rgnSize + incrementSize);

	if (MemErr)
		return false;

	BlockMove((Ptr)rgnBuffer, (Ptr)(((unsigned char *)(*theRgn)) + rgnSize), incrementSize);

	(*theRgn)->rgnSize  += incrementSize;

	return true;
}

pascal void MaskRgn(RgnHandle	theRgn,
					ParamBlock	*theBlock)
{
	unsigned char	*data = theBlock->maskMap.baseAddr;		//	This may or may not be locked down

	unsigned short	rowBytes = theBlock->maskMap.rowBytes,
					rowIndex,
					rgnBuffer[TempRgnBufferSize],
					*regionPtr = rgnBuffer,
					spaceLeftInBuffer = TempRgnBufferSize;

	unsigned long	sizeMask = (rowBytes > 2) ? 0xFFFFFFFF : 0xFFFF0000,
					lastRow = 0;

	//	initialize output region
	(*theRgn)->rgnSize = sizeof(Region);
	(*theRgn)->rgnBBox = theBlock->boundary;

	data += ShortMul((unsigned short)rowBytes, (unsigned short)(theBlock->boundary.top));	//	Point to the top of real data

	//	find the inversion points row-by-row

	for (rowIndex = theBlock->boundary.top;
			rowIndex <= theBlock->boundary.bottom;
			++rowIndex, data += rowBytes)
	{
		unsigned long	thisRow,
						inversion;

		if (rowIndex != theBlock->boundary.bottom)
		{
			thisRow = *((unsigned long *)data);
			inversion = thisRow ^ lastRow;			//	inversion is a bit-map difference between this row and the last
			lastRow = thisRow;
		}
		else
			inversion = lastRow;					//	To finish the region, we compare the last row to nothing (all zeros)

		inversion &= sizeMask;						//	Take only the significant data.

		//	if there is any difference between this row and the last, we describe this difference as a set of inversion points

		if (inversion)
		{
			short columnIndex = 0;

			//	I'm about to add two words to the region (the row index and the first column index, so I make one check to see
			//	if I've got two words left in my buffer.  If I don't, I dump my temporary buffer to the output region and free
			//	up the temp buffer.

			if (spaceLeftInBuffer < 2)
			{
				if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
					goto error;
				regionPtr = rgnBuffer;
				spaceLeftInBuffer = TempRgnBufferSize;
			}

			*regionPtr++ = rowIndex;

			//	find the first inversion point (may be the first point in the row)

			while (!(inversion & 0x80000000))
			{
				++columnIndex;
				inversion <<= 1;
			}

			//	record the first inversion point.

			*regionPtr++ = columnIndex++;

			//	update our gas guage.
			spaceLeftInBuffer -= 2;

			//	find and record all subsequent inversion points

			do
			{
				unsigned long check = inversion & 0xC0000000;

				if (check && (check != 0xC0000000))
				{
					//	check if I need to flush my temp buffer

					if (!spaceLeftInBuffer)
					{
						if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
							goto error;
						regionPtr = rgnBuffer;
						spaceLeftInBuffer = TempRgnBufferSize;
					}

					*regionPtr++ = columnIndex;
					--spaceLeftInBuffer;
				}

				columnIndex++;
				inversion <<= 1;
			} while (inversion);

			//	cap off the row with a row-capping marker.

			if (!spaceLeftInBuffer)
			{
				if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
					goto error;
				regionPtr = rgnBuffer;
				spaceLeftInBuffer = TempRgnBufferSize;
			}

			*regionPtr++ = 0x7FFF;
			--spaceLeftInBuffer;
		}
	}

	if (!spaceLeftInBuffer)
	{
		if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
			goto error;
		regionPtr = rgnBuffer;
		spaceLeftInBuffer = TempRgnBufferSize;
	}

	*regionPtr++ = 0x7FFF;	//	cap off the rgn

	//	If the region is purely rectangular, minimize it and return it.

	if (((*theRgn)->rgnSize == sizeof(Region)) &&
		(((unsigned long)regionPtr - (unsigned long)rgnBuffer) == 18) &&
			(rgnBuffer[1] == rgnBuffer[5]) &&
			(rgnBuffer[2] == rgnBuffer[6]))
	{
		SetHandleSize((Handle)theRgn, sizeof(Region));
		return;
	}

	//	Flush the remainder of the buffer to the region.

	if (!AddBufferToRgn(theRgn, rgnBuffer, regionPtr))
		goto error;

	return;

error:
	(*theRgn)->rgnSize = sizeof(Region);
}

//-------------------------------------------------------------------------------------
//	Search utilities.
//-------------------------------------------------------------------------------------

//	GetBestData finds the best available data that is appropriate for the destination device.
//	Initially, theBlock->dataDepth is set equal to the screen depth (by SetUpParamBlock).  On
//	exit, theData is set to the best available color data, theClut is set to the appropriate
//	CLUT to use, and dataDepth is updated to reflect the depth of the output data.  If no color
//	data is available or appropriate, theData and theClut are set NULL and dataDepth is set to 1,
//	all of which means that this is a one-bit plot.  IconGetClut calls CheckClut to see if the clut
//	of the specified depth fits into the current color environments.  It returns false if it is not,
//	causing us to degrade to a smaller bit-depth.

pascal void GetBestData(ParamBlock *theBlock)
{
	long			theData = theBlock->userData,
					IconGetClut(ParamBlock *, short);

	short			checkDepth = 8,
					whichIcon = theBlock->theGroup + largeIcon8;	//	whichIcon is initialized to the code
																	//	for the deepest data in our size group

	long			IconGetClut(ParamBlock *, short);

	IconRetriever	theProc = theBlock->userProc;
	void			MakeClut(ParamBlock *);

	do {
		if ((theBlock->dataDepth >= checkDepth)
			&& IconGetClut(theBlock, checkDepth)
			&& (theBlock->theData = (*theProc)(theData, whichIcon)))
		{
			theBlock->dataDepth = checkDepth;

			//	if the the clut returned by IconGetClut is purged, build a new one

			if (!(*(theBlock->theClut)))
				MakeClut(theBlock);

			return;
		}

		checkDepth -= 4;
		--whichIcon;
	} while (checkDepth);

	theBlock->dataDepth = 1;
	theBlock->theClut = NULL;
}

//	ChooseBestSize maps a destination rectangle size to an icon size category.

short ChooseBestSize(const Rect *theRect)
{
	int			width = theRect->right - theRect->left,
				height = theRect->bottom - theRect->top;

	if ((width >= 32) || (height >= 32))
		return largeIcon1;

	if (height > 12)
		return smallIcon1;

	return miniIcon1;
}

#define FIXED(a)		(long)(((a)<<16))
#define THEPIXMAP		((PixMapPtr)theBlock)	// only works if the pixmap is the first element

//	SetUpPixMap initializes the necessary fields of dataMap for a color plot.

pascal void SetUpPixMap(ParamBlock *theBlock)
{
	unsigned short		depth = theBlock->dataDepth;

	THEPIXMAP->baseAddr = *(theBlock->theData);

	//	we use the pixelSize field to indicate whether or not we already have the dataMap
	//	initialized for a color plot, and if so to what depth.

	if (THEPIXMAP->pixelSize == depth)
		return;

	THEPIXMAP->pmVersion = 0;
	THEPIXMAP->packType = 0;
	THEPIXMAP->packSize = 0;
	THEPIXMAP->hRes = FIXED(72);
	THEPIXMAP->vRes = FIXED(72);
	THEPIXMAP->pixelType = 0;
	THEPIXMAP->cmpCount = 1;
	THEPIXMAP->planeBytes = 0;

	THEPIXMAP->pixelSize = depth;
	THEPIXMAP->cmpSize = depth;

	//	if the icon is a true CIcon, the rowBytes could be any arbitrary value.  Otherwise,
	//	we can calculate the proper PixMap rowBytes value from the basic rowBytes and the
	//	depth.

	if (theBlock->theGroup != aCIcon)
		THEPIXMAP->rowBytes = ShortMul((unsigned short)(theBlock->rowBytes), depth) | 0x8000;
	else
		THEPIXMAP->rowBytes = ((CIconPtr)(theBlock->userProc))->iconPMap.rowBytes;

	THEPIXMAP->pmTable = theBlock->theClut;
}

//-------------------------------------------------------------------------------------
//	IconSuite Manipulation routines.
//-------------------------------------------------------------------------------------

//	__NewIconSuite is called directly by the dispatcher to implement the NewIconSuite call.

pascal OSErr __NewIconSuite(IconSuiteHandle *theSuite)
{
	*theSuite = (IconSuiteHandle)NewHandleClear(sizeof(IconSuite));
	return MemErr;
}

//	__MakeIconCache is called directly by the dispatcher to implement the MakeIconCache call.

pascal OSErr __MakeIconCache(IconCacheHandle *theHandle,
						IconGetter makeIcon,
						void *yourDataPtr)
{
	IconCacheHandle	theCache;

	if (!(theCache = (IconCacheHandle)NewHandleClear(sizeof(IconCache))))
		return MemErr;

	(*theCache)->theSuite.type = CACHE;		//	set the suite type to indicate that this is a cache
	(*theCache)->userPtr = yourDataPtr;		//	install the call-back and the data.
	(*theCache)->userMethod = makeIcon;

	*theHandle = theCache;

	return noErr;
}

//	EditIconSuite is called by __AddIconToSuite and __GetIconFromSuite (defined in IconUtils.a)
//	If isAdd is false, EditIconSuite assumes refCon is a pointer to a Handle and returns the
//	entry of type theType from theSuite (possibly NULL).  If isAdd is true, the value in refcon
//	is added to theSuite in the slot for data of type theType.

pascal OSErr EditIconSuite(long refCon,		//	Handle or referenct to handle of icon data
							IconSuiteHandle theSuite,
							ResType theType,
							long isAdd)
{
	Handle	*theSlot = (*theSuite)->table;
	long	theEnd = (long)(theSlot + IconSpace);
	ResType	*theTable = typeTable;

	do
	{
		if (theType == *theTable++)
		{
			if (isAdd)
				*theSlot = (Handle)refCon;
			else
				*((Handle *)refCon) = *theSlot;

			return noErr;
		}
	} while ((long)(++theSlot) < theEnd);

	return paramErr;
}

//	__ForEachIconDo is called by the trap dispatcher to implement the ForEachIconDo call.  The action
//	call-back is called for every member of theSuite specified by selector (See ERS for selector values).

pascal OSErr __ForEachIconDo(IconSuiteHandle theSuite,
							unsigned long selector,
							IconAction action,
							void *yourDataPtr)
{
	Handle 	*theTable = (Handle *)((*theSuite)->table);
	long	end = (long)(theTable + IconSpace);		// there are more data registers than address
	ResType *theTypes = typeTable;
	OSErr	rval = noErr;

	char theState = HGetState((Handle)theSuite);
	HLock((Handle) theSuite);

	do
	{
		short i = IconSizeSpace;

		do
		{
			if (selector & 0x00000001)
				if (rval = (*action)(*theTypes, theTable, yourDataPtr))
					goto out;

			theTable++;
			theTypes++;
			selector >>= 1;
		} while (--i);

		selector >>= (8 - IconSizeSpace);
	} while (theTable < (Handle *)end);

out:
	HSetState((Handle) theSuite, theState);

	return rval;
}

//	This is the call back used with ForEachIconDo to implement the __GetIconSuite
//	call below.  A more sophisticated call-back could be used to implement a
//	Get1IconSuite or to ensure that GetIconSuite only got icons from a single
//	file.  A user could do this kind of thing by rewriting GetIconSuite with
//	their own call-back.

pascal OSErr GetIconFromResource(ResType theType, Handle *theIcon, void *yourDataPtr)
{
	*theIcon = GetResource(theType, (short)yourDataPtr);
	return noErr;
}

//	Implements the GetIconSuite call.  The selector allows only specific members of the
//	suite to be asked for.

pascal OSErr __GetIconSuite(Handle *theHandle, short theID, unsigned long selector)
{
	if (__NewIconSuite((IconSuiteHandle *)theHandle))
		return MemErr;

	__ForEachIconDo((IconSuiteHandle)(*theHandle), selector, GetIconFromResource, (void *)theID);

	return noErr;
}

//	Implements the SetSuiteLabel call.  This specifies a default label for a suite
//	which is used if no label is specified by the transform.  Yes, this is currently used.
//	Namely, by the process manager for the Apple Menu.

pascal OSErr __SetSuiteLabel(IconSuiteHandle theSuite, unsigned short theLabel)
{
	if (theLabel > 7)
		return paramErr;

	theLabel <<= labelShift;
	(*theSuite)->label = theLabel;
	return noErr;
}

//	Implements the GetSuiteLabel.  Just for completeness.

pascal short __GetSuiteLabel(IconSuiteHandle theSuite)
{
	return (((*theSuite)->label) >> labelShift);
}

//	This is the call back used with ForEachIconDo to implement DisposIconSuite.  An icon
//	is only disposed if it is not a resource.

pascal OSErr Disposer(ResType , Handle *theIcon, void *)
{
	if (*theIcon && (HomeResFile(*theIcon) == -1))
		DisposHandle(*theIcon);

	*theIcon = NULL;

	return noErr;
}

//	Implements the DisposeIconSuite call.  disposeData indicates whether an attempt should
//	be made to dispose of the data in the suite.  Only non-resource handles will be disposed.

pascal OSErr __DisposeIconSuite(IconSuiteHandle theSuite, Boolean disposeData)
{
	if (disposeData)
		(void)__ForEachIconDo(theSuite, svAllAvailableData, Disposer, NULL);

	DisposHandle((Handle)theSuite);

	return MemErr;
}

//	These are the call-backs used with all the generic routines (PlotIconGeneric, HitTestIconGeneric and
//	IconToRgnGeneric) to implement PlotIconID, PlotIconMethod, PlotIconSuite, HitTestIconID, etc. etc.
//	FromMethod and FromResource are defined in IconUtils.a

Handle FromMethod(long data, IconType theIcon);

Handle FromResource(long data, IconType theIcon);

Handle FromSuite(long data, IconType theIcon)
{
	IconCachePtr	theCache = *((IconCacheHandle)data);
	Handle			rval = theCache->theSuite.table[theIcon];

	//	if a suite is a cache (See Cache routines below) and we don't have any data, we call the user's
	//	call back to get some data.

	if (!rval && theCache->theSuite.type)
	{
		IconGetter theProc = theCache->userMethod;

		if (theProc)
		{
			rval = (*theProc)(typeTable[theIcon], theCache->userPtr);
			(*((IconSuiteHandle)data))->table[theIcon] = rval;
		}
	}

	return rval;
}

//	AdjustRgn checks the from and to rects and decides whether the region needs mapping,
//	offsetting or nothing.  It is defined in IconUtils.a and is called by DoRegionTransform
//	and HitTestIconGeneric.

extern pascal void AdjustRgn(RgnHandle maskRgn, Rect *from, Rect *to);

//	DoRegionTransform performs a transform using region operations.  This will be called
//	when the useRegions flag is true.

pascal void DoRegionTransform(ParamBlock *theBlock, short theTransform)
{
	RgnHandle	patRgn;
	Boolean		isASuite = (theBlock->theGroup != aCIcon);
	PenState	saveState;
	char		maskState;

	GetPenState(&saveState);

	//	If we are not a true CIcon, we need to generate a region from the icon mask.  The
	//	locking of theMask is redundant in the color case and extraneous in the case where
	//	we synthesize a mask but the complexity of determining exactly whether it needs to be done
	//	dosen't buy that much.

	if (isASuite)
	{
		maskState = HGetState(theBlock->theMask);
		HLock(theBlock->theMask);
		patRgn = NewRgn();
		MaskRgn(patRgn, theBlock);
		AdjustRgn(patRgn, &(theBlock->maskMap.bounds), &(theBlock->theRect));
	}
	else
		patRgn = (RgnHandle)theBlock->userData;			//	This is used for true CIcon to hold a region equivalent
														//	to its mask (See SetupParamBlockForCIcon)
	switch (theTransform)
	{
		//	makes an outline and fills it with a 25% gray pattern

		case ttOpen:
			FillRgn(patRgn, GetBlack());
			InsetRgn(patRgn, 1, 1);
			FillRgn(patRgn, GetLightGray());
			break;

		//	Or's over the data (already drawn) with a 25% gray pattern

		case ttOffline:
			PenPat(GetLightGray());
			PenMode(patOr);
			PaintRgn(patRgn);
			break;

		//	Bit-clears the already drawn data with a 50% pattern

		case ttDisabled:
			PenPat(GetHalfGray());
			PenMode(patBic);
			PaintRgn(patRgn);
			break;
	}

	SetPenState(&saveState);

	if (isASuite)
	{
		DisposeRgn(patRgn);
		HSetState(theBlock->theMask, maskState);
	}
}

//	The following routines are used by DoBitMapTransform to accomplish the various
//	transform effects.  They rely on the fact that the theBlock->maskMap describes
//	exactly the countours of the shape they are transforming.  This is guaranteed
//	by PrescaleMask (See below)

//	DoOutline takes the pre-scaled maskMap and OR's an outline of the icon
//	over destination.  It is used to implement the open transform.

void DoOutline(BitMapPtr maskMap, unsigned long *destination)
{
	unsigned char *source = (unsigned char *)(maskMap->baseAddr);

	unsigned long	dataMask = (unsigned long)(-1),
					lastRow,
					row,
					nextRow,
					destRow,
					checkRow,
					setMask;

	short			end = maskMap->bounds.bottom - 1,
					i;

	//	data mask is used to mask off all but the bits belonging to one row of the mask map.

	dataMask <<= (32 - maskMap->bounds.right);

	//	last row represents the row of mask data above the one we're analyzing.  It is initialized
	//	to zero because logicly, there are zero bits outside the bounds of our maskMap.

	lastRow = 0L;

	//	row is the row of mask data we are currently trying to hollow out.

	row = *((unsigned long *)source) & dataMask;
	source += maskMap->rowBytes;

	//	we march through the rows of our mask, hollowing our rows as we go.  We only go as far
	//	as the row before the bottom-most row, since we are only interested in examining rows
	//	that are samwiched between other rows with data.

	for (i = 0; i < end; i++)
	{
		//	destRow will be our hollowed out row.  It is initialized to row since we don't know if it
		//	needs hollowing.

		destRow = row;

		//	nextRow is the row below the one we are examining.  We advance our source pointer here.  We
		//	won't be advancing off the icon because we're stopping one row before the last.

		nextRow = *((unsigned long *)source) & dataMask;
		source += maskMap->rowBytes;

		//	if row has data above and below it, we examine its bits left-to-right.  If any one bit has
		//	a one-bit to left of it, to the right of it, on top of it and below it, we turn that bit
		//	off in destRow.  It works, really.

		if (destRow)
		{
			if (lastRow && nextRow)
			{
				setMask = 0x40000000;

				for (checkRow = row; checkRow; checkRow <<= 1, setMask >>= 1)
					if (((checkRow & 0xE0000000) == 0xE0000000) && (setMask & lastRow) && (setMask & nextRow))
						destRow ^= setMask;
			}

			//	here we OR the destRow with the destination data

			*destination |= destRow;
		}

		//	advance the destination pointer as well as lastRow and row.

		++destination;
		lastRow = row;
		row = nextRow;
	}

	//	nextRow now contains the data from the last row in the icon.  It either has data, in which case
	//	it gets OR'd in its entirety, or it dosen't and we do nothing.

	if (nextRow)
		*destination |= nextRow;
}

//	ApplyPattern is used to either bitClear the data in theMap with a 50% pattern (for the disabled transform)
//	or to OR the data with a 25% pattern (for offline and open).

pascal void ApplyPattern(BitMapPtr theMap, BitMapPtr theMask, Boolean bitClear)
{
	unsigned long	*mapData = (unsigned long *)(theMap->baseAddr);

	unsigned char	*maskData = (unsigned char *)(theMask->baseAddr);	// we know how wide this buffer is
	short			rowBytes = theMask->rowBytes,
					end = theMask->bounds.bottom;

	do {
		if (bitClear)
		{
			*mapData++ &= 0xAAAAAAAA;
			*mapData++ &= 0x55555555;
		}
		else
		{
			*mapData++ |= (0x88888888 & *((unsigned long *)maskData));
			maskData += rowBytes;
			*mapData++ |= (0x22222222 & *((unsigned long *)maskData));
			maskData += rowBytes;
		}

		end -= 2;
	} while (end);
}


//	DoBitMapTransform causes an icon to appear transformed (open, offline or disabled).  If onePass is true,
//	It means that we are plotting a one-bit icon and we want to combine the transform patterns with the
//	icon data off-screen and plot it all at once.  If onePass is false, it means we are plotting a color
//	icon and that the color data has already been plotted.

pascal void DoBitMapTransform(ParamBlock *theBlock, short theTransform, Boolean onePass)
{
	BitMap			transformMap;				//	BitMap used for transform data.
	unsigned long	transformBuffer[32],		//	offscreen area where we will create transform data
					*buffPtr = transformBuffer,
					*endPtr = buffPtr + 32;

	//	IsNotPreScaled is true if we did not have to scale the mask prior to this point.  We need to know this to decide
	//	whether to scale the data for one pass attempt and whether we should use CopyMask.

	Boolean			IsNotPreScaled = (MakeLong(theBlock->maskMap.bounds.bottom) == MakeLong(theBlock->dataMap.bounds.bottom));

	//	empty out the transformBuffer, since we are OR'ing data over it.

	do {
		*buffPtr++ = 0L;
	} while (buffPtr < endPtr);

	//	initialize transform BitMap

	transformMap.baseAddr = (Ptr)transformBuffer;
	transformMap.rowBytes = 4;
	transformMap.bounds = theBlock->maskMap.bounds;

	//	if we are trying to do this plot in one pass, we copy the one-bit data into the transformBuffer.  If the mask has
	//	not been prescaled, the image data data will not need scaling and we can just copy it into the buffer.  (this will
	//	always be the case in the Finder since they prescale their old-style icons into different size categories)  Otherwise,
	//	we will have to stretch the data into transform buffer;

	if (onePass && (theTransform != ttOpen))
	{
		if (IsNotPreScaled)
		{
			short			height = theBlock->dataMap.bounds.bottom;
			unsigned char	*theData = *(theBlock->theMask);

			buffPtr = transformBuffer;

			do {
				*buffPtr++ = *((unsigned long *)theData);
				theData += theBlock->rowBytes;
			} while (--height);
		}
		else	//	use srcXor because we do not know the foreColor or backColor but we know the destination is all zeros
			CopyBits((BitMapPtr)(&(theBlock->dataMap)), &transformMap, &(theBlock->dataMap.bounds), &(transformMap.bounds), srcXor, NULL);
	}

	//	OR in an outline of the mask if the transform is ttOpen

	if (theTransform == ttOpen)
		DoOutline(&(theBlock->maskMap), transformBuffer);

	//	if the transform is ttDisabled, bit clear the data with a 50% gray.  If it is ttOffline or ttOpen, OR the data with 25% gray

	if (theTransform == ttDisabled)
		ApplyPattern(&transformMap, &(theBlock->maskMap), true);
	else
		ApplyPattern(&transformMap, &(theBlock->maskMap), false);

	//	I'm putting this in a block to take advantage of register re-use

	{
		Rect	*maskRect = &(theBlock->maskMap.bounds);

		//	if we're plotting a one-bit, transformed icon or if we're plotting an opened icon, we have all the
		//	data we need in the transform map and we'll try to render that in one shot (although we may have
		//	to put down the mask separately.  See comments below).  Otherwise, we have a color transform and
		//	we assume that the basic icon data has already been rendered and that we're just going to OR over it
		//	with our transform pattern.

		if (onePass || (theTransform == ttOpen))
		{
			//	The choice of which rendering technique to use is based on three things.  If you have don't have color
			//	QuickDraw, CopyMask dosen't stretch bits so you generally can't use it.  If you're printing or plotting
			//	to an open PICT, again CopyMask won't work.  If you're mask is pre-scaled, and therefore has different
			//	bounds than your data, CopyMask won't work (a bug).  In general you want to try to use CopyMask because
			//	it goes down in one-pass and avoids any possible flashing.  If you can't use it, you BitClear the mask
			//	and OR over it with data, which works regardless of fore and backGround colors.

			if (HaveCQD() && (theBlock->onScreen) && IsNotPreScaled)
				CopyMask(&transformMap, &(theBlock->maskMap), theBlock->dstMap, maskRect, maskRect, &(theBlock->theRect));
			else
			{
				CopyBits(&(theBlock->maskMap), theBlock->dstMap, maskRect, &(theBlock->theRect), srcBic, NULL);
				CopyBits(&transformMap, theBlock->dstMap, maskRect, &(theBlock->theRect), srcOr, NULL);
			}
		}
		else
			CopyBits(&transformMap, theBlock->dstMap, maskRect, &(theBlock->theRect), srcOr, NULL);
	}
}

//	Render is called by PlotDeep, PlotShallowBlackAndWhite and PlotShallowColor.  It uses the same criterion described above
//	to decide which rendering technique to use with the main icon image data.

pascal void Render(ParamBlock *theBlock)
{
	Rect	*destRect = &(theBlock->theRect);

	if (HaveCQD() && (theBlock->onScreen) && (MakeLong(theBlock->dataMap.bounds.bottom) == MakeLong(theBlock->maskMap.bounds.bottom)))
		CopyMask((BitMapPtr)(&(theBlock->dataMap)), &(theBlock->maskMap), theBlock->dstMap,
													&(theBlock->dataMap.bounds), &(theBlock->maskMap.bounds), destRect);
	else
	{
		CopyBits(&(theBlock->maskMap), theBlock->dstMap, &(theBlock->maskMap.bounds), destRect, srcBic, NULL);
		CopyBits((BitMapPtr)(&(theBlock->dataMap)), theBlock->dstMap, &(theBlock->dataMap.bounds), destRect, srcOr, NULL);
	}
}

//	PlotDeep, PlotShallowBlackAndWhite and PlotShallowColor are all called by PlotTheIcon, once it has been decided what kind of data
//	we will be plotting.  When these routines are called, theBlock has theDevice, theClut and theData set up.  The clipRgn has also been
//	set up to whatever extent is necessary.

//	PlotDeep is used to plot color data to a deep color screen.

pascal void PlotDeep(ParamBlock		*theBlock)
{
	ColorSpec	foreGround,
				backGround;

	Handle		theData = theBlock->theData;
	short		theTransform = theBlock->theTransform;
	char		dataState;

	//	the CLUT is purgeable and rendering may move memory.

	HNoPurge((Handle)(theBlock->theClut));

	//	if we've gotten as far as calling PlotDeep we know that there's a good, disabled clut for this screen.
	//	Setting the local copy theTransform to none indicates that we should not attempt a bit cleared disabled
	//	transform.

	if (theTransform == ttDisabled)
		theTransform = ttNone;

	//	if our data is unloaded, we attempt to load it.

	if (!(*theData))
		LoadResource(theData);

	//	lock down the image data because CopyBits or CopyMask may move memory (if only rarely)

	dataState = HGetState(theData);
	HLock(theData);

	SetUpPixMap(theBlock);	//	initialize dataMap with additional fields needed for a PixMap

	//	Save the color environment

	SaveFore(&foreGround);
	SaveBack(&backGround);

	//	Ensure black and white for fore and backGround color

	ForeColor(blackColor);
	BackColor(whiteColor);

	//	theTransform is ttOpen, the transform code will do all the work.  Otherwise, we render
	//	the main color image.

	if (theTransform != ttOpen)
		Render(theBlock);

	//	If there's a transform, we OR or bit clear over the image in an appropriate way.

	if (theTransform)
	{
		if (theBlock->useRegions)
			DoRegionTransform(theBlock, theTransform);
		else
			DoBitMapTransform(theBlock, theTransform, false);
	}

	//	restore the color environment

	RestoreFore(&foreGround);
	RestoreBack(&backGround);

	HPurge((Handle)(theBlock->dataMap.pmTable));
	HSetState(theData, dataState);
}

//	PlotShallowBlackAndWhite is used to plot to a one-bit screen (Either a black and white Mac or
//	a monitor set to one-bit)

pascal void PlotShallowBlackAndWhite(ParamBlock	*theBlock)
{
	short		theTransform = theBlock->theTransform;
	ColorSpec	foreGround,
				backGround;

	theBlock->dataMap.baseAddr = *(theBlock->theMask);

	//	If the icon is selected, we invert the current fore and backGround color

	if (theBlock->selected)
	{
		if (HaveCQD())
		{
			RGBColor	RGBFore,
						RGBBack;

			SaveFore(&foreGround);
			SaveBack(&backGround);

			GetForeColor(&RGBFore);
			GetBackColor(&RGBBack);

			RGBForeColor(&RGBBack);
			RGBBackColor(&RGBFore);
		}
		else
		{
			GrafPtr curPort = myGetPort();
			*((long *)(&foreGround)) = curPort->fgColor;
			*((long *)(&backGround)) = curPort->bkColor;
			ForeColor(*((long *)(&backGround)));
			BackColor(*((long *)(&foreGround)));
		}
	}

	//	If we have a transform, use the transform code to render the icon and the transform.  Otherwise,
	//	just render the data.

	if (theTransform)
	{
		//	If we have to use regions to generate the transformed image, render the icon and modify it
		//	by the transform.  Otherwise, do the operation in one pass with DoBitMapTransform.

		if (theBlock->useRegions)
		{
			if (theTransform != ttOpen)
				Render(theBlock);

			DoRegionTransform(theBlock, theTransform);
		}
		else
			DoBitMapTransform(theBlock, theTransform, true);
	}
	else
		Render(theBlock);

	//	If we mucked with the color environment, restore it.

	if (theBlock->selected)
	{
		if (HaveCQD())
		{
			RestoreFore(&foreGround);
			RestoreBack(&backGround);
		}
		else
		{
			ForeColor(*((long *)(&foreGround)));
			BackColor(*((long *)(&backGround)));
		}
	}
}

//	PlotShallowColor is for plotting one-bit data to a deep screen

pascal void PlotShallowColor(ParamBlock	*theBlock)
{
	RGBColor		foreColor,
					backColor;

	ColorSpec		foreGround,
					backGround;

	short			screenDepth = theBlock->screenDepth,
					theTransform = theBlock->theTransform,
					theLabel;

	Boolean			isDeep = (screenDepth > 4);	//	Are we deeper than 4-bits?

	//	this check makes sure that labeling does not happen in gray-scale or in 2-bit mode

	if ((screenDepth <= 2) || (theBlock->theDevice && !((*(theBlock->theDevice))->gdFlags & 0x1)))
		theLabel = 0;
	else
		theLabel = theBlock->theLabel;

	if (theLabel)
		(void)GetLabel(theLabel, &foreColor, NULL);
	else
		GetForeColor(&foreColor);

	if (theTransform == ttOpen)
	{
		backColor.red = 52266;			//	It seems strange to hard-code this color.
		backColor.green = 52266;		//	Should it maybe be function of the label
		backColor.blue = 65322;			//	or some other factor?  HMMMM?
	}
	else
		GetBackColor(&backColor);

	theBlock->dataMap.baseAddr = *(theBlock->theMask);

	//	This check allows for a pure disabled color to be used on a screen that has one.
	//	Otherwise, a disabled icon will be Bit-Cleared with a 50% pattern.  GetGray has
	//	the side-effect of setting foreColor to the average of backColor and foreColor.
	//	It returns false if no good average could be found.  The condition should read
	//	something like "If we're on a an 8-bit monitor or we're on a 4-bit monitor and
	//	there's no label, try to find a disabled color for this icon.  If you can find
	//	one, use that color as the foreground color and treat the icon as not transformed."

	if ((theTransform == ttDisabled) && (isDeep || (!theLabel)) &&
			GetGray(theBlock->theDevice, &backColor, &foreColor))
		theTransform = ttNone;

	//	If the icon is selected, set up the color environment to make it appear darkened.

	if (theBlock->selected)
	{
		void Darken(RGBColor *aColor);

		//	on a four-bit screen, there is almost never an appropriate, darkened color available,
		//	 so we don't try.

		if (isDeep)
			Darken(&foreColor);

		Darken(&backColor);
	}

	SaveFore(&foreGround);
	SaveBack(&backGround);

	//	Note:  There is no guarantee that a good approximation of foreColor and backColor exist
	//	on the destination device.  This is a hole in the whole color protection scheme in this code.
	//	These colors should probably be validated in some way before they are used, falling back on a
	//	black & white image.

	RGBForeColor(&foreColor);
	RGBBackColor(&backColor);

	//	Use the same criterion as in PlotShallowBlackAndWhite for render the image.

	if (theTransform)
	{
		if (theBlock->useRegions)
		{
			if (theTransform != ttOpen)
				Render(theBlock);

			DoRegionTransform(theBlock, theTransform);
		}
		else
			DoBitMapTransform(theBlock, theTransform, true);
	}
	else
		Render(theBlock);

	RestoreFore(&foreGround);
	RestoreBack(&backGround);
}

//	This routine simply decides which of the two PlotShallow flavors to use.  It exists for
//	historical reasons and should be folded into PlotTheIcon in a future version.

pascal void PlotShallow(ParamBlock *theBlock)
{
	theBlock->dataMap.rowBytes = theBlock->rowBytes;
	theBlock->dataMap.pixelSize = 1;

	if (theBlock->screenDepth >= 2)
		PlotShallowColor(theBlock);
	else
		PlotShallowBlackAndWhite(theBlock);
}

//	PlotTheIcon sets up the ParamBlock for plotting the icon to the particular screen and sets up
//	the clip region as appropriate.  It is either called directly by PlotIconGeneric or indirectly
//	by way of DeviceLoop.  On entry, theBlock has been set up by SetupParamBlock, MakeBoundary has
//	been called, alignment has been done and the mask has been prescaled if necessary.  PlotTheIcon
//	gets the best available data for the screen depth and calls one of the plotting routines above
//	to render the data with its transform.

pascal void PlotTheIcon(short		screenDepth,
						short		flags,
						GDHandle	theDevice,
						ParamBlock	*theBlock)
{
	Rect				boundRect,
						sectRect;

	RgnHandle			saveClip = NULL;

	short				theTransform = theBlock->theTransform;

	//	flags will be non-zero if PlotTheIcon was called from DeviceLoop.  Flags will be zero if it
	//	was called directly from PlotIconGeneric which implies a single target device.

	if (theDevice && flags)
	{
		//	the following code generates a global rectangle which bounds the icon in its destination
		//	size.

		boundRect = theBlock->boundary;
		MapRect(&boundRect, &(theBlock->dataMap.bounds), &(theBlock->theRect));
		LocalToGlobal((Point *)&boundRect);
		LocalToGlobal((Point *)(&(boundRect.bottom)));

		//	If this global rectangle dosen't intersect the GDevice were checking, then we won't plot to it.
		//	Otherwise, we will check if it is only partially on the screen.  If it is, we will adjust the
		//	clipRgn so that only the portion of the icon that appears on the screen will be drawn.

		if (SectRect(&((*theDevice)->gdRect), &boundRect, &sectRect))
		{
			if ((!EqualRect(&sectRect, &boundRect)) && (!theBlock->dontDraw))
			{
				RgnHandle realClip = myGetPort()->clipRgn;

				saveClip = NewRgn();
				GetClip(saveClip);
				GlobalToLocal((Point *)(&sectRect));
				GlobalToLocal((Point *)(&(sectRect.bottom)));
				ClipRect(&sectRect);
				SectRgn(saveClip, realClip, realClip);
			}
		}
		else
			return;
	}

	theBlock->screenDepth = screenDepth;
	theBlock->theDevice = theDevice;

	//	The following conditional reads - "try to plot the icon as color if A) we are plotting to an 8-bit
	//	screen and the transform is not ttOpen or B) we are plotting to a 4-bit screen, the icon is not
	//	selected, and the transform is neither ttOpen nor ttDisabled.  Otherwise, treat it as one-bit"

	if (((screenDepth > 4) && (theTransform != ttOpen)) ||
		((screenDepth == 4) && (!(theBlock->selected)) && (!(theTransform & ttDisabled))))
	{
		theBlock->dataDepth = screenDepth;		//	Initialize data depth to screen depth.  GetBestData may
		GetBestData(theBlock);					//	change this value.
	}
	else
	{
		theBlock->dataDepth = 1;
		theBlock->theClut = NULL;
	}

	//	PlotTheIcon is being called from LoadIconCache, we're done loading the data and we run away.

	if (theBlock->dontDraw)
		return;

	if (theBlock->dataDepth >= 4)
		PlotDeep(theBlock);
	else
		PlotShallow(theBlock);

	if (saveClip)
	{
		SetClip(saveClip);
		DisposeRgn(saveClip);
	}
}

//	MakeBoundary creates a rectangle within the bounds of the maskMap (before it is pre-scaled) which
//	bounds the non-zero bits of the mask.  It returns false if the mask is all zeros.

Boolean MakeBoundary(ParamBlock *theBlock)
{
	unsigned char	*ptr = (unsigned char *)(theBlock->maskMap.baseAddr); 			// <57>

	unsigned short	mark,
					side,
					rowBytes = theBlock->maskMap.rowBytes,
					height = theBlock->maskMap.bounds.bottom;

	unsigned long	temp,
					checkSum,
					sizeMask = (theBlock->rowBytes < 4) ? 0xFFFF0000 : 0xFFFFFFFF;

	//	Find the first row that has data and call it the top.
	for (side = 0; side < height; ++side, ptr += rowBytes)
		if (*((unsigned long *)ptr) & sizeMask)
			break;

	//	If no data in the mask return false
	if (side == height)
		return false;

	theBlock->boundary.top = side;

	checkSum = 0;

	//	mark is set equal to the row number of the last row with data.  checkSum
	//	is set to the OR of all the rows.
	for (; side < height; ++side, ptr += rowBytes)
		if (temp = (*((unsigned long *)ptr) & sizeMask))
		{
			mark = side;
			checkSum |= temp;
		}

	//	the bottom is equal to the row number of the last row with data plus one.
	theBlock->boundary.bottom = mark + 1;

	//	checkSum is shifted left until the most significant bit is one.
	for (side = 0; !(checkSum & 0x80000000); checkSum <<= 1, ++side);

	//	the number of shifts will be the left boundary
	theBlock->boundary.left = side;

	//	checkSum is further shifted left until all the data is gone.
	for (; checkSum; checkSum <<= 1, ++side);

	//	the number of shifts plus the left offset equals the right boundary.
	theBlock->boundary.right = side;

	return true;
}

//	CreateMask generates a mask from some data.  It is generated into a buffer
//	at the end of theBlock.  CalcMask is used to generate the data.

void CreateMask(ParamBlock *theBlock, long maskByteLength)
{
	Ptr		maskBuffer = (Ptr)(theBlock->maskBuffer);
	short	rowBytes = theBlock->maskMap.rowBytes,
			height = ShortDiv(maskByteLength, rowBytes);

	//	This catches the odd case in which an icon is larger than an icon
	//	but smaller than an icon with a mask.  This makes sure we only use
	//	32 bits of height so as not to overrun our buffer.

	if (height > 32)
		height = 32;

	CalcMask((Ptr)(*(theBlock->theMask)),
			maskBuffer, rowBytes, 4,
			height, rowBytes >> 1);

	theBlock->maskMap.baseAddr = maskBuffer;
	theBlock->maskMap.rowBytes = 4;
}

//	SetupParamBlock finds the appropriate size category of icon to be dealing
//	with, gets (or synthesizes) the mask for the data, locks all necessary data
//	down, sets up the maskMap and some parameters that don't change throughout
//	the course of the plot (or make region) call.  It returns FALSE if no mask
//	(1-bit data) could be found in any size category.

short SetupParamBlock(ParamBlock *theBlock)
{
	short	size,
			rowBytes,
			desired = ChooseBestSize(&(theBlock->theRect));

	long	maskByteLength;

	Handle	theMask;

	//	Find a mask of the right size category

	for (size = desired; size <= miniIcon1; size += IconDepthSpace)
		if (theMask = (*(theBlock->userProc))(theBlock->userData, size))
			goto done;

	for (size = desired - IconDepthSpace; size >= largeIcon1; size -= IconDepthSpace)
		if (theMask = (*(theBlock->userProc))(theBlock->userData, size))
			goto done;

	return false;

	done:

	theBlock->theMask = theMask;
	theBlock->theGroup = size;
	theBlock->theData = NULL;

	//	size here means the basic dimension (height) of the icon of specific size
	//	category.  rowBytes here is a basic rowBytes of the one bit data.

	if (size == largeIcon1)
	{
		rowBytes = 4;
		size = 32;
	}
	else
	{
		rowBytes = 2;

		if (size == smallIcon1)
			size = 16;
		else
			size = 12;
	}

	//	the basic rowBytes is stored because maskMap.rowBytes might be changed to 4
	//	arbitrarily if the mask is synthesized.
	theBlock->rowBytes = rowBytes;

	//	maskMap is setup
	theBlock->maskMap.rowBytes = rowBytes;
	MakeLong(theBlock->maskMap.bounds) = 0L;
	theBlock->maskMap.bounds.bottom = size;
	theBlock->maskMap.bounds.right = rowBytes << 3;	// this insures that a mini icon is 12X16

	//	lock down the mask member and remember its state.

	if (!(*theMask))
		LoadResource(theMask);	//	theoretically its cool to let LoadResource decide whether the resource is loaded or not
								//	but LoadResource has been back-patched thrice at the time of this writing and all kinds
								//	of annoying stuff goes before the simple check of the master pointer.  sigh.

	//	The following does a size check to guess whether an icon has a mask or not.  If not,
	//	a probable mask is synthesized.

	rowBytes *= size;		//	I'm hijacking rowbytes to use as a temp
	rowBytes <<= 1;
	maskByteLength = myGetHandleSize(theMask);

	//	if there is no mask data appended to the one-bit version of this icon (i.e. its not a proper ICN#, ics#, icm#)
	//	synthesize a mask.  Otherwise, we just point the maskMap baseAddr at the mask data in the one-bit icon.

	if (maskByteLength < rowBytes)
	{
		rowBytes >>= 1;
		CreateMask(theBlock, maskByteLength);
	}
	else
		theBlock->maskMap.baseAddr = (*theMask) + (maskByteLength >> 1);

	//	This is just to allow the Finder to use some SICN's as minis without special-casing (Grrrr!)

	if ((theBlock->theGroup == miniIcon1) && (maskByteLength > rowBytes))
	{
		long center = ((theBlock->theRect.bottom + theBlock->theRect.top) >> 1);

		theBlock->theRect.top = center - 8;
		theBlock->theRect.bottom = center + 8;
		theBlock->maskMap.bounds.bottom = 16;
	}

	theBlock->dataMap.bounds = theBlock->maskMap.bounds;	//	the mask and data have the same source bounds (may change later)

	theBlock->dataMap.pixelSize = 0;		//	functions as flag saying "the data map is not yet initialized as a color grafport"

	theBlock->useRegions = false;			//	Initially assume that we will not use regions for transforms and hit testing.
											//	PrescaleMask may change this value.

	return true;
}

//	PrescaleMask is used after SetupParamBlock.  If the destination rectangle of an icon is
//	less than or equal to 32 in both dimensions, and if it is different in height or width
//	to maskMap.bounds, we do the stretching that will take place off-screen.  The result ends
//	up in the maskBuffer and maskMap is modified to point to it.  After calling PrescaleMask,
//	you are guaranteed that the maskMap describes the countours of the icon's mask as it will
//	be drawn (which is important for doing transforms and for hit-testing without doing region
//	operations).  If an icon is larger than our off-screen buffer (never under 7.0), we use
//	region operations for hit-testing and transforms.  PrescaleMask is called by HitTestIconGeneric
//	and by PlotIconGeneric (only if there is a transform).

pascal void PrescaleMask(ParamBlock *theBlock)
{
	short	width = theBlock->theRect.right - theBlock->theRect.left,
			height = theBlock->theRect.bottom - theBlock->theRect.top;

	if ((width > 32) || (height > 32))
	{
		theBlock->useRegions = true;
		return;
	}

	//	This checks if any stretching will be done… we know that maskMap.bounds has (0,0) for its
	//	upper left coordinate.

	if ((width != theBlock->maskMap.bounds.right) || (height != theBlock->maskMap.bounds.bottom))
	{
		unsigned long	tempBuffer[32];
		BitMap			tempMap;
		Rect			newBounds;

		MakeLong(newBounds) = 0;
		newBounds.right = width;
		newBounds.bottom = height;

		MakeLong(tempMap.bounds) = 0;
		MakeLong(tempMap.bounds.bottom) = 0x00200020;
		tempMap.rowBytes = 4;
		tempMap.baseAddr = (Ptr)theBlock->maskBuffer;

		//	if maskBuffer is currently being used, as in the case of a missing mask we have synthesized,
		//	we dump it temporarily to an intermediate buffer.

		if (theBlock->maskMap.baseAddr == theBlock->maskBuffer)
		{
			theBlock->maskMap.baseAddr = (Ptr)tempBuffer;
			BlockMove(theBlock->maskBuffer, tempBuffer, 32 * sizeof(unsigned long));
		}

		CopyBits(&(theBlock->maskMap), &tempMap, &(theBlock->maskMap.bounds), &newBounds, srcCopy, NULL);

		MakeLong(theBlock->maskMap.bounds.bottom) = MakeLong(newBounds.bottom);
		theBlock->maskMap.baseAddr = (Ptr)theBlock->maskBuffer;
		theBlock->maskMap.rowBytes = 4;
	}
}

//	This is the routine that all plotting goes through.  By the time we get to PlotIconGeneric, we
//	have a handler routine and some data pushed that identifies how we are going to get suite
//	members.  For example, in the case of PlotIconID, the refCon is equal to the resource ID and
//	the IconRetriever is set to a function call FromResource which calls GetResource for an icon
//	type and the given ID.  The dontDraw parameter allows for LoadIconCache to act as if it were
//	drawing, but in stead just load the data.
//
//	PlotIconGeneric is called from assembly language wrappers in IconUtils.a that Implement
//	PlotIconID, PlotIconSuite, PlotIconMethod and LoadIconCache.

#define PrintVars (*((short *)0x948))
pascal OSErr PlotIconGeneric(const Rect *theRect,
						short align,
						short transform,
						long refCon,
						IconRetriever iconProc,
						short dontDraw)
{
	GrafPort		*curPort;
	ParamBlock		theBlock;

	//	if this is a call to LoadIconCache and the input is not an iconcache,
	//	exit with a paramErr

	if (dontDraw && !((*((IconSuiteHandle)refCon))->type))
		return paramErr;

	//	save some basic information in the parameter block
	theBlock.theRect = *theRect;
	theBlock.userData = refCon;
	theBlock.userProc = iconProc;
	theBlock.theTransform = transform & transformMask;
	theBlock.selected = transform & ttSelected;
	theBlock.dontDraw = dontDraw;

	curPort = myGetPort();

	//	onScreen is true if a print page is not open and no picture is open
	theBlock.onScreen = ((PrintVars == -1) && (!(curPort->picSave)));

	theBlock.dstMap = &(curPort->portBits);

	if (!SetupParamBlock(&theBlock))
		return noMaskFoundErr;

	//	See MakeBoundary above.  If no data was found in the mask portion of the
	//	one-bit data, then clearly CopyMask would not plot anything.  So why bother
	//	doing any more work?  This is not the same case as no one-bit data available.

	if (!MakeBoundary(&theBlock))
		goto out;

	//	boundary-making and alignment have to take place before PrescaleMask, because they
	//	make certain assumptions about the mask which in general are no longer valid after the
	//	pre-scale.

	if (align)
		PerformAlignment(&theBlock, align);

	//	this line performs an off-screen stretching of the mask data if there is a
	//	a difference between the size of the destination rect and the mask size.  Thus,
	//	the maskMap field will always have a correctly scaled mask of the data which
	//	can be used for creating transform effects etc.  The one exception would be if the
	//	destination rect was so large that the pre-scaled mask would not fit in the 32 X 32
	//	buffer in the parameter block.  In this case, the useRegions flag would be set and
	//	regions would be used for transforms, hit-testing etc.

	if (theBlock.theTransform)
		PrescaleMask(&theBlock);

	//	If we are plotting to a PixMap, we lock down the mask (because color CopyBits/CopyMask
	//	can move memory), get label information and decide how to plot based on our screen setup.

	if (curPort->portBits.rowBytes < 0)
	{
		Handle theMask = theBlock.theMask;
		char maskState = HGetState(theMask);
		HLock(theMask);

		theBlock.theLabel = (transform & labelMask) >> labelShift;

		//	If we are plotting to actual video devices, then we need to see if we've got a
		//	device set by SetIconDevice or if our system has only one screen.  If so, we call
		//	PlotTheIcon once with the appropriate device.  If not, we have multiple monitors and
		//	we need to use DeviceLoop to call PlotTheIcon in the appropriate way.  If we're not
		//	plotting to a video device, but rather to an offscreen thing or a printer, we call PlotTheIcon
		//	once with a device of NULL, meaning that we're plotting off-screen.

		if (theBlock.onScreen)
		{
			GlobalIconDataPtr	theGlobals = GetGlobalPtr();
			GDHandle			theScreen;

			if (theGlobals->deviceListSize == 1)
				theScreen = theGlobals->deviceCheckList[0].theDevice;
			else
				theScreen = theGlobals->virtualScreen;

			if (theScreen)
				PlotTheIcon((*((*theScreen)->gdPMap))->pixelSize, 0, theScreen, &theBlock);
			else
				DeviceLoop(NULL, (DeviceLoopDrawingProcPtr)PlotTheIcon, (long)(&theBlock),
						dontMatchSeeds | singleDevices | allDevices);
		}
		else
			PlotTheIcon(8, 0, NULL, &theBlock);

		HSetState(theMask, maskState);
	}
	else
	{
		theBlock.theLabel = 0;					//	If we're not plotting to a CGrafPort, we don't use a label.
		PlotTheIcon(1, 0, NULL, &theBlock);
	}

out:
	return noErr;
}

//	IconToRgnGeneric is called by a bunch of assembly language wrappers in IconUtils.a to implement
//	IconIDToRgn, IconSuiteToRgn and IconMethodToRgn.  It is also called from HitTestIconGeneric if regions
//	will be used to perform hit-testing.  It takes a valid region, the rect to which the theoretical icon was drawn, the
//	alignment used and the same kind of call-back used by PlotIconGeneric above.  theRgn is changed to be
//	equivalent to the icon's mask when aligned and stretched to the destination rect.

pascal OSErr IconToRgnGeneric(RgnHandle		theRgn,
						Rect				*theRect,
						IconAlignmentType	align,
						long				refCon,
						IconRetriever		iconProc)
{
	ParamBlock		theBlock;

	//	minimal set up for call to SetupParamBlock (I know, its a little squirly that SetupParamBlock needs set up but…)

	theBlock.theRect = *theRect;
	theBlock.userData = refCon;
	theBlock.userProc = iconProc;

	//	we set up the ParamBlock, which finds the right-sized mask for the destination rect and loads it.  It also
	//	may synthesize a mask if a SICN or ICON is used for a one-bit icon.

	if (!SetupParamBlock(&theBlock))
		return noMaskFoundErr;

	//	if there is no data in the mask, we make an empty region.  Makes sense.

	if (MakeBoundary(&theBlock))
	{
		//	MaskRgn moves memory so we lock down our mask.  Note for future change:  a check should be made here
		//	to see if the mask is in maskBuffer, in which case this lock and restore is extraneous.

		Handle theMask = theBlock.theMask;
		char maskState = HGetState(theMask);
		HLock(theMask);

		//	If there's an alignment specified, we do it.

		if (align)
			PerformAlignment(&theBlock, align);

		//	make the region and	stretch it appropriately

		MaskRgn(theRgn, &theBlock);
		AdjustRgn(theRgn, &(theBlock.maskMap.bounds), &(theBlock.theRect));

		HSetState(theMask, maskState);
	}
	else
		SetEmptyRgn(theRgn);

	return noErr;
}

//	HitTestIconMaskGeneric is called from HitTestIconGeneric (See Below) if regions are not used for the
//	hit-testing.  It takes the same input as HitTestIconGeneric.  The test is performed by stretching the
//	mask data off-screen (if necessary) and comparing the point or rect to this data arithmeticly.

#define	isAPoint	0
#define isARect		1

pascal Boolean HitTestIconMaskGeneric(unsigned long pointOrRect,
										short IsRect,
										Rect *theRect,
										IconAlignmentType align,
										long refCon,
										IconRetriever iconProc)
{
	ParamBlock		theBlock;
	Rect			sectRect;
	unsigned long	testMap;
	short			theRow,
					theCount;

	//	Set up for call to SetupParamBlock (yeah yeah, I know)

	theBlock.theRect = *theRect;
	theBlock.userData = refCon;
	theBlock.userProc = iconProc;

	//	If there is no one-bit data or the mask contains nothing but zeros, we return false

	if (!SetupParamBlock(&theBlock) || !MakeBoundary(&theBlock))
		return false;

	//	If there is alignment, we do it.

	if (align)
		PerformAlignment(&theBlock, align);

	theRect = &(theBlock.theRect);	//	we hijak the parameter theRect to point to the possibly shifted destination rectangle
									//	(NOTE:  This is very silly.  The idea was to make references to theRect smaller and faster.
									//	Why not make a new variable, or not do this at all and refer to the rect as theBlock.theRect??
									//	We luck out under the 3.2 compiler, which sees that we refer to theRect a lot and allocates A3
									//	for this.  If we were out of address registers, this would make larger and slower code)

	PrescaleMask(&theBlock);		//	if the icon is to be shrunken, this routine will do the shrinking offscreen and place the
									//	result in maskMap

	//	we set up theRow, theCount and testMap to test against the pre-scaled mask data.  theRow is equal to the first row of the maskMap
	//	that we will test.  theCount is equal to the number of rows we will test.  testMap is a long-word bitMap which we will intersect
	//	with each row that we test.  If any row has intersection with testMap, then we have a hit.

	if (IsRect)
	{
		//	we do the intersection with the possibly shifted rectangle to see if alignment has moved us
		//	out of the test rect.  (if there is no alignment, this is redundant with the SectRect we did
		//	in HitTestIconGeneric below.  This probably needs restructuring in a new version.)

		if (!SectRect((Rect *)pointOrRect, theRect, &sectRect))
			return false;

		//	we set test map equal to one scan-line of the rectangle intersection of the test rect and the destination

		testMap = 0xffffffff;
		testMap <<= (32 - (sectRect.right - sectRect.left));
		testMap >>= (sectRect.left - theRect->left);

		//	theRow is the top of the intersection rectangle in destination co-ordinates.

		theRow = (sectRect.top - theRect->top);

		//	theCount is equal to the height of the intersection

		theCount = (sectRect.bottom - sectRect.top);
	}
	else
	{
		//	testMap is set to a one bit at the horizontal location of the test point in destination co-ordinates

		testMap = 1 << (31 - ((short)(pointOrRect & 0xffff) - theRect->left));

		//	theRow is set equal to the vertical location of the test point in destination co-ordinates

		theRow = ((short)(pointOrRect >> 16)) - theRect->top;

		//	there is only one row to test

		theCount = 1;
	}

	//	Here we take theRow, theCount and testMap and compare it to the mask data.

	{
		short			rowBytes = theBlock.maskMap.rowBytes;

		//	since we treat rows that are sometimes only a word in width as long words, we need
		//	to mask off the right most data that is significant.

		unsigned long	significantMask = -1L << (32 - theBlock.maskMap.bounds.right);

		unsigned char	*maskData = theBlock.maskMap.baseAddr + ShortMul(rowBytes, theRow),
						*endData = maskData + ShortMul(rowBytes, theCount);

		//	start at theRow and loop through theCount rows, testing for itersection with testMap

		do {
			if (*((unsigned long *)maskData) & significantMask & testMap)
				return true;

			maskData += rowBytes;
		} while (maskData < endData);
	}

	return false;
}

//	HitTestIconGeneric is called by assembly language wrappers in IconUtils.a to implement PtInIconID,
//	PtInIconSuite, PtInIconMethod, RectInIconID, RectInIconSuite and RectInIconMethod.  It takes a value
//	which is either a point or pointer to a rect along with a flag indicating which one it is.  It also
//	takes a rect which is where the icon being hit-test was drawn and the alignment that was used.  Finally,
//	the generic call-back and refcon used in all the generic routines (PlotIconGeneric, IconToRgnGeneric, etc.)
//	It returns true if the mask data as aligned and stretched intersects either the rect or the pixel to
//	the lower right of the point.  It returns false if there is no intersection or if any kind of error was
//	encountered.

pascal Boolean HitTestIconGeneric(unsigned long		pointOrRect,
								Rect				*theRect,
								IconAlignmentType	align,
								long				refCon,
								IconRetriever		iconProc,
								short				IsRect)
{
	Boolean			rval = false;
	RgnHandle		testRgn = NULL;
	Rect			sectRect;

	if (IsRect)
	{
		//	if the test rectangle does not even touch the destination rect, clearly there is no hit

		if (!SectRect((Rect *)pointOrRect, theRect, &sectRect))
			goto out;

		//	if the test rectangle completely engulfs the destination rectangle, clearly there is a
		//	hit and no further testing is required.

		if (EqualRect(theRect, &sectRect))
			goto trueOut;
	}
	else
		if (!PtInRect(*((Point *)(&pointOrRect)), theRect))		//	If our point is not in the ball-bark, we bail
			goto out;

	//	The only time we would use IconToRgnGeneric to test pointOrRect would be if our maskMap needed scaling
	//	and we could not do the scaling off-screen with PrescaleMask.  This is only true if the height or
	//	width of the destination rect is greater than 32 (the size of our off-screen buffer).  Otherwise, we
	//	use HitTestIconmaskGeneric which is faster and dosen't move memory as much.

	if (((theRect->right - theRect->left) > 32) || ((theRect->bottom - theRect->top) > 32))
	{
		testRgn = NewRgn();

		if (IconToRgnGeneric(testRgn, theRect, align, refCon, iconProc))
			goto out;

		if (IsRect)
		{
			if (!RectInRgn((Rect *)pointOrRect, testRgn))
				goto out;
		}
		else
			if (!PtInRgn(*((Point *)(&pointOrRect)), testRgn))
				goto out;
	}
	else if (!HitTestIconMaskGeneric(pointOrRect, IsRect, theRect, align, refCon, iconProc))
		goto out;

trueOut:

	rval = true;

out:
	if (testRgn)
		DisposeRgn(testRgn);

	return rval;
}

//	MakeIconData is called after DeviceList is initialized to set up the ExpandMem global structure used by the icon utilities.
//	The structure is returned in the variable parameter iconCluts.  (Named for historical reasons and not changed lately)
//
//	This is the layout of the structure:
//
//	#define Labels 		8
//	#define Sets		8
//
//	typedef struct
//	{
//		Strings				labelStrings;					//	array of eight string handles to label strings.  On a black and
//															//	white Mac, the rest of the structure after this point is not allocated.
//		long				seeds[Labels * Sets];			//	array of seeds for all the cluts (transformed and labeled)
//		CTabHandle			colorTables[Labels * Sets],		//	array of CTabHandles for all cluts
//							originals[2];					//	handles to the two original rom cluts (clut 4 and clut 8)
//		RGBColor			labelColors[Labels];			//	array of 8 label colors
//		unsigned char		**indexLists[2];				//	two handles to arrays of indices used in checking colors
//		GDHandle			virtualScreen;					//	a GDHandle used by SetIconDevice and GetIconDevice
//		long				deviceListSize;					//	number of GDevices at boot time
//		DeviceCheck			deviceCheckList[0];				//	a variable number of DeviceCheck structures used to cache
//															//	the results of CheckColors for all GDevices
//	} GlobalIconData;
//
//	typedef Handle Strings[Labels];
//
//	typedef struct
//	{
//		GDHandle		theDevice;				//	Handle to the GDevice that we're caching the information about
//		long			theSeed;				//	the seed of its GDPmap's pmTable at last caching
//		unsigned long	checkMask,				//	a bit-map specifying which cluts we actually tried and cached results for
//						checkResults,			//	results of caching as a bit-map
//						disabledCheckMask,		//	the other long word of the CheckMask bit-map needed when we added disabled cluts
//						disabledCheckResults;	//	the other long word of the CheckResults bit-map needed when we added disabled cluts
//	} DeviceCheck;

pascal void MakeIconData(GlobalIconDataHandle *iconCluts)
{
	long					size,
							devices;
	GlobalIconDataHandle	temp;
	short					curResFile, i;
	Handle					aHandle,
							*strPtr;
	RGBColor				*colorPtr;
	GDHandle				aGDHandle;
	DeviceCheck				*checkPtr;

	//	If we're running on a color machine, allocate the whole structure.  Otherwise, just allocate the
	//	label string list.

	if (HaveCQD())
	{
		aGDHandle = DeviceList;
		size = sizeof(GlobalIconData);

		devices = 0;

		do {
			++devices;
			size += sizeof(DeviceCheck);
			aGDHandle = (GDHandle)((*aGDHandle)->gdNextGD);
		} while (aGDHandle);
	}
	else
		size = sizeof(Strings);

	temp = (GlobalIconDataHandle)NewHandleSysClear(size);
	HLock((Handle)temp);		//	temporarily lock the structure

	*iconCluts = temp;			//	return the structure in the variable parameter

	strPtr = &((*temp)->labelStrings);	//	point to the beginning of the label string handles

	//	Get the label strings (Should they be unloaded?)

	for (i = labelColorBase; i < (labelColorBase + Labels); ++i)
		*strPtr++ = GetResource('lstr', i);

	//	If we're not on a color machine, we're done.

	if (!HaveCQD())
		goto out;

	//	virtualScreen is initially NULL, indicating that we will traverse the list of devices normally.

	(*temp)->virtualScreen = NULL;

	//	set deviceListSize with the count we generated for allocation

	(*temp)->deviceListSize = devices;

	//	Run through the list of devices, initializing our cache of device information.

	aGDHandle = DeviceList;
	checkPtr = &((*temp)->deviceCheckList[0]);
	do {
		checkPtr->theDevice = aGDHandle;
		checkPtr->theSeed = (*((*((*aGDHandle)->gdPMap))->pmTable))->ctSeed;
		checkPtr->checkMask = 0L;
		checkPtr->disabledCheckMask = 0L;
		++checkPtr;
		aGDHandle = (GDHandle)((*aGDHandle)->gdNextGD);
	} while (aGDHandle);

	//	Save the current resource file and set to the System file.

	curResFile = CurResFile();
	UseResFile(0);

	//	Get	the two lists of color indices to check at CheckColors time

	strPtr = (Handle *)(&((*temp)->indexLists));
	*strPtr++ = GetResource('indl', labelColorBase);
	*strPtr = GetResource('indl', labelColorBase + 1);

	//	Get the two standard system clut's for 8 and 4 bits from ROM

	strPtr = (Handle *)(&((*temp)->originals));
	*((short *)RomMapInsert) = mapTrue;
	*strPtr++ = GetResource('clut',8);
	*((short *)RomMapInsert) = mapTrue;
	*strPtr++ = GetResource('clut',4);

	//	Set the first label color to black.

	colorPtr = &((*temp)->labelColors);
	*((long *)(colorPtr)) = 0L;
	colorPtr->blue = 0;
	++colorPtr;

	//	Get the other seven label colors from resources

	for (i = (labelColorBase + 1); i < (labelColorBase + Labels); ++i)
		*colorPtr++ = **((RGBColor **)GetResource('rgb ', i));

	//	Restore the resource file

	UseResFile(curResFile);

	//	Get the seed for the 8-bit system clut

	strPtr = (Handle *)(&((*temp)->seeds));
	*strPtr++ = (Handle)((*((*temp)->originals[0]))->ctSeed);

	//	Generate seeds for the other 7 labeled 8-bit cluts and the
	//	selected clut and the 7 labeled selected 8-bit cluts

	for (i = 1; i < (Labels * 2); ++i)
		*strPtr++ = (Handle)GetCTSeed();

	//	Get the see for the 4-bit system clut

	*strPtr++ = (Handle)((*((*temp)->originals[1]))->ctSeed);

	//	Generate seeds for the other 7 labeled 4-bit cluts and the
	//	selected 4-bit clut and the 7 labeled selected 4-bit cluts and
	//	all the other combinations of labeled, selected and disabled cluts

	for (i = 1; i < (Labels * 6); ++i)
		*strPtr++ = (Handle)GetCTSeed();

	//	Allocate empty handles for all the cluts

	strPtr = (Handle *)(&((*temp)->colorTables));

	for (i = 0; i < (Labels * Sets); ++i)
	{
		aHandle = NewEmptyHandleSys();
		*strPtr++ = aHandle;
	}

	out:
	HUnlock((Handle)temp);
}

#define acceptableDelta (2 * 256)

//	CompareComponent takes two color components and returns 0 if the two are equal within
//	the tolerance specified by acceptableDelta, a negative number if the first is less than
//	the second by acceptableDelta and a positive number if the first is greater than the
//	second by acceptableDelta.

long CompareComponent(unsigned short comp1, unsigned short comp2)
{
	long absoluteDifference = (unsigned long)comp1 - (unsigned long)comp2;

	if ((absoluteDifference < acceptableDelta) && (absoluteDifference > -acceptableDelta))
		return 0;
	else
		return absoluteDifference;
}

//	Darken is used to reduce the brightness of an RGBColor to create selected clut's.  The intention
//	is to take a color, halve all the components and then increase the saturation of the result by
//	halving again all but the strongest component.  It is necessary to increase the saturation of
//	a color when you halve its components because dividing all the components by two causes
//	the difference in component values to become much smaller, decreasing the saturation.  The alogrithm
//	implemented below causes the HSV value of the color to be halved, while approximately maintaining
//	the color's original saturation.

void Darken(RGBColor *aColor)
{
	unsigned short	*smallest = &(aColor->red),
					*notSmallest1 = &(aColor->green),
					*notSmallest2 = &(aColor->blue),
					*temp = smallest;

	long			compareValue;

	//	halve all values

	aColor->red >>= 1;
	aColor->green >>= 1;
	aColor->blue >>= 1;

	//	Gray has zero saturation, so it can't lose anything in
	//	the darkening process.  Thus we are done in the grayish case.

	if (!CompareComponent(*smallest, *notSmallest1) && !CompareComponent(*smallest, *notSmallest2))
		return;

	//	Find the smallest color component.

	if (CompareComponent(*smallest, *notSmallest1) > 0)
	{
		temp = smallest;
		smallest = notSmallest1;
		notSmallest1 = temp;
	}

	if (CompareComponent(*smallest, *notSmallest2) > 0)
	{
		temp = smallest;
		smallest = notSmallest2;
		notSmallest2 = temp;
	}

	//	Divide its value by two again, so that the larger components are emphasized.

	(*smallest) >>= 1;

	//	take the runner-up in the race for first place and divide it by two again so that
	//	the largest component is emphasized.  If its a tie, we're done.

	compareValue = CompareComponent(*notSmallest1, *notSmallest2);

	if (compareValue < 0)
		(*notSmallest1) >>= 1;
	else if (compareValue > 0)
		(*notSmallest2) >>= 1;
}

//	Luminance takes an RGBColor and generates a value from 0 to 0xffff equal to the percieved
//	gray-scale brightness of the color.  The algorithm (which is the same one used by the QuickDraw
//	to make such conversions) is to take a weighted average of all the components, where the weights
//	are 1/8 of the blue component, 5/16 of the red component and 9/16 of the green component.  The routine
//	is used by Brighten to decide how much to brighten a color.

unsigned long Luminance(RGBColor *aColor)
{
	unsigned long	temp;

	temp = ((unsigned long)aColor->red) << 2;		//	five times the red component
	temp += aColor->red;

	temp += ((unsigned long)aColor->green) << 3;	//	plus nine times the green component
	temp += aColor->green;

	temp += ((unsigned long)aColor->blue) << 1;		//	plus two times the blue component

	return (temp >> 4);								//	divide the sum by 16 to get the weighted average
}

//	Brighten takes an RGBColor, reduces its saturation and increases its brightness (virtually the inverse
//	of Darken).  It is used by MakeClut to take a raw label color and convert it into a color which can be
//	applied to a clut and used to label 8-bit icons.  Brightened colors will not seriously affect the
//	brightness of the icon's unselected appearance, but will retain much of the character of the original color.
//
//	A little background:  A label color is applied to a clut color by multiplying each component by the
//	percentage strength of the corresponding label color component.  For example, a label color of
//	{0x7fff, 0x3fff, 0xffff} would be interpreted as 50% red, 25% green, 100% blue and each clut color
//	would have its components multiplied by the corresponding fraction.  Since the standard label colors
//	vary greatly in their effect on a clut color and since a label color can be customized by the user to
//	any value, a label color must be processed so that it does not reduce the brightness of an affected
//	icon to the point where it looks selected or unreadably dark.  An ideal label color has low HSV saturation
//	so that no color component is so attenuated that it becomes very dark.  A pure blue for example is
//	100% saturated and would reduce all components except blue to zero, making for a very dark icon.  A low
//	saturation color has a lot of gray in it, thus allowing some representation from all colors in the icon.
//	A good label color should also have a value (in the HSV sense of the word value) of 100%; that is, it should
//	have at least one component that is 100%, so that the overall value of the resultant color is not greatly reduced.
//	Finally, whatever process is used to turn a bad label color into a good label color should preserve the HSV hue
//	of the color, since that is what gives the color its "character".  A further constraint on the algorithm to make
//	any label color usable is that it be sensitive to the hue.  For example, an unprocessed blue label color of a
//	certain saturation and value will make an icon look darker than an unprocessed green label color of the same
//	saturation and value, because the human eye is many times more sensitive to green than to blue.  Thus a blue
//	label color needs to have its saturation reduced more than a green label color.
//
//	The algorithm:  The processing of any arbitrary color into a good label color takes place conceptually in three
//	steps (although it it implemented in one pass through the components).  The first step is to increase the HSV
//	value of the color to 100%, while nearly maintaining the hue.  This is done by multiplying each component by a
//	factor which takes the maximum component to 0xffff, thus "stretching" the color to its maximum value.
//	The second step multiplies the color by a fraction related to the original luminance of the label color.  This
//	makes the color closer to a gray (i.e. lowers its saturation) and incidentally lowers its over all value (brightness).
//	The third step compensates for the incidental loss of brightness by adding in a constant value, which is the complement
//	of the compression factor.  The maximum component in the resulting color is 100%, the saturation has been lowered to
//	an extent related to the luminance and the hue is maintained.

void Brighten(RGBColor *labelColor)
{
	unsigned short	*component = (unsigned short *)labelColor,
					*end = component + 3;

	unsigned long	temp,
					luminance = Luminance(labelColor);

	unsigned short	basis,
					max = labelColor->red;

	//	Find the maximum component

	if (labelColor->green > max)
		max = labelColor->green;

	if (labelColor->blue > max)
		max = labelColor->blue;

	//	This is the ultimate brightening.  Black gets mapped to white.  This implements the "feature"
	//	that specifying black as a label color means "label this with a text only.  Don't apply a color
	//	to this icon."  White will in no way affect the appearance of the icon.  This was the lowest impact
	//	change to implement this feature.  A more optimal solution in terms of performance would be to
	//	bail in PlotIconGeneric when we see a black label color.  This implies that we should store the
	//	label color in the parameter block (since we're going to all the trouble of getting it so early on
	//	in the plot process).  This involves some broad changes in labeling code which did not seem warranted
	//	at B4 time.

	if (!max)
	{
		*((long *)labelColor) = -1;
		labelColor->blue = 0xffff;

		return;
	}

	//	here we calculate the parameters basis and luminance.  luminance is related to the gray-scale brightness
	//	of the color being brightened, but is compressed to range from 0x6000 to 0xffff.  Basis is the complement
	//	of luminance and ranges from 0x9fff to 0.  Calculating the values in this way makes the process of
	//	brightening sensitive to the color being brightened.  If the luminance of the color is low, the basis
	//	will be large and their will be a greater reduction in saturation.
	//
	//	NOTE:  the extent to which luminance is compressed into a smaller range ensures that there is a minimum
	//	saturation decrease.  In the code below, luminance is multiplied by 5/8 and 3/8 of 100% (0x6000) is added
	//	on to it to achieve the compression.  If a fraction smaller the 5/8 is used, the overall saturation of all
	//	labeled colors will increase.  Multiplying luminance by 1/2 and adding 0x8000 produces labeled icons which
	//	are slightly to dark.  Some observers have noted that the current settings produce icons whose labeling
	//	does not sufficiently suggest the color chosen from the Finder labels menu.  This is an area that bears
	//	some further investigation.

	luminance >>= 1;				//	halve luminance
	luminance += (luminance >> 2);	//	add a quarter of the halved luminance to the halved luminance to get 5/8 luminance
	luminance += 0x6000;			//	add about 3/8 of the whole range to the number
	basis = 0xffff - luminance;

	//	This loop uses the calculated parameters luminance and basic to perform the brightening.

	do {
		temp = (unsigned long)(*component);

		//	stretch the color.  If the maximum component is already 100%, skip this step for all components

		if (max < 0xffff)
		{
			temp *= 0xffff;
			temp /= max;	//	max will only be zero if the label color is black in which case we will not reach this point
		}

		//	compress the color

		temp *= luminance;
		temp /= 0xffff;

		//	add in a gray component

		*component++ = basis + (unsigned short)temp;
	} while (component < end);

	//	NOTE:  for a future version, the following loop body would have been much better.  On the other hand, the current
	//	version makes the underlying model more clear and performance is not particularly critical in this routine.
	//
	//	temp = (unsigned long)(*component);
	//
	//	if (temp < 0xffff)
	//	{
	//		temp *= luminance;
	//		temp /= max;
	//		*component = basis + (unsigned short)temp;
	//	}
	//
	//	++component;
}

//	FindBlack is called by MakeClut to find the lowest luminance color in a clut.  Why?  It's for the 4-bit labeled true CIcon
//	(don't laugh).  When such an icon is labeled, the rule is that I turn the black color to the label color.  In an Icon
//	suite, the black color we want is well-defined as the 15 entry.  In a CIcon, there may be no real black, so I'll have to
//	choose something to label.  Might as well be the closest thing to black.  NOTE:  a next version of this code should check
//	the last entry in the clut to see if it is black, and if so it should return clutSize.  If the last entry is black, that's
//	the one we want.  A good way to accomplish this would be to do the search from the last entry backward, bailing if any
//	color is black (something this code should do anyway), taking the minimum luminance value otherwise.

long FindBlack(CTabHandle theClut, long clutSize)
{
	ColorSpec		*specList = (*theClut)->ctTable;
	unsigned long	luminance,
					minLuminance = 0xffff;
	long			rval = 0,
					index = 0;

	do {
		luminance = Luminance(&(specList->rgb));

		if (luminance < minLuminance)
		{
			minLuminance = luminance;
			rval = clutSize;
		}

		++index;
		++specList;
	} while (index <= clutSize);

	return rval;
}

//	MakeClut is called by CheckClut if IconGetClut passes it a purged clut or by GetBestData if IconGetClut returns a purged clut.
//	The clut is reallocated, a copy of the ROM clut of the appropriate bit-depth (4 or 8) is copied into it, it is given
//	its old seed back and is then processed according to the transform and label.  MakeClut is also called by
//	SetupParamBlockForCIcon to process a copy of the CIcon's clut to do transforms.

void MakeClut(ParamBlock *theBlock)
{
	short				labelNumber = theBlock->clutNumber;		//	0-7, which label, if any, we are using

	CTabHandle			theClut = theBlock->theClut;			//	the purged clut handle we are rebuilding

	short				sourceNumber;			//	0 or 1, specifies which of the ROM clut's we will use for a source

	unsigned char		*listPtr;				//	pointer to the list of clut indices to be protected with CheckColors

	long				blackEntry,				//	index of the color we will consider black for this clut (see FindBlack above)
						entryNo,
						clutSize;				//	number of entries in the processed clut

	ColorSpec			*specList;

	RGBColor			aColor,
						labelColor;

	//	If we're recreating a purged IconSuite clut, we will use the clut database in our ExpandMem
	//	globals for the source.  If we are processing a copy of a CIcon clut, we get the clut from
	//	our parameter block.

	if (theBlock->theGroup != aCIcon)
	{
		GlobalIconDataPtr	theGlobals = GetGlobalPtr();
		long				theSeed = theGlobals->seeds[labelNumber];
		CTabHandle			source;
		unsigned char		**theList;
		unsigned long		size;

		//	this convoluted arithmetic of getting a labelNumber and sourceNumber from the
		//	clutNumber (as opposed to just looking at the theLabel field) comes from the fact
		//	that theLabel specifies what label the user asked for, not necessarily the one
		//	we're going to give him.  For example, if the plotting takes place on a gray screen,
		//	the label is ignored.  The clutNumber reflects this fact, and theLabel does not.
		//	I could have had more fields in the ParamBlock to figure all this out, but they would
		//	have to be set up by IconGetClut.  In general, it is more important for IconGetClut to be fast,
		//	since in most cases, the clut will be already created and cached.  We now return you
		//	to your regularly scheduled programming.

		//	If the transform is ttDisabled, labelNumber will be in the range of disabled clut numbers.
		//	This line brings it back into the lower range.

		if (theBlock->theTransform == ttDisabled)
			labelNumber -= ((Labels * Sets) >> 1);

		//	If the clut is in the 4-bit range, bring it into the lower range and record the fact that
		//	we will be building a 4-bit clut.

		if (labelNumber >= (Labels << 1))
		{
			labelNumber -= (Labels << 1);
			sourceNumber = 1;
			blackEntry = 15;	//	Only used if we are processing 4-bit cluts.  For non-CIcons blackEntry is always 15
		}
		else
			sourceNumber = 0;

		labelNumber >>= 1;		//	There is a selected and non-selected version of every clut.  Dividing by two puts
								//	labelNumber into the range 0-7.

		source = theGlobals->originals[sourceNumber];	//	get a handle to the source clut
		theList = theGlobals->indexLists[sourceNumber];	//	get a handle to the list of indices to protect
		size = myGetHandleSize((Handle)source);			//	get the size to reallocate the purged clut
		clutSize = (*source)->ctSize;					//	get the number of entries in the clut

		ReallocHandle((Handle)theClut, size);			//	reallocate the clut
		HPurge((Handle)theClut);						//	make clut purgable

		BlockMove((Ptr)(*source), (Ptr)(*theClut), size);	//	copy in the source clut
		(*theClut)->ctSeed = theSeed;						//	restore its unique seed
		listPtr = *theList;									//	dereference the index list handle
	}
	else
	{
		//	a CIcon's clut is copied and processed each time it is plotted (to each screen if on multiple monitors)  Thus,
		//	the values used to determine the parameters used in processing are determined differently.

		listPtr = NULL;						//	CIcon cluts are never checked with CheckColors so listPtr is irrelevant
		clutSize = (*theClut)->ctSize;		//	theClut is already a copy so whatever we take whatever size it is

		//	For a CIcon, sourceNumber is used only to mean "Shall we process this clut in the 4-bit style (1) or the
		//	8-bit style (0)"

		if (theBlock->screenDepth < 8)
		{
			sourceNumber = 1;
			blackEntry = FindBlack(theClut, clutSize);
		}
		else
			sourceNumber = 0;
	}

	//	If there's a label get the RGBColor

	if (labelNumber)
	{
		GetLabel(labelNumber, &labelColor, NULL);

		//	If we're processing an 8-bit clut, we Brighten the color.  We don't do it in four bits because there's very little
		//	hope of achieving the subtle color effects that we can in 8-bits.  The labeling algorithm in 4-bits is different
		//	anyway and does not require the brightening process.  (See Brighten above)

		if (!sourceNumber)
			Brighten(&labelColor);
	}

	specList = (*theClut)->ctTable;		//	point to the beginning of the list of ColorSpecs in theClut

	//	Loop through all the clut entries, doing various kinds of color arithmetic to achieve the
	//	transformed clut effects.

	for (entryNo = 0; entryNo <= clutSize; ++entryNo)
	{
		//	we only do labeling and selection processing on clut entries in the index list.  That way,
		//	an icon designer could decide to use some colors for detail outside this list and not have
		//	them labeled or selected.  Thus, if we have a listPtr, we check if this entry is in the list
		//	before processing.  We process all colors otherwise (for CIcons).

		if ((!listPtr) || (entryNo == *listPtr))
		{
			//	If we have a listPtr, increment it.

			if (listPtr)
				++listPtr;

			aColor = specList->rgb;	//	make a copy of the color to process

			//	if there is a label color, apply it to this clut entry.

			if (labelNumber)
			{
				//	if the clut is 4-bit, set the black entry of the clut to the label color.
				//	Otherwise, tint the entire clut to the brightened label color by multiplying
				//	each component of each color by the fractional strength of the corresponding
				//	component of the label color.  The net effect of this process is to "filter"
				//	the clut entry through the label color.

				if (sourceNumber)
				{
					if (entryNo == blackEntry)
						aColor = labelColor;
				}
				else
				{
					if (entryNo)
					{
						//  multiplying the components together in this fashion has the effect of "filtering"
						//	the entry color through the label color.

						aColor.red = (aColor.red * (unsigned long)(labelColor.red)) >> 16;
						aColor.green = (aColor.green * (unsigned long)(labelColor.green)) >> 16;
						aColor.blue = (aColor.blue * (unsigned long)(labelColor.blue)) >> 16;
					}
					else
					{
						//	if possible, the white entry has to be "brighter" than any other color, so instead of
						//	multiplying white by the label color (which would simply produce the label color),
						//	we bias the label color toward white and use that.

						aColor.red = ((unsigned long)(aColor.red) + labelColor.red) >> 1;
						aColor.green = ((unsigned long)(aColor.green) + labelColor.green) >> 1;
						aColor.blue = ((unsigned long)(aColor.blue) + labelColor.blue) >> 1;
					}
				}
			}

			//	If this clut is for a selected icon, we apply the darkening algorithm to every affected entry

			if (theBlock->selected)
				Darken(&aColor);

			specList->rgb = aColor;		// record the processed color
		}
		else
			specList->value |= 4;	//	If this color is not in the protected list, we set the inhibit bit which causes
									//	CheckColors to ignore this entry.

		//	If the transform is ttDisabled, we average the color with white.  This has the affect of "bleaching" the color,
		//	making it seem "grayed" in the same sense that disabled text is shown.

		if (theBlock->theTransform == ttDisabled)
		{
			aColor = specList->rgb;
			aColor.red = ((unsigned long)(aColor.red) + 0xffff) >> 1;
			aColor.green = ((unsigned long)(aColor.green) + 0xffff) >> 1;
			aColor.blue = ((unsigned long)(aColor.blue) + 0xffff) >> 1;
			specList->rgb = aColor;
		}

		++specList;			//	Get the next entry in the clut
	}
}

//	GetDeviceCheck is called by IconGetClut to get the DeviceCheck structure corresponding to theDevice.
//	The DeviceCheck structure is used to cache information about which cluts have been checked against
//	a certain GDevice's available colors.
//
//	A little background:  CheckColors is a palette manager routine that takes a CTabHandle, a GDevice and some
//	tolerance values and returns a boolean, indicating whether the clut's colors can be represented on that device
//	to the specified tolerance.  To avoid calling CheckColors every time we plot, we cache the results of the
//	check in a list of structures stored at the end of our ExpandMem globals.  There is one structure for every
//	GDevice in the DeviceList.  Each structure contains a GDHandle to the corresponding device, the seed of the
//	device's color table at the time of the last check and two 64-bit bit-maps, each corresponding to the 64
//	different cluts used in plotting color icons.  One of the bit-maps indicates whether the clut has been
//	checked.  The other bit-map indicates the result (good or bad) of the check.  Before a clut is returned by
//	IconGetClut, the DeviceCheck for the given GDevice is consulted to see if the clut has already been checked
//	for that device, and if so, what the result was.  If the clut has never before been checked, CheckColors
//	is called, the DeviceCheck structure is updated to indicate that the clut has been checked and the
//	result is cached.

DeviceCheck *GetDeviceCheck(GDHandle theDevice, GlobalIconDataPtr theGlobals)
{
	DeviceCheck	*theCheck = &(theGlobals->deviceCheckList[0]);			//	prime the pointer to the beginning of deviceCheckList
	long		listSize = theGlobals->deviceListSize,
				theSeed = (*((*((*theDevice)->gdPMap))->pmTable))->ctSeed;	//	Get the device's ColorTable's seed

	//	Loop through all the DeviceCheck structures in the list, looking for the one corresponding to theDevice

	do {
		if (theCheck->theDevice == theDevice)
		{
			//	if we find a device, check if its seed has changed since the last check we did.  If it has, none of
			//	the cached check values are valid, so we invalidate the cache and update theSeed.

			if (theCheck->theSeed != theSeed)
			{
				theCheck->theSeed = theSeed;
				theCheck->checkMask = 0L;
				theCheck->disabledCheckMask = 0L;
			}

			return theCheck;
		}
		--listSize;
		++theCheck;
	} while (listSize);

	return NULL;
}

#define iconMaxTol 0x4000
#define iconMaxAveTol 0x2000

//	CheckClut takes a parameter block and calls CheckColors on theClut.  If theClut is purged,
//	we rebuild it first.  (NOTE:  I don't know why this routine returns a long.  Should be a Boolean)

long CheckClut(ParamBlock *theBlock)
{
	if (!(*(theBlock->theClut)))
	{
		//	IconGetClut assumes that our ExpandMem block won't move.  Since MakeClut can move memory, we lock
		//	down our global block before calling and unlock afterward.

		GlobalIconDataHandle	GlobalDataHandle = GetGlobalHandle();

		HLock((Handle)GlobalDataHandle);
		MakeClut(theBlock);
		HUnlock((Handle)GlobalDataHandle);
	}

	//	the tolerances used here mean that any given color can be represented if the device has a
	//	color within 25% of	it.  The average tolerance of all colors in the clut must be within
	//	12.5%.  These values are empirically derived, but otherwise arbitrary.

	return CheckColors(theBlock->theDevice, theBlock->theClut, iconMaxTol, iconMaxAveTol);
}

//	IconGetClut gets the clut that should be used to plot an icon described by theBlock using data
//	of the theDepth depth.  On entry, theBlock has theDevice, theTransform, selected and theLabel
//	fields set up.  On exit, theBlock has theClut and clutNumber set up.  IconGetClut returns false if
//	the clut for the icon plotted to theDepth does not pass the CheckColors test for the given
//	device.  (See GetDeviceCheck above).  The basic procedure for finding the clut is to generate
//	an index into the array of cluts in the ExpandMem globals from the information in theBlock and
//	theDepth.  The clut is then checked, using the cached information in the deviceCheckList (See
//	GetDeviceCheck above) or CheckClut (see above) if necessary.  If the clut passes the test we
//	return true, otherwise we return false.  (NOTE:  again this function should return a boolean
//	in stead of a long)

long IconGetClut(ParamBlock *theBlock, short theDepth)
{
	GlobalIconDataPtr	theGlobals = GetGlobalPtr();
	short				clutNumber;			//	index into clut array
	unsigned long		clutMask,
						*theCheckMap,
						*theResult;
	DeviceCheck			*theCheck;
	GDHandle			theDevice = theBlock->theDevice;

	//	if we are not on a gray-scale monitor, clutNumber is set to twice the label number (because
	//	there are two cluts for each label, selected and not selected).  No labeling is attempted
	//	on a gray-scale monitor.

	if ((theDevice && (!((*theDevice)->gdFlags & 0x1))))
		clutNumber = 0;
	else
		clutNumber = theBlock->theLabel << 1;

	//	Even numbered cluts are unselected, odd numbered cluts are selected.

	if (theBlock->selected)
		++clutNumber;

	//	The second half of the array contains transformed cluts

	if (theBlock->theTransform == ttDisabled)
		clutNumber += ((Labels * Sets) >> 1);

	//	the 4-bit cluts follow the 8-bit cluts in the transformed and untransformed groups

	if (theDepth < 8)
		clutNumber += (Labels << 1);

	//	Get the clut and record it and its index in theBlock.

	theBlock->theClut = theGlobals->colorTables[clutNumber];
	theBlock->clutNumber = clutNumber;

	//	theDevice will be NULL if we are plotting to a printer or to pict.  If so,
	//	we don't do any checks.

	if (!theDevice)
		return true;

	//	We get the DeviceCheck structure corresponding to the device we're checking.  If GetDeviceCheck
	//	returns NULL, it means that the caller is plotting to an off-screen GDevice (which is OK and will
	//	work).  We won't have any information cached for such a device so we call CheckClut immediately.

	if (theCheck = GetDeviceCheck(theDevice, theGlobals))
	{
		//	get clutNumber into the 0-31 range and set theCheckMap and theResult to either the disabled
		//	or non-disabled check bit-maps (See GetDeviceCheck above for a general description of
		//	of these bit-maps)

		if (clutNumber >= ((Labels * Sets) >> 1))
		{
			clutNumber -= ((Labels * Sets) >> 1);
			theCheckMap = &(theCheck->disabledCheckMask);
			theResult = &(theCheck->disabledCheckResults);
		}
		else
		{
			theCheckMap = &(theCheck->checkMask);
			theResult = &(theCheck->checkResults);
		}

		//	create a one-bit mask to check against the bit-maps

		clutMask = 1 << clutNumber;

		//	if the clut has never been checked against this device, update theCheckMap, call CheckClut
		//	and record the result.

		if (!((*theCheckMap) & clutMask))
		{
			(*theCheckMap) |= clutMask;

			if (CheckClut(theBlock))
				(*theResult) |= clutMask;
			else
				(*theResult) &= ~clutMask;
		}

		//	if the result of the check was that this clut cannot be well-represented on this device,
		//	return false

		if (!((*theResult) & clutMask))
			return false;
	}
	else
		return CheckClut(theBlock);

	return true;
}

//	__GetLabel is used to get information about the system labels.  It is called by the
//	trap dispatcher to implement the public GetLabel call.  labelNumber specifies which
//	label we are querying.  labelColor and labelString are buffers into which store the
//	label's RGB value and text, respectively.  Either labelColor or labelString can be
//	NULL to indicate that no information is required about the color or the text.

pascal OSErr __GetLabel(short labelNumber, RGBColor *labelColor,
						Str255 labelString)
{
	GlobalIconDataPtr	theGlobals;

	//	if labelNumber is out of range we return an error

	if ((labelNumber < 0) || (labelNumber > 7))
		return paramErr;

	//	Get a pointer to the global parameter block hanging off of ExpandMem

	theGlobals = GetGlobalPtr();

	//	If we don't have color QuickDraw, the global parameter block will not
	//	contain any label color information.  (Should this return an error?)

	if (labelColor && HaveCQD())
		*labelColor = theGlobals->labelColors[labelNumber];

	if (labelString)
	{
		Handle	aHandle = theGlobals->labelStrings[labelNumber];
		LoadResource(aHandle);

		if (ResErr)
			return ResErr;

		BlockMove((Ptr)(*aHandle), (Ptr)labelString, (long)(**aHandle) + 1);
	}

	return noErr;
}

//	InvalLabelCluts is called by SetLabel to purge all the icon-plotting cluts associated with a
//	label whose color has been changed

#define EightBitNonSelected 0
#define EightBitSelected 1
#define FourBitNonSelected (Labels * 2)
#define FourBitSelected (FourBitNonSelected + 1)
#define DisabledEightBitNonSelected ((Labels * Sets) / 2)
#define DisabledEightBitSelected (DisabledEightBitNonSelected + 1)
#define DisabledFourBitNonSelected (DisabledEightBitNonSelected + (Labels * 2))
#define DisabledFourBitSelected (DisabledFourBitNonSelected + 1)

pascal void InvalLabelCluts(short labelNumber)
{
	GlobalIconDataHandle	GlobalDataHandle = GetGlobalHandle();
	CTabHandle				*clutPtr;

	HLock((Handle)GlobalDataHandle);

	//	Get a pointer to the unselected, non-disabled, 8-bit CTabHandle for this label in the
	//	global array of icon-plotting cluts and purge it.
	clutPtr = &((*GlobalDataHandle)->colorTables[labelNumber << 1]);
	EmptyHandle((Handle)(*clutPtr + EightBitNonSelected));

	//	purge the 8-bit, selected clut
	EmptyHandle((Handle)(*(clutPtr + EightBitSelected)));

	//	purge the 4-bit non-selected clut
	EmptyHandle((Handle)(*(clutPtr + FourBitNonSelected)));

	//	purge the 4-bit selected clut
	EmptyHandle((Handle)(*(clutPtr + FourBitSelected)));

	//	purge the 8-bit, non-selected, disabled clut
	EmptyHandle((Handle)(*(clutPtr + DisabledEightBitNonSelected)));

	//	purge the 8-bit, selected, disabled clut
	EmptyHandle((Handle)(*(clutPtr + DisabledEightBitSelected)));

	//	purge the 4-bit, non-selected, disabled clut
	EmptyHandle((Handle)(*(clutPtr + DisabledFourBitNonSelected)));

	//	purge the 4-bit, selected, disabled clut
	EmptyHandle((Handle)(*(clutPtr + DisabledFourBitSelected)));

	HUnlock((Handle)GlobalDataHandle);
}

//	__SetLabel is used to get information about the system labels.  It is called through the
//	trap dispatcher to implement the GetLabel call, which is exported but private.  labelNumber
//	specifies which label we are modifying.  labelColor and labelString are buffers containing the
//	label's RGB value and text, respectively.  Either labelColor or labelString can be
//	NULL to indicate that the color or text is not to be changed.

pascal OSErr __SetLabel(short labelNumber, RGBColor *labelColor,
					Str255 labelString)
{
	GlobalIconDataPtr	theGlobals;
	Handle				aHandle;

	//	If the labelNumber is out of range, return a paramErr.  Note that the range of valid
	//	label numbers is different for __SetLabel than for __GetLabel.  Label 0 cannot be
	//	changed and is always black with text "None".

	if ((labelNumber < 1) || (labelNumber > 7))
		return paramErr;

	//	If labelColor is specified and the global data structure contains color information,
	//	set the label value.

	if (labelColor && HaveCQD())
	{
		//	Update the list of label colors in memory

		theGlobals = GetGlobalPtr();
		theGlobals->labelColors[labelNumber] = *labelColor;

		//	Update the label color in the system file

		aHandle = GetResource('rgb ', labelColorBase + labelNumber);
		*((RGBColor *)(*aHandle)) = *labelColor;

		HNoPurge(aHandle);
		ChangedResource(aHandle);
		WriteResource(aHandle);
		HPurge(aHandle);

		//	Invalidate cluts associated with labelNumber
		InvalLabelCluts(labelNumber);
	}

	//	if there is a label string to set…

	if (labelString)
	{
		unsigned char size = *labelString + 1;

		//	if the string is larger than a Str32, truncate it

		if (size > 32)
			size = 32;

		//	update the string in memory.

		theGlobals = GetGlobalPtr();
		aHandle = theGlobals->labelStrings[labelNumber];
		LoadResource(aHandle);
		BlockMove((Ptr)labelString, *aHandle, (long)size);
		**aHandle = size-1;

		//	update the system file

		HNoPurge(aHandle);
		ChangedResource(aHandle);
		WriteResource(aHandle);
		HPurge(aHandle);
	}

	UpdateResFile(0);

	return noErr;
}

//	These two routines are used as call-backs for PlotIconGeneric to implement
//	PlotIconHandle, PlotSICNHandle

Handle FromICON(long data, IconType theIcon)
{
	if (theIcon == largeIcon1)
		return ((Handle)data);
	else
		return NULL;
}

Handle FromSICN(long data, IconType theIcon)
{
	if ((theIcon == smallIcon1) || (theIcon == miniIcon1))
		return ((Handle)data);
	else
		return NULL;
}

//	These two routines are called through the trap dispatcher to implement the public
//	calls PlotIconHandle and PlotSICNHandle.  NOTE:  It was my intention to move these
//	and their respective call-backs to IconUtils.a for speed and size efficiency.

pascal OSErr __PlotIconHandle(const Rect *theRect,
						short align,
						short transform,
						Handle theIcon)
{
	return PlotIconGeneric(theRect, align, transform, (long)theIcon, FromICON, false);
}

pascal OSErr __PlotSICNHandle(const Rect *theRect,
						short align,
						short transform,
						Handle theIcon)
{
	return PlotIconGeneric(theRect, align, transform, (long)theIcon, FromSICN, false);
}

//	The QuickDraw.h definition of a CIcon includes a one-byte array at the end as a place-holder
//	for the variable-sized mask data.  This is a CIcon definition that includes only the fixed-size
//	data for a sizeof() operator.

typedef struct
{
	PixMap iconPMap;				/*the icon's pixMap*/
	BitMap iconMask;				/*the icon's mask*/
	BitMap iconBMap;				/*the icon's bitMap*/
	Handle iconData;				/*the icon's data*/
} myCIcon;

//	SetupParamBlockForCIcon initializes a parameter block for a true CIcon.  It is called by
//	__PlotCIconHandle after the theBlock has its theRect, userProc, theTransform, theLabel
//	selected and dontDraw fields initialized.  On exit theBlock is setup to be called by
//	PlotTheCIcon (See below).

void SetupParamBlockForCIcon(ParamBlock *theBlock)
{
	//	the userProc field is used to hold a pointer to the locked CIcon structure
	CIconPtr		iconPtr = (CIconPtr)(theBlock->userProc);

	short			height = iconPtr->iconMask.bounds.bottom - iconPtr->iconMask.bounds.top;
	unsigned long	maskDataSize = (iconPtr->iconMask.rowBytes * height);

	//	oneBitImageHandle is a Handle which we will set up to take the place of the one-bit
	//	member of an icon suite.  We will be setting up maskMap to point to the mask data
	//	within the CIcon itself, so we only need to make oneBitImageHandle as big as the
	//	one-bit image data.
	Handle			oneBitImageHandle = NewHandle(maskDataSize);

	//	theOneBitData points to the one-bit data (one-bit image and mask) for this CIcon.
	//	oneBitPtr is the beginning of the one-bit image data.  It is initialized to theOneBitData
	//	because it is legal to omit the one-bit image data from a CIcon, in which case we use
	//	the mask as the one-bit data.
	unsigned char	*theOneBitData = (unsigned char *)(iconPtr->iconMaskData),
					*oneBitPtr = theOneBitData;

	RgnHandle		maskRgn = NewRgn();
	GrafPtr			curPort = myGetPort();

	HLock(oneBitImageHandle);

	//	If there is one bit-data, we point oneBitPtr to it.  Otherwise, we leave it pointing
	//	to the mask data.

	if (theBlock->userData > (sizeof(myCIcon) + maskDataSize))
		oneBitPtr += maskDataSize;

	//	copy the one-bit image data to the simulated one-bit suite entry and point the theMask
	//	field in the paramBlock to point at it.

	BlockMove((Ptr)oneBitPtr, *oneBitImageHandle, maskDataSize);
	theBlock->theMask = oneBitImageHandle;

	//	copy the bounds and rowBytes information from the CIcon's iconMask into theBlock->maskMap
	//	and point the baseAddr field to the mask data within the CIcon

	BlockMove((Ptr)(&(iconPtr->iconMask)), (Ptr)(&(theBlock->maskMap)), sizeof(BitMap));
	theBlock->maskMap.baseAddr = (Ptr)theOneBitData;

	//	copy all the PixMap information from the CIcon into the PixMap within theBlock and set
	//	the theData field to point to the color data from the CIcon.

	BlockMove((Ptr)(&(iconPtr->iconPMap)), (Ptr)(&(theBlock->dataMap)), sizeof(PixMap));
	theBlock->theData = iconPtr->iconData;

	//	We use BitMapToRegion to generate the mask region used in doing transforms, etc.
	//	From this region we also derive our boundary information.  userData is overloaded for
	//	CIcons to contain the handle to the mask region, since unlike icon suites, this region
	//	is generated once as opposed to when needed.

	BitMapToRegion(maskRgn, &(theBlock->maskMap));
	theBlock->boundary = (*maskRgn)->rgnBBox;
	theBlock->userData = (long)maskRgn;

	//	Initialize miscelaneous fields for plotting

	theBlock->onScreen = ((PrintVars == -1) && (!(curPort->picSave)));
	theBlock->dstMap = &(curPort->portBits);
	theBlock->dataMap.pixelSize = 0;
	theBlock->theClut = NULL;
	theBlock->rowBytes = theBlock->maskMap.rowBytes;

	theBlock->useRegions = true;	//	we don't have any choice with CIcon's
}

//	myColorTable is a ColorTable with only the fixed-sized portion for a sizeof() operator

typedef struct
{
	long ctSeed;					/*unique identifier for table*/
	short ctFlags;					/*high bit: 0 = PixMap; 1 = device*/
	short ctSize;					/*number of entries in CTTable*/
} myColorTable;

//	CopyClut is called by GetCIconClut to create a clut that is the same as input clut but
//	padded to the depth of the pixMap data to which it belongs.  Thus a 5-entry clut for a
//	4-bit data pixMap would be padded to 16 entries.  If a clut needs padding, the entries
//	not specified are set to a ramp between white and black.  This is to simulate the fact
//	that relative colors in a CIcon are assigned to a ramp between foreGround and backGround.
//	Since we always set foreGround to black and backGround to white, this will have the desired effect.

CTabHandle CopyClut(CTabHandle theClut, short depth)
{
	long		specArraySize = sizeof(ColorSpec) << depth;		//	size of the CSpecArray in new clut
	CTabHandle	theNewClut = (CTabHandle)NewHandle(sizeof(myColorTable) + specArraySize);
	CTabPtr		oldClutPtr = *theClut,
				newClutPtr = *theNewClut;
	short		newClutEntries = (1 << depth) - 1,		//	ctSize of new clut
				oldClutEntries = oldClutPtr->ctSize;	//	ctSize of old clut

	newClutPtr->ctSeed = GetCTSeed();					//	get unique seed
	newClutPtr->ctSize = newClutEntries;				//	initialize ctSize entry
	newClutPtr->ctFlags = oldClutPtr->ctFlags;			//	copy flags from old clut to new

	//	copy common data between the two cluts (yeah, and some garbage as well)
	BlockMove((Ptr)(oldClutPtr->ctTable), (Ptr)(newClutPtr->ctTable), specArraySize);

	//	if there is no difference in CSpecArray size, we are done now.
	if (newClutEntries > oldClutEntries)
	{
		ColorSpec	tempColor,
					*theTable = newClutPtr->ctTable;

		*((long *)(&(tempColor.rgb))) = 0;			//	set the last entry to black
		tempColor.rgb.blue = 0;
		tempColor.value = newClutEntries;
		theTable[newClutEntries] = tempColor;

		++oldClutEntries;							//	we are now looking one entry beyond the
													//	last entry in the old clut

		if (newClutEntries > oldClutEntries)		//	Is there more than one extra spot to fill?
		{
			long			index;
			unsigned short	increment = 0xffff / (newClutEntries - oldClutEntries),
							value = 0;

			*((long *)(&(tempColor.rgb))) = -1;		//	set entry after the last specified to white
			tempColor.rgb.blue = 0xffff;
			tempColor.value = oldClutEntries;
			theTable[oldClutEntries] = tempColor;

			//	fill all intermediate entries with an evenly spaced ramp from white to black

			for (index = newClutEntries - 1; index > oldClutEntries; --index)
			{
				value += increment;
				tempColor.rgb.red = value;
				tempColor.rgb.green = value;
				tempColor.rgb.blue = value;
				tempColor.value = index;
				theTable[index] = tempColor;
			}
		}
	}

	return theNewClut;
}

//	This is the CIcon couterpart to IconGetClut.  It is called by PlotTheCIcon (Below) to get
//	the appropriate clut to use for plotting to deep devices.

pascal void GetCIconClut(ParamBlock *theBlock, GDHandle theDevice, short flags)
{
	CTabHandle	theClut = ((CIconPtr)(theBlock->userProc))->iconPMap.pmTable;
	short		clutDepth = ((CIconPtr)(theBlock->userProc))->iconPMap.pixelSize,
				clutNumber;

	//	copy the CIcon's clut and pad if necessary

	theBlock->theClut = CopyClut(theClut, clutDepth);

	//	clutNumber is either set to the label number or zero if	we're on a gray-scale device.
	//	MakeClut (See above) uses this value differently for CIcons than for icon suites

	if (theDevice && (!(flags & 0x1)))
		clutNumber = 0;
	else
		clutNumber = theBlock->theLabel;

	theBlock->clutNumber = clutNumber;

	//	process the copied clut appropriately for our device, transform and label

	MakeClut(theBlock);
}

//	PlotTheCIcon is called directly by __PlotCIconHandle (Below) or indirectly via DeviceLoop.
//	It is responsible for deciding whether the color icon data will be plotted or the one-bit data.
//	When PlotTheCIcon is called from DeviceLoop, we make use of DeviceLoop's setting up of visRgn to
//	clip the image appropriately.  (In PlotTheIcon, we do our own clipping and only use DeviceLoop
//	to iterate through the active devices.)

pascal void PlotTheCIcon(short		screenDepth,
						short		flags,
						GDHandle	theDevice,
						ParamBlock	*theBlock)
{
	theBlock->screenDepth = screenDepth;
	theBlock->theDevice = theDevice;

	//	if our monitor is deeper than four bits, or if our monitor is four bits and our icon is
	//	not selected or disabled, then we can plot the deep data.  Otherwise we plot the one-bit
	//	data.

	if (((screenDepth > 4) && (theBlock->theTransform != ttOpen)) ||
		((screenDepth == 4) && (!(theBlock->selected)) && (!(theBlock->theTransform & ttDisabled))))
	{
		theBlock->dataDepth = ((CIconPtr)(theBlock->userProc))->iconPMap.pixelSize;	//	set up dataDepth for PlotDeep
		GetCIconClut(theBlock, theDevice, flags);									//	get a processed copy of the clut
		PlotDeep(theBlock);															//	plot the icon
		DisposHandle((Handle)(theBlock->theClut));									//	dispose of our copy of the clut
		theBlock->theClut = NULL;
	}
	else
	{
		theBlock->dataDepth = 1;
		theBlock->theClut = NULL;
		PlotShallow(theBlock);
	}
}

//	__PlotCIconHandle is called through the trap dispatcher to implement the PlotCIconHandle call.  It allows
//	a CIcon to be plotted, using the modifiers for an icon suite.

pascal OSErr __PlotCIconHandle(const Rect *theRect,
							short align,
							short transform,
							CIconHandle theIcon)
{
	ParamBlock	theBlock;
	char		iconState;

	//	CIcon's don't exist on non-color machines.

	if (!HaveCQD())
		return paramErr;

	//	Since PlotCIconHandle is slower than PlotCIcon, we fall through to PlotCIcon when there is no alignment
	//	or transform.  For visual consistancy, we set fore and back colors to black and white, because PlotDeep
	//	(See above) does that before plotting deep data.

	if ((!align) && (!transform))
	{
		ColorSpec	foreGround,
					backGround;

		SaveFore(&foreGround);
		SaveBack(&backGround);

		ForeColor(blackColor);
		BackColor(whiteColor);

		PlotCIcon(theRect, theIcon);

		RestoreFore(&foreGround);
		RestoreBack(&backGround);
		return noErr;
	}

	//	Lock down the CIcon while we plot it.

	iconState = HGetState((Handle)theIcon);
	HLock((Handle)theIcon);

	//	set up for call to SetupParamBlockForCIcon

	theBlock.theGroup = aCIcon;
	theBlock.userData = myGetHandleSize((Handle)theIcon);	//	Going into SetupParamBlockForCIcon, userData is the size of theIcon,
															//	coming out, it is the mask region.
	theBlock.theRect = *theRect;
	theBlock.userProc = (IconRetriever)(*theIcon);
	theBlock.theTransform = transform & transformMask;
	theBlock.theLabel = (transform & labelMask) >> labelShift;
	theBlock.selected = transform & ttSelected;
	theBlock.dontDraw = false;

	SetupParamBlockForCIcon(&theBlock);

	//	do alignment, if any

	if (align)
		PerformAlignment(&theBlock, align);

	//	map the region to the size of the destination rect

	AdjustRgn((RgnHandle)(theBlock.userData),
			&(theBlock.maskMap.bounds),
			&(theBlock.theRect));

	//	If we're plotting to a BitMap as opposed to a PixMap, we call PlotTheCIcon directly with a depth of 1.
	//	Otherwise, we need to to decide more carefully how to plot.

	if (theBlock.dstMap->rowBytes < 0)
	{
		//	If we're plotting to a printer or a PICT, we call PlotTheCIcon directly with with maximum depth.
		//	Otherwise, we do more checks.

		if (theBlock.onScreen)
		{
			GlobalIconDataPtr	theGlobals = GetGlobalPtr();
			GDHandle			theScreen;

			//	If we're assuming we're plotting to a given monitor, we call PlotTheCIcon directly for that
			//	monitor.  Otherwise, we call DeviceLoop and let PlotTheCIcon decide for itself how to plot
			//	the data on monitor-by-monitor basis.

			if (theScreen = theGlobals->virtualScreen)
				PlotTheCIcon((*((*theScreen)->gdPMap))->pixelSize, 0, theScreen, &theBlock);
			else
				DeviceLoop((RgnHandle)(theBlock.userData),
						(DeviceLoopDrawingProcPtr)PlotTheCIcon,
						(long)(&theBlock), 0);
		}
		else
			PlotTheCIcon(8, 0, NULL, &theBlock);
	}
	else
		PlotTheCIcon(1, 1, NULL, &theBlock);

	//	restore the handle state of the CIconHandle

	HSetState((Handle)theIcon, iconState);

	//	dispose of the mask region

	DisposeRgn((RgnHandle)(theBlock.userData));

	//	dispose of the one-bit data handle we created in SetupParamBlockForCIcon

	DisposHandle((Handle)(theBlock.theMask));
}

#endif