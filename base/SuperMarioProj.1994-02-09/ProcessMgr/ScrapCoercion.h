/*
	File:		ScrapCoercion.h

	Contains:	Process Mgr scrap coercion module header file.  Scrap coercion is the
				means by which the clipboard (scrap) is kept intact when switching
				foreground applications.

	Written by:	Guido Goldinini

	Copyright:	© 1986-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <3>	12/17/90	DFH		Rename CheckForCutCopy to CheckForCutCopyMenuKey.
		 <2>	12/15/90	DFH		Fix prototype for src_scrap_setup.  Added pScrapOwner.
		 <0>	 x/xx/86	PYG		New Today.

*/ 
 
#ifndef __SCRAPCOERCION__
#define __SCRAPCOERCION__

#ifndef __EVENTS__
#include <Events.h>
#endif

/* Information we keep about the SCRAPHANDLE */
typedef struct MyScrapInfo
	{
	long		size;					/* size of scrap in bytes */
	Handle		memHandle;				/* handle to scrap in mem or 0 */
	Boolean		orphanedScrap;			/* whether to dump memHandle after coercion */
	StringPtr	fileName;				/* file name if on disk */
	PEntryPtr	pScrapOwner;			/* process from whence this info sprang */
	} MyScrapInfo;

/* Values for global CoercionState */
#define CS_DONE							(0)
#define CS_GENERATE_DEACTIVATE1			(1)
#define CS_GENERATE_DEACTIVATE2			(2)
#define CS_GENERATE_NULLS_THEN_CMD_V	(3)
#define CS_MOVE_SCRAP					(4)
#define CS_GENERATE_NULLS_THEN_CMD_C	(5)
#define CS_CLICK_IN_WINDOW				(6)
#define CS_TAKE_DOWN_WINDOW				(7)
#define CS_PRESUSPEND_DEACTIVATE		(8)
#define CS_SUSPEND						(9)
#define CS_RESUME						(10)
#define CS_EXITING						(11)
#define CS_ENTERING						(12)
#define CS_INIT							(13)
#define CS_EATMOUSE						(14)

/* Older applications (i.e. ones that have no SIZE resource, or whose SIZE resource does
 * not say acceptSuspendResumeEvents and doesActivateOnFGSwitch), must be fooled into
 * a) activating or deactivating their front window, and b) posting their scrap.  We do
 * it by pretending like a DA came up and the user pasted (suspend) or copied (resume).
 * NOTE: Somewhat strangely, this fake DA window stays in the application's WINDOWLIST
 * all the while the application is in the background.  Have not figured out whether
 * this serves a purpose other than to prevent an additional NewWindow when bringing the
 * application to the front again.
 */
#define SCRAP_COERCION_FAKE_DA_WINDOW_ID ((short) 0x8001)

/* Limits to state change efforts in scrap coercion */
#define	CLICK_DA_WIND_TIMEOUT_INIT		(4)
#define EAT_MOUSE_TIMEOUT				(5)
#define	NULL_TIMEOUT_CUTOFF				(4)
#define NULL_TIMEOUT_INIT				(7)

/* Values for message field in suspend or resume event records */
#define SUSPRES_MESSAGE					(suspendResumeMessage << 24)
#define SUSPEND_MESSAGE					(SUSPRES_MESSAGE+convertClipboardFlag)
#define RESUME_MESSAGE					(SUSPRES_MESSAGE+resumeFlag)

/* Internal function prototypes */
void		AddToFrontProcessQueue(PEntryPtr);
PEntryPtr	PopFrontProcessQueue(void);
void		RemoveFromFrontProcessQueue(PEntryPtr);
void		StartForegroundSwitch(PEntryPtr);
short		Coercion_State_Engine(unsigned short, EventRecord *, Ptr, Boolean);
void		src_scrap_setup(void);
void		PutUpFakeWindow(void);
void		TakeDownFakeWindow(void);
void		CheckForCutCopyMenuKey(unsigned char, short *);
void		CheckForCutCopySysEditCmd(short, short *);
void		OrphanScrapIntoPartition(void);

#endif __SCRAPCOERCION__
