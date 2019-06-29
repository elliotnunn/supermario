/*
	File:		Data.h

	Contains:	Structures and externs of Process Manager global data

	Written by:	Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<45>	 12/4/92	DRF		Added nativeFPR storage area in PCB. Fixed up some #includes.
		<44>	11/25/92	DRF		Added additional PCB field for tracking the application’s code
									fragment manager context.  Changes for <44> and <43> are
									conditionalized under PsychicTV.
		<43>	11/25/92	DRF		Added some #includes for PowerPC Code Fragment Manager.
		<42>	 8/26/92	DTY		Change ModSquad’s funky conditional to use #ifdefs like everyone
									else.
		<41>	 8/26/92	DTY		Roll in latest changes for A/UX.
		<40>	 8/17/92	hjcr	Adding support for OCE's "defer drag" feature. Conditionally
									compiled under MODSQUAD
		<39>	  8/4/92	KST		Added comments for the TSM change.
		<38>	 6/22/92	DTY		#1033275 <csd>: Change the 16-1 to a 15 because of some compiler
									idiosyncracies.
		<37>	 6/12/92	pvh		#1032359: Added QUAD_ALIGN and QUADALIGN_MASK macros for 16 byte
									alignment on '040 Macs.
		<36>	 3/30/92	DTY		#1025416,<DDG>: Add skiaExists global.
		<35>	 3/25/92	DTY		#1024114,<pvh>: Add c_PEntryFromProcessSerialNumber to the
									OSDispatch table. Check out the comment in OSDispatch.c, version
									12 about why this routine is being added.
		<34>	 2/18/92	DTY		#1021226: Switch emMessageManagerGlobals again.
		<33>	 1/11/92	YK		Two more calls and two more gloabls for TSM. NewTSMLayerOwner to
									get the event for the floating window. RouteEventToSWM to do the
									bottom line input.
		<32>	11/22/91	DTY		Add a semaphore, allocateGMBlkInSysHeap, which tells GMBlk to
									automatically go to the System heap for memory instead of
									mucking about with Process Manager memory first. This is a 7-Up
									fix to prevent fragmentation of the Process Manager heap when an
									'obit' event is sent.
		<31>	10/16/91	YK		Added InlineAware dispatch entry.
		<30>	10/15/91	YK		Added global data and dispatch entries for supporting the Text
									Service Manager.
		<29>	 10/9/91	YK		Added IMLayer(LayerPtr :global), NewFloatLayer and
									DisposeFloatLayer (traps) all for supporting input windows for
									text services.  Particularly, FEP's (input methods).
		<28>	 10/4/91	JSM		Change PsychoticFarmerOrLater to TheFuture.
		<27>	 9/22/91	DTY		Change PsychoticFarmerAndLater to PsychoticFarmerOrLater.
		<26>	 9/16/91	DFH		Removed INITSR (#define to 0x2000). Added initialProcessSR.
									Needed for NuKernel.
		<25>	 9/13/91	DTY		Conditionalise previous change so it doesn’t get built for
									CubeE. (It’ll get built for PsychoticFarmerAndLater.)
		<24>	  6/4/91	DFH		Added emMessageManagerGlobals to PCB.
		<23>	 3/22/91	DFH		jmp,WS#DFH-910322a: Fixed FPUUserRegs defintion to use
									"extended96" for type of FP registers. Was using plain
									"extended", which is only 80 bits (SANE-style) rather than 96
									bits (Motorola-style). This caused ReadWriteProcessFPURegs to
									return less than the full complement of registers. This doesn't
									effect any other code.
		<22>	 2/25/91	DFH		DC,WS#DFH-910225a: Added p_scrapIsNotFromLaunch.
		<21>	 2/21/91	DFH		dba,#82504, #82681, #83168, #83182, #83207: HiNewHandle rolled
									into ProcessMgrHiNewHandle. Changed prototype. Added
									SysZoneFloatSizes.
		<20>	  2/9/91	JWM		DFH,#79583:Restart and Shutdown don't work in low memory
									conditions. A new routine was defined -- GMBlk. It is modeled
									after ProcessMgrHiNewHandle but checks the system heap if
									ProcessMgrHiNewHandle returns a null handle.
		<19>	 1/28/91	DFH		JSM,#81425: Removed inhibit_DAs global and PopUpMenuSelect
									patch.
		<18>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
		<17>	 1/15/91	DFH		(VL) Conditionalize out the AUX fields in PEntry.
		<16>	 1/14/91	DFH		(dba) Make system mode per process.
		<15>	 1/14/91	DFH		(JDR) Conditionalize out AUX support.
		<14>	 1/10/91	DFH		(JDR) Added p_preventFirstSysBeepPatch.
		<13>	  1/8/91	DFH		(rdd) Got rid of pErrantProcess since it is no longer needed.
		<12>	12/20/90	DFH		(dba) Removed obsolete dispatch TWKill.  Replaced it with
									GetNextHighLevelEvent.
		<11>	12/18/90	DFH		Added GetScrap patch.
		<10>	12/17/90	DFH		Touched up Aliases.h include.
		 <9>	12/14/90	DFH		Added kernelMode.
		 <8>	12/10/90	DFH		Moved process icon info from PCB to PEntry.
		 <7>	 12/5/90	DFH		Added AUXIsPresent and AUX fields in PEntry.  Moved VBLDesc and
									DCEDesc here from SysMisc.h.
		 <6>	11/26/90	DFH		Removed trespass() macro.
		 <5>	 11/6/90	DFH		Renamed emAppleEventsGlobal to emAppleEvents.
		 <4>	 11/5/90	DFH		Correct names for TEMPTOPMEM and TEMPDISPOSEHANDLE.
		 <3>	 11/1/90	DFH		Added prototype for DebuggerMsg().
		 <0>	  9/2/86	ELR		New Today.

*/ 

#ifndef __DATA__
#define __DATA__

/* Pointer to process entry */
typedef	struct PEntry		*PEntryPtr;

/* Non-unique process identifier */
typedef short				ProcessID;
#define MAX_PROCESSID		(0x7FFF)

#include	<menus.h>

#ifndef __EVENTS__
#include	<events.h>
#endif
#ifndef __MEMORY__
#include	<memory.h>
#endif
#ifndef __ALIASES__
#include 	<Aliases.h>
#endif
#ifndef __SANE__
#include 	<Sane.h>
#endif
#ifndef __SYSMISC__
#include	"SysMisc.h"
#endif
#ifndef __HLIST__
#include 	"HList.h"
#endif
#ifndef __OSDISPATCHPRIVATE__
#include	"OSDispatchPrivate.h"
#endif
#ifndef __PROCESSES__
#include 	<Processes.h>
#endif
#ifndef __EPPCPRIVATE__
#include	"EppcPrivate.h"
#endif
#ifndef __PUPPET__
#include 	"Puppet.h"
#endif
#ifndef __SCRAPCOERCION__
#include	"ScrapCoercion.h"
#endif

#if		PsychicTV
#include	<CodeFragments.h>
#include	<CodeFragmentsTrap.h>
#endif	PsychicTV

/************************************************************************************
 * 					Generally useful things, not dependant on Process Mgr			*
 ************************************************************************************/

/* Shortened names for some types */
typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned long	u_long;
typedef unsigned long	u_size;

/* Word-sized Boolean values */
#define wTrue			(0x0100)
#define wFalse			(0x0000)

/* Macro for easy access to QuickDraw data */
#ifdef __QUICKDRAW__
#define GET_BOUNDS(pBits) (((pBits)->rowBytes < 0)	\
	? &((**((BitMap ***) (pBits)))->bounds) 		\
	: &((pBits)->bounds))
#endif

/* Macros to get tiny pascal strings */
#define ONE_CHAR_STRING(ch1)				(((1)<<8) + ch1)
#define TWO_CHAR_STRING(ch1, ch2)			(((2)<<24) + ((ch1)<<16) + ((ch2)<<8))
#define THREE_CHAR_STRING(ch1, ch2, ch3)	(((3)<<24) + ((ch1)<<16) + ((ch2)<<8) + ch3) 

/* Macro to look up a particular byte in a pascal string */
#define StringByte(pStr, byteIndex)			(*((char *) pStr + byteIndex + 1))

/* Cute macros for type conversions */
#define TWO_CHARS_TO_SHORT(ch1,ch2)		(((ch1)<<8)+(ch2))
#define SET_PT(pPt,x,y)					(*((long *) (pPt)) = (y)*0x10000 + (x))
#define PT_TO_PT(pPtSrc,pPtDst)			(*((long *) (pPtDst)) = *((long *) (pPtSrc)))
#define LONG_TO_PT(theLong,pPtDst)		(*((long *) (pPtDst)) = (theLong))
#define PT_AS_LONG(pPtSrc)				(*((long *) (pPtSrc)))
#define SET_RECT(pRect, x_ulh, y_ulh, x_lrh, y_lrh) \
	SET_PT(&(pRect)->top, (x_ulh), (y_ulh)), SET_PT(&(pRect)->bottom, (x_lrh), (y_lrh))

/* Masks and a macro to ensure alignment of counts and addresses.  Word alignment is
 * required by the 68000 for non-byte pointer refereneces.  Note that the result of
 * WORD_ALIGN is the same as the original if the original was already word-aligned.
 * Long alignment is for 68020 (and up) efficiency in general, and is a requirement
 * of blocks in a 32-bit mode heap. 
 *
 * QUAD_ALIGN added to be parallel with 68040 memory manager overpatch.					// <37>
 */
#define QUADALIGN_MASK		0xFFFFFFF0													// <37>
#define LONGALIGN_MASK		0xFFFFFFFC
#define WORDALIGN_MASK		0xFFFFFFFE
#define QUAD_ALIGN(value)	(value = ((value + 15) & QUADALIGN_MASK))					// <37>
#define LONG_ALIGN(value)	(value = ((value + sizeof(long) - 1) & LONGALIGN_MASK))
#define WORD_ALIGN(value)	(value = ((value + sizeof(short) - 1) & WORDALIGN_MASK))
#define LONG_ALIGNED(value)	(((value) & (sizeof(long) - 1)) == 0)
#define WORD_ALIGNED(value)	(((value) & (sizeof(short) - 1)) == 0)

/* Largest value for an unsigned field of any size */
#define MAXVALUE		(0xFFFFFFFF)

/* Prototypes for utility routines */
void			_DataInit(void);
Handle			ProcessMgrHiNewHandle(u_size size, Ptr lockPtr);
Handle			ProcessMgrNewHandle(u_size);
Handle			GMBlk(u_size logicalSize, OSErr *error);
void			MemClear(Ptr, unsigned long);
StringPtr		StripLeadingNulls(StringPtr);
Handle			NewMungedString(StringPtr, StringPtr);
typedef short	(*RelStringGluePtr)(StringPtr pStr1, StringPtr pStr2);
StringPtr		StrInStrList(Ptr, Ptr, short, RelStringGluePtr);
Handle			GetRsrcHdlMatchingFirstLongword(long, long);
short			GetIconIDFromBundle(OSType, unsigned long);
Boolean			OptionKeyIsDown(void);
unsigned short	disable(void);
void			spl(unsigned short);


/************************************************************************************
 * 				Process Mgr structure prototypes and access macros					*
 ************************************************************************************/

/*
 * Secondary process information structures.
 */

/* Printer type strings */
#define PRTYPE_STRING_SIZE	32
typedef Str31				PrTypeString, *PrTypeStringPtr, **PrTypeStringHandle;
		
/* Info specifying one directory */
typedef struct AppWDRec
	{
	short			awdVolume;
	unsigned long	awdDirID;
	} AppWDRec, *AppWDRecPtr;

/* List of directory specifiers. */
typedef struct AppWDList
	{
	short		wdlCount;
	AppWDRec	wdlDirectories[0];
	} AppWDList, *AppWDListPtr, **AppWDListHdl;

/* VBL descriptor */
typedef struct VBLDesc {
	VBLTask				*v_eladdr;
	char				*v_addr;
	short				v_count;
	} VBLDesc, *VBLDescPtr, **VBLDescHdl;

/* Driver/DA descriptor */
typedef struct DCEDesc {
	short				d_unit;
	Handle				d_handle;
	} DCEDesc, *DCEDescPtr, **DCEDescHdl;

/* Auxiliary process information in relocatable block */
/* ScriptMgrKeyGetSet.a is depending on the offset of currenta5 in this record.	*/
/* If you change this record, be sure to update ScriptMgrKeyGetSet.a	 <#39>	*/
typedef struct PCB								
	{
	/* OS lomem */
	short			sysevtmask;
	short			bootvol;
	Ptr				switchdata;
	char			*defvcbptr;
	WDCB			defaultwdcb;
	short			defvrefnum;
	Ptr				pmsphook;
	void			(*fsqueuehook)();
	short			dserrcode;
	
	/* Toolbox lomem */
	char			*stklowpt;
	char			*appllimit;
	THz				applzone;
	unsigned long	currenta5;
	Str31			curapname;
	LayerPtr		curlayer;
	Handle			topmaphandle;
	short			curmap;
	Handle			menucinfo;
	short			sfsavedisk;
	long			curdirstore;
	
	/* switched ExpandMem */
	Ptr				emScriptAppGlobals;			/* Script Mgr stuff to be zeroed, switched */
	long			emAppleEvents;				/* AppleEvent Mgr stuff to be zeroed, switched */
	Handle			emEditionMgrPerApp;			/* Edition Mgr stuff to be zeroed, switched */
	long			emMessageManagerGlobals;	/* Message Mgr stuff to be zeroed, switched */

	/* process specific fields */	
	VBLDescHdl		vblvars;					/* VBLs switched out last time */
	DCEDescHdl		dces;						/* drivers switched out last time */
	Handle			applPatchTable;				/* registry of patches by application */
	short			applPatchCount;				/* count of traps patched */
	Ptr				appSystemEventPatch;		/* Special overlay patch, if needed */
	Ptr				appRecoverHandlePatch;		/* Special overlay patch, if needed */
	Handle			puppetstringhdl;			/* process' custom puppet string information */
	PrTypeString	localPrTypeString;			/* name of printer type */
	char			enablePrTypeChanges;		/* Can this proc change printer types if chooser does? */
	short			p_fileRefNum;				/* resource file refnum */
	short			p_vrefnum;					/* WD for parent directory of app file */
	short			p_appVolume;				/* Volume for p_vrefnum */
	long			p_appDirID;					/* Directory for p_vrefnum */
	AppWDListHdl	p_appWDList;				/* WDs to open after launch */
	unsigned long	launchDate;					/* TICKS at time of launch */
	HListHdl		p_PseudoEvtQHList;			/* pseudo event queue */
	ProcPtr			p_gzproc;					/* pointer to his appl zone grow zone proc */
	HListHdl		tempMemHListHdl;			/* list of all the temporary handles for this process */
	short			p_daResource;				/* DA resource number if this is DA handler */
#if	PsychicTV
	FragContextID	fragContext;				/* <44> CodeFragMgr context for this process			*/
	unsigned long	nativeFPRStorage[18*2];		/* <45> Storage for 18 64 bit floating point registers	*/
#endif
	} PCB;

/* Define the descriptors for the Apple menu items that applications can add.
 * IMPORTANT: aiNext must be in 0th long of the record so that the insertion
 * code in AddAppleMenuItem works.  aiNameStorage is at the end so we can put
 * the (variable length) name there.
 */
typedef struct AppleItem
	{
	struct AppleItem		**aiNext;			/* list link (DO NOT MOVE) */
	Handle					aiIcon;				/* item icon handle */
	PEntryPtr				aiProcess;			/* process that installed this entry */
	long					aiRefCon;			/* identifier to send to aiProcess */
	short					aiScript;			/* script code (international) */
	Style					aiStyle;			/* text styles */
	char					aiMark;				/* item mark character */
	StringHandle			aiHelpString;		/* descriptive string */
	unsigned char			aiNameStorage[];	/* item text */
	} AppleItem, *AppleItemPtr, **AppleItemHdl;

/* Machine register context */
typedef struct SaveRegs							/* general registers */
	{
	unsigned long	d2,d3,d4,d5,d6,d7;
	unsigned long	a2,a3,a4,a5,a6;
	unsigned short	sr;
	unsigned long	pc;
	} SaveRegs;
	
typedef struct FPUUserRegs						/* floating pointer registers */
	{
	unsigned long	fpcr, fpsr, fpiar;
	extended96		fp0, fp1, fp2, fp3, fp4, fp5, fp6, fp7;
	} FPUUserRegs;
	
typedef struct FPUFrame							/* frame for FSAVE/FRESTORE instructions */
	{
	char			isFullFPUFrame;
	FPUUserRegs		fpuUserRegs;
	} FPUFrame;

/* This is the format of the register context save area pushed on the stack
 * by the process switching routines (SwitchAllRegs and SwitchCPURegs).
 */
typedef struct AllRegs							/* altogether now */
	{
	SaveRegs		saveRegs;
	unsigned long	tmp1, tmp2;
	FPUFrame		fpuFrame;
	} AllRegs;

/* Initial value in process context */
#define INITRET		userret						/* process' return address from main routine */

/* Primary process information structure */
/* ScriptMgrKeyGetSet.a is depending on the offset of p_pcb in this record.		*/
/* If you change this record, be sure to update ScriptMgrKeyGetSet.a	 <#39>	*/

typedef struct		PEntry
	{
	short			p_state;					/* process state */
	ProcessID		p_mypid;					/* the process id */
	long			p_type;						/* type of task (usually APPL) */
/* NOTE:  _Open patch assumes offset of p_signature field is 8 */
	long			p_signature;				/* signature of application */
	unsigned short	p_nameScript;				/* script code from app file name */		
	Boolean			p_scrapIsNotFromLaunch;		/* whether we've tried to MoveScrap() */		
#ifdef DEBUG
	unsigned long	p_tattoo;					/* telltale field */
#endif DEBUG
	unsigned long	p_version;					/* version of application, for MicroSoft */
	PEntryPtr		p_NextProcess;				/* link to next process on system */
	PEntryPtr		p_NextProcessInState;		/* next process in same state list */
	PEntryPtr		p_PrevProcessInState;		/* previous process in same state list */
	PEntryPtr		p_nextQueuedFrontProcess;	/* next process in pFrontProcessQueue */
	ProcessSerialNumber		p_serialNumber;		/* serial number (unique since startup) */
	THz				p_zone;						/* pointer to application heap */
	THz				*p_zoneh;					/* Handle to minor zone */
	PEntryPtr		nextAppleMenuProc;			/* next process in all the apple menus */
	PEntryPtr		nextApplicationMenuProc;	/* next process in Application Menu */
	unsigned long	p_taskmode;					/* tasks created mode */
	Boolean			p_isfirstbackevt;			/* is this the first background event? */
	Boolean			p_preventFirstSysBeepPatch;	/* for HyperCard 1.2.5 */
	unsigned short	p_waitnexteventlevel;		/* (how many levels deep) is the proc inside a _WaitNextEvent call */
	PEntryPtr		p_dadProcess;				/* PEntryPtr to my dad */
	unsigned long	p_size;						/* size of his world */
	unsigned long	p_ssize;					/* size of his stack */
	char			*p_wptr;					/* a phony window ptr */
	PCB				**p_pcb;					/* Handle to process control block */
	unsigned long 	p_sp;						/* Saved stack pointer */
	short			p_eventmask;				/* The process' event mask */
	char			p_haseditcmdkeys;			/* whether or not the process can do cuts and copies */
	unsigned char	p_updatechances;			/* remaining chances to complete BG updates */
	short			p_cutcopycount;				/* how many cut or copies is he consistent with? */
	HListHdl		p_applemenuhlisthdl;		/* handle to list of this guy's apple menus or nil if none */
	Handle			p_hideCurrentStr;			/* handle to application menu front app hide item text */
	Handle			iconCacheHdl;				/* application icon family cache */
	short			iconResourceID;				/* application icon resource ID */
	LayerPtr		p_layer;					/* pointer to my layer */
	unsigned long	p_slices;					/* # of times process has CPU */
	unsigned long	p_wakeuptime;				/* TICKS at which to wake this process */
	unsigned long	p_activeTime;				/* Sum of ticks in possession of CPU */
	short			p_condemned;				/* whether someone is killing this process */
	unsigned short	p_systemModeLevel;			/* non-zero if system is its own client */
	PuppetVars		p_puppetvars;				/* puppet-string related variables */
	Handle			p_lmemtool;					/* handle to toolbox mem to be switched */
	EPPCBlk			eppcBlk;					/* High Level Event control block */
#ifdef HAS_AUX_PROCESSMGR
	short			aux_waitproc;				/* AUX process we're waiting to attach */
	short			aux_realpid;				/* AUX native pid, not related to the MF pid */
	short			wakeup_flag;				/* Process needs to leave GNE/EA early */
#endif HAS_AUX_PROCESSMGR
	Handle			p_fakeDAHandle;				/* BS handle for FullPaint and MacDraw II */
	LayerPtr		p_floatLayer;				/* For text services floating windows */
	Boolean			p_inlineAware;				/* Flag for the Text Services Manager */
	PEntryPtr		*p_HandleForThisPEntry;		/* == RecoverHandle(*(this_PEntry)) */
	} PEntry;

/* External declarations for PEntry locators */
#define HighestkProcess			(2)
#define LowestNonSystemPSN		(8*1024)
#define EqualPSNConst(X, A)	(((A)->lowLongOfPSN == (X)) && ((A)->highLongOfPSN == 0))
#define SetPSN(X, dst)		{(dst)->highLongOfPSN = 0; (dst)->lowLongOfPSN = (X);}
#define IsGreaterPSN(src, dst)							\
	(((src)->highLongOfPSN != (dst)->highLongOfPSN) ? 	\
		((src)->highLongOfPSN > (dst)->highLongOfPSN)	\
		: ((src)->lowLongOfPSN > (dst)->lowLongOfPSN))
extern Boolean		EqualPSN(ProcessSerialNumberPtr, ProcessSerialNumberPtr);
extern PEntryPtr	PEntryFromPSN(ProcessSerialNumberPtr);
extern PEntryPtr	PEntryFromPID(ProcessID);
extern PEntryPtr	PEntryFromFileSpec(FSSpecPtr, StringPtr);

#ifdef MODSQUAD
	//	Drag.h is not yet in the project.  These definitions belong there
#define _DragDispatch	0xABED
#define gestaltDragMgrVersion 'drag'			/* drag manager version */
pascal Boolean IsDragAware(WindowPtr, ProcessSerialNumber*)
			= { 0x303C, 0x040F, _DragDispatch };

#define bgMouseDown	16

#define DRAG_DONE		(0)
#define DRAG_INIT		(1)
#define DRAG_IGNORED	(2)
#endif

/* Process states (values of p_state) */
#define PRSLEEPING	(0)							/* on sleep queue, passing the time */
#define PRREADY 	(1)							/* ready to go */
#define PRNULL		(2)							/* the last guy you'd want to switch in */
#define PRBACKRUN	(3)							/* sliced in for some extracurricular time */
#define PRRUN		(4)							/* frontmost user process */
#define PRUPDATE	(5)							/* current, but only here for an update visit */
#define PRMOVING	(6)							/* moving from ready to RUNNING as foreground */
#define PRPUPPET	(7)							/* getting events from the scratchpad event list */

#ifdef MODSQUAD
#define	PRSTARTDRAG	(8)							/* waiting for a start drag call	*/
#endif

/* Special values of other fields */
#ifdef DEBUG
#define BRAND	'MOM '
#define IsProcessEntry(pProc)	(pProc->p_tattoo == BRAND)
#endif DEBUG

/* The file type for Finder or a Finder-replacement */
#define FINDERS_TYPE		'FNDR'
#define IS_FINDER(pProc)	(pProc->p_type == FINDERS_TYPE)

/* Data type for supporting the Text Service Manager.	*/
typedef struct FakeKeyRecord
	{
	 long message;								/*	We need these two fields of the EventRecord	 */
	 short modifiers;
	} FakeKeyRecord, *FakeKeyPtr, **FakeKeyHandle;


/************************************************************************************
 * 				Process Mgr global data prototypes and external declarations		*
 ************************************************************************************/

/* Macros for general heap setting in our patches/routines.  Many heap operations
 * on the SYSZONE or ProcessMgrZone can cause the Process Mgr heap to grow or shrink, which
 * will result in a new value in ProcessMgrZone.  We must be careful when setting THEZONE
 * around such operations: the saved value is obsolete if it had been ProcessMgrZone, and
 * ProcessMgrZone moved.  The solution is to NOT save and restore THEZONE if it is initially
 * equal to ProcessMgrZone.  Any change to ProcessMgrZone will be reflected automatically by
 * THEZONE.
 * NOTE: These macros are not needed in some special cases, like when a) you know
 * THEZONE != ProcessMgrZone to begin with, or b) you know the heap operations can not
 * cause SYSZONE or ProcessMgrZone to change size.
 * NOTE: Assumes a5 == PROCESSMGRGLOBALS
 */

#define SafeSetZone(saveZone, tempZone)		\
	{ if ((saveZone = THEZONE) == ProcessMgrZone) saveZone = nil; THEZONE = tempZone; }
#define SafeRestoreZone(saveZone)		{ if (saveZone != nil) THEZONE = saveZone; }

/* External declarations for global data */
extern PEntry			NullProcess;
#define	pNullProcess	(&NullProcess)
#define pProcessList	pNullProcess->p_NextProcess

#ifdef HAS_AUX_PROCESSMGR
extern Boolean			AUXIsPresent;
#endif HAS_AUX_PROCESSMGR

extern unsigned long	ProcessNumberSource;
extern ProcessID		ProcessIDSource;
extern short			nbacks;
extern short			remnulls;
extern short			initialProcessSR;
extern PEntryPtr		pCurrentProcess;
extern PEntryPtr		pFrontProcess;
extern PEntryPtr		pNewFrontProcess;
extern PEntryPtr		pFrontProcessQueue;
extern PEntryPtr		pLastBGProcess;
extern PEntryPtr		pShellProcess;
extern PEntryPtr		pDebugProcess;

extern PEntryPtr		pSleepingStateList;
extern PEntryPtr		pDyingStateList;

extern Boolean			ShellWasFinderType;

extern LayerPtr			MFLayer;
extern LayerPtr			IMLayer;
extern LayerPtr			desklayer;
extern PEntryPtr		desklayerowner;

extern unsigned long 	lastswitch;
extern unsigned long	nextswitch;

extern short			cutCopyCount;
extern short			coercionState;
extern short			dont_switch;
extern Boolean			napOver;

#ifdef MODSQUAD
extern short			gDragState;
extern Boolean			gDragMgrIsAvailable;
extern EventRecord		gDragEvent;
extern PEntryPtr		gFrontDragProcess;
#endif

extern MyScrapInfo		srcMyScrapInfo;
extern short			nullTimeout;

extern AppleItemHdl		FirstAppleItem;
extern Handle			hGenericAppIconSuite;
extern Handle			hGenericDAIconSuite;

extern Handle			standardMenuDefProcRsrc;
extern MenuHandle		ApplicationMenuHdl;
extern StringHandle		hHideCurrent;

extern THz				ProcessMgrZone;
extern HListHdl			SystemTempMemHList;		

extern short			kernelMode;
extern char				*kernelstack;
extern unsigned long	kernelstackbase;
extern short			kernelbusy;

extern Boolean			In32BitMode;
extern unsigned long	initLocationZero;
extern Ptr				initMemTop;
extern Ptr				initFSQueueHook;

extern Ptr				initCurStackBase;

extern Handle			appleMenuDefProcRsrc;

extern Ptr				switchTabPtr;
extern u_size			lmemToolDataSize;

extern Boolean			IsOldDebugger;
extern Ptr				oldExceptionVectors[];

extern Ptr				debugEntryRoutine;
extern Ptr				debugExitRoutine;
extern Ptr				debugToAppRoutine;
extern char				debugControlKeyCode;
extern short			debugKeyTryCount;

extern OSErr			ppcInitErr;
extern MFmsgBlkPtr		pSystemQuitAllMsg;

extern Boolean			allocateGMBlkInSysHeap;
extern Boolean			Colorized;
extern Boolean			MachineHasFPU;
extern Boolean			MachineHasMacPlusKbd;

extern unsigned short	fakeKeyCount;
extern FakeKeyHandle	fakeKeyHandle;
extern unsigned short	dontGetFakeKey;

extern PEntryPtr		tsmLayerOwner;
extern Boolean			routeEvent;

extern Boolean			skiaExists;												// <36>

/************************************************************************************
 * 					OSDispatch dispatch table
 ************************************************************************************/

/* Entry to record a single OSDispatch dispatch table entry */
typedef struct	OSDispatchEntry {
	void	(*dispatchAddr)();
	short	dispatchDepth;
	short	dispatchRetSize;
}	OSDispatchEntry;

/* Dispatch table itself */
extern struct OSDispatchEntry OSDispatchTable[];

/* Dispatch numbers for specific routines */
#define _IDRequestScratchSwitchTask			(0x0000)
#define _IDGetActiveTaskInfo				(0x0001)
#define _IDGetSwitchInfo					(0x0002)
#define _IDCancelSwitch						(0x0003)
#define GETLAYERID 							(0x0004)
#define LCALCVISID							(0x0005)
#define LCVBEHINDID							(0x0006)
#define LPAINTBEHINDID						(0x0007)
#define LCLIPABOVEID						(0x0008)
#define LPAINTONEID							(0x0009)
#define LAYERINSERTID						(0x000A)
#define LAYERDELETEID						(0x000B)
#define MAKETOPLAYERID						(0x000C)
#define LAYERCLOSEID						(0x000D)
#define LAYERINITID							(0x000E)
#define LAYERFINDID							(0x000F)	
#define	KERNELLAUNCHID						(0x0010)
#define	TWFGSWITCHID						(0x0011)
#define GETNEXTHIGHLEVELEVENTID				(0x0012)
#define TWGETPIDID							(0x0013)
#define NEWDESKTOPLAYEROWNERID				(0x0014)
#define TEMPMAXMEMID						(0x0015)
#define TEMPTOPMEMID						(0x0016)
#define TWGETPROCINFOID						(0x0017)
#define TEMPFREEMEMID						(0x0018)
#define DISABLESWITCHINGID					(0x0019)
#define ENABLESWITCHINGID					(0x001A)
#define TWPOSTPSEUDOID						(0x001B)
#define SLEEPPROCESSID						(0x001C)
#define TEMPNEWHANDLEID						(0x001D)
#define TEMPHLOCKID							(0x001E)
#define TEMPHUNLOCKID						(0x001F)
#define TEMPDISPOSEHANDLEID					(0x0020)
#define LNEWWINDOWID						(0x0021)
#define LCLOSEWINDOWID						(0x0022)
#define LGETAUXWINID						(0x0023)
#define TWPOSTALLPSEUDOID					(0x0024)
#define LCOLORINVALRECTID					(0x0025)
#define SETSTDFILEDIRID						(0x0026)
#define GETPRTYPESTRINGSID					(0x0027)
#define SETPRTYPESTRINGSID					(0x0028)
#define BACKGROUNDNOTIFYID					(0x0029)
#define	LSHOWHIDEID							(0x002A)
#define	LGETNEXTLAYER						(0x002B)
#define	GETPROCESSTRAPADDRESSID				(0x002C)
#define SETPROCESSTRAPADDRESSID				(0x002D)
#define	READWRITEPROCESSMEMORYID			(0x002E)
#define	READWRITEPROCESSFPUREGSID			(0x002F)
#define	REGISTERDEBUGGERID					(0x0030)
#define ADDAPPLEMENUITEMID					(0x0031)
#define DELETEAPPLEMENUITEMID				(0x0032)
#define	ACCEPTHIGHLEVELEVENTID				(0x0033)
#define POSTHIGHLEVELEVENTID				(0x0034)
#define GETSERIALNUMBERFROMPORTNAMESID		(0x0035)
#define LAUNCHDESKACCESSORYID				(0x0036)
#define GETCURRENTPROCESSID					(0x0037)
#define GETNEXTPROCESSID					(0x0038)
#define GETPROCESSFROMLAYERID				(0x0039)
#define GETPROCESSINFORMATIONID				(0x003A)
#define SETFRONTPROCESSID					(0x003B)
#define WAKEUPPROCESSID						(0x003C)
#define SAMEPROCESSID						(0x003D)
#define GETAUXMENUITEMID					(0x003E)
#define GETSYSTEMCLIENTPROCESSID			(0x003F)
#define BEGINSYSTEMMODEID					(0x0040)
#define ENDSYSTEMMODEID						(0x0041)
#define REQUESTVOLUMENOTIFICATIONID			(0x0042)
#define DECLINEVOLUMENOTIFICATIONID			(0x0043)
#define KILLPROCESSID						(0x0044)
#define GETSPECIFICHIGHLEVELEVENTID			(0x0045)
#define GETPORTNAMEFROMPSNID				(0x0046)
#define SYSZONEFLOATSIZESID					(0x0047)
#define NEWFLOATLAYERID						(0x0048)
#define DISPOSEFLOATLAYERID					(0x0049)
#define	POSTFAKEKEYDOWNID					(0x004A)
#define	POSTTEXTID							(0x004B)
#define INLINEAWAREID						(0x004C)
#define NEWTSMLAYEROWNERID					(0x004D)
#define ROUTEEVENTTOSWMID					(0x004E)
#define PENTRYFROMPROCESSSERIALNUMBERID		(0x004F)

#ifdef MODSQUAD
#define ACCEPTDRAGID						(0x0050)
#endif

/* Stack depth (number of bytes of parameters) for specific routines */
#define REQUESTSCRATCHSWITCHDEPTH			((2*2)+(2*4))
#define GETACTIVETASKINFODEPTH				((1*2)+(1*4))
#define GETSWITCHINFODEPTH					((0*2)+(1*4))
#define CANCELSWITCHDEPTH					((0*2)+(0*4))
#define GETLAYERDEPTH 						(0*4)
#define LCALCVISDEPTH						(2*4)
#define LCVBEHINDDEPTH						(3*4)
#define LPAINTBEHINDDEPTH					(3*4)
#define LCLIPABOVEDEPTH						(2*4)
#define LPAINTONEDEPTH						(3*4)
#define LAYERINSERTDEPTH					(2*4)
#define LAYERDELETEDEPTH					(1*4)
#define MAKETOPLAYERDEPTH					(1*4)
#define LAYERCLOSEDEPTH						(1*4)
#define LAYERINITDEPTH						(0*4)
#define LAYERFINDDEPTH						(1*4)
#define KERNELLAUNCHDEPTH					(1*4)
#define TWFGSWITCHDEPTH						(1*2)
#define GETNEXTHIGHLEVELEVENTDEPTH			((2*2)+(1*4))
#define TWGETPIDDEPTH						(0)
#define NEWDESKTOPLAYEROWNERDEPTH			(2*4)
#define TEMPMAXMEMDEPTH						(4)
#define TEMPTOPMEMDEPTH						(0)
#define TWGETPROCINFODEPTH					((1*2)+(1*4))
#define TEMPFREEMEMDEPTH					(0)
#define DISABLESWITCHINGDEPTH				(0*4)
#define ENABLESWITCHINGDEPTH				(0*4)
#define TWPOSTPSEUDODEPTH					(2+4+2)
#define SLEEPPROCESSDEPTH					(4)
#define TEMPNEWHANDLEDEPTH					(2*4)
#define TEMPHLOCKDEPTH						(2*4)
#define TEMPHUNLOCKDEPTH					(2*4)
#define TEMPDISPOSEHANDLEDEPTH				(2*4)
#define LNEWWINDOWDEPTH						(1*4)
#define LCLOSEWINDOWDEPTH					(1*4)
#define LGETAUXWINDEPTH						(3*4)
#define TWPOSTALLPSEUDODEPTH				(3*4+2)
#define LCOLORINVALRECTDEPTH				(4+2+2)
#define SETSTDFILEDIRDEPTH					(2+2+4)
#define GETPRTYPESTRINGSDEPTH				(4)
#define SETPRTYPESTRINGSDEPTH				(6)
#define BACKGROUNDNOTIFYDEPTH				(4)
#define	LSHOWHIDEDEPTH						(4+2)
#define	LGETNEXTLAYERDEPTH					(4+2)
#define	GETPROCESSTRAPADDRESSDEPTH			(4+4+2+2)
#define SETPROCESSTRAPADDRESSDEPTH			(4+4+2+2)
#define	READWRITEPROCESSMEMORYDEPTH			(4+4+4+4+2)
#define	READWRITEPROCESSFPUREGSDEPTH		(4+4+2)
#define	REGISTERDEBUGGERDEPTH				(4+4+4+4+2)
#define ADDAPPLEMENUITEMDEPTH				(4+2+4+2+4)
#define DELETEAPPLEMENUITEMDEPTH			(4)
#define ACCEPTHIGHLEVELEVENTDEPTH			(4*4)
#define POSTHIGHLEVELEVENTDEPTH				(6*4)
#define GETSERIALNUMBERFROMPORTNAMEDEPTH	(2*4)
#define LAUNCHDESKACCESSORYDEPTH			(2*4)
#define GETCURRENTPROCESSDEPTH				(1*4)
#define GETNEXTPROCESSDEPTH					(1*4)
#define GETPROCESSFROMLAYERDEPTH			(2*4)
#define GETPROCESSINFORMATIONDEPTH			(2*4)
#define SETFRONTPROCESSDEPTH				(1*4)
#define WAKEUPPROCESSDEPTH					(1*4)
#define SAMEPROCESSDEPTH					(2*4 + 1*2)
#define GETAUXMENUITEMDEPTH					(4*4)
#define GETSYSTEMCLIENTPROCESSDEPTH			(1*4)
#define BEGINSYSTEMMODEDEPTH				(2*4)
#define ENDSYSTEMMODEDEPTH					(2*4)
#define REQUESTVOLUMENOTIFICATIONDEPTH		(2*4)
#define DECLINEVOLUMENOTIFICATIONDEPTH		(2*4)
#define KILLPROCESSDEPTH					(1*4)
#define GETSPECIFICHIGHLEVELEVENTDEPTH		(2*4+1*2)
#define GETPORTNAMEFROMPSNDEPTH				(2*4)
#define SYSZONEFLOATSIZESDEPTH				(2*4)
#define NEWFLOATLAYERDEPTH					(4)
#define DISPOSEFLOATLAYERDEPTH				(4)
#define	POSTFAKEKEYDOWNDEPTH				(4)
#define	POSTTEXTDEPTH						(4+2)
#define INLINEAWAREDEPTH					(4+2)
#define NEWTSMLAYEROWNERDEPTH				(4)
#define ROUTEEVENTTOSWMDEPTH				(2)
#define PENTRYFROMPROCESSSERIALNUMBERDEPTH	(4)

#ifdef MODSQUAD
#define ACCEPTDRAGDEPTH						(0)
#endif

/* Size of return value for specific routines */
#define REQUESTSCRATCHSWITCHRETSIZE			(0)
#define GETACTIVETASKINFORETSIZE			(2)
#define GETSWITCHINFORETSIZE				(0)
#define CANCELSWITCHRETSIZE					(0)
#define GETLAYERRETSIZE 					(1*4)
#define LCALCVISRETSIZE						(0*4)
#define LCVBEHINDRETSIZE					(0*4)
#define LPAINTBEHINDRETSIZE					(0*4)
#define LCLIPABOVERETSIZE					(0*4)
#define LPAINTONERETSIZE					(0*4)
#define LAYERINSERTRETSIZE					(0*4)
#define LAYERDELETERETSIZE					(0*4)
#define MAKETOPLAYERRETSIZE					(0*4)
#define LAYERCLOSERETSIZE					(0*4)
#define LAYERINITRETSIZE					(0*4)
#define LAYERFINDRETSIZE					(1*4)

#define KERNELLAUNCHRETSIZE					(2)
#define TWFGSWITCHRETSIZE					(4)
#define GETNEXTHIGHLEVELEVENTRETSIZE		(2)
#define TWGETPIDRETSIZE						(2)
#define NEWDESKTOPLAYEROWNERRETSIZE			(2)
#define TEMPMAXMEMSIZE						(4)
#define TEMPTOPMEMSIZE						(4)
#define TWGETPROCINFOSIZE					(4)
#define TEMPFREEMEMSIZE						(4)
#define DISABLESWITCHINGSIZE				(0)
#define ENABLESWITCHINGSIZE					(0)
#define TWPOSTPSEUDORETSIZE					(0)
#define SLEEPPROCESSRETSIZE					(0)
#define TEMPNEWHANDLESIZE					(4)
#define TEMPHLOCKSIZE						(0)
#define TEMPHUNLOCKSIZE						(0)
#define TEMPDISPOSEHANDLESIZE				(0)
#define LNEWWINDOWRETSIZE					(0)
#define LCLOSEWINDOWRETSIZE					(0)
#define LGETAUXWINRETSIZE					(2)
#define TWPOSTALLPSEUDORETSIZE				(2)
#define LCOLORINVALRECTRETSIZE				(0)
#define SETSTDFILEDIRRETSIZE				(0)
#define GETPRTYPESTRINGSRETSIZE				(2)
#define SETPRTYPESTRINGSRETSIZE				(0)
#define BACKGROUNDNOTIFYRETSIZE				(0)
#define	LSHOWHIDERETSIZE					(0)
#define	LGETNEXTLAYERRETSIZE				(4)
#define	GETPROCESSTRAPADDRESSRETSIZE		(2)
#define SETPROCESSTRAPADDRESSRETSIZE		(2)
#define	READWRITEPROCESSMEMORYRETSIZE		(2)
#define	READWRITEPROCESSFPUREGSRETSIZE		(2)
#define	REGISTERDEBUGGERRETSIZE				(2)
#define ADDAPPLEMENUITEMRETSIZE				(2)
#define DELETEAPPLEMENUITEMRETSIZE			(2)
#define ACCEPTHIGHLEVELEVENTRETSIZE			(2)
#define POSTHIGHLEVELEVENTRETSIZE			(2)
#define GETSERIALNUMBERFROMPORTNAMERETSIZE 	(2)
#define LAUNCHDESKACCESSORYSIZE				(2)
#define GETCURRENTPROCESSRETSIZE			(2)
#define GETNEXTPROCESSRETSIZE				(2)
#define GETPROCESSFROMLAYERRETSIZE			(2)
#define GETPROCESSINFORMATIONRETSIZE		(2)
#define SETFRONTPROCESSRETSIZE				(2)
#define WAKEUPPROCESSRETSIZE				(2)
#define SAMEPROCESSRETSIZE					(2)
#define GETAUXMENUITEMRETSIZE				(2)
#define GETSYSTEMCLIENTPROCESSRETSIZE		(2)
#define BEGINSYSTEMMODERETSIZE				(2)
#define ENDSYSTEMMODERETSIZE				(2)
#define REQUESTVOLUMENOTIFICATIONRETSIZE	(2)
#define DECLINEVOLUMENOTIFICATIONRETSIZE	(2)
#define KILLPROCESSRETSIZE					(2)
#define GETSPECIFICHIGHLEVELEVENTRETSIZE	(2)
#define GETPORTNAMEFROMPSNRETSIZE			(2)
#define SYSZONEFLOATSIZESRETSIZE			(2)
#define NEWFLOATLAYERRETSIZE				(2)
#define DISPOSEFLOATLAYERRETSIZE			(2)
#define	POSTFAKEKEYDOWNSIZE					(2)
#define	POSTTEXTSIZE						(2)
#define	INLINEAWARESIZE						(2)
#define NEWTSMLAYEROWNERSIZE				(2)
#define ROUTEEVENTTOSWMSIZE					(2)
#define PENTRYFROMPROCESSSERIALNUMBERSIZE	(4)

#ifdef MODSQUAD
#define ACCEPTDRAGSIZE						(2)
#endif

/************************************************************************************
 * 								Patch Table
 ************************************************************************************/

/* Indices into the saved trap array (patchtrap).
 * ***NOTE:  Mark all new traps (i.e. the ones you don't expect to already
 * exist with ISNEWTRAP in the oldtrap field in each entry of the patchtraps[] array.
 */
#define ISOLDTRAP	((Ptr)(0))
#define ISNEWTRAP	((Ptr)(-1))	/* Specially defined (by us) traps */
 
#define FIRST_TRAP			(0)

/* NOTE:  Changes here need to also happen to tables in Data.c and Data.a */
/* First define all traps that are in ROM 78 only */
#define FIRST_ROM78_TRAP	(FIRST_TRAP+0)
#define HANDTOHAND			(FIRST_ROM78_TRAP+0)
#define LAST_ROM78_TRAP		(HANDTOHAND)

/* Now define all ROM 75 traps */
#define FIRST_ROM75_TRAP	(LAST_ROM78_TRAP+1)
#define	ADDRESMENU			(FIRST_ROM75_TRAP+0)
#define CHECKLOAD			(FIRST_ROM75_TRAP+1)
#define CLOSE				(FIRST_ROM75_TRAP+2)
#define DISPOSHANDLE		(FIRST_ROM75_TRAP+3)
#define DRAWMENUBAR			(FIRST_ROM75_TRAP+4)
#define EJECT				(FIRST_ROM75_TRAP+5)
#define EVENTAVAIL			(FIRST_ROM75_TRAP+6)
#define EXITTOSHELL			(FIRST_ROM75_TRAP+7)
#define GETNAMEDRESOURCE	(FIRST_ROM75_TRAP+8)
#define GETNEXTEVENT		(FIRST_ROM75_TRAP+9)
#define GETRESATTRS			(FIRST_ROM75_TRAP+10)
#define GETRESOURCE			(FIRST_ROM75_TRAP+11)
#define GETSCRAP			(FIRST_ROM75_TRAP+12)
#define	GETVOL				(FIRST_ROM75_TRAP+13)
#define	GETVOLINFO			(FIRST_ROM75_TRAP+14)
#define HANDLEZONE			(FIRST_ROM75_TRAP+15)
#define HFSDISPATCH			(FIRST_ROM75_TRAP+16)
#define INITWINDOWS			(FIRST_ROM75_TRAP+17)
#define LAUNCH				(FIRST_ROM75_TRAP+18)
#define	MENUKEY				(FIRST_ROM75_TRAP+19)
#define	MENUSELECT			(FIRST_ROM75_TRAP+20)
#define MOUNTVOL			(FIRST_ROM75_TRAP+21)
#define NEWHANDLE			(FIRST_ROM75_TRAP+22)
#define OFFLINE				(FIRST_ROM75_TRAP+23)
#define OPEN				(FIRST_ROM75_TRAP+24)
#define	OPENDESKACC			(FIRST_ROM75_TRAP+25)
#define OPENRF				(FIRST_ROM75_TRAP+26)
#define OSDISPATCH			(FIRST_ROM75_TRAP+27)
#define OSRESERVED			(FIRST_ROM75_TRAP+28)
#define PACK3				(FIRST_ROM75_TRAP+29)
#define POSTEVENT			(FIRST_ROM75_TRAP+30)
#define PUTSCRAP			(FIRST_ROM75_TRAP+31)
#define RDRVRINSTALL		(FIRST_ROM75_TRAP+32)
#define REALLOCHANDLE		(FIRST_ROM75_TRAP+33)
#define RECOVERHANDLE		(FIRST_ROM75_TRAP+34)
#define RELEASERESOURCE		(FIRST_ROM75_TRAP+35)
#define SETCURSOR			(FIRST_ROM75_TRAP+36)
#define SETGROWZONE			(FIRST_ROM75_TRAP+37)
#define SIZERSRC			(FIRST_ROM75_TRAP+38)
#define STILLDOWN			(FIRST_ROM75_TRAP+39)
#define	SYSEDIT				(FIRST_ROM75_TRAP+40)
#define SYSERROR			(FIRST_ROM75_TRAP+41)
#define SYSTEMCLICK			(FIRST_ROM75_TRAP+42)
#define	SYSTEMEVENT			(FIRST_ROM75_TRAP+43)
#define	SYSTEMMENU			(FIRST_ROM75_TRAP+44)
#define SYSTEMTASK			(FIRST_ROM75_TRAP+45)
#define UNMOUNTVOL			(FIRST_ROM75_TRAP+46)
#define UPDATERESFILE		(FIRST_ROM75_TRAP+47)
#define WAITMOUSEUP			(FIRST_ROM75_TRAP+48)
#define WAITNEXTEVENT		(FIRST_ROM75_TRAP+49)
#define	WAKEUP				(FIRST_ROM75_TRAP+50)
#define ZEROSCRAP			(FIRST_ROM75_TRAP+51)
#define LAST_ROM75_TRAP		(ZEROSCRAP)

/* Patch SetTrapAddress last so we don't use patched version to make our patches */
#define SETTRAPADDRESS		(LAST_ROM75_TRAP+1)

/* Entry to record a single trap patch */
typedef struct PatchEntry {
	unsigned short	trapnum;
	unsigned short	traptype;
	pascal void		(*newtrap)();
	pascal void		(*oldtrap)();
} PatchEntry, *PatchEntryPtr, **PatchEntryHdl;

/* The trap array itself */
extern PatchEntry	patchtraps[];

#ifdef DEBUG
/* This is debugging stuff; it don't have to be pretty (and it ain't) */
void DebuggerMsg(char *, int, char *);
#define NO_MESSAGE_ID nil
#define ASSERTION_FAILURE_ID ((Ptr) -1)
#define db() {														\
	unsigned long	dbOldA5;										\
	dbOldA5 = ProcessMgrA5SimpleSetup();							\
	DebuggerMsg(__FILE__, __LINE__, NO_MESSAGE_ID);					\
	A5SimpleRestore(dbOldA5); }
#define dbmsg(msg) {												\
	unsigned long	dbOldA5;										\
	dbOldA5 = ProcessMgrA5SimpleSetup();							\
	DebuggerMsg(__FILE__, __LINE__, msg);							\
	A5SimpleRestore(dbOldA5); }
#define assert(assertion)											\
	{if (!(assertion)) {											\
		unsigned long	dbOldA5;									\
		dbOldA5 = ProcessMgrA5SimpleSetup();						\
		DebuggerMsg(__FILE__, __LINE__, ASSERTION_FAILURE_ID);		\
		A5SimpleRestore(dbOldA5); } }
#else
#define db()
#define dbmsg(msg)
#define assert(assertion)
#endif DEBUG

#endif __DATA__
