/*
	File:		ROM_EM.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 11/5/92	RB		first checked in

*/

/*
			© 1990-1992 Apple Computer, Inc.    By Ricardo Batista
			
	This file is a ROM version of Extensions Manager based on Extensions Manager 1.8
	The ROM version provides minimal functionality and looks for resources in the system
	file first, so that it can be localized if nessesary.  To invoke it the user needs to press
	the space bar and Command keys while booting.  We get called by the InstallMgr routine
	in StartBoot.a,  InstallMgr gets called from the SuperMario Gibbly, just before extensions
	are loaded.
	
	We first test for space-Cmd down, and if it is then we initialize enough
	of the tollbox so that we are able to make windows and dialogs.  We then call the
	pseudo-cdev code, make our dialog and pretend that we are the control panel until the user
	clicks on the close box of our window.  This code comes from a Control Panel, so hang on...
	
	Notice that we do use GetNextEvent and routines like it because those routines do
	a lot of work in the background, the least we use the toolbox the more compatible
	that we will be in this case.  (The state of the toolbox managers at startup
	varies with machines and system releases).
	
	SPECIAL INSTRUCTIONS:
	
	Compiled with MPW 3.2
	
	
	HISTORY:
	
	11/5/92		RB		Copied some code from Extensions Manager 1.8 and made it work in ROM.
						Look for resources in the system file first, then in ROM so that someone
						can localize this code for other countries.
						Got rid of all the sets and other functionality which is no part of my
						idea of a minimal EM, such as old type INITs, menus, etc.
	
	*/

#define		SystemSixOrLater		1
#define		SystemSevenOrLater			true

#include <Types.h>
#include <QuickDraw.h>
#include <Files.h>
#include <Folders.h>
#include <Dialogs.h>
#include <Lists.h>
#include <Memory.h>
#include <Devices.h>
#include <Resources.h>
#include <ToolUtils.h>
#include <Events.h>
#include <SysEqu.h>
#include <Menus.h>
#include <TextEdit.h>
#include <Fonts.h>
#include <Packages.h>
#include <Traps.h>
#include <Aliases.h>
#include <ShutDown.h>
#include <TextUtils.h>
#include <SysEqu.h>




/*
	This is a copy of the quickdraw globals that MPW usually allocates for us
	under a5, to be really safe and compatible with the future we need to add space
	for the a5 reference (tempA5) and set a5 to point there before calling InitGraf
	once InitGraf is called we can initialize some of the toolbox managers and make
	windows and dialogs.
*/


typedef struct  {
	char privates[76];
	long randSeed;
	BitMap screenBits;
	Cursor arrow;
	Pattern dkGray;
	Pattern ltGray;
	Pattern gray;
	Pattern black;
	Pattern white;
	GrafPtr thePort;
	long tempA5;
} qdGlob;



typedef struct {
	char 			fileName[40];
	long			enabledDirID;
	long			disabledDirID;
	short			vRefNum;		// must be kept for each, because next system may
	Boolean			enabled;		// have special folders in different volumes
	char			filler;
	long			creator;
} InitInfo, *InitInfoPtr, **InitInfoH;


typedef struct {
	ListHandle 		list;
	InitInfoH		files;
} cdev_data, **cdev_dataH;




//		PROTOTYPES




void ShowExtensions(void);
pascal void mySetA5(long) = 0x2A5F;				// move.l (sp)+,a5
extern pascal void ROM_EM_OSDISPATCH(void *);
pascal void ROM_EM(void);

cdev_dataH EM_cdev(short message, short item, short numItems, short CPanelID,
				EventRecord *theEvent, cdev_dataH cdevStorage, DialogPtr CPDialog);


void ReadFiles(ListHandle list,InitInfoH files);
void SaveInfo(ListHandle list,InitInfoH files);
void RevertInfo(ListHandle list,InitInfoH files);
void AddFolder(InitInfoH files, long GoodFolder, long DisFolder, Boolean useEnabledF);
void AllInfo(ListHandle list,InitInfoH files, Boolean on);
void Add7Folders(void);
Boolean HasINIT(CInfoPBRec *info);
void ErrDlg(short id);
void Sort(InitInfoH files, long folder);
pascal Boolean UpdateFilter(DialogPtr theDialog, EventRecord *theEvent,
			short *item);
void myBlockMove(void *source, void *destination, long count);
Boolean MyCompare(unsigned char *a, unsigned char *b);


#define		gTime		0x020C
#define		spaceK		0x00000200L
#define		enterK		0x00100000L
#define		returnK		0x10000000L
#define		gCommandK	0x00008000L

#define			bOK			1

#define			bRevert			1
#define			bList			2
#define			bAllOff			3
#define			bAllOn			4



#define		kUseDisabledFolder		false
#define		kUseEnabledFolder		true

#define		okItem					1
#define		cancelItem				2

// if you change these id's, change them also in ROM_EM.r

#define		kEMID					-15020

#define		kEM_Dialog				-15020

#define		kNeedINITID				-15020
#define		kNeedNoINITID			-15021
#define		duplicateDlg			-15021


#define		kUpdateCode				1898


/*
	This is the entry point of the ROM version of EM, we test for the Cmd-space bar down and if it is down
	we initialize enough of the tollbox to make dialogs, then call the code that will
	look like a cdev and cleanup.  We also replace _OSDispatch tenmporarily, because the Process Manager
	is not fully functinal yet.
*/

pascal void ROM_EM(void)
{
	qdGlob myQuick;
	GrafPort myPort;
	KeyMap keys;
	char copyright[] = {"\p© 1990-1992 Apple Computer, Inc.  By Ricardo Batista"};
	long oldOSDispatch;
	long oldCurrentA5, oldA5;
	
	GetKeys(&keys[0]);
	if (((keys[1] & spaceK) == 0) || ((keys[1] & gCommandK) == 0))		// Cmd-space bar down ?
		return;
	oldCurrentA5 = *((long*) CurrentA5);
	myQuick.tempA5 = (long) &myQuick.thePort;
	oldA5 = SetA5((long) &myQuick.tempA5);	// INIT 31 restores A5
	InitGraf(&myQuick.thePort);
	*(long*) CurrentA5 = (long) &myQuick.tempA5;
	OpenPort(&myPort);
	InitFonts();
	InitWindows();						// calls InitMenus and DrawMenuBar
	InitMenus();
	TEInit();
	InitDialogs(0L);
	InitAllPacks();
	InitCursor();
	*((long*)DeskHook) = 0L;
	*((long*)DragHook) = 0L;
	*((long*)GhostWindow) = 0L;
	TextFont(0);
	TextSize(12);
	DrawMenuBar();
	oldOSDispatch = GetTrapAddress(0x8F);
	SetTrapAddress((long) ROM_EM_OSDISPATCH, 0x8F);
	ShowExtensions();
	SetTrapAddress(oldOSDispatch, 0x8F);
	ClosePort(&myPort);
	*(long*)CurrentA5 = oldCurrentA5;
	SetA5(oldA5);
}






/*
	This function loads a dialog similar to the control panel, then calls our cdev-like code
	as if we were the control panel.   We close our dialog when the user
	presses the closes our window.  Errors are checked from the dialog, cdev code and exec of
	the cdev code.
*/

void ShowExtensions()
{
	
	DialogPtr 			cpDialog, theDialog;
	DialogRecord 		dr;
	long 				result;
	EventRecord 		event;
	GrafPtr 			savePort;
	Boolean 			quit = false;
	short 				item = 0;
	Boolean 			dEvent;
	short 				part;
	WindowPtr 			w;
	char 				theChar;
	short				mask = keyDownMask + mDownMask + updateMask + mUpMask + keyUpMask;
	
	GetPort(&savePort);
	cpDialog = GetNewDialog(kEM_Dialog, (Ptr) &dr, (WindowPtr) -1L);
	if (!cpDialog)
		return;
	ShowWindow(cpDialog);
	SetPort(cpDialog);
	SetOrigin(89,0);
	TextFont(1);
	TextSize(9);
 	BeginUpdate(cpDialog);
	DrawDialog(cpDialog);
 	EndUpdate(cpDialog);
	result = EM_cdev(initDev, 0, 0, kEM_Dialog, &event, (cdev_dataH) 2L, cpDialog);
	if (result == 1L)
		SysBeep(1);
	if (result > 3L) {
		result = EM_cdev(updateDev, 0, 0, kEM_Dialog, &event, (cdev_dataH) result, cpDialog);
		result = EM_cdev(activDev, 0, 0, kEM_Dialog, &event, (cdev_dataH) result, cpDialog);
		while (!quit) {
			SetPort(cpDialog);
			InitCursor();					// avoid disk insert events
			if (GetNextEvent(mask, &event)) {
				if (event.what == updateEvt) {
					BeginUpdate(cpDialog);
					result = EM_cdev(updateDev, 0, 0, kEM_Dialog, &event, (cdev_dataH) result, cpDialog);
					EndUpdate(cpDialog);
				}
				if (event.what == mouseDown) {
					part = FindWindow(event.where, &w);
					if (part == inGoAway) {
						TrackGoAway(cpDialog, event.where);
						quit = true;
					}
					if (part == inContent) {
						dEvent = IsDialogEvent(&event);
						if (DialogSelect(&event, &theDialog, &item)) {
							result = EM_cdev(hitDev, item, 0, kEM_Dialog, &event, (cdev_dataH) result, cpDialog);
						}
					}
				}
				if (event.what == keyDown) {
					theChar = event.message & charCodeMask;
					item = theChar;
					result = EM_cdev(keyEvtDev, item, 0, kEM_Dialog, &event, (cdev_dataH) result, cpDialog);
				}
			}
		}
	}
	if (result > 3L)
		result = EM_cdev(closeDev, 0, 0, kEM_Dialog, &event, (cdev_dataH) result, cpDialog);
	CloseDialog(cpDialog);
	SetPort(savePort);
}












/*
	This is the main entry point of the cdev code, the Control Panel calls us
	giving us a message with the action to do.  When initializing we have to create
	a list and add to it all the files that could have INITs.
	From then on we will get called with messages like hit or key pressed, on close
	we dispose from our list.
*/


cdev_dataH EM_cdev(short message, short item, short numItems, short CPanelID,
				EventRecord *theEvent, cdev_dataH cdevStorage, DialogPtr CPDialog)
{
	cdev_dataH myData = 0L;
	short type;
	Handle H;
	Rect box;
	short myItem;
	Point cell;
	Rect bounds;
	ListHandle list = 0L;
	InitInfoH files = 0L;
	KeyMap keys;
	
	if (cdevStorage == 0L)
		return(0L);
	myData = 0L;
	if (message == initDev) {
		SetCursor(*(GetCursor(watchCursor)));
		TextFont(1);
		myData = (cdev_dataH) NewHandle(sizeof(cdev_data));
		if (myData == 0L)
			return((cdev_dataH) 1L);		// bail out
		HLock((Handle) myData);
		files = (InitInfoH) NewHandle(0L);
		if (files == 0L)
			return((cdev_dataH) 1L);		// bail out
		(**myData).files = files;
		GetDItem(CPDialog,numItems + bList,&type,&H,&box);
		box.right -= 15;
		SetRect(&bounds,0,0,1,0);
		cell.h = cell.v = 0;
		list = LNew(&box,&bounds,cell,0,CPDialog,false,false,false,true);
		if (list == 0L)
			return((cdev_dataH) 1L);		// bail out
		(**myData).list = list;
		(**list).selFlags += lNoExtend + lUseSense;
		ReadFiles(list,files);
		SetResLoad(true);
		InitCursor();
	}
	else {
		myData = cdevStorage;			// handle is still locked
		files = (**myData).files;
		list = (**myData).list;
	}
	if (message == hitDev) {
		SetResLoad(true);
		myItem = item - numItems;
		if (myItem == bList) {
			GlobalToLocal(&(theEvent->where));
			LClick(theEvent->where, shiftKey, list);
			cell = LLastClick(list);
		}
		if (myItem == bRevert) {
			RevertInfo(list, files);
			LUpdate(CPDialog->visRgn, list);
		}
		if (myItem == bAllOff) {
			AllInfo(list, files, false);
			LUpdate(CPDialog->visRgn, list);
		}
		if (myItem == bAllOn) {
			AllInfo(list, files, true);
			LUpdate(CPDialog->visRgn, list);
		}
	}
	if (message == closeDev) {
		SaveInfo(list, files);
		DisposHandle((Handle) files);
		LDispose(list);
		HUnlock((Handle) myData);
		DisposHandle((Handle) myData);
		myData = 0L;
		GetKeys(&keys[0]);
		if ((keys[1] & gCommandK) != 0)
			ShutDwnStart();
	}
	if (message == nulDev) {
	}
	if (message == updateDev) {
		if (CPanelID)
			SetPort(CPDialog);	// redundant but gets rid of warning !!!
		DrawDialog(CPDialog);
		GetDItem(CPDialog,numItems + bList,&type,&H,&box);
		InsetRect(&box,-1,-1);
		FrameRect(&box);
		LUpdate(CPDialog->visRgn, list);
	}
	if (message == activDev) {
		LActivate(true, list);
	}
	if (message == deactivDev) {
		LActivate(false, list);
	}
	if (message == macDev) {
		myData = (cdev_dataH) 1L;
	}
	return(myData);
}









// This routine determines whether a file should be added to our INIT list
// or not.  If the file is capable of having an INIT then we try to find it,
// otherwise we display it. (i.e.  a Finder extension)


Boolean HasINIT(CInfoPBRec *info)
{
	short resFile;
	register long type;
	register short counter, typeCount;
	long **InitTypesH, *initType;
	Boolean isValid;
	FSSpec file;
	short err;
	Boolean isFolder, wasAlias;
	short len, ourFile;
	
	type = info->hFileInfo.ioFlFndrInfo.fdType;
	if (info->hFileInfo.ioFlFndrInfo.fdCreator == '7INI')	// don't disable ourselves
		return(false);
	if (info->hFileInfo.ioFlFndrInfo.fdCreator == 'extE')	// don't bother with EM Ext.
		return(true);
	InitTypesH = (long**) GetResource('iNIT',kNeedNoINITID);
	if (!InitTypesH) {
		*((short*) RomMapInsert) = mapTrue;
		InitTypesH = (long**) GetResource('iNIT',kNeedNoINITID);
	}
	if (!InitTypesH)
		return(false);
	LoadResource((Handle) InitTypesH);
	HLock((Handle) InitTypesH);
	initType = *InitTypesH;
	typeCount = GetHandleSize((Handle) InitTypesH) / 4L;
	for (counter = 0; counter < typeCount; counter++) {
		if (type == initType[counter])
			return(true);	// these files don't need an INIT, just show them
	}
	
	InitTypesH = (long**) GetResource('iNIT',kNeedINITID);
	InitTypesH = (long**) GetResource('iNIT',kNeedINITID);
	if (!InitTypesH) {
		*((short*) RomMapInsert) = mapTrue;
		InitTypesH = (long**) GetResource('iNIT',kNeedINITID);
	}
	if (!InitTypesH)
		return(false);
	LoadResource((Handle) InitTypesH);
	HLock((Handle) InitTypesH);
	initType = *InitTypesH;
	typeCount = GetHandleSize((Handle) InitTypesH) / 4L;
	isValid = false;
	for (counter = 0; counter < typeCount; counter++) {
		if (type == initType[counter]) {
			isValid = true;
			counter = typeCount + 1;	// these files need an INIT
		}
	}
	if (isValid == false)
		return(false);		// not a valid file
	file.vRefNum = info->hFileInfo.ioVRefNum;
	file.parID = info->hFileInfo.ioDirID;
	len = info->hFileInfo.ioNamePtr[0];
	myBlockMove(info->hFileInfo.ioNamePtr, file.name, (long) (len + 1L));
	err = ResolveAliasFile(&file, true, &isFolder, &wasAlias);
	if (err || isFolder)
		return(false);		// file not found, or volume not found
	SetResLoad(false);
	ourFile = CurResFile();
	if (info->hFileInfo.ioFRefNum)
		resFile = info->hFileInfo.ioFRefNum;
	else
		resFile = HOpenResFile(file.vRefNum, file.parID,(Str255) file.name, fsCurPerm);
	if (resFile != -1) {
		SetResLoad(false);
		counter = Count1Resources('INIT');
		if (info->hFileInfo.ioFRefNum == 0)
			CloseResFile(resFile);
		SetResLoad(true);
	}
	UseResFile(ourFile);
	SetResLoad(true);
	return(counter);
}














// This is the main routine that can go out looking for new files, it calls the appropiate
// routines to fill in the list with the right file names.

void ReadFiles(ListHandle list,InitInfoH files)
{
	long **FolderTypesH = 0L;
	register short counter, total;
	Point cell;
	InitInfoPtr pt;
	short len;
	long enabledFolder, disabledFolder;
	Handle H;
	
	LDoDraw(false, list);
	len = (**list).dataBounds.bottom;
	if (len) {
		LDelRow(len,0,list);
		SetHandleSize((Handle) files, 0L);
	}
	FolderTypesH = (long**) GetResource('FTyp',kEMID);
	if (!FolderTypesH) {
		*((short*) RomMapInsert) = mapTrue;
		FolderTypesH = (long**) GetResource('FTyp',kEMID);
	}
	if (FolderTypesH) {
		LoadResource((Handle) FolderTypesH);
		HLock((Handle) FolderTypesH);
		Add7Folders();
		total = GetHandleSize((Handle) FolderTypesH) / 4;
		for (counter = 0; counter < total; counter += 2) {
			enabledFolder = (*FolderTypesH)[counter];
			disabledFolder = (*FolderTypesH)[counter + 1];
			AddFolder(files, enabledFolder, disabledFolder,
						kUseEnabledFolder);
			AddFolder(files, enabledFolder, disabledFolder,
						kUseDisabledFolder);
			Sort(files, disabledFolder); // sort before inserting in list
		}
		H = GetResource('iNIT',kNeedNoINITID);		// the code to scan a folder
		if (H) {									// loaded this resources and
			LoadResource(H);						// left them locked, so
			HUnlock(H);								// lets release them
			ReleaseResource(H);
		}
		H = GetResource('iNIT',kNeedINITID);
		if (H) {
			LoadResource(H);
			HUnlock(H);
			ReleaseResource(H);
		}
		total = GetHandleSize((Handle) files) / sizeof(InitInfo);
		LAddRow(total,0,list);
		cell.h = cell.v = 0;
		HLock((Handle) files);
		pt = *files;
		for (counter = 0; counter < total; counter++) {
			len = pt->fileName[0];
			LSetCell(&(pt->fileName[1]), len, cell, list);
			if (pt->enabled)
				LSetSelect(true,cell,list);
			cell.v++;
			pt++;
		}
		HUnlock((Handle) files);
	}
	LDoDraw(true, list);
}






// This routine adds all the files in a given folder that have a type
// included in a list to the INIT list.

void AddFolder(InitInfoH files, long GoodFolder, long DisFolder, Boolean useEnabledF)
{
	register short index;
	short err, vRefNum;
	long EnabledDirID, DisabledDirID;
	CInfoPBRec info;
	InitInfo anInit;
	long offset;
	long testDirID;
	
	err = FindFolder(kOnSystemDisk, GoodFolder, kCreateFolder, &vRefNum, &EnabledDirID);
	if (err)
		return;
	err = FindFolder(kOnSystemDisk, DisFolder, kCreateFolder, &vRefNum, &DisabledDirID);
	if (err)
		return;
	if (useEnabledF)
		testDirID = EnabledDirID;
	else
		testDirID = DisabledDirID;
	index = 0;
	info.hFileInfo.ioCompletion = 0L;
	info.hFileInfo.ioNamePtr = (StringPtr) anInit.fileName;
	info.hFileInfo.ioVRefNum = vRefNum;
	anInit.vRefNum = vRefNum;
	anInit.enabledDirID = EnabledDirID;
	anInit.disabledDirID = DisabledDirID;
	anInit.enabled = useEnabledF;
	offset = GetHandleSize((Handle) files);
	while (!err) {
		index++;
		info.hFileInfo.ioFDirIndex = index;
		info.hFileInfo.ioDirID = testDirID;
		info.hFileInfo.ioFlAttrib = 0;
		info.hFileInfo.ioFlFndrInfo.fdFlags = 0;
		info.hFileInfo.ioFRefNum = 0;
		err = PBGetCatInfo(&info,false);
		if (!err) {
			if ((info.hFileInfo.ioFlAttrib & 16) == 0) {
				anInit.creator = info.hFileInfo.ioFlFndrInfo.fdCreator;
				info.hFileInfo.ioDirID = testDirID;
				if (HasINIT(&info)) {
					Munger((Handle) files,offset,(Ptr) 2L,0L,
						(Ptr) &anInit,(long) sizeof(anInit));
					offset += sizeof(anInit);
				}
			}
		}
	}
}









// We make sure that we have our "(Disabled)" folders available, if not we
// setup the system so we that we can use them.
// To do that we load our 'fld#' resource and check every folder type and
// name we have specified.  If the folder type is not there
// then we also need to add it into the system's fld#.

void Add7Folders(void)
{
	short vRefNum;
	long dirID;
	Handle sysH, ourH;
	long sysSize, ourSize, fldSize;
	short err;
	Ptr fldPtr;
	register short counter;
	long folderType;
	Boolean changed = false;
	
	sysH = GetResource('fld#', 0);
	ourH = GetResource('fld#', kEMID);
	if (ourH == 0L) {
		*((short*) RomMapInsert) = mapTrue;
		ourH = GetResource('fld#', kEMID);
	}
	if ((sysH == 0L) || (ourH == 0L))
		return;
	LoadResource(sysH);
	LoadResource(ourH);
	HLock(ourH);
	fldSize = GetHandleSize(ourH);		// we have to keep track of size
	fldPtr = *ourH;
	sysSize = GetHandleSize(sysH);
	for (counter = 0; counter < fldSize; ) {
		myBlockMove(fldPtr, &folderType, 4L);  // get type
		ourSize = 8 + fldPtr[7];	// add type, id, string len
		if (ourSize % 2)
			ourSize++;		// string is padded !
		err = FindFolder(kOnSystemDisk, folderType, kCreateFolder, &vRefNum, &dirID);
		if (err) {	// need to add our folder
			Munger(sysH, sysSize, (Ptr) 2L, 0L, fldPtr, ourSize);
			sysSize += ourSize;
			changed = true;
		}
		fldPtr += (Ptr) ourSize;	// add offset to next fld# entry
		counter += ourSize;			// add bytes scanned from fld# entry
	}
	if (changed) {
		ChangedResource(sysH);
		WriteResource(sysH);
		UpdateResFile(0);			// update the system
		sysH = GetResource('fld#', 0);
	}
	ReleaseResource(ourH);
}






// This is a very custom sorting rotuine, it takes the disabled folder id to match
// files agains it, it first finds the start of the folder type so that it can compare
// the disabled ones with the enabled ones and insert them at the right position.
// It also knows that it gets called after each folder type insert, so the end of the
// insert list is always the total number of records.

void Sort(InitInfoH files, long folder)
{
	short startIndex, endIndex, sortStart;
	register short counter, counter2;
	register InitInfoPtr inits;
	InitInfo tempInfo;
	long dirID;
	short vRefNum;
	
	if (FindFolder(kOnSystemDisk, folder, kDontCreateFolder, &vRefNum, &dirID))
		return;
	HLock((Handle) files);
	inits = *files;
	endIndex = GetHandleSize((Handle) files) / sizeof(InitInfo);
	for (counter = 0; counter < endIndex; counter++) {		// find the start of this folder
		if (inits[counter].disabledDirID == dirID) {
			startIndex = counter;
			break;
		}
	}
	if (counter == endIndex) {
		HUnlock((Handle) files);		// there was only one new extension
		return;
	}
	sortStart = startIndex;
	while ((inits[sortStart].enabled) && (sortStart < endIndex))
		sortStart++;		// skip enabled
	if (sortStart == endIndex) {
		HUnlock((Handle) files);		// there was only enabled extensions
		return;
	}
	while (sortStart < endIndex) {
		for (counter = startIndex; counter < endIndex; counter++) {
			if (sortStart == counter) {		// checking against ourselves ?
				startIndex++;
				sortStart++;	// we are the end of it.
				break;
			}
			else {
				if (MyCompare((unsigned char*) inits[sortStart].fileName, 
								(unsigned char*) inits[counter].fileName)) {	// a < b
					tempInfo = inits[sortStart];
					for (counter2 = sortStart; counter2 > counter; counter2--) {
						inits[counter2] = inits[counter2 - 1];
					}
					inits[counter] = tempInfo;
					startIndex++;  // skip at least one more from enabled, since it was sorted already
					break;
				}
			}
		}
		sortStart++; 			// one less from the disabled folder
	}
	HUnlock((Handle) files);
}








//
// This routine makes permanent the changes the user has made, we look at the
// "enabled" flag and compare this with the current higlight state, that way we
// can tell if the file needs to be moved from one folder to the other.

void SaveInfo(ListHandle list,InitInfoH files)
{
	register short counter, total, err;
	Point cell;
	CMovePBRec info;
	Boolean toggle;
	InitInfo *initPtr;
	
	total = GetHandleSize((Handle) files) / sizeof(InitInfo);
	HLock((Handle) files);
	cell.h = cell.v = 0;
	for (counter = 0; counter < total; counter++) {
		cell.v = counter;
		toggle = false;
		if (LGetSelect(false,&cell,list)) {
			if (((*files)[counter]).enabled == false) {
				toggle = true;
				((*files)[counter]).enabled = true;
			}
		}
		else {
			if (((*files)[counter]).enabled) {
				toggle = true;
				((*files)[counter]).enabled = false;
			}
		}
		if (toggle) {
			initPtr = &((*files)[counter]);
			info.ioCompletion = 0L;
			info.ioNamePtr = initPtr->fileName;
			info.ioNewName = 0L;
			info.ioVRefNum = initPtr->vRefNum;
			if (initPtr->enabled) {
				info.ioDirID = initPtr->disabledDirID;
				info.ioNewDirID = initPtr->enabledDirID;
			}
			else {
				info.ioDirID = initPtr->enabledDirID;
				info.ioNewDirID = initPtr->disabledDirID;
			}
			err = PBCatMove(&info,false);
			if (err)
				ErrDlg(duplicateDlg);
		}
	}
	HUnlock((Handle) files);
}













//
// This function reverts the higlight state of each file to what the "enabled" flag
// was, since that flag indicates to us what folder the file comes from.

void RevertInfo(ListHandle list,InitInfoH files)
{
	register short counter, total;
	Point cell;
	
	LDoDraw(false, list);
	total = GetHandleSize((Handle) files) / sizeof(InitInfo);
	HLock((Handle) files);
	cell.h = cell.v = 0;
	for (counter = 0; counter < total; counter++) {
		cell.v = counter;
		if (((*files)[counter]).enabled)
			LSetSelect(true,cell,list);
		else
			LSetSelect(false,cell,list);
	}
	HUnlock((Handle) files);
	LDoDraw(true, list);
}








//
// This function set all of the files either on or off

void AllInfo(ListHandle list,InitInfoH files, Boolean on)
{
	register short counter, total;
	Point cell;
	
	LDoDraw(false, list);
	total = GetHandleSize((Handle) files) / sizeof(InitInfo);
	cell.h = cell.v = 0;
	for (counter = 0; counter < total; counter++) {
		cell.v = counter;
		LSetSelect(on, cell, list);
	}
	LDoDraw(true, list);
}













/*
	This routine shows a dialog box with the id given to it and waits for the
	user to click the OK button.  Mainly to show error dialogs.
*/


void ErrDlg(id)
short id;
{
	GrafPtr savePort;
	DialogPtr aDialog;
	DialogRecord d;
	short item;
	Handle H;
	Rect box;
	
	GetPort(&savePort);
	aDialog = GetNewDialog(id, &d, (WindowPtr) -1L);
	if (!aDialog) {
		*((short*) RomMapInsert) = mapTrue;
		aDialog = GetNewDialog(id, &d, (WindowPtr) -1L);
	}
	if (!aDialog) {
		SysBeep(1);
		return;
	}
	SetPort(aDialog);
	
	item = kUpdateCode;
	InitCursor();
	while (item != okItem) {
		if (item == kUpdateCode) {
			BeginUpdate(aDialog);
			GetDItem(aDialog,okItem,&item,&H,&box);
			InsetRect(&box,-4,-4);
			DrawDialog(aDialog);
			PenSize(3,3);
			FrameRoundRect(&box,16,16);
			EndUpdate(aDialog);
		}
		ModalDialog(UpdateFilter, &item);
	}
	CloseDialog(aDialog);
	SetPort(savePort);
}










pascal Boolean UpdateFilter(DialogPtr theDialog,EventRecord *theEvent,short *item)
{
	char theChar;
	long delayed;
	short type;
	Rect box;
	Handle H;
	
	if (theEvent->what == updateEvt) {
		*item = kUpdateCode;				/* A special code so I can update screen */
		return(true);
	}
	if ((theEvent->what == keyDown) || (theEvent->what == autoKey)) {
		theChar = theEvent->message & charCodeMask;
		if ((theChar == 13) || (theChar == 3)) {
			*item = okItem;		// or default item, one anyway
			GetDItem(theDialog, okItem, &type, &H, &box);
			HiliteControl((ControlHandle) H, 1);
			Delay(8L, &delayed);
			HiliteControl((ControlHandle) H, 0);
			return(true);
		}
	}
	return(false);
}











// faster than calling the real blockmove when the data you usually copy
// is small

void myBlockMove(void *source, void *destination, long count)
{
	register Ptr s, d;
	
	s = source;
	d = destination;
	while (count) {
		*d++ = *s++;
		count--;
	}
}







// This function returns true if the first string has precedence over the second one
// otherwise it returns false.  Not a very good routine when it comes to International

Boolean MyCompare(unsigned char *a, unsigned char *b)
{
	register short len, counter = 1;
	register char theCharA, theCharB;
	
	len = a[0];
	if (len > b[0])
		len = b[0];		// compare up to the smallest common length
	while (len) {
		theCharA = a[counter];
		theCharB = b[counter];
		if ((theCharA >= 'a') && (theCharA <= 'z'))
			theCharA -= 32;		// make it lowercase
		if ((theCharB >= 'a') && (theCharB <= 'z'))
			theCharB -= 32;		// make it lowercase
		if (theCharA < theCharB)
			return(true);
		if (theCharA > theCharB)
			return(false);
		counter++;
		len--;
	}
	if (a[0] < b[0])
		return(true);
	return(false);		// equal strings, or b is longer string
}

