/************************************************************

Created: Tuesday, March 21, 1989 at 9:39 AM
	DatabaseAccess.h
	C Interface to the Macintosh Libraries


	Copyright Apple Computer, Inc.	 1989-1991
	All rights reserved

************************************************************/

#ifndef __DATABASEACCESS__
#define __DATABASEACCESS__

#ifndef __RESOURCES__
#include <Resources.h>
#endif

/* OSErr error and status codes */
enum {rcDBNull = -800};
enum {rcDBValue = -801};
enum {rcDBError = -802};
enum {rcDBBadType = -803};
enum {rcDBBreak = -804};
enum {rcDBExec = -805};
enum {rcDBBadSessID = -806};
enum {rcDBBadSessNum = -807};       /* bad session number for DBGetConnInfo */

enum {rcDBBadDDEV = -808};          /* bad ddev specified on DBInit */

enum {rcDBAsyncNotSupp = -809};     /* ddev does not support async calls */
enum {rcDBBadAsyncPB = -810};       /* tried to kill a bad pb */

enum {rcDBNoHandler = -811};        /* no app handler for specified data type */

enum {rcDBWrongVersion = -812};     /* incompatible versions */
enum {rcDBPackNotInited = -813};    /* attempt to call other routine before InitDBPack */

/*  messages for status functions for DBStartQuery  */
enum {kDBUpdateWind = 0};
enum {kDBAboutToInit = 1};
enum {kDBInitComplete = 2};
enum {kDBSendComplete = 3};
enum {kDBExecComplete = 4};
enum {kDBStartQueryComplete = 5};

/*  messages for status functions for DBGetQueryResults  */
enum {kDBGetItemComplete = 6};
enum {kDBGetQueryResultsComplete = 7};

/* data type codes */
typedef OSType DBType;

#define 	typeDate		'date'
#define 	typeTime		'time'
#define 	typeTimeStamp	'tims'
#define 	typeChar		'TEXT'
#define 	typeDecimal 	'deci'
#define 	typeMoney		'mone'
#define 	typeVChar		'vcha'
#define 	typeVBin		'vbin'
#define 	typeLChar		'lcha'
#define 	typeLBin		'lbin'
#define 	typeDiscard 	'disc'

/* "dummy" types for DBResultsToText */
#define 	typeUnknown 	'unkn'
#define 	typeColBreak	'colb'
#define 	typeRowBreak	'rowb'

/* pass this in to DBGetItem for any data type */
#define 	typeAnyType 	(DBType)0

/* infinite timeout value for DBGetItem */
enum {kDBWaitForever = -1};

/*  flags for DBGetItem  */
enum {kDBLastColFlag = 0x0001};
enum {kDBNullFlag = 0x0004};

/* structure for asynchronous parameter block */
struct DBAsyncParamBlockRec {
	ProcPtr 	completionProc; /* pointer to completion routine */
	OSErr		result; 		/* result of call */
	long		userRef;		/* for application's use */
	long		ddevRef;		/* for ddev's use */
	long		reserved;		/* for internal use */
};
typedef struct DBAsyncParamBlockRec DBAsyncParamBlockRec;
typedef DBAsyncParamBlockRec *DBAsyncParmBlkPtr;

/* structure for resource list in QueryRecord */

struct ResListElem {
	ResType 	theType;		/* resource type */
	short		id; 			/* resource id */
};
typedef struct ResListElem ResListElem;
typedef ResListElem *ResListPtr,**ResListHandle;

/* structure for query list in QueryRecord */

typedef Handle **QueryListHandle;	/* just handles to 'wstr' resources */

struct QueryRecord {
	short			version;		/* version */
	short			id; 			/* id of 'qrsc' this came from */
	Handle			queryProc;		/* handle to query def proc */
	Str63			ddevName;		/* ddev name */
	Str255			host;			/* host */
	Str255			user;			/* user */
	Str255			password;		/* password */
	Str255			connStr;		/* connection string */
	short			currQuery;		/* current query */
	short			numQueries; 	/* number of queries in queryList */
	QueryListHandle queryList;		/* handle to list of queries */
	short			numRes; 		/* number of resources in resList */
	ResListHandle	resList;		/* handle to list of other resources */
	Handle			dataHandle; 	/* data used by query def proc */
	long			refCon; 		/* query's reference value */
};
typedef struct QueryRecord QueryRecord;
typedef QueryRecord *QueryPtr,**QueryHandle;

/* structure for column info in ResultsRecord */
struct DBColInfoRecord {
	short			len;
	short			places;
	short			flags;
};
typedef struct DBColInfoRecord DBColInfoRecord;

/* structure of results returned by DBGetResults */
struct ResultsRecord {
	short			numRows;		/* number of rows in result */
	short			numCols;		/* number of columns per row */
	Handle			colTypes;		/* data type array */
	Handle			colData;		/* actual results */
	Handle			colInfo;		/* DBColInfoRecord array */
};
typedef struct ResultsRecord ResultsRecord;

#ifdef __cplusplus
extern "C" {
#endif

pascal OSErr InitDBPack(void)
	= {0x3F3C, 0x0004, 0x303C, 0x0100, 0xA82F};
pascal OSErr DBInit(long *sessID, ConstStr63Param ddevName, ConstStr255Param host,
	ConstStr255Param user, ConstStr255Param passwd, ConstStr255Param connStr,
	DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0E02, 0xA82F};
pascal OSErr DBEnd(long sessID, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0403, 0xA82F};
pascal OSErr DBGetConnInfo(long sessID, short sessNum, long *returnedID,
	long *version, Str63 ddevName, Str255 host, Str255 user, Str255 network,
	Str255 connStr, long *start, OSErr *state, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x1704, 0xA82F};
pascal OSErr DBGetSessionNum(long sessID, short *sessNum, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0605, 0xA82F};
pascal OSErr DBSend(long sessID, char *text, short len, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0706, 0xA82F};
pascal OSErr DBSendItem(long sessID, DBType dataType, short len, short places,
	short flags, void *buffer, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0B07, 0xA82F};
pascal OSErr DBExec(long sessID, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0408, 0xA82F};
pascal OSErr DBState(long sessID, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0409, 0xA82F};
pascal OSErr DBGetErr(long sessID, long *err1, long *err2, Str255 item1,
	Str255 item2, Str255 errorMsg, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0E0A, 0xA82F};
pascal OSErr DBBreak(long sessID, Boolean abort, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x050B, 0xA82F};
pascal OSErr DBGetItem(long sessID, long timeout, DBType *dataType, short *len,
	short *places, short *flags, void *buffer, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x100C, 0xA82F};
pascal OSErr DBUnGetItem(long sessID, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x040D, 0xA82F};
pascal OSErr DBKill(DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x020E, 0xA82F};
pascal OSErr DBGetNewQuery(short queryID, QueryHandle *query)
	= {0x303C, 0x030F, 0xA82F};
pascal OSErr DBDisposeQuery(QueryHandle query)
	= {0x303C, 0x0210, 0xA82F};
pascal OSErr DBStartQuery(long *sessID, QueryHandle query, ProcPtr statusProc, 
	DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0811, 0xA82F};
pascal OSErr DBGetQueryResults(long sessID, ResultsRecord *results,
	long timeout, ProcPtr statusProc, DBAsyncParmBlkPtr asyncPB)
	= {0x303C, 0x0A12, 0xA82F};
pascal OSErr DBResultsToText(ResultsRecord *results, Handle *theText)
	= {0x303C, 0x0413, 0xA82F};
pascal OSErr DBInstallResultHandler(DBType dataType, ProcPtr theHandler, 
	Boolean isSysHandler)
	= {0x303C, 0x0514, 0xA82F};
pascal OSErr DBRemoveResultHandler(DBType dataType)
	= {0x303C, 0x0215, 0xA82F};
pascal OSErr DBGetResultHandler(DBType dataType, ProcPtr *theHandler, 
	Boolean getSysHandler)
	= {0x303C, 0x0516, 0xA82F};

#ifdef __cplusplus
}
#endif


#endif
