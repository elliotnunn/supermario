/*------------------------------------------------------------------------------
#
#	Macintosh Developer Technical Support
#
#	EditText Sample Control Panel Device
#
#	EditCdev
#
#	EditCdev.r	-	Rez Source
#
#	Copyright © Apple Computer, Inc. 1988-1990
#	All rights reserved.
#
#	Versions:	1.1					7/88
#				1.0					6/88
#
#	Components:	EditCdev.c			Feb.  1, 1990
#				EditCdev.r			Feb.  1, 1990
#				EditCdev.make		Feb.  1, 1990
#
#	EditCdev demonstrates how to implement an editText item
#	in a Control Panel Device.  It utilizes the new undo, cut, copy,
#	paste, and delete messages that are sent to cdevs in
#	response to user menu selections.  How to handle private
#	storage is also covered.
#
------------------------------------------------------------------------------*/


#include "Types.r"
#include "SysTypes.r"

type 'hack' as 'STR ';

resource 'vers' (1) {
	0x01, 0x00, release, 0x00,
	verUS,
	"1.1",
	"1.1, Copyright © Apple Computer, Inc. 1988-1990"
};

resource 'hack' (0, purgeable) {
	"Control Panel Device, INIT and CODE by Macintosh Developer Technical Support"
};

resource 'BNDL' (-4064, purgeable) {
	'hack', 0,
	{	'ICN#', {0, -4064},
		'FREF', {0, -4064}
	}
};

resource 'ICN#' (-4064, purgeable) {
	{ /* array: 2 elements */
		/* [1] */
		$"00 00 00 00 00 00 3F E0 00 00 48 10 00 00 44 10"
		$"00 01 83 10 00 02 01 90 00 04 01 90 00 04 01 90"
		$"00 04 01 90 00 04 03 90 00 04 02 90 00 04 02 90"
		$"00 08 02 90 00 08 02 90 00 10 06 90 00 20 0C 90"
		$"03 C0 18 90 04 00 3F 10 0F FF C0 10 08 00 00 10"
		$"08 00 00 10 08 00 00 10 08 00 FF 10 08 00 00 10"
		$"08 00 00 10 08 00 00 10 08 00 00 10 07 FF FF E0"
		$"04 00 00 20 04 00 00 20 04 00 00 20 07 FF FF E0",
		/* [2] */
		$"00 00 00 00 00 00 3F E0 00 00 7F F0 00 00 7F F0"
		$"00 01 FF F0 00 03 FF F0 00 07 FF F0 00 07 FF F0"
		$"00 07 FF F0 00 07 FF F0 00 07 FF F0 00 07 FF F0"
		$"00 0F FF F0 00 0F FF F0 00 1F FF F0 00 3F FF F0"
		$"03 FF FF F0 07 FF FF F0 0F FF FF F0 0F FF FF F0"
		$"0F FF FF F0 0F FF FF F0 0F FF FF F0 0F FF FF F0"
		$"0F FF FF F0 0F FF FF F0 0F FF FF F0 07 FF FF E0"
		$"07 FF FF E0 07 FF FF E0 07 FF FF E0 07 FF FF E0"
	}
};

resource 'DITL' (-4064) {
	{ /* array DITLarray: 1 elements */
		/* [1] */
		{60, 110, 76, 280},
		EditText {
			enabled, ""
		};
		/* [2] */
		{85, 110, 101, 280},
		EditText {
			enabled, ""
		};
		/* [3] */
		{15, 110, 50, 380},
		StaticText {
			disabled, "Apple Macintosh Developer Technical Support"
			"\nTextEdit Control Panel Device Example"
			"\n© Apple Computer, Inc. 1988-1990"
		}
	}
};

resource 'FREF' (-4064, purgeable) {
	'cdev', 0, ""
};

resource 'nrct' (-4064, purgeable) {
	{	/* array RectArray: 1 elements */
		/* [1] */
		{-1, 87, 130, 322}
	}
};

resource 'mach' (-4064, purgeable) {
	0xFFFF,
	0
};
