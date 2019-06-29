/************************************************************

Created: Sunday, January 6, 1991 at 10:02 PM
    ShutDown.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1987-1989
        All rights reserved

************************************************************/


#ifndef __SHUTDOWN__
#define __SHUTDOWN__

#ifndef __TYPES__
#include <Types.h>
#endif


enum {

    sdOnPowerOff = 1,       /*call procedure before power off.*/
    sdOnRestart = 2,        /*call procedure before restart.*/
    sdOnUnmount = 4,        /*call procedure before unmounting.*/
    sdOnDrivers = 8,        /*call procedure before closing drivers.*/
    sdRestartOrPower = 3    /*call before either power off or restart.*/
};

typedef pascal void (*ShutDwnProcPtr)(void);

#ifdef __cplusplus
extern "C" {
#endif
pascal void ShutDwnPower(void)
    = {0x3F3C,0x0001,0xA895}; 
pascal void ShutDwnStart(void)
    = {0x3F3C,0x0002,0xA895}; 
pascal void ShutDwnInstall(ShutDwnProcPtr shutDownProc,short flags)
    = {0x3F3C,0x0003,0xA895}; 
pascal void ShutDwnRemove(ShutDwnProcPtr shutDownProc)
    = {0x3F3C,0x0004,0xA895}; 
#ifdef __cplusplus
}
#endif

#endif
