/*
	File:		CommResourceMgrExtensions.c

	Contains:	Extensions to the CommResource Manager

	Written by:	Byron Han

	Copyright:	© 1988-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 8/12/93	kc		<mb>: I deleted the "TheFuture" conditionals from all of the
									CommToolbox files as per Mike Bells instructions. I also deleted
									some old code that was compiled under the "CubeE
		 <9>	  2/7/92	BH		changed define for MANAGERCODE to be TRUE or FALSE to allow
									CommResourceMgrExtensions.c to properly conditionalize for
									TheFuture and for CubeE and for building the glue file
		 <8>	 10/4/91	JSM		Change PsychoticFarmerOrLater conditionals to TheFuture.
		 <7>	 10/2/91	DTY		Conditionalise <4> and <5> out of CubeE.
		 <6>	 7/10/91	BH		remove #define garbage to avoid duplicate wierd symbols in
									object file - now use Lib in SysObj.make
		 <5>	  7/9/91	BH		add __CRMGetToolNamedResource
		 <4>	  7/3/91	BH		add __CRMParseCAPSResource
		 <3>	  7/2/91	BH		prefix routines with __ to avoid glue conflicts
		 <2>	 3/16/90	BBH		fix interface files
		 <1>	 3/14/90	BBH		first checked in to BBS

*/

//	<9>	changed to do TRUE and FALSE for the define
#ifndef __MANAGERCODE__
#define	__MANAGERCODE__		FALSE

#include <Resources.h>			// gimme the resource manager please
#include <Memory.h>				// gimme the memory   manager please

#include <CommResources.h>
#include <CommToolboxPriv.h>

//#define __CRMGetToolResource 		CRM_PROC1
//#define __CRMReleaseToolResource 	CRM_PROC2
//#define __CRMParseCAPSResource	 	CRM_PROC3
//#define __CRMGetToolNamedResource	CRM_PROC4
#endif __MANAGERCODE__


// Get a single resource from the specified tool's resource fork.
// Returns NIL if not found.
pascal Handle 
__CRMGetToolResource (procID, type, id)
	short procID;
	ResType type;
	short id;
{
	Handle h;
	
	CRMToolContext oldContext;
	short saveResFile = CurResFile ();

	CRMToolPreflight (procID, &oldContext);		// CRM private routine to add tool to resource chain

	UseResFile (procID);						// Only look in the tool's resource fork

	h = CRMGet1Resource (type, id);				// Get the specified resource from the tool

	CRMToolPostflight (&oldContext);			// Undo CRMToolPreflight ()

	UseResFile (saveResFile);					// Restore saved current resource file refnum
	
	return (h); 
}

pascal Handle 
__CRMGetToolNamedResource (procID, type, name)
	short procID;
	ResType type;
	ConstStr255Param name;
{
	Handle h;
	
	CRMToolContext oldContext;
	short saveResFile = CurResFile ();

	CRMToolPreflight (procID, &oldContext);		// CRM private routine to add tool to resource chain

	UseResFile (procID);						// Only look in the tool's resource fork

	h = CRMGet1NamedResource (type, name);		// Get the specified resource from the tool

	CRMToolPostflight (&oldContext);			// Undo CRMToolPreflight ()

	UseResFile (saveResFile);					// Restore saved current resource file refnum
	
	return (h);
}

// Release a resource previously acquired via CRMGetToolResource
pascal void 
__CRMReleaseToolResource (procID, theHandle)
	short procID;
	Handle theHandle;
{
	CRMToolContext oldContext;
	short saveResFile = CurResFile ();

	CRMToolPreflight (procID, &oldContext);		// CRM private routine to add tool to resource chain
	UseResFile (procID);						// Only look in the tool's resource fork
	CRMReleaseResource (theHandle);				// Release user's resource
	CRMToolPostflight (&oldContext);			// Undo CRMToolPreflight ()
	UseResFile (saveResFile);					// Restore saved current resource file refnum
}


typedef struct {
	ResType		token;
	long		value;
} TokenValuePair;

pascal OSErr
__CRMParseCAPSResource(Handle theHandle, ResType selector, long *value) 
{
	OSErr			rval 		= crmGenericError;
	short			nEntries 	= ** (short **) theHandle;
	short			i;
	TokenValuePair	*pInternal 	= (TokenValuePair *) (*theHandle + sizeof(short));
	
	*value = 0;
	for (i = 0; i < nEntries; i++, pInternal++) {
		if ( (long)pInternal->token == (long)selector) {
			*value = pInternal->value;
			return crmNoErr;
		}
	}
	return rval;	
}
