{
	File:		dpPubIO.inc.p

	Written by:	Nick Kledzik
	
	Copyright:	© 1989-1990, 1992 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<40>	 1/20/92	ngk		Fix bug in dpPubCloseFile. It was not switching to system mode
									before closing the file. This is now required in CubeE.
		<39>	  4/5/91	ngk		just added a comment about FindPublisher
		<38>	  4/3/91	ngk		MM,#c1-MTM-007: Check for nil controlblock in OpenNewEdition.
		<37>	 3/22/91	ngk		VL, #b6-ngk-005: check MemErr after SetHandleSize.
	   								Add missing Success calls.  
		<36>	 2/27/91	ngk		MM,#83903: Move sending of read events to pubSync
	   								from dp_EditionMgrBackGroundTask in dpEvents.inc.p
		<35>	 1/25/91	ngk		VL, #?????: Fix DeleteEditionContainer to not reference a
									control block after releasing it.
		<34>	 1/14/91	ngk		<MM> Fix use count problem in dp_DeleteEditionContainerFile
		<33>	12/14/90	ngk		<KSM>Fix setting of type & creator til after copying of other
									Finder info.
		<32>	12/14/90	ngk		<MM>Use new PBxxxSync and FSpXxx calls. Changed dpFindPublisher
	   								to tell app to open edition if edition does not contain an alias.
									Fix for "throw me away" bug, by retrying delete.  Fixed some
									failure handlers that were not cleaning up properly.   Shrunk 
									window for throw me aways to be made by change the subscriber lock
									strategy to try to read before retring lock - this distingushes 
									between the publisher lock and another subscriber lock.  Change
									Sync to only try FCBInfo if GetCatInfo failed - this prevents syncing
									to temp file on FileShare during exchange files.  Clear Inited bit
									if overwriting non-edition file.  Don't require edition to have been
									open during SwapAndDelete.  
		<31>	 12/5/90	dba		<dnf> Get rid of conflicts of bit definitions in here with new
									global definitions in Files.p.
		<30>	10/30/90	ngk		Don't track editions into trash. In PubClose, now free maps.
	   								Remove sync param from PBDTGetPath.
		<29>	 9/21/90	ngk		DC NIL ioNamePtr when calling PBDTGetPath
		<28>	 9/15/90	ngk		Fixed memory leak in dpResolveAliasToPublisher if edition
	   								does not contain an alias.
		<27>	 9/13/90	ngk		Changed file type for edition files to be all lowercase.
		<26>	 8/28/90	dnf		Changed references to ResolveFileID into references to
									ResolveFileIDRef.
		<25>	 8/26/90	ngk		Another fix to GetStandardFormats. It was decrementing the
	   								usage count one too many if it encountered an error.
		<24>	 8/16/90	ngk		In sync, if file is missing, then mark so in control block.
	   								In GetStandardFormats, on error remember to close edition.  
									Now handle case of bad verb to standard opener by return paramErr.  
									Fix createEditionFile to not set script if it is smSystemScript(-1).  
		<23>	  8/4/90	ngk		fix createEditionFile to have type edtu.
	    <22>	 7/31/90	ngk		fix throw me away files.  Fix register usage with failure handling.
		<21>	 7/14/90	ngk		Fix sync of MFS floppies. Changed more bAND's to bTST's.
	   								Put hack in to work around GetVolParms bug.
									Fix dpStandardSubscriberClose when sectionH=NIL to dec use count.
									Fix dpStandardSubscriberClose to not close file if it was open for writing.
									Fix dpGetStandardFormats to release thePubCB on error.
		<20>	  7/2/90	ngk		need to release pubcb in delete edition
		<19>	  7/2/90	ngk		bAND to bTST. Rework dpResolveAliasToPublisher and
									dpFindPublisher.
	   								Changed HDelete to FSpDelete.  Did a bunch of code saving munges.
									DeleteEditionFile stops all I/O with subscribers and only deletes
									the file if there are no register publishers.
		<18>	 6/20/90	ngk		Changed CanonicalFileSpec to FSSpec.
		<17>	  6/1/90	ngk		Add 'snd ' to GetStandardFormats and established a priority
	   								for choosing the preview format.
		<16>	 5/31/90	ngk		Added new edition file types and made dpStandardCanSubscribe a
									routine.
		<15>	  5/3/90	ngk		Added ResolveAliasToPublisher routine.
		<14>	  4/7/90	ngk		Use dynamic arrays for allocation and format lists. Changed
	   								dpPubSetInfo to dpPubSync.  Use RangeLocking to avoid "Throw me
									away" files.  Use new FailOSErr stuff.
		<13>	 3/20/90	ngk		Added script parameter to dp_CreateEditionContainerFile
		<12>	 3/16/90	dnf		Change DTOpen to DTGetPath
		<12>	 3/16/90	dnf		Change DTOpen to DTGetPath
		<11>	 3/10/90	ngk		Use bHasFileIDs attribute of VolParms.  Removed dpGetVolumeInfo
									and call GetVolParms directly.  Moved locking and unlocking of
									package to dispatcher.
	   								dpGetVolumeInfo and call GetVolParms directly.
									Moved locking and unlocking of package to dispatcher.
		<10>	 2/26/90	ngk		When previewing TEXT, only read in first 500 bytes.
		 <9>	 2/25/90	ngk		Fix replace-existing non-edition file bug.
		 <8>	 2/16/90	ngk		Publisher^^.mdDate is used to set the edition File's mdDate.
									Fixed bug where edition's create date kepts changing.
		 <7>	  2/4/90	ngk		Fix bug header.formatLength of edition files. Allow
									OpenNewEdition to have NIL sectionDocument. Get mdDate for
									newEditions from section record. Now use PBDT calls to exchange
									edition comments.
		 <6>	 1/26/90	ngk		Fixed calls to Alias manager to use new non-pointer style
		 <5>	 1/22/90	ngk		Fix standard formatIO routine to use symbolic name. Changed
									publisher to OpenDenyOtherWriters.
		 <4>	  1/8/90	ngk		Renamed GetAppRefNum
		<2+>	  1/6/90	ngk		Changed name of GetAppRefNum,removed eoCanPublish
		 <2>	  1/6/90	ngk		Convert to BBS, removed wakeUpTime. Added bottleneck routines.
									renamed some routines.
	  	<2.3>	11/13/89	ngk		Added Locking and restoring of Pack11 in all public routines
		<2.2>	11/04/89	ngk 	Removed call to PBHCreateID in NewPublicationFile
									Changed AppRefNum to be a handle to app's globals.
		<2.1>	10/25/89	ngk 	Moved some inline code out.
		<2.0>	10/13/89	ngk		nothing
		<1.9>	10/02/89	ngk 	added wakeUpTime
		<1.8>	09/18/89	ngk 	Added dp_DeleteEditionContainerFile
		<1.7>	09/08/89	ngk		Removed call to CreateID in NewPublication
		<1.6>	09/07/89	ngk 	Changed DisposeAlias to DisposHandle
		<1.5>	08/29/89	ngk 	Changed dpGetVolumeInfo to return VolumeServices. Changed pub open 
									strategy to always have pub files open for read/write if a 
									publisher is registered and Subscribers leave the file open for
									read-only. dpBackGroundTask will close read-only pub files if
									no Subscribers are doing I/O.  Seperated the loading of allocMap
									and formats into dpPubLoadMap and dpPubReleaseMap.  Stop using fileIDs.
		<1.4>	08/08/89	ngk 	Split dp_CloseEdition into closing after reading and after writing
									Used new error codes. qDebug -> qCatchFailures & qRangeCheck
									Started using new FileID calls.
		<1.3>	06/11/89	ngk		Changed file format to include a separate foramt list and allocation table
									changed dpGetPubInfo to dpSetPubInfo
									made interface to dpWriteFormat & dpReadFormat consistent
		<1.2>	05/31/89	ngk		Changed SofaLink usage to Aliases
		<1.1>	05/29/89	ngk 	Removed VirtualOpen and VirtualClose. Changed AccessMode to my own
									enumerated type. Added WriteFormat and ReadFormat to allow I/O
									with a Section I/O Control Block, as needed for the "pointer" from 
									the Publication file back to the Publisher document.
		<1.0>	05/19/89	ngk		Submitted for first time

	To Do:
}


{$IFC UNDEFINED qExtraDebug }		
	{$SETC qExtraDebug = FALSE}
{$ENDC}

{========================== low level ========================}
	
	CONST
		kThrowMeAwayName 			= 'throw me away';
		kTicksBetweenDeleteRetries	= 20;
		kMaxDeleteRetries			= 10;
		kTicksUntilOpenRetry		= 10;
		
	TYPE
		VolumeParms	=	RECORD
								vMVersion:		INTEGER;
								vMAttrib:		LONGINT;
								vMLocalHand:	LONGINT;
								vMServerAdr:	LONGINT;
								{vMVolumeGrade:	LONGINT;}
								{vMAltPrivModel:INTEGER;}
							END;


{========================== internal routines ========================}


	{ this will be eventually moved into File system }
	FUNCTION FSpOpenDeny(spec: FSSpec; denyModes: INTEGER; VAR refNum: INTEGER): OSErr;
	VAR
		PBH:	HParamBlockRec;
	BEGIN
		WITH PBH DO
		BEGIN
			PBH.ioVRefNum	:= spec.vRefNum;
			PBH.ioDirID		:= spec.parID;
			PBH.ioNamePtr	:= @spec.name;
			PBH.ioVersNum	:= 0;
			PBH.ioMisc		:= NIL;
			PBH.ioDenyModes	:= denyModes;
			FSpOpenDeny 	:= PBHOpenDenySync(@PBH);
			refNum 			:= ioRefNum;
		END; {with}
	END; { FSpOpenDeny }
		
	
	{-------------  dpNotifySubscribers -------------} 
	FUNCTION dpNotifySubscribers(thePubCB: PubCBHandle): OSErr;
	
		PROCEDURE NotifyIfOutOfDateSubscriber(aSection: SectionHandle; inApp: AppRefNum);
		BEGIN
			WITH aSection^^ DO
			BEGIN
				IF 				controlBlock = Handle(thePubCB)
					{&} THEN IF bTST(kind, kCanReadEditionsBit) 
					{&} THEN IF mode = sumAutomatic
					{&} THEN IF mdDate <> thePubCB^^.info.mdDate THEN
				BEGIN
					IgnoreOSErr(dp_PostSectionEvent(aSection, inApp, sectionReadMsgID));
				END; {if}
			END; {with}
		END; { NotifyIfOutOfDateSubscriber }
		
	BEGIN
		dpForEachSectionDo(NotifyIfOutOfDateSubscriber);	
		dpNotifySubscribers := noErr;
	END; { dpNotifySubscribers }
	
	
	
	{ makes sure a controlblock's fields are up to date with the file system world }
	{-------------  dpPubSync -------------} 
	FUNCTION dpPubSync(thePubCB: PubCBHandle): OSErr;
	TYPE
		VCBPtr	=	^VCB;
	VAR
		PBH:		HParamBlockRec;
		PBF:		FCBPBRec;
		VP:			VolumeParms;
		theInfo:	EditionInfoRecord;
		infoErr:	OSErr;
		tempStr:	Str63;
		tempThrow:	Str63;
		p:			VCBPtr;
		trashVRefNum:INTEGER;
		trashDirID:	LONGINT;
	BEGIN 
		WITH thePubCB^^ DO
		BEGIN
			BlockMove(@{thePubCB^^.}info, @theInfo, SizeOf(theInfo)); { theInfo := thePubCB^^.info; }
		
			{ PBHGetVolParms only called once }
			IF {thePubCB^^.}volumeInfo = 0 THEN
			BEGIN
				{ get volume attributes }
				WITH PBH DO
				BEGIN
					ioVRefNum	:= theInfo.container.theFile.vRefNum;
					ioNamePtr	:= NIL;
					ioBuffer	:= @VP;
					ioReqCount	:= SizeOf(VP);
				END; {with}
				
				{ If it fails, then volume has no attributes }
				IF PBHGetVolParmsSync(@PBH) = noErr
					THEN {thePubCB^^.}volumeInfo := VP.vMAttrib;
				
				{### super hack to get around bug that MFS volumes return bHasFileIDs ###}
				p := VCBPtr(GetVCBQHdr^.qHead);
				WHILE (p<>NIL) & (p^.vcbVRefNum <> theInfo.container.theFile.vRefNum)
					DO p := VCBPtr(p^.qLink);
				IF (p <> NIL) & (p^.vcbSigWord = $D2D7 ) 
					THEN bClr(thePubCB^^.volumeInfo, bHasFileIDs);
				{### end of super hack ###}
			END; {if}
	
			{ get edition, create date, and creator }
			PBH.ioNamePtr	:= @theInfo.container.theFile.name;
			PBH.ioVRefNum	:= theInfo.container.theFile.vRefNum;
			PBH.ioFDirIndex	:= 0;
			PBH.ioFVersNum	:= 0;
			PBH.ioDirID		:= theInfo.container.theFile.parID;
			infoErr := PBGetCatInfoSync(@PBH);		
			
			IF infoErr = fnfErr THEN
			BEGIN
				{ if file is known to be missing no need to try any harder }
				IF {thePubCB^^.}fileMissing THEN
				BEGIN
					dpPubSync := noErr;
					EXIT(dpPubSync);
				END; {if}
		
				{ if file is open use FCB to track file }
				IF {thePubCB^^.}fileRefNum <> kClosedFileRefNum THEN
				BEGIN
					{ file is open, can track down access path to get info }
					PBF.ioNamePtr	:= @tempStr;
					PBF.ioRefNum	:= {thePubCB^^.}fileRefNum;
					PBF.ioFCBIndx	:= 0;
					IF PBGetFCBInfoSync(@PBF) = noErr 
					 {&} THEN IF PBF.ioFCBParID <> 0 THEN { only do this for HFS volumes }
					BEGIN
						{ don't sync to files that start with 'throw me away' }
						tempThrow := kThrowMeAwayName;
						IF NOT dpSameBytes(@tempStr[1], @tempThrow[1], LENGTH(tempThrow)) THEN
						BEGIN
							BlockMove(@tempStr, @theInfo.container.theFile.name, SizeOf(Str63));
							theInfo.container.theFile.parID := PBF.ioFCBParID;
							PBH.ioNamePtr	:= @theInfo.container.theFile.name;			{ <36, #83903> }
							PBH.ioVRefNum	:= theInfo.container.theFile.vRefNum;		{ <36, #83903> }
							PBH.ioFDirIndex	:= 0;										{ <36, #83903> }
							PBH.ioDirID		:= theInfo.container.theFile.parID;			{ <36, #83903> }
							infoErr := PBGetCatInfoSync(@PBH);							{ <36, #83903> }
						END; {if}
					END; {if}
				END; {if}
			END; {if}
			
			IF infoErr <> noErr THEN
			BEGIN
				{ file was renamed or moved, try looking up by cNodeID }
				IF bTST({thePubCB^^.}volumeInfo, bHasFileIDs) THEN
				BEGIN
					PBH.ioVRefNum	:= theInfo.container.theFile.vRefNum;
					PBH.ioFileID	:= {thePubCB^^.}pubCNodeID;
					PBH.ioNamePtr	:= @theInfo.container.theFile.name;
					IF PBResolveFileIDRefSync(@PBH) = noErr THEN
					BEGIN
						{ make sure that it was not moved into the trash }
						IF FindFolder(theInfo.container.theFile.vRefNum,{vRefNum}
									kTrashFolderType,					{folderType}
									FALSE,								{createFolder}
									trashVRefNum,						{foundVRefNum}
									trashDirID)	= noErr	THEN			{foundDirID}
						BEGIN
							IF PBH.ioDirID <> trashDirID THEN
							BEGIN
								{ if found, but not in trash, then sync to new location }
								theInfo.container.theFile.parID := PBH.ioSrcDirID;
								PBH.ioNamePtr	:= @theInfo.container.theFile.name;
								PBH.ioVRefNum	:= theInfo.container.theFile.vRefNum;
								PBH.ioFDirIndex	:= 0;
								PBH.ioDirID		:= theInfo.container.theFile.parID;
								infoErr := PBGetCatInfoSync(@PBH);	
							END 
							ELSE infoErr := fnfErr;
						END; {if}
					END; {if}
				END; {if}	
			END; {if}	
			
			{ files can not be found so mark it so }
			IF infoErr = fnfErr
				THEN {thePubCB^^.}fileMissing := TRUE;
			
			IF infoErr = noErr THEN
			BEGIN		
				{ update control block }
				BlockMove(@theInfo.container.theFile, @info.container.theFile, SizeOf(FSSpec)); { info.container.theFile := theInfo.container.theFile; }
				info.crDate		:= PBH.ioFlCrDat;
				info.fdCreator	:= PBH.ioFlFndrInfo.fdCreator;
				info.fdType		:= PBH.ioFlFndrInfo.fdType;
				pubCNodeID 		:= PBH.ioDirID;
				
				{ file definitely exists now. This reverses 'fileMissing' if file should reappear }
				{thePubCB^^.}fileMissing := FALSE;
				
				{ Whenever the control block's modDate needs to be updated, read events need to be sent }
				IF info.mdDate <> PBH.ioFlMdDat THEN				{ <36, #83903> }
				BEGIN												{ <36, #83903> }
					info.mdDate	:= PBH.ioFlMdDat;					{ <36, #83903> }
					IgnoreOSErr(dpNotifySubscribers(thePubCB));		{ <36, #83903> }
				END;												{ <36, #83903> }
			END; {if}		
						
		END; {with}
		dpPubSync := infoErr;
	END; { dpPubSync }

	

	{-------------  dpPubOpenFile -------------} 
	FUNCTION dpPubOpenFile(thePubCB: PubCBHandle; kind: SectionType): OSErr;
	VAR
		theRefNum:		INTEGER;
		pubFile:		FSSpec;
		transPerm:		INTEGER;
		hfsPerm:		INTEGER;
		orgOpenMode:	INTEGER;
		PBH:			HParamBlockRec;
		inSystemMode:	BOOLEAN;
		anErr:			OSErr;
		ignoreLong:		LONGINT;
		fi:				FailInfo;
	BEGIN 
		DoNotPutInRegister(@inSystemMode);
		DoNotPutInRegister(@orgOpenMode);
		inSystemMode := FALSE;
		theRefNum 	:= kClosedFileRefNum;
		orgOpenMode	:= thePubCB^^.openMode;
		pubFile 	:= thePubCB^^.info.container.theFile;
	
		{ set up failure handling }
		IF isFailure(fi, dpPubOpenFile) THEN
		BEGIN
			IF inSystemMode THEN IgnoreOSErr(EndSystemMode);
			WITH thePubCB^^ DO
			BEGIN
				fileRefNum	:=	kClosedFileRefNum;
				openMode	:=	dmNotOpen;
			END; {with}
			IF orgOpenMode <> dmNotOpen THEN
			BEGIN
				{$IFC qExtraDebug }		
				DebugStr('dpPubOpenFile: Failing and re-opening file');
				{$ENDC }
				{ failed opening for more permssions, so reopen with less }
				CASE orgOpenMode OF
					dmRequestReadPerm:
						IgnoreOSErr(dpPubOpenFile(thePubCB, stSubscriber));
					dmRequestReadPerm+dmRequestWritePerm+dmDenyOtherWriters:
						IgnoreOSErr(dpPubOpenFile(thePubCB, stSubscriber));
				END; {case}
			END; {if}
			EXIT(dpPubOpenFile);
		END; {if}
		
		{ do translation to deny mode permissions }
		transPerm := 0;
		IF bTST(kind, kCanReadEditionsBit)			
			THEN transPerm := bOR(transPerm, dmRequestReadPerm);
			
		IF bTST(kind, kCanWriteEditionsBit)		
			THEN transPerm := bOR(transPerm, dmRequestWritePerm+dmRequestReadPerm+dmDenyOtherWriters);

		{ see if already open with enough permissions }
		{ i.e. a subscriber tries to open, but publisher already has it open }
		IF bAND(orgOpenMode, transPerm) = transPerm THEN
		BEGIN
			{ already open with enough perms }
			dpPubOpenFile := noErr;
			Success(fi);
			EXIT(dpPubOpenFile);
		END;
	
		{ to handle case of subscriber already has it open and then a publisher }
		{ tries to open it, close the file and reopen with more permissions }
		IF orgOpenMode <> dmNotOpen THEN
		BEGIN
			FailOSErr(dpPubCloseFile(thePubCB, NOT kFlush));
		END;
		
		IF bTST(thePubCB^^.volumeInfo, bHasOpenDeny) THEN
		BEGIN
			FailOSErr(BeginSystemMode);		inSystemMode := TRUE;
			{ open file of given spec }
			anErr := FSpOpenDeny(pubFile, transPerm, theRefNum);
			IF anErr = fnfErr {&} THEN IF (NOT bTST(thePubCB^^.volumeInfo, bHasFileIDs)) THEN
			BEGIN
				{ there is a small time window, during swap and delete when the file is gone. }
				{ for this case, wait a moment and retry. }
				Delay({numTicks}kTicksUntilOpenRetry, {VAR finalTicks}ignoreLong);
				anErr := FSpOpenDeny(pubFile, transPerm, theRefNum);
			END;
			{$IFC qExtraDebug }		
			IF anErr <> noErr
				THEN DebugStr('dpPubOpenFile: OpenDeny failed');
			{$ENDC }
			FailOSErr(anErr);
			FailOSErr(EndSystemMode);	inSystemMode := FALSE;
		END ELSE
		BEGIN
			{ do translation to HFS permissions }
			hfsPerm := 0;
			IF bTST(kind, kCanWriteEditionsBit) 			
				THEN hfsPerm := fsRdWrPerm
			ELSE IF bTST(kind, kCanReadEditionsBit)		
				THEN hfsPerm := fsRdPerm
			ELSE FailOSErr(permErr);
			
			FailOSErr(BeginSystemMode);		inSystemMode := TRUE;
			{ open file using spec in thePubCB }
			WITH PBH DO
			BEGIN
				ioNamePtr	:= @pubFile.name;
				ioVRefNum	:= pubFile.vRefNum;
				ioDirID		:= pubFile.parID;
				ioVersNum	:= 0;
				ioMisc		:= NIL;
				ioPermssn	:= hfsPerm;
				FailOSErr(PBHOpenSync(@PBH));		
				theRefNum	:= ioRefNum;
			END; {with}
			FailOSErr(EndSystemMode);	inSystemMode := FALSE;
		END; {if}
		
		{ set more info fields }
		WITH thePubCB^^ DO
		BEGIN
			fileRefNum	:=	theRefNum;
			openMode	:=	transPerm;
		END; {with}
		
		Success(fi);
		
	END; { dpPubOpenFile }
	
	
	
	
	{------------- dpPubFlushFile -------------} 
	{ only used after writing a new edition }
	FUNCTION dpPubFlushFile(thePubCB: PubCBHandle): OSErr;
	VAR
		theMap:		AllocationMapHandle;
		theFormats:	FormatListHandle;
		theRefNum:	INTEGER;
		header:		EditionFileMinHeader;
		PB:			ParamBlockRec;
		i:			INTEGER;
		
	BEGIN
		{ get pub file refnum & map handle }
		WITH thePubCB^^ DO
		BEGIN
			theRefNum	:= fileRefNum;
			theMap 		:= allocMap;
			theFormats 	:= formats;
		END; {with}
		
		{ make new header }
		WITH header DO
		BEGIN
			version				:= kMapVersion;
			formatsOffset		:= bAND(thePubCB^^.fileMark+$F,$FFFFFFF0); { round up to nearest paragraph }				
			formatsLength		:= theFormats^^.lastUsedSlot * SizeOf(Format) + SizeOf(INTEGER);
			mapOffset			:= bAND(formatsOffset+formatsLength+$F,$FFFFFFF0);	{ round up to nearest paragraph }		
			mapLength			:= theMap^^.lastUsedSlot * SizeOf(AllocationRecord) + SizeOf(INTEGER);
		END; {with}
				
		{ write formats out }
		WITH PB DO
		BEGIN
			ioRefNum	:= theRefNum;
			ioBuffer	:= @theFormats^^.lastUsedSlot;
			ioReqCount	:= header.formatsLength;
			ioPosMode	:= fsFromStart;
			ioPosOffset	:= header.formatsOffset;
			FailOSErr(PBWriteSync(@PB));
			IF ioActCount <> header.formatsLength THEN FailOSErr(dskFulErr); {when would this ever happen?}
		END; {with}
		
		{ write map out }
		WITH PB DO
		BEGIN
			ioRefNum	:= theRefNum;
			ioBuffer	:= @theMap^^.lastUsedSlot;
			ioReqCount	:= header.mapLength;
			ioPosMode	:= fsFromStart;
			ioPosOffset	:= header.mapOffset;
			FailOSErr(PBWriteSync(@PB));
			IF ioActCount <> header.mapLength THEN FailOSErr(dskFulErr); {when would this ever happen?}
		END; {with}
		
		
		{ write header out }
		WITH PB DO
		BEGIN
			ioRefNum	:= theRefNum;
			ioBuffer	:= @header;
			ioReqCount	:= SizeOf(header);
			ioPosMode	:= fsFromStart;
			ioPosOffset	:= 0;
			FailOSErr(PBWriteSync(@PB));
			IF ioActCount <> SizeOf(header) THEN FailOSErr(dskFulErr); {when would this ever happen?}
		END; {with}

		{ call _flushFile }
		WITH PB DO
		BEGIN
			ioRefNum	:= theRefNum;
			FailOSErr(PBFlushFileSync(@PB));
		END; {with}
		
		{### should we call flush volume? }
		dpPubFlushFile := noErr;
	END; { dpPubFlushFile }

		
	
	{------------- dpPubCloseFile -------------} 
	FUNCTION dpPubCloseFile(thePubCB: PubCBHandle; flush: BOOLEAN): OSErr;
	VAR
		PB:			ParamBlockRec;
		theRefNum: 	INTEGER;
	BEGIN
		theRefNum := thePubCB^^.fileRefNum;

		{$IFC qRangeCheck}
		IF flush {&} THEN IF NOT bTST(thePubCB^^.openMode, dmRequestWritePermBit)
			THEN DebugStr('dpPubCloseFile: trying to flush a read-only pub.');
		IF theRefNum = kClosedFileRefNum 
			THEN DebugStr('dpPubCloseFile: trying to close a closed pub file.');
		{$ENDC}
		
		{ write out map to disk if needed }
		IF flush THEN FailOSErr(dpPubFlushFile(thePubCB));
	
		{ free the map, cause it is invalid once the file is closed }
		IF thePubCB^^.allocMap <> NIL
			THEN IgnoreOSErr(dpPubReleaseMap(thePubCB));
			
		{ mark file closed in the PCB }
		WITH thePubCB^^ DO 
		BEGIN
			fileRefNum		:= kClosedFileRefNum;
			openMode 		:= dmNotOpen;
			rangeLockStart	:= 0;					{ once the file is closed, range locks are invalid }
			rangeLockLen	:= 0;
			fileMark		:= kBadPosition;
		END; {with}
		
		
		{ close file }
		WITH PB DO
		BEGIN
			{ need to be in system mode to close this file }
			IgnoreOSErr(BeginSystemMode);
			
			ioRefNum		:= theRefNum;
			dpPubCloseFile	:= PBCloseSync(@PB);
			
			IgnoreOSErr(EndSystemMode);	
		END; {with}
				
	END; { dpPubCloseFile }



	
	{-------------  dpPubLoadMap -------------} 
	FUNCTION dpPubLoadMap(thePubCB: PubCBHandle; kind: SectionType): OSErr;
	VAR
		theRefNum:	INTEGER;
		header:		EditionFileMinHeader;
		PBH:		HParamBlockRec;
		theMap:		AllocationMapHandle;
		theFormats:	FormatListHandle;
		justOpened:	BOOLEAN;
		slots:		INTEGER;
		remainder:	INTEGER;
		fi:			FailInfo;		
	BEGIN 
		DoNotPutInRegister(@theMap);
		DoNotPutInRegister(@theFormats);
		DoNotPutInRegister(@justOpened);
		theMap := NIL;
		theFormats := NIL;
		justOpened := FALSE;
	
		{ set up failure handling }
		IF isFailure(fi, dpPubLoadMap) THEN
		BEGIN
			IF justOpened THEN IgnoreOSErr(dpPubCloseFile(thePubCB, NOT kFlush));
			IF theMap <> NIL THEN DisposHandle(Handle(theMap));
			IF theFormats <> NIL THEN DisposHandle(Handle(theFormats));
			EXIT(dpPubLoadMap);
		END; {if}

		{ make sure the file is open }
		IF thePubCB^^.openMode = dmNotOpen THEN
		BEGIN
			FailOSErr(dpPubOpenFile(thePubCB, kind));
			justOpened := TRUE;
		END;
		
		(*
		{$IFC qRangeCheck}
		IF thePubCB^^.openMode <> kind
			THEN DebugStr('dpPubLoadMap: already open for wrong access');
		{$ENDC}
		*)
		
		CASE kind OF
			stPublisher:
				BEGIN					
					{ create a map for allocation }
					FailOSErr(dpNewDynamicArray(0, 						{headerSize}
											SizeOf(AllocationRecord),	{slotSize}
											kInitialAllocCount,			{initialSlots}
											NewHandleSys,				{MyNewHandle}
											theMap));					{VAR arrayH}
		
					{ create a format list for an edition with no formats }
					FailOSErr(dpNewDynamicArray(0, 			{headerSize}
											SizeOf(Format),	{slotSize}
											kInitialFormats,{initialSlots}
											NewHandleSys,	{MyNewHandle}
											theFormats));	{VAR arrayH}
				END;
				
			stSubscriber:
				BEGIN
					{ if starting to read then get map and format info from file }
	
					theRefNum := thePubCB^^.fileRefNum;
					{ read header from file }
					WITH PBH DO
					BEGIN
						ioRefNum	:= theRefNum;
						ioBuffer	:= @header;
						ioReqCount	:= 20;		{ only required part of header }
						ioPosMode	:= fsFromStart;
						ioPosOffset	:= 0;
						FailOSErr(PBReadSync(@PBH));
						IF ioActCount <> 20 THEN FailOSErr(badEditionFileErr);
					END; {with}
					
					{ check header for validity }
					IF header.version <> kMapVersion THEN FailOSErr(badEditionFileErr);
					
					{ allocate space for formats, read it from file }
					UnsignedDivide(header.formatsLength-SizeOf(INTEGER),	{numer}
									SizeOf(Format),							{denom}
									slots,									{quotient}
									remainder);								{remainder}
					{$IFC qRangeCheck }
					IF remainder <> 0 THEN DebugStr('dpPubLoadMap: bad formats size');
					{$ENDC }
					FailOSErr(dpNewDynamicArray(0, 			{headerSize}
											SizeOf(Format),	{slotSize}
											slots,			{initialSlots}
											NewHandleSys,	{MyNewHandle}
											theFormats));	{VAR arrayH}
					theFormats^^.lastUsedSlot := slots;
					HLock(Handle(theFormats));
					WITH PBH DO
					BEGIN
						ioRefNum	:= theRefNum;
						ioBuffer	:= @theFormats^^.lastUsedSlot;
						ioReqCount	:= header.formatsLength;
						ioPosMode	:= fsFromStart;
						ioPosOffset	:= header.formatsOffset;
						FailOSErr(PBReadSync(@PBH));
						IF ioActCount <> header.formatsLength THEN FailOSErr(badEditionFileErr);
					END; {with}
					HUnLock(Handle(theFormats));
					
					{ allocate space for map, read it from file }
					UnsignedDivide(header.mapLength-SizeOf(INTEGER),		{numer}
									SizeOf(AllocationRecord),				{denom}
									slots,									{quotient}
									remainder);								{remainder}
					{$IFC qRangeCheck }
					IF remainder <> 0 THEN DebugStr('dpPubLoadMap: bad formats size');
					{$ENDC }
					FailOSErr(dpNewDynamicArray(0, 						{headerSize}
											SizeOf(AllocationRecord),	{slotSize}
											slots,						{initialSlots}
											NewHandleSys,				{MyNewHandle}
											theMap));					{VAR arrayH}
					theMap^^.lastUsedSlot := slots;
					HLock(Handle(theMap));
					WITH PBH DO
					BEGIN
						ioRefNum	:= theRefNum;
						ioBuffer	:= @theMap^^.lastUsedSlot;
						ioReqCount	:= header.mapLength;
						ioPosMode	:= fsFromStart;
						ioPosOffset	:= header.mapOffset;
						FailOSErr(PBReadSync(@PBH));
						IF ioActCount <> header.mapLength THEN FailOSErr(badEditionFileErr);
					END; {with}
					HUnLock(Handle(theMap));
				END;
		END; {case}
		
		{ set more info fields }
		WITH thePubCB^^ DO
		BEGIN
			allocMap 		:= theMap;
			formats 		:= theFormats;
			fileMark		:= SizeOf(EditionFileHeader); { really only needed for writing }
		END; {with}
		
		Success(fi);
	END; { dpPubLoadMap }
	
		
	
	{-------------  dpPubReleaseMap -------------} 
	FUNCTION dpPubReleaseMap(thePubCB: PubCBHandle): OSErr;
	BEGIN 
		WITH thePubCB^^ DO
		BEGIN
			{ dispose of the allocation map }
			DisposHandle(Handle(allocMap));
			allocMap := NIL;
			
			{ dispose of the format list }
			DisposHandle(Handle(formats));
			formats := NIL;
		END; {with}
		
		dpPubReleaseMap := noErr;
	END; { dpPubReleaseMap }
	


	
	{------------- dpTweakCatInfo -------------} 
	FUNCTION dpTweakCatInfo(edition: FSSpec; 
							PROCEDURE Tweaker(VAR PBC: CInfoPBRec) ): OSErr;
	VAR
		PBC:	CInfoPBRec;
	BEGIN
		WITH PBC, edition DO
		BEGIN
			{ set up param block }
			ioNamePtr	:= @name;	
			ioVRefNum	:= vRefNum;
			ioFDirIndex	:= 0;
			ioFVersNum	:= 0;
			ioDirID		:= parID;
			FailOSErr(PBGetCatInfoSync(@PBC));
			
			{ tweak the param block }
			Tweaker(PBC);
			
			{ write out the tweaked param block }
			ioNamePtr	:= @name;	
			ioVRefNum	:= vRefNum;
			ioFDirIndex	:= 0;
			ioDirID		:= parID;
			FailOSErr(PBSetCatInfoSync(@PBC));
		END; {with}
		
		dpTweakCatInfo := noErr;
	END; { dpTweakCatInfo }
	
			
		
	{------------- dpRandomFileName -------------} 
	PROCEDURE dpRandomFileName(VAR seedName: Str63); 
	VAR
		timeNumber:	Str255;
		seedLen:	INTEGER;
		timeLen:	INTEGER;
	BEGIN
		NumToString(TickCount, timeNumber);
		seedLen := LENGTH(seedName);
		timeLen := LENGTH(timeNumber);
		IF seedLen + timeLen <= 31 THEN
		BEGIN
			BlockMove(	Ptr(ORD(@timeNumber)+1), 
						Ptr(ORD(@seedName)+seedLen+1),
						timeLen );
			seedName[0] := CHAR(SeedLen + timeLen);
		END ELSE
		BEGIN
			BlockMove(	Ptr(ORD(@timeNumber)+1), 
						Ptr(ORD(@seedName)+31-timeLen),
						TimeLen );
			seedName[0] := CHAR(31);
		END; {if}
	END; { dpRandomFileName }

		
	{------------- dpSwapAndDelete -------------} 
	FUNCTION dpSwapAndDelete(orgPubCB, newPubCB: PubCBHandle): OSErr;
	VAR
		PBH:				HParamBlockRec;
		PBC:				CInfoPBRec;
		orgEdition:			FSSpec;
		newEdition:			FSSpec;
		throwAway:			FSSpec;
		info1:				FInfo;
		info2:				FXInfo;
		originalCrDate: 	TimeStamp;
		finderComments:		Str255;
		deleteErr:			OSErr;
		exchangeErr:		OSErr;
		setCommentErr:		OSErr;
		getCommentErr:		OSErr;
		openDTErr:			OSErr;
		DTRefNum:			INTEGER;
		i:					INTEGER;
		ignoreLong:			LONGINT;
		fileToDelete:		FSSpecPtr;
		wasNotEdition:		BOOLEAN;
		fi:					FailInfo;
		
		PROCEDURE dpSetFinderInfo(VAR PBC: CInfoPBRec);
		CONST
			hasBeenInited = 8;
		VAR
			tempLong:		LONGINT;
		BEGIN
			WITH newPubCB^^.info DO
			BEGIN
				{ only need to set other info if we can't use exchangeFiles }
				IF NOT bTST(orgPubCB^^.volumeInfo, bHasFileIDs) THEN
				BEGIN
					PBC.ioFlFndrInfo 		:= info1;
					PBC.ioFlXFndrInfo		:= info2;
					PBC.ioFlCrDat 			:= originalCrDate;
				END;
				{ set new type and creator }
				PBC.ioFlFndrInfo.fdCreator 	:= fdCreator;
				PBC.ioFlFndrInfo.fdType 	:= fdType;
				{ if we are overwriting a non-edition file, we need to let the  }
				{ Finder know to re-instantiate the object for this file. }
				IF wasNotEdition THEN
				BEGIN
					tempLong := PBC.ioFlFndrInfo.fdFlags;
					bCLR(tempLong, hasBeenInited);
					PBC.ioFlFndrInfo.fdFlags := tempLong;
				END;
			END; {with}
		END; { dpSetFinderInfo }
		
	BEGIN
		{ set up failure handling }
		IF isFailure(fi, dpSwapAndDelete) THEN
		BEGIN
			{ well, all hell broke loose.  Lets at least close the }
			{ new file as if it did swap. }
			IgnoreOSErr(dpPubCloseFile(newPubCB, NOT kFlush));
			EXIT(dpSwapAndDelete);
		END; {if}

		{ switch contents of old and new pub file }
		{$IFC qRangeCheck }
		IF original.vRefNum <> newEdition.vRefNum 
			THEN DebugStr('dpSwapAndDelete: files on different volumes!');
		{$ENDC}
		
		{ make sure the original was not moved during write of new edition }
		IgnoreOSErr(dpPubSync(orgPubCB));

		{ make local copies of files specs do reduce code size }
		BlockMove(@orgPubCB^^.info.container.theFile, @orgEdition, SizeOf(FSSpec));
		BlockMove(@newPubCB^^.info.container.theFile, @newEdition, SizeOf(FSSpec));
			
		{ remember if we are overwriting a non-edition file }
		wasNotEdition := NOT IsEditionFile(orgPubCB^^.info.fdType);
		
		{$IFC qRangeCheck }
		{ if original pubCB file is open, close it }
		IF orgPubCB^^.openMode = dmNotOpen THEN
		BEGIN
			DebugStr('dpSwapAndDelete: org edition file not open.');
			IF orgPubCB^^.usageInfo^^.totalIOCount <> 0
				THEN DebugStr('dpSwapAndDelete: could not kill off all subscribers.');
		END; {if}
		{$ENDC }

		IF bTST(orgPubCB^^.volumeInfo, bHasFileIDs) THEN
		BEGIN
			{ Use ExchangeFiles }
			FailOSErr(FSpExchangeFiles(orgEdition, newEdition));
	
			{ set type and creator of to match contents }
			FailOSErr(dpTweakCatInfo(orgEdition, dpSetFinderInfo));

			{ remember which file to delete }
			fileToDelete := @newEdition;
		END ELSE
		BEGIN		
			{ exchangeFiles is not implemented, so do it by hand }

			{ get Finder info from orginal file }
			WITH PBC DO
			BEGIN
				ioNamePtr	:= @orgEdition.name;	
				ioVRefNum	:= orgEdition.vRefNum;
				ioFDirIndex	:= 0;
				ioFVersNum	:= 0;
				ioDirID		:= orgEdition.parID;
				FailOSErr(PBGetCatInfoSync(@PBC));
				info1 := ioFlFndrInfo;
				info2 := ioFlXFndrInfo;
				originalCrDate := ioFlCrDat;
			END; {with}

			{ move Finder info to new edition file, except for type and creator }
			FailOSErr(dpTweakCatInfo(newEdition, dpSetFinderInfo));
					
			{ get original's Finder comments if able }
			finderComments := '';
			IF bTST(orgPubCB^^.volumeInfo, bHasDesktopMgr) THEN
			BEGIN
				WITH PBH DO
				BEGIN
					ioVRefNum	:= orgEdition.vRefnum;
					ioNamePtr	:=	NIL;								{ <29> }
					openDTErr 	:= PBDTGetPath(@PBH);
					{$IFC qCatchFailures}
						IF (openDTErr <> noErr) 
							THEN DebugStr('dpSwapAndDelete: PBDTOpen failed.');
					{$ENDC}
					DTRefNum 	:= ioRefNum;
				END;
				
				{ continue if could open desk top database }
				IF openDTErr = noErr THEN
				BEGIN
					WITH PBH DO
					BEGIN
						ioNamePtr	:= @orgEdition.name;
						ioRefNum	:= DTRefNum;
						ioDirID		:= orgEdition.parID;
						ioBuffer	:= @finderComments[1];
						ioReqCount	:= 255;
					END; {with}
					getCommentErr := PBDTGetCommentSync(@PBH);
					IF getCommentErr = noErr 
						THEN finderComments[0] := CHR(PBH.ioActCount);
					{$IFC qCatchFailures}
						IF (getCommentErr <> noErr) 
							{&} THEN IF (getCommentErr <> afpItemNotFound)
							{&} THEN IF (getCommentErr <> fnfErr){## bug in DTmanger}
							THEN DebugStr('dpSwapAndDelete: GetComment failed.');
					{$ENDC}
				END; {if}
			END; {if}
			
			{ if original had finder comments, set the new one to have the same }
			IF LENGTH(finderComments) <> 0 THEN
			BEGIN	
				WITH PBH DO
				BEGIN
					ioNamePtr	:= @newEdition.name;
					ioRefNum	:= DTRefNum;
					ioDirID		:= newEdition.parID;
					ioBuffer	:= @finderComments[1];
					ioReqCount	:= LENGTH(finderComments);
					setCommentErr:= PBDTSetCommentSync(@PBH);
				END;
				{$IFC qCatchFailures}
				IF setCommentErr <> noErr THEN DebugStr('dpSwapAndDelete: PBDTSetComment failed.');
				{$ENDC}
			END; {if}

			{ rename orginal Edition to temp name }
			throwAway.vRefNum	:= orgEdition.vRefNum;
			throwAway.parID 	:= orgEdition.parID;
			throwAway.name 		:= kThrowMeAwayName;
			dpRandomFileName(throwAway.name);
			FailOSErr(FSpRename(orgEdition, throwAway.name));

			{ rename new Edition to original name }
			FailOSErr(FSpRename(newEdition, orgEdition.name));
			
			{ remember which file to delete }
			fileToDelete := @throwAway;
		END; {if}

		{ close original (if it was open) now that we are done }
		{ it could be close now becuase 1) we are overwriting a non-edition file }
		{ or 2) there was multiple publishers on this machine and the publisher }
		{ unregistered while this publishers was between OpenNew/CloseEdition. }
		IF orgPubCB^^.fileRefNum <> kClosedFileRefNum
			THEN IgnoreOSErr(dpPubCloseFile(orgPubCB, NOT kFlush));

		{ delete original file } 
		deleteErr := FSpDelete(fileToDelete^);
		{$IFC qExtraDebug }		
		IF deleteErr <> noErr
			THEN DebugStr('dpSwapAndDelete: final delete failed');
		{$ENDC }
		
		{ There is a small window between the close and delete, during which a subscriber could }
		{ add a range lock.  When this happens, a subscriber has the "throw me away" file open,  }
		{ so the delete fails.  The fix for this is to wait for the subscriber to close, then delete. }
		{ We don't have to worry about another subscriber re-opening, because they don't }
		{ open files called "throw me away". }
		FOR i := 1 TO kMaxDeleteRetries DO
		BEGIN
			IF deleteErr = noErr THEN LEAVE; {for}
			Delay({numTicks}kTicksBetweenDeleteRetries, {VAR finalTicks}ignoreLong);
			deleteErr := FSpDelete(fileToDelete^);
		END; {for}
		FailOSErr(deleteErr);
		
		Success(fi);				{ <37> }
	END; { dpSwapAndDelete }
	
	

	{------------- dpTryToGetFormat -------------} 
	FUNCTION dpTryToGetFormat(whichFormat: FormatType; dataHandle: Handle; 
							ioRefNum: LONGINT; ioProc: FormatIOProcPtr): OSErr;
	CONST
		kMaxTextPreviewSize = 500; { max byte count of a text preview }
	VAR
		IOParams:		FormatIOParamBlock;	
		fi:				FailInfo;
		hasFormatErr:	OSErr;
	BEGIN
		{ failure handler needed }
		IF IsFailure(fi, dpTryToGetFormat) 
			THEN EXIT(dpTryToGetFormat);

		{ see if format is available }
		IOParams.ioRefNum := ioRefNum;
		IOParams.format := whichFormat;
		FailOSErr(dp_CallFormatIOProc(ioHasFormat, IOParams, ioProc));

		{ limit text size }
		IF LONGINT(whichFormat) = LONGINT('TEXT') THEN
		BEGIN
			IF IOParams.buffLen > kMaxTextPreviewSize 
				THEN IOParams.buffLen := kMaxTextPreviewSize;
		END; {if}
		
		{ resize handle to hold it }
		SetHandleSize(dataHandle, IOParams.buffLen);
		FailOSErr(MemError);								{ <37> }
		HLock(dataHandle);
		
		{ read in format into handle }
		IOParams.ioRefNum 		:= ioRefNum;
		IOParams.format 		:= whichFormat;
		{IOParams.formatIndex	already set}
		IOParams.offset 		:= 0;
		IOParams.buffPtr 		:= dataHandle^;
		{IOParams.buffLen 		already set }
		FailOSErr(dp_CallFormatIOProc(ioReadFormat, IOParams, ioProc));
		HUnLock(dataHandle);
		
		Success(fi);
	END; { dpTryToGetFormat }





	{------------- dpPubRemoveLock -------------} 
	FUNCTION dpPubRemoveLock(thePubCB: PubCBHandle): OSErr;
	VAR
		PBH:		HParamBlockRec;
		anErr:		OSErr;
	BEGIN
		dpPubRemoveLock := noErr;
		
		WITH thePubCB^^ DO
		BEGIN
			{ is there a lock? }
			IF rangeLockLen > 0 THEN
			BEGIN
				{ remove lock we put on file header }
				WITH PBH DO
				BEGIN
					ioRefNum	:= fileRefNum;
					ioReqCount	:= rangeLockLen;
					ioPosMode	:= fsFromStart;
					ioPosOffset	:= rangeLockStart;
				END; {with}
				anErr := PBUnLockRangeSync(@PBH);	
				dpPubRemoveLock := anErr;
				{$IFC qExtraDebug }		
				IF anErr <> noErr 
					THEN DebugStr('dpPubRemoveLock: unlock returned unexpected err');
				{$ENDC }
			END; {if}
		END; {with}
		
		WITH thePubCB^^ DO
		BEGIN
			rangeLockStart	:= 0;
			rangeLockLen	:= 0;
		END; {with}
			
	END; { dpPubRemoveLock }

	

	{------------- dpPubAddLock -------------} 
	FUNCTION dpPubAddLock(thePubCB: PubCBHandle; lockStart, lockLen: INTEGER): OSErr;
	VAR
		PBH:		HParamBlockRec;
		lockErr:	OSErr;
	BEGIN		
		lockErr := noErr;
	{$IFC qRangeCheck}
		IF lockLen = 0 
			THEN DebugStr('dpPubAddLock: no lock to add');
		IF thePubCB^^.rangeLockLen <> 0 
			THEN DebugStr('dpPubAddLock: a lock already exists;g');
	{$ENDC}

		WITH thePubCB^^ DO
		BEGIN
			{ is there a lock? }
			IF rangeLockLen = 0 THEN
			BEGIN
				{ try to lock the requested range }
				WITH PBH DO
				BEGIN
					ioRefNum	:= fileRefNum;
					ioReqCount	:= lockLen;
					ioPosMode	:= fsFromStart;
					ioPosOffset	:= lockStart;
				END; {with}
				lockErr := PBLockRangeSync(@PBH);
				
				IF lockErr = noErr THEN
				BEGIN
					{ if succesful, record it in control block }
					rangeLockStart	:= lockStart;
					rangeLockLen	:= lockLen;
				END; {if}
			END; {if}
		END; {with}
			
		dpPubAddLock := lockErr;
	END; { dpPubAddLock }



	{-------------  dpFindPublisher -------------}
	FUNCTION dpFindPublisher(thePubCB: PubCBHandle; canAskUser: BOOLEAN;
							VAR publisherSectionH: SectionHandle;
							VAR publisherApplication: AppRefNum;
							VAR publisherDoc: FSSpec;
							VAR theSectionID: LONGINT): OSErr;
	{ 
	,(publisherDoc, publisherDoc), xor (publisherDoc).  
	publisherSectionH is not NIL is the first case.
	theSectionID is non-zero is the second case.
	xor (publisherDoc, publisherDoc).  publisherSectionH is not NIL is the first case.
	
	Note: currently no one calls this with canAskUser=FALSE.  The parameter
	is here in case this is ever used for "preflighting" of finding the publisher.
		
	}
	VAR
		publisherAlias:		AliasHandle;
		dummyFormat:		FormatType;
		tempPtr:			pLongint;
		aliasCount:			INTEGER;
		needsUpdate:		BOOLEAN;
		resolveErr:			OSErr;
		openErr:			OSErr;
		PBH:				HParamBlockRec;
		rfi:				FailInfo;
		
		PROCEDURE dpLookForRegisteredPublisher(sectionH: SectionHandle; inApp: AppRefNum);
		BEGIN
			IF sectionH^^.controlBlock = Handle(thePubCB) THEN
			BEGIN
				publisherSectionH := sectionH;
				publisherApplication := inApp;
			END; {if}
		END; { dpLookForRegisteredPublisher }
	
	BEGIN
		dpFindPublisher := noErr;
		publisherSectionH := NIL;
		
		{ see if we already know publisher, if so we are done }
		WITH thePubCB^^ DO
		BEGIN
			IF publisher <> NIL THEN
			BEGIN
				{ easy case, publisher known }
				publisherSectionH := publisher;
				publisherApplication := publisherApp;
				EXIT(dpFindPublisher);
			END; {if}
		END; {with}

		{ try using alias in edition file to find publisher }
		IF NOT IsFailure(rfi, resolveErr) THEN { catch any failures properly }
		BEGIN
			FailOSErr(dpResolveAliasToPublisher(thePubCB, canAskUser, publisherDoc, theSectionID));
			Success(rfi);
		END;

		{ if we found publisher via alias then we are done }
		IF resolveErr = noErr 
			THEN EXIT(dpFindPublisher);
				
		{ next possible way to find publisher is to check all currently registered sections }
		IF thePubCB^^.publisherCount > 0 THEN 
		BEGIN
			{ so scan all register sections }
			dpForEachSectionDo(dpLookForRegisteredPublisher);
			EXIT(dpFindPublisher);
		END;
		
		{ last way to find publisher is if there was no alias and no registered publisher }
		IF resolveErr <> noTypeErr THEN FailOSErr(resolveErr); {noTypeErr => no alias in edition file }
		
		{ ####  note: the rest of this code needs to be fixed.  This was written }
		{ back when no alias meant that the edition was a "publisherless-edition" }
		{ (that is a stand alone file.  Opening a "publisherless-edition" launches an  }
		{ app which can edit it.)  Now, the existance of a 'doev' format has that meaning.  }
		{ The following code should be replaced with a check for a 'doev' format }
		{ (by calling GetStandardFormats and looking at the formats list.)  If there }
		{ is a 'doev' format, this routine should return the edition file as the  }
		{ publisherDoc (i.e. keep the BlockMove). }		
				
		{ assume app knows what it is doing (by not putting an alias  }
		{ in edition) and send it an odoc event to open edition }
		WITH thePubCB^^ DO
		BEGIN
			IF bTST({thePubCB^^.}volumeInfo, bHasOpenDeny) THEN
			BEGIN
				{ This is a shared volume, so the publisher could be in }
				{ an untitled document on another machine.  If that is the }
				{ case, we don't want to open edition here. }
				
				{ We will try to open the edition for writing, if that fails }
				{ then there is a publisher open for it somewhere.   }
				
				{ If edition file is already open, we can't check so just fail. }
				IF {thePubCB^^.}fileRefNum <> kClosedFileRefNum THEN FailOSErr(fBsyErr);
				
				{ try to open }
				FailOSErr(FSpOpenDeny(info.container.theFile, dmRequestWritePerm+dmDenyOtherWriters, PBH.ioRefNum));
				
				{ we could open it, so close it }
				IgnoreOSErr(PBCloseSync(@PBH));
			END; {if}
			
			{ return edition file as publisher, and sectionID = 0 }
			BlockMove(@info.container.theFile, @publisherDoc, SizeOf(FSSpec));
			theSectionID := 0;
		END; {with}

	END; { dpFindPublisher }
	

	
	{-------------  dpResolveAliasToPublisher -------------}
	FUNCTION dpResolveAliasToPublisher(thePubCB: PubCBHandle; canAskUser: BOOLEAN;
										VAR publisherDoc: FSSpec;
										VAR theSectionID: LONGINT): OSErr;
	VAR
		publisherAlias:		AliasHandle;
		dummyFormat:		FormatType;
		tempPtr:			pLongint;
		aliasCount:			INTEGER;
		needsUpdate:		BOOLEAN;
		rulesMask:			LONGINT;
		fi:					FailInfo;
	BEGIN			
		DoNotPutInRegister(@publisherAlias);
		
		IF IsFailure(fi, dpResolveAliasToPublisher) THEN
		BEGIN
			IF publisherAlias <> NIL THEN DisposHandle(Handle(publisherAlias));
			EXIT(dpResolveAliasToPublisher);
		END; 
		
		{ try to resolve alias to publisher doc }	
		Handle(publisherAlias) := NewHandle(100); { dp_GetStandardFormats will resize as needed }
		FailOSErr(MemError);
		
		FailOSErr(dp_GetStandardFormats(thePubCB^^.info.container, dummyFormat, NIL, Handle(publisherAlias), NIL));

		{ snag sectionID of publisher from end of alias }
		tempPtr := pLongint(pLongint(publisherAlias)^ + publisherAlias^^.aliasSize);
		theSectionID := tempPtr^ ;

		{ resolve the alias }
		aliasCount := 1;
		IF canAskUser 
			THEN rulesMask := kARMsearch+kARMsearchRelFirst+kARMmountVol
			ELSE rulesMask := kARMsearch+kARMsearchRelFirst+kARMnoUI;
		{###? add a filter for documents only }	
		FailOSErr(MatchAlias(@thePubCB^^.info.container.theFile, 			{ fromFile }
								rulesMask,									{ rulesMask }
								publisherAlias, 							{ alias }
								aliasCount,									{ VAR aliasCount }
								@publisherDoc, 								{ buffer  }
								needsUpdate, 								{ VAR needsUpdate }
								NIL, 										{ aliasFilter }
								NIL)); 										{ yourDataPtr }
								
		{ free up the temp alias }
		DisposHandle(Handle(publisherAlias));
		
		Success(fi);					{ <37> }
	END; { dpResolveAliasToPublisher }


{================================ standard routines ===================================}

		
	{------------- dpStandardSubscriberOpen -------------} 
	FUNCTION dpStandardSubscriberOpen(VAR info:	EditionInfoRecord;
									sectionH:	SectionHandle;
									VAR ioRefNum: LONGINT;
									VAR ioProc:	FormatIOProcPtr): OSErr;
	VAR
		thePubCB:		PubCBHandle;
		thisApp:		AppRefNum;
		PBH:			HParamBlockRec;
		PBF:			FCBPBRec;
		randomRangeLock:LONGINT;
		lockErr:		OSErr;
		i:				INTEGER;
		justOpened:		BOOLEAN;
		mapJustLoaded:	BOOLEAN;
		addedLock:		BOOLEAN;
		editionName:	Str63;
		temp:			LONGINT;
		fi:				FailInfo;
	BEGIN	
		{ initialize state }
		DoNotPutInRegister(@justOpened);
		DoNotPutInRegister(@mapJustLoaded);
		DoNotPutInRegister(@addedLock);
		DoNotPutInRegister(@thePubCB);
		mapJustLoaded := FALSE;
		justOpened := FALSE;
		addedLock := FALSE;
		thePubCB := NIL;
		
		{ need a failure handler to undo, if failed half way through }
		IF IsFailure(fi, dpStandardSubscriberOpen) THEN
		BEGIN
			IF NOT justOpened THEN
			BEGIN
				IF addedLock THEN IgnoreOSErr(dpPubRemoveLock(thePubCB));
			END
			ELSE IgnoreOSErr(dpPubCloseFile(thePubCB, NOT kFlush));
			IF mapJustLoaded THEN IgnoreOSErr(dpPubReleaseMap(thePubCB));
			IF thePubCB <> NIL {&} THEN IF sectionH = NIL 
				THEN IgnoreOSErr(dpReleasePubCB(thePubCB, thisApp));
			EXIT(dpStandardSubscriberOpen);
		END;
		
		FailOSErr(dp_GetCurrentAppRefNum(thisApp));
		
		{ get the control block through the section or by filename }
		IF sectionH = NIL THEN
		BEGIN
			FailOSErr(dpRequestPubCB(info.container.theFile, thisApp, 
								kCheckExisting, NOT kCanAllocateNew, thePubCB));
		END ELSE
		BEGIN
			thePubCB := PubCBHandle(sectionH^^.controlBlock);
		END;
		
		{ load edition file maps, if not already so }
		IF thePubCB^^.allocMap = NIL THEN
		BEGIN
			{ make sure the file is open }
			IF thePubCB^^.openMode = dmNotOpen THEN
			BEGIN
				FailOSErr(dpPubOpenFile(thePubCB, stSubscriber));
				justOpened := TRUE;
			END;

			{ on shared volumes we have to range lock a byte in the header }
			{ to prevent publisher from writing }
			IF bTST(thePubCB^^.volumeInfo, bHasOpenDeny) THEN
			BEGIN
				{ only need to add a lock if we don't already have one }
				IF thePubCB^^.rangeLockLen = 0 THEN
				BEGIN
					{ first try random lock }
					randomRangeLock := kSubscriberRangeLockStart + bAND(Random, kSubscriberRangeLockMask);
					
					{ try to lock a bit, to let the publisher know we are here }
					lockErr := dpPubAddLock(thePubCB, randomRangeLock, 1);
					
					IF lockErr = fLckdErr THEN
					BEGIN
						{ if range lock failed, then chances are it is because }
						{ the publisher is writing.  To test this we try to read }
						{ a bit from the header, which publishers have locked. }
						PBH.ioRefNum	:= thePubCB^^.fileRefNum;
						PBH.ioBuffer	:= @temp;
						PBH.ioReqCount	:= 2;		
						PBH.ioPosMode	:= fsFromStart;
						PBH.ioPosOffset	:= 0;
						FailOSErr(PBReadSync(@PBH));		{ if publisher is writing, this will fail }
					END; {if}
	
					{ publisher is not writing, so it is rotton luck that we }
					{ happened to choose the same offset as another subscriber }
					FOR i := kSubscriberLockRetries DOWNTO 0 DO
					BEGIN
						{ if unexpected error then FailOSErr out }
						IF lockErr <> fLckdErr 
							THEN FailOSErr(lockErr);
						
						{ try next byte, but wrap around if needed }
						randomRangeLock := kSubscriberRangeLockStart + bAND(randomRangeLock+1, kSubscriberRangeLockMask);
						lockErr := dpPubAddLock(thePubCB, randomRangeLock, 1);
	
						{ jump out as soon as lock succeeds }
						IF lockErr = noErr THEN LEAVE; {for}
					END; {for}
					{ fail if last try did not succeed }
					FailOSErr(lockErr);
					addedLock := TRUE;
				END; {if}
				
				{ check FCB to see if file has been renamed }
				{ this can happen if lock gets in between time publisher closes and deletes }
				WITH thePubCB^^ DO
				BEGIN
					{ file is open, can track down access path to get info }
					PBF.ioNamePtr	:= @editionName;			{ put current name here }
					PBF.ioRefNum	:= {thePubCB^^.}fileRefNum;
					PBF.ioFCBIndx	:= 0;
					FailOSErr(PBGetFCBInfoSync(@PBF));
					IF PBF.ioFCBParID <> 0 THEN { only do this for HFS volumes }
					BEGIN
						{ if the file name has changed, then publisher is busy writing }
						IF NOT dpSameBytes(@editionName[0], @{thePubCB^^.}info.container.theFile.name[0], LENGTH(editionName)+1) 
							THEN FailOSErr(fLckdErr);
					END; {if}
				END; {with}
								
			END; {if}
		
			{ load map }
			FailOSErr(dpPubLoadMap(thePubCB, stSubscriber));
			mapJustLoaded := TRUE;
		END; {if}
		
		{ bump this apps open count for this PubCB }
		FailOSErr(dpPubCBbumpUsageCount(thePubCB^^.usageInfo, thisApp, kCanGrowUsage, 0, 1));
	
		ioRefNum := ORD(thePubCB);
		ioProc := FormatIOProcPtr(kStandardFormatIOProcPtr); 
		
		Success(fi);
	END; { dpStandardSubscriberOpen }



	
	{------------- dpStandardSubscriberClose -------------} 
	FUNCTION dpStandardSubscriberClose(sectionH: SectionHandle;
										ioRefNum: LONGINT;
										ioProc:	FormatIOProcPtr): OSErr;
	VAR
		thePubCB:	PubCBHandle;
		thisApp: 	AppRefNum;
		anErr:		OSErr;
	BEGIN		
		thePubCB := PubCBHandle(ioRefNum); 
		FailOSErr(dp_GetCurrentAppRefNum(thisApp));

		{ dec this apps open count for this PubCB }
		FailOSErr(dpPubCBbumpUsageCount(thePubCB^^.usageInfo, thisApp, NOT kCanGrowUsage, 0, -1));
		
		{ stop IO, if this is last section that is doing any IO }
		IF thePubCB^^.usageInfo^^.totalIOCount = 0 THEN
		BEGIN
			{ dump allocation map and formats list }
			FailOSErr(dpPubReleaseMap(thePubCB));	
			
			IF bTST(thePubCB^^.openMode, dmRequestWritePermBit) THEN
			BEGIN
				{ a publisher also has the file open, so don't close it, }
				{ just remove the subscriber's lock. }
				FailOSErr(dpPubRemoveLock(thePubCB));
			END ELSE
			BEGIN
				{ no publisher, so close and release lock }
				FailOSErr(dpPubCloseFile(thePubCB, NOT kFlush)); { this will also remove the range lock }
			END;
		END; {if}
		
		{ undo what we did in dpStandardSubscriberOpen }
		IF sectionH = NIL 
			THEN FailOSErr(dpReleasePubCB(thePubCB, thisApp));
	
		dpStandardSubscriberClose := noErr;
	END; { dpStandardSubscriberClose }
	
	
	
	{------------- dpStandardPublisherOpen -------------} 
	FUNCTION dpStandardPublisherOpen(sectionH:	SectionHandle;
									fdCreator:	OSType;
									document:	FSSpecPtr;
									VAR ioRefNum: LONGINT;
									VAR ioProc:	FormatIOProcPtr): OSErr;
	VAR
		thisApp:			AppRefNum;
		tempEdtnFile:		FSSpec;
		thePubCB:			PubCBHandle;
		newPubCB:			PubCBHandle;
		aliasH:				AliasHandle;
		tempPtr:			pLongint;
		fileCreated:		BOOLEAN;
		ioCountBumped:		BOOLEAN;
		PBH:				HParamBlockRec;
		ignoreLong:			LONGINT;
		i:					INTEGER;
		lockErr:			OSErr;
		fi:					FailInfo;
	BEGIN
		DoNotPutInRegister(@fileCreated);
		DoNotPutInRegister(@ioCountBumped);
		DoNotPutInRegister(@newPubCB);
		fileCreated := FALSE;
		ioCountBumped := FALSE;
		newPubCB := NIL;
		
		{ set up failure handling }
		IF isFailure(fi, dpStandardPublisherOpen) THEN
		BEGIN
			{ undo any thing we allocated, in reverse order of allocation }
			IF ioCountBumped THEN
			BEGIN
				IgnoreOSErr(dpPubCBbumpUsageCount(newPubCB^^.usageInfo, thisApp, NOT kCanGrowUsage, 0, -1));
			END;
			IF newPubCB <> NIL THEN
			BEGIN
				sectionH^^.controlBlock := Handle(thePubCB);
				IgnoreOSErr(dpReleasePubCB(newPubCB, thisApp));
			END;
			IF fileCreated THEN 
			BEGIN
				IgnoreOSErr(FSpDelete(tempEdtnFile));
			END; {if}
			EXIT(dpStandardPublisherOpen);
		END; {if}

		FailOSErr(dp_GetCurrentAppRefNum(thisApp));
		thePubCB := PubCBHandle(sectionH^^.controlBlock);
			
		{ is this on a shared volume? }
		IF bTST(thePubCB^^.volumeInfo, bHasOpenDeny) THEN
		BEGIN
			{ if subscriber/publisher already has a lock (is reading/writing), tell publisher it is inuse }
			IF thePubCB^^.rangeLockLen > 0 
				THEN FailOSErr(fLckdErr);
		
			{ range lock file header to prevent subscribers from opening for read }
			{ and potentially preventing me from deleting old edition file when I am done }
			FOR i := kPublisherLockRetries DOWNTO 0 DO
			BEGIN
				lockErr := dpPubAddLock(thePubCB, kPublisherRangeLockStart, kPublisherRangeLockLength);

				{ jump out as soon as lock succeeds }
				IF lockErr = noErr THEN LEAVE; {for}
				
				{ if unexpected error then FailOSErr out }
				IF lockErr <> fLckdErr THEN FailOSErr(lockErr);
				
				{ wait a bit before retry }
				Delay({numTicks}kTicksBetweenPublisherRetries, {VAR finalTicks}ignoreLong);
			END; {for}
			FailOSErr(lockErr);
		END; {if}

		{ create a temp-named edition file }
		tempEdtnFile := thePubCB^^.info.container.theFile;
		dpRandomFileName(tempEdtnFile.name);
		FailOSErr(FSpCreate(tempEdtnFile, fdCreator, kUnknownEditionFileType, thePubCB^^.info.container.theFileScript));
		fileCreated := TRUE;
		
		{ create a PubCB for it }
		FailOSErr(dpRequestPubCB(tempEdtnFile, thisApp, NOT kCheckExisting, kCanAllocateNew, newPubCB));
		sectionH^^.controlBlock := Handle(newPubCB);
		
		{ fill in the fields not done right by RequestPubCB }
		WITH newPubCB^^ DO
		BEGIN
			oldPubCB := thePubCB;
			publisher := sectionH; { same as thePubCB^^.publisher }
			publisherApp := thisApp; { same as thePubCB^^.publisherApp }
		END; {with}
		
		{ initialize an empty map and formats list }
		FailOSErr(dpPubLoadMap(newPubCB, stPublisher));
		
		{ bump this apps open count for this PubCB }
		FailOSErr(dpPubCBbumpUsageCount(newPubCB^^.usageInfo, thisApp, kCanGrowUsage, 0, 1));
		ioCountBumped := TRUE;
		
		{ create the "backward" link from the EditionFile to the PublisherDocument }
		aliasH := NIL;
		IF document <> NIL THEN
		BEGIN
			FailOSErr(NewAlias(@newPubCB^^.info.container.theFile, document^, aliasH));
			{$IFC qCatchFailures}
				IF aliasH^^.aliasSize <> GetHandleSize(Handle(aliasH)) 
					THEN DebugStr('dp_OpenNewEdition: incorrect alias size');
			{$ENDC}
			
			{ add the sectionID of the publisher to the end of the alias }
			SetHandleSize(Handle(aliasH), aliasH^^.aliasSize + SizeOf(LONGINT));
			IF MemError = noErr THEN
			BEGIN
				tempPtr := pLongint(pLongint(aliasH)^ + aliasH^^.aliasSize);
				tempPtr^ := sectionH^^.sectionID;
			END; {if}
		END; {if}
		
		{ save off alias until close }
		newPubCB^^.publisherAlias := aliasH;
			
		ioRefNum := ORD4(newPubCB);
		ioProc := FormatIOProcPtr(kStandardFormatIOProcPtr);
		
		Success(fi);
	END; { dpStandardPublisherOpen }
		
		
		
	{------------- dpStandardPublisherClose -------------} 
	FUNCTION dpStandardPublisherClose(sectionH: SectionHandle; 
									success: BOOLEAN;
									ioRefNum: LONGINT;
									ioProc:	FormatIOProcPtr): OSErr;
	VAR
		newPubCB:			PubCBHandle;
		orgPubCB:			PubCBHandle;
		thisApp: 			AppRefNum;
		aliasH:				AliasHandle;
		IOParams:			FormatIOParamBlock;
		areSubscribers:		BOOLEAN;
		originalCrDate:		TimeStamp;
		newMdDate:			TimeStamp;
		foundIndex: 		INTEGER;
		foundPtr: 			FormatPtr;
		newType:			OSType;
		knownFormatsCount:	INTEGER;
		savedPublisherCount:INTEGER;
		swapSucceeded:		BOOLEAN;
		fi:					FailInfo;
		
		FUNCTION FindStandardFormat(anEntryIndex: INTEGER; anEntryPtr: FormatPtr): BOOLEAN;
		BEGIN			
			{ handle each case }
			WITH anEntryPtr^ DO
			BEGIN
				IF LONGINT(theType) = LONGINT('PICT') THEN
				BEGIN
					IF knownFormatsCount = 0 
						THEN newType := kPICTEditionFileType;
					knownFormatsCount := knownFormatsCount + 1;
				END ELSE
				IF LONGINT(theType) = LONGINT('TEXT') THEN
				BEGIN
					IF knownFormatsCount = 0 
						THEN newType := kTEXTEditionFileType;
					knownFormatsCount := knownFormatsCount + 1;
				END ELSE
				IF LONGINT(theType) = LONGINT('snd ') THEN
				BEGIN
					IF knownFormatsCount = 0 
						THEN newType := ksndEditionFileType;
					knownFormatsCount := knownFormatsCount + 1;
				END;
			END; {with}
			FindStandardFormat := FALSE;
		END; { FindStandardFormat }

	
		PROCEDURE KillIOtoOldPubCB(aSection: SectionHandle; inApp: AppRefNum);
		BEGIN
			WITH aSection^^ DO
			BEGIN
				IF (SIOCBHandle(refNum) <> NIL) {&} THEN IF (PubCBHandle(controlBlock) = orgPubCB) THEN
				BEGIN
					SIOCBHandle(refNum)^^.ioProc := FormatIOProcPtr(kBogusFormatIOProcPtr);
					IF dpPubCBbumpUsageCount(orgPubCB^^.usageInfo, inApp, NOT kCanGrowUsage, 0, -1) <> noErr THEN
					BEGIN
					{$IFC qRangeCheck }
						DebugStr('dp_CloseEditionThatWasOpenForWriting.KillIOtoOldPubCB: could not close a subscriber.');
					{$ENDC}					
					END;
				END; {if}
			END; {with}
		END; { KillIOtoOldPubCB }
		
		PROCEDURE dpSetMdCrDate(VAR PBC: CInfoPBRec);
		VAR
			flags: LONGINT;
		BEGIN
			PBC.ioFlMdDat := newMdDate;
			PBC.ioFlCrDat := originalCrDate;
			
			flags := PBC.ioFlFndrInfo.fdFlags; 
			IF knownFormatsCount > 1 
				THEN bSet(flags, kHasMultipleFormatsBit)
				ELSE bClr(flags, kHasMultipleFormatsBit);
			PBC.ioFlFndrInfo.fdFlags := flags;
		END; { dpSetMdCrDate }
	
	BEGIN		
		FailOSErr(dp_GetCurrentAppRefNum(thisApp));
		newPubCB := PubCBHandle(ioRefNum);
		orgPubCB := newPubCB^^.oldPubCB;
		
		{ dec this apps IO count for this PubCB }
		FailOSErr(dpPubCBbumpUsageCount(newPubCB^^.usageInfo, thisApp, NOT kCanGrowUsage, 0, -1));
		
		{$IFC qRangeCheck }
		IF newPubCB^^.usageInfo^^.totalIOCount <> 0
			THEN DebugStr('dp_CloseEditionThatWasOpenForWriting: opened by someone other than Publisher ?!?!');
		{$ENDC }
		
		IF success THEN
		BEGIN
			{ retrieve the "backward" link from the EditionFile to the PublisherDocument }
			aliasH := newPubCB^^.publisherAlias;
		
			{ no alias means the app does not have a disk version of the document yet }
			IF aliasH <> NIL THEN
			BEGIN
				{ create an 'alis' format }
				IOParams.ioRefNum	:= ioRefNum;
				IOParams.format		:= kPublisherDocAliasFormat;
				FailOSErr(dp_CallFormatIOProc(ioNewFormat, IOParams, ioProc));
				
				{ write the 'alis' format }
				HLock(Handle(aliasH));
				{IOParams.refNum  		already set }
				{IOParams.format 		already set }
				{IOParams.formatIndex 	already set }
				IOParams.offset		:= 0;
				IOParams.buffPtr	:= Ptr(aliasH^);
				IOParams.buffLen 	:= GetHandleSize(Handle(aliasH));
				FailOSErr(dp_CallFormatIOProc(ioWriteFormat, IOParams, ioProc));
				
				{ dispose of backward link }
				newPubCB^^.publisherAlias := NIL;
				DisposHandle(Handle(aliasH));
			END; {if}
			
			{ flush info to disk }
			FailOSErr(dpPubFlushFile(newPubCB));
	
			{ figure out fdType for new edition file }
			newType := kUnknownEditionFileType;
			knownFormatsCount := 0;
			IF dpFindElement(newPubCB^^.formats, 0, FindStandardFormat, foundIndex, foundPtr) THEN;
			
			{ set new finder type based of first known format and set flag if multiple known }
			newPubCB^^.info.fdType := newType;				
			
			{ try to dump new map and format list }
			areSubscribers := (orgPubCB^^.usageInfo^^.totalUseCount > 1);
			IF NOT areSubscribers THEN
			BEGIN
				{ no current subscribers, so can free up new map and formats }
				FailOSErr(dpPubReleaseMap(newPubCB));
			END;
			
			{ if orgPubCB file has map in memory, free it (maybe blowing up subscribers) }
			IF orgPubCB^^.allocMap <> NIL THEN
			BEGIN
				IF areSubscribers THEN dpForEachSectionDo(KillIOtoOldPubCB);
				FailOSErr(dpPubReleaseMap(orgPubCB));
			END; {if}

			{ save create and modification date }
			IF sectionH <> NIL 
				THEN newMdDate := sectionH^^.mdDate
				ELSE newMdDate := newPubCB^^.info.mdDate;
			originalCrDate := orgPubCB^^.info.crDate;
			
			{ swap data from new into old pub file, delete new pub file }
			{ if swap fails, we keep orgPubCB as is, and delete newPubCB }
			swapSucceeded := (dpSwapAndDelete(orgPubCB, newPubCB) = noErr);
			
			{ blockmove contents of newPubCB over orgPubCB, to keep subscribers }
			{ but, keep old info.container and publisherCount }
			WITH orgPubCB^^ DO
			BEGIN
				IF swapSucceeded THEN
				BEGIN
					savedPublisherCount := {orgPubCB^^.}publisherCount;
					BlockMove(	@newPubCB^^.pubCNodeID, {from}
								@{orgPubCB^^.}pubCNodeID, {to}
								ORD(@{orgPubCB^^.}info.container) - ORD(@{orgPubCB^^.}pubCNodeID) {len});
					{orgPubCB^^.}publisherCount := savedPublisherCount;
					{orgPubCB^^.}oldPubCB := NIL;
				
					{ fix up mod dates to make polling more efficient }
					lastVolMod := {orgPubCB^^.}info.mdDate;
					lastDirMod := {orgPubCB^^.}info.mdDate;
				
					{ set new edition's mdDate and crDate }
					{orgPubCB^^.}info.crDate := originalCrDate;
					{orgPubCB^^.}info.mdDate := newMdDate;
					FailOSErr(dpTweakCatInfo({orgPubCB^^.}info.container.theFile, dpSetMdCrDate));
				END;
				
				{ make this section the publisher for orgPubCB }
				{orgPubCB^^.}publisher := sectionH;
				WITH sectionH^^ DO
				BEGIN
					PubCBHandle({sectionH^^.}controlBlock) := orgPubCB;
					publisherKind := {sectionH^^.}kind;
				END; {with}
			END; {with}

			{ remove newPubCB }
			WITH newPubCB^^ DO
			BEGIN
				{ the refnum in newPubCB has been copied into orgPubCB, but DisposePubCB }
				{ does a sanity check that the fileRefNum says the file is closed, so fake it }
				{newPubCB^^.}fileRefNum := kClosedFileRefNum; 
				IgnoreOSErr(dpPubCBbumpUsageCount({newPubCB^^.}usageInfo, thisApp, NOT kCanGrowUsage, -1, 0));
			END; {with}
			IgnoreOSErr(dpDisposePubCB(newPubCB));
		END ELSE
		BEGIN
			{ unsuccessful write, want to roll back to state before OpenNewEdition }
			{ note that since we are closing, we ignore all errors and plow on ahead }
			
			{ remove lock }
			IgnoreOSErr(dpPubRemoveLock(newPubCB));
						
			{ point section back to old PubCB }
			PubCBHandle(sectionH^^.controlBlock) := orgPubCB;
			
			{ dump map built so far and close temp file }
			IgnoreOSErr(dpPubReleaseMap(newPubCB));
			IgnoreOSErr(dpPubCloseFile(newPubCB, NOT kFlush));

			{ delete temp file, since it won't be used }
			IgnoreOSErr(FSpDelete(newPubCB^^.info.container.theFile));
				
			{ remove newPubCB }
			IgnoreOSErr(dpDisposePubCB(newPubCB));
		END; {if success}
		
		dpStandardPublisherClose := noErr;
	END; { dpStandardPublisherClose }
	
	
	
	{-------------  dpStandardCanSubscribe -------------} 
	FUNCTION dpStandardCanSubscribe(VAR info: EditionInfoRecord; formatsMask: SignedByte; hasMultipleFormats: BOOLEAN): OSErr; 
	VAR
		hasFormat:	SignedByte;
		formatsH:	Handle;
		ignoreType:	FormatType;
		getErr:		OSErr;
		ignoreRem:	INTEGER;
		i:			INTEGER;
		p:			FormatPtr;
	BEGIN
		{ assume failure }
		dpStandardCanSubscribe := noTypeErr;
		
		{ can only subscribe to whole files of type: edtp, edtt, edts, or edtu }
		IF info.container.thePart <> kPartsNotUsed 
			THEN EXIT(dpStandardCanSubscribe);
			
		{ ### grandfather in all 'publ' s}
		IF LONGINT(info.fdType) = LONGINT('publ') THEN
		BEGIN
			dpStandardCanSubscribe := noErr;
			EXIT(dpStandardCanSubscribe);
		END;
		
		{ map finder type to appropriate mask bit or fail }
		IF LONGINT(info.fdType) = LONGINT(kPICTEditionFileType) THEN hasFormat := kPICTformatMask 
		ELSE IF LONGINT(info.fdType) = LONGINT(kTEXTEditionFileType) THEN hasFormat := kTEXTformatMask 
		ELSE IF LONGINT(info.fdType) = LONGINT(ksndEditionFileType) THEN hasFormat := ksndformatMask 
		ELSE IF LONGINT(info.fdType) = LONGINT(kUnknownEditionFileType) THEN hasFormat := 0 
		ELSE IF LONGINT(info.fdType) = LONGINT('edtP') THEN hasFormat := kPICTformatMask { ### grandfather in edtP }
		ELSE IF LONGINT(info.fdType) = LONGINT('edtT') THEN hasFormat := kTEXTformatMask { ### grandfather in edtT }
		ELSE EXIT(dpStandardCanSubscribe);

		{ return if bit corrosponding to finder type is in mask }
		IF bAND(formatsMask, hasFormat) <> 0 THEN
		BEGIN
			{ the mask says it can handle this edition file }
			dpStandardCanSubscribe := noErr;
			EXIT(dpStandardCanSubscribe);
		END;
		
		{ bit not in mask, so need to check secondary formats }
		{ hasMultipleFormats flag is state of kHasMultipleFormatsBit flag }
		IF hasMultipleFormats {&} THEN IF bAND(formatsMask,kPICTformatMask+kTEXTformatMask+ksndformatMask) <> 0 THEN 
		BEGIN
			formatsH := NewHandle(0);
			getErr := GetStandardFormats(info.container, ignoreType, NIL, NIL, formatsH);
			IF getErr = noErr THEN
			BEGIN
				UnSignedDivide(GetHandleSize(formatsH), SizeOf(Format), i, ignoreRem);
				{$IFC qRangeCheck }
				IF ignoreRem <> 0 THEN DebugStr('dpStandardCanSubscribe: bad fmts handle length.');
				{$ENDC }
				p := FormatPtr(formatsH^);
				REPEAT
					WITH p^ DO
					BEGIN
						IF LONGINT(theType) = LONGINT('PICT') {&} THEN IF bAND(formatsMask, kPICTformatMask) <> 0
							THEN dpStandardCanSubscribe := noErr;
						IF LONGINT(theType) = LONGINT('TEXT') {&} THEN IF bAND(formatsMask, kTEXTformatMask) <> 0
							THEN dpStandardCanSubscribe := noErr;
						IF LONGINT(theType) = LONGINT('snd ') {&} THEN IF bAND(formatsMask, ksndformatMask) <> 0
							THEN dpStandardCanSubscribe := noErr;
					END; {with}
					p := FormatPtr( ORD(p) + SizeOf(Format) );
					i := i - 1;
				UNTIL i <= 0;
				DisposHandle(formatsH);
			END
			ELSE dpStandardCanSubscribe := getErr;
		END; {if}
				
	END; { dpStandardCanSubscribe }
	


	{-------------  dpStandardOpener -------------} 
	FUNCTION dpStandardOpener(selector: EditionOpenerVerb; VAR PB: EditionOpenerParamBlock): OSErr; 
	BEGIN
		WITH PB DO
		BEGIN
			CASE selector OF
				eoOpen:
					dpStandardOpener := dpStandardSubscriberOpen(info, sectionH, ioRefNum, ioProc);
					
				eoClose:
					dpStandardOpener := dpStandardSubscriberClose(sectionH, ioRefNum, ioProc);
					
				eoOpenNew:
					dpStandardOpener := dpStandardPublisherOpen(sectionH, fdCreator, document, ioRefNum, ioProc);

				eoCloseNew:
					dpStandardOpener := dpStandardPublisherClose(sectionH, success, ioRefNum, ioProc);
					
				eoCanSubscribe:	
					dpStandardOpener := dpStandardCanSubscribe(info, formatsMask, success);
					
				OTHERWISE
					dpStandardOpener := paramErr;
			END; {case}
		END; {with}
	END; { dpStandardOpener }


{================================ public routines ===================================}



	{-------------  dp_CreateEditionContainerFile -------------} 
	FUNCTION dp_CreateEditionContainerFile(editionFile: FSSpec;
											fdCreator: OSType;
											editionFileNameScript: INTEGER): OSErr;
	BEGIN 
		dp_CreateEditionContainerFile := FSpCreate(editionFile, fdCreator, 
													kUnknownEditionFileType, editionFileNameScript);
	END; { dp_CreateEditionContainerFile }


	
	{-------------  dp_DeleteEditionContainerFile -------------} 
	FUNCTION dp_DeleteEditionContainerFile(editionFile: FSSpec): OSErr;
	VAR
		thePubCB:		PubCBHandle;
		thisApp:		AppRefNum;
		deleteIt:		Boolean;
		fi:				FailInfo;
				
		PROCEDURE KillIOtoThePubCB(aSection: SectionHandle; inApp: AppRefNum);
		BEGIN
			WITH aSection^^ DO
			BEGIN
				IF (SIOCBHandle(refNum) <> NIL) {&} THEN IF (PubCBHandle(controlBlock) = thePubCB) THEN
				BEGIN
					SIOCBHandle(refNum)^^.ioProc := FormatIOProcPtr(kBogusFormatIOProcPtr);
					IF dpPubCBbumpUsageCount(thePubCB^^.usageInfo, inApp, NOT kCanGrowUsage, 0, -1) <> noErr THEN
					BEGIN
					{$IFC qRangeCheck }
						DebugStr('dp_DeleteEditionContainerFile.KillIOtoThePubCB: could not close a subscriber.');
					{$ENDC}					
					END;
				END; {if}
			END; {with}
		END; { KillIOtoThePubCB }
				
	BEGIN 
		{ assume we will delete the edition file }
		deleteIt := TRUE;
		
		{ top level functions must set up a failure handler }
		IF IsFailure(fi, dp_DeleteEditionContainerFile)
			THEN EXIT(dp_DeleteEditionContainerFile);

		FailOSErr(dp_GetCurrentAppRefNum(thisApp));
		
		{ see if it is in use by any sections }
		IF dpRequestPubCB(editionFile, thisApp, kCheckExisting,
						NOT kCanAllocateNew, thePubCB) = noErr THEN
		BEGIN
			{ are there are publishers? }
			IF thePubCB^^.publisherCount = 0 THEN
			BEGIN
				{ is some subscribers are doing I/O then stop them }
				IF thePubCB^^.usageInfo^^.totalIOCount > 0 THEN 
				BEGIN
					dpForEachSectionDo(KillIOtoThePubCB);
					
					{ if original pubCB file is open, close it }
					IF thePubCB^^.openMode <> dmNotOpen THEN
					BEGIN
						{$IFC qRangeCheck }
						IF thePubCB^^.usageInfo^^.totalIOCount <> 0
							THEN DebugStr('dp_DeleteEditionContainerFile: could not kill off all subscribers.');
						{$ENDC }
						IgnoreOSErr(dpPubCloseFile(thePubCB, NOT kFlush));
					END; {if}
				END; {if}
				
				{ mark file missing in control block }
				thePubCB^^.fileMissing := TRUE;
			END
			ELSE deleteIt := FALSE;
			
			{ now release the control block we requested }
			IgnoreOSErr(dpReleasePubCB(thePubCB, thisApp));
		END; {if}
		
		IF deleteIt THEN
		BEGIN
			{ delete pub file } 
			FailOSErr(FSpDelete(editionFile));
		END;
			
		Success(fi);
	END; { dp_DeleteEditionContainerFile }	
	
	

	{------------- dp_OpenEdition -------------} 
	FUNCTION dp_OpenEdition(sectionH: SectionHandle; VAR sectCB: SIOCBHandle): OSErr;
	VAR
		thePubCB:		PubCBHandle;
		thisApp:		AppRefNum;
		openerParams:	EditionOpenerParamBlock;
		fi:				FailInfo;
	BEGIN
		sectCB := NIL;
		
		{ set up failure handling }
		IF isFailure(fi, dp_OpenEdition) THEN
		BEGIN
			IF sectCB <> NIL 
				THEN DisposHandle(Handle(sectCB));
			sectCB := NIL;
			SIOCBHandle(sectionH^^.refNum) := NIL;
			EXIT(dp_OpenEdition);
		END; {if}

	{$IFC qDebug }
		FailOSErr(dpCheckSection(sectionH));
	{$ENDC }
	
		{ make sure section can read }
		IF NOT bTST(sectionH^^.kind, kCanReadEditionsBit) 
			THEN FailOSErr(permErr);
		
		{ make sure this section is not already reading  }
		IF SIOCBHandle(sectionH^^.refNum) = NIL THEN 
		BEGIN
			{ be sure we have a control block }
			thePubCB := PubCBHandle(sectionH^^.controlBlock);
			IF thePubCB = NIL THEN FailOSErr(fnfErr);
			
			{ make sure control block is up to date }
			FailOSErr(dpPubSync(thePubCB));

			{ create a Section I/O record for this Section }
			FailOSErr(dpCreateSIOCBRecord(sectionH, 0, NIL, sectCB));
			SIOCBHandle(sectionH^^.refNum) := sectCB;			

			{ call EditionOpener }
			openerParams.info := thePubCB^^.info;
			openerParams.sectionH := sectionH;
			FailOSErr(dp_GetCurrentAppRefNum(thisApp));
			FailOSErr(dp_CallEditionOpenerProc(eoOpen, openerParams, thisApp^^.emulator));
			
			{ set up fields in I/O control block, now that we have them }
			WITH sectCB^^ DO
			BEGIN
				{sectCB^^.}ioRefNum	:= openerParams.ioRefNum;
				{sectCB^^.}ioProc	:= openerParams.ioProc;
			END; {with}
		END ELSE
		BEGIN
			sectCB := SIOCBHandle(sectionH^^.refNum);
			dp_OpenEdition := containerAlreadyOpenWrn;
		END;
		
		Success(fi);
	END; { dp_OpenEdition }
		


	{------------- dp_OpenNewEdition -------------} 
	FUNCTION dp_OpenNewEdition(sectionH: SectionHandle; fdCreator: OSType;
							sectionDocument: FSSpecPtr;
							VAR sectCB: SIOCBHandle): OSErr;
	VAR
		thisApp:			AppRefNum;
		thePubCB:			PubCBHandle;
		openerParams:		EditionOpenerParamBlock;
		localSectionDocument: FSSpec;
		openErr:			OSErr;
		fi:					FailInfo;				
	BEGIN
		sectCB := NIL;

		{ set up failure handling }
		IF isFailure(fi, dp_OpenNewEdition) THEN
		BEGIN
			IF sectCB <> NIL 
				THEN DisposHandle(Handle(sectCB));
			sectCB := NIL;
			SIOCBHandle(sectionH^^.refNum) := NIL;
			EXIT(dp_OpenNewEdition);
		END; {if}

		{ make local copy if not NIL }
		IF sectionDocument <> NIL THEN
		BEGIN
			BlockMove(Ptr(sectionDocument), @localSectionDocument, SizeOf(FSSpec));	{ localSectionDocument := sectionDocument^;}
			sectionDocument 		:= @localSectionDocument;
		END;

		{$IFC qRangeCheck }
		FailOSErr(dpCheckSection(SectionH));
		{$ENDC }
		FailOSErr(dp_GetCurrentAppRefNum(thisApp));
		thePubCB := PubCBHandle(sectionH^^.controlBlock);
	
		{ make sure section can write }
		IF NOT bTST(sectionH^^.kind, kCanWriteEditionsBit) 
			THEN FailOSErr(wrPermErr);

		{ make sure it is not already open }
		IF SIOCBHandle(sectionH^^.refNum) <> NIL THEN
		BEGIN
			sectCB := SIOCBHandle(sectionH^^.refNum);
			FailOSErr(containerAlreadyOpenWrn);
		END;
					
		{ make sure control block is not NIL }				{ <38, #c1-MTM-007> }
		IF thePubCB = NIL									{ <38, #c1-MTM-007> }
			THEN FailOSErr(nsvErr);							{ <38, #c1-MTM-007> }

		{ make sure control block is up to date }
		FailOSErr(dpPubSync(thePubCB));

		{ make sure that the edition file is open }
		WITH thePubCB^^ DO
		BEGIN
			IF IsEditionFile(info.fdType) 
			 {&} THEN IF (bAND(openMode, dmRequestWritePerm) = 0) THEN
			BEGIN
				{ ahh!! pub file is not open, probably means when the publisher was }
				{ instantiated, there was another publisher on another machine with file open. }
				{ So see if that publisher is gone and 'sectionH' can become the publisher. }
				{ Open the file, to lock it as the publisher }
				openErr := dpPubOpenFile(thePubCB, sectionH^^.kind);
				CASE openErr OF
					permErr, afpDenyConflict, afpAccessDenied:
						FailOSErr(openErr);
					noErr:
						;
					OTHERWISE
						FailOSErr(openErr);	
				END; {case}	
			END; {if}				
		END; {with}
		
		{ create a Section I/O record for this Section }
		FailOSErr(dpCreateSIOCBRecord(sectionH, 0, NIL, sectCB));
		SIOCBHandle(sectionH^^.refNum) := sectCB;

		{ call Edition Opener }
		openerParams.info 		:= thePubCB^^.info;
		openerParams.sectionH 	:= sectionH;
		openerParams.fdCreator 	:= fdCreator;
		openerParams.document	:= sectionDocument;
		FailOSErr(dp_CallEditionOpenerProc(eoOpenNew, openerParams, thisApp^^.emulator));
		
		{ set up fields in I/O control block, now that we have them }
		WITH sectCB^^ DO
		BEGIN
			ioRefNum	:= openerParams.ioRefNum;
			ioProc		:= openerParams.ioProc;
		END; {with}
		
		Success(fi);
	END; { dp_OpenNewEdition }
	

	
	{------------- dp_CloseEdition -------------} 
	FUNCTION dp_CloseEdition(sectCB: SIOCBHandle; appWasSuccessful: BOOLEAN): OSErr;
	VAR
		thisApp:		AppRefNum;
		sectionH:		SectionHandle;
		thePubCB:		PubCBHandle;
		params:			EditionOpenerParamBlock;
		originalEditionMdDate: TimeStamp;
		fi:				FailInfo;
	BEGIN
		DoNotPutInRegister(@sectionH);
		sectionH := NIL;	
		
		{ set up failure handling }
		IF isFailure(fi, dp_CloseEdition) THEN
		BEGIN
			IF (sectionH <> NIL) {&} THEN IF (SIOCBHandle(sectionH^^.refNum) <> NIL) THEN 
			BEGIN
				DisposHandle(Handle(sectCB));
				sectionH^^.refNum := NIL;
			END;
			EXIT(dp_CloseEdition);
		END; {if}

		FailOSErr(dp_GetCurrentAppRefNum(thisApp));

		IF sectCB = NIL THEN FailOSErr(rfNumErr);

		{ get preliminary stuff }
		sectionH := SectionHandle(sectCB^^.section);
		thePubCB := PubCBHandle(sectionH^^.controlBlock);
		
		{ make sure it is already opened by this section }
		IF SIOCBHandle(sectionH^^.refNum) <> sectCB THEN FailOSErr(rfNumErr);
	
		{ set up call to Edition Opener }
		params.info 	:= thePubCB^^.info;
		params.sectionH := sectionH;
		params.success	:= appWasSuccessful;
		params.ioRefNum	:= sectCB^^.ioRefNum;
		params.ioProc 	:= sectCB^^.ioProc;
		
		IF thePubCB^^.oldPubCB = NIL THEN 
		BEGIN 
			{ closing an OpenEdition }
			FailOSErr(dp_CallEditionOpenerProc(eoClose, params, thisApp^^.emulator));
			
			{ if successful, then mark that the subscirber has same stuff as edition }
			IF appWasSuccessful 
				THEN sectionH^^.mdDate := thePubCB^^.info.mdDate;
		END ELSE
		BEGIN 
			{ save off the mdDate of the last successful edition }
			originalEditionMdDate := thePubCB^^.oldPubCB^^.info.mdDate;
			
			{ closing an OpenNewEdition }
			FailOSErr(dp_CallEditionOpenerProc(eoCloseNew, params, thisApp^^.emulator));
			
			{ control block has changed }
			thePubCB := PubCBHandle(sectionH^^.controlBlock);
			
			IF appWasSuccessful THEN
			BEGIN
				{ sync the controlBlock to have the new mod. date }
				thePubCB^^.info.mdDate := sectionH^^.mdDate;

				{ only send read event to subscribers if data has changed }
				IF originalEditionMdDate <> sectionH^^.mdDate THEN
				BEGIN
					{ send read event to all automatic subscribers }
					FailOSErr(dpNotifySubscribers(thePubCB));
				END;
				
				{ now mark this section as 'publisher' for priority in goto publisher }
				FailOSErr(dpMakeItThePublisher(thePubCB, sectionH, thisApp));
			END; {if}

		END; {else}	

		{ dispose of the Section I/O record for this Section }
		SIOCBHandle(sectionH^^.refNum) := NIL;
		DisposHandle(Handle(sectCB));
		FailOSErr(MemError);
		
		Success(fi);
	END; { dp_CloseEdition }


	{-------------  dp_GetStandardFormats -------------} 
	FUNCTION dp_GetStandardFormats(container: EditionContainerSpec; VAR previewFormat: FormatType;
									preview, publisherAlias, formats: Handle): OSErr;
	CONST
		kNumberOfPreviewFormats  = 4;
		kNumberOfPriorityArrangements = 3;
	TYPE
		FormatArray = ARRAY [1..kNumberOfPreviewFormats] OF FormatType;
		FormatPriorityList = ARRAY [1..kNumberOfPriorityArrangements] OF FormatArray;
		FormatPriorityListAlt = PACKED ARRAY [1..SizeOf(FormatPriorityList)] OF CHAR;
	VAR
		thisApp:		AppRefNum;
		thePubCB:		PubCBHandle;
		openerParams:	EditionOpenerParamBlock;
		previewErr:		OSErr;
		aliasErr:		OSErr;
		formatsErr:		OSErr;
		i:				INTEGER;
		aFormat:		FormatType;
		formatsPriority:FormatPriorityList;
		thePriorityList:INTEGER;
		theError:		OSErr;
		opened:			BOOLEAN;
		fi:				FailInfo;
	BEGIN 
		DoNotPutInRegister(@thePubCB);
		DoNotPutInRegister(@opened);
		thePubCB := NIL;
		opened := FALSE;
		dp_GetStandardFormats := noErr;
		
		{ top level functions must set up a failure handler }
		IF IsFailure(fi, theError) THEN
		BEGIN
			IF opened THEN
			BEGIN
				openerParams.success	:= FALSE;
				IgnoreOSErr(dp_CallEditionOpenerProc(eoClose, openerParams, thisApp^^.emulator));
			END;
			IF thePubCB <> NIL 
				THEN IgnoreOSErr(dpReleasePubCB(thePubCB, thisApp));
			IF theError = eofErr 
				THEN dp_GetStandardFormats := noTypeErr	{ map end-of-file errors into noType errors }
				ELSE dp_GetStandardFormats := theError;
			EXIT(dp_GetStandardFormats);
		END;
	
		{ make sure we have a PubCB for it }
		FailOSErr(dp_GetCurrentAppRefNum(thisApp));
		FailOSErr(dpRequestPubCB(container.theFile, thisApp, kCheckExisting, kCanAllocateNew, thePubCB));

		{ call EditionOpener to open file }
		openerParams.info := thePubCB^^.info;
		openerParams.sectionH := NIL;
		FailOSErr(dp_CallEditionOpenerProc(eoOpen, openerParams, thisApp^^.emulator));
		opened := TRUE;
		
		{ look for a preview }
		previewErr := noErr;
		IF preview <> NIL THEN
		BEGIN
			previewErr := noTypeErr;
			
			{ set up priority of search }
			FormatPriorityListAlt(formatsPriority) := 'prvwPICTTEXTsnd prvwTEXTPICTsnd prvwsnd TEXTPICT';
			
			thePriorityList := 1; {default priority }
			IF LONGINT(openerParams.info.fdType) = LONGINT(kTEXTEditionFileType) 
			THEN thePriorityList := 2
			ELSE IF LONGINT(openerParams.info.fdType) = LONGINT(ksndEditionFileType) 
			THEN thePriorityList := 3;
			
			{ search for format }
			FOR i := 1 TO kNumberOfPreviewFormats DO
			BEGIN
				aFormat := formatsPriority[thePriorityList,i];
				IF dpTryToGetFormat(aFormat, preview, openerParams.ioRefNum, openerParams.ioProc) = noErr THEN
				BEGIN
					previewFormat := aFormat;
					previewErr := noErr;
					LEAVE;
				END; {if}
			END; {for}
		END; {else}
	
		{ look for 'alis' }
		aliasErr := noErr;
		IF publisherAlias <> NIL THEN
		BEGIN
			aliasErr := dpTryToGetFormat(kPublisherDocAliasFormat, publisherAlias, openerParams.ioRefNum, openerParams.ioProc);
		END; 

		{ look for 'fmts' }
		formatsErr := noErr;
		IF formats <> NIL THEN
		BEGIN
			formatsErr := dpTryToGetFormat(kFormatListFormat, formats, openerParams.ioRefNum, openerParams.ioProc);
		END; 
	
		{ call EditionOpener to close file }
		{openerParams.ioRefNum	already set }
		{openerParams.ioProc	already set }
		{openerParams.sectionH	already set }
		openerParams.success	:= FALSE;
		FailOSErr(dp_CallEditionOpenerProc(eoClose, openerParams, thisApp^^.emulator));
		opened := FALSE;
	
		{ release the PubCB }
		FailOSErr(dpReleasePubCB(thePubCB, thisApp));
		thePubCB := NIL;

		{ return an error code if any of the formats were not found }
		FailOSErr(aliasErr);
		FailOSErr(formatsErr);
		FailOSErr(previewErr);
		
		Success(fi);
	END; { dp_GetStandardFormats }	

