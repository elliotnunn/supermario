{
	File:		AEHandlerTable.p

	Copyright:	© 1990-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <2>	 5/21/91	gbm		Stop using the "empty" units (MemTypes, etc...)
		 <1>	12/13/90	Lai		first checked in

	To Do:
}

UNIT AEHandlerTable;

INTERFACE

USES
	Types, Memory, Errors,
	AppleEvents, AEHashTable, AEUtil;

{--------------------------------------------------------------------------------}

	{ AppleEvent Manger calls provided by this unit }



	FUNCTION AE_InstallEventHandler(theAEEventClass: AEEventClass;
								   theAEEventID: AEEventID;
								   theHandler: ProcPtr;
								   handlerRefcon: LongInt;
								   isSysHandler: Boolean): OSErr;
	{ Add an AppleEvent Handler }
	
	FUNCTION AE_RemoveEventHandler(theAEEventClass: AEEventClass;
								  theAEEventID: AEEventID;
								  theHandler: ProcPtr;
								  isSysHandler: Boolean): OSErr;
	{ Remove one or more AppleEvent Handler }

	FUNCTION AE_GetEventHandler(theAEEventClass: AEEventClass;
							   theAEEventID: AEEventID;
							   var theHandler: ProcPtr;
							   var handlerRefcon: LongInt;
							   isSysHandler: Boolean): OSErr;
	{ Get the corresponding AppleEvent Handler }

{--------------------------------------------------------------------------------}

	FUNCTION AE_InstallCoercionHandler(fromType: DescType;
									  toType: DescType;
									  theHandler: ProcPtr;
									  handlerRefcon: LongInt;
									  fromTypeAsDesc: Boolean;
									  isSysHandler: Boolean): OSErr;
	{ Add an AppleEvent Handler }
	
	FUNCTION AE_RemoveCoercionHandler(fromType: DescType;
									 toType: DescType;
									 theHandler: ProcPtr;
									 isSysHandler: Boolean): OSErr;
	{ Remove one or more AppleEvent Handler }
	
	FUNCTION AE_GetCoercionHandler (fromType: DescType;
								  toType: DescType;
								  var theHandler: ProcPtr;
								  var handlerRefcon: LongInt;
								  var fromTypeAsDesc: Boolean;
								  isSysHandler: Boolean): OSErr;
	{ Get the corresponding AppleEvent Handler }

{--------------------------------------------------------------------------------}

	FUNCTION AE_InstallSpecialHandler(functionClass:		AEKeyWord;
									 handler:			ProcPtr;
									 isSysHandler:		BOOLEAN):OSErr;
	{ Install the special handler named by the KeyWord }
									   
	FUNCTION AE_RemoveSpecialHandler(functionClass:		AEKeyWord;
									handler:			ProcPtr;
									isSysHandler:		BOOLEAN):OSErr;
	{ Remove the special handler named by the Keyword }
									   
	FUNCTION AE_GetSpecialHandler(functionClass:		AEKeyWord;
								 VAR handler:		ProcPtr;
								 isSysHandler:		BOOLEAN):OSErr;
	{ Get the special handler named by the Keyword }


{--------------------------------------------------------------------------------}

	{ utilty routines for other part of the AppleEvents Manager }

	FUNCTION AddToTable(key1, key2: OSType;                 { --> search keys }
						theHandler: ProcPtr;                { --> the handler }
						handlerRefcon: LONGINT;             { --> the refcon }
						isSysHandler: Boolean;              { --> system or application }
						isSpecialHandler: Boolean;          { --> type of handler }
						isEventHandler: Boolean             { --> eventhandler or coercion }
						): OSErr;
	
	FUNCTION RemoveFromTable(key1, key2: OSType;
							 theHandler: ProcPtr;
							 isSysHandler: Boolean;
							 isEventHandler: Boolean
							 ): OSErr;
	
	FUNCTION GetEventHandler(key1, key2: OSType;            { --> key }
							 VAR theHandler: ProcPtr;       { <-- return the procPtr }
							 VAR handlerRefcon: LONGINT;    { <-- return the refcon }
							 VAR isSpecialHandler: Boolean; { <-- is it special handler ? }
							 isSysHandler: Boolean;         { --> application or system ? }
							 isEventHandler: Boolean        { --> event or coerecion ? }
							 ): OSErr;
	
	FUNCTION GetTableInfo(theTable: HHand;         { --> handle to table }
						  key1, key2: OSType;              { --> search these keys }
						  VAR theHandlerRec: HandlerRec    { <-- return this ProcPtr record }
						  ):BOOLEAN;  				{ return true if found }

{--------------------------------------------------------------------------------}

IMPLEMENTATION

{$I AEHandlerTable.inc1.p}

END. { AEHandleTable }