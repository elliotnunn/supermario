{
	File:		dpSectionIO.p

	Contains:	Code layered on top of PubIO.p to control I/O to Edition files
				via a Section I/O control Block 

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	  4/7/90	ngk		Fixed uses
		 <3>	 3/10/90	ngk		Fixed order of USES files.
		 <2>	  1/6/90	ngk		Changed USES to use Edition.p and EditionPrivate.p Renamed
									routines.
	   <1.6>	11/13/89	ngk		Cleaned up USES.
	   <1.5>	 10/2/89	ngk		Updated to new API
	   <1.4>	 8/29/89	ngk		Better names. pos->mark, whichType->whichFormat
	   <1.3>	  8/8/89	ngk		
	   <1.2>	 6/11/89	ngk		Added dpHasEditionFormat
	   <1.1>	 5/29/89	ngk		Changed to new dpCompileOptions usage
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}

UNIT dpSectionIO;


INTERFACE

USES	
	{ • public Interface files }
	StandardEqu, Editions,

	{ • private Interface files }
	EditionsPrivate,
	
	{ • other Edition Manager Units }
	dpMisc, dpSection, dpPubIO;

	
	FUNCTION dpCreateSIOCBRecord(itsSectionH:	SectionHandle; 
								 itsRefNum:		LONGINT;
								 itsProc:		FormatIOProcPtr;
								 VAR sectCB: 	SIOCBHandle): OSErr; 
								 
	FUNCTION dpStandardIO(selector: FormatIOVerb; VAR PB: FormatIOParamBlock): OSErr; 


	FUNCTION dp_EditionHasFormat(sectCB: SIOCBHandle; whichFormat: FormatType; 
								VAR length: Size): OSErr; 

	FUNCTION dp_GetEditionFormatMark(sectCB: SIOCBHandle; whichFormat: FormatType; 
							VAR mark: Longint): OSErr; 
	
	FUNCTION dp_SetEditionFormatMark(sectCB: SIOCBHandle; whichFormat: FormatType; 
							mark: Longint): OSErr; 
	
	FUNCTION dp_ReadEdition(sectCB: SIOCBHandle; whichFormat: FormatType;
							buffPtr: Ptr; VAR buffLen: Size): OSErr;
	
	FUNCTION dp_WriteEdition(sectCB: SIOCBHandle; whichFormat: FormatType; 
							buffPtr: Ptr; buffLen: Size): OSErr;


	
			
IMPLEMENTATION

{$I dpCompileOptions.inc.p }
{$I dpSectionIO.inc.p }

END. { dpSectionIO }
