/*
	File:		Zone24.h

	Contains:	Information needed by 24-bit heap management routines.  Mode non-specific
				information is in Zone.h.  32-bit heap information is in Zone32.h.

	Written by:	David Harrison

	Copyright:	© 1989-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <0>	 x/xx/89	DFH		New Today.

*/ 

/************************************************************************************
 * 					Structure definition for heap block header
 ************************************************************************************/

typedef struct HeapBlock
	{
	unsigned int	tagBC24		: 4;		/* tag field (reloc/nonreloc/free) */
	unsigned int	SizeCor24	: 4;		/* size correction */
	unsigned int	blkSize24	: 24;		/* physical block size */
	Handle			handle24;				/* handle24 depends on block type, as follows:
											 * non-relocatable: address of zone
											 * relocatable: offset from zone to master pointer
											 * free: undefined
											 */
	long			blkData24[0]; 			/* data starts here */
	} HeapBlock, *HeapBlockPtr;

/************************************************************************************
 * Macros for basic block header field access.
 * NOTE: Header access could be made faster/small by special-casing combinations of the
 * fields in the first long word.  I opted not to do this, though, so that the code in
 * MemoryMgr24Patches.c and MemoryMgr32Patches.c matched as closely as possible.
 ************************************************************************************/
 
/* Characteristics */
#define SIZEOF_BLOCK_HEADER					(8)
#define MIN_PHYSICAL_SIZE					(SIZEOF_BLOCK_HEADER + MIN_LOGICAL_SIZE)

/* Field extraction and evaluation */
#define TAGBYTE(heapBlockPtr)				((heapBlockPtr)->tagBC24)
#define	freeTagVal				0x0	
#define	nRelTagVal				0x4	
#define	relTagVal				0x8	
#define SIZE_CORR(heapBlockPtr)				((heapBlockPtr)->SizeCor24)
#define BLOCK_SIZE(heapBlockPtr)			((heapBlockPtr)->blkSize24)
#define BLOCK_HANDLE(heapBlockPtr)			((heapBlockPtr)->handle24)
#define BLOCK_DATA(heapBlockPtr)			((heapBlockPtr)->blkData24)
#define MP_TAGBYTE(heapBlockPtr,theZone)	(*((Ptr)(BLOCK_HANDLE(heapBlockPtr))+(unsigned long)(theZone)))
#define resourceMPTagFlag		0x20
#define purgeableMPTagFlag		0x40
#define lockedMPTagFlag			0x80

/************************************************************************************
 * Compound macros for block header field access.
 ************************************************************************************/

#define CLEAR_BLOCK_ATTRS(heapBlockPtr)		(*((char *) (heapBlockPtr)) = 0)
#define IS_FREE_BLOCK(heapBlockPtr)			(TAGBYTE(heapBlockPtr) == freeTagVal)
#define IS_NOT_FREE_BLOCK(heapBlockPtr)		(TAGBYTE(heapBlockPtr) != freeTagVal)
#define IS_NRELOC_BLOCK(heapBlockPtr)		(TAGBYTE(heapBlockPtr) == nRelTagVal)
#define IS_RELOC_BLOCK(heapBlockPtr)		(TAGBYTE(heapBlockPtr) == relTagVal)
#define IS_LOCKED_RELOC_BLOCK(heapBlockPtr,zonePtr)		((MP_TAGBYTE(heapBlockPtr,zonePtr) & lockedMPTagFlag) != 0)
