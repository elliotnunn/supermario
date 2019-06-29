/*
	File:		Bass_Cache.c

	Contains:	Interfaces between Font Scaler code, QuickDraw and the Font Manager.  Retrieves
				and caches glyphs.  Initializes spline state variables.

	Written by:	Charlton E. Lui
	
	Date:		May 8, 1988
	
	Revision:	1.0

	Copyright:	© 1988-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<23>	 6/22/92	JH		#1033107 <AMW>: Changed call to GetEncodingTable to
									Font2RealScript.
		<22>	  5/3/92	JH		#1028023 <AMW>: Changing straight jump to fs_NewSfnt to the trap
									version. This enables our Korean subsidiary and potentially
									other non U.S distributors to patch the NewSfnt trap and look
									for a dongle.
		<21>	  1/3/92	JSM		Roll-in changes from Sakura: sb_CheckCache() can return NO_BITS
									from sb_RenderGlyph() (seems to only happen when printing with
									the StyleWriter), check for this and call
									CheckForMaximumDiskCacheSize() in sb_RetrieveGlyph() anyway. We
									need to understand this better. Call
									DumpFontCacheToDiskIfNotTooBig(), not DumpFontCacheToDisk(), in
									sb_DisposeCacheHandle(). Pass squashed to
									LoadFontCacheFromDisk(). If banding in TrueType, don't
									AddGlyphOffset table more than once in sb_RetrieveGlyph.
		<20>	 12/9/91	JH		#1017215,Fixing problem in sb_SetFontMemory... where if
									fs_NewSfnt fails it just bails (calls sb_Debug). The thing about
									this is that we (whose we) have released about 16 international
									fonts that actually have a specificID of Roman for their CMAP
									despite being fonts for scripts other than Roman. These fonts
									work on 7.0, but since we now do the right thing in
									sb_SetFontMemory (figure out which script the font should belong
									to and then use that script for specificID) they won't work for
									7.1 (cubee) so to fix that we try again with the Roman platform
									ID if the first call to sb_SetFontMemory fails.
		<19>	10/23/91	DTY		Flush the FOND candidate cache in sb_KillSomeCaches when
									everything else is being flushed.
		<18>	10/22/91	RB		Add support for sfnt-embedded-bitmaps
		<17>	  9/25/91	jlf		Rolled in modifications from Pacific TrueType project and rolled out
									DOUBLE_BYTE conditional code.
									
									Added support for double-byte bass caches. The glyphArray field of a
									cache is not compatible with double-byte operating systems. Use the
									routines in DoubleByteBassCache.c for international support. Added
									support for disk caching code through the LoadFontCacheFromDisk and
									CheckForMaximumDiskCacheSize routines. The spline directory of a
									TrueType font is now appended to it's global width table for partial
									font support. sb_SetMemPtrs now accepts a widthTable handle instead of
									an sfnt handle in order to initialize the spline sfntDirectory field
									of the Scaler input parameter block. Call GetResInfo to obtain the sfntID
									in FillPrinterWidths instead of hacking it to zero - it would be nice if
									the printer driver passed the sfntID value in, but, that means we would
									have to rev all printer drivers that use this call. Added partial font
									support. Added disposecachehandle routine to support disk caching (routine
									calls DumpFontCacheToDisk before calling disposezonehandle). Call
									sb_DisposeCacheHandle in sb_KillSomeCaches instead of letting the cache
									handles die through a heap reformat. Replaced calls to sb_DisposeZoneHandle
									with cache handle arguments with calls to sb_DisposeCacheHandle. Call
									KillApplicationHeapFragmentTableCaches in flush fonts to flush partial
									font fragments. Removed static qualifier in front of sb_ShrinkCacheToMin.
									Initialize familyID field of a cache in sb_InitCacheHead to the new spline
									key global fLastSwapFontFamilyID since the font manager has the obnoxious
									habit of setting the fID field of a width table and certain low memory
									globals to -1 when the width table is invalidated. This caused major
									problems with cmap lookup when using Asian fonts. Added widthTable parameter
									to SetFontMemory since it calls SetMemPtrs. Initialized specificID field
									of newsfnt input parameter block to the proper script number instead of
									hard-coding it to Roman. Always maintain the cache list in the system heap.
									sb_NewZoneHandle no longer assumes that the current heap is always the
									application heap (this previously caused major heap trashes when the
									system font was an outline font). Rolled back in code for MultiFinder
									memory allocation (this code relies on a bug fix in 6.x MultiFinder
									where the current zone is set to the MultiFinder heap before calling
									TempDisposHandle on a purged handle. 7.x systems shouldn't be affected).
									Rolled in fixes for bugs found during the development of the background
									Tabasco driver (Charlton Lui via Allen Cronce). I friggin' hate this file.
									
		<16>	  3/5/91	CL		(RB)#82681. Needed to make one more change to sb_searchforcache.
									The cache needed to be set to nil to when a cache was
									purged.
		<15>	 2/28/91	CL		(KON)(RB)#82681. Making current cache purgeable.
		<14>	 2/11/91	RB		MR, CL,CC,: Fix dropouts when squashing by trying twice to
									squash rather than once.
		<11>	 1/22/91	CL		(RB) #81198, #81199. Fixing large glyph crash (ptsize 22,000
									pt). Need to pass on error codes.
		<10>	12/11/90	MR		Improve squishing math, use leading from cache for fmLeading.
									Low mem hack. [rb]
		 <9>	11/27/90	MR		Change ByteMax() to return an unsigned char, stop caching
									algo-styles [rb]
		 <8>	11/21/90	CL		(RB)Adding in scan converter runInMinMode operations. Broke
									sb_renderglyph into smaller subroutines.
		 <7>	10/29/90	CL		(RB)if fs_OpenFonts fails we need to get rid of the workhandle
									since it is only partially set up for use. Needed to call
									sb_setmemptrs before settting the clientid.
		 <6>	10/23/90	CL		(MR)Making the preflight call work better.
		 <5>	10/23/90	CL		Somehow this file got corrupt by either BBS, projector or the
									compare file tool??? Fixed it back up!!!
		 <4>	10/22/90	CL		(BKR)Adding runInMinMode code. This will run in a minimal memory
									space when memory is very low. Added in banding of character for
									low memory situations.
		 <3>	10/20/90	MR		Change transform[2][2] from a fix_one to         frac_one
		<2+>	10/20/90	MR,rb	Change transform[2][2] from a fix_one to frac_one
		 <2>	10/11/90	CL		Set ENTRY_ERROR to skip glyphs with zero scan lines.
	    <34>	 10/9/90	CL		Added preflight routine to see if there seems to be enough
	   								room to render the glyphs.
	    <33>	 10/2/90	CL		Fixing up memory error handling.
	    <32>	 9/24/90	CL		Fixing error handling in sb_renderglyph. Needed to check
									the result after sb_renderglyphinfo call.
	    <31>	 9/10/90	CL		Added in a shrinkTest to sb_SizeTheBlock to see whether a block
									should be resized down.  This is good for sizing down the extra
									scan converter space needed for large chars and for large bitmapHands.
	    <30>	 9/10/90	CL		Now checking if system and app heap is full.  If so purge will
	   								be left on.  When both heaps are full it will switch back
									and forth to try to get the memory.  Conditional is in for
									Multi-Finder or system memory.
	    <29>	  9/5/90	CL		Getting rid of call to sb_Debug in sb_newZoneHandle since it
									really is not an error but just a possible temporary failure.
									Bug in sb_FlushFonts… needed to zero out the cacheListHand.
	   	<28>	  9/4/90	CL		Fixing up memory allocations… Bug in sb_GrowZoneHandle - Assigned
	   								new zone before DiposeZoneHandle giving this function possibly
									the wrong zone.  Reworked sb_GrowZoneHandle, sb_NewZoneHandle
									logic.  Added in sb_SetCachesNoPurge, sb_SetCachesPurge so caches
									would not unneccessarily get purged.  Took out splineKeyPtr param
									out of sb_DisposeWorkHandle to make code consistent. Took out
									extra call to sb_PrepareWorkHandle from sb_CheckCache.  Bug in
									original sb_GrowZoneHandle code now in sb_SizeTheBlock fix.  When
									a disposeZoneHandle was called the current cache was not cleared
									so it would be used and blamo the heap free list would get
									corrupt.  Bug in sb_SearchForCache… Must make the font non-purgeable
									before calling sb_PrepareWorkHandle and sb_SetNewSFNT.  Font got
									purged when it should not have.  Got rid of the now unneeded
									call to sb_initmemory.  Major bug in the sb_GrowZoneHandle that
									is now in the sb_SizeTheBlock routine.  This routine would
									dispose a handle without being able to update the variables
									from the calling routine.  Therefore you could dispose the
									handle, try to get a new handle and fail, then try to use the
									disposed handle which would of course corrupt the free list
									eventually causing major havoc in the system. Bug when calling
									sb_shrinkbitmap on a cached contour.  Since the scan lines
									could be less than the full height the shrink bitmap call would
									go to far since it used the full scan height that is cached
									in the font cache.  Bug when allocating with the new memory
									routines the bitmapHand needs to be disposed and cleared if
									it is in the quiting apps heap space.  Terrible bug in living
									in the MultiFinder temp memory and being purgeable.  There is
									no way to know the zone and we would die if we dipose the handle
									not knowing the true zone if it is in the Multi_Finder heap.  Fixed
									by living only in the system heap and application heap as we did before
									just like the init code does now.
		<27>	  9/4/90	KSM		Fix Begin/EndSystem mode bug in sb_NewZoneHandle.
		<26>	 8/28/90	MR		Change SetHandleSize calls to set the correct zone by using
									common routines (NewZoneHandle, GrowZoneHandle,
									DisposeZoneHandle).  Changed WorkHandMem to ResizeWorkHandle.
									Added calls to PrepareWorkHandle to pascal routines that use
									fsWorkHand but didn't first check it.  Moved bitmapHand and
									fsWorkHand to use same memory routines as caches.
		<25>	 8/27/90	MR		Fix uninitialized zone in SearchForCache, added call to gestalt
									to NOT use system heap if it is not growable.
		<24>	 8/22/90	MR		Change calls to FillWidthTable to pass sfnt handle and rsrc ID
		<23>	 8/21/90	MR		More consistant checking of fsWorkHand for purginess.  Still not
									completely confident I’ve caught every case.
		<22>	 8/20/90	MR		Restructure memory allocation across heaps.  New routines:
									NewCacheHandle, NewWorkHandle, sb_DisposeZoneHandle.  Conditional
									code for system_7 and init.
		<21>	 8/11/90	MR		Use splineKey as global instead of passing it as a parameter
		<20>	 8/10/90	MR		Removed conditional MIKE code
		<19>	 8/10/90	MR		Remove disk_cache code, Removed call to ReallocHandle in
									SearchForCache (cross-zone bug)
		<18>	  8/1/90	gbm		rid file of vile warnings
		<17>	 7/20/90	MR		change NewHandleSys to NewHandleSys
		<16>	 7/20/90	MR		Remove warnings, cache optimizations, found cacheHand-purge bug
		<15>	 7/18/90	MR		Add Fonts.h to include file list (to define FMOutput since it
									was removed from bass_cache.h
		<14>	 7/16/90	CL		Made Finder compatible for the Goudy init.
		<13>	 7/14/90	MR		changed reference from SQRT2 to FIXEDSQRT2
		<12>	 7/13/90	MR		Changed to use union in FSInput
		<11>	  7/2/90	RB		No change, just get comments synched
		<9+>	  7/2/90	RB		bbs screwup
		 <9>	  7/2/90	CL		Fixed runtime conditional to make sure it clips when needed.
		 <8>	  7/2/90	CL		Reworked sb_renderGlyph… Now clips the character if squishing
									was not enough. Took out encryption.
		 <7>	 5/29/90	CL		Took out cache references from the widthtables. Fixed printer
									Font Scaler reset bug.
		 <6>	  5/3/90	CL		Adding shutdown proc for bass clean up. More work on disk
									caching. Removed extra part of transform matrix not needed by
									system. In sb_FlushFonts got rid disposed of Handles that had
									nil pointers. Adding support for new encryption scheme and scan
									converter. Added new memory area
		 <5>	 4/10/90	CL		Added support for double byte codes. Worked on disk caching.
									Fixed squishing routine.
		 <4>	 3/24/90	CL		Font Scaler is returning totally bogus values causing terrible
									stack corruption. Added range checking so we can handle these
									values.
		 <3>	 3/20/90	CL		Adding Disk Cache…  Support for the tag format
		 <2>	 2/27/90	CL		Added lots of support for DrText’s optimizations.   Added a
									routine to shrink bitmaps after the Font Scaler call and save a
									possible 2x in memory usage.  Re-wrote renderglyph.
	   <2.0>	11/16/89	CEL		Fix in sb_fillprinterwidths. Bad assign from ReallocHandle.
	   <1.9>	11/15/89	CEL		Oops forgot to check memerr in sb_fillprinterwidths.
	   <1.8>	11/14/89	CEL		Added device metrics. Added device capabilities.
	   <1.7>	 9/28/89	CEL		KillSomeCaches now make sure to get rid of purged font handles
									also.
	   <1.6>	 9/25/89	CEL		Was setting numer and denom back to identity for splines since
									splines do the stretch but this would cause bad font manager
									matching. Let the v and h factors to be set back to 1. Added
									back in rounding of widths for width table when fractenable is
									off.
	   <1.5>	 9/15/89	CEL		Checking in just in case something changed that reflects the
									Bass_Cache.h file.
	   <1.4>	 8/28/89	CEL		Put in conditional for debugger state. Only drops into debugger
									if flag is set by hacker.
	   <1.3>	 8/14/89	CEL		Added in new encryption checks. Looks in more places for bitmap
									temp memory.
	   <1.2>	  8/1/89	CEL		Fixed EASE’s “C” comment brackets. On check in they were wrong!
	   <1.1>	  8/1/89	CEL		Fixed the include file buffer.h to be Bass_Cache.h
	   <1.0>	  8/1/89	CEL		Adding into Bass build for the first time…
	   <1.9>	  8/1/89	CEL		Fixed over-run array bug. Re-worked data structures.
	   <1.8>	 6/13/89	CEL		Integrated Sampo’s changes to the GlyphInputType…
	   <1.7>	 6/10/89	CEL		Moved Private.a QuickDraw Equates into proper QuickDraw private
									file (colorequ.a), got rid of QuickDraw nFiles dependencies and
									fixed up necessary files…
	   <1.6>	  6/2/89	CEL		Took out hard coded ascent, descent, leading calculations…
	   <1.5>	 5/26/89	CEL		Integrated the new Font Scaler 1.0 into Spline Fonts
	   <1.4>	 5/15/89	CEL		Bug in sb_SearchForCache - compared purged cache
	   <1.3>	  5/3/89	CEL		Comment brackets were previously messed up from override
	  <•1.2>	  5/3/89	CEL		All kinks worked out, Blasting in bass files…
	  <•1.1>	  5/2/89	CEL		Changed glue.h include file to BassGlue.h
	   <1.0>	  5/1/89	cel		Adding Bass for first time…

	To Do:
*/
/*********************/
/** System Includes **/
/*********************/
#include	"BaseIncludes.h"
#include 	<types.h>
#include	<Errors.h>
#include	<memory.h>
#include	<MFPrivate.h>
#include	<QuickDraw.h>
#include	<FixMath.h>
#include 	"ToolUtils.h"
#include 	<resources.h>
#include	<Script.h>
#include	<ShutDown.h>
#include	<Fonts.h>
#include	<GestaltEqu.h>
#include	<SysEqu.h>
#include	<ScriptPriv.h>

/*********************/
/** BASS’s Includes **/
/*********************/
#include	"FSCdefs.h"
#include	"FontScaler.h"
#include	"MacExtra.h"
#include	"FragmentTable.proto"
#include	"PartialFontExtensions.proto"
#include	"Bass_Cache.h"
#include	"SplineError.h"
#include	"FontMath.h"
#include	"sfnt.h"

#include	"DoubleByteBassCache.h"
#include	"DiskCacheExtensions.proto"

/*  prototypes for DoubleByteBassCache.c routines  */

OSErr									AddLowByteGlyphOffsetTable( uint16 glyphID, sb_SplineKeyHdl splineKey );	
uint32									GetSingleOrDoubleByteGlyphEntryOffset( uint16 glyphID, sb_CacheHeadPtr cachePointer );
extern pascal EncodingTablePointer		GetEncodingTable( short fondID, uint16 *scriptCodePointer );

//
//The trap version of this trap is deffed out unless MACINIT is true
//so just define a new constant here, FS_NEWSFNT is defined in FontScaler.h <22>
//
pascal int32 __NewSfntTrap(fs_GlyphInputType *inputPtr, fs_GlyphInfoType *outputPtr)
	= {0x303C,FS_NEWSFNT,0xA854};


#ifndef INITVERSION
#define SYSTEM_7
#endif



#ifdef DEBUG
N2S(unsigned long n, Str255 s)
{
	short i;
	for (i = 10; i > 0; i--)
	{
		char c = n % 10;
		s[i] = '0' + c;
		n /= 10;
	}
	s[0] = 10;
}

HeapCheck()
{
	Str255 s;
	s[0] = 0;
	s[++s[0]] = ';';
	s[++s[0]] = 'h';
	s[++s[0]] = 'c';
	s[++s[0]] = ';';
	s[++s[0]] = 'g';
	s[++s[0]] = ';';
	DebugStr(s);
}
#endif

extern Boolean MultiFinderExist();
extern long PurgeSpaceSysTotal();
long sb_SetUpWorkHand();

/***********************************************************************************************/								
/***********************************************************************************************/								
										/***********/
										/** DEBUG **/
										/***********/
										

static void sb_Debug()
{
	sb_SplineKeyHdl					splineKey		= SPLINE_KEY_HAND;

	if ((*splineKey)->callDebug)		Debugger();
}	/* End sb_Debug */


static char *sb_ReturnDataPointer ( sfntHand, offset, length )
Handle 		sfntHand;
long 		offset, length;
{
	int	i	= length;
	return( *sfntHand + offset );
}

static long LONG_WORD_ALIGN ( long n )
{
	return( (n + 3) & ~3 );
}

static unsigned char ByteMax ( Fixed n )
{
	register short		shortVal = FixRound(n);			/* get rid of low bits */
	
	if (shortVal > 255)		shortVal = 255;
	
	return(shortVal );
}

static char ByteRange ( Fixed n )
{
	register short		shortVal = FixRound(n);			/* get rid of low bits */
	
	if (shortVal > 127)				shortVal = 127;
	else if (shortVal < -127)		shortVal = -127;
	
	return(shortVal);
}

/***********************************************************************************************/								
									/******************/
									/**	MEMORY CALLS **/
									/******************/

/*********************************************************************************************
**									sb_DisposeZoneHandle
**
**	Allocations are made in different zones and a handle may be purged.  When this is the case
**	the memory manager does not have the ptr to figure out what zone it is coming from so it
**	can not hook the memory links up correctly.
**
**	Switch to the correct zone and dipose the handle and then return back to the previous zone.
**
**	<28-CEL> added some comments
**
*/
void sb_DisposeZoneHandle(Handle h, THz zonePtr)
{
	THz		prevZone = GetZone();
	OSErr	error = noErr;
	
	if (zonePtr) {
		SetZone(zonePtr);
		DisposHandle(h);
		SetZone(prevZone);
	} else MFTempDisposHandle(h, &error);
	
}	/* End sb_DisposeZoneHandle */



/*********************************************************************************************
**									sb_SetCachesNoPurge			<28-CEL>
**
**	Run through the caches which have not been purged and make them non-purgeable
**
**	WHEN TO CALL: System heap allocation
**	WHY: System heap only guestimates when to grow which can cause purgeability or caches.
**
*/
void sb_SetCachesNoPurge()
{
	sb_SplineKeyHdl					splineKey		= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr		splineKeyPtr	= *splineKey;

	if (splineKeyPtr->cacheListHandle) {
		register short 				fontCount	= splineKeyPtr->fontCount;
		register CacheReference* 	cacheRef	= *splineKeyPtr->cacheListHandle;

		for (--fontCount; fontCount >= 0; --fontCount) {
			if (cacheRef->cache && *cacheRef->cache)		/* have a ptr??? */
				HNoPurge(cacheRef->cache);
			cacheRef++;
		}
	}

}	/* End sb_SetCachesNoPurge */



/*********************************************************************************************
**									sb_SetCachesPurge			<28-CEL>
**
**	Run through the caches which have not been purged and make them purgeable
**
**	WHEN TO CALL: After system heap memory allocations have been made,
**				  if sb_SetCachesNoPurge was called.
**
*/
void sb_SetCachesPurge()
{
	sb_SplineKeyHdl					splineKey		= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr		splineKeyPtr	= *splineKey;
	register Handle					curCacheHand	= splineKeyPtr->cacheHand;

	if (splineKeyPtr->cacheListHandle) {
		register short 				fontCount	= splineKeyPtr->fontCount;
		register CacheReference* 	cacheRef	= *splineKeyPtr->cacheListHandle;

		for (--fontCount; fontCount >= 0; --fontCount) {
			if (cacheRef->cache && *cacheRef->cache)		/* have a ptr??? */
				HPurge(cacheRef->cache);
			cacheRef++;
		}
	}
	if (curCacheHand && *curCacheHand)		HNoPurge(curCacheHand);	/* Keep current cache non-purgeable */

}	/* End sb_SetCachesPurge */

#ifndef USE_SYSTEM_HEAP
/*********************************************************************************************
**									sb_MultiFinderHandle			<30-CEL>
**
**	Call the Mutlifinder memory manager.
**	Since this is system type memory make sure to turn on BeginSystem mode to make
**	sure Multifinder does not release the memory when the application quits!
**
*/
Handle sb_MultiFinderHandle(long blockSize)
{
	register Handle		h;
	OSErr				resultCode;

	BeginSystemMode();
	h			= MFTempNewHandle((Size)blockSize, &resultCode);
	EndSystemMode();

	if (resultCode)		return nil;
	else				return h;

}	/* End sb_MultiFinderHandle */
#endif

/*********************************************************************************************
**									sb_GetSysHandle			<30-CEL>
**
**
*/
Handle sb_GetSysHandle(long size)
{
	register Handle 			h	= nil;

#ifdef USE_SYSTEM_HEAP
	h 		= NewHandleSys(size);			/* system heap mem */
#else
	h = MultiFinderExist( ) ? sb_MultiFinderHandle( size ) : NewHandleSys( size );
#endif
	
	return h;
}	/* End sb_GetSysHandle */

/*********************************************************************************************
**									sb_GetSysHandleZone			<30-CEL>
**
**
*/
Handle sb_GetSysHandleZone(long size, THz* zone)
{
	register Handle 			h				= nil;

#ifdef USE_SYSTEM_HEAP
	h 		= NewHandleSys(size);			/* system heap mem */
	*zone 	= SystemZone();					/* <30-CEL> */
#else
	if ( MultiFinderExist( ) ) {
	
		h = sb_MultiFinderHandle( size );
		*zone = 0;
	
	} else {
	
		h = NewHandleSys(size);
		*zone = SystemZone( );

	}
	
#endif
	
	return h;
}	/* End sb_GetSysHandleZone */



/*********************************************************************************************
**									sb_GetAppHandleZone			<30-CEL>
**
**
*/
Handle sb_GetAppHandleZone(long size, THz* zone)
{
	register Handle				h;
	register THz				curZone 		= GetZone();
	register GrowZoneProcPtr	growZonePtr		= curZone->gzProc;
	
	curZone->gzProc		= nil;
	h 		= NewHandle(size);				/* <30-CEL> Try the App Heap */
	curZone->gzProc		= growZonePtr;		/* restore the grow zone proc ptr */
	*zone				= curZone;
	
	return h;
	
}	/* End sb_GetAppHandleZone */



/*********************************************************************************************
**									sb_GetAppHandle			<30-CEL>
**
**
*/
Handle sb_GetAppHandle(long size)
{
	register Handle				h;
	register THz				curZone 		= GetZone();
	register GrowZoneProcPtr	growZonePtr		= curZone->gzProc;
	
	curZone->gzProc		= nil;
	h 		= NewHandle(size);				/* <30-CEL> Try the App Heap */
	curZone->gzProc		= growZonePtr;		/* restore the grow zone proc ptr */
	
	return h;
	
}	/* End sb_GetAppHandle */


/*********************************************************************************************
**									sb_GrowAppHandle			<30-CEL>
**
**
*/
long sb_GrowAppHandle(Handle h, long size)
{
	THz					curZone 		= GetZone();
	GrowZoneProcPtr		growZonePtr		= curZone->gzProc;
	
	curZone->gzProc		= nil;
	SetHandleSize(h, size);							/* <30-CEL> took out set and restore zone for sethandlesize */
	curZone->gzProc		= growZonePtr;				/* restore the grow zone proc ptr */
	return MemError();								/* <30-CEL> */
		
}	/* End sb_GrowAppHandle */



/*********************************************************************************************								
**									sb_NewZoneHandle
**
**	BEHAVIOR:										<28-CEL> some logic
**		7.0 - 
**			•	Make caches non-purgeable
**			•	Try for Multi-Finder temp memory.
**			•	Failure: Try for Application memory.
**			•	Failure: Make caches purgeable and try again from the top.
**					(Better)Selectively choose caches for purgeability.
**		6.0 -
**			Multi-Finder
**				•	Make caches non-purgeable
**				•	Try for System Heap
**				•	Failure: Try for Application memory.
**				•	Failure: Make caches purgeable and try again from the top.
**						(Better)Selectively choose caches for purgeability.
**			Finder
**				•	Make caches non-purgeable
**				•	Try for Application memory.
**				•	Failure: Make caches purgeable and try again from the top.
**						(Better)Selectively choose caches for purgeability.
**		No luck -
**			•	Return nil
**
**	Under 7.0 NCH looks in temp mem first, bracketing the call with sysmode
**		What is this bracketing magic???
**
**	Allocate in non app heap with no purge 
**	Allocate in App heap with purge
**	Allocate in Multi heap with purge
**
**	New stuff:
**		• Allocate in non-app heap (sys or multi) until full with no purge
**		• Allocate in app heap until full with no purge
**		• Switch back and forth with purge on
*/
Handle sb_NewZoneHandle(sb_SplineKeyHdl splineKey, long size, THz* zone)
{
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;		/* <28-CEL> */
	register Handle 			h				= nil;
	THz							curZone 		= GetZone();
	THz							savedZone;
	Boolean						sameAppZone;

	savedZone = curZone;

	if ( splineKeyPtr->curAppZone == SystemZone( ) )
		splineKeyPtr->curAppZone = nil;
	
	if ( curZone == SystemZone( ) ) {
	
		curZone = splineKeyPtr->curAppZone;
		if ( curZone == nil )
			curZone = ApplicZone( );
	
	}
	
	SetZone( curZone );
	sameAppZone = (splineKeyPtr->curAppZone == curZone);
	if (!sameAppZone){
		splineKeyPtr->curAppZone 	= curZone;			/* log current zone */
		splineKeyPtr->appFull 		= false;			/* Reset for different zone */
	}
	if (!splineKeyPtr->appFull) {						/* <30-CEL> */
		sb_SetCachesNoPurge();
		if (splineKeyPtr->mfExists && !splineKeyPtr->sysFull) {
			h = sb_GetSysHandleZone(size, zone);
			if (!h) {
				splineKeyPtr	= *splineKey;
				splineKeyPtr->sysFull = true;
			}
		}
		if (!h)	{
			h = sb_GetAppHandleZone(size, zone);
			if (!h) {
				splineKeyPtr	= *splineKey;
				splineKeyPtr->appFull 	= true;
			}
		}
		sb_SetCachesPurge();
	}
	if (!h) {							/* Full with no purge on so try without it */
		if (splineKeyPtr->useHeap == useApp) {
			h = sb_GetAppHandleZone(size, zone);
			if (!h)		h = sb_GetSysHandleZone(size, zone);
			splineKeyPtr			= *splineKey;
			splineKeyPtr->useHeap	= useSys;
		} else {
			h = sb_GetSysHandleZone(size, zone);
			if (!h)		h = sb_GetAppHandleZone(size, zone);
			splineKeyPtr			= *splineKey;
			splineKeyPtr->useHeap	= useApp;
		}
	}	

	SetZone( savedZone );
	return h;
	
}	/* End sb_NewZoneHandle */


/*********************************************************************************************								
**									sb_NewSharedHandle		<30-CEL> new routine
**
**	This data will be shared across heaps in Multi-Finder mode so it must be in the system heap.
**	Otherwise under finder just put it in the app heap since it will not be multiply shared.
**		Finder Mode:		Application Heap
**		Multi Finder:		System Heap (Shared)
**
*/
Handle sb_NewSharedHandle(sb_SplineKeyHdl splineKey, long cacheSize)
{
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;		/* <28-CEL> */
	register Handle 			h				= nil;

	if (splineKeyPtr->mfExists) {
		if (!splineKeyPtr->appFull && !splineKeyPtr->sysFull) {
			sb_SetCachesNoPurge();							/* <30-CEL> */
			h = sb_GetSysHandle(cacheSize);					/* system heap mem */
			sb_SetCachesPurge();
			if (!h) {
				splineKeyPtr			= *splineKey;
				splineKeyPtr->sysFull 	= true;
			}
		}
		
		if (!h)		h = sb_GetSysHandle(cacheSize);		/* system heap mem */
	} else {
		THz							curZone 		= GetZone();
		Boolean						sameAppZone		= (splineKeyPtr->curAppZone == curZone);

		if (!sameAppZone){
			splineKeyPtr->curAppZone 	= curZone;			/* log current zone */
			splineKeyPtr->appFull 		= false;			/* Reset for different zone */
		}
		if (!splineKeyPtr->appFull) {
			sb_SetCachesNoPurge();							/* <30-CEL> */
			h = sb_GetAppHandle(cacheSize);					/* <28-CEL> Try the App Heap */
			sb_SetCachesPurge();
			if (!h) {
				splineKeyPtr			= *splineKey;
				splineKeyPtr->appFull 	= true;
			}
		}
		
		if (!h)		h = sb_GetAppHandle(cacheSize);			/* app heap mem */
	}	
	
	return h;
}	/* End sb_NewSharedHandle */



/*********************************************************************************************								
**									sb_GrowHandle		<28-CEL> new routine
**
**	Grow the current shared data block handle…
**	If this is in the App heap make sure there is enough space before requesting
**	the memory.  Otherwise this could cause the Application purge proc to be called
**	and the App might exit with an out of memory.
**
**	If in the Multi Heap try with no purgeability.
**	Fails:  try with purgeability
**
**	App heap always try with purgeability.
**
*/
long sb_GrowHandle(sb_SplineKeyHdl splineKey, Handle h, long size)
{
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;		/* <28-CEL> */
	register long				result 			= NO_ERR;
	THz							curZone 		= GetZone();
	Boolean						sameAppZone		= (splineKeyPtr->curAppZone == curZone);

	if (!sameAppZone) {
		splineKeyPtr->curAppZone 	= curZone;			/* log current zone */
		splineKeyPtr->appFull 		= false;			/* Reset for different zone */
	}
	
	if (HandleZone(h) != curZone) {						/* <30-CEL> sys heap */
		if (!splineKeyPtr->appFull) {					/* <30-CEL> */
			sb_SetCachesNoPurge();						/* <30-CEL> */
			SetHandleSize(h, size);						/* <30-CEL> took out set and restore zone for sethandlesize */
			result = MemError();						/* <30-CEL> */
			sb_SetCachesPurge();						/* <30-CEL> */
			if (result) {
				SetHandleSize(h, size);					/* <30-CEL> took out set and restore zone for sethandlesize */
				result = MemError();					/* <30-CEL> */
			}
		} else {
			SetHandleSize(h, size);						/* <30-CEL> took out set and restore zone for sethandlesize */
			result = MemError();						/* <30-CEL> */
		}
	} else {											/* <30-CEL> app heap */
		if (!splineKeyPtr->appFull) {
			sb_SetCachesNoPurge();						/* <30-CEL> */
			result = sb_GrowAppHandle(h, size);
			sb_SetCachesPurge();
			if (result)	{
				result 					= sb_GrowAppHandle(h, size);
				splineKeyPtr			= *splineKey;
				splineKeyPtr->appFull 	= true;
			}
		} else result = sb_GrowAppHandle(h, size);
	}
	
	return result;
}	/* End sb_GrowHandle */





/*********************************************************************************************								
**									sb_SizeTheBlock			<28-CEL> some logic
**
**	Block exist: Resize handle… if this fails, get new one, copy over data if needed, and dipose old one. 
**	Empty or purged Handle: Dipose if handle is purged but exists and then get brand new one.
**
**	Upfront we will try to keep existing caches.  If memory allocations fail try again with purgeable
**	caches.
**
**	If it is the App heap, make sure there is enough contiguous space to make the request.
**	If this is not done, it could cause the applicaton purge proc to be called and it could
**	cause a out of memory error.
**
**	MERGE comments from sb_GrowZoneHandle…
**	Block exist: Resize handle… if this fails, get new one, copy over data if needed, and dipose old one. 
**	Empty or purged Handle: Dispose if handle is purged but exists and then get brand new one.
**
**	Upfront we will try to keep existing caches.  If memory allocations fail try again with purgeable
**	caches.
**
**	CachePlace:
**		• When a cacheHandle is moved via a newHandle of somekind the cacheHandle inside the
**		  cacheList must be updated.  Use CachePlace to find the correct cache info inside the
**		  the cache list.
**		• if cachePlace == -1, ignore the update.  This means it is not a cacheChunk.
**
*/
Handle sb_SizeTheBlock(sb_SplineKeyHdl splineKey, Handle inputH, THz* inputZone, long inputSize, 
					   ZoneHandleOptions options, short cachePlace, ShrinkOptions shrinkTest)
{
	register Handle		h			= inputH;			/* <28-CEL> */
	register THz		*zone		= inputZone;		/* <28-CEL> */
	register long		size		= inputSize;		/* <28-CEL> */
	Boolean				room		= true;

	if (h) {											/* <28-CEL> */
		if (*h) {										/* <28-CEL> */
			if (shrinkTest == noShrink && size <= GetHandleSize(h))		return h;	/* <30-CEL> Right size… leave it alone */
			if (!sb_GrowHandle(splineKey, h, size))		return h;	/* <30-CEL> */

			/* THIS SHOULD BE A RARE CASE!!! */
			/* If we get this far, growing the existing handle failed, so allocate a new one */
			if (options & kKeepContents) {
				register long 	origSize = GetHandleSize(h);	/* <28-CEL> */
				THz 			newZ;
				Handle 			newH;
				short			handState	= HGetState(h);
				
				if (newH = sb_NewZoneHandle(splineKey, size, &newZ)) {
				
					BlockMove( *h, *newH, origSize );
					sb_DisposeZoneHandle( h, *zone );			/* <28-CEL> assigned zone before disposing caused problems */

					if (cachePlace != -1) {		/* <28-CEL> Need to update cache info in cacheList */
						register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;		/* <28-CEL> */
						register CacheReference		*cacheR			= *splineKeyPtr->cacheListHandle + splineKeyPtr->cachePlace; /* <28-CEL> */

						cacheR->cache		= newH;				/* <28-CEL> */
						cacheR->zone		= newZ;				/* <28-CEL> */
					}
					*zone 	= newZ;								/* <28-CEL> */
					HSetState(newH,handState);					/* <31-CEL> set to original state */
					return	 newH;
				} else		return nil;
			}
		} else {		/* ERROR!!!: This routine should never be called with a purged handle!!! */
			sb_Debug();			/* <27-CEL> */
			return nil;			/* <27-CEL> */
		}
	}
	return(sb_NewZoneHandle( splineKey, size, zone ));
	
}	/* End sb_SizeTheBlock */



/*********************************************************************************************								
**									sb_ModCacheList
**
**	sb_ModCacheList increases the number of nil cache_head entries in the cacheListHandle.
**		either it grows cacheListHandle by GROW_CACHE_NUM
**		or if cacheListHandle is nil, it allocates INITIAL_CACHES entries
**
**	Finder Mode:
**		CacheListHand lives in the App Heap
**	Multi Mode:
**		CacheListHand lives in the Sys Heap
**
*/
static long sb_ModCacheList(sb_SplineKeyHdl splineKey)
{
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;
	register short				newFontCount;
	register long				cacheSize;
	register short				initCount;
	long						result			= NO_ERR;
	register Handle				tempHand		= nil;
	
	if (splineKeyPtr->cacheListHandle) {
		register Handle		h			= splineKeyPtr->cacheListHandle;
		THz					zone		= HandleZone(h);
		
		newFontCount	= splineKeyPtr->fontCount + GROW_CACHE_NUM;
		initCount		= GROW_CACHE_NUM;
		cacheSize		= GetHandleSize(h) + GROW_CACHE_NUM * sizeof(CacheReference);

		result = sb_GrowHandle(splineKey, h, cacheSize);					/* <30-CEL> */
 	} else {
		splineKeyPtr->fontCount		= 0;
		newFontCount = initCount	= INITIAL_CACHES;
		cacheSize					= INITIAL_CACHES * sizeof(CacheReference);
		
		tempHand = NewHandleSys( cacheSize );
		if (tempHand) {														/* <30-CEL> */
			splineKeyPtr	= *splineKey;
			splineKeyPtr->cacheListHandle = (CacheReference**)tempHand;		/* <30-CEL> */
		} else	
			result = NEWCACHELIST_HANDLE_ERR;								/* <30-CEL> */
	}	/* End If */

	if (!result) {
		register CacheReference*		cacheR; /* <28-CEL> */
		register Handle 				zero = 0;
		
		splineKeyPtr	= *splineKey;
		cacheR = *splineKeyPtr->cacheListHandle + splineKeyPtr->fontCount;
		
		for (--initCount; initCount >= 0; --initCount) {	/* begin new caches as nil */
			cacheR->cache = zero;
			cacheR++;
		}
		splineKeyPtr->fontCount	= newFontCount;
	}

	return result;											/* <27-CEL */
	
}	/* End sb_ModCacheList */



/*******************************/
/** WRAPPERS For memory calls **/
/*******************************/

void sb_DisposeCacheHandle( Handle cacheHandle, THz cacheZone )

{

	DumpFontCacheToDiskIfNotTooBig( (sb_CacheHeadHdl) cacheHandle );
	sb_DisposeZoneHandle( cacheHandle, cacheZone );

}


/*********************************************************************************************								
**									sb_DisposeWorkHandle				<28-CEL> a little comment
**
**	If a handle is purged and the zone from which the handle came from is not the current zone,
**	the memory manager does not know where to link the free handle.  This is bad news unless
**	we switch to the correct zone and then dispose of the handle!
**
*/
void sb_DisposeWorkHandle()		/* <28-CEL> took out splineKeyPtr param to make code consistent */
{
	sb_SplineKeyHdl				splineKey 		= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;

	if (splineKeyPtr->fsWorkHand) {				/* <28-CEL> */
		sb_DisposeZoneHandle(splineKeyPtr->fsWorkHand, splineKeyPtr->fsWorkZone);
		splineKeyPtr->fsWorkHand = 0;
		splineKeyPtr->fsWorkZone = 0;			/* <28-CEL> */
	}
}

/*********************************************************************************************								
**									sb_SizeWorkHandle				<28-CEL>
**
**	Size the Font Scaler work space.  If it does not exist it will
**	create one.
**
**	ZoneHandleOptions: Sets state if the memory needs to be preserved.
**	letShrink: Set if memory should be shrunk.
**
*/
long sb_SizeWorkHandle( long size, ZoneHandleOptions zOptions, ShrinkOptions shrinkTest )
{
	sb_SplineKeyHdl				splineKey 		= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;					/* <28-CEL> */
	register Handle 			h				= splineKeyPtr->fsWorkHand; 	/* <28-CEL> */
	THz 						z; 												/* <28-CEL> */
	register long				result			= NO_ERR;		/* <28-CEL> */

	if (h && !*h) 	sb_DisposeWorkHandle();						/* <31-CEL> */
		
	z 				= splineKeyPtr->fsWorkZone;					/* <28-CEL> */
	h 				= splineKeyPtr->fsWorkHand;					/* <28-CEL> */
	
	if (h = sb_SizeTheBlock( splineKey, h, &z, size, zOptions, -1, shrinkTest )) {
		splineKeyPtr				= *splineKey;				/* <28-CEL> */
		splineKeyPtr->fsWorkHand	= h;						/* <28-CEL> */
		splineKeyPtr->fsWorkZone	= z;						/* <28-CEL> */
	} else {
		result = MEM_ERR;
	}
	
	return result;
}	/* End sb_SizeWorkHandle */




/*********************************************************************************************								
**									sb_GetBitMapSpace		<28-CEL> rewrote
**
**	This is a temporary bitmap to blit in when the glyph is not cached.
**
*/
static Handle sb_GetBitMapSpace(register long bmSize)
{
	sb_SplineKeyHdl				splineKey 		= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;				/* <28-CEL> */
	register Handle				h				= splineKeyPtr->bitmapHand; /* <28-CEL> */
	THz							z;
	register long				result			= NO_ERR;

	if (h && !*h) {		/* purged so get rid of it and start over */	/* <28-CEL> */
		sb_DisposeZoneHandle(h, splineKeyPtr->bitmapZone);				/* <28-CEL> */
		splineKeyPtr->bitmapHand	= 0;								/* <28-CEL> */
		splineKeyPtr->bitmapZone	= 0;								/* <28-CEL> */
	}
	z	= splineKeyPtr->bitmapZone;										/* <28-CEL> */
	if (h = sb_SizeTheBlock( splineKey, splineKeyPtr->bitmapHand, &z, bmSize, kNoOptions, -1, doShrink )) {	/* <28-CEL> */
		splineKeyPtr				= *splineKey;						/* <28-CEL> */
		splineKeyPtr->bitmapZone 	= z;								/* <28-CEL> */
		splineKeyPtr->bitmapHand	= h;
	}
	
	return h;															/* <28-CEL> */

}	/* End sb_GetBitMapSpace */


/*********************************************************************************************								
**									sb_BandChar
**
**
*/
Handle sb_BandChar(sb_SplineKeyHdl splineKey, short *scan, short byteWidth, long memForScanLine)
{
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;
	register Handle				tempHand;
	long						sizeOfMem6;
	
	for ((*scan) >>= 1;*scan >= 1;(*scan) >>= 1) {
		sizeOfMem6		= (long)LONG_WORD_ALIGN((*scan * memForScanLine));			/* divide by 2 */
		
		if (!sb_SizeWorkHandle((*splineKey)->rowArrayOffset6+sizeOfMem6, kKeepContents, doShrink)) {
			if (tempHand = sb_GetBitMapSpace(*scan * byteWidth))
				return tempHand;
		}
	}
			
	return 0;
	
}	/* End sb_BandChar */
									/**********************/
									/**	END MEMORY CALLS **/
									/**********************/
/***********************************************************************************************/								


/***********************************************************************************************/								
/***********************************************************************************************/								
										/********************/
										/* SET UP AND CACHE */
										/********************/

/*********************************************************************************************						
**									sb_ResetFSWorkState
**
**	Reset the SplineKey Font Scaler state.
**
**
*/
sb_ResetFSWorkState(sb_SplineKeyPtr	splineKeyPtr)
{
	register sb_SplineKeyPtr	skPtr	= splineKeyPtr;
	
	skPtr->StrikeID		= -1;		
	skPtr->ptSize		= 0;		
	skPtr->device		= 0;		
	skPtr->face			= 0;			
	skPtr->trans00		= -1;	
	skPtr->trans11		= -1;	
	skPtr->trans10		= -1;
	
}	/* End sb_ResetFSWorkState */



/*********************************************************************************************								
**									sb_SetMemPtrs
**
**	sb_SetMemPtrs
**
**	Usage:			Called once to set up spline package vectors and init the workspace.
**
**	Notes: memory [5] points to bitmap hand
*
*	Assumes that fsWorkHand is NOT purged or empty
**
*/

static void sb_SetMemPtrs( widthTableHdl widthTableHandle )
{
	
	register sb_SplineKeyPtr	splineKeyPtr	= *(SPLINE_KEY_HAND);
	char						*memoryPtr		= *splineKeyPtr->fsWorkHand;
	register char**				memBase;	/*<16> increment ptr instead of array */

	splineKeyPtr->inPtr							= (fs_GlyphInputType *)(memoryPtr + splineKeyPtr->inputOffset);
	splineKeyPtr->outPtr						= (fs_GlyphInfoType *)(memoryPtr + splineKeyPtr->infoOffset);
	memBase = (*splineKeyPtr->inPtr).memoryBases;
	*memBase++		= memoryPtr + splineKeyPtr->globalOffset0;		/* memoryBases[0] */
	*memBase++		= memoryPtr + splineKeyPtr->globalOffset1;		/* memoryBases[1] */
	*memBase++		= memoryPtr + splineKeyPtr->globalOffset2;		/* memoryBases[2] */
	*memBase++		= memoryPtr + splineKeyPtr->sfntDataOffset3;	/* memoryBases[3] */
	*memBase++		= memoryPtr + splineKeyPtr->sfntDataOffset4;	/* memoryBases[4] */
	memBase++;														/* memoryBases[5] */
	*memBase++		= memoryPtr + splineKeyPtr->rowArrayOffset6;	/* memoryBases[6] */
	*memBase		= memoryPtr + splineKeyPtr->columnArrayOffset7;	/* memoryBases[7] */
	(*splineKeyPtr->inPtr).sfntDirectory = (widthTableHandle == nil) ? nil : (int32*) (*widthTableHandle)->fSplineDirectory;

}	/* End sb_SetMemPtrs */



/*********************************************************************************************								
**									sb_CheckWorkHandle				<28-CEL> new routine
**
**	Check to see if the workhandle exists.  If it does then just return, otherwise re-establish.
**
*/
long sb_CheckWorkHandle()
{
	sb_SplineKeyHdl				splineKey 		= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;
	register Handle 			h 				= splineKeyPtr->fsWorkHand;
	register long				result			= NO_ERR;

	if (!h || !*h) {
		long 						position		= 0;
		short						splineKeyState	= HGetState((Handle)splineKey);
		register char				*memoryPtr;

		HLock((Handle)splineKey);
		
		splineKeyPtr->inputOffset		= position;
		position						+= LONG_WORD_ALIGN(sizeof(fs_GlyphInputType));
		splineKeyPtr->infoOffset		= position; 
		position						+= LONG_WORD_ALIGN( sizeof(fs_GlyphInfoType));
	
		if (!(result = sb_SizeWorkHandle(position, kNoOptions, noShrink))) {
			memoryPtr 				= *splineKeyPtr->fsWorkHand;
			splineKeyPtr->inPtr		= (fs_GlyphInputType *)(memoryPtr + splineKeyPtr->inputOffset);
			splineKeyPtr->outPtr	= (fs_GlyphInfoType *)(memoryPtr + splineKeyPtr->infoOffset);
			(*splineKeyPtr->inPtr).GetSfntFragmentPtr	= (GetSFNTFunc)QDGetFontFrag;
			(*splineKeyPtr->inPtr).ReleaseSfntFrag		= (ReleaseSFNTFunc)QDUngetFontFrag;
			
			/* reinitialize the state to make sure the FS gets ran through */
			sb_ResetFSWorkState(splineKeyPtr);
			
			if (!(result = fs_OpenFonts( splineKeyPtr->inPtr, splineKeyPtr->outPtr ))) {
				/*******************/
				/* SETTING OFFSETS */
				/*******************/
				splineKeyPtr->globalOffset0		= position;
				position += LONG_WORD_ALIGN((*splineKeyPtr->outPtr).memorySizes[0]);
		
				splineKeyPtr->globalOffset1		= position;
				position += LONG_WORD_ALIGN((*splineKeyPtr->outPtr).memorySizes[1]);
		
				splineKeyPtr->globalOffset2		= position;
				position += LONG_WORD_ALIGN((*splineKeyPtr->outPtr).memorySizes[2]);
				splineKeyPtr->sfntDataOffset3	= position;
				/*******************/
			
				/***********************/
				/** FSWorkHand HANDLE **/
				/***********************/
				if (!(result = sb_SizeWorkHandle( position, kKeepContents, noShrink ))) {
					sb_SetMemPtrs(0);
					if (result = fs_Initialize( splineKeyPtr->inPtr, splineKeyPtr->outPtr ))
						sb_Debug();
				}
			} else sb_Debug();
			if (result)		sb_DisposeWorkHandle();		/* failed so get rid of state */
		}
		HSetState((Handle)splineKey, splineKeyState);
	}
	return result;
}	/* End sb_CheckWorkHandle */


/*********************************************************************************************						
**									sb_ShrinkBitmap
**
**	The scan converted bitmap can have extra space at the end of it so shrink down to get  rid
**	of the excess.
**
**
*/
static void sb_ShrinkBitmap( sb_GlyphData *glyphDataPtr, long *bitMapPtr, short inputScan )
{
	register short		bitWidth		= glyphDataPtr->bitWidth;
	register short		scan			= inputScan - 1;
	short 				scanByteWidth	= glyphDataPtr->scanByteWidth;

	/*************************/
	/* Shrinking down the bitmap
	** 16-bit or less	-> word 
	** 32-bit or less	-> long
	** > 32-bit			-> multiple of longs
	*/
	if (bitWidth <= 16) {		/*<16> fast scan-loop */
		register short		*src, *dest;
		
		src = dest = (short *)bitMapPtr;
		for (; scan >= 0; --scan) {
			*dest++ = *src++;
			src++;				/* next src row */
		}
		glyphDataPtr->byteWidth		= 2;
	} else {	/*<16> fast scan-loops, set src and dest once */
		register long		*src, *dest;

		src = dest = (long *)bitMapPtr;
		if (bitWidth <= 32) {
			if (scanByteWidth > 4) {					/* Sampo uses two longs even though bitwidth is only 32 */
				for (; scan >= 0; --scan) {
					*dest++ = *src++;
					src++;						/* next src row */
				}
				glyphDataPtr->byteWidth		= 4;
			}
		} else {	/*<16> fast j-loop */
			register short	numOfLongs = ((bitWidth + 31) >> 5);	/* number of longs truely needed */
			
			if (numOfLongs > (scanByteWidth >> 2)) {		/* Sampo uses an extra long if last whole long is used */
				register short j;
				--numOfLongs;			/* save decrement in j-loop */
				for (; scan >= 0; --scan) {
					for (j = numOfLongs; j >= 0; --j)
						*dest++ = *src++;
					src++;						/* next src row */
				}
				numOfLongs++;			/* restore to orig value */
				glyphDataPtr->byteWidth 	= numOfLongs << 2;			/* turn to bytes */
			}
		}
	}
}	/* End sb_ShrinkBitmap */

/*********************************************************************************************								
**									sb_SetFontMemory
**
** Note on 6/19/92 change.  This routine formerly called GetEncodingTable to get the script.
**	the funny thing about that is that GetEncodingTable's primary raison d'etre is to return
**	a table of booleans used to distinguish high and low bytes in a CJK font.  It also
**	happened to return the script of the encoding table returned.  Well, until now if
**	the FOND ID being passed in wasn't an ID for a CJK font then GetEncodingTable returned
**	roman.  This tended to make international fonts with correctly coded cmap tables not
**  work at all.  The first thing I did was change GetEncodingTable to return the correct value
**	However, I've rethought the wisdom of calling a routine primarily designed for CJK and have
**	changed this routine (which never really cared about encoding tables) to call the more generic
**	script manager trap Font2RealScript.  The change to GetEncodingTable was left in though.
*/
static long sb_SetFontMemory(sb_SplineKeyHdl splineKey, widthTableHdl widthTableHandle )
{
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;
	register long				result				= NO_ERR;
	register long				position;
	uint16						scriptID;
	
	sb_SetMemPtrs(widthTableHandle);
	//GetEncodingTable( splineKeyPtr->fLastSwapFontFamilyID, &scriptID );
	scriptID = Font2RealScript( splineKeyPtr->fLastSwapFontFamilyID );
	(*splineKeyPtr->inPtr).param.newsfnt.platformID = 1;
	(*splineKeyPtr->inPtr).param.newsfnt.specificID = scriptID;
	result = __NewSfntTrap(splineKeyPtr->inPtr, splineKeyPtr->outPtr);		//<22>
	if ( result ) {
	
		if ( scriptID != smRoman ) {	//we tried for a script this might be an old font so let's try Roman
		
			(*splineKeyPtr->inPtr).param.newsfnt.platformID = 1;
			(*splineKeyPtr->inPtr).param.newsfnt.specificID = smRoman;
			result = __NewSfntTrap(splineKeyPtr->inPtr, splineKeyPtr->outPtr);		//<22> 
	 		
		}
		
		//
		//test result again, if still non-zero that is it for us, if it is 0 we fall throught and set up
		//memory.
		//
		if ( result ) {
			sb_Debug();
			return	result;
		}
	}

	position							= splineKeyPtr->sfntDataOffset3;
	position 							+= LONG_WORD_ALIGN( (*splineKeyPtr->outPtr).memorySizes[3]);
	splineKeyPtr->sfntDataOffset4		= position;
	position 							+= LONG_WORD_ALIGN((*splineKeyPtr->outPtr).memorySizes[4]);
	splineKeyPtr->rowArrayOffset6		= position;
	splineKeyPtr->columnArrayOffset7	= position;
	
	if (result = sb_SizeWorkHandle( position, kKeepContents, noShrink ))		return MEM_ERR;

	return result;

}


/*********************************************************************************************								
**									sb_SetNewSFNT
**
**	If the sfnt has changed, reset the FontScaler environment.
**
**	Logic:
**		• Font change: run fs_NewSfnt and fs_NewTransformation
**		• ptSize or transformation changed : run fs_NewTransformation
**	
**
*/
static long sb_SetNewSFNT( widthTableHdl widTabHand )
{
	sb_SplineKeyHdl				splineKey			= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;
	register widthTablePtr		widTabPtr			= *widTabHand;
	register long				result				= NO_ERR;
	short						doNewTrans			= false;

	sb_SetMemPtrs(widTabHand);
	(*splineKeyPtr->inPtr).clientID 		= widTabPtr->tabFont;		/* Always reset because the memory environment may have changed */

	if ( splineKeyPtr->StrikeID != widTabPtr->StrikeID  ) {
		if (result = sb_SetFontMemory(splineKey, widTabHand))		return MEM_ERR;
		splineKeyPtr			= *splineKey;
		widTabPtr				= *widTabHand;
		doNewTrans				= true;
		splineKeyPtr->StrikeID 	= widTabPtr->StrikeID;
	}
	
	if ( doNewTrans	||
		 splineKeyPtr->ptSize 				  	!= widTabPtr->fSize		||
		 splineKeyPtr->trans00 					!= widTabPtr->trans00	||
		 splineKeyPtr->trans11				 	!= widTabPtr->trans11	||
		 splineKeyPtr->trans10				 	!= widTabPtr->trans10
		 ) {
		register fs_GlyphInputType* 	fsInput;
		transMatrix 					matrix;			/* this could be a global matrix */
		
		matrix.transform[2][0] = 0;
		matrix.transform[0][1] = 0;
		matrix.transform[2][1] = 0;
		matrix.transform[0][2] = 0;
		matrix.transform[1][2] = 0;
		matrix.transform[2][2] = (1L << 30);

		matrix.transform[0][0] = widTabPtr->trans00;
		matrix.transform[1][1] = widTabPtr->trans11;
		matrix.transform[1][0] = widTabPtr->trans10;

		sb_SetMemPtrs(widTabHand);
		fsInput = splineKeyPtr->inPtr;
		fsInput->param.newtrans.transformMatrix = &matrix;
		fsInput->param.newtrans.pointSize 		= (Fixed)widTabPtr->fSize << 16;
		fsInput->param.newtrans.xResolution 	= POINTSPERINCH;
		fsInput->param.newtrans.yResolution 	= POINTSPERINCH;
		fsInput->param.newtrans.pixelDiameter	= FIXEDSQRT2;
		fsInput->param.newtrans.traceFunc 		= 0;
#if TheFuture /* <18 > EBITMAP */
		fsInput->param.newtrans.bitmapMethodPreferences 		= 1;
		fsInput->param.newtrans.scalingPreference 		= 0;
#endif		
		if ( result = fs_NewTransformation( splineKeyPtr->inPtr, splineKeyPtr->outPtr ) ) {
			splineKeyPtr->ptSize	 	= 0;
			splineKeyPtr->trans00		= 0;
			splineKeyPtr->trans11 		= 0;
			splineKeyPtr->trans10		= 0;
			sb_Debug();
			return	result;
		}
		splineKeyPtr->ptSize	 	= widTabPtr->fSize;
		splineKeyPtr->trans00		= widTabPtr->trans00;
		splineKeyPtr->trans11		= widTabPtr->trans11;
		splineKeyPtr->trans10 		= widTabPtr->trans10;
	 
	}	/* End If */
	
	return NO_ERR;
	
}	/* End sb_SetNewSFNT */

/*
 *	Returns the font's real style, ignoring any algorithmic effects
 */
short IntrinsicStyle(widthTablePtr w)
{
	return (w->face >> 8) & ~w->aFace;
}

/*********************************************************************************************								
**									sb_InitCacheHead
**
**	Initialize the cacheHead with the widthTable information.
**
**
**
*/
static long sb_InitCacheHead(widthTableHdl widthTabHand, sb_CacheHeadHdl cacheHand )
{
	sb_SplineKeyHdl				splineKey			= SPLINE_KEY_HAND;
	register widthTablePtr		widTabPtr 			= *widthTabHand;
	sb_SplineKeyPtr				splineKeyPtr		= *splineKey;	/*<16> removed register */
	register sb_CacheHeadPtr	cachePtr			= *cacheHand;
	char						*hiliteModePtr		= HILITEMODE;
	long						result				= NO_ERR;

	cachePtr->familyID					= (*splineKey)->fLastSwapFontFamilyID;
	cachePtr->StrikeID					= widTabPtr->StrikeID;
	cachePtr->ptSize					= widTabPtr->fSize;
	cachePtr->face						= IntrinsicStyle(widTabPtr);
	cachePtr->squashed					= !(*hiliteModePtr & PRESERVE_GLYPH);
	cachePtr->trans00 					= widTabPtr->trans00;
	cachePtr->trans11 					= widTabPtr->trans11;
	cachePtr->trans10 					= widTabPtr->trans10;
	cachePtr->bitsCached				= widTabPtr->aSize <= MAX_PPEM_CACHED;
	cachePtr->addEntryOffset			= (GLYPH_COUNT * sizeof(long)) + sizeof(sb_CacheHead);
	cachePtr->fEncodingTablePointer		= (Ptr) GetEncodingTable( (*splineKey)->fLastSwapFontFamilyID, &cachePtr->fScriptNumber );
	
	sb_SetMemPtrs(widthTabHand);
	
	result = fs_FillLayoutInfo( splineKeyPtr->inPtr, splineKeyPtr->outPtr, (fs_LayoutInfo *)&cachePtr->ascent );
	if (!result) {
		if (cachePtr->trans10 /*OBLIQUE*/) {		/* If oblique changes change this slop adjust */
			if (widTabPtr->aSize <= 8) {
				cachePtr->rightItalicSlop	=  (cachePtr->ascent / 2);		
				cachePtr->leftItalicSlop	=  (cachePtr->descent / 2);	
			} else if (widTabPtr->aSize <= 16) {
				cachePtr->rightItalicSlop	=  (cachePtr->ascent / 3);
				cachePtr->leftItalicSlop	=  (cachePtr->descent / 3);	
			} else if (widTabPtr->aSize <= 22) {
				cachePtr->rightItalicSlop	=  (cachePtr->ascent / 4);
				cachePtr->leftItalicSlop	=  (cachePtr->descent / 4);	
			} else {
				cachePtr->rightItalicSlop	=  (cachePtr->ascent / 5);
				cachePtr->leftItalicSlop	=  (cachePtr->descent / 5);	
			}
		} else {
			cachePtr->rightItalicSlop	=  0;	
			cachePtr->leftItalicSlop	=  0;		
		}
		/********************/
		/* Round to ceiling */
		/********************/
		/*<16> put masks into registers */
		{
			Fixed lowordmask = 0x0000FFFF;
			Fixed hiwordmask = 0xFFFF0000;
			cachePtr->ascent			= (cachePtr->ascent + lowordmask) & hiwordmask;
			cachePtr->descent			= (-cachePtr->descent + lowordmask) & hiwordmask;
			if ( (*hiliteModePtr & PRESERVE_GLYPH) ) {
				cachePtr->yMax				= (cachePtr->yMax + lowordmask) & hiwordmask;
				cachePtr->yMin				= (cachePtr->yMin - lowordmask) & hiwordmask;
			} else {
				cachePtr->yMax				= cachePtr->ascent;
				cachePtr->yMin				= -cachePtr->descent;
			}
			cachePtr->height			= (short)((cachePtr->yMax - cachePtr->yMin) >> 16);
			cachePtr->widMax			= (cachePtr->widMax + lowordmask) & hiwordmask;
			cachePtr->leading			= (cachePtr->leading + lowordmask) & hiwordmask;
			cachePtr->lOverHMax			= (cachePtr->lOverHMax + lowordmask) & hiwordmask;
			cachePtr->rOverHMax			= (cachePtr->rOverHMax + lowordmask) & hiwordmask;
			cachePtr->leftItalicSlop	= -((cachePtr->leftItalicSlop + lowordmask) & hiwordmask);
			cachePtr->rightItalicSlop	= (cachePtr->rightItalicSlop + lowordmask) & hiwordmask;
			
			
			cachePtr->fmAscent			= ByteMax(cachePtr->ascent);
			cachePtr->fmDescent			= ByteMax(cachePtr->descent);
			cachePtr->fmWidMax			= ByteMax(cachePtr->widMax);
			cachePtr->fmLeading			= ByteRange(cachePtr->leading);
			
		}
		/********************/
	}

	/*<16> fast loop, local i, initArrayPtr */
	{
		register long* initArrayPtr	= (long*)cachePtr->glyphArray;
		register short i = GLYPH_COUNT - 1;
		for (; i >= 0; --i)
			*initArrayPtr++ 		= EMPTY;
	}
		
	return result;

}	/* End sb_InitCacheHead */


/*********************************************************************************************								
**									sb_ShrinkCacheToMin			<3-CEL>
**
**	Low on memory so shrink the cache to a minimum and initialize new state.
**
**
**	AddEntryOffset = 0:	Do not copy glyph entry.
**
*/
long sb_ShrinkCacheToMin(sb_CacheHeadHdl cacheHand, long addEntryOffset)
{
	sb_SplineKeyHdl				splineKey			= SPLINE_KEY_HAND;
	sb_SplineKeyPtr				splineKeyPtr		= *splineKey;
	sb_CacheHeadPtr				cachePtr			= *cacheHand;
	register long				entryArraySize		= (GLYPH_COUNT * sizeof(long));
	register long* 				initArrayPtr		= cachePtr->glyphArray;
	long						result				= NO_ERR;
	register short 				i;
	register long				*oldGDataPtr;
	register long				*newGDataPtr;
	
	/* new start for add entry */
	cachePtr->addEntryOffset			= entryArraySize + sizeof(sb_CacheHead);
	if (addEntryOffset) {
		oldGDataPtr		= (long *)((char *)cachePtr + addEntryOffset);
		newGDataPtr		= (long *)((char *)cachePtr + cachePtr->addEntryOffset);
		i	= (GLYPH_HEAD_BYTES>>2);		/* longs to copy */
		for (--i; i >= 0; --i)	
			*newGDataPtr++ = *oldGDataPtr++;
	}
	
	for (i=GLYPH_COUNT-1; i >= 0; --i)
		*initArrayPtr++ = EMPTY;

	SetHandleSize((Handle)cacheHand, sizeof(sb_CacheHead) + entryArraySize + GLYPH_HEAD_BYTES);
	if (!(result = MemError()))
		result = sb_SizeWorkHandle((*splineKey)->rowArrayOffset6, kKeepContents, doShrink);		/* <8-CEL,RB> */

	return result;

}	/* End sb_ShrinkCacheToMin */



/*********************************************************************************************						
**									sb_FigureCacheSize
**
**	Takes the current WidthTable and calculates the initial cache size needed.
**
**	Make it the minimum of the cache header and enough room for one entry header.
**
*/
static long sb_FigureCacheSize(widthTableHdl widthTabHand)
{
	register widthTablePtr	widTabPtr 			= *widthTabHand;
	register long			entryArraySize		= (GLYPH_COUNT * sizeof(long));
	register long			cacheSize			= 0;
	Boolean					bitsCached			= widTabPtr->aSize < MAX_PPEM_CACHED;

	cacheSize			= sizeof(sb_CacheHead) + GLYPH_HEAD_BYTES;		/* <34-CEL> */
	
#ifdef LEAVE_OUT				/* <33-CEL> forget about room for characters for now */
	if ( bitsCached )
			cacheSize	+= (widTabPtr->aSize / 8) * widTabPtr->aSize * INITIAL_CHAR_COUNT;
	else	cacheSize	+= AVG_CONTOUR_SIZE * INITIAL_CHAR_COUNT;
#endif

	cacheSize	+= entryArraySize;				/* Add glyph array */
	cacheSize	+= 3;	cacheSize	&= ~3;		/* Start Long word aligned */

	return cacheSize;

}	/* End sb_FigureCacheSize */


/*********************************************************************************************							
**									sb_ResetHeapState
**
**
**
*/
void sb_ResetHeapState( sb_SplineKeyPtr splineKeyPtr )
{
	register sb_SplineKeyPtr	sPtr = splineKeyPtr;
	
	sPtr->curAppZone 	= 0;				/* <30-CEL> */			
	sPtr->appFull 		= false;			/* <30-CEL> */	
	sPtr->sysFull 		= false;			/* <30-CEL> */
	sPtr->useHeap 		= useApp;			/* <30-CEL> */
	
}	/* End sb_ResetHeapState */


/*********************************************************************************************							
**									sb_FlushFonts
**
**	sb_FlushFonts
**
**
*/
pascal OSErr sb_FlushFonts()
{
	sb_SplineKeyHdl		splineKey		= SPLINE_KEY_HAND;
	sb_SplineKeyPtr		splineKeyPtr	= *splineKey;
	long* 				lastSpExtrPtr	= LASTSPEXTRA;
		
	*lastSpExtrPtr		= 0xFFFFFFFF;

	if (splineKeyPtr->cacheListHandle) {
		register short fontCount = splineKeyPtr->fontCount;
		register CacheReference* cacheRef = *splineKeyPtr->cacheListHandle;
		register Handle zero = 0;
		for (--fontCount; fontCount >= 0; --fontCount) {
			if (cacheRef->cache) {
				sb_DisposeCacheHandle(cacheRef->cache, cacheRef->zone);
				cacheRef->cache	= zero;
			}
			cacheRef++;
		}
		splineKeyPtr->cacheHand = 0;		/* <28-CEL> */
		splineKeyPtr->cacheZone = 0;		/* <28-CEL> */
		splineKeyPtr->cachePlace = 0;		/* <28-CEL> */
		
		sb_DisposeZoneHandle((Handle)splineKeyPtr->cacheListHandle,		/* <28-CEL> start over */
							 HandleZone((Handle)splineKeyPtr->cacheListHandle));
		splineKeyPtr->cacheListHandle	= 0;
	}
	
	sb_DisposeWorkHandle();					/* <28-CEL> */
	
	/* Dispose of all fragment table caches */
	KillApplicationHeapFragmentTableCaches( (Handle) 0, (Handle) 0x7fffffff );
	
	/* Reset since the heaps have changed */
	sb_ResetHeapState(splineKeyPtr);

	return NO_ERR;
}	/* End sb_FlushFonts */


/*********************************************************************************************							
**									sb_KillSomeCaches
**
**	Clean up caches in the current heap that is going away.  If the cachelisthandle is going
**	away, throw away the caches and get rid of the cachelisthandle.  Make sure to get rid of
**	any blocks that are in this heap also like the fsWorkHand. 
**
**
*/
pascal long sb_KillSomeCaches( Handle inputStartAddr, Handle inputEndAddr )
{
	register sb_SplineKeyPtr	splineKeyPtr	= *(SPLINE_KEY_HAND);		/* <28-CEL> register */
	register CacheReference* 	cacheR;										/* <28-CEL> */
	register short 				fontCount		= splineKeyPtr->fontCount;
	register Handle				startAddr		= inputStartAddr;			/* <28-CEL> */
	register Handle				endAddr			= inputEndAddr;				/* <28-CEL> */
	register Handle 			h;											/* <28-CEL> */

	if (splineKeyPtr->cacheListHandle) {					/* <28-CEL> make sure it even exists */
		cacheR			= *splineKeyPtr->cacheListHandle;					/* <28-CEL> */
		h				= (Handle)splineKeyPtr->cacheListHandle;			/* <28-CEL> */
		if (h >= startAddr && h < endAddr) {				/* <28-CEL> cacheListHand in this heap clean all caches */
			for (--fontCount; fontCount >= 0; --fontCount) {							/* <28-CEL> */
				if (cacheR->cache)	sb_DisposeCacheHandle(cacheR->cache, cacheR->zone);	/* <28-CEL> */
				cacheR++;																/* <28-CEL> */
			}
			sb_DisposeZoneHandle(h, HandleZone(h));
			splineKeyPtr->cacheListHandle	= 0;		/* <28-CEL> */
			splineKeyPtr->cacheHand			= 0;		/* <28-CEL> */
			splineKeyPtr->cacheZone			= 0;		/* <28-CEL> */
			splineKeyPtr->cachePlace		= 0;		/* <28-CEL> */
			sb_ResetFSWorkState(splineKeyPtr);
		} else {
			for (--fontCount; fontCount >= 0; --fontCount) {
				h 		= cacheR->cache;						/* <28-CEL> */
				
				if (h >= startAddr && h < endAddr) {			/* <28-CEL> */
					if (splineKeyPtr->cacheHand == h) {			/* <28-CEL> */
						splineKeyPtr->cacheHand 	= 0;		/* <28-CEL> */
						splineKeyPtr->cacheZone		= 0;		/* <28-CEL> */
						splineKeyPtr->cachePlace	= 0;		/* <28-CEL> */
					}
					
					sb_DisposeCacheHandle( cacheR->cache, cacheR->zone );
					cacheR->cache		= 0;
					cacheR->zone		= 0;					/* <28-CEL> */
				}
				cacheR++;
			}
		}
	}
		
	KillApplicationHeapFragmentTableCaches( startAddr, endAddr );

	if ((h = splineKeyPtr->bitmapHand) && (h >= startAddr && h < endAddr)) {	/* <28-CEL> */
		sb_DisposeZoneHandle(h, splineKeyPtr->bitmapZone);						/* <28-CEL> */
		splineKeyPtr->bitmapHand		= 0;									/* <28-CEL> */
		splineKeyPtr->bitmapZone		= 0;									/* <28-CEL> */
	}
	
	if ((h = splineKeyPtr->fsWorkHand) && (h >= startAddr && h < endAddr))		/* <28-CEL> */											/* <28-CEL> */
		sb_DisposeWorkHandle();													/* <28-CEL> */

	// <19> If FOND candidate lists are cached, flush them out too.

	h = splineKeyPtr->fondCache[0];												// <19>
	if (h) {																	// <19>
		DisposeHandle(h);														// <19>
		splineKeyPtr->fondCache[0] = nil;										// <19>
	}
	h = splineKeyPtr->fondCache[1];												// <19>
	if (h) {																	// <19>
		DisposeHandle(h);														// <19>
		splineKeyPtr->fondCache[1] = nil;										// <19>
	}
		
	/* Reset since the heaps have changed */
	sb_ResetHeapState(splineKeyPtr);
	
	return NO_ERR;
}	/* End sb_KillSomeCaches */



/*********************************************************************************************						
**									sb_CacheSFNTMatch
**
**	Simply checks to see if the current widthtable request matches the cache found.
**
**
*/
Boolean sb_CacheSFNTMatch( sb_CacheHeadPtr cachePtr, widthTablePtr widTabPtr )
{
	register sb_CacheHeadPtr		cPtr	= cachePtr;		/* registers */
	register widthTablePtr 			wtPtr	= widTabPtr;

	return(cPtr->StrikeID 	== wtPtr->StrikeID 			&&
		   cPtr->ptSize		== wtPtr->fSize 			&&
		   cPtr->face 		== IntrinsicStyle(wtPtr)	&&
		   cPtr->trans00 	== wtPtr->trans00  			&&
		   cPtr->trans11 	== wtPtr->trans11  			&&
		   cPtr->trans10	== wtPtr->trans10);
}	/* End sb_CacheSFNTMatch */


/*********************************************************************************************						
**									sb_LoadNewCache
**
**	On entry
**		splineKeyPtr is locked
**		
**
*/
long sb_LoadNewCache( sb_SplineKeyHdl splineKey, widthTableHdl widthTabHand, Boolean squashed )
{
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;			/* <28-CEL> */
	register CacheReference* 	cacheR			= *splineKeyPtr->cacheListHandle;
	register short 				i, fontCount	= splineKeyPtr->fontCount;
	register short 				cachePlace 		= -1;
	long						result			= NO_ERR;

	/* Search through the list of caches to see if we have a match */
	for (i = 0; i < fontCount; i++) {
		if (cacheR->cache && *cacheR->cache) {
			sb_CacheHeadPtr cachePtr	= (sb_CacheHeadPtr)*cacheR->cache;
			if (sb_CacheSFNTMatch(cachePtr, *widthTabHand) && cachePtr->squashed == squashed) {
				splineKeyPtr->cacheHand 	= (sb_CacheHeadHdl)cacheR->cache;	/* <28-CEL> */
				splineKeyPtr->cacheZone		= cacheR->zone;						/* <28-CEL> */
				splineKeyPtr->cachePlace	= i;								/* <28-CEL> */
				HNoPurge((Handle)splineKeyPtr->cacheHand);	/* Current one is non-purgable */
				break;
			}	/* End If */ 
		} else if (cachePlace == -1)
			cachePlace		= i;	/* remember first available (purged or gone) */
		cacheR++;
	}	/* End For */

	if (!splineKeyPtr->cacheHand) {					/* no match, use cachePlace if valid */
		THz 		zone;							/* <28-CEL> just moved this above */
		Handle		cache;							/* <28-CEL> just moved this above */

		if (cachePlace == -1) {						/* -1 means grow the cache */
			cachePlace	= splineKeyPtr->fontCount;	/* first available of new caches */
			if ( sb_ModCacheList(splineKey) ) {		/* add more cache heads if needed */
				cachePlace	= 1;
			}
		}	/* End If */

		cacheR			= *splineKeyPtr->cacheListHandle + cachePlace;			/* <28-CEL> */
		zone			= cacheR->zone;

		if (cacheR->cache && !*cacheR->cache) {	/* <28-CEL> been purged so get rid of it and start over */
			sb_DisposeZoneHandle( cacheR->cache, zone );
			cacheR->cache = 0;
			cacheR->zone = 0;
		}
		
		result = LoadFontCacheFromDisk( (sb_CacheHeadHdl*) &cache, &zone, widthTabHand, squashed );
		cacheR = *splineKeyPtr->cacheListHandle + cachePlace;		
		if ( (result == noErr) && (cache != nil) ) {
		
			cacheR->cache = cache;
			cacheR->zone = zone;
			splineKeyPtr->cacheHand = (sb_CacheHeadHdl) cache;
			splineKeyPtr->cacheZone = zone;
			splineKeyPtr->cachePlace = cachePlace;
			HNoPurge( cache );
		
		} else {

			if (cache = sb_SizeTheBlock( splineKey, cacheR->cache, &zone, sb_FigureCacheSize(widthTabHand), 
										   kKeepContents, cachePlace, doShrink )) {
				if (!(result = sb_SetNewSFNT(widthTabHand))) {
					if (!(result = sb_InitCacheHead(widthTabHand, (sb_CacheHeadHdl)cache))) {
						cacheR			= *splineKeyPtr->cacheListHandle + cachePlace;	/* <34> */
						cacheR->cache	= cache;										/* <34> */
						cacheR->zone	= zone;											/* <34> */
						splineKeyPtr->cacheHand		= (sb_CacheHeadHdl)cacheR->cache;	/* <34> */
						splineKeyPtr->cacheZone		= cacheR->zone;						/* <34> */
						splineKeyPtr->cachePlace	= cachePlace;						/* <34> */	
						HNoPurge(cache);
					}
				}
				if (result) {															/* <34> */
					cacheR			= *splineKeyPtr->cacheListHandle + cachePlace;		/* <34> */
					cacheR->cache	= 0;
					cacheR->zone	= 0;
					DisposHandle(cache);		/* Get rid of cachehandle */
					sb_Debug();
				}
			} else	result = GROWZONE_HANDLE_ERR;
			
		}
		
	}	/* End If */
	
	return result;
	
}	/* End sb_LoadNewCache */


/*********************************************************************************************						
**									sb_SearchForCache
**
**	Find the cache that matches the width table request…
**		• Check if the current one is okay - FAST CASE
**		• NOT FOUND: Check through the list of caches to see if one exists
**		• NOT FOUND: Get a brand new one
**		• NOT FOUND: re-use a cache
**
*/
pascal long sb_SearchForCache( widthTableHdl widthTabHand )
{
	sb_SplineKeyHdl				splineKey		= SPLINE_KEY_HAND;		/* <28-CEL> */
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;			/* <28-CEL> */
	register widthTablePtr		widTabPtr		= *widthTabHand;		/* <27-CEL> */
	long						result			= NO_ERR;
	char* 						hiliteModePtr	= HILITEMODE;
	register Boolean			squashed		= !(*hiliteModePtr & PRESERVE_GLYPH); 
	short						splineKeyState, fontState, workHandState;
	register Handle				aCacheHand		= splineKeyPtr->cacheHand;

	if (aCacheHand && *aCacheHand) {			/* Current cache okay??? Fast case */
		sb_CacheHeadPtr cachePtr	= (sb_CacheHeadPtr)*aCacheHand;
		if ( sb_CacheSFNTMatch(cachePtr, widTabPtr) && cachePtr->squashed == squashed)
				return NO_ERR;
		else	HPurge((Handle)aCacheHand);		/* <16-CEL> */
	}
	splineKeyPtr->cacheHand 	= 0;			/* we need a new current cache */
	splineKeyPtr->cacheZone		= 0;			/* <16-CEL> */
	splineKeyPtr->cachePlace	= 0;			/* <16-CEL> */
	
	widTabPtr		= *widthTabHand;
	splineKeyPtr	= *splineKey;
	splineKeyState	= HGetState ((Handle)splineKey);
	fontState		= HGetState ((Handle)widTabPtr->tabFont);
	HLock((Handle)splineKey);											/* <32-CEL> */
	HNoPurge(widTabPtr->tabFont);

	if (!(result = sb_CheckWorkHandle())) {								/* <32-CEL> */
		workHandState		= HGetState ((Handle)splineKeyPtr->fsWorkHand);
		HNoPurge(splineKeyPtr->fsWorkHand);								/* <32-CEL> */
		if (splineKeyPtr->cacheListHandle || !(result = sb_ModCacheList(splineKey)))
			result = sb_LoadNewCache(splineKey, widthTabHand, squashed);
		HSetState((Handle)splineKeyPtr->fsWorkHand, workHandState);
	}

	HPurge((Handle)(*splineKey)->cacheHand);							/* 15-CEL */
	HSetState((Handle)splineKey, splineKeyState);
	HSetState((*widthTabHand)->tabFont, fontState);
	return result;

}	/* End sb_SearchForCache */




/***************************************************************************************************/
/***************************************************************************************************/
								/**********************************/
								/* ROUTINES THAT CALL FONT SCALER */
								/**********************************/

/*********************************************************************************************								
**									sb_SetUpWorkHand
**
**	sb_SetUpWorkHand
**
**
*/
static long sb_SetUpWorkHand()
{
	sb_SplineKeyHdl				splineKey		= SPLINE_KEY_HAND;
	long 						position		= 0;
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;
	short						splineKeyState	= HGetState ((Handle)splineKey);
	long						result;
	register char				*memoryPtr;

	HLock((Handle)splineKey);
	
	splineKeyPtr->inputOffset		= position;
	position						+= LONG_WORD_ALIGN(sizeof(fs_GlyphInputType));
	splineKeyPtr->infoOffset		= position; 
	position						+= LONG_WORD_ALIGN( sizeof(fs_GlyphInfoType));

	if (result = sb_SizeWorkHandle(position, kNoOptions, noShrink))		goto EXIT;			/* Error so can not continue */

	memoryPtr 				= *splineKeyPtr->fsWorkHand;
	splineKeyPtr->inPtr		= (fs_GlyphInputType *)(memoryPtr + splineKeyPtr->inputOffset);
	splineKeyPtr->outPtr	= (fs_GlyphInfoType *)(memoryPtr + splineKeyPtr->infoOffset);
	(*splineKeyPtr->inPtr).GetSfntFragmentPtr				= (GetSFNTFunc)QDGetFontFrag;
	(*splineKeyPtr->inPtr).ReleaseSfntFrag					= (ReleaseSFNTFunc)QDUngetFontFrag;;
	
	/* reinitialize the state to make sure the FS gets ran through */
	sb_ResetFSWorkState(splineKeyPtr);
	
	if (!(result = fs_OpenFonts( splineKeyPtr->inPtr, splineKeyPtr->outPtr ))) {
		/*******************/
		/* SETTING OFFSETS */
		/*******************/
		splineKeyPtr->globalOffset0		= position;
		position += LONG_WORD_ALIGN((*splineKeyPtr->outPtr).memorySizes[0]);

		splineKeyPtr->globalOffset1		= position;
		position += LONG_WORD_ALIGN((*splineKeyPtr->outPtr).memorySizes[1]);

		splineKeyPtr->globalOffset2		= position;
		position += LONG_WORD_ALIGN((*splineKeyPtr->outPtr).memorySizes[2]);
		splineKeyPtr->sfntDataOffset3	= position;
		/*******************/
	
		/***********************/
		/** FSWorkHand HANDLE **/
		/***********************/
		if (!(result = sb_SizeWorkHandle( position, kKeepContents, noShrink ))) {
			sb_SetMemPtrs(0);
			if (result = fs_Initialize( splineKeyPtr->inPtr, splineKeyPtr->outPtr ))
				sb_Debug();
		}
	}	/* End if */

EXIT:
	HSetState((Handle)splineKey, splineKeyState);
	return result;
}	/* End sb_SetUpWorkHand */

/*********************************************************************************************								
**									sb_SFNTMatchSize
**
**	sb_SFNTMatchSize
**
**
*/
pascal Boolean sb_SFNTMatchSize(Handle /*sfntHand*/, short /*matchSize*/)
{
	return true;

}	/* End sb_SFNTMatchSize */





/*********************************************************************************************								
**								AppendSplineDirectoryToWidthTable
**
**	AppendSplineDirectoryToWidthTable
**
**	This routine appends the spline directory for the sfnt specified by the widthTableHandle's
**	tabFont field to the end of the widthTableHandle by resizing the widthTableHandle and
**	blockmoving the directory data.
**
*/


long AppendSplineDirectoryToWidthTable( Handle splineHandle, widthTableHdl widthTableHandle )

{

	// Locals
	
		long									error;
		Size									splineDirectorySize;
		sfnt_OffsetTable*						splineDirectoryPointer;
	
	error = memFullErr;
	splineDirectoryPointer = (sfnt_OffsetTable*) QDGetFontFrag( splineHandle, 0, OFFSETTABLESIZE );
	if ( splineDirectoryPointer != nil ) {
	
		splineDirectorySize = OFFSETTABLESIZE + (splineDirectoryPointer->numOffsets * sizeof(sfnt_DirectoryEntry));
		QDUngetFontFrag( splineHandle, splineDirectoryPointer );
		
		splineDirectoryPointer = (sfnt_OffsetTable*) QDGetFontFrag( splineHandle, 0, splineDirectorySize );
		if ( splineDirectoryPointer != nil ) {
		
			SetHandleSize( (Handle) widthTableHandle, sizeof( widthTable ) + splineDirectorySize );
			if ( (error = MemError( )) == noErr ) {
			
				BlockMove( (Ptr) splineDirectoryPointer, (Ptr) (*widthTableHandle)->fSplineDirectory, splineDirectorySize );
				
			}
		
		}
		
		QDUngetFontFrag( splineHandle, splineDirectoryPointer );
	
	}

	return( error );
	
}


/*********************************************************************************************								
**									sb_FillWidthTab
**
**	sb_FillWidthTab
**
**	This routine is called by the Font Manager when a new font width table needs to be filled
**	out with the proper metrics for spline fonts.
**
*/
pascal long sb_FillWidthTab(widthTableHdl widthTabHand, Boolean fractEnable)
{
	register sb_SplineKeyHdl		splineKey		= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr		splineKeyPtr	= *splineKey;
	register widthTablePtr			widTabPtr 		= *widthTabHand;
	short							FontHandState	= HGetState (widTabPtr->tabFont);
	FMOutput						*fmRecPtr		= &FMOUTPUT_PTR;
	register long					result			= NO_ERR;
	Boolean							runInMinMode	= false;

#ifndef SYSTEM_7
	if (!splineKeyPtr->mfExists)			/* If it's not already running, check */
		splineKeyPtr->mfExists = MultiFinderExist();
#endif

	HNoPurge(widTabPtr->tabFont);

	widTabPtr->vOutput		= 1 << 8;			/* reset to no factor */
	widTabPtr->hOutput		= 1 << 8;
	widTabPtr->vFactor		= 1 << 8;
	widTabPtr->hFactor		= 1 << 8;	
	widTabPtr->notFast 		= (fmRecPtr->boldPixels || fmRecPtr->ulThick || fmRecPtr->shadowPixels);
	widTabPtr->trans00 		= FixDiv(widTabPtr->inNumer.h, widTabPtr->inDenom.h);
	widTabPtr->trans11 		= FixDiv(widTabPtr->inNumer.v, widTabPtr->inDenom.v);

	if (widTabPtr->aFace & OBLIQUE_BIT) {
		if (widTabPtr->aSize <= 8) {
			widTabPtr->trans10 = (widTabPtr->trans00) / 2;
		} else if (widTabPtr->aSize <= 16) {
			widTabPtr->trans10 = (widTabPtr->trans00) / 3;
		} else if (widTabPtr->aSize <= 22) {
			widTabPtr->trans10 = (widTabPtr->trans00) / 4;
		} else {
			widTabPtr->trans10 = (widTabPtr->trans00) / 5;
		}
	} else 		widTabPtr->trans10 = 0L;

	result = AppendSplineDirectoryToWidthTable( widTabPtr->tabFont, widthTabHand );
	if ( result == noErr ) {
	
		widTabPtr = *widthTabHand;

		if ((result = sb_CheckWorkHandle()) == MEM_ERR) {
			splineKeyPtr = *splineKey;
			if (!(result = sb_ShrinkCacheToMin(splineKeyPtr->cacheHand, 0))) {
				runInMinMode	= true;
				result 			= sb_CheckWorkHandle();
			}
		}
		if (!result) {
			splineKeyPtr	= *splineKey;
			HNoPurge(splineKeyPtr->fsWorkHand);								/* <30-CEL> */
			if (((result = sb_SetNewSFNT(widthTabHand)) == MEM_ERR) && !runInMinMode) {
				splineKeyPtr	= *splineKey;
				if (!(result = sb_ShrinkCacheToMin(splineKeyPtr->cacheHand, 0)))
					result 			= sb_SetNewSFNT(widthTabHand);
			}
			splineKeyPtr	= *splineKey;
			if (!result) {
				widTabPtr 		= *widthTabHand;
				sb_SetMemPtrs(widthTabHand);
	
				result = fs_FillWidthTable(splineKeyPtr->inPtr, (Fixed*)widTabPtr, widTabPtr->tabFont, widTabPtr->style, widTabPtr->fID, !fractEnable);
				if (result)
					sb_Debug();
			}
			HPurge(splineKeyPtr->fsWorkHand);								/* <30-CEL> */
		}
		
	}
	
	HSetState((**widthTabHand).tabFont, FontHandState);
	return (**widthTabHand).badFontFlag	= result;

}	/* End sb_FillWidthTab */


/*********************************************************************************************								
**									sb_FillPrinterWidths
**
**	sb_FillPrinterWidths
**
**	This routine is called by the Font Manager when a new font width table needs to be filled
**	out with the proper metrics for spline fonts.
**
*/
pascal long sb_FillPrinterWidths(Fixed *widthsArrayPtr, Handle sfntHand)
{
	sb_SplineKeyHdl					splineKey		= SPLINE_KEY_HAND;	/*<16> removed register */
	register sb_SplineKeyPtr		splineKeyPtr	= *splineKey;
	register long					result			= NO_ERR;
	register widthTablePtr 			widTabPtr;

#ifndef SYSTEM_7
	if (!splineKeyPtr->mfExists)			/* If it's not already running, check */
		splineKeyPtr->mfExists = MultiFinderExist();
#endif

	HLock( (Handle)splineKey );
	
	if (result = sb_CheckWorkHandle())		goto CLEANUP;
	HNoPurge(splineKeyPtr->fsWorkHand);									/* <28-CEL> */

	if (!splineKeyPtr->tempWidthTabHand) {
		splineKeyPtr->tempWidthTabHand		= (widthTableHdl)NewHandleSys(sizeof(widthTable));
		result = MemError();
	} else if (!*splineKeyPtr->tempWidthTabHand) {
		THz prevZone = GetZone();
		
		SetZone(SystemZone());
		ReallocHandle((Handle)splineKeyPtr->tempWidthTabHand, sizeof(widthTable));
		result = MemError();
		SetZone(prevZone);
	}
	
	if (!result) {
		widTabPtr				= *splineKeyPtr->tempWidthTabHand;
		widTabPtr->tabFont		= sfntHand;
		widTabPtr->fSize		= 1;
		widTabPtr->inNumer.h	= 1;
		widTabPtr->inNumer.v	= 1;
		widTabPtr->inDenom.h	= 1;
		widTabPtr->inDenom.v	= 1;
		widTabPtr->face			= 0;
		widTabPtr->aFace		= 0;
		widTabPtr->aSize		= 1;
		widTabPtr->trans00		= 1L << 16;
		widTabPtr->trans11		= 1L << 16;
		widTabPtr->trans10		= 0;
		
		result = AppendSplineDirectoryToWidthTable( sfntHand, splineKeyPtr->tempWidthTabHand );
		if ( result == noErr ) {
		
			if (result = sb_SetNewSFNT ( splineKeyPtr->tempWidthTabHand ))	
				sb_Debug();
			else {
				short	sfntID;			/* hack, should be id of this sfnt (passed in by caller) */
				ResType	sfntType;
				Str255	sfntName;
				
				GetResInfo( sfntHand, &sfntID, &sfntType, &sfntName[0] );
				sb_SetMemPtrs(splineKeyPtr->tempWidthTabHand);
				if (result = fs_FillWidthTable(splineKeyPtr->inPtr, widthsArrayPtr, sfntHand, 0, sfntID, 0))
					sb_Debug();
			}
		}
	} else sb_Debug();
	
	HPurge(splineKeyPtr->fsWorkHand);									/* <28-CEL> */
CLEANUP:
	HPurge((Handle)(splineKeyPtr->tempWidthTabHand));		/* Do not need if memory is needed */
	HUnlock((Handle)splineKey);
	return result;

}	/* End sb_FillPrinterWidths */


/*********************************************************************************************			
**									sb_PreFlightFontMem
**
**	sb_PreFlightFontMem
**
**	LONG_WORD_ALIGN((ppem+8) >> 3) == scan bytes long aligned.
**
**	Called by:		DrText, FMSwapFont
**
**	This preflighting will get better!!!
*/
#define CACHE_ARRAY_SIZE		1024
pascal long sb_PreFlightFontMem ( widthTableHdl widTabHand, Handle fontHandle, short scanLines, short ppem )
{
	sb_SplineKeyHdl				splineKey		= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr	splineKeyPtr	= *splineKey;
	register long				result			= NO_ERR;
	Handle						tempHand;
	register long				bitAndSlopSpace;
	register widthTablePtr		widTabPtr		= *widTabHand;
	short						fontHandState;

#ifndef SYSTEM_7
	if (!splineKeyPtr->mfExists)			/* If it's not already running, check */
		splineKeyPtr->mfExists = MultiFinderExist();
#endif

	fontHandState	= HGetState(fontHandle);
	HNoPurge(fontHandle);
	
	result = AppendSplineDirectoryToWidthTable( fontHandle, widTabHand );
	if ( result == noErr ) {
	
		if (!(result = sb_CheckWorkHandle())) {
			splineKeyPtr		= *splineKey;
			sb_ResetFSWorkState(splineKeyPtr);
			HNoPurge(splineKeyPtr->fsWorkHand);
			
			sb_SetMemPtrs(widTabHand);
			(*splineKeyPtr->inPtr).clientID 		= fontHandle;
			if (!(result = sb_SetFontMemory(splineKey, widTabHand))) {
				register short		calcRowBytes = (LONG_WORD_ALIGN((ppem+8) >> 3));
	
				scanLines			= 4;		/* Hard code minumum band */
				bitAndSlopSpace		= scanLines * calcRowBytes;
				bitAndSlopSpace		+= (sizeof(sb_CacheHead) + CACHE_ARRAY_SIZE);
				/* bitAndSlopSpace		+= (ppem * calcRowBytes); */	/* for scan converter extra */
	
				if (!(tempHand = sb_GetBitMapSpace(bitAndSlopSpace))) {
					result	= PRE_FLIGHT_ERR;
				} else 		HPurge(tempHand);
			}
			HPurge((*splineKey)->fsWorkHand);
		}
	}
	HSetState(fontHandle, fontHandState);
	return result;

}	/* End sb_PreFlightFontMem */


/*********************************************************************************************						
**									sb_RenderGlyphInfo
**
**	sb_RenderGlyphInfo
**
*/
static long sb_RenderGlyphInfo ( sb_SplineKeyHdl splineKey, widthTableHdl widTabHand, long addEntryOffset )
{
	register sb_SplineKeyPtr	splineKeyPtr;
	register widthTablePtr		widTabPtr;
	register sb_CacheHeadPtr	cachePtr;
	register fs_GlyphInfoType	*infoPtr;
	register sb_GlyphData		*glyphDataPtr;
	char						*hiliteModePtr	= HILITEMODE;
	long						result;
	BitMap						*bitMapPtr;
	int16						ascent, descent, yMax, yMin;
	Fixed						newTrans11		= (*widTabHand)->trans11;
	unsigned char				resetTrans	= 0;						/* <14> */

SQUASH:
	if (result = sb_SetNewSFNT ( widTabHand )) 		return result;

	/* re-estab ptrs */
	splineKeyPtr	= *splineKey;
	widTabPtr		= *widTabHand;
	cachePtr		= *splineKeyPtr->cacheHand;
	glyphDataPtr	= (sb_GlyphData *)((char *)cachePtr + addEntryOffset);
	
	sb_SetMemPtrs(widTabHand);
#if TheFuture /* <18 > EBITMAP */
		(*splineKeyPtr->inPtr).param.newglyph.bitmapMethodPreferences 		= 1;
		(*splineKeyPtr->inPtr).param.newglyph.scalingPreference 		= 0;
#endif		
	(*splineKeyPtr->inPtr).param.newglyph.characterCode 	= glyphDataPtr->glyphID;
	if (result = fs_NewGlyph( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {
		sb_Debug();
		return	result;
	}

	(*splineKeyPtr->inPtr).param.gridfit.styleFunc = 0;
	(*splineKeyPtr->inPtr).param.gridfit.traceFunc = 0;

#if TheFuture /* <18 > EBITMAP */
	if (result = fs_GetGlyphInfo( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {
		sb_Debug();
		return	result;
	}
	if (!(*splineKeyPtr->outPtr).outlinesExist) {
		return SKIP_CHAR;	/* really means char is ok, but has no contours */
	}
#endif
#if !TheFuture /* <18 > EBITMAP */
	if (result = fs_ContourGridFit( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {
		sb_Debug();
		return	result;
	}
	if (!(*splineKeyPtr->outPtr).outlinesExist) {
		return SKIP_CHAR;	/* really means char is ok, but has no contours */
	}
	if (result = fs_FindBitMapSize( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {
		sb_Debug();
		return	result;
	}
#endif

	ascent						= (cachePtr->ascent >> 16);
	descent						= -(cachePtr->descent >> 16);			/* turn back into it’s true sign */

	infoPtr						= (fs_GlyphInfoType	*)splineKeyPtr->outPtr;
	bitMapPtr					= &(infoPtr->bitMapInfo);
	glyphDataPtr->yMin = yMin	= bitMapPtr->bounds.top;
	glyphDataPtr->yMax = yMax 	= bitMapPtr->bounds.bottom;

	/* Squashing */
	if ( !(*hiliteModePtr & PRESERVE_GLYPH) && (resetTrans<2) && (yMax > ascent || yMin < descent)) /* <14> */
	{
		Fixed				maxTrans11, minTrans11;								/* <14> */
		++resetTrans;															/* <14> */

		if (yMax > ascent && ascent > 0)										/* <14> */
				maxTrans11 = ShortMulDiv(widTabPtr->trans11, ascent, yMax);		/* <14> */
		else	maxTrans11 = widTabPtr->trans11;								/* <14> */		
		if (yMin < descent && descent < 0)										/* <14> */
				minTrans11 = ShortMulDiv(widTabPtr->trans11, descent, yMin);	/* <14> */
		else	minTrans11 = widTabPtr->trans11;								/* <14> */
		
		widTabPtr->trans11	= (maxTrans11 < minTrans11) ? maxTrans11 : minTrans11;/* <14> */
			
		goto SQUASH;
	}	/* End Testing no line height */

	glyphDataPtr->lsb 			= infoPtr->metricInfo.leftSideBearing.x;
	glyphDataPtr->devLSB 		= infoPtr->metricInfo.devLeftSideBearing.x >> 16;
	glyphDataPtr->xMin			= infoPtr->bitMapInfo.bounds.left;
	glyphDataPtr->bitWidth 		= bitMapPtr->bounds.right - bitMapPtr->bounds.left;
	glyphDataPtr->scan 			= yMax - yMin;
	glyphDataPtr->byteWidth 	= glyphDataPtr->scanByteWidth = (uint32)bitMapPtr->rowBytes;

	widTabPtr->trans11 			= newTrans11;									/* <14> */

	return	NO_ERR;
}	/* End sb_RenderGlyphInfo */


/*********************************************************************************************						
**									sb_RenderMemShrink
**
**	
**
*/
long sb_RenderMemShrink(sb_SplineKeyHdl splineKey, long *addEntryOffset, Boolean *runInMinMode)
{
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;	
	register sb_CacheHeadPtr	cachePtr;
	long						result;
	
	if (result = sb_ShrinkCacheToMin(splineKeyPtr->cacheHand, *addEntryOffset)) {
		sb_Debug();
		return result;
	} else {
		*runInMinMode 			= true;
		cachePtr				= *splineKeyPtr->cacheHand;
		*addEntryOffset			= cachePtr->addEntryOffset;
	}
	return NO_ERR;
}	/* End sb_RenderMemShrink */


/*********************************************************************************************						
**									sb_RoomForHead
**	Make room for the header
**
**	MOVES MEMORY
**
*/
long sb_RoomForHead(sb_SplineKeyHdl splineKey, long *addEntryOffset, Boolean *runInMinMode)
{
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;	
	long						cacheSize;
	register Handle				tempHand;

	/**************************
	** Make room for the header
	**
	** MOVES MEMORY
	*/
	cacheSize		= GetHandleSize((Handle)splineKeyPtr->cacheHand);
	if (GLYPH_HEAD_BYTES > ( cacheSize - *addEntryOffset )) {
		THz		zone = splineKeyPtr->cacheZone;
		
		if (tempHand = sb_SizeTheBlock(splineKey, (Handle)splineKeyPtr->cacheHand, &zone, 
										 *addEntryOffset + GLYPH_HEAD_BYTES, kKeepContents,
										 splineKeyPtr->cachePlace, noShrink)) {
			splineKeyPtr			= *splineKey;
			splineKeyPtr->cacheHand = (sb_CacheHeadHdl)tempHand;
			splineKeyPtr->cacheZone = zone;
		} else	return(sb_RenderMemShrink(splineKey, addEntryOffset, runInMinMode));
	}
	return NO_ERR;
}	/* End 	sb_RoomForHead */




/*********************************************************************************************						
**									sb_RangeCheck
**
**	
**
*/
long sb_RangeCheck(sb_CacheHeadPtr cachePtr, sb_GlyphData *glyphDataPtr, 
				   short yMin16, short yMax16, long addEntryOffset, long glyphEntryOffset)
{
	/****** Font Scaler may return completely bogus values because of a bad font *******/
	/***** is the font bad????  do some range checking ******/
	if (!glyphDataPtr->scan			||
		glyphDataPtr->yMax < yMin16 ||
		glyphDataPtr->yMin > yMax16 ||
		glyphDataPtr->bitWidth == 0 || 
		glyphDataPtr->bitWidth > ((short)(cachePtr->widMax >> 16) * 2) ||
		(glyphDataPtr->yMax - glyphDataPtr->yMin) > ((yMax16 << 1) - yMin16)) {
		*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) = (addEntryOffset | ERROR_ENTRY_BIT);
		sb_Debug( );		
		return ENTRY_ERROR;
	}
	return NO_ERR;
}	/* End sb_RangeCheck */

/*********************************************************************************************						
**									sb_SquishFailAdjust
**
** 	Squish Failure Adjustment
**
** 	If squishing was not enough, then clip away to
** 	keep in range
**
**	
**
*/
sb_SquishFailAdjust(sb_GlyphData *glyphDataPtr, sb_Glyph *glyphPtr, 
					short yMin16, short yMax16, Boolean *doBits, Boolean bitsCached)
{
	if (glyphDataPtr->yMax > yMax16)	glyphDataPtr->yMax = yMax16;
	if (glyphDataPtr->yMin < yMin16)	glyphDataPtr->yMin = yMin16;	
	glyphDataPtr->scan 			= glyphDataPtr->yMax - glyphDataPtr->yMin;
	glyphDataPtr->adjustTop 	= yMax16 - glyphDataPtr->yMax;
	glyphPtr->yMin				= glyphDataPtr->yMin;
	glyphPtr->yMax				= glyphDataPtr->yMax;
	glyphPtr->scan				= glyphDataPtr->scan;
	
	if (!bitsCached || glyphPtr->clipVert) {
		if (glyphPtr->topClip < glyphDataPtr->yMax)		glyphPtr->yMax = glyphPtr->topClip;
		if (glyphPtr->botClip > glyphDataPtr->yMin)		glyphPtr->yMin = glyphPtr->botClip;
		glyphPtr->scan	= glyphPtr->yMax - glyphPtr->yMin;
	
		if (glyphPtr->scan <= 0)	*doBits	= false;
	}
	return;

}	/* End sb_SquishFailAdjust */


/*********************************************************************************************						
**									sb_GrowForGlyphData
**
**	
**
*/
long sb_GrowForGlyphData(sb_SplineKeyHdl splineKey, widthTableHdl widTabHand, sb_GlyphData *glyphDataPtr,
						 long *addEntryOffset, Boolean *runInMinMode, Boolean bitsCached,
						 long *dataByteSize)
{
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;	
	widthTablePtr				widTabPtr			= *widTabHand;
	register Handle				tempHand;
	long						result;
	long						cacheSize;

	/***********************
	** CALC THE DATA SIZE
	*/
	if (bitsCached) {
		*dataByteSize = ((glyphDataPtr->scanByteWidth * glyphDataPtr->scan) + 3) & ~3;
	} else {
		sb_SetMemPtrs(widTabHand);
		fs_SizeOfOutlines( splineKeyPtr->inPtr, splineKeyPtr->outPtr );
		*dataByteSize = ((*splineKeyPtr->outPtr).outlineCacheSize + 3) & ~3;
	}

	/************************************
	** Check CACHE size - MOVES MEMORY
	*/
	cacheSize		= GetHandleSize((Handle)splineKeyPtr->cacheHand);
	if (*dataByteSize + GLYPH_HEAD_BYTES > cacheSize - *addEntryOffset) {
		THz		zone = splineKeyPtr->cacheZone;

		if (!(tempHand = sb_SizeTheBlock( splineKey, (Handle)splineKeyPtr->cacheHand, &zone, 
								*addEntryOffset + *dataByteSize + GLYPH_HEAD_BYTES + INCREASE_CACHE,
								kKeepContents, splineKeyPtr->cachePlace, noShrink))) {
			splineKeyPtr	= *splineKey;	/* Try for minimum chunk */
			tempHand = sb_SizeTheBlock( splineKey, (Handle)splineKeyPtr->cacheHand, &zone, 
												*addEntryOffset + GLYPH_HEAD_BYTES + *dataByteSize, 
												kKeepContents, splineKeyPtr->cachePlace, noShrink);
		}
		if (tempHand) {
			splineKeyPtr				= *splineKey;
			splineKeyPtr->cacheHand 	= (sb_CacheHeadHdl)tempHand;
			splineKeyPtr->cacheZone 	= zone;
		} else if (!*runInMinMode) {
			if (result = sb_RenderMemShrink(splineKey, addEntryOffset, runInMinMode))
				return result;
		} else return GROWZONE_HANDLE_ERR;
	}
	return NO_ERR;

}	/* End sb_GrowForGlyphData */


/*********************************************************************************************						
**									sb_GrowWork6and7
**
**	
**
*/
long sb_GrowWork6and7(sb_SplineKeyHdl splineKey, widthTableHdl widTabHand, 
				 	  long *addEntryOffset, Boolean *runInMinMode)
{
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;	
	widthTablePtr				widTabPtr			= *widTabHand;
	long						result, position;

	/*********************************************
	** Do the clipping dance - CAN MOVE MEMORY
	**
	** <6> new memory for the scan converter
	*/
	sb_SetMemPtrs(widTabHand);
	position	= splineKeyPtr->rowArrayOffset6 + LONG_WORD_ALIGN(splineKeyPtr->outPtr->memorySizes[6]);
	
	if (!*runInMinMode) {
		splineKeyPtr->columnArrayOffset7 = position;
		position 	+= LONG_WORD_ALIGN(splineKeyPtr->outPtr->memorySizes[7]);
	}
	
	if ((result = sb_SizeWorkHandle( position, kKeepContents, doShrink )) && !*runInMinMode)
		if (!(result = sb_RenderMemShrink(splineKey, addEntryOffset, runInMinMode)))
			result = sb_SizeWorkHandle( position, kKeepContents, doShrink );

	return result;


}	/* End sb_GrowWork6and7 */

/*********************************************************************************************						
**									sb_GetMemForBits
**
**	
**
*/
long sb_GetMemForBits(sb_SplineKeyHdl splineKey, widthTableHdl widTabHand, 
					  sb_Glyph *glyphPtr, Boolean *runInMinMode, long *addEntryOffset, 
					  short yMax16, long memForScanLine)
{
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;	
	register widthTablePtr		widTabPtr			= *widTabHand;
	register sb_CacheHeadPtr	cachePtr			= *splineKeyPtr->cacheHand;
	register sb_GlyphData		*glyphDataPtr		= (char *)cachePtr + *addEntryOffset;
	short						scanBytes			= glyphDataPtr->scanByteWidth;
	Handle						tempHand;
	long						bmSize;
	long						result, position;
	Boolean						bandGlyph			= false;

	if (glyphPtr->nextBand) {
		glyphDataPtr->yMax = glyphPtr->yMax	= glyphPtr->nextTopBand;
		if (glyphPtr->nextTopBand - glyphPtr->nextBotBand <= glyphPtr->bandScan) {
			glyphDataPtr->yMin = glyphPtr->yMin	= glyphPtr->nextBotBand;
		} else {
			glyphDataPtr->yMin = glyphPtr->yMin	= glyphPtr->yMax - glyphPtr->scan;
			glyphPtr->nextTopBand	= glyphPtr->yMin;
		}
		glyphDataPtr->scan = glyphPtr->scan	= glyphPtr->yMax - glyphPtr->yMin;
		glyphDataPtr->adjustTop = yMax16 - glyphDataPtr->yMax;
	}
	
	bmSize		= glyphPtr->scan * scanBytes;
	if (*runInMinMode && !glyphPtr->nextBand) {
		sb_SetMemPtrs(widTabHand);
		position	= splineKeyPtr->rowArrayOffset6 + LONG_WORD_ALIGN(memForScanLine*glyphPtr->scan);
		if (result = sb_SizeWorkHandle( position, kKeepContents, doShrink ))
			bandGlyph	= true;
	}
	
	tempHand		= nil;		/* init to nil */
	if (!bandGlyph || glyphPtr->nextBand) {
		if (!(tempHand = sb_GetBitMapSpace(bmSize))) {
			if (!glyphPtr->nextBand) {
				if (!*runInMinMode) {
					if (result = sb_RenderMemShrink(splineKey, addEntryOffset, runInMinMode))
						return result;
				}
				bandGlyph			= true;
			} else {
				glyphPtr->nextBand		= false;			/* second time so we are in trouble */
				sb_Debug();
				return MEM_ERR;
			}
		} else if (glyphPtr->yMin == glyphPtr->nextBotBand)
			glyphPtr->nextBand		= false;			/* finished char loop */
	}

	if (bandGlyph) {		/* could still not get the bitmap */
		if (tempHand = sb_BandChar(splineKey, &glyphPtr->scan, scanBytes, memForScanLine)) {
			splineKeyPtr			= *splineKey;
			cachePtr				= (sb_CacheHead *)*splineKeyPtr->cacheHand;
			glyphDataPtr			= (sb_GlyphData *)((char *)cachePtr + *addEntryOffset);
			
			glyphPtr->nextBand		= true;
			glyphPtr->nextBotBand	= glyphPtr->yMin;
			glyphDataPtr->yMin 		= glyphPtr->yMin = glyphPtr->yMax - glyphPtr->scan;
			glyphPtr->nextTopBand	= glyphPtr->yMin;
			glyphDataPtr->scan 		= glyphPtr->bandScan = glyphPtr->scan;
			glyphDataPtr->yMax 		= glyphPtr->yMax;
		} else {
			sb_Debug();
			return MEM_ERR;
		}
	}

	/* This is a temporary hack!!! This fixes the problem with low memory banding */
	if (glyphPtr->topClip - glyphPtr->botClip == 0 && glyphPtr->nextBand)
		glyphPtr->nextBand	= false;
	
	glyphPtr->bitMapPtr		= (uint32 *)*tempHand;
	HPurge(tempHand);
	return NO_ERR;

}	/* End sb_GetMemForBits */


/*********************************************************************************************						
**									sb_DoTheBits
**
**	
**
*/
long sb_DoTheBits(sb_SplineKeyHdl splineKey, widthTableHdl widTabHand, register sb_Glyph *glyphPtr,
				  short topClip, short bottomClip, Boolean bitsCached, Boolean  runInMinMode, 
				  long addEntryOffset, long glyphEntryOffset)									
{
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;	
	register widthTablePtr		widTabPtr			= *widTabHand;
	register sb_CacheHeadPtr	cachePtr			= *splineKeyPtr->cacheHand;
	register sb_GlyphData		*glyphDataPtr		= (char *)cachePtr + addEntryOffset;
	long						result;

	sb_SetMemPtrs(widTabHand);
	(*splineKeyPtr->inPtr).memoryBases[5]			= (char *)glyphPtr->bitMapPtr; 				/* bitmap space */
	(*splineKeyPtr->inPtr).param.scan.topClip		= topClip;
	(*splineKeyPtr->inPtr).param.scan.bottomClip	= bottomClip;
	if (runInMinMode)		(*splineKeyPtr->inPtr).memoryBases[7]		= 0;
#if TheFuture /* <18 > EBITMAP */
	if (result = fs_GetGlyphData( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {
		*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) |= ERROR_ENTRY_BIT;						
		sb_Debug();
		return result;
	}
#endif
#if !TheFuture /* <18 > EBITMAP */
	if (result = fs_ContourScan( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {
		*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) |= ERROR_ENTRY_BIT;						
		sb_Debug();
		return result;
	}
#endif
	if ((bitsCached && !runInMinMode) || !glyphPtr->clipVert) {			/* <3-CEL> */
		sb_ShrinkBitmap( glyphDataPtr, (long *)glyphPtr->bitMapPtr, topClip-bottomClip);
	}
	
	glyphPtr->byteWidth = glyphDataPtr->byteWidth;

	if (bitsCached && !runInMinMode) {									/* <3-CEL> */
		int16		sourceYMaxAdjust = glyphDataPtr->yMax - glyphPtr->topClip;

		cachePtr->addEntryOffset 	+= (((glyphDataPtr->byteWidth * glyphDataPtr->scan) + 3) & ~3);
		if (sourceYMaxAdjust > 0)
			glyphPtr->bitMapPtr = (uint32 *)((char *)glyphPtr->bitMapPtr + (sourceYMaxAdjust * glyphDataPtr->byteWidth));
	}
	return NO_ERR;

}	/* End sb_DoTheBits */

/*********************************************************************************************						
**									sb_RenderGlyph
**
**	
**
*/
static long sb_RenderGlyph( sb_SplineKeyHdl splineKey, widthTableHdl widTabHand, register sb_Glyph *glyphPtr )
{
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;	
	register widthTablePtr		widTabPtr			= *widTabHand;
	register sb_CacheHeadPtr	cachePtr			= *splineKeyPtr->cacheHand;
	Boolean						bitsCached			= cachePtr->bitsCached;
	long						addEntryOffset		= cachePtr->addEntryOffset;
	register sb_GlyphData		*glyphDataPtr		= (char *)cachePtr + addEntryOffset;
	uint16						yMinAdjust			= 0;			/* adjust for scan lines out of bounds */
	Boolean						doBits				= true;
	short						yMax16				= (cachePtr->yMax >> 16);
	short						yMin16				= (cachePtr->yMin >> 16);
	Boolean						runInMinMode		= false;			/* <3-CEL> */
	long						dataByteSize;
	long						glyphEntryOffset;															
	long						result;
	int16 						topClip, bottomClip;
	long						memForScanLine;


	glyphPtr->entryOffset				= 0;
	if (glyphPtr->nextBand) {
		glyphEntryOffset = GetSingleOrDoubleByteGlyphEntryOffset( glyphPtr->glyphID, cachePtr );		
		runInMinMode	= true;
		goto DO_NEXT_BAND;
	} else if (result = sb_RoomForHead(splineKey, &addEntryOffset, &runInMinMode))
		return result;
		
	/************************************
	** Re-Establish pointers
	*/
	splineKeyPtr	= *splineKey;
	cachePtr		= *splineKeyPtr->cacheHand;
	glyphDataPtr	= (sb_GlyphData *)((char *)cachePtr + addEntryOffset);
	glyphDataPtr->glyphID			= glyphPtr->glyphID;
	
	if (!runInMinMode) {							/* <3-CEL> */
		cachePtr->addEntryOffset 		+= GLYPH_HEAD_BYTES;			/* For next entry */
	}

	if ((result = sb_RenderGlyphInfo(splineKey, widTabHand, addEntryOffset)) == MEM_ERR) {
		if (!runInMinMode) {
			if (result = sb_RenderMemShrink(splineKey, &addEntryOffset, &runInMinMode))
				return result;
			if (result = sb_RenderGlyphInfo(splineKey, widTabHand, addEntryOffset))
				return result;
		}
	}
	
	/***********************
	** Re-establish pointers
	*/
	splineKeyPtr		= *splineKey;
	widTabPtr			= *widTabHand;
	cachePtr			= *splineKeyPtr->cacheHand;
	glyphDataPtr		= (sb_GlyphData *)((char *)cachePtr + addEntryOffset);

	/************************************
	** Determine glyph position in array
	*/

	glyphEntryOffset = GetSingleOrDoubleByteGlyphEntryOffset( glyphDataPtr->glyphID, cachePtr );		

	if (result) {
		*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) = ERROR_ENTRY_BIT;								
		return ENTRY_ERROR;
	}
	if (glyphDataPtr->scan)
		memForScanLine = LONG_WORD_ALIGN(splineKeyPtr->outPtr->memorySizes[6])/glyphDataPtr->scan;

	if (result = sb_RangeCheck(cachePtr, glyphDataPtr, yMin16, yMax16, addEntryOffset, glyphEntryOffset))	
		return result;
	
	/**********************************************
	** Figure out the bottom and top scan lines
	*/
	if (!runInMinMode) {						/* <3-CEL> */
		if (bitsCached)
			*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) = addEntryOffset;						
		else
			*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) = (addEntryOffset | CONTOUR_ENTRY_BIT);	
	}
	
	sb_SquishFailAdjust(glyphDataPtr, glyphPtr, yMin16, yMax16, &doBits, bitsCached);

	if (!glyphDataPtr->scan) {						/* <2-CEL> if no scan lines, skip char */
		*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) = ERROR_ENTRY_BIT;								
		return ENTRY_ERROR;
	}
	
	if (!runInMinMode) {
		if (result = sb_GrowForGlyphData(splineKey, widTabHand, glyphDataPtr, &addEntryOffset, 
					 					 &runInMinMode, bitsCached, &dataByteSize)) {
			sb_Debug();
			return result;
		}
	}

	if (result = sb_GrowWork6and7(splineKey, widTabHand, &addEntryOffset, &runInMinMode)) {
		sb_Debug();
		return result;
	}

	/************************************
	** Re-Establish pointers
	*/
	splineKeyPtr					= *splineKey;	
	widTabPtr						= *widTabHand;
	cachePtr						= (sb_CacheHead *)*splineKeyPtr->cacheHand;
	glyphDataPtr					= (sb_GlyphData *)((char *)cachePtr + addEntryOffset);
	sb_SetMemPtrs(widTabHand);

	/************************************
	** Make space at tail and init pointers 
	*/
	if (bitsCached) {
		glyphPtr->bitMapPtr					= (uint32 *)&glyphDataPtr->cacheData[0];
		topClip 	= glyphDataPtr->yMax;
		bottomClip 	= glyphDataPtr->yMin;
	} else if (!runInMinMode) {
		(*splineKeyPtr->inPtr).param.scan.outlineCache		= (long *)&glyphDataPtr->cacheData[0];
		if (result = fs_SaveOutlines( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {
			*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) |= ERROR_ENTRY_BIT;							
			sb_Debug();
			return result;
		}
		cachePtr->addEntryOffset		+= dataByteSize;
	}
DO_NEXT_BAND:	
	if (doBits && (runInMinMode || !bitsCached)) {			/* <3-CEL> */
		if (result = sb_GetMemForBits(splineKey, widTabHand, glyphPtr, &runInMinMode, 
									  &addEntryOffset, yMax16, memForScanLine))
			return result;
		bottomClip 				= glyphPtr->yMin;
		topClip 				= glyphPtr->yMax;
	}
	
	if (doBits || (bitsCached && !runInMinMode)) {
		if (result = sb_DoTheBits(splineKey, widTabHand, glyphPtr, topClip, bottomClip, bitsCached, 
						 runInMinMode, addEntryOffset, glyphEntryOffset))	/* 11-CEL assign result */
			return result;
	}
	glyphPtr->entryOffset	= addEntryOffset;
	if (!doBits) 	return NO_BITS;

	return NO_ERR;
}	/* sb_RenderGlyph */


/*********************************************************************************************						
**									sb_CheckCache
**
**	sb_CheckCache
**
**
*/
pascal long sb_CheckCache( sb_SplineKeyHdl splineKey, widthTableHdl widthTabHand, register sb_Glyph *glyphPtr )
{
	register sb_SplineKeyPtr	splineKeyPtr;
	register widthTablePtr		widTabPtr 			= *widthTabHand;
	register sb_CacheHead		*cachePtr;
	register sb_GlyphData		*glyphDataPtr;
	long						glyphEntryOffset;														
	register long				cacheDataOffset;
	Handle						tempHand;
	long						result;
	Boolean						runInMinMode		= false;
	
	if (glyphPtr->nextBand) 		goto MIN_MODE;
	
	widTabPtr		= *widthTabHand;
	splineKeyPtr 	= *splineKey;
	cachePtr 		= *splineKeyPtr->cacheHand;
	glyphPtr->entryOffset				= 0;

	/*********************************************/
	/** FIND THE POSITION, OFFSET, AND DATA PTR **/
	/*********************************************/

	glyphEntryOffset = GetSingleOrDoubleByteGlyphEntryOffset( glyphPtr->glyphID, cachePtr );		
	cacheDataOffset = (*(uint32*) ((Ptr) cachePtr + glyphEntryOffset)) & ENTRY_MASK;									
	
	/********************/
	/** FIND THE ENTRY **/
	/********************/
	if (cacheDataOffset) {
		long		bmSize;
		long		position;
		
		glyphDataPtr		= (sb_GlyphData *)((char *)cachePtr + cacheDataOffset);
		glyphPtr->yMin 		= glyphDataPtr->yMin;
		glyphPtr->yMax 		= glyphDataPtr->yMax;
		glyphPtr->scan		= glyphDataPtr->scan;
		glyphPtr->byteWidth = glyphDataPtr->scanByteWidth;
		if (glyphPtr->clipVert) {
			if (glyphPtr->topClip < glyphDataPtr->yMax)	 	glyphPtr->yMax = glyphPtr->topClip;
			if (glyphPtr->botClip > glyphDataPtr->yMin)	 	glyphPtr->yMin = glyphPtr->botClip;

			glyphPtr->scan	= glyphPtr->yMax - glyphPtr->yMin;
			if (glyphPtr->scan <= 0)	return CLIPPED_OUT;
		}

		sb_SetMemPtrs(widthTabHand);
		(*splineKeyPtr->inPtr).param.scan.outlineCache = (long *)&glyphDataPtr->cacheData[0];
		if (result = fs_RestoreOutlines( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {	
			sb_Debug();
			return result;
		}

		/* <6> new memory for the scan converter */
		position	= splineKeyPtr->rowArrayOffset6 + LONG_WORD_ALIGN((*splineKeyPtr->outPtr).memorySizes[6]);
		splineKeyPtr->columnArrayOffset7 = position;
		position 	+= LONG_WORD_ALIGN((*splineKeyPtr->outPtr).memorySizes[7]);

		bmSize 		= glyphPtr->scan * glyphDataPtr->scanByteWidth;

		/****************/
		/* MOVES MEMORY */
		/****************/
		if (result = sb_SizeWorkHandle( position, kKeepContents, doShrink )) {
			splineKeyPtr = *splineKey;
			if (result = sb_ShrinkCacheToMin(splineKeyPtr->cacheHand, cacheDataOffset)) {
				sb_Debug();
				return result;
			} else {
				goto MIN_MODE;			/* must rerun since data is lost */
			}
		}

		/****************/
		/* MOVES MEMORY */
		/****************/
		if (!(tempHand = sb_GetBitMapSpace(bmSize))) {
			splineKeyPtr = *splineKey;
			if (result = sb_ShrinkCacheToMin(splineKeyPtr->cacheHand, cacheDataOffset)) {
				sb_Debug();
				return result;
			} else {
				goto MIN_MODE;			/* must rerun since data is lost */
			}
		}
		HPurge(tempHand);
		/****************/

		/*************************************************/
		/* Re-establish pointers after memory allocation */
		splineKeyPtr			= *splineKey;
		widTabPtr 				= *widthTabHand;
		cachePtr 				= (sb_CacheHead *)*splineKeyPtr->cacheHand;
		glyphDataPtr			= (sb_GlyphData *)((char *)cachePtr + cacheDataOffset);
		glyphPtr->bitMapPtr		= (uint32 *)*tempHand;

		sb_SetMemPtrs(widthTabHand);
		(*splineKeyPtr->inPtr).memoryBases[5] 			= (char *)glyphPtr->bitMapPtr;
		(*splineKeyPtr->inPtr).param.scan.outlineCache	= (long *)&glyphDataPtr->cacheData[0];
		(*splineKeyPtr->inPtr).param.scan.bottomClip	= glyphPtr->yMin;
		(*splineKeyPtr->inPtr).param.scan.topClip		= glyphPtr->yMax;
#if TheFuture /* <18 > EBITMAP */
		if (result = fs_GetGlyphData( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {
			sb_Debug();
			*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) |= ERROR_ENTRY_BIT;						
			return result;
		}
#endif
#if !TheFuture /* <18 > EBITMAP */
		if (result = fs_ContourScan( splineKeyPtr->inPtr, splineKeyPtr->outPtr )) {
			sb_Debug();
			*(uint32*) ((Ptr) cachePtr + glyphEntryOffset) |= ERROR_ENTRY_BIT;						
			return result;
		}
#endif

		if (!glyphPtr->clipVert) {
			sb_ShrinkBitmap( glyphDataPtr, (long *)glyphPtr->bitMapPtr, glyphPtr->yMax-glyphPtr->yMin);
			glyphPtr->byteWidth = glyphDataPtr->byteWidth;
		}
		glyphPtr->entryOffset	= cacheDataOffset;
		return NO_ERR;
		
	} 	/* End If */
		
MIN_MODE:
	return( sb_RenderGlyph(splineKey, widthTabHand, glyphPtr) );

}	/* End sb_CheckCache */

/*********************************************************************************************						
**									sb_RetrieveGlyph
**
**	sb_RetrieveGlyph
**
**
*/
pascal long sb_RetrieveGlyph( widthTableHdl widthTabHand, sb_Glyph *glyphPtr )
{
	sb_SplineKeyHdl				splineKey			= SPLINE_KEY_HAND;
	register sb_SplineKeyPtr	splineKeyPtr		= *splineKey;
	register widthTablePtr		widTabPtr 			= *widthTabHand;
	short						fontHandState		= HGetState (widTabPtr->tabFont);
	short						cacheHandState		= HGetState ((Handle)splineKeyPtr->cacheHand);	/* 15-CEL */
	long						result;
 
#ifndef SYSTEM_7
	if (!splineKeyPtr->mfExists)			/* If it's not already running, check */
		splineKeyPtr->mfExists = MultiFinderExist();
#endif
	splineKeyPtr->glyphID	= glyphPtr->glyphID;		/* keep track in key which glyphid */
	
	HNoPurge(widTabPtr->tabFont);
	HNoPurge((Handle)splineKeyPtr->cacheHand);							/* 15-CEL */

	if ((result = sb_CheckWorkHandle()) == MEM_ERR) {
		splineKeyPtr = *splineKey;
		if (!(result = sb_ShrinkCacheToMin(splineKeyPtr->cacheHand, 0)))
			result 			= sb_CheckWorkHandle();
	}
	if (!result) {
		HNoPurge(splineKeyPtr->fsWorkHand);
		
		/* If banding in TrueType, don't AddGlyphOffset table more than once <KON Sakura2>*/
		if (!glyphPtr->nextBand)
			result = AddLowByteGlyphOffsetTable( glyphPtr->glyphID, splineKey );
			
		if ( result == noErr ) {
		
			result	= sb_CheckCache( splineKey, widthTabHand, glyphPtr );
			if ((result == noErr) ||
				(result == NO_BITS))		// can be returned by sb_RenderGlyph for some reason
											// seems to only happen printing with the StyleWriter
			{

				CheckForMaximumDiskCacheSize( (*splineKey)->cacheHand );
				
			}
			
		}
		
		HPurge((*splineKey)->fsWorkHand);
	}

	HSetState((Handle)(*splineKey)->cacheHand, cacheHandState);         /* 15-CEL */
	HSetState((*widthTabHand)->tabFont, fontHandState);
	return result;


}	/* End sb_RetrieveGlyph */

									/****************************/
									/* END CALLS TO FONT SCALER */
									/****************************/
/***************************************************************************************************/
/***************************************************************************************************/
