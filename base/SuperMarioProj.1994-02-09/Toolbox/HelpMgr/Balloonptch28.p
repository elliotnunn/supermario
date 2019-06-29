{
	File:		Balloonptch28.p

	Contains:	Pascal routines called from Balloonptch28.p

	Written by:	Randy Carr

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<47>	 3/23/92	JSM		OSEvents.p is obsolete, use Events.p.
		<46>	 11/1/91	KSM		Added gestaltHelpMgrExtensions set up and conditionalized it for
									theFuture. Decided not to change the gestaltHelpMgrPresent set
									up to BSET with gestalt constant since it shipped as (result:=1)
									and will always be that way.
		<45>	 2/20/91	RLC		ksm,#81872 - Fix bug that wasn't restoring port after 'About
									Balloon Help' dialog is dismissed.
		<44>	 1/23/91	RLC		<rnv> Remove calls to TextFont(), TextSize(), and ShowWindow()
									the copyright notice was removed from About… dialog as per John
									Sullivan.  We aren't doing any special checking for contents
									checking inside the alert either.
		<43>	 12/3/90	RLC		<ksm>Update for new Dmgr FrontWindowModal calls.
		<42>	10/23/90	RLC		<ngk> Eliminate unused globals when allocating our globals ptr.
									Toss code that used to be called from _InitWindows patch.
									Change code that handles the About Help… dialog to call
									the new dialog mgr set default item stuff.
		<41>	 9/23/90	RLC		Toss patch to InitWindows that was reallocating the layerptr
									each time InitWindows was called.  Change to have the
									HMSetBalloons(true) allocate the layer and the window only when
									balloon help comes on.
		<40>	 9/22/90	gbm		Change InitWindows patch to stop putting help layer in the
									system heap.
		<39>	 9/19/90	ngk		<ksm> Now explicitly call standard filter proc in
									ptchHMShowHelpAboutDialog
		<38>	 9/13/90	RLC		Change HMGlobals to be a ptr instead of a handle, as it gets
									allocated at init time (early) and preallocation of a master
									ptr; Update all references to the hmGlobalHandle to hmGlobalPtr.
		<37>	 8/31/90	RLC		Update file to use newest naming conventions in equates and put
									in support for patch to OSEventAvail to call HMBalloonBulk for
									sleeping applications in foreground.
		<36>	 8/20/90	RLC		Fix bug that ‘Bonehead Ed’ said was the correct way to test
									ColorQD is present.  This makes balloons hang off of the edge.
		<35>	 8/16/90	RLC		Change private interfaces to always return OsErr.
		<35>	  8/6/90	RLC		Shorten the about box code.
		<34>	 7/30/90	RLC		Update to support changes to about dialog.
		<33>	 7/24/90	RLC		Conditionally remove the “Feedback” balloon and update calls to
									HMShowBalloon to match real interface.
		<32>	 7/13/90	RLC		Shorten some global initialization, don't call SysEnvirons
									anymore, assume presence of LayerMgr.
		<31>	  7/3/90	RLC		Fix “Balloons On” Balloon.
		<30>	  7/2/90	RLC		Conditionally remove help icon tracking code.
		<29>	 6/22/90	RLC		Remove some false/NIL/ or 0 assignments that is done from
									NewHandleClear now.
		<28>	 6/14/90	RLC		Add routines to support “Balloons On” balloon and include
									BalloonDefs.p
		<27>	  6/8/90	KSM		Update for new IsFrontWindowModal.
		<26>	  6/7/90	VL		Remove InstallPatch code. Move installation code for  Gestalt
									function to Balloonpack28.a.
		<25>	  6/5/90	RLC		Fix some front window bugs.
		<24>	  6/1/90	RLC		Add call to ScriptMgr in InitLayers code to retrieve the help
									font & size.
		<23>	 5/30/90	RLC		Remove some patch references and update global record.
		<22>	  5/4/90	RLC		Patches CONST changes.
		<21>	  5/3/90	RLC		Update hmGlobalRecord to support lastModalDialog window.
		<20>	 4/18/90	RLC		Call DaveC's cool centering code for About… dialog.
		<19>	 4/10/90	KSM		Toss some stupid patches and add to InitWindows patch.
		<18>	  4/2/90	RLC		Rearrange some parts to allow for cutback in future.
		<17>	 3/28/90	RLC		Fix append items to help menu for twitches.
		<16>	 3/23/90	RLC		Add first time Balloon.
		<15>	 3/21/90	RLC		Add delay back in.
		<14>	 3/20/90	RLC		Fix Help menu update problem by calling ptchHMUpdateHelpMenu at
									ptch init time.
		<13>	 3/15/90	RLC		Update HMGlobalRecord again, fix NewGestalt call.
		<12>	  3/7/90	RLC		Fix hmgProcess globals.
		<11>	 2/23/90	RLC		Toss a bunch of globals from HMGlobalRecord.
		<10>	 2/12/90	RLC		Add code to support HMSetDialogResID & HMSetMenuResID.
		 <9>	  2/7/90	RLC		Initialize hmgTix := Tickcount;
		 <8>	  2/5/90	RLC		Add Delay items to About Dialog…
		 <7>	  2/5/90	RLC		Remove dead code, optimize, change Using dialog again.
		 <6>	 1/31/90	RLC		Optimize ptchHMBootInitHelpMgr & add HMPutDSPatch
		 <5>	 1/29/90	RLC		Fix Using Help… Modal dialog code.
		 <4>	 1/26/90	RLC		Update global record for hmgOurBalloon word.
		 <3>	 1/22/90	RLC		Fix debug options and remove obsolete routines.
		 <2>	 1/10/90	RLC		Add hmgTix to HMGlobalRecord.
	   <1.4>	11/30/89	RLC		Fixed dialog tracker and updated HMGlobalsRecord to match
									BalloonPack.p
	   <1.3>	11/15/89	RLC		Remove _Debugger calls for real builds.
	   <1.2>	11/13/89	RLC		Changed _IsDialogEvent patch call to HMTrackHelpIcon to check
									for _FrontWindow=DialogKind.
	   <1.1>	 11/7/89	RLC		Updated tracking routine for Help icon.
	   <1.0>	 11/2/89	RLC		Created Today.
}

{$ifc UNDEFINED HelpMgrTesting}
{$setc HelpMgrTesting = false}
{$endc}

{$setc HelpIncludeHelpIcon = false}

{$ifc HelpMgrTesting}
{$D+}
{$INIT+}
{$elsec}
{$D-}
{$INIT-}
{$endc}

UNIT Balloonptch28;

INTERFACE

USES
		Types, QuickDraw, ToolUtils, Fonts, Events, Controls,
		Windows, Dialogs, Files, Menus, Desk, Traps, OSUtils, Script,
		Resources, Memory, Packages, FixMath, Errors, Layers, Processes, Balloons, GestaltEqu, MenuMgrPriv, DialogsPriv;
	
CONST																		
		UnImplementedTrap		= $9F;
						
		{Using Help Dialog consts}
		UsingHelpOK				= 1;
		UsingHelpPict1			= 2;
		UsingHelpPict2			= 3;
		
		MBarHeight				= $BAA;
								

{$I BalloonDefs.p}						
							
FUNCTION  ptchHMPascalGetBalloons: BOOLEAN;
FUNCTION  ptchHMGestaltIsThereHelp(selector: OSType; VAR result: LongInt): OsErr;
FUNCTION  ptchHMBootInitHelpManager : OsErr;
								
PROCEDURE ptchHMToggleWhatIs;			{ maintenance routine for _SystemMenu patch }

{$ifc HelpIncludeHelpIcon}
PROCEDURE ptchTrackHelpIcon;			{ tracks the help icon in modal dialogs and alerts }
{$endc}

PROCEDURE ptchHMShowHelpAboutDialog;
PROCEDURE ptchHMCallTrackerInContext;


IMPLEMENTATION


PROCEDURE ptchHMUpdateHelpMenu;														EXTERNAL;
FUNCTION  ptchHMGetHelpGlobal: HMGlobalPtr;											EXTERNAL;
PROCEDURE ptchHMSetHelpGlobal(gp: HMGlobalPtr);										EXTERNAL;




FUNCTION  HMBalloonBulk: OsErr;
		INLINE $303C,$00FC,_Pack14;




{$ifc HelpMgrTesting}
PROCEDURE Assert(b: BOOLEAN; s: STR255);
	BEGIN
		if b then DebugStr(s);
	END;
{$endc}	


FUNCTION  IsThisAModalDialog: BOOLEAN;
	VAR class:	INTEGER;
		result:		OSErr;
	BEGIN
		result := GetFrontWindowModalClass(class);
		
		IF result <> noErr THEN
			IsThisAModalDialog := FALSE
		ELSE
			IsThisAModalDialog := (class = dBoxProc);
	END;


FUNCTION  ptchHMPascalGetBalloons: BOOLEAN; {we need this for PACK call: FUNCTION HMGetBalloons: BOOLEAN;}
	BEGIN
		ptchHMPascalGetBalloons := ptchHMGetHelpGlobal^.hmgWhatIs;
	END;

FUNCTION  ptchHMGestaltIsThereHelp(selector: OSType; VAR result: LongInt): OsErr;
	BEGIN
		ptchHMGestaltIsThereHelp := noErr;
		result := 1;							{<47> Show that the Help Mgr is present }
		{ Additional feature flags below }
{$IFC theFuture}
		BSET(result,gestaltHelpMgrExtensions);	{<47> Conditionalized OUT until extensions rolled in }
{$ENDC}
	END;

FUNCTION  ptchHMBootInitHelpManager : OsErr; { *** eventually we'll want to put this past cut off point }
	VAR	gp				: Ptr;
		mh				: MenuHandle;
		theUniverse		: SysEnvRec;
		r				: Rect;
	BEGIN
		ptchHMBootInitHelpManager := noErr;
		
		if ptchHMGetHelpGlobal <> NIL then exit(ptchHMBootInitHelpManager); { help manager is already initialized }
		
		gp := NewPtrSysClear(Sizeof(HMGlobalRecord)); {always make it clear, saves code this way!}
		if gp<>NIL then
			with HMGlobalPtr(gp)^ do
				begin
					hmgHasColorQD		:= IntegerPtr($28E)^ <= $3FFF;
					
					hmgItemNum			:= -1;
					hmgMenuID			:= -1;
					
					hmgDialogQueue		:= HHMHelpQueueRecord(NewHandleSysClear(Sizeof(INTEGER)));	{ just room for count }
					hmgMenuQueue		:= HHMHelpQueueRecord(NewHandleSysClear(Sizeof(INTEGER)));	{ just room for count }
					
					hmgDelay			:= 5;
					hmgTix				:= Tickcount;	{ initialize delay count }
					
				end else
					ptchHMBootInitHelpManager := MemError;

		ptchHMSetHelpGlobal(HMGlobalPtr(gp));
	END;
	

PROCEDURE ptchHMToggleWhatIs;
	BEGIN
		if HMSetBalloons(not ptchHMGetHelpGlobal^.hmgWhatIs)=noErr then
			ptchHMUpdateHelpMenu;
	END;
	
	
PROCEDURE ptchHMCallTrackerInContext;
	VAR	frontPSN			: ProcessSerialNumber;
		info				: ProcessInfoRec;
	BEGIN
		if GetFrontProcess(frontPSN)=noErr then
			if WakeUpProcess(frontPSN)=noErr then
				if HMBalloonBulk=noErr then; { this call is also reentrant protected }
							
		ptchHMGetHelpGlobal^.hmgOSEventReentrantCount := 0;
	END;
	
	
{$ifc HelpIncludeHelpIcon}	

PROCEDURE ptchTrackHelpIcon;
{	This code pops up a duplicate pull-down Help menu when the mouse goes down over the
	Help icon.  As of A12, we're no longer including this source.  Maybe someday, when
	I get my way.
}
	VAR	pt					: Point;
		d					: DialogPtr;
		theID				: INTEGER;
		theType				: ResType;
		name				: STR255;
		offset				: Point;
		iconRect			: Rect;
		hDITL				: hItemList;
		pItem				: pDITLItem;
		hItems				: hItemList;
		USB					: SByteArray;
		err					: OsErr;
		value				: INTEGER;
		dataSize,i			: INTEGER;
		numItems			: INTEGER;
		trackRect			: Rect;
		h					: Handle;
		originalState		: BOOLEAN;
		foundHelpIcon		: BOOLEAN;
		aResult				: LongInt;
		aMenu				: MenuHandle;
		oldPort				: GrafPtr;
		ibits				: BitMap;
		
	BEGIN
		d				:= FrontWindow;
		
		if d=NIL then exit(ptchTrackHelpIcon);
		
		{ quik check to see if the front window is truely a dialog…
			This was a BAD bug in D19 build!!						}
		if WindowPeek(d)^.windowKind<>DialogKind then exit(ptchTrackHelpIcon);
		
		GetPort(oldPort);
		SetPort(d);
		
		foundHelpIcon	:= false;
		
		hItems		:= hItemList(DialogPeek(d)^.items);
		
		numItems	:= hItems^^.dlgMaxIndex + 1;
		pItem		:= @hItems^^.DITLItems;
		for i := 1 to numItems do
			begin
				USB.Int := 0;
				USB.SBArray[1] := pItem^.itmData[0];
				
				with pItem^ do
					if BAnd(itmType,$7F) = iconItem then
						if (IntegerPtr(@itmdata[1])^ = kHMHelpIconOff) |
								(IntegerPtr(@itmdata[1])^ = kHMHelpIconOn) then
							begin
								foundHelpIcon	:= true;
								trackRect		:= itmRect;
								leave;
							end;
				
				dataSize	:= BAnd(USB.Int + 1, $FFFE);
				pItem		:= pDITLItem(ptr(ord4(@pItem^) + datasize + sizeof(DITLItem)));
			end;
			
		
		if foundHelpIcon then { there is our Help Icon in the DITL handle }
			begin
				(*originalState := ptchHMPascalGetBalloons;
					we don't need this code unless we need to change the icon's appearance *)
				
				if button then { the button is down so check to see if it is in our Icon }
					begin
						HLock(Handle(hItems));
						GetMouse(pt);
						
						if PtInRect(pt,trackRect) then
							begin
								with iconRect do
									begin
										topLeft	:= trackRect.topLeft;
										right	:= left+20;
										bottom	:= top+20;
									end;
								
								aMenu := GetMHandle(kHMHelpMenuID);
								
								ptchHMUpdateHelpMenu;

								InsertMenu(aMenu,-1); {insert it in the hierportion of menulist}
								
								InvertRect(iconRect);
								
								with d^.portBits.bounds do
									aResult := PopUpMenuSelect(aMenu,
													trackRect.top-top+21,
													trackRect.left-left+1,0);
								
								case LoWrd(aResult) of
									kHMUsingHelpItem:	ptchHMShowHelpAboutDialog;
									kHMWhatIsItem:		ptchHMToggleWhatIs;
									{ ### otherwise handle a user menu item }
								end;
								
								InvertRect(iconRect);
								
								ptchHMUpdateHelpMenu;
								
								DeleteMenu(kHMHelpMenuID); {this is OK as hier portion is chked 1st}
							end;
														
						HUnlock(Handle(hItems));
					end;
					
				(*  we don't need this code unless we change the help icon's appearance
				if originalState<>ptchHMPascalGetBalloons then
					begin
						value := kHMHelpIconOff - ord(ptchHMPascalGetBalloons);
						
						h := GetIcon(value);
						
						if h<>NIL then
							begin
								SByteArray(pItem^.itmData[2]).Int := value;	{ force the itemnum to be this }
							
								pItem^.itmHndl := h;						{ & force the icon handle too}
								
								PlotIcon(trackRect,h);
								iconRect := trackRect;
								
								iconRect.top := trackRect.bottom - 13;
								InvalRect(iconRect);
								
								iconRect.top := trackRect.top;
								iconRect.left := trackRect.right - 13;
								InvalRect(iconRect);
								{Used to just use: PlotIcon(trackRect,h);
										but this wipes the entire 32X32 bit rectangle}	
							end;
						
						if HMRemoveBalloon=noErr then;	{calls _Pack14!! - make sure no balloons are up }
					end;
				*)
			end;
			
		SetPort(oldPort);
	END;
	
{$endc} { end of HelpIncludeHelpIcon conditional }

	
PROCEDURE ptchHMShowHelpAboutDialog;
	VAR		d			: DialogPtr;
			savePort	: GrafPtr;
			itemhit		: INTEGER;
	BEGIN
		GetPort(savePort);	{ <45> }
		d := GetNewDialog(kHMHelpID,NIL,Pointer(-1));
		if d<>NIL then
			begin
				if SetDialogDefaultItem(d,1)=0 then ;
				SetPort(d);
				ptchHMGetHelpGlobal^.hmgOurDialog := true;
				REPEAT
					ModalDialog(NIL,itemhit);
				UNTIL itemhit = UsingHelpOK;
				DisposDialog(d);
				ptchHMGetHelpGlobal^.hmgOurDialog := false;
			end;
		SetPort(savePort);	{ <45> always restore the original port as we'll end up with the wmgrport if we don't }
	END;
			
	
END.
