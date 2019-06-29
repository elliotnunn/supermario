{
Created: Wednesday, June 27, 1990 at 6:42 PM
    SoundInput.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc. 1989-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT SoundInput;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingSoundInput}
{$SETC UsingSoundInput := 1}

{$I+}
{$SETC SoundInputIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingDialogs}
{$I $$Shell(PInterfaces)Dialogs.p}
{$ENDC}
{$IFC UNDEFINED UsingFiles}
{$I $$Shell(PInterfaces)Files.p}
{$ENDC}
{$SETC UsingIncludes := SoundInputIncludes}


CONST

siDeviceIsConnected =	 1;				{ input device is connected and ready for input }
siDeviceNotConnected =	 0;				{ input device is not connected }
siDontKnowIfConnected =	-1;				{ can't tell if input device is connected }

siReadPermission =		 0;				{ permission passed to SPBOpenDevice }
siWritePermission = 	 1;				{ permission passed to SPBOpenDevice }

{ Info Selectors for Sound Input Drivers }

siDeviceConnected =		'dcon';			{ input device connection status }
siAGCOnOff =			'agc ';			{ automatic gain control state }
siPlayThruOnOff	=		'plth';			{ playthrough state }
siTwosComplementOnOff =	'twos';			{ two's complement state }
siLevelMeterOnOff =		'lmet';			{ level meter state }
siRecordingQuality =	'qual';			{ recording quality }
siVoxRecordInfo	=		'voxr';			{ VOX record parameters }
siVoxStopInfo =			'voxs';			{ VOX stop parameters }
siNumberChannels =		'chan';			{ current number of channels }
siSampleSize =			'ssiz';			{ current sample size }
siSampleRate =			'srat';			{ current sample rate }
siCompressionType =		'comp';			{ current compression type }
siCompressionFactor	=	'cmfa';			{ current compression factor }
siCompressionHeader	=	'cmhd';			{ return compression header }
siDeviceName =			'name';			{ input device name }
siDeviceIcon =			'icon';			{ input device icon }
siDeviceBufferInfo =	'dbin';			{ size of interrupt buffer }
siSampleSizeAvailable =	'ssav';			{ sample sizes available }
siSampleRateAvailable =	'srav';			{ sample rates available }
siCompressionAvailable ='cmav';			{ compression types available }
siChannelAvailable =	'chav';			{ number of channels available }
siAsync	=				'asyn';			{ asynchronous capability }
siOptionsDialog =		'optd';			{ display options dialog }
siContinuous = 			'cont';			{ continous recording }
siActiveChannels = 		'chac';			{ active channels }
siActiveLevels = 		'lmac';			{ active meter levels }
siInitializeDriver =	'init';			{ reserved for internal use only }
siCloseDriver =			'clos';			{ reserved for internal use only }
siPauseRecording =		'paus';			{ reserved for internal use only }
siUserInterruptProc	=	'user';			{ reserved for internal use only }

{ Qualities }

siBestQuality =			'best';
siBetterQuality =		'betr';
siGoodQuality =			'good';


TYPE

SPBPtr = ^SPB;
SPB = RECORD
	inRefNum:			LONGINT;			{ reference number of sound input device }
	count:				LONGINT;			{ number of bytes to record }
	milliseconds:		LONGINT;			{ number of milliseconds to record }
	bufferLength:		LONGINT;			{ length of buffer in bytes }
	bufferPtr:			Ptr;				{ buffer to store sound data in }
	completionRoutine:	ProcPtr;			{ completion routine }
	interruptRoutine:	ProcPtr;			{ interrupt routine }
	userLong:			LONGINT;			{ user-defined field }
	error:				OSErr;				{ error }
	unused1:			LONGINT;			{ reserved - must be zero }
	END;


FUNCTION SPBVersion : NumVersion;
	INLINE $203C,$0000,$0014,$A800;

FUNCTION SndRecord(filterProc: ModalFilterProcPtr; corner: Point; quality: OSType;
	VAR sndHandle: Handle) : OSErr;
	INLINE $203C,$0804,$0014,$A800;

FUNCTION SndRecordToFile(filterProc: ModalFilterProcPtr; corner: Point; quality: OSType;
	fRefNum: INTEGER) : OSErr;
	INLINE $203C,$0708,$0014,$A800;

FUNCTION SPBSignInDevice(deviceRefNum: INTEGER; deviceName: Str255) : OSErr;
	INLINE $203C,$030C,$0014,$A800;

FUNCTION SPBSignOutDevice(deviceRefNum: INTEGER) : OSErr;
	INLINE $203C,$0110,$0014,$A800;

FUNCTION SPBGetIndexedDevice(count: INTEGER; VAR deviceName: Str255;
	VAR deviceIconHandle: Handle) : OSErr;
	INLINE $203C,$0514,$0014,$A800;

FUNCTION SPBOpenDevice(deviceName: Str255; permission: INTEGER; VAR inRefNum: LONGINT) : OSErr;
	INLINE $203C,$0518,$0014,$A800;

FUNCTION SPBCloseDevice(inRefNum: LONGINT) : OSErr;
	INLINE $203C,$021C,$0014,$A800;

FUNCTION SPBRecord(inParamPtr: SPBPtr; asynchFlag: BOOLEAN) : OSErr;
	INLINE $203C,$0320,$0014,$A800;

FUNCTION SPBRecordToFile(fRefNum: INTEGER; inParamPtr: SPBPtr; asynchFlag: BOOLEAN) : OSErr;
	INLINE $203C,$0424,$0014,$A800;

FUNCTION SPBPauseRecording(inRefNum: LONGINT) : OSErr;
	INLINE $203C,$0228,$0014,$A800;

FUNCTION SPBResumeRecording(inRefNum: LONGINT) : OSErr;
	INLINE $203C,$022C,$0014,$A800;

FUNCTION SPBStopRecording(inRefNum: LONGINT) : OSErr;
	INLINE $203C,$0230,$0014,$A800;

FUNCTION SPBGetRecordingStatus(inRefNum: LONGINT; VAR recordingStatus: INTEGER; VAR meterLevel: INTEGER;
	VAR totalSamplesToRecord: LONGINT; VAR numberOfSamplesRecorded: LONGINT;
	VAR totalMsecsToRecord: LONGINT; VAR numberOfMsecsRecorded: LONGINT) : OSErr;
	INLINE $203C,$0E34,$0014,$A800;

FUNCTION SPBGetDeviceInfo(inRefNum: LONGINT; infoType: OSType; infoData: Ptr) : OSErr;
	INLINE $203C,$0638,$0014,$A800;

FUNCTION SPBSetDeviceInfo(inRefNum: LONGINT; infoType: OSType; infoData: Ptr) : OSErr;
	INLINE $203C,$063C,$0014,$A800;

FUNCTION SPBMillisecondsToBytes(inRefNum: LONGINT; VAR milliseconds: LONGINT) : OSErr;
	INLINE $203C,$0440,$0014,$A800;

FUNCTION SPBBytesToMilliseconds(inRefNum: LONGINT; VAR byteCount: LONGINT) : OSErr;
	INLINE $203C,$0444,$0014,$A800;

FUNCTION SetupSndHeader(sndHandle: Handle; numChannels: INTEGER; sampleRate: Fixed;
				sampleSize: INTEGER; compressionType: OSType; baseNote: INTEGER;
				numBytes: LONGINT; VAR headerLen: INTEGER) : OSErr;
	INLINE $203C,$0D48,$0014,$A800;

FUNCTION SetupAIFFHeader(fRefNum: INTEGER;  numChannels: INTEGER; sampleRate: Fixed;
				sampleSize: INTEGER; compressionType: OSType;
				numBytes: LONGINT; numFrames: LONGINT) : OSErr;
	INLINE $203C,$0B4C,$0014,$A800;

{$ENDC}    { UsingSoundInput }

{$IFC NOT UsingIncludes}
	END.
{$ENDC}
