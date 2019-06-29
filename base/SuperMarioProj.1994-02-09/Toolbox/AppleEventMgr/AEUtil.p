{
	File:		AEUtil.p

	Copyright:	© 1990, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 3/23/92	JSM		OSEvents.p is obsolete, use Events.p instead.
		 <5>	 5/21/91	gbm		Stop using the "empty" units (MemTypes, etc...)
		 <4>	 1/29/91	LN		took out keySelectProc - defined in AppleEvents.p
		 <3>	 1/11/91	Lai		msgAddrTarget is union of the 4 eppc address type rather than
									just TargetID
		 <2>	12/14/90	bbm		(Lai) fix duplicate defines.
		 <1>	12/13/90	Lai		first checked in
}

UNIT AEUtil;

INTERFACE

USES
	Types, Memory, Events, Errors,
	AppleEvents, AEHashTable;

CONST
	{ Warning:	Be sure to check that this is up to date with SysPrivateEqu.a }
	kAEAppGlobalOffset	= 340;
	kAESysGlobalOffset	= 380;
	ExpandMem			= $2B6;

	kAskResetTimer 		= 1990;
	kNoneOfYourBusiness	= 1991;
	
	kAevtSicnID			= -5888; { id for the AppleEvent sicn }

	
	roundUpMask			= -2;
	RoundUpValue		= 1;

	typeMeta			= 'meta';
	kMhdrType			= 'mhdr';
	kLhdrType			= 'lhdr';
	aeEndOfMetaDataKeyword = ';;;;';
	aeRecTypeKeyword	= 'type';
	
	kInteractLevelKeyword = 'inte';
	kReplyRequestedKeyword = 'repq';
	kNeverInteract = 'nevi';
	kCanInteract	= 'cani';
	kCanSwitchIfInteract = 'cans';
	kAlwaysInteract = 'alwi';
	kAlwaysSwitchIfInteract = 'alws';
	
	{ event ID for reset timeout }
	kAEWaitLonger		= 'wait';
	
	keyBeforeTime		= 'befo';
	
	{ the keyword for the blocking and unblocking special handler }
	keyBlockingProc		= 'blck';
	keyUnblockProc		= 'unbk';
	keyDiposeTokenProc	= 'xtok';
	keyCmprProc			= 'cmpr';
	keyContProc			= 'cont';
	keyMkidProc			= 'mkid';
	keyMarkProc			= 'mark';
	keyAdjmProc			= 'adjm';
	
	kAERefconAttribute = 'refc';
	kVersionOne			= $00010001;
	
	
	standardTimeOut		= 3600;					{ this is the value of standard timeout, one day
													we may change this to a function }

TYPE
	AENotifyRec 		= RECORD
		theNMRec:			NMRec;
		posted: 			Boolean;
		END;
	AENotifyRecPtr		= ^AENotifyRec;
	
	ListClassRec		= RECORD
		hintIndex:			LONGINT;
		hintOffset: 		LONGINT;
		ParamBegin: 		LONGINT;
		metaCountOrObjType:	LONGINT;
		END;		

	CommonHeader		= RECORD
		theKeyword: 		AEKeyWord;
		theType:			DescType;
		theLength:			LONGINT;
		END;
	CommonHeaderPointer = ^CommonHeader;
	CommonHeaderHandle	= ^CommonHeaderPointer;

	EppcValidAddress = RECORD
		CASE Integer OF
			0:	(asTargetID:	TargetID);
			1:	(asPSN:			ProcessSerialNumber);
			2:	(asSessionID:	LONGINT);
			3:	(asSignature:	OSType);
		END;

	MsgHdrPtr			= ^MsgHdr;
	MsgHdrHdl			= ^MsgHdrPtr;
	MsgHdr				= RECORD
		listClassHeader:	ListClassRec;
		paramCount: 		LONGINT;
		inUse:				Boolean;
		inWaiting:			Boolean;
		waitLink:			MsgHdrHdl;
		accessMask:			LONGINT;
		switchFromPSN:		ProcessSerialNumber;		{ switch back to this process afterwards }
		notifyRecPtr:		AENotifyRecPtr;
		prevMsg:			MsgHdrHdl;					{ we were processing this before this one }
		userRefcon:			LONGINT;
		thisAEEventClass: 	AEEventClass;
		thisAEEventID:		AEEventID;
		returnID:			LONGINT;
		msgAddrType: 		DescType;
		msgAddrSize:		LONGINT;
		msgAddrTarget:		EppcValidAddress;
		END;

	ListHdr 			= RECORD
		listClassHeader:	ListClassRec;
		paramCount:			LONGINT;
		sizeOfPrefix:		LONGINT;
		END;
	ListHdrPtr			= ^ListHdr;
	ListHdrHdl			= ^ListHdrPtr;

	AEMetaDesc			= RECORD
		aevtMarker: 		AEKeyWord;
		version:			LONGINT;
		metaTerminator: 	AEKeyWord;
		END;
	AEMetaPointer		= ^AEMetaDesc;
	
	
	AcceptRecord		= RECORD						{record to send to filterProc and
														 GetSpecific..}
		filterErr:			OSErr;						{any errors in getting the reply}
		extraFilter:		ProcPtr;					{filter for extra events}
		msgReturnID: 		Longint; 					{to match reply to msg}
		aBufferHdl: 		MsgHdrHdl; 					{to hold the reply}
		END;
	AcceptRecordPtr		= ^AcceptRecord;
	
	HandlePtr			= ^Handle;
	ExtendedPtr         = ^Extended;
	DoublePtr           = ^Double;
	RealPtr             = ^Real;
	CompPtr             = ^Comp;
	AEKeyDescPtr        = ^AEKeyDesc;
	AEDescPtr           = ^AEDesc;

	SysGlobalRec		= RECORD
		aeHashTable:		HHand;
		coercionHashTable:	HHand;
		kurtHashTable:		HHand;
		phacProcPtr:		ProcPtr;
		selhProcPtr:		ProcPtr;
		blockingPtr:		ProcPtr;
		unblockingPtr:		ProcPtr;
		diposeTokenProcPtr:	ProcPtr;
		compareProcPtr:		ProcPtr;
		countProcPtr:		ProcPtr;
		getMarkIDProcPtr:	ProcPtr;
		MarkProcPtr:		ProcPtr;
		AdjustMarksProcPtr:	ProcPtr;
		AENonAevtHandler:	ProcPtr;
		GetIndexInContainer:ProcPtr;
		END;

	GlobalRec			= RECORD
		aeHashTable:		HHand;
		coercionHashTable:	HHand;
		kurtHashTable:		HHand;
		phacProcPtr:		ProcPtr;
		selhProcPtr:		ProcPtr;
		blockingPtr:		ProcPtr;
		unblockingPtr:		ProcPtr;
		diposeTokenProcPtr:	ProcPtr;
		compareProcPtr:		ProcPtr;
		countProcPtr:		ProcPtr;
		getMarkIDProcPtr:	ProcPtr;
		MarkProcPtr:		ProcPtr;
		AdjustMarksProcPtr:	ProcPtr;
		AENonAevtHandler:	ProcPtr;
		GetIndexInContainer:ProcPtr;
		waitReplyList:		MsgHdrHdl;				{ first item in linked list of reply handle waiting for event }
		returnIDCounter: 	LONGINT;
		currentMsg: 		MsgHdrHdl;
		interactAllowance:	AEInteractAllowed;
		END;
	GlobalRecPointer	= ^GlobalRec;
	GlobalRecHandle 	= ^GlobalRecPointer;

	
	{ these are of offset of the field from the beginning of globalRec }
	
CONST
	phacOffset			=	12;
	selhOffset			=	16;
	blockingoffset		=	20;
	unblockOffset		=	24;
	diposeTokenOffset	=	28;
	kurProc1Offset		=	32;
	kurProc2Offset		=	36;
	kurProc3Offset		=	40;
	kurProc4Offset		=	44;
	kurProc5Offset		=	48;

TYPE


	KeyRec = Record
				secondKey:	OSType;
				firstKey:	OSType;
				END;
					
	handlerRec = Record
					theRefCon:	LONGINT;
					theProc:	ProcPtr;
					END;
					
	DescClass = (classMeta, classAevt, classList, classReco, classEmpty, classOther);
	
{--------------------------------------------------------------------------------}
	{ some common inlines }

	PROCEDURE IgnoreOSErr(anErr: OSErr);
		INLINE $548F;                                       { addq #2,sp }

	{ get the global specific to the application }
	FUNCTION GetGlobalRef: GlobalRecHandle; inline $2078, $02B6, $2EA8, $0154;

	{ get the global common to all applications }
	FUNCTION GetSysGlobal: GlobalRecHandle; inline $2078, $02B6, $2EA8, $017C;

{--------------------------------------------------------------------------------}

	{ AppleEvent Manger calls provided by this unit }

	FUNCTION AE_CreateDesc(typeCode: DescType;
                      dataPtr: Ptr;
                      dataSize: LONGINT;
                      VAR result: AEDesc): OSErr;

	FUNCTION AE_CreateList(factoringPtr: Ptr;
						factoredSize: LONGINT;
						isRecord: boolean;
						VAR resultList: AEDesc): OSErr;
	
	FUNCTION AE_DuplicateDesc(VAR theAEDesc: AEDesc;	{ VAR is for efficiency only }
							 VAR result: AEDesc): OSErr;

	FUNCTION AE_GetInteractionAllowed(VAR level:AEInteractAllowed): OSErr;
	
	FUNCTION AE_GetTheCurrentEvent(VAR theAppleEvent: AppleEvent): OSErr;

	FUNCTION AE_SetInteractionAllowed(level:AEInteractAllowed): OSErr;
	
	FUNCTION AE_SetTheCurrentEvent(VAR theAppleEvent: AppleEvent): OSErr;{ VAR is for efficiency only }
	
	FUNCTION AE_SuspendTheCurrentEvent(VAR theAppleEvent: AppleEvent): OSErr;{ VAR is for efficiency only }
	
{--------------------------------------------------------------------------------}

	{ utilty routines for other part of the AppleEvents Manager }

	PROCEDURE CalculateTimeOut(VAR timeOut:longint);
	
	FUNCTION CreateList(factoringPtr: Ptr;
						factoredSize: LONGINT;
						isRecord: boolean;
						VAR resultList: AEDesc;
						objectType: DescType): OSErr;
	
	PROCEDURE EventSource(theMsgHdl: MsgHdrHdl; 
						  VAR theAEEventSource:AEEventSource;
						  VAR fromPSN: ProcessSerialNumber);

	FUNCTION GeneralWait(VAR timeOut: LONGINT;
                     waitingHook: ProcPtr;
                     filterProc: ProcPtr;
                     filterInfo: Ptr): OSErr;

	FUNCTION MakeMeta(VAR theAevt: AppleEvent): AppleEvent;	{ VAR is for efficiency only }

	FUNCTION MustHaveGlobal(isSysHandler: Boolean;
							VAR aGlobalRef: GlobalRecHandle): OSErr;
							
	PROCEDURE NukeIt(VAR theAEDesc: AEDesc);
	
	PROCEDURE OffWaitList(aHandle: MsgHdrHdl);
	
	FUNCTION TryBothProc(VAR theDesc:AEDesc; procOffset: LONGINT):OSErr;
	
{--------------------------------------------------------------------------------}

IMPLEMENTATION

{$I AEUtil.inc1.p}

END. { AEUtil }