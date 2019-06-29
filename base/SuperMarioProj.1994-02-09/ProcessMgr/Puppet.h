/*
	File:		Puppet.h

	Contains:	Puppet string interface

	Written by:	Phillip Y. Goldman, Software Atomic Knee Drop

	Copyright:	© 1986-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <0>	11/15/86	PYG		New Today.

*/ 

#ifndef __PUPPET__
#define __PUPPET__

/************************************************************************************
 *								Basic definitions.
 ************************************************************************************/

#ifndef FOR_REZ

/* Prototype instruction list */
typedef struct eventList
	{
	long		offset;
	short		count;
	EventRecord	events[0];
	} EventList, *EventListPtr, **EventListHdl;

/* Size of event list header, and the offset to use when initializing instruction
 * list to specify first instruction.
 * NOTE: Relies on the fact that we defined the EventRecord array with count == 0.
 */
#define	SIZEOF_EVENTLIST_HDR (sizeof(EventList))

#endif FOR_REZ

/************************************************************************************
 *								 Puppet event information.
 ************************************************************************************/

/* SYS_PUPPET_STRING_RSRC_ID is the ID of the puppet string resources we've added to
 * the system file.  There is a generic default one, and there is a custom default
 * one for each of three (old) apps that required it for compatibility.  The value of
 * this constant is determined by the MakeFile, since it needs to add the resource.
 * APP_PUPPET_STRING_RSRC_ID is the ID of a puppet string resource that developers can
 * add to the app file.
 * Type of the resource is the signature of the application.  The generic default
 * uses (DefaultPupStrType) we made up in the MakeFile.
 */
/* #define SYS_PUPPET_STRING_RSRC_ID	{SysPupStrID}  from MakeFile */
#define APP_PUPPET_STRING_RSRC_ID		(127)

/* Puppet String types */
#define pstNormalEvent				0
#define pstNullInstr				1
#define pstUnknownInstr				2
#define pstMenuItem					3
#define pstEventList				4
#define pstEventCode				5
#define pstImplementCode			6
#define pstIllegalInstr				7
#define pstMenuString				8

/* Instruction Types */
#define instrReset					2
#define instrCut					3
#define instrCopy					4
#define instrPaste					5
#define instrSelRect				6
#define instrSelAll					7
#define instrMemCopy				8
#define instrOutlineRect			9
#define instrPrivate1				10		/* For intra-puppet string use only */
#define instrQuit					11
#define	instrOpen					12
#define	instrCancel					13

/* Instruction/Event Flags */
#define instrMouseLocal				0x8000
#define evtMouseLocal				0x8000
#define evtMouseGlobal				0x0000
#define evtMouseEither				0x4000
#define evtIsImportant				0x0000
#define evtIsUnimportant			0x2000

/* String IDs for type pstMenuString */
#define menuStringListResType		'mst#'
#define menuStringResType			'mstr'

#define strIDQuitMenu				100
#define strIDQuitItem				101
#define strIDOpenMenu				102
#define strIDOpenItem				103

#ifndef FOR_REZ

/* Prototypes for routines imbedded in puppet resources */
typedef pascal short
(*Puppetstringimplcode_ptr)(EventRecord *evtPtr);

typedef pascal EventListHdl
(*Puppetstringevtcode_ptr)(EventRecord *evtPtr);

#endif FOR_REZ

/************************************************************************************
 *								 Trap interface.
 ************************************************************************************/

#ifndef FOR_REZ

pascal void
RequestScratchSwitchTask(ProcessID taskDescriptor, Handle eventListIn, Handle eventListOut, short timeout)
    = {0x3F3C,0x0000,0xA88F};

/* This key (comma) with cmd key causes return to master */
#define SCRATCHPADRETURNKEYCODE			(0x2B)

/* Bit of resume event message that signals a scratchpad return */
#define bitResumeScratch				2

/* RequestScratchSwitchTask() return values (stored in bits 24-31 of resume event message) */
#define ResumeFromScratchpad			0
/* #define resumeFlag					1		…in {CIncludes}Events.h */
#define ResumeTimeout					2
#define ResumeEventListInErr			3
#define ResumeEventListOutErr			4
#define ResumeFromCancel				5

/* Information record returned by GetActiveTaskInfo */
typedef struct TaskInfoRec {
	ProcessID	taskDescriptor;
	long		signature;
	long		version;
	} TaskInfoRec, *TaskInfoPtr, **TaskInfoHdl;
	
pascal short
GetActiveTaskInfo(TaskInfoRec taskInfo[], short maxSlot)
    = {0x3F3C,0x0001,0xA88F};

/* Information record returned by GetSwitchInfo */
typedef struct SwitchInfoRec {
	long		signature;
	long		version;
	} SwitchInfoRec, *SwitchInfoPtr, **SwitchInfoHdl;
	
pascal void
GetSwitchInfo(SwitchInfoPtr switchInfoPtr)
    = {0x3F3C,0x0002,0xA88F};

pascal void
CancelSwitch(void)
    = {0x3F3C,0x0003,0xA88F};

#endif FOR_REZ

/************************************************************************************
 *								 Private information.
 ************************************************************************************/

#ifndef FOR_REZ

/* List of menu item names for puppet purposes */
typedef struct StrList
	{
	unsigned short		cStrings;				/* number of strings in pStrings */
	Str255				pStrings[];				/* all strings OK for this menu item */
	} StrList, *StrListPtr, **StrListHdl;

extern StrListPtr		pStrListCopyNamePrefix;

/* Instruction list queue data and manipulation routines */
typedef	struct PuppetInstrElem	*PuppetInstrElemPtr, **PuppetInstrElemHdl;
typedef struct PuppetInstrElem {
	PuppetInstrElemHdl	nextPuppetInstrElem;	/* handle to next instruction list */
	EventListHdl		instrsHdl;				/* handle to instruction list proper */
	unsigned short		instrModBits;			/* evtMouseGlobal or evtMouseLocal */
} PuppetInstrElem;

/* Puppet string control information */
typedef struct		PuppetVars
	{
	PEntryPtr			puppetMasterProc;		/* puppet: PEntry of master process */
	unsigned short		puppetTimeout;			/* puppet: count of GNEs before timeout */
	unsigned short		puppetModBits;			/* puppet: default event mods */
	MFmsgBlkPtr			puppetEppcMsgBlk;		/* puppet: EPPC being translated, or 0 */
	PuppetInstrElemHdl	puppetInstrQueue;		/* puppet: queue of event lists */
	PuppetInstrElemHdl	puppetOutInstrsHdl;		/* puppet: event list for canceled switch */
	PEntryPtr			masterPuppetProc;		/* master: PEntry of puppet process */
	char				masterResumeMsg;		/* master: value for resume_message */
	} PuppetVars;

/* Control tuning */
#define TIMEOUT_MASKYIELD			15			/* wait limit for a legal event mask */
#define TIMEOUT_BUTTONDOWN			10			/* wait limit for a mouse up event */

/* Shared data */
extern Point	buttonUpPt;						/* point where mouse button comes up */
extern long		forcedPupStrMenuSelection;		/* menu selection we are stuffing */
extern short	timeoutMaskYield;				/* wait limit for a legal event mask */
extern short	timeoutButtonDown;				/* wait limit for a mouse up event */

/* Some simple operations as macros, rather than subroutines */
#define SetOutInstrsHdl(pProc, eventListHdl) 	(pProc->p_puppetvars.puppetOutInstrsHdl = (eventListHdl))
#define InstrsQueueIsEmpty()					(pCurrentProcess->p_puppetvars.puppetInstrQueue == nil)
#define ClearInstrsQueue()						{while (DumpFirstInstr() != nil) ;}
#define IsOutEventListActive()					(pCurrentProcess->p_puppetvars.puppetOutInstrsHdl == (Handle) -1)

/* Function prototypes */
void					PuppetStringsInit(void);
OSErr					CoreRequestScratchSwitchTask(PEntryPtr, EventListHdl, EventListHdl, short, MFmsgBlkPtr);
EventListHdl			GetActiveInstrsHdl(void);
OSErr					PushInstrsHdl(PEntryPtr, EventListHdl, short);
OSErr					AddInstrsHdl(PEntryPtr, EventListHdl);
PuppetInstrElemHdl		DumpFirstInstr(void);
void					CancelSwitchWithError(void);

#endif FOR_REZ

#endif __PUPPET__
