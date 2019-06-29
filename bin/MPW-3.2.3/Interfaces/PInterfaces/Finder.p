{
	File:		Finder.p	

	Contains:	File types of alias files and other Finder/system things.
	
				
 	Copyright © Apple Computer, Inc. 1990-1991 
	All rights reserved

}

{$IFC UNDEFINED UsingIncludes}
	{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT Finder;
	INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingFinder}
{$SETC UsingFinder := 1}

{$I+}
{$SETC FinderIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingAliases}
{$I $$Shell(PInterfaces)Aliases.p}			{ include ResolveAliasFile definition }
{$ENDC}
{$IFC UNDEFINED UsingFolders}
{$I $$Shell(PInterfaces)Folders.p}			{ include FindFolder definition }
{$ENDC}
{$IFC UNDEFINED UsingFiles}
{$I $$Shell(PInterfaces)Files.p}			{ include Finder info and Desktop Database routines }
{$ENDC}
{$IFC UNDEFINED UsingGestaltEqu}
{$I $$Shell(PInterfaces)GestaltEqu.p}		{ include gestalt selectors }
{$ENDC}
{$SETC UsingIncludes := FinderIncludes}


CONST
 	kCustomIconResource				=		-16455;		{ Custom icon family resource ID }

 	kContainerFolderAliasType		=		'fdrp';		{ type for folder aliases }
 	kContainerTrashAliasType 		=		'trsh';		{ type for trash folder aliases }
 	kContainerHardDiskAliasType 	=		'hdsk';		{ type for hard disk aliases }
 	kContainerFloppyAliasType 		=		'flpy';		{ type for floppy aliases }
 	kContainerServerAliasType 		=		'srvr';		{ type for server aliases }
 	kApplicationAliasType 			=		'adrp';		{ type for application aliases }
 	kContainerAliasType 			=		'drop';		{ type for all other containers }

	{ type for Special folder aliases }
 	kSystemFolderAliasType 			=		'fasy';
 	kAppleMenuFolderAliasType 		=		'faam';
 	kStartupFolderAliasType 		=		'fast';
 	kPrintMonitorDocsFolderAliasType =		'fapn';
 	kPreferencesFolderAliasType 	=		'fapf';
 	kControlPanelFolderAliasType 	=		'fact';
 	kExtensionFolderAliasType 		=		'faex';

	{ type for AppleShare folder aliases }
	kExportedFolderAliasType		=		'faet';
 	kDropFolderAliasType 			=		'fadr';	
 	kSharedFolderAliasType 			=		'fash';	
 	kMountedFolderAliasType 		=		'famn';	


{$ENDC}    { UsingFinder }

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

