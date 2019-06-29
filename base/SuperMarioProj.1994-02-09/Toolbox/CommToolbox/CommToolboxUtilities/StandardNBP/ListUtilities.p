{
	File:		ListUtilities.p

	Contains:	List Manager utilities for StandardNBP package

	Written by:	Byron Han
				Jerry Godes

	Copyright:	© 1987-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <7>	 11/6/90	kaz		Adding IntlUtilsPriv USES file for typeAhead. <jng>
		 <6>	 5/17/90	kaz		Fixing Point coercions for MPW 3.1+
		 <4>	 3/28/90	kaz		Using RelString for string comparisons - Removed calls to
									PCopyString, MyUprString
		 <3>	 3/20/90	kaz		Merging in Code compression/cleanup from 1.0
									- Took common code out of FindSortedItemIndex() and 
				 						moved it into MyCellCompare()
									- Code crunch of FindSortedItemIndex: White Dwarf Density
									- Removed unSAFERSORT code
	   								- Changed _SetPt to set the record directly
									- Cleaned up AddSortedToList()
									- Use PCopyString instead of Pascal aStr := bStr
		 <2>	 3/16/90	BBH		made it compile
		 <1>	 3/14/90	BBH		first checked in to BBS

	To Do:

	Pre-BBS Modification History:
		9/5/89		BBH		Removed ChangeSortedSelection (same as SelectSortedItem)
							Removed SearchSortedItem (same as FindSortedItem)
							Cleaned up SelectSortedItem
		8/31/89		BBH		Added SAFERsort which is defined in ListItems.  This allows for
								smaller ListItem data structures by not caching
		5/31/89		JNG		FindSortedItemIndex now uses a binary search for object, followed
								by a sequential search for the type.
		5/30/89		JNG		All procedures now have same two first parameters.
							List item is now a var parameter - we don't have to copy it.
							AddSorted now adds only unique items.
		4/24/89		JNG		Changed GetSortedSelection to return the cell as well
		10/28/88	JNG		Changed to deal with type and zone fields in list.
}

UNIT ListUtilities;
INTERFACE
USES
	MemTypes, QuickDraw, OSIntf, ToolIntf, PackIntf,
	Appletalk,
	CTBUtilities,
	CommToolboxPriv,
{$IFC NOT PRE70}
	IntlUtilsPriv,		{ type ahead	}
{$ENDC}
	StandardNBPStructures;

PROCEDURE 	AddSortedToList(VAR theItem : ListItem; theList : ListHandle);
FUNCTION 	SelectSortedItem(VAR theItem : ListItem; theList : ListHandle) : BOOLEAN;
FUNCTION  	FindSortedItem(VAR theItem : ListItem; theList : ListHandle; VAR theCell : Point) : BOOLEAN;
FUNCTION 	GetSortedSelection(VAR theItem : ListItem; theList : ListHandle; 
								VAR theCell : Cell) : BOOLEAN;

IMPLEMENTATION

{ Searches the sorted list for an occurence of theItem.  Returns true if the
 item is found, or FALSE if it isn't found.  Also returns the index of the first item in
 the list which is >= theItem.
 }
 
TYPE
	CompareType		= (objCmp, typeCmp);

FUNCTION MyCellCompare(theList: ListHandle; theCell: Point; cmpStr: Str32; 
															cmp: CompareType) : INTEGER;
VAR
	len			: INTEGER;
	compItem	: ListItem;
	dataStr		: StringPtr;
	
BEGIN
	{ Get the data from the cell		}
	len := SIZEOF(ListItem);
	LGetCell(Ptr(@compItem),len,theCell,theList);

	dataStr := StringPtr(@compItem.typeStr);
	IF (cmp = objCmp) THEN
		dataStr := StringPtr(@compItem.objStr);
		
	MyCellCompare := RelString(dataStr^, StringPtr(@cmpStr)^,FALSE, FALSE);
END;


FUNCTION FindSortedItemIndex(VAR theItem : ListItem; theList : ListHandle; 
													VAR itemIndex : Integer) : BOOLEAN;
VAR
	minIndex		: INTEGER;
	maxIndex		: INTEGER;
	theCell			: Cell;
	resultObj, 
	resultType		: INTEGER;
	goingUp			: BOOLEAN;

BEGIN
	theCell.h := 0;
	
	minIndex := 0;
	maxIndex := theList^^.dataBounds.bottom - 1;
	
	IF maxIndex < minIndex THEN	BEGIN 		{ nothing in list - bail					}
		itemIndex := -1;
		FindSortedItemIndex := FALSE;		{	did not find anything					}
		EXIT (FindSortedItemIndex);
	END;
	
	{ Find the first match		}
	REPEAT
		theCell.v := BSR(minIndex+maxIndex,1);	{Find the midpoint}

		resultObj := MyCellCompare(theList, theCell, theItem.objStr, objCmp);
		
		IF (resultObj < 0) THEN				{Higher, Higher}
			minIndex := theCell.v + 1
		ELSE IF (resultObj > 0)	THEN		{Lower, Lower}
			maxIndex := theCell.v - 1;
	UNTIL ((minIndex > maxIndex) OR (resultObj = 0));
	
	{ If Object matched (resultObj = 0), then match types	}
	IF (resultObj > 0) THEN
		itemIndex := theCell.v;
		
	IF (resultObj < 0) THEN
		itemIndex := theCell.v + 1;
		
	IF (resultObj = 0) THEN BEGIN		{ A Matched name		}
		
		resultType := MyCellCompare(theList, theCell, theItem.typeStr, typeCmp);
		
		{ If the type is wrong, find another match		}
		IF (resultType = 0) THEN
			itemIndex := theCell.v
		ELSE BEGIN
		
			goingUp := (resultType < 0);		{ which we are we headed	}
			
			{ Skip up or down to find the right one	}
			WHILE ((resultObj = 0) AND (theCell.v > 0)) DO BEGIN
			
				IF goingUp THEN
					theCell.v := theCell.v + 1
				ELSE
					theCell.v := theCell.v - 1;

				resultObj := MyCellCompare(theList, theCell, theItem.objStr, objCmp);
				resultType := MyCellCompare(theList, theCell, theItem.typeStr, typeCmp);
				
				IF goingUp AND (resultType >= 0) THEN
					LEAVE;
					
				IF NOT goingUp AND (resultType <= 0) THEN
					LEAVE;
			END; { while }
			
			itemIndex := theCell.v;				
			
			{ Backed up too far with the object or type	}
			IF (resultObj < 0)	OR (resultType < 0) THEN
				itemIndex := itemIndex + 1;
				
		END; { Find type	}
		
	END; {resultObj = 0}
	
	FindSortedItemIndex := (resultType = 0) AND (resultObj = 0);
END;

{	
	speeded this up by not manually looping through the list and turning off
	selection ourselves…
}
FUNCTION SelectSortedItem(VAR theItem: ListItem; theList: ListHandle): BOOLEAN;
VAR
	found			: BOOLEAN;
	j				: INTEGER;
	theCell			: Cell;
	selectedIndex	: INTEGER;	
	
BEGIN
	found := TRUE;
	WHILE found DO BEGIN
		LONGINT(theCell) := 0;							{	go to the top 	}
		found := LGetSelect(TRUE, theCell, theList);	{	get selection	}
		IF found THEN
			LSetSelect(FALSE, theCell, theList);		{	so turn it off	}
	END;
	
	found := FindSortedItemIndex(theItem, theList, selectedIndex);
	IF found THEN
		WITH theCell DO BEGIN
			h := 0;
			v := selectedIndex; 
			LSetSelect(TRUE, theCell, theList);
		END;
	
	SelectSortedItem := found;
END;


{	
	this adds an item to the list (in sorted order) avoiding adding duplicates
}
PROCEDURE AddSortedToList(VAR theItem : ListItem; theList : ListHandle);
VAR
	theCell	: Point;
	
BEGIN	
	LONGINT(theCell) := 0;
	
	IF FindSortedItem(theItem, theList, theCell) THEN
		EXIT (AddSortedToList);						{	we don't do duplicates			}
	
	WITH theCell DO BEGIN
		
		IF v < 0 THEN								{	insert at beginnning of list	}
			LONGINT(theCell) := 0;

		v := LAddRow(1, v, theList);
	END;
	
	LSetCell(Ptr(@theItem), SIZEOF(ListItem), theCell, theList);
END;


{ Find an item in a sorted list }

FUNCTION  FindSortedItem(VAR theItem : ListItem; theList : ListHandle; VAR theCell : Point) : BOOLEAN;
VAR
	found 	: BOOLEAN;
	j 		: INTEGER;
	
BEGIN
	found := FindSortedItemIndex(theItem, theList, j);
	
	LONGINT(theCell) := -1;
	
	IF (j >= 0) THEN
		WITH theCell DO BEGIN
			h := 0;
			v := j;
		END;

	FindSortedItem := found;
END;

{	
	get current selected cell and data, returning TRUE if there is a selection
	and FALSE if there is none 
}
FUNCTION GetSortedSelection(VAR theItem: ListItem; theList: ListHandle; 
							VAR theCell: Cell) : BOOLEAN;
VAR
	len : INTEGER;
	
BEGIN
	LONGINT(theCell) := 0;
	
	GetSortedSelection := FALSE;
	
	IF LGetSelect(TRUE, theCell, theList) THEN BEGIN
		len := SIZEOF(ListItem);
		LGetCell(Ptr(@theItem), len, theCell, theList);
		
		GetSortedSelection := TRUE;
	END;
END;

END.
