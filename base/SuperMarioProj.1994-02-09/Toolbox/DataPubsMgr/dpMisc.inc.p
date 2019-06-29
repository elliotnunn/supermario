{
	File:		dpMisc.inc.p

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<13>	11/26/90	ngk		<DFH> Add warning handlers FailNonWrn and IgnoreWrn.
	   								Moved dp_GetCurrentAppRefNum to dpMisc.a
		<12>	 9/13/90	ngk		Changed file type for edition files to be all lowercase.
		<11>	 7/16/90	RLC		Change GetAppSignature routine to set info.processAppSpec to NIL
									instead of .processFileName.
		<10>	  7/2/90	ngk		Added IsEditionFile.
		 <9>	 5/31/90	ngk		Changed em globals from a handle to a pointer.
		 <8>	  4/7/90	ngk		Add dynamic array. Add new failure handlers.
		 <7>	 3/10/90	ngk		Removed GetStateAndLock and RestoreState. Added dpSameBytes.
		 <6>	  3/4/90	ngk		Fix bug in calling GetProcessInformation with setting up
									processInfoLength field.
		 <5>	 2/25/90	ngk		Use SameProcess in dp_GetCurrentAppRefNum instead of comparing
									bits. Changed GetCurrentAppSignature to GetAppSignature.
		 <4>	  2/4/90	ngk		Removed Desktop database routines
		 <3>	  1/8/90	ngk		renamed dp_GetCurrentAppRefNum
		 <2>	  1/6/90	ngk		Convert to BBS

	To Do:
}
 

	{ the following are actually in dpMisc.a }
	PROCEDURE UnSignedDivide; EXTERNAL;
	PROCEDURE UnSignedMultiply; EXTERNAL;
	FUNCTION  dpGetGlobalsLocation; EXTERNAL;
	PROCEDURE dpSetCurrentAppRefNum(app: AppRefNum); EXTERNAL;
	FUNCTION  dp_GetCurrentAppRefNum(VAR app: AppRefNum): OSErr; EXTERNAL;
	FUNCTION  dp_CallEditionOpenerProc;  EXTERNAL;
	FUNCTION  dp_CallFormatIOProc;  EXTERNAL;
	PROCEDURE FailOSErr; EXTERNAL;
	FUNCTION  IsFailure; EXTERNAL;
								


	{------------- GetAppSignature -------------}
	FUNCTION GetAppSignature(PSN: ProcessSerialNumber; VAR signature: OSType): OSErr;
	VAR 
		info: ProcessInfoRec;
	BEGIN
		info.processInfoLength := SizeOf(info);
		info.processName := NIL;
		info.processAppSpec	:= NIL;		{info.processFileName := NIL;  This was the old way…}
		GetAppSignature := GetProcessInformation(PSN, info);
		signature := info.processSignature;
	END; { GetAppSignature }
	



	{-------------  RemoveAppsGlobals -------------}
	PROCEDURE RemoveAppsGlobals(whichApp: AppRefNum); { called only by dp_QuitEditionPack }
	VAR
		prevAppGlobal:	PerAppGlobalsHandle;
		nextAppGlobal:	PerAppGlobalsHandle;
		globals:		EdtnMgrGlobalsPtr;
	BEGIN
		globals := dpGetGlobalsLocation^;
		IF globals = NIL THEN EXIT(RemoveAppsGlobals);
		
		nextAppGlobal := globals^.perAppListHead;
		IF nextAppGlobal = NIL THEN EXIT(RemoveAppsGlobals);
		
		IF nextAppGlobal = whichApp THEN
		BEGIN
			{ is first in list }
			dpGetGlobalsLocation^^.perAppListHead := nextAppGlobal^^.nextApp;
		END ELSE
		BEGIN
			{ is not first in list }
			REPEAT
				prevAppGlobal := nextAppGlobal;
				nextAppGlobal := prevAppGlobal^^.nextApp;
				IF nextAppGlobal = NIL THEN 
				BEGIN
					{ cant find??? }
					{$IFC qCatchFailures }
					DebugStr('RemoveAppsGlobals: cannot find app');
					{$ENDC }
					EXIT(RemoveAppsGlobals); 
				END;
			UNTIL (nextAppGlobal = whichApp);
			
			{ remove whichApp's globals from list }
			prevAppGlobal^^.nextApp := nextAppGlobal^^.nextApp;
		END;
		
		{ clear switched expandmem global }
		dpSetCurrentAppRefNum(NIL);
		
		{ actually dispose of app's global memory }
		DisposHandle(Handle(whichApp));
		
	END; { RemoveAppsGlobals }
		
	
	
	{------------- dpSameBytes -------------}
	FUNCTION dpSameBytes(aPtr, bPtr: UNIV Ptr; byteCount: INTEGER): BOOLEAN;
	TYPE
		BytePtr = ^SignedByte;
	VAR
		i: INTEGER;
	BEGIN
		dpSameBytes := FALSE;
		FOR i := (byteCount - 1) DOWNTO 0 DO
		BEGIN
			IF BytePtr(aPtr)^ <> BytePtr(bPtr)^
				THEN EXIT(dpSameBytes);
			aPtr := POINTER(ORD(aPtr) + 1);
			bPtr := POINTER(ORD(bPtr) + 1);
		END;
		dpSameBytes := TRUE;
	END; { dpSameBytes }
	

	
	


	{------------- dpNewDynamicArray -------------} 
	FUNCTION dpNewDynamicArray(headerSize: Size; slotSize, initialSlots: INTEGER;
								FUNCTION MyNewHandle(amount: Size): Handle;
								VAR arrayH: DyanmicArrayHandle): OSErr;
	VAR
		initialSlotDataSize:	Size;
		arrayP:					DyanmicArrayPtr;
	BEGIN
		UnSignedMultiply(slotSize, initialSlots, initialSlotDataSize);
		arrayH := DyanmicArrayHandle(MyNewHandle(initialSlotDataSize + headerSize + SizeOf(DyanmicArray)) );
		IF MemError = noErr THEN
		BEGIN
			arrayP := DyanmicArrayPtr( ORD(arrayH^) + headerSize );
			arrayP^.slotSize 		:= slotSize;
			arrayP^.maxSlots 		:= initialSlots;
			arrayP^.lastUsedSlot 	:= 0;
			dpNewDynamicArray := noErr;
		END ELSE 
		BEGIN
			dpNewDynamicArray := MemError;
		END;
	END; { dpNewDynamicArray }
	
	
	
	{------------- dpFindElement -------------} 
	FUNCTION dpFindElement(arrayH: DyanmicArrayHandle; headerSize: Size;
							FUNCTION Matches(anEntryIndex: INTEGER; anEntryPtr: UNIV Ptr): BOOLEAN;
							VAR foundIndex: INTEGER; VAR foundPtr: UNIV Ptr): BOOLEAN;
	VAR
		slotSize:		INTEGER;
		elemPtr:		Ptr;
		lastUsedSlot:	INTEGER;
		i:				INTEGER;
	BEGIN
		elemPtr := POINTER( ORD(arrayH^) + headerSize );
		lastUsedSlot := DyanmicArrayPtr(elemPtr)^.lastUsedSlot;
		slotSize := DyanmicArrayPtr(elemPtr)^.slotSize;
		
		elemPtr := POINTER( ORD(arrayH^) + headerSize + SizeOf(DyanmicArray) );
		FOR i :=  1 TO lastUsedSlot DO
		BEGIN
			IF Matches(i, elemPtr) THEN 
			BEGIN
				foundIndex	:= i;
				foundPtr	:= elemPtr;
				dpFindElement := TRUE;
				EXIT(dpFindElement);
			END; {if}
			elemPtr := POINTER( ORD(elemPtr) + slotSize );
		END; {for}
		
		dpFindElement := FALSE;
	END; { dpFindElement }
	
	
	
	{------------- dpGetDynamicArrayEntry -------------} 
	FUNCTION dpGetDynamicArrayEntry(arrayH: DyanmicArrayHandle; headerSize: Size; 
									entryIndex: INTEGER; VAR entryPtr: UNIV Ptr): OSErr;
	VAR
		arrayP: 		DyanmicArrayPtr;
		newSlotOffset:	Size;
	BEGIN
		arrayP := DyanmicArrayPtr( ORD(arrayH^) + headerSize );
		
		UnSignedMultiply(arrayP^.slotSize, entryIndex-1, newSlotOffset);
		entryPtr := Ptr( ORD(arrayP) + SizeOf(DyanmicArray) + newSlotOffset );
		
		dpGetDynamicArrayEntry := noErr;
	END; { dpGetDynamicArrayEntry }



	{------------- dpAddDynamicArrayEntry -------------} 
	FUNCTION dpAddDynamicArrayEntry(arrayH: DyanmicArrayHandle; headerSize: Size; 
									VAR entryIndex: INTEGER; VAR entryPtr: UNIV Ptr): OSErr;
	VAR
		slotToUse:		INTEGER;		
		maxSlots:		INTEGER;
		slotSize:		INTEGER;
		dummy:			INTEGER;
		arrayP:			DyanmicArrayPtr;
		newSlotDataSize:Size;
		newSlotOffset:	Size;
	BEGIN
		arrayP := DyanmicArrayPtr( ORD(arrayH^) + headerSize );
		slotSize := arrayP^.slotSize;
		slotToUse := arrayP^.lastUsedSlot + 1;
		maxSlots := arrayP^.maxSlots; 
		IF slotToUse > maxSlots THEN 
		BEGIN
			{$IFC qRangeCheck }
			{ find how many slots there currently is }
			UnSignedDivide(GetHandleSize(Handle(arrayH)) - SizeOf(DyanmicArray) - headerSize,	{numer}
					arrayP^.slotSize,															{denom}
					maxSlots,																	{quotient}
					dummy);																		{remainder}
			IF dummy <> 0 THEN DebugStr('dpAddDynamicArrayEntry: bad dynamic record size');
			{$ENDC }
			maxSlots := 2 * maxSlots;	{ double the number of slots }
			UnSignedMultiply(slotSize, maxSlots, newSlotDataSize);
			SetHandleSize(Handle(arrayH), headerSize + SizeOf(DyanmicArray) + newSlotDataSize);
			IF MemError <> noErr THEN
			BEGIN
				dpAddDynamicArrayEntry := MemError;
				EXIT(dpAddDynamicArrayEntry);
			END; {if}
			{ re-derefence handle }
			arrayP := DyanmicArrayPtr( ORD(arrayH^) + headerSize );
			arrayP^.maxSlots 	:= maxSlots;
		END; {if}
		
		entryIndex := slotToUse;
		arrayP^.lastUsedSlot:= slotToUse;
		
		UnSignedMultiply(slotSize, slotToUse-1, newSlotOffset);
		entryPtr := Ptr( ORD(arrayP) + SizeOf(DyanmicArray) + newSlotOffset );
		
		dpAddDynamicArrayEntry := noErr;
	END; { dpAddDynamicArrayEntry }
	
		
	
	{------------- FailNonWrn -------------} 
	FUNCTION FailNonWrn(anErr: OSErr): OSErr;
	BEGIN
		IgnoreWrn(anErr);
		FailNonWrn := anErr;
	END; { FailNonWrn }


	{------------- IgnoreWrn -------------} 
	PROCEDURE IgnoreWrn(anErr: OSErr);
	BEGIN
		IF 			 (anErr <> noErr) 
		 {&} THEN IF (anErr <> containerNotFoundWrn)
		 {&} THEN IF (anErr <> multiplePublisherWrn)
		 {&} THEN IF (anErr <> notThePublisherWrn)
		 {&} THEN IF (anErr <> userCanceledErr)
			 THEN FailOSErr(anErr);
	END; { IgnoreWrn }


	{------------- Success -------------} 
	PROCEDURE Success(VAR info: FailInfo);
	BEGIN
		{$IFC qRangeCheck}
		IF dpGetGlobalsLocation^^.failureHandler <> @info 
			THEN DebugStr('Success: wrong FailInfo record');
		{$ENDC}
		dpGetGlobalsLocation^^.failureHandler := info.nextInfo;
	END; { Success }
	
	
		
	{-------------  IsEditionFile -------------}
	FUNCTION IsEditionFile(fdType: OSType): BOOLEAN;
	BEGIN
		IsEditionFile := TRUE;
		IF LONGINT(fdType) <> LONGINT(kPICTEditionFileType) 
		{&} THEN IF LONGINT(fdType) <> LONGINT(kTEXTEditionFileType) 
		{&} THEN IF LONGINT(fdType) <> LONGINT(ksndEditionFileType) 
		{&} THEN IF LONGINT(fdType) <> LONGINT(kUnknownEditionFileType) 
		{&} THEN IF LONGINT(fdType) <> LONGINT('edtP') { grandfather in this type }
		{&} THEN IF LONGINT(fdType) <> LONGINT('edtT') { grandfather in this type }
		{&} THEN IF LONGINT(fdType) <> LONGINT('publ') { grandfather in this type }
			THEN IsEditionFile := FALSE;
	END; { IsEditionFile }
	
	
	