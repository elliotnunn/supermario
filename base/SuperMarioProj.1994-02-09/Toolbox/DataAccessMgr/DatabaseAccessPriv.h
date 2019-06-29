/*
	File:		DatabaseAccessPriv.h

	Contains:	Private header for the Data Access Manager (codename SnarfMan).

	Written by:	Jeff Miller

	Copyright:	© 1989-1990, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 3/13/92	JSM		Renamed this file DatabaseAccessPriv.h from SnarfMan.h, keeping
									all the old revisions.
		<14>	 7/26/91	stb		remove glue for HOpenResFile. Inline is used now when
									SystemSevenOrLater is defined.
		<13>	 7/15/91	dba		get rid of MEMERR
		<12>	 6/14/91	JL		Changed inclusion of DatabaseAccess to AppleEvents.
		<11>	  4/2/90	JSM		Get rid of resArrayElem (same as ResListElem), add CURMAP, macro
									version of pstrcpy, and glueless HOpenResFile.
		<10>	 3/28/90	JSM		Add RESERR and MEMERR, remove TRUE and FALSE.
		 <9>	 3/13/90	JSM		Update interface to status functions, change endLastRowLen to
									endLastRowInfo in asyncVars.
		 <8>	 3/12/90	JSM		Add support for separate completion routine 'proc', result
									handlers now in 'proc' resource, remove some unneeded typedef
									struct tags.
		 <7>	 2/23/90	JSM		Add support for separate results handlers ('rtt#' and 'rttc'
									resources).
		 <6>	 2/20/90	JSM		Remove refNum from asyncVars.
		 <5>	  2/5/90	JSM		Include Folders.h.
		 <4>	 1/18/90	JSM		Add constants to get strings for results handlers from System
									file.
		 <3>	  1/4/90	JSM		Change some variable names in response to suggestions from
									documentation.
		 <2>	12/20/89	JSM		Update to use public includes.
	   <2.4>	 12/7/89	JSM		Added support for buffering partial rows in DBGetQueryResults.
	   <2.3>	 11/7/89	JSM		Added status functions, keep track of ddevs with vRefNum and
									dirID.
	   <2.2>	 10/6/89	JSM		Updated to make SnarfMan a PACK.
	   <2.1>	 9/18/89	JSM		Changed kStateDone to kAllDone, general cleanup.
	   <2.0>	 8/29/89	JSM		Updated for new version 2 API.
	   <1.9>	  8/7/89	JSM		Added block comments, support for kill and results-to-text.
	   <1.8>	 7/26/89	JSM		Added support for ddev flags.
	   <1.7>	 7/19/89	JSM		Changed 'type' to 'theType' in ResListElem, 'type' to 'dataType'
									elsewhere.
	   <1.6>	 7/15/89	JSM		Async stuff works now.
	   <1.5>	 6/22/89	JSM		Updated async stuff.
	   <1.4>	  6/9/89	JSM		Added more stuff to support async, not quite working yet.
	   <1.3>	 5/31/89	JSM		Reduced fields in globals, change strings in DDEVElem to
									pointers, added constants for offsets in query STR#.
	   <1.2>	 5/18/89	JSM		Added #defines for resource and file types, added version to
									'qrsc' definition, changed ddev and qdef interface.
	   <1.1>	 4/28/89	JSM		Added high-level information.
	   <1.0>	 4/11/89	JSM		Added to EASE for the first time.

*/

/* include Mac header files */
#include	<Types.h>
#include	<stdio.h>
#include	<Resources.h>
#include	<OSUtils.h>
#include	<ToolUtils.h>
#include	<Memory.h>
#include	<Errors.h>
#include	<Files.h>
#include	<Packages.h>
#include	<SANE.h>
#include	<Folders.h>
#include	<AppleEvents.h>		/* Snarf stuff */
#include	<DDEV.h>			/* Snarf stuff */

/* make it easy to call ddev */
typedef pascal OSErr (*DDEVPtr)(DDEVParams *op);

/* make it easy to call qdef */
typedef pascal OSErr (*QDEFPtr)(long *sessID, QueryHandle op);

/* make it easy to call status functions for DBStartQuery and DBGetQueryResults */
typedef pascal Boolean (*statusPtr)(short message, OSErr result, short dataLen,
	short dataPlaces, short dataFlags, DBType dataType, Ptr dataPtr);

/* make it easy to call result handlers */
typedef pascal OSErr (*HandlerPtr)(DBType dataType, short theLen, short thePlaces,
						short theFlags, Ptr theData, Handle theText);

/* macro to zero parameter block */
#define ClearParams(x)	mymemclear((char*)&x, sizeof(DDEVParams))

/* macro to copy pascal strings */
#define pstrcpy(dest, src)			BlockMove(src, dest, src[0]+1)

/* don't use the ResError() and CurResFile() traps */
#define	RESERR			*((short*)0xA60)
#define	CURMAP			*((short*)0xA5A)

#define DDEVTYPE		'ddev'		/* file type of ddev file */
#define QDOCTYPE		'qery'		/* file type of query documents */

#define DDEVRESTYPE		'ddev'		/* resource type of ddev code */
#define DDEVFLAGRESTYPE	'dflg'		/* resource type of ddev flags */
#define QRSCRESTYPE		'qrsc'		/* resource type of query resource */
#define QDEFRESTYPE		'qdef'		/* resource type of query def proc */
#define QUERYRESTYPE	'wstr'		/* resource type of queries */

#define RTTLISTRESTYPE	'rtt#'		/* results to text list */
#define RTTCODERESTYPE	'proc'		/* results to text code */

#define COMPCODERESTYPE	'proc'		/* completion routine code */

/* note - id -5728 is first owned resource for PACK 13 */
#define	RTTLISTID		-5728		/* 'rtt#' specifying default system handlers */
#define HANDLERSSTR		-5728		/* STR# with strings for result handlers */
#define COMPCODERESID	-5727		/* 'proc' resource with completion routine */

/* offsets into STR# for strings used by result handlers */
#define BOOLEANTRUESTR	1			/* typeBoolean - true */
#define BOOLEANFALSESTR	2			/* typeBoolean - false */
#define UNKNOWNSTR		3			/* typeUnknown */
#define COLBREAKSTR		4			/* typeColBreak */
#define ROWBREAKSTR		5			/* typeRowBreak */


/* offsets into STR# for ddevName, host, etc. in query document */
#define DDEVOFFSET		1			/* ddevName */
#define HOSTOFFSET		2			/* host */
#define USEROFFSET		3			/* user */
#define PASSWORDOFFSET	4			/* password */
#define CONNSTROFFSET	5			/* connStr */

/* arbitrary number for indicating an async high level call is done */
/* make sure this doesn't match any of the ddev messages! */
#define kAllDone		200

/************************************************/
/* Structure of package header. 				*/
/* Used by InitDBPack to compare version nums.	*/
/************************************************/
typedef struct {
	short				bra;					/* branch instruction to dispatcher */
	ResType				pack;					/* 'PACK' */
	short				number;					/* id of package */
	short				version;				/* version */
}
	PackHeader,
	*PackHeaderPtr,
	**PackHeaderHndl;

/************************************************/
/* Low memory global with a handle to our PACK.	*/
/* This is App2Packs (0xBC8) + 4 * (the package	*/
/* id - 8) = 0xBC8 + 4 * (13 - 8).				*/
/************************************************/
#define DataAccessMgrPack		*((PackHeaderHndl*)(0xBC8 + 0x14))

/************************************************/
/* Structure for global variables. 				*/
/* Under 7.0, a pointer to this structure is 	*/
/* stored in the emSnarfGlobals low mem global.	*/
/************************************************/
typedef struct {
	struct ddevElem			**ddevRoot;			/* root of ddev list */
	struct snarfSessElem	**snarfRoot;		/* root of snarf ID list */
	DBAsyncParmBlkPtr		pbRoot;				/* list of queued parameter blocks */
	short					numHandlers;		/* number of handlers in array */
	struct HandlerRecord	**handlers;			/* handle to array of result handlers */
	Handle					handlerRes;			/* handle to 'proc' for result handlers */
	Handle					completionRes;		/* handle to 'proc' for completion routine */
}
	snarfGlob,
	*snarfGlobPtr,
	**snarfGlobHndl;

/************************************************/
/* Structure to keep track of ddevs. 			*/
/* A list of handles to these structures is 	*/
/* hung off ddevRoot in the globals.			*/
/************************************************/
typedef struct ddevElem {
	struct ddevElem		**next;				/* next element in list */
	Boolean				exists;				/* if ddev file deleted */
	Boolean				canAsync;			/* true if can do async calls */
	char				*name;				/* ddev name in STR resource */
	char				*fname;				/* ddev file name */
	short				vRefNum;			/* where located (System Folder) */
	long				dirID;				/* dirID where located */
	DDEVPtr				*ddev;				/* handle to (detached) ddev resource */
	long				ddevStorage;		/* ddev private storage */
	struct sessElem		**sessRoot;			/* list of active sessions */
}
	ddevElem,
	*ddevElemPtr,
	**ddevElemHndl;

/************************************************/
/* Structure to keep track of sessions for an	*/
/* individual ddev.  A list of handles to these	*/
/* structures is hung off sessRoot in each		*/
/* ddevElem.									*/
/************************************************/
typedef struct sessElem {
	struct sessElem		**next;				/* next element in list */
	struct ddevElem		**parent;			/* ddev who owns session */
	long				sessID;				/* ID as seen by ddev */
}
	sessElem,
	*sessElemPtr,
	**sessElemHndl;

/************************************************/
/* Structure to keep track of snarf IDs.		*/
/* A snarf ID is a unique identifier for 		*/
/* sessions across all ddevs.  A list of		*/
/* handles to these structures is hung off		*/
/* snarfRoot in the globals.					*/
/************************************************/
typedef struct snarfSessElem {
	struct snarfSessElem	**next;				/* next element in list */
	long					snarfID;			/* ID as seen by app */
	long					processID;			/* parent process - not used yet! */
	struct sessElem			**sess;				/* handle to session element */
	ResultsRecord			*oldResults;		/* leftover results for DBGetQueryResults */
	short					currCol;			/* number of cols in leftover results */
}
	snarfSessElem,
	*snarfSessElemPtr,
	**snarfSessElemHndl;

/************************************************/
/* Partial structure of 'qrsc'.  Since there	*/
/* are variable length arrays of 'wstr' IDs and	*/
/* resources, it only goes up to the start of	*/
/* the 'wstr' array.							*/
/************************************************/
typedef struct {
	short					version;			/* version */
	short					qdefID;				/* QDEF ID */
	short					strID;				/* STR# with ddevName etc. */
	short					currQuery;			/* current query */

	short					numQueries;			/* number of queries that follow */
	short					queryArray[];		/* array of 'wstr' IDs */

/*	After the variable 'wstr' array, it looks like (see resArrayList below):
	short					numRes;
	resArrayElem			res[numRes];
*/
}
	queryRes,
	*queryResPtr,
	**queryResHndl;

/************************************************/
/* Structure of list of resource elements in a	*/
/* 'qrsc'.  See queryRes structure above.		*/
/************************************************/
typedef struct {
	short					numRes;			/* number of elements that follow */
	ResListElem				res[];			/* array of resource elements - same as in QueryRecord */
}
	resArrayList,
	*resArrayListPtr;

/************************************************/
/* Structure for async vars when making async	*/
/* calls to Data Access Mgr routines.  A pointer*/
/* to this structure is hung off the reserved	*/
/* field in the DBAsyncParamBlockRec.  Make 	*/
/* sure this matches definition in				*/
/* DataAccessMgr.a.								*/
/************************************************/
typedef struct {
	DBAsyncParmBlkPtr		next;			/* next pb in list */
	DBAsyncParmBlkPtr		asyncPB;		/* async pb passed in to routine */
	DDEVParamsPtr			params;			/* pointer to ddev parameters */
	short					currAction;		/* current action - selector or kAllDone */
	Boolean					executing;		/* true if call executing */
	Boolean					highlevel;		/* true if this is a highlevel call */
	ddevElemHndl			ddevH;			/* handle to ddev - need to keep for kill */
	statusPtr				statusProc;		/* status function for high level stuff */

	/* variables for DBInit */
	Boolean					firstsess;		/* true if first session for ddev */
	Boolean					filler;
	long					*sessIDPtr;		/* pointer to session ID */

	/* variables for DBEnd */
	snarfSessElemHndl		snarf;			/* handle to snarf session cell */
											/* also used by DBGetQueryResults */

	/* variables for DBGetConnInfo */
	long					*returnedIDPtr;	/* pointer to returned sess ID */
	long					*versionPtr;	/* pointer to version */
	long					*startPtr;		/* pointer to start time */
	OSErr					*statePtr;		/* pointer to state */

	/* variables for DBGetSessionNum */
	short					*sessNumPtr;	/* pointer to returned sess number */

	/* variables for DBGetErr */
	long					*err1Ptr;		/* pointer to error 1 */
	long					*err2Ptr;		/* pointer to error 2 */

	/* variables for DBGetItem */
	DBType					*dataTypePtr;	/* pointer to type of data */
	short					*lenPtr;		/* pointer to length of data */
	short					*placesPtr;		/* pointer to places */
	short					*flagsPtr;		/* pointer to flags */

	/* variables for DBStartQuery */
	QueryHandle				query;			/* query for DBSend */
	Handle					queryText;		/* text for DBSend */
	char					queryState;		/* state of query handle */
	char					textState;		/* state of text handle */

	/* variables for DBGetQueryResults */
	long					sessID;			/* session ID */
	ResultsRecord			*results;		/* to hold results */
	ResultsRecord			*oldResults;	/* old results stored in snarf elem */
	DBType					dataType;		/* data type of current column */
	long					timeout;		/* timeout for DBGetItem */
	short					len, places, flags;	/* stuff for current column */
	long					currOffset;		/* current offset into data */
	long					oldCurrOffset;	/* current offset into old data */
	long					endLastRowData;	/* end of last row (data) */
	long					endLastRowInfo;	/* end of last row (info) */
	short					currCol;		/* current column */
	Boolean					firsttime;		/* true if getting first column */
	Boolean					doingGetItemInfo; /* true if DBGetItem for info only */
}
	asyncVars,
	*asyncVarsPtr;

/************************************************/
/* Structure to keep track of result handlers.	*/
/* A handle to a dynamic array of these records	*/
/* is kept in the handlers field of the globals.*/
/* The array is sorted based on type.			*/
/************************************************/
typedef struct HandlerRecord {
	DBType					type;			/* what data type this handler is for */
	HandlerPtr				theHandler;		/* pointer to actual procedure */
	struct HandlerRecord	**next;			/* for multiple handlers for one type */
	THz						hZone;			/* current zone when InstallHandler called */
	Boolean					isSysHandler;	/* TRUE if system handler */
	char					filler;
}
	HandlerRecord,
	*HandlerRecordPtr,
	**HandlerRecordHandle;

