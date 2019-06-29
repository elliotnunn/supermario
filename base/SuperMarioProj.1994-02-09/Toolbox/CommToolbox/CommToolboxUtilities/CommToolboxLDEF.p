{
	File:		CommToolboxLDEF.p

	Contains:	Icon-like LDEF used by the tools

	Written by:	Byron Han

	Copyright:	© 1987-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<10>	  2/8/91	kaz		jng, #82582: LAddColumn calls drawMsg with no dataLen, and we
									were blindly reading data — we now erase the rect and exit.
		 <9>	12/13/90	JNG		Use the System LDEF <kaz>
		 <8>	 5/24/90	kaz		Code shrinking; Uses GetIndflst; Getflst moved to Utility
									Routines
		 <7>	 5/22/90	kaz		Updated to use TextState instead of FontInfoRec
		<5+>	 4/18/90	kaz		Code shrink/Cleanup
									- Broke into several procedures
									- Got rid of unnecessary HLocks, etc
		 <5>	 3/21/90	EMT		Added AppleTalk USES for Str32 in CommToolboxPriv.p.
		 <4>	 3/19/90	BBH		restore grafport information via quickdraw calls instead of
									stuffing the grafport directly; also added handling of partial
									finf from the resource file (use the system file's finf if the
									tool does not provide finf information for the ldef)
		 <3>	 3/18/90	BBH		was restoring curmap to CurResFile (!) instead of savedResRef
									(old variable name was curResRef)
		<2+>	 3/18/90	BBH		was restoring curmap to CurResFile instead of savedResRef
		 <2>	 3/16/90	BBH		change iconOffset from 40 to 8
		 <1>	 3/16/90	BBH		first checked in

	To Do:

	
	Pre BBS Modification
		9/18/89		BBH		Subtract one in TruncateString calculation to be extra sure
		8/25/89		BBH		Changed entry point from LDEF to RealLDEF for standard Apple header			
							in IconLDEF.a
		7/27/89		BBH		Call TruncateString before drawing the icon's title
							Use constants for HiliteMode and hiliteBit.
		5-May-89	kaz		• Checked for a 'finf' resource in the tool with
							procID stored in the refcon.  If it exists, use
							it, else use the toolbox's.
		4-May-89	kaz		• Allocates a private storage area for font info and
							hiliting.
							• Text info is gotten from the toolbox 'finf' resource
							• Checks to see if the tool wants to use its own
							finf resource.
							• Adjusted the distance between icon & title to be
							1 pixel (à la Finder/Control Panel)
							• Adjusted the right side of the hilited text rect
		3-May-89	kaz		• Ported from ModemLDEF.p  Put definition of CellBlock
							struct in CTBPrivate along with a res ID constant.
							Modified Makefile to build it into utilities
							• Changed HUnlock() to HSetState() so we don't 
							inadvertantly unlock it behind someone's back
	
}

UNIT IconLDEF;

INTERFACE
USES
	MemTypes, 
	QuickDraw, 
	OSIntf, 
	ToolIntf, 
	PackIntf, 
	Script, 
	SysEqu, 
	Traps,
	AppleTalk,
	CommToolboxPriv;
		
PROCEDURE RealLDEF(lMessage : INTEGER; lSelect : BOOLEAN; lRect : Rect;
				lCell : Cell; lDataOffset, lDataLen : INTEGER; lHandle : ListHandle);

IMPLEMENTATION

PROCEDURE CallLDEF(lMessage : INTEGER; lSelect : BOOLEAN; lRect : Rect;
				lCell : Cell; lDataOffset, lDataLen : INTEGER; lHandle : ListHandle;
				theLDef : ProcPtr);
{ inline is equivalent to : movea.l (a7)+,a0; jsr (a0) }
    INLINE $205F, $4E90;
	
PROCEDURE RealLDEF(lMessage : INTEGER; lSelect : BOOLEAN; lRect : Rect;
				lCell : Cell; lDataOffset, lDataLen : INTEGER; lHandle : ListHandle);
CONST
	ICONOFFSET		= 8;		{ pixel offset from top of cell to top of icon	}
	ICONSIZE		= 32;		{ size of an icon								}
	ICONHALF		= 16;
	kIconEntry		= 2;		{ finf index for Icon entry	}
	REALICONSIZE	= 256;
	
VAR	
	theData		: CellDataBlock;
	myCloset	: IconPrivateH;
	theFinf		: TextState; 		{ for font info		}

	tempData	: NewCellDataBlockH;
	tempIcon	: Handle;
	tempResFile	: integer;
	theState	: SignedByte;
	theLDEF		: Handle;
	oldCells	: DataHandle;
	
BEGIN
	{ Create our private struct									}
	{ We need to save the tool's resRef so Getflst will work	}
	IF (lMessage = lInitMsg) THEN BEGIN
		myCloset := IconPrivateH(NewHandleClear(sizeOf(IconPrivateBlock)));
		IF (myCloset <> NIL)  THEN 
		BEGIN
			tempData  := NewCellDataBlockH(NewHandleClear(sizeof(NewCellDataBlock)));
			tempIcon := NewHandleClear(REALICONSIZE);
			tempData^^.cHand := tempIcon;
			myCloset^^.tempData := tempData;
		END;
		lHandle^^.refCon := LONGINT(myCloset);
	END;
	
	IF (lMessage = lCloseMsg) THEN BEGIN
		IF (lHandle^^.refCon <> 0) THEN
		BEGIN
			IF (NewCellDataBlockH(IconPrivateH(lHandle^^.refCon)^^.tempData) <> NIL) THEN
			BEGIN
				DisposHandle(NewCellDataBlockH(IconPrivateH(lHandle^^.refCon)^^.tempData)^^.cHand);
				DisposHandle(Handle(IconPrivateH(lHandle^^.refCon)^^.tempData));
			END;
			DisposHandle(Handle(lHandle^^.refCon));
		END;
	END;
			
	IF (lMessage = lDrawMsg) OR (lMessage = lHiliteMsg) THEN BEGIN
		myCloset := IconPrivateH(lHandle^^.refCon);
		IF (myCloset = NIL) THEN
			EXIT(RealLDEF);
		
		{ If dataLen = 0, we need to erase and drop out	}
		IF (lDataLen = 0) THEN BEGIN
			EraseRect(lRect);
			EXIT(RealLDEF);
		END;
		
		{ get the cell data			}
		BlockMove(Ptr(ORD4(lHandle^^.cells^) + lDataOffset), 
									Ptr(@theData), SIZEOF(CellDataBlock));
		
		{ Set the new text info		}
		Getflst(myCloset^^.procID, CTB_ChoosefinfID, kIconEntry, theFinf);

		with myCloset^^.tempData^^ do
		BEGIN
			cName := theData.title;
			cFont := theFinf.theFont;
			cFace := theFinf.theFace;
			cSize := theFinf.theSize;
			BlockMove(@theData.icon,cHand^,REALICONSIZE);
		END;
				
		With lHandle^^ do BEGIN
			oldCells := cells;
			cells	:= DataHandle(myCloset^^.tempData);
		END;
		
		tempResFile := CurResFile;
		UseResFile(0);
		theLDEF := Get1Resource('LDEF',19);
		useResFile(tempResFile);

		theState := HGetState(theLDEF);
		HLock(theLDEF);
		CallLDEF(lMessage, lSelect, lRect,lCell,0,sizeof(NewCellDataBlock),lHandle,ProcPtr(theLDEF^));
		HSetState(theLDef,theState);
		lHandle^^.cells	:= oldCells;
	END; { Draw/Hilite	}

END;

END.
