/*
	File:		MenuMgrPatches.c

	Contains:	Routines which patch the Menu Manager traps.

	Written by:	Erich Ringewald

	Copyright:	© 1986-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		<10>	 9/25/92	DRF		Eliminate superflous prototype for MyGestalt()
		 <9>	 3/20/92	DCL		#1025005,<KSM>:
									Included Script.h because: For New IM: Moved some declarations
									and routines from Packages.[ph] to Script.[ph] #1025005
		 <8>	  2/2/91	DFH		csd,WhiteBoard:AdjustApplicationMenu now allows for the case
									when the pTitleProc does not yet have a layer. This can happen
									when we are called from ExitToShell with pTitleProc being a
									nascent application that has not yet called InitWindows (wherein
									we add the app name to the application menu).
		 <6>	 1/28/91	DFH		JSM,#81425: Removed inhibit_DAs mechanism. Removed
									DisableSwitching and EnableSwitching calls from our MenuSelect
									and PopupMenuSelect patches since it is already taken care of
									by the WaitMouseUp patch. This meant the PopUpMenuSelect
									patch was no longer needed!
		 <5>	 1/21/91	DFH		(KSM) Process Menu is now called Application Menu.
		 <3>	12/17/90	DFH		Rename CheckForCutCopy to CheckForCutCopyMenuKey.
		 <2>	12/10/90	DFH		Adding support for custom icons (i.e. the ones the user can
									specify).
		 <0>	 x/xx/86	ELR		New Today.

*/  
 
#include <types.h>
#include <memory.h>
#include <quickdraw.h>
#include <fonts.h>
#include <events.h>
#include <desk.h>
#include <resources.h>
#include <osutils.h>
#include <retrace.h>
#include <menus.h>
#include <windows.h>
#include <files.h>
#include <toolutils.h>
#include <segload.h>
#include <errors.h>
#include <EPPC.h>
#include <AppleEventsInternal.h>
#include <MFPrivate.h>
#include <GestaltEqu.h>
#include <Icons.h>
#include <IconUtilsPriv.h>
#include <Windows.h>
#include <ScriptPriv.h>
#include <IntlUtilsPriv.h>
#include <MenuMgrPriv.h>

#include "Patches.h"
#include "Glue.h"
#include "HList.h"
#include "Lomem.h"
#include "Data.h"
#include "SysMisc.h"
#include "Puppet.h"
#include "OSDispatchPrivate.h"
#include "AppleEventExtensions.h"
#include "Script.h"
#include "Language.h"

/* -----------------------------------------------------------------------------
 *
 *								Data and prototypes.
 *
 * -----------------------------------------------------------------------------*/

/* Some function prototypes that should be in (yet another) header file */
StringPtr		StripLeadingNulls(pString);
Handle			NewMungedString(StringPtr, StringPtr);
short			GetIconIDFromBundle(OSType, unsigned long);

/* Function prototypes internal to this file */
Boolean			IsEnabledItem(long);
Ptr				MenuItemAddress(MenuHandle, short);
pascal short	MyIUTextOrder(StringPtr, StringPtr, ScriptCode, ScriptCode);
Boolean			IsMenuItemForNamePrefix(MenuHandle, short, StringPtr);
long			MenuSelectionGivenName(MenuHandle, StringPtr);
void			ApplyToAllAppleMenus(HListElemFnc *);
void			CheckItemNameAndCharForCutCopy(long, short *);
void			InitAppleMenuList(void);
pascal Handle	GetProcessIcon(ResType, long);
void			ChangeIconicTitle(MenuHandle, Handle);
short			MyPrefixRelString(StringPtr, StringPtr);
void			InsertCustomItem(MenuHandle, short, AppleItemHdl);
void			InsertCustomItemSorted(MenuHandle);
OSErr			DeleteAppleItem(PEntryPtr, long);
void			DeleteCustomItem(AppleItemHdl);
void			DeleteCustomItemFromMenu(MenuHandle);
PEntryPtr		PEntryFromApplicationMenuItem(short);
unsigned short	ApplicationMenuItemFromPEntry(PEntryPtr);
unsigned short	ItemNumFromItemPtr(MenuHandle, Ptr);
AppleItemHdl	StringToAppleItemHdl(StringPtr);

/* The resource type we use to identify Apple menus in _AddResMenu patch */
#define DA_RESOURCE_TYPE		'DRVR'

/* Menu item text strings can have up to this many characters */
#define MENU_ITEM_MAX_LENGTH	(255)
#define IS_ADDED_ITEM_NAME(pText)	( ((StringByte(pText, 0) == NULL) && (StringByte(pText, 1) == NULL)) \
										|| (Length(pText) == MENU_ITEM_MAX_LENGTH) )
#define IS_DA_NAME(pText)			((StringByte(pText, 0) == NULL) && (StringByte(pText, 1) != NULL))
#define IS_ADDED_OR_DA_NAME(pText)	((StringByte(pText, 0) == NULL) || (Length(pText) == MENU_ITEM_MAX_LENGTH))

/* Cheater way to see whether the title of a menu is the AppleMark */
#define APPLE_MENU_TITLE			0x0114
#define HAS_APPLE_TITLE(theMenu)	(*((short *) &((*(theMenu))->menuData)) == APPLE_MENU_TITLE)

/* Apple Item to insert/delete in DA menu */
AppleItemHdl		hAppleItemShare;

#define pAppleMenuProcessList		(pNullProcess->nextAppleMenuProc)
#define pApplicationMenuProcessList	(pNullProcess->nextApplicationMenuProc)

/* -----------------------------------------------------------------------------
 *
 *								Internal utilities.
 *
 * -----------------------------------------------------------------------------*/
#pragma segment Main
	
/* IsEnabledItem.  Return whether the selected menu item is enabled.
 * NOTE: Allows for menu manager weakness that limits the range of items that can
 * really be enabled or disabled (i.e. items 1 through MAX_ENABLEFLAG_INDEX).  As
 * per Inside Mac, higher numbered items are always enabled.
 */
Boolean
IsEnabledItem(long menuSelection)
	{
	MenuHandle		menuHdl;
	short			itemNum;
	
	/* Assume disabled if the selection isn't for a current menu (!) */
	if ((menuHdl = GetMHandle(MENU_ID_OF_SELECTION(menuSelection))) == nil)
		return (false);
	
	/* Extract item number, make range check */
	itemNum = ITEM_ID_OF_SELECTION(menuSelection);
	assert(itemNum > 0 && itemNum <= CountMItems(menuHdl));
	if (itemNum > MAX_ENABLEFLAG_INDEX)
		return (true);
	
	/* Return whether flag is on */
	return (((*menuHdl)->enableFlags & (1<<itemNum)) != 0);
	}

/* MyIUTextOrder.  For my eyes.  Extracts string lengths, and defaults the language.
 * This also saves a little code compared to the inline equivalent when the string
 * address is complex, since that address is only calculated once here, but twice
 * there.
 * NOTE: IUTextOrder can call RecoverHandle, which requires proper setting of THEZONE.
 */
pascal short
MyIUTextOrder(StringPtr aStr, StringPtr bStr, ScriptCode aScript, ScriptCode bScript)
	{
	assert(THEZONE != ProcessMgrZone);
	
	return (IUTextOrderSys(&StringByte(aStr, 0), &StringByte(bStr, 0),
				(short) Length(aStr), (short) Length(bStr),
				aScript, bScript,
				iuScriptDefLang, iuScriptDefLang));
	}

/* IsMenuItemForNamePrefix.  Return whether the given string prefixes the item name. */
Boolean
IsMenuItemForNamePrefix(MenuHandle menuHdl, short iItem, StringPtr pStrPrefix)
	{
	Str255			strItemName;
	
	assert(menuHdl != nil);
	
	GetItem(menuHdl, iItem, &strItemName);
	return ((Boolean)(MyPrefixRelString(pStrPrefix, &strItemName) == 0));
	}

/* MenuItemAddress.  Gets the address of the specified menu item.  Useful when
 * beginning to crawl menu structure from the middle.
 */
Ptr
MenuItemAddress(MenuHandle hMenu, short item)
	{
	Ptr		pMenuItem;
	
	/* locate menu data */
	pMenuItem = (Ptr) &((*hMenu)->menuData);
	
	/* skip menu title to get to first item */
	pMenuItem += Length(pMenuItem) + 1;
	
	/* iterate through menu items */
	while ( (IS_NOT_LAST_MENUITEM(pMenuItem)) && (--item > 0) )
		{
		INC_MENU_ITEMPTR(pMenuItem);
		}
	
	/* return nil if search failed */
	return(IS_NOT_LAST_MENUITEM(pMenuItem) ? pMenuItem : nil);
	}

/* MenuSelectionGivenName.  Given a menu handle and item name, return the menu
 * selection, or 0 if the item cannot be found.
 */
long
MenuSelectionGivenName(MenuHandle menuHdl, StringPtr itemNamePtr)
	{
	short		item;
	Ptr			posInMenu;
		
	/* Can do nothing of there is no menu to look at! */
	if (menuHdl == nil)
		return (0);
		
	/* Skip menu title to get to first item */
	posInMenu = (Ptr) &((*menuHdl)->menuData);
	posInMenu += Length(posInMenu) + 1;
	
	/* Iterate through menu items to match the specified name */
	item = 1;
	while (IS_NOT_LAST_MENUITEM(posInMenu))
		{
		if (MyRelString(posInMenu, itemNamePtr) == sortsEqual)
			return (BUILD_MENUSELECTION((*menuHdl)->menuID, item));
		
		INC_MENU_ITEMPTR(posInMenu);
		item++;
		}

	/* Falling out of loop means no-find */
	return (0);
	}

/* ApplyToAllAppleMenus.  Handy routine that calls the given HList function against all
 * Apple menu handles on the system.  For each process, it uses ApplyToHListElems on the
 * p_applemenuhlisthdl.  The only parameter to the function call is the menu handle,
 * so any other data must be passed in globals.
 */
void
ApplyToAllAppleMenus(HListElemFnc *pHListElemFnc)
	{
	HListHdl				hMenuList;
	register PEntryPtr		pCurProc;

	/* Traverse the process descriptor list. */
	pCurProc = pAppleMenuProcessList;
	while (pCurProc != nil)
		{
		hMenuList = pCurProc->p_applemenuhlisthdl;
		if (HListExists(hMenuList))
			ApplyToHListElems(hMenuList, pHListElemFnc);
		pCurProc = pCurProc->nextAppleMenuProc;
		}
	}

/* -----------------------------------------------------------------------------
 *
 *								Patches to existing traps.
 *
 * -----------------------------------------------------------------------------*/
 
#pragma segment Main

/* c_addresmenu.  Check if adding DAs to Apple menu.  If so, use the opportunity to
 * add all of our stuff, too.  Our definition of Apple menu is it's the one with
 * the DAs (i.e. the menu resource type is DA_RESOURCE_TYPE).
 * NOTE:  An application can have more than one Apple menu.  Ex:  Excel.
 */
pascal void
c_addresmenu(MenuHandle menuHdl, ResType resType)
	{
	short				cMItems, checkItem, currItem;
	HListHdl			menuHdlHListHdl;
	unsigned long		str2Divider = TWO_CHAR_STRING('(','-');
	unsigned short		emptyName = ONE_CHAR_STRING(' ');
	AppleItemHdl		hAppleItem;
	unsigned long		olda5;
	Str255				tempString;
	Addresmenu_ptr		oldtrap;
	
	olda5 = ProcessMgrA5Setup();
	oldtrap = (Addresmenu_ptr) (patchtraps[ADDRESMENU].oldtrap);

	/* Add this menu to the list of AppleMenus */
	if ((resType == DA_RESOURCE_TYPE) && (menuHdl != nil))
		{
		/* Create the recording list, if necessary */
		if (!HListExists(pCurrentProcess->p_applemenuhlisthdl))
			{
			pCurrentProcess->p_applemenuhlisthdl = CreateHList(0);

			/* Link PEntry into list of similar apps */
			pCurrentProcess->nextAppleMenuProc = pAppleMenuProcessList;
			pAppleMenuProcessList = pCurrentProcess;
			}
			
		menuHdlHListHdl = pCurrentProcess->p_applemenuhlisthdl;
		
		/* Stick menu handle in list of menu handles for this process */
		(void)AddToHList(menuHdlHListHdl, (HListElem)menuHdl);

		/* Ensure the standard mdefproc */
		(*menuHdl)->menuProc = standardMenuDefProcRsrc;
			
		/* Add a dashed line at the end (unless the app already has one there). */
		Length(&tempString) = 0;
		if ((cMItems = CountMItems(menuHdl)) > 0)
			GetItem(menuHdl, cMItems, &tempString);
		
		if (IS_DASH_STRING(&tempString) == false)
			{
			AppendMenu(menuHdl, (StringPtr) &str2Divider);
			cMItems++;
			}
		}
		
	/* Do ROM routine to get all the items */
	CALL_FNC_PTR(Addresmenu_ptr, oldtrap, (menuHdl,resType));

	/* Append remainder of Process Mgr items if this is the Apple menu */
	if ((resType == DA_RESOURCE_TYPE) && (menuHdl != nil))
		{	
		/* Remember position before add, and get new size */
		currItem = checkItem = cMItems;
		cMItems = CountMItems(menuHdl);
		
		/* If there are no DAs and no added items, remove the dashed item we added */
		hAppleItem = FirstAppleItem;
		if ((hAppleItem == nil) && (currItem == cMItems))
			DelMenuItem(menuHdl, currItem);

		/* Insert items or item runs until no more or off end of menu.
		 * We use checkItem (current insertion point candidate) and
		 * cMItems (size of menu) to traverse the menu.  checkItem is
		 * incremented each time a new item is fetched or inserted,
		 * cMItems is incremented when the list size is increased.
		 * Ignores the leading NULLs.
		 */
		while ( (hAppleItem != nil) && (++checkItem <= cMItems) )
			{
			StringPtr	pDAName, pAMIName;
			
			GetItem(menuHdl, checkItem, &tempString);
			pDAName = StripLeadingNulls(&tempString);
			
			while (hAppleItem != nil)
				{
				short			compResult;
				
				pAMIName = StripLeadingNulls(&(*hAppleItem)->aiNameStorage);
				compResult = MyIUTextOrder(pAMIName, pDAName, (*hAppleItem)->aiScript, iuSystemScript);
				
				/* Restore NULL (it was replaced by a length byte) */
				Length(pAMIName) = NULL;
				if (compResult >= sortsEqual)
					break;
					
				InsertCustomItem(menuHdl, checkItem-1, hAppleItem);
				cMItems++;
				checkItem++;
				hAppleItem = (*hAppleItem)->aiNext;
				}
			}
			
		/* home stretch ... append remainder of list, if any */
		while (hAppleItem != nil)
			{
			InsertCustomItem(menuHdl, cMItems++, hAppleItem);
			hAppleItem = (*hAppleItem)->aiNext;
			}

		}
		
	A5Restore(olda5);
	}

/* CheckItemNameAndCharForCutCopy.  Update our global scrap coercion counter of
 * cuts/copies vs pastes, based on the specified menu selection.  Examines both
 * the item text and the command key equivalent to see which, if any, of these
 * actions is occurring.
 */
void
CheckItemNameAndCharForCutCopy(long menuSelection, short *pCutCopyCount)
	{
	MenuHandle	menuHdl;
	Str255		strItemName;
	short		ch;
	
	if (VALID_MENUSELECTION(menuSelection))
		{
		if ((menuHdl = GetMHandle(MENU_ID_OF_SELECTION(menuSelection))) != nil)
			{
			/* Force 'copy' designation if the item text prefixes any predefined ones.
			 * Otherwise, act on whether the command key indicates cut or copy. 
			 */
			GetItem(menuHdl, ITEM_ID_OF_SELECTION(menuSelection), &strItemName);
			if (StrInStrList(&strItemName, &pStrListCopyNamePrefix->pStrings[0], pStrListCopyNamePrefix->cStrings, MyPrefixRelString) != nil)
				ch = 'C';
			else
				GetItemCmd(menuHdl, ITEM_ID_OF_SELECTION(menuSelection), &ch);
				
			CheckForCutCopyMenuKey(ch, pCutCopyCount);		
			}
		}
	}

/* c_menuselect.  Take advantage of the application's calling MenuSelect to further
 * our cause in a) puppet strings, b) scrap coercion, or c) layer hiding.    Uses
 * caller's A5 and graphics world when calling though.
 * NOTE: FullWrite never calls this trap.  So anything you depend on here may need
 * to be duplicated elsewhere if you want FullWrite to use it.
 */
pascal long
c_menuselect(Point startPt)
	{
	long			retval;
	Menuselect_ptr	oldtrap;
	unsigned long	olda5;

	olda5 = ProcessMgrA5Setup();
	
	/* Return selection forced by puppet string, if any.  Cancel the puppet show if that
	 * item is no longer available.  In any case, reset the forced selection.
	 */
	retval = 0;
	if (forcedPupStrMenuSelection != 0)
		{
		if (IsEnabledItem(forcedPupStrMenuSelection))
			{
			CheckItemNameAndCharForCutCopy(forcedPupStrMenuSelection, &cutCopyCount);
			retval = forcedPupStrMenuSelection;
			}
		else
			CancelSwitchWithError();

		forcedPupStrMenuSelection = 0;
		}
	
	/* Call the old trap.  Disallow process switching during this critical time.
	 * Use caller's A5 and graphics world.
	 */
	else
		{		
		oldtrap = (Menuselect_ptr) (patchtraps[MENUSELECT].oldtrap);	
		A5Restore(olda5);
		retval = CALL_FNC_PTR(Menuselect_ptr, oldtrap, (startPt));
		olda5 = ProcessMgrA5Setup();
		
		/* See if we should adjust the global scrap cut/copy count. */
		if (VALID_MENUSELECTION(retval))
			CheckItemNameAndCharForCutCopy(retval, &cutCopyCount);
		}
		
	A5Restore(olda5);
	return (retval);
	}

/* c_menukey.  Patch to MenuKey to keep our scrap coercion cut/copy counter up to date. */	
pascal long
c_menukey(short ch)
	{
	u_long		retval, olda5;
	
	/* call the old trap */
	olda5 = ProcessMgrA5Setup();
	retval = CALL_FNC_PTR(Menukey_ptr, patchtraps[MENUKEY].oldtrap, (ch));
	
	/* Synch up only if we're not in middle of coercion, and the key actually
	 * was a key equivalent for a menu item.
	 */
	if ((coercionState == CS_DONE) && (VALID_MENUSELECTION(retval)))
		CheckForCutCopyMenuKey(ch, &cutCopyCount);
		
	A5Restore(olda5);
	return (retval);
	}

/* c_setcursor.  Don't allow _SetCursor thru unless frontmost.
 * NOTE: This can have unintended effect if the app was JUST switched to the back
 * and really needs to reset the cursor.
 */
pascal void
c_setcursor(Ptr cursorDataPtr)
	{
	unsigned long		olda5;
	Setcursor_ptr		oldtrap;
	Boolean				frontmost;
	
	olda5 = ProcessMgrA5SimpleSetup();
	oldtrap = (Setcursor_ptr) (patchtraps[SETCURSOR].oldtrap);
	frontmost = (pCurrentProcess == pFrontProcess);
	A5SimpleRestore(olda5);
	
	if (frontmost)
		CALL_FNC_PTR(Setcursor_ptr, oldtrap, (cursorDataPtr));
	}

/* -----------------------------------------------------------------------------
 *
 *					Auxiliary item information for Apple menu.
 *
 * -----------------------------------------------------------------------------*/

/* ItemNumFromItemPtr.  Return the item number of the entry pointed to by pItemEntry. */
unsigned short
ItemNumFromItemPtr(MenuHandle menuHdl, Ptr pItemEntry)
	{
	short		item;
	Ptr			posInMenu;
	
	posInMenu = (Ptr) StripAddress(&((*menuHdl)->menuData));
	posInMenu += Length(posInMenu) + 1;
	item = 0;
	while (IS_NOT_LAST_MENUITEM(posInMenu))
		{
		item += 1;
		if (posInMenu == pItemEntry)
			break;
		INC_MENU_ITEMPTR(posInMenu);
		}
		
	return(item);
	}

/* StringToAppleItemHdl.  Given a string pointer, return the associated AppleItemHdl,
 * or nil.  Relies on the fact that all AppleItem names (aiNameStorage) have preceding
 * NULLs.  If the given string has no such null, we return nil without having to scan
 * the list.  Notice that the check will let an empty string through to the loop, but
 * the loop will never find a match in this (uncommon) case.  An even less likely case
 * that we DO have to check for is if the item was too long to have NULLs prepended to it.
 */
AppleItemHdl
StringToAppleItemHdl(StringPtr itemText)
	{
	AppleItemHdl	elemHdl;
	
	/* Check only if it has basic appearance of item we added. */
	elemHdl = nil;
	if (IS_ADDED_ITEM_NAME(itemText))
		{	
		elemHdl = FirstAppleItem;
		while (elemHdl != nil)
			{
			if (MyRelString(&(*elemHdl)->aiNameStorage, itemText) == sortsEqual)
				break;
			elemHdl = (*elemHdl)->aiNext;
			}
		}
	
	return (elemHdl);
	}


/* c_getauxmenuitem.  Trap that MDEF/MBDF can call to get more information than can
 * be stored concurrently in a menu item.  Return noTypeErr if this menu seems to not
 * have any items that need auxiliary information.  This allows Menu Mgr/MDEF/MBDF to
 * avoid calling us any more for this menu.
 */
#define SIZEOF_SMALLEST_AUXMENUITEMREC		(3*sizeof(short) + 2*sizeof(long))
pascal OSErr
c_getauxmenuitem(MenuHandle theMenu, unsigned char *pItemEntry, AuxMenuItemRecPtr theStorage, unsigned long recLength)
	{
	StringPtr		pItemName;
	AppleItemPtr	elemPtr;
	AppleItemHdl	elemHdl;
	PEntryPtr		pProc;
	u_long			olda5;
	OSErr			retval;
	
	/* Are we even in a menu we do anything about? */
	olda5 = ProcessMgrA5SimpleSetup();
	retval = noErr;	
	theMenu = (MenuHandle) StripAddress(theMenu);
	
	/* Is length at least as big as the smallest record we ever return? */
	if ((recLength < SIZEOF_SMALLEST_AUXMENUITEMREC) || (theMenu == nil))
		return (paramErr);
	
	/* Is this the Application Menu? */
	if (theMenu == ApplicationMenuHdl)
		{
		short		itemNum;
		
		if ( ((itemNum = ItemNumFromItemPtr(theMenu, (Ptr) StripAddress(pItemEntry))) > APPLICATION_MENU_COUNT) 
			&& ((pProc = PEntryFromApplicationMenuItem(itemNum)) != nil) )
			{
			theStorage->auxMenuIconHandle = pProc->iconCacheHdl;
			theStorage->auxMenuIconSize = familyIconCmd;
			theStorage->auxMenuIconGray = (((LayerPeek) (pProc->p_layer))->visible == false);
			theStorage->auxMenuScript = pProc->p_nameScript;
			theStorage->auxMenuHelpString = nil;
			}
		}

	/* Is this the Keyboard menu? */
	else if ((*theMenu)->menuID == kScriptMenuID)
		{
		retval = SetKbdMenuInfo(theMenu, ItemNumFromItemPtr(theMenu, (Ptr) StripAddress(pItemEntry)), theStorage);
		}

	/* Is this an Apple menu?  Avoid longer check if the menu title is an AppleMark.
	 * NOTE: This also fixes an app that calls _AddResMenu('DRVR'), then copies that
	 * menu to another one.  This happens with SuperCard.
	 */
	else if ( (HAS_APPLE_TITLE(theMenu)) ||
		(InHList(pCurrentProcess->p_applemenuhlisthdl, (HListElem) theMenu) == noErr) )
		{	
		/* Item name is first thing in an entry */
		pItemName = (StringPtr) pItemEntry;
	
		/* Is item one that as added by AddAppleMenuItem? */
		if ((elemHdl = StringToAppleItemHdl(pItemName)) != nil)
			{
			elemPtr = *elemHdl;
			theStorage->auxMenuIconSize = ((theStorage->auxMenuIconHandle = elemPtr->aiIcon) != nil)
				? familyIconCmd : 0;
			theStorage->auxMenuIconGray = false;
			theStorage->auxMenuScript = elemPtr->aiScript;
			theStorage->auxMenuHelpString = elemPtr->aiHelpString;
			}
		
		/* Is it a DA from the System file? */
		else if (IS_DA_NAME(pItemName))
			{
			theStorage->auxMenuIconHandle = hGenericDAIconSuite;
			theStorage->auxMenuIconSize = familyIconCmd;
			theStorage->auxMenuIconGray = false;
			theStorage->auxMenuScript = iuSystemScript;
			theStorage->auxMenuHelpString = nil;
			}
		}
	else
		retval = noTypeErr;
		
	A5SimpleRestore(olda5);
	return (retval);
	}

/* -----------------------------------------------------------------------------
 *
 *								Apple menu control.
 *
 * -----------------------------------------------------------------------------*/

#pragma segment INIT

/* InitAppleMenuList.  Initialize Apple menu list, a tough job for anyone.
 * NOTE: assumes a5 == PROCESSMGRGLOBALS, THEZONE == ProcessMgrZone
 */
void
InitAppleMenuList(void)
	{	
	/* initialize list of Apple menu items */
	FirstAppleItem = nil;
	}	

#pragma segment kernel_segment

/* InsertCustomItem.  Insert custom item into menu. */
void
InsertCustomItem(MenuHandle menuHdl, short insertAfter, AppleItemHdl hDesc)
	{
	unsigned short		emptyName = ONE_CHAR_STRING(' ');
	Style				theStyle;
	char				theMark;

	InsMenuItem(menuHdl, (StringPtr) &emptyName, insertAfter++);
	SetItem(menuHdl, insertAfter, &(*hDesc)->aiNameStorage);
	
	if ((theStyle = (*hDesc)->aiStyle) != (Style) 0)
		SetItemStyle(menuHdl, insertAfter, theStyle);
	
	if ((theMark = (*hDesc)->aiMark) != noMark)
		SetItemMark(menuHdl, insertAfter, theMark);
	}

/* InsertCustomItemSorted.  Insert item in (alphabetical) DA portion of Apple menu.
 * Hard part is determining which item number to use.  Can move or purge memory.
 */
void
InsertCustomItemSorted(MenuHandle menuHdl)
	{
	short			item;
	unsigned long	str2Divider;
	Ptr				posInMenu, pPrevItem;
	StringPtr		pDAName, pAMIName;
	AppleItemHdl	aiHdl;
	ScriptCode		daScript;

	/* Can't do it if there's no menu */
	if (menuHdl == nil)
		return;

	/* iterate through menu items to find the first of our items, if any, after app items */
	posInMenu = (Ptr) &((*menuHdl)->menuData);
	posInMenu += Length(posInMenu) + 1;
	pPrevItem = nil;
	item = 1;
	while (IS_NOT_LAST_MENUITEM(posInMenu))
		{
		if (IS_ADDED_OR_DA_NAME(((StringPtr) posInMenu)))
			break;
		
		pPrevItem = posInMenu;
		INC_MENU_ITEMPTR(posInMenu);
		item += 1;
		}
	
	/* Insert dashed line if there isn't one */
	if ((pPrevItem == nil) || (IS_DASH_STRING(pPrevItem) == false))
		{
		str2Divider = TWO_CHAR_STRING('(','-');
		InsMenuItem(menuHdl, (StringPtr) &str2Divider, item);
		posInMenu = MenuItemAddress(menuHdl, item);
		INC_MENU_ITEMPTR(posInMenu);
		}
	
	/* Item now at preexisting dashed line, search for insertion point.  Ignore the
	 * leading NULL found at the beginning of an actual DA name.  This keeps things
	 * ordered properly regardless of whether the installer of the new item was savvy
	 * about DA names or has mixed and matched names himself.
	 */
	else
		{
		HLock(hAppleItemShare);
		pAMIName = StripLeadingNulls(&(*hAppleItemShare)->aiNameStorage);

		while (IS_NOT_LAST_MENUITEM(posInMenu))
			{		
			short		compResult;
			
			/* Get script code of this item */
			daScript = ((aiHdl = StringToAppleItemHdl(posInMenu)) != nil) ? (*aiHdl)->aiScript : iuSystemScript;
			
			/* Ignore leading NULLs */
			pDAName = StripLeadingNulls(posInMenu);
	
			/* Compare! */
			compResult = MyIUTextOrder(pAMIName, pDAName, (*hAppleItemShare)->aiScript, daScript);
		
			/* Restore NULL (it was replaced by a length byte) */
			if (pDAName != posInMenu)
				Length(pDAName) = NULL;
			
			if (compResult < sortsEqual)
				break;
				
			/* Stay up to date */
			++item;
			INC_MENU_ITEMPTR(posInMenu);
			}
		
		/* Restore NULL (it was replaced by a length byte) */
		Length(pAMIName) = NULL;
		HUnlock(hAppleItemShare);
		
		/* item now after insertion point or off the end, so back off by one */
		--item;
		}

	/* Insert the item (finally!) */
	InsertCustomItem(menuHdl, item, hAppleItemShare);
	}

/* c_AddAppleMenuItem.  Trap that adds a speciality item to the Apple menu. */
pascal OSErr
c_AddAppleMenuItem(StringPtr itemName, ScriptCode itemScript, short itemStyle, Handle itemIcon, short itemMark, StringPtr itemHelpString, unsigned long itemRefCon)
	{
	long					olda5;
	THz						currZone = nil;
	AppleItemPtr			stackItemPtr;
	register AppleItemPtr	itemPtr;
	AppleItemHdl			itemHdl, elemHdl, compHdl;
	StringHandle			hHelpString;
	unsigned short			numNulls;
	short					compResult;
	short					retval;
	Str255					tempString;

	/* eliminate bozos */
	if (itemName == nil || Length(itemName) == 0 || itemRefCon == DELETE_ALL_ITEMS)
		return (paramErr);
	
	/* requisite set up */
	olda5 = ProcessMgrA5Setup();
	assert(THEZONE != ProcessMgrZone);
	
	/* remove existing duplicate item, ignore error (no find) */
	(void) DeleteAppleItem(pCurrentProcess, itemRefCon);

	/* Make sure item starts with at least two NULLs */
	*((long *) &tempString) = TWO_CHAR_STRING(NULL, NULL);
	Length(&tempString) = Length(itemName);
	numNulls = 2;
	while ((unsigned short) (Length(&tempString) + numNulls) > 255)
		numNulls -= 1;
	Length(&tempString) += numNulls;
	
	BlockMove(&StringByte(itemName, 0), &StringByte(&tempString, numNulls), Length(itemName));

	stackItemPtr = &FirstAppleItem;
	elemHdl = &stackItemPtr;
	while ((compHdl = (*elemHdl)->aiNext) != nil)
		{
		if ( (compResult = MyIUTextOrder(&tempString, &(*compHdl)->aiNameStorage, itemScript, (*compHdl)->aiScript)) < sortsEqual )
			break;

		if (compResult == sortsEqual)
			{
			A5Restore(olda5);
			return (protocolErr);
			}
			
		elemHdl = compHdl;
		}

	/* Allocate descriptors from our own heap
	 * NOTE: Assumes THEZONE != ProcessMgrZone, in that it restores THEZONE unconditionally,
	 * instead of using SafeSetZone and RestoreZone.
	 */
	currZone = THEZONE;
	THEZONE = ProcessMgrZone;
		
	/* allocate a new descriptor */
	if ((itemHdl = NewHandle(sizeof(AppleItem)+Length(&tempString)+1)) == nil)
		goto memError2;
		
	/* Copy help string to our heap */
	hHelpString = nil;
	if ((itemHelpString != nil) && ((hHelpString = NewString(itemHelpString)) == nil))
		goto memError1;
	
	/* fill in the descriptor */
	itemPtr = *itemHdl;
	BlockMove(&tempString, &itemPtr->aiNameStorage, Length(&tempString)+1);
	itemPtr->aiScript = itemScript;
	itemPtr->aiStyle = itemStyle;
	itemPtr->aiIcon = itemIcon;
	itemPtr->aiMark = (char) itemMark;
	itemPtr->aiHelpString = hHelpString;
	itemPtr->aiRefCon = itemRefCon;
	itemPtr->aiProcess = pCurrentProcess;
	
	/* Link the new element into the existing list at previously determined location.
	 * elemHdl refers to the element before the place for the new one, or to the
	 * list head itself.  We treat these cases equivalently because the link field
	 * is in the 0th position.
	 */
	itemPtr->aiNext = (*elemHdl)->aiNext;
	(*elemHdl)->aiNext = itemHdl;
	
	/* Now that all the red tape is out of the way, actually make the item appear
	 * in everyone's Apple menus (including current app).  We make sure to restore
	 * THEZONE beforehand, because MyIUTextOrder can load sysHeap resources.
	 */
	THEZONE = currZone;
	hAppleItemShare = itemHdl;
	ApplyToAllAppleMenus(InsertCustomItemSorted);
	
	/* clean up and leave */
	A5Restore(olda5);
	return (noErr);
	
	/* Combine error recovery so we don't have to duplicate code.
	 * NOTE: This would be less tacky if done with exception handling instead of GOTOs.
	 */
memError1:
	DisposHandle(itemHdl);
memError2:
	retval = memFullErr;
	THEZONE = currZone;
	A5Restore(olda5);
	return (retval);
	}

/* DeleteCustomItemFromMenu.  Remove custom item from given menu.  If it is the
 * last item in the DA/Dustom section, remove the dashed line at the beginning
 * of that (now empty) section.
 */
void
DeleteCustomItemFromMenu(MenuHandle menuHdl)
	{
	register short		itemNum;
	Boolean				dumpTheDash;
	Ptr					itemPtr;
	
	/* Find the number of the item in this menu */
	if ((itemNum = MenuSelectionGivenName(menuHdl, &((*hAppleItemShare)->aiNameStorage))) == 0)
		return;
	
	/* Dump the item */
	DelMenuItem(menuHdl, itemNum);
	
	/* See if we should dump the dash */
	itemPtr = MenuItemAddress(menuHdl, --itemNum);
	if ( (itemPtr == nil) || (IS_DASH_STRING(itemPtr) == false) )
		return;

	/* Item before is a dash.  Is item after a dash or the end of menu? */
	INC_MENU_ITEMPTR(itemPtr)
	dumpTheDash = ( (IS_LAST_MENUITEM(itemPtr)) || (IS_DASH_STRING(itemPtr)) );
		
	if (dumpTheDash)
		DelMenuItem(menuHdl, itemNum);
	}

/* DeleteCustomItem.  Removes specified item from all Apple menus. */
void
DeleteCustomItem(AppleItemHdl targetHdl)
	{
	PEntryPtr		pProc;
	
	if (Colorized)
		(*pCurrentProcess->p_pcb)->menucinfo = MENUCINFO;
	
	/* loop through all processes to remove menu item from all Apple menus */
	hAppleItemShare = targetHdl;
	pProc = pAppleMenuProcessList;
	while (pProc != nil)
		{		
		/* Can't do menu operations unless correct lomem in place */
		if (Colorized)
			MENUCINFO = (*pProc->p_pcb)->menucinfo;
			
		/* Get all Apple menus */
		if (HListExists(pProc->p_applemenuhlisthdl))	
			ApplyToHListElems(pProc->p_applemenuhlisthdl, &DeleteCustomItemFromMenu);
		
		pProc = pProc->nextAppleMenuProc;
		}

	if (Colorized)
		MENUCINFO = (*pCurrentProcess->p_pcb)->menucinfo;
	}

/* DeleteAppleItem.  Do the work of speciality item removal.  If the refcon is
 * DELETE_ALL_ITEMS, we delete all AppleItems that were made by the given process.
 * Assumes a5 == PROCESSMGRGLOBALS
 */
OSErr
DeleteAppleItem(PEntryPtr pProc, long itemRefCon)
	{
	AppleItemPtr	targetPtr, elemPtr;
	AppleItemHdl	targetHdl, elemHdl;
	Handle			hdl;
	Boolean			deleteAll = (itemRefCon == DELETE_ALL_ITEMS);
	THz				currZone;
	OSErr			retVal = noErr;
	
	/* Switch to our zone for disposes */
	SafeSetZone(currZone, ProcessMgrZone);

	/* major loop -- do each menu item */
	elemPtr = &FirstAppleItem;
	elemHdl = &elemPtr;
	do
		{
		/* minor loop -- locate specified item in registry */
		while ((targetHdl = (*elemHdl)->aiNext) != nil)
			{
			targetPtr = *targetHdl;
			if ((targetPtr->aiProcess == pProc)
				&& (deleteAll || targetPtr->aiRefCon == itemRefCon))
				break;
			elemHdl = targetHdl;
			}
		
		/* no find means we're done, give error if specific delete requested */
		if (targetHdl == nil)
			{
			if (deleteAll == false)
				retVal = paramErr;
			break;
			}
		
		/* Delink the target.  elemHdl refers to the element before the target, or
		 * to the list head itself.  We delink these cases equivalently because the
		 * descriptor link field is in the 0th position.
		 */
		(*elemHdl)->aiNext = targetPtr->aiNext;
	
		/* Remove item from everyone's Apple menu */
		DeleteCustomItem(targetHdl);

		/* Dispose the storage we'd used */
		if ((hdl = (*targetHdl)->aiHelpString) != nil)
			DisposHandle(hdl);
		if ((hdl = (*targetHdl)->aiIcon) != nil)
			(void) DisposeIconSuite(hdl, true);
		DisposHandle(targetHdl);
		}
	while (deleteAll);

	/* Restore the zone */
	SafeRestoreZone(currZone);
	return(retVal);
	}

/* c_DeleteAppleMenuItem.  Trap that removes from the Apple menu one or all speciality
 * item(s) associated with the current process.  A refcon of DELETE_ALL_ITEMS means "all".
 */
pascal OSErr
c_DeleteAppleMenuItem(long itemRefCon)
	{
	long	olda5;
	short	retVal;

	olda5 = ProcessMgrA5Setup();
	retVal = DeleteAppleItem(pCurrentProcess, itemRefCon);
	A5Restore(olda5);
	return (retVal);
	}

/* Message format for aeAppleMenuItemSelect we send */
typedef struct AppleMenuSelectMsg {
	AETFHeader				messageHeader;
	KeyWord					metaDataMark;
	AETFParameter			amsHdr;
	unsigned long			amsRefCon;
} AppleMenuSelectMsg;

/* SendAppleMenuSelectMsg.  Send an AppleEvent for Apple menu item selection to the
 * application that installed the item.
 */
void
SendAppleMenuSelectMsg(AppleItemHdl chosenOne)
	{
	HighLevelEventRecord	theEvent;
	ProcessSerialNumber		receiverPSN;
	AppleMenuSelectMsg		msgStorage;
	OSErr					result;

	/* Set up params.  Refcon is copied to the stack for use as message body,
	 * so we don't have to lock the item descriptor.
	 */
	receiverPSN = ((*chosenOne)->aiProcess)->p_serialNumber;

	SetAppleEvent(aeAppleMenuItemSelect, &theEvent);
	GenericAEMessage(&msgStorage.messageHeader);
	msgStorage.metaDataMark = aeEndOfMetaDataKeyword;
	SetAEParmHeader(&msgStorage.amsHdr, aeAppleMenuItemKeyword, aeLongintParamType, sizeof(long));
	msgStorage.amsRefCon = ((*chosenOne)->aiRefCon);
	
	/* Post the event */
	(void) BeginSystemMode();
	result = PostHighLevelEvent((EventRecord *) &theEvent, (u_long) &receiverPSN, 0, &msgStorage, sizeof(AppleMenuSelectMsg), receiverIDisPSN);
	(void) EndSystemMode();
	assert(result == noErr);
	}

/* AppleMenuCleanup.  Deal with death of pProc as it relates to the Apple Menu. */
void
AppleMenuCleanup(PEntryPtr pProc)
	{
	register PEntryPtr	pPrev, pCurr;

	/* Remove this process from list of processes with Apple Menus */
	pPrev = pNullProcess;
	pCurr = pAppleMenuProcessList;
	while (pCurr != nil)
		{
		if (pCurr == pProc)
			{
			pPrev->nextAppleMenuProc = pProc->nextAppleMenuProc;
			break;
			}
		
		pPrev = pCurr;
		pCurr = pCurr->nextAppleMenuProc;
		}
	
	/* Remove any items added by this process */
	(void) DeleteAppleItem(pProc, DELETE_ALL_ITEMS);
	}

/* -----------------------------------------------------------------------------
 *
 *							Application Menu
 *
 * -----------------------------------------------------------------------------*/

pascal void HMInitHelpMenu(void)
		= {0x303C, 0xFFFD, 0xA830};
/*	move.w	xx, d0
 * 	_Pack14
 */

#pragma segment INIT
/* InitSystemMenus.  Install the system menus in the right-hand end of the menu bar. */
void
InitSystemMenus(void)
	{
	register MenuHandle		menuHdl;
	Str255					itemText;

	/* Reach an understanding with the Menu Mgr */
	InitMenus();
	
	/* Set up the Apple menus list */
	InitAppleMenuList();

	/* Let the Help Mgr install the Help menu */
	HMInitHelpMenu();

	/* Let international install the Keyboard menu */
	(void) InitKeybdMenu();
	
	/* Get process list menu */
	if ((ApplicationMenuHdl = menuHdl = GetResource('MENU', kApplicationMenuID)) == nil)
		{
		assert(RESERR == memFullErr);
		SysError(dsMemFullErr);
		}

	/* Ensure the standard mdefproc */
	(*menuHdl)->menuProc = standardMenuDefProcRsrc;
	
	/* Get menu item string for hiding current application */
	GetItem(menuHdl, HIDE_APP_ITEM, &itemText);
	hHideCurrent = NewString(itemText);
	
	/* Place it in the bar as the far right-hand (system) menu */
	InsertMenu(menuHdl, 0);
	
	/* Set up the suites for the generic icons (we only ever need small icons) */
	SetResLoad(false);
	(void) GetIconSuite(&hGenericAppIconSuite, genericApplicationIconResource, svAllSmallData);
	(void) GetIconSuite(&hGenericDAIconSuite, genericDeskAccessoryIconResource, svAllSmallData);
	SetResLoad(true);
	}
	
#pragma segment Main

/* FlushIcon.  Dispose the icon, and nil its holder in the cache. */
pascal OSErr
FlushIcon(ResType theType, Handle *theIcon, void *myDataPtr)
	{
#pragma unused (theType, myDataPtr)
	register Handle		localHandle;
	
	if ((localHandle = *theIcon) != nil)
		{
		*theIcon = nil;
		DisposHandle(localHandle);
		}
		
	return(noErr);
	}

/* GetProcessIcon. Switch to process' application file in its resource chain to fetch
 * an icon.  This routine is called by the icon cache utilities when an icon of a new
 * depth is needed.  We only deal with one depth at a time (since our icons are displayed
 * only on the main screen), so we can flush any earlier non-1 bit data.
 */
pascal Handle
GetProcessIcon(ResType theType, long refCon)
	{
	PEntryPtr		pProc;
	PCB				*pPCB;
	THz				saveZone;
	short			saveFile;
	Handle			saveTopMap;
	Boolean			lomemNotHis;
	Handle			retHdl;
	unsigned long	olda5;
	
	/* make mini-swap if the process does not have it's info in lomem */
	olda5 = ProcessMgrA5SimpleSetup();
	pProc = (PEntryPtr) refCon;
	pPCB = *(pProc->p_pcb);
	if (lomemNotHis = (pProc != pCurrentProcess))
		{
		saveTopMap = TOPMAPHANDLE;
		TOPMAPHANDLE = pPCB->topmaphandle;
		}

	saveFile = CURMAP;
 	CURMAP = pPCB->p_fileRefNum;
	SafeSetZone(saveZone, pProc->p_zone);
	
	if ((retHdl = GetResource(theType, pProc->iconResourceID)) != nil)
		if (HandToHand(&retHdl) != noErr)
			retHdl = nil;

	/* Dump all non-B&W data */
	(void) ForEachIconDo(pProc->iconCacheHdl, (svAllAvailableData ^ svAll1BitData), &FlushIcon, (void *) nil);

	/* restore lomem if we changed it */
	CURMAP = saveFile;
	SafeRestoreZone(saveZone);
	if (lomemNotHis)
		TOPMAPHANDLE = saveTopMap;
	
	A5SimpleRestore(olda5);
	return(retHdl);
	}
 
/* ChangeIconicTitle.  Changes the title icon to the specified one.  The title must
 * already be exactly five characters in length.
 */
void
ChangeIconicTitle(MenuHandle menuHdl, Handle theCacheHandle)
	{
	register Ptr		pChar;
	
	pChar = &(StringByte(&((*menuHdl)->menuData), 0));
	*pChar = 1;
	*((long *) (++pChar)) = theCacheHandle;
	}

/* AdjustApplicationMenu.  Routine to ensure that the Application Menu items are set
 * correctly for the front application (or the specified one).  The PEntry specification
 * is needed during layer switching because we want to set up the menu and draw the
 * menu bar BEFORE bringing the layer forward.
 * NOTE: This is a pretty brute force kinda' routine.
 */
void
AdjustApplicationMenu(PEntryPtr pTitleProc)
	{	
	register MenuHandle		menuHdl;
	register Ptr			pMenuData;
	register PEntryPtr		pProc;
	Boolean					showAll, showHideSome;
	short					checkItemNum;
	short					toDo;

	/* Cache menu handle in a register */
	menuHdl = ApplicationMenuHdl;
	
	if (pTitleProc == nil)
		pTitleProc = LayerOwner(GetFrontAppLayer());

	/* Set the title to be the application's icon 
	 * NOTE: This will be nil if pTitleProc has not yet called InitWindows.
	 */
	ChangeIconicTitle(menuHdl, pTitleProc->iconCacheHdl);
	
	/* Get out now if this application has no corresponding item in the app menu
	 * (we don't need to mark it, since the app can not yet call DrawMenuBar).
	 */
	if (pTitleProc->p_layer == nil)
		return;
	
	/* Ensure hiding item has the front (or soon to be front) application's name */
	SetItem(menuHdl, HIDE_APP_ITEM, *(pTitleProc->p_hideCurrentStr));

	/* Figure out which show/hide items are appropriate */
	showAll = showHideSome = false;
	pProc = pApplicationMenuProcessList;
	while ( (pProc != nil) && ((showAll == false) || (showHideSome == false)) )
		{
		if (pProc != pTitleProc)
			{
			if (((LayerPeek) (pProc->p_layer))->visible)
				showHideSome = true;
			else
				showAll = true;
			}
		
		pProc = pProc->nextApplicationMenuProc;
		}

	/* Enable HIDE_APP_ITEM and HIDE_OTHERS_ITEM iff there are other visible layers */
	if (showHideSome)
		{
		EnableItem(menuHdl, HIDE_APP_ITEM);
		EnableItem(menuHdl, HIDE_OTHERS_ITEM);
		}
	else
		{
		DisableItem(menuHdl, HIDE_APP_ITEM);
		DisableItem(menuHdl, HIDE_OTHERS_ITEM);
		}

	/* Enable SHOW_ALL_ITEM iff there are other invisible layers */
	if (showAll)
		EnableItem(menuHdl, SHOW_ALL_ITEM);
	else
		DisableItem(menuHdl, SHOW_ALL_ITEM);

	/* Move check mark from old to new place */
	pMenuData = MenuItemAddress(menuHdl, APPLICATION_MENU_COUNT+1);
	assert(pMenuData != nil);
	checkItemNum = ApplicationMenuItemFromPEntry(pTitleProc) - APPLICATION_MENU_COUNT;
	toDo = 2;	
	while ((IS_NOT_LAST_MENUITEM(pMenuData)) && (toDo > 0))
		{
		SKIP_TO_MENU_ATTRIBUTES(pMenuData);
		if (((MenuItemAttributes *) pMenuData)->itemMark == checkMark)
			{
			((MenuItemAttributes *) pMenuData)->itemMark = noMark;
			toDo -= 1;
			}
		
		if (--checkItemNum == 0)
			{
			((MenuItemAttributes *) pMenuData)->itemMark = checkMark;
			toDo -= 1;
			}
			
		FROM_MENU_ATTRIBUTES_TO_NEXT_ITEM(pMenuData);
		}
	}
	
/* GetProcessIconCache.  Set the iconCacheHdl.  Set iconResourceID so GetProcessIcon can
 * get the correct resource when it's called by the cache plotter.
 */
void
GetProcessIconCache(PEntryPtr pProc)
	{
	register short	iconID;
	register short	saveFile;
	Handle			hCache;

	/* No need for icons of BG-only app, since only facefull apps are shown in the
	 * Application Menu title and application list menu items.
	 */
	if ((pProc->p_taskmode & modeOnlyBackground) != 0)
		return;
	
	/* Determine ID of icon family for this application or DA.  iconResourceID may have
	 * already been set from the Finder information about the file.
	 */
	if ((iconID = pProc->iconResourceID) == 0)
		{
		saveFile = CURMAP;
		if ((CURMAP = (*pProc->p_pcb)->p_fileRefNum) != SYSMAP)
			iconID = GetIconIDFromBundle(pProc->p_type, pProc->p_signature);
		CURMAP = saveFile;
		}

	/* Use default if there is no special ID, or the special cache can not be made */
	if ((iconID == 0) || (MakeIconCache(&hCache, &GetProcessIcon, (void *) pProc) != noErr) )
		{
		iconID = 0;
		hCache = ((pProc->p_taskmode & modeDeskAccessory) == 0)
			? hGenericAppIconSuite : hGenericDAIconSuite;
		}
	
	/* Use the cache handle we nabbed */
	pProc->iconResourceID = iconID;
	pProc->iconCacheHdl = hCache;
	}

/* MakeHidingString.  Allocate the p_hideCurrentStr for the specified process.
 * NOTE: Assumes that hHideCurrent is not in the current heap, since hHideCurrent
 * does not get locked across call to NewMungedString.
 */
OSErr
MakeHidingString(PEntryPtr pProc, StringPtr pName)
	{
	/* Don't bother if the application is a type that is not put in the menu */
	if ((pProc->p_taskmode & modeOnlyBackground) != 0)
		return(noErr);

	/* Make the string for application hiding Application Menu item */
	return ((pProc->p_hideCurrentStr = NewMungedString(pName, *hHideCurrent)) != nil) ? noErr : appMemFullErr;
	}
	
/* InsertAppInApplicationMenu.  Add current app name to right-hand menu.  This can not be
 * called without a reasonable menu world (i.e. current process had to InitMenus).
 * Should not be called during Launch, since launcher might be BG-only.
 * NOTE: You'll see that, in the case of duplicate names, the new app will go at
 * the end of the section of duplicates.  This is a *little* extra effort for us,
 * but is a) a minor nicety for the user (i.e. it makes sense to go at the end), and
 * b) the Notification Mgr relies on it to find the application name in the menu (it
 * assumes that duplicates are in ascending order of PSN).
 */
void
InsertAppInApplicationMenu(void)
	{
	register PEntryPtr		pCurr, pPrev;
	register MenuHandle		menuHdl;
	StringPtr				pCurApName, pThisAppName;
	short					insertAfter;
	unsigned short			emptyName = ONE_CHAR_STRING(' ');
	Str63					thisAppName;
		
	/* Find insertion point */
	pPrev = pNullProcess;
	pCurr = pApplicationMenuProcessList;
	insertAfter = APPLICATION_MENU_COUNT;
	pCurApName = StripLeadingNulls(CURAPNAME);
	while (pCurr != nil)
		{
		pThisAppName = &(*(pCurr->p_pcb))->curapname;
		BlockMove(pThisAppName, &thisAppName, Length(pThisAppName)+1);
		pThisAppName = StripLeadingNulls(&thisAppName);
		if (MyIUTextOrder(pCurApName, pThisAppName, pCurrentProcess->p_nameScript, pCurr->p_nameScript) < sortsEqual)
			break;
			
		insertAfter += 1;
		pPrev = pCurr;
		pCurr = pCurr->nextApplicationMenuProc;
		};
		
	/* Restore NULL (it was replaced by a length byte) */
	if (pCurApName != CURAPNAME)
		Length(pCurApName) = NULL;
	
	/* Two-step to avoid meta-character evaluation */
	menuHdl = ApplicationMenuHdl;
	InsMenuItem(menuHdl,(StringPtr) &emptyName, insertAfter);
	SetItem(menuHdl, ++insertAfter, CURAPNAME);
	
	/* Add to list now that position in menu is assured. */
	pCurrentProcess->nextApplicationMenuProc = pPrev->nextApplicationMenuProc;
	pPrev->nextApplicationMenuProc = pCurrentProcess;
	}

/* DeleteAppFromApplicationMenu.  Delete app name from right-hand menu.  This can not be
 * called without a reasonable menu world (i.e. current process had to InitMenus).
 */
void
DeleteAppFromApplicationMenu(PEntryPtr pProc)
	{
	register PEntryPtr		pPrev, pCurr;
	unsigned short			itemNum;
	
	if ((pProc->p_taskmode & modeOnlyBackground) != 0)
		return;
	
	/* Remove this process from list of processes with Apple Menus */
	pPrev = pNullProcess;
	pCurr = pApplicationMenuProcessList;
	itemNum = APPLICATION_MENU_COUNT + 1;
	while (pCurr != nil)
		{
		/* Remove the menu item */
		if (pCurr == pProc)
			{
			pPrev->nextApplicationMenuProc = pProc->nextApplicationMenuProc;
			DelMenuItem(ApplicationMenuHdl, itemNum); 
			break;
			}
		
		++itemNum;
		pPrev = pCurr;
		pCurr = pCurr->nextApplicationMenuProc;
		}
	}

/* PEntryFromApplicationMenuItem.  Locate process corresponding to the given item in the
 * Application Menu.
 */
PEntryPtr
PEntryFromApplicationMenuItem(short itemNum)
	{
	PEntryPtr			pProc;
	
	itemNum -= APPLICATION_MENU_COUNT;
	pProc = pApplicationMenuProcessList;
	while (--itemNum > 0)
		{
		assert(pProc != nil);
		pProc = pProc->nextApplicationMenuProc;
		}
		
	return(pProc);
	}

/* ApplicationMenuItemFromPEntry.  Return the index of the Application Menu item naming
 * the specified process.
 * NOTE: This will be an infinite loop if the PEntry is not in the nextApplicationMenuProc list.
 */
unsigned short
ApplicationMenuItemFromPEntry(PEntryPtr pProc)
	{
	unsigned short		itemNum;
	PEntryPtr			pCurr;
	
	itemNum = APPLICATION_MENU_COUNT + 1;
	pCurr = pApplicationMenuProcessList;
	while (pCurr != pProc)
		{
		pCurr = pCurr->nextApplicationMenuProc;
		++itemNum;
		}
		
	return(itemNum);
	}

/* -----------------------------------------------------------------------------
 *
 *								External utilities.
 *
 * -----------------------------------------------------------------------------*/
	
#pragma segment Main

/* GetApplicationMenuLeftCoord.  Return the point representing the lower lefthand
 * corner of the Proces menu title.
 * NOTE: The vertical offset is taken directly from MBARHEIGHT.  Be aware that
 * someone may have zeroed it.
 */
Point
GetApplicationMenuLeftCoord(void)
	{
	Point					appleMenuLeftCoord;
	register MenuPair		*pMenuPair;
	register MenuListPtr	pMenuList;
	
	appleMenuLeftCoord.h = 0;
	appleMenuLeftCoord.v = MBARHEIGHT;

	pMenuList = (MenuListPtr) StripAddress(*MENULIST);
	pMenuPair = (Ptr) pMenuList + pMenuList->lastMenu;
	while (pMenuPair >= &(pMenuList->menuPairs[0]))
		{
		if (StripAddress(pMenuPair->menuHdl) == ApplicationMenuHdl)
			{
			appleMenuLeftCoord.h = pMenuPair->leftEdge;
			break;
			}
		--pMenuPair;
		}

	return (appleMenuLeftCoord);
	}

/* RemoveFromAppleMenuList.  Check if this handle is a menu handle in the Apple menu
 * list of the current task.  If so, remove him from the list.  Called by patch to
 * ReleaseResource so we don't continue to operate on bad handle.
 * NOTE: Can't just make this a _Munger call because no way to guarantee that the match
 * is made on sizeof(Handle) boundaries (need a step param for _Munger).
 */
void
RemoveFromAppleMenuList(Handle hdl)
	{
	HListHdl		appleMenuListHdl;

	appleMenuListHdl = pCurrentProcess->p_applemenuhlisthdl;
	if (HListExists(appleMenuListHdl))
		RemoveFromHList(appleMenuListHdl, (HListElem) hdl);
	}
