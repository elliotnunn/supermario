/*
	File:		ThingFiles.c

	Copyright:	© 1991-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM4>	  8/5/93	JDR		integrate Component Mgr from Reality
		 <8>	 6/13/93	JDR		Rolled in the ComponentMgr sources from QuickTime 1.6.
		 <7>	 5/29/92	DCL		Included TextUtils.h. RelString moved there for the New Inside
									Mac.
		 <6>	12/12/91	DTY		#1018054: If _HomeResFile returns 0 in _FSSpecFromResource, use
									the file reference number in SysMap instead.
		 <5>	 12/6/91	YK		Removed System6 conditionals. Object compare identical to <4>.
		 <4>	 12/5/91	YK		Rolled in QuickTime GM:
		 <3>	 11/6/91	YK		Corrected comments.  I misunderstood revision number.
		 <2>	 11/6/91	YK		Replaced code with System 7 trap.  Added code for 'thng' in
									System file.  Removed unnecessary include statement.

		 <3>	 5/18/91	dvb		Thing->Component
		 <2>	  5/2/91	BAL		Updated to latest think headers
		 <1>	 3/28/91	BAL		first checked in

	To Do:
*/

#include <Files.h>
#include <Resources.h>
#include <Memory.h>
#include <SysEqu.h>
#include <TextUtils.h>
#include <Errors.h>
#include <Icons.h>

#include "PrivateThingManager.h"


OSErr GrowRTFileTable(short growCount);

OSErr _FSSpecFromResource( Handle h, FSSpec *fileSpec )
{
	OSErr error = 0;
	FCBPBRec anFCB;
	Str255 fName;
	short fref;

	fref = HomeResFile( h );
	if (fref < 0) return ResError();

	if (fref == 0)										// <6> Convert a reference number
		fref = *(short *)SysMap;						// <6> of 0 to the system files’ refNum.

	anFCB.ioCompletion = 0;
	anFCB.ioVRefNum = 0;
	anFCB.ioRefNum = fref;
	anFCB.ioFCBIndx = 0;
	fName[0] = 0;
	anFCB.ioNamePtr = (StringPtr)fName;

	if (error = PBGetFCBInfoSync( &anFCB )) goto bail;

	error = FSMakeFSSpec( anFCB.ioFCBVRefNum, anFCB.ioFCBParID, anFCB.ioNamePtr, fileSpec);

bail:
	return (error);
}



Boolean FSSpecEqual( register FSSpec *fileSpecA, register FSSpec *fileSpecB )
{
	return ((fileSpecA->vRefNum == fileSpecB->vRefNum)
		  && (fileSpecA->parID == fileSpecB->parID)
		  && !RelString(fileSpecA->name,fileSpecB->name,false,true));
}




short AddComponentFSSpec( FSSpec *fspec )
{
	register rtFile *rtFileList = ComponentManagerGlobals->rtFileTable;
	register short i, empty = -1;
	register short count = ComponentManagerGlobals->rtFileTotalCount;
	HParamBlockRec pb;
	OSErr err = -1;

	// get the file's id
	pb.fidParam.ioNamePtr = fspec->name;
	pb.fidParam.ioVRefNum = fspec->vRefNum;
	pb.fidParam.ioSrcDirID = fspec->parID;
	err = PBCreateFileIDRefSync(&pb);
	if (err == fidExists) err = noErr;

	/* first check to see if it's already there */

	for (i=0; i<count; i++) {
		if (rtFileList->referenceCount)		/* is this spot in use? */
			{
			if (rtFileList->vRefNum == 0)
				{
				// this entry contains an FSSpec handle, not a file id
				if ( FSSpecEqual(*(FSSpec **)rtFileList->fileID, fspec) )
					{
foundIndex:
				rtFileList->referenceCount++;
				return i;
				}
			}
		else
				{
				// this entry contains a file id
				if ((fspec->vRefNum == rtFileList->vRefNum) &&
					(pb.fidParam.ioFileID == rtFileList->fileID))
					goto foundIndex;
				}
			}
		else
			empty = i;
	rtFileList++;
	}

	/* didn't find it so add it to an empty spot */

	if (empty < 0) {
		// need to grow the table. no empty spot was available.
		if (GrowRTFileTable( kComponentFileAllocationSize ))
			return empty;								/* error: could not allocate a file entry */

		empty = ComponentManagerGlobals->rtFileTotalCount - 1;		// last entry should always be free
	}

	rtFileList = ComponentManagerGlobals->rtFileTable + empty;
	if (err) {
		// need to make a copy of the FSSpec and save it away
		rtFileList->fileID = (Handle)NewHandleSys(0);
		if (rtFileList->fileID) {
			PtrAndHand((Ptr)fspec, (Handle)rtFileList->fileID, sizeof(FSSpec));
			if (MemError() == noErr) {
				rtFileList->vRefNum = 0;
				goto rememberFile;
			}
		}
		empty = -1;
	}
	else {
		// just save away the file id
		rtFileList->vRefNum = fspec->vRefNum;
		rtFileList->fileID = pb.fidParam.ioFileID;
rememberFile:
		rtFileList->referenceCount = 1;
		ComponentManagerGlobals->rtFileUsedCount++;
	}

	return empty;
}



OSErr GrowRTFileTable( short growCount )
/*
 * Grow the table of registered thing files.
 */
	{
	register rtFile *aTable;
	rtFile *oldTable, *newTable;
	long oldTableSize;

	oldTableSize = ComponentManagerGlobals->rtFileTotalCount * sizeof(rtFile);
	oldTable = ComponentManagerGlobals->rtFileTable;
	newTable = aTable = (rtFile *)NewPtrSysClear( oldTableSize  + (long)growCount * sizeof(rtFile));
	if (!aTable)
		return MemError();

	BlockMove((Ptr)oldTable, (Ptr)aTable, oldTableSize);

	ComponentManagerGlobals->rtFileTable = newTable;			/* begin using new table */
	ComponentManagerGlobals->rtFileTotalCount += growCount;
	if (oldTable)
		DisposPtr((Ptr)oldTable);								/* dump the old one */
	return 0;
	}


rtFileNum AddComponentResFile( Handle h )
{
	FSSpec fspec;

	if (_FSSpecFromResource( h, &fspec ))
		return -1;

	return AddComponentFSSpec(&fspec);
}


OSErr RemoveComponentResFile( rtFileNum fn )
{
	register rtFile *rtFileList;

	/* first check to see if it's a valid file number */

	if ((fn < 0) || (fn >= ComponentManagerGlobals->rtFileTotalCount))
		return -1;

	rtFileList = ComponentManagerGlobals->rtFileTable + fn;
	if (!--rtFileList->referenceCount)
		{
		// since the reference count went to zero, this spot is free
		if (rtFileList->vRefNum == 0)
			{
			// the file id was really a handle to an FSSpec, so toss it
			DisposHandle((Handle)rtFileList->fileID);
			}
		ComponentManagerGlobals->rtFileUsedCount--;
		}

	return 0;
}

typedef struct getIconRecord {
	Component	c;
	short		resID;
} getIconRecord;

static pascal OSErr GetIconFromResource(ResType theType, Handle *theIcon, getIconRecord *refCon)
{
	OSErr err;
	Handle h;

	if (!refCon->resID)
		h = nil;						// no icon suite, but fake it for the 1-bit icon case
	else {
		h = Get1Resource(theType, refCon->resID);
		err = ResError();
	}
	if (!h || (err == -192)) {
		if (theType == 'ICN#') {
			// just get the old style component icon instead
			Handle h = NewHandle(128);

			GetComponentInfo(refCon->c, nil, nil, nil, h);
			if (!h || !*h)
				DisposHandle(h);
			else
				*theIcon = h;
		}
		return noErr;
	}
	if (err) return err;

	LoadResource(h);
	if (err = ResError()) return err;

	DetachResource(h);
	if (err = ResError()) return err;

	*theIcon = h;
	HNoPurge(*theIcon);

	return noErr;
}

pascal OSErr __GetComponentIconSuite(Component aComponent, Handle *iconSuite)
{
	register RegisteredComponent *rt;
	OSErr err;
	short resref = -1;
	short saveRes;
	short suiteID = 0;
	getIconRecord refCon;

	*iconSuite = nil;

	if( ! (rt = ValidComponent(aComponent)) )
		return invalidComponentID;					/* error : not a valid component ID */

	if (rt->rtFlags & rtfHasIconSuite) {
		Handle h;
		ComponentResourceExtension *cre;

		// open the thing's resource file
		saveRes = CurResFile();
		resref = OpenComponentResFile(aComponent);
		if (resref == -1)
			return resNotFound;

		// get the thing resource which points to the suite
		h = Get1Resource(rt->rtResourceType, rt->rtResourceID);
		if (err = ResError()) goto bail;
		LoadResource(h);
		if (err = ResError()) goto bail;

		cre = (ComponentResourceExtension *)((long)*h + sizeof(ComponentResource));
		suiteID = cre->componentIconFamily;

		ReleaseResource(h);								// we're done with this
	}

	// use the Icon Suite Manager stuff to load up the icon suite (can't use GetIconSuite
	//	because it won't detach the resources for us)
	err = NewIconSuite(iconSuite);
	if (err) goto bail;

	refCon.resID = suiteID;
	refCon.c = aComponent;
	err = ForEachIconDo(*iconSuite, -1, (IconAction)GetIconFromResource, &refCon);

bail:
	if (resref != -1) {
		CloseComponentResFile(resref);
		UseResFile(saveRes);
	}

	if (err && *iconSuite) {
		DisposeIconSuite(*iconSuite, true);
		*iconSuite = nil;
	}

	return err;
}

