{
	File:		dpEvents.p

	Contains:	Code for posting Section Events (PPCEvents)
				and does polling of AppleShare volumes

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	  9/5/90	ngk		Use AppleEventsInternal instead of AppleEvents.
		 <7>	  7/2/90	ngk		Added appIsRunning to dpPostOpenDoc.
		 <6>	 6/20/90	ngk		change CanonicalFileSpec to FSSpec
		 <5>	  4/7/90	ngk		update USES
		 <4>	 3/20/90	ngk		Use new AppleEvents
		 <3>	 3/10/90	ngk		Fixed order of USES files.
		 <2>	  1/6/90	ngk		Changed USES to use Edition.p and EditionPrivate.p Renamed
									routines. Removed EventsOnceOnlyInit
	   <1.9>	11/13/89	ngk		Cleaned up USEs
	   <1.8>	 11/4/89	ngk		Now use AppleEvents.p
	   <1.7>	10/24/89	ngk		Now use MFPrivate.
	   <1.6>	 10/2/89	ngk		Change polling to happen at SystemTask time
	   <1.5>	 9/18/89	ngk		Changed FileSpec to CanonicalFileSpec.
	   <1.4>	 8/29/89	ngk		Now use dpPubIO unit in dpBackGroundTask
	   <1.3>	  8/8/89	ngk		Changed interface to dpBackGroundTask and dpPostDPEent
	   <1.2>	 6/15/89	ngk		Updated USES clause
	   <1.1>	 5/29/89	ngk		Added PostOpenDoc for GotoPublisher
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}

UNIT dpEvents;


INTERFACE


USES	
	{ • public Interface files }
	StandardEqu, Editions, Aliases, AppleEventsInternal,

	{ • private Interface files }
	EditionsPrivate, MFPrivate,
	
	{ • other Edition Manager Units }
	dpMisc, dpPubControlBlock, dpPubIO, dpSection;
	
		
	FUNCTION dp_PostSectionEvent(sectionH: SectionHandle; toApp: AppRefNum; messageID: ResType): OSErr;

	FUNCTION dp_EditionMgrBackGroundTask: OSErr;

	FUNCTION dpPostOpenDoc(document: FSSpec; VAR appIsRunning: BOOLEAN): OSErr;

	
IMPLEMENTATION

{$I dpCompileOptions.inc.p }
{$I dpEvents.inc.p }

END. { dpEvents }


