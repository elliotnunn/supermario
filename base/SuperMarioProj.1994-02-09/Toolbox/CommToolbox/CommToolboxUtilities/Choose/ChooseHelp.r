/*
	File:		ChooseHelp.r

	Contains:	Balloon Help for choose dialog

	Written by:	Byron Han

	Copyright:	© 1990-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <3>	  2/8/91	kaz		jng, #81849: Oops - fixed help string for popup control.
		 <2>	  2/4/91	kaz		jng, #81849: Fix bubble help strings so they are nicely verbose.
		 <1>	 3/14/90	BBH		first checked in

	To Do:
*/

#include "CommToolboxPriv.r"
#include "BalloonTypes.r"


#define kHMHelpIconID -5696		

resource 'hdlg' (CTB_ChoosehdlgID) {
		HelpMgrVersion,							/* Help Version */
		0,										/* offset to first DITL */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem {},						// skip missing message
		{
			HMStringItem {					// OK
				{0,0},
				{0,0,0,0},	
				"To confirm the settings for this communications tool "
				"and close this box, click this button (or press the "
				"Return or Enter key).",
				"",
				"",
				""
			},
			HMStringItem {					// cancel
				{0,0},
				{0,0,0,0},	
				"To cancel any changes you have made to the settings "
				"in this box and close the box, click this button.",
				"",
				"",
				""
			},
			HMSkipItem {},
			HMSkipItem {},
			HMSkipItem {},
			HMSkipItem {},
			HMStringItem {					// popup
				{0,0},
				{0,0,0,0},	
				"",
				"",
				"Use this popup to select a new communications tool.",
				"Use this popup to select a new communications tool.",
			},
			HMSkipItem {},
		}
};

resource 'hmnu' (CTB_ChoosehmnuID, "Choose Menu", purgeable) {
	HelpMgrVersion,
	0,
	0,
	0,
	HMStringItem {							// Variable item popup menu
		"To select this communications tool and modify its settings "
		"choose this item.",
		"This communications tool is not currently available.",
		"This is the currently selected communications tool.  "
		"Choosing this item will have no effect below.",
		""
	},
	{
	HMSkipItem {},
	}
};

