/*
	File:		Queue.c

	Contains:	Process Mgr queueing structures

	Written by:	Erich Ringewald

	Copyright:	© 1987-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
			 <0>	 3/13/87	ELR		New Today.

*/ 

#include <types.h>
#include <memory.h>

#include "Glue.h"
#include "Data.h"

#define IS ==
#define AND &&
#define ISNT !=

#define NULL 0L

static struct QUEUE *q;

void q_push(char *newitem)
{
	if (ll_length() IS 0)
		llinit(newitem);
	else
		lladdhead(newitem);
}

void q_pop(char **old_item)
{
	lltail();
	llretrieve(old_item);
	lldelete();
}

LINKLIST *list;

#if justPtrs
#define moveitem(A,B) B=A
#else
#define moveitem(A,B) memcpy(B,A,list->itemlength)
#endif
/* use this macro as moveitem(from, to) */

void llsetMatch(ProcPtr numatch) /* Set matching function */
{
	list->match = numatch;
}

/* set clp to desired link.  return Ture if found. False otherwise */
Boolean llcheck(char *lookfor)
{
	if (ll_length() IS 0)
		return(false);
	for (;;) {
		if ((Boolean)(*list->match)(list->clp->item, lookfor))
			return(true);
		else
			if (!llnext())
				return(false);
	}
}

void llsetlist(LINKLIST *new_list) /* Set this module to work with a new list */
{
	list = new_list;
}

void llsetsize(int size) /* Set the storage requirements for the list */
{
	list->itemlength = size;
}

#if !justPtrs
static LINKTYPE *llcrlink() /* Allocate storage for a link */
{
	LINKTYPE *link;
	
	link = (LINKTYPE *)malloc(sizeof(LINKTYPE));
	link->item = malloc(list->itemlength);
	return(link);
}
#endif

void llinit(char *newitem) /* Initialize the structure. */
{
#if justPtrs
	list->head = list->tail = list->clp = (LINKTYPE *)newitem;
#else
	list->head = list->tail = list->clp = llcrlink();
#endif
	list->clp->next = list->clp->previous = NULL;
	moveitem(newitem, list->clp->item);
	list->listlength = 1;
}

void llhead(void) /* Set the CLP to the head of the list */
{
	list->clp = list->head;
}

void lltail(void) /* Set the CLP to the head of the list */
{
	list->clp = list->tail;
}


/* Set the CLP to the next link, return FALSE if at end of list, 
	TRUE otherwise. */
Boolean llnext(void)
{
	if (list->clp->next IS NULL)
		return(false);
	else {
		list->clp = list->clp->next;
		return(true);
	}
}

/*
Set the CLP to the previous link, return FALSE if at head of list, 
TRUE otherwise. */
Boolean llprevious(void)
{
	if (list->clp->previous IS NULL)
		return(false);
	else {
		list->clp = list->clp->previous;
		return(true);
	}
}

#if justPtrs
void llretrieve(char **newitem)
{
	*newitem = list->clp->item;
}
#else
void llretrieve(char **newitem) /* Retrieve the item from the CLP link. */
{
	moveitem(list->clp->item, *newitem);
}
#endif

/*
Add a new link containing this item to the link following the CLP, 
and reset CLP to new link. */
void lladd(char *newitem)
{
	LINKTYPE *newlink;
	
/* Create new link. */

#if justPtrs
newlink = (LINKTYPE *)newitem;
#else
newlink = llcrlink();
#endif
moveitem(newitem, newlink->item);
list->listlength++;

/* Reset pointers. */

newlink->next = list->clp->next;
newlink->previous = list->clp;
if (list->tail IS list->clp)
	list->tail = newlink;
else
	list->clp->next->previous = newlink;
list->clp->next = newlink;
list->clp = newlink;
}

void lladdhead(char *newitem) /* Add a new head, reset CLP. */
{
	LINKTYPE *newlink;
	
	/* if empty, initialize list. */
	if (ll_length() IS 0) { 
		llinit(newitem);	/* calling lladdhead before initialing ll_length */
		return;
	}
	/* create new link */
#if justPtrs
	newlink = (LINKTYPE *)newitem;
#else
	newlink = llcrlink();
#endif
	moveitem(newitem, newlink->item);
	list->listlength++;
	
	/* Reset pointers. */
	
	newlink->previous = NULL;
	newlink->next = list->head;
	list->head->previous = newlink;
	list->clp = list->head = newlink;
}


void lldelete(void) /* Delete and free the CLP, reset CLP to head. */
{
	LINKTYPE *before, *after;
	
	/* Is this the only link? */
	
	assert(list->listlength ISNT 0);
	
	if (list->head IS list->clp AND list->tail IS list->clp) {
		list->head = list->tail = NULL;
	}
	/* Is this the head? */
	else if (list->head IS list->clp) {
		list->head = list->head->next;
		list->head->previous = NULL;
	}
	/* Is this the tail? */
	else if (list->tail IS list->clp) {
		list->tail = list->tail->previous;
		list->tail->next = NULL;
	}
	/* Otherwise, it must be inside the list. */
	else {
		before = list->clp->previous;
		after = list->clp->next;
		before->next = after;
		after->previous = before;
	}
	/* Delete CLP */
#if justPtrs
	list->clp->item = NULL;
	list->clp->next = NULL;
	list->clp->previous = NULL;
#else
	free(list->clp->item);
	free(list->clp);
#endif
	list->clp = list->head;
	list->listlength--;
}

int ll_length(void) /* return the length of the list */
{
	return(list->listlength);
}
