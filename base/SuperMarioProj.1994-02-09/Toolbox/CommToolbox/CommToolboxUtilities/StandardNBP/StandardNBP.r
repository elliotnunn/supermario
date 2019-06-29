/*
	File:		StandardNBP.r

	Contains:	Resources for StandardNBP dialog

	Written by:	Byron Han

	Copyright:	© 1989-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<19>	 10/1/91	JSM		Don’t use SystemSevenOrLater, it always is.
		<18>	 3/29/91	kaz		Final version string!!
		<17>	  2/5/91	kaz		jng: Updating version strings
		<16>	 1/10/91	kaz		<jng>Updating version string
		<15>	12/18/90	stb		& JDR; change SystemSevenOrBetter to SystemSevenOrLater
		<14>	12/10/90	kaz		b3 version strings <BBH>
		<13>	 11/6/90	kaz		Changing version string to b2.
		<12>	  8/5/90	kaz		Version string for a3
		<11>	  7/2/90	kaz		Version Alpha!!!
		<10>	 6/24/90	kaz		Updated version 'PICT' to 1.0.1 scheme.
		 <9>	 6/22/90	dba		move include of Pict.r before Types.r, since Types.r is now
									polite, and will respect the PICT definition from Pict.r
		 <8>	  6/7/90	DC		fix up ALRTs and DLOGs for auto-positioning
		 <7>	 5/22/90	kaz		Added textmode to 'flst' resource
		 <6>	 5/17/90	kaz		Fixing Bubble Help. Cleaned up Version 'PICT' Added 'flst'.
									Removed parms.
		 <5>	 5/15/90	kaz		Added version # as a PICT
		 <4>	 3/27/90	BBH		re-add zone list title and change version to 7.0d3
		 <3>	 3/22/90	BBH		update DITL to reflect latest help item format in Types.r
		 <2>	 3/16/90	BBH		made it compile
		 <1>	 3/14/90	BBH		first checked in

	Pre-BBS History:

		8/14/89		BBH		Changed outline item in nulookup DITL to be outset 3 pixels
								from the OK button rectaqngle
		8/25/89		BBH		Changed about Window to be invisible initially.
		8/29/89		BBH		Added dctb
		9/2/89		BBH		Disable outline userItem in nulookup dlog
		9/13/89		BBH		Changed WIND to plainDBox from dBoxProc
		9/15/89		BBH		Changed version to b9
		9/18/89		BBH		Changed statText for itemTitle and zoneTitle to 16 pixels high
		9/20/89		BBH		Changed b9 to b10
		9/25/89		JNG		Changed b10 to b11
		9/27/89		kaz		Changing the layout of NuLookup DITL per HI requests
		10/2				Made item list text and list widths equal
							Made zone list text and list widths equal
							Moved version string below zone list and made width equal to zone item list
							Reversed OK and Cancel (OK in bottom right corner)
							Moved OK/Cancel buttons so that OK button outline right edge lines up with
								left edge of the scrollbar in the itemlist
							Vertical line now starts at top of the item/zone lists
		10/5/89		BBH		Changed to b13
		10/13/89	BBH		Added switch for WHEATIES/BILL version #
		11/8/89		BBH		Changed 1.0f1 to 1.0
		11/14/89	JNG		Changed 1.1d0 to 1.1d2
		2/13/90		kaz		Added help icon & item to DITL
*/


#include "PICT.r"
#include "Types.r"
#include "CommToolboxPriv.r"
#include "CTBTypes.r"


resource 'DLOG' (CTB_NulookupDLOGID, purgeable) {
	{0, 0, 207, 400},
	dBoxProc,
	inVisible,
	noGoAway,
	0x0,
	CTB_NulookupDITLID,
	"",								// LookUp DLOG
	noAutoCenter
};

/*	dialog color table */
resource 'dctb' (CTB_NulookupdctbID, purgeable) {
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

/* Default DITL */
resource 'DITL' (CTB_NulookupDITLID, purgeable) {
	{	/* array DITLarray: 13 elements */
		/* [1] OK Button */
		{172, 300, 192, 370},
		Button {
			enabled,
			"OK"
		},
		/* [2] cancel button */
		{172, 220, 192, 290},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] outlining for OK button */
		{168, 296, 196, 374},
		UserItem {
			disabled
		},
		/* [4] item list title */
		{6, 200, 22, 390},
		StaticText {
			disabled,
			""								// Title goes here…
		},
		/* [5] items list */
		{29, 200, 161, 390},
		UserItem {
			enabled
		},
		/* [6] zones list title */
		{6, 10, 22, 176},
		StaticText {
			disabled,
			"AppleTalk Zones:"				// Appletalk Zones:
		},
		/* [7] zones list */
		{29, 10, 193, 176},
		UserItem {
			enabled
		},
		/* [8] vertical line */
		{29, 187, 195, 188},
		UserItem {
			disabled
		},
		/* [9] version string */
#ifdef Pre70
		{194, 10, 207, 176},
#else
		{197, 10, 207, 176},			// need more room for focus
#endif Pre70
		UserItem {
			enabled
		},
		/* [10] reserved */
		{0, 0, 1, 1},
		UserItem {
			disabled
		},
		/* [11] reserved */
		{0, 0, 1, 1},
		UserItem {
			disabled
		},
		/* [12] reserved */
		{0, 0, 1, 1},
		UserItem {
			disabled
		},
		/* [13] reserved */
		{0, 0, 1, 1},
		UserItem {
			disabled
		},
#ifndef Pre70
		/* [14] hdlg ref 	*/
		{0, 0, 0, 0},
		HelpItem {
			enabled,
			HMScanhdlg {
				CTB_NulookuphdlgID
			},
		},
#endif
	}
};

/* Version String PICT	*/
resource 'PICT' (CTB_NulookupPICTID, purgeable) {
	{-1, -1, 13, 36},
	VersionOne {
		{	/* array OpCodes: 4 elements */
			/* [1] */
			clipRgn {
				{0, 0, 12, 35},
				$""
			},
			/* [2] */
			txFont {
				3
			},
			/* [3] */
			txSize {
				9
			},
			/* [4] */
			dhdvText {
				1,
				8,
				"1.0.1"
			},
		}
	}
};

resource 'flst' (CTB_NuLookupParmsID, purgeable) {
	{	/* array Fonts: 1 element */
		/* [1] */						/* Window Font	*/
		"Geneva",
		plain,
		12,
		1								/* srcOr	*/
	}
};