/*
	File:		NuBus_BusMgr.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	11/11/93	fau		first checked in

*/

/*
 * File:		NuBus_BusMgr.c
 *
 * Contains:	NuBus Bus Manager
 *
 * Written by:	Al Kossow
 *
 * Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.
 *
 * Change History (most recent first):
 *
 * <1>			10/20/93	aek		Created
 *
 * All of the stub routines required for backwards compatibilty with the
 * Slot Manager for systems without a NuBus.
 *
 * These routines field all the Nubus slot manager requests, forwarding
 * them to code installed dynamically for xxBus <-> NuBus bridges.
 *
 * Any 'Slot Manager Compatibility Layer' routines are here as well.
 *
 * -------------------------------------------------------------------------
 *
 * The NuBus slot manager provides calls to manipulate slot parameter ram, the slot resource table,
 * and fetching of information from a configuration ROM on an expansion card. It also scans slots
 * at system initialization time, finding config ROMs and setting up the appropriate information.
 *
 *
 * Here's the selector list as of Oct, 1993...
 *
 *                             Sel# 
 * SReadByte()				// [00]
 * SReadWord()				// [01]
 * SreadLong()				// [02]
 * SGetCString()			// [03] Copy a cString into a buffer
 * -----	    		***	// [04]
 * SGetBlock()				// [05] Copy an sBlock into a new block
 * SFindStruct()			// [06] Return pointer to data structure
 * SReadStruct()			// [07] Copy a data structure to a new block
 * SVersion()				// [08] Return the version number of the Slot Manager
 * SetSRsrcState()			// [09] Enables or disables an sResource data structure
 * InsertSRTRec()			// [10] Adds an sResource data structure to the Slot Resource Table
 * SGetSRsrc()				// [11] Return information about the sResource data structure
 * SGetTypeSRsrc()			// [12] Returns information about the matching sResource
 * -----                *** // [13]
 * -----				***	// [14]
 * DoSecondaryInit()		// [15]
 *
 * SReadInfo()				// [16] Read a data structure into a new block
 * SReadPRAMRec()			// [17] Copy the sPRAM data structure into a new block
 * SPutPRAMRec()			// [18] Copy data from spsPointer's block into SPRAMRecord
 * SReadFHeader()			// [19] 
 * SNextSRsrc()				// [20] Determine the set of all of a slot cards's or NuBus sResources <<<<
 * SNextTypeSRsrc()			// [21] Return information about sResources of a specified type
 * SlotRsrcInfo				// [22]
 * SlotDisposPtr			// [23]
 * SCkCardStat()			// [24] Check the SInfoRecord's InitStatusA field
 * SReadDrvrName()			// [25] Read the name of the sResource for this slot and list ID
 * FindSRTRec				// [26]
 * SFindDevBase				// [27] Return a pointer to the base of the device
 * FindDevBase				// [28]
 * GetSRsrcPtr				// [29]
 * StubbAddCard			**	// [30]
 * StubRemoveCard		**	// [31]
 *
 * InitSDeclMgr()			// [32] Initialize the Slot Manager (only executable at sys init time)
 * SPrimaryInit()			// [33] Call card primary init routines (only executable at sys init time)
 * SCardChanged()			// [34] Indicate a card has been changed
 * SExec()					// [35] Load and execute an sExec code block
 * SOffsetData()			// [36] Return contents of offset/data field
 * SInitPRAMRecs()			// [37] Initialize slot PRAM records (only executable at sys init time)
 * SReadPBSize()			// [38] Read the size of the indicated sBlock
 * StubCheckSlot		**	// [39]
 * SCalcStep()				// [40] Find the field sizes in the indicated block
 * SInitSRsrcTable()		// [41] Initialize the Slot Resource Table (only executable at sys init time)
 * SSearchSRT()				// [42] Find the record corresponding to this sResource
 * SUpdateSRT()				// [43] Update the Slot Resource Table
 * SCalcSPointer()			// [44] Return pointer to a byte in declaration ROM
 * SGetDriver()				// [45] Load driver corresponding to sResource
 * SPtrToSlot()				// [46] Return the slot number of the card
 * SFindSInfoRecPtr()		// [47] Return pointer to the sInfoRecord
 * SFindSRsrcPtr()			// [48] Return pointer to sRsrc list
 * SDeleteSRTrec()			// [49] Delete and sResource
 * Secondary_Init()			// [50]
 * InitSlotPRAM()			// [51]
 *
 * pNewSRTEntry				// [52]
 * pInitEntry				// [53]
 * pSrToSpBlock				// [54]
 * pRead4Bytes				// [55]
 * pGetBoard				// [56]
 * pBusException			// [57]
 * pInstallBus				// [58]
 * pRestoreBus				// [59]
 * pAllocSRTBlk				// [60]
 * GetSRTEntry				// [61]
 *
 */
 
 #include "ExpansionMgrInternal.h"
 
 /*
  *
  */
