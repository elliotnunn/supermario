{
	File:		dpInit.inc.p

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<14>	  4/5/91	ngk		MM, #86190: remove unlock from end of QuitPack. This was a
	   								remnant from the days before the package dispatcher.  Now
									the package dispatcher will unlock and respect the lock 
									count it keeps.
		<13>	11/26/90	ngk		<DFH> Use dpSetCurrentAppRefNum.
		<12>	  7/2/90	ngk		Use NewPtrSysClear to save code.
		<11>	 5/31/90	ngk		Moved last edition used to preference folder. Install and
	   								remove a volume notification proc.
		<10>	  5/3/90	ngk		Changed GetLastEditionContainerUsed to use same folder as last
									edition if it can.
		 <9>	  4/7/90	ngk		Use new failure mechanism.
		 <8>	 3/25/90	ngk		Script now in EditionContainerSpec
		 <7>	 3/10/90	ngk		Moved locking and unlocking of package to dispatcher. Removed
									dialogCodeH.
		 <6>	 2/25/90	ngk		Use GetAppSignature instead of GetCurrentAppSignature.
		 <5>	 1/26/90	ngk		Changed Alias calls to new non-pointer style
		 <4>	 1/22/90	ngk		Used symbolic names for default opener. Fixed
									dpRetreivedSavedLastContainer to not mount servers.
		 <3>	  1/8/90	ngk		Renamed dp_GetCurrentAppRefNum.
		 <2>	  1/6/90	ngk		Added GetEditionOpenerProc & SetEditionOpenerProc. Moved all
									once-only init code into here. Moved saving and restoring of
									last container to here.
	   <1.8>	 12/1/89	ngk		Fixed a bug in dp_QuitEditionPack of unassigned return value.
	   <1.7>	11/13/89	ngk		InitDP & dp_QuitEditionPack now allow pack11 to be relocated.
	   <1.6>	 11/4/89	ngk		Remove globals when no apps using pack.
	   <1.5>	10/13/89	ngk		Added caching of dialogCodeH (dialog code resource)
	   <1.4>	 10/2/89	ngk		Now call DialogsOnceOnlyQuit
	   <1.3>	 8/29/89	ngk		Now get apps signature at InitDPpack time. Restore old
									ExitToShell trap in dp_QuitEditionPackpack.
	   <1.2>	  8/8/89	ngk		Changed Quit to return an OSErr. Added
									PubControlBlockOnceOnlyInit call
	   <1.1>	 5/29/89	ngk		Changed InitDP to check package version instead of the glue
									patched _ExitToShell to call dp_QuitEditionPack to clean up after app
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}

	
{========================== low level ============================}

CONST
	kPack11Handle	=	$BD4;
		


	{-------------  dpOnceOnlyInit -------------}
	FUNCTION dpOnceOnlyInit: OSErr;
	VAR
		emGlobalsLoc:	EdtnMgrGlobalsPtrPtr;
		emGlobals:		EdtnMgrGlobalsPtr;
		theErr:			OSErr;	
		ignore:			OSErr;
	BEGIN
		{ allocate space for dp system globals in system heap }
		emGlobals := EdtnMgrGlobalsPtr(NewPtrSysClear(SizeOf(EdtnMgrGlobals)));
		theErr := MemError;
		IF theErr = noErr THEN
		BEGIN
			{ stuff handle to globals in special location }
			emGlobalsLoc := dpGetGlobalsLocation;
			emGlobalsLoc^ := emGlobals;
	
			WITH emGlobals^ DO
			BEGIN
				{ initialize list of PubCB's }
				pubCBlinks.nextPubCB := PubCBLinkHandle(emGlobalsLoc);
				pubCBlinks.prevPubCB := PubCBLinkHandle(emGlobalsLoc);
				
				{ initialize list of applications }
				{perAppListHead := NIL;}
				
				{ initialize poll timer }
				nextPollTime := TickCount+ kPollPeriod;
				
				{ initialize no goto happening }
				{gotoHere := NIL;}
						
				{ initialize no failure handler }
				{failureHandler := NIL;}
						
				{ initialize last edition file used }
				{lastEditionUsed := NIL;}
				
				{ initialize volume notification glue code }
				volumeNotifyCode[1] := $303C;	{ move.w #02F4,d0 }
				volumeNotifyCode[2] := $02F4;
				volumeNotifyCode[3] := $AC2D;	{ _EditionMgrVolumeNotifier,AutoPop }
			
				{ tell process manger to call me anytime volumes change }
				ignore := BeginSystemMode;
				{$IFC qCatchFailures}
				IF ignore <> noErr 
					THEN DebugStr('dpOnceOnlyInit: BeginSystemMode failed');
				{$ENDC}
				ignore := RequestVolumeNotification(VNProcPtr(@volumeNotifyCode[1]),{refCon}0);
				{$IFC qCatchFailures}
				IF ignore <> noErr 
					THEN DebugStr('dpOnceOnlyInit: RequestVolumeNotification failed');
				{$ENDC}
				ignore := EndSystemMode;
				{$IFC qCatchFailures}
				IF ignore <> noErr 
					THEN DebugStr('dpOnceOnlyInit: EndSystemMode failed');
				{$ENDC}
			END; {with}

		END; {if}
		
		dpOnceOnlyInit := theErr;
	END; { dpOnceOnlyInit }




	{-------------  dp_InitEditionPack -------------}
	FUNCTION dp_InitEditionPack(versionAppKnows:INTEGER): OSErr; { called once by each app at start }
	TYPE
		phPackHeader=	^hPackHeader;
		hPackHeader	=	^pPackHeader;
		pPackHeader	=	^PackHeader;
		PackHeader	=	RECORD
							bra:		INTEGER;
							pack:		ResType;
							number:		INTEGER;
							version:	INTEGER;
						END;
	VAR
		package:		hPackHeader;
		emGlobals:		EdtnMgrGlobalsPtr;
		appsGlobals:	PerAppGlobalsHandle;
		fi:				FailInfo;		
		anErr:			OSErr;
	BEGIN
		appsGlobals := NIL;
		
		{ make sure was caller know right routines }
		package := phPackHeader(kPack11Handle)^;
		IF (package^^.version <> versionAppKnows) & (versionAppKnows <> 16) THEN { 16 is emulated }
		BEGIN
			dp_InitEditionPack := editionMgrInitErr;
			EXIT(dp_InitEditionPack);
		END; {if}
			
		{ make sure we are not purgable down }
		HNoPurge(Handle(package));
			
		emGlobals := dpGetGlobalsLocation^;
		IF emGlobals = NIL THEN
		BEGIN
			anErr := dpOnceOnlyInit;
			IF anErr <> noErr THEN
			BEGIN
				dp_InitEditionPack := anErr;
				EXIT(dp_InitEditionPack);
			END; {if}
		END; {if}
		
		{ set up failure handling, now that globals are around }
		IF isFailure(fi, dp_InitEditionPack) THEN
		BEGIN
			IF appsGlobals <> NIL 
				THEN DisposHandle(Handle(appsGlobals));
			EXIT(dp_InitEditionPack);
		END; {if}

		{ allocate space for apps dp globals in system heap }
		appsGlobals := PerAppGlobalsHandle(NewHandleSys(SizeOf(PerAppGlobals)));
		FailOSErr(MemError);
		
		{ initialize the globals }
		HLock(Handle(appsGlobals));
		WITH appsGlobals^^ DO
		BEGIN
			FailOSErr(GetCurrentProcess(appPSN));
			FailOSErr(GetAppSignature(appPSN, signature));

			sectionHandleListHead := SectionHandle(kNoMoreSections);
			emulator := EditionOpenerProcPtr(kStandardOpenerProcPtr);
			
			initVersion := versionAppKnows;
			
			{$IFC qUseAppName}
			BlockMove(Ptr($910), @appName, 31);				
			{$ENDC}	
			
			{ insert in list of app globals }
			emGlobals := dpGetGlobalsLocation^;
			nextApp := emGlobals^.perAppListHead;
			emGlobals^.perAppListHead := appsGlobals;
		END; {with}
		HUnLock(Handle(appsGlobals));
		
		{ remember this app's globals in switched low-mem }
		dpSetCurrentAppRefNum(appsGlobals);
		
		Success(fi);
	END; { dp_InitEditionPack }

		
		
	{-------------  dp_GetEditionOpenerProc -------------}
	FUNCTION dp_GetEditionOpenerProc(VAR appsEmulator: EditionOpenerProcPtr): OSErr;
	VAR
		anErr:		OSErr;
		thisApp:	AppRefNum;
	BEGIN
		anErr := dp_GetCurrentAppRefNum(thisApp);
		
		IF anErr = noErr 
			THEN appsEmulator := thisApp^^.emulator;
		
		dp_GetEditionOpenerProc := anErr;
	END; { dp_GetEditionOpenerProc }


	{-------------  dp_SetEditionOpenerProc -------------}
	FUNCTION dp_SetEditionOpenerProc(appsEmulator: EditionOpenerProcPtr): OSErr;
	VAR
		anErr:		OSErr;
		thisApp:	AppRefNum;
	BEGIN
		anErr := dp_GetCurrentAppRefNum(thisApp);
		
		IF anErr = noErr 
			THEN thisApp^^.emulator := appsEmulator;
		
		dp_SetEditionOpenerProc := anErr;
	END; { dp_SetEditionOpenerProc }


		
	{-------------  dp_QuitEditionPack -------------}
	FUNCTION dp_QuitEditionPack: OSErr;
	VAR
		thisApp:		AppRefNum;
		pack:			Handle;
		appsGlobals:	PerAppGlobalsHandle;
		emGlobalsLoc:	EdtnMgrGlobalsPtrPtr;
		ignore:			OSErr;
	BEGIN
		dp_QuitEditionPack := noErr;
		
		IF dp_GetCurrentAppRefNum(thisApp) = noErr THEN
		BEGIN	
			{ decrement UseCount in proper PupCBs, then remove if needed }
			dpRemoveAppsPubCBs(thisApp);
		
			{ remove Apps globals from list }
			RemoveAppsGlobals(thisApp);
		
			{ If no more Apps using this PACK then mark it purgeable }
			IF dpGetGlobalsLocation^^.perAppListHead = NIL THEN
			BEGIN
				emGlobalsLoc := dpGetGlobalsLocation;
				
				{ tell process manger to not call me anytime volumes change }
				ignore := DeclineVolumeNotification(VNProcPtr(@emGlobalsLoc^^.volumeNotifyCode[1]),{refCon}0);
				{$IFC qCatchFailures}
				IF ignore <> noErr 
					THEN DebugStr('dp_QuitEditionPack: DeclineVolumeNotification failed');
				{$ENDC}

				{ remove globals }
				DisposPtr(Ptr(emGlobalsLoc^));
				emGlobalsLoc^ := NIL;
				
				{ mark this pack purgable }
				pack := pHandle(kPack11Handle)^;
				HPurge(pack);							
	{ <MM, #86190> }
				{ ### we really should check the lockCount kept by the package dispatcher }
				{ and force it to 1.  If we don't force it to 1, the lock count will }
				{ stay at that level and this pack will never get unlocked.  }
				{ One condition that could corrupt the lock count is }
				{ if an ExitToShell happens while in an EditionMgr routine. } 
			END; {if}
		END; {if}
		
	END; { dp_QuitEditionPack }
	
	


	