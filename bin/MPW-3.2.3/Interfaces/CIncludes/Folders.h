/************************************************************

Created: Sunday, January 6, 1991 at 9:25 PM
    Folders.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1989-90
        All rights reserved

************************************************************/


#ifndef __FOLDERS__
#define __FOLDERS__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif


enum {

    kOnSystemDisk = 0x8000


#define kCreateFolder true
#define kDontCreateFolder false

#define kSystemFolderType 'macs'                /*the system folder*/
#define kDesktopFolderType 'desk'               /*the desktop folder; objects in this folder show on the desk top.*/
#define kTrashFolderType 'trsh'                 /*the trash folder; objects in this folder show up in the trash*/
#define kWhereToEmptyTrashFolderType 'empt'     /*the "empty trash" folder; Finder starts empty from here down*/

#define kPrintMonitorDocsFolderType 'prnt'      /* Print Monitor documents */

#define kStartupFolderType 'strt'               /*Finder objects (applications, documents, DAs, aliases, to...) to open at startup go here*/
#define kAppleMenuFolderType 'amnu'             /*Finder objects to put into the Apple menu go here*/
#define kControlPanelFolderType 'ctrl'          /*Control Panels go here (may contain INITs)*/
#define kExtensionFolderType 'extn'             /*Finder extensions go here*/

#define kPreferencesFolderType 'pref'           /*preferences for applications go here*/
#define kTemporaryFolderType 'temp'             /*temporary files go here (deleted periodically, but don't rely on it.)*/
};

#ifdef __cplusplus
extern "C" {
#endif
#if SystemSevenOrLater
pascal OSErr FindFolder(short vRefNum,OSType folderType,Boolean createFolder,
    short *foundVRefNum,long *foundDirID)
    = {0x7000,0xA823}; 
#else
pascal OSErr FindFolder(short vRefNum,OSType folderType,Boolean createFolder,
    short *foundVRefNum,long *foundDirID);
#endif
#ifdef __cplusplus
}
#endif

#endif
