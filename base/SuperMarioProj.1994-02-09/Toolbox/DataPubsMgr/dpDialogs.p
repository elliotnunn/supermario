{
	File:		dpDialogs.p

	Contains:	code for all modal dialogs in the Edition Manager

	Written by:	Nick Kledzik

	Copyright:	© 1989-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<15>	  5/7/92	DCL		April Fools! Un-Included DateTime.
		<14>	  4/1/92	DCL		Include DateTime.p to get the calls that were moved there from
									elsewhere.  Part of #1025005
		<13>	 10/2/91	JSM		Use StandardFilePriv instead of StandardFile3Priv.
		<12>	 1/29/91	JAL		Added include for MenuMgrPriv.
		<11>	12/14/90	ngk		<RC>add USE of dpSection
		<10>	10/29/90	ngk		added use of DialogsPriv
		 <9>	  6/6/90	ngk		Use IconUtilsPriv for generic icons
		 <8>	 5/31/90	ngk		use CTBUtilities
		 <7>	  4/7/90	ngk		Add use of dpPubIO. Fix USES again.
		 <6>	 3/17/90	ngk		Add dp_DrawPreview to interface.
		 <5>	 3/10/90	ngk		Fix USES for faster compiles.
		 <4>	 2/25/90	ngk		Use StandardFile3 LDEF.
		 <3>	  2/7/90	ngk		Add dependency on Dialogs and Files.
		 <2>	  1/6/90	ngk		Change USES to Editions.p & EditionsPrivate.p moved last
									container code to dpInit.p removed short versions of routines.
	   <1.7>	11/13/89	ngk		Cleaned up USEs
	  <•1.6>	 10/2/89	ngk		Added DialogsOnceOnlyQuit to support saving of last
									EditionContainer throught shutdowns
	   <1.5>	 9/25/89	ngk		Changed some uses to support new SectionInfoDialog.
	   <1.4>	 9/18/89	ngk		Added filter proc to dialogs
	   <1.3>	  8/8/89	ngk		Changed definition of dpSubscribeToDialog and dpPublishAsDialog
									to use reply records instead of lots of VAR parameters.
	   <1.2>	 5/31/89	ngk		Removed dependency on SofaLinks
	   <1.1>	 5/29/89	ngk		Added SectionInfoDialog Changed Interface to PublishAsDialog to
									return UpdateMode
	   <1.0>	 5/19/89	ngk		Submitted for first time

}

UNIT dpDialogs;


INTERFACE

USES	
	{ • public Interface files }
	StandardEqu, Editions, StandardFile, CTBUtilities, 
		
	{ • private Interface files }
	EditionsPrivate, IconUtilsPriv, StandardFilePriv, Layers, DialogsPriv,MenuMgrPriv,
	
	{ • other Edition Manager Units }
	dpMisc, dpSection, dpPubIO, dpEvents;
	
	
	FUNCTION dp_DrawPreview(theData: Handle; theFormat: FormatType; previewRect: Rect): OSErr;
	

	FUNCTION dp_NewSubscriberExpDialog(VAR reply: NewSubscriberReply; where: Point; 
									extentionDITLresID: INTEGER; dlgHook: ExpDlgHookProcPtr; 
									filterProc:	ExpModalFilterProcPtr; callBackPtr: Ptr): OSErr;


	FUNCTION dp_NewPublisherExpDialog(VAR reply: NewPublisherReply; where: Point; 
									extentionDITLresID: INTEGER; dlgHook: ExpDlgHookProcPtr; 
									filterProc:	ExpModalFilterProcPtr; callBackPtr: Ptr): OSErr;
		
							
	FUNCTION dp_SectionOptionsExpDialog(VAR reply: SectionOptionsReply; where: Point; 
									extentionDITLresID: INTEGER; dlgHook: ExpDlgHookProcPtr; 
									filterProc:	ExpModalFilterProcPtr; callBackPtr: Ptr): OSErr;

	
	
IMPLEMENTATION

{$I dpCompileOptions.inc.p }
{$I dpDialogs.inc.p }

END. { dpDialogs }


