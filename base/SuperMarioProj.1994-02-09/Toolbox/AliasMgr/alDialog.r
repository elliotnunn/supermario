/*
	File:		alDialog.r

	Contains:	resource file for alias manager

	Written by:	Prashant Patel

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<10>	 1/31/91	PP		ich,#WS ich-AMgr-0092:Add string for Unknown Name. (Projector
									problems between 9 1nd 10)
		 <9>	 1/31/91	PP		ich,#WS ich-AMgr-0092:Add string for Unknown Name.
		 <8>	10/10/90	PP		Add consts for AFPServer and NoUserAuthent strings.
		 <7>	 8/18/90	dba		use auto-centering for the alert and the dialog; also set the
									sysHeap bit for all resources in this file
		 <6>	 7/19/90	PP		Remove resources of AlreadyConnected dialog. Remove string for
									Not Enough Pribvileges alert.
		 <5>	 6/28/90	PP		Mount volume dailog related wording is different.
		 <4>	 6/15/90	PP		"SelectAlias" dialog and related resources are removed (feature
									has been removed).
		 <3>	  6/7/90	DC		Fix up existing ALRT, DLOG and WIND templates for
									auto-positioning
		 <2>	 3/20/90	PP		Put resources for "Already connected to server" dialog.
	   <1.7>	11/27/89	prp		Add SwitchDisk dialog for Ejectable floppies.
	   <1.6>	11/16/89	prp		Cancel dialog is with 'Stop Search' button instead of
									cmd-period.
	   <1.5>	10/17/89	prp		Alias Resolution dialog Rect is changed so that it fits
									correctly on original Mac screen.
	   <1.4>	10/13/89	prp		Add resource for Folder Selection dialog.
	   <1.3>	 10/2/89	prp		The title of 'SelectAlias' dialog box is different.
	   <1.2>	 9/18/89	prp		Additional STR# resource and SICN resource for Desktop.
	   <1.1>	  9/6/89	prp		Changes from CodeReview.
	   <1.0>	  8/7/89	prp		Initial Creation

	To Do:
*/

#include "Types.r"
#include "aliasPriv.h"
#include "aliasDlogPriv.h"

/* AppleShare Icon*/
resource 'ICON' (kAppleShareICONID, sysheap) {
	$"00 01 00 00 00 02 9F E0 00 04 50 30 00 08 30 28"
	$"00 10 10 3C 07 A0 08 04 18 7F 84 04 10 00 82 04"
	$"10 00 81 04 10 00 82 04 10 00 84 04 10 00 88 04"
	$"10 00 90 04 10 00 B0 04 10 00 D0 04 FF FF FF FF"
	$"40 00 00 02 3F FF FF FC 01 2A 05 00 00 AA 05 00"
	$"00 AB 05 00 00 65 DD 00 00 22 37 00 00 19 09 00"
	$"00 0C 05 00 00 03 03 00 00 00 C1 00 00 00 21 00"
	$"00 00 21 00 00 00 7F 80 00 00 7F 80 00 00 7F 80"
};

/* Various strings used by the alias manager. */
resource 'STR#' (kAliasResID, sysHeap, purgeable) { {
	"Sorry, your password is incorrect.  Please try again.";
	"The user name “";
	"” is not valid.";
	"Connect to shared disk “";
	"” as:";
	"AFPServer";
	"No User Authent";
	"Unknown Name"
} };

/* Alert box used by Alias Manager to notify user of various error conditions. */
resource 'ALRT' (kAlAlertID, sysHeap, purgeable) {
	{53, 78, 198, 423}, kAlAlertID,
	{
		OK, visible, sound1,
		OK, visible, sound1,
		OK, visible, sound1,
		OK, visible, sound1
	},
	alertPositionMainScreen
};

resource 'DITL' (kAlAlertID, sysHeap, purgeable) { {
	{117, 132, 137, 212}, Button { enabled, "OK" },
	{10, 65, 107, 340}, StaticText { disabled, "^0" }
} };

/*
	Following are support resources for putting up a DiskSwitch dialog
	for ejectable volumes that are unmounted and off-line.
*/

resource 'ICON' (kAlSwitchDiskID, sysHeap, purgeable) {
	$"00 2A DE 00 00 80 01 00 00 00 00 80 01 00 00 80"
	$"00 00 03 E0 01 00 01 C0 00 00 00 80 00 00 00 00"
	$"FF F8 3F FC 88 A4 44 52 88 A2 44 51 88 22 44 11"
	$"8F E2 47 F1 80 02 40 01 80 02 40 01 9F F2 4F F9"
	$"A0 0A 50 05 A0 0A 50 05 A0 0A 50 05 A0 0A 50 05"
	$"A0 0A 50 05 A0 0A 50 05 FF FE 7F FF 00 00 00 00"
	$"01 00 00 00 03 80 00 80 07 C0 00 00 01 00 00 80"
	$"01 00 00 00 00 80 01 00 00 7B 54"
};

resource 'DLOG' (kAlSwitchDiskID, sysHeap, purgeable) {
	{148, 176, 236, 436}, dBoxProc, visible, noGoAway, 0x0, kAlSwitchDiskID, "",
	alertPositionMainScreen
};

resource 'DITL' (kAlSwitchDiskID, sysHeap, purgeable) { {
	{15, 15, 47, 47}, Icon { disabled, kAlSwitchDiskID },
	{15, 56, 31, 208}, StaticText { disabled, "Please insert the disk:" },
	{35, 56, 51, 248}, StaticText { disabled, "^0" },
	{55, 184, 75, 245}, Button { enabled, "Cancel" }
} };
