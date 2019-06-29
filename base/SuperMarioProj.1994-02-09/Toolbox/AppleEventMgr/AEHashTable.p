{
	File:		AEHashTable.p

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <2>	  1/7/91	Lai		Fix interface for GetIndexedEntry
		 <1>	12/13/90	Lai		first checked in

	To Do:
}

{*-------------------------------------------------------------------------------*
 |																				 |
 |						  <<< Integrated Environment >>>						 |
 |																				 |
 |				 Copyright Apple Computer, Inc. 1986, 1987, 1988		 		 |
 |							   All rights reserved. 							 |
 |																				 |
 *-------------------------------------------------------------------------------*}

UNIT AEHashTable;
	INTERFACE

Uses
	Types;
	
	CONST

	  { error numbers }

	  ErrAlreadyExists = -1722;			{ fix this constant }
	  ErrNotFound = -1723;				{ fix this constant }
	  ErrEndOfTable = -1724;			{ fix this constant }

	TYPE
		HHand = Handle; 
		HEntryPtr = Ptr;
		KeyPtr = Ptr;
		HashProc = ProcPtr;
		MemProcs = ^MemProcBlock;
		MemProcBlock = RECORD
						DerefProc:		ProcPtr;
						NewProc:		ProcPtr;
						GrowProc:		ProcPtr;
						DisposeProc:	ProcPtr;
					END;

		HashInfo = RECORD
						usedEntries: LONGINT;
						collidedEntries: LONGINT;
						totalEntries: LONGINT;
						tableSize: LONGINT;
					END;


	Function	NewHashTable(NumEntries:LONGINT; KeySize, ValueSize:INTEGER;
					MemHooks:MemProcs; SysHeap:Boolean; VAR Table:HHand): OSErr;
	Function	DisposeHashTable(VAR Hash:HHand; MemHooks:MemProcs): OSErr;
	
	Function	AddKeyEntry(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr; Value:HEntryPtr): OSErr;
	Function	ReplaceEntry(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr; Value:HEntryPtr):OSErr;
	Function	RemoveKeyEntry(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr): OSErr;
	Function	GetKeyValue(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr; Value:HEntryPtr): OSErr;
	Function 	CheckKey(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr): boolean;
	Function	GetIndexedEntry(Hash:HHand; MemHooks:MemProcs; Index:LONGINT; Key:KeyPtr): OSErr;
	
IMPLEMENTATION

	Function	NewHashTable(NumEntries:LONGINT; KeySize, ValueSize:INTEGER;
					MemHooks:MemProcs; SysHeap:Boolean; VAR Table:HHand): OSErr; EXTERNAL;
	Function	DisposeHashTable(VAR Hash:HHand; MemHooks:MemProcs): OSErr; EXTERNAL;
	
	Function	AddKeyEntry(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr; Value:HEntryPtr): OSErr; EXTERNAL;
	Function	ReplaceEntry(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr; Value:HEntryPtr):OSErr; EXTERNAL;
	Function	RemoveKeyEntry(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr): OSErr; EXTERNAL;
	Function	GetKeyValue(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr; Value:HEntryPtr): OSErr; EXTERNAL;
	Function 	CheckKey(Hash:HHand; MemHooks:MemProcs; Key:KeyPtr): boolean; EXTERNAL;
	Function	GetIndexedEntry(Hash:HHand; MemHooks:MemProcs; Index:LONGINT; Key:KeyPtr): OSErr; EXTERNAL;


END.

