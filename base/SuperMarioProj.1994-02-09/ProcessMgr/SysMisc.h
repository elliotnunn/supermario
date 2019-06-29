/*
	File:		SysMisc.h

	Contains:	Miscellaneous Mac system nonsense that has no better home...

	Written by:	Phil Goldman and Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<13>	 8/26/92	DTY		Roll in latest changes for A/UX.
		<12>	 1/22/92	DTY		Take the synFOND field out of the synthetic strike table
									structure. We’re gauranteeing unique font IDs, so this extra
									differentiation won’t be necessary.
		<11>	11/15/91	DTY		Added synFOND to the SynEntry structure.  (We might need to
									store this information elsewhere if it turns out that developers
									are looking at the synthetic fonts.
		<10>	 9/16/91	DFH		Added GetCPUStatusRegister and SR definitions.
		 <9>	  4/1/91	DFH		(with John Iarocci) Merge in latest AUX support code. NOTE: No
									AUX support code is referenced unless the build defines the
									HAS_AUX_PROCESSMGR condition.
		 <8>	  3/1/91	DFH		KST,WS#DFH-910301a: Added xfiScriptCodeMask.
		 <7>	 1/21/91	DFH		(KSM) Removed system menu IDs, since they are now in
									MenuMgrPriv.a.
		 <6>	 1/15/91	DFH		(VL) Conditionalize out the AUX call macros.
		 <4>	12/10/90	DFH		Moved file attribute and finder flag equates from
									OSDispatchPrivate.h.
		 <3>	 12/5/90	DFH		Added #defines for prettier calls to AuxDispatch.  Removed
									NewFXInfo since FXInfo is now in Files.h.
		 <0>	10/25/86	PYG		New Today.

*/ 

#ifndef __SYSMISC__
#define __SYSMISC__

#include <Retrace.h>

/* Access to the CPU's status register (SR) */
#define kCPUSupervisorMode			(1<<13)
#define	kCPUMasterStack				(1<<12)
unsigned short GetCPUStatusRegister(void) = {0x40C0};

/* Definitions of the flags in the ioFlAttrib of a CInfoPBRec */
#define FILE_ATTRIB_ISLOCKED		(0x00)
#define FILE_ATTRIB_RFOPEN			(0x04)
#define FILE_ATTRIB_DFOPEN			(0x08)
#define FILE_ATTRIB_ISDIR			(0x10)
#define FILE_ATTRIB_ISBUSY			(0x80)
	
/* Definitions for the fdFlags field in the ioFndrInfo record of an FInfo structure */
#define FILE_FLAG_LOCKED			(1<<15)
#define FILE_FLAG_INVISIBLE			(1<<14)
#define FILE_FLAG_HASBUNDLE			(1<<13)
#define FILE_FLAG_SYSTEM			(1<<12)
#define FILE_FLAG_HAS_CUSTOM_ICON	(1<<10)
#define FILE_FLAG_CHANGED			(1<<9)
#define FILE_FLAG_INITED			(1<<8)
#define FILE_FLAG_MULTILAUNCH		(1<<6)
#define FILE_FLAG_BF_ALWAYS			(1<<5)
#define FILE_FLAG_BF_NEVER			(1<<4)
#define FILE_FLAG_BF_OWNAPPL		(1<<1)
#define FILE_FLAG_ONDESKTOP			(1<<0)

/* The icon family resource ID when FILE_FLAG_HAS_CUSTOM_ICON is on */
#define kCustomIconID				(-16455)

/* Flag in fdScript field of FXInfo (extended Finder file information). */
#define kScriptValidBit		(7)
#define	xfiScriptCodeMask	(0x7F)

#ifdef HAS_AUX_PROCESSMGR
/* Stuff that ought to be in Aux.h, whenever that gets moved to {CIncludes}. */
#define AUX_StartTimer()			(void) AUXDispatch(AUX_STARTTIMER, 0)
#define AUX_StopTimer()				(void) AUXDispatch(AUX_STOPTIMER, 0)
#define AUX_Highest()				(short) AUXDispatch(AUX_HIGHEST, 0)
#define AUX_SetAUXMask(newMask)		(Boolean) AUXDispatch(AUX_SETAUXMASK, (void *) newMask)
#define AUX_ForkExec(pParams)		AUXDispatch(AUX_FORKEXEC, pParams)
#define AUX_GetAnyEvent(pParams)	AUXDispatch(AUX_GETANYEVENT, pParams)
#define AUX_GetTask(pTaskId)		(void) AUXDispatch(AUX_GETTASK, pTaskId)
#define AUX_Kill(fatedTaskID)		(void) AUXDispatch(AUX_KILL, (void *) fatedTaskID)
#define AUX_Switch(taskID)			AUXDispatch(AUX_SWITCH, (void *) taskId)
#define AUX_SetTimeOut(timeout)		(void) AUXDispatch(AUX_SETTIMEOUT, (void *) timeout)
#define AUX_SetBounds(bounds)		(void) AUXDispatch(AUX_SETBOUNDS, bounds)
#define AUX_COFF_FSSpec(coffname)   AUXDispatch(AUX_COFFFSSPEC, (char *) coffname)
#define AUX_EnableCoffLaunch()		AUXDispatch(AUX_ENABLECOFFLAUNCH, 0)

#endif HAS_AUX_PROCESSMGR

/* Command characters understood by the MDEF */
#define noMarkSmallIconCmd		(0x1A)		/* small icon with no left margin */
#define shrunkenIconCmd			(0x1D)		/* shrink the icon to 16x16 */
#define smallIconCmd			(0x1E)		/* small icon */
#define largeIconCmd			(0x1F)		/* large icon */
#define familyIconCmd			(0x20)		/* icon suite/cache */

/* Internal menu manager structures we crawl */
typedef struct MenuPair
	{
	MenuHandle		menuHdl;
	short			leftEdge;
	} MenuPair;
	
typedef struct MenuList
	{
	unsigned short	lastMenu;
	short			lastRight;
	short			notUsed;
	MenuPair		menuPairs[];
	} MenuList, *MenuListPtr, **MenuListHandle;

typedef struct MenuItemAttributes {
	unsigned char	itemIcon;						/* icon resource number */	
	unsigned char	itemCmd;						/* key equivalent (& variant) */	
	unsigned char	itemMark;						/* mark character */	
	unsigned char	itemStyle;						/* text style */	
} MenuItemAttributes;

/* Menu Manager keeps track of enable/disabled state as bits in a long word, and one
 * flag (bit 0) is assigned for the menu itself.  This means that item numbered greater
 * than MAX_ENABLEFLAG_INDEX can't be tracked, so they are always enabled.
 */
#define MAX_ENABLEFLAG_INDEX	(sizeof(long) - 1)
	
/* Mungy way to walk a menu data structure */	
#define INC_MENU_ITEMPTR(pItem)	\
		pItem += Length(pItem) + 1 + sizeof(MenuItemAttributes);
#define SKIP_TO_MENU_ATTRIBUTES(pItem)				pItem += Length(pItem) + 1;
#define FROM_MENU_ATTRIBUTES_TO_NEXT_ITEM(pItem)	pItem += sizeof(MenuItemAttributes);
#define IS_NOT_LAST_MENUITEM(pItem)		(Length(pItem) != 0)
#define IS_LAST_MENUITEM(pItem)			(Length(pItem) == 0)

/* If we're using GetItem, all our IS_DASH_STRING calls are against
 * strings on the stack, which are naturally even-aligned.  If we look
 * directly into the menu structure, alignment is not guaranteed.
 * This requires a different, bigger, and slower check.
 */
#define IS_DASH_STRING(pStr) ( (Length(pStr) == 1) && (StringByte(pStr, 0) == '-') )

/* Macros to construct or examine a menu selection. A menu selection is a long
 * word whose high word is the menu ID, and whose low word is the item ID.
 */
#define BUILD_MENUSELECTION(menu, item)			((menu << 16) | item)
#define MENU_ID_OF_SELECTION(menuSelection)		(*((short *)&(menuSelection)))
#define ITEM_ID_OF_SELECTION(menuSelection)		((unsigned short)(menuSelection))
#define SMALLEST_VALID_MENULONG					0x00010000
#define VALID_MENUSELECTION(menuSelection)		((u_long) menuSelection >= SMALLEST_VALID_MENULONG)

/* Owned resource format (from IM I-109). */
#define OWNED_RESOURCE			(0xC000)
#define OWNED_BY_DRVR			(OWNED_RESOURCE | (0 <<11))
#define OWNED_BY_WDEF			(OWNED_RESOURCE | (1 <<11))
#define OWNED_BY_MDEF			(OWNED_RESOURCE | (2 <<11))
#define OWNED_BY_CDEF			(OWNED_RESOURCE | (3 <<11))
#define OWNED_BY_PDEF			(OWNED_RESOURCE | (4 <<11))
#define OWNED_BY_PACK			(OWNED_RESOURCE | (5 <<11))

#define MAX_OWNED_RESOURCE_ID	(63)

/* Tests for ROM versions */
#define IS_NUMAC_ROMS(romVersion) ((*((char *)&(romVersion)) & (char)0xC0) == 0)
#define IS_OLD_ROMS(romVersion) (*((char *)&(romVersion)) < 0)	

/* Flags in EXTENSIONSENABLEBYTE */
#define ExtensionsEnabledBit		(5)
/* 
 * Structures we don't know where else to put.
 */

/* Previously unused field in the WDPBRec */
#define ioWDCreated		filler1

/* Working directory structure from fsequ.a */
typedef struct WDCB
	{
	Ptr				WDVCBPtr;
	long			WDDirID; 
	long			WDCatHint;
	long			WDProcID;
	} WDCB, *WDCBPtr;

/* Definition of the APPPARMHANDLE (files to open and/or print) */
typedef struct AppParmRec {
	short				message;
	short				count;
	AppFile				appFiles[0];
} AppParmRec, *AppParmRecPtr, **AppParmRecHdl;

/* Launch-time parameters to application */
struct appparmarea {
	unsigned long		stdin;
	unsigned long		stdout;
	Handle				parmhandle;
	unsigned short		physexec;
};

/* SIZE resource */
typedef struct swParmDesc {
	unsigned short		flags;
	unsigned long		psize;
	unsigned long		msize;
} swParms, *swParmPtr, **swParmHdl;

/* The value of the DEFLTSTACK lomem on a MacPlus. MacPlus is used as a base machine
 * for default partition and stack sizes.
 */
#define MACPLUS_DEFLTSTACK	(0x2000)

/* Segment header information  */
typedef struct CodeEntry
	{
	short			pea;		/* This is the PEA instruction ($3F3C) */
#define	PEA			0x3F3C
	short			segID;		/* This is the segment (i.e. rsrc) id */
	short			LoadSeg;	/* This is the _LoadSeg ($A9F0) */
	} CodeEntry;

/* Jump table entry after CODE segment has been loaded in by LoadSeg */
typedef struct JTEntry {
	short			jmpOpCode;
	void			(*routinePC)();
} JTEntry;

#define CODE_ENTRY_UNLOADED(pCodeEntry)			((pCodeEntry)->pea == PEA)
#define	FNC_PTR_UNLOADED(pFnc)					(CODE_ENTRY_UNLOADED((CodeEntry *)(pFnc)))
#define	GET_SEGMENT_ID_FROM_FNC_PTR(pFnc)		(((CodeEntry *)pFnc)->segID)
#define	GET_SCOD_HDL_FROM_FNC_PTR(pFnc)			(GetResource(SYS_SEGMENT_TYPE, GET_SEGMENT_ID_FROM_FNC_PTR(pFnc)))
#define ROUTINE_ADDR(jta)						(((JTEntry *)jta)->routinePC)

/* The header of a CODE 0 resource looks like this... */
struct code0 {
	unsigned long	abovea5;
	unsigned long	belowa5;
	unsigned long	jtsize;
	unsigned long	jtoffset;
	unsigned char	jt[];
};

/* Header information for driver and DA code.  There is no hard and fast way to
 * distinguish a DA from a device driver just by looking in the unit table or in
 * this header.  By convention, though, device driver names start with a particular
 * character.  The character is a period ('.', ASCII $2E).
 */
 
typedef struct Driver
	{
	short			drvrFlags;			/* flags */
	short			drvrDelay;			/* ticks between periodic actions */
	short			drvrEMask;			/* DA event mask */
	short			drvrMenu;			/* menu ID of driver's menu */
	short			drvrOpen;			/* offset to open routine */
	short			drvrPrime;			/* offset to prime routine */
	short			drvrCtl;			/* offset to control routine */
	short			drvrStatus;			/* offset to status routine */
	short			drvrClose;			/* offset to close routine */
	char			drvrName[];			/* driver name (Pascal string) */
	} Driver, *DriverPtr, **DriverHandle;
	
#define DrvrNameFirstChar	'.'				/* distinction between drivers and DAs */			
#define DNeedLock			(1<<(8+6))		/* Driver will be locked when opened? */
#define DNeedTime			(1<<(8+5))		/* Does driver need periodic control calls? */
#define DNeedGoodbye		(1<<(8+4))		/* Does driver need a goodbye kiss? */
#define DStatEnable			(1<<(8+3))		/* Can driver respond to Status calls? */
#define DCtlEnable			(1<<(8+2))		/* Can driver respond to Control calls? */
#define DWritEnable			(1<<(8+1))		/* Can driver respond to Write calls? */
#define DReadEnable			(1<<(8+0))		/* Can driver respond to Read calls? */
#define DActive				(1<<7)			/* Is driver currently executing? */
#define DRamBased			(1<<6)			/* Is driver RAM-based? */
#define DOpened				(1<<5)			/* Is driver already open? */

#define WantsTime(pDCE)		\
		((pDCE->dCtlFlags & (DNeedTime | DCtlEnable | DOpened | DActive)) == (DNeedTime | DCtlEnable | DOpened))

#define FIRSTDREFNUM (-1)					/* Ref Num of first driver */
#define NOTDREFNUM (0)						/* Invalid ref num */
#define csCodeGoodBye ((short) -1)			/* GoodBye control code */

/* Synthetic strike descriptor */
typedef struct SynEntry
	{
	Handle				synStrikeHdl;
	short				synNFNT;
	short				synID;
	RGBColor			synFGColor, synBGColor;
	} SynEntry, *SynEntryPtr, **SynEntryHandle;

/* Count of color inverse tables */
#define NUMITABLES	(12)

/* Flags in ALARMSTATE lomem */
#define AlrmParity		(0x80)
#define AlrmBeeped		(0x40)
#define AlrmFlEnable	(0x01)

/* MacJmp bits */
#define DEBUGGER_RUNNING		(1<<7)
#define DEBUGGER_INITIALIZED	(1<<6)
#define DEBUGGER_INSTALLED		(1<<5)

#endif __SYSMISC__
