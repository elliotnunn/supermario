/*------------------------------------------------------------------------------
#
#	Macintosh Developer Technical Support
#
#	Simple Color QuickDraw Animation Sample Application
#
#	TubeTest
#
#	TubeTest.r	-	Rez Source
#
#	Copyright © Apple Computer, Inc. 1988-1990
#	All rights reserved.
#
#	Versions:	1.1	(MPW 3.0 Final)	8/88
#
#	Components:	TubeTest.p			Feb.   1, 1990
#				TubeTest.r			Feb.   1, 1990
#				TubeTest.make		Feb.   1, 1990
#
#	The TubeTest program is a simple demonstration of how to use the Palette 
#	Manager in a color program.  It has a special color palette that is associated
#	with the main window.  The colors are animated using the Palette Manager 
#	to give a flowing tube effect.  The program is very simple, and the Palette
#	Manager and drawing parts are put in separate subroutines to make it easier
#	to figure out what is happening.
#	
#	The program is still a complete Macintosh application with a Main Event Loop,
#	so there is the extra code to run the MEL.  
#	
#	There is a resource file that is necessary as well, to define the Menus, Window,
#	Dialog, and Palette resources used in the program.  
#
#	See Sample and TESample for the general structure and MultiFinder techniques that
#	we recommend that you use when building a new application.
#
------------------------------------------------------------------------------*/

#include "SysTypes.r"
#include "Types.r"

resource 'vers' (1) {
	0x02, 0x00, release, 0x00,
	verUS,
	"1.1",
	"1.1, Copyright \251 Apple Computer, Inc. 1988-1990"
};

/* These define's are used in the MENU resources to disable specific
   menu items. */
#define AllItems	0b1111111111111111111111111111111	/* 31 flags */
#define MenuItem1	0b00001
#define MenuItem2	0b00010
#define MenuItem3	0b00100
#define MenuItem4	0b01000
#define MenuItem5	0b10000


resource 'MENU' (1000, "Apple", preload) {
	1000, textMenuProc,
	AllItems & ~MenuItem2,	/* Disable item #2 */
	enabled, apple,
	{
		"About TubeTest…",
			noicon, nokey, nomark, plain;
		"-",
			noicon, nokey, nomark, plain
	}
};

resource 'MENU' (1001, "File", preload) {
	1001, textMenuProc,
	AllItems & ~MenuItem2,	/* Disable item #2 */
	enabled, "File",
	{
		"Totally Tubular",
			noicon, nokey, nomark, plain;
		"-",
			noicon, nokey, nomark, plain;
		"Quit",
			noicon, "Q", nomark, plain
	}
};

resource 'MENU' (1002, "Edit", preload) {
	1002, textMenuProc,
	AllItems & ~MenuItem2,	/* Disable items #2 */
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


resource 'WIND' (1000, "Tubeland") {
	{50, 10, 350, 610},
	documentProc, visible, noGoAway, 0x0, "Tubeland"
};


resource 'DLOG' (1000, "About TubeTest…") {
	{90, 50, 220, 460},
	rDocProc, visible, noGoAway, 0x0, 1000, "TubeAbout"
};


resource 'DITL' (1000) {
	 {
/* 1 */ {100, 167, 121, 244},
		button {
			enabled,
			"OK"
		};
/* 2 */ {5, 10, 100, 400},				/* SourceLanguage Item */
		staticText {
			disabled,
			"Another totally tubular example program from Bo3b Johnson.  "
			"This one shows how to use the Palette Manager, and color animation.  "
			"Version 1.1\n\nCopyright © Apple Computer, Inc. 1988-1990"
		}
	}
};


data 'pltt' (1000, preload) {
	$"0098 0000 0000 0000 0000 0000 0000 0000"	/* 150 entries, plus B&W. (152 total) */

	$"FFFF FFFF FFFF 0000 0000 0000 0000 0000"	/* white as first guy. */
	$"0000 0000 0000 0002 0000 0000 0000 0000"	/* black as next guy. */

	$"FFFF 0000 0000 0004 0000 0000 0000 0000"   /* 25 in first set, ramp up green. */
	$"FFFF 0A40 0000 0004 0000 0000 0000 0000"
	$"FFFF 1480 0000 0004 0000 0000 0000 0000"
	$"FFFF 1EC0 0000 0004 0000 0000 0000 0000"
	$"FFFF 2900 0000 0004 0000 0000 0000 0000"
	$"FFFF 3340 0000 0004 0000 0000 0000 0000"
	$"FFFF 3D80 0000 0004 0000 0000 0000 0000"
	$"FFFF 47C0 0000 0004 0000 0000 0000 0000"
	$"FFFF 5200 0000 0004 0000 0000 0000 0000"
	$"FFFF 5C40 0000 0004 0000 0000 0000 0000"
	$"FFFF 6680 0000 0004 0000 0000 0000 0000"
	$"FFFF 70C0 0000 0004 0000 0000 0000 0000"
	$"FFFF 7B00 0000 0004 0000 0000 0000 0000"
	$"FFFF 8540 0000 0004 0000 0000 0000 0000"
	$"FFFF 8F80 0000 0004 0000 0000 0000 0000"
	$"FFFF 99C0 0000 0004 0000 0000 0000 0000"
	$"FFFF A400 0000 0004 0000 0000 0000 0000"
	$"FFFF AE40 0000 0004 0000 0000 0000 0000"
	$"FFFF B880 0000 0004 0000 0000 0000 0000"
	$"FFFF C2C0 0000 0004 0000 0000 0000 0000"
	$"FFFF CD00 0000 0004 0000 0000 0000 0000"
	$"FFFF D740 0000 0004 0000 0000 0000 0000"
	$"FFFF E180 0000 0004 0000 0000 0000 0000"
	$"FFFF EBC0 0000 0004 0000 0000 0000 0000"
	$"FFFF F600 0000 0004 0000 0000 0000 0000"

	$"FFFF FFFF 0000 0004 0000 0000 0000 0000"   /* 2nd set is lowering red. */
	$"F5C0 FFFF 0000 0004 0000 0000 0000 0000"
	$"EB80 FFFF 0000 0004 0000 0000 0000 0000"
	$"E140 FFFF 0000 0004 0000 0000 0000 0000"
	$"D700 FFFF 0000 0004 0000 0000 0000 0000"
	$"CCC0 FFFF 0000 0004 0000 0000 0000 0000"
	$"C280 FFFF 0000 0004 0000 0000 0000 0000"
	$"B840 FFFF 0000 0004 0000 0000 0000 0000"
	$"AE00 FFFF 0000 0004 0000 0000 0000 0000"
	$"A3C0 FFFF 0000 0004 0000 0000 0000 0000"
	$"9980 FFFF 0000 0004 0000 0000 0000 0000"
	$"8F40 FFFF 0000 0004 0000 0000 0000 0000"
	$"8500 FFFF 0000 0004 0000 0000 0000 0000"
	$"7AC0 FFFF 0000 0004 0000 0000 0000 0000"
	$"7080 FFFF 0000 0004 0000 0000 0000 0000"
	$"6640 FFFF 0000 0004 0000 0000 0000 0000"
	$"5C00 FFFF 0000 0004 0000 0000 0000 0000"
	$"51C0 FFFF 0000 0004 0000 0000 0000 0000"
	$"4780 FFFF 0000 0004 0000 0000 0000 0000"
	$"3D40 FFFF 0000 0004 0000 0000 0000 0000"
	$"3300 FFFF 0000 0004 0000 0000 0000 0000"
	$"28C0 FFFF 0000 0004 0000 0000 0000 0000"
	$"1E80 FFFF 0000 0004 0000 0000 0000 0000"
	$"1440 FFFF 0000 0004 0000 0000 0000 0000"
	$"0A00 FFFF 0000 0004 0000 0000 0000 0000"

	$"0000 FFFF 0000 0004 0000 0000 0000 0000"   /* 3rd set is raising blue. */
	$"0000 FFFF 0A40 0004 0000 0000 0000 0000"
	$"0000 FFFF 1480 0004 0000 0000 0000 0000"
	$"0000 FFFF 1EC0 0004 0000 0000 0000 0000"
	$"0000 FFFF 2900 0004 0000 0000 0000 0000"
	$"0000 FFFF 3340 0004 0000 0000 0000 0000"
	$"0000 FFFF 3D80 0004 0000 0000 0000 0000"
	$"0000 FFFF 47C0 0004 0000 0000 0000 0000"
	$"0000 FFFF 5200 0004 0000 0000 0000 0000"
	$"0000 FFFF 5C40 0004 0000 0000 0000 0000"
	$"0000 FFFF 6680 0004 0000 0000 0000 0000"
	$"0000 FFFF 70C0 0004 0000 0000 0000 0000"
	$"0000 FFFF 7B00 0004 0000 0000 0000 0000"
	$"0000 FFFF 8540 0004 0000 0000 0000 0000"
	$"0000 FFFF 8F80 0004 0000 0000 0000 0000"
	$"0000 FFFF 99C0 0004 0000 0000 0000 0000"
	$"0000 FFFF A400 0004 0000 0000 0000 0000"
	$"0000 FFFF AE40 0004 0000 0000 0000 0000"
	$"0000 FFFF B880 0004 0000 0000 0000 0000"
	$"0000 FFFF C2C0 0004 0000 0000 0000 0000"
	$"0000 FFFF CD00 0004 0000 0000 0000 0000"
	$"0000 FFFF D740 0004 0000 0000 0000 0000"
	$"0000 FFFF E180 0004 0000 0000 0000 0000"
	$"0000 FFFF EBC0 0004 0000 0000 0000 0000"
	$"0000 FFFF F600 0004 0000 0000 0000 0000"

	$"0000 FFFF FFFF 0004 0000 0000 0000 0000"   /* 4th set is lowering green. */
	$"0000 F5C0 FFFF 0004 0000 0000 0000 0000"
	$"0000 EB80 FFFF 0004 0000 0000 0000 0000"
	$"0000 E140 FFFF 0004 0000 0000 0000 0000"
	$"0000 D700 FFFF 0004 0000 0000 0000 0000"
	$"0000 CCC0 FFFF 0004 0000 0000 0000 0000"
	$"0000 C280 FFFF 0004 0000 0000 0000 0000"
	$"0000 B840 FFFF 0004 0000 0000 0000 0000"
	$"0000 AE00 FFFF 0004 0000 0000 0000 0000"
	$"0000 A3C0 FFFF 0004 0000 0000 0000 0000"
	$"0000 9980 FFFF 0004 0000 0000 0000 0000"
	$"0000 8F40 FFFF 0004 0000 0000 0000 0000"
	$"0000 8500 FFFF 0004 0000 0000 0000 0000"
	$"0000 7AC0 FFFF 0004 0000 0000 0000 0000"
	$"0000 7080 FFFF 0004 0000 0000 0000 0000"
	$"0000 6640 FFFF 0004 0000 0000 0000 0000"
	$"0000 5C00 FFFF 0004 0000 0000 0000 0000"
	$"0000 51C0 FFFF 0004 0000 0000 0000 0000"
	$"0000 4780 FFFF 0004 0000 0000 0000 0000"
	$"0000 3D40 FFFF 0004 0000 0000 0000 0000"
	$"0000 3300 FFFF 0004 0000 0000 0000 0000"
	$"0000 28C0 FFFF 0004 0000 0000 0000 0000"
	$"0000 1E80 FFFF 0004 0000 0000 0000 0000"
	$"0000 1440 FFFF 0004 0000 0000 0000 0000"
	$"0000 0A00 FFFF 0004 0000 0000 0000 0000"

	$"0000 0000 FFFF 0004 0000 0000 0000 0000"   /* 5th set is raising red. */
	$"0A40 0000 FFFF 0004 0000 0000 0000 0000"
	$"1480 0000 FFFF 0004 0000 0000 0000 0000"
	$"1EC0 0000 FFFF 0004 0000 0000 0000 0000"
	$"2900 0000 FFFF 0004 0000 0000 0000 0000"
	$"3340 0000 FFFF 0004 0000 0000 0000 0000"
	$"3D80 0000 FFFF 0004 0000 0000 0000 0000"
	$"47C0 0000 FFFF 0004 0000 0000 0000 0000"
	$"5200 0000 FFFF 0004 0000 0000 0000 0000"
	$"5C40 0000 FFFF 0004 0000 0000 0000 0000"
	$"6680 0000 FFFF 0004 0000 0000 0000 0000"
	$"70C0 0000 FFFF 0004 0000 0000 0000 0000"
	$"7B00 0000 FFFF 0004 0000 0000 0000 0000"
	$"8540 0000 FFFF 0004 0000 0000 0000 0000"
	$"8F80 0000 FFFF 0004 0000 0000 0000 0000"
	$"99C0 0000 FFFF 0004 0000 0000 0000 0000"
	$"A400 0000 FFFF 0004 0000 0000 0000 0000"
	$"AE40 0000 FFFF 0004 0000 0000 0000 0000"
	$"B880 0000 FFFF 0004 0000 0000 0000 0000"
	$"C2C0 0000 FFFF 0004 0000 0000 0000 0000"
	$"CD00 0000 FFFF 0004 0000 0000 0000 0000"
	$"D740 0000 FFFF 0004 0000 0000 0000 0000"
	$"E180 0000 FFFF 0004 0000 0000 0000 0000"
	$"EBC0 0000 FFFF 0004 0000 0000 0000 0000"
	$"F600 0000 FFFF 0004 0000 0000 0000 0000"

	$"FFFF 0000 FFFF 0004 0000 0000 0000 0000"   /* 6th set is lowering blue. */
	$"FFFF 0000 F5C0 0004 0000 0000 0000 0000"
	$"FFFF 0000 EB80 0004 0000 0000 0000 0000"
	$"FFFF 0000 E140 0004 0000 0000 0000 0000"
	$"FFFF 0000 D700 0004 0000 0000 0000 0000"
	$"FFFF 0000 CCC0 0004 0000 0000 0000 0000"
	$"FFFF 0000 C280 0004 0000 0000 0000 0000"
	$"FFFF 0000 B840 0004 0000 0000 0000 0000"
	$"FFFF 0000 AE00 0004 0000 0000 0000 0000"
	$"FFFF 0000 A3C0 0004 0000 0000 0000 0000"
	$"FFFF 0000 9980 0004 0000 0000 0000 0000"
	$"FFFF 0000 8F40 0004 0000 0000 0000 0000"
	$"FFFF 0000 8500 0004 0000 0000 0000 0000"
	$"FFFF 0000 7AC0 0004 0000 0000 0000 0000"
	$"FFFF 0000 7080 0004 0000 0000 0000 0000"
	$"FFFF 0000 6640 0004 0000 0000 0000 0000"
	$"FFFF 0000 5C00 0004 0000 0000 0000 0000"
	$"FFFF 0000 51C0 0004 0000 0000 0000 0000"
	$"FFFF 0000 4780 0004 0000 0000 0000 0000"
	$"FFFF 0000 3D40 0004 0000 0000 0000 0000"
	$"FFFF 0000 3300 0004 0000 0000 0000 0000"
	$"FFFF 0000 28C0 0004 0000 0000 0000 0000"
	$"FFFF 0000 1E80 0004 0000 0000 0000 0000"
	$"FFFF 0000 1440 0004 0000 0000 0000 0000"
	$"FFFF 0000 0A00 0004 0000 0000 0000 0000"

/* End of table, all 150 real entries, and 2 for B&W. */
};


/* here is the quintessential MultiFinder friendliness device, the SIZE resource */

resource 'SIZE' (-1) {
	dontSaveScreen,
	ignoreSuspendResumeEvents,
	disableOptionSwitch,
	cannotBackground,			/* we cannot background */
								/* guarantees we don't hog the Mac while we are in the background */
	notMultiFinderAware,		/* this says we dont do our own activate/deactivate */
	backgroundAndForeground,	/* this is definitely not a background-only application! */
	dontGetFrontClicks,			/* change this is if you want "do first click" behavior like the Finder */
	ignoreChildDiedEvents,		/* essentially, I'm not a debugger (sub-launching) */
	not32BitCompatible,			/* this app should not be run in 32-bit address space */
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	100 * 1024,					/* we made this (preferred) size bigger than the other (minimum) size */
								/* so you can have more text & scraps */
	100 * 1024					/* we looked at a heap dump while the program was running */
								/* it was using about 27K; we added 13K for stack, text & scraps */
};
