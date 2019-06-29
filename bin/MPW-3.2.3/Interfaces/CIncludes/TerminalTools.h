/*
	TerminalTool.h
	C Interface to the Terminal Manager Tools
	Copyright © Apple Computer, Inc.	1988-90
	All rights reserved
	
*/

#ifndef __TERMINALTOOLS__
#define __TERMINALTOOLS__

#ifndef __DIALOGS__
#include <dialogs.h>
#endif	__DIALOGS__

#ifndef __TERMINALS__
#include <Terminals.h>
#endif	__TERMINALS__

#define	tdefType			'tdef'
#define	tvalType			'tval'
#define	tsetType			'tset'
#define	tlocType			'tloc'
#define	tscrType			'tscr'
#define	tbndType			'tbnd'
#define	tverType			'vers'

#define	tmInitMsg 			0	
#define	tmDisposeMsg 		1
#define	tmSuspendMsg		2
#define	tmResumeMsg			3
#define	tmMenuMsg			4
#define	tmEventMsg			5
#define tmActivateMsg 		6
#define tmDeactivateMsg 	7
#define	tmGetErrorStringMsg	8


#define tmIdleMsg 			50
#define	tmResetMsg 			51

#define	tmKeyMsg			100
#define	tmStreamMsg			101
#define	tmResizeMsg			102 
#define	tmUpdateMsg			103 
#define	tmClickMsg			104
#define	tmGetSelectionMsg	105
#define	tmSetSelectionMsg	106
#define	tmScrollMsg			107 
#define	tmClearMsg			108
#define	tmGetLineMsg		109
#define	tmPaintMsg			110
#define	tmCursorMsg			111
#define	tmGetEnvironsMsg	112	
#define	tmDoTermKeyMsg		113	
#define tmCountTermKeysMsg	114
#define	tmGetIndTermKeyMsg	115

#define	tmValidateMsg		0
#define	tmDefaultMsg		1

#define	tmSpreflightMsg		0
#define	tmSsetupMsg			1
#define tmSitemMsg			2
#define	tmSfilterMsg		3
#define	tmScleanupMsg		4

#define	tmMgetMsg			0
#define	tmMsetMsg			1

#define tmL2English			0
#define tmL2Intl			1

struct TMSearchBlock {
		StringHandle	theString;
		Rect			where;
		TMSearchTypes	searchType;
		ProcPtr			callBack;
		short			refnum;
		struct TMSearchBlock	*next;
};

#ifndef __cplusplus
typedef struct TMSearchBlock TMSearchBlock;
#endif

typedef TMSearchBlock *TMSearchBlockPtr;

struct TMSetupStruct {
		DialogPtr		theDialog;
		short			count;
		Ptr				theConfig;
		short			procID;	
};

#ifndef __cplusplus
typedef struct TMSetupStruct TMSetupStruct;
#endif

typedef TMSetupStruct *TMSetupPtr;


#endif __TERMINALTOOLS__