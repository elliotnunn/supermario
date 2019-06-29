/*
	File:		IconUtils.r

	Contains:	System resources necesary for icon plotting

	Written by:	David Collins

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <9>	  3/6/91	DC		RLC, #83132:  Replaced definition of labelColorBase with
									#Include of IconUtilsPriv.h
		 <8>	  1/8/91	VL		<JWS> Changed label strings to final version.
		 <7>	  9/6/90	DC		fixed index lists
		 <6>	  9/4/90	DC		Added zero-case label colors and string
		 <5>	  9/3/90	JSM		Set sysHeap bit on all resources.
		 <4>	  9/3/90	JSM		Make label colors match current Finder.
		 <3>	 7/12/90	DC		Tuned the index-lists a little further, still not quite right
		 <2>	 6/29/90	DC		Added default strings for labels
		 <1>	 6/27/90	DC		first checked in

	To Do:
*/

#ifndef __ICONUTILSPRIV__
#include "IconUtilsPriv.h"
#endif

type 'lstr' {
	pstring[31];
};

resource 'lstr' (labelColorBase, sysHeap, purgeable) {
	"None"
};

resource 'lstr' (labelColorBase + 1, sysHeap, purgeable) {
	"Project 2"
};

resource 'lstr' (labelColorBase + 2, sysHeap, purgeable) {
	"Project 1"
};

resource 'lstr' (labelColorBase + 3, sysHeap, purgeable) {
	"Personal"
};

resource 'lstr' (labelColorBase + 4, sysHeap, purgeable) {
	"Cool"
};

resource 'lstr' (labelColorBase + 5, sysHeap, purgeable) {
	"In Progress"
};

resource 'lstr' (labelColorBase + 6, sysHeap, purgeable) {
	"Hot"
};

resource 'lstr' (labelColorBase + 7, sysHeap, purgeable) {
	"Essential"
};

type 'rgb ' {
	unsigned hex integer;
	unsigned hex integer;
	unsigned hex integer;
};

resource 'rgb ' (labelColorBase, sysHeap, purgeable) {
	0x0, 0x0, 0x0				// black
};

resource 'rgb ' (labelColorBase + 1, sysHeap, purgeable) {
	0x5600, 0x2C9D, 0x0524		// brown
};

resource 'rgb ' (labelColorBase + 2, sysHeap, purgeable) {
	0x0000, 0x64AF, 0x11B0		// green
};

resource 'rgb ' (labelColorBase + 3, sysHeap, purgeable) {
	0x0000, 0x0000, 0xD400		// blue
};

resource 'rgb ' (labelColorBase + 4, sysHeap, purgeable) {
	0x0241, 0xAB54, 0xEAFF		// cyan
};

resource 'rgb ' (labelColorBase + 5, sysHeap, purgeable) {
	0xF2D7, 0x0856, 0x84EC		// pink
};

resource 'rgb ' (labelColorBase + 6, sysHeap, purgeable) {
	0xDD6B, 0x08C2, 0x06A2		// red
};

resource 'rgb ' (labelColorBase + 7, sysHeap, purgeable) {
	0xFFFF, 0x648A, 0x028C		// orange
};


type 'indl' {
	array IndexList {
		unsigned byte;
	};
};

resource 'indl' (labelColorBase, sysHeap) {{
	0x0, 0x1, 0x5, 0x8, 0x13, 0x16, 0x2a, 0x2b,
	0x33,  0x48,  0x54,  0x5c, 	0x69, 0x7f, 0x92, 0x9f, 0xa5, 0xab,
	0xb0, 0xc0, 0xd8, 0xe3, 0xec, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
	0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
}};

resource 'indl' (labelColorBase + 1, sysHeap) {{
	0x0, 0x1, 0x7, 0x8, 0xb, 0xc, 0xd, 0xe, 0xf
}};
