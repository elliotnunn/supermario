/*
	File:		DAHandler.r

	Contains:	Resources for the DA Handler application

	Written by:	Phil Goldman

	Copyright:	© 1986-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <7>	 3/26/91	DFH		mcn,WS#DFH-910326a: Up the partition size from 16000 to 20000,
									since we recently stopped reducing the stack size, and we need
									to maintain the same amount of free space in the Application
									heap (all the ToolBox init calls are made in the Application
									Heap). Helps out copy/paste in QuickMail, since TEScrpHandle is
									in the Application heap.
		 <6>	  2/4/91	DFH		JWS,WS#DFH-910204a:Fix wording of 'STR ' #-16471 to say ”Apple
									Menu Items folder” instead of “Apple Menu Folder”.
		 <4>	  1/3/91	JDR		(dba) Changing the SIZE resource to use reserved instead of old
									switcher constants.
		 <3>	12/14/90	RLC		<jt> Update to J.Trumble's latest strings.
		 <2>	10/30/90	RLC		<jat> Update the 'STR#' resource to the latest strings.
		 <0>	 x/xx/86	PYG		New Today.

*/ 

#include "Types.r"
#include "BalloonTypes.r"

/* MenuIDs (not resource IDs) for the menu bar.  The MenuIDs must be positive so they
 * don't look like system menus, but the ResourceIDs must be negative to be in the 
 * range reserved for Apple.  Ergo: the ResourceID is the negative of the MenuID.
 */
#define kAppleMenuID	16473
#define kFileMenuID		16472
#define kEditMenuID		16471

/* Resource ID for HELP strings */
#define kDAStrings	-16413

/* Apple menu */
resource 'MENU' (-kAppleMenuID) {
	kAppleMenuID, textMenuProc, 0x7FFFFFFD, enabled, apple,
	{
		"About ^0…",
			noicon, nokey, nomark, plain;
		"-",
			noicon, nokey, nomark, plain
	}
};

/* File menu */
resource 'MENU' (-kFileMenuID) {
	kFileMenuID, textMenuProc, allEnabled, enabled, "File",
	 {
		"Close",
			noicon, "W", nomark, plain;
		"Quit",
			noicon, "Q", nomark, plain
	}
};

/* Edit menu */
resource 'MENU' (-kEditMenuID) {
	kEditMenuID, textMenuProc, 0x3D, enabled, "Edit",
	 {
		"Undo",
			noicon, "Z", nomark, plain;
		"-",
			noicon, nokey, nomark, plain;
		"Cut",
			noicon, "X", nomark, plain;
		"Copy",
			noicon, "C", nomark, plain;
		"Paste",
			noicon, "V", nomark, plain;
		"Clear",
			noicon, nokey, nomark, plain
	}
};

/* About Box (centered by ShowAboutMe after custom sizing) */
resource 'DLOG' (-16473, purgeable) {
	{0, 0, 97, 357},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	-16473,
	"",
	noAutoCenter
};

resource 'DITL' (-16473, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{65, 280, 85, 345},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 20, 42, 52},
		UserItem {
			disabled
		},
		/* [3] */
		{18, 62, 40, 345},
		StaticText {
			disabled,
			""
		},
		/* [4] */
		{45, 62, 45, 345},
		StaticText {
			disabled,
			""
		},
		/* [5] */
		{65, 62, 65, 345},
		StaticText {
			disabled,
			""
		}
	}
};

resource 'dctb' (-16473, purgeable) {
	{	/* array ColorSpec: 0 elements */
	}
};

resource 'STR ' (-16471, purgeable) {
	"Installed in System file.  You can drag "
	"it to the Apple Menu Items folder, or "
	"anywhere else."
};

/* "Insufficient memory" alert */
resource 'ALRT' (-16472, purgeable) {
	{70, 100, 195, 400},
	-16472,
		{
		OK,
		visible,
		sound1,
		OK,
		visible,
		sound1,
		OK,
		visible,
		sound1,
		OK,
		visible,
		sound1
		},
	alertPositionMainScreen
	};

resource 'DITL' (-16472, purgeable) {
	 {	/* array DITLarray: 3 elements */
		/* [1] */
		{92, 118, 112, 183},
		Button {
			enabled,
			"OK"
		};
		/* [2] */
		{10, 80, 75, 290},
		StaticText {
			disabled,
			"Not enough memory to open ^0.  Try closing"
			" another desk accessory or quitting an application."
		}
	}
};

/* "Couldn't find the DA" alert */
resource 'ALRT' (-16471, purgeable) {
	{70, 100, 165, 400},
	-16471,
		{
		OK,
		visible,
		sound1,
		OK,
		visible,
		sound1,
		OK,
		visible,
		sound1,
		OK,
		visible,
		sound1
		},
	alertPositionMainScreen
	};

resource 'DITL' (-16471, purgeable) {
	 {	/* array DITLarray: 2 elements */
		/* [1] */
		{62, 118, 82, 183},
		Button {
			enabled,
			"OK"
		};
		/* [2] */
		{10, 80, 45, 290},
		StaticText {
			disabled,
			"Couldn't find ^0."
		}
	}
};

/* Tell our better half (the Process Mgr) what we can do */
resource 'SIZE' (-16471) {
	reserved,
	acceptSuspendResumeEvents,
	reserved,
	canBackground,
	doesActivateOnFGSwitch,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreAppDiedEvents,
	is32BitCompatible,
	notHighLevelEventAware,
	onlyLocalHLEvents,
    notStationeryAware,
	reserved,
	reserved,
    reserved,
	reserved,
	20000,
	20000
};

/* HELP bullshit */
resource 'hmnu' (kAppleMenuID, purgeable, sysheap) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {
		},
		{
			HMSkipItem {		/* title */
			},

			HMStringResItem {	/* About… */
				kDAStrings,1,
				kDAStrings,1,
				kDAStrings,1,
				0,0
			}
		}
};

resource 'hmnu' (kFileMenuID, purgeable, sysheap) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {
		},
		{
			HMStringResItem {	/* File title */
				kDAStrings,2,
				kDAStrings,2,
				kDAStrings,2,
				0,0
			},

			HMStringResItem {	/* Close */
				kDAStrings,3,
				kDAStrings,3,
				kDAStrings,3,
				0,0
			},
			
			HMStringResItem {	/* Quit */
				kDAStrings,4,
				kDAStrings,4,
				kDAStrings,4,
				0,0
			}
		}
};

resource 'hmnu' (kEditMenuID, purgeable, sysheap) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {
		},
		{
			HMStringResItem {	/* Edit title */
				kDAStrings,5,
				kDAStrings,5,
				kDAStrings,5,
				0,0
			},

			HMStringResItem {	/* Undo */
				kDAStrings,6,
				kDAStrings,6,
				kDAStrings,6,
				0,0
			},
			
			HMSkipItem {		/* dash */
			},

			HMStringResItem {	/* Cut */
				kDAStrings,7,
				kDAStrings,7,
				kDAStrings,7,
				0,0
			},
			
			HMStringResItem {	/* Copy */
				kDAStrings,8,
				kDAStrings,8,
				kDAStrings,8,
				0,0
			},
			
			HMStringResItem {	/* Paste */
				kDAStrings,9,
				kDAStrings,9,
				kDAStrings,9,
				0,0
			},
			
			HMStringResItem {	/* Clear */
				kDAStrings,10,
				kDAStrings,10,
				kDAStrings,10,
				0,0
			},
		}
};


resource 'STR#' (kDAStrings, purgeable, sysheap) {
  {	/* [1] Apple Menu, About this DA, Normal */
		"Displays information about this desk accessory.";

	/* [2] File, Menu title, Normal */
		"File Menu\n\nUse this menu to close windows belonging to a desk accessory or to quit "
		"the desk accessory.";

	/* [3] File, Close, Normal */
		"Closes the active window.";

	/* [4] File, Quit, Normal */
		"Quits the active desk accessory.";

	/* [5] Edit, Menu title, Normal */
		"Edit Menu\n\nUse this menu to work with text and graphics, or to undo an "
		"action.";

	/* [6] Edit, Undo, Normal */
		"Undoes your last action. You can use this command to put back material you "
		"have cut or cleared or to remove material you have pasted or typed.";

	/* [7] Edit, Cut, Normal */
		"Cuts the selected text or graphics.  The material you cut is removed and "
		"placed temporarily into a storage area called the Clipboard.";

	/* [8] Edit, Copy, Normal */
		"Copies the selected text or graphics.  The original selection remains "
		"where it was. The copy is placed temporarily into a storage area called "
		"the Clipboard.";

	/* [9] Edit, Paste, Normal */
		"Inserts the contents of the Clipboard at the location of the insertion "
		"point.";

	/* [10] Edit, Clear, Normal */
		"Removes the selected text or graphics without storing it on the Clipboard.";
  }
};

/* Add code resources created by the linker */
include $$Shell("CodeResFile");
