{
Created Tuesday, May 10 1989 at 6:00 AM
	CRMSerialDevices.p
	Pascal Interface to the Communications Resource Manager Serial Device Utilities
	
	Copyright Apple Computer, Inc.	 1988-90
	All rights reserved
}
{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT CRMSerialDevices;
	INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingCRMSerialDevices}
{$SETC UsingCRMSerialDevices := 1}

{$I+}
{$SETC CRMSerialDevicesIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}

{$SETC UsingIncludes := CRMSerialDevicesIncludes}

CONST
 { crmDeviceType}
	crmSerialDevice		=	1;
					
	curCRMSerRecVers 	= 	1;


TYPE
	CRMIconHandle	= ^CRMIconPtr;
	CRMIconPtr		= ^CRMIconRecord;
	CRMIconRecord	= RECORD
		oldIcon		: ARRAY [0..31] OF LONGINT;		{ 'ICN#' data & mask	}
		oldMask		: ARRAY [0..31] OF LONGINT;
		theSuite	: Handle;						{ IconSuite handle		}
		reserved	: LONGINT;
	END;

	CRMSerialPtr = ^CRMSerialRecord;
	CRMSerialRecord = RECORD
		version				: INTEGER;
		inputDriverName		: StringHandle;
		outputDriverName	: StringHandle;
		name				: StringHandle;
		deviceIcon			: CRMIconHandle;
		ratedSpeed			: LONGINT;
		maxSpeed			: LONGINT;
		reserved			: LONGINT;
	END;

{$ENDC} {UsingCRMSerialDevices}

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

