/*
	File:		DiskCacheMap.c

	Contains:	Code for maintaining a disk cache map.

	Written by:	John Farmer

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 4/14/92	DTY		#1027094,<BBM>: Do some stripping of pointers before doing any
									pointer arithmetic in FontNameToFontNameIndex.  Because this
									routine doesn’t move memory, it’s OK to dereference the handle
									and strip it once and use this value without having to
									constantly dereference the handle.
		 <3>	 3/27/92	DTY		#1024868: Remove calls to Assert since it doesn’t do anything
									any more.
		 <2>	  1/3/92	JSM		Roll-in changes from Sakura: Use squashed and fSquashed in
									search routines.
		 <1>	 7/17/91	jlf		Split these routines out of DiskCacheExtensions.c

*/

// Include Statements


	#include	"BaseIncludes.h"
	#include	"Assertion.h"

	#include	"Errors.h"
	#include	"Files.h"
	#include	"Fonts.h"
	#include	"StringUtility.proto"
	
	#include	"DoubleByteBassCache.h"
	#include	"FSCdefs.h"
	#include	"MapString.h"
	#include	"PartialFontExtensions.proto"
	#include	"sfnt.h"
	#include	"sfnt_enum.h"
	#include	"FontScaler.h"
	#include	"Bass_Cache.h"

	#include	"DiskCacheMap.proto"


/*	--------------------------------------------------------------------------------------

	Routine:	error = LoadDiskCacheMap( &diskCacheMapHandle, fileRefNum )
		
	Purpose:	Read the disk cache map into the system heap. The read takes place in two
				parts: the first reads in the map header; the second reads in the cache
				data.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */

OSErr LoadDiskCacheMap( DiskCacheMapStructureHandle* diskCacheMapHandlePointer, Integer fileRefNum )

{

	// Locals
	
		OSErr									error;
		THz										savedZone;
		DiskCacheMapStructureHandle				diskCacheMapHandle;
		DiskCacheMapStructure					diskCacheMapHeader;
		ParamBlockRec							parameterBlock;
		
	error = noErr;
	diskCacheMapHandle = nil;
	*diskCacheMapHandlePointer = nil;
	
	savedZone = GetZone( );
	SetZone( SystemZone( ) );
	
	parameterBlock.ioParam.ioRefNum = fileRefNum;
	parameterBlock.ioParam.ioBuffer = (Ptr) &diskCacheMapHeader;
	parameterBlock.ioParam.ioReqCount = sizeof( DiskCacheMapStructure );
	parameterBlock.ioParam.ioPosMode = fsFromStart;
	parameterBlock.ioParam.ioPosOffset = 0;
	error = PBRead( &parameterBlock, false );
	if ( error == noErr ) {
	
		if ( diskCacheMapHeader.fFileVersion != kCurrentDiskCacheFileVersion ) {
		
			error = kUnknownDiskCacheFileVersion;
		
		} else {
		
			diskCacheMapHandle = (DiskCacheMapStructureHandle) NewHandle( (Size) diskCacheMapHeader.fDiskCacheMapSize );
			if ( (error = MemError( )) == noErr ) {
			
				parameterBlock.ioParam.ioRefNum = fileRefNum;
				parameterBlock.ioParam.ioBuffer = StripAddress( (Ptr) *diskCacheMapHandle );
				parameterBlock.ioParam.ioReqCount = diskCacheMapHeader.fDiskCacheMapSize;
				parameterBlock.ioParam.ioPosMode = fsFromStart;
				parameterBlock.ioParam.ioPosOffset = 0;
				error = PBRead( &parameterBlock, false );
				if ( error == noErr ) {
				
					(*diskCacheMapHandle)->fFileRefNum = fileRefNum;
					*diskCacheMapHandlePointer = diskCacheMapHandle;
					
				}
				
			}
	
		}
		
	}
	
	if ( (error != noErr) && (diskCacheMapHandle != nil) ) {
	
		DisposHandle( (Handle) diskCacheMapHandle );
	
	}
	
	SetZone( savedZone );
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	error = DumpDiskCacheMap( diskCacheMapHandle )
		
	Purpose:	Write the disk cache map to disk.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr DumpDiskCacheMap( DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		OSErr									error;
		Integer									fileRefNum;
		DiskCacheOffset							diskCacheMapSize;
	
	error = noErr;
	
	fileRefNum = (*diskCacheMapHandle)->fFileRefNum;
	diskCacheMapSize = (*diskCacheMapHandle)->fDiskCacheMapSize;
	
	error = SetFPos( fileRefNum, fsFromStart, 0 );
	if ( error == noErr ) {
	
		error = FSWrite( fileRefNum, &diskCacheMapSize, (Ptr) *diskCacheMapHandle );
		
	}
	
	return( error );
		
}


/*	--------------------------------------------------------------------------------------

	Routine:	MapFontNameToFontNumber( &fontExists, &fontNumber, fontName )
		
	Purpose:	Map a font name to a font number. If the fontName maps to the system font
				a second check is made to see if the fontName is equal to the system font
				name. If they aren't equal substituion occured and the font doesn't exist.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void MapFontNameToFontNumber( Boolean* fontExistsPointer, Integer* fontNumberPointer, StringPtr fontName )

{

	// Locals
	
		OSErr									error;
		Str255									systemFontName;
		
	error = noErr;
	*fontExistsPointer = true;
	
	GetFNum( fontName, fontNumberPointer );
	if ( *fontNumberPointer == systemFont ) {
	
		GetFontName( systemFont, systemFontName );
		*fontExistsPointer = ComparePascalString( systemFontName, fontName );
	
	}

}


/*	--------------------------------------------------------------------------------------

	Routine:	FontNameIndexToFontName( &fontName[0], fontNameIndex, diskCacheMapHandle )
		
	Purpose:	Nasty routine to traverse a string list structure and return a copy of
				a string giving a string index.  By the way, the ugly phrase:
				
					(((stringEntryPointer->fString[0] + 1) + 1) & ~1)
					
				is used to compute the word aligned, physical length (logical length plus
				the string's length byte) of a string. My apologies in advance for it's
				extreme nastiness.
				
				The string list area is a fixed amount of space reserved at the end of
				the disk cache map.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


void FontNameIndexToFontName( StringPtr fontNamePointer, Integer fontNameIndex, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		Integer									stringIndex, roundedStringLength;
		StringListEntryStructure				*stringEntryPointer;
		StringListStructure						*stringListPointer;
		
	stringListPointer = (StringListStructure*) ((Ptr) *diskCacheMapHandle + (*diskCacheMapHandle)->fFontNameListOffset);
	stringEntryPointer = &stringListPointer->fStringList[0];
	
	for ( stringIndex = 0; stringIndex < fontNameIndex; stringIndex++ ) {
	
		roundedStringLength = (((stringEntryPointer->fString[0] + 1) + 1) & ~1);
		stringEntryPointer = (StringListEntryStructure*) ((Ptr) stringEntryPointer + sizeof( StringListEntryStructure ) + roundedStringLength);
	
	}
	
	BlockMove( (Ptr) stringEntryPointer->fString, (Ptr) fontNamePointer, stringEntryPointer->fString[0] + 1 );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	FontNameToFontNameIndex( &fontNameIndex, fontNamePointer, diskCacheMapHandle )
		
	Purpose:	Search through a nasty string list - itexists in a fixed area at the end
				of the disk cache map - looking for a font name that matches the fontNamePointer
				parameter. If the font name is found we return the list index. Otherwise,
				the string is added to the string list.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr FontNameToFontNameIndex( Integer *fontNameIndexPointer, StringPtr fontNamePointer, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		OSErr									error;
		Integer									stringIndex, stringCount, roundedStringLength,
												sizeOfNewFontNameStructure;
		DiskCacheOffset							stringListOffset;
		StringListEntryStructure				*stringEntryPointer;
		StringListStructure						*stringListPointer;
		DiskCacheMapStructurePointer			strippedDiskCachePointer = StripAddress((Ptr) *diskCacheMapHandle);		// We can dereference this once and use it because this routine doesn’t move memory.

	error = noErr;
	*fontNameIndexPointer = kFontNameNotFound;
	
	stringListPointer = (StringListStructure*) ((Ptr) strippedDiskCachePointer + strippedDiskCachePointer->fFontNameListOffset);
	stringCount = stringListPointer->fStringCount;
	stringEntryPointer = &stringListPointer->fStringList[0];
	
	for ( stringIndex = 0; (*fontNameIndexPointer == kFontNameNotFound) && (stringIndex < stringCount); stringIndex++ ) {
	
		if ( ComparePascalString( stringEntryPointer->fString, fontNamePointer ) ) {
		
			*fontNameIndexPointer = stringIndex;
			stringEntryPointer->fReferenceCount++;
			
		} else {
		
			roundedStringLength = (((stringEntryPointer->fString[0] + 1) + 1) & ~1);
			stringEntryPointer = (StringListEntryStructure*) ((Ptr) stringEntryPointer + sizeof( StringListEntryStructure ) + roundedStringLength);
	
		}
		
	}
	
	if ( *fontNameIndexPointer == kFontNameNotFound ) {
	
		roundedStringLength = (((fontNamePointer[0] + 1) + 1) & ~1);
		sizeOfNewFontNameStructure = sizeof( StringListEntryStructure ) + roundedStringLength;
		stringListOffset = (Ptr) stringEntryPointer - strippedDiskCachePointer;
		if ( stringListOffset + sizeOfNewFontNameStructure > strippedDiskCachePointer->fDiskCacheMapSize ) {
		
			error = memFullErr;
		
		} else {
		
			stringEntryPointer->fReferenceCount = 1;
			BlockMove( (Ptr) fontNamePointer, (Ptr) stringEntryPointer->fString, fontNamePointer[0] + 1 );
			*fontNameIndexPointer = stringListPointer->fStringCount++;
			
		}
		
	}
	
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	doesMatch = CompareDiskCacheEntry( searchTemplate, cacheIndex, diskCacheMapHandle )
		
	Purpose:	Compare a search template against a cacheEntry specified by cacheIndex.
				Return true if they match. The fontName index field of the searchTemplate contains
				the fontID to be compared. Since the disk cache stores font names to avoid
				font renumbering madness, we use the fontID to get the font name and compare
				that to the cache's font name. If the font doesn't exist we have severe problems
				and return immediately.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


Boolean CompareDiskCacheEntry( DiskCacheEntryStructure searchTemplate, DiskCacheIndex cacheIndex, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		Boolean									found, fontExists;
		Integer									diskCacheFontNumber;
		DiskCacheEntryStructurePointer			diskCacheEntryPointer;
		Str255									diskCacheFontName, searchFontName;
		
	found = false;
		
	FontNameIndexToFontName( diskCacheFontName, (*diskCacheMapHandle)->fDiskCache[cacheIndex].fFontNameIndex, diskCacheMapHandle );
	MapFontNameToFontNumber( &fontExists, &diskCacheFontNumber, diskCacheFontName );
	if ( fontExists ) {
	
		GetFontName( searchTemplate.fFontNameIndex, searchFontName );
		diskCacheEntryPointer = &(*diskCacheMapHandle)->fDiskCache[cacheIndex];
		
		found = ComparePascalString( diskCacheFontName, searchFontName ) &&
				(diskCacheEntryPointer->fPointSize == searchTemplate.fPointSize) &&
				(diskCacheEntryPointer->fStyle == searchTemplate.fStyle) &&
				(diskCacheEntryPointer->fHorizontalScale == searchTemplate.fHorizontalScale) &&
				(diskCacheEntryPointer->fVerticalScale == searchTemplate.fVerticalScale) &&
				(diskCacheEntryPointer->fSkewTransform == searchTemplate.fSkewTransform) &&
				(diskCacheEntryPointer->fSquashed == searchTemplate.fSquashed);
				
	}
	
	return( found );

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = SearchForDiskCache( &cacheIndex, searchTemplate, diskCacheMapHandle )
		
	Purpose:	Cycle through the disk cache comparing searchTemplate to each entry until
				a match is found. Return a valid cacheIndex if found or kDiskCacheNotFound
				if not found. This routine cycles through the offset list although the date
				list would also work.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr SearchForDiskCache( DiskCacheIndex* cacheIndexPointer, DiskCacheEntryStructure searchTemplate, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		OSErr									error;
		DiskCacheIndex							cacheIndex;
		
	error = noErr;
	*cacheIndexPointer = kDiskCacheNotFound;
	cacheIndex = (*diskCacheMapHandle)->fFirstOffsetIndex;
	
	while ( (*cacheIndexPointer == kDiskCacheNotFound) && (cacheIndex != kDiskCacheNotFound)) {
	
		if ( CompareDiskCacheEntry( searchTemplate, cacheIndex, diskCacheMapHandle ) ) {
		
			*cacheIndexPointer = cacheIndex;
		
		} else {
		
			cacheIndex = (*diskCacheMapHandle)->fDiskCache[cacheIndex].fNextOffsetIndex;
		
		}
		
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = SearchForDiskCacheUsingWidthTable( &cacheIndex, widthTableHandle, diskCacheMapHandle )
		
	Purpose:	Setup a search template based on information from the widthTableHandle parameter
				and call SearchForDiskCache.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr SearchForDiskCacheUsingWidthTable( DiskCacheIndex* cacheIndexPointer, widthTableHdl widthTableHandle,
		DiskCacheMapStructureHandle diskCacheMapHandle, Boolean squashed )

{

	// Locals
	
		widthTablePtr							widthTablePointer;
		DiskCacheEntryStructure					searchTemplate;
		
	widthTablePointer = *widthTableHandle;
	searchTemplate.fFontNameIndex = widthTablePointer->fID;
	searchTemplate.fPointSize = widthTablePointer->fSize;
	searchTemplate.fStyle = IntrinsicStyle( *widthTableHandle );
	searchTemplate.fHorizontalScale = widthTablePointer->trans00;
	searchTemplate.fVerticalScale = widthTablePointer->trans11;
	searchTemplate.fSkewTransform = widthTablePointer->trans10;
	searchTemplate.fSquashed = squashed;
	return( SearchForDiskCache( cacheIndexPointer, searchTemplate, diskCacheMapHandle ) );

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = SearchForDiskCacheUsingCache( &cacheIndex, cacheHandle, diskCacheMapHandle )
		
	Purpose:	Setup a search template based on information from the cacheHandle parameter
				and call SearchForDiskCache.
				
	Warnings:	None
	
	-----------------------------------------------------------------------------------  */


OSErr SearchForDiskCacheUsingCache( DiskCacheIndex* cacheIndexPointer, sb_CacheHeadHdl cacheHandle, DiskCacheMapStructureHandle diskCacheMapHandle )

{

	// Locals
	
		sb_CacheHeadPtr							cachePointer;
		DiskCacheEntryStructure					searchTemplate;
		
	cachePointer = *cacheHandle;
	searchTemplate.fFontNameIndex = cachePointer->familyID;
	searchTemplate.fPointSize = cachePointer->ptSize;
	searchTemplate.fStyle = cachePointer->face;
	searchTemplate.fHorizontalScale = cachePointer->trans00;
	searchTemplate.fVerticalScale = cachePointer->trans11;
	searchTemplate.fSkewTransform = cachePointer->trans10;
	searchTemplate.fSquashed = cachePointer->squashed;;
	return( SearchForDiskCache( cacheIndexPointer, searchTemplate, diskCacheMapHandle ) );

}