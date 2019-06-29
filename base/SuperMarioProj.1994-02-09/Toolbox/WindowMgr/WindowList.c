/*
	File:		WindowList.c
	Contains:	Patches to key routines in the window and layer manager to implement a 
				layerless windowing environment for applications.

	Written by:	David Collins

	Copyright:	© 1992-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM4>	 6/17/93	KW		(LW2 fau) Changed the DragFlag from a Boolean Ptr to a short
									ptr, since it is a word in lowmem.
		<14>	10/30/92	DTY		Use Get/Set macros to access ExpandMem.
		<13>	 6/29/92	DC		Fixed the patch on HiliteWindow to make windoids in MacDraw Pro
									work.  (and hopefully, by extension, other home-brew windoids)
		<12>	 6/25/92	DC		Fixed a bug wind GetParent such that it returns a NULL if it
									can't find a layer for the window.  (previously, it was
									returning garbage)  Fixed my patch on SetTrapAddress to preserve
									D1 and A1.
		<11>	  6/8/92	DC		Added startup icon.  Fixed FindLayer for Plus's and SE's.  Fixed
									BusErr with EvenBetterBusErr
		<10>	  6/3/92	DC		Fixed some bugs with FindWindow.  No more crashes with Help on.
		 <9>	 5/31/92	DC		Did the extra work referred to in the last comment.  Most of the
									rough edges taken off of layer ShowHiding.
		 <8>	 5/15/92	DC		Changes to ShowHide to make applicatin hiding work - still needs
									work.
		 <7>	 5/14/92	DC		Fix SendBehind to adjust the ordering of windows at the right
									time.
		 <6>	 4/20/92	DC		Fixed BringToFrontGuts to work across layers.
		 <5>	 4/20/92	DTY		Rename emPackGlobals[0] to emWindowListGlobals.  Also, reduce
									__InitWindowLists to a call to _NewPtr ,Sys,Clear.  This saves
									all the code that clears out the new data structure.
		 <4>	 4/14/92	DC		Fixed GetNextWindow to be pascal (just fits a little better in
									assembly that way) Added BringToFrontGuts (unrefined).
		 <3>	 4/10/92	DC		What I said in comment <2> (I only checked in last time to save
									my state before restructuring the INIT)  Works O.K. but needs
									more patching to get around cases where the ROM window manager
									explicitly get the NextWindow field of a WindowRecord.
		 <2>	  4/1/92	DC		Patched everything over LayerMgr.  Not a good idea.  I get
									spanked by LayerMgr's backpatches.  I'm going to incorporate a
									copy of LayerMgr.c and WindowMgrPatches.a into my INIT so that I
									can make changes to that code as necessary.  All changes to
									working code will be conditionalized.
		 <1>	 3/20/92	DC		first checked in

*/

/*
There are a number of assumptions made in this implementation of a layerless application
world.  They are all currently valid.  If we are to keep this implementation in future system
releases, they must remain valid.  This basicly promotes them from assumptions to enforcable
restrictions on future uses of layers.  If these assumptions are no longer valid for some
system release in which this code is used, the implementation will have to be changed to
adjust.  These assumptions are as follows:

1.) No application except process manager will ever be allowed to create a layer that is not
	a never-active (floating) layer.  This code assumes that if you start at the root and travel
	down only non-floating layers, you will either hit a window in an application layer or a
	never-active (floating) layer within an application layer.  This is certainly O.K. for now
	since no third-party application can create any layers.  If we want to allow application
	writers to explicitly or implicitly create floating layers, that will also be O.K. and the
	world will work.  Creating a non-floating layer under an application doesen't make sense
	anymore anyway, since a non-floating layers don't actually layer things.

2.) Process manager is never going to do anything drasticly different in terms of its layer handling.
	Specificly, it should never use non-floating layers for anything other than the root,
	one multifinder layer and all the application layers.

3.)	Never-active (floating) layers have only windows for children - no layers.  This restriction
	could be relaxed to be that never-active layers only have windows or other never-active
	layers for children with a minor tweak to the existing code.  I'm not sure that we would
	ever need a hierarchy of floating windows, but it is an arbitrary restriction and it could
	be removed.

To build the INIT using BBS use the following command line (impressive, isn't it):

Build -d hasLayerlessAppsINIT=TRUE -d hasLayerlessApps=TRUE system -co '-opt full' -INIT Shuffler -p ∂
-noLink ∂
-wointerface ∂
WindowMgrPatches.a.o ∂
LayerMgr.c.o ∂
InvalMenuBarPatch.a.o ∂
WindowList.a.o ∂
WindowList.c.o
Rez -i "{RIncludes}" "{Sources}ToolBox:WindowMgr:WindowList.r" -append -o "{Sources}BuildResults:System:Shuffler"
SetFile -a iB -t INIT -c shfl "{Sources}BuildResults:System:Shuffler"

*/
	
#ifndef __MEMORY__
#include "Memory.h"
#endif

#ifndef __LAYERS__
#include "Layers.h"
#endif

#ifndef __SYSEQU__
#include "SysEqu.h"
#endif

#ifndef __EXPANDMEMPRIV__
#include "ExpandMemPriv.h"
#endif

#ifndef __OSUTILS__
#include "OSUtils.h"
#endif

#ifndef __MFPRIVATE__
#include "MFPrivate.h"
#endif

#ifndef hasLayerlessAppsINIT
#define hasLayerlessAppsINIT 0
#endif

#define kIsLayer	((short) 0xDEAD)
#define FastIsLayer(window) (((GrafPtr) window)->txSize == kIsLayer)

#define theDeskHook			(*(long *)DeskHook)
#define	CurLayer 			(*(LayerPeek*)0xA90)
#define	SaveVisRgn			(*(RgnHandle*)0x9F2)
#define	WMgrUpdate			(*(WindowPeek*)0xAFC)
#define theWMgrPort 		(*(GrafPtr*)WMgrPort)
#define theWMgrCPort 		(*(GrafPtr*)0xD2C)
#define theGrayRgnHandle	(*(RgnHandle*)GrayRgn)
#define paintWhiteState		(*(short*)PaintWhite)
#define saveUpdateState		(*(short*)0x9DA)
#define	DeskPort			(*(GrafPtr*)0x9E2)
#define	ROM85				(*(unsigned short*)0x28E)
#define DragFlag			(*(short *)0xA44)				// <LW2>
#define	HaveCQD()			(ROM85 <= 0x3FFF)

pascal void PaintDesk(void);

#define cacheSize 101

typedef struct holder
{
	WindowPeek			whichWindow;		//	the window we are managing
	LayerPeek			parent;				//	its layer
	struct holder		**inFrontOf;		//	window in behind this window
	struct holder		**behind;			//	window in front of this window
	struct holder		**next;				//	the next WindowHolder in the hash bucket
	Boolean				visible				//	stored show/hide state for layer show/hides
} WindowHolder, *WindowHolderPtr, **WindowHolderHandle;

typedef struct 
{
	WindowHolderHandle	lastWindow;					//	windowHolder of the window whose click was detected most recently
	Ptr					oldOSDispatch;				//	address of unpatched OSDispatch
	ProcessSerialNumber	bringToFrontProcess;		//	process being made frontmost
	WindowHolderHandle	windowHolderList;
	WindowHolderHandle	windowHolderListEnd;
	WindowHolderHandle	hashTable[cacheSize];
} WindowListData, *WindowListDataPtr;
	
pascal void __InitWindowLists(void)
{
	SetExpandMemWindowListGlobals((long)(NewPtrSysClear(sizeof(WindowListData))));
}

pascal WindowHolderHandle *GetHashLoc(unsigned long windowPeekValue)
{
	register unsigned short entryNo = (windowPeekValue >> 1) & 0xffff;
	entryNo %= cacheSize;
	return &(((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->hashTable[entryNo]);
}

pascal WindowHolderHandle GetWindowHolder(register WindowPeek theWindow)
{
	register WindowHolderHandle	hashBucket;

	if (theWindow <= 0)
	{
		register WindowListDataPtr theData = (WindowListDataPtr)(GetExpandMemWindowListGlobals());
		
		if (!theWindow)
			hashBucket = theData->windowHolderListEnd;
		else if ((long)theWindow == -1)
			hashBucket = theData->windowHolderList;
			
		if (hashBucket)
			theWindow = (*hashBucket)->whichWindow;
		else
			return NULL;
	}
	
	hashBucket = *GetHashLoc((unsigned long)theWindow);

	while (hashBucket)
	{
		WindowHolderPtr hashBucketPtr = *hashBucket;
		
		if (hashBucketPtr->whichWindow == theWindow)
			break;
			
		hashBucket = hashBucketPtr->next;
	}

	return hashBucket;
}

WindowHolderHandle FirstNonFloatingWindowHolder(void)
{
	register WindowHolderHandle theWindowHolderHandle = ((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->windowHolderList;
	register WindowHolderPtr theWindowHolderPtr;
	
	while (theWindowHolderHandle &&
			(theWindowHolderPtr = *theWindowHolderHandle, theWindowHolderPtr->parent->neverActive))
		theWindowHolderHandle = theWindowHolderPtr->inFrontOf;
		
	return theWindowHolderHandle;
}

//	This routine takes a floating layer with no children and returns the WindowHolder
//	in front of which a new member of this layer should be placed.  The basic rule is
//	to place ourselves behind the first floating window in front of us.  If there are not
//	any, we will place ourselves in front of the front most window.  The net result is that
//	we will "bump into" the first floating layer in front of us.

pascal OSErr FindPlaceAction(WindowPeek theWindow, LayerPeek theParent, WindowPeek *lastNeverActiveWindow)
{
	if (theParent->neverActive)
		*lastNeverActiveWindow = theWindow;
		
	return noErr;
}

pascal WindowHolderHandle FindPlaceForFloater(LayerPeek floatParent)
{
	register WindowHolderHandle theWindowHolder = ((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->windowHolderList;

	if (theWindowHolder)
	{
		WindowPeek lastNeverActiveWindow = NULL;
		
		EachWindow((*theWindowHolder)->whichWindow,
				(WindowPeek)floatParent,
				floatParent->parent,
				(WindowAction *)FindPlaceAction,
				(void *)(&lastNeverActiveWindow));
				
		if (lastNeverActiveWindow)
			return GetWindowHolder(lastNeverActiveWindow);
		else
			return theWindowHolder;
	}

	return NULL;
}

//	FirstNonFloatingLayer is a utility used by FrontMostApplicationLayer below.  It returns the first
//	non never-active layer in a the parameter layer.

pascal LayerPeek FirstNonFloatingLayer(register LayerPeek theLayer)
{
	register WindowPeek child = theLayer->subWindows;
	
	while (child && (!FastIsLayer(child) || ((LayerPeek)child)->neverActive))
		child = child->nextWindow;
	
	return (LayerPeek)child;
}

//	FrontMostApplicationLayer:  the name says it all.  Returns the front-most application layer.  This routine
//	is a little more outlandish than it has to be, since we can pretty much assume that the multifinder layer
//	is the front-most non floating child of the root and that the front-most application layer will be the first
//	child of that layer.  This routine will find the front-most application layer under slightly broader
//	conditions (say for example, process manager is changed to add another layer of hierarchy between it and the root)
//	without adding too much more overhead.
//
//	NOTE:  this is where we make the assumption that no application layer will ever have anything other than
//	floating layera or windows for children.  If it had any other kind, we might descend into them, and return
//	something other than an application layer.

LayerPeek FrontMostApplicationLayer(void)
{
	register long	parent = (long)GetRootLayer(),
					layer;
						
	while (layer = (long)FirstNonFloatingLayer((LayerPeek)parent))
		parent = layer;
		
	return (LayerPeek)parent;
}

pascal WindowPtr __FrontWindowIn(LayerPtr layer);

//	This routine takes a window that is being either moved to the front or otherwise
//	inserted front most into its layer.  It returns the WindowHolder of the window
//	in front of which this window should be placed in the global window list.  If this
//	routine returns NULL, it means place the window at the back of the list.

pascal WindowHolderHandle GetNullInsertionPoint(WindowHolderHandle theWindowHolder)
{
	register LayerPeek	theParent = (*theWindowHolder)->parent;
	register WindowPeek frontWindow;

	//	Choose ActiveWindow as the insertion point if this window is a child of
	//	either the root layer or the front-most application layer.
	
	if ((theParent->parent == NULL) || (theParent == FrontMostApplicationLayer()))
		return FirstNonFloatingWindowHolder();

	//	At this point the window we're looking at is either the child of a never-active
	//	layer or the child of a background application layer.  In either case, the right
	//	thing to do is to place the window in front of the front-most window in this layer
	//
	//	NOTE:  This is the place where an assumption is made that a floating window will
	//	only have windows for children.  If, for example, the Parent were a floating
	//	layer with two neverActive layers for children, __FrontWindowIn would return NULL.
	
	if (frontWindow = (WindowPeek)__FrontWindowIn((LayerPtr)theParent))		
		return GetWindowHolder(frontWindow);
	
	//	At this point we're dealing with an empty layer, floating or regular.  Most likely 
	//	this is because its a new layer and the window we're inserting is its first child.
	//	The question is:  where to put the child we're inserting in the global ordering?
	//	If this is an application layer (non-floater) the answer is easy.  We return NULL
	//	which will inform the caller to GetNullInstertionPoint that we should put this 
	//	window back-most.  I mean, why not?  In a layer-less world this makes as much
	//	sense as anyplace for a window that's being created by a background application.
	//	The floating layer is more difficult so we have a routine that executes the logic
	//	for that case.

	if (theParent->neverActive)
		return FindPlaceForFloater(theParent);
	else
		return NULL;
}

pascal void InsertInFrontOf(register WindowHolderHandle theWindowHolder, WindowHolderHandle inFrontOfHolder)
{
	register WindowHolderHandle *insertionPoint = (inFrontOfHolder != NULL) ?
													&((*inFrontOfHolder)->behind) :
													&(((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->windowHolderListEnd);
													
	register long restOfList = (long)(*insertionPoint);
	
	if (restOfList == theWindowHolder)
		return;

	*insertionPoint = theWindowHolder;

	(*theWindowHolder)->inFrontOf = inFrontOfHolder;
	(*theWindowHolder)->behind = (WindowHolderHandle)restOfList;
	
	if (restOfList)
		(*((WindowHolderHandle)restOfList))->inFrontOf = theWindowHolder;
	else
		((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->windowHolderList = theWindowHolder;
}

pascal void InsertBehind(WindowHolderHandle theWindowHolder, register WindowHolderHandle behindHolder)
{
	InsertInFrontOf(theWindowHolder, (behindHolder != NULL) ? (*behindHolder)->inFrontOf : GetNullInsertionPoint(theWindowHolder));
}

pascal void DeleteHolder(WindowHolderHandle theWindowHolder)
{
	register WindowHolderHandle firstPartOfList;
	register WindowHolderHandle lastPartOfList;

	if (!theWindowHolder)
		return;
		
	firstPartOfList = (*theWindowHolder)->behind;
	lastPartOfList = (*theWindowHolder)->inFrontOf;
		
	if (firstPartOfList)
		(*firstPartOfList)->inFrontOf = lastPartOfList;
	else
		((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->windowHolderList = lastPartOfList;

	if (lastPartOfList)		
		(*lastPartOfList)->behind = firstPartOfList;
	else
		((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->windowHolderListEnd = firstPartOfList;
}

pascal OSErr WindowHolderDeleter(register WindowPeek theWindow, LayerPeek, void *)
{
	if (!FastIsLayer(theWindow))
		DeleteHolder(GetWindowHolder(theWindow));
		
	return noErr;
}

pascal void DeleteWindowHolders(register WindowPeek theWindow)
{
	if (FastIsLayer((WindowPtr)theWindow))
	{
		EachWindow(firstWindow, afterLastWindow, (LayerPeek)theWindow, (WindowAction *)WindowHolderDeleter, NULL);
		((LayerPeek)theWindow)->subWindows = NULL;	//	this is so that KillPicture isn't called on the child list
	}
	else
		DeleteHolder(GetWindowHolder(theWindow));
}

pascal void HashWindow(WindowPeek theWindow, WindowPeek behind, Boolean visible)
{
	register WindowHolderHandle	theNewHolderHandle = (WindowHolderHandle)NewHandleSys(sizeof(WindowHolder)),
								*hashLoc = GetHashLoc((long)theWindow);
	register WindowHolderPtr	theNewHolderPtr = *theNewHolderHandle;
	
	if (theNewHolderHandle)
	{
		theNewHolderPtr->whichWindow = theWindow;
		theNewHolderPtr->next = (*hashLoc);
		(*hashLoc) = theNewHolderHandle;
		theNewHolderPtr->parent = CurLayer;
		theNewHolderPtr->visible = visible;
		
		InsertBehind(theNewHolderHandle, GetWindowHolder(behind));
	}
}

pascal void DeHashWindow(WindowPeek theWindow)
{
	if (!FastIsLayer((WindowPtr)theWindow))
	{
		register WindowHolderHandle *theWindowHolderPtr = GetHashLoc((long)theWindow);
		register WindowHolderHandle	theWindowHolder;
		
		while (theWindowHolder = *theWindowHolderPtr)
		{
			if ((*theWindowHolder)->whichWindow == theWindow)
			{
				(*theWindowHolderPtr) = (*theWindowHolder)->next;
				
				DeleteHolder(theWindowHolder);
				DisposHandle((Handle)theWindowHolder);
				
				return;
			}
			
			theWindowHolderPtr = &((*theWindowHolder)->next);
		}
	}
}

pascal void Rearrange(register WindowHolderHandle theWindowHolder, WindowHolderHandle behind)
{
	//	The following condition checks if theWindow is for real, that we're not trying to
	//	insert ourselves behind ourselves, and that if behind is not null, that the
	//	arrangement we want is not already in effect.  If all this is O.K. we delete
	//	theWindowHolder from its place in the global window list and insert it behind
	//	its intended destination.
	
	if (theWindowHolder && (theWindowHolder != behind) &&
		(!behind || ((*behind)->inFrontOf != theWindowHolder)))
	{
		DeleteHolder(theWindowHolder);
		InsertBehind(theWindowHolder, behind);
	}
}

pascal LayerPeek __patchGetParent(WindowPeek theWindow)
{
	register WindowHolderHandle theWindowHolder;
	
	if (!theWindow)
		return CurLayer;
	else if (FastIsLayer((WindowPtr)theWindow))
		return ((LayerPeek)theWindow)->parent;
	else if (theWindowHolder = GetWindowHolder(theWindow))
		return (*theWindowHolder)->parent;
	else
		return NULL;
}

#pragma parameter VisRgnChanged(__A0)
pascal void VisRgnChanged(WindowPtr)
	= { 0x7000, 0xA0A5 };
	
pascal void UpdatedVisRgn(WindowPeek window);

pascal short HitTestWindow(Point,WindowPeek);

pascal WindowHolderHandle FindWindowHolder(register Point thePoint, short *theLocation)
{
	register WindowHolderHandle theWindowHolder = ((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->windowHolderList;

	while (theWindowHolder)
	{
		register WindowPeek theWindow = (*theWindowHolder)->whichWindow;
		
		if (theWindow->visible && PtInRgn(thePoint, theWindow->strucRgn) && (*theLocation = HitTestWindow(thePoint, theWindow)))
		{
			((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->lastWindow = theWindowHolder;
			return theWindowHolder;
		}
		
		theWindowHolder = (*theWindowHolder)->inFrontOf;
	}
	
	return NULL;
}

pascal LayerPeek AncestorThatIsChildOfCurLayer(register LayerPeek theLayer)
{
	while (theLayer && (theLayer->parent != CurLayer))
		theLayer = theLayer->parent;

	return theLayer;
}

pascal long PointInMenuBar(Point);

pascal short __patchFindLayer(Point thePoint, register WindowPeek *whichWindow)
{
	short temp;
	register short returnValue;
	register WindowHolderHandle theWindowHolder;
	
	*whichWindow = NULL;

	if (PointInMenuBar(thePoint) >= 0)
		returnValue = inMenuBar;
	else if (theWindowHolder = FindWindowHolder(thePoint, &temp))
	{
		register LayerPeek theParent = (*theWindowHolder)->parent;
		
		if (theParent == CurLayer)
		{
			*whichWindow = (*theWindowHolder)->whichWindow;
			
			if ((*whichWindow)->windowKind < 0)
				returnValue = inSysWindow;
			else
				returnValue = temp + 2;
		}
		else if (theParent = AncestorThatIsChildOfCurLayer(theParent))
		{
			*whichWindow = (WindowPeek)theParent;
			returnValue = inContent;
		}
		else
			returnValue = inDesk;
	}
	else
	{
		if (theDeskHook)
			returnValue = inSysWindow;
		else
			returnValue = inDesk;
	}
		
	return returnValue;
}

pascal short __FindWindow(Point point, WindowPtr *window)
{
	short returnValue;
	
	if (((returnValue = FindLayer(point, window)) == inContent) && FastIsLayer(*window))
		return inDesk;
	else
		return returnValue;
}

pascal void VisRgnToLocalCoordinates(WindowPeek theWindow)
{
	BitMap	*theMap;
	
	if ((theMap = &(theWindow->port.portBits))->rowBytes < 0)
		theMap = *((BitMap **) theMap->baseAddr);
		
	OffsetRgn(((WindowPtr)theWindow)->visRgn, theMap->bounds.left, theMap->bounds.top);
}

pascal OSErr VisCalcer(WindowPeek theWindow, LayerPeek, long)
{
	if (!FastIsLayer(theWindow))
		CalcVis(theWindow);
	
	return noErr;
}

pascal void __patchCalcVis(register WindowPeek theWindow)
{
	if (!theWindow)
		return;
		
	if (!FastIsLayer((WindowPtr)theWindow))
	{
		if (theWindow->visible)
		{
			register long theVisRgn = (long)theWindow->port.visRgn;
			register RgnHandle	*wMgrPortClipRgnPtr = &(theWMgrPort->clipRgn);
			register long saveClip = (long)(*wMgrPortClipRgnPtr);
			CopyRgn(theWindow->contRgn, (RgnHandle)theVisRgn);
			*wMgrPortClipRgnPtr = (RgnHandle)theVisRgn;
			ClipAbove(theWindow);
			*wMgrPortClipRgnPtr = (RgnHandle)saveClip;
			
			VisRgnToLocalCoordinates(theWindow);
		}
		else
			SetEmptyRgn(((WindowPtr)theWindow)->visRgn);
			
		UpdatedVisRgn(theWindow);	
	}
	else
		EachWindow(firstWindow, afterLastWindow, (LayerPeek)theWindow, (WindowAction *)VisCalcer, NULL);
		
}

//	This routine can be made to be order N in the number of visible windows.  It is currently and
//	always has been order N^2.

pascal void __patchCalcVisBehind(register WindowPeek theWindow, RgnHandle clobberedRegion)
{
	register WindowHolderHandle	theWindowHolder;
	
	if (!theWindow)
		return;
		
	if (!FastIsLayer((WindowPtr)theWindow))
	{
		CalcVis(theWindow);
		
		if (!(theWindowHolder = GetWindowHolder(theWindow)) || !(theWindowHolder = (*theWindowHolder)->inFrontOf))
			return;
		
		do {
			theWindow = (*theWindowHolder)->whichWindow;
			
			if (theWindow->visible && RectInRgn(&((*(theWindow->contRgn))->rgnBBox), clobberedRegion))
				CalcVis(theWindow);
				
			theWindowHolder = (*theWindowHolder)->inFrontOf;
		} while (theWindowHolder);
	}
}

pascal void __patchClipAbove(register WindowPeek theWindow)
{
	register WindowHolderHandle theWindowHolder;
	register long wMgrClipRgn = (long)(theWMgrPort->clipRgn);
	
	SectRgn((RgnHandle)wMgrClipRgn, theGrayRgnHandle, (RgnHandle)wMgrClipRgn);

	//	doing a clipabove on a layer is a meaningless operation in the layerless world
	
	if (theWindow && FastIsLayer((WindowPtr)theWindow))
		return;

	//	we will loop through all the of the windows in front of theWindow, merrily subtracting out structure
	//	regions to arrive at a visRgn that allows us to draw in ourselves, but not our brethren to the front.
	//	If theWindow is NULL, as it is in the case of CalcDeskPortVisRgn, GetWindowHolder will return the
	//	rear of the global window list, which is where we want to seed the loop in this case.
	
	if ((theWindowHolder = GetWindowHolder(theWindow)) && (!theWindow || (theWindowHolder = (*theWindowHolder)->behind)))
	{
		do {
			theWindow = (*theWindowHolder)->whichWindow;
			
			if (theWindow->visible)
				DiffRgn((RgnHandle)wMgrClipRgn, theWindow->strucRgn, (RgnHandle)wMgrClipRgn);
	
			theWindowHolder = (*theWindowHolder)->behind;
		} while (theWindowHolder);
	}
}

void CalcDeskPortVisRgn(void)
{
	register GrafPtr wMgrPort = theWMgrPort;
	register long deskPortVisRgn = (long)(DeskPort->visRgn);
	register RgnHandle wMgrPortClipRgn = wMgrPort->clipRgn;

	CopyRgn(theGrayRgnHandle, (RgnHandle)deskPortVisRgn);
	wMgrPort->clipRgn = (RgnHandle)deskPortVisRgn;
	ClipAbove(nil);
	wMgrPort->clipRgn = wMgrPortClipRgn;
}

void PaintTheDesk(RgnHandle clobberedRgn)
{
	register GrafPtr wMgrPort = theWMgrPort;
	register GrafPtr deskPort = DeskPort;
	register RgnHandle wMgrPortClipRgn = wMgrPort->clipRgn;

	CalcDeskPortVisRgn();
	SectRgn(clobberedRgn, deskPort->visRgn, wMgrPortClipRgn);
	{
		register RgnHandle deskUpdate = ((WindowPeek) deskPort)->updateRgn;
		UnionRgn(deskUpdate, wMgrPortClipRgn, deskUpdate);
	}

	if (HaveCQD())
		wMgrPort = (GrafPtr) theWMgrCPort;
		
	SetPort(wMgrPort);
	PaintDesk();
}

pascal void __patchPaintBehind(register WindowPeek theWindow, register long clobberedRgn)
{
	PaintTheDesk((RgnHandle)clobberedRgn);
	
	if (!theWindow)
		return;
	
	if (!FastIsLayer(theWindow))
	{
		register WindowHolderHandle theWindowHolder;
	
		theWindowHolder = GetWindowHolder(theWindow);
		
		while (theWindowHolder)
		{
			if ((theWindow = (*theWindowHolder)->whichWindow)->visible)
				PaintOne(theWindow, (RgnHandle)clobberedRgn);
			
			theWindowHolder = (*theWindowHolder)->inFrontOf;
		}
	}
}

//	This patch catches the HiliteWindow that is called on the frontmost window of an application that is being brought forward
//	by process manager.  It probably catches some other cases as well, but that should be O.K. since BringToFront won't do
//	anything if the window is frontmost which should be the true except in the case of changing front processes.  If the window
//	is being hilited.

pascal void __patchHiliteWindow(WindowPeek theWindow, Boolean hiliteValue)
{
	pascal void oldHiliteWindow(WindowPeek theWindow, Boolean hiliteValue);

	if (!FastIsLayer((WindowPtr)theWindow))
	{
		register long windowParent = (long)GetParent((WindowPtr)theWindow);
		register WindowHolderHandle	*lastWindow = &(((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->lastWindow);
		
		if (hiliteValue && (theWindow == (WindowPeek)__FrontWindowIn((LayerPtr)windowParent)) && (windowParent == (long)FrontMostApplicationLayer()))
		{
			if ((GetWVariant((WindowPtr)theWindow) == movableDBoxProc) || (*lastWindow && ((**lastWindow)->whichWindow == theWindow)))
				BringToFront((WindowPtr)theWindow);
				
			*lastWindow = NULL;
		}

		oldHiliteWindow(theWindow, hiliteValue);
	}
}

pascal WindowHolderHandle GetNextWindowHolder(register WindowPeek theWindow)
{
	register WindowHolderHandle theWindowHolder;
	
	if (!(theWindowHolder = GetWindowHolder(theWindow)))
		return NULL;
		
	return (*((WindowHolderHandle)theWindowHolder))->inFrontOf;
}

pascal WindowPeek GetNextWindow(register WindowPeek theWindow)
{
	register WindowHolderHandle theWindowHolder = GetNextWindowHolder(theWindow);
	
	if (theWindowHolder)
		return (*theWindowHolder)->whichWindow;
	else
		return NULL;
}

pascal WindowPeek GetNextVisibleWindow(register WindowPeek theWindow)
{
	register WindowHolderHandle theWindowHolder = GetNextWindowHolder(theWindow);
	
	theWindow = NULL;
	
	while (theWindowHolder)
	{
		theWindow = (*theWindowHolder)->whichWindow;
		
		if (theWindow->visible)
			break;
			
		theWindowHolder = (*theWindowHolder)->inFrontOf;
	}
	
	return theWindow;
}

pascal WindowPeek FirstWindowIn(LayerPeek);

pascal void BringChildrenToFront(LayerPeek theLayer)
{
	if (FastIsLayer((WindowPtr)theLayer))
	{
		ProcessSerialNumber *bringToFrontProcess = &(((WindowListDataPtr)(GetExpandMemWindowListGlobals()))->bringToFrontProcess);

		if (bringToFrontProcess->highLongOfPSN || bringToFrontProcess->lowLongOfPSN)
		{
			ProcessSerialNumber thePSN;
			
			if ((GetProcessFromLayer(&thePSN, (LayerPtr)theLayer) == noErr) && (thePSN == *bringToFrontProcess))
			{
				WindowPeek theWindow;
				
				bringToFrontProcess->highLongOfPSN = 0;
				bringToFrontProcess->lowLongOfPSN = 0;

				if (theWindow = FirstWindowIn(theLayer))
				{
					WindowHolderHandle	lastWindowHolder = NULL;
					register long		clobberedRegion = (long)NewRgn();
								  
					do {
						WindowHolderHandle windowHolder;
						
						if (windowHolder = GetWindowHolder(theWindow))
						{
							WindowHolderHandle	inFrontHolder = (*windowHolder)->behind;
							
							while (inFrontHolder)
							{
								if ((*inFrontHolder)->parent != theLayer)
									UnionRgn((RgnHandle)clobberedRegion, ((*inFrontHolder)->whichWindow)->strucRgn, (RgnHandle)clobberedRegion);
									
								inFrontHolder = (*inFrontHolder)->behind;
							}
							
							if (!lastWindowHolder || ((*lastWindowHolder)->inFrontOf != windowHolder))
							{
								DeleteHolder(windowHolder);
								InsertBehind(windowHolder, lastWindowHolder);
							}
							
							lastWindowHolder = windowHolder;
						}
						
						PaintOne(theWindow, (RgnHandle)clobberedRegion);
						CalcVis(theWindow);
						
						theWindow = theWindow->nextWindow;
					} while (theWindow);
					
					if (lastWindowHolder && (lastWindowHolder = (*lastWindowHolder)->inFrontOf))
						CalcVisBehind((*lastWindowHolder)->whichWindow, (RgnHandle)clobberedRegion);
					
					DisposeRgn((RgnHandle)clobberedRegion);
				}
			}
		}
	}
}

pascal void CoolOffsetRgn(RgnHandle rgn, Point offsetPoint)
 = 0xA8E0;
 
pascal void BringToFrontGuts(register WindowPeek theWindow)
{
	pascal void BringToFrontOfCurLayer(WindowPeek);
	register WindowHolderHandle theWindowHolder,
								insertionPoint;
	
	//	Layers don't need any fussing around visRgns, etc.  They just need to be brought to the front.

	if (FastIsLayer(theWindow))
	{
		BringToFrontOfCurLayer(theWindow);
		
		return;
	}
	
	theWindowHolder = GetWindowHolder(theWindow);
	
	if (theWindowHolder && ((insertionPoint = GetNullInsertionPoint(theWindowHolder)) != theWindowHolder))
	{
		WindowHolderHandle	windowTowardScreen = (*theWindowHolder)->behind;
		register long		clobberRegion = (long)NewRgn();
		register long		changedWindowsStructRgn = (long)(theWindow->strucRgn);
		GrafPtr				savedPort;
		
		GetPort(&savedPort);
		
		while (windowTowardScreen)
		{
			WindowPtr whichWindow = (*windowTowardScreen)->whichWindow;
			
			if (((WindowPeek)whichWindow)->visible)
			{
				register long		visRgn = (long)(((GrafPtr)whichWindow)->visRgn);
				Point				offsetPoint = {0,0},
									backPoint;

				SetPort(whichWindow);
				LocalToGlobal(&offsetPoint);
				backPoint.h = -offsetPoint.h;
				backPoint.v = -offsetPoint.v;
	
				CoolOffsetRgn((RgnHandle)visRgn, offsetPoint);
				DiffRgn((RgnHandle)visRgn, (RgnHandle)changedWindowsStructRgn, (RgnHandle)visRgn);
				CoolOffsetRgn((RgnHandle)visRgn, backPoint);
				
				UnionRgn(((WindowPeek)whichWindow)->strucRgn, (RgnHandle)clobberRegion, (RgnHandle)clobberRegion);
			}
			
			windowTowardScreen = (*windowTowardScreen)->behind;	
		}
		
		BringToFrontOfCurLayer(theWindow);
		
		DeleteHolder(theWindowHolder);
		InsertInFrontOf(theWindowHolder, insertionPoint);
				
		SetPort(theWMgrPort);
		
		if (!DragFlag)
		{
			SectRgn((RgnHandle)changedWindowsStructRgn, (RgnHandle)clobberRegion, (RgnHandle)clobberRegion);
			PaintOne(theWindow, (RgnHandle)clobberRegion);
		}
		
		__patchCalcVisBehind(theWindow, (RgnHandle)changedWindowsStructRgn);
		
		SetPort(savedPort);

		DisposeRgn((RgnHandle)clobberRegion);
	}
}

pascal void	romCallWindowCalc(WindowPeek);

pascal OSErr StateSaver(register WindowPeek theWindow, LayerPeek theLayer, long showFlag)
{	
	if (!FastIsLayer(theWindow))
	{
		register long clobberedRgn = (long)(((GrafPtr)theLayer)->visRgn);
		register WindowHolderHandle theWindowHolder;
		
		if (theWindowHolder = GetWindowHolder(theWindow))
		{
			if (!showFlag)
			{
				if (theWindow->visible)
					(*theWindowHolder)->visible = true;
					
				theWindow->visible = false;
			}
			else
				theWindow->visible = (*theWindowHolder)->visible;
		}
		
		if (!showFlag)
		{
			UnionRgn((RgnHandle)clobberedRgn, theWindow->strucRgn, (RgnHandle)clobberedRgn);
			romCallWindowCalc(theWindow);
		}
		else
		{
			romCallWindowCalc(theWindow);
			UnionRgn((RgnHandle)clobberedRgn, theWindow->strucRgn, (RgnHandle)clobberedRgn);
			PaintOne(theWindow, theGrayRgnHandle);
		}
		
		CalcVis(theWindow);
	}
	
	return noErr;
}

pascal OSErr WindowFinder(register WindowPeek theWindow, LayerPeek, WindowPeek *foundWindow)
{
	if (!FastIsLayer(theWindow))
	{
		*foundWindow = theWindow;
		return haltIterator;
	}
	
	return noErr;
}

pascal WindowPeek FirstWindowIn(register LayerPeek theLayer)
{
	WindowPeek theWindow;
	
	if (EachWindow(firstWindow, afterLastWindow, theLayer, (WindowAction *)WindowFinder, (void *)(&theWindow)) != noErr)
		return theWindow;
	else
		return NULL;
}

pascal WindowHolderHandle FirstWindowHolderIn(register LayerPeek theLayer)
{
	register long theWindow = (long)FirstWindowIn(theLayer);
	
	if (theWindow)
		return GetWindowHolder((WindowPeek)theWindow);
	else
		return NULL;
}

pascal void ShowHideLayer(register LayerPeek theLayer, register Boolean showFlag)
{
	if ((theLayer->visible ^ showFlag) & 1)
	{
		theLayer->visible = showFlag;
		
		if (theLayer->subWindows)
		{
			register WindowHolderHandle theWindowHolder = FirstWindowHolderIn(theLayer);
			
			if (theWindowHolder)
			{
				register long clobberedRgn = (long)(((GrafPtr)theLayer)->visRgn);
				GrafPtr	savePort;
				
				SetEmptyRgn((RgnHandle)clobberedRgn);
				
				GetPort(&savePort);
				SetPort(theWMgrPort);

				//	if we're hiding this layer, we should unhilite the front most window.  Process Mgr tries to do
				//	this but it dosen't work anymore because I make all the windows in the layer invisible.  Process Mgr
				//	calls FrontWindowIn to find the frontmost window in the hidden layer so it can call unhilite it.
				//	Unfortunately, I make all the windows in the layer invisible (as you're about to see in the following
				//	code).  FrontWindowIn returns NULL for a layer full of invisible windows so Process Manager just
				//	craps out (lucky it dosen't crash).
				
				if (!showFlag)
					HiliteWindow((WindowPtr)__FrontWindowIn((LayerPtr)theLayer), false);
				
				(void)EachWindow(firstWindow, afterLastWindow, theLayer, (WindowAction *)StateSaver, (void *)showFlag);
				
				PaintTheDesk((RgnHandle)clobberedRgn);
				
				theWindowHolder = (*theWindowHolder)->inFrontOf;

				while (theWindowHolder)
				{
					register WindowPeek theWindow = (*theWindowHolder)->whichWindow;
					
					if (theWindow->visible && ((*theWindowHolder)->parent != theLayer))
					{
						if (!showFlag)
							PaintOne(theWindow, (RgnHandle)clobberedRgn);
							
						if (RectInRgn(&((*(theWindow->contRgn))->rgnBBox), (RgnHandle)clobberedRgn))
							CalcVis(theWindow);
					}
					
					theWindowHolder = (*theWindowHolder)->inFrontOf;
				}
				
				SetPort(savePort);
			}
		}
	}
}

