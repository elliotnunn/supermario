/*
	File:		CommResourceMgr.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	  7/2/90	kaz		Defining some macros
		 <1>	 3/15/90	BBH		first checked in

	To Do:
*/


// macros for CRM use

//	returns header to CRM queue
#define	CRMGetHeader 	(QHdrPtr) ((CTBBlockPtr) CRMGetGlobals)->CRMQueue

//	this returns the top of the resource count list
#define	GetList 		(CRMResourceCountPtr) ((CTBBlockPtr) CRMGetGlobals)->resList


//	prototypes for internal routiens for CommResourceMgr.c

pascal long			GetDeviceID(long dType);
CRMResourceCountPtr	FindResource(Handle hResource);
void 				ClearResource(short refnum);
CRMResourceCountPtr	RegisterDevice(Handle hResource);
void 				RemoveEntry(CRMResourceCountPtr pMatch);
void 				HandleResource(Handle hResource);
