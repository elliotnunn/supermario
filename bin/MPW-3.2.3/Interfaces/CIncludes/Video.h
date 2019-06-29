/************************************************************

Created: Sunday, January 6, 1991 at 10:14 PM
    Video.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1986-1990
        All rights reserved

************************************************************/


#ifndef __VIDEO__
#define __VIDEO__

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


enum {

    mBaseOffset = 1,            /*Id of mBaseOffset.*/
    mRowBytes = 2,              /*Video sResource parameter Id's */
    mBounds = 3,                /*Video sResource parameter Id's */
    mVersion = 4,               /*Video sResource parameter Id's */
    mHRes = 5,                  /*Video sResource parameter Id's */
    mVRes = 6,                  /*Video sResource parameter Id's */
    mPixelType = 7,             /*Video sResource parameter Id's */
    mPixelSize = 8,             /*Video sResource parameter Id's */
    mCmpCount = 9,              /*Video sResource parameter Id's */
    mCmpSize = 10,              /*Video sResource parameter Id's */
    mPlaneBytes = 11,           /*Video sResource parameter Id's */
    mVertRefRate = 14,          /*Video sResource parameter Id's */
    mVidParams = 1,             /*Video parameter block id.*/
    mTable = 2,                 /*Offset to the table.*/
    mPageCnt = 3,               /*Number of pages*/
    mDevType = 4,               /*Device Type*/

    oneBitMode = 128,           /*Id of OneBitMode Parameter list.*/
    twoBitMode = 129,           /*Id of TwoBitMode Parameter list.*/
    fourBitMode = 130,          /*Id of FourBitMode Parameter list.*/
    eightBitMode = 131          /*Id of EightBitMode Parameter list.*/
};
enum {
    sixteenBitMode = 132,       /*Id of SixteenBitMode Parameter list.*/
    thirtyTwoBitMode = 133,     /*Id of ThirtyTwoBitMode Parameter list.*/

    firstVidMode = 128,         /*The new, better way to do the above.    */
    secondVidMode = 129,        /*    QuickDraw only supports six video    */
    thirdVidMode = 130,         /*    at this time.                        */
    fourthVidMode = 131,
    fifthVidMode = 132,
    sixthVidMode = 133,

    spGammaDir = 64,
    spVidNamesDir = 65,

/* Control Codes */
    cscReset = 0,
    cscKillIO = 1,
    cscSetMode = 2,
    cscSetEntries = 3,
    cscSetGamma = 4,
    cscGrayPage = 5,
    cscGrayScreen = 5,
    cscSetGray = 6,
    cscSetInterrupt = 7,
    cscDirectSetEntries = 8
};
enum {
    cscSetDefaultMode = 9,

/* Status Codes */
    cscGetMode = 2,
    cscGetEntries = 3,
    cscGetPageCnt = 4,
    cscGetPages = 4,            /* This is what C&D 2 calls it. */
    cscGetPageBase = 5,
    cscGetBaseAddr = 5,         /* This is what C&D 2 calls it. */
    cscGetGray = 6,
    cscGetInterrupt = 7,
    cscGetGamma = 8,
    cscGetDefaultMode = 9
};

struct VPBlock {
    long vpBaseOffset;          /*Offset to page zero of video RAM (From minorBaseOS).*/
    short vpRowBytes;           /*Width of each row of video memory.*/
    Rect vpBounds;              /*BoundsRect for the video display (gives dimensions).*/
    short vpVersion;            /*PixelMap version number.*/
    short vpPackType;
    long vpPackSize;
    long vpHRes;                /*Horizontal resolution of the device (pixels per inch).*/
    long vpVRes;                /*Vertical resolution of the device (pixels per inch).*/
    short vpPixelType;          /*Defines the pixel type.*/
    short vpPixelSize;          /*Number of bits in pixel.*/
    short vpCmpCount;           /*Number of components in pixel.*/
    short vpCmpSize;            /*Number of bits per component*/
    long vpPlaneBytes;          /*Offset from one plane to the next.*/
};

typedef struct VPBlock VPBlock;
typedef VPBlock *VPBlockPtr;

struct VDEntryRecord {
    Ptr csTable;                /*(long) pointer to color table entry=value, r,g,b:INTEGER*/
};

typedef struct VDEntryRecord VDEntryRecord;
typedef VDEntryRecord *VDEntRecPtr;

/* Parm block for SetGray control call */
struct VDGrayRecord {
    Boolean csMode;             /*Same as GDDevType value (0=mono, 1=color)*/
};

typedef struct VDGrayRecord VDGrayRecord;
typedef VDGrayRecord *VDGrayPtr;

/* Parm block for SetEntries control call */
struct VDSetEntryRecord {
    ColorSpec *csTable;         /*Pointer to an array of color specs*/
    short csStart;              /*Which spec in array to start with, or -1*/
    short csCount;              /*Number of color spec entries to set*/
};

typedef struct VDSetEntryRecord VDSetEntryRecord;
typedef VDSetEntryRecord *VDSetEntryPtr;

/* Parm block for SetGamma control call */
struct VDGammaRecord {
    Ptr csGTable;               /*pointer to gamma table*/
};

typedef struct VDGammaRecord VDGammaRecord;
typedef VDGammaRecord *VDGamRecPtr;

struct VDPageInfo {
    short csMode;               /*(word) mode within device*/
    long csData;                /*(long) data supplied by driver*/
    short csPage;               /*(word) page to switch in*/
    Ptr csBaseAddr;             /*(long) base address of page*/
};

typedef struct VDPageInfo VDPageInfo;
typedef VDPageInfo *VDPgInfoPtr;

struct VDSizeInfo {
    short csHSize;              /*(word) desired/returned h size*/
    short csHPos;               /*(word) desired/returned h position*/
    short csVSize;              /*(word) desired/returned v size*/
    short csVPos;               /*(word) desired/returned v position*/
};

typedef struct VDSizeInfo VDSizeInfo;
typedef VDSizeInfo *VDSzInfoPtr;

struct VDSettings {
    short csParamCnt;           /*(word) number of params*/
    short csBrightMax;          /*(word) max brightness*/
    short csBrightDef;          /*(word) default brightness*/
    short csBrightVal;          /*(word) current brightness*/
    short csCntrstMax;          /*(word) max contrast*/
    short csCntrstDef;          /*(word) default contrast*/
    short csCntrstVal;          /*(word) current contrast*/
    short csTintMax;            /*(word) max tint*/
    short csTintDef;            /*(word) default tint*/
    short csTintVal;            /*(word) current tint*/
    short csHueMax;             /*(word) max hue*/
    short csHueDef;             /*(word) default hue*/
    short csHueVal;             /*(word) current hue*/
    short csHorizDef;           /*(word) default horizontal*/
    short csHorizVal;           /*(word) current horizontal*/
    short csHorizMax;           /*(word) max horizontal*/
    short csVertDef;            /*(word) default vertical*/
    short csVertVal;            /*(word) current vertical*/
    short csVertMax;            /*(word) max vertical*/
};

typedef struct VDSettings VDSettings;
typedef VDSettings *VDSettingsPtr;



#endif
