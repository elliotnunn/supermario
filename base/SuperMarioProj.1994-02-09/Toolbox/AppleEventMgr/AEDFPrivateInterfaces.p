{
	File:		AEDFPRivateInterfaces.p

	Written by:	Ed Lai

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

        Codes used in:  BigBang
	Change History (most recent first):

		 <5>	12/14/90	dba		<JDR> get rid of redundant definition of OSTypePtr, now in
									Types.p
		 <4>	 9/18/90	Lai		Change receiptRecord
		 <3>	 9/15/90	Lai		Change AcceptRecord and ReceiptRecord so that they can share the
									same procedure.
		 <2>	 9/10/90	Lai		Remove obsolete constants
		 <1>	  9/7/90	Lai		first checked in
		<0>	  9/7/90	Lai		First Checked in.

	To Do:
}

{[a-,body+,h-,o=100,r+,rec+,t=4,u+,#+,j=20/57/1$,n-]}	{ Pasmat format control line }
{ AEDFPrivateInterfaces.p }
{ Copyright © 1984-1990 by Apple Computer Inc.	All rights reserved. }

CONST
	errResetTimer 		= 1990;

	keySelectProc		= 'selh'; { your own idle procedure }

	typeMeta			= 'meta';
	kMhdrType			= 'mhdr';
	kLhdrType			= 'lhdr';
	aeEndOfMetaDataKeyword = ';;;;';
	aeRecTypeKeyword	= 'type';
	
	kInteractLevelKeyword = 'inte';
	kReplyRequestedKeyword = 'repq';
	kNeverInteract = 'nevi';
	kCanInteact	= 'cani';
	kCanSwitchIfInteract = 'cans';
	kAlwaysInteract = 'alwi';
	kAlwaysSwitchIfInteract = 'alws';
	
	kVersionOne			= $00010001;
	
	kAEProtocolMask		= $0000000F;					{ mask for sending protocols }
	kAEInteractMask 	= $00000070;					{ mask for interact level in send protocol }
	
	standardTimeOut		= 3600;					{ this is the value of standard timeout, one day
													we may change this to a function }


TYPE
	AENotifyRec 		= RECORD
		theNMRec:			NMRec;
		posted: 			Boolean;
		END;
	AENotifyRecPtr		= ^AENotifyRec;

	CommonHeader		= RECORD
		theKeyword: 		Keyword;
		theType:			DescType;
		theLength:			LONGINT;
		END;
	CommonHeaderPointer = ^CommonHeader;
	CommonHeaderHandle	= ^CommonHeaderPointer;

	MsgHdrPtr			= ^MsgHdr;
	MsgHdrHdl			= ^MsgHdrPtr;
	MsgHdr				= RECORD
		theHeader:			CommonHeader;
		hintIndex:			LONGINT;
		hintOffset: 		LONGINT;
		ParamBegin: 		LONGINT;
		paramCount: 		LONGINT;
		metaCount:			LONGINT;
		inUse:				Boolean;
		inWaiting:			Boolean;
		accessMask:			LONGINT;
		switchFromPSN:		ProcessSerialNumber;		{ switch back to this process afterwards }
		notifyRecPtr:		AENotifyRecPtr;
		prevMsg:			MsgHdrHdl;					{ we were processing this before this one }
		thisEventClass: 	EventClass;
		thisEventID:		EventID;
		returnID:			LONGINT;
		msgAddrType: 		DescType;
		msgAddrSize:		LONGINT;
		msgAddrTarget:		TargetID;
		END;

	ListHdr 			= RECORD
		theHeader:			CommonHeader;
		hintIndex:			LONGINT;
		hintOffset: 		LONGINT;
		END;
	ListHdrPtr			= ^ListHdr;
	ListHdrHdl			= ^ListHdrPtr;

	AEMetaDesc			= RECORD
		aevtMarker: 		Keyword;
		version:			LONGINT;
		metaTerminator: 	Keyword;
		END;
	AEMetaPointer		= ^AEMetaDesc;
	
	AEMetaTransaction	= RECORD
		aevtMarker: 		Keyword;
		version:			LONGINT;
		theKeyword: 		Keyword;
		theType:			DescType;
		theLength:			LONGINT;
		theValue:			LONGINT;
		metaTerminator: 	Keyword;
		END;
	AEMetaTransPointer	= ^AEMetaTransaction;
	

	AETableRec			= RECORD
		firstKey:			OSType;
		secondKey:			OSType;
		theValue:			ProcPtr;
		theRefCon:			LongInt;
		END;
	AETablePointer		= ^AETableRec;
	AETableHandle		= ^AETablePointer;

	PLongint			= ^LONGINT;
	HLongint			= ^PLongint;
	PInteger			= ^Integer;

	GlobalRec			= RECORD
		aeTable:			AETableHandle;
		aeTableSize:		LONGINT;
		coercionTable:		AETableHandle;
		coerTableSize:		LONGINT;
		phacProcPtr:		ProcPtr;
		selhProcPtr:		ProcPtr;
		blockingPtr:		ProcPtr;
		unblockingPtr:		ProcPtr;
		AENonAevtHandler:	ProcPtr;
		waitReplyList:		Handle;				{ handle to a list of reply handle waiting for event }
		waitReplyCount:		Integer;
		returnIDCounter: 	LONGINT;
		currentMsg: 		MsgHdrHdl;
		interactAllowance:	AEInteractAllowed;
		END;
	GlobalRecPointer	= ^GlobalRec;
	GlobalRecHandle 	= ^GlobalRecPointer;
	
	{ these are of offset of the field from the beginning of globalRec }
	
CONST
	phacOffset			=	16;
	selhOffset			=	20;

TYPE

	AEListRec			= RECORD
		theHeader:			CommonHeader;
		numberOfItems:		LONGINT;
		sizeOfPrefix:		LONGINT;
		END;
	AEListPointer		= ^AEListRec;

	ReceiptRecord		= RECORD						{record to send to filterProc and
														 GetSpecific..}
		aTransactionID: 	LONGINT;					{to match reply to msg}
		anErr:				OSErr;						{any errors in getting the reply}
		areturnID: 			Longint;					{to match reply to msg}
		END;

	AcceptRecord		= RECORD						{record to send to filterProc and
														 GetSpecific..}
		aBufferHdl: 		MsgHdrHdl; 					{to hold the reply}
		anErr:				OSErr;						{any errors in getting the reply}
		extraFilter:		ProcPtr;					{filter for extra events}
		END;
	AcceptRecordPtr		= ^AcceptRecord;
	
	FUNCTION theGlobalRef: GlobalRecHandle; inline $2078, $02B6, $2EA8, $0154;
	procedure SetGlobalRef(aGlobal:GlobalRecHandle); inline $2078, $02B6, $215F, $0154;
	FUNCTION GetSysGlobal: GlobalRecHandle; EXTERNAL;
