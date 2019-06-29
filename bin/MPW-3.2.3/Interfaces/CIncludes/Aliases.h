/************************************************************

Created: Monday, January 28, 1991 at 12:55 PM
    Aliases.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1989-1990
        All rights reserved

************************************************************/


#ifndef __ALIASES__
#define __ALIASES__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __APPLETALK__
#include <AppleTalk.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif


enum {


#define rAliasType 'alis'                                           /* Aliases are stored as resources of this type */

/* define alias resolution action rules mask */

    kARMMountVol = 0x00000001,                                      /* mount the volume automatically */
    kARMNoUI = 0x00000002,                                          /* no user interface allowed during resolution */
    kARMMultVols = 0x00000008,                                      /* search on multiple volumes */
    kARMSearch = 0x00000100,                                        /* search quickly */
    kARMSearchMore = 0x00000200,                                    /* search further */
    kARMSearchRelFirst = 0x00000400,                                /* search target on a relative path first */

/* define alias record information types */
    asiZoneName = -3,                                               /* get zone name */
    asiServerName = -2,                                             /* get server name */
    asiVolumeName = -1,                                             /* get volume name */
    asiAliasName = 0,                                               /* get aliased file/folder/volume name */
    asiParentName = 1                                               /* get parent folder name */

};

/* define the alias record that will be the blackbox for the caller */
struct AliasRecord {
    OSType userType;                                                /* appl stored type like creator type */
    unsigned short aliasSize;                                       /* alias record size in bytes, for appl usage */
};

typedef struct AliasRecord AliasRecord;
typedef AliasRecord *AliasPtr, **AliasHandle;


typedef short AliasInfoType;                                        /* alias record information type */
typedef pascal Boolean (*AliasFilterProcPtr) (CInfoPBPtr cpbPtr,    /*I*/
Boolean *quitFlag,                                                  /*O*/
Ptr yourDataPtr);                                                   /*I*/

#ifdef __cplusplus
extern "C" {
#endif
/*  create a new alias between fromFile-target and return alias record handle  */
pascal OSErr NewAlias(const FSSpec *fromFile,
                      const FSSpec *target,
                      AliasHandle *alias)
    = {0x7002,0xA823}; 
/* create a minimal new alias for a target and return alias record handle */
pascal OSErr NewAliasMinimal(const FSSpec *target,
                             AliasHandle *alias)
    = {0x7008,0xA823}; 
/* create a minimal new alias from a target fullpath (optional zone and server name) and return alias record handle  */
pascal OSErr NewAliasMinimalFromFullPath(short fullPathLength,
                                         const unsigned char *fullPath,
                                         ConstStr32Param zoneName,
                                         ConstStr31Param serverName,
                                         AliasHandle *alias)
    = {0x7009,0xA823}; 
#define NewAliasMinimalFromFullpath NewAliasMinimalFromFullPath

/* given an alias handle and fromFile, resolve the alias, update the alias record and return aliased filename and wasChanged flag. */
pascal OSErr ResolveAlias(const FSSpec *fromFile,
                          AliasHandle alias,
                          FSSpec *target,
                          Boolean *wasChanged)
    = {0x7003,0xA823}; 
/* given an alias handle and an index specifying requested alias information type, return the information from alias record as a string. */
pascal OSErr GetAliasInfo(const AliasHandle alias,
                          AliasInfoType index,
                          Str63 theString)
    = {0x7007,0xA823}; 

/* 
     given a file spec, return target file spec if input file spec is an alias.
        It resolves the entire alias chain or one step of the chain.  It returns
        info about whether the target is a folder or file; and whether the input
        file spec was an alias or not. 
*/

pascal OSErr ResolveAliasFile(FSSpec *theSpec,
                              Boolean resolveAliasChains,
                              Boolean *targetIsFolder,
                              Boolean *wasAliased)
    = {0x700C,0xA823}; 

/* 
      Low Level Routines 
    given an alias handle and fromFile, match the alias and return aliased filename(s) and needsUpdate flag
*/

pascal OSErr MatchAlias(const FSSpec *fromFile,
                        unsigned long rulesMask,
                        const AliasHandle alias,
                        short *aliasCount,
                        FSSpecArrayPtr aliasList,
                        Boolean *needsUpdate,
                        AliasFilterProcPtr aliasFilter,
                        void *yourDataPtr)
    = {0x7005,0xA823}; 
/* given a fromFile-target pair and an alias handle, update the lias record pointed to by alias handle to represent target as the new alias. */
pascal OSErr UpdateAlias(const FSSpec *fromFile,
                         const FSSpec *target,
                         AliasHandle alias,
                         Boolean *wasChanged)
    = {0x7006,0xA823}; 
#ifdef __cplusplus
}
#endif

#endif
