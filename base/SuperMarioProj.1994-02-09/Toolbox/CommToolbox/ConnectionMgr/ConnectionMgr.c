/*
	File:		ConnectionMgr.c

	Contains:	This is the main guts of the Connection Manager

	Written by:	Dean Wong, Byron Han, and Jerry Godes

	Copyright:	© 1988-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 8/27/90	kaz		Preserving MemErr, ResErr if _CMNew returns NIL; Code shrinkage;
									Uses Length(theStr) instead of theStr[0].
		 <4>	 7/10/90	dba		add __ to distinguish routines from glue
		 <3>	  6/4/90	BBH		include new file ConnectionsExtensions.h
		 <2>	 3/15/90	BBH		include the proper .h files
		 <1>	 3/14/90	BBH		first checked in to BBS

	To Do:


	Pre BBS Revision History
				1/26/90		BBH		Added CMHRead, CMHWrite, CMHIOKill
				1/3/90		BBH		Added CMGetErrorString, CMAllocateIORec, and CMFreeIORec
				9/26/89		jng		changed CMSearchCallback to be a pascal procedure.
				9/15/89		BBH		Changed BufferSizes to CMBufferSizes
									Changed BufFields to CMBufFields
									Changed Buffers to CMBuffers
				7/27/89		BBH		Changed dataBuffer to DataBuffer
				7/14/89		BBH		CMDispose now calls CMClearSearch before it completes.
				5 June 1989	JNG		New capitalization
				2 June 1989	JNG		New interface to CTB core routines.
				23 May 1989	JNG		fix new Interfaces.
				23 May 1989	JNG		Redefine flags for Searching - removed existing flags and added one
										flag to go between 8 bit and 7 bit searches.
				19 May 1989	JNG		New interfaces
				7 May 1989	BBH		Changed eom to flags in CMRead/Write
				1 May 1989	BBH		Added comments to CMNew.  In process, discovered that if
										CMDefault for oldConfig failed, we would not discover it.
									Also initialize reserved1 and reserved2 to nil
									Added common exit cleanup code for CMNew
										CMDispose now checks to see that RecoverHandle returns a valid
										handle before releasing
				24 Apr 1989	JNG		Moved CMResume CMMenu CMEvent CMActivate CMReset CMIdle
										CMAbort to CTBCore
				24 Apr 1989	BBH		Added CMGetConnEnvirons routine
										CMRead, CMWrite and CMIOKill do not call ToolPreflight/Postflight
				18 Apr 1989	JNG		CMGetProcID now in CTBCore
				17 Apr 1989	JNG		CMDefault now in CTBCore
				13 Apr 1989	JNG		CMValidate now in CTBCore.  This required changing internal
										calls to CMValidate to use the core routine CTBValidate.
				12 Apr 1989			CMGetToolName now in CTBCore
				11 Apr 1989			Changed CMGetAResource to CTBGetAResource, that and CRMGetAResource
										are now part of the CTBGlue routines.
									CMGetVersion is now part of the CTBCore
				7 Apr 1989			Changed CMGetConnName to CMGetToolName
									Removed remaining code that was still ifdefed due to separate files
									Added more error checking in CMNew
									Added validate to CMNew
									Fixed CMDefault to set config to nil only if allocate
									SetupPreflight returns nil if it can't get the setup resource
									Setup code now just calls CMGetAResource most times -
										it used to call CRMGetResource which does the proper indexing
										on reusable resources.  Now, we only use CRMCalls in preflight
										and cleanup.
				6 Apr 1989 			add more comments
				5 Apr 1989			Changed CMGetVersion to get 'vers' 1 resource
				2 Apr 1989			change connection manager to use separate files instead of single
									communications file with all tools.  procID is now the resource refnum
									of the tool and is dynamically assigned.
									All calls to tools now call CRMToolPreflight/Postflight to modify the
									resource chain before and after the tool executes.
									change CMGetProcID to open the file and return the refnum
									change CMGetVersion to do a 1 deep search on the tool for cver resource
									change CMGetName to convert refnum to filename
									added CMGetAResource and CRMGetAResource to do 1 deep searches on the tool
									file for a particular resource, with optional registration with the CRM
									changed LocalToRealID to CRMLocalToRealID
				1 Apr 1989			change refcon/userdata stuff to assembler
*/

#include <Errors.h>
#include <Memory.h>
#include <Resources.h>
#include <OSUtils.h>
#include <SysEqu.h>

#include "CTBUtilities.h"						/* toolbox interface files				*/
#include "CommResources.h"
#include "Connections.h"
#include "ConnectionsExtensions.h"
#include "ConnectionTools.h"

#include "ConnectionsPriv.h"					/* Connection Manager private includes 	*/
#include "CommToolboxPriv.h"

#include "ConnectionMgrUtilities.c"				// for CDEFExecuteDef

#define __MANAGERCODE__
#include "ConnectionMgrExtensions.c"			// for GetErrorString/AllocateIORec/FreeIORec etc
#undef __MANAGERCODE__

pascal void __CMClearSearch(ConnHandle);

/*
 *	InitCM - the real meat here
 *
 *	This routine will initialize the Connection Manager and will also initialize the
 *	Communications Toolbox Utilities and the Communications Resource Manager
 *
 *	Under MultiFinder patch on demand, the trap dispatch code will automatically load in
 *	the Connection Manager code into the application heap and load the appropriate pointers
 * 	into the internal dispatch table
 */
pascal short __InitCM(void)
{
	return ( InitManager(CMSel));
}

/*	routine to create a connection record and pass the INIT message to the connection tool
		after allocating the config and oldConfig data structures via a call to CMDefault

		Calls CMValidate after INIT message
 */

pascal ConnHandle __CMNew( procID, flags, desiredSizes, refCon, userData )
	short 			procID;
	CMRecFlags 		flags;
	long 			desiredSizes[];
	long 			refCon, userData;
{
	ConnHandle 		hConn;
	ConnPtr			pConn;
	Handle 			theHand;
	long			rval;
	Ptr				aConfig;
	CRMToolContext 	theContext;
	short			savedMem = GetMemErr(),
					savedRes = GetResErr();
	
	if (CRMReserveRF(procID) != noErr)
		return (nil);
	
	/* Allocate & fill the conn rec	*/
	hConn = (ConnHandle) NewHandleClear(sizeof(ConnRecord));
	if (hConn == nil) {
		savedMem = GetMemErr();
		goto err0;	
	}
	
	/* Done by _NewHandleClear	*/
	/*
		(**hConn).defProc = nil;
		(CMSearchPtr)(**hConn).mluField = nil;
		(**hConn).reserved1 = 0;
		(**hConn).reserved2 = 0;
		(**hConn).cmPrivate = nil;
		(**hConn).oldConfig = nil;
		(**hConn).config = nil;
	
	*/
	
	pConn = (ConnPtr) *hConn;
	pConn->procID = procID;	
	pConn->refCon = refCon;
	pConn->userData = userData;
	pConn->flags = flags;

	/* Get the 'cdef' code resource	*/
	theHand = CRMGetAResource(cdefType, procID);
	if (theHand == nil) {
		savedRes = GetResErr();
		goto err1;
	}

	MoveHHi(theHand);
	HLock(theHand);
	(**hConn).defProc = (ProcPtr) *theHand;
	
	BlockMove((Ptr) desiredSizes, (Ptr) (**hConn).bufSizes, sizeof(CMBufferSizes));		/* 32 = long[8] */
	
	/* Need to allocate the config record twice	*/
	CTBDefault( &aConfig, procID, true, CMSel);
	if (aConfig == nil) {
		savedMem = GetMemErr();
		savedRes = GetResErr();
		goto err2;
	}
	(**hConn).config = aConfig;

	/* We can't just duplicate the struct as the tool might do something special	*/
	CTBDefault( &aConfig, procID, true, CMSel);
	if (aConfig == nil) {
		savedMem = GetMemErr();
		savedRes = GetResErr();
		goto err3;
	}	
	(**hConn).oldConfig = aConfig;
	
	/* call the tool's initMsg	*/
	rval =  CDEFExecuteDef((long)hConn, (long)(**hConn).defProc, cmInitMsg, 0, 0, 0 );
	if (rval != 0) {
		savedMem = GetMemErr();
		savedRes = GetResErr();
		goto err4;
	}
	
	// Should really check this error
	if (CTBValidate((CoreHandle) hConn, CMSel))
		;

	return(hConn);										/* and go home */

//----------------------------------------

err4:
		DisposPtr((**hConn).oldConfig);	
err3:
		DisposPtr((**hConn).config);	
err2:
		CRMToolPreflight(procID, &theContext);			/* place me into resource chain */
		CRMReleaseResource(theHand);					/* release cdef resource */
		CRMToolPostflight(&theContext);
err1:
		DisposHandle( (Handle)hConn );	
err0:
		CRMReleaseRF(procID);							/* close file or decrement useCount */
		GetMemErr() = savedMem;							// restore the error values
		GetResErr() = savedRes;
		
		return( nil );
}



/*	CMDispose dispose of the connection record */
pascal void __CMDispose( hConn )
ConnHandle hConn;
{
	Handle theHand;
	CMErr err;
	short	procID;
	CRMToolContext theContext;

	__CMClearSearch(hConn);		/* ??? use the CTB dispatcher here? */

	procID = (**hConn).procID;

	CDEFExecuteDef( (long)hConn, (long)(**hConn).defProc, cmDisposeMsg, 0, 0, 0 );

	theHand = RecoverHandle( (Ptr)(**hConn).defProc);
	if (theHand != nil)
	{
		CRMToolPreflight(procID, &theContext);				/* put into the resource chain */
		CRMReleaseResource(theHand);						/* make sure we have handle */
		CRMToolPostflight(&theContext);
	}
	DisposPtr((**hConn).config);
	DisposPtr((**hConn).oldConfig);
	DisposHandle( (Handle) hConn );

	err = CRMReleaseRF(procID);
}


pascal CMErr __CMOpen( hConn, async, completor, timeout )
ConnHandle hConn;
Boolean async;
ProcPtr completor;
long timeout;
{
	CMCompletorRecord  cp;
	CMErr err;

	cp.async = async;
	cp.completionRoutine = completor;
	err = (CMErr)CDEFExecuteDef( (long)hConn, (long)(**hConn).defProc, cmOpenMsg, (long)&cp, timeout, 0 );

	if ( !err  )
		CTBValidate( (CoreHandle)  hConn, CMSel);

	return( err );
}

pascal CMErr __CMListen( hConn, async, completor, timeout )
ConnHandle hConn;
Boolean async;
ProcPtr completor;
long timeout;
{
	CMCompletorRecord  cp;

	cp.async = async;
	cp.completionRoutine = completor;
	return ( (CMErr)CDEFExecuteDef( (long)hConn, (long)(**hConn).defProc, cmListenMsg,
				(long)&cp, timeout, 0 ));
}

pascal CMErr __CMAccept( hConn, accept )
ConnHandle hConn;
Boolean accept;
{
	return ((CMErr)CDEFExecuteDef( (long)hConn, (long)(**hConn).defProc, cmAcceptMsg,
				(long)accept, 0, 0 ));
}

pascal CMErr __CMClose( hConn, async, completor, timeout, now )
ConnHandle hConn;
Boolean async;
long timeout;
ProcPtr completor;
Boolean now;
{
	CMCompletorRecord  cp;

	cp.async = async;
	cp.completionRoutine = completor;
	return((CMErr)CDEFExecuteDef( (long)hConn, (long)(**hConn).defProc, cmCloseMsg,
			(long)&cp, (long)now, timeout ));
}


/*
	This routine will return the connection environment record for the given data connection

	It passes a cmEnvironsMsg to the connection tool.  The tool should return noErr if there is
	no problem or the appropriate operating system error code.
*/

pascal CMErr __CMGetConnEnvirons( hConn, theEnvirons)
ConnHandle hConn;
ConnEnvironRec *theEnvirons;
{
	return( (CMErr)CDEFExecuteDef( (long)hConn, (long)(**hConn).defProc, cmEnvironsMsg,
				(long)theEnvirons, 0, 0 ));
}

/*
	This routine performs a hashing operation to support the connection manager searching routines.
*/

#define HASH(c)		(0x1L << (0x1FL & (long)(c)))

short IsThisInBuffer(theSearch, theBlock)
CMSearchPtr theSearch;
CMSearchBlockPtr theBlock;
{
	StringPtr aString;
	unsigned char bufferStepper;
	unsigned char charStepper;
	unsigned char aChar,bChar;
	unsigned char bitMask;
	unsigned char *theBuf;
	Boolean match;

	aString = &theBlock->theString;
	charStepper = aString[0];
	bufferStepper = theSearch->bufferPtr;
	bitMask = ((theBlock->flags&cmSearchSevenBit) ? 0x7F : 0xFF);
	theBuf = &theSearch->buffer;

	match = true;

	while  ((charStepper > 0) && match) {
		aChar = aString[charStepper] & bitMask;
		bChar = theBuf[bufferStepper] & bitMask;

		match = (aChar == bChar);
		bufferStepper--;
		charStepper--;
	}
	return (match);
}

typedef		pascal void				(* CBProcPtr) (ConnHandle);

CMSearchit(hConn,buffer, count)
ConnHandle hConn;
Ptr buffer;
long count;
{
	CMSearchPtr	theSearch;
	CMSearchBlockPtr theBlock, tmpPtr;
	unsigned char aChar, *theChar;
	pascal void			(*callProc) (ConnHandle, Ptr, long);

	theSearch = (CMSearchPtr)(**hConn).mluField;

	for (theChar = buffer; (theChar-buffer)<count; theChar++) { /* Copy all chars to buffer */
		aChar = *theChar;
		theSearch->buffer[++(theSearch->bufferPtr)] = aChar;
		if ((HASH(aChar) & theSearch->hashValue) != 0 || theSearch->hashNotValid) {
			for (theBlock = (CMSearchBlockPtr)(theSearch->theQueue.qHead);
				 theBlock;  theBlock = tmpPtr) {
					tmpPtr = (CMSearchBlockPtr)(theBlock->qLink); /* Get this first, in case it's removed */
					if (IsThisInBuffer(theSearch, theBlock) == 1) {
						(ProcPtr) callProc = theBlock->callBack;
						(*callProc) (hConn,  theChar, (long)theBlock);
					}
			}
		}
	}
}

pascal void CMSearchCallBack(hConn)
ConnHandle hConn;
{
	CMSearchPtr searchHdr;

	searchHdr = (CMSearchPtr)(**hConn).mluField;

	CMSearchit(hConn,searchHdr->ReadBuffer,
				(**hConn).asyncCount[searchHdr->ReadChannelIn]);

	if (searchHdr->ReadCallBack) { /* Ok - now call the users callback */
		(*(CBProcPtr) searchHdr->ReadCallBack)(hConn);
	}
}
pascal CMErr __CMRead( hConn, buffer, toread, channel, async, completor, timeout, flags )
	ConnHandle 	hConn;
	Ptr 		buffer;
	long 		*toread, timeout;
	CMChannel 	channel;
	Boolean 	async;
	short 		*flags;
	ProcPtr 	completor;
{
	CMSearchPtr			theSearch;
	CMErr 				err;
	CMDataBuffer 		dbuf;
	CMCompletorRecord 	cp;
	pascal long			(*callProc) (ConnHandle, short, long, long, long);

	dbuf.thePtr = buffer;
	dbuf.count = *toread;
	dbuf.channel = channel;
	dbuf.flags = *flags;

	cp.async = async;
	
	theSearch = (CMSearchPtr) (**hConn).mluField;
	
	cp.completionRoutine = completor;
	if (theSearch && async) { 			/* There are strings to search  ... */
		cp.completionRoutine = (ProcPtr) CMSearchCallBack;
		theSearch->ReadCallBack = completor;

		switch (channel) {
			case cmData:
				theSearch->ReadChannelIn = cmDataIn;
				break;
			case cmCntl:
				theSearch->ReadChannelIn = cmCntlIn;
				break;
			case cmAttn:
				theSearch->ReadChannelIn = cmAttnIn;
				break;
			case (1<<3) /* cmRsrv */:
				theSearch->ReadChannelIn = cmRsrvIn;
				break;
		}

		theSearch->ReadBuffer = buffer;
	}
		
	(ProcPtr) callProc = (**hConn).defProc;
	err = (CMErr) (*callProc)(hConn, cmReadMsg, (long) &dbuf, timeout, (long) &cp);

	*toread = dbuf.count;
	*flags = dbuf.flags;

	if (theSearch && ! async)			 /* There are strings to search */
		CMSearchit(hConn,buffer,*toread);
		
	return( err );
}

pascal CMErr __CMWrite( hConn, buffer, towrite, channel, async, completor, timeout, flags )
ConnHandle hConn;
Ptr buffer;
long *towrite, timeout;
CMChannel channel;
Boolean async;
short flags;
ProcPtr completor;
{
	CMErr err;
	CMDataBuffer dbuf;
	CMCompletorRecord cp;
	pascal long		(*callProc) (ConnHandle, short, long, long, long);

	dbuf.thePtr = buffer;
	dbuf.count = *towrite;
	dbuf.channel = channel;
	dbuf.flags = flags;

	cp.async = async;
	cp.completionRoutine = completor;

	(ProcPtr) callProc = (**hConn).defProc;
	err = (CMErr)(*callProc) ( hConn, cmWriteMsg, (long)&dbuf, timeout, (long)&cp );

	*towrite = dbuf.count;
	return( err );
}

pascal CMErr __CMIOKill( hConn, which )
ConnHandle hConn;
short	   which;
{
	pascal long		(*callProc) (ConnHandle, short, long, long, long);

	(ProcPtr) callProc = (**hConn).defProc;
	return( (CMErr)(*callProc) ( hConn, cmIOKillMsg, which, 0, 0 ));
}

pascal void __CMBreak( hConn, duration, async, completor )
ConnHandle hConn;
long duration;
Boolean async;
ProcPtr completor;
{
	CMCompletorRecord cp;
	pascal long		(*callProc) (ConnHandle, short, long, long, long);

	cp.async = async;
	cp.completionRoutine = completor;

	(ProcPtr) callProc = (**hConn).defProc;
	(*callProc) ( hConn,  cmBreakMsg, duration, (long)&cp, 0 );

}

pascal long __CMAddSearch(hConn, theString, flags, callBack)
	ConnHandle 		hConn;
	Str255 			theString;
	CMSearchFlags 	flags;
	ProcPtr 		callBack;
{
	CMSearchPtr 		theSearch;
	CMSearchBlockPtr 	theBlock;

	if ((theBlock = (CMSearchBlockPtr)NewPtr(sizeof(CMSearchBlock))) == nil)
		return (-1);

	theBlock->qType = 9;
	BlockMove(theString,theBlock->theString,Length(theString) + 1);

	theBlock->flags = flags;
	theBlock->callBack = callBack;

	theSearch = (CMSearchPtr)(**hConn).mluField;

	if (!theSearch)	{						/* This is the first search */
		if (!(theSearch = (CMSearchPtr) NewPtrClear(sizeof(CMSearch)))) {
			DisposPtr((Ptr) theBlock);
			return (-1);
		}
		
		/* Done by _NewPtrClear
		/*
			theSearch->bufferPtr = 0;
			theSearch->padding = 0;
			theSearch->theQueue.qHead = nil;
			theSearch->theQueue.qTail = nil;
			theSearch->theQueue.qFlags = 0;
		*/
		
		
		theSearch->hashValue = HASH(theString[Length(theString)]);
		(CMSearchPtr)(**hConn).mluField = theSearch;
	}
	else
		theSearch->hashValue |= HASH(theString[Length(theString)]);

	theSearch->hashNotValid = false;					/* Hash is fine for what we've got */

	Enqueue((QElemPtr)theBlock,&theSearch->theQueue);

	return ((long)theBlock);
}


pascal void __CMRemoveSearch(hConn, refnum)
ConnHandle	hConn;
long		refnum;
{
	CMSearchPtr			theSearch;
	CMSearchBlockPtr	theBlock;
	OSErr				err;

	if ((theSearch = (CMSearchPtr)(**hConn).mluField) == nil) {
		return;
	}

	err = Dequeue((QElemPtr)refnum,&theSearch->theQueue);


	if (theSearch->theQueue.qHead == nil) { /* Nothing left */
		(**hConn).mluField = nil;
		DisposPtr((Ptr)theSearch);
	}

	if (err == qErr) {
		return;
	}

	DisposPtr((Ptr)refnum);								 /* Then the block itself */


	/* Regenerate Hash value */
	if (theSearch = (CMSearchPtr)(**hConn).mluField) {

		/* If we're interrupted - they can't depend on the hash value */

		theSearch->hashNotValid = true;
		theSearch->hashValue = 0;
		for (theBlock = (CMSearchBlockPtr)theSearch->theQueue.qHead;
			 theBlock;
			 theBlock = (CMSearchBlockPtr)theBlock->qLink) {
			theSearch->hashValue |=
				HASH(theBlock->theString[Length(theBlock->theString)]);
		theSearch->hashNotValid = false;	/* It's back up to date */
		}
	}

}


pascal void __CMClearSearch(ConnHandle hConn)
{
	CMSearchPtr 		theSearch;
	CMSearchBlockPtr	theBlock;

	if ((theSearch = (CMSearchPtr)(**hConn).mluField) == nil) {
		return;
	}

	(CMSearchPtr)(**hConn).mluField = nil;		/* This really signifies clearing the search -
													if a read comes in, there won't be anything
													in the list */

	while (theBlock = (CMSearchBlockPtr)theSearch->theQueue.qHead) { /* Now free everything up */
		Dequeue((QElemPtr)theBlock,&theSearch->theQueue);
		DisposPtr( (Ptr) theBlock);

	}

	DisposPtr( (Ptr) theSearch);
}
/* end of file */
