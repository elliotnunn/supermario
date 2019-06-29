{
	File:		AEDFAppleEvents.p

	Written by:	Ed Lai

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

        Codes used in:  BigBang,BigBang,BigBang,BigBang
	Change History (most recent first):

		 <5>	 10/3/90	gbm		NMRecPtr are no longer QElemPtr, but are now NMRecPtr.
		 <4>	 9/25/90	Lai		Bug fix for jfr0009, change order of parameter in special
									handler calls
		 <3>	 9/20/90	Lai		Add error constant errAEIllegalIndex
		 <2>	 9/15/90	Lai		new idle proc in AEInteractWithUser
		 <1>	  9/7/90	Lai		first checked in
		<0>	  9/7/90	Lai		First Checked in.

	To Do:
}

{[a-,body+,h-,o=100,r+,rec+,t=4,u+,#+,j=20/57/1$,n-]}	{ Pasmat format control line }
{ AEDFAppleEvents.p }
{ Copyright © 1984-1990 by Apple Computer Inc.	All rights reserved. }

{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
UNIT AEDFAppleEvents;

	INTERFACE
		{$ENDC}

		{$IFC UNDEFINED AEDFAppleEvents}
		{$SETC AEDFAppleEvents := 0}
		{$ENDC}

		{$IFC NOT AEDFAppleEvents}
		{$SETC AEDFAppleEvents := 1}

		{$I+}
		{$SETC AEDFAppleEventsIncludes := UsingIncludes}
		{$SETC UsingIncludes := 1}
		{$IFC UNDEFINED UsingTypes}
		{$I $$Shell(PInterfaces)Types.p}
		{$ENDC}
		{$IFC UNDEFINED UsingAppleTalk}
		{$I $$Shell(PInterfaces)AppleTalk.p}
		{$ENDC}
		{$IFC UNDEFINED UsingEvents}
		{$I $$Shell(PInterfaces)Events.p}
		{$ENDC}
		{$IFC UNDEFINED UsingPPCToolBox}
		{$I $$Shell(PInterfaces)PPCToolBox.p}
		{$ENDC}
		{$IFC UNDEFINED UsingProcesses}
		{$I $$Shell(PInterfaces)Processes.p}
		{$ENDC}
		{$IFC UNDEFINED UsingEPPC}
		{$I $$Shell(PInterfaces)EPPC.p}
		{$ENDC}
		{$IFC UNDEFINED UsingNotification}
		{$I $$Shell(PInterfaces)Notification.p}
		{$ENDC}
		{$IFC UNDEFINED UsingSANE}
		{$I $$Shell(PInterfaces)SANE.p}
		{$ENDC}
		{$SETC UsingIncludes := AEDFAppleEventsIncludes}

		CONST
			typeBoolean			= 'bool';
			typeChar			= 'TEXT';
			typeSMInt			= 'shor';
			typeInteger			= 'long';
			typeSMFloat			= 'sing';
			typeFloat			= 'doub';
			
			typeExtended		= 'exte';
			typeComp			= 'comp';
			typeMagnitude		= 'magn';
			typeAEList 			= 'list';
			typeAERecord		= 'reco';
			typeTrue	 		= 'true';
			typeFalse			= 'fals';
			typeAlias			= 'alis';
			typeEnumerated		= 'enum';
			typeType			= 'type';
			typeAppParameters	= 'appa';
			typeProperty		= 'prop';
			typeNull			= 'null';  {the type of null/nonexistent data}
			typeKeyword			= 'keyw';
			typeFSS				= 'fss ';
			typeSectionH		= 'sect';	
			
			typeWildCard	 	= '****';

			typeApplSignature	= 'sign';
			typeSessionID		= 'ssid';
			typeTargetID		= 'targ';
			typeProcessSerialNumber = 'psn ';

			kAEDirectParameterKeyword = '----';

			kCoreEventClass 	= 'aevt';

			kAEOpenApplication	= 'oapp';
			kAEOpenDocuments	= 'odoc';
			kAEPrintDocuments	= 'pdoc';
			kAEQuitApplication	= 'quit';
			
			kAEAnswer			= 'ansr';
			
			keyPreDispatch		= 'phac';	{ PreHandler Accessor Call }

			kAETransactionIDAttribute 	= 'tran';
			kAEReturnIDAttribute 		= 'rtid';
			kAEEventClassAttribute 		= 'evcl';
			kAEEventIDAttribute 		= 'evid';
			kAEAddressAttribute 		= 'addr';
			kAEInteractLevelAttribute 	= 'inte';
			kAEEventSourceAttribute		= 'esrc';
			kAEMissedKeywordAttribute	= 'miss';
			kAEOptionalKeywordAttribute	= 'optk';

			kAENoReply			= $00000001;			{ Sender doesn't want a reply to event }
			kAEQueueReply		= $00000002;			{ Sender wants a reply but won't wait }
			kAEWaitReply		= $00000003;			{ Sender wants a reply and will be waiting }

			kAENeverInteract	= $00000010;			{ Server should not interact with user }
			kAECanInteract		= $00000020;			{ Server may try to interact with user }
			kAEAlwaysInteract	= $00000030;			{ Server should always interact with user }

			kAECanSwitchLayer	= $00000040;			{ Interaction may switch layer }

			kAEDontReconnect	= $00000080;			{ don't reconnect if there is a
														 sessClosedErr }

			kAEWantReceipt		= nReturnReceipt;		{ Send wants a receipt of message }

			kAENormalPriority	= $00000000;			{ Post message at the end of event queue }
			kAEHighPriority 	= nAttnMsg; 			{ Post message at the front of the event
														 queue }
			kAEUseStandardDispatch	= -1;

			keyTimeoutAttr			= 'timo';

			kUndefinedTransactionID = 0;
			kAutoGenerateReturnID = -1;

			kAEDefaultTimeout		= -1;				{ timeout value determined by AEM }
			kNoTimeOut				= -2;				{ wait until reply comes back, however long it takes }

			{ Error messages in response to reading and writing buffer contents }
			errAECoercionFail 	= - 1700;
			errAEDescNotFound 	= - 1701;
			errAECorruptData	= - 1702;
			errAEWrongDataType	= - 1703;
			errAENotAEDesc		= - 1704;
			errAEBadListElement	= - 1705;
			errAEWrongVersion 	= - 1706;
			errAENotAppleEvent	= - 1707;

			{ Error messages in response to sending/receiving a message }
			errAEEventNotHandled= - 1708;				{ The AppleEvent is not handled by any handler }
			errAEReplyNotValid	= - 1709;				{ AEResetTimer was passed an invalid reply }
			errAEUnknownSendMode= - 1710;				{ Mode wasn't NoReply, WaitReply,
														 QueueReply; or Interaction level is
														 unknown }
			errAEWaitCanceled 	= - 1711;				{ User cancelled out of wait loop for reply
														 or receipt }
			errAETimeoutErr		= - 1712;				{ AppleEvent time out }

			errAENoUserInteraction = - 1713;			{ no user interaction allowed }
			errAENotASpecialFunction = -1714;
			
			errAENotInitialized	= - 1715;
			errAEUnknownAddress = - 1716;
			errAEHandlerNotFound= - 1717;				{ The event isn't in the supplied event
														 table }
			errAEReplyNotArrived= - 1718;
			errAEIllegalIndex = -1719;
			errAENotImplemented	= - 1724;				
		TYPE
			KeyWord 			= PACKED ARRAY [1..4] OF CHAR;
			EventClass			= PACKED ARRAY [1..4] OF CHAR;
			EventID 			= PACKED ARRAY [1..4] OF CHAR;
			DescType			= OSType;

			AEDesc				= RECORD
				descriptorType: 	DescType;
				dataHandle: 		Handle;
				END;
				
			AEAddressDesc		= AEDesc;

			AEDescList			= AEDesc;
			
			AERecord			= AEDescList;
			
			AppleEvent			= AERecord;

			AEInteractAllowed = (kAEInteractWithSelf, kAEInteractWithLocal, kAEInteractWithAll);

			AEEventSource = (kAEUnkownSource, kAEDirectCall, kAESameProcess, kAELocalProcess, kAERemoteProcess);

			AEKeyDesc			= RECORD
				descKey:			KeyWord;
				descContent:		AEDesc;
				END;
				
			AEArrayType 		= (kAEDataArray, kAEPackedArray, kAEHandleArray, kAEDescArray,
								   kAEKeyDescArray);
								   
			AEArrayData = RECORD
				case AEArrayType OF
					kAEDataArray:	
						(AEDataArray: 		Array[0..0] OF Integer);
					kAEPackedArray:	
						(AEPackedArray: 	Packed Array[0..0] OF Char);
					kAEHandleArray:	
						(AEHandleArray: 	Array[0..0] OF Handle);
					kAEDescArray:	
						(AEDescArray: 		Array[0..0] OF AEDesc);
					kAEKeyDescArray:	
						(AEKeyDescArray: 	Array[0..0] OF AEKeyDesc);
				END;
				
			AEArrayDataPointer = ^AEArrayData;

			IdleProcPtr			= ProcPtr;

{--------------------------------------------------------------------------------}

		{ The follow calls apply to any AEDesc }

		FUNCTION AECreateDesc(typeCode: DescType;
							  dataPtr: Ptr;
							  dataSize: LONGINT;
							  VAR result: AEDesc): OSErr;

		FUNCTION AECoerceData(typeCode: DescType;
							  dataPtr: Ptr;
							  dataSize: LONGINT;
							  toType: DescType;
							  VAR result: AEDesc): OSErr;

		FUNCTION AECoerceDesc(desc: AEDesc;
							  toType: DescType;
							  VAR result: AEDesc): OSErr;

		FUNCTION AEDisposeDesc(VAR desc: AEDesc): OSErr;
		
		FUNCTION AEDuplicateDesc(desc:AEDesc; VAR result: AEDesc): OSErr;

{--------------------------------------------------------------------------------}

		{ The following calls apply to AEDescList.
		  Since AEDescList is a subtype of AEDesc, the calls in the previous
		  section can also be used for AEDescList }
		

		FUNCTION AECreateList(factoringPtr: Ptr;
							  factoredSize: LONGINT;
							  isRecord: boolean;
							  VAR resultList: AEDescList): OSErr;

		FUNCTION AECountListElems(theDescList: AEDescList; 
							  VAR theCount:Longint): OSErr;

		FUNCTION AEPutPtr(theDescList: AEDescList;
						  index: LONGINT;
						  typeCode: DescType;
						  dataPtr: Ptr;
						  dataSize: LONGINT): OSErr;

		FUNCTION AEPutDesc(theDescList: AEDescList;
						   index: LONGINT;
						   theDesc: AEDesc): OSErr;

		FUNCTION AEGetNthPtr(theDescList: AEDescList;
							 index: LONGINT;
							 desiredType: DescType;
							 VAR key: KeyWord;
							 VAR typeCode: DescType;
							 dataPtr: Ptr;
							 maximumSize: LONGINT;
							 VAR actualSize: LONGINT): OSErr;

		FUNCTION AEGetNthDesc(theDescList: AEDescList;
							  index: LONGINT;
							  desiredType: DescType;
							  VAR key: KeyWord;
							  VAR theDesc: AEDesc): OSErr;
							  
		FUNCTION AESizeOfNthDesc(theDescList: AEDescList;
						  		 index: LONGINT;
								 VAR typeCode: DescType;
								 VAR dataSize: Longint): OSErr;

		FUNCTION AEGetArray(theDescList: AEDescList;
							arrayType: AEArrayType;
							arrayPtr: AEArrayDataPointer;
							bufSize: LONGINT;
							VAR elemType: DescType;
							VAR elemSize: LONGINT;
							VAR itemCount: LONGINT): OSErr;

		FUNCTION AEPutArray(theDescList: AEDescList;
							arrayType: AEArrayType;
							arrayPtr: AEArrayDataPointer;
							elemType: DescType;
							elemSize: LONGINT;
							itemCount: LONGINT): OSErr;

		FUNCTION AEDeleteIndex(theDescList: AEDescList;
							   index: LONGINT): OSErr;

{--------------------------------------------------------------------------------}

		{ The following calls apply to AERecord.
		  Since AERecord is a subtype of AEDescList, the calls in the previous
		  sections can also be used for AERecord
		  an AERecord can be created by using AECreateList with isRecord set to true }
		
		FUNCTION AEPutKeyPtr(theAERecord: AERecord;
							 key: KeyWord;
							 typeCode: DescType;
							 dataPtr: Ptr;
							 dataSize: LONGINT): OSErr;

		FUNCTION AEPutKeyDesc(theAERecord: AERecord;
							  key: KeyWord;
							  theDesc: AEDesc): OSErr;

		FUNCTION AEGetKeyPtr(theAERecord: AERecord;
							 key: KeyWord;
							 desiredType: DescType;
							 VAR typeCode: DescType;
							 dataPtr: Ptr;
							 maximumSize: LONGINT;
							 VAR actualSize: LONGINT): OSErr;

		FUNCTION AEGetKeyDesc(theAERecord: AERecord;
							  key: KeyWord;
							  desiredType: DescType;
							  VAR theDesc: AEDesc): OSErr;

		FUNCTION AESizeOfKeyDesc(theAERecord: AERecord;
							 	 key: Keyword;
								 VAR typeCode: DescType;
								 VAR dataSize: Longint): OSErr;

		FUNCTION AEDeleteKeyDesc(theDescList: AERecord;
							 key: KeyWord): OSErr;

{--------------------------------------------------------------------------------}

		{ The following calls applies to AppleEvent.
		  Since AppleEvent is a subtype of AERecord, the calls in the previous
		  sections can also be used for AppleEvent }

		FUNCTION AECreateAevt(theEventClass: EventClass;
							  theEventID: EventID;
							  target: AEAddressDesc;
							  theReturnID: Integer;
							  theTransactionID: longint;
							  VAR result: AppleEvent): OSErr;
		
		FUNCTION AEGetPtrAttribute(theAevt: AppleEvent;
							 	   key: Keyword;
							 	   desiredType: descType;
							 	   VAR typeCode: descType;
							 	   dataPtr: Ptr;
							  	   maximumSize: longint;
							 	   VAR actualSize: longint): OSErr;

		FUNCTION AEGetDescAttribute(theAevt: AppleEvent;
							 		key: Keyword;
							 		desiredType: descType;
							 		VAR result: AEDesc): OSErr;
		
		FUNCTION AESizeOfAttribute(theAevt: AERecord;
							 	   key: Keyword;
								   VAR typeCode: DescType;
								   VAR dataSize: Longint): OSErr;

		FUNCTION AEPutPtrAttribute(theAevt: AppleEvent;
								   key: Keyword;
								   typeCode: descType;
								   dataPtr: Ptr;
								   dataSize: longint): OSErr;
		
		FUNCTION AEPutDescAttribute(theAevt: AEDescList;
									key: Keyword;
									theDesc: AEDesc): OSErr;
		
		FUNCTION AESend(theAevt: AppleEvent;
						VAR theReply: AppleEvent;
						sendMode: LONGINT;
						sendPriority: INTEGER;
						timeOut: LONGINT;
						idleProc: ProcPtr;
						filterProc: ProcPtr): OSErr;

		FUNCTION AESuspendTheEvent(theAevt: AppleEvent): OSErr;
		
		FUNCTION AEResumeTheEvent(theAevt: AppleEvent): OSErr;
		
		FUNCTION AEFinishEventHandling(theAevt, theReply: AppleEvent; 
									   dispatcher:ProcPtr; 
									   handlerRefcon:Longint): OSErr; 
		
		FUNCTION AEResetTimer(reply:AppleEvent): OSErr;
		{ Convience routine.  Create and send a 'wait' message for the }
		{ current reply. }

		FUNCTION AEWhichAevt(VAR curAevt: AppleEvent): OSErr;

{--------------------------------------------------------------------------------}

		FUNCTION AEProcessAppleEvent(eventRec: EventRecord): OSErr;

		FUNCTION AEGetInteractionAllowed(VAR theLevel:AEInteractAllowed): OSErr;
		
		FUNCTION AESetInteractionAllowed(theLevel:AEInteractAllowed): OSErr;
		
		FUNCTION AEInstallEventHandler(theEventClass: EventClass;
								 	   theEventID: EventID;
								 	   theHandler: ProcPtr;
									   handlerRefcon: LongInt;
								 	   isSysHandler: Boolean): OSErr;
		{ Add an AppleEvent Handler }
		
		FUNCTION AERemoveEventHandler(theEventClass: EventClass;
								 	  theEventID: EventID;
								 	  theHandler: ProcPtr;
								 	  isSysHandler: Boolean): OSErr;
		{ Remove one or more AppleEvent Handler }

		FUNCTION AEGetEventHandler(theEventClass: EventClass;
								   theEventID: EventID;
								   var theHandler: ProcPtr;
								   var handlerRefcon: LongInt;
								   isSysHandler: Boolean): OSErr;
		{ Get the corresponding AppleEvent Handler }

		FUNCTION AEInstallCoercionHandler(fromType: DescType;
								 		  toType: DescType;
								 		  theHandler: ProcPtr;
									   	  handlerRefcon: LongInt;
										  fromTypeAsDesc: Boolean;
								 		  isSysHandler: Boolean): OSErr;
		{ Add an AppleEvent Handler }
		
		FUNCTION AERemoveCoercionHandler(fromType: DescType;
								 		 toType: DescType;
								 		 theHandler: ProcPtr;
								 		 isSysHandler: Boolean): OSErr;
		{ Remove one or more AppleEvent Handler }
		
		FUNCTION AEGetCoercionHandler (fromType: DescType;
								 	  toType: DescType;
								 	  var theHandler: ProcPtr;
								   	  var handlerRefcon: LongInt;
									  var fromTypeAsDesc: Boolean;
									  isSysHandler: Boolean): OSErr;
		{ Get the corresponding AppleEvent Handler }

		FUNCTION AEInteractWithUser(timeOut: LONGINT;	{ how long are you willing to wait }
									nmReqPtr: NMRecPtr;	{ your own custom notification }
									idleProc: IdleProcPtr 		{ your own idle procedure }
									): OSErr;			{ return the error }

	FUNCTION AEInstallSpecialHandler(functionClass:		Keyword;
									 handler:			ProcPtr;
									 isSystemHandler:		BOOLEAN):OSErr;
	{ Install the special handler named by the Keyword }
									   
	FUNCTION AERemoveSpecialHandler(functionClass:		Keyword;
									handler:			ProcPtr;
									isSystemHandler:		BOOLEAN):OSErr;
	{ Remove the special handler named by the Keyword }
									   
	FUNCTION AEGetSpecialHandler(functionClass:		Keyword;
								 VAR handler:		ProcPtr;
								 isSystemHandler:		BOOLEAN):OSErr;
	{ Get the special handler named by the Keyword }

		{$ENDC} 										{ AEDFAppleEvents }

		{$IFC NOT UsingIncludes}
END.
{$ENDC}
