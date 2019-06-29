/*
	File:		Connections.h

	Contains:	C Interface to the Connection Manager

	Copyright:	© 1988-1990 by Apple Computer, Inc.
				All rights reserved.

*/


#ifndef	__CONNECTIONS__
#define __CONNECTIONS__

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif	__DIALOGS__

#ifndef __CTBUTILITIES__
#include <CTBUtilities.h>
#endif	__CTBUTILITIES__

/*	current Connection Manager version	*/
#define	curCMVersion 		2

/*	current Connection Manager Environment Record version 	*/
#define	curConnEnvRecVers	0
	
/* CMErr */
typedef	OSErr CMErr;

#define cmGenericError 		(-1)
#define	cmNoErr				0
#define cmRejected			1
#define cmFailed			2
#define cmTimeOut			3
#define cmNotOpen			4
#define cmNotClosed			5
#define cmNoRequestPending	6
#define cmNotSupported 		7
#define cmNoTools			8	
#define	cmUserCancel		9
#define	cmUnknownError		11

/* CMRecFlags and CMChannel		*/
/*		Low word of CMRecFlags is same as CMChannel	*/
typedef long CMRecFlags;
typedef short CMChannel;

enum {
 	cmData 			= 1 << 0,
	cmCntl 			= 1 << 1,
	cmAttn 			= 1 << 2,
	
	cmDataNoTimeout	= 1 << 4,
	cmCntlNoTimeout = 1 << 5,
	cmAttnNoTimeout = 1 << 6,

	cmDataClean 	= 1 << 8,		
	cmCntlClean		= 1 << 9,
	cmAttnClean		= 1 << 10,

/*		Only for CMRecFlags (not CMChannel) in the rest of this enum	*/
	cmNoMenus 		= 1 << 16,	
	cmQuiet			= 1 << 17
};

/*	connection status flags	*/
typedef long CMStatFlags;

enum {
	cmStatusOpening		= 1 << 0,
	cmStatusOpen 		= 1 << 1,
	cmStatusClosing 	= 1 << 2,
	cmStatusDataAvail 	= 1 << 3,
	cmStatusCntlAvail 	= 1 << 4,
	cmStatusAttnAvail 	= 1 << 5,
	
	cmStatusDRPend 		= 1 << 6,		/* data read pending	*/
	cmStatusDWPend 		= 1 << 7,		/* data write pending	*/
	cmStatusCRPend 		= 1 << 8,		/* cntl read pending	*/
	cmStatusCWPend 		= 1 << 9,		/* cntl write pending	*/
	cmStatusARPend 		= 1 << 10,		/* attn read pending	*/
	cmStatusAWPend 		= 1 << 11,		/* attn write pending	*/

	cmStatusBreakPend 	= 1 << 12,
	cmStatusListenPend 	= 1 << 13,
	cmStatusIncomingCallPresent 
						= 1 << 14,

	cmStatusReserved0	= 1 << 15
};

/*	CMBuffers	fields	*/
typedef short CMBufFields;

enum {
	cmDataIn,
	cmDataOut,
	
	cmCntlIn,
	cmCntlOut,
	
	cmAttnIn,
	cmAttnOut,
	
	cmRsrvIn,
	cmRsrvOut
};

typedef Ptr CMBuffers[8];
typedef long CMBufferSizes[8];

/*	CMSearchFlags	*/
typedef short CMSearchFlags;

enum {
	cmSearchSevenBit =	1 << 0
};

/*	CMFlags (CMRead/Write deliminator flags)	*/
typedef short CMFlags;
enum {
	cmFlagsEOM		=	1 << 0
};

struct ConnEnvironRec {
	short		version;
	long		baudRate;
	short		dataBits;
	CMChannel	channels;
	Boolean		swFlowControl;
	Boolean		hwFlowControl;
	CMFlags		flags;
};
typedef struct ConnEnvironRec ConnEnvironRec;

typedef ConnEnvironRec *ConnEnvironRecPtr;


struct ConnRecord {
	short		procID;
	
	CMRecFlags	flags;
	CMErr		errCode;
	
	long		refCon;
	long		userData;
	
	ProcPtr		defProc;
	
	Ptr			config;
	Ptr			oldConfig;
	
	long		asyncEOM;
	
	long		reserved1;
	long		reserved2;

	Ptr			cmPrivate;

	CMBuffers		bufferArray;
	CMBufferSizes	bufSizes;

	long		mluField;
	
	CMBufferSizes	asyncCount;
};

typedef struct ConnRecord ConnRecord;
typedef ConnRecord *ConnPtr, **ConnHandle;


#ifdef __cplusplus
extern "C" {
#endif	
pascal CMErr		InitCM(void);
pascal Handle 	CMGetVersion(ConnHandle hConn);
pascal short 	CMGetCMVersion(void);

pascal ConnHandle	CMNew(short procID, CMRecFlags flags, const CMBufferSizes desiredSizes, 
								long refCon, long userData);

pascal void 		CMDispose(ConnHandle hConn);

pascal CMErr 	CMListen(ConnHandle hConn, Boolean async, ProcPtr completor, long timeout);
pascal CMErr 	CMAccept(ConnHandle hConn, Boolean accept);

pascal CMErr 	CMOpen(ConnHandle hConn, Boolean async, ProcPtr completor, long timeout);
pascal CMErr 	CMClose(ConnHandle hConn, Boolean async, ProcPtr completor, long timeout, Boolean now);

pascal CMErr 	CMAbort(ConnHandle hConn);

pascal void 		CMIdle(ConnHandle hConn);
pascal CMErr 	CMStatus(ConnHandle hConn, CMBufferSizes sizes, CMStatFlags *flags);

pascal void 		CMReset(ConnHandle hConn);

pascal void 		CMBreak(ConnHandle hConn, long duration, Boolean async, 
							ProcPtr completor);

pascal CMErr 	CMRead(ConnHandle hConn, Ptr theBuffer, long *toRead, 
							CMChannel theChannel, Boolean async, ProcPtr completor, long timeout, 
							CMFlags *flags);

pascal CMErr 	CMWrite(ConnHandle hConn, Ptr theBuffer, long *toWrite,
							CMChannel theChannel, Boolean async, ProcPtr completor, long timeout, 
							CMFlags flags);

pascal CMErr 	CMIOKill(ConnHandle hConn, short which);

pascal void 		CMActivate(ConnHandle hConn, Boolean activate);
pascal void 		CMResume(ConnHandle hConn, Boolean resume);
pascal Boolean 	CMMenu(ConnHandle hConn, short menuID, short item);

pascal Boolean 	CMValidate(ConnHandle hConn);
pascal void 		CMDefault(Ptr *theConfig, short procID, Boolean allocate);

pascal Handle 	CMSetupPreflight(short procID, long *magicCookie);
pascal Boolean 	CMSetupFilter(short procID, Ptr theConfig, short count, 
							DialogPtr theDialog, EventRecord *theEvent, short *theItem, 
							long *magicCookie);
pascal void 		CMSetupSetup(short procID, Ptr theConfig, short count, 
							DialogPtr theDialog, long *magicCookie);
pascal void 		CMSetupItem(short procID, Ptr theConfig, short count, 
							DialogPtr theDialog, short *theItem, long *magicCookie);
pascal void 		CMSetupCleanup(short procID, Ptr theConfig, short count, 
							DialogPtr theDialog, long *magicCookie);
pascal void 		CMSetupPostflight(short procID);

pascal Ptr 		CMGetConfig(ConnHandle hConn);
pascal short 	CMSetConfig(ConnHandle hConn, Ptr thePtr);

pascal OSErr 	CMIntlToEnglish(ConnHandle hConn, 
							Ptr inputPtr, Ptr *outputPtr, short language);
pascal OSErr 	CMEnglishToIntl(ConnHandle hConn, 
							Ptr inputPtr, Ptr *outputPtr, short language);

pascal long 		CMAddSearch(ConnHandle hConn, const Str255 theString, 
							CMSearchFlags flags, ProcPtr callBack);
pascal void		CMRemoveSearch(ConnHandle hConn, long refnum);
pascal void		CMClearSearch(ConnHandle hConn);

pascal CMErr 	CMGetConnEnvirons(ConnHandle hConn, ConnEnvironRec *theEnvirons);
		
pascal short 	CMChoose(ConnHandle *hConn, Point where, ProcPtr idleProc);

pascal void		CMEvent(ConnHandle hConn, const EventRecord *theEvent);

pascal void 		CMGetToolName(short procID, Str255 name);
pascal short 	CMGetProcID(const Str255 name);

pascal void		CMSetRefCon(ConnHandle hConn, long refCon);
pascal long 		CMGetRefCon(ConnHandle hConn);

pascal long 		CMGetUserData(ConnHandle hConn);
pascal void 		CMSetUserData(ConnHandle hConn, long userData);



#ifdef __cplusplus
}
#endif

#endif __CONNECTIONS__