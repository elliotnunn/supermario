/*
	File:		DiskCacheExtensions.h

	Contains:	Header file information for the file DiskCacheExtensions.c

	Written by:	John Farmer

	Copyright:	© 1990-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 3/16/92	YK		Conditionalize some lines by ‘rez’ variable since DoubleTalk.r
									now needs to include this file. System build will not be
									affected by this change.
		 <3>	  1/8/92	JSM		Add some comments, remove kDiskCacheFileName,
									kDiskCacheFileCreator, kDiskCacheFileType, and
									kDefaultDiskCacheFileSize which are only used by the disk cache
									builder.
		 <2>	  1/3/92	JSM		Roll-in changes from Sakura: Add kDiskCacheFileNameResID. Add
									fMapDirty and fMapRebuildCount fields to DiskCacheMapStructure.
									Move default sizes from the cache builder project here so we can
									create a clean map. Add kMinimumDiskCacheBuffer for
									AllocateLockedBufferForCompression. Add fSquashed to
									DiskCacheEntryStructure.
		 <1>	 6/29/91	jlf		Created file.
*/

#ifndef			__DiskBasedCache__
#define			__DiskBasedCache__

// Constants

	#define	kUnknownDiskCacheFileVersion		(-1)
	#define	kCacheNotFound						(-1)
	#define	kDiskCacheNotFound					(-1)
	#define	kFontNameNotFound					(-1)
	#define	kShrinkFailedError					(-1)
	#define	kCurrentDiskCacheFileVersion		(1)
	#define kMapDirty							(true)
	
	// we get the name of the font cache file from this STR resource
	#define	kDiskCacheFileNameResID				-16391
	
	// largest size for one cache on disk
	#define	kMaximumDiskCacheSize				(300*1024)
	
	// minimum buffer that needs to be allocated when compacting disk cache
	#define kMinimumDiskCacheBufferSize			(16*1024)

// Defaults

	// max number of caches in disk cache
	#define	kMaximumDiskCacheCount				128
	
	// size of array of font names (StringListStructure) at end of caches
	#define	kFontNameListSize					(5*1024)

#ifndef	rez
// Type definitions


	typedef	Integer								DiskCacheIndex;
	typedef	LongInteger							DiskCacheOffset;


// Structures


	typedef struct {
	
		Integer									fReferenceCount;
		char									fString[];
	
	} StringListEntryStructure;
	
	
	typedef struct {
	
		Integer									fStringCount;
		StringListEntryStructure				fStringList[];
	
	} StringListStructure;
	
	
	typedef struct {
	
		Integer									fFontNameIndex;
		Integer									fPointSize;
		Integer									fStyle;
		Fixed									fHorizontalScale;
		Fixed									fVerticalScale;
		Fixed									fSkewTransform;
		Boolean									fSquashed;
		
		DiskCacheOffset							fLogicalOffset;
		DiskCacheOffset							fLogicalSize;

		DiskCacheIndex							fPreviousOffsetIndex;
		DiskCacheIndex							fNextOffsetIndex;
		DiskCacheIndex							fPreviousDateIndex;
		DiskCacheIndex							fNextDateIndex;
		
		Boolean									fCacheIsLocked;
		Boolean									fCacheIsMaximumSize;
		Boolean									fCacheIsFree;
		
	} DiskCacheEntryStructure, *DiskCacheEntryStructurePointer;
	
	
	typedef struct {
	
		Integer									fFileVersion;
		Integer									fFileRefNum;
		
		Boolean									fMapDirty;
		LongInteger								fMapRebuildCount;
		
		Boolean									fFontCacheIsOn;
		
		DiskCacheIndex							fFirstOffsetIndex;
		DiskCacheIndex							fLastOffsetIndex;
		DiskCacheIndex							fFirstDateIndex;
		DiskCacheIndex							fLastDateIndex;
		
		DiskCacheOffset							fFontNameListOffset;
		DiskCacheOffset							fDiskCacheMapSize;
		DiskCacheIndex							fMaximumDiskCacheCount;
		DiskCacheIndex							fDiskCacheCount;
		DiskCacheEntryStructure					fDiskCache[];
		
		// StringList data follows array of disk caches
		
	} DiskCacheMapStructure, *DiskCacheMapStructurePointer, **DiskCacheMapStructureHandle;
		
#endif rez

#endif			__DiskBasedCache__
