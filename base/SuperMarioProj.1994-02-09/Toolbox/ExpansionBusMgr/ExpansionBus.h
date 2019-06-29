/*
	File:		ExpansionBus.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	11/11/93	fau		first checked in

*/

/*
 * File:		ExpansionBus.h
 *
 * Contains:	Expansion Mgr Device Tree Manipulation Routines
 *
 * Written by:	Al Kossow
 *
 * Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.
 *
 * Change History (most recent first):
 *
 * <1>			10/20/93	aek		Created
 */
#pragma once
#ifndef __EXPANSIONBUS__
#define __EXPANSIONBUS__

#include "Types.h"
#include "Devices.h"

#define kTypeMask 		0x3		
#define	kDeviceNode		0x1
#define kPropertyNode	0x2

#define kAttribMask		0x70
#define	kAttribLocked	0x10
#define kAttribNVRAM	0x20
#define	kAttribSysSav	0x40

#define kAttribROMProp	0x80
#define kAttribDefProp	0x100

/*
 * The device tree parameter block
 */

typedef unsigned long NodeID;

typedef struct NodeInfo {
		unsigned long	attrib;				// attributes of this node
		NodeID			parentNode;			// nil if node is root node
		NodeID			peerNode;			// nil if no peers
		NodeID			childNode;			// nil if no children
		NodeID			propertyNode;		// device property nodes
		NodeID			propertySize;		// size of property
		char			name[32];			// name of this node
}NodeInfo;

typedef NodeInfo *NodeInfoPtr;

#ifndef _ExpansionBusDispatch
#define _ExpansionBusDispatch 0xAAF3
#endif

/*
 * Expansion Manager C Interface
 *
 * pascal NodeID DevTreeRoot();
 * pascal OSErr DevTreeNodeInfo(NodeID node, NodeInfoPtr p);
 * pascal NodeID DevTreeAddNode(NodeID parentNode, ulong attrib, char *name);
 * pascal OSErr DevTreeDelNode(NodeID theNode);
 * pascal OSErr DevTreeSetProperty(NodeID theNode, unsigned char *buf, long size);
 * pascal OSErr DevTreeGetProperty(NodeID theNode, unsigned char *buf, long size);
 *
 * pascal ExpIntInstall(expIntQElemPtr, nodeID)
 * pascal ExpIntRemove(expIntQElemPtr, nodeID)
 *
 * pascal ExpVInstall(expVblTaskPtr, nodeID)
 * pascal ExpVRemove(expVblTaskPtr, nodeID)
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

pascal OSErr ExpIntInstall(QElemPtr sIntQElemPtr, NodeID nodeID)
	= { 0x303c, 0x0400, _ExpansionBusDispatch};
	
pascal OSErr ExpIntRemove(QElemPtr sIntQElemPtr, NodeID nodeID)
	= { 0x303c, 0x0401, _ExpansionBusDispatch};
	
pascal OSErr ExpVInstall(QElemPtr sIntQElemPtr, NodeID nodeID)
	= { 0x303c, 0x0402, _ExpansionBusDispatch};
	
pascal OSErr ExpVRemove(QElemPtr sIntQElemPtr, NodeID nodeID)
	= { 0x303c, 0x0403, _ExpansionBusDispatch};
	
pascal NodeID DevTreeRoot(void)
	= { 0x303c, 0x0404, _ExpansionBusDispatch};

pascal OSErr DevTreeNodeInfo(NodeID theNode, NodeInfoPtr p)
	= { 0x303c, 0x0405, _ExpansionBusDispatch};

pascal NodeID DevTreeAddNode(NodeID parentNode, unsigned long attrib, char *name)
	= { 0x303c, 0x0606, _ExpansionBusDispatch};

pascal OSErr DevTreeDelNode(NodeID theNode)
	= { 0x303c, 0x0207, _ExpansionBusDispatch};

pascal OSErr DevTreeSetProperty(NodeID theNode, unsigned char *buf, long size)
	= { 0x303c, 0x0608, _ExpansionBusDispatch};

pascal OSErr DevTreeGetProperty(NodeID theNode, unsigned char *buf, long size)
	= { 0x303c, 0x0609, _ExpansionBusDispatch};

#ifdef __cplusplus
}
#endif

#endif