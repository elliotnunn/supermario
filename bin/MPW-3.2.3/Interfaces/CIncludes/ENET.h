/************************************************************

Created: Thursday, October 4, 1990 at 8:00 AM
	ENET.h
	C Interface to the Macintosh Libraries


	Copyright Apple Computer, Inc. 1989-1990
	All rights reserved

************************************************************/


#ifndef __ENET__
#define __ENET__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#define ENetSetGeneral 253		 /*Set "general" mode*/
#define ENetGetInfo 252			 /*Get info*/
#define ENetRdCancel 251		 /*Cancel read*/
#define ENetRead 250			 /*Read*/
#define ENetWrite 249			 /*Write*/
#define ENetDetachPH 248		 /*Detach protocol handler*/
#define ENetAttachPH 247		 /*Attach protocol handler*/
#define ENetAddMulti 246		 /*Add a multicast address*/
#define ENetDelMulti 245		 /*Delete a multicast address*/

#define eLenErr -92            /*Length error ddpLenErr*/
#define eMultiErr -91          /*Multicast address error ddpSktErr*/

#define EAddrRType 'eadr'      /*Alternate address resource type*/


#define EParamHeader \
	QElem *qLink;				/*next queue entry*/\
	short qType;				/*queue type*/\
	short ioTrap;				/*routine trap*/\
	Ptr ioCmdAddr;				/*routine address*/\
	ProcPtr ioCompletion;		/*completion routine*/\
	OSErr ioResult; 			/*result code*/\
	StringPtr ioNamePtr;		/*->filename*/\
	short ioVRefNum;			/*volume reference or drive number*/\
	short ioRefNum; 			/*driver reference number*/\
	short csCode;				/*Call command code*/

typedef struct {
	EParamHeader					/*General EParams*/
	short eProtType;					/*Ethernet protocol type*/
	Ptr ePointer;
	short eBuffSize;					/*buffer size*/
	short eDataSize;					/*number of bytes read*/
}EParamMisc1;

typedef struct {
	EParamHeader
	EParamMisc1 EParms1;		
	char eMultiAddr[5];					/*Multicast Address*/
}EParamMisc2;

typedef union {
	EParamMisc1 EParms1;		
	EParamMisc2 EParms2;		
}EParamBlock;

typedef EParamBlock *EParamBlkPtr;


#ifdef __cplusplus
extern "C" {
#endif
pascal OSErr EWrite(EParamBlkPtr thePBptr,Boolean async); 
pascal OSErr EAttachPH(EParamBlkPtr thePBptr,Boolean async);
pascal OSErr EDetachPH(EParamBlkPtr thePBptr,Boolean async); 
pascal OSErr ERead(EParamBlkPtr thePBptr,Boolean async); 
pascal OSErr ERdCancel(EParamBlkPtr thePBptr,Boolean async);
pascal OSErr EGetInfo(EParamBlkPtr thePBptr,Boolean async);
pascal OSErr ESetGeneral(EParamBlkPtr thePBptr,Boolean async);
pascal OSErr EAddMulti(EParamBlkPtr thePBptr,Boolean async);
pascal OSErr EDelMulti(EParamBlkPtr thePBptr,Boolean async);

#ifdef __cplusplus
}
#endif

#endif
