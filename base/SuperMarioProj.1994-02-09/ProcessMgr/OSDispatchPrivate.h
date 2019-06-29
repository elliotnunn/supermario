/*
	File:		OSDispatchPrivate.h

	Contains:	Private interface to the Process Mgr OSDispatch routines.

	Written by:	Phil Goldman and Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	
		<11>	 8/26/92	DTY		Roll in latest changes for A/UX.
		<10>	  4/9/92	KST		#1025131,<JH>: Changing SWM from a background app to a driver.
									Adding a new call: SendEventToSWMDriver.
		 <9>	10/15/91	YK		Added GetFrontAppFloatLayer macro.
		 <8>	 10/9/91	YK		Added FindAppFloatLayer. Searches IMLayer for the application
									layer.
		 <7>	 1/28/91	DFH		JSM,#81425:Remove EnableSwitching and DisableSwitching calls.
									They are now in MFPrivate.h.
		 <6>	 1/14/91	DFH		(JDR) Conditionalize out AUX support.
		 <4>	12/10/90	DFH		Moved file attribute and Finder flag equates to SysMisc.h.
		 <3>	 12/5/90	DFH		Added #defines for prettier access to AuxDispatch.
		 <2>	11/27/90	DFH		Removed unneeded parameter from cpu_resched.
		 <0>	 x/xx/86	PYG		New Today.

*/ 

#ifndef __OSDISPATCHPRIVATE__
#define __OSDISPATCHPRIVATE__

#ifndef __PPCTOOLBOX__
#include <PPCToolBox.h>
#endif __PPCTOOLBOX__

#ifndef __DATA__
#include "Data.h"
#endif __DATA__

#ifndef __FILES__
#include "Files.h"
#endif __FILES__

#ifndef __LAYERS__
#include <Layers.h>
#endif __LAYERS__

/************************************************************************************
 *							Process Manager 										*
 ************************************************************************************/

#ifdef HAS_AUX_PROCESSMGR
#define ASYNC_LAUNCH	(0x00080000)
#define COFF_BINARY		(0x00040000)
#endif HAS_AUX_PROCESSMGR

/* record returned by TWGetProcInfo (old MultiFinder call we must support) */
struct procinfo {
	short			p_state;			/* process state */
	ProcessID		p_mypid;			/* the process id */
	long			p_type;				/* type of task (usually APPL) */
	long			p_signature;		/* signature of task */
	long			p_version;			/* version of task */
	THz				p_zone;				/* pointer to minor zone */
	unsigned long	p_taskmode;			/* tasks created mode */
	Boolean			p_needsuspresevts;	/* whether or not the process expects suspend and resume events */
	Boolean			p_back;				/* can accept background time */
	Boolean			p_activateonresume;	/* whether the app will activate and deactivate on susppend/resume */
	ProcessID	 	p_dad;				/* process id of my dad */
	unsigned long	p_size;				/* size of his world */
	unsigned long	p_ssize;			/* size of his stack */
	unsigned long	p_slices;			/* # of times process has seen the CPU */
	
	unsigned long	p_freemem;			/* amount of free memory in heap */
	Str31			p_name;				/* name of backing app */
	short			p_vrefnum;			/* vrefnum of app file as passed to _KernelLaunch */
};

/* Constants for printer type calls */
#define PRTYPE_CHANGESTATUS_SAME		(-1)
#define PRTYPE_CHANGESTATUS_DISABLED	(0)
#define PRTYPE_CHANGESTATUS_ENABLED		(1)

/* Block to specify a desk accessory by name */
typedef struct DASpecBlock {
	FSSpec			*daFileSpec;
	StringPtr		daDrvrName;
} DASpecBlock, *DASpecBlockPtr;

/* Output param block for StandardLaunch */
typedef struct LaunchResults {
	OSErr				LaunchError;
	PEntryPtr			pNewProcess;
	unsigned long		PreferredSize;
	unsigned long		MinimumSize;
	unsigned long		AvailableSize;
} LaunchResults, *LaunchResultsPtr;

/* Param block used by internal routines for launch */
typedef struct InternalLaunchPB {
	FSSpecPtr			ilAppSpecPtr;
	unsigned long		ilTaskMode;
	DASpecBlockPtr		ilDAInfoPtr;
	PEntryPtr			ilResultProc;
	unsigned long		ilPartitionSize;
	unsigned long		ilStackSize;
	Ptr					ilAppParameters;
	Handle				ilAppParamHdl;
} InternalLaunchPB, *InternalLaunchPBPtr;

void		StandardLaunch(FSSpecPtr, unsigned short, unsigned long, Ptr, LaunchResultsPtr, DASpecBlockPtr);
void		LaunchFacelessTasks(void);

/************************************************************************************
 *																					*
 *					Pseudo-event definitions										*			
 *																					*
 ************************************************************************************/
/* Event number for pseudo events */
#define PSEVT_WHAT app4Evt

/* Selector goes in high byte of the message field. */
#define PSEVT_MOUSEMOVED	(250)

/************************************************************************************
 *																					*
 *  				 Function prototypes											* 
 *																					*
 ************************************************************************************/
void				INITRET(void);
void				InitializeProcess(void);
void				cpu_resched(void);
PEntryPtr			fg_resched(EventRecord *, Boolean);
void				CancelSleep(PEntryPtr);
void				PutOnSleepQueue(PEntryPtr, unsigned long);
void				RemoveFromStateList(PEntryPtr, PEntryPtr *);
void				switch_task(PEntryPtr);
void				save_state(PEntryPtr);
unsigned short		restore_state(PEntryPtr, Boolean);

pascal void			HideLayer(LayerPtr layer, Boolean showFlag);
void				ShowHideOthers(Boolean showFlag);
LayerPtr			FindAppBaseLayer(WindowPtr window, LayerPtr layerPtr);
#define				FindAppLayer(window)		(FindAppBaseLayer(window, MFLayer))
#define				FindAppFloatLayer(window)	(FindAppBaseLayer(window, IMLayer))

void 				SendEventToSWMDriver( EventRecord  *theEvent );

void				CancelPendingUpdates(LayerPtr layerPtr);
#define				LayerOwner(pLayer)	((PEntryPtr) GetWRefCon(pLayer))
#define				GetFrontAppLayer()		((LayerPtr) GetSubWindows(MFLayer))
#define				GetFrontAppFloatLayer()	((LayerPtr) GetSubWindows(IMLayer))

#ifdef HAS_AUX_PROCESSMGR
/************************************************************************************
 *																					*
 * AUX calls we define and use internally.											*
 *																					*
 ************************************************************************************/

void				AUX_Block(PEntryPtr p);
pascal long			AUX_Dispatch_Patch(short selector, char *p);
pascal short		AUX_EventAvail(short evtMask, EventRecord *theEvent);
PEntryPtr 			AUX_FindProc(long taskId);
short				AUX_ForkExecGlue(short, char *);
short	 			AUX_GetAnyEventGlue(short evtMask, EventRecord *theEvent, 
									long timeout, RgnHandle bounds,
									Boolean pullit);
long				AUX_GetMinTimeout(void);
RgnHandle			AUX_GetMouseBounds(void);
pascal short		AUX_GetNextEvent(short evtMask, EventRecord *theEvent);
long 				AUX_GetTaskGlue(void);
short				AUX_HandleEvent(EventRecord *theEvent);
void				AUX_IdleProc(void);
short 				AUX_KernelEvents(EventRecord *eventRecord);
void				AUX_KillLayer(long taskId);
short	 			AUX_Launch(InternalLaunchPBPtr pParams);
void 				AUX_MakeLayer(long taskId, unsigned short flags, long size);
void 				AUX_MakeRunnable(long taskId);
long 				AUX_ProcessMgrTask(void);
void 				AUX_Setup(void);
void				AUX_SwitchGlue(long taskId);
PEntryPtr			AUX_Unblock(long taskId);
short				AUX_WaitForEvent(short mask, EventRecord *theEvent);
short				AUX_WaitNextEvent(short evtMask, EventRecord *theEvent, long timeout, RgnHandle mbo);
void				aux_restore_ctx(unsigned long);
#endif HAS_AUX_PROCESSMGR

/************************************************************************************
 *																					*
 * OSDispatch calls we use internally that are not available in the more public		*
 * files (like Processes.h and MFPrivate.h).										*
 *																					*
 ************************************************************************************/

/* scheduler stuff */
pascal short
KernelLaunch(InternalLaunchPBPtr pParams)
    = {0x3F3C,0x0010,0xA88F};
	
pascal void
SleepProcess(unsigned long time)
    = {0x3F3C,0x001C,0xA88F};

/* layer management stuff */
pascal LayerPtr
GetLayer(void)
    = {0x3F3C,0x0004,0xA88F};
	
pascal long
MakeTopLayer(LayerPtr layer)
    = {0x3F3C,0x000C,0xA88F};
	
pascal long
LayerClose(LayerPtr layer)
    = {0x3F3C,0x000D,0xA88F};
	
pascal long
LayerInit(void)
    = {0x3F3C,0x000E,0xA88F};

pascal LayerPtr
LGetNextLayer(LayerPtr pCurLayer, Boolean wantVisiblesOnly)
    = {0x3F3C,0x002B,0xA88F};
	
pascal LayerPtr
LayerFind(Point mousepos)
    = {0x3F3C,0x000F,0xA88F};

pascal short
LGetAuxWin(LayerPtr layer, WindowPeek window, AuxWinHandle awh)
    = {0x3F3C,0x0023,0xA88F};
	
pascal short
LColorInvalRect(Rect *rect, short device, short sysupdate)
    = {0x3F3C,0x0025,0xA88F};
	
#endif __OSDISPATCHPRIVATE__
