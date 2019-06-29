/*
  File:		DeviceTree.c
 
  Contains:	Expansion Mgr Device Tree Manipulation Routines
 
  Written by:	Al Kossow
 
  Copyright:	© 1993-1994 by Apple Computer, Inc., all rights reserved.
 
  Change History (most recent first):

	   <SM5>	  2/7/94	AK		fix warning in universal builds

	   <SM5>    2/07/94 aek		get rid of warning on universal builds
	   <SM4>	1/18/94	AK		fix device tree init
 */
 
/*
 *		 <4>	01/18/94	aek		make sure adrs aren't updated on restart
 *		 <3>	01/18/94	aek		correct tree initialization code
 *		 <2>	01/17/94	aek		wire into Open Boot
 * 		 <1>	10/20/93	aek		Created
 *
 *
 * Device nodes can have other device nodes as children.
 *
 * The bulk of the Device Tree is built at system initialization time in
 * the system heap from information passed to StartInit from Open Boot.
 *
 * It is slightly different from the Device Tree in Open Boot, since there
 * are client calls to remove as well as add devices and properties to
 * the tree.
 *
 * No assuptions should be made about what a nodeid represents. It may be
 * a pointer, a handle, or a small integer, depending upon the underlying
 * implemetation of the device tree routines.
 *
 *
 * pascal NodeID DevTreeRoot();
 * pascal OSErr  DevTreeNodeInfo(NodeID node, NodeInfoPtr p);
 * pascal NodeID DevTreeAddNode(NodeID parentNode, ulong attrib, char *name);
 * pascal OSErr  DevTreeDelNode(NodeID theNode);
 * pascal OSErr  DevTreeSetProperty(NodeID theNode, unsigned char *buf, long size);
 * pascal OSErr  DevTreeGetProperty(NodeID theNode, unsigned char *buf, long size);
 *
 *
 * TODO:
 *	Real error code #'s
 *	Non-volatile properties
 *  Properties saved in the system file
 *
 */

#include "ExpansionMgrInternal.h"
#include "Memory.h"

#ifndef TEST


#ifndef __ERRORS__
#include <Errors.h>
#endif

#ifndef __EXPANDMEMPRIV__
#include "ExpandMemPriv.h"
#endif
#endif

/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* Private Device Tree Interface Routines                                 */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

/*
 * Internal routine to allocate memory for a tree node
 * Returns NIL if allocation fails
 */

Ptr AllocTreeMem(size)
{
#ifdef TEST
	return(NewPtrClear(size));
#else
	return(NewPtrSysClear(size));
#endif
}

/*
 * Internal routine to free memory for a tree node
 */
FreeTreeMem(ptr)
Ptr ptr;
{
	DisposPtr(ptr);
}

/*
 * Internal routine to free data associated with a property
 * Checks to make sure data isn't in ROM or is indirectly referenced
 */
FreePropData(pp)
PropertyNodePtr pp;
{
	if(pp->attrib & (kAttribROMProp | kAttribDefProp))
	 return(-1);
	DisposPtr((Ptr)pp);
}

/*
 * CheckNodeName(name)
 *
 * Verify that name is a valid device tree name
 * 31 chrs max, letters, digits ,',' ,'_', '+', or '-'
 *
 * Returns the number of characters in the name, or zero
 * if there was an error
 *
 * TODO: really verify it..
 */
int CheckNodeName(name)
char *name;
{
	register char *cp = name;
	register int len = 0;
	
	while( *cp && (len < 32)){
	 len++, cp++;
	}
	if(len == 32)
	 return(0);
	else
	 return(len);
}

/*
 * CheckPropertyName(name)
 *
 * Verify that the name is a valid property name
 * 31 chrs max, no upper case, '/', '\', ':', '[', ']' or '@'
 * Returns the number of characters in the name, or zero
 * if there was an error
 *
 * TODO: really verify it..
 */
int CheckPropertyName(name)
char *name;
{
	register char *cp = name;
	register int len = 0;
	
	while( *cp && (len < 32)){
	 len++, cp++;
	}
	if(len == 32)
	 return(0);
	else
	 return(len);
}

/*
 * Validate the NodeID that was passed to us
 * Returns TRUE if NodeID actually exists in
 * the tree by looking at all nodes starting
 * at the root
 *
 * TODO: really verify it..
 */
Boolean CheckNodeID(theNode)
NodeID theNode;
{

#pragma unused(theNode);

	return(TRUE);
}

/**************************************************************************/
/*                                                                        */
/* Munge the tree passed to us by open boot                               */
/*                                                                        */
/**************************************************************************/

cvtDevNode(dp, depth, DTBase)
DeviceNodePtr dp;
int depth;
unsigned long DTBase;
{
	register DeviceNodePtr childptr;
	register PropertyNodePtr propptr;
	
	if(dp->parent)
	 dp->parent = (DeviceNodePtr)((unsigned long)(dp->parent) + DTBase);
	 
	dp->peer   = (DeviceNodePtr)((unsigned long)(dp->peer) + DTBase);
	if(dp->propertyList){
		dp->propertyList = (PropertyNodePtr)((unsigned long)dp->propertyList + DTBase);
		dp->propertyTail = (PropertyNodePtr)((unsigned long)dp->propertyTail + DTBase);
		propptr = dp->propertyList;
		do {
		 propptr->parent = (DeviceNodePtr)((unsigned long)propptr->parent + DTBase);
		 propptr->peer   = (PropertyNodePtr)((unsigned long)propptr->peer + DTBase);
		 if(propptr->dataPtr)
		  propptr->dataPtr = (unsigned char *)propptr->dataPtr + DTBase;
		 propptr = propptr->peer;
		} while (propptr != dp->propertyList);
	}
	if(dp->childList){
		dp->childList = (DeviceNodePtr)((unsigned long)(dp->childList) + DTBase);
		dp->childTail = (DeviceNodePtr)((unsigned long)(dp->childTail) + DTBase);
		childptr = (DeviceNodePtr)dp->childList;
		do {
		 cvtDevNode(childptr, depth+1, DTBase);
		 childptr = childptr->peer;
		} while (childptr != (DeviceNodePtr)dp->childList);
	}
}

/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* Public Device Tree Interface Routines                                  */
/*                                                                        */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* DevTreeRoot()                                                          */
/*                                                                        */
/* Return the root node ID in the nodeID field of the parameter block     */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
pascal NodeID DevTreeRoot()
{
	register DeviceNodePtr dp;
	register unsigned short i;
	
	/*
	 * get the saved tree base address
	 */
	 
	dp = (DeviceNodePtr)GetExpandMemExpansionBusGlobals();

	/*
	 * test if the device tree has been initialized
	 * return node id of root if it has been 
	 */
	 
	if(dp)
	 return((NodeID)dp);

	/*
	 * If we're running on a machine with Open Firmware
	 * we get device tree info at startup
	 * Wire the info in as the initial device tree
	 */
	 
#ifdef forTNTDebug
	i = *(unsigned short *)0x5ff00000;
	if((i & 0x8000) == 0){
	 cvtDevNode(0x5ff00004, 1, (unsigned long)0x5ff00000L);
	 *(unsigned short *)0x5ff00000 = i | 0x8000;
	}
	dp = (DeviceNodePtr)0x5ff00004;
#endif

/*
 * Create a device tree in the system heap if there
 * wasn't one passed to us.
 */
#ifndef forTNTDebug
		dp = (DeviceNodePtr)AllocTreeMem(sizeof(DeviceNode));
		dp->attrib = kDeviceNode | kAttribLocked;
		dp->name[0] = 'r'; dp->name[1] = 'o'; dp->name[2] = 'o'; dp->name[3] = 't';
#endif

	SetExpandMemExpansionBusGlobals((Ptr)dp);		/* remember where the root is */
	return((NodeID)dp);
}


/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* DevTreeNodeInfo()                                                      */
/*                                                                        */
/* Return info about a node in the device tree                            */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
 
pascal OSErr DevTreeNodeInfo(node, p)
NodeID node;
NodeInfoPtr p;
{
	register DeviceNodePtr dp;
	register PropertyNodePtr pp;
	register char *src, *dst;
	register int cnt;
	
	dp = (DeviceNodePtr)node;

	/*
	 * Fill in values unique to each node type
	 */
	switch(dp->attrib & kTypeMask) {

		case(kDeviceNode):
		 p->childNode = (NodeID)dp->childList;
		 p->propertyNode = (NodeID)dp->propertyList;
		 src = dp->name;
		 break;
		 
		case(kPropertyNode):
		 pp = (PropertyNodePtr)dp;	// convert to prop node ptr
		 p->childNode = 0;			// properties don't have children
		 p->propertySize = pp->size;
		 p->propertyNode = 0;
		 src = pp->name;
		 break;
		 
		default:	
		 return(-1);				// all other node types are bogus
	}
	
	/*
	 * Fill in info common to all nodes;
	 * attributes, parent, peer, and name.
	 */
	p->attrib = dp->attrib & (kTypeMask | kAttribMask);		// return only user-settable bits
	p->parentNode = (NodeID)dp->parent;
	p->peerNode	  = (NodeID)dp->peer;

	/*
	 * Copy node name using src pointer formed in node-specific
	 * code
	 */
	dst = p->name;
	for(cnt = 32; cnt; cnt--)
	 *dst++ = *src++;				// copy name into record	
	return(noErr);
}

/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* DevTreeAddNode()                                                       */
/*                                                                        */
/* Add a node to the Device Tree                                          */
/* Creating nodes with special attributes are done through a non-exported */
/* node creation routine                                                  */
/*                                                                        */
/* The naming conventions for Device and Property nodes are the same as   */
/* the conventions for Open Boot:                                         */
/*                                                                        */
/* Device Nodes: Mixed-case letters, digits, ',' '_' '+' '-'              */
/*                                                                        */
/*          ' ' !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /               */
/*                                            _  _  _                     */
/*                                                                        */
/*           0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?               */
/*           _  _  _  _  _  _  _  _  _  _                                 */
/*                                                                        */
/*           @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O               */
/*              _  _  _  _  _  _  _  _  _  _  _  _  _  _  _               */
/*                                                                        */
/*           P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _               */
/*           _  _  _  _  _  _  _  _  _  _  _                              */
/*                                                                        */
/*           `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o               */
/*              _  _  _  _  _  _  _  _  _  _  _  _  _  _  _               */
/*                                                                        */
/*           p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~                  */
/*           _  _  _  _  _  _  _  _  _  _  _                              */
/*                                                                        */
/*                                                                        */
/* Properties:   All printable characters except upper case letters,      */
/*               '/' '\' ':' '[' ']' '@'                                  */
/*                                                                        */
/*          ' ' !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /               */
/*           _  _  _  _  _  _  _  _  _  _  _  _  _  _  _                  */
/*                                                                        */
/*           0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?               */
/*           _  _  _  _  _  _  _  _  _  _     _  _  _  _  _               */
/*                                                                        */
/*           @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O               */
/*              _  _  _  _  _  _  _  _  _  _  _  _  _  _  _               */
/*                                                                        */
/*           P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _               */
/*           _  _  _  _  _  _  _  _  _  _  _           _  _               */
/*                                                                        */
/*           `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o               */
/*           _                                                            */
/*                                                                        */
/*           p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~                  */
/*                                            _  _  _  _                  */
/*                                                                        */
/* Note: Open Boot defines Printable Characters as 0x21 -> 0x7e and       */
/*       0xa1 -> 0xfe                                                     */
/*                                                                        */
/**************************************************************************/

pascal NodeID DevTreeAddNode( parentNode ,attrib, name)
NodeID parentNode;
unsigned long attrib;
char *name;
{
	register nameLen;
	char *src, *dst;
	DeviceNodePtr dp;
	PropertyNodePtr pp;
	NodeID retVal;
	
	if(!CheckNodeID(parentNode))
	 return(-1);												// make sure parent exists
	src = name;	
	dp = (DeviceNodePtr)parentNode;
	if((dp->attrib & kTypeMask) != kDeviceNode)
	 return(-1);												// parent must be device node
	switch(attrib & kTypeMask) {								// check for valid node types
	
		/*
		 * create a new device node
		 */
		case(kDeviceNode):
			if((nameLen = CheckNodeName(name)) == 0)
				return(-1);										// something wrong with name
			if(attrib & (kAttribNVRAM | kAttribSysSav))
			 return(-1);										// only properties are non-volatile
			dp = (DeviceNodePtr)AllocTreeMem(sizeof(DeviceNode));
			retVal = (NodeID)dp;
			dst = dp->name;
			dp->attrib = attrib & (kTypeMask | kAttribMask);	// set user-settable bits
			dp->parent = (DeviceNodePtr)parentNode;
			if(dp->parent->childList == 0){						// first child node
				dp->parent->childList = dp;						// new node is head of child list
				dp->parent->childTail = dp;						// and is at the tail of the list
				dp->peer = dp;									// no other peers
			}
			else {												// add node to others in child list
				dp->peer = dp->parent->childList;
				dp->parent->childTail->peer = dp;
				dp->parent->childTail = dp;
			}
			break;
		
		/*
		 * create a new property node
		 */
		case(kPropertyNode):
			if((attrib & kTypeMask) == kDeviceNode)
			 return(-1);										// can't attach dev node to property
			if((nameLen = CheckPropertyName(name)) == 0)
				return(-1);										// something wrong with name
			pp = (PropertyNodePtr)AllocTreeMem(sizeof(PropertyNode));
			retVal = (NodeID)pp;
			dst = pp->name;
			pp->attrib = attrib & (kTypeMask | kAttribMask);	// set user-settable bits
			pp->parent = (DeviceNodePtr)parentNode;
			if(pp->parent->propertyList == 0){					// first property node
			 pp->parent->propertyList = pp;
			 pp->parent->propertyTail = pp;
			 pp->peer = pp;
			}
			else {												// add node to others in property list
			 pp->peer = pp->parent->propertyList;
			 pp->parent->propertyTail->peer = pp;
			 pp->parent->propertyTail = pp;
			}
			break;
			
		default:
			return(-1);
	}
	
	/*
	 * copy name into the node
	 */
	while(nameLen){
	 *dst++ = *src++;
	 nameLen--;
	}
	*dst = '\0';
	return(retVal);
}

/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* DevTreeDelNode()                                                       */
/*                                                                        */
/* Delete a node, and child nodes from the Device Tree                    */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
pascal OSErr DevTreeDelNode(theNode)
NodeID theNode;
{
	register DeviceNodePtr dp, chldp, parentp, peerp;
	register PropertyNodePtr pp, prevprop;
	register OSErr status;

	if(!CheckNodeID(theNode))
	 return(-1);												// make sure node exists
	switch(((DeviceNodePtr)theNode)->attrib & kTypeMask) {
	
		/*
		 * Delete a device tree node, its property nodes
		 * and its decendents.
		 * ..checking to make sure it's not locked
		 *
		 * NOTE: locks on any child nodes are cheerfully ignored!
		 *
		 */
		case(kDeviceNode):
		
			dp = (DeviceNodePtr)theNode;
			if(dp->attrib & kAttribLocked)
				return(-1);
			/*
			 * start recursive deletion for all child nodes
			 */
			while(dp->childList){							// while there are child nodes
			  chldp = dp->childList;
			  peerp = chldp->peer;
			  status = DevTreeDelNode((NodeID)chldp);		// recurse down one branch
			}
			
			/*
			 * Free all the property nodes (if any attached)
			 * Break condition is when the node you've just deleted
			 * is the tail node.
			 */
			while(dp->propertyList){
			 status = DevTreeDelNode((NodeID)dp->propertyList);
			}
			
			/*
			 * If we're at the bottom of the recursion,
			 * free memory used for the device node,
			 * and return. Note that if you try to delete the
			 * root node, it deletes all of the root node's
			 * properties and children, but not the node itself
			 *
			 */
			if(dp->parent == 0)
			 return(noErr);

			parentp = dp->parent;
			
			if(parentp->childList == dp){			// delete child at list head
			 if(parentp->childTail  == dp){			// only one node in list
			  parentp->childList = 0;
			  parentp->childTail = 0;
			 }
			 else{
			  parentp->childTail->peer = dp->peer;
			  parentp->childList = dp->peer;
			 }
			}
			else{
			 chldp = parentp->childList;
			 while(chldp->peer != dp){
			  chldp = chldp->peer;
			 }
			 /*
			  * chldp points to prev node, so squeeze out
			  * the node we're deleting, updating childTail too
			  */
			 if(parentp->childTail == dp){
			  parentp->childTail = chldp;
			 }
			 chldp->peer = dp->peer;
			}
			
			if(dp->childList == 0){					// if we have no children
			 FreeTreeMem((Ptr)dp);					// delete ourselves..
			}
			break;
		
		/*
		 * Delete a property node from a device, making sure
		 * it's not locked. Backing storage is released for properties
		 * with NVRAM and SYSSAVED attributes
		 * (take care of this in FreeTreeMem??)
		 */
		case(kPropertyNode):
			pp = (PropertyNodePtr)theNode;
			if(pp->attrib & kAttribLocked){
			 return(-1);
			}
			/*
			 * delete property specified by theNode
			 */
			dp = pp->parent;
			if(dp->propertyList == pp){				// node to del at head
			 if(dp->propertyTail == pp){
			  dp->propertyList = 0;					// only one property
			  dp->propertyTail = 0;
			  if(pp->size)
			   FreePropData(pp);					// free property data
			  FreeTreeMem((Ptr)pp);
			  break;
			 }
			 else{
			  dp->propertyTail->peer = pp->peer;	// move back-link in tail node
			  dp->propertyList = pp->peer;			// move list head down one
			  if(pp->size)
			   FreePropData(pp);					// free property data
			  FreeTreeMem((Ptr)pp);
			  break;
			 }
			}
			else{
			 /*
			  * since there are no back pointers, spin around the list to
			  * find the node just before us
			  */
			 prevprop = pp;
			 pp = pp->peer;
			 while(pp != (PropertyNodePtr)theNode){
			   prevprop = pp;
			   pp = pp->peer;
			 }
			 /*
			  * prevprop now points to the node before us in the list.
			  * Squish out the node we want to delete. If the node being
			  * deleted is the last node in the list, make sure the tail
			  * pointer is backed up one.
			  */
			 if(dp->propertyTail == pp)
			  dp->propertyTail = prevprop;
			 prevprop->peer = pp->peer;

			 /*
			  * NOTE: only works for direct property data
			  */
			 if(pp->size)
			   FreePropData(pp);					// free property data
			   
			 FreeTreeMem((Ptr)pp);
			}
			break;
			
		default:									// all other node types fail
			return(-1);
	}
	
	return(noErr);
}

/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* DevTreeSetProperty()                                                   */
/*                                                                        */
/* Change the value of a property                                         */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
pascal OSErr DevTreeSetProperty(theNode, buf, size)
NodeID theNode;
unsigned char *buf;
long size;
{
	PropertyNodePtr pp;
	
	switch(((DeviceNodePtr)theNode)->attrib & kTypeMask) {
	
		case(kDeviceNode):
			return(-1);									// dev nodes have no value attached
			
		case(kPropertyNode):
			pp = (PropertyNodePtr)theNode;
			if(pp->attrib & kAttribROMProp)
			 return(-1);								// can't change a ROM property
			if(pp->attrib & kAttribDefProp)
			 return(-1);								// can't set a Defered property (for now)
			if(pp->size){								// already has a prop?
			 FreeTreeMem((Ptr)pp->dataPtr); 			// replace with new value
			}
			 pp->dataPtr = (unsigned char *)AllocTreeMem(size);
			 pp->size = size;
			 BlockMove(buf, pp->dataPtr, size);
			break;
			
		default:
			return(-1);
	}
	
	return(noErr);
}

/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* DevTreeGetProperty()                                                   */
/*                                                                        */
/* Get the value of a property                                            */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
pascal OSErr DevTreeGetProperty(theNode, buf, size)
NodeID theNode;
unsigned char *buf;
long size;
{
	PropertyNodePtr pp;
	
	switch(((DeviceNodePtr)theNode)->attrib & kTypeMask) {
	
		case(kDeviceNode):
			return(-1);									// dev nodes have no value attached
			
		case(kPropertyNode):
			pp = (PropertyNodePtr)theNode;
			if(pp->attrib & kAttribDefProp)				// no defered properties yet
			 return(-1);
			if((pp->size == 0)||(pp->dataPtr == 0))
			 return(-1);								// node has no property value
			if(pp->size < size)
			 size = pp->size;							// return only as much as is there
			BlockMove(pp->dataPtr, buf, size);
			break;
			
		default:
			return(-1);
	}
	
	return(noErr);
}
