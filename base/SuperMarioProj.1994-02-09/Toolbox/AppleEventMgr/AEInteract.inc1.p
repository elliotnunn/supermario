{
	File:		AEInteract.inc1.p

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <4>	 3/15/91	Lai		BM,#83128: Remove notification when interacting with event from
									another process.
		 <3>	 2/13/91	Lai		BM,#82759: In AE_InteractWithUser, timeOutInTicks are not
									translated to actual value before attempting to switch layer,
									now this is fixed by calling CalculateTimeOut in the front of
									the procedure.
		 <2>	 1/11/91	Lai		change msgAddrTargetID to match change in AEUtil.p
		 <1>	12/13/90	Lai		first checked in

	To Do:
}

{--------------------------------------------------------------------------------}

{ ask the front application to take me to the front by sending an AppleEvent }
{ later write to use AEM calls to see if it is shorter }

PROCEDURE AskToComeToFront(theMsg: MsgHdrHdl;
                           waitTillTime: LONGINT);

    TYPE
        CansBuffer = RECORD
                         aevtMarker: AEKeyWord;
                         version: LONGINT;
                         timeOutKeyword: AEKeyWord;
                         timeOutType: DescType;
                         timeOutSize: LONGINT;
                         timeOutValue: LONGINT;
                         metaTerminator: AEKeyWord;
                     END;

    VAR
        theEvent: EventRecord;
        aCansBuffer: CansBuffer;

    BEGIN
        WITH theEvent DO
            BEGIN
            theEvent.message := LONGINT(kCoreEventClass);
            LONGINT(where) := LONGINT(kCanSwitchIfInteract);
            modifiers := 0;
            END;
        WITH aCansBuffer DO
            BEGIN
            aevtMarker := kCoreEventClass;
            version := kVersionOne;
            metaTerminator := aeEndOfMetaDataKeyword;
            timeOutKeyword := keyBeforeTime;
            timeOutType := typeLongInteger;
            timeOutSize := SizeOf(LONGINT);
            timeOutValue := waitTillTime;
            END;
        WITH theMsg^^ DO
            IgnoreOSErr(PostHighLevelEvent(theEvent, Ptr(msgAddrTarget.asTargetID.sessionID), returnID,
                                           @aCansBuffer, SizeOf(aCansBuffer),
                                           receiverIDisSessionID));
    END;

{--------------------------------------------------------------------------------}


{ we try to determine if we can interact with the user }
{ may be no interaction is allowed either because the client don't want it or the
    server machine don't want to have any interaction }
{ if interaction is allowed, we still need to have get a change to do it }
{ if the server is the front application, you get it immediately }
{ if the server is not in front, switch to front if the client is in front and
    grant you the permission to go to the front, otherwise post a notification and
    wait until your application is in the front }

FUNCTION AE_InteractWithUser(timeOutInTicks: LONGINT; { how long are you willing to wait }
                             nmReqPtr: NMRecPtr; { your own custom notification }
                             idleProc: IdleProcPtr { your own idle procedure }
                             ): OSErr; { return the error }

    LABEL 999, 1000;

    VAR
        myPSN, frontPSN, fromPSN: ProcessSerialNumber;
        err: OSErr;
        myNMRec: NMRec;
        nowInFront, localProcess, switchDirectly: Boolean;
        interactType: OSType;
        dummyType: DescType;
        actualSize: LONGINT;
        waitSetFrontProcess: LONGINT;
        curInteract: AEInteractAllowed;
        flag: SignedByte;
        scaled: Boolean;
        aevtSource: AEEventSource;
        currentDesc: AppleEvent;
		
	{ this may be nested procedure but actual compiled code show it is shorter }

    FUNCTION GetDefaultID(VAR theID: integer): Boolean;
    { get the ID of the application ICON }

        VAR
            tempHandle: Handle;
            i, j, k, localID: integer;
            count1: integer;
            count2: integer;
            countFREF: integer;
            countBNDL: integer;
            aPtr: IntegerPtr;
            hasID: Boolean;
            aProcessInfoRec: ProcessInfoRec;

        BEGIN
            GetDefaultID := FALSE;
            WITH aProcessInfoRec DO
                BEGIN
                processInfoLength := SizeOf(ProcessInfoRec); { we only need processSignature }
                processName := NIL; { who cares }
                processAppSpec := NIL;
                END;
            IF GetProcessInformation(myPSN, aProcessInfoRec) = noErr THEN
                BEGIN
                countFREF := CountResources('FREF');
            { we first try to find the resource ID, by first looking at FREF resource and find the local ID
              of APPL, then we look at the BNDL resource to find the ID corresponding to the local ID }
                FOR i := 1 TO countFREF DO
                    BEGIN
                    { look through all 'FREF' resources }
                    tempHandle := GetIndResource('FREF', i);
                    IF (tempHandle <> NIL) & (LONGINTPtr(tempHandle^)^ = LONGINT('APPL')) THEN
                        BEGIN
                        localID := IntegerPtr(ord(tempHandle^) + 4)^;
                        countBNDL := CountResources('BNDL');
                        FOR j := 1 TO countBNDL DO
                            BEGIN { we try to find the BNDL from this application }
                            tempHandle := GetIndResource('BNDL', j);
                            IF (tempHandle <> NIL) & (LONGINTPtr(tempHandle^)^ =
                               LONGINT(aProcessInfoRec.processSignature)) THEN
                                BEGIN { this is the right BNDL }
                                aPtr := IntegerPtr(ord(tempHandle^) + 6);
                                count1 := aPtr^;
                                aPtr := IntegerPtr(ord(aPtr) + 2);
                                FOR j := 0 TO count1 DO
                                    BEGIN
                                    IF LONGINTPtr(aPtr)^ = LONGINT('FREF') THEN
                                        BEGIN
                                        aPtr := IntegerPtr(ord(aPtr) + 4); { skip over FREF }
                                        count2 := aPtr^;
                                        aPtr := IntegerPtr(ord(aPtr) + 2); { skip over count }
                                        FOR k := 0 TO count2 DO
                                            BEGIN
                                            IF aPtr^ = localID THEN
                                                BEGIN
                                                GetDefaultID := TRUE;
                                                theID := IntegerPtr(ord(aPtr) + 2)^;
                                                Exit(GetDefaultID);
                                                END;
                                            aPtr := IntegerPtr(ord(aPtr) + 4); { skip to next one }
                                            END;
                                        Exit(GetDefaultID);
                                        END
                                    ELSE
                                        BEGIN
                                        aPtr := IntegerPtr(ord(aPtr) + 4); { skip over type name }
                                        aPtr := IntegerPtr(ord(aPtr) + 4 * aPtr^ + 6); { skip over
                                            this type }
                                        END;
                                    END; { look for FREF }
                                END; { is the right BNDL }
                            END; { search all BNDL loop }
                        END; { is FREF for APPL }
                    END; { search all FREF loop }
                END; { no err in GetProcessInformation }
        END;

    FUNCTION GetDefaultSICN: Handle;
    { get a default SICN from the application }

        VAR
            tempHandle: Handle;
            tryICN: Handle;
            srcBits, dstBits: BitMap;
            theID: integer;

        BEGIN
            tempHandle := NIL;
            IF GetDefaultID(theID) THEN
                BEGIN
                { we find out the default ID, use it to get the resource }
                tempHandle := GetResource('SICN', theID);
                IF tempHandle <> NIL THEN
                    BEGIN
                    { we have a SICN, use it }
                    flag := HGetState(tempHandle);
                    HNoPurge(tempHandle);
                    scaled := FALSE;
                    END
                ELSE
                    BEGIN
                    { we do not have a small icon, then try to scale it down from the icon }
                    tryICN := GetResource('ICN#', theID); { first we try to read it in }
                    IF tryICN <> NIL THEN { yes, we have one }
                        BEGIN
                        flag := HGetState(tryICN);
                        HLock(tryICN); { lock it down first }
                        tempHandle := NewHandle(32); { create a handle for a small icon }
                        IF tempHandle <> NIL THEN
                            BEGIN { copy it in }
                            HLock(tempHandle);
                            WITH srcBits DO
                                BEGIN
                                SetRect(bounds, 0, 0, 32, 32);
                                rowBytes := 4;
                                baseAddr := tryICN^
                                END;
                            WITH dstBits DO
                                BEGIN
                                SetRect(bounds, 0, 0, 16, 16);
                                rowBytes := 2;
                                baseAddr := tempHandle^
                                END;
                            { scale it down }
                            CopyBits(srcBits, dstBits, srcBits.bounds, dstBits.bounds, srcCopy,
                                     NIL);
                            HUnLock(tempHandle);
                            scaled := TRUE;
                            END;
                        HSetState(tryICN, flag);
                        END;
                    END;
                END;
            GetDefaultSICN := tempHandle;
        END;

    BEGIN
        err := noErr;
		CalculateTimeOut(timeOutInTicks);
        IgnoreOSErr(GetFrontProcess(frontPSN));
        IgnoreOSErr(GetCurrentProcess(myPSN));
        IgnoreOSErr(SameProcess(frontPSN, myPSN, nowInFront)); { are we already in front? }
        interactType := kCanInteract; { this is the default if it is not specified in the message }
        switchDirectly := FALSE;
        IgnoreOSErr(AE_GetTheCurrentEvent(currentDesc));
        EventSource(MsgHdrHdl(currentDesc.dataHandle), aevtSource, fromPSN); { where is it from? }
        IF aevtSource <> kAEUnknownSource THEN
            BEGIN { we have a message }
            IgnoreOSErr(AE_GetInteractionAllowed(curInteract));
            { find the interact level specified by the message }
            IgnoreOSErr(AE_GetKeyPtr(MakeMeta(currentDesc), keyInteractLevelAttr, typeEnumerated,
                                     dummyType, @interactType, 4, actualSize));
            IF LONGINT(interactType) = LONGINT(kNeverInteract) THEN { the message specified no
                                                                     interact }
                GOTO 999;
            WITH MsgHdrHdl(currentDesc.dataHandle)^^ DO
                BEGIN
                IF (aevtSource = kAELocalProcess) OR (aevtSource = kAERemoteProcess) THEN
                    BEGIN
                    { now we know it is not send-to-self }
                    IF (curInteract = kAEInteractWithSelf) THEN GOTO 999; { the server don't want to
                                                                           interact with others }
                    IF aevtSource = kAELocalProcess THEN
                        BEGIN
                        { if msg came from front process, we can switch directly }
                        IF (interactType[4] = 's') THEN { and they give us permission }
                            IgnoreOSErr(SameProcess(frontPSN, fromPSN, switchDirectly));
                        END
                    ELSE
                        BEGIN
                        { this comes from another machine }
                        IF curInteract <> kAEInteractWithAll THEN GOTO 999;
                        END;
                    END;
                END
            END;
        IF NOT nowInFront THEN { only need to do something if nor already in front }
            BEGIN { not the front process, try to get to the front }
            IF timeOutInTicks = 0 THEN
                err := errAETimeOut
            ELSE
                BEGIN
                IF switchDirectly THEN
                    BEGIN
                    waitSetFrontProcess := 15; { we can wait this long for it to switch }
                    { but we don't have that much time we have to make do with less }
                    IF timeOutInTicks < waitSetFrontProcess THEN
                        waitSetFrontProcess := timeOutInTicks;
					{ this is the amount left over }
                    timeOutInTicks := timeOutInTicks - waitSetFrontProcess;
                    AskToComeToFront(MsgHdrHdl(currentDesc.dataHandle), TickCount + waitSetFrontProcess);
                    err := GeneralWait(waitSetFrontProcess, idleProc, NIL, NIL);
                    IF err = errAETimeOut THEN
                        switchDirectly := FALSE { we fail to switch before time is up }
					ELSE { we did not use it all up, so add it back }
                        timeOutInTicks := timeOutInTicks + waitSetFrontProcess;
                    END;

                IF switchDirectly THEN { remember it so we can go back }
                    MsgHdrHdl(currentDesc.dataHandle)^^.switchFromPSN := fromPSN
                ELSE
                    BEGIN
                    { otherwise we just use notification }
                    IF nmReqPtr = NIL THEN
                        BEGIN { if no notification record is provided, we make our own }
                        WITH myNMRec DO
                            BEGIN
                            qType := nmType;
                            nmMark := 1;
                            nmIcon := GetDefaultSICN;
							IF nmIcon = NIL THEN
								nmSound := Handle( - 1)
							ELSE
                            	nmSound := NIL;
                            nmStr := NIL;
                            nmResp := NIL;
                            END;
                        nmReqPtr := @myNMRec;
                        END;
                    IgnoreOSErr(NMInstall(nmReqPtr));
    { now we call the general wait routine, last two parameter are nil means wait to come to front }
                    err := GeneralWait(timeOutInTicks, idleProc, NIL, NIL);
                    IgnoreOSErr(NMRemove(nmReqPtr));
                    IF ord(nmReqPtr) = ord(@myNMRec) THEN
                        WITH myNMRec DO { it is our own, and it was installed, we have to take care
                                         of it }
                            IF nmIcon <> NIL THEN
                                BEGIN { we have a small icon }
                                IF scaled THEN
                                    DisposHandle(nmIcon) { we created it, so we free it }
                                ELSE
                                    HSetState(nmIcon, flag); { it is from resource, just restore its
                                                              state }
                                END;
                    END;
                END;
            END;

        GOTO 1000;
    999:
        err := errAENoUserInteraction;
    1000:
        AE_InteractWithUser := err;
    END;

{--------------------------------------------------------------------------------}

{ Used by the server if the command received is going to take some time to process. }
{ Use the reply to get the necessary information to send a 'wait' back to the client }
{ This will cause the client to reset its timeout loop and wait longer }

FUNCTION AE_ResetTimer(VAR reply: AppleEvent): OSErr;

    VAR
        theEvent: EventRecord;

    BEGIN
        AE_ResetTimer := errAEReplyNotValid;
        IF reply.dataHandle <> NIL THEN
            BEGIN
            WITH MsgHdrHdl(reply.dataHandle)^^ DO
                BEGIN
                IF LONGINT(thisAEEventClass) = LONGINT(kCoreEventClass) THEN
                    IF LONGINT(thisAEEventID) = LONGINT(kAEAnswer) THEN
                        IF LONGINT(msgAddrType) = LONGINT(typeSessionID) THEN
                            BEGIN
                            theEvent.message := LONGINT(kCoreEventClass);
                            LONGINT(theEvent.where) := LONGINT(kAEWaitLonger);
                            theEvent.modifiers := 0;
                            AE_ResetTimer := PostHighLevelEvent(theEvent,
                                                                Ptr(msgAddrTarget.asTargetID.sessionID),
                                                                returnID, NIL, 0,
                                                                receiverIDisSessionID);
                            END;
                END;
            END;
    END;

{--------------------------------------------------------------------------------}
