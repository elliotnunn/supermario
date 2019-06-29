/*
	File:		puPrivate.h

	Contains:	This is the main header file for the picture utilities package.

	Written by:	Dave Good. Some ideas stolen from Konstantin Othmer and Bruce Leak. Prototyped by Cris Rys.

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 9/25/90	DDG		Saved a few bytes by changing the IsColorPort and IsPixMap
									macros to cast everything to short before the “and” operation.
		 <8>	 9/21/90	DDG		Made changes from code review. Pulled out all the public stuff
									into the public interface file (“PictUtil.h”).
		 <7>	  8/5/90	DDG		Fixed the prototype of the RecordColors routine. Added two
									fields for checking to see if we are reading data past the end
									of a picture.
		 <6>	  8/3/90	DDG		Renumbered the verbs and added a prototype for the
									CalcExactTable routine.
		 <5>	  8/2/90	DDG		Added a field for custom color procs and added a prototype for
									the custom RecordColors routine.
		 <4>	 7/31/90	DDG		Replaced the workingCTPtr field by a buffer field
									(colorTableBuffer) and a color table entries count
									(colorTableEntries).
		 <3>	 7/30/90	DDG		Added fields to support colorizing and exact histograms.
		 <2>	 7/29/90	DDG		Fixed header.
		 <1>	 7/29/90	DDG		First checked in using new structure.

	To Do:
*/


/*----------------------------------------------------------------------------------------------------------*/

/*
|	Includes
*/

#include <types.h>
#include <memory.h>
#include <quickdraw.h>
#include <palettes.h>
#include <pictutil.h>
#include <errors.h>



/*----------------------------------------------------------------------------------------------------------*/

/*
|	Defines
*/

#define	allVerbMask					( (short) 0x001F )


#define IsColorPort(x)				( ((unsigned short)(x) & (unsigned short)0xC000) != 0 )
#define IsPixMap(x)					( ((unsigned short)(x) & (unsigned short)0x8000) != 0 )


#define CURRENT_VERSION				0


#define HistogramTableSize			(32768 * 2)
#define maximumExactColors			256


#define bufferFixedType				1
#define bufferFixedLastingType		2
#define bufferVariableType			3



/*----------------------------------------------------------------------------------------------------------*/

/*
|	Functions
*/

extern pascal OSErr __GetPictInfo( PicHandle, PictInfo *, short, short, short, short );
extern pascal OSErr __GetPixMapInfo( PixMapHandle, PictInfo *, short, short, short, short );

extern pascal OSErr __NewPictInfo( PictInfoID *, short, short, short, short );	

extern pascal OSErr __RecordPixMapInfo( PictInfoID, PixMapHandle );
extern pascal OSErr __RecordPictInfo( PictInfoID, PicHandle );
extern pascal OSErr __RetrievePictInfo( PictInfoID, PictInfo *, short );

extern pascal OSErr __DisposPictInfo( PictInfoID );



/*----------------------------------------------------------------------------------------------------------*/

/*
|	Internal Structures
*/


/*
|	This is the definition for the external function that is called to read in data
|	for the picture. The APPLICATION provides this function.
*/

typedef pascal void( *GetPicDataProcPtr )( Ptr, short );



typedef	struct Buffer {
	unsigned char		*ptr;
	unsigned long		size;
	unsigned long		ref;
} Buffer;



typedef	struct InternalPictInfo {

	PictInfo			ext;
	PictInfoID			thePictInfoID;			// for identifications/verification purposes
	short				verb;
	short				colorPickMethod;
	short				colorsRequested;
	Buffer				spoolBuffer;
	Buffer				lineBuffer;
	Buffer				bandBuffer;
	unsigned char		*spoolPtr;				// a pointer to the current offset in the spool buffer
	unsigned char		*spoolMax;				// the ending pointer for the spool buffer (start + size)
	PicHandle			spoolPictHandle;		// handle to the picture (for spooling purposes)
	unsigned long		spoolPictOffset;		// offset into the picture (for spooling purposes)
	unsigned long		spoolPictLength;		// length of the picture handle
	OSErr				spoolPictError;			// error that occurred in the spooling routines
	GetPicDataProcPtr	spoolProc;				// the APPLICATION procedure that reads the picture into the buffer
	short				colorBankType;			// if this is true then the color bank is a 555 histogram
	Buffer				colorBankBuffer;
	short				colorBankIndex;			// this is only used for custom colors.
	Buffer				colorTableBuffer;		// this contains the pre-colorized, fleshed out color table for index stuff
	short				colorTableEntries;		// number of entries in the color table buffer
	long				*indexCachePtr;
	RGBColor			foreColor;
	RGBColor			backColor;
	short				colorizing;
	long				pickProcData;

} InternalPictInfo, *InternalPictInfoPtr, **InternalPictInfoHandle;



/*----------------------------------------------------------------------------------------------------------*/

/*
|	Internal Funtions
*/

unsigned char	GetPictByte( InternalPictInfoPtr );
unsigned short	GetPictWord( InternalPictInfoPtr );
unsigned long	GetPictLong( InternalPictInfoPtr );
void			GetPictData( InternalPictInfoPtr, unsigned char *, unsigned long );
void			SkipPictData( InternalPictInfoPtr, unsigned long );
void			SkipPictByte( InternalPictInfoPtr );
void			SkipPictWord( InternalPictInfoPtr );
void			SkipPictLong( InternalPictInfoPtr );

void			AddPadBytes( unsigned char *, unsigned char *, unsigned long );
void			MergeRGBData( unsigned char *, unsigned char *, unsigned short, Boolean );
void			UnpackWords( unsigned char *, unsigned char **, unsigned short );

void			ClearMemoryBytes( char *, unsigned long );

void			NewEmptyBuffer( Buffer * );
OSErr			NewBuffer( Buffer *, unsigned long, short );
OSErr			NewBufferClear( Buffer *, unsigned long, short );
OSErr			SetBufferSize( Buffer *, unsigned long, short );
OSErr			SetBufferSizeClear( Buffer *, unsigned long, short );
OSErr			KillBuffer( Buffer * );

OSErr			RecordPixMapGuts( InternalPictInfoPtr, PixMapHandle );
void			RecordPixels( InternalPictInfoPtr, short, short, short, long, short );
void			RecordRGBColor( InternalPictInfoPtr, RGBColor * );
void			CalcExactTable( short, short, ColorSpec *, ColorSpec * );
InternalPictInfoPtr DerefPictInfoID( PictInfoID, char * );

pascal OSErr	InitPickMethod( short, long *, short *, short );
pascal OSErr	RecordColors( long, RGBColor *, long, long *, short );
pascal OSErr	CalcColorTable( long, short, char *, ColorSpec *, short );
pascal OSErr	KillPickMethod( long, short );

/*----------------------------------------------------------------------------------------------------------*/
