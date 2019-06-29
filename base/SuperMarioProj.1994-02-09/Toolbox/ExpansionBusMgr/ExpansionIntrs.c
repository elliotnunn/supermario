/*
	File:		ExpansionIntrs.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	11/11/93	fau		first checked in

*/

/*
 * File:		DeviceTree.c
 *
 * Contains:	Expansion Mgr Interrupt Registration and Removal
 *
 * Written by:	Al Kossow
 *
 * Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.
 *
 * Change History (most recent first):
 *
 * <1>			10/20/93	aek		Created
 *
 *
 *	ExpIntInstall(expIntQElemPtr, nodeID)
 *	ExpIntRemove(expIntQElemPtr, nodeID)
 *
 *	ExpVInstall(expVblTaskPtr, nodeID)
 *	ExpVRemove(expVblTaskPtr, nodeID)
 *
 *
 */
 
 
 #include "ExpansionMgrInternal.h"
 #include "OSUtils.h"
 
 pascal OSErr ExpIntInstall(sIntQElemPtr, nodeID)
 QElemPtr sIntQElemPtr;
 NodeID nodeID;
 {

 #pragma unused(sIntQElemPtr)
 #pragma unused(nodeID)
 
 	return(noErr);
 }
 
 pascal OSErr ExpIntRemove(sIntQElemPtr, nodeID)
 QElemPtr sIntQElemPtr;
 NodeID nodeID;
 {
 
 #pragma unused(sIntQElemPtr)
 #pragma unused(nodeID)
 
	return(noErr);
 }
 
 pascal OSErr ExpVInstall(vblTaskPtr, nodeID)
 QElemPtr vblTaskPtr;
 NodeID nodeID;
 {

 #pragma unused(vblTaskPtr)
 #pragma unused(nodeID)
 
 	return(noErr);
 }
 
 pascal OSErr ExpVRemove(vblTaskPtr, nodeID)
 QElemPtr vblTaskPtr;
 NodeID nodeID;
 {

 #pragma unused(vblTaskPtr)
 #pragma unused(nodeID)
 
 	return(noErr);
 }