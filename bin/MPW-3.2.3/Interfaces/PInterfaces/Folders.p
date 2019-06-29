{
Created: Sunday, January 6, 1991 at 10:43 PM
    Folders.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1989-90
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Folders;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingFolders}
{$SETC UsingFolders := 1}

{$I+}
{$SETC FoldersIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingFiles}
{$I $$Shell(PInterfaces)Files.p}
{$ENDC}
{$SETC UsingIncludes := FoldersIncludes}

CONST
kOnSystemDisk = $8000;

kCreateFolder = TRUE;
kDontCreateFolder = FALSE;

kSystemFolderType = 'macs';             {the system folder}
kDesktopFolderType = 'desk';            {the desktop folder; objects in this folder show on the desk top.}
kTrashFolderType = 'trsh';              {the trash folder; objects in this folder show up in the trash}
kWhereToEmptyTrashFolderType = 'empt';  {the "empty trash" folder; Finder starts empty from here down}

kPrintMonitorDocsFolderType = 'prnt';   { Print Monitor documents }

kStartupFolderType = 'strt';            {Finder objects (applications, documents, DAs, aliases, to...) to open at startup go here}
kAppleMenuFolderType = 'amnu';          {Finder objects to put into the Apple menu go here}
kControlPanelFolderType = 'ctrl';       {Control Panels go here (may contain INITs)}
kExtensionFolderType = 'extn';          {Finder extensions go here}

kPreferencesFolderType = 'pref';        {preferences for applications go here}
kTemporaryFolderType = 'temp';          {temporary files go here (deleted periodically, but don't rely on it.)}

FUNCTION FindFolder(vRefNum: INTEGER;folderType: OSType;createFolder: BOOLEAN;
    VAR foundVRefNum: INTEGER;VAR foundDirID: LONGINT): OSErr;
{$IFC SystemSevenOrLater }
    INLINE $7000,$A823;
{$ENDC}


{$ENDC}    { UsingFolders }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

