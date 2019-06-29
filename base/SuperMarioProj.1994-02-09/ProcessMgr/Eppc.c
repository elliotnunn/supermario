/*
	File:		Eppc.c

	Contains:	Routines which implement Event Process-to-Process Communication (EPPC),
				aka high-level events.

	Written by:	Jay Moreland

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<26>	 5/29/92	DCL		Included Script.h. GetEnvirons moved for the New Inside Mac.
		<25>	11/25/91	DTY		Add #include <MemoryMgrPriv.h> to get interface for MoveHLow in
									its new home.
		<24>	11/21/91	DTY		Rolling in the 7•Up version of findFreeSessionRecord that
									allocates the session record low in the System Heap instead of
									in the middle of the temporary heap. This is conditionalized for
									CubeE only until a decision is made about whether this is really
									the way we want to fix the problem.
		<23>	 3/29/91	JWM		CCN,#85997: In GetSpecificHighLevelEvent, it was possible to
									exit without restoring eppcBlk->mfMsgBlk to its original value.
									This would cause event_common to call flushMsg for a HLE that
									was still linked into the app's HLE Queue. As part of flushMsg
									the memory would be returned to the free pool. This caused weird
									memory roaching problems. The fix is a one line addition in
									c_GetSpecificHighLevelEvent during the error path for the second
									copyMsg.
		<22>	 3/26/91	DTY		Having been thoroughly chastised by Jay for touching the source
									code without him standing next to me, I’m about to risk his
									wrath again to make things the way they were in version <20>.
		<21>	 3/25/91	DTY		JDR, #85618: The Gang of Five relegated this bug to Post 7.0, so
									it needed to be rolled out.  The other change, JWM-001, was left
									in.
		<20>	 3/25/91	JWM		jc,#JWM-001,#85618: •removing a DisposHandle from inside a loop
									where interrupts are disabled. •removing the restriction on
									starting a session when the application is being coerced.
		<19>	 3/18/91	JWM		ewa,#85038, #85039: •The System crashes when
									GetSpecificHighLevelEvent is called twice in a row with no
									events pending and there is a partial HLEvent outstanding.
									•GetSpecificHighLevelEvent goes through the HighLevelEventQueue
									twice if AcceptHighLevelEvent is called for the last event in
									the HighLevelEventQueue from within the filter function.
		<18>	 2/26/91	JWM		DFH,#83798: fix bug where ports are sometimes left open.
		<17>	 2/21/91	DFH		dba,#82504, #82681, #83168, #83182, #83207: HiNewHandle rolled
									into ProcessMgrHiNewHandle.
		<16>	 2/21/91	JWM		ewa,#83492,#78885: •The System crashes when processing a return
									receipt over a network for an application which has quit and is
									no longer running. Fix is in scanReadBQ. •QuarkXPress 3.0 —
									Double clicking on the document opens the app, but not the
									document. fix is in makePortname. •Also call PPC post processor
									routines (scanReadBQ, scanWriteBQ and scanInformBQ) when
									GetSpecificHighLevelEvent is called. This is for AppleEvent
									manager.
		<15>	  2/9/91	JWM		DFH,#79583,#81622,#82463: •Restart and Shutdown don't work in
									low memory conditions. Routines getPPCRead, getPPCInformPB,
									findFreeSessionRecord, GetMsgEventBuffer, fetchMsgData use to
									call ProcessMgrHiNewHandle. ProcessMgrHiNewHandle just searched
									the Process Managers heap. The routines now call GMBlk which
									follows an unsucessful search of the Process Manager's heap with
									a search of the System's heap. In addition,
									AppleEventExtensions.c and Puppet.c were modified to use GMBlk
									instead of just looking in the Process Manager's heap. GMBlk is
									defined in Data.h and implemented in Memory.c. •Event PPC does
									not register the correct port name with the correct name script
									for foriegn systems. The PPCPortname has a script code which was
									being stuffed with 0 in makePortName. A call is now made to
									GetEnvirons to fetch the script code of the system script. This
									value is place in the PPCPortName when open the PPCPort on
									behalf of the app. •The maximum number of IDENTICAL Portnames
									that can be browsed on the browser is 8. Any number over 8 will
									not be recognize as EPPC aware application. A bug was introduced
									in 7.0b1 in the routine GetNextPortName. StringByte is defined
									as having a 0 based index instead of 1 based index. This caused
									the code which runs in an attempt to generate a unique
									PPCPortname to generate 8 uniques name instead of the 99 it
									should have.
		<14>	 1/28/91	LN		Changed connectionInValid to conectionInvalid to match change in
									EPPC.h. Sorry Jay.
		<13>	 1/10/91	JWM		(DFH) •1 BRC #79831 allow app to post using kCurrentProcess
									constant. The change was in a parameter to the first
									EPPCBlkFromPSN in GetConnectionType. Before <13>, EPPCBlkFromPSN
									was called using the PSN Ptr in the PostHighLevelEvent. The fix
									was to use the PSN in the process table. This means when the
									caller of PostHighLevelEvent uses pCurrentProcess or
									pFrontProcess, GetConnectionType will find the eppcBlk
									associated with the target process. (EMT) •2 To keep the IO
									completion routine (KnockKnock) from someday getting hung while
									spinning on the ioresult field in the async PPCAccept or
									PPCReject parameter block, a new switch was added for command
									completion of a PPCAccept and PPCReject. The ppcInformBlk was
									modified to include a PPCRejectParams structure
									(ppcInformReplyPB). This structure is used for both the async
									PPCReject and the async PPCAccept which occur when processing a
									completion of a PPCInform. This change is isolated to
									KnockKnock. (DFH) •3 Rework the DisableInterrupts and
									EnableInterrupts macros to require fewer instruction for
									compiler generated setup.
		<12>	12/21/90	LN		change constStrxxxParameter to ConstStrxxxParam as per change in
									Types.h
		<11>	12/21/90	DFH		(gb) Added A5 setup in GetNextHighLevelEvent.
		<10>	12/20/90	DFH		(dba) Convert GetNextHighLevelEvent to a dispatch.
		 <9>	12/19/90	gbm		(dba) Fix pointer arithmetic bugs throughout the file, including
									the one that kept us from quitting applications with the 32-bit
									Memory Manager.
		 <8>	12/14/90	JWM		change rtrnReciptMsgID to rtrnReceiptMsgID
		 <7>	12/13/90	JWM		Make the checkin comments look right
		 <6>	12/13/90	JWM		DFH •1	Fix a bug with doDuplicateNameOpen.  The termination of the loop
									is wrong.
									DFH •2	Added a call to Delete User Identity after a StartSecureSession
									call.  eppc doesn't need the User Identity.  Deleting it frees up
									resources inside the PPCToolbox.
									DFH •3	StartSecureSession may cause the Dialog Manager to run.  It is
									best if our app's A5 is in the machine because user items in
									dialog windows behind the authentication dialog(s) may execute.
									Some user item procedures assume that A5 points to the application's
									global not the Process Manager's globals.
									DFH •4	The PPCToolbox as of 7.0B3 will no longer return errors through
									PPCInit.  References to ppcInitErr have been removed.  In addition
									a message was formatted and sent to the Shell if ppcInitErr did not
									equal 0 (noErr).  This message will be elimated in Startup.c.
									DFH •5	BRC #78779-During PostHighLevelEvent to a remote machine,
									if a proper port location is specified, but not a proper port name (i.e.
									port name doesn't exist on the target machine), then the authentication
									dialog is brought up.  This behavior was fixed by making guestAllowed
									return destPortErr (-906) if listPortPB.actualCount IS 0.  getSessionID
									will see the destPortErr and not call StartSecureSession.
									DFH •6	Request from DTS to place TICKS in the when field of the event when
									the message arrives.  This is now done in KnockKnock when a read completes.
									DFH •7	BRC # 78336–HighLevel event posted shortly before an application quits
									does not get delivered to its target.  This ‘bug’ needs fixing.  It is 
									due to closing the ppc port of an application during _ExitToShell.  This
									close causes all outstanding i/o for a port to go to completion.  Any
									post an application did between his last _WaitNextEvent and _ExitToShell
									may not be delivered.  This is due to the facts that processing the
									i/o completion queues for PPCToolbox calls occurs during WaitNextEvent
									if the system decides to call GetNextHLE and that PPCClose causes any
									outstanding PPCWrites to end with an error.  A goal for eppc has been
									to shield the application writer from the async nature of the PPCToolbox
									In this spirit, code was changed to keep ppc ports open after
									application calls _ExitToShell if the port has PPCWrites which are
									still pending. This allow the posts of a terminated application to
									complete.  Note:  the PPCWrite still could  error, but no one is
									listening.
									Changes to affect leaving the ppc port open after application
									_ExitToShell involved storing the address of the last PPCWrite in
									the session record, not breaking connections with outstanding
									PPCWrites during _ExitToShell, closing the port when the last PPCWrite
									completed and weaning all code paths in eppc that run after
									_ExitToShell from there dependence on the EPPCBlk which goes away
									at _ExitToShell.
									DFH •8	An application posting a high level event from the background which
									requires the network authenication dialog will receive an error
									noUserInteractionAllowed (–610).  This was done to fix the User Interface
									No–No of bringing up the a modal dialog when the application was
									not the frontmost.  Note:  this situation occurs only on the first
									post to a remote target.  The application writer should use the
									notification manager to indicate a need to interact with the user.
									When the user brings the application forward, the application writer
									should do the post again.  After the post is sucessful, the
									application can return to the background and post messages to the
									remote target without the noUserInteractionAllowed error.
									DFH •9	In eppc:getSessionID there were two error paths which did not release
									the session table entry correctly.
		 <5>	11/26/90	DFH		Change trespass() to dbmsg().
		 <4>	 11/6/90	JWM		•1	for the priority 1 bug dealing with an assert error when the
									sender quits before the receiver has been delivered all his
									messages.
									•2 	you can now post messages of minimum unsigned long through
									maximum unsigned long, if you have the memory.
									•3	In previous releases, when you didn’t accept a message fully
									an incorrect return message receipt was return to the sender.
									This is now fixed and works as documented.
									•4	In previous releases three memory allocations and two memory
									releases were necessary for each message received.  In this
									version each number has been reduced by one.
									•5	In previous releases, if the first PPCRead failed, the
									connection would go dormant.  In this release if the first
									PPCRead fails for reasons other than sessionClosedErr or
									noSessionErr, the connection is broken and the target’s session
									table entry is updated to reflect the connection was explicitly
									broken.  If the reason was sessionClosedErr or noSessionErr, the
									target’s connection table entry is updated to reflect the
									connection was broken by the other end.  Both these states are
									seen from the programmer’s view as sessionClosedErr if he tries
									to post using for him what is a valid connection.  He must
									repost his message to establish a new connection with his
									target.
									•6	In previous releases, if the second PPCRead failed, the
									connection was broken and the target’s session table entry is
									updated as in •4.  However in the second read there are two
									different classes of errors – memory errors and PPCToolbox
									errors.  In this release, memory errors cause the system to
									restart the second read at a later time when, hopefully, the
									memory configuration has changed.  This change keeps the system
									from breaking a session because of memory shortage.  In the
									previous release breaking the connection because of memory
									failure caused the PPCWrites on the other end of the connection
									to fail.  Sometimes there were a lot of writes.  For instance in
									the Large Data Buffer test.
	<2>	10/30/90	csd		Update the symbols to match the headers on BBS.
	<0>	 9/02/89	JM		New Today.

*/ 

#include <Errors.h>
#include <MFPrivate.h>
#include <Memory.h>
#include <MemoryMgrPriv.h>
#include <PPCToolBox.h>
#include <AppleEventsInternal.h>
#include <Errors.h>
#include <Resources.h>
#include <Values.h>
#include <Script.h>

#include "Data.h"
#include "Glue.h"
#include "Zone.h"
#include "Lomem.h"
#include "Patches.h"
#include "AppleEventExtensions.h"

#define IS		==
#define ISNT 	!=
#define AND 	&&
#define OR 		||

#define ppcOpenCmd		1
#define ppcStartCmd		2
#define ppcInformCmd	3
#define ppcAcceptCmd	4
#define ppcRejectCmd	5
#define ppcWriteCmd		6
#define ppcReadCmd		7
#define ppcEndCmd		8
#define ppcCloseCmd		9
#define IPCListPortsCmd	10

typedef short		ConnectionType;
#define eppcConnection				(0)
#define systemReceiverConnection	(1)
#define systemSenderConnection		(2)
#define noEppcConnection			(3)
#define badConnection				(4)

#define noSessionID					(0)
#define systemSessionID				(-1)

#define INITIALIZED(eBlk)		(eBlk->portID != 0)
#define TarEPPCBlkPtr(pPtr)		(&(pPtr->eppcBlk))
#define CanDoEPPC(pPtr)			(INITIALIZED(TarEPPCBlkPtr(pPtr)))

/* status bits in the Inform, Read and Write parameter blocks */

#define closeInProgress 		0x8000

/* status bits in cTableEntry */

#define informConnection 		0x8000
#define startConnection 		0x4000
#define sessionClosed			0x2000
#define sessionEnded			0x1000
#define waitingForWrites		0x0800
#define HLEQDoesNotExist		0x0400

#define sizeOfPPCpbHeader		(sizeof(LINKTYPE)+sizeof(Handle)+sizeof(EPPCBlkPtr)+2*sizeof(unsigned long))
#define sizeOfPPCWriteBlk		(sizeOfPPCpbHeader+sizeof(PPCWritePBRec))
#define sizeOfPPCReadBlk		(sizeOfPPCpbHeader+sizeof(PPCReadPBRec))
#define sizeOfPPCInformBlk		(sizeOfPPCpbHeader+sizeof(PPCInformPBRec))

#define CheckForFatalError		assert

#define SYNCHRONOUS				0
#define ASYNCHRONOUS			1

#define NROFINFORMPBS			3

#define MAXNROFTRIES			99

typedef struct ppcWriteBlk {
	LINKTYPE			ppcWriteBlkQ;
	Handle				pbHdl;
	EPPCBlkPtr			pbID;
	unsigned long		portID;
	unsigned long		status;
	PPCWritePBRec		ppcWritePB;
	MFmsgBlk			mfMsgBlk;
	} ppcWriteBlk, *ppcWritePBPtr;

typedef struct ppcReadBlk {
	LINKTYPE			ppcReadBlkQ;
	Handle				pbHdl;
	EPPCBlkPtr			pbID;
	unsigned long		portID;
	unsigned long		status;
	PPCReadPBRec		ppcReadPB;
	MFmsgBlk			mfMsgBlk;
	} ppcReadBlk, *ppcReadPBPtr;

typedef struct ppcInformBlk {
	LINKTYPE			ppcInformBlkQ;
	Handle				pbHdl;
	EPPCBlkPtr			pbID;
	unsigned long		portID;
	unsigned long		status;
	PPCInformPBRec		ppcInformPB;
	PPCRejectPBRec		ppcInformReplyPB;		// <13>
	PPCPortRec			portName;
	LocationNameRec		portLoc;
	} ppcInformBlk, *ppcInformPBPtr;

typedef struct cTableEntry {
	LINKTYPE			sLList;		/* queue element		*/
	Handle				sHdl;		/* handle for element	*/
	unsigned long		sportID;	/* port id for _ExitToShell fix */
	unsigned long 		sID;		/* session id 			*/
	short 				status;		/* status word 			*/
	PPCReadPBPtr		srPB;		/* outstanding read		*/
	PPCWritePBPtr		swPB;		/* outstanding write	*/
	EPPCBlkPtr			sEppcBlkPtr;/* eppc control block	*/
	PPCPortRec 			sPName;		/* whose port name - depends on status */
	LocationNameRec 	sLocName;	/* whose location - depends on status  */
	} cTableEntry, *cTableEntryPtr, **cTableEntryHdl;
	
void 				KnockKnock(PPCParamBlockPtr ppcPB);
ConnectionType 		GetConnectionType(ProcessSerialNumberPtr postersPSN, unsigned long receiverID, unsigned long postingOptions, unsigned long *connectionID, ProcessSerialNumberPtr targetsPSN, OSErr *err);
unsigned long 		validateSessionID(ProcessSerialNumberPtr ownersPSN, unsigned long sessionID, OSErr *err);
Boolean				PSNFromName(const PPCPortPtr portName, ProcessSerialNumberPtr pPSN);
Boolean				PSNFromSignature(const ResType signature, ProcessSerialNumberPtr pPSN);
Boolean				findElement(char *A, char *B);
Boolean				byPBID(ppcWritePBPtr A, EPPCBlk *B);
MFmsgBlkPtr			GetMsgEventBuffer (u_long lengthInBytes, OSErr *err);
unsigned long 		getSessionID(ProcessSerialNumberPtr sendersPSN, TargetIDPtr portN, OSErr *err);
EPPCBlkPtr 			EPPCBlkFromPSN(ProcessSerialNumberPtr psn);
PPCParamBlockPtr 	getPPCInformPB(EPPCBlkPtr refCon, unsigned short portRefNum, OSErr *err);
PPCParamBlockPtr 	getPPCReadPB(EPPCBlkPtr refCon, unsigned long sessionID, OSErr *err);
void 				releasePPCpb(PPCParamBlockPtr pb);
void 				postMsgToPPC(const MFmsgBlkPtr msgBlk, unsigned long sessionID, OSErr *err);
MFmsgBlkPtr 		fetchMsgData(EPPCBlkPtr eppcBlkPtr, MFmsgBlkPtr mfMsgBlk, OSErr *err);
void				scanInformBQ(void);
void				scanWriteBQ(void);
void				scanReadBQ(void);
void				getSessionRecord(SenderIDPtr senderID, EPPCBlk *recvrEppcB);
HighLevelEventMsgHdl
					copyMsg(MFmsgBlkPtr aMsg, OSErr *err);
Boolean				guestAllowed(TargetIDPtr portN, OSErr *err);
Boolean				comparePortLocation(LocationNamePtr L1, LocationNamePtr L2);
Boolean				comparePortNames(PPCPortPtr N1, PPCPortPtr N2);
void				closePPCPort(unsigned long portID);
void				openPPCPort(EPPCBlkPtr eppcB);
Boolean				PortNameAndLocationAreEqual(PPCPortPtr portName, LocationNamePtr locName, TargetIDPtr targetID);
TargetID			getTIDfromEppcBlk(EPPCBlkPtr eppcBPtr);
ppcReadPBPtr		startPPCRead(EPPCBlkPtr eppcBPtr, unsigned long sessionID, OSErr *err);
ppcReadPBPtr		restartPPCRead(ppcReadPBPtr rPB, OSErr *err);
PEntryPtr			PEntryFromPSN(ProcessSerialNumberPtr pPSN);
cTableEntryPtr		findSessionRecord(unsigned long sessionRefNum);
cTableEntryPtr		findFreeSessionRecord(void);
void				makePortName(EPPCBlkPtr eppcB);
void				doDuplicateNameOpen(EPPCBlkPtr eppcB, PPCOpenPBPtr PPCpb);
void				GetNextPortName(EPPCBlkPtr eppcB);
void				DeleteBQEntries(unsigned long portID, LINKLIST *theBQ);
void				qPPCPb(ppcWritePBPtr wPBPtr, LINKLIST *BQList);
void 				FlushHLEQueue(EPPCBlkPtr eppcBlk, Boolean sendReturnReceipts, short rtnMsg);
Boolean				NameFromPSN(PPCPortPtr portName, ProcessSerialNumberPtr pPSN);
Boolean				bySessionID(cTableEntryPtr A, unsigned long *B);
Boolean				bysEppcBlkPtr(cTableEntryPtr A, EPPCBlk *B);
Boolean				byportID(cTableEntryPtr A, unsigned long *B);
Boolean				byportIDforPB(ppcWriteBlk *A, unsigned long *B);
short				breakConnections(unsigned long portID);
void				endSessionAfterAReadError(cTableEntryPtr sRec, OSErr err);

#pragma parameter DisableInterrupts(__A0)
void DisableInterrupts(unsigned short *oldMask) = {0x40D0, 0x007C, 0x0700};
/*	move	sr, (a0)
 *	ori.w	0x0700, sr
 */

#pragma parameter EnableInterrupts(__D0)
void EnableInterrupts(unsigned short oldMask) = {0x46C0};
/*	move.w	d0,sr
 */

typedef pascal Boolean
(*hleFilter_ptr)(unsigned long *params, Ptr msgBuff, SenderIDPtr senderID);

typedef pascal OSErr
(*StartSecureSession_ptr)(PPCStartPBPtr,
								  Str32					userName,
								  Boolean 				useDefault,
								  Boolean 				allowGuest,
								  Boolean 				*guestSelected,
								  ConstStr255Param		prompt);

#pragma segment	eppc_segment

StringPtr		SSSPromptpStr;				/* prompt string for StartSecureSession */
StringPtr		MacOSPortNamepStr;			/* port name of kSystemProcess */
StringPtr		MacOSPortTypepStr;			/* port type of kSystemProcess */

LINKLIST eppcBQ = {nil,nil,nil,0,0,sizeof(EPPCBlk),nil};
LINKLIST ppcInformBQ = {nil,nil,nil,0,0,sizeof(ppcInformBlk),nil};
LINKLIST ppcReadBQ = {nil,nil,nil,0,0,sizeof(ppcReadBlk),nil};
LINKLIST ppcWriteBQ = {nil,nil,nil,0,0,sizeof(ppcWriteBlk),nil};

LINKLIST connectionQ = {nil,nil,nil,0,0,sizeof(cTableEntry),nil};

/*____________________________________________________________________________________________*/
void
ePPCInit(void)
	{
	Handle hdl;

	/* Ask the PPC Toolbox to initialize.*/
	ppcInitErr = PPCInit();

	/* Check for acceptable result, then try for our string resource */
	if ((hdl = Get1Resource('STR#',-16415)) ISNT nil)
		{
		MoveHHi(hdl);
		HLock(hdl);
		
		SSSPromptpStr = *hdl+(Ptr)2;
		
		MacOSPortNamepStr = (Ptr)SSSPromptpStr + (Ptr)(*(char *)SSSPromptpStr);
		MacOSPortNamepStr += (Ptr)1;
		
		MacOSPortTypepStr = (Ptr)MacOSPortNamepStr + (Ptr)(*(char *)MacOSPortNamepStr);
		MacOSPortTypepStr += (Ptr)1;
		}
	}
/*____________________________________________________________________________________________*/
pascal OSErr
c_GetPortNameFromPSN(PPCPortPtr portName, ProcessSerialNumberPtr pPSN)
	{
	unsigned long	olda5;
	Boolean			foundIt;
	
	olda5 = ProcessMgrA5SimpleSetup();
	foundIt = NameFromPSN(portName,pPSN);
	A5SimpleRestore(olda5);
	return (foundIt) ? noErr : noPortErr;
	}
/*____________________________________________________________________________________________*/
pascal OSErr 	
c_GetPSNFromPortName(PPCPortPtr portName, ProcessSerialNumberPtr psnStorage)
	{
	unsigned long	olda5;
	Boolean			foundIt;
	
	olda5 = ProcessMgrA5SimpleSetup();
	foundIt = PSNFromName(portName, psnStorage);
	A5SimpleRestore(olda5);
	return (foundIt) ? noErr : procNotFound;
	}
/*____________________________________________________________________________________________*/
pascal Boolean
c_GetSpecificHighLevelEvent(ProcPtr aFilter, unsigned long *params, OSErr *err) 
	{
	Boolean					answer = false;
	HighLevelEventMsgHdl	msgCopyHandle;
	unsigned long			olda5;
	MFmsgBlkPtr				aMsg, bMsg, outstandingMsg;
	Ptr						theBuff;
	EPPCBlkPtr				eppcB;
	int						priority;
	SenderID				senderID;

	olda5 = ProcessMgrA5SimpleSetup();
	
	scanInformBQ();
	scanReadBQ();
	scanWriteBQ();
	
	assert(pCurrentProcess ISNT pNullProcess);
	assert(EqualPSNConst(kSystemProcess, &pCurrentProcess->p_serialNumber) IS false);
	
	*err = noErr;
	eppcB = &pCurrentProcess->eppcBlk; /* It maybe a bad assumption that pCurrentProcess an app */
	
	if (eppcB->portID IS 0)
		*err = noPortErr;
	if (aFilter IS NULL)
		*err = paramErr;
	if (*err ISNT noErr)
		{
		A5SimpleRestore(olda5);
		return(answer);
		}
		
	if (eppcB->mfMsgBlk ISNT nil)
		{ 
		aMsg = eppcB->mfMsgBlk;
		msgCopyHandle = copyMsg(aMsg, err);
		if (*err IS noErr) ;
		else
			{
			A5SimpleRestore(olda5);
			return(answer);
			}
		theBuff = *msgCopyHandle;
		aMsg->msgStatus |= getSpecificHLE;
		senderID.sessionID = (aMsg->sendermfid.sessionID IS systemSessionID) ? systemSessionID : aMsg->targetmfid.sessionID;
		getSessionRecord(&senderID, eppcB);
		A5SimpleRestore(olda5);
		answer = CALL_FNC_PTR(hleFilter_ptr, aFilter, (params, theBuff, &senderID));
		olda5 = ProcessMgrA5SimpleSetup();
		DisposHandle(msgCopyHandle);
		CheckForFatalError(MEMERROR IS noErr);
		aMsg->msgStatus &= ~getSpecificHLE;
		if (aMsg->msgStatus & msgPartialyAccepted)
			{
			aMsg->msgStatus &= ~msgPartialyAccepted;
			aMsg->MFRefcon = 0;
			if (aMsg->msgStatus & msgCompletelyAccepted)
				flushMsg(aMsg,msgWasFullyAccepted);
			else
				flushMsg(aMsg,msgWasPartiallyAccepted);
			eppcB->mfMsgBlk = nil;
			}
		if (answer != false)
			{
			A5SimpleRestore(olda5);
			return(answer);
			}
		}
	
	outstandingMsg = eppcB->mfMsgBlk;
	
	for (priority=1;priority>=0;--priority) {
		qset(&(eppcB->msgQ[priority]));
		if (ll_length() IS 0)
			aMsg = nil;
		else
			{
			lltail();
			llretrieve(&aMsg);
			}
		while (aMsg ISNT nil)
			{
			msgCopyHandle = copyMsg(aMsg, err);
			if (*err IS noErr) ;
			else {
				answer = false;
				eppcB->mfMsgBlk = outstandingMsg;
				A5SimpleRestore(olda5);
				return(answer);
				}
			theBuff = *msgCopyHandle;
			eppcB->mfMsgBlk = aMsg;
			aMsg->msgStatus |= getSpecificHLE;
			senderID.sessionID = (aMsg->sendermfid.sessionID IS systemSessionID) ? systemSessionID : aMsg->targetmfid.sessionID;
			getSessionRecord(&senderID, eppcB);
			A5SimpleRestore(olda5);
			answer = CALL_FNC_PTR(hleFilter_ptr, aFilter, (params, theBuff, &senderID));
			olda5 = ProcessMgrA5SimpleSetup();
			DisposHandle(msgCopyHandle);
			CheckForFatalError(MEMERROR IS noErr);
			aMsg->msgStatus &= ~getSpecificHLE;
			qset(&(eppcB->msgQ[priority]));			/* the user may have called something which
													   undid our call to qset(), so redo it     */
			
			if (aMsg->msgStatus & msgPartialyAccepted)		/* was AcceptHighLevelEvent called? */
				{
				aMsg->msgStatus &= ~msgPartialyAccepted;
				aMsg->MFRefcon = 0;							/* in case we AcceptHLEvent this one again */
				
				bMsg = aMsg->MFmsgBlkQ.previous;
				lldelete();
				flushMsg(aMsg,(aMsg->msgStatus & msgCompletelyAccepted) ? msgWasFullyAccepted:msgWasPartiallyAccepted);

				qset(&(eppcB->msgQ[priority]));			/* flushMsg may have called something which
														   undid our call to qset(), so redo it     */
				if (ll_length())						/* now find our next HLE */
					if (bMsg ISNT nil) 
						{
						eppcB->msgQ[priority].clp = bMsg;
						aMsg = bMsg;					/* llprevious() would return head of list */
						}
					else
						{
						aMsg = nil;
						}
				else
					aMsg = nil;							/* we deleted the last HLE */
				}
			else
				{
				if (llprevious())
					llretrieve(&aMsg);
				else
					aMsg = nil;
				}
			if (answer != false)
				break;
			} 
		if (answer != false)
			break;
		} /* end of 'for loop' for each HLEvent Q attached to this port */
		
	eppcB->mfMsgBlk = outstandingMsg;
	A5SimpleRestore(olda5);
	return(answer);
	}
/*____________________________________________________________________________________________*/
HighLevelEventMsgHdl
copyMsg(MFmsgBlkPtr aMsg, OSErr *err)
	{
	HighLevelEventMsgHdl		aHdl;
	Ptr							theCopyPtr;

	aHdl = ProcessMgrHiNewHandle(sizeof(HighLevelEventMsg)+aMsg->eppcMsgBlk.msgLength, nil);
	if ((*err = MEMERROR) != noErr)
		return(aHdl);
	HLock(aHdl);
	theCopyPtr = *aHdl;
	BlockMove(&aMsg->eppcMsgBlk,theCopyPtr,sizeof(HighLevelEventMsg));
	theCopyPtr += (Ptr)sizeof(HighLevelEventMsg);
	BlockMove(aMsg->addrOfMsg,theCopyPtr,aMsg->eppcMsgBlk.msgLength);
	return(aHdl);
	}
/*____________________________________________________________________________________________*/
pascal OSErr
c_AcceptHighLevelEvent(SenderIDPtr senderID, u_long *msgRefcon, Ptr msgBuff, u_long *msgLen)
	{
	unsigned long	olda5;
	short			err;
	unsigned long	xferCount;
	MFmsgBlkPtr		mfMsgBlk;
	
	/* save away the current caller's state, set major zone */
	
	olda5 = ProcessMgrA5SimpleSetup();
	err = noErr;
	
	/* not sure about reference to pCurrentProcess being assumable. Bad things happen
	 * if it doesn't point to an app.
	 */
	 
	if (pCurrentProcess->eppcBlk.mfMsgBlk) { /* is there a message for accepting? */
	
	/* mfMsgBlk was set in event_common if we were delivering a pEvent.what = kHighLevelEvent  or in 
	  * GetSpecificHighLevelEvent.
	  */

		mfMsgBlk = (pCurrentProcess->eppcBlk.mfMsgBlk);
		mfMsgBlk->msgStatus |= msgPartialyAccepted;
		
		/* Can message fit into the user's buffer? */
		
		if ((mfMsgBlk->eppcMsgBlk.msgLength - mfMsgBlk->MFRefcon) > *msgLen) {
			
			/* message is larger than user's buffer */
			
			xferCount = *msgLen; /* move what we can */
			err = bufferIsSmall; /* tell the user his buffer is small */
								 /* tell the user how much is left to move */
			*msgLen = mfMsgBlk->eppcMsgBlk.msgLength - mfMsgBlk->MFRefcon - *msgLen;
			}
		else {
			/* message fits fine */
			
			xferCount = mfMsgBlk->eppcMsgBlk.msgLength - mfMsgBlk->MFRefcon; 
			*msgLen = xferCount;
			}
		
		/* Move message */
		
		if (msgBuff ISNT NULL && xferCount ISNT 0)
			BlockMove(mfMsgBlk->addrOfMsg + (Ptr)mfMsgBlk->MFRefcon,msgBuff,xferCount);
		*msgRefcon = mfMsgBlk->eppcMsgBlk.userRefcon;
		senderID->sessionID = (mfMsgBlk->sendermfid.sessionID IS systemSessionID) ? systemSessionID : mfMsgBlk->targetmfid.sessionID;
		getSessionRecord(senderID, &pCurrentProcess->eppcBlk);
		
		/* Check to see if we moved all the message */
		
		mfMsgBlk->MFRefcon += xferCount;
		if (mfMsgBlk->MFRefcon >= mfMsgBlk->eppcMsgBlk.msgLength) {
			
			/* message is all moved.  Send the return receipt and release the memory */
			
			mfMsgBlk->msgStatus |= msgCompletelyAccepted;
			flushMsg(mfMsgBlk,msgWasFullyAccepted);
			pCurrentProcess->eppcBlk.mfMsgBlk = nil;
			}
		}
	else
		err = noOutstandingHLE; /* Humm…  no High Level Event.  His mistake or mine? */
	
	
	/* restore caller's world */
	
	A5SimpleRestore(olda5);
	return(err);
	}
/*____________________________________________________________________________________________*/
void
getSessionRecord(SenderIDPtr senderID, EPPCBlk *recvrEppcB)
	{
	cTableEntryPtr	sRec;

	MemClear(&senderID->location, sizeof(LocationNameRec));
	MemClear(&senderID->name, sizeof(PPCPortRec));
	MemClear(&senderID->recvrName, sizeof(PPCPortRec));
	sRec = findSessionRecord(senderID->sessionID);
	if (sRec ISNT nil)
		{
		senderID->name = sRec->sPName;
		if (sRec->sLocName.locationKindSelector IS ppcNBPLocation)
			senderID->location = sRec->sLocName;
		else
			senderID->location.locationKindSelector = ppcNoLocation;
		senderID->recvrName = sRec->sEppcBlkPtr->nameOfMsgQ;
		}
	else
		{
		if (senderID->sessionID IS systemSessionID)
			{
			BlockMove(MacOSPortNamepStr, &senderID->name.name, *(char *)MacOSPortNamepStr+1);
			senderID->name.portKindSelector = ppcByString;
			BlockMove(MacOSPortTypepStr, &senderID->name.u.portTypeStr, *(char *)MacOSPortTypepStr+1);
			senderID->recvrName = recvrEppcB->nameOfMsgQ;
			}
		}
	}
/*____________________________________________________________________________________________*/
void 
FlushHLEQueue(EPPCBlkPtr eppcBlk, Boolean sendReturnReceipts, short rtnMsg)
	{
	MFmsgBlkPtr aMsg=eppcBlk->mfMsgBlk;
	int priority;

	if (aMsg ISNT nil)
		if (sendReturnReceipts) 
			flushMsg(aMsg, rtnMsg);
		else
			{
			RelMsgEventBuffer(aMsg);
			}
		
	for (priority=1;priority>=0;priority--)
		{
		LINKLIST *theMsgQ = &(eppcBlk->msgQ[priority]);
		
		qset(theMsgQ);
		llhead();
		while(ll_length() ISNT 0)
			{
			llretrieve(&aMsg);
			lldelete();
			if (sendReturnReceipts) 
				{
				flushMsg(aMsg, rtnMsg);
				qset(theMsgQ);				/* flush queue may undo our qset, so redo it. */
				}
			else
				{
				RelMsgEventBuffer(aMsg);
				}
			}
		}
	}
/*____________________________________________________________________________________________*/
void
flushMsg(MFmsgBlkPtr msgBlk, unsigned short rtnModifiers)
	{
	if ((msgBlk->eppcMsgBlk.postingOptions & nReturnReceipt) AND ((msgBlk->msgStatus & getSpecificHLE) IS 0))
		postReturnReceipt(msgBlk,rtnModifiers);
	RelMsgEventBuffer(msgBlk);
	}
/*_____________________________________fetchMsgData______________________________________*/
MFmsgBlkPtr
fetchMsgData(EPPCBlkPtr eppcBlkPtr, MFmsgBlkPtr mfMsgBlk, OSErr *err)
	{
	ppcReadPBPtr		readPB;
	Handle				aHdl;

	aHdl = GMBlk(sizeof(ppcReadBlk)+mfMsgBlk->eppcMsgBlk.msgLength, err);
	if (*err ISNT noErr)
		return nil;
			
	HLock(aHdl);
	readPB = *aHdl;
	MemClear(readPB,sizeof(ppcReadBlk)+mfMsgBlk->eppcMsgBlk.msgLength);
	readPB->pbHdl = aHdl;
	readPB->pbID = eppcBlkPtr;
	readPB->ppcReadPB.bufferPtr = readPB + 1; // <9>
	readPB->ppcReadPB.bufferLength = mfMsgBlk->eppcMsgBlk.msgLength;
	readPB->mfMsgBlk = *mfMsgBlk;
	readPB->mfMsgBlk.addrOfMsg = readPB->ppcReadPB.bufferPtr;
	readPB->mfMsgBlk.pbHandle = aHdl;
	
	readPB->ppcReadPB.csCode = ppcReadCmd;
	readPB->ppcReadPB.ioCompletion = nil;
	readPB->ppcReadPB.sessRefNum = mfMsgBlk->targetmfid.sessionID;
	
	if (mfMsgBlk->eppcMsgBlk.msgLength IS 0);
	else
		*err = PPCRead(&readPB->ppcReadPB,SYNCHRONOUS);
	return (&readPB->mfMsgBlk);
	}
/*____________________________________________________________________________________________*/
void
postReturnReceipt(const MFmsgBlkPtr mfMsgBlk, signed short rtnModifiers)
	{
	EventRecord		theEvent;
	u_long			receiverID;
	u_long			msgRefcon;
	u_long			postingOptions;
	
	if (mfMsgBlk->targetmfid.sessionID IS systemSessionID)
		{
		receiverID = &mfMsgBlk->sendermfid.localPSN;
		postingOptions = receiverIDisPSN;
		}
	else
		{
		receiverID = mfMsgBlk->targetmfid.sessionID;
		postingOptions = receiverIDisSessionID;
		}
	msgRefcon = mfMsgBlk->eppcMsgBlk.userRefcon;
	
	theEvent.message = HighLevelEventMsgClass;
	LONG_TO_PT(rtrnReceiptMsgID,&theEvent.where);
	theEvent.modifiers = rtnModifiers;
	
	PostHighLevelEvent(&theEvent, receiverID, msgRefcon, nil, 0, postingOptions);
	}
/*____________________________________________________________________________________________*/
pascal short
c_PostHighLevelEvent(EventRecord *theEvent, u_long receiverID, u_long msgRefcon, Ptr msgBuff, u_long msgLen, u_long postingOptions)
	{
	ProcessSerialNumber		SendersPSN;
	unsigned long  			olda5, connectionID;
	short					err, anotherErr;
	ConnectionType			connectionType;
	MFmsgBlkPtr	   			msgBlk;
	ProcessSerialNumber		aPSN;
		
	olda5 = ProcessMgrA5SimpleSetup();
	err = noErr;

	/* Make sure msgLen is reasonable */
	if (msgBuff == nil)
		msgLen = 0;
	else if (msgLen > 0x0000ffffL)
		{
		A5SimpleRestore(olda5);
		return(ddpLenErr);
		}

	(void) GetSystemClientProcess(&SendersPSN);
	connectionType = GetConnectionType(&SendersPSN, receiverID, postingOptions, &connectionID, &aPSN, &err);
	if (connectionType == badConnection)
		{
		A5SimpleRestore(olda5);
		return(err);
		}

	/* badConnection has been weeded out. Try and get memory for message and necessary control info */
	
	if ((msgBlk = GetMsgEventBuffer(msgLen, &anotherErr)) IS nil)
		{
		A5SimpleRestore(olda5);
		return(anotherErr);
		}
	
	/* Set up the event record we'll give to the target */
	msgBlk->eppcMsgBlk.theMsgEvent = *theEvent;
	msgBlk->eppcMsgBlk.theMsgEvent.what = kHighLevelEvent;
	msgBlk->eppcMsgBlk.theMsgEvent.when = TICKS;
	
	/* Set up the message and refcon */
	if (msgLen != 0)
		BlockMove(msgBuff, msgBlk->addrOfMsg, msgLen);
	msgBlk->eppcMsgBlk.userRefcon = msgRefcon;
	msgBlk->MFRefcon = 0;
	msgBlk->eppcMsgBlk.postingOptions = postingOptions;
	
	/* Set up routing information */
	msgBlk->sendermfid.localPSN = SendersPSN;
	msgBlk->targetmfid.localPSN = aPSN;
	msgBlk->sendermfid.sessionID = connectionID;
	msgBlk->targetmfid.sessionID = noSessionID;
	
	/* msgBlk is complete.  Put it where it belongs based on connectionType. */
	if (connectionType == eppcConnection)
		{
		postMsgToPPC(msgBlk, connectionID, &anotherErr);
		if ((postingOptions & receiverIDMask) == receiverIDisTargetID)
			((TargetIDPtr)receiverID)->sessionID = (anotherErr == noErr) ? connectionID : noSessionID;
		err = anotherErr;
		}
	else if (connectionType == systemSenderConnection)
		{
		msgBlk->msgStatus |= localOnly;
		postMsg(msgBlk);
		}
	else if (connectionType == systemReceiverConnection)
		{
		HandleSystemMessage(msgBlk, &err);
		}
	else if (connectionType == noEppcConnection)
			TranslateAppleEvent(msgBlk, &err);
		
	A5SimpleRestore(olda5);
	return(err);
	}
/*____________________________________________________________________________________________*/
void
CreateMsgQ(PEntryPtr pProc)
	{
	EPPCBlk			eppcB;
	short			i;
	
	MemClear(&eppcB,sizeof(EPPCBlk));
	
	eppcB.pTablePtr = pProc;
	qset(&eppcB.msgQ[0]);
	qitemsize(sizeof(MFmsgBlk)); /* actually represents the header size only */
	qset(&eppcB.msgQ[1]);
	qitemsize(sizeof(MFmsgBlk)); /* actually represents the header size only */
	
	/* Build eppcB port name from resource or p_taskmode */
	
	makePortName(&eppcB);
	
	openPPCPort(&eppcB);

	if (eppcB.createErr IS noErr)
	
		/* carve out a parameter block for a async PPCInform call and start the Informs*/
		
		for (i=0;i<NROFINFORMPBS;i++)
			{
			eppcB.iPBs[i] = (PPCInformPBPtr)getPPCInformPB(&pProc->eppcBlk, eppcB.portID, &eppcB.createErr);	
			if (eppcB.createErr ISNT noErr)
				break;
			eppcB.createErr = PPCInform(eppcB.iPBs[i],ASYNCHRONOUS);
			if (eppcB.createErr ISNT noErr)
				break;
			} /* end of for loop */
			
	if (eppcB.createErr ISNT noErr)
		closePPCPort(eppcB.portID);
		
	pProc->eppcBlk = eppcB;
	if (eppcB.createErr IS noErr)
		{
		qset(&eppcBQ);
		q_push(&pProc->eppcBlk);
		}
	}
/*____________________________________________________________________________________________*/
void
makePortName(EPPCBlkPtr eppcB)
	{
	Ptr				namePtr = nil;
	short			lenOfPortName;
	Handle			hdl;
	
	if ((eppcB->pTablePtr->p_taskmode & modeHighLevelEventAware) AND ((hdl = (Handle)Get1Resource('eppc',0)) ISNT nil))
		{		
		namePtr = *hdl;
		eppcB->optionFlags = *(unsigned long *)namePtr;
		if (eppcB->pTablePtr->p_taskmode & modeLocalAndRemoteHLEvents)
			eppcB->optionFlags |= registerOnNetwork;
		namePtr += (Ptr)sizeof(unsigned long);
		eppcB->reserved1 = *(unsigned long *)namePtr;
		namePtr += (Ptr)sizeof(unsigned long);
		
		eppcB->nameOfMsgQ.nameScript = *(short *)namePtr;
		namePtr += (Ptr)sizeof(short);
		lenOfPortName = *(char *)namePtr;
		namePtr += (Ptr)sizeof(char);
		eppcB->nameOfMsgQ.name[0] = lenOfPortName;
		BlockMove(namePtr,&eppcB->nameOfMsgQ.name[1],lenOfPortName);
		eppcB->nameOfMsgQ.portKindSelector = ppcByString;
		eppcB->nameOfMsgQ.u.portTypeStr[0] = 8;
		BlockMove((Ptr)&eppcB->pTablePtr->p_signature,(Ptr)&eppcB->nameOfMsgQ.u.portTypeStr[1],4);
		BlockMove("ep01",(Ptr)&eppcB->nameOfMsgQ.u.portTypeStr[5],4);
		ReleaseResource(hdl);
		return;
		}

	if (eppcB->pTablePtr->p_taskmode & modeHighLevelEventAware)
		{
		if (eppcB->pTablePtr->p_taskmode & modeLocalAndRemoteHLEvents)
			eppcB->optionFlags |= registerOnNetwork;
		eppcB->reserved1 = 0;
		eppcB->nameOfMsgQ.nameScript = (short)GetEnvirons(smSysScript);
		BlockMove(CURAPNAME, &eppcB->nameOfMsgQ.name, *((unsigned char *)CURAPNAME) + 1);
		eppcB->nameOfMsgQ.portKindSelector = ppcByString;
		eppcB->nameOfMsgQ.u.portTypeStr[0] = 8;
		BlockMove((Ptr)&eppcB->pTablePtr->p_signature,(Ptr)&eppcB->nameOfMsgQ.u.portTypeStr[1],4);
		BlockMove("ep01",(Ptr)&eppcB->nameOfMsgQ.u.portTypeStr[5],4);
		return;
		}
	eppcB->createErr = -1;
	}
/*____________________________________________________________________________________________*/
void
openPPCPort(EPPCBlkPtr eppcB) {
	PPCOpenPBRec 	PPCpb;
	
	if (eppcB->createErr ISNT noErr)
		return;
	MemClear(&PPCpb,sizeof(PPCOpenPBRec));
	PPCpb.csCode = ppcOpenCmd;
	PPCpb.ioCompletion = NULL;
	PPCpb.serviceType = ppcServiceRealTime;
	
	if (eppcB->optionFlags & registerOnNetwork)
		PPCpb.networkVisible = true;
	
	PPCpb.portName = &eppcB->nameOfMsgQ;
	PPCpb.locationName = NULL;
	
	eppcB->createErr = PPCOpen(&PPCpb, SYNCHRONOUS);
	
	/* If error was name duplication, try adjusting the name until it works */
	if (eppcB->createErr IS portNameExistsErr)
		doDuplicateNameOpen(eppcB, &PPCpb);
	
	/* Check result of PPCOpen or doDuplicateNameOpen */
	if (eppcB->createErr IS noErr)	
		eppcB->portID = PPCpb.portRefNum;
	}
/*____________________________________________________________________________________________*/
void
doDuplicateNameOpen(register EPPCBlkPtr eppcB, register PPCOpenPBPtr PPCpb)
	{
	register short nrOfTries;
	
	nrOfTries = MAXNROFTRIES;
	do {
		GetNextPortName(eppcB);
		eppcB->createErr = PPCOpen(PPCpb, SYNCHRONOUS);
		} while ((eppcB->createErr IS portNameExistsErr) AND (--nrOfTries > 0));
	}
/*____________________________________________________________________________________________*/
void
GetNextPortName(EPPCBlkPtr eppcB)
	{
	char		lsd, msd;
	
	lsd = StringByte(eppcB->nameOfMsgQ.u.portTypeStr, 7);
	msd = StringByte(eppcB->nameOfMsgQ.u.portTypeStr, 6);
	if (lsd IS '9')
		{
		lsd = '0';
		if (msd IS '9')
			msd = '0';
		else
			msd++;
		}
	else
		lsd++;
	StringByte(eppcB->nameOfMsgQ.u.portTypeStr, 7) = lsd;
	StringByte(eppcB->nameOfMsgQ.u.portTypeStr, 6) = msd;		
	}
/*_________________________________________closePPCPort___________________________________*/
void
closePPCPort(unsigned long portID)
	{
	PPCClosePBRec closePB;
	
	if (portID ISNT 0) {
		closePB.csCode = ppcCloseCmd;
		closePB.ioCompletion = nil;
		closePB.portRefNum = portID;
		PPCClose(&closePB, SYNCHRONOUS);
		}
	DeleteBQEntries(portID, &ppcReadBQ);
	DeleteBQEntries(portID, &ppcWriteBQ);
	DeleteBQEntries(portID, &ppcInformBQ);
	return;
	}
/*_____________________________________________DestroyMsgQ____________________________________*/
void
DestroyMsgQ(EPPCBlk *eppcBlk)
	{
	unsigned short	oldMask = 0;
	short i;
	register PPCParamBlockPtr	*pIPB;

	if (INITIALIZED(eppcBlk) == false)
		return;
	
	/* tell KnockKnock to reject informs */
	
	pIPB = &eppcBlk->iPBs[0];
	DisableInterrupts(&oldMask);
	for (i=NROFINFORMPBS; i > 0; i--)
		{
		ppcInformPBPtr thePB = (Ptr)*pIPB - sizeOfPPCpbHeader; // <9>
		
		thePB->status |= closeInProgress;
		pIPB++;
		}
	EnableInterrupts(oldMask);
	
	/* take care of outstanding messages */

	FlushHLEQueue(eppcBlk, false, 0);

	if (breakConnections(eppcBlk->portID) IS 0)
		closePPCPort(eppcBlk->portID);
	
	/* remove the eppc block from the chain because its memory is going away*/
	
	qset(&eppcBQ);
	llsetMatch((ProcPtr)findElement);
	llhead();
	if (llcheck(eppcBlk))
		lldelete();
	else
		assert(0);
	}
/*_______________________________________breakConnections_________________________________*/
short
breakConnections(unsigned long portID)
	{
	OSErr			ppcEndErr;
	PPCEndPBRec		endPB;
	cTableEntryPtr	sRec;
	short			nrOfSessionsLeftOpen = 0;

	qset(&connectionQ);
	if (ll_length() ISNT 0) 
		{
		llsetMatch((ProcPtr)byportID);
		llhead();
		while (llcheck(&portID))
			{
			llretrieve(&sRec);
			
			sRec->status |= HLEQDoesNotExist;

			if (sRec->swPB IS nil)
				{
				if (!(sRec->status & (sessionClosed+sessionEnded)))
					{
					endPB.csCode = ppcEndCmd;
					endPB.ioCompletion = nil;
					endPB.sessRefNum = sRec->sID;
					ppcEndErr = PPCEnd(&endPB,SYNCHRONOUS);
					if (ppcEndErr IS noErr OR ppcEndErr IS noSessionErr) ;
					else
						assert(0);
					}
				lldelete();
				DisposHandle(sRec->sHdl);
				}
			else
				{
				nrOfSessionsLeftOpen++;
				sRec->status |= waitingForWrites;
				if (!llnext())
					break;
				}
			}
		}
	return(nrOfSessionsLeftOpen);
	}
#if 0
/*____________________________________________DeleteBQEntries_________________________________*/
void
DeleteBQEntries(unsigned long portID, LINKLIST *theBQ)
	{
	ppcWritePBPtr 	pb;
	unsigned short	oldMask;

	qset(theBQ);
	DisableInterrupts(&oldMask);
	if (ll_length() ISNT 0) 
		{
		llsetMatch((ProcPtr)byportIDforPB);
		llhead();
		while (llcheck(&portID))
			{
			llretrieve(&pb);
			lldelete();
			DisposHandle(pb->pbHdl);
			}
		}
	EnableInterrupts(oldMask);
	}
#else
/*____________________________________________DeleteBQEntries_________________________________*/
void
DeleteBQEntries(unsigned long portID, LINKLIST *theBQ)
	{
	ppcWritePBPtr 	pb;
	ppcWritePBPtr 	nextPB;
	unsigned short	oldMask;
	LINKLIST		aListToDisposeOf;

	MemClear(&aListToDisposeOf, sizeof(LINKLIST));
	qset(theBQ);
	DisableInterrupts(&oldMask);
	++theBQ->listInUse;
	if (ll_length() ISNT 0) 
		{
		llhead();
		llretrieve(&pb);
		while (pb ISNT nil)
			{
			if (llnext())
				llretrieve(&nextPB);
			else
				nextPB = nil;
			if (pb->portID IS portID)
				{
				theBQ->clp = pb;
				lldelete();
				qset(&aListToDisposeOf);
				q_push(pb);
				qset(theBQ);
				}
			if (nextPB ISNT nil)
				theBQ->clp = nextPB;
			pb = nextPB;
			}
		}
	--theBQ->listInUse;
	EnableInterrupts(oldMask);
	
	qset(&aListToDisposeOf);
	while (ll_length() ISNT 0)
		{
		q_pop(&pb);
		DisposHandle(pb->pbHdl);
		}
	}
#endif
/*__________________________________GetMsgEventBuffer__________________________________________*/
MFmsgBlkPtr
GetMsgEventBuffer(u_long lengthInBytes, OSErr *err)
	{
	ppcWritePBPtr	msgBlk = nil;
	Handle			aHdl = nil;

	aHdl = GMBlk(sizeof(ppcWriteBlk)+lengthInBytes, err);
	if (*err ISNT noErr)
		return(nil);
	HLock(aHdl);
	msgBlk = *aHdl;
	MemClear(msgBlk,sizeof(ppcWriteBlk)+lengthInBytes);
	msgBlk->pbHdl = aHdl;
	msgBlk->mfMsgBlk.pbHandle = aHdl;
	msgBlk->mfMsgBlk.eppcMsgBlk.version = 3;
	msgBlk->mfMsgBlk.eppcMsgBlk.HighLevelEventMsgHeaderLength = sizeof(HighLevelEventMsg);
	msgBlk->mfMsgBlk.eppcMsgBlk.msgLength = lengthInBytes;
	msgBlk->mfMsgBlk.addrOfMsg = (lengthInBytes IS 0) ? NULL : msgBlk + 1; // <9>
	return(&msgBlk->mfMsgBlk);
	}
/*_____________________________________RelMsgEventBuffer________________________________________*/
void
RelMsgEventBuffer (MFmsgBlkPtr mfMsgBlk)
	{
	
	if (mfMsgBlk->msgStatus & getSpecificHLE)
		return;
	DisposHandle(mfMsgBlk->pbHandle);
	CheckForFatalError(MEMERROR IS noErr);
	}
/*____________________________________________________________________________________________*/
void
postMsg(const MFmsgBlkPtr msgBlk)
	{
	PEntryPtr		pTargetProc;
	EPPCBlkPtr 		teppcBlk;
	
	pTargetProc = PEntryFromPSN(&msgBlk->targetmfid.localPSN);
	CheckForFatalError(pTargetProc != nil);
	teppcBlk = TarEPPCBlkPtr(pTargetProc);
	if (msgBlk->eppcMsgBlk.postingOptions & nAttnMsg)
		qset(&(teppcBlk->msgQ[1]));
	else
		qset(&(teppcBlk->msgQ[0]));
	q_push(msgBlk);
	
	/* Wake him up, if necessary */
	CancelSleep(pTargetProc);
	}
/*____________________________________postMsgToPPC___________________________________________*/
void
postMsgToPPC(const MFmsgBlkPtr msgBlk, unsigned long sessionID, OSErr *err)
	{
	ppcWritePBPtr writePB;
	unsigned long *along;
	cTableEntryPtr sRec;
	
	writePB = (ppcWritePBPtr) ((Ptr)msgBlk - sizeOfPPCWriteBlk); // <9>
	
	writePB->ppcWritePB.csCode = ppcWriteCmd;
	writePB->ppcWritePB.ioCompletion = KnockKnock;
	writePB->ppcWritePB.sessRefNum = sessionID;
	writePB->ppcWritePB.blockCreator = msgBlk->eppcMsgBlk.theMsgEvent.message;
	along = &(msgBlk->eppcMsgBlk.theMsgEvent.where);
	writePB->ppcWritePB.blockType = *along;
	writePB->ppcWritePB.bufferLength = sizeof(HighLevelEventMsg)+msgBlk->eppcMsgBlk.msgLength;
	writePB->ppcWritePB.bufferPtr = &msgBlk->eppcMsgBlk;
	writePB->pbID = EPPCBlkFromPSN(&msgBlk->sendermfid.localPSN);
	
	/* The following code was added to keep track of the last PPCWrite.         */
	/* A new field was created in the connection table entry record (session    */
	/* record) - swPB. Into this field we store the address of the ppcWritePBPtr*/
	/* The swPB is used in ScanWriteBQ.  This is part of the fix for lost post 	*/
	/* during _ExitToShell														*/

	sRec = findSessionRecord(sessionID);
	sRec->swPB = writePB;
	writePB->portID = sRec->sportID;
	
	*err = PPCWrite(&writePB->ppcWritePB,ASYNCHRONOUS);
	
	}
/*_____________________________GetNextHighLevelEvent____________________________________________*/
pascal Boolean
c_GetNextHighLevelEvent(short eventmask, EventRecord *pReturnEvent, Boolean pullevent)
	{
	MFmsgBlkPtr		aMsg, bMsg;
	short			priority;
	unsigned long  	olda5;
	Boolean			retVal;
	
	olda5 = ProcessMgrA5SimpleSetup();
	retVal = false;
	scanInformBQ();
	scanReadBQ();
	scanWriteBQ();
	
	assert(pCurrentProcess ISNT pNullProcess);
	assert(EqualPSNConst(kSystemProcess, &pCurrentProcess->p_serialNumber) IS false);
	
	for (priority=1;priority>=0;priority--)
		{
		if (pCurrentProcess->eppcBlk.msgQ[priority].listlength ISNT 0 AND (eventmask & networkMask))
			{
			qset(&pCurrentProcess->eppcBlk.msgQ[priority]);
			lltail();
			llretrieve(&aMsg);
			*pReturnEvent = aMsg->eppcMsgBlk.theMsgEvent;
			if (pullevent)
				{
				q_pop(&bMsg);
				CheckForFatalError(bMsg IS aMsg);
				pCurrentProcess->eppcBlk.mfMsgBlk = (Handle)bMsg;
				}
				
			retVal = true;
			break;
			}
		}
		
	A5SimpleRestore(olda5);
	return(retVal);
	}
/*________________________________________scanInformBQ__________________________________________*/
void
scanInformBQ(void)
	{
	ppcInformPBPtr		iPB;
	PPCEndPBRec			ppcEndPB;
	OSErr				err;
	unsigned short		oldMask = 0;
	cTableEntryPtr		sRec;
	
	for (;;)
		{
		DisableInterrupts(&oldMask);
		++ppcInformBQ.listInUse;
		if (ppcInformBQ.listlength IS 0)
			{
			--ppcInformBQ.listInUse;
			EnableInterrupts(oldMask); 
			break;
			}
		qset(&ppcInformBQ);
		q_pop(&iPB);
		--ppcInformBQ.listInUse;
		EnableInterrupts(oldMask);
	
		if (iPB->ppcInformPB.ioResult ISNT noErr)
			{
			releasePPCpb(&iPB->ppcInformPB);
			continue;
			}
		
		if (iPB->ppcInformReplyPB.csCode IS ppcRejectCmd);
		else
			if ((sRec = findFreeSessionRecord()) ISNT nil)
				{
				sRec->status |= informConnection;
				if (iPB->status & closeInProgress)
					sRec->status |= HLEQDoesNotExist;
					
				sRec->sID = iPB->ppcInformPB.sessRefNum;
		
				sRec->sPName = *(iPB->ppcInformPB.portName);
				if (iPB->ppcInformPB.requestType IS ppcRemoteOrigin) 
					sRec->sLocName = *(iPB->ppcInformPB.locationName);
				else
					sRec->sLocName.locationKindSelector = ppcNoLocation;
		
				sRec->sEppcBlkPtr = iPB->pbID; /* pbID may no longer be valid */
				sRec->sportID = iPB->portID;
				
				sRec->srPB = startPPCRead(iPB->pbID, iPB->ppcInformPB.sessRefNum, &err);
				if (err ISNT noErr)
					DisposHandle(sRec->sHdl);
				else
					{
					qset(&connectionQ);
					q_push(sRec);
					}
				}
			else
				{
				MemClear(&ppcEndPB,sizeof(PPCEndPBRec));
				ppcEndPB.sessRefNum = iPB->ppcInformPB.sessRefNum;
				ppcEndPB.csCode = ppcEndCmd;
				ppcEndPB.ioCompletion = nil;
				PPCEnd(&ppcEndPB,SYNCHRONOUS);
				}
			
		iPB->ppcInformPB.sessRefNum = 0;
		MemClear(iPB->ppcInformPB.locationName, sizeof(LocationNameRec));
		MemClear(iPB->ppcInformPB.portName, sizeof(PPCPortRec));
		
		err = PPCInform(&iPB->ppcInformPB,ASYNCHRONOUS);
		}
	}
/*____________________________________scanWriteBQ_______________________________________________*/
void
scanWriteBQ(void)
	{
	ppcWritePBPtr 	wPB = nil;
	ppcWritePBPtr 	lastwPB = nil;
	unsigned short 	oldMask = 0;
	cTableEntryPtr 	sRec;
	PPCEndPBRec		ppcEndPB;
	
	for (;;)
		{
		DisableInterrupts(&oldMask);
		++ppcWriteBQ.listInUse;
		if (ppcWriteBQ.listlength IS 0)
			{
			--ppcWriteBQ.listInUse;
			EnableInterrupts(oldMask);
			break;
			};
		qset(&ppcWriteBQ);
		q_pop(&wPB);
		--ppcWriteBQ.listInUse;
		EnableInterrupts(oldMask);
		
		sRec = findSessionRecord(wPB->ppcWritePB.sessRefNum);
		
		if (sRec IS nil)
			{
			releasePPCpb(&wPB->ppcWritePB);
			continue;
			}

		if (wPB->ppcWritePB.ioResult IS noErr);
		else
			{
			/* closing the port or ending a session causes writes to go to completion */
			
			if (wPB->ppcWritePB.ioResult IS noSessionErr OR wPB->ppcWritePB.ioResult IS sessClosedErr)
				sRec->status |= sessionClosed;
			else
				{
				if (sRec->status & waitingForWrites);
				else
					{
					MemClear(&ppcEndPB,sizeof(PPCEndPBRec));
					ppcEndPB.sessRefNum = wPB->ppcWritePB.sessRefNum;
					ppcEndPB.csCode = ppcEndCmd;
					ppcEndPB.ioCompletion = nil;
					PPCEnd(&ppcEndPB,SYNCHRONOUS);
					sRec->status |= sessionEnded;
					}
				}
			}
		releasePPCpb(&wPB->ppcWritePB);
		
		lastwPB = sRec->swPB;
		if (lastwPB IS wPB)
			{
			sRec->swPB = nil;
			if (sRec->status & waitingForWrites)
				{
				unsigned long portID = sRec->sportID;
				
				sRec->status &= ~waitingForWrites;
				if (breakConnections(portID) IS 0)
					closePPCPort(portID);
				}
			}
		}
	}
/*_____________________________________scanReadBQ____________________________________________*/
void
scanReadBQ(void)
	{
	ppcReadPBPtr	rPB;
	OSErr			err;
	unsigned short	oldMask = 0;
	cTableEntryPtr	sRec;
	MFmsgBlkPtr		aMFmsg;

/* Loop until the queue is empty.  The queue is added to by KnockKnock, the
   io completion for PPCToolbox calls.  The loop is terminated when the queue
   length goes to 0.  Notice the calls to DisableInterrupts around the queue
   manipulation logic.
*/
	for (;;)
		{
		DisableInterrupts(&oldMask); 
		++ppcReadBQ.listInUse;
		if (ppcReadBQ.listlength IS 0)
			{
			--ppcReadBQ.listInUse;
			EnableInterrupts(oldMask); 
			break; /* the queue is empty.  exit the for loop.  */
			}
		qset(&ppcReadBQ);
		q_pop(&rPB);
		--ppcReadBQ.listInUse;
		EnableInterrupts(oldMask);
		
		sRec = findSessionRecord(rPB->ppcReadPB.sessRefNum);
		
		if (sRec IS nil)
			{
			releasePPCpb(&rPB->ppcReadPB);  /* could not find a session record match */
			continue;						/* for this completed read.	This should  */
			}								/* mean that DestroyMsgQ ended the       */
											/* session (i.e. the target has called   */
											/* _ExitToShell							 */

		if (rPB->ppcReadPB.ioResult IS noErr);
		else
			{
			endSessionAfterAReadError(sRec, rPB->ppcReadPB.ioResult);
			continue;
			}
			
		/* set up to get the data portion of this High Level Event, if any. */
		
		rPB->mfMsgBlk.targetmfid.sessionID = rPB->ppcReadPB.sessRefNum;
		
		/* The next assignment determines the local target of the message.  Note:  you
		   cannot assume that the target is still alive (i.e. rPB->pbID->pTablePtr is 
		   valid).  If HLEQDoesNotExist is set, then the app called _ExitToShell and
		   we need to set localPSN accordingly.
		*/
		
		if (sRec->status & HLEQDoesNotExist)
			{
			SetPSN(kNoProcess, &rPB->mfMsgBlk.targetmfid.localPSN);
			}
		else
			rPB->mfMsgBlk.targetmfid.localPSN = rPB->pbID->pTablePtr->p_serialNumber;
			
		rPB->mfMsgBlk.addrOfMsg = nil;

		aMFmsg = fetchMsgData(rPB->pbID, &rPB->mfMsgBlk, &err);
		if (err IS noErr);
		else	/* There was an error during fetchMsgData */
			{
			if (aMFmsg IS nil)	/* Was it a memory error? */
				{
				rPB->mfMsgBlk.msgStatus |= msgMemoryRestart;
				qset(&ppcReadBQ);
				DisableInterrupts(&oldMask); 
				++ppcReadBQ.listInUse;
				if (ppcReadBQ.listlength IS 0)
					q_push(rPB);
				else
					{
					lltail();
					lladd(rPB);
					}
				--ppcReadBQ.listInUse;
				EnableInterrupts(oldMask);
				break;
				}
			else	/* it was not a memory error */
				{
				endSessionAfterAReadError(sRec, err);
				if (aMFmsg)
					RelMsgEventBuffer(aMFmsg);
				continue;
				}
			}
		
		if (sRec->status & HLEQDoesNotExist)
			RelMsgEventBuffer(aMFmsg);
		else
			{
			/* Put the HLE on the appropriate list */
			
			if (rPB->mfMsgBlk.eppcMsgBlk.postingOptions & nAttnMsg)
				qset(&(rPB->pbID->msgQ[1]));
			else
				qset(&(rPB->pbID->msgQ[0]));
			q_push(aMFmsg);
			
			/* Wake him up, if necessary */
			CancelSleep(rPB->pbID->pTablePtr);
			}
		
		/* start the outstanding read on this session */

		sRec->srPB = restartPPCRead(rPB, &err);
		}
	}
/*__________________________________endSessionAfterAReadError________________________________________*/
void
endSessionAfterAReadError(cTableEntryPtr sRec, OSErr err)
	{
	PPCEndPBRec			ppcEndPB;
	
	if (err IS noSessionErr OR err IS sessClosedErr)
		sRec->status |= sessionClosed;
	else
		{
		if (sRec->status & waitingForWrites);
		else
			{
			MemClear(&ppcEndPB,sizeof(PPCEndPBRec));
			ppcEndPB.sessRefNum = sRec->sID;
			ppcEndPB.csCode = ppcEndCmd;
			ppcEndPB.ioCompletion = nil;
			PPCEnd(&ppcEndPB,SYNCHRONOUS);
			sRec->status |= sessionEnded;
			}
		}
	releasePPCpb(sRec->srPB);
	sRec->srPB = nil;
	}
/*____________________________________________________________________________________________*/
/* GetConnectionType.  Returns the type of connection we can have from the sender to the receiver.
 * It takes as input a SendersPSN, a receiverID, postion options.
 * It returns, in addition to type of connection, connectionID, PSN of the target, and err.
 * Results: eppcConnection means the connection is routed thru the PPCToolbox.
 *			noEppcConnection means that the receiver was located, but it cannot understand EPPC.
 *			systemReceiverConnection means the message was addressed to "the system"
 *			systemSenderConnection means the message was from the "the system"
 *			badConnection means that there is no possibility of connection.
 * If the result is eppcConnection, the target is in connectionID.  Otherwise, it is in targetsPSN.
 * If the result is badConnection or noEppcConnection, the appropriate error code is returned
 * in err.  This error should be returned in the noEppcConnection case if the event can not
 * be translated.
 * Note:	• SendersPSN can be kSystemProcess, kNoProcess, or somebodies PSN.
 *			• SendersPSN equal to kNoProcess is an error, I thought; I coded.
 *			• SendersPSN equal to kSystemProcess means someone called us with SystemMode
 *			  turned on.
 *			• receiverID is the target identification.  How receiverID is interpeted is determined
 *			  by postingOptions.  There are four options as of 7.0aA7.
 *			• connectionID is the target identification.  On output how connectionID is interpeted is 
 *			  determined by type of connection.  There are five types as of 7.0aA7.
 *			• targetsPSN is the target process serial number.  On output how targetsPSN is
 *			  interpeted is determined by type of connection.  There are five options as of 7.0aA7.
 *			• err is error.  Means something.
 */
ConnectionType
GetConnectionType(ProcessSerialNumberPtr SendersPSN, unsigned long receiverID, unsigned long postingOptions, unsigned long *connectionID, ProcessSerialNumberPtr targetsPSN, OSErr *err)
	{
	PEntryPtr				pReceiverProc;
	EPPCBlkPtr				eppcBPtr;
	TargetID				tID;
	ProcessSerialNumber		PSNFromSign;
	ConnectionType			retVal;
	cTableEntryPtr			sRec;
	
	/* Assume the worst */
	retVal = badConnection;
	
	/* Sender must be able to do EPPC if a return receipt was requested
	 * NOTE: nReturnReceipt check is first because kSystemProcess may send
	 * an event when pCurrentProcess is nil.
	 */
	if ( ((postingOptions & nReturnReceipt) != 0) && (CanDoEPPC(pCurrentProcess) == false) )
		{
		*err = connectionInvalid;
		*connectionID = noSessionID;
		SetPSN(kNoProcess, targetsPSN);
		return(retVal);
		}
		
	/* Determine specific connection type. */
	if ((postingOptions &= receiverIDMask) IS receiverIDisPSN)
		{
		pReceiverProc = PEntryFromPSN((ProcessSerialNumberPtr) receiverID);
		if (pReceiverProc != nil)
			{
			eppcBPtr = EPPCBlkFromPSN(&pReceiverProc->p_serialNumber);	// <13>
			if (eppcBPtr ISNT nil) { /* yes, target has eppcBlk */
				/* Now consider the sender's identity */
				if (EqualPSNConst(kSystemProcess, SendersPSN) == false)
					{
					tID = getTIDfromEppcBlk(eppcBPtr);
					if ((*connectionID = getSessionID(SendersPSN,&tID, err)) != 0)
						{
						*targetsPSN = *((ProcessSerialNumberPtr) receiverID);
						retVal = eppcConnection;
						}
					else /* we couldn't get a sessionID */
						{
						SetPSN(kNoProcess, targetsPSN);
						retVal = badConnection;
						}
					}
				else
					{ /* SendersPSN was kSystemProcess and target has eppcBlk */
					*err = noErr;
					*connectionID = systemSessionID;
					*targetsPSN  = *((ProcessSerialNumberPtr) receiverID);
					retVal = systemSenderConnection;
					}
				}
			else /* target had no eppcBlk. */
				{
				*err = destPortErr;
				*connectionID = noSessionID;
				*targetsPSN  = *((ProcessSerialNumberPtr) receiverID);
				retVal = noEppcConnection;
				} 
			}
		else /* we didn't find a process table for the target.  See if it is a special case */
			if ( EqualPSNConst(kSystemProcess, (ProcessSerialNumberPtr)(receiverID)) )
				{
				*err = noErr;
				*connectionID = systemSessionID;
				*targetsPSN  = *((ProcessSerialNumberPtr) receiverID);
				retVal = systemReceiverConnection;
				}
			else /* not a special case.  Oh well…  not connection */
				{
				*err = procNotFound;
				*connectionID = noSessionID;
				SetPSN(kNoProcess, targetsPSN);
				retVal = badConnection;
				}
		}
		
	if (postingOptions IS receiverIDisTargetID)
		{
		if ((*connectionID = getSessionID(SendersPSN,(TargetIDPtr)receiverID, err)) != 0)
			{
			retVal = eppcConnection;
			}				
		}
		
	if (postingOptions IS receiverIDisSignature)
		{
		if (PSNFromSignature((ResType)receiverID, &PSNFromSign))
			{
			eppcBPtr = EPPCBlkFromPSN(&PSNFromSign);
			if (eppcBPtr ISNT nil) {
				if (EqualPSNConst(kSystemProcess, SendersPSN) == false)
					{
					tID = getTIDfromEppcBlk(eppcBPtr);
					if ((*connectionID = getSessionID(SendersPSN,&tID, err)) != 0)
						{
						*targetsPSN = PSNFromSign;
						retVal = eppcConnection;
						}
					else
						{
						SetPSN(kNoProcess, targetsPSN);
						retVal = badConnection;
						}
					}
				else /* SendersPSN was kSystemProcess and target has eppcBlk */
					{
					*err = noErr;
					*connectionID = systemSessionID;
					*targetsPSN  = PSNFromSign;
					retVal = systemSenderConnection;
					}
				}
			else /* target had no eppcBlk. */
				{
				*err = destPortErr;
				*connectionID = noSessionID;
				*targetsPSN = PSNFromSign;
				retVal = noEppcConnection;
				}
			}
		else /* couldn't find a PSN from the signature. check special case */
			{
			if ( NULLPROC_SIGNATURE IS (ResType)receiverID )
				{
				*err = noErr;
				*connectionID = systemSessionID;
				SetPSN(kSystemProcess, targetsPSN);
				retVal = systemReceiverConnection;
				}
			else /* not a special case.  Oh well…  not connection */
				{
				*err = connectionInvalid;
				*connectionID = noSessionID;
				SetPSN(kNoProcess, targetsPSN);
				retVal = badConnection;
				}
			}
		} /* end of receiverIDisSignature */
		
	if (postingOptions IS receiverIDisSessionID)
		{
		sRec = findSessionRecord(receiverID);
		if (sRec ISNT nil)
			{
			if (EqualPSNConst(kSystemProcess, SendersPSN) == false)
				{
				if ((*connectionID = validateSessionID(SendersPSN, receiverID, err)) != 0)
					{
					SetPSN(kNoProcess, targetsPSN);
					retVal = eppcConnection;
					}
				else
					{
					SetPSN(kNoProcess, targetsPSN);
					retVal = badConnection;
					}
				}
			else
				{
				*err = noErr;
				*connectionID = systemSessionID;
				*targetsPSN  = sRec->sEppcBlkPtr->pTablePtr->p_serialNumber;
				retVal = systemSenderConnection;
				}
			}
		else /* no connection.  check for special cases */
			{
			if (receiverID IS systemSessionID)
				{
				*err = noErr;
				*connectionID = systemSessionID;
				SetPSN(kSystemProcess, targetsPSN);
				retVal = systemReceiverConnection;
				}
			else /* not a special case.  Oh well…  not connection */
				{
				*err = connectionInvalid;
				*connectionID = noSessionID;
				SetPSN(kNoProcess, targetsPSN);
				retVal = badConnection;
				}
			}
		} /* end of receiverIDisSessionID */
	return (retVal);
	}
/*____________________________________________________________________________________________*/
unsigned long
validateSessionID(ProcessSerialNumberPtr ownersPSN, unsigned long sessionID, OSErr *err)
	{
	cTableEntryPtr sRec;
	
	sRec = findSessionRecord(sessionID);
	if (sRec ISNT nil)
		if ( EqualPSN(ownersPSN, &sRec->sEppcBlkPtr->pTablePtr->p_serialNumber))
			{
			if (sRec->status & (sessionClosed+sessionEnded))
				{
				qset(&connectionQ);
				llhead();
				llsetMatch((ProcPtr)findElement);
				if (llcheck(sRec))
					{
					lldelete();
					DisposHandle(sRec->sHdl);
					}
				*err = sessClosedErr;
				return(0);
				}
			*err = noErr;
			return(sessionID);
			}
	*err = connectionInvalid;
	return(0);		
	}
/*____________________________________________________________________________________________*/
Boolean
comparePortNames(PPCPortPtr N1, PPCPortPtr N2)
	{
	short i;

	if (N1->nameScript ISNT N2->nameScript)
		return(true);
	if (N1->name[0] ISNT N2->name[0])
		return(true);
	if (N1->name[0]) {
		for(i=1;i<=(N1->name[0]);++i)
			if (N1->name[i] ISNT N2->name[i])
				return(true);
		}
	if (N1->portKindSelector ISNT N2->portKindSelector)
		return(true);
	if (N1->portKindSelector IS ppcByString) {
		if (N1->u.portTypeStr[0] ISNT N2->u.portTypeStr[0])
			return(true);
		if (N1->u.portTypeStr[0]) {
			for(i=1;i<=(N1->u.portTypeStr[0]);++i)
				if (N1->u.portTypeStr[i] ISNT N2->u.portTypeStr[i])
					return(true);
			}
		}
	else {
		if (N1->u.port.creator ISNT N2->u.port.creator)
			return(true);
		if (N1->u.port.type ISNT N2->u.port.type)
			return(true);
		}
	return(false);
	}
/*____________________________________________________________________________________________*/
Boolean
comparePortLocation(LocationNamePtr L1, LocationNamePtr L2)
	{
	short i;
	
	if (L1->locationKindSelector ISNT L2->locationKindSelector)
		return(true);
	if (L1->locationKindSelector IS ppcNoLocation)
		return(false);
	if (L1->u.nbpEntity.objStr[0] ISNT L2->u.nbpEntity.objStr[0])
		return(true);
	if (L1->u.nbpEntity.objStr[0]) {
		for(i=1;i<=(L1->u.nbpEntity.objStr[0]);++i)
			if (L1->u.nbpEntity.objStr[i] ISNT L2->u.nbpEntity.objStr[i])
				return(true);
		}
	if (L1->u.nbpEntity.typeStr[0] ISNT L2->u.nbpEntity.typeStr[0])
		return(true);
	if (L1->u.nbpEntity.typeStr[0]) {
		for(i=1;i<=(L1->u.nbpEntity.typeStr[0]);++i)
			if (L1->u.nbpEntity.typeStr[i] ISNT L2->u.nbpEntity.typeStr[i])
				return(true);
		}	
	if (L1->u.nbpEntity.zoneStr[0] ISNT L2->u.nbpEntity.zoneStr[0])
		return(true);
	if (L1->u.nbpEntity.zoneStr[0]) {
		for(i=1;i<=(L1->u.nbpEntity.zoneStr[0]);++i)
			if (L1->u.nbpEntity.zoneStr[i] ISNT L2->u.nbpEntity.zoneStr[i])
				return(true);
		}
	return(false);
	}
/*____________________________________________________________________________________________*/
Boolean
PortNameAndLocationAreEqual(PPCPortPtr portName, LocationNamePtr locName, TargetIDPtr targetID)
	{
	Boolean different;
	
	if ((different = comparePortNames(portName, &targetID->name)) == false)
		if (locName IS nil)
			return (targetID->location.locationKindSelector IS ppcNoLocation);
		else 
			different = comparePortLocation(locName, &targetID->location);
	return(!different);
	}
/*____________________________________________________________________________________________*/
TargetID
getTIDfromEppcBlk(EPPCBlkPtr eppcBPtr)
	{
	TargetID tID;
	
	MemClear(&tID,sizeof(TargetID));
	tID.name = eppcBPtr->nameOfMsgQ;
	tID.location.locationKindSelector = ppcNoLocation;
	return(tID);
	}
/*____________________________________________________________________________________________*/
cTableEntryPtr
findSessionRecord(unsigned long sessionRefNum)
	{
	cTableEntryPtr 	sRec = nil;
	
	if (sessionRefNum IS 0 OR sessionRefNum IS -1)
		return(sRec);
		
	qset(&connectionQ);
	if (ll_length() ISNT 0)
		{
		llsetMatch((ProcPtr)bySessionID);
		llhead();
		if (llcheck((char *)&sessionRefNum))
			llretrieve((char *)&sRec);
		}
	
	return(sRec);
	}
/*____________________________________________________________________________________________*/
#if !CubeE											// <24> Keep the old version around for
cTableEntryPtr										// <24> Non CubeE builds for now.
findFreeSessionRecord(void)
	{
	Handle			aHdl;
	OSErr			err;
	
	aHdl = GMBlk(sizeof(cTableEntry), &err);
	if (err ISNT noErr)
		return(nil);
	HLock(aHdl);
	MemClear(*aHdl,sizeof(cTableEntry));
	
	((cTableEntryPtr)(*aHdl))->sHdl = aHdl;
	return(*aHdl);
	}
#endif

#if CubeE											// <24> The “fixed” version from 7-Up
cTableEntryPtr
findFreeSessionRecord(void)
	{
	Handle			aHdl;
	
	aHdl = NewHandleSysClear(sizeof(cTableEntry));	// <24> Allocate the block in the System heap
	if (!aHdl)
		return(nil);
		
	MoveHLow(aHdl);									// <24> Move it low in the System heap
	HLock(aHdl);									// <24> before locking it.
	
	((cTableEntryPtr)(*aHdl))->sHdl = aHdl;
	return(*aHdl);
	}
#endif

/*____________________________________getSessionID_____________________________________________*/
unsigned long
getSessionID(ProcessSerialNumberPtr sendersPSN, TargetIDPtr portN, OSErr *err)
	{
	PPCStartPBRec		pbS;
	cTableEntryPtr		sRec;
	TargetID			senderTID;
	Boolean				portMatchfound = false;
	Boolean				enableGuestButton;
	Str255 				userName;
	Boolean				guestSelected;
	EPPCBlkPtr			postersEppcBlk;
	Ptr					funcPtr=StartSecureSession;
	unsigned long		myA5;
	
	*err = noErr;
	
	if ((postersEppcBlk = EPPCBlkFromPSN(sendersPSN)) IS nil)
		{
		*err = noPortErr;
		return(0);
		}
		
	if (postersEppcBlk->createErr)
		{
		*err = postersEppcBlk->createErr;
		return(0);
		}

	/* see if connection already exists */
	
	senderTID = getTIDfromEppcBlk(postersEppcBlk);
	
	qset(&connectionQ);
	llhead();
	if (ll_length() ISNT 0)
		llretrieve(&sRec);
	else
		sRec = nil;
	while (sRec)
		{
		if (sRec->sID)
			{
			if (sRec->sEppcBlkPtr IS postersEppcBlk)
				if (PortNameAndLocationAreEqual(&sRec->sPName, &sRec->sLocName, portN))
					{
					portMatchfound = true;
					break;
					}
			else
				if (PortNameAndLocationAreEqual(&sRec->sEppcBlkPtr->nameOfMsgQ, nil, portN))
					if (PortNameAndLocationAreEqual(&sRec->sPName, &sRec->sLocName, &senderTID))
						{
						portMatchfound = true;
						break;
						}
			}
		if (llnext())
			llretrieve(&sRec);
		else
			sRec = nil;
		}

	if (portMatchfound)
		if (sRec->status & (sessionClosed+sessionEnded))
			{
			*err = sessClosedErr;
			lldelete();
			DisposHandle(sRec->sHdl);
			return(0);
			}
		else
			return(sRec->sID);

	/* connection didn't exist. See if we have a free one to give out. */
	
	if ((sRec = findFreeSessionRecord()) IS nil)
		{ 					
		*err = sessTableErr;		/* session table has no free entries */
		return (0);
		}
	
	MemClear(&pbS,sizeof(PPCStartPBRec));
	pbS.csCode = ppcStartCmd;
	pbS.portRefNum = postersEppcBlk->portID;
	pbS.ioCompletion = nil;
	pbS.serviceType = ppcServiceRealTime;
	
	if (portN->location.locationKindSelector IS ppcNBPLocation) 
		pbS.locationName = &portN->location;
	else
		pbS.locationName = nil;
	pbS.portName = &portN->name;
	
	/* The following if statement causes noUserInteractionAllowed to be returned if we are	*/
	/* establishing a remote connection and the application is not frontmost or is being	*/
	/* coerced.																				*/
	/* For 7.0B6 the check for the application being coerced was removed.  A background app */
	/* may want to come to the foreground and post a HLE which requires a session to be     */
	/* established.  The app may still be in the coerced state, yet pFrontProcess IS 		*/
	/* pCurrentProcess and the app is in the frontmost layer of windows.  At this point		*/
	/* it should be OK for authentication to occur.  Hopefully.
	
	/* This does not deal with the app filling in the NBP name of his local machine.		*/

#if 0	
	if ((pbS.locationName ISNT nil) AND ((pFrontProcess ISNT pCurrentProcess) OR (coercionState ISNT CS_DONE)))
#else
	if ((pbS.locationName ISNT nil) AND (pFrontProcess ISNT pCurrentProcess))
#endif
		{
		DisposHandle(sRec->sHdl);
		*err = noUserInteractionAllowed;
		return(0);
		}
		
	enableGuestButton = guestAllowed(portN, err);
	if (*err ISNT noErr)
		{
		DisposHandle(sRec->sHdl);
		return (0);
		}
	
	MemClear(&userName,sizeof(Str255));
	
	/* StartSecureSession may cause the Dialog Manager to run.  It is best if our
	   app's A5 is in the machine because user items in dialog windows behind the 
	   authentication dialog(s) may execute.  Some user item procedure assume that
	   A5 points to the application's global not the Process Manager's globals.
	*/
	myA5 = CurrentA5SimpleSetup();
	*err = CALL_FNC_PTR(StartSecureSession_ptr, funcPtr, (&pbS, &userName, false, enableGuestButton, &guestSelected, nil));
	A5SimpleRestore(myA5);
	
	/*
	*err = StartSecureSession (&pbS, &userName, false, enableGuestButton, &guestSelected, nil);
	*/
	if (*err ISNT noErr)
		{
		DisposHandle(sRec->sHdl);
		return (0);
		}
		
	/* delete user identity.  eppc doesn't use it.  deleting it frees PPCToolbox resources. */
	
	DeleteUserIdentity (pbS.userRefNum);
	
	/* fill in the connection entry */
	
	sRec->status |= startConnection;
	sRec->sID = pbS.sessRefNum;

	sRec->sEppcBlkPtr = postersEppcBlk;
	sRec->sportID = postersEppcBlk->portID;

	sRec->sPName = portN->name;
	if (portN->location.locationKindSelector IS ppcNBPLocation) 
		sRec->sLocName = portN->location;
	else
		sRec->sLocName.locationKindSelector = ppcNoLocation;
		
	sRec->srPB = startPPCRead(postersEppcBlk, sRec->sID, err);
	if (sRec->srPB IS nil)
		{
		DisposHandle(sRec->sHdl);
		return(0);
		}
	else
		{
		qset(&connectionQ);
		q_push(sRec);
		}

	return(sRec->sID);
	}
/*_________________________________restartPPCRead_____________________________________________*/
PPCReadPBPtr
restartPPCRead(ppcReadPBPtr rPB, OSErr *err)
	{

	MemClear(&rPB->mfMsgBlk,sizeof(MFmsgBlk));
	rPB->mfMsgBlk.pbHandle = rPB->pbHdl;
	*err = PPCRead(&rPB->ppcReadPB,ASYNCHRONOUS);

	return(&rPB->ppcReadPB);
	}
/*___________________________________startPPCRead_____________________________________________*/
PPCReadPBPtr
startPPCRead(EPPCBlkPtr eppcBPtr, unsigned long sessionID, OSErr *err)
	{
	PPCReadPBPtr		rPB;

	rPB = getPPCReadPB(eppcBPtr, sessionID, err);
	if (*err IS noErr)
		*err = PPCRead(rPB,ASYNCHRONOUS);

	return(rPB);
	}
/*____________________________________________________________________________________________*/
Boolean
guestAllowed(TargetIDPtr portN, OSErr *err)
	{
	PortInfoRec 		infoBuff;
	IPCListPortsPBRec	listPortPB;
	
	*err = noErr;
	if (portN->location.locationKindSelector IS ppcNoLocation)
		return(false);
	MemClear(&listPortPB,sizeof(IPCListPortsPBRec));
	listPortPB.csCode = IPCListPortsCmd;
	listPortPB.requestCount = 1;
	listPortPB.portName = &portN->name;
	listPortPB.locationName = &portN->location;
	listPortPB.bufferPtr = &infoBuff;
	*err = IPCListPorts(&listPortPB,SYNCHRONOUS);
	if (*err ISNT noErr)
		return(false);
	if (listPortPB.actualCount IS 1)
		return (!infoBuff.authRequired);
	else
		{
		*err = destPortErr;
		return(false);
		}
	}
/*____________________________________________________________________________________________*/
Boolean
findElement(char *A, char *B)
	{
	return((*(LINKTYPE *)A).item IS (*(LINKTYPE *)B).item);
	}
/*____________________________________________________________________________________________*/
Boolean
byPSN(EPPCBlkPtr A, ProcessSerialNumberPtr B)
	{
	return(EqualPSN(&(A->pTablePtr->p_serialNumber),B));
	}
/*____________________________________________________________________________________________*/
Boolean
byPortName(EPPCBlkPtr A, PPCPortPtr B)
	{
	return(!comparePortNames(&A->nameOfMsgQ,B));
	}
/*____________________________________________________________________________________________*/
Boolean
bySignature(EPPCBlkPtr A, ResType *B)
	{
	return(A->pTablePtr->p_signature IS *B);
	}
/*____________________________________________________________________________________________*/
Boolean
byPBID(ppcWritePBPtr A, EPPCBlk *B)
	{
	return(A->pbID IS B);
	}
/*____________________________________________________________________________________________*/
Boolean
bySessionID(cTableEntryPtr A, unsigned long *B)
	{
	return(A->sID IS *B);
	}
/*____________________________________________________________________________________________*/
Boolean
bysEppcBlkPtr(cTableEntryPtr A, EPPCBlk *B)
	{
	return(A->sEppcBlkPtr IS B);
	}
/*____________________________________________________________________________________________*/
Boolean
byportIDforPB(ppcWriteBlk *A, unsigned long *B)
	{
	return(A->portID IS *B);
	}
/*____________________________________________________________________________________________*/
Boolean
byportID(cTableEntryPtr A, unsigned long *B)
	{
	return(A->sportID IS *B);
	}
/*____________________________________________________________________________________________*/
Boolean
PSNFromName(PPCPortPtr portName, ProcessSerialNumberPtr pPSN)
	{
	EPPCBlkPtr	eppcB = nil;
	Boolean		retVal;
	
	qset(&eppcBQ);
	if (ll_length() ISNT 0)
		{
		llsetMatch((ProcPtr)byPortName);
		llhead();
		if (llcheck(portName))
			llretrieve((char *)&eppcB);
		}
	
	/* Set output parameter and function value */
	if (retVal = (eppcB != nil))
		*pPSN = eppcB->pTablePtr->p_serialNumber;
	else
		SetPSN(kNoProcess, pPSN);
		
	return(retVal);
	}
/*____________________________________________________________________________________________*/
Boolean
NameFromPSN(PPCPortPtr portName, ProcessSerialNumberPtr pPSN)
	{
	EPPCBlkPtr	eppcB = nil;
	Boolean		retVal;
	
	qset(&eppcBQ);
	if (ll_length() ISNT 0)
		{
		llsetMatch((ProcPtr)byPSN);
		llhead();
		if (llcheck(pPSN))
			llretrieve((char *)&eppcB);
		}
	
	/* Set output parameter and function value */
	if (retVal = (eppcB != nil))
		*portName = eppcB->nameOfMsgQ;
		
	return(retVal);
	}
/*____________________________________________________________________________________________*/
Boolean
PSNFromSignature(ResType qid, ProcessSerialNumberPtr pPSN)
	{
	EPPCBlkPtr	eppcB = nil;
	Boolean		retVal;

	qset(&eppcBQ);
	if (ll_length() ISNT 0)
		{
		llsetMatch((ProcPtr)bySignature);
		llhead();
		if (llcheck((char *)&qid))
			llretrieve((char *)&eppcB);
		}
	
	/* Set output parameter and function value */
	if (retVal = (eppcB != nil))
		*pPSN = eppcB->pTablePtr->p_serialNumber;
	else
		SetPSN(kNoProcess, pPSN);
		
	return(retVal);
	}
/*____________________________________________________________________________________________*/
EPPCBlkPtr
EPPCBlkFromPSN(ProcessSerialNumberPtr pPSN)
	{
	qset(&eppcBQ);
	if (ll_length() IS 0)
		return(nil);

	llsetMatch((ProcPtr)byPSN);
	llhead();
	if (llcheck((char *)pPSN))
		return(eppcBQ.clp);
	else
		return(nil);
	}
/*____________________________________________________________________________________________*/
void
releasePPCpb(PPCParamBlockPtr pb)
	{
	if (pb IS nil)
		return;
	(Ptr)pb -= sizeOfPPCpbHeader;
	DisposHandle(((ppcReadPBPtr)pb)->pbHdl);
	CheckForFatalError(MEMERROR IS noErr);
	}
/*____________________________________________________________________________________________*/
PPCInformPBPtr
getPPCInformPB(EPPCBlkPtr refCon, unsigned short portID, OSErr *err)
	{
	Handle			aHdl;
	ppcInformPBPtr	ppcInformBlkPtr;

	/* carve out a parameter block for a async PPC call */
	aHdl = GMBlk(sizeof(ppcInformBlk), err);
	if (*err  ISNT noErr)
		return(nil);
	HLock(aHdl);
	ppcInformBlkPtr = *aHdl;
	MemClear(ppcInformBlkPtr,sizeof(ppcInformBlk));
	ppcInformBlkPtr->pbID = refCon;
	ppcInformBlkPtr->portID = portID;
	ppcInformBlkPtr->pbHdl = aHdl;
	ppcInformBlkPtr->ppcInformPB.portName = &ppcInformBlkPtr->portName;
	ppcInformBlkPtr->ppcInformPB.locationName = &ppcInformBlkPtr->portLoc;
	
	ppcInformBlkPtr->ppcInformPB.csCode = ppcInformCmd;
	ppcInformBlkPtr->ppcInformPB.ioCompletion = KnockKnock;
	ppcInformBlkPtr->ppcInformPB.portRefNum = portID;
	ppcInformBlkPtr->ppcInformPB.autoAccept = false;
	
	return(&ppcInformBlkPtr->ppcInformPB);
	}
/*____________________________________________________________________________________________*/
PPCReadPBPtr
getPPCReadPB(EPPCBlkPtr refCon, unsigned long sessionID, OSErr *err)
	{
	Handle aHdl;
	ppcReadPBPtr ppcReadBlkPtr;

	/* carve out a parameter block for a async PPC call */
	
	aHdl = GMBlk(sizeof(ppcReadBlk), err);
	if (*err ISNT noErr)
		return(nil);
	HLock(aHdl);
	ppcReadBlkPtr = *aHdl;
	MemClear(ppcReadBlkPtr,sizeof(ppcReadBlk));
	
	ppcReadBlkPtr->pbID = refCon;
	ppcReadBlkPtr->portID = refCon->portID;
	ppcReadBlkPtr->pbHdl = aHdl;
	ppcReadBlkPtr->ppcReadPB.bufferPtr = &ppcReadBlkPtr->mfMsgBlk.eppcMsgBlk;
	ppcReadBlkPtr->ppcReadPB.bufferLength = sizeof(HighLevelEventMsg);
	
	ppcReadBlkPtr->ppcReadPB.csCode = ppcReadCmd;
	ppcReadBlkPtr->ppcReadPB.ioCompletion = KnockKnock;
	ppcReadBlkPtr->ppcReadPB.sessRefNum = sessionID;
	
	return(&ppcReadBlkPtr->ppcReadPB);
	}
/*____________________________________________________________________________________________*/
extern LINKLIST *list;
/*____________________________________________________________________________________________*/
void
qPPCPb(ppcWritePBPtr wPBPtr, LINKLIST *BQList)
	{
	LINKLIST *oldList = list;

	assert(!BQList->listInUse);
	qset(BQList);
	q_push(wPBPtr);
	list = oldList;
	}
/*____________________________________________________________________________________________*/
void
KnockKnock( PPCParamBlockPtr ppcPb )
	{
	ppcWritePBPtr			wPBPtr;
	ppcInformPBPtr			iPBPtr;

	wPBPtr = (ppcWritePBPtr)((Ptr)ppcPb - (Ptr)sizeOfPPCpbHeader);

	switch(ppcPb->startParam.csCode) {
		case ppcInformCmd:
			
			/* closing the port causes all async calls to go to completion */
			if (ppcPb->informParam.ioResult ISNT noErr)
				{
				qPPCPb(wPBPtr, &ppcInformBQ);
				break;
				}
			
			iPBPtr = (ppcInformPBPtr)wPBPtr;
			iPBPtr->ppcInformReplyPB.ioCompletion = KnockKnock;
			iPBPtr->ppcInformReplyPB.sessRefNum = ppcPb->informParam.sessRefNum;	// <13>

			/* if close is in progress on the port reject the incoming session */

			if (wPBPtr->status & closeInProgress)
				PPCReject(&iPBPtr->ppcInformReplyPB,ASYNCHRONOUS);
			else
				PPCAccept(&iPBPtr->ppcInformReplyPB,ASYNCHRONOUS);
			break;
		case ppcWriteCmd:
			qPPCPb(wPBPtr, &ppcWriteBQ);
			break;
		case ppcReadCmd:
			wPBPtr->mfMsgBlk.eppcMsgBlk.theMsgEvent.when = TICKS;
			qPPCPb(wPBPtr, &ppcReadBQ);
			break;
		case ppcAcceptCmd:
		case ppcRejectCmd:
			iPBPtr = (ppcInformPBPtr)((Ptr)wPBPtr - (Ptr)sizeof(PPCInformPBRec));
			qPPCPb(iPBPtr, &ppcInformBQ);
			break;
		case ppcOpenCmd:
		case ppcStartCmd:
		case ppcEndCmd:
		case ppcCloseCmd:
		case IPCListPortsCmd:
		default:
			dbmsg("Eppc completion routine called for unknown command.");
			break;
		}
	}
