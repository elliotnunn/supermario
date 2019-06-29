{
	File:		AEHandlerTable.inc1.p

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <3>	 1/11/91	Lai		Revert change 2 now that NewHashTable has been fixed
		 <2>	12/18/90	Lai		Chagne initial size of hash table from 8 to 7 to get around bug
									in NewHashTable
		 <1>	12/13/90	Lai		first checked in

	To Do:
}

{ Copyright © 1984-1990 by Apple Computer Inc. All rights reserved. }

{--------------------------------------------------------------------------------}

FUNCTION AE_InstallEventHandler(theAEEventClass: AEEventClass;
                                theAEEventID: AEEventID;
                                theHandler: ProcPtr;
                                handlerRefcon: LONGINT;
                                isSysHandler: Boolean): OSErr;
    EXTERNAL;

FUNCTION AE_RemoveEventHandler(theAEEventClass: AEEventClass;
                               theAEEventID: AEEventID;
                               theHandler: ProcPtr;
                               isSysHandler: Boolean): OSErr;
    EXTERNAL;

FUNCTION AE_InstallCoercionHandler(fromType: DescType;
                                   toType: DescType;
                                   theHandler: ProcPtr;
                                   handlerRefcon: LONGINT;
                                   fromTypeAsDesc: Boolean;
                                   isSysHandler: Boolean): OSErr;
    EXTERNAL;

FUNCTION AE_RemoveCoercionHandler(fromType: DescType;
                                  toType: DescType;
                                  theHandler: ProcPtr;
                                  isSysHandler: Boolean): OSErr;
    EXTERNAL;

FUNCTION AE_GetCoercionHandler(fromType: DescType;
                               toType: DescType;
                               VAR theHandler: ProcPtr;
                               VAR handlerRefcon: LONGINT;
                               VAR fromTypeAsDesc: Boolean;
                               isSysHandler: Boolean): OSErr;
    EXTERNAL;

FUNCTION GetSpecialProcOffset(functionClass: AEKeyWord): LONGINT;
    EXTERNAL;

{--------------------------------------------------------------------------------}

FUNCTION GetHashTable(isEventHandler: Boolean;
                      isSysHandler: Boolean;
                      VAR theHashTable: HHand): OSErr; FORWARD;

{--------------------------------------------------------------------------------}

{ add a handler to the table, it may either be event handler or coercion handler }
{ system wide or application specific, and use odd value for special handler }

FUNCTION AddToTable(key1, key2: OSType; { --> search keys }
                    theHandler: ProcPtr; { --> the handler }
                    handlerRefcon: LONGINT; { --> the refcon }
                    isSysHandler: Boolean; { --> system or application }
                    isSpecialHandler: Boolean; { --> type of handler }
                    isEventHandler: Boolean { --> eventhandler or coercion }
                    ): OSErr;

    VAR
        offset: LONGINT;
        err: OSErr;
        theHashTable: HHand;
        theHandlerRec: HandlerRec;

    BEGIN
        IF (theHandler = NIL) OR BTst(ord(theHandler), 0) THEN
            err := paramErr
        ELSE
            BEGIN
            WITH theHandlerRec DO
                BEGIN
                theRefCon := handlerRefcon;
                theProc := theHandler;
                IF isSpecialHandler THEN { special handler is mark as odd }
                    theProc := ProcPtr(ord(theProc) + 1);
                END;
            err := GetHashTable(isEventHandler, isSysHandler, theHashTable); { get the hash table }
            IF (err = noErr) THEN
                BEGIN
                { since on the stack it is refcon, theHandler, key2, key1 }
                { this means we should point to key as the key and refcon as the value }
                { this means that this goes into the table in key2 key 1 order }
                err := ReplaceEntry(theHashTable, NIL, @key2, @theHandlerRec);
                END;
            END;
        AddToTable := err;
    END;

{--------------------------------------------------------------------------------}

FUNCTION AE_GetEventHandler(theAEEventClass: AEEventClass;
                            theAEEventID: AEEventID;
                            VAR theHandler: ProcPtr;
                            VAR handlerRefcon: LONGINT;
                            isSysHandler: Boolean): OSErr;

    VAR
        isSpecialHandler: Boolean;

    BEGIN
        { just get it by calling GetEventHandler }
        { we can ignore the isSpecialHandler flag because we don't use it for now }
        AE_GetEventHandler := GetEventHandler(theAEEventClass, theAEEventID, theHandler,
                                              handlerRefcon, isSpecialHandler, isSysHandler, TRUE);
    END;

{--------------------------------------------------------------------------------}

FUNCTION AE_GetSpecialHandler(functionClass: AEKeyWord;
                              VAR handler: ProcPtr;
                              isSysHandler: BOOLEAN): OSErr;

    VAR
        offset: LONGINT;
        aGlobalRef: GlobalRecHandle;

    BEGIN
		handler := NIL;
        offset := GetSpecialProcOffset(functionClass);
        IF offset > 0 THEN
            BEGIN
            AE_GetSpecialHandler := MustHaveGlobal(isSysHandler, aGlobalRef);
            IF aGlobalRef <> NIL THEN
                handler := ProcPtr(LONGINTPtr(ord(aGlobalRef^) + offset)^);
            END;
        IF handler = NIL THEN
            AE_GetSpecialHandler := errAENotASpecialFunction
		ELSE
            AE_GetSpecialHandler := noErr;
    END;

{--------------------------------------------------------------------------------}

FUNCTION AE_InstallSpecialHandler(functionClass: AEKeyWord;
                                  handler: ProcPtr;
                                  isSysHandler: BOOLEAN): OSErr;

    VAR
        offset: LONGINT;
        aGlobalRef: GlobalRecHandle;
        aLongPtr: LONGINTPtr;
        err: OSErr;

    BEGIN
        offset := GetSpecialProcOffset(functionClass); { get the offset }
        IF offset > 0 THEN
            BEGIN
            { must have global intialized before installing }
            err := MustHaveGlobal(isSysHandler, aGlobalRef);
            IF err = noErr THEN
                BEGIN
                IF (handler = NIL) OR BTst(ord(handler), 0) THEN
                    err := paramErr
                ELSE
                    BEGIN
                    aLongPtr := LONGINTPtr(ord(aGlobalRef^) + offset);
                    aLongPtr^ := ord(handler);
                    END;
                END;
            END
        ELSE
            err := errAENotASpecialFunction;
        AE_InstallSpecialHandler := err;
    END;

{--------------------------------------------------------------------------------}

FUNCTION AE_RemoveSpecialHandler(functionClass: AEKeyWord;
                                 handler: ProcPtr;
                                 isSysHandler: BOOLEAN): OSErr;

    VAR
        offset: LONGINT;
        aGlobalRef: GlobalRecHandle;
        aLongPtr: LONGINTPtr;

    BEGIN
        offset := GetSpecialProcOffset(functionClass);
        IF offset > 0 THEN
            BEGIN
            AE_RemoveSpecialHandler := MustHaveGlobal(isSysHandler, aGlobalRef);
            IF aGlobalRef <> NIL THEN
                BEGIN
                aLongPtr := LONGINTPtr(ord(aGlobalRef^) + offset);
                IF (aLongPtr^ = ord(handler)) OR (ord(handler) = 0) THEN aLongPtr^ := 0;
                END;
            END
        ELSE
            AE_RemoveSpecialHandler := errAENotASpecialFunction;
    END;

{--------------------------------------------------------------------------------}

FUNCTION GetEventHandler(key1, key2: OSType; { --> key }
                         VAR theHandler: ProcPtr; { <-- return the procPtr }
                         VAR handlerRefcon: LONGINT; { <-- return the refcon }
                         VAR isSpecialHandler: Boolean; { <-- is it special handler ? }
                         isSysHandler: Boolean; { --> application or system ? }
                         isEventHandler: Boolean { --> event or coerecion ? }
                         ): OSErr;

    VAR
        theHashTable: HHand;
        theHandlerRec: HandlerRec;

    BEGIN
        GetEventHandler := errAEHandlerNotFound;
        IF GetHashTable(isEventHandler, isSysHandler, theHashTable) = noErr THEN { get the hash
               table }
            BEGIN { no need to look unless global is initialized }
            IF GetKeyValue(theHashTable, NIL, @key2, @theHandlerRec) = NoErr THEN
                BEGIN
                WITH theHandlerRec DO
                    BEGIN
                    theHandler := theProc;
                    handlerRefcon := theRefCon;
                    IF BTst(ord(theHandler), 0) THEN
                        BEGIN { odd means special handler }
                        theHandler := ProcPtr(ord(theHandler) - 1);
                        isSpecialHandler := TRUE;
                        END
                    ELSE
                        isSpecialHandler := FALSE;
                    GetEventHandler := noErr;
                    END;
                END;
            END;
    END;

{--------------------------------------------------------------------------------}

{ get the particular hash table }

FUNCTION GetHashTable(isEventHandler: Boolean;
                      isSysHandler: Boolean;
                      VAR theHashTable: HHand): OSErr;

    VAR
        err: OSErr;
        aGlobalRef: GlobalRecHandle;

    BEGIN
        err := MustHaveGlobal(isSysHandler, aGlobalRef); { get the global }
        IF (err = noErr) THEN
			BEGIN
            WITH aGlobalRef^^ DO
                BEGIN { get different table for different handler }
                IF isEventHandler THEN
                    theHashTable := aeHashTable
                ELSE
                    theHashTable := coercionHashTable;
				END;
			IF theHashTable = NIL THEN
				BEGIN
				err := NewHashTable(8, SizeOf(KeyRec), SizeOf(handlerRec), NIL, isSysHandler,
									theHashTable);
				IF err = noERR THEN
					WITH aGlobalRef^^ DO { we must dereference again }
						BEGIN
						IF isEventHandler THEN
							aeHashTable := theHashTable
						ELSE
							coercionHashTable := theHashTable;
						END;
				END;
			END;
        GetHashTable := err;
    END;

{--------------------------------------------------------------------------------}

{ find the handler and refcon for the corresponding key, wildcard is used }

FUNCTION GetTableInfo(theTable: HHand; { --> handle to table }
                      key1, key2: OSType; { --> search these keys }
                      VAR theHandlerRec: HandlerRec { <-- return this ProcPtr record }
                      ): BOOLEAN; { return true if found }

    VAR
        theKeyRec: KeyRec;

    BEGIN
        GetTableInfo := TRUE;
        WITH theKeyRec DO
            BEGIN
            firstKey := key1;
            secondKey := key2;
            END;
        IF GetKeyValue(theTable, NIL, @theKeyRec, @theHandlerRec) <> NoErr THEN
            BEGIN
            theKeyRec.firstKey := typeWildCard;
            IF GetKeyValue(theTable, NIL, @theKeyRec, @theHandlerRec) <> NoErr THEN
                BEGIN
                WITH theKeyRec DO
                    BEGIN
                    firstKey := key1;
                    secondKey := typeWildCard;
                    END;
                IF GetKeyValue(theTable, NIL, @theKeyRec, @theHandlerRec) <> NoErr THEN
                    BEGIN
                    theKeyRec.firstKey := typeWildCard;
                    IF GetKeyValue(theTable, NIL, @theKeyRec, @theHandlerRec) <> NoErr THEN
                        GetTableInfo := FALSE;
                    END;
                END;
            END;
    END;

{--------------------------------------------------------------------------------}
{ remove an entry from the table }

FUNCTION RemoveFromTable(key1, key2: OSType;
                         theHandler: ProcPtr;
                         isSysHandler: Boolean;
                         isEventHandler: Boolean): OSErr;

    VAR
        curHandler: ProcPtr;
        refcon: LONGINT;
        dummy: Boolean;
        err: OSErr;
        theHashTable: HHand;

    BEGIN
        err := errAEHandlerNotFound;
        IF GetHashTable(isEventHandler, isSysHandler, theHashTable) = noErr THEN { get the hash
               table }
            BEGIN { no need to look unless global is initialized }
            err := GetEventHandler(key1, key2, curHandler, refcon, dummy, isSysHandler,
                                   isEventHandler);
            IF err = NoErr THEN
                IF (curHandler = theHandler) OR (theHandler = NIL) THEN
                    err := RemoveKeyEntry(theHashTable, NIL, @key2);
            END;
        RemoveFromTable := err;
    END;

{--------------------------------------------------------------------------------}
