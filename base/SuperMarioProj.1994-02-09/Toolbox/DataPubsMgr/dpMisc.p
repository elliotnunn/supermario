{
	File:		dpMisc.p

	Contains:	Code for handling globals and identifying applications.

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<12>	11/26/90	ngk		Added FailNonWrn and IgnoreWrn routines and
	   								lots of comments about how failure handling works.
	    <11>	  8/2/90	ngk		add DoNotPutInRegister
		<10>	 7/30/90	ngk		Added IgnoreOSErr
		 <9>	  7/2/90	ngk		Added IsEditionFile.
		 <8>	 5/31/90	ngk		Changed em globals from a handle to a pointer
		 <7>	  4/7/90	ngk		Add dynamic array. Add Failure handling. Fix USES
		 <6>	 3/10/90	ngk		Fixed order of USES files.
		 <5>	 2/25/90	ngk		Changed GetCurrentAppSignature to GetAppSignature.
		 <4>	  2/4/90	ngk		Removed Desktop database routines
		 <3>	  1/8/90	ngk		Renamed getCurrentAppRefNum
		 <2>	  1/6/90	ngk		Changed USES to use Editions.p and EditionsPrivate.p
	   <1.9>	11/13/89	ngk		Added GetStateAndLock and RestoreState. Removed Desktop manger
									externals. Cleaned up USEs.
	   <1.8>	 11/4/89	ngk		Changed AppRefNum to be a handle to app's globals.
	   <1.7>	10/24/89	ngk		Put all interface stuff that should go in other parts of the
									system here.
	   <1.6>	 10/2/89	ngk		EASE sucks
	  <•1.5>	 10/2/89	ngk		nothing
	   <1.4>	 9/18/89	ngk		Added unsigned multiple and divide
	   <1.3>	 8/29/89	ngk		Added GetCurrentAppSignature
	   <1.2>	  8/8/89	ngk		Added DeskTop Manager support routines.
	   <1.1>	 5/29/89	ngk		Changed to work with new dpCompileOptions scheme
	   <1.0>	 5/19/89	ngk		Submitted for first time

	To Do:
}
 

UNIT dpMisc;


INTERFACE

USES	
	{ • public Interface files }
	StandardEqu, Editions,
	
	{ • private Interface files }
	EditionsPrivate;


	PROCEDURE UnSignedDivide(numerator: {unsigned}LONGINT; 
							denom: {unsigned}INTEGER; 
							VAR quotient,remainder: {unsigned}INTEGER);
			
	PROCEDURE UnSignedMultiply(mult1, mult2: {unsigned}INTEGER; 
								VAR result: {unsigned}LONGINT);

	FUNCTION dpGetGlobalsLocation: EdtnMgrGlobalsPtrPtr; 

	FUNCTION dp_CallEditionOpenerProc(selector: EditionOpenerVerb; 
										VAR PB: EditionOpenerParamBlock;
										routine: EditionOpenerProcPtr): OSErr;

	FUNCTION dp_CallFormatIOProc(selector: FormatIOVerb; VAR PB: FormatIOParamBlock;
								routine: FormatIOProcPtr): OSErr;

	PROCEDURE dpSetCurrentAppRefNum(app: AppRefNum);
	
	FUNCTION dp_GetCurrentAppRefNum(VAR app: AppRefNum): OSErr;
	
	FUNCTION GetAppSignature(PSN: ProcessSerialNumber; VAR signature: OSType): OSErr;

	{ only called thruough QuitEditionPack }
	PROCEDURE RemoveAppsGlobals(whichApp: AppRefNum);

	FUNCTION dpSameBytes(aPtr, bPtr: UNIV Ptr; byteCount: INTEGER): BOOLEAN;

	FUNCTION dpNewDynamicArray(headerSize: Size; slotSize, initialSlots: INTEGER;
								FUNCTION MyNewHandle(amount: Size): Handle;
								VAR arrayH: DyanmicArrayHandle): OSErr;

	FUNCTION dpFindElement(arrayH: DyanmicArrayHandle; headerSize: Size;
							FUNCTION Matches(anEntryIndex: INTEGER; anEntryPtr: UNIV Ptr): BOOLEAN;
							VAR foundIndex: INTEGER; VAR foundPtr: UNIV Ptr): BOOLEAN;
								
	FUNCTION dpAddDynamicArrayEntry(arrayH: DyanmicArrayHandle; headerSize: Size; 
									VAR entryIndex: INTEGER; VAR entryPtr: UNIV Ptr): OSErr;
									
	FUNCTION dpGetDynamicArrayEntry(arrayH: DyanmicArrayHandle; headerSize: Size; 
									entryIndex: INTEGER; VAR entryPtr: UNIV Ptr): OSErr;
									
									
	FUNCTION  IsEditionFile(fdType: OSType): BOOLEAN;
	

	(* 
	     *** Failure handling stategy ***
	
	Just about every routine in the Edition Mgr returns an OSErr.  If the return
	value is not noErr then the routine failed (unless it is a warning).
	
	Originally, every routine had a local procedure called FailOSErr that checked 
	the return value and exited the main procedure if it was not noErr. It turned
	out that the Pascal Compiler does no register allocation if a local procedure
	exists.  So the failure handling was changed to the follow general form:
	
	FUNCTION Foo: OSErr;
	VAR
		step1:	Boolean;
		step2:	Boolean;
		fi: 	FailInfo;
	BEGIN
		DoNotPutInRegister(@step1);
		DoNotPutInRegister(@step2);
		step1 := FALSE;
		step2 := FALSE;
		
		IF IsFailure(fi, Foo) THEN
		BEGIN
			IF step1 THEN IgnoreOSErr(UndoStep1);
			IF step2 THEN IgnoreOSErr(UndoStep2);
			EXIT(Foo);
		END;
		
		{ do step 1}
		FailOSErr(DoStep1);
		step1 := TRUE;
		
		{ do step 2}
		FailOSErr(DoStep2);
		step2 := TRUE;
		
		{ do step 3}
		FailOSErr(DoStep3);
	
		Success(fi);
	END; { Foo }
	
	
	This whole scheme was developed for minimal code size.  Here is a description:
	
	1)	IsFailure installs a failure handler and returns FALSE.  If an error happens 
		afterwards, all registers (including SP and PC) are reset to their values
		when IsFailure was last called, and IsFailure returns TRUE.
		
	2)  Success removes the last installed failure handler.  
	
	3)  The compiler may put some local variable into registers.  This is bad if 
		they are tested inside the failure case, because the registers are reset 
		by the failure handling scheme.  So, any locals you need in the failure
		case must be flagged by DoNotPutInRegister();
		
	4)  As a rule all exported routines (they all start with dp_) have a failure handler.
		Internal routines may be simple enough to just return an error, or it may invoke
		a failure.  If you want to do something special if an internal routine returns a
		particular error, then you can not use "anErr := dpInternal();".  You must install
		a dummy handler around the call.  For example:
		
		IF NOT IsFailure(fi, anErr) THEN { catch any failures properly }
		BEGIN
			FailOSErr(dpInternal());
			Success(fi);
		END;
		{ CASE anErr OF ... }
		
	5)	IgnoreOSErr just pops the OSErr off the stack, totally ignoring it.
	
	6)  Some routines return warnings (i.e. notThePublisherWrn).  Often you don't want to 
		fail with them.  You can either use:
		
			anErr := FailNonWrn(dpInternal());
			
			IgnoreWrn(dpInternal());
			
		FailNonWrn returns warnings and noErr, but call FailOSErr on anything else.  
		IgnoreWrn eats warnings and noErr, but call FailOSErr on anything else.

	7)  If no failure handler is installed and a failure happens, Debugger is called.
		You should never let this happen!  This is why all exported routines start with dp_.
	
	
	*)



	{ fail if anErr <> 0 }
	PROCEDURE FailOSErr(anErr: OSErr);
	
	{ ignore any error }
	PROCEDURE IgnoreOSErr(anErr: OSErr);
		INLINE $548F; 
	
	{ fail if anErr <> 0 and anErr <> a warning }
	FUNCTION  FailNonWrn(anErr: OSErr): OSErr;

	{ ignore any warning, but fail if error }
	PROCEDURE IgnoreWrn(anErr: OSErr);

	{ call this at start of a function to install a failure handler }
	{ It saves off all registers into 'info', including SP and PC }
	{ and pushes it to the top of the failure handler stack }
	{ Initializes 'resultValue' to noErr }
	FUNCTION  IsFailure(VAR info: FailInfo; VAR resultValue: OSErr): BOOLEAN;
	
	{ call this at the end of a function to remove a failure handler }
	PROCEDURE Success(VAR info: FailInfo);
									
	{ this is needed for all variables in a function that will be tested }
	{ in the IsFailure part. }	
	PROCEDURE DoNotPutInRegister(p: UNIV Ptr);
		INLINE $588F;	


IMPLEMENTATION

{$I dpCompileOptions.inc.p }
{$I dpMisc.inc.p }

END. { dpMisc }

