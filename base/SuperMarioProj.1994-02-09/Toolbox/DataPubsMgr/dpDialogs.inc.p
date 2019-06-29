{
	File:		dpDialogs.inc.p

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<37>	 3/13/91	ngk		MTM,#B6-MTM-001: Fix date string in 'OptionsDialog to show that
									edition is missing instead of Jan 1904.
		<36>	 2/10/91	ngk		KSM,#82241: Decrease stack requirements in 'OptionsDialog by
	   								allocating locals in heap. 
		<35>	 1/14/91	ngk		<MM> Fix Get Edition Now error handling. Fix pop up dropshadow.
	   								Fix SysBeep on subdialogs, til after they are drawn.  Remove
									extraneous flash of buttons.  Conditioanlize range checking on 
									formatsMask.  Fix sound preview to use ICON.
		<34>	12/14/90	ngk		<MM>Added feature of allowing user to reconnect to an edition in the
	   								options dialog, by pressing the Get/Send Now button.  Added error
									dialog to support it that fails.  Fixed NewSubscriber dialog to not
									flash preview if last edition can not be subscribed to now.  Added
									blank string to STR# to make errors easier to localize.  Fixed disabled
									popup bug if edition name contained a "(".
		<33>	11/20/90	DC		fixed names of id's of system icons
		<32>	10/31/90	ngk		<KSM> Fix bug in pop up menu where it was drawing one to many
									items.
		<31>	10/29/90	ngk		Fix memory leak in SectionOptionsDialog, it was not disposing
	   								of the item list.  In SectionOptionsSetup, 1) also check fileIsMissing
									field to disable buttons, 2) allow / in pop up menu names, 3) be sure
									to set canceled field of reply record.  In optionDialog, set up
									dialog flags, as per new StdFilterProc.
		<30>	 8/13/90	dba		fix Nick’s typo
		<29>	 8/13/90	ngk		Fix bug in optionsFilter, deferencing NIL.
		<28>	  8/8/90	ngk		Fix getting string for name of desktop
		<27>	  8/6/90	ngk		dir bit is 4 not 3.
		<26>	 7/14/90	ngk		In draw preview user item , check that drawing is really
									necessary.
		<25>	  7/2/90	ngk		Fix bit test of ioDirFlg, un-comment-out use of StdFilter
		<24>	  7/2/90	ngk		Use new StandardFile change selection hook. Create new redraw
	   								preview hook item.  Moved NewPublisher functions out of nest.  
									Check for desktop folder in 'options path.  Did some code savings
									by using BlockMove and more with statements. Fix NP expansion location.
									Restructured options dialog code to keep calling apps dialog hook
									and filter even during sub dialogs.  Added sub dialog for open
									publisher errors. 
		<23>	 6/20/90	ngk		Changed CanonicalFileSpec to FSSpec.
		<22>	  6/8/90	ngk		Update to name changes in StandardFile.p
		<21>	  6/1/90	ngk		Get save/restore bits to work in OptionsDialog. Do a simple
	   								preview for 'snd ' formats.
		<20>	 5/31/90	ngk		Moved all nested procedures in SectionOptionsDialog out, to
	   								get better code generation.  Rearranged items in OptionsDialog.
									Now use pop up instead of list for path to edition.  Use
									AppendDITL instead of my own hack.  Move OK/Cancel down when
									dialog is expanded and remove gray bar.  Fixed bug with
									Opener params for eoCanSubscribe.  Added formatsMask to 
									NewSubscriberDialog.
		<19>	  5/3/90	ngk		Preflight goto in OptionsDialog. Fix initial flash of preview in
	   								NewSubscriberDialog.  Dim "Get Edition Now" button if control
									block is NIL.
		<18>	 4/18/90	ngk		In options dialog, don't sync if no control block. Add seconds
	   								to edition times.
		<17>	 4/11/90	ngk		Upgrade to new SF dialogs
		<16>	  4/7/90	ngk		Call dpPubSync in SectionOptionsDialog. Use new failure
									handling.
		<15>	 3/25/90	ngk		Moved script out of NewPublisherReply into EditionContainerSpec.
		<14>	 3/21/90	ngk		Rearranged NewXDialog items and removed box around preview.
		<12>	 3/20/90	ngk		Added script to NewPublisherReply
		<11>	 3/17/90	ngk		Use dialog centering code in layer manager, instead of my hack.
	   								No longer map update events to null events in subHook 'cause SF
									does it for me.  Made dp_DrawPreview public.
		<10>	 3/10/90	ngk		Fix bug in NewPublisher hook of offset off by one. Removed code
									to handle purging 'dlcd' handle, since dialog code is now part
									of PACK 11.
		 <9>	 2/27/90	ngk		In NewSubscriberDialog, map update events to null events if no
									one filters update event. This makes preview work even if app
									ignores update events. In OptionsDialog, Show/HideDItem section
									mdDate time and title instead of not drawing and Enable/Disable
									PathList, all for sake of help balloons.
		 <8>	 2/25/90	ngk		Use hook constants from StandardFile.p Use new names for
									CustomStandardXFile. Dim "Find Publisher", use grayed-out path
									list from alias, and use resource string for "latest time" when
									section has no control block. Use StandardFile's LDEF in
									'OptionsDialog.
		 <7>	  2/1/90	ngk		Fix bug in auto-centering of NewPublisher dialog. Flash OK and
									Cancel button when using keyboard equivalent.
		 <6>	 1/22/90	ngk		Remove last reminates of comments box in dialogs
		 <5>	  1/8/90	ngk		Fix bug in NewSubscriberFileFilter, called canSubscribe
									incorrectly.
		 <4>	  1/8/90	ngk		fixes
		 <3>	  1/7/90	ngk		Made call to GetAppRefNum use trap version.
		 <2>	  1/6/90	ngk		Changed dialogs to have simple and extendable form. Changed
									SectionOptions to use a reply record. Added format to
									NewPublisher preview. PreFlight Standard file and List package.
									Removed all code dealing with edition comments. Added
									Command-Period exit to SectionOptionsDialog.
	   <2.8>	 12/1/89	ngk		Fixed bug of dereferencing the unlocked dummy LDEF handle.
	   <2.7>	 12/1/89	ngk		Fixed bug where I forgot to set port to the dialog in
									SectionOptions
	   <2.6>	11/13/89	ngk		Changed SectionOptionsDialog to allow additioal dialog items
	   <2.5>	 11/4/89	ngk		Added dpScalePict to let NewPublisherDialog scale pict better.
									Fixed DialogsOnceOnlyQuit to not save an alias to nothing.
									Changed AppRefNum to be a handle to app's globals. Made dialog
									code unlocked and purgeable, after use. Added Locking and
									restoring of Pack11 in all public routines.
	   <2.4>	10/24/89	ngk		Moved inline into dpMisc.p and glue code.
	   <2.3>	10/14/89	ngk		Moved NewStandardFile definitions to Packages.p
	   <2.2>	10/13/89	ngk		Bug fixes to SectionOptionsDialog. Fixed bug of disposing of
									resource in Init.
	   <2.1>	10/13/89	ngk		Moved some type definitions to dpInternalTypes. Handle
									unregistered sections in SectionOptionsDialog better.
	   <2.0>	 10/2/89	ngk		Added list path to SectionOptions, change parameters. Updated to
									new API.
	   <1.9>	 9/25/89	ngk		Redid SectionOptionsDialog to be closer to real thing. Interface
									still the same.
	   <1.8>	 9/18/89	ngk		Updated NewSFReply to match changes is StandardFile. Added
									filterProcs to PA and ST. Kept scale on preview in ST.
	   <1.7>	  9/7/89	ngk		Fixed autokey bug in NewPublisher comments
	   <1.6>	 8/29/89	ngk		Used NewSFReply record. Changed preview to abide by new pub file
									opening coneventions. Inset finder comments & blink cursor.
	   <1.5>	  8/8/89	ngk		Overhauled dpNewPublisherDialog and dpNewSubscriberDialog to
									show a preview and Finder comments. Used routines ending in
									"Glue" so I could use local procedures and share variables
									without needing globals. Changed dpGetLastPublication to return
									a PubSpec and no modes. Added auto-centering of dialogs if
									'where' is (-1,-1).
	   <1.4>	  7/3/89	ngk		Change dpNewPublisherDialog to get prompt from DITL
	   <1.3>	 6/15/89	ngk		Fixed dpGetLastUsed to return a vRefNum instead of a drive num
									still waiting for SFPutFile to tell me when replacing
	   <1.2>	 5/31/89	ngk		Change to use Alias instead of SofaLinks
	   <1.1>	 5/29/89	ngk		SectionOptionsDialog Added DITL to NewPublisherDialog Changed
									Interface to NewPublisherDialog to return UpdateMode
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}

CONST
	{ stuff for NewSubscriber Dialog }
	rNewSubscriberDialog 	= -5792;	{ owned DITL & DLOG resource of PACK 11 }
	kNSPreviewUserItem			= 10;  		{ DITL item number of User Item }
	kNSPreviewBottomPictItem	= 13;  		{ DITL item number of User Item }
	kNSExpansionUserItem		= 14;		{ DITL item number of User Item }

	{ stuff for NewPublisher Dialog }
	rNewPublisherDialog		= -5791;	{ owned DITL & DLOG resource of PACK 11 }
	kNPPreviewUserItem			= 13;  		{ DITL item number of User Item }
	kNPPreviewBottomPictItem	= 16;  		{ DITL item number of User Item }
	kNPExpansionUserItem		= 17;		{ DITL item number of User Item }
	
	{ stuff for Section Info Dialog }
	rSubscriberOptionsDialog 	= -5790;	{ owned DITL & DLOG resource of PACK 11 }
	rPublisherOptionsDialog 	= -5789;	{ owned DITL & DLOG resource of PACK 11 }
	kSOOKButton						= 1;  		{ DITL item number of Button }
	kSOCancelButton					= 2;  		{ DITL item number of Button }
	kSOOKOutLineUserItem			= 3;  		{ DITL item number of User Item }
	kSOCancelSectionButton			= 4;  		{ DITL item number of Button }
	kSOGotoPublisherButton			= 5;  		{ DITL item number of Button }
	kSODoEditionButton				= 6;  		{ DITL item number of Button }
	kSOUpdateMode1Button	 		= 7;  		{ DITL item number of RadioButton }
	kSOUpdateMode0Button			= 8;  		{ DITL item number of RadioButton }
	kSOMainTitleText				= 9;  		{ DITL item number of Static Text }
	kSOPopUpUserItem				= 10;  		{ DITL item number of User Item }
	kSODoubleLinePictItem			= 11;  		{ DITL item number of Pict Item }
	kSOClusterBorderPictItem		= 12;  		{ DITL item number of Pict Item }
	kSOUpdateModeClusterText		= 13;  		{ DITL item number of Static Text }
	kSOEditionModDateTitleUserItem	= 14;  		{ DITL item number of User Item }
	kSOSectionModDateTitleUserItem	= 15;  		{ DITL item number of User Item }
	kSOEditionModDateTimeUserItem	= 16;  		{ DITL item number of User Item }
	kSOSectionModDateTimeUserItem	= 17;  		{ DITL item number of User Item }
	kSOCancelOKDividerPictItem		= 18;  		{ DITL item number of Pict Item }
	kSOHelpItem						= 19;  		{ DITL item number of Help Item }
	kSOExpansionUserItem			= 20;		{ DITL item number of User Item }

	{ stuff for remove section sub-dialog }
	rRemoveYesNoDialog			= -5788;	{ owned DITL & DLOG resource of PACK 11 }
	kRYNYesButton					= 1;		{ DITL item number of Button }
	kRYNNoButton					= 2;		{ DITL item number of Button }
	kRYNStaticText					= 3;		{ DITL item number of Static Text }	

	{ stuff for open publisher error sub-dialog }
	rOpenPublisherErrorDialog	= -5787;	{ owned DITL & DLOG resource of PACK 11 }
	kOPEOKButton					= 1;		{ DITL item number of Button }
	
	{ stuff for reconnect to edition error sub-dialog }
	rReconnectErrorDialog		= -5786;	{ owned DITL & DLOG resource of PACK 11 }
	kREOKButton					= 1;		{ DITL item number of Button }
	
	kSOSTReditionModDateTitleIndex	= 1;		{ index into STR# }
	kSOSTRSectionModDateTitleIndex	= 2;		{ index into STR# }
	kSOSTRcancelWarningIndex		= 3;		{ index into STR# }
	kSOSTRdefaultEditionTimeIndex	= 4;		{ index into STR# }
	kSOSTRVolumeNFErrorIndex		= 5;		{ index into STR# }
	kSOSTRFileNFErrorIndex			= 6;		{ index into STR# }
	kSOSTRUnknowErrIndex			= 7;		{ index into STR# }

	{ General dialgo stuff }
	kRadioButtonOn	 		= 1;		{ control value for on RadioButton }
	kRadioButtonOff	 		= 0;		{ control value for off RadioButton }
	kInvertButtonHilite		= 1;		{ inverted highlighting state for a button }
	kNormalButtonHilite		= 0;		{ normal highlighting state for a button }
	kDimButtonHilite		= 255;		{ dimmed button title }
	
	
	emReconnectErrDialogRefCon = 'cnct'; { this should go in Editions.[pha]
	
	{ some character codes }
	chTAB		=	CHR(9);
	chEnter		=	CHR(3);
	chReturn	=	CHR(13);
	chESC		=	CHR(27);
	
	{ low mem global with Pack3 handle }
	kPack3Handle	=	$AC4;
	AppPacks  		=	$AB8;  { array [0..7] of handles to packs }
	
	{ Generic update mode }
	umAutomatic		=	0;
	
	{ bit in Finder flags }
	fdAliasMask 	=	$8000; 	
	fdAliasBit		=	15;					
	
	
	FUNCTION GetA6: LONGINT;
			INLINE $2E8E;	{ MOVE.L A6,(A7) }


	FUNCTION CallUserExpFilter(theDialog: DialogPtr; VAR theEvent: EventRecord; 
								itemOffset: INTEGER; VAR itemHit: INTEGER; 
								yourDataPtr: Ptr; routine: ProcPtr): BOOLEAN; 
		INLINE	$205F,		{	MOVE.L 	(SP)+, A0	}
				$4E90;		{	JSR		(A0)		}


	FUNCTION CallUserExpDlgHook(itemOffset, item: INTEGER; 
							 	theDialog: DialogPtr; yourDataPtr: Ptr;
								routine: ProcPtr): INTEGER;
		INLINE	$205F,		{	MOVE.L 	(SP)+, A0	}
				$4E90;		{	JSR		(A0)		}

				


	{------------- dpPreFlightPackage -------------}
	FUNCTION dpPreFlightPackage(number: INTEGER): OSErr;
	VAR
		pack:	Handle;
	BEGIN
		pack := pHandle(AppPacks+number*SizeOf(Handle))^;
		IF pack^ = NIL THEN
			LoadResource(pack);
		IF pack^ <> NIL THEN
		BEGIN
			HNoPurge(pack);
			dpPreFlightPackage := noErr;
		END ELSE
			dpPreFlightPackage := memFullErr;
	END; { dpPreFlightPackage }


		
	{------------- dpPostFlightPackage -------------}
	FUNCTION dpPostFlightPackage(number: INTEGER): OSErr;
	VAR
		pack:	Handle;
	BEGIN
		pack := pHandle(AppPacks+number*SizeOf(Handle))^;
		HPurge(pack);
		dpPostFlightPackage := noErr;
	END; { dpPostFlightPackage }

	
	{------------- dpAppendDITL -------------}
	PROCEDURE dpAppendDITL(theDialog: DialogPtr; extentionDITLresID, mainUserItem: INTEGER);
	VAR
		extentionDITL:	Handle;
	BEGIN				
		{ get extentionDITL }
		extentionDITL := GetResource('DITL', extentionDITLresID);
		IF extentionDITL = NIL THEN FailOSErr(memFullErr);

		AppendDITL(theDialog, extentionDITL, -mainUserItem);
	
		ReleaseResource(extentionDITL);
	END; { dpAppendDITL }


	{------------- MoveDItem -------------}
	PROCEDURE MoveDItem(theDialog: DialogPtr; item, dv: INTEGER);
	VAR
		itemType:		INTEGER;
		itemToChange:	Handle;
		itemRect:		Rect;
	BEGIN	
		{ move bounding rect in item list }
		GetDItem(theDialog, item, itemType,itemToChange,itemRect);
		itemRect.bottom := itemRect.bottom + dv;
		itemRect.top := itemRect.top + dv;
		SetDItem(theDialog, item, itemType,itemToChange,itemRect);
		
		{ if it is a control, then move the control }
		IF bAND(itemType, ctrlItem) <> 0 THEN
		BEGIN
			MoveControl(ControlHandle(itemToChange), itemRect.left, itemRect.top);
		END;
	END; { MoveDItem }
	
	
	{------------- SetDItemEnable -------------}
	PROCEDURE SetDItemEnable(theDialog: DialogPtr; itemNumber: INTEGER; enable: BOOLEAN);
	VAR
		itemToChange: 	Handle;	        	
		itemType:		Point;	  	{ for fast and lose type coersions }      
		itemRect:		Rect;
	BEGIN
		GetDItem(theDialog, itemNumber, itemType.h, itemToChange, itemRect);
		IF enable 
			THEN bClr(LONGINT(itemType), 7)
			ELSE bSet(LONGINT(itemType), 7);
		SetDItem(theDialog, itemNumber, itemType.h, itemToChange, itemRect);
	END; { SetDItemEnable }
	
	
{========================== Drawing Previews ============================}



	{------------- dpScalePict -------------}
	PROCEDURE dpScalePict(thePictBounds: Rect; previewRect: Rect; VAR imageRect: Rect);
	VAR
		imageSize:		Point;
		previewSize:	Point;
		tempLong:		LONGINT;
		tempInteger:	INTEGER;
	BEGIN
		WITH thePictBounds DO
		BEGIN
			imageSize.h := right - left;
			imageSize.v := bottom - top;
		END; {with}						
		WITH previewRect DO
		BEGIN
			previewSize.h := right - left;
			previewSize.v := bottom - top;
		END; {with}						
	
		{ scale so it fits horizontally }
		IF (imageSize.h > previewSize.h) THEN
		BEGIN
			UnSignedMultiply({mult1}imageSize.v, {mult2}kPreviewWidth, {VAR result}tempLong);
			UnSignedDivide({numerator}tempLong, {denom}imageSize.h, 
							{VAR quotient}imageSize.v, {VAR remainder}tempInteger);
			imageSize.h := kPreviewWidth;
		END;
	
		{ scale so it fits vertically }
		IF (imageSize.v > previewSize.v) THEN
		BEGIN
			UnSignedMultiply({mult1}imageSize.h, {mult2}kPreviewHeight, {VAR result}tempLong);
			UnSignedDivide({numerator}tempLong, {denom}imageSize.v, 
							{VAR quotient}imageSize.h, {VAR remainder}tempInteger);
			imageSize.v := kPreviewHeight;
		END;
	
		{ if image size is smaller than preview box then center it }
		WITH previewRect DO
			SetRect(imageRect, left, top, left+imageSize.h, top+imageSize.v);
		offsetRect(imageRect, (previewSize.h-imageSize.h) DIV 2, (previewSize.v-imageSize.v) DIV 2);

	END; { dpScalePict }


	{------------- dp_DrawPreview -------------}
	FUNCTION dp_DrawPreview(theData: Handle; theFormat: FormatType; previewRect: Rect): OSErr;
	VAR
		imageRect:		Rect;
		curPort: 		GrafPtr;
		tempL:			LONGINT;
		orginalFont:	INTEGER;
		originalSize:	INTEGER;
		iconRect:		Rect;
	BEGIN
		dp_DrawPreview := noErr;
		IF theData = NIL 
			THEN EXIT(dp_DrawPreview);

		{ if preview is a 'snd ' then show the standard sound preview icon }
		IF LONGINT(theFormat) = LONGINT('snd ') THEN
		BEGIN
			iconRect.top := 0;
			iconRect.left := 0;
			iconRect.bottom := 32;
			iconRect.right := 32;
			dpScalePict(iconRect, previewRect, imageRect);
			theData := GetResource('ICON', -16491);	 { ### this should be a constant in SoundMgrPriv.p }
			PlotIcon(imageRect, theData);
		END ELSE

		{ if preview is a 'PICT' then do some smart scaling/centering before imaging }
		IF LONGINT(theFormat) = LONGINT('PICT') THEN
		BEGIN
			dpScalePict(PicHandle(theData)^^.picFrame, previewRect, imageRect);
			DrawPicture(PicHandle(theData), imageRect);
		END ELSE
		
		{ if preview is a 'prvw' then image it directly }
		IF LONGINT(theFormat) = LONGINT(kPreviewFormat) THEN
		BEGIN
			DrawPicture(PicHandle(theData), previewRect);
		END ELSE
		
		{ if preview is TEXT then do a text box }
		IF LONGINT(theFormat) = LONGINT('TEXT') THEN
		BEGIN
			{ remeber what the current font is }
			GetPort(curPort);
			orginalFont := curPort^.txFont;
			originalSize := curPort^.txSize;
			
			{ set to the "small app" font  }
			tempL := GetScript(FontScript, smScriptSmallFondSize);
			TextFont(HiWrd(tempL));
			TextSize(LoWrd(tempL));
			
			{ draw the text }
			HLock(theData);
			TextBox(theData^, GetHandleSize(theData), previewRect, GetSysJust);
			
			{ restore current font }
			TextFont(orginalFont);
			TextSize(originalSize);
		END ELSE
		
		{ if preview is not of a known kind then return error }
		BEGIN
			dp_DrawPreview := noTypeErr;
		END;
	END; { dp_DrawPreview }


	
{========================== NewSubscriberDialog ============================}
	
	
	PROCEDURE dpNewSubscriberDrawPreview(theDialog: DialogPtr; A6link: LONGINT); EXTERNAL;
	
	
	PROCEDURE dpNewSubscriberDrawPreviewGlue(theDialog: DialogPtr; itemNo: INTEGER);
	BEGIN
		dpNewSubscriberDrawPreview(theDialog, dpGetGlobalsLocation^^.A6Link);
	END;
	
	
	{------------- dp_NewSubscriberExpDialog -------------}
	FUNCTION dp_NewSubscriberExpDialog(VAR reply: NewSubscriberReply; where: Point; 
									extentionDITLresID: INTEGER; dlgHook: ExpDlgHookProcPtr; 
									filterProc:	ExpModalFilterProcPtr; callBackPtr: Ptr): OSErr;
	VAR
		saveVol:			INTEGER;
		saveDir:			LONGINT;
		previewSize:		Point;
		previewRect:		Rect;
		fileTypes:			SFTypeList;
		getFileReply:		StandardFileReply;
		thisApp:			AppRefNum;
		lastSelection: 		FSSpec;
		nsItemCount:		INTEGER;
		fi:					FailInfo;
	
		{$PUSH } {$Z+}
		PROCEDURE dpNewSubscriberDrawPreview(theDialog: DialogPtr);  FORWARD;
		{$POP }
	
	
	
		{-------------  dpNewSubscriberDrawPreview -------------}
		PROCEDURE dpNewSubscriberDrawPreview(theDialog: DialogPtr); 
		VAR
			previewFormat:		FormatType;
			theData:			Handle;
			
			PROCEDURE FailDraw(anErr: OSErr);
			BEGIN
				IF anErr <> noErr THEN
				BEGIN
					{$IFC qCatchFailures } Debugger; {$ENDC }
					IF theData <> NIL
						THEN DisposHandle(Handle(theData));
					EXIT(dpNewSubscriberDrawPreview);
				END;
			END; { FailDraw }

		BEGIN
			theData := NIL;
			
			{ _ModalDialog calls all user item's draw, even if outside the update region }
			{ so I will not draw unless really necessary }
			IF RectInRgn(previewRect, WindowPeek(theDialog)^.port.visRgn) THEN
			BEGIN
				{ clear background }
				PenNormal;
				EraseRect(previewRect);
				
				{ check if a previewable item is selected }
				IF LENGTH(reply.container.theFile.name) > 0 THEN
				BEGIN
					{ get preview }
					theData := NewHandle(200);
					FailDraw(MemError);
					FailDraw(GetStandardFormats(reply.container, previewFormat, theData, NIL, NIL));
					
					{ draw it }
					FailDraw(DrawPreview(theData, previewFormat, previewRect));
	
					DisposHandle(Handle(theData));
				END; {if}
			END; {if}	
		END; { dpNewSubscriberDrawPreview }
		
		
	
		{-------------  dpNewSubscriberHook -------------}
		FUNCTION dpNewSubscriberHook(sfItem: INTEGER; theDialog: DialogPtr): INTEGER;
		VAR
			itemToChange:	Handle;	        	{needed for GetDItem and SetCtlValue}
			itemType:		INTEGER;	        {needed for GetDItem}
		BEGIN  
			IF WindowPeek(theDialog)^.refcon = LONGINT(sfMainDialogRefCon) THEN
			BEGIN
				IF sfItem = sfHookFirstCall THEN
				BEGIN
					{ save off how many items are really in this dialog }
					nsItemCount := CountDITL(theDialog); 
					
					IF extentionDITLresID <> 0 
						THEN dpAppendDITL(theDialog, extentionDITLresID, kNSExpansionUserItem)	{ append the app's items }
						ELSE HideDItem(theDialog, kNSPreviewBottomPictItem);					{ only show this is appending items }
				END; {if}
				
				IF sfItem = sfHookNullEvent THEN
				BEGIN
					{ see if user has changed selection }
					IF NOT dpSameBytes(@lastSelection, @getFileReply.sfFile, SizeOf(FSSpec)) THEN
					BEGIN
						{ lastSelection := getFileReply.sfFile; }
						BlockMove(@getFileReply.sfFile, @lastSelection, SizeOf(lastSelection));
						reply.container.theFile.name := '';
						
						{ see if new selection is a container } 
						IF (ORD(getFileReply.sfIsFolder) = 0)  					{ NOT getFileReply.sfIsFolder }
							{&} THEN IF (ORD(getFileReply.sfIsVolume) = 0) 		{ NOT getFileReply.sfIsVolume }
							{&} THEN IF getFileReply.sfFile.name[0] <> chr(0)	{ valid file name }
							{&} THEN IF (NOT bTST(getFileReply.sfFlags, fdAliasBit)) THEN { not alias to edition }
						BEGIN
							BlockMove(@getFileReply.sfFile, @reply.container.theFile, SizeOf(FSSpec)); { reply.container.theFile := getFileReply.sfFile; }
							reply.container.thePart := kPartsNotUsed;
						END;
						sfItem := emHookRedrawPreview;
					END; {if}
				END; {if}
			END; {if}
	
			{ give app chance to do hook call }
			IF dlgHook <> NIL 
				THEN sfItem := CallUserExpDlgHook(nsItemCount, sfItem, theDialog, callBackPtr, dlgHook);
			
			{ do post-hook processsing }
			IF WindowPeek(theDialog)^.refcon = LONGINT(sfMainDialogRefCon) THEN
			BEGIN
				IF sfItem = sfHookFirstCall THEN
				BEGIN
					{ do my set up }
					GetDItem(theDialog,kNSPreviewUserItem, itemType,itemToChange,previewRect);
					SetDItem(theDialog,kNSPreviewUserItem, itemType,Handle(@dpNewSubscriberDrawPreviewGlue),previewRect);
	
					WITH previewRect DO
					BEGIN
						previewSize.h := right - left;
						previewSize.v := bottom - top;
					END; {with}	
					
					{ tell SF to select this guy }
					BlockMove(@reply.container.theFile, @getFileReply.sfFile, SizeOf(FSSpec));  { getFileReply.sfFile := reply.container.theFile; }
					getFileReply.sfScript := reply.container.theFileScript;
					
					{ now forget about this guy.  It could be unsubscribable (masked out) and don't want preview to flash }
					reply.container.theFile.name[0] := chr(0);
					BlockMove(@reply.container.theFile, @lastSelection, SizeOf(lastSelection)); { lastSelection := reply.container.theFile;}
					sfItem := sfHookChangeSelection;					
				END ELSE
				IF sfItem = emHookRedrawPreview THEN
				BEGIN
					InvalRect(previewRect);
				END; {if}
			END; {if}
			
			dpNewSubscriberHook:= sfItem;  {!!!!very important !!!! We must pass SF's 'standard' item hits back to SF}
		END;  { dpNewSubscriberHook }


		{------------- dpNewSubscriberFilter -------------}
		FUNCTION dpNewSubscriberFilter(theDialog: DialogPtr; VAR theEvent: EventRecord; VAR itemHit: INTEGER): BOOLEAN; 
		BEGIN
			dpNewSubscriberFilter := FALSE;
			
			{ do user filter, if it exists }
			IF filterProc <> NIL THEN
			BEGIN
				dpNewSubscriberFilter := CallUserExpFilter(theDialog, theEvent, nsItemCount, itemHit, callBackPtr, filterProc); 
			END; {if}

		END; { dpNewSubscriberFilter }		
 

		{------------- dpNewSubscriberFileFilter -------------}
		FUNCTION dpNewSubscriberFileFilter(PB: CInfoPBPtr): BOOLEAN; 
		VAR
			EPB:		EditionOpenerParamBlock;
		BEGIN
			{ fill out opener reply record }
			WITH EPB.info, PB^ DO
			BEGIN
				{ allow all folders to be shown }
				IF bTST(ioFlAttrib, 4{ioDirFlg}) THEN 
				BEGIN
					dpNewSubscriberFileFilter := FALSE;
					EXIT(dpNewSubscriberFileFilter);
				END;
				crDate 		:= ioFlCrDat;
				mdDate 		:= ioFlMdDat;
				fdCreator	:= ioFlFndrInfo.fdCreator;
				fdType 		:= ioFlFndrInfo.fdType;
				container.theFile.vRefNum 	:= ioVRefNum;
				container.theFile.parID 	:= ioFlParID;
				BlockMove(Ptr(ioNamePtr), @container.theFile.name, LENGTH(ioNamePtr^)+1);
				container.thePart			:= kPartsNotUsed;
				EPB.success := bTST(ioFlFndrInfo.fdFlags, kHasMultipleFormatsBit);
			END; {with}
			EPB.formatsMask := reply.formatsMask;
			
			{ filter out files that can not be subscribed to }
			dpNewSubscriberFileFilter := (CallEditionOpenerProc(eoCanSubscribe, EPB, thisApp^^.emulator) <> noErr);
		END; { dpNewSubscriberFileFilter }		


	BEGIN { dp_NewSubscriberExpDialog }
	
		{ set up failure handling }
		IF isFailure(fi, dp_NewSubscriberExpDialog) THEN
		BEGIN
			reply.canceled := TRUE; { seems safe }
			EXIT(dp_NewSubscriberExpDialog);
		END; {if}

		FailOSErr(dpPreFlightPackage(stdFile));
		FailOSErr(dp_GetCurrentAppRefNum(thisApp));
		
		{$IFC qRangeCheck }
		{ sanity check for formats }
		IF (bAND(reply.formatsMask, -8) <> 0) | (bAND(reply.formatsMask, 7) = 0) THEN
		BEGIN
			DebugStr('You forgot to initialize reply.formatsMask');
			reply.formatsMask := kPICTformatMask + kTEXTformatMask;
		END;
		{$ENDC}
	
		dpGetGlobalsLocation^^.A6Link := GetA6;	{ save off my A6, for use by local routines }
		
		saveVol := pInteger(SFSaveDisk)^;
		saveDir := pLongint(CurDirStore)^;

		CustomGetFile(	@dpNewSubscriberFileFilter,	{ fileFilter }
						-1,						{ numTypes => all types }
						fileTypes, 				{ dummy list of types }
						getFileReply,			{ reply record }
						rNewSubscriberDialog,	{ dialog ID }
						where, 					{ used where passed in }
						@dpNewSubscriberHook,	{ dlgHook }
						@dpNewSubscriberFilter,	{ filterProc }
						NIL,					{ activeList }
						NIL,					{ activateProc }
						GetA6);					{ yourDataPtr }
		
		{ restore low mem globals }
		pInteger(SFSaveDisk)^  := saveVol;
		pLongint(CurDirStore)^ := saveDir;
		
		WITH reply DO
		BEGIN
			{ set return value for canceled }
			canceled := NOT getFileReply.sfGood;
			
			IF NOT canceled THEN 
			BEGIN
				container.theFileScript	:= getFileReply.sfScript;
				container.thePart 		:= kPartsNotUsed;
				BlockMove(@getFileReply.sfFile, @container.theFile, SizeOf(FSSpec)); { container.theFile := getFileReply.sfFile; }
			END; {if}
		END; {with}
		
		FailOSErr(dpPostFlightPackage(stdFile));
		
		Success(fi);
	END; { dp_NewSubscriberExpDialog }
	


{========================== NewPublisherDialog ============================}

TYPE
	PublisherGlobalsPtr = ^PublisherGlobalsRec;
	PublisherGlobalsRec = 
		RECORD
			putFileReply:		StandardFileReply;
			previewRect:		Rect;
			appsReplyRec: 		^NewPublisherReply;
			appsDialogFilter:	ExpModalFilterProcPtr;
			appsDialogHook: 	ExpDlgHookProcPtr; 
			appsDataPtr:		Ptr;
			appsDITLresID: 		INTEGER; 
			npItemCount:		INTEGER;
		END;


	{------------- dpNewPublisherUserItem -------------}
	PROCEDURE dpNewPublisherUserItem(theDialog: DialogPtr; itemNo: INTEGER); 
	BEGIN
		WITH PublisherGlobalsPtr(dpGetGlobalsLocation^^.A6Link)^ DO
		BEGIN
			{ _ModalDialog calls all user item's draw, even if outside the update region }
			{ so I will not draw unless really necessary }
			IF RectInRgn(previewRect, WindowPeek(theDialog)^.port.visRgn) THEN
			BEGIN
				{ clear background and draw frame }
				PenNormal;
				EraseRect(previewRect);
			
				{ draw contents }
				IF DrawPreview(appsReplyRec^.preview, appsReplyRec^.previewFormat, previewRect) <> noErr THEN;
			END; {if}
		END; {with}
	END; { dpNewPublisherUserItem }
	
	
	
	{------------- dpNewPublisherHook -------------}
	FUNCTION dpNewPublisherHook(SFitem: INTEGER; theDialog: DialogPtr; VAR globals: PublisherGlobalsRec): INTEGER;
	VAR
		itemContents:	Handle;	        	{needed for GetDItem}
		itemType:		INTEGER;	        {needed for GetDItem}
		itemRect:		Rect;		        {needed for GetDItem}
	BEGIN 
		WITH globals DO
		BEGIN
			{ pre-process first hook call }
			IF WindowPeek(theDialog)^.refcon = LONGINT(sfMainDialogRefCon) 
			 {&} THEN IF sfItem = sfHookFirstCall THEN
			BEGIN
				{ save off how many items are really in this dialog }
				npItemCount := CountDITL(theDialog); 
				
				IF appsDITLresID <> 0 
					THEN dpAppendDITL(theDialog, appsDITLresID, kNPExpansionUserItem)	{ append the app's items }
					ELSE HideDItem(theDialog, kNPPreviewBottomPictItem);				{ only show this is appending items }
			END; {if}
			
			{ give app chance to do hook call }
			IF appsDialogHook <> NIL 
				THEN sfItem := CallUserExpDlgHook(npItemCount, sfItem, theDialog, appsDataPtr, appsDialogHook);
		
			{ post-process first hook call }
			IF WindowPeek(theDialog)^.refcon = LONGINT(sfMainDialogRefCon) 
			 {&} THEN IF sfItem = sfHookFirstCall THEN
			BEGIN
				{ set up user item handler for preview }
				GetDItem(theDialog, kNPPreviewUserItem, itemType, itemContents, previewRect);
				SetDItem(theDialog, kNPPreviewUserItem, itemType, Handle(@dpNewPublisherUserItem),previewRect);
			END; {if}	
		END; {with}
		
		dpNewPublisherHook:= sfItem;  {!!!!very important !!!! We must pass SF's 'standard' item hits back to SF}
	END;  { dpNewPublisherHook }
	


	{------------- dpNewPublisherFilter -------------}
	FUNCTION dpNewPublisherFilter(theDialog: DialogPtr; VAR theEvent: EventRecord; VAR itemHit: INTEGER; VAR globals: PublisherGlobalsRec): BOOLEAN; 
	BEGIN
		WITH globals DO
		BEGIN
			{ do user filter, if it exists }
			IF appsDialogFilter <> NIL 
				THEN dpNewPublisherFilter := CallUserExpFilter(theDialog, theEvent, npItemCount, itemHit, appsDataPtr, appsDialogFilter)
				ELSE dpNewPublisherFilter := FALSE;		
		END; {with}			
	END; { dpNewPublisherFilter }



	{------------- dp_NewPublisherExpDialog -------------}
	FUNCTION dp_NewPublisherExpDialog(VAR reply: NewPublisherReply; where: Point; 
									extentionDITLresID: INTEGER; dlgHook: ExpDlgHookProcPtr; 
									filterProc:	ExpModalFilterProcPtr; callBackPtr: Ptr): OSErr;
	VAR
		globals:		PublisherGlobalsRec;
		saveVol:		INTEGER;
		saveDir:		LONGINT;
		fi:				FailInfo;
	BEGIN 
		{ set up failure handling }
		IF isFailure(fi, dp_NewPublisherExpDialog) THEN
		BEGIN
			reply.canceled := TRUE; { seems safe }
			EXIT(dp_NewPublisherExpDialog);
		END; {if}

		FailOSErr(dpPreFlightPackage(stdFile));
		
		WITH globals DO
		BEGIN
			appsReplyRec		:= @reply;
			appsDialogFilter	:= filterProc;
			appsDialogHook		:= dlgHook;
			appsDataPtr			:= callBackPtr;
			appsDITLresID		:= extentionDITLresID;
			dpGetGlobalsLocation^^.A6Link := ORD(@globals);
			
			saveVol := pInteger(SFSaveDisk)^;
			saveDir := pLongint(CurDirStore)^;
	
				pInteger(SFSaveDisk)^	:=  -1 * reply.container.theFile.vRefNum;
				pLongint(CurDirStore)^	:=  reply.container.theFile.parID;
				
				putFileReply.sfScript	:=	reply.container.theFileScript;  { ### yuck }
				
				{$PUSH } {$R- }	
				CustomPutFile(	'',									{ prompt, '' => use string in DITL }
								reply.container.theFile.name,		{ default name }
								putFileReply,						{ reply record }
								rNewPublisherDialog,				{ DITL resource ID }
								where, 								{ used where passed in }
								@dpNewPublisherHook,				{ dlgHook }
								@dpNewPublisherFilter,				{ Dialog Filter }
								NIL,								{ activeList }
								NIL,								{ activateProc }
								@globals);							{ yourDataPtr }
				{$POP }
	
			{ restore low mem globals }
			pInteger(SFSaveDisk)^  := saveVol;
			pLongint(CurDirStore)^ := saveDir;
			
			WITH reply DO
			BEGIN
				canceled := NOT putFileReply.sfGood;
				IF NOT canceled THEN 
				BEGIN
					replacing 					:= putFileReply.sfReplacing;
					container.theFileScript 	:= putFileReply.sfScript;
					container.thePart 			:= kPartsNotUsed;
					BlockMove(@putFileReply.sfFile, @container.theFile, SizeOf(FSSpec)); { container.theFile := putFileReply.sfFile; }
				END; {if}
			END; {with}
		END; {with}
		
		FailOSErr(dpPostFlightPackage(stdFile));
		
		Success(fi);
	END; { dp_NewPublisherExpDialog }


	
{========================== SectionOptionsDialog ============================}

TYPE
	OptionsGlobalsPtr = ^OptionsGlobalsRec;
	OptionsGlobalsRec = 
		RECORD
			mainOptionsDialog:		DialogRecord;
			subOptionsDialog:		DialogRecord;
			pathListBox:			Rect;
			pathListPoppedBox:		Rect;
			OKOutlineRect:			Rect;
			sectionModDateTimeBox:	Rect;
			editionModDateTimeBox:	Rect;
			sectionModDateTitleBox:	Rect;
			editionModDateTitleBox:	Rect;
			sectionModDateTime:		Str255;
			editionModDateTime:		Str255;
			sectionModDateTitle:	Str255;
			editionModDateTitle:	Str255;
			defaultStyle:			TextStyle;			{ only use font and size }
			infoStyle:				TextStyle;			{ only use font and size }
			sysStyle:				TextStyle;			{ only use font and size }
			curJust:				INTEGER;
			soItemCount:			INTEGER;
			thePubCB:				PubCBHandle;
			appsDialogFilter:		ExpModalFilterProcPtr;
			appsDialogHook:			ExpDlgHookProcPtr;
			appsReplyPtr:			^SectionOptionsReply;
			appsDataPtr:			Ptr;
			thePopUpMenu:			MenuHandle;
			aliasH:					AliasHandle;
			thePopUpMenuItemCount:	INTEGER;
			appsDITLresID:			INTEGER;
			appsWhere:				Point;
			lastKnownEdition:		TimeStamp;
			optionsResID:			INTEGER;
			changeToMode:			UpdateMode;
			needToBeep:				Boolean; 
		END;
		


	{------------- dpSectionOptionsSetUpdateMode -------------}
	PROCEDURE dpSectionOptionsSetUpdateMode(newMode: UpdateMode; VAR globals: OptionsGlobalsRec);
	VAR
	  mode0Btn:		INTEGER;
	  mode1Btn:		INTEGER;
	  itemType:		INTEGER;
	  itemToChange:	Handle;
	  itemBox:		Rect;
	BEGIN
		{ calculate the new radio button states }
		IF newMode = umAutomatic THEN
		BEGIN
			mode0Btn	:= kRadioButtonOn;
			mode1Btn	:= kRadioButtonOff;
		END ELSE
		BEGIN
			mode0Btn	:= kRadioButtonOff;
			mode1Btn	:= kRadioButtonOn;
		END;
		
		WITH globals DO
		BEGIN
			changeToMode	:= newMode;
			
			{ set the radio buttons }
			GetDItem(@mainOptionsDialog, kSOUpdateMode1Button, itemType,itemToChange,itemBox);
			SetCtlValue(controlHandle(itemToChange), mode1Btn);
			GetDItem(@mainOptionsDialog, kSOUpdateMode0Button, itemType,itemToChange,itemBox);
			SetCtlValue(controlHandle(itemToChange), mode0Btn);
			
			{ show the section times iff nemMode <> umAutomatic }
			IF newMode = umAutomatic THEN
			BEGIN
				HideDItem(@mainOptionsDialog, kSOSectionModDateTitleUserItem);
				HideDItem(@mainOptionsDialog, kSOSectionModDateTimeUserItem);
			END ELSE
			BEGIN
				ShowDItem(@mainOptionsDialog, kSOSectionModDateTitleUserItem);
				ShowDItem(@mainOptionsDialog, kSOSectionModDateTimeUserItem);
			END; {if}
			
			{ make sure EditionModDate time is draw or erased }
			(*
			InvalRect(editionModDateTitleBox);	
			InvalRect(editionModDateTimeBox);	
			*)
			InvalRect(sectionModDateTitleBox);	
			InvalRect(sectionModDateTimeBox);
		END; {with}
	END; { dpSectionOptionsSetUpdateMode }
	
	
	PROCEDURE CallMBARInline(message: INTEGER; theMenu: MenuHandle; VAR menuRect: Rect;
								hitPt: Point; VAR whichItem: INTEGER; routine: ProcPtr);
		INLINE	$205F,		{	MOVE.L 	(SP)+, A0	}
				$4E90;		{	JSR		(A0)		}

	
	{------------- CallMBAR -------------}
	PROCEDURE CallMBAR(message: INTEGER; theMenu: MenuHandle; VAR menuRect: Rect;
						hitPt: Point; VAR whichItem: INTEGER); 
	VAR
		theMDEFhandle:	Handle;
		savedHState:	SignedByte;
	BEGIN
		theMDEFhandle := theMenu^^.menuProc;
		
		IF theMDEFhandle^ = NIL
			THEN LoadResource(theMDEFhandle);
	
		IF theMDEFhandle^ <> NIL THEN
		BEGIN
			savedHState := HGetState(theMDEFhandle);
			HLock(theMDEFhandle);
			
				CallMBARInline(message, theMenu, menuRect, hitPt, whichItem, theMDEFhandle^);
			
			HSetState(theMDEFhandle, savedHState);
		END;
	END; { CallMBAR }
	
	
	{------------- dpDrawShadowRect -------------}
	PROCEDURE dpDrawShadowRect(theRect: Rect); 
	BEGIN
		{ draw box and shadow }
		PenNormal;
		InsetRect(theRect, -1, -1);
		EraseRect(theRect);
		FrameRect(theRect);
		WITH theRect DO
		BEGIN
			MoveTo(right, top+2);
			LineTo(right, bottom);
			LineTo(left+2, bottom);
		END; {with}
	END; { dpDrawShadowRect }
	
	
	{------------- dpDoPopUpPathMenu -------------}
	PROCEDURE dpDoPopUpPathMenu(VAR globals: OptionsGlobalsRec); 
	VAR
		whichItem:	INTEGER;
		bits:		SavedBits;
		saveRect:	Rect;
		itemBox:	Rect;
	BEGIN
		WITH globals DO
		BEGIN
			saveRect := pathListPoppedBox;
			InsetRect(saveRect, -2, -2);
			LocalToGlobal(saveRect.topLeft);
			LocalToGlobal(saveRect.botRight);
			
			IF SaveBits(saveRect, {purgeable}FALSE, bits) = noErr THEN;
			
			dpDrawShadowRect(pathListPoppedBox);
			(*
			whichItem := 1;
			SetOrigin(0, pathListPoppedBox.top + 4);
			CallMBAR(mDrawMsg, thePopUpMenu, pathListPoppedBox, Point(ORD4(0)), whichItem);	
			SetOrigin(0,0);
			*)
			
			itemBox := pathListBox;
			FOR whichItem := 1 TO thePopUpMenuItemCount DO
			BEGIN
				CallMBAR(mDrawItemMsg, thePopUpMenu, itemBox, Point(ORD4(0)), whichItem);	
				WITH itemBox DO
				BEGIN
					top := top + 17;
					bottom := bottom + 17;
				END; {with}
			END; {for}
			
			WHILE StillDown DO;

			IF RestoreBits(bits) <> noErr THEN;
			(*
			EraseRect(saveRect);
			InvalRect(saveRect);
			*)
		END; {with}
	END; { dpDoPopUpPathMenu }


	{------------- dpSectionOptionsDrawPathPopUp -------------}
	PROCEDURE dpSectionOptionsDrawPathPopUp(VAR globals: OptionsGlobalsRec); 
	VAR
		whichItem:			INTEGER;
		popUpSymbolPict:	PicHandle;
		symbolRect:			Rect;
		temp:				INTEGER;
	BEGIN
		WITH globals DO
		BEGIN
			dpDrawShadowRect(pathListBox);

			whichItem := 1;
			CallMBAR(mDrawItemMsg, thePopUpMenu, pathListBox, Point(ORD4(0)), whichItem);	
			
			popUpSymbolPict := PicHandle(GetResource('PICT',-8224));
			symbolRect := pathListBox;
			WITH symbolRect DO
			BEGIN
				temp := (top + bottom) DIV 2;
				top := temp - 3;
				bottom := temp + 3;
				right := right - 3;
				left := right - 12;
			END;
			DrawPicture(popUpSymbolPict, symbolRect);
			
		END; {with}
	END; { dpSectionOptionsDrawPathPopUp }

	
	
	{------------- dpSectionOptionsAppendDateTime -------------}
	PROCEDURE dpSectionOptionsAppendDateTime(edition: TimeStamp; VAR theString: Str255); 
	VAR
		tmp1,tmp2:	Str255;
		len1,len2:	LONGINT;
	BEGIN
		IUDateString(edition, longDate, tmp1);
		IUTimeString(edition, {wantsSeconds}TRUE, tmp2);
		len1 := LENGTH(tmp1);
		len2 := LENGTH(tmp2);
		BlockMove(@tmp1[1], @theString[1], len1);
		theString[len1+1] := ' ';	{### is this kosher?? }
		BlockMove(@tmp2[1], @theString[len1+2], len2);
		theString[0] := CHR( len1 + 1 + len2 );
		{*** same as:  theString := CONCAT(tmp1, ' ', tmp2);	***}
	END; { dpSectionOptionsAppendDateTime }
	
		
	
	{------------- dpSectionOptionsUserItem -------------}
	PROCEDURE dpSectionOptionsUserItem(theDialog: DialogPtr; itemNo: INTEGER);
	VAR
		globals: OptionsGlobalsPtr;
	BEGIN
		globals := OptionsGlobalsPtr(dpGetGlobalsLocation^^.A6Link);
		WITH globals^ DO
		BEGIN
			IF itemNo = kSOPopUpUserItem THEN
			BEGIN
				dpSectionOptionsDrawPathPopUp(globals^);
			END ELSE
			BEGIN
				TextFont(infoStyle.tsFont);
				TextSize(infoStyle.tsSize);
				CASE itemNo OF
					kSOSectionModDateTimeUserItem:
						BEGIN
							IF changeToMode <> umAutomatic 
								THEN TextBox(@sectionModDateTime[1], LENGTH(sectionModDateTime), sectionModDateTimeBox, curJust)
								ELSE EraseRect(sectionModDateTimeBox);
						END;
					kSOEditionModDateTimeUserItem:
						BEGIN
							{ get mod-date of edition, or "edition not found" }										 { <37> }
							IF (thePubCB = NIL) | (thePubCB^^.fileMissing) 											 { <37> }
								THEN GetIndString(editionModDateTime, optionsResID, kSOSTRdefaultEditionTimeIndex)	 { <37> }
								ELSE dpSectionOptionsAppendDateTime(thePubCB^^.info.mdDate, editionModDateTime);	 { <37> }
							TextBox(@editionModDateTime[1], LENGTH(editionModDateTime), editionModDateTimeBox, curJust);
						END;
					kSOSectionModDateTitleUserItem:
						BEGIN
							IF changeToMode <> umAutomatic
								THEN TextBox(@SectionModDateTitle[1], LENGTH(sectionModDateTitle), sectionModDateTitleBox, curJust)
								ELSE EraseRect(sectionModDateTitleBox);
						END;
					kSOEditionModDateTitleUserItem:
						BEGIN
							TextBox(@editionModDateTitle[1], LENGTH(editionModDateTitle), editionModDateTitleBox, curJust);
						END;
				END; {case}
				TextFont(defaultStyle.tsFont);
				TextSize(defaultStyle.tsSize);
			END; {if}
		END; {with}
	END; { dpSectionOptionsUserItem }
	
	
	{------------- dpSectionOptionsFlashButton -------------}
	PROCEDURE dpSectionOptionsFlashButton(theDialog: DialogPtr; whichButton: INTEGER); 
	VAR
		theControl:		ControlHandle;
		ignore:			LONGINT;
		itemType:		INTEGER;
		itemBox:		Rect;
	BEGIN
		GetDItem(theDialog, whichButton, itemType, Handle(theControl), itemBox);
		HiliteControl(theControl, kInvertButtonHilite);
		Delay(8, ignore);
		HiliteControl(theControl, kNormalButtonHilite);
	END; { dpSectionOptionsFlashButton }
	
					
	{------------- dpSectionOptionsBuildPathMenu -------------}
	PROCEDURE dpSectionOptionsBuildPathMenu(VAR globals: OptionsGlobalsRec); 
	VAR
		aFileSpec:		FSSpec;
		PBC:			CInfoPBRec;
		temp:			StringPtr;
		index:			AliasInfoType;
		aliasH:			AliasHandle;
		thePathMenu:	MenuHandle;
		curItem:		INTEGER;
		dimmed:			BOOLEAN;
		desktopDirID:	LONGINT;
		ignore:			INTEGER;

		PROCEDURE AddEntry(iconID: INTEGER; namePtr: StringPtr);
		BEGIN
			AppendMenu(thePathMenu, namePtr^);
			SetItem(thePathMenu, curItem, namePtr^);
			SetItemIcon(thePathMenu, curItem, iconID-genericIconBase);
			SetItemCmd(thePathMenu, curItem, CHAR($1A));
			IF dimmed 
				THEN DisableItem(thePathMenu, curItem)
				ELSE EnableItem(thePathMenu, curItem);
			curItem := curItem + 1;
		END; { AddEntry }
		
	BEGIN
		WITH globals DO
		BEGIN
			{ create menu }
			thePathMenu := NewMenu(-4000, ' '); { SetItem requires title to have length > 0 }
			InsertMenu(thePathMenu, -1);
			curItem := 1;
				
			IF thePubCB = NIL THEN
			BEGIN
				dimmed := TRUE;
				{ disable item, so help will use correct message }
				SetDItemEnable(@mainOptionsDialog, kSOPopUpUserItem, {enable}FALSE);
				
				{ use alias to build path list }
				index := asiAliasName;
				IF aliasH = NIL 
					THEN EXIT(dpSectionOptionsBuildPathMenu);
				
				{ walk up folders to volume, assume edition file }
				WHILE (GetAliasInfo(aliasH, index, aFileSpec.name)=noErr)
					& (LENGTH(aFileSpec.name) > 0) DO
				BEGIN
					IF index > asiAliasName 
						THEN AddEntry(openFolderIconResource, @aFileSpec.name)
						ELSE AddEntry(genericEditionFileIconResource, @aFileSpec.name);
					index := index + 1;
				END; {while}
				
				{ add entry for volume, assume hard disk }
				IF GetAliasInfo(aliasH, asiVolumeName, aFileSpec.name) = noErr 
					THEN AddEntry(genericHardDiskIconResource, @aFileSpec.name)
				
				{ fall into code that adds entry for desktop }
			END ELSE
			BEGIN
				dimmed := thePubCB^^.fileMissing;
				{ set up path list using FileSpec in control block }
				BlockMove(@thePubCB^^.info.container.theFile, @aFileSpec, SizeOf(FSSpec));
				
				{ get desktop folder, so we know when to stop }
				ignore := FindFolder(aFileSpec.vRefNum, kDesktopFolderType, kDontCreateFolder, ignore, deskTopDirID);
				
				{ do container file }
				IF IsEditionFile(thePubCB^^.info.fdType)
					THEN AddEntry(genericEditionFileIconResource, @aFileSpec.name)
					ELSE AddEntry(genericDocumentIconResource, @aFileSpec.name);
				
				{ walk up folders }
				PBC.ioNamePtr 	:= @aFileSpec.name;
				PBC.ioFDirIndex	:= -1;
				PBC.ioDrDirID 	:= aFileSpec.parID;
				PBC.ioVRefNum 	:= aFileSpec.vRefNum;
				REPEAT
					IF PBGetCatInfoSync(@PBC) <> noErr 
						THEN LEAVE; {repeat}
					IF PBC.ioDrParID = 1 THEN
					BEGIN
						{ do volume icon }
						AddEntry(genericHardDiskIconResource, @aFileSpec.name);
						LEAVE;
					END;
					IF PBC.ioDrDirID = deskTopDirID 
						THEN LEAVE;
					AddEntry(openFolderIconResource, @aFileSpec.name);
					PBC.ioDrDirID := PBC.ioDrParID;
				UNTIL FALSE;
			END; {if}
			
			{ do desktop icon }
			temp := @aFileSpec.name;
			GetIndString(temp^, rStandardFileStringsID, sfDesktopName);	{ snag name from Standard file }
			AddEntry(desktopIconResource, @aFileSpec.name);

			CalcMenuSize(thePathMenu);
			pathListPoppedBox := pathListBox;
			WITH pathListPoppedBox, thePathMenu^^ DO
			BEGIN
				bottom := top + menuHeight + 2;
				right := left + menuWidth + 2 + 16;
			END;
			pathListBox.right := pathListPoppedBox.right;
			thePopUpMenu := thePathMenu;
			thePopUpMenuItemCount := curItem - 1;  { <32> }
		END; {with}
		
	END; { dpSectionOptionsBuildPathMenu }
	
		
	
	{------------- dpSetUserDrawAndCacheRect -------------}
	PROCEDURE dpSetUserDrawAndCacheRect(theDialog: DialogPtr; itemNumber: INTEGER; VAR theRect: Rect);
	VAR
		itemToChange: 	Handle;	        	{needed for GetDItem}
		itemType:		INTEGER;	        {needed for GetDItem}
	BEGIN
		GetDItem(theDialog, itemNumber, itemType, itemToChange, theRect);
		SetDItem(theDialog, itemNumber, itemType, Handle(@dpSectionOptionsUserItem), theRect);
	END; { dpSetUserDrawAndCacheRect }
	

	FUNCTION dpSectionOptionsDoDialog(theDialog: DialogPtr; theDialogRefCon: ResType; 
										dialogID: INTEGER; VAR globals: OptionsGlobalsRec): INTEGER; 
		FORWARD;
		
		
	{------------- dpSectionOptionsErrorDialog -------------}
	PROCEDURE dpSectionOptionsErrorDialog(error: OSErr; errDialogID: INTEGER; 
											errDialogRefCon: ResType; VAR globals: OptionsGlobalsRec);
	VAR
		alertString:		Str255;
		stringIndex:		INTEGER;
	BEGIN
		IF error <> userCanceledErr THEN
		BEGIN
			{ know that goto will fail, so alert user now }
			stringIndex := kSOSTRUnknowErrIndex;
			IF error = nsvErr THEN stringIndex := kSOSTRVolumeNFErrorIndex ELSE
			IF error = fnfErr THEN stringIndex := kSOSTRFileNFErrorIndex;
			
			{ only add extra verbage if I understand the error code }
			GetIndString(alertString, rSubscriberOptionsDialog, stringIndex);
			{$PUSH} {$R-}
			ParamText(alertString, '', '', '');
			{$POP}
			{ put up the dialog and ignore the result }
			IF dpSectionOptionsDoDialog(@globals.subOptionsDialog, errDialogRefCon, errDialogID, globals)=0 THEN;
		END;
	END; { dpSectionOptionsErrorDialog }
	

	{------------- dpSectionOptionsDoAppsHook -------------}
	PROCEDURE dpSectionOptionsDoAppsHook(theDialog: DialogPtr; VAR itemHit: INTEGER; VAR globals: OptionsGlobalsRec);
	BEGIN
		WITH globals DO
		BEGIN
			IF appsDialogHook <> NIL THEN
			BEGIN
				itemHit := CallUserExpDlgHook(soItemCount, itemHit, theDialog, appsDataPtr, appsDialogHook);
			END;
		END; {with}
	END; { dpSectionOptionsDoAppsHook }
	
	

	{------------- dpSectionOptionsMainHook -------------}
	FUNCTION dpSectionOptionsMainHook(itemHit: INTEGER; VAR globals: OptionsGlobalsRec): BOOLEAN;
	VAR
		publisherSectionH:	SectionHandle;
		publisherApplication: AppRefNum;
		publisherDoc: 		FSSpec;
		theSectionID: 		LONGINT;
		alertString:		Str255;
		gotoFI:				FailInfo;		
		gotoErr:			OSErr;
		edition: 			EditionContainerSpec;
		editionWasCreated:	BOOLEAN;
		aliasWasChanged: 	BOOLEAN;
		connectErr:			OSErr;
		thisApp:			AppRefNum;
	BEGIN
		dpSectionOptionsMainHook := FALSE;
		WITH globals DO
		BEGIN
			{ map real buttons to pseudo-items }
			CASE itemHit OF
				kSOCancelSectionButton:
					itemHit := emHookCancelSection;
				kSOGotoPublisherButton:
					itemHit := emHookGotoPublisher;
				kSODoEditionButton:
					itemHit := emHookGetEditionNow;
				kSOUpdateMode1Button:
					itemHit := emHookManualUpdateMode;
				kSOUpdateMode0Button:
					itemHit := emHookAutoUpdateMode;
			END; {case}
			
			{ call apps hook }
			dpSectionOptionsDoAppsHook(@mainOptionsDialog, itemHit, globals);
	
			{ process item hit }
			CASE itemHit OF
				kSOOKButton,kSOCancelButton:
					dpSectionOptionsMainHook := TRUE;
				kSOPopUpUserItem:	
					dpDoPopUpPathMenu(globals);
				emHookCancelSection:
					BEGIN
						{dpSectionOptionsFlashButton(@mainOptionsDialog, kSOCancelSectionButton);}
						{ get warning string appropriate for this type of section }
						GetIndString(alertString, optionsResID, kSOSTRcancelWarningIndex);
						{$PUSH} {$R-}
						ParamText(alertString, '', '', '');
						{$POP}
						IF dpSectionOptionsDoDialog(@subOptionsDialog, emCancelSectionDialogRefCon, 
													rRemoveYesNoDialog, globals) = kRYNYesButton THEN
						BEGIN
							appsReplyPtr^.action := sectionCancelMsgID;
							dpSectionOptionsMainHook := TRUE;
						END;
					END;
				emHookGotoPublisher:
					BEGIN
						{dpSectionOptionsFlashButton(@mainOptionsDialog, kSOGotoPublisherButton);}
						{ preflight goto }
						{ need failure handler }
						IF NOT IsFailure(gotoFI, gotoErr) THEN
						BEGIN
							FailOSErr(dpFindPublisher(thePubCB, {canAskUser}TRUE, publisherSectionH, publisherApplication,
													publisherDoc, theSectionID));
							Success(gotoFI);
						END;
						IF gotoErr = noErr THEN
						BEGIN
							appsReplyPtr^.action := 'goto';
							dpSectionOptionsMainHook := TRUE;
						END 
						ELSE dpSectionOptionsErrorDialog(gotoErr, rOpenPublisherErrorDialog,
														emGotoPubErrDialogRefCon, globals);
					END;
				emHookGetEditionNow:
					BEGIN
						{dpSectionOptionsFlashButton(@mainOptionsDialog, kSODoEditionButton);}
						connectErr := noErr;

						IF thePubCB = NIL THEN
						BEGIN	
							{ if no control block, then try to get one }
							IgnoreOSErr(dp_GetCurrentAppRefNum(thisApp));
							connectErr := dpReconnectSection({sectionDoc}NIL, 
													appsReplyPtr^.sectionH, thisApp,  
													editionWasCreated, aliasWasChanged);
							{ ignore publisher warnings }
							IF (connectErr = notThePublisherWrn) | (connectErr = multiplePublisherWrn)
								THEN connectErr := noErr;
							{ tell app that something changed }
							IF aliasWasChanged 
								THEN appsReplyPtr^.changed := TRUE;
						END ELSE
						BEGIN
							{ try to make sure that control block is up to date }
							IgnoreOSErr(dpPubSync(thePubCB));
							
							{ if controlblock, but file is missing then tell user }
							IF thePubCB^^.fileMissing 
								THEN connectErr := fnfErr;
						END;
						
						IF connectErr <> noErr THEN
						BEGIN
							{ put up the error dialog and ignore the result }
							dpSectionOptionsErrorDialog(connectErr, rReconnectErrorDialog,
														emReconnectErrDialogRefCon, globals);
						END ELSE
						BEGIN
							{ return from dialog with right action code }
							WITH appsReplyPtr^ DO
							BEGIN
								IF bTST({appsReplyPtr^.}sectionH^^.kind, kCanReadEditionsBit) 
									THEN {appsReplyPtr^.}action := sectionReadMsgID
									ELSE {appsReplyPtr^.}action := sectionWriteMsgID;
							END; {with}
							dpSectionOptionsMainHook := TRUE;
						END; {if}
							
					END;
				emHookAutoUpdateMode: 
					IF changeToMode <> 0 
						THEN dpSectionOptionsSetUpdateMode(0, globals);
				emHookManualUpdateMode: 
					IF changeToMode <> 1 
						THEN dpSectionOptionsSetUpdateMode(1, globals);
				END;  {case}
			END; {with}

	END; { dpSectionOptionsMainHook }


	{------------- dpSectionOptionsSetUp -------------}
	PROCEDURE dpSectionOptionsSetUp(VAR globals: OptionsGlobalsRec);
	VAR
		itemToChange: 	Handle;	        	{needed for GetDItem}
		itemBox:		Rect;				{needed for GetDItem}
		itemType:		INTEGER;	        {needed for GetDItem}
		tempL:			LONGINT;
		orgWindowBottom:INTEGER;
		windowGrowSize:	INTEGER;
		ignore:			INTEGER;
		dummyHit:		INTEGER;
	BEGIN
		WITH globals DO
		BEGIN
			soItemCount := CountDITL(@mainOptionsDialog);
			
			{ add app's extra items }
			IF appsDITLresID <> 0 THEN
			BEGIN
				{ add app's items to dialog and calculate amount window grew }
				orgWindowBottom := WindowPeek(@mainOptionsDialog)^.port.portRect.bottom;
				dpAppendDITL(@mainOptionsDialog, appsDITLresID, kSOExpansionUserItem);
				windowGrowSize := WindowPeek(@mainOptionsDialog)^.port.portRect.bottom - orgWindowBottom;
				
				{ move the OK & Cancel buttons down to new bottom }
				MoveDItem(@mainOptionsDialog, kSOOKButton, windowGrowSize);
				MoveDItem(@mainOptionsDialog, kSOCancelButton, windowGrowSize);
				
				{ hide the dividing line, now that buttons have moved }
				HideDItem(@mainOptionsDialog, kSOCancelOKDividerPictItem);
			END; {if}

			{ move dialog }
			IF LONGINT(appsWhere) = $FFFFFFFF 
				THEN AutoPositionWindow(@mainOptionsDialog, lcMainScreen, hcCenter, vcAlertCenter)
				ELSE MoveWindow(@mainOptionsDialog, appsWhere.h, appsWhere.v, TRUE);
			
			{ give caller a chance to do initialization }
			dummyHit := sfHookFirstCall;
			dpSectionOptionsDoAppsHook(@mainOptionsDialog, dummyHit, globals);
						
			{ set up user item handler for path list }
			dpSetUserDrawAndCacheRect(@mainOptionsDialog, kSOPopUpUserItem, pathListBox);
			InsetRect(pathListBox, 1,1);  { drop shadow to fit inside of item }
			
			{ set up user item handler for edition time/date box }
			dpSetUserDrawAndCacheRect(@mainOptionsDialog, kSOSectionModDateTimeUserItem, sectionModDateTimeBox);
			dpSetUserDrawAndCacheRect(@mainOptionsDialog, kSOEditionModDateTimeUserItem, editionModDateTimeBox);
			dpSetUserDrawAndCacheRect(@mainOptionsDialog, kSOSectionModDateTitleUserItem, sectionModDateTitleBox);
			dpSetUserDrawAndCacheRect(@mainOptionsDialog, kSOEditionModDateTitleUserItem, editionModDateTitleBox);
			 									
			{ set correct radion button }
			dpSectionOptionsSetUpdateMode(appsReplyPtr^.sectionH^^.mode, globals);
			
			{ disable some stuff }
			IF (thePubCB = NIL) | (thePubCB^^.fileMissing) THEN
			BEGIN
				{ dim "Open Publisher" if edition not found }
				GetDItem(@mainOptionsDialog, kSOGotoPublisherButton, itemType, itemToChange, itemBox);
				HiliteControl(ControlHandle(itemToChange), kDimButtonHilite);
				
				{ also disable editions dates for correct help }
				SetDItemEnable(@mainOptionsDialog, kSOEditionModDateTitleUserItem, {enable}FALSE);
				SetDItemEnable(@mainOptionsDialog, kSOEditionModDateTimeUserItem, {enable}FALSE);
			END; {if}
			
			{ remember what the current font is }
			defaultStyle.tsFont := GrafPtr(@mainOptionsDialog)^.txFont;
			defaultStyle.tsSize := GrafPtr(@mainOptionsDialog)^.txSize;
			
			{ remember what the "small" font is }
			tempL := GetScript(FontScript, smScriptSmallFondSize);
			infoStyle.tsFont := HiWrd(tempL);
			infoStyle.tsSize := LoWrd(tempL);
		
			{ remember what the system font is}
			tempL := GetScript(FontScript, smScriptSysFondSize);
			sysStyle.tsFont := HiWrd(tempL);
			sysStyle.tsSize := LoWrd(tempL);
	
			{ remember how to justify in current script }
			curJust := GetSysJust;
	
			{ convert edition times to strings }
			dpSectionOptionsAppendDateTime(appsReplyPtr^.sectionH^^.mdDate, sectionModDateTime);
			IF thePubCB <> NIL 										{ <37> }
				THEN lastKnownEdition := thePubCB^^.info.mdDate;	{ <37> }
					
			{ get edition times titles }
			GetIndString(editionModDateTitle, optionsResID, kSOSTReditionModDateTitleIndex);
			GetIndString(sectionModDateTitle, optionsResID, kSOSTRSectionModDateTitleIndex);
	
			{ set up path pop up }
			dpSectionOptionsBuildPathMenu(globals);	
			
			{ force an arrow cursor }
			InitCursor;	
		END; {with}
	END; { dpSectionOptionsSetUp }
	

	{------------- dpSectionOptionsTakeDown -------------}
	PROCEDURE dpSectionOptionsTakeDown(VAR globals: OptionsGlobalsRec);
	VAR
		dummyHit:	INTEGER;
	BEGIN
		WITH globals DO
		BEGIN			
			{ remove pop up }
			DeleteMenu(-4000);
			DisposeMenu(thePopUpMenu);
			
			{ give caller a chance to do finalizing }
			dummyHit := sfHookLastCall;
			dpSectionOptionsDoAppsHook(@mainOptionsDialog, dummyHit, globals);
		END; {with}
	END; { dpSectionOptionsTakeDown }
	

	{------------- dpSectionOptionsDialogHook -------------}
	FUNCTION dpSectionOptionsDialogHook(theDialog: DialogPtr; itemHit: INTEGER): BOOLEAN;
	VAR
		globals: 	OptionsGlobalsPtr;
	BEGIN
		dpSectionOptionsDialogHook := FALSE;
		globals := OptionsGlobalsPtr(dpGetGlobalsLocation^^.A6Link);
		WITH globals^ DO
		BEGIN
			{ main dialog is split out into three cases }
			IF theDialog = @mainOptionsDialog THEN
			BEGIN
				IF itemHit = sfHookFirstCall THEN
				BEGIN
					dpSectionOptionsSetUp(globals^);
				END ELSE
				IF itemHit = sfHookLastCall THEN
				BEGIN
					dpSectionOptionsTakeDown(globals^);
				END ELSE
				BEGIN
					dpSectionOptionsDialogHook := dpSectionOptionsMainHook(itemHit, globals^);
				END;
			END ELSE
			BEGIN
				IF itemHit = sfHookFirstCall THEN
				BEGIN
					{ if it is the "cancel section warning" switch default item to the "no" button }
					IF WindowPeek(theDialog)^.refcon = LONGINT(emCancelSectionDialogRefCon)
						THEN IgnoreOSErr(SetDialogDefaultItem(theDialog, kRYNNoButton));

					{ if it is the "goto will fail warning" switch cancel item to the "OK" button }
					IF (WindowPeek(theDialog)^.refcon = LONGINT(emGoToPubErrDialogRefCon))
						| (WindowPeek(theDialog)^.refcon = LONGINT(emReconnectErrDialogRefCon))
						THEN IgnoreOSErr(SetDialogCancelItem(theDialog, kOPEOKButton));
						
					{ ### send deactivate event to main dialog }
				END ELSE
				IF itemHit = sfHookNullEvent THEN
				BEGIN
					IF needToBeep THEN
					BEGIN
						SysBeep(1);
						needToBeep := FALSE;
					END;
				END;
			
				{ give caller a chance to do hook }
				dpSectionOptionsDoAppsHook(theDialog, itemHit, globals^);

				{ other dialogs end if item 1 or 2 is hit }
				IF (itemHit = 1) OR (itemHit = 2) 
					THEN dpSectionOptionsDialogHook := TRUE;
			END; {else}
		END; {with}
	END; { dpSectionOptionsDialogHook }
	
	
	
	{------------- dpSectionOptionsDialogFilter -------------}
	FUNCTION dpSectionOptionsDialogFilter(theDialog: DialogPtr; VAR theEvent: EventRecord; VAR itemHit: INTEGER): BOOLEAN; 
	VAR
		globals: 	OptionsGlobalsPtr;
		localWhere:	Point;
	BEGIN
		{ assume it will not be mapped }
		dpSectionOptionsDialogFilter := FALSE;

		globals := OptionsGlobalsPtr(dpGetGlobalsLocation^^.A6Link);
		WITH globals^ DO
		BEGIN
			{ try app's filter }
			IF appsDialogFilter <> NIL THEN
			BEGIN
				{ call users filter proc }
				IF CallUserExpFilter(theDialog, theEvent, soItemCount, itemHit, appsDataPtr, appsDialogFilter) THEN
				BEGIN
					{ if he handled it then boogie on out }
					dpSectionOptionsDialogFilter := TRUE;
					EXIT(dpSectionOptionsDialogFilter);	
				END; {if}
			END; {if}
	
			{ try standard filter }
			IF StdFilterProc(theDialog, theEvent, itemHit) THEN
			BEGIN
				dpSectionOptionsDialogFilter := TRUE;
				EXIT(dpSectionOptionsDialogFilter);	
			END; {if}

			{ try my filtering }
			CASE theEvent.what OF
				nullEvent:
					BEGIN
						{ map this to a sfHookNullEvent }
						itemHit := sfHookNullEvent;
						dpSectionOptionsDialogFilter := TRUE;
						
						{ check if edition changed, and update displayed time }
						IF thePubCB <> NIL THEN
						BEGIN
							WITH thePubCB^^.info DO
							BEGIN
								IF lastKnownEdition <> mdDate THEN
								BEGIN
									lastKnownEdition := mdDate;
									InvalRect(editionModDateTimeBox);	
								END; {if}
							END; {with}
						END; {if}
					END;
				mouseDown:
					BEGIN
						{ we need to check for this item because it is disabled for balloon help to work }
						IF theDialog = @mainOptionsDialog THEN
						BEGIN
							{ only handle mouse down in pop up box }
							localWhere := theEvent.where;
							GlobalToLocal(localWhere);
							IF PtInRect(localWhere, globals^.pathListBox) THEN
							BEGIN
								itemHit := kSOPopUpUserItem;
								dpSectionOptionsDialogFilter := TRUE;
							END; {if}
						END; {if}
					END;
				updateEvt:
					BEGIN
						IF theEvent.message <> ORD(@mainOptionsDialog) 
						{&} THEN IF theEvent.message <> ORD(@subOptionsDialog) THEN
						BEGIN
							{ update event for another window, but filter did not handle it }
							{ so, let's map it to a null event }
							itemHit := sfHookNullEvent;
							dpSectionOptionsDialogFilter := TRUE;
						END; {if}
					END;
			END; {case}
		END; {with}
	END; { dpSectionOptionsDialogFilter }
	
	
	
	{------------- dpSectionOptionsDoDialog -------------}
	FUNCTION dpSectionOptionsDoDialog(theDialog: DialogPtr; theDialogRefCon: ResType; 
										dialogID: INTEGER; VAR globals: OptionsGlobalsRec): INTEGER;
	VAR
		savePort:	GrafPtr;
		itemHit:	INTEGER;
	BEGIN
		{ get the dialog window loaded, but not shown }
		theDialog := GetNewDialog(dialogID, Ptr(theDialog), Pointer(-1) );
		WindowPeek(theDialog)^.refcon := LONGINT(theDialogRefCon);
		GetPort(savePort);
		SetPort(theDialog);
		
		{ tell dialog mgr which items are default and cancel }
		{ this also works for subdialogs }
		IgnoreOSErr(SetDialogDefaultItem(theDialog, kSOOKButton));
		IgnoreOSErr(SetDialogCancelItem(theDialog, kSOCancelButton));
		
		{ do first hook then show window }
		IF dpSectionOptionsDialogHook(theDialog, sfHookFirstCall) THEN;
		ShowWindow(theDialog);
		
		{ the first hook call for a sub dialog, needs a beep just like alerts }
		IF theDialog = @globals.subOptionsDialog
			THEN globals.needToBeep := TRUE;
		
		{ keep calling modalDialog until dialogHook returns true }
		REPEAT
			ModalDialog(@dpSectionOptionsDialogFilter, itemHit);
		UNTIL dpSectionOptionsDialogHook(theDialog, itemHit);
		{ return item that caused end of dialog }
		dpSectionOptionsDoDialog := itemHit;
		
		{ hide window, do last hook, then dump the dialog }
		HideWindow(theDialog);
		IF dpSectionOptionsDialogHook(theDialog, sfHookLastCall) THEN;
		CloseDialog(theDialog);
		{### we may want to see if the dialog items have a color table and dispose of it }
		DisposHandle(DialogPeek(theDialog)^.items);
		
		SetPort(savePort);
	END; { dpSectionOptionsDoDialog }
	


	{------------- dp_SectionOptionsExpDialog -------------}
	FUNCTION dp_SectionOptionsExpDialog(VAR reply: SectionOptionsReply; where: Point; 
								extentionDITLresID: INTEGER; dlgHook: ExpDlgHookProcPtr; 
								filterProc:	ExpModalFilterProcPtr; callBackPtr: Ptr): OSErr;
	VAR
		globalsPtr:		OptionsGlobalsPtr;
		doneHit:		INTEGER;
		fi:				FailInfo;		
	BEGIN 
		DoNotPutInRegister(@globalsPtr);
		globalsPtr := NIL;
		
		{ set up failure handling }
		IF isFailure(fi, dp_SectionOptionsExpDialog) THEN
		BEGIN
			reply.canceled := TRUE;
			IF globalsPtr <> NIL THEN DisposePtr(Ptr(globalsPtr));
			EXIT(dp_SectionOptionsExpDialog);
		END; {if}
	
		globalsPtr := OptionsGlobalsPtr(NewPtr(SizeOf(OptionsGlobalsRec)));
		IF globalsPtr = NIL
			THEN FailOSErr(memFullErr);
		
		WITH globalsPtr^ DO 
		BEGIN
			reply.canceled	:= TRUE;
			reply.changed 	:= FALSE;
			reply.action 	:= '    ';
			appsDialogFilter:= filterProc;
			appsDialogHook	:= dlgHook;
			appsDataPtr		:= callBackPtr;
			appsReplyPtr	:= @reply;
			appsDITLresID	:= extentionDITLresID;
			appsWhere		:= where;
			needToBeep 		:= FALSE;
			
			WITH reply.sectionH^^ DO
			BEGIN
				thePubCB := PubCBHandle(controlBlock);
				aliasH := alias;
			END;
			
			{ try to make sure that control block is up to date }
			IF thePubCB <> NIL 
				THEN IgnoreOSErr(dpPubSync(thePubCB));
			
			dpGetGlobalsLocation^^.A6Link := ORD(globalsPtr);
			
			{ use correct dialog }
			IF reply.sectionH^^.kind = stSubscriber 
				THEN optionsResID := rSubscriberOptionsDialog
				ELSE optionsResID := rPublisherOptionsDialog;
			
			doneHit := dpSectionOptionsDoDialog(@mainOptionsDialog, emOptionsDialogRefCon, optionsResID, globalsPtr^);
			
			IF doneHit <> kSOCancelButton {### doneHit = kSOOKButton  ???} THEN
			BEGIN
				WITH reply.sectionH^^ DO
				BEGIN
					reply.canceled := FALSE;
					reply.changed := ({reply.sectionH^^.}mode <> changeToMode);
					{reply.sectionH^^.}mode := changeToMode;
					{ Be nice.  If a subscriber changed to automatic and it is out of date, send read event }
					IF reply.changed 
						{&} THEN IF {reply.sectionH^^.}kind = stSubscriber
						{&} THEN IF changeToMode = sumAutomatic
						{&} THEN IF thePubCB <> NIL
						{&} THEN IF {reply.sectionH^^.}mdDate <> thePubCB^^.info.mdDate
						THEN IF dp_PostSectionEvent(reply.sectionH, {currentApp}NIL, sectionReadMsgID)<>noErr THEN;
				END; {with}
			END; {if}
				
			Success(fi);
		END; {with}
		
		{ deallocate locals }
		DisposePtr(Ptr(globalsPtr));
	END; { dp_SectionOptionsExpDialog }
	