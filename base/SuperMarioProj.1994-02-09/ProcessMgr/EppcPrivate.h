/*
	File:		EppcPrivate.h

	Contains:	Data and externs private to EPPC.

	Written by:	Jay Moreland

	Copyright:	© 1989-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <8>	11/25/91	DTY		Take out interface for MoveHLow() since it’s now in
									MemoryMgrPriv.h.
		 <7>	11/21/91	DTY		Add interface for MoveHLow here since it’s currently only used
									by findFreeSessionRecord. We’ll probably want to move it into
									Memory.h someday.
		 <5>	12/14/90	JWM		moving a bit from eppc.h to eppcPrivate.h. The bit is
									registerOnNetwork (0x80000000). It is not part of the API.
		 <4>	 11/6/90	JWM		removed conditional ForB2.  Added a status bit to indicate
									memory restart condition when trying to deliver a message in the
									scanReadBQ path.  This will keep the system from breaking
									connections because of failure to get memory.
		 <2>	10/30/90	csd		Make the symbols match the BBS headers.
		 <0>	 4/26/89	JWM		New Today.

*/ 

#ifndef __EPPCPRIVATE__
#define __EPPCPRIVATE__

#ifndef __MEMORY__
#include	<memory.h>
#endif

#ifndef __TYPES__
#include	<types.h>
#endif

#ifndef __OSUTILS__
#include	<OSUtils.h>
#endif

#ifndef __LLQUEUE__
#include	"LLQueue.h"
#endif

#ifndef __PPCTOOLBOX__
#include	<PPCToolBox.h>
#endif

#ifndef __EPPC__
#include	<EPPC.h>
#endif

#ifndef __PROCESSES__
#include	<Processes.h>
#endif

/* status bit in eppcBlk->optionFlags */

enum {registerOnNetwork = 0x80000000};

/* status bits for a message in the EPPCBlk.mfMsgBlk */

#define localOnly				0x8000
#define getSpecificHLE			0x4000
#define msgCompletelyAccepted	0x2000
#define msgPartialyAccepted		0x1000
#define msgMemoryRestart		0x0001

typedef struct MFID {
	ProcessSerialNumber	localPSN;
	unsigned long		sessionID;
	} MFID;
	
typedef struct MFmsgBlk {
	LINKTYPE			MFmsgBlkQ;
	Handle				pbHandle;
	MFID				sendermfid;
	MFID				targetmfid;
	unsigned long		MFRefcon;
	unsigned long		msgStatus;
	Ptr					addrOfMsg;
	HighLevelEventMsg	eppcMsgBlk;
	} MFmsgBlk, *MFmsgBlkPtr, **MFmsgBlkHdl;
	
typedef struct EPPCBlk {
	LINKTYPE 			eppcBlkQ;
	OSErr				createErr;
	PEntryPtr			pTablePtr;
	PPCPortRec			nameOfMsgQ;
	unsigned long		reserved1;
	unsigned long		optionFlags;
	unsigned long		portID;
	MFmsgBlkPtr			mfMsgBlk;
	PPCParamBlockPtr	iPBs[3];
	unsigned long		status;
	LINKLIST			msgQ[2];
	} EPPCBlk, *EPPCBlkPtr, **EPPCBlkHdl;

void		ePPCInit(void);
void		CreateMsgQ(PEntryPtr pProc);
void		DestroyMsgQ(EPPCBlk *eppcBlk);
void		RelMsgEventBuffer(MFmsgBlkPtr mfMsgBlk);
void		postMsg(const MFmsgBlkPtr msgBlk);
void		postReturnReceipt(const MFmsgBlkPtr mfMsgBlk, signed short rtnModifiers);
void		postTranslationResult(MFmsgBlkPtr, OSErr);
void		flushMsg(MFmsgBlkPtr aMsg, unsigned short rtnModifiers);

#endif __EPPCPRIVATE__
