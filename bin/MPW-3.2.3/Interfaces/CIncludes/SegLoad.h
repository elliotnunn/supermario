/************************************************************

Created: Sunday, January 6, 1991 at 10:01 PM
    SegLoad.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved

************************************************************/


#ifndef __SEGLOAD__
#define __SEGLOAD__

#ifndef __TYPES__
#include <Types.h>
#endif


enum {

    appOpen = 0,    /*Open the Document (s)*/
    appPrint = 1    /*Print the Document (s)*/
};

struct AppFile {
    short vRefNum;
    OSType fType;
    short versNum;  /*versNum in high byte*/
    Str255 fName;
};

typedef struct AppFile AppFile;


#ifdef __cplusplus
extern "C" {
#endif
pascal void UnloadSeg(void * routineAddr)
    = 0xA9F1; 
pascal void ExitToShell(void)
    = 0xA9F4; 
pascal void GetAppParms(Str255 apName,short *apRefNum,Handle *apParam)
    = 0xA9F5; 
pascal void CountAppFiles(short *message,short *count); 
pascal void GetAppFiles(short index,AppFile *theFile); 
pascal void ClrAppFiles(short index); 
void getappparms(char *apName,short *apRefNum,Handle *apParam); 
#ifdef __cplusplus
}
#endif

#endif
