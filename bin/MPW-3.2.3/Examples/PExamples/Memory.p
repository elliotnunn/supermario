{
 File Memory.p

 Copyright Apple Computer, Inc. 1985-1988
 All rights reserved.
}

{$D+} { MacsBug symbols on }
{$R-} { No range checking  }

UNIT Memory;

INTERFACE

USES
	MemTypes, QuickDraw, OSIntf, ToolIntf, PackIntf;


FUNCTION DRVROpen		(ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;
FUNCTION DRVRControl	(ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;
FUNCTION DRVRStatus 	(ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;
FUNCTION DRVRPrime		(ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;
FUNCTION DRVRClose		(ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;
			

IMPLEMENTATION

TYPE
	EventPtr = ^EventRecord;
	trix =	RECORD			{ needed for some coercive behavior }
		CASE Boolean of
			TRUE:	(FayWray: ARRAY[0..10] OF Integer);
			FALSE:	(Away:	EventPtr);
		END;


FUNCTION RsrcID(dCtl: DCtlPtr): Integer;
BEGIN
	RsrcID := (BOR($C000,(BSL(BNOT(dCtl^.dCtlRefNum),5))));
END;


FUNCTION DRVROpen(ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;
VAR 	
	SavePort:	GrafPtr;
	heapGrow:	LongInt;
	wRect:		Rect;
	myWindow:	WindowPtr;
BEGIN
	IF dCtl^.dCtlWindow = NIL THEN
	BEGIN
		GetPort (SavePort);
			myWindow := GetNewWindow(RsrcID(dCtl),nil,POINTER(-1));
			windowpeek(myWindow)^.WindowKind := dCtl^.dCtlRefNum;  { show a DA owns this window }
		dCtl^.dCtlWindow := myWindow;	{ let the desk manager know too }
		heapGrow := MaxMem (heapGrow);
		SetPort (SavePort);
	END;
	DRVROpen := NOErr;
END;


FUNCTION DRVRClose(ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;
BEGIN
	IF dCtl^.dCtlwindow <> NIL THEN
	BEGIN
		DisposeWindow (WindowPtr(dCtl^.dCtlWindow));
		dCtl^.dCtlWindow := NIL;
	END;
	DRVRClose := NOErr;
END;


FUNCTION DRVRControl(ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;

	PROCEDURE DrawWindow;
	VAR 
		saveZone:			THz;
		tempStr,VolName:	Str255;
		freeBytes:			LongInt;
		ourID:				Integer;

		PROCEDURE PrintNum (num:LongInt);	{outputs the number in plain text}
		VAR 
			outStr: Str255;
		BEGIN
			NumToString (num,outStr);
			TextFace ([]);					{ the empty set = Plain }
			DrawString (outStr);
			TextFace ([Bold]);
		END;
	
		PROCEDURE GetVolStuff;	
		VAR
			error:	OsErr;
			myParamBlk: HParamBlockRec;
		BEGIN
			WITH myParamBlk DO			
				BEGIN
					ioNamePtr := @VolName;
					ioVRefNum := 0; 		{ if ioVRefNum and ioVolIndex are zero, }
					ioVolIndex := 0;		{ go for the default volume. }
				END;
			error := PBHGetVInfo (@myParamBlk, FALSE);
			
			{ ioVFrBlk is an unsigned integer.  If > 32767 and assigned
			  to freeBytes (a LongInt), Pascal will think it is negative
			  and sign extend it.  The expression below masks off this
			  high word so that freeBytes is correctly signed.  See
			  tech note #157 more a more in-depth explanation. }
			freeBytes := myParamBlk.ioVFrBlk;
			freeBytes := BAND(freeBytes, $0000ffff) * myParamBlk.ioVAlBlkSiz;
		END;

		PROCEDURE PrtRsrcStr(index: integer);
		BEGIN
			GetIndString(tempStr,ourID,index);
			DrawString(tempStr);
		END;

	BEGIN	{ DrawWindow }
		ourID := RsrcID(dCtl);
		
		TextMode (SrcCopy); TextFont (Monaco); TextSize (9); TextFace ([Bold]);

		MoveTo (6,10); PrtRsrcStr(1);	{ "AppHeap: " }
		SaveZone := GetZone;
		SetZone (ApplicZone);
		PrintNum (FreeMem); 
		
		PrtRsrcStr(2);	{ " SysHeap: " }
		SetZone (SystemZone);
		PrintNum (FreeMem);
		SetZone (SaveZone); 		{ always put things back the way you found them }

		PrtRsrcStr(3);		{ " Disk: " }
		GetVolStuff;
		PrintNum (FreeBytes);

		PrtRsrcStr(4);		{ " free on " }
		TextFace ([UnderLine]);
		DrawString (VolName);
	END;	{ DrawWindow }

VAR
	eventAt:	EventPtr;	{ Pointer to our event }
	
BEGIN { of DRVRControl }
	SetPort(GrafPtr(dCtl^.dCtlWindow)); { the desk manager restores thePort}
	CASE ctlPB^.csCode OF
		accEvent:	
			BEGIN
				eventAt := trix(ctlPB^.CSParam).Away;	{ get the event pointer }
				WITH eventAt^ do
				BEGIN
					IF eventAt^.what = UpDateEvt THEN { we only handle one event }
						BEGIN
							BeginUpdate (WindowPtr(eventAt^.message));
								DrawWindow;
							EndUpdate (WindowPtr(eventAt^.message));
						END
				END; { of accEvent Case }
			END;

		accRun: 	
			BEGIN			{ our periodic call }
				DrawWindow;
			END;

	END; { of CASE }
	DRVRControl := NoErr;
END; { of DRVRControl }


FUNCTION DRVRPrime (ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;
BEGIN
	DRVRPrime := NoErr;
END;

FUNCTION DRVRStatus (ctlPB: ParmBlkPtr; dCtl: DCtlPtr): OSErr;
BEGIN
	DRVRStatus := NoErr;
END;


END. {of memory UNIT}

