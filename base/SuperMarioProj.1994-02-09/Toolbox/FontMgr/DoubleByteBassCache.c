/*
	File:		DoubleByteBassCache.c

	Contains:	Support routines for double byte modifications to the TrueType cache.

	Written by:	John Farmer

	Copyright:	© 1990-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	  1/3/92	JSM		Roll-in changes from Sakura: When shrinking cache, add Glyph
									header size to new cache size. Fix
									AllocateEntrySpaceInCurrentCache() so heap isn’t trashed.

		System 6.1.0 Changes:
		
		 <2>	 7/01/91	jlf		Removed incorrect header documentation.
		 <1>	 1/09/90	jlf		Created file.
*/

// Include Statements


	#include	"Errors.h"
	#include	"Fonts.h"
	#include	"Memory.h"
	#include	"Script.h"
	#include	"Types.h"
	
	#include	"FSCdefs.h"
	#include	"FontScaler.h"
	#include	"PartialFontExtensions.h"
	#include	"Bass_Cache.h"

	#include	"DoubleByteBassCache.h"
		

/*	--------------------------------------------------------------------------------------

	Routine:	error = AllocateEntrySpaceInCurrentCache( entrySize, splineKey )
		
	Purpose:	
				
	Warnings:	
	
	-----------------------------------------------------------------------------------  */


OSErr AllocateEntrySpaceInCurrentCache( Size entrySize, sb_SplineKeyHdl splineKey )

{

	// Locals
	
		OSErr								error;
		uint16								cachePlace;
		Size								oldCacheSize, newCacheSize;
		sb_CacheHeadHdl						cacheHandle, newCacheHandle;
		THz									cacheZone;
		
	error = noErr;
	
	cacheHandle = (*splineKey)->cacheHand;
	oldCacheSize = GetHandleSize( (Handle) cacheHandle );
	
	// add GLYPH_HEAD_BYTES so heap isn't trashed
	newCacheSize = (*cacheHandle)->addEntryOffset + entrySize + GLYPH_HEAD_BYTES;
	
	if ( newCacheSize > oldCacheSize ) {
	
		cacheZone = (*splineKey)->cacheZone;
		cachePlace = (*splineKey)->cachePlace;
		newCacheHandle = (sb_CacheHeadHdl) sb_SizeTheBlock( splineKey, (Handle) cacheHandle, &cacheZone, newCacheSize, kKeepContents, cachePlace, noShrink );
		
		if ( newCacheHandle != nil ) {
		
			(*splineKey)->cacheHand = newCacheHandle;
			(*splineKey)->cacheZone = cacheZone;
		
		} else {
		
			error = sb_ShrinkCacheToMin( cacheHandle, 0 );
			
			if ( error == noErr ) {
				
				newCacheSize = (*cacheHandle)->addEntryOffset + entrySize + GLYPH_HEAD_BYTES;
				newCacheHandle = (sb_CacheHeadHdl) sb_SizeTheBlock( splineKey, (Handle) cacheHandle, &cacheZone, newCacheSize, kKeepContents, cachePlace, noShrink );
				
				if ( newCacheHandle == nil ) {
				
					error = memFullErr;
					
				} else {
				
					(*splineKey)->cacheHand = newCacheHandle;
					(*splineKey)->cacheZone = cacheZone;
	
				}
				
			}
		
		}
		
	}
	
	return( error );

}


/*	--------------------------------------------------------------------------------------

	Routine:	error = AddLowByteGlyphOffsetTable( glyphID, splineKey )
		
	Purpose:	
				
	Warnings:	change entry in sb_glyph.
	
	-----------------------------------------------------------------------------------  */


OSErr AddLowByteGlyphOffsetTable( uint16 glyphID, sb_SplineKeyHdl splineKey )

{

	// Locals

		unsigned char						lowByte, highByte;
		uint16								lowByteOffsetIndex;
		uint32								*lowByteGlyphOffsetTablePointer;
		OSErr								error;	
		EncodingTablePointer				encodingPointer;
		sb_CacheHeadHdl						cacheHandle;
		
	error = noErr;
	highByte = (glyphID >> 8);
	lowByte = (glyphID & 0x00FF);
	cacheHandle = (*splineKey)->cacheHand;
	encodingPointer = (EncodingTablePointer) (*cacheHandle)->fEncodingTablePointer;
	
	if ( (encodingPointer != nil) && (lowByte != 0) ) {
			
		if ( (encodingPointer->fHighByte[highByte] != kSingleByte) && ((*cacheHandle)->glyphArray[highByte] == 0) ) {
		
			error = AllocateEntrySpaceInCurrentCache( kLowByteGlyphOffsetTableSize, splineKey );
			if ( error == noErr ) {
			
				cacheHandle = (*splineKey)->cacheHand;
				lowByteGlyphOffsetTablePointer = (uint32*) ((Ptr) *cacheHandle + (*cacheHandle)->addEntryOffset);
				
				for ( lowByteOffsetIndex = 0; lowByteOffsetIndex <= 255; lowByteOffsetIndex++ )
					lowByteGlyphOffsetTablePointer[lowByteOffsetIndex] = 0;
	
				(*cacheHandle)->glyphArray[highByte] = (*cacheHandle)->addEntryOffset;
				(*cacheHandle)->addEntryOffset += kLowByteGlyphOffsetTableSize;
				
			}
			
		}
	
	}
	
	return( error );
	
}


/*	--------------------------------------------------------------------------------------

	Routine:	cacheOffset = GetSingleOrDoubleByteGlyphEntryOffset( glyphID, cacheHandle )
		
	Purpose:	
				
	Warnings:	
	
	-----------------------------------------------------------------------------------  */


uint32 GetSingleOrDoubleByteGlyphEntryOffset( uint16 glyphID, sb_CacheHeadPtr cachePointer )

{

	// Locals
	
		unsigned char						highByte, lowByte;
		uint32								lowByteTableOffset, *glyphEntryPointer;
		EncodingTablePointer				encodingPointer;
		
	highByte = (glyphID >> 8);
	lowByte = (glyphID & 0x00FF);		
	glyphEntryPointer = &cachePointer->glyphArray[0];
	encodingPointer = (EncodingTablePointer) cachePointer->fEncodingTablePointer;

	if ( encodingPointer == nil ) {
	
		glyphEntryPointer = &cachePointer->glyphArray[lowByte];
	
	} else if ( lowByte == 0 ) {
	
		if ( encodingPointer->fHighByte[highByte] == kSingleByte ) {
		
			glyphEntryPointer = &cachePointer->glyphArray[highByte];		
	
		}
		
	} else {
	
		if ( (encodingPointer->fHighByte[highByte] != kSingleByte) && (encodingPointer->fLowByte[lowByte] != kSingleByte) ) {
		
			lowByteTableOffset = cachePointer->glyphArray[highByte] & ENTRY_MASK;
			if ( lowByteTableOffset != 0 ) {

				glyphEntryPointer = (uint32*) ((Ptr) cachePointer + lowByteTableOffset) + lowByte;
			
			}
	
		}
		
	}
	
	return( (Ptr) glyphEntryPointer - (Ptr) cachePointer );

}