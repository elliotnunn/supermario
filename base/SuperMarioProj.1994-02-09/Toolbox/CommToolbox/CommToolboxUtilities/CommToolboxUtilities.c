/*
	File:		CommToolboxUtilities.c

	Contains:	Skeleton of the CommToolbox Utilities

	Written by:	Byron Han

	Copyright:	© 1989-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<10>	  2/4/91	kaz		jng, #82013: Fix loading of 'proc' resources to check for System
									heap space & purged resources
		 <9>	 1/25/91	stb		JDR: change LookupReply to NBPReply
		 <8>	 10/5/90	kaz		Removed _CTBKeystrokeFilter as unused <jng>
		 <7>	 9/14/90	kaz		Loading DITL 'proc' correctly to avoid heap fragmentation.
		 <6>	 7/10/90	dba		add __ to distinguish routines from glue
		 <5>	 3/27/90	BBH		StandardNBP entry returns short not long.
		 <4>	 3/15/90	BBH		fix comments
		 <3>	 3/15/90	BBH		Fixed interfaces
		 <2>	 3/15/90	BBH		Changed resource management of DITL and NLUP.  do not use
									CRMresource calls - simply do a GetResource.  Remove error
									checking for resources missing.  Also, do not release resource.
									They should be marked purgeable for best results.
		 <1>	 3/14/90	BBH		first checked in

	To Do:

	Pre-BBS Modification History:

		7/25/89		CL		Added spcifiedStr to CTBKeyStrokekFilter
*/

#include	<Memory.h>
#include	<Resources.h>
#include	<Dialogs.h>
#include	<Events.h>

#include	<AppleTalk.h>

#include	"CommResources.h"
#include	"CTBUtilities.h"
#include	"CommToolboxPriv.h"

pascal CTBUErr	__InitCTBUtilities()
{
	return(noErr);
}

pascal long DITLEntry(DialogPtr pDialog, Handle hDITL, short appendMethod, 
														short nItems, short msg)
{
	long	rval = 0;
	Handle	theProc;
	pascal 	long (**callProc) (DialogPtr, Handle, short, short, short);
	
	// Resource is SysHeap (i.e. no _MoveHHi), Purgeable
	// it's also originally locked to make it load low in the sysHeap
	
	theProc = GetResource(CTB_DITLPackType, CTB_DITLPackID);
	if (theProc != nil) {
		LoadResource(theProc);			// in case it was purged
		
		if (*theProc != nil) {
			HLock(theProc);
			(long) callProc = (long) theProc;
			rval = (**callProc) (pDialog, hDITL, appendMethod, nItems, msg);		
			HUnlock(theProc);
		}
	}
	return(rval);
}

pascal 	short	__NuPLookup(Point where, const Str255 prompt, short numTypes, 
							const NLType typeList, nameFilterProcPtr nameFilter,
							zoneFilterProcPtr zoneFilter, DlgHookProcPtr hookProc, 
							long userData, short dialogID, ModalFilterProcPtr filterProc,
							NBPReply *theReply)
{
	short		rval = nlEject;
	Handle		theProc;
	pascal 		short (**callProc) (Point, const Str255, short, const NLType,
							nameFilterProcPtr, zoneFilterProcPtr,
							DlgHookProcPtr, long, short, ModalFilterProcPtr,
							NBPReply *);
	
	// Resource is SysHeap (i.e. no _MoveHHi), Purgeable
	// Originally marked locked to make it load low in the SysHeap
	
	theProc = GetResource(CTB_NuLookupType, CTB_NuLookupID);
	if (theProc != nil) {
		LoadResource(theProc);
		
		if (*theProc != nil) {
			HLock(theProc);
			(long) callProc = (long) theProc;
			rval = (**callProc) (where, prompt, numTypes, typeList, nameFilter,
						zoneFilter, hookProc, userData, dialogID, filterProc, theReply);
			HUnlock(theProc);
		}
	}
	return(rval);
}
