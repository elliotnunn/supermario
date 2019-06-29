/*
	File:		CommResources.h

	Copyright:	© 1990 by Apple Computer, Inc.
				All rights reserved.

*/

#ifndef __COMMRESOURCES__
#define __COMMRESOURCES__

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

/*	version of the Comm Resource Manager	*/
#define				curCRMVersion		2

/*	tool classes (also the tool file types)	*/
#define				classCM				'cbnd'
#define				classFT				'fbnd'
#define				classTM				'tbnd'


/*	error codes */
typedef				OSErr	CRMErr;

#define				crmGenericError		(-1)
#define				crmNoErr			0

/*	crm data structures	*/
#define				crmType				9			/* queue type	*/
#define				crmRecVersion  		1			/* version of queue structure */

struct CRMRec {
	QElemPtr		qLink;
	short			qType;
	short			crmVersion;
	long			crmPrivate;
	short			crmReserved;

	long			crmDeviceType;
	long			crmDeviceID;
	long			crmAttributes;
	long			crmStatus;

	long			crmRefCon;
};

typedef struct CRMRec CRMRec;
typedef CRMRec *CRMRecPtr;

#ifdef __cplusplus
extern "C" {
#endif	


pascal CRMErr 	InitCRM(void);
pascal QHdrPtr 	CRMGetHeader(void);
pascal void 		CRMInstall(QElemPtr crmReqPtr);
pascal OSErr 	CRMRemove(QElemPtr crmReqPtr);
pascal QElemPtr 	CRMSearch(QElemPtr crmReqPtr);
pascal short 	CRMGetCRMVersion(void);

pascal Handle 	CRMGetResource(ResType theType, short theID);
pascal Handle 	CRMGet1Resource(ResType theType, short theID);
pascal Handle 	CRMGetIndResource(ResType theType, short index);
pascal Handle 	CRMGet1IndResource(ResType theType, short index);
pascal Handle 	CRMGetNamedResource(ResType theType, const Str255 name);
pascal Handle 	CRMGet1NamedResource(ResType theType, const Str255 name);
pascal void 		CRMReleaseResource(Handle theHandle);
pascal Handle 	CRMGetToolResource (short procID, ResType theType, short theID);
pascal void   	CRMReleaseToolResource (short procID, Handle theHandle);

pascal long 		CRMGetIndex(Handle theHandle);

pascal short 	CRMLocalToRealID(ResType bundleType, short toolID, ResType theKind, short localID);
pascal short 	CRMRealToLocalID(ResType bundleType, short toolID, ResType theKind, short realID);
 
pascal OSErr		CRMGetIndToolName(OSType bundleType, short index, Str255 toolName);

pascal OSErr 	CRMFindCommunications(short *vRefNum, long *dirID);

#ifdef __cplusplus
}
#endif

#endif __COMMRESOURCES__