/*
	File:		DisplayMgrUtils.c

	Contains:	Contains utility routines for Display Manager

	Written by:	Ian Hendry

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<11>	 11/5/93	IH		Sync with Excelsior.
	  <SM10>	10/10/93	IH		Sync with Excelsior.
		 <9>	 9/10/93	IH		Fix cursor bug in horror ROMs and SuperMario..
		 <8>	 8/26/93	IH		#1108969 <KON>: Move Display Manager Apple event define to
									Displays.h.
		 <7>	 8/16/93	IH		#1099391 <KON>: Fix apple event notification. Move some
									utilities from DisplayMgr.c to here.
		 <6>	  8/4/93	IH		#1101633,1098397,1101795,1101636,1102280 <KON>: Fix 1102280:
									return new device rect from declROM. Also bail gracefully if
									there is no slot for a given gDevice.
		 <5>	 6/25/93	IH		Fix decl ROM lookup. (searched one level too far).  It still
									worked, but I'm not sure why.
		 <4>	 6/22/93	IH		Fix code to get timing information from DeclROM.
		 <3>	  6/1/93	IH		Add Utility call to timing mode info from Decl ROM.
		 <2>	 5/28/93	IH		#1081805,1086363: Add activeOnly booleans for
									DMGetFirstScreenDevice and DMGetNextScreenDevice.  Added some
									warnings and comments for future development.
		 <1>	  4/8/93	IH		first checked in

*/

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

#ifndef __GESTALTEQU__
#include <GestaltEqu.h>
#endif

#ifndef __MENUS__
#include <Menus.h>
#endif

#ifndef __PROCESSES__
#include <Processes.h>
#endif

#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif

#ifndef __RESOURCES__
#include <Resources.h>
#endif

#ifndef __RETRACE__
#include <Retrace.h>
#endif

#ifndef __TOOLUTILS__
#include <ToolUtils.h>					// Might want to get rid of "HiWord" and then I don't need this
#endif

#ifndef __SYSEQU__
#include <SysEqu.h>
#endif

#ifndef __ERRORS__
#include <Errors.h>
#endif

#ifndef __PALETTES__
#include <Palettes.h>
#endif

#ifndef __SLOTS__
#include <Slots.h>
#endif

#ifndef __VIDEO__
#include <Video.h>
#endif

#ifndef __ROMDEFS__
#include <ROMDefs.h>
#endif



#ifndef __DEVICES__
#include <Devices.h>
#endif

#ifndef __MENUMGRPRIV__
#include "MenuMgrPriv.h"
#endif

#ifndef __PALETTESPRIV__
#include "PalettesPriv.h"
#endif

#ifndef __PROCESSESPRIV__
#include "ProcessesPriv.h"
#endif

#ifndef __DATA__
#include "Data.h"						// Needs a better way to get the PEntry struct (now in {Sources}ProcessMgr:Data.h
#endif

#ifndef __DISPLAYS__
#include "Displays.h"
#endif

#ifndef __DISPLAYSPRIV__
#include "DisplaysPriv.h"
#endif

#ifndef __EXPANDMEMPRIV__
#include "ExpandMemPriv.h"
#endif



Boolean IsActiveScreenDevice(GDHandle theDevice,Boolean activeOnly)
{
	// Check to see that the device is non-nil
	// Has a driver (i.e., non-zero refNum)
	// and that it is a screen device
	// and either that we do not care if it is active only or that it is active now
	return(	(	nil != theDevice ) &&
			(	0   != (*theDevice)->gdRefNum )	&&
			(	TestDeviceAttribute(theDevice, mainScreen) || TestDeviceAttribute(theDevice, screenDevice) ) &&
			(	!activeOnly || TestDeviceAttribute(theDevice, screenActive))		);
}

/* GetActiveScreenDeviceCount() returns either number of active screen (if activeOnly is set true), or it
// returns the total number screen devices (if activeOnly is false).
*/
unsigned short
GetActiveScreenDeviceCount(
	Boolean			activeOnly)

{	GDHandle		gDevice					= DMGetFirstScreenDevice(activeOnly);
	unsigned short	activeScreenDeviceCount	= 0;	
	
	while (nil != gDevice)
	{	if (IsActiveScreenDevice(gDevice,activeOnly))
			activeScreenDeviceCount++;
			
		gDevice = DMGetNextScreenDevice(gDevice,activeOnly);
		}
	
	return(activeScreenDeviceCount);
	}

GDHandle FindMaxCoverageDevice(const Rect* windowRect)
// Find the device with most area of the window
// Should we return nil if there is no intersection or should we return MainDevice?
//
// What about alerts and modals?  They need to go completely on the screen (not just partially).
// How do I tell if a dialog has a title bar?
{
	Rect			checkRect;								// Used to check whether the window intersects the device
	Rect			tempRect;								// Rectangle of intersection between window and rect
	unsigned long	tempArea;								// Temp coverage
	unsigned long	maxArea = 0;							// Max coverage so far
	GDHandle		maxDevice = nil;						// In case there is NO intersection with a device
	GDHandle		checkDevice = DMGetFirstScreenDevice(true);	// Start to walk device list
	

	while( checkDevice )
	{
		checkRect = (*checkDevice)->gdRect;
		
		if( SectRect(&checkRect,windowRect,&tempRect) )		// Does rect hit device?
		{
			
			tempArea = (tempRect.right-tempRect.left)*(tempRect.bottom-tempRect.top);
			if( tempArea > maxArea )
			{
				maxArea = tempArea;
				maxDevice = checkDevice;
			}
		}
		
		checkDevice = DMGetNextScreenDevice(checkDevice,true);
	}
	return(maxDevice);
}


GDHandle FindClosestDevice(const Rect* windowRect)
// MUST RETURN A DEVICE!!
{
	Rect					checkRect;							// Used to check whether the window intersects the device
	register long			minDistanceSqrd = 0x7FFFFFFF;		// Minimum Distance so far
	register long			testDistanceSqrd;
	register long			displayRadiusSqrd;
	long					displayX,displayY;
	long					xDist,yDist;
	register GDHandle		minDevice = GetMainDevice();			// In case there is NO intersection with a device
	register GDHandle		checkDevice = DMGetFirstScreenDevice(true);	// Start to walk device list
	
	while( checkDevice )
	{
		checkRect = (*checkDevice)->gdRect;
		
		// We will be measuring distance from the center of the monitors to
		// the center of the window.
		//
		// This may mean we favor small monitors (because the distance to their
		// centers is smaller).  I can always change it.
		//
		// We do not really want to spent a lot of time getting this just right
		// so we adjust the size of the monitor
		// from the distance.
		
		xDist = (checkRect.left+checkRect.right)/2 - (windowRect->left+windowRect->right)/2;
		yDist = (checkRect.top+checkRect.bottom)/2 - (windowRect->top+windowRect->bottom)/2;
		
		displayX = checkRect.right - checkRect.left;
		displayY = checkRect.bottom - checkRect.top;
		
		displayRadiusSqrd =	(	(checkRect.right - checkRect.left)*(checkRect.right - checkRect.left) +
								(checkRect.bottom - checkRect.top)*(checkRect.bottom - checkRect.top) ) / 4;

		displayRadiusSqrd = 0;
		
		testDistanceSqrd = xDist*xDist + yDist*yDist - displayRadiusSqrd;
		
		
		if( testDistanceSqrd < minDistanceSqrd )
		{
			minDistanceSqrd = testDistanceSqrd;
			minDevice = checkDevice;
		}
		checkDevice = DMGetNextScreenDevice(checkDevice,true);
	}
	return(minDevice);
}



void GetWindowGlobalRegion(WindowPeek theWindow,RgnHandle windowRgn,Point *structOffset, Boolean isMovable,Boolean hasStructRgn)
{
	Point		portOffset;
	Rect		windowRect;
	
	// Design Issue - Windows in hidden layers have empty structure regions
	//	Getting the struct window directly gets a better judge of the actual window position but
	//	it means window movement may be different depending on whether it is hidden.
	//
	if( hasStructRgn )								// When a layer is hidden, window regions are empty
	{
		UnionRgn( theWindow->strucRgn,theWindow->contRgn,windowRgn);
		structOffset->v = (*theWindow->contRgn)->rgnBBox.top - (*theWindow->strucRgn)->rgnBBox.top;
		structOffset->h = (*theWindow->contRgn)->rgnBBox.left - (*theWindow->strucRgn)->rgnBBox.left;
	}
	else
	{
		//	In this case, we are not sure how big the structRgn of the window is because it is hidden.
		//	Therefore we have to guess.
		//
		//	We guess the rect of the port with room for a titlebar
		//
		
		if( isMovable )
				structOffset->v = kDefaultTitleBarHeight;
		else	structOffset->v = 0;
		structOffset->h = 0;
		
		windowRect = theWindow->port.portRect;
		
		// DANGER - in resolution independent QD, the pixmap bounds are adjusted for the resolution of the device.
		//
		
		if( DM__IsColorPort( &theWindow->port ) )
				portOffset = *(Point *)&(*((CGrafPtr )&theWindow->port)->portPixMap)->bounds.top;
		else	portOffset = *(Point *)&theWindow->port.portBits.bounds.top;
		
		OffsetRect(&windowRect,-portOffset.h,-portOffset.v);
		
		if( isMovable )
		{
			windowRect.top -= kDefaultTitleBarHeight;					// Guess at window title height
			windowRect.left -= kDefaultTitleBarHeight;					// Guess at window title height
		}
		RectRgn(windowRgn,&windowRect);
		
		// DANGER -- add more checks for known WDEFs.  Are Truth WDEFs ever going to ship?
		//	If so they we need to handle them.  Perhaps we need to keep the offsets in globals somewhere
		//	so I do not have to alter the code.
	}
}




Rect *LocalPinRectToRect(register Rect *r,register Rect *pinRect)
{
	register short	dx,dv;
	
	dx = pinRect->right - r->right;
	if (dx<0)		{	r->left += dx;		r->right += dx;		}
	else
	{
		dx = pinRect->left - r->left;
		if (dx>0)	{	r->left += dx;		r->right += dx;		}
	}
	
	dv = pinRect->bottom - r->bottom;
	if (dv<0)		{	r->top += dv;		r->bottom += dv;	}
	else
	{
		dv = pinRect->top - r->top;
		if (dv>0)	{	r->top += dv;		r->bottom += dv;	}
	}
	return r;
}

/*
Boolean TestFlagBit(unsigned long flagLong,short flagBit)
{
	return( 0 != (flagLong & (1<<flagBit)) )
}


void SetFlagBit(unsigned long *flagLong,short flagBit)
{
	flagLong |= (1<<flagBit);
}
*/


OSErr CheckExpandMem(Boolean allocateIfNeeded)
{
	ExpandMemPtr					newExpandMem = nil;					// Set to non-nil if we need it
	ExpandMemPtr					savedExpandMem = GetExpandMem();	// How is expandmem now?

	
	if( ((Ptr )-1) == savedExpandMem )				// -1 means not installed (we check for nil next)
		savedExpandMem = nil;
	
	if( nil == savedExpandMem || GetExpandMemVersion() < emCurVersion )
	{
		if( allocateIfNeeded )
			newExpandMem = (ExpandMemPtr )NewPtrSysClear(sizeof(ExpandMemRec));
		if( nil == newExpandMem )
		{
			SysError(dsMemFullErr);					// Get error code and croak
			return(memFullErr);						// In case some Macsbug user wants to keep going, return an error
		}
	}
	if( nil != newExpandMem )						// If we allocated a new expandmem, then install it
	{
		if( savedExpandMem )						// We blockmove if we have a source
		BlockMove((Ptr )savedExpandMem,(Ptr )newExpandMem,savedExpandMem->emSize);
		
		newExpandMem->emVersion = emCurVersion;			// Just to be safe, I will make sure new ExpandMem is 
		newExpandMem->emSize = sizeof(ExpandMemRec);	// completely correct before replacing the old ExpandMem
		(*((ExpandMemPtr *)ExpandMem)) = newExpandMem;	// Replace the old ExpandMem vector
		
		if( savedExpandMem )							// If we allocated a newExpandMem,  
			DisposePtr( (Ptr )savedExpandMem );			// Then get rid of the old ExpandMem.
	}

	return( noErr );
}



OSErr GetGDeviceVideoMode(GDHandle theDevice,VDSwitchInfoPtr videoMode,Boolean* driverHasGetCurMode)
{
	OSErr				getModeErr;
	CntrlParam			pBlock;
	AuxDCEHandle		theDCE;
			
	*driverHasGetCurMode = true;
	pBlock.ioNamePtr = nil;
	pBlock.ioCRefNum = (*theDevice)->gdRefNum;
	pBlock.csCode = cscGetCurMode;
	*(Ptr *)&pBlock.csParam[0] = (Ptr )videoMode;
	
	getModeErr = PBStatusSync((ParmBlkPtr )&pBlock);
			
	if( statusErr == getModeErr )				// If the call is not implemented in the driver, then fill it out by hand
	{
		*driverHasGetCurMode = false;			// Mark that this driver does not support get video mode
		
		theDCE = (AuxDCEHandle )GetDCtlEntry( (*theDevice)->gdRefNum );
		
		if( theDCE )
		{
			videoMode->csMode		= (*theDevice)->gdMode;
			videoMode->csData		= (unsigned char )(*theDCE)->dCtlSlotId;
			videoMode->csPage		= 0;							// What should I set this to?
			videoMode->csBaseAddr	= (Ptr )(*theDCE)->dCtlDevBase;
			videoMode->csReserved	= 0;
			
			getModeErr = noErr;
		}
		
	}
			
	return(getModeErr);
}

OSErr SwitchVideoMode(GDHandle theDevice,VDSwitchInfoPtr newMode)
{
	OSErr				switchModeErr;
	CntrlParam			pBlock;
			
	pBlock.ioNamePtr = nil;
	pBlock.ioCRefNum = (*theDevice)->gdRefNum;
	pBlock.csCode = cscSwitchMode;	
	*(Ptr *)&pBlock.csParam[0] = (Ptr )newMode;
	
	
	HideCursor();
	switchModeErr = PBControlSync((ParmBlkPtr )&pBlock);
	DMMoveCursor(nil,nil);
	ShowCursor();
	
	return(switchModeErr);
}

OSErr GetDisplayConnection(GDHandle theDevice,VDDisplayConnectInfoPtr connectInfo)
{
	OSErr				connectionErr;
	CntrlParam			pBlock;
			
	pBlock.ioNamePtr = nil;
	pBlock.ioCRefNum = (*theDevice)->gdRefNum;
	pBlock.csCode = cscGetConnection;	
	*(Ptr *)&pBlock.csParam[0] = (Ptr )connectInfo;
	
	connectionErr = PBStatusSync((ParmBlkPtr )&pBlock);
	
	return(connectionErr);
}

OSErr GetTimingMode(GDHandle theDevice,VDTimingInfoPtr timingInfo)
{
	OSErr				connectionErr;
	CntrlParam			pBlock;
	
	pBlock.ioNamePtr = nil;
	pBlock.ioCRefNum = (*theDevice)->gdRefNum;
	pBlock.csCode = cscGetModeTiming;
	*(Ptr *)&pBlock.csParam[0] = (Ptr )timingInfo;
	
	connectionErr = PBStatusSync((ParmBlkPtr )&pBlock);
	
	return(connectionErr);
}


Boolean DisplayHasMenu(GDHandle aDevice)
// Needs to change to support multiple menu bars.
{
//	DANGER -- should we only check for hasAuxMenuBar (ie hasMenuBar)?
//	Should we move this flag into internal data structures?
//	Should we have a global that tell us so I don't have to change the code?

	return( TestDeviceAttribute(aDevice, mainScreen) || TestDeviceAttribute(aDevice, hasAuxMenuBar) );
}




pascal OSErr DM_GetBestDepthModeInfo(GDHandle theDevice,VDSwitchInfoPtr modeInfo,unsigned long depthMode,short* bestDepth, Rect* modeRect)
//
//	Return the mode number (128-133 or so) for the some depth
//	or zero if no such mode available.
// This routine should be patched if we ever stop using the slot manager.
//
// depthMode = 0	=> current device depth
// depthMode < 128	=> Special constant (for now depth only)
// depthMode >=128	=> Actual mode requested
{
	SpBlock			slotBlock;
	Ptr				savedSlotInfoPtr;
	VPBlockPtr		videoParams;
	OSErr			slotError;
	OSErr			modeInfoError;
	unsigned char	walkModes;
	short			depthSoFar = 0;			
	unsigned short		theSlot;
	unsigned short		theID;
	unsigned short		theExtDev = 0,
						tempExtDev;
	Rect				rectSoFar;
	
	modeInfoError = GetDeviceSlotInfo(theDevice,&theSlot,&theID,&tempExtDev);
	
	SetRect(&rectSoFar,0,0,0,0);
	if( noErr == modeInfoError )
	{
		if( 0 == depthMode ) 						// Special case if depthmode is zero, then look for current depth
			depthMode = (*(*theDevice)->gdPMap)->pixelSize;			
		if( 0 == modeInfo->csData )
		{
			modeInfo->csData = theID;
			theExtDev = tempExtDev;
		}
		
		slotBlock.spParamData	= (1<<fall)+(1<<foneslot);	// Search only the specified slot and search all resources (even disabled)			
		slotBlock.spSlot		= theSlot;			// Which slot
		slotBlock.spID			= modeInfo->csData;	// Funtional sResource ID if new screen mode
		slotBlock.spExtDev		= theExtDev;		// Why - because you always do (although it seems to work even setting it to different values)
	
	//	slotBlock.spCategory	= catDisplay;		// We want only display sResources	
	//	slotBlock.spCType		= typeVideo;		// We want only video sResources from the display card
	//	slotBlock.spDrvrSW		= drSwApple;		// We want only video sResources for an apple driver	
	//	slotBlock.spTBMask		= 1;				// ignore  spDrvrHW
	//	slotError = SGetTypeSRsrc(&slotBlock);		// Find the slot resource DOES NOT WORK FOR THIS -- always gets next resource
													//	This is very lame if you happen to be trying to set to the last
													//	sResource (and then you get an error).  I could try something fancy
													//	like setting the search bit (in case anyone ever pays attention to it)
													//	and subtracting one from the spID, but that is gross.
	
		slotError = SGetSRsrc(&slotBlock);			// Find the slot resource
		
		if( noErr == slotError && 					// If I cannot use SGetTypeSRsrc, then I need to check my work.
				(slotBlock.spCategory != catDisplay || slotBlock.spCType != typeVideo || slotBlock.spDrvrSW != drSwApple))
			slotError = paramErr;
		else
			savedSlotInfoPtr = slotBlock.spsPointer;	// Save the functional sResource table
				
		if( noErr == slotError )
		{
			if ( 128 > depthMode )							// Search by depth
			{
			walkModes = 0x80;							// Start looking for video parameters at 0x80;
			
			while( walkModes != endOfList )				// Walk ALL the vidParams (even past holes)
			{
				slotBlock.spID = walkModes;					// Look at walkModes
				slotBlock.spsPointer = savedSlotInfoPtr;	// Start at the functional sResource table
				if( noErr == SFindStruct(&slotBlock) )		// Did I find a video mode?	
				{
					slotBlock.spID = mVidParams;			// Look for mVid parameters in the video mode
					if( noErr == SGetBlock(&slotBlock) )	// Did I find them?
					{
						videoParams = (VPBlockPtr )slotBlock.spResult;
							if( depthMode == videoParams->vpPixelSize )	// Is the video is the same depth?
							{
								depthSoFar = videoParams->vpPixelSize;
								modeInfo->csMode = walkModes;		// If so, we got what we came for
									rectSoFar = videoParams->vpBounds;
								modeInfoError = noErr;				// We have a solution
								break;							// Go home with this mode
							}
							else								// Otherwise update the closest mode
							{
								if(		0 == modeInfo->csMode ||		// If we have no candidate or this is closer to our desired depth
										(depthSoFar < videoParams->vpPixelSize) && (depthMode > videoParams->vpPixelSize) )
								{
									// Best fit so far.
									// We are going for as deep as possible without overshooting the old depth
									// DANGER: this will screw up would be if the first entry was deeper than our desired depth
									depthSoFar = videoParams->vpPixelSize;		// Save where we are so far
									modeInfo->csMode = walkModes;					// Take this mode as a candidate
									rectSoFar = videoParams->vpBounds;			// Get the rect too
									modeInfoError = noErr;						// We have a candidate
								}
							}
							DisposePtr((Ptr )videoParams);				// SGetBlock always returns a pointer
							videoParams = 0;							// Zero it just to be paranoid
						}
					}
				
					walkModes++;
				}
				
				if( videoParams )								// Did we exit loop with break?
					DisposePtr((Ptr )videoParams);				// If so then we leaked.
			}
			else		// Search for real ID....
			{
				modeInfo->csMode = depthMode;
				
				slotBlock.spID = depthMode;					// Look at depthMode
				slotBlock.spsPointer = savedSlotInfoPtr;	// Start at the functional sResource table

				if( noErr == SFindStruct(&slotBlock) )		// Did I find a video mode?	
				{
					slotBlock.spID = mVidParams;			// Look for mVid parameters in the video mode
					if( noErr == SGetBlock(&slotBlock) )	// Did I find them?
					{
						videoParams	= (VPBlockPtr )slotBlock.spResult;
						depthSoFar	= videoParams->vpPixelSize;
						rectSoFar	= videoParams->vpBounds;

						DisposePtr((Ptr )videoParams);				// SGetBlock always returns a pointer
						videoParams = 0;							// Zero it just to be paranoid
					}
				}
			}
		}
	}
	if( bestDepth )
		*bestDepth = depthSoFar;
	if( modeRect )
		*modeRect = rectSoFar;

	return(modeInfoError);
}



OSErr GetDeviceSlotInfo(GDHandle theDevice,unsigned short* theSlot,unsigned short* theID, unsigned short* theExtDev)
{
	OSErr			getSlotError;
	short			gdRefNum;
	AuxDCEHandle	theDCE;
	
	if(	theSlot )		*theSlot	= 0;
	if(	theID )			*theID		= 0;
	if(	theExtDev )		*theExtDev	= 0;

	getSlotError	= unitEmptyErr;
	gdRefNum		= (*theDevice)->gdRefNum;
	
	if( gdRefNum )
	{
		theDCE = *((*(AuxDCEHandle **)UTableBase) - (1+gdRefNum));
		
		if( theDCE )
		{
			if(	theSlot )		*theSlot	= (*theDCE)->dCtlSlot;
			if(	theID )			*theID		= (unsigned char )(*theDCE)->dCtlSlotId;
			if(	theExtDev )		*theExtDev	= (*theDCE)->dCtlExtDev;

			getSlotError = noErr;
		}
	}
	return(getSlotError);
}



OSErr GetTimingFromDecl(GDHandle theDevice,short displayType,VDTimingInfoPtr declTiming)
{
#pragma unused(displayType);
	SpBlock			slotBlock;
	OSErr			slotError;
	unsigned short	theSlot;

	slotError = GetDeviceSlotInfo(theDevice,&theSlot,nil,nil);

	if( noErr == slotError )
	{
		declTiming->csTimingFormat = kDeclROMtables;
		
		slotBlock.spParamData	= (1<<foneslot);	// Search only the specified slot and search only enalbed resources			
		slotBlock.spSlot		= theSlot;			// Which slot
		slotBlock.spID			= 0;				// Funtional sResource ID if new screen mode
		slotBlock.spExtDev		= 0;				// Why - because you always do (although it seems to work even setting it to different values)
	
		slotBlock.spCategory	= catBoard;			// We want only baord sResources	
		slotBlock.spCType		= typeBoard;		// We want only baord sResources
		slotBlock.spDrvrSW		= 0;				// We want do not care about this	
		slotBlock.spDrvrHW		= 0;				// We want do not care about this	
		slotBlock.spTBMask		= 3;				// ignore  spDrvrHW && spDrvrHW
		slotError = SGetTypeSRsrc(&slotBlock);		// Find the slot resource WORKS FOR THIS -- always gets next resource
													//	We are searching for the first board sResource.
					
		if( noErr == slotError )
		{
			slotBlock.spID = 0x7B;
			slotError = SFindStruct(&slotBlock);			// Look for video parameters in sVidAuxParams
			
			if( noErr == slotError )						// Did I find a video parameters?	
			{
				slotBlock.spID = declTiming->csTimingMode;
					slotError == SReadLong(&slotBlock);		// Look for timing mode
					if( noErr == slotError )
						declTiming->csTimingData = slotBlock.spResult;						// Zero it just to be paranoid
			}
		}
	}
	return(slotError);
}





OSErr ConvertDisplayConfigToAERecord(DisplayConfigPtr displayConfig,AERecord* configList)
{
//
//	displayConfig must be locked.
//	configList must be locked returned complete of null.
//
	OSErr			convertError;
	
	convertError = AECreateList(nil,0,true,configList);

	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDMConfigVersion,		typeLongInteger,	(Ptr )&displayConfig->configVersion,		sizeof(displayConfig->configVersion) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDMConfigFlags,		typeLongInteger,	(Ptr )&displayConfig->configFlags,			sizeof(displayConfig->configFlags) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDMConfigReserved,		typeLongInteger,	(Ptr )&displayConfig->configReserved,		sizeof(displayConfig->configReserved) );

	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDisplayID,			typeLongInteger,	(Ptr )&displayConfig->displayID,			sizeof(displayConfig->displayID) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDisplayComponent,		typeLongInteger,	(Ptr )&displayConfig->displayComponent,		sizeof(displayConfig->displayComponent) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDisplayDevice,		typeLongInteger,	(Ptr )&displayConfig->displayDevice,		sizeof(displayConfig->displayDevice) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDisplayFlags,			typeLongInteger,	(Ptr )&displayConfig->displayFlags,			sizeof(displayConfig->displayFlags) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDisplayMode,			typeLongInteger,	(Ptr )&displayConfig->displayMode,			sizeof(displayConfig->displayMode) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDisplayModeReserved,	typeLongInteger,	(Ptr )&displayConfig->displayModeReserved,	sizeof(displayConfig->displayModeReserved) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDisplayReserved,		typeLongInteger,	(Ptr )&displayConfig->displayReserved,		sizeof(displayConfig->displayReserved) );

	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDeviceFlags,			typeShortInteger,	(Ptr )&displayConfig->deviceFlags,			sizeof(displayConfig->deviceFlags) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDeviceDepthMode,		typeLongInteger,	(Ptr )&displayConfig->deviceDepthMode,		sizeof(displayConfig->deviceDepthMode) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyDeviceRect,			typeQDRectangle,	(Ptr )&displayConfig->deviceRect,			sizeof(displayConfig->deviceRect) );

	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapRect,			typeQDRectangle,	(Ptr )&displayConfig->pixMapRect,			sizeof(displayConfig->pixMapRect) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapHResolution,	typeFixed,			(Ptr )&displayConfig->pixMapHResolution,	sizeof(displayConfig->pixMapHResolution) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapVResolution,	typeFixed,			(Ptr )&displayConfig->pixMapVResolution,	sizeof(displayConfig->pixMapVResolution) );

//	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapAlignment,		typeLongInteger,	(Ptr )&displayConfig->pixMapAlignment,		sizeof(displayConfig->pixMapAlignment) );			// be ready for res-ind QD
//	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapReserved,		typeLongInteger,	(Ptr )&displayConfig->pixMapResReserved,	sizeof(displayConfig->pixMapResReserved) );			// be ready for res-ind QD 

	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapPixelType,		typeShortInteger,	(Ptr )&displayConfig->pixMapPixelType,		sizeof(displayConfig->pixMapPixelType) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapPixelSize,		typeShortInteger,	(Ptr )&displayConfig->pixMapPixelSize,		sizeof(displayConfig->pixMapPixelSize) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapCmpCount,		typeShortInteger,	(Ptr )&displayConfig->pixMapCmpCount,		sizeof(displayConfig->pixMapCmpCount) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapCmpSize,		typeShortInteger,	(Ptr )&displayConfig->pixMapCmpSize,		sizeof(displayConfig->pixMapCmpSize) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapReserved,		typeLongInteger,	(Ptr )&displayConfig->pixMapReserved,		sizeof(displayConfig->pixMapReserved) );
	if( noErr == convertError )	 	convertError = AEPutKeyPtr( configList,keyPixMapColorTableSeed,	typeLongInteger,	(Ptr )&displayConfig->pixMapColorTableSeed, sizeof(displayConfig->pixMapColorTableSeed) );

	if( noErr != convertError )
		AEDisposeDesc(configList);

	return(convertError);
}


OSErr BuildDisplayConfigHandle(DisplayConfigHdl *displayConfigs,short *displayCount)
{
	DisplayConfigRec	tempConfig;
	short				localDisplayCount;
	DisplayConfigHdl 	localDisplayConfigs;
	short				displayIndex;
	GDHandle			walkDisplay = DMGetFirstScreenDevice(false);
	OSErr				buildError;
	
	buildError = DMSetDisplayID(nil,0);						// Make sure all current displays have displayIDs

	if( noErr == buildError )
	{
		while( walkDisplay )
		{
			localDisplayCount++;
			walkDisplay = DMGetNextScreenDevice(walkDisplay,false);
		}
		localDisplayConfigs = (DisplayConfigHdl )NewHandleClear( sizeof(DisplayConfigRec)*localDisplayCount );
		buildError = MemError();
	}

	if( noErr != buildError )
	{
		localDisplayCount = 0;
		localDisplayConfigs = nil;
	}
	else
	{
		displayIndex = 0;
		walkDisplay = DMGetFirstScreenDevice(false);
		while( walkDisplay )											// Walk the displays and fill in the display configuration information
		{
			GetDisplayConfig(walkDisplay,&tempConfig);					// Get the config
			(*localDisplayConfigs)[displayIndex++] = tempConfig;		// Copy config to handle
	
			walkDisplay = DMGetNextScreenDevice(walkDisplay,false);
		}
	}
	
	*displayCount = localDisplayCount;								// Return the corrct followers
	*displayConfigs = localDisplayConfigs;							// Set the followers
	
	return(buildError);
}


static short PartitionDisplayConfigs(DisplayConfigPtr configArray,short leftBound, short rightBound)
{
	DisplayConfigRec	tempDisplayConfigs;
	short				keyIndex;
	short				leftIndex;
	short				rightIndex;

	
	keyIndex = rightBound--;
	
	leftIndex = leftBound;
	rightIndex = rightBound;
	
	while( leftIndex < rightIndex )
	{
		while( (leftIndex < rightBound) && (configArray[leftIndex].displayID < configArray[keyIndex].displayID) )
			leftIndex++;
		
		while( (leftBound < rightIndex) && (configArray[keyIndex].displayID < configArray[rightIndex].displayID) )
			rightIndex--;

		if( (leftIndex < rightIndex) )
		{
			tempDisplayConfigs		= configArray[rightIndex];
			configArray[rightIndex]	= configArray[leftIndex];
			configArray[leftIndex]	= tempDisplayConfigs;
		}
	}
	
	if( configArray[keyIndex].displayID < configArray[leftIndex].displayID )
	{
		tempDisplayConfigs		= configArray[keyIndex];
		configArray[keyIndex]	= configArray[leftIndex];
		configArray[leftIndex]	= tempDisplayConfigs;
		
		keyIndex = leftIndex;						// Return the left index
	}
	return(keyIndex);
}

static void SortDisplayConfigs(DisplayConfigPtr configArray,short startSort,short endSort)
{
	short				splitIndex;
		
	if( startSort < endSort )
	{
		splitIndex = PartitionDisplayConfigs(configArray,startSort,endSort);
		
		SortDisplayConfigs(configArray,0,splitIndex-1);
		SortDisplayConfigs(configArray,splitIndex+1,endSort);
		
	}
}


void InitAEDesc(AEDesc* theDesc)
{
	theDesc->descriptorType = typeNull;
	theDesc->dataHandle = nil;
}


OSErr BuildNotificationAppleEvent(DisplaysStateHdl displayState,AERecord* notifyRecord)
{
	OSErr				buildError			= noErr;
	short				newIndex			= 0;
	short				oldIndex			= 0;
	short				oldDisplayCount  	= (*displayState)->displayCount;
	DisplayConfigHdl 	oldDisplayConfigs	= (*displayState)->displayConfigs;

	short				newDisplayCount;
	DisplayConfigHdl 	newDisplayConfigs;
	
	DisplayIDType		newID,oldID,workingID;
	AERecord			oldConfigDesc,newConfigDesc;	
	AERecord			displayDesc;	
	AERecord			notifyDesc;	
	
	
	InitAEDesc(&notifyDesc);					// Init this because we use it if something goes wrong
	InitAEDesc(&displayDesc);					// Init this just to be sure

	buildError = BuildDisplayConfigHandle(&newDisplayConfigs,&newDisplayCount);
	
	if( noErr == buildError )
	{
		SortDisplayConfigs(*newDisplayConfigs,0,newDisplayCount-1);
		SortDisplayConfigs(*oldDisplayConfigs,0,oldDisplayCount-1);
		
		#if DEBUG_DISPLAY_CODE
			{
				short 				countCheck = oldDisplayCount;
				DisplayConfigPtr	configCheck = *oldDisplayConfigs;
				while( countCheck-- )
				{
					if( countCheck && configCheck[countCheck].displayID < configCheck[countCheck-1].displayID )
						DMDebugStr("\p Sort 1 failed");
				}
			}
			{
				short 				countCheck = newDisplayCount;
				DisplayConfigPtr	configCheck = *newDisplayConfigs;
				while( countCheck-- )
				{
					if( countCheck && configCheck[countCheck].displayID < configCheck[countCheck-1].displayID )
						DMDebugStr("\p Sort 2 failed");
				}
			}
		#endif
		
		if( noErr == buildError )
			buildError = AECreateList(nil,0,true,&notifyDesc);		// Create list for display descriptors (keyed by displayID)

		//
		// Both lists are sorted, so we can merge them together.
		//
		
		while( (noErr == buildError) && ((oldIndex < oldDisplayCount) || (newIndex < newDisplayCount)) )
		{
			DisplayConfigRec			tempDisplayConfig;
	
			buildError = AECreateList(nil,0,true,&displayDesc);								// Pairs of display pre and post (except for added displays)
			
			InitAEDesc(&newConfigDesc);
			InitAEDesc(&oldConfigDesc);

			oldID = (*oldDisplayConfigs)[oldIndex].displayID;
			newID = (*newDisplayConfigs)[newIndex].displayID;
			
			if( (oldID == newID) && (oldIndex < oldDisplayCount) && (newIndex < newDisplayCount) )				// Build Before/After Event
			{
				workingID			= oldID;
				tempDisplayConfig	= (*oldDisplayConfigs)[oldIndex++];
				buildError			= ConvertDisplayConfigToAERecord(&tempDisplayConfig, &oldConfigDesc);
				
				if( noErr == buildError )
				{
					tempDisplayConfig	= (*newDisplayConfigs)[newIndex++];
					buildError			= ConvertDisplayConfigToAERecord(&tempDisplayConfig, &newConfigDesc);
				}
			}
			else if(	 (newIndex >= newDisplayCount) ||							// if we are at the end of the new displays all the remaining displays must be old
						((oldIndex < oldDisplayCount) && (oldID < newID)) )			// Build deleted display event
			{
				DMDebugStr("\p Deleted Display");
				workingID			= oldID;
				tempDisplayConfig	= (*oldDisplayConfigs)[oldIndex++];
				buildError			= ConvertDisplayConfigToAERecord(&tempDisplayConfig, &oldConfigDesc);
			}
			else								// Build new display event
			{
				DMDebugStr("\p Added Display");
				workingID			= newID;
				tempDisplayConfig	= (*newDisplayConfigs)[newIndex++];
				buildError			= ConvertDisplayConfigToAERecord(&tempDisplayConfig, &newConfigDesc);
			}
						
			if( noErr == buildError )
				buildError = AEPutKeyDesc(&displayDesc,keyDisplayOldConfig,&oldConfigDesc);
			
			if( noErr == buildError )
				buildError = AEPutKeyDesc(&displayDesc,keyDisplayNewConfig,&newConfigDesc);
			
			if( noErr == buildError )
				buildError = AEPutKeyDesc(&notifyDesc,workingID,&displayDesc);
			
			AEDisposeDesc(&oldConfigDesc);
			AEDisposeDesc(&newConfigDesc);
			AEDisposeDesc(&displayDesc);
		}
		if( noErr != buildError )						// If we got an error, then pass an empty event
			AEDisposeDesc(&notifyDesc);					// Just so there is SOME notification

	}
	*notifyRecord = notifyDesc;
		
	return(buildError);
}


pascal OSErr DM_GetGDeviceByDisplayID(DisplayIDType displayID, GDHandle* displayDevice, Boolean failToMain)
{
	OSErr						getDeviceError	= kDMDisplayNotFoundErr;
	DisplayManagerGlobalsPtr	dmGlobals		= GetDMGlobalsPtr();
	DisplayInfoPtr				displayWalk		= *dmGlobals->fDisplayTable;
	short						displayCount	= dmGlobals->fDisplayCount;

	*displayDevice = nil;
	
	if( dmGlobals )
	{
		while(displayCount--)
		{
			if( displayID == displayWalk->displayID )
			{
				*displayDevice = displayWalk->displayDevice;
				getDeviceError = noErr;
				break;
			}
			displayWalk++;
		}
		
		if( nil == *displayDevice && failToMain)
			*displayDevice = GetMainDevice();
	}
	else
		getDeviceError = kDMSWNotInitializedErr;
	
	return(getDeviceError);
}


pascal OSErr DM_GetDisplayIDByGDevice(GDHandle displayDevice, DisplayIDType *displayID, Boolean failToMain)
{
	OSErr						getDeviceError	= kDMDisplayNotFoundErr;
	DisplayManagerGlobalsPtr	dmGlobals		= GetDMGlobalsPtr();
	DisplayInfoPtr				displayWalk		= *dmGlobals->fDisplayTable;
	short						displayCount	= dmGlobals->fDisplayCount;

	*displayID = nil;
	
	if( dmGlobals )
	{
		while(displayCount--)
		{
			if( displayDevice == displayWalk->displayDevice )
			{
				*displayID = displayWalk->displayID;
				getDeviceError = noErr;
				break;
			}
			displayWalk++;
		}
		
		if( nil == *displayID && failToMain)
		{
			GDHandle		mainDevice = GetMainDevice();
			
			getDeviceError = DMGetDisplayIDByGDevice(GetMainDevice(), displayID, false);
		}
	}
	else
		getDeviceError = kDMSWNotInitializedErr;
	
	return(getDeviceError);
}



DisplayInfoPtr DM_GetDisplayInfoByGDevice(GDHandle displayDevice,Boolean failToMain)
	// Warning:  DM_GetDisplayInfoByGDevice returns a pointer into an unlocked handle
	//		do not move memory.
{
	DisplayInfoPtr				displayInfo = nil;
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();

	if( displayDevice && dmGlobals && dmGlobals->fDisplayTable && dmGlobals->fDisplayCount )
	{
		short			displayCount	= dmGlobals->fDisplayCount;
		DisplayInfoPtr	walkdisplays	= *dmGlobals->fDisplayTable;
		
		while(displayCount--)
		{
			if( walkdisplays->displayDevice == displayDevice )
			{
				displayInfo = walkdisplays;
				break;
			}
			walkdisplays++;
		}
	}
	
	if( (nil == displayInfo) && failToMain )
	{
		GDHandle		theMainDevice = GetMainDevice();
		
		if( theMainDevice != displayDevice )
			displayInfo = DM_GetDisplayInfoByGDevice(theMainDevice,false);
	}

	return(displayInfo);
}



DisplayInfoPtr DM_GetDisplayInfoByDisplayID(DisplayIDType displayID,Boolean failToMain)
{
	// Warning:  DM_GetDisplayInfoByDisplayID returns a pointer into an unlocked handle
	//		do not move memory.
	DisplayInfoPtr				displayInfo = nil;
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();

	if( dmGlobals && dmGlobals->fDisplayTable && dmGlobals->fDisplayCount )
	{
		short			displayCount	= dmGlobals->fDisplayCount;
		DisplayInfoPtr	walkdisplays	= *dmGlobals->fDisplayTable;
		
		while(displayCount--)
		{
			if( walkdisplays->displayID == displayID )
			{
				displayInfo = walkdisplays;
				break;
			}
			walkdisplays++;
		}
	}

	if( (nil == displayInfo) && failToMain )
		displayInfo = DM_GetDisplayInfoByGDevice(GetMainDevice(),false);

	return(displayInfo);
}


pascal OSErr DM_SwapDevices(GDHandle device1, GDHandle device2)
{

	OSErr					swapError = paramErr;
	DisplayInfoPtr			displayInfo1 = DM_GetDisplayInfoByGDevice(device1,false);
	DisplayInfoPtr			displayInfo2 = DM_GetDisplayInfoByGDevice(device2,false);
	
	// Warning:  DM_GetDisplayInfoByGDevice returns a pointer into an unlocked handle
	//		do not move memory.
	
	if( displayInfo1 && displayInfo2 )
	{
		displayInfo1->displayDevice = device2;
		displayInfo2->displayDevice = device1;
		swapError = noErr;
	}
	
	return(swapError);
}




OSErr DM_SetDisplayInfoByDisplayID(DisplayInfoPtr newDisplayInfo)
{
	OSErr						setInfoError		= noErr;
	DisplayManagerGlobalsPtr	dmGlobals			= GetDMGlobalsPtr();
	DisplayInfoPtr				currentDisplayInfo	= DM_GetDisplayInfoByDisplayID(newDisplayInfo->displayID,false);

	if(currentDisplayInfo)
		*currentDisplayInfo = *newDisplayInfo;
	else
	{
		if( dmGlobals )
		{
			if( dmGlobals->fDisplayTable )
					SetHandleSize( (Handle )dmGlobals->fDisplayTable,sizeof(DisplayInfoRec)*(dmGlobals->fDisplayCount+1) );
			else	dmGlobals->fDisplayTable = (DisplayInfoHdl )NewHandleSysClear(sizeof(DisplayInfoRec));
			
			setInfoError = MemError();
			
			if( noErr == setInfoError )
				(*dmGlobals->fDisplayTable)[dmGlobals->fDisplayCount++] = *newDisplayInfo;
		}
	}

	return(setInfoError);
}



OSErr DM_DeleteDisplayInfoByDisplayID(DisplayIDType displayID)
{
	OSErr						setInfoError		= noErr;
	DisplayManagerGlobalsPtr	dmGlobals			= GetDMGlobalsPtr();
	DisplayInfoPtr				currentDisplayInfo	= DM_GetDisplayInfoByDisplayID(displayID,false);
	
	if(currentDisplayInfo)
	{
		long			offset = (long )(char *)currentDisplayInfo-(long )*dmGlobals->fDisplayTable;
		Handle			mungeee = (Handle )dmGlobals->fDisplayTable;

		DMDebugStr("\p About to munge data string");
		Munger(mungeee,offset,nil,sizeof(DisplayInfoRec),(Ptr)-1,0);
	}

	return(setInfoError);
}

void InitNewDisplayInfo(		DisplayIDType		displayID,
								ComponentInstance	displayComponent,
								GDHandle			displayDevice,
								unsigned long		displayFlags,
								unsigned long		displayReserved,
								DisplayInfoPtr		newdisplayInfo)
{
	if( 0 == displayID )
		displayID = GetDMGlobalsPtr()->fDisplayNextID++;

	newdisplayInfo->displayID			= displayID;
	newdisplayInfo->displayComponent	= displayComponent;
	newdisplayInfo->displayDevice		= displayDevice;
	newdisplayInfo->displayFlags		= displayFlags;
	newdisplayInfo->displayReserved		= displayReserved;
}




void GetDisplayConfig(GDHandle displayDevice,DisplayConfigPtr displayConfig)
{
	PixMapHandle	displayPixMap;
	DisplayInfoPtr	displayInfo;
	DisplayInfoRec	saveDisplayInfo;
	VDSwitchInfoRec	videoMode;
	Boolean				hasGetCurMode;
	
	if( displayDevice )
	{
		displayInfo = DM_GetDisplayInfoByGDevice(displayDevice,false);
	
		if( displayInfo )
			saveDisplayInfo = *displayInfo;		// Gotten from unlocked handle, so I make a copy
	}
	
		
	if( displayDevice && displayInfo )
	{
		displayPixMap = (*displayDevice)->gdPMap;
				
		if( noErr == GetGDeviceVideoMode(displayDevice,&videoMode,&hasGetCurMode) )
		{
			displayConfig->displayMode			= videoMode.csData;			// Functional sResource (for NuBus)
			displayConfig->displayModeReserved	= videoMode.csReserved;		// Reserved mode info
		}

		displayConfig->configVersion		= kDMConfigLatestVersion;
		displayConfig->configFlags			= 0;
		displayConfig->configReserved		= 0;
		
		displayConfig->displayID			= saveDisplayInfo.displayID;
		displayConfig->displayComponent		= saveDisplayInfo.displayComponent;
		displayConfig->displayDevice		= saveDisplayInfo.displayDevice;
		displayConfig->displayFlags			= saveDisplayInfo.displayFlags;
		//	displayConfig->displayMode;										// filled above
		//	displayConfig->displayModeReserved;								// filled above
		displayConfig->displayReserved		= 0;
	
		// Information about the display state
		displayConfig->deviceFlags			= (*displayDevice)->gdFlags;	// Reserved
		displayConfig->deviceDepthMode		= (*displayDevice)->gdMode;		// Functional sResource (for NuBus)
		displayConfig->deviceRect			= (*displayDevice)->gdRect;
		
		displayConfig->pixMapRect			= (*displayPixMap)->bounds;
		displayConfig->pixMapHResolution	= (*displayPixMap)->hRes;
		displayConfig->pixMapVResolution	= (*displayPixMap)->vRes;

		displayConfig->pixMapAlignment		= 0;		// be ready for res-ind QD
		displayConfig->pixMapResReserved	= 0;

		displayConfig->pixMapPixelType		= (*displayPixMap)->pixelType;
		displayConfig->pixMapPixelSize		= (*displayPixMap)->pixelSize;
		displayConfig->pixMapCmpCount		= (*displayPixMap)->cmpCount;
		displayConfig->pixMapCmpSize		= (*displayPixMap)->cmpSize;
		displayConfig->pixMapReserved		= 0;
		displayConfig->pixMapColorTableSeed	= (*(*displayPixMap)->pmTable)->ctSeed;
	}
	else		// We have no device handle, so we just slam everything
	{
		char*		zeroWalker = (char*)displayConfig;
		short		zeroCounter = sizeof(DisplayConfigRec);
		
		while(zeroCounter--)
			*zeroWalker++ = 0;
	}
}

#if DEBUG_DISPLAY_CODE
/*----------------------------------------------------------------------
|  Determine if the stack builds up or down
|
|  Formerly, this peeked at the values on the stack; now it just
|  checks the address of the parameters.
----------------------------------------------------------------------*/
static short StackDirection( short one, short two, short three )
	{
	if( &three > &two ) return( 1 );
	if( &one > &two ) return( -1 );
	
	DebugStr( "\pStackDirection didn't work" );
	return( 0 );
	}

/*----------------------------------------------------------------------
|  Convert a long to a hexadecimal ascii string.  The string
|  pointer provided is advanced past the last character converted.
----------------------------------------------------------------------*/
static void InsertHexNumberInPstr( Str255 pstr, unsigned long number )
	{
	unsigned long		highDigits;
	long				n;
	
	highDigits = number >> 4;
	if( highDigits )
		InsertHexNumberInPstr( pstr, highDigits );
	n = (unsigned char) (number & 0x0F);
	pstr[++pstr[0]] = (unsigned char) ( n > 9 ? 'A' + (n - 10) : n + '0' );
	}

/*----------------------------------------------------------------------
|  This routine takes a source string, a destination string and
|  an array of long integers.  The source string contains exactly
|  one tilde character ("~") for every element in the array of
|  longs.  The source string is copied to the destination string; in
|  the process, every tilde is replaced with a hexadecimal 
|  representation of one of the items from the array.
----------------------------------------------------------------------*/
static void BuildNumberPstr( Str255 pSrc, Str255 pDest, long *number )
	{
	short			index = 0;
	unsigned char	len = pSrc[0];
	short			i = 1;
	
	pDest[0] = 0;
	while( len-- )
		{
		if( pSrc[i] == '~' )
			{
			InsertHexNumberInPstr( pDest, number[index] );
			index += StackDirection( 1, 2, 3 );
			}
		else if( pSrc[i] == '@' )
			{
			char* t = (char*) (&number[index]);
			pDest[++pDest[0]] = t[0];
			pDest[++pDest[0]] = t[1];
			pDest[++pDest[0]] = t[2];
			pDest[++pDest[0]] = t[3];
			index += StackDirection( 1, 2, 3 );
			}
		else
			{
			pDest[++pDest[0]] = pSrc[i];
			}
		++i;
		}
	}
#endif

void BuildMessage( Str255 msg, Str255 pstr, long number, ... )
	{
#if DEBUG_DISPLAY_CODE
	BuildNumberPstr( pstr, msg, &number );
#else
	#pragma unused(pstr, msg, number)
#endif
	}

