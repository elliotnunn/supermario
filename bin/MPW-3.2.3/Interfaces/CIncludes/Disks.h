/************************************************************

Created: Sunday, January 6, 1991 at 9:08 PM
    Disks.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.     1985-1989
        All rights reserved

************************************************************/


#ifndef __DISKS__
#define __DISKS__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


struct DrvSts {
    short track;
    char writeProt;
    char diskInPlace;
    char installed;
    char sides;
    QElemPtr qLink;
    short qType;
    short dQDrive;
    short dQRefNum;
    short dQFSID;
    char twoSideFmt;
    char needsFlush;
    short diskErrs;
};

typedef struct DrvSts DrvSts;

struct DrvSts2 {
    short track;
    char writeProt;
    char diskInPlace;
    char installed;
    char sides;
    QElemPtr qLink;
    short qType;
    short dQDrive;
    short dQRefNum;
    short dQFSID;
    short driveSize;
    short driveS1;
    short driveType;
    short driveManf;
    short driveChar;
    char driveMisc;
};

typedef struct DrvSts2 DrvSts2;


#ifdef __cplusplus
extern "C" {
#endif
pascal OSErr DiskEject(short drvNum); 
pascal OSErr SetTagBuffer(void *buffPtr); 
pascal OSErr DriveStatus(short drvNum,DrvSts *status); 
#ifdef __cplusplus
}
#endif

#endif
