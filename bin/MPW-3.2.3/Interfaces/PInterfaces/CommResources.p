{
	CommResources.p
	Pascal Interface to the Communications Resource Manager
	
	Copyright © Apple Computer, Inc. 1988-90
	All rights reserved
}
{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT CommResources;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingCommResources}
{$SETC UsingCommResources := 1}

{$I+}
{$SETC CommResourcesIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingOSUtils}
{$I $$Shell(PInterfaces)OSUtils.p}
{$ENDC}

{$SETC UsingIncludes := CommResourcesIncludes}

CONST
{	current version of the CRM }
	curCRMVersion 	= 	2;
	

{	tool classes (also the tool file types)	}
	classCM 		=	'cbnd';
	classFT 		=	'fbnd';
	classTM			=	'tbnd';
	
{	constants general to the use of the Communications Resource Manager }
	crmType 		= 	9;				{queueType}
	crmRecVersion	= 	1;				{version of queue data structure}

{	error codes }
	crmGenericError	=	-1;
	crmNoErr		=	0;
	
TYPE
{	data structures general to the use of the Communications Resource Manager }
	CRMErr			=	OSErr;

	CRMRecPtr 		= ^CRMRec;
	CRMRec 			= RECORD
		qLink: 			QElemPtr;  	{reserved}
		qType: 			INTEGER;  	{queue type -- ORD(crmType) = 9}
		crmVersion: 	INTEGER;    {version of queue element data structure}
		crmPrivate: 	LONGINT;    {reserved}
		crmReserved: 	INTEGER;   	{reserved}
		
		crmDeviceType: 	LONGINT;	{type of device, assigned by DTS}
		crmDeviceID: 	LONGINT;	{device ID; assigned when CRMInstall is called}
		crmAttributes: 	LONGINT;	{pointer to attribute block}
									{device specific}
		crmStatus: 		LONGINT;	{status variable - device specific}

		crmRefCon: 		LONGINT;	{for device private use}
		END;

FUNCTION 	InitCRM : CRMErr;
FUNCTION 	CRMGetHeader: QHdrPtr;
PROCEDURE 	CRMInstall(crmReqPtr: QElemPtr);
FUNCTION 	CRMRemove(crmReqPtr: QElemPtr): OSErr;
FUNCTION 	CRMSearch(crmReqPtr: QElemPtr): QElemPtr;	
FUNCTION 	CRMGetCRMVersion : INTEGER;

FUNCTION 	CRMGetResource(theType : ResType; theID : INTEGER) : Handle;
FUNCTION 	CRMGet1Resource(theType : ResType; theID : INTEGER) : Handle;
FUNCTION 	CRMGetIndResource(theType : ResType; index : INTEGER) : Handle;
FUNCTION 	CRMGet1IndResource(theType : ResType; index : INTEGER) : Handle;
FUNCTION 	CRMGetNamedResource(theType : ResType; name : Str255) : Handle;
FUNCTION 	CRMGet1NamedResource(theType : ResType; name : Str255) : Handle;
PROCEDURE 	CRMReleaseResource(theHandle : Handle);
FUNCTION 	CRMGetIndex(theHandle: Handle): LONGINT;

FUNCTION 	CRMLocalToRealID( bundleType:ResType; 
					toolID:INTEGER; theKind:ResType; localID:INTEGER) : INTEGER;
FUNCTION 	CRMRealToLocalID( bundleType:ResType; 
					toolID:INTEGER; theKind:ResType; realID:INTEGER) : INTEGER;
					
FUNCTION	CRMGetIndToolName(bundleType: OSType; index: INTEGER; VAR toolName: Str255): OSErr;

FUNCTION	CRMFindCommunications(VAR vRefNum: INTEGER; VAR dirID: LONGINT): OSErr;

FUNCTION 	CRMGetToolResource(procID: INTEGER; theType: ResType; theID: INTEGER): Handle;
PROCEDURE 	CRMReleaseToolResource(procID: INTEGER; theHandle: Handle);

{$ENDC} {UsingCommResources}

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

