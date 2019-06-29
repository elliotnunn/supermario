{
	File:		StandardNBPLDEF.p

	Contains:	LDEF used by StandardNBP package

	Written by:	Jerry Godes

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		<11>	 11/7/90	kaz		Adding IntlUtilsPriv in USES - oops.
		<10>	  9/4/90	kaz		Changing gray text _PenMode to _TextMode.
		 <9>	 8/19/90	kaz		Fixed gray text stuff to check for ColorQD and a color port.
		 <8>	  8/6/90	kaz		Add new dimming algorithm w/ new penmode.
		 <7>	 5/23/90	kaz		Code shrinking
		 <6>	 5/22/90	kaz		Changed point coercions for MPW 3.1+
		 <5>	 5/22/90	kaz		Updated to use new TextState rec
		 <4>	 5/17/90	kaz		Fixing Font handling
		 <3>	 4/17/90	kaz		Code Shrinking/Cleanup
									- Blended Icon plotting routines to PlotGraphic()
									- Defaults to teJustLeft if not teJustRight
									- RestoreWindowState() uses QD traps
									- Fixed bug that was TextBoxing the wrong string
		 <2>	 3/16/90	BBH		use TruncString instead of our own homegrown one
		 <1>	 3/14/90	BBH		first checked in

	To Do:
		
	
	Pre-BBS MODIFICATION HISTORY
		4/12/89		JNG		GetData now uses GetHState and SetHState
		5/31/89		JNG		Using DrawString instead of TextBox.  Using indent field of the
								list for our indent value.
		6/20/89		BBH		Changed TruncateString to pass boolean if script manager is
								installed or not.
		8/13/89		BBH		Changed ICON to hIcon in NLTypeEntry
		8/28/89		BBH		Removed use of window manager port.  Now use system font and system size
								in SetWindowState.
							Use RefCon to store whether or not script manager is installed.
							
							Use TextBox and GetSysJust to respect system justification in drawing lists
		8/29/89		BBH		Added color hilighting
							Removed beep if don't understand list manager message.
							
							If the icon handle passed in is of size 32 bytes (SICN handle) we assume
							that we are plotting a SICN directly instead of scaling a ICON handle…
							
							Draw icon centered in 18 pixel area (vertically)
		9/18/89		BBH		Subtract one in TruncateString calculation to be extra sure
	
}

UNIT StandardNBPLDEF;

INTERFACE
USES
	MemTypes, QuickDraw, ToolIntf, OSIntf, PackIntf, 
	AppleTalk,
	Script,
	Traps, SysEqu,
	CTBUtilities,
	CommToolboxPriv,
{$IFC NOT PRE70}
	IntlUtilsPriv,		{ type ahead	}
{$ENDC}
	StandardNBPStructures;

PROCEDURE RealLDEF(lMessage : INTEGER; lSelect : BOOLEAN; lRect : Rect;
				lCell : Cell; lDataOffset, lDataLen : INTEGER; lHandle : ListHandle);

IMPLEMENTATION

CONST
	SINGLESICNSIZE	=	32;			{ bytes per single SICN }
	SICNSIZE		=	16;			{ 16 pixels square 		}
	SICNPOINT		=	$00100010;	{ For point access		}
	SICNROWBYTES	=	2;			{ # rowbytes			}
	
	SINGLEICONSIZE	=	128;		{ 128 bytes per ICON 	}
	ICONSIZE		=	32;			{ 32 pixels square		}
	ICONPOINT		=	$00200020;	{ For point access		}
	ICONROWBYTES	=	4;			{ # rowbytes			}
	
	GRAY_ID			=	4;			{ index of 50% pattern in system pattern list 	}
	INDENT			=	3;			{ amount to indent horizontally 				}
	

PROCEDURE PlotGraphic(where: Rect; theIcon: Handle); FORWARD;

PROCEDURE RealLDEF(lMessage : INTEGER; lSelect : BOOLEAN; lRect : Rect;
				lCell : Cell; lDataOffset, lDataLen : INTEGER; lHandle : ListHandle);	
TYPE
	IntegerPtr	= ^INTEGER;
VAR
	savedText	: TextState;				{ saved grafport states		}
	savedPen  	: PenState;
	tempString	: Str255;					{ For truncation, etc		}
	width 		: INTEGER;
	newRect,
	sicnRect	: Rect;
	pat 		: Pattern;
	theItem 	: ListItem;
	theJust		: INTEGER;
	sLists		: SecretListHandle;
	aPort		: GrafPtr;
	
BEGIN
	IF (lMessage = lInitMsg) THEN
		lHandle^^.indent.h := INDENT;
	
	IF (lMessage = lHiliteMsg) OR (lMessage = lDrawMsg) THEN BEGIN
		{ get the local data	}
		BlockMove(Ptr(ORD4(lHandle^^.cells^) + lDataOffset), 
												Ptr(@theItem), lDataLen);

		IF (lMessage = lDrawMsg) THEN
			WITH theItem DO BEGIN
				sLists := SecretListHandle(lHandle^^.refCon);				
				
				{ Save the current port state & set it to what we want	}
				GetTextState(savedText);
				GetPenState(savedPen);
				
				PenNormal;
				SetTextState(sLists^^.theFontInfo);
				
				EraseRect(lRect);
				
				theJust := GetSysJust;
				newRect := lRect;
				
				{ Determine Rect for drawing stuff	}
				WITH newRect DO
					IF theJust = teJustRight THEN BEGIN
						left := left + 1;
						right := right - lHandle^^.indent.h;
					END
					ELSE BEGIN
						left := left + lHandle^^.indent.h;
						right := right - 1;
					END;
				
				{ Calc rect for plotting icon	}
				IF sLists^^.hasIcons THEN BEGIN	
					SicnRect := newRect;
					
					WITH SicnRect DO BEGIN
						top := top + 1;
						bottom := top + SICNSIZE;
						
						IF theJust = teJustRight THEN BEGIN
							left := right - SICNSIZE;	
							newRect.right := left - 3;		{Text goes before the icon}
						END
						ELSE BEGIN
							right := left + SICNSIZE;
							newRect.left := right + 3;		{Text goes after the icon}
						END
							
					END;					
				END;
				
				{Draw the ICON or sicn	}
				IF (hIcon <> NIL) THEN
					PlotGraphic(SicnRect, hIcon);
				
				{ Draw the string		}
				WITH newRect DO
					width := right - left - 1;
				
				{ theItem is already duplicated but Pascal won't let me pass	}
				{ a Str32 for a VAR Str255 in _TrucString! fuck!				}
				BlockMove(@theItem.objStr, @tempString, LENGTH(objStr) + 1);
				
				IF StringWidth(tempString) > width THEN BEGIN
					TextFace(sLists^^.theFontInfo.theFace + [condense]);
					IF TruncString(width, tempString, smTruncEnd) = 0 THEN
						;
				END;
				
				{ Try for true gray text	}
				IF dimmed THEN BEGIN
					GetPort(aPort);
					IF LONGINT(IntegerPtr($28E)^) <= $3FFF THEN
						IF BAND(CGrafPtr(aPort)^.portVersion, $C000) <> 0 THEN
							TextMode(grayishTextOr);
				END;
				
				TextBox(Ptr(LONGINT(@tempString) + 1), LENGTH(tempString),
																newRect,theJust);
				IF dimmed THEN BEGIN	
					GetIndPattern(pat, sysPatListID, GRAY_ID);				
					PenPat(pat);
					PenMode(patBic);
					
					{ If true gray, then we just paint the icon	}
					newRect := lRect;
					IF LONGINT(IntegerPtr($28E)^) <= $3FFF THEN
						IF BAND(CGrafPtr(aPort)^.portVersion, $C000) <> 0 THEN
							newRect := SicnRect;
					PaintRect(newRect);
				END;
					
				SetTextState(savedText);
				SetPenState(savedPen);				
			END; { with }
		
		IF lSelect OR (lMessage = lHiliteMsg) THEN
			IF NOT theItem.dimmed THEN BEGIN
				BitClr(Ptr(HiliteMode), pHiliteBit);	{ use color hilighting please }
				InvertRect(lRect);
			END;
	
	END;	{ Draw/Hilite	}
			
END;


{ -----------------------------------------------------------------
	PlotGraphic		- Plots either a 'ICON' or 'sicn'
	
		where		- The place to put it
		theIcon		- Handle of data (must ne non-NIL)
-------------------------------------------------------------------	}

PROCEDURE PlotGraphic(where: Rect; theIcon: Handle);
VAR
	srcBits			: BitMap;
	port			: GrafPtr;
	theHandleSize	: LONGINT;
	
BEGIN
	theHandleSize := GetHandleSize(theIcon);
	IF theHandleSize = 0 THEN					{ Error in GetHandleSize	}
		EXIT(PlotGraphic);
	
	{ Fill in the BitMap	}
	WITH srcBits DO BEGIN
		baseAddr := Ptr(theIcon^);
		rowBytes := ICONROWBYTES;
		
		LONGINT(bounds.topLeft) := 0;
		LONGINT(bounds.botright) := ICONPOINT;
		
		IF theHandleSize = SINGLESICNSIZE THEN BEGIN
			rowBytes := SICNROWBYTES;
			LONGINT(bounds.botright) := SICNPOINT;
		END;	
	END;
	
	GetPort(port);
	CopyBits(srcBits, port^.portBits, srcBits.bounds, where, srcCopy, NIL);
END;

END.