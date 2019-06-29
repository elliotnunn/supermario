/*
	File:		AIFF.h

	Contains:	Definition of AIFF file format componentes

	Copyright © Apple Computer, Inc. 1989-1991 
	All rights reserved

*/

#ifndef __AIFF__
#define __AIFF__

#ifndef __TYPES__
#include <Types.h>
#endif

#define AIFFID						'AIFF'
#define	AIFCID						'AIFC'
#define FormatVersionID				'FVER'
#define CommonID					'COMM'
#define FORMID						'FORM'
#define SoundDataID					'SSND'
#define MarkerID					'MARK'
#define InstrumentID				'INST'
#define MIDIDataID					'MIDI'
#define AudioRecordingID			'AESD'
#define ApplicationSpecificID		'APPL'
#define CommentID					'COMT'
#define NameID						'NAME'
#define AuthorID					'AUTH'
#define CopyrightID					'(c) '
#define AnnotationID				'ANNO'
#define NoLooping					0
#define ForwardLooping				1
#define ForwardBackwardLooping		2

/* AIFF-C Versions */
#define AIFCVersion1				0xA2805140

/* Compression Types */
#define	NoneName					"\pnot compressed"
#define	ACE2to1Name					"\pACE 2-to-1"
#define	ACE8to3Name					"\pACE 8-to-3"
#define	MACE3to1Name				"\pMACE 3-to-1"
#define	MACE6to1Name				"\pMACE 6-to-1"

/* Compression Names */
#define	NoneType					'NONE'
#define	ACE2Type					'ACE2'
#define	ACE8Type					'ACE8'
#define	MACE3Type					'MAC3'
#define	MACE6Type					'MAC6'

typedef  unsigned long				ID; 	 
typedef	 short						MarkerIdType;

typedef struct
{
ID					ckID;
long				ckSize;
} ChunkHeader;

typedef struct
{
ID					ckID;
long				ckSize;
ID					formType;
} ContainerChunk;

typedef struct
{
ID					ckID;
long				ckSize;
unsigned long		timestamp;
} FormatVersionChunk,*FormatVersionChunkPtr;

typedef struct
{
ID					ckID;
long				ckSize;
short				numChannels;
unsigned long		numSampleFrames;
short				sampleSize;
extended			sampleRate;
} CommonChunk,*CommonChunkPtr;

typedef struct
{
ID					ckID;
long				ckSize;
short				numChannels;
unsigned long		numSampleFrames;
short				sampleSize;
extended			sampleRate;
ID					compressionType;
char				compressionName[1];
} ExtCommonChunk,*ExtCommonChunkPtr;

typedef struct
{
ID					ckID;
long				ckSize;
unsigned long		offset;
unsigned long		blockSize;
} SoundDataChunk, *SoundDataChunkPtr;

typedef struct
{
MarkerIdType			id;
unsigned long		position;
Str255				markerName;
} Marker;

typedef struct
{
ID					ckID;
long				ckSize;
unsigned short		numMarkers;
Marker				Markers[1];
} MarkerChunk, *MarkerChunkPtr;

typedef struct
{
short				playMode;
MarkerIdType		beginLoop;
MarkerIdType		endLoop;
} AIFFLoop;

typedef struct
{
ID					ckID;
long				ckSize;
char				baseFrequency;
char				detune;
char				lowFrequency;
char				highFrequency;
char				lowVelocity;
char				highVelocity;
short				gain;
AIFFLoop			sustainLoop;
AIFFLoop			releaseLoop;
} InstrumentChunk, *InstrumentChunkPtr;

typedef struct
{
ID					ckID;
long				ckSize;
unsigned char		MIDIdata[1];
} MIDIDataChunk, *MIDIDataChunkPtr;

typedef struct
{
ID					ckID;
long				ckSize;
unsigned char		AESChannelStatus[24];
} AudioRecordingChunk, *AudioRecordingChunkPtr;

typedef struct
{
ID					ckID;
long				ckSize;
OSType				applicationSignature;
char				data[1];
} ApplicationSpecificChunk, *ApplicationSpecificChunkPtr;

typedef struct
{
unsigned long		timeStamp;
MarkerIdType		marker;
unsigned short		count;
char				text[1];
} Comment;

typedef struct
{
ID					ckID;
long				ckSize;
unsigned short		numComments;
Comment				comments[1];
} CommentsChunk, *CommentsChunkPtr;

typedef struct
{
ID					ckID;
long				ckSize;
char				text[1];
} TextChunk, *TextChunkPtr;


#endif
