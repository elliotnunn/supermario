{
	File:		ZIPUtilities.p

	Contains:	Zone Info Protocol utility routines

	Written by:	Byron Han
				Jerry Godes

	Copyright:	© 1987-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <5>	 11/6/90	kaz		Adding IntlUtilsPriv USES file for typeAhead. <jng>
		 <4>	 8/15/90	kaz		Put back in NIL checks for _DisposPtr. Need them here for System
									6 too.
		<1+>	 3/20/90	kaz		Merging in code shrink/cleanup from 1.0
									- Changed ZonesExist() to return the bridge, net, 
										and OSErr
	   								- Moved common ParamBlock futz code from GetZones() and
										GetZoneName() into DoZoneInfo().
									- Direct string access instead of x = '', etc
	   								- Checked NewPtrClear for unnecessary zero-initialization
									- Removed checks for NIL on DisposPtr
		 <1>	 3/14/90	BBH		first checked in

	To Do:
		• Use WITH
}

UNIT ZIPUtilities;

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
	
FUNCTION GetZoneName(VAR aZone : STR32) : OSErr;
FUNCTION GetZones(zonesPtr : Ptr; index : INTEGER; VAR found : INTEGER;
														VAR more : BOOLEAN) : OSErr;
FUNCTION ZonesToList(theData : DataH) : OSErr;
FUNCTION ZonesExist(VAR bridge: INTEGER; VAR net: INTEGER; VAR err: OSErr): BOOLEAN;

IMPLEMENTATION

CONST
	standardInterval 	= 3;
	standardRetry 		= 5;
	
	kGetZonesCmd		= $08000000;		{ Param Block User Data		}
	kGetMyZoneCmd		= $07000000;
	
TYPE
	BDSElementPtr = ^BDSElement;

FUNCTION CallFilter(filterProc: ProcPtr; zoneStr: Str32): INTEGER; EXTERNAL;


{-----------------------------------------------------------------
	ZonesExist	-	Checks to see if we have zones and returns
					the net and bridge values
			
		net		-	The info we need for paramBlocks
		bridge
		
		returns	-	true if there be zones
-------------------------------------------------------------------	}

FUNCTION ZonesExist(VAR bridge: INTEGER; VAR net: INTEGER; VAR err: OSErr): BOOLEAN;
VAR
	node : INTEGER;
	result : OSErr;
	
BEGIN
	bridge := GetBridgeAddress;
	err := GetNodeAddress(node, net);
	
	IF (bridge = 0) OR (net = 0) THEN
		ZonesExist := FALSE
	ELSE
		ZonesExist := TRUE;
END;


{ ---------------------------------------------------------------------------
	DoZoneInfo	-	Performs the nitty gritty allocation and filling
					out of the ATP param block.  Nice high level 
					interface, guys…
					
		userData	-	Commang msg to pass
		buffSize	-	Size of the buffer
		buffPtr		-	Where to stuff the info
		found		-	# of Zones found
		more		-	Are there more to be gotten
		
		returns		-	Ye Olde OSErr
-----------------------------------------------------------------------------	}

FUNCTION DoZoneInfo(theUserData: LONGINT; theBuffSize: INTEGER; theBuffPtr: Ptr; 
								VAR found : INTEGER; VAR more : BOOLEAN) : OSErr;

	
VAR
	bridge 		: INTEGER;
	net 		: INTEGER;
	thePBptr 	: ATPpbPtr;
	result 		: OSErr;
	theBDSPtr 	: BDSElementPtr;

LABEL
	1;										{	common exit code				}
	
BEGIN
	result := noErr;
	
	more := FALSE;							{	pessimism						}
	found := 0;
	thePBptr := NIL;						{	for common exit code			}
											{ bridge, net, and result are returned	}
	IF ZonesExist(bridge, net, result) THEN BEGIN
		
		{ Allocate and fill out the param block	}
		thePBptr := ATPpbPtr(NewPtrClear(SIZEOF(ATPParamBlock)));
		result := MemError;
		IF result <> noErr THEN
			GOTO 1;
			
		WITH thePBptr^ DO BEGIN
			userData := theUserData; 		{ Zone command		}
			{ atpFlags := 0; set by NewPtrClear	}
			
			WITH addrBlock DO BEGIN
				aNet := net;				{ From ZonesExist()	}
				aNode := bridge;
				aSocket := $06; 			{ ZIP socket		}
			END;
			
			{ reqLength := 0; request is zero length.  Set by NewPtrClear 		}
			{ reqPointer := NIL; pointer to request data. Set by NewPtrClear 	}
			
			bdsPointer := NewPtrClear(SIZEOF(BDSElement));
			result := MemError;
			IF result <> noErr THEN
				GOTO 1;
				
			theBDSPtr := BDSElementPtr(bdsPointer);
			theBDSPtr^.buffSize := theBuffSize;
			theBDSPtr^.buffPtr := theBuffPtr;
			
			numOfBuffs := $01;
			timeOutVal := standardInterval;
			retryCount := standardRetry;
		END; {with}
		
		{ gimme data	}
		result := PSendRequest(thePBptr, FALSE);
		IF result <> noErr THEN
			GOTO 1;
			
		theBDSPtr := BDSElementPtr(thePBptr^.bdsPointer);
		
		{ Fill out the info asked for		}
		WITH theBDSPtr^ DO BEGIN
			found := BAND(userBytes, $0000FFFF);			{ # found					}
			more := (BAND(userBytes, $FF000000) = 0);		{ there are more to be had	}
		END;
		
	END; { if zones exist }
	
1:
	IF thePBptr <> NIL THEN BEGIN
		IF (thePBptr^.bdsPointer <> NIL) THEN
			DisposPtr(thePBptr^.bdsPointer);	{ DisposPtr does not check for nil			}
		DisposPtr(Ptr(thePBptr));
	END;
	
	DoZoneInfo := result;
END;


{-----------------------------------------------------------------
	ZonesToList	-	Pulls the zone names out of the ParamBlock
					info and add them to the zone list
			
		theData		-	All the junk we need.  No LOCK assumptions
		
		returns		-	OSErr
-------------------------------------------------------------------	}

FUNCTION ZonesToList(theData : DataH) : OSErr;
VAR
	result		: OSErr;
	
	thePtr		: Ptr;						{	zone lookup buffer			}
	tempPtr		: Ptr;						{ 	zoneName extraction pointer }
	i			: INTEGER;
	found 		: INTEGER;
	more		: BOOLEAN;
	index		: INTEGER;
	myItem		: ListItem;	
	include		: INTEGER;
	addit		: Boolean;
	theList		: ListHandle;
	rView		: Rect;						{ local copy so we don't have to lock	}
	
BEGIN
	theList := theData^^.zoneList;
	result := noErr;						{	optimism					}
	LDoDraw(FALSE, theList); 				{	drawing off					}
	LDelRow(0, 0, theList); 				{	clear the list				}
	
	WITH myItem DO BEGIN
		typeStr[0] := CHR(0);			{	Fill in constant fields		}
		hIcon := NIL;
	END;
	
	index := 578;											{ size for the zone lookup buffer 
																(max size of ATP response packet) 	}
	thePtr := NewPtr(index);								{	go for gold…						}
	result := MemError;										{ get error codes						}
	WHILE (index >= SIZEOF(Str32) ) AND (thePtr = NIL) DO	{ leave space for at least one response }
	BEGIN													{	try silver, bronze, iron……………		}
		index := index DIV 2;
		IF index < SIZEOF(Str32) THEN
			index := SIZEOF(Str32);
			
		thePtr := NewPtr(index);
		result := MemError;
		IF (thePtr = NIL) AND (index = SIZEOF(Str32)) THEN
			index := 0;										{	bombed out… }
	END;
	
	IF thePtr <> NIL THEN BEGIN
		index := 1;											{	start at first zone					}
		
		REPEAT
			result := GetZones(thePtr, index, found, more);	{	Get the zone list					}
			tempPtr := thePtr;								{	for extraction						}
			
			FOR i := 1 TO found DO BEGIN
				BlockMove(tempPtr,@myItem.objStr,Byte(tempPtr^)+1);
				tempPtr := Ptr(ORD4(tempPtr)+Byte(tempPtr^)+1);
	
				myItem.dimmed := FALSE;
	
				{ Note Bridge tries to eliminate duplicates - but may not actually do so.
					AddSortedtoList does not add duplicates}
	
				addit := (theData^^.zoneFilter = nil);
				IF NOT addit THEN BEGIN
					include := CallFilter(theData^^.zoneFilter,myItem.objStr);
					addit := (include <> zoneReject);
					IF include = zoneDisable THEN
						myItem.dimmed := TRUE;
				END;
				
				IF addit THEN
					AddSortedToList(myItem, theList);		{	This adds unique items				}
			END; {For i}
			
			index := index + found;
		UNTIL ((NOT more) OR (result <> noErr));
		
		DisposPtr(thePtr);
	END; {thePtr <> NIL}
	
	LDoDraw(TRUE, theList);									{	turn drawing back on				}
	
	rView := theList^^.rView;								{ cause redraw to occur	}
	InvalRect(rView);										{ moves memory			}
	
	zonesToList := result;									{	and return error code				}
END;



{-----------------------------------------------------------------
	GetZones	-	Asks for the zones and returns # found,
					and if there are more than we can fit in
					the buffer.
			
		zonesPtr	-	Where to stuff all that info
		index		-	What zone # we're on
		found		-	# of zones found
		more		-	Are there more to be had
		
		returns		-	OSErr
-------------------------------------------------------------------	}

FUNCTION GetZones(zonesPtr : Ptr; index : INTEGER;
					VAR found : INTEGER; VAR more : BOOLEAN) : OSErr;
BEGIN
	GetZones := DoZoneInfo(kGetZonesCmd + index, GetPtrSize(zonesPtr), zonesPtr, found, more);
END;


{-----------------------------------------------------------------
	GetZoneName		-	Gets the home zone name
			
		aZone		-	Returns the string
		
		returns		-	OSErr
-------------------------------------------------------------------	}

FUNCTION GetZoneName(VAR aZone : STR32) : OSErr;	
VAR	
	found	: INTEGER;				{ dummy vars: we don't care			}
	more	: Boolean;	
	
BEGIN
	GetZoneName := DoZoneInfo(kGetMyZoneCmd, SIZEOF(aZone), Ptr(@aZone), found, more);
END;


END.