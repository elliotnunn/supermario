/*
	File:		DataAccessMgr.c

	Contains:	C routines for the Data Access Manager (codename SnarfMan).
				These internal routines have parallel names to the public
				routines to avoid name conflicts (e.g. DBInit is SMInit).

	Written by:	Jeff Miller

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM2>	11/19/92	RB		Set ROMMapInsert to MapTrue just before doing some GetResource
									calls so that we look in ROM first.
		<30>	 3/13/92	JSM		Renamed this file DataAccessMgr.c from SnarfMan.c, keeping all
									the old revisions.
		<29>	 7/15/91	dba		get rid of MEMERR
		<28>	 6/12/91	JSM		Completely delete about box code, which hasn't been used since
									revision 19 anyway.
		<27>	10/21/90	gbm		Changing .hfileInfo to .hFileInfo again to match interfaces.
		<27>	 10/2/90	JAL		Changed .hfileInfo to .hFileInfo to match interfaces.
		<26>	 9/12/90	JSM		Fix bug when DBGetQueryResults is called async and DBState
									returns noErr.
		<25>	 7/22/90	JSM		rcDBNull is a valid return code to get from DBGetItem in
									DBGetQueryResults, don't return an error in this case.
		<24>	  6/4/90	DC		changed divide by 2 in FindHandler to right shift 1.
		<23>	 4/25/90	JSM		Remove SMQuitDBPack, since it's no longer implemented.
		<22>	 4/24/90	JSM		More code size reduction: Add CallStatusProc (common code for
									most status function calls), calculate resListSize once in
									SMGetNewQuery.
		<21>	 4/23/90	JSM		Save and restore current GrafPort across call to qdef in
									SMStartQuery.
		<20>	 4/10/90	JSM		Make DBGetNewQuery return more errors, don't explicitly return
									resNotFound unless ResErr is noErr, change back to .hfileInfo
									(thanks, Dave).
		<19>	  4/2/90	JSM		Even more code size reduction:  remove about box and other
									changes far too numerous to mention.
		<18>	 3/28/90	JSM		Code size reduction:  read ResErr and MemErr directly from
									lomem.
		<17>	 3/13/90	JSM		Change names of ResultsHandler routines to ResultHandler, add
									support for new status function and result handler interface,
									add support for new ResultsRecord format.
		<16>	 3/12/90	JSM		Add support for completion routine in a separate resource.
		<15>	  3/9/90	JAL		Changed .hfileInfo to .hFileInfo to match Files.h change.
		<14>	 2/26/90	JSM		Don't HLock and HNoPurge package, now handled by PACKHelper.
		<13>	 2/23/90	JSM		Add support for results handlers in separate resource.
		<12>	 2/22/90	JSM		Add kDBStartQueryComplete, fix bug where AllDoneProc was never
									cleaning up.
		<11>	 2/20/90	JSM		Don't allow NULL query on DBStartQuery.
		<10>	  2/6/90	JSM		Change rcDBCancel and rcDBStatusCancel to userCanceledErr, add
									support for new status function message
									kDBGetQueryResultsComplete.
		 <9>	  2/5/90	JSM		Look for ddevs in Extensions Folder.
		 <8>	 1/31/90	JSM		Fix return code bug in DBGetConnInfo.
		 <7>	 1/18/90	JSM		Use STR# from System file in default results handlers.
		 <6>	  1/5/90	JSM		Change default results handler for text to strip off trailing
									spaces.
		 <5>	  1/4/90	JSM		Change some variable names in response to suggestions from
									documentation.
		 <4>	12/28/89	dba		Use quotes around the SnarfMan.h header file name, since the
									file is a local header rather than part of public Interfaces.
		 <3>	12/22/89	JSM		Use kDBLastColFlag in DBGetQueryResults.
		 <2>	12/20/89	JSM		Cleanup for new build system.
	   <2.9>	 12/7/89	JSM		Added support for buffering partial rows in DBGetQueryResults.
	   <2.8>	11/21/89	JSM		Set queryList and resList handles to NIL instead of empty
									handles in DBGetNewQuery if no queries or resources in 'qrsc'.
	   <2.7>	11/15/89	JSM		Always return sessID from ddev on DBInit, fix bug where calling
									DBGetQueryResults async when no results causes problems.
	   <2.6>	 11/7/89	JSM		Added status functions, changed DBUnGetVal to DBUnGetItem, keep
									track of ddev folder with vRefNum and dirID.
	   <2.5>	 11/6/89	JSM		Fixed bug with return codes from DBGetConnInfo.
	   <2.4>	 11/1/89	JSM		Fixed error handling for GetResource calls.
	   <2.3>	10/13/89	JSM		Removed hack in DBGetQueryResults processing that worked around
									a bug in CL/1 5.022.
	   <2.2>	 10/6/89	JSM		Updated to make SnarfMan a PACK.
	   <2.1>	 9/18/89	JSM		General cleanup and optimizing, fixed potential problem if null
									parameters were passed in on some calls.
	   <2.0>	 8/29/89	JSM		Updated for new version 2 API.
	   <1.9>	  8/7/89	JSM		Added support for colLens in ResultsRecord, and kill and
									results-to-text stuff (not quite done yet).
	   <1.8>	 7/26/89	JSM		Added support for ddev flags.
	   <1.7>	 7/19/89	JSM		Changed 'type' to 'dataType' in several places.
	   <1.6>	 7/15/89	JSM		Async stuff works now.
	   <1.5>	 6/22/89	JSM		Don't build ddev list at patch time, updated SMGetRow, messed
									around with async some more.
	   <1.4>	  6/9/89	JSM		Added more stuff to support async, not quite working yet.
	   <1.3>	 5/31/89	JSM		No longer checks for rcDBReady, doesn't return rcDBSize, added
									async support.
	   <1.2>	 5/18/89	JSM		Now return OSErr for everything, plus numerous other changes.
	   <1.1>	 4/28/89	JSM		Added lots o' new stuff, mostly high-level calls.
	   <1.0>	 4/11/89	JSM		Added to EASE for the first time.

*/

/* include our private header */
#include	"DatabaseAccessPriv.h"
#include	<SysEqu.h>

// #define DODPRINTF

/* I M P O R T S */
snarfGlobPtr	GetGlobals();
snarfGlobPtr*	GetGlobalsPtr();
void			mymemclear(char *s, long size);
void			CallCompletion(DBAsyncParmBlkPtr asyncPB);

/* E X P O R T S */
pascal OSErr	SMInitDBPack(short version);
OSErr			InstallDefaultHandlers(snarfGlobPtr	glob);
pascal OSErr	SMIdle();

OSErr			SMOpen(ddevElemHndl ddevH);
OSErr			SMClose(ddevElemHndl ddevH);

pascal OSErr	SMInit(long *sessID, const Str63 ddevName, const Str255 host,
					const Str255 user, const Str255 password, const Str255 connStr,
					DBAsyncParmBlkPtr asyncPB);
pascal void		SMInitComplete(ddevElemHndl ddevH, long *sessIDPtr,
					long sessID, Boolean firstsess);

pascal OSErr	SMEnd(long sessID, DBAsyncParmBlkPtr asyncPB);
OSErr			SMEndComplete(snarfSessElemHndl snarf);

pascal OSErr	SMGetConnInfo(long sessID, short sessNum, long *returnedID,
					long *version, Str63 ddevName, Str255 host, Str255 user,
					Str255 network, Str255 connStr, long *start, OSErr *state,
					DBAsyncParmBlkPtr asyncPB);
pascal OSErr	SMGetSessionNum(long sessID, short *sessNum, DBAsyncParmBlkPtr asyncPB);

pascal OSErr	SMSend(long sessID, char *text, short len, DBAsyncParmBlkPtr asyncPB);
pascal OSErr	SMSendItem(long sessID, DBType dataType, short len, short places,
					short flags, Ptr buffer, DBAsyncParmBlkPtr asyncPB);
pascal OSErr	SMExec(long sessID, DBAsyncParmBlkPtr asyncPB);
pascal OSErr	SMState(long sessID, DBAsyncParmBlkPtr asyncPB);
pascal OSErr	SMGetErr(long sessID, long *err1, long *err2, Str255 item1,
					Str255 item2, Str255 errorMsg, DBAsyncParmBlkPtr asyncPB);
pascal OSErr	SMBreak(long sessID, Boolean abort, DBAsyncParmBlkPtr asyncPB);

pascal OSErr	SMGetItem(long sessID, long timeout, DBType *dataType, short *len,
					short *places, short *flags, Ptr buffer, DBAsyncParmBlkPtr asyncPB);
pascal OSErr	SMUnGetItem(long sessID, DBAsyncParmBlkPtr asyncPB);

pascal OSErr 	SMGetNewQuery(short queryID, QueryHandle *query);
pascal OSErr	SMDisposeQuery(QueryHandle query);
pascal OSErr	SMStartQuery(long *sessID, QueryHandle query, statusPtr statusProc,
					DBAsyncParmBlkPtr asyncPB);
pascal OSErr	SMGetQueryResults(long sessID, ResultsRecord *results,
					long timeout, statusPtr statusProc, DBAsyncParmBlkPtr asyncPB);

pascal OSErr	SMKill(DBAsyncParmBlkPtr asyncPB);

pascal OSErr	SMResultsToText(ResultsRecord *results, Handle *theText);
HandlerPtr		GetCorrectResultHandler(DBType dataType);
pascal OSErr	SMInstallResultHandler(DBType dataType, ProcPtr theHandler,
					Boolean isSysHandler);
pascal OSErr	SMRemoveResultHandler(DBType dataType);
pascal OSErr	SMGetResultHandler(DBType dataType, ProcPtr *theHandler,
					Boolean getSysHandler);

short			FindHandler(DBType dataType);
OSErr			InsertHandler(HandlerRecord *newRec);
OSErr			RemoveHandler(short index);

OSErr			SMGeneric(long snarfID, DBAsyncParmBlkPtr asyncPB,
					DDEVParams *params);

OSErr			InitDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars);
OSErr			SendDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars);
OSErr			ExecDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars);
OSErr			StateDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars);
OSErr			GetItemDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars);
OSErr			AllDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars);

void			BuildDDEVList(ddevElemHndl *root);
void			ClearDDEVList(ddevElemHndl root);
void			PurgeDDEVList(ddevElemHndl *root);
void			AddDDEVFile(const Str255 fname, short vRefNum, long dirID,
					ddevElemHndl *root);

ddevElemHndl	FindAndBuildDDEV(char *name, ddevElemHndl *rootPtr);
ddevElemHndl	FindDDEV(char *name, ddevElemHndl root);

OSErr			OpenDDEVFile(ddevElemHndl ddevH, short *refNum);
OSErr			CallDDEV(ddevElemHndl ddevH, DDEVParams *params);

long			GetUniqueSnarfID(snarfSessElemHndl root);
snarfSessElemHndl	FindSnarfID(long id, snarfSessElemHndl root);
long			ConvertSessIDToSnarfID(long sessID, ddevElemHndl ddevH);
void			InsertSessionCell(sessElemHndl *root, sessElemHndl cell);
void			InsertSnarfCell(snarfSessElemHndl *root, snarfSessElemHndl cell);
DBAsyncParmBlkPtr	MakePB(DDEVParamsPtr params, DBAsyncParmBlkPtr asyncPB);
pascal void		InsertPB(DBAsyncParmBlkPtr asyncPB);
pascal void		RemovePB(DBAsyncParmBlkPtr asyncPB);
DBAsyncParmBlkPtr FindAppPB(DBAsyncParmBlkPtr appPB);

void			GetDDEVFolder(short *vRefNum, long *dirID);
OSErr			GrowResultsRecord(ResultsRecord *results, short len,
					Boolean inFirstRow);
OSErr			SetupResults(long sessID, snarfSessElemHndl *snarfPtr,
					ResultsRecord *results, ResultsRecord **oldResultsPtr,
					long *currOffset, short *currCol);
pascal void		DisposeResultsRecord(ResultsRecord *results);
OSErr			InitResultsRecord(ResultsRecord *results, Boolean inSysHeap);
OSErr			CallStatusProc(statusPtr statusProc, short message, OSErr rc);
pascal void		MyHLock(Handle h, char *state);

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMInitDBPack
 *
 * DESCRIPTION:	Called to initialize SnarfMan by each application.  The version
 *				number is implicitly passed in by the routine definition in
 *				DatabaseAccess.h.  This number is compared to the version in the
 *				PACK resource.  The first time this routine is called, it
 *				allocates space for globals and installs default result handlers.
 *
 * INPUTS:		version:		Version as defined in DatabaseAccess.h.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		rcDBWrongVersion
 *				Errors from NewPtrSysClear and InstallDefaultHandlers.
 *
 * MODIFICATIONS:
 *	10-03-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMInitDBPack(short version)
{
	register snarfGlobPtr	*globPtr;
	register snarfGlobPtr	glob;
	register short			rc;
	Handle					resH;
	PackHeaderHndl			thePack;

	/* compare version numbers */
	thePack = DataAccessMgrPack;
	if ((**thePack).version != version)
		return(rcDBWrongVersion);

	/*
		For now, we always stay locked down.
		Hopefully, we'll be more intelligent about this in
		the future.

		We no longer need to do this, PACKHelper takes care of it!
	*/

	// HNoPurge((Handle)thePack);
	// HLock((Handle)thePack);

	/* see if we need to allocate globals */
	globPtr = GetGlobalsPtr();

	rc = noErr;

	if (*globPtr == NULL)
	{
		/* allocate a cleared pointer in the system heap for globals */
		*globPtr = (snarfGlobPtr)NewPtrSysClear(sizeof(snarfGlob));
		rc = MemError();

		if (rc == noErr)
		{
			glob = *globPtr;

			/* build the list of installed ddevs */
			/*
				It turns out we can't do this at patch time because there is
				no International Utilities package available yet (AddDDEVFile
				calls IUCompStr).  So, it gets done at the first DBInit call.

				BuildDDEVList(&((*globPtr)->ddevRoot));

			*/

			/*
				Load the standard completion routine.
				We have to keep this separate so it doesn't
				get unlocked and/or purged at the wrong
				time by PACKHelper.
			*/
			*((short*) RomMapInsert) = mapTrue;
			
			resH = GetResource(COMPCODERESTYPE, COMPCODERESID);
			if (resH != NULL)
			{
				/* detach it */
				DetachResource(resH);

				/* resource should have locked bit set */

				/* save this handle in the globals */
				/* we may purge this someday */
				glob->completionRes = resH;
			}
			else
			{
				/* couldn't get specified 'proc' resource */
				if ((rc = RESERR) == noErr)
					rc = resNotFound;

				/* we better not continue, so get rid of the globals */
				DisposPtr((Ptr)glob);
				*globPtr = NULL;
			}

			if (rc == noErr)
			{
				/* install default result handlers */
				rc = InstallDefaultHandlers(glob);

				/* even if this fails, it isn't a fatal error, so leave globals around */
			}
		}
	}

	return(rc);

} /* SMInitDBPack() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		InstallDefaultHandlers
 *
 * DESCRIPTION:	Called by SMInitDBPack to install the default system result handlers.
 *				Reads the 'rtt#' resource and 'proc' resource containing the handlers,
 *				then calls DBInstallResultHandler for each one.
 *
 * INPUTS:		glob:		Pointer to globals.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		resNotFound
 *				Errors from NewHandleSys and DBInstallResultHandler.
 *
 * MODIFICATIONS:
 *	03-29-90	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
OSErr
InstallDefaultHandlers(snarfGlobPtr	glob)
{
	register OSErr		rc;
	register Handle		resH;
	register Ptr		resP;
	register short		numTypes;
	register short		handlerID;
	Handle				handlerH;
	ProcPtr				p;
/*	short				numHandlers; */

	/* allocate empty array of result handlers */
	glob->handlers = (HandlerRecordHandle)NewHandleSys(NULL);
	rc = MemError();

	/*
		Install the default handlers.

		These live in a separate 'proc' resource so it can stay locked
		down in one place all the time.  The 'rtt#' resource specifies
		which handlers we have.  It looks like:

			short	numHandlers;	number of 'proc' resources
									we only support one now

			For each 'proc':

			short	id;						id of 'proc'
			short	numTypes;				number of types handled by this 'proc'
			DBType	typeArray[numTypes];	types handled by this 'proc'
	*/

	if (rc == noErr)
	{
		/* get the 'rtt#' */
		*((short*) RomMapInsert) = mapTrue;
		
		resH = GetResource(RTTLISTRESTYPE, RTTLISTID);
		if (resH != NULL)
		{
			HLock(resH);
			resP = *resH;

			/* get number of 'proc' resources */
			/* we support one and only one for now */
			/* numHandlers = *((short*)resP); */
			resP += sizeof(short);

			/* get the id of this 'proc' */
			handlerID = *((short*)resP);
			resP += sizeof(short);

			/* get number of types supported by this 'proc' */
			numTypes = *((short*)resP);
			resP += sizeof(short);

			/* load in 'proc' */
			
			*((short*) RomMapInsert) = mapTrue;
			
			handlerH = GetResource(RTTCODERESTYPE, handlerID);
			if (handlerH != NULL)
			{
				/* detach it */
				DetachResource(handlerH);

				/* resource should have locked bit set */

				/* save this handle in the globals */
				/* we may purge this someday */
				glob->handlerRes = handlerH;

				/* make sure we're 32 bit clean */
				p = (ProcPtr)StripAddress(*handlerH);

				/* now, install it as a result handler for specified types */
				while((numTypes != 0) && (rc == noErr))
				{
					/* install handler for current type */
					rc = DBInstallResultHandler(*((DBType*)resP), p, true);
					resP += sizeof(DBType);

					numTypes--;
				}
			}
			else
			{
				/* couldn't get specified 'proc' resource */
				if ((rc = RESERR) == noErr)
					rc = resNotFound;
			}

			/* get rid of 'rtt#' (no need to unlock it) */
			ReleaseResource(resH);
		}

		/* we could put an else here if 'rtt#' resource not found, but it's not a fatal error */
	}

	return(rc);

} /* InstallDefaultHandlers() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMIdle
 *
 * DESCRIPTION:	Called every time SystemTask called.  Sends DBIdle message to any
 *				ddev with active sessions.  Also checks global list of asynchronous
 *				parameter blocks to see if it needs to perform a task that cannot be
 *				done at interrupt time.
 *
 * INPUTS:		None.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	04-05-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMIdle()
{
	register ddevElemHndl		ddevH;
	register DBAsyncParmBlkPtr	myPB;
	register asyncVarsPtr		myVars;
	register short				rc;
	snarfGlobPtr				glob;
	DBAsyncParmBlkPtr			appPB, nextPB;
	DDEVParams					params;

	glob = GetGlobals();

	/* get first ddev in list */
	ddevH = glob->ddevRoot;

	/****************************************/
	/* send idle messages to all open ddevs */
	/****************************************/
	while (ddevH != NULL)
	{
		/* see if this ddev has any open sessions */
		if ((**ddevH).sessRoot != NULL)
		{
			/* yes, send it an idle message */

			/* fill in parameter block for DBIdle */
			ClearParams(params);
			params.message = kDBIdle;

			/* ignore any errors */
			(void)CallDDEV(ddevH, &params);
		}

		/* get next ddev in list */
		ddevH = (**ddevH).next;
	}

	/***************************************/
	/* go through list of parameter blocks */
	/***************************************/
	myPB = glob->pbRoot;

	while (myPB != NULL)
	{
		myVars = (asyncVarsPtr)myPB->reserved;

		/* get next pb here in case we dispose of the current one */
		nextPB = myVars->next;

		if (!(myVars->executing))
		{
			/* current pb is done executing */
			if (!myVars->highlevel)
			{
				/********************************/
				/* low-level call has completed */
				/********************************/

				/* have to return parameters and do additional
					cleanup for some calls */
				switch (myVars->currAction)
				{
					case kDBInit:
						/* finish up using common code */
						SMInitComplete(myVars->ddevH, myVars->sessIDPtr,
							myVars->params->sessID, myVars->firstsess);
						break;

					case kDBEnd:
						if (myPB->result == noErr)
						{
							/* session ended OK */
							/* finish up using common code */
							myPB->result = SMEndComplete(myVars->snarf);
						}
						break;

					case kDBGetConnInfo:
						/* return returnedID, version, start, and state */

						/*
							Note that the returnedID in the DDEVParams is the
							ddev's session ID, which we have to convert
							to a global Snarf ID.
						*/
						if (myVars->returnedIDPtr != NULL)
						{
							if (myPB->result == noErr)	/* fix for bug DSL-008 */
							{
								/* ddev thinks it returned a valid session ID */
								*(myVars->returnedIDPtr) =
									ConvertSessIDToSnarfID(myVars->params->returnedID,
										myVars->ddevH);

								if (*(myVars->returnedIDPtr) == NULL)
								{
									/*
										The session ID returned by the ddev does
										not match any of the active sessions we
										have for it.  Hopefully, this should
										never happen.
									*/

									myPB->result = rcDBBadSessID;
								}
							}
						}

						if (myVars->versionPtr != NULL)
						{
							*(myVars->versionPtr) = myVars->params->version;
						}

						if (myVars->startPtr != NULL)
						{
							*(myVars->startPtr) = myVars->params->start;
						}

						if (myVars->statePtr != NULL)
						{
							*(myVars->statePtr) = myVars->params->state;
						}
						break;

					case kDBGetSessionNum:
						/* return sessNum */
						*(myVars->sessNumPtr) = myVars->params->sessNum;
						break;

					case kDBGetErr:
						/* return err1 and err2 */
						if (myVars->err1Ptr != NULL)
						{
							*(myVars->err1Ptr) = myVars->params->err1;
						}

						if (myVars->err2Ptr != NULL)
						{
							*(myVars->err2Ptr) = myVars->params->err2;
						}
						break;

					case kDBGetItem:
						/* return dataType, len, places, and flags */
						if (myVars->dataTypePtr != NULL)
						{
							*(myVars->dataTypePtr) = myVars->params->dataType;
						}

						if (myVars->lenPtr != NULL)
						{
							*(myVars->lenPtr) = myVars->params->len;
						}

						if (myVars->placesPtr != NULL)
						{
							*(myVars->placesPtr) = myVars->params->places;
						}

						if (myVars->flagsPtr != NULL)
						{
							*(myVars->flagsPtr) = myVars->params->flags;
						}
						break;
				}

				/* copy result into application's parameter block */
				appPB = myVars->asyncPB;
				appPB->result = myPB->result;

				/* remove pb from list */
				RemovePB(myPB);

				/* call application's completion routine, if any */
				if (appPB->completionProc != NULL)
				{
					/* have to use assembly glue to get pb into A0 */
					CallCompletion(appPB);
				}
			}
			else
			{
				/***********************************************************/
				/* high-level call has completed, call appropriate routine */
				/***********************************************************/
				switch (myVars->currAction)
				{
					/* DBStartQuery actions */
					case kDBInit:
						rc = InitDoneProc(myPB, myVars);
						break;

					case kDBSend:
						rc = SendDoneProc(myPB, myVars);
						break;

					case kDBExec:
						rc = ExecDoneProc(myPB, myVars);
						/* always go to done from here */
						rc = AllDoneProc(myPB, myVars);
						break;

					/* DBGetQueryResults actions */
					case kDBState:
						rc = StateDoneProc(myPB, myVars);
						break;

					case kDBGetItem:
						rc = GetItemDoneProc(myPB, myVars);
						break;

					/* all processing complete */
					/* this is only from GetItemDoneProc */
					case kAllDone:
						/* let AllDoneProc know what to clean up */
						myVars->currAction = kDBGetItem;
						rc = AllDoneProc(myPB, myVars);
						break;

				} /* switch (myVars->currAction) */

				if (rc != noErr)
				{
					/* error occurred someplace, so we're done */
					(void)AllDoneProc(myPB, myVars);
				}
			} /* if (!myVars->highlevel) */

		} /* if (!(myVars->executing)) */

		myPB = nextPB;

	} /* while (myPB != NULL) */

	return(noErr);

} /* SMIdle() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMOpen
 *
 * DESCRIPTION:	Called to send a DBOpen to ddev when first session is initiated.
 *				This is always a synchronous call.  Opens file, gets and detaches
 *				resource, calls ddev, and closes file.
 *
 * INPUTS:		ddevH:		Handle to ddevElem with info.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from opening resource file, getting ddev resource,
 *				detaching ddev resource, and CallDDEV.
 *
 * MODIFICATIONS:
 *	05-23-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
SMOpen(ddevElemHndl ddevH)
{
	register DDEVPtr	*ddevRes;
	register OSErr		rc;
	register short		currResFile;
	short				currVol, refNum = -1;
	short				vRefNum;
	long				dirID;
	DDEVParams			params;
	WDPBRec				pb;

	/* save current file */
	currResFile = CURMAP;

	/* open ddev file */
	rc = OpenDDEVFile(ddevH, &refNum);
	if (rc == noErr)
	{
		/* file open, get the resource */
		ddevRes = (DDEVPtr*)Get1Resource(DDEVRESTYPE, kDDEVID);

		if (ddevRes != NULL)
		{
			/* detach ddev resource */
			DetachResource((Handle)ddevRes);
			rc = RESERR;
		}
		else
		{
			/* couldn't get ddev */
			/* note that GetResource does not set ResErr for resNotFound! */
			if ((rc = RESERR) == noErr)
				rc = resNotFound;
		}

		if (rc == noErr)
		{
			/* lock down ddev code always */
			HLock((Handle)ddevRes);

			/* save handle to ddev in ddevH */
			(**ddevH).ddev = ddevRes;

			/* save old volume and set to ddev's volume */
			(void)GetVol(NULL, &currVol);

			/********************************************/
			/* •WARNING• •WARNING• •WARNING• •WARNING•	*/
			/*											*/
			/* The following is kind of bogus, but it	*/
			/* is probably better than doing an HSetVol.*/
			/* We need a working directory, though, to	*/
			/* do a normal SetVol.  Darin suggested not	*/
			/* closing it when we're done.				*/
			/*											*/
			/* •WARNING• •WARNING• •WARNING• •WARNING•	*/
			/********************************************/

			/* get vRefNum and dirID of ddev folder */
			GetDDEVFolder(&vRefNum, &dirID);

			/* open a working directory */
			pb.ioCompletion = 0L;
			pb.ioNamePtr = 0L;
			pb.ioVRefNum = vRefNum;
			pb.ioWDDirID = dirID;
			pb.ioWDProcID = 'ERIK';		/* Darin says use this */

			rc = PBOpenWD(&pb, false);

			if (rc == noErr)
			{
				/* opened working directory */
				(void)SetVol(NULL, pb.ioVRefNum);

				/* set up parameters for open */
				ClearParams(params);
				params.message = kDBOpen;

				/* call ddev - DBOpen */
				rc = CallDDEV(ddevH, &params);

				/* restore current volume */
				(void)SetVol(NULL, currVol);
			}
		}

		if ((rc != noErr) && (ddevRes != NULL))
		{
			/* error opening ddev, dispose of everything */

			/* dispose of ddev code (no need to unlock it) */
			DisposHandle((Handle)ddevRes);
			(**ddevH).ddev = NULL;

			/* no more storage */
			(**ddevH).ddevStorage = NULL;
		}
	}

	/* clean up */
	if (refNum != -1)
		CloseResFile(refNum);
	UseResFile(currResFile);

	return(rc);

} /* SMOpen() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMClose
 *
 * DESCRIPTION:	Called to send a DBClose to ddev when last session has ended.
 *				Disposes of ddev code.  This is always a synchronous call.
 *
 * INPUTS:		ddevH:		Handle to ddevElem with info.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Return codes from CallDDEV, DisposHandle.
 *
 * MODIFICATIONS:
 *	05-23-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
SMClose(ddevElemHndl ddevH)
{
	register short	rc;
	DDEVParams		params;

	ClearParams(params);
	params.message = kDBClose;

	/* call ddev - DBClose */
	rc = CallDDEV(ddevH, &params);

	/* dispose of ddev code (no need to unlock it) */
	DisposHandle((Handle)(**ddevH).ddev);
	(**ddevH).ddev = NULL;

	/* no more storage */
	(**ddevH).ddevStorage = NULL;

	/* report errors from DBClose first before MemError */
	if (rc == noErr)
	{
		/* DBClose OK, so report any error from DisposHandle */
		rc = MemError();
	}

	return(rc);

} /* SMClose() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMInit
 *
 * DESCRIPTION:	DBInit routine.  Searches for specified ddev, opens it if
 *				this is the first session for it (sending it a DBOpen message),
 *				sends it a DBInit message, and calls common code which inserts a
 *				new session and snarf id into the session and snarf id lists.
 *
 * INPUTS:		sessID:		Pointer to variable for session ID.
 *				ddevName:	Corresponds to 'STR ' 128 in ddev.
 *				host:		Host name.
 *				user:		User name.
 *				passwd:		Password.
 *				connStr:	Connection string.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		sessID:		Pointer to filled in session ID.
 *
 * RETURNS:		rcDBBadDDEV
 *				rcDBAsyncNotSupp
 *				Errors from SMOpen and CallDDEV.
 *
 * MODIFICATIONS:
 *	03-23-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMInit(long *sessID, const Str63 ddevName, const Str255 host,
	const Str255 user, const Str255 password, const Str255 connStr, DBAsyncParmBlkPtr asyncPB)
{
	register ddevElemHndl	ddevH;
	register asyncVarsPtr	myVars;
	register OSErr			rc = noErr;
	register Boolean		firstsess;
	snarfGlobPtr			glob;
	DBAsyncParmBlkPtr		myPB;
	DDEVParams				params;

	/* assume no session will come up */
	*sessID = 0;

	glob = GetGlobals();

	/* look for ddevElem with specified ddevName, rebuilding list if necessary */
	ddevH = FindAndBuildDDEV((char*)ddevName, &(glob->ddevRoot));

	if (ddevH == NULL)
	{
		/* bad ddev name */
		rc = rcDBBadDDEV;
	}

	if (rc == noErr)
	{
		/* see if ddev code loaded, if not, then it needs a DBOpen call */
		firstsess = ((**ddevH).ddev == NULL);

		if (firstsess)
		{
			/* special case - check for async here so we don't send open */
			if (asyncPB && !(**ddevH).canAsync)
			{
				/* ddev doesn't support async, but DBInit called async */
				rc = rcDBAsyncNotSupp;
			}

			if (rc == noErr)
			{
				/* call ddev with open message */
				rc = SMOpen(ddevH);
			}
		}

		if (rc == noErr)
		{
			/* fill in parameter block for DBInit */
			ClearParams(params);
			params.message = kDBInit;
			params.host = host;
			params.user = user;
			params.password = password;
			params.connStr = connStr;

			/* check for asynchronous call */
			if (asyncPB)
			{
				/* we have to set up our own parameter block */
				myPB = MakePB(&params, asyncPB);
				myVars = (asyncVarsPtr)myPB->reserved;

				/* keep track if first session for ddev */
				myVars->firstsess = firstsess;

				/* keep track of session ID */
				myVars->sessIDPtr = sessID;

				/* put parameter block into global list */
				InsertPB(myPB);

				/* return immediately */
				return(CallDDEV(ddevH, myVars->params));
			}

			/* call ddev synchronously - DBInit */
			rc = CallDDEV(ddevH, &params);

			/* finish up using common code */
			SMInitComplete(ddevH, sessID, params.sessID, firstsess);
		}
	}

	return(rc);

} /* SMInit() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMInitComplete
 *
 * DESCRIPTION:	Called after a synchronous or asynchronous DBInit call completes.
 *				Allows code sharing.  If DBInit returned a session ID, inserts a
 *				new session and snarf id into the session and snarf id lists.
 *
 *				Declared as a pascal function because compiler generates smaller code.
 *
 * INPUTS:		ddevH:		Handle to ddev called.
 *				sessIDPtr:	Pointer to return session ID.
 *				sessID:		Session ID returned by ddev.
 *				firstsess:	If TRUE, this is the first session for this ddev.
 *
 * OUTPUTS:		sessIDPtr:	Pointer to filled in session ID.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	09-07-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal void
SMInitComplete(ddevElemHndl ddevH, long *sessIDPtr, long sessID,
	Boolean firstsess)
{
	register long	snarfID;
	snarfGlobPtr	glob;
	sessElem		**sess;
	snarfSessElem	**snarf;
	char			currState;

	/*
		Note that we can't depend on the rc from the ddev
		to determine if a session is up.  CL/1, for example,
		will always return a session ID, even if an error occurs.
	*/

	if (sessID == 0)
	{
		/* ddev didn't return a session ID, so definitely no session */
		if (firstsess)
		{
			/* first session for this ddev, so close it */
			(void)SMClose(ddevH);
		}
	}
	else
	{
		/* ddev returned a session ID, but there may still be an error */

		/* set up cell for session */
		sess = (sessElemHndl)NewHandleSysClear(sizeof(sessElem));
		(**sess).parent = ddevH;
		(**sess).sessID = sessID;

		/* insert it into the list of sessions for this ddev */
		MyHLock((Handle)ddevH, &currState);
		InsertSessionCell(&((**ddevH).sessRoot), sess);
		HSetState((Handle)ddevH, currState);

		/* find a unique snarf session ID for this session */
		glob = GetGlobals();
		snarfID = GetUniqueSnarfID(glob->snarfRoot);

		/* set up snarf cell */
		snarf = (snarfSessElemHndl)NewHandleSysClear(sizeof(snarfSessElem));
		(**snarf).snarfID = snarfID;
		/* (**snarf).processID = ?; */
		(**snarf).sess = sess;

		/* insert it into the global list of sessions */
		InsertSnarfCell(&(glob->snarfRoot), snarf);

		/* return the ID */
		*sessIDPtr = snarfID;
	}

} /* SMInitComplete() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMEnd
 *
 * DESCRIPTION:	DBEnd routine.  Calls DBEnd and, if successful, calls common code
 *				which removes session id and snarf id from the lists, and, if the
 *				last session for this ddev was just ended, calls SMClose to send
 *				the ddev a DBClose message.
 *
 * INPUTS:		sessID:		Session ID.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from SMGeneric and SMEndComplete.
 *
 * MODIFICATIONS:
 *	03-26-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMEnd(long sessID, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	snarfGlobPtr		glob;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	glob = GetGlobals();

	/* fill in parameter block for DBEnd */
	ClearParams(params);
	params.message = kDBEnd;

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* keep track of snarf session cell */
		myVars->snarf = FindSnarfID(sessID, glob->snarfRoot);

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{
		/* call ddev synchronously - DBEnd */
		if ((rc = SMGeneric(sessID, NULL, &params)) == noErr)
		{
			/* session ended OK, clean up lists */

			/* finish up using common code */
			rc = SMEndComplete(FindSnarfID(sessID, glob->snarfRoot));
		}
	}

	return(rc);

} /* SMEnd() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMEndComplete
 *
 * DESCRIPTION:	Called after a synchronous or asynchronous DBEnd call completes.
 *				Allows code sharing.  This routine should only be called if DBEnd
 *				was successful.  Removes session id and snarf id from the lists,
 *				and, if the last session for this ddev was just ended, calls
 *				SMClose to send the ddev a DBClose message.
 *
 * INPUTS:		snarf:		Snarf session cell that was just ended.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Return codes from SMClose.
 *
 * MODIFICATIONS:
 *	09-09-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
SMEndComplete(snarfSessElemHndl snarf)
{
	register short			rc = noErr;
	snarfGlobPtr			glob;
	ddevElemHndl			ddevH;
	ddevElemPtr				ddevP;
	snarfSessElemHndl		prevsnarf;
	sessElemHndl			sess, prevsess;
	ResultsRecord			*oldResults;
	char					currState;

	glob = GetGlobals();

	/* delete any leftover results */
	if ((oldResults = (**snarf).oldResults) != NULL)
	{
		DisposeResultsRecord(oldResults);
		DisposPtr((Ptr)oldResults);
	}

	/* get session and ddev info */
	sess = (**snarf).sess;
	ddevH = (**sess).parent;

	MyHLock((Handle)ddevH, &currState);
	ddevP = *ddevH;

	/* delete snarf element */
	if (snarf == glob->snarfRoot)
	{
		/* deleting root */
		glob->snarfRoot = (**snarf).next;
	}
	else
	{
		/* find previous snarf element */
		prevsnarf = glob->snarfRoot;

		while ((**prevsnarf).next != snarf)
		{
			prevsnarf = (**prevsnarf).next;
		}

		/* fix list */
		(**prevsnarf).next = (**snarf).next;
	}

	DisposHandle((Handle)snarf);

	/* delete session element */
	if (sess == ddevP->sessRoot)
	{
		/* deleting root */
		ddevP->sessRoot = (**sess).next;
	}
	else
	{
		/* find previous sess element */
		prevsess = ddevP->sessRoot;

		while ((**prevsess).next != sess)
		{
			prevsess = (**prevsess).next;
		}

		/* fix list */
		(**prevsess).next = (**sess).next;
	}

	DisposHandle((Handle)sess);

	/* finally, check if last session for this ddev was just ended */
	if (ddevP->sessRoot == NULL)
	{
		/* yes, need to send close message and close ddev */
		rc = SMClose(ddevH);
	}

	HSetState((Handle)ddevH, currState);

	return(rc);

} /* SMEndComplete() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMGetConnInfo
 *
 * DESCRIPTION:	DBGetConnInfo routine.  If sessID is specified, returns
 *				ddevName.  Otherwise, tries to find ddev with specified
 *				name.  After call is made, if sessID is specified, just
 *				returns it in returnedID, otherwise converts session ID
 *				returned by ddev to Snarf ID.
 *
 * INPUTS:		sessID:		Session ID.  If NULL, sessNum and ddevName must
 *							be specified.
 *				sessNum:	Session number.  Only used if sessID is NULL.
 *				returnedID:	Pointer to variable for returned session ID.
 *				version:	Pointer to variable for version.
 *				ddevName:	Name of ddev.  Only used if sessID is NULL.
 *				host:		Pointer to variable for host (optional).
 *				user:		Pointer to variable for user (optional).
 *				network:	Pointer to variable for network (optional).
 *				connStr:	Pointer to variable for connStr (optional).
 *				start:		Pointer to variable for start time (optional).
 *				state:		Pointer to variable for state (optional).
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		returnedID:	Pointer to filled in returned session ID.
 *				version:	Pointer to filled in version.
 *				ddevName:	Name of ddev.  Only set if sessID is NULL.
 *				host:		Pointer to filled in host.
 *				user:		Pointer to filled in user.
 *				network:	Pointer to filled in network.
 *				connStr:	Pointer to filled in connStr.
 *				start:		Pointer to filled in start time.
 *				state:		Pointer to filled in state.
 *
 * RETURNS:		rcDBBadDDEV
 *				rcDBBadSessID
 *				rcDBBadSessNum
 *				Errors from CallDDEV and SMGeneric.
 *
 * MODIFICATIONS:
 *	08-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMGetConnInfo(long sessID, short sessNum, long *returnedID, long *version,
	Str63 ddevName, Str255 host, Str255 user, Str255 network, Str255 connStr,
	long *start, OSErr *state, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc = noErr;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	snarfGlobPtr 		glob;
	ddevElemHndl		ddevH;
	snarfSessElemHndl	snarf;
	sessElemHndl		sess;
	char				*theName;
	Boolean				isSessNumCall;
	DDEVParams			params;

	glob = GetGlobals();

	/* special case - if session ID is specified, need to return ddev name */
	if (sessID != 0)
	{
		/* not being called based on session number */
		isSessNumCall = false;

		/* find snarf ID */
		if ((snarf = FindSnarfID(sessID, glob->snarfRoot)) == NULL)
		{
			/* bad session ID */
			rc = rcDBBadSessID;
		}
		else
		{
			/* got snarf ID, get session and ddev info */
			sess = (**snarf).sess;
			ddevH = (**sess).parent;

			/* get pointer to ddev name */
			theName = (**ddevH).name;

			/* return ddev name */
			if (ddevName != NULL)
				pstrcpy(ddevName, theName);
		}
	}
	else
	{
		/* session ID not specifed, need to use sessNum and ddevName */
		isSessNumCall = true;

		/* assume failure */
		rc = rcDBBadDDEV;			/* fix for bug DSL-008 */

		/* look for ddevElem with specified ddevName, rebuilding list if necessary */
		ddevH = FindAndBuildDDEV((char*)ddevName, &(glob->ddevRoot));

		if (ddevH != NULL)
		{
			/* found ddev, see if it is loaded */
			if ((**ddevH).ddev != NULL)
			{
				/* yes, we can call it */
				rc = noErr;
			}
			else
			{
				/* ddev in list, but not loaded */
				rc = rcDBBadSessNum;		/* fix for bug DSL-012 */
			}
		}

		/* else the ddev doesn't exist, so rcDBBadDDEV is the right error */

	}

	if (rc == noErr)
	{
		/* fill in parameter block for DBGetConnInfo */
		ClearParams(params);
		params.message = kDBGetConnInfo;
		params.sessNum = sessNum;
		params.host = host;
		params.user = user;
		params.network = network;
		params.connStr = connStr;

		/* check for asynchronous call */
		if (asyncPB)
		{
			/* we have to set up our own parameter block */
			myPB = MakePB(&params, asyncPB);
			myVars = (asyncVarsPtr)myPB->reserved;

			/* keep track of returnedID, version, start, and state pointers */
			myVars->returnedIDPtr = returnedID;
			myVars->versionPtr = version;
			myVars->startPtr = start;
			myVars->statePtr = state;

			/* set current action */
			myVars->currAction = params.message;

			/* put parameter block into global list */
			InsertPB(myPB);

			/* return immediately */
			if (isSessNumCall)
			{
				/* no sessID, have to CallDDEV ourselves */
				rc = CallDDEV(ddevH, myVars->params);
			}
			else
			{
				rc = SMGeneric(sessID, myPB, myVars->params);
			}
		}
		else
		{
			/* call ddev synchronously - DBGetConnInfo */
			if (isSessNumCall)
			{
				/* no sessID, have to CallDDEV ourselves */
				rc = CallDDEV(ddevH, &params);
			}
			else
			{
				rc = SMGeneric(sessID, NULL, &params);
			}

			/* see if we can return other parameters */
			if (isSessNumCall || (rc != rcDBBadSessID))
			{
				/* ddev was invoked */

				if (returnedID != NULL)
				{
					/*
						Note that the returnedID in the DDEVParams is the
						ddev's session ID, which we have to convert
						to a global Snarf ID.
					*/
					if (rc == noErr)	/* fix for bug DSL-008 */
					{
						/* ddev thinks it returned a valid session ID */
						*returnedID = ConvertSessIDToSnarfID(params.returnedID, ddevH);
						if (*returnedID == NULL)
						{
							/*
								The session ID returned by the ddev does
								not match any of the active sessions we
								have for it.  Hopefully, this should
								never happen.
							*/
							rc = rcDBBadSessID;
						}
					}
				}

				/* return version, start, and state */
				if (version != NULL)
				{
					*version = params.version;
				}

				if (start != NULL)
				{
					*start = params.start;
				}

				if (state != NULL)
				{
					*state = params.state;
				}
			}
		}
	}

	return(rc);

} /* SMGetConnInfo() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMGetSessionNum
 *
 * DESCRIPTION:	DBGetSessionNum routine.  Calls DBGetSessionNum with specified
 *				parameters.
 *
 * INPUTS:		sessID:		Session ID.
 *				sessNum:	Pointer to variable for session number.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		sessNum:	Pointer to filled in session number.
 *
 * RETURNS:		Errors from SMGeneric.
 *
 * MODIFICATIONS:
 *	08-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMGetSessionNum(long sessID, short *sessNum, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	/* fill in parameter block for DBGetErr */
	ClearParams(params);
	params.message = kDBGetSessionNum;

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* keep track of session number pointer */
		myVars->sessNumPtr = sessNum;

		/* set current action */
		myVars->currAction = params.message;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{

		/* call ddev synchronously - DBGetSessionNum */
		rc = SMGeneric(sessID, NULL, &params);

		/* see if we can return other parameters */
		if (rc != rcDBBadSessID)
		{
			/* ddev was invoked */
			/* return sessNum */
			*sessNum = params.sessNum;
		}
	}

	return(rc);

} /* SMGetSessionNum() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMSend
 *
 * DESCRIPTION:	DBSend routine.  Calls DBSend with specified parameters.
 *
 * INPUTS:		sessID:		Session ID.
 *				text:		Pointer to text to send.
 *				len:		Length of text.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from SMGeneric.
 *
 * MODIFICATIONS:
 *	03-27-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMSend(long sessID, char *text, short len, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	/* fill in parameter block for DBSend */
	ClearParams(params);
	params.message = kDBSend;
	params.buffer = text;
	params.len = len;

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{
		/* call ddev synchronously - DBSend */
		rc = SMGeneric(sessID, NULL, &params);
	}

	return(rc);

} /* SMSend() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMSendItem
 *
 * DESCRIPTION:	DBSendItem routine.  Calls DBSendItem with specified parameters.
 *
 * INPUTS:		sessID:		Session ID.
 *				dataType:	Type of data to send.
 *				len:		Length of data.
 *				places:		Places in data.
 *				flags:		Flags for data.
 *				buffer:		Pointer to data to send.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from SMGeneric.
 *
 * MODIFICATIONS:
 *	08-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMSendItem(long sessID, DBType dataType, short len, short places, short flags,
	Ptr buffer, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	/* fill in parameter block for DBGetItem */
	ClearParams(params);
	params.message = kDBSendItem;
	params.buffer = buffer;
	params.dataType = dataType;
	params.len = len;
	params.places = places;
	params.flags = flags;

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{
		/* call ddev synchronously - DBSendItem */
		rc = SMGeneric(sessID, NULL, &params);
	}

	return(rc);

} /* SMSendItem() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMExec
 *
 * DESCRIPTION:	DBExec routine.  Calls DBExec with specified parameters.
 *
 * INPUTS:		sessID:		Session ID.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from SMGeneric.
 *
 * MODIFICATIONS:
 *	03-27-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMExec(long sessID, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	/* fill in parameter block for DBExec */
	ClearParams(params);
	params.message = kDBExec;

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{
		/* call ddev synchronously - DBExec */
		rc = SMGeneric(sessID, NULL, &params);
	}

	return(rc);

} /* SMExec() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMState
 *
 * DESCRIPTION:	DBState routine.  Calls DBState with specified parameters.
 *
 * INPUTS:		sessID:		Session ID.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from SMGeneric.
 *
 * MODIFICATIONS:
 *	03-27-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMState(long sessID, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	/* fill in parameter block for DBState */
	ClearParams(params);
	params.message = kDBState;

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{
		/* call ddev synchronously - DBState */
		rc = SMGeneric(sessID, NULL, &params);
	}

	return(rc);

} /* SMState() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMGetErr
 *
 * DESCRIPTION:	DBGetErr routine.  Calls DBGetErr with specified parameters.
 *
 * INPUTS:		sessID:		Session ID.
 *				err1:		Pointer to error 1 (optional).
 *				err2:		Pointer to error 2 (optional).
 *				item1:		Pointer to item1.
 *				item2:		Pointer to item2.
 *				errorMsg:	Pointer to errorMsg.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		err1:		Modified.
 *				err2:		Modified.
 *				item1:		Modified.
 *				item2:		Modified.
 *				errorMsg:	Modified.
 *
 * RETURNS:		Errors from SMGeneric.
 *
 * MODIFICATIONS:
 *	03-27-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMGetErr(long sessID, long *err1, long *err2, Str255 item1,
	Str255 item2, Str255 errorMsg, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	/* fill in parameter block for DBGetErr */
	ClearParams(params);
	params.message = kDBGetErr;
	params.item1 = item1;
	params.item2 = item2;
	params.errorMsg = errorMsg;

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* keep track of error pointers */
		myVars->err1Ptr = err1;
		myVars->err2Ptr = err2;

		/* set current action */
		myVars->currAction = params.message;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{
		/* call ddev synchronously - DBGetErr */
		rc = SMGeneric(sessID, NULL, &params);

		/* see if we can return other parameters */
		if (rc != rcDBBadSessID)
		{
			/* ddev was invoked */
			/* return err1 and err2 */
			if (err1 != NULL)
			{
				*err1 = params.err1;
			}

			if (err2 != NULL)
			{
				*err2 = params.err2;
			}
		}
	}

	return(rc);

} /* SMGetErr() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMBreak
 *
 * DESCRIPTION:	DBBreak routine.  Calls DBBreak with specified parameters.
 *
 * INPUTS:		sessID:		Session ID.
 *				abort:		How to break.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from SMGeneric.
 *
 * MODIFICATIONS:
 *	03-27-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMBreak(long sessID, Boolean abort, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	/* fill in parameter block for DBBreak */
	ClearParams(params);
	params.message = kDBBreak;
	params.abort = abort;

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{
		/* call ddev synchronously - DBBreak */
		rc = SMGeneric(sessID, NULL, &params);
	}

	return(rc);

} /* SMBreak() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMGetItem
 *
 * DESCRIPTION:	DBGetItem routine.  Calls DBGetItem with specified parameters.
 *
 * INPUTS:		sessID:		Session ID.
 *				timeout:	How long to wait for data to be available.
 *				dataType:	Pointer to variable with type of data expected (optional).
 *				len:		Pointer to variable for length (optional).
 *				places:		Pointer to variable for places (optional).
 *				flags:		Pointer to variable for flags (optional).
 *				buffer:		Pointer for data to be returned (optional).
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		dataType:	Pointer to filled in data type.
 *				len:		Pointer to filled in length.
 *				places:		Pointer to filled in places.
 *				flags:		Pointer to filled in flags.
 *				buffer:		Pointer filled in with data.
 *
 * RETURNS:		Errors from SMGeneric.
 *
 * MODIFICATIONS:
 *	08-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMGetItem(long sessID, long timeout, DBType *dataType, short *len, short *places,
	short *flags, Ptr buffer, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	/* fill in parameter block for DBGetItem */
	ClearParams(params);
	params.message = kDBGetItem;
	params.timeout = timeout;
	params.buffer = buffer;
	if (dataType != NULL)
	{
		params.dataType = *dataType;
	}

	if (len != NULL)
	{
		params.len = *len;
	}

	if (places != NULL)
	{
		params.places = *places;
	}

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* keep track of len, dataType, places, and flags pointers */
		myVars->lenPtr = len;
		myVars->dataTypePtr = dataType;
		myVars->placesPtr = places;
		myVars->flagsPtr = flags;

		/* set current action */
		myVars->currAction = params.message;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{
		/* call ddev synchronously - DBGetItem */
		rc = SMGeneric(sessID, NULL, &params);

		/* see if we can return other parameters */
		if (rc != rcDBBadSessID)
		{
			/* ddev was invoked */
			/* return len, dataType, places, and flags */
			if (len != NULL)
			{
				*len = params.len;
			}

			if (dataType != NULL)
			{
				*dataType = params.dataType;
			}

			if (places != NULL)
			{
				*places = params.places;
			}

			if (flags != NULL)
			{
				*flags = params.flags;
			}
		}
	}

	return(rc);

} /* SMGetItem() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMUnGetItem
 *
 * DESCRIPTION:	DBUnGetItem routine.  Calls DBUnGetItem with specified parameters.
 *
 * INPUTS:		sessID:		Session ID.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from SMGeneric.
 *
 * MODIFICATIONS:
 *	03-27-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMUnGetItem(long sessID, DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;
	DDEVParams			params;

	/* fill in parameter block for DBUnGetItem */
	ClearParams(params);
	params.message = kDBUnGetItem;

	/* check for asynchronous call */
	if (asyncPB)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(&params, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* return immediately */
		rc = SMGeneric(sessID, myPB, myVars->params);
	}
	else
	{
		/* call ddev synchronously - DBUnGetItem */
		rc = SMGeneric(sessID, NULL, &params);
	}

	return(rc);

} /* SMUnGetItem() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMGetNewQuery
 *
 * DESCRIPTION:	DBGetNewQuery routine.  Gets the specified 'qrsc' resource, and
 *				builds a QueryRecord from it.
 *
 * INPUTS:		queryID:	ID of 'qrsc'.
 *				query:		Pointer to variable to return query in.
 *
 * OUTPUTS:		query:		Filled in if successful.
 *
 * RETURNS:		Errors from GetResource and NewHandle.
 *
 * MODIFICATIONS:
 *	03-28-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMGetNewQuery(short queryID, QueryHandle *query)
{
	register short	i;
	register OSErr	rc = noErr;
	queryRes		**resH, *resP;
	QueryRecord		**queryH, *queryP;
	resArrayList	*resList;
	Handle			*queries;
	short			*wstrID;
	unsigned long	resListSize = 0;

	/* no query yet */
	*query = NULL;

	/* get the 'qrsc' */
	*((short*) RomMapInsert) = mapTrue;
	
	resH = (queryRes**)GetResource(QRSCRESTYPE, queryID);

	if (!resH)
	{
		/* couldn't get 'qrsc' */
		/* note that GetResource does not set ResError for resNotFound! */
		if ((rc = RESERR) == noErr)
			rc = resNotFound;
	}
	else
	{
		queryH = (QueryRecord**)NewHandleClear(sizeof(QueryRecord));
		if (!queryH)
		{
			/* couldn't allocate QueryHandle */
			rc = MemError();

			/* resH is released below */
		}
	}

	if (rc == noErr)
	{
		HLock((Handle)resH);
		resP = *resH;

		HLock((Handle)queryH);
		queryP = *queryH;

		/* allocate memory for queries and resources first */
		if (resP->numQueries > 0)		/* fix for bug DSL-010 */
		{
			/* there are some queries, allocate list */
			queryP->queryList = (QueryListHandle)NewHandleClear(resP->numQueries *
											sizeof(Handle));
			rc = MemError();
		}

		if (rc == noErr)
		{
			/* get pointer to list of resources */
			resList = (resArrayList*)(&(resP->queryArray[resP->numQueries]));

			if (resList->numRes > 0)	/* fix for bug DSL-010 */
			{
				/* there are some resources, allocate list */

				/* calculate size of resList once here, as we use it for BlockMove later */
				resListSize = sizeof(ResListElem) * resList->numRes;
				queryP->resList = (ResListHandle)NewHandleClear(resListSize);
				rc = MemError();
			}

			if (rc == noErr)
			{
				/* copy info into record */
				queryP->version = resP->version;			/* version */
				queryP->id = queryID;						/* id of 'qrsc' */
				queryP->currQuery = resP->currQuery;		/* current query offset */
				queryP->numQueries = resP->numQueries;		/* number of queries in array */
				queryP->numRes = resList->numRes;			/* number of resources in array */

				/* get qdef proc - id 0 means no qdef */
				if (resP->qdefID != 0)
				{
					*((short*) RomMapInsert) = mapTrue;
					
					queryP->queryProc = GetResource(QDEFRESTYPE, resP->qdefID);
					if (!queryP->queryProc)
					{
						/* couldn't get qdef */
						if ((rc = RESERR) == noErr)
							rc = resNotFound;
					}
				}

				if (rc == noErr)
				{
					/* get DBInit parameters */
					GetIndString(queryP->ddevName, resP->strID, DDEVOFFSET);
					GetIndString(queryP->host, resP->strID, HOSTOFFSET);
					GetIndString(queryP->user, resP->strID, USEROFFSET);
					GetIndString(queryP->password, resP->strID, PASSWORDOFFSET);
					GetIndString(queryP->connStr, resP->strID, CONNSTROFFSET);

					/* set up list of queries */
					HLock((Handle)queryP->queryList);
					queries = *(queryP->queryList);
					wstrID = &(resP->queryArray[0]);

					for (i = 0; i < resP->numQueries; i++)
					{
						*((short*) RomMapInsert) = mapTrue;
						
						*queries = GetResource(QUERYRESTYPE, *wstrID);
						if (*queries)
						{
							DetachResource(*queries);
						}
						else
						{
							/* couldn't get specified resource */
							if ((rc = RESERR) == noErr)
								rc = resNotFound;
						}

						/* bump pointers */
						queries++;
						wstrID++;
					}
					HUnlock((Handle)queryP->queryList);

					/*
						Set up list of resources.

						Since the ResListElem array in the 'qrsc' is the same
						as the ResListElem array in the QueryRecord, we can
						just blast it in.
					*/
					if (resListSize > 0)
					{
						BlockMove((Ptr)&(resList->res[0]), (Ptr)*(queryP->resList), resListSize);
					}
				}
			}

			/* check for any errors */
			if (rc != noErr)
			{
				/* an error occurred someplace, so clean up everything */

				/* dispose of queryList, if necessary */
				if (queryP->queryList)
				{
					/* queryList was allocated */
					HLock((Handle)queryP->queryList);
					queries = *(queryP->queryList);

					/* dispose of all the 'wstr' handles */
					for (i = 0; i < resP->numQueries; i++)
					{
						if (*queries)
						{
							/* dispose of the current detached 'wstr' handle */
							DisposHandle(*queries);
						}

						/* bump pointer */
						queries++;
					}

					/* dispose of queryList (no need to unlock it) */
					DisposHandle((Handle)queryP->queryList);
				}

				/* dispose of resList, if necessary */
				if (queryP->resList)
					DisposHandle((Handle)queryP->resList);

				/* finally, get rid of queryH */
				DisposHandle((Handle)queryH);
			}
			else
			{
				/* everything OK */
				HUnlock((Handle)queryH);

				/* return query */
				*query = queryH;
			}
		}
	}

	/* no need to unlock resource before disposing */
	if (resH)
		ReleaseResource((Handle)resH);

	return(rc);

} /* SMGetNewQuery() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMDisposeQuery
 *
 * DESCRIPTION:	DBDisposeQuery routine.  Unlocks qdef, disposes of all queries in
 *				queryList and querylist itself, disposes of resList, and finally
 *				disposes of query record itself.
 *
 *				Questions:  Correct strategy for qdef (unlock or dispose?)
 *							Right now, only checks for last MemError.
 *
 * INPUTS:		query:		Handle to query to dispose.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		nilHandleErr
 *				Errors from DisposHandle.
 *
 * MODIFICATIONS:
 *	03-28-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMDisposeQuery(QueryHandle query)
QueryHandle		query;
{
	register short			rc = noErr;
	register short			i;
	register QueryRecord	*queryP;
	register Handle			*queries;

	if (query == NULL)
	{
		/* passed in nil handle */
		rc = nilHandleErr;
	}
	else
	{
		HLock((Handle)query);
		queryP = *query;

		/* unlock qdef */
		if (queryP->queryProc != NULL)
		{
			HUnlock(queryP->queryProc);
			// DisposHandle(queryP->queryProc);
		}

		/* dispose of queries */
		if (queryP->queryList != NULL)
		{
			HLock((Handle)queryP->queryList);

			/* dispose of all the handles in the array */
			queries = *(queryP->queryList);
			i = queryP->numQueries;
			while ((i > 0) && (rc == noErr))
			{
				DisposHandle(*queries);
				rc = MemError();
				queries++;
				i--;
			}

			if (rc == noErr)
			{
				/* no need to unlock handle before disposing */
				DisposHandle((Handle)queryP->queryList);
				rc = MemError();
			}
		}

		if (rc == noErr)
		{
			/* dispose of resource list */
			if (queryP->resList != NULL)
			{
				DisposHandle((Handle)queryP->resList);
				rc = MemError();
			}

			if (rc == noErr)
			{
				/* finally, dispose of query record (no need to unlock it) */
				DisposHandle((Handle)query);
				rc = MemError();
			}
		}
	}

	return(rc);

} /* SMDisposeQuery() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMStartQuery
 *
 * DESCRIPTION:	DBStartQuery routine.  If query is NULL, prompts for query document.
 *				Then, calls qdef with query, followed by DBInit, DBSend, and DBExec.
 *
 *				Still need to do async stuff right.
 *
 * INPUTS:		sessID:		Pointer for session ID.
 *				query:		Handle to query.
 *				statusProc:	Pointer to user status function, if any.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		sessID:		Pointer to filled in session ID.
 *
 * RETURNS:		nilHandleErr (if query is NULL)
 *				userCanceledErr
 *				rcDBError (if currQuery not in range)
 *				Errors from qdef, DBInit, DBSend, and DBExec.
 *
 * MODIFICATIONS:
 *	04-14-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMStartQuery(long *sessID, QueryHandle query, statusPtr statusProc,
	DBAsyncParmBlkPtr asyncPB)
{
	QDEFPtr			qdef;
	QueryRecord		*qp;
	short			rc;
	Handle			queryText, *queries;
	short			len;
	char			currState, textState;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr	myVars;

	/********************************************/
	/* Check for a valid query handle.			*/
	/********************************************/

	if (query == NULL)
	{
		/* query is null, return error */
		rc = nilHandleErr;

		/* call status function one last time */
		rc = CallStatusProc(statusProc, kDBStartQueryComplete, rc);

		if (asyncPB)
			asyncPB->result = rc;
		return(rc);
	}

	/********************************************/
	/* We have a query, now call the qdef if	*/
	/* there is one.							*/
	/********************************************/

	if ((**query).queryProc != NULL)
	{
		GrafPtr			oldPort;

		/* lock down queryProc and get a pointer to it */
		MyHLock((**query).queryProc, &currState);
		qdef = (QDEFPtr)(*((**query).queryProc));

		/* call qdef with query, saving current GrafPort */
		GetPort(&oldPort);
		rc = (qdef)(sessID, query);
		SetPort(oldPort);

		/* restore state of handle */
		HSetState((**query).queryProc, currState);

		if ((rc == noErr) && (statusProc != NULL))
		{
			/* qdef completed successfully, call app's
				status function */
			rc = CallStatusProc(statusProc, kDBUpdateWind, noErr);
		}

		if (rc != noErr)
		{
			/* user or status function canceled query or error occurred */

			/* call status function one last time */
			rc = CallStatusProc(statusProc, kDBStartQueryComplete, rc);

			if (asyncPB)
				asyncPB->result = rc;
			return(rc);
		}

	}

	/* get pointer to QueryRecord */
	MyHLock((Handle)query, &currState);
	qp = *query;

	/********************************************/
	/* We've called the qdef, now we need to do	*/
	/* a DBInit if a session is not up yet.		*/
	/********************************************/

	/* make sure a valid query is specified */
	if ((qp->currQuery >= 1) && (qp->currQuery <= qp->numQueries))
	{
		/* query valid, check if session is up */
		if (*sessID == 0)
		{
			rc = noErr;

			/* call status function, if specified */
			rc = CallStatusProc(statusProc, kDBAboutToInit, noErr);

			if (rc == noErr)
			{
				if (asyncPB)
				{
					/* we have to set up our own parameter block */
					myPB = MakePB(NULL, asyncPB);
					myVars = (asyncVarsPtr)myPB->reserved;

					/* keep track of session ID */
					myVars->sessIDPtr = sessID;

					/* keep track of status function */
					myVars->statusProc = statusProc;

					/* keep track of query for DBSend */
					myVars->query = query;
					myVars->queryState = currState;

					/* get query now */
					queries = *(qp->queryList);
					myVars->queryText = queryText = queries[qp->currQuery - 1];
					MyHLock(queryText, &(myVars->textState));

					/* this is a high level call */
					myVars->highlevel = true;

					/* set current action */
					myVars->currAction = kDBInit;

					/* put parameter block into global list */
					InsertPB(myPB);

					/* make the call asynchronously and return */
					return(DBInit(sessID, qp->ddevName, qp->host, qp->user, qp->password,
							qp->connStr, myPB));
				}
				else
				{
					/* app made call synchronously */

					/* no session up yet, call DBInit with specified parameters */
					rc = DBInit(sessID, qp->ddevName, qp->host, qp->user, qp->password,
							qp->connStr, NULL);

					/* call status function, if specified */
					rc = CallStatusProc(statusProc, kDBInitComplete, rc);
				}
			}
		}
		else
		{
			/* session up already */
			rc = noErr;
		}

		/********************************************/
		/* DBInit is done, now do a DBSend to send 	*/
		/* the actual query.						*/
		/********************************************/

		if (rc == noErr)
		{
			/* session up, send query */

			/* get query now */
			queries = *(qp->queryList);
			queryText = queries[qp->currQuery - 1];
			MyHLock(queryText, &textState);
			len = *((short*)(*queryText));

			if (asyncPB != NULL)
			{
				/* we could get here if session was up and we didn't call DBInit */

				/* we have to set up our own parameter block */
				myPB = MakePB(NULL, asyncPB);
				myVars = (asyncVarsPtr)myPB->reserved;

				/* keep track of session ID */
				myVars->sessIDPtr = sessID;

				/* keep track of status function */
				myVars->statusProc = statusProc;

				/* keep track of query for DBSend */
				myVars->query = query;
				myVars->queryState = currState;

				/* keep track of query text for DBSend */
				myVars->queryText = queryText;
				myVars->textState = textState;

				/* this is a high level call */
				myVars->highlevel = true;

				/* set current action */
				myVars->currAction = kDBSend;

				/* put parameter block into global list */
				InsertPB(myPB);

				/* make the call asynchronously and return */
				return(DBSend(*sessID, *queryText + 2, len, myPB));
			}
			else
			{
				/* app made call synchronously */

				/* call DBSend with specified query */
				rc = DBSend(*sessID, *queryText + 2, len, asyncPB);

				/* restore state */
				HSetState(queryText, textState);

				/* call status function, if specified */
				rc = CallStatusProc(statusProc, kDBSendComplete, rc);
			}
		}
	}
	else
	{
		/* currQuery is not in a valid range */
		rc = rcDBError; /* do we need another return code here? */
	}

	/* clean up */
	HSetState((Handle)query, currState);

	/********************************************/
	/* DBSend is done, now do a DBExec.		 	*/
	/********************************************/

	if (rc == noErr)
	{
		/* everything is OK, execute query */
		/* if we get here, we are definitely synchronous */
		rc = DBExec(*sessID, asyncPB);

		/* call status function, if specified */
		rc = CallStatusProc(statusProc, kDBExecComplete, rc);
	}

	/* call status function one last time */
	return(CallStatusProc(statusProc, kDBStartQueryComplete, rc));

} /* SMStartQuery() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMGetQueryResults
 *
 * DESCRIPTION:	DBGetQueryResults routine.
 *
 * INPUTS:		sessID:		Session ID.
 *				results:	Pointer to results record.
 *				timeout:	Timeout value for DBGetItem.
 *				statusProc:	Pointer to user status function, if any.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		results:	Pointer to filled in results record.
 *
 * RETURNS:		userCanceledErr
 *				rcDBValue
 *				Errors from DBState, SetupResults, DBGetItem, and
 *				GrowResultsRecord.
 *
 * MODIFICATIONS:
 *	04-19-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMGetQueryResults(long sessID, ResultsRecord *results, long timeout,
	statusPtr statusProc, DBAsyncParmBlkPtr asyncPB)
{
	short		rc;
	DBType		dataType, *typeArray;
	long		currOffset, oldCurrOffset;		/* current offset into data */
	long		endLastRowData, endLastRowInfo;			/* end of last row */
	short		currCol;			/* current column */
	short		len, places, flags;
	short		retval;
	long		oldlen;
	Handle		typeH, dataH, infoH;
	Handle		oldTypeH, oldDataH, oldInfoH;
	Boolean		done, firsttime;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr	myVars;
	ResultsRecord	*oldResults;
	snarfSessElemHndl	snarf;
	DBColInfoRecord	*currInfo;

	if (asyncPB != NULL)
	{
		/* we have to set up our own parameter block */
		myPB = MakePB(NULL, asyncPB);
		myVars = (asyncVarsPtr)myPB->reserved;

		/* keep track of session ID */
		myVars->sessID = sessID;

		/* keep track of status function */
		myVars->statusProc = statusProc;

		/* keep track of ResultsRecord */
		myVars->results = results;

		/* this is a high level call */
		myVars->highlevel = true;

		/* set current action */
		myVars->currAction = kDBState;

		/* put parameter block into global list */
		InsertPB(myPB);

		/* make the call asynchronously and return */
		return(DBState(sessID, myPB));
	}

	/* app made call synchronously */
	/* check the state of the current session */
	rc = DBState(sessID, NULL);

	/* we only care if there is data to return */
	if (rc != rcDBValue)
	{
		/* call status function one last time */
		return(CallStatusProc(statusProc, kDBGetQueryResultsComplete, rc));
	}

#ifdef DODPRINTF
	dprintf("DBState returned noErr");
#endif

	/* use common code to initialize results */
	rc = SetupResults(sessID, &snarf, results, &oldResults,
		&currOffset, &currCol);

	if (rc != noErr)
	{
		/* an error occurred initializing results */

		/* call status function one last time */
		return(CallStatusProc(statusProc, kDBGetQueryResultsComplete, rc));
	}

	/* current offset in old and user results is the same */
	oldCurrOffset = currOffset;

	/* save handles in local variables for convenience */
	typeH = results->colTypes;
	dataH = results->colData;
	infoH = results->colInfo;

	oldTypeH = oldResults->colTypes;
	oldDataH = oldResults->colData;
	oldInfoH = oldResults->colInfo;

	/* didn't go through a row yet */
	endLastRowData = 0;
	endLastRowInfo = 0;

	done = false;
	firsttime = true;

	do
	{
		 /* get information on current column without getting data */
		 dataType = typeAnyType;

/*
		 dprintf("DBGetItem info: colTypes len = %d, colData len = %d",
		 	GetHandleSize(oldTypeH), GetHandleSize(oldDataH));
		 dprintf("DBGetItem info: colLens len = %d",
		 	GetHandleSize(oldLenH));
*/

		 rc = DBGetItem(sessID, timeout, &dataType, &len, &places, &flags, NULL, NULL);

		 switch (rc)
		 {
			case rcDBNull:
			case rcDBValue:
				/* information retrieved successfully */
#ifdef DODPRINTF
				dprintf("DBGetItem for info - len = %d, dataType = %d, flags = %d", len,
					dataType, flags);
#endif
				/* grow old results first */
				if ((retval = GrowResultsRecord(oldResults, len,
					(endLastRowData == 0))) == noErr)
				{
					/* old results grown successfully, try growing user results */
					if ((retval = GrowResultsRecord(results, len,
						(endLastRowData == 0))) != noErr)
					{
						/* couldn't grow user results, resize old results */
						SetHandleSize(oldDataH, (long)(oldCurrOffset));
						SetHandleSize(oldInfoH, GetHandleSize(oldInfoH) -
							sizeof(DBColInfoRecord));
						if (endLastRowData == 0)
						{
							SetHandleSize(oldTypeH, GetHandleSize(oldTypeH) -
								sizeof(DBType));
						}
						done = true;
					}
				}
				else
					/* error growing old results */
					done = true;

				if (!done)
				{
					/* everything grown OK, update stuff */

					/* save info on current column */
					oldlen = GetHandleSize(oldInfoH) - sizeof(DBColInfoRecord);
					currInfo = (DBColInfoRecord*)(*oldInfoH + oldlen);
					currInfo->len = len;
					currInfo->places = places;
					currInfo->flags = flags;

					oldlen = GetHandleSize(infoH) - sizeof(DBColInfoRecord);
					currInfo = (DBColInfoRecord*)(*infoH + oldlen);
					currInfo->len = len;
					currInfo->places = places;
					currInfo->flags = flags;

					/* save data type, if necessary */
					if (endLastRowData == 0)
					{
						typeArray = (DBType*)(*oldTypeH);
						typeArray[currCol] = dataType;

						typeArray = (DBType*)(*typeH);
						typeArray[currCol] = dataType;
					}
				}
				break;

			case noErr:
				/* no more data */
#ifdef DODPRINTF
				dprintf("DBGetItem for info - got noErr");
#endif
				done = true;
				if (firsttime)
				{
					/* first time through, so query had no results */
					retval = noErr;
				}
				else
				{
					/* not first time, so we have data to return */
					retval = rcDBValue;
				}
				break;

			default:
				/* some other error */
				/* should only be rcDBError from DBGetItem */
#ifdef DODPRINTF
				dprintf("DBGetItem for info - got rc = %d", rc);
#endif
				done = true;
				retval = rc;
				break;
		}

		if (!done)
		{
			firsttime = false;		/* there's at least one column of data */

			/* got information on current column successfully, get actual column */
			HLock(dataH);
/*
		 dprintf("DBGetItem data: colTypes len = %d, colData len = %d",
		 	GetHandleSize(oldTypeH), GetHandleSize(oldDataH));
		 dprintf("DBGetItem data: colLens len = %d",
		 	GetHandleSize(oldLenH));
*/

			rc = DBGetItem(sessID, timeout, &dataType, &len, &places, &flags,
				*dataH + currOffset, NULL);

			/* call status function, if specified */
			if (statusProc != NULL)
			{
				if (!statusProc(kDBGetItemComplete, rc, len, places, flags, dataType,
					*dataH + currOffset))
				{
					/* status function cancelled */
					done = true;
					retval = userCanceledErr;
				}
			}

			HUnlock(dataH);

			switch (rc)
			{
				case rcDBNull:
				case rcDBValue:
					/* got data */
#ifdef DODPRINTF
					dprintf("DBGetItem - got data of dataType %d len %d, at %X",
						dataType, len, (long)*dataH + currOffset);
#endif
					/* copy data to old results */
					BlockMove(*dataH + currOffset, *oldDataH + oldCurrOffset,
						(long)len);

					/* update current position in data handle */
					currOffset += len;
					oldCurrOffset += len;

					/* check if end of row */
					if (flags & kDBLastColFlag)
					{
						/* yes, update currCol and endLastRows */
#ifdef DODPRINTF
						dprintf("DBGetItem - at end of row, col = %d", currCol);
#endif

						if (endLastRowData == 0)
						{
							/* just went through first row, update results */
							results->numCols = currCol + 1;
						}

						currCol = 0;
						endLastRowData = currOffset;
						endLastRowInfo = GetHandleSize(infoH);

						/* add one to the number of rows processed */
						results->numRows++;

						/* we got a complete row, shrink down old results */
						SetHandleSize(oldDataH, 0);
						SetHandleSize(oldInfoH, 0);
						oldCurrOffset = 0;

						/* keep the old types handle around */
					}
					else
					{
						/* still in same row */
						currCol++;
					}
					break;

				default:
					/* some other error occurred from DBGetItem */
#ifdef DODPRINTF
					dprintf("DBGetItem - other error %d", rc);
#endif
					done = true;
					retval = rc;
					break;
			}
		}

	} while (!done);

	if (retval != rcDBValue)
	{
		/* some error occurred, either out of space or ddev error */
		/* or status function cancelled */
		/* need to back up to last full row */

		if (endLastRowData == 0)
		{
			/* didn't get one full row, need to dispose of everything */
			DisposeResultsRecord(results);

			results->numRows = 0;
			results->numCols = 0;
		}
		else
		{
			/* got at least one row */
			/* adjust data and info handle */
			SetHandleSize(dataH, endLastRowData);
			SetHandleSize(infoH, endLastRowInfo);
		}

		/* update currCol in snarf elem */
		(**snarf).currCol = currCol;

		/* roll back types array to current column */
		SetHandleSize(oldTypeH, (long)((currCol+1) * sizeof(DBType)));
	}
	else
	{
		/* we got all the data successfully, so we don't need the old results */
		DisposeResultsRecord(oldResults);
		DisposPtr((Ptr)oldResults);
		(**snarf).oldResults = NULL;
		(**snarf).currCol = 0;
	}

#ifdef DODPRINTF
	dprintf("SMGetQueryResults exit - rc = %d, results = %X", rc, results);
#endif

	/* call status function one last time */
	retval = CallStatusProc(statusProc, kDBGetQueryResultsComplete, retval);

	return(retval);

} /* SMGetQueryResults() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMKill
 *
 * DESCRIPTION:	DBKill routine.  Calls DBKill with specified parameters.
 *
 * INPUTS:		asyncPB:	Pointer to asynchronous parameter block.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		rcDBBadAsyncPB
 *				rcDBError
 *				Errors from CallDDEV.
 *
 * MODIFICATIONS:
 *	07-28-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMKill(DBAsyncParmBlkPtr asyncPB)
{
	register short		rc;
	DBAsyncParmBlkPtr	realPB, otherPB;
	asyncVarsPtr		myVars;
	ddevElemHndl		ddevH;
	Boolean				highlevel;
	Boolean				closeDDEV;
	DDEVParams			params;

	/* assume bad asyncPB */
	rc = rcDBBadAsyncPB;

	if (asyncPB)
	{
		/* try to find asyncPB embedded in our list of PBs */
		realPB = FindAppPB(asyncPB);
		if (realPB)
		{
			/* got the actual parameter block */
			myVars = (asyncVarsPtr)realPB->reserved;

			if (highlevel = myVars->highlevel)
			{
				/* we've found the pb for StartQuery or GetQueryResults */
				/* now, we need the pb for the actual call to the ddev */
				otherPB = realPB;
				realPB = FindAppPB(otherPB);

				if (realPB)
				{
					/* get the right variables */
					myVars = (asyncVarsPtr)realPB->reserved;
				}
			}

			if (realPB)
			{
				/* make sure call is still executing before killing it */
				if (myVars->executing)
				{

					/* fill in parameter block for DBKill */
					ClearParams(params);
					params.message = kDBKill;
					params.asyncPB = realPB;

					/* session ID not needed, so just pass 0 */
					/* call ddev synchronously - DBKill */
					rc = CallDDEV(((asyncVarsPtr)realPB->reserved)->ddevH, &params);

					if (rc == noErr)
					{
						/* copy over the result, just in case */
						asyncPB->result = realPB->result;

						/* check if this is a DBInit, if so, we have to
							close ddev if no sessions are up */
						if (closeDDEV = ((myVars->currAction == kDBInit) &&
										(myVars->firstsess)))
						{
							/* save ddev handle to close after removing pbs */
							ddevH = myVars->ddevH;
						}

						if (highlevel)
						{
							/* we just killed the actual PB, need to
								remove the high level PB */
							RemovePB(otherPB);
						}

						/* remove pb from list */
						RemovePB(realPB);

						/* close ddev, if necessary */
						if (closeDDEV)
							(void)SMClose(ddevH);
					}
				}
				else
				{
					/* call has finished executing, we just haven't processed it
						yet in DBIdle, so return an error ?? */
					rc = rcDBError;
				}
			}
		}
	}

	return(rc);

} /* SMKill() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMResultsToText
 *
 * DESCRIPTION:	DBResultsToText routine.  Converts specified ResultsRecord to text.
 *
 * INPUTS:		results:	Pointer to ResultsRecord returned by DBGetQueryResults.
 *
 * OUTPUTS:		theText:	Handle allocated with converted text.
 *
 * RETURNS:		Errors from NewHandle and the result handler.
 *
 * MODIFICATIONS:
 *	08-03-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMResultsToText(ResultsRecord *results, Handle *theText)
{
	Handle				textH;
	HandlerPtr			theHandler, colHandler, rowHandler, unknHandler;
	DBType				*typeArray;
	Ptr					buffer;
	DBColInfoRecord		*infoArray;
	short				currRow, currCol;
	short				rc = noErr;
	char				typeState, dataState, infoState;

	/* assume failure */
	*theText = NULL;

	/* check if this results records contains any results and is valid */
	if ((results->numRows == 0) || (results->numCols == 0) ||
		(results->colTypes == NULL) || (results->colData == NULL) ||
		(results->colInfo == NULL))
	{
		return(noErr);
	}

	/* start with empty handle */
	textH = NewHandle(NULL);
	if (textH == NULL)
		rc = MemError();

	if (rc == noErr)
	{
		/* lock everything down */
		MyHLock(results->colTypes, &typeState);
		MyHLock(results->colData, &dataState);
		MyHLock(results->colInfo, &infoState);

		/* get pointers to all the arrays */
		typeArray = (DBType*)(*(results->colTypes));
		buffer = (*(results->colData));
		infoArray = (DBColInfoRecord*)(*(results->colInfo));

		/* prefetch the unknown, column break, and row break handlers */
		unknHandler = GetCorrectResultHandler(typeUnknown);
		colHandler = GetCorrectResultHandler(typeColBreak);
		rowHandler = GetCorrectResultHandler(typeRowBreak);

		/* start at the beginning */
		currRow = 0;

		/* do all the rows */
		while ((currRow < results->numRows) && (rc == noErr))
		{
			/* do all the columns in this row */
			currCol = 0;
			while ((currCol < results->numCols) && (rc == noErr))
			{
				/* try to get the application handler for the type of data in this column */
				theHandler = GetCorrectResultHandler(typeArray[currCol]);

				if (theHandler == NULL)
				{
					/* none installed for this type, use the unknown handler */
					theHandler = unknHandler;
				}

				/* call the handler to do the conversion */
				rc = (*theHandler)(typeArray[currCol], infoArray->len, infoArray->places,
						infoArray->flags, buffer, textH);

				/* adjust pointer into buffer */
				buffer += infoArray->len;

				/* adjust pointer into DBColInfoRecord array */
				infoArray++;

				/* go to next column */
				currCol++;

				/* see if we're at the end of a row */
				if ((currCol < results->numCols) && (rc == noErr))
				{
					/* no, do column break */
					rc = (*colHandler)(typeColBreak, 0, NULL, NULL, NULL, textH);
				}

			} /* while ((currCol < results->numCols) && (rc == noErr)) */

			/* go to next row */
			currRow++;

			/* do row break */
			if (rc == noErr)
			{
				rc = (*rowHandler)(typeRowBreak, 0, NULL, NULL, NULL, textH);
			}

		} /* while ((currRow < results->numRows) && (rc == noErr)) */

		/* clean up */
		*theText = textH;
		HSetState(results->colTypes, typeState);
		HSetState(results->colData, dataState);
		HSetState(results->colInfo, infoState);
	}

	return(rc);

} /* SMResultsToText() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		GetCorrectResultHandler
 *
 * DESCRIPTION:	Get the specified result handler.  Tries to get application handler first,
 *				then system handler if there is no application handler.
 *
 * INPUTS:		dataType:		Data type for handler.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		theHandler:		Handler ProcPtr.
 *
 * MODIFICATIONS:
 *	04-02-90	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
HandlerPtr
GetCorrectResultHandler(DBType dataType)
{
	ProcPtr				p;

	/* check for application handlers first */
	DBGetResultHandler(dataType, &p, false);
	if (p == NULL)
	{
		/* no application handler, get system handler */
		DBGetResultHandler(dataType, &p, true);
	}

	return((HandlerPtr)p);

} /* GetCorrectResultHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMInstallResultHandler
 *
 * DESCRIPTION:	DBInstallResultHandler routine.  Installs a results
 *				handler for the specified data type.
 *
 * INPUTS:		dataType:		Data type for this handler.
 *				theHandler:		Pointer to handler.
 *				isSysHandler:	If TRUE, install this as a system handler,
 *								otherwise handler is for current application.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from NewHandleSys and InsertHandler.
 *
 * MODIFICATIONS:
 *	08-03-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMInstallResultHandler(DBType dataType, ProcPtr theHandler, Boolean isSysHandler)
{
	snarfGlobPtr	glob;
	HandlerRecord	*hArray;
	short			currIndex;
	HandlerRecord	newRec, *arrayHandler, **newHandler;
	short			rc;
	char			currState;
	Boolean			foundit;

	/* assume success */
	rc = noErr;

	/* set up new record to put in array */
	newRec.type = dataType;
	newRec.theHandler = (HandlerPtr)theHandler;
	newRec.next = (HandlerRecord**)NULL;
	newRec.hZone = GetZone();
	newRec.isSysHandler = isSysHandler;

	/* see if there already is any handler for this type */
	if ((currIndex = FindHandler(dataType)) != -1)
	{
		/* yes, get the handler in the array */

		/* get a pointer to the array */
		glob = GetGlobals();
		MyHLock((Handle)glob->handlers, &currState);
		hArray = *(glob->handlers);

		/* arrayHandler is the handler in the array, that is,
			the first handler for this type */
		arrayHandler = &hArray[currIndex];

		if (isSysHandler)
		{
			/* installing a system handler */
			/* if there is a system handler, it is always first in the list */
			if (!(arrayHandler->isSysHandler))
			{
				/* first handler is an app handler, need to move it down
					the list, so duplicate it into a new handle */
				newHandler = (HandlerRecordHandle)NewHandleSysClear(
					sizeof(HandlerRecord));
				if ((rc = MemError()) == noErr)
				{
					BlockMove((Ptr)arrayHandler, (Ptr)*newHandler,
						sizeof(HandlerRecord));

					/* set the next field of system handler record */
					newRec.next = newHandler;
				}
			}
			else
			{
				/* replacing current system handler, set next field */
				newRec.next = arrayHandler->next;
			}

			/* move in the new system handler */
			if (rc == noErr)
			{
				BlockMove((Ptr)&newRec, (Ptr)arrayHandler, sizeof(HandlerRecord));
			}
		}
		else
		{
			/* installing an application handler */

			/* try and find a current handler for this app */

			/* check the array handler first */
			if (!(arrayHandler->isSysHandler) &&
				(arrayHandler->hZone != newRec.hZone))
			{
				/* handler in array is the current app handler */
				/* just set the proc ptr */
				arrayHandler->theHandler = (HandlerPtr)theHandler;
			}
			else
			{
				/* have to search list of handlers */
				newHandler = arrayHandler->next;
				foundit = false;

				while (newHandler && !foundit)
				{
					if ((**newHandler).hZone == newRec.hZone)
					{
						/* found current app handler */
						foundit = true;
					}
					else
						newHandler = (**newHandler).next;
				}

				if (foundit)
				{
					/* an app handler already exists, just set the proc ptr */
					(**newHandler).theHandler = (HandlerPtr)theHandler;
				}
				else
				{
					/* new handler for current app, insert one after array handler */
					newRec.next = arrayHandler->next;
					newHandler = (HandlerRecordHandle)NewHandleSysClear(
						sizeof(HandlerRecord));
					if ((rc = MemError()) == noErr)
					{
						BlockMove((Ptr)&newRec, (Ptr)*newHandler,
							sizeof(HandlerRecord));
						arrayHandler->next = newHandler;
					}
				}
			}
		}

		/* restore state of handler array */
		HSetState((Handle)glob->handlers, currState);
	}
	else
	{
		/* no handler for this type, install new handler into the array */
		rc = InsertHandler(&newRec);
	}

	return(rc);

} /* SMInstallResultHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMRemoveResultHandler
 *
 * DESCRIPTION:	DBRemoveResultHandler routine.  Remove a result
 *				handler for the specified data type.
 *
 * INPUTS:		dataType:		Data type for handler to remove.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		rcDBNoHandler
 *				Errors from DisposHandle, and RemoveHandler.
 *
 * MODIFICATIONS:
 *	08-03-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMRemoveResultHandler(DBType dataType)
{
	short			index;
	snarfGlobPtr	glob;
	HandlerRecord	*hArray, **newHandler, **lastHandler;
	THz				hZone;
	short			rc;
	Boolean			foundit;
	char			currState;

	/* assume failure */
	rc = rcDBNoHandler;

	/* find the array entry for this data type */
	index = FindHandler(dataType);

	if (index == -1)
	{
		/* no handler (system or app) installed */
		return(rcDBNoHandler);
	}

	/* get specific handler element */
	glob = GetGlobals();
	MyHLock((Handle)(glob->handlers), &currState);
	hArray = *(glob->handlers);

	/* get current application's zone */
	hZone = GetZone();

	/* check the array handler first */
	if (!(hArray[index].isSysHandler) &&
		(hArray[index].hZone == hZone))
	{
		/* handler in array is the current app handler */

		/* see if there are any other handlers left */
		if (newHandler = hArray[index].next)
		{
			/* yes, just move the next one into the array */
			BlockMove((Ptr)*newHandler, (Ptr)&(hArray[index]),
				sizeof(HandlerRecord));

			DisposHandle((Handle)newHandler);
			rc = MemError();
		}
		else
		{
			/* we're removing the only handler for this data type */
			HSetState((Handle)(glob->handlers), currState);
			rc = RemoveHandler(index);
		}
	}
	else
	{
		/* handler in array isn't the right one, check list */
		newHandler = hArray[index].next;
		lastHandler = NULL;
		foundit = false;

		while (newHandler && !foundit)
		{
			if ((**newHandler).hZone == hZone)
			{
				/* found current app handler */
				foundit = true;
			}
			else
			{
				lastHandler = newHandler;
				newHandler = (**newHandler).next;
			}
		}

		if (foundit)
		{
			/* found the app handler in the list off the array */

			/* fix up list */
			if (lastHandler)
			{
				(**lastHandler).next = (**newHandler).next;
			}
			else
			{
				/* removing first handler in list */
				hArray[index].next = (**newHandler).next;
			}

			/* get rid of handler record */
			DisposHandle((Handle)newHandler);
			rc = MemError();
		}
	}

	HSetState((Handle)(glob->handlers), currState);

	return(rc);

} /* SMRemoveResultHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMGetResultHandler
 *
 * DESCRIPTION:	DBGetResultHandler routine.  Returns a pointer to
 *				the result handler for the specified data type.  getSysHandler
 *				specifies which handler to return.
 *
 * INPUTS:		dataType:		Data type for handler.
 *				getSysHandler:	If TRUE, return system handler, if any.
 *								Otherwise, return app handler, if any.
 *
 * OUTPUTS:		theHandler:		Pointer to handler.
 *
 * RETURNS:		rcDBNoHandler
 *
 * MODIFICATIONS:
 *	08-03-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal OSErr
SMGetResultHandler(DBType dataType, ProcPtr *theHandler, Boolean getSysHandler)
{
	short			index;
	snarfGlobPtr	glob;
	HandlerRecord	*hArray, **newHandler;
	short			rc;
	THz				hZone;
	Boolean			foundit;
	char			currState;

	/* assume failure */
	*theHandler = NULL;
	rc = rcDBNoHandler;

	/* find the array entry for this data type */
	index = FindHandler(dataType);

	if (index == -1)
	{
		/* no handler (system or app) installed */
		return(rcDBNoHandler);
	}

	/* get specific handler element */
	glob = GetGlobals();
	MyHLock((Handle)(glob->handlers), &currState);
	hArray = *(glob->handlers);

	if (getSysHandler)
	{
		/* wants system handler, always first one if it exists */
		if (hArray[index].isSysHandler)
		{
			/* this is it */
			*theHandler = (ProcPtr)(hArray[index].theHandler);
			rc = noErr;
		}
	}
	else
	{
		/* wants current application's handler */
		hZone = GetZone();

		/* check the array handler first */
		if (!(hArray[index].isSysHandler) &&
			(hArray[index].hZone == hZone))
		{
			/* handler in array is the current app handler */
			*theHandler = (ProcPtr)(hArray[index].theHandler);
			rc = noErr;
		}
		else
		{
			/* have to search list of handlers */
			newHandler = hArray[index].next;
			foundit = false;

			while (newHandler && !foundit)
			{
				if ((**newHandler).hZone == hZone)
				{
					/* found current app handler */
					foundit = true;
				}
				else
					newHandler = (**newHandler).next;
			}

			if (foundit)
			{
				/* found the app handler in the list off the array */
				*theHandler = (ProcPtr)((**newHandler).theHandler);
				rc = noErr;
			}
		}
	}

	HSetState((Handle)(glob->handlers), currState);

	return(rc);

} /* SMGetResultHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		FindHandler
 *
 * DESCRIPTION:	Do a binary search of the array of HandlerRecords for the
 *				handler of the specified type.  Returns array index of handler,
 *				or -1 if not found.
 *
 * INPUTS:		dataType:		Data type for handler to find.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Index into array of handler, or -1 if not found.
 *
 * MODIFICATIONS:
 *	08-06-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
short
FindHandler(DBType dataType)
{
	snarfGlobPtr	glob;
	HandlerRecord	*hArray;
	short			curr, first, last, rc;
	char			currState;

	glob = GetGlobals();

	/* check for empty array */
	if ((last = glob->numHandlers) == 0)
	{
		rc = -1;
	}
	else
	{
		/* lock down array */
		MyHLock((Handle)glob->handlers, &currState);
		hArray = *(glob->handlers);

		/* start with whole array */
		first = 0;

		do
		{
			curr = (first + last) >> 1;
			if (dataType < hArray[curr].type)
				/* in first half */
				last = curr - 1;
			else
				/* in second half */
				first = curr + 1;
		} while ((dataType != hArray[curr].type) && (first <= last));

		if (dataType == hArray[curr].type)
			rc = curr;
		else
			rc = -1;

		HSetState((Handle)glob->handlers, currState);
	}

	return(rc);

} /* FindHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		InsertHandler
 *
 * DESCRIPTION:	Insert specified HandlerRecord in array of handlers.
 *
 * INPUTS:		newRec:		HandlerRecord to insert.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from SetHandleSize.
 *
 * MODIFICATIONS:
 *	08-06-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
InsertHandler(HandlerRecord *newRec)
{
	snarfGlobPtr	glob;
	HandlerRecord	*hArray;
	short			rc;
	short			numHandlers, i;
	char			currState;

	/* find where to put new record */
	glob = GetGlobals();
	MyHLock((Handle)glob->handlers, &currState);
	hArray = *(glob->handlers);

	i = 0;
	numHandlers = glob->numHandlers;
	while ((i < numHandlers) && (hArray[i].type < newRec->type))
		i++;

	/* grow array */
	HSetState((Handle)glob->handlers, currState);

	SetHandleSize((Handle)glob->handlers, GetHandleSize((Handle)glob->handlers)
		+ sizeof(HandlerRecord));

	if ((rc = MemError()) == noErr)
	{
		hArray = *(glob->handlers);

		/* see if we have to shift array */
		if (i != numHandlers)	/* are we inserting at the end? */
		{
			/* no, have to move some records down */
			BlockMove((Ptr)&(hArray[i]), (Ptr)&(hArray[i+1]), (numHandlers - i) *
				sizeof(HandlerRecord));
		}

		/* move in new record */
		BlockMove((Ptr)newRec, (Ptr)&(hArray[i]), sizeof(HandlerRecord));

		glob->numHandlers++;
	}

	return(rc);

} /* InsertHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		RemoveHandler
 *
 * DESCRIPTION:	Remove handler at specified index in array.
 *
 * INPUTS:		index:		Index of record to remove.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from SetHandleSize.
 *
 * MODIFICATIONS:
 *	08-09-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
RemoveHandler(short index)
{
	snarfGlobPtr	glob;
	HandlerRecord	*hArray;

	/* find record to delete */
	glob = GetGlobals();

	if (index != (glob->numHandlers - 1))
	{
		/* we're not removing the last record in the array,
			need to move other elements up */
		hArray = *(glob->handlers);
		BlockMove((Ptr)&(hArray[index+1]), (Ptr)&(hArray[index]),
			sizeof(HandlerRecord) * (glob->numHandlers - index - 1));
	}

	/* decrement number of handlers */
	glob->numHandlers--;

	/* resize handle */
	SetHandleSize((Handle)glob->handlers, GetHandleSize((Handle)glob->handlers)
		- sizeof(HandlerRecord));

	return(MemError());

} /* RemoveHandler() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SMGeneric
 *
 * DESCRIPTION:	Generic routine to invoke ddev with specified parameter block.
 *				Converts snarfID to the ddev's session ID, and fills in
 *				the sessID and asyncPB field of the DDEVParams.  We do this
 *				all here to save code.  Note: if caller gets back
 *				rcDBBadSessID then ddev was not called.
 *
 * INPUTS:		snarfID:	Snarf ID of session.
 *				asyncPB:	Pointer to asynchronous parameter block.
 *				params:		Pointer to filled in parameter block for ddev.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		rcDBBadSessID
 *				Errors from CallDDEV.
 *
 * MODIFICATIONS:
 *	03-27-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
SMGeneric(long snarfID, DBAsyncParmBlkPtr asyncPB, DDEVParams *params)
{
	snarfGlobPtr 		glob;
	ddevElemHndl		ddevH;
	snarfSessElemHndl	snarf;
	sessElemHndl		sess;
	short				rc;

	glob = GetGlobals();

	/* find snarf element with specified id */
	if ((snarf = FindSnarfID(snarfID, glob->snarfRoot)) == NULL)
	{
		/* bad session ID */
		rc = rcDBBadSessID;
	}
	else
	{
		/* got snarf element, get session and ddev info */
		sess = (**snarf).sess;
		ddevH = (**sess).parent;

		/* fill in real session ID */
		params->sessID = (**sess).sessID;

		/* fill in asyncPB pointer */
		params->asyncPB = asyncPB;

		/* call ddev with specified parameter block */
		rc = CallDDEV(ddevH, params);
	}

	return(rc);

} /* SMGeneric() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		InitDoneProc
 *
 * DESCRIPTION:	Routine called after DBInit for an asynchronous DBStartQuery
 *				completes.  If DBInit was successful, calls DBSend.
 *
 * INPUTS:		myPB:		Async parameter block.
 *				myVars:		Async variables.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		userCanceledErr
 *				Errors from just finished DBInit and DBSend.
 *
 * MODIFICATIONS:
 *	06-21-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
InitDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars)
{
	Handle			queryText, *queries;
	short			len;
	OSErr			rc;

	/* call status function, if specified */
	rc = CallStatusProc(myVars->statusProc, kDBInitComplete, myPB->result);

	/* check if successful */
	if (rc == noErr)
	{
		/* DBInit completed successfully, call DBSend */

		/* get query text */
		queries = *((**(myVars->query)).queryList);
		myVars->queryText = queryText =
			queries[(**(myVars->query)).currQuery - 1];
		MyHLock(queryText, &(myVars->textState));
		len = *((short*)(*queryText));

		/* we are about to be executing */
		myVars->executing = true;

		/* get next action */
		myVars->currAction = kDBSend;

		/* call DBSend asynchronously */
		rc = DBSend(*(myVars->sessIDPtr), *queryText + 2, len, myPB);
	}

	myPB->result = rc;
	return(rc);

} /* InitDoneProc() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SendDoneProc
 *
 * DESCRIPTION:	Routine called after DBSend for an asynchronous DBStartQuery
 *				completes.  If DBSend was successful, calls DBExec.
 *
 * INPUTS:		myPB:		Async parameter block.
 *				myVars:		Async variables.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		userCanceledErr
 *				Errors from just finished DBSend and DBExec.
 *
 * MODIFICATIONS:
 *	06-21-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
SendDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars)
{
	OSErr	rc;

	/* call status function, if specified */
	rc = CallStatusProc(myVars->statusProc, kDBSendComplete, myPB->result);

	/* check if successful */
	if (rc == noErr)
	{
		/* DBSend completed successfully, call DBExec */

		/* we are about to be executing */
		myVars->executing = true;

		/* get next action */
		myVars->currAction = kDBExec;

		/* call DBExec asynchronously */
		rc = DBExec(*(myVars->sessIDPtr), myPB);
	}

	myPB->result = rc;
	return(rc);

} /* SendDoneProc() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		ExecDoneProc
 *
 * DESCRIPTION:	Routine called after DBExec for an asynchronous DBStartQuery
 *				completes.  Just returns the result.
 *
 * INPUTS:		myPB:		Async parameter block.
 *				myVars:		Async variables.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		userCanceledErr
 *				Errors from just finished DBExec.
 *
 * MODIFICATIONS:
 *	06-21-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
ExecDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars)
{
	/* call status function, if specified */
	myPB->result = CallStatusProc(myVars->statusProc, kDBExecComplete, myPB->result);

	/* this was the last call we needed to do */
	/* but we don't need to set currAction since SMIdle knows this */
	/* myVars->currAction = kAllDone; */

	/* just return result from DBExec */
	return(myPB->result);

} /* ExecDoneProc() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		StateDoneProc
 *
 * DESCRIPTION:	Routine called after DBState for an asynchronous
 *				DBGetQueryResults completes.  If DBState was successful
 *				and there is data to return, calls DBGetItem.
 *
 * INPUTS:		myPB:		Async parameter block.
 *				myVars:		Async variables.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Errors from just finished DBState, SetupResults,
 *				and DBGetItem.
 *
 * MODIFICATIONS:
 *	06-21-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
StateDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars)
{
	short			rc;

	/* we only care if there is data to return */
	if (myPB->result == rcDBValue)
	{
		/* program complete, possibly results available */

		/* use common code to initialize results */
		rc = SetupResults(myVars->sessID, &(myVars->snarf), myVars->results,
			&(myVars->oldResults), &(myVars->currOffset), &(myVars->currCol));

		if (rc == noErr)
		{
			/* current offset in old and user results is the same */
			myVars->oldCurrOffset = myVars->currOffset;

			/* didn't go through a row yet */
			myVars->endLastRowData = 0;
			myVars->endLastRowInfo = 0;

			myVars->firsttime = true;

			/* set up to call DBGetItem */
			myVars->executing = true;

			/* get next action */
			myVars->currAction = kDBGetItem;

			/* we are about to get info on the current column */
			myVars->doingGetItemInfo = true;

			/* get information on current column without getting data */
			myVars->dataType = typeAnyType;
			rc = DBGetItem(myVars->sessID, myVars->timeout, &(myVars->dataType),
				&(myVars->len), &(myVars->places), &(myVars->flags), NULL, myPB);
		}

		if (rc != noErr)
		{
			/* error occurred in SetupResults or DBGetItem, set up for return */
			myPB->result = rc;
		}
	}
	else
	{
		/* no data or some other error */
		rc = myPB->result;
		if (rc == noErr)
		{
			/*
				There's no data to return, but we have to call AllDoneProc here
				since idle considers noErr to mean that we started the DBGetItem
				successfully.
			*/
			(void)AllDoneProc(myPB, myVars);
		}
	}

	return(rc);

} /* StateDoneProc() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		GetItemDoneProc
 *
 * DESCRIPTION:	Routine called after DBGetItem for an asynchronous
 *				DBGetQueryResults completes.  Note that DBGetItem can
 *				either be called with buffer set to NULL, indicating that
 *				we wanted infomation on the current column only, or with
 *				buffer set to the value passed into DBGetQueryResults,
 *				indicating we got the actual data.  In the latter case,
 *				if more data is available, we call DBGetItem again, otherwise
 *				we indicate to SMIdle that the call has completed.
 *
 * INPUTS:		myPB:		Async parameter block.
 *				myVars:		Async variables.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		rcDBValue
 *				Errors from just finished DBGetItem, GrowResultsRecord, and
 *				next DBGetItem.
 *
 * MODIFICATIONS:
 *	08-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
GetItemDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars)
{
	short			rc, oldlen;
	ResultsRecord	*results, *oldResults;
	DBType			*typeArray;
	DBColInfoRecord	*currInfo;

	/* see if the DBGetItem that just completed was for info or the
		actual data */
	if (myVars->doingGetItemInfo)
	{
		/* just got info */

		switch (myPB->result)
		{
			case rcDBNull:
			case rcDBValue:
				/* information retrieved successfully */
#ifdef DODPRINTF
				dprintf("DBGetItem info - len = %d, dataType = %d, flags = %d", myVars->len,
					myVars->dataType, myVars->flags);
#endif
				/* grow old results first */
				oldResults = myVars->oldResults;
				if ((rc = GrowResultsRecord(oldResults, myVars->len,
					(myVars->endLastRowData == 0))) == noErr)
				{
					/* old results grown successfully, try growing user results */
					results = myVars->results;
					if ((rc = GrowResultsRecord(results, myVars->len,
						(myVars->endLastRowData == 0))) != noErr)
					{
						/* couldn't grow user results, resize old results */
						SetHandleSize(oldResults->colData, myVars->oldCurrOffset);
						SetHandleSize(oldResults->colInfo,
							GetHandleSize(oldResults->colInfo) - sizeof(DBColInfoRecord));
						if (myVars->endLastRowData == 0)
						{
							SetHandleSize(oldResults->colTypes,
								GetHandleSize(oldResults->colTypes) -
								sizeof(DBType));
						}
					}
				}

				if (rc != noErr)
				{
					/* error resizing results */
					myPB->result = rc;
					return(myPB->result);
				}

				/* everything grown OK, update stuff */

				/* save info on current column */
				oldlen = GetHandleSize(oldResults->colInfo) - sizeof(DBColInfoRecord);
				currInfo = (DBColInfoRecord*)(*(oldResults->colInfo) + oldlen);
				currInfo->len = myVars->len;
				currInfo->places = myVars->places;
				currInfo->flags = myVars->flags;

				oldlen = GetHandleSize(results->colInfo) - sizeof(DBColInfoRecord);
				currInfo = (DBColInfoRecord*)(*(results->colInfo) + oldlen);
				currInfo->len = myVars->len;
				currInfo->places = myVars->places;
				currInfo->flags = myVars->flags;

				/* save data type, if necessary */
				if (myVars->endLastRowData == 0)
				{
					typeArray = (DBType*)(*(oldResults->colTypes));
					typeArray[myVars->currCol] = myVars->dataType;

					typeArray = (DBType*)(*(results->colTypes));
					typeArray[myVars->currCol] = myVars->dataType;
				}
				break;

			case noErr:
				/* no more data */
#ifdef DODPRINTF
				dprintf("DBGetItem info - got noErr");
#endif
				/* get next action */
				myVars->currAction = kAllDone;

				if (myVars->firsttime)
				{
					/* first time through, so query had no results */
					myPB->result = noErr;
				}
				else
				{
					/* not first time, so we have data to return */
					myPB->result = rcDBValue;
				}

				return(noErr);
				break;

			default:
				/* some other error */
				/* should only be rcDBError from DBGetItem */
#ifdef DODPRINTF
				dprintf("DBGetItem info - got rc = %d", myPB->result);
#endif
				/* get next action */
				myVars->currAction = kAllDone;

				return(myPB->result);
				break;
		}

		/* got here if rcDBNull or rcDBValue */
		myVars->firsttime = false;		/* there's at least one column of data */

		/* we are about to get the actual column, not just the info */
		myVars->doingGetItemInfo = false;

		/* we are about to be executing */
		myVars->executing = true;

		/* action still the same */
		/* myVars->currAction = kDBGetItem; */

/*
		 dprintf("DBGetItem data: colTypes len = %d, colData len = %d",
		 	GetHandleSize(oldResults->colTypes), GetHandleSize(oldResults->colData));
		 dprintf("DBGetItem data: colLens len = %d",
		 	GetHandleSize(oldResults->colLens));
*/

		/* got information on current column successfully, get actual column */
		HLock(results->colData);
		rc = DBGetItem(myVars->sessID, myVars->timeout, &(myVars->dataType),
			&(myVars->len), &(myVars->places), &(myVars->flags),
			*(results->colData) + myVars->currOffset, myPB);

		if (rc != noErr)
		{
			/* error starting DBGetItem, set up for return */
			myPB->result = rc;
		}

		return(rc);
	}
	else
	{
		/* just got actual column */
		results = myVars->results;
		oldResults = myVars->oldResults;

		/* save result of DBGetItem */
		rc = myPB->result;

		/* call status function, if specified */
		if (myVars->statusProc != NULL)
		{
			if (!(myVars->statusProc)(kDBGetItemComplete, myPB->result,
					myVars->len, myVars->places, myVars->flags, myVars->dataType, *(results->colData)
					+ myVars->currOffset))
			{
				/* status function cancelled */
				rc = userCanceledErr;
			}
		}

		/* unlock data handle */
		HUnlock(results->colData);

		switch (myPB->result)
		{
			case rcDBNull:
			case rcDBValue:
				/* got data */
#ifdef DODPRINTF
				dprintf("DBGetItem - got data of dataType %d len %d, at %X",
					myVars->dataType, myVars->len, (long)*(results->colData) + currOffset);
#endif
				/* copy data to old results */
				BlockMove(*(results->colData) + myVars->currOffset,
					*(oldResults->colData) + myVars->oldCurrOffset,
					(long)myVars->len);

				/* update current position in data handle */
				myVars->currOffset += myVars->len;
				myVars->oldCurrOffset += myVars->len;

				/* check if end of row */
				if (myVars->flags & kDBLastColFlag)
				{
					/* yes, update currCol and endLastRows */
#ifdef DODPRINTF
					dprintf("DBGetItem - at end of row, col = %d", myVars->currCol);
#endif

					if (myVars->endLastRowData == 0)
					{
						/* just went through first row, update results */
						results->numCols = myVars->currCol + 1;
					}

					myVars->currCol = 0;
					myVars->endLastRowData = myVars->currOffset;
					myVars->endLastRowInfo = GetHandleSize(results->colInfo);

					/* add one to the number of rows processed */
					results->numRows++;

					/* we got a complete row, shrink down old results */
					SetHandleSize(oldResults->colData, 0);
					SetHandleSize(oldResults->colInfo, 0);
					myVars->oldCurrOffset = 0;
				}
				else
				{
					/* still in same row */
					myVars->currCol++;
				}

				if (rc != userCanceledErr)
				{
					/* set up to call DBGetItem */
					myVars->executing = true;

					/* action still the same */
					/* myVars->currAction = kDBGetItem; */

					/* we are about to get info on the current column */
					myVars->doingGetItemInfo = true;

/*
		 dprintf("DBGetItem info: colTypes len = %d, colData len = %d",
		 	GetHandleSize(oldResults->colTypes), GetHandleSize(oldResults->colData));
		 dprintf("DBGetItem info: colLens len = %d",
		 	GetHandleSize(oldResults->colLens));
*/

					/* get information on current column without getting data */
					myVars->dataType = typeAnyType;
					rc = DBGetItem(myVars->sessID, myVars->timeout, &(myVars->dataType),
						&(myVars->len), &(myVars->places), &(myVars->flags), NULL, myPB);
				}

				if (rc != noErr)
				{
					/* error starting DBGetItem or status function cancelled, set up for return */
					myPB->result = rc;
				}

				return(rc);
				break;

			default:
				/* some other error occurred from DBGetItem */
#ifdef DODPRINTF
				dprintf("DBGetItem - other error %d", myPB->result);
#endif
				/* save result in case status function changed it */
				myPB->result = rc;
				return(myPB->result);
				break;
		}
	}

} /* GetItemDoneProc() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		AllDoneProc
 *
 * DESCRIPTION:	State routine called after all routines for an asynchronous call
 *				to DBStartQuery or DBGetQueryResults have completed,
 *				or a routine ends in an error.
 *
 * INPUTS:		myPB:		Async parameter block.
 *				myVars:		Async variables.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		noErr always.
 *
 * MODIFICATIONS:
 *	06-21-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
AllDoneProc(DBAsyncParmBlkPtr myPB, asyncVarsPtr myVars)
{
	DBAsyncParmBlkPtr	asyncPB;
	ResultsRecord		*results, *oldResults;
	snarfSessElem		**snarf;

	/* see what we have to do to clean up */
	switch (myVars->currAction)
	{
		/* are we DBStartQuery? */
		case kDBInit:
		case kDBSend:
		case kDBExec:
			/* clean up */
			HSetState((Handle)myVars->query, myVars->queryState);
			HSetState(myVars->queryText, myVars->textState);

			/* call status function, if specified */
			myPB->result = CallStatusProc(myVars->statusProc, kDBStartQueryComplete, myPB->result);
			break;


		/* are we DBGetQueryResults? */
		case kDBState:
			/* do nothing if DBState didn't return rcDBValue */
			/* call status function, if specified */
			myPB->result = CallStatusProc(myVars->statusProc, kDBGetQueryResultsComplete, myPB->result);
			break;		/* fix for bug DSL-009 */

		case kDBGetItem:
			oldResults = myVars->oldResults;
			snarf = myVars->snarf;
			if (myPB->result != rcDBValue)
			{
				/* some error occurred, either out of space or ddev error */
				/* need to back up to last full row */

				results = myVars->results;

				if (myVars->endLastRowData == 0)
				{
					/* didn't get one full row, need to dispose of everything */
					DisposeResultsRecord(results);

					results->numRows = 0;
					results->numCols = 0;
				}
				else
				{
					/* got at least one row */
					/* adjust handle */
					SetHandleSize(results->colData, myVars->endLastRowData);
					SetHandleSize(results->colInfo, myVars->endLastRowInfo);
				}

				/* update currCol in snarf elem */
				(**snarf).currCol = myVars->currCol;

				/* roll back types array to current column */
				SetHandleSize(oldResults->colTypes,
					(long)((myVars->currCol+1) * sizeof(DBType)));
			}
			else
			{
				/* we got all the data successfully, so we don't need the old results */
				DisposeResultsRecord(oldResults);
				DisposPtr((Ptr)oldResults);
				(**snarf).oldResults = NULL;
				(**snarf).currCol = 0;
			}

			/* call status function, if specified */
			myPB->result = CallStatusProc(myVars->statusProc, kDBGetQueryResultsComplete, myPB->result);
			break;

	} /* switch (myVars->currAction) */

	/* copy result into application's parameter block */
	asyncPB = myVars->asyncPB;
	asyncPB->result = myPB->result;

	/* remove pb from list */
	RemovePB(myPB);

	/* call application's completion routine, if any */
	if (asyncPB->completionProc != NULL)
	{
		/* have to use assembly glue to get pb into A0 */
		CallCompletion(asyncPB);
	}

	return(noErr);

} /* AllDoneProc() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		BuildDDEVList
 *
 * DESCRIPTION:	Searches ddev folder for ddevs and builds list.
 *				Thanks to Tech Notes 67 and 68.  (Will this work with aliases?)
 *
 * INPUTS:		root:			Pointer to root of ddev list.
 *
 * OUTPUTS:		root:			Pointer to root of filled in ddev list.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	03-23-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
void
BuildDDEVList(ddevElemHndl *root)
{
	short			index = 1;
	OSErr			err;
	short			vRefNum;
	long			dirID;
	CInfoPBRec		myCPB;
	char			fName[64];

	/* first, mark all current ddevs as not existing in case one was deleted */
	ClearDDEVList(*root);

	/* get vRefNum and dirID of folder with ddevs */
	GetDDEVFolder(&vRefNum, &dirID);

	do
	{
		/* fix for bug DSL-004 - look in ddev folder for ddevs */
		fName[0] = '\0';
		myCPB.hFileInfo.ioNamePtr = fName;
		myCPB.hFileInfo.ioVRefNum = vRefNum;	/* folder with ddevs (DSL-004) */
		myCPB.hFileInfo.ioFDirIndex = index;
		myCPB.hFileInfo.ioDirID = dirID;		/* using real vRefNum and dirID */

		err = PBGetCatInfo(&myCPB, false);

		if (err == noErr)
		{
			if (((myCPB.hFileInfo.ioFlAttrib >> 4) & 0x01) != 1)
			{
				/* this is a file, not a folder */
				/* check for ddev file type */
				if (myCPB.hFileInfo.ioFlFndrInfo.fdType == DDEVTYPE)
				{
					/* ddev file, add to list */
					AddDDEVFile(fName, vRefNum, dirID, root);
				}
			}

			/* get next file */
			index++;
		}
		else if (err != fnfErr)
		{
			/* some other error occurred, just purge list and return */
			PurgeDDEVList(root);
			return;
		}

	} while (err != fnfErr);

	/* get rid of any ddevs that were deleted */
	PurgeDDEVList(root);

} /* BuildDDEVList() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		ClearDDEVList
 *
 * DESCRIPTION:	Sets "exists" to FALSE in all ddev elements in list.
 *				Called just before rebuilding list.
 *
 * INPUTS:		root:		Root of ddev list.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	03-26-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
void
ClearDDEVList(ddevElemHndl root)
{
	ddevElemHndl	curr;

	curr = root;

	while (curr)
	{
		(**curr).exists = false;
		curr = (**curr).next;
	}

} /* ClearDDEVList() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		PurgeDDEVList
 *
 * DESCRIPTION:	Deletes all elements in ddev list with "exists" set to FALSE.
 *
 * INPUTS:		root:		Pointer to root of ddev list.
 *
 * OUTPUTS:		root:		Pointer to root of purged ddev list.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	03-26-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
void
PurgeDDEVList(ddevElemHndl *root)
{
	ddevElemHndl	curr, next, prev;
	ddevElem		*p;

	curr = *root;
	prev = NULL;

	while (curr != NULL)
	{
		if ((**curr).exists == false)
		{
			/* check if we're removing the root */
			if (curr == *root)
				*root = (**curr).next;
			next = (**curr).next;

			/* get rid of string pointers */
			HLock((Handle)curr);
			p = *curr;

			if (p->name)
				DisposPtr(p->name);
			if (p->fname)
				DisposPtr(p->fname);

			HUnlock((Handle)curr);

			DisposHandle((Handle)curr);

			curr = next;
			if (prev != NULL)
				(**prev).next = curr;
		}
		else
		{
			prev = curr;
			curr = (**curr).next;
		}
	}

} /* PurgeDDEVList() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		AddDDEVFile
 *
 * DESCRIPTION:	Adds specified ddev file to list, if it's not there already.
 *
 * INPUTS:		fname:			Name of file
 *				vRefNum:		vRefNum of folder with ddevs.
 *				dirID:			dirID of folder with ddevs.
 *				root:			Pointer to root of list.
 *
 * OUTPUTS:		root:			Pointer to root of list with file added.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	03-23-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
void
AddDDEVFile(const Str255 fname, short vRefNum, long dirID, ddevElemHndl *root)
{
	ddevElem		**curr, *currptr;
	char			currState;
	char			**ddevNameRes;
	short			refNum;
	DDEVFlags		**flagsRes;
	Boolean			canAsync;
	char			ddevName[64];

	/* first, search to see if file exists in list already */
	curr = *root;

	while (curr)
	{
		MyHLock((Handle)curr, &currState);

		/* problem - can't use IUCompString at patch time */
		if ((IUCompString((**curr).fname, fname) == 0) &&
			((**curr).vRefNum == vRefNum) && ((**curr).dirID == dirID))
		{
			/* file name already exists, set exists and return */
			(**curr).exists = true;
			HSetState((Handle)curr, currState);
			return;
		}

		HSetState((Handle)curr, currState);
		curr = (**curr).next;
	}

	/* file not in list already, open it to get STR resource */
	refNum = HOpenResFile(vRefNum, dirID, fname, fsRdWrPerm);
	if (refNum != -1)
	{
		/* file open, get STR resource with name */
		ddevNameRes = (char**)Get1Resource('STR ', kDDEVName);
		if (ddevNameRes != NULL)
		{
			/* save ddev name - note that not locking ddevNameRes depends on pstrcpy just doing BlockMove */
			// HLock((Handle)ddevNameRes);
			pstrcpy(ddevName, *ddevNameRes);
			// HUnlock((Handle)ddevNameRes);

			/* get 'dflg' resource to check async support */
			flagsRes = (DDEVFlags**)Get1Resource(DDEVFLAGRESTYPE, kDDEVFlags);
			if (flagsRes)
			{
				canAsync = ((**flagsRes).flags & kAsyncSupported) != 0;
			}
			else
			{
				/* no resource, assume we can't do async */
				canAsync = false;
			}

			/* see if we already have a ddev with this name */
			if (FindDDEV(ddevName, *root) == NULL)
			{
				/* no, we have a valid ddev, add it to the list */
				curr = (ddevElemHndl)NewHandleSysClear(sizeof(ddevElem));
				HLock((Handle)curr);
				currptr = *curr;
				currptr->next = *root;
				currptr->exists = true;

				/* allocate string pointers */
				currptr->name = (char*)NewPtrSysClear(ddevName[0] + 1);
				currptr->fname = (char*)NewPtrSysClear(fname[0] + 1);
				pstrcpy(currptr->name, ddevName);
				pstrcpy(currptr->fname, fname);

				currptr->vRefNum = vRefNum;
				currptr->dirID = dirID;
				currptr->canAsync = canAsync;

				HUnlock((Handle)curr);

				/* insert into list */
				*root = curr;
			}
		}

		CloseResFile(refNum);
	}

} /* AddDDEVFile() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		FindAndBuildDDEV
 *
 * DESCRIPTION:	Looks for ddev with specified name in specified list.
 *				If not found, tries to rebuild the list again and find it.
 *				Called by SMInit and SMGetConnInfo.
 *
 * INPUTS:		name:		Name of ddev from STR resource.  This is
 *							a Pascal string.
 *				rootPtr:	Pointer to root of list.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Handle to ddevElem, or NULL if none found.
 *
 * MODIFICATIONS:
 *	03-30-90	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
ddevElemHndl
FindAndBuildDDEV(char *name, ddevElemHndl *rootPtr)
{
	register ddevElemHndl	ddevH;

	/* look for ddevElem with specified ddevName */
	ddevH = FindDDEV(name, *rootPtr);
	if (ddevH == NULL)
	{
		/*
			We couldn't find the specified ddev.  This means that either
			this is the first time DBInit or DBGetConnInfo was called, or
			the ddev doesn't exist.  Of course, the user may have installed
			the ddev since the last time we built the list.  Question:
			What about ddevs that have INIT code in them?  Answer:  The
			user is hosed if he installs them without rebooting.
		*/
		BuildDDEVList(rootPtr);

		/* try again */
		ddevH = FindDDEV(name, *rootPtr);
	}

	return(ddevH);

} /* FindAndBuildDDEV() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		FindDDEV
 *
 * DESCRIPTION:	Looks for ddev with specified name in specified list.
 *
 * INPUTS:		name:		Name of ddev from STR resource.  This is
 *							a Pascal string.
 *				root:		Root of list.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Handle to ddevElem, or NULL if none found.
 *
 * MODIFICATIONS:
 *	03-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
ddevElemHndl
FindDDEV(char *name, ddevElemHndl root)
{
	ddevElemHndl	curr = root;
	char			currState;

	while (curr)
	{
		MyHLock((Handle)curr, &currState);
		if ((**curr).exists && (IUCompString(name, (**curr).name) == 0))
		{
			/* found ddev */
			HSetState((Handle)curr, currState);
			return(curr);
		}

		HSetState((Handle)curr, currState);
		curr = (**curr).next;
	}

	/* couldn't find ddev */
	return(NULL);

} /* FindDDEV() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		OpenDDEVFile
 *
 * DESCRIPTION:	Opens specified DDEV file.  File is opened in the system heap.
 *
 * INPUTS:		ddevH:		Handle to ddevElem to open.
 *
 * OUTPUTS:		refNum:		refNum of open file.
 *
 * RETURNS:		Errors from OpenRFPerm.
 *
 * MODIFICATIONS:
 *	03-26-89	JSM - Created.
 *	11-07-89	JSM - Use HOpenResFile.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
OpenDDEVFile(ddevElemHndl ddevH, short *refNum)
{
	ddevElemPtr		ddevP;
	short			rc;
	THz				currZone;
	char			currState;

	/* save current zone */
	currZone = GetZone();

	/* make sure current zone is System Heap */
	SetZone(SystemZone());

	MyHLock((Handle)ddevH, &currState);
	ddevP = *ddevH;

	/* open the ddev */
	*refNum = HOpenResFile(ddevP->vRefNum, ddevP->dirID,
		ddevP->fname, fsRdWrPerm);
	rc = RESERR;

	HSetState((Handle)ddevH, currState);

	/* restore zone */
	SetZone(currZone);

	/* return any error from OpenRFPerm */
	return(rc);

} /* OpenDDEVFile() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		CallDDEV
 *
 * DESCRIPTION:	Calls specified ddev with specified parameter block.
 *				If an async call is indicated, checks to see if ddev
 *				will support it.  Also sets up ddevStorage in DDEVParams.
 *				All calls to ddevs pass through here!
 *
 * INPUTS:		ddevH:		Handle to ddevElem with info.
 *				params:		Pointer to parameter block for ddev.
 *
 * OUTPUTS:		ddevH:		Data may be changed.
 *
 * RETURNS:		rcDBAsyncNotSupp
 *				Errors from ddev.
 *
 * MODIFICATIONS:
 *	05-23-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
CallDDEV(ddevElemHndl ddevH, DDEVParams *params)
{
	ddevElemPtr		ddevP;
	short			rc;
	char			currState;

	MyHLock((Handle)ddevH, &currState);
	ddevP = *ddevH;

	/* check if we need to support async */
	if (params->asyncPB)
	{
		if (!ddevP->canAsync)
		{
			/* ddev doesn't support async */
			HSetState((Handle)ddevH, currState);
			return(rcDBAsyncNotSupp);
		}
		else
		{
			/* asynchronous call, we need to save ddev handle
				in case of a later DBKill */
			((asyncVarsPtr)(params->asyncPB->reserved))->ddevH = ddevH;
		}
	}

	/* pass in storage */
	params->ddevStorage = ddevP->ddevStorage;

	/* call ddev with specified parameter block */
	rc = ((DDEVPtr)StripAddress((Ptr)*(ddevP->ddev)))(params);

	/* save ddev's storage for him */
	ddevP->ddevStorage = params->ddevStorage;

	HSetState((Handle)ddevH, currState);

	return(rc);

} /* CallDDEV() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		GetUniqueSnarfID
 *
 * DESCRIPTION:	Returns a unique SnarfMan session ID.  Assumes list is ordered.
 *
 * INPUTS:		root:		Root of Snarf session list.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Unique ID, always greater than 0.
 *
 * MODIFICATIONS:
 *	03-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
long
GetUniqueSnarfID(snarfSessElemHndl root)
{
	long				id = 1;
	Boolean				done = false, found;
	snarfSessElemHndl	curr;

	while (true)
	{
		/* search list for current ID */
		curr = root;
		found = false;

		while (curr && !found)
		{
			/* see if the current cell matches the id */
			if ((**curr).snarfID == id)
			{
				/* yes, need to try again */
				found = true;
			}
			else
			{
				/* see if the current cell is greater than the id */
				if ((**curr).snarfID > id)
				{
					/* yes, we can use this id */
					return(id);
				}
				else
				{
					curr = (**curr).next;
				}
			}
		}

		if (!found)
		{
			/* no matching id found */
			return(id);
		}

		/* id already in use, try again */
		id++;
	}

} /* GetUniqueSnarfID() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		FindSnarfID
 *
 * DESCRIPTION:	Looks for cell with specified snarf ID in specified list.
 *
 * INPUTS:		id:			ID of cell to look for.
 *				root:		Root of list.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Handle to snarfSessElem, or NULL if none found.
 *
 * MODIFICATIONS:
 *	03-26-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
snarfSessElemHndl
FindSnarfID(long id, snarfSessElemHndl root)
{
	snarfSessElemHndl	curr = root;

	while (curr)
	{
		if ((**curr).snarfID == id)
		{
			/* found ID */
			return(curr);
		}

		curr = (**curr).next;
	}

	/* couldn't find ID */
	return(NULL);

} /* FindSnarfID() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		ConvertSessIDToSnarfID
 *
 * DESCRIPTION:	Convert ddev specific session ID to global Snarf ID.
 *
 * INPUTS:		sessID:		Session ID to convert.
 *				ddevH:		ddev which owns session.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Snarf ID, or 0 if no match.
 *
 * MODIFICATIONS:
 *	08-28-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
long
ConvertSessIDToSnarfID(long sessID, ddevElemHndl ddevH)
{
	sessElemHndl		sessH;
	snarfSessElemHndl	snarfH;
	snarfGlobPtr 		glob;
	Boolean				found;

	found = false;
	sessH = (**ddevH).sessRoot;

	/* look for session cell hung off of ddev */
	while (sessH && !found)
	{
		if ((**sessH).sessID == sessID)
			found = true;
		else
			sessH = (**sessH).next;
	}

	if (!found)
		/* bad session ID */
		return(0);

	/* now, scan for this session cell in the snarf list */
	found = false;
	glob = GetGlobals();
	snarfH = glob->snarfRoot;

	while (snarfH && !found)
	{
		if ((**snarfH).sess == sessH)
			found = true;
		else
			snarfH = (**snarfH).next;
	}

	if (!found)
		/* bad session ID */
		return(0);
	else
		return((**snarfH).snarfID);

} /* ConvertSessIDToSnarfID() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		InsertSessionCell
 *
 * DESCRIPTION:	Inserts a session cell into a ddev's list.
 *
 * INPUTS:		root:		Handle to root of ddev session list.
 *				cell:		Session cell to insert.
 *
 * OUTPUTS:		root:		May be modified.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	03-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
void
InsertSessionCell(sessElemHndl *root, sessElemHndl cell)
{
	/* just stick it at the front for now */
	(**cell).next = *root;
	*root = cell;

} /* InsertSessionCell() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		InsertSnarfCell
 *
 * DESCRIPTION:	Inserts a Snarf session cell into global list.
 *
 * INPUTS:		root:		Handle to root of global list.
 *				cell:		Session cell to insert.
 *
 * OUTPUTS:		root:		May be modified.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	03-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
void
InsertSnarfCell(snarfSessElemHndl *root, snarfSessElemHndl cell)
{
	snarfSessElemHndl	curr, last = NULL;
	Boolean				done = false;

	curr = *root;
	if (curr == NULL)
	{
		/* first cell in list */
		*root = cell;
		return;
	}

	/* list is ordered numerically */
	while (curr && !done)
	{
		if ((**cell).snarfID > (**curr).snarfID)
		{
			/* cell goes after curr */
			last = curr;
			curr = (**curr).next;
		}
		else
		{
			/* cell goes before curr */
			done = true;
		}
	}

	if (last)
	{
		/* not adding at root */
		(**cell).next = (**last).next;
		(**last).next = cell;
	}
	else
	{
		(**cell).next = curr;
		*root = cell;
	}

} /* InsertSnarfCell() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		MakePB
 *
 * DESCRIPTION:	Allocate and initialize a DBAsyncParamBlockRec and its associated
 *				async variables.  We need our own parameter block for every
 *				async call.
 *
 * INPUTS:		params:		DDEVParams to copy.
 *				asyncPB:	DBAsyncParmBlkPtr passed in by application.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Pointer to created asyncPB.
 *
 * MODIFICATIONS:
 *	06-29-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
DBAsyncParmBlkPtr
MakePB(DDEVParamsPtr params, DBAsyncParmBlkPtr asyncPB)
{
	snarfGlobPtr		glob;
	DBAsyncParmBlkPtr	myPB;
	asyncVarsPtr		myVars;

	glob = GetGlobals();

	/* allocate space */
	myPB = (DBAsyncParmBlkPtr)NewPtrSysClear(sizeof(DBAsyncParamBlockRec));
	myVars = (asyncVarsPtr)NewPtrSysClear(sizeof(asyncVars));

	/* make myPB point to variables */
	myPB->reserved = (long)myVars;

	/* set up completion routine */
	myPB->completionProc = (ProcPtr)StripAddress(*(glob->completionRes));

	/* keep track of application's asyncPB */
	myVars->asyncPB = asyncPB;

	/* app made call asynchronously, set result */
	asyncPB->result = 1;

	/* assume that this is not a high level call */
	myVars->highlevel = false;

	/* call about to be executing, so tell DBIdle to ignore it */
	myVars->executing = true;

	if (params)
	{
		myVars->params = (DDEVParamsPtr)NewPtrSysClear(sizeof(DDEVParams));

		/* copy ddev parameters */
		BlockMove((Ptr)params, (Ptr)myVars->params, sizeof(DDEVParams));

		/* we want ddev to use our asyncPB */
		(myVars->params)->asyncPB = myPB;

		/* set current action */
		myVars->currAction = params->message;
	}

	return(myPB);

} /* MakePB() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		InsertPB
 *
 * DESCRIPTION:	Inserts a DBAsyncParmBlkPtr into the global list.
 *
 *				Declared as a pascal function because compiler generates smaller code.
 *
 * INPUTS:		asyncPB:	Pointer to pb to insert.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	06-12-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal void
InsertPB(DBAsyncParmBlkPtr asyncPB)
{
	snarfGlobPtr	glob;

	glob = GetGlobals();

	/* just put it at the front of the list */
	((asyncVarsPtr)(asyncPB->reserved))->next = glob->pbRoot;
	glob->pbRoot = asyncPB;

} /* InsertPB() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		RemovePB
 *
 * DESCRIPTION:	Removes a DBAsyncParmBlkPtr from the global list.
 *				Disposes of the memory used by it and its variables.
 *
 *				Declared as a pascal function because compiler generates smaller code.
 *
 * INPUTS:		asyncPB:	Pointer to pb to remove.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	06-12-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal void
RemovePB(DBAsyncParmBlkPtr asyncPB)
{
	snarfGlobPtr	glob;
	asyncVarsPtr	myVars, lastVars;
	DBAsyncParmBlkPtr curr, last;

	glob = GetGlobals();
	last = NULL;
	curr = glob->pbRoot;

	while (curr && (curr != asyncPB))
	{
		last = curr;
		myVars = (asyncVarsPtr)curr->reserved;
		curr = myVars->next;
	}

	if (curr)
	{
		/* fix up list */
		myVars = (asyncVarsPtr)curr->reserved;
		if (last)
		{
			lastVars = (asyncVarsPtr)last->reserved;
			lastVars->next = myVars->next;
		}
		else
		{
			/* replacing root */
			glob->pbRoot = myVars->next;
		}

		/* dispose of variables and pb */
		if (myVars->params)
			DisposPtr((Ptr)(myVars->params));
		DisposPtr((Ptr)myVars);
		DisposPtr((Ptr)asyncPB);
	}

} /* RemovePB() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		FindAppPB
 *
 * DESCRIPTION:	Finds the parameter block in the global list that has
 *				the specified PB in the "asyncPB" field of its async variables.
 *
 * INPUTS:		appPB:		Pointer to pb to find.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		PB with appPB, or NULL if none found.
 *
 * MODIFICATIONS:
 *	07-28-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
DBAsyncParmBlkPtr
FindAppPB(DBAsyncParmBlkPtr appPB)
{
	snarfGlobPtr	glob;
	asyncVarsPtr	myVars;
	DBAsyncParmBlkPtr realPB, curr;

	glob = GetGlobals();
	realPB = NULL;
	curr = glob->pbRoot;

	while (curr && !realPB)
	{
		/* get variables for this PB */
		myVars = (asyncVarsPtr)curr->reserved;

		/* check if it points to the application's PB */
		if (myVars->asyncPB == appPB)
			/* yes, return it */
			realPB = curr;

		curr = myVars->next;
	}

	return(realPB);

} /* FindAppPB() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		GetDDEVFolder
 *
 * DESCRIPTION:	Returns vRefNum and dirID of folder with ddevs.  This is
 *				the Extensions Folder.
 *
 * INPUTS:		vRefNum:	Pointer where vRefNum is returned.
 *				dirID:		Pointer where dirID is returned.
 *
 * OUTPUTS:		vRefNum:	Filled in.
 *				dirID:		Filled in.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	05-23-89	JSM - Created.
 *	11-07-89	JSM - Modified to return dirID.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
void
GetDDEVFolder(short *vRefNum, long *dirID)
{
	/*
		Find the Extensions Folder on the current system disk,
		creating it if necessary, and ignoring errors.
	*/
	FindFolder(kOnSystemDisk, kExtensionFolderType, kCreateFolder,
		vRefNum, dirID);

} /* GetDDEVFolder() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		InitResultsRecord
 *
 * DESCRIPTION:	Initialize fields of specified ResultsRecord.  Allocates
 *				empty handles and sets numRows and numCols to 0.
 *
 * INPUTS:		results:		Pointer to ResultsRecord.
 *				inSysHeap:		If TRUE, handles allocated in sys heap.
 *
 * OUTPUTS:		results:		ResultsRecord initilized.
 *
 * RETURNS:		Errors from NewHandle.
 *
 * MODIFICATIONS:
 *	09-11-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
InitResultsRecord(ResultsRecord *results, Boolean inSysHeap)
{
	short	rc;

	/* zero out whole record first */
	mymemclear((char*)results, sizeof(ResultsRecord));

	/* allocate empty handles initially */
	if (inSysHeap)
		results->colTypes = NewHandleSys(NULL);
	else
		results->colTypes = NewHandle(NULL);

	if ((rc = MemError()) == noErr)
	{
		if (inSysHeap)
			results->colData = NewHandleSys(NULL);
		else
			results->colData = NewHandle(NULL);

		if ((rc = MemError()) == noErr)
		{
			if (inSysHeap)
				results->colInfo = NewHandleSys(NULL);
			else
				results->colInfo = NewHandle(NULL);

			rc = MemError();
		}
	}

	if (rc != noErr)
	{
		/* error occurred on one of the NewHandles */
		/* may need to clean up colTypes and/or colData */
		if (results->colTypes)
		{
			DisposHandle(results->colTypes);
			results->colTypes = NULL;

			if (results->colData)
			{
				DisposHandle(results->colData);
				results->colData = NULL;
			}
		}
	}

	return(rc);

} /* InitResultsRecord() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		GrowResultsRecord
 *
 * DESCRIPTION:	Grows handles in specified ResultsRecord in preparation
 *				for new data.  If an error occurs at any time, rolls
 *				back any changes.
 *
 * INPUTS:		results:		Pointer to ResultsRecord.
 *				len:			Length of data about to be added.
 *				inFirstRow:		If TRUE, in first row, and colTypes
 *								will be grown.
 *
 * OUTPUTS:		results:		ResultsRecord with expanded handles.
 *
 * RETURNS:		Errors from SetHandleSize.
 *
 * MODIFICATIONS:
 *	11-29-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
GrowResultsRecord(ResultsRecord *results, short len, Boolean inFirstRow)
{
	short	rc;
	Handle	dataH, infoH, typeH;
	Boolean	dataGrown, infoGrown;

	/* we haven't grown anything yet */
	dataGrown = false;
	infoGrown = false;

	/* grow data handle */
	dataH = results->colData;
	SetHandleSize(dataH, GetHandleSize(dataH) + (long)len);
	if ((rc = MemError()) == noErr)
	{
		/* data handle grown OK, grow length handle */
		dataGrown = true;

		infoH = results->colInfo;
		SetHandleSize(infoH, GetHandleSize(infoH) + sizeof(DBColInfoRecord));

		rc = MemError();
	}

	/* if we're in the first row, have to grow dataType array */
	if (inFirstRow && (rc == noErr))
	{
		/* info handle grown OK */
		infoGrown = true;

		typeH = results->colTypes;
		SetHandleSize(typeH, GetHandleSize(typeH) + sizeof(DBType));

		rc = MemError();
	}

	if (rc != noErr)
	{
		/* an error occurred, roll back to original sizes */
		if (dataGrown)
			SetHandleSize(dataH, GetHandleSize(dataH) - (long)len);

		if (infoGrown)
			SetHandleSize(infoH, GetHandleSize(infoH) - sizeof(DBColInfoRecord));
	}

	return(rc);

} /* GrowResultsRecord() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		SetupResults
 *
 * DESCRIPTION:	Common code for both synchronous and asynchronous
 *				DBGetQueryResults.  Initializes user results and
 *				copies over any old results.
 *
 * INPUTS:		sessID:			ID of current session.
 *				snarf:			Pointer for snarfSessElemHndl.
 *				results:		Pointer for user's ResultsRecord.
 *				oldResults:		Pointer for old ResultsRecord pointer.
 *				currOffset:		Pointer for currOffset.
 *				currCol:		Pointer for currCol.
 *
 * OUTPUTS:		snarf:			Filled in with snarfSessElemHndl.
 *				results:		Initialized user results with any old data.
 *				oldResults:		Allocated, if necessary, and initialized.
 *				currOffset:		Set up based on oldResults.
 *				currCol:		Set up based on oldResults.
 *
 * RETURNS:		rcDBBadSessID
 *				Errors from InitResultsRecord, NewPtrSys, and SetHandleSize.
 *
 * MODIFICATIONS:
 *	12-06-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
OSErr
SetupResults(long sessID, snarfSessElemHndl *snarfPtr, ResultsRecord *results,
	ResultsRecord **oldResultsPtr, long *currOffset, short *currCol)
{
	snarfSessElem	**snarf;
	short			rc;
	Handle			typeH, dataH, infoH;
	ResultsRecord	*oldResults;
	long			oldTypesLen, oldDataLen, oldInfoLen;

	/* find snarf ID */
	if ((snarf = *snarfPtr = FindSnarfID(sessID, (GetGlobals())->snarfRoot))
		== NULL)
	{
		/* bad session ID */
		/* this should never happen, or the DBState would have failed */
		rc = rcDBBadSessID;
	}
	else
	{
		/* initialize results in app heap */
		rc = InitResultsRecord(results, false);
	}

	if (rc == noErr)
	{
		typeH = results->colTypes;
		dataH = results->colData;
		infoH = results->colInfo;

		/* see if there are any old results */
		if ((**snarf).oldResults == NULL)
		{
			/* there were no previous results that weren't returned */

			/* allocate empty ResultsRecord to keep old results */
			oldResults = *oldResultsPtr =
				(ResultsRecord*)NewPtrSys(sizeof(ResultsRecord));

			if ((rc = MemError()) == noErr)
			{
				/* initialize old results in sys heap */
				rc = InitResultsRecord(oldResults, true);
			}

			if (rc != noErr)
			{
				/* error allocating or initializing ResultsRecord */
				if (oldResults)
					DisposPtr((Ptr)oldResults);

				/* dispose of user's ResultsRecord */
				DisposeResultsRecord(results);
			}
			else
			{
				/* save results */
				(**snarf).oldResults = oldResults;

				/* we're starting at the beginning */
				*currCol = 0;
				*currOffset = 0;
			}
		}
		else
		{
			/* there were some old results, copy them to the user */
			oldResults = *oldResultsPtr = (**snarf).oldResults;

			/* we're starting in the middle of a row */
			*currCol = (**snarf).currCol;

			/* grow handles accordingly */
			SetHandleSize(typeH,
				oldTypesLen = GetHandleSize(oldResults->colTypes));
			rc = MemError();

			if (rc == noErr)
			{
				SetHandleSize(dataH,
					oldDataLen = GetHandleSize(oldResults->colData));
				rc = MemError();
			}

			if (rc == noErr)
			{
				/* point to end of data */
				*currOffset = oldDataLen;

				SetHandleSize(infoH,
					oldInfoLen = GetHandleSize(oldResults->colInfo));
				rc = MemError();
			}

			if (rc == noErr)
			{
				/* move in stuff */
				BlockMove(*(oldResults->colTypes), *typeH,
					oldTypesLen);
				BlockMove(*(oldResults->colData), *dataH,
					oldDataLen);
				BlockMove(*(oldResults->colInfo), *infoH,
					oldInfoLen);
			}
			else
			{
				/* an error occurred trying to resize handles */

				/* dispose of user's ResultsRecord */
				DisposeResultsRecord(results);
			}
		}
	}

	return(rc);

} /* SetupResults() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		DisposeResultsRecord
 *
 * DESCRIPTION:	Dispose of handles inside a results record.
 *				Assumes all handles are allocated.
 *
 *				Declared as a pascal function because compiler generates smaller code.
 *
 * INPUTS:		results:		Pointer to results to be disposed of.
 *
 * OUTPUTS:		results:		colTypes, colData, colInfo are all nulled.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	12-06-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal void
DisposeResultsRecord(ResultsRecord *results)
{
	DisposHandle(results->colTypes);
	DisposHandle(results->colData);
	DisposHandle(results->colInfo);
	results->colTypes = NULL;
	results->colData = NULL;
	results->colInfo = NULL;

} /* DisposeResultsRecord() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		CallStatusProc
 *
 * DESCRIPTION:	Common code to call a status function passed into DBStartQuery or
 *				DBGetQueryResults with no extra parameters.
 *
 * INPUTS:		statusProc:		Pointer to status function passed in by application.
 *								If NULL, does not call it (obviously).
 *				message:		Message for status function.
 *				rc:				Result for status function.
 *
 * OUTPUTS:		None.
 *
 * RETURNS:		userCanceledErr if status function returns false, otherwise
 *				original rc passed in.
 *
 * MODIFICATIONS:
 *	04-23-90	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1990
 *	All rights reserved.
 *
 */
OSErr
CallStatusProc(statusPtr statusProc, short message, OSErr rc)
{
	if (statusProc != NULL)
	{
		if (!statusProc(message, rc, NULL, NULL, NULL, NULL, NULL))
		{
			/* status function cancelled */
			rc = userCanceledErr;
		}
	}

	/* return original rc or userCanceledErr */
	return(rc);

} /* CallStatusProc() */

/*FORM_FEED*/
/*
 *
 * OBJECT:		MyHLock
 *
 * DESCRIPTION:	Smart handle locking.  Saves current state of handle, then locks
 *				it.  Just call HSetState to restore.
 *
 *				Declared as a pascal function because compiler generates smaller code.
 *
 * INPUTS:		h:			Handle to be locked.
 *
 * OUTPUTS:		state:		State of handle before locking.
 *
 * RETURNS:		Nothing.
 *
 * MODIFICATIONS:
 *	03-24-89	JSM - Created.
 *
 *	Copyright Apple Computer, Inc. 1989
 *	All rights reserved.
 *
 */
pascal void
MyHLock(Handle h, char *state)
{
	*state = HGetState(h);
	HLock(h);

} /* MyHLock() */
