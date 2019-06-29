{
	File:		dpSectionIO.inc.p

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<13>	12/14/90	ngk		<RC>Use new PBxxSync calls. Stop subscribers from writing.
		<12>	 7/30/90	ngk		fix zero length read of zero length buffer.
		<11>	  7/6/90	ngk		Fixed bug in dp_ReadOrWrite, where current mark not being
									updated.
		<10>	  7/2/90	ngk		Fixed dpStandardReadFormat to return buffLen=0 if nothing read
									and return eofErr if trying to read from end of format.
		 <9>	 5/31/90	ngk		Check for valid mark in ReadEdition. Combine ReadEdition and
									WriteEdition into one routine. Set default mark to be 0 instead
									of -1. Combine Add and FindFormat into GetFormat. Using more
									WITH's more code savings.
		 <8>	  4/7/90	ngk		Make number of formats growable. Use dynamic arrays for
									SIOCBRecord, format list, and allocation map. Use new failure
									handling.
		 <7>	 3/10/90	ngk		Moved locking and unlocking of package to dispatcher.
		 <6>	  2/4/90	ngk		Fix dpStandardReadFormat to correctly reading 'fmts' and return
									length read for all formats.
		 <5>	 1/12/90	ngk		Fixed bug where mark not being set first time
		 <4>	  1/8/90	ngk		add fmts
		<2+>	  1/7/90	ngk		special 'fmts' in dpStandardLookUpFormat and dpStandardReadFormat
		 <2>	  1/6/90	ngk		Converted to BBS. Added FormatIO bottle necks. Moved StandardIO
									routines into this file.
	   <1.6>	11/13/89	ngk		Added Locking and restoring of Pack11 in all public routines
	   <1.5>	 10/2/89	ngk		Updated to new API
	   <1.4>	 8/29/89	ngk		Renamed XetEditionPos to XetEditionMark. Used new IO stategy.
	   <1.3>	  8/8/89	ngk		Fixed bug in GetEditionMark not returning dpBadPosErr. Used new
									error codes. qDebug -> qCatchFailures
	   <1.2>	 6/11/89	ngk		Added dpHasEditionFormat and more debugging checks
	   <1.1>	 5/29/89	ngk		Change ReadEdition to call ReadFormat and WriteEdition to call
									WriteFormat
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}

	
{============================= low level =============================}
	
	
	{-------------  dpCreateSIOCBRecord -------------}
	FUNCTION dpCreateSIOCBRecord(itsSectionH:	SectionHandle; 
								 itsRefNum:		LONGINT;
								 itsProc:		FormatIOProcPtr;
								 VAR sectCB: 	SIOCBHandle): OSErr; 
	VAR
		i:	INTEGER;
		fi:	FailInfo;		
		
	BEGIN
		{ sectCB is never in a register because it is a VAR parameter }
		sectCB := NIL;
		
		{ set up failure handling }
		IF IsFailure(fi, dpCreateSIOCBRecord) THEN
		BEGIN
			IF sectCB <> NIL 
				THEN DisposHandle(Handle(sectCB));
			sectCB := NIL;
			EXIT(dpCreateSIOCBRecord);
		END; {if}
		
		{$IFC qRangeCheck }
		FailOSErr(dpCheckSection(itsSectionH));
		{$ENDC }
		
		{ allocate Section I/O Control block }
		FailOSErr(dpNewDynamicArray(SizeOf(SIOCBRecord), 			{headerSize}
									SizeOf(FormatPositionInfo),		{slotSize}
									kInitialFormats,				{initialSlots}
									NewHandle,						{MyNewHandle} { in app heap }
									DyanmicArrayHandle(sectCB)));	{VAR arrayH}
	
		{ initialize it }
		WITH sectCB^^ DO
		BEGIN
			section :=	itsSectionH;
			ioRefNum :=	itsRefNum;
			ioProc :=	itsProc;
		END;
		
		{ set sectionH to point to it }
		SIOCBHandle(itsSectionH^^.refNum) := sectCB;

		Success(fi);
	END; { dpCreateSIOCBRecord }
	

	{-------------  dpGetFormat -------------}
	{ returns an index and pointer into the sectCB FormatPositionInfo array }
	FUNCTION dpGetFormat(sectCB: SIOCBHandle; whichFormat: FormatType; canCreate: BOOLEAN;
							VAR SIOCBIndex: INTEGER; VAR SIOCBPtr: FormatPositionInfoPtr): OSErr; 
	VAR
		selector:	FormatIOVerb;
		params:		FormatIOParamBlock;
		
		FUNCTION MatchFormat(index: INTEGER; elemPtr: FormatPositionInfoPtr): BOOLEAN;
		BEGIN
			MatchFormat :=  LONGINT(elemPtr^.format) = LONGINT(whichFormat);
		END; { MatchFormat }
		
	BEGIN
		{ if I can't find format in sectCB list, then try to create it }
		IF NOT dpFindElement(DyanmicArrayHandle(sectCB), SizeOf(SIOCBRecord), MatchFormat, SIOCBIndex, SIOCBPtr) THEN
		BEGIN
			{ need to ask I/O proc to make it }
			params.ioRefNum := sectCB^^.ioRefNum;
			params.format := whichFormat;
			IF canCreate 
				THEN selector := ioNewFormat
				ELSE selector := ioHasFormat;
			FailOSErr(dp_CallFormatIOProc(selector, params, sectCB^^.ioProc));
			
			{ and add it to my list of known types }
			FailOSErr(dpAddDynamicArrayEntry(DyanmicArrayHandle(sectCB), SizeOf(SIOCBRecord), SIOCBIndex, SIOCBPtr));
			WITH SIOCBPtr^ DO
			BEGIN
				format	:= whichFormat;
				mark	:= 0;
				index	:= params.formatIndex;
				length	:= params.buffLen;
			END;
		END; {if}
		dpGetFormat := noErr;
	END; { dpGetFormat }

	
	
{=============================== standard routines ==================================}
	
	
	{------------- dpStandardLookUpFormat -------------} 
	FUNCTION dpStandardLookUpFormat(thePubCB: PubCBHandle; whichFormat: FormatType; 
							VAR mapIndex: LONGINT; VAR formatSize: Size): OSErr;
	VAR
		foundPtr:	FormatPtr;
		foundAt:	INTEGER;
		
		FUNCTION MatchFormat(index: INTEGER; elemPtr: FormatPtr): BOOLEAN;
		BEGIN
			MatchFormat := LONGINT(elemPtr^.theType) = LONGINT(whichFormat);
		END; { MatchFormat }
		
	BEGIN
		dpStandardLookUpFormat := noErr;
		{ special case 'fmts' }
		IF LONGINT(whichFormat) = LONGINT(kFormatListFormat) THEN
		BEGIN
			mapIndex := 0;
			formatSize := thePubCB^^.formats^^.lastUsedSlot * SizeOf(Format);
		END ELSE
		BEGIN
			IF dpFindElement(thePubCB^^.formats, 0, MatchFormat, foundAt, foundPtr) THEN
			BEGIN
				mapIndex := foundAt;
				formatSize := foundPtr^.theLength;
			END ELSE
			BEGIN
				dpStandardLookUpFormat := noTypeErr;
			END;
		END; {if}
	END; { dpStandardLookUpFormat }
	
	
	

	{------------- dpStandardCreateNewFormat -------------} 
	FUNCTION dpStandardCreateNewFormat(thePubCB: PubCBHandle; newFormat: FormatType;
									VAR mapIndex: LONGINT): OSErr;
	VAR
		newElementPtr:	FormatPtr;
		addErr:			OSErr;
		index:			INTEGER;
	BEGIN
		addErr := dpAddDynamicArrayEntry(thePubCB^^.formats, 0, index, newElementPtr); 
		IF addErr = noErr THEN
		BEGIN
			mapIndex := index;
			WITH newElementPtr^ DO
			BEGIN
				theType 	:= newFormat;
				theLength	:= 0;
			END; {with}
		END;
		dpStandardCreateNewFormat := addErr;
	END; { dpStandardCreateNewFormat }


	
	{------------- dpStandardWriteFormat -------------}
	FUNCTION dpStandardWriteFormat(thePubCB: PubCBHandle; buffPtr: Ptr; buffLen: Size; 
						whichFormat: FormatType; mapIndex: LONGINT; offset: LONGINT): OSErr;
	VAR
		theMap:		AllocationMapHandle;
		theRefNum:	INTEGER;
		PB:			ParamBlockRec;
		canReUse:	BOOLEAN;
		fPtr:		FormatPtr;
		alPtr:		AllocationRecordPtr;
		aSlot:		INTEGER;
				
	BEGIN 
		{ use PubCB to get theMap & theRefNum }
		WITH thePubCB^^ DO
		BEGIN
			theMap 		:= {thePubCB^^.}allocMap;
			theRefNum	:= {thePubCB^^.}fileRefNum;
		END;
		
		{ write data to end of file, then update the map }
		IF buffLen <> 0 THEN
		BEGIN
			WITH PB DO
			BEGIN
				ioRefNum	:= theRefNum;
				ioBuffer	:= buffPtr;
				ioReqCount	:= buffLen;
				ioPosMode	:= fsFromStart;
				ioPosOffset	:= thePubCB^^.fileMark;
				FailOSErr(PBWriteSync(@PB));
				IF ioActCount <> buffLen THEN FailOSErr(dskFulErr); {when would this ever happen?}
			END; {with}
		END; {if}
		
		{ since when writing, we appended to the end of the file,  }
		{ we can reuse the last allocation record if the app has been appending a single format }
		aSlot := theMap^^.lastUsedSlot;
		IF aSlot > 0 THEN
		BEGIN
			FailOSErr(dpGetDynamicArrayEntry(theMap, 0, aSlot, alPtr));
			WITH alPtr^ DO
			BEGIN
				canReUse := (LONGINT(theType) = LONGINT(whichFormat)) & (logicalStart+logicalLen = offset);
			END; {with}
		END
		ELSE canReUse := FALSE;
		
		IF canReUse THEN
		BEGIN
			{ can reuse last allocation record, because app is appending }
			WITH alPtr^ DO
			BEGIN
				logicalLen := logicalLen + buffLen;
			END; {with}
		END ELSE
		BEGIN
			{ can't reuse one, so make new allocation entry, using current file pos }
			FailOSErr(dpAddDynamicArrayEntry(theMap, 0, aSlot, alPtr));
			WITH alPtr^ DO
			BEGIN
				theType			:= whichFormat;
				logicalStart 	:= offset;			{ new allocation begins at clients current mark }
				logicalLen 		:= buffLen;		
				fileOffset		:= thePubCB^^.fileMark;		{ put it at end of file }
			END; {with}
		END; {if}		
		
		FailOSErr(dpGetDynamicArrayEntry(thePubCB^^.formats, 0, mapIndex, fPtr));
		WITH fPtr^ DO
		BEGIN
			{ if increased the length of a format, then bump format length }
			IF theLength < offset+buffLen THEN
			BEGIN
				theLength :=  offset+buffLen;
			END;
		END; {with}
		
		WITH thePubCB^^ DO
		BEGIN
			fileMark := fileMark + buffLen;
		END;
				
		dpStandardWriteFormat := noErr;
	END; { dpStandardWriteFormat }
	
	
	{-------------  Intersection -------------} 
	{ compares two ranges and returns their intersection }
	{ the ranges are specified by start and length }
	{ use by ReadEdition when walking the allocation table }
	FUNCTION Intersection(R1start,R1len, R2start,R2len: LONGINT; 
							VAR Istart,Ilen: LONGINT): BOOLEAN;
	BEGIN
		Intersection := FALSE;
		Ilen := 0;
		IF (R1start <= R2start+R2len) & (R1start+R1len > R2start) THEN
		BEGIN
			IF R1start > R2start 
				THEN Istart := R1start
				ELSE Istart := R2start;
			IF R1start+R1len > R2start+R2len 
				THEN Ilen := R2start+R2len-Istart
				ELSE Ilen := R1start+R1len-Istart;
			Intersection := TRUE;
		END;
	END; { Intersection }
	
	
	{------------- dpStandardReadFormat -------------} 
	FUNCTION dpStandardReadFormat(thePubCB: PubCBHandle; buffPtr: Ptr; VAR buffLen: Size; 
								whichFormat: FormatType; mapIndex: LONGINT; 
								offsetIntoFormat: LONGINT): OSErr;

	VAR
		fmtsStart:				LONGINT;
		fmtsLen:				LONGINT;
		fmtsEnd:				LONGINT;
		maxOffsetInfoFormat:	LONGINT;
		theRefNum:				INTEGER;
		ignoreIndex:			INTEGER;
		ignorePtr:				Ptr;
		formatEntryPtr:			FormatPtr;
				
		FUNCTION ReadIfIntersects(allocIndex: INTEGER; allocPtr: AllocationRecordPtr): BOOLEAN;
		VAR
			interStart:	LONGINT;
			interLen:	LONGINT;
			PB:			ParamBlockRec;
		BEGIN
			WITH allocPtr^ DO
			BEGIN
				IF (LONGINT(theType) = LONGINT(whichFormat)) THEN
					IF Intersection(logicalStart,logicalLen,  offsetIntoFormat,buffLen,  interStart,interLen) THEN
					WITH PB DO
					BEGIN { if it contains data to be read }
						ioRefNum	:= theRefNum;
						ioBuffer	:= Pointer(ORD(buffPtr)+interStart-offsetIntoFormat);
						ioReqCount	:= interLen;
						ioPosMode	:= fsFromStart;
						ioPosOffset	:= interStart+fileOffset-logicalStart;
						FailOSErr(PBReadSync(@PB));
						IF ioActCount <> interLen THEN FailOSErr(eofErr); {is this redundant?}
						{ save off how far we have touched in format }
						IF interStart+interLen > maxOffsetInfoFormat 
							THEN maxOffsetInfoFormat := interStart+interLen;
					END; {with}
			END; {with}
			ReadIfIntersects := FALSE;
		END; { ReadIfIntersects }
	
	BEGIN 
		IF LONGINT(whichFormat) = LONGINT(kFormatListFormat) THEN
		BEGIN
			{ special case 'fmts' }
			WITH thePubCB^^ DO
			BEGIN
				fmtsEnd := formats^^.lastUsedSlot * SizeOf(Format);
				IF offsetIntoFormat > fmtsEnd  { ### should be an unsigned comparison }
					THEN FailOSErr(eofErr);
				IF offsetIntoFormat = fmtsEnd  {&} THEN IF buffLen <> 0 { only OK to read nothing }
					 THEN FailOSErr(eofErr);
				IF Intersection(0,fmtsEnd, offsetIntoFormat,buffLen, fmtsStart,fmtsLen) 
					THEN BlockMove(Ptr(ORD(formats^)+SizeOf(DyanmicArray)+fmtsStart), buffPtr, fmtsLen);
			END; {with}
			buffLen := fmtsLen;
		END ELSE
		BEGIN
			{ check if trying to read out of past end of format }
			FailOSErr(dpGetDynamicArrayEntry(thePubCB^^.formats, 0, mapIndex, formatEntryPtr));
			
			IF offsetIntoFormat > formatEntryPtr^.theLength  { ### should be an unsigned comparison }
				 THEN FailOSErr(eofErr);

			IF offsetIntoFormat = formatEntryPtr^.theLength  {&} THEN IF buffLen <> 0 { only OK to read nothing }
				 THEN FailOSErr(eofErr);

			{ use PubCB to get theMap & theRefNum }
			theRefNum	:= thePubCB^^.fileRefNum;
			
			{ read stuff if requested }
			IF buffLen <> 0 THEN 
			BEGIN
				{ for each intersection of table block and read range, copy to buffer }
				maxOffsetInfoFormat := offsetIntoFormat;
				IF dpFindElement(thePubCB^^.allocMap, 0, ReadIfIntersects, ignoreIndex, ignorePtr) THEN;
			
				{ return amount read in buffLen }
				buffLen := maxOffsetInfoFormat - offsetIntoFormat;
			END;
		END;
		
		dpStandardReadFormat := noErr;
	END; { dpStandardReadFormat }
	


	{------------- dpStandardIO -------------}
	FUNCTION dpStandardIO(selector: FormatIOVerb; VAR PB: FormatIOParamBlock): OSErr; 
	VAR
		thePubCB:	PubCBHandle;
		fi:			FailInfo;
	BEGIN
		{ set up failure handling }
		IF IsFailure(fi, dpStandardIO) 
			THEN EXIT(dpStandardIO);

		WITH PB DO
		BEGIN
			thePubCB := PubCBHandle(ioRefNum);
			CASE selector OF
				ioHasFormat:	
					BEGIN
						dpStandardIO := dpStandardLookUpFormat(thePubCB, format, formatIndex, buffLen);
					END;
				ioReadFormat:
					BEGIN
						dpStandardIO := dpStandardReadFormat(thePubCB, buffPtr, buffLen, format, formatIndex, offset);
					END;
				ioNewFormat:
					BEGIN
						dpStandardIO := dpStandardCreateNewFormat(thePubCB, format, formatIndex);
					END;
				ioWriteFormat:
					BEGIN
						dpStandardIO := dpStandardWriteFormat(thePubCB, buffPtr, buffLen, format, formatIndex, offset);
					END;
			END; {case}
		END; {with}
		
		Success(fi);
	END; { dpStandardIO }
	


{=============================== public routines ==================================}


	{------------- dp_EditionHasFormat -------------}
	FUNCTION dp_EditionHasFormat(sectCB: SIOCBHandle; whichFormat: FormatType; 
								VAR length: Size): OSErr; 
	VAR
		SIOCBIndex: 	INTEGER;
		SIOCBPtr:		FormatPositionInfoPtr;
		fi:				FailInfo;
	BEGIN		
		{ set up failure handling }
		IF IsFailure(fi, dp_EditionHasFormat) 
			THEN EXIT(dp_EditionHasFormat);

		{ get format position in SectCB array }
		FailOSErr(dpGetFormat(sectCB, whichFormat, {canCreate}FALSE, SIOCBIndex, SIOCBPtr));
		
		{ return length of the format }
		length := SIOCBPtr^.length;

		{ remove failure handling }
		Success(fi);
	END; { dp_EditionHasFormat }
	


	{------------- dp_GetEditionFormatMark -------------}
	FUNCTION dp_GetEditionFormatMark(sectCB: SIOCBHandle; whichFormat: FormatType; 
									VAR mark: LONGINT): OSErr; 
	VAR
		SIOCBIndex:		INTEGER;
		SIOCBPtr:		FormatPositionInfoPtr;
		fi:				FailInfo;
	BEGIN
		{ set up failure handling }
		IF IsFailure(fi, dp_GetEditionFormatMark) 
			THEN EXIT(dp_GetEditionFormatMark);

		{ get format position in SectCB array }
		FailOSErr(dpGetFormat(sectCB, whichFormat, {canCreate}FALSE, SIOCBIndex, SIOCBPtr));

		{ yes, I do. So return mark I know }
		mark := SIOCBPtr^.mark;
				
		{ remove failure handling }
		Success(fi);
	END; { dp_GetEditionFormatMark }
	
	
	
	{------------- dp_SetEditionFormatMark -------------}
	FUNCTION dp_SetEditionFormatMark(sectCB: SIOCBHandle; whichFormat: FormatType; 
									mark: LONGINT): OSErr; 
	VAR
		SIOCBIndex:		INTEGER;
		SIOCBPtr:		FormatPositionInfoPtr;
		sectionH:		SectionHandle;
		canCreateFormat:BOOLEAN;
		fi:				FailInfo;
	BEGIN
		{ set up failure handling }
		IF IsFailure(fi, dp_SetEditionFormatMark) 
			THEN EXIT(dp_SetEditionFormatMark);
		
		{ figure out if this could cause the creation of a new format }
		sectionH := sectCB^^.section;
		canCreateFormat := (sectionH <> NIL) & bTST(sectionH^^.kind, kCanWriteEditionsBit);

		{ get format position in SectCB array }
		FailOSErr(dpGetFormat(sectCB, whichFormat, canCreateFormat, SIOCBIndex, SIOCBPtr));

		{ set to new mark }
		SIOCBPtr^.mark := mark;
		
		{ remove failure handling }
		Success(fi);
	END; { dp_SetEditionFormatMark }




	{------------- dp_ReadOrWriteEdition -------------} 
	FUNCTION dp_ReadOrWriteEdition(selector: FormatIOVerb; sectCB: SIOCBHandle; whichFormat: FormatType;
								buffPtr: Ptr; VAR buffLen: Size): OSErr;
	VAR
		params:			FormatIOParamBlock;	
		SIOCBIndex:		INTEGER;
		SIOCBPtr:		FormatPositionInfoPtr;
		fi:				FailInfo;
	BEGIN
		{ set up failure handling }
		IF IsFailure(fi, dp_ReadOrWriteEdition) 
			THEN EXIT(dp_ReadOrWriteEdition);

		{ get format position in SectCB array }
		FailOSErr(dpGetFormat(sectCB, whichFormat, {canCreate}selector=ioWriteFormat, SIOCBIndex, SIOCBPtr));

		WITH SIOCBPtr^, sectCB^^ DO
		BEGIN
			{ set up call to io routine }
			params.ioRefNum 	:= {sectCB^^.}ioRefNum;
			params.format 		:= whichFormat;
			params.formatIndex	:= {SIOCBPtr^.}index;
			params.offset 		:= {SIOCBPtr^.}mark;
			params.buffPtr 		:= buffPtr;
			params.buffLen 		:= buffLen;
			FailOSErr(dp_CallFormatIOProc(selector, params, {sectCB^^.}ioProc));
		END;
		
		{ return amount read or written }
		buffLen := params.buffLen;
		
		{ advance marker, read may have moved memory so recalc pointer }
		FailOSErr(dpGetDynamicArrayEntry(DyanmicArrayHandle(sectCB), SizeOf(SIOCBRecord), SIOCBIndex, SIOCBPtr));
		WITH SIOCBPtr^ DO
			mark := mark + params.buffLen;
				
		Success(fi);
	END; { dp_ReadOrWriteEdition }
	
	
	
	{------------- dp_ReadEdition -------------} 
	FUNCTION dp_ReadEdition(sectCB: SIOCBHandle; whichFormat: FormatType;
							buffPtr: Ptr; VAR buffLen: Size): OSErr;
	BEGIN
		dp_ReadEdition := dp_ReadOrWriteEdition(ioReadFormat, sectCB, whichFormat, buffPtr, buffLen);
	END; { dp_ReadEdition }
	
	
	
	{------------- dp_WriteEdition -------------} 
	FUNCTION dp_WriteEdition(sectCB: SIOCBHandle; whichFormat: FormatType;
							buffPtr: Ptr; buffLen: Size): OSErr;
	VAR
		sectionH:	SectionHandle;
	BEGIN
		{ if this is a subscriber trying to write, then return error }
		sectionH := sectCB^^.section;
		IF (sectionH <> NIL) & (NOT bTST(sectionH^^.kind, kCanWriteEditionsBit))
			THEN dp_WriteEdition := wrPermErr
			ELSE dp_WriteEdition := dp_ReadOrWriteEdition(ioWriteFormat, sectCB, whichFormat, buffPtr, buffLen);
	END; { dp_WriteEdition }
	
	



