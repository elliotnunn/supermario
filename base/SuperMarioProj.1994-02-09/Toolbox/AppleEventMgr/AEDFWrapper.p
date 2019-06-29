{
	File:		AEDFWrapper.p

	Copyright:	© 1990-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <2>	 5/21/91	gbm		Stop using the "empty" units (MemTypes, etc...)
		 <1>	12/13/90	Lai		first checked in

	To Do:
}

UNIT AEDFWrapper;

INTERFACE

USES
	Types, Memory, ToolUtils,
	AppleEvents, AEHashTable, AEUtil, AEHandlerTable, AECoercion;


	{ AppleEvent Manger calls provided by this unit }
	
{--------------------------------------------------------------------------------}

	FUNCTION AE_DisposeDesc(VAR theAEDesc: AEDesc): OSErr;	
	
	FUNCTION AE_CountItems(VAR theAEDescList: AEDescList; { VAR is for efficiency only }
						  VAR theCount:LONGINT): OSErr;

	FUNCTION AE_PutPtr(VAR theAEDescList: AEDescList;	{ VAR is for efficiency only }
					  index: LONGINT;
					  typeCode: DescType;
					  dataPtr: Ptr;
					  dataSize: LONGINT): OSErr;

	FUNCTION AE_PutDesc(VAR theAEDescList: AEDescList;	{ VAR is for efficiency only }
					   index: LONGINT;
					   VAR theAEDesc: AEDesc): OSErr;

	FUNCTION AE_GetNthPtr(VAR theAEDescList: AEDescList;	{ VAR is for efficiency only }
						 index: LONGINT;
						 desiredType: DescType;
						 VAR theAEKeyword: AEKeyWord;
						 VAR typeCode: DescType;
						 dataPtr: Ptr;
						 maximumSize: LONGINT;
						 VAR actualSize: LONGINT): OSErr;

	FUNCTION AE_GetNthDesc(VAR theAEDescList: AEDescList;	{ VAR is for efficiency only }
						  index: LONGINT;
						  desiredType: DescType;
						  VAR theAEKeyword: AEKeyWord;
						  VAR theAEDesc: AEDesc): OSErr;
						  
	FUNCTION AE_SizeOfNthItem(VAR theAEDescList: AEDescList;	{ VAR is for efficiency only }
							 index: LONGINT;
							 VAR typeCode: DescType;
							 VAR dataSize: Longint): OSErr;

	FUNCTION AE_GetArray(VAR theAEDescList: AEDescList;			{ VAR is for efficiency only }
						arrayType: AEArrayType;
						arrayPtr: AEArrayDataPointer;
						maximumSize: LONGINT;
						VAR itemType: DescType;
						VAR itemSize: Size;
						VAR itemCount: LONGINT): OSErr;

	FUNCTION AE_PutArray(VAR theAEDescList: AEDescList;			{ VAR is for efficiency only }
						arrayType: AEArrayType;
						arrayPtr: AEArrayDataPointer;
						itemType: DescType;
						itemSize: Size;
						itemCount: LONGINT): OSErr;

	FUNCTION AE_DeleteItem(VAR theAEDescList: AEDescList;
						   index: LONGINT): OSErr;

	FUNCTION AE_PutKeyPtr(VAR theAERecord: AERecord;	{ VAR is for efficiency only }
						 theAEKeyword: AEKeyWord;
						 typeCode: DescType;
						 dataPtr: Ptr;
						 dataSize: LONGINT): OSErr;

	FUNCTION AE_PutKeyDesc(VAR theAERecord: AERecord;	{ VAR is for efficiency only }
						  theAEKeyWord: AEKeyWord;
						  VAR theAEDesc: AEDesc): OSErr;	{ VAR is for efficiency only }

	FUNCTION AE_GetKeyPtr(VAR theAERecord: AERecord;	{ VAR is for efficiency only }
						 theAEKeyWord: AEKeyWord;
						 desiredType: DescType;
						 VAR typeCode: DescType;
						 dataPtr: Ptr;
						 maximumSize: Size;
						 VAR actualSize: Size): OSErr;

	FUNCTION AE_GetKeyDesc(VAR theAERecord: AERecord;	{ VAR is for efficiency only }
						  theAEKeyWord: AEKeyWord;
						  desiredType: DescType;
						  VAR result: AEDesc): OSErr;

	FUNCTION AE_SizeOfKeyDesc(VAR theAERecord: AERecord;	{ VAR is for efficiency only }
							 theAEKeyWord: AEKeyWord;
							 VAR typeCode: DescType;
							 VAR dataSize: Longint): OSErr;

	FUNCTION AE_DeleteKeyDesc(VAR theAERecord: AERecord;	{ VAR is for efficiency only }
							  theAEKeyWord: AEKeyWord): OSErr;

	FUNCTION AE_CreateAppleEvent(theAEEventClass: AEEventClass;
								theAEEventID: AEEventID;
								VAR target: AEAddressDesc;	{ VAR is for efficiency only }
								returnID: Integer;
								transactionID: longint;
								VAR result: AppleEvent): OSErr;
	
	FUNCTION AE_GetAttributePtr(VAR theAppleEvent: AppleEvent;	{ VAR is for efficiency only }
							   theAEKeyWord: AEKeyWord;
							   desiredType: descType;
							   VAR typeCode: descType;
							   dataPtr: Ptr;
							   maximumSize: Size;
							   VAR actualSize: Size): OSErr;

	FUNCTION AE_GetAttributeDesc(VAR theAppleEvent: AppleEvent;	{ VAR is for efficiency only }
								theAEKeyWord: AEKeyWord;
								desiredType: descType;
								VAR result: AEDesc): OSErr;
	
	FUNCTION AE_SizeOfAttribute(VAR theAppleEvent: AppleEvent;	{ VAR is for efficiency only }
							   theAEKeyWord: AEKeyWord;
							   VAR typeCode: DescType;
							   VAR dataSize: Size): OSErr;

	FUNCTION AE_PutAttributePtr(VAR theAppleEvent: AppleEvent;	{ VAR is for efficiency only }
							   theAEKeyWord: AEKeyWord;
							   typeCode: descType;
							   dataPtr: Ptr;
							   dataSize: Size): OSErr;
	
	FUNCTION AE_PutAttributeDesc(VAR theAppleEvent: AppleEvent;	{ VAR is for efficiency only }
								theAEKeyWord: AEKeyWord;
								VAR theAEDesc: AEDesc			{ VAR is for efficiency only }
								): OSErr;
	

{--------------------------------------------------------------------------------}

	{ utilty routines for other part of the AppleEvents Manager }

	FUNCTION AddDesc(VAR theAEDescList: AEDescList;	{ VAR is for efficiency only }
					 key: AEKeyWord;
					 VAR theArg: AEDesc;				{ VAR is for efficiency only }
					 index: LONGINT
					 ): OSErr;

	FUNCTION CheckMessage(msg: MsgHdrHdl): OSErr;
	
	FUNCTION CreateBareMessage(msgAddrLen: LONGINT;
							   theAEEventClass: AEEventClass;
							   theAEEventID: AEEventID;
							   theReturnID: LONGINT;
							   theTransactionID: LONGINT;
							   VAR result: AppleEvent): OSErr;
	
	FUNCTION CreateBuffer(extraBytes: LONGINT;
						  msgAddrLen: LONGINT;
						  theAEEventClass: AEEventClass;
						  theAEEventID: AEEventID;
						  VAR result: AppleEvent): OSErr;
	
	FUNCTION FetchDesc(VAR theAEDescList: AEDescList;	{ VAR is for efficiency only }
					   index: LONGINT;
					   wantType: DescType;
					   VAR key: AEKeyWord;
					   VAR res: AEDesc): OSErr;
	
	FUNCTION GetAttribute(VAR theAevt: AppleEvent;	{ VAR is for efficiency only }
						  key: AEKeyWord;
						  desiredType: DescType;
						  VAR typeCode: DescType;
						  dataPtr: Ptr;
						  maximumSize: LONGINT;
						  VAR actualSizeOrHandle: LONGINT;
						  wantDesc: boolean
						  ): OSErr;
	
	FUNCTION ReadData(VAR theAEDescList: AEDescList;	{ VAR is for efficiency only }
					  index: LONGINT;
					  desiredType:	DescType;
					  VAR key: AEKeyWord;
					  VAR typeCode: DescType;
					  dataPtr: Ptr;
					  maximumSize: LONGINT;
					  VAR actualSize: LONGINT): OSErr;
	
	FUNCTION WriteData(VAR theAEDescList: AEDescList;	{ VAR is for efficiency only }
					   key: AEKeyWord;
					   dataType: DescType;
					   dataPtr: Ptr;
					   dataLength: LONGINT;
					   index: LONGINT;
					   numberOfItems: LONGINT
					   ): OSErr;
					   

IMPLEMENTATION

{$I AEDFWrapper.inc1.p}

END. { AEDFWrapper }