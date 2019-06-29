/*------------------------------------------------------------------------------

	  Sample.r	-  Resources for the Sample Application

	  Copyright Apple Computer, Inc. 1985-1987
	  All rights reserved.

------------------------------------------------------------------------------*/

#include "Types.r"

/* These define's are used in the MENU resources to disable specific
   menu items. */
#define AllItems	0b1111111111111111111111111111111	/* 31 flags */
#define MenuItem1	0b00001
#define MenuItem2	0b00010
#define MenuItem3	0b00100
#define MenuItem4	0b01000
#define MenuItem5	0b10000

resource 'WIND' (128, "Sample Window") {
	{64, 60, 314, 460},
	documentProc, visible, noGoAway, 0x0, "Sample Window"
};

resource 'DLOG' (128, "About Sample…") {
	{66, 102, 224, 400},
	dBoxProc, visible, noGoAway, 0x0, 128, ""
};

resource 'DITL' (128) {
	 {
/* 1 */ {130, 205, 150, 284},
		button {
			enabled,
			"Continue"
		};
/* 2 */ {104, 144, 120, 296},				/* SourceLanguage Item */
		staticText {
			disabled,
			""
		};
/* 3 */ {88, 144, 105, 218},				/* Author Item */
		staticText {
			disabled,
			""
		};
/* 4 */ {8, 32, 26, 273},
		staticText {
			disabled,
			"Macintosh Programmer's Workshop"
		};
/* 5 */ {32, 80, 50, 212},
		staticText {
			disabled,
			"Sample Application"
		};
/* 6 */ {56, 16, 74, 281},
		staticText {
			enabled, "Copyright © 1985-1987 Apple Computer"
		};
/* 7 */ {88, 16, 104, 144},
		staticText {
			enabled, "Source Language:"
		};
/* 8 */ {104, 16, 120, 144},
		staticText {
			enabled, "Brought to you by:"
		}
	}
};

resource 'MENU' (128, "Apple", preload) {
	128, textMenuProc,
	AllItems & ~MenuItem2,	/* Disable item #2 */
	enabled, apple,
	{
		"About Sample…",
			noicon, nokey, nomark, plain;
		"-",
			noicon, nokey, nomark, plain
	}
};

resource 'MENU' (129, "File", preload) {
	129, textMenuProc,
	allEnabled,
	enabled, "File",
	{
		"Quit",
			noicon, "Q", nomark, plain
	}
};

resource 'MENU' (130, "Edit", preload) {
	130, textMenuProc,
	AllItems & ~(MenuItem1 | MenuItem2),	/* Disable items #1 & #2 */
	enabled, "Edit",
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


type 'ASMP' as 'STR ';

resource 'ASMP' (0) {
	"Sample Assembly Language Application - Version 1.0"
};

resource 'ICN#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 1200 0000 5D00 0001 7080 0005 C040"
		$"0017 0020 005C 0010 0170 0018 05C0 0078"
		$"1700 01F0 5C00 07C8 7000 1F28 4000 7CA0"
		$"4001 F280 6007 CA00 701F 2800 387C A000"
		$"1DF2 8000 0FCA 0000 0728 0000 00A0 0000"
		$"0080",
		/* [2] */
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 1200 0000 5E00 0001 7F00 0005 FF80"
		$"0017 FFC0 005F FFE0 017F FFE0 05FF FF80"
		$"17FF FE00 5FFF F800 7FFF E000 7FFF 8000"
		$"3FFE 0000 1FF8 0000 0FE0 0000 0780 0000"
		$"02"
	}
};

resource 'BNDL' (128) {
	'ASMP',
	0,
	{
		'ICN#',
		{
			0, 128
		},
		'FREF',
		{
			0, 128
		}
	}
};

resource 'FREF' (128) {
	'APPL',
	0,
	""
};
