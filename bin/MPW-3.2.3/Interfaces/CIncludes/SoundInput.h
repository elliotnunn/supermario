/*
	File:		SoundInput.h

	Contains:	Definitions for Sound Input Manager

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

*/



#ifndef __SOUNDINPUT__
#define __SOUNDINPUT__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif



#define siDeviceIsConnected		 1				/* input device is connected and ready for input */
#define siDeviceNotConnected	 0				/* input device is not connected */
#define siDontKnowIfConnected	-1				/* can't tell if input device is connected */

#define siReadPermission		 0				/* permission passed to SPBOpenDevice */
#define siWritePermission		 1				/* permission passed to SPBOpenDevice */

/* Info Selectors for Sound Input Drivers */

#define siDeviceConnected		'dcon'			/* input device connection status */
#define siAGCOnOff				'agc '			/* automatic gain control state */
#define siPlayThruOnOff			'plth'			/* playthrough state */
#define siTwosComplementOnOff	'twos'			/* two's complement state */
#define siLevelMeterOnOff		'lmet'			/* level meter state */
#define siRecordingQuality		'qual'			/* recording quality */
#define siVoxRecordInfo			'voxr'			/* VOX record parameters */
#define siVoxStopInfo			'voxs'			/* VOX stop parameters */
#define siNumberChannels		'chan'			/* current number of channels */
#define siSampleSize			'ssiz'			/* current sample size */
#define siSampleRate			'srat'			/* current sample rate */
#define siCompressionType		'comp'			/* current compression type */
#define siCompressionFactor		'cmfa'			/* current compression factor */
#define siCompressionHeader		'cmhd'			/* return compression header */
#define siDeviceName			'name'			/* input device name */
#define siDeviceIcon			'icon'			/* input device icon */
#define siDeviceBufferInfo		'dbin'			/* size of interrupt buffer */
#define siSampleSizeAvailable	'ssav'			/* sample sizes available */
#define siSampleRateAvailable	'srav'			/* sample rates available */
#define siCompressionAvailable	'cmav'			/* compression types available */
#define siChannelAvailable		'chav'			/* number of channels available */
#define siAsync					'asyn'			/* asynchronous capability */
#define siOptionsDialog			'optd'			/* display options dialog */
#define siContinuous	 		'cont'			/* continous recording */
#define siActiveChannels	 	'chac'			/* active channels */
#define siActiveLevels			'lmac'			/* active meter levels */
#define siInitializeDriver		'init'			/* reserved for internal use only */
#define siCloseDriver			'clos'			/* reserved for internal use only */
#define siPauseRecording		'paus'			/* reserved for internal use only */
#define siUserInterruptProc		'user'			/* reserved for internal use only */

/* Qualities */

#define siBestQuality			'best'
#define siBetterQuality			'betr'
#define siGoodQuality			'good'

/* Sound Input Parameter Block */

typedef struct {
	long			inRefNum;					/* reference number of sound input device */
	unsigned long	count;						/* number of bytes to record */
	unsigned long	milliseconds;				/* number of milliseconds to record */
	unsigned long	bufferLength;				/* length of buffer in bytes */
	Ptr		 		bufferPtr;					/* buffer to store sound data in */
	ProcPtr 		completionRoutine;			/* completion routine */
	ProcPtr 		interruptRoutine;			/* interrupt routine */
	long			userLong;					/* user-defined field */
	OSErr			error;						/* error */
	long			unused1;					/* reserved - must be zero */
} SPB, *SPBPtr;


#ifdef __cplusplus
extern "C" {
#endif
pascal NumVersion SPBVersion(void)
	= {0x203C,0x0000,0x0014,0xA800};

pascal OSErr SndRecord(ModalFilterProcPtr filterProc, Point corner, OSType quality, Handle *sndHandle)
	= {0x203C,0x0804,0x0014,0xA800};

pascal OSErr SndRecordToFile(ModalFilterProcPtr filterProc, Point corner, OSType quality, short fRefNum)
	= {0x203C,0x0708,0x0014,0xA800};

pascal OSErr SPBSignInDevice(short deviceRefNum, Str255 deviceName)
	= {0x203C,0x030C,0x0014,0xA800};

pascal OSErr SPBSignOutDevice(short deviceRefNum)
	= {0x203C,0x0110,0x0014,0xA800};

pascal OSErr SPBGetIndexedDevice(short count, Str255 deviceName, Handle *deviceIconHandle)
	= {0x203C,0x0514,0x0014,0xA800};

pascal OSErr SPBOpenDevice(ConstStr255Param deviceName, short permission, long *inRefNum)
	= {0x203C,0x0518,0x0014,0xA800};

pascal OSErr SPBCloseDevice(long inRefNum)
	= {0x203C,0x021C,0x0014,0xA800};

pascal OSErr SPBRecord(SPBPtr inParamPtr, Boolean asynchFlag)
	= {0x203C,0x0320,0x0014,0xA800};

pascal OSErr SPBRecordToFile(short fileRefNum, SPBPtr inParamPtr, Boolean asynchFlag)
	= {0x203C,0x0424,0x0014,0xA800};

pascal OSErr SPBPauseRecording(long inRefNum)
	= {0x203C,0x0228,0x0014,0xA800};

pascal OSErr SPBResumeRecording(long	inRefNum)
	= {0x203C,0x022C,0x0014,0xA800};

pascal OSErr SPBStopRecording(long inRefNum)
	= {0x203C,0x0230,0x0014,0xA800};

pascal OSErr SPBGetRecordingStatus( long			inRefNum,
									short			*recordingStatus,
									short			*meterLevel,
									unsigned long	*totalSamplesToRecord,
									unsigned long	*numberOfSamplesRecorded,
									unsigned long	*totalMsecsToRecord,
									unsigned long	*numberOfMsecsRecorded )
	= {0x203C,0x0E34,0x0014,0xA800};

pascal OSErr SPBGetDeviceInfo(long inRefNum, OSType infoType, char *infoData)
	= {0x203C,0x0638,0x0014,0xA800};

pascal OSErr SPBSetDeviceInfo(long inRefNum, OSType infoType, char *infoData)
	= {0x203C,0x063C,0x0014,0xA800};

pascal OSErr SPBMillisecondsToBytes(long inRefNum, long *milliseconds)
	= {0x203C,0x0440,0x0014,0xA800};

pascal OSErr SPBBytesToMilliseconds(long inRefNum, long *byteCount)
	= {0x203C,0x0444,0x0014,0xA800};

pascal OSErr SetupSndHeader(Handle sndH, short numChannels, Fixed sampleRate,
				short sampleSize, OSType compressionType,
				short baseNote, unsigned long numBytes, short *headerLen)
	= {0x203C,0x0D48,0x0014,0xA800};

pascal OSErr SetupAIFFHeader(short fRefNum, short numChannels, Fixed sampleRate,
				short sampleSize, OSType compressionType,
				unsigned long numBytes, unsigned long numFrames)
	= {0x203C,0x0B4C,0x0014,0xA800};

#ifdef __cplusplus
}
#endif

#endif
