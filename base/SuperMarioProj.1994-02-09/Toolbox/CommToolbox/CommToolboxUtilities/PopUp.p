{
	File:		PopUp.p

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 3/14/90	BBH		first checked in (this is for pre-70)

	To Do:
}

{	
	PopUp CDEF - Copyright © 1987-1990 by Apple Computer, Inc.  All rights reserved.
	Written by Byron Han, MS: 27Y.  For internal use only.
	
	
	This implements a pop-up menu using the PopUpMenuSelect trap taking care of
	hilighting and updating stuff.  Requires System 4.1 or later.
	
	This control performs its own tracking so pass POINTER(-1) to TrackControl.
	
	
	Modification History
		10/20/87	BBH		First version
		11/2/87		BBH		Second version
		7/88		BBH		First major rewrite.  Use refCon to specify menuID and popup title
								width.  Use flag bit in refCon to determine if application 
								is performing InsertMenu/DeleteMenu.
								
							Refcon values
								HiWord contains ID of menu
								LoWord contains width of popup title
							
								If the hi bit of the low word of the refcon is set, then the calling
								applciation must install/deinstall the menu from the menu list.
								
							Rect is rect.
							
		9/88		BBH		Second major rewrite to add AddResMenu capability
		
							refCon is NIL - nothing
							refCon is NOT nil - do a AddResMenu(ResType(refcon));
							
							menuID is MIN
							popuptitlewidth is MAX
							
		12/22/88 	BBH		value is justification of the popup menu title.  It is
								reset to min after control creation
					
							application must reset the value then.
	
		12/23/88 	BBH		Drawing of the ow done with TextBox.  Is using GetSysJust
							Also, instead of using System Font size 12, we get font and
								size information from window manager port.
								
							The test routine will return 0 or myPartCode which is 1.
			
							Caveats
								The CDEF installs and deinstalls the menu from the menu list 
								when the control is created/destroyed.
								
								If the menu does not exist, you are at your own risk.
					
		3/14/89 	BBH		use System Font size 0.
							also added support for variant codes.
							moved refcon/AddResMenu stuff to variant code 4.
							added color quickdraw stuff with support for mctb.
								opens a CGrafPort is necessary.
							added support for sys just of teJustRight to draw
								title to the RIGHT of the pop up box.
			
		3/21/89 	AEK		in DoTrack() check to see if variant is UseWFont
								then save the old WMgrPort text info, set it to the
								current window text info, then reset it.  This
								should paint the menus in the right font/size
					
							Changed Pop-up to use a bullet mark • for the
								checked menu item instead of a checkmark
		
		3/22/89 	BBH		use checkmark if using system font
							use • if using window font instead of checkMark
					
							we now call preflightFont to load in font info if using window font
								prior to calling CalcMenuSize in the DoResize routine.  
					
							Also indentation when drawing popup box is using the max char width
								of the font minus two.

		4/26/89 	BBH		Changed all BitAnd's to BANDs
							Changed drawing to not subtract 3.  This fixes shifting of quiescent and
								active menu items with system 6.0.4
	
		5/3/89		BBH		Changed HLock and HUnlock of the control handle to HGetState, 
								HLock, HSetState
		
		5/31/89		BBH		popup value has the justification stored as a signed Byte 
								in the low byte and stores the popup title style in 
								the high byte if the high bit is clear

							Defined new constants in CTBUtils.r, .p, and .h
							If high bit of the popup value is set, ignore style (this is for
								backwards compatibility with old CNTL templates which use -1 for
								the popup title justification)
							
									procID
									#define	popupMenuCDEFproc	32
									
									varCodes
									#define	popupFixedWidth		1
									#define	popupUseCQD			2
									#define	popupUseAddResMenu	4
									#define	popupUseWFont		8
									
									cntlValues
									#define	popupTitleBold		0x00000100
									#define	popupTitleItalic	0x00000200
									#define	popupTitleUnderline	0x00000400
									#define	popupTitleOutline	0x00000800
									#define	popupTitleShadow	0x00001000
									#define	popupTitleCondense	0x00002000
									#define	popupTitleExtend	0x00004000
									#define popupTitleNoStyle	0x00008000
									
									#define	popupLeftJust		0x00000000
									#define	popupCenterJust		0x00000001
									#define	popupRightJust		0x000000FF	
					
		6/4/89		BBH		PreflightFont is called after resetting the window manager ports
								in DoResize.
							
		6/7/89		BBH		Changed EraseRect of the control rectangle to a EraseControl call
								which will only erase the parts of the control that get drawn
								which means if the window is filled with a pattern, no annoying
								whitespace gets left behind.
	
		6/8/89		BBH		Fixed bug that Rich Hoiberg was having that if you have a control
								attached to a dialog item list and after creating the dialog
								(and hence creating the control) you change the dialog font and
								dialog size, the height of the popup is not recalculated properly.
								fixed by calling GetFontInfo in DoResize and manually adjusting
								the control height if the variation code for fixed size is not being
								used.
							
							Removed info: FontInfo and height: INTEGER from myData data structure
							
		6/14/89		BBH		Added support for calcCntlRgn and calcThumbRgn messages for System 7.0 
								compatibility.  See TN 212
	
		6/16/89		BBH		TruncateString is now script manager compatible - uses CharByte.
						
							Also, when drawing the control, we clip to the control rectangle.
							This prevents a problem with descenders in geneva 9pt, shadow being
								drawing outside of the control rectangle.
		6/18/89		BBH		Instead of setting clip to the control rectangle, set to the intersection
								of the existing clipRgn and the control rectangle.
							Removed scripts field from private data structure.  Was not being used.
							
							Reformatted modification history.  Added more generous comments.
							Also use CTBUtils.p pascal interface file for the variant code 
							constants.
							
							Removed some dead code.  Did a personal review of the code.
					
		6/19/89		BBH		Added checks for the popup menu trap and the script manager trap.
							If the popup menu trap is NOT available, then draw
								the control as if disabled and do not perform tracking.
							If the script manager trap is NOT available, then truncate the
								string one byte at a time (do not test for multibute chars)
		
		7/25/89		BBH		Always recalculate the height regardless of popupFixedWidth bit.
		
		10/3/89		---		If menu preexists in the menu list, we do not a GetMenu, InsertMenu
								upon creation, and a DeleteMenu, DisposMenu upon disposal
								(SAFEMENUS set to TRUE for this feature)
							
							Fixed bug in TruncateString if space given is insufficient for even
								an ellipsis caused infinite loop in truncation.
								
							TruncateString routine has been modified to return empty string
								if there is insuffient space for 1 character (can be multibyte)
								plus the ellipsis.
		
		10/4/89		BBH		Minimum safe title width is 1 pixel.  We now check.
								
	Things to be done:
		TruncateString should be internationalized.  Test it.
		Why does TrackControl always return non zero even if we track out of the control?
}

UNIT CDEF;
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
	CTBUtilities;

FUNCTION PPopUp(theVar : INTEGER; theControl : ControlHandle;
										message : INTEGER; param : LONGINT) : LONGINT;
	
IMPLEMENTATION

{$SETC SAFEMENUS:=TRUE}

CONST
{	low memory globals used	}
	SysFontFam 			= 	$BA6;	{ font number of system font								}
	SysFontSize 		= 	$BA8;	{ font size of system font									}
	CurFMInput			= 	$988;	{ current cached font manager information					}
	
	BulletMark			= 	$A5;	{ Bullet item check mark	} { for popupUseWFont 			}
	
	grayPatternIndex	=	4;		{ pattern number of 50% gray in system pattern list			}
	
	myPartCode 			=	1;		{ in PopUp 													}

	popupSlop			=	3;
	
{=======================================================
=======================================================}
	calcCntlRgn		=	10;			{ for system 7.0 }
	calcThumbRgn	=	11;			{ for system 7.0 }
{=======================================================
=======================================================}

TYPE
	IntPtr	= ^INTEGER;
	LongPtr	= ^LONGINT;
	
	myDataH = ^myDataP;
	myDataP = ^myData;
	myData = PACKED RECORD
		theMenu 	: MenuHandle;										{ the MenuHandle		}
		theMenuID 	: INTEGER;											{ the Menu ID 			}

		width      	: INTEGER;						{ width of PopUp title area					}
		hasColor	: BOOLEAN;						{ color quickdraw exists					}
		color		: BOOLEAN;						{ color quickdraw to be used				}
		makeCPort	: BOOLEAN;						{ need to make our own CPort				}
		titleStyle	: Style;						{ do we bold the title? 					}
		just		: INTEGER;						{ title justification						}
		noPopupTrap	: BOOLEAN;						{ no popup menu select trap					}
		noScriptMgr	: BOOLEAN;						{ no script manager trap					}
{$IFC SAFEMENUS}
		noKillMenu	: BOOLEAN;						{ menu preexisted in menu list so no delete	}
{$ENDC}
	END;
	
	TextInfo = PACKED RECORD
		SSFont		:	INTEGER;					{ saved system font family number			}
		SSSize		:	INTEGER;					{ saved system font size					}
		
		wmgrFont	:	INTEGER;					{ saved window manager port size			}
		wmgrCFont	:	INTEGER;					{ saved color window manager port size		}
		wmgrSize	:	INTEGER;					{ saved window manager port size			}
		wmgrCSize	:	INTEGER;					{ saved color window manager port size		}
	END;
	
{	messages	}
PROCEDURE DoInit(theControl: ControlHandle;
					theVar: INTEGER); FORWARD;
FUNCTION  DoDraw(theControl : ControlHandle;
					theVar: INTEGER) : LONGINT; FORWARD;
FUNCTION  DoTest(theControl : ControlHandle; param : LONGINT) : LONGINT; FORWARD;
PROCEDURE DoCalc(theControl : ControlHandle; theVar: INTEGER;
					VAR param : LONGINT); FORWARD;
PROCEDURE DoDispose(theControl : ControlHandle); FORWARD;
FUNCTION  DoTrack(theControl : ControlHandle; theVar: INTEGER) : LONGINT; FORWARD;

{	Utility Routines }
PROCEDURE TruncateString(VAR theString: STR255; width: INTEGER; noSM: BOOLEAN); FORWARD;

PROCEDURE PreflightFont; FORWARD;
PROCEDURE SetWMPortText(theWindow: WindowPtr; color: BOOLEAN); FORWARD;
PROCEDURE SaveWMPortText(VAR theTextInfo: TextInfo; color: BOOLEAN); FORWARD;
PROCEDURE ResetWMPortText(theTextInfo: TextInfo; color: BOOLEAN); FORWARD;

PROCEDURE ResizeControl(theControl : ControlHandle; theVar: INTEGER); FORWARD;
PROCEDURE EraseControl(theRect: Rect; theData: myDataH; enabled: BOOLEAN); FORWARD;

{	sort of main entry point }
{===============================================================================================}
{===============================================================================================}
{===============================================================================================}
{
						M A I N   E N T R Y   P O I N T   T O   C D E F
										(sort of)
}
{===============================================================================================}
{===============================================================================================}
{===============================================================================================}
FUNCTION  PPopUp(theVar : INTEGER; theControl : ControlHandle; 
										message : INTEGER; param : LONGINT) : LONGINT;
VAR
	savedFont 	: INTEGER;
	savedSize 	: INTEGER;
	savedFace 	: Style;
	savedPen  	: PenState;
	savedMode 	: INTEGER;
	
	theData		: myDataH;
	savedPort	: GrafPtr;
	theState	: SIgnedByte;

BEGIN
	PPopup := 0;

	GetPort(savedPort);
	SetPort(theControl^^.contrlOwner);
	
	theState := HGetState(Handle(theControl));
	HLock(Handle(theControl));									{ lock to be sure 				}
	WITH theControl^^.contrlOwner^ DO
	BEGIN
		savedFont := txFont;									{ save the environment			}
		savedSize := txSize;
		savedFace := txFace;
		savedMode := txMode;
	END;
	GetPenState(savedPen);
	
	PenNormal;
	IF BAND(theVar, popupUseWFont) <> popupUseWFont THEN		{ are we a variation?			}			
	BEGIN														{ no so use system stuff		}
		TextFont(systemFont);
		TextSize(0);
		TextFace([]);
		TextMode(srcOr);
	END;
	
	CASE message OF												{ take care of our massages		}
		drawCntl:		
			IF theControl^^.contrlVis <> 0 THEN 
				PPopup := DoDraw(theControl, theVar);
			
		testCntl:	
			PPopUp := DoTest(theControl, param);
			
		calcCRgns, 
		calcCntlRgn, 				{ for 7.0 compatibility }
		calcThumbRgn : 				{ for 7.0 compatibility }
			DoCalc(theControl, theVar, param);
			
		initCntl: 
			DoInit(theControl, theVar);
			
		dispCntl: 
			DoDispose(theControl);

		autoTrack: 
			PPopup := DoTrack(theControl, theVar);
	END; {case}
	
	WITH theControl^^.contrlOwner^ DO
	BEGIN
		txFont := savedFont;							{ restore environment					}
		txSize := savedSize;
		txFace := savedFace;
		txMode := savedMode;
	END;
	
	SetPenState(savedPen);
	HSetState(Handle(theControl), theState);			{ restore the control handle 			}
	SetPort(savedPort);
END;

{===============================================================================================}
PROCEDURE DoInit(theControl : ControlHandle; theVar: INTEGER);
VAR
	theData 	: 	myDataH;
	theWorld 	: 	SysEnvRec;
	theErr 		:	OSErr;
		
	tempJust	:	INTEGER;
	tempSB		:	SignedByte;
	myInfo		:	FontInfo;
	height		:	INTEGER;							{ height of the control 				}
	
BEGIN
	WITH theControl^^ DO
	BEGIN
		tempJust := GetSysJust;							{ get system justification				}
		
		contrlData := NewHandle(SIZEOF(myData));		{ create control private data block 	}
		theData := myDataH(contrlData);
		IF theData = NIL THEN							{ sanity checking 						}
			Exit(DoInit);
			
		HLock(Handle(theData));							{ lock if down for now 					}
		
		theErr := SysEnvirons(curSysEnvVers, theWorld);	
														{ This is Radio Moscow... 				}
		
		IF theErr <> noErr THEN							{ being careful }
		BEGIN											{ at this point we are in deep shit	 	}
			WITH theData^^ DO
			BEGIN
				hasColor := FALSE;						{ no color capability 					}
				color := FALSE;							{ no color desired						}
				
				makeCPort := FALSE;						{ no need to create a cGrafPort 		}
			END;
		END
		ELSE
		BEGIN
			WITH theData^^ DO
			BEGIN
				hasColor := theWorld.hasColorQD;					{ are we color capable?		}
				color := hasColor;									{ dp we want color?			}
				IF BAND(theVar, popupUseCQD)<>popupUseCQD THEN		{ variation code no go QD	}
					color := FALSE;						
					
				makeCPort := FALSE;									{ do we create cGrafPort?	}
				IF color THEN			{ if color capable and we want color but owner is a		}
				BEGIN					{ classic grafPort, create cGrafPort to draw control in	}
					IF BAND(contrlOwner^.portBits.rowBytes, $0000C000) <> $0000C000 THEN
						makeCPort := TRUE;
				END;
			END;
		END;
		
		WITH theData^^ DO								{ get the appropriate menu information	}
		BEGIN
		
		{ determine if the traps we need are available…	}
			noPopupTrap :=  
				(NGetTrapAddress(_Unimplemented, OSTrap) = NGetTrapAddress(_PopUpMenuSelect, ToolTrap));
			noScriptMgr :=  
				(NGetTrapAddress(_Unimplemented, OSTrap) = NGetTrapAddress(_ScriptUtil, ToolTrap));
			
		{ get menu ID and handle  }
			theMenuID := contrlMin;						{ the menu ID							}
			width := contrlMax;							{ width of the popup title area			}
			IF width < 1 THEN
				width := 1;								{ minimum sane value for width			}
{$IFC SAFEMENUS}
			theMenu := GetMHandle(theMenuID);
			noKillMenu := (theMenu <> NIL);				{ if menu preexists, do not dispose it	}
			IF NOT noKillMenu THEN
			BEGIN
				theMenu := GetMenu(theMenuID);			{ get the menu handle					}
				InsertMenu(theMenu, -1);				{ stick into the menu list			 	}
			END;
{$ELSEC}
			theMenu := GetMenu(theMenuID);				{ get the menu handle					}
			InsertMenu(theMenu, -1);					{ stick into the menu list			 	}
{$ENDC}
		
			IF BAND(theVar, popupUseAddResMenu) = popupUseAddResMenu THEN
			BEGIN										{ do a addresmenu on the contrlRfCon	}
				AddResMenu(theMenu, ResType(contrlRFcon));
			END;	
		END;
				
		contrlMin := 1;									{ set bounds of control value			}
		contrlMax := CountMItems(theData^^.theMenu);	{ max is # menu items 					}
		IF contrlMax = 0 THEN
			contrlMin := 0;
		
		WITH theData^^ DO
		BEGIN
			tempSB := BAND($000000FF, contrlValue);					{ get the justification 	}
			just := tempSB;											{ sign extend for me 		}
			
			titleStyle := [];										{ do not fuck with title 	}
			IF BAND(contrlValue, popupTitleNoStyle) <> popupTitleNoStyle THEN	
				titleStyle := Style(BSR(BAND(contrlValue, $00007F00), 8));	
																	{ hi bit overrides styles 	}
		END; {with}
		contrlValue := contrlMin;									{ set value to min at init	}
		
		IF BAND(theVar, popupFixedWidth) <> popupFixedWidth THEN
		BEGIN
			WITH contrlRect DO										{ calculate cntl dimensions	}
			BEGIN
			{ set up the control's rectangle properly }
			{ ===== ===== ===== ===== ===== ===== === }
			
			{ ===== calculate control height ===== }
				GetFontInfo(myInfo);					
				WITH myInfo DO
					height := 
						ascent + 
						descent + 
						leading;
				bottom := top + height + popupSlop;
		
			{ ===== calculate control width ===== }
				IF tempJust = teJustLeft THEN
				BEGIN
					right := 
						left + theData^^.width + 
						theData^^.theMenu^^.menuWidth + popupSlop;
				END
				ELSE
				BEGIN
					left := 
						right - theData^^.width - 
						theData^^.theMenu^^.menuWidth - popupSlop;
				END;
			END;
		END;
	
		ResizeControl(theControl, theVar);					{ resize the control if necessary	}				
	
	{ handle own autotrack - pass POINTER(-1) for actionProc in TrackControl }
		contrlAction := ProcPtr(-1);
		
		HUnLock(Handle(theData));
	END;	
END;

{===============================================================================================}
FUNCTION  DoDraw(theControl : ControlHandle; theVar: INTEGER) : LONGINT;
VAR	
	theData			:	myDataH;						{ private storage 						}
	theString		:	STR255;							{ for truncating menu item being drawn 	}
	theRect			:	Rect;
	theMenu			:	MenuHandle;
	
	tempJust		:	INTEGER;						{ justification of the popup box 		}
	
	dimTitle		:	BOOLEAN;						{ booleans for drawing action 			}
	dimItem 		:	BOOLEAN;
	
	pat				: 	Pattern;						{ gray pattern - cannot use QD globals 	}
	
	myInfo			:	FontInfo;						{ font information 						}
	
	oldStyle		:	Style;							{ grafport txStyle before drawing title }
	theStyle		:	Style;							{ style for the popup title 			}
	
	result 			:	LONGINT;						{ value to pass back 					}
	
	thePoint		:	Point;
		
	theMenuColor	:	MCEntryPtr;						{ pointer to menu color entry table 	}
	oldCQDFore		:	RGBColor;
	oldCQDBack		:	RGBColor;
	
	newCQDFore		:	RGBColor;
	newCQDBack		:	RGBColor;
	
	tempMakeCPort	:	BOOLEAN;						{ do we need to create a color port? 	}
	myCPort			:	CGrafPtr;
	
	savedPort		:	GrafPtr;
	currentPort		:	GrafPtr;
	
	headerSpace		:	INTEGER;						{ space for leading check/bullet		}
	savedClip:			RgnHandle;						{ saved clipping region 				}
	newClip:			RgnHandle;						{ new clipping rgn which is intersection 
															of old clip and the control rect 	}
													
	theHilite		:	INTEGER;						{ hiliting of the control				}
	
BEGIN
	DoDraw := 0;
	
	{===== setup grafPort information =====}
	theData := myDataH(theControl^^.contrlData);		{ get the control data handle			}
	IF theData = NIL THEN
		Exit(DoDraw);
	
	IF theData^^.noPopupTrap THEN
		theHilite := 255
	ELSE
		theHilite := theControl^^.contrlHilite;			{ get the current control hilite		}
				
	GetPort(savedPort);
	tempJust := GetSysJust;								{ get system justification				}
	
	GetFontInfo(myInfo);								{ get font information 					}
	
	result := 0;										{ optimistic							}
	tempMakeCPort := theData^^.makeCPort;				{ make a copy							}
	
	headerSpace := myInfo.widMax;
		
	IF tempMakeCPort = TRUE THEN						{ do we need to make a cGrafPort?		}
	BEGIN
		myCPort := CGrafPtr(NewPtr(SIZEOF(CGrafPort)));
		IF myCPort = NIL THEN
			tempMakeCPort := FALSE						{ failed, but gamely we continue 		}
		ELSE
		BEGIN
			thePoint := theControl^^.contrlOwner^.portRect.topLeft;
			LocalToGlobal(thePoint);
			OpenCPort(myCPort);							{ NOTE port is now set to myCPort		}
			MovePortTo(thePoint.h, thePoint.v);
			WITH theControl^^, contrlOwner^, portRect DO
				PortSize(right - left, bottom - top);
			
			WITH theControl^^.contrlOwner^ DO			{ copy the important stuff 				}
			BEGIN
				myCPort^.txFont := txFont;				{ setup the world						}
				myCPort^.txSize := txSize;
				myCPort^.txFace := txFace;
				myCPort^.txMode := txMode;
				PenNormal;
				CopyRgn(clipRgn, myCPort^.clipRgn);
				CopyRgn(visRgn, myCPort^.visRgn);
			END;
		END;
	END;
	
	IF theData^^.color THEN								{ if CQD, save state					}
	BEGIN		
		GetForeColor(oldCQDFore);						{ get old fore/background colors		}
		GetBackColor(oldCQDBack);
	END;
	
	{===== resize the control if necessary =====}
	theRect := theControl^^.contrlRect;					{ erase the control rectangle			}
	EraseControl(theRect, theData, (theHilite <> 255));
	
	HLock(Handle(theData));
	ResizeControl(theControl, theVar);					{ resize the control (maybe)			}			
	HUnlock(Handle(theData));
	
	theRect := theControl^^.contrlRect;					{ erase the control rectangle			}
	EraseControl(theRect, theData, (theHilite <> 255));
	
	savedClip := NewRgn;								{ save current clipRgn 					}
	GetClip(savedClip);									{ into safe place 						}
	
	newClip := NewRgn;									{ new clip rgn 							}
	RectRgn(newClip, theRect);							{ and create it 						}
	
	SectRgn(newClip, savedClip, newClip);				{ newClip = newClip && savedClip 		}
	SetClip(newClip);
	
	{===== determine what needs to be dimmed =====}
	dimItem := FALSE;									{ dim contents of popup box				}
	dimTitle := FALSE;									{ dim popup title area					}
	
	IF (theHilite = 255) THEN 
	BEGIN 												{ control disabled						}
		dimItem := TRUE;								{ so dim both							}
		dimTitle := TRUE;
	END
	ELSE 
	BEGIN
		IF (BAND(theData^^.theMenu^^.enableFlags, $00000001) = 0) THEN 
		BEGIN
			dimItem := TRUE;							{ entire menu is disabled				}
			dimTitle := TRUE;							{ so dim both title and item			}
		END
		ELSE 
		BEGIN
			IF theControl^^.contrlValue <= 31 THEN 		{ check if item is disabled				}
				dimItem := (BAND(theData^^.theMenu^^.enableFlags,
									BitShift($00000001, theCOntrol^^.contrlValue)) = 0);
		END;
	END;

	{===== erase popup title area =====}
	HLock(Handle(theData));
	IF theData^^.color THEN								{ set up fore/back ground colors 		}
	BEGIN
		WITH newCQDFore DO								{ black foreground by default 			}
		BEGIN
			red := 0;
			green := 0;
			blue := 0;
		END;
		
		WITH newCQDBack DO								{ white background by default 			}
		BEGIN
			red := $FFFF;
			green := $FFFF;
			blue := $FFFF;
		END;

		theMenuColor := GetMCEntry(theData^^.theMenuID, 0);
		IF theMenuColor <> NIL THEN				{ got menu color table entry for this menu 		}
		BEGIN
			WITH theMenuColor^ DO
			BEGIN
				newCQDFore := mctRGB1;			{ menu title color								}
				newCQDBack := mctRGB2;			{ menu menubar color							}
			END;
		END
		ELSE
		BEGIN
			theMenuColor := GetMCEntry(0, 0);	{ get default mctb								}
			IF theMenuColor <> NIL THEN
			BEGIN
				WITH theMenuColor^ DO
				BEGIN
					newCQDFore := mctRGB1;		{ default title color							}
					newCQDBack := mctRGB4;		{ default menubar color							}
				END;
			END;
		END;
		
		RGBForeColor(newCQDFore);				{ setup colors									}
		RGBBackColor(newCQDBack);
	END;
	
	IF theHilite = myPartCode THEN
	BEGIN
		IF theData^^.color THEN
		BEGIN
			RGBForeColor(newCQDBack);			{ swap back/foreground for hilite 				}
			RGBBackColor(newCQDFore);
		END;
	END;
	
	theRect := theControl^^.contrlRect;			{ this is for CQD to work...					}
	WITH theRect DO
	BEGIN
		IF tempJust = teJustLeft THEN			{ need to erase the title area					}
		BEGIN									{ with the proper RGB fore/back					}
			right := left + theData^^.width ;	{ colors...										}			
		END
		ELSE
		BEGIN
			left := right - theData^^.width;				
		END;
		InsetRect(theRect, 0, 1);
		bottom := bottom - 1;	
	END;
	EraseRect(theRect);
	
	{===== draw popup title area =====}
	GetPort(currentPort);						{ get the current port - note that this may be 
													the control owner or may be the color 
													grafport that we created					}
													
	oldStyle := currentPort^.txFace;		 	{ save old style 								}
	WITH theData^^ DO
		TextFace(titleStyle);					{ go for bold 									}

	theRect := theControl^^.contrlRect;			{ calculate the popup title area				}
	WITH theRect DO
	BEGIN
		IF tempJust = teJustLeft THEN
		BEGIN
			right := left + theData^^.width - 4;				
			left := left + popupSlop;
		END
		ELSE
		BEGIN
			left := right - theData^^.width + 4;				
			right := right - popupSlop;
		END;
		InsetRect(theRect, 0, 1);
		bottom := bottom - 1;
		
		theString := theControl^^.contrlTitle;				{ get control title					}
		TruncateString(theString, right - left - 1, theData^^.noScriptMgr);		{ shorten		}
	END;
	TextBox( Ptr(ORD4(@theString)+1), LENGTH(theString),		{ title is drawn here			}
				theRect, theData^^.just);
				

	TextFace(oldStyle);
		
	{===== dim the title if necessary =====}
	IF dimTitle THEN BEGIN										{ dim if disabled				}
		PenMode(patBic);
		GetIndPattern(pat, sysPatListID, grayPatternIndex);		{ get 50% gray from system file	}
		PenPat(pat);
		
		theRect := theControl^^.contrlRect;						{ dim popup title				}
		InsetRect(theRect, 0, 1);
		WITH theRect DO
		BEGIN
			bottom := bottom - 1;
			IF tempJust = teJustLeft THEN
			BEGIN
				right := left + theData^^.width;
			END
			ELSE
			BEGIN
				left := right - theData^^.width;
			END;
		END;
		PaintRect(theRect);
		PenNormal;												{ set pen back to normal		}
	END;
	
	{===== hilite the title if necessary =====}
	IF theHilite = myPartCode THEN 								{ Take care of hilighting 		}
	BEGIN														{ the popup control				}
	{
		Note that for color operations, we take care of hilighting by swapping the fore and
		background colors.  For classic QuickDraw operations, we merely draw the control and
		then invert.
	}
		IF NOT theData^^.color THEN
		BEGIN
			theRect := theControl^^.contrlRect;					
	
			WITH theRect DO
			BEGIN
				IF tempJust = teJustLeft THEN
				BEGIN
					right := right - 2;
				END
				ELSE
				BEGIN
					left := left + 2;
				END;
				bottom := bottom - 1;		
			END;
			
			InsetRect(theRect, 0, 1);
			InvertRect(theRect);			
		END;
	END;

	
	{===== erase the popup box =====}
	GetItemStyle(theData^^.theMenu, theControl^^.contrlValue, theStyle);		{ get style 	}
	TextFace(theStyle);										{ to draw popup box with 			}

	IF theData^^.color THEN									{ process color (pun, eh?			}
	BEGIN
		WITH newCQDFore DO									{ default foreground color black 	}
		BEGIN
			red := 0;
			green := 0;
			blue := 0;
		END;
		
		WITH newCQDBack DO 									{ default background color white	}
		BEGIN
			red := $FFFF;
			green := $FFFF;
			blue := $FFFF;
		END;

		theMenuColor := GetMCEntry(theData^^.theMenuID, theControl^^.contrlValue);		
															{ for mctb for the menu/item used	}
		IF theMenuColor <> NIL THEN
		BEGIN									{ use the menu color table entry for this item	}
			WITH theMenuColor^ DO
			BEGIN
				newCQDFore := mctRGB2;			{ item color									}
				newCQDBack := mctRGB4;			{ item background color							}
			END;
		END
		ELSE
		BEGIN
			theMenuColor := GetMCEntry(theData^^.theMenuID, 0);	{ for mctb for the menu used	}
			IF theMenuColor <> NIL THEN
			BEGIN							{ use the menu color table entry for this menu		}
				WITH theMenuColor^ DO
				BEGIN
					newCQDFore := mctRGB3;					{ menu item color					}
					newCQDBack := mctRGB4;					{ menu item background color		}
				END;
			END
			ELSE
			BEGIN
				theMenuColor := GetMCEntry(0, 0);				{ get default mctb				}
				IF theMenuColor <> NIL THEN
				BEGIN						{ use the menu color table entry for the menubar	}
					WITH theMenuColor^ DO
					BEGIN
						newCQDFore := mctRGB3;					{ default title color			}
						newCQDBack := mctRGB2;					{ default menubar color			}
					END;
				END;
			END;
			
		END;
		
		RGBForeColor(newCQDFore);								{ setup the colors				}
		RGBBackColor(newCQDBack);
	END;

	theRect := theControl^^.contrlRect;							{ calculate popup box rect		}
	WITH theRect DO
	BEGIN
		IF tempJust = teJustLeft THEN
		BEGIN
			left := left + theData^^.width;
			right := right - (popupSlop - 1);
		END
		ELSE
		BEGIN
			right := right - theData^^.width;
			left := left + (popupSlop - 1);
		END;
		bottom := bottom - 1;
		EraseRect(theRect);					{ this is so that CQD can erase the rectangle with	}
											{ with the proper RGB fore/back colors set			}
	
	END;
	
	{===== draw the popup box contents =====}
	theRect := theControl^^.contrlRect;							{ calculate rect to draw in		}	
	WITH theRect DO
	BEGIN
		IF tempJust = teJustLeft THEN
		BEGIN
			left := left + theData^^.width;
		END
		ELSE
		BEGIN
			right := right - theData^^.width;
		END;
		
		IF tempJust = teJustLeft THEN
			BEGIN
			left := left + headerSpace - 1  ;		{ was -3 4/26/89 BBH}
			IF BAND(theVar, popupUseWFont) = popupUseWFont THEN	
				left := left + 2;
			right := right - (popupSlop + 1);
			END;
		
		IF tempJust = teJustRight THEN
			BEGIN
			left := left + popupSlop + 1;
			right := right - headerSpace;
			IF BAND(theVar, popupUseWFont) = popupUseWFont THEN	
				right := right - 2;
			OffsetRect(theRect, 2, 0);
			END;
			
		top := top + 1;
		bottom := bottom - 1;
		GetItem(theData^^.theMenu, theControl^^.contrlValue, theString);
	
		{ if not fixed width, just draw }
		IF BAND(theVar, popupFixedWidth) = popupFixedWidth THEN	
			TruncateString(theString, right - left - 1, theData^^.noScriptMgr);				
																		{ truncate if needed	}			
	END;	
	TextBox( Ptr(ORD4(@theString)+1), LENGTH(theString), theRect, GetSysJust);
	
	{===== dim the popup box contents if necessary =====}
	IF dimItem THEN BEGIN												{ dim if disabled		}
		PenMode(patBic);
		GetIndPattern(pat, sysPatListID, grayPatternIndex);				{ 50% gray from system	}
		PenPat(pat);

		theRect := theControl^^.contrlRect;								{ dim popup item		}
		WITH theRect DO
		BEGIN
			top := top + 1;
			IF tempJust = teJustLeft THEN
			BEGIN
				left := left + theData^^.width;
				right := right - 2;
			END
			ELSE
			BEGIN
				right := right - theData^^.width;
				left := left + 2;
			END;
			bottom := bottom - 2;
		END;		
		PaintRect(theRect);
		PenNormal;														{ restore the pen		}
	END;

	IF theData^^.color THEN
	BEGIN
		WITH newCQDFore DO										
		BEGIN												{ set up black foreground color		}
			red := 0;
			green := 0;
			blue := 0;
		END;
		
		WITH newCQDBack DO
		BEGIN												{ set up white background color		}
			red := $FFFF;
			green := $FFFF;
			blue := $FFFF;
		END;
		
		RGBForeColor(newCQDFore);								{ set up the colors				}
		RGBBackColor(newCQDBack);
	END;
	
	{===== draw the popup box frame =====}
	theRect := theControl^^.contrlRect;											
	WITH theRect DO
	BEGIN														{ calculate popup box frame		}
		IF tempJust = teJustLeft THEN
		BEGIN
			left := left + theData^^.width - 1;
			right := right - 2;
		END
		ELSE
		BEGIN
			right := right - theData^^.width + 1;
			left := left + 2;
		END;
		bottom := bottom - 1;
	END;	
	FrameRect(theRect);

	{===== draw the popup box frame drop shadow if necessary =====	}
	{		algorithm:	is the control disabled?					}
	{					if not, draw the drop shadow				}

	IF theHilite <> 255 THEN 
	BEGIN
		theRect := theControl^^.contrlRect;						{ draw 1 pixel drop box shadow	}
		theRect.bottom := theRect.bottom - 1;
		IF tempJust = teJustLeft THEN
		BEGIN
			WITH theRect DO
			BEGIN
				left := left + theData^^.width;
				right := right - 2;
				MoveTo(right, top+2);
				LineTo(right, bottom);
				LineTo(left+2, bottom);
			END;			
		END
		ELSE
		BEGIN
			WITH theRect DO
			BEGIN
				right := right - theData^^.width;
				left := left + popupSlop;
				InsetRect(theRect, -1, 0);					{ move right and left out 1 pixel	}
				MoveTo(right, top+popupSlop);
				LineTo(right, bottom);
				LineTo(left+popupSlop-1, bottom);
			END;
		END;
	END;

	{===== clean up our tracks =====	}
	HUnLock(Handle(theData));
	IF theData^^.color THEN
	BEGIN
		RGBForeColor(oldCQDFore);						{ restore saved fore/background colors	}
		RGBBackColor(oldCQDBack);
	END;

	SetClip(savedClip);									{ restore clipping						}
	DisposeRgn(savedClip);								{ and clean up 							}
	DisposeRgn(newClip);								{ and clean up 							}
	
	IF tempMakeCPort THEN								{ remove color grafport we created 		}
	BEGIN
		CloseCPort(myCPort);
		DisposPtr(Ptr(myCPort));
	END;
	DoDraw := result;
	SetPort(savedPort);									{ back to a safe port					}
END;

{===============================================================================================}
FUNCTION DoTest(theControl : ControlHandle; param : LONGINT) : LONGINT;
VAR
	thePoint : Point;

BEGIN
	thePoint.h := LoWord(param); 									{ in local coordinates 		}
	thePoint.v := HiWord(param);
	
	WITH theControl^^ DO
	BEGIN
		IF PtInRect(thePoint, contrlRect) THEN
			DoTest := myPartCode									{ you've tested positive!	}
		ELSE
			DoTest := 0;
		
		IF contrlHilite = 255 THEN
			DoTest := 0;
	END;
END;

{===============================================================================================}
PROCEDURE DoDispose(theControl : ControlHandle);
VAR
	theData		:	myDataH;
	
BEGIN
	theData := myDataH(theControl^^.contrlData);
	IF theData <> NIL THEN								{ sanity checking						}
	BEGIN
		HLock(Handle(theData));
		WITH theData^^ DO
		BEGIN		
{$IFC SAFEMENUS}
			IF NOT noKillMenu THEN						{ time to die…							}
			BEGIN
{$ENDC}
				DeleteMenu(theMenuID);					{ take it out of the menu list			}
				DisposeMenu(theMenu);					{ dispose of the menu handle			}
{$IFC SAFEMENUS}
			END;
{$ENDC}
		END;
		
		DisposHandle(Handle(theData));					{ dispose of internal data structure	}
	END;
END;

{===============================================================================================}
PROCEDURE DoCalc(theControl : ControlHandle; theVar: INTEGER; VAR param : LONGINT);
VAR
	theData		:	myDataH;
	
BEGIN
	WITH theControl^^ DO
	BEGIN
		theData := myDataH(contrlData);

		IF theData <> NIL THEN								{ sanity checking					}
		BEGIN
			HLock(Handle(theData));
			ResizeControl(theControl, theVar);				{ try to resize control if needed	}
			HUnlock(Handle(theData));
		END;
	
		HidePen;
		OpenRgn;
			FrameRect(contrlRect);
		CloseRgn(RgnHandle(param));
		ShowPen;
	END;	
END;

{===============================================================================================}
FUNCTION  DoTrack(theControl : ControlHandle; theVar: INTEGER) : LONGINT;
VAR
	theData 	:	myDataH;
	thePoint	:	Point;
	result 		:	LONGINT;
	tempJust	:	INTEGER;
	savedText	:	TextInfo;
	theChar		:	INTEGER;
	
BEGIN
	WITH theControl^^ DO
	BEGIN
		result := 0;
		tempJust := GetSysJust;								{ get current system justification	}
		theData := myDataH(contrlData);
		IF theData = NIL THEN								{ sanity checking					}
			Exit(DoTrack);
			
		HLock(Handle(theData));								{ where do we pop up the menu?		}
		
		ResizeControl(theControl, theVar);					{ resize the control if necessary	}
			
		thePoint.v := contrlRect.top + 1;
		IF tempJust = teJustLeft THEN
		BEGIN
			thePoint.h := contrlRect.left + theData^^.width;
		END
		ELSE
		BEGIN
			thePoint.h := contrlRect.right - theData^^.width -
				theData^^.theMenu^^.menuWidth;
		END;
		LocalToGlobal(thePoint);							{ change to global coordinates		}
		
		{ if the bit's set, use the window font info for the menus	}
		IF BAND(theVar, popupUseWFont) = popupUseWFont THEN
		BEGIN
			SaveWMPortText(savedText, theData^^.hasColor);			{ save wmgr port font/size	}
			SetWMPortText(contrlOwner, theData^^.hasColor);			{ set wmgr port font/size	}
			SetItemMark(theData^^.theMenu,contrlValue,chr(BulletMark));			{ use bullet •	}
		END
		ELSE
		BEGIN
			SetItemMark(theData^^.theMenu,contrlValue,chr(checkMark));			{ use checkmark	}
		END;
		
		IF theData^^.noPopupTrap THEN								{ don't crash please		}
			result := 0
		ELSE
			result := PopUpMenuSelect(theData^^.theMenu, thePoint.v, 
						thePoint.h, theControl^^.contrlValue);		{ popping					}
			
		CheckItem(theData^^.theMenu, contrlValue, FALSE);			{ uncheck the item 			}
		
		IF BAND(theVar, popupUseWFont) = popupUseWFont THEN
			ResetWMPortText(savedText, theData^^.hasColor);			{ reset wmgr port font/size	}
		
		tempJust := HiWord(result);									{ was anything selected? 	}
		IF tempJust = 0 THEN										{ nothing selected 			}
		BEGIN
			result := 0;											{ so return nothing 		}
		END
		ELSE
		BEGIN
			contrlValue := LoWord(result);							{ set control value 		}
			result := myPartCode;									{ return inControl 			}
		END;
				
		HUnLock(Handle(theData));
		DoTrack := result;
	END;
END;

{===============================================================================================}
{===============================================================================================}
{
		E N D   O F   E X T E R N A L L Y   A C C E S S I B L E   R O U T I N E S
}
{===============================================================================================}
{===============================================================================================}
{===============================================================================================}

{===============================================================================================}
{
	This routine will resize the popup control as necessary if the appropriate variation code
		is set.
}
PROCEDURE  ResizeControl(theControl : ControlHandle; theVar: INTEGER);
VAR	
	theData			:	myDataH;					{ private storage 							}
	savedText		:	TextInfo;					{ save grafport context 					}

	myInfo			:	FontInfo;
	height			:	INTEGER;					{ control height 							}
	
BEGIN
	theData := myDataH(theControl^^.contrlData);	{ get the control data handle				}
	
	IF BAND(theVar, popupUseWFont) = popupUseWFont THEN	{ use window font info varCode			}
	BEGIN
		SaveWMPortText(savedText, theData^^.hasColor);	{ save window manager port font/size	}
		SetWMPortText(theControl^^.contrlOwner, theData^^.hasColor);	{ set to owner's font	}
		PreflightFont;								{ uncache previous font manager font info	}
	END;
	
	CalcMenuSize(theData^^.theMenu);				{ recalculate menusize						}
	GetFontInfo(myInfo);							{ get current font information 				}
	WITH myInfo DO
		height := 									{ recalculate control height				}
			ascent + 								{ ALWAYS 									}
			descent + 
			leading;
			
	WITH theControl^^ DO
		WITH contrlRect DO							{ recalculate control width					}
			bottom := 								{ bottom = top + height + slop 				}
				top + height + popupSlop;
	
	IF BAND(theVar, popupFixedWidth) <> popupFixedWidth THEN	{ aha - resize the control...	}
	BEGIN
		WITH theControl^^ DO
		BEGIN
			WITH contrlRect DO						{ recalculate control width					}
			BEGIN	
				bottom := 							{ bottom = top + height + slop 				}
					top + height + popupSlop;
 
				IF GetSysJust = teJustLeft THEN
				BEGIN
					right := left + theData^^.width + theData^^.theMenu^^.menuWidth + popupSlop;
				END
				ELSE
				BEGIN
					left := right - theData^^.width - theData^^.theMenu^^.menuWidth - popupSlop;
				END
			END;			
		END;
	END;

	IF BAND(theVar, popupUseWFont) = popupUseWFont THEN
	BEGIN
		ResetWMPortText(savedText, theData^^.hasColor);		{ restore window mgr port font/size	}
		PreflightFont;										{ void the font manager cache		}
	END;
END;

{===============================================================================================}
{
	This routine will set the window manager port font and size to match the txFont and txSize
	fields of the grafPort referenced by theWindow.  
	
	Also the low memory global locations for sysFontFam and sysFontSize are stuffed with
	the txFont and txSize fields of theWindow's grafPort.
	
	If color is TRUE, the same operations that were performed on the window manager port
	are performed on the color window manager port.
}
PROCEDURE SetWMPortText(theWindow:	WindowPtr; color: BOOLEAN);	
VAR
	wmgrPort:	GrafPtr;
	wmgrCPort:	CGrafPtr;
	savedPort:	GrafPtr;

BEGIN	
	WITH theWindow^ DO
	BEGIN
{	Starting life again by playing with low memory }
		IntPtr(sysFontFam)^ := txFont;								{ stuff the sys font #		}
		IntPtr(sysFontSize)^ := txSize;								{ stuff the sys font size	}
		
		GetPort(savedPort);
		GetWMgrPort(wmgrPort);
		SetPort(wmgrPort);
	
		TextSize(txSize);											{ fuck with the wmgr port	}
		TextFont(txFont);
	
		IF color THEN
		BEGIN
			GetCWMgrPort(wmgrCPort);
			SetPort(GrafPtr(wmgrCPort));							{ fuck with c wmgr port		}
			TextSize(txSize);
			TextFont(txFont);
		END;
	END;
	SetPort(savedPort);
END;


{===============================================================================================}
{
	This routine will save the txFont and txSize fields from the window manager port
	as well as the values in the low memory locations for sysFontFam and sysFontNum.
	
	If color is TRUE, the txFont and txSize fields of the color window manager port
	are saved also.
}
PROCEDURE SaveWMPortText(VAR theTextInfo: TextInfo; color: BOOLEAN);
VAR
	wmgrPort	:	GrafPtr;
	wmgrCPort	:	CGrafPtr;
	
BEGIN
	with theTextInfo do begin
		SSFont := IntPtr(SysFontFam)^;
		SSSize := IntPtr(SysFontSize)^;
		
		GetWMgrPort(wmgrPort);
		wmgrSize := wmgrPort^.txSize;
		wmgrFont := wmgrPort^.txFont;
		
		IF color THEN
		BEGIN
			GetCWMgrPort(wmgrCPort);
			wmgrCSize := wmgrCPort^.txSize;
			wmgrCFont := wmgrCPort^.txFont;
		END;
	end; { with }
END;

{===============================================================================================}
{
	This routine restore save the txFont and txSize fields from the window manager port
	as well as the values in the low memory locations for sysFontFam and sysFontNum.
	
	If color is TRUE, the txFont and txSize fields of the color window manager port
	are restored also.
}
PROCEDURE ResetWMPortText(theTextInfo: TextInfo; color: BOOLEAN);
VAR
	wmgrPort	:	GrafPtr;
	wmgrCPort	:	CGrafPtr;
	savedPort	:	GrafPtr;
	
	
BEGIN	
	with theTextInfo do begin
		IntPtr(SysFontFam)^ := SSFont;
		IntPtr(SysFontSize)^ := SSSize;

		GetWMgrPort(wmgrPort);
		GetPort(savedPort);
		SetPort(wmgrPort);
		TextSize(wmgrSize);
		TextFont(wmgrFont);
		
		IF color THEN
		BEGIN
			GetCWMgrPort(wmgrCPort);
			SetPort(GrafPtr(wmgrCPort));
			TextFont(wmgrCFont);
			TextSize(wmgrCSize);
		END;
		SetPort(savedPort);
		
	end; { with }
END;

{===============================================================================================}
{
	This routine will truncate a given string to be within the given width, appending an
		ellipsis if truncation is necessary
}
PROCEDURE TruncateString(VAR theString : STR255; width : INTEGER; noSM: BOOLEAN);
VAR
	newLength : INTEGER;
	offset:		INTEGER;
	
	charType:	INTEGER;											{ type of char to strip 	}
	deltaL:		INTEGER;											{ amount to strip 			}
	
BEGIN
	IF width <=0 THEN												{ sanity chex				}
	BEGIN
		theString := '';
		Exit(TruncateString);
	END;

	IF StringWidth(theString) > width THEN						{ do we need to shorten?	}
	BEGIN			
		width := width - CharWidth('…');						{ leave space for ellipsis 	}
		IF width < 0 THEN										{ no room for ellipsis		}
			width := 0;
		
		newLength := LENGTH(theString);							{ length of string			}
		WHILE StringWidth(theString) > width DO					{ script mgr should be 		}
		BEGIN													{ patched into StringWidth	}
		
			IF noSM THEN
				charType := smSingleByte						{ no SM, no multibyte char	}
			ELSE
				charType := CharByte( Ptr(@theString), newLength );

			CASE charType OF
				smFirstByte, smSingleByte:  
					deltaL := 1;								{ default strip 1 byte	 	}
				smLastByte, smMiddleByte:  
					BEGIN
						deltaL := 1;
						{ strip until the first byte of the multibyte char 	}
						WHILE (charType <> smFirstByte) AND (deltaL < newLength) DO
						BEGIN
							charType := CharByte(Ptr(@theString), newLength-deltaL);
							deltaL := deltaL + 1;
						END;
					END;
				OTHERWISE
					deltaL := 1;								{ default for sanity  		}
			END; {case}
			
			newLength := newLength - deltaL;					{ lets take her down		}
			IF newLength < 0 THEN
				newLength := 0;
				
			theString[0] := CHAR(newLength);					{ and shorten the string	}
		END;
		
		IF LENGTH(theString) <> 0 THEN							{ need at least one char to	}
			theString := CONCAT(theString, '…');				{ append the ellipsis		}
	END;
END;
	
{===============================================================================================}
{
	This routine blows away the font manager font cache		
}
PROCEDURE PreflightFont;			{ this is needed to insure that the MDEF proc somehow gets the
										correct font information when it does a StringWidth in the
										call to CalcMenuSize									}
BEGIN
{	this flushes the font manager cache on font 0 size 0 }
{	so that the changes we have made will cause results from }
{	StringWidth etc etc etc to reflect the new system font being used }

	IF LongPtr(CurFMInput)^ = 0 THEN
		LongPtr(CurFMInput)^ := $FFFFFFFF;
END;

{===============================================================================================}
{
	This routine will erase the control (popup box area, popup title area, and popup box drop
		shadow area
}
PROCEDURE EraseControl(theRect: Rect; theData: myDataH; enabled: BOOLEAN);
VAR
	tempRect:	Rect;
	tempJust:	INTEGER;
	
BEGIN
	tempJust := GetSysJust;
	
	tempRect := theRect;								{ erase popup control title area 	}
	WITH tempRect DO
	BEGIN
		top := top + 1 ;
		bottom := bottom - 3;
		
		IF tempJust = teJustLeft THEN
		BEGIN
			right := left + theData^^.width;
		END
		ELSE
		BEGIN
			left := right - theData^^.width;
		END;
	END; {with}
	EraseRect(tempRect);
	
	tempRect := theRect;								{ erase popup box 					}
	WITH tempRect DO
	BEGIN
		top := top ;
		bottom := bottom - 3;
		
		IF tempJust = teJustLeft THEN
		BEGIN
			left := left + theData^^.width;
			right := right - 2;
		END
		ELSE
		BEGIN
			right := right - theData^^.width;
			left := left + 2;
		END;
	END; {with}
	EraseRect(tempRect);
	
	IF Enabled THEN										{ now erase the drop shadow area 	}
	BEGIN												{ only if control is enabled		}
		tempRect := theRect;
		WITH tempRect DO
		BEGIN
			top := top + 2;
			IF tempJust = teJustLeft THEN
			BEGIN
				left := left + theData^^.width;
				right := right - 1;
			END
			ELSE
			BEGIN
				right := right - theData^^.width;
				right := right + 1;
				left := left + 2;
			END;
			
			left := left + 2;	
		END; {with}
		EraseRect(tempRect);
	END; {if}
END;
END. { of CDEF }

