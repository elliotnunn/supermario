/*
	File:		StandardFile.r

	Contains:	All resources needed for Standard File

	Copyright:	© 1990-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM5>	11/12/92	kc		Remove shell variable path names. Add compile time conditional.
	   <SM4>	 11/2/92	kc		Change ObjDir to RsrcDir.
		 <5>	 6/28/92	DTY		Mark StandardFile 'PACK' as purgeable.
		 <4>	 4/28/92	DTY		Get rid of hasBalloonHelp conditional.
		 <3>	 3/27/92	DTY		Dean gets too big for his britches and gets caught checking
									files in without making sure the system still builds.
		 <2>	 3/27/92	DTY		Set resSysHeap bit on 'PACK' resource.
		 <1>	 10/1/91	JSM		first checked in
		 <0>	 10/1/91	JSM		Created from StandardFile3.r.

	Modification history from StandardFile3.r below:

		<36>	 10/1/91	JSM		Don’t use SystemSevenOrLater, it always is.
		<35>	 4/10/91	ngk		JDR, #BB-KA-036: set MENU -4000 to be non-purgable
		<34>	  4/4/91	ngk		#f1-ngk-005: fix misspelling of "original" in error dialog.
		<33>	 3/21/91	RLC		ngk,#83924 - Change string for save & open buttons to include
									why you can't save to a locked or unaccessible volume, i.e. one
									with not enuf access privs.
		<32>	 2/26/91	RLC		ngk,#81872 - Roll in new strings from final edit. Approved by
									GO5.
		<31>	 2/17/91	ngk		KSM,#BS-044: Changed "A system error occurred…" to 
									"Sorry, an error occurred."
		<30>	 2/11/91	ngk		RLC, #82234: Fix stationery warning layout to fit
	   								longer file names.
									RLC, #67912: Use tip hit in for volume name so it looks
									better with short volume names.
		<29>	12/19/90	stb		& JDR; change SystemSevenOrBetter to SystemSevenOrLater
		<28>	12/18/90	RLC		<ngk> Update the help strings to the latest version from
									J.Trumble.
		<27>	12/13/90	ngk		add original in trash alert string, fix rect of pop up.
		<26>	11/20/90	DC		Changed identifier names for generic icons to conform to new
									public names
	    <25>	 10/9/90	ngk		Change "Replace Existing" button names.  Updated fmap cause
	   								the file type for aliases to application changed.  Added
									a 'dctb' for NewFolderDialog.
		<24>	 9/13/90	ngk		Changed file type for edition files to be all lowercase.
		<23>	 8/31/90	ngk		Add table mapping resource types to icons. Add string of
	   								command keys for localization.
		<22>	 8/21/90	ngk		Add file locked warning
		<21>	  8/2/90	ngk		Factor out StandardFile3Type.r
		<20>	 7/25/90	RLC		Nick's tweek to John's Help Strings (hsisfOpenFolder).
		<19>	 7/24/90	RLC		Update the STR# resource to J.Trumble's latest help strings.
		<18>	 7/24/90	ngk		Add hsisfOpenFolder help message for putfile "open" button.
	   								Changed rGrayRectID to 11.
		<17>	 7/24/90	RLC		Change explicit #include of BalloonTypes.r to reference file in
									{RIncludes}.
		<16>	  7/6/90	RLC		Make the Open/Save button in both SF2 & SF3 templates use
									optional CntlMax>1 help message.
		<15>	  7/2/90	ngk		Renumbered IDs because of conflict with disk init.
	   								Made all resources purgable.  Change ALRTs to DLOGs.
									Fixed bugs in help resources.
		<14>	  6/7/90	ngk		Change Desktop back to Drive in SF2 dialogs. Fix DLOG and ALRTs
	   								to assume system 7 instead of the funky conditionals.
		<13>	  6/7/90	DC		fix up ALRT's and DLOG's for auto-positioning
		<12>	  6/1/90	ngk		Add folder name already exists error msg. Tweaked standard
									alert.
		<11>	 5/23/90	ngk		Added help for NewFolder subdialog. Added MENU for pop up CDEF.
		<10>	  5/4/90	RLC		Update to latest J.Trumble strings.
		 <9>	 4/10/90	ngk		Add dialogs templates for new SF dialogs. Add new folder dialog.
									Renumbered new resources to be owned resources of PACK3. Added
									symbolic constants for help string indexes. Moved icon data to
									GenericIcons.r
		 <8>	 3/27/90	csd		added the color versions of the generic folder icon--it got left
									out before.
		 <7>	 3/19/90	JRM		add generic stationery icon
		 <6>	 3/19/90	JRM		add full trash and special folders icons. now, all icons are in
									the same place. Add open old stationery warning string. update
									standalonePopup to smaller version which matches window title
		 <5>	  3/8/90	RLC		Update strings to newest release from J.Trumble.
		 <4>	  3/7/90	JSM		Remove type definitions for icl4, icl8, ics4, and ics8 (these
									are now in Types.r).
		 <3>	  3/5/90	csd		added several color icons
		 <2>	 2/18/90	ngk		Add Replace on Folder error message
		 <1>	 2/17/90	ngk		Moved all system 7.0 related standard file resources into this
									one .r file.

*/

#include "Types.r"
#include "BalloonTypes.r"
#if NewBuildSystem
#include "IconUtilsPrivTypes.r"
#include "FinderPriv.h"
#include "StandardFileTypes.r"
#else
#include $$Shell("IntRIncludes")"IconUtilsPrivTypes.r"
#include $$Shell("IntCIncludes")"FinderPriv.h"
#include $$Shell("StandardFile")"StandardFileTypes.r"
#endif

Include $$Shell("RsrcDir")"StandardFilePACK.rsrc" 'PACK' (3) as 'PACK' (3, sysHeap, purgeable);		// get PACK code

Include $$Shell("RsrcDir")"StandardFileLDEF.rsrc";		// get LDEF code


// MENU resource for pop up
resource 'MENU' (sfPopUpMenuID) 						// do not make purgeable, <35, #BB-KA-036>
	{ sfPopUpMenuID, textMenuProc, 0, enabled, "", {} };

// PICT resources
resource 'PICT' (rNewFolderPictID, purgeable) {
	124,
	{10, 10, 30, 90},
	$"1101 0100 0A00 0A00 0A00 1E00 5A0A 0000"
	$"0000 0000 0000 0D00 0C2B 1718 034E 6577"
	$"9000 0401 CD01 4801 DA01 6001 CD01 4901"
	$"DA01 5900 0D00 3E00 1A00 4E00 011E 0000"
	$"0321 0002 547F FF02 5440 0082 5540 0080"
	$"0340 0080 0440 0080 0440 0082 5540 0080"
	$"0040 0080 0040 0080 0040 0080 007F FF80"
	$"00FF"
};


// CNTL resources
resource 'CNTL' (rNewFolderControlID, purgeable) {
	{0, 0, 20, 80}, 0, visible, 1, 0, 16*61, rNewFolderPictButtonID, ""
};


// picb resources
resource 'picb' (rNewFolderPictButtonID, purgeable) {
	rNewFolderPictID, 0, 0
};


// StandardFile Dialogs

resource 'DLOG' (rStandardFilePutID, purgeable)
	{ {0, 0, 188, 344}, dBoxProc, invisible, noGoAway, 0, rStandardFilePutID, "", noAutoCenter };		
resource 'dctb' (rStandardFilePutID, purgeable)
	{ {} };		
resource 'DITL' (rStandardFilePutID, purgeable)
	 { {	
		{161, 252, 181, 332}, Button { enabled, "Save" },
		{130, 252, 150, 332}, Button { enabled, "Cancel" },
		{0,0,0,0}, HelpItem { disabled, HMScanhdlg {rStandardFilePutID}},
		{8, 235, 24, 337}, UserItem { enabled },
		{32, 252, 52, 332}, Button { enabled, "Eject" },
		{60, 252, 80, 332}, Button { enabled, "Desktop" },
		{29, 12, 127, 230}, UserItem { enabled },
		{6, 12, 25, 230}, UserItem { enabled },
		{119, 250, 120, 334}, Picture { disabled, rGrayRectPictID },
		{157, 15, 173, 227}, EditText {	enabled, "" },
		{136, 15, 152, 227}, StaticText { disabled, "Save as:" },
		{88, 252, 108, 332}, UserItem { disabled },
	} };


resource 'DLOG' (rStandardFileGetID, purgeable)
	{ {0, 0, 166, 344}, dBoxProc, invisible, noGoAway, 0, rStandardFileGetID, "", noAutoCenter };
resource 'dctb' (rStandardFileGetID, purgeable)
	{ {} };		
resource 'DITL' (rStandardFileGetID, purgeable) 
	{ { 	
		{135, 252, 155, 332}, Button { enabled, "Open" },
		{104, 252, 124, 332}, Button { enabled, "Cancel" },
		{0,0,0,0}, HelpItem { disabled, HMScanhdlg {rStandardFileGetID} },
		{8, 235, 24, 337}, UserItem { enabled },
		{32, 252, 52, 332}, Button { enabled, "Eject" },
		{60, 252, 80, 332}, Button { enabled, "Desktop" },
		{29, 12, 159, 230}, UserItem { enabled },
		{6, 12, 25, 230}, UserItem { enabled },
		{91, 251, 92, 333}, Picture { disabled, rGrayRectPictID },
	} };


resource 'DLOG' (rSFNewFolderDialogID, purgeable)
	{ {40, 40, 130, 244}, dBoxProc, invisible, noGoAway, 0, rSFNewFolderDialogID, "", noAutoCenter };
resource 'dctb' (rSFNewFolderDialogID, purgeable)
	{ {} };		
resource 'DITL' (rSFNewFolderDialogID, purgeable) 
	{ {
		{61, 130, 81, 190}, Button { enabled, "Create" },
		{61, 10, 81, 70}, Button { enabled, "Cancel" },
		{33, 12, 49, 189}, EditText { disabled, "untitled folder" },
		{8, 10, 28,178}, StaticText { disabled, "Name of new folder:" },
		{0,0,0,0}, HelpItem { disabled, HMScanhdlg {rSFNewFolderDialogID}}
	} };



// Old StandardFile Dialogs

resource 'DLOG' (rStandardFileOldPutID, purgeable)
	{ {0, 0, 184, 304}, dBoxProc, invisible, noGoAway, 0, rStandardFileOldPutID, "", noAutoCenter };
resource 'DITL' (rStandardFileOldPutID, purgeable)
	{ {
	{132, 218, 150, 288}, Button { enabled, "Save" };
	{158, 218, 176, 288}, Button { enabled, "Cancel" };
	{136, 14, 152, 197}, StaticText { disabled, "Save as:" };
	{29, 200, 49, 302}, UserItem { disabled };
	{56, 218, 74, 288}, Button { enabled, "Eject" };
	{82, 218, 100, 288}, Button { enabled, "Drive" };
	{157, 17, 173, 194}, EditText { enabled, "" };
	{29, 14, 127, 197}, UserItem { disabled };
	} };


resource 'DLOG' (rStandardFileOldGetID, purgeable)
	{ {0, 0, 200, 348}, dBoxProc, invisible, noGoAway, 0, rStandardFileOldGetID, "", noAutoCenter };
resource 'DITL' (rStandardFileOldGetID, purgeable)
	{ {
	{138, 256, 156, 336}, Button { enabled, "Open" };
	{1152, 59, 1232, 77}, Button { enabled, ""};
	{163, 256, 181, 336}, Button { enabled, "Cancel" };
	{39, 232, 59, 347}, UserItem { disabled };
	{68, 256, 86, 336}, Button { enabled, "Eject" };
	{93, 256, 111, 336}, Button { enabled, "Drive" };
	{39, 12, 185, 230}, UserItem { enabled };
	{39, 229, 185, 246}, UserItem { enabled };
	{124, 252, 125, 340}, UserItem { disabled };
	{1044, 20, 1145, 116}, StaticText { disabled, "" };
	} };


// “Replace Existing” dialog: keep bounds within SF dialog to allow updates
resource 'DLOG' (rSFReplaceExistingDialogID, purgeable)
	{ {0, 0, 80, 200}, dBoxProc, invisible, noGoAway, 0, rSFReplaceExistingDialogID, "", centerParentWindow };
resource 'DITL' (rSFReplaceExistingDialogID, purgeable)
	{ {
	{50, 120, 70, 190}, Button { enabled, "Replace" };
	{50, 10, 70, 80}, Button { enabled, "Cancel" };
	{10, 10, 40, 190}, StaticText { disabled, "Replace existing “^0” ?" };
	} };



// “Stationary Warning” dialog: keep bounds within SF dialog to allow updates
resource 'DLOG' (rSFStationeryWrnDialogID, purgeable)
	{ {0, 0, 150, 200}, dBoxProc, invisible, noGoAway, 0, rSFStationeryWrnDialogID, "", centerParentWindow }; /* <30, #82234> */
resource 'DITL' (rSFStationeryWrnDialogID, purgeable)
	{ {
	{120, 120, 140, 190}, Button { enabled, "OK" };
	{120, 10, 140, 80}, Button { enabled, "Cancel" };
	{8, 10, 110, 190}, StaticText { disabled, "“^0” is a stationery pad.  If you make changes, they will be saved into the stationery pad itself." }; /* <30, #82234> */
	} };


// “Locked File Warning” dialog: keep bounds within SF dialog to allow updates
resource 'DLOG' (rSFLockedWrnDialogID, purgeable)
	{ {0, 0, 110, 200}, dBoxProc, invisible, noGoAway, 0, rSFLockedWrnDialogID, "", centerParentWindow };
resource 'DITL' (rSFLockedWrnDialogID, purgeable)
	{ {
	{80, 120, 100, 190}, Button { enabled, "OK" };
	{80, 10, 100, 80}, Button { enabled, "Cancel" };
	{10, 10, 70, 190}, StaticText { disabled, "“^0” is locked, so you will not be able to save any changes." };
	} };


// generic error dialog: keep bounds within SF dialog to allow updates
resource 'DLOG' (rSFOKDialogID, purgeable)
	{ {0, 0, 100, 223}, dBoxProc, invisible, noGoAway, 0, rSFOKDialogID, "", centerParentWindow };
resource 'DITL' (rSFOKDialogID, purgeable)
	{ {
	{70, 140, 90, 210}, Button { enabled, "OK" };
	{10, 10, 66, 210}, StaticText { disabled, "^0" };
	} };


// standard file strings
resource 'STR#' (rStandardFileStringsID, purgeable)
	{ {
	"Desktop",
	"Trash",
	"Open",
	"The disk is locked.",
	"You do not have the access privileges to open that folder.",
	"Bad character in name, or can’t find that disk.",
	"Sorry, an error occurred.",													/* <31, #BS-044> */
	"The alias could not be opened, because the original could not be found.",		/* <34, #f1-ngk-005> */
	"That name is already used by a folder.",
	"That name is already taken; please use another name.",
	"oødn",			// command keys for open, option-open, desktop, and newfolder
	"The alias could not be opened, because the original is in the trash."
	} };



type 'fmap' 
	{
	array 
		{
		unsigned longint;	/* file type	*/
		unsigned integer;  	/* sf icon ID   */
		unsigned integer;   /* finder icon ID */
		};
	};

resource 'fmap' (rAliasTypeMapTable, purgeable)
	{ {									// icon StandardFile uses		// icon Finder users
	kContainerFolderAliasType, 		 	genericFolderIconResource,		genericFolderIconResource;
	kContainerServerAliasType, 		 	genericFileServerIconResource,	genericFileServerIconResource;
	kContainerFloppyAliasType, 			floppyIconResource,				floppyIconResource;
	kContainerHardDiskAliasType, 		genericHardDiskIconResource,	genericHardDiskIconResource;
	kControlPanelFolderAliasType, 		genericFolderIconResource,		controlPanelFolderIconResource;
	kExtensionFolderAliasType, 		 	genericFolderIconResource,		extensionsFolderIconResource;
	kPreferencesFolderAliasType, 		genericFolderIconResource,		preferencesFolderIconResource;
	kPrintMonitorDocsFolderAliasType,	genericFolderIconResource,		spoolFolderIconResource;
	kStartupFolderAliasType, 		 	genericFolderIconResource,		startupFolderIconResource;
	kAppleMenuFolderAliasType, 		 	genericFolderIconResource,		appleMenuFolderIconResource;
	kSystemFolderAliasType, 		 	genericFolderIconResource,		systemFolderIconResource;
	kContainerTrashAliasType, 		 	trashIconResource,				trashIconResource;
	kSharedFolderAliasType, 		 	genericFolderIconResource,		sharedFolderIconResource;
	kDropFolderAliasType, 		 		genericFolderIconResource,		dropFolderIconResource;
	kExportedFolderAliasType, 		 	genericFolderIconResource,		ownedFolderIconResource;
	kMountedFolderAliasType, 		 	genericFolderIconResource,		mountedFolderIconResource;
	kApplicationAliasType, 				genericApplicationIconResource,	genericApplicationIconResource;	
	kContainerAliasType,				genericApplicationIconResource,	genericApplicationIconResource;  // 'drop' used to be for applications
	0,0,0;
	} };


resource 'fmap' (rGenericFileTypeMapTableID, purgeable)
	{ {
	'APPL', 		genericApplicationIconResource, 	genericApplicationIconResource;
	'dfil', 		genericDeskAccessoryIconResource, 	genericDeskAccessoryIconResource; 
	'edtt',			genericEditionFileIconResource,		genericEditionFileIconResource;
	'edtp',			genericEditionFileIconResource, 	genericEditionFileIconResource;
	'edts',			genericEditionFileIconResource, 	genericEditionFileIconResource;
	'edtt',			genericEditionFileIconResource, 	genericEditionFileIconResource;
	'edtu',			genericEditionFileIconResource, 	genericEditionFileIconResource;
	
	'edtT',			genericEditionFileIconResource, 	genericEditionFileIconResource;	// will be removed
	'edtP',			genericEditionFileIconResource, 	genericEditionFileIconResource;	// will be removed
	'publ',			genericEditionFileIconResource, 	genericEditionFileIconResource;	// will be removed
	0,0,0;
	} };



// hdlg template for put file
	resource 'hdlg' (rStandardFileOldPutID, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* offset to first DITL */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {	/* skip missing message */
		},
		{
			HMStringResItem {	/* Open/Save button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfSave,				/* Enabled button */
				rStandardFileHelpStringsID,hsisfSaveDimmed,			/* Disabled */
				rStandardFileHelpStringsID,hsisfOpenFolder,
				rStandardFileHelpStringsID,hsisfOpenFolderDimmed
			},
			HMStringResItem {	/* Cancel button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfCancelSave,			/* Enabled button */
				rStandardFileHelpStringsID,0,						/* Disabled */
				rStandardFileHelpStringsID,0,
				rStandardFileHelpStringsID,0
			},
			HMSkipItem {		/* Save As static text */
			},
			HMStringResItem {	/* Current drive User Item */
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
			HMStringResItem {	/* Save Edit text item */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfFileNameTE,			/* Enabled item */
				rStandardFileHelpStringsID,0,						/* Disabled */
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
			}
		}
	};


	resource 'hdlg' (rStandardFilePutID, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* offset to first DITL */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {	/* skip missing message */
		},
		{
			HMStringResItem {	/* Open button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfSave,				/* Enabled button */
				rStandardFileHelpStringsID,hsisfSaveDimmed,			/* Disabled */
				rStandardFileHelpStringsID,hsisfOpenFolder,
				rStandardFileHelpStringsID,hsisfOpenFolderDimmed
			},
			HMStringResItem {	/* Cancel button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfCancelSave,			/* Enabled button */
				rStandardFileHelpStringsID,0,						/* Disabled */
				rStandardFileHelpStringsID,0,
				rStandardFileHelpStringsID,0
			},
			HMSkipItem {		/* help on help?? */
			},
			HMStringResItem {	/* Current volume User Item */
				{12,80},																/* <30, #67912> */
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
			HMStringResItem {	/* TextEdit for file name */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfFileNameTE,			/* Enabled item */
				rStandardFileHelpStringsID,0,						/* Disabled */
				rStandardFileHelpStringsID,0,
				rStandardFileHelpStringsID,0
			},
			HMSkipItem {		/* no prompt */
			},
			HMStringResItem {	/* new folder button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfNewFolder,			/* Enabled item */
				rStandardFileHelpStringsID,hsisfNewFolderDimmed,	/* Disabled */
				rStandardFileHelpStringsID,0,
				rStandardFileHelpStringsID,0
			},
		}
	};



// hdlg template for Get file

	resource 'hdlg' (rStandardFileOldGetID, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* offset to first DITL */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {	/* skip missing message */
		},
		{
			HMStringResItem {	/* Open button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfOpen,				/* Enabled button */
				rStandardFileHelpStringsID,hsisfOpenDimmed,			/* Disabled */
				rStandardFileHelpStringsID,hsisfOpenFolder,
				rStandardFileHelpStringsID,hsisfOpenFolderDimmed
			},
			HMSkipItem {		/* Undefined button */
			},
			HMStringResItem {	/* Cancel button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfCancelOpen,			/* Enabled button */
				rStandardFileHelpStringsID,0,						/* Disabled */
				rStandardFileHelpStringsID,0,
				rStandardFileHelpStringsID,0
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
			}
		}
	};


	resource 'hdlg' (rStandardFileGetID, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* offset to first DITL */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {	/* skip missing message */
		},
		{
			HMStringResItem {	/* Open button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfOpen,				/* Enabled button */
				rStandardFileHelpStringsID,hsisfOpenDimmed,			/* Disabled */
				rStandardFileHelpStringsID,hsisfOpenFolder,
				rStandardFileHelpStringsID,hsisfOpenFolderDimmed
			},
			HMStringResItem {	/* Cancel button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfCancelOpen,			/* Enabled button */
				rStandardFileHelpStringsID,0,						/* Disabled */
				rStandardFileHelpStringsID,0,
				rStandardFileHelpStringsID,0
			},
			HMSkipItem {		/* help on help?? */
			},
			HMStringResItem {	/* Current volume User Item */
				{12,80},												/* <30, #67912> */
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
			}
		}
	};


	// balloons for new folder sub dialog
	resource 'hdlg' (rSFNewFolderDialogID, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* offset to first DITL */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {	/* skip missing message */
		},
		{
			HMStringResItem {	/* Create button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfCreate,				/* Enabled button */
				rStandardFileHelpStringsID,hsisfCreateDimmed,		/* Disabled */
				rStandardFileHelpStringsID,0,
				rStandardFileHelpStringsID,0
			},
			HMStringResItem {	/* Cancel button */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfCancelCreate,		/* Enabled button */
				rStandardFileHelpStringsID,0,						/* Disabled */
				rStandardFileHelpStringsID,0,
				rStandardFileHelpStringsID,0
			},
			HMStringResItem {	/* name of new folder edit text */
				{0,0},
				{0,0,0,0},
				rStandardFileHelpStringsID,hsisfNewFolderTE,		/* Enabled item */
				rStandardFileHelpStringsID,hsisfNewFolderTE,		/* Disabled */
				rStandardFileHelpStringsID,0,
				rStandardFileHelpStringsID,0
			}
		}
	};


// help for pop up menu showing full path
	resource 'hmnu' (sfPopUpMenuID, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMStringResItem {
				rStandardFileHelpStringsID,hsisfPopUpItem,
				rStandardFileHelpStringsID,hsisfPopUpItem,
				rStandardFileHelpStringsID,hsisfPopUpItem,
				rStandardFileHelpStringsID,hsisfPopUpItem,
			},
		{
			HMSkipItem {						/* no title for popups */
			}
		}
	};
	
resource 'STR#' (rStandardFileHelpStringsID, purgeable) {
	{
	/* [hsisfPopUpMenu] Standard file "open", Hierarchy pop-up (top of window), Normal */
		"Use this pop-up menu to select the folder or disk that contains the file "
		"you want to open.  When you choose an item from this menu, its contents "
		"appear in the list directly below this menu.";

	/* [hsisfFileList] Standard file "open", directory window, Normal */
		"This list shows the contents of the item named above.  Double-click a "
		"folder or disk to see its contents in this window. Double-click a document "
		"to open it.";

	/* [hsisfVolume] Standard file "open", Root directory icon & name, Normal */
		"This icon represents a disk on your desktop.  Click here to see the "
		"contents of the folder or disk that contains the folder named to the left.";

	/* [hsisfEject] Standard file "open", Eject button, Normal */
		"To eject the disk named above, click this button.";

	/* [hsisfEjectDImmed] Standard file "open", Eject button, Dimmed */
		"This button ejects the disk named above. The button is not available "
		"because the item listed above is not an ejectable disk.";

	/* [hsisfDesktop] Standard file "open", Desktop button, Normal */
		"To see a list of the items on your desktop, click this button. To see the "
		"contents of a different disk, click this button, then double-click the "
		"disk’s name in the window on the left.";

	/* [hsisfDesktopDimmed] Standard file "open", Desktop button, Dimmed */
		"To see a list of the items on your desktop, click this button. The button "
		"is not available because items on the desktop are already listed in the "
		"window.";

	/* [hsisfOpen] Standard file "open", Open button, Normal */
		"To see the contents of the selected disk or folder, or to open the "
		"selected file, click this button.";

	/* [hsisfOpenDimmed] Standard file "open", Open button, Dimmed */
		"To see the contents of the selected disk or folder, or to open the selected "
		"file, click this button.  The button is not available because nothing is "
		"selected, or because you cannot open the selected item.";

	/* [hsisfCancel] Standard file "open", Cancel button, Normal */
		"To close the dialog box without opening a document, click this button.";

	/* [hsisfFileNameTE] Standard file "save", Save this document as box, Normal */
		"Type a name for your new document in this box.";

	/* [hsisfSave] Standard file "save", Save button, Normal */
		"To save your document in the folder or disk whose contents are listed in "
		"the window, click this button.  The document will have the name shown in "
		"the box below the window.";

	/* [hsisfSaveDimmed] Standard file "save", Save button, Dimmed */
		"To save your document in the folder or disk whose contents are listed in "
		"the window, click here.  The button is not available because no name has "
		"been provided for the document, or because you cannot make changes to the "
		"selected folder or disk.";

	/* [hsisfCancelSave] Standard file "save", Cancel button, Normal */
		"To close the dialog box without saving the document, click this button.";
		
	/* [hsisfNewFolder] Standard file "save", New Folder button, Normal */
		"To create a new folder in the disk or folder listed to the left, click "
		"this button.";
		
	/* [hsisfNewFolderDimmed] Standard file "save", New Folder button, Dimmed */
		"To create a new folder in the disk or folder listed to the left, click "
		"this button.  Not available now because you cannot create a new folder on "
		"the disk named above.";
		
	/* [hsisfPopUpItem] Standard file "open", pop-up menu items, Normal */
		"Displays this item’s contents in the list below.";
		
	/* [hsisfCreate] Standard file "save", new folder dialog, new button, Normal */
		"To create a new folder with the name given above, click this button.";
		
	/* [hsisfCreateDimmed] Standard file "save", new folder dialog, new button, Dimmed */
		"To create a new folder, click this button.  The button is not available "
		"because no name is entered for the folder.";
		
	/* [hsisfCancelCreate] Standard file "save", new folder dialog, cancel button, Normal */
		"To cancel your new folder, click this button.";
		
	/* [hsisfNewFolderTE] Standard file "save", New folder dialog, name box, Normal */
		"Type a name for your new folder in this box.";
		
	/* [hsisfOpenFolder] Standard file "save", Open button (NOT the same as bubble for "open" dialog), Normal */
		"To see the contents of the selected disk or folder, click this button.";

	/* [hsisfOpenFolderDimmed] Standard file "save", Open button (NOT the same as bubble for "open" dialog), Dimmed */
		"To see the contents of the selected disk or folder, click this button.  The "
		"button is not available because no folder or disk is selected, or because "
		"you cannot open the selected item.";
	}
};

