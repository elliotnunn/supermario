/*
	File:		Terminals.h

	Contains:	C Interface to the Terminal Manager

	Copyright:	© 1988-1990 by Apple Computer, Inc.
				All rights reserved.

*/


#ifndef __TERMINALS__
#define __TERMINALS__

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif	__DIALOGS__

#ifndef __CTBUTILITIES__
#include <CTBUtilities.h>
#endif	__CTBUTILITIES__

#ifndef __CONNECTIONS__
#include <Connections.h>
#endif	__CONNECTIONS__

/* current Terminal Manager version 	*/
#define		curTMVersion	2

/* current Terminal Manager Environment Record version 	*/

#define		curTermEnvRecVers	0

/* error codes	*/
typedef	OSErr TMErr;

#define	tmGenericError		(-1)
#define	tmNoErr				0
#define	tmNotSent			1
#define	tmEnvironsChanged	2
#define	tmNotSupported		7
#define	tmNoTools			8

/* TMFlags 	*/
typedef long TMFlags;

enum {
	tmInvisible 		= 1 << 0,
	tmSaveBeforeClear 	= 1 << 1,
	tmNoMenus 			= 1 << 2,
	tmAutoScroll 		= 1 << 3
};

/* TMSelTypes & TMSearchTypes	*/
typedef short TMSelTypes;
typedef short TMSearchTypes;

enum {
	selTextNormal 		= 1 << 0,
	selTextBoxed 		= 1 << 1,
	selGraphicsMarquee 	= 1 << 2,
	selGraphicsLasso 	= 1 << 3,
	
	tmSearchNoDiacrit 	= 1 << 8,			/*	These are only for TMSearchTypes	*/
	tmSearchNoCase 		= 1 << 9
};

/*	TMCursorTypes 	*/
typedef short TMCursorTypes;
enum {
	cursorText = 1,
	cursorGraphics = 2
};

/*	TMTermTypes 	*/
typedef short TMTermTypes;
enum {
	tmTextTerminal 		= 1 << 0,
	tmGraphicsTerminal 	= 1 << 1
};

struct	TermDataBlock {
	TMTermTypes		flags;	
	Handle			theData;
	Handle			auxData;
	long			reserved;
};

typedef struct TermDataBlock TermDataBlock;
typedef TermDataBlock *TermDataBlockPtr, **TermDataBlockH;

struct TermEnvironRec {
	short			version;
	TMTermTypes		termType;
	short			textRows;
	short			textCols;
	Point			cellSize;
	Rect			graphicSize;
	Point			slop;
	Rect			auxSpace;
};

typedef struct TermEnvironRec TermEnvironRec;
typedef TermEnvironRec *TermEnvironPtr;
	
union TMSelection {
	Rect			selRect;
	RgnHandle		selRgnHandle;
};

typedef union TMSelection TMSelection;

/* TMChoose data structures */
struct TMChooseRec {
	long	reserved;
	long	msg;
	ProcPtr	idleProc;
	ProcPtr	filterProc;
	Str63	newTool;
	Ptr		newConfig;
	ProcPtr	eventProc;
};

typedef struct TMChooseRec TMChooseRec;
typedef TMChooseRec *TMChooseRecPtr;

/* TMChooseRec messages */
#define	tmChooseNochange		1
#define	tmChooseBackdoor		2

struct TermRecord {
		short			procID;
		
		TMFlags			flags;
		TMErr			errCode;
		
		long			refCon;
		long			userData;
		
		ProcPtr			defProc;
		
		Ptr				config;
		Ptr				oldConfig;
		
		ProcPtr			environsProc;
		long			reserved1;
		long			reserved2;
	
		Ptr				tmPrivate;
						
		ProcPtr			sendProc;
		ProcPtr			breakProc;
		ProcPtr			cacheProc;
		ProcPtr			clikLoop;
		
		WindowPtr		owner;
		Rect			termRect;
		Rect			viewRect;
		Rect			visRect;
		
		long			lastIdle;
		
		TMSelection		selection;
		TMSelTypes		selType;
		
		long			mluField;
};

typedef struct TermRecord TermRecord;
typedef TermRecord **TermHandle, *TermPtr;

#ifdef __cplusplus
extern "C" {
#endif
pascal TMErr 	InitTM(void); 
pascal Handle 	TMGetVersion(TermHandle hTerm);
pascal short 	TMGetTMVersion(void);

pascal TermHandle TMNew(const Rect *termRect, const Rect *viewRect, TMFlags flags, 
							short procID, WindowPtr owner, ProcPtr sendProc, 
							ProcPtr cacheProc, ProcPtr breakProc, ProcPtr clikLoop, ProcPtr environsProc,
							long refCon, long userData);

pascal void 	TMDispose(TermHandle hTerm);

pascal void 	TMKey(TermHandle hTerm, const EventRecord *theEvent);
pascal void 	TMUpdate(TermHandle hTerm, RgnHandle visRgn);
pascal void 	TMPaint(TermHandle hTerm, const TermDataBlock *theTermData, const Rect *theRect);
pascal void 	TMActivate(TermHandle hTerm, Boolean activate);
pascal void 	TMResume(TermHandle hTerm, Boolean resume);
pascal void 	TMClick(TermHandle hTerm, const EventRecord *theEvent);
pascal void 	TMIdle(TermHandle hTerm);

pascal long 	TMStream(TermHandle hTerm, void *theBuffer, long theLength, CMFlags flags);
pascal Boolean 	TMMenu(TermHandle hTerm, short menuID, short item);

pascal void 	TMReset(TermHandle hTerm);
pascal void 	TMClear(TermHandle hTerm);

pascal void		TMResize(TermHandle hTerm, const Rect *newViewRect);

pascal long 	TMGetSelect(TermHandle hTerm, Handle theData, ResType *theType);
pascal void 	TMGetLine(TermHandle hTerm, short lineNo, TermDataBlock *theTermData);
pascal void 	TMSetSelection(TermHandle hTerm, const TMSelection *theSelection, TMSelTypes selType);

pascal void 	TMScroll(TermHandle hTerm, short dh, short dv);


pascal Boolean 	TMValidate(TermHandle hTerm);
pascal void 		TMDefault(Ptr *theConfig, short procID, Boolean allocate);

pascal Handle 	TMSetupPreflight(short procID, long *magicCookie);
pascal void 	TMSetupSetup(short procID, const void *theConfig, short count, DialogPtr theDialog,
					long *magicCookie);
pascal Boolean 	TMSetupFilter(short procID, const void *theConfig, short count, DialogPtr theDialog,
					EventRecord *theEvent, short *theItem, long *magicCookie);
pascal void 	TMSetupItem(short procID, const void *theConfig, short count, DialogPtr theDialog,
					short *theItem, long *magicCookie);
pascal void 	TMSetupCleanup(short procID, const void *theConfig, short count, DialogPtr theDialog,
					long *magicCookie);
pascal void 	TMSetupXCleanup(short procID, const void *theConfig, short count, DialogPtr theDialog,
					Boolean OKed, long *magicCookie );
pascal void 	TMSetupPostflight(short procID);

pascal Ptr 		TMGetConfig(TermHandle hTerm);
pascal short 	TMSetConfig(TermHandle hTerm, const void *thePtr);

pascal OSErr 	TMIntlToEnglish(TermHandle hTerm, const void *inputPtr, Ptr *outputPtr, short language);
pascal OSErr 	TMEnglishToIntl(TermHandle hTerm, const void *inputPtr, Ptr *outputPtr, short language);

pascal void 	TMGetToolName(short id, Str255 name);
pascal short 	TMGetProcID(ConstStr255Param name);

pascal void 	TMSetRefCon(TermHandle hTerm, long refCon);
pascal long 	TMGetRefCon(TermHandle hTerm);

pascal void 	TMSetUserData(TermHandle hTerm, long userData);
pascal long 	TMGetUserData(TermHandle hTerm);

pascal short 	TMAddSearch(TermHandle hTerm, ConstStr255Param theString, const Rect *where, 
							TMSearchTypes searchType, ProcPtr callBack);
pascal void 	TMRemoveSearch(TermHandle hTerm, short refnum);
pascal void 	TMClearSearch(TermHandle hTerm);

pascal Point 	TMGetCursor(TermHandle hTerm, TMCursorTypes cursType);

pascal TMErr 	TMGetTermEnvirons(TermHandle hTerm, TermEnvironRec *theEnvirons);

pascal short 	TMChoose(TermHandle *hTerm, Point where, ProcPtr idleProc);
pascal short 	TMPChoose(TermHandle *hTerm, Point where, TMChooseRec *cRec);
pascal void 	TMEvent(TermHandle hTerm, const EventRecord *theEvent);

pascal Boolean 	TMDoTermKey(TermHandle hTerm, ConstStr255Param theKey);
pascal short 	TMCountTermKeys(TermHandle hTerm);
pascal void 	TMGetIndTermKey(TermHandle hTerm, short id, Str255 theKey);

pascal void		TMGetErrorString(TermHandle hTerm, short id, Str255 errMsg);

#ifdef __cplusplus
}
#endif

#endif __TERMINALS__
