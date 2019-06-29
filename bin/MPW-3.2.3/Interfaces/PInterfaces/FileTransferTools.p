{
	FileTransferTools.p
	Pascal Interface to the Connection Manager Tools.
	
	Copyright © Apple Computer, Inc.	1988-90
	All rights reserved
}

{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT FileTransferTools;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingFileTransferTools}
{$SETC UsingFileTransferTools := 1}

{$I+}
{$SETC FileTransferToolsIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}

{$IFC UNDEFINED UsingDialogs}
{$I $$Shell(PInterfaces)Dialogs.p}
{$ENDC}

{$IFC UNDEFINED UsingFileTransfers}
{$I $$Shell(PInterfaces)FileTransfers.p}
{$ENDC}

{$SETC UsingIncludes := FileTransferToolsIncludes}
CONST
{	messages for fDEFs		}
	ftInitMsg			=	0;
	ftDisposeMsg		=	1;
	ftSuspendMsg		=	2;
	ftResumeMsg			=	3;
	ftMenuMsg			=	4;
	ftEventMsg			=	5;
	ftActivateMsg		=	6;
	ftDeactivateMsg		=	7;
	ftGetErrorStringMsg	=	8;
	
	ftAbortMsg			=	52;

	ftStartMsg			=	100;
	ftExecMsg			=	102;

 { messages for validate DefProc				}

	ftValidateMsg		=	0;
	ftDefaultMsg		=	1;
	
{	messages for Setup DefProc				}

	ftSpreflightMsg		=	0;
	ftSsetupMsg			=	1;
	ftSitemMsg			=	2;
	ftSfilterMsg		=	3;
	ftScleanupMsg		=	4;
	
{	messages for scripting defProc 			}
	ftMgetMsg			=	0;
	ftMsetMsg			=	1;
	
{	messages for localization defProc		}

	ftL2English			=	0;
	ftL2Intl			=	1;

{ def type }

	fsetType			=	'fset';
	fvalType			= 	'fval';
	flocType			=	'floc';
	fscrType			=	'fscr';
	fdefType			=	'fdef';
	fbndType			=	'fbnd';
	fverType			=	'vers';
		
		
TYPE	
	FTSetupPtr		=	^FTSetupStruct;
	FTSetupStruct	=	PACKED RECORD
		theDialog	:	DialogPtr;
		count		:	INTEGER;
		theConfig	:	Ptr;
		procID		:	INTEGER;
	END;
	
{$ENDC} {UsingFileTransferTools}

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

