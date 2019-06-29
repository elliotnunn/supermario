{
    File:       AEDF.Inc1.p

    Written by: Ed Lai

    Copyright:  © 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

        Codes used in:  BigBang
    Change History (most recent first):

		<18>	 3/15/91	Lai		BM,#83128: Remove notification when interacting with event from
									another process.
		<17>	  2/5/91	Lai		BM,#81595: Set the descriptorType of reply parameter in the
									handler to 'null' when there is no reply requested.
		<16>	 1/11/91	Lai		Processing another AppleEvent in WaitReply loop no longer drop
									out of the loop.
		<15>	12/14/90	gbm		<jdr> Correct spelling of rtrnReceiptMsgID
		<14>	12/13/90	Lai		Separation of the main files into separate modules
		<13>	10/29/90	Lai		Return an error if receipt indicates that server did not accept
									the AppleEvent fully. Also round up any AppleEvent buffer to
									even length (already the case if message send from AEM, this is
									to take care of those who don't use AEM to send AppleEvent).
		<12>	 10/3/90	gbm		NMRecPtr are no longer QElemPtr, but really NMRecPtr.
		<11>	 9/25/90	Lai		Bug fix for jfr0009, change order of parameter in special
									handler calls
		<10>	 9/21/90	Lai		Send actual timeout value rather than timeout constant (e.g.-1)
									in message.
		 <9>	 9/20/90	Lai		Do not allow installing illegal handlers (NIL or odd)
		 <8>	 9/19/90	Lai		pass back return ID in the wait reply filter, handle
									AEInteractionWithUser when not processing an AppleEvent.
		 <7>	 9/19/90	Lai		In the waitloop, generate the first null event by calling
									OSEventAvail so that other fields will be filled.
		 <6>	 9/18/90	Lai		Use refcon as the return ID, and put the transaction ID back as
									meta parameter
		 <5>	 9/15/90	ngk		fix for names changes in PPCToolBox.p
		 <4>	 9/15/90	Lai		don't use modifiers as returnID in receipt
		 <3>	 9/15/90	Lai		Post code review cleanup, idle proc now recieve
									update/suspend/activate event
        <2+>     9/13/90    Lai     Post code review cleanup
         <2>     9/10/90    Lai     Type coercion OSType to LONGINT for better compil under 3.0
                                    compiler.
         <1>      9/7/90    Lai     first checked in
        <0>   9/7/90    Lai     First Checked in.

    To Do:
}
{$P}
{ AEDF.inc1.p }
{ Copyright © 1984-1990 by Apple Computer Inc. All rights reserved. }

{--------------------------------------------------------------------------------}

CONST
	kFirstLegalReply = kAENoReply;
	kLstLegalRely = kAEWaitReply;


TYPE
    EppcAddress = RECORD
                      postingOptions: LONGINT;
                      sendToAddr: Ptr;
                  END;

    {--------------------------------------------------------------------------------}

FUNCTION CallNotAppleEvent(sender: TargetID;
                           msgRefcon: LONGINT;
                           theAEEventClass: AEEventClass;
                           theAEEventID: AEEventID;
                           msgBuffer: Ptr;
                           msgLen: LONGINT;
                           callproc: ProcPtr): OSErr;
{ inline is equivalent to : movea.l (a7)+,a0; jsr (a0) }
    INLINE $205F, $4E90;

{--------------------------------------------------------------------------------}

FUNCTION DoExtraFilter(VAR theMsgEvent: EventRecord;
                       returnID: LONGINT;
                       eventRefCon: LONGINT;
                       sender: AEAddressDesc;
                       callproc: ProcPtr): Boolean;
{ inline is equivalent to : movea.l (a7)+,a0; jsr (a0) }
    INLINE $205F, $4E90;

{--------------------------------------------------------------------------------}

FUNCTION CallTableDispatch(msg, theReply: AppleEvent;
                           refcon: LONGINT;
                           callproc: ProcPtr): OSErr;
{ inline is equivalent to : movea.l (a7)+,a0; jsr (a0) }
    INLINE $205F, $4E90;

{--------------------------------------------------------------------------------}

FUNCTION CheckAppleEvent(VAR theAppleEvent: AppleEvent): OSErr; { VAR is for efficiency only }
    FORWARD;

FUNCTION CreateDefaultReply(VAR origMsg: AppleEvent; { VAR is for efficiency only }
                            VAR theReply: AppleEvent { --> the reply }
                            ): OSErr;
    FORWARD;

FUNCTION PeekMetaParameter(EPPCBuff: HighLevelEventMsgPtr; { the EPPC msg buffer }
                           theAttribute: AEKeyWord; { look for this key word }
                           VAR isAevt: Boolean; { does it look like an AppleEvent? }
                           VAR theValue: LONGINT { value of the keyword attribute }
                           ): Boolean;
    FORWARD;

PROCEDURE PullOffTheQueue;
    FORWARD;
	
FUNCTION SendMessage(theMsg: MsgHdrHdl;
					 sendMode: LONGINT;
                     sendPriority: integer; { AEHighPriority, AENormalPriority }
                     timeOut: LONGINT; { sender's timeout in ticks }
                     waitingHook: ProcPtr; {Routine to call to spin the cursor}
                     extraHandlingFilter: ProcPtr { extra event processing filter }
                     ): OSErr;
    FORWARD;

FUNCTION SendWithReply(VAR msg: AppleEvent; { VAR is for efficiency only }
                       VAR reply: AppleEvent; {Uninitialized, we create it(pass to createmsg)}
                       isWaitReply: Boolean; { Queue or Wait for a reply after sending }
                       sendPriority: integer; { AEHighPriority, AENormalPriority }
					   sendMode: LONGINT;
                       timeOut: LONGINT; {sender's timeout in ticks}
                       waitingHook: ProcPtr; {Routine to call to spin the cursor}
                       extraHandlingFilter: ProcPtr { extra event processing filter }
                       ): OSErr;
    FORWARD;

FUNCTION TryEppcFail(theMsg: MsgHdrHdl;
                     toType: DescType;
                     VAR coercedTarget: AEDesc): Boolean;
    FORWARD;

{--------------------------------------------------------------------------------}

{ Used internally to send message buffers. Called from a routine inside AESend }
{ Uses EPPC to send a message and wait for a receipt if required. }

PROCEDURE AddrTypeToPostingOptions(addrType: LONGINT;
                                   addrPtr: Ptr;
                                   VAR address: EppcAddress);
{ return eppc posting option and address depending on address type }

    BEGIN
        WITH address DO
            IF addrType = LONGINT(typeSessionID) THEN
                BEGIN
                postingOptions := receiverIDisSessionID;
                sendToAddr := Ptr(LONGINTPtr(addrPtr)^);
                END
            ELSE IF addrType = LONGINT(typeTargetID) THEN
                BEGIN
                postingOptions := receiverIDisTargetID;
                sendToAddr := addrPtr;
                END
            ELSE IF addrType = LONGINT(typeProcessSerialNumber) THEN
                BEGIN
                postingOptions := receiverIDisPSN;
                sendToAddr := addrPtr;
                END
            ELSE IF addrType = LONGINT(typeApplSignature) THEN
                BEGIN
                postingOptions := receiverIDisSignature;
                sendToAddr := Ptr(LONGINTPtr(addrPtr)^);
                END
            ELSE
                BEGIN
                postingOptions := 0;
                END;
    END;

{--------------------------------------------------------------------------------}

{ All dispatching goes through AEMDispatcher. }
{ We try dispatching in the following order until it is handled }
{ 1. application's preHandler proc }
{ 2. application's dispatch table }
{ 3. system's preHandler proc }
{ 4. system's dispatch table }

FUNCTION AEMDispatcher(VAR msgDesc: AppleEvent; { --> Pass the message to the handler}
                       VAR theReply: AppleEvent; { --> put reply here }
                       handlerRefcon: LONGINT): OSErr;

    VAR
        err: OSErr;
        aLongErr: LONGINT;
        dummySize: LONGINT;
        returnType: DescType; {Dummy for return of replyrequested}

    FUNCTION TryEventTable(aGlobalRef: GlobalRecHandle): OSErr;

        VAR
            theHandlerRec: HandlerRec;
            err: OSErr;

        BEGIN
            err := errAEEventNotHandled;
            IF aGlobalRef <> NIL THEN
                BEGIN
                { if we have a PreHandler dispatch, go through it first }
                WITH aGlobalRef^^ DO
                    IF phacProcPtr <> NIL THEN
                        err := CallTableDispatch(msgDesc, theReply, 0, phacProcPtr);
                IF err = errAEEventNotHandled THEN
                    BEGIN
                    { search the table for a handler }
                    WITH MsgHdrHdl(msgDesc.dataHandle)^^, aGlobalRef^^ DO
						IF aeHashTable <> NIL THEN
							IF GetTableInfo(aeHashTable, thisAEEventClass, thisAEEventID,
							   theHandlerRec) THEN
								WITH theHandlerRec DO
									IF theProc <> NIL THEN { we have dispatch table entry }
										err := CallTableDispatch(msgDesc, theReply, theRefCon, theProc);
                    END;
                END;
            TryEventTable := err;
        END;

    BEGIN
        err := TryEventTable(GetGlobalRef); { try the application specific dispatch }
        IF err = errAEEventNotHandled THEN err := TryEventTable(GetSysGlobal); { not there, then try
            the system wid one }
        AEMDispatcher := err; { Return Err from dispatch to Server }
    END;

{--------------------------------------------------------------------------------}

{ we first create a empty message and read in the content, then check if it is
    a AppleEvent, if it is we process it by dispatching }
{ if it is an 'aevt' 'ansr', we look at our pending list and put the result there }

FUNCTION AE_ProcessAppleEvent(VAR theEventRecord: EventRecord): OSErr; { --> how should this be
                                                                        passed? }

    LABEL 999, 1000;

    VAR
        theAEEventClass: AEEventClass;
        theAEEventID: AEEventID;
        theReply: AppleEvent; {AEMDispatcher initializes this, we send it }
        msgDesc: AppleEvent;
        msgLen: LONGINT; {msgLen is returned by AcceptEPPC }
        theRefCon: LONGINT;
        err: OSErr;
        tempErr: OSErr;
        sender: TargetID; {Get from AcceptHighLevelEvent}
        msgBufPtr: Ptr;
        aGlobalRef: GlobalRecHandle;

        returnType: DescType; {Dummy for return of replyrequested}
        dummySize: LONGINT; {used for size of repq = 0}

        matchWaitList: Boolean;
        aLongPtr: LONGINTPtr;
        i: integer;

        myAENotifyRec: AENotifyRec;
        metaDesc: AppleEvent;

        curMsgHdl: MsgHdrHdl;

    BEGIN
        IgnoreOSErr(MustHaveGlobal(FALSE, aGlobalRef));
        WITH theEventRecord DO
            BEGIN
            theAEEventClass := AEEventClass(message);
            theAEEventID := AEEventID(where);
            END;

        msgDesc.dataHandle := NIL; {so it won't be free by mistake}
        NukeIt(theReply); {so it won't be free by mistake}
        matchWaitList := FALSE;

        msgLen := 0;
        err := AcceptHighLevelEvent(sender, theRefCon, NIL, msgLen); {Get the length of data}
    { we first check if it for a pending reply, because then we have to read directly
        into the waiting reply }
        IF (LONGINT(theAEEventID) = LONGINT(kAEAnswer)) AND (LONGINT(theAEEventClass) =
           LONGINT(kCoreEventClass)) AND (aGlobalRef <> NIL) THEN
            BEGIN { we check through our list to see if we are waiting for this }
            curMsgHdl := aGlobalRef^^.waitReplyList; { start looking from the head of the list }
            WHILE curMsgHdl <> NIL DO
                BEGIN
                WITH curMsgHdl^^ DO
                    IF ReturnID = theRefCon THEN { id matches, this is the one }
                        BEGIN
                        msgDesc.descriptorType := kCoreEventClass;
                        msgDesc.dataHandle := Handle(curMsgHdl);
                        matchWaitList := TRUE;
                        LEAVE;
                        END
                    ELSE
                        curMsgHdl := waitLink; { otherwise check with the next one }
                END;
            END;

        IF err = bufferIsSmall THEN
            BEGIN { we do have a message content }
            IF matchWaitList THEN { we already have a message waiting }
                WITH MsgHdrHdl(msgDesc.dataHandle)^^ DO
                    BEGIN
                    { leave enought room to read it in }
                    SetHandleSize(msgDesc.dataHandle, SizeOf(MsgHdr) - SizeOf(TargetID) +
                                  msgAddrSize + msgLen);
                    err := MemError;
                    END
            ELSE { otherwise we create a new message for reading }
                BEGIN
                err := CreateBuffer(BAnd(msgLen - SizeOf(AEMetaDesc) + RoundUpValue, RoundUpMask),
                                    SizeOf(TargetID), theAEEventClass, theAEEventID, msgDesc);
                END;
            { we are unlikely to be able even to reply failure, just bail out }
            IF (err <> noErr) THEN GOTO 999;
            HLock(msgDesc.dataHandle);

            WITH MsgHdrHdl(msgDesc.dataHandle)^^ DO
                BEGIN
                msgAddrType := typeTargetID;
                msgAddrSize := SizeOf(TargetID);
                msgBufPtr := Ptr(ord(msgDesc.dataHandle^) + SizeOf(MsgHdr) -
                                 SizeOf(TargetID) + msgAddrSize); {skip AEM's secret data}

        { data is read into the buffer, targetID, refcon is also read directly into message header }
                err := AcceptHighLevelEvent(msgAddrTarget.asTargetID, ReturnID, msgBufPtr, msgLen);
                END;
            HUnLock(msgDesc.dataHandle);
            END
        ELSE IF err = noErr THEN
            BEGIN { there is no content in the message }
            IF NOT matchWaitList THEN
                WITH MsgHdrHdl(msgDesc.dataHandle)^^ DO
                    err := CreateBareMessage(SizeOf(TargetID), theAEEventClass, theAEEventID,
                                             theRefCon, kAnyTransactionID, msgDesc); { create an
                empty message }
            IF (err = noErr) THEN
                BEGIN
                WITH MsgHdrHdl(msgDesc.dataHandle)^^ DO
                    BEGIN
                    msgAddrType := typeTargetID;
                    msgAddrSize := SizeOf(TargetID);
                    msgAddrTarget.asTargetID := sender; {Make an addressObject out of the targetID from
                                              Accetp...}
                    END;
                END;
            END;
        IF err = NoErr THEN
            BEGIN
            err := CheckMessage(MsgHdrHdl(msgDesc.dataHandle));
            { we call the nonAevtHandler if we cannot understand the message }
            IF (err = noErr) THEN
                BEGIN
                IF NOT matchWaitList THEN
                    BEGIN
                    metaDesc := MakeMeta(msgDesc);
                    { if either there is a 'repq' or 'timo' keyword, we want a reply }
                    IF (AE_SizeOfKeyDesc(metaDesc, keyTimeoutAttr, returnType, dummySize) = noErr) |
                       (AE_SizeOfKeyDesc(metaDesc, kReplyRequestedKeyword, returnType, dummySize) =
                       noErr) THEN
                        BEGIN
                        err := CreateDefaultReply(msgDesc, theReply);
                        IF err <> noErr THEN GOTO 999;
                        END;
                    err := AE_ResumeTheCurrentEvent(msgDesc, theReply, @AEMDispatcher, 0);
                    GOTO 1000; { just return the error, no need to handle the error }
                    END;
                END
            ELSE IF (aGlobalRef <> NIL) THEN { error from CheckMessage, then call non-AppleEvent
                                              handler if there is one }
                WITH aGlobalRef^^ DO
                    IF (AENonAevtHandler <> NIL) THEN
                        BEGIN
                        HLock(Handle(msgDesc.dataHandle));
                        tempErr := CallNotAppleEvent(sender, theRefCon, theAEEventClass,
                                                     theAEEventID, msgBufPtr, msgLen,
                                                     AENonAevtHandler);
                        HUnLock(Handle(msgDesc.dataHandle));
                        IF tempErr <> noErr THEN err := tempErr;
                        END;
            END;

    999:
        IF matchWaitList THEN
            BEGIN
            OffWaitList(MsgHdrHdl(msgDesc.dataHandle)); { we got it, no need to wait any more }
            WITH MsgHdrHdl(msgDesc.dataHandle)^^ DO
                BEGIN
                inWaiting := FALSE;
                IF err <> noErr THEN paramCount := - 1; { indicate corrupt data }
                END;
            { if we need to unblock thread, here is the place to do it }
            IgnoreOSErr(TryBothProc(msgDesc, unblockOffset));
            END
        ELSE IF err <> noErr THEN DisposHandle(msgDesc.dataHandle);
    1000:
        AE_ProcessAppleEvent := err;
    END;

{--------------------------------------------------------------------------------}

{ finish process this event by first setting it to current, then execute the
    dispatch routine or redispatch it, send the result back and then dispose
    the message and the reply }

{ it should be noted that the processing of an AppleEvent and the send-direct
    execution also calls this procedure, this is so that it can establish the
    message as the current event being processed and then dispatched, therefore
    we need to check for the send-direct case so we won't dispose the message }

FUNCTION AE_ResumeTheCurrentEvent(VAR theAppleEvent, reply: AppleEvent; { VAR is for efficiency only
                                                                         }
                                  dispatcher: EventHandlerProcPtr;
                                  handlerRefcon: Longint): OSErr;

    LABEL
		999;
		
	VAR
        err: 			OSErr;
        sendErr: 		OSErr; { holding place for send error before we decide which error to report}

        aGlobalRef: 	GlobalRecHandle;

        aLongErr: 		LONGINT;
        thePSN: 		ProcessSerialNumber;
        tempIcon: 		Handle;

        returnType: 	DescType; {Dummy for return of replyrequested}
        dummySize: 		LONGINT; {used for size of repq = 0}
        sendToSelf: 	Boolean;

    BEGIN
		err := CheckAppleEvent(theAppleEvent);
		IF (err <> noErr) THEN goto 999;
        aGlobalRef := GetGlobalRef;
        IgnoreOSErr(AE_SetTheCurrentEvent(theAppleEvent));
        sendToSelf := (LONGINT(MsgHdrHdl(theAppleEvent.dataHandle)^^.msgAddrType) = LONGINT(typeProcessSerialNumber));

        IF ord(dispatcher) = kAEUseStandardDispatch THEN dispatcher := @AEMDispatcher;

        IF dispatcher <> NIL THEN
            err := CallTableDispatch(theAppleEvent, reply, handlerRefcon, dispatcher);

        WITH MsgHdrHdl(theAppleEvent.dataHandle)^^ DO
            BEGIN
            thePSN := switchFromPSN; { get the value before event is disposed }
            notifyRecPtr := NIL; { no longer has a notification, assocaited with the message }
            END;
        sendErr := noErr;
        IF (aGlobalRef <> NIL) AND
           (aGlobalRef^^.currentMsg = MsgHdrHdl(theAppleEvent.dataHandle)) THEN
            BEGIN { it finished without suspension }
            IF reply.dataHandle <> NIL THEN
                BEGIN
                IF err <> noErr THEN
                    BEGIN
                    IF AE_GetKeyPtr(reply, keyErrorNumber, typeLongInteger, returnType, NIL, 0,
                                    dummySize) <> noErr THEN
                        BEGIN { if the application is not already reporting, we shall }
                        aLongErr := err; { extend to 32 bit }
                        IgnoreOSErr(AE_PutKeyPtr(reply, keyErrorNumber, typeLongInteger, @aLongErr,
                                                 4)); { we report back error }
                        END;
                    END;
                IF NOT sendToSelf THEN
                    BEGIN { we know it is not send to self }
                    { Send the Reply, don't worry about a receipt }
                    sendErr := SendMessage(MsgHdrHdl(reply.dataHandle), 0,
                                            kAENormalPriority, 0, NIL, NIL);
                    END;
                IF err = noErr THEN err := sendErr;
                IgnoreOSErr(AE_DisposeDesc(reply)); {dispose of the Reply}
                END;

            IgnoreOSErr(AE_SuspendTheCurrentEvent(theAppleEvent));
            IgnoreOSErr(AE_DisposeDesc(theAppleEvent)); {dispose of the message}
            END
        ELSE
            BEGIN
       { we were being suspended, for the direct call case, this is equivalent to a time out error }
            IF sendToSelf THEN err := errAETimeout;
            END;
        { IF (thePSN.lowLongOfPSN <> 0) OR (thePSN.highLongOfPSN <> 0) THEN }
        IF BOr(thePSN.lowLongOfPSN, thePSN.highLongOfPSN) <> 0 THEN
            IgnoreOSErr(SetFrontProcess(thePSN)); { if we don't need to switch back thePSN is 0 so
                                                   nothing happenss otherwise we switch back }

	999:
        AE_ResumeTheCurrentEvent := err;
    END;

{--------------------------------------------------------------------------------}

    {

    Send this message to the server

    Client: still owns the msg desc and the reply and must eventually deallocate.
    Server: gives up control of reply desc.

    If Client == Server

        short circut the send and not go through EPPC

    Client <> Server
        Set up calls to EPPC and sends
        If receipt is wanted
            ...wait for for return receipt

        If waitReply, then
            ...wait in Eventavail loop for reply message
    }

FUNCTION AE_Send(VAR theAppleEvent: AppleEvent; { --> send this one, VAR is for efficiency only }
                 VAR reply: AppleEvent; { <-- return reply here, AEM does NewHandle }
                 sendMode: AESendMode; { --> e.g. AENoReply, AEWaitReply OR AEWithReceipt }
                 sendPriority: AESendPriority; { --> e.g. AEHighPrioirty, AENormalPriority }
                 timeOutInTicks: LONGINT; { --> number of ticks to wait for reply }
                 idleProc: IdleProcPtr; { Routine to call to spin the cursor }
                 filterProc: EventFilterProcPtr): OSErr;

    LABEL 999;

    VAR
        theAEEventClass: 	AEEventClass;
        theAEEventID: 		AEEventID;
        sendToSelf: 		Boolean;
        dummyType: 			DescType;
        interaction: 		OSType;
        actualSize: 		LONGINT;
        tempReply: 			AppleEvent;
        err: 				OSErr;
        aGlobalRef: 		GlobalRecHandle;
        theSendMode: 		Integer;

    BEGIN
        NukeIt(tempReply); { nil our internal copy of reply first }
		err := CheckAppleEvent(theAppleEvent);
		IF err <> noErr THEN goto 999;
        WITH MsgHdrHdl(theAppleEvent.dataHandle)^^ DO
            BEGIN
            theAEEventClass := thisAEEventClass;
            theAEEventID := thisAEEventID;
            sendToSelf := FALSE;
            { check to see if short circut is possible }
            IF LONGINT(msgAddrType) = LONGINT(typeProcessSerialNumber) THEN
				WITH msgAddrTarget.asPSN DO
					IF lowLongOfPSN = kCurrentProcess THEN
						IF highLongOfPSN = 0 THEN
							sendToSelf := TRUE;
            END;

        theSendMode := BAnd(sendMode, kAEProtocolMask); { Mask of the receipt for comparision }
        CASE BAnd(sendMode, kAEInteractMask) OF
            $00,$40:
                WITH MsgHdrHdl(theAppleEvent.dataHandle)^^ DO
                    BEGIN
                    { the user did not provide one, we determine it for them }
                    IF (msgAddrType = typeTargetID) AND
                       (msgAddrTarget.asTargetID.location.locationKindSelector <> ppcNoLocation) THEN
                        interaction := kNeverInteract { default is no interact for remote machine }
                    ELSE IF theSendMode = kAEWaitReply THEN
                        interaction := kCanSwitchIfInteract { wait reply to same machine should be
                                                             switchable }
                    ELSE
                        interaction := kCanInteract;
                    END;
            $10, $50: interaction := kNeverInteract;
            $20: interaction := kCanInteract;
            $30: interaction := kAlwaysInteract;
            $60: interaction := kCanSwitchIfInteract;
            $70: interaction := kAlwaysSwitchIfInteract;
            { OTHERWISE
                BEGIN
                err := errAEUnknownSendMode;
                GOTO 999;
                END; }
        END;
        { we put in the interact level attribute }
        IF (LONGINT(interaction) <> LONGINT(kCanInteract)) | (AE_GetKeyPtr(MakeMeta(theAppleEvent),
                                                                           kInteractLevelKeyword,
                                                                           typeEnumerated,
                                                                           dummyType, NIL, 4,
                                                                           actualSize) = noErr) THEN
            BEGIN
            err := WriteData(MakeMeta(theAppleEvent), kInteractLevelKeyword, typeEnumerated,
                             @interaction, SizeOf(interaction), 0, 1);
            IF err <> noErr THEN GOTO 999;
            END;
        IF (theSendMode < kFirstLegalReply) OR (theSendMode > kLstLegalRely) THEN
            err := errAEUnknownSendMode
        ELSE IF sendToSelf THEN
            BEGIN
            { this is the short circuit direct send case }
        { since the same message is used by both the client and the server we need to
            take some precaution, we set the in use flag so that we know it will be
            shared and we clean up the access mask }
            IF theSendMode <> kAENoReply THEN
                BEGIN
                err := CreateDefaultReply(theAppleEvent, tempReply);
                IF err <> noErr THEN GOTO 999;
                MsgHdrHdl(tempReply.dataHandle)^^.inUse := TRUE; { about to be used by the server
                                                                  too }
                END;

            WITH MsgHdrHdl(theAppleEvent.dataHandle)^^ DO
                BEGIN
				inUse := TRUE;
                { make sure we have a clean copy of the access mask }
                IF paramCount > 32 THEN { accessMask is a handle, dispose it }
                    DisposHandle(Handle(accessMask));
                accessMask := 0;
                END;
            err := AE_ResumeTheCurrentEvent(theAppleEvent, tempReply, @AEMDispatcher, 0);
            END
        ELSE
            BEGIN { not to self, then actually send a message }
            IF theSendMode = kAENoReply THEN { Client Doesn't want reply}
                BEGIN
                err := SendMessage(MsgHdrHdl(theAppleEvent.dataHandle), sendMode,
                                   sendPriority, timeOutInTicks, idleProc, filterProc);
                END
            ELSE
                BEGIN { Reply created from return message }
                err := SendWithReply(theAppleEvent, tempReply, (theSendMode = kAEWaitReply),
                                     sendPriority, sendMode, timeOutInTicks,
                                     idleProc, filterProc);
                END;
            END;
        IF tempReply.dataHandle <> NIL THEN
            BEGIN
            IF err = errAETimeout THEN
                BEGIN
                { if it is a time out error, we set it up so that it can be handled in the future }
                IF sendToSelf THEN { we don't set it earlier beacuse inWaiting flag will confuse
                                    server }
                    MsgHdrHdl(tempReply.dataHandle)^^.inWaiting := TRUE { server is still using it }
                ELSE
                    BEGIN
                { we add it to the waiting list, if that fails we returns the memory error and
                    the reply will be disposed in error handling below }
                    err := MustHaveGlobal(FALSE, aGlobalRef);
                    IF err = noErr THEN
                        BEGIN
               { this becomes the head of the wait list, and this points to the previous list head }
                        WITH aGlobalRef^^ DO
                            BEGIN
                            MsgHdrHdl(tempReply.dataHandle)^^.waitLink := waitReplyList;
                            waitReplyList := MsgHdrHdl(tempReply.dataHandle);
                            END;
                        err := errAETimeout; { since we destroy its value, we must put it back }
                        END;
                    END;
                END
            ELSE
                BEGIN
                MsgHdrHdl(tempReply.dataHandle)^^.inWaiting := FALSE; { we have got the reply or we
                                                                       will throw it away }
                END;
            END;
    999:
        IF tempReply.dataHandle <> NIL THEN
            BEGIN
            IF err <> noErr THEN IF err <> errAETimeout THEN IgnoreOSErr(AE_DisposeDesc(tempReply));
            END
        ELSE
            tempReply.descriptorType := typeNull;
        { check to make sure one was passed to us in case we have some C programmer }
        IF @reply <> NIL THEN reply := tempReply;
        AE_Send := err;
    END;

{--------------------------------------------------------------------------------}

FUNCTION CheckAppleEvent(VAR theAppleEvent: AppleEvent): OSErr; { VAR is for efficiency only }
	VAR
		err:		OSErr;
	BEGIN
	err := NoErr;
    WITH theAppleEvent DO
        BEGIN
        IF dataHandle = NIL THEN
            err := errAENotAEDesc
        ELSE IF LONGINT(descriptorType) <> LONGINT(kCoreEventClass) THEN
            err := paramErr;
		END;
	CheckAppleEvent := err;
	END;
	
{--------------------------------------------------------------------------------}

{ Creates standard 'aevt''ansr' reply. Stuffs in errorcode, errorstring as passed. }
{ Stuffs refcon from origMsg. }

FUNCTION CreateDefaultReply(VAR origMsg: AppleEvent; { VAR is for efficiency only }
                            VAR theReply: AppleEvent { --> the reply }
                            ): OSErr;

    VAR
        err: OSErr;
        addrType: DescType;
        addrLen: LONGINT;
        transactionID: LONGINT;
        aKey: AEKeyWord;
        wantType: DescType;
        actualSize: LONGINT;

    BEGIN
        { the return address is in the header, we just locate its position and size }
        WITH MsgHdrHdl(origMsg.dataHandle)^^ DO
            BEGIN
            IF LONGINT(msgAddrType) = LONGINT(typeTargetID) THEN
                BEGIN { if it is target ID, we use session ID for efficiency }
                addrType := typeSessionID;
                addrLen := 4;
                END
            ELSE
                BEGIN
                addrType := msgAddrType;
                addrLen := msgAddrSize;
                END;
            END;
        { if origMsg has transaction ID, we sent it back }
        transactionID := kAnyTransactionID; { the default value }
        aKey := keyTransactionIDAttr;
        IgnoreOSErr(ReadData(MakeMeta(origMsg), 0, typeLongInteger, aKey, wantType, @transactionID,
                             SizeOf(transactionID), actualSize));
        WITH MsgHdrHdl(origMsg.dataHandle)^^ DO
            err := CreateBareMessage(addrLen, kCoreEventClass, kAEAnswer, ReturnID, transactionID,
                                     theReply);
        IF err = noErr THEN
            BEGIN
            { we set up the return address }
            BlockMove(Ptr(ord(origMsg.dataHandle^) + SizeOf(MsgHdr) - SizeOf(TargetID)),
                      Ptr(ord(theReply.dataHandle^) + SizeOf(MsgHdr) - SizeOf(TargetID)), addrLen);
            WITH MsgHdrHdl(theReply.dataHandle)^^ DO
                BEGIN
                msgAddrSize := addrLen;
                msgAddrType := addrType;
                userRefcon := MsgHdrHdl(origMsg.dataHandle)^^.userRefcon;
                END;
            err := CheckMessage(MsgHdrHdl(theReply.dataHandle));
            END;
        CreateDefaultReply := err;
    END;

{--------------------------------------------------------------------------------}

FUNCTION EppcFilter(VAR acceptRec: AcceptRecord; {vars used to compare to fields in incoming msg}
                    EPPCBuff: HighLevelEventMsgPtr;
                    VAR senderID: TargetID   { var is for efficency only }
					): Boolean; 
{
    This routine is passed as a procPtr to EPPC's GetSpecificHighLevelEvent.
    It looks at the refcon to match up the reply with the original message.
    Called by GetSpecificHighLevelEvent which is called by WaitGetReply.

    Initially, field AcceptRecord must be SET to the sender's return ID and
    transaction ID which is matched in the reply.

    Field AcceptRecord.aBufferHdl must have been initialized as a Handle to a valid buffer.

    On completion, the reply message (IF any) is accepted and is deposited into
    AcceptRecord.aBufferHdl^^.  AcceptRecord.aErr returns the error code.

    Specification OF ReplyFilter must match the EPPC specification.

    This must NOT be a nested PROCEDURE, gets called from the ToolBox
}

    VAR
        lenMsg: LONGINT;
        processed: Boolean;
		doDispatching: Boolean;
        isAevt: Boolean;
        senderDesc: AEDesc; { AEDesc for who sent this message }
        msgRefcon: LONGINT;
        transactionID: LONGINT;
        switchBefore: LONGINT;
        fromPSN: ProcessSerialNumber;

    BEGIN {ReplyFilter}
        processed := FALSE;
		doDispatching := FALSE;
        WITH acceptRec, EPPCBuff^ DO
            BEGIN
            filterErr := noErr;
            IF userRefcon = msgReturnID THEN { does the refCon match? }
				BEGIN
				IF aBufferHdl = NIL THEN
					BEGIN { we are checking for receipt }
					IF LONGINT(theMsgEvent.message) = LONGINT(HighLevelEventMsgClass) THEN
						IF LONGINT(theMsgEvent.where) = LONGINT(rtrnReceiptMsgID) THEN
							BEGIN
							{ pull it off the queue }
							processed := TRUE; { this is the receipt }
							IF theMsgEvent.modifiers <> msgWasFullyAccepted THEN
								filterErr := errAEEventNotHandled; { the other side did not accept it }
							PullOffTheQueue;
							END;
					END
				ELSE
					BEGIN { we are checking for reply }
					IF LONGINT(theMsgEvent.message) = LONGINT(kCoreEventClass) THEN
						IF LONGINT(theMsgEvent.where) = LONGINT(kAEAnswer) THEN
							BEGIN
							processed := TRUE;
	
							{ get length from secret message header buried in the message }
							lenMsg := msgLength;
							IF lenMsg = 0 THEN lenMsg := SizeOf(AEMetaDesc);
	
							SetHandleSize(Handle(aBufferHdl), SizeOf(MsgHdr) + lenMsg);
							filterErr := MemError;
							IF filterErr = noErr THEN
								BEGIN
								HLock(Handle(aBufferHdl));
								WITH MsgHdrHdl(aBufferHdl)^^ DO
									BEGIN
			{ data is read into the buffer, targetID, refcon is also read directly into message header }
									filterErr := AcceptHighLevelEvent(msgAddrTarget.asTargetID, msgRefcon,
																	  Ptr(ord(Handle(aBufferHdl)^) +
																	  SizeOf(MsgHdr)), lenMsg);
									END;
								IF filterErr = noErr THEN filterErr := CheckMessage(aBufferHdl);
								HUnLock(Handle(aBufferHdl));
								END;
							END
						ELSE IF LONGINT(theMsgEvent.where) = LONGINT(kCanSwitchIfInteract) THEN
							BEGIN
							IF PeekMetaParameter(EPPCBuff, keyBeforeTime, isAevt, switchBefore) THEN
								IF isAevt THEN
									IF GetProcessSerialNumberFromPortName(senderID.name, fromPSN) =
									   noErr THEN
										IF (switchBefore - TickCount) > 1 THEN { and we met the deadline
											   }
											IgnoreOSErr(SetFrontProcess(fromPSN)); { go ahead and do it }
							PullOffTheQueue;
							processed := TRUE;
							filterErr := kNoneOfYourBusiness;
							END
						ELSE IF LONGINT(theMsgEvent.where) = LONGINT(kAEWaitLonger) THEN
							BEGIN
							PullOffTheQueue;
							processed := TRUE;
							filterErr := kAskResetTimer;
							END;
					IF BTst(theMsgEvent.modifiers, 0) THEN { part of the reply process }
						doDispatching := TRUE;
					END;
				END;
            { extraFilter allows you to process AppleEvent which is not the reply }
            { the filter should return TRUE if you are willing to do so for this event }
            IF NOT processed THEN
				BEGIN
                IF extraFilter <> NIL THEN
                    BEGIN
                    IF AE_CreateDesc(typeTargetID, @senderID, SizeOf(TargetID), senderDesc) =
                       noErr THEN
                        BEGIN
                    { we are asking the application to filter it, in order this this we need to
                        scan to find out the tranaction id to pass it back to applicaiton }
                        IF NOT PeekMetaParameter(EPPCBuff, keyTransactionIDAttr, isAevt,
                                                 transactionID) THEN
                            transactionID := 0; { default if transaction ID cannot be found }
                        IF isAevt THEN { we only need to do it for AppleEvents }
                            doDispatching := DoExtraFilter(theMsgEvent, userRefcon, transactionID,
                                                       senderDesc, extraFilter);
                        IgnoreOSErr(AE_DisposeDesc(senderDesc));
                        END;
                    END;
                IF doDispatching THEN IgnoreOSErr(AE_ProcessAppleEvent(theMsgEvent));
				filterErr := kNoneOfYourBusiness;
				END;
            EppcFilter := processed OR doDispatching;
            END;
    END;

{--------------------------------------------------------------------------------}

{ from an EPPC buffer, we want to find out the parameter value of a particular keyword }
{ we use this instead of GetKeyPtr because it has not been converted into an AppleEvent }

FUNCTION PeekMetaParameter(EPPCBuff: HighLevelEventMsgPtr; { the EPPC msg buffer }
                           theAttribute: AEKeyWord; { look for this key word }
                           VAR isAevt: Boolean; { does it look like an AppleEvent? }
                           VAR theValue: LONGINT { value of the keyword attribute }
                           ): Boolean;

    VAR
        aCommonPtr: CommonHeaderPointer;
        dataEnd: LONGINT;

    BEGIN
        PeekMetaParameter := FALSE;
        WITH EPPCBuff^ DO
            BEGIN
            IF msgLength >= 12 THEN
                BEGIN { must be at least this long to be an non empty AppleEvent }
                { we first point to beginning of buffer }
                aCommonPtr := CommonHeaderPointer(ord(EPPCBuff) + HighLevelEventMsgHeaderLength);
                { check for AppleEvent header }
                WITH AEMetaPointer(aCommonPtr)^ DO
                    isAevt := (LONGINT(aevtMarker) = LONGINT(kCoreEventClass)) AND (version =
                              kVersionOne);
                IF isAevt THEN
                    BEGIN
                    dataEnd := ord(aCommonPtr) + msgLength;
                    aCommonPtr := CommonHeaderPointer(ord(aCommonPtr) + 8); { skip the header }
                    WHILE ord(aCommonPtr) < dataEnd DO
                        WITH aCommonPtr^ DO
                            BEGIN
                            IF LONGINT(theKeyword) = LONGINT(aeEndOfMetaDataKeyword) THEN LEAVE; {
                                cannot find it in meta parameter }
                            IF LONGINT(theKeyword) = LONGINT(theAttribute) THEN
                                BEGIN { we found it }
                                IF LONGINT(theType) = LONGINT(typeLongInteger) THEN
                                    BEGIN { must be a LONGINT for this routine }
                                    theValue := LONGINTPtr(ord(aCommonPtr) + SizeOf(commonHeader))^;
                                    PeekMetaParameter := TRUE;
                                    END;
                                LEAVE;
                                END;
                        { aCommonPtr := CommonHeaderPointer(ord(aCommonPtr) + RoundUp(theLength +
                                  SizeOf(CommonHeader))); }
                            aCommonPtr := CommonHeaderPointer(ord(aCommonPtr) + BAnd(theLength +
                                                              SizeOf(CommonHeader) + RoundUpValue,
                                                              RoundUpMask));
                            END;
                    END;
                END
            ELSE IF msgLength = 0 THEN isAevt := TRUE; { an empty message }
            END;
    END;

{--------------------------------------------------------------------------------}
{ take an HLE off the queue by accepting it }

PROCEDURE PullOffTheQueue;

    VAR
        sender: TargetID;
        msgRefcon: LONGINT;
        msgLen: LONGINT;

    BEGIN
        msgLen := 0;
        IgnoreOSErr(AcceptHighLevelEvent(sender, msgRefcon, NIL, msgLen));
    END;

{--------------------------------------------------------------------------------}

{ Used internally to send message buffers. Called from a routine inside AESend }
{ Uses EPPC to send a message and wait for a receipt if required. }

FUNCTION SendMessage(theMsg: MsgHdrHdl;
					 sendMode: LONGINT;
                     sendPriority: integer; { AEHighPriority, AENormalPriority }
                     timeOut: LONGINT; { sender's timeout in ticks }
                     waitingHook: ProcPtr; {Routine to call to spin the cursor}
                     extraHandlingFilter: ProcPtr { extra event processing filter }
                     ): OSErr;

    LABEL 999;

    VAR
        theEvent: EventRecord;
        address: EppcAddress;
        acceptRec: AcceptRecord;
        done: Boolean;
        gotEvent: Boolean;
        withReceipt: Boolean;
        withReconnect: Boolean;
        msgSize: LONGINT;
        err: OSErr;
        theRefCon: LONGINT;
        headerSize: LONGINT;
        coercedTarget: AEDesc; { if not a known address mode, then try to coerce it to one in here }

    BEGIN
        coercedTarget.dataHandle := NIL;
        HLock(Handle(theMsg));
        WITH theMsg^^ DO AddrTypeToPostingOptions(LONGINT(msgAddrType), @msgAddrTarget, address);
        { if address mode is not known, try to coerce to something that eppc can understand }
        IF address.postingOptions = 0 THEN
            BEGIN
            { try to coerce to a type that eppc can understand }
            IF TryEppcFail(theMsg, typeProcessSerialNumber, coercedTarget) THEN
                IF TryEppcFail(theMsg, typeTargetID, coercedTarget) THEN
                    IF TryEppcFail(theMsg, typeSessionID, coercedTarget) THEN
                        IF TryEppcFail(theMsg, typeApplSignature, coercedTarget) THEN
                            BEGIN
                            err := errAEUnknownAddressType;
                            GOTO 999;
                            END;
            WITH coercedTarget DO
                BEGIN
                HLock(dataHandle);
                AddrTypeToPostingOptions(LONGINT(coercedTarget.descriptorType), dataHandle^,
                                         address);
                END;
            END;
        WITH theMsg^^ DO
            BEGIN
            theEvent.message := LONGINT(thisAEEventClass);
            LONGINT(theEvent.where) := LONGINT(thisAEEventID);
			IF BTst(sendMode, kBitPartOfReply) THEN
            	theEvent.modifiers := 1
			ELSE
            	theEvent.modifiers := 0;
            theRefCon := ReturnID;
            END;

        withReceipt := BTst(sendMode, kBitWantReceipt);
        withReconnect := NOT BTst(sendMode, kBitDontReconnect);

        IF withReceipt THEN BSet(address.postingOptions, kBitWantReceipt);

        IF sendPriority = kAEHighPriority THEN BSet(address.postingOptions, kBitAttnMsg);

        {Send the message.  Include the metadata but not the message header}
        {Point ePPC past the msgHdr so add size to it.  Pass size as msg - msgHeader}

        headerSize := SizeOf(MsgHdr) - SizeOf(TargetID) + theMsg^^.msgAddrSize;
        msgSize := GetHandleSize(Handle(theMsg)) - headerSize;
        IF msgSize <= SizeOf(AEMetaDesc) THEN msgSize := 0; { There is no message buffer, so it is
                                                             an empty message }

        done := FALSE;
        REPEAT
            WITH address DO
                err := PostHighLevelEvent(theEvent, sendToAddr, theRefCon, Ptr(ord(theMsg^) +
                                          headerSize), msgSize, postingOptions);
            IF withReconnect AND (err = sessClosedErr) THEN
                withReconnect := FALSE { if withReconnect is TRUE, we are willing to do 1 more time
                                        }
            ELSE
                done := TRUE; { otherwise that is it }
        UNTIL done;

        { if the temporary target was from coercion, we no longer need it }
        WITH coercedTarget DO
            IF dataHandle <> NIL THEN { avoid trap call if possible }
                DisposHandle(dataHandle);
        IF withReceipt THEN
            IF (err = noErr) THEN
                BEGIN { if need to wait for the receipt }
				WITH acceptRec DO
					BEGIN
                    extraFilter := extraHandlingFilter;
                	msgReturnID := theRefCon; { make a record for the reciept filter }
					aBufferHdl := NIL; { to indicate this is for receipt }
					END;
                err := GeneralWait(timeOut, waitingHook, @EppcFilter, @acceptRec);
                END;
    999:
        HUnLock(Handle(theMsg));

        SendMessage := err;
    END;

{--------------------------------------------------------------------------------}

{ Call SendMessage. Create Reply buffer. }
{ Call Do EventAvail and GetSpecificHighLevelEvent until got msg or timeout }

FUNCTION SendWithReply(VAR msg: AppleEvent; { VAR is for efficiency only }
                       VAR reply: AppleEvent; {Uninitialized, we create it(pass to createmsg)}
                       isWaitReply: Boolean; { Queue or Wait for a reply after sending }
                       sendPriority: integer; { AEHighPriority, AENormalPriority }
					   sendMode: LONGINT;
                       timeOut: LONGINT; {sender's timeout in ticks}
                       waitingHook: ProcPtr; {Routine to call to spin the cursor}
                       extraHandlingFilter: ProcPtr { extra event processing filter }
                       ): OSErr;

    VAR
        err: OSErr;
        acceptRec: AcceptRecord; {Contains msgRefcon,the replyBuffer,aErr}
        typeCode: DescType;
        actualSize: LONGINT;

    BEGIN {SendWaitReply}
        err := noErr;
        CalculateTimeout(timeOut); { convert default to actual value }
        IF timeOut <> 0 THEN
            BEGIN { we put out a time out attriubte to inform the server }
			err := WriteData(MakeMeta(msg), keyTimeoutAttr, typeLongInteger, @timeOut, 4, 0, 1);{
				send time out over }
            END
        ELSE { otherwise 'repq' is used to indicate a reply is needed }
            err := WriteData(MakeMeta(msg), kReplyRequestedKeyword, typeTrue, NIL, 0, 0, 1); {add
            reply requested as meta}
        IF err = noErr THEN
        { now we send out the message }
            err := SendMessage(MsgHdrHdl(msg.dataHandle), sendMode, sendPriority,
                               timeOut, waitingHook, extraHandlingFilter);

        IF err = noErr THEN
            IF isWaitReply THEN
                BEGIN { we are going to wait for the reply }
                { we set up buffer and accept record for the reply filter }
                WITH MsgHdrHdl(msg.dataHandle)^^ DO
                    err := CreateBareMessage(SizeOf(TargetID), kCoreEventClass, kAEAnswer, ReturnID,
                                             kAnyTransactionID, reply);
                IF err = noErr THEN
                    BEGIN
                    WITH MsgHdrHdl(reply.dataHandle)^^ DO
                        BEGIN
                        msgAddrType := typeTargetID;
                        msgAddrSize := SizeOf(TargetID);
                        userRefcon := MsgHdrHdl(msg.dataHandle)^^.userRefcon;
                        inWaiting := TRUE;
						WITH acceptRec DO
							BEGIN
							extraFilter := extraHandlingFilter;
							aBufferHdl := MsgHdrHdl(reply.dataHandle); { This sets up the reply!!!!}
							msgReturnID := ReturnID
							END;
                        END;
                    err := GeneralWait(timeOut, waitingHook, @EppcFilter, @acceptRec);
                    END;
                END;
        SendWithReply := err;
    END;

{--------------------------------------------------------------------------------}

FUNCTION TryEppcFail(theMsg: MsgHdrHdl;
                     toType: DescType;
                     VAR coercedTarget: AEDesc): Boolean;
    { here we are trying to coerce it something eppc can understand,
      doing it this way we have all the overhead of coercion
      in future version we may try to find corecion routine directly
      in stead of using AE_CoercePtr }

    BEGIN
        WITH theMsg^^ DO
            TryEppcFail := (AE_CoercePtr(msgAddrType, @msgAddrTarget, msgAddrSize, toType,
                                         coercedTarget) <> noErr);
    END;

{--------------------------------------------------------------------------------}
