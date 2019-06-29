{
	File:		dpSection.inc.p

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<27>	  4/5/91	ngk		MM, #86192: fix GotoPublisher bug. Change canAskUser parameter
	   								from false to true.
		<26>	 2/27/91	ngk		MM, #b5-MTM-006: unregister bug could crash if section not
									registered.
		<25>	 1/14/91	ngk		<MM> RegisterSection was not returning containerNotFoundWrn.
		<24>	12/14/90	ngk		<MM>Pulled resolving code out of RegisterSection into
									dpReconnectSection, allowing SectionOptionsDialog to resolve and
									create a pubCB. Removed dp_FindEditionDialog. Reworked resolving
									to use ResolveAlias instead of MatchAlias.
		<23>	10/30/90	ngk		fix priority of multiplePublisherWrn and notThePublisherWrn in
									AddPublisher.
		<22>	  8/4/90	ngk		Add extra protection to dpDeQueueSection
		<21>	  8/2/90	ngk		Use DoNotPutInRegister
		<20>	 7/30/90	ngk		Use IgnoreOSErr.
		<19>	 7/23/90	ngk		fix register section to always set aliasWasChanged
		<18>	 7/14/90	ngk		Fix dpRemovePublisher to close file when publisher unregisters.
		<17>	  7/2/90	ngk		Changed sameBytes to EqualString for comparing document names
		<16>	  7/2/90	ngk		Made new dpAddPublisher and dpRemovePublisher routines.
	   								During register of publisher, resolve alias from edition file
									to see if this is the publisher that last wrote.
	   								Return userCanceledErr in registersection.
		<15>	 6/20/90	ngk		Changed CanonicalFileSpec to FSSpec.
		<14>	 6/11/90	ngk		Fix bug in goto publisher, caused by change of Edition fdType.
		<13>	 5/31/90	ngk		Changed saving of last edition used to be an alias
	   								stored in preference file.  Made some code saving
									tweaks.
		<12>	  5/3/90	ngk		Get rid of Select alias dialog. If publisher can not
	   								find edition, one is created for it.  
		<11>	 4/11/90	ngk		Give relative path priorty in resolving
		<10>	  4/7/90	ngk		Call dpPubSync in GetEditionInfo. Use new FailOSErr.  Fixed
									IsRegisteredSection to not return noErr on NIL
		 <9>	 3/10/90	ngk		Moved locking and unlocking of package to dispatcher.
		 <8>	 2/25/90	ngk		Fixed IsRegisteredSection bug, where it sometimes falsely
									returned noErr. Fixed bug in NewSection error handling, where it
									did not deQueue a section in AddPubCB failed.
		 <7>	 2/16/90	ngk		Clear refNum in RegisterSection. Get GotoPublisher to work on
									non-edition files. Set mdDate of publisher to now in NewSection.
		 <6>	  2/4/90	ngk		Fix bug in opening publisher in folder without write permission
		 <5>	 1/26/90	ngk		Changed calls to Alias Manager to use non-pointer style
		 <4>	  1/8/90	ngk		remove get, save, and dispose section
		<2+>	  1/7/90	ngk		remove get, save, and dispose section
		 <2>	  1/6/90	ngk		Convert to BBS Change register stategy. Change 'goto' to 'scrl'.
									Renamed routines. Added FindEditionDialog
	   <2.2>	11/13/89	ngk		Added Locking and restoring of Pack11 in all public routines
	   <2.1>	 11/4/89	ngk		Changed AppRefNum to be a handle to app's globals.
	   <2.0>	10/13/89	ngk		Added stuff to dpGotoPublisher to remember to send goto event
	   <1.9>	10/10/89	ngk		Changed dp_NewSection to allow NIL for sectionDocument
	  <•1.8>	 10/2/89	ngk		Changed to EditionContainerSpec instead of PubSpec
	   <1.7>	 9/18/89	ngk		Changed some handling of multiple publishers
	   <1.6>	  9/7/89	ngk		Changed DisposeAlias to DisposHandle
	   <1.5>	 8/29/89	ngk		Factored dp_RegisterSection into dpEnQueueSection and
									dpAddPubCBSection Factored dp_UnRegisterSection into
									dpDeQueueSection and dpRemovePubCBSection Changed publication
									specification to be a PubSpec instead of FileSpec. Force pub
									file open when publisher is registered.
	   <1.4>	  8/8/89	ngk		Added use of PubPart throughout. Changed to new error codes.
	   <1.3>	 6/11/89	ngk		Fixed bug in AssociateSection, now uses UpdateAlias Changed
									GetPubInfo to not return format list
	   <1.2>	 5/31/89	ngk		Changed SofaLink usage to new Alias manager
	   <1.1>	 5/29/89	ngk		Changed NewSection to save InitalMode in DP globals Changed to
									separate resources for SectionRecord and AliasRecord
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}


{========================== private routines ============================}

CONST
	kSectionVersion 		= 1;
	kSectionUserType 		= 'publ';	

	{-------------  dpCheckSection -------------}
	FUNCTION dpCheckSection(sectionH: SectionHandle): OSErr;
	BEGIN
		{$IFC qRangeCheck }
		IF sectionH = NIL THEN DebugStr('dpCheckSection: sectionH is NIL');
		{$ENDC }
		
		IF LONGINT(AliasHandle(sectionH^^.alias)^^.userType) <> LONGINT(kSectionUserType)
			THEN FailOSErr(badSectionErr);
		
		IF sectionH^^.Version <> kSectionVersion 
			THEN FailOSErr(badSectionErr);
			
		dpCheckSection := noErr;
	END; { dpCheckSection }
	

	
	{-------------  dpEnQueueSection -------------}
	FUNCTION dpEnQueueSection(sectionH: SectionHandle; anAppGlobal: AppRefNum): OSErr;
	BEGIN
		{ make sure it is a valid kind of section }
		WITH sectionH^^ DO
			IF (kind <> stSubscriber) {&} THEN IF (kind <> stPublisher) 
				THEN FailOSErr(badSectionErr);
			
		{ put sectionH in this apps section list }
		WITH anAppGlobal^^ DO
		BEGIN
			sectionH^^.nextSection := sectionHandleListHead;
			sectionHandleListHead := sectionH;
		END; {with}
		
		dpEnQueueSection := noErr;
	END; { dpEnQueueSection }
	
	
	{-------------  dpDeQueueSection -------------}
	FUNCTION dpDeQueueSection(sectionH: SectionHandle; anAppGlobal: AppRefNum): OSErr;
	VAR
		nextSectionH:	SectionHandle;
		prevSectionH:	SectionHandle;
	BEGIN		
		{ find previous section in this apps section list }
		prevSectionH := anAppGlobal^^.SectionHandleListHead;
		IF prevSectionH = SectionHandle(kNoMoreSections)
			THEN FailOSErr(notRegisteredSectionErr);
		
		IF prevSectionH = sectionH THEN
		BEGIN
			{ is first in list }
			anAppGlobal^^.SectionHandleListHead := sectionH^^.nextSection;
		END ELSE
		BEGIN
			{ is not first in list }
			nextSectionH := prevSectionH;
			REPEAT
				prevSectionH := nextSectionH;
				IF prevSectionH^^.nextSection = SectionHandle(kNoMoreSections)
					THEN FailOSErr(notRegisteredSectionErr);
				nextSectionH := prevSectionH^^.nextSection;
			UNTIL nextSectionH = sectionH;
			
			{ remove sectionH from this apps section list }
			prevSectionH^^.nextSection := sectionH^^.nextSection;
		END;
				
		dpDeQueueSection := noErr;
	END; { dpDeQueueSection }
	
	
	{-------------  dpAddPublisher -------------}
	FUNCTION dpAddPublisher(thePubCB: PubCBHandle; sectionH: SectionHandle; 
							inApp: AppRefNum; sectionDocument: FSSpecPtr): OSErr;
	VAR
		thisKind:			SectionType;
		openErr:			OSErr;
		resolveErr:			OSErr;
		lastPublisherID:	LONGINT;
		lastPublisherDoc:	FSSpec;
		theGoto:			GotoInfoHandle;
		isThePublisher:		BOOLEAN;
		multiplePublishers:	BOOLEAN;
		rfi:				FailInfo;
	BEGIN
		multiplePublishers := FALSE;
		
		thisKind := sectionH^^.kind;
		WITH thePubCB^^ DO
		BEGIN		
			{ if this publisher won't share and there are others then fail }
			IF bTST(thisKind, kDoNotAllowMultipleWritersBit) THEN
			BEGIN
				IF publisherCount > 0 THEN FailOSErr(notThePublisherErr);
				
				{ mark it as king of the mountain }
				FailOSErr(dpMakeItThePublisher(thePubCB, sectionH, inApp));
			END; {if}
			
			{ if this publisher wants to be warned about other publishers }
			IF bTST(thisKind, kWarnIfMultipleWritersBit) THEN
			BEGIN
				IF publisherCount > 0 
					THEN multiplePublishers := TRUE;
			END; {if}
		
			{ for edition files we make sure it is open }
			{ ### this really should go through the opener ### }
			IF IsEditionFile(info.fdType) THEN
			BEGIN
				{ open the file, to lock it as the publisher }
				openErr := dpPubOpenFile(thePubCB, thisKind);
				CASE openErr OF
					permErr, afpDenyConflict, afpAccessDenied:
						BEGIN
							{ there is a publisher on another machine }
							IF bTST(thisKind, kDoNotAllowMultipleWritersBit) 
								THEN FailOSErr(notThePublisherErr)
								ELSE multiplePublishers := TRUE;
						END;
					noErr:
						;
					OTHERWISE
						FailOSErr(openErr);	{ skip out and don't increment publisherCount }
				END; {case}	
			END; {if}				
		END; {with}
	
		WITH thePubCB^^ DO
			publisherCount := publisherCount + 1;
		
		{ now lets see if this was the publisher that last wrote to the edition }
		IF NOT IsFailure(rfi, resolveErr) THEN { catch any failures properly }
		BEGIN
			FailOSErr(dpResolveAliasToPublisher(thePubCB, {canAskUser}FALSE, lastPublisherDoc, lastPublisherID));
			Success(rfi);
		END;
		{ assume it was not }
		isThePublisher := FALSE;
		
		{ if there is no alias in the edition, then it is OK to assume this guy is the publisher }
		IF resolveErr = noTypeErr 
			THEN isThePublisher := TRUE;
			
		{ if there was an alias, and it resolves to this publisher's document and section ID then bingo }
		IF resolveErr = noErr 
		 {&} THEN IF sectionDocument <> NIL
		 {&} THEN IF lastPublisherID = sectionH^^.sectionID 
		 {&} THEN IF sectionDocument^.vRefNum = lastPublisherDoc.vRefNum
		 {&} THEN IF sectionDocument^.parID = lastPublisherDoc.parID
		 {&} THEN IF EqualString(sectionDocument^.name, lastPublisherDoc.name, FALSE, FALSE)
		THEN isThePublisher := TRUE;
		
		{ if this is the publisher that last wrote, then record in PubCB, else return warning }
		IF isThePublisher THEN
		BEGIN
			WITH thePubCB^^ DO
			BEGIN
				publisher := sectionH;
				publisherApp := inApp;
			END; {with}
		END;

		{ check if there is a goto event pending for this publisher }
		theGoto := dpGetGlobalsLocation^^.gotoHere;
		IF theGoto <> NIL THEN
		BEGIN
			WITH thePubCB^^, theGoto^^ DO
			BEGIN
				IF info.container.theFile.vRefNum = editionVRefNum 
				 {&} THEN IF pubCNodeID = editionID THEN
				BEGIN
					{ only remove goto handle when correct publisher registers }
					IF sectionH^^.sectionID = {theGoto^^.}sectionID THEN
					BEGIN
						{if the sectionID is right, remove pending goto info }
						dpGetGlobalsLocation^^.gotoHere := NIL;
						DisposHandle(Handle(theGoto));
					END; {if}

					{ a publisher to correct edition is being registered }
					{ send it a goto event }
					IgnoreOSErr(dp_PostSectionEvent(sectionH, inApp, sectionScrollMsgID));
				END; {if}
			END; {with}
		END; {if}
		
		{ return correct error/warning code }
		dpAddPublisher := noErr;
		IF multiplePublishers 
			THEN dpAddPublisher := multiplePublisherWrn
			ELSE IF NOT isThePublisher
				THEN dpAddPublisher := notThePublisherWrn;
	
	END; { dpAddPublisher }
	
	
	{-------------  dpRemovePublisher -------------}
	FUNCTION dpRemovePublisher(thePubCB: PubCBHandle; sectionH: SectionHandle; inApp: AppRefNum): OSErr;
	BEGIN
		WITH thePubCB^^ DO
		BEGIN
			publisherCount := publisherCount - 1;

			IF publisher = sectionH THEN
			BEGIN
				publisher := NIL;
				
				{ for edition files we close when done to mark it is up for grabs }
				{ ### this really should go through the opener ### }
				IF IsEditionFile(info.fdType) 
				 {&} THEN IF bTST({thePubCB^^.}openMode, dmRequestWritePermBit) THEN
				BEGIN
					{ we need to close file, because publisher is unregistering }
					IF {thePubCB^^.}usageInfo^^.totalIOCount = 0 THEN
					BEGIN
						{ close the file }
						IgnoreOSErr(dpPubCloseFile(thePubCB, NOT kFlush));
					END ELSE
					BEGIN
						{ looks like some subscribers are reading, }
						{ so leave file open but have subscriber close it when its done }
						{thePubCB^^.}openMode := {thePubCB^^.}openMode - dmRequestWritePerm; 
					END; 
				END; {if}				
			END; {if}	
			
		END; {with}
		dpRemovePublisher := noErr;
	END; { dpRemovePublisher }
	

	
	{-------------  dpMakeItThePublisher -------------}
	FUNCTION dpMakeItThePublisher(thePubCB: PubCBHandle; sectionH: SectionHandle; inApp: AppRefNum): OSErr;
	BEGIN
		{ mark PubCB that we have *the* publisher }
		WITH thePubCB^^ DO
		BEGIN
			publisher 		:= sectionH;
			publisherKind	:= sectionH^^.kind;
			publisherApp 	:= inApp;
		END;
		dpMakeItThePublisher	:= noErr;
	END; { dpMakeItThePublisher }
	


		
	{-------------  dpAddPubCBSection -------------}
	FUNCTION dpAddPubCBSection(sectionH: SectionHandle; withPub: EditionContainerSpecPtr; 
								inApp: AppRefNum; sectionDocument: FSSpecPtr): OSErr;
	VAR
		thePubCB:		PubCBHandle;
		fi:				FailInfo;
	BEGIN
		thePubCB  := NIL;
		
		IF isFailure(fi, dpAddPubCBSection) THEN
		BEGIN
			EXIT(dpAddPubCBSection);
		END; {if}
		
		{ make sure a sub-part is not used }
		IF withPub^.thePart <> kPartsNotUsed THEN FailOSErr(badSubPartErr);
	
		{ make sure there is a PubControlBlock for it }
		FailOSErr(dpRequestPubCB(withPub^.theFile, inApp, kCheckExisting, kCanAllocateNew, thePubCB));
		sectionH^^.controlBlock := Handle(thePubCB);
		
		WITH sectionH^^ DO	{don't need to lock because calls are mutually exclusive }
		BEGIN
			{ send event if it is a not up to date subscriber }
			IF bTST(kind, kCanReadEditionsBit) 
				{&} THEN IF (mdDate <> thePubCB^^.info.mdDate)
				{&} THEN IF (mode = sumAutomatic) 
				{&} THEN IF (thePubCB^^.fileMissing = FALSE)
				THEN IgnoreOSErr(dp_PostSectionEvent(sectionH, inApp, sectionReadMsgID));
				
			{ If this can write, check for conflicts }
			IF bTST(kind, kCanWriteEditionsBit) 
				THEN dpAddPubCBSection := dpAddPublisher(thePubCB, sectionH, inApp, sectionDocument);
		END; {with}
		
		Success(fi);
	END; { dpAddPubCBSection }
	
	
	
	{-------------  dpRemovePubCBSection -------------}
	FUNCTION dpRemovePubCBSection(sectionH: SectionHandle; inApp: AppRefNum): OSErr;
	VAR
		thePubCB:	PubCBHandle;
	BEGIN
		WITH sectionH^^ DO
		BEGIN
			{ can not remove PubCB if I/O is still open }
			IF SIOCBHandle(refNum) <> NIL 
				THEN FailOSErr(fBsyErr);
			
			thePubCB := PubCBHandle(controlBlock);
	
			{ If this can write, let everyone know it is leaving }
			IF bTST(kind, kCanWriteEditionsBit) 
				THEN FailOSErr(dpRemovePublisher(thePubCB, sectionH, inApp));
		END; {with}
		
		{ decrement count in PubControlBlock for it, possibly remove PubCB }
		FailOSErr(dpReleasePubCB(thePubCB, inApp));
		
		sectionH^^.controlBlock := NIL;
				
		dpRemovePubCBSection := noErr;
	END; { dpRemovePubCBSection }
	
		
	
	{-------------  dpForEachSectionDo -------------}
	PROCEDURE dpForEachSectionDo(PROCEDURE Doit(sectionH: SectionHandle; inApp: AppRefNum));
	VAR
		anApp: 		PerAppGlobalsHandle;
		aSection: 	SectionHandle;
	BEGIN
		{ for each app }
		anApp := dpGetGlobalsLocation^^.perAppListHead;
		WHILE anApp <> NIL DO
		BEGIN
			{ for each registered section }
			aSection := anApp^^.SectionHandleListHead;
			WHILE aSection <> SectionHandle(kNoMoreSections) DO
			BEGIN
				Doit(aSection, anApp);
				aSection := aSection^^.nextSection;
			END; {while}
			anApp := anApp^^.nextApp;
		END; {while}
	END; { dpForEachSectionDo }



	{-------------  dpReconnectSection -------------}
	FUNCTION dpReconnectSection(sectionDocument: FSSpecPtr;
								sectionH: SectionHandle; thisApp: AppRefNum; 
								VAR editionWasCreated, aliasWasChanged: BOOLEAN): OSErr;
	{ can return noErr, fnfErr, nsvErr, userCanceledErr,  }
	{			paramErr, notThePublisherWrn, multiplePublisherWrn }
	VAR
		resolveErr:		OSErr;
		addErr:			OSErr;
		theAlias:		AliasHandle;
		edition: 		EditionContainerSpec;
	BEGIN
		aliasWasChanged := FALSE;
		editionWasCreated := FALSE;
							
		{ get the alias }
		theAlias := sectionH^^.alias;
		IF theAlias = NIL THEN
		BEGIN
			{ be nice and do nothing if alias is missing }
			dpReconnectSection := containerNotFoundWrn;
			EXIT(dpReconnectSection);
		END;
		
		{ resolve alias with section to find target edition file }
		resolveErr := ResolveAlias(sectionDocument, theAlias, edition.theFile, aliasWasChanged);

		{ change FSSpec into a container spec }
		{### when we support parts, we can get the part off the end of the alias }
		edition.thePart := kPartsNotUsed; 
		
		IF resolveErr <> noErr THEN
		BEGIN
			IF resolveErr <> fnfErr THEN
			BEGIN
				{ ResolveAlias return somehting other than noErr or fnfErr, so we're hosed }
				dpReconnectSection := resolveErr;
				EXIT(dpReconnectSection);
			END ELSE
			BEGIN
				{ ResolveAlias returned, fnfErr => the edition contains the last known location }
				IF sectionH^^.kind = stPublisher THEN
				BEGIN
					{ have publisher with missing edition, so recreate it }
					{ alias manager returns where it used to be in first entry }
					editionWasCreated := (FSpCreate(edition.theFile, '    ', kUnknownEditionFileType,
													smSystemScript) = noErr);
				END; {if}
			END;
		END;	
	
		{ if there is no control block for section, make one }
		IF sectionH^^.controlBlock = NIL THEN
		BEGIN
			{ set up a control block, return AppPubCBSection's error }
			addErr := FailNonWrn(dpAddPubCBSection(sectionH, @edition, thisApp, sectionDocument));
			
			{ want to return the dpAddPubCBSection error, unless dpReconnectSection return fnf }
			IF resolveErr <> fnfErr
				THEN resolveErr := addErr;
		END; {if}
			
		dpReconnectSection := resolveErr;					
	END; { dpReconnectSection }

	
		
	
{========================== public routines ============================}
	

	{-------------  dp_NewSection -------------}
	FUNCTION dp_NewSection(container: EditionContainerSpec; 
						sectionDocument: FSSpecPtr; 
						itsSectionType: SectionType; itsSectionID: longint;
						initialMode: UpdateMode; 
						VAR sectionH: SectionHandle): OSErr;
	VAR
		thisApp:				AppRefNum;
		aliasH:					AliasHandle;
		anErr:					OSErr;
		localSectionDocument:	FSSpec;
		isInQueue:				BOOLEAN;
		fi: 					FailInfo;
	BEGIN 
		DoNotPutInRegister(@isInQueue);
		DoNotPutInRegister(@aliasH);
		aliasH := NIL;
		sectionH := NIL;
		isInQueue := FALSE;
		
		{ set up failure handler }
		IF isFailure(fi, dp_NewSection) THEN
		BEGIN
			IF sectionH <> NIL THEN 
			BEGIN
				IF isInQueue THEN
				BEGIN
					anErr := dpDeQueueSection(sectionH,thisApp);
					{$IFC qCatchFailures }
					IF anErr <> noErr 
						THEN DebugStr('dp_NewSection.fail: could not DeQueue section.');
					{$ENDC }
				END;
				DisposHandle(Handle(sectionH));
				sectionH := NIL;
			END;
			IF aliasH <> NIL THEN 
			BEGIN
				DisposHandle(Handle(aliasH));
				aliasH := NIL;
			END;
			EXIT(dp_NewSection);
		END; {if}
		
		{ make sure a container part is not used }
		IF container.thePart <> kPartsNotUsed THEN FailOSErr(badSubPartErr);
		
		FailOSErr(dp_GetCurrentAppRefNum(thisApp));

		{ Create alias to container }
		(*
		IF sectionDocument <> NIL THEN
		BEGIN
			{ make local copy if not NIL }
			localSectionDocument 	:= sectionDocument^;
			sectionDocument 		:= @localSectionDocument;
		END;
		*)
		FailOSErr(NewAlias(sectionDocument, container.theFile, aliasH));

		{ mark the Alias to be a pointer to a container }
		aliasH^^.userType := kSectionUserType;

		{ Create a relocatable SectionRecord }
		sectionH := SectionHandle(NewHandleClear(SizeOf(SectionRecord)));
		FailOSErr(MemError);
		
		{ initialize the section record  }
		WITH sectionH^^ DO
		BEGIN
			version		:= kSectionVersion;
			kind		:= itsSectionType;
			mode		:= InitialMode;
			{mdDate		:= 0;}
			sectionID	:= itsSectionID;
			{refCon		:= 0;}
			alias			:= aliasH;
			subPart			:= container.thePart;	{ ought to be kPartsNotUsed }
			{nextSection	:= NIL;}
			{controlBlock	:= NIL;}
			{refNum			:= NIL;}
		END;

		{ put it in registered section list }
		FailOSErr(dpEnQueueSection(sectionH, thisApp));
		isInQueue := TRUE;

		{ get a pubCB for it }
		dp_NewSection := FailNonWrn(dpAddPubCBSection(sectionH, @container,
														thisApp, sectionDocument));
		
		{ remember it }
		dpGetGlobalsLocation^^.lastEditionUsed := PubCBHandle(sectionH^^.controlBlock);
		
		{ publishers start with current time as mdDate }
		IF bTST(itsSectionType, kCanWriteEditionsBit)
			THEN GetDateTime(sectionH^^.mdDate);
		
		Success(fi);	
	END; { dp_NewSection }
	
	
	
	{-------------  dp_RegisterSection -------------}
	FUNCTION dp_RegisterSection(sectionDocument: FSSpec; sectionH: SectionHandle;
								VAR aliasWasChanged: BOOLEAN): OSErr;
	VAR
		thisApp:			AppRefNum;
		editionWasCreated:	BOOLEAN;
		sectionWasEnqueued:	BOOLEAN;
		connectErr:			OSErr;
		fi:					FailInfo;
	BEGIN
		DoNotPutInRegister(@sectionWasEnqueued);
		sectionWasEnqueued := FALSE;
		aliasWasChanged := FALSE;
		
		{ always clear refNum and controlBlock }
		WITH sectionH^^ DO
		BEGIN
			controlBlock 	:= NIL;
			refNum 			:= NIL;
		END; {with}

		{ set up failure handler }
		IF isFailure(fi, dp_RegisterSection) THEN
		BEGIN
			IF sectionWasEnqueued  
				THEN IgnoreOSErr(dpDeQueueSection(sectionH, thisApp));
			EXIT(dp_RegisterSection);
		END; {if}

		FailOSErr(dp_GetCurrentAppRefNum(thisApp));
	{$IFC qRangeCheck }
		FailOSErr(dpCheckSection(sectionH));
	{$ENDC }
		
		{ put it in registered section list }
		FailOSErr(dpEnQueueSection(sectionH, thisApp));
		sectionWasEnqueued := TRUE;
					
		{ find the edition that this section is for, and set up controlblock }
		connectErr := dpReconnectSection(@sectionDocument, sectionH, thisApp,
										editionWasCreated, aliasWasChanged);
		
		{ app expects noErr, containerNotFoundWrn, }
		{ userCanceledErr(considered a warning), or something really bad }
		IF (connectErr = nsvErr) | (connectErr = fnfErr) 
			THEN connectErr := containerNotFoundWrn;
		dp_RegisterSection := FailNonWrn(connectErr);
 
		{ if it is a publisher and an edition was created, then need to fill it }
		IF editionWasCreated {&} THEN IF sectionH^^.kind = stPublisher 
			THEN IgnoreOSErr(dp_PostSectionEvent(sectionH, thisApp, sectionWriteMsgID));
				
		Success(fi);
	END; { dp_RegisterSection }
	


	{-------------  dp_UnRegisterSection -------------}
	FUNCTION dp_UnRegisterSection(sectionH: SectionHandle): OSErr;
	VAR
		thisApp:		AppRefNum;
		fi:				FailInfo;
	BEGIN
		{ top level functions must set up a failure handler }
		IF IsFailure(fi, dp_UnRegisterSection)
			THEN EXIT(dp_UnRegisterSection);

	{$IFC qRangeCheck }
		FailOSErr(dpCheckSection(sectionH));
		FailOSErr(dp_IsRegisteredSection(sectionH));
	{$ENDC }
		FailOSErr(dp_GetCurrentAppRefNum(thisApp));
		
		FailOSErr(dpDeQueueSection(sectionH, thisApp));		{ <26> move dequeue before removePub }
		IF sectionH^^.controlBlock <> NIL 
			THEN FailOSErr(dpRemovePubCBSection(sectionH, thisApp));

		Success(fi);
	END; { dp_UnRegisterSection }
	
	
	
	{-------------  dp_IsRegisteredSection -------------}
	FUNCTION dp_IsRegisteredSection(sectionH: SectionHandle): OSErr;
	VAR
		anAppGlobal:	PerAppGlobalsHandle;
		aSectionH:		SectionHandle;
		fi:				FailInfo;
		
	LABEL 10;  { you didn't see this }
	
	BEGIN
		{ top level functions must set up a failure handler }
		IF IsFailure(fi, dp_IsRegisteredSection)
			THEN EXIT(dp_IsRegisteredSection);

		{ see if NIL was passed in }
		IF sectionH = NIL THEN FailOSErr(notRegisteredSectionErr);
		
		{ walk this apps section list }
		FailOSErr(dp_GetCurrentAppRefNum(anAppGlobal));
		aSectionH := anAppGlobal^^.SectionHandleListHead;

		(*
		{ This is logically what I want to do }
		WHILE (aSectionH <> sectionH) 
			& (aSectionH <> SectionHandle(kNoMoreSections)) DO
		BEGIN
			aSectionH := aSectionH^^.nextSection;
		END;
		
		IF aSectionH = SectionHandle(kNoMoreSections) 
			THEN FailOSErr(notRegisteredSectionErr);
		*)
		
		{ But, doing it this way produces much better code }
		10:
		IF (aSectionH <> sectionH) THEN
		BEGIN
			IF (aSectionH = SectionHandle(kNoMoreSections)) THEN
			BEGIN
				FailOSErr(notRegisteredSectionErr);
			END ELSE
			BEGIN
				aSectionH := aSectionH^^.nextSection;
				GOTO 10;	{ forgive me Dijkstra, for I have sinned }
			END;
		END; {if}
		
		Success(fi);
	END; { dp_IsRegisteredSection }
	
	
	
	{-------------  dp_AssociateSection -------------}
	FUNCTION dp_AssociateSection(sectionH: SectionHandle; newSectionDocument: FSSpecPtr): OSErr;
	VAR
		container:		FSSpec;
		wasChanged:		BOOLEAN;
		fi:				FailInfo;
	BEGIN
		{ top level functions must set up a failure handler }
		IF IsFailure(fi, dp_AssociateSection)
			THEN EXIT(dp_AssociateSection);

	{$IFC qRangeCheck }
		FailOSErr(dpCheckSection(sectionH));
		FailOSErr(dp_IsRegisteredSection(sectionH));
	{$ENDC }
		IF sectionH^^.controlBlock <> NIL THEN
		BEGIN
			{ get container file }
			container := PubCBHandle(sectionH^^.controlBlock)^^.info.container.theFile;
			
			{ update Alias to container from NewDocument }
			FailOSErr(UpdateAlias(newSectionDocument, container, sectionH^^.alias, wasChanged));
		END;
		
		Success(fi);
	END; { dp_AssociateSection }


		
	{-------------  dp_GetEditionInfo -------------}
	FUNCTION dp_GetEditionInfo(sectionH: SectionHandle; 
							VAR theInfo: EditionInfoRecord): OSErr;
	VAR
		thePubCB:		PubCBHandle;
		fi:				FailInfo;
	BEGIN
		{ top level functions must set up a failure handler }
		IF IsFailure(fi, dp_GetEditionInfo)
			THEN EXIT(dp_GetEditionInfo);

	{$IFC qRangeCheck }
		FailOSErr(dpCheckSection(sectionH));
		FailOSErr(dp_IsRegisteredSection(sectionH));
	{$ENDC }
		{ see if there is a pubCB }
		thePubCB := PubCBHandle(sectionH^^.controlBlock);
		IF thePubCB = NIL THEN FailOSErr(fnfErr);
		
		{ make sure control block is up to date (file may have been renamed...)}
		FailOSErr(dpPubSync(thePubCB));

		WITH thePubCB^^ DO
		BEGIN
			theInfo						:= info;
			theInfo.container.thePart	:= kPartsNotUsed;
		END;
	
		Success(fi);
	END; { dp_GetEditionInfo }
	

	{-------------  dp_GotoPublisherSection -------------}
	FUNCTION dp_GotoPublisherSection(container: EditionContainerSpec): OSErr;
	VAR
		thePubCB:				PubCBHandle;
		me:						AppRefNum;
		publisherDocSpec:		FSSpec;
		theGotoInfo:			GotoInfoHandle;
		theSectionID:			LONGINT;
		publisherSectionH: 		SectionHandle;
		publisherApplication:	AppRefNum;
		PBC:					CInfoPBRec;
		appIsRunning:			BOOLEAN;
		ignoreAppIsRunning:		BOOLEAN;
		findErr:				OSErr;
		fi:						FailInfo;
	BEGIN
		{ thePubCB is never in a register because it is a VAR parameter to RequestPubCB }
		thePubCB := NIL;
		
		{ top level functions must set up a failure handler }
		IF IsFailure(fi, dp_GotoPublisherSection) THEN
		BEGIN
			IF thePubCB <> NIL 
				THEN IgnoreOSErr(dpReleasePubCB(thePubCB, me));
			EXIT(dp_GotoPublisherSection);
		END; {if}

		FailOSErr(dp_GetCurrentAppRefNum(me));
		
		{ make sure a container part is not used }
		IF container.thePart <> kPartsNotUsed THEN FailOSErr(badSubPartErr);

		{ get a PubCB for the container file }
		FailOSErr(dpRequestPubCB(container.theFile, me, kCheckExisting, kCanAllocateNew, thePubCB));
		
		IF IsEditionFile(thePubCB^^.info.fdType) THEN
		BEGIN
			{ subscribing to an edition file, so find the publisher for it }
			FailOSErr(dpFindPublisher(thePubCB, {canAskUser}TRUE,				{ <MM, #86192> }
									publisherSectionH, publisherApplication,
									publisherDocSpec, theSectionID));
			IF publisherSectionH <> NIL THEN 
			BEGIN
				{ found a registered section for, so pass go, collect $200 }
				FailOSErr(dp_PostSectionEvent(publisherSectionH, publisherApplication, sectionScrollMsgID));
			END ELSE
			BEGIN
				{ only found the publisher document, so open it }				
				FailOSErr(dpPostOpenDoc(publisherDocSpec, appIsRunning));
				
				{ theSectionID <> 0 => want to wait and send scroll event after doc has opened }
				IF theSectionID <> 0 THEN
				BEGIN
					{ get some info on publisher Document }
					WITH PBC DO
					BEGIN
						ioNamePtr	:= @publisherDocSpec.name;	
						ioVRefNum	:= publisherDocSpec.vRefNum;
						ioFDirIndex	:= 0;
						ioDirID		:= publisherDocSpec.parID;
						FailOSErr(PBGetCatInfoSync(@PBC));
					END; {with}
					
					{ remember to send GotoSection event to app after it registers Publisher }
					theGotoInfo := GotoInfoHandle(NewHandleSys(SizeOf(GotoInfo)));
					FailOSErr(MemError);
					WITH theGotoInfo^^ DO
					BEGIN
						timeOut 			:= TickCount + kMaxGotoWait;
						IF NOT appIsRunning
							THEN timeOut	:= timeOut + kMaxAppLaunchTime;
						publisherDocVRefNum := publisherDocSpec.vRefNum;
						publisherDocID 		:= PBC.ioDirID;
						sectionID 			:= theSectionID;
						editionVRefNum 		:= thePubCB^^.info.container.theFile.vRefNum;
						editionID			:= thePubCB^^.pubCNodeID;
					END; {with}
					dpGetGlobalsLocation^^.gotoHere := theGotoInfo;
				END; {if}
			END; {else}
		END ELSE
		BEGIN
			{ subscribing to a non-edition file, so open it }
			
			{ tell the Finder to "fake double-click" on the document }
			FailOSErr(dpPostOpenDoc(container.theFile, ignoreAppIsRunning));
		END; {else}

		{ release the PubCB }
		FailOSErr(dpReleasePubCB(thePubCB, me));

		Success(fi);
	END; { dp_GotoPublisherSection }
	


