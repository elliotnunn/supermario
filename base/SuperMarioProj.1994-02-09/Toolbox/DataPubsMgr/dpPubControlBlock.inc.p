{
	File:		dpPubControlBlock.inc.p

	Written by:	Nick Kledzik
	
	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<13>	 2/11/91	ngk		KSM,#B4-MTM-017: Fix initial edition folder to not be prefernece
									folder
		<12>	 1/14/91	ngk		<MM> fix GetLast… case if file is missing.
		<11>	12/14/90	ngk		<VL>In dpCreatePubCB it is now OK if dpPubSync returns fnfErr.
	   								Update SaveOffContainer to use new FspXXX calls and handle
									case of corrupted preference file.  Simplified
									RetreivedLastContainer by using ResolveAliasFile.
		<10>	  8/2/90	ngk		Use DoNotPutInRegister
		 <9>	  7/2/90	ngk		New fields in PubCB. Call EqualString instead of SameBytes when
									comparing files names to see if we already have a control block.
		 <8>	 6/20/90	ngk		Changed CanonicalFileSpec to FSSpec.
		 <7>	 5/31/90	ngk		Moved saving and restoring of last edition used to here. Now
									save alias in preference folder instead of system. When
									disposing of pubCB check if it is referenced by lastEditionUsed.
									Change em globals to a pointer.
		 <6>	  4/7/90	ngk		Now dynamic arrays for app usage info. Use new failure handling.
		 <5>	 3/10/90	ngk		Change VolumeServices to VolumeAttributes. Changed RequestPubCB
									to use dpSameBytes to compare file names intead of
									IUEqualString.
		 <4>	  1/8/90	ngk		fixes
		<2+>	  1/6/90	ngk		Fixed bug in dpRemoveAppsPubCBs
		 <2>	  1/6/90	ngk		Convert to BBS. Fixed bug in dpRemoveAppsPubCBs where edition
									files were not being closed. Renamed routines
	   <1.9>	 11/4/89	ngk		Changed AppRefNum to be a handle to app's globals.
	  <•1.8>	 10/2/89	ngk		Change PubSpec to EditionContainerSpec
	   <1.7>	 9/18/89	ngk		Changed FileSpec to FSSpec
	   <1.6>	  9/7/89	ngk		Added lastVolMod and lastDirMod to support way cool polling
	   <1.5>	 8/29/89	ngk		Fixed bug when using > 2 apps. Change openCount to IOCount
	   <1.4>	  8/8/89	ngk		Now use PubCBLinks for better list managment. qDebug ->
									qRangeCheck shared -> volumeInfo
	   <1.3>	 6/11/89	ngk		Made better use of GetPubCBIndex
	   <1.2>	 5/31/89	ngk		Changed to use Alias manager instead of SofaLinks
	   <1.1>	 5/29/89	ngk		Changed PubCB to have a handle to usage info instead of the
									PubCB being variable size. Factored usage changes into
									dpPubCBbumpUsageCount
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}


	PROCEDURE dpSaveOffLastContainer;	FORWARD;

	
{========================== low level ============================}
	
		
	{
	Return the head of PubCB linked list.  Since head is a Ptr not a handle, return
	the location of the pointer. If globals not allocated, return NIL.
	}
	{-------------  dpGetPubCBListHeadNode -------------}
	FUNCTION dpGetPubCBListHeadNode(VAR headNode: PubCBLinkHandle): OSErr;
	VAR
		emGlobalsPtr:	EdtnMgrGlobalsPtrPtr;
	BEGIN
		emGlobalsPtr := dpGetGlobalsLocation;
		headNode := PubCBLinkHandle(emGlobalsPtr);
		IF emGlobalsPtr^ = NIL 
			THEN dpGetPubCBListHeadNode := editionMgrInitErr
			ELSE dpGetPubCBListHeadNode := noErr;
	END; { dpGetPubCBListHeadNode }
	
	
	{
	Creates a PubCB for 'editionFile' and puts it in the PubCB list.  If 
	unsuccessful, it returns NIL and an error.
	}
	{-------------  dpCreatePubCB -------------}
	FUNCTION dpCreatePubCB(editionFile: FSSpec; VAR thePubCB: PubCBHandle):OSErr;
	VAR
		i:			INTEGER;
		usage:		TotalAppUsageHandle;
		aPubCB: 	PubCBLinkHandle;
		headPubCB:	PubCBLinkHandle;
		foundAt: 	INTEGER; 
		foundPtr:	Ptr;
		syncErr:	OSErr;
		fi:			FailInfo;		
	BEGIN
		DoNotPutInRegister(@usage);
		thePubCB	:= NIL;
		usage		:= NIL;
		
		{ set up failure handling }
		IF isFailure(fi, dpCreatePubCB) THEN
		BEGIN
			IF thePubCB <> NIL THEN 
			BEGIN
				DisposHandle(Handle(thePubCB));
				thePubCB := NIL;
			END;
			IF usage <> NIL THEN 
			BEGIN
				DisposHandle(Handle(usage));
			END;
			EXIT(dpCreatePubCB);
		END; {if}

		{ allocate relocatable block in the system heap }
		thePubCB := PubCBHandle(NewHandleSysClear(SizeOf(PubCBRecord)));
		FailOSErr(MemError);
			
		FailOSErr(dpNewDynamicArray(SizeOf(TotalAppUsage), 			{headerSize}
									SizeOf(EachAppsUsage),			{slotSize}
									kInitialApps,					{initialSlots}
									NewHandleSysClear,				{MyNewHandle} {zeros memory}
									DyanmicArrayHandle(usage)));	{VAR arrayH}
		(*
		{ initialization of usage is not needed because NewHandleSysClear does all the work }
		WITH usage^^ DO
		BEGIN			
			totalUseCount	:= 0;
			totalIOCount	:= 0;
			FOR i := 1 TO kInitialApps DO
			BEGIN
				WITH appArray[i] DO
				BEGIN
					app			:= NIL;
					useCount	:= 0;
					IOCount		:= 0;
				END; {with}
			END; {for}
		END; {with}
		*)
		
		WITH thePubCB^^ DO
		BEGIN			
			{ initialize some fields, note: NewHandleSysClear already zeros out most fields }
			usageInfo		:= usage;
			{volumeInfo		:= 0;}
			{pubCNodeID		:= 0;}
			{lastVolMod		:= 0;}
			{lastDirMod		:= 0;}
			{oldPubCB		:= NIL;}
			{publisherApp	:= NIL;}
			{publisher		:= NIL;}
			{publisherAlias	:= NIL;}
			{publisherCount	:= 0;}
			{publisherKind	:= 0;}
			{fileMissing	:= FALSE;}
			fileRefNum		:= kClosedFileRefNum;
			openMode		:= dmNotOpen;
			fileMark		:= kBadPosition;
			{rangeLockStart	:= 0;}
			{rangeLockLen	:= 0;}
			{allocMap		:= NIL;}
			{formats		:= NIL;}
			{info.mdDate			:= 0;}
			{info.crDate			:= 0;}
			{info.fdCreator			:= '    ';}
			{info.fdType			:= '    ';}
			info.container.theFile	:= editionFile;
		END; {with}
			
		{ get initial info, OK if file is missing }
		syncErr := dpPubSync(thePubCB);
		IF syncErr <> noErr {&} THEN IF syncErr <> fnfErr THEN FailOSErr(syncErr);
		
		
		{ insert into the sorted PubCB list }	
		{ the sort order is pubFile.vRefNum then pubFile.dirID }
		{ that is, vRefNum=-3 is before vRefNum=-1 and dirID=0045 is before dirID=0167 }
		{ note: this is done last so that a failure above will not have to fix the PubCB list }

		FailOSErr(dpGetPubCBListHeadNode(headPubCB));
		aPubCB := headPubCB^^.nextPubCB;
		
		WHILE (aPubCB <> headPubCB) 
			& (PubCBHandle(aPubCB)^^.info.container.theFile.vRefNum < thePubCB^^.info.container.theFile.vRefNum) 
			DO aPubCB := aPubCB^^.nextPubCB;
		
		IF (aPubCB <> headPubCB) {&} THEN IF (PubCBHandle(aPubCB)^^.info.container.theFile.vRefNum = thePubCB^^.info.container.theFile.vRefNum) THEN
		BEGIN
			WHILE (aPubCB <> headPubCB) & (PubCBHandle(aPubCB)^^.info.container.theFile.parID < thePubCB^^.info.container.theFile.parID) 
				DO aPubCB := aPubCB^^.nextPubCB;
		END; {if}
		
		{ at this point we want to insert thePubCB just before aPubCB }
		WITH PubCBLinkHandle(thePubCB)^^ DO
		BEGIN
			nextPubCB 				:= aPubCB;
			prevPubCB 				:= aPubCB^^.prevPubCB;
			nextPubCB^^.prevPubCB 	:= PubCBLinkHandle(thePubCB);
			prevPubCB^^.nextPubCB 	:= PubCBLinkHandle(thePubCB);
		END; {with}
		
		Success(fi);
	END; { dpCreatePubCB }


	{
	Remove 'thePubCB' from the linked list and release the memory.
	}
	{-------------  dpDisposePubCB -------------}
	FUNCTION dpDisposePubCB(thePubCB: PubCBHandle): OSErr;
	BEGIN
		{ release usage information }
	{$IFC qRangeCheck }
		IF PubCBLinkHandle(thePubCB) = PubCBLinkHandle(dpGetGlobalsLocation^)
			THEN DebugStr('dpDisposePubCB: Tried to delete PubCBHeadNode.');
		WITH thePubCB^^.usageInfo^^ DO
		BEGIN
			IF totalUseCount <> 0 
				THEN DebugStr('dpDisposePubCB: TotalUseCount <> 0');
			IF totalIOCount <> 0 
				THEN DebugStr('dpDisposePubCB: TotalIOCount <> 0');
		END; {with}
	{$ENDC }	
		DisposHandle(Handle(thePubCB^^.usageInfo));
		
		{ remove it from the PubCB list }
		WITH PubCBLinkHandle(thePubCB)^^ DO
		BEGIN
			nextPubCB^^.prevPubCB := prevPubCB;
			prevPubCB^^.nextPubCB := nextPubCB;
		END; {with}

	{$IFC qRangeCheck }		
		{ do something if file is still open }
		IF thePubCB^^.fileRefNum <> kClosedFileRefNum
			THEN DebugStr('dpDisposePubCB: file not closed');
	{$ENDC }

		DisposHandle(Handle(thePubCB));
		dpDisposePubCB := MemError;
	END; { dpDisposePubCB }



	{
	Find the index for the Application 'me' within 'theUsageInfo'.
	If 'me' does not have an index and 'canGrow' is true, then resize PubCB to
	make room for an index for 'me'.
	}
	{-------------  dpGetAppsUsagePtr -------------}
	FUNCTION dpGetAppsUsagePtr(theUsageInfo: TotalAppUsageHandle; me: AppRefNum;
								canAddEntry: BOOLEAN; VAR usagePtr: EachAppsUsagePtr): OSErr;
	VAR
		anOpenSlotPtr:	EachAppsUsagePtr;
		ignoreSlot:		INTEGER;
		ignoreSlotPtr:	EachAppsUsagePtr;
		slotToUse:		INTEGER;
		
		FUNCTION MatchAppRefNum( index: INTEGER; elemPtr: EachAppsUsagePtr): BOOLEAN;
		BEGIN
			{ found the slot if app refnum matches }
			MatchAppRefNum := (elemPtr^.app = me);
			
			{ while we are at it, record off an open slot }
			IF elemPtr^.app = NIL THEN
			BEGIN
				slotToUse 		:= index;
				anOpenSlotPtr 	:= elemPtr;
			END;
		END; { MatchAppRefNum }
	
	BEGIN
		anOpenSlotPtr := NIL;
		IF NOT dpFindElement(DyanmicArrayHandle(theUsageInfo), SizeOf(TotalAppUsage), MatchAppRefNum, slotToUse, usagePtr) THEN
		BEGIN
			IF NOT canAddEntry THEN FailOSErr(memFullErr); 

			{ app does not have already have a slot, need to get one }
			IF anOpenSlotPtr <> NIL THEN
			BEGIN
				usagePtr := anOpenSlotPtr;
			END ELSE
			BEGIN

				{ no open slots, will need to grow the array }
				FailOSErr(dpAddDynamicArrayEntry(DyanmicArrayHandle(theUsageInfo), SizeOf(TotalAppUsage), slotToUse, usagePtr));
			END;
			{ is a slot available, use it }
			WITH usagePtr^ DO
			BEGIN
				app := me;
				useCount := 0;
				IOCount := 0;
			END; {with}
		END; {if}
		
		dpGetAppsUsagePtr := noErr;
	END; { dpGetAppsUsagePtr }


{========================== high level ============================}


	{-------------  dpRequestPubCB -------------}
	FUNCTION dpRequestPubCB(editionFile: FSSpec; me: AppRefNum; 
							checkExisting, canAllocateNew: BOOLEAN; 
							VAR thePubCB: PubCBHandle): OSErr;
	VAR
		aPubCB:			PubCBLinkHandle;
		headPubCB:		PubCBLinkHandle;
		created:		BOOLEAN;
		fi:				FailInfo;
	BEGIN
		DoNotPutInRegister(@created);
		thePubCB := NIL;
		created := FALSE;
		
		{ set up failure handling }
		IF IsFailure(fi, dpRequestPubCB) THEN
		BEGIN
			IF (thePubCB <> NIL) {&} THEN IF created 
				THEN IgnoreOSErr(dpDisposePubCB(thePubCB));
			thePubCB := NIL;
			EXIT(dpRequestPubCB);
		END; {if}

		FailOSErr(dpGetPubCBListHeadNode(headPubCB));
		aPubCB := headPubCB;
		
		{ search existing if OK }
		IF checkExisting THEN 
		BEGIN
			aPubCB := headPubCB^^.nextPubCB;
			{ walk PubCB looking for matching container }
			WHILE aPubCB <> headPubCB DO
			BEGIN
				WITH PubCBHandle(aPubCB)^^ DO
				BEGIN
					IF (info.container.theFile.vRefNum = editionFile.vRefNum)
					{&} THEN IF (info.container.theFile.parID = editionFile.parID)
					{&} THEN IF info.container.theFile.name[0] = editionFile.name[0]
					{&} THEN IF EqualString(info.container.theFile.name, editionFile.name, {caseSens}FALSE, {diacSens}FALSE)
							(*
							dpSameBytes(@info.container.theFile.name[1],
											@editionFile.name[1], 
											LENGTH(editionFile.name)) 
							*)
						THEN LEAVE; {while}
					aPubCB := links.nextPubCB;
				END; {with}
			END; {while}
		END; {if}
		
		{ if one does not already exist }
		IF aPubCB = headPubCB THEN
		BEGIN
			IF NOT canAllocateNew THEN FailOSErr(memFullErr); 
			{ and OK to create one }
			FailOSErr(dpCreatePubCB(editionFile, thePubCB));
			created := TRUE;
		END ELSE
		BEGIN
			thePubCB := PubCBHandle(aPubCB);
			created := FALSE;
		END; {if}
		
		{ mark that one more is using thePubCB }
		FailOSErr(dpPubCBbumpUsageCount(thePubCB^^.usageInfo, me, kCanGrowUsage, 1, 0));
						
		Success(fi);
	END; { dpRequestPubCB }
	
	

	{-------------  dpReleasePubCB -------------}
	FUNCTION dpReleasePubCB(thePubCB: PubCBHandle; me: AppRefNum): OSErr;
	VAR
		usageInfo:		TotalAppUsageHandle;
		emGlobals:		EdtnMgrGlobalsPtr;
	BEGIN
		usageInfo := thePubCB^^.usageInfo;
		
		FailOSErr(dpPubCBbumpUsageCount(usageInfo, me, NOT kCanGrowUsage, -1, 0));
		
		IF 	usageInfo^^.totalUseCount = 0 THEN 
		BEGIN
			{ if file still open because of slop, close it }
			IF thePubCB^^.openMode <> dmNotOpen THEN
			BEGIN
				{$IFC qRangeCheck}
				IF usageInfo^^.totalIOCount <> 0
					THEN DebugStr('dpReleasePubCB: I/O not completed.');
				{$ENDC}
				FailOSErr(dpPubCloseFile(thePubCB, NOT kFlush));
			END;
			
			{ if last edition used is this, then mark that need to use alias }
			emGlobals := dpGetGlobalsLocation^;
			IF emGlobals^.lastEditionUsed = thePubCB THEN
			BEGIN
				dpSaveOffLastContainer;
				emGlobals^.lastEditionUsed := NIL;
			END;
			
			FailOSErr(dpDisposePubCB(thePubCB));
		END;
		
		dpReleasePubCB := noErr;
	END; { dpReleasePubCB }



	{-------------  dpPubCBbumpUsageCount -------------}
	FUNCTION dpPubCBbumpUsageCount(usageInfo: TotalAppUsageHandle; 
								whichApp: AppRefNum; canGrowUsage: BOOLEAN;
								bumpUseCount,bumpIOCount: INTEGER):OSErr;
	VAR
		appUseCountPtr:	EachAppsUsagePtr;	
	BEGIN
		FailOSErr(dpGetAppsUsagePtr(usageInfo, whichApp, canGrowUsage, appUseCountPtr));
	
		{ bump individual count for app }
		WITH appUseCountPtr^ DO
		BEGIN
			useCount	:= useCount + bumpUseCount;
			IOCount		:= IOCount + bumpIOCount;
		END;
		
		{ bump total count of all apps }
		WITH usageInfo^^ DO
		BEGIN
			totalUseCount	:= totalUseCount + bumpUseCount;
			totalIOCount	:= totalIOCount + bumpIOCount;
		END;
		
	{$IFC qRangeCheck }
		WITH appUseCountPtr^ DO
		BEGIN
			IF useCount < 0 
				THEN DebugStr('dpPubCBbumpUsageCount: UseCount < 0');
			IF IOCount < 0
				THEN DebugStr('dpPubCBbumpUsageCount: IOCount < 0');
			IF IOCount > UseCount 
				THEN DebugStr('dpPubCBbumpUsageCount: IOCount > UseCount');
			IF (useCount = 0) & (IOCount <> 0) 
				THEN DebugStr('dpPubCBbumpUsageCount: IOCount <> 0');
		END; {with}
	{$ENDC }

		dpPubCBbumpUsageCount := noErr;
	END; { dpPubCBbumpUsageCount }
	


	{-------------  dpRemoveAppsPubCBs -------------}
	{ this is only used by patch of ExitToShell to clean up a crashed App }
	PROCEDURE dpRemoveAppsPubCBs(whichApp: AppRefNum);
	VAR
		aPubCB:				PubCBLinkHandle;
		aNextPubCB:			PubCBLinkHandle;
		headPubCB:			PubCBLinkHandle;
		appUseCountPtr:		EachAppsUsagePtr;	
		thisAppIOCount:		INTEGER;
		thisAppUseCount:	INTEGER;
		ignore:				OSErr;
		totalOtherIOCount:	INTEGER;
		usageInfo:			TotalAppUsageHandle;
		fi:					FailInfo;
		usagePtrErr:		OSErr;
	BEGIN
		{ walk PubCB looking for PubCBs in use by 'whichApp' }
		IF dpGetPubCBListHeadNode(headPubCB) <> noErr THEN EXIT(dpRemoveAppsPubCBs);
		aPubCB := headPubCB^^.nextPubCB;
		WHILE aPubCB <> headPubCB DO
		BEGIN
			aNextPubCB := aPubCB^^.nextPubCB;
			usageInfo := PubCBHandle(aPubCB)^^.usageInfo;
			
			{ see if aPubCB used by this app }
			{ catch any failures properly }
			IF NOT IsFailure(fi, usagePtrErr) THEN
			BEGIN
				FailOSErr(dpGetAppsUsagePtr(usageInfo, whichApp, NOT kCanGrowUsage, appUseCountPtr));
				Success(fi);
			END;
			IF usagePtrErr = noErr THEN
			BEGIN
				{ mark PubCB not longer used by this App }
				WITH appUseCountPtr^ DO
				BEGIN
					{$IFC qRangeCheck }
					IF app <> whichApp THEN DebugStr('dpRemoveAppsPubCBs: dpGetAppsUsagePtr blew it');
					{$ENDC }
					thisAppIOCount := IOCount;
					thisAppUseCount := useCount;
					app := NIL;
					useCount := 0;
					IOCount := 0;
				END;
				
				WITH usageInfo^^ DO
				BEGIN
					totalIOCount	:= totalIOCount - thisAppIOCount;
					totalUseCount	:= totalUseCount - thisAppUseCount;
				END;
				
				{ if this app used to have *the*publisher then free it }
				IF PubCBHandle(aPubCB)^^.publisherApp = whichApp THEN
				BEGIN
					WITH PubCBHandle(aPubCB)^^ DO
					BEGIN
						publisherApp	:= NIL;
						publisher		:= NIL;
					END; {with}
				END; {if}
				
				{##? change to call bottleNecks }
				{ if no one is suppose to be doing IO, terminate any IO that is set up }
				IF (usageInfo^^.totalIOCount = 0) 
					{&} THEN IF (PubCBHandle(aPubCB)^^.allocMap <> NIL) THEN
				BEGIN
					{ ignore failures }
					IF NOT IsFailure(fi, ignore) THEN
					BEGIN
						FailOSErr(dpPubReleaseMap(PubCBHandle(aPubCB)));
						Success(fi);
					END;
					{## maybe delete file, if open for new edition? }
				END;

				{##? change to call bottleNecks }
				{ if no one is suppose to have the file open and it is, then close it }
				IF (usageInfo^^.totalIOCount = 0) 
					{&} THEN IF (PubCBHandle(aPubCB)^^.publisher = NIL)
					{&} THEN IF (PubCBHandle(aPubCB)^^.fileRefNum <> kClosedFileRefNum) THEN
				BEGIN
					{ ignore failures }
					IF NOT IsFailure(fi, ignore) THEN
					BEGIN
						FailOSErr(dpPubCloseFile(PubCBHandle(aPubCB), NOT kFlush));
						Success(fi);
					END;
				END;

				{ If no other app is using the controlBlock, then remove it }
				IF usageInfo^^.totalUseCount = 0 THEN
				BEGIN
					{ ignore failures }
					IF NOT IsFailure(fi, ignore) THEN
					BEGIN
						FailOSErr(dpDisposePubCB(PubCBHandle(aPubCB)));
						Success(fi);
					END;
				END;
				
			END; {if}
			aPubCB := aNextPubCB;
		END; {while}
		
	END; { dpRemoveAppsPubCBs }
	



{========================== preferences ============================}

(*
 { In systemn 7.0 parts are not supported.  So, we just put an alias to edition
   file in preference file.  When we do support parts, we should append a 
   ContainerQualifier on the end of the alias record.
 }

TYPE
	pContainerQualifier	=	^ContainerQualifier;
	ContainerQualifier	=	RECORD
								thePart:		LONGINT;
								thePartName:	Str31;
							END;
*)



	{-------------  dpGetPreferenceFile -------------}
	PROCEDURE dpGetPreferenceFile(VAR prefFile:	FSSpec);
	BEGIN
		{ get preference file from preference folder }
		{### assumes string and folder exist }
		{$PUSH} {$R-}
		prefFile.name := GetString(rLastEditionUsedString)^^;
		IF FindFolder(kOnSystemDisk, kPreferencesFolderType, {createFolder}TRUE, prefFile.vRefNum, prefFile.parID) <> noErr THEN;
		{$POP}
	END; { dpGetPreferenceFile }



	{-------------  dpRetreivedLastContainer -------------}
	PROCEDURE dpRetreivedLastContainer(VAR lastEdition: EditionContainerSpec);	
	VAR
		prefFile:		FSSpec;
		targetIsFolder:	BOOLEAN;
		wasAliased:		BOOLEAN;
		fndrInfo: 		FInfo;
		resolveErr:		OSErr;
	BEGIN
		{ make up a default last edition, in case alias fails }
		WITH lastEdition.theFile DO
		BEGIN
			vRefNum 	:= -1 * pInteger(SFSaveDisk)^;
			parID 		:= pLongint(CurDirStore)^;
			name		:= '';
		END; {with}
		lastEdition.theFileScript	:= LoWrd(GetEnvirons(smSysScript));
		lastEdition.thePart 		:= kPartsNotUsed;
		lastEdition.thePartName 	:= '';
		lastEdition.thePartScript	:= lastEdition.theFileScript;
		
		{ get preference file spec }
		dpGetPreferenceFile(prefFile);
		
		{ if preference file exists, use it }
		IF FSpGetFInfo(prefFile, fndrInfo) = noErr THEN
		BEGIN
			{ resolve it to last edition used, use it if found }
			resolveErr := ResolveAliasFile(prefFile, {resolveAliasChains}FALSE, targetIsFolder, wasAliased);
			IF (resolveErr = noErr) OR (resolveErr = fnfErr)
				THEN BlockMove(@prefFile, @lastEdition.theFile, SizeOf(FSSpec));    { lastEdition.theFile := prefFile }
		END; {if}
		
		{ when we support parts, we will not be able to use ResolveAliasFile }
		{ because we will need to get to ContainerQualifier at end of alias. }
	END; { dpRetreivedLastContainer }




			
	{-------------  dpSaveOffLastContainer -------------}
	PROCEDURE dpSaveOffLastContainer;	
	VAR
		saveRes:		Integer;
		lastEdition:	EditionContainerSpec;
		lastUsedAlias:	AliasHandle;
		wasChanged:		BOOLEAN;
		ignore:			OSErr;
		thePubCB: 		PubCBHandle;
		prefFile:		FSSpec;
		prefResFile:	INTEGER;
		fi:				FailInfo;
	
		PROCEDURE dpSetAliasBitAndType(VAR PBC: CInfoPBRec);
		CONST
			kAliasFlag = $8000;
		BEGIN
			PBC.ioFlFndrInfo.fdFlags := bOr(PBC.ioFlFndrInfo.fdFlags, kAliasFlag);
			WITH thePubCB^^.info DO
			BEGIN
				PBC.ioFlFndrInfo.fdCreator := fdCreator;
				PBC.ioFlFndrInfo.fdType := fdType;
			END; {with}
		END; { SetAliasBit }

	LABEL 999; { use this to save duplication of code to clean up, between failure handler and end of proc }
	
	BEGIN
		thePubCB := dpGetGlobalsLocation^^.lastEditionUsed;
		HLock(Handle(thePubCB));
		saveRes := CurResFile;
		
		{ open the preferences file }
		dpGetPreferenceFile(prefFile);
		prefResFile := FSpOpenResFile(prefFile, {permissions}0);
		
		{ handle initial case where preference file does not exist or is corrupt }
		IF prefResFile = -1 THEN
		BEGIN
			{ delete corrupted one, or do nothing }
			IgnoreOSErr(FSpDelete(prefFile));
		
			{ make a new one with no type or creator }
			FSpCreateResFile(prefFile, '    ', '    ', smSystemScript);

			{ open it }
			prefResFile := FSpOpenResFile(prefFile, {permissions}0);
		END; {if}
		
		{ set up failure handling }
		IF isFailure(fi, ignore) 
			THEN GOTO 999;
		
		{ if preference file is around then use it }
		IF prefResFile <> -1 THEN
		BEGIN
			{ now update the type and creator }
			IgnoreOSErr(dpTweakCatInfo(prefFile, dpSetAliasBitAndType));
			
			{ get the alias record pointing to last edition used }
			lastUsedAlias := AliasHandle(Get1Resource(rAliasType, rLastEditionAliasID));

			IF lastUsedAlias = NIL THEN
			BEGIN
				{ first time through?  create a new alias resource }
				FailOSErr(NewAlias(NIL, thePubCB^^.info.container.theFile, lastUsedAlias));
				AddResource(Handle(lastUsedAlias), rAliasType, rLastEditionAliasID, '');
			END ELSE
			BEGIN
				{ update existing alias resource }
				FailOSErr(UpdateAlias(NIL, thePubCB^^.info.container.theFile, lastUsedAlias, wasChanged));
			END;
			
			(*
			{ when we support parts, add the container qualifier to the end of the alias }
			SetHandleSize(Handle(lastUsedAlias), lastUsedAlias^^.aliasSize + SizeOf(ContainerQualifier));
			IF MemError = noErr THEN
			BEGIN
				subPartPtr 	:= pContainerQualifier(pLongint(lastUsedAlias)^ + lastUsedAlias^^.aliasSize);
				subPartPtr^.thePart	 	:= lastEdition.thePart;
				{$PUSH} {$R-}
				subPartPtr^.thePartName := lastEdition.thePartName;
				{$POP}
			END; {if}
			*)
			
			{ flush changes to disk }
			ChangedResource(Handle(lastUsedAlias));
		END; {if}

		Success(fi);
	999:		
		IF prefResFile <> -1 
			THEN CloseResFile(prefResFile);
		HUnLock(Handle(thePubCB));
		UseResFile(saveRes);
	END; { dpSaveOffLastContainer }
	
	
	
	{------------- dp_GetLastEditionContainerUsed -------------}
	FUNCTION dp_GetLastEditionContainerUsed(VAR container: EditionContainerSpec): OSErr;	
	VAR
		fndrInfo:		FInfo;
		infoErr:		OSErr;
		lastPubCB:		PubCBHandle;
	BEGIN
		dp_GetLastEditionContainerUsed := noErr;
		
		{ get last pub from globals }
		lastPubCB := dpGetGlobalsLocation^^.lastEditionUsed;
		IF lastPubCB <> NIL  
			THEN BlockMove(@lastPubCB^^.info.container, @container, SizeOf(EditionContainerSpec)) { container := lastPubCB^^.info.container }
			ELSE dpRetreivedLastContainer(container);	
		
		{ check that file really exists }
		WITH container.theFile DO
		BEGIN
			infoErr := FSpGetFInfo(container.theFile, fndrInfo);
			IF infoErr <> noErr THEN
			BEGIN
				{ if file does not exist, then don't return it }
				name 	:= '';
				dp_GetLastEditionContainerUsed := fnfErr;
				
				{ if folder or volume does not exist, then use current app SF location }
				IF infoErr <> fnfErr THEN
				BEGIN
					vRefNum := -1 * pInteger(SFSaveDisk)^;
					parID 	:= pLongint(CurDirStore)^;
				END;
			END;
		END; {with}
		
	END; { dp_GetLastEditionContainerUsed }

