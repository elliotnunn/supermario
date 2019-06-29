/*
	File:		ConnectionTools.h

	Contains:	C Interface to the Connection Manager Tools

	Copyright:	© 1988-1990 by Apple Computer, Inc.
				All rights reserved.

*/

#ifndef __CONNECTIONTOOLS__
#define __CONNECTIONTOOLS__

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif	__DIALOGS__

#ifndef __CONNECTIONS__
#include <Connections.h>
#endif	__CONNECTIONS__

/* messages for DefProc */

#define cmInitMsg 			0
#define cmDisposeMsg 		1
#define cmSuspendMsg 		2
#define cmResumeMsg 		3 
#define cmMenuMsg 			4 
#define	cmEventMsg			5
#define cmActivateMsg 		6
#define cmDeactivateMsg 	7

#define cmIdleMsg 			50
#define	cmResetMsg 			51
#define cmAbortMsg 			52

#define cmReadMsg 			100
#define cmWriteMsg 			101
#define cmStatusMsg 		102
#define cmListenMsg 		103
#define cmAcceptMsg 		104
#define cmCloseMsg 			105
#define cmOpenMsg 			106
#define cmBreakMsg 			107
#define	cmIOKillMsg			108
#define	cmEnvironsMsg 		109

/*	messages for validate DefProc	*/

#define cmValidateMsg	0
#define cmDefaultMsg	1
	
	
/*	messages for Setup DefProc	*/

#define cmSpreflightMsg	0
#define cmSsetupMsg		1
#define cmSitemMsg		2
#define cmSfilterMsg	3
#define cmScleanupMsg	4
	
	
/*	messages for scripting defProc	*/

#define cmMgetMsg		0
#define cmMsetMsg		1
	

/*	messages for localization defProc	*/

#define cmL2English		0
#define cmL2Intl		1


/*	private data constants */

#define cdefType	'cdef'		/* main connection definition procedure	*/
#define cvalType	'cval'		/* validation definition procedure	*/
#define csetType	'cset'		/* connection setup definition procedure	*/
#define clocType	'cloc'		/* connection configuration localization defProc	*/
#define cscrType	'cscr'		/* connection scripting defProc interfaces	*/

#define cbndType	'cbnd'		/* bundle type for connection	*/
#define cverType	'vers'	


struct CMCompletorRecord {
	Boolean	async;
	ProcPtr	completionRoutine;
};

typedef struct CMCompletorRecord CMCompletorRecord;

typedef CMCompletorRecord *CMCompletorPtr;


struct CMSetupStruct {
	DialogPtr theDialog;
	short count;
	Ptr theConfig;
	short	procID;
};

typedef struct CMSetupStruct CMSetupStruct;

typedef CMSetupStruct *CMSetupPtr;


struct CMDataBuffer {
	Ptr		thePtr;
	long	count;
	CMChannel channel;
	CMFlags	flags;
};
typedef struct CMDataBuffer CMDataBuffer;

typedef CMDataBuffer *CMDataBufferPtr;

#endif __CONNECTIONTOOLS__