{
	CTBUtilities.p
	Pascal Interface to the Communications Toolbox Utilities
	
	Copyright © Apple Computer, Inc. 1988-90
	All rights reserved
}
{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT CTBUtilities;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingCTBUtilities}
{$SETC UsingCTBUtilities := 1}

{$I+}
{$SETC CTBUtilitiesIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingMemory}
{$I $$Shell(PInterfaces)Memory.p}
{$ENDC}
{$IFC UNDEFINED UsingPackages}
{$I $$Shell(PInterfaces)Packages.p}
{$ENDC}
{$IFC UNDEFINED UsingAppleTalk}
{$I $$Shell(PInterfaces)AppleTalk.p}
{$ENDC}
{$IFC UNDEFINED UsingDialogs}
{$I $$Shell(PInterfaces)Dialogs.p}
{$ENDC}
{$SETC UsingIncludes := CTBUtilitiesIncludes}

CONST
{ version of the Comm Toolbox Utilities }
	curCTBUVersion		= 2;
	
{ CTBUErr}
	ctbuGenericError	= -1;
	ctbuNoErr			= 0;

{ DITLMethod }
	overlayDITL 		= 0;
	appendDITLRight 	= 1;
	appendDITLBottom 	= 2;

{ Choose responses }
	chooseDisaster		= -2;
	chooseFailed		= -1;
	chooseAborted		=  0;
	chooseOKMinor		=  1;
	chooseOKMajor		=  2;
	chooseCancel		=  3;

{ NuLookup responses }
	nlOk				=	0;
	nlCancel			=	1;
	nlEject				=	2;
	
{ Name FilterProc responses }
	nameInclude			=	1;
	nameDisable			=	2;
	nameReject			=	3;
	
{ Zone FilterProc responses }
	zoneInclude			=	1;
	zoneDisable			=	2;
	zoneReject			=	3;
	
{ Dialog items for hook procedure }
	hookOK				=	1;
	hookCancel 			=	2;
	hookOutline			=	3;
	hookTitle			=	4;
	hookItemList		=	5;
	hookZoneTitle		=	6;
	hookZoneList		=	7;
	hookLine			=	8;
	hookVersion			=	9;
	hookReserved1		=	10;
	hookReserved2 		=	11;
	hookReserved3		=	12;
	hookReserved4		=	13;
{ Virtual items in the dialog item list }
	hookNull			=	100;
	hookItemRefresh		=	101;
	hookZoneRefresh		=	102;
	hookEject			=	103;
	hookPreflight		=	104;
	hookPostflight		=	105;
	hookKeyBase			=	1000;
	
TYPE
	CTBUErr 	= OSErr;
	DITLMethod 	= INTEGER;
	
	NLTypeEntry = RECORD
		hIcon		: Handle;
		typeStr 	: Str32;
	END;
	
	NLType 		= Array[0..3] of NLTypeEntry;
	
	NBPReply = RECORD
		theEntity	:	EntityName;
		theAddr		:	AddrBlock;
	END;
	

FUNCTION	InitCTBUtilities: CTBUErr;
FUNCTION	CTBGetCTBVersion : INTEGER;

PROCEDURE 	AppendDITL(theDialog: DialogPtr; theDITL: Handle; method: DITLMethod);
FUNCTION 	CountDITL(theDialog: DialogPtr) : INTEGER;
PROCEDURE 	ShortenDITL(theDialog: DialogPtr; numberItems: INTEGER);

FUNCTION	StandardNBP(where: Point; prompt: Str255; numTypes: INTEGER;
					 typeList: NLType; nameFilter: ProcPtr; zoneFilter: ProcPtr;
					 hookProc: ProcPtr; VAR theReply: NBPReply) : INTEGER;

FUNCTION	CustomNBP(where: Point; prompt: Str255; numTypes: INTEGER;
				   typeList: NLType; nameFilter: ProcPtr; zoneFilter: ProcPtr;
				   hookProc: ProcPtr; userData: LONGINT; dialogID: INTEGER; 
				   filterProc: ProcPtr; VAR theReply: NBPReply) : INTEGER;				   

{ Obsolete synonyms for above routines }

FUNCTION	NuLookup(where: Point; prompt: Str255; numTypes: INTEGER;
					 typeList: NLType; nameFilter: ProcPtr; zoneFilter: ProcPtr;
					 hookProc: ProcPtr; VAR theReply: NBPReply) : INTEGER;

FUNCTION	NuPLookup(where: Point; prompt: Str255; numTypes: INTEGER;
				   typeList: NLType; nameFilter: ProcPtr; zoneFilter: ProcPtr;
				   hookProc: ProcPtr; userData: LONGINT; dialogID: INTEGER; 
				   filterProc: ProcPtr; VAR theReply: NBPReply) : INTEGER;
				   

{$ENDC}    { UsingCTBUtilities }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

