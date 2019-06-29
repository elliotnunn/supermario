{
Created: Tuesday, February 5, 1991 at 4:32 PM
	AIFF.p
	Pascal Interface to the Macintosh Libraries

		Copyright Apple Computer, Inc. 1990-1991
		All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT AIFF;
	INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingAIFF}
{$SETC UsingAIFF := 1}

{$I+}
{$SETC AIFFIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$SETC UsingIncludes := AIFFIncludes}

CONST
AIFFID = 'AIFF';
AIFCID = 'AIFC';
FormatVersionID = 'FVER';
CommonID = 'COMM';
FORMID = 'FORM';
SoundDataID = 'SSND';
MarkerID = 'MARK';
InstrumentID = 'INST';
MIDIDataID = 'MIDI';
AudioRecordingID = 'AESD';
ApplicationSpecificID = 'APPL';
CommentID = 'COMT';
NameID = 'NAME';
AuthorID = 'AUTH';
CopyrightID = '(c) ';
AnnotationID = 'ANNO';
NoLooping = 0;
ForwardLooping = 1;
ForwardBackwardLooping = 2;

{ AIFF-C Versions }
AIFCVersion1 = $A2805140;

{ Compression Types }
NoneName = 'pnot compressed';
ACE2to1Name = 'ACE 2-to-1';
ACE8to3Name = 'ACE 8-to-3';
MACE3to1Name = 'MACE 3-to-1';
MACE6to1Name ='MACE 6-to-1';

{ Compression Names }
NoneType = 'NONE';
ACE2Type = 'ACE2';
ACE8Type = 'ACE8';
MACE3Type = 'MAC3';
MACE6Type = 'MAC6';

TYPE
ID = LONGINT;
MarkerIdType = INTEGER;

ChunkHeader = RECORD
	ckID: ID;
	ckSize: LONGINT;
	END;

ContainerChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	formType: ID;
	END;

FormatVersionChunkPtr = ^FormatVersionChunk;
FormatVersionChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	timestamp: LONGINT;
	END;

CommonChunkPtr = ^CommonChunk;
CommonChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	numChannels: INTEGER;
	numSampleFrames: LONGINT;
	sampleSize: INTEGER;
	sampleRate: extended;
	END;

ExtCommonChunkPtr = ^ExtCommonChunk;
ExtCommonChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	numChannels: INTEGER;
	numSampleFrames: LONGINT;
	sampleSize: INTEGER;
	sampleRate: extended;
	compressionType: ID;
	compressionName: PACKED ARRAY [0..0] OF Byte;
	END;

SoundDataChunkPtr = ^SoundDataChunk;
SoundDataChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	offset: LONGINT;
	blockSize: LONGINT;
	END;

Marker = RECORD
	id: MarkerIdType;
	position: LONGINT;
	markerName: Str255;
	END;

MarkerChunkPtr = ^MarkerChunk;
MarkerChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	numMarkers: INTEGER;
	Markers: ARRAY [0..0] OF Marker;
	END;

AIFFLoop = RECORD
	playMode: INTEGER;
	beginLoop: MarkerIdType;
	endLoop: MarkerIdType;
	END;

InstrumentChunkPtr = ^InstrumentChunk;
InstrumentChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	baseFrequency: Byte;
	detune: Byte;
	lowFrequency: Byte;
	highFrequency: Byte;
	lowVelocity: Byte;
	highVelocity: Byte;
	gain: INTEGER;
	sustainLoop: AIFFLoop;
	releaseLoop: AIFFLoop;
	END;

MIDIDataChunkPtr = ^MIDIDataChunk;
MIDIDataChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	MIDIdata: ARRAY [0..0] OF SignedByte;
	END;

AudioRecordingChunkPtr = ^AudioRecordingChunk;
AudioRecordingChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	AESChannelStatus: ARRAY [0..23] OF SignedByte;
	END;

ApplicationSpecificChunkPtr = ^ApplicationSpecificChunk;
ApplicationSpecificChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	applicationSignature: OSType;
	data: ARRAY [0..0] OF Byte;
	END;

Comment = RECORD
	timeStamp: LONGINT;
	marker: MarkerIdType;
	count: INTEGER;
	text: ARRAY [0..0] OF Byte;
	END;

CommentsChunkPtr = ^CommentsChunk;
CommentsChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	numComments: INTEGER;
	comments: ARRAY [0..0] OF Comment;
	END;

TextChunkPtr = ^TextChunk;
TextChunk = RECORD
	ckID: ID;
	ckSize: LONGINT;
	text: ARRAY [0..0] OF Byte;
	END;



{$ENDC}    { UsingAIFF }

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

