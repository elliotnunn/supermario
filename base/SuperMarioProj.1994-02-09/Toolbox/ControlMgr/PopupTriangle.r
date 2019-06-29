/*
	File:		PopupTriangle.r

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	  3/5/90	BBH		checkin script nuked part of file so I'm putting it back in
		 <1>	  3/5/90	BBH		first checked in

	To Do:
*/

#include "Pict.r"
#include "ControlPriv.h"

resource 'PICT' (PopupTrianglePictID, purgeable) {
	{-1, -1, 74, 146},
	VersionOne {
		{	/* array OpCodes: 24 elements */
			/* [2] */
			clipRgn {
				{-1, -1, 74, 146},
				$""
			},
			/* [7] */
			paintPoly {
				{1, 1, 73, 145},
				{	/* array framePoly: 4 elements */
					/* [1] */
					{1, 1},
					/* [2] */
					{1, 145},		
					/* [3] */
					{73, 73},
					/* [4] */
					{1, 1}
				}
			},
			/* [8] */
			clipRgn {
				{0, 0, 0, 0},
				$""
			},
		}
	}
};

