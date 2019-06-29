{
	File:		dpSection.p

	Contains:	Code for creating, deleting, cloning, saving, reading, etc SectionRecords

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990, 1992 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<11>	  5/7/92	DCL		April Fools! Un-Included DateTime.
		<10>	  4/1/92	DCL		Include DateTime.p to get the calls that were moved there from
									elsewhere. part of #1025005
		 <9>	12/14/90	ngk		<MM>Added dpReconnectSection. Removed dp_FindEditionDialog.
		 <8>	 6/20/90	ngk		Change CanonicalFileSpec to FSSpec
		 <7>	  4/7/90	ngk		Fix uses
		 <6>	 3/10/90	ngk		Fix order of USES files
		 <5>	 2/16/90	ngk		Removed save, clone, get, and dispose section calls.
		 <4>	  1/8/90	ngk		Rename USE of Alias to Aliases
		<2+>	  1/7/90	ngk		Rename USE of Alias to Aliases
		 <2>	  1/6/90	ngk		Changed USES to use Edition.p and EditionPrivate.p Renamed
									routines
	   <1.9>	11/13/89	ngk		Cleaned up dependencies
	   <1.8>	10/13/89	ngk		now USE files.p
	   <1.7>	10/10/89	ngk		Changed dpNewSection to allow NIL for sectionDocument
	  <•1.6>	 10/2/89	ngk		Updated to new API
	   <1.5>	 9/18/89	ngk		Changed FileSpec to FSSpec
	   <1.4>	 8/29/89	ngk		Changed interfaces to use PubSpecs. Used dpRoutineName
									convention.
	   <1.3>	  8/8/89	ngk		Changed interface to dpNewSection and dpRegisterSection to use
									PubSpecs.
	   <1.2>	 5/31/89	ngk		Changed to use new Alias manager instead of SofaLinks
	   <1.1>	 5/29/89	ngk		Added AssociateSection
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}


UNIT dpSection;

INTERFACE

USES	
	{ • public Interface files }
	StandardEqu, Editions, Aliases,
	
	{ • private Interface files }
	EditionsPrivate, 
	
	{ • other Edition Manager Units }
	dpMisc, dpPubControlBlock, dpEvents, dpPubIO;
	
	
	FUNCTION  dpCheckSection(sectionH: SectionHandle): OSErr;
	
	FUNCTION  dpMakeItThePublisher(thePubCB: PubCBHandle; sectionH: SectionHandle; inApp: AppRefNum): OSErr;
	
	FUNCTION  dpRemovePubCBSection(sectionH: SectionHandle; inApp: AppRefNum): OSErr;

	PROCEDURE dpForEachSectionDo(PROCEDURE Doit(sectionH: SectionHandle; inApp: AppRefNum));

	FUNCTION  dpReconnectSection(sectionDocument: FSSpecPtr;
								 sectionH: SectionHandle; thisApp: AppRefNum; 
								VAR editionWasCreated, aliasWasChanged: BOOLEAN): OSErr;



	FUNCTION  dp_NewSection(container: EditionContainerSpec;
						sectionDocument: FSSpecPtr; 
						itsSectionType: SectionType; itsSectionID: longint;
						initialMode: UpdateMode; 
						VAR sectionH: SectionHandle): OSErr;
	
	FUNCTION  dp_RegisterSection(sectionDocument: FSSpec; sectionH: SectionHandle;
								VAR aliasWasChanged: Boolean): OSErr;

	FUNCTION  dp_UnRegisterSection(sectionH: SectionHandle): OSErr;

	FUNCTION  dp_IsRegisteredSection(sectionH: SectionHandle): OSErr;

	FUNCTION  dp_AssociateSection(sectionH: SectionHandle; newSectionDocument: FSSpecPtr): OSErr;
	
	FUNCTION  dp_GetEditionInfo(sectionH: SectionHandle; 
							VAR theInfo: EditionInfoRecord): OSErr;
	
	FUNCTION  dp_GotoPublisherSection(container: EditionContainerSpec): OSErr;
	
	
	
IMPLEMENTATION

{$I dpCompileOptions.inc.p }
{$I dpSection.inc.p }

END. { dpSection }
