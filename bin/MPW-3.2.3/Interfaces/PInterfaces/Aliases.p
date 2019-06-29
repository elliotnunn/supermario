{
Created: Monday, January 28, 1991 at 1:26 PM
    Aliases.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1989-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Aliases;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingAliases}
{$SETC UsingAliases := 1}

{$I+}
{$SETC AliasesIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$IFC UNDEFINED UsingDialogs}
{$I $$Shell(PInterfaces)Dialogs.p}
{$ENDC}
{$IFC UNDEFINED UsingAppleTalk}
{$I $$Shell(PInterfaces)AppleTalk.p}
{$ENDC}
{$IFC UNDEFINED UsingFiles}
{$I $$Shell(PInterfaces)Files.p}
{$ENDC}
{$SETC UsingIncludes := AliasesIncludes}

CONST
rAliasType = 'alis';                { Aliases are stored as resources of this type }

{ define alias resolution action rules mask }
kARMMountVol = $00000001;           { mount the volume automatically }
kARMNoUI = $00000002;               { no user interface allowed during resolution }
kARMMultVols = $00000008;           { search on multiple volumes }
kARMSearch = $00000100;             { search quickly }
kARMSearchMore = $00000200;         { search further }
kARMSearchRelFirst = $00000400;     { search target on a relative path first }

{ define alias record information types }
asiZoneName = -3;                   { get zone name }
asiServerName = -2;                 { get server name }
asiVolumeName = -1;                 { get volume name }
asiAliasName = 0;                   { get aliased file/folder/volume name }
asiParentName = 1;                  { get parent folder name }


TYPE
{ define the alias record that will be the blackbox for the caller }
AliasPtr = ^AliasRecord;
AliasHandle = ^AliasPtr;
AliasRecord = RECORD
    userType: OSType;               { appl stored type like creator type }
    aliasSize: INTEGER;             { alias record size in bytes, for appl usage }
    END;


AliasInfoType = INTEGER;            { alias record information type }
AliasFilterProcPtr = ProcPtr;


{  create a new alias between fromFile-target and return alias record handle  }
FUNCTION NewAlias(fromFile: FSSpecPtr;
                  target: FSSpec;
                  VAR alias: AliasHandle): OSErr;
    INLINE $7002,$A823;

{ create a minimal new alias for a target and return alias record handle }
FUNCTION NewAliasMinimal(target: FSSpec;
                         VAR alias: AliasHandle): OSErr;
    INLINE $7008,$A823;

{ create a minimal new alias from a target fullpath (optional zone and server name) and return alias record handle  }
FUNCTION NewAliasMinimalFromFullPath(fullPathLength: INTEGER;
                                     fullPath: Ptr;
                                     zoneName: Str32;
                                     serverName: Str31;
                                     VAR alias: AliasHandle): OSErr;
    INLINE $7009,$A823;

{ given an alias handle and fromFile, resolve the alias, update the alias record and return aliased filename and wasChanged flag. }
FUNCTION ResolveAlias(fromFile: FSSpecPtr;
                      alias: AliasHandle;
                      VAR target: FSSpec;
                      VAR wasChanged: BOOLEAN): OSErr;
    INLINE $7003,$A823;

{ given an alias handle and an index specifying requested alias information type, return the information from alias record as a string. }
FUNCTION GetAliasInfo(alias: AliasHandle;
                      index: AliasInfoType;
                      VAR theString: Str63): OSErr;
    INLINE $7007,$A823;

{  given a file spec, return target file spec if input file spec is an alias.
    It resolves the entire alias chain or one step of the chain.  It returns
    info about whether the target is a folder or file; and whether the input
    file spec was an alias or not.  }
FUNCTION ResolveAliasFile(VAR theSpec: FSSpec;
                          resolveAliasChains: BOOLEAN;
                          VAR targetIsFolder: BOOLEAN;
                          VAR wasAliased: BOOLEAN): OSErr;
    INLINE $700C,$A823;

{   Low Level Routines 
given an alias handle and fromFile, match the alias and return aliased filename(s) and needsUpdate flag }
FUNCTION MatchAlias(fromFile: FSSpecPtr;
                    rulesMask: LONGINT;
                    alias: AliasHandle;
                    VAR aliasCount: INTEGER;
                    aliasList: FSSpecArrayPtr;
                    VAR needsUpdate: BOOLEAN;
                    aliasFilter: AliasFilterProcPtr;
                    yourDataPtr: UNIV Ptr): OSErr;
    INLINE $7005,$A823;

{ given a fromFile-target pair and an alias handle, update the lias record pointed to by alias handle to represent target as the new alias. }
FUNCTION UpdateAlias(fromFile: FSSpecPtr;
                     target: FSSpec;
                     alias: AliasHandle;
                     VAR wasChanged: BOOLEAN): OSErr;
    INLINE $7006,$A823;


{$ENDC}    { UsingAliases }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

