/*
	File:		Choose.r

	Contains:	Resources for the Tool Choose Package

	Written by:	Byron Han and Alex Kazim

	Copyright:	© 1989-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<12>	 10/1/91	JSM		Don’t use SystemSevenOrLater, it always is.
		<11>	12/18/90	stb		& JDR; change SystemSevenOrBetter to SystemSevenOrLater
		<10>	 9/10/90	JNG		Add alert if fail to open a tool
		 <9>	 8/15/90	kaz		Centering shutdown alert.
		 <8>	 6/12/90	kaz		Adjusted DLOG & CNTL item rects for shrink-to-fit dialog.
		 <7>	  6/7/90	DC		Fixed up ALRT's and DLOG's for auto-positioning
		 <6>	 5/24/90	kaz		Removed 'finf' resource in lieu of 'flst'
		 <5>	 5/22/90	kaz		Added textMode to 'flst' resource
		 <4>	 3/22/90	RLC		Update to new helpItem format in Types.r 'DITL'.
		 <3>	 3/16/90	BBH		fix constants so that this will compile
		 <2>	 3/16/90	BBH		fix constants so that this will compile
		 <1>	 3/14/90	BBH		first checked in to BBS

	Pre-BBS Modification History
		1/31/90		kaz		Changed the rectangles for the Choose DLOG/DITL
								for shrink to fit
		1/29/90		BBH		Added balloon help stuff
		9/28/89		kaz		Changed the wording in the DLOG for breaking
								the current connection
		9/18/89		BBH		Changed COnfiguration to Settings
		9/14/89		BBH		Modified ReallyShutdown alert to add icon.
		7/10/89		BBH		remove names from resources.  made most rsrcs purgeable		
		6/6/89		BBH		Reversed OK and Cancel buttons in Choose dialog	
		5/24/89		BBH		Changed baseID from -10240 to -10176
		5/2/89		BBH		Changed ID from 9990 to -3991
		4/25/89		BBH		Resized the close connection confirmation dialog
*/


#include "SysTypes.r"
#include "Types.r"
#include "CTBTypes.r"
#include "CommToolboxPriv.r"

resource 'flst' (CTB_ChoosefinfID, purgeable) {
	{	/* array Fonts: 2 elements */
		/* [1] */						/* Window Font	*/
		"Geneva",		
		plain,
		9,
		1,
		/* [2] */						/* Icon Font	*/
		"Geneva",		
		plain,
		9,
		1
	}
};

/* 	standard strings */
resource 'STR#' (CTB_ChooseSTRsID, purgeable) {			
	{
	"Connection Settings",
	"Terminal Settings",
	"File Transfer Settings",
	"Method:",
	"Emulation:",
	"Protocol:",
	".",
	}
};

/*	setup dialog */
resource 'DLOG' (CTB_ChooseDLOGID, purgeable) {
	{0, 0, 70, 350},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	CTB_ChooseDITLID,
	"",						// Setup Dialog Box
	noAutoCenter

};

/* 	confirm closing connection */
resource 'DLOG' (CTB_ChooseConfirmDLOGID, purgeable) {
	{0, 0, 110, 365},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	CTB_ChooseConfirmDITLID,
	"",						// confirmation dialog
	alertPositionMainScreen
};

/* 	Tool unavailable */
resource 'DLOG' (CTB_ChooseUnavailableDLOGID, purgeable) {
	{0, 0, 110, 365},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	CTB_ChooseUnavailableDITLID,
	"",						// confirmation dialog
	alertPositionMainScreen
};

/*	dialog color table */
resource 'dctb' (CTB_ChoosedctbID, purgeable) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 65535, 65535, 65535,
		/* [2] */
		wFrameColor, 0, 0, 0,
		/* [3] */
		wTextColor, 0, 0, 0,
		/* [4] */
		wHiliteColor, 0, 0, 0,
		/* [5] */
		wTitleBarColor, 65535, 65535, 65535
	}
};

/* tools control */
resource 'CNTL' (CTB_ChooseCNTLID, purgeable) {
	{30, 25, 50, 250},
	popupTitleLeftJust,
	visible,
	90,					/* width of title */
	CTB_ChooseMENUID,	/* menu associated */
	popupMenuCDEFproc,	/* CDEF 10 = 16 * 10 + variation code */
	0,					/* reference menu 11000, popup title width 50 */
	""					// title:
};

/*	standard config DITL */
resource 'DITL' (CTB_ChooseDITLID, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{5, 262, 25, 332},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{32, 262, 52, 332},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] outline of OK button */
		{1, 258, 29, 336},
		UserItem {
			enabled
		},
		/* [4] title */
		{5, 5, 21, 200},
		userItem {
			disabled
		},
		/* [5] version */
		{50, 85, 60, 130},
		UserItem {
			disabled
		},
		/* [6] dotten line separating static from dynamic portions */
		{62, 0, 63, 335},
		UserItem {
			disabled
		},
		/* [7] select tool popup menu user item */
		{30, 25, 50, 250},
		Control {
			enabled,
			CTB_ChooseCNTLID,
		},
		// 9 help icon 
		{0, 0, 0, 0},				// this used to be the help icon
		UserItem {
			disabled,
		},
		// 10 help item
		{0, 0, 0, 0},
		HelpItem {
			enabled,
			HMScanhdlg {
				CTB_ChoosehdlgID
			}
		},
	}
};

/*	confirmation dialog DITL */
resource 'DITL' (CTB_ChooseConfirmDITLID, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{80, 280, 100, 350},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{80, 200, 100, 270},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{76, 276, 104, 354},
		UserItem {
			enabled
		},
		/* [4] */
		{10, 55, 70, 350},
		StaticText {
			disabled,
			"Changing the connection tool will close "
			"the current connection.  Are you sure you "
			"want to do this?"
		},
		/* [5] */
		{10, 10, 42, 42},
		Icon {
			enabled,
			2
		}
	}
};
/*	confirmation dialog DITL */
resource 'DITL' (CTB_ChooseUnavailableDITLID, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{80, 280, 100, 350},
		Button {
			enabled,
			"OK"
		},
		/* [3] */
		{76, 276, 104, 354},
		UserItem {
			enabled
		},
		/* [4] */
		{10, 55, 70, 350},
		StaticText {
			disabled,
			"The selected tool is unavailable.  "
			"Reverting to the previous tool."
		},
		/* [5] */
		{10, 10, 42, 42},
		Icon {
			enabled,
			2
		}
	}
};

/*	popup menu for tools */
resource 'MENU' (CTB_ChooseMENUID) {
	CTB_ChooseMENUID,
	textMenuProc,
	allEnabled,
	enabled,
	"",									// Choose Menu
	{
	}
};
