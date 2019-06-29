/************************************************************

Created: Sunday, January 6, 1991 at 10:01 PM
    Serial.h
    C Interface to the Macintosh Libraries


        Copyright Apple Computer, Inc.    1985-1990
        All rights reserved

************************************************************/


#ifndef __SERIAL__
#define __SERIAL__

#ifndef __TYPES__
#include <Types.h>
#endif


enum {

    baud300 = 380,
    baud600 = 189,
    baud1200 = 94,
    baud1800 = 62,
    baud2400 = 46,
    baud3600 = 30,
    baud4800 = 22,
    baud7200 = 14,
    baud9600 = 10,
    baud19200 = 4,
    baud57600 = 0,
    stop10 = 16384,
    stop15 = -32768,
    stop20 = -16384,
    noParity = 0,
    oddParity = 4096,
    evenParity = 12288,
    data5 = 0,
    data6 = 2048,
    data7 = 1024
};
enum {
    data8 = 3072,
    ctsEvent = 32,
    breakEvent = 128,
    xOffWasSent = 128,
    dtrNegated = 64,
    ainRefNum = -6,         /*serial port A input*/
    aoutRefNum = -7,        /*serial port A output*/
    binRefNum = -8,         /*serial port B input*/
    boutRefNum = -9         /*serial port B output*/
};

enum {sPortA,sPortB};
typedef unsigned char SPortSel;


struct SerShk {
    char fXOn;              /*XOn flow control enabled flag*/
    char fCTS;              /*CTS flow control enabled flag*/
    unsigned char xOn;      /*XOn character*/
    unsigned char xOff;     /*XOff character*/
    char errs;              /*errors mask bits*/
    char evts;              /*event enable mask bits*/
    char fInX;              /*Input flow control enabled flag*/
    char fDTR;              /*DTR input flow control flag*/
};

typedef struct SerShk SerShk;

struct SerStaRec {
    char cumErrs;
    char xOffSent;
    char rdPend;
    char wrPend;
    char ctsHold;
    char xOffHold;
};

typedef struct SerStaRec SerStaRec;


#ifdef __cplusplus
extern "C" {
#endif
pascal OSErr SerReset(short refNum,short serConfig); 
pascal OSErr SerSetBuf(short refNum,Ptr serBPtr,short serBLen); 
pascal OSErr SerHShake(short refNum,const SerShk *flags); 
pascal OSErr SerSetBrk(short refNum); 
pascal OSErr SerClrBrk(short refNum); 
pascal OSErr SerGetBuf(short refNum,long *count); 
pascal OSErr SerStatus(short refNum,SerStaRec *serSta); 
#ifdef __cplusplus
}
#endif

#endif
