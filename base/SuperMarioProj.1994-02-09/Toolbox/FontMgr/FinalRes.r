/*

	File:		FinalRes.r

	Contains:	Resource Version for TrueType Init
	
	Written by:	Charlton E. Lui
	
	Copyright © 1989-1991 by Apple Computer, Inc.  All rights reserved.

	Change History (most recent first):

		 <4>	10/23/91	ngk		Fix bundle and use Rez templates.
		 <3>	 9/25/91	jlf		Rolled in modifications from Pacific TrueType project:
		 							bumped the sysz size from 132k to 310k to compensate
									for 150k partial font heap and 40k of extra code in
									the J-TrueType init. This change should only affect the
									memory configuration of the init version of TrueType.
		 <2>	10/15/90	CL		Added alert dialog data.  Added stuff for desktop icon and show
									init.
		 <3>	 6/13/90	CL		Changing copyright date
		 <2>	  6/5/90	CL		Added the system size resource to this file.
		 <1>	 4/16/90	CL		first checked in
*/
	
	
#include "Types.r"
#include "SysTypes.r"

resource 'vers' (1,purgeable) {
	numVers,						/* Contains all four bytes of information */
	langInt,
	stringVers,
	stringVers ", ©Apple Computer, Inc. 1986-90",
};

data 'sysz' (0) {
	$"00 04 D8 00"
};


resource 'BNDL' (-4064, purgeable) {
	'CHAZ',
	0,
	{	/* array TypeArray: 2 elements */
		/* [1] */
		'FREF',
		{	/* array IDArray: 1 elements */
			/* [1] */
			0, 128
		},
		/* [2] */
		'ICN#',
		{	/* array IDArray: 1 elements */
			/* [1] */
			0, 128
		}
	}
};

data 'CHAZ' (0) {
	$"4E 54 68 69 73 20 69 73 20 61 20 67 72 61 6E 64"    /* NThis is a grand */
	$"20 54 72 75 65 54 79 70 65 AA 20 69 6E 69 74 2E"    /*  TrueType™ init. */
	$"00"                                                 /* . */
};

resource 'FREF' (128, purgeable) {
	'INIT',
	0,
	""
};


/* Icon for TrueType™ */
resource 'ICN#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"00 03 80 00 00 04 80 00 00 08 80 00 00 14 9C 00"
		$"00 2C A4 00 00 54 C4 00 00 A4 84 00 01 7D 04 78"
		$"02 86 44 88 05 04 C5 08 0A 09 46 08 1F 12 44 08"
		$"00 24 48 08 00 4F D0 08 00 80 23 08 01 3F C5 08"
		$"02 40 89 08 04 81 11 08 19 02 21 08 20 84 41 08"
		$"3F 88 FF 08 00 10 00 08 00 20 00 08 00 47 FF 08"
		$"00 88 01 08 01 10 01 08 02 20 01 08 04 40 01 08"
		$"08 80 01 08 31 00 02 04 40 80 04 02 7F 80 07 FE",
		/* [2] */
		$"00 03 80 00 00 07 80 00 00 0F 80 00 00 1F 9C 00"
		$"00 3F BC 00 00 77 FC 00 00 E7 FC 00 01 FF FC 78"
		$"03 FF FC F8 07 07 FD F8 0E 0F 7F F8 1F 1E 7F F8"
		$"00 3C 7F F8 00 7F FF F8 00 FF FF F8 01 FF FD F8"
		$"03 C0 F9 F8 07 81 F1 F8 1F 03 E1 F8 3F 87 C1 F8"
		$"3F 8F FF F8 00 1F FF F8 00 3F FF F8 00 7F FF F8"
		$"00 F8 01 F8 01 F0 01 F8 03 E0 01 F8 07 C0 01 F8"
		$"0F 80 01 F8 3F 00 03 FC 7F 80 07 FE 7F 80 07 FE"
	}
};


/* for alerts */
#define ALERT_SKIP					1				/* skipping install because of mouse */
#define ALERT_WRONG_SYS				2				/* wrong system */
#define ALERT_SYS_ENV_ERR			3				/* System environment error */
#define ALERT_ALREADY_INSTALLED		4				/* already installed */


resource 'ALRT' (0) {
	{100, 100, 182, 408},
	0,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	}
};

resource 'ALRT' (1) {
	{100, 100, 182, 408},
	1,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	}
};

resource 'ALRT' (2) {
	{100, 100, 178, 352},
	2,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	}
};

resource 'ALRT' (3) {
	{100, 100, 182, 408},
	3,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	}
};

resource 'DITL' (0) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{48, 223, 68, 283},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{4, 7, 43, 289},
		StaticText {
			disabled,
			"Mouse button down… skipping TrueType™ installation."
		}
	}
};

resource 'DITL' (1) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{51, 234, 73, 295},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 9, 46, 302},
		StaticText {
			disabled,
			"Cannot install TrueType™… system 6.0.5 or greater is required."
		}
	}
};

resource 'DITL' (2) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{46, 171, 66, 231},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{7, 6, 43, 284},
		StaticText {
			disabled,
			"System Error:  aborting TrueType™ installation!"
		}
	}
};

resource 'DITL' (3) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{51, 233, 75, 289},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 9, 47, 294},
		StaticText {
			disabled,
			"TrueType™ is already installed…  skipping installation!!!"
		}
	}
};


