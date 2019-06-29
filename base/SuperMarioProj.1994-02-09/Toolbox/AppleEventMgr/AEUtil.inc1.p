{
	File:		AEUtil.inc1.p

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <5>	  2/5/91	Lai		BM,#81596: Also for bug jfr101 on whiteboard. In AECreateDesc
									take care of case when it is a reco/list, code is moved from
									AECoercion.inc1.p.
		 <4>	 1/17/91	Lai		Return the correct error when creating a null desc
		 <3>	 1/11/91	Lai		AECreateDesc now returns null desc only if typeCode is 'null'.
		 <2>	12/21/90	Lai		TryBothProc not returning the error causing AEDisposeDesc not to
									dispose the handle.
		 <1>	12/13/90	Lai		first checked in

	To Do:
}

{--------------------------------------------------------------------------------}

FUNCTION CallOneParamProc(theDesc: AEDesc;
                          callproc: ProcPtr): OSErr;
{ inline is equivalent to : movea.l (a7)+,a0; jsr (a0) }
    INLINE $205F, $4E90;

{--------------------------------------------------------------------------------}

FUNCTION CallWaitHook(VAR theEvent: EventRecord;
                      VAR sleep: LONGINT;
                      VAR mouseRgn: RgnHandle;
                      theProc: ProcPtr): Boolean;
{ inline is equivalent to : movea.l (a7)+,a0; jsr (a0) }
    INLINE $205F, $4E90;

{--------------------------------------------------------------------------------}

FUNCTION AE_CreateList(factoringPtr: Ptr;
                       factoredSize: LONGINT;
                       isRecord: boolean;
                       VAR resultList: AEDesc): OSErr;
    EXTERNAL;

{--------------------------------------------------------------------------------}

{ for now it is just a PtrToHand, if we ever to a #XXX descriptor type, we need
    to change this }

FUNCTION AE_CreateDesc(typeCode: DescType;
                       dataPtr: Ptr;
                       dataSize: LONGINT;
                       VAR result: AEDesc): OSErr;
	VAR
		err:	OSErr;
    BEGIN
		err := NoErr;
        WITH result DO
            BEGIN
            descriptorType := typeCode;
            dataHandle := NIL;
			IF (LONGINT(typeCode) = LONGINT(typeAEList)) OR (LONGINT(typeCode) = LONGINT(typeAERecord)) THEN
				BEGIN
				err := PtrToHand(Ptr(ord(dataPtr) - SizeOf(ListClassRec)), dataHandle,
								 dataSize + SizeOf(ListClassRec));
				IF err = NoErr THEN
					BEGIN
					WITH ListHdrHdl(dataHandle)^^ DO
						BEGIN
						WITH listClassHeader DO
							BEGIN
							hintIndex := 0;
							paramBegin := BAnd(sizeOfPrefix + SizeOf(ListHdr) + RoundUpValue,
											   RoundUpMask);
							metaCountOrObjType := LONGINT(typeCode);
							END;
						END;
					END;
				END
            ELSE IF LONGINT(typeCode) <> LONGINT(typeNull) THEN 
				err := PtrToHand(dataPtr, dataHandle, dataSize);
            IF dataHandle = NIL THEN descriptorType := typeNull;
            END;
		AE_CreateDesc := err;
    END;

{--------------------------------------------------------------------------------}

{ duplicate a desc, again take special care for AppleEvent }

FUNCTION AE_DuplicateDesc(VAR theAEDesc: AEDesc; { VAR is for efficiency only }
                          VAR result: AEDesc): OSErr;

    VAR
        err: OSErr;

    BEGIN
        result := theAEDesc;
        WITH result DO
            BEGIN
			IF dataHandle = NIL THEN { just copy it is good enough }
				err := noErr
			ELSE
				BEGIN
				err := HandToHand(dataHandle);
				IF (err = noErr) THEN
					BEGIN
					IF (LONGINT(descriptorType) = LONGINT(kCoreEventClass)) THEN
						WITH MsgHdrHdl(dataHandle)^^ DO
							BEGIN
							inUse := FALSE;
							accessMask := 0;
							END;
					END
				ELSE
					NukeIt(result);
				END;
            END;
        AE_DuplicateDesc := err;
    END;

{--------------------------------------------------------------------------------}

{ get the interact level allowed when used as a server }
{ it is in the global, just get it from there }

FUNCTION AE_GetInteractionAllowed(VAR level: AEInteractAllowed): OSErr;

    VAR
        aGlobalRef: GlobalRecHandle;

    BEGIN
        AE_GetInteractionAllowed := noErr;
        aGlobalRef := GetGlobalRef;
        IF aGlobalRef <> NIL THEN
            level := aGlobalRef^^.interactAllowance
        ELSE { otherwise it is the default }
            level := kAEInteractWithLocal;
    END;

{--------------------------------------------------------------------------------}

{ return the AppleEvent being processed, just fetch the value from the global }

FUNCTION AE_GetTheCurrentEvent(VAR theAppleEvent: AppleEvent): OSErr;

    VAR
        aGlobalRef: GlobalRecHandle;

    BEGIN
        WITH theAppleEvent DO
            BEGIN
            descriptorType := kCoreEventClass;
            dataHandle := NIL;
			aGlobalRef := GetGlobalRef;
			IF aGlobalRef <> NIL THEN 
				dataHandle := Handle(aGlobalRef^^.currentMsg);
			IF dataHandle = NIL THEN
            	descriptorType := typeNull;
            END;
        AE_GetTheCurrentEvent := noErr;
    END;

{--------------------------------------------------------------------------------}

{ set the interact level allowed when used as a server }
{ it is in the global, just put it there }

FUNCTION AE_SetInteractionAllowed(level: AEInteractAllowed): OSErr;

    VAR
        aGlobalRef: GlobalRecHandle;

    BEGIN
        AE_SetInteractionAllowed := MustHaveGlobal(FALSE, aGlobalRef);
        IF aGlobalRef <> NIL THEN aGlobalRef^^.interactAllowance := level;
    END;

{--------------------------------------------------------------------------------}
{ make the current event, if there is a current event, we push it up the stack
    chain so that we can return to it when we are done with it }

FUNCTION AE_SetTheCurrentEvent(VAR theAppleEvent: AppleEvent): OSErr; { VAR is for efficiency only }

    VAR
        aGlobalRef: GlobalRecHandle;
        thisMsg: MsgHdrHdl;

    BEGIN
        AE_SetTheCurrentEvent := noErr;
        aGlobalRef := GetGlobalRef;
        thisMsg := MsgHdrHdl(theAppleEvent.dataHandle);
        IF (aGlobalRef <> NIL) AND (thisMsg <> NIL) THEN
            WITH aGlobalRef^^ DO
                IF currentMsg <> thisMsg THEN
                    BEGIN { from now until we return from dispatcher, this is the current message }
                    thisMsg^^.prevMsg := currentMsg;
                    currentMsg := thisMsg;
                    END;
    END;

{--------------------------------------------------------------------------------}

FUNCTION AE_SuspendTheCurrentEvent(VAR theAppleEvent: AppleEvent): OSErr; { VAR is for efficiency
                                                                           only }

    VAR
        aGlobalRef: GlobalRecHandle;

    BEGIN
        AE_SuspendTheCurrentEvent := noErr;
        aGlobalRef := GetGlobalRef;

        IF aGlobalRef <> NIL THEN
            WITH aGlobalRef^^ DO
                BEGIN
                IF currentMsg = MsgHdrHdl(theAppleEvent.dataHandle) THEN
                    currentMsg := currentMsg^^.prevMsg;
                END;

    END;

{--------------------------------------------------------------------------------}

{ calculate the timeout value, for now, default is just a constant }

PROCEDURE CalculateTimeOut(VAR timeOut: longint);

    BEGIN
        IF timeOut = kAEDefaultTimeout THEN
            timeOut := standardTimeOut
        ELSE IF timeOut = kNoTimeOut THEN
            timeOut := MaxLONGINT
        ELSE IF timeOut < 0 THEN timeOut := 0;
    END;

{--------------------------------------------------------------------------------}

{ for creating a list or a record }

FUNCTION CreateList(factoringPtr: Ptr;
                    factoredSize: LONGINT;
                    isRecord: boolean;
                    VAR resultList: AEDesc;
                    objectType: DescType): OSErr;
{ factoringPtr is ptr to buffer containing header of list }
{ factoredSize is size of that buffer }
{ dataPtr does not include the header }

    VAR
        aHandle: Handle;
        err: OSErr;

    BEGIN
        aHandle := NIL;
        IF factoringPtr = NIL THEN factoredSize := 0;
        IF (factoredSize = 0) OR (factoredSize = 4) OR (factoredSize >= 8) THEN
            BEGIN
            aHandle := NewHandleClear(SizeOf(ListHdr) + BAnd(factoredSize + RoundUpValue,
                                                             roundUpMask));
            IF aHandle <> NIL THEN
                BEGIN
                WITH ListHdrHdl(aHandle)^^ DO
                    BEGIN
                    WITH listClassHeader DO
                        BEGIN
                        paramBegin := SizeOf(ListHdr) + BAnd(factoredSize + RoundUpValue,
                                                             roundUpMask);
                        WITH resultList DO
                            BEGIN
                            IF isRecord THEN
                                BEGIN
                                descriptorType := typeAERecord;
                                IF LONGINT(objectType) = 0 THEN
                                    metaCountOrObjType := LONGINT(descriptorType)
                                ELSE
                                    metaCountOrObjType := LONGINT(objectType);
                                END
                            ELSE
                                BEGIN
                                descriptorType := typeAEList;
                                metaCountOrObjType := LONGINT(descriptorType);
                                END;
                            END;
                        END;
                    IF factoredSize <> 0 THEN { put in the factoring data }
                        BEGIN
                        sizeOfPrefix := factoredSize;
                        BlockMove(factoringPtr, Ptr(ord(aHandle^) + SizeOf(ListHdr)), factoredSize);
                        END;
                    END;
                err := noErr;
                END
            ELSE
                err := MemError;
            END
        ELSE
            BEGIN
            { only legal values are 0=unfactored, 4=homogeneous, 8=homogeneous+fixedSized or above }
            err := paramErr;
            END;
        WITH resultList DO
            BEGIN
            dataHandle := aHandle;
            IF err <> noErr THEN descriptorType := typeNull;
            END;
        CreateList := err;
    END;

{--------------------------------------------------------------------------------}

{ given a message, determine its source }

PROCEDURE EventSource(theMsgHdl: MsgHdrHdl;
                      VAR theAEEventSource: AEEventSource;
                      VAR fromPSN: ProcessSerialNumber);

    VAR
        myPSN: ProcessSerialNumber;
        sameOne: boolean;

    BEGIN
        theAEEventSource := kAEUnknownSource;
        IF theMsgHdl <> NIL THEN
            WITH theMsgHdl^^ DO
                BEGIN
                IF LONGINT(msgAddrType) = LONGINT(typeTargetID) THEN { we know it is not send to
                                                                      self }
                    BEGIN
                    IF msgAddrTarget.asTargetID.location.locationKindSelector = ppcNoLocation THEN
                        BEGIN { process is in the same machine }
						WITH myPSN DO
							BEGIN
							highLongOfPSN := 0;
							lowLongOfPSN := kCurrentProcess;
							END;
						sameOne := FALSE;
                        IF GetProcessSerialNumberFromPortName(msgAddrTarget.asTargetID.name, fromPSN) =
                           noErr THEN
                            IgnoreOSErr(SameProcess(fromPSN, myPSN, sameOne));
                        IF sameOne THEN
                            theAEEventSource := kAESameProcess
                        ELSE
                            theAEEventSource := kAELocalProcess;
                        END
                    ELSE
                        theAEEventSource := kAERemoteProcess;
                    END
                ELSE IF LONGINT(msgAddrType) = LONGINT(typeProcessSerialNumber) THEN
                    BEGIN
					WITH msgAddrTarget.asPSN DO
						IF lowLongOfPSN = kCurrentProcess THEN
							IF highLongOfPSN = 0 THEN
                    			theAEEventSource := kAEDirectCall;
                    END;
                END;
    END;

{--------------------------------------------------------------------------------}

{ this is the common routine for waiting until timeout and pass back some event to application }
{ this routine is shared between the case where you are filtering high level event, and the case
    where you are waiting to come to the front during AEInteractWith User }
{ if this routine is called with a filterProc, it is considered to be the former case, if filter
    proc is nil, it is the latter case }

FUNCTION GeneralWait(VAR timeOut: LONGINT;
                     waitingHook: ProcPtr;
                     filterProc: ProcPtr;
                     filterInfo: Ptr): OSErr;

    VAR
        err: OSErr;
        done: Boolean;
        passBackEvent: EventRecord;
        sleep: LONGINT;
        mouseRgn: RgnHandle;
        expiredTime: LONGINT;
        waitMask: integer;
        myPSN: ProcessSerialNumber; { the PSN of this process }
        frontPSN: ProcessSerialNumber; { the PSN of the front process }
        { a evil nested procedure }

    FUNCTION CheckWaitHook: Boolean;

        BEGIN
            CheckWaitHook := FALSE;
            IF waitingHook <> NIL THEN { do some idle time processing }
                BEGIN
                waitMask := BXOR(waitMask, updateMask); { toggle it }
                IF CallWaitHook(passBackEvent, sleep, mouseRgn, waitingHook) THEN { user cancel }
                    BEGIN
                    err := errAEWaitCanceled;
                    CheckWaitHook := TRUE;
                    END;
                END;
        END;

    BEGIN
        done := FALSE;
        err := noErr;
        CalculateTimeOut(timeOut);
        expiredTime := TickCount + timeOut;
        waitMask := 0; { nothing to look at }
 { these are default value in case there is no wait hook and they did not bother to return a value }
        sleep := 1; { just yield }
        mouseRgn := NIL; { no mouse region }
        IF waitingHook <> NIL THEN
            BEGIN
        { we can pass these event back to the application, we do not set update mask here even
            we want it because we will toggle the update mask bit every time just in case
            the update event was never processed by the idle proc and we can never get to the
            high level event }
            waitMask := activMask + app4Mask;
            { Fake a null event and pass it to idle proc for the first time to get mouseRgn }
            IF OSEventAvail(0, passBackEvent) THEN; { generate a dummy null event }
            done := CheckWaitHook;
            END;

        { now we loop until reply received, timeout or user cancel }

        WHILE NOT done DO
            BEGIN { until got reply or timeout or error}
            IF filterProc <> NIL THEN
                BEGIN { we want to do a filter on high level event }
                IF GetSpecificHighLevelEvent(filterProc, filterInfo, err) THEN
                    WITH AcceptRecordPtr(filterInfo)^ DO
                        BEGIN
                        IF filterErr = kAskResetTimer THEN
                            expiredTime := TickCount + timeOut { it is 'aevt' 'wait' }
                        ELSE IF filterErr <> kNoneOfYourBusiness THEN 
                            BEGIN
                            done := TRUE;
                            IF err = noErr THEN { the error is either err or in the accept record }
                                err := filterErr;
                            END;
                        END;
                END
            ELSE { if we are not checking on high level event, then we are checking for come to
                  front }
                BEGIN
                IgnoreOSErr(GetCurrentProcess(myPSN));
                IgnoreOSErr(GetFrontProcess(frontPSN));
                IgnoreOSErr(SameProcess(myPSN, frontPSN, done));
                END;
            IF NOT done THEN
                BEGIN
                timeOut := expiredTime - TickCount; { we keep track of remaining time }
            { note that this line should be timeOut > 0 and not expiredTime > TickCount,
                because it would work even if expiredTime is negative }
                IF timeOut > 0 THEN { we are still waiting }
                    BEGIN
                    IF sleep > timeOut THEN sleep := timeOut;
                    IF WaitNextEvent(waitMask, passBackEvent, sleep, mouseRgn) THEN; { so that we
                        can yield if necessary }
                    IF CheckWaitHook THEN LEAVE;
                    END
                ELSE { otherwise it is time out }
                    BEGIN
                    err := errAETimeout;
                    LEAVE;
                    END;
                END;
            END;
    { we return the remaining time out because wait for reciept may use up some time and then
        timeOut left for wait reply would have to be reduced }
        IF timeOut < 0 THEN timeOut := 0;
        GeneralWait := err;
    END;

{--------------------------------------------------------------------------------}

FUNCTION MakeMeta(VAR theAevt: AppleEvent): AppleEvent; { VAR is for efficiency only }

    BEGIN
        MakeMeta.dataHandle := theAevt.dataHandle;
        MakeMeta.descriptorType := typeMeta;
    END;

{--------------------------------------------------------------------------------}

{ get a application/system global, initialize if necessary }

FUNCTION MustHaveGlobal(isSysHandler: Boolean;
                        VAR aGlobalRef: GlobalRecHandle): OSErr;

    TYPE
        GlobalRecHandlePtr = ^GlobalRecHandle;

    VAR
        err: OSErr;
        aGlobalHandle: GlobalRecHandle;
        aGlobalRecHandlePtr: GlobalRecHandlePtr;

    BEGIN
        err := noErr;
        IF isSysHandler THEN
            aGlobalRecHandlePtr := GlobalRecHandlePtr(LongIntPtr(ExpandMem)^ + kAESysGlobalOffset)
        ELSE
            aGlobalRecHandlePtr := GlobalRecHandlePtr(LongIntPtr(ExpandMem)^ + kAEAppGlobalOffset);
        aGlobalRef := aGlobalRecHandlePtr^;
        IF aGlobalRef = NIL THEN
            BEGIN
            IF isSysHandler THEN
                aGlobalHandle := GlobalRecHandle(NewHandleSysClear(SizeOf(SysGlobalRec))) { create a
                                                     new one }
            ELSE
                aGlobalHandle := GlobalRecHandle(NewHandleClear(SizeOf(GlobalRec))); { create a new
                one }
            IF aGlobalHandle = NIL THEN
                err := MemError
            ELSE
                BEGIN
                IF NOT isSysHandler THEN
                    WITH aGlobalHandle^^ DO
						BEGIN { these field only available for the application global }
						ReturnIDCounter := $10000 + LoWrd(TickCount); { because high word should
																	   not be zero }
						interactAllowance := kAEInteractWithLocal;
						END;
				aGlobalRecHandlePtr^ := aGlobalHandle;
				aGlobalRef := aGlobalHandle;
                END;
            END;
        MustHaveGlobal := err;
    END;

{--------------------------------------------------------------------------------}

PROCEDURE NukeIt(VAR theAEDesc: AEDesc);

    BEGIN
        WITH theAEDesc DO
            BEGIN
            descriptorType := typeNull;
            dataHandle := NIL;
            END;
    END;

{--------------------------------------------------------------------------------}

{ Wait list is the list of message that has a reply handle but no content }
{ this proceudre removes a message from the wait list either because the content
    has arrived or the message is disposed }
{ the wait list is a linked list of message, if the message is found in the linked list
    remove it from the linked list }

PROCEDURE OffWaitList(aHandle: MsgHdrHdl);

    VAR
        aGlobalRef: GlobalRecHandle;
        curMsgHdl: MsgHdrHdl;
        nextMsgHdl: MsgHdrHdl;

    BEGIN
        aGlobalRef := GetGlobalRef;
        IF aGlobalRef <> NIL THEN
            WITH aGlobalRef^^ DO
                BEGIN
                curMsgHdl := waitReplyList;
                IF aHandle = curMsgHdl THEN
                { if it is the head of list, then 2nd one becomes head of list }
                    waitReplyList := curMsgHdl^^.waitLink
                ELSE
                    BEGIN
                    WHILE curMsgHdl <> NIL DO
                        BEGIN
                        nextMsgHdl := curMsgHdl^^.waitLink; { next item in the link list }
                        IF nextMsgHdl = aHandle THEN
                            BEGIN { next item is the one to be removed }
                            { make wait link point to item after the next one }
                            curMsgHdl^^.waitLink := nextMsgHdl^^.waitLink;
                            LEAVE;
                            END
                        ELSE
                            curMsgHdl := nextMsgHdl; { look for the next one }
                        END;
                    END;
                END;
    END;

{--------------------------------------------------------------------------------}

FUNCTION TryBothProc(VAR theDesc: AEDesc;
                     procOffset: LONGINT): OSErr;

    VAR
        err: OSErr;

    FUNCTION TryOneProc(aGlobalRef: GlobalRecHandle): OSErr;

        VAR
            aProcPtr: ProcPtr;

        BEGIN
            TryOneProc := errAEEventNotHandled;
            IF aGlobalRef <> NIL THEN
                BEGIN
                aProcPtr := ProcPtr(LONGINTPtr(ord(aGlobalRef^) + procOffset)^);
                IF aProcPtr <> NIL THEN TryOneProc := CallOneParamProc(theDesc, aProcPtr);
                END;
        END;

    BEGIN
        err := TryOneProc(GetGlobalRef);
        IF err = errAEEventNotHandled THEN err := TryOneProc(GetSysGlobal); { not there, then try
            the system wide one }
		TryBothProc := err;
    END;

{--------------------------------------------------------------------------------}
