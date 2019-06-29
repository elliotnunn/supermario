/*
	File:		Finder.h	

	Contains:	File types of alias files and other Finder/system things.
	
	Copyright © Apple Computer, Inc. 1990-1991 
	All rights reserved

*/


#ifndef __FINDER__
#define __FINDER__

#ifndef __ALIASES__
#include <Aliases.h>			/* include ResolveAliasFile definition */
#endif

#ifndef __FOLDERS__
#include <Folders.h>			/* include FindFolder definition */
#endif

#ifndef __FILES__
#include <Files.h>				/* include Finder info and Desktop Database routines */
#endif

#ifndef __GESTALTEQU__
#include <GestaltEqu.h>			/* include gestalt selectors */
#endif

/* make only the following consts avaiable to resource files that include this file */

#define kCustomIconResource	-16455 			/* Custom icon family resource ID */

#define kContainerFolderAliasType	'fdrp'	/* type for folder aliases */
#define kContainerTrashAliasType 	'trsh'	/* type for trash folder aliases */
#define kContainerHardDiskAliasType 'hdsk'	/* type for hard disk aliases */
#define kContainerFloppyAliasType 	'flpy'	/* type for floppy aliases */
#define kContainerServerAliasType 	'srvr'	/* type for server aliases */
#define kApplicationAliasType 		'adrp'	/* type for application aliases */
#define kContainerAliasType 		'drop'	/* type for all other containers */

/* type for Special folder aliases */
#define kSystemFolderAliasType 		'fasy'
#define kAppleMenuFolderAliasType 	'faam'
#define kStartupFolderAliasType 	'fast'
#define kPrintMonitorDocsFolderAliasType 'fapn'
#define kPreferencesFolderAliasType 'fapf'
#define kControlPanelFolderAliasType 'fact'
#define kExtensionFolderAliasType 	'faex'

/* type for AppleShare folder aliases */
#define kExportedFolderAliasType	'faet'
#define kDropFolderAliasType 		'fadr'	
#define kSharedFolderAliasType 		'fash'	
#define kMountedFolderAliasType 	'famn'	

#endif __FINDER__



