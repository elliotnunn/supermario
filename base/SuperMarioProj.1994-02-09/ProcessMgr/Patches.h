/*
	File:		Patches.h

	Contains:	Glue for traps and for calling through to original trap address.

	Written by:	Phil Goldman

	Copyright:	© 1986-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <5>	 1/28/91	DFH		JSM,#81425:Remove PopUpMenuSelect prototype.
		 <3>	  1/8/91	DFH		(rdd) DebugToAppRoutine_ptr is now pascal.
		 <0>	 x/xx/86	PYG		New Today.

*/ 

#ifndef __PATCHES__
#define __PATCHES__

#ifndef __DATA__
#include	"Data.h"
#endif __DATA__

#define CALL_FNC_PTR(fncType,fncPtr,fncArgs)	(*((fncType) fncPtr))fncArgs

/* For 2nd arg to save_or_restore_dispatch() */
#define SAVE_DISPATCH 0
#define RESTORE_DISPATCH 1

/* Old routines from trap table */
typedef OSErr
(*Setgrowzone_ptr) (ProcPtr newGzProc);

typedef pascal void
(*Addresmenu_ptr)(Handle menuHdl,long resType);

#define FAKE_OPENDA_RETVAL ((short) -80)
typedef pascal short
(*Opendeskacc_ptr)(Ptr stringPtrDAName);

typedef pascal long
(*Menuselect_ptr)(Point startPt);

typedef pascal long
(*Menukey_ptr)(short ch);

typedef pascal short
(*Sysedit_ptr)(short editCmd);

typedef pascal short
(*Getnextevent_ptr)(short eventmask, Ptr theevent);

typedef pascal short
(*Eventavail_ptr)(short eventmask,Ptr theevent);

/* ...and when it's either of the two above */
typedef pascal short
(*Eventcommon_ptr)(short eventmask, Ptr theevent);

typedef pascal short
(*Stilldown_ptr)(void);

typedef pascal Handle
(*Getresource_ptr)(long thetype, short id);

typedef pascal Handle
(*Getnamedresource_ptr)(long thetype,Ptr name);

typedef pascal long
(*Sizersrc_ptr)(Handle resHdl);

typedef pascal short
(*Getresattrs_ptr)(Handle rsrcHdl);

pascal void
settrapaddress_glue(Ptr trapAddr, short trapNum, short setTrapAddressNum, Ptr oldtrap)
extern;

typedef pascal void
(*Systemtask_ptr)(void);

typedef pascal short
(*Systemevent_ptr)(Ptr theevent);

typedef pascal void
(*Systemclick_ptr)(Ptr theevent, Ptr windowPtr);

typedef pascal void
(*Systemmenu_ptr)(long menuResult);

typedef pascal void
(*Enableitem_ptr)(Handle menuHdl, short item);

typedef pascal void
(*ShowHide_ptr)(Ptr window, Boolean showFlag);

typedef pascal void
(*Dragwindow_ptr)(Ptr window, Point startPt, Rect *pBoundsRect);

typedef pascal void
(*Updateresfile_ptr)(short refNum);

typedef pascal void
(*Releaseresource_ptr)(Handle resHdl);

typedef pascal void
(*Setcursor_ptr)(Ptr cursorDataPtr);

typedef pascal void
(*PaintOne_ptr)(Ptr window,Handle clobberedRgn);

typedef pascal void
(*DebugToAppRoutine_ptr)(void);

short
MyRelString(StringPtr pStr1, StringPtr pStr2);

/* Function prototypes */
void					InstallPatches(void);
void					InitApplPatchTable(PatchEntryHdl *, short *);
void					UpdatePatchRegistry(PEntryPtr, Ptr, TrapType, unsigned short);
void					AddApplPatch(PatchEntryHdl, Ptr, Ptr, unsigned short, unsigned short);
Ptr						GetRealTrapAddr(unsigned short, unsigned short);
PatchEntryPtr			GetApplPatch(PatchEntryHdl, unsigned short, unsigned short);
#define RemoveApplPatch(pPatch)		pPatch->traptype = NullTrapType
void					SetOldCheckLoad(Ptr);

#endif __PATCHES__
