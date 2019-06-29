{	File:		dpEvents.inc.p

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<15>	 2/27/91	ngk		MM, #83903: Moved sending of read events from
	   	   							dp_EditionMgrBackGroundTask to dpPubSync in dpPubIO.inc.p
		<14>	12/14/90	ngk		<VL>Use Sync calls and set ioFVersnum for MFS volumes.
		<13>	  7/2/90	ngk		Added meta-param for interact level to open appleevent.
									PostOpenDoc
	   								now returns if app is running.  Time out of scroll message now tries
									for another publisher.
		<12>	 6/20/90	ngk		change CanonicalFIleSpec to FSSpec
		<11>	 5/31/90	ngk		change globals from a handle to a pointer. Added
									dp_EditionMgrVolumeNotifier.
		<10>	 4/19/90	ngk		Changed 'odoc' to 'open' to when posting to Finder to launch an
									app.
		 <9>	  4/7/90	ngk		Changed backGroundTask to use dpPubSync. Use new Failure
									handling.
		 <8>	 3/20/90	ngk		PostPPCEvent changed to PostHighLevelEvent. Now call
									NewAliasMinimal instead of NewAlias, for open AppleEvent.
		 <7>	 3/20/90	ngk		change to new apple events 7.0a8
		 <6>	 3/10/90	ngk		Get rid of post by PSN kludge in System mode. Moved locking and
									unlocking of package to dispatcher.
		 <5>	 2/16/90	ngk		Fix bug in polling. Used to not check update mode of subscribers
									before sending read events.
		 <4>	  1/8/90	ngk		Changed PostSectionEvent to have NIL toApp mean current
									application.
		 <3>	  1/6/90	ngk		Renamed routines. Moved once only int to dpInit.p
		 <2>	12/19/89	ngk		fix bug in dp_EditionMgrBackGroundTask. Dereferencing NIL goto handle
	   <2.8>	 12/3/89	ngk		Fixed some unsafe dereferences when calling TickCount.
	   <2.7>	11/13/89	ngk		Added Locking and restoring of Pack11 in all public routines
	   <2.6>	11/10/89	ngk		Commented out Begin/EndSytemMode around PostPPCEvent
	   <2.5>	 11/4/89	ngk		Use AppleEvents.p Interface file Changed AppRefNum to be a
									handle to app's globals.
	   <2.4>	10/24/89	ngk		Moved some inlines to dpMisc.p and glue. Fixed post by signature
									bug.
	   <2.3>	10/13/89	ngk		Now intialize gotoHere.
	   <2.2>	10/13/89	ngk		Added goto publisher timeout to dp_EditionMgrBackGroundTask
	   <2.1>	10/10/89	ngk		Now use ProcessSerialNumber and Begin/EndSystemMode
	   <2.0>	 10/2/89	ngk		Changed polling to be called by SystemTask.
	   <1.9>	 9/18/89	ngk		Change dpPostOpenDoc to send an AppleEvent to the Finder, if
									launch needed.
	   <1.8>	  9/7/89	ngk		Changed dp_EditionMgrBackGroundTask to use way cool, efficient polling
	   <1.7>	 8/29/89	ngk		Use qUseAppName with AppRefNum. Changed polling to check all pub
									files which do not have a publisher on this machine.
	   <1.6>	  8/8/89	ngk		Change dp_PostSectionEvent to use classID. Changed dp_EditionMgrBackGroundTask to
									return an OSErr for consistency. Used new new PubCBLinks in
									walking PubCBs.
	   <1.5>	 6/27/89	ngk		Switched to completely use HighLevelEvents for section events
									and open doc events.
	   <1.4>	 6/15/89	ngk		Enhanced dpPostOpenDoc to almost use HighLevelEvents still have
									HighLevelEvents stuff commented out
	   <1.3>	 6/11/89	ngk		Tried to use HighLevelEvents instead of pseudoEvents
	   <1.2>	 5/31/89	ngk		Changed to use Alias manager instead of SofaLinks
	   <1.1>	 5/29/89	ngk		Added dpPostOpenDoc for GotoPublisher
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}

TYPE
	SectionEventMessage = 	RECORD
									header:			AETFHeader;		{ 'aevt', 1, 1 }
									endOfMetaData:	Keyword;		{ always ';;;;' }
									directObjKey:	KeyWord;		{ always '----' }
									dataType:		ParamType;		{ always 'tid ' }
									dataLength:		LONGINT;		{ always 4 }
									tempIDType:		ResType;		{ always 'sect' }
									tempID:			LONGINT;		{ always SectionHandle }
								END;

	SimpleOpenMessage = 	RECORD
									header:			AETFHeader;		{ 'aevt', 1, 1 }
									metaKey:		KeyWord;		{ always 'inte' }
									metaType:		ParamType;		{ always 'enum' }
									metaLength:		LONGINT;		{ always 4 }
									metaData:		ResType;		{ always 'cans' }
									endOfMetaData:	Keyword;		{ always ';;;;' }
									directObjKey:	KeyWord;		{ always '----' }
									dataType:		ParamType;		{ always 'alis' }
									dataLength:		LONGINT;		{ always ?? }
									data:			SignedByte;		{ beginning of alias }
								END;
								



	{-------------  dp_PostSectionEvent -------------}
	FUNCTION dp_PostSectionEvent(sectionH: SectionHandle; toApp: AppRefNum; messageID: ResType): OSErr;
	VAR
		anEvent:		HighLevelEventRecord;
		aSectionMsg:	SectionEventMessage;
		inSystemMode:	BOOLEAN;
		ignore:			OSErr;
		fi:				FailInfo;
	BEGIN
		{ set up failure handling }
		IF isFailure(fi, dp_PostSectionEvent) THEN
		BEGIN
			IF inSystemMode
				THEN ignore := EndSystemMode;
			EXIT(dp_PostSectionEvent);
		END; {if}

		{ map NIL to this App }
		IF toApp = NIL  
			THEN FailOSErr(dp_GetCurrentAppRefNum(toApp));
		
		WITH anEvent DO
		BEGIN
			class	:= 	sectionEventMsgClass;
			ID		:= 	messageID;
		END; {with}

		WITH aSectionMsg DO
		BEGIN
			header.signature 		:= LONGINT('aevt');
			header.majorVersion		:= 1;
			header.minorVersion		:= 1;
			endOfMetaData 			:= aeEndOfMetaDataKeyword;
			directObjKey 			:= aeDirectObjectKeyword;
			dataType				:= aeTemporaryIDParamType;
			dataLength				:= 8;
			tempIDType				:= rSectionType;
			tempID					:= ORD(sectionH);
		END; {with}
		
		FailOSErr(BeginSystemMode);	inSystemMode := TRUE;
		dp_PostSectionEvent := PostHighLevelEvent(EventRecord(anEvent), @toApp^^.appPSN, {refCon}0, 
												@aSectionMsg, SizeOf(aSectionMsg), receiverIDisPSN);
		FailOSErr(EndSystemMode);	inSystemMode := FALSE;

		Success(fi);
	END; { dp_PostSectionEvent }



	FUNCTION GetVolModDate(vRefNum: INTEGER): TimeStamp;
	TYPE
		VCBPtr	=	^VCB;
	VAR
		p:	VCBPtr;
	BEGIN
		p := VCBPtr(GetVCBQHdr^.qHead);
		WHILE (p<>NIL) & (p^.vcbVRefNum <> vRefNum)
			DO p := VCBPtr(p^.qLink);
		IF p <> NIL
			THEN GetVolModDate := p^.vcbLsMod
		{$IFC qCatchFailures }
			ELSE DebugStr('GetVolModDate: could not find vRefNum');
		{$ENDC}
			;
	END; { GetVolModDate }


	FUNCTION GetDirModDate(vRefNum: INTEGER; dirID: LONGINT): TimeStamp;
	VAR
		PBC:	CInfoPBRec;
	BEGIN
		WITH PBC DO
		BEGIN
			ioNamePtr	:= NIL;
			ioVRefNum	:= vRefNum;
			ioFDirIndex	:= -1;
			ioFVersNum	:= 0;
			ioDrDirID	:= dirID;
		END; {with}
		IF PBGetCatInfoSync(@PBC) = noErr
			THEN GetDirModDate := PBC.ioDrMdDat
		{$IFC qCatchFailures }
			ELSE DebugStr('GetDirModDate: PBGetCatInfo returned an error');
		{$ENDC}
			;
	END; { GetDirModDate }


	PROCEDURE dpTimeOutGoto;
	VAR
		editionVol:		INTEGER;
		editionCNode:	LONGINT;
		publisherID:	LONGINT;
		pubSectionH:	SectionHandle;
		pubApp:			AppRefNum;
		
		PROCEDURE FindAPublisher(sectionH: SectionHandle; inApp: AppRefNum);
		BEGIN
			WITH sectionH^^ DO
			BEGIN
				IF publisherID = sectionID 
				{&} THEN IF bTST(kind, kCanWriteEditionsBIt) THEN
				BEGIN
					WITH PubCBHandle(controlBlock)^^ DO
					BEGIN
						IF info.container.theFile.vRefNum = editionVol 
						 {&} THEN IF pubCNodeID = editionCNode THEN
						BEGIN
							pubSectionH := sectionH;
							pubApp := inApp;
						END;
					END; {with}
				END; {if}
			END; {with}
		END; { FindAPublisher }
		
	BEGIN
		pubSectionH := NIL;
		WITH dpGetGlobalsLocation^^ DO
		BEGIN
			{ remember pertainent goto info }
			WITH gotoHere^^ DO
			BEGIN
				editionCNode	:= editionID;
				editionVol		:= editionVRefNum;
				publisherID		:= sectionID;
			END; {with}
			{ dispose of goto info }
			gotoHere := NIL;
			DisposHandle(Handle(gotoHere));
		END; {with}
		
		{ scan through registered sections. remember the last publisher to the edition } 
		dpForEachSectionDo(FindAPublisher);
		 
		{ a publisher to correct edition is registered, send it a goto event}
		IF pubSectionH <> NIL THEN 
			IF dp_PostSectionEvent(pubSectionH, pubApp, sectionScrollMsgID) <> noErr THEN;
		
	END; { dpTimeOutGoto }


{
dp_EditionMgrBackGroundTask does two major things. 1) see if any shared edition files
have changed (i.e file on AppleShare volume). 2) close any edition files
files that are open for use by subscribers, and none of them are currently reading.

method:

Walk PubCB list.  It is known to be sorted by volume and then by dirID.
Look ahead to next file.

If this one and next one are both on the same volume, then look in the VCB
for the lastModeDate. If the last mod date is the same as this.lastVolMod
then skip the polling for all pubCBs with this volume.  If the dates are
different then when polling, set each PubCBs lastVolMod to be the new date.

If this one and next one are both in the same folder, then do a GetCatInfo
on that folder.  If the folder mod date is the same as this.lastDirMod then
skip the polling for all pubCBs in this folder.  If the dates are
different then when polling, set each PubCBs lastDirMod to be the new date.

If a PubCB has no publisher & is open & no subscribers are doing I/O
then close the file.  This is part of the slop of keeping files open
for multiple subscribers - for efficiency.


### there is currently a bug when this is used with EditionOpenerProc's.
The smart polling will not work with non-edition files if they are not
saved with a "safe save" (write new file, rename, delete).  This is because
an updated file will not cause the folder mod date to be touched.  Thus,
this polling will miss those files if there is more than one in the same
folder.

}


	{-------------  dp_EditionMgrBackGroundTask -------------}
	FUNCTION dp_EditionMgrBackGroundTask: OSErr;
	VAR
		emGlobals:		EdtnMgrGlobalsPtr;
		headPubCB:		PubCBLinkHandle;
		aPubCB:			PubCBHandle;
		PBH:			HParamBlockRec;
		ignore:			OSErr;
		lookAHeadPubCB:	PubCBHandle;
		thisVolMod:		TimeStamp;
		thisVol:		INTEGER;
		skipThisVol:	Boolean;
		thisDirMod:		TimeStamp;
		thisDir:		INTEGER;
		skipThisDir:	BOOLEAN;
		validDirMod:	BOOLEAN;
		theGoto:		GotoInfoHandle;
		curTickCount:	LONGINT;
		fi:				FailInfo;

	BEGIN
		{ must have failure handler }
		IF isFailure(fi, dp_EditionMgrBackGroundTask) THEN
		BEGIN
			EXIT(dp_EditionMgrBackGroundTask);
		END; {if}

		emGlobals := dpGetGlobalsLocation^;

		{ walk PubCB list }
		thisVol := 1000;
		FailOSErr(dpGetPubCBListHeadNode(headPubCB));
		aPubCB := PubCBHandle(headPubCB^^.nextPubCB);
		WHILE PubCBLinkHandle(aPubCB) <> headPubCB DO
		BEGIN
			lookAHeadPubCB := PubCBHandle(PubCBLinkHandle(aPubCB)^^.nextPubCB);

			{ see if we've walked into another volume }
			IF thisVol <> aPubCB^^.info.container.theFile.vRefNum THEN
			BEGIN
				{ we have a different volume, see if  }
				{ anything has changed since we last looked. }
				{ note: because GetVolModDate is a cheap call }
				{ its OK to call it even if there is only one }
				{ file we care about on the volume. }
				thisVol := aPubCB^^.info.container.theFile.vRefNum;
				thisDir := 0;
				thisDirMod := 0;
				thisVolMod := GetVolModDate(thisVol);
				skipThisVol := (thisVolMod = aPubCB^^.lastVolMod); { nothing changed }
			END; {if different volume }

			IF NOT skipThisVol THEN
			BEGIN
				{ something on thisVol has changed, }
				{ so see if it was a file we care about }

				{ see if we've walked into another folder }
				IF thisDir <> aPubCB^^.info.container.theFile.parID THEN
				BEGIN
					{ we have a different folder, see if there are }
					{ multiple edition files in it }
					thisDir := aPubCB^^.info.container.theFile.parID;
					IF ( lookAHeadPubCB <> PubCBHandle(headPubCB) )
						AND (lookAHeadPubCB^^.info.container.theFile.parID = thisDir) THEN
					BEGIN
						{ at least two files in this folder, }
						{ so check if anything in the folder has changed }
						thisDirMod := GetDirModDate(thisVol, thisDir);
						skipThisDir := (thisDirMod = aPubCB^^.lastDirMod); { nothing changed }
					END ELSE
					BEGIN
						{ only one file in this folder }
						{ no need to check folder mod date }
						thisDirMod := 0;
						skipThisDir := FALSE;
					END;
				END;

				IF NOT skipThisDir THEN
				BEGIN
					{ something in thisDir has changed, so look at each file we care about }

					{ make sure control block has lastest data }
					IgnoreOSErr(dpPubSync(aPubCB));
					{ <15, #83903>  Removed call to dpNotifySubscribers from here. }
					{ <15, #83903>  It is now done in dpPubSync. }
				END; {if NOT skipThisDir}

			END; {if NOT skipThisVol}

			WITH aPubCB^^ DO
			BEGIN
				{ make sure every pubCB has the correct last mode date }
				{ for its folder and volume }
				{aPubCB^^.}lastVolMod := thisVolMod;
				IF thisDirMod <> 0 
					THEN {aPubCB^^.}lastDirMod := thisDirMod;

				{ if no subscribers are doing I/O and file is open, close it }
				IF {aPubCB^^.}publisherCount = 0 							{ just subscribers }
				{&} THEN IF {aPubCB^^.}openMode <> dmNotOpen 				{ file is open }
				{&} THEN IF {aPubCB^^.}usageInfo^^.totalIOCount = 0 THEN	{ no I/O }
				BEGIN
					 ignore := dpPubCloseFile(aPubCB, NOT kFlush);
				END;
			END; {with}
			
			aPubCB := lookAHeadPubCB;
		END; {while}

		{ check goto timeout }
		theGoto := emGlobals^.gotoHere;
		curTickCount := TickCount;
		IF (theGoto <> NIL) {&} THEN IF (theGoto^^.timeout > curTickCount) THEN
		BEGIN
			{ looks like publisher will never register, look to see if it already is registered }
			dpTimeOutGoto;
		END; {if}

		{ set next time to poll }
		emGlobals^.nextPollTime := curTickCount+ kPollPeriod;

		Success(fi);
	END; { dp_EditionMgrBackGroundTask }


	{-------------  dp_EditionMgrVolumeNotifier -------------}
	{$PUSH} {$Z+} { export this routine, without requiring every other unit to know about VolumeNoticeBlkPtr }
	FUNCTION dp_EditionMgrVolumeNotifier(pb: VolumeNoticeBlkPtr): OSErr;
	{$POP}
	VAR
		headPubCB:		PubCBLinkHandle;
		aPubCB:			PubCBHandle;
	BEGIN
		IF pb^.VNBNotice = VNAboutToUnmount THEN
		BEGIN
			{ walk pub control block list, if any editions are in use }
			{ on the volume that is about to unmount, then stop it! }
			IF dpGetPubCBListHeadNode(headPubCB) = noErr THEN
			BEGIN
				aPubCB := PubCBHandle(headPubCB^^.nextPubCB);
				WHILE PubCBLinkHandle(aPubCB) <> headPubCB DO
				BEGIN
					WITH aPubCB^^.info.container.theFile, pb^ DO
					BEGIN
						IF vRefNum = VNBVolume THEN
						BEGIN	
							{ tell Process Mgr to not take this volume away }
							dp_EditionMgrVolumeNotifier := fBsyErr;
							EXIT(dp_EditionMgrVolumeNotifier);
						END;
					END; {with}
					aPubCB := PubCBHandle(PubCBLinkHandle(aPubCB)^^.nextPubCB);
				END; {while}		
			END; {if}
		END; {if}
	
		dp_EditionMgrVolumeNotifier := noErr;
	END; { dp_EditionMgrVolumeNotifier }
	


	{-------------  dpPostOpenDoc -------------}
	FUNCTION dpPostOpenDoc(document: FSSpec; VAR appIsRunning: BOOLEAN): OSErr;
	VAR
		info:			FInfo;
		anAppGlobal:	PerAppGlobalsHandle;
		emGlobals:		EdtnMgrGlobalsPtr;
		thePSN:			ProcessSerialNumber;
		anEvent:		HighLevelEventRecord;
		inSystemMode:	BOOLEAN;
		appRunning:		BOOLEAN;
		thisApp: 		AppRefNum;
		aevtOpenPtr:	^SimpleOpenMessage;
		aevtOpenLen:	Size;
		msgBuff:		ARRAY [1..500] OF INTEGER; { enough room for biggest alias }
		aliasH:			AliasHandle;
		finderSignature:OSType;
		fi:				FailInfo; 
		ignore:			OSErr;
	BEGIN
		inSystemMode := FALSE;

		{ set up failure handling }
		IF isFailure(fi, dpPostOpenDoc) THEN
		BEGIN
			IF inSystemMode
				THEN ignore := EndSystemMode;
			EXIT(dpPostOpenDoc);
		END; {if}

		{ get docs creator }
		FailOSErr(FSpGetFInfo(document, info));
	
		{ see if it is a running app }
		emGlobals := dpGetGlobalsLocation^;
		IF emGlobals = NIL 
			THEN FailOSErr(editionMgrInitErr);

		anAppGlobal := emGlobals^.perAppListHead;
		appRunning := FALSE;
		WHILE anAppGlobal <> NIL DO
		BEGIN
			WITH anAppGlobal^^ DO
			BEGIN
				IF LONGINT({anAppGlobal^^.}signature) = LONGINT(info.fdCreator) THEN
				BEGIN
					appRunning := TRUE;
					thePSN := {anAppGlobal^^.}appPSN;
					LEAVE; {while}
				END;
				anAppGlobal := {anAppGlobal^^.}nextApp;
			END; {with}
		END; {while}
		
		{ make a minimal alias to the document }
		FailOSErr(NewAliasMinimal(document, aliasH));

		{ fill out apple event for opening document }
		aevtOpenPtr := @msgBuff;
		WITH aevtOpenPtr^ DO
		BEGIN
			header.signature 		:= LONGINT('aevt');
			header.majorVersion		:= 1;
			header.minorVersion		:= 1;
			metaKey					:= 'inte';
			metaType				:= 'enum';
			metaLength				:= 4;
			metaData				:= 'cans';
			endOfMetaData 			:= aeEndOfMetaDataKeyword;
			directObjKey 			:= aeDirectObjectKeyword;
			dataType				:= aeAliasParamType;
			dataLength				:= aliasH^^.aliasSize;
			BlockMove(Ptr(aliasH^), @data, dataLength);
			
			{ -2 is kludge to take .alias field off the end of AppleEventSimpleMessage.directObjectFile }
			aevtOpenLen := SizeOf(SimpleOpenMessage) - 2 + dataLength;
		END; {with}
		
		{ now that we've copied the alias into the buffer, we can lose it }
		DisposHandle(Handle(aliasH));
		
		WITH anEvent DO
		BEGIN
			class	:= standardAppleEventMsgClass;
			ID		:= 'odoc';
		END; {with}

		IF appRunning THEN
		BEGIN
			{ send open doc message directly to app }
			FailOSErr(BeginSystemMode);		inSystemMode := TRUE;
			FailOSErr(PostHighLevelEvent(EventRecord(anEvent), @thePSN, 0,
									Ptr(aevtOpenPtr), aevtOpenLen, receiverIDisPSN));
			FailOSErr(EndSystemMode);		inSystemMode := FALSE;
		END ELSE
		BEGIN
			{ app needs to be launched, send open event to Finder }
			finderSignature := 'MACS';
			anEvent.ID := 'open';	{ finder wants open instead of odoc }
			FailOSErr(BeginSystemMode);		inSystemMode := TRUE;
			FailOSErr(PostHighLevelEvent(EventRecord(anEvent), Ptr(finderSignature), 0,
									Ptr(aevtOpenPtr), aevtOpenLen, receiverIDisSIGNATURE));
			FailOSErr(EndSystemMode);		inSystemMode := FALSE;
		END; {if}

		appIsRunning := appRunning;
		
		Success(fi);
	END; { dpPostOpenDoc }


