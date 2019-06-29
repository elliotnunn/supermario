/*
	File:		LLQueue.h

	Contains:	Process Mgr link list queueing structures

	Written by:	Jay Moreland

	Copyright:	© 1986-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <0>	 x/xx/89	JWM		New Today.

*/

#ifndef __LLQUEUE__
#define __LLQUEUE__

#ifndef __TYPES__
#include	<types.h>
#endif

#define justPtrs 1

typedef struct LINKTYPE	*LINKPTR;
typedef struct LINKTYPE {
	LINKPTR		next;
	LINKPTR		previous;
	char *item;
	} LINKTYPE;
	
typedef struct LINKLIST {
	LINKPTR		head;
	LINKPTR		tail;
	LINKPTR		clp;
	short		listInUse;
	int 		listlength;
	int 		itemlength;
	int 		(*match)();
	} LINKLIST;
	
void		llsetMatch(ProcPtr numatch);
Boolean		llcheck(char *lookfor);
void		llsetlist(struct LINKLIST *new_list);
void		llsetsize(int size);
void		llinit(char *newitem);
void		llhead(void);
void		lltail(void);
Boolean		llnext(void);
Boolean		llprevious(void);
void		llretrieve(char **newitem);
void		lladd(char *newitem);
void		lladdhead(char *newitem);
void		lldelete(void);
int			ll_length(void);

#define qitemsize		llsetsize
#define qset			llsetlist
#define qlength			ll_length

#define QUEUE LINKLIST

void q_push(char *newitem);
void q_pop(char **old_item);

#endif __LLQUEUE__
