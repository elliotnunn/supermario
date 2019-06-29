/*
	File:		ExpansionMgrInternal.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	11/11/93	fau		first checked in

*/

/*
 *
 *	Expansion Manager
 *	Device Tree Internal Data Structures
 *
 */
#include "Types.h"

/*
 *	Device Tree Data Structures
 *
 *
 *  A Device Node:
 *
 *            +---------------------------+
 *            | Node Attributes           |
 *            +------+------+------+------+
 *            | Parent Node of this Node  |		<<-- Root nodes's parent is Nil
 *            +------+------+------+------+
 *            | Next Peer Device Node     |		<<-- Circular list of all peers
 *            +------+------+------+------+
 *            | Child Node List Head      |		<<-- Pointer to first Child, or Nil
 *            +------+------+------+------+
 *            | Child Node List Tail      |		<<-- Allocation hint for Children
 *            +------+------+------+------+
 *            | Property List Head        |		<<-- Pointer to first Property, or Nil
 *            +------+------+------+------+
 *            | Property List Tail        |		<<-- Allocation hint for Properties
 *            +------+------+------+------+-----------------+--+
 *            | Null-terminated Node Name  (31 chrs max)    |\0|
 *            +---------------------------------------------+--+
 *
 */

typedef struct DeviceNode {
		unsigned long 		attrib;
		struct DeviceNode 	*parent;
		struct DeviceNode 	*peer;
		struct DeviceNode	*childList;
		struct DeviceNode 	*childTail;
		struct PropertyNode *propertyList;
		struct PropertyNode *propertyTail;
		char				name[32];
}DeviceNode;

typedef DeviceNode *DeviceNodePtr;

/*
 *  
 *  A Property Node:
 *
 *            +------+------+------+------+
 *            | Node Attributes           |
 *			  +------+------+------+------+
 *			  | Parent Node of this Prop  |
 *            +------+------+------+------+
 *            | Next Peer Property        |		<<-- Circular list of all peers	
 *            +------+------+------+------+
 *            | Property Size             |
 *            +------+------+------+------+
 *            | Property Value Pointer    |
 *            +------+------+------+------+------------------+--+
 *            | Null-terminated Property Name (31 chrs max)  |\0|
 *            +----------------------------------------------+--+
 *
 *
 * NOTE: attrib, parent, peer offsets must be kept the same relative to device
 *       nodes for this implementation.
 */

typedef struct PropertyNode {

		unsigned long 		attrib;
		struct DeviceNode 	*parent;
		struct PropertyNode	*peer;
		unsigned long		size;
		unsigned char		*dataPtr;
		char 				name[32];
}PropertyNode;

typedef PropertyNode *PropertyNodePtr;

/*
 *	Node attributes:
 *									// User-visible NODE TYPES
 *  kDeviceNode						// Node is a Device Node
 *  kPropertyNode					// Node is a Property Node
 *
 *									// User-visible NODE ATTRIBUTES
 *	kAttribLocked					// Node cannot be changed or deleted
 *	kAttribNVRAM					// Property saved in system NVRAM
 *	kAttribSysSav					// Property saved in the system folder
 *
 *									// Implementation private NODE ATTRIBUTES
 *	kAttribROMProp					// Property data in ROM, val cannot be changed
 *  kAttribDefProp					// Property data indirectly accessed
 */

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

/*
 * Function Prototypes
 */
 
pascal NodeID DevTreeRoot();
pascal OSErr DevTreeNodeInfo(NodeID node, NodeInfoPtr p);
pascal NodeID DevTreeAddNode(NodeID parentNode, ulong attrib, char *name);
pascal OSErr DevTreeDelNode(NodeID theNode);
pascal OSErr DevTreeSetProperty(NodeID theNode, unsigned char *buf, long size);
pascal OSErr DevTreeGetProperty(NodeID theNode, unsigned char *buf, long size);

