/************************************************************

Created: Sunday, January 6, 1991 at 9:31 PM
    Menus.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved

************************************************************/


#ifndef __MENUS__
#define __MENUS__

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


enum {


#define noMark '\0'         /*mark symbol for MarkItem*/

/* menu defProc messages */

    mDrawMsg = 0,
    mChooseMsg = 1,
    mSizeMsg = 2,
    mDrawItemMsg = 4,
    mCalcItemMsg = 5,
    textMenuProc = 0,
    hMenuCmd = 27,          /*itemCmd == 0x001B ==> hierarchical menu*/
    hierMenu = -1,          /*a hierarchical menu - for InsertMenu call*/
    mPopUpMsg = 3,          /*menu defProc messages - place yourself*/
    mctAllItems = -98,      /*search for all Items for the given ID*/
    mctLastIDIndic = -99    /*last color table entry has this in ID field*/
};

struct MenuInfo {
    short menuID;
    short menuWidth;
    short menuHeight;
    Handle menuProc;
    long enableFlags;
    Str255 menuData;
};

typedef struct MenuInfo MenuInfo;
typedef MenuInfo *MenuPtr, **MenuHandle;

struct MCEntry {
    short mctID;            /*menu ID.  ID = 0 is the menu bar*/
    short mctItem;          /*menu Item. Item = 0 is a title*/
    RGBColor mctRGB1;       /*usage depends on ID and Item*/
    RGBColor mctRGB2;       /*usage depends on ID and Item*/
    RGBColor mctRGB3;       /*usage depends on ID and Item*/
    RGBColor mctRGB4;       /*usage depends on ID and Item*/
    short mctReserved;      /*reserved for internal use*/
};

typedef struct MCEntry MCEntry;
typedef MCEntry *MCEntryPtr;


/**/

typedef MCEntry MCTable[1], *MCTablePtr, **MCTableHandle;

struct MenuCRsrc {
    short numEntries;       /*number of entries*/
    MCTable mcEntryRecs;    /*ARRAY [1..numEntries] of MCEntry*/
};

typedef struct MenuCRsrc MenuCRsrc;
typedef MenuCRsrc *MenuCRsrcPtr, **MenuCRsrcHandle;


#ifdef __cplusplus
extern "C" {
#endif
pascal void InitMenus(void)
    = 0xA930; 
pascal MenuHandle NewMenu(short menuID,const Str255 menuTitle)
    = 0xA931; 
pascal MenuHandle GetMenu(short resourceID)
    = 0xA9BF; 
pascal void DisposeMenu(MenuHandle theMenu)
    = 0xA932; 
pascal void AppendMenu(MenuHandle menu,ConstStr255Param data)
    = 0xA933; 
pascal void AddResMenu(MenuHandle theMenu,ResType theType)
    = 0xA94D; 
pascal void InsertResMenu(MenuHandle theMenu,ResType theType,short afterItem)
    = 0xA951; 
pascal void InsertMenu(MenuHandle theMenu,short beforeID)
    = 0xA935; 
pascal void DrawMenuBar(void)
    = 0xA937; 
pascal void InvalMenuBar(void)
    = 0xA81D; 
pascal void DeleteMenu(short menuID)
    = 0xA936; 
pascal void ClearMenuBar(void)
    = 0xA934; 
pascal Handle GetNewMBar(short menuBarID)
    = 0xA9C0; 
pascal Handle GetMenuBar(void)
    = 0xA93B; 
pascal void SetMenuBar(Handle menuList)
    = 0xA93C; 
pascal void InsMenuItem(MenuHandle theMenu,ConstStr255Param itemString,
    short afterItem)
    = 0xA826; 
pascal void DelMenuItem(MenuHandle theMenu,short item)
    = 0xA952; 
pascal long MenuKey(short ch)
    = 0xA93E; 
pascal void HiliteMenu(short menuID)
    = 0xA938; 
pascal void SetItem(MenuHandle theMenu,short item,ConstStr255Param itemString)
    = 0xA947; 
pascal void GetItem(MenuHandle theMenu,short item,Str255 itemString)
    = 0xA946; 
pascal void DisableItem(MenuHandle theMenu,short item)
    = 0xA93A; 
pascal void EnableItem(MenuHandle theMenu,short item)
    = 0xA939; 
pascal void CheckItem(MenuHandle theMenu,short item,Boolean checked)
    = 0xA945; 
pascal void SetItemMark(MenuHandle theMenu,short item,short markChar)
    = 0xA944; 
pascal void GetItemMark(MenuHandle theMenu,short item,short *markChar)
    = 0xA943; 
pascal void SetItemIcon(MenuHandle theMenu,short item,short iconIndex)
    = 0xA940; 
pascal void GetItemIcon(MenuHandle theMenu,short item,short *iconIndex)
    = 0xA93F; 
pascal void SetItemStyle(MenuHandle theMenu,short item,short chStyle)
    = 0xA942; 
pascal void GetItemStyle(MenuHandle theMenu,short item,Style *chStyle); 
pascal void CalcMenuSize(MenuHandle theMenu)
    = 0xA948; 
pascal short CountMItems(MenuHandle theMenu)
    = 0xA950; 
pascal MenuHandle GetMHandle(short menuID)
    = 0xA949; 
pascal void FlashMenuBar(short menuID)
    = 0xA94C; 
pascal void SetMenuFlash(short count)
    = 0xA94A; 
pascal long MenuSelect(Point startPt)
    = 0xA93D; 
pascal void InitProcMenu(short resID)
    = 0xA808; 
pascal void GetItemCmd(MenuHandle theMenu,short item,short *cmdChar)
    = 0xA84E; 
pascal void SetItemCmd(MenuHandle theMenu,short item,short cmdChar)
    = 0xA84F; 
pascal long PopUpMenuSelect(MenuHandle menu,short top,short left,short popUpItem)
    = 0xA80B; 
pascal long MenuChoice(void)
    = 0xAA66; 
pascal void DelMCEntries(short menuID,short menuItem)
    = 0xAA60; 
pascal MCTableHandle GetMCInfo(void)
    = 0xAA61; 
pascal void SetMCInfo(MCTableHandle menuCTbl)
    = 0xAA62; 
pascal void DispMCInfo(MCTableHandle menuCTbl)
    = 0xAA63; 
pascal MCEntryPtr GetMCEntry(short menuID,short menuItem)
    = 0xAA64; 
pascal void SetMCEntries(short numEntries,MCTablePtr menuCEntries)
    = 0xAA65; 
MenuHandle newmenu(short menuID,char *menuTitle); 
void getitem(MenuHandle menu,short item,char *itemString); 
void appendmenu(MenuHandle menu,char *data); 
void insmenuitem(MenuHandle theMenu,char *itemString,short afterItem); 
long menuselect(Point *startPt); 
void setitem(MenuHandle menu,short item,char *itemString); 
#ifdef __cplusplus
}
#endif

#endif
