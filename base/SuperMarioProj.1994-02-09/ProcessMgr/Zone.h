/*
	File:		Zone.h

	Contains:	Information needed by heap management routines, regardless of whether we
				are in 24-bit or 32-bit zone.  Mode specific information is in Zone24.h
				and Zone32.h.

	Written by:	Phil Goldman

	Copyright:	© 1986-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):
	 
		 <6>	  4/9/91	DFH		VL, #83672 : Added GetProcessMgrHiMaxBlock vector definitions.
		 <5>	  4/7/91	DFH		gbm, WS#JC-PM-005 : Added MIN_PHYSICAL_SIZE_EITHER_MODE.
		 <4>	 3/14/91	DFH		gbm,#82504, #82681, #83168, #83182, #83207: Removed
									GUARANTEED_SYSHEAP_FREE_CONTIGUOUS_SPACE.
		 <3>	 2/21/91	DFH		dba,#82504, #82681, #83168, #83182, #83207: Added
									GetSystemHiFreeBytes vector. need to grow.
		 <0>	 x/xx/86	PYG		New Today.

*/ 
 
/* Macros for heap access */
#define SIZEOF_ZONE_HEADER							(sizeof(Zone) - sizeof(short))
#define SIZEOF_24BIT_BLOCK_HEADER					((unsigned int) 8)
#define SIZEOF_32BIT_BLOCK_HEADER					((unsigned int) 12)
#define HEAP_FREE_SPACE(theZone)					((theZone)->zcbFree)
#define FNoRvrAlloc									(1<<5)
#define ZONE_BYTES_PER_MP 							(8*1024)
#define MIN_PHYSICAL_SIZE_EITHER_MODE				((unsigned int) 16)
#define MIN_LOGICAL_SIZE							((unsigned int) 4)
#define SIZEOF_SMALLEST_PTR_DATUM					MIN_LOGICAL_SIZE
#define MAX_LOGICAL_SIZE							(0xFFFFFFF0)
#define MINGROWTH									((unsigned int) 16)
#define	MAX_TOO_FEW_SYS_MPS							((unsigned int) 32)
#define EXTRA_SLOP_FOR_NEXT_TIME					(10*1024)

/* maxSize that works both in 24-bit and 32-bit modes
 * NOTE: This must not be too big, since the memory manager automatically
 * adds the block header size to this value, which would cause it to wrap
 * to a small positive number.
 */
#define compactAll		0xFFFFFF00

/* Typedef that zone files can share */
typedef unsigned long		FreeBlockAddr;			/* address of free block */

/* Function prototypes */
typedef pascal long (*GZProcPtr)(long cbNeeded);

pascal long	ProcessMgr_GZProc_Glue(u_size cbNeeded)
extern;

pascal long	ProcessMgrSysGZProc(u_size cbNeeded)
extern;

void	MakeNonRelocatable24(Ptr *);
void	MakeNonRelocatable32(Ptr *);

/* Function prototypes for vectored routines */
typedef u_size						(*zvr1Ptr) (Handle, Boolean *);
typedef void						(*zvr2Ptr) (THz pZone, u_size cbNeeded);
typedef void						(*zvr3Ptr) (THz oldZone, THz newZone);
typedef long						(*zvr4Ptr) (u_size cbNeeded);
typedef Ptr							(*zvr5Ptr) (Ptr ceilingPtr);
typedef FreeBlockAddr				(*zvr6Ptr) (u_size size, Ptr lockPtr);
typedef long						(*zvr7Ptr) (FreeBlockAddr pTopBlock);
typedef FreeBlockAddr				(*zvr8Ptr) (FreeBlockAddr pBlockHdr, u_size newSize);
typedef FreeBlockAddr				(*zvr9Ptr) (FreeBlockAddr pBlock, Handle *pHandle,u_size sizeNeeded, short sizeCorrection);
typedef Ptr							(*zvr10Ptr) (THz pZone);
typedef u_size						(*zvr11Ptr) (void);
typedef u_size						(*zvr12Ptr) (void);

/* Macros to get vectored routines */
#define GetProcessMgrLoFreeBytes	(*vrGetProcessMgrLoFreeBytes)
#define ExtendZone					(*vrExtendZone)
#define MoveZone					(*vrMoveZone)
#define ShrinkProcessMgrZone		(*vrShrinkProcessMgrZone)
#define GetLockPtr					(*vrGetLockPtr)
#define GetHighestLargeFreeBlock 	(*vrGetHighestLargeFreeBlock)
#define GetTopBlockSize				(*vrGetTopBlockSize)
#define FreeTop						(*vrFreeTop)
#define ChipOffRelocFromFree		(*vrChipOffRelocFromFree)
#define ZoneMPBlockPtr				(*vrZoneMPBlockPtr)
#define GetSystemHiFreeBytes		(*vrGetSystemHiFreeBytes)
#define GetProcessMgrHiMaxBlock		(*vrGetProcessMgrHiMaxBlock)

/* Import the vectors */
extern zvr1Ptr						vrGetProcessMgrLoFreeBytes;
extern zvr2Ptr						vrExtendZone;
extern zvr3Ptr						vrMoveZone;
extern zvr4Ptr						vrShrinkProcessMgrZone;
extern zvr5Ptr						vrGetLockPtr;
extern zvr6Ptr						vrGetHighestLargeFreeBlock;
extern zvr7Ptr						vrGetTopBlockSize;
extern zvr8Ptr						vrFreeTop;
extern zvr9Ptr						vrChipOffRelocFromFree;
extern zvr10Ptr						vrZoneMPBlockPtr;
extern zvr11Ptr						vrGetSystemHiFreeBytes;
extern zvr12Ptr						vrGetProcessMgrHiMaxBlock;

/* Import the non-vectored routines */
extern void							InitVectorTable24(void);
extern void							InitVectorTable32(void);

/* Exports */
void								InitHeaps(void);
pascal long							MyGrowZone(unsigned long);
u_size								ProcessMgrMaxBlock(void);
u_size								StraddleBlockSize(u_size *, u_size *, Handle, Boolean *);
void								ReclaimSpaceIfSysHeapLow(Handle *);
#define LogicalToPhysicalSize(size)	(size) += (In32BitMode) ? SIZEOF_32BIT_BLOCK_HEADER : SIZEOF_24BIT_BLOCK_HEADER;
#define PhysicalToLogicalSize(size)	(size) -= (In32BitMode) ? SIZEOF_32BIT_BLOCK_HEADER : SIZEOF_24BIT_BLOCK_HEADER;
