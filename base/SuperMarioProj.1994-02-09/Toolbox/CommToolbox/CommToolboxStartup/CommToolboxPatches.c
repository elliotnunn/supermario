/*
	File:		CommToolboxPatches.c

	Contains:	Linked Patches for CommToolbox

	Written by:	Byron Han, Jerry Godes

	Copyright:	© 1990-1991, 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/12/93	kc		<mb>: I deleted all of the "TheFuture" conditionals in all of
									the CommToolbox files per Mike Bell's instructions. I also had
									to delete some old code that was compiled under the "CubeE"
									conditional.
		<12>	 10/4/91	JSM		Change PsychoticFarmerOrLater to TheFuture.
		<11>	 10/2/91	DTY		Conditionalise <10> out of CubeE.
		<10>	 6/17/91	BH		fix problem in CleanupCommToolbox() which did not handle
									cleaning up after multiply instantiated tools (used *if
									CRMReleaseRF* instead of *while CRMReleaseRF*)
		 <9>	 2/13/91	kaz		BBH, AEK-035: Fixing crash bug when _InitCM is called at INIT
									time: TrackManagers() will check for ProcessMgr before calling
									it.
		 <8>	 11/4/90	dba		put in some const
		 <7>	 10/2/90	kaz		CleanupCommToolbox() now releases TM, FT, and CM using CRM if
									the quitting app is a CommToolbox one. Added routines to keep
									track of which apps initialized TM, FT, and CM. <jng>
		 <6>	 9/18/90	kaz		Backing out last change to unload the unused 'cmtb' managers.
									<BBH>
		 <5>	 9/11/90	kaz		Calls _CRMReleaseResource for TM, CM, and FT to unload it if
									it's not being used.
		 <4>	 6/21/90	kaz		Replaced unnecessary code with _CRMReleaseRF.
		 <3>	  6/4/90	BBH		moved copy of routines SetList() and GetList() from
									CommResources.c so that link step goes better (lib/link w/o
									duplicate definitions)
		 <2>	 4/10/90	BBH		remove debugger statements, fixed bug with not advancing linked
									list pointer after deletion, and now close the resource file
									when deleting its entry from our resource file management list…
		 <1>	  4/9/90	BBH		first checked into CommToolboxStartup project.  This is NOT
									production code yet!

*/

#include <Memory.h>
#include <Types.h>
#include <Resources.h>
#include <SysEqu.h>

#include <CommToolboxPriv.h>
#include <CommResources.h>
#include <CTBUtilities.h>

#include <CommResourceMgr.h>			// for functional prototypes
#include <Processes.h>

#define	UnknownProcess		-1L
#define	JugglerTrap			0xA88F
#define	UnimplementedTrap	0xA89F


/********************************************************
	CleanUpCommToolbox	- Cleans up the tool files;
						deletes the CRM and CTB file entries;
						unload TM, FT, and CM if possible
						
*********************************************************/

pascal void CleanUpCommToolbox()
{
	ResFileRecHdl		curFile;
	short				oldRefNum;
	CTBBlockPtr			pBlock = CRMGetGlobals;
	Handle				theRes;
	ProcessSerialNumber	theProcess;
	CRMAppRecPtr		theApp;
	short				i;
	long				inUse = 0;

	//	leave if globals are not initialized
	if (pBlock == nil || (long) pBlock == -1)
		return;
	
	// Get the list of managed files
	curFile = pBlock->resFiles;
	while (curFile != nil) {
		oldRefNum = (**curFile).refNum;
		curFile = (**curFile).next;

//														added for <10>
//		_CRMReleaseRF will find tool being used in this heap and return
//			noErr if useCount is decremented, fnfErr otherwise
//			
//		using "while" means that we keep releasing until all instantiations
//		of the tool in the current heap are deleted and then the resource file
//		is closed.
//

		while (CRMReleaseRF(oldRefNum) == noErr)
			;
	}

	// We try to find the quitting app in our list of CTB apps
	// Then we relase which mgrs the app had initialized
	
	GetCurrentProcess(&theProcess);
	theApp = FindApp(&theProcess);
	
	if (theApp != nil) {
		// We do a GetResource to get the handle without incrementing the count
		
		for (i = CTB_ConnectionMgrID;i <= CTB_TerminalMgrID;i++) {
			inUse = 1 << (i - CTB_ConnectionMgrID);
			if (theApp->cmtbFlags & inUse) {
				theRes = GetResource(CTB_ConnectionMgrType,i);
				CRMReleaseResource(theRes);
			}
		}
		
		RemoveApp(theApp);
	}	// if app found

}

/********************************************************
	TrackManagers	- Keeps a list of apps that 
						initialize the CM, TM, and FT
						
		mgrID		- resID for the mgr
		returns		- -1 if attempt to _InitXX more than once

*********************************************************/
pascal short TrackManagers(short mgrID)
{
	ProcessSerialNumber		theProcess;
	CRMAppRecPtr			theApp = nil;
	long					inUse = 0;
	
	theProcess.highLongOfPSN = UnknownProcess;			// set to unknown process
	theProcess.lowLongOfPSN = UnknownProcess;
	
	/* 
	If this is called from an init, ProcessMgr (_JugglerDispatch) will not be there
	so we add a process with a bogus processID to our list. Note that if this happens, 
	the Manager will never be unloaded until rebootski.
	*/
	
	if (NGetTrapAddress(JugglerTrap, ToolTrap) != GetTrapAddress(UnimplementedTrap)) {
		GetCurrentProcess(&theProcess);					// get the calling application
		theApp = FindApp(&theProcess);					// If we're not already tracking, add it to the list
	}
	
	if (theApp == nil)
		theApp = AddApp(&theProcess);
	
	// Convert mgr res id to the bit in the flag
	inUse = 1 << (mgrID - CTB_ConnectionMgrID);
	
	// If the same app tries to init a manager twice, we ignore it
	if (theApp->cmtbFlags & inUse) 
		return(-1);
	else {
		theApp->cmtbFlags |= inUse;
		return(noErr);
	}
}


/********************************************************
	FindApp		- Looks thru the list for the matching
					process id
						
		theProcess	- unique serial number
		returns		- a ptr to the struct

*********************************************************/
CRMAppRecPtr FindApp(const ProcessSerialNumber *aProcess)
{
	CTBBlockPtr		pBlock = CRMGetGlobals;
	CRMAppRecPtr	listWalker = pBlock->appList;
	Boolean			testIt = false;
	
	while (listWalker != nil) {
		SameProcess(aProcess, &listWalker->aProcess, &testIt);
		if (testIt)
			return(listWalker);
		listWalker = listWalker->next;
	}
	
	return(nil);
}

/********************************************************
	AddApp		- Adds a new element to the CRM app list
						
		theProcess	- unique serial number
		returns		- a ptr to the new element

*********************************************************/
CRMAppRecPtr AddApp(const ProcessSerialNumber *aProcess)
{
	CTBBlockPtr		pBlock = CRMGetGlobals;
	CRMAppRecPtr	listHead = pBlock->appList,
					theApp;
	
	// Allocate and place at the head of the list
	theApp = (CRMAppRecPtr) NewPtrSysClear(sizeof(CRMAppRec));
	if (theApp != nil) {
		theApp->aProcess = *aProcess;
		theApp->next = listHead;
		pBlock->appList = theApp;
	}
	
	return(theApp);
}

/********************************************************
	RemoveApp	- Removes the list element; does not
					check for NIL
						
		theProcess	- unique serial number

*********************************************************/
void RemoveApp(CRMAppRecPtr theApp)
{
	CTBBlockPtr		pBlock = CRMGetGlobals;
	CRMAppRecPtr	listWalker = pBlock->appList,
					prevElem = nil;
	Boolean			testIt;
	
	// Check the serial numbers; if matched, fix the list & dispose
	
	while (listWalker != nil) {
		SameProcess(&theApp->aProcess, &listWalker->aProcess, &testIt);
		
		if (testIt) {
			if (prevElem == nil)			// at head
				pBlock->appList = listWalker->next;
			else
				prevElem->next = listWalker->next;	
			break;
		}
		
		prevElem = listWalker;
		listWalker = listWalker->next;
	}
	
	DisposPtr((Ptr) theApp);
}