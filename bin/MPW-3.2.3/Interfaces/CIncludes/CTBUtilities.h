/*
	File:		CTBUtilities.h

	Copyright:	© 1990 by Apple Computer, Inc.
				All rights reserved.

*/

#ifndef __CTBUTILITIES__
#define __CTBUTILITIES__

#ifndef	__MEMORY__
#include	<Memory.h>
#endif

#ifndef __PACKAGES__
#include	<Packages.h>
#endif

#ifndef __APPLETALK__
#include <AppleTalk.h>
#endif

/*	version of Comm Toolbox Utilities	*/
#define	curCTBUVersion			2

/*	Error codes/types	*/
typedef		OSErr	CTBUErr;

#define		ctbuGenericError	(-1)
#define		ctbuNoErr			0

/*	Dialog Item List Manipulation Constants	*/
typedef short DITLMethod;
enum {
	overlayDITL,
	appendDITLRight,
	appendDITLBottom
};

/*	Choose return codes	*/
enum {
	chooseDisaster = -2,
	chooseFailed,
	chooseAborted,
	chooseOKMinor,
	chooseOKMajor,
	chooseCancel
};

/*	NuLookup return codes	*/
enum {
	nlOk,
	nlCancel,
	nlEject
};
	
/*	Name filter proc return codes	*/
enum {
	nameInclude = 1,
	nameDisable,
	nameReject
};
	
/*	Zone filter proc return codes	*/
enum {
	zoneInclude = 1,
	zoneDisable,
	zoneReject
};
	
/*	Values for hookProc items		*/
#define		hookOK				1
#define		hookCancel 			2
#define		hookOutline			3
#define		hookTitle			4
#define		hookItemList		5
#define		hookZoneTitle		6
#define		hookZoneList		7
#define		hookLine			8
#define		hookVersion			9
#define		hookReserved1		10
#define		hookReserved2 		11
#define		hookReserved3		12
#define		hookReserved4		13
/*	"virtual" hookProc items	*/
#define		hookNull			100
#define		hookItemRefresh		101
#define		hookZoneRefresh		102
#define		hookEject			103
#define		hookPreflight		104
#define		hookPostflight		105
#define		hookKeyBase			1000


/*	NuLookup structures/constants	*/
struct NLTypeEntry {
	Handle hIcon;
	Str32 typeStr;
};

typedef struct NLTypeEntry NLTypeEntry;

typedef NLTypeEntry NLType[4];

struct NBPReply {
	EntityName	theEntity;
	AddrBlock	theAddr;
};

typedef struct NBPReply NBPReply;

typedef pascal short (*NameFilterProcPtr)(EntityName theEntity);
typedef pascal short (*ZoneFilterProcPtr)(Str32 theZone);

typedef NameFilterProcPtr nameFilterProcPtr;
typedef ZoneFilterProcPtr zoneFilterProcPtr;

#ifdef __cplusplus
extern "C" {
#endif
pascal CTBUErr	InitCTBUtilities(void);
pascal short 	CTBGetCTBVersion(void);

pascal void		AppendDITL(DialogPtr theDialog, Handle theHandle, DITLMethod method);
pascal short		CountDITL(DialogPtr theDialog);
pascal void		ShortenDITL(DialogPtr theDialog, short numberItems);

pascal short 	NuLookup(Point where, ConstStr255Param prompt, short numTypes, 
							  NLType typeList, NameFilterProcPtr nameFilter, 
							  ZoneFilterProcPtr zoneFilter, DlgHookProcPtr hookProc,
							  NBPReply *theReply);

pascal short 	NuPLookup(Point where, ConstStr255Param prompt, short numTypes, 
							  NLType typeList, NameFilterProcPtr nameFilter, 
							  ZoneFilterProcPtr zoneFilter, DlgHookProcPtr hookProc, 
							  long userData, short dialogID,
							  ModalFilterProcPtr filterProc, 
							  NBPReply *theReply);
							  
pascal short 	StandardNBP(Point where, ConstStr255Param prompt, short numTypes, 
							  NLType typeList, NameFilterProcPtr nameFilter, 
							  ZoneFilterProcPtr zoneFilter, DlgHookProcPtr hookProc,
							  NBPReply *theReply);

pascal short 	CustomNBP(Point where, ConstStr255Param prompt, short numTypes, 
							  NLType typeList, NameFilterProcPtr nameFilter, 
							  ZoneFilterProcPtr zoneFilter, DlgHookProcPtr hookProc, 
							  long userData, short dialogID,
							  ModalFilterProcPtr filterProc, 
							  NBPReply *theReply);
							  
							  
#ifdef __cplusplus
}
#endif

#endif __CTBUTILITIES__