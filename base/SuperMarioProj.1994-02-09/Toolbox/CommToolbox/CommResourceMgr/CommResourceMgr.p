{
	File:		CommResourceMgr.p

	Written by:	Byron Han

	Copyright:	© 1989-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM3>	 9/28/93	PN		Adding two new calls AddStdPort and RemoveStdPort to the
									CommResourceManager from DBLite gibbly.
		 <8>	 5/29/92	DCL		Included TextUtils.h. EqualString moved for the New Inside Mac.
		 <7>	 5/21/91	gbm		Stop using "empty" units like Memtypes...
		 <6>	 7/10/90	dba		add __ to distinguish routines from glue
		 <5>	  7/2/90	kaz		Cleaning up/code shrinkage. Removed CRMDumpResChain as this call
									can be done w/ 'rd' dcmd. Added NIL checks for CRMIsDriverOpen().
		 <4>	  4/9/90	BBH		* Moving CRMCreateCommunications to CommResourceMgr.c
		 <3>	 3/21/90	EMT		Added AppleTalk USES for Str32 in CommToolboxPriv.p.
		 <2>	 3/16/90	BBH		made it compile
		 <1>	 3/14/90	BBH		first checked in

	To Do:

}
UNIT ResourceFile;
{
	This routine contains routines private to the Communications Toolbox that manipulate the
	resource manager chain.

	Written by Byron Han.  Copyright © 1989 Apple Computer, Inc.  All rights reserved.

	Review History

	Modification History
		3/27/89		BBH		Got it up.
		4/7/89		BBH		Documented it.... he he he
							CRMHOpenResFile had a bug where if you opened the file
							but were unable to register with the CRM, no error code
							was returned.  Now returns -1 and closes the file.

		4/9/89		BBH		CRMReleaseRF/CRMReserveRF now return fnfErr if refnum doesn't
							match a file in the resource file list
							CRMCreateCommunications now returns noErr properly.  was not
							setting the return value if no errors resulted.
							CRMGetIndFile was not returning an error code if the
							file was not found.  It now does.

		4/11/89		BBH		Don't change CurMap in CRMToolPreflight/ToolPostflight
		4/26/89AM	BBH		Added CRMIsDriverOpen function.
		4/26/89		JNG		Added CRMCheckTool
		5/6/89		BBH		Changed to new resource map scheme (1 fcb/tool/zone)
		5/9/89		BBH		Changed CRMCheckTool to return TRUE if file not found.
								prevents crash in Sample's MakeNew routine.
		6/1/89		BBH		CRMOpenRFPerm now performs filename comparison using
								EqualString which is case INSENSITIVE and diacritc SENSITIVE
		6/14/89		JNG		Some little cleanup stuff
		7/13/89		JNG		Ported massive portions to assembler.  Check old versions of this
							file for pascal equivalents.
		7/27/89		JNG		Ported CRMFindCommunications, CRMCountTool, CRMGetIndFile to assembler
		9/1/89		BBH		Changed hard coded Communications to Communications Folder
		9/19/89		BBH		Changed exit code to CRMLocalToRealID and CRMRealToLocalID
								to do a HPurge instead of ReleaseResource
}
INTERFACE

{$SETC COMMRESOURCEMGR:=TRUE}
{$SETC OLDWAY:=FALSE}
USES
	Types, Memory, Dialogs, Menus, Files, Resources, Devices, SysEqu, TextUtils,	{ system equates }
	CommToolboxPriv,																{ comm rsrc mgr private routines }
	ToolIntf, CommResources, GestaltEqu, Traps, CRMSerialDevices, Power,
	CTBUtilities;																	{ comm toolbox utilities }

CONST
	portNameIndex		= 1;			{ 'STR#' indices	}
	inputDriverIndex	= 2;
	outputDriverIndex	= 3;
	NoSuchPortErr		= 1;								{<3>}

{$IFC DBLite}
	gestaltPortInfoAttr			= 'port';
	gestaltModemMapped			= 0;
	gestaltNoPrinterPort		= 1;
	gestaltNoModemPort			= 2;								{<3>}
	
	CTB_InternalModemIconID		= -32509;		{ RJ-11 or telephone	}
	CTB_PrinterModemIconID		= -32508;		{ combo icon			}
{$ENDC}

TYPE
	IconSelectorValue	=	LONGINT;
	
PROCEDURE 	RemovePort(resID: INTEGER);
PROCEDURE 	AddPort(resID: INTEGER);
PROCEDURE 	NoPurgeResources;								 			{<4>}

FUNCTION	__CRMParseRefNum(refNum: INTEGER; VAR fName: STR63; VAR vRefNum: INTEGER): OSErr;
													{ given file refnum, return fName and WD }
FUNCTION 	__CRMLocalToRealID( bundleType:ResType;
					toolID:INTEGER; theKind:ResType; localID:INTEGER) : INTEGER;
FUNCTION 	__CRMRealToLocalID( bundleType:ResType;
					toolID:INTEGER; theKind:ResType; realID:INTEGER) : INTEGER;

FUNCTION	__CRMIsDriverOpen(driverName: STR255): BOOLEAN;

FUNCTION 	__CRMCheckTool(fName: STR63; fType: OSType;
					vRefNum: INTEGER; dirID: LONGINT) : Boolean;
IMPLEMENTATION


FUNCTION DisguiseID(resID: INTEGER): INTEGER; FORWARD;				{<3>}
FUNCTION NoPurge(theType: ResType; VAR theIcon: Handle; 			{<4>}
							yourDataPtr: Ptr): OSErr; FORWARD;		{#4: no purge proc }
FUNCTION ExpressModemPresent: BOOLEAN; FORWARD;

{ Icon Utils Stuff	}
FUNCTION DisposeIconSuite(theIconSuite: Handle;
							disposeData: BOOLEAN): OSErr;
	INLINE	$303C, $0302, $ABC9;
FUNCTION GetIconSuite(VAR theIconSuite: Handle;
							theResID: INTEGER;
							selector: IconSelectorValue): OSErr;
	INLINE	$303C, $0501, $ABC9;
FUNCTION GetIconFromSuite(VAR theIconData: Handle;
							theSuite: Handle;
							theType: ResType): OSErr;
	INLINE	$303C, $0609, $ABC9;
FUNCTION ForEachIconDo(theSuite: Handle;selector: IconSelectorValue;	{<4>}
							action: ProcPtr; yourDataPtr: Ptr): OSErr;
	INLINE	$303C, $060A, $ABC9;
		
FUNCTION DockingDispatch(selector: LONGINT;params: LONGINT): LONGINT;
	INLINE	$AA57;
	
{ PowerMgr Patch	}
FUNCTION GetModemType: LONGINT;
	INLINE	$203C, $0003, $000A,	{ move.l	#$0003000A, d0		; move the selector	}
			$A09F,					{ _PowerDispatch				; call trap			}
			$2E80;					{ move.l 	d0, (sp)			; return the type on stack	}

	
{ FUNCTION ModemStat(VAR status:Byte): OSErr; EXTERNAL; }

Function ModemStat(VAR status:Byte) : OSErr;
BEGIN
	ModemStat := -1
END;
	
{ **************************************************************
	GetCRMEntry	- Scans the CRM list and returns the CRM entry
					whose input driver name is given
		
		resID		- Std Serial Port ID to locate
**************************************************************** }
FUNCTION GetCRMEntry(resID: INTEGER): CRMRecPtr;
VAR
	theCRM		: CRMRecPtr;
	theCRMRec	: CRMRec;
	inName		: Str255;
	
BEGIN
	GetIndString(inName,resID,inputDriverIndex);
	
	theCRMRec.crmDeviceType := crmSerialDevice;
	theCRMRec.crmDeviceID := 0;					{ Find first one	}
	
	REPEAT
		{ Fill out the search record	}
		theCRM := @theCRMRec;
		
		{ CRMSearch looks for the next element whose crmDevice	}
		{ is greater than the one passed						}
		
		theCRM := CRMRecPtr(CRMSearch(QElemPtr(theCRM)));
		
		IF (theCRM = NIL) THEN
			LEAVE;
				
		{ Is it the one we love?	}
		WITH CRMSerialPtr(theCRM^.crmAttributes)^ DO
			IF EqualString(inName, inputDriverName^^, FALSE, FALSE) THEN
				LEAVE;
		
		{ Next index for search	}
		theCRMRec.crmDeviceID := theCRM^.crmDeviceID;
			
	UNTIL FALSE; { endless loop }
	
	GetCRMEntry := theCRM;
END;

{ **************************************************************
	RemovePort	- removes the printer port
	
		resID	- the port id if needed
**************************************************************** }
PROCEDURE RemovePort(resID: INTEGER);
VAR
	newCRM		: CRMRecPtr;
	ignore		: OSErr;
{$IFC OLDWAY}																	{<4>}
	guiseID		: INTEGER;														{<3>}
{$ENDC}																			{<4>}

BEGIN	
{$IFC OLDWAY}																	{<4>}
	{ We ignore the error returned by DisguiseID as we're removing the problem	 <3>}
	guiseID := DisguiseID(resID);												{<3>}
	IF (guiseID = NoSuchPortErr) THEN											{<3>}
		guiseID := resID;														{<3>}
	
	newCRM := GetCRMEntry(guiseID);
{$ENDC}																			{<4>}
	newCRM := GetCRMEntry(resID);												{<4>}
	IF (newCRM = NIL) THEN
		EXIT(RemovePort);
		
	IF (CRMRemove(QElemPtr(newCRM)) <> noErr) THEN
		EXIT(RemovePort);
	
	WITH CRMSerialPtr(newCRM^.crmAttributes)^ DO BEGIN
		IF (deviceIcon <> NIL) THEN
			IF (GetHandleSize(Handle(deviceIcon)) > 256) THEN
				ignore := DisposeIconSuite(deviceIcon^^.theSuite,false);
				
		DisposHandle(Handle(deviceIcon));		{ checks for nil	}
		DisposHandle(Handle(name));
		DisposHandle(Handle(inputDriverName));
		DisposHandle(Handle(outputDriverName));
	END;
	
	DisposPtr(Ptr(newCRM));
END;


{ **************************************************************
	AddPort	- adds the printer port
	
		resID		- port ID for resources in System file
**************************************************************** }
PROCEDURE AddPort(resID: INTEGER);	
VAR
	newCRM		: CRMRecPtr;
	pSerial		: CRMSerialPtr;
	hIcon		: Handle;
	theCRMIcon	: CRMIconHandle;
	savedZone	: THz;
	ignore		: OSErr;
	guiseID		: INTEGER;			{ For alternate icon and/or port name	}

LABEL
	1;
	
	FUNCTION MyGetString(resID: INTEGER; theIndex: INTEGER): StringHandle;
	VAR
		theString	: Str255;
		theResource	: Handle;
	BEGIN
		GetIndString(theString, resID, theIndex);
		MyGetString := NewString(theString);
	END;
	
BEGIN	
	{ Don't add if it's already there	}
	newCRM := GetCRMEntry(resID);
	IF (newCRM <> NIL) THEN
		EXIT(AddPort);		{ Not a real error condition	}
		
	guiseID := DisguiseID(resID);											{<3>}
	IF (guiseID = NoSuchPortErr) THEN										{<3>}
		EXIT(AddPort);														{<3>}
				
	newCRM := CRMRecPtr(NewPtrSysClear(sizeof(CRMRec)));
	IF (newCRM = NIL) THEN
		GOTO 1;
	
	pSerial := CRMSerialPtr(NewPtrSysClear(sizeof(CRMSerialRecord)));
	IF (pSerial = NIL) THEN
		GOTO 1;
	
	WITH newCRM^ DO BEGIN
		crmDeviceType := crmSerialDevice;
		crmAttributes := LONGINT(pSerial);
	END;
	
	WITH pSerial^ DO BEGIN
		version := curCRMSerRecVers;
					
		theCRMIcon := CRMIconHandle(NewHandleSys(SIZEOF(CRMIconRecord)));
		IF (theCRMIcon = NIL) THEN
			GOTO 1;
		
		savedZone := GetZone;
		SetZone(SystemZone);
			
			HLock(Handle(theCRMIcon));
			WITH theCRMIcon^^ DO BEGIN
				ignore := GetIconSuite(hIcon,guiseID,-1);		{ -1 => all available	}
				theSuite := hIcon;
				
				{ Extract the old style icon & move into the struct	}
				IF (theSuite <> NIL) THEN				
					ignore := GetIconFromSuite(hIcon,theSuite,'ICN#');
				{ IF (hIcon <> NIL) THEN
					BlockMove(hIcon^,Ptr(@theCRMIcon^^.oldIcon),256);} { icon & mask	}
					
				reserved := 0;
			END; { with }
			HUnlock(Handle(theCRMIcon));
			
			{ Get the names of the port & drivers	}
			name := MyGetString(guiseID, portNameIndex);		
			inputDriverName := MyGetString(guiseID, inputDriverIndex);			{ <5> changed resID to guiseID	}
			outputDriverName := MyGetString(guiseID, outputDriverIndex);	
			
		SetZone(savedZone);

		deviceIcon := theCRMIcon;
		ratedSpeed := 9600;
		maxSpeed := 57600;
	
	END; {with}
	
	CRMInstall(QElemPtr(newCRM));
	EXIT(AddPort);
	
1:
	IF (pSerial <> NIL) THEN
		DisposPtr(Ptr(pSerial));
	IF (newCRM <> NIL) THEN
		DisposPtr(Ptr(newCRM));
END;

{ **************************************************************
	DisguiseID		- tests the given ID for validity & 			<3>
					transmogrifies it based on the docking state	<3>
	
		resID		- port ID for resources in System file
		returns		- new res ID or err
**************************************************************** }
FUNCTION DisguiseID(resID: INTEGER): INTEGER;
CONST
	ModemInstalledMask	= $0800;		{ Pascal will put the status in the high byte	}
VAR
	newID 		: INTEGER;
	response	: LONGINT;
	mdmStatus	: INTEGER;
	
LABEL
	1;
	
BEGIN
	DisguiseID := resID;
		
	{ #3: Don't add if the port already exists; <kaz> }
{$IFC DBLite}
	IF (Gestalt(gestaltPortInfoAttr,response) = noErr) THEN BEGIN
		IF (resID = CTB_PrinterIconID) AND 
						(BTST(response,gestaltNoPrinterPort)) THEN
			GOTO 1;
		
		{ #4: Don't add internal modem if it's not installed or already mapped	}
		IF (resID = CTB_InternalModemIconID) THEN BEGIN
{$IFC OLDWAY}
			IF (ModemStat(Byte(mdmStatus)) <> noErr) THEN		{ #4: Pascal treats VAR Bytes starngely <kaz>	}
				GOTO 1;
			IF (BAND(mdmStatus, ModemInstalledMask) = 0) THEN
				GOTO 1;
{$ENDC}
			IF NOT ExpressModemPresent THEN
				GOTO 1;
			IF (BTST(response,gestaltModemMapped)) THEN
				GOTO 1;
		END;
		
		{ Modem is more complex as it can have different IDs depending on the state of	}
		{ the machine.  We also have to allow for an internal mapped modem with no		}
		{ port A on the machine.														}
	
		IF (resID = CTB_ModemIconID) THEN BEGIN
			IF (ExpressModemPresent AND BTST(response,gestaltModemMapped)) THEN
				DisguiseID := CTB_InternalModemIconID
			ELSE IF (BTST(response,gestaltNoModemPort)) THEN
				GOTO 1
			ELSE IF (BTST(response,gestaltNoPrinterPort)) THEN
				DisguiseID := CTB_PrinterModemIconID;
		END; { Port A Icon }
	END; { Misc Port Info }
	
{$ENDC}

	EXIT(DisguiseID);
1:
	DisguiseID := NoSuchPortErr;
END;

{ **************************************************************
	NoPurgeResources - Since AddPort can be called on wakeup					<4>
					before SCSI reinit, we can't have disk i/o.					<4>
					So we make sure we won't get purged							<4>
**************************************************************** }
PROCEDURE NoPurgeResources;
VAR
	theResource		: Handle;
	ignore			: OSErr;
	
BEGIN
	theResource := NIL;
	ignore := NoPurge('STR#',theResource,Ptr(CTB_ModemIconID));
	ignore := NoPurge('STR#',theResource,Ptr(CTB_PrinterIconID));
	
	ignore := GetIconSuite(theResource,CTB_ModemIconID,-1);		{ -1 => all available	}
	IF (theResource <> NIL) THEN
		ignore := ForEachIconDo(theResource,-1,@NoPurge,NIL);	{ no purge all icons }
		
	ignore := GetIconSuite(theResource,CTB_PrinterIconID,-1);	{ -1 => all available	}
	IF (theResource <> NIL) THEN
		ignore := ForEachIconDo(theResource,-1,@NoPurge,NIL);	{ no purge all icons }
END;

{ **************************************************************
	NoPurge		- Call Back proc to mark resources non-purgeable
	
		theType		- Icon suite type or STR#
		theIcon		- Res Handle -- garbage for STR#
		yourDataPtr	- my use -- resID for STR#
**************************************************************** }
FUNCTION NoPurge(theType: ResType; VAR theIcon: Handle; 
												yourDataPtr: Ptr): OSErr;
BEGIN
	IF (theType = 'STR#') THEN
		theIcon := GetResource(theType,INTEGER(yourDataPtr));
	HNoPurge(theIcon);
	NoPurge := noErr;
END;

{ **************************************************************
	ExpressModemPresent	- Determines if the Express Modem is
							present by checking HW & SW
		
		returns		- true if present
**************************************************************** }
FUNCTION ExpressModemPresent: BOOLEAN;
CONST
	ModemInstalledMask	= $0800;		{ Pascal puts result in high byte		}
	ModemTypeDUO		= 2;			{ rockwell data pump modem (RC144DPL)	}
	ModemTypeDUOPlus	= 3;			{ rockwell data pump modem (RC144DPL) + 6805 controller	}
	
	gestaltDBLite		= 29;
	gestaltDBLiteLC		= 32;

VAR
	anyErr			: OSErr;
	machineType,
	powrMgr,
	modemType		: LONGINT;
	mdmStatus		: INTEGER;
	
BEGIN
	ExpressModemPresent := FALSE;
		
	{ We would prefer to see if the 3615 gestalt selector	}
	{ is installed, but we're not guaranteed to run after	}
	{ 3615 is installed, so we check HW instead				}
	
	{ First, is there a PowerMgr	}
	anyErr := Gestalt(gestaltPowerMgrAttr, powrMgr);
	IF (anyErr <> noErr) THEN
		EXIT(ExpressModemPresent);
	IF NOT BTST(powrMgr,gestaltPMgrExists) THEN
		EXIT(ExpressModemPresent);
		
	{ Now, check machine types so we don't call a non-existent patch	}
	
	anyErr := Gestalt(gestaltMachineType, machineType);
	IF (anyErr <> noErr) THEN
		EXIT(ExpressModemPresent);
		
	{ Check CPU type	}
	CASE machineType OF
		gestaltPortable,		{ For older PowerBooks, no 3615 Modem	}
		gestaltPowerBook170,
		gestaltPowerBook100,
		gestaltPowerBook140,
		gestaltPowerBook145:
			;
		
		gestaltDBLite,			{ If installed, must be 3615 for DBs	}
		gestaltDBLiteLC:
			IF (ModemStat(Byte(mdmStatus)) = noErr) THEN
				IF (BAND(mdmStatus,ModemInstalledMask) <> 0) THEN
					ExpressModemPresent := TRUE;
		
		OTHERWISE				{ we assume this patch is on all later machines	}
			BEGIN
				modemType := GetModemType;
				IF ((modemType = ModemTypeDUO) OR (modemType = ModemTypeDUOPlus)) THEN
					ExpressModemPresent := TRUE;
			END;
	END; { compatible CPUs	}		
END;

FUNCTION 	CRMGetToolIndResource(procID: INTEGER; theType: ResType; index: INTEGER): Handle; EXTERNAL;


{=====================================================================================================}
{
	This routine crms to make sure the specified tool is of the correct type .

	Returns FALSE if file is found and of incorrect type.

	Returns TRUE if file is found and of correct type.
	If file does NOT exist, TRUE is returned anyway
}
FUNCTION __CRMCheckTool(fName: STR63; fType: OSType;
					vRefNum: INTEGER; dirID: LONGINT) : Boolean;
VAR
	thePB:	HParamBlockRec;
	err:	OSErr;
BEGIN
	__CRMCheckTool := TRUE; 							{default}
	WITH thePB DO									{ set up parameter block }
	BEGIN
		ioCompletion := NIL;
		ioNamePtr := StringPtr(@fName);
		ioVRefNum := vRefNum;
		ioDirID := dirID;
		ioFRefNum := 0;
		ioFVersNum := 0;
		ioFDirIndex := 0;
	END;
	err := PBHGetFInfo(@thePB, FALSE);				{ get the file information }
	IF err = noErr THEN
		__CRMCheckTool := (thePB.ioFlFndrInfo.fdType = fType);	{ got a matched filetype }
END;


{=====================================================================================================}
{	this routine converts a path access reference number to a file name and a working directory
}
FUNCTION	__CRMParseRefNum(refNum: INTEGER; VAR fName: STR63; VAR vRefNum: INTEGER): OSErr;
													{ given file refnum, return fName and WD }
VAR
	err: 	OSErr;
	thePB:	FCBPBRec;

BEGIN
	WITH thePB DO									{ set up the parameter block }
	BEGIN
		ioCompletion := NIL;
		ioNamePtr := StringPtr(@fName);
		ioVRefNum := 0;
		ioFCBIndx := 0;
		ioRefNum := refNum;
	END;
	err := PBGetFCBInfo(@thePB, FALSE);				{ make the call }
	vRefNum := thePB.ioVRefNum;						{ return the working directory number }
	__CRMParseRefNum := err;							{ and any error resulting }
END;

TYPE
{	local/real resource ID data types	}
	equivalence		=	RECORD
		localID		:	INTEGER;
		realID		:	INTEGER;
	END;

	mapRecordPtr	=	^mapRecord;
	mapRecord		=	RECORD
		theType		:	ResType;
		count		:	INTEGER;
		equivs		:	ARRAY[0..0] OF equivalence;
	END;

	bundleHandle	=	^bundlePointer;
	bundlePointer	=	^bundleRecord;
	bundleRecord	=	RECORD
		numberMaps	:	INTEGER;
		mapList		:	ARRAY[0..0] OF mapRecord;
	END;

{=====================================================================================================}
FUNCTION CRMLocalToRealToLocalID( bundleType:ResType; toolID:INTEGER; theKind:ResType; theID:INTEGER; localToReal: BOOLEAN) : INTEGER;
VAR
	i,j				:	INTEGER;

	theHandle		:	Handle;
	thePtr			:	BundlePointer;
	mapPtr			:	MapRecordPtr;
	oldRF			:	INTEGER;
	testVal,
	retVal			: 	INTEGER;

LABEL
	1;

BEGIN
	CRMLocalToRealToLocalID := -1;

	oldRF := CurResFile;
	UseResFile(toolID);
	theHandle := Get1IndResource(bundleType, 1);		{ this loads it into memory if it is purged }
	UseResFile(oldRF);

	IF theHandle = NIL THEN
		Exit(CRMLocalToRealToLocalID);

	HNoPurge(theHandle);
	HLock(theHandle);

	thePtr := BundlePointer(theHandle^);

	WITH thePtr^ DO BEGIN
		mapPtr := @thePtr^.mapList[0];

		FOR i := numberMaps DOWNTO 0 DO
			WITH mapPtr^ DO BEGIN
				IF theType = theKind THEN

					FOR j:= count DOWNTO 0 DO
						WITH equivs[j] DO BEGIN
							testVal := realID;		{ assume RealToLocal	}
							retVal := localID;

							IF localToReal THEN BEGIN
								testVal := localID;
								retVal := realID;
							END;

							IF testVal = theID THEN BEGIN
								CRMLocalToRealToLocalID := retVal;
								GOTO 1;
							END;
						END; {with}

				mapPtr := MapRecordPtr(ORD4(mapPtr) +
									SIZEOF(resType) +
									SIZEOF(INTEGER) +
									SIZEOF(Equivalence) * (1 + count));
			END; {with}

	END; {with}
1:
	HUnlock(theHandle);
	HPurge(theHandle);
END;

FUNCTION __CRMLocalToRealID( bundleType:ResType; toolID:INTEGER;
										theKind:ResType; localID:INTEGER) : INTEGER;
BEGIN
	__CRMLocalToRealID := CRMLocalToRealToLocalID( bundleType, toolID, theKind, localID, TRUE);
END;

FUNCTION __CRMRealToLocalID( bundleType:ResType; toolID:INTEGER;
										theKind:ResType; realID:INTEGER) : INTEGER;
BEGIN
	__CRMRealToLocalID := CRMLocalToRealToLocalID( bundleType, toolID, theKind, realID, FALSE);
END;


FUNCTION __CRMIsDriverOpen(driverName: STR255): BOOLEAN;
CONST
	RamBased 	= $0080;
	Opened 		= $0040;

	dRAMBased 	= 6;
	dOpened 	= 5;

TYPE
	IntPtr = ^INTEGER;

	DriverHandle = ^DriverPtr;
	DriverPtr = ^DriverHeader;
	DriverHeader = RECORD
		drvrFlags : INTEGER;
		drvrDelay : INTEGER;
		drvrEMask : INTEGER;
		drvrMenu  : INTEGER;
		drvrOpen  : INTEGER;
		drvrPrime : INTEGER;
		drvrCtl   : INTEGER;
		drvrStatus: INTEGER;
		drvrClose : INTEGER;
		drvrName  : STR255;
	END;

VAR
	theDCT : 	DCtlHandle;
	index:		INTEGER;
	maxIndex :	INTEGER;
	theDriver : DriverPtr;

BEGIN
	__CRMIsDriverOpen := FALSE;						{ pessimism }

	FOR index := IntPtr(UnitNtryCnt)^ DOWNTO 1 DO BEGIN

		theDCT := GetDCtlEntry(-index);				{ get the DCT entry for given refnum }

		IF theDCT <> NIL THEN
			WITH theDCT^^ DO
				IF (dCtlDriver <> NIL) THEN BEGIN

					IF BTST(dCtlFlags, dRAMBased) THEN
						theDriver := DriverHandle(dCtlDriver)^
					ELSE
						theDriver := DriverPtr(dCtlDriver);

					{ EqualString will NOT move memory, so the bit test is safe	}
					{ case insens, diacrit sens 								}

					IF (theDriver <> NIL) THEN
						IF EqualString(theDriver^.drvrName, driverName, FALSE, TRUE) THEN BEGIN
							__CRMIsDriverOpen := BTST(dCtlFlags, dOpened);
							Exit(__CRMIsDriverOpen);
						END;

				END; {if good driver}

	END; {for}

END;

END.
