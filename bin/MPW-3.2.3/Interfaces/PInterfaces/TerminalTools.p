{
	TerminalTools.p
	Pascal Interface to the Terminal Manager Tools.
	
	Copyright © Apple Computer, Inc.	1988-90
	All rights reserved
}

{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT TerminalTools;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingTerminalTools}
{$SETC UsingTerminalTools := 1}

{$I+}
{$SETC TerminalToolsIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}

{$IFC UNDEFINED UsingDialogs}
{$I $$Shell(PInterfaces)Dialogs.p}
{$ENDC}

{$IFC UNDEFINED UsingTerminals}
{$I $$Shell(PInterfaces)Terminals.p}
{$ENDC}

{$SETC UsingIncludes := TerminalToolsIncludes}

CONST
	tdefType			=	'tdef';
	tvalType			=	'tval';
	tsetType			=	'tset';
	tlocType			=	'tloc';
	tscrType			=	'tscr';
	tbndType			=	'tbnd';	
	tverType			=	'vers';
	
{ messages }
	tmInitMsg 			= 	0;
	tmDisposeMsg 		= 	1;
	tmSuspendMsg		= 	2;
	tmResumeMsg			= 	3;
	tmMenuMsg			=	4;
	tmEventMsg			=	5;
	tmActivateMsg		= 	6;
	tmDeactivateMsg		= 	7;
	tmGetErrorStringMsg	=	8;
	
	tmIdleMsg			=	50;
	tmResetMsg			=	51;

	tmKeyMsg			= 	100;
	tmStreamMsg			= 	101;
	tmResizeMsg			= 	102;
	tmUpdateMsg			=	103;
	tmClickMsg			=	104;
	tmGetSelectionMsg	=	105;
	tmSetSelectionMsg	=	106;
	tmScrollMsg			=	107;
	tmClearMsg			=	108;
	tmGetLineMsg		=	109;
	tmPaintMsg			=	110;
	tmCursorMsg			=	111;
	tmGetEnvironsMsg	=	112;
	tmDoTermKeyMsg		=	113;
	tmCountTermKeysMsg	=	114;
	tmGetIndTermKeyMsg	=	115;
	
{	messages for validate DefProc				}

	tmValidateMsg		=	0;
	tmDefaultMsg		=	1;
	
{	messages for Setup DefProc				}

	tmSpreflightMsg		=	0;
	tmSsetupMsg			=	1;
	tmSitemMsg			=	2;
	tmSfilterMsg		=	3;
	tmScleanupMsg		=	4;
	
{	messages for scripting defProc 			}
	tmMgetMsg			=	0;
	tmMsetMsg			=	1;
	
{	messages for localization defProc		}

	tmL2English			=	0;
	tmL2Intl			=	1;
	
	
TYPE

	TMSearchBlockPtr	=	^TMSearchBlock;
	TMSearchBlock		=	RECORD
		theString 	:	StringHandle;
		where		:	Rect;
		searchType	:	TMSearchTypes;
		callBack	:	ProcPtr;
		refnum		:	INTEGER;
		next		:	TMSearchBlockPtr;
	END;
	
	TMSetupPtr		=	^TMSetupStruct;
	TMSetupStruct	=	RECORD
		theDialog	:	DialogPtr;
		count		:	INTEGER;
		theConfig	:	Ptr;
		procID		:	INTEGER;		{ procID of the tool	}
	END;
	
{$ENDC} {UsingTerminalTools}

{$IFC NOT UsingIncludes}
	END.
{$ENDC}


