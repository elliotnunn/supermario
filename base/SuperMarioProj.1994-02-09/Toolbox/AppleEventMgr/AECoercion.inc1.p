{
	File:		AECoercion.inc1.p

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <3>	  2/5/91	Lai		BM,#81596: Also for bug jfr101 on whiteboard. Move the
									functionality of making a 'reco' in coercion to AECreateDesc and
									just call AECreateDesc.
		 <2>	 1/17/91	Lai		In StdCoercion, check for overflow going from 'sing' to 'doub'
									because source may be equal to 'INF'.
		 <1>	12/13/90	Lai		first checked in after subdividing into smaller modules

	To Do:
}

{ Copyright © 1984-1991 by Apple Computer Inc. All rights reserved. }

CONST
    singPrecision = 8; { from SANE manual page 14 }
    doubPrecision = 16; { from SANE manual page 14 }
    compPrecision = 19; { from SANE manual page 14 }
    extePrecision = 20; { from SANE manual page 14 }
    magnPrecision = 10;

TYPE
    CompOrTwoLong = RECORD
                        CASE Boolean OF
                            FALSE:
                                (asComp: Comp);
                            TRUE:
                                (asLong: ARRAY [0..1] OF LONGINT);
                    END;

    UnivNum = RECORD
                  CASE Integer OF
                      1:
                          (asSignedByte: SignedByte);
                      2:
                          (asInteger: Integer);
                      3:
                          (asLongint: Longint);
                      4:
                          (asReal: Real);
                      5:
                          (asComp: Comp);
                      6:
                          (asDouble: Double);
                      7:
                          (asExtended: Extended);
                      8:
                          (asResType: ResType);
              END;

{--------------------------------------------------------------------------------}

FUNCTION CallPtrCoercion(typeCode: DescType;
                         dataPtr: Ptr;
                         dataSize: LONGINT;
                         toType: DescType;
                         refcon: LONGINT;
                         VAR result: AEDesc;
                         DoProc: ProcPtr): OSErr;
{ inline is equivalent to : movea.l (a7)+,a0; jsr (a0) }
    INLINE $205F, $4E90;

{--------------------------------------------------------------------------------}

FUNCTION CallDescCoercion(desc: AEDesc;
                          toType: DescType;
                          refcon: LONGINT;
                          VAR result: AEDesc;
                          DoProc: ProcPtr): OSErr;
{ inline is equivalent to : movea.l (a7)+,a0; jsr (a0) }
    INLINE $205F, $4E90;

{--------------------------------------------------------------------------------}

{ we need these from AEInteract }

FUNCTION AE_InteractWithUser(timeOutInTicks: LONGINT; { how long are you willing to wait }
							 nmReqPtr: NMRecPtr; { your own custom notification }
							 idleProc: IdleProcPtr { your own idle procedure }
							 ): OSErr; EXTERNAL;

{--------------------------------------------------------------------------------}

{ we need these from AEDFWrapper }

FUNCTION AE_CountItems(VAR theAEDescList: AEDescList; { VAR is for efficiency only }
                       VAR theCount: LONGINT): OSErr;
    EXTERNAL;

FUNCTION AE_PutPtr(VAR theAEDescList: AEDescList; { VAR is for efficiency only }
                   index: LONGINT;
                   typeCode: DescType;
                   dataPtr: Ptr;
                   dataSize: LONGINT): OSErr;
    EXTERNAL;

FUNCTION AE_GetNthDesc(VAR theAEDescList: AEDescList; { VAR is for efficiency only }
                       index: LONGINT;
                       desiredType: DescType;
                       VAR theAEKeyword: AEKeyWord;
                       VAR theAEDesc: AEDesc): OSErr;
    EXTERNAL;

FUNCTION AE_DisposeDesc(VAR theAEDesc: AEDesc): OSErr;
    EXTERNAL;

{--------------------------------------------------------------------------------}

FUNCTION StdCoercion(fromType: DescType;
                     dataPtr: Ptr;
                     dataSize: LONGINT;
                     toType: DescType;
                     VAR result: AEDesc): OSErr;
    FORWARD;

{--------------------------------------------------------------------------------}

{ coerce data froma a buffer to a desc }
{ not that if the coercion routine is for desc to desc, we have to make a desc first }

FUNCTION AE_CoercePtr(typeCode: DescType;
                      dataPtr: Ptr;
                      dataSize: LONGINT;
                      toType: DescType;
                      VAR result: AEDesc): OSErr;

    VAR
        err: OSErr;
        tempDesc: AEDesc;

    FUNCTION TryFromTable(aGlobal: GlobalRecHandle): OSErr;

        VAR
            theHandlerRec: HandlerRec;
            hasProc: Boolean;
            err: OSErr;

        BEGIN
            err := errAECoercionFail;
            IF aGlobal <> NIL THEN
                BEGIN
                WITH aGlobal^^ DO
					IF coercionHashTable <> NIL THEN
                    	IF GetTableInfo(coercionHashTable, typeCode, toType, theHandlerRec) THEN
							WITH theHandlerRec DO
								BEGIN
								IF BTst(ord(theProc), 0) { odd(ord(theCoercionProc))} THEN
									BEGIN { it is the format from desc to desc }
									{ since it is desc to desc, we have to make one first }
									err := AE_CreateDesc(typeCode, dataPtr, dataSize, tempDesc);
									IF err = noErr THEN
										BEGIN
										{ now we call coerce it, the real address is off by 1  }
										err := CallDescCoercion(tempDesc, toType, theRefCon, result,
																ProcPtr(ord(theProc) - 1));
										IgnoreOSErr(AE_DisposeDesc(tempDesc)); 
										END;
									END
								ELSE { otherwise we can do it directly }
									err := CallPtrCoercion(typeCode, dataPtr, dataSize, toType, theRefCon,
														   result, theProc);
								END;
                END;
            TryFromTable := err;
        END;

    BEGIN
        err := TryFromTable(GetGlobalRef);
        IF err = errAECoercionFail THEN err := TryFromTable(GetSysGlobal);
        IF err = errAECoercionFail THEN
            err := StdCoercion(typeCode, dataPtr, dataSize, toType, result);
        IF err <> noErr THEN NukeIt(result);
        AE_CoercePtr := err;
    END;

{--------------------------------------------------------------------------------}
{ coerce data from a desc to a desc }
{ the built in desc to desc coercion is here rather than in StdCercion because
    StdCoercion is for ptr to desc }
{ the built in ones are
    AEList to anything, fetch the only item in the list
    AERecord to anything, same format
    AppleEvent to typeAppParameters, so it can be used in launch
    Alias to FSS, because it is useful for the required events }

FUNCTION AE_CoerceDesc(VAR desc: AEDesc; { VAR is for efficiency only }
                       toType: DescType;
                       VAR result: AEDesc): OSErr;

    VAR
        err: 				OSErr;
        flags: 				SignedByte;
        count: 				LONGINT; { number of elements in the list }
        dummyKey: 			AEKeyWord;
        dataSize: 			LONGINT;
        aHandle: 			Handle;
        aFSS: 				FSSpec;
        aBool: 				boolean;
        refcon: 			LONGINT;
		aliasCount:			Integer;

    FUNCTION TryFromTable(aGlobal: GlobalRecHandle): OSErr;

        VAR
            theHandlerRec: HandlerRec;
            hasProc: Boolean;
            err: OSErr;

        BEGIN
            err := errAECoercionFail;
            IF aGlobal <> NIL THEN
                BEGIN
                WITH aGlobal^^ DO
					IF coercionHashTable <> NIL THEN
                    	IF  GetTableInfo(coercionHashTable, desc.descriptorType, toType,
                                            theHandlerRec) THEN
							WITH theHandlerRec DO
								BEGIN
								IF BTst(ord(theProc), 0) { odd(ord(theCoercionProc))} THEN
									BEGIN { it is the format from desc to desc, value of proc is off by 1 }
									err := CallDescCoercion(desc, toType, theRefCon, result,
															ProcPtr(ord(theProc) - 1));
									END
								ELSE
									WITH desc DO
										BEGIN
										IF dataHandle <> NIL THEN
											BEGIN
											flags := HGetState(dataHandle);
											HLock(dataHandle);
											dataSize := GetHandleSize(dataHandle)
											END
										ELSE
											dataSize := 0;
										err := CallPtrCoercion(descriptorType, dataHandle^, dataSize,
															   toType, theRefCon, result, theProc);
										HSetState(dataHandle, flags);
										END;
								END;
                END;
            TryFromTable := err;
        END;

    BEGIN
        err := TryFromTable(GetGlobalRef);
        IF err = errAECoercionFail THEN err := TryFromTable(GetSysGlobal);
        IF err = errAECoercionFail THEN
            WITH desc DO
                BEGIN
				IF (LONGINT(descriptorType) = LONGINT(toType)) THEN
					err := AE_DuplicateDesc(desc, result)
                ELSE IF (LONGINT(descriptorType) = LONGINT(typeAEList)) AND (LONGINT(toType) <>
                   LONGINT(typeWildCard)) THEN
                    BEGIN { special handling for list since it cannot be done as pointer to data }
                    { get the first element if there is only one element }
                    IF AE_CountItems(desc, count) = noErr THEN
                        IF count = 1 THEN
                            BEGIN { get first and only element and coerce it to right type }
                        { note that this means it can be recurseive, so if you have an element in a list
                        in a list in a list etc, it would still be fetched }
                            err := AE_GetNthDesc(desc, 1, toType, dummyKey, result);
                            END;
                    END
                ELSE IF (LONGINT(descriptorType) = LONGINT(typeAERecord)) AND 
					(LONGINT(toType) <> LONGINT(typeWildCard)) THEN
                    WITH desc DO
                        BEGIN
                    { special case for AERecord, anything can be have the same format as AERecord
                        if you ask to coerce AERecord to any type, it is assumed that you know that
                        the format is the same as AERecord, so if it is not, as in the case
                        you try to coerce an AERecord into a 'PICT', it is your mistake }
                        flags := HGetState(dataHandle);
                        HLock(dataHandle);
                        WITH ListHdrHdl(dataHandle)^^ DO
                            err := PtrToHand(Ptr(ord(dataHandle^) + SizeOf(listClassRec)),
                                             result.dataHandle, GetHandleSize(dataHandle) -
                                             SizeOf(listClassRec));
                        result.descriptorType := toType;
                        HSetState(dataHandle, flags);
                        END
                ELSE IF (LONGINT(descriptorType) = LONGINT(kCoreEventClass)) AND (LONGINT(toType) =
                        LONGINT(typeAppParameters)) THEN
                    BEGIN
                    { this convert an AppleEvent into the data strucutre used in launch }
                    { count is offset to the begin of AppleEvent }
                    count := SizeOf(MsgHdr) - SizeOf(TargetID) + MsgHdrHdl(desc.dataHandle)^^.
                             msgAddrSize;
                    dataSize := GetHandleSize(dataHandle) - count;
                    aHandle := NewHandle(dataSize + SizeOf(EventRecord) + 8);
                    err := MemError;
                    IF aHandle <> NIL THEN
                        BEGIN
                        WITH MsgHdrHdl(desc.dataHandle)^^, AppParametersPtr(aHandle^)^ DO
                            BEGIN
                            WITH theMsgEvent DO
                                BEGIN
                                what := kHighLevelEvent;
                                message := LONGINT(thisAEEventClass);
                                where := Point(thisAEEventID);
                                modifiers := 0;
                                END;
                            eventRefCon := ReturnID;
                            messageLength := dataSize;
                            END;
                        BlockMove(Ptr(ord(desc.dataHandle^) + count), Ptr(ord(aHandle^) +
                                                                          SizeOf(EventRecord) + 8),
                                  dataSize);
                        WITH result DO
                            BEGIN
                            descriptorType := toType;
                            dataHandle := aHandle;
                            END;
                        END;
                    END
                ELSE IF (LONGINT(descriptorType) = LONGINT(typeAlias)) AND (LONGINT(toType) =
                        LONGINT(typeFSS)) THEN
                    BEGIN
                    { alias to SFS is just resolve alias }
                    { err := ResolveAlias(NIL, AliasHandle(desc.dataHandle), aFSS, aBool); }
					{ we can use MatchAlias instead because we may not want to involve user }
					aliasCount := 1;
					err := MatchAlias(NIL, kARMsearch + kARMmountVol + kARMnoUI, AliasHandle(desc.dataHandle), aliasCount, 
						@aFSS, aBool, NIL, NIL);
					{ if volume is not mounted, we try again to mount volume if we can interact }
					IF err = nsvErr THEN
						IF AE_InteractWithUser(kAEDefaultTimeout, NIL, NIL) = noErr THEN
							BEGIN
							aliasCount := 1;
							err := MatchAlias(NIL, kARMsearch + kARMmountVol, AliasHandle(desc.dataHandle), aliasCount, 
								@aFSS, aBool, NIL, NIL);
							END;
                    IF err = noErr THEN
                        BEGIN
                        err := PtrToHand(@aFSS, result.dataHandle, SizeOf(aFSS));
                        result.descriptorType := typeFSS;
                        END;
                    END
                ELSE
                    BEGIN { point to data and call AE_CoercePtr }
                { we cannot do it with standard desc to desc coercion
                    maybe we can do it with standard buffer to desc coercion }
                    flags := HGetState(dataHandle);
                    HLock(dataHandle);
                    err := StdCoercion(descriptorType, dataHandle^, GetHandleSize(dataHandle),
                                       toType, result);
                    HSetState(dataHandle, flags);
                    END;
                END;
        IF err <> noErr THEN NukeIt(result);
        AE_CoerceDesc := err;
    END;

{--------------------------------------------------------------------------------}

{ whether the data type is a kind of boolean, and is what is the value }

FUNCTION BooleanType(fromType: DescType;
                     dataPtr: Ptr;
                     VAR result: integer): Boolean;

    VAR
        booleanAsInteger: Integer;

    BEGIN
        booleanAsInteger := - 1; { an illegal value }
        IF LONGINT(fromType) = LONGINT(typeBoolean) THEN
            booleanAsInteger := dataPtr^
        ELSE IF LONGINT(fromType) = LONGINT(typeTrue) THEN
            booleanAsInteger := 1
        ELSE IF LONGINT(fromType) = LONGINT(typeFalse) THEN
            booleanAsInteger := 0
        ELSE IF LONGINT(fromType) = LONGINT(typeEnumerated) THEN
            BEGIN
            IF LONGINTPtr(dataPtr)^ = LONGINT(typeTrue) THEN
                booleanAsInteger := 1
            ELSE IF LONGINTPtr(dataPtr)^ = LONGINT(typeFalse) THEN booleanAsInteger := 0;
            END
        ELSE IF LONGINT(fromType) = LONGINT(typeShortInteger) THEN
            booleanAsInteger := IntegerPtr(dataPtr)^;
        BooleanType := (booleanAsInteger = 0) OR (booleanAsInteger = 1);
        result := booleanAsInteger;
    END;

{--------------------------------------------------------------------------------}

{ Check to see the buffer look anything like an AERecord buffer at all }

FUNCTION CheckIsRecord(dataPtr: Ptr;
                       dataLen: longint): boolean;

    TYPE
        factoredHeader = RECORD
                             parameterCount: LONGINT;
                             sizeOfPrefix: LONGINT;
                             typeOfData: OSType;
                             eachItemSize: LONGINT;
                         END;
        factoredHeaderPtr = ^factoredHeader;

    VAR
        dataEnd, count, prefixSize, index: longint;
        aPtr: ptr;

    BEGIN
        CheckIsRecord := false;
        { dataEnd := ord(dataPtr) + RoundUp(dataLen); }
        dataEnd := ord(dataPtr) + BAnd(dataLen + RoundUpValue, roundUpMask); { this is the end of
            the record }
        WITH factoredHeaderPtr(dataPtr)^ DO
            BEGIN
            count := parameterCount;
            prefixSize := sizeOfPrefix;
            IF count < 0 THEN EXIT(CheckIsRecord);
            { skip to the beginning of first record }
            { we skip parameterCount and sizeOfPrefix field, i.e. 8 bytes then the prefix }
            { aPtr := Ptr(ord(dataPtr) + RoundUp(prefixSize+8)); }
            aPtr := Ptr(ord(dataPtr) + BAnd(prefixSize + 8 + RoundUpValue, roundUpMask));
            IF prefixSize >= 8 THEN
                BEGIN
{ factorout is RoundUp(prefixSize - 8), so real item size is eachItemSize - RoundUp(prefixSize + 8) }
                { and we need to add 4 for the keyword }
           { aPtr := Ptr(ord(aPtr) + RoundUp(eachItemSize - RoundUp(prefixSize + 8) + 4) * count); }
                aPtr := Ptr(ord(aPtr) + BAnd(eachItemSize - BAnd(prefixSize + 8 + RoundUpValue,
                                                                 RoundUpMask) + 4 + RoundUpValue,
                                             RoundUpMask) * count);
                END
            ELSE
                BEGIN
                IF prefixSize <> 0 THEN IF prefixSize <> 4 THEN exit(CheckIsRecord);

                FOR index := count DOWNTO 1 DO
                    BEGIN
                    { prefixSize is either 0 or 4 }
                    { LONGINTPtr(ord(aPtr) + 8 - prefixSize)^ points to the the length of field }
                { 12 - prefixSize is either 12 in the case of key type length or
                                             8 in the case of key length }
                    { so the following skip a field }
    { aPtr := Ptr(ord(aPtr) + RoundUp(LONGINTPtr(ord(aPtr) + 8 - prefixSize)^ + 12 - prefixSize)); }
                    aPtr := Ptr(ord(aPtr) + BAnd(LONGINTPtr(ord(aPtr) + 8 - prefixSize)^ +
                                                 12 + RoundUpValue - prefixSize, RoundUpMask));
                    IF ord(aPtr) > dataEnd THEN exit(CheckIsRecord);
                    END;
                END;
            END;
        IF ord(aPtr) = dataEnd THEN CheckIsRecord := true;
    END;

{--------------------------------------------------------------------------------}

{ take a descriptor and convert its value to an Extended }

FUNCTION DescToExtd(theType: DescType;
                    dataPtr: Ptr;
                    dataSize: LONGINT;
                    VAR precisionDigits: Integer;
                    VAR anExtended: Extended): boolean;

    VAR
        aComp: 				CompOrTwoLong;
        aPtr: 				LONGINTPtr;
        aStr: 				DecStr;
        theNumClass: 		NumClass;
        saneDecimal: 		Decimal;
        validPrefix: 		Boolean;
		conversionResult:	Boolean;
        index: 				Integer;

    BEGIN
        conversionResult := TRUE;
        IF LONGINT(theType) = LONGINT(TypeChar) THEN
            BEGIN
            conversionResult := FALSE;
            IF dataSize <= DecStrLen THEN { not too long for SANE }
                BEGIN
                aStr[0] := chr(dataSize);
                BlockMove(dataPtr, @aStr[1], dataSize);
                index := 1; { we start scanning from 1 }
                Str2Dec(aStr, index, saneDecimal, validPrefix);
                IF validPrefix AND (index > dataSize) THEN
                    BEGIN { it is valid }
                    anExtended := Dec2Num(saneDecimal); { ask SANE to finish the job }
					conversionResult := TRUE;
                    END;
                END;
            END
        ELSE IF (LONGINT(theType) = LONGINT(typeLongInteger)) AND (dataSize = SizeOf(LONGINT)) THEN
            anExtended := LONGINTPtr(dataPtr)^
        ELSE IF (LONGINT(theType) = LONGINT(typeShortInteger)) AND (dataSize = SizeOf(Integer)) THEN
            anExtended := IntegerPtr(dataPtr)^
        ELSE IF (LONGINT(theType) = LONGINT(typeComp)) AND (dataSize = SizeOf(Comp)) THEN
            BEGIN
            anExtended := CompPtr(dataPtr)^;
            precisionDigits := compPrecision;
            END
        ELSE IF (LONGINT(theType) = LONGINT(typeShortFloat)) AND (dataSize = SizeOf(Real)) THEN
            BEGIN
            anExtended := RealPtr(dataPtr)^;
            precisionDigits := singPrecision;
            END
        ELSE IF (LONGINT(theType) = LONGINT(typeLongFloat)) AND (dataSize = SizeOf(Double)) THEN
            BEGIN
            anExtended := DoublePtr(dataPtr)^;
            precisionDigits := doubPrecision;
            END
        ELSE IF (LONGINT(theType) = LONGINT(typeExtended)) AND (dataSize = SizeOf(Extended)) THEN
            BEGIN
            anExtended := ExtendedPtr(dataPtr)^;
            precisionDigits := extePrecision;
            END
        ELSE IF (LONGINT(theType) = LONGINT(typeMagnitude)) AND (dataSize = SizeOf(LONGINT)) THEN
            BEGIN { first make it into comp, then coerce to extend }
            aComp.asLong[0] := 0;
            aComp.asLong[1] := LONGINTPtr(dataPtr)^;
            anExtended := aComp.asComp;
            precisionDigits := magnPrecision;
            END
        ELSE
            conversionResult := FALSE;
		{ valid only if it is zeroNum, NormalNum or DenormalNum }
		IF conversionResult THEN
			conversionResult := (ClassExtended(anExtended) >= ZeroNum);
		DescToExtd := conversionResult;
    END;

{--------------------------------------------------------------------------------}

{ take an extened and convert it to the type toType }

FUNCTION ExtdToNum(VAR x: Extended; { VAR is for efficiency purpose only }
                   toType: DescType;
                   VAR anUnivNum: UnivNum): boolean;

    VAR
        saneEnvironment: Environment;
        canConvert: boolean;
        aComp: CompOrTwoLong;

    BEGIN
        GetEnvironment(saneEnvironment);
        { clear all flags and halt but preserve rounding direction and precision }
        SetEnvironment(BAnd(saneEnvironment, $6060));
        canConvert := TRUE;
        IF LONGINT(toType) = LONGINT(typeShortInteger) THEN
            anUnivNum.asInteger := Num2Integer(x)
        ELSE IF LONGINT(toType) = LONGINT(typeLongInteger) THEN
            anUnivNum.asLongint := Num2Longint(x)
        ELSE IF LONGINT(toType) = LONGINT(typeShortFloat) THEN
            anUnivNum.asReal := Num2Real(x)
        ELSE IF LONGINT(toType) = LONGINT(typeLongFloat) THEN
            anUnivNum.asDouble := Num2Double(x)
        ELSE IF LONGINT(toType) = LONGINT(typeComp) THEN
            anUnivNum.asComp := Num2Comp(x)
        ELSE IF LONGINT(toType) = LONGINT(typeMagnitude) THEN
            BEGIN
            aComp.asComp := Num2Comp(x);
            canConvert := (aComp.asLong[0] = 0);
            anUnivNum.asLongint := aComp.asLong[1];
            END
        ELSE
            canConvert := FALSE;
        ExtdToNum := canConvert AND (NOT TestException(Invalid + Overflow));
        SetEnvironment(saneEnvironment); { restore original state }
    END;

{--------------------------------------------------------------------------------}

{ convert an extended to text, make reasonable assumption about float/fixed
    and number of decimal places }

PROCEDURE ExtdToText(precisionDigits: Integer;
                     VAR anExtended: Extended;
                     VAR aStr: Str255);

    VAR
        form: DecForm;
        aPtr: Ptr;
        saneDecimal: decimal;
        saneEnvironment: Environment;
        integerDigit: integer;

    BEGIN
        GetEnvironment(saneEnvironment);
        { clear all flags and halt but preserve rounding direction and precision }
        SetEnvironment(BAnd(saneEnvironment, $6060));
        WITH form DO
            BEGIN
            style := floatDecimal;
            digits := precisionDigits;
            END;
        { we first convert to immediate form so we can find out number of decimal places needed }
        Num2Dec(form, anExtended, saneDecimal);
        WITH saneDecimal DO
            BEGIN
            { we elminiate trailing zeroes, adjusting exp at the same time }
            aPtr := Ptr(ord(@sig) + length(sig));
            WHILE (aPtr^ = ord('0')) DO
                BEGIN
                exp := exp + 1;
                aPtr := Ptr(ord(aPtr) - 1);
                END;
            { now we know number of significant digits, use it if it is floating point }
            form.digits := ord(aPtr) - ord(@sig);
            { integerDigit is the number of digits in the integer part }
            integerDigit := exp + form.digits;
            IF exp < 0 THEN
                BEGIN { we have some digits after decimal point, so it cannot be too large }
                IF integerDigit >= - 5 THEN
                    BEGIN { but we still need to do floating point if it is less than 1e-6 }
                    form.style := FixedDecimal;
                    form.digits := - exp;
                    END;
                END
            ELSE
                BEGIN { no digit after decimal point, digits = 0 if in fixed point }
                IF integerDigit <= precisionDigits THEN
                    BEGIN { use fixed point only if number is not too large }
                    form.style := FixedDecimal;
                    form.digits := 0;
                    END;
                END;
            END;
        Num2Str(form, anExtended, DecStr(aStr));
        SetEnvironment(saneEnvironment); { restore original state }
    END;

{--------------------------------------------------------------------------------}

{ This is the built in coercion routine }

FUNCTION StdCoercion(fromType: DescType;
                     dataPtr: Ptr;
                     dataSize: LONGINT;
                     toType: DescType;
                     VAR result: AEDesc): OSErr;

    VAR
        err: OSErr;
        anUnivNum: UnivNum;
        canDo: boolean;
        anExtended: Extended;
        rstDataSize: Integer;
        rstDataPtr: Ptr;
        aStr: Str255;
        precisionDigits: Integer;

    BEGIN
        err := errAECoercionFail;
        result.descriptorType := toType;
        IF LONGINT(toType) = LONGINT(fromType) THEN
            err := AE_CreateDesc(toType, dataPtr, dataSize, result) { just create it }
        ELSE IF LONGINT(toType) = LONGINT(typeAEList) THEN
            BEGIN { make a list of one element }
            err := CreateList(NIL, 0, FALSE, result, DescType(0));
            IF err = noErr THEN
                BEGIN
                err := AE_PutPtr(result, 0, fromType, dataPtr, dataSize);
                IF err <> noErr THEN IgnoreOSErr(AE_DisposeDesc(result));
                END;
            END
        ELSE IF LONGINT(toType) = LONGINT(typeAERecord) THEN
			BEGIN
			IF CheckIsRecord(dataPtr, dataSize) THEN
				BEGIN { check to make sure this is really a record }
				err := AE_CreateDesc(toType, dataPtr, dataSize, result);
				IF err = NoErr THEN
					ListHdrHdl(result.dataHandle)^^.listClassHeader.metaCountOrObjType := LONGINT(fromType);
				END;
			END
        ELSE
            BEGIN { we always do PtrToHand for the following coercions }
            canDo := FALSE;
            rstDataPtr := @anUnivNum; { most likely value }
            rstDataSize := 4; { most like value }
            IF (LONGINT(toType) = LONGINT(typeChar)) THEN
                BEGIN
                aStr[0] := chr(0); { empty string }
                IF (LONGINT(fromType)) = LONGINT(typeShortInteger) THEN
                    NumToString(IntegerPtr(dataPtr)^, aStr)
                ELSE IF (LONGINT(fromType)) = LONGINT(typeLongInteger) THEN
                    NumToString(LONGINTPtr(dataPtr)^, aStr)
                ELSE IF DescToExtd(fromType, dataPtr, dataSize, precisionDigits, anExtended) THEN
                    ExtdToText(precisionDigits, anExtended, aStr); { make an extended to text string
                                                                    }
                rstDataSize := length(aStr);
                IF rstDataSize > 0 THEN
                    BEGIN
                    canDo := TRUE;
                    rstDataPtr := @aStr[1];
                    END;
                END
            ELSE IF (LONGINT(toType) = LONGINT(typeBoolean)) THEN
                BEGIN
                IF BooleanType(fromType, dataPtr, anUnivNum.asInteger) THEN { is this a kind of
                       boolean ? }
                    BEGIN
                    canDo := TRUE;
                    rstDataPtr := Ptr(ord(@anUnivNum) + 1);
                    rstDataSize := 1;
                    END;
                END
            ELSE IF LONGINT(toType) = LONGINT(typeShortInteger) THEN
                BEGIN
                rstDataSize := SizeOf(Integer);
                IF LONGINT(fromType) = LONGINT(typeLongInteger) THEN
                    BEGIN { long to short is done often, so we special case it }
                    IF LONGINTPtr(dataPtr)^ >= - (MAXINT + 1) THEN
                        IF LONGINTPtr(dataPtr)^ <= MAXINT THEN
                            BEGIN { it is within range }
                            anUnivNum.asInteger := LONGINTPtr(dataPtr)^;
                            canDo := TRUE;
                            END;
                    END
                ELSE IF BooleanType(fromType, dataPtr, anUnivNum.asInteger) THEN
                    canDo := TRUE { it is a basic boolean type }
                ELSE { convert to extended and back }
                    BEGIN
                    IF DescToExtd(fromType, dataPtr, dataSize, precisionDigits, anExtended) THEN
                        canDo := ExtdToNum(anExtended, toType, anUnivNum)
                    END;
                END
            ELSE IF LONGINT(toType) = LONGINT(typeLongInteger) THEN
                BEGIN
                { rstDataSize := SizeOf(LONGINT); not needed since it is same as 4 }
                IF LONGINT(fromType) = LONGINT(typeMagnitude) THEN
                    BEGIN
                    anUnivNum.asLongint := LONGINTPtr(dataPtr)^;
                    IF anUnivNum.asLongint >= 0 THEN canDo := TRUE;
                    END
                ELSE IF LONGINT(fromType) = LONGINT(typeShortInteger) THEN
                    BEGIN
                    anUnivNum.asLongint := IntegerPtr(dataPtr)^;
                    canDo := TRUE;
                    END
                ELSE { convert to extended and back }
                    BEGIN
                    IF DescToExtd(fromType, dataPtr, dataSize, precisionDigits, anExtended) THEN
                        canDo := ExtdToNum(anExtended, toType, anUnivNum)
                    END;
                END
            ELSE IF (LONGINT(toType) = LONGINT(typeMagnitude)) OR (LONGINT(toType) =
                    LONGINT(typeComp)) OR (LONGINT(toType) = LONGINT(typeShortFloat)) OR
                    (LONGINT(toType) = LONGINT(typeLongFloat)) THEN
                BEGIN
                IF DescToExtd(fromType, dataPtr, dataSize, precisionDigits, anExtended) THEN
                    BEGIN
                    canDo := ExtdToNum(anExtended, toType, anUnivNum);
{ IF LONGINT(toType) = LONGINT(typeMagnitude) THEN rstDataSize := 4 not needed since it is same as 4 }
                    IF LONGINT(toType) = LONGINT(typeComp) THEN
                        rstDataSize := SizeOf(Comp)
                    ELSE IF LONGINT(toType) = LONGINT(typeLongFloat) THEN
                        rstDataSize := SizeOf(Double);
{ IF LONGINT(toType) = LONGINT(typeMagnitude) THEN rstDataSize := SizeOf(LONGINT) not needed since it is same as 4 }
{ IF LONGINT(toType) = LONGINT(typeShortFloat) THEN rstDataSize := SizeOf(Real) not needed since it is same as 4 }
                    END;
                END
            ELSE IF LONGINT(toType) = LONGINT(typeExtended) THEN
                BEGIN
                IF DescToExtd(fromType, dataPtr, dataSize, precisionDigits,
                   anUnivNum.asExtended) THEN
                    BEGIN
                    canDo := TRUE;
                    rstDataSize := SizeOf(Extended);
                    END;
                END
            ELSE IF LONGINT(toType) = LONGINT(typeEnumerated) THEN
                BEGIN
                { rstDataSize := SizeOf(ResType); not needed since it is same as 4 }
                IF BooleanType(fromType, dataPtr, anUnivNum.asInteger) THEN
                    BEGIN
                    canDo := TRUE;
                    IF anUnivNum.asInteger = 0 THEN
                        anUnivNum.asResType := typeFalse
                    ELSE
                        anUnivNum.asResType := typeTrue;
                    END;
                END
            ELSE IF LONGINT(toType) = LONGINT(typeSectionH) THEN
                BEGIN
                IF LONGINT(fromType) = LONGINT('tid ') THEN
                    IF dataSize = 8 THEN { must be 8 byte for sect record }
                        IF LONGINTPtr(dataPtr)^ = LONGINT(typeSectionH) THEN { first 4 byte must be
                               'sect' }
                            BEGIN
                            canDo := TRUE;
                            { rstDataSize := 4; not needed since it is same as 4 }
                            rstDataPtr := Ptr(ORD(dataPtr) + 4);
                            END;
                END;
            IF canDo THEN err := PtrToHand(rstDataPtr, result.dataHandle, rstDataSize);
            END;

        StdCoercion := err;
    END;

{--------------------------------------------------------------------------------}

