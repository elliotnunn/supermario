/*
	File:		Dictionary.c

	Contains:	Implemetation of Dictionary Manager routines.

	Written by:	Hae-sung Kim, Simon Lee

	Copyright:	© 1991-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	
	   <SM6>	 7/21/93	joe		Back out <SM5>.
		<19>	  7/9/92	hsK		#1035398,<CJW>: Modify the CopySourceUsingAttributeTable routine
									so that it can get more than one attribute entries.
		<18>	 6/26/92	hsK		#1033512: Improved the speed of FindRecordByIndex routine by
									giving the hint to the BTGetRec routine.
		<17>	 6/25/92	hsK		Use the BTree manager error code names.
		<16>	 5/27/92	hsK		#1030902,<YK>: Includes the Errors.h.
		<15>	 5/26/92	hsK		• #1030304 bug fix: call BTFlush routine after BTOpen in the
									OpenDictionary_ATrap routine. • change the "scriptCode" of the
									DictionaryInformation record to "script" for correct Pascal
									syntax.
		<14>	 5/15/92	hsK		Dean updated ResourceMgrExtensions file to add
									CloseResFileUnderSystemMap so I changed to use it instead of
									using SuspendProcessMgrFileTracking routine <13>.
		<13>	 5/14/92	hsK		• changed kDataBufferSize from 1K to 4K and resource file clump
									size from 50K to 8K. • #1029463: change the InitializeDictionary
									routine to accept the script code. add formatNumber and
									scriptCode to the DictionaryInformation record. • check
									dictionary format number in the OpenDictionary. Later if we have
									different format from current one (format 1) • for this build, I
									used SuspendProcessMgrFileTracking and
									ResumeProcessMgrFileTracking when closing the dictionary to make
									Process manager happy (FileMgrPatches.a).
		<12>	 5/10/92	KSM		<dty> Fixed call to OpenResFileUnderSystemMap() to fix the
									build.
		<11>	  5/5/92	hsK		• #1027602 bug fix: call OpenResFileUnderSystemMap instead of
									OpenResFile in the OpenDictionary_ATrap routine to open the
									dictionary across the applications. • rewrote CompactDictionary
									routine. made it cleaner, smaller and faster than old
									CompactDictionary. • allocate the internal dataBuffer inside
									dictionary reference record instead of allocating it separately
									as a handle. • call ResError after UseResFile. • save the
									current resource file number on entry and restore it on exit for
									every Dictionary routines except InitializeDictionary. • get rid
									of MoveHHi because dictionary reference handle may be allocated
									in the system heap. • use Get1Resource instead of GetResource. •
									check whether both dictionary info and data resources are
									avaiable when OpenDictionary.
		<10>	  3/5/92	hsK		Call UseResFile to use multiple dictionaries in chain.
		 <9>	  2/7/92	hsK		return badDictionaryErr in OpenDictionary_ATrap, if cannot find
									kDictInfoType resource in the opened file .
		 <8>	 1/29/92	hsK		Use ConstStr255Param and const qualifier to unchanged pointer
									parameters. Fixed bugs on returning wrong error codes. The
									numberOfAttributes in CopySourceUsingAttributeTable function is
									now type casted from the unsinged byte type. Changed
									entryDataHandle to recordDataHandle.
		<6>		01/23/92	hsK		Fixed bugs in partial resource cache scheme.
									Fixed bugs in CompactDictionary algorithm.
									Changed the DictionaryInformation record (see Dictionary.h).
									Delete the DictionaryPrivate.h file and moved its all type and constant 
									definitions into here.  Inhence input parameters check for every routine.
									Always use the FFSpec instead of old vRef number plus something
									in every local routines too.  Fixed memory leak problems
									caused by not disposing some handles. 
									This is the major upgrade from previous code.  
		<5>		11/27/91	hsK		Instead of using 4 byte long (OSType) attribute type, use one
									signed byte field to represent the attribute types to avoid
									accessing odd address problem and to remove Compress and
									Decompress routines.  Added GetRequestedAttributes routine instead of 
									Decompress routine.  Also Don't use dataBufferHandle in the 
									InsertRecordToDictionary routine.
		<4>		11/20/91	hsK		Changed the dictionary manager public routine names to "name_ATrap" so that 
									they are different from the public interface names in the Dictionary.h file.
		<3>		11/20/91	KST		Changed IntializeDictionary and OpenDictionary to use FSSpec.
									Changed OpenDictionary to take access permission. And fixed many
									miscsllaneous bugs.
		<2>		30 Jul 91	Simon Lee -- Use partial resource to store the actual data in the dictionary.
									A simple caching scheme has been put in to cache the partial
									resource data.
		<1>		10 May 91	Hae-sung Kim -- New today.

*/


#include	"Types.h"
#include	"BaseTypes.h"
#include	"Resources.h"
#include 	"Memory.h"
#include	"Files.h"
#include 	"BTEqu.h"
#include 	"Dictionary.h"
#include 	"ResourceMgrPriv.h"		// <11>

/* ------------------------------------------------------------------------------------------
	Local constant definitions.
------------------------------------------------------------------------------------------ */

#define		kDictionaryFormatNumber	0x00010000	// <13> current dictionary format number (fixed).
#define		kDictInfoType			'dinf'		// dictionary information resource type.
#define		kDictDataType			'ddat'		// dictionary data resource type.

enum { kMaximumKeyLength = 129 };				// maximum key length is 129 bytes.
enum { kBTreeClumpSize = 0 };					// let B*Tree manager fills this in for us.
enum { kDataBufferSize = 4096 };				// <13> size of the cache buffer which Dictonary Manager uses to store. 
enum { kDictDataClumpSize = 8192 };				// <13> dictionary data resource clump size (50K). 
enum { kDictionaryResourceID = 128 };			// id of the dictionary info and dictionary data resource.
	   
enum {		// <17> cannot use the Errors.h because the BTEqu.h has the same constants.
/*Dictionary Manager errors*/
 dupFNErr = -48,								/*duplicate filename (rename)*/
//notBTree 				= 	-410,				/*The file is not a dictionary.*/
//btNoSpace 			= 	-413,				/*Can't allocate disk space.*/
//btDupRecErr 			= 	-414,				/*Record already exists.*/
//btRecNotFnd			= 	-415,				/*Record cannot be found.*/
//btKeyLenErr			= 	-416,				/*Maximum key length is too long or equal to zero.*/
//btKeyAttrErr			=	-417,				/*There is no such a key attribute.*/
unknownInsertModeErr	=	-20000,				/*There is no such an insert mode.*/
recordDataTooBigErr		= 	-20001,				/*The record data is bigger than buffer size (1024 bytes).*/
invalidIndexErr			=	-20002				/*The recordIndex parameter is not valid.*/
};

/* ------------------------------------------------------------------------------------------
	Local type definitions.
------------------------------------------------------------------------------------------ */

// Dictionary information record.
typedef struct {

	Fixed				formatNumber;				// <13> dictionary format number.
	LongInteger			currentGarbageSize;			// current garabage size in a dictionary.
	LongInteger			dictDataLEOF;				// logical end of file of a dictionary.
	LongInteger			dictDataPEOF;				// physical end of file of a dictionary.
	UnsignedByte		keyAttributes;				// key attributes flag of a dictionary.
	UnsignedByte		reserved;					// <13> fill byte.
	ScriptCode			scriptCode;					// <13> dictionary data script code.	

} DictInfoRecord, *DictInfoRecordPtr, **DictInfoRecordHandle;

// Private data for each opened dictionary.
// removed the dictInfoRecord field. dictionary info resource is loaded for every dictionary manager call. <11>
// removed the compactFlag field. it is not used because of CompactDictionary routine change. <11>
typedef struct {

	FSSpec				fsspec;					// file spec record for the current dictionary file.
	Integer				dataRefNum;				// access path reference for the data fork.
	Integer				resourceRefNum;			// access path reference for the resource fork.
	LongInteger			bTreeReservedUID;		// used for BTree routines.
	LongInteger			cacheStartOffset;		// offset which marks the start of the data which we cache in the buffer.

	LongInteger			previousIndex;			// <17> used in FindRecordByIndex routine to place the file marker.
	LongInteger			ioBTHint1;				// <17> used in FindRecordByIndex routine to speed up.
	LongInteger			ioBTHint2;				// <17>
	Integer				ioBTHint3;				// <17>
	Integer				ioBTHint4;				// <17>
	LongInteger			ioBTHint5;				// <17>

	UnsignedByte		dataBuffer[ kDataBufferSize];	// used for disk cache. <11>
		
} DictReferenceRecord, *DictReferenceRecordPointer, **DictReferenceRecordHandle;

// The information which are actually stored into the node of the BTree.
typedef struct {

	LongInteger			dataOffset;			// offset from beginning the data resource to beginning of the data in this node.
	LongInteger			dataSize;			// length of the data in this node.
	
} BTreeRecord;

/* ------------------------------------------------------------------------------------------
	Forward function declarations.
------------------------------------------------------------------------------------------ */
/* <14>
// <13> different names of the BeginSystemMode and EndSystemMode.
pascal OSErr SuspendProcessMgrFileTracking(void)
= {0x303C,0x0011,0xAA52}; 
pascal OSErr ResumeProcessMgrFileTracking(void)
= {0x303C,0x0012,0xAA52}; 
*/

pascal OSErr GetRequestedAttributes( 	const UnsignedBytePointer	sourceBufferPointer,
										Size						sourceBufferSize,
										const Ptr					requestedAttributeTablePointer,
										Handle						targetHandle);
pascal OSErr CopySourceWithAllAttributes( 	const UnsignedBytePointer	sourceBufferPointer,
											Size						sourceBufferSize,
											Handle						targetHandle);
pascal OSErr CopySourceWithoutAttributes( 	UnsignedBytePointer		sourceBufferPointer,
											Handle					targetHandle);
pascal OSErr CopySourceUsingAttributeTable( UnsignedBytePointer		sourceBufferPointer,
											const Ptr				requestedAttributeTablePointer,
											Handle					targetHandle);

/* ------------------------------------------------------------------------------------------
	•	Initialize a dictonary file with a given name on a specified volume/directory.
		User provide the dictionary with the length of key (maximumKeyLength) so as
		minimize disk space use. "keyAttributes" specifies the searching critereas 
		for a key. The pre-defined attributes are as followed:
				
	•	kIsCaseSensitive:			an upper case key is different from a lower case key.
		kIsDiacriticalNotSensitive:	a key with diacritical is the same as one without it.
    
	•	A dictionary file should be created by "Create" before this routine.
------------------------------------------------------------------------------------------ */

pascal OSErr InitializeDictionary_ATrap( 	FSSpecPtr		fsspecPointer,
											Integer			maximumKeyLength,
											UnsignedByte	keyAttributes,
											ScriptCode		scriptCode)				// <13>
{
	OSErr					functionResult = noErr;
	UnsignedByte			keyDescriptorRecord[4];
	BTParam					bTreeParameterBlock;
	Integer					resourceRefNumber = 0;
	Handle					dictDataResource = nil;
	Handle					dictInfoResource = nil;
	Boolean					closeResourceFile = false;
	Boolean					disposeDictDataResource = false, disposeDictInfoResource = false;
	
	// First check if input parameters are valid.
	if (maximumKeyLength > kMaximumKeyLength) {
		functionResult = btKeyLenErr;
		goto bail;
	} else if ((keyAttributes != caseSens) && (keyAttributes != diacNsens)) {
		functionResult = btKeyAttrErr;	// <8> hsK
		goto bail;
	}
		
	// Fill in the BTree parameter block.
	bTreeParameterBlock.ioCompletion = nil;
	bTreeParameterBlock.ioNamePtr = fsspecPointer->name;
	bTreeParameterBlock.ioBTMaxKLen = maximumKeyLength;
	bTreeParameterBlock.ioBTClumpSize = kBTreeClumpSize;
	bTreeParameterBlock.ioVRefNum = fsspecPointer->vRefNum;
	bTreeParameterBlock.ioDirID = fsspecPointer->parID;

	// Set the key descriptor.
	keyDescriptorRecord[0] = 3;				// number of bytes in the record.
	keyDescriptorRecord[1] = kdString;		// key is a Pascal string.
	keyDescriptorRecord[2] = 1;				// occurrence.
	keyDescriptorRecord[3] = keyAttributes;	// the key attribute flags.
	bTreeParameterBlock.ioBTKDPtr = (Ptr) keyDescriptorRecord;

	// Install the BTree structure into the file.
	functionResult = BTInit( &bTreeParameterBlock, false); 
	if (functionResult != noErr)  {
		goto bail;
	}
	
	// Create the resource file and open it.
	HCreateResFile(fsspecPointer->vRefNum, fsspecPointer->parID, fsspecPointer->name);
	functionResult = ResError();
	if (functionResult != noErr) {
		goto bail;
	}
	resourceRefNumber = FSpOpenResFile( fsspecPointer, fsRdWrPerm);
	functionResult = ResError();
	if (functionResult != noErr) {
		goto bail;
	}
	closeResourceFile = true;
		
	// Create the empty dictionary data resource.
	dictDataResource = NewHandle(0);
	if (dictDataResource == nil) {
		functionResult = MemError();
		goto bail;
	}
	AddResource( dictDataResource, kDictDataType, kDictionaryResourceID, "\pDictionary Data");
	functionResult = ResError();
	if (functionResult != noErr) {
		disposeDictDataResource = true;
		goto bail;
	}
			
	// Create the dictionary information resource.
	// All fields except the keyAttributes are initialized to zero implicitely.
	dictInfoResource = NewHandleClear(sizeof(DictInfoRecord));
	if (dictInfoResource == nil) {
		functionResult = MemError();
		goto bail;
	}
	(**(DictInfoRecordHandle) dictInfoResource).formatNumber = kDictionaryFormatNumber;	// <13>
	(**(DictInfoRecordHandle) dictInfoResource).keyAttributes = keyAttributes;
	(**(DictInfoRecordHandle) dictInfoResource).scriptCode = scriptCode;				// <13>
	AddResource( dictInfoResource, kDictInfoType, kDictionaryResourceID, "\pDictionary Info");
	functionResult = ResError();
	if (functionResult != noErr) {
		disposeDictInfoResource = true;
		goto bail;
	}
			
bail:
	if (closeResourceFile)
		CloseResFile( resourceRefNumber);
	if (disposeDictDataResource)
		DisposHandle( dictDataResource);
	if (disposeDictInfoResource)
		DisposHandle( dictInfoResource);
		
	return functionResult;

}	// InitializeDictionary

/* ------------------------------------------------------------------------------------------
	•	Open the dictonary file which is referenced by dictionaryReference.
    
	•	The structure of dictionaryReference is private.  One should always pass in
		the value which is returned by the OpenDicitonary routine.
	
	•	<15> must call BTFlush after the BTOpen routine to save the changes, because dictionary
		may be opened in system mode and in that case nobody can't save the changes.
------------------------------------------------------------------------------------------ */

pascal	OSErr	OpenDictionary_ATrap( 	FSSpecPtr		fsspecPointer,
										SignedByte		accessPermission,
										LongInteger		*dictionaryReference )
{
	OSErr								functionResult = noErr;
	BTioParam							bTreeParameterBlock;
	Integer								resourceRefNumber = 0;
	Integer								savedResourceRefNumber = CurResFile();		// <11>
	Integer								ioRefNum = 0;
	LongInteger							ioBTRsrvUID = 0;
	DictInfoRecordHandle				dictInfoHandle = nil;
	DictReferenceRecordHandle			dictReferenceRecordHandle = nil;
	Boolean								closeBTree = false, closeResourceFile = false;
	
	// Set up B*Tree parameter block.
	bTreeParameterBlock.ioCompletion = nil;										
	bTreeParameterBlock.ioNamePtr = fsspecPointer->name;	
	bTreeParameterBlock.ioVRefNum = fsspecPointer->vRefNum;	
	bTreeParameterBlock.ioDirID = fsspecPointer->parID;	
	bTreeParameterBlock.ioBTKCProc = nil;										
	bTreeParameterBlock.ioPermssn = accessPermission;		

	// Open the BTree file.
	functionResult = BTOpen( &bTreeParameterBlock, false);									
	if (functionResult != noErr) {
		goto bail;
	}
	ioRefNum = bTreeParameterBlock.ioRefNum;
	
	// Flush the BTree file. <15>
	bTreeParameterBlock.ioCompletion = nil;										
	bTreeParameterBlock.ioBTWriteFlag = true;	
	functionResult = BTFlush( &bTreeParameterBlock, false);									
	if (functionResult != noErr) {
		goto bail;
	}

	// Get the BTree ioBTRsrvUID.
	functionResult = BTGetUid(&bTreeParameterBlock, false);
	if (functionResult != noErr) {
		closeBTree = true;
		goto bail;
	}
	ioBTRsrvUID = bTreeParameterBlock.ioBTRsrvUID;

	// Open the dictionary resource file with the map under the system resource.	<11>
	resourceRefNumber = OpenResFileUnderSystemMap( fsspecPointer, accessPermission ); 	// <12>
	UseResFile( resourceRefNumber);														// <12>
	functionResult = ResError();
	if (functionResult != noErr) {
		closeBTree = true;
		goto bail;
	}
	
	// Check if there are dictionary info and dictionary data resources. <13> <11>
	dictInfoHandle = (DictInfoRecordHandle) Get1Resource( kDictInfoType, kDictionaryResourceID);	
	if (dictInfoHandle == nil) {
		functionResult = ResError();
		closeBTree = true;
		closeResourceFile = true;
		goto bail;
	}
	if (Count1Resources( kDictDataType) != 1) {
		functionResult = notBTree;
		closeBTree = true;
		closeResourceFile = true;
		goto bail;
	}
	
	// Check if the dictionary format number is correct. <13>
	if ((**dictInfoHandle).formatNumber != kDictionaryFormatNumber) {
		functionResult = notBTree;
		closeBTree = true;
		closeResourceFile = true;
		goto bail;
	}
	
	// Set up the dictionary reference.
	dictReferenceRecordHandle = (DictReferenceRecordHandle) NewHandleClear(sizeof(DictReferenceRecord));
	if (dictReferenceRecordHandle == nil) {
		functionResult = MemError();
		closeBTree = true;
		closeResourceFile = true;
		goto bail;
	}

	// Initialize the dictReferenceRecord fields.
	(*dictReferenceRecordHandle)->fsspec = *fsspecPointer;
	(*dictReferenceRecordHandle)->dataRefNum = ioRefNum;	
	(*dictReferenceRecordHandle)->resourceRefNum = resourceRefNumber;	
	(*dictReferenceRecordHandle)->bTreeReservedUID = ioBTRsrvUID;		// this is needed by the BTree search and insert routines.
	(*dictReferenceRecordHandle)->cacheStartOffset = -1;
	(*dictReferenceRecordHandle)->previousIndex = -1;					// <17> initialize the index value.
	*dictionaryReference = (LongInteger)dictReferenceRecordHandle;

bail:
	if (closeBTree)
		functionResult = BTClose( &bTreeParameterBlock, false);	
	if (closeResourceFile)
		CloseResFileUnderSystemMap( resourceRefNumber);					// <14>
	UseResFile( savedResourceRefNumber);								// <11>
	
	return functionResult;
	
}	// OpenDictionary

/* ------------------------------------------------------------------------------------------
	•	Close the dictonary file which is referenced by dictionaryReference.
    
	•	The structure of dictionaryReference is private.  One should always pass in
		the value which is returned by the OpenDicitonary routine.
------------------------------------------------------------------------------------------ */

pascal 	OSErr 	CloseDictionary_ATrap( LongInteger dictionaryReference )
{
	OSErr								functionResult = noErr;
	BTioParam							bTreeParameterBlock;
	DictReferenceRecordHandle			dictReferenceRecordHandle = (DictReferenceRecordHandle) dictionaryReference;	// <11>
	Integer								resourceRefNumber = 0;
	Integer								savedResourceRefNumber = CurResFile();		// <11>

	// Get the dictionary resource file reference number.
	resourceRefNumber = (*dictReferenceRecordHandle)->resourceRefNum;	// <10>
	UseResFile( resourceRefNumber);										// <10>
	functionResult = ResError();										// <11>
	if (functionResult != noErr) {
		goto bail;
	}
	
	// Close the resource file under the system mode.
	CloseResFileUnderSystemMap( resourceRefNumber);						// <14>
	functionResult = ResError();										// <14>
	if (functionResult != noErr) {
		goto bail;
	}

	// Close the BTree file.
	bTreeParameterBlock.ioCompletion = nil;
	bTreeParameterBlock.ioRefNum = (*dictReferenceRecordHandle)->dataRefNum;		
	bTreeParameterBlock.ioBTRsrvUID = (*dictReferenceRecordHandle)->bTreeReservedUID;								
	functionResult = BTClose( &bTreeParameterBlock, false);										
	if (functionResult != noErr) {
		goto bail;
	}
		
	// Dispose the memory storage used by dictionary manager.
	DisposHandle((Handle) dictReferenceRecordHandle);	

bail:	
	UseResFile( savedResourceRefNumber);								// <11>
	return functionResult;

}	// CloseDictionary

/* -------------------------------------------------------------------------------------------
	•	Insert an entry into a dictionary which is referenced by "dictionaryReference".
		The data of the record is in "recordDataHandle".  There are three different insert modes.

				•insertOrReplace -- insert the entry if there is nothing in the dictionary which
									matches the key.  Or replace all the existing information in
									the dictionary which matches the key with the entry.
				•replace -- replace the existing information in the dictionary which matches
							the key with the entry.  If there is nothing in the dictionary which
							matches the key, a recordNotFoundErr is returned.
				•insert -- Only insert the entry if there is nothing in the dictionary which
						   matches the key.  If there is already something which matches the 
						   key, a recordAlreadyExistErr is returned.
						   
    •	An entry in "recordDataHandle" has the following format.

				------------------------------------------------------------------------------------------
				| Entry length(byte) | Raw data length(byte) | Raw data | Attribute 1 | .. | Attribute N |
				------------------------------------------------------------------------------------------
				
				Each attribute has the following format on entry:
				---------------------------------------------------------------------------
				| Attribute length(byte) | Attribute type(1 byte) | Other attribute data |
				---------------------------------------------------------------------------

	•	recordDataHandle should be valid upon entry.
		The length of the key has to equal to or less than 129 byte.
		
	•	dataBufferHandle (cache buffer) is not used for this routine.
------------------------------------------------------------------------------------------ */

pascal	OSErr	InsertRecordToDictionary_ATrap( 	LongInteger 		dictionaryReference,
													ConstStr255Param	key, 
													Handle				recordDataHandle, 
													InsertMode 			whichMode )
{
	OSErr							functionResult = noErr;
	DictReferenceRecordHandle		dictReferenceRecordHandle = (DictReferenceRecordHandle) dictionaryReference;	// <11>
	Size							dataHandleSize = 0;
	Size							savedOffset = 0;
	Size							garbageIncrease = 0;
	Size							keyBlockSize = 0;
	Ptr								keyPointer = nil;
	Handle							dictDataResource = nil;
	DictInfoRecordHandle			dictInfoHandle = nil;
	Integer							resourceRefNumber = 0;
	Integer							savedResourceRefNumber = CurResFile();		// <11>
	BTreeRecord						nodeRecord;
	BTioParam						bTreeParameterBlock;
	Boolean							recordExist = false;
	Boolean							hasGarbage = false, appendData = false;
	Boolean							disposeKeyPointer = false;

	// First check input parameters are valid.
	dataHandleSize = GetHandleSize( recordDataHandle);
	if (key[0] > kMaximumKeyLength) {
		functionResult = btKeyLenErr;
		goto bail;
	} else if ((whichMode < kInsert) || (whichMode > kInsertOrReplace)) {
		functionResult = unknownInsertModeErr;
		goto bail;
	} else if (dataHandleSize > kDataBufferSize) {
		functionResult = recordDataTooBigErr;
		goto bail;
	} 
	
	// set current resource file.		<10>
	resourceRefNumber = (*dictReferenceRecordHandle)->resourceRefNum;
	UseResFile( resourceRefNumber);
	functionResult = ResError();			// <11>
	if (functionResult != noErr) {	
		goto bail;	
	}

	// Get an empty dictionary data handle to write partial resource.
	SetResLoad(false);									// don't load any resource.
	dictDataResource = Get1Resource( kDictDataType, kDictionaryResourceID);
	SetResLoad(true);									// reset to always load.
	if (dictDataResource == nil) {
		functionResult = ResError();
		goto bail;
	}

	// Get the dictionary info	resource to update.
	dictInfoHandle = (DictInfoRecordHandle) Get1Resource( kDictInfoType, kDictionaryResourceID);	
	if (dictInfoHandle == nil) {
		functionResult = ResError();
		goto bail;
	}
	
	// Prepare the key string pointer.
	keyBlockSize = *(UnsignedBytePointer)key + 2;		// Two extra byte requires by BTree key descriptor.
	keyPointer = NewPtrClear( keyBlockSize);
	if (keyPointer == nil) {
		functionResult = MemError();
		goto bail;
	}
	disposeKeyPointer = true;
	*keyPointer = keyBlockSize -1;						// key length byte.
	BlockMove( (Ptr)key, keyPointer +1, keyBlockSize -1);

	// See if there is a node in the BTree which matches the key.
	bTreeParameterBlock.ioCompletion = nil;	
	bTreeParameterBlock.ioRefNum = (*dictReferenceRecordHandle)->dataRefNum;		
	bTreeParameterBlock.ioBuffer = (Ptr)&nodeRecord;					 						
	bTreeParameterBlock.ioReqCount = sizeof(BTreeRecord);
	bTreeParameterBlock.ioBTKeyPtr = keyPointer;
	bTreeParameterBlock.ioBTRsrvUID = (*dictReferenceRecordHandle)->bTreeReservedUID;
	bTreeParameterBlock.ioBTHint1 = 0;
	recordExist = (BTSearch( &bTreeParameterBlock, false) == noErr);
	
	// If error occured depending on insertion mode, bail out at this point.
	if (whichMode == kInsert) {
		if (recordExist) {
			functionResult = btDupRecErr;
			goto bail;
		}
	} else if (whichMode == kReplace) {
		if (!recordExist) {
			functionResult = btRecNotFnd;
			goto bail;
		}
	}

	// If there is a record which matches the key in the dictionary. If the current data size
	// in the record is the same as the new data size, we can store the new data into the same area.
	// Otherwise, we will get a new area for the new data at the end of the resource.
	if (recordExist) {
		garbageIncrease = nodeRecord.dataSize;
		appendData = hasGarbage = (garbageIncrease != dataHandleSize);
	} else 
		appendData = true;

	savedOffset = (**dictInfoHandle).dictDataLEOF;
	if (appendData) {
		
		// Update the logical and physical size of the dictionary resource.	
		if ( ((**dictInfoHandle).dictDataPEOF - (**dictInfoHandle).dictDataLEOF) < dataHandleSize) {
		
			SetResourceSize( dictDataResource, (**dictInfoHandle).dictDataPEOF + kDictDataClumpSize);
			functionResult = ResError();
			if (functionResult != noErr) {
				goto bail;
			}
			(**dictInfoHandle).dictDataPEOF += kDictDataClumpSize;
			(**dictInfoHandle).dictDataLEOF += dataHandleSize;
		} else {
			(**dictInfoHandle).dictDataLEOF += dataHandleSize;
		}
		nodeRecord.dataSize = dataHandleSize;
		nodeRecord.dataOffset = savedOffset;
	}
	
	// Write out the dictionary data to the dictionary resource file.
	HLock((Handle) recordDataHandle);
	WritePartialResource( dictDataResource, nodeRecord.dataOffset, *recordDataHandle, dataHandleSize);
	HUnlock((Handle) recordDataHandle);
	functionResult = ResError();
	if (functionResult != noErr) {
		(**dictInfoHandle).dictDataLEOF = savedOffset;
		goto bail;
	}
	
	// Insert the nodeRecord into the btree file.
	bTreeParameterBlock.ioCompletion = nil;	
	bTreeParameterBlock.ioRefNum = (*dictReferenceRecordHandle)->dataRefNum;		
	bTreeParameterBlock.ioBuffer = (Ptr)&nodeRecord;					 						
	bTreeParameterBlock.ioReqCount = sizeof(BTreeRecord);
	bTreeParameterBlock.ioBTKeyPtr = keyPointer;
	bTreeParameterBlock.ioBTRsrvUID = (*dictReferenceRecordHandle)->bTreeReservedUID;
	bTreeParameterBlock.ioBTHint1 = 0;
	switch (whichMode) {
	
		case kInsert:			// insert the entry into the dictionary.
			functionResult = BTInsRec( &bTreeParameterBlock, false);	
			break;
		case kReplace:			// replace existing data in dictionary with the entry.
			functionResult = BTReplRec( &bTreeParameterBlock, false);
			break;
		case kInsertOrReplace:	// insert or replace data with the entry in the dictionary.
			functionResult = BTSetRec( &bTreeParameterBlock, false);	
			break;
		default:
			break;
	}
	if (functionResult != noErr) {
		(**dictInfoHandle).dictDataLEOF = savedOffset;
		goto bail;
	}
	
	// Update the garbage size and invalidate the cache offset of the dictionaryReference record.
	if (hasGarbage) {
		(**dictInfoHandle).currentGarbageSize += garbageIncrease;
	}
	(*dictReferenceRecordHandle)->cacheStartOffset = -1;
	
	// Save the dictInfo resource changes into the resource file.
	ChangedResource((Handle) dictInfoHandle);
	UpdateResFile( resourceRefNumber);
	
bail:		
	if (disposeKeyPointer)
		DisposPtr(keyPointer);
	UseResFile( savedResourceRefNumber);	// <11>

	return functionResult;
	
}	// InsertRecordToDictionary

/* ------------------------------------------------------------------------------------------
	•	Removes a record with the key from the dicitonary.
    
	•	The structure of dictionaryReference is private.  One should always pass in
		the value which is returned by the OpenDicitonary routine.
------------------------------------------------------------------------------------------ */

pascal 	OSErr	DeleteRecordFromDictionary_ATrap(	LongInteger 		dictionaryReference,
													ConstStr255Param	key )
{
	OSErr							functionResult = noErr;  
	Size							keyBlockSize = 0;
	Ptr								keyPointer = nil;
	DictReferenceRecordHandle		dictReferenceRecordHandle = (DictReferenceRecordHandle) dictionaryReference;	// <11>
	DictInfoRecordHandle			dictInfoHandle = nil;
	Integer							resourceRefNumber = 0;
	Integer							savedResourceRefNumber = CurResFile();		// <11>
	BTioParam						bTreeParameterBlock;
	BTreeRecord						nodeRecord;
	Boolean							disposeKeyPointer = false;
	
	// First check if key is valid, i.e. less than 129 bytes.
	if (key[0] > kMaximumKeyLength) {
		functionResult = btKeyLenErr;
		goto bail;
	}

	// set current resource file.	<10>
	resourceRefNumber = (*dictReferenceRecordHandle)->resourceRefNum;
	UseResFile( resourceRefNumber);
	functionResult = ResError();			// <11>
	if (functionResult != noErr) {
		goto bail;
	}

	// Get the dictionary info	resource to update the resource file.
	dictInfoHandle = (DictInfoRecordHandle) Get1Resource( kDictInfoType, kDictionaryResourceID);	
	if (dictInfoHandle == nil) {
		functionResult = ResError();
		goto bail;
	}
	
	// Prepare the key string pointer.
	keyBlockSize = key[0]+2;
	keyPointer = NewPtr( keyBlockSize);
	if (keyPointer == nil) {
		functionResult = MemError();
		goto bail;
	}
	disposeKeyPointer = true;
	*keyPointer = keyBlockSize -1;		
	BlockMove( (Ptr)key, keyPointer +1, keyBlockSize -1);
	
	// Get the nodeRecord which has the index of the real data stored in the resource file.
	bTreeParameterBlock.ioCompletion = nil;	
	bTreeParameterBlock.ioRefNum = (*dictReferenceRecordHandle)->dataRefNum;		
	bTreeParameterBlock.ioBuffer = (Ptr)&nodeRecord;					 						
	bTreeParameterBlock.ioReqCount = sizeof(BTreeRecord);
	bTreeParameterBlock.ioBTKeyPtr = keyPointer;
	bTreeParameterBlock.ioBTRsrvUID = (*dictReferenceRecordHandle)->bTreeReservedUID;
	bTreeParameterBlock.ioBTHint1 = 0;
	functionResult = BTSearch( &bTreeParameterBlock, false);
	if (functionResult != noErr) {
		goto bail;
	}
	
	// Delete the nodeRecord by calling BTDelete.
	bTreeParameterBlock.ioCompletion = nil;	
	bTreeParameterBlock.ioRefNum = (*dictReferenceRecordHandle)->dataRefNum;		
	bTreeParameterBlock.ioBTKeyPtr = keyPointer;
	bTreeParameterBlock.ioBTRsrvUID = (*dictReferenceRecordHandle)->bTreeReservedUID;
	functionResult = BTDelete(&bTreeParameterBlock, false);
	if (functionResult != noErr) {
		goto bail;
	}
	
	// Increase the garbage size caused by the logically deleting an entry from the dictionary data resource.
	(**dictInfoHandle).currentGarbageSize += nodeRecord.dataSize;
	
	// Save the dictInfoRecord changes into the resource file.
	ChangedResource((Handle) dictInfoHandle);
	UpdateResFile( resourceRefNumber);

bail:
	if (disposeKeyPointer)
		DisposPtr(keyPointer);
	UseResFile( savedResourceRefNumber);	// <11>
	
	return functionResult;

}	// DeleteRecordFromDictionary

/* ------------------------------------------------------------------------------------------
	•	Search for a record in a dictionary referenced by "dictionaryReference" which
		matches the "key".  The caller can filter out some of the entries by specifying
		which kind of entries he wants the Dictionary Manager to return.  
		If "requestedAttributeTablePointer" is not nil, only the entries which contain 
		attributes in the "requestedAttributeTable" list will be returned.
		If "requestedAttributeTablePointer" is nil, only "raw data" will be returned.
		If "requestedAttributeTablePointer" is Ptr(-1), all data entries will be returned.
				
	•	Resulting "recordDataHandle" will contain requested data entries.
				
				The format of "requestedAttributeTable" is as follow:

				------------------------------------
				| Number of attribute IDs (1 Byte) |
				------------------------------------
				| Attribute ID 1 (1 byte)          |
				------------------------------------
				| ...							   |
				------------------------------------
				| Attribute ID N (1 byte) 		   |
				------------------------------------
				
    			"recordDataHandle" contains a collection of entries and each entry has the following format.
				Actually this is the same format as the internal structure of B*Tree node.
				------------------------------------------------------------------------------------------
				| Entry length(byte) | Raw data length(byte) | Raw data | Attribute 1 | .. | Attribute N |
				------------------------------------------------------------------------------------------
				
				Each attribute has the following format on entry:
				----------------------------------------------------------------------
				| Attribute length(byte) | Attribute ID(byte) | Other attribute data |
				----------------------------------------------------------------------
				
	•	The structure of dictionaryReference is private.  One should always pass in
		the value which is returned by the OpenDicitonary routine.
		"recordDataHandle" must be a valid handle on entry.
------------------------------------------------------------------------------------------ */

pascal 	OSErr	FindRecordInDictionary_ATrap( 	LongInteger 		dictionaryReference,
												ConstStr255Param	key,
												Ptr					requestedAttributeTablePointer,
												Handle				recordDataHandle )
{
	OSErr							functionResult = noErr;
	Handle							dictDataResource = nil;
	Size							keyBlockSize = 0;
	Ptr								keyPointer = nil;
	DictReferenceRecordHandle		dictReferenceRecordHandle = (DictReferenceRecordHandle) dictionaryReference;	// <11>
	UnsignedBytePointer				dataBufferPointer = nil;
	LongInteger						cacheStartOffset = 0;
	Integer							savedResourceRefNumber = CurResFile();		// <11>
	LongInteger						count = 0;									// <17>
	DictInfoRecordHandle			dictInfoHandle = nil;						// <17>
	BTreeRecord						nodeRecord;
	BTioParam						bTreeParameterBlock;
	Boolean							disposeKeyPointer = false;
	Boolean							unlockDictionaryReference = false;
	
	// First check if key is valid, i.e. less than 129 bytes.
	if (key[0] > kMaximumKeyLength) {
		functionResult = btKeyLenErr;
		goto bail;
	}

	// Prepare enough space for dataHandle.	
	SetHandleSize( recordDataHandle, kDataBufferSize);
	functionResult = MemError();
	if (functionResult != noErr) {
		goto bail;
	}
	
	// Prepare the key string pointer.
	keyBlockSize = *(UnsignedBytePointer)key + 2;
	keyPointer = NewPtr( keyBlockSize);	
	if (keyPointer == nil) {
		functionResult = MemError();
		goto bail;
	}
	disposeKeyPointer = true;
	*keyPointer = keyBlockSize -1;						// key length byte.
	BlockMove( (Ptr)key, keyPointer +1, keyBlockSize -1);

	// Get the nodeRecord which has the index of the real data stored in the resource file.
	bTreeParameterBlock.ioCompletion = nil;	
	bTreeParameterBlock.ioRefNum = (*dictReferenceRecordHandle)->dataRefNum;		
	bTreeParameterBlock.ioBuffer = (Ptr)&nodeRecord;					 						
	bTreeParameterBlock.ioReqCount = sizeof(BTreeRecord); 
	bTreeParameterBlock.ioBTKeyPtr = keyPointer;
	bTreeParameterBlock.ioBTRsrvUID = (*dictReferenceRecordHandle)->bTreeReservedUID;
	bTreeParameterBlock.ioBTHint1 = 0;
	functionResult = BTSearch(&bTreeParameterBlock, false);
	if (functionResult != noErr) {
		goto bail;
	}
	
	// set current resource file.	<10>
	UseResFile( (*dictReferenceRecordHandle)->resourceRefNum);
	functionResult = ResError();		// <11>
	if (functionResult != noErr) {
		goto bail;
	}

	// Lock down the dictionary reference handle and dereference the buffer. <11>
	HLock((Handle) dictReferenceRecordHandle);	
	unlockDictionaryReference = true;
	dataBufferPointer = (*dictReferenceRecordHandle)->dataBuffer;

	// If data is not in the cache, load from the disk.
	cacheStartOffset = (*dictReferenceRecordHandle)->cacheStartOffset;
	if ((cacheStartOffset == -1) || (cacheStartOffset > nodeRecord.dataOffset) || 
		((nodeRecord.dataOffset + nodeRecord.dataSize) > (cacheStartOffset + kDataBufferSize))) {
	
		// <17> Adjust the byte count to read by looking logical end of file.
		dictInfoHandle = (DictInfoRecordHandle) Get1Resource( kDictInfoType, kDictionaryResourceID);	
		if (dictInfoHandle == nil) {
			functionResult = ResError();
			goto bail;
		}
		cacheStartOffset = nodeRecord.dataOffset;	// update the offset.
		if ((**dictInfoHandle).dictDataLEOF > (cacheStartOffset +kDataBufferSize))
			count = kDataBufferSize;
		else
			count = (**dictInfoHandle).dictDataLEOF - cacheStartOffset;

		// Get the dictionary data into the buffer.
		SetResLoad(false);									// set to don't load.
		dictDataResource = Get1Resource( kDictDataType, kDictionaryResourceID);
		SetResLoad(true);									// reset to always load.
		if (dictDataResource == nil) {
			functionResult = ResError();
			goto bail;
		}
		ReadPartialResource( dictDataResource, cacheStartOffset, dataBufferPointer, count);	// <17>
		functionResult = ResError();
		if (functionResult != noErr) {
			goto bail;
		}
		(*dictReferenceRecordHandle)->cacheStartOffset = cacheStartOffset;	// Update the cacheStartOffset.
	} else {

		dataBufferPointer += (nodeRecord.dataOffset - cacheStartOffset);
	}

	// Get only requested attributes from the dataBufferPointer and copy the result data into the recordDataHandle.
	functionResult = GetRequestedAttributes( dataBufferPointer, nodeRecord.dataSize, requestedAttributeTablePointer, recordDataHandle);
	
bail:
	if (unlockDictionaryReference)						// <11>
		HUnlock((Handle) dictReferenceRecordHandle);	// <11>
	if (disposeKeyPointer)
		DisposPtr(keyPointer);
	if (functionResult != noErr)
		SetHandleSize( recordDataHandle, 0);		
	UseResFile( savedResourceRefNumber);				// <11>
	
	return functionResult;

}	// FindRecordInDictionary

/* ------------------------------------------------------------------------------------------
	•	Search for a record in a dictionary referenced by "dictionaryReference" which
		matches the "key".  The caller can filter out some of the entries by specifying
		which kind of entries he wants the Dictionary Manager to return.  
		If "requestedAttributeTablePointer" is not nil, only the entries which contain 
		attributes in the "requestedAttributeTable" list will be returned.
		If "requestedAttributeTablePointer" is nil, only "raw data" will be returned.
		If "requestedAttributeTablePointer" is Ptr(-1), all data entries will be returned.
				
		Resulting "recordDataHandle" will contain requested data entries.
				
				The format of "requestedAttributeTable" is as follow:

				------------------------------------
				| Number of attribute IDs (1 Byte) |
				------------------------------------
				| Attribute ID 1 (1 byte)          |
				------------------------------------
				| ...							   |
				------------------------------------
				| Attribute ID N (1 byte) 		   |
				------------------------------------
				
    			"recordDataHandle" contains a collection of entries and each entry has the following format.
				Actually this is the same format as the internal structure of B*Tree node.
				------------------------------------------------------------------------------------------
				| Entry length(byte) | Raw data length(byte) | Raw data | Attribute 1 | .. | Attribute N |
				------------------------------------------------------------------------------------------
				
				Each attribute has the following format on entry:
				----------------------------------------------------------------------
				| Attribute length(byte) | Attribute ID(byte) | Other attribute data |
				----------------------------------------------------------------------
				
	•	The structure of dictionaryReference is private.  One should always pass in
		the value which is returned by the OpenDicitonary routine.
		"recordDataHandle" must be a valid handle on entry.
------------------------------------------------------------------------------------------ */

pascal 	OSErr	FindRecordByIndexInDictionary_ATrap( 	LongInteger dictionaryReference,
														LongInteger	recordIndex,
														Ptr			requestedAttributeTablePointer,
														Str255		recordKey,
														Handle		recordDataHandle )
{
	OSErr							functionResult = noErr;
	Handle							dictDataResource = nil;
	Ptr								keyPointer = nil;
	DictReferenceRecordHandle		dictReferenceRecordHandle = (DictReferenceRecordHandle) dictionaryReference;	// <11>
	UnsignedBytePointer				dataBufferPointer = nil;
	Integer							returnedKeySize = 0;
	Integer							savedResourceRefNumber = CurResFile();		// <11>
	LongInteger						cacheStartOffset = 0;
	LongInteger						difference = 0;
	LongInteger						remaining = 0, count = 0;					// <17>
	DictInfoRecordHandle			dictInfoHandle = nil;						// <17>
	BTreeRecord						nodeRecord;
	BTioParam						bTreeParameterBlock;
	Boolean							disposeKeyPointer = false;
	Boolean							unlockDictionaryReference = false;
	
	// First check if index is valid, i.e. 0 =< recordIndex.
	if (recordIndex < 0) {
		functionResult = invalidIndexErr;
		goto bail;
	} 

	// Prepare enough space for dataHandle.	
	SetHandleSize( recordDataHandle, kDataBufferSize);
	functionResult = MemError();
	if (functionResult != noErr) {
		goto bail;
	}
	
	// Prepare key string.	
	keyPointer = NewPtr( kMaximumKeyLength +1);	
	if (keyPointer == nil) {
		functionResult = MemError();
		goto bail;
	}
	disposeKeyPointer = true;
	
	// <17> check if this is the first FindRecordByIndex call after the OpenDictionary call.
	if ((*dictReferenceRecordHandle)->previousIndex == -1) {
resetHint:
		bTreeParameterBlock.ioBTHint1 = 0;
		bTreeParameterBlock.ioBTHint2 = 0;
		bTreeParameterBlock.ioBTHint3 = 0;	
		bTreeParameterBlock.ioBTHint4 = 0;
		bTreeParameterBlock.ioBTHint5 = 0;

		if (recordIndex > 0x7FFF) {		// if record index is bigger than 32K, loop later.	
			bTreeParameterBlock.ioBTPosMode = 0x7FFF;
			remaining = recordIndex - 0x7FFF;
		} else {
			bTreeParameterBlock.ioBTPosMode = (Integer) recordIndex;
			remaining = 0;
		}
	} else {	// This is not the first time so that we are using the hint.
		bTreeParameterBlock.ioBTHint1 = (*dictReferenceRecordHandle)->ioBTHint1;
		bTreeParameterBlock.ioBTHint2 = (*dictReferenceRecordHandle)->ioBTHint2;
		bTreeParameterBlock.ioBTHint3 = (*dictReferenceRecordHandle)->ioBTHint3;
		bTreeParameterBlock.ioBTHint4 = (*dictReferenceRecordHandle)->ioBTHint4;
		bTreeParameterBlock.ioBTHint5 = (*dictReferenceRecordHandle)->ioBTHint5;
		difference = recordIndex - (*dictReferenceRecordHandle)->previousIndex;
		if (difference > 0x7FFF) {
			bTreeParameterBlock.ioBTPosMode = 0x7FFF;
			remaining = difference - 0x7FFF;
		} else {
			bTreeParameterBlock.ioBTPosMode = (Integer) difference;
			remaining = 0;
		}
	}
	
	// Get data (both key and entry data) from the dictionary.
	bTreeParameterBlock.ioCompletion = nil;	
	bTreeParameterBlock.ioRefNum = (*dictReferenceRecordHandle)->dataRefNum;		
	bTreeParameterBlock.ioBuffer = (Ptr)&nodeRecord;					 						
	bTreeParameterBlock.ioReqCount = sizeof(BTreeRecord); 
	bTreeParameterBlock.ioBTKeyPtr = keyPointer;
	bTreeParameterBlock.ioKReqCount = kMaximumKeyLength +1;
	bTreeParameterBlock.ioBTRsrvUID = (*dictReferenceRecordHandle)->bTreeReservedUID;
	
	functionResult = BTGetRec(&bTreeParameterBlock, false);
	if (functionResult == btPMInvalid) {
		// if we have the InsertRecord or DeleteRecord before this call,
		// the hint in BTree data structure is not valid and go back to the resetHint.
		goto resetHint;
	}
	if (functionResult != noErr) {
		// Set possible BTree errors except notBTree and btRecNotFnd to invalidIndexErr. <8> hsK
		if ((functionResult != notBTree) && (functionResult != btRecNotFnd))
			functionResult = invalidIndexErr;
		goto bail;
	}

	// <17> hint is in the param block of the current stack.
	while (remaining > 0) {
		if (remaining > 0x7FFF) {
			bTreeParameterBlock.ioBTPosMode = 0x7FFF;
			remaining -= 0x7FFF;
		} else {
			bTreeParameterBlock.ioBTPosMode = (Integer) remaining;
			remaining = 0;
		}
		functionResult = BTGetRec(&bTreeParameterBlock, false);
		if (functionResult != noErr) {
			if ((functionResult != notBTree) && (functionResult != btRecNotFnd))
				functionResult = invalidIndexErr;
			goto bail;
		}
	}
	
	// <17> save the BTree hints and the last index.
	(*dictReferenceRecordHandle)->previousIndex = recordIndex;
	(*dictReferenceRecordHandle)->ioBTHint1 = bTreeParameterBlock.ioBTHint1;
	(*dictReferenceRecordHandle)->ioBTHint2 = bTreeParameterBlock.ioBTHint2;
	(*dictReferenceRecordHandle)->ioBTHint3 = bTreeParameterBlock.ioBTHint3;	
	(*dictReferenceRecordHandle)->ioBTHint4 = bTreeParameterBlock.ioBTHint4;
	(*dictReferenceRecordHandle)->ioBTHint5 = bTreeParameterBlock.ioBTHint5;
	
	// Copy keyPointer into recordKey.
	returnedKeySize = bTreeParameterBlock.ioKActCount;
	BlockMove((Ptr)(keyPointer +1), (Ptr)recordKey, returnedKeySize-1);

	// set current resource file.	<10>
	UseResFile( (*dictReferenceRecordHandle)->resourceRefNum);
	functionResult = ResError();		// <11>
	if (functionResult != noErr) {
		goto bail;
	}

	// Lock down the dictionary reference handle and dereference the buffer. <11>
	HLock((Handle) dictReferenceRecordHandle);	
	unlockDictionaryReference = true;
	dataBufferPointer = (*dictReferenceRecordHandle)->dataBuffer;

	// If data is not in the cache, load from the disk.
	cacheStartOffset = (*dictReferenceRecordHandle)->cacheStartOffset;
	if ((cacheStartOffset == -1) || (cacheStartOffset > nodeRecord.dataOffset) || 
		((nodeRecord.dataOffset + nodeRecord.dataSize) > (cacheStartOffset + kDataBufferSize))) {

		// <17> Adjust the byte count to read by looking logical end of file.
		dictInfoHandle = (DictInfoRecordHandle) Get1Resource( kDictInfoType, kDictionaryResourceID);	
		if (dictInfoHandle == nil) {
			functionResult = ResError();
			goto bail;
		}
		cacheStartOffset = nodeRecord.dataOffset;			// update the offset.
		if ((**dictInfoHandle).dictDataLEOF > (cacheStartOffset +kDataBufferSize))
			count = kDataBufferSize;
		else
			count = (**dictInfoHandle).dictDataLEOF - cacheStartOffset;

		// Get the dictionary data into the buffer.
		SetResLoad(false);									// set to don't load.
		dictDataResource = Get1Resource( kDictDataType, kDictionaryResourceID);
		SetResLoad(true);									// reset to always load.
		if (dictDataResource == nil) {
			functionResult = ResError();
			goto bail;
		}
		ReadPartialResource( dictDataResource, cacheStartOffset, dataBufferPointer, count);	// <17>
		functionResult = ResError();
		if (functionResult != noErr) {
			goto bail;
		}
		(*dictReferenceRecordHandle)->cacheStartOffset = cacheStartOffset;	// Update the cacheStartOffset.
	} else {

		dataBufferPointer += (nodeRecord.dataOffset - cacheStartOffset);
	}

	// Get only requested attributes from the dataBufferPointer and copy the result data into the recordDataHandle.
	functionResult = GetRequestedAttributes( dataBufferPointer, nodeRecord.dataSize, requestedAttributeTablePointer, recordDataHandle);
	
bail:
	if (unlockDictionaryReference)						// <11>
		HUnlock((Handle) dictReferenceRecordHandle);	// <11>
	if (disposeKeyPointer)
		DisposPtr(keyPointer);
	if (functionResult != noErr)
		SetHandleSize( recordDataHandle, 0);		
	UseResFile( savedResourceRefNumber);				// <11>
	
	return functionResult;

}	// FindRecordByIndexInDictionary

/* ------------------------------------------------------------------------------------------
	•	Get useful dictionary information which are defined as DictionaryInformation.
		Currently returns number of entries( records), max key size, current garabge size,
		and max garbage size.
------------------------------------------------------------------------------------------ */

pascal 	OSErr	GetDictionaryInformation_ATrap( 	LongInteger dictionaryReference,
													DictionaryInformation *dictionaryInformation)
{
	OSErr							functionResult = noErr;
	UnsignedByte					keyAttributes = 0;
	UnsignedByte					keyDescriptorRecord[4];
	DictReferenceRecordHandle		dictReferenceRecordHandle = (DictReferenceRecordHandle) dictionaryReference;	// <11>
	BTParam							bTreeParameterBlock;
	Integer							savedResourceRefNumber = CurResFile();		// <11>
	DictInfoRecordHandle			dictInfoHandle = nil;						// <11>

	// set current resource file.	<10>
	UseResFile( (*dictReferenceRecordHandle)->resourceRefNum);
	functionResult = ResError();			// <11>
	if (functionResult != noErr) {
		goto bail;
	}

	// Get the dictionary info	resource to update the resource file.
	dictInfoHandle = (DictInfoRecordHandle) Get1Resource( kDictInfoType, kDictionaryResourceID);	
	if (dictInfoHandle == nil) {
		functionResult = ResError();
		goto bail;
	}
	
	// Get dictionary information.
	keyAttributes = (**dictInfoHandle).keyAttributes;
	bTreeParameterBlock.ioCompletion = nil;
	bTreeParameterBlock.ioRefNum = (*dictReferenceRecordHandle)->dataRefNum;		
	bTreeParameterBlock.ioBTKDReqCount = 4; 
	keyDescriptorRecord[0] = 3;					// number of bytes in the record.
	keyDescriptorRecord[1] = kdString;			// key is a Pascal string.
	keyDescriptorRecord[2] = 1;					// occurrence.
	keyDescriptorRecord[3] = keyAttributes;
	bTreeParameterBlock.ioBTKDPtr = (Ptr)keyDescriptorRecord;
	functionResult = BTGetInfo(&bTreeParameterBlock, false);
	if (functionResult != noErr) {
		goto bail;
	}
	
	// return the dictionary information record.
	dictionaryInformation->dictionaryFSSpec = (*dictReferenceRecordHandle)->fsspec;
	dictionaryInformation->numberOfRecords = bTreeParameterBlock.ioBTRecNum;
	dictionaryInformation->currentGarbageSize = (**dictInfoHandle).currentGarbageSize;
	dictionaryInformation->script = (**dictInfoHandle).scriptCode;					// <15><13>
	dictionaryInformation->maximumKeyLength = bTreeParameterBlock.ioBTMaxKLen;
	dictionaryInformation->keyAttributes = keyAttributes;

bail:
	UseResFile( savedResourceRefNumber);	// <11>
	return functionResult;

}	// GetDictionaryInformation
    
/* ------------------------------------------------------------------------------------------
	•	Compact the dictionary by creating a brand-new dictionary and inserting all entries
		of the existing dictionary into the new dictionary.
------------------------------------------------------------------------------------------ */

pascal OSErr CompactDictionary_ATrap( LongInteger dictionaryReference )
{
	OSErr							functionResult = noErr;
	DictInfoRecordHandle			dictInfoHandle = nil;
	Handle							recordDataHandle = nil;				
	Integer							index = 0;
	Str255							recordKey = "";
	DictionaryInformation			oldDictionaryInformation;					// <11>
	FSSpec							oldDictionaryFsspec, newDictionaryFsspec;	// <11>	
	DictReferenceRecordHandle		dictReferenceRecordHandle = (DictReferenceRecordHandle) dictionaryReference;	// <11>
	DictReferenceRecordHandle		temporaryDictionaryReferenceHandle = nil;	// <11>
	LongInteger						newDictionaryReference = 0;					// <11>
	Integer							savedResourceRefNumber = CurResFile();		// <11>
	FInfo							finderInfo;									// <11>
	LongInteger						numberOfRecords = 0;						// <11>
	Boolean							disposeRecordDataHandle = false;

	// set current resource file. <11>
	UseResFile( (*dictReferenceRecordHandle)->resourceRefNum);
	functionResult = ResError();
	if (functionResult != noErr) {
		goto bail;
	}

	// Get the dictionary info	resource to update the resource file. <11>
	dictInfoHandle = (DictInfoRecordHandle) Get1Resource( kDictInfoType, kDictionaryResourceID);	
	if (dictInfoHandle == nil) {
		functionResult = ResError();
		goto bail;
	}
	
	// If currentGarbageSize is zero, we don't need to compact.
	if ((**dictInfoHandle).currentGarbageSize <= 0) {
		goto bail;
	}
	
	// Prepare the space to copy each record data in the will-be-compact dictionary.
	recordDataHandle = NewHandleClear( kDataBufferSize);
	if (recordDataHandle == nil) {
		functionResult = MemError();
		goto bail;
	}
	disposeRecordDataHandle = true;
	
	// Create a new file in the current directory with the same finder info of the original dictionary. <11>
	oldDictionaryFsspec = (*dictReferenceRecordHandle)->fsspec;
	newDictionaryFsspec.vRefNum = oldDictionaryFsspec.vRefNum;
	newDictionaryFsspec.parID = oldDictionaryFsspec.parID;
	newDictionaryFsspec.name[0] = 1;
	newDictionaryFsspec.name[1] = 0x0a;	// just a weird name that cannot be typed by the user.
	functionResult = HGetFInfo( oldDictionaryFsspec.vRefNum, oldDictionaryFsspec.parID, 
								oldDictionaryFsspec.name, &finderInfo);
	functionResult = HCreate( newDictionaryFsspec.vRefNum, newDictionaryFsspec.parID, newDictionaryFsspec.name,
								finderInfo.fdCreator, finderInfo.fdType);
	if (functionResult == dupFNErr) {
		functionResult = FSpDelete( &newDictionaryFsspec);
		if (functionResult != noErr) {
			goto bail;
		}
		functionResult = HCreate( newDictionaryFsspec.vRefNum, newDictionaryFsspec.parID, newDictionaryFsspec.name,
									finderInfo.fdCreator, finderInfo.fdType);
		if (functionResult != noErr) {
			goto bail;
		}
	}
	functionResult = HSetFInfo( newDictionaryFsspec.vRefNum, newDictionaryFsspec.parID, 
								newDictionaryFsspec.name, &finderInfo);

	// Install dictionary structure into the new file and open it. <11>
	functionResult = GetDictionaryInformation_ATrap( dictionaryReference, &oldDictionaryInformation);
	if (functionResult != noErr) {
		goto bail;
	}
	functionResult = InitializeDictionary_ATrap( &newDictionaryFsspec, 
												oldDictionaryInformation.maximumKeyLength, 
												oldDictionaryInformation.keyAttributes,
												(**dictInfoHandle).scriptCode);
	if (functionResult != noErr) {
		goto bail;
	}
	functionResult = OpenDictionary_ATrap( &newDictionaryFsspec, fsRdWrPerm, &newDictionaryReference);
	if (functionResult != noErr) {
		goto bail;
	}
		
	// Get all the records from the backup dictionary and copy them into the original. <11>
	numberOfRecords = oldDictionaryInformation.numberOfRecords;
	for (index = 0; (index < numberOfRecords); index++) {
	
		functionResult = FindRecordByIndexInDictionary_ATrap( dictionaryReference, index, (Ptr)-1, recordKey, recordDataHandle);
		if (functionResult != noErr) {
			goto bail;
		}
		functionResult = InsertRecordToDictionary_ATrap( newDictionaryReference, recordKey, recordDataHandle, kInsertOrReplace);
		if (functionResult != noErr) {
			goto bail;
		}
	}

	// Close and delete the current (old) dictionary file. <11>
	temporaryDictionaryReferenceHandle = (DictReferenceRecordHandle) NewHandleClear(sizeof(DictReferenceRecord));
	if (temporaryDictionaryReferenceHandle == nil) {
		functionResult = MemError();
		goto bail;
	}
	**temporaryDictionaryReferenceHandle = **dictReferenceRecordHandle;
	functionResult = CloseDictionary_ATrap( (LongInteger) temporaryDictionaryReferenceHandle);
	if (functionResult != noErr) {
		goto bail;
	}
	functionResult = FSpDelete( &oldDictionaryFsspec);
	
	// Update dictionaryReference and rename the new dictionary. <11>
	**dictReferenceRecordHandle = **(DictReferenceRecordHandle) newDictionaryReference;
	(**dictReferenceRecordHandle).fsspec = oldDictionaryFsspec;		// copy the file name.
	functionResult = HRename( newDictionaryFsspec.vRefNum, newDictionaryFsspec.parID, 
								newDictionaryFsspec.name, oldDictionaryFsspec.name);

bail:
	if (disposeRecordDataHandle)
		DisposHandle( recordDataHandle);
	UseResFile( savedResourceRefNumber);	// <11>
	
	return functionResult;

}	// CompactDictionary

/* ------------------------------------------------------------------------------------------
	•	Input is the sourceBufferPointer with the requestedAttributeTablePointer.
		Depending on the requestedAttributeTable, copy data into the output targetHandle.
				
			If "requestedAttributeTablePointer" is not nil, only the entries which contain 
				attributes in the "requestedAttributeTable" list will be returned.
			If "requestedAttributeTablePointer" is nil, only "raw data" will be returned.
			If "requestedAttributeTablePointer" is Ptr(-1), all data entries will be returned.
	
	•	This routine moves heap memory.
		Upon entry targetHandle size was set to kDataBufferSize (currently 1024 bytes). 
------------------------------------------------------------------------------------------ */

pascal 	OSErr	GetRequestedAttributes( const UnsignedBytePointer	sourceBufferPointer,
										Size						sourceBufferSize,
										const Ptr					requestedAttributeTablePointer,
										Handle						targetHandle)
{
	OSErr			functionResult = noErr;

	switch ((LongInteger) requestedAttributeTablePointer) {

		case ( -1):
			functionResult = CopySourceWithAllAttributes( sourceBufferPointer, sourceBufferSize, targetHandle);
			break;
		case ( 0):
			functionResult = CopySourceWithoutAttributes( sourceBufferPointer, targetHandle);
			break;
		default:
			functionResult = CopySourceUsingAttributeTable( sourceBufferPointer, requestedAttributeTablePointer, targetHandle);
			break;
	}
	
	return functionResult;
	
}	// GetRequestedAttributes

/* ------------------------------------------------------------------------------------------
	•	Copy the original source which also has all attributes.
------------------------------------------------------------------------------------------ */

pascal OSErr CopySourceWithAllAttributes( 	const UnsignedBytePointer	sourceBufferPointer,
											Size						sourceBufferSize,
											Handle						targetHandle)
{
	OSErr			functionResult = noErr;
	
	BlockMove( sourceBufferPointer, *targetHandle, sourceBufferSize);
	SetHandleSize( targetHandle, sourceBufferSize); 
	functionResult = MemError();

	return functionResult;
	
}	// CopySourceWithAllAttributes

/* ------------------------------------------------------------------------------------------
	•	Copy only raw data from the original source.
------------------------------------------------------------------------------------------ */

pascal OSErr CopySourceWithoutAttributes( 	UnsignedBytePointer		sourceBufferPointer,
											Handle					targetHandle)
{
	OSErr									functionResult = noErr;
	Integer									totalNumberOfEntries = 0;
	Integer									i = 0;
	Integer									numberOfBytesToSkip = 0, rawDataSize = 0;
	Size									targetHandleSize = 0;
	UnsignedBytePointer						targetPointer = nil;
	
	targetPointer = (UnsignedBytePointer)*targetHandle;
	totalNumberOfEntries = *sourceBufferPointer;
	
	*targetPointer = totalNumberOfEntries;		// Copy number of entries into target.
	sourceBufferPointer++;						// Move to the first entry in  source.
	targetPointer++;							// Move to the first entry in target.			
	
	for ( i = 0; i < totalNumberOfEntries; i++) {

		numberOfBytesToSkip	 = *sourceBufferPointer;	// Get source entry length.
		sourceBufferPointer++;							// Move to raw data size byte of the source.
		rawDataSize = *sourceBufferPointer +1;			// Raw data size plus one (raw data entry byte size).
		*targetPointer = (UnsignedByte) rawDataSize;					// Set target entry length.
		targetPointer++;												// Move to raw data size byte of the target.
		BlockMove( sourceBufferPointer, targetPointer, rawDataSize);	// Copy raw data including entry length.
		sourceBufferPointer += numberOfBytesToSkip;
		targetPointer += rawDataSize;
	}

	targetHandleSize = ((Ptr)targetPointer - *targetHandle);	// Get target handle size.
	SetHandleSize( targetHandle, targetHandleSize); 
	functionResult = MemError();

	return functionResult;
	
}	// CopySourceWithoutAttributes

/* ------------------------------------------------------------------------------------------
	•	Copy the raw data with selected attributes.
------------------------------------------------------------------------------------------ */

pascal OSErr CopySourceUsingAttributeTable( UnsignedBytePointer		sourceBufferPointer,
											const Ptr				requestedAttributeTablePointer,
											Handle					targetHandle)
{
	OSErr									functionResult = noErr;
	Integer									attributeID = 0, attributeLength = 0;
	Integer									totalNumberOfEntries = 0, numberOfTargetEntries = 0;
	Integer									i = 0, j = 0;
	Integer									rawDataSize = 0;
	Integer									numberOfAttributes = 0;
	Integer									sourceEntryLength = 0;
	Size									targetHandleSize = 0;
	UnsignedBytePointer						targetPointer = nil;
	UnsignedBytePointer						sourceEntryLengthPointer = nil, targetEntryLengthPointer = nil;
	UnsignedBytePointer						sourceRawDataPointer = nil, targetRawDataPointer = nil;
	Ptr										tempPointer = nil;
	Boolean									foundRequestedAttribute = false;
	Boolean									foundAtLeastOneAttribute = false;	// <19>
		
	targetPointer = (UnsignedBytePointer)*targetHandle;
	totalNumberOfEntries = *sourceBufferPointer;

	numberOfAttributes = (UnsignedByte) *requestedAttributeTablePointer;	// <8> hsK
	
	numberOfTargetEntries = 0;					// Initially zero of entries in targetHandle.
	sourceBufferPointer++;						// Move to the first entry in source.
	targetPointer++;							// Move to the first entry in target.
	
	// Copy the needed attributes along with raw data from the source buffer to targetHandle.
	for ( i = 0; i < totalNumberOfEntries; i++) {

		// Mark source/targetEntryLengthPointers.
		sourceEntryLengthPointer = sourceBufferPointer;
		targetEntryLengthPointer = targetPointer;
		sourceEntryLength = *sourceEntryLengthPointer;
		
		// Move to the raw data size byte.
		sourceBufferPointer++;
		targetPointer++;
		
		// Reserve raw data space in the targetHandle.
		sourceRawDataPointer = sourceBufferPointer;
		targetRawDataPointer = targetPointer;

		// Move to the first attribute.
		rawDataSize = *sourceRawDataPointer +1;
		sourceBufferPointer += rawDataSize;
		targetPointer += rawDataSize;
		
		// Copy only requested attribute ID's into the targetHandle.
		while ( (sourceEntryLength +1) > (sourceBufferPointer - sourceEntryLengthPointer)) {

			attributeID = (Integer) *(Ptr)(sourceBufferPointer +1);	// Get source attribute ID.
			tempPointer = requestedAttributeTablePointer +1;		// Points the first attribute.
			foundRequestedAttribute = false;
			j = 0;
			
			while ((!foundRequestedAttribute) && (j < numberOfAttributes)) {

				if ((Integer)*tempPointer == attributeID) {
					foundRequestedAttribute = true;
				} else {
					tempPointer++;
					j++;
				}
			}
			if (foundRequestedAttribute) {
			
				*targetPointer = *sourceBufferPointer;			// target attribute length will be the same as the source.
				attributeLength = *sourceBufferPointer +1;
				BlockMove( sourceBufferPointer, targetPointer, attributeLength);	// Copy the attribute.
				sourceBufferPointer += attributeLength;			// Move to the next attribute field in source.
				targetPointer += attributeLength;				// Move to the next attribute field in target.
				foundAtLeastOneAttribute = true;				// <19>
			} else {
			
				sourceBufferPointer += *sourceBufferPointer +1;	// Move only source pointer to the next attribute.
			}
		}
		
		// Copy the raw data into the target and set sourceBufferPointer to next entry.
		if (foundAtLeastOneAttribute) {		// <19>
		
			BlockMove( sourceRawDataPointer, targetRawDataPointer, rawDataSize); 
			*targetEntryLengthPointer = (targetPointer - targetEntryLengthPointer -1);	// Fill the entry length byte.
			numberOfTargetEntries++;
		} else {
		
			targetPointer = targetEntryLengthPointer;
		}
		
		foundAtLeastOneAttribute = false;	// <19>
		
	}	// for
	
	// Fill the total number of entries in byte.
	*((UnsignedBytePointer)*targetHandle) = (UnsignedByte) numberOfTargetEntries;
	
	targetHandleSize = ((Ptr)targetPointer - *targetHandle);	// Get target handle size.
	SetHandleSize( targetHandle, targetHandleSize); 

	return functionResult;
	
}	// CopySourceUsingAttributeTable
