/*
	File:		LayerMgrPatches.c

	Contains:	Glue between Process Mgr and the Layer Manager.

	Written by:	Ed Tecot

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	  4/9/92	KST		JH,There is only one float layer now instead of one float layer
									for each application.
		 <5>	 10/9/91	YK		Added NewFloatLayer and DisposeFloatLayer.
		 <4>	 5/23/91	dba		do a style of coercion that MPW 3.2 C likes
		 <3>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
		 <0>	 4/18/89	EMT		New Today.

*/


#pragma segment	kernel_segment

#include <types.h>
#include <toolutils.h>
#include <osutils.h>
#include <memory.h>
#include <quickdraw.h>
#include <events.h>
#include <resources.h>
#include <retrace.h>
#include <windows.h>
#include <palette.h>
#include <errors.h>

#include "Glue.h"
#include "Lomem.h"
#include "Data.h"
#include "ScrapCoercion.h"
#include "OSDispatchPrivate.h"

Point				GetApplicationMenuLeftCoord(void);
pascal void			ZoomRect(Rect *, Rect *, Boolean);

/* GetLayer. This routine returns the layer pointer for the current
 * process.
 */
pascal LayerPtr
c_getlayer(void)
	{
	unsigned long	olda5;
	LayerPtr		retval;

	olda5 = ProcessMgrA5SimpleSetup();
	retval = pCurrentProcess->p_layer;
	A5SimpleRestore(olda5);
	return(retval);
	}

/* LayerInsert. Can't support anymore.  SysError instead. (June 89) */
pascal void
c_layerinsert(LayerPtr, LayerPtr)
	{
	SysError(dsCoreErr);
	}

/* LayerDelete. Can't support anymore.  SysError instead. (June 89) */
pascal LayerPtr
c_layerdelete(LayerPtr)
	{
	SysError(dsCoreErr);
	return nil;
	}

/* LNewWindow.  Can't support anymore.  SysError instead. (June 89) */
pascal void
c_lnewwindow(WindowPeek)
	{
	SysError(dsCoreErr);
	}

/* LCloseWindow.  Can't support anymore.  SysError instead. (June 89) */
pascal void
c_lclosewindow(WindowPeek)
	{
	SysError(dsCoreErr);
	}

/* LCalcVis.  Don't support anymore.  SysError instead. (April 90) */
pascal void
c_LCalcVis(LayerPtr, WindowPeek)
	{
	SysError(dsCoreErr);
	}

/* LCVBehind.  Don't support anymore.  SysError instead. (April 90) */
pascal void
c_LCVBehind(LayerPtr, WindowPeek, RgnHandle)
	{
	SysError(dsCoreErr);
	}

/* LPaintOne.  Don't support anymore.  SysError instead. (April 90) */
pascal void
c_LPaintOne(LayerPtr, WindowPeek, RgnHandle)
	{
	SysError(dsCoreErr);
	}

/* LPaintBehind.  Don't support anymore.  SysError instead. (April 90) */
pascal void
c_LPaintBehind(LayerPtr, WindowPeek, RgnHandle)
	{
	SysError(dsCoreErr);
	}

/* LShowHide.  Don't need anymore.  SysError instead. (March 90) */
pascal void
c_LShowHide(LayerPtr, Boolean)
	{
	SysError(dsCoreErr);
	}

/* LClipAbove.  ClipAbove knowing about layers closer to the front. */
pascal void
c_LClipAbove(LayerPtr layer, WindowPeek window)
	{
	layer = SwapCurLayer(layer);
	ClipAbove(window);
	SetCurLayer(layer);
	}

/* MakeTopLayer. This routine will make the given layer the frontmost
 * layer, doing the necessary redrawing and visRgn recomputation.  We avoid
 * calling SelectWindow, since it will generate an extra deactivate/activate.
 */
pascal void
c_maketoplayer(LayerPtr layer)
	{
	WindowPtr		topWindow;
	unsigned long	olda5;
	THz				tempZone;

	olda5 = ProcessMgrA5Setup();

	/* Recalculate vis regions, etc.  Switch to the system heap since this is a
	 * cross-application operation, and we don't want to penalize the current
	 * application's zone for the memory needed to do the region operations.
	 * Layer operations are to us what window operations are to apps.
	 */
	SafeSetZone(tempZone, SYSZONE);
	BringToFront(layer);
	SafeRestoreZone(tempZone);

	/* Highlight the front window (before making layer visible, to avoid flash). */
	if ((topWindow = FrontWindowIn(layer)) != nil)
		{
		HiliteWindow(topWindow, true);
		if (Colorized)
			ActivatePalette(topWindow);
		}

	/* Bring layer back out of hiding */
	if (((LayerPeek) layer)->visible == false)
		HideLayer(layer, true);

	A5Restore(olda5);
	}

/* Closelayer. This routine is called by exiting processes to remove the
 * layer from the screen.  Although it is named layerCLOSE, it actually
 * destroys the layer.  If the layer is owned by the owner of the desktop,
 * we also close the desktop layer.
 * NOTE: This will not be good if the desktop owner has multiple layers,
 * and just wants to close one of them!
 */
pascal void
c_layerclose(LayerPtr layer)
	{
	/* We own the desklayer too. */
	if ((LayerOwner(layer) == desklayerowner))
		desklayerowner = nil;
	DisposeWindow(layer);
	}

/* LayerInit. This routine will initialize the current process' layer. If the
 * process is a normal one, the layer goes to the front of the list. If the process
 * is a background layer, he goes to the back of the list.
 * NOTE: We assume that launch called SetCurLayer(MFLayer).  This keeps the layer/window
 * lists synchronized before this call, and ensures proper placement of the layer we
 * create here.
 */
pascal void
c_layerinit(void)
	{
	register PEntryPtr	pCurrProc;
	LayerPtr			behind;
	unsigned long		olda5;

	olda5 = ProcessMgrA5Setup();
	pCurrProc = pCurrentProcess;
	if (pCurrProc->p_layer != nil)
		dbmsg("Current application called InitWindows twice");

	/* Figure out where the layer should go (foreground vs background). */
	behind = (pCurrProc == pFrontProcess) ? (LayerPtr) -1 : nil;

	(void) NewLayer(&pCurrProc->p_layer, true, false, behind, (long) pCurrProc);
	SetCurLayer(pCurrProc->p_layer);

	A5Restore(olda5);
	}

/* c_LGetNextLayer.  Given a layer, returns the next layer in the list.  Given nil,
 * returns first.
 */
pascal LayerPtr
c_LGetNextLayer(LayerPtr pCurLayer, Boolean wantVisiblesOnly)
	{
	unsigned long	olda5;

	olda5 = ProcessMgrA5Setup();

	/* Nil means start at the first layer */
	pCurLayer = (pCurLayer == nil) ? GetFrontAppLayer() : ((LayerPeek) pCurLayer)->nextWindow;

	/* If the caller will take any layer, return the pCurLayer.
	 * Otherwise, find the first visible one.
	 */
	while (pCurLayer != nil)
		{
		if (wantVisiblesOnly == false || ((LayerPeek) pCurLayer)->visible)
			break;
		pCurLayer = ((LayerPeek) pCurLayer)->nextWindow;
		}

	A5Restore(olda5);
	return(pCurLayer);
	}

/* LayerFind. Given a global mouse coordinate, this routine will snoop through all of
 * the layers' windowlists looking for the frontmost layer which contains this mouse
 * point. The layer pointer is returned if the mouse is in a layer, otherwise zero is
 * returned.  Note that it no longer checks the deskrgn.
 */
pascal LayerPtr
c_layerfind(Point where)
	{
	LayerPtr		pLayer, saveLayer;
	unsigned long	olda5;

	olda5 = ProcessMgrA5Setup();

	saveLayer = SwapCurLayer(MFLayer);
	if (FindLayer(where, &pLayer) != inContent)
		pLayer = nil;
	SetCurLayer(saveLayer);
	A5Restore(olda5);
	return(pLayer);
	}

/* LGetAuxWin. This routine returns the AuxWin Handle for the given window in the
 * given layer.  If the window is nil, the handle to the default record is returned.
 * The boolean return is TRUE if a unique aux handle was found, FALSE if the default
 * is returned.
 */
pascal short
c_lgetauxwin(LayerPtr layer, WindowPeek window, AuxWinHandle awh)
	{
	short			result;

	layer = SwapCurLayer(layer);
	result = GetAuxWin(window, awh);
	SetCurLayer(layer);
	return result;
	}

/* ColorInvalRect. This routine intersects every displayed window with the given
 * device rectangle; for those that intersect, an update is generated.
 * NOTE: This routine may change thePort.
 */
pascal void
c_lcolorinvalrect(Rect *pDevRect, short, short sysupdates)
	{
#pragma unused (sysupdates)
	RgnHandle	clobberedRgn;

	clobberedRgn = NewRgn();
	RectRgn(clobberedRgn, pDevRect);
	PaintOnePal(nil, clobberedRgn);
	DisposeRgn(clobberedRgn);
	}

#define ARE_EQUAL_BOOLEANS(b1, b2)	((((b1) ^ (b2)) & 1) == 0)
#define ARE_NOT_EQUAL_BOOLEANS(b1, b2)	((((b1) ^ (b2)) & 1) != 0)

/* HideLayer.  Do the user interface-y aspects of layer hiding.
 * NOTE:  Requires A5 == PROCESSMGRGLOBALS
 */
Rect						hideDstRect;			/* Set up at init time */

pascal void
HideLayer(LayerPtr layer, Boolean showFlag)
	{
	register WindowPeek 	pFrontWindow;
	PEntryPtr				pProc;

	/* Do nothing if layer is already in requested state (to prevent zooming) */
	if (ARE_EQUAL_BOOLEANS(((LayerPeek) layer)->visible, showFlag))
		return;

	/* Locate topmost visible window. */
	pFrontWindow = FrontWindowIn(layer);
	do
		{
		/* Escape from loop if there are no visibles */
		if (pFrontWindow == nil)
			break;

		/* Ignore window if it has nothing on the screen (empty strucRgn), or it is
		 * a fake DA window (p_wptr is a pointer to it).
		 */
		if (pFrontWindow->visible)
			{
			pProc = LayerOwner(layer);
			if ( (EmptyRgn(pFrontWindow->strucRgn) == false)
				&& (pFrontWindow != pProc->p_wptr) )
				break;
			}

		/* Link to next window back in layer */
		pFrontWindow = pFrontWindow->nextWindow;
		}
	while (true);

	/* If hiding, hide windows before zoom */
	if (!showFlag)
		ShowHide(layer, false);

	/* Zoom the top window, if any.  */
	if (pFrontWindow != nil)
		{
		Rect				hideDstRect, globalOfPortRect;
		Rect				*pPortBitsBounds;
		register short		globalToLocalY;
		register short		globalToLocalX;

		/* Create the rect for the top window */
		pPortBitsBounds = (pFrontWindow->port.portBits.rowBytes < 0)
							? &(*((CGrafPtr)pFrontWindow)->portPixMap)->bounds
							: &pFrontWindow->port.portBits.bounds;
		globalToLocalY = pPortBitsBounds->top;
		globalToLocalX = pPortBitsBounds->left;

		/* Create the rect for the Application menu. */
		*((Point *)&hideDstRect.top) = *((Point *)&hideDstRect.bottom) = GetApplicationMenuLeftCoord();

		/* Convert the "global" port rect to global coords */
		globalOfPortRect = pFrontWindow->port.portRect;
		globalOfPortRect.top -= globalToLocalY;
		globalOfPortRect.left -= globalToLocalX;
		globalOfPortRect.bottom -= globalToLocalY;
		globalOfPortRect.right -= globalToLocalX;

		/* Zoom it, globally */
		ZoomRect(&hideDstRect, &globalOfPortRect, showFlag);
		}

	/* If showing, show windows now that zoom is done. */
	if (showFlag)
		ShowHide(layer, true);
	}

/* ShowHideOthers.  Set the invisibility of all layers but the current app and the
 * desk layer.
 */
void
ShowHideOthers(Boolean showFlag)
	{
	LayerPtr		pLayer;

	pLayer = GetFrontAppLayer();
	while ((pLayer = ((LayerPeek) pLayer)->nextWindow) != nil)
		ShowHide(pLayer, showFlag);
	}

/* ---------------------------------------------------------------------
 *
 * Below are the kernel support routines for the layer manager.
 *
 * --------------------------------------------------------------------- */

/* FindAppLayer.  For the given window, this routine finds the ancestral
 * layer which is a child of MFLayer.  If it can't be found, returns nil.
 */
LayerPtr
FindAppBaseLayer(WindowPtr window, LayerPtr layerPtr)
	{
	register LayerPtr	ancestor;

	/* Paddle upriver.  Must call GetParent the first time, since WindowRecords have
	 * no "parent" field (parent must be derived).  Once we know we have a layer, we
	 * can use the "parent" field directly.
	 */
	ancestor = GetParent(window);
	while ((ancestor != nil) && (ancestor != layerPtr))
		{
		window = ancestor;
		ancestor = ((LayerPeek) ancestor)->parent;
		}

	/* No-find means the given layer not below MFLayer.  By (the current)
	 * definition, applications do not control such layers.
	 */
	if (ancestor == nil)
		window = nil;

	return (LayerPtr) window;
	}

/* CancelUpdateAction.  An action procedure which sets the update region to empty. */
pascal OSErr
CancelUpdateAction(WindowPeek window, LayerPeek, void *)
	{
	SetEmptyRgn(window->updateRgn);
	return noErr;
	}

/* CancelPendingUpdates. For the given layer, set the update regions of every
 * window to the empty region.
 */
void
CancelPendingUpdates(LayerPtr layerPtr)
	{
	(void) EachWindow(layerPtr, ((LayerPeek)layerPtr)->nextWindow, nil, CancelUpdateAction, nil);
	}



/* Floating Window stuff */

#pragma parameter __D0 GetHelpLayer(__D0);
pascal LayerPtr GetHelpLayer()
	=	{
			0x2078,0x02B6,		//	MOVE.L	ExpandMem,A0
			0x2068,0x0078,		//	MOVE.L	ExpandMemRec.emHelpGlobals(A0),A0
			0x2028,0x00E6		//	MOVE.L	hmgLayer(A0),D0
		};

pascal OSErr
C_NewFloatLayer( LayerPtr *newFloatLayer)
	{
	LayerPtr		hmLayer;
	LayerPtr		currLayer;
	OSErr			err = noErr;
	unsigned long	olda5;
	THz				tempZone;

	olda5 = ProcessMgrA5Setup();
	if ( (*newFloatLayer = pCurrentProcess->p_floatLayer)!=nil )
		{
		A5Restore(olda5);
		return noErr;
		}

	currLayer = SwapCurLayer( GetRootLayer());
	SafeSetZone(tempZone, SYSZONE);
	if ( IMLayer==nil )
		{
		if ( ( hmLayer = GetHelpLayer() )==nil )
			{
			hmLayer = (LayerPtr)-1;
			}
		err = NewLayer( &IMLayer, true, true, hmLayer, (long)nil);
		}
	/****
	if ( err==noErr )
		{
		SetCurLayer( IMLayer);
		err = NewLayer( newFloatLayer, true, true, (WindowPtr)-1, (long)pCurrentProcess);
		}
	****/
	SafeRestoreZone(tempZone);
	//pCurrentProcess->p_floatLayer = *newFloatLayer;
	*newFloatLayer = IMLayer;
	SetCurLayer( currLayer);

	A5Restore(olda5);
	return err;
	}

pascal OSErr
C_DisposeFloatLayer( LayerPtr *floatLayer)
{
	OSErr			err = noErr;
	unsigned long	olda5;

	olda5 = ProcessMgrA5Setup();

	if ( pCurrentProcess->p_floatLayer == (*floatLayer) )
		{
		DisposeWindow(*floatLayer);
		*floatLayer = nil;
		}
	else
		err = paramErr;
		
	A5Restore(olda5);
	return err;
}

