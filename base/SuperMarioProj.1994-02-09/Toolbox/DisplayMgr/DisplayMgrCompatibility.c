/*
	File:		DisplayMgrCompatibility.c

	Contains:	Compatibility fixes for display manager.
				To add a new fix, add a true/false (NOT a defined/undefined) flag
					to DisplaysPriv.h.  Try to make the fixes independent and clean.
					There should be some way to detect that a fix is no longer necessary.
					
					For example:  if CloseView is upgraded to work with Display Manager, all
					he has to do is set bit 1 or his gestalt selector and my patch will no
					longer fire.

	Written by:	Ian Hendry

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/16/93	IH		#1099391 <KON>: Sync w/Reality. Add fix for applications that
									incorrectly set the "modeDisplayManagerAware" bit in the size
									resource.
		 <1>	  8/4/93	IH		first checked in
		 <0>	  8/4/93	IH		First checked in.  Fix 1102280.

*/

#ifndef __GESTALTEQU__
#include <GestaltEqu.h>
#endif

#ifndef __NOTIFICATION__
#include <Notification.h>
#endif

#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif

#ifndef __DISPLAYSPRIV__
#include "DisplaysPriv.h"
#endif

#ifndef FixCloseView
	must define fix flags
#endif

#ifndef FixFalseDMAwareBits
	must define fix flags
#endif



#if FixCloseView
struct CloseViewGlobalsRec {
// 		See Header.p in closeview sources fir more details
	short			rLineBytes;				// (0)		See Header.p in closeview sources
	char			mystery1[118];			// (2)		Filler
	Ptr				rScreen;				// (120)	Base address of actual screen
	char			mystery2[4];			// (124)	Filler
	unsigned long	screenSize;				// (128)	Size of the buffer allocated for screen
	unsigned short	sX;						// (132)	Size of the buffer allocated for screen
	unsigned short	sY;						// (134)	Size of the buffer allocated for screen
	char			mystery3[6];			// (136)	Filler
	short			maxPixels;				// (142)	Max Depth of main screen
	char			mystery4[54];			// (144)	Filler
	short			rPixels;				// (198)	Max Depth of main screen
	char			mystery5[12];			// (200)	Filler
	Boolean			wantChangeScreen;		// (212)	Flag to turn closeview back on
	char			mystery6[3];			// (213)	Filler
	NMRec			tooDeepNotification;	// (216)	Max Depth of main screen
};

typedef struct CloseViewGlobalsRec CloseViewGlobalsRec;
typedef CloseViewGlobalsRec *CloseViewGlobalsPtr;

#endif




OSErr DisplayManagerCloseViewCompatibility(GDHandle theGDevice,Rect* newDeviceRect,short newDepth)
// Called just before setting the main display
{
	OSErr			compatibilityErr = noErr;
	long			response;

#if FixCloseView
	
	if( noErr == compatibilityErr )
	{		
		if(		(GetMainDevice() == theGDevice) &&
				(noErr == Gestalt(gestaltCloseViewAttr,&response)) &&
				(0 == (response & (1<<gestaltCloseViewDisplayMgrFriendly)))		)
		{
			EventRecord			bogusEvent;
			CloseViewGlobalsPtr	closeViewGlobals;
			unsigned short		screenY;
			unsigned short		screenX;
			unsigned long		screenLineBytes;
			unsigned long		newMaxDepth;
			
			bogusEvent.message = 'BSDa';
			GetOSEvent(0,&bogusEvent);
	
			if( 'here' == bogusEvent.message )						// Check for signature
			{
				closeViewGlobals = *(CloseViewGlobalsPtr *)&bogusEvent.where;	// global pointer returns in where field
							
		// Danger:  what about ResIndQD and large rowbytes?
		//		this will probably be evil... we need to be able to use long rowbytes
				
				screenX = newDeviceRect->right - newDeviceRect->left;
				screenY = newDeviceRect->bottom - newDeviceRect->top;
				screenLineBytes = ((((screenX+15) / 8) >> 1) << 1);
				newMaxDepth = closeViewGlobals->screenSize / (((unsigned long )screenY) * screenLineBytes);
								
				closeViewGlobals->maxPixels = 0;			// Make sure closeview turns off
				InitGDevice((*theGDevice)->gdRefNum,-1,theGDevice);
				
				// Could I get close view to turn back on if it
				// was on when I started?
				// Right now, closeview turns off when I execute this code

				if( newDepth <= newMaxDepth )
					NMRemove(&closeViewGlobals->tooDeepNotification);
				
				if( 0 != (response & (1<<gestaltCloseViewEnabled)) )
					closeViewGlobals->wantChangeScreen = true;
				
				closeViewGlobals->maxPixels = newMaxDepth;	// Allow closeview to turn back on (if possible) on the next initgdevice
				closeViewGlobals->sX = screenX;
				closeViewGlobals->sY = screenY;
			}
			else
				compatibilityErr = -666;
		}
	}
#endif

	return(compatibilityErr);
}





OSErr DisplayManagerAwareBitCompatibility(ProcessInfoRec *processInfo)
{
	OSErr				awareBitError = noErr;
	FSSpec				processSpec;
	CInfoPBRec			processCatInfo;

#if FixFalseDMAwareBits

#define kEarliestDisplayMgrDate 0xA8894400

	processSpec = *processInfo->processAppSpec;
	processCatInfo.hFileInfo.ioVRefNum		= processSpec.vRefNum;
	processCatInfo.hFileInfo.ioDirID		= processSpec.parID;
	processCatInfo.hFileInfo.ioNamePtr		= &processSpec.name;
	processCatInfo.hFileInfo.ioFVersNum		= 0;					// MFS compatibility (although I don't know why)
	processCatInfo.hFileInfo.ioFDirIndex	= 0;					// By name

	// Since almost all applications work, assume if we cannot get the catalog info, that all is okay	
	if( noErr == PBGetCatInfo(&processCatInfo,false) )
	{
		if( kEarliestDisplayMgrDate > processCatInfo.hFileInfo.ioFlCrDat )
		{
#if DEBUG_DISPLAY_CODE
			DebugStr("\p Application created to early to be display manager aware." );
#endif
			awareBitError = -666;
		}
	}
	else
	{
#if DEBUG_DISPLAY_CODE
		DebugStr("\p Could not get create date on app with DM aware true.");
#endif
	}
	
#endif
	return(awareBitError);
}