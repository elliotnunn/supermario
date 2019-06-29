/*
	File:		HList.h

	Contains:	Interface definition for routines in HList.c

	Written by:	Phil Goldman

	Copyright:	© 1988-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <0>	 5/12/88	PYG		New Today.

*/ 

#ifndef		__HLIST__
#define		__HLIST__

/* HList types */
typedef unsigned long	HListElem, *HListPtr, **HListHdl;
typedef void			(HListElemFnc)(HListElem hListElem);

/* Macros that should be inline functions */
#define CreateHList(initialSlopSlots)	((HListHdl)NewHandleClear((initialSlopSlots) * sizeof(HListElem)))
#define HListExists(hListHdl)			((hListHdl) != nil)
#define HListCount(hListHdl)			(((u_size)(GetHandleSize(hListHdl)))/sizeof(HListElem))

/* Function headers */
OSErr		AddToHList(HListHdl, HListElem);
OSErr		RemoveFromHList(HListHdl, HListElem);
void		DestroyHList(HListHdl);
HListElem	RecoverHListElem(HListHdl, Ptr);
void		ApplyToHListElems(HListHdl, HListElemFnc *);
OSErr		InHList(HListHdl, HListElem);
HListElem	GetFirstHListElem(HListHdl);

#endif		__HLIST__
