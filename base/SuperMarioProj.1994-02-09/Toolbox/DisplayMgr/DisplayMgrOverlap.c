/*
	File:		DisplayMgrOverlap.c

	Contains:	Overlapping and purely overlapping (aka “mirroring”) support for
				the Display Manager.

	Written by:	Mike Puckett, September 29, 1993.

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 11/5/93	IH		Sync with Excelsior.
		 <1>	10/10/93	IH		first checked in
		----------------------------------------------
			Pre-SuperMario Comments
		----------------------------------------------
		 <3>	10/10/93	IH		#1116329: Fixed mirroring call to use new process manager check
									(so it is usable at extension loading time).
		 <2>	10-08-93	jmp		In the MirrorDevices() routine, made it so that we now activate
									any inactive devices before mirroring them.  Also, took a first
									crack at cleaning up the cursor flashing by calling
									DM_Hide/ShowCursor().
		 <1>	10-04-93	jmp		first checked in
*/

#ifndef __TYPES__
#include "Types.h"
#endif

#ifndef __GESTALTEQU__
#include "GestaltEqu.h"
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

/* QDIsMirroringCapable() returns whether or not QuickDraw can handle mirroring.
//
// We are distinguishing between mirroring and overlap, as mirroring-capable
// is a superset of overlapping-capable.  This distinction will eventually
// go away (I think).
*/
pascal OSErr
DM_QDIsMirroringCapable(
	Boolean	*qdIsMirroringCapable)
	
{	long	response;

	// Assume that we’re not mirroring capable.
	//
	*qdIsMirroringCapable = false;
	
	// Call Gestalt to get the QuickDraw feature set.  If QD is mirroring
	// capable, the appropriate bit will be set.
	//
	if (Gestalt(gestaltQuickdrawFeatures,&response) == noErr)
		*qdIsMirroringCapable = (response & (1<<gestaltSupportsMirroring))? true: false;

	return(noErr);
	}

/* CanMirrorNow() returns whether or not the current environment will support mirroring.
// At the moment, mirroring only works if and only if there are 2 displays.  In the
// future, we should be able to mirror any number of devices.  We might even support
// distinct set of mirrors, who knows?
//
// Note:  Several third parties have asked us to let them to simply block mirroring.
// 		  This is supported thru the DMBlockMirroring() and DMUnblockMirroring() calls.
//		  CanMirrorNow() supports these routines.
*/
pascal OSErr
DM_CanMirrorNow(
	Boolean						*canMirrorNow)
	
{	OSErr						theErr = kSysSWTooOld;
	Boolean						qdIsMirroringCapable;
	
	// Assume that we can’t mirror now.
	//
	*canMirrorNow = false;
	
	// Without IPC support from the Process Manager, just bail.
	//
	//	Except we can do mirroring early in the boot before the process manager arrives.
	//
	if(IsProcessMgrOkay())
	{	//
		// We can only mirror if: 1) QDIsMirroringCapable and 2) there are
		// only 2 screen devices.  The 2-device-only restriction should
		// go away someday.
		//
		if ((noErr == DMQDIsMirroringCapable(&qdIsMirroringCapable)) && qdIsMirroringCapable)
		{	DisplayManagerGlobalsPtr dmGlobals = GetDMGlobalsPtr();
			theErr = kDMMirroringBlocked;
			
			if (!dmGlobals->fMirroringBlocked)
			{	theErr = kDMWrongNumberOfDisplays;
				
				if (GetActiveScreenDeviceCount(false) == kDMNumDisplaysSupportedByMirroring)
				{	*canMirrorNow = true;
					theErr = noErr;
					}
				}
			}
		}

	return(theErr);
	}

/* IsMirroringOn() returns whether or not mirroring has been enabled.
*/
pascal OSErr
DM_IsMirroringOn(
	Boolean *isMirroringOn)
	
{	DisplayManagerGlobalsPtr dmGlobals = GetDMGlobalsPtr();
	*isMirroringOn = dmGlobals->fDevicesMirrored;

	return(noErr);
	}
	
/* MirrorDevices() attempts to mirror two gDevices.  At the moment, since mirroring is only
// capable of handling 2 gDevices, it will return an error if mirroing is already on
// when it is called.
*/
#define dontFailToMain	false
#define MIN(a,b)		(((a) < (b))? (a): (b))

pascal OSErr
DM_MirrorDevices(
	GDHandle			gD1,
	GDHandle			gD2,
	DisplaysStateHdl	displayState)
	
{	Boolean				cursorState = DM_HideCursor();
	OSErr				theErr;
	THz					savedZone;

	theErr = PrepImmediateUpdate(noErr, &displayState, &savedZone);
	
	if (noErr == theErr)
	{	Boolean						canMirrorNow;
		DisplayManagerGlobalsPtr	dmGlobals = GetDMGlobalsPtr();
		
		theErr = DMCanMirrorNow(&canMirrorNow);
		if (canMirrorNow)
		{	theErr = kDMMirroringOnAlready;

			/* If we can’t mirror now and we’re not already mirroring, then
			// just get out of here for now.
			*/
			if (!dmGlobals->fDevicesMirrored)
			{	DisplayInfoPtr				displayInfo1,
											displayInfo2;
				GDHandle					theMainDevice = GetMainDevice();
				
				theErr = kDMGenErr;
				
				/* Make sure that the two passed-in devices are valid.  Also, if either
				// of the passed-in devices are inactive, then activate them now.
				//
				// Question:  When mirroring is turned off, should we deactivate
				// 			  devices that started out inactive?
				*/
				if (IsActiveScreenDevice(gD1,false) && IsActiveScreenDevice(gD2,false))
				{	if (!TestDeviceAttribute(gD1,screenActive))
						DMEnableDisplay(gD1,nil);
					if (!TestDeviceAttribute(gD2,screenActive))
						DMEnableDisplay(gD2,nil);
				
					displayInfo1 = DM_GetDisplayInfoByGDevice(gD1,dontFailToMain);
					displayInfo2 = DM_GetDisplayInfoByGDevice(gD2,dontFailToMain);
					
					/* Make sure the Display Manager knows about these devices,
					// for now and later.
					*/
					if ((nil != displayInfo1) && (nil != displayInfo2))
					{	unsigned short		hRectLength1,
											hRectLength2,
											vRectLength1,
											vRectLength2;
						Ptr					origBaseAddr;
						Rect				origRect,
											mirrorRect;
						
						origBaseAddr = (*(*theMainDevice)->gdPMap)->baseAddr;
						origRect	 = (*theMainDevice)->gdRect;
						
						displayInfo1->unMirroredBaseAddr = (*(*gD1)->gdPMap)->baseAddr;
						displayInfo1->unMirroredRect	 = (*gD1)->gdRect;
						displayInfo1->mirroredDevice	 = gD2;
						
						displayInfo2->unMirroredBaseAddr = (*(*gD2)->gdPMap)->baseAddr;
						displayInfo2->unMirroredRect	 = (*gD2)->gdRect;
						displayInfo2->mirroredDevice	 = gD1;
						
						/* Get the dimensions of both devices.
						*/
						hRectLength1 = (*gD1)->gdRect.right  - (*gD1)->gdRect.left;
						vRectLength1 = (*gD1)->gdRect.bottom - (*gD1)->gdRect.top;
						hRectLength2 = (*gD2)->gdRect.right  - (*gD2)->gdRect.left;
						vRectLength2 = (*gD2)->gdRect.bottom - (*gD2)->gdRect.top;
		
						/* The mirroring rectangle is defined as the smallest rectangle
						// that contains both devices.
						*/
						mirrorRect.top		= 0;	
						mirrorRect.left		= 0;
						mirrorRect.bottom	= MIN(vRectLength1,vRectLength2);
						mirrorRect.right	= MIN(hRectLength1,hRectLength2);
						
						/* Do it!
						*/
						(*gD1)->gdRect = (*gD2)->gdRect = mirrorRect;
						dmGlobals->fDevicesMirrored = dmGlobals->fDevicesOverLap = true;
							
						/* What if these fail?  I think we’re hosed.  We should probably have
						// some sort of restoration capability here.
						*/
						DMMoveDisplay(gD1,0,0,nil);
						DMMoveDisplay(gD2,0,0,nil);
						
						/* Since we only work with two devices, one of them, by definition, must be the
						// mainScreen device.  Do the final clean up here using the MainDevice.  
						*/
						FixLowMem(theMainDevice,theMainDevice,0,0);
						FixPorts(theMainDevice,false,&(*theMainDevice)->gdRect,(*theMainDevice)->gdPMap,
								 &origRect,origBaseAddr,(*(*theMainDevice)->gdPMap)->pmTable);
						FixWindowMgrPorts(theMainDevice);
						
						CallAllProcesses((ProcPtr )InterProcessChangeMainDevice,(Ptr )theMainDevice);
						
						theErr = noErr;
						}
					}
				}
			}
		}
	
	theErr = FinishImmediateUpdate(theErr,displayState,savedZone);
	DM_ShowCursor(cursorState);

	return(theErr);
	}
	
/* UnmirrorDevice() removes a particular gDevice from a mirrored set.  At the moment, mirroring
// only works with two devices, so UnmirrorDevice() effectively turns mirroring off.
*/
pascal OSErr
DM_UnmirrorDevice(
	GDHandle			gDevice,
	DisplaysStateHdl	displayState)
	
{	Boolean				cursorState = DM_HideCursor();
	OSErr				theErr;
	THz					savedZone;

	theErr = PrepImmediateUpdate(noErr, &displayState, &savedZone);
	
	if (noErr == theErr)
	{	DisplayManagerGlobalsPtr dmGlobals = GetDMGlobalsPtr();
		theErr = kDMMirroringNotOn;

		/* If we’re not mirroring, then we can’t unmirror.
		*/
		if (dmGlobals->fDevicesMirrored)
		{	theErr = kDMGenErr;
		
			/* If the passed-in device isn’t active, then it hasn’t been mirrored.
			*/
			if (IsActiveScreenDevice(gDevice,true))
			{	DisplayInfoPtr displayInfo = DM_GetDisplayInfoByGDevice(gDevice,dontFailToMain);
			
				/* Make sure that the Display Manager knows what’s going on.
				*/
				if (nil != displayInfo)
				{	GDHandle mirroredDevice = displayInfo->mirroredDevice;
					
					/* Make sure that the device we’re supposed to unmirror was really
					// mirrored to something else.  And if so, go thru the restoration
					// process, which includes clearing out the globals.
					*/
					if (nil != mirroredDevice)
					{	GDHandle	theMainDevice;
						Ptr			mirroredBaseAddr;
						Rect		mirroredRect;
						
						dmGlobals->fDevicesMirrored = dmGlobals->fDevicesOverLap = false;
						
						/* Fix up gDevice…
						*/
						mirroredBaseAddr = (*(*gDevice)->gdPMap)->baseAddr;
						mirroredRect = (*gDevice)->gdRect;
						
						(*(*gDevice)->gdPMap)->baseAddr = displayInfo->unMirroredBaseAddr;
						(*(*gDevice)->gdPMap)->bounds = displayInfo->unMirroredRect;
						(*gDevice)->gdRect = displayInfo->unMirroredRect;
						
						displayInfo->unMirroredBaseAddr = nil;
						displayInfo->mirroredDevice = nil;
						SetRect(&displayInfo->unMirroredRect,0,0,0,0);
						
						DMMoveDisplay(gDevice,0,0,nil);
						FixPorts(gDevice,false,&(*gDevice)->gdRect,(*gDevice)->gdPMap,
								 &mirroredRect,mirroredBaseAddr,(*(*gDevice)->gdPMap)->pmTable);

						/* Fix up mirroredDevice…
						*/
						displayInfo = DM_GetDisplayInfoByGDevice(mirroredDevice,dontFailToMain);
						
						mirroredBaseAddr = (*(*mirroredDevice)->gdPMap)->baseAddr;
						mirroredRect = (*mirroredDevice)->gdRect;
						
						(*(*mirroredDevice)->gdPMap)->baseAddr = displayInfo->unMirroredBaseAddr;
						(*(*mirroredDevice)->gdPMap)->bounds = displayInfo->unMirroredRect;
						(*mirroredDevice)->gdRect = displayInfo->unMirroredRect;
				
						displayInfo->unMirroredBaseAddr = nil;
						displayInfo->mirroredDevice = nil;
						SetRect(&displayInfo->unMirroredRect,0,0,0,0);
						
						DMMoveDisplay(mirroredDevice,0,0,nil);
						FixPorts(mirroredDevice,false,&(*mirroredDevice)->gdRect,(*mirroredDevice)->gdPMap,
								 &mirroredRect,mirroredBaseAddr,(*(*mirroredDevice)->gdPMap)->pmTable);
												
						/* Since we only work with two devices, one of them, by definition, must be the
						// mainScreen device.  Do the final clean up here using the MainDevice.  
						*/
						theMainDevice = GetMainDevice();
						FixLowMem(theMainDevice,theMainDevice,0,0);
						FixWindowMgrPorts(theMainDevice);
						
						CallAllProcesses((ProcPtr )InterProcessChangeMainDevice,(Ptr )theMainDevice);

						theErr = noErr;
						}
					}
				}
			}
		}

	theErr = FinishImmediateUpdate(theErr,displayState,savedZone);
	DM_ShowCursor(cursorState);
	
	return(theErr);
	}
	
/* GetNextMirroredDevice() returns the device to which the known gDevice
// is mirrored.  By definition, the link of mirrored devices is circular.
*/
pascal OSErr
DM_GetNextMirroredDevice(
	GDHandle	gDevice,
	GDHandle	*mirroredDevice)
	
{	OSErr		theErr = kDMMirroringNotOn;
	Boolean		mirroringIsOn;

	/* Assume we’ll fail (safety first, you know).
	*/ 
	*mirroredDevice = nil;
	
	/* Make sure mirroring is even on.
	*/
	if ((noErr == DMIsMirroringOn(&mirroringIsOn)) && mirroringIsOn)
	{	theErr = kDMGenErr;
	
		/* If the passed-in device isn’t active, then it hasn’t been mirrored.
		*/
		if (IsActiveScreenDevice(gDevice,true))
		{	DisplayInfoPtr displayInfo = DM_GetDisplayInfoByGDevice(gDevice,dontFailToMain);
			
			/* Everything checks out, so returned the mirroredDevice.
			*/
			if (displayInfo != nil)
				*mirroredDevice = displayInfo->mirroredDevice;
			}
		}
	
	return(theErr);
	}

/* BlockMirroring() allows mirroring to be disabled.  Several third-parties have
// asked for this.  If mirroring is already on, this routine will simply ignore
// the request.
*/
pascal OSErr
DM_BlockMirroring(
	void)
	
{	OSErr	theErr = kDMCantBlock;
	Boolean	mirroringIsOn;
	
	if ((noErr == DMIsMirroringOn(&mirroringIsOn)) && !mirroringIsOn)
	{	DisplayManagerGlobalsPtr dmGlobals = GetDMGlobalsPtr();
		dmGlobals->fMirroringBlocked = true;
		theErr = noErr;
		}
		
	return(theErr);
	}

/* UnblockMirroing() allows mirroring to be enabled again.  This call has
// no restrictions.
*/
pascal OSErr
DM_UnblockMirroring(
	void)
	
{	DisplayManagerGlobalsPtr dmGlobals = GetDMGlobalsPtr();
	dmGlobals->fMirroringBlocked = false;
	
	return(noErr);
	}