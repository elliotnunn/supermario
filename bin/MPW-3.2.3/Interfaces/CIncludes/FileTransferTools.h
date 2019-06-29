/*
	File:		FileTransferTools.h

	Contains:	C Interface to the File Transfer Manager Tools

	Copyright:	© 1988-1990 by Apple Computer, Inc.
				All rights reserved.

*/

#ifndef __FILETRANSFERTOOLS__
#define __FILETRANSFERTOOLS__

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif

#ifndef __FILETRANSFERS__
#include <FileTransfers.h>
#endif	__FILETRANSFERS__

/* messages */
/* Control */
#define		ftInitMsg		0
#define		ftDisposeMsg	1
#define		ftSuspendMsg	2
#define		ftResumeMsg		3
#define		ftMenuMsg		4
#define		ftEventMsg		5
#define 	ftActivateMsg 	6
#define 	ftDeactivateMsg 7
#define		ftGetErrorStringMsg 8

#define		ftAbortMsg		52

#define		ftStartMsg		100
#define		ftExecMsg		102
 
/*Setup */
#define		ftSpreflightMsg		0
#define		ftSsetupMsg			1
#define		ftSitemMsg			2
#define		ftSfilterMsg		3
#define		ftScleanupMsg		4

/* validate */
#define		ftValidateMsg		0
#define		ftDefaultMsg		1

/* scripting */
#define		ftMgetMsg			0
#define		ftMsetMsg			1

/* localization */
#define		ftL2English			0
#define		ftL2Intl			1

/* DEFs */
#define		fdefType		'fdef'
#define		fsetType		'fset'
#define		fvalType		'fval'
#define		flocType		'floc'
#define		fscrType		'fscr'

#define		fbndType		'fbnd'
#define		fverType		'vers'


struct FTSetupStruct
{
	DialogPtr	theDialog;			/* the dialog form the application */
	short		count;				/* first appended item */
	Ptr			theConfig;			/* the config record to setup */
	short		procID;				/* procID of the tool */
};

typedef struct FTSetupStruct FTSetupStruct;

typedef FTSetupStruct *FTSetupPtr;	
	
#endif __FILETRANSFERTOOLS__