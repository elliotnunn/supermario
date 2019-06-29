/*
	File:		DisplayMgrExtension.r

	Contains:	Additional Resoures used to build "Display Manager" extension

	Written by:	Ian Hendry

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 3/25/93	IH		first checked in
		 <2>	  3/5/93	IH		Fix headers
		 <1>	  3/1/93	IH		first checked in

*/

#include "Types.r"
#include "SysTypes.r"

resource 'vers' (1) {
	0x01, 0x00, alpha, 0x03,
	verUS,
	"1.0d5e2",
	"Display Manager 1.0d5e2, © Apple Computer, Inc. 1991-1993"
};

resource 'vers' (2) {
	0x07, 0x10, release, 0x00,
	verUS,
	"7.1",
	"Macintosh System Software 7.1"
};



type 'ptbl' {
	integer PatchTableVersion1 = 1;		// 'ptbl' version
	integer = $$CountOf(PatchRanges) - 1;	// # of ranges to load
	wide array PatchRanges {
		integer;						// Start of range to load
		integer;						// End of range to load (inclusive)
	};
};
resource 'ptbl' (0, sysHeap, locked) {
	PatchTableVersion1,
	{
		$0000,							// Start of range
		$FFFF							// End of range.  (System file loads them all.)
	};
};
