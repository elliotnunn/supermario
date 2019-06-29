/*
	File:		FileTransfers.h

	Contains:	C Interface to the File Transfer Manager

	Copyright:	© 1988-1990 by Apple Computer, Inc.
				All rights reserved.

*/


#ifndef __FILETRANSFERS__
#define __FILETRANSFERS__

#ifndef __PACKAGES__
#include <Packages.h>
#endif	__PACKAGES__

#ifndef __CTBUTILITIES__
#include <CTBUtilities.h>
#endif	__CTBUTILITIES__

/* current file transfer manager version	*/
#define		curFTVersion		2

/* FTErr	*/
typedef	OSErr FTErr;

#define		ftGenericError		(-1)
#define		ftNoErr				0
#define 	ftRejected			1
#define 	ftFailed			2
#define 	ftTimeOut			3
#define		ftTooManyRetry		4
#define		ftNotEnoughDSpace	5
#define		ftRemoteCancel		6
#define		ftWrongFormat		7
#define		ftNoTools			8	
#define		ftUserCancel		9
#define 	ftNotSupported 		10

/* FTDirection 	*/
typedef short FTDirection;

enum {
	ftReceiving,
	ftTransmitting,
	ftFullDuplex
};

/* FTFlags	*/
typedef unsigned long FTFlags;

enum {
	ftIsFTMode 	= 1 << 0,
	ftNoMenus 	= 1 << 1,
	ftQuiet 	= 1 << 2,
	ftSucc 		= 1 << 7
};

/* FTAttributes	*/
typedef short FTAttributes;
enum {
	ftSameCircuit 		= 1 << 0,
	ftSendDisable 		= 1 << 1,
	ftReceiveDisable 	= 1 << 2,
	ftTextOnly 			= 1 << 3,
	ftNoStdFile 		= 1 << 4
};

/* FTChoose data structures */
struct FTChooseRec {
	long	reserved;
	long	msg;
	ProcPtr	idleProc;
	ProcPtr	filterProc;
	Str63	newTool;
	Ptr		newConfig;
	ProcPtr	eventProc;
};

typedef struct FTChooseRec FTChooseRec;

typedef FTChooseRec *FTChooseRecPtr;

/* FTChooseRec messages */
#define	ftChooseNochange		1
#define	ftChooseBackdoor		2


struct FTRecord
{	
	short			procID;

	FTFlags			flags;
	FTErr			errCode;

	long			refCon;
	long			userData;

	ProcPtr			defProc;

  	Ptr				config;
  	Ptr				oldConfig;

	ProcPtr			environsProc;
	long			reserved1;
	long			reserved2;
		
	Ptr				ftPrivate;

 	ProcPtr			sendProc;
  	ProcPtr			recvProc;
	ProcPtr			writeProc;
	ProcPtr			readProc;

 	WindowPtr		owner;
	
	FTDirection		direction;
  	SFReply			theReply;
 
  	long  			writePtr;
  	long  			readPtr;
  	char  			*theBuf;
	long			bufSize;
	Str255			autoRec;
	FTAttributes	attributes;
};

typedef struct FTRecord FTRecord;

typedef FTRecord  *FTPtr, **FTHandle;
	 
	 
#ifdef __cplusplus
extern "C" {
#endif		
pascal FTErr 	InitFT(void);
pascal Handle 	FTGetVersion(FTHandle hFT);
pascal short 	FTGetFTVersion(void);
	
pascal FTHandle FTNew(short procID, FTFlags flags, ProcPtr sendProc, 
						ProcPtr recvProc, ProcPtr readProc, ProcPtr writeProc, 
						ProcPtr environsProc, WindowPtr owner, long refCon, 
						long userData);

pascal void 	FTDispose(FTHandle hFT);

pascal FTErr 	FTStart(FTHandle hFT, FTDirection direction, const SFReply *fileInfo);
pascal FTErr 	FTAbort(FTHandle hFT);

pascal void 	FTExec(FTHandle hFT);

pascal void 	FTActivate(FTHandle hFT, Boolean activate);
pascal void 	FTResume(FTHandle hFT, Boolean resume);
pascal Boolean 	FTMenu(FTHandle hFT, short menuID, short item);

pascal short 	FTChoose(FTHandle *hFT, Point where, ProcPtr idleProc);
pascal short 	FTPChoose(FTHandle *hFT, Point where, FTChooseRec *cRec);
pascal void 	FTEvent(FTHandle hFT, const EventRecord *theEvent);

pascal Boolean 	FTValidate(FTHandle hFT); 
pascal void 	FTDefault(Ptr *theConfig, short procID, Boolean allocate);

pascal Handle 	FTSetupPreflight(short procID, long *magicCookie);
pascal void 	FTSetupSetup(short procID, const void *theConfig, short count, DialogPtr theDialog,
					long *magicCookie);
pascal Boolean 	FTSetupFilter(short procID, const void *theConfig, short count, DialogPtr theDialog,
					EventRecord *theEvent, short *theItem, long *magicCookie);
pascal void		FTSetupItem(short procID, const void *theConfig, short count, DialogPtr theDialog,
					short *theItem, long *magicCookie);
pascal void 	FTSetupCleanup(short procID, const void *theConfig, short count, DialogPtr theDialog,
					long *magicCookie);
pascal void 	FTSetupXCleanup(short procID, const void *theConfig, short count, DialogPtr theDialog,
					Boolean OKed, long *magicCookie);
pascal void 	FTSetupPostflight(short procID);

pascal Ptr 		FTGetConfig(FTHandle hFT);
pascal short 	FTSetConfig(FTHandle hFT, const void *thePtr);

pascal OSErr 	FTIntlToEnglish(FTHandle hFT, const void *inputPtr, Ptr *outputPtr, short language);
pascal OSErr 	FTEnglishToIntl(FTHandle hFT, const void *inputPtr, Ptr *outputPtr, short language);

pascal void 	FTSetRefCon(FTHandle hFT, long refCon);
pascal long 	FTGetRefCon(FTHandle hFT);

pascal void 	FTSetUserData(FTHandle hFT, long userData);
pascal long 	FTGetUserData(FTHandle hFT);

pascal void 	FTGetToolName(short procID, Str255 name);
pascal short  	FTGetProcID(ConstStr255Param name);

pascal void		FTGetErrorString(FTHandle hFT, short id, Str255 errMsg);
#ifdef __cplusplus
}
#endif	

#endif __FILETRANSFERS__