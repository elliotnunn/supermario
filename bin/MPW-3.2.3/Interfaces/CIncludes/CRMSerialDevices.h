/*
Created Tuesday, May 9, 1989 at 6:00 AM
	CRMSerialDevices.h
	C interface to the Communications Resource Manager Serial Device Utilities

	Copyright Apple Computer, Inc.  1988-90
	All rights reserved

*/

#ifndef __CRMSERIALDEVICES__
#define __CRMSERIALDEVICES__

#include <Types.h>

/*	for the crmDeviceType field of the CRMRec data structure */
#define	crmSerialDevice		1			

/*  version of the CRMSerialRecord below */
#define	curCRMSerRecVers	1


/* Maintains compatibility w/ apps & tools that expect an old style icon	*/
struct CRMIconRecord {
	long		oldIcon[32];			/* ICN#	*/
	long		oldMask[32];
	Handle		theSuite;				/* Handle to an IconSuite	*/
	long		reserved;
};

typedef struct CRMIconRecord CRMIconRecord;
typedef CRMIconRecord *CRMIconPtr, **CRMIconHandle;

struct CRMSerialRecord {
	short			version;
	StringHandle	inputDriverName;
	StringHandle	outputDriverName;
	StringHandle	name;
	CRMIconHandle	deviceIcon;
	long			ratedSpeed;
	long			maxSpeed;
	long			reserved;
};

typedef struct CRMSerialRecord CRMSerialRecord;

typedef CRMSerialRecord *CRMSerialPtr;

#endif __CRMSERIALDEVICES__