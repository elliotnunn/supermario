/*
	File:		Data.c

	Contains:	Definitions of Process Mgr's global data

	Written by:	Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<25>	10/20/92	DRF		Moved MODSQUAD globals to end of globals in an attempt to
									minimize the a5 world differences when they are compiled-in.
		<24>	 8/26/92	DTY		Change ModSquad’s funky conditional to use #ifdefs like everyone
									else.
		<23>	 8/17/92	hjcr	Adding support for OCE's "defer drag" feature. Conditionally
									compiled under MODSQUAD
		<22>	 3/30/92	DTY		#1025416,<DDG>: Add skiaExists global.
		<21>	 3/25/92	DTY		#1024114,<pvh>: Add c_PEntryFromProcessSerialNumber to the
									OSDispatch table. Check out the comment in OSDispatch.c, version
									12 about why this routine is being added.
		<20>	 1/11/92	YK		Two more calls and two more gloabls for TSM.
									NewTSMLayerOwner to get the event for the floating window.
									RouteEventToSWM to do the bottom line input.
		<19>	11/22/91	DTY		Added a global which tells GMBlk to automatically allocate the
									block in the System heap. This is the inline version of the 7-Up
									patch which did this to force the 'obit' event to be allocated
									in the System heap instead of the Process Manager heap.
		<18>	10/16/91	YK		Added InlineAware dispatch entry.
		<17>	10/15/91	YK		Added global data and dispatch entries for supporting the Text
									Service Manager.
		<16>	 10/9/91	YK		Added IMLayer(LayerPtr :global), NewFloatLayer and
									DisposeFloatLayer (traps) all for supporting input windows for
									text services.  Particularly, FEP's (input methods).
		<15>	 9/16/91	DFH		Added initialProcessSR. Needed for NuKernel.
		<14>	  4/9/91	DFH		xxx, #83672 : Added vrGetProcessMgrHiMaxBlock vector.
		<13>	 2/21/91	DFH		dba,#82504, #82681, #83168, #83182, #83207: Added
									vrGetSystemHiFreeBytes and SysZoneFloatSizes.
		<12>	 1/28/91	DFH		JSM,#81425: Removed inhibit_DAs global and PopUpMenuSelect
									patch.
		<11>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
		<10>	 1/14/91	DFH		(dba) Make system mode per process.
		 <9>	 1/14/91	DFH		(JDR) Conditionalize out AUX support.
		 <7>	  1/8/91	DFH		(rdd) Got rid of pErrantProcess since it is no longer needed.
		 <6>	12/20/90	DFH		(dba) Remove obsolete dispatch TWKill. Replace it with
									GetNextHighLevelEvent.
		 <5>	12/18/90	DFH		Added GetScrap patch.
		 <4>	12/14/90	DFH		Added kernelMode.
		 <3>	 12/5/90	DFH		Added AUXIsPresent.
		 <2>	 11/5/90	DFH		Correct names for TEMPTOPMEM and TEMPDISPOSEHANDLE.
		 <0>	 x/xx/86	ELR		New Today.

*/ 
 
#include <types.h>
#include <memory.h>
#include <osutils.h>
#include <quickdraw.h>
#include <events.h>
#include <resources.h>
#include <retrace.h>

#include "Data.h"
#include "Lomem.h"
#include "Puppet.h"
#include "Zone.h"

/************************************************************************************
 *						 Here is Process Mgr's global data							*
 ************************************************************************************/
 
/* struct qd	QuickDrawGlobals; 			already defined by compiler */
PEntry		 	NullProcess;				/* a process entry for our own use */

/* System operation */
#ifdef HAS_AUX_PROCESSMGR
Boolean			AUXIsPresent;				/* we're running under A/UX */
#endif HAS_AUX_PROCESSMGR

/* Process descriptors */
unsigned long	ProcessNumberSource;		/* next available unique process identifier */
ProcessID		ProcessIDSource;			/* next ProcessID to try (can't give negative)*/
short			nbacks;						/* number of background processes */
short			remnulls;					/* number of nulls before switch */
short			initialProcessSR;			/* process' initial CPU status register */
PEntryPtr		pCurrentProcess;			/* current process */
PEntryPtr		pFrontProcess;				/* current frontmost process
											 * NOTE: During scrap coercion, pFrontProcess
											 * might not be the owner of the frontmost layer.
											 */
PEntryPtr		pNewFrontProcess;			/* process we are foreground switching into */
PEntryPtr		pFrontProcessQueue;			/* processes we are want to foreground switch into */
PEntryPtr		pLastBGProcess;				/* process most recently scheduled in BG */
PEntryPtr		pShellProcess;				/* process (re)launched from FINDERNAME */
PEntryPtr		pDebugProcess;				/* registered debugger */

/* Process lists threaded through pProcessList, to isolate certain states for more
 * efficient handling.
 */
PEntryPtr		pSleepingStateList;			/* processes waiting for an event */
PEntryPtr		pDyingStateList;			/* processes waiting to die */

/* Whether type of process launched from FINDERNAME is FINDERS_TYPE.  Must be known
 * even if that process has since quit.
 */
Boolean			ShellWasFinderType;

/* Layer management */
LayerPtr		MFLayer;					/* Grandparent layer */
LayerPtr		IMLayer;					/* Grandparent Float layer */
LayerPtr		desklayer;					/* layer which owns desktop */
PEntryPtr		desklayerowner;				/* ownér d'desktop */

/* Process scheduling */
unsigned long 	lastswitch;					/* ticks of last switch */
unsigned long	nextswitch;					/* ticks of next switch */

/* Context switching control */
Ptr				switchTabPtr;				/* ptr to toolbox lomem to switch tab */
u_size			lmemToolDataSize;			/* size of lomem image */

/* Scrap coercion control */
short			cutCopyCount;				/* Number of times a cut or copy has occurred */
short			coercionState;				/* state of coercion machine */
short			dont_switch;				/* lockout on CPU rescheduling */
Boolean			napOver;					/* Convenient flag to save processing time */


MyScrapInfo		srcMyScrapInfo;
short			nullTimeout;				/* number of times thru GNE until we timeout of
											 * states CS_GENERATE_NULLS_THEN_CMD_[CV]
											 */

/* Apple menu control */									 
Handle			standardMenuDefProcRsrc;	/* standard MDefProc */
StringHandle	hHideCurrent;				/* Basis of application menu item to hide front app */
StrListPtr		pStrListCopyNamePrefix;		/* Prefix of extra items that might be the copy item */

AppleItemHdl	FirstAppleItem;				/* Apple menu items set by applications */
Handle			hGenericAppIconSuite;		/* Generic application icon family */
Handle			hGenericDAIconSuite;		/* Generic DA icon family */

/* Application Menu */
MenuHandle		ApplicationMenuHdl;			/* Handle to Application Menu structure */

/* Memory management */
THz				ProcessMgrZone;				/* the major zone */
Boolean			In32BitMode;				/* Whether machine started in 32-bit mode */

/* Routine vectors for dirty heap operations (see zone.h) */
zvr1Ptr			vrGetProcessMgrLoFreeBytes;
zvr2Ptr			vrExtendZone;
zvr3Ptr			vrMoveZone;
zvr4Ptr			vrShrinkProcessMgrZone;
zvr5Ptr			vrGetLockPtr;
zvr6Ptr			vrGetHighestLargeFreeBlock;
zvr7Ptr			vrGetTopBlockSize;
zvr8Ptr			vrFreeTop;
zvr9Ptr			vrChipOffRelocFromFree;
zvr10Ptr		vrZoneMPBlockPtr;
zvr11Ptr		vrGetSystemHiFreeBytes;
zvr12Ptr		vrGetProcessMgrHiMaxBlock;

HListHdl		SystemTempMemHList;			/* temp memory allocated by system */

/* Bullshit saved at startup for easy retrieval */
short			kernelMode;					/* whether we started in supervisor mode */
char			*kernelstack;
unsigned long	kernelstackbase;
short			kernelbusy;					/* kernel stack busy */

unsigned long	initLocationZero;			/* LOCATIONZERO when Process Mgr launched */
Ptr				initMemTop;					/* MemTop when Process Mgr launched */
Ptr				initFSQueueHook;			/* FSQueueHook when Process Mgr launched */
Ptr				initCurStackBase;			/* CurStackBase when Process Mgr launched */

/* Error handling */
Boolean			IsOldDebugger;				/* Is old MacsBug in? */

Ptr				oldExceptionVectors[ID_TRAPF-ID_BUSERR+1];	/* space for old exception vectors */

ProcPtr			debugEntryRoutine;			/* Exception entry routine provided by debugger */
ProcPtr			debugExitRoutine;			/* Exception exit routine provided by debugger */
ProcPtr			debugToAppRoutine;			/* Last routine before going back to app provided by debugger */
char			debugControlKeyCode;		/* Key code of cmd-sequence to get back to the debugger */
short			debugKeyTryCount;			/* Number of times user has hit the debug key */

/* EPPC and AppleEvents */
OSErr			ppcInitErr;					/* Error returned by PPCInit */
MFmsgBlkPtr		pSystemQuitAllMsg;			/* EPPC message block that started system quit */

Boolean			allocateGMBlkInSysHeap;		/* <19> Allocate the block in the System heap */
Boolean			Colorized;					/* Whether machine has color capability */
Boolean			MachineHasFPU;				/* Whether machine does floating point */
Boolean			MachineHasMacPlusKbd;		/* Whether machine has older keyboard */

unsigned short	fakeKeyCount;				/* Number of events kept in fakeKeyHandle */
FakeKeyHandle	fakeKeyHandle;				/* Fake-KeyDown events generated by the Text Service Manager */
unsigned short	dontGetFakeKey;				/* Flag which means "don't pass it to TSM" */

PEntryPtr		tsmLayerOwner;				/* app for the bottom line input */
Boolean			routeEvent;					/* send event to the tsmLayerOwner */

Boolean			skiaExists;					// <22>

#ifdef MODSQUAD
short			gDragState;
Boolean			gDragMgrIsAvailable;
EventRecord		gDragEvent;
PEntryPtr		gFrontDragProcess;			/*	Process in the front when we initiated the dragging */
#endif

/************************************************************************************
 *								Trap patching table
 ***********************************************************************************/

void	a_handtohand();
void	C_ADDRESMENU(), a_checkload(), a_close(), a_disposhandle(), a_drawmenubar();
void	a_eject(), C_EVENTAVAIL(), a_exittoshell(), a_getnamedresource(), C_GETNEXTEVENT();
void	a_getresattrs(), a_getresource(), a_getscrap(), a_getvol(), a_getvolinfo();
void	a_handlezone(), a_hfsdispatch(), C_INITWINDOWS(), a_launch(), C_MENUKEY(); 
void	C_MENUSELECT(), a_mountvol(), a_newhandle(), a_offline(), a_open();
void	C_OPENDESKACC(), a_openrf(), a_osdispatch(), a_osreserved(), a_pack3();
void	a_postevent(), a_putscrap(), C_RDRVRINSTALL(), a_reallochandle();
void	a_recoverhandle(), a_releaseresource(), C_SETCURSOR(), a_setgrowzone();
void	a_sizersrc(), C_STILLDOWN(), C_SYSEDIT(), a_syserror(), C_SYSTEMCLICK();
void	C_SYSTEMEVENT(), C_SYSTEMMENU(), C_SYSTEMTASK(), a_unmountvol();
void	a_updateresfile(), C_WAITMOUSEUP(), C_WAITNEXTEVENT(), a_wakeup(), a_zeroscrap();
void	a_settrapaddress();

/* NOTE:  Changes here need to also happen to tables in Data.h and Data.a */
/* NOTE:  Mark all new traps (i.e. the ones you don't expect to already
 * exist with ISNEWTRAP in the oldtrap field in each entry of the patchtraps[] array.
 */
PatchEntry patchtraps[] =

	{
	/* ROM78 Traps */
	0x1E1, ToolTrap, a_handtohand, 0,
	
	/* ROM75 Traps */
	0x14D, ToolTrap, C_ADDRESMENU, 0,
	0x0FC, OSTrap, a_checkload, 0,
	0x001, OSTrap, a_close, 0,
	0x023, OSTrap, a_disposhandle, 0,
	0x137, ToolTrap, a_drawmenubar, 0,
	0x017, OSTrap, a_eject, 0,
	0x171, ToolTrap, C_EVENTAVAIL, 0,
	0x1F4, ToolTrap, a_exittoshell, 0,
	0x1A1, ToolTrap, a_getnamedresource, 0,
	0x170, ToolTrap, C_GETNEXTEVENT, 0,
	0x1A6, ToolTrap, a_getresattrs, 0,
	0x1A0, ToolTrap, a_getresource, 0,
	0x1FD, ToolTrap, a_getscrap, 0,
	0x014, OSTrap, a_getvol, 0,
	0x007, OSTrap, a_getvolinfo, 0,
	0x126, OSTrap, a_handlezone, 0,
	0x060, OSTrap, a_hfsdispatch, 0,
	0x112, ToolTrap, C_INITWINDOWS, 0,
	0x1F2, ToolTrap, a_launch, 0,
	0x13E, ToolTrap, C_MENUKEY, 0,
	0x13D, ToolTrap, C_MENUSELECT, 0,
	0x00F, OSTrap, a_mountvol, 0,
	0x022, OSTrap, a_newhandle, 0,
	0x035, OSTrap, a_offline, 0,
	0x000, OSTrap, a_open, 0,
	0x1B6, ToolTrap, C_OPENDESKACC, 0,
	0x00A, OSTrap, a_openrf, 0,
	0x08F, ToolTrap, a_osdispatch, ISNEWTRAP,
	0x073, OSTrap, a_osreserved, ISNEWTRAP,
	0x1EA, ToolTrap, a_pack3, 0,
	0x02F, OSTrap, a_postevent, 0,
	0x1FE, ToolTrap, a_putscrap, 0,
	0x04F, OSTrap, C_RDRVRINSTALL, 0,
	0x027, OSTrap, a_reallochandle, 0,
	0x128, OSTrap, a_recoverhandle, 0,
	0x1A3, ToolTrap, a_releaseresource, 0,
	0x051, ToolTrap, C_SETCURSOR, 0,
	0x04B, OSTrap, a_setgrowzone, 0,
	0x1A5, ToolTrap, a_sizersrc, 0,
	0x173, ToolTrap, C_STILLDOWN, 0,
	0x1C2, ToolTrap, C_SYSEDIT, 0,
	0x1C9, ToolTrap, a_syserror, 0,
	0x1B3, ToolTrap, C_SYSTEMCLICK, 0,
	0x1B2, ToolTrap, C_SYSTEMEVENT, 0,
	0x1B5, ToolTrap, C_SYSTEMMENU, 0,
	0x1B4, ToolTrap, C_SYSTEMTASK, 0,
	0x00E, OSTrap, a_unmountvol, 0,
	0x199, ToolTrap, a_updateresfile, 0,
	0x177, ToolTrap, C_WAITMOUSEUP, 0,
	0x060, ToolTrap, C_WAITNEXTEVENT, ISNEWTRAP,
	0x08C, OSTrap, a_wakeup, ISNEWTRAP,
	0x1FC, ToolTrap, a_zeroscrap, 0,
	
	/* HEY YOU! ==> SetTrapAddress MUST be the last real entry, since we don't want
	 * its patched effect when WE are patching.
	 */
	0x47,  OSTrap, a_settrapaddress, 0,
	0, 0, 0, 0, /* last entry */
	}; 
					
/************************************************************************************
 *							OSDispatch dispatch table
 ***********************************************************************************/

void	DOREQUESTSCRATCHSWITCHTASK(), DOGETACTIVETASKINFO(), DOGETSWITCHINFO(), DOCANCELSWITCH();
void	C_GETLAYER(), C_LCALCVIS(), C_LCVBEHIND(), C_LPAINTBEHIND(), C_LCLIPABOVE();
void	C_LPAINTONE(), C_LAYERINSERT(), C_LAYERDELETE(), C_MAKETOPLAYER();
void	C_LAYERCLOSE(), C_LAYERINIT(), C_LAYERFIND(), C_KERNELLAUNCH(), C_TWFGSWITCH();
void	C_GETNEXTHIGHLEVELEVENT(), C_TWGETPID(), C_NEWDESKTOPLAYEROWNER(), C_TEMPMAXMEM(), C_TEMPTOPMEM();
void	C_TWGETPROCINFO(), C_TEMPFREEMEM(), C_DISABLESWITCHING(), C_ENABLESWITCHING();
void	C_TWPOSTPSEUDOEVENT(), C_SLEEPPROCESS(), C_TEMPNEWHANDLE(), C_TEMPHLOCK();
void	C_TEMPHUNLOCK(), C_TEMPDISPOSEHANDLE(), C_LNEWWINDOW(), C_LCLOSEWINDOW();
void	C_LGETAUXWIN(), C_TWPOSTALLPSEUDOEVENT(), C_LCOLORINVALRECT(), C_SETSTDFILEDIR();
void	C_GETPRTYPESTRINGS(), C_SETPRTYPESTRINGS(), C_BACKGROUNDNOTIFY(), C_LSHOWHIDE();
void	C_LGETNEXTLAYER(), C_GETPROCESSTRAPADDRESS(), C_SETPROCESSTRAPADDRESS(), C_READWRITEPROCESSMEMORY();
void	C_READWRITEPROCESSFPUREGS(), C_REGISTERDEBUGGER(), C_ADDAPPLEMENUITEM();
void	C_DELETEAPPLEMENUITEM(), C_ACCEPTHIGHLEVELEVENT(), C_POSTHIGHLEVELEVENT();
void	C_GETPSNFROMPORTNAME(), C_LAUNCHDESKACCESSORY(), C_GETCURRENTPROCESS();
void	C_GETNEXTPROCESS(), C_GETPROCESSFROMLAYER(), C_GETPROCESSINFORMATION();
void	C_SAMEPROCESS(), C_SETFRONTPROCESS(), C_WAKEUPPROCESS();
void	C_GETAUXMENUITEM(), C_GETSYSTEMCLIENTPROCESS(), C_BEGINSYSTEMMODE();
void	C_ENDSYSTEMMODE(), C_REQUESTVOLUMENOTIFICATION(), C_DECLINEVOLUMENOTIFICATION();
void	C_KILLPROCESS(), C_GETSPECIFICHIGHLEVELEVENT(), C_GETPORTNAMEFROMPSN();
void	C_SYSZONEFLOATSIZES();
void	C_NEWFLOATLAYER(), C_DISPOSEFLOATLAYER(), C_POSTFAKEKEYDOWN(), C_POSTTEXT();
void	C_INLINEAWARE(), C_NEWTSMLAYEROWNER(), C_ROUTEEVENTTOSWM();
void	C_PENTRYFROMPROCESSSERIALNUMBER();

#ifdef MODSQUAD
void	C_ACCEPTDRAG();
#endif

OSDispatchEntry OSDispatchTable[] = {

	DOREQUESTSCRATCHSWITCHTASK, REQUESTSCRATCHSWITCHDEPTH, REQUESTSCRATCHSWITCHRETSIZE,
	DOGETACTIVETASKINFO, GETACTIVETASKINFODEPTH, GETACTIVETASKINFORETSIZE,
	DOGETSWITCHINFO, GETSWITCHINFODEPTH, GETSWITCHINFORETSIZE,
	DOCANCELSWITCH, CANCELSWITCHDEPTH, CANCELSWITCHRETSIZE,

	C_GETLAYER, GETLAYERDEPTH, GETLAYERRETSIZE,
	C_LCALCVIS, LCALCVISDEPTH, LCALCVISRETSIZE,
	C_LCVBEHIND, LCVBEHINDDEPTH, LCVBEHINDRETSIZE,
	C_LPAINTBEHIND, LPAINTBEHINDDEPTH, LPAINTBEHINDRETSIZE,
	C_LCLIPABOVE, LCLIPABOVEDEPTH, LCLIPABOVERETSIZE,	
	C_LPAINTONE, LPAINTONEDEPTH, LPAINTONERETSIZE,
	C_LAYERINSERT, LAYERINSERTDEPTH, LAYERINSERTRETSIZE,
	C_LAYERDELETE, LAYERDELETEDEPTH, LAYERDELETERETSIZE,
	C_MAKETOPLAYER, MAKETOPLAYERDEPTH, MAKETOPLAYERRETSIZE,
	C_LAYERCLOSE, LAYERCLOSEDEPTH, LAYERCLOSERETSIZE, 
	C_LAYERINIT, LAYERINITDEPTH, LAYERINITRETSIZE, 
	C_LAYERFIND, LAYERFINDDEPTH, LAYERFINDRETSIZE,
	C_KERNELLAUNCH, KERNELLAUNCHDEPTH, KERNELLAUNCHRETSIZE,
	C_TWFGSWITCH, TWFGSWITCHDEPTH, TWFGSWITCHRETSIZE,
	C_GETNEXTHIGHLEVELEVENT, GETNEXTHIGHLEVELEVENTDEPTH, GETNEXTHIGHLEVELEVENTRETSIZE,
	C_TWGETPID, TWGETPIDDEPTH, TWGETPIDRETSIZE,
	C_NEWDESKTOPLAYEROWNER, NEWDESKTOPLAYEROWNERDEPTH, NEWDESKTOPLAYEROWNERRETSIZE,
	C_TEMPMAXMEM, TEMPMAXMEMDEPTH, TEMPMAXMEMSIZE,
	C_TEMPTOPMEM, TEMPTOPMEMDEPTH, TEMPTOPMEMSIZE,
	C_TWGETPROCINFO, TWGETPROCINFODEPTH, TWGETPROCINFOSIZE, 
	C_TEMPFREEMEM, TEMPFREEMEMDEPTH, TEMPFREEMEMSIZE,
	C_DISABLESWITCHING, DISABLESWITCHINGDEPTH, DISABLESWITCHINGSIZE,
	C_ENABLESWITCHING, ENABLESWITCHINGDEPTH, ENABLESWITCHINGSIZE,
	C_TWPOSTPSEUDOEVENT, TWPOSTPSEUDODEPTH, TWPOSTPSEUDORETSIZE,
	C_SLEEPPROCESS, SLEEPPROCESSDEPTH, SLEEPPROCESSRETSIZE,
	C_TEMPNEWHANDLE, TEMPNEWHANDLEDEPTH, TEMPNEWHANDLESIZE,
	C_TEMPHLOCK, TEMPHLOCKDEPTH, TEMPHLOCKSIZE,
	C_TEMPHUNLOCK, TEMPHUNLOCKDEPTH, TEMPHUNLOCKSIZE,
	C_TEMPDISPOSEHANDLE, TEMPDISPOSEHANDLEDEPTH, TEMPDISPOSEHANDLESIZE,
	C_LNEWWINDOW, LNEWWINDOWDEPTH, LNEWWINDOWRETSIZE,
	C_LCLOSEWINDOW, LCLOSEWINDOWDEPTH, LCLOSEWINDOWRETSIZE,
	C_LGETAUXWIN, LGETAUXWINDEPTH, LGETAUXWINRETSIZE,
	C_TWPOSTALLPSEUDOEVENT, TWPOSTALLPSEUDODEPTH, TWPOSTALLPSEUDORETSIZE, 
	C_LCOLORINVALRECT, LCOLORINVALRECTDEPTH, LCOLORINVALRECTRETSIZE,
	C_SETSTDFILEDIR, SETSTDFILEDIRDEPTH, SETSTDFILEDIRRETSIZE,
	C_GETPRTYPESTRINGS, GETPRTYPESTRINGSDEPTH, GETPRTYPESTRINGSRETSIZE,
	C_SETPRTYPESTRINGS, SETPRTYPESTRINGSDEPTH, SETPRTYPESTRINGSRETSIZE,
	C_BACKGROUNDNOTIFY, BACKGROUNDNOTIFYDEPTH, BACKGROUNDNOTIFYRETSIZE,
	C_LSHOWHIDE, LSHOWHIDEDEPTH, LSHOWHIDERETSIZE,
	C_LGETNEXTLAYER, LGETNEXTLAYERDEPTH, LGETNEXTLAYERRETSIZE,
	C_GETPROCESSTRAPADDRESS, GETPROCESSTRAPADDRESSDEPTH, GETPROCESSTRAPADDRESSRETSIZE,
	C_SETPROCESSTRAPADDRESS, SETPROCESSTRAPADDRESSDEPTH, SETPROCESSTRAPADDRESSRETSIZE,
	C_READWRITEPROCESSMEMORY, READWRITEPROCESSMEMORYDEPTH, READWRITEPROCESSMEMORYRETSIZE,
	C_READWRITEPROCESSFPUREGS, READWRITEPROCESSFPUREGSDEPTH, READWRITEPROCESSFPUREGSRETSIZE,
	C_REGISTERDEBUGGER, REGISTERDEBUGGERDEPTH, REGISTERDEBUGGERRETSIZE,
	C_ADDAPPLEMENUITEM, ADDAPPLEMENUITEMDEPTH, ADDAPPLEMENUITEMRETSIZE,
	C_DELETEAPPLEMENUITEM, DELETEAPPLEMENUITEMDEPTH, DELETEAPPLEMENUITEMRETSIZE,
	C_ACCEPTHIGHLEVELEVENT, ACCEPTHIGHLEVELEVENTDEPTH, ACCEPTHIGHLEVELEVENTRETSIZE,
	C_POSTHIGHLEVELEVENT, POSTHIGHLEVELEVENTDEPTH, POSTHIGHLEVELEVENTRETSIZE,
	C_GETPSNFROMPORTNAME, GETSERIALNUMBERFROMPORTNAMEDEPTH, GETSERIALNUMBERFROMPORTNAMERETSIZE,
	C_LAUNCHDESKACCESSORY, LAUNCHDESKACCESSORYDEPTH, LAUNCHDESKACCESSORYSIZE,
	C_GETCURRENTPROCESS, GETCURRENTPROCESSDEPTH, GETCURRENTPROCESSRETSIZE,
	C_GETNEXTPROCESS, GETNEXTPROCESSDEPTH, GETNEXTPROCESSRETSIZE,
	C_GETPROCESSFROMLAYER, GETPROCESSFROMLAYERDEPTH, GETPROCESSFROMLAYERRETSIZE,
	C_GETPROCESSINFORMATION, GETPROCESSINFORMATIONDEPTH, GETPROCESSINFORMATIONRETSIZE,
	C_SETFRONTPROCESS, SETFRONTPROCESSDEPTH, SETFRONTPROCESSRETSIZE,
	C_WAKEUPPROCESS, WAKEUPPROCESSDEPTH, WAKEUPPROCESSRETSIZE,
	C_SAMEPROCESS, SAMEPROCESSDEPTH, SAMEPROCESSRETSIZE,
	C_GETAUXMENUITEM, GETAUXMENUITEMDEPTH, GETAUXMENUITEMRETSIZE,
 	C_GETSYSTEMCLIENTPROCESS, GETSYSTEMCLIENTPROCESSDEPTH, GETSYSTEMCLIENTPROCESSRETSIZE,
 	C_BEGINSYSTEMMODE, BEGINSYSTEMMODEDEPTH, BEGINSYSTEMMODERETSIZE,
 	C_ENDSYSTEMMODE, ENDSYSTEMMODEDEPTH, ENDSYSTEMMODERETSIZE,
 	C_REQUESTVOLUMENOTIFICATION, REQUESTVOLUMENOTIFICATIONDEPTH, REQUESTVOLUMENOTIFICATIONRETSIZE,
 	C_DECLINEVOLUMENOTIFICATION, DECLINEVOLUMENOTIFICATIONDEPTH, DECLINEVOLUMENOTIFICATIONRETSIZE,
	C_KILLPROCESS, KILLPROCESSDEPTH, KILLPROCESSRETSIZE,
	C_GETSPECIFICHIGHLEVELEVENT, GETSPECIFICHIGHLEVELEVENTDEPTH, GETSPECIFICHIGHLEVELEVENTRETSIZE,
	C_GETPORTNAMEFROMPSN, GETPORTNAMEFROMPSNDEPTH, GETPORTNAMEFROMPSNRETSIZE,
	C_SYSZONEFLOATSIZES, SYSZONEFLOATSIZESDEPTH, SYSZONEFLOATSIZESRETSIZE,
	C_NEWFLOATLAYER, NEWFLOATLAYERDEPTH, NEWFLOATLAYERRETSIZE,
	C_DISPOSEFLOATLAYER, DISPOSEFLOATLAYERDEPTH, DISPOSEFLOATLAYERRETSIZE,
	C_POSTFAKEKEYDOWN, POSTFAKEKEYDOWNDEPTH, POSTFAKEKEYDOWNSIZE,
	C_POSTTEXT, POSTTEXTDEPTH, POSTTEXTSIZE,
	C_INLINEAWARE, INLINEAWAREDEPTH, INLINEAWARESIZE,
	C_NEWTSMLAYEROWNER, NEWTSMLAYEROWNERDEPTH, NEWTSMLAYEROWNERSIZE,
	C_ROUTEEVENTTOSWM, ROUTEEVENTTOSWMDEPTH, ROUTEEVENTTOSWMSIZE,
	C_PENTRYFROMPROCESSSERIALNUMBER, PENTRYFROMPROCESSSERIALNUMBERDEPTH, PENTRYFROMPROCESSSERIALNUMBERSIZE,
#ifdef MODSQUAD
	C_ACCEPTDRAG, ACCEPTDRAGDEPTH, ACCEPTDRAGSIZE,
#endif
	nil, 0, 0					/* end of table */
	};
