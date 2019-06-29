/*
	File:		MenuMgr.c

	Contains:	C language portions of Menu Manager

	Written by:	Kevin MacDonell and David Collins

	Copyright:	© 1991-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<14>	 11/3/92	DTY		Strip out unnecessary includes.
		<13>	 5/29/92	DCL		Included TextUtils.h. NBreakTablePtr moved there for the New
									Inside Mac.
		<12>	 4/22/92	DTY		#1024445,<KSM>: If at first you don’t succeed… The last change
									wasn’t doing what we wanted it to do.  Move the line from the
									last checkin up a line so that the script code will be forced to
									smRoman a little earlier if we’re trying to insert 'FONT' 0.
		<11>	  4/1/92	KSM		#1024445,<DTY>: Fix case where a Chicago 'FONT' (family id = 0)
									maps to the current system script instead of Roman.
		<10>	 3/30/92	DC		#1025860,<KSM>: Fix IDToScriptCode. Boundary case would cause ID
									16384 to map to Roman script.
		 <9>	 3/27/92	DC		#1025860,<KSM>: Added InsertIntlResMenu to allow a user to filter a
									resource by its associatiation with a script system
		 <8>	 3/20/92	DCL		#1025005,<KSM>:
									Included Language.h because: For New IM: Moved some declarations
									and routines from Packages.[ph] to Language.[ph] #1025005
		 <7>	 2/11/92	DTY		#1021194, #1021189: To deal with resource names containing
									metacharacters, call InsMenuItem with a space, then SetItem with
									the actual menu item string.
		 <6>	12/30/91	KSM		xxx,#1018832: (Bruges fix) AddResMenu must call CalcMenuSize
									when finished!
		 <5>	10/10/91	PKE		#1012949, For Cube-E (and Bruges): Use new Font2RealScript
									routine (ignores FontForce & script enable status) instead of
									Font2Script to determine script of fonts in menus. Need to
									include ScriptPriv.h. Also added bug # to previous comment.
		 <4>	 10/7/91	KSM		#1012768, For Cube-E (and Bruges): Fix bug where FONT and FOND
		 							names were duplicated.
		 <3>	 9/23/91	DTY		Dean finds a better way of making fonts load in the System Heap,
									so take out the last change.
		 <2>	 9/19/91	DTY		Dean makes a unilateral decision at 1 in the morning to make all
									'FOND's and 'FONT's load into the System heap.
		 <1>	 8/22/91	KSM		first checked in
	To Do:
*/

#include <Types.h>
#include <Memory.h>
#include <Menus.h>
#include <Resources.h>
#include <Script.h>
#include <SysEqu.h>

#include <IntlUtilsPriv.h>
#include <ScriptPriv.h>

#define GetResLoad() (*(Boolean *)ResLoad)
#define GetItmIcon(ip) (*(ip + (*ip + 1)))
#define GetItmCmd(ip) (*(ip + (*ip + 2)))
#define kFirstType 'FOND'
#define kSecondType 'FONT'

/*--------------------------------------------------------------------------------------------------
	Purpose:	Compares 'rsrcName' with a contiguous subset of the items of the given menu
				and determines its internationally correct sort position within that menu.
	Inputs:		rsrcName	- name of item we wish to insert
				rsrcScript	- the script code of the string (to be internationally correct)
				theMenu		- the menu in which 'rsrcName' will be inserted
				afterItem	- the first item to compare against for insertion
				addedResources - the number of items to compare against (1st time is always zero)
	Outputs:	FindInsertIPt - the item number after which rsrcName should be inserted.
*/
short FindInsertIPt(StringPtr rsrcName, short rsrcScript, MenuHandle theMenu, short afterItem, short addedResources)
{
	StringPtr	itemRecPtr;
	short		i;

	HLock((Handle)theMenu);									// we can point into it
	SetResLoad(true);										// Let Int’l rsrcs load for IUTextOrderSys

	itemRecPtr = (**theMenu).menuData;						// Make pointer to menu title
	itemRecPtr += *itemRecPtr + 1;							// Skip over title
	for (i=afterItem-1;i >= 0; --i)							// Point to item 'afterItem'
		if (*itemRecPtr)									// Check to see we haven’t run off end
			itemRecPtr += (*itemRecPtr + 5);
		else break;

	// Do compares across only the items we’ve added to this menu
	for (; *itemRecPtr && (addedResources--); itemRecPtr += (*itemRecPtr + 5))
	{
		short cmpResult;
		// Do the Int’l correct compare.  Must use iuSystemScript and not zero as compare scripts.
		// Note that if the itmCmd field is $1c then the itmIcon field contains the script code.
		cmpResult = IUTextOrderSys(
			rsrcName+1, itemRecPtr+1, *rsrcName, *itemRecPtr,							// Strings
			rsrcScript,
			(GetItmCmd(itemRecPtr) == 0x1c) ? (char)GetItmIcon(itemRecPtr) : (char)iuSystemScript,	// Scripts
			0, 0 );																		// Languages
			
		if (cmpResult <= 0)									// true if (rsrcName <= itemName)
		{
			if (!cmpResult)									// if (rsrcName == itemName)
				afterItem = -1;								// show this is a duplicate
			break;											// otherwise afterItem is now correct
		}
		++afterItem;										// Insert after this item
	}
	SetResLoad(false);										// Restore ResLoad for caller
	HUnlock((Handle)theMenu);								// Set it free again

	return afterItem;
}

#define romanCieling 16384

short IDToScriptCode(short rsrcID)
{
	rsrcID -= romanCieling;
	
	if (rsrcID >= 0)
	{
		rsrcID >>= 9;
		++rsrcID;
	}
	else
		rsrcID = 0;
		
	return rsrcID;
}

/*--------------------------------------------------------------------------------------------------
	Purpose:	Loops thru each resource ID of the given type and inserts it into the menu.
				Empty names, names beginning with '.' or '%' are skipped.
	Inputs:		theMenu		- the menu in which the name of the resources will be inserted
				theType		- the resource type to add
				afterItem	- the item number after which rsrc names will be added
				fontFondFlag - TRUE if theType is 'FOND' or 'FONT' and we need to use
								the Script code in comparing the strings
	Outputs:	The menu is updated as appropriate.
*/
pascal void __InsertIntlResMenu(MenuHandle theMenu,
				ResType theType,
				short afterItem,
				short scriptFilter)
{
	Boolean oldResLoad;
	short nRsrcs, currSysScript, addedResources = 0;
	Boolean fontFondFlag = ((theType == kSecondType) || (theType == kFirstType));

	// If we are adding fonts, we will do a pass to add resources of type kFirstType and
	// then come back and do a pass for kSecondType.  We interpret the special script filter
	// values here and turn them into their functional values.
	// Otherwise, the script filter must be set to 'smAllScripts' to avoid sorting problems.

	currSysScript = GetEnvirons(smSysScript);

	// Interpret special ScriptMgr script values, get real values from pseudo-values
	if (scriptFilter == smSystemScript)
		scriptFilter = currSysScript;
	else if (scriptFilter == smCurrentScript)
		scriptFilter = FontScript();

	if (fontFondFlag)												// Special case?
		theType = kFirstType;										// Always start with first type

	{
		short nItems = CountMItems(theMenu);						// Find out # items in menu
		if (afterItem < 0) afterItem = 0;							// Insure reasonableness
		if (afterItem > nItems) afterItem = nItems;					// Insure min(afterItem,nItems)
	}

	oldResLoad = GetResLoad();
	SetResLoad(false);												// Just need the map info

	NextFONTPass:													// Comes here 2nd time for fonts
	if (!(nRsrcs = CountResources(theType))) goto PassDone;			// None to add...
		
	do
	{
		Handle	theRsrcH;

		theRsrcH = GetIndResource(theType, nRsrcs);						// Get each one in turn
		if (theRsrcH != NULL)
		{
			short	rsrcID, item;
			Str255	rsrcName;
			short rsrcScript;
			char *fc=&rsrcName[1];

			GetResInfo(theRsrcH, &rsrcID, &theType, rsrcName);
			if (!(*rsrcName) || (*fc == '.') || (*fc == '%')) continue;
			rsrcScript = fontFondFlag ? Font2RealScript(rsrcID) : IDToScriptCode(rsrcID);
			if ((rsrcID == 0) && (theType == 'FONT'))					// <12> Force Chicago FONT to be in the Roman script system
				rsrcScript = smRoman;
			if ((scriptFilter != smAllScripts) && (rsrcScript != scriptFilter)) continue;
			item = FindInsertIPt(rsrcName, rsrcScript, theMenu, afterItem, addedResources);
			if (item >= 0)	// negative means duplicate, skip this item
			{
				InsMenuItem(theMenu, "\p ", item);						// <7> Create an empty menu item after “item”.
				SetItem(theMenu, item + 1, rsrcName);					// <7> Now fill the new item with the resource name.
				// Only set the script code if fontFondFlag
				// and non-system font script to optimize menu drawing speed over this routine
				if ((fontFondFlag) && (rsrcScript != currSysScript))
				{
					SetItemCmd(theMenu, item+1, (char)0x1c);			// Show item has a script
					SetItemIcon(theMenu, item+1, rsrcScript);			// scripts go in icon field
				}
				++addedResources;										// Keep count of items added
			}
		}
	} while (--nRsrcs);

	PassDone:
	if (fontFondFlag && (theType == kFirstType))	// If doing fonts and only have done kSecondType...
	{
		theType = kSecondType;
		goto NextFONTPass;
	}
	SetResLoad(oldResLoad);
	CalcMenuSize(theMenu);
}

/*--------------------------------------------------------------------------------------------------
	Purpose:	Set up for calling InsResGuts for FONT/FOND menus.
	Inputs:		theMenu		- the menu in which the name of the resources will be inserted
				afterItem	- the item number after which rsrc names will be added
				scriptFilter- smAllScripts: Adds all the FONT/FONDs to the menu
							  smSystemScript: Adds those FONT/FONDs for the system script
							  smCurrentScript: Adds those FONT/FONDs for the current script
							  otherwise adds only those FONT/FONDs = to that script.
	Outputs:	The menu is updated as appropriate.
*/
pascal void __InsrtFontResMenu(MenuHandle theMenu, short afterItem, short scriptFilter)
{
	InsertIntlResMenu(theMenu, kFirstType, afterItem, scriptFilter);		// Do the FONTs and FONDs
}

/*--------------------------------------------------------------------------------------------------
	Purpose:	Set up for calling InsResGuts.
	Note:		Special cased for FONT and FOND resources: will do both if either is mentioned.
	Inputs:		theMenu		- the menu in which the name of the resources will be inserted
				theType		- the resource type to add
				afterItem	- the item number after which rsrc names will be added
	Outputs:	The menu is updated as appropriate.
*/
pascal void InsrtResMenu(MenuHandle theMenu, ResType theType, short afterItem)
{
	InsertIntlResMenu(theMenu, theType, afterItem, smAllScripts);
}
