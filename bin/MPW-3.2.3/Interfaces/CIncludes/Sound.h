/************************************************************

Created: Monday, January 28, 1991 at 6:54 PM
    Sound.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1986-1990
        All rights reserved

************************************************************/


#ifndef __SOUND__
#define __SOUND__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif


enum {

    swMode = -1,                            /* Sound Driver modes */
    ftMode = 1,
    ffMode = 0,


#define synthCodeRsrc 'snth'                /* Resource types used by Sound Manager */
#define soundListRsrc 'snd '

#define twelfthRootTwo 1.05946309434

    rate22khz = 0x56EE8BA3,                 /* 22254.54545 in fixed-point */
    rate11khz = 0x2B7745D1,                 /* 11127.27273 in fixed-point */

/* synthesizer numbers for SndNewChannel */
    squareWaveSynth = 1,                    /*square wave synthesizer*/
    waveTableSynth = 3,                     /*wave table synthesizer*/
    sampledSynth = 5,                       /*sampled sound synthesizer*/

/* old Sound Manager MACE synthesizer numbers */
    MACE3snthID = 11,
    MACE6snthID = 13,

/* command numbers for SndDoCommand and SndDoImmediate */
    nullCmd = 0,
    initCmd = 1,
    freeCmd = 2,
    quietCmd = 3,
    flushCmd = 4,
    reInitCmd = 5,

    waitCmd = 10,
    pauseCmd = 11,
    resumeCmd = 12,
    callBackCmd = 13
};
enum {
    syncCmd = 14,
    emptyCmd = 15,

    tickleCmd = 20,
    requestNextCmd = 21,
    howOftenCmd = 22,
    wakeUpCmd = 23,
    availableCmd = 24,
    versionCmd = 25,
    totalLoadCmd = 26,
    loadCmd = 27,

    scaleCmd = 30,
    tempoCmd = 31,

    freqDurationCmd = 40,
    restCmd = 41,
    freqCmd = 42,
    ampCmd = 43,
    timbreCmd = 44,
    getAmpCmd = 45,

    waveTableCmd = 60,
    phaseCmd = 61
};
enum {

    soundCmd = 80,
    bufferCmd = 81,
    rateCmd = 82,
    continueCmd = 83,
    doubleBufferCmd = 84,
    getRateCmd = 85,

    sizeCmd = 90,
    convertCmd = 91,

    stdQLength = 128,
    dataOffsetFlag = 0x8000,

    waveInitChannelMask = 0x07,
    waveInitChannel0 = 0x04,
    waveInitChannel1 = 0x05,
    waveInitChannel2 = 0x06,
    waveInitChannel3 = 0x07,

/* channel initialization parameters */
    initPanMask = 0x0003,                   /* mask for right/left pan values */
    initSRateMask = 0x0030,                 /* mask for sample rate values */
    initStereoMask = 0x00C0,                /* mask for mono/stereo values */
    initCompMask = 0xFF00,                  /* mask for compression IDs */

    initChanLeft = 0x0002                   /* left stereo channel */
};
enum {
    initChanRight = 0x0003,                 /* right stereo channel */
    initNoInterp = 0x0004,                  /* no linear interpolation */
    initNoDrop = 0x0008,                    /* no drop-sample conversion */
    initMono = 0x0080,                      /* monophonic channel */
    initStereo = 0x00C0,                    /* stereo channel */
    initMACE3 = 0x0300,                     /* MACE 3:1 */
    initMACE6 = 0x0400,                     /* MACE 6:1 */

    initChan0 = 0x0004,                     /* channel 0 - wave table only */
    initChan1 = 0x0005,                     /* channel 1 - wave table only */
    initChan2 = 0x0006,                     /* channel 2 - wave table only */
    initChan3 = 0x0007,                     /* channel 3 - wave table only */

    stdSH = 0x00,                           /* Standard sound header encode value */
    extSH = 0xFF,                           /* Extended sound header encode value */
    cmpSH = 0xFE,                           /* Compressed sound header encode value */

    notCompressed = 0,                      /* compression ID's */
    twoToOne = 1,
    eightToThree = 2,
    threeToOne = 3,
    sixToOne = 4,

    outsideCmpSH = 0                        /* MACE constants */
};
enum {
    insideCmpSH = 1,
    aceSuccess = 0,
    aceMemFull = 1,
    aceNilBlock = 2,
    aceBadComp = 3,
    aceBadEncode = 4,
    aceBadDest = 5,
    aceBadCmd = 6,
    sixToOnePacketSize = 8,
    threeToOnePacketSize = 16,
    stateBlockSize = 64,
    leftOverBlockSize = 32,

    firstSoundFormat = 0x0001,              /* general sound format */
    secondSoundFormat = 0x0002,             /* special sampled sound format (HyperCard) */

    dbBufferReady = 0x00000001,             /* double buffer is filled */
    dbLastBuffer = 0x00000004,              /* last double buffer to play */

    sysBeepDisable = 0x0000,                /* SysBeep() enable flags */
    sysBeepEnable = 0x0001,

    unitTypeNoSelection = 0xFFFF,           /* unitTypes for AudioSelection.unitType */
    unitTypeSeconds = 0x0000
};


/*            Structures for Sound Driver            */


typedef unsigned char FreeWave[30001]; 

struct FFSynthRec {
    short mode;
    Fixed count;
    FreeWave waveBytes;
};

typedef struct FFSynthRec FFSynthRec;
typedef FFSynthRec *FFSynthPtr;

struct Tone {
    short count;
    short amplitude;
    short duration;
};

typedef struct Tone Tone;


typedef Tone Tones[5001];

struct SWSynthRec {
    short mode;
    Tones triplets;
};

typedef struct SWSynthRec SWSynthRec;
typedef SWSynthRec *SWSynthPtr;


typedef unsigned char Wave[256]; 
typedef Wave *WavePtr;

struct FTSoundRec {
    short duration;
    Fixed sound1Rate;
    long sound1Phase;
    Fixed sound2Rate;
    long sound2Phase;
    Fixed sound3Rate;
    long sound3Phase;
    Fixed sound4Rate;
    long sound4Phase;
    WavePtr sound1Wave;
    WavePtr sound2Wave;
    WavePtr sound3Wave;
    WavePtr sound4Wave;
};

typedef struct FTSoundRec FTSoundRec;
typedef FTSoundRec *FTSndRecPtr;

struct FTSynthRec {
    short mode;
    FTSndRecPtr sndRec;
};

typedef struct FTSynthRec FTSynthRec;
typedef FTSynthRec *FTSynthPtr;


typedef pascal void (*SndCompletionProcPtr)(void);

/*            Structures for Sound Manager            */

struct SndCommand {
    unsigned short cmd;
    short param1;
    long param2;
};

typedef struct SndCommand SndCommand;


typedef long Time;                          /* in half milliseconds */



typedef struct SndChannel SndChannel;
typedef SndChannel *SndChannelPtr;


typedef pascal void (*SndCallBackProcPtr)(SndChannelPtr chan, SndCommand cmd);

struct SndChannel {
    struct SndChannel *nextChan;
    Ptr firstMod;                           /* reserved for the Sound Manager */
    SndCallBackProcPtr callBack;
    long userInfo;
    Time wait;                              /* The following is for internal Sound Manager use only.*/
    SndCommand cmdInProgress;
    short flags;
    short qLength;
    short qHead;                            /* next spot to read or -1 if empty */
    short qTail;                            /* next spot to write = qHead if full */
    SndCommand queue[stdQLength];
};

/* MACE structures */
struct StateBlock {
    short stateVar[stateBlockSize];
};

typedef struct StateBlock StateBlock;
typedef StateBlock *StateBlockPtr;

struct LeftOverBlock {
    unsigned long count;
    char sampleArea[leftOverBlockSize];
};

typedef struct LeftOverBlock LeftOverBlock;
typedef LeftOverBlock *LeftOverBlockPtr;

struct ModRef {
    unsigned short modNumber;
    long modInit;
};

typedef struct ModRef ModRef;

struct SndListResource {
    short format;
    short numModifiers;
    ModRef modifierPart[1];                 /*This is a variable length array*/
    short numCommands;
    SndCommand commandPart[1];              /*This is a variable length array*/
    char dataPart[1];                       /*This is a variable length array*/
};

typedef struct SndListResource SndListResource;
typedef SndListResource *SndListPtr;

struct SoundHeader {
    Ptr samplePtr;                          /* if NIL then samples are in sampleArea */
    unsigned long length;                   /* length of sound in bytes */
    Fixed sampleRate;                       /* sample rate for this sound */
    unsigned long loopStart;                /* start of looping portion */
    unsigned long loopEnd;                  /* end of looping portion */
    unsigned char encode;                   /* header encoding */
    unsigned char baseFrequency;            /* baseFrequency value */
    char sampleArea[1];
};

typedef struct SoundHeader SoundHeader;
typedef SoundHeader *SoundHeaderPtr;


struct CmpSoundHeader {
    Ptr samplePtr;                          /* if nil then samples are in sample area */
    unsigned long numChannels;              /* number of channels i.e. mono = 1 */
    Fixed sampleRate;                       /* sample rate in Apples Fixed point representation */
    unsigned long loopStart;                /* loopStart of sound before compression */
    unsigned long loopEnd;                  /* loopEnd of sound before compression */
    unsigned char encode;                   /* data structure used , stdSH, extSH, or cmpSH */
    unsigned char baseFrequency;            /* same meaning as regular SoundHeader */
    unsigned long numFrames;                /* length in frames ( packetFrames or sampleFrames ) */
    extended AIFFSampleRate;                /* IEEE sample rate */
    Ptr markerChunk;                        /* sync track */
    Ptr futureUse1;                         /* reserved by Apple */
    Ptr futureUse2;                         /* reserved by Apple */
    StateBlockPtr stateVars;                /* pointer to State Block */
    LeftOverBlockPtr leftOverSamples;       /* used to save truncated samples between compression calls */
    unsigned short compressionID;           /* 0 means no compression, non zero means compressionID */
    unsigned short packetSize;              /* number of bits in compressed sample packet */
    unsigned short snthID;                  /* resource ID of Sound Manager snth that contains NRT C/E */
    unsigned short sampleSize;              /* number of bits in non-compressed sample */
    char sampleArea[1];                     /* space for when samples follow directly */
};

typedef struct CmpSoundHeader CmpSoundHeader;
typedef CmpSoundHeader *CmpSoundHeaderPtr;

struct ExtSoundHeader {
    Ptr samplePtr;                          /* if nil then samples are in sample area */
    unsigned long numChannels;              /* number of channels,  ie mono = 1 */
    Fixed sampleRate;                       /* sample rate in Apples Fixed point representation */
    unsigned long loopStart;                /* same meaning as regular SoundHeader */
    unsigned long loopEnd;                  /* same meaning as regular SoundHeader */
    unsigned char encode;                   /* data structure used , stdSH, extSH, or cmpSH */
    unsigned char baseFrequency;            /* same meaning as regular SoundHeader */
    unsigned long numFrames;                /* length in total number of frames */
    extended AIFFSampleRate;                /* IEEE sample rate */
    Ptr markerChunk;                        /* sync track */
    Ptr instrumentChunks;                   /* AIFF instrument chunks */
    Ptr AESRecording;
    unsigned short sampleSize;              /* number of bits in sample */
    unsigned short futureUse1;              /* reserved by Apple */
    unsigned long futureUse2;               /* reserved by Apple */
    unsigned long futureUse3;               /* reserved by Apple */
    unsigned long futureUse4;               /* reserved by Apple */
    char sampleArea[1];                     /* space for when samples follow directly */
};

typedef struct ExtSoundHeader ExtSoundHeader;
typedef ExtSoundHeader *ExtSoundHeaderPtr;

struct ConversionBlock {
    short destination;
    short unused;
    CmpSoundHeaderPtr inputPtr;
    CmpSoundHeaderPtr outputPtr;
};

typedef struct ConversionBlock ConversionBlock;
typedef ConversionBlock *ConversionBlockPtr;

struct SMStatus {
    short smMaxCPULoad;
    short smNumChannels;
    short smCurCPULoad;
};

typedef struct SMStatus SMStatus;
typedef SMStatus *SMStatusPtr;

struct SCStatus {
    Fixed scStartTime;
    Fixed scEndTime;
    Fixed scCurrentTime;
    Boolean scChannelBusy;
    Boolean scChannelDisposed;
    Boolean scChannelPaused;
    Boolean scUnused;
    unsigned long scChannelAttributes;
    long scCPULoad;
};

typedef struct SCStatus SCStatus;
typedef SCStatus *SCStatusPtr;

struct AudioSelection {
    long unitType;
    Fixed selStart;
    Fixed selEnd;
};

typedef struct AudioSelection AudioSelection;
typedef AudioSelection *AudioSelectionPtr;

struct SndDoubleBuffer {
    long dbNumFrames;
    long dbFlags;
    long dbUserInfo[2];
    char dbSoundData[1];
};

typedef struct SndDoubleBuffer SndDoubleBuffer;
typedef SndDoubleBuffer *SndDoubleBufferPtr;


typedef pascal void (*SndDoubleBackProcPtr) (SndChannelPtr channel,
                                            SndDoubleBufferPtr doubleBufferPtr);

struct SndDoubleBufferHeader {
    short dbhNumChannels;
    short dbhSampleSize;
    short dbhCompressionID;
    short dbhPacketSize;
    Fixed dbhSampleRate;
    SndDoubleBufferPtr dbhBufferPtr[2];
    SndDoubleBackProcPtr dbhDoubleBack;
};

typedef struct SndDoubleBufferHeader SndDoubleBufferHeader;
typedef SndDoubleBufferHeader *SndDoubleBufferHeaderPtr;


#ifdef __cplusplus
extern "C" {
#endif
pascal OSErr SndDoCommand(SndChannelPtr chan,const SndCommand *cmd,Boolean noWait)
    = 0xA803; 
pascal OSErr SndDoImmediate(SndChannelPtr chan,const SndCommand *cmd)
    = 0xA804; 
pascal OSErr SndNewChannel(SndChannelPtr *chan,short synth,long init,SndCallBackProcPtr userRoutine)
    = 0xA807; 
pascal OSErr SndDisposeChannel(SndChannelPtr chan,Boolean quietNow)
    = 0xA801; 
pascal OSErr SndPlay(SndChannelPtr chan,Handle sndHdl,Boolean async)
    = 0xA805; 
pascal OSErr SndAddModifier(SndChannelPtr chan,ProcPtr modifier,short id,
    long init)
    = 0xA802; 
pascal OSErr SndControl(short id,SndCommand *cmd)
    = 0xA806; 

pascal void SetSoundVol(short level); 
#pragma parameter GetSoundVol(__A0)
pascal void GetSoundVol(short *level)
    = {0x4218,0x10B8,0x0260}; 
pascal void StartSound(const void *synthRec,long numBytes,SndCompletionProcPtr completionRtn); 
pascal void StopSound(void); 
pascal Boolean SoundDone(void); 

pascal NumVersion SndSoundManagerVersion(void)
    = {0x203C,0x000C,0x0008,0xA800}; 
pascal OSErr SndStartFilePlay(SndChannelPtr chan,short fRefNum,short resNum,
    long bufferSize,void *theBuffer,AudioSelectionPtr theSelection,ProcPtr theCompletion,
    Boolean async)
    = {0x203C,0x0D00,0x0008,0xA800}; 
pascal OSErr SndPauseFilePlay(SndChannelPtr chan)
    = {0x203C,0x0204,0x0008,0xA800}; 
pascal OSErr SndStopFilePlay(SndChannelPtr chan,Boolean async)
    = {0x203C,0x0308,0x0008,0xA800}; 
pascal OSErr SndChannelStatus(SndChannelPtr chan,short theLength,SCStatusPtr theStatus)
    = {0x203C,0x0010,0x0008,0xA800}; 
pascal OSErr SndManagerStatus(short theLength,SMStatusPtr theStatus)
    = {0x203C,0x0014,0x0008,0xA800}; 
pascal void SndGetSysBeepState(short *sysBeepState)
    = {0x203C,0x0018,0x0008,0xA800}; 
pascal OSErr SndSetSysBeepState(short sysBeepState)
    = {0x203C,0x001C,0x0008,0xA800}; 
pascal OSErr SndPlayDoubleBuffer(SndChannelPtr chan,SndDoubleBufferHeaderPtr theParams)
    = {0x203C,0x0020,0x0008,0xA800}; 

pascal NumVersion MACEVersion(void)
    = {0x203C,0x0000,0x0010,0xA800}; 
pascal void Comp3to1(const void *inBuffer,void *outBuffer,unsigned long cnt,
    const void *inState,void *outState,unsigned long numChannels,unsigned long whichChannel)
    = {0x203C,0x0004,0x0010,0xA800}; 
pascal void Exp1to3(const void *inBuffer,void *outBuffer,unsigned long cnt,
    const void *inState,void *outState,unsigned long numChannels,unsigned long whichChannel)
    = {0x203C,0x0008,0x0010,0xA800}; 
pascal void Comp6to1(const void *inBuffer,void *outBuffer,unsigned long cnt,
    const void *inState,void *outState,unsigned long numChannels,unsigned long whichChannel)
    = {0x203C,0x000C,0x0010,0xA800}; 
pascal void Exp1to6(const void *inBuffer,void *outBuffer,unsigned long cnt,
    const void *inState,void *outState,unsigned long numChannels,unsigned long whichChannel)
    = {0x203C,0x0010,0x0010,0xA800}; 
#ifdef __cplusplus
}
#endif

#endif
