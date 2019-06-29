/*
	File:		alDialog.c

	Contains:	Implementation of SelectAlias dialog box.

	Written by:	Prashant Patel

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<13>	10/21/90	gbm		Changing hfileInfo to hFileInfo.
	    <13>	10/15/90	JL		Changing hfileInfo to hFileInfo it should have never been
									changed back.
		<12>	  6/8/90	ngk		update for changes to StandardFile.h
		<11>	 5/16/90	PP		Parameters to AL_findVolume has cahnged.
		<10>	  5/3/90	PP		Incorporate Diet tips.
		 <9>	 4/10/90	PP		Change once more hFileInfo to hfileInfo to stay consistent with
									Files.h
		 <8>	  4/5/90	PP		Change "callbackPtr" to "yourDataPtr".
		 <7>	  3/8/90	JAL		Changed hfileInfo to hFileInfo to stay consistent with change 
		 							in Files.h
		 <6>	  3/5/90	PKE		Added smTruncEnd as truncWhere parameter for TruncString call,
									since TruncString interface now matches former NTruncString
									interface.
		 <5>	 2/26/90	ngk		Changed names of new standardfile calls to match header.
		 <4>	 2/26/90	dnf		Change capitalization to match changes in files.h, rename
									positionRec to CatPositionRec.
		 <3>	 1/21/90	PP		Fix bugs related to flashing and beeping while in scroll bar;
									"Select" button hilited evenif no selection was selected.
	   <2.1>	12/15/89	prp		Restore current port correctly.
	   <2.0>	11/27/89	prp		SetArrowCursor is now a Trap selector routine.
	   <1.9>	11/16/89	prp		Cancel dialog box uses 'Stop Search' button instead of
									cmd-period. MyFilter does not bring up cancel dialog but a fake
									returned item does. StopWatch and arrow cursor work correctly
									while cancel dialog is up. If initial volume not found, curVref
									still gets set correctly.
	   <1.8>	10/30/89	prp		• AcceptFile has additional vref argument. This allows choices
									from multiple disks to show up correclty. • Lock and Unlock
									sp->alList handle in order to not cause any purged handle
									referencing problems. • Handle CatSearch returning wrgVolTypeErr
									for AppleShare volumes where it is not supported. • Do not do
									AcceptFile of fastList explicitly. Reordering based on
									alFastList count.
	   <1.7>	10/17/89	prp		NewSFPGetFile is now released. No need to coerce arguments.
	   <1.6>	10/14/89	ngk		Changed call to NewSFPGetFile to not need coercing.
	   <1.5>	10/13/89	prp		Add support for folder 'Another One' selection. Works with
									NewSFPGetFile.
	   <1.4>	 10/2/89	prp		Added support for searching all mounted volumes when user pushes
									'TryHarder' button.
	   <1.3>	 9/18/89	prp		SelectAlias has additional filtering parameters.
	   <1.2>	  9/6/89	prp		Changes from CodeReview.
	   <1.1>	 8/11/89	prp		Do not call CatSearch again if previous call has returned an
									eofErr.
	   <1.0>	  8/7/89	prp		Initial Creation

	To Do:
*/

/*EASE$$$ READ ONLY COPY of file “alDialog.c”
** 2.1	prp 12/15/1989 Restore current port correctly.
** 2.0	prp 11/27/1989 SetArrowCursor is now a Trap selector routine.
** 1.9	prp 11/16/1989 Cancel dialog box uses 'Stop Search' button instead of
**		cmd-period. MyFilter does not bring up cancel dialog but a fake
**		returned item does. StopWatch and arrow cursor work correctly while
**		cancel dialog is up. If initial volume not found, curVref still gets
**		set correctly.
** 1.8	prp 10/30/1989 • AcceptFile has additional vref argument. This allows
**		choices from multiple disks to show up correclty. • Lock and Unlock
**		sp->alList handle in order to not cause any purged handle referencing
**		problems. • Handle CatSearch returning wrgVolTypeErr for AppleShare
**		volumes where it is not supported. • Do not do AcceptFile of fastList
**		explicitly. Reordering based on alFastList count.
** 1.7	prp 10/17/1989 NewSFPGetFile is now released. No need to coerce
**		arguments.
** 1.6	ngk 10/14/1989 	Changed call to NewSFPGetFile to not need coercing.
** 1.5	prp 10/13/1989 Add support for folder 'Another One' selection. Works
**		with NewSFPGetFile.
** 1.4	prp 10/02/1989 Added support for searching all mounted volumes when
**		user pushes 'TryHarder' button.
** 1.3	prp 09/18/1989 SelectAlias has additional filtering parameters.
** 1.2	prp 09/06/1989 Changes from CodeReview.
** 1.1	prp 08/11/1989 Do not call CatSearch again if previous call has
**		returned an eofErr. 
** 1.0	prp 08/07/1989 Initial Creation
** END EASE MODIFICATION HISTORY */
/*********************************************************************
*
*	File:		alDialog.c
*	Project: 	Alias Manager
*	Contains:	support for alias resolution through a dialog box
*	Written by:	Prashant Patel
*
*	Copyright 1989 by Apple Computer, Inc.
*	All Rights Reserved.
*
**********************************************************************/

/**********************************************************************
 ***************		Public Include files		*******************
 *********************************************************************/
 
 #include "Aliases.h"		
 
/**********************************************************************
 ***************		 Private Include files		*******************
 *********************************************************************/
 
 #include "aliasPriv.h"		
 #include "aliasDlogPriv.h"		

/**********************************************************************
 ***************		External C Include files		***************
 *********************************************************************/

#ifndef __ERRORS__
#include <errors.h>
#endif
#ifndef __DIALOGS__
#include <dialogs.h>
#endif
#ifndef __LISTS__
#include <Lists.h>
#endif
#ifndef __FONTS__
#include <Fonts.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __STANDARDFILE__
#include <StandardFile.h>
#endif
#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif
#ifndef __SCRIPT__
#include <Script.h>
#endif

/*********************************************************************
******** pseudo static Variables, since static not allowed ***********
*********************************************************************/
/* This is a structure that holds variables that have a scope for this entire file.
   Since static variables are not allowed for stand-alone code resources, we will
   use a pointer to this struct as a way of accessing these variables.  All local
   routines needing access to it will be passed a pointer to it.  It will also be
   stored in RefCon of our dialog box record so that routines like myFilter can
   also access it.
*/
typedef struct {
	 AliasPtrPriv aptr;			// pointer to locked AliasRecord
	 DialogPtr aliasDialog;  	// Pointer to alias dialog 
	 DialogPtr cancelDialog;  	// Pointer to cancel further search dialog 
	 SICNHandle alSICNHandle; 	// handle to SICN resource
	 ControlHandle ButtonCH[kNumButtons]; // button control handles
	 Rect nameRect;				// Rectangle for names list
	 Rect locationRect;  		// Rectangle for location display list 
	 ListHandle nameList;		// handle to list of names
	 ListHandle locationList;	// handle to the list displaying location
	 short selRow;    			// Current selection row in names list
	 short prevSel;				// previous selection row in names list
	 short smallFontSize;		// size of small font in which list entry text is drawn
	 Boolean DoubleClick;  		// Flag to say that a double click on a list happened
	 Boolean TryHarder;			// Flag to say if in TryHarder mode
	 Boolean firstTry;			// Flag to say if TryingHard first time or not
	 Boolean CSrch;				// Flag to say if CatSearch available or not
	 Boolean quitFlag;			// Flag to say if client filter proc asked us to quit
	 short firstVref;			// volume ref num of first volume we searched
	 short curVref;				// current volume ref num of volume we are searching
	 long curDir;				// current directory we are searching
	 long curIdx;				// current directory index for the search
	 CatPositionRec curPrec;	// current CatSearch position record
	 Str63 curFname;			// current file name
	 CInfoPBRec asyncCPB;		// async I/O CPB record
	 unsigned long startCount;	// remember ticks when trying hard starts
	 short curCnt;				// current # of entries in alias list
	 short curLim;				// current # of entries allocated in alias list 
	 VCB *vcbPtr;				// pointer to VCB entry
	 CanonicalFileSpecHandle alList; // alias list buffer handle
	 LongsHandle dirStack;		// directory stack
	 AliasFilterProcPtr fileFilter; // client supplied file filter proc ptr
	 ModalFilterWithCallback modalFilter; // client supplied ModalDialog filter proc ptr
	 Ptr yourDataPtr;			// additional param to be passed to fileFilter & modalFilter
	 StandardFileReply mySFreply;	// reply record for CustomGetFile
	 Boolean folderSelected;	// Flag to say that a folder was selected during 'AnotherOne'
} myStaticVars;

typedef myStaticVars *myStaticPtr; // pointer to my static var record

	
/*********************************************************************
******** forward declarations ****************************************
*********************************************************************/
 static OSErr Do_findNext(const myStaticPtr sp);
 static void DoneTryingHard(Boolean forEver, const myStaticPtr sp);
 static void NextVolumeSearch(const myStaticPtr sp);

 extern pascal OSErr AL_selectAliasWithDialog(const CanonicalFileSpec *fromFile, //I
						const Str31 fTypeName,			//I
						short cnt,						//I
						CanonicalFileSpecList alFastList,//I
						AliasFilterProcPtr aliasFilter,	//I
						AliasHandlePriv Alias,			//I/O
						CanonicalFileSpec *toFile,		//O
						ModalFilterWithCallback filterProc, //I
						Ptr yourDataPtr);				//I

/*********************************************************************
******** Main Entry for dialog code. This has to be the first code ***
******** in this file.  The code contained in this file gets loaded **
******** dynamically only if needed.  The first byte of the loaded ***
******** code is assumed to be the starting code for alias dialog. ***
*********************************************************************/
pascal OSErr AL_mainEntryForDialog(const CanonicalFileSpec *fromFile, //I from file 
						const Str31 fTypeName,			//I descriptive name for file type
						short cnt,						//I #of entries found during fast search
						CanonicalFileSpecList alFastList,//I fast search answer
						AliasFilterProcPtr aliasFilter,	//I file filter proc pointer
						AliasHandlePriv Alias,			//I/O alias record handle
						CanonicalFileSpec *toFile,		//O returned toFile
						ModalFilterWithCallback filterProc, //I modal filter proc pointer
						Ptr yourDataPtr)				//I additional param to be passed to filter procs

{
	return (AL_selectAliasWithDialog (fromFile, fTypeName, cnt, alFastList,
				aliasFilter, Alias, toFile, filterProc, yourDataPtr));
}


/*********************************************************************
******** Set the Font and size to our small, informational font ******
*********************************************************************/
static void DoSmFont()
{
	TextFont(kSmallFont);
	TextSize(kSmallSize);
}

/*********************************************************************
******** Set the Font and size to the system font ********************
*********************************************************************/
static void DoDlgFont()
{
	TextFont(0);
	TextSize(0);
}

/*********************************************************************
******** Draw one small icon from a SICN List ************************
*********************************************************************/
static void PlotSICN (  short theIndex,			//I	index into SICN list
						Rect *theRect,			//I	rect in which it is drawn
						const myStaticPtr sp)   //I pointer to my static storage

{
	BitMap theBits;						// bitmap of the SICN
	SICNHandle theHandle;				//	handle to SICN list resource

	// set up the bitmap for the SICN to be drawn and copy it to the rect
	theHandle = sp->alSICNHandle;
	theBits.baseAddr = (Ptr) ((*theHandle)[theIndex]);
	theBits.rowBytes = 2;
	SetRect(&theBits.bounds, 0, 0, kSICNSize, kSICNSize);
	CopyBits(&theBits, &((sp->aliasDialog)->portBits), 
				&theBits.bounds, theRect, srcCopy, NULL);
}

/*********************************************************************
******** Flash a button for user feedback ****************************
*********************************************************************/
static void  FlashButton(const ControlHandle button) //I button's control handle
{ 
	long dummyCount;					// dummy tick count
	
	HiliteControl (button, 1);			// hiliting on
	Delay (8, &dummyCount);				// delay for 8 ticks
	HiliteControl (button, 0);			// hiliting off
} 

/*********************************************************************
******** draw or hide the default outline of Select button ***********
*********************************************************************/
static void  DefaultOutline(Boolean on,				//I	outline on or off?
 					const DialogPtr aliasDialog)	//I	my dialog box record pointer
{ 
	Rect    itemBox;   					// rectangle surrounding default item
	short    DType; 					// Type of dialog item 
	Handle    DItem;    				// Handle to the dialog item 
	 
	GetDItem(aliasDialog, kI_SelectOutline, &DType, &DItem, &itemBox);// Get the item handle 
	if (!on)
		PenMode(patBic);				// clear it
	PenSize(3, 3);  					// Change pen to draw thick default outline 
	FrameRoundRect(&itemBox, 16, 16); 	// Draw the outline 
	PenNormal();						// restore initial state of pen 
} 

/*********************************************************************
******** Change Select, Cancel and Another One button states *********
*********************************************************************/
static void ChangeOthers(short hiliteState, 	//I	new hilite state
 				const myStaticPtr sp)   		//I pointer to my static storage
{
	HiliteControl (sp->ButtonCH[kI_AnotherOne-1], hiliteState); // Another One button
	// If there is nothing to pick, don't activate Select button
	HiliteControl (sp->ButtonCH[kI_Select-1], 
	  (hiliteState == kButtonActive && sp->curCnt > 0) ? kButtonActive : kButtonInactive);	// Select button to new state
	DefaultOutline ((hiliteState == kButtonActive && sp->curCnt > 0) ? kDrawOutline : kEraseOutline,
	  sp->aliasDialog); // outline Select button
	HiliteControl (sp->ButtonCH[kI_Cancel-1], hiliteState);	// Cancel button to new state
}

/*********************************************************************
***************	Handle a list draw message   *************************
*********************************************************************/
static void ListDrawMsg(	Rect lRect,		//I	rect in which cell is drawn
						short lDataOffset, 	//I offset into cell data of the cell
						short lDataLen, 	//I length in bytes of cell data
						const ListHandle lHandle) //I list handle
{
	ListEntry *theEntry;		// ptr to entry we stored in the list
	DataHandle theCells; 		// Handle to cells 
	Rect tempRect;				// temporary rectangle
	Str63 aString; 				// String for mashing file name
	myStaticPtr sp;				// pointer to my static data
	char masterFlags;			// flags to remember before locking and unlocking handles

	sp = (myStaticPtr)(*lHandle)->userHandle;
	EraseRect(&lRect);			// we are going to redraw it
	if (lDataLen > 0) {
		tempRect.top = lRect.top;		
		tempRect.left = lRect.left;
		tempRect.bottom = lRect.bottom;
		tempRect.right = tempRect.left+kSICNSize; // boundry for SICN
		theCells = (*lHandle)->cells;
		masterFlags = HGetState ((Handle)theCells); // remember master flags
		HLock((Handle)theCells);
		theEntry = (ListEntry *) ((*theCells)+lDataOffset); // our list entry
		PlotSICN(theEntry->icon, &tempRect, sp);
		MoveTo(tempRect.right + kSpSICN, tempRect.bottom - kSIBase); // now draw text
		DoSmFont();				// small application font
		AL_copyPString (theEntry->fileName, aString);
		TruncString(lRect.right - (tempRect.right+kSpSICN), aString, smTruncEnd); // fit it in rectangle <6>
		DrawString(aString);
		DoDlgFont();			// back to default font
		HUnlock((Handle)theCells);
		HSetState((Handle)theCells, masterFlags); // restore original state
	}
	
} // end of Alias_LDEF

/*********************************************************************
***************	LDEF procedure for name list *************************
*********************************************************************/
static pascal void Name_LDEF(	short lMessage, //I	operation to be performed 
						Boolean lSelect,		//I	select the cell or not
						Rect lRect,				//I	rect in which cell is drawn
						Cell lCell,				//I	the cell which is operated on 
						short lDataOffset, 		//I offset into cell data of the cell
						short lDataLen, 		//I length in bytes of cell data
						ListHandle lHandle)		//I list handle
{
	switch (lMessage) {
	
	case lDrawMsg:
		ListDrawMsg (lRect, lDataOffset, lDataLen, lHandle);
		if (lSelect)	
			InvertRect(&lRect);
		break;
		
	case lHiliteMsg:
		InvertRect(&lRect);
		break;
		
	default:					// ignore initialize and close
		break;
		
	} // end of switch
		
} // end of Name_LDEF

/*********************************************************************
***************	LDEF procedure for location list *********************
*********************************************************************/
static pascal void Location_LDEF(	short lMessage, //I	operation to be performed 
						Boolean lSelect,		//I	select the cell or not
						Rect lRect,				//I	rect in which cell is drawn
						Cell lCell,				//I	the cell which is operated on 
						short lDataOffset, 		//I offset into cell data of the cell
						short lDataLen, 		//I length in bytes of cell data
						ListHandle lHandle)		//I list handle
{	
	if (lMessage == lDrawMsg) {
		lRect.left += kSpSICN;	// open folder looks better with offset		
		ListDrawMsg (lRect, lDataOffset, lDataLen, lHandle);
	}
	// no need to handle any other messages, since picking not allowed in LocationList
} // end of Location_LDEF

 /*********************************************************************
 ***************	Get Icon ID given a directory ID ******************
 *********************************************************************/

static short GetIconID(long dirID,			//I directory ID of list entry
 				unsigned short volType) //I current volume type 
{ 
	if (dirID != kVolRootDirID)
		return (kSIOpen);		// open folder for non-root level directory
	else if (volType == kVolFloppy400K || volType == kVolFloppy800K || volType == kVolFloppy1400K)
		return (kSIFloppy);		// root level is a folppy volume
	else
		return (kSIHard);		// root level is hard disk volume
}

 /*********************************************************************
 ***************	add an entry to a list ****************************
 *********************************************************************/

static void  Add_toList(short iconID,			//I	icon id for this list entry
						short vref,				//I volume ref num for list entry
						long dirID,				//I directory ID of list entry
						const StringPtr theString, //I text of list entry
						ListHandle theList)		//I	list handle
{ 
	short	theRow;						// the row that we are going to add
	Point   cSize;  					// Pointer to a cell in a list
	short 	len;						// length of the string
	ListEntry theEntry;					// entry to be added to the list
 
	cSize.h = 0;    				// Point to the correct column 
	theRow = LAddRow(1, 32767, theList);// Add another row at the end of the list 
	cSize.v = theRow;   			// Point to the row just added
	len = *theString;				// prepare list entry
	theEntry.icon = iconID;
	theEntry.vRefNum = vref;
	theEntry.dirID = dirID;
	BlockMove (theString, theEntry.fileName, len+1);
	if (! (len & 1))				// force it even # of bytes
		++len;
	// Place the entry in row just created
	LSetCell((Ptr)&theEntry, 
		sizeof(theEntry)-(sizeof(theEntry.fileName)-1)+len, cSize, theList);

} // end of routine Add_toList


 /*********************************************************************
 ***************	select a given name row  **************************
 *********************************************************************/

static void  SelectNameRow(short thisRow, //I name list row that should be selected
 				const myStaticPtr sp)   //I pointer to my static storage
{ 
	Point selPt;

	SetPt (&selPt, 0, sp->selRow=thisRow);
	LSetSelect (TRUE, selPt, sp->nameList);
	
} // end of routine SelectNameRow

 /*********************************************************************
 ***************	display location for given name cell **************
 *********************************************************************/

static void  DisplayLoc(short thisRow,		//I name list row that was selected
 				const myStaticPtr sp)   //I pointer to my static storage
{ 
	OSErr		err;					// return code
	ListEntry 	theEntry;				// a list entry
	Point 		theCell;				// the cell to be retrieved
	short 		vref;					// volume ref num of selected name
	long		dirID;					// parent dir ID of selected name
	Str63		localName;				// name of a parent directory
	short		len;					// length of list entry
	Rect		tempRect;				// temporary rectangle

	LDelRow (0, 0, sp->locationList);	// we will redraw location list
	LDoDraw (FALSE, sp->locationList);	// do not display while building it 
	len = sizeof (ListEntry);
	SetPt (&theCell, 0, thisRow);		// get list entry of selected row
	LGetCell ((Ptr)&theEntry, &len, theCell, sp->nameList);
	vref = theEntry.vRefNum; 			// get its volume reference number
	dirID = theEntry.dirID; 			// get its parent directory ID
	
	// get names of all directories in its path and add them to location list
	while ((err = AL_findDirByID (vref, dirID, &dirID, localName)) == noErr)
		Add_toList (GetIconID(dirID, sp->aptr->volumeType), 0, 0, 
					localName, sp->locationList);
	GetIndString (localName, kAliasResID, kDeskStrIdx); // get the "Desktop" display string
	Add_toList (kSIDesktop, 0, 0, localName, sp->locationList); // show the "Desktop"
 
	LDoDraw (TRUE, sp->locationList);	// drawing on for next update
	tempRect = sp->locationRect;
	tempRect.right = tempRect.right - kScrMinOne;
	InvalRect (&tempRect);				// invalidate to force an update
}

 /*********************************************************************
 ***************	display cancel dialog if needed *******************
 *********************************************************************/

static void  DisplayCancelIfNeeded(const myStaticPtr sp)  //I pointer to my static storage
{ 

	if (sp->startCount > 0 && (TickCount() >= (sp->startCount + kCancelWait))) {
		// put up cancel through stop search button dialog box
		sp->cancelDialog = GetNewDialog(kAlCancelDLOGID, NULL, (WindowPtr)-1);
		SetWRefCon (sp->cancelDialog, (long)sp); // store address of our static storage
		sp->startCount = 0;					// remember that cancel dailog box is up
		AL_setArrowCursor();				// arrow cursor, so 'StopSearch' can be hit
	}
			
}

/**********************************************************************
 ***************	Draw User Items  **********************************
 **********************************************************************/

static pascal void DrawUserItem(WindowPtr theDialog, //I pointer to my dialog window
								short itemNo)		//I item number
{
	myStaticPtr sp;				// pointer to my static data	 
	ListHandle	itemToChange;	// item's handle
	Rect		itemBox;		// item's rectanglular box

	sp = (myStaticPtr) (GetWRefCon (theDialog)); // get our storage pointer

	switch (itemNo) {
	
	case kI_SelectOutline:
		// draw Select button outline
		DefaultOutline (sp->curCnt > 0 ? kDrawOutline : kEraseOutline, theDialog);
		break;

	case kI_NamesList:
	case kI_LocationList:
			
		if (itemNo == kI_NamesList) {
			itemBox = sp->nameRect;
			itemToChange = sp->nameList;
		} else {
			itemBox = sp->locationRect;
			itemToChange = sp->locationList;
		}
		// draw the names or location list outside rectangle and inside rectangle
		InsetRect(&itemBox, -1, -1);   	// Set for framing 
		FrameRect(&itemBox);   			// Frame the main rectangle
		itemBox.right = itemBox.right - kScrMinOne;// Make room for the scroll bar on the right 
		FrameRect(&itemBox);   			// Frame the inside rectangle
		LUpdate(theDialog->visRgn, itemToChange); // Update names list or location list
		break;

	default:
		break;
		
	} // end of itemNo switch

} // end of DrawUserItem routine

/**********************************************************************
 ***************	dlgHook for NewSFPGetFile for folder selection ****
 **********************************************************************/

static pascal short FolderSFGetHook(short mySFItem,		//I item number from SFPGetFile
							DialogPtr theDialog,	//I SFPGetFile dialog record pointer
							myStaticPtr sp) 
{
	Handle		itemToChange;	// needed for GetDItem and SetCtlValue 
	Rect		itemBox;		// needed for GetDItem 
	short		itemType;		// needed for GetDItem 
	Str255		buttonTitle;	// needed for GetIndString
	
	if (mySFItem == sfHookFirstCall) {
		/* Before the dialog is drawn, our hook gets called with a -1.
	   This gives us the opportunity to change 'open' title to 'Open Folder'. */
	   
		GetIndString(&buttonTitle, kAliasResID, kOpenFldrStrIdx);
		if (*buttonTitle != 0) { // if we really got the resource
			GetDItem(theDialog, getOpen, &itemType, &itemToChange, &itemBox);
			SetCTitle((ControlHandle)itemToChange,&buttonTitle);
		}
	} else {
		GetDItem(theDialog, kI_SelectFolder, &itemType, &itemToChange, &itemBox);
		if (sp->mySFreply.sfType == 0 && (Length(sp->mySFreply.sfFile.name) == 0)) {
			// nothing is selected, deselect 'Select Folder' button
			if ((*(ControlHandle)itemToChange)->contrlHilite == kButtonActive) 
				HiliteControl ((ControlHandle)itemToChange, kButtonInactive);
		} else { // if 'Select Folder' not active, activate it
			if ((*(ControlHandle)itemToChange)->contrlHilite == kButtonInactive) 
				HiliteControl ((ControlHandle)itemToChange, kButtonActive);
		}
	
		if (mySFItem == kI_SelectFolder) { // the user selected a folder
			sp->folderSelected = TRUE; // mark it as such, otherwise can't distinguish from real 'Cancel'
			mySFItem = getCancel;	// tell SFPGetFile to get out
		}
	}
	
	return(mySFItem);
}

/**********************************************************************
 ***************	Filter for ModalDialog call  **********************
 **********************************************************************/

static pascal Boolean MyFilter (DialogPtr theDialog,	//I dialog record pointer
						EventRecord *theEvent,			//I event record pointer
						short *itemHit)					//O item hit
{
	Point selPt;						// current selection point
	char ch;							// key-down character
	myStaticPtr sp;						// pointer to my static data
	Boolean returnFromFilter;			// answer from client's ModalDialog filter

	sp = (myStaticPtr) (GetWRefCon (theDialog)); // get our storage pointer
	
	if (sp->modalFilter != NULL) {		// give client a chance to filter events
		returnFromFilter = ((ModalFilterWithCallback)(*(sp->modalFilter)))
									(theDialog, theEvent, itemHit, sp->yourDataPtr);
		if (returnFromFilter)	
			return (TRUE);				// client handled the event
	}

	switch (theEvent->what) {
	
	case mouseDown:
		selPt = theEvent->where;     	// Get position of click
		GlobalToLocal(&selPt);   		// Convert to local coordinates

		if (PtInRect(selPt,&sp->nameRect)) { // See if in names list
			sp->DoubleClick = LClick(selPt, theEvent->modifiers, sp->nameList); // select it
			if (selPt.h < (sp->nameRect.right - kScrMinOne)) { // not in scroll bar
				sp->prevSel = sp->selRow;	// remember previous row to avoid unnecessary redraw
				SetPt (&selPt, 0, 0);		// get the selection row > 0
				if (LGetSelect(TRUE, &selPt, sp->nameList)) { // got the selection row
					sp->selRow = selPt.v;	// newly selected row
					*itemHit = sp->DoubleClick ? kI_Select : kI_NamesList; //actual select or just a pick?
					return (TRUE);		// we will handle the event
				} else if (sp->curCnt > 0) { // pick in an empty selection and there are entries
					SelectNameRow ((sp->curCnt-1), sp); // select the last entry in the list
					*itemHit = kI_NamesList;//actual select or just a pick?
					return (TRUE);		// we will handle the event
				}
			} else { // click in scrollbar, return an item that does nothing
				*itemHit = kI_NameText;
				return (TRUE);
			}

		} else if (PtInRect(selPt,&sp->locationRect)) { // See if in display location list
		// do nothing for the contents region since this list is for display only.
		// if it is in the scroll bars, call LClick
			if (selPt.h >= (sp->locationRect.right - kScrMinOne))
				(void) LClick(selPt, theEvent->modifiers, sp->locationList);
		}  
		break;
		
	case keyDown:
		ch = theEvent->message & charCodeMask; // get the character
		switch (ch) { 					// which charcter was it?
		
		case kChrETX: // 'enter' or 'cr' means user has selected an alias
		case kChrCR: 
			if ((*(sp->ButtonCH[kI_Select-1]))->contrlHilite == kButtonActive) {
										// disabled during TryHard mode
				*itemHit = kI_Select; 	// pretend that Select button was hit 
				FlashButton (sp->ButtonCH[kI_Select-1]); // flash the Select button as feedback
				return(TRUE);			// we will handle the event 
			}
			
			
		case kChrESC: // 'ESC' or 'CMD-period' means user is canceling out
		case kChrPeriod:
			if (ch == kChrESC || (theEvent->modifiers & cmdKey)) { // 'ESC' or 'CMD-.'
				if ((*(sp->ButtonCH[kI_Cancel-1]))->contrlHilite == kButtonActive) { 
					// cancel disabled during TryHarder mode
					*itemHit = kI_Cancel; // canceling out of alias selection dialog box
					FlashButton (sp->ButtonCH[kI_Cancel-1]);// flash the Cancel button as feedback
					return(TRUE);		// we will handle the event 
				}
			}
			break;
			
		default:
			break;
			
		} // end of character code switch
		
		break; // from keydown case
		
	case updateEvt:
	case nullEvent:
		if (theEvent->what == updateEvt && theDialog == theEvent->message)
			break;	// could get an updateEvt for some other window instead of nullEvent
			
		if (sp->TryHarder) { // in TryingHarder mode
		
			if (sp->firstTry)  {// are we seeing the TryHarder mode for the first time? Then
			// TryHarder mode was set programatically, pretend that TryHarder button was pushed
				*itemHit = kI_TryHarder; // pretend that TryHarder button was hit 
				FlashButton (sp->ButtonCH[kI_TryHarder-1]); // flash the TryHarder button as feedback
			} else {
				*itemHit = kI_SearchInProgress; // TryHarder mode search is in progress
			}
			return(TRUE);			// we will handle the event 
		}
		break;

	defualt:
		break;
		
	} // end of switch statement
		
	return (FALSE);						// Let ModalDialog handle the event
	
} // end of MyFilter routine

 /**********************************************************************
 ***************	Show empty dialog box initially *******************
 *********************************************************************/

static void Display_aliasDialog (const Str31 fTypeName, //I descriptive name of file type
  				const myStaticPtr sp)   	//I pointer to my static storage
{
	Rect    	dataBounds; 				// Rect to setup the list  
	Point   	cSize;  					// Pointer to a cell in a list 
	FontInfo	thisFontInfo;				// info about application font
	Rect		inRect;						// inside rectangle for a list
	short		Dtype;						// dialog item type
	Handle		Ditem;						// dialog item handle
	Rect		box;						// dialog itm box
	register short i;						// do loop index
	char		*s;							// temp char string pointer
	Str255		localName;					// local name string

	sp->aliasDialog = GetNewDialog(kAliasDLOGID, NULL, (WindowPtr)-1);// Bring in the dialog resource 
	SetWRefCon (sp->aliasDialog, (long)sp); // store address of our static storage
	AL_setArrowCursor();					// arrow cursor
	SetPort(sp->aliasDialog);  				// Prepare to add conditional text 
	sp->alSICNHandle = (SICNHandle)(GetResource('SICN', kAliasResID)); // load all SICNs

	// display title with it's  text and two parameters.
	// first is the descrptive name for the file type, second is the file name.
	if (Length(fTypeName) > 0) // client supplied file type name
		s = fTypeName;
	else 
		GetIndString (s=localName, kAliasResID, 
			sp->aptr->thisAliasKind == kFileAlias ? kDocStrIdx : kFldrStrIdx);
	ParamText (s, sp->aptr->fileName,'','');		
	
	// get all button's control handles
	for (i=1; i<=kNumButtons; i++) {
		GetDItem (sp->aliasDialog, i, &Dtype, &Ditem, &box); // get button control item
		sp->ButtonCH[i-1] = (ControlHandle) Ditem;
	}
	// get Select button outline box and install its userItem draw proc
	GetDItem (sp->aliasDialog, kI_SelectOutline, &Dtype, &Ditem, &inRect);
	SetDItem (sp->aliasDialog, kI_SelectOutline, Dtype, (Handle)&DrawUserItem, &inRect);
	
	// get name list userItem and install its userItem draw proc
	GetDItem (sp->aliasDialog, kI_NamesList, &Dtype, &Ditem, &sp->nameRect);
	SetDItem (sp->aliasDialog, kI_NamesList, Dtype, (Handle)&DrawUserItem, &sp->nameRect);
	
	// set userItem draw proc for name list and location list rectanlges
	GetDItem (sp->aliasDialog, kI_LocationList, &Dtype, &Ditem, &sp->locationRect);
	SetDItem (sp->aliasDialog, kI_LocationList, Dtype, (Handle)&DrawUserItem, &sp->locationRect);

	SetRect(&dataBounds, 0, 0, 1, 0);		// Set list up, 1 column 
	DoSmFont();								// get info about small font
	GetFontInfo(&thisFontInfo); 			// Get the current font sizes 
	cSize.v = thisFontInfo.ascent + thisFontInfo.descent + thisFontInfo.leading;
	DoDlgFont();							// back to default font
	SetPt(&cSize, 0, (kSICNSize >= cSize.v) ? kSICNSize : cSize.v);
											// height of cell in the list
	
	// create name list
	inRect = sp->nameRect;
	inRect.right = inRect.right - kScrMinOne;// Make room for the scroll bar on the right 
	sp->nameList =  LNew(&inRect, &dataBounds, cSize, kAliasResID, sp->aliasDialog, 
				TRUE, FALSE, FALSE, TRUE);	// Make the list 
	(*(sp->nameList))->selFlags = lOnlyOne + lNoNilHilite;// Set the attributes 

	// create location list
	inRect = sp->locationRect;
	inRect.right = inRect.right - kScrMinOne;// Make room for the scroll bar on the right
 	sp->locationList =  LNew(&inRect, &dataBounds, cSize, kAliasResID, sp->aliasDialog, 
				TRUE, FALSE, FALSE, TRUE); // Make the list 
	(*(sp->locationList))->selFlags = lOnlyOne + lNoNilHilite;// Set the attributes 

 	// Patch the refcon fields in the ListHandles so that the dummy LDEFproc
	// can find the real, linked-in LDEFproc which is written in C.
	// Also, save the static storage pointer so that LDEF can access it.
	(*(sp->nameList))->refCon = &Name_LDEF;
	(*(sp->locationList))->refCon = &Location_LDEF;	
	(*(sp->nameList))->userHandle = (Handle)sp;
	(*(sp->locationList))->userHandle = (Handle)sp;

} // end of Display_dialog routine
  
/*********************************************************************
********  done trying harder for now, provide feedback ***************
*********************************************************************/ 
static void DoneTryingHard(Boolean forEver,	//I TryHard done, is it for ever done?
   				const myStaticPtr sp)   	//I pointer to my static storage
{
	SysBeep(0);								// beep to let the user know
	sp->TryHarder = FALSE;  				// set our global flag
	if (forEver && sp->dirStack != NULL)	// dispose directory stack if done forever
		DisposHandle ((Handle)sp->dirStack);
	if (sp->startCount == 0 && sp->cancelDialog != NULL) { // a cancel dialog was put up
		DisposDialog(sp->cancelDialog); 	// Flush the cancel dialog out of memory
		sp->cancelDialog = NULL;
		sp->startCount = 1;					// mark the flag
	}
	// if done forEver with TryingHard, make the button inactive, otherwise active.
	HiliteControl(sp->ButtonCH[kI_TryHarder-1], 
		(forEver) ? kButtonInactive : kButtonActive); 
	ChangeOthers(kButtonActive, sp);		// all other buttons are now active
	if (sp->selRow == -1 && sp->curCnt > 0) { // nothing was selected yet
		SelectNameRow (0, sp);				// select first entry by default
		DisplayLoc (0, sp);					// display path of default selection
	}
	AL_setArrowCursor();					// back to arrow cursor
}

/*********************************************************************
******** Accept a file as an alias ***********************************
*********************************************************************/ 
static OSErr AcceptFile (short vref,		//I volume reference number
				long dirID, 				//I directory ID of the file
				const StringPtr fname, 		//I file name
   				const myStaticPtr sp)   	//I pointer to my static storage

{
	OSErr err;								// result code
	Boolean returnFromFilter;				// client wants to filter it or not?						
	
	if (sp->fileFilter != NULL) {	// client supplied file filter proc
		if ((err = AL_filterFile (vref, dirID, fname, sp->fileFilter, sp->yourDataPtr,
				 (sp->CSrch) ? NULL : &sp->asyncCPB,
				 kRealCPB, &returnFromFilter, &sp->quitFlag)) != noErr)
			return (err);
		if (returnFromFilter) 	// client says don't display it
			return (noErr);	
	}
	
	if (sp->alList == NULL) { // allocate the slow search alias list buffer
		sp->alList = (CanonicalFileSpecHandle)
			(NewHandle(sizeof(CanonicalFileSpec) * sp->curLim));
		if (sp->alList == NULL)
			return (MemError());
		HLock((Handle)sp->alList);	
	} else if (sp->curCnt >= sp->curLim) { // ran out of room in alias list buffer
		sp->curLim += kMaxCSrchEnt;				// double its size
		HUnlock((Handle)sp->alList);	
		SetHandleSize ((Handle)sp->alList, 
			(sizeof(CanonicalFileSpec) * sp->curLim)); // grow alias list
		err = MemError(); 
		HLock((Handle)sp->alList);	
		if (err != noErr) { // could not extend block
			AL_displayAlert(kMem1StrIdx);
			return (err);
		}
	}
	
	if (AL_updateAList(vref, dirID, fname,	 // add an entry to alias list
					&sp->curCnt, sp->curLim, (*(sp->alList)))) { // room in the list & not duplicate
			Add_toList ((sp->aptr->thisAliasKind == kFileAlias) ? kSIDocument : kSIFolder, 
					vref, dirID, fname, sp->nameList);	 // add it to Names List
	}
	
	return (noErr);
}

/*********************************************************************
 ***************	Find file(s) or directory(s) by CatSearch	******
 *********************************************************************/

static OSErr FindByCatSrch( 	short vref,			//I volume reference number/WD ref
						const AliasPtrPriv aptr,	//I alias record pointer
						long specBits,				//I search specification mask
						long ioSearchTime,			//I length of time to run search
						short lim,					//I alias list buffer limit
						short *fcnt,				//I/O current num of entries in buffer
						CanonicalFileSpecList flist,//O alias list buffer
						CatPositionRec *prec)		//I/O CatSearch IO position record
						
{						
	OSErr err = paramErr;			// result code, assume CatSrch not available
	CSParam cspb;					// CatSearch parameter block
	CInfoPBRec cpb1, cpb2;			// CatSearch setup catlog param blocks
	Str63 localName;				// local temporary name
	FSSpec ans[kMaxCSrchEnt];		// maximum CatSearch return entries
	register short i;				// for loop index

	// set up CatSearch parameter block
	cspb.ioNamePtr = cspb.ioOptBuffer = NULL;
	cspb.ioVRefNum = vref;
	cspb.ioMatchPtr = ans;
	cspb.ioReqMatchCount = (lim < kMaxCSrchEnt) ? lim : kMaxCSrchEnt;
	cspb.ioSpecBits = fsSBFlAttrib + specBits;
	if (aptr->thisAliasKind == kDirAlias && (specBits & fsSBFlFndrInfo))
		// for files, type and creator are important but for folders they are not
		cspb.ioSpecBits -= fsSBFlFndrInfo;
	cspb.ioSpec1 = &cpb1;
	cspb.ioSpec2 = &cpb2;
	cspb.ioSearchTime = ioSearchTime;	// length of time to run search
									// later on, may want to refine it depending upon performance
	cspb.ioCatPosition = *prec;		// input CatSearch position record
	
	if (cspb.ioSpecBits & (fsSBFullName | fsSBPartialName)) { // by name specified
		AL_copyPString (aptr->fileName, localName);
		cpb1.hFileInfo.ioNamePtr = localName; // put name in first catalog param blk
	}
	else 
		cpb1.hFileInfo.ioNamePtr = NULL; // name was not important
	cpb2.hFileInfo.ioNamePtr = NULL;
	
	cpb1.hFileInfo.ioFlAttrib = (aptr->thisAliasKind == kDirAlias) ? kDirMask : 0;
	cpb2.hFileInfo.ioFlAttrib = kDirMask;// only bit 4 is important
	
	if (cspb.ioSpecBits & fsSBFlCrDat) // creation date mask was specified
		cpb1.hFileInfo.ioFlCrDat = cpb2.hFileInfo.ioFlCrDat = aptr->fileCrDate; 

	if (cspb.ioSpecBits & fsSBFlFndrInfo) { 
		// for files, type and creator are important
		cpb1.hFileInfo.ioFlFndrInfo.fdType = aptr->fileType;;
		cpb1.hFileInfo.ioFlFndrInfo.fdCreator = aptr->fdCreator;

		// create the mask bits so that only type and creator are important.
		cpb2.hFileInfo.ioFlFndrInfo.fdType = 0xFFFFFFFF;
		cpb2.hFileInfo.ioFlFndrInfo.fdCreator = 0xFFFFFFFF;
		cpb2.hFileInfo.ioFlFndrInfo.fdFlags = 0;
		cpb2.hFileInfo.ioFlFndrInfo.fdLocation.h = 0;
		cpb2.hFileInfo.ioFlFndrInfo.fdLocation.v = 0;
		cpb2.hFileInfo.ioFlFndrInfo.fdFldr = 0;
	}
		
	
	err = PBCatSearch (&cspb, false);
	if (err == noErr || err == eofErr) { //if reached end of volume, it is ok
		for (i = 0; i < cspb.ioActMatchCount; ++i) { // load alias list buffer
			(void) AL_updateAList (vref, ans[i].parID, &ans[i].name, fcnt, lim, flist);
			if (*fcnt >= lim)	// no room in return list
				break;
		}
		if (err == noErr) // new position not meaningful for eofErr or fnfErr
			*prec = cspb.ioCatPosition;	// return the new position
	}

	return (err);			
	
} // end of FindByCatSrch routine

/*********************************************************************
******** Perform next CatSearch **************************************
*********************************************************************/ 
static OSErr NextCatSrch(const myStaticPtr sp)   	//I pointer to my static storage		
{
	OSErr 		err, err2 = noErr; 	// result code
	CanonicalFileSpec	nextFile;	// next file that is found
	short 		fcnt = 0;			// search count

	/* currently, CatSearch is by matching partial name, type, creator and creation date.
	 Also, do one at a time.  This gives user a chance to cancel out of search.
	 curPrec maintains the current CatSearch position and hence search could be continued. */

	err = FindByCatSrch (sp->curVref, sp->aptr, fsSBFlFndrInfo + fsSBFlCrDat,
					KMaxCSrchQuant, 1, &fcnt, &nextFile, &sp->curPrec);
	if (err == noErr || err == eofErr) { // this is ok, just reached end of volume while CatSearching
		if (fcnt != 0) // if nothing found, get out. otherwise, accept the alias
			err2 = AcceptFile(sp->curVref, nextFile.dirID, nextFile.fileName, sp);
	}

	return (err ? err : err2);	
}

/*********************************************************************
******** Perform next asynchronous search ****************************
*********************************************************************/ 
static OSErr NextAsyncSrch(const myStaticPtr sp)   	//I pointer to my static storage
{
	OSErr err;					// return code
	
	err = sp->asyncCPB.hFileInfo.ioResult; // result of last async search
	if (err == noErr) { 		// last async search was successful
		if (sp->asyncCPB.hFileInfo.ioFlAttrib & kDirMask) // found a directory
			if (sp->curIdx >= (kMaxDirStack-1)) { // ran out of directory stack
				AL_displayAlert(kMem2StrIdx);	// too many dirs on volume 
				err = memFullErr;		// out of memory
			}
			else 				// add it to directory stack in order to search it later
				(*(sp->dirStack))[++sp->curIdx] = sp->asyncCPB.dirInfo.ioDrDirID;
		// does type, creator and creation date match? If yes, accept it as an alias.
		if (AL_attribMatches(sp->aptr, &sp->asyncCPB, kMatchFileNumber))
			err = AcceptFile (sp->curVref, sp->asyncCPB.hFileInfo.ioFlParID, 
								sp->asyncCPB.hFileInfo.ioNamePtr, sp);
		++sp->asyncCPB.hFileInfo.ioFDirIndex;	// search next file in the current dir

	} else if (err == fnfErr || err == dirNFErr || err == afpAccessDenied) { 
		// these are valid errors. Either a directory was completely searched, or for
		// some reason the directory has disappeared or we don't have access to search it.
		if (sp->curIdx >= 0) {	// any more directories remaining to be searched?
			sp->curDir = (*(sp->dirStack))[sp->curIdx--];	// next directory from stack
			sp->asyncCPB.hFileInfo.ioFDirIndex = 1;	// start search at the beginning
			err = noErr;		// force an async pending search 
		}

	} else
		; // ******************** serious error, get out **************
		
	if (err==noErr) { 			// new I/O request if no fatal error
		sp->asyncCPB.dirInfo.ioDrDirID = sp->curDir;// replace file number with dirID 
		(void) PBGetCatInfo (&sp->asyncCPB, TRUE); // issue an async CatInfo call
	}
	
	return (err);				// error from last async call or memFullErr
}

/*********************************************************************
******** Find next matching alias via CatSearch or slow async search *
*********************************************************************/ 
static OSErr Do_findNext(const myStaticPtr sp)   	//I pointer to my static storage	
{
	if (sp->CSrch)						// CatSearch is available
		return (NextCatSrch(sp));		// do it
	else if (sp->asyncCPB.hFileInfo.ioResult != 1) // Did last async call complete?
		return (NextAsyncSrch(sp));	// yes, start the next one
	else
		return (noErr);				// last async did not complete, just return
}

/*********************************************************************
******** User pushed the TryHarder button ****************************
*********************************************************************/ 
static OSErr Do_TryHarder(const myStaticPtr sp)   	//I pointer to my static storage
{
	OSErr err = noErr;				// return code 
	
	HiliteControl (sp->ButtonCH[kI_TryHarder-1], kButtonInactive); // make it inactive for now
	ChangeOthers(kButtonInactive, sp);	// while trying hard, all other buttons are inactive
	if (sp->startCount == 1) {			// flag says no cancel dialog up
		sp->startCount = TickCount();	// remember when try hard started
		SetCursor(*(GetCursor(watchCursor))); // cursor is in watch mode only before cancel dialog shows up
	}
	sp->TryHarder = TRUE;				// set the  flag
	if (sp->firstTry) {					// if first time, need to allocate & initialize
		err = NextCatSrch(sp);
		if (err != noErr && err != eofErr) { // CatSearch not available
			sp->CSrch = FALSE;			// remember that fact
			// allocate a directory stack to save all dirIDs found on the volume.
			// so each can be searched later on for matching alias.
			if (sp->dirStack == NULL) { // not already allocated
				sp->dirStack = (LongsHandle)(NewHandle(sizeof(long)*kMaxDirStack));
				if (sp->dirStack == NULL)
					return (MemError());
			}
			// allocated the directory stack
			(*(sp->dirStack))[0] = sp->curDir; // from root directory
			sp->curIdx = 0;			// only root directory in the stack for now
			sp->asyncCPB.hFileInfo.ioResult = fnfErr; // this will start initial search
			sp->asyncCPB.hFileInfo.ioCompletion = NULL; // no completion routine
			sp->asyncCPB.hFileInfo.ioNamePtr = &sp->curFname;// returned name from GetCatInfo
			sp->asyncCPB.hFileInfo.ioFVersNum = 0;	  // keep GetCatInfo happy
			sp->asyncCPB.hFileInfo.ioVRefNum = sp->curVref; // volume to search
			err = NextAsyncSrch(sp);// start the async search
		} else if (err == eofErr) { 	// end of volume reached while CatSearching
			NextVolumeSearch (sp); 		// search the next volume
			err = noErr;				// not a real error
		}
		sp->firstTry = FALSE;			// from now on, no allocation and initialization unless next volume is searched
	}
	return (err);
}

/*********************************************************************
 ***************	Initialize  volume search statics ****************
 *********************************************************************/

static void Init_volSearchStatics(const myStaticPtr sp) //I pointer to my static storage
{
	sp->firstTry = TRUE;			// will be TryingHard the first time
	sp->CSrch = TRUE;				// assume CatSearch available
	sp->quitFlag = FALSE;			// assume client filter proc does not want to quit
	sp->curDir = kRootDirID;		// current directory we are searching
	sp->curIdx = 0;					// current directory index for the search
	sp->curPrec.initialize = 0;		// initialize CatSearch 
}

/*********************************************************************
 ***************	Perform next mounted volume search ****************
 *********************************************************************/

static void NextVolumeSearch(const myStaticPtr sp) //I pointer to my static storage
{
	short nextVref;					// volume ref num of next volume to be searched

	for (;;) {
		if (sp->vcbPtr == NULL) {
			DoneTryingHard(kDoneForever, sp);// done forever TryingHard
			return;
		} else {
			nextVref = sp->vcbPtr->vcbVRefNum;
			sp->vcbPtr = (VCB *) sp->vcbPtr->qLink;
			if (sp->firstVref == nextVref)
				continue;
			else
				break;
		}
	}
	// we found a valid vrefNum that needs to be searched
	sp->curVref = nextVref;			// this the current one we are searching
	sp->TryHarder = TRUE;			// again trying harder on next volume
	Init_volSearchStatics (sp);		// init volume search statics
	/* before we do the next volume search, make sure the cancel dialog box
	comes up if the timer has expired during (last CatSearch that returned eofErr) */
	DisplayCancelIfNeeded (sp); // display cancel dialog box if needed
	return;	
}

/*********************************************************************
******** User pushed the AnotherOne button ***************************
*********************************************************************/ 
static void Do_AnotherOne(const myStaticPtr sp)   	//I pointer to my static storage
{
	Point where;					// display location of SFPGetFile dialog box
	SFTypeList	typeList;			// alias type passed to SFGetFile

	sp->folderSelected = FALSE;	 	// when we come back from SFPGetFile, was a Folder really selected?
	//  SFGetFile is linked here.  User presumably knows where the aliased 
	//  entity is.  Let him find it through modified SFPGetFile modal dialog.
	SetPt (&where, sp->aliasDialog->portRect.left+kSFGetHOffset, 
			sp->aliasDialog->portRect.top+kSFGetVOffset);
	LocalToGlobal(&where);	// put SFGetFile dialog box nicely offseted from alias dialog
	typeList[0] = sp->aptr->fileType; // match original alias type only
	if (sp->aptr->thisAliasKind == kDirAlias) {
		typeList[0] = kSelectNoFile; // show only folders
		
		CustomGetFile(NULL, 1, &typeList, &sp->mySFreply, kAlFldrSFGetID,
						where, FolderSFGetHook, NULL, NULL, NULL, sp);		// put up the folder selection dialog
	} else 
		CustomGetFile(NULL, 1, &typeList, &sp->mySFreply, kAlFileSFGetID,
						where, NULL, NULL, NULL, NULL, sp);					// put up the file/volume selection dialog 	
	return;
}

/********************************************************************
 ***************	Initialize  my static storage once ***************
 *********************************************************************/

static void Init_s(const myStaticPtr sp)   	//I pointer to my static storage
{
	sp->aliasDialog = NULL; 		// Pointer to alias dialog
	sp->cancelDialog = NULL;		// Pointer to cancel further search dialog
	sp->alSICNHandle = NULL; 		// handle to SICN resource
	sp->alList = NULL;				// handle to alias list buffer
	sp->nameList = NULL;			// handle to list of names
	sp->locationList = NULL;		// handle to the list displaying location
	sp->DoubleClick = FALSE;  		// Flag to say that a double click on a list happened
	sp->TryHarder = FALSE;			// Flag to say if in TryHarder mode
	sp->startCount = 1;				// mark the flag to say that the cancel dialog box is not up
	sp->selRow = -1;    			// Current selection row in names list
	sp->prevSel = -1;				// previous selection row in names list
	sp->dirStack = NULL;			// handle to directory stack
	sp->curCnt = 0;					// current count of alias list buffer
	sp->curLim = kMaxCSrchEnt;		// current max entries allocated in alias list
	sp->vcbPtr = (VCB *)GetVCBQHdr()->qHead; // VCB Q header
	Init_volSearchStatics (sp);		// init volume search statics
}

 /*********************************************************************
 ***************	Select an alias	with dialog box *******************
 *********************************************************************/

pascal OSErr AL_selectAliasWithDialog(const CanonicalFileSpec *fromFile, //I from file 
						const Str31 fTypeName,			//I descriptive name for file type
						short cnt,						//I #of entries found during fast search
						CanonicalFileSpecList alFastList, //I fast search answer
						AliasFilterProcPtr aliasFilter,	//I file filter proc pointer
						AliasHandlePriv Alias,			//I/O alias record handle
						CanonicalFileSpec *toFile, 		//O returned toFile
						ModalFilterWithCallback filterProc, //I modal filter proc pointer
						Ptr yourDataPtr)				//I additional param passed to filter procs

{
	OSErr 		err = noErr;		 // result code	
	short    	itemHit;   			// Get selection from ModalDialog
	register 	int i;				// do loop index
	char 		masterFlags;		// flags to remember before locking and unlocking handles
	Boolean   	ExitDialog = FALSE; // Flag used to exit the Dialog 
	long		fnum;				// file number
	long		parDirID;			// parent directory ID
	Boolean		needsUpdate;		// alias record needs update or not?
	GrafPtr		savePort;			// save current port for restoring later
	register CanonicalFileSpec *fptr; // pointer into alias list 

	/* the variables in structure myStaticVars are used by many routines in this file.  
		They are meant as static variables.  Since static is not allowed in stand-alone
		code resource, we are using pointer to this structure to simulate having static
		variables.  None of these variables are needed across SelectAlias calls.  They
		are valid while the dialog box is up.
	*/
	myStaticPtr sp;					// pointer to our static storage
		
	if ((sp = (myStaticPtr)NewPtr(sizeof(myStaticVars))) == NULL)
		return (MemError());

	// do initialization of our storage variables
	Init_s(sp);
	sp->fileFilter = aliasFilter;	// remember client supplied file filter proc ptr
	sp->modalFilter = filterProc;	// remember client supplied modal filter proc ptr
	sp->yourDataPtr = yourDataPtr;	// remember additional param for both filter procs	

	masterFlags = HGetState ((Handle)Alias); // remember master flags
	HLock((Handle)Alias);			// lock the passed alias record	
	sp->aptr = *Alias;				// pointer to alias record as a  variable

	// obtain volume reference number for the volume that we are searching
	if (cnt > 0) 					// found >=2 during fast search
		// remember the first volume that we will search
		sp->curVref = alFastList[0].vRefNum;
	else {
		err = AL_findVolume(sp->aptr, 0 /*no autoMount*/, &sp->curVref, &needsUpdate, NULL); // obtain curVref
		if (err == nsvErr)
			// could not find original aliase's volume, start with first in VCB chain
			sp->curVref = sp->vcbPtr->vcbVRefNum;
		else if (err != noErr) 		// fatal error
			goto EXIT;
	}
	sp->firstVref = sp->curVref;	// remember the first volume that we searched

	// display initial dialog box
	GetPort (&savePort);			// save the current port
	Display_aliasDialog(fTypeName, sp); // pass the descriptive file type name

	if (cnt > 0) { 					// was anything found during fast search?
		// fill entries of name list
		LDoDraw(FALSE, sp->nameList);// don't draw list while creating it
		
		for (i=0; i<cnt; i++)		// add matching entries to name scroll list
			if ((err = AcceptFile (alFastList[i].vRefNum, alFastList[i].dirID, &alFastList[i].fileName, sp)) != noErr)
				goto EXIT;
				
		SelectNameRow (0, sp);     // select the first entry by default

		// fill location list with the path of first alias (the default selection)
		DisplayLoc (sp->selRow, sp);
		
	} else 	{ // nothing matching was found, kick in TryHarder search automatically
		sp->TryHarder = TRUE; // let MyFilter convert this flag into 'TryHarder' button hit
	}

	LDoDraw(TRUE, sp->nameList);	// let us see list boxes initially
	LDoDraw(TRUE, sp->locationList);
	
	do {							// Start of dialog handle loop 
		ModalDialog(MyFilter, &itemHit); // Wait until an item is hit 
		 
		switch (itemHit) {
		
		case kI_Select:				// Select Button OR Stop Search Button of cancel dialog
			
			if (sp->TryHarder && (sp->startCount == 0)) { // was Stop Search button hit?
			// canceling out of TryHard mode with cancel Dialog Box up?
				DoneTryingHard(kNotDoneForever, sp); // not done forever, user may push TryHarder again
			} else {				// Select button of main dialog was hit
				
				ExitDialog = TRUE;  	// Exit the dialog when this selection is made
				fptr = &(*(sp->alList))[sp->selRow];
				toFile->vRefNum = fptr->vRefNum;
				toFile->dirID = fptr->dirID;
				AL_copyPString (fptr->fileName, toFile->fileName);
				err = AL_findByName (toFile->vRefNum, toFile->dirID, toFile->fileName,
										&fnum, &parDirID); // make sure that the file still exists
			}
			break;

		case kI_Cancel: 			// Handle the Cancel Button
			ExitDialog =TRUE;   	// Exit the dialog when this selection is made 
			err = userCanceledErr;	// return user cancelling out of operation status
			break;
		
		case kI_TryHarder: 		// Handle the Try Harder Button
			if ((err = Do_TryHarder(sp)) != noErr) // start further search
				ExitDialog =TRUE;   // Exit the dialog, serious error occurred
			break;
			
		case kI_SearchInProgress: 	// searching during TryHarder mode is in progress
		
			if (sp->quitFlag) { // client filter proc told us to quit current 'TryHard' seacrh
				DoneTryingHard(kNotDoneForever, sp); // not done forever, user may push TryHarder again
				sp->quitFlag = FALSE; // reset the flag

			} else {
			
				DisplayCancelIfNeeded (sp); // display cancel dialog box if needed
				// find the next matching alias and display it. If can't find any more,
				// provide feedback to the user.
				sp->TryHarder = (Do_findNext(sp) == noErr) ? TRUE : FALSE;
				if (! sp->TryHarder) 	// done searching this volume
					NextVolumeSearch(sp);// try searching next mounted volume
			}
			break;

		case kI_AnotherOne:			// Handle the AnotherOne button 
			
			Do_AnotherOne (sp);		// do it!
			if (sp->mySFreply.sfGood || sp->folderSelected) { // return the selected file/folder as the returned alias
				BlockMove ((Ptr)&sp->mySFreply.sfFile, (Ptr)toFile, sizeof(CanonicalFileSpec));
				ExitDialog = TRUE; 	// user chose the alias, now get out
			} 						//  else user cancelled out of SFPGetFile dialog
			break;
		
		case kI_NamesList:			// Handle a pick in the Names List
			if (sp->prevSel != sp->selRow)	// display its location if a new row was picked
				DisplayLoc (sp->selRow, sp);
			break;
					
		case kI_LocationText:		// if any of the presumably disabled items
		case kI_NameText:			// nothing to do, just go back in loop
		case kI_Title:
		case kI_LocationList:
			break;
		
		default:					// what was it?
			ExitDialog = TRUE;
			err = fnfErr;			// alias not found
			break;
			
		} // end of switch statement
		 
	} while (! ExitDialog);    		// Handle dialog items until exit selected 
	 	
EXIT:
	// release resources, dispose of handles and dialogs.
	if (sp->alSICNHandle != NULL)
		ReleaseResource ((Handle)sp->alSICNHandle); // release SICN resource
	if (sp->nameList != NULL)			// names list
		LDispose (sp->nameList);
	if (sp->locationList != NULL)		// location list
		LDispose (sp->locationList);
	if (sp->alList != NULL)				// deallocate alias list buffer
		DisposHandle ((Handle)sp->alList);
	if (sp->startCount == 0 && sp->cancelDialog != NULL) // a cancel dialog was put up
		DisposDialog(sp->cancelDialog); // Flush the cancel dialog out of memory
	if (sp->aliasDialog != NULL) {
		DisposDialog(sp->aliasDialog); 	// dispose of our dialog
		SetPort(savePort);				// restore the original port
	}

	DisposPtr ((Ptr)sp);				// dispose of our static storage
		
	HUnlock((Handle)Alias);				// unlock the alias record
	HSetState((Handle)Alias, masterFlags); // restore original state

	return (err);
	
} // end of routine AL_selectAliasWithDialog

/***************************** end of alDialog.c file *****************************/

