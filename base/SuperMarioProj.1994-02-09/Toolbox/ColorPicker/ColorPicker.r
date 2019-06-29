/*
	File:		ColorPicker.r

	Contains:	Resources for the color picker package

	Copyright:	© 1987-1992 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

	   <SM2>	 4/30/92	TN		Included Types.r and BalloonTypes.r for ROM build.
		<11>	  3/4/91	dba		dty: get rid of SysVers conditionals
		<10>	 2/26/91	RLC		pjh - Roll in new strings from final edit.
		 <9>	 1/18/91	gbm		(csd) Make prompt string just a teensy bit wider.
		 <8>	12/18/90	stb		& JDR; change SystemSevenOrBetter to SystemSevenOrLater
		 <7>	12/14/90	RLC		<jt> Update to J.Trumble's latest strings.
		 <7>	12/14/90	RLC		<jt> Update to J.Trumble's latest strings.
		 <6>	10/12/90	dvb		Remove color arrows
		 <5>	  9/4/90	dvb		Add whizzy color arrows
		<3+>	 7/25/90	RLC		Update balloon help strings to latest J.Trumble strings.
		 <3>	  6/7/90	DC		Fix up ALRT's and DLOG's for auto-positioning
		 <2>	 4/25/90	RLC		Add BalloonHelp resources matching DITL.
	   <1.2>	  8/4/89	CSD		Changed window title (and thus, the displayed vers number) to
									1.5
	  <•1.1>	 7/24/89	CSD		Forcing submission to upgrade to the 32-Bit QuickDraw Color
									Picker. Also, removed the include of ColorPickerWedge.rsrc and
									PixPat4colors.rsrc--These are now Rez'ed with Sys.r

	To Do:
*/

#include "Types.r"
#include "BalloonTypes.r"

resource 'DLOG' (0xE980,purgeable) {
	{0, 0, 282, 430}, dBoxProc, invisible, noGoAway, 0x0, 0xE980, "", noAutoCenter
};

data 'dctb' (0xE980,purgeable) {
	/* Signals that ours is a color dialog. */
	$"0000 0000 0000 0004"
	$"0000 FFFF FFFF FFFF"
	$"0001 0000 0000 0000"
	$"0002 0000 0000 0000"
	$"0003 0000 0000 0000"
	$"0004 FFFF FFFF FFFF"
};

resource 'CNTL' (0xE980,purgeable) {
	/* The brightness scroll bar */
	{20, 440, 220, 456}, 0, visible, 32767, -32768, scrollBarProc, 0, ""
};

data 'PICT' (0xE980,purgeable) {
	/* Inc/dec control picture */
	$"0059"										/* length */
	$"0086 0110 0098 011B"						/* frame */
	$"1101 0100 0A8A D08A D075 3075 3090 0002"
	$"00FE 0160 0110 0170 00FE 0160 0110 016B"
	$"0086 0110 0098 011B 0000 3F80 4040 8420"
	$"8E20 9F20 BFA0 8E20 8E20 8020 8020 8E20"
	$"8E20 BFA0 9F20 8E20 8420 4040 3F80 FF"
};

#if 0
/* color arrow inc/dec control */

data 'PICT' (0xE981,purgeable) {
	$"01A2"										/* length */
	$"0000 0000 0012 000B"						/* frame */
	$"0011 02FF 0C00"
	$"FFFF FFFF 0000 0000 0000 0000 000B 0000"
	$"0012 0000 0000 0000 00A1 0064 002E 5354"
	$"2F38 0000 0012 0000 7FC0 FFE0 FFE0 FFE0"
	$"FFE0 FFE0 FFE0 FFE0 FFE0 FFE0 FFE0 FFE0"
	$"FFE0 FFE0 FFE0 FFE0 FFE0 7FC0 001E 0001"
	$"000A 0000 0000 0012 000B 0090 8006 0000"
	$"0000 0012 000B 0000 0001 0000 0000 0048"
	$"0000 0048 0000 0000 0004 0001 0004 0000"
	$"0000 0000 1F10 0000 0000 0000 24C0 8000"
	$"000F 0000 FFFF FFFF FFFF 0000 FC00 F37D"
	$"052F 0000 FFFF 648A 028C 0000 DD6B 08C2"
	$"06A2 0000 F2D7 0856 84EC 0000 46E3 0000"
	$"A53E 0000 0000 0000 D400 0000 0241 AB54"
	$"EAFF 0000 1F21 B793 1431 0000 0000 64AF"
	$"11B0 0000 5600 2C9D 0524 0000 CCCC CCCC"
	$"FFFF 0000 C000 C000 C000 0000 8000 8000"
	$"8000 0000 4000 4000 4000 0000 0000 0000"
	$"0000 0000 0000 0012 000B 0000 0000 0012"
	$"000B 0000 0FFF FFFF FF05 FCCC CCCC CDFB"
	$"FCDD DBDD DEF0 FCDD BBBD DEF0 FCDB BBBB"
	$"DEF0 FCBB BBBB BEF0 FCDD BBBD DEF0 FCDD"
	$"BBBD DEF0 FCDD DDDD DEF0 FCDD DDDD DEF0"
	$"FCDD BBBD DEF0 FCDD BBBD DEFF FCBB BBBB"
	$"BEF1 FCDB BBBB DEF0 FCDD BBBD DEF2 FCDD"
	$"DBDD DEF0 FDEE EEEE EEF0 0FFF FFFF FF00"
	$"00FF"
};
#endif

resource 'DITL' (0xE980,purgeable) {
	{	/* array DITLarray: 28 elements */
		/* [1] */
		{250, 302, 270, 362}, Button { enabled, "OK" };
		/* [2] */
		{250, 226, 270, 286}, Button { enabled, "Cancel" };
		/* [3] */
		/* {264, 242, 284, 302}, */
		{264, 8242, 284, 8302}, Button { enabled, "Model" };
		/* [4] */
		/* User-supplied prompt string */
		{13, 15, 33, 240}, StaticText { disabled, "" };
		/* [5] */
		/* Brightness control (scroll bar) */
		{30, 400, 230, 416}, Control { enabled, 0xE980 };
		/* [6] */
		/* OK button outline */
		{246, 298, 274, 366}, UserItem { disabled };
		/* [7] */
		/* Hue/saturation wheel */
		{30, 185, 230, 385}, UserItem { enabled };
		/* [8] */
		/* Sample of color being picked */
		{40, 50, 75, 130}, UserItem { disabled };
		/* [9] */
		/* Sample of input color (one shorter than output color, */
		/* because input paints over midline the samples share) */
		{75, 50, 109, 130}, UserItem { disabled };
		/* [10] */
		/* Version number for color picker (text is snagged from */
		/* the title of the color picker dialog) */
		{4, 382, 14, 412}, UserItem { disabled };
		/* [11] */
		/* Hue component increment/decrement control */
		{128, 150, 146, 161}, UserItem { enabled };
		/* [12] */
		/* Saturation component increment/decrement control */
		{149, 150, 167, 161}, UserItem { enabled };
		/* [13] */
		/* Lightness component increment/decrement control */
		{170, 150, 188, 161}, UserItem { enabled };
		/* [14] */
		/* Red component increment/decrement control */
		{201, 150, 219, 161}, UserItem { enabled };
		/* [15] */
		/* Green component increment/decrement control */
		{222, 150, 240, 161}, UserItem { enabled };
		/* [16] */
		/* Blue component increment/decrement control */
		{243, 150, 261, 161}, UserItem { enabled };
		/* [17] */
		{130, 55, 144, 89}, StaticText { disabled, "Hue" };
		/* [18] */
		{151, 9, 165, 89}, StaticText { disabled, "Saturation" };
		/* [19] */
		{172, 9, 186, 89}, StaticText { disabled, "Brightness" };
		/* [20] */
		{203, 57, 217, 89}, StaticText { disabled, "Red" };
		/* [21] */
		{224, 43, 238, 89}, StaticText { disabled, "Green" };
		/* [22] */
		{245, 53, 259, 89}, StaticText { disabled, "Blue" };
		/* [23] */
		/* Hue component entry */
		{129, 91, 145, 144}, EditText { disabled, "0" };
		/* [24] */
		/* Saturation component entry */
		{150, 91, 166, 144}, EditText { disabled, "0" };
		/* [25] */
		/* Lightness/brightness component entry */
		{171, 91, 187, 144}, EditText { disabled, "0" };
		/* [26] */
		/* Red component entry */
		{202, 91, 218, 144}, EditText { disabled, "0" };
		/* [27] */
		/* Green component entry */
		{223, 91, 239, 144}, EditText { disabled, "0" };
		/* [28] */
		/* Blue component entry */
		{244, 91, 260, 144}, EditText { disabled, "0" };

		{-1,-1,-1,-1}, HelpItem { disabled, HMScanhdlg { 0xE980 } }
	}
};

resource 'hdlg' (0xE980,purgeable) {
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
				0xE981,11,
				0xE981,11,
				0xE981,11,
				0xE981,11
			},
			HMStringResItem {	/* Cancel button */
				{0,0},
				{0,0,0,0},
				0xE981,10,
				0xE981,10,
				0xE981,10,
				0xE981,10
			},
			HMSkipItem {		/* Model button */
			},
			HMSkipItem {		/* User Supplied prompt static text */
			},
			HMStringResItem {	/* Brightness control scroll bar */
				{0,0},
				{0,0,0,0},
				0xE981,9,
				0xE981,9,
				0xE981,9,
				0xE981,9
			},
			HMSkipItem {		/* OK button outline */
			},

			HMStringResItem {	/* Wheel user item */
				{0,0},
				{0,0,0,0},
				0xE981,8,
				0xE981,8,
				0xE981,8,
				0xE981,8
			},
			HMStringResItem {	/* Sample area 1 user item */
				{0,0},
				{0,0,0,0},
				0xE981,1,
				0xE981,1,
				0xE981,1,
				0xE981,1
			},
			HMStringResItem {	/* Sample area 2 user item */
				{0,0},
				{0,0,0,0},
				0xE981,1,
				0xE981,1,
				0xE981,1,
				0xE981,1
			},
			HMStringItem {		/* Version string user item */
				{0,0},
				{0,0,0,0},
				"",
				"",
				"",
				""
			},
			HMStringResItem {	/* Hue Component control user item */
				{0,0},
				{0,0,0,0},
				0xE981,2,
				0xE981,2,
				0xE981,2,
				0xE981,2
			},
			HMStringResItem {	/* Saturation Component control user item */
				{0,0},
				{0,0,0,0},
				0xE981,3,
				0xE981,3,
				0xE981,3,
				0xE981,3
			},
			HMStringResItem {	/* Lightness Component control user item */
				{0,0},
				{0,0,0,0},
				0xE981,4,
				0xE981,4,
				0xE981,4,
				0xE981,4
			},
			HMStringResItem {	/* Red Component control user item */
				{0,0},
				{0,0,0,0},
				0xE981,5,
				0xE981,5,
				0xE981,5,
				0xE981,5
			},
			HMStringResItem {	/* Green Component control user item */
				{0,0},
				{0,0,0,0},
				0xE981,6,
				0xE981,6,
				0xE981,6,
				0xE981,6
			},
			HMStringResItem {	/* Blue Component control user item */
				{0,0},
				{0,0,0,0},
				0xE981,7,
				0xE981,7,
				0xE981,7,
				0xE981,7
			},
			HMSkipItem {		/* Hue static text */
			},
			HMSkipItem {		/* Saturation static text */
			},
			HMSkipItem {		/* Brightness static text */
			},
			HMSkipItem {		/* Red static text */
			},
			HMSkipItem {		/* Green static text */
			},
			HMSkipItem {		/* Blue static text */
			},
			HMStringResItem {	/* Hue Component control edit item */
				{0,0},
				{0,0,0,0},
				0xE981,2,
				0xE981,2,
				0xE981,2,
				0xE981,2
			},
			HMStringResItem {	/* Saturation Component control edit item */
				{0,0},
				{0,0,0,0},
				0xE981,3,
				0xE981,3,
				0xE981,3,
				0xE981,3
			},
			HMStringResItem {	/* Lightness Component control edit item */
				{0,0},
				{0,0,0,0},
				0xE981,4,
				0xE981,4,
				0xE981,4,
				0xE981,4
			},
			HMStringResItem {	/* Red Component control edit item */
				{0,0},
				{0,0,0,0},
				0xE981,5,
				0xE981,5,
				0xE981,5,
				0xE981,5
			},
			HMStringResItem {	/* Green Component control edit item */
				{0,0},
				{0,0,0,0},
				0xE981,6,
				0xE981,6,
				0xE981,6,
				0xE981,6
			},
			HMStringResItem {	/* Blue Component control edit item */
				{0,0},
				{0,0,0,0},
				0xE981,7,
				0xE981,7,
				0xE981,7,
				0xE981,7
			},
		}
	};

resource 'STR#' (0xE981, purgeable) {	/* Balloon Help Strings */
	{
	/* [84] Color picker, color box (upper-left):both top and bottom, Normal */
		"The top half of this box displays the new color you have chosen.  The "
		"bottom half displays the original color. To revert to the original color, "
		"click the bottom half of this box.";

	/* [78] Color picker, Hue box and control, Normal */
		"This box displays the hue setting for the current color (the current "
		"mixture of red, green, and blue).  To change the hue, type a new number "
		"into this box or click the arrows.";

	/* [79] Color picker, Saturation box and controls, Normal */
		"This box displays the saturation setting for the current color (the "
		"average intensity of red, green, and blue).  To change the saturation, "
		"type a new number into this box or click the arrows.";

	/* [80] Color picker, brightness box and controls, Normal */
		"This box displays the brightness setting for the current color (the amount "
		"of light given off by the color). To change the brightness, type a new "
		"number in this box, click the arrows, or use the scroll bar to the right "
		"of the color wheel.";

	/* [81] Color picker, Red box and controls, Normal */
		"This box displays the amount of red in the current color. To change the "
		"amount of red, type a new number into this box or click the arrows.";

	/* [82] Color picker, green box and controls, Normal */
		"This box displays the amount of green in the current color. To change the "
		"amount of green, type a new number into this box or click the arrows.";

	/* [83] Color picker, blue box and controls, Normal */
		"This box displays the amount of blue in the current color. To change the "
		"amount of blue, type a new number into this box or click the arrows.";

	/* [91] Color picker, Color wheel, Normal */
		"This wheel displays the range of hues and saturations available at the "
		"current brightness setting. To change the color, click or drag on the "
		"wheel, and adjust the brightness with the scroll bar to the right.";

	/* [92] Color picker, Brightness scroll bar, Normal */
		"To set the brightness for the current color (the amount of light given off "
		"by the color), click the arrows, drag the scroll box, or click the gray "
		"bar.";

	/* [86] Color picker, Cancel button, Normal */
		"To close this dialog box without changing the color, click this button.";

	/* [87] Color picker, OK button, Normal */
		"To accept the new color and close the dialog box, click this button.";
	}
};

resource 'STR#' (0xE980) { { "R", "G", "B", "C", "M", "Y" } };

data 'CURS' (0xE980) {
	/* An easy-to-see cursor */
	$"0000 03E0 0FF8 1FFC"
	$"1E3C 3C1E 380E 380E"
	$"380E 3C1E 1E3C 1FFC"
	$"0FF8 03E0 0000 0000"
	$"07F0 1FFC 3FFE 3FFE"
	$"7E3F 7C1F 780F 780F"
	$"780F 7C1F 7E3F 3FFE"
	$"3FFE 1FFC 07F0 0000"
	$"0007 0008"
	};

/* Unable to run due to lack of memory */
resource 'ALRT' (-5758, purgeable) {
	{100, 110, 210, 402},
	-5758,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	noAutoCenter
};

resource 'DITL' (-5758, purgeable) { {
	{82, 198, 100, 272}, Button { enabled, "OK" },
	{10, 70, 77, 272}, StaticText { disabled,
		"There is not enough memory to use the Color Picker dialog."
	}
} };
