{
	File:		dpPubIO.p

	Contains:	Code to control all I/O access to an edition file

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<13>	12/14/90	ngk		<MM>Removed dpCreateEditionFile routine.
		<12>	  7/2/90	ngk		Changed dpResolveAliasToPublisher. Added dpFindPublisher.
		<11>	 6/20/90	ngk		change CanonicalFileSpec to FSSpec
		<10>	 5/31/90	ngk		export tweakCatInfo
		 <9>	  5/3/90	ngk		Export dpCreateEditionFile and dpResolveAliasToPublisher
		 <8>	  4/7/90	ngk		Add dpPubSync & dpNotifySubscribers. Fixed USES.
		 <7>	 3/20/90	ngk		Add script to dp_CreateEditionContainerFile.
		 <6>	 3/10/90	ngk		Fixed order of USES files.
		 <5>	  2/4/90	ngk		Allow OpenNewEdition to have NIL sectionDocument
		 <4>	  1/8/90	ngk		Change USE from Alias to Aliases
		<2+>	  1/7/90	ngk		Change USE from Alias to Aliases
		 <2>	  1/6/90	ngk		Changed USES to use Edition.p and EditionPrivate.p Renamed
									routines.
	   <1.9>	11/13/89	ngk		Cleaned up USEs
	   <1.8>	10/24/89	ngk		Now USE MFPrivate.
	  <•1.7>	 10/2/89	ngk		Updated to new API
	   <1.6>	 9/18/89	ngk		Changed FileSpec to CanonicalFileSpec
	   <1.5>	 8/29/89	ngk		Changed to dpPubOpen/CloseFile & dpPubStart/EndIO
	   <1.4>	  8/8/89	ngk		Added comments param to dpNewPublication
	   <1.3>	 6/11/89	ngk		Added dp prefix to all routines
	   <1.2>	 5/31/89	ngk		Changed from SofaLinks to new Aliases
	   <1.1>	 5/29/89	ngk		Added WriteFormat and ReadFormat
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}


UNIT dpPubIO;

INTERFACE

USES	
	{ • public Interface files }
	StandardEqu, Editions, Aliases,
	
	{ • private Interface files }
	EditionsPrivate, MFPrivate, 
	
	{ • other Edition Manager Units }
	dpMisc, dpEvents, dpPubControlBlock, dpSection, dpSectionIO;

CONST
	kFlush	=	TRUE;

	{ public routines }					
	FUNCTION dp_CreateEditionContainerFile(editionFile: FSSpec;
											fdCreator: OSType;
											editionFileNameScript: INTEGER): OSErr;
		
	FUNCTION dp_DeleteEditionContainerFile(editionFile: FSSpec): OSErr;

	FUNCTION dp_OpenEdition(sectionH: SectionHandle; VAR sectCB: SIOCBHandle): OSErr;

	FUNCTION dp_OpenNewEdition(sectionH: SectionHandle; fdCreator: OSType;
							sectionDocument: FSSpecPtr;
							VAR sectCB: SIOCBHandle): OSErr;

	FUNCTION dp_CloseEdition(sectCB: SIOCBHandle; appWasSuccessful: BOOLEAN): OSErr;
		
	FUNCTION dp_GetStandardFormats(container: EditionContainerSpec; VAR previewFormat: FormatType;
									preview, publisherAlias, formats: Handle): OSErr;
	

	{ internal routines }		
	FUNCTION dpPubSync(thePubCB: PubCBHandle): OSErr;

	FUNCTION dpNotifySubscribers(thePubCB: PubCBHandle): OSErr;

	FUNCTION dpPubLoadMap(thePubCB: PubCBHandle; kind: SectionType): OSErr;
	
	FUNCTION dpPubReleaseMap(thePubCB: PubCBHandle): OSErr;

	FUNCTION dpTweakCatInfo(edition: FSSpec; 
							PROCEDURE Tweaker(VAR PBC: CInfoPBRec) ): OSErr;

	FUNCTION dpPubOpenFile(thePubCB: PubCBHandle; kind: SectionType): OSErr;

	FUNCTION dpPubCloseFile(thePubCB: PubCBHandle; flush: BOOLEAN): OSErr;
	
	FUNCTION dpStandardOpener(selector: EditionOpenerVerb; VAR PB: EditionOpenerParamBlock): OSErr; 
	
	FUNCTION dpFindPublisher(thePubCB: PubCBHandle; canAskUser: BOOLEAN;
							VAR publisherSectionH: SectionHandle;
							VAR publisherApplication: AppRefNum;
							VAR publisherDoc: FSSpec;
							VAR theSectionID: LONGINT): OSErr;

	FUNCTION dpResolveAliasToPublisher(thePubCB: PubCBHandle; canAskUser: BOOLEAN;
										VAR publisherDoc: FSSpec;
										VAR theSectionID: LONGINT): OSErr;

	
	
IMPLEMENTATION

{$I dpCompileOptions.inc.p }
{$I dpPubIO.inc.p }

END. { dpPubIO }


