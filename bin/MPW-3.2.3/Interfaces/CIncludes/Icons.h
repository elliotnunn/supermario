/*
	File:		Icons.h

	Contains:	Public C Interfaces for color icon plotting routines

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

*/

#ifndef __ICONS__
#define __ICONS__

/* The following are icons for which there are both icon suites and SICNs. */

#define genericDocumentIconResource			-4000
#define genericStationeryIconResource		-3985
#define genericEditionFileIconResource		-3989
#define genericApplicationIconResource		-3996
#define genericDeskAccessoryIconResource	-3991

#define genericFolderIconResource			-3999
#define privateFolderIconResource			-3994

#define floppyIconResource					-3998
#define trashIconResource					-3993

/* The following are icons for which there are SICNs only. */

#define desktopIconResource					-3992
#define openFolderIconResource				-3997
#define genericHardDiskIconResource			-3995
#define genericFileServerIconResource		-3972
#define genericSuitcaseIconResource			-3970
#define genericMoverObjectIconResource		-3969

/* The following are icons for which there are icon suites only. */

#define genericPreferencesIconResource		-3971
#define genericQueryDocumentIconResource	-16506
#define genericExtensionIconResource		-16415

#define systemFolderIconResource			-3983	
#define appleMenuFolderIconResource			-3982
#define startupFolderIconResource			-3981
#define ownedFolderIconResource				-3980
#define dropFolderIconResource				-3979
#define sharedFolderIconResource			-3978
#define mountedFolderIconResource			-3977
#define controlPanelFolderIconResource		-3976
#define printMonitorFolderIconResource		-3975
#define preferencesFolderIconResource		-3974
#define extensionsFolderIconResource		-3973

#define fullTrashIconResource				-3984

#define large1BitMask	'ICN#'
#define large4BitData	'icl4'
#define large8BitData	'icl8'
#define small1BitMask	'ics#'
#define small4BitData	'ics4'
#define small8BitData	'ics8'
#define mini1BitMask	'icm#'
#define mini4BitData	'icm4'
#define mini8BitData	'icm8'


#endif