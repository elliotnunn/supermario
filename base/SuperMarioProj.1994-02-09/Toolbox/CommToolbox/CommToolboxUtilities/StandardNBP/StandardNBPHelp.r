/*
	File:		StandardNBPHelp.r

	Contains:	Bubble Help resources for StandardNBP dialog

	Written by:	Byron Han

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <4>	  2/4/91	kaz		jng, #81849: Fix bubble help strings so they are nicely verbose.
		 <3>	 5/17/90	kaz		Fixing Bubble Help
		 <2>	 3/16/90	BBH		made it compile
		 <1>	 3/14/90	BBH		first checked in

	To Do:
*/



#include "BalloonTypes.r"
#include "CommToolboxPriv.r"

resource 'hdlg' (CTB_NulookuphdlgID) {
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
				"To confirm the selection of this Appletalk item "
				"and close the box, click this button (or press "
				"the Return or Enter key).",
				"You cannot click here because no Appletalk item "
				"is selected in the list above.",
				"",
				""
			},
			HMStringItem {					// cancel
				{0,0},
				{0,0,0,0},	
				"To cancel any changes you have made in this box "
				"and close the box, click this button.",
				"",
				"",
				""
			},
			HMSkipItem {},
			HMSkipItem {},
			HMStringItem {					// item list
				{0,0},
				{0,0,0,0},	
				"This list displays all of the requested Appletalk items "
				"in the selected Appletalk zone.  To select one of "
				"the items, click on the name in this list.",
				"",
				"",
				""
			},
			HMSkipItem {},
			HMStringItem {					// zone list
				{0,0},
				{0,0,0,0},	
				"This list displays all of the Appletalk zones on "
				"this network.  To select a new zone click on the "
				"name in this list.",
				"",
				"",
				""
			},
			HMSkipItem {},
			HMSkipItem {},
			HMSkipItem {},
			HMSkipItem {},
			HMSkipItem {},
			HMSkipItem {}
		}
};
