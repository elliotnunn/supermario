/************************************************************

Created: Sunday, January 27, 1991 at 8:21 PM
    Balloons.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc. 1990
        All rights reserved

************************************************************/


#ifndef __BALLOONS__
#define __BALLOONS__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __MENUS__
#include <Menus.h>
#endif

#ifndef __TRAPS__
#include <Traps.h>
#endif


enum {

    hmBalloonHelpVersion = 0x0002,      /* The real version of the Help Manager */

/*Help Mgr error range: -850 to -874*/
    hmHelpDisabled = -850,              /* Show Balloons mode was off, call to routine ignored */
    hmBalloonAborted = -853,            /* Returned if mouse was moving or mouse wasn't in window port rect */
    hmSameAsLastBalloon = -854,         /* Returned from HMShowMenuBalloon if menu & item is same as last time */
    hmHelpManagerNotInited = -855,      /* Returned from HMGetHelpMenuHandle if help menu not setup */
    hmSkippedBalloon = -857,            /* Returned from calls if helpmsg specified a skip balloon */
    hmWrongVersion = -858,              /* Returned if help mgr resource was the wrong version */
    hmUnknownHelpType = -859,           /* Returned if help msg record contained a bad type */
    hmOperationUnsupported = -861,      /* Returned from HMShowBalloon call if bad method passed to routine */
    hmNoBalloonUp = -862,               /* Returned from HMRemoveBalloon if no balloon was visible when call was made */
    hmCloseViewActive = -863,           /* Returned from HMRemoveBalloon if CloseView was active */
    kHMHelpMenuID = -16490,             /* Resource ID and menu ID of help menu */
    kHMAboutHelpItem = 1,               /* help menu item number of About Balloon Help… */
    kHMShowBalloonsItem = 3,            /* help menu item number of Show/Hide Balloons */
    kHMHelpID = -5696,                  /* ID of various Help Mgr package resources (in Pack14 range) */
    kBalloonWDEFID = 126,               /* Resource ID of the WDEF proc used in standard balloons */

/*Dialog item template type*/
    helpItem = 1,                       /* key value in DITL template that corresponds to the help item */

/* Options for Help Manager resources in 'hmnu', 'hdlg', 'hrct', 'hovr', & 'hfdr' resources */
    hmDefaultOptions = 0,               /* default options for help manager resources */
    hmUseSubID = 1,                     /* treat resID's in resources as subID's of driver base ID */
    hmAbsoluteCoords = 2                /* ignore window port origin and treat rectangles as absolute coords */
};
enum {
    hmSaveBitsNoWindow = 4,             /* don't create a window, just blast bits on screen. No update event is generated */
    hmSaveBitsWindow = 8,               /* create a window, but restore bits behind window when window goes away */
    hmMatchInTitle = 16,                /* for hwin resources, match string anywhere in window title string */

/* Constants for Help Types in 'hmnu', 'hdlg', 'hrct', 'hovr', & 'hfdr' resources */
    kHMStringItem = 1,                  /* pstring used in resource */
    kHMPictItem = 2,                    /* 'PICT' ResID used in resource */
    kHMStringResItem = 3,               /* 'STR#' ResID & index used in resource */
    kHMTEResItem = 6,                   /* Styled Text Edit ResID used in resource ('TEXT' & 'styl') */
    kHMSTRResItem = 7,                  /* 'STR ' ResID used in resource */
    kHMSkipItem = 256,                  /* don't display a balloon */
    kHMCompareItem = 512,               /* Compare pstring in menu item w/ PString in resource item */
    kHMNamedResourceItem = 1024,        /* Use pstring in menu item to get 'STR#', 'PICT', or 'STR ' resource ('hmnu' only) */
    kHMTrackCntlItem = 2048,            /* Reserved */

/* Constants for hmmHelpType's when filling out HMMessageRecord */
    khmmString = 1,                     /* help message contains a PString */
    khmmPict = 2,                       /* help message contains a resource ID to a 'PICT' resource */
    khmmStringRes = 3,                  /* help message contains a res ID & index to a 'STR#' resource */
    khmmTEHandle = 4,                   /* help message contains a Text Edit handle */
    khmmPictHandle = 5,                 /* help message contains a Picture handle */
    khmmTERes = 6,                      /* help message contains a res ID to 'TEXT' & 'styl' resources */
    khmmSTRRes = 7,                     /* help message contains a res ID to a 'STR ' resource */

#define kHMTETextResType 'TEXT'         /* Resource Type of text data for styled TE record w/o style info */
#define kHMTEStyleResType 'styl'        /* Resource Type of style information for styled TE record */

/*Generic defines for the switch items used in 'hmnu' & 'hdlg'*/

    kHMEnabledItem = 0                  /* item is enabled, but not checked or control value = 0 */
};
enum {
    kHMDisabledItem = 1,                /* item is disabled, grayed in menus or disabled in dialogs */
    kHMCheckedItem = 2,                 /* item is enabled, and checked or control value = 1 */
    kHMOtherItem = 3,                   /* item is enabled, and control value > 1 */

/* Resource Types for whichType parameter used when extracting 'hmnu' & 'hdlg' messages */

#define kHMMenuResType 'hmnu'           /* ResType of help resource for supporting menus */
#define kHMDialogResType 'hdlg'         /* ResType of help resource for supporting dialogs */
#define kHMWindListResType 'hwin'       /* ResType of help resource for supporting windows */
#define kHMRectListResType 'hrct'       /* ResType of help resource for rectangles in windows */
#define kHMOverrideResType 'hovr'       /* ResType of help resource for overriding system balloons */
#define kHMFinderApplResType 'hfdr'     /* ResType of help resource for custom balloon in Finder */

/* constants to pass to method parameter in HMShowBalloon */

    kHMRegularWindow = 0,               /* Create a regular window floating above all windows */
    kHMSaveBitsNoWindow = 1,            /* Just save the bits and draw (for MDEF calls) */
    kHMSaveBitsWindow = 2               /* Regular window, save bits behind, AND generate update event */
};

struct HMStringResType {
    short hmmResID;
    short hmmIndex;
};

typedef struct HMStringResType HMStringResType;

struct HMMessageRecord {
        short        hmmHelpType;
        union {
                    char                hmmString[256];
                    short                hmmPict;
                    Handle                hmmTEHandle;
                    HMStringResType        hmmStringRes;
                    short                hmmPictRes;
                    Handle                hmmPictHandle;
                    short                hmmTERes;
                    short                hmmSTRRes;
                } u;
};

typedef struct HMMessageRecord HMMessageRecord;
typedef HMMessageRecord *HMMessageRecPtr;


#ifdef __cplusplus
extern "C" {
#endif
/*  Public Interfaces  */
pascal OSErr HMGetHelpMenuHandle(MenuHandle *mh)
    = {0x303C,0x0200,_Pack14}; 
pascal OSErr HMShowBalloon(const HMMessageRecord *aHelpMsg,
                           Point tip,
                           RectPtr alternateRect,
                           Ptr tipProc,
                           short theProc,
                           short variant,
                           short method)
    = {0x303C,0x0B01,_Pack14}; 
pascal OSErr HMRemoveBalloon(void)
    = {0x303C,0x0002,_Pack14}; 
pascal Boolean HMGetBalloons(void)
    = {0x303C,0x0003,_Pack14}; 
pascal OSErr HMSetBalloons(Boolean flag)
    = {0x303C,0x0104,_Pack14}; 
pascal OSErr HMShowMenuBalloon(short itemNum,
                               short itemMenuID,
                               long itemFlags,
                               long itemReserved,
                               Point tip,
                               RectPtr alternateRect,
                               Ptr tipProc,
                               short theProc,
                               short variant)
    = {0x303C,0x0E05,_Pack14}; 
pascal OSErr HMGetIndHelpMsg(ResType whichType,
                             short whichResID,
                             short whichMsg,
                             short whichState,
                             long *options,
                             Point *tip,
                             Rect *altRect,
                             short *theProc,
                             short *variant,
                             HMMessageRecord *aHelpMsg,
                             short *count)
    = {0x303C,0x1306,_Pack14}; 
pascal Boolean HMIsBalloon(void)
    = {0x303C,0x0007,_Pack14}; 
pascal OSErr HMSetFont(short font)
    = {0x303C,0x0108,_Pack14}; 
pascal OSErr HMSetFontSize(short fontSize)
    = {0x303C,0x0109,_Pack14}; 
pascal OSErr HMGetFont(short *font)
    = {0x303C,0x020A,_Pack14}; 
pascal OSErr HMGetFontSize(short *fontSize)
    = {0x303C,0x020B,_Pack14}; 
pascal OSErr HMSetDialogResID(short resID)
    = {0x303C,0x010C,_Pack14}; 
pascal OSErr HMSetMenuResID(short menuID,
                            short resID)
    = {0x303C,0x020D,_Pack14}; 
pascal OSErr HMBalloonRect(const HMMessageRecord *aHelpMsg,
                           Rect *coolRect)
    = {0x303C,0x040E,_Pack14}; 
pascal OSErr HMBalloonPict(const HMMessageRecord *aHelpMsg,
                           PicHandle *coolPict)
    = {0x303C,0x040F,_Pack14}; 
pascal OSErr HMScanTemplateItems(short whichID,
                                 short whichResFile,
                                 ResType whichType)
    = {0x303C,0x0410,_Pack14}; 
pascal OSErr HMExtractHelpMsg(ResType whichType,short whichResID,short whichMsg,
    short whichState,HMMessageRecord *aHelpMsg)
    = {0x303C,0x0711,_Pack14}; 
pascal OSErr HMGetDialogResID(short *resID)
    = {0x303C,0x0213,_Pack14}; 
pascal OSErr HMGetMenuResID(short menuID,short *resID)
    = {0x303C,0x0314,_Pack14}; 
pascal OSErr HMGetBalloonWindow(WindowPtr *window)
    = {0x303C,0x0215,_Pack14}; 
#ifdef __cplusplus
}
#endif

#endif
