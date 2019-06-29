/*
	File:		ThingSearch.c

	Copyright:	© 1991, 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<10>	 6/13/93	JDR		Removed a SystemSevenOrLater define.
		 <9>	 6/13/93	JDR		Rolled in the ComponentMgr sources from QuickTime 1.6.
		 <8>	  2/6/92	JH		#1020142, calling ResolveAliasFile for an alias to a component
									on a file server protected by a password or on an unmounted
									floppy will crash on boot. Changed call to ResolveAliasFile to a
									call to our own HomeBrewed MatchAliasFile.  In MatchAliasFile we
									call MatchAlias with the no UI bit set for alias files.  If we
									get a nsvErr we just forget about it, and go on our merry way.
									If the filespec passed to MatchAliasFile is a regular file we
									just call ResolveAliasFile like the old way.
		 <7>	12/12/91	DTY		Take the spaces out of the Component Manager files.
		 <6>	 12/7/91	YK		Remove System6 conditionals. Re-write for System 7.
		 <5>	11/23/91	YK		Include private header insterd of Components.h.  Now
									ComponentSearch can be called via _ComponentDispatch.
		 <4>	10/30/91	YK		Modified ComponentSearch as pascal type function so that install
									proc can call it.
		 <3>	10/29/91	YK		Conditionalized code for System 6.  Replace them with System 7
									traps.
		 <2>	10/24/91	DTY		Nail some warnings.
		<13>	10/14/91	MD		search recursively in Extensions folder (and make smaller)
		<12>	 9/30/91	BAL		Now uses new wiley registerthingresourcefile routine.
		<11>	  7/7/91	BL		Remove compiler warnings
		<10>	  7/4/91	MD		No longer uses working directories for finding 'thng' files
									(fixes problems with later inits trying to access files)
		 <9>	 5/18/91	dvb		Thing->Component
		 <8>	  5/2/91	BAL		Updated to latest think interfaces
		 <7>	 3/14/91	JM		Remove MPWHeaders, create extensions string, clean up FS calls
		 <6>	 2/16/91	BAL		Updated to latest THINKC headers
		 <5>	  2/9/91	BAL		undefined testing so that thngs would auto-install again.
		 <4>	  2/8/91	JM		Implemented alias search for System 7

	To Do:
*/


#include	<Resources.h>
#include	<Memory.h>
#include 	<SysEqu.h>
#include 	<Aliases.h>
#include 	<Finder.h>
#include 	<Errors.h>
#include 	<Folders.h>

#undef tLINKED
#include "Components.h"

#undef testing
#ifdef testing
#define targetType 'cdev'
#define targetRsrc 'INIT'
#else
#define targetType 'thng'
#define targetRsrc 'thng'
#endif

#ifndef nil
  #define nil 0L
#endif

static void searchFolder(register CInfoPBRec *fInfo, FSSpec *fileSpec,
			long dirID, Boolean recurse);

long TestFile (FSSpecPtr fileSpec)
{
	short fRef;
	Boolean oldResLoad;
	short result = 0;

	oldResLoad = *(Boolean *)ResLoad;
	SetResLoad(false);

	fRef = FSpOpenResFile(fileSpec, fsRdPerm);
	if (ResError() == noErr)
		{
		result = RegisterComponentResourceFile(fRef, registerComponentGlobal);
		CloseResFile(fRef);
		}
	SetResLoad(oldResLoad);
	return (result > 0);
}

void doComponentSearch ()
{
	short		systemVRef;
	long		blessedFolder;
	CInfoPBRec	fInfo;
	long		extensionFolder;
	FSSpec		fileSpec;
	Str255		fName;

	if (FindFolder( kOnSystemDisk, kSystemFolderType, kDontCreateFolder, &systemVRef, &blessedFolder) == noErr) {

		fInfo.hFileInfo.ioVRefNum = systemVRef;
		fInfo.hFileInfo.ioNamePtr = fName;
		searchFolder(&fInfo, &fileSpec, blessedFolder, false);

		if (FindFolder(systemVRef, kExtensionFolderType, kDontCreateFolder, &systemVRef, &extensionFolder) == noErr)

			searchFolder(&fInfo, &fileSpec, extensionFolder, true);

		}
}

/*
	This rates among the most pathetic solutions to a system software problem ever
	created. We are building a whole system. How about just fixing the Alias Manager to
	know not to bring up a dialog if its a bad time? How about a check of WWExist for
	starters inside the top of MatchAlias? Perhaps it could set the no UI flag itself?
	Or maybe we should just pass this MatchAliasFile code around to all other clients
	of the buggy Alias Manager???
*/
///////////////////////////////////////////////////////////////////////////
//
// MatchAliasFile
//		First check to see if the incoming fileSpec is for an Alias file
//		If it is open the file, load the first resource ( Get1IndResource )
//		Detach that resource and then close the file
//		Once you have the resource use it in a MatchAlias with the NoUI bit set
//		Aliases for files on protected volumes or unmounted floppies will
//		cause a nsvErr to be returned.
//		
//		For other files just use ResolveAliasFile to fill out the spec
////////////////////////////////////////////////////////////////////////
static OSErr MatchAliasFile( FSSpec *fileSpec, FInfo *finderInfo, AliasHandle *alias, Boolean *isFolder)
{
	short			aliasCount = 1;
	FSSpec			foundSpec;
	Boolean			needsUpdate;
	OSErr			err;
	short			refNum;
	Boolean			wasAliased;
	
	if ( finderInfo->fdFlags & kIsAlias ) {		//is this an alias?  If so open the resource file, close it, and use MatchAlias with the no interaction bit set.
		
		refNum = FSpOpenResFile(fileSpec,fsRdPerm);						//open read only
		*alias = (AliasHandle)Get1IndResource(rAliasType, 1);			//just get the first one
		
		if ( *alias == nil ) {
			CloseResFile(refNum);
			return ResError();
		}
			
		DetachResource( (Handle)*alias );			//detach so we can close the file
		CloseResFile(refNum);
		
		err = MatchAlias( fileSpec,kARMSearch + kARMNoUI + kARMMountVol,*alias, &aliasCount,(FSSpecArrayPtr)&foundSpec,&needsUpdate,nil,nil);
		
		if ( err == fnfErr && aliasCount == 1 )		//according to IM VI if fndErr, but aliasCount == 1 it found the directory but not the file
			*isFolder == true;
			
		if ( err ) {
			return err;
		}
		
		*fileSpec = foundSpec;
		
	}
	else		//do it the old way
		ResolveAliasFile(fileSpec, true, isFolder, &wasAliased);
		
	return noErr;
}



static void searchFolder(register CInfoPBRec *fInfo, FSSpec *fileSpec,
			long dirID, Boolean recurse)
{
	short ix = 0;
	Boolean isFolder, wasAliased;

	while (true)
	{
		fInfo->hFileInfo.ioDirID = dirID;
		fInfo->hFileInfo.ioFDirIndex = ++ix;
		if (PBGetCatInfoSync(fInfo))
			break;

		if (!(fInfo->hFileInfo.ioFlAttrib & ioDirMask)) {
			if (fInfo->hFileInfo.ioFlFndrInfo.fdType == targetType) {
				fileSpec->vRefNum = fInfo->hFileInfo.ioVRefNum;
				fileSpec->parID = dirID;
				BlockMove(fInfo->hFileInfo.ioNamePtr, fileSpec->name, fInfo->hFileInfo.ioNamePtr[0]+1);
				isFolder = false;
				ResolveAliasFile(fileSpec, true, &isFolder, &wasAliased);
				if (!isFolder)
					TestFile(fileSpec);
			}
		} else if (recurse)
			searchFolder(fInfo, fileSpec, fInfo->dirInfo.ioDrDirID, true);
	}
}
