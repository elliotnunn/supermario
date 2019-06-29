/*EASE$$$ READ ONLY COPY of file “task.h”
** 1.2	CCH 11/ 9/1988 Fixed header.
** 1.1	CCH 11/ 9/1988 Fixed headers and removed projector resource.
** 1.0	CCH 11/ 9/1988 Adding to EASE.
** END EASE MODIFICATION HISTORY */
/*
 *	Task.h -- Interface to the Twitcher tasking routines.
 *
 *	C Interface to the Macintosh Libraries
 *	Copyright Apple Computer,Inc. 1985,1986
 *	All rights reserved.
 *
 * (C) Copyright Apple Computer, Inc.		1986
 *
 * Written by Erich Ringewald
 *
 * 26 Nov 86		New Today.
 *
 */


#ifndef __KERNEL__
#include <kernel.h>
#endif

#ifndef __TASK__
#define __TASK__

/*__________________________________________________________________*/

/*							Task Manager 							*/

/*__________________________________________________________________*/


struct procinfo {
	short			p_state;			/* process state */
	short			p_mypid;			/* the process id */
	long			p_type;				/* type of task (usually APPL) */
	long			p_signature;		/* signature of task */
	long			p_version;			/* version of task */
	THz				p_zone;				/* pointer to minor zone */
	unsigned long	p_taskmode;			/* tasks created mode */
	Boolean			p_needsuspresevts;	/* whether or not the process expects suspend and resume events */
	Boolean			p_back;				/* can accept background time */
	Boolean			p_activateonresume;	/* whether the app will activate and deactivate on susppend/resume */
	short 			p_dad;				/* process id of my dad */
	unsigned long	p_size;				/* size of his world */
	unsigned long	p_ssize;			/* size of his stack */
	unsigned long	p_slices;			/* # of times process has CPU */
	
	unsigned long	p_freemem;			/* amount of free memory in heap */
	Str31			p_name;				/* name of backing app */
	short			p_vrefnum			/* vrefnum of app res file as passed to _TWLaunch */
};

/* Constants for printer type calls */

#define PRTYPE_CHANGESTATUS_SAME		(-1)
#define PRTYPE_CHANGESTATUS_DISABLED	(0)
#define PRTYPE_CHANGESTATUS_ENABLED		(1)

/*
 * mask constants for the longword bitmap of taskmode given to the
 * launch command.
 */
 
#define MULTILAUNCH				(0x00010000)
#define	SAVESCREEN				(0x00008000)
#define	GETSUSPRESEVENTS		(0x00004000)
#define OPTIONKEYDISABLE		(0x00002000)
#define GETBACKTIME				(0x00001000)
#define ACTIVATEONRESUME		(0x00000800)
#define ONLYBACKGROUND			(0x00000400)


/*__________________________________________________________________*/

/*						Task call MACRO definitions					*/

/*__________________________________________________________________*/

#define TWLaunch(name, size, vrefnum, taskmode, stack) \
_TWLaunch((name), (size), (vrefnum), (taskmode), (stack), TWLAUNCHID)

#define TWFGSwitch(pid) \
_TWFGSwitch((pid), TWFGSWITCHID)

#define TWKill(pid) \
_TWKill((pid), TWKILLID)

#define TWGetPID() \
_TWGetPID(TWGETPIDID)

#define TWSetDeskRgn(deskrgn) \
_TWSetDeskRgn((deskrgn), TWSETDESKRGNID)

#define TWMaxMem(grow) \
_TWMaxMem((grow), TWMAXMEMID)

#define TWMemTop() \
_TWMemTop(TWMEMTOPID)

#define TWGetProcInfo(pid, inforec) \
_TWGetProcInfo((pid), (inforec), TWGETPROCINFOID)

#define TWFreeMem() \
_TWFreeMem(TWFREEMEMID)

#define TWDontSwitch() \
_TWDontSwitch(TWDONTSWITCHID)

#define TWDoSwitch() \
_TWDoSwitch(TWDOSWITCHID)

#define TWPostPseudoEvent(pid, theevent, priority) \
_TWPostPseudoEvent((pid), (theevent), (priority), TWPOSTPSEUDOID)

#define TWPostAllPseudoEvent(signature, type, theevent, priority) \
_TWPostAllPseudoEvent((signature), (type), (theevent), (priority), TWPOSTALLPSEUDOID)

#define TWSleep(time) \
_TWSleep((time), TWSLEEPID)

#define TWTempNewHandle(logicalSize, resultCode) \
 _TWTempNewHandle((logicalSize), (resultCode), TWTEMPNEWHANDLEID)

#define TWTempHLock(h, resultCode) \
 _TWTempHLock((h), (resultCode), TWTEMPHLOCKID)

#define TWTempHUnLock(h, resultCode) \
 _TWTempHUnLock((h), (resultCode), TWTEMPHUNLOCKID)

#define TWTempDisposHandle(h, resultCode) \
 _TWTempDisposHandle((h), (resultCode), TWTEMPDISPOSHANDLEID)

#define TWSetStdFileDir(pid, sfSaveDisk, curDirID) \
_TWSetStdFileDir(pid, sfSaveDisk, curDirID, TWSETSTDFILEDIRID)

#define TWGetPrTypeStrings(pLocalStrPtr, pGlobalStrPtr) \
_TWGetPrTypeStrings(pLocalStrPtr, pGlobalStrPtr, TWGETPRTYPESTRINGSID)
	
#define TWSetPrTypeStrings(localStrPtr, globalStrPtr, enablePrTypeChanges) \
_TWSetPrTypeStrings(localStrPtr, globalStrPtr, enablePrTypeChanges, TWSETPRTYPESTRINGSID)

#define TWBackgroundNotify(msgStringPtr) \
_TWBackgroundNotify(msgStringPtr, TWBACKGROUNDNOTIFYID)


pascal short _TWLaunch(name, size, vrefnum, taskmode, stack, dispatch)
	char			*name;
	unsigned long	size, stack;
	short			vrefnum;
	unsigned long	taskmode;
	short 			dispatch;
	extern KERNELDISPATCH;

pascal long _TWFGSwitch(pid, dispatch)
	short	pid;
	short	dispatch;
	extern KERNELDISPATCH;

pascal long _TWKill(pid, dispatch)
	short	pid;
	short	dispatch;
	extern KERNELDISPATCH;
	
pascal short _TWGetPID(dispatch)
	short	dispatch;
	extern KERNELDISPATCH;

pascal void _TWSetDeskRgn(deskrgn, dispatch)
	RgnHandle	deskrgn;
	short		dispatch;
	extern KERNELDISPATCH;
	
pascal long _TWMaxMem(grow, dispatch)
	long		*grow;
	short		dispatch;
	extern KERNELDISPATCH;
	
pascal Ptr _TWMemTop(dispatch)
	short		dispatch;
	extern KERNELDISPATCH;
	
pascal long _TWGetProcInfo(pid, inforec, dispatch)
	short			pid;
	struct procinfo	*inforec;
	short			dispatch;
	extern 			KERNELDISPATCH;
	
pascal Size _TWFreeMem(dispatch)
	short			dispatch;
	extern 			KERNELDISPATCH;

pascal void _TWDontSwitch(dispatch)
	short			dispatch;
	extern 			KERNELDISPATCH;
		
pascal void _TWDoSwitch(dispatch)
	short			dispatch;
	extern 			KERNELDISPATCH;
		
pascal void _TWPostPseudoEvent(pid, theevent, priority, dispatch)
	short			pid;
	Ptr				theevent;
	unsigned short	priority;
	short			dispatch;
	extern 			KERNELDISPATCH;
		
pascal short
_TWPostAllPseudoEvent(signature, tyype, pEvent, priority, dispatch)
	long				signature, tyype;
	EventRecord			*pEvent;
	unsigned short		priority;
	short			dispatch;
	extern 			KERNELDISPATCH;
	
pascal void _TWSleep(time, dispatch)
	unsigned long	time;
	short			dispatch;
	extern 			KERNELDISPATCH;
		
 pascal Handle _TWTempNewHandle(logicalSize, resultCode, dispatch)
	unsigned long	logicalSize;
	short 			*resultCode;
	short 			dispatch;
	extern KERNELDISPATCH;

 pascal void _TWTempHLock(h, resultCode, dispatch)
	Handle			h;
	short 			*resultCode;
	short 			dispatch;
	extern KERNELDISPATCH;

 pascal void _TWTempHUnLock(h, resultCode, dispatch)
	Handle			h;
	short 			*resultCode;
	short 			dispatch;
	extern KERNELDISPATCH;

 pascal void _TWTempDisposHandle(h, resultCode, dispatch)
	Handle			h;
	short 			*resultCode;
	short 			dispatch;
	extern KERNELDISPATCH;

 pascal void _TWSetStdFileDir(pid, sfSaveDisk, curDirID, dispatch)
	short			pid;
	short			sfSaveDisk;
	unsigned long	curDirID;
	short 			dispatch;
	extern KERNELDISPATCH;
		
pascal char _TWGetPrTypeStrings(pLocalStrPtr, pGlobalStrPtr, dispatch)
	StringPtr		*pLocalStrPtr, *pGlobalStrPtr;
	short 			dispatch;
	extern KERNELDISPATCH;
	
pascal void _TWSetPrTypeStrings(localStrPtr, globalStrPtr, enablePrTypeChanges, dispatch)
	StringPtr		localStrPtr, globalStrPtr;
	char			enablePrTypeChanges;
	short 			dispatch;
	extern KERNELDISPATCH;

pascal void _TWBackgroundNotify(msgStringPtr, dispatch)
	StringPtr		msgStringPtr;
	short 			dispatch;
	extern KERNELDISPATCH;

#endif
