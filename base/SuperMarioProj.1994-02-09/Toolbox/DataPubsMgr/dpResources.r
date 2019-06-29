/*
	File:		dpResources.r

	Contains:	owned resources for the Edition Manager

	Written by:	Nick Kledzik

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM5>	11/12/92	kc		Remove shell variable path names. Add compile time conditional.
	   <SM4>	 11/2/92	kc		Add #if !NewBuildSystem around the inclusion of
									EditionMgr.PACK.rsrc. We will delete it code later.
		<31>	 6/28/92	DTY		Mark Edition Manager 'PACK' as purgeable.
		<30>	 4/28/92	DTY		Get rid of hasBalloonHelp conditional.
		<29>	 3/27/92	DTY		Dean gets too big for his britches and gets caught checking
									files in without making sure the system still builds.
		<28>	 3/27/92	DTY		Set resSysHeap bit on 'PACK' resource.
		<27>	 10/2/91	JSM		StandardFile3Types.r is now StandardFileTypes.r.
		<26>	 2/20/91	ngk		RLC,#82851: Fix balloon string for preview.
		<25>	 1/14/91	ngk		in remove yes/no dialog, move action button to right side
		<24>	12/14/90	ngk		<MM>Add error dialog if edition can not be found
		<23>	10/30/90	ngk		disable edition location pop up user item
		<22>	 8/27/90	ngk		Add 'dctb' resource to main dialogs to have CDialog's.
		<21>	  8/2/90	ngk		Combined sf and em hdlg's in NewPub and NewSub dialogs.
		<20>	 7/30/90	ngk		Update help strings to J. Trumbles latest.
	   								Add help balloons for NewPublisher and NewSubscriber.
									Include StandardFile3Types.r
		<19>	 7/24/90	ngk		Changed rGrayRectPictID to 11
		<18>	 7/24/90	RLC		Change explicit include of BalloonTypes.r to plain include
									(BalloonTypes.r is now part of {RIncludes}).
		<17>	  7/2/90	ngk		Added sub DLOG's for options dialogs.
		<16>	  6/7/90	DC		Fix up ALRT's and DLOG's for auto-positioning
		<15>	  6/1/90	ngk		Fix help items in options dialog.
		<14>	 5/31/90	ngk		Added STR for name of preference file. Designed new
	   								SectionOptionsDialog layout.
		<13>	 4/11/90	ngk		use new standard file dialog layouts
		<12>	 3/22/90	RLC		Update types to reflect the helpItem changes to 'DITL' type in
									Types.r
		<11>	 3/21/90	ngk		Rearranged NewXDialog items.
		 <9>	 3/10/90	ngk		Removed include of 'dlcd' code
		 <8>	 2/27/90	ngk		Add Balloon help resources 'hdlg' and 'STR#' for 'OptionsDialog.
		 <7>	 2/25/90	ngk		Added string for edition file mdDate if edition file can not be
									found.
		 <6>	  2/4/90	ngk		Touch up SectionOptions dialog
		 <5>	 1/22/90	ngk		Move volume icon in NewPublisherDialog to make room for border
									around file list. Removed comment boxes.
		 <4>	  1/6/90	ngk		include code resources
		 <3>	  1/6/90	ngk		rearange dialogs to not have a comments box
		 <2>	12/28/89	dba		include Types.r
	   <1.7>	11/13/89	ngk		Cleaned up SectionOptionDialog items
	  <•1.6>	 10/2/89	ngk		Updated to use "Edition" instead of "Publication"
	   <1.5>	 9/25/89	ngk		New resources for imporved SectionInfoDialog
	   <1.4>	 8/29/89	ngk		Shifted around PublishAs and SubscribeTo to fit better on small
									screens.
	   <1.3>	  8/8/89	ngk		Added preview and comment fields to PublishAs & SubscribeTo
	   <1.2>	 6/15/89	ngk		Added rUnsavedPublishers alert
	   <1.1>	 5/29/89	ngk		Added SectionInfoDialog and Publisher Update mode to
									PublishAsDialog
	   <1.0>	 5/19/89	ngk		Submitted for first time

*/

#if !NewBuildSystem
include $$Shell("ObjDir")"EditionMgr.PACK.rsrc" 'PACK' (11) as 'PACK' (11, sysHeap, purgeable);
#endif

#include "Types.r"
#if NewBuildSystem
#include "StandardFileTypes.r"
#else
#include $$Shell("StandardFile")"StandardFileTypes.r"
#endif

#define PackNumber			11
#define ownedByPackage		0xE800 
#define ownedResourceBase  (ownedByPackage + (PackNumber << 5))

// DLOG's
#define rNewSubscriberDialog		(ownedResourceBase + 0)		/* -5792 */
#define rNewPublisherDialog			(ownedResourceBase + 1)		/* -5791 */
#define rSubscriberOptionsDialog	(ownedResourceBase + 2)		/* -5790 */
#define rPublisherOptionsDialog		(ownedResourceBase + 3)		/* -5789 */
#define rRemoveYesNoDialog			(ownedResourceBase + 4)		/* -5788 */
#define rOpenPublisherErrorDialog	(ownedResourceBase + 5)		/* -5787 */
#define rReconnectErrorDialog		(ownedResourceBase + 6)		/* -5786 */


// STR#'s
#define rNewSubscriberHelpStrings		(ownedResourceBase + 0)		/* -5792 */
#define rNewPublisherHelpStrings		(ownedResourceBase + 1)		/* -5791 */
#define rSubscriberOptionsStrings		(ownedResourceBase + 2)		/* -5790 */
#define rPublisherOptionsStrings		(ownedResourceBase + 3)		/* -5789 */
#define rSubscriberOptionsHelpStrings	(ownedResourceBase + 4)		/* -5788 */
#define rPublisherOptionsHelpStrings	(ownedResourceBase + 5)		/* -5787 */


// STR's
#define rLastEditionUsedString			(ownedResourceBase + 6)		/* -5786 */


// PICT's 
#define rBlackDoubleLinePictID			(ownedResourceBase + 2)		/* -5790 */
#define rUpdateClusterBorderPictID		(ownedResourceBase + 3)		/* -5789 */


// PICT's for visual frosting of options dialogs
resource 'PICT' (rUpdateClusterBorderPictID, purgeable) {
	42,
	{0, 0, 92, 306},
	$"1101 0100 0A00 0000 0000 5C01 3209 AA55"
	$"AA55 AA55 AA55 3000 0000 0000 5C01 32FF"
};

resource 'PICT' (rBlackDoubleLinePictID, purgeable) {
	38,
	{0, 0, 3, 16},
	$"1101 0100 0A00 0000 0000 0300 1022 0000"
	$"0000 1000 2200 0200 0010 00FF"
};


// name of preferences file that holds alias to last edition used
resource 'STR ' (rLastEditionUsedString, purgeable) { 	
	"Last Edition Used"
};



// NewSubscriberDialog
resource 'DLOG' (rNewSubscriberDialog, purgeable) 
	{ {0, 0, 166, 494}, dBoxProc, invisible, noGoAway, 0, rNewSubscriberDialog, "", noAutoCenter };
resource 'dctb' (rNewSubscriberDialog, purgeable)
	{ {} };		
resource 'DITL' (rNewSubscriberDialog,  purgeable) { {
		{135, 402, 155, 482}, Button { enabled, "Subscribe" },
		{104, 402, 124, 482}, Button { enabled, "Cancel" },
		{0,0,0,0}, HelpItem { disabled, HMScanhdlg {rNewSubscriberDialog} },	
		{8, 385, 24, 487}, UserItem { enabled },
		{32, 402, 52, 482}, Button { enabled, "Eject" },
		{60, 402, 80, 482}, Button { enabled, "Desktop" },
		{29, 162, 159, 380}, UserItem { enabled },
		{6, 162, 25, 380}, UserItem { enabled },
		{91, 401, 92, 483}, Picture { disabled, rGrayRectPictID },
		{39, 14, 159, 134}, UserItem { disabled },
		{8, 42, 24, 110}, StaticText { disabled, "Preview" },
		{6, 150, 160, 151}, Picture { enabled, rGrayRectPictID },
		{160, 6, 161, 150}, Picture { enabled, rGrayRectPictID },
		{161, 8, 162, 486}, UserItem { enabled },
} };


// NewPublisherDialog
resource 'DLOG' (rNewPublisherDialog, purgeable) 
	{ {0, 0, 188, 494}, dBoxProc, invisible, noGoAway, 0, rNewPublisherDialog, "", noAutoCenter };
resource 'dctb' (rNewPublisherDialog, purgeable)
	{ {} };		
resource 'DITL' (rNewPublisherDialog,  purgeable) { {
		{161, 402, 181, 482}, Button { enabled, "Publish" },
		{130, 402, 150, 482}, Button { enabled, "Cancel" },
		{0,0,0,0}, HelpItem { disabled, HMScanhdlg {rNewPublisherDialog}},		
		{8, 385, 24, 485}, UserItem { enabled },
		{32, 402, 52, 482}, Button { enabled, "Eject" },
		{60, 402, 80, 482}, Button { enabled, "Desktop" },
		{29, 162, 127, 380}, UserItem { enabled },
		{6, 162, 25, 380}, UserItem { enabled },
		{117, 401, 118, 483}, Picture { disabled, rGrayRectPictID },
		{157, 165, 173, 377}, EditText { enabled, "" },
		{136, 165, 152, 377}, StaticText { disabled, "Name of new edition:" },
		{87, 402, 107, 482}, UserItem { disabled },
		{45, 14, 165, 134}, UserItem { disabled },
		{8, 42, 24, 110}, StaticText { disabled, "Preview" },
		{6, 150, 182, 151}, Picture { disabled, rGrayRectPictID },
		{182, 6, 183, 150}, Picture { disabled, rGrayRectPictID },
		{183, 8, 184, 486}, UserItem { disabled },
} };


// SubscriberOptionsDialog
resource 'DLOG' (rSubscriberOptionsDialog, purgeable) 
	{ {40, 40, 195, 517}, dBoxProc, invisible, noGoAway, 0, rSubscriberOptionsDialog, "", noAutoCenter };
resource 'dctb' (rSubscriberOptionsDialog, purgeable)
	{ {} };		
resource 'DITL' (rSubscriberOptionsDialog, sysheap, purgeable) { {
		{124, 405, 144, 465}, Button { enabled, "OK" },
		{124, 330, 144, 390}, Button { enabled, "Cancel" },
		{120, 401, 148, 469}, UserItem { disabled },
		{54, 330, 74, 466}, Button { enabled, "Cancel Subscriber" },
		{83, 330, 103, 466}, Button { enabled, "Open Publisher" },
		{83, 160, 103, 296}, Button { enabled, "Get Edition Now" },
		{84, 35, 102, 124}, RadioButton { enabled, "Manually" },
		{65, 35, 83, 153}, RadioButton { enabled, "Automatically" },
		{10, 8, 26, 104}, StaticText { disabled, "Subscriber to:" },
		{8, 110, 28, 280}, UserItem { disabled },
		{35, 6, 38, 469}, Picture { disabled, rBlackDoubleLinePictID },
		{54, 11, 144, 310}, Picture { disabled, rUpdateClusterBorderPictID },
		{47, 18, 64, 104}, StaticText { enabled, "Get Editions:" },
		{110, 17, 122, 101}, UserItem { enabled },
		{125, 17, 137, 101}, UserItem { enabled },
		{110, 106, 122, 305}, UserItem { enabled },
		{125, 106, 137, 305}, UserItem { enabled },
		{112, 329, 113, 467}, Picture { disabled, rGrayRectPictID },
		{0,0,0,0}, HelpItem { enabled, HMScanhdlg {rSubscriberOptionsDialog} },
		{150, 8, 151, 469}, UserItem { disabled },
} };
resource 'STR#' (rSubscriberOptionsStrings, purgeable) { {	
	/* [1] */	"Latest Edition: ",
	/* [2] */	"Last Received: ",
	/* [3] */	"Are you sure you want to remove this subscriber?",
	/* [4] */	"Edition can not be found.",
	/* [5] */	", because the volume it is on could not be mounted",
	/* [6] */	", because it is missing",
	/* [7] */	"",
} };


// PublisherOptionsDialog
resource 'DLOG' (rPublisherOptionsDialog, purgeable) 
	{ {40, 40, 195, 517}, dBoxProc, invisible, noGoAway, 0, rPublisherOptionsDialog, "", noAutoCenter };
resource 'dctb' (rPublisherOptionsDialog, purgeable)
	{ {} };		
resource 'DITL' (rPublisherOptionsDialog, purgeable) { {
		{124, 405, 144, 465}, Button { enabled, "OK" },
		{124, 330, 144, 390}, Button { enabled, "Cancel" },
		{120, 401, 148, 469}, UserItem { disabled },
		{54, 330, 74, 466}, Button { enabled, "Cancel Publisher" },
		{83, 1330, 103, 1466}, Button { enabled, "Hidden Button" },
		{83, 160, 103, 296}, Button { enabled, "Send Edition Now" },
		{84, 35, 102, 124}, RadioButton { enabled, "Manually" },
		{65, 35, 83, 153}, RadioButton { enabled, "On Save" },
		{10, 8, 26, 104}, StaticText { disabled, "Publisher to:" },
		{8, 110, 28, 280}, UserItem { disabled },
		{35, 6, 38, 469}, Picture { disabled, rBlackDoubleLinePictID },
		{54, 11, 144, 310}, Picture { disabled, rUpdateClusterBorderPictID },
		{46, 18, 63, 112}, StaticText { enabled, "Send Editions:" },
		{110, 17, 122, 101}, UserItem { enabled },
		{125, 17, 137, 101}, UserItem { enabled },
		{110, 106, 122, 305}, UserItem { enabled },
		{125, 106, 137, 305}, UserItem { enabled },
		{112, 329, 113, 467}, Picture { disabled, rGrayRectPictID },
		{0,0,0,0}, HelpItem { enabled, HMScanhdlg {rPublisherOptionsDialog} },
		{150, 8, 151, 469}, UserItem { disabled },
} };
resource 'STR#' (rPublisherOptionsStrings, purgeable) { {	
	/* [1] */	"Latest Edition: ",
	/* [2] */	"Last Change: ",
	/* [3] */	"Are you sure you want to remove this publisher?",
	/* [4] */	"Edition can not be found.",
} };



// “Remove Section” dialog: keep bounds within main dialog to allow updates
resource 'DLOG' (rRemoveYesNoDialog, purgeable)
	{ {0, 0, 80, 200}, dBoxProc, invisible, noGoAway, 0, rRemoveYesNoDialog, "", centerParentWindow };
resource 'DITL' (rRemoveYesNoDialog, purgeable)
	{ {
	{50, 120, 70, 190}, Button { enabled, "Yes" };
	{50, 10, 70, 80}, Button { enabled, "No" };
	{10, 10, 40, 190}, StaticText { disabled, "^0" };
	} };



// “Open Publisher Error” dialog: keep bounds within main dialog to allow updates
resource 'DLOG' (rOpenPublisherErrorDialog, purgeable)
	{ {0, 0, 120, 290}, dBoxProc, invisible, noGoAway, 0, rOpenPublisherErrorDialog, "", centerParentWindow };
resource 'DITL' (rOpenPublisherErrorDialog, purgeable)
	{ {
	{87, 199, 107, 269}, Button { enabled, "OK" };
	{10, 78, 74, 277}, StaticText { disabled, "Could not open the publisher document^0." };
	{13, 23, 45, 55}, Icon { disabled, 1 };
	} };


// “Reconnect Error” dialog: keep bounds within main dialog to allow updates
resource 'DLOG' (rReconnectErrorDialog, purgeable)
	{ {0, 0, 110, 280}, dBoxProc, invisible, noGoAway, 0, rReconnectErrorDialog, "", centerParentWindow };
resource 'DITL' (rReconnectErrorDialog, purgeable)
	{ {
	{77, 197, 97, 267}, Button { enabled, "OK" };
	{13, 78, 64, 267}, StaticText { disabled, "Could not locate the edition^0." };
	{13, 23, 45, 55}, Icon { disabled, 1 };
	} };




#include "BalloonTypes.r"

// indexes into help STR#

// for New Subscriber
#define hsinsPreview 				1	
#define hsinsPreviewDimmed 			2	
#define hsinsFileList 				3	
#define hsinsCancel 				4	
#define hsinsSubscribe 				5	
#define hsinsSubscribeDimmed 		6	

// for New Publisher 
#define hsinpPreview 				1	
#define hsinpEditionNameTE 			2	
#define hsinpCancel 				3	
#define hsinpPublish 				4	
#define hsinpPublishDimmed 			5	

// for Subscriber Options
#define hsisoPopUpMenu 				1	
#define hsisoPopUpMenuDimmed 		2	
#define hsisoCancelSubscriber		3	
#define hsisoOpenPublisher	 		4	
#define hsisoOpenPublisherDimmed	5	
#define hsisoGetEditionNow			6	
#define hsisoGetEditionNowDimmed	7	
#define hsisoAutomatically			8	
#define hsisoAutomaticallyChecked	9	
#define hsisoManually				10	
#define hsisoManuallyChecked		11	
#define hsisoLatestEdition			12	
#define hsisoLastReceived			13	
#define hsisoLatestEditionDimmed	14	
#define hsisoCancel					15	
#define hsisoOK						16	

// for Publisher Options
#define hsipoPopUpMenu 				1	
#define hsipoPopUpMenuDimmed 		2	
#define hsipoCancelPublisher		3	
#define hsipoSendEditionNow			4	
#define hsipoSendEditionNowDimmed	5	
#define hsipoOnSave					6	
#define hsipoOnSaveChecked			7	
#define hsipoManually				8	
#define hsipoManuallyChecked		9	
#define hsipoLatestEdition			10	
#define hsipoLatestEditionDimmed	11	
#define hsipoLastChange				12	



resource 'hdlg' (rNewSubscriberDialog, purgeable) {
	HelpMgrVersion,							/* Help Version */
	0,										/* offset to first DITL */
	0,										/* options */
	0,										/* theProc */
	0,										/* variant */
	HMSkipItem {	/* skip missing message */
	},
	{
		HMStringResItem {	/* Open/Subscribe button */
			{0,0},
			{0,0,0,0},
			rNewSubscriberHelpStrings,hsinsSubscribe,		/* Enabled button */
			rNewSubscriberHelpStrings,hsinsSubscribeDimmed,	/* Disabled */
			rStandardFileHelpStringsID,hsisfOpenFolder,
			rStandardFileHelpStringsID,hsisfOpenFolderDimmed
		},
		HMStringResItem {	/* Cancel button */
			{0,0},
			{0,0,0,0},
			rNewSubscriberHelpStrings,hsinsCancel,			/* Enabled button */
			rStandardFileHelpStringsID,0,						/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMSkipItem {		/* help on help?? */
		},
		HMStringResItem {	/* Current volume User Item */
			{0,0},
			{0,0,0,0},
			rStandardFileHelpStringsID,hsisfVolume,				/* Enabled item */
			rStandardFileHelpStringsID,0,						/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMStringResItem {	/* Eject button */
			{0,0},
			{0,0,0,0},
			rStandardFileHelpStringsID,hsisfEject,				/* Enabled item */
			rStandardFileHelpStringsID,hsisfEjectDImmed,		/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMStringResItem {	/* Desktop button */
			{0,0},
			{0,0,0,0},
			rStandardFileHelpStringsID,hsisfDesktop,			/* Enabled item */
			rStandardFileHelpStringsID,hsisfDesktopDimmed,		/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMStringResItem {	/* list of files item */
			{0,0},
			{0,0,0,0},
			rNewSubscriberHelpStrings,hsinsFileList,			/* Enabled item */
			rStandardFileHelpStringsID,0,						/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMSkipItem {		/* pop up menu */
		},
		HMSkipItem {		/* divider line */
		},
		HMStringResItem {	/* preview */
			{0,0},
			{0,0,0,0},
			rNewSubscriberHelpStrings,hsinsPreview,			/* Enabled item */
			rNewSubscriberHelpStrings,hsinsPreviewDimmed,	/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
	}
};

resource 'STR#' (rNewSubscriberHelpStrings, purgeable) {
  {	
  	/* [hsinsPreview] Subscribe to dialog, Preview item, Normal */
		"This area shows you the contents of the edition selected at right.";

	/* [hsinsPreviewDimmed] Subscribe to dialog, Preview item, Dimmed */
		"This area shows you the contents of the edition selected at right.";  				/* 26, #82851 */
		 /* When code is fixed to enable/disable preview userItem then the following 	*/	/* 26, #82851 */
		 /* string should be appended "It is empty because no edition file is selected" */	/* 26, #82851 */
		
	/* [hsinsFileList] Subscribe to dialog, directory box, Normal */
		"This window lists the contents of the folder, disk, or file server named "
		"above.  Double-click a folder or disk to see its contents in this window. "
		"Double-click an edition to subscribe to it.";

	/* [hsinsCancel] Subscribe to dialog, cancel button, Normal */
		"To close this dialog box without subscribing to an edition, click this "
		"button.";
		
	/* [hsinsSubscribe] Subscribe to dialog, subscribe button, Normal */
		"To subscribe to the selected edition, click this button.  The material "
		"stored in the edition (shown in the preview area) is inserted into your "
		"document.  This material is automatically updated when the edition is "
		"updated by a publisher.";		
		
	/* [hsinsSubscribeDimmed] Subscribe to dialog, subscribe button, Dimmed */
		"To subscribe to the selected edition, click this button. The button "
		"is not available because nothing is selected.";
	}
};


resource 'hdlg' (rNewPublisherDialog, purgeable) {
	HelpMgrVersion,							/* Help Version */
	0,										/* offset to first DITL */
	0,										/* options */
	0,										/* theProc */
	0,										/* variant */
	HMSkipItem {	/* skip missing message */
	},
	{
		HMStringResItem {	/* Open/Publish button */
			{0,0},
			{0,0,0,0},
			rNewPublisherHelpStrings,hsinpPublish,			/* Enabled button */
			rNewPublisherHelpStrings,hsinpPublishDimmed,		/* Disabled */
			rStandardFileHelpStringsID,hsisfOpenFolder,
			rStandardFileHelpStringsID,hsisfOpenFolderDimmed
		},
		HMStringResItem {	/* Cancel button */
			{0,0},
			{0,0,0,0},
			rNewPublisherHelpStrings,hsinpCancel,			/* Enabled button */
			rStandardFileHelpStringsID,0,						/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMSkipItem {		/* help on help?? */
		},
		HMStringResItem {	/* Current volume User Item */
			{0,0},
			{0,0,0,0},
			rStandardFileHelpStringsID,hsisfVolume,				/* Enabled item */
			rStandardFileHelpStringsID,0,						/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMStringResItem {	/* Eject button */
			{0,0},
			{0,0,0,0},
			rStandardFileHelpStringsID,hsisfEject,				/* Enabled item */
			rStandardFileHelpStringsID,hsisfEjectDImmed,		/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMStringResItem {	/* Desktop button */
			{0,0},
			{0,0,0,0},
			rStandardFileHelpStringsID,hsisfDesktop,			/* Enabled item */
			rStandardFileHelpStringsID,hsisfDesktopDimmed,		/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMStringResItem {	/* list of files item */
			{0,0},
			{0,0,0,0},
			rStandardFileHelpStringsID,hsisfFileList,			/* Enabled item */
			rStandardFileHelpStringsID,0,						/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMSkipItem {		/* no help on pop up menu */
		},
		HMSkipItem {		/* no help on divider line */
		},
		HMStringResItem {	/* TextEdit for edition name */
			{0,0},
			{0,0,0,0},
			rNewPublisherHelpStrings,hsinpEditionNameTE,		/* Enabled item */
			rStandardFileHelpStringsID,0,						/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMSkipItem {		/* prompt */
		},
		HMStringResItem {	/* new folder button */
			{0,0},
			{0,0,0,0},
			rStandardFileHelpStringsID,hsisfNewFolder,			/* Enabled item */
			rStandardFileHelpStringsID,hsisfNewFolderDimmed,	/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
		HMStringResItem {	/* preview */
			{0,0},
			{0,0,0,0},
			rNewPublisherHelpStrings,hsinpPreview,			/* Enabled item */
			rNewPublisherHelpStrings,hsinpPreview,			/* Disabled */
			rStandardFileHelpStringsID,0,
			rStandardFileHelpStringsID,0
		},
	}
};


resource 'STR#' (rNewPublisherHelpStrings, purgeable) {
  {	
  	/* [hsinpPreview] Create Publisher, Preview item, Normal */
		"This area shows you the material that will be published (the material that "
		"is selected in your document).";
		
	/* [hsinpEditionNameTE] Create Publisher, name of edition box, Normal */
		"Type a name for the edition in this box.  The edition is a file that "
		"stores the material being published.";

	/* [hsinpCancel] Subscribe to dialog, cancel button, Normal */
		"To close this dialog box without publishing the selected material, click "
		"this button.";
		
	/* [hsinpPublish] Create Publisher, Publish button, Normal */
		"To create an edition with the name shown, click this button.  The selected "
		"material from your document (shown in the preview area) will be stored in "
		"the edition file.";
		
	/* [hsinpPublishDimmed] Create Publisher, Publish button, Dimmed */
		"To create an edition with the name shown, click this button.  The button "
		"is not available because no name has been provided for the edition.";
	}
};


resource 'hdlg' (rSubscriberOptionsDialog, purgeable) {
	HelpMgrVersion,							/* Help Version */
	0,										/* offset to first DITL */
	0,										/* options */
	0,										/* theProc */
	0,										/* variant */
	HMSkipItem {	/* skip missing message */
	},
	{
		HMStringResItem {	/* OK button */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoOK,					/* Enabled item */
			rSubscriberOptionsHelpStrings,0,						/* Disabled */
			rSubscriberOptionsHelpStrings,0,
			rSubscriberOptionsHelpStrings,0
		},
		HMStringResItem {	/* Cancel button */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoCancel,				/* Enabled item */
			rSubscriberOptionsHelpStrings,0,						/* Disabled */
			rSubscriberOptionsHelpStrings,0,
			rSubscriberOptionsHelpStrings,0
		},
		HMSkipItem {		/* Outline around the OK button */
		},
		HMStringResItem {	/* Cancel Subscriber button */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoCancelSubscriber,	/* Enabled button */
			rSubscriberOptionsHelpStrings,0,
			rSubscriberOptionsHelpStrings,0,
			rSubscriberOptionsHelpStrings,0
		},
		HMStringResItem {	/* Open Publisher button */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoOpenPublisher,			/* Enabled button */
			rSubscriberOptionsHelpStrings,hsisoOpenPublisherDimmed,		/* Disabled */
			rSubscriberOptionsHelpStrings,0,
			rSubscriberOptionsHelpStrings,0
		},
		HMStringResItem {	/* Get Edition Now button */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoGetEditionNow,			/* Enabled button */
			rSubscriberOptionsHelpStrings,hsisoGetEditionNowDimmed,		/* Disabled */
			rSubscriberOptionsHelpStrings,0,
			rSubscriberOptionsHelpStrings,0
		},
		HMStringResItem {	/* Manually button */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoManually,				/* Enabled, Unchecked item */
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,hsisoManuallyChecked,			/* Checked */
			rSubscriberOptionsHelpStrings,0
		},
		HMStringResItem {	/* Automatically button */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoAutomatically,			/* Enabled, Unchecked item */
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,hsisoAutomaticallyChecked,	/* Checked */
			rSubscriberOptionsHelpStrings,0
		},
		HMSkipItem {		/* main title text */
		},
		HMStringResItem {	/* pop up path item */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoPopUpMenu,				/* Enabled item */
			rSubscriberOptionsHelpStrings,hsisoPopUpMenuDimmed,			/* Disabled */
			rSubscriberOptionsHelpStrings,0,
			rSubscriberOptionsHelpStrings,0
		},
		HMSkipItem {		/* double line pict */
		},
		HMSkipItem {		/* cluster border pict */
		},
		HMSkipItem {		/* cluster border title */
		},
		HMStringResItem {	/* Latest Edition: text */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoLatestEdition,			/* Enabled */
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,0
		},
		HMStringResItem {	/* Latest Received: text */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoLastReceived,			/* Enabled */
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,0
		},
		HMStringResItem {	/* Latest Edition date: text */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoLatestEdition,			/* Enabled */
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,0
		},
		HMStringResItem {	/* Latest Received Date: text */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoLastReceived,			/* Enabled */
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,0,						
			rSubscriberOptionsHelpStrings,0
		},
	}
};


resource 'STR#' (rSubscriberOptionsHelpStrings, purgeable) {
  {	/* [hsisoPopUpMenu] subscriber options, path box, Normal */
		"Use this pop-up menu to see the path to the edition for the "
		"selected subscriber.";

	/* [hsisoPopUpMenuDimmed] subscriber options, path box, Dimmed */
		"Use this pop-up menu to see the path to the edition for the "
		"selected subscriber.  The items in the menu are dimmed because the edition "
		"cannot be found (it has been deleted, or the disk it is on is "
		"unavailable).";

	/* [hsisoCancelSubscriber] Subscriber options, Cancel subscriber button, Normal */
		"To stop subscribing to the edition named in the pop-up menu, click this "
		"button.  The selected material remains in your document, but it is not "
		"automatically updated when the edition is updated.";

	/* [hsisoOpenPublisher] Subscriber options, Find publisher button, Normal */
		"To open the document that contains the publisher for the selected "
		"subscriber, click this button.";

	/* [hsisoOpenPublisherDimmed] Subscriber options, Find publisher button, Dimmed */
		"To open the document that contains the publisher for the selected "
		"subscriber, click this button.  This button is not available because the "
		"edition cannot be found (it has been deleted, or the disk it is on is "
		"unavailable).";

	/* [hsisoGetEditionNow] Subscriber options, Get edition now button, Normal */
		"To update the selected subscriber now, click this button.  The  subscriber "
		"will be replaced by the material in the edition named above.";

	/* [hsisoGetEditionNowDimmed] Subscriber options, Get edition now button, Dimmed */
		"To update the selected subscriber now, click this button.  The button is "
		"not available because the edition cannot be found (it has been deleted, or "
		"the disk it is on is unavailable).";

	/* [hsisoAutomatically] Subscriber options, Automatically button, Normal */
		"To update the selected subscriber automatically whenever the edition is "
		"changed, click this button.";

	/* [hsisoAutomaticallyChecked] Subscriber options, Automatically button, Checked */
		"The dot in this button indicates that the selected subscriber is updated "
		"automatically whenever the edition is changed.";

	/* [hsisoManually] Subscriber options, Manually button, Normal */
		"To update the selected subscriber only when you click the Get Edition Now "
		"button , click this button.";

	/* [hsisoManuallyChecked] Subscriber options, Manually, Checked */
		"The dot in this button indicates that the selected subscriber is updated "
		"only when you click the Get Edition Now button.";

	/* [hsisoLatestEdition] Subscriber options, Latest edition, Normal */
		"This line tells you the last time the material in the edition was updated "
		"by the publisher.  \n\nIf you are updating automatically, it also tells "
		"you the last time the selected subscriber was updated by the edition.";

	/* [hsisoLatestEditionDImmed] Subscriber options, Latest edition, Dimmed */
		"This tells you the last time the material in the edition file was updated "
		"by the publisher.  The date is currently unavailable becuase the edition "
		"cannot be found (it has been deleted, or the disk it is on is unavailable).";

	/* [hsisoLastReceived] Subscriber options, Last received, Normal */
		"This line tells you the date and time of the last change to the edition "
		"that the selected subscriber received.\n\nIf this time is earlier "
		"than the time on the line above, you have not received the latest edition.";
		
	/* [hsisoCancel] Subscriber options, cancel button, Normal */
		"To cancel any changes you made to the settings in this dialog box and "
		"close the dialog box, click this button.";

	/* [hsisoOK] Subscriber options, ok button, Normal */
		"To confirm any changes you made to the settings in this dialog box and "
		"close the dialog box, click this button.";

	}
};


resource 'hdlg' (rPublisherOptionsDialog, purgeable) {
	HelpMgrVersion,							/* Help Version */
	0,										/* offset to first DITL */
	0,										/* options */
	0,										/* theProc */
	0,										/* variant */
	HMSkipItem {	/* skip missing message */
	},
	{
		HMStringResItem {	/* OK button */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoOK,					/* Enabled item */
			rPublisherOptionsHelpStrings,0,						/* Disabled */
			rPublisherOptionsHelpStrings,0,
			rPublisherOptionsHelpStrings,0
		},
		HMStringResItem {	/* Cancel button */
			{0,0},
			{0,0,0,0},
			rSubscriberOptionsHelpStrings,hsisoCancel,				/* Enabled item */
			rPublisherOptionsHelpStrings,0,						/* Disabled */
			rPublisherOptionsHelpStrings,0,
			rPublisherOptionsHelpStrings,0
		},
		HMSkipItem {		/* Outline around the OK button */
		},
		HMStringResItem {	/* Cancel Publisher button */
			{0,0},
			{0,0,0,0},
			rPublisherOptionsHelpStrings,hsipoCancelPublisher,		/* Enabled button */
			rPublisherOptionsHelpStrings,0,
			rPublisherOptionsHelpStrings,0,
			rPublisherOptionsHelpStrings,0
		},
		HMSkipItem {		/* !Does not show up! Open Publisher button */
		},
		HMStringResItem {	/* Send Edition Now button */
			{0,0},
			{0,0,0,0},
			rPublisherOptionsHelpStrings,hsipoSendEditionNow,			/* Enabled button */
			rPublisherOptionsHelpStrings,hsipoSendEditionNowDimmed,	/* Disabled */
			rPublisherOptionsHelpStrings,0,
			rPublisherOptionsHelpStrings,0
		},
		HMStringResItem {	/* Manually button */
			{0,0},
			{0,0,0,0},
			rPublisherOptionsHelpStrings,hsipoManually,				/* Enabled, Unchecked item */
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,hsipoManuallyChecked,			/* Checked */
			rPublisherOptionsHelpStrings,0
		},
		HMStringResItem {	/* On Save button */
			{0,0},
			{0,0,0,0},
			rPublisherOptionsHelpStrings,hsipoOnSave,				/* Enabled, Unchecked item */
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,hsipoOnSaveChecked,		/* Checked */
			rPublisherOptionsHelpStrings,0
		},
		HMSkipItem {		/* main title text */
		},
		HMStringResItem {	/* pop up path item */
			{0,0},
			{0,0,0,0},
			rPublisherOptionsHelpStrings,hsipoPopUpMenu,			/* Enabled item */
			rPublisherOptionsHelpStrings,hsipoPopUpMenuDimmed,		/* Disabled */
			rPublisherOptionsHelpStrings,0,
			rPublisherOptionsHelpStrings,0
		},
		HMSkipItem {		/* double line pict */
		},
		HMSkipItem {		/* cluster border pict */
		},
		HMSkipItem {		/* cluster border title */
		},
		HMStringResItem {	/* Latest Edition Sent: text */
			{0,0},
			{0,0,0,0},
			rPublisherOptionsHelpStrings,hsipoLatestEdition,		/* Enabled */
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,0
		},
		HMStringResItem {	/* Latest Section: text */
			{0,0},
			{0,0,0,0},
			rPublisherOptionsHelpStrings,hsipoLastChange,			/* Enabled */
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,0
		},
		HMStringResItem {	/* Latest Edition Sent date: text */
			{0,0},
			{0,0,0,0},
			rPublisherOptionsHelpStrings,hsipoLatestEdition,		/* Enabled */
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,0
		},
		HMStringResItem {	/* Latest Section Date: text */
			{0,0},
			{0,0,0,0},
			rPublisherOptionsHelpStrings,hsipoLastChange,			/* Enabled */
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,0,						
			rPublisherOptionsHelpStrings,0
		},
	}
};


resource 'STR#' (rPublisherOptionsHelpStrings, purgeable) {
	{
	/* [hsipoPopUpMenu] Publisher options, Path box, Normal */
		"Use this pop-up menu to see the path to the edition for the "
		"selected publisher.";

	/* [hsipoPopUpMenuDimmed] subscriber options, path box, Dimmed */
		"Use this pop-up menu to see the path to the edition for the "
		"selected publisher.  The items in the menu are dimmed because the edition "
		"cannot be found (it has been deleted, or the disk it is on is "
		"unavailable).";

	/* [hsipoCancelPublisher] Publisher options, Cancel Publisher button, Normal */
		"To permanently stop sending editions from this publisher, click this "
		"button.  Future changes you make to the selected material will not be sent "
		"on to an edition and subscribers in other documents will not receive the "
		"changes.";

	/* [hsipoSendEditionNow] Publisher options, Send edition button, Normal */
		"To update the material in the edition now, click this button.\n\nThe "
		"material in the edition will be replaced with the material in the selected "
		"publisher.";

	/* [hsipoSendEditionNowDimmed] Publisher options, Send edition button, Dimmed */
		"To update the material in the edition now, click this button.  The button "
		"is not available because the edition is on a disk that is unavailable.";

	/* [hsipoOnSave] Publisher options, On save button, Normal */
		"To update the edition automatically each time you save the document that "
		"contains the publisher, click this button.";

	/* [hsipoOnSaveChecked] Publisher options, On save button, Checked */
		"The dot in this button indicates that the material in the edition is "
		"updated automatically each time you save the document that contains the "
		"publisher.";

	/* [hsipoManually] Publisher options, Manually button, Normal */
		"To update the edition only when you click the Send Edition Now button, "
		"click this button.";

	/* [hsipoManuallyChecked] Publisher options, Manually button, Checked */
		"The dot in this button indicates that the edition is updated only when you "
		"click the Send Edition Now button.";

	/* [hsipoLatestEdition] Publisher options, latest edition, Normal */
		"This line tells you the last time this publisher updated the edition.";

	/* [hsipoLatestEditionDimmed] Publisher options, latest edition, Dimmed */
		"This line tells you the last time this publisher updated the edition file.  "
		"It is currently unavailable because the edition file can not be found.";

	/* [hsipoLastChange] Publisher options, last change, Normal */
		"This line tells you the last time you saved changes to the document that "
		"contains this publisher.\n\nIf this time is later than the time listed "
		"above, you have not updated the edition with the latest changes to the "
		"publisher.";
	}
};
