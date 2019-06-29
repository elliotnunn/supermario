{
Created: Tuesday, March 5, 1991 at 5:12 PM
    Sound.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1986-1991
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Sound;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingSound}
{$SETC UsingSound := 1}

{$I+}
{$SETC SoundIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingFiles}
{$I $$Shell(PInterfaces)Files.p}
{$ENDC}
{$SETC UsingIncludes := SoundIncludes}

CONST
swMode = -1;                                    { Sound Driver modes }
ftMode = 1;
ffMode = 0;

synthCodeRsrc = 'snth';                         { Resource types used by Sound Manager }
soundListRsrc = 'snd ';

twelfthRootTwo = 1.05946309434;
rate22khz = $56EE8BA3;                          { 22254.54545 in fixed-point }
rate11khz = $2B7745D1;                          { 11127.27273 in fixed-point }

{ synthesizer numbers for SndNewChannel }
squareWaveSynth = 1;                            {square wave synthesizer}
waveTableSynth = 3;                             {wave table synthesizer}
sampledSynth = 5;                               {sampled sound synthesizer}

{ old Sound Manager MACE synthesizer numbers }
MACE3snthID = 11;
MACE6snthID = 13;

{ command numbers for SndDoCommand and SndDoImmediate }
nullCmd = 0;
initCmd = 1;
freeCmd = 2;
quietCmd = 3;
flushCmd = 4;
reInitCmd = 5;

waitCmd = 10;
pauseCmd = 11;
resumeCmd = 12;
callBackCmd = 13;
syncCmd = 14;
emptyCmd = 15;

tickleCmd = 20;
requestNextCmd = 21;
howOftenCmd = 22;
wakeUpCmd = 23;
availableCmd = 24;
versionCmd = 25;
totalLoadCmd = 26;
loadCmd = 27;

scaleCmd = 30;
tempoCmd = 31;

freqDurationCmd = 40;
restCmd = 41;
freqCmd = 42;
ampCmd = 43;
timbreCmd = 44;
getAmpCmd = 45;

waveTableCmd = 60;
phaseCmd = 61;

soundCmd = 80;
bufferCmd = 81;
rateCmd = 82;
continueCmd = 83;
doubleBufferCmd = 84;
getRateCmd = 85;

sizeCmd = 90;
convertCmd = 91;

stdQLength = 128;
dataOffsetFlag = $8000;

waveInitChannelMask = $07;
waveInitChannel0 = $04;
waveInitChannel1 = $05;
waveInitChannel2 = $06;
waveInitChannel3 = $07;

{ channel initialization parameters }
initPanMask = $0003;                            { mask for right/left pan values }
initSRateMask = $0030;                          { mask for sample rate values }
initStereoMask = $00C0;                         { mask for mono/stereo values }
initCompMask = $FF00;                           { mask for compression IDs }

initChanLeft = $0002;                           { left stereo channel }
initChanRight = $0003;                          { right stereo channel }
initNoInterp = $0004;                           { no linear interpolation }
initNoDrop = $0008;                             { no drop-sample conversion }
initMono = $0080;                               { monophonic channel }
initStereo = $00C0;                             { stereo channel }
initMACE3 = $0300;                              { MACE 3:1 }
initMACE6 = $0400;                              { MACE 6:1 }

initChan0 = $0004;                              { channel 0 - wave table only }
initChan1 = $0005;                              { channel 1 - wave table only }
initChan2 = $0006;                              { channel 2 - wave table only }
initChan3 = $0007;                              { channel 3 - wave table only }

stdSH = $00;                                    { Standard sound header encode value }
extSH = $FF;                                    { Extended sound header encode value }
cmpSH = $FE;                                    { Compressed sound header encode value }

notCompressed = 0;                              { compression ID's }
twoToOne = 1;
eightToThree = 2;
threeToOne = 3;
sixToOne = 4;

outsideCmpSH = 0;                               { MACE constants }
insideCmpSH = 1;
aceSuccess = 0;
aceMemFull = 1;
aceNilBlock = 2;
aceBadComp = 3;
aceBadEncode = 4;
aceBadDest = 5;
aceBadCmd = 6;
sixToOnePacketSize = 8;
threeToOnePacketSize = 16;
stateBlockSize = 64;
leftOverBlockSize = 32;

firstSoundFormat = $0001;                       { general sound format }
secondSoundFormat = $0002;                      { special sampled sound format (HyperCard) }

dbBufferReady = $00000001;                      { double buffer is filled }
dbLastBuffer = $00000004;                       { last double buffer to play }

sysBeepDisable = $0000;                         { SysBeep() enable flags }
sysBeepEnable = $0001;

unitTypeNoSelection = $FFFF;                    { unitTypes for AudioSelection.unitType }
unitTypeSeconds = $0000;

TYPE
{            Structures for Sound Driver            }


FreeWave = PACKED ARRAY [0..30000] OF Byte;

FFSynthPtr = ^FFSynthRec;
FFSynthRec = RECORD
    mode: INTEGER;
    count: Fixed;
    waveBytes: FreeWave;
    END;

Tone = RECORD
    count: INTEGER;
    amplitude: INTEGER;
    duration: INTEGER;
    END;


Tones = ARRAY [0..5000] OF Tone;

SWSynthPtr = ^SWSynthRec;
SWSynthRec = RECORD
    mode: INTEGER;
    triplets: Tones;
    END;


Wave = PACKED ARRAY [0..255] OF Byte;
WavePtr = ^Wave;

FTSndRecPtr = ^FTSoundRec;
FTSoundRec = RECORD
    duration: INTEGER;
    sound1Rate: Fixed;
    sound1Phase: LONGINT;
    sound2Rate: Fixed;
    sound2Phase: LONGINT;
    sound3Rate: Fixed;
    sound3Phase: LONGINT;
    sound4Rate: Fixed;
    sound4Phase: LONGINT;
    sound1Wave: WavePtr;
    sound2Wave: WavePtr;
    sound3Wave: WavePtr;
    sound4Wave: WavePtr;
    END;

FTSynthPtr = ^FTSynthRec;
FTSynthRec = RECORD
    mode: INTEGER;
    sndRec: FTSndRecPtr;
    END;

{            Structures for Sound Manager            }

SndCommand = PACKED RECORD
    cmd: INTEGER;
    param1: INTEGER;
    param2: LONGINT;
    END;


Time = LONGINT;                                 { in half milliseconds }



SndChannelPtr = ^SndChannel;
SndChannel = PACKED RECORD
    nextChan: SndChannelPtr;
    firstMod: Ptr;                              { reserved for the Sound Manager }
    callBack: ProcPtr;
    userInfo: LONGINT;
    wait: Time;                                 { The following is for internal Sound Manager use only.}
    cmdInProgress: SndCommand;
    flags: INTEGER;
    qLength: INTEGER;
    qHead: INTEGER;                             { next spot to read or -1 if empty }
    qTail: INTEGER;                             { next spot to write = qHead if full }
    queue: ARRAY [0..stdQLength - 1] OF SndCommand;
    END;

{ MACE structures }
StateBlockPtr = ^StateBlock;
StateBlock = RECORD
    stateVar: ARRAY [0..stateBlockSize - 1] OF INTEGER;
    END;

LeftOverBlockPtr = ^LeftOverBlock;
LeftOverBlock = RECORD
    count: LONGINT;
    sampleArea: PACKED ARRAY [0..leftOverBlockSize - 1] OF Byte;
    END;

ModRef = RECORD
    modNumber: INTEGER;
    modInit: LONGINT;
    END;

SndListPtr = ^SndListResource;
SndListResource = RECORD
    format: INTEGER;
    numModifiers: INTEGER;
    modifierPart: ARRAY [0..0] OF ModRef;       {This is a variable length array}
    numCommands: INTEGER;
    commandPart: ARRAY [0..0] OF SndCommand;    {This is a variable length array}
    dataPart: PACKED ARRAY [0..0] OF Byte;      {This is a variable length array}
    END;

SoundHeaderPtr = ^SoundHeader;
SoundHeader = PACKED RECORD
    samplePtr: Ptr;                             { if NIL then samples are in sampleArea }
    length: LONGINT;                            { length of sound in bytes }
    sampleRate: Fixed;                          { sample rate for this sound }
    loopStart: LONGINT;                         { start of looping portion }
    loopEnd: LONGINT;                           { end of looping portion }
    encode: Byte;                               { header encoding }
    baseFrequency: Byte;                        { baseFrequency value }
    sampleArea: PACKED ARRAY [0..0] OF Byte;
    END;


CmpSoundHeaderPtr = ^CmpSoundHeader;
CmpSoundHeader = PACKED RECORD
    samplePtr: Ptr;                             { if nil then samples are in sample area }
    numChannels: LONGINT;                       { number of channels i.e. mono = 1 }
    sampleRate: Fixed;                          { sample rate in Apples Fixed point representation }
    loopStart: LONGINT;                         { loopStart of sound before compression }
    loopEnd: LONGINT;                           { loopEnd of sound before compression }
    encode: Byte;                               { data structure used , stdSH, extSH, or cmpSH }
    baseFrequency: Byte;                        { same meaning as regular SoundHeader }
    numFrames: LONGINT;                         { length in frames ( packetFrames or sampleFrames ) }
    AIFFSampleRate: extended;                   { IEEE sample rate }
    markerChunk: Ptr;                           { sync track }
    futureUse1: Ptr;                            { reserved by Apple }
    futureUse2: Ptr;                            { reserved by Apple }
    stateVars: StateBlockPtr;                   { pointer to State Block }
    leftOverSamples: LeftOverBlockPtr;          { used to save truncated samples between compression calls }
    compressionID: INTEGER;                     { 0 means no compression, non zero means compressionID }
    packetSize: INTEGER;                        { number of bits in compressed sample packet }
    snthID: INTEGER;                            { resource ID of Sound Manager snth that contains NRT C/E }
    sampleSize: INTEGER;                        { number of bits in non-compressed sample }
    sampleArea: PACKED ARRAY [0..0] OF Byte;    { space for when samples follow directly }
    END;

ExtSoundHeaderPtr = ^ExtSoundHeader;
ExtSoundHeader = PACKED RECORD
    samplePtr: Ptr;                             { if nil then samples are in sample area }
    numChannels: LONGINT;                       { number of channels,  ie mono = 1 }
    sampleRate: Fixed;                          { sample rate in Apples Fixed point representation }
    loopStart: LONGINT;                         { same meaning as regular SoundHeader }
    loopEnd: LONGINT;                           { same meaning as regular SoundHeader }
    encode: Byte;                               { data structure used , stdSH, extSH, or cmpSH }
    baseFrequency: Byte;                        { same meaning as regular SoundHeader }
    numFrames: LONGINT;                         { length in total number of frames }
    AIFFSampleRate: extended;                   { IEEE sample rate }
    markerChunk: Ptr;                           { sync track }
    instrumentChunks: Ptr;                      { AIFF instrument chunks }
    AESRecording: Ptr;
    sampleSize: INTEGER;                        { number of bits in sample }
    futureUse1: INTEGER;                        { reserved by Apple }
    futureUse2: LONGINT;                        { reserved by Apple }
    futureUse3: LONGINT;                        { reserved by Apple }
    futureUse4: LONGINT;                        { reserved by Apple }
    sampleArea: PACKED ARRAY [0..0] OF Byte;    { space for when samples follow directly }
    END;

ConversionBlockPtr = ^ConversionBlock;
ConversionBlock = RECORD
    destination: INTEGER;
    unused: INTEGER;
    inputPtr: CmpSoundHeaderPtr;
    outputPtr: CmpSoundHeaderPtr;
    END;

SMStatusPtr = ^SMStatus;
SMStatus = PACKED RECORD
    smMaxCPULoad: INTEGER;
    smNumChannels: INTEGER;
    smCurCPULoad: INTEGER;
    END;

SCStatusPtr = ^SCStatus;
SCStatus = RECORD
    scStartTime: Fixed;
    scEndTime: Fixed;
    scCurrentTime: Fixed;
    scChannelBusy: BOOLEAN;
    scChannelDisposed: BOOLEAN;
    scChannelPaused: BOOLEAN;
    scUnused: BOOLEAN;
    scChannelAttributes: LONGINT;
    scCPULoad: LONGINT;
    END;

AudioSelectionPtr = ^AudioSelection;
AudioSelection = PACKED RECORD
    unitType: LONGINT;
    selStart: Fixed;
    selEnd: Fixed;
    END;

SndDoubleBufferPtr = ^SndDoubleBuffer;
SndDoubleBuffer = PACKED RECORD
    dbNumFrames: LONGINT;
    dbFlags: LONGINT;
    dbUserInfo: ARRAY [0..1] OF LONGINT;
    dbSoundData: PACKED ARRAY [0..0] OF Byte;
    END;

SndDoubleBufferHeaderPtr = ^SndDoubleBufferHeader;
SndDoubleBufferHeader = PACKED RECORD
    dbhNumChannels: INTEGER;
    dbhSampleSize: INTEGER;
    dbhCompressionID: INTEGER;
    dbhPacketSize: INTEGER;
    dbhSampleRate: Fixed;
    dbhBufferPtr: ARRAY [0..1] OF SndDoubleBufferPtr;
    dbhDoubleBack: ProcPtr;
    END;


FUNCTION SndDoCommand(chan: SndChannelPtr;cmd: SndCommand;noWait: BOOLEAN): OSErr;
    INLINE $A803;
FUNCTION SndDoImmediate(chan: SndChannelPtr;cmd: SndCommand): OSErr;
    INLINE $A804;
FUNCTION SndNewChannel(VAR chan: SndChannelPtr;synth: INTEGER;init: LONGINT;
    userRoutine: ProcPtr): OSErr;
    INLINE $A807;
FUNCTION SndDisposeChannel(chan: SndChannelPtr;quietNow: BOOLEAN): OSErr;
    INLINE $A801;
FUNCTION SndPlay(chan: SndChannelPtr;sndHdl: Handle;async: BOOLEAN): OSErr;
    INLINE $A805;
FUNCTION SndAddModifier(chan: SndChannelPtr;modifier: ProcPtr;id: INTEGER;
    init: LONGINT): OSErr;
    INLINE $A802;
FUNCTION SndControl(id: INTEGER;VAR cmd: SndCommand): OSErr;
    INLINE $A806;

PROCEDURE SetSoundVol(level: INTEGER);
PROCEDURE GetSoundVol(VAR level: INTEGER);
PROCEDURE StartSound(synthRec: Ptr;numBytes: LONGINT;completionRtn: ProcPtr);
PROCEDURE StopSound;
FUNCTION SoundDone: BOOLEAN;

FUNCTION SndSoundManagerVersion: NumVersion;
    INLINE $203C,$000C,$0008,$A800;
FUNCTION SndStartFilePlay(chan: SndChannelPtr;fRefNum: INTEGER;resNum: INTEGER;
    bufferSize: LONGINT;theBuffer: Ptr;theSelection: AudioSelectionPtr;theCompletion: ProcPtr;
    async: BOOLEAN): OSErr;
    INLINE $203C,$0D00,$0008,$A800;
FUNCTION SndPauseFilePlay(chan: SndChannelPtr): OSErr;
    INLINE $203C,$0204,$0008,$A800;
FUNCTION SndStopFilePlay(chan: SndChannelPtr;async: BOOLEAN): OSErr;
    INLINE $203C,$0308,$0008,$A800;
FUNCTION SndChannelStatus(chan: SndChannelPtr;theLength: INTEGER;theStatus: SCStatusPtr): OSErr;
    INLINE $203C,$0010,$0008,$A800;
FUNCTION SndManagerStatus(theLength: INTEGER;theStatus: SMStatusPtr): OSErr;
    INLINE $203C,$0014,$0008,$A800;
PROCEDURE SndGetSysBeepState(VAR sysBeepState: INTEGER);
    INLINE $203C,$0018,$0008,$A800;
FUNCTION SndSetSysBeepState(sysBeepState: INTEGER): OSErr;
    INLINE $203C,$001C,$0008,$A800;
FUNCTION SndPlayDoubleBuffer(chan: SndChannelPtr;theParams: SndDoubleBufferHeaderPtr): OSErr;
    INLINE $203C,$0020,$0008,$A800;

FUNCTION MACEVersion: NumVersion;
    INLINE $203C,$0000,$0010,$A800;
PROCEDURE Comp3to1(inBuffer: Ptr;outBuffer: Ptr;cnt: LONGINT;inState: Ptr;
    outState: Ptr;numChannels: LONGINT;whichChannel: LONGINT);
    INLINE $203C,$0004,$0010,$A800;
PROCEDURE Exp1to3(inBuffer: Ptr;outBuffer: Ptr;cnt: LONGINT;inState: Ptr;
    outState: Ptr;numChannels: LONGINT;whichChannel: LONGINT);
    INLINE $203C,$0008,$0010,$A800;
PROCEDURE Comp6to1(inBuffer: Ptr;outBuffer: Ptr;cnt: LONGINT;inState: Ptr;
    outState: Ptr;numChannels: LONGINT;whichChannel: LONGINT);
    INLINE $203C,$000C,$0010,$A800;
PROCEDURE Exp1to6(inBuffer: Ptr;outBuffer: Ptr;cnt: LONGINT;inState: Ptr;
    outState: Ptr;numChannels: LONGINT;whichChannel: LONGINT);
    INLINE $203C,$0010,$0010,$A800;


{$ENDC}    { UsingSound }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

