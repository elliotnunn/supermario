/*------------------------------------------------------------------------------
#
#	LittleDialog XCMD
#
#	LittleDialog.r	-	Rez Source
#
#	Copyright © 1988 Apple Computer, Inc.
#	All rights reserved.
#
#	HyperCard does not have a convention for resources belonging
#	to XCMDs or XFCNs. Currently using a resource mover to copy the
#	XCMD or XFCN to another stack can leave needed resources behind.
#	There are two things that can be done to help prevent such errors:
#
#	1. To indicate that a resource may belong to an XCMD, this example
#	uses resources having the same id number as the XCMD itself. This
#	is not a complete solution since it will not work for an XCMD
#	which has more than one resource of a given type. This is therefor
#	only a suggestion until a better means of bundling XCMDs with other
#	resources is developed.
#
#	2. Within the code of the XCMD, always check for the availability of
#	of the resource before it is to be used. Then report an error that
#	indicates what resource is missing.
#
------------------------------------------------------------------------------*/


#include "Types.r"


resource 'DITL' (10002, "LittleDialog", purgeable) {
	 {
/* 1 */ {84, 261, 105, 363},
		button {
			enabled,
			"Acknowledged"
		};
/* 2 */ {84-4, 261-4, 105+4, 363+4},	/* surround button rect */
		userItem {
			disabled,
		};
/* 3 */ {9, 13, 75, 362},				/* SourceLanguage Item */
		staticText {
			disabled,
			"^0"
		};
	};
};


resource 'DLOG' (10002, "LittleDialog", purgeable) {
	{34, 36, 150, 412},
	dBoxProc, invisible, noGoAway, 0x0, 10002, "LittleDialog"
};

