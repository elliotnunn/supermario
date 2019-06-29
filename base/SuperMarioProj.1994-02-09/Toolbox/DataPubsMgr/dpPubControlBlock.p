{
	File:		dpPubControlBlock.p

	Contains:	Code for creation, disposing, and using Publication Control Blocks

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 6/20/90	ngk		change CanonicalFileSpec to FSSpec
		 <5>	 5/31/90	ngk		now have GetLastEditionContainerUsed
		 <4>	  4/7/90	ngk		Fix uses.
		 <3>	 3/10/90	ngk		Fixed USES file order.
		 <2>	  1/6/90	ngk		Changed USES to use Edition.p and EditionPrivate.p removed
									PubCBOnceOnlyInt to Init.p
	   <1.7>	11/13/89	ngk		Cleaned up dependencies
	   <1.6>	 10/2/89	ngk		EASE sucks
	  <•1.5>	 10/2/89	ngk		nothing
	   <1.4>	 9/18/89	ngk		Changed FileSpec to CanonicalFileSpec.
	   <1.3>	 8/29/89	ngk		Change PubCBbumpUsageCount to use IOCount instead of openCount.
	   <1.2>	  8/8/89	ngk		Added PubControlBlockOnceOnlyInit
	   <1.1>	 5/29/89	ngk		Changed PubCB to have a handle to usage info instead of the
									PubCB being variable size. Factored usage changes into
									PubCBbumpUsageCount
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}


UNIT dpPubControlBlock;


INTERFACE

USES	
	{ • public Interface files }
	StandardEqu, Editions,
	
	{ • private Interface files }
	EditionsPrivate,
	
	{ • other Edition Manager Units }
	dpMisc, dpPubIO;
	
	
CONST
	kCheckExisting = TRUE;
	kCanAllocateNew = TRUE;
	kCanGrowUsage = TRUE;
	
	FUNCTION  dpGetPubCBListHeadNode(VAR headNode: PubCBLinkHandle): OSErr;
	
	FUNCTION  dpRequestPubCB(editionFile: FSSpec; me: AppRefNum;
						checkExisting, canAllocateNew: BOOLEAN; 
						VAR thePubCB: PubCBHandle): OSErr;

	FUNCTION  dpReleasePubCB(thePubCB: PubCBHandle; me: AppRefNum): OSErr;
	
	FUNCTION  dpPubCBbumpUsageCount(usageInfo: TotalAppUsageHandle; 
								whichApp: AppRefNum; canGrowUsage: BOOLEAN;
								bumpUseCount,bumpIOCount: INTEGER):OSErr;
		
	FUNCTION  dpDisposePubCB(thePubCB: PubCBHandle): OSErr;


	PROCEDURE dpRemoveAppsPubCBs(whichApp: AppRefNum);
	
	FUNCTION dp_GetLastEditionContainerUsed(VAR container: EditionContainerSpec): OSErr;	
		
	
	
IMPLEMENTATION

{$I dpCompileOptions.inc.p }
{$I dpPubControlBlock.inc.p }

END. { dpPubControlBlock }

