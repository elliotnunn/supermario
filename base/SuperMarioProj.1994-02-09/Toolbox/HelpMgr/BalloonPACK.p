{__________________________________________________________________________________________________
	File:		BalloonPACK.p

	Contains:	Pascal support routines for _Pack14 (HelpMgr)

	Written by:	Randy Carr

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM3>	 2/17/93	kc		Allocate pictures in the System Heap because they persist longer
									than the application heap.
		<81>	  6/2/92	KST		#1030643 <JH>: We have only 1 default message for the floating
									window instead of two (floating windows are all active).
		<80>	 5/14/92	KST		#1027497 <JH>: #1025797,#1027497: In '__HMGetWindowPartCode',
									defined 2 new window parts when mouse is over a floating window.
									Provided default message so that Balloon help can work when
									mouse is over a floating window.
		<79>	 4/23/92	JH		#1027497,<YK>: CubeE adds a IMlayer for use by floating windows
									that input windows need. Unfortunately, the Help mgr doesn't
									know anything about this new layer. So I'm changing
									__HMGetWindowPartCode to call the TSM call FindServiceWindow
									before calling FindWindow. If FindServiceWindow returns a part
									code in a real window we use GetParent to find IMlayer. Swap
									that layer in, let the help mgr do its thing, and then swap the
									old layer back.
		<78>	  4/4/92	csd		#1017165,<BBM>: In HMShowWindowPartBalloon, set the port to the
									BalloonWindow before calling ReplaceText since ReplaceText uses
									the current port’s font and script information.
		<77>	 3/23/92	JSM		OSEvents.p is obsolete, use Events.p.
		<76>	11/21/91	csd		#83334: Used a single string with ReplaceText instead a two
									strings with Concat to build "This window belongs to the
									application “xxx”. To make this window active, click in it."
		<75>	 11/3/91	KSM		In theFuture, we should not arbitrarily limit the size of
									balloons (per Randy).
		<74>	  4/1/91	RLC		vl, #85955- FIx bug that wasn't respecting current process when
									tracking dialogs that contain a help item in the DITL handle.
		<73>	 3/19/91	RLC		ksm,#BH-TCA-0083 - FIx bug that wasn't passing window method
									parameter to HMShowBalloon() for 'hdlg' & 'hrct' resources.
		<72>	 3/13/91	RLC		ksm,#84586 - Change reset of help msgs using a text edit handle
									that are passed to HMShowBalloon to force the text edit handle
									to the help mgr font.
		<71>	 2/20/91	RLC		ksm,#BBSLostItsMind - Add a bracket back in that somehow got
									left out of a comment in the HMSelectBestVariant routine.
		<70>	 2/20/91	RLC		ksm,#82854 - Add a check in HMScanTemplateItems that makes sure
									that the window we're examining is a dialog if the type if
									kHMDialogResType. The call to FindDItem in HMSameItemState can
									otherwise be made on a non-Dialog window (i.e. Crash!).
		<69>	 2/19/91	RLC		ksm,#81544 Fix balloon positioner to make sure all variants are
									tried when initial variant does not fit. This fixes the clipped
									balloon for Empty Trash and KeyCaps. GO5 approved.
		<68>	  2/8/91	RLC		<ksm> #82161 Add a call to strip the null character from the
									processes that are desk accessories. This causes text edit to
									word break the process name leaving beginning and ending quotes
									on different lines of the balloon. <BHTCA0080> = force grayed
									controls to get the HMDisabled state in all cases.
		<67>	 1/24/91	RLC		<gbm> #81591 Change __HMFillInDialogMessage() calls to pass the
									resource file refnum from HMScanTemplateItems(). Those routines
									will call GetResource() for file refnum<>-1 otherwise call
									Get1Resource() [which means specifically search a resource file
									for a help resource]. Also changed occurrances of hmmPic to
									hmmPict as per Scott Boyd's changes to Balloons.p.
		<66>	 1/22/91	RLC		<dc> #81592 Fix bug that leaves a balloon up if the mouse
									moves from a system tracked balloon to the menubar and remove
									the extraneous tip parameter from HMShowTitleBalloon() routine.
		<65>	 1/10/91	RLC		<stb> Change the code for HMTrackModalItems and
									HMScanTemplateItems to not set the hmgLastItem and
									hmgLastItemState when the 'hdlg' resource that we're tracking
									returns a hmSkippedBalloon for an item's message.  This makes
									sure that the 'hdlg' resource indicated by the HMSetDialogResID
									setting gets checked in case the resource comes later in the
									resource map.
		<64>	 1/10/91	RLC		<ksm> Fix __HMPinTipAndAltRect routine to do the right thing if
									tip is outside screenbounds and after correction is still
									outside altRect.
		<63>	12/17/90	RLC		<ksm> Change HMSetBalloons to always remove the balloon before
									disposing of the layer when balloon help is turned off else we
									risk orphaning the balloon window in a non-existent layer.
		<62>	12/14/90	RLC		<ngk> Add check to __HMShowWindowPartBalloon so that unhilited
									windows that are behind a movable modal get the right balloon.
									(i.e. you can switch to another layer, but you cannot bring an
									inactive window to the front) Remove HMNewBestWorld conditional
									and the older positioning code that was excluded by it. Change
									giant case statement into table driven HMVarCodeToBias() asm
									routine (saves 238 bytes). Changed kHMFinderOnly to
									kHMActiveApplOther to support menu items in Process menu that
									are marked. Make HMCountDITLHelpItems external so that Standard
									file can call it from the private interface.
		<61>	12/14/90	dba		<JDR> get rid of LongIntPtr, now in Types.p
		<60>	 12/3/90	RLC		<ksm> Add check to __HMShowWindowPartBalloon so that unhilited
									windows that are behind a movable modal get the right balloon.
									(i.e. you can switch to another layer, but you cannot bring an
									inactive window to the front.
		<59>	11/16/90	RLC		<ksm> Always call HMFillInMenuMessage when a modal dialog is up
									so that the disabled AppleMenu items get the right help string.
									Fix bug that would not clear the pic VAR if HMCalculateBalloon
									was called too frequently and not enough time elapsed. Roll in
									new HMBestWorld calculation to fix all those agonizing balloons
									that hang offscreen bugs.  The new routine is simpler, smaller,
									and tries more ways to get the balloon to fit.
		<58>	 10/4/90	RLC		<ksm> Fix mem leak that wasn't always disposing of the picture
									handles. The bug was that if the mouse was moving quickly
									enough to cause HMCalculateBalloon to abort its calculation,
									HMCalculateBalloon would return hmBalloonAborted, but with a
									valid picHandle.  In this case, the caller must dispose of
									the picture by calling KillPicture.  Remove references to 
									hmgContentWindow and hmgOurBalloon variables as these were
									internal to the .a code of which is now calling the external
									routine HMScanTemplateItems. Fixed bug in HMScanTemplate that
									wasn't obeying the hmAbsoluteCoords option in the resource.
									Change code that handles picture resources or picHandles when
									showing a balloon: we don't copy the picture data, we just use
									the pic as is and stuff it in the windowPic field. Call
									DisposeWindow instead of CloseWindow for custom balloon winds.
		<57>	 9/30/90	RLC		Change hits in the 'inGrow' message of a window to be
									'inContent'. Reduce more redundant code in TrackModalHelp
									routines by consolidating some code into:__HMSameItemState (this
									saved >370 bytes).
		<56>	 9/26/90	RLC		Make a change to the automatically generated window part
									balloons to allow non-front windows that are hilighted to get
									'front window' balloons. (This will allow windoids and other
									palette windows that respect the window record's hilited boolean
									to work correctly).
		<55>	 9/25/90	RLC		Consolidate by having HMExtractHelpMsg call HMGetIndHelpMsg
									instead of having separate (duplicate) code (saves >370 bytes).
									Change HMShowMenuBalloon call to only call HMRemoveBalloon if a
									new balloon is about to be displayed and add a call to
									HMRemoveBalloon if HMSavedBits… is called.
		<54>	 9/21/90	RLC		Add code to the HMGetBalloons toggle to create both help window
									and layer when the flag goes true and tear down the window &
									layer for false.  This means that the help layer won't be
									burdening the window list hierarchy unless balloon help is on.
		<53>	 9/16/90	RLC		(KSM also) Utilize itemReserved longword in HMShowMenuBalloon
									routine to draw that string instead of fetching a help message
									from the 'hmnu' resource.
		<52>	 9/15/90	RLC		Repair positioner code in HMBestWorld that was calculating
									balloon tip position incorrectly for balloons that failed to fit
									on screen after 2 re-fits.
		<51>	 9/13/90	RLC		Change references to our global handle (HMGlobalHandle) into
	   								references to a global ptr (HMGlobalPtr), rename all routines
									in this package to have __HM in front so that Balloons.p can
									be part of uses statement, remove all multiple derefs to use
									a single with as we don't have to worry about relocation of 
									handle's ptr.
		<50>	 8/31/90	RLC		Remove duplicate and extraneous equates.
		<49>	 8/20/90	RLC		Fix offscreen balloons bug that allows a balloon to hang off the
									edge.
		<48>	 8/20/90	RLC		Repair Philip's merge of the old file.
		<46>	 7/17/90	RLC		Fix HMExtract calling conventions & styled text edit balloon
									bugs.
		<45>	 7/16/90	RLC		Change format of Styled-Text edit.
		<44>	 7/13/90	RLC		Just about every interface routine got changed.  Improved speed
									of WDEF calc routines. Shortened code for everything. Made
									multiple helpItems in dialogs work, fixed offset as well.
		<43>	  7/6/90	RLC		Repair skip items in modal dialog to really be skipped instead
									of always removing balloons.
		<42>	  7/5/90	RLC		Fix multiple 'hdlg' occurrances.
		<41>	  7/2/90	RLC		Some short changes
		<40>	 6/19/90	RLC		More correction in variant central.
		<39>	 6/18/90	RLC		Repair pinning algo again & add variant sensitivity/
		<38>	 6/14/90	RLC		Include BalloonDefs.p for HMGlobalRecord.
		<37>	  6/8/90	KSM		Update for new IsFrontWindowModal.
		<36>	  6/7/90	RLC		Optimize a bit, fix misc positioning bugs.
		<35>	  6/5/90	RLC		Fix HMSetDialogResID & system menu title balloon tip.
		<34>	  6/1/90	RLC		Update global record
		<33>	 5/31/90	RLC		Fix possible address error for 68K machines in HMShowBalloon.
		<32>	 5/29/90	RLC		Add support routines for new HMGetHelpMenu selector.
		<31>	  5/4/90	RLC		Patches CONST changes.
		<30>	  5/3/90	RLC		Fix bugs in modal dialog balloons, launching, etc.
		<29>	 4/19/90	RLC		Add scriptmgr help font & size to HMSet & HMGet font calls.
		<28>	 4/17/90	RLC		Add reference to new utility routine: HMFillCitationString.
		<27>	 4/10/90	KSM		New constants for system menu patch.
		<26>	  4/5/90	RLC		Add support for the 'hovr' resource.
		<25>	  4/3/90	RLC		Fix Modal Dialog tracking
		<24>	  4/2/90	RLC		Reconnect HMSetDialogResID & HMSetMenuResID.
		<23>	 3/30/90	RLC		Make sure MF rect is in line with MultiFinder.
		<23>	 3/28/90	RLC		Add support for appends to the help menu.
		<21>	 3/26/90	RLC		Fix global lasts for menus AND saveBits options.
		<20>	 3/22/90	RLC		Remove hardcoded strings except for testing.
		<19>	 3/19/90	RLC		Bug Fixes galore (58881 thru 62272).
		<18>	 3/15/90	RLC		Fix all error return codes, abort remove if point is in our help
									balloon.
		<17>	  3/7/90	RLC		Fix the pt not in current layer stuff.
		<16>	 2/23/90	RLC		Repair HMSetMenuResID for Pascal3.1 & add other layer support.
		<15>	 2/22/90	dnf		Per RLC's instructions: Comment out code in HMSetMenuResID
									Function
		<14>	  2/8/90	RLC		Fix scanning of window parts to always return the correct
									windowpart balloon.
		<13>	  2/7/90	RLC		Add in change to respect tip & hotRect parameters in 'hdlg'
									resources.
		<12>	  2/5/90	RLC		Add support for delay in balloon drawing.
		<11>	 1/31/90	RLC		Revise HMGlobalRecord again.
		<10>	 1/26/90	RLC		Increase Performance.
		 <9>	 1/26/90	RLC		Update hmgGlobalRecord and support for a modal condition
									(again).
		 <8>	 1/25/90	RLC		Change shape of Balloon look.
		 <7>	 1/19/90	RLC		Update selectors and error codes.
		 <6>	 1/11/90	RLC		Change mechanism for controlling how system balloons appear.
		 <5>	  1/9/90	RLC		Add support for new resource format.
		 <4>	  1/3/90	RLC		Fix to remove balloons in a modal dialog case where the mouse
									leaves the dialog.
		 <3>	  1/3/90	RC		Change hdlgSaveBits option to only save bits if on.
		 <2>	12/20/89	RC		Add change to support Menu title balloons better.
	   <4.1>	12/15/89	RLC		Added TipProc, theProc, & variant support.
	   <4.0>	 12/6/89	RLC		Tweak tip when horizontal balloon crosses hotRect.
	   <3.9>	 12/1/89	RLC		Fix HMScanTemplate routine check to see if dialog is a modal
									dialog before setting flag.
	   <3.8>	11/30/89	RLC		Fixed bug in HMTrackTemplate code that couldn't handle HMSkip
									types & updated HMGlobalsRecord
	   <3.7>	11/21/89	RLC		Temp fix for bug in _DrawPicture & remember to TEDispose the
									TENew.
	   <3.6>	11/20/89	RLC		Update routines that find the helpItem parameter in a 'DITL'
									template handle.
	   <3.5>	11/16/89	RLC		Change parameter interface to HMTrackTemplateHelpItems.
	   <3.4>	11/15/89	RLC		Change 'hdlg' support to correctly handle local offsets.
	   <3.3>	11/13/89	RLC		Changed dialog tracking routines, window parts tracker.
	   <3.2>	 11/9/89	RLC		Fix bug in window part balloons for grow box.
	   <3.1>	 11/7/89	RLC		Add support for window parts tracking.
	   <3.0>	 11/2/89	RLC		Removed all non-PACK14 routines.
	   <2.9>	10/27/89	RLC		Fix bug when displaying khmmPict and PicHandle types that didn't
									set origin correctly.
	   <2.8>	 10/1/89	RLC		Added new routine HMGetStyledTE & Updated HMBalloonRect for
									HelpMsg types: StyledTE & STR.
	   <2.7>	 9/28/89	RLC		Fixed bug that didn't correct the Balloon tip if on GDevice
									monitors.
	   <2.6>	 9/27/89	RLC		Fixed positioner for Balloons in ModalDialogs.
	   <2.5>	 9/25/89	RLC		Added more support for MultiFinder appl icon.
	   <2.4>	 9/22/89	RLC		Fixed bug in HMGetGDevice & potential dangling pointer bug in
									HMSaveBalloonBits
	   <2.3>	 9/21/89	RLC		Changed Balloon WDEF ID to 126.
	   <2.2>	 9/21/89	RLC		Added better error checking on message passing to HMBalloonRect
									& HMCalcBalloon routines.
	   <2.1>	 9/20/89	RLC		Bug Fixes, change hmnu scanning routines to make sure
									HMSkipItems never display balloons.
	   <2.0>	 9/19/89	RLC		Bug Fix in ShowMenuTitle.
	   <1.9>	 9/19/89	RLC		Added support for Multifinder Balloons.
	   <1.8>	 9/18/89	RLC		Dialog Template scan routines changed.
	   <1.7>	 9/15/89	RLC		Changed Modal Dialog Tracking to use PopupMenu instead of ICON.'
	   <1.6>	 9/15/89	RLC		Changed Debuggers to Assert calls.
	   <1.5>	 9/13/89	RLC		Repaired Balloon Calculation routines, made MenuTitle Balloons
									better.
	   <1.4>	 8/29/89	RLC		Changed HMCalc & Changed all Balloon routines to use new calling
									convention.
				 8/22/89	RCarr		Rewrite nearly every interface!
	   <1.3>	 8/10/89	RLC		Added code for autocontents & added patch to _InitWindows
				  8/8/89	RCarr		Optimize by preallocating Help Layer @ _InitWindows time, fix
									bug that was calling _KillPicture twice when calling
									_DisposeWindow.
				 7/31/89	RCarr		Added support for autocontents patches for help mgr.
	   <1.2>	 7/20/89	RLC		Added patchcode to support help menu & move HMBalloonRect from C
									to Pascal.
				 7/20/89	RCarr		Added tons of support stuff & Help Menu
	   <1.1>	 7/13/89	RLC		Add HMBalloonRect to pack
	   <1.0>	 6/29/89	RLC		Created Today.
				 5/22/89	RCarr		New Today!
__________________________________________________________________________________________________}


{$ifc UNDEFINED HelpMgrTesting}
{$setc HelpMgrTesting = false}
{$endc}


{$setc AllowCloseViewChanges = false}

{$ifc HelpMgrTesting}
{$D+}
{$INIT+}
{$elsec}
{$D-}
{$INIT-}
{$endc}

UNIT BalloonPack;

INTERFACE

USES
		Types, QuickDraw, ToolUtils, Fonts, Events, Controls,
		Windows, Dialogs, Files, Menus, Desk, Traps, OSUtils, GestaltEqu,
		Resources, Memory, Packages, FixMath, Errors, Layers, Processes, MFPrivate, MenuMgrPriv, DialogsPriv, Balloons,
		TextServices;				{<79>}

CONST	
	{ Start of internal CONST }
	
	hmGetNextHelpItem			= -873;
	hmWasAppleMenu				= -874;

	{System STR# resource indexes}
	kHMMenuTitleIndex			= 1;
	kHMAboutHelpIndex			= 2;
	kHMAboutHelpDisabled		= 3;
	kHMReserved					= 4;
	kHMWhatIsHide				= 5;
	kHMInDragIndex				= 6;
	kHMInGrowIndex				= 7;
	kHMInGoAwayIndex			= 8;
	kHMInZoomIndex				= 9;
	kHMMultiFinderIndex			= 10;
	kHMInScrollBar				= 11;
	kHMInDisabledScrollBar		= 12;
	kHMInOtherWindow			= 13;
	kHMInLayerPreamble			= 14;
	kHMInLayerBody				= 15;
	kHMOutsideModalWindow		= 16;
	kHMAppleMenuTitle			= 17;
	kHMShowHideEnabled			= 18;
	kHMHideOthersEnabled		= 19;
	kHMShowAllEnabled			= 20;		
	kHMShowHideDisabled			= 21;		
	kHMHideOthersDisabled		= 22;		
	kHMShowAllDisabled			= 23;		
	kHMEasy1Access				= 24;
	kHMEasy2Access				= 25;
	kHMEasy3Access				= 26;
	kHMActiveApplEnabled		= 27;
	kHMActiveApplDisabled		= 28;
	kHMActiveApplChecked		= 29;
	kHMActiveApplOther			= 30;
	kHMActiveApplDisabledTitle	= 31;
	kHMActiveApplDisabledItem	= 32;
	kHMInFrontFloatingWindow	= 33;			{ for IM, mouse is over a floating window }



	kHMDelayTix				= 5;

	{Dialog item template type}
	kHelpItem				= 1;

	{Dialog item template offsets}
	kHelpItemType			= 1;	{new, allows the helpItem to contain more than just a resID}
	kHelpItemData			= 3;	{offset in HelpItem array item in DITL to read ResID}
	kHelpItemOffset			= 5;	{offset to reach overriding append offset for hdlg}

	{helpItem type constants}
	kScanhdlg				= 1;
	kScanhrct				= 2;
	kScanAppendhdlg			= 8;	{ <58> for some reason, this was a 3 in previous, sync up w/ Types.r }
			

	{ internal balloon creation consts (Tweak these values until we die) }
	DicksOffsetPixels		= 9; {was 9+3 for Dick's design}
	HMMenuOffsetPixels		= 8;
	kHMFrameThickness		= 6;
	
	kHMVertContentMargin	= 13;
	kHMHorizContentMargin	= 13;
	kHMScreenBorder			= 20;
	kHMCornerDiameter		= 16;
	kHMPointerSize			= 18;
	kHMBottomMargin			= 50;
	
	kHMShadowThickness		= 2;
	
	kHMVertFrameMargin		= (kHMPointerSize + (kHMShadowThickness * 4));
	kHMHorizFrameMargin		= (kHMPointerSize + (kHMShadowThickness * 4));

	kHMMiscMargin			= 5;
		
	maxTETextArea			= 50 * 340;	{50 lines @ 340 pixels/line}
	MBarHeight				= $BAA;
	UnImplementedTrap		= $9F;
		
	wasWindow				= 0;
	wasMenu					= 1;
		
	kTitleType				= 1;
	kDialogType				= 2;
	kMenuType				= 3;
	kRectType				= 4;
	kEAType					= 5;
	kWindPartType			= 6;

	{hmgMenuID flag values}
	kwasMultiFinder			= -2;
	kwasWindowPart			= -3;
	kwasMenuTitle			= -4;
	kwasCleared				= -5;
	kwasEasyAccess			= -6;
	
	kGetFont				= 0;
	kSetFont				= 1;
	
	kVariantToRBS			= true;
	kRBSToVariant			= false;
	
	kShiftBit				= 0;
	kBottomBit				= 1;
	kRightBit				= 2;

TYPE
	EventRecPtr		= ^EventRecord;

	RBSRecord = Packed RECORD		{ this is it boys! }
			CASE INTEGER OF
				0:	(
					Filler1		: 0..8191;
					rightValue	: 0..1;		{ if 1 then balloon is hanging right }
					bottomValue	: 0..1;		{ if 1 then balloon is hanging down }
					shiftValue	: 0..1;		{ if 1 then balloon has vertical tip }
					);
				1:	(
					Whole	: INTEGER;
					);
		END;
		
	biasRecord	= Packed RECORD
			hShift			: INTEGER;
			vShift			: INTEGER;
			hCornerVector	: INTEGER;
			vCornerVector	: INTEGER;
			hBaseVector		: INTEGER;
			vBaseVector		: INTEGER;
		END;

	
{$I BalloonDefs.p}


FUNCTION  __HMShowBalloon(aHelpMsg: HMMessageRecord;
						tip: Point;
						alternateRect: RectPtr;
						tipProc: Ptr;
						theProc,variant: INTEGER;
						method: INTEGER): OsErr;
FUNCTION  __HMRemoveBalloon: OsErr;
FUNCTION  __HMGetBalloons: BOOLEAN;
FUNCTION  __HMSetBalloons(flag: BOOLEAN): OsErr;
FUNCTION  __HMShowMenuBalloon(itemNum,itemMenuID: INTEGER;
							itemFlags,itemReserved: LongInt;
							tip: Point; alternateRect: RectPtr; tipProc: Ptr;
							theProc,variant: INTEGER): OsErr;
FUNCTION  __HMIsBalloon: BOOLEAN;
FUNCTION  __HMSetFont(font: INTEGER): OsErr;
FUNCTION  __HMSetFontSize(fontSize: INTEGER): OsErr;
FUNCTION  __HMGetFont(VAR font: INTEGER): OsErr;
FUNCTION  __HMGetFontSize(VAR fontSize: INTEGER): OsErr;
FUNCTION  __HMSetDialogResID(theResID: INTEGER): OsErr;
FUNCTION  __HMGetDialogResID(VAR theResID: INTEGER): OsErr;
FUNCTION  __HMSetMenuResID(theMenuID,theResID: INTEGER): OsErr;
FUNCTION  __HMGetMenuResID(theMenuID: INTEGER; VAR theResID: INTEGER): OsErr;
FUNCTION  __HMBalloonRect(aHelpMsg: HMMessageRecord; VAR coolRect: Rect): OsErr;
FUNCTION  __HMBalloonPict(aHelpMsg: HMMessageRecord; VAR coolPict: PicHandle): OsErr;
FUNCTION  __HMScanTemplateItems(whichID,whichResFile: INTEGER; whichType: ResType): OsErr;
FUNCTION  __HMGetBalloonWindow(VAR window: WindowPtr): OsErr;

{private calls}
FUNCTION  __HMCountDITLHelpItems(d: DialogPtr): INTEGER;
FUNCTION  __HMModalDialogMenuSetup(ModalDialogComingUp: BOOLEAN): OsErr;
FUNCTION  __HMTrackModalHelpItems: OsErr;			{ tracks items in modal dialogs and alerts }

FUNCTION  __HMDrawBalloonFrame(rgn: RgnHandle): OsErr;
FUNCTION  __HMSetupBalloonRgns(	varCode: INTEGER;
								VAR structRgn: RgnHandle;
								VAR contentRgn: RgnHandle;
								VAR bounds: Rect): OsErr;

PROCEDURE __HMShowEasyAccessBalloon;
PROCEDURE __HMShowTitleBalloon(menuID: INTEGER; flags: LongInt);
FUNCTION  __HMShowContentBalloon(tip: Point; alternateRect: Rect;
									theProc,variant: INTEGER;
									aHelpMsg: HMMessageRecord; options: LongInt): OsErr;

FUNCTION  __HMGetWindowPartCode(VAR window: WindowPtr): INTEGER;
FUNCTION  __HMShowWindowPartBalloon(window: WindowPtr; windowCode: INTEGER): BOOLEAN;
FUNCTION  __HMFillFrontProcessString(s: StringPtr): OsErr;
FUNCTION  __HMExtractHelpMsg(whichType: ResType;
								whichResID, whichMsg, whichState: INTEGER;
								VAR aHelpMsg: HMMessageRecord): OsErr;


IMPLEMENTATION


FUNCTION  __HMGetScreenGDevice(pt: Point): GDHandle;							FORWARD;
FUNCTION  __HMSaveBalloonBits(aHelpMsg: HMMessageRecord;
							tip: Point;
							alternateRect: RectPtr;
							tipProc: Ptr;
							variant: INTEGER;
							balloonType: INTEGER): OsErr;						FORWARD;
FUNCTION  __HMCalculateBalloon(	aHelpMsg: HMMessageRecord;
								tip: Point;
								alternateRect: RectPtr;
								VAR varCode: INTEGER;
								VAR bounds: Rect; VAR pic: PicHandle;
								balloonType: INTEGER;
								abortable: BOOLEAN): OsErr;						FORWARD;
								
PROCEDURE __HMTwitchVarCode(isVariantToRBS: BOOLEAN;
				FromValue: INTEGER; VAR ToValue: INTEGER);						EXTERNAL;
PROCEDURE __HMVarCodeToBiasRecord(varCode: INTEGER; VAR bias: biasRecord);		EXTERNAL;
FUNCTION  __HMSlopMouse(tip: Point): BOOLEAN;									EXTERNAL;
FUNCTION  __HMCallTipProc(tipProc: Ptr;
					tip: Point;
					structure: RgnHandle;
					VAR bounds: Rect;
					VAR varCode: INTEGER): OsErr;								EXTERNAL;
PROCEDURE __HMSetGetFontSizeGlobal(setit: INTEGER; VAR fontsize: LongInt);		EXTERNAL;
FUNCTION  __HMSaveBitsInRect(theRect: Rect): PixMapHandle;						EXTERNAL;
PROCEDURE __HMRestoreBitsToRect(thePM: PixMapHandle);							EXTERNAL;
FUNCTION  __HMFillCitationString(origString,stuffString: StringPtr;
								key: Char) : OsErr;								EXTERNAL;
FUNCTION  __HMCallCustomWDEFCalc(v: INTEGER; w: WindowPtr): INTEGER;			EXTERNAL;

FUNCTION  __HMGetIndHelpMsg(whichType: ResType;
								whichResID, whichMsg, whichState: INTEGER;
								VAR options: LongInt;
								VAR tip: Point;
								altRect: RectPtr;
								VAR theProc: INTEGER; VAR variant: INTEGER;
								VAR aHelpMsg: HMMessageRecord;
								VAR count: INTEGER): OsErr;						EXTERNAL;
FUNCTION  __HMBalloonBulk: OsErr;												EXTERNAL;
FUNCTION  __HMFillInMenuMessage(menuID,item,resID: INTEGER; flags: LongInt;
								VAR variant: INTEGER;
								VAR aHelpMsg: HMMessageRecord): OsErr;			EXTERNAL;

FUNCTION  __HMFillInDialogMessage(	resID,item,whichHelpItem,
								totalHelpItems,state,offset: INTEGER;
								resFile: INTEGER;
								VAR options: LongInt;
								VAR tip: Point; altRect: RectPtr;
								VAR theProc: INTEGER; VAR variant: INTEGER;
								VAR aHelpMsg: HMMessageRecord): OsErr;			EXTERNAL;

FUNCTION  __HMScanHRCTResource(whichWindow: WindowPtr; whichResID: INTEGER; whichPt: Point;
							VAR whichMsg: INTEGER): OsErr;						EXTERNAL;
							
FUNCTION  __HMGetMenuItemStatus(mh: MenuHandle; flags: LongInt;
								item: INTEGER): INTEGER;						EXTERNAL;

FUNCTION  __HMGetHelpGlobal: HMGlobalPtr;										EXTERNAL;
FUNCTION  __HMGetRealWindowHitResult(p: Point; w: WindowPtr): INTEGER;			EXTERNAL;
FUNCTION  __HMGetGlobalMouse: Point;											EXTERNAL;
PROCEDURE __HMGetTextFontStuff(VAR theFont: INTEGER; VAR theSize: INTEGER);		EXTERNAL;
FUNCTION  __HMShiftKey: BOOLEAN;												EXTERNAL;
PROCEDURE __HMStuffWindowInfo(window: WindowPtr; varCode: INTEGER; bounds: Rect);	EXTERNAL;
PROCEDURE __HMStripNullFromProcessName(s: StringPtr);							EXTERNAL;



{$ifc HelpMgrTesting}
PROCEDURE Assert(b: BOOLEAN; s: STR255);
	BEGIN
		if b then DebugStr(s);
	END;
{$endc}

PROCEDURE __HMDisposeRgn(r: RgnHandle);
	BEGIN
		if r<>NIL then
			DisposeRgn(r);
	END;
	
FUNCTION __HMCanPictureBeKilled(kind: INTEGER): BOOLEAN;	{ <59> Code saver }
	BEGIN
		__HMCanPictureBeKilled := (kind <> khmmPict) and (kind <> khmmPictHandle);
	END;
	
FUNCTION  __HMIsThisAModalDialog: BOOLEAN;
	VAR class		: INTEGER;
	BEGIN
		__HMIsThisAModalDialog := false;
		if GetFrontWindowModalClass(class) = noErr then
			__HMIsThisAModalDialog := (class = dBoxProc);
	END;

FUNCTION __HMFillFrontProcessString(s: StringPtr): OsErr;
	VAR	PSN		: ProcessSerialNumber;
		info	: ProcessInfoRec;
		result	: OsErr;
	BEGIN
		result := GetFrontProcess(PSN);
		
		if result=noErr then
			begin
				with info do
					begin
						processInfoLength	:= Sizeof(ProcessInfoRec);
						processName			:= s;
						processAppSpec		:= NIL;
					end;
				result := GetProcessInformation(PSN,info);
			end;
			
		__HMFillFrontProcessString := result;
	END;
	
	
FUNCTION __HMGetStyledHandleResource( textRect: Rect;
									textResType: ResType;
									textID: INTEGER): TEHandle;
	VAR	hTE			: TEHandle;
		hStyl		: Handle;
		hText		: Handle;
	BEGIN
		__HMGetStyledHandleResource := NIL;		{ assume that we didn't get it }
		if LongInt(textResType) = LongInt(kHMTETextResType) then
			begin
				hTE		:= TEStylNew(textRect,textRect);
				if hTE<>NIL then
					begin
						hText	:= GetResource(kHMTETextResType,textID); { we hope this is purgeable! }
						
						if hText<>NIL then { set up the TEHandle w the char data }
							begin
								hStyl	:= GetResource(kHMTEStyleResType,textID);
								
								HLock(hText);
								TESetText(Ptr(hText^),GetHandleSize(hText),hTE);
								HUnlock(hText);
								
								if hStyl<>NIL then
									SetStylScrap(0,$7FF,stScrpHandle(hStyl),false,hTE);
							end else
								begin
									if hTE<>NIL then
										TEDispose(hTE);
									hTE := NIL;
								end;
					end;
				__HMGetStyledHandleResource := hTE;
			end;
	END;
	
	

FUNCTION  __HMBalloonPict(aHelpMsg: HMMessageRecord; VAR coolPict: PicHandle): OsErr;
	VAR	tip		: Point;
		bounds	: Rect;
		varCode	: INTEGER;
	BEGIN
		LongInt(tip)	:= 0;
		varCode			:= 0;
		__HMBalloonPict	:= __HMCalculateBalloon(aHelpMsg,tip,NIL,
							varCode,bounds,coolPict,wasWindow,false {can't abort balloon if mouse is moving});
	END;
	
	
FUNCTION  __HMBalloonRect(aHelpMsg: HMMessageRecord; VAR coolRect: Rect): OsErr;
	VAR	aTE			: TEHandle;
		fInfo		: FontInfo;
		lineHeight	: LongInt;
		ph			: PicHandle;
		textH		: Handle;
		textArea	: LongInt;
		ht,wd		: LongInt;
		numLines	: LongInt;
		r,rp		: LongInt;
		tempstr		: STR255;
		tempRect	: Rect;
		result		: INTEGER;
		resizeAble	: BOOLEAN;
		tempFont	: INTEGER;
		tempSize	: INTEGER;


		FUNCTION FastSQRT(val: LongInt): LongInt;
			BEGIN
				FastSQRT := BSR(FracSqrt(Fract(val)),15);

				{ in C: FastSQRT = (long) FracSqrt((Fract)val) >> 15;}
			END;

		PROCEDURE ResizeTE(teh: TEHandle; width, height: INTEGER);
			VAR	ter	: Rect;
			BEGIN
				with teh^^ do
					begin
						ter := destRect;
						with ter do
							begin
								bottom	:= top + height;
								right	:= left + width;
							end;
						destRect := ter;
						viewRect := ter;
					end;
				TECalText(teh);
			END;


	BEGIN
		GetFontInfo(fInfo);
		with fInfo do
			lineHeight := ascent + descent + leading;

		__HMBalloonRect	:= noErr;
		
		wd				:= 0;
		ht				:= 0;

		aTE				:= NIL;
		result			:= noErr;
		resizeAble		:= false;
		
		SetRect(tempRect,5,5,100,100);	{ Start with this rect initially }
		
		case aHelpMsg.hmmHelpType of

			khmmString,khmmStringRes,khmmSTRRes:
				begin
					with aHelpMsg do
						begin
							tempstr := ''; { zero for failures }
							
							if hmmHelpType = khmmString then
								tempstr := hmmString
								
							else if hmmHelpType = khmmSTRRes then
								begin
									textH := Handle(GetString(hmmSTRRes));
									if textH<>NIL then
										tempstr := StringHandle(textH)^^;
								end else
									GetIndString(tempstr,hmmStringRes.hmmResID,
														hmmStringRes.hmmIndex);
								
							if length(tempstr) > 0 then
								begin
									aTE := TENew(tempRect,tempRect);

									{$ifc HelpMgrTesting}
									Assert(aTE=NIL,'HMBalloonRect[X]: a te handle was NIL');
									{$endc}

									if aTE<>NIL then
										begin
											TESetText(pointer(ord(@tempstr)+1),
													length(tempstr),aTE);
											textArea := lineHeight * StringWidth(tempstr);
											
										end else result := memFullErr;
										
								end else result := paramErr;
						end;
				end;
				
			khmmPict,khmmPictHandle:
				begin
					if aHelpMsg.hmmHelpType = khmmPict then
						ph := PicHandle(GetResource('PICT',aHelpMsg.hmmPict))
							else ph := aHelpMsg.hmmPictHandle;
							
					if ph<>NIL then
						begin
							with ph^^.picFrame do
								begin
									wd := right-left;
									ht := bottom-top;
								end;
			
						end else
							if aHelpMsg.hmmHelpType = khmmPict then
								result := resNotFound
									else result := paramErr;
							
					{$ifc HelpMgrTesting}
					Assert(ph=NIL,'HMBalloonRect[2]: a pic handle was NIL');
					{$endc}
				end;

			khmmTEHandle,khmmTERes:
				begin
					if (aHelpMsg.hmmHelpType = khmmTERes) then
						aTE := __HMGetStyledHandleResource(tempRect,kHMTETextResType,aHelpMsg.hmmTERes)
							else aTE := aHelpMsg.hmmTEHandle;
							
					if aTE<>NIL then
						begin
							resizeAble := (aTE^^.txSize<>-1);
							
							textH := aTE^^.hText;
							HLock(textH);

							if resizeAble then
								with aTE^^ do
									begin
										{ <72> don't need these anymore
										tempFont := txFont;
										tempSize := txSize;
										}
										__HMGetTextFontStuff(txFont,txSize);
										{ *** how do we reset the old TEHandle's font & size helpmgr font here? }														
										
										GetFontInfo(fInfo);
										with fInfo do
											lineHeight := ascent + descent + leading;
											
									end else
										lineHeight := TEGetHeight(0,$7FF,aTE) div aTE^^.nLines;
										
							textArea := lineHeight * TextWidth(textH^, 0, aTE^^.teLength);
							HUnlock(textH);
{$IFC NOT theFuture}		{<75>}
							if textArea > maxTETextArea then
								result := paramErr;
{$ENDC}
						end else result := paramErr;
				end;
			
			otherwise
				begin
					{$ifc HelpMgrTesting}
					NumToString(aHelpMsg.hmmHelpType,tempstr);
					DebugStr(concat('Bad HelpType (',tempstr,') passed to HMBalloonRect'));
					{$endc}
					
					__HMBalloonRect := hmUnknownHelpType;
					Exit(__HMBalloonRect);
				end;
		end;

		if result<>noErr then
			begin
				__HMBalloonRect := result;
				Exit(__HMBalloonRect);
			end;

		if aTE<>NIL then { for khmmPict or khmmPictHandle case, don't do resizing}
			begin
			
				if textArea=0 then
					r := 0 else
						r := FastSQRT(textArea);

				ht := (r * 1000) div 1272;				{(1272/1000)**2 approx golden ratio}

				{ make height a multiple of line height }
				rp := ht mod lineHeight;
				if (rp <> 0) then
					ht := ht + lineHeight - rp;

				numLines := ht div lineHeight;

				wd := textArea div ht;

				{ *** make sure longest word fits, someday! }

				ResizeTE(aTE, wd, ht);
					
				while (TEGetHeight(0,$7FF,aTE)>ht) do {loop until the # of lines fits in bounds}
					begin
						if (TEGetHeight(0,$7FF,aTE)>ht) then
							begin
								wd := (ht * 3236) div 2000;
								ResizeTE(aTE, wd, ht);
							end;

						if (TEGetHeight(0,$7FF,aTE)>ht) then
							begin
								wd := ((ht + lineHeight) * 3236) div 2000;
								ResizeTE(aTE, wd, ht);
							end;

						if (TEGetHeight(0,$7FF,aTE)>ht) then
							begin
								wd := (ht * 3236) div 2000;
								ht := ht + lineHeight;
								ResizeTE(aTE, wd, ht);
							end;
					end;

				if (TEGetHeight(0,$7FF,aTE)<ht) then
					begin
						ht := TEGetHeight(0,$7FF,aTE);
						ResizeTE(aTE, wd, ht); {resize again so HMCalc won't have to also}
					end;
				
				{ <72> don't reset the font and size of the text edit handle passed, as we've just calculated
						the balloon for this text edit handle using the help mgr font
				if resizeAble then
					begin
						with aTE^^ do
							begin
								txFont := tempFont;
								txSize := tempSize;
							end;
					end;
				}
				
				with __HMGetHelpGlobal^ do
					if hmgKeepTEHandle then
						hmgTEHandle := aTE else
							if aHelpMsg.hmmHelpType <> khmmTEHandle then {don't toss the supplied handle in khmmTEHandle case}
								TEDispose(aTE);
		end;

		SetRect(coolRect,0,0,wd,ht);
	END;


FUNCTION  __HMGetBalloons: BOOLEAN;
		
	BEGIN

			__HMGetBalloons := __HMGetHelpGlobal^.hmgWhatIs;
	END;

{$ifc AllowCloseViewChanges}	
FUNCTION __HMIsCloseViewActive: BOOLEAN;
	VAR	result			: LongInt;
	BEGIN
		__HMIsCloseViewActive := true;
		(*
		__HMIsCloseViewActive := false;
		if Gestalt('BSDa',result) = noErr then
			__HMIsCloseViewActive := (result <> 0);
		*)
	END;
{$endc}	

FUNCTION  __HMRemoveBalloon: OsErr;
	VAR	window 		: WindowPtr;
		windowPic	: PicHandle;
		oldPort		: GrafPtr;
		wPort		: GrafPtr;
		oldLayer	: LayerPtr;
		floatLayer	: LayerPtr;
		bounds		: Rect;
		refcon		: LongInt;
		balloonDown	: BOOLEAN;
		pt			: Point;
		windowCode	: INTEGER;
		floatwindowCode : INTEGER;
		ignoreErr	: OSErr;
	BEGIN
		with __HMGetHelpGlobal^ do
			begin
				{$ifc AllowCloseViewChanges}	
				if __HMIsBalloon then
					if __HMIsCloseViewActive then
						if __HMShiftKey then
							begin
								hmgCloseViewCount := 1 {means remove current balloon when the shift key goes up};
									
								__HMRemoveBalloon := hmCloseViewActive;
								exit(__HMRemoveBalloon);
							end;
				{$endc}
				
				
					
				if not hmgWhatIs then
					__HMRemoveBalloon := hmHelpDisabled;
							
				window		:= hmgWindow;
				balloonDown := false;	{ assume that a balloon wasn't removed }
				
				
				if (window<>NIL) then
					begin
						if hmgLayer<>NIL then
							oldLayer := SwapCurLayer(hmgLayer);
						
						windowPic	:= GetWindowPic(window);				{ <59> call toolbox instead of WindowPeeking record }
						refcon		:= GetWRefCon(window);					{ <59> call toolbox instead of WindowPeeking record }
						
						if windowPic<>NIL then								{ <58> make sure picture was actually there! }
							if __HMCanPictureBeKilled(refcon) then			{ <59> don't 'kill' a resource! }
								KillPicture(windowPic);						{ <54> but be sure to kill the picture }
								
						SetWindowPic(window,NIL);							{ <59> call toolbox instead of WindowPeeking record }
						
						if window = hmgHelpWindow then						{ we don't toss our window <54> }
							begin
								SetWRefCon(window,0);						{ <59> call toolbox instead of WindowPeeking record }
								HideWindow(window);							{ just hide it <54> }
							end else
								DisposeWindow(window);						{ this does a KillPicture(windowPic) automagically! }
		
						if hmgLayer<>NIL then
							SetCurLayer(oldLayer);
		
						hmgWindow			:= NIL;
						hmgTitleBalloon 	:= 0;		{ clear the flag that says that a menu title balloon is up }
						hmgItemNum			:= -1;
						
						balloonDown			:= true;
					end;
		
				if hmgSavedBitsHandle<>NIL then	{ <57> we don't need to set wmgrport anymore }
					begin
						__HMRestoreBitsToRect(hmgSavedBitsHandle);
						
						hmgSavedBitsHandle	:= NIL;	{ zero the handle [flag] }
						hmgItemNum			:= -1;	{ force new balloon to get shown }
						balloonDown			:= true;
					end;
		
				with hmgSlopRect do
					begin
						LongInt(topLeft) := 0;
						LongInt(botRight):= 0;
					end;
					
				with hmgItemRect do
					begin
						LongInt(topLeft) := 0;
						LongInt(botRight):= 0;
					end;
							
				hmgLastPartCode		:= 0;	
				hmgLastWindowPtr	:= NIL;
				
				hmgLastMenuID		:= -1;
				hmgLastEnabled		:= 0;
				hmgLastItem			:= 0;
				
				hmgState			:= -1;
				hmgMenuID			:= kwasCleared;
				
				{$ifc AllowCloseViewChanges}	
				if balloonDown then
					hmgCloseViewCount := 0;
				{$endc}

			end; { with __HMGetHelpGlobal^ }
			
		{ do this nonsense 'cuz the WindowMgr clips the WmgrPort to menubar,
			the result is that the pulldown menus get drawn w/o being erased first! }
		SetRect(bounds,-32768, -32768, 32767,32767);
		GetPort(oldPort);
		GetWMgrPort(wPort);
		SetPort(wPort);
		ClipRect(bounds);
		SetPort(oldport);

		if balloonDown then
			__HMRemoveBalloon := noErr
				else __HMRemoveBalloon := hmNoBalloonUp;
	END;


FUNCTION  __HMIsBalloon: BOOLEAN;
	BEGIN
		with __HMGetHelpGlobal^ do
			__HMIsBalloon := (hmgWindow<>NIL) | (hmgSavedBitsHandle<>NIL);
	END;


FUNCTION __HMSetBalloons(flag: BOOLEAN): OsErr; { the entire routine was rewritten <54> RLC }
	VAR	aLayer		: LayerPtr;
		fontAndSize	: HMFontRecord;
		gp			: HMGlobalPtr;
		oldLayer	: LayerPtr;
		oldZone		: THz;
		bounds		: Rect;
		chain		: WindowPeek;
		aMenu		: MenuHandle;
		result		: OsErr;
	BEGIN
		__HMSetBalloons := noErr;	{ in case we're exiting early }
		
		gp := __HMGetHelpGlobal;
		if gp<>NIL then
			with gp^ do
				begin
					if hmgWhatIs = flag then exit(__HMSetBalloons);	{ the state is already setup so exit now }
					
					{ reset the script info again }
					LongInt(fontAndSize) := GetScript(INTEGER(GetEnvirons(smSysScript)), smScriptHelpFondSize);
					LongInt(hmgFontAndSize)	:= LongInt(fontAndSize);	{ all this LongInt stuff is a code saver }
					
					if flag then { let's create the layer and window now! }
						begin
							chain := LayerPeek(GetRootLayer)^.subWindows; { get the first children of root }
							if chain<>NIL then { there is a child }
								begin
									{ *** Assert if the child is our help layer then everything is set already (how could this happen?) }
									{$ifc HelpMgrTesting} 
										Assert(chain = WindowPeek(hmgLayer),'HMSetBalloons: Help layer was not deallocated!');
									
										{walk the children of the root, looking for our help layer}
										repeat
											chain := chain^.nextWindow;
										until (chain = NIL) | (chain = WindowPeek(hmgLayer));
										
										Assert(chain<>NIL,'HMSetBalloons: Help layer was still in layer chain!');
									{$endc}
											
									{ get and set the rootlayer }
									oldLayer := SwapCurLayer(GetRootLayer);

									oldZone := GetZone;		{ get the current heap zone }
									SetZone(SystemZone);	{ point at the system heap }
									
									aLayer := NIL;
											
									{ create the help layer, having its rgns in sys heap }
									result	:= NewLayer(aLayer,true{visible},true{neveractive},
														pointer(-1){infront},LongInt($BADFACE){refcon});
									
									if result<>noErr then
										begin
											{$ifc HelpMgrTesting} 
											DebugStr('HMSetBalloons: Unable to create a help layer');
											{$endc}		
											
											aLayer := NIL;	{ this seems to be garbage sometimes when there isn't the memory }
										end else
											begin
												SetCurLayer(aLayer);

												SetRect(bounds,0,0,100,100);
												hmgHelpWindow := NewWindow(@hmgHelpWRecord,bounds,'Balloon',
																false,(16 * kBalloonWDEFID) + 0 {default varCode},pointer(-1),false,0);
											end;
										
									SetZone(oldZone);		{ restore the heap zone }
									hmgLayer := aLayer;		{ make our globals have that layer }
									SetCurLayer(oldLayer);	{ restore the current layer }
											
								end;	{ else there wasn't any root layer (how can this happen?) }
						end else
							begin		{ tear down the layer and window }
								{$ifc HelpMgrTesting} 
									if hmgHelpWindow = NIL then
										begin
											DebugStr('HMSetBalloons: we tried to deallocate the help window twice!');
											exit(__HMSetBalloons);
										end;

									if hmgLayer = NIL then
										begin
											DebugStr('HMSetBalloons: we tried to deallocate the help layer twice!');
											exit(__HMSetBalloons);
										end;
								{$endc}
								
								if __HMRemoveBalloon=noErr then; { <63> always hide the balloons if help is turned off }
								
								oldLayer := SwapCurLayer(hmgLayer);
								
								CloseWindow(hmgHelpWindow);	{ close the window because we've got our own storage for the wRecord }
								hmgHelpWindow	:= NIL;
								DisposeWindow(hmgLayer);	{ dispose window because we allocated it in the System heap via NIL }
								hmgLayer		:= NIL;
								
								SetCurLayer(oldLayer);		{ restore the current layer }
							end;
							
					hmgWhatIs := flag;	{ Set the HMGetBalloons global flag }
				end;

		__HMSetBalloons := noErr;
	END;



FUNCTION  __HMShowBalloon(aHelpMsg: HMMessageRecord;
						tip: Point;
						alternateRect: RectPtr;
						tipProc: Ptr;
						theProc,variant: INTEGER;
						method: INTEGER): OsErr;

	VAR	oldPort			: GrafPtr;
		oldZone			: THz;
		bounds			: Rect;
		wBounds			: Rect;
		temprect		: Rect;
		gp				: HMGlobalPtr;
		pic				: PicHandle;
		window			: WindowPtr;
		oldLayer		: LayerPtr;
		initialWindowPtr: WindowPtr;
		initialPartCode	: INTEGER;
		procID			: INTEGER;
		varCode			: INTEGER;
		wVarCode		: INTEGER;
		result			: OsErr;
		oldState		: SignedByte;
		
		PROCEDURE __HMSetLastGlobals;
			BEGIN
				with gp^ do
					begin
						if alternateRect<>NIL then
							hmgSlopRect := alternateRect^ else
								SetRect(hmgSlopRect,0,0,0,0);
									
						hmgLastPartCode		:= initialPartCode;
						hmgLastWindowPtr	:= initialWindowPtr;
						
						if hmgLastWindowPtr<>NIL then
							begin
								if BAnd($C000,CGrafPtr(hmgLastWindowPtr)^.portVersion)<>0 then
									temprect := CGrafPtr(hmgLastWindowPtr)^.portPixMap^^.bounds
										else temprect := hmgLastWindowPtr^.portBits.bounds;
										
								with hmgLastWindowPtr^.portRect do
									begin
										hmgLastWidth	:= (right-left);
										hmgLastHeight	:= (bottom-top);
										hmgLastLeft		:= temprect.left - left;
										hmgLastTop		:= temprect.top - top;
									end;
							end else
								begin
									hmgLastWidth	:= 0;
									hmgLastHeight	:= 0;
									hmgLastLeft		:= 0;
									hmgLastTop		:= 0;
								end;
								
						
					end;
			END;
	BEGIN
		gp := __HMGetHelpGlobal;
		
		if __HMGetBalloons 
			{$ifc AllowCloseViewChanges}	
				& (gp^.hmgCloseViewCount = 0) {CloseView is active, shift key was down, & balloon was up}
			{$endc} then
			begin
				__HMShowBalloon := hmBalloonAborted;
				
				
				
				if __HMRemoveBalloon=noErr then;
				
				initialPartCode	:= __HMGetWindowPartCode(initialWindowPtr);
				
				method := BAnd($0007,method);	{ strip off hi bits of method }
					
				{ *** we currently support only:
						kHMRegularWindow		= 0;		 Create a regular window floating above all windows
						kHMSaveBitsNoWindow		= 1;		 Just save the bits and draw (for MDEF calls
						kHMSaveBitsWindow		= 2;		 Regular window, save bits behind, AND generate update event
				}

				if method = kHMSaveBitsNoWindow then
					begin
						if theProc<>0 then {only support proc 0 when saving bits}
							__HMShowBalloon := hmOperationUnsupported else
								begin
									result := __HMSaveBalloonBits(aHelpMsg,tip,alternateRect,
															tipProc,variant,wasWindow);
															
									if result=noErr then
										__HMSetLastGlobals;
										
									__HMShowBalloon := result;
								end;
					end else
					begin		{ it must have been a kHMRegularWindow or kHMSaveBitsWindow method }
						GetPort(oldPort);

						with gp^ do
							if hmgLayer<>NIL then
								oldLayer := SwapCurLayer(hmgLayer);

						varCode := variant;
						result := __HMCalculateBalloon(aHelpMsg,tip,alternateRect,
														varCode,bounds,pic,wasWindow,true { will abort balloon if mouse moves });
						if result = noErr then
							begin
								if theProc<>0 then
									begin
										procID	:= theProc;
										varCode := variant;	{ override just calculated varCode in __HMCalculateBalloon
																as we have our own defProc whose variant could be different }
										
										if GetResource('WDEF',theProc)=NIL then { we couldn't load the custom 'WDEF' }
											result := resNotFound;
									end else
										procID := kBalloonWDEFID; { use our balloon 'WDEF' }
								
								if result = noErr then
									with gp^ do
										begin
											if theProc<>0 then
												begin
													oldZone := GetZone;		{ get the current heap zone }
													SetZone(SystemZone);	{ point at the system heap }
													
													{ only call new window if theProc isn't our balloon definition <54> }
													window := NewWindow(NIL,bounds,'appBalloon',false,(16 * procID) + varCode,
																			pointer(-1),false,0);
											
													SetZone(oldZone);
												end else
													begin
														{ change our balloon window's defproc variant to be the new one <54> }
														__HMStuffWindowInfo(hmgHelpWindow,varCode,bounds);
														
														{ make the showwindow call recalculate the window's regions <54> }
														with WindowPeek(hmgHelpWindow)^ do
															begin
																SetEmptyRgn(strucRgn);
																SetEmptyRgn(contRgn);
															end;

														{ make the current balloon our window <54> }
														window := hmgHelpWindow;
													end;
											
											if window<>NIL then
												begin
													SetPort(window);
			
													if (method = kHMSaveBitsWindow) | (tipProc<>NIL) then	{ <58> check tip procedure too }
														begin
															if __HMCallCustomWDEFCalc(wCalcRgns,window)=noErr then;
															
															if tipProc<>NIL then							{ <58> give the caller a chance to abort }
																begin
																	with WindowPeek(window)^ do				{ <58> using the balloon window information… }
																		begin
																			wBounds		:= bounds;
																			wVarCode	:= varCode;

																			result := __HMCallTipProc(tipProc,tip,strucRgn,wBounds,wVarCode); { <58> see if caller doesn't like balloon }
																	
																			if result = noErr then			{ <58> balloon might have changed a bit, so update info }
																				if (wVarCode<>varCode) |
																					(LongInt(wBounds.topLeft)<>LongInt(bounds.topLeft)) |
																					(LongInt(wBounds.botRight)<>LongInt(bounds.botRight)) then
																						begin
																							bounds	:= wBounds;
																							varCode	:= wVarCode;
																							
																							__HMStuffWindowInfo(window,varCode,bounds);	{ <58> restuff the window info }
																						
																							SetEmptyRgn(strucRgn);	{ <58> will cause window to recalc when visible }
																							SetEmptyRgn(contRgn);
																						end;
																		end;
																end;
															
															if result = noErr then							{ <58> either there wasn't a tip proc or caller didn't abort }
																if (method = kHMSaveBitsWindow) then
																	with WindowPeek(window)^ do
																		begin
																			wBounds := strucRgn^^.rgnBBox;
																			hmgSavedBitsHandle	:= __HMSaveBitsInRect(wBounds);	{ save bits behind window b4 putting up window }
																		
																			{ make the ShowWindow() call recalculate the window's regions }
																			SetEmptyRgn(strucRgn);
																			SetEmptyRgn(contRgn);
																		end;
														end;
													
													if result = noErr then
														begin
															SetWindowPic(window,pic);					{ <59> Call toolbox instead of WindowPeeking record }
															SetWRefCon(window,aHelpMsg.hmmHelpType);	{ <59> Call toolbox instead of WindowPeeking record }					
															ShowWindow(window);
															
															{ The code below makes the balloon contents appear immediately
																instead of waiting for the window manager to draw the picture }
																
															BeginUpdate(window);
															
															if pic^ = NIL then		{ <58> the pic was really a resource that was purged }
																LoadResource(Handle(pic));
																
															if pic^ <> NIL then		{ <58> the pic was successfully reloaded (if purged) }
																begin
																	oldState := HGetState(Handle(pic));	{ <58> remember pic handle's state }
																	HNoPurge(Handle(pic));				{ <58> keep it around (if resource) }
																	
																	with pic^^.picFrame do
																		SetRect(bounds,0,0,right-left,bottom-top); { <58> force picture to window coords }
																		
																	DrawPicture(pic,bounds);
																	HSetState(Handle(pic),oldState);
																end;
																
															EndUpdate(window);
															
															hmgWindow := window;
															
														end else	{ there was an abort returned from calling tip proc, so toss the window }
															begin
																if window <> hmgHelpWindow then
																	begin
																		DisposeWindow(window);		{ <58> Dispose the wind if the window was custom }
																		pic := NIL;  { *** }		{ <59> NIL picHandle, DisposeWindow() kills the pic automatically }
																	end;
																	
																if (hmgSavedBitsHandle <> NIL) then
																	begin
																		if DiscardBits(LongInt(hmgSavedBitsHandle))=noErr then;
																		hmgSavedBitsHandle := NIL;		{ <58> NIL this in case someone calls HMRemoveBalloon() }
																	end;
															end;
												end; 
												
										end;
							end;
						
						if result <> noErr then		{ some sort of error or abort occurred, so toss the picture (if any) }
							if pic <> NIL then
								if __HMCanPictureBeKilled(aHelpMsg.hmmHelpType) then	{ <59> KillPicture if type wasn't resource based }
									KillPicture(pic);

						__HMShowBalloon := result;

						with gp^ do
							if (hmgLayer<>NIL) then
								SetCurLayer(oldLayer);

						SetPort(oldPort);
						
						__HMSetLastGlobals;
					end;
					
			end else __HMShowBalloon := hmHelpDisabled;
	END;




PROCEDURE __HMShowTitleBalloon(menuID: INTEGER; flags: LongInt);
	VAR	aHelpMsg		: HMMessageRecord;
		altRect			: Rect;
		pt				: Point;
		width			: INTEGER;
		variant			: INTEGER;
		resID			: INTEGER;
		result			: OsErr;
		gp				: HMGlobalPtr;
	BEGIN
		gp := __HMGetHelpGlobal;
		
		with gp^ do
			if (hmgItemNum = 0) then if (menuID = hmgMenuID) then
				Exit(__HMShowTitleBalloon);

		if __HMGetMenuResID(menuID,resID)<>noErr then
			resID := -1;
			
		variant	:= 0;	{ default in the case of the Apple Menu }
		
		if __HMFillInMenuMessage(menuID,0,resID,flags,variant,aHelpMsg)<>noErr then
			Exit(__HMShowTitleBalloon);
			
		if GetMenuTitleRect(menuID,altRect)=noErr then;
		with altRect do
			begin
				width	:= right-left;
				pt.h	:= left + BSR(width,1);
				pt.v	:= IntegerPtr(MBarHeight)^;
				
				top		:= top - 1;
				bottom	:= top + pt.v;
			end;

		result := __HMShowBalloon(aHelpMsg,pt,@altRect,NIL,0,variant,2);

		if result=noErr then
			with gp^ do
				begin
					hmgLastMenuID		:= menuID;
					hmgLastEnabled		:= flags;
					hmgLastItem			:= 0;
					
					hmgItemNum			:= 0;
					hmgMenuID			:= menuID;
					hmgTitleBalloon 	:= 1;			{ when this is non-zero, the show hide patch won't toss this balloon }
				end;
	END;





FUNCTION  __HMShowMenuBalloon(itemNum,itemMenuID: INTEGER;
							itemFlags,itemReserved: LongInt;
							tip: Point; alternateRect: RectPtr; tipProc: Ptr;
							theProc,variant: INTEGER): OsErr;
	VAR	aHelpMsg		: HMMessageRecord;
		menuBarRect		: Rect;
		menuHeight		: INTEGER;
		overVariant		: INTEGER;
		gp				: HMGlobalPtr;
		pt				: Point;
		result			: OsErr;
		resID			: INTEGER;
	BEGIN
		gp := __HMGetHelpGlobal;
		
		__HMShowMenuBalloon := hmSameAsLastBalloon;	{assume that it's already displayed}

		if __HMGetBalloons 
			
			{$ifc AllowCloseViewChanges}	
				& (gp^.hmgCloseViewCount = 0) {CloseView is active, shift key was down, & balloon was up}
			{$endc} 
							then
			with gp^ do
				begin
					if (hmgLastItem = itemNum) then
						if (hmgLastMenuID = itemMenuID) then
							if (hmgLastEnabled = itemFlags) then EXIT(__HMShowMenuBalloon);
								 	
					if itemNum <= 0 then				{ if 0 then mouse was in a title, if -1 then it was a dash }
						begin
							if __HMRemoveBalloon=noErr then;	{ toss any existing Balloons, if 0 or -1 }
							Exit(__HMShowMenuBalloon);
						end;
	
					if __HMGetMenuResID(itemMenuID,resID)<>noErr then
						resID := -1;
					
					if alternateRect<>NIL then
						hmgItemRect := alternateRect^;
						
					hmgItemNum	:= itemNum;
					hmgMenuID	:= itemMenuID;
					
					overVariant := variant;
					
					{ <59> Ignore itemReserved parameter menu or item is disabled }
					if (itemReserved <> 0) then
						begin
							IF not BTST(itemFlags,0) THEN	{ The whole menu is disabled }
								itemReserved := 0
							ELSE IF (itemNum <= 31) THEN	{ The item has a flag bit }
								IF NOT BTST(itemFlags,itemNum) THEN	{ That item is disabled }
									itemReserved := 0;
						end;
					
					if (itemReserved = 0) or (ODD(itemReserved)) then { we need to fetch the msg out of the 'hmnu' resource }
						begin
							result := __HMFillInMenuMessage(itemMenuID,itemNum,resID,itemFlags,overVariant,aHelpMsg);
							
							if result<>noErr then	{ we didn't find anything in the 'hmnu' or the 'hmnu' was missing }
								begin
									__HMShowMenuBalloon := result;
									exit(__HMShowMenuBalloon);
								end;
								
						end else	{ let's assume that itemReserved is a StringHandle to the message string }
							with aHelpMsg do
								begin
									hmmHelpType := khmmString;
									hmmString	:= StringHandle(itemReserved)^^; { copy the string data }
									result := noErr;
								end;
	
					with aHelpMsg do
						if hmmHelpType = khmmString then
							if length(hmmString)=0 then
								begin
									__HMShowMenuBalloon := paramErr;
									exit(__HMShowMenuBalloon);
								end;
					
					if overVariant<>0 then
						variant := overVariant;
					
					result := __HMSaveBalloonBits(aHelpMsg,tip,alternateRect,tipProc,variant,wasMenu);
						
					if result=noErr then
						begin
							if alternateRect<>NIL then
								hmgSlopRect := alternateRect^
									else SetRect(hmgSlopRect,0,0,0,0);

							hmgLastItem			:= itemNum;	
							hmgLastMenuID		:= itemMenuID;
							hmgLastEnabled		:= itemFlags;
						end;
							
				end else
					result := hmHelpDisabled;
			
		__HMShowMenuBalloon := result;
	END;

	

FUNCTION  __HMSetFont(font: INTEGER): OsErr;
	VAR	fontlong	: LongInt;
	BEGIN
		__HMSetGetFontSizeGlobal(kGetFont,fontlong);
		with __HMGetHelpGlobal^ do
			begin
				hmgFontAndSize.hmgFont := font;
				__HMSetGetFontSizeGlobal(kSetFont,LongInt(hmgFontAndSize));
			end;
		__HMSetFont := noErr;
	END;



FUNCTION  __HMSetFontSize(fontSize: INTEGER): OsErr;
	VAR	fontlong	: LongInt;
	BEGIN
		__HMSetGetFontSizeGlobal(kGetFont,fontlong);
		with __HMGetHelpGlobal^ do
			begin
				hmgFontAndSize.hmgFontSize := fontSize;
				__HMSetGetFontSizeGlobal(kSetFont,LongInt(hmgFontAndSize));
			end;
		__HMSetFontSize := noErr;
	END;



FUNCTION  __HMGetFont(VAR font: INTEGER): OsErr;
	BEGIN
		font := __HMGetHelpGlobal^.hmgFontAndSize.hmgFont;
		__HMGetFont := noErr;
	END;



FUNCTION  __HMGetFontSize(VAR fontSize: INTEGER): OsErr;
	BEGIN
		fontSize := __HMGetHelpGlobal^.hmgFontAndSize.hmgFontSize;
		__HMGetFontSize := noErr;
	END;


{ <58> HMFind now will find BOTH menu & dialog PSN driven indices in open apps }
FUNCTION  __HMFind(queue: HHMHelpQueueRecord; theMenuID: INTEGER;
						VAR index: INTEGER; PSN: ProcessSerialNumber): INTEGER;
	VAR	i		: INTEGER;
		count	: INTEGER;
	BEGIN
		__HMFind := -1;			{ say we didn't find it in the array }
		
		if queue<>NIL then
			begin
				count := queue^^.queueCount;
				for i := 1 to count do
					with queue^^.queueRecords[i] do
						begin
							if (theMenuID = slotID) then { theMenuID will always be equal when searching the dialog queue }
								if (PSN.lowLongOfPSN = slotPID.lowLongOfPSN) then
									if (PSN.highLongOfPSN = slotPID.highLongOfPSN) then
										begin
											__HMFind	:= slotResID;
											index		:= i;
											leave;
										end;
						end;
			end;
	END;


FUNCTION  __HMSetDialogResID(theResID: INTEGER): OsErr; 	{ <58> rewritten to work with the current PSN instead of just one index }
	VAR	i		: INTEGER;
		result	: OsErr;
		offset	: LongInt;
		p		: LongInt;
		queue	: HHMHelpQueueRecord;
		PSN		: ProcessSerialNumber;
	BEGIN
		result := GetCurrentProcess(PSN);	{ <58> make this work for the current process who is assigning the res ID }
		
		if result=noErr then				{ <58> only use Process IDs that are good }
			begin	
				queue	:= __HMGetHelpGlobal^.hmgDialogQueue;							{ <58> use the dialog queue } 
				
				if __HMFind(queue,0 {allow only 1 entry in dialogs},i,PSN)<>-1 then		{ <58> we found a setup index, just replace the record's data }
					begin
						if theResID = -1 then											{ <58> remove this record from our queue }
							begin
								{believe it or not, the calculation below IS 32 bit clean}
								
								p		:= ord4(@queue^^.queueRecords[i]);				{ <58> }
								offset	:= p - ord4(queue^);							{ <58> }
								
								{ <58> this does a SetHandleSize smaller so it should always (gulp!) succeed }
								if Munger(Handle(queue),offset,NIL,SIZEOF(HMHelpSlot),Ptr(-1),0)=0 then;	{ <58> }
								
								with queue^^ do
									queueCount := queueCount - 1;						{ <58> }
									
								{ *** <58> do we need to hide any currently displayed balloon here? }
									
							end else
								queue^^.queueRecords[i].slotResID := theResID;			{ <58> }
							
					end else { <58> the ID returned from HMFind was -1 which means add a new element to the queue }
						begin
							SetHandleSize(Handle(queue), GetHandleSize(Handle(queue)) + SIZEOF(HMHelpSlot));	{ <58> }
							
							result := memError;																	{ <58> }
							
							if result=noErr then																{ <58> }
								begin
									with queue^^ do																{ <58> }
										begin
											queueCount := queueCount + 1;
											with queueRecords[queueCount] do									{ <58> }
												begin
													{queueRecords[queueCount].}slotPID		:= PSN;				{ <58> }
													{queueRecords[queueCount].}slotResID	:= theResID;		{ <58> }
													{queueRecords[queueCount].}slotID		:= 0;				{ <58> }
												end;
										end;
								end;
						end;
			end;
			
		__HMSetDialogResID := result;	{ <58> }
	END;
	
	
FUNCTION  __HMGetDialogResID(VAR theResID: INTEGER): OsErr;
	VAR	i				: INTEGER;
		PSN				: ProcessSerialNumber;
	BEGIN
		__HMGetDialogResID := resNotFound;						{ <58> assume that we didn't find any dialog res id assignment }
		
		with __HMGetHelpGlobal^ do
			begin
				if hmgDialogQueue^^.queueCount > 0 then			{ <58> quick check to see if anybody has ever overridden any dialog res ID }
					if GetCurrentProcess(PSN)=noErr then		{ <58> only use Process IDs that are good }
						begin
							theResID := __HMFind(hmgDialogQueue,0,i,PSN);	{ <58> returns assigned res id for the PSN's dialog override }
							if theResID<>-1 then
								__HMGetDialogResID := noErr;	{ <58> return noErr if we found an assignment }
						end;
			end;
	END;

	
 	
FUNCTION  __HMSetMenuResID(theMenuID,theResID: INTEGER): OsErr;	{ <58> now can return ProcessMgr error by calling GetCurrentProcess }
	VAR	i		: INTEGER;
		result	: OsErr;
		offset	: LongInt;
		p		: LongInt;
		queue	: HHMHelpQueueRecord;
		PSN		: ProcessSerialNumber;
	BEGIN
		result := GetCurrentProcess(PSN);
		if result=noErr then								{ only use Process IDs that are good }
			begin
				__HMSetMenuResID := noErr;
	
				queue	:= __HMGetHelpGlobal^.hmgMenuQueue;
				
				if __HMFind(queue,theMenuID,i,PSN)<>-1 then	{ we found this theMenuID in the list already, returns found index in i for array }
					begin
						if theResID = -1 then				{ remove this record }
							begin
								{believe it or not, the calculation below IS 32 bit clean}
								p		:= ord4(@queue^^.queueRecords[i]); {ord(Handle(queue^^.queueRecords[i])^);}
								offset	:= p - ord4(queue^);
								
								{ the following routine will remove the data at the given offset:
									offset			:= p - ord(queue^);
									count to remove := SIZEOF(record);  (size of our record HMHelpSlot)
						
									the trick to munger in this case below is that the last parameter must be zero
									as we're not replacing anything & munger calls SetHandleSize for us! 
									and since this is a delete (SetHandleSize smaller), it will always (gulp!) succeed… }
			
								if Munger(Handle(queue),offset,NIL,SIZEOF(HMHelpSlot),Ptr(-1),0)=0 then;
								
								with queue^^ do
									queueCount := queueCount - 1;
									
							end else
								queue^^.queueRecords[i].slotResID := theResID;
							
					end else { the ID was -1 which means add a new record }
						begin
							SetHandleSize(Handle(queue), GetHandleSize(Handle(queue)) + SIZEOF(HMHelpSlot));
							
							result := memError;
							
							if result=noErr then
								begin
									with queue^^ do
										begin
											queueCount := queueCount + 1;
											with queueRecords[queueCount] do
												begin
													{queueRecords[queueCount].}slotPID		:= PSN;
													{queueRecords[queueCount].}slotResID	:= theResID;
													{queueRecords[queueCount].}slotID		:= theMenuID;
												end;
										end;
								end;
						end;
			end;
		
		__HMSetMenuResID := result;
	END;
	
	
	
FUNCTION  __HMGetMenuResID(theMenuID: INTEGER; VAR theResID: INTEGER): OsErr;
	VAR	i		: INTEGER;
		PSN		: ProcessSerialNumber;
	BEGIN
		__HMGetMenuResID := resNotFound;	{ assume that we didn't find any resource }
		
		{ <58> note: don't bother with quick check that HMGetDialogResID does by checking the queueCount field
			in the menu queue.  Finder always overrides the hmnu res id's when it is launched, so, there
			is always something in the queue }
			
		if GetCurrentProcess(PSN)=noErr then { only use Process IDs that are good }
			begin
				theResID := __HMFind(__HMGetHelpGlobal^.hmgMenuQueue,theMenuID,i,PSN); { <58> pass the queue now }
				if theResID<>-1 then
					__HMGetMenuResID := noErr;
			end;
	END;
	
	
	
FUNCTION  __HMModalDialogMenuSetup(ModalDialogComingUp: BOOLEAN): OsErr;
	VAR	mh				: MenuHandle;
		i				: INTEGER;
	BEGIN
		__HMModalDialogMenuSetup := noErr;
		mh := GetMHandle(kHMHelpMenuID);
		if (mh<>NIL) then
			begin
				for i := 1 to CountMItems(mh) do
					begin
						if (i<>kHMShowBalloonsItem) then
							if (ModalDialogComingUp) then
									DisableItem(mh,i) else
										EnableItem(mh,i);
					end;
			end else
				__HMModalDialogMenuSetup := hmHelpManagerNotInited;
	END;

FUNCTION __HMGetScreenGDevice(pt: Point): GDHandle;
	VAR	gh				: GDHandle;
	BEGIN
		gh := GetDeviceList;
		while gh<>NIL do
			begin
				if TestDeviceAttribute(gh,screenDevice) then
					if TestDeviceAttribute(gh,screenActive) then
						if PtInRect(pt,gh^^.gdRect) then
							leave;
				gh := GetNextDevice(gh);	{ *** faster if we use: gh^^.gdNextGD }
			end;
		__HMGetScreenGDevice := gh;
	END;
	


PROCEDURE __HMGetVariantRect(rbs: RBSRecord; VAR fullRect: Rect);	{ Returns the rect that can encompass the balloon }
	VAR		theword			: IntegerPtr;
	BEGIN
		with fullRect do
			begin
				theword := @right;
				if BTST(rbs.whole, kShiftBit) then
					theword := @bottom;
			end;
		theword^ := theword^ + kHMPointerSize;
	END;
	

	


PROCEDURE __HMSelectProperVariant(	balloonBounds: Rect;	{ bounds of balloon w/o tip }
									altRect: Rect;			{ alternate rectangle }
									screenRect: Rect;		{ bounds of monitor we're about to display on }
									VAR rbs: RBSRecord;		{ balloon's variant word, right, bottom, shift values }
									VAR tip: Point			{ balloon's initial tip position }
									);		

	CONST	
		horizontal	= 0;
		vertical	= 1;

	VAR	dL			: INTEGER;
		dR			: INTEGER;
		dT			: INTEGER;
		dB			: INTEGER;
		retries		: INTEGER;
		
		width		: INTEGER;
		height		: INTEGER;
		border		: INTEGER;
		
		deltaH		: INTEGER;
		deltaV		: INTEGER;
		
		whichAxis	: INTEGER;
		axisCount	: INTEGER;
		
		hangRight	: INTEGER;	{ 1 if balloon hangs right }
		hangDown	: INTEGER;	{ 1 if balloon hangs down }
		hangShift	: INTEGER;	{ 1 if balloon has a vertical tip }
		
		bestRight	: INTEGER;
		bestDown	: INTEGER;
		bestShift	: INTEGER;
		bestTip		: Point;
		
		bestArea	: LongInt;
		
		area		: LongInt;
		areaH		: LongInt;
		areaV		: LongInt;
		
		destRect	: Rect;
		areaRect	: Rect;
							
		{ •	First: See if tip's position delegates having a vertical (shift = 1) tip.
					
		  •	Second: Try the horizontal position of the balloon to see if it fits with the
		  			original tip and variant. The routine below is written to calculate balloons
					in a specific order.  Instead of just computing a balloon that fits, we walk
					thru choices of variants to produce what appears better rather than what is
					just mathematically appropriate.
					
					Every second time that a variant is changed, the balloon
					shift (whether or not a vertical tip is used) is toggled.  This ensures that
					every variant is tried in order to get the balloon to fit without clipping.
					
		  • Third:  Try the vertical position.
		  
		  • Fourth: If all variants have been tried then use the best balloon that we had calculated
		  			along the way.  The best balloon is the one with the most area.
						
		}
			
	BEGIN
		border := IntegerPtr(MBarHeight)^ + 2;		{ compute the menubar's height plus room for the line & 1 pix }
		
		hangRight	:= rbs.rightValue;				{ get right hang direction in local (if hangRight = 1 then the balloon hangs right) }
		hangDown	:= rbs.bottomValue;				{ get downward hang direction in local (if hangDown = 1 then the balloon hangs down) }
		hangShift	:= rbs.shiftValue;				{ get shift value in local (if hangShift = 1 then the balloon has a vertical tip) }

		if hangShift = 0 then
			with screenRect do 
				if (tip.v < top + border) or
					(tip.v > bottom - border) then	{ the balloon tip is in menubar area or near very bottom of screen bounds }
					begin
						hangShift := 1;				{ force a vertical tip as we're near the top or bottom of screen }
					end;
							
		whichAxis	:= horizontal;					{ start with the horizontal axis first }
		axisCount	:= 0;							{ reset the counter for keeping track of how many times we've tried either horizontal or vertically }				
		bestArea	:= -1;							{ initialize the bestArea local that keeps track of the balloon with the greatest area }
		
		for retries := 1 to 8 do					{ keep computing until we tried 8 times }
			begin
				destRect := balloonBounds;			{ reset destination rect to be the balloon bounds EACH time thru the loop }
				with destRect do					{ recalculate the dimensions of the balloon }
					begin
						if hangShift = 0 then
							right := right + kHMPointerSize
						else bottom := bottom + kHMPointerSize;				{ include space for tip dimensions }
						
						width	:= right - left + kHMHorizFrameMargin;		{ include balloon horizontal margin }
						height	:= bottom - top + kHMVertFrameMargin;		{ include balloon vertical margin }
					end;

				with screenRect do
					begin
						dR := (right - tip.h);		{ compute the space between the tip and the right edge of the screen bounds }
						dL := (tip.h - left);		{ compute the space between the tip and the left edge of the screen bounds }
						
						dB := (bottom - tip.v);		{ compute the space between the tip and the bottom edge of the screen bounds }
						dT := (tip.v - (top + border));		{ compute the space between the tip and the top edge of the screen bounds (minus border) }
					end;

				deltaH := dR;						{ Assume balloon hangs right so use deltaH for right }
				if hangRight = 0 then				{ T => Nope, it hangs left, so use left deltaH }
					deltaH := dL;
				areaH := width;						{ remember this balloon's width so that if no balloons fit, we }
				if width > deltaH then				{	can use the 'best' balloon as a last resort… }
					areaH := deltaH;
					
				deltaV := dB;						{ Assume balloon hangs down so use deltaV for bottom }
				if hangDown = 0 then				{ T => Nope, it hangs up, so use top deltaV }
					deltaV := dT;
				areaV := height;					{ remember this balloon's height so that if no balloons fit, we }
				if height > deltaV then				{	can use the 'best' balloon as a last resort… }
					areaV := deltaV;
					
				area := areaV * areaH;				{ compute this balloons area in case all balloons fail to fit }
				if area > bestArea then				{ this balloon's area was larger than the last, so copy the balloon's params }
					begin
						bestRight	:= hangRight;
						bestDown	:= hangDown;
						bestShift	:= hangShift;
						bestTip		:= tip;
						bestArea	:= area;		{	and save the bestArea for the next go 'round }
					end;

			
				case whichAxis of
					horizontal:
						begin
							if (width > deltaH) then				{ the balloon was too wide to fit, try tweeking the tip &/or variant }
								begin
									axisCount := axisCount + 1;		{ increment the counter for walking thru the balloon variants }
									
									if axisCount <> 4 then			{ don't move the tip the 4th time we've checked the horiz size }
										begin
											tip.h := altRect.left + (altRect.right - tip.h); { move tip to the opposite side of the altRect }
										end else
											hangShift := 1 - hangShift;	{ change from a vertical tip to a horizontal one or horiz to vertical }
										
									hangRight := 1 - hangRight;		{ change the balloon hanging right to hanging left or left to right }
								end else
									begin { we successfully fit horizontally }
										whichAxis := vertical; 		{ so now try the vertical }
										axisCount := 0;				{ reset the counter	}
										cycle;						{ to the next for retries }
									end;
						end;
						
					vertical:
						begin
							if (height > deltaV) then				{ the balloon was too tall to fit, try tweeking the tip &/or variant }
								begin
									axisCount := axisCount + 1;		{ increment the counter for walking thru the balloon variants }
									
									if axisCount <> 4 then			{ don't move the tip the 4th time we've checked the vertical size }
										begin
											tip.v := altRect.top + (altRect.bottom - tip.v); { move tip to the opposite side of the altRect }
										end else
											hangShift := 1 - hangShift;	{ change from a vertical tip to a horizontal one or horiz to vertical }
										
									hangDown := 1 - hangDown;		{ change the balloon hanging down to hanging up or down to up }	
								end else
									begin
										leave;						{ both the horizontal and vertical fit so exit now }
									end;
						end;
				end;
				
				if retries >= 8 then { balloon didn't fit, use the which one was best }
					begin
						hangRight	:= bestRight;
						hangDown	:= bestDown;
						hangShift	:= bestShift;
						tip			:= bestTip;
						
						leave;
					end
				else if (axisCount mod 2) = 0 then	{ every second try, change the shift factor }
					begin
						hangShift := 1 - hangShift;	{ change from a vertical tip to a horizontal one or horiz to vertical }
					end;
			end;
			
		rbs.rightValue	:= hangRight;				{ reset the rbs value to use the just tweaked (possibly) hanging values }
		rbs.bottomValue	:= hangDown;
		rbs.shiftValue	:= hangShift;
	END;


FUNCTION  __HMPinTipAndAltRect(screenBounds: Rect; VAR tip: Point; VAR altRect: Rect): BOOLEAN; { Returns false if no intersection }
	VAR	deltaLeft			: LongInt;
		deltaTop			: LongInt;
		width,height		: LongInt;
		newAltRect			: Rect;
		insideScreenBounds	: BOOLEAN;
	BEGIN
		{ see if the altRect is really in the screen bounds (and clip it to the screen bounds at the same time) }
		insideScreenBounds := SectRect(screenBounds,altRect,newAltRect);
		
		if insideScreenBounds then	{ at least part of the altRect is visible }
			begin
				if not EqualRect(altRect,newAltRect) then	{ if part of the altRect is clipped, then use the ratio for the tip inside the altRect }
					begin
						{ use the original altRect to determine the tip deltas between the right & left, top & bottom with respect to the tip }
						with altRect do
							begin
								deltaLeft	:= (tip.h - left);	{ this is the space between the right edge of the altRect and the horizontal part of the tip }
								deltaTop	:= (tip.v - top);
								
								width		:= right - left;	{ get old altRect width and height }
								height 		:= bottom - top;
								
							end;
							
						if width = 0 then width := 1;			{ make sure were not zero }
						if height = 0 then height := 1;
						
						with newAltRect do
							begin
								tip.h := left + (ORD4((right - left) * deltaLeft) div width);	{ compute ratio of tip within rect }
								tip.v := top + (ORD4((bottom - top) * deltaTop) div height);
							end;
							
						altRect := newAltRect;
						LongInt(tip) := PinRect(altRect,tip);	{ always repin the tip to the new altRect }
					end;
			end;
		
		__HMPinTipAndAltRect := insideScreenBounds;
	END;


FUNCTION  __HMBestWorld(worldBounds: Rect; tip: Point; areaRect: Rect; balloonType: INTEGER;
							VAR balloonBounds: Rect; VAR variant: INTEGER): OsErr;
	VAR	rbs				: RBSRecord;
		i,j				: INTEGER;
		deltaL			: INTEGER;
		deltaR			: INTEGER;
		deltaT			: INTEGER;
		deltaB			: INTEGER;
		result			: INTEGER;
		boundsWidth		: INTEGER;
		boundsHeight	: INTEGER;
		tweek			: INTEGER;
		moveValue		: Point;
		destRect		: Rect;
		offset			: Point;
		rbsOK			: BOOLEAN;
		oldPort			: GrafPtr;
		
	BEGIN
		rbsOK	:= false;
		
		__HMTwitchVarCode(kVariantToRBS,variant,rbs.whole);
					
		{ Compute the 'best' rbs that will fit the balloon in the worldBounds }
		__HMSelectProperVariant(balloonBounds, areaRect, worldBounds, rbs, tip);
		
		destRect := balloonBounds;			{ start with the balloon bounds }
		__HMGetVariantRect(rbs,destRect);	{ tweak the bounds to take in account the tip placement }			
				
		with balloonBounds do
			begin
				boundsWidth		:= right - left;
				boundsHeight	:= bottom - top;

				left	:= tip.h - ((1-rbs.rightValue) * (destRect.right - destRect.left));
				top		:= tip.v - ((1-rbs.bottomValue) * (destRect.bottom - destRect.top));
				right	:= left + boundsWidth;
				bottom	:= top + boundsHeight;
			end;	
		
		LongInt(offset) :=  0;
		
		if BTST(rbs.whole,kShiftBit) then
			offset.v := kHMPointerSize * rbs.bottomValue
		else 
			offset.h := kHMPointerSize * rbs.rightValue;

		offset.h := offset.h + (rbs.rightValue * kHMFrameThickness) - ((1-rbs.rightValue) * kHMPointerSize);
		offset.v := offset.v + (rbs.bottomValue * kHMFrameThickness) - ((1-rbs.bottomValue) * kHMPointerSize);

		OffsetRect(balloonBounds,offset.h,offset.v);
		
		__HMTwitchVarCode(kRBSToVariant,INTEGER(rbs),variant);

		__HMBestWorld := noErr;
	END;
	


FUNCTION  __HMCalculateBalloon(	aHelpMsg: HMMessageRecord;
								tip: Point;
								alternateRect: RectPtr;
								VAR varCode: INTEGER;
								VAR bounds: Rect; VAR pic: PicHandle;
								balloonType: INTEGER;
								abortable: BOOLEAN): OsErr;
								
	VAR	gp				: HMGlobalPtr;
		screenBounds	: Rect;
		areaRect		: Rect;
		visibleRect		: Rect;
		gh				: GDHandle;
		oldDevice		: GDHandle;
		oldPort			: GrafPtr;
		tPort			: GrafPtr;
		picPort			: GrafPort;
		err				: INTEGER;
		pt				: Point;
		slopTip			: Point;
		aTE				: TEHandle;
		tix				: LongInt;
		sloppy			: BOOLEAN;
		hasColorQD		: BOOLEAN;
		oldZone			: THz;				{ <SM2> }
			
	BEGIN
		__HMCalculateBalloon	:= hmBalloonAborted;		{ make default case aborted }
		gp						:= __HMGetHelpGlobal;
		pic						:= NIL;						{ <59> make sure pic is NIL in case we exit early }
		
		if abortable then
			begin
				tix := Tickcount;
				with gp^ do
					begin
						if (hmgDelay - (tix - hmgTix)) > 0 then
							exit(__HMCalculateBalloon);		{ exit if not enough time has elapsed }
						hmgTix	:= tix;
					end;
			end;
		
		oldDevice	:= NIL;
		sloppy		:= false;
		slopTip		:= __HMGetGlobalMouse;
		hasColorQD	:= gp^.hmgHasColorQD;
		
		if hasColorQD then
			begin
				{ *** do we want to use the tip's GDevice or the mouse point's ? }
				gh := __HMGetScreenGDevice(slopTip);		{ use the mouse's GDevice }
				if gh<>NIL then
					begin
						screenBounds	:= gh^^.gdRect;
						oldDevice		:= GetGDevice;
						SetGDevice(gh);
					end else
						exit(__HMCalculateBalloon);			{ how could the mouse be offscreen? }
			end else
				begin
					screenBounds		:= GetGrayRgn^^.rgnBBox;
					screenBounds.top	:= 0;
				end;
				
		if alternateRect<>NIL then
			begin
				areaRect := alternateRect^;
				if not __HMPinTipAndAltRect(screenBounds,tip,areaRect) then
					begin
						if oldDevice<>NIL then				{ =NIL for non-color ports }
							SetGDevice(oldDevice);
		
						exit(__HMCalculateBalloon);
					end;
			end else
				begin
					LongInt(tip) := PinRect(screenBounds,tip);
					with areaRect do
						begin
							topLeft		:= tip;
							bottom		:= tip.v + 1;
							right		:= tip.h + 1;
						end;
				end;
		
			
		{$ifc HelpMgrTesting}
		Assert(EmptyRect(areaRect),'HMCalculateBalloon: Empty area Rect!');
		{$endc}
			
		GetPort(oldPort);

		{if hasColorQD then
			OpenCPort(@picPort)
				else }OpenPort(@picPort);

		SetPort(@picPort);

		with gp^ do
			begin
				TextFont(hmgFontAndSize.hmgFont);
				TextSize(hmgFontAndSize.hmgFontSize);
				
				hmgKeepTEHandle := true;	{we don't want the __HMBalloonRect() call to toss the TEHandle (if any)}
				hmgTEHandle		:= NIL;		{in case __HMBalloonRect fails}
			end;

		SetRect(bounds,0,0,0,0);
		err := __HMBalloonRect(aHelpMsg,bounds);
		
		__HMCalculateBalloon := err;
		
		if err=noErr then
			begin
				err := __HMBestWorld(screenBounds,tip,areaRect,balloonType,bounds,varCode);
				
				{$ifc HelpMgrTesting}
				if err<>noErr then
					DebugStr('Unable to fit balloon on screen…');
				{$endc}
			end;
			
		if err<>noErr then	
			begin
				{if hasColorQD then
					CloseCPort(@picPort)
						else} ClosePort(@picPort);
		
				SetPort(oldPort);
		
				if oldDevice<>NIL then	{ always NIL for non-color ports }
					SetGDevice(oldDevice);
				exit(__HMCalculateBalloon);
			end;
						
		with bounds do
			begin
				SetRect(visibleRect,0,0,right-left,bottom-top);	
				
				right	:= right + kHMHorizContentMargin;
				bottom	:= bottom + kHMVertContentMargin;
			end;
	
		aTE := gp^.hmgTEHandle;

		if not __HMSlopMouse(slopTip) or not abortable then
			begin
				with aHelpMsg do
					case hmmHelpType of
						khmmPict,khmmPictHandle:
							if hmmHelpType = khmmPict then
								pic := PicHandle(GetResource('PICT',hmmPict))
									else pic := hmmPictHandle;
						
						khmmTEHandle: 
							begin
								aTE := hmmTEHandle;
								if aTE<>NIL then
									begin
										with aTE^^ do
											begin
												tPort 	:= inPort;
												inPort	:= @picPort;
											end;
											
										oldZone := GetZone;		{ get the current heap zone }	{ <SM2> }										
										SetZone(SystemZone);	{ point at the system heap } 	{ <SM2> }
										          												{ <SM2> }
										pic := OpenPicture(visibleRect);
										TEUpdate(visibleRect,aTE);
										ClosePicture;

										SetZone(oldZone);										{ <SM2> }
																								{ <SM2> }
										with aTE^^ do
											inPort := tPort;
									end;
							end;
													
						otherwise
							if aTE<>NIL then
								begin
									oldZone := GetZone;		{ get the current heap zone }		{ <SM2> }										
									SetZone(SystemZone);	{ point at the system heap } 		{ <SM2> }
																								{ <SM2> }
									pic := OpenPicture(visibleRect);
									TEUpdate(visibleRect,aTE);
									ClosePicture;
																								{ <SM2> }
									SetZone(oldZone);											{ <SM2> }
																								{ <SM2> }
								end;
					end;

			end else Sloppy := true;

		if aHelpMsg.hmmHelpType<>khmmTEHandle then
			if aTE<>NIL then
				TEDispose(aTE);
		
		with gp^ do
			begin
				hmgTEHandle		:= NIL;
				hmgKeepTEHandle	:= false;
			end;

		{if hasColorQD then
			CloseCPort(@picPort)
				else} ClosePort(@picPort);

		SetPort(oldPort);

		if oldDevice<>NIL then	{=NIL for non-color ports}
			SetGDevice(oldDevice);

		if Sloppy | (pic = NIL) then
			begin
				gp^.hmgItemNum := -1;
				
				__HMCalculateBalloon := memFullErr;
				
				if pic<>NIL then
					begin
						if __HMCanPictureBeKilled(aHelpMsg.hmmHelpType) then	{ <59> Kill picture if it wasn't resource based }
							KillPicture(pic);
						pic := NIL;
					end;
					
				if Sloppy then
					__HMCalculateBalloon := hmBalloonAborted;
									
				exit(__HMCalculateBalloon);
			end;
		
		{ <58> the third call to HMSlopMouse below means that HMCalculate can return w/ an error
			but still return (in VAR pic) a valid picture handle, the calling code must call
			KillPicture() to dispose of it in this case!! Memory leak bug in 7.0b1 }
			
		if (abortable and __HMSlopMouse(slopTip)) or EmptyRect(bounds) then { <58> add abortable }
			__HMCalculateBalloon := hmBalloonAborted;
	END;



FUNCTION  __HMSetupBalloonRgns(	varCode: INTEGER;
								VAR structRgn: RgnHandle;
								VAR contentRgn: RgnHandle;
								VAR bounds: Rect): OsErr;

{	 Balloon Tip Variants

	The diagram below relates the varcode parameter value to the bubble
	 tip position and orientation.  The table below shows the Unit
	 Vectors which are used to draw the tip.

			1               2

			*               *
			|\             /|
			| \           / |		NOTES
	   0 *---------------------* 3
		  \ |               | /		a.  The 8 asterisks indicate the 8
		   \|               |/			bubble tips.
		    |    Bubble     |
		    |   Structure   |       b.  The digits are the varcode values for each
		    |   Rectangle   |			bubble tip.
		   /|               |\
		  / |               | \
	   7 *---------------------* 4	c.  The hCorner,vCorner columns below are a unit vector from
			| /           \ |			the corner to the bubble tip.
			|/             \|
			*               *		d.  The hBase,vBase columns are a unit vector from the
										bubble tip to the base of the diagonal.
			6               5

										Pointing
		varcode		Hotspot Location	Direction  hCorner	vCorner		hBase	vBase
	  -------		----------------	---------  ------	-------		-------	-------
			0		Top, Left			Left		 -1		   0		  1		  1
			1		Top, Left			Up			  0		  -1	 	  1		  1
			2		Top, Right			Up			  0		  -1		 -1		  1
			3		Top, Right			Right		  1		   0		 -1		  1
			4		Bottom, Right		Right		  1		   0		 -1		 -1
			5		Bottom, Right		Down		  0		   1  		 -1		 -1
			6		Bottom, Left		Down		  0		   1		  1		 -1
			7		Bottom, Left		Left		 -1		   0	 	  1		 -1
}

	VAR	frame			: Rect;
		originalFrame	: Rect;
		tempRgn			: RgnHandle;
		bias			: biasRecord;
	BEGIN
		RectRgn(contentRgn, bounds);

		frame := bounds;
		InsetRect(frame,-kHMFrameThickness,-kHMFrameThickness);
		
		originalFrame := frame;
		
		varCode := BAnd($0007,varCode);	{ strip off hi bits of varCode, in case the varCode is out of range }

		with frame.topleft do
			begin
				if varCode >= 4 then
					v := frame.bottom;

				if (varCode >= 2) then if (varCode <= 5) then
					h := frame.right;
				
				__HMVarCodeToBiasRecord(varCode,bias);
				
				with bias do
					begin
						h := h + (kHMPointerSize * hCornerVector);
						v := v + (kHMPointerSize * vCornerVector);
						
						OpenRgn;
							FrameRoundRect(originalFrame, kHMCornerDiameter, kHMCornerDiameter);
	
							MoveTo(h,v);	
							Line(hBaseVector * kHMPointerSize,VBaseVector * kHMPointerSize);
							Line(DicksOffsetPixels * hShift, DicksOffsetPixels * vShift);
							LineTo(h,v);
						CloseRgn(structRgn);
					end;
			end;

		tempRgn := GetGrayRgn;
		SectRgn(tempRgn, structRgn, structRgn);
		SectRgn(tempRgn, contentRgn, contentRgn);
		
		__HMSetupBalloonRgns := noErr;
	END;
	
	

FUNCTION  __HMDrawBalloonFrame(rgn: RgnHandle): OsErr;
	VAR	oldClip		: RgnHandle;
	BEGIN
		oldClip := NewRgn;

		{$ifc HelpMgrTesting}
		Assert(oldClip=NIL,'HMDrawBalloonFrame[1]: NIL Region for NewRgn call');
		{$endc}

		GetClip(oldClip);

		EraseRgn(rgn);
		FrameRgn(rgn);
		SetClip(rgn);
		OffsetRgn(rgn, -1, -1);
		FrameRgn(rgn);
		OffsetRgn(rgn, 1, 1);

		SetClip(oldClip);
		__HMDisposeRgn(oldClip);
		
		__HMDrawBalloonFrame := noErr;
	END;


FUNCTION  __HMSaveBalloonBits(aHelpMsg: HMMessageRecord;
							tip: Point;
							alternateRect: RectPtr;
							tipProc: Ptr;
							variant: INTEGER;
							balloonType: INTEGER): OsErr;
	VAR	bounds			: Rect;
		bitsbounds		: Rect;
		oldBounds		: Rect;
		pic				: PicHandle;
		result			: OsErr;
		varCode			: INTEGER;
		wVarCode		: INTEGER;
		wBounds			: Rect;
		oldPort			: GrafPtr;
		wPort			: GrafPtr;
		oldClipRgn		: RgnHandle;
		wideOpenRgn		: RgnHandle;
		aStructRgn		: RgnHandle;
		aContentRgn		: RgnHandle;
		oldState		: SignedByte;
	BEGIN
		GetPort(oldPort);
		GetWMgrPort(wPort);
		SetPort(wPort);
		
		oldClipRgn	:= NewRgn;
		
		if oldClipRgn<>NIL then
			begin
				GetClip(oldClipRgn);
		
				wideOpenRgn	:= NewRgn;
				
				if wideOpenRgn<>NIL then
					begin
						SetRectRgn(wideOpenRgn,-32768,-32768,32767,32767);
						SetClip(wideOpenRgn);
						__HMDisposeRgn(wideOpenRgn);
					end;
					
				varCode := variant;
				
				result := __HMCalculateBalloon(aHelpMsg,tip,alternateRect,varCode,bounds,pic,balloonType,true{can be aborted});
				if result = noErr then	{ we can display a balloon }
					begin
						if __HMRemoveBalloon=noErr then;	{ toss any existing Balloons, if we get this far }
						
						aStructRgn	:= NewRgn;
						aContentRgn	:= NewRgn;
						
						result := __HMSetupBalloonRgns(varCode,aStructRgn,aContentRgn,bounds);
							
						if tipProc<>NIL then
							begin
								wBounds		:= bounds;
								wVarCode	:= varCode;

								result := __HMCallTipProc(tipProc,tip,aStructRgn,wBounds,wVarCode);				{ <58> give the caller a chance to abort }
								
								if result = noErr then															{ <58> balloon might have changed a bit, so update info }
									if (wVarCode<>varCode) |
										(LongInt(wBounds.topLeft)<>LongInt(bounds.topLeft)) |
										(LongInt(wBounds.botRight)<>LongInt(bounds.botRight)) then
											begin
												bounds	:= wBounds;
												varCode	:= wVarCode;
												
												if __HMSetupBalloonRgns(varCode,aStructRgn,aContentRgn,bounds)=noErr then;	{ <58> recalculate the balloon's regions }
											end;

							end else
								result := noErr;
								
						if result = noErr then
							with __HMGetHelpGlobal^ do
								begin
									bitsbounds := aStructRgn^^.rgnBBox;
									hmgSavedBitsHandle	:= __HMSaveBitsInRect(bitsbounds);
					
									if hmgSavedBitsHandle<>NIL then
										begin
											if __HMDrawBalloonFrame(aStructRgn)=noErr then;
							
											if pic<>NIL then
												begin
													SetClip(aContentRgn);
													
													if pic^ = NIL then							{ <58> the pic was really a resource that was purged }
														LoadResource(Handle(pic));
														
													if pic^ <> NIL then							{ <58> the pic was successfully reloaded (if purged) }
														begin
															oldState := HGetState(Handle(pic));	{ <58> remember pic handle's state }
															HNoPurge(Handle(pic));				{ <58> keep it around (if resource) }
															
															oldBounds := pic^^.picFrame; 		{ <58> keep old picture bounds as we're offsetting the frame }
															
															with pic^^.picFrame do
																begin
																	bounds.right := bounds.left + (right-left);
																	bounds.bottom := bounds.top + (bottom-top);
																end;
																
															DrawPicture(pic,bounds);
															pic^^.picFrame := oldBounds;		 { <58> restore old picture bounds }
															
															HSetState(Handle(pic),oldState);	 { <58> restore picture handle state }
														end;
												end;
										end else
											result := memFullErr;
								end;
							
						__HMDisposeRgn(aStructRgn);
						__HMDisposeRgn(aContentRgn);
					end;
					
				if (pic<>NIL) then
					if __HMCanPictureBeKilled(aHelpMsg.hmmHelpType) then	{ <59> Kill picture if it wasn't resource based }
						KillPicture(pic);	{ bug before <58> [used to be before DisposeRgn(aStructRgn) above] }
					
				SetClip(oldClipRgn);
				DisposeRgn(oldClipRgn);			{ we don't need to NIL check handle here }
				
			end else
				result := memFullErr;
			
		SetPort(oldPort);
		
		__HMSaveBalloonBits := result;
	END;


FUNCTION __HMGetHelpItemResID(d: DialogPtr; whichOne: INTEGER; VAR kind: INTEGER; VAR offset: INTEGER): INTEGER;
	VAR	numItems	: INTEGER;
		dataSize	: INTEGER;
		i			: INTEGER;
		hItems		: hItemList;
		pItem		: pDITLItem;
		USB			: SByteArray;
		count		: INTEGER;
	BEGIN
		__HMGetHelpItemResID := -1;
		
		count		:= 1;
		hItems		:= hItemList(DialogPeek(d)^.items);
		numItems	:= hItems^^.dlgMaxIndex + 1;
		pItem		:= @hItems^^.DITLItems;
		
		for i := 1 to numItems do	{ code below is handle safe! }
			begin
				USB.Int			:= 0;
				USB.SBArray[1]	:= pItem^.itmData[0];
	
				with pItem^ do
					if BAnd(itmType,$7F)=kHelpItem then
						if count = whichOne then
							begin
								kind	:= IntegerPtr(@itmdata[kHelpItemType])^;
								
								{ always check the ranges we know in case in the future we use HelpItem for something else }
								if (kind = kScanhdlg) or (kind = kScanhrct) or (kind = kScanAppendhdlg) then
									begin
										__HMGetHelpItemResID := IntegerPtr(@itmdata[kHelpItemData])^;
										offset	:= 0;
										
										if kind = kScanAppendhdlg then
											offset := IntegerPtr(@itmdata[kHelpItemOffset])^;
									end;
								leave;
							end else
								count := count + 1;

				dataSize	:= BAnd(USB.Int + 1, $FFFE);
				pItem		:= pDITLItem(ptr(ord4(@pItem^) + datasize + sizeof(DITLItem)));
			end;
	END;

FUNCTION __HMCountDITLHelpItems(d: DialogPtr): INTEGER;
	VAR	numItems	: INTEGER;
		i			: INTEGER;
		itemType	: INTEGER;
		itemHandle	: Handle;
		itemRect	: Rect;
		count		: INTEGER;
	BEGIN
		count 		:= 0;
		numItems	:= hItemList(DialogPeek(d)^.items)^^.dlgMaxIndex + 1;
		for i := 1 to numItems do
			begin
				GetDItem(d,i,itemType,itemHandle,itemRect);

				if BAnd(itemType,$7F)=kHelpItem then
					count := count + 1;
			end;
		__HMCountDITLHelpItems := count;
	END;


FUNCTION  __HMSameItemState(theDialog: DialogPtr; thePoint: Point;
								VAR item: INTEGER; VAR state: INTEGER; VAR itemRect: Rect): BOOLEAN;
	{ Finds an item in theDialog at thePoint and returns true if the item & state were the same, therefore, 
		no balloon needs to be redrawn.  It returns the item & state as VAR params.
	}
	VAR	value		: INTEGER;
		itemType	: INTEGER;
		itemHandle	: Handle;
	BEGIN
		__HMSameItemState := false;							{ say that we need to redraw the balloon }
		item := FindDItem(theDialog,thePoint);
						
		if item >= 0 then
			begin
				item := item + 1;							{ convert to GetDItem offset (always 1 based) }
				
				GetDItem(theDialog,item,itemType,itemHandle,itemRect);	{ get the item's info }
						
				if BAnd(itemType,$80)<>0 then				{ item was disabled }
					state := kHMDisabledItem else
						state := kHMEnabledItem;			{ otherwise it was enabled }

				value := BAnd(itemType,$7F);				{ toss item's disabled bit }
				
				if (value >= ctrlItem + btnCtrl) then
					if (value <= ctrlItem + resCtrl) then	{ if item is a button, checkbox, or 'CNTL' then… }
					with ControlHandle(itemHandle)^^ do		{	convert it's state to it's value }
						begin
							if contrlHilite = 255 then		{ the item's control was gray, make it a disabled item }
								state := kHMDisabledItem;
								
							if value = ctrlItem + btnCtrl then
								begin
									if GetCtlMax(ControlHandle(itemHandle))>1 then
										if state = kHMEnabledItem then
											state := kHMCheckedItem
										else state := kHMOtherItem; 
								end else
									if state = kHMEnabledItem then			{ <68> BHTCA0080, check for 'checked' or 'other' only if enabled }
										begin
											if contrlValue = 1 then			{ the item's control value was 1, make it a checked item }
												state := kHMCheckedItem
											else if contrlValue > 1 then	{ the item's control value was >1, make it an 'other' item }
												state := kHMOtherItem;
										end;
						end;

				with __HMGetHelpGlobal^ do					{ return true if item & state were the same as last time }
					__HMSameItemState := (hmgState = state) and (hmgItemNum = item);
			end;
	END;

FUNCTION  __HMTrackModalHelpItems: OsErr;
	VAR	d				: DialogPtr;
		result			: OsErr;
		itemRect		: Rect;
		helpTemplateID	: INTEGER;
		founditem		: INTEGER;
		eachHelpItem	: INTEGER;
		state			: INTEGER;
		theProc			: INTEGER;
		variant			: INTEGER;
		index			: INTEGER;
		altRect			: Rect;
		pt				: Point;
		err				: OsErr;
		i				: INTEGER;
		aHelpMsg		: HMMessageRecord;
		gp				: HMGlobalPtr;
		oldPort			: GrafPtr;
		options			: LongInt;
		helpItemCount	: INTEGER;
		kind			: INTEGER;
		offset			: INTEGER;
		count			: INTEGER;
		method			: LongInt;
		currentPSN		: ProcessSerialNumber;
		frontPSN		: ProcessSerialNumber;
		sameResult		: BOOLEAN;
		
	BEGIN
		if __HMGetHelpGlobal^.hmgOurDialog then		{ if our dialog is in front then call bulk once to kick menuselect }
			if __HMBalloonBulk=noErr then;

		__HMTrackModalHelpItems := noErr;
		
		{ we already checked to see if Balloon Help mode was on }
		d := FrontWindow;

		if d=NIL then exit(__HMTrackModalHelpItems);
		
		{ quik check to see if the front window is truely a dialog }
		if WindowPeek(d)^.windowKind<>DialogKind then exit(__HMTrackModalHelpItems);
		
		{ <74> now make sure that we're being executed from the frontmost process, otherwise exit }
		err := GetCurrentProcess(currentPSN);								{ <74> }
		if err = noErr then													{ <74> }
			begin															{ <74> }
				err := GetFrontProcess(frontPSN);							{ <74> }
				if err = noErr then											{ <74> }
					err := SameProcess(currentPSN,frontPSN,sameResult);		{ <74> }
			end;															{ <74> }
		if not sameResult or (err <> noErr) then							{ <74> }
			exit(__HMTrackModalHelpItems);									{ <74> }

		helpItemCount := __HMCountDITLHelpItems(d);	{ returns how many help items are in the dialog's item list }
		
		if helpItemCount>0 then						{ there weren't any help items in the dialog so scram! }
			begin
				GetPort(oldPort);
				SetPort(d);
				GetMouse(pt);
		
				if PtInRect(pt,d^.portrect) then	{ make sure local mouse is in window portrect }
					begin
						result := -1;
						gp := __HMGetHelpGlobal;

						if __HMSameItemState(d,pt,foundItem,state,itemRect) then
							begin
								SetPort(oldPort);
								Exit(__HMTrackModalHelpItems);
							end;
																	
						for eachHelpItem := 1 to helpItemCount do	{ check each help item present in the dialog item list }
							begin
								helpTemplateID := __HMGetHelpItemResID(d,eachHelpItem,kind,offset);
								if helpTemplateID <> -1 then
									begin
										if (kind = kScanhrct) then
											begin
												result := __HMScanHRCTResource(WindowPtr(d),helpTemplateID,pt,index);
												if result = noErr then
													begin
														with gp^ do { check to see if these were the same as last time, if so, exit! }
															if (helpTemplateID = hmgState) then if (index = hmgItemNum) then
																begin
																	SetPort(oldPort);
																	Exit(__HMTrackModalHelpItems);
																end;
																
														result := __HMGetIndHelpMsg(kHMRectListResType,helpTemplateID,
																			index,kHMEnabledItem,options,pt,@itemRect,
																			theProc,variant,aHelpMsg,count);
																			
														if result = noErr then
															begin
																state := helpTemplateID;
																founditem := index;
															end;
													end;
											end else
												begin
													if founditem >= 0 then
														result := __HMFillInDialogMessage(	helpTemplateID,founditem,
																						eachHelpItem,helpItemCount,
																						state,offset,-1{current resfile},options,pt,@altRect,
																						theProc,variant,aHelpMsg);
																			
													if result = noErr then
														with itemRect do	{ correct the dialog item's hot rect [<59> always add in altRect params] }
															begin
																left	:= left		+ altRect.left;
																top		:= top		+ altRect.top;
																right	:= right	+ altRect.right;
																bottom	:= bottom	+ altRect.bottom;
															end;
												end;
												
										case result of
											noErr,hmSkippedBalloon:
												leave;
												
											otherwise { including hmGetNextHelpItem }
												cycle;
												
										end; { case }
										
									end else { end helpTemplateID=-1 }
										{$ifc HelpMgrTesting}
										DebugStr('HMGetHelpItemResID returned a -1 for a hdlg resource ID');
										{$endc}
										
								end; { end of for loop }
								
						if result = noErr then { we hit something; either an hdlg driven item or hrct driven rect }
							begin
								altRect := itemRect;
		
								with altRect do
									begin
										LocalToGlobal(topLeft);	{ note: the port is set already }
										LocalToGlobal(botRight);
										
										if LongInt(pt)=0 then
											begin
												pt		:= botRight;
												pt.h	:= pt.h - 10;
												pt.v	:= pt.v - 10;
											end else { setup the tip as it wasn't the special 'EmptyPt' case }
												if (kind <> kScanhrct) then
													begin
														pt.h := pt.h + left;	{ for 'hdlg's move the tip relative to the item's rect }
														pt.v := pt.v + top;
													end else
														LocalToGlobal(pt);		{ for 'hrct's just convert the tip to global coords }
									end;
		
								method := BSR(BAnd(options,$0000000C),2);
								result := __HMShowBalloon(aHelpMsg,pt,@altRect,
											NIL,theProc,variant,method);
		
								if result = noErr then
									with gp^ do
										begin
											hmgLastWindowPtr	:= d;
											hmgItemNum			:= founditem;
											hmgState			:= state;
										end;
							end;
					end;
					
				SetPort(oldPort);
				
		end; { helpItemCount>0 }
	END;



FUNCTION __HMShowContentBalloon(	tip: Point; alternateRect: Rect;
									theProc,variant: INTEGER;
									aHelpMsg: HMMessageRecord; options: LongInt): OsErr;
	BEGIN
		{Note: port is already set, and front window cannot be NIL!}

		LocalToGlobal(tip);			{yucko!}
		LocalToGlobal(alternateRect.topLeft);
		LocalToGlobal(alternateRect.botRight);

		if BTst(options,kBTSTAbsoluteCoords) then
			with FrontWindow^.portrect,tip do
				begin
					{tip.}h := {tip.}h + left;
					{tip.}v := {tip.}v + top;

					OffsetRect(alternateRect,left,top);
				end;

		__HMShowContentBalloon := __HMShowBalloon(aHelpMsg,tip,@alternateRect,NIL,theProc,variant,0);

		{ hmgLastBalloon information is set in BalloonPack.a, when a balloon comes up }
	END;


PROCEDURE __HMShowEasyAccessBalloon;
	VAR	aHelpMsg		: HMMessageRecord;
		altRect			: Rect;
		pt				: Point;
		result			: LongInt;
	BEGIN
		if Gestalt(gestaltEasyAccessAttr,result) = noErr then	{ Easy Access is installed }
			if GetMBARRect(altRect)=noErr then
				begin
					altRect.left := altRect.right-10;
					
					if PtInRect(__HMGetGlobalMouse,altRect) then
						with __HMGetHelpGlobal^ do
							begin	
								result := BAnd($0007,result);	{ toss hi bits so that Don Brady can use the call for somethin' else }
						
								if (hmgMenuID = kwasEasyAccess) then if (hmgItemNum = result) then
									Exit(__HMShowEasyAccessBalloon);
										
								if (result = 0) then
									begin
										if __HMIsBalloon then
											if __HMRemoveBalloon=noErr then;
										exit(__HMShowEasyAccessBalloon);
									end;
										
								with aHelpMsg do
									begin
										hmmHelpType	:= khmmString;
										GetIndString(hmmString,kHMHelpID,kHMEasy1Access-1+result);
						
										if length(hmmString)=0 then
											exit(__HMShowEasyAccessBalloon);
									end;
									
								pt.h := altRect.left + 5;
								pt.v := altRect.bottom;
						
								if __HMShowBalloon(aHelpMsg,pt,@altRect,NIL,0,2,0)=noErr then
									begin
										hmgItemNum			:= result;
										hmgMenuID			:= kwasEasyAccess;
										hmgTitleBalloon 	:= 1;	{ treat this balloon as if it was a menu title balloon }
									end;
							end;
				end;
	END;
	

FUNCTION __HMGetWindowPartCode(VAR window: WindowPtr): INTEGER;
{	returns the typical FindWindow result codes:
		inDesk = 0;
		inMenuBar = 1;
		inSysWindow = 2;
		inContent = 3;
		inDrag = 4;
		inGrow = 5;
		inGoAway = 6;
		inZoomIn = 7;
		inZoomOut = 8;
		
	Note: Don't call this unless curLayer == applicationLayer
	}

	VAR	layer			: LayerPtr;
		saveLayer		: LayerPtr;
		pt				: Point;
		windowCode		: INTEGER;
		variant			: INTEGER;
		err				: OsErr;
		PSN				: ProcessSerialNumber;
		info			: ProcessInfoRec;	
		modalClass		: INTEGER;
		windowIsFloater	: BOOLEAN;
		gp				: HMGlobalPtr;
		
	BEGIN
		pt	:= __HMGetGlobalMouse;
		
		
		{
			look in the IMlayer first, and explicitly set savelayer to NIL
		}
		saveLayer := NIL;
		windowIsFloater := false;
		windowCode := FindServiceWindow(pt,window);
		
		if ( windowCode <> inDesk ) and ( windowCode <> inMenuBar ) then		{ appears that we have something in the IMlayer }
			begin
				saveLayer := SwapCurLayer(LayerPtr(GetParent(window)));
				windowIsFloater := true;
			end else	
				windowCode := FindWindow(pt,window);
		
		if windowCode = inSysWindow then
			windowCode := __HMGetRealWindowHitResult(pt,window);
			
		case windowCode of
			inGrow:
				windowCode := inContent;
				{ *** someday, figure out whether the grow box is really visible!
						the code below doesn't work correctly as developers really
						don't use the correct type of window for their applications!
				begin
					variant := GetWVariant(window);
					if variant <> documentProc then
						windowCode := inContent;
				end;
				}
				
			inZoomOut:
				windowCode := inZoomIn;
		end;
				
		if (windowCode = inDesk) and (window<>FrontWindow) then		{ we're in the desk or in another layer's window }
			begin
				saveLayer	:= SwapCurLayer(NIL);	{set curLayer to RootLayer}
				windowCode	:= FindWindow(pt,window);
				SetCurLayer(saveLayer);
				saveLayer := NIL;					{<79> set this to NIL so we can test below}
				
				if (GetFrontWindowModalClass(modalClass) = noErr) and (modalClass = dBoxProc) then			{ <60> don't put up a balloon for window areas outside a modal dialog }
					windowCode := kHMOutsideModalWindow;
						
				with __HMGetHelpGlobal^ do
					begin
						if (window=hmgLayer) or (window=hmgWindow) then
							begin
								windowCode	:= hmgLastPartCode;
								window		:= hmgLastWindowPtr;
							end else
								if (windowCode <> inDesk) and (windowCode <> kHMOutsideModalWindow) then	{ <61> and if code isnt kHMOutsideModalWindow }
									begin
										if GetProcessFromLayer(PSN,window)=noErr then	{ works w/ WindowPtrs }
											begin
												with info do
													begin
														processInfoLength	:= Sizeof(ProcessInfoRec);
														processName			:= @hmgProcessName;
														processAppSpec		:= NIL;
													end;
													
												if GetProcessInformation(PSN,info)=noErr then
													begin
														windowCode := kHMInLayerPreamble;
														__HMStripNullFromProcessName(@hmgProcessName);
													end;
	
											end else windowCode := kHMInOtherWindow;
									end else 
										begin
											{ *** the window code was inDesk (probably the Finder desktop), so lets do somethin' }
										end;
					end;
			end else
				begin
				{	if (window<>NIL) then if (ORD(WindowPeek(window)^.hilited) = ORD(false)) and ( NOT windowIsFloater) then }	{ mouse isn't in a hilited window and it isn't a floater }

					if (window<>NIL) then
						if (ORD(WindowPeek(window)^.hilited) = ORD(false)) then			{ mouse isn't in a hilited window }
						begin
							if ( windowIsFloater )	then								{ it is a floater }
							begin
								if ( windowCode = inContent ) then						{ and the mouse is in content region  }
										windowCode := kHMInFrontFloatingWindow;			{ special Balloon for frontmost floater }
							end
							else														{ not a floater }
							begin
								windowCode := kHMInOtherWindow;
															
								if (GetFrontWindowModalClass(modalClass) = noErr) then
									if (modalClass = movableDBoxProc) or (modalClass = dBoxProc) then	{ <60> don't put up a balloon for window areas outside a modal dialog }
										windowCode := kHMOutsideModalWindow;
							end
						end;
				end;
				
		__HMGetWindowPartCode := windowCode;
		
		if saveLayer <> NIL then						{ <79> if we are in the IMlayer restore to the saved layer }
			SetCurLayer(saveLayer);
	END;


	
FUNCTION __HMShowWindowPartBalloon(window: WindowPtr; windowCode: INTEGER): BOOLEAN;
		{ returns true if a Balloon was shown }

	VAR	aHelpMsg		: HMMessageRecord;
		w				: WindowPtr;
		pt				: Point;
		i				: INTEGER;
		thisFile		: INTEGER;
		count			: INTEGER;
		aID				: INTEGER;
		mapCode			: INTEGER;
		balloonProc		: INTEGER;
		balloonVariant	: INTEGER;
		aType			: ResType;
		overrideH		: Handle;
		tempstr			: STR255;
		overridden		: BOOLEAN;
		tempBaseStr		: Handle;									{<76>}
		tempSubstituteStr : Handle;									{<76>}
		anErr			: INTEGER;									{<76>}
		savedPort		: GrafPtr;									{<78>}
		
	BEGIN
		__HMShowWindowPartBalloon := false;
			
		balloonProc		:= 0;
		balloonVariant	:= 0;
		
		overridden := false;
				
		count := CountResources('hovr');
		if count>0 then
			begin
				thisFile := CurResFile;
				for i := 1 to count do
					begin
						overrideH := GetIndResource('hovr',i);
						if HomeResFile(overrideH)=thisFile then
							begin
								GetResInfo(overrideH,aID,aType,tempstr);
								
								with HMhfdrRecordHdl(overrideH)^^ do
									begin
										balloonProc		:= theProc;
										balloonVariant	:= variant;
									end;
									
								{	Remember that FindWindow returns:
										inDrag		= 4;
										inGrow		= 5;
										inGoAway	= 6;
										inZoomIn	= 7;
										inZoomOut	= 8;
								}
								
								case windowCode of
									inDrag..inZoomIn: mapCode := windowCode - 3;	{ convert to our indexes }
									
									kHMInOtherWindow :
										mapCode := 5;
									kHMInLayerPreamble..kHMInLayerBody:
										mapCode := 6;
									kHMOutsideModalWindow:
										mapCode := 7;
									otherwise mapCode := windowCode;
								end;
								
								overridden := __HMExtractHelpMsg(aType,aID,mapCode,0,aHelpMsg)=noErr;
								if overridden then
									leave;
							end;
					end;
			end;
				
		if not overridden then
			with aHelpMsg do
				begin
					hmmHelpType	:= khmmString;
					
					case windowCode of
						kHMInOtherWindow..kHMOutsideModalWindow:
							begin
								GetIndString(hmmString,kHMHelpID,windowCode);
								if windowCode=kHMInLayerPreamble then
									begin
										GetPort(savedPort);																{<78>}
										w := __HMGetHelpGlobal^.hmgHelpWindow;											{<78>}
										IF w <> NIL THEN																{<78>}
											SetPort(w);																	{<78>}
										tempBaseStr := NewHandle(Length(hmmString));									{<76>}
										tempSubstituteStr := NewHandle(Length(__HMGetHelpGlobal^.hmgProcessName));		{<76>}
										IF (tempBaseStr <> NIL) AND (tempSubstituteStr <> NIL) THEN BEGIN				{<76>}
											BlockMove(@hmmString[1], tempBaseStr^, Length(hmmString));					{<76>}
											with __HMGetHelpGlobal^ do													{<76>}
												BlockMove(@hmgProcessName[1], tempSubstituteStr^, Length(hmgProcessName));	{<76>}
											anErr := ReplaceText(tempBaseStr, tempSubstituteStr, '^0');					{<76>}
											hmmString[0] := Chr(GetHandleSize(tempBaseStr));							{<76>}
											BlockMove(tempBaseStr^, @hmmString[1], Ord(hmmString[0]));					{<76>}
										END;																			{<76>}
										DisposeHandle(tempBaseStr);														{<76>}
										DisposeHandle(tempSubstituteStr);												{<76>}
										SetPort(savedPort);																{<78>}
									end;
							end;
						
						kHMInFrontFloatingWindow:																		{<80>}
							begin																						{<80>}
								GetIndString(hmmString, kHMHelpID, windowCode);											{<80>}
							end;																						{<80>}

						otherwise 
							GetIndString(hmmString,kHMHelpID,windowCode+(kHMInDragIndex-inDrag));
					end;
	
					if length(hmmString)=0 then	{ yikes! what happened to our string resource? }
						{$ifc HelpMgrTesting}
						hmmString	:= 'The window parts help resource was not found…';
						{$elsec}
						Exit(__HMShowWindowPartBalloon);
						{$endc}
				end;
				
		if __HMShowBalloon(aHelpMsg,__HMGetGlobalMouse,NIL,NIL,balloonProc,balloonVariant,0) = noErr then
			with __HMGetHelpGlobal^ do
				begin
					hmgItemNum			:= windowCode;
					hmgMenuID			:= kwasWindowPart;
					
					__HMShowWindowPartBalloon := true;
				end;
	END;



FUNCTION  __HMGetBalloonWindow(VAR window: WindowPtr): OsErr;
	BEGIN
		window := __HMGetHelpGlobal^.hmgWindow;
		__HMGetBalloonWindow := noErr;
	END;
	
	
FUNCTION  __HMScanTemplateItems(whichID,whichResFile: INTEGER; whichType: ResType): OsErr;
	VAR	d				: DialogPtr;
		result			: OsErr;
		founditem		: INTEGER;
		state			: INTEGER;
		theProc			: INTEGER;
		variant			: INTEGER;
		oldResFile		: INTEGER;
		count			: INTEGER;
		index			: INTEGER;
		itemRect		: Rect;
		altRect			: Rect;
		pt				: Point;
		err				: OsErr;
		aHelpMsg		: HMMessageRecord;
		gp				: HMGlobalPtr;
		oldPort			: GrafPtr;
		options			: LongInt;
		
	BEGIN
		__HMScanTemplateItems := paramErr;

		d := FrontWindow;
		if d=NIL then exit(__HMScanTemplateItems);
						
		if (LongInt(whichType) = LongInt(kHMDialogResType)) or (LongInt(whichType) = LongInt(kHMRectListResType)) then
			begin
				__HMScanTemplateItems := hmSameAsLastBalloon;	{ assume that it's already displayed }
				
				GetPort(oldPort);
				SetPort(d);
				GetMouse(pt);
				
				if PtInRect(pt,d^.portrect) then 								{ make sure local mouse is in window portrect }
					begin
						gp := __HMGetHelpGlobal;

						if (LongInt(whichType) = LongInt(kHMDialogResType)) then { see if the mouse is in any of the dialog item rects }
							begin
								{ <67> the following check MUST be short circuit! }
								if (WindowPeek(d)^.windowKind<>DialogKind) | (__HMSameItemState(d,pt,foundItem,state,itemRect)) then									begin
										SetPort(oldPort);
										Exit(__HMScanTemplateItems);
									end;

								if whichResFile<>-1 then
									begin
										oldResFile := CurResFile;
										UseResFile(whichResFile);
									end;

								result := __HMFillInDialogMessage(whichID,founditem,0,0,state,0{offset},whichResFile,options,
																		pt,@altRect,theProc,variant,aHelpMsg);
								
								if result = noErr then
									with itemRect do	{ correct the dialog item's “hot” rect [<59> always add in altRect params] }
										begin
											left	:= left		+ altRect.left;
											top		:= top		+ altRect.top;
											right	:= right	+ altRect.right;
											bottom	:= bottom	+ altRect.bottom;
										end;

							end else { it must have been an hrct resource that we wanted to scan }
								begin
									if whichResFile<>-1 then
										begin
											oldResFile := CurResFile;
											UseResFile(whichResFile);
										end;

									result := __HMScanHRCTResource(WindowPtr(d),whichID,pt,index);
									if result = noErr then
										begin
											with gp^ do { check to see if these were the same as last time, if so, exit! }
												if (hmgState = whichID) then if (hmgItemNum = index) then
													begin
														if whichResFile<>-1 then
															UseResFile(oldResFile);

														SetPort(oldPort);
														Exit(__HMScanTemplateItems);
													end;
															
											result := __HMGetIndHelpMsg(kHMRectListResType,whichID,
																index,kHMEnabledItem,options,pt,@itemRect,
																theProc,variant,aHelpMsg,count);
																
											if result = noErr then
												begin
													state := whichID;
													founditem := index;
												end;
										end;
								end;
							
						if result <> noErr then
							begin
								if whichResFile<>-1 then
									UseResFile(oldResFile);

								SetPort(oldPort);
								
								__HMScanTemplateItems := result;
								exit(__HMScanTemplateItems);
							end;
							
						altRect := itemRect;
						with altRect do
							begin
								LocalToGlobal(topLeft);	{ watch the port!  Make sure it is set! }
								LocalToGlobal(botRight);
								
								if LongInt(pt)=0 then
									begin
										pt		:= botRight;
										pt.h	:= pt.h - 10;
										pt.v	:= pt.v - 10;
									end else { setup the tip as it wasn't the special 'EmptyPt' case }
										if (LongInt(whichType) = LongInt(kHMDialogResType)) then
											begin
												pt.h := pt.h + left;	{ for 'hdlg's move the tip relative to the item's rect }
												pt.v := pt.v + top;
											end else
												LocalToGlobal(pt);		{ for 'hrct's just convert the tip to global coords }
							end;
							
						if BTst(options,kBTSTAbsoluteCoords) then
							with WindowPtr(d)^.portrect,pt do
								begin
									{pt.}h := {pt.}h + left;
									{pt.}v := {pt.}v + top;
				
									OffsetRect(altRect,left,top);
								end;

						result := __HMShowBalloon(aHelpMsg,pt,@altRect,
									NIL,theProc,variant,BSR(BAnd(options,$0000000C),2) { convert hmSaveBitsNoWindow & hmSaveBitsWindow to method parameter });

						if whichResFile<>-1 then
							UseResFile(oldResFile);

						if result = noErr then
							with gp^ do
								begin
									hmgItemNum			:= founditem;
									hmgState			:= state;
								end;
								
						__HMScanTemplateItems := result;

					end else { mouse wasn't in window's port rect so return hmBalloonAborted }
						__HMScanTemplateItems := hmBalloonAborted;
						
				SetPort(oldPort);
			end;
	END;


FUNCTION  __HMExtractHelpMsg(	whichType: ResType;
								whichResID, whichMsg, whichState: INTEGER;
								VAR aHelpMsg: HMMessageRecord): OsErr;
	VAR options			: LongInt;
		tip				: Point;
		altRect			: Rect;
		theProc			: INTEGER;
		variant			: INTEGER;
		count			: INTEGER;
	BEGIN
		__HMExtractHelpMsg := __HMGetIndHelpMsg(whichType,whichResID,whichMsg,whichState,
												options,tip,@altRect,theProc,variant,aHelpMsg,count);
	END;


END.
