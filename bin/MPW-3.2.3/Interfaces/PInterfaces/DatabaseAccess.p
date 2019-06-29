{

Created: Monday, March 20, 1989 at 9:06 AM
	DatabaseAccess.p
	Pascal Interface to the Macintosh Libraries

	Copyright Apple Computer, Inc.	 1989-1991
	All rights reserved

}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT DatabaseAccess;
	INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingDatabaseAccess}
{$SETC UsingDatabaseAccess := 1}

{$I+}
{$SETC DatabaseAccessIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingResources}
{$I $$Shell(PInterfaces)Resources.p}
{$ENDC}
{$SETC UsingIncludes := DatabaseAccessIncludes}


CONST

{ error and status codes }
rcDBNull = -800;
rcDBValue = -801;
rcDBError = -802;
rcDBBadType = -803;
rcDBBreak = -804;
rcDBExec = -805;
rcDBBadSessID = -806;
rcDBBadSessNum = -807;		{ bad session number for DBGetConnInfo }

rcDBBadDDEV = -808; 		{ bad ddev specified on DBInit }

rcDBAsyncNotSupp = -809;	{ ddev does not support async calls }
rcDBBadAsyncPB = -810;		{ tried to kill a bad pb }

rcDBNoHandler = -811;		{ no app handler for specified data type }

rcDBWrongVersion = -812;	{ incompatible versions }
rcDBPackNotInited = -813;	{ attempt to call other routine before InitDBPack }

{ messages for status functions for DBStartQuery }
kDBUpdateWind	= 0;
kDBAboutToInit	= 1;
kDBInitComplete = 2;
kDBSendComplete = 3;
kDBExecComplete = 4;
kDBStartQueryComplete = 5;

{ messages for status functions for DBGetQueryResults }
kDBGetItemComplete	= 6;
kDBGetQueryResultsComplete = 7;

typeNone		= 'none';
typeDate		= 'date';
typeTime		= 'time';
typeTimeStamp	= 'tims';
typeDecimal 	= 'deci';
typeMoney		= 'mone';
typeVChar		= 'vcha';
typeVBin		= 'vbin';
typeLChar		= 'lcha';
typeLBin		= 'lbin';
typeDiscard 	= 'disc';

{ "dummy" types for DBResultsToText }
typeUnknown 	= 'unkn';
typeColBreak	= 'colb';
typeRowBreak	= 'rowb';

{ pass this in to DBGetItem for any data type }
typeAnyType 	= 0;

{ infinite timeout value for DBGetItem }
kDBWaitForever	= -1;

{ flags for DBGetItem }
kDBLastColFlag	= $0001;
kDBNullFlag		= $0004;

TYPE

DBType = OSType;

{ structure for asynchronous parameter block }
DBAsyncParamBlockRec = RECORD
	completionProc: 	ProcPtr;	{ pointer to completion routine }
	result: 			OSErr;		{ result of call }
	userRef:			LONGINT;	{ for application's use }
	ddevRef:			LONGINT;	{ for ddev's use }
	reserved:			LONGINT;	{ for internal use }
	END;

DBAsyncParmBlkPtr = ^DBAsyncParamBlockRec;

{ structure for resource list in QueryRecord }

ResListElem = RECORD
	theType:	ResType;
	id: 		INTEGER;
	END;

ResListPtr = ^ResListArray;
ResListHandle = ^ResListPtr;
ResListArray = ARRAY [0..255] OF ResListElem;

{ structure for query list in QueryRecord }

QueryListPtr = ^QueryArray;
QueryListHandle = ^QueryListPtr;
QueryArray = ARRAY [0..255] OF Handle;

QueryPtr = ^QueryRecord;
QueryHandle = ^QueryPtr;
QueryRecord = RECORD
	version:		INTEGER;			{ version }
	id: 			INTEGER;			{ id of 'qrsc' this came from }
	queryProc:		Handle; 			{ handle to query def proc }
	ddevName:		Str63;				{ ddev name }
	host:			Str255; 			{ host name }
	user:			Str255; 			{ user name }
	password:		Str255; 			{ password }
	connStr:		Str255; 			{ connection string }
	currQuery:		INTEGER;			{ index of current query }
	numQueries: 	INTEGER;			{ number of queries in list }
	queryList:		QueryListHandle;	{ handle to array of handles to text }
	numRes: 		INTEGER;			{ number of resources in list }
	resList:		ResListHandle;		{ handle to array of resource list elements }
	dataHandle: 	Handle; 			{ for use by query def proc }
	refCon: 		LONGINT;			{ for use by application }
	END;

{ structure of column types array in ResultsRecord }

ColTypesPtr = ^ColTypesArray;
ColTypesHandle = ^ColTypesPtr;
ColTypesArray = ARRAY [0..255] OF DBType;

{ structure for column info in ResultsRecord }

DBColInfoRecord = RECORD
	len:			INTEGER;
	places:			INTEGER;
	flags:			INTEGER;
	END;

ColInfoPtr = ^ColInfoArray;
ColInfoHandle = ^ColInfoPtr;
ColInfoArray = ARRAY [0..255] OF DBColInfoRecord;

ResultsRecord = RECORD
	numRows:		INTEGER;			{ number of rows in result }
	numCols:		INTEGER;			{ number of columns per row }
	colTypes:		ColTypesHandle; 	{ data type array }
	colData:		Handle; 			{ actual results }
	colInfo:		ColInfoHandle; 		{ DBColInfoRecord array }
	END;

FUNCTION InitDBPack : OSErr;
	INLINE $3F3C,$0004,$303C,$0100,$A82F;

FUNCTION DBInit(VAR sessID : LONGINT; ddevName : Str63; host, user, passwd,
		connStr : Str255; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0E02,$A82F;

FUNCTION DBEnd(sessID : LONGINT; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0403,$A82F;

FUNCTION DBGetConnInfo(sessID : LONGINT; sessNum : INTEGER;
		VAR returnedID, version : LONGINT; VAR ddevName : Str63;
		VAR host, user, network, connStr : Str255; VAR start : LONGINT;
		VAR state : OSErr; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$1704,$A82F;

FUNCTION DBGetSessionNum(sessID : LONGINT; VAR sessNum : INTEGER;
		asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0605,$A82F;

FUNCTION DBSend(sessID : LONGINT; text : Ptr; len : INTEGER;
		asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0706,$A82F;

FUNCTION DBSendItem(sessID : LONGINT; dataType : DBType;
		len, places, flags : INTEGER; buffer : Ptr; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0B07,$A82F;

FUNCTION DBExec(sessID : LONGINT; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0408,$A82F;

FUNCTION DBState(sessID : LONGINT; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0409,$A82F;

FUNCTION DBGetErr(sessID : LONGINT; VAR err1, err2 : LONGINT; VAR item1, item2,
		errorMsg : Str255; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0E0A,$A82F;

FUNCTION DBBreak(sessID : LONGINT; abort : BOOLEAN; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$050B,$A82F;

FUNCTION DBGetItem(sessID, timeout : LONGINT; VAR dataType : DBType;
		VAR len, places, flags : INTEGER; buffer : Ptr; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$100C,$A82F;

FUNCTION DBUnGetItem(sessID : LONGINT; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$040D,$A82F;

FUNCTION DBKill(asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$020E,$A82F;

FUNCTION DBGetNewQuery(queryID : INTEGER; VAR query : QueryHandle) : OSErr;
	INLINE $303C,$030F,$A82F;

FUNCTION DBDisposeQuery(query : QueryHandle) : OSErr;
	INLINE $303C,$0210,$A82F;

FUNCTION DBStartQuery(VAR sessID : LONGINT; query : QueryHandle;
		statusProc : ProcPtr; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0811,$A82F;

FUNCTION DBGetQueryResults(sessID : LONGINT; VAR results : ResultsRecord;
		timeout : LONGINT; statusProc : ProcPtr; asyncPB : DBAsyncParmBlkPtr) : OSErr;
	INLINE $303C,$0A12,$A82F;

FUNCTION DBResultsToText(results : ResultsRecord; VAR theText : Handle) : OSErr;
	INLINE $303C,$0413,$A82F;

FUNCTION DBInstallResultHandler(dataType : DBType; theHandler : ProcPtr;
		isSysHandler : BOOLEAN) : OSErr;
	INLINE $303C,$0514,$A82F;

FUNCTION DBRemoveResultHandler(dataType : DBType) : OSErr;
	INLINE $303C,$0215,$A82F;

FUNCTION DBGetResultHandler(dataType : DBType; VAR theHandler : ProcPtr;
		getSysHandler : BOOLEAN) : OSErr;
	INLINE $303C,$0516,$A82F;

{$ENDC}    { UsingDatabaseAccess }

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

