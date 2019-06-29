{
    File:       AEDFWrapper.inc1.p

    Written by: Ed Lai

    Copyright:  © 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

        Codes used in:  BigBang,BigBang,BigBang,BigBang
    Change History (most recent first):

		<16>	  2/5/91	Lai		BM,#81596,81597: Also for bug jfr101 on whiteboard. For 81597,
									hintIndex is now invalidated for regular and meta parameter
									writes. For 81586, roundup is used in SetMsgAddr. In SetMsgAddr
									and PutAttribute, GetDescInfo is used to obtain the correct
									offset and size for list/reco.
		<15>	 1/18/91	Lai		When adding a list/record to an attribute, skip the header.
		<14>	 1/11/91	Lai		Fix bug of adding a null desc to AppleEvent and GetArray with
									array type kAEDataArray
		<13>	12/13/90	Lai		Separation of the main files into separate modules
		<12>	10/29/90	Lai		Get around change in GetHandleSize glue
		<11>	10/10/90	Lai		MakeReturnID should take a parameter of LONGINT
		<10>	 9/21/90	Lai		Fix bug that access mask and data available is only check when
									doing GetPtr but not GetDesc.
		 <9>	 9/20/90	Lai		Do not allow adding a descriptor with key into an ordinary list
		 <8>	 9/20/90	Lai		When doing a Put N, return error if (N - number of items in
									List) > 1
		 <7>	 9/19/90	Lai		Fix bug that in CreateMessage, extrabytes is not passed to
									CreateBuffer
		 <6>	 9/18/90	Lai		transaction ID is now back as a meta parameter. More robust 'tid
									' to 'sect' coercion.
		 <5>	 9/15/90	ngk		Fix for name changes in PPCToolbox.p
		 <4>	 9/15/90	Lai		Post code review clean-up, Coercion from tid to sect
        <3+>     9/13/90    Lai     Post code review clean-up
         <3>     9/10/90    Lai     Type case OSType to LONGINT for better compilation with 3.0
                                    compiler.
         <2>      9/7/90    Lai     Fix comment
         <1>      9/7/90    Lai     first checked in
        <0>   9/7/90    Lai     First Checked in to BBS
        ---------------------
         3-13 Mike Farr. I added wantAEDF to GlobalRec.
         3-16 Mike Farr.  Added clientsAddr to GlobalRec. Initialized in AEInit.
         3-16 Eric House. Commented some code. Fixed bug in storing factored lists.
         3-16 EKL CreateList, CreateMessage, CreateBuffer now return an OSErr, and
                  a var adddescparameter is used for the result
         3-16 EKL AEPutPtr/AEPutDesc now returns an error when adding parameter to record/message
         3-16 EKL Can now use AEPutPtr/AEPutDesc to record/message for replacing parameter
         3-16 EKL AEInit has an additional procPtr for handling non-aevt high level event
         3-19 EKL Now keep track of number of parameters in message
         3-19 EKL Add AEListItemCount to return number of items in a list or a message
         3-19 EKL Add AEQuit to terminate use of AEM
         3-20 EKL Use Nick's Header file names
         3-20 EKL Add counter for generating refcon
         3-23 EKL In AEPutXXXX a nil ptr/handle is treated as 0 byte data rather than delete.
         3-23 EKL Add AEDeteleXXXX to do the actual delete.
         3-23 EKL CreateList is renamed to AECreateList and only create an empty list
         3-23 EKL AEPutArray is used to write an array of data to a list
                  Currently if there is an error in middle of AEPutArray, it is possible to
                  have a partial addition, this will be improved in future.
         3-23 EKL AEGetArray is for reading an array from a list.
         3-26 EKL AERef is now a parameter in AEDisposeDesc
         3-27 EKL AEInit now set AETable to nil
         3-27 EKL Remove aMetaProto from Glboal, minimize the need to use global.
         3-30 EKL Check if message end exactly at end of handle, otherwise it is error.
         3-30 EKL Reclassify the error codes.
         3-30 EKL Make AEDescCoercion public in stead of StdCoercion.
         3-30 EKL In coercion, source = destination means change in place.
         4-06 EJL In standard coercion, bug of not returning proper error fixed

    To Do:
}

{[a-,body+,h-,o=100,r+,rec+,t=4,u+,#+,j=20/57/1$,n-]}   { Pasmat format control line }
{ AEDFWrapper.inc1.p }
{ Copyright © 1984-1990 by Apple Computer Inc.  All rights reserved. }

{--------------------------------------------------------------------------------}

TYPE

    { this is the header with a transaction id as the meta parameter }
    AEMetaTransaction   = RECORD
        aevtMarker:         AEKeyWord;
        version:            LONGINT;
        theKeyword:         AEKeyWord;
        theType:            DescType;
        theLength:          LONGINT;
        theValue:           LONGINT;
        metaTerminator:     AEKeyWord;
        END;
    AEMetaTransPointer  = ^AEMetaTransaction;

    { this record has the header info for an item in an AEList }
    HeaderInfoRec       = RECORD
        aHeader:            CommonHeader;
        headSize:           LONGINT;
        prefixSize:         LONGINT;
        factorOut:          LONGINT;
        hasKey:             BOOLEAN;
        END;

    { this record is for extracting location of data in a AEDesc }
	DescInfoRec       = RECORD
		dataType:			DescType;
		dataPtr:			Ptr;
		dataSize:			LONGINT;
		flag:				SignedByte;
		END;
		
    {--------------------------------------------------------------------------------}

FUNCTION IntMultiply(x, y: INTEGER): LONGINT;           { result := x * y }
    INLINE $301F,                                       { move.w (a7)+,d0; pop y }
           $C1DF,                                       { muls.w (a7)+,d0; *x }
           $2E80;                                       { move.l do,(a7) ; put back result }

FUNCTION DivIFFShort(aShort: INTEGER;
                     aLong: LONGINT): INTEGER;          { result := aLong DIV aShort, 0 if overflow
                                                         }
    INLINE $201F,                                       { move.l (a7)+,d0; aLong }
           $81DF,                                       { divs.w (a7)+,d0; aLong DIV aShort }
           $6802,                                       { bvc OK; no oveflow }
           $4240,                                       { clr.w, d0; 0 if overflow }
           $3E80;                                       { move.w d0, (a7); put back result }

{--------------------------------------------------------------------------------}

FUNCTION AE_DeleteItem(VAR theAEDescList: AEDescList;   { VAR is for efficiency only }
                       index: LONGINT): OSErr;
    EXTERNAL;

FUNCTION AE_DeleteKeyDesc(VAR theAERecord: AERecord;    { VAR is for efficiency only }
                          theAEKeyWord: AEKeyWord): OSErr;
    EXTERNAL;

FUNCTION AE_GetNthDesc(VAR theAEDescList: AEDescList;   { VAR is for efficiency only }
                       index: LONGINT;
                       desiredType: DescType;
                       VAR theAEKeyWord: AEKeyWord;
                       VAR theAEDesc: AEDesc): OSErr;
    EXTERNAL;

FUNCTION AE_GetNthPtr(VAR theAEDescList: AEDescList;
                      index: LONGINT;
                      desiredType: DescType;
                      VAR theAEKeyWord: AEKeyWord;
                      VAR typeCode: DescType;
                      dataPtr: Ptr;
                      maximumSize: LONGINT;
                      VAR actualSize: LONGINT): OSErr;
    EXTERNAL;

FUNCTION AE_PutKeyPtr(VAR theAERecord: AERecord;        { VAR is for efficiency only }
                      theAEKeyWord: AEKeyWord;
                      typeCode: DescType;
                      dataPtr: Ptr;
                      dataSize: LONGINT): OSErr;
    EXTERNAL;

FUNCTION AE_PutDesc(VAR theAEDescList: AEDescList;      { VAR is for efficiency only }
                    index: LONGINT;
                    VAR theAEDesc: AEDesc               { VAR is for efficiency only }
                    ): OSErr;
    EXTERNAL;

FUNCTION AE_PutPtr(VAR theAEDescList: AEDescList;       { VAR is for efficiency only }
                   index: LONGINT;
                   typeCode: DescType;
                   dataPtr: Ptr;
                   dataSize: LONGINT): OSErr;
    EXTERNAL;

FUNCTION AE_PutKeyDesc(VAR theAERecord: AERecord;       { VAR is for efficiency only }
                       theAEKeyWord: AEKeyWord;
                       VAR theAEDesc: AEDesc            { VAR is for efficiency only }
                       ): OSErr;
    EXTERNAL;

FUNCTION AE_GetAttributePtr(VAR theAppleEvent: AppleEvent; { VAR is for efficiency only }
                            theAEKeyWord: AEKeyWord;
                            desiredType: DescType;
                            VAR typeCode: DescType;
                            dataPtr: Ptr;
                            maximumSize: Size;
                            VAR actualSize: Size): OSErr;
    EXTERNAL;

FUNCTION AE_GetAttributeDesc(VAR theAppleEvent: AppleEvent; { VAR is for efficiency only }
                             theAEKeyWord: AEKeyWord;
                             desiredType: DescType;
                             VAR result: AEDesc): OSErr;
    EXTERNAL;

FUNCTION AE_SizeOfAttribute(VAR theAppleEvent: AppleEvent; { VAR is for efficiency only }
                            theAEKeyWord: AEKeyWord;
                            VAR typeCode: DescType;
                            VAR dataSize: Size): OSErr;
    EXTERNAL;

{--------------------------------------------------------------------------------}

FUNCTION CheckDesc(VAR theAEDescList: AEDescList;
                   VAR theDescClass: DescClass;
                   waitReady: BOOLEAN): OSErr;
    FORWARD;

FUNCTION ExtractDescriptor(msg: MsgHdrHdl;
                           dataType: DescType;
                           dataSize, dataOffset, factorOut: LONGINT;
                           VAR res: AEDesc): OSErr;
    FORWARD;

PROCEDURE FindDescOffset(msgClass: DescClass;
                         msg: MsgHdrHdl;
                         VAR index: LONGINT;
                         VAR key: AEKeyWord;
                         VAR offset: LONGINT;
                         VAR aHeaderInfoRec: HeaderInfoRec);
    FORWARD;

PROCEDURE GetAttributeOffset(key: AEKeyWord;
                             VAR offset: LONGINT;
                             VAR dataType: DescType);
    FORWARD;

FUNCTION GetDescInfo(VAR theAEDesc: AEDesc;      { VAR is for efficiency only }
                     VAR descInfo: DescInfoRec): OSErr;
    FORWARD;

FUNCTION MakeReturnID(ReturnID: LONGINT): LONGINT;
    FORWARD;

PROCEDURE SetAccessMask(msg: MsgHdrHdl;
                        index: LONGINT);
    FORWARD;

FUNCTION SetMsgAddress(theMsgHdrHdl: MsgHdrHdl;
                       typeCode: DescType;
                       dataPtr: Ptr;
                       dataSize: LONGINT): OSErr;
    FORWARD;

{--------------------------------------------------------------------------------}

{ This is the basic routine to add a desc to a list, it essentially calls write data
    but takes special caution in the case where theArg is a list/record, an
    AppleEvent or is nil }

FUNCTION AddDesc(VAR theAEDescList: AEDescList;         { VAR is for efficiency only }
                 key: AEKeyWord;
                 VAR theArg: AEDesc;                    { VAR is for efficiency only }
                 index: LONGINT): OSErr;

    VAR
        err:                OSErr;
		descInfo:			DescInfoRec;

    BEGIN
	err := GetDescInfo(theArg, descInfo); { get the location }
	IF err = noErr THEN
		WITH descInfo DO
			BEGIN
			err := WriteData(theAEDescList, key, dataType, dataPtr, dataSize, index, 1);
			HSetState(theArg.dataHandle, flag);
			END;
    AddDesc := err;
    END;

{--------------------------------------------------------------------------------}

{ count number of items, just get it from the header }

FUNCTION AE_CountItems(VAR theAEDescList: AEDescList;   { VAR is for efficiency only }
                       VAR theCount: LONGINT): OSErr;

    VAR
        msgClass:           DescClass;
        err:                OSErr;

    BEGIN
    theCount := 0;
    err := CheckDesc(theAEDescList, msgClass, TRUE);
    IF err = NoErr THEN
        WITH MsgHdrHdl(theAEDescList.dataHandle)^^ DO
            BEGIN
            IF msgClass = classMeta THEN
                theCount := listClassHeader.metaCountOrObjType
            ELSE                                        { note that this works because paramCount is
                                                         at the same position }
                theCount := paramCount;
            END;
    AE_CountItems := err;
    END;

{--------------------------------------------------------------------------------}

{ create an AppleEvent message, createBare message set up the buffer, then we
    set the address and finish initialization by calling CheckMessage }

FUNCTION AE_CreateAppleEvent(theAEEventClass: AEEventClass;
                             theAEEventID: AEEventID;
                             VAR target: AEAddressDesc; { VAR is for efficiency only }
                             ReturnID: INTEGER;
                             transactionID: LONGINT;
                             VAR result: AppleEvent): OSErr;

    VAR
        err:                OSErr;
		descInfo:			DescInfoRec;

    BEGIN
	err := GetDescInfo(target, descInfo);{ get the location }
	IF err = noErr THEN
		WITH descInfo DO
			BEGIN
			err := CreateBareMessage(dataSize, theAEEventClass, theAEEventID, ReturnID, transactionID,
									 result);
			IF err = NoErr THEN
				BEGIN
				err := SetMsgAddress(MsgHdrHdl(result.dataHandle), dataType, dataPtr, dataSize);
				IF err <> NoErr THEN
					DisposHandle(result.dataHandle)
				ELSE
					BEGIN
					{ use CheckMessage to finish the initialization }
					IgnoreOSErr(CheckMessage(MsgHdrHdl(result.dataHandle))); { would not return error }
					END;
				END;
			HSetState(target.dataHandle, flag);
			END;
    IF err <> NoErr THEN
        NukeIt(result);
    AE_CreateAppleEvent := err;
    END;

{--------------------------------------------------------------------------------}

{ release a desc, we have to watch out for the special case where it is an AppleEvent
    in which case we make sure it is not also be used by the client/server (then
    the status will be that it is used by one party instead of both), we should
    also free up the access mask handle if there is one }

FUNCTION AE_DisposeDesc(VAR theAEDesc: AEDesc): OSErr;

    VAR
        canDispose:         BOOLEAN;
        err:                OSErr;

    BEGIN
    err := NoErr;
    WITH theAEDesc DO
        BEGIN
        IF dataHandle <> NIL THEN
            BEGIN
			canDispose := TRUE;
			IF LONGINT(descriptorType) = LONGINT(kCoreEventClass) THEN
				WITH MsgHdrHdl(dataHandle)^^ DO
					BEGIN
					IF inUse THEN
						BEGIN
						{ we cannot dispose it because it is held by both parties }
						canDispose := FALSE;
					{ but dispose by one party means it will no longer be held by both parties }
						inUse := FALSE;
						END
					ELSE IF paramCount > 32 THEN
						DisposHandle(Handle(accessMask)); { accessMask is really a handle }
					IF inWaiting THEN               { we better take it off the waiting list }
						OffWaitList(MsgHdrHdl(dataHandle));
					END;
			IF canDispose THEN
				BEGIN
				DisposHandle(dataHandle);
				dataHandle := NIL;                  { we nil it for safety }
				END;
            IF dataHandle = NIL THEN
                descriptorType := typeNull;
            END;
        END;
    AE_DisposeDesc := err;
    END;

{--------------------------------------------------------------------------------}

{ read a whole list of descriptors }

FUNCTION AE_GetArray(VAR theAEDescList: AEDescList;     { VAR is for efficiency only }
                     arrayType: AEArrayType;
                     arrayPtr: AEArrayDataPointer;
                     maximumSize: LONGINT;
                     VAR itemType: DescType;
                     VAR itemSize: LONGINT;
                     VAR itemCount: LONGINT): OSErr;
{ not yet optimized }

    LABEL 999;

    VAR
        dataEnd:            LONGINT;
        aDesc:              AEDesc;
        aHeaderInfoRec:     HeaderInfoRec;
        done:               BOOLEAN;
        err:                OSErr;
        itemLength:         LONGINT;
        listSize:           LONGINT;
        dummySize:          LONGINT;
        dummyType:          DescType;
        dummyKey:           AEKeyWord;
        msgClass:           DescClass;

{   errAEWrongDataType means that you cannot get kAEDataArray, kAEHandleArray out of a list
    where items can be of variable size }

    BEGIN
    err := CheckDesc(theAEDescList, msgClass, FALSE);   { no need to check for reply ready here }
    IF err <> NoErr THEN
        GOTO 999;
    IgnoreOSErr(AE_CountItems(theAEDescList, listSize));
    itemCount := 0;
    done := FALSE;
    { find header info by calling FindDescOffset with an illegal Index }
    dummySize := - 1;
    FindDescOffset(msgClass, MsgHdrHdl(theAEDescList.dataHandle), dummySize, dummyKey,
                   dataEnd {dummy} , aHeaderInfoRec);
    WITH aHeaderInfoRec DO
        BEGIN
        IF prefixSize < 4 THEN
            itemType := typeWildCard                    { it is not a homogenous list }
        ELSE
            itemType := aHeader.theType;                { it is a homogenous list }
        IF prefixSize < 8 THEN
            itemSize := - 1                             { size is variable }
        ELSE
            itemSize := aHeader.theLength;              { size is fixed }
        IF (ord(arrayType) <= ord(kAEPackedArray)) THEN { a shorter way of saying kAEDataArray or
                                                         KAEPackeArray }
            BEGIN
            { read data array, if header is factored out we read in a single operation,
                otherwise we do it one by oen }
            WITH aHeader DO
                BEGIN
                IF (prefixSize < 8) THEN
                    BEGIN
                    err := errAEWrongDataType;
                    GOTO 999;
                    END;
                itemLength := theLength;
                IF itemLength <= 0 THEN
                    GOTO 999;                           { there is no data, so nothing is read }
                IF arrayType = kAEdataArray THEN
                    itemLength := BAnd(itemLength + RoundUpValue, roundUpMask);
                IF (factorOut = 0) THEN                 { if no factoring }
                    IF NOT hasKey THEN                  { if no key then data is contiguous }
                        IF ((itemSize = 1) = (arrayType = kAEPackedArray)) OR (NOT BTst(itemSize,
                           0)) THEN
                            IF (itemLength < MAXINT) THEN { if does not require long division }
                                BEGIN                   { see we can do it in one operation? }
                                itemCount := DivIFFShort(maximumSize, itemLength);
                                IF itemCount > 0 THEN   { we can do it in one operation }
                                    BEGIN
                                    IF itemCount > listSize THEN
                                        itemCount := listSize; { only so many is in there }
                                    WITH ListHdrHdl(theAEDescList.dataHandle)^^ DO
                                        BlockMove(Ptr(ord(theAEDescList.dataHandle^) +
                                                      listClassHeader.paramBegin), Ptr(arrayPtr),
                                                  IntMultiply(itemCount, itemLength));
                                    done := TRUE;
                                    END;
                                END;
                END;
            END
        ELSE
            BEGIN
            itemLength := BSL(ord(arrayType) - ord(kAEPackedArray), 2); { 4, 8, 12 for the three
                                                                         types }
            IF arrayType = kAEHandleArray THEN
                BEGIN
                IF (prefixSize < 4) THEN
                    BEGIN
                    err := errAEWrongDataType;
                    GOTO 999;
                    END;
                aDesc.descriptorType := aHeader.theType;
                END;
            END;
        END;

    IF NOT done THEN
        BEGIN
        dataEnd := ord(arrayPtr) + maximumSize - itemLength;
        WHILE (ord(arrayPtr) <= dataEnd) AND (itemCount < listSize) DO
            BEGIN
            IF (ord(arrayType) <= ord(kAEPackedArray)) THEN { a shorter way of saying kAEDataArray
                                                             or KAEPackeArray }
                err := ReadData(theAEDescList, itemCount + 1, typeWildCard, dummyKey, dummyType,
                                Ptr(arrayPtr), itemSize, dummySize)
            ELSE
                err := AE_GetNthDesc(theAEDescList, itemCount + 1, typeWildCard, dummyKey, aDesc);
            IF err <> NoErr THEN
                GOTO 999;
            itemCount := itemCount + 1;
            CASE arrayType OF
                kAEHandleArray:
                    BEGIN
                    LONGINTPtr(arrayPtr)^ := LONGINT(aDesc.dataHandle);
                    END;
                kAEDescArray:
                    BEGIN
                    AEDescPtr(arrayPtr)^ := aDesc;
                    END;
                kAEKeyDescArray:
                    WITH AEKeyDescPtr(arrayPtr)^ DO
                        BEGIN
                        descKey := dummyKey;
                        descContent := aDesc;
                        END;
            END;
            arrayPtr := AEArrayDataPointer(ord(arrayPtr) + itemLength);
            END;
        END;
999:
    AE_GetArray := err;
    END;

{--------------------------------------------------------------------------------}

{ it is more difficult to use the assembler trick here because theAEKeyWord Parameter
    in FetchDesc is a VAR }

FUNCTION AE_GetKeyDesc(VAR theAERecord: AERecord;       { VAR is for efficiency only }
                       theAEKeyWord: AEKeyWord;
                       desiredType: DescType;
                       VAR result: AEDesc): OSErr;

    BEGIN
    AE_GetKeyDesc := FetchDesc(theAERecord, 0, desiredType, theAEKeyWord, result);
    END;

{--------------------------------------------------------------------------------}

FUNCTION AE_GetKeyPtr(VAR theAERecord: AERecord;        { VAR is for efficiency only }
                      theAEKeyWord: AEKeyWord;
                      desiredType: DescType;
                      VAR typeCode: DescType;
                      dataPtr: Ptr;
                      maximumSize: Size;
                      VAR actualSize: Size): OSErr;

    BEGIN
    AE_GetKeyPtr := ReadData(theAERecord, 0, desiredType, theAEKeyWord, typeCode, dataPtr,
                             maximumSize, actualSize);
    END;

{--------------------------------------------------------------------------------}

FUNCTION AE_PutArray(VAR theAEDescList: AEDescList;     { VAR is for efficiency only }
                     arrayType: AEArrayType;
                     arrayPtr: AEArrayDataPointer;
                     itemType: DescType;
                     itemSize: Size;
                     itemCount: LONGINT): OSErr;
{ not yet optimized }

    VAR
        msgClass:           DescClass;
        i:                  LONGINT;
        dummy:              LONGINT;
        aDesc:              AEDesc;
        err:                OSErr;
        aHeaderInfoRec:     HeaderInfoRec;
        uniform:            BOOLEAN;
        done:               BOOLEAN;

    LABEL 999;

    BEGIN
    done := FALSE;
    err := CheckDesc(theAEDescList, msgClass, FALSE);
    IF err <> NoErr THEN
        GOTO 999;
    { find header info by calling FindDescOffset with an illegal Index }
    i := - 1;
    FindDescOffset(msgClass, MsgHdrHdl(theAEDescList.dataHandle), i, AEKeyWord(dummy), dummy,
                   aHeaderInfoRec);

    WITH aHeaderInfoRec DO
        BEGIN
        IF hasKey THEN
            IF (arrayType <> kAEKeyDescArray) THEN
                BEGIN
                err := errAEWrongDataType;
                GOTO 999;
                END;
        IF (ord(arrayType) <= ord(kAEPackedArray)) THEN { a shorter way of saying kAEDataArray or
                                                         KAEPackeArray }
            BEGIN
            { if it is uniform and factored, we may do it in one operation, otherewise
                we do it one by one }
            WITH aHeader DO
                BEGIN
                uniform := FALSE;
                IF prefixSize < 4 THEN
                    theType := itemType;
                IF prefixSize < 8 THEN
                    theLength := itemSize
                ELSE
                    uniform := TRUE;
                IF (LONGINT(theType) <> LONGINT(itemType)) OR (theLength <> itemSize) THEN
                    BEGIN
                    err := errAEWrongDataType;
                    GOTO 999;
                    END;
                IF arrayType = kAEdataArray THEN
                    theLength := BAnd(theLength + RoundUpValue, roundUpMask);
                IF uniform THEN                         { data is contiguous because type and size
                                                         is factored and there is no key }
                    IF (factorOut = 0) THEN             { if no factoring }
                        IF ((itemSize = 1) = (arrayType = kAEPackedArray)) OR (NOT BTst(itemSize,
                           0)) THEN                    { data array is some layout as internal }
                            BEGIN                       { we can do it in one operation }
                            err := PtrAndHand(Ptr(arrayPtr), theAEDescList.dataHandle, itemCount *
                                              theLength);
                            IF err <> NoErr THEN
                                GOTO 999;
                            WITH ListHdrHdl(theAEDescList.dataHandle)^^ DO
                                paramCount := paramCount + itemCount;
                            done := TRUE;
                            END;
                END;
            END;
        IF NOT done THEN
            BEGIN
            IF arrayType = kAEHandleArray THEN
                BEGIN
                IF prefixSize >= 4 THEN
                    IF (LONGINT(aHeader.theType) <> LONGINT(itemType)) THEN
                        BEGIN
                        err := errAEWrongDataType;
                        GOTO 999;
                        END;
                aDesc.descriptorType := itemType;
                END;
            FOR i := itemCount DOWNTO 1 DO
                BEGIN
                CASE arrayType OF
                    kAEHandleArray:
                        BEGIN
                        aDesc.dataHandle := HandlePtr(arrayPtr)^;
                        err := AE_PutDesc(theAEDescList, 0, aDesc);
                        arrayPtr := AEArrayDataPointer(ord(arrayPtr) + SizeOf(Handle));
                        END;                            { end kAEHandleArray }
                    kAEDescArray:
                        BEGIN
                        err := AE_PutDesc(theAEDescList, 0, AEDescPtr(arrayPtr)^);
                        arrayPtr := AEArrayDataPointer(ord(arrayPtr) + SizeOf(AEDesc));
                        END;                            { end kAEDescArray }
                    kAEKeyDescArray:
                        BEGIN
                        WITH AEKeyDescPtr(arrayPtr)^ DO
                            err := AE_PutKeyDesc(theAEDescList, descKey, descContent);
                        arrayPtr := AEArrayDataPointer(ord(arrayPtr) + SizeOf(AEKeyDesc));
                        END;                            { end kAEkeyDescArray }
                    OTHERWISE
                        BEGIN
                        err := AE_PutPtr(theAEDescList, 0, itemType, Ptr(arrayPtr), itemSize);
                        arrayPtr := AEArrayDataPointer(ord(arrayPtr) + aHeader.theLength);
                        END;
                END;                                    { case }
                IF err <> NoErr THEN
                    GOTO 999;
                END;
            END;
        END;
999:
    AE_PutArray := err;
    END;

{--------------------------------------------------------------------------------}

{ to set an attribute }
{ just lock it and call PutPtrAttribute }

FUNCTION AE_PutAttributeDesc(VAR theAppleEvent: AEDescList; { VAR is for efficiency only }
                             theAEKeyWord: AEKeyWord;
                             VAR theAEDesc: AEDesc      { VAR is for efficiency only }
                             ): OSErr;

    VAR
		err:		OSErr;
		descInfo:	DescInfoRec;

    BEGIN
	err := GetDescInfo(theAEDesc, descInfo);{ get the location }
	IF err = noErr THEN
		WITH descInfo DO
			BEGIN
			err := AE_PutAttributePtr(theAppleEvent, theAEKeyWord, dataType, dataPtr, dataSize);
			HSetState(theAEDesc.dataHandle, flag);
			END;
	AE_PutAttributeDesc := err;
    END;

{--------------------------------------------------------------------------------}

{ to add an attribute pointed to by a pointer }
{ if it is a pseudo meta parameter, we determine its offset and size and put it
    there, otherwise do it like a meta parameter }

FUNCTION AE_PutAttributePtr(VAR theAppleEvent: AEDescList; { VAR is for efficiency only }
                            theAEKeyWord: AEKeyWord;
                            typeCode: DescType;
                            dataPtr: Ptr;
                            dataSize: LONGINT): OSErr;

    VAR
        err:                OSErr;
        offset:             LONGINT;
        result:             AEDesc;
        expectSize:         LONGINT;
        expectType:         DescType;
        disp:               LONGINT;

    BEGIN
    err := errAENotAEDesc;
    IF (LONGINT(theAppleEvent.descriptorType) = LONGINT(kCoreEventClass)) AND
       (theAppleEvent.dataHandle <> NIL) THEN
        BEGIN
        err := NoErr;
        result.dataHandle := NIL;
        { for those at fixed position and fix size, we determine the position first }
        expectSize := 4;                                { most common used value, use as default }
        GetAttributeOffset(theAEKeyWord, offset, expectType);
        IF offset > 0 THEN
            BEGIN
            { these are the fixed postion fixed size cases }
            { for these, we must have it in the correct type }
            IF expectType <> typeCode THEN
                err := AE_CoercePtr(typeCode, dataPtr, dataSize, expectType, result);
            IF err = NoErr THEN
                BEGIN
                IF result.dataHandle <> NIL THEN
                    BEGIN
                    dataPtr := result.dataHandle^;
                    dataSize := GetHandleSize(result.dataHandle);
                    END;
                { the size must be right or we cannot do it }
                IF expectSize = dataSize THEN
                    BEGIN
                    { we just move it in }
                    BlockMove(dataPtr, Ptr(ord(theAppleEvent.dataHandle^) + offset), expectSize);
                    { for return ID, we need to consider case of autogeneartion of ID }
                    IF theAEKeyWord = keyReturnIDAttr THEN
                        WITH MsgHdrHdl(theAppleEvent.dataHandle)^^ DO
                            IF ReturnID = kAutoGenerateReturnID THEN
                                ReturnID := MakeReturnID(kAutoGenerateReturnID);
                    END
                ELSE
                    err := errAECoercionFail;
                DisposHandle(result.dataHandle);
                END;
            END
        ELSE IF LONGINT(theAEKeyWord) = LONGINT(keyAddressAttr) THEN
        { special case for address because it is variable size }
            err := SetMsgAddress(MsgHdrHdl(theAppleEvent.dataHandle), typeCode, dataPtr, dataSize)
        ELSE
            BEGIN                                       { otherwise it is just a meta parameter }
            err := WriteData(MakeMeta(theAppleEvent), theAEKeyWord, typeCode, dataPtr, dataSize, 0,
                             1);
            END;
        END;
    AE_PutAttributePtr := err;
    END;

{--------------------------------------------------------------------------------}

{ get size of descriptor , just pass NIL to Ptr in ReadData }

FUNCTION AE_SizeOfKeyDesc(VAR theAERecord: AERecord;    { VAR is for efficiency only }
                          theAEKeyWord: AEKeyWord;
                          VAR typeCode: DescType;
                          VAR dataSize: LONGINT): OSErr;

    BEGIN
    AE_SizeOfKeyDesc := ReadData(theAERecord, 0, typeWildCard, theAEKeyWord, typeCode, NIL, 0,
                                 dataSize);
    END;

{--------------------------------------------------------------------------------}

{ get size of descriptor , just pass NIL to Ptr in ReadData }

FUNCTION AE_SizeOfNthItem(VAR theAEDescList: AEDescList; { VAR is for efficiency only }
                          index: LONGINT;
                          VAR typeCode: DescType;
                          VAR dataSize: LONGINT): OSErr;

    VAR
        dummyKey:           AEKeyWord;

    BEGIN
    AE_SizeOfNthItem := ReadData(theAEDescList, index, typeWildCard, dummyKey, typeCode, NIL, 0,
                                 dataSize);
    END;

{--------------------------------------------------------------------------------}

{ check the message to see if it is an AppleEvent, and initialized its header }

FUNCTION CheckMessage(msg: MsgHdrHdl): OSErr;

    LABEL 998, 999;

    VAR
        err:                OSErr;
        aMetaPtr:           AEMetaPointer;
        aCommonPtr:         CommonHeaderPointer;
        dataEnd:            LONGINT;
        count:              LONGINT;

    BEGIN
    WITH msg^^ DO
        BEGIN
        listClassHeader.hintIndex := 0;
        aMetaPtr := AEMetaPointer(ord(Handle(msg)^) + BAnd(SizeOf(MsgHdr) - SizeOf(TargetID) +
                                  msg^^.msgAddrSize + RoundUpValue, RoundUpMask));
        END;
    WITH aMetaPtr^ DO
        BEGIN
        IF (LONGINT(aevtMarker) <> LONGINT(kCoreEventClass)) THEN
            BEGIN
            err := errAENotAppleEvent;
            GOTO 999;
            END
        ELSE IF (version <> kVersionOne) THEN
            BEGIN
            err := errAENewerVersion;
            GOTO 999;
            END;
        END;
    dataEnd := ord(Handle(msg)^) + GetHandleSize(Handle(msg));
    aCommonPtr := CommonHeaderPointer(ord(aMetaPtr) + 8);
    { count the number of meta parameters }
    count := 0;
    WHILE (ord(aCommonPtr) < dataEnd) AND (LONGINT(aCommonPtr^.theKeyword) <>
          LONGINT(aeEndOfMetaDataKeyword)) DO
        BEGIN
        count := count + 1;
        IF aCommonPtr^.theLength < 0 THEN
            GOTO 998;
        { aCommonPtr := CommonHeaderPointer(ord(aCommonPtr) + RoundUp(aCommonPtr^.theLength +
                                          SizeOf(CommonHeader))); }
        aCommonPtr := CommonHeaderPointer(ord(aCommonPtr) + BAnd(aCommonPtr^.theLength +
                                                                 SizeOf(CommonHeader) +
                                                                 RoundUpValue, roundUpMask));
        END;
    IF (ord(aCommonPtr) < dataEnd) AND (LONGINT(aCommonPtr^.theKeyword) =
       LONGINT(aeEndOfMetaDataKeyword)) THEN
        WITH msg^^ DO
            BEGIN
            WITH listClassHeader DO
                BEGIN
                paramBegin := ord(aCommonPtr) - ord(Handle(msg)^) + 4;
                metaCountOrObjType := count;
                END;
            aCommonPtr := CommonHeaderPointer(ord(aCommonPtr) + 4); { skip meta terminator }
            { count the number of parameters }
            count := 0;
            WHILE (ord(aCommonPtr) < dataEnd) DO
                BEGIN
                count := count + 1;
                IF aCommonPtr^.theLength < 0 THEN
                    GOTO 998;
                aCommonPtr := CommonHeaderPointer(ord(aCommonPtr) + BAnd(aCommonPtr^.theLength +
                                                                         SizeOf(CommonHeader) +
                                                                         RoundUpValue,
                              roundUpMask));
                END;
            { the message must end when the parameters ends }
            IF (ord(aCommonPtr) <> dataEnd) THEN
                GOTO 998;                               { descriptor ending must match data end
                                                         exactly }
            paramCount := count;
            inWaiting := FALSE;
            CheckMessage := NoErr;
            END
    ELSE
        GOTO 998;
    err := NoErr;
    GOTO 999;
998:
    err := errAECorruptData;
999:
    CheckMessage := err;
    END;

{--------------------------------------------------------------------------------}

{ create an empty message with no parameter, but header is not yet initialized }

FUNCTION CreateBareMessage(msgAddrLen: LONGINT;
                           theAEEventClass: AEEventClass;
                           theAEEventID: AEEventID;
                           theReturnID: LONGINT;
                           theTransactionID: LONGINT;
                           VAR result: AppleEvent): OSErr;

    VAR
        err:                OSErr;
        anAEMetaPointer:    AEMetaPointer;
        extraBytes:         LONGINT;

    BEGIN
    { transaction ID is a meta parameter, but we don't put it up if it is zero }
    IF theTransactionID = kAnyTransactionID THEN
        extraBytes := 0
    ELSE
        extraBytes := SizeOf(AEMetaTransaction) - SizeOf(AEMetaDesc);
    err := CreateBuffer(extraBytes, msgAddrLen, theAEEventClass, theAEEventID, result);
    IF err = NoErr THEN
        BEGIN
        WITH MsgHdrHdl(result.dataHandle)^^ DO
            ReturnID := MakeReturnID(theReturnID);
        anAEMetaPointer := AEMetaPointer(ord(result.dataHandle^) + BAnd(SizeOf(MsgHdr) -
                                         SizeOf(TargetID) + msgAddrLen + RoundUpValue, RoundUpMask));
        WITH anAEMetaPointer^ DO
            BEGIN
            aevtMarker := kCoreEventClass;
            version := kVersionOne;
            metaTerminator := aeEndOfMetaDataKeyword;
            END;
        { if we have nonzero transaction ID, we put it up as parameter }
        IF theTransactionID <> kAnyTransactionID THEN
            WITH AEMetaTransPointer(anAEMetaPointer)^ DO
                BEGIN
                theKeyword := keyTransactionIDAttr;
                theType := typeLongInteger;
                theLength := SizeOf(theValue);
                theValue := theTransactionID;
                metaTerminator := aeEndOfMetaDataKeyword;
                END;
        END;
    CreateBareMessage := err;
    END;

{--------------------------------------------------------------------------------}

{ create a message buffer }
{ this can be used to generate an empty message, or it can be used as the buffer
    for reading in a message }

FUNCTION CreateBuffer(extraBytes: LONGINT;
                      msgAddrLen: LONGINT;
                      theAEEventClass: AEEventClass;
                      theAEEventID: AEEventID;
                      VAR result: AppleEvent): OSErr;

    VAR
        aHandle:            Handle;
        anAEMetaPointer:    AEMetaPointer;

    BEGIN
    { extra bytes is the number of extra bytes needed in the handle }
    { no extra byte means AEAddress has no length }
    aHandle := NewHandle(SizeOf(AEMetaDesc) + SizeOf(MsgHdr) - SizeOf(TargetID) +
                         extraBytes + BAnd(msgAddrLen + RoundUpValue, RoundUpMask));
    WITH result DO
        BEGIN
        descriptorType := kCoreEventClass;
        dataHandle := aHandle;
        END;
    IF aHandle <> NIL THEN
        BEGIN
        WITH MsgHdrHdl(aHandle)^^ DO
            BEGIN
            msgAddrSize := msgAddrLen;
            listClassHeader.hintIndex := 0;
            inUse := FALSE;
            inWaiting := FALSE;
            waitLink := NIL;
            userRefcon := 0;
            thisAEEventClass := theAEEventClass;
            thisAEEventID := theAEEventID;
            WITH switchFromPSN DO
                BEGIN
                highLongOfPSN := 0;
                lowLongOfPSN := 0;
                END;
            prevMsg := NIL;
            notifyRecPtr := NIL;
            accessMask := 0;
            paramCount := 0;
            END;
        CreateBuffer := NoErr;
        END
    ELSE
        CreateBuffer := MemError;
    END;

{--------------------------------------------------------------------------------}

{ make the descriptor at dataOffset into an AEDesc }
{ if the descriptor is AEList or AERecord, it is a special case that needs special
    handling }

FUNCTION ExtractDescriptor(msg: MsgHdrHdl;
                           dataType: DescType;
                           dataSize, dataOffset, factorOut: LONGINT;
                           VAR res: AEDesc): OSErr;
{ note dataSize include the length that has been factored out }

    VAR
        flag:               SignedByte;
        err:                OSErr;
        aHandle:            Handle;
        aPtr:               Ptr;
        startPtr:           CommonHeaderPointer;

    BEGIN
    flag := HGetState(Handle(msg));
    HLock(Handle(msg));
    aPtr := Ptr(ord(Handle(msg)^) + dataOffset);
    IF (dataType = typeAERecord) OR (dataType = typeAEList) THEN
        BEGIN                                           { the case of AEList or AERecord }
        { first reserve the space }
        { err := PtrToHand(Ptr(ord(Handle(msg)^) + dataOffset - SizeOf(ListClassRec) -
                             factorOut), aHandle, RoundUp(dataSize + SizeOf(ListClassRec))); }
        err := PtrToHand(Ptr(ord(Handle(msg)^) + dataOffset - SizeOf(ListClassRec) - factorOut),
                         aHandle, BAnd(dataSize + SizeOf(ListClassRec) + RoundUpValue,
               roundUpMask));
        IF err = NoErr THEN
            BEGIN
            { we must do this first because sizeOfPrefix used below may be factored out }
            { it is possible that some of the data came from the factored header }
            IF factorOut > 0 THEN                       { this must be list or reco }
            { + 8 belows because we want to skip the common type and size }
                BlockMove(Ptr(ord(Handle(msg)^) + SizeOf(ListHdr) + 8), Ptr(ord(aHandle^) +
                          SizeOf(ListClassRec)), factorOut);
            { put in the list header }
            WITH ListHdrHdl(aHandle)^^ DO
                BEGIN
                WITH listClassHeader DO
                    BEGIN
                    hintIndex := 0;
                    { paramBegin :=  RoundUp(sizeOfPrefix + SizeOf(ListHdr)); }
                    paramBegin := BAnd(sizeOfPrefix + SizeOf(ListHdr) + RoundUpValue, roundUpMask);
                    metaCountOrObjType := LONGINT(dataType);
                    END;
                END;
            END;
        END
    ELSE
        BEGIN                                           { ordinary descriptor, just make it into a
                                                         handle }
        err := PtrToHand(Ptr(ord(Handle(msg)^) + dataOffset - factorOut), aHandle, dataSize);
        { it is possible that some of the data came from the factored header }
        IF (err = NoErr) AND (factorOut > 0) THEN
            BlockMove(Ptr(ord(Handle(msg)^) + SizeOf(ListHdr) + 8), aHandle^, factorOut);
        END;
    HSetState(Handle(msg), flag);
    WITH res DO
        BEGIN
        descriptorType := dataType;
        dataHandle := aHandle;
        END;
    ExtractDescriptor := err;
    END;

{--------------------------------------------------------------------------------}

{ this is the basic routine to fetch a descriptor from an AppleEvent/AEList }
{ it find the descriptor, then call ExtractDescriptor and if the extracted
    descriptor is of the wrong type, coercion is used }

FUNCTION FetchDesc(VAR theAEDescList: AEDescList;       { VAR is for efficiency only }
                   index: LONGINT;
                   wantType: DescType;
                   VAR key: AEKeyWord;
                   VAR res: AEDesc): OSErr;

    VAR
        msgClass:           DescClass;
        msg:                MsgHdrHdl;
        startPtr:           CommonHeaderPointer;
        offset:             LONGINT;
        err:                OSErr;
        aHeaderInfoRec:     HeaderInfoRec;
        converted:          AEDesc;

    BEGIN
    err := CheckDesc(theAEDescList, msgClass, TRUE);
    msg := MsgHdrHdl(theAEDescList.dataHandle);
    IF err = NoErr THEN
        BEGIN
        err := errAEDescNotFound;
        FindDescOffset(msgClass, msg, index, key, offset, aHeaderInfoRec);
        IF offset > 0 THEN
            BEGIN
            IF msgClass = classAevt THEN
                SetAccessMask(msg, index);
            { since we now have AE_CoercePtr in addition to AE_CoerceDesc, we may take
                advantage of it in future for better performance, but be careful about
                'list' and factored list }
            WITH aHeaderInfoRec DO
                BEGIN
                err := ExtractDescriptor(msg, aHeader.theType, aHeader.theLength, offset + headSize,
                                         factorOut, res);
                IF (LONGINT(wantType) <> LONGINT(aHeader.theType)) AND (LONGINT(wantType) <>
                   LONGINT(typeWildCard)) THEN
                    BEGIN                               { not the right type, but coercion may get
                                                         it }
                    { We have the convention that any data type can have the same data structrue
                        as an AERecord, so coercion to/from AERecord is a simple assignment.
                        So the corecion to/from AERecord always succeed, AEM just assumes you
                        know what you are doing and you only coerce somthint to/from AERecord
                        if they have the same data structure as AERecord. But this means that
                        if the descriptor invovled here is an AERecord, you are asking for a
                        'PICT', then you get an AERecord which is declared as 'PICT' and hence
                        instant disaster. That is why we disallowed FetchDesc to fetch an
                        AERecord as anything other than an AERecord }
                    IF LONGINT(aHeader.theType) = LONGINT(typeAERecord) THEN
                        err := errAECoercionFail
                    ELSE                                { anything else we can try to coerce it }
                        err := AE_CoerceDesc(res, wantType, converted);
                    { if there was an error, we can throw away the original }
{ if it succeed, we can throw away the original provided it is not the same as the converted result }
                    IF (err <> NoErr) OR (ord(res.dataHandle) <> ord(converted.dataHandle)) THEN
                        IgnoreOSErr(AE_DisposeDesc(res));
                    res := converted;
                    END;
                END;
            END;
        END;
    IF err <> NoErr THEN
        NukeIt(res);
    FetchDesc := err;
    END;

{--------------------------------------------------------------------------------}

{ Given a key or index, find the beginning offset of that descriptor }
{ if index <> 0, it is search by index, return the KeyWord in Key }
{ if index = 0, then it is search by KeyWord, return actual index in index }

{ aHeaderInfoRec returns all sort of information about the message and the item }
{          prefixSize  factorOut          headSize   hasKey
  list          0           0               8           F
  record        0           0               12          T
  list          4           0               4           F
  record        4           0               8           T
  list          8+      prefixSize-8        0           F
  record        8+      prefixSize-8        4           T
  AppleEvent    0           0               12          T }

PROCEDURE FindDescOffset(msgClass: DescClass;
                         msg: MsgHdrHdl;
                         VAR index: LONGINT;
                         VAR key: AEKeyWord;
                         VAR offset: LONGINT;
                         VAR aHeaderInfoRec: HeaderInfoRec);

    LABEL 999;

    VAR
        startPtr:           CommonHeaderPointer;
        dataEnd:            LONGINT;

        varSize:            BOOLEAN;
        itemSize:           LONGINT;
        useKey:             BOOLEAN;
        lookingAt:          LONGINT;
        aLongPtr:           LONGINTPtr;

    BEGIN
    WITH aHeaderInfoRec DO
        BEGIN
        offset := 0;
        useKey := (index = 0);
        IF msgClass >= classList THEN
            BEGIN
            hasKey := (msgClass = classReco);
            prefixSize := ListHdrHdl(msg)^^.sizeOfPrefix;
            { copy the header in case it is factored to the prefix }
            aHeader := CommonHeaderPointer(ord(msg^) + SizeOf(ListHdr) - 4)^;
            varSize := (prefixSize <= 4);
            IF varSize THEN
                BEGIN
                factorOut := 0;
                headSize := 8 - prefixSize;
                END
            ELSE
                BEGIN
                factorOut := prefixSize - 8;
                headSize := 0;
                END;
            IF hasKey THEN
                headSize := headSize + 4;
            { now we have fill up according to the above table }
            IF varSize THEN
            { so that itemSize would not include theLength field }
                itemSize := headSize - 4
            ELSE
                WITH ListHdrHdl(msg)^^ DO
                    BEGIN
                    { if fixed sized and search by index, just calculate the position }
                { in the fixed size case, itemSize is the fix length, minus the factorout and
                    add back the headSize because there may be a key }
                    itemSize := aHeader.theLength - factorOut + headSize;
                    IF itemSize <> 1 THEN               { only round up if it is not packing, note
                                                         no key is implied }
                        itemSize := BAnd(itemSize + RoundUpValue, roundUpMask);
                    IF NOT useKey THEN
                        BEGIN                           { we may calculate the offset directly }
                        IF (index > 0) THEN
                            IF (index <= paramCount) THEN
                                BEGIN
                                offset := (index - 1) * itemSize + listClassHeader.paramBegin;
                                IF hasKey THEN
                                    key := AEKeyWord(LONGINTPtr(ord(Handle(msg)^) + offset)^);
                                END;
                        GOTO 999;
                        END;
                    END;
            END
        ELSE
            BEGIN                                       { it is an AppleEvent }
            hasKey := TRUE;
            varSize := TRUE;
            factorOut := 0;
            prefixSize := 0;
            headSize := 12;
            itemSize := 8;                              { keyword + type }
            END;
        IF index < 0 THEN                               { we are not trying to get any item }
            Exit(FindDescOffset);
        IF msgClass = classMeta THEN
            BEGIN
            { start point of search is just beyond 'aevt0101' }
            { end search at parameter BEGIN minus ';;;;' }
            lookingAt := 1;
            startPtr := CommonHeaderPointer(ord(Handle(msg)^) + BAnd(SizeOf(MsgHdr) -
                                            SizeOf(TargetID) + msg^^.msgAddrSize + 8 + RoundUpValue, RoundUpMask));
            dataEnd := ord(Handle(msg)^) + msg^^.listClassHeader.paramBegin - 4;
            END
        ELSE
            BEGIN
            lookingAt := msg^^.listClassHeader.hintIndex;
            IF (lookingAt > 0) AND (index > 0) AND (index >= lookingAt) THEN
                BEGIN
                { if this is beyond what we looked last time, we can start it from there }
                startPtr := CommonHeaderPointer(ord(Handle(msg)^) + msg^^.listClassHeader
                                                .hintOffset);
                END
            ELSE
                BEGIN
                { if we don't have hint, just start from the beginning }
                { note paramBegin is at the same position for both message and list }
                lookingAt := 1;
                startPtr := CommonHeaderPointer(ord(Handle(msg)^) + msg^^.listClassHeader
                                                .paramBegin);
                END;
            dataEnd := ord(Handle(msg)^) + GetHandleSize(Handle(msg));
            END;
        WHILE ord(startPtr) < dataEnd DO
            BEGIN
            IF (lookingAt = index) OR (useKey AND (LONGINTPtr(startPtr)^ = LONGINT(key))) THEN
                BEGIN
                offset := ord(startPtr) - ord(Handle(msg)^);
                IF hasKey THEN
                    key := startPtr^.theKeyword;
                IF NOT (useKey OR (msgClass = classMeta)) THEN
                    BEGIN
                    WITH msg^^.listClassHeader DO
                        BEGIN
                        hintIndex := lookingAt;
                        hintOffset := offset;
                        END;
                    END;
                index := lookingAt;                     { report where it was found }
                LEAVE;
                END
            ELSE
                BEGIN
                startPtr := CommonHeaderPointer(ord(startPtr) + itemSize);
                IF varSize THEN
            { startPtr := CommonHeaderPointer(ord(startPtr) + RoundUp(LONGINTPtr(startPtr)^ + 4)); }
                    startPtr := CommonHeaderPointer(ord(startPtr) + BAnd(LONGINTPtr(startPtr)^ +
                                                                         4 + RoundUpValue,
                                                                         roundUpMask));
                END;
            lookingAt := lookingAt + 1;
            END;
        END;
999:
    IF offset > 0 THEN
        WITH aHeaderInfoRec DO
            BEGIN
            aLongPtr := LONGINTPtr(ord(Handle(msg)^) + offset);
            IF hasKey THEN
                BEGIN                                   { pointer was at the keyword }
                aHeader.theKeyword := AEKeyWord(aLongPtr^);
                aLongPtr := LONGINTPtr(ord(aLongPtr) + 4);
                END;
            IF prefixSize = 0 THEN
                BEGIN                                   { type is at the item header }
                aHeader.theType := DescType(aLongPtr^);
                aLongPtr := LONGINTPtr(ord(aLongPtr) + 4);
                END;
            IF prefixSize <= 4 THEN                     { size is at the item header }
                aHeader.theLength := aLongPtr^;
            END;
    END;

{--------------------------------------------------------------------------------}

{ find the first required keyword that has not been accessed }

FUNCTION FirstRequiredUnread(VAR theAppleEvent: AppleEvent; { VAR is for efficiency only }
                             VAR theKey: AEKeyWord): BOOLEAN;

    VAR
        aHeaderPtr:         CommonHeaderPointer;
        aPtr:               Ptr;
        i:                  LONGINT;
        j:                  LONGINT;
        maxSize:            LONGINT;
        optionalCount:      LONGINT;
        anOptsKey:          AEKeyWord;
        wantType:           DescType;
        alwaysFalse:        BOOLEAN;
        functionResult:     BOOLEAN;
        hasOptionList:      BOOLEAN;
        err:                OSErr;
        optDesc:            AEDesc;

    BEGIN
    functionResult := FALSE;
    { get the list of optional parameters }
    theKey := keyOptionalKeywordAttr;
    hasOptionList := (FetchDesc(MakeMeta(theAppleEvent), 0, typeAEList, theKey, optDesc) = NoErr);
    optionalCount := 0;
    IF hasOptionList THEN
        err := AE_CountItems(optDesc, optionalCount);
    WITH MsgHdrHdl(theAppleEvent.dataHandle)^^ DO
        BEGIN
        alwaysFalse := (accessMask = 0);
        aPtr := @accessMask;
        IF (paramCount > 32) THEN
            IF (accessMask <> 0) THEN
                aPtr := Handle(accessMask)^;            { if > 32 parameter, we have a handle }
        aHeaderPtr := CommonHeaderPointer(ord(theAppleEvent.dataHandle^) + listClassHeader.
                                          paramBegin);
        FOR i := 0 TO paramCount - 1 DO
            BEGIN                                       { look at each parameter }
            IF alwaysFalse OR (NOT BitTst(aPtr, i)) THEN
                BEGIN                                   { access bit is 0, so not yet accessed }
                theKey := aHeaderPtr^.theKeyword;       { this is the one }
                functionResult := TRUE;
                { it was not accessed, but if it is optional that is OK, so check it }
                FOR j := 1 TO optionalCount DO
                    BEGIN
                    { this ReadData call should not move memory }
                    IF ReadData(optDesc, j, typeKeyword, theKey, wantType, @anOptsKey, 4, maxSize) =
                       NoErr THEN
                        IF (theKey = anOptsKey) THEN
                            BEGIN
                            functionResult := FALSE;    { this is optional, doesn't count }
                            LEAVE;
                            END;
                    END;
                IF functionResult THEN
                    LEAVE;                              { we found the missed required parameter }
                END;
            { aHeaderPtr := CommonHeaderPointer(ord(aHeaderPtr) + RoundUp(aHeaderPtr^.theLength +
                                              SizeOf(CommonHeader))); }
            aHeaderPtr := CommonHeaderPointer(ord(aHeaderPtr) + BAnd(aHeaderPtr^.theLength +
                                                                     SizeOf(CommonHeader) +
                                                                     RoundUpValue, roundUpMask));
            END;
        END;
    IF hasOptionList THEN
        err := AE_DisposeDesc(optDesc);
    FirstRequiredUnread := functionResult;
    END;

{--------------------------------------------------------------------------------}

{ this is the basic routine to get the attribute }
{ if it is a pseudo meta parameter, either calculate its position or calculate
    its value }
{ otherwise it is fetched just like a meta parameter }

FUNCTION GetAttribute(VAR theAevt: AppleEvent;          { VAR is for efficiency only }
                      key: AEKeyWord;
                      desiredType: DescType;
                      VAR typeCode: DescType;
                      dataPtr: Ptr;
                      maximumSize: LONGINT;
                      VAR actualSizeOrHandle: LONGINT;
                      wantDesc: BOOLEAN): OSErr;

    VAR
        err:                OSErr;
        result:             AEDesc;
        theDescClass:       DescClass;
        offset:             LONGINT;
        myPtr:              Ptr;
        flag:               SignedByte;
        enumAsInteger:      INTEGER;
        anEnum:             OSType;
        theAEEventSource:   AEEventSource;
        fromPSN:            ProcessSerialNumber;
		needCoercion:		Boolean;
		
    BEGIN
    err := errAENotAEDesc;
    IF (LONGINT(theAevt.descriptorType) = LONGINT(kCoreEventClass)) AND (theAevt.dataHandle <>
       NIL) THEN
        WITH MsgHdrHdl(theAevt.dataHandle)^^ DO
            BEGIN
            err := NoErr;
            myPtr := NIL;
            result.dataHandle := NIL;
            actualSizeOrHandle := 4;                    { most common used value, use as default }
            GetAttributeOffset(key, offset, typeCode);
            IF offset > 0 THEN
                BEGIN
                myPtr := Ptr(ord(theAevt.dataHandle^) + offset);
                END
            ELSE IF LONGINT(key) = LONGINT(keyAddressAttr) THEN
                BEGIN
                myPtr := @msgAddrTarget;
                actualSizeOrHandle := msgAddrSize;
                typeCode := msgAddrType;
                END
            ELSE IF LONGINT(key) = LONGINT(keyEventSourceAttr) THEN
                BEGIN                                   { calculate the event source }
                EventSource(MsgHdrHdl(theAevt.dataHandle), theAEEventSource, fromPSN);
                enumAsInteger := INTEGER(theAEEventSource);
                myPtr := @enumAsInteger;
                actualSizeOrHandle := 2;
                typeCode := typeShortInteger;
                END
            ELSE IF (LONGINT(key) = LONGINT(keyInteractLevelAttr)) AND (LONGINT(desiredType) <>
                    LONGINT(typeEnumerated)) THEN
                BEGIN                                   { get the interaction type and calculate the
                                                         interact level }
                enumAsInteger := kAECanInteract;
                IF ReadData(MakeMeta(theAevt), 0, typeEnumerated, key, typeCode, @anEnum, 4,
                            actualSizeOrHandle) = NoErr THEN
                    BEGIN
                    IF (LONGINT(anEnum) = LONGINT(kAlwaysInteract)) OR (LONGINT(anEnum) =
                       LONGINT(kAlwaysSwitchIfInteract)) THEN
                        enumAsInteger := kAEAlwaysInteract
                    ELSE IF LONGINT(anEnum) = LONGINT(kNeverInteract) THEN
                        enumAsInteger := kAENeverInteract;
                    END;
                myPtr := @enumAsInteger;
                actualSizeOrHandle := 2;
                typeCode := typeShortInteger;
                END
            ELSE
                BEGIN
                { for the rest of the attribute, we better make sure we have the reply first }
                err := CheckDesc(theAevt, theDescClass, TRUE);
                IF err = NoErr THEN
                    BEGIN
                    IF LONGINT(key) = LONGINT(keyMissedKeywordAttr) THEN
                        BEGIN                           { get the first missed required keyword }
                        IF FirstRequiredUnread(theAevt, AEKeyWord(anEnum)) THEN
                            BEGIN
                            myPtr := @anEnum;
                            actualSizeOrHandle := 4;
                            typeCode := typeKeyword;
                            END
                        ELSE
                            err := errAEDescNotFound;
                        END
                    ELSE
                        BEGIN                           { otherwise it is just a meta parameter }
                        IF wantDesc THEN
                            BEGIN                       { for descriptor use FetchDesc }
                            err := FetchDesc(MakeMeta(theAevt), 0, desiredType, key, result);
                            typeCode := result.descriptorType;
                            END
                        ELSE
                            BEGIN                       { for pointer data use ReadData }
                            err := ReadData(MakeMeta(theAevt), 0, desiredType, key, typeCode,
                                            dataPtr, maximumSize, actualSizeOrHandle);
                            END;
                        END;
                    END;
                END;
            IF myPtr <> NIL THEN
                BEGIN                                   { it is not a regular meta parameter }
                needCoercion := (LONGINT(desiredType) <> LONGINT(typeCode)) AND (LONGINT(desiredType) <>
                   LONGINT(typeWildCard));         { type does not match }
                IF needCoercion OR wantDesc THEN         { type does not match }
                    BEGIN
                    flag := HGetState(theAevt.dataHandle);
                    HLock(theAevt.dataHandle);
					IF needCoercion THEN
						BEGIN
						err := AE_CoercePtr(typeCode, myPtr, actualSizeOrHandle, desiredType, result);
						actualSizeOrHandle := 0;            { in case it is 'true' }
						typeCode := desiredType;
						END
					ELSE
						BEGIN 						{ type match, but we want a handle, so put it in }
                    	err := AE_CreateDesc(typeCode, myPtr, actualSizeOrHandle, result);
						END;
                    HSetState(theAevt.dataHandle, flag);
                    END;
                IF (err = NoErr) AND (NOT wantDesc) THEN
                    BEGIN
                    IF result.dataHandle <> NIL THEN
                        BEGIN                           { result was coerced, so we need to change
                                                         value of myPtr }
                        myPtr := result.dataHandle^;
                        actualSizeOrHandle := GetHandleSize(result.dataHandle);
                        END;
                    IF dataPtr <> NIL THEN
                        BEGIN
                        IF maximumSize > actualSizeOrHandle THEN
                            maximumSize := actualSizeOrHandle;
                        BlockMove(myPtr, dataPtr, maximumSize);
                        END;
                    DisposHandle(result.dataHandle);
                    END;
                END;
            IF (err = NoErr) AND wantDesc THEN
                actualSizeOrHandle := LONGINT(result.dataHandle); { return handle if it is desc }
            END;
    IF err <> NoErr THEN
        BEGIN
        actualSizeOrHandle := 0;
        typeCode := typeNull;
        END;
    GetAttribute := err;
    END;

{--------------------------------------------------------------------------------}

PROCEDURE GetAttributeOffset(key: AEKeyWord;
                             VAR offset: LONGINT;
                             VAR dataType: DescType);

    CONST
        msgAddrTargetOffset = SizeOf(MsgHdr) - SizeOf(TargetID);
        ReturnIDOffset      = msgAddrTargetOffset - 8 - SizeOf(LONGINT);
        eventIDOffset       = ReturnIDOffset - SizeOf(AEEventClass);
        eventClassOffset    = eventIDOffset - SizeOf(AEEventID);
        refconOffset        = eventClassOffset - SizeOf(LONGINT);

    VAR
        offsetFromFront:    INTEGER;

    BEGIN
    offsetFromFront := 0;
    dataType := typeType;                               { most common value, use as default }
    IF LONGINT(key) = LONGINT(keyEventClassAttr) THEN
        BEGIN
        offsetFromFront := eventClassOffset;
        END
    ELSE IF LONGINT(key) = LONGINT(keyEventIDAttr) THEN
        BEGIN
        offsetFromFront := eventIDOffset;
        END
    ELSE
        BEGIN
        dataType := typeLongInteger;
        IF LONGINT(key) = LONGINT(keyReturnIDAttr) THEN
            BEGIN
            offsetFromFront := ReturnIDOffset;
            END
        ELSE IF LONGINT(key) = LONGINT(kAERefconAttribute) THEN
            BEGIN
            offsetFromFront := refconOffset;
            END;
        END;
    offset := offsetFromFront;
    END;

{--------------------------------------------------------------------------------}

{ translate an desc to ptr and size, take into account it may be a list or record }

FUNCTION GetDescInfo(VAR theAEDesc: AEDesc;      { VAR is for efficiency only }
                     VAR descInfo: DescInfoRec): OSErr;

    VAR
		err:		OSErr;

    BEGIN
    WITH theAEDesc, descInfo DO
        BEGIN
		err := noErr;
		IF dataHandle = NIL THEN
			dataPtr := NIL
		ELSE
			dataPtr := dataHandle^;
		dataSize := GetHandleSize(dataHandle);
		dataType := descriptorType;
		IF (LONGINT(descriptorType) = LONGINT(typeAEList)) OR (LONGINT(descriptorType) = LONGINT(typeAERecord)) THEN
			BEGIN { skip the header if it is a list/record }
			IF dataPtr = NIL THEN
				err := errAENotAEDesc;
			dataType := DescType(ListHdrPtr(dataPtr)^.listClassHeader.metaCountOrObjType); { real type in the record }
			dataPtr := Ptr(ord(dataPtr)+SizeOf(ListClassRec));
			dataSize := dataSize - SizeOf(ListClassRec);
			END
        ELSE IF LONGINT(dataType) = LONGINT(kCoreEventClass) THEN
            BEGIN                                       { we don't allow AppleEvent to be added as
                                                         descriptor }
            err := errAEWrongDataType;
            END;
		IF err = noErr THEN { we lock it down so the ptr can be used }
			BEGIN
			flag := HGetState(dataHandle);
			HLock(dataHandle);
			END;
        END;
		GetDescInfo := err;
    END;

{--------------------------------------------------------------------------------}

{ if it is autoGenerateReturnID, then we make one up ourself }

FUNCTION MakeReturnID(ReturnID: LONGINT): LONGINT;

    VAR
        globalHandle:       GlobalRecHandle;
        anID:               INTEGER;

    BEGIN
    globalHandle := GetGlobalRef;
    IF ReturnID = kAutoGenerateReturnID THEN
        BEGIN
        IF globalHandle = NIL THEN
            BEGIN
            { no global, just make a random one where high word cannot be 0 or -1 }
            anID := $20000 + Random;
            MakeReturnID := anID;
            END
        ELSE
            WITH globalHandle^^ DO
                BEGIN                                   { we just add 1 to our counter }
                ReturnIDCounter := ReturnIDCounter + 1;
                { ReturnID count must not have high word = 0 or -1 }
                { as we increment returnID, we would get up to $7FFFFFFF, $80000000 so on,
                    eventually to get to $FFFEFFFF, $FFFF0000 then we should skip ahead to
                    $00010000 }
                IF (ReturnIDCounter = $FFFF0000) THEN
                    ReturnIDCounter := $10000;
                MakeReturnID := ReturnIDCounter;
                END;
        END
    ELSE
        MakeReturnID := ReturnID;
    END;

{--------------------------------------------------------------------------------}

{ classify the type of AEDesc, and if waitReady is TRUE then if it is an AppleEvent then check
    to see if reply is ready, also check for blocking }

FUNCTION CheckDesc(VAR theAEDescList: AEDescList;
                   VAR theDescClass: DescClass;
                   waitReady: BOOLEAN): OSErr;

    VAR
        theClass:           DescClass;                  { shorter code if we use these two local
                                                         variables }
        err:                OSErr;
        tempAppleEvent:     AppleEvent;

    BEGIN
    err := NoErr;
    WITH theAEDescList DO
        BEGIN
        IF dataHandle = NIL THEN
            BEGIN
            err := errAENotAEDesc;
            theClass := classEmpty;
            END
        ELSE IF LONGINT(descriptorType) = LONGINT(kCoreEventClass) THEN
            theClass := classAevt
        ELSE IF LONGINT(descriptorType) = LONGINT(typeAEList) THEN
            theClass := classList
        ELSE IF LONGINT(descriptorType) = LONGINT(typeAERecord) THEN
            theClass := classReco
        ELSE IF LONGINT(descriptorType) = LONGINT(typeMeta) THEN
            theClass := classMeta
        ELSE
            BEGIN
            err := errAENotAEDesc;
            theClass := classOther;
            END;
        theDescClass := theClass;
        IF (theClass <= classAevt) THEN
            BEGIN
            WITH MsgHdrHdl(dataHandle)^^ DO
                BEGIN
                IF paramCount < 0 THEN
                    err := errAECorruptData
                ELSE IF waitReady THEN
					IF inWaiting THEN
						BEGIN
						tempAppleEvent := theAEDescList;
						tempAppleEvent.descriptorType := kCoreEventClass;
						IgnoreOSErr(TryBothProc(tempAppleEvent, blockingoffset)); { call blocking proc
							if necessary }
						{ we check again since thing may have been changed by the blocking proc }
						{ we need to dereference again because memory may have been moved }
						WITH MsgHdrHdl(dataHandle)^^ DO
							BEGIN
							IF paramCount < 0 THEN
								err := errAECorruptData
							ELSE IF inWaiting THEN
								err := errAEReplyNotArrived;
							END;
						END;
                END;
            END;
        END;
    CheckDesc := err;
    END;

{--------------------------------------------------------------------------------}
{ this is the core routine for reading a descriptor into area pointed to by dataPtr }
{ if search by index, then index <> 0, if search by key, index = 0 }

FUNCTION ReadData(VAR theAEDescList: AEDescList;        { VAR is for efficiency only }
                  index: LONGINT;
                  desiredType: DescType;                { --> desired type }
                  VAR key: AEKeyWord;                   { --> the searchkey if index <> 0 }
  { <-- the actual key if index = 0 }
                  VAR typeCode: DescType;               { <-- actual type }
                  dataPtr: Ptr;
                  maximumSize: LONGINT;                 { --> size of buffer }
                  VAR actualSize: LONGINT               { <-- actual size of descriptor }
                  ): OSErr;

    VAR
        startPtr:           CommonHeaderPointer;
        offset:             LONGINT;
        err:                OSErr;
        msg:                MsgHdrHdl;
        msgClass:           DescClass;
        aPtr:               Ptr;
        toBeConverted, converted: AEDesc;
        aHeaderInfoRec:     HeaderInfoRec;
        dataSize, moved:    LONGINT;

    BEGIN
    err := CheckDesc(theAEDescList, msgClass, TRUE);
    msg := MsgHdrHdl(theAEDescList.dataHandle);
    IF err = NoErr THEN
        BEGIN
        FindDescOffset(msgClass, msg, index, key, offset, aHeaderInfoRec);
        IF offset > 0 THEN
            BEGIN
            typeCode := desiredType;
            { dataoffset is offset to beginning of that item in the list }
            WITH aHeaderInfoRec DO
                BEGIN
                offset := offset + headSize;            { skip the header, this is the real data }
                aPtr := Ptr(ord(Handle(msg)^) + offset);
                dataSize := aHeader.theLength - factorOut;
                toBeConverted.dataHandle := NIL;
                converted.dataHandle := NIL;
                IF (LONGINT(aHeader.theType) <> LONGINT(desiredType)) AND (LONGINT(desiredType) <>
                   LONGINT(typeWildCard)) THEN
                    BEGIN                               { the type is not right, we need to do it by
                                                         coercion }
                    WITH aHeader DO                     { first just get it as is }
                        err := ExtractDescriptor(msg, theType, theLength, offset, factorOut,
                                                 toBeConverted);
                    IF err = NoErr THEN
                        BEGIN
                        { since we now have AE_CoercePtr in addition to AE_CoerceDesc, in future
                            we can take advantage of it for better performance, but be careful about
                            'list' and factored list }
                        { then we coerce it }
                        err := AE_CoerceDesc(toBeConverted, desiredType, converted);
                        { dispose the as is copy if not same as the result }
                        IF toBeConverted.dataHandle <> converted.dataHandle THEN
                            IgnoreOSErr(AE_DisposeDesc(toBeConverted));
                        IF err = NoErr THEN
                            BEGIN
                            IF (LONGINT(converted.descriptorType) = LONGINT(typeAEList)) OR
                               (LONGINT(converted.descriptorType) = LONGINT(typeAERecord)) THEN
                                BEGIN                   { cannot read list or record into a buffer }
                                err := errAEWrongDataType;
                                END
                            ELSE
                                WITH converted DO
                                    BEGIN
                                    IF dataHandle <> NIL THEN
                                        BEGIN
                                        dataSize := GetHandleSize(dataHandle);
                                        aPtr := dataHandle^;
                                        HLock(dataHandle);
                                        END
                                    ELSE                { in case it is 'true' }
                                        dataSize := 0;
                                    END;
                            factorOut := 0;
                            END;
                        END;
                    END
                ELSE
                    typeCode := aHeader.theType;
                IF (err = NoErr) AND (dataPtr <> NIL) THEN
                    BEGIN
                    moved := 0;                         { we have not moved anything yet }
                    IF factorOut > 0 THEN
                        BEGIN
                        moved := factorOut;             { we plan to move factorOut data from the
                                                         prefix }
                        IF moved > maximumSize THEN
                            moved := maximumSize;       { but only up to maximumSize }
                        BlockMove(Ptr(ord(Handle(msg)^) + SizeOf(ListHdr) + 8), dataPtr, moved);
                        maximumSize := maximumSize - moved; { we can still move that many bytes }
                        END;
                    IF maximumSize > dataSize THEN      { but no more than the size of data }
                        maximumSize := dataSize;
                    BlockMove(aPtr, Ptr(ord(dataPtr) + moved), maximumSize); { move the non-facoted
                        part }
                    END;
                actualSize := dataSize + factorOut;
                IgnoreOSErr(AE_DisposeDesc(converted));
                IF msgClass = classAevt THEN
                    SetAccessMask(msg, index);
                END;
            END
        ELSE
            BEGIN
            err := errAEDescNotFound;
            IF LONGINT(key) = LONGINT(aeRecTypeKeyword) THEN
                IF (index = 0) AND (msgClass = classReco) THEN
                    BEGIN                               { it may be the pseudo keyword for the
                                                         record type }
                    IF (LONGINT(desiredType) = LONGINT(typeType)) OR (LONGINT(desiredType) =
                       LONGINT(typeWildCard)) THEN
                        BEGIN
                        typeCode := typeType;
                        actualSize := 4;
                        IF maximumSize >= 4 THEN
                            BEGIN
                            LONGINTPtr(dataPtr)^ := ListHdrHdl(msg)^^.listClassHeader.
                                                    metaCountOrObjType;
                            err := NoErr;
                            END;
                        END
                    END;
            END;
        END;
    ReadData := err;
    END;

{--------------------------------------------------------------------------------}
{ set access mask to indicate that parameter has been accessed }

PROCEDURE SetAccessMask(msg: MsgHdrHdl;
                        index: LONGINT);

    VAR
        aHandle:            Handle;

    BEGIN
    { we want to mark it as being read }
    IF msg^^.paramCount > 32 THEN
        BEGIN
        IF msg^^.accessMask = 0 THEN
            BEGIN                                       { we make a handle out of it }
            aHandle := NewHandleClear(BSR(msg^^.paramCount + 7, 3));
            msg^^.accessMask := LONGINT(aHandle);
            END;
        IF msg^^.accessMask <> 0 THEN
            BitSet(Handle(msg^^.accessMask)^, index - 1);
        END
    ELSE
        BitSet(@msg^^.accessMask, index - 1);
    END;

{--------------------------------------------------------------------------------}
{ put the target address into an AppleEvent }

FUNCTION SetMsgAddress(theMsgHdrHdl: MsgHdrHdl;
                       typeCode: DescType;
                       dataPtr: Ptr;
                       dataSize: LONGINT): OSErr;

    CONST
        msgAddrTargetOffset = SizeOf(MsgHdr) - SizeOf(TargetID);

    VAR
        err:                OSErr;
        disp:               LONGINT;
		roundOldSize:		LONGINT;
		roundNewSize:		LONGINT;

    BEGIN
	roundOldSize := BAnd(theMsgHdrHdl^^.msgAddrSize + RoundUpValue, RoundUpMask);
	roundNewSize := BAnd(dataSize + RoundUpValue, RoundUpMask);
    disp := Munger(Handle(theMsgHdrHdl), msgAddrTargetOffset, NIL, roundOldSize,
                   dataPtr, roundNewSize);
    err := MemError;
    IF err = NoErr THEN
        WITH theMsgHdrHdl^^ DO
            BEGIN
            disp := roundNewSize - roundOldSize;             { did the size changed ? }
            msgAddrSize := dataSize;
            msgAddrType := typeCode;
            WITH listClassHeader DO
                BEGIN
                hintIndex := 0;
                IF disp <> 0 THEN                       { if size changed, better adjust }
                    paramBegin := paramBegin + disp;
                END;
            END;
    SetMsgAddress := err;
    END;

{--------------------------------------------------------------------------------}
{ this is the core routine to write a descriptor into a AEList/AppleEvent }

FUNCTION WriteData(VAR theAEDescList: AEDescList;       { VAR is for efficiency only }
                   key: AEKeyWord;
                   dataType: DescType;
                   dataPtr: Ptr;
                   dataLength: LONGINT;
                   index: LONGINT;
                   numberOfItems: LONGINT): OSErr;

    VAR
        msgClass:           DescClass;
        msg:                MsgHdrHdl;
        testKey:            AEKeyWord;
        startPtr:           CommonHeaderPointer;
        offset, disp, toBeReplaced, replaceLength: LONGINT;
        err:                OSErr;
        extraItem:          INTEGER;
        aHeaderInfoRec:     HeaderInfoRec;
        dummyHeaderInfoRec: HeaderInfoRec;
        dummyKey:           AEKeyWord;
        packing:            BOOLEAN;
        aLongPtr:           LONGINTPtr;
        currentCount:       LONGINT;
        endingIndex:        LONGINT;
        endingOffset:       LONGINT;

    BEGIN
    err := CheckDesc(theAEDescList, msgClass, FALSE);
    msg := MsgHdrHdl(theAEDescList.dataHandle);
    IF err = NoErr THEN
        BEGIN
        IF msgClass = classAevt THEN
            WITH msg^^ DO
                BEGIN
                { access mask will be all wrong anyway, in particular we want to avoid going from
                    32 parameter to 33 parameter and hence change the meaning of accessMask }
                IF paramCount > 32 THEN                 { accessMask is really a handle }
                    DisposHandle(Handle(accessMask));
                accessMask := 0;
                END;
        testKey := key;
        IgnoreOSErr(AE_CountItems(theAEDescList, currentCount));
        { first find if there is such an item }
        FindDescOffset(msgClass, msg, index, testKey, offset, aHeaderInfoRec);
        IF dataPtr = NIL THEN
            BEGIN
            dataPtr := @dataLength;                     { we must put in some non-zero non-odd value
                                                         }
            dataLength := 0;
            END;
        WITH aHeaderInfoRec DO
            BEGIN
            WITH aHeader DO
                packing := (headSize = 0) AND ((theLength - factorOut) = 1);
            IF offset <= 0 THEN
                BEGIN                                   { we did not have an entry yet }
                { key = '****'      hasKey
                    true            true        error, require a key and we don't have one
                    false           false       error, adding a key desc to an ordinary list
                    false           true        add key desc to record, OK
                    true            fasle       add desc to list, OK }
                IF (LONGINT(key) = LONGINT(typeWildCard)) = hasKey THEN
                    BEGIN
                    WriteData := errAEWrongDataType;
                    Exit(WriteData);
                    END
                ELSE IF index <> 0 THEN                 { we have to check if we are writing to N+1
                                                         }
                    BEGIN
                    IF (index <> (currentCount + 1)) THEN
                        BEGIN
                        WriteData := errAEIllegalIndex;
                        Exit(WriteData);
                        END;
                    END;
                toBeReplaced := 0;
                IF msgClass = classMeta THEN            { meta parameter go to just before the
                                                         ';;;;' }
                    offset := MsgHdrHdl(msg)^^.listClassHeader.paramBegin - 4
                ELSE IF (LONGINT(key) = LONGINT(keyDirectObject)) AND
                        (LONGINT(theAEDescList.descriptorType) = LONGINT(kCoreEventClass)) THEN
                { put direct object in front }
                    offset := MsgHdrHdl(msg)^^.listClassHeader.paramBegin
                ELSE                                    { put it at the end }
                    BEGIN
                    offset := GetHandleSize(Handle(msg));
                    END;
                extraItem := 0;                         { we start with no item }
                END
            ELSE
                BEGIN
                { wildcard key means keep original key }
                IF LONGINT(key) = LONGINT(typeWildCard) THEN
                    key := aHeader.theKeyword;
                extraItem := - numberOfItems;           { we are removing so many items }
                IF numberOfItems > 1 THEN               { we want to replace more than 1 items }
                    BEGIN
                    endingIndex := index + numberOfItems;
                    IF endingIndex > currentCount THEN
                        BEGIN
                        toBeReplaced := GetHandleSize(Handle(msg)) - offset;
                        extraItem := index - currentCount - 1; { negative of items left at end }
                        END
                    ELSE
                        BEGIN
                        { we find the end of the range so that we know how many to replace }
                        FindDescOffset(msgClass, msg, endingIndex, dummyKey, endingOffset,
                                       dummyHeaderInfoRec);
                        toBeReplaced := endingOffset - offset;
                        END;
                    END
                ELSE
                    BEGIN                               { only trying to do to one item }
                    IF packing THEN                     { then we only have 1 byte }
                        toBeReplaced := 1
                    ELSE                                { this is the length in the item plus any
                                                         associated header }
                        toBeReplaced := BAnd(aHeader.theLength + headSize - factorOut +
                                             RoundUpValue, roundUpMask);
                    END;
                END;
          { since ptr point to the factored data, we should reduce headSize by size of prefix data }
            IF ord(dataPtr) = - 1 THEN
                BEGIN
                dataPtr := @dataLength;                 { we must put in some non-zero non-odd value
                                                         }
                replaceLength := 0;                     { new length is 0 }
                END
            ELSE
                BEGIN
                { since we are adding element, we need to check if it agrees with the prefix }
                WITH aHeader DO
                    BEGIN
                    { if there is a prefix, we check if see the new item agree with prefix }
                    IF ((prefixSize >= 8) AND (theLength <> dataLength)) OR ((prefixSize >= 4) AND
                       (theType <> dataType)) THEN
                        BEGIN
                        WriteData := errAEBadListItem;
                        Exit(WriteData);
                        END;
                    END;
                extraItem := extraItem + 1;             { we are adding 1 item back }
                IF packing THEN                         { length is always 1 for packing }
                    replaceLength := 1
                ELSE                                    { this is the length of the new item }
                    replaceLength := BAnd(dataLength + headSize - factorOut + RoundUpValue,
                                          roundUpMask);
                END;
            { and now we insert/replace/delete the item }
            disp := Munger(Handle(msg), offset, NIL, toBeReplaced, Ptr(ord(dataPtr) -
                                                                       headSize + factorOut),
                           replaceLength);

            err := MemError;
            IF (extraItem >= 0) AND (err = NoErr) THEN
                BEGIN                                   { it is not a delete }
                aLongPtr := LONGINTPtr(ord(Handle(msg)^) + offset);
                IF hasKey THEN
                    BEGIN                               { put in the keyword }
                    aLongPtr^ := LONGINT(key);
                    aLongPtr := LONGINTPtr(ord(aLongPtr) + 4);
                    END;
                IF prefixSize = 0 THEN
                    BEGIN                               { type is not uniform, so it is in the
                                                         header }
                    aLongPtr^ := LONGINT(dataType);
                    aLongPtr := LONGINTPtr(ord(aLongPtr) + 4);
                    END;
                IF prefixSize <= 4 THEN                 { size is not uniform, so it is in the
                                                         header }
                    aLongPtr^ := dataLength;
                END;
            END;
        IF err = NoErr THEN
            WITH MsgHdrHdl(msg)^^ DO
                BEGIN
                listClassHeader.hintIndex := 0;     { invalidate hint on write }
                IF msgClass = classMeta THEN
                    WITH listClassHeader DO
                        BEGIN
                        paramBegin := paramBegin + replaceLength - toBeReplaced;
                        metaCountOrObjType := metaCountOrObjType + extraItem;
                        END
                ELSE                                    { paramCount has same position for aevt,
                                                         list and reco }
                    BEGIN
                    paramCount := paramCount + extraItem;
                    END;
                END;
        END;
    WriteData := err;
    END;

{--------------------------------------------------------------------------------}







