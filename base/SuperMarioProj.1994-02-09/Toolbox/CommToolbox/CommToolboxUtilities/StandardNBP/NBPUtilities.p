{
	File:		NBPUtilities.p

	Contains:	NBP Utility Routines

	Written by:	Byron Han

	Copyright:	© 1987-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <5>	 1/25/91	stb		JDR: rename LookupName to avoid conflict with AppleTalk.p
		 <4>	 11/6/90	kaz		Adding IntlUtilsPriv USES file for typeAhead. <jng>
		<1+>	 3/20/90	kaz		Merging code shrink/cleqanup from 1.0
									- use WITH
									- use PCopyString instead of aStr:= bStr
		 <1>	 3/14/90	BBH		first checked in

	To Do:
}

UNIT NBPUtilities;

INTERFACE
USES
	MemTypes, QuickDraw, OSIntf, ToolIntf, PackIntf,
	Appletalk,
	CTBUtilities,
	CommToolboxPriv,
{$IFC NOT PRE70}
	IntlUtilsPriv,		{ type ahead	}
{$ENDC}
	StandardNBPStructures,
	ListUtilities;
					
FUNCTION __LookupName(theData : DataH;  item : integer; VAR found : INTEGER; count : Byte; async : BOOLEAN) : OSErr;

FUNCTION ItemsToList(theData : DataH; item : integer) : OSErr;
					 
IMPLEMENTATION

FUNCTION CallFilter(filterProc: ProcPtr; entity: EntityName): INTEGER; EXTERNAL;

{---------------------------------------------------------------	
	__LookupName	- Does the Appletalk name lookup
	
		theData		- NuLookup data -- assumed LOCKED
		item		- which item to look for
		found		- returns number of entities found
		count		- # of retries
		async		- pay now or pay later
		
		returns		- any OSErr
----------------------------------------------------------------	}

FUNCTION __LookupName(theData : DataH;  item : integer; VAR found : INTEGER; 
												count : Byte; async : BOOLEAN) : OSErr;
BEGIN	
	WITH theData^^ DO BEGIN
		NBPSetEntity(thePBptr^.entityPtr, '=', 
						types^[item].typeStr, theRPtr^.theEntity.zoneStr);
		thePBPtr^.count := count;
		
		__LookupName := PLookupName(thePBptr, async);
	
		found := thePBptr^.numGotten;
	END;
END;


{---------------------------------------------------------------	
	ItemsToList		- Adds an entity to a list, but first gives
						the caller a chance to filter it
	
		theData		- NuLookup data -- assumed LOCKED
		item		- which item to add
----------------------------------------------------------------	}

FUNCTION ItemsToList(theData : DataH; item : integer) : OSErr;
CONST
	kSyncRetry	= 2;
VAR
	result 		: OSErr;
	theList 	: ListHandle;
	found 		: INTEGER;
	i 			: INTEGER;
	theEntity 	: EntityName;
	theAddr 	: AddrBlock;
	myItem 		: ListItem;
	addit 		: Boolean;
	include 	: Integer;
	tempRect	: Rect;
	
BEGIN
	WITH theData^^ DO BEGIN
		theList := itemList;
			
		thePBptr^.interval := 2;						{Set up for the synchronous call}
			
		result := __LookupName(theData, item, found, kSyncRetry, FALSE);
		
		thePBptr^.interval := parms^^.retryInterval;	{Set back for the asynchronous call}
		
		IF result = noErr THEN
			FOR i := 1 TO found DO BEGIN
	
				result := NBPExtract(thePBptr^.retBuffPtr, found, i, theEntity, theAddr);
				IF result = noErr THEN
					WITH myItem DO BEGIN
						{ We fill out myItem info				}
						PCopyString(theEntity.objStr,objStr);
						PCopyString(theEntity.typeStr,typeStr);
						
						addr := theAddr;
						dimmed := false;
		
						addit := (nameFilter = nil);
						
						{ Give the caller a chance to filter this item	}
						IF NOT addit THEN BEGIN
							PCopyString(theRPtr^.theEntity.zoneStr,theEntity.zoneStr);
							
							include := CallFilter(nameFilter,theEntity);
							addit := (include < nameReject);
							dimmed := (include = nameDisable);
						END;
						
						IF addit THEN BEGIN					
							age := parms^^.ageLimit;
							hIcon := types^[item].hIcon;
							AddSortedToList(myItem,theList);
						END;
						
					END; { with myItem	}
					
			END; { for loop	}
				
		tempRect := theList^^.rView;
		InvalRect(tempRect);
		
	END; { with theData^^ }
	
	ItemsToList := result;
END;


END.