/*
	File:		CRMBuiltInSerial.c

	Contains:	Registers the Serial & Modem Ports with the CommResource Mgr

	Written by:	Byron Han

	Copyright:	© 1988-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <5>	 1/27/91	gbm		csd,#gbm-0001: Make failure to find an icon an okay situation.
									Well, not exactly okay, but we'll make it not crash...
		 <4>	  1/9/91	JDR		(dba) Private Icon routines are now in IconUtilsPriv.h.
		 <3>	  1/9/91	kaz		<jng> Fixing bug in call to GetIconSuite: was passing an address
									that could move. Get driver names from resources.
		 <2>	 10/2/90	kaz		Support for icon suites: uses IconUtils to Get the color icons.
									<jng>
		 <1>	 4/11/90	BBH		first checked into CommToolboxStartup project
*/

#include <Memory.h>
#include <Resources.h>
#include <ToolUtils.h>
#include <OSUtils.h>
#include <IconUtilsPriv.h>

#include <CommResources.h>
#include <CRMSerialDevices.h>

#include <CommToolboxPriv.h>

#define		portNameIndex		1		// 'STR#' indices
#define		inputDriverIndex	2
#define		outputDriverIndex	3

Handle	MyGetString(short resID, short theIndex)
{
	Str255		theString;
	
	GetIndString(theString, resID, theIndex);
	return((Handle) NewString(theString));
}

pascal void RegisterBuiltinPorts()
{
	short			i;
	CRMRecPtr		pCRMRec;
	CRMSerialPtr	pSerial;
	Handle			hIcon;
	THz				savedZone;
	CRMIconHandle	theCRMIcon;
	
	for (i = (CTB_ModemIconID); i <= (CTB_PrinterIconID); i++) {
		pCRMRec = (CRMRecPtr) NewPtrSysClear(sizeof(CRMRec));
		pSerial = (CRMSerialPtr) NewPtrSysClear(sizeof(CRMSerialRecord));
		
		pCRMRec->crmDeviceType = crmSerialDevice;
		pCRMRec->crmAttributes = (long) pSerial;
	
		pSerial->version = curCRMSerRecVers;
			
		savedZone = GetZone();
		SetZone(SystemZone());
		
			// Get the new fangled icon suite
			theCRMIcon = (CRMIconHandle) NewHandleSys(sizeof(CRMIconRecord));		
			GetIconSuite(&hIcon,i,svAllLargeData);
			(**theCRMIcon).theSuite = hIcon;
			
			// Extract the old style icon & move into the struct
			GetIconFromSuite(&hIcon, (**theCRMIcon).theSuite, 'ICN#');
			if ((hIcon)&&(theCRMIcon))
				BlockMove((Ptr) *hIcon, (Ptr) &((**theCRMIcon).oldIcon), sizeof(long) << 6);	// icon & mask
			
			(**theCRMIcon).reserved = 0;
			
			// Get the names of the port & drivers
			pSerial->name = MyGetString(i, portNameIndex);		
			pSerial->inputDriverName = MyGetString(i, inputDriverIndex);	
			pSerial->outputDriverName = MyGetString(i, outputDriverIndex);	
			
		SetZone(savedZone);

		pSerial->deviceIcon = theCRMIcon;
		
		pSerial->ratedSpeed = 9600;
		pSerial->maxSpeed = 57600;
		
		CRMInstall( (QElemPtr) pCRMRec );
	}
}