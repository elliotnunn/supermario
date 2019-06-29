/*
	File:		LayerWDEF.c

	Contains:	Layer definition function

	Written by:	Ed Tecot

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 8/28/90	DFH		Speedups to calc.
		 <3>	  7/2/90	EMT		Miscellaneous size and performance improvements: Content region
									is now empty instead of "wide open".
		 <2>	 2/13/90	EMT		Made wide open region "less wide" because Darin thinks there's a
									bug in Mac Plus QuickDraw.
	   <1.1>	12/11/89	EMT		Include Layers.h instead of Windows.h.
	   <1.0>	 5/13/89	EMT		Added to EASE.
*/

#include <Layers.h>

/* Layer definition function */
pascal long
LayerProc(short, LayerPeek layer, short message, long parameter)
{
	/* Since we only pass the layerStrucRgn along, we’ll do something very sick.
	 * We’ll keep it in a long local variable, so the compiler will use a data register.
	 */
	register long layerStrucRgnLong = (long) layer->strucRgn;
	#define layerStrucRgn ((RgnHandle) layerStrucRgnLong)

	if (message == wCalcRgns) {
		register WindowPeek child;

		/* Structure region is union of children's.
		 * This reflects the fact that a layer encompasses all its children.
		 */
		if ((child = layer->subWindows) != nil) {
			register WindowPeek next;

			if ((next = child->nextWindow) != nil) {
				/* Two or more children.
				 * Union the first two, and loop through the rest.
				 */
				UnionRgn(child->strucRgn, next->strucRgn, layerStrucRgn);
				while ((next = next->nextWindow) != nil)
					UnionRgn(layerStrucRgn, next->strucRgn, layerStrucRgn);
			} else
				/* One child. Copy his strucRgn. */
				CopyRgn(child->strucRgn, layerStrucRgn);
		} else
			/* No children. No strucRgn. */
			SetEmptyRgn(layerStrucRgn);

		/* Content region is always empty. No need to recalculate it. */
	} else if (message == wHit) {
		/* If the point is inside our structure region, we've been hit. */
		if (PtInRgn(*((Point *) &parameter), layerStrucRgn))
			return wInContent;
	}
	return wNoHit;
}
