{
	File:		StandardNBPStructures.p

	Contains:	Constants, etc for StandardNBP

	Written by:	Byron Han

	Copyright:	© 1988-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <8>	 1/25/91	stb		JDR: change LookupReply to NBPReply
		 <7>	 11/6/90	kaz		Merged separate fields into an EventRecord; added
									TypeSelectRecord. <jng>
		 <6>	 8/22/90	kaz		Cleaned up parmData stuct.
		 <5>	 5/22/90	kaz		Changed FontInfoRec to TextState
		 <4>	 5/17/90	kaz		Cleaning up
		 <3>	 4/19/90	kaz		Adding activeList for focusable items
		 <2>	 3/16/90	BBH		made it compile
		 <1>	 3/14/90	BBH		first checked in

	To Do:
}

UNIT StandardNBPStructures;
{
	ListItems.p - written by Jerry Godes, Communication Tools Group.
	
	Copyright © 1988, 1989 - Apple Computer, Inc. - All rights reserved.
	
	MODIFICATION HISTORY:
		5/31/89		JNG		Added pre-uppercased items to list.  Added other NuLookup data structures
								to this file so others can get at them
		8/13/89		BBH		Changed ICON to hIcon in ListItem
		8/29/89		BBH		Added SecretListRecord
		10/13/89	BBH		Added Focus for WHEATIES compilerdirective
		2/12/90		kaz		Added isZoning as a flag for delaying a lookup
								if there's typing in the zone list
}

INTERFACE
USES 
	MemTypes,
	QuickDraw,
	ToolIntf,
	OSIntf,
	PackIntf,
	AppleTalk,
	CTBUtilities,
{$IFC NOT PRE70}
	IntlUtilsPriv,		{ type ahead	}
{$ENDC}
	CommToolboxPriv;

CONST
	buttonHiliteDelay	=	8;			{ empirical value for keeping control hilited in response
											to return/enter/command-./esc/or double click
											taken from ModalDialog
										}
	hookFocusItem = hookReserved1;

TYPE
{	used to store information pertinent to the lists
	like font/size to use, like is there a script manager
	like are there icons being used, etc
}
	SecretListHandle = ^SecretListPointer;
	SecretListPointer= ^SecretListRecord;
	SecretListRecord = RECORD
		hasSM		: BOOLEAN;
		hasIcons	: BOOLEAN;
		theFontInfo	: TextState;
		dialog		: DialogPtr;
		itemNo		: INTEGER;
	END;
	
{	structure for each item in the list }
	ListItem 	= RECORD
		dimmed 		: BOOLEAN;
		objStr 		: Str32;
		typeStr 	: Str32;
		age			: INTEGER;
		addr		: AddrBlock;
		hIcon		: HANDLE;
	END;
	
{	format of the parm resource }
	parmH 			= ^parmP;
	parmP 			= ^parmData;
	parmData = RECORD		
		maxLim			: INTEGER;			{ # of items to add to list at a 
												time (smaller is more user responsive, 
												but takes longer to add items to
												itemList
											}
		retryCount 		: INTEGER;			{ # of retries in NBP lookups }
		retryInterval 	: INTEGER;			{ interval for NBP lookups }
		NBPLimit		: INTEGER;			{ max # of NBP entities to look for }
		AgeLimit		: INTEGER;			{ # of consecutive async NBP lookups
												items can miss and still stay
												in the list
											}
	END;
	
	NLTypePtr		= ^NLType;

	DataH	= ^DataP;
	DataP		= ^DataR;
	DataR	=	RECORD
		userData	: LONGINT;				{ Hook to give context to user }
		parms	 	: parmH;				{ NPB Lookup Paramaters }
				
		iLists		: SecretListHandle;		{ data to be transmitted to/from list }
		zLists		: SecretListHandle;		{ for zone list }
		
		nameFilter	: ProcPtr;				{ Users Filter Procedures }
		zoneFilter	: ProcPtr;				
		FilterProc	: ProcPtr;
		HookProc	: ProcPtr;
		
		dialog		: DialogPtr;
		zoneList	: ListHandle;			{ Lists }
		itemList	: ListHandle;
		
		myEvent		: EventRecord;
{$IFC NOT PRE70}
		tsr			: TypeSelectRecord;		{ for type ahead	}
{$ENDC}
		myBufferSize: INTEGER;				{ NBP Structures and NBP Status Info}
		theBuffer	: Handle;				{ where the NBP junk goes }
	
		thePBPtr	: MPPpbPtr;
		CurrentNBP	: INTEGER;				{ What lookup is outstanding - offset into types }
		LastIChecked: INTEGER;				{ Last Item the checkProcess dealt with }
		types		: NLTypePtr;
		
		fake		: BOOLEAN;				{ did we need to fake a type list (-1 was passed in ) }
		
		numTypes	: INTEGER;
		
		zonable 	: BOOLEAN;				{ are able to see zones }
		itemable 	: BOOLEAN;				{ are we able to lookup items? }
		
		shouldHilite: BOOLEAN;				{ should be hilite the control? }
		
		theRPtr		: ^NBPReply;			{ what we send home }
		homeZone 	: Str32;				{ home zone for me, '*' if none }
		
		searchString: Str32;				{Status for user typing through list}
		lastKey		: LONGINT;
		
		focus		: INTEGER;				{ item number of focus where typing goes }
		fUserList	: BOOLEAN;				{ user-supplied list of items		}
		activeList	: Ptr;					{ list of focusable items			}
		
		isZoning	: BOOLEAN;				{ Are we waiting on keyboard input 	}
	END;

END.