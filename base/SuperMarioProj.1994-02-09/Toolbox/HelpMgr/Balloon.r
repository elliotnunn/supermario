/*
	File:		Balloon.r

	Contains:	resource definitions needed in Balloon Help

	Written by:	Randy Carr

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<54>	  6/2/92	KST		#1030643 <JH>: For change #53, leave only 1 resource in
									Balloon.r. (Only 1 default message for the floating window).
		<53>	 5/14/92	KST		#1027497 <JH>: #1025797,#1027497: Added default resources so
									that Balloon help can work when mouse is over a floating window.
		<52>	11/21/91	csd		#83334: Merged "This window belongs to the application “" with
									"”. To make this window active, click in it." into one string
									for easier localization.
		<51>	 2/26/91	RLC		ngk: Roll in new strings from final edit. (Approved by GO5)
		<50>	  2/8/91	dba		dty: trim about 11K off of the help pictures, without affecting
									the appearance
		<49>	  2/1/91	RLC		<ngk> Change the About Dialog's appearance again by adding a
									color pict and a 'dctb' resource.
		<48>	 1/23/91	RLC		<rnv> Remove copyright notice from About… dialog as per John
									Sullivan.
		<47>	 1/18/91	KSM		<RLC> Fix name difference between this file and Balloons.a!
		<46>	 1/15/91	RLC		Update the Finder Help file icon again as per Paulien.
		<45>	 1/15/91	RLC		<DH> Update the Finder Help file icon.
		<44>	  1/2/91	RLC		<ngk> Add the 'ics8' icon for the menu back in. Where'd it go?
		<43>	12/20/90	RLC		<VL> Add Finder's help file icon data back in.
		<42>	12/14/90	RLC		<jt> Remove kHMAppleMenuDisabledTitle & kHMAppleMenuDisabledItem
									strings that these are identical to kHMActiveApplDisabledTitle &
									kHMActiveApplDisabledItem strings.
		<41>	 12/3/90	RLC		<jt> Update the help strings to the latest.
		<40>	 8/31/90	RLC		Change help menu 'hmnu' resource to use missing message for
									modal dialog gray menu items.
		<39>	 8/21/90	RLC		Make the ics8 & ics4 better (more pillowlike).
		<38>	 8/20/90	RLC		Add ics8 & ics4 data back to file.
	   <36+>	 8/17/90	RLC		Change ics# to new design.
	   <37+>	  8/6/90	RLC		Change about box dialog resources again.
	   <36+>	 7/27/90	RLC		Update about box and use styled text for one balloon msg.
	   <35+>	 7/24/90	RLC		YMC (Yet More Changes).
	   <34+>	 7/20/90	RLC		Add data to support the styled text balloon for 'Turns Balloon
									Help off.'
	   <33+>	 6/25/90	RLC		Switch About Balloon Help… dialog main picts.
	   <32+>	 6/15/90	RLC		Update some strings.
	   <31+>	 6/14/90	RLC		Update DLOG to do autocentering.
		<31>	  6/7/90	DC		Fix up ALRT's and DLOG's for auto-positioning
	   <29+>	  6/5/90	RLC		Add a string to support more what is messages.
	   <28+>	  6/1/90	RLC		Add a testing flag checkbox in About dialog for Dave Scroit.
	   <27+>	 5/31/90	RLC		Add a message to support the "Show All! Finder only is open"
									message
	   <26+>	 5/29/90	RLC		Add strings for "Show All" item in process menu.
		<26>	  5/7/90	RLC		Revise some strings that just don't make sense.
	   <24+>	  5/3/90	RLC		Update to latest J.Trumble strings.
		<24>	 4/18/90	RLC		Update the About… Dialog box again.
	   <22+>	 4/17/90	RLC		Update strings so that ^0 paramtext substitution can be used.
		<22>	 4/16/90	RLC		Switch the data to the right icon data for the System Menu icon.
		<21>	 4/13/90	RLC		change icon back.
	   <19+>	 4/10/90	KSM		Go for that new menu for the system menu patch.
	   <18+>	  4/9/90	RLC		New About Box (again!).
		<18>	 3/30/90	RLC		Add new ics & icl icons.
	   <16+>	 3/27/90	RLC		Update some defines and add default to 'MENU'
	   <15+>	 3/22/90	RLC		Add testing flag check box to dialog.
		<15>	 3/21/90	RLC		Add new D.Holmes About Help… dialog.
	   <13+>	 3/19/90	RLC		Change 'MENU' to use 'STR ' resource items for naming 'What Is'
									item.
		<13>	 3/14/90	RLC		Add Finder Help file icon.
		<12>	  3/8/90	RLC		Update to newest release of STR#'s from J.Trumble
		<11>	 2/27/90	EMT		<9+> broke build.  Removed offending //s.
		<9+>	 2/23/90	RLC		Update STR#, DLOG, & DITL resources.
		<8+>	  2/9/90	RLC		String fixes.
		<7+>	  2/5/90	RLC		Add Delay items to About Help… DITL
		<6+>	  2/2/90	RLC		Update Using… dialog, menus.
		<5+>	 1/29/90	RLC		Update Using Help… DITL template.
		<4+>	 1/22/90	RLC		Update Using Help… Dialog.
		<3+>	  1/4/90	RLC		Change to new resource format
		 <3>	  1/2/90	RC		Update Help Strings to latest text.
		 <2>	12/28/89	dba		include Types.r and BalloonTypes.r
	   <2.6>	11/30/89	RLC		Add new strings, remove 'on' help icon for modal dialogs (until
									later)
	   <2.5>	11/16/89	RLC		Oops, forgot that semi-colon again.
	   <2.4>	11/16/89	RLC		Update and add new strings to STR# resource.
	   <2.3>	11/13/89	RLC		Reordered the STR# resources to correspond to _FindWindow result
									codes.
	   <2.2>	11/10/89	RLC		Change some strings in 'STR#' resource.
	   <2.1>	 11/7/89	RLC		Add window parts strings to STR# resource.
	   <2.0>	 9/21/89	RLC		Changed Using Help… DLOG & DITL. Added PICT -5695, changed hmnu
									to use STR# resources to save some space.
	   <1.9>	 9/19/89	RLC		Added miscellaneous strings for Help Mgr.
	   <1.8>	 9/18/89	RLC		Added John Trumble's text to the Help Menu Balloons.
	   <1.7>	 9/18/89	RLC		MENU, DLOG, DITL, PICT resources changed.
	   <1.6>	 9/15/89	RLC		Changed ICON Resources for using Popupmenus in ModalDialogs
	   <1.5>	 9/15/89	RLC		Rearranged Help Menu again.
	   <1.4>	 9/14/89	RLC		Changed MenuBar Help ICN# Resource
	   <1.3>	 9/13/89	RLC		Shortened file to just include the resource data, not type decs.
	   <1.2>	 8/29/89	RLC		Added type declarations for new 'hmnu', 'hdlg', 'hrct'
									resources.
				 8/22/89	RLC		Rewrite all resource formats!
	   <1.1>	 7/20/89	RLC		Added resources to support the help manager menu & modal
									dialogs.
				 7/20/89	RLC		Added menu resources, 'hmnu' resource for help menu
	   <1.0>	 6/29/89	RLC		Created Today.
				 6/28/89	RLC		New Today!

	To Do:
*/

#include "PICT.r"
#include "Types.r"
#include "BalloonTypes.r"

#define kHMMenuTitleIndex			1
#define kHMAboutHelpIndex			2
#define kHMAboutHelpDisabled		3
#define kHMReserved					4
#define kHMWhatIsHide				5
#define kHMInDragIndex				6
#define kHMInGrowIndex				7
#define kHMInGoAwayIndex			8
#define kHMInZoomIndex				9
#define kHMMultiFinderIndex			10
#define kHMInScrollBar				11
#define kHMInDisabledScrollBar		12
#define kHMInOtherWindow			13
#define kHMInLayerPreamble			14
#define kHMInLayerBody				15
#define kHMInLayerSuffix			16
#define kHMAppleMenuTitle			17
#define kHMAboutEnabled				18
#define kHMAboutDisabled			19
#define kHMShowOnlyEnabled			20
#define kHMShowOnlyDisabled			21
#define kHMHideWindowsEnabled		22
#define kHMHideWindowsDisabled		23
#define kHMEasy1Access				24
#define kHMEasy2Access				25
#define kHMEasy3Access				26
#define kHMActiveApplEnabled		27
#define kHMActiveApplDisabled		28
#define kHMActiveApplChecked		29
#define kHMFinderOnlyDisabled		30
#define kHMActiveApplDisabledTitle	31
#define kHMActiveApplDisabledItem	32
#define kHMInFrontFloatingWindow	33			{ for IM, mouse is over a floating window }



#define kHMHelpIconID				-5695		/* unused */
#define kHMHelpID					-5696
#define kHMAboutHelpID				-5696

#define kHMKeybdMenuID				-16491
#define kHMHelpMenuID				-16490
#define kMFMenuID					-16489

#define kHMHideMenuString			"Hide Balloons"
#define kHMShowMenuString			"Show Balloons"


#define kAllItems	0b1111111111111111111111111111111	/* 31 flags */
#define kMenuItem2	0b10

resource 'MENU' (kHMHelpMenuID) {
	kHMHelpMenuID, textMenuProc,
	kAllItems & ~kMenuItem2,	/* disable dash only */
	enabled, "rANdY",
	{
		"About Balloon Help…",
			noicon, nokey, nomark, plain;
		"-",
			noicon, nokey, nomark, plain;
		"Show/Hide Balloons",
			noicon, nokey, nomark, plain
	}
};


resource 'STR ' (kHMHelpID, purgeable, sysheap) { kHMShowMenuString };
resource 'STR ' (kHMHelpID+1, purgeable, sysheap) { kHMHideMenuString };


resource 'STR#' (kHMHelpID, sysheap, purgeable) {
	{
	/* [119] Help, (Menu Title), Normal */
		"Help menu\n\nUse this menu to get information that helps you use your "
		"computer.";

	/* [120] Help, About help..., Normal */
		"Displays information about using the items in the Help menu.";

	/* [121] Help, About help..., Dimmed */
		"Displays information about using the items in the Help menu. Not available "
		"because the About Help dialog box is already open or because another "
		"dialog box is open.";

		""; /* Reserved */

	/* [116] Help, Hide Balloons, Normal */
		"Turns Balloon help off.";

	/* [147] Window Parts, Title bar (system), Normal */
		"Title bar\n\nThis is the window’s title bar.  To move the window, position "
		"the pointer in the title bar and drag the window.";

	/* [153] Window Parts, Resize box, Normal */
		"Size box\n\nTo change the height and width of the window, drag the size "
		"box.";

	/* [145] Window Parts, Close box, Normal */
		"Close box\n\nTo close this window, click here.";

	/* [149] Window Parts, Zoom box, Normal */
		"Zoom box\n\nTo change the size of the window quickly, click here.  "
		"Clicking once sizes the window so that all of its contents are visible (if "
		"possible).  Clicking again returns the window to its original size.";

	/* [125] Active Application menu, menu title, Normal */
		"Application menu\n\nUse this menu to switch from one application program "
		"to another when more than one program is open.";

	/* [150] Window Parts, Scroll bar (active), Normal */
		"Scroll bar\n\nTo scroll a little at a time, press one of the scroll "
		"arrows.  To scroll by the windowful, click in the gray bar.  To scroll to "
		"another part of the window, drag the scroll box.";

	/* [151] Window Parts, Scroll bar (inactive), Normal */
		"Scroll bar\n\nUse the scroll bar to see items that are out of view.  This "
		"scroll bar is not available because this window contains no items above or "
		"below the window’s borders.";

	/* [95] Window Parts, Window Parts, inactive window belonging to active app, Normal */
		"This is an inactive window.  To make this window active, click in it.";

	/* [154] Window Parts, window belonging to inactive app, Normal */			/* <52> */
		"This window belongs to the application “^0”.  To make this window "	/* <52> */
		"active, click in it.";													/* <52> */

		"";	/* reserved */		/* <52> */

	/* [168] finder object, inactive window behind modal dialog (inactive), Normal */
		"This window is not active because there is a dialog box on your screen. To "
		"make this window active, close the dialog box and then (if necessary) "
		"click in this window.";

	/* [3] Apple Menu, (Menu Title), Normal */
		"Apple menu\n\nUse this menu to open an item in the Apple Menu Items "
		"folder, or to see information about the active application or program. You "
		"can customize the items in this menu by adding or removing items from the "
		"Apple Menu Items folder.";

	/* [7] Application Menu, Hide command, Normal */
		"Hides ^0 windows, leaving all other windows visible.";

	/* [5] Application Menu, Show only command, Normal */
		"Hides the windows belonging to all inactive applications or programs.  "
		"Only ^0 windows remain visible.";

	/* [9] Application Menu, Show All, Normal */
		"Displays all hidden windows.";

	/* [8] Application Menu, Hide command, Dimmed */
		"Hides ^0 windows, leaving all other windows visible.  Not available "
		"because ^0 windows are the only open windows.";

	/* [9] Application Menu, Show only command, Dimmed */
		"Hides the windows belonging to all inactive applications or programs.  "
		"Only ^0 windows remain visible.  Not available because ^0 windows are the "
		"only visible windows.";

	/* [10] Application Menu, Show All, Dimmed */
		"Displays all hidden windows.  Not available because no windows are hidden.";

	/* [82] Finder object, Easy access icon (no arrow), Normal */
		"Sticky Keys\n\nThis symbol indicates that Sticky Keys is turned on.  When "
		"Sticky Keys is on, you can type combination keystrokes by pressing the "
		"keys one at a time instead of all at once. Press the Shift key 5 times to "
		"turn Sticky Keys on or off.";

	/* [83] Finder object, Easy access icon (arrow/empty), Normal */
		"Sticky Keys\n\nThis symbol indicates that Sticky Keys is turned on, and "
		"that you have pressed a modifier key (Command, Option, or Control). To "
		"complete a combination keystroke, press another modifier key or a letter "
		"or number key.";

	/* [84] Finder object, Easy access icon (arrow/full), Normal */
		"Sticky Keys\n\nThis symbol indicates that Sticky Keys is turned on, and "
		"that the key you have pressed is locked down until you press it again. You "
		"can now reuse the key (to cut and paste repeatedly, for example).";

	/* [126] Active Application menu, (Open applications), Normal */
		"Makes ^0 active.";

	/* [127] Active Application menu, (Open applications), Dimmed */
		"Makes this application or program active and shows its windows. The icon "
		"is dimmed because ^0 windows are hidden.";

	/* [128] Active Application menu, (Open applications), Checked */
		"Makes ^0 active. Checked because ^0 is active.";

	/* [129] Active Application menu, (Open applications), Other */
		"Makes ^0 active. Marked because ^0 needs immediate attention.";

	/* [1] Modal Dialog Menu, Menu titles, Normal */
		"This menu is not available because it cannot be used with the dialog box "
		"on your screen.";

	/* [2] Modal Dialog menu, Menu items, Normal */
		"This item is not available because it cannot be used with the dialog box "
		"on your screen.";
	
	/* [33] Window Parts, Window Parts, inactive window belonging to the IM layer, Normal */
	"This is a text service floating window. It floats above any application's windows.";
	}
};

resource 'hmnu' (kMFMenuID, purgeable, sysheap) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMStringResItem {	/* use missing msg */
				kHMHelpID,kHMActiveApplEnabled,
				kHMHelpID,kHMActiveApplDisabled,
				kHMHelpID,kHMActiveApplChecked,
				kHMHelpID,kHMActiveApplChecked
			},
		{
			HMStringResItem {
				kHMHelpID,kHMMultiFinderIndex,
				kHMHelpID,kHMMultiFinderIndex,
				kHMHelpID,kHMActiveApplDisabledTitle,
				kHMHelpID,kHMActiveApplDisabledItem
			},

			HMStringResItem {
				kHMHelpID,kHMShowOnlyEnabled,
				kHMHelpID,kHMShowOnlyDisabled,
				0,0,
				0,0
			},

			HMStringResItem {
				kHMHelpID,kHMHideWindowsEnabled,
				kHMHelpID,kHMHideWindowsDisabled,
				0,0,
				0,0
			}
		}
};

resource 'hmnu' (kHMHelpMenuID, purgeable, sysheap) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {	/* skip missing msg */
			},
		{
			HMStringResItem {
				kHMHelpID,kHMMenuTitleIndex,
				kHMHelpID,kHMMenuTitleIndex,
				kHMHelpID,kHMActiveApplDisabledTitle,
				kHMHelpID,kHMActiveApplDisabledItem
			},

			HMStringResItem {
				kHMHelpID,kHMAboutHelpIndex,
				kHMHelpID,kHMAboutHelpIndex,
				0,0,
				0,0
			},

			HMSkipItem { },

			HMStringResItem {
				kHMHelpID,kHMWhatIsHide,
				kHMHelpID,kHMWhatIsHide,
				kHMHelpID,kHMWhatIsHide,
				0,0
			},
		}
};

resource 'DLOG' (kHMAboutHelpID, sysheap, purgeable) {
	{40, 40, 217, 414},
	dBoxProc,
	visible,
	noGoAway,
	0x0,
	kHMAboutHelpID,
	"About…",
	alertPositionMainScreen
};

resource 'DITL' (kHMAboutHelpID, sysheap, purgeable) { {
	{142, 296, 162, 356}, Button { enabled, "OK" },
	{20, 24, 162, 112}, Picture { disabled, -5695 },
	{24, 123, 129, 356}, Picture { disabled, -5696 },
	{103, 173, 119, 189}, Picture { disabled, -5694 }
} };

/* color table for about balloon help dialog (so we can draw color picts) */
resource 'dctb' (kHMAboutHelpID) { { } };

// black & white part of About Help dialog

resource 'PICT' (-5696, purgeable) {
	{0, 118, 105, 351},
	VersionOne { {
		clipRgn { {0, 0, 105, 352}, "" },
		packBitsRect { 30, {0, 112, 105, 352}, {0, 118, 105, 351}, {0, 118, 105, 351}, srcCopy,
			$"1208 01E3 0000 0180 F800 CCFE 0002 0CC0"
			$"0CF2 0012 0803 3300 0001 80CC 00CC FE00"
			$"020C C00C F200 130F 0333 E1E3 33C0 CC78"
			$"CC78 78F8 0CC7 8CF8 F300 1100 03FE 3303"
			$"3180 CC8C FDCC 000C FECC F300 1107 03F3"
			$"3333 3180 F87C FDCC 000F FECC F300 1000"
			$"03FE 3301 3180 FBCC 030C CFCC CCF3 0010"
			$"0003 FE33 0131 80FB CC03 0CCC 0CCC F300"
			$"1200 03FE 3301 3180 FBCC 050C CC4C CCCC"
			$"C0F5 0015 1103 33E1 E1F0 C0F8 7CCC 7878"
			$"CC0C C78C F8CC C0F5 0006 F200 00C0 F300"
			$"06F2 0000 C0F3 0002 E300 02E3 0002 E300"
			$"02E3 0006 0003 E5FF 00FE 02E3 0002 E300"
			$"02E3 0002 E300 02E3 0002 E300 02E3 0002"
			$"E300 02E3 0002 E300 02E3 0002 E300 02E3"
			$"0002 E300 1905 03B8 000F 8022 FE00 01EE"
			$"01FE 0000 20FD 0003 4000 0008 FA00 1A05"
			$"0110 0004 4066 FE00 0544 0300 0400 60FD"
			$"0004 C000 0400 80FB 001F 1B01 11CE 044E"
			$"2238 E2C0 4471 1C0E 7023 8E2C B01C 70E6"
			$"6E09 CE3B 0E0E 2CFF 001F 1B01 1251 0791"
			$"2245 1720 7C89 3204 8824 5175 C822 4912"
			$"2418 9164 9211 72FF 001F 1B01 111F 044F"
			$"2245 1220 44F9 1104 8827 CF20 881E 4512"
			$"2408 9F24 8811 22FF 001F 1B01 1090 0451"
			$"2245 1220 4481 1104 8824 1120 8822 4512"
			$"2408 9024 8411 22FF 001F 1B01 1251 0451"
			$"2245 1220 4489 1204 8824 5120 8822 4912"
			$"7408 9124 9211 22FF 001F 1B00 E38E 0F8E"
			$"F738 E770 EE73 9C02 7073 8EF1 DC1D 70E1"
			$"A21C 4E7F DC0E 77FF 0006 F600 0010 EF00"
			$"06F6 0000 38EF 0002 E300 02E3 0002 E300"
			$"02E3 0002 E300 0C08 03BB 9996 0E31 671C"
			$"58EC 000C 0801 1448 BA12 4BA8 A2E4 EC00"
			$"0C08 00A4 4890 0881 0FBE 44EC 000C 0800"
			$"A448 9004 8108 2044 EC00 0C08 0044 49D0"
			$"1249 08A2 44EC 000D 0900 4386 B81C 3387"
			$"1CEE 40ED 0005 0100 80E5 0005 0100 80E5"
			$"0002 E300 02E3 0002 E300 02E3 0002 E300"
			$"02E3 0002 E300 02E3 0002 E300 02E3 0002"
			$"E300 02E3 0002 E300 02E3 0002 E300 02E3"
			$"001D 0103 E0FE 000A 3E00 8800 0003 B804"
			$"0000 18FE 0006 03D8 0000 07C0 11FD 001D"
			$"0102 A0FE 000A 1101 9800 0001 100C 0000"
			$"08FE 0006 0448 0000 0220 33FD 001F 1D00"
			$"8706 6738 1138 88E3 8B01 11C4 7001 8B0E"
			$"38E7 040B 0E7E C227 111C 7161 C01F 1D00"
			$"8882 2944 1E44 8914 5C81 F224 C802 4C91"
			$"4528 830C 9128 83C8 9122 8B92 401F 1D00"
			$"8882 247C 113C 8914 4881 13E4 4404 0891"
			$"448F 8088 9115 0227 9122 8911 001F 1D00"
			$"8882 2240 1144 8914 4881 1204 4404 0891"
			$"4448 0048 9115 0228 9122 8910 801F 1D00"
			$"8882 6944 1144 8914 4881 1224 4802 4891"
			$"4528 8448 910A 0228 9122 8912 401F 1D01"
			$"C701 AE38 3E3B DCE3 9DC3 B9CE 7081 9DCE"
			$"39C7 079D CE0A 07C7 7B9C 73BB 8006 F400"
			$"0041 F100 06F4 0000 E0F1 0002 E300 02E3"
			$"0002 E300 1601 00C0 FE00 00C0 F900 013E"
			$"C0FE 0000 01FE 0000 01FB 0015 0501 2000"
			$"0004 40F9 0001 2A40 FC00 0310 2000 10FC"
			$"001F 1D03 8B38 EC0E 5870 0000 0EC3 8B19"
			$"8008 5871 6038 7116 3873 8139 C761 C1C5"
			$"801F 1D01 1D45 9204 6488 0000 1924 5C88"
			$"8008 648B 9064 8B39 1024 4312 2C92 422E"
			$"401F 1D01 0844 9204 44F8 0000 0927 C888"
			$"8008 44F9 1022 8911 1024 4113 E491 0224"
			$"401F 1D01 0844 9204 4480 0000 0924 0888"
			$"8008 4481 1022 8911 1024 4112 0490 8224"
			$"401F 1D01 0844 9204 4488 0000 0924 4889"
			$"C008 4489 1024 8911 1024 4112 2492 4224"
			$"401F 1D03 9C39 FF02 EE70 0000 1FF3 9DC6"
			$"901C EE73 B838 73BB 8813 8389 CFFB 81CE"
			$"E006 EF00 0020 F600 06EF 0000 70F6 0002"
			$"E300 02E3 0002 E300 14F8 0006 0180 0200"
			$"0800 44FD 0000 10FE 0000 60FC 0017 FC00"
			$"0001 FD00 0580 0600 1800 CCFD 0004 3000"
			$"0102 20FC 001E 1A01 C62C E38B 039C 0E71"
			$"C0B0 E238 0E1C 4471 C587 071C 399B 872C"
			$"38EC FE00 1E1A 0249 7514 5C81 2212 8A20"
			$"C912 6409 2244 8A2E 4908 9244 8902 3245"
			$"92FE 001E 1A01 1021 F7C8 8122 08FB E089"
			$"F222 089E 448A 2444 0791 4489 0222 7C92"
			$"FE00 1E1A 0090 2104 0881 2204 8200 8902"
			$"2208 A244 8A24 4208 9144 8902 2240 92FE"
			$"001E 1A02 4921 1448 8122 128A 2089 1224"
			$"0922 448A 2449 0892 449D 0222 4492 FE00"
			$"1F1B 0386 70E3 9DC0 9C1C 71C1 DCE7 380E"
			$"1DEE 71CE EE07 5C38 6881 7739 FF20 FF00"
			$"06F5 0000 20F0 0006 F500 0070 F000"
		}
	} }
};

// color part of About Help dialog

resource 'PICT' (-5695, purgeable) {
	{0, 0, 142, 88},
	VersionTwo { {
		HeaderOp { Version2, 0, 0, 0x580000, 0x8E0000, 0 },
		Clip { {0, 0, 142, 88}, "" },
		PackBitsRect { 44, {0, 0, 142, 88}, 0, unpacked, 0, 0x480000, 0x480000,
			chunky, 4, 1, 4, 0, 15026132, 0x45D, 0, {
				0, 65535, 65535, 65535,
				1, 17476, 17476, 17476,
				2, 39321, 65535, 65535,
				3, 13107, 39321, 26214,
				4, 52428, 65535, 65535,
				5, 61166, 61166, 61166,
				6, 52428, 39321, 65535,
				7, 56797, 56797, 56797,
			},
			{0, 0, 142, 88}, {0, 0, 142, 88}, srcCopy,
			$"07F0 0001 1110 E800 08F1 00FE 1100 10E9"
			$"000A F200 0401 1112 1110 E900 0BF3 0005"
			$"0111 1020 2111 E900 0CF4 0006 0111 1102"
			$"0201 11E9 000B F400 0111 11FD 2000 11E9"
			$"000C F500 0111 11FC 0201 1110 EA00 0DF6"
			$"0002 1111 10FC 2001 1110 EA00 10F7 0002"
			$"0111 12FE 0204 3232 0201 11EA 0011 F800"
			$"0201 1110 FE20 0523 3330 2021 11EA 0011"
			$"F800 0411 1102 0203 FE32 0302 0201 11EA"
			$"0012 F900 0111 11FE 2002 3320 23FD 2001"
			$"1110 EB00 13FA 0002 1111 12FE 0202 3402"
			$"32FD 0201 1110 EB00 15FB 0002 0111 10FE"
			$"2004 3030 2020 23FE 2001 2110 EB00 12FC"
			$"0003 0111 1202 FD03 0042 FB02 0101 11EB"
			$"0014 FC00 0511 1120 2033 30FC 2005 2330"
			$"2020 2111 EB00 17FD 0006 1111 0202 0332"
			$"33FD 0207 0333 5203 0202 1110 EC00 16FE"
			$"0002 1111 10FD 2000 30FE 2001 2330 FC20"
			$"0111 10EC 001A 1600 0001 1112 0202 0302"
			$"3202 0203 0232 3202 0232 3202 0111 EC00"
			$"1903 0001 1110 FE20 0033 FD20 0A23 2033"
			$"2020 2330 3020 2111 EC00 180C 0111 1102"
			$"0332 0233 0402 0332 33FC 0204 3202 0201"
			$"11EC 0016 0101 11FE 2000 33FD 2000 23FB"
			$"2000 30FE 2001 1110 EC00 1704 1111 0203"
			$"03FC 0201 3304 FD02 0133 32FE 0201 1110"
			$"EC00 1803 1110 2030 FC20 0723 4040 2020"
			$"3333 23FE 2002 2111 10EC 0018 0311 1202"
			$"03FD 0203 0334 0332 FE02 0032 FE02 0201"
			$"1110 EB00 1703 1120 2023 FE20 0230 2030"
			$"FD20 0033 FE20 0221 1110 EA00 1501 1112"
			$"FE02 0732 0332 0342 0202 03FC 0201 1110"
			$"E900 150C 0111 2020 2320 2320 3320 2033"
			$"33FD 2001 1111 E800 1401 0111 FE02 0033"
			$"FC02 0132 32FE 0202 0111 10E8 0014 0500"
			$"1110 2023 30FD 2007 2320 3020 2021 1110"
			$"E700 1202 0011 12FC 0202 0302 03FE 0202"
			$"0111 10E6 000F 0200 1110 FC20 0033 FC20"
			$"0111 10E5 000F 0200 0111 FC02 0032 FD02"
			$"0111 11E4 0010 0200 0111 FD20 0123 33FE"
			$"2001 2111 E300 0F06 0000 1112 0202 33FC"
			$"0201 1110 E300 0C03 0000 1110 FA20 0121"
			$"11E2 000C 0300 0001 12FA 0201 1110 E200"
			$"0B03 0000 0111 FA20 0011 E100 0CFE 0001"
			$"1112 FC02 0101 11E1 000E FE00 0811 1020"
			$"2011 2020 2111 E100 0DFE 0000 01FD 1103"
			$"0202 1110 E100 0AFD 00FD 1102 1020 11E0"
			$"000B FC00 0501 1001 1202 11E0 0009 FA00"
			$"0301 1021 10E0 0008 F900 0211 0110 E000"
			$"07F9 0001 1111 DF00 08F9 00FE 1100 10E1"
			$"0009 F900 0301 1116 10E1 000B FB00 0511"
			$"1001 1164 11E1 000A FC00 FD11 0216 4611"
			$"E100 0DFD 0007 0111 1221 1414 1110 E100"
			$"0DFD 0007 1112 2411 4661 1411 E100 0FFE"
			$"0009 0111 2411 1664 1122 1110 E200 0FFE"
			$"0009 1112 2116 4641 1242 2111 E200 10FE"
			$"000A 1142 1146 6411 2222 4211 10E3 0010"
			$"FE00 0A11 1116 6411 1242 1122 2111 E300"
			$"10FE 000A 0111 6461 1222 2421 1422 11E3"
			$"0011 FE00 0B11 1646 1142 2422 2211 2411"
			$"10E4 0012 0E00 0001 1146 6112 2211 4224"
			$"2122 2111 E400 130F 0000 1114 6411 2224"
			$"2112 4221 1224 1110 E500 130F 0000 1166"
			$"1112 2422 2211 2242 1122 2110 E500 130F"
			$"0000 1141 1224 1122 2421 1222 4112 4211"
			$"E500 1410 0000 0111 1122 4112 4222 1122"
			$"2211 2411 10E6 0012 FD00 0C11 1222 1122"
			$"2241 2422 4112 2111 E600 12FD 000C 0111"
			$"2421 1242 2212 2422 1242 11E6 0012 FC00"
			$"0C11 4222 1222 2411 2224 2122 2110 E700"
			$"12FC 000C 0112 4221 2422 2112 2221 1421"
			$"11E7 0013 FC00 0D01 1122 4112 2242 1422"
			$"4211 4211 10E8 0012 FB00 0C11 1222 1122"
			$"2221 2224 2122 2111 E800 12FB 000C 0114"
			$"2241 1224 2214 2221 1224 11E8 0015 FA00"
			$"0B11 2222 1122 4221 2242 1241 10FB 0000"
			$"11EF 0019 FA00 0B11 1224 2142 2221 1222"
			$"2411 10FC 0000 01FE 1100 10F2 0014 FA00"
			$"0A01 1422 2112 2422 1422 1111 FB00 FA11"
			$"F400 1AF9 0009 1122 4211 2224 2221 1110"
			$"FC00 0301 1110 21FA 1100 10F8 0019 F900"
			$"0811 1224 2112 4222 1110 FB00 0101 12FD"
			$"02F9 1100 10FB 001A F900 0701 1122 2412"
			$"2211 11FA 0001 1110 FB20 0021 F911 0010"
			$"FE00 14F8 0006 1112 2242 2111 10FA 0000"
			$"11F6 02F9 1100 0016 F800 0501 1142 2211"
			$"10FA 0001 0111 F420 0021 FC11 0010 14F7"
			$"0003 1122 2111 F900 0501 1202 0203 32F5"
			$"02FD 1117 F700 0311 1411 10F9 0001 1110"
			$"FE20 0230 2023 F520 0111 1117 F700 0201"
			$"1111 F800 0011 FE02 0403 0332 3233 F602"
			$"0101 1119 F600 0011 F800 0101 11FD 2007"
			$"4030 2023 2320 3033 FA20 0111 1013 EC00"
			$"0101 12F7 0205 3303 0232 0233 FE02 0111"
			$"1015 EC00 0111 10FE 2000 30F9 2003 2330"
			$"2323 FE20 0111 1012 EC00 0011 FE02 0503"
			$"3332 3202 03F6 0201 1100 13ED 0001 0111"
			$"FB20 0433 2030 2033 F920 0221 1100 15ED"
			$"0004 0112 0202 32FC 0203 3302 3332 FA02"
			$"0201 1100 18ED 0004 1110 2020 30FB 200C"
			$"2330 2320 2330 2330 2020 2110 001A ED00"
			$"0811 0202 0302 3302 0332 FD02 0A03 3202"
			$"0333 5202 0211 1000 14EE 0001 0111 FD20"
			$"0533 2333 2020 33F7 2002 1110 000F EE00"
			$"0101 11F7 0200 32F8 0200 11FF 0014 EE00"
			$"0111 10F9 2004 2320 3020 33FB 2001 2111"
			$"FF00 13EE 0001 1112 F502 0833 3233 0232"
			$"0202 0110 FF00 10EE 0001 0111 F220 0030"
			$"FE20 0121 10FF 000D EE00 0001 FE11 F002"
			$"0311 1000 000C ED00 FE11 0010 F120 0011"
			$"FE00 10EB 0006 0112 0202 0111 12F6 0200"
			$"11FE 0013 EC00 0410 0110 2021 FD11 0010"
			$"F920 0121 11FE 0011 ED00 0411 1101 1201"
			$"F911 FB02 0111 10FE 0017 EE00 0001 FE11"
			$"0210 1111 FE00 0001 FA11 0320 2111 10FE"
			$"000D EE00 0111 14FD 11FA 00F9 11FD 0014"
			$"EF00 0611 1146 6111 1110 F700 0001 FE11"
			$"0010 FD00 11F0 0006 1111 1664 6411 11F4"
			$"0001 0111 FC00 0DF1 0002 0111 11FE 4601"
			$"1111 ED00 0EF1 0001 1111 FE64 0366 4611"
			$"10EE 000F F200 0901 1146 4666 4664 6461"
			$"11EE 0011 F300 0B01 1116 6411 4664 6466"
			$"4611 10EF 0014 F600 0E01 1110 1111 4641"
			$"1116 4646 6466 4111 EF00 13F7 0000 01FD"
			$"1101 6464 FE11 FD64 0261 1110 F000 16F7"
			$"0010 1116 4641 1646 6111 3711 1611 1646"
			$"4611 10F0 0016 F800 0201 1146 FD64 0311"
			$"1373 71FE 1103 6464 6111 F000 18F8 0012"
			$"1114 6646 6646 1117 3737 3111 3711 4666"
			$"4611 10F1 0018 F900 0701 1166 4464 6461"
			$"11FD 7307 1373 7114 6464 6111 F100 16F9"
			$"0007 1116 4646 6641 1117 FB37 0511 1646"
			$"6641 11F1 0018 FA00 0201 1146 FE64 0111"
			$"13FB 7307 7111 1646 4664 1110 F200 18FA"
			$"0006 0114 6464 6641 11FA 3708 3114 6466"
			$"4646 6111 10F3 0016 FA00 0601 1664 6646"
			$"1113 FA73 0171 11FC 6401 1111 F300 16FA"
			$"0005 0116 4664 6111 F837 0711 1646 6646"
			$"4641 11F3 0015 FA00 0401 1464 6411 F773"
			$"0771 1164 4664 6611 10F3 0016 FA00 0611"
			$"1646 4611 3735 F937 0631 1146 6646 6111"
			$"F200 18FB 0008 0111 6466 6411 1371 11FA"
			$"7301 1114 FE64 0111 10F2 0019 FB00 0511"
			$"1446 6446 61FE 1100 17FC 3706 1111 6646"
			$"4611 11F1 001A FB00 0901 1164 6664 6411"
			$"1161 13FD 7307 7111 1646 6461 1110 F100"
			$"17FA 0001 1116 FE46 0311 1646 11FC 3705"
			$"1114 6646 1111 F000 15FA 0001 0114 FD64"
			$"0214 6411 FC73 0471 1146 6111 EF00 18FA"
			$"0012 0111 6646 6646 6646 6117 3737 1137"
			$"3111 6411 10EF 0016 F900 1011 1664 4664"
			$"6411 1113 7371 1171 1116 4111 EE00 15F9"
			$"0009 0111 4666 4646 1117 3737 FD11 0264"
			$"1110 EE00 14F8 0001 1114 FE64 0911 1373"
			$"1111 6111 6461 11ED 0014 F800 0E01 1146"
			$"6646 6111 3111 1641 1646 1110 ED00 11F7"
			$"0001 1114 FE64 FE11 0464 6646 6111 EC00"
			$"12F7 000C 1111 4666 4661 1166 4664 6411"
			$"10EC 0011 F700 0101 11FE 6406 6114 6464"
			$"6641 11EB 0010 F600 0A11 1646 4664 6646"
			$"6641 1110 EB00 0FF6 0004 0111 6464 66FE"
			$"4601 1111 EA00 0EF5 0002 1116 46FE 6402"
			$"1111 10EA 000D F500 0701 1164 4666 4111"
			$"10E9 000B F400 0511 1664 6411 10E8 000A"
			$"F400 0401 1166 4111 E700 09F3 0003 1141"
			$"1110 E700 06F3 00FE 11E6 0008 F300 0201"
			$"1110 E600"
		},
		OpEndPic { }
	} }
};

// help icon as a PICT

resource 'PICT' (-5694, purgeable) {
	{0, 0, 16, 16},
	VersionTwo { {
		HeaderOp { Version2, 0, 0, 0x100000, 0x100000, 0 },
		Clip { {0, 0, 16, 16}, "" },
		PackBitsRect { 8, {0, 0, 16, 16}, 0, unpacked, 0, 0x480000, 0x480000,
			chunky, 4, 1, 4, 0, 15025960, 0x454, 0, {
				0, 65535, 65535, 65535,
				1, 0, 0, 0,
				2, 52428, 52428, 65535,
				3, 34952, 34952, 34952,
			},
			{0, 0, 16, 16}, {0, 0, 16, 16}, srcCopy,
			$"0600 00FB FF00 0007 000F FC22 0123 F009"
			$"07F2 2000 FFFF 0022 3F09 07F2 000F F00F"
			$"F002 3F09 07F2 000F F00F F002 3F09 00F2"
			$"FE00 03FF 0002 3F09 07F2 0000 0FF0 0002"
			$"3F09 07F2 0000 0FF0 0002 3F07 00F2 FC00"
			$"0102 3F09 07F2 0000 0FF0 0022 3F09 07F2"
			$"2000 0FF0 2223 3F08 000F FD22 0223 33F0"
			$"0802 00F2 3FFD FF00 0006 0200 F3F0 FC00"
			$"0501 00FF FB00 0501 00F0 FB00"
		},
		OpEndPic { }
	} }
};

data 'ics#' (kHMHelpMenuID, purgeable) {
	$"3FFC 4002 83C1 8661 8661 80C1 8181 8181"            /* ?.@.É¡ÜaÜaÄ¡ÅÅÅÅ */
	$"8001 8181 8181 4002 27FC 2800 3000 2000"            /* Ä.ÅÅÅÅ@.'.(.0. . */
	$"3FFC 7FFE FFFF FFFF FFFF FFFF FFFF FFFF"            /* ?............... */
	$"FFFF FFFF FFFF 7FFE 3FFC 3800 3000 2000"            /* ........?.8.0. . */
};

data 'ics4' (kHMHelpMenuID, purgeable) {
	$"00FF FFFF FFFF FF00 0FCC CCCC CCCC CDF0"            /* .........ÃÃÃÃÃÕ. */
	$"FCC0 00FF FF00 CCDF FC00 0FF0 0FF0 0CDF"            /* .¿....Ã......... */
	$"FC00 0FF0 0FF0 0CDF FC00 0000 FF00 0CDF"            /* ................ */
	$"FC00 000F F000 0CDF FC00 000F F000 0CDF"            /* ................ */
	$"FC00 0000 0000 0CDF FC00 000F F000 CCDF"            /* ..............Ã. */
	$"FCC0 000F F0CC CDDF 0FCC CCCC CCCD DDF0"            /* .¿...ÃÕ..ÃÃÃÃÕ.. */
	$"00FC DFFF FFFF FF00 00FD F000 0000 0000"            /* ................ */
	$"00FF 0000 0000 0000 00F0 0000 0000 0000"            /* ................ */
};

data 'ics8' (kHMHelpMenuID, purgeable) {
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF 0000"            /* ................ */
	$"00FF 2A2A 2A2A 2A2A 2A2A 2A2A 2AF9 FF00"            /* ..***********... */
	$"FF2A 2A00 0000 FFFF FFFF 0000 2A2A F9FF"            /* .**.........**.. */
	$"FF2A 0000 00FF FF00 00FF FF00 002A F9FF"            /* .*...........*.. */
	$"FF2A 0000 00FF FF00 00FF FF00 002A F9FF"            /* .*...........*.. */
	$"FF2A 0000 0000 0000 FFFF 0000 002A F9FF"            /* .*...........*.. */
	$"FF2A 0000 0000 00FF FF00 0000 002A F9FF"            /* .*...........*.. */
	$"FF2A 0000 0000 00FF FF00 0000 002A F9FF"            /* .*...........*.. */
	$"FF2A 0000 0000 0000 0000 0000 002A F9FF"            /* .*...........*.. */
	$"FF2A 0000 0000 00FF FF00 0000 2A2A F9FF"            /* .*..........**.. */
	$"FF2A 2A00 0000 00FF FF00 2A2A 2AF9 F9FF"            /* .**.......***... */
	$"00FF 2A2A 2A2A 2A2A 2A2A 2AF9 F9F9 FF00"            /* ..*********..... */
	$"0000 FF2A F9FF FFFF FFFF FFFF FFFF 0000"            /* ...*............ */
	$"0000 FFF9 FF00 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 FFFF 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 FF00 0000 0000 0000 0000 0000 0000"            /* ................ */
};

data 'ICN#' (kHMHelpID, purgeable) {
	$"7FFF FFF0 8000 0008 8000 0008 800F FF08"            /* ....Ä...Ä...Ä... */
	$"8010 0088 8020 F048 8021 9848 8021 9848"            /* Ä..àÄ .HÄ!òHÄ!òH */
	$"A020 3048 D020 604A 9020 604D 1020 0049"            /* † 0H– `Jê `M. .I */
	$"11E0 6041 1220 6041 1270 0081 1289 FF01"            /* ..`A. `A.p.Å.â.. */
	$"128A 2801 128C 2801 1288 2801 1280 2801"            /* .ä(..å(..à(..Ä(. */
	$"1280 2809 927F C80D D200 080A A200 0808"            /* .Ä(∆í.»¬“...¢... */
	$"8207 C808 8200 0808 8200 0808 8200 0808"            /* Ç.».Ç...Ç...Ç... */
	$"81FF F008 81FF F008 8000 0008 7FFF FFF0"            /* Å...Å...Ä....... */
	$"7FFF FFF0 FFFF FFF8 FFFF FFF8 FFFF FFF8"            /* ................ */
	$"FFFF FFF8 FFFF FFF8 FFFF FFF8 FFFF FFF8"            /* ................ */
	$"FFFF FFF8 DFFF FFFA 9FFF FFFF 1FFF FFFF"            /* ........ü....... */
	$"1FFF FFFF 1FFF FFFF 1FFF FFFF 1FFF FFFF"            /* ................ */
	$"1FFF FFFF 1FFF FFFF 1FFF FFFF 1FFF FFFF"            /* ................ */
	$"1FFF FFFF 9FFF FFFF DFFF FFFA FFFF FFF8"            /* ....ü........... */
	$"FFFF FFF8 FFFF FFF8 FFFF FFF8 FFFF FFF8"            /* ................ */
	$"FFFF FFF8 FFFF FFF8 FFFF FFF8 7FFF FFF0"            /* ................ */
};

data 'ics#' (kHMHelpID, purgeable) {
	$"FFFE 8002 87F2 89CA 894A C8CB 4889 4889"            /* ..Ä.á.â âJ»ÀHâHâ */
	$"55F1 5341 D243 9042 9042 9042 8F82 FFFE"            /* U.SA“CêBêBêBèÇ.. */
	$"FFFE FFFE FFFE FFFE FFFE FFFF 7FFF 7FFF"            /* ................ */
	$"7FFF 7FFF FFFF FFFE FFFE FFFE FFFE FFFE"            /* ................ */
};

data 'ics4' (kHMHelpID, purgeable) {
	$"FFFF FFFF FFFF FFF0 F000 0000 0000 00F0"            /* ................ */
	$"F00C 0FFF FFFF C0F0 F0C0 F00E EE0C FCF0"            /* ......¿..¿...... */
	$"F00C F00E 0E0C F0F0 FF00 F000 EE0C FCFF"            /* ................ */
	$"0F0C F000 E00C F0CF 0F00 FCCC ECCC FC0F"            /* .......œ...Ã.Ã.. */
	$"0F0E CFCF FFFF C0CF 0F0E C0FF CE0C 0C0F"            /* ..œœ..¿œ..¿.Œ... */
	$"FFCE CDFD CEC0 C0FF F00E C0D0 CE0C 0CF0"            /* .ŒÕ.Œ¿¿...¿–Œ... */
	$"F0CE CCCC CEC0 C0F0 F00E CCCC CE0C 0CF0"            /* .ŒÃÃŒ¿¿...ÃÃŒ... */
	$"F0C0 EEEE E0C0 C0F0 FFFF FFFF FFFF FFF0"            /* .¿...¿¿......... */
};

data 'icl4' (kHMHelpID, purgeable) {
	$"0FFF FFFF FFFF FFFF FFFF FFFF FFFF 0000"            /* ................ */
	$"F000 0000 0000 0000 0000 0000 0000 F000"            /* ................ */
	$"F0C0 C0C0 C0C0 C0C0 C0C0 C0C0 C0CC F000"            /* .¿¿¿¿¿¿¿¿¿¿¿¿Ã.. */
	$"F00C 0C0C 0C0C FFFF FFFF FFFF 0C0C F000"            /* ................ */
	$"F0C0 C0C0 C0CF CCCC CCCC CCCD F0C0 F000"            /* .¿¿¿¿œÃÃÃÃÃÕ.¿.. */
	$"F00C 0C0C 0CFC C000 FFFF 00CC DF0C F000"            /* ......¿....Ã.... */
	$"F0C0 C0C0 C0FC 000F F00F F00C DFC0 F000"            /* .¿¿¿¿........¿.. */
	$"F00C 0C0C 0CFC 000F F00F F00C DF0C F000"            /* ................ */
	$"F0F0 C0C0 C0FC 0000 00FF 000C DFC0 F000"            /* ..¿¿¿........¿.. */
	$"FF0F 0C0C 0CFC 0000 0FF0 000C DF0C F0F0"            /* ................ */
	$"F00F 00C0 C0FC 0000 0FF0 000C DFC0 FF0F"            /* ...¿¿........¿.. */
	$"000F 0C0C 0CFC 0000 0000 000C DF0C F00F"            /* ................ */
	$"000F 00CF FFFC 0000 0FF0 00CC DFC0 C0CF"            /* ...œ.......Ã.¿¿œ */
	$"000F 0CFC CCFC C000 0FF0 CCCD DF0C 0C0F"            /* ....Ã.¿...ÃÕ.... */
	$"000F 00FC CEEF CCCC CCCC CDDD FCC0 C0CF"            /* ....Œ.ÃÃÃÃÕ..¿¿œ */
	$"000F 0CFC E0D0 FCDF FFFF FFFF 0C0C 0C0F"            /* .....–.......... */
	$"000F 00FC ED0D FDFD 0D0C F0C0 C0C0 C0CF"            /* .....¬..¬..¿¿¿¿œ */
	$"000F 0CFC E0D0 FFD0 D0CC FC0C 0C0C 0C0F"            /* .....–.––Ã...... */
	$"000F 00FC ED0D FD0D 0D0C F0C0 C0C0 C0CF"            /* .....¬.¬¬..¿¿¿¿œ */
	$"000F 0CFC E0D0 D0D0 D0CC FC0C 0C0C 0C0F"            /* .....––––Ã...... */
	$"000F 00FC ED0D 0D0D 0D0C F0C0 C0C0 F0CF"            /* .....¬¬¬¬..¿¿¿.œ */
	$"F00F 0CFC C0C0 C0C0 C0CC FC0C 0C0C FF0F"            /* ....¿¿¿¿¿Ã...... */
	$"FF0F 00FC CCCC CCCC CCCC F0C0 C0C0 F0F0"            /* ....ÃÃÃÃÃÃ.¿¿¿.. */
	$"F0F0 0CFC CCCC CCCC CCCC FC0C 0C0C F000"            /* ....ÃÃÃÃÃÃ...... */
	$"F0C0 C0FC CCCC CFFF FFCC F0C0 C0C0 F000"            /* .¿¿.ÃÃœ..Ã.¿¿¿.. */
	$"F00C 0CFC 88CC C000 00CC FC0C 0C0C F000"            /* ....àÃ¿..Ã...... */
	$"F0C0 C0FC 33CC CCCC CCCC F0C0 C0C0 F000"            /* .¿¿.3ÃÃÃÃÃ.¿¿¿.. */
	$"F00C 0CFC CCCC CCCC CCCC FC0C 0C0C F000"            /* ....ÃÃÃÃÃÃ...... */
	$"F0C0 C0CF FFFF FFFF FFFF C0C0 C0C0 F000"            /* .¿¿œ......¿¿¿¿.. */
	$"F00C 0C0F FFFF FFFF FFFF 0C0C 0C0C F000"            /* ................ */
	$"F0C0 C0C0 C0C0 C0C0 C0C0 C0C0 C0C0 F000"            /* .¿¿¿¿¿¿¿¿¿¿¿¿¿.. */
	$"0FFF FFFF FFFF FFFF FFFF FFFF FFFF 0000"            /* ................ */
};
