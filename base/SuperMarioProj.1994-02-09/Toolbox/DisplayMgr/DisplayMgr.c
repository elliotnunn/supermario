/*
	File:		DisplayMgr.c

	Contains:	Sources for the Display Manager

	Written by:	Ian Hendry

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<20>	 11/5/93	IH		Sync with Excelsior.
	  <SM19>	10/10/93	IH		Sync with Excelsior.
		<18>	 9/10/93	IH		Fix cursor bug in horror ROMs and SuperMario
		<17>	  9/2/93	IH		#1101633 <SAH>: Fix SetMainDisplay for Canvas. Canvas has a
									custom color table on a screen port. I was blasting their port
									CTable with the CTable from the new main device. This was bad
									because Canvas (correctly) assumed that it owned the color table
									and wrote over it. Since I replaced their huge CTable with a
									small one, they killed their heap. I will now correctly restore
									custom port CTables.
		<16>	  9/2/93	IH		Make sure Display Manager works during boot process.  Fix my use
									of CopyPixMap so that I no longer count on QuickDraw to do the
									right thing with nil color tables.  I cleaned up the code a
									little around there so I am not making bad assumptions about
									ports vs windows.
		<15>	 8/26/93	IH		#1108969 <KON>: First A5 change. Support PPC UPPs for
									notification procs. Make sure notification calls do not depend
									on process manager.
	  <SM14>	 8/20/93	IH		Last check-in broke Apple event notification current owning
									application.  Make DM send to all apps again.
		<13>	 8/16/93	IH		#1099391 <KON>: Move some ultilities to DisplayUtils.c. Fix
									apple event notifications for proc ptrs. Fix display manager
									MoveWindow compatibility code to call from the application
									context for GX.
		<12>	  8/4/93	IH		#1101633,1098397,1101795,1101636,1102280 <KON>: Change many
									interfaces to propagate displaystate handles. Make compatible
									with CloseView. Make sure gdFlags bits are correctly propogated
									when moving the main display. Setting depth could cause the
									screens to overlap when the device was reinitialized. Add
									Animation Trap.
		<11>	  7/2/93	IH		Make sure cursor is reinitialized when moving displays.  Updated
									gestalt calls to match new prototypes.
		<10>	 6/25/93	IH		Fix decl ROM lookup, record initialized in wrong place.
		 <9>	 6/25/93	IH		Fix DMAddDisplay and support smart monitors.
		 <8>	 6/22/93	IH		Changes for A2 build.  Support for Decl ROM timing lookups.
		 <7>	  6/1/93	IH		Switched around some selectors and added better mode clipping
									code. The idea is to ask the driver, but be able to find which
									modes are valid via the decl ROM for simple new video cards.
	   <SM6>	 5/28/93	IH		Fix unused var
		 <5>	 5/28/93	IH		#1081805,1086363: Cursor fixes (cursor has a nasty tendency to
									write on things it shouldn't if thing are not just SO).  Support
									screenBits updates when changing size of main display.
		 <4>	  4/8/93	IH		Add support for mode switching.
	   <SM3>	 3/31/93	IH		Remove a little debug code I left in by accident.
		 <2>	 3/31/93	IH		Fix CalcMenuBar call
		 <1>	 3/25/93	IH		first checked in
		 <5>	 3/24/93	IH		Fix reentrancy bug.
		 <4>	 3/24/93	IH		Change several device manipulation calls to do more with
									"update" parameter.  Add commented ideas for more
									palette-manager-friendly code changes.  Change SetMainDevice to
									be trap selector DM_SetMainDevice.  Rearrange SetMainDevice code
									(fix some bugs, add comments and use process manager IPC to
									propagate menu bar and gDevice changes to other processes).  Fix
									DM_RedrawAll to calc regions before drawing.  Cleaned out unused
									defines and changed #include files to be included only if they
									had not been included before.
		 <3>	 3/17/93	IH		Make Display Manager work with monitors control panel.
		 <2>	 2/25/93	IH		Clean up code for dual use with ThinkC environment.
		 <1>	 2/23/93	IH		first checked in

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

#ifndef __CRSRDEVPRIV__
#include "CrsrDevPriv.h"
#endif


// For Reality CallDMNotificationProc will always be defined in the
// universal c includes.  For SuperMario it is not yet defined.  Therefore,
// we conditionally define it here so that the sources are the same.
#ifndef CallDMNotificationProc
#define CallDMNotificationProc(userRoutine, theEvent)		(*userRoutine)(theEvent)
#endif


#ifndef CallDMExtendedNotificationProc
#define CallDMExtendedNotificationProc(userRoutine, theMessage, theData, theEvent)		(*userRoutine)(theMessage, theData, theEvent)
#endif


#define LomemMouse	0x0830								// [GLOBAL VAR]  processed mouse coordinate [long]*/ 
#define kQDGlobalsSize 	0xCE							// sizeof(struct qd) + thePort

#define screenRadiusDrw	22,22								// Horst Beepmanh’s “patented” method, you know.
#define screenRadiusBld	16,16								// D’oh! Drawing and building are different.
#define penRadius		 3, 3								// Part of the plan. [22 = 16 + 3 + 3]
#define penOutset		-3,-3								// It never ends.

#define TheCurrentA5	(*((Ptr *)(CurrentA5)))				// A better CurrentA5.

typedef struct
{	char		privates[72];
	RgnHandle	wideOpen;
	long		randSeed;
	BitMap		screenBits;
	Cursor  	arrow;
	Pattern 	dkGray;
	Pattern 	ltGray;
	Pattern 	gray;
	Pattern 	black;
	Pattern 	white;
	GrafPtr 	thePort;
	}
QuickDraw;

//
// Prototypes for static functions
//
OSErr			CallProcInAnyProcess(ProcessSerialNumberPtr thePSN, ProcPtr theProc,Ptr userData);
pascal void		MyDeviceLoopDrawingProcPtr(short depth, short deviceFlags, GDHandle targetDevice,long userData);
static THz		SetSystemZone(void);
static Boolean	DM_HideCursor(void);
static void		DM_ShowCursor(Boolean cursorHidden);
static Boolean	IsProcessMgrInstalled(void);
static Boolean	IsKNoProcess(ProcessSerialNumberPtr testProcess);
static Boolean	QDrawInitialized(void);
static Boolean	IsLCDScreen(GDHandle gDevice);
static GDHandle	DM_NewGDevice(short refNum, short mode);
static void		DM_DisposeGDevice(GDHandle disposeDevice);




#ifdef APPLICATION_PATCH_BUILD		// A hack so I can have my A5 when I build as part of an application
static unsigned long GetApplicationA5()
{
	unsigned long		appA5 = GetDMGlobalsPtr()->fAnimateA5;
	
	asm {
		Move.l			A5,D0
		move.l			appA5,A5
	}
}

static void SetRegisterA5(unsigned long savedA5)
{
	asm {
		Move.l			savedA5,A5
	}
}


#define SAVE_A5_IF_APPLICATION			unsigned long savedA5 = GetApplicationA5();
#define RESTORE_A5_IF_APPLICATION		SetRegisterA5(savedA5);
#else
#define SAVE_A5_IF_APPLICATION
#define RESTORE_A5_IF_APPLICATION

#endif


pascal GDHandle DM_GetNextScreenDevice(GDHandle theDevice,Boolean activeOnly)
{
	while( theDevice )
	{
		theDevice = (GDHandle )(*theDevice)->gdNextGD;
		if( IsActiveScreenDevice(theDevice,activeOnly) )
			break;
	}
	
	return(theDevice);
}


pascal GDHandle DM_GetFirstScreenDevice(Boolean activeOnly)
{
	GDHandle		firstDevice = GetDeviceList();
	
	if( !IsActiveScreenDevice(firstDevice,activeOnly) )
		firstDevice = DMGetNextScreenDevice(firstDevice,activeOnly);
	
	return(firstDevice);
}
				

pascal OSErr DM_InstallDisplayManager(void)
// Allocates memory and completes installation of Display Manager with the exception of those
// Things that require the Window Manager to be initialized
{
	DisplayManagerGlobalsPtr		dmGlobals;
	OSErr							installError;
	SelectorFunctionProcPtr			oldGestalt;
	THz								savedZone = GetZone();
	
	installError = CheckExpandMem(true);
	if( noErr == installError )
	{
		if( nil == GetDMGlobalsPtr() ) 
		{
			SetZone(SystemZone());
			dmGlobals = (DisplayManagerGlobalsPtr )NewPtrSysClear(sizeof(DisplayManagerGlobalsRec));
			if( dmGlobals )
			{
				dmGlobals->fSavedDMGlobals		= GetExpandMemDisplayManagerGlobals();	// DEBUGGING (allow new version to be patched in)
				
			//	dmGlobals->fDrawingPort			= &dmGlobals->fPortStorage;		// Done in DM_InitDisplayManager
				dmGlobals->fWorkingDeskRectRgn	= NewRgn();						// This is a temp region for the desktop 
				dmGlobals->fWorkingDeskFullRgn	= NewRgn();
				dmGlobals->fWorkingUpdateRgn	= NewRgn();
			//	dmGlobals->fDeskIconRgn			= nil;							// Already nil from Mem Alloc
				dmGlobals->fDesktopCacheRgn		= NewRgn();						// Cache of desktop rgn (what is showing under windows)
				dmGlobals->fAnimationCacheRgn	= NewRgn();						// Cache of animation rgn (behind windows AND icons)

				dmGlobals->fIconRgnInvalid		= true;
			//	dmGlobals->fDeskDrawNest		= 0;							// Already nil from Mem Alloc

				dmGlobals->fNextAnimateTime		= kNever;
				dmGlobals->fAnimatePSN.highLongOfPSN	= 0;
				dmGlobals->fAnimatePSN.lowLongOfPSN		= kNoProcess;

				dmGlobals->fDisplayNextID		= kFirstDisplayID;
			//	dmGlobals->fDisplayCount		= 0;
				dmGlobals->fDisplayEntrySize	= sizeof(DisplayInfoRec);
			//	dmGlobals->fDisplayTable		= 0;
								
				dmGlobals->fRemovedDevices		= nil;							// Make the RemovedDevices list empty.
				
				dmGlobals->fDevicesOverLap		= false;						// These will be set up dynamically as they…
				dmGlobals->fDevicesMirrored		= false;						// …apply.
				dmGlobals->fMirroringBlocked	= false;
				
			//	dmGlobals->fQDA5World			= 0;								// Initialized by DM_InitQDWorld
				dmGlobals->fQDGlobals			= NewPtrSys( kQDGlobalsSize );		// sizeof(struct qd)
				
				DM_InitQDWorld(dmGlobals);

				SetExpandMemDisplayManagerGlobals( (Ptr )dmGlobals);			// Save our new globals
				
				SetRectRgn(dmGlobals->fDesktopCacheRgn,-32768,-32768,32767,32767);	// Panic DeskRgn (for before window mgr is inited)

				//
				// Register Gestalt Selectors for Display Manager
				//
				
				if( noErr == installError )
				{
					installError = NewGestalt(gestaltDisplayMgrAttr,DM__GestaltProc);
					if( noErr != installError )
						installError = ReplaceGestalt(gestaltDisplayMgrAttr,DM__GestaltProc,&oldGestalt);
				}

				if( noErr == installError )
					installError = DMSetDisplayID(nil,0);

				// Install the gestalt selector LAST so that if something goes wrong,
				//	noone knows I am here.
				
				if( noErr == installError )
				{
					installError = NewGestalt(gestaltDisplayMgrVers,DM__GestaltProc);
					if( noErr != installError )
						installError = ReplaceGestalt(gestaltDisplayMgrVers,DM__GestaltProc,&oldGestalt);
				}
			}
			else
				SysError(dsMemFullErr);
		}
	}
	SetZone(savedZone);
	return(installError);
}





pascal OSErr DM_InitDisplayManager(void)
// Should be called after WMgrPort is set up.
{
	CGrafPtr						visPort;
	GrafPtr							savedPort;
	DisplayManagerGlobalsPtr		dmGlobals = nil;
	THz								savedZone;
	OSErr							initError = noErr;

	if( noErr == CheckExpandMem(false) )		// If ExpandMem is OK (ie DMInstallDisplayManager() was called)
		dmGlobals = GetDMGlobalsPtr();			// Then get my globals
		
	if( nil == dmGlobals )						// DANGER Just in case we have not been installed, try now
	{
		DMDebugStr("\p Installing Display Manager during InitDisplayManager");
		DMInstallDisplayManager();
		dmGlobals = GetDMGlobalsPtr();
	}

	if( dmGlobals )
	{
		if( (nil == dmGlobals->fDrawingPort) && QDrawInitialized()  )
		{
			savedZone = SetSystemZone();
			
			GetCWMgrPort(&visPort);
			if( nil != visPort && ((CGrafPtr )(-1)) != visPort )
			{
				dmGlobals->fDrawingPort = &dmGlobals->fPortStorage;
				
				GetPort(&savedPort);
				OpenCPort(dmGlobals->fDrawingPort);						// Create a port for desktop drawing
				CopyRgn(visPort->clipRgn,dmGlobals->fDrawingPort->visRgn);
				PortChanged((GrafPtr )dmGlobals->fDrawingPort);
				SetPort(savedPort);
			}
			SetZone(savedZone);
		}
		if( noErr == initError )
			initError = DMSetDisplayID(nil,0);							// Just make sure we catch any late comers

	}
	else
		initError = kDMSWNotInitializedErr;
	return(initError);
}


pascal OSErr DM_KillDisplayManager(void)
{
	GrafPtr								windowMgrPort;
	DisplayManagerGlobalsPtr			dmGlobals = GetDMGlobalsPtr();
	Ptr									savedDMGlobals = nil;

//	NEEDSFIX(6,"This call will go away when I am part of the system");
	if( dmGlobals )
	{
		GetWMgrPort(&windowMgrPort);
		SetPort(windowMgrPort);
		savedDMGlobals = dmGlobals->fSavedDMGlobals;
		
		if( dmGlobals->fDrawingPort)			CloseCPort(dmGlobals->fDrawingPort);
		if( dmGlobals->fWorkingDeskRectRgn )	DisposeRgn(dmGlobals->fWorkingDeskRectRgn);
		if( dmGlobals->fWorkingDeskFullRgn )	DisposeRgn(dmGlobals->fWorkingDeskFullRgn);
		if( dmGlobals->fWorkingUpdateRgn )		DisposeRgn(dmGlobals->fWorkingUpdateRgn);
		if( dmGlobals->fDesktopCacheRgn )		DisposeRgn(dmGlobals->fDesktopCacheRgn);
		if( dmGlobals->fAnimationCacheRgn )		DisposeRgn(dmGlobals->fAnimationCacheRgn);
		
		if( dmGlobals->fDisplayTable )			DisposeHandle((Handle )dmGlobals->fQDGlobals);
		if( dmGlobals->fQDGlobals )				DisposePtr(dmGlobals->fQDGlobals);
		if( dmGlobals->fNotificationTable )
		{
			// I could send close messages to all these notifications.
			// But the only reason we would be killing display manager would be to replace it
			// and we will preserve all these handles anyway.
			DisposeHandle((Handle )dmGlobals->fNotificationTable);
		}
		DisposePtr((Ptr )dmGlobals);
		SetExpandMemDisplayManagerGlobals(savedDMGlobals);
	}
	SetExpandMemFndrDeskRgnTracking(false);
	
	return(noErr);
}

static Boolean WindowMgrInitialized(void)
{
	return( 0 == *(char *)WWExist );
}


static Boolean QDrawInitialized(void)
{
	return( 0 == *(char *)QDExist );
}


static RgnHandle SafeGetGrayRgn(void)
// SafeGetGrayRgn can be called at init time.
{
	RgnHandle			theGrayRgn = GetGrayRgn();
	
	if( (-1L) == (long )theGrayRgn )	// Is GrayRgn initialized?
		theGrayRgn = nil;				// If no, return nil

		
	return( theGrayRgn );
}

typedef struct {
	short		portCount;
	GrafPtr		ports[];
} **PortListHdl;

#define PortList		0xD66

static PortListHdl SafeGetPortList(void)
{
	PortListHdl			thePortList = *(PortListHdl *)PortList;
	
	if( (-1L) == (long )thePortList )	// Is PortList initialized?
		thePortList = nil;				// If no, return nil
		
	return( thePortList );
}


static CGrafPtr SafeGetDeskPort(void)
{
	CGrafPtr			theDeskPort = (*(CGrafPtr *)(0x9E2));
	
	if( (-1L) == (long )theDeskPort )	// Is DeskPort initialized?
		theDeskPort = nil;				// If no, return nil
		
	return( theDeskPort );
}


static RgnHandle InternalGetDesktopRgn(void)
{
//	Before the window manager is initialized, the DeskPort does
//	not have the visrgn.  In particular, the first time the
//	desktop is drawn by InitWindows, DeskPort is invalid.
//	Therefore, if the windowmgr is not initialized, then I take the gray rgn.
	
	RgnHandle			theDeskRgn = nil;

	if( WindowMgrInitialized() )	
	{
		CGrafPtr			theDeskPort = SafeGetDeskPort();
	
		if( theDeskPort )
			theDeskRgn = theDeskPort->visRgn;
	}
	else
		theDeskRgn = SafeGetGrayRgn();

	if( nil == theDeskRgn )
	{
		// We are really hurting here and we need to return a dummy
		//		this is why this is never called from outside ( see DM_GetDeskRegion() comments )
		//		before window manager is up and running.
		
		theDeskRgn = GetDMGlobalsPtr()->fDesktopCacheRgn;
	}
	return( theDeskRgn );
}

static THz SetSystemZone()
{
	THz		currentZone = GetZone();
	
	SetZone(SystemZone());
	
	return(currentZone);
}


static void InternalDrawDesktopRgn(DisplayManagerGlobalsPtr	dmGlobals,RgnHandle drawRgn)
{
	DrawParamBlockRec			drawingParams;
	MyDeviceLoopRec				deviceLoopParams;
	CGrafPtr					savedPort;
	GDHandle					savedGDHandle;

	// DANGER Conditionalize so I do not bother when in ROM
#if !ForROM
	dmGlobals->fDeskDrawNest++;		// I only need to do this if I am patching QD to draw (ie not in ROM)
#endif

	GetGWorld(&savedPort,&savedGDHandle);
	SetPort((GrafPtr )dmGlobals->fDrawingPort);

	drawingParams.isThumbNail		= false;
	drawingParams.isAnimate			= false;
	drawingParams.fullRgn			= dmGlobals->fWorkingDeskFullRgn;
	drawingParams.drawFlags			= 0;
	drawingParams.drawRgn			= dmGlobals->fWorkingUpdateRgn;
	drawingParams.drawPort			= dmGlobals->fDrawingPort;

 	deviceLoopParams.drawingParams	= &drawingParams;
 	deviceLoopParams.updateRgn		= drawRgn;

	SetClip(InternalGetDesktopRgn());

	DeviceLoop(drawRgn,MyDeviceLoopDrawingProcPtr,(long )&deviceLoopParams,singleDevicesBit);
	
	if( 0 != dmGlobals->fAnimatePSN.highLongOfPSN ||  0 != dmGlobals->fAnimatePSN.lowLongOfPSN )
		WakeUpProcess(&dmGlobals->fAnimatePSN);			// Make sure our process wakes up to check for more animates (won't move memory)
	
	SetGWorld(savedPort,savedGDHandle);
	
	// DANGER Conditionalize so I do not bother when in ROM
#if !ForROM
	dmGlobals->fDeskDrawNest--;		// I only need to do this if I am patching QD to draw (ie not in ROM)
#endif
}

/*
//	IsLCDScreen() calls the video driver associated with gDevice (which is assumed to be a valid gDevice here)
//	to determine the connection type.  If the connection type is either FSTN or TFT, then IsLCDScreen returns
//	true.  Otherwise, it is just assumed that the connection type is some kind of CRT screen, so false is
//	returned.
*/
static Boolean IsLCDScreen(GDHandle gDevice)
{
	VDDisplayConnectInfoRec connectInfo;
	Boolean isLCDScreen = false;
	
	if (noErr == GetDisplayConnection(gDevice,&connectInfo))
		isLCDScreen = ((connectInfo.csDisplayType == kPanelTFTConnect) || (connectInfo.csDisplayType == kPanelFSTNConnect));

	return(isLCDScreen);
}

pascal void DM_DrawDesktopRect(Rect *drawRect)
//pascal OSErr DM_DrawDesktopRect(Rect *drawRect)
{
	SAVE_A5_IF_APPLICATION
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();
	RgnHandle					workingDeskRectRgn;
	
	if( dmGlobals && dmGlobals->fDrawingPort )
	{
		workingDeskRectRgn = dmGlobals->fWorkingDeskRectRgn;
		RectRgn(workingDeskRectRgn,drawRect);
		InternalDrawDesktopRgn(dmGlobals,workingDeskRectRgn);			// Should this be a trap?
	}
	else
	{
		workingDeskRectRgn = NewRgn();
		RectRgn(workingDeskRectRgn,drawRect);
		DrawDeskPattern(workingDeskRectRgn);
		DisposeRgn(workingDeskRectRgn);
	}
	RESTORE_A5_IF_APPLICATION
//	return(noErr);
}


pascal void DM_DrawDesktopRegion(RgnHandle drawRgn)
//pascal OSErr DM_DrawDesktopRegion(RgnHandle drawRgn)
{
	SAVE_A5_IF_APPLICATION
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();

	if( dmGlobals && dmGlobals->fDrawingPort )
		InternalDrawDesktopRgn(dmGlobals,drawRgn);
	else
		DrawDeskPattern(drawRgn);
	
	RESTORE_A5_IF_APPLICATION
//	return(noErr);
}

pascal OSErr DM_SetDesktopIconRgn(RgnHandle iconRgn)
{
	GetDMGlobalsPtr()->fDeskIconRgn = iconRgn;
	return(noErr);
}


pascal OSErr DM_GetDesktopIconRgn(RgnHandle *iconRgn)
{
	STOP_HERE();
	*iconRgn = GetDMGlobalsPtr()->fDeskIconRgn;

	return(noErr);
}


static pascal void DM_RedrawAll(RgnHandle clobberedRgn)
{	
	#pragma unused( clobberedRgn );

	if ( WindowMgrInitialized() )
	{
		GrafPtr		savedPort;
		GrafPtr		theWMgrPort;
		RgnHandle	theGrayRgn = SafeGetGrayRgn();
		
		GetWMgrPort(&theWMgrPort);
		GetPort(&savedPort);
		SetPort(theWMgrPort);
	
		RedrawAll();
	
		/*
		// If the GrayRgn is non-nil, then let’s draw the rounded corners.  For “hasDisplayMgrWindows”
		// builds, we’ll do things the new-and-improved way.  For everyone else, we just keep doing it
		// the tried-and-true way, including the un-squaring bug on LCD screens when the desktop changes.
		*/
		if ( theGrayRgn )
		{
			QuickDraw *qD = (QuickDraw *)(*(char **)TheCurrentA5 - (sizeof(QuickDraw) - sizeof(GrafPtr)));
			GDHandle walkDevice = DMGetFirstScreenDevice(true);
			RgnHandle savedClip = NewRgn();
			PenState savedPenState;
			Rect tempRect;
			Rect deskRect;
			
			SetRect(&tempRect,0,0,0,0);
			deskRect = tempRect;
			
			GetPenState(&savedPenState);
			PenNormal();
			PenSize(penRadius);
			
			if ( savedClip )
				GetClip(savedClip);
				
			// Set the WMgrPort.clipRgn to the wide open region.
			//
			SetClip(qD->wideOpen);
			
			#if hasDisplayMgrWindows
				while (walkDevice)
				{
					if (TestDeviceAttribute(walkDevice,roundedDevice))
					{
						tempRect = (*walkDevice)->gdRect;
						ClipRect(&tempRect);
						InsetRect(&tempRect,penOutset);
						FrameRoundRect(&tempRect,screenRadiusDrw);
					}
					
					walkDevice = DMGetNextScreenDevice(walkDevice,true);
				}
			#else
				while (walkDevice)
				{
					tempRect = (*walkDevice)->gdRect;
					UnionRect(&tempRect,&deskRect,&deskRect);
					
					walkDevice = DMGetNextScreenDevice(walkDevice,true);
				}
				ClipRect(&deskRect);
				InsetRect(&deskRect,penOutset);
				FrameRoundRect(&deskRect,screenRadiusDrw);
			#endif
	
			SetPenState(&savedPenState);
			
			if ( savedClip )
			{
				SetClip(savedClip);
				DisposeRgn(savedClip);
			}
		}
		
		SetPort(savedPort);
	}
}


pascal void DM_ChangedDesktopIconRgn(void)
{
	GetDMGlobalsPtr()->fIconRgnInvalid = true;
}



pascal Boolean DM_DeskClick(EventRecord* theEvent)
{
#pragma unused(theEvent);
	Boolean				ultraDeskClick = false;

//	NeedsFix(7,"We need to hit check items");

//	if( DebugCntlDownNow() )
//		ultraDeskClick = true;
	
	return(ultraDeskClick);
}


pascal OSErr DM_KeepWindowOnscreen(WindowPeek theWindow,RgnHandle oldGrayRgn)
// This is very experimental API and code.  Should we try to resize?  I think that
// Is up to the App which will get an AppleEvent.
//
// Can I get modal and doc info from the window?
//
// Warning:  This is an O(N*M) algorithm where N is the number of windows off screen and 
//				M is the number of monitors.
{
	GDHandle		maxCoverageDevice;
	short			windowVarient;
	Point			structOffset;
	Boolean			isMovable;					// Can you move this window?
	Boolean			needsMove = false;
	Boolean			wasOnScreen = true;
	Boolean			hasStructRgn;
	Rect			criticalRect;				// The area of a window that must stay on screen
	Rect			pinRect;					// Working Rect for temp calculations
	Rect			windowRect;					// Bounding rect of the window
	Rect			usableDeviceRect;			// Usable area on a device
	RgnHandle		windowRgn;
	RgnHandle		tempRgn;
	GrafPtr			savedPort;
	GrafPtr			theDeskPort = SafeGetDeskPort();
	

	if( theDeskPort && (((WindowPeek )theDeskPort)->updateRgn != theWindow->updateRgn) )		// We do not move the Finder desktop window
	{
		windowVarient = GetWVariant(&theWindow->port);
		// Applications that want to work correctly need to show in the process manager that they are 
		// Dynamic Desktop aware.
		//
		// What about notification dialogs?
		//
		// isMovable is really bogus because other wdefs might not use the same conventions.
		// We really want to be able to ask the wdef directly (new selector):
		//	• Is the window movable?
		//		If the window is moveable, then I do not have to be sure the entire window is visible
		//		If the window cannot be moved, then the entire window should be on screen.
		//	• What is the window's drag region?
		//		For moveable windows, I want to be sure that the part of the window that can be dragged
		//		is on screen (usually the title bar but sometimes the entire window can be dragged).
		// Other interesting questions might be:
		//	• Is the window modal
		//		Just on general principles.
		//
		// Should we ever change the WDEF API and have an opportunity to add calls, these would be good ones.
		// The drag region is a little harder because some apps drag some windows by clicks anywhere so
		// there would have to be a way for that information to be communicated.
		// 
		// Things to watch for:
		// 	Apps that draw directly into screen memory.
		//	Can apps count on their A5 to be set up when a custom WDEF is called?
		//	Can apps count on their resource chain to be set up when a custom WDEF is called?
	
		isMovable = ( 	(windowVarient == documentProc)		||	(windowVarient == noGrowDocProc)	||
						(windowVarient == movableDBoxProc)	||	(windowVarient == zoomDocProc)		||
						(windowVarient == zoomNoGrow)													);
		
		tempRgn			= NewRgn();
		windowRgn		= NewRgn();
		hasStructRgn	= !EmptyRgn(theWindow->strucRgn);
		
		GetWindowGlobalRegion(theWindow,windowRgn,&structOffset,isMovable,hasStructRgn);
		
		windowRect		= (*windowRgn)->rgnBBox;
		
		if( oldGrayRgn )
		{
			// If we got an oldGrayRgn, then we can tell more about
			//	where the window used to be.
			
			SectRgn(oldGrayRgn,windowRgn,tempRgn);			// Was the window onscreen at all?
			wasOnScreen = !EmptyRgn(tempRgn);				// If not then I will not put it on screen.
			
			if( wasOnScreen )								// If it was onscreen before, I can limit my critical area
				SectRgn(oldGrayRgn,windowRgn,windowRgn);	// to the intersetion of the old gray region and the whole window
		}

		if( wasOnScreen )
		{
			criticalRect	= (*windowRgn)->rgnBBox;

			if( isMovable )
			{
				// This is all approximate.  We assume that the title bar is on the top
				// This can still strand windows (for example floating palettes in some paint
				// programs have their drag bars on the left side -- this algorithm could strand a window 
				// positioned so its upper-right corner was still visible).
				
				criticalRect.bottom = criticalRect.top + structOffset.v;	// Get the title rect (approx)
				
				// DEBUG CODE
			//	criticalRect.right = criticalRect.left + 12;						// Truncate the title rect
				// DEBUG CODE
				
				RectRgn(tempRgn,&criticalRect);
				
				if( hasStructRgn )								// When a layer is hidden, window regions are empty
					SectRgn(theWindow->strucRgn,tempRgn,tempRgn);
				
				SectRgn(SafeGetGrayRgn(),tempRgn,tempRgn);
				
				needsMove = EmptyRgn(tempRgn);
				
			}
			else		// We need to be sure the entire window is on screen.
			{
				SectRgn(windowRgn,GetGrayRgn(),tempRgn);
				
				needsMove = !EqualRgn(windowRgn,tempRgn);				// If we are not completely on screen, we need to move the window
			}
		
			if( needsMove )
			{
				GetPort(&savedPort);
				SetPort(&theWindow->port);

				maxCoverageDevice = FindMaxCoverageDevice(&windowRect);
				if( nil == maxCoverageDevice )
					maxCoverageDevice = FindClosestDevice(&windowRect);
				
				pinRect = criticalRect;
				usableDeviceRect = (*maxCoverageDevice)->gdRect;
									
				if( DisplayHasMenu(maxCoverageDevice) )
					usableDeviceRect.top += GetMBarHeight(); 
				
				LocalPinRectToRect(&pinRect,&usableDeviceRect);
				InvalRgn(theWindow->contRgn);
				MoveWindow(&theWindow->port,windowRect.left + (pinRect.left-criticalRect.left)+structOffset.h,windowRect.top + (pinRect.top-criticalRect.top)+structOffset.v,false);
	
				SetPort(savedPort);
			}
		}
			
		DisposeRgn(windowRgn);
		DisposeRgn(tempRgn);
	}
	return(noErr);
}





static pascal OSErr KeepWindowsOnScreenAction(WindowPeek window, LayerPeek parent, void *privateData)
{
#pragma unused(parent);
	
	if( window )
	{
		if( !IsLayer( (WindowPtr )window) )
		{
			DisplaysStateHdl	displayState = (DisplaysStateHdl )privateData;
			RgnHandle			oldGrayRgn = nil;
			
			if( displayState )
				oldGrayRgn = (*displayState)->oldGrayRgn;
			
			
			DMKeepWindowOnscreen(window,oldGrayRgn);
			
	//		Move the windows and CalcVis regions
		}
	}
	else
		STOP_HERE();

	return(noErr);
}

static pascal void InterProcessAppCompatibilityProc(void *privateData)
// This call is made within the application's context.
{
	if( WindowMgrInitialized() )
	{
		AppCompatibilityPtr			theAppCompatibility = (AppCompatibilityPtr )privateData;
	
		EachWindow(firstWindow, afterLastWindow,theAppCompatibility->appLayer, KeepWindowsOnScreenAction,theAppCompatibility->displaysState);
	}
}


static OSErr CreateAppleEvent(ProcessSerialNumberPtr notifyPSN,AEDesc* notifyAEDesc,AppleEvent* theAppleEvent)
{
// This routine should fail only if memory is REALLY LOW.  Even if we run out of memory creating the full event,
//		we will try to make some event (otherwise apps will not get notified)
//
// Is there any way to get memory from other heaps?

	OSErr				createError = noErr;
	AEAddressDesc		notifyTarget;
	Boolean				emptyNotifyDesc;
	
	InitAEDesc(theAppleEvent);
	InitAEDesc(&notifyTarget);
	
	while( true )					// Only way out is breaking
	{
		emptyNotifyDesc = (notifyAEDesc->descriptorType == typeNull);
		createError = AECreateDesc(typeProcessSerialNumber,(Ptr )notifyPSN,sizeof(ProcessSerialNumber),&notifyTarget);
	
		if( noErr == createError )
			createError = AECreateAppleEvent(kCoreEventClass,kAESystemConfigNotice,&notifyTarget, kAutoGenerateReturnID, kAnyTransactionID, theAppleEvent);

		if( noErr == createError )
			createError = AEPutKeyDesc(theAppleEvent,kAEDisplayNotice,notifyAEDesc);		// Notify that the displays changed

		if( noErr == createError || emptyNotifyDesc )			// If we are done or dead, leave
			break;
		else													// Otherwise try to free up some room
		{
			AEDisposeDesc(notifyAEDesc);						// These get big, try killing it off
			AEDisposeDesc(theAppleEvent);						// Clean this up
			AEDisposeDesc(&notifyTarget);						// Clean this up
		}
	}
	AEDisposeDesc(&notifyTarget);								// We need to dispose the address descriptor
	
	return(createError);
}

static OSErr NotifyAProcess(ProcessSerialNumberPtr notifyPSN,AEDesc* notifyAEDesc)
{
	OSErr				notifyError;
	AppleEvent			theAppleEvent;

	notifyError = CreateAppleEvent(notifyPSN,notifyAEDesc,&theAppleEvent);
		
	if( noErr == notifyError )
	{
		BeginSystemMode();

			AevtDebugPrint(&theAppleEvent);

			notifyError = AESend(&theAppleEvent, nil, kAENoReply+kAECanInteract,kAEHighPriority,kAEDefaultTimeout,nil,nil);
		EndSystemMode();
	}

	AEDisposeDesc(&theAppleEvent);
	return(notifyError);
}


static void CallNotificationProc(DMProcPtrRegPtr theNotifyRegistration, short theMessage, AppleEvent* theAppleEvent)
{
	AppleEvent			spareEvent;
	#if ENHANCE_DISPLAY_CODE
	short		should_we_make_notifications_associated_with_a_process_be_notified_in_their_context_from_their_process;
	#endif
	
	if( nil == theAppleEvent )												// Do we have an apple event?
	{
		spareEvent.descriptorType = typeNull;								// If not, then make a null event.
		spareEvent.dataHandle = nil;
		theAppleEvent = &spareEvent;
	}

	if( 0 == ( theNotifyRegistration->registrationFlags & kExtendedNotificationProc) )
	{	
		if( kDMNotifyEvent == theMessage )									// Normal notification only takes the kDMNotifyEvent message
			CallDMNotificationProc((theNotifyRegistration->registrationProc), theAppleEvent);
	}
	else
	{
		CallDMExtendedNotificationProc( ((DMExtendedNotificationProcPtr )theNotifyRegistration->registrationProc),theNotifyRegistration->registrationUserData, theMessage, theAppleEvent);
	}
}

pascal OSErr DM_NotifyAllProcesses(DisplaysStateHdl displayState)
{
		// Copy the process notification list
		//	Walk notification list and notify each process and extension in list
		//
		// Walk processes list
		//	if process NOT in notification list, then call do compatibility
		//
		// For debug, check that no extra process are in the list

	OSErr						notifyError = kDMSWNotInitializedErr;			// Not initilized error
	OSErr						processError;
	ProcessSerialNumber			walkPSN;
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();

	AEDesc						notifyDesc;
	ProcessInfoRec				processInfo;
	FSSpec						processSpec;
	
	if( dmGlobals )
	{
		THz			savedZone = SetSystemZone();
		
		notifyError = BuildNotificationAppleEvent(displayState,&notifyDesc);
		
		if( noErr != notifyError )
		{
			DMDebugStr("\p Failed to create apple event.");
						
			// We can still go on (with blank notification)
			// These events may get quite large when there are multiple displays 
			// (1.5k for two displays).
			// 
			// We need to keep going if we fail.
			
			notifyError = noErr;		// We can still go on (with blank notification)
		}
		
		if( noErr == notifyError )
		{
			if( dmGlobals->fNotificationTable )				// Are there any procedures to notify?
			{
				short						notificationWalk = dmGlobals->fNotificationCount;
				AppleEvent					theAppleEvent;
				DMProcPtrRegRec				tempProcReg;

				while(notificationWalk--)					// Post Decrement i to get index into zero-based proc array
				{
					tempProcReg = (*dmGlobals->fNotificationTable)[notificationWalk];
					
					CreateAppleEvent(&walkPSN,&notifyDesc,&theAppleEvent);						// Even if I get an error, I still want to call the proc
			//		AevtDebugPrint(&theAppleEvent);
					CallNotificationProc(&tempProcReg,kDMNotifyEvent,&theAppleEvent);
					
					AEDisposeDesc(&theAppleEvent);				// Get rid of event
				}
			}
			
			if( IsProcessMgrInstalled() )						// Only call process mgr if it is around
			{
				walkPSN.highLongOfPSN			= 0;
				walkPSN.lowLongOfPSN			= kNoProcess;	
		
				processInfo.processInfoLength	= sizeof(ProcessInfoRec);
				processInfo.processName			= nil;
				processInfo.processAppSpec		= &processSpec;	
				
				while( noErr == (processError = GetNextProcess(&walkPSN)) )
				{
					noErr == GetProcessInformation(&walkPSN,&processInfo);			// Get information about the process
			
					#if DEBUG_DISPLAY_CODE
						if(		(0 == (processInfo.processMode & modeHighLevelEventAware)) &&		// Is App high level event aware?
								(0 != (processInfo.processMode & modeDisplayManagerAware)) )
							
								// This is possible if the application plans to ask for ProcPtr notification rather than event notification.
							// For now, I'll warn myself.
							DMDebugStr("\p Found app with DMAware, but no HLEventAware.");
					#endif
		
		
					if( (0 != (processInfo.processMode & modeDisplayManagerAware)) &&		// Is the App display manager aware?
						(noErr == DisplayManagerAwareBitCompatibility(&processInfo))	)	// Is its date late enough?
					{
						if( 0 != (processInfo.processMode & modeHighLevelEventAware) )		// Is App high level event aware?
						{
							NotifyAProcess(&walkPSN,&notifyDesc);
						}
						else
						{
							DMDebugStr("\p Found app with DMAware, but no HLEventAware with good date.");
						}
					}
					else
					{
							//
							//	For QuickDraw GX compatibility, make sure I make MoveWindow calls in the application's context!!
							//	GX only patches MoveWindow inside the application's context.
							//
			
						PEntryPtr				theProcessEntry;
						AppCompatibilityRec		theAppCompatibility;
		
						theProcessEntry = (PEntryPtr )PEntryFromProcessSerialNumber(&walkPSN);
			
						theAppCompatibility.appLayer = (LayerPeek )theProcessEntry->p_layer;
						theAppCompatibility.displaysState = displayState;
						
						CallProcInAnyProcess(&walkPSN, (ProcPtr )InterProcessAppCompatibilityProc,(Ptr )&theAppCompatibility);
					}
				}
			}
		}
		AEDisposeDesc(&notifyDesc);
		
		SetZone(savedZone);
	}
	return( notifyError );
}



static void InitDeviceBlock(DMDisplayBlocksPtr displayRegions)
{
	displayRegions->blockCount = 0;
	
	#if DEBUG_DISPLAY_CODE
	{
		register short		i;
		for(i=0;i<kMaxBlocks;)
			displayRegions->displayBlocks[i++] = 0;
	
	}
	#endif
}


static OSErr AddDeviceBlock(DMDisplayBlocksPtr displayRegions,GDHandle addDevice,DMDeviceBlockHdl *deviceBlock)
{
	OSErr					addError;
	DMDeviceBlockHdl		newBlock;
	Rect					newDeviceRect;
	RgnHandle				blockRgn = nil;

	// We do a new handle clear to avoid initializing all offset points and devices to zero.
	newBlock = (DMDeviceBlockHdl  )NewHandleClear(sizeof(DMDeviceBlockRec));
	addError = MemError();
	
	if( newBlock )
	{
		blockRgn = NewRgn();
		addError = MemError();
		
		
		if( blockRgn )
		{
			newDeviceRect = (*addDevice)->gdRect;
			RectRgn(blockRgn,&newDeviceRect);
			
			(*newBlock)->blockRgn		= blockRgn;				// Set the region for the block
			(*newBlock)->deviceCount	= 1;					// We initialize with a device
			(*newBlock)->devices[0]		= addDevice;			// Set the first device
			displayRegions->displayBlocks[displayRegions->blockCount++] = newBlock;
		}
	}
	
	
	if( addError )
	{
		if(blockRgn)		DisposeRgn(blockRgn);
		if(newBlock)		DisposeHandle( (Handle )newBlock);
		
		blockRgn = nil;
		newBlock = nil;
	}
	if( deviceBlock )
		*deviceBlock = newBlock;
	return(addError);
}


static void KillDeviceBlock(DMDisplayBlocksPtr displayRegions,short blockIndex)
{	
	displayRegions->blockCount -= 1;
	DisposeRgn((*displayRegions->displayBlocks[blockIndex])->blockRgn);
	DisposeHandle( (Handle )displayRegions->displayBlocks[blockIndex]);
	
	if( blockIndex < displayRegions->blockCount )
		BlockMove(&displayRegions->displayBlocks[blockIndex+1],&displayRegions->displayBlocks[blockIndex],
						sizeof(DMDeviceBlockHdl)*(displayRegions->blockCount-blockIndex));
	
	displayRegions->displayBlocks[displayRegions->blockCount] = 0;
}

static void KillAllDisplayBlocks(DMDisplayBlocksPtr displayRegions)
{
	short			blockIndex = displayRegions->blockCount;
	
	while( 0 < blockIndex )
	{
		KillDeviceBlock(displayRegions,--blockIndex);
	}
	#if DEBUG_DISPLAY_CODE
	{
		register short		i;

		for(i=displayRegions->blockCount;i<kMaxBlocks;)
			if( displayRegions->displayBlocks[i++] )
				STOP_HERE();
	
	}
	#endif
}


static OSErr CalculateDeviceRegions(DMDisplayBlocksPtr displayRegions,GDHandle keyDevice,Boolean makeKeyBlock,DMDeviceBlockHdl *keyBlock)
{
	//
	// Called when a display is removed or resized.  It calculates blocks of contiguous displays.  These
	//		blocks are later moved and merged to reform a single contiguous desktop.
	//
	//	Inputs:
	//		displayRegions		will contain blocks of contiguous devices
	//		keyDevice			is a device that is not considered when creating the blocks.  When deleting or resizing
	//								the key device is device for that display.
	//								NOTE:  the key device may or may not already be in the device list.
	//		makeKeyBlock		TRUE => the key device should have a block in the displayRegions.  For resizing you will
	//								still have the device.
	//							FALSE => the key device should not have a block in the displayRegions.  For deleting the device,
	//								you would not want a block in the displayRegions because you will not be merging it later.
	//	OutPuts:
	//		displayRegions		contains blocks of contiguous devices that need to be moved and merged into the new desktop
	//
	//
	// Yet another O(n^2) algorithm.  There is almost certainly a much more complex 
	// O( nLOG(n) ) algorithm to do this.  Before implementing it keep in mind the following:
	//
	// The O(n^2) algorithm may actually ship.
	// We sell 2 Meg Macs and they sometimes don't have much RAM left for segment trees etc.
	// This is not intended to be called often
	// I estimate 90% users have only one monitor
	// Powerbook duo users will be the biggest users and they have mostly 2 display (removing one => one display)
	// The most monitors our current machines can support is 6 which should still be doable.
	//
	// If these assumptions ever change we may need to review this algorithm.

	GDHandle			walkDevice;
	DMDeviceBlockHdl	buildingBlock;
	RgnHandle			deviceBlockRgn;
	Rect				deviceBlockRect;
	DMDeviceBlockRec	deviceArray;
	short				deviceWalker;

	short				blockCheckWalker;
	GDHandle			blockCheckDevice;
	Rect				blockCheckRect;
	RgnHandle			blockCheckRgn;

	OSErr				theError = noErr;
	
	
	blockCheckRgn				= NewRgn();
	
	deviceArray.deviceCount		= 0;
	walkDevice					= DMGetFirstScreenDevice(true);
	
	while( walkDevice )							// Build an array of the current screen devices (except the device we are removing
	{
		if( walkDevice != keyDevice )												// If it is not the device we are removing and it is active
			deviceArray.devices[deviceArray.deviceCount++] = walkDevice;			// Then add it to the list
		walkDevice = DMGetNextScreenDevice(walkDevice,true);								// Go on to the next deviced
	}
	
	deviceWalker = deviceArray.deviceCount;

	while( deviceWalker )
	{
		walkDevice = deviceArray.devices[--deviceWalker];
		if( walkDevice )															// If this device has not yet been assigned to a block
		{
			theError = AddDeviceBlock(displayRegions,walkDevice,&buildingBlock);	// Make another block
			if( noErr != theError )
				break;
			
			deviceBlockRgn = (*buildingBlock)->blockRgn;
			deviceBlockRect = (*walkDevice)->gdRect;								// Start a new block
			RectRgn(deviceBlockRgn,&deviceBlockRect);								// By creating a region
			
			blockCheckWalker = deviceWalker;
			while( 0 < blockCheckWalker )											// Walk the remaining devices to see if they are contiguous
			{
				blockCheckDevice = deviceArray.devices[--blockCheckWalker];			// Get a device to check
				if( blockCheckDevice )												// Only check devices not yet assigned to a block
				{
					blockCheckRect = (*blockCheckDevice)->gdRect;
					SetRectRgn(blockCheckRgn,	blockCheckRect.left-1,blockCheckRect.top-1,		// The check region is outset by one
												blockCheckRect.right+1,blockCheckRect.bottom+1);	// to get intersection on contiguous displays
					
					SectRgn(deviceBlockRgn,blockCheckRgn,blockCheckRgn);			// See if the device intersects the block
					
					if( !EmptyRgn(blockCheckRgn) )									// We have an intersection
					{
						RectRgn(blockCheckRgn,&blockCheckRect);						// Reset the region
						UnionRgn(deviceBlockRgn,blockCheckRgn,deviceBlockRgn);		// Make the block larger
						
						(*buildingBlock)->devices[(*buildingBlock)->deviceCount++] = blockCheckDevice;		// Add the device to the block
						
						deviceArray.devices[blockCheckWalker] = 0;					// Mark the device not to be checked again
						blockCheckWalker = deviceWalker;							// Here is the n sqrd part where I check the devices again
					}
				}
			}			
		}
	}

	if( makeKeyBlock && (noErr == theError) )
		theError = AddDeviceBlock(displayRegions,keyDevice,keyBlock);				// Make another block

	DisposeRgn(blockCheckRgn);
	return(theError);
}

static Rect* GetAdjustedDeviceRect(DMDeviceBlockPtr theDeviceBlock,short deviceIndex,Rect* adjustedRect)
// Must not move memory
{
	*adjustedRect = (*theDeviceBlock->devices[deviceIndex])->gdRect;
	OffsetRect(adjustedRect,theDeviceBlock->deviceOffset[deviceIndex].h,theDeviceBlock->deviceOffset[deviceIndex].v);
	
	return(adjustedRect);
}


static void MergeBlocks(DMDisplayBlocksPtr displayBlocks,short growBlockIndex,short mergeBlockIndex,Point mergeBlockDelta)
{
	short				baseDeviceCount;
	short				deviceIndex;
	DMDeviceBlockHdl	baseBlock,mergeBlock;
	Point				tempPoint;
	
	
	baseBlock		= displayBlocks->displayBlocks[growBlockIndex];
	mergeBlock		= displayBlocks->displayBlocks[mergeBlockIndex];
	
	OffsetRgn((*mergeBlock)->blockRgn,mergeBlockDelta.h,mergeBlockDelta.v);
	UnionRgn((*baseBlock)->blockRgn,(*mergeBlock)->blockRgn,(*baseBlock)->blockRgn);

	baseDeviceCount	= (*baseBlock)->deviceCount;
	deviceIndex		= (*mergeBlock)->deviceCount;
	
	while( 0 < deviceIndex-- )
	{
		(*baseBlock)->devices[baseDeviceCount+deviceIndex] = (*mergeBlock)->devices[deviceIndex];
		
		tempPoint = (*mergeBlock)->deviceOffset[deviceIndex];						// Get the device offset
		tempPoint.h	+= mergeBlockDelta.h;											// Adjust the offset by the merge delta
		tempPoint.v += mergeBlockDelta.v;
		(*baseBlock)->deviceOffset[baseDeviceCount+deviceIndex] = tempPoint;		// Copy the offset to the merged block
	}
	
	(*baseBlock)->deviceCount += (*mergeBlock)->deviceCount;						// Set # devices to the sum of the merged blocks 

	KillDeviceBlock(displayBlocks,mergeBlockIndex);
}

static void MakeDevicesContiguous(DMDisplayBlocksPtr displayRegions)
//		here we need to move the displays together to form a contiguous space.
{
	Boolean				tooHard = false;
	Boolean				swappedBlocks = false;
	short				deviceIndex;
	short				walkBlockIndex;
//	short				baseDeviceCount;
	DMDeviceBlockHdl	leftBlock,rightBlock;
	DMDeviceBlockHdl	tempBlock;
	Point				rightBlockDelta;
	Rect				rightmostDeviceRect;
	Rect				leftmostDeviceRect;
	Rect				tempRect;
	short				rightBlockIndex;
	short				leftBlockIndex;
	short				tempIndex;
	
	while( (1 < displayRegions->blockCount) && !tooHard )
	{
		// Try to do a smart merge of blocks
		tooHard = true;
	}

	if( tooHard )
	{
		walkBlockIndex	= displayRegions->blockCount;
		while( 0 < --walkBlockIndex )
		{
			SetRect(&leftmostDeviceRect,32767,32767,32767,32767);			// Start at far right
			SetRect(&rightmostDeviceRect,-32768,-32768,-32768,-32768);		// Start far left
						
			leftBlockIndex	= 0;
			rightBlockIndex	= walkBlockIndex;

			leftBlock	= displayRegions->displayBlocks[leftBlockIndex];
			rightBlock	= displayRegions->displayBlocks[rightBlockIndex];
			
			// If the rigth block is further to the left overall (as determined by the block region),
			// then we swap the blocks.
			if( (*(*rightBlock)->blockRgn)->rgnBBox.left < (*(*leftBlock)->blockRgn)->rgnBBox.left  )
			{
				tempBlock		= leftBlock;					// Swap blocks
				leftBlock		= rightBlock;
				rightBlock		= tempBlock;
				
				tempIndex		= leftBlockIndex;				// Swap Indices
				leftBlockIndex	= rightBlockIndex;
				rightBlockIndex	= tempIndex;
				swappedBlocks	= true;
			}
	
			// Find the rightmost device of the leftmost block
			deviceIndex = (*leftBlock)->deviceCount;
			while( 0 < deviceIndex-- )
			{
				if( rightmostDeviceRect.right < GetAdjustedDeviceRect(*leftBlock,deviceIndex,&tempRect)->right )
					rightmostDeviceRect = tempRect;
			}
	
	
			// Find the leftmost device of the rightmost block
			deviceIndex = (*rightBlock)->deviceCount;		
			while( 0 < deviceIndex-- )
			{
				if( leftmostDeviceRect.left > GetAdjustedDeviceRect(*rightBlock,deviceIndex,&tempRect)->left )
					leftmostDeviceRect = tempRect;
			}
			
			rightBlockDelta.h = rightmostDeviceRect.right - leftmostDeviceRect.left;
			
			if( rightmostDeviceRect.bottom < leftmostDeviceRect.top ||
				rightmostDeviceRect.top > leftmostDeviceRect.bottom )
				rightBlockDelta.v = rightmostDeviceRect.top - leftmostDeviceRect.top;		// If there is no vertical overlap then align tops of devices
			else
				rightBlockDelta.v = 0;		// If there is overlap then leave alignment alone
					
			MergeBlocks(displayRegions,leftBlockIndex,rightBlockIndex,rightBlockDelta);				
		}
	}
}


static void ResolveDisplayBlocks(DMDisplayBlocksPtr displayRegions)
{
	short				blockIndex;
	short				displayIndex;
	DMDeviceBlockHdl	displayBlock;
	Point				tempPoint;
	Point				deviceOffset;
	GDHandle			theDevice;
	
	tempPoint.h=tempPoint.v=0;
/*
	blockIndex = displayRegions->blockCount;
	while(0 < blockIndex--)								// Walk all the blocks
	{
		displayBlock = displayRegions->displayBlocks[blockIndex];
		displayIndex = (*displayBlock)->deviceCount;
		
		while(0 < displayIndex--)						// Walk all the displays in the block
		{
		}
	}
*/
	blockIndex = displayRegions->blockCount;
	while(0 < blockIndex--)								// Walk all the blocks
	{
		displayBlock = displayRegions->displayBlocks[blockIndex];
		displayIndex = (*displayBlock)->deviceCount;
		
		while(0 < displayIndex--)						// Walk all the displays in the block
		{
			deviceOffset = (*displayBlock)->deviceOffset[displayIndex];
			if( deviceOffset.h || deviceOffset.v )		// If the display is supposed to move, them move it
			{
				(*displayBlock)->deviceOffset[displayIndex] = tempPoint;
				theDevice = (*displayBlock)->devices[displayIndex];				
				DMMoveDisplay(theDevice,(*theDevice)->gdRect.left + deviceOffset.h,(*theDevice)->gdRect.top + deviceOffset.v,false);
			}
		}
	}
}

#if DEBUG_DISPLAY_CODE
pascal short DebugDisableMouseInturrupts(void)
    = {	0x40C0,							// Move.w		SR,D0
		0x3E80,							// Move.w		D0,(SP)		; Return old level
		0x0040, 0x0200,					// Ori.w		#2,D0		; Up the inturrupt level
		0x46C0}; 						// Move.w		D0,SR		; Set the inturrupt level

pascal void DebugRestoreInturrupts(short restore)
    = {	0x46DF}; 						// Move.w		(SP)+,SR	; Slam the status reg
#else
#define DebugDisableMouseInturrupts			inturrupts stuff is not for non-debug versions
#define DebugRestoreInturrupts				inturrupts stuff is not for non-debug versions
#endif



typedef long (*MBarProcPtr)(short selector,short message,short parameter1,long parameter2);


static void RebuildDeskRegions(RgnHandle savedGraySectDevicesRgn, Boolean drawMBar)
{
	RgnHandle theGrayRgn = SafeGetGrayRgn();
	RgnHandle theDeskRgn = NewRgn();
	RgnHandle tempRgn    = NewRgn();
	#pragma unused(savedGraySectDevicesRgn)

	if ( WindowMgrInitialized() && theGrayRgn && theDeskRgn && tempRgn )
	{	
		GDHandle walkDevice;
		CGrafPtr dmDrawPort;
		GrafPtr	 theWMgrPort;
		GrafPtr	 savedPort;
		Rect	 tempRect;
		
		DisplayManagerGlobalsPtr dmGlobals = GetDMGlobalsPtr();
		
		// Get some useful values up front.
		//
		GetWMgrPort(&theWMgrPort);
		GetPort(&savedPort);
		SetPort(theWMgrPort);
		
		dmDrawPort = dmGlobals->fDrawingPort;

		/*
		// Reinitialize the DeskRgn, the union of all screen devices.  Set WMgrPort.visRgn to
		// to DeskRgn.
		*/
		walkDevice = DMGetFirstScreenDevice(true);
		while (walkDevice)
		{
			tempRect = (*walkDevice)->gdRect;
			RectRgn(tempRgn,&tempRect);
			UnionRgn(tempRgn,theDeskRgn,theDeskRgn);
			
			walkDevice = DMGetNextScreenDevice(walkDevice,true);
		}
		CopyRgn(theDeskRgn,theWMgrPort->visRgn);	
		
		// Draw the empty menubar, leaving the clip region set to the menubar; or just set
		// the clip region to the menubar.
		//
		CalcMBHeight();
		
		if (drawMBar)
			DrawMBar();
		else
			GetMenuRgn(theWMgrPort->clipRgn);

		/*
		// Reinitialize the GrayRgn.  (Note that for “hasDisplayMgrWindows” builds, we use the new-and-improved
		// method of creating the rounded vs. square desktop.  For the non-“hasDisplayMgrWindows” builds,
		// we use the old HORROR/current-SuperMario method.  I suppose that eventually the former method
		// will actually be rolled into SuperMario.  Hopefully, when that happens, this code will have
		// been rolled in as well!)
		*/
		#if hasDisplayMgrWindows
			SetEmptyRgn(theGrayRgn);
			walkDevice = DMGetFirstScreenDevice(true);
			while (walkDevice)
			{
				tempRect = (*walkDevice)->gdRect;

				if (dmGlobals->fDevicesMirrored || IsLCDScreen(walkDevice))
				{	
					RectRgn(tempRgn,&tempRect);
					SetDeviceAttribute(walkDevice,roundedDevice,false);
				}
				else
				{	
					OpenRgn();
					FrameRoundRect(&tempRect,screenRadiusBld);
					CloseRgn(tempRgn);
					SetDeviceAttribute(walkDevice,roundedDevice,true);
				}
				
				UnionRgn(theGrayRgn,tempRgn,theGrayRgn);
				walkDevice = DMGetNextScreenDevice(walkDevice,true);
			}
		#else
			tempRect = (*theDeskRgn)->rgnBBox;
			OpenRgn();
			
			// If the menubar screen is an LCD screen, then make it square.  Otherwise, round it.
			//
			if (IsLCDScreen(GetMainDevice()))
				FrameRect(&tempRect);
			else
				FrameRoundRect(&tempRect,screenRadiusBld);
			
			CloseRgn(theGrayRgn);
		#endif

		// Now, subtract out the MenuBar from the GrayRgn.  (The WMgrPort.clipRgn is
		// the menubar region from the calls to the MBDF above.)
		//
		DiffRgn(theGrayRgn,theWMgrPort->clipRgn,theGrayRgn);
		
		// Finally, intersect the GrayRgn with the DeskRgn.
		//
		SectRgn(theDeskRgn,theGrayRgn,theGrayRgn);

		// Set the WMgrPort.clipRgn to the GrayRgn.
		//
		SetClip(theGrayRgn);
		
		if( dmDrawPort )
			CopyRgn(theGrayRgn,dmDrawPort->visRgn);

		SetPort(savedPort);
	}
	
	// Clean up and go home.
	//
	if ( theDeskRgn )
		DisposeRgn(theDeskRgn);
	if ( tempRgn )
		DisposeRgn(tempRgn);
}

static OSErr CheckAndUpdateDisplaysLayout(DisplaysStateHdl displayState)
{
	RebuildDeskRegions(nil,false);
	DM_RedrawAll(nil);
		
	DMNotifyAllProcesses(displayState);

	return( noErr );
}


typedef pascal void (*UserDataProc)(Ptr userData);

pascal void InterProcessChangeMainDevice( GDHandle newMainDevice )
{
	// DANGER -- should I check for the device to be set to the old main device?
	if( QDrawInitialized() )
	{
		UpdateScreenBits(newMainDevice);
	}
	// Recalc the menu bar only in processes with windows (ie, NOT in background Apps)	
	if( WindowMgrInitialized() )
	{
		// NOTE -- When I am sure that _CalcMenuBar is available,
		//	I will change this to CalcMenuBar(0);
		RecalcMenuBar();
	}
}


pascal void InterProcessChangedModes( GDHandle changedDevice )
{
#pragma unused(unused);
	if( QDrawInitialized() )
	{
		if( GetMainDevice() == changedDevice )
			UpdateScreenBits(changedDevice);
	}

	// Recalc the menu bar only in processes with windows (ie, NOT in background Apps)	
	if( WindowMgrInitialized() )
	{
		// DANGER -- When I am sure that _CalcMenuBar is available,
		//	I will change this to CalcMenuBar(0);
		RecalcMenuBar();
	}
}


static OSErr CallProcInAnyProcess(ProcessSerialNumberPtr thePSN, ProcPtr theProc,Ptr userData)
// Process Manager returns an error if you try to use CallProcInProcess on the current process
// so I check to see if the process is the current process.  If it is not then I use the process
// manager call, otherwise I just make a direct proc call.
{
	OSErr					processError = noErr;
	Boolean					isSameProcess;
	ProcessSerialNumber		currentprocess;

	currentprocess.highLongOfPSN = 0;
	currentprocess.lowLongOfPSN	= kCurrentProcess;	

	if( !IsProcessMgrInstalled() || ((noErr == SameProcess(&currentprocess,thePSN,&isSameProcess)) && isSameProcess) )
		(*(UserDataProc )theProc)(userData);			// Call for my process directly
	else
		processError = CallProcInProcess( thePSN,theProc,userData,sizeof( Ptr ) );

	return( processError );
}


OSErr CallAllProcesses(ProcPtr theProc,Ptr userData)
{
	OSErr					processError;
	ProcessSerialNumber		walkPSN;
	
	if( IsProcessMgrInstalled() )
	{
		walkPSN.highLongOfPSN	= 0;
		walkPSN.lowLongOfPSN	= kNoProcess;	
		
		while( noErr == (processError = GetNextProcess(&walkPSN)) )
			CallProcInAnyProcess(&walkPSN, theProc, userData);
	}
	else
		CallProcInAnyProcess(&walkPSN, theProc, userData);

	return( noErr );
}

void FixLowMem(GDHandle oldMainDevice,GDHandle newMainDevice,short deviceOffsetX,short deviceOffsetY)
{
#pragma unused(oldMainDevice, deviceOffsetX, deviceOffsetY);

	PixMapHandle				newMainPixMap;
	Rect						tempRect;
	Rect						newMainRect;
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();
	Boolean						roundMenuBar;
	
	newMainPixMap			= (*newMainDevice)->gdPMap;
	newMainRect				= (*newMainDevice)->gdRect;				// 72 dpi rect
			
	*(short *)ScrVRes = HiWord((*newMainPixMap)->vRes);				// [GLOBAL VAR] Pixels per inch vertically (word)
	*(short *)ScrHRes = HiWord((*newMainPixMap)->hRes);				// [GLOBAL VAR] Pixels per inch horizontally (word)
	
	tempRect			= (*newMainPixMap)->bounds;
	
	// DANGER!!
	// Set these using slot manager information
	//
	*(Ptr *)ScrnBase	= (*newMainPixMap)->baseAddr;				// [GLOBAL VAR] Address of main screen buffer Screen Base [pointer]
	*(short *)ScreenRow	= ((*newMainPixMap)->rowBytes & 0x7FFF);	// [GLOBAL VAR]  rowBytes of screen [word]

	// DANGER - should get this from vpRowBytes in slot manager
	*(short *)ScreenRow	= (((newMainRect.right-newMainRect.left)+15)/16)*2;		// Assumes 1-bit for compatibility
	
	// Make sure the “squareness” of the MenuBar is preserved, but default to “rounded” if we can’t
	// do anything else.
	//
	// Exception:  If mirroring is on, we ALWAYS want the menubar to be square because this helps
	//			   QuickDraw out a lot speedwise.
	//
	roundMenuBar = !dmGlobals->fDevicesMirrored;
	if (roundMenuBar)
		roundMenuBar = !IsLCDScreen(newMainDevice);
	*(Boolean *)ntsc = roundMenuBar;

	DMMoveCursor(nil,nil);
}


void FixPorts(
		GDHandle		newMainDevice,		Boolean			alwaysRestoreColors,
		Rect*			newMainRect,		PixMapHandle	newMainPixMap,
		Rect*			oldMainRect,		Ptr				oldBaseAddress,
		CTabHandle		oldCTable)
{
	PixMapHandle	portPixMap;
	Ptr				portBaseAddr;
	Boolean			needColorSlam;							// If the port shares the same color table handle as the old main device, we cannot just copy it
	Boolean			portColor;								// Is the walk port color?
	short			index;									// Index for walking port list
	GrafPtr			walkPort;								// Current port while walking the portlist
	CGrafPtr		savedPort;
	GDHandle		savedDevice;
	long			savedPortCTabSeed;						// Saved color table seed from port (so we know if colors are correct)
	Boolean			isPortAWindow;
	Boolean			isPortALayer;
	AuxWinHandle	colors;
	Boolean			isWindowMgrInitialized = WindowMgrInitialized();
	PortListHdl		thePortList = SafeGetPortList();
	RgnHandle		oldMainRgn	= NewRgn();

	// Color2Index() uses the current GDevice, so I need to set the device to the new device, otherwise
	//	my fixes for the port's fgColor and bgColor do not work (since they use the old main device's 
	//	color table which is the wrong one)
	
	if( thePortList )
	{
		GetGWorld(&savedPort,&savedDevice);
		SetGDevice(newMainDevice);
		RectRgn(oldMainRgn,oldMainRect);
		
		
		index = (*thePortList)->portCount;
		while( 0 < index )
		{
			walkPort = (*thePortList)->ports[--index];
				
			SetPort(walkPort);				// Give CloseView patches a chance to work
			
			isPortAWindow = ( isWindowMgrInitialized && GetAuxWin(walkPort,&colors) );
			isPortALayer = ( isPortAWindow && IsLayer(walkPort) );
	
			
			if( portColor = DM__IsColorPort(walkPort) )
			{
				portBaseAddr	= (*((CGrafPtr )walkPort)->portPixMap)->baseAddr;
				portPixMap		= ((CGrafPtr )walkPort)->portPixMap;
		
			}
			else
			{
				portPixMap		= nil;					// Not needed
				portBaseAddr	= walkPort->portBits.baseAddr;
			}
			
			if( portBaseAddr == oldBaseAddress )						// If it is a screen port
			{			
				if( portColor )
				{
					Rect			savedPortPixMapBounds;					// Saved from pixmap
					
							// Save the color table seed so we can determine if the new device has
							//	a different color table.  If it does then the port's fgColor and bgColor
							//	are invalid and will need to be updated.
							
					savedPortCTabSeed = (*(*portPixMap)->pmTable)->ctSeed;
					
							// Save the port bounds in case this port does not have the default bounds.
							//	Pixmap bounds are copied by CopyPixMap and if the port's pixmap bounds
							//	do not match the original device bounds, then I probably do not want to
							//	jam the new device bounds rect over it.
							
					savedPortPixMapBounds = (*portPixMap)->bounds;
					
							// The color table of the port's pixmap is often the same handle as
							//	the main device.  If this is the case, then we need to replace it
							//	with the color table of the new pixmap.  It would be bad to later
							//	remove a device and have its color table handle still used in some
							//	random ports.
					
					needColorSlam = ( (*portPixMap)->pmTable == oldCTable );
					
							// ColorQD CopyPixMap gracefully ignores a nil color table, so by forcing a 
							//	nil into the port's pixmap we can later jam the color table from 
							//	the new main device without worrying about memory leaks etc.
							//
							//	Unfortunately, NQD does not gracefully ignore a nil color table, so rather
							//	than nil the port's pixmap, we pre-jam it with the now pixmap's color table
							//	so that copying the handle to itself does nothing.
					
					(*portPixMap)->pmTable = (*newMainPixMap)->pmTable;		// Make it copy itself
					
					CopyPixMap(newMainPixMap,portPixMap);
											
					if( !needColorSlam )
						(*portPixMap)->pmTable = oldCTable;					// If there was a special color table, restore it.
							
							// The pixmap bounds offset is wiped out by the CopyPixMap.  It needs to be offset
							//	by the same amount that to maintain its global position.
					
					(*portPixMap)->bounds = *newMainRect;
					OffsetRect( &(*portPixMap)->bounds,savedPortPixMapBounds.left,savedPortPixMapBounds.top);						
						
										
							// Make sure the port's fgColor and bgColor are correct.  fgColor and bgColor will be
							//	wrong if main device to a device with a different color table.  This is especially 
							//	noticable when you switch from indexed to direct.
							
							// If the color table for the new device is different, we need to update the port's fgColor and bgColor
							//	If we reconfigured the main device (vs changing it), then the color seed has already been hammered
							//	and we need to restore the colors anyway.
							
					if( savedPortCTabSeed != (*(*newMainPixMap)->pmTable)->ctSeed || alwaysRestoreColors )
					{
						ColorSpec	tempColorSpec;
						
						SaveFore(&tempColorSpec);
						RestoreFore(&tempColorSpec);
			
						SaveBack(&tempColorSpec);
						RestoreBack(&tempColorSpec);
					}
				}
				else
				{
					Rect				savedPortBitMapBounds = walkPort->portBits.bounds;					// Saved from port bit map
					
					walkPort->portBits.baseAddr = (*newMainPixMap)->baseAddr;
					walkPort->portBits.rowBytes = ((*newMainPixMap)->rowBytes & 0x3FFF);
					walkPort->portBits.bounds	= *newMainRect;
					
					OffsetRect( &walkPort->portBits.bounds,savedPortBitMapBounds.left,savedPortBitMapBounds.top);
				}
					
				if( !isPortAWindow || isPortALayer )		// Layers have VisRegions that are bounded by the maindevice
				{
					if( EqualRect(oldMainRect,&walkPort->portRect) )		// Hits most ports and layers
						walkPort->portRect = *newMainRect;
		
					if( EqualRgn(oldMainRgn,walkPort->visRgn) )				// Hits most ports and layers
						RectRgn(walkPort->visRgn,newMainRect);
	
					if( EqualRgn(oldMainRgn,walkPort->clipRgn) )			// This one doesn't really happen because the clip is usually wide open on ports
						ClipRect(newMainRect);
				}
				PortChanged(walkPort);
			}
		}
		SetGWorld(savedPort,savedDevice);
	}
	if( oldMainRgn )
		DisposeRgn(oldMainRgn);
}



void FixWindowMgrPorts(GDHandle newMainDevice)
{
	if( SafeGetGrayRgn() )
	{
		CGrafPtr		theWMgrCPort;
		GrafPtr			theWMgrPort;
		
		// Update the port rectangles.
		//
		GetCWMgrPort(&theWMgrCPort);
		theWMgrCPort->portRect = (*newMainDevice)->gdRect;
		GetWMgrPort(&theWMgrPort);
		theWMgrPort->portRect = (*newMainDevice)->gdRect;
		
		// Rebuild the desktop regions.
		// 
		RebuildDeskRegions(nil,false);
		
		// Say that things changed.
		//
		PortChanged(theWMgrPort);
		PortChanged((GrafPtr )theWMgrCPort);
	}
}


static Boolean IsKNoProcess(ProcessSerialNumberPtr testProcess)
{
	return( nil == testProcess || (0 == testProcess->highLongOfPSN && kNoProcess == testProcess->lowLongOfPSN) );
}

static Boolean IsProcessMgrInstalled(void)
{
// Checked with Dave Faulkenburg and this is a valid test for process manager.
//		Another way is to check expand mem, but I don't need to be that fast.
	long					response;
	return( noErr == Gestalt(gestaltOSAttr,&response) );
}



Boolean IsProcessMgrOkay(void)
// IsProcessMgrOkay() returns true if either
//	1) There is no process manager OR
//	2) The process manager supports interprocess calls
{
	long					response;
	Boolean					isOkay = true;
	
	if( noErr == Gestalt(gestaltOSAttr,&response) )			// If process mgr 
		isOkay = (0 != (response & (1<<10)));				// gestaltBgndMouseDownSupport -- DANGER FIX THIS IN GestaltEqu.h

	return(isOkay);
}

OSErr PrepImmediateUpdate(OSErr currentError, DisplaysStateHdl *displayState,THz *savedZone)
{
	OSErr			tempError;
	
	*savedZone = SetSystemZone();
	tempError = DMPrivateBeginConfigureDisplays(displayState,true);
	
	if( noErr == currentError )					// If we have no error and we have no state
		currentError = tempError;

	return( currentError );
}

OSErr FinishImmediateUpdate(OSErr currentError, DisplaysStateHdl displayState,THz savedZone)
{
	OSErr			tempError;

	tempError = DMEndConfigureDisplays( (Handle )displayState);
	if( noErr == currentError )				// if there is no error so far
		currentError = tempError;			// Then return the error I got from ending the configuration
	
	SetZone(savedZone);
	return( currentError );
}


pascal OSErr DM_SetMainDisplay(GDHandle newMainDevice,DisplaysStateHdl displayState)
{
//	DANGER:  should use pointers for gDevices when I get a bunch of fields.

	GDHandle		oldMainDevice	= GetMainDevice();
	GDHandle		oldTheGDevice	= GetGDevice();
	PixMapHandle	newMainPixMap,mainPixMap;
	Ptr				newMainBaseAddr;
	Ptr				mainBaseAddr;
	Rect			newMainRect;
	Rect			oldMainRect;
	GDHandle		walkDevice;
	short			deviceOffsetX,deviceOffsetY;			// Amount to move monitors to put main device at 0,0
	OSErr			setMainError = noErr;
	Rect			tempRect;
	THz				savedZone;
	Boolean			cursorState = false;
	long			mainDeviceSize;										// Size of the gDevice
	Handle			tempGDevice;

	mainDeviceSize = GetHandleSize( (Handle )newMainDevice);			// Shannon sez we may have expanded gDevices.  I assume a devices sizes are the same
	setMainError = MemError();
	
	if( noErr == setMainError )
	{
		tempGDevice = NewHandleSys( mainDeviceSize );					// Make a temp device big enough for blockmoves
		setMainError = MemError();
	}

	if( noErr == setMainError && !IsProcessMgrOkay() )
		setMainError = kSysSWTooOld;				// Cannot move monitor without process manager IPC support

	if( noErr == setMainError && (oldMainDevice == newMainDevice))
		setMainError = paramErr;
	
	setMainError = PrepImmediateUpdate(setMainError, &displayState, &savedZone);

	if( noErr == setMainError )
	{
		cursorState			= DM_HideCursor();

		oldMainRect			= (*oldMainDevice)->gdRect;
		mainPixMap			= (*oldMainDevice)->gdPMap;

		newMainPixMap		= (*newMainDevice)->gdPMap;
		newMainBaseAddr		= (*newMainPixMap)->baseAddr;
		
		deviceOffsetX		= -(*newMainDevice)->gdRect.left;
		deviceOffsetY		= -(*newMainDevice)->gdRect.top;
		
		newMainRect			= (*newMainDevice)->gdRect;
		
		OffsetRect(&newMainRect,deviceOffsetX,deviceOffsetY);
		
		if( noErr == setMainError )
		{
			tempRect = newMainRect;
			setMainError = DisplayManagerCloseViewCompatibility(oldMainDevice,&tempRect,(*newMainPixMap)->pixelSize);
		}
	}
	if( noErr == setMainError )
	{
		mainBaseAddr		= (*mainPixMap)->baseAddr;				// Must happen AFTER compatibility because closeview may change base address

		for( walkDevice = DMGetFirstScreenDevice(true); walkDevice; walkDevice = DMGetNextScreenDevice(walkDevice,true) )
		{
			OffsetRect( &(*(*walkDevice)->gdPMap)->bounds,deviceOffsetX,deviceOffsetY);
			OffsetRect( &(*walkDevice)->gdRect,deviceOffsetX,deviceOffsetY);
		}
		
		{
			Handle			saveOldNextDevice;
			Handle			saveNewNextDevice;
			Boolean			allDeviceInited;
			
			// By block copying the gdevices we can support applications that cache the main device.
			//
			//		Test with accelerator cards and we may suck.
			//
			//
			// DANGER:   IF ANY USE HAPPENS DURING INTURRUPT TIME!!
			//
			// CURSOR HIDDEN
			// APPLESHARE?
			//
			allDeviceInited = TestDeviceAttribute(GetDeviceList(),allInit);
			
			saveOldNextDevice = (*oldMainDevice)->gdNextGD;
			saveNewNextDevice = (*newMainDevice)->gdNextGD;
			
			BlockMove( (Ptr )*oldMainDevice,	(Ptr )*tempGDevice,		mainDeviceSize );
			BlockMove( (Ptr )*newMainDevice,	(Ptr )*oldMainDevice,	mainDeviceSize );
			BlockMove( (Ptr )*tempGDevice,		(Ptr )*newMainDevice,	mainDeviceSize );

			DisposeHandle(tempGDevice);										// We are done with our temp device
			tempGDevice = nil;
			
			(*oldMainDevice)->gdCCDepth = 0;								// Stomp the cursor depth
			(*newMainDevice)->gdCCDepth = 0;								// Stomp the cursor depth

			(*oldMainDevice)->gdNextGD = saveOldNextDevice;
			(*newMainDevice)->gdNextGD = saveNewNextDevice;

			DM_SwapDevices(oldMainDevice, newMainDevice);

			tempGDevice = (Handle )oldMainDevice;
			oldMainDevice = newMainDevice;
			newMainDevice = (GDHandle )tempGDevice;
			
			SetDeviceAttribute(newMainDevice, mainScreen, true);			// Set Main Device flag on new main device
			SetDeviceAttribute(oldMainDevice, mainScreen, false);			// Clear Main Device flag on old main device
			SetDeviceAttribute(newMainDevice, ramInit,true);				// Old main device is sometimes NOT set to be ramInited.  but since it is not at 0,0 anymore, it needs to be inited.
			SetDeviceAttribute(oldMainDevice, ramInit,true);				// Old main device is sometimes NOT set to be ramInited.  but since it is not at 0,0 anymore, it needs to be inited.

			SetDeviceAttribute(GetDeviceList(),allInit,allDeviceInited);	// Make sure all devices are inited if they were before

			GDeviceChanged(oldMainDevice);
			GDeviceChanged(newMainDevice);
	
			InitGDevice((*oldMainDevice)->gdRefNum,-1,oldMainDevice);
			InitGDevice((*newMainDevice)->gdRefNum,-1,newMainDevice);
		}

		FixLowMem(oldMainDevice,newMainDevice,deviceOffsetX,deviceOffsetY);
		FixPorts(newMainDevice,false,&newMainRect,newMainPixMap,&oldMainRect,mainBaseAddr,(*mainPixMap)->pmTable);	
		FixWindowMgrPorts(newMainDevice);
		
		CallAllProcesses( (ProcPtr )InterProcessChangeMainDevice,(Ptr )newMainDevice);
	}	
	DM_ShowCursor(cursorState);

	return( FinishImmediateUpdate(setMainError, displayState, savedZone) );
}


pascal OSErr DM_AddDisplay(	GDHandle			newDevice,
							short				driverRefNum,
							unsigned long		mode,
							unsigned long		reserved,
							DisplayIDType		displayID,					// Pass nil to assign new ID
							ComponentInstance	displayComponent,			// Pass nil to leave as is
							DisplaysStateHdl	displayState)
//	Devices are always added in the disabled state
{
#pragma unused(reserved);

	OSErr			addError;
	GDHandle		followDevice,walkDevice;
	THz				savedZone;
	
	addError = PrepImmediateUpdate(noErr, &displayState, &savedZone);

	if( noErr == addError )
	{
		followDevice = walkDevice = GetDeviceList();
		while(walkDevice)
		{
			if(walkDevice == newDevice)
			{
				addError = kDMDisplayAlreadyInstalledErr;
				break;
			}
			followDevice = walkDevice;
			walkDevice = (GDHandle )(*walkDevice)->gdNextGD;
		}
		
		if( noErr == addError )
		{
		#if DEBUG_DISPLAY_CODE
			short		check_for_server_case_with_only_a_single_fake_device;
			short		does_mirroring_mind_having_devices_added_at_the_end;
		#endif
			
			SetDeviceAttribute(newDevice,screenActive,false);			// Make sure the device is marked as disabled.
			InitGDevice(driverRefNum,mode,newDevice);					// Initialize the device (redundant for devices that have just been added, but everyone patches InitGDevice so this is probably a good idea)
			(*followDevice)->gdNextGD = (Handle )newDevice;				// Put display into display list (is this a problem for mirroring?)

			addError = DMSetDisplayID(newDevice,displayID);				// Do this first so when the component gets called, it is with the correct number
	
			if( (noErr == addError) && (nil != displayComponent) )		// If we have a display component, use it
				addError = DMSetDisplayComponent(newDevice,displayComponent);

		}
	}
	return( FinishImmediateUpdate(addError, displayState, savedZone) );
}

static GDHandle DM_NewGDevice(short refNum, short mode)
{
	DisplayManagerGlobalsPtr dmGlobals = GetDMGlobalsPtr();
	GDHandle newDevice = dmGlobals->fRemovedDevices;
	
	GDHandle walkDevice;
	short rightEdge;
	Point gdOffset;
	Rect tempRect;
	
	// Unfortunately, lot’s o’ folks cache various parts of GDevices. So, we
	// maintain a linked list of removed GDevices in the privates.  If
	// we ever need to create a new device, we first look in the removed
	// GDevices list, and recycle from there.
	//
	if ( newDevice )
	{	
		dmGlobals->fRemovedDevices = (GDHandle)(*newDevice)->gdNextGD;
		(*newDevice)->gdMode = 0;
		
		InitGDevice(refNum,mode,newDevice);
	}
	else
		newDevice = NewGDevice(refNum,mode);
		
	// Initialize this GDevice’s rectangles to be as rightmost as possible.
	//
	gdOffset.h = gdOffset.v = 0;
	
	walkDevice = DMGetFirstScreenDevice(false);
	while (walkDevice)
	{
		rightEdge = (*walkDevice)->gdRect.right;
		
		if (rightEdge > gdOffset.h)
			gdOffset.h = rightEdge;
			
		walkDevice = DMGetNextScreenDevice(walkDevice,false);
	}
	
	// Set up the GDRect…
	//
	tempRect = (*newDevice)->gdRect;
	OffsetRect(&tempRect,gdOffset.h,gdOffset.v);
	(*newDevice)->gdRect = tempRect;
	
	// Set up the PixMap bounds…
	//
	tempRect = (*(*newDevice)->gdPMap)->bounds;
	OffsetRect(&tempRect,gdOffset.h,gdOffset.v);
	(*(*newDevice)->gdPMap)->bounds = tempRect;
		
	return(newDevice);
}

static void DM_DisposeGDevice(GDHandle disposeDevice)
{
	DisplayManagerGlobalsPtr dmGlobals = GetDMGlobalsPtr();
	
	// If sure would be nice if we could call DisposGDevice, but too many folks
	// cache various things from inside GDevices.  So, we’ve got to recycle.
	//
	(*disposeDevice)->gdNextGD = (Handle)dmGlobals->fRemovedDevices;
	dmGlobals->fRemovedDevices = disposeDevice;
}

pascal OSErr DM_NewDisplay(	GDHandle*			newDevice,
							short				driverRefNum,
							unsigned long		mode,
							unsigned long		reserved,
							DisplayIDType		displayID,					// Pass nil to assign new ID
							ComponentInstance	displayComponent,			// Pass nil to leave as is
							DisplaysStateHdl	displayState)
{
#pragma unused(reserved);
	OSErr				newDIsplayError = noErr;
	THz					savedZone = GetZone();
	GDHandle			newDisplay = nil;
	
	SetZone(SystemZone());
	
	
	newDisplay = DM_NewGDevice(driverRefNum, mode);
	
	if( newDisplay )	newDIsplayError = DMAddDisplay(newDisplay,driverRefNum,mode,reserved,displayID,displayComponent,(Handle )displayState);
	else				newDIsplayError = memFullErr;

	SetZone(savedZone);
	
	*newDevice = newDisplay;
	return( newDIsplayError );
}

pascal OSErr DM_RemoveDisplay(GDHandle removeDevice,DisplaysStateHdl displayState)
{
	GDHandle		followDevice,walkDevice;
	OSErr			removeErr;
	THz				savedZone;
	DisplayIDType	removeDisplayID;
	
		// DMDisableDisplay hammers the main device for us
		// and sets up the dummy device if you disable the last
		// real device.
		//
		// If updateNow is false, should DMDisableDisplay set up
		// the fake device?  No.  So I need to handle the case
		// where we are removing the last device here too.
		
	removeErr = PrepImmediateUpdate(noErr, &displayState, &savedZone);
	
	if( noErr == removeErr )
	{			
		Boolean cursorState = DM_HideCursor();											// Don’t want cursors blitting on other devices during the remove process.
		
		DMGetDisplayIDByGDevice(removeDevice, &removeDisplayID, false);
			
		removeErr = DMDisableDisplay(removeDevice,(Handle )displayState);				// This may switch the main device

		DMGetGDeviceByDisplayID(removeDisplayID, &removeDevice, false);					// Make sure I get the right device back
		
		if( noErr == removeErr )
		{
			followDevice = walkDevice = GetDeviceList();
	
			if( followDevice == removeDevice )			// Is it the first device?
			{
				walkDevice = (GDHandle )(*walkDevice)->gdNextGD;
				if( nil == walkDevice )					// Is it the only device?
				{
		#if DEBUG_DISPLAY_CODE
					short		Support_for_removing_the_last_device_is_not_yet_working;
		#endif				
					DMDebugStr("\p Support for removing the last device is not yet working");
					removeErr = paramErr;			// Fix this to allow the last device;
				}
				else
					*(GDHandle *)DeviceList = walkDevice;
	
			}
			else
			{
				while(walkDevice)
				{
					if(walkDevice == removeDevice)
					{
						(*followDevice)->gdNextGD = (*walkDevice)->gdNextGD;
						break;
					}
					followDevice = walkDevice;
					walkDevice = DMGetNextScreenDevice(walkDevice,false);
				}
			}
		}
		
		DM_ShowCursor(cursorState);
	}
	return( FinishImmediateUpdate(removeErr, displayState, savedZone) );
}

pascal OSErr DM_DisposeDisplay(GDHandle removeDevice,DisplaysStateHdl displayState)
{
	Boolean			isMainDisplay = (removeDevice == GetMainDevice());
	OSErr			disposeError;
	OSErr			idError;
	DisplayIDType	displayID;

	idError = DMGetDisplayIDByGDevice(removeDevice, &displayID, false);			// Get the display ID (if any).
	disposeError = DMRemoveDisplay(removeDevice,(Handle )displayState);			// Remove the display from the device list.

	// If we had a displayID, then we’ll delete the DisplayInfo by ID.  Also,
	// if the removeDevice happened to be the MainDevice, it moved.  So,
	// we need to get back the “real” removeDevice by ID.
	//
	if ( noErr == idError )
	{
		if ( isMainDisplay )
			disposeError = DMGetGDeviceByDisplayID(displayID,&removeDevice,false);
		
		DM_DeleteDisplayInfoByDisplayID(displayID);
	}

	// If everything went okay in the removal process, we’ll go ahead and
	// dipose the GDevice.
	//
	if ( (noErr == disposeError) && (nil != removeDevice) )
		DM_DisposeGDevice(removeDevice);
	
	return( disposeError );
}

pascal OSErr DM_DisableDisplay(GDHandle removeDevice,DisplaysStateHdl displayState)
{
	DMDisplayBlocksRec		displayRegions;
	GDHandle				theMainDevice,newMainDevice;
	OSErr					disableErr;
	THz						savedZone;

	// Make sure remove device is a screen and that it is installed
	// Should I check that the devices are the right type?

	disableErr = PrepImmediateUpdate(noErr, &displayState, &savedZone);

	if( noErr == disableErr )
	{
		if( nil != DMGetNextScreenDevice(newMainDevice = DMGetFirstScreenDevice(true),true) )		// Make sure there are multiple devices
		{	/*
			// At the moment, mirroring can be enabled if and only if there are two gDevices around.  When
			// mirroring is enabled, any disabled device is automatically enabled.  So, if mirroring
			// has been enabled and we’re now trying to disable one of the devices, we’ll just automatically
			// disable mirroring as well.  NOTE:  This will probably change in the future, especially when
			// we start mirroring among more than two gDevices.
			*/
			DisplayManagerGlobalsPtr dmGlobals = GetDMGlobalsPtr();
			
			/* Just ignore the error from DMUnMirrorDevice() for now.
			*/
			if (dmGlobals->fDevicesMirrored)
				DMUnmirrorDevice(removeDevice, (Handle)displayState);
			
			if( GetDeviceList() == removeDevice )								// If we disable the first device
			{
				GDHandle		walkDevice = (GDHandle )(*removeDevice)->gdNextGD;
				short			allDevicesInited = TestDeviceAttribute(removeDevice,allInit);	// Is device list is marked allInit?
	
				while( walkDevice )
				{
					SetDeviceAttribute(walkDevice, allInit,allDevicesInited);		// propogate allInit to the all devices (in case we remove the second device)
			
					if( allDevicesInited )											// If device list is marked allInit
						SetDeviceAttribute(walkDevice, ramInit,true);				// Mark next device as ramInit
				
					walkDevice = (GDHandle )(*walkDevice)->gdNextGD;
				}
			}
		
			theMainDevice = GetMainDevice();
			if( removeDevice == theMainDevice )
			{
				// newMainDevice = DMGetFirstScreenDevice(true); was done when we determined that there were multiple devices
				while( newMainDevice == theMainDevice )
					newMainDevice = DMGetNextScreenDevice(newMainDevice,true);
				
				if( nil == newMainDevice )
				{
					// DANGER:  support removing the last device
					STOP_HERE();
				}
			
				disableErr = DMSetMainDisplay(newMainDevice,(Handle )displayState);
				
				if( noErr == disableErr )
				{
					removeDevice = newMainDevice;			// Remember, we now swap devices to set the main display
				}
			}
				
			if( noErr == disableErr )
			{
				InitDeviceBlock(&displayRegions);
				CalculateDeviceRegions(&displayRegions,removeDevice,false,nil);
		
				SetDeviceAttribute(removeDevice, screenActive, false);				// Remove the display from the list
		
				GDeviceChanged(removeDevice);
				InitGDevice((*removeDevice)->gdRefNum,-1,removeDevice);
				
				
				if( 1 < displayRegions.blockCount )			// If the monitors are not contiguous
				{
					MakeDevicesContiguous(&displayRegions);
					ResolveDisplayBlocks(&displayRegions);
				}
				DMMoveCursor(nil,nil);						// Just be sure things are cool with the cursor
	
				KillAllDisplayBlocks(&displayRegions);
			}
			
			if( noErr == disableErr)
				InitGDevice((*removeDevice)->gdRefNum,-1,removeDevice);			// Just notify the world
		}
	}
		
//		DMDebugger();
//	Make sure the palette manager releases all entries for this device
//		CMP.L	#-1,PMgrHandle				; has the Palette Mgr been initialized?
//		BEQ.S	@noPMgr						; nope, so skip this
//		MOVE.L	GDH(A6),-(SP)				; push the device handle
//		_RestoreDeviceClut					; call the Palette Manager Unhook device vector
//	@noPMgr

	return( FinishImmediateUpdate(disableErr, displayState, savedZone) );
}



static OSErr LocalMoveDisplay(GDHandle moveDevice,short x,short y, DisplaysStateHdl displayState)
{
	Rect				newDeviceRect;
	OSErr				moveError;
	THz					savedZone;
	Boolean				dontChangeRects = ((0 == x) && (0 == y));
	
	moveError = PrepImmediateUpdate(noErr, &displayState, &savedZone);
	
	if( noErr == moveError )
	{
		newDeviceRect = (*moveDevice)->gdRect;
		
		/* If we’re trying to turn mirroring on or off, then don’t apply the offsets
		// because we’ve already pre-calculated all the rectangles (thank you very much).
		//
		// See note in DM_MoveDisplay() regarding (x=0,y=0).
		*/
		if (!dontChangeRects)
		{
			OffsetRect(&newDeviceRect,x - newDeviceRect.left,y - newDeviceRect.top);
		
			(*moveDevice)->gdRect = newDeviceRect;
			(*(*moveDevice)->gdPMap)->bounds = newDeviceRect;
		}
		
		SetDeviceAttribute(moveDevice, ramInit, true);				// Mark Display as ram inited
		
		GDeviceChanged(moveDevice);
		InitGDevice((*moveDevice)->gdRefNum,-1,moveDevice);
	}
	
	return( FinishImmediateUpdate(moveError, displayState, savedZone) );
}

pascal OSErr DM_MoveDisplay(GDHandle moveDevice,short x,short y,DisplaysStateHdl displayState)
{
	OSErr				moveError;
	GDHandle			theMainDevice = GetMainDevice();
	GDHandle			walkDevice;
	THz					savedZone;
	
	// If we are asked to move the main device, instead we move all the other
	// devices the other way to keep the origin at zero.
	//
	// Exception:  We are reserving moves of (x=0,y=0) to imply that mirroring
	//			   is either being turned on or off, and, in this case,
	//			   “moving” the mainScreen device is okay.  This holds
	//			   for the routine LocalMoveDisplay(), too.
	//
	
	moveError = PrepImmediateUpdate(noErr, &displayState, &savedZone);

	if ( noErr == moveError )
	{
		Boolean		mainMoveOkay	= ((0 == x) && (0 == y)),
					cursorState		= DM_HideCursor();
		
		if( !mainMoveOkay && (moveDevice == theMainDevice) )
		{
			Rect		walkRect;
	
			walkDevice = DMGetFirstScreenDevice(true);
			while( walkDevice && (noErr == moveError) )
			{
				if( walkDevice != theMainDevice )
				{
					walkRect = (*walkDevice)->gdRect;
					moveError = LocalMoveDisplay(walkDevice,walkRect.left + x,walkRect.top + y,displayState);
				}
				
				walkDevice = DMGetNextScreenDevice(walkDevice,true);
			}
		}
		else
				moveError = LocalMoveDisplay(moveDevice,x,y,displayState);
	
		DMMoveCursor(nil,nil);				// Just be sure things are cool with the cursor
		DM_ShowCursor(cursorState);
	}

	return( FinishImmediateUpdate(moveError, displayState, savedZone) );
}


pascal OSErr DM_EnableDisplay(GDHandle addDevice,DisplaysStateHdl displayState)
{
	OSErr					enableError;
	DMDisplayBlocksRec		displayRegions;
	THz						savedZone;
	Boolean					cursorState;

	enableError = PrepImmediateUpdate(noErr, &displayState, &savedZone);
	if( noErr == enableError && addDevice )
	{		
		InitDeviceBlock(&displayRegions);
		enableError = CalculateDeviceRegions(&displayRegions,addDevice,true,nil);
		
		cursorState = DM_HideCursor();
		
		SetDeviceAttribute(addDevice, screenActive, true);			// Mark Display as active
		SetDeviceAttribute(addDevice, ramInit, true);				// Mark Display as ram inited
		
		MakeDevicesContiguous(&displayRegions);
		ResolveDisplayBlocks(&displayRegions);
	
		// Resolution independent QD test.
		// DANGER.... ask res ind QD guys about a call set the pixmap correctly
		
		(*(*addDevice)->gdPMap)->bounds = (*addDevice)->gdRect;

		GDeviceChanged(addDevice);
		InitGDevice((*addDevice)->gdRefNum,-1,addDevice);

		AllocCursor();								// Need to make sure the rest of the low mems are up to date
		DM_ShowCursor(cursorState);
		KillAllDisplayBlocks(&displayRegions);
	
	}

	return( FinishImmediateUpdate(enableError, displayState, savedZone) );
}


pascal OSErr DM_GetComponentAnimateTicks(ComponentInstance animationComponent,unsigned long *goodDelay,unsigned long *maxDelay)
{
#pragma unused(animationComponent,goodDelay,maxDelay)
// NEEDSFIX(10,"not complete");
// NEEDSFIX(9,"must have a queue of active devices and sort this in");

	Debugger();
	return(noErr);
}

pascal OSErr DM_SetComponentAnimateTicks(ComponentInstance animationComponent,unsigned long goodDelay,unsigned long maxDelay)
{
#pragma unused(animationComponent,goodDelay,maxDelay)
// NEEDSFIX(10,"not complete");
// NEEDSFIX(9,"must have a queue of active devices and sort this in");

	GetDMGlobalsPtr()->fNextAnimateTime = 0;
	
	return(noErr);
}

pascal OSErr DM_GetNextAnimateTime(unsigned long *nextAnimateTime)
{	
	*nextAnimateTime = GetDMGlobalsPtr()->fNextAnimateTime;

	return(noErr);
}


pascal OSErr DM_SetAnimateProcess(const ProcessSerialNumberPtr animateProcess)
{
	
	GetDMGlobalsPtr()->fAnimatePSN = *animateProcess;
#ifdef APPLICATION_PATCH_BUILD
	// A hack so I can have my A5 when I build as part of an application
		GetDMGlobalsPtr()->fAnimateA5 = *(unsigned long *)CurrentA5;
#endif

	return(noErr);
}


pascal OSErr DM_GetDeskRegion(RgnHandle* desktopRegion)
// Window Mgr must be initialized
// External call do not even try to get the internal rgn (which returns grayrgn
//	when the window mgr is 1/2 set up)
{
	OSErr			getDeskError = noErr;
	
	*desktopRegion = nil;
	
	if( WindowMgrInitialized() )
		*desktopRegion = InternalGetDesktopRgn();
	
	if( nil == *desktopRegion )
		getDeskError = kDMSWNotInitializedErr;

	return(getDeskError);
}



pascal OSErr DM_SetGDeviceTablecloth(GDHandle displayDevice,ComponentInstance tableclothInstance)
{
	OSErr				theError = noErr;
	ComponentInstance	oldInstance;
	RgnHandle			deviceRgn;
	RgnHandle			desktopRgn;
	Rect				deviceRect;
		
	if( oldInstance = (ComponentInstance )(*displayDevice)->gdRefCon )
		CloseComponent(oldInstance);

	(*displayDevice)->gdRefCon = (long )tableclothInstance;
	
	if(oldInstance)			// I install as copies of existing pattern
	{
		deviceRgn = NewRgn();
		
		deviceRect = (*displayDevice)->gdRect;
		RectRgn(deviceRgn,&deviceRect);
		DMGetDeskRegion(&desktopRgn);
		
		SectRgn(deviceRgn,desktopRgn,deviceRgn);
		PaintBehind(nil,deviceRgn);
		DisposeRgn(deviceRgn);
	}
	
	return(theError);
}



pascal OSErr DM_GetGDeviceTablecloth(GDHandle displayDevice,ComponentInstance *tableclothInstance)
{
	OSErr			theError = noErr;
	
	*tableclothInstance = (ComponentInstance )(*displayDevice)->gdRefCon;
	if( nil == *tableclothInstance )
		theError = paramErr;
		
	return(theError);
}






static pascal void MyDeviceLoopDrawingProcPtr(short depth, short deviceFlags, GDHandle targetDevice,long userData)
{
#pragma unused(depth,deviceFlags);
	OSErr				drawError;
	ComponentInstance	tableclothInstance;
	MyDeviceLoopPtr		deviceLoopParams = (MyDeviceLoopPtr )userData;
	DrawParamBlockPtr	drawingParams = deviceLoopParams->drawingParams;

	drawError = DMGetGDeviceTablecloth(targetDevice,&tableclothInstance);
	
	if( noErr == drawError )
	{
		Rect				deviceRect;

		SetGDevice(targetDevice);
		deviceRect = (*targetDevice)->gdRect;

		RectRgn(drawingParams->fullRgn,&deviceRect);
		SectRgn(drawingParams->fullRgn,deviceLoopParams->updateRgn,drawingParams->drawRgn);
		drawError = RenderTablecloth(tableclothInstance,drawingParams);
	}
	
	if( noErr != drawError )
		DrawDeskPattern(deviceLoopParams->updateRgn);
//		FillRgn(drawingParams->drawRgn,(Pattern *)DeskPattern);
}


pascal OSErr DM_SetDisplayMode(
				GDHandle theDevice,
				unsigned long modeID,				// Functional sResource of new mode
				unsigned long *desiredDepthMode,			// VAR param which takes a suggested mode/depth and returns the chosen mode
				VDSwitchInfoPtr switchModeInfo,
				DisplaysStateHdl displayState)
{
	OSErr					setModeErr = paramErr;
	Rect					oldDeviceRect;
	Rect					newDeviceRect;
	PixMapHandle			thePixMap;
	PixMapHandle			oldPixMap;
	Ptr						oldBaseAddress;
	CTabHandle				oldCTable;
	Boolean					hasMainScreenBit;
	Boolean					hasRamInitBit;
	VDSwitchInfoRec			savedModeInfo;
	VDSwitchInfoRec			newModeInfo;
//	DMDisplayBlocksRec		displayRegions;
	short					bestDepth;
	THz						savedZone;
	unsigned long			depthMode = 0;
	Boolean					hasGetCurMode = false;
	
	// Check for interprocess call before proceeding
	// 
	
	if( IsProcessMgrOkay() )
		setModeErr = GetGDeviceVideoMode(theDevice,&savedModeInfo,&hasGetCurMode);
	
	if( !hasGetCurMode )
		setModeErr = kDMDriverNotDisplayMgrAwareErr;
	
	setModeErr = PrepImmediateUpdate(setModeErr, &displayState, &savedZone);

	
	if( noErr == setModeErr )
	{
		if( desiredDepthMode )
			depthMode = *desiredDepthMode;
		
		oldDeviceRect			= (*theDevice)->gdRect;
		oldPixMap				= (*theDevice)->gdPMap;
		oldCTable				= (*oldPixMap)->pmTable;
		
		if( switchModeInfo )										// Is everything overridden by a real switchModeInfo?
		{
			newModeInfo = *switchModeInfo;
			modeID		= newModeInfo.csData;
			depthMode	= newModeInfo.csMode;
		}
		else
		{
		newModeInfo.csMode		= 0;							// Failure Depth information
		newModeInfo.csData			= modeID;						// Functional sResource of new mode
		newModeInfo.csPage			= 0;							// Page
		newModeInfo.csBaseAddr		= 0;							// base address returned by driver calls
			newModeInfo.csReserved	= 0;
		}
		
		setModeErr = DMGetBestDepthModeInfo(theDevice,&newModeInfo,depthMode,&bestDepth,&newDeviceRect);			// Find a default mode (depth) to switch to
		OffsetRect(&newDeviceRect,oldDeviceRect.left - newDeviceRect.left,oldDeviceRect.top - newDeviceRect.top);
		
		if( noErr == setModeErr )
			setModeErr = DisplayManagerCloseViewCompatibility(theDevice,&newDeviceRect,bestDepth);								// Let closeview remove it's hooks

		oldBaseAddress			= (*oldPixMap)->baseAddr;																		// Base address is now corrected (closeview is off)

		if( noErr == setModeErr )
		{
			setModeErr = SwitchVideoMode(theDevice,&newModeInfo);
			
			thePixMap = (*theDevice)->gdPMap;
			if( noErr == setModeErr )
			{

				(*theDevice)->gdMode = 0;												// Force InitGDevice to Reset
				hasMainScreenBit = TestDeviceAttribute(theDevice, mainScreen);			// Is this the main device?
				hasRamInitBit = TestDeviceAttribute(theDevice, ramInit);				// Is it initialized 
				 
			// Force InitGDevice to do what we want
			//		InitGDevice will ignore ports if it does not think it is dealing with the main device.  Therefore I clear the mainScreen flag
			//		InitGDevice will use the slot manager to get the rect if the ramInit flag is not set (otherwise it overwrites the pixmap rect with the gdRect).
			//			An alternative would be to get the rect from the mVidParms and put it in the gdRect to be copied by InitGDevice
				SetDeviceAttribute(theDevice, mainScreen, false);
				SetDeviceAttribute(theDevice, ramInit, false);

					InitGDevice((*theDevice)->gdRefNum,newModeInfo.csMode,theDevice);
				
				SetDeviceAttribute(theDevice, mainScreen, hasMainScreenBit);			// Restore the maindevice bit
				SetDeviceAttribute(theDevice, ramInit, hasRamInitBit);					// Restore the ramInit bit
				
				if( oldDeviceRect.left != (*theDevice)->gdRect.left || oldDeviceRect.top != (*theDevice)->gdRect.top )
					setModeErr = DMMoveDisplay(theDevice,oldDeviceRect.left,oldDeviceRect.top,false);

				if( desiredDepthMode && noErr == setModeErr )
					*desiredDepthMode = newModeInfo.csMode;								// Return the depth mode

				if( hasMainScreenBit )													// Do I need to fix the ports?
				{
					newDeviceRect = (*theDevice)->gdRect;
					
					FixLowMem(theDevice,theDevice,0,0);
					FixPorts(theDevice,true,&newDeviceRect,thePixMap,&oldDeviceRect,oldBaseAddress,oldCTable);
					FixWindowMgrPorts(theDevice);
					
					CallAllProcesses( (ProcPtr )InterProcessChangedModes,(Ptr )theDevice);
				}
			}
			else
				SwitchVideoMode(theDevice,&savedModeInfo);			// Try to restore video ( The damage is done, I do not want the error code)
		}
	}
	
	return( FinishImmediateUpdate(setModeErr, displayState, savedZone) );
}

static pascal OSErr DisposeDisplayConfigurationState(DisplaysStateHdl displayState)
{
	if( displayState )
	{
		if( (*displayState)->oldGrayRgn )
			DisposeRgn((*displayState)->oldGrayRgn);
	
		if( (*displayState)->displayConfigs )
			DisposeHandle( (Handle )(*displayState)->displayConfigs );
	
		DisposeHandle( (Handle )displayState );
	}
	
	return(noErr);
}

pascal OSErr DM_PrivateBeginConfigureDisplays(DisplaysStateHdl *displayState,Boolean internalCall)
//
// Internal calls are made with a displayState == nil or displayState == result of BeginConfigureDisplays
// External calls are always non-nested
//
{
	DisplaysStateHdl	localDisplayState;
	OSErr				beginConfigErr;
	
	
	localDisplayState = (internalCall ? *displayState : nil);
	
	if( nil == localDisplayState )	
	{		
		DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();
		
		if( dmGlobals && dmGlobals->fNotificationTable )				// Are there any procedures to notify?
		{
			short						notificationWalk = dmGlobals->fNotificationCount;
			DMProcPtrRegRec				tempProcReg;

			while(notificationWalk--)					// Post Decrement i to get index into zero-based proc array
			{
				tempProcReg = (*dmGlobals->fNotificationTable)[notificationWalk];
				CallNotificationProc(&tempProcReg,kDMNotifyPrep,nil);				// Let anyone who cares, know we are about to start rearranging stuff.
			}
		}

	
		localDisplayState = (DisplaysStateHdl )NewHandleClear( sizeof(DisplaysStateRec) );
		beginConfigErr = MemError();
	
		if( noErr == beginConfigErr )
		{
			short				displayCount;
			DisplayConfigHdl 	displayConfigs;
			
			beginConfigErr = BuildDisplayConfigHandle(&displayConfigs,&displayCount);

			(*localDisplayState)->displayCount = displayCount;
			(*localDisplayState)->displayConfigs = displayConfigs;
		}
	
		if( noErr == beginConfigErr )
		{
			RgnHandle		theGrayRgn = SafeGetGrayRgn();
			RgnHandle		savedGrayRgn = NewRgn();
			
			beginConfigErr = MemError();
			(*localDisplayState)->oldGrayRgn = savedGrayRgn;
			
			if( noErr == beginConfigErr )
			{
				if( theGrayRgn )
					CopyRgn(theGrayRgn,savedGrayRgn);
				else
					CopyRgn(GetDMGlobalsPtr()->fDesktopCacheRgn,savedGrayRgn);
					
				beginConfigErr = MemError();
			}
		}
		
		if( noErr != beginConfigErr )
		{
			DisposeDisplayConfigurationState(localDisplayState);
			localDisplayState = nil;
		}
	}
	
	if( localDisplayState )
		(*localDisplayState)->nesting++;	// Increment nesting for both internal and external calls

	
	*displayState = localDisplayState;
	return( beginConfigErr );
}

pascal OSErr DM_BeginConfigureDisplays(DisplaysStateHdl *displayState)
{
	return(DMPrivateBeginConfigureDisplays(displayState,false));
}


pascal OSErr DM_EndConfigureDisplays(DisplaysStateHdl displayState)
{
	OSErr				endConfigError = paramErr;
	
	if( displayState )
	{
		endConfigError = noErr;
		if( 0 == --(*displayState)->nesting )			// Decrement and test nexting.  If we are done nesting, we can update
		{		
			endConfigError = CheckAndUpdateDisplaysLayout(displayState);
			
			DisposeDisplayConfigurationState(displayState);
		}
	}
	return( endConfigError );
}



pascal OSErr DM_CheckDisplayMode(	GDHandle		theDevice,
									unsigned long	displayMode,
									unsigned long	depthMode,
									unsigned long*	switchFlags,
									unsigned long	reserved,
									Boolean*		modeValid)
{
	OSErr					checkModeErr = paramErr;
	OSErr					componentError = noErr;
	VDDisplayConnectInfoRec	connectInfo;
	Boolean					isSwitchSafe = false;
	Boolean					doSearchDeclRom = true;
	Boolean					haveTimingInfo = false;
	Boolean					haveConnectInfo = false;
	
//	short					timingFamily;
	VDTimingInfoRec			modeTiming;
	MultiModeTablePtr		multiModeROMTable,multiModeWalker;
	MultiModeNodePtr		multiModeROMEntry;
	ComponentInstance		displayComponent = nil;
	VDSwitchInfoRec			videoMode;
	Boolean					hasGetCurMode = false;
	short					bestDepth = 0;
	
	*modeValid = true;			// Assume that the mode was not trimmed in primary init because it might be valid
	*switchFlags = 0;									// Clear the switch flags
	checkModeErr = noErr;
	
	// Initialize timing record
	modeTiming.csTimingMode = displayMode;
	modeTiming.csTimingReserved = reserved;				// This will someday be the extended timing spec
	modeTiming.csTimingFormat =							// LONGINT - (long) what format is the timing info
	modeTiming.csTimingData =       					// LONGINT - (long) data supplied by driver
	modeTiming.csTimingFlags = 0;   					// LONGINT - (long) mode within device
	
	// Initialize connection record
	connectInfo.csDisplayType		= kUnknownConnect;
	connectInfo.csConnectTagged		= 0;
	connectInfo.csConnectFlags		= 0;
	connectInfo.csDisplayComponent	= nil;
	connectInfo.csConnectReserved	= 0;
	
	// Check the depth request
	
	if( 0 == depthMode )
		depthMode = (*(*theDevice)->gdPMap)->pixelSize;			

	if( 0 == displayMode )						// Are we talking about the current mode?
	{
		checkModeErr = GetGDeviceVideoMode(theDevice,&videoMode,&hasGetCurMode);
		displayMode = videoMode.csData;
	}
	
	videoMode.csMode		= 0;							// Failure Depth information
	videoMode.csData		= displayMode;					// Functional sResource of new mode
	videoMode.csPage		= 0;							// Page
	videoMode.csBaseAddr	= 0;							// base address returned by driver calls
	videoMode.csReserved	= 0;
	
	if( noErr == DMGetBestDepthModeInfo(theDevice,&videoMode,depthMode,&bestDepth, nil) )
	{
		// If we matched a depth for a mode, then we have the mode, but if the depthMode is
		//	really a depth then we need to be sure that it matches bestDepth.
		
		if( 128 > depthMode && bestDepth != depthMode )	
			*switchFlags |= (1<<kDepthNotAvailableBit);
	}
	else
		*switchFlags |= (1<<kDepthNotAvailableBit);
	
	// Now check the mode
	//
	// Check if all modes are safe (in which case we are done)
	//
	// This is the simple way for machines like portables with panel displays to get away from confirmation 
	// dialogs on their mode switches; support the GetDisplayConnection info call and set the "kAllModesSafe" bit.
	//
	
	if( noErr == GetDisplayConnection(theDevice,&connectInfo) )
	{
		haveConnectInfo = true;
		isSwitchSafe = (0 != (connectInfo.csConnectFlags&(1<<kAllModesSafe)) );
	}
	
	if( !isSwitchSafe )				// If we do not know if the switch is safe, then try to find out
	{
		#if DEBUG_DISPLAY_CODE
		short			think_about_what_order_the_connection_SLASH_driver_SLASH_rom_table_info_should_be_used_and_overridden;
		#endif
		//
		// Does the driver know the timing mode?
		// To support upgraded drivers (but old decl ROMs) we make a driver call return
		// information about the device mode.  We want 3rd parties (& Apple) to support
		// multisyncs without changing decl ROMs. (on hardware that has already shipped).
		//

		
		if( noErr == GetTimingMode(theDevice,&modeTiming) ||									// Try to get Timing mode from driver
				noErr == GetTimingFromDecl(theDevice,connectInfo.csDisplayType,&modeTiming) )	// Try to get Timing mode from decl ROM
			haveTimingInfo = true;
		
		if( haveTimingInfo )																	// Did we get timing info?
		{
			if( 0 != (modeTiming.csTimingFlags & (1<<kModeSafe)) )								// If the timing info says we are safe, believe it
				isSwitchSafe = true;
		
			if( noErr != DMGetDisplayComponent(theDevice,&displayComponent) || (nil == displayComponent) )		// Is there a display component?
			{
				if( haveConnectInfo )															// Did we get connection info?
					displayComponent = (ComponentInstance )connectInfo.csDisplayComponent;							// If so, does the driver have a display component?
			}
		}
		
		//
		// If we have timing info, then we check the multimode table
		// We need to have the connection info AND the timing info
		//
		if( haveTimingInfo && haveConnectInfo )
		{
			if( noErr == DMGetROMMultiModeTable(&multiModeROMTable) )
			{
				multiModeWalker = multiModeROMTable;
				while( 0 != multiModeWalker->displayConnection )
				{
					if( multiModeWalker->displayConnection == connectInfo.csDisplayType )
					{
						multiModeROMEntry = (MultiModeNodePtr )(((char *)multiModeROMTable) + multiModeWalker->offset);
						while( 0 != multiModeROMEntry->timingMode )
						{
							if(multiModeROMEntry->timingMode == modeTiming.csTimingData )
							{
								if( 0 != (multiModeROMEntry->timingFlags & (1<<kModeSafe)) )	// If the timing info says we are safe, believe it
									isSwitchSafe = true;
								break;															// We are done searching
							}
							multiModeROMEntry++;				// Go on to the next entry
						}
						break;
					}
					multiModeWalker++;				// Go on to the next entry
				}
				
			}
		}
		
		if( nil != displayComponent )															// Did we get a display component?
		{
			// The display component (if one is present) gets the final say on whether a mode is valid
			// FUTURE:  we might want to negociate between the driver an component to see whether we can
			//			find a compromise mode.
			
			if( noErr == checkModeErr )
				componentError = DisplayComponentCheckDisplayMode(displayComponent, theDevice, &connectInfo, &modeTiming, bestDepth, switchFlags, reserved, modeValid);
		}
	}
	
	if( !*modeValid )
	{
		*switchFlags &= ((-1) ^ ((long )1<<kNoSwitchConfirmBit));		// Switch is NOT safe (and damn if MPW C doesn't produce a Bclr instruction from this!!)
	}
	else if( isSwitchSafe )
	{
		*switchFlags |= (1<<kNoSwitchConfirmBit);						// Switch is safe
		*modeValid = true;
	}
	// If not, then try monitor components -- try for a match
	

	return(checkModeErr);
}



pascal OSErr DM_GiveDeskTime(void)
// Gives animations time.
// Private call used by the Animation process.
{
	ComponentInstance					tableclothInstance;
	CGrafPtr							savedPort;
	GDHandle							savedGDHandle;
	Rect								deviceRect;
	DrawParamBlockRec					drawParams;
	RgnHandle							deskRgn;
	RgnHandle							iconRgn;	
	Boolean								deskRgnCacheInvalid;
	RgnHandle							drawingRgn;
	RgnHandle							savedClip;
	OSErr								giveTimeErr = noErr;
	
//	CLAIM(dmGlobals);

	iconRgn = (RgnHandle )GetExpandMemFndrDeskRgn();

	if( iconRgn )
	{
		GDHandle							walkDevice = DMGetFirstScreenDevice(true);
		register DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();
		THz									savedZone = SetSystemZone();

		GetGWorld(&savedPort,&savedGDHandle);
		SetPort((GrafPtr )dmGlobals->fDrawingPort);
		drawingRgn = NewRgn();
		savedClip = NewRgn();
		
		if( drawingRgn && savedClip )
		{
			drawParams.fullRgn			= dmGlobals->fWorkingDeskFullRgn;
			drawParams.isThumbNail		= false;
			drawParams.isAnimate		= true;
			drawParams.drawFlags		= 0;
			drawParams.drawRgn			= drawingRgn;
			drawParams.drawPort			= dmGlobals->fDrawingPort;
			
			GetClip(savedClip);
			DMGetDeskRegion(&deskRgn);
			
			deskRgnCacheInvalid		= !EqualRgn(deskRgn,dmGlobals->fDesktopCacheRgn);
			
			if( dmGlobals->fIconRgnInvalid || deskRgnCacheInvalid )
			{
				if( deskRgnCacheInvalid )
					CopyRgn(deskRgn,dmGlobals->fDesktopCacheRgn);				// Copy the desk rgn
	
				DiffRgn(deskRgn,iconRgn,dmGlobals->fAnimationCacheRgn);
		
				dmGlobals->fIconRgnInvalid = false;								// Mark the rgn valid
			}
				
					
			while(walkDevice)
			{
				if(noErr == DMGetGDeviceTablecloth(walkDevice,&tableclothInstance) )
				{
					SetGDevice(walkDevice);
					deviceRect = (*walkDevice)->gdRect;
					RectRgn(drawParams.fullRgn,&deviceRect);
					SectRgn(drawParams.fullRgn,dmGlobals->fAnimationCacheRgn,drawParams.drawRgn);
					SetClip(drawParams.drawRgn);
					
					CopyRgn(drawParams.drawRgn,dmGlobals->fDrawingPort->visRgn);
					PortChanged((GrafPtr )dmGlobals->fDrawingPort);
				
		//			Debugger();			
					RenderTablecloth(tableclothInstance,&drawParams);
					
				}
				walkDevice = DMGetNextScreenDevice(walkDevice,true);
			}
			
			CopyRgn(GetGrayRgn(),dmGlobals->fDrawingPort->visRgn);
			PortChanged((GrafPtr )dmGlobals->fDrawingPort);
			
			SetClip(savedClip);
		
			SetGWorld(savedPort,savedGDHandle);
		}

		if( drawingRgn ) DisposeRgn(drawingRgn);
		if( savedClip ) DisposeRgn(savedClip);
		
		SetZone(savedZone);
	}
	return(giveTimeErr);
}


pascal OSErr DM_RemoveCurrentProcessProcs(void)
{
	OSErr						removeError = paramErr;
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();
	
	if( dmGlobals && IsProcessMgrInstalled() )
	{
		ProcessSerialNumber		currentprocess;
		
		if( noErr == GetCurrentProcess(&currentprocess) )
		{
			DMProcPtrRegHdl			notificationTable = dmGlobals->fNotificationTable;
			short					notificationIndex = 0;
			DMProcPtrRegRec			notificationEntry;
			Boolean					equalProcess;
						
			if( notificationTable && dmGlobals->fNotificationCount )
			{
				notificationEntry = (*notificationTable)[notificationIndex];
				
				while( notificationIndex < dmGlobals->fNotificationCount )
				{
					notificationEntry = (*notificationTable)[notificationIndex];
					if( noErr == SameProcess(&notificationEntry.registrationPSN, &currentprocess, &equalProcess) && equalProcess )
					{
						Munger((Handle )notificationTable,notificationIndex*sizeof(DMProcPtrRegRec),nil,sizeof(DMProcPtrRegRec),(Ptr )-1,0);
						dmGlobals->fNotificationCount--;
					}
					else
						notificationIndex++;
				}
			}
		}
	}
	
	return(removeError);
}


static OSErr InternalRegisterNotifyProc(DMNotificationProcPtr notifyProc,void* notifyData,unsigned long notifyFlags,ProcessSerialNumberPtr registerProcess)
{
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();
	OSErr						regError = noErr;
	THz							savedZone = SetSystemZone();
	
	if( notifyProc && dmGlobals )
	{
		ProcessSerialNumber			notifyProcess;

		if( registerProcess )
			notifyProcess = *registerProcess;
		else
		{
			notifyProcess.highLongOfPSN		= 0;
			notifyProcess.lowLongOfPSN		= kNoProcess;
		}

		if( !IsKNoProcess(&notifyProcess) )
		{
			ProcessInfoRec			processInfo;
	
			processInfo.processInfoLength	= sizeof(ProcessInfoRec);
			processInfo.processName			= nil;
			processInfo.processAppSpec		= nil;	

			if( IsProcessMgrInstalled() )
					regError = GetProcessInformation(registerProcess,&processInfo);			// Make sure any constant are converted
			else	regError = paramErr;					// You cannot register for a process unless the process manager is installed
			
			if( noErr == regError )
				notifyProcess = processInfo.processNumber;
		}
		
		if( noErr == regError )	
		{
			if( dmGlobals->fNotificationTable )
			{
				SetHandleSize((Handle )dmGlobals->fNotificationTable,(dmGlobals->fNotificationCount+1)*sizeof(DMProcPtrRegRec));
				regError = MemError();
				
				if( noErr != regError )				// Try to get around heap fragmentation problems
				{
					DMProcPtrRegHdl				tempHandle = (DMProcPtrRegHdl )NewHandleSys((dmGlobals->fNotificationCount+1)*sizeof(DMProcPtrRegRec));
					
					DMDebugStr("\p Unable to add more notification procs.  Trying to allocate new block");
					if( noErr == MemError() )
					{
						DMDebugStr("\p Allocated new notification proc block");

						BlockMove(		(Ptr )*dmGlobals->fNotificationTable,						// Copy the old handle
										(Ptr )*tempHandle,											// to the new
										dmGlobals->fNotificationCount*sizeof(DMProcPtrRegRec) );
										
						DisposeHandle((Handle )dmGlobals->fNotificationTable);								// Delete the old handle
						dmGlobals->fNotificationTable = tempHandle;									// Save the new handle
					}
				}
			}
			else
			{
				dmGlobals->fNotificationTable = (DMProcPtrRegHdl )NewHandleSys(sizeof(DMProcPtrRegRec));
				regError = MemError();
			}
			
			if( noErr == regError )
			{
				DMProcPtrRegRec			notificationEntry;
				
				notificationEntry.registrationPSN  = notifyProcess;
				notificationEntry.registrationProc = notifyProc;
				notificationEntry.registrationFlags		= notifyFlags;
				notificationEntry.registrationUserData	= notifyData;
				
				(*dmGlobals->fNotificationTable)[dmGlobals->fNotificationCount++] = notificationEntry;
				
				CallNotificationProc(&notificationEntry,kDMNotifyInstalled,nil);
		
			}
		}
	}
	else
		regError = paramErr;
	
	SetZone(savedZone);
	return(regError);
}


pascal OSErr DM_RegisterNotifyProc(DMNotificationProcPtr notifyProc,ProcessSerialNumberPtr registerProcess)
{
	return( InternalRegisterNotifyProc(notifyProc,0,0,registerProcess) );
}

pascal OSErr DM_RegisterExtendedNotifyProc(DMExtendedNotificationProcPtr notifyProc,void* notifyData,unsigned short notifyFlags,ProcessSerialNumberPtr registerProcess)
// DMExtendedNotification
//
// NOTE:
//	C++ programs with can register a pascal member functions, set userData an object get their this pointer automagically set up.
//		(same trick as works with Thread Manager).  Note this explanation is not ment as encouragement.
//
//	class ClassName {
//		pascal void foo(short message, AppleEvent* theEvent);
//	}
//	ClassName*		myObject;
//
//	DMExtendedRegister( (DMExtendedNotificationPtr )myObject::foo, myObject);
//
//	Foo gets called and myObject is correctly set to the "this" parameter and you can automagically
//		use you object members etc.
//
{
	unsigned long	registerFlags = (((unsigned long )notifyFlags) | kExtendedNotificationProc ); 
	
	return( InternalRegisterNotifyProc( (DMNotificationProcPtr )notifyProc,notifyData,registerFlags,registerProcess) );
}



pascal OSErr	DM_RemoveNotifyProc(DMNotificationProcPtr notifyProc,ProcessSerialNumberPtr registerProcess)
{
	OSErr						registerError = noErr;
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();
	ProcessSerialNumber			notifyProcess;
	THz							savedZone			= SetSystemZone();
		
	if( dmGlobals && notifyProc )			// Must have globals and must specify a ProcPtr (nil == registerProcess -> kNoProcess)
	{
		if( dmGlobals->fNotificationTable )
		{
			DMProcPtrRegHdl			notificationTable	= dmGlobals->fNotificationTable;
			short					notificationIndex	= 0;
			DMProcPtrRegRec			notificationEntry;
			Boolean					foundEntry			= false;
			Boolean					equalProcess;
			Boolean					hasProcessMgr		= IsProcessMgrInstalled();
			
			if( registerProcess )
				notifyProcess = *registerProcess;
			else
			{
				notifyProcess.highLongOfPSN		= 0;
				notifyProcess.lowLongOfPSN		= kNoProcess;
			}

			while( notificationIndex < dmGlobals->fNotificationCount )
			{
				Boolean		bothKNoProcess;
				notificationEntry = (*notificationTable)[notificationIndex];
				
				// Passing nil for the notify proc means kill first from that process

				bothKNoProcess = ( IsKNoProcess(&notifyProcess) && IsKNoProcess(&notificationEntry.registrationPSN) );

				
				// If the ProcPtrs are the same and either both are kNoProcess
				// or they are the same process then we will remove the entry.

				if(		notifyProc == notificationEntry.registrationProc &&
						(	bothKNoProcess ||
								(	hasProcessMgr &&						// Make the work extra safe for extensions
									noErr == SameProcess(&notificationEntry.registrationPSN, &notifyProcess, &equalProcess) &&
									equalProcess)	) )
				{
					CallNotificationProc(&notificationEntry,kDMNotifyRemoved,nil);			// Let them know they are going away

					if( --dmGlobals->fNotificationCount )				// If there are still more entries
					{
						Munger((Handle )notificationTable,notificationIndex*sizeof(DMProcPtrRegRec),nil,sizeof(DMProcPtrRegRec),(Ptr )-1,0);
						registerError = MemError();
					}
					else												// Otherwise, we are removing the last entry, so just kill handle
					{
						dmGlobals->fNotificationTable = nil;
						DisposeHandle( (Handle )notificationTable );
					}

					foundEntry = true;
					break;
				}
				else
					notificationIndex++;
			}
			
			if( noErr == registerError && !foundEntry )			// If we have not found an error yet, make sure we found an entry
				registerError = kDMDisplayNotFoundErr;
		}
		else
			registerError = kDMDisplayNotFoundErr;
	}
	else
		registerError = paramErr;
	
	SetZone(savedZone);
	return(registerError);
}


static Boolean PointOnScreen(Point thePoint,GDHandle* screenDevice)
{
	Boolean		isOnScreen = false;
	GDHandle	walkDevice = DM_GetFirstScreenDevice(true);
	
	
	while( walkDevice )
	{
		if( PtInRect(thePoint,&(*walkDevice)->gdRect ) )
		{
			*screenDevice = walkDevice;
			isOnScreen = true;
			break;
		}
	
		walkDevice = DMGetNextScreenDevice(walkDevice,true);
	}
		
	return( isOnScreen );
}


Boolean DM_HideCursor()
{
	Boolean		hideNeeded = (0 == *(short *)CrsrState);
	
	if( hideNeeded )
		HideCursor();
		
	return(hideNeeded);
}


void DM_ShowCursor(Boolean cursorHidden)
{
	if( cursorHidden )
		ShowCursor();

}

static pascal void TempCrsrTask(void)
{

}

pascal OSErr DM_MoveCursor(const Point* cursorWhere,const long* theCursorData)
//	This is an evil call from hell.  NEVER move the cursor.
//		but, under the display manager, the devices may move and the cursor should not
//		appear to move and if a device is DISABLED (especially prior to disposing the
//		device) the cursor must be disassociated from that device.
//
//	I spoke with Shannon Holland (QD engineer) about making this a QD trap and he agrees.
//		He will let me know when it is written and how to test for its existance.
//		
//		theCursorData is for supporting multiple cursors.
//		The code in CrsrDev.a seems to imply future expansion to support multiple cursors
//		as well as multiple cursor devices.  I'll try to support that too as long as I am adding this call.
// 
//	DMMoveCursor should validate the positions of ALL cursors for all devices.
//
//	cursorWhere is the point to move the cursor
//	theCursorData should be nil for now
//
// I assume that emCursorGlobals is present in ExpandMemRec (although it may be nil) because
//	emDisplayManagerGlobals (which requires a later version of ExpandMem) are installed
//	during my initialization.
//
// Open Issue:  What happens when the cursor is not coupled to the mouse?
{
	ProcPtr				cursorTask = *(ProcPtr *)JCrsrTask;
	CrsrDevGlobalsPtr	cursorGlobals = (CrsrDevGlobalsPtr )GetExpandMemCursorGlobals();
	CrsrDataPtr			cursorWalkData;
	short				debugCount = 0;
	Point				requestedMouse;
	Boolean				cursorState = DM_HideCursor();
	Boolean				cursorOnScreen;
	GDHandle			theCursorDevice;

	//	Make SURE the cursor is hidden and JCrsrTask is called before showing the cursor.  Otherwise we can
	//	crash while blitting the cursor because the blit code depends on relative values of CrsrPin and Mouse).
	// By stuffing JCrsrTask with a dummy proc, I can stop the real crsr task without killing inturrupts.

	*(ProcPtr *)JCrsrTask = (ProcPtr )TempCrsrTask;							// Cursor code is not reentrant, so put a cork in it
	*(char *)CrsrBusy = 1;						// Pause the cursor
		(**(GDHandle *)CrsrDevice)->gdCCDepth = 0;	// Stomp the cursor depth

		// The low memory CrsrPin rectangle caches the current gdevice rect.
		// By clearing it and setting the CrsrNew we can force QD to decide that 
		// the cursor is not in the same device it was and find the device again.
		// This seems to be a minimal hit on the low memory cursor data structures
		// which will cause the cursor to re-init.
	
		SetRect((Rect *)CrsrPin,-32768,-32768,-32767,-32767);				// Set the pin rect to fail
		
		requestedMouse		= *(Point *)LomemMouse;
		
		if( cursorWhere  )
			requestedMouse = *cursorWhere;
		
		cursorOnScreen = PointOnScreen(requestedMouse,&theCursorDevice);
		
		if( !cursorOnScreen )
		{
			theCursorDevice = GetMainDevice();
			SetPt(&requestedMouse,0,0);
			SetPt((Point *)LomemMouse,-32768,-32768);
		}
		
	{
		PixMapHandle	theCursorPixMap = (*theCursorDevice)->gdPMap;
		Rect			theCursorPMBounds = (*theCursorPixMap)->bounds;
		AuxDCEHandle	theCursorDCE = GetDCtlEntry( (*theCursorDevice)->gdRefNum);
		
		(**(GDHandle *)CrsrDevice)->gdCCDepth = 0;				// Stomp the cursor depth
		*(GDHandle *)CrsrDevice = theCursorDevice;
		(*theCursorDevice)->gdCCDepth = 0;						// Stomp the cursor depth

		*(Point *)MTemp			= requestedMouse;				// Update real mouse location with
		*(Point *)RawMouse		= requestedMouse; 				// Update cursor loc with clipped pt
		*(Point *)LomemMouse	= requestedMouse;				// Update mouse position
		*(Rect *)CrsrPin		= (*theCursorDevice)->gdRect;	// Update mouse position


		*(short *)ChunkyDepth	= (*theCursorPixMap)->pixelSize;
		*(Ptr *)CrsrBase		= (*theCursorPixMap)->baseAddr;
		*(short *)CrsrRow		= ((*theCursorPixMap)->rowBytes & 0x7FFF);
		
		*(short *)ColLines		= (theCursorPMBounds.bottom - theCursorPMBounds.top);
		*(short *)RowBits		= (theCursorPMBounds.right - theCursorPMBounds.left);
					
		if( theCursorDCE )
				AttachVBL( (*theCursorDCE)->dCtlSlot );
		else
		{
			DMDebugStr("\p Attaching to a device with no slot should not happen");
			AttachVBL( 0 );			// If all else fails, go for the motherboard
		}
		if( cursorGlobals )			// On SuperMario and Horror ROMs
		{
			cursorWalkData = cursorGlobals->firstCrsrData;
			
			while( cursorWalkData )
			{
				if( (nil == theCursorData) || (cursorWalkData == (CrsrDataPtr )theCursorData) )
				{
					cursorWalkData->whereX	= (requestedMouse.h << 16);
					cursorWalkData->whereY	= (requestedMouse.v << 16);
					cursorWalkData->where	= (requestedMouse);
				}
				cursorWalkData->isAbs = true;			// Make sure we are going to check for drawing
				cursorWalkData = cursorWalkData->nextCrsrData;
			}
		}
	}
		
		//  Sets the following variables
		//		CrsrBase			= 0x898,			// [GLOBAL VAR]  (long) scrnBase for cursor
		//		CrsrDevice			= 0x89C,			// [GLOBAL VAR]  (long) current cursor device
		//		ScreenBytes			= 0x0C24,			// [GLOBAL VAR]  (long) total screen bytes
		//		ScrnVBLPtr			= 0x0D10,			// [GLOBAL VAR]  save for ptr to main screen VBL queue
		//		CrsrRow				= 0x8AC,			// [GLOBAL VAR]  (word) rowbytes for current cursor screen
		//		RowBits				= 					// [GLOBAL VAR]  (word) screen horizontal pixels
		//		ColLines			= 					// [GLOBAL VAR]  (word) screen vertical pixels

			*(Boolean *)CrsrNew		= true;				// Set flag saying that cursor has moved
		*(char *)CrsrBusy = 0;							// Resume the cursor (must happen before call to JCrsrTask)
		
		(*cursorTask)();								// Call JCrsrTask explictly to be sure low mem gets updated

		(**(GDHandle *)CrsrDevice)->gdCCDepth = 0;		// Stomp the cursor depth
		AllocCursor();

		*(ProcPtr *)JCrsrTask = cursorTask;				// Restore the cursor task

	DM_ShowCursor(cursorState);

	return(noErr);
}


pascal OSErr DM_GetDisplayMgrA5World(unsigned long* dmA5)
{
	DisplayManagerGlobalsPtr			dmGlobals = GetDMGlobalsPtr();
	OSErr								getA5Error = kDMSWNotInitializedErr;
	
	*dmA5 = 0;
	
	if( dmGlobals )
	{
		*dmA5 = &dmGlobals->fQDA5World;
		getA5Error = noErr;
	}
	return(getA5Error);
}



pascal OSErr DM_GetDisplayComponent(GDHandle theDevice,ComponentInstance *displayComponent)
{
	OSErr			getComponentError = kDMDisplayNotFoundErr;
	DisplayInfoPtr	displayInfo;

	*displayComponent = nil;
	displayInfo = DM_GetDisplayInfoByGDevice(theDevice,false);
	if( nil != displayInfo )
	{
		getComponentError = noErr;
		*displayComponent = displayInfo->displayComponent;
	}
	return(getComponentError);
}


pascal OSErr DM_SetDisplayComponent(GDHandle theDevice,ComponentInstance displayComponent)
{
	OSErr				setComponentError = kDMDisplayNotFoundErr;
	DisplayInfoPtr		displayInfo;
	ComponentInstance	existingComponent;

	if( nil == displayComponent )
		setComponentError = paramErr;
	else
	{
		displayInfo = DM_GetDisplayInfoByGDevice(theDevice,false);
		if( nil != displayInfo )
		{
			setComponentError = noErr;
			existingComponent = displayInfo->displayComponent;
			displayInfo->displayComponent = displayComponent;
			
			if( nil != existingComponent )
				CloseComponent(existingComponent);
				
		}
	}
	return(setComponentError);
}


pascal OSErr DM_SetDisplayID(GDHandle assignDevice,DisplayIDType displayID)
// assignDevice == nil	means assign ids to all displays.
// displayID == 0		means pick a new ID for specifed display IFF display has not ID already assocated with it (this means it can be called multiple times).
//
// DMSetDisplayID allocates the display info record if necessary.
//
// Caution:  assignDevice is used only to set up walkDisplay and to determine whether we
//			are assigning ids to one of all displays.  If you start using assignDevice inside the loops you
//			will be causing problems.
{
	DisplayInfoPtr				theDisplayInfo;
	DisplayIDType				tempID;
	ComponentInstance			displayComponent;
	GDHandle					walkDisplay;
	OSErr						setIDError = noErr;
	DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();
	DisplayInfoRec				newdisplayInfo;
	Boolean						needComponentNotify;								// True => we need to notify the component that the ID was assigned.

	if( (nil == assignDevice) && (0 != displayID) )									// We can only assign displayIDs to multiple devices if the displayID is nil
		setIDError = paramErr;														// This makes sure a nil device is innocuous
	else if( dmGlobals )
	{
		if( assignDevice )	walkDisplay = assignDevice;								// If we doing only a single device, then use it
		else				walkDisplay = DMGetFirstScreenDevice(false);			// Otherwise start at the beginning of the displays
		
		while( walkDisplay && noErr == setIDError )
		{
			needComponentNotify = false;											// Init flag to tell us if the component needs notification.
			
			//
			// Check to see if an info record is already allocated.
			//
			theDisplayInfo = DM_GetDisplayInfoByGDevice(walkDisplay,false);
			if( nil == theDisplayInfo )												// If there is not record allocated, then make one
			{
				if( displayID )	tempID = displayID;									// If we have no display ID.
				else			tempID = dmGlobals->fDisplayNextID++;				// Pick a new one.
					
				InitNewDisplayInfo(tempID,0,walkDisplay,0,0,&newdisplayInfo);		// Make a new info
				setIDError = DM_SetDisplayInfoByDisplayID(&newdisplayInfo);			// Install the new info
				needComponentNotify = true;
			}
			else																	// We did get a record for this display
			{
				// We assign a new display ID if & only if:
				//	1) Setting a particular device to be a particular (non-zero) ID or
				//	2) Setting unset device info (this should never happen because devices
				//			are assigned IDs when their info records are first allocated - see above).
				//
				
				if( 0 == theDisplayInfo->displayID || nil != displayID )			// If we are hitting a specific device
				{
					if( displayID )	tempID = displayID;								// If we have no display ID.
					else			tempID = dmGlobals->fDisplayNextID++;			// Pick a new one.
	
	
					// If we are assigning display IDs to all displays, then we long assign IDs to those displays
					// that do not already have an ID.
	
					if( theDisplayInfo->displayID != displayID )					// If the ID changed
					{
						theDisplayInfo->displayID = displayID;						// Finally, what the user wanted us to do
						needComponentNotify = true;									// And mark that we need to tell the component too
					}
				}
				setIDError = noErr;													// Success
			}
			
			if( needComponentNotify && noErr == DMGetDisplayComponent(walkDisplay,&displayComponent) && nil != displayComponent )
				DisplayComponentSetDisplayID(displayComponent, tempID);				// If we have assigned a new ID, make sure the component knows about it

			if( assignDevice )	walkDisplay = nil;										// If we doing only a single device, we are done
			else				walkDisplay = DMGetNextScreenDevice(walkDisplay,false);	// Otherwise get the next display
		}
	}
	else
		setIDError = kDMSWNotInitializedErr;
	return( setIDError );
}
