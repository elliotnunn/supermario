{
Created: Sunday, January 6, 1991 at 10:48 PM
    Menus.p
    Pascal Interface to the Macintosh Libraries

        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
    UNIT Menus;
    INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingMenus}
{$SETC UsingMenus := 1}

{$I+}
{$SETC MenusIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingQuickdraw}
{$I $$Shell(PInterfaces)Quickdraw.p}
{$ENDC}
{$SETC UsingIncludes := MenusIncludes}

CONST
noMark = 0;                             {mark symbol for MarkItem}

{ menu defProc messages }
mDrawMsg = 0;
mChooseMsg = 1;
mSizeMsg = 2;
mDrawItemMsg = 4;
mCalcItemMsg = 5;
textMenuProc = 0;
hMenuCmd = 27;                          {itemCmd == 0x001B ==> hierarchical menu}
hierMenu = -1;                          {a hierarchical menu - for InsertMenu call}
mPopUpMsg = 3;                          {menu defProc messages - place yourself}
mctAllItems = -98;                      {search for all Items for the given ID}
mctLastIDIndic = -99;                   {last color table entry has this in ID field}

TYPE
MenuPtr = ^MenuInfo;
MenuHandle = ^MenuPtr;
MenuInfo = RECORD
    menuID: INTEGER;
    menuWidth: INTEGER;
    menuHeight: INTEGER;
    menuProc: Handle;
    enableFlags: LONGINT;
    menuData: Str255;
    END;

MCEntryPtr = ^MCEntry;
MCEntry = RECORD
    mctID: INTEGER;                     {menu ID.  ID = 0 is the menu bar}
    mctItem: INTEGER;                   {menu Item. Item = 0 is a title}
    mctRGB1: RGBColor;                  {usage depends on ID and Item}
    mctRGB2: RGBColor;                  {usage depends on ID and Item}
    mctRGB3: RGBColor;                  {usage depends on ID and Item}
    mctRGB4: RGBColor;                  {usage depends on ID and Item}
    mctReserved: INTEGER;               {reserved for internal use}
    END;


{}

MCTablePtr = ^MCTable;
MCTableHandle = ^MCTablePtr;

MCTable = ARRAY [0..0] OF MCEntry;      { the entries themselves }

MenuCRsrcPtr = ^MenuCRsrc;
MenuCRsrcHandle = ^MenuCRsrcPtr;
MenuCRsrc = RECORD
    numEntries: INTEGER;                {number of entries}
    mcEntryRecs: MCTable;               {ARRAY [1..numEntries] of MCEntry}
    END;


PROCEDURE InitMenus;
    INLINE $A930;
FUNCTION NewMenu(menuID: INTEGER;menuTitle: Str255): MenuHandle;
    INLINE $A931;
FUNCTION GetMenu(resourceID: INTEGER): MenuHandle;
    INLINE $A9BF;
PROCEDURE DisposeMenu(theMenu: MenuHandle);
    INLINE $A932;
PROCEDURE AppendMenu(menu: MenuHandle;data: Str255);
    INLINE $A933;
PROCEDURE AddResMenu(theMenu: MenuHandle;theType: ResType);
    INLINE $A94D;
PROCEDURE InsertResMenu(theMenu: MenuHandle;theType: ResType;afterItem: INTEGER);
    INLINE $A951;
PROCEDURE InsertMenu(theMenu: MenuHandle;beforeID: INTEGER);
    INLINE $A935;
PROCEDURE DrawMenuBar;
    INLINE $A937;
PROCEDURE InvalMenuBar;
    INLINE $A81D;
PROCEDURE DeleteMenu(menuID: INTEGER);
    INLINE $A936;
PROCEDURE ClearMenuBar;
    INLINE $A934;
FUNCTION GetNewMBar(menuBarID: INTEGER): Handle;
    INLINE $A9C0;
FUNCTION GetMenuBar: Handle;
    INLINE $A93B;
PROCEDURE SetMenuBar(menuList: Handle);
    INLINE $A93C;
PROCEDURE InsMenuItem(theMenu: MenuHandle;itemString: Str255;afterItem: INTEGER);
    INLINE $A826;
PROCEDURE DelMenuItem(theMenu: MenuHandle;item: INTEGER);
    INLINE $A952;
FUNCTION MenuKey(ch: CHAR): LONGINT;
    INLINE $A93E;
PROCEDURE HiliteMenu(menuID: INTEGER);
    INLINE $A938;
PROCEDURE SetItem(theMenu: MenuHandle;item: INTEGER;itemString: Str255);
    INLINE $A947;
PROCEDURE GetItem(theMenu: MenuHandle;item: INTEGER;VAR itemString: Str255);
    INLINE $A946;
PROCEDURE DisableItem(theMenu: MenuHandle;item: INTEGER);
    INLINE $A93A;
PROCEDURE EnableItem(theMenu: MenuHandle;item: INTEGER);
    INLINE $A939;
PROCEDURE CheckItem(theMenu: MenuHandle;item: INTEGER;checked: BOOLEAN);
    INLINE $A945;
PROCEDURE SetItemMark(theMenu: MenuHandle;item: INTEGER;markChar: CHAR);
    INLINE $A944;
PROCEDURE GetItemMark(theMenu: MenuHandle;item: INTEGER;VAR markChar: CHAR);
    INLINE $A943;
PROCEDURE SetItemIcon(theMenu: MenuHandle;item: INTEGER;iconIndex: Byte);
    INLINE $A940;
PROCEDURE GetItemIcon(theMenu: MenuHandle;item: INTEGER;VAR iconIndex: Byte);
    INLINE $A93F;
PROCEDURE SetItemStyle(theMenu: MenuHandle;item: INTEGER;chStyle: Style);
    INLINE $A942;
PROCEDURE GetItemStyle(theMenu: MenuHandle;item: INTEGER;VAR chStyle: Style);
PROCEDURE CalcMenuSize(theMenu: MenuHandle);
    INLINE $A948;
FUNCTION CountMItems(theMenu: MenuHandle): INTEGER;
    INLINE $A950;
FUNCTION GetMHandle(menuID: INTEGER): MenuHandle;
    INLINE $A949;
PROCEDURE FlashMenuBar(menuID: INTEGER);
    INLINE $A94C;
PROCEDURE SetMenuFlash(count: INTEGER);
    INLINE $A94A;
FUNCTION MenuSelect(startPt: Point): LONGINT;
    INLINE $A93D;
PROCEDURE InitProcMenu(resID: INTEGER);
    INLINE $A808;
PROCEDURE GetItemCmd(theMenu: MenuHandle;item: INTEGER;VAR cmdChar: CHAR);
    INLINE $A84E;
PROCEDURE SetItemCmd(theMenu: MenuHandle;item: INTEGER;cmdChar: CHAR);
    INLINE $A84F;
FUNCTION PopUpMenuSelect(menu: MenuHandle;top: INTEGER;left: INTEGER;popUpItem: INTEGER): LONGINT;
    INLINE $A80B;
FUNCTION MenuChoice: LONGINT;
    INLINE $AA66;
PROCEDURE DelMCEntries(menuID: INTEGER;menuItem: INTEGER);
    INLINE $AA60;
FUNCTION GetMCInfo: MCTableHandle;
    INLINE $AA61;
PROCEDURE SetMCInfo(menuCTbl: MCTableHandle);
    INLINE $AA62;
PROCEDURE DispMCInfo(menuCTbl: MCTableHandle);
    INLINE $AA63;
FUNCTION GetMCEntry(menuID: INTEGER;menuItem: INTEGER): MCEntryPtr;
    INLINE $AA64;
PROCEDURE SetMCEntries(numEntries: INTEGER;menuCEntries: MCTablePtr);
    INLINE $AA65;


{$ENDC}    { UsingMenus }

{$IFC NOT UsingIncludes}
    END.
{$ENDC}

