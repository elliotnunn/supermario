{
	File:		BitEdit.i

	Written by:	Steve Horowitz, Peter Alley, and Craig Carper

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	  2/5/90	dba		get rid of BitMapPtr, since it is now in QuickDraw.p
		 <1>	 1/22/90	JRM		declare BitmapPtr as a type

	To Do:
}

{ BitEdit IMPLEMENTATION section }

{ 8-18-87 }
{ version 2.0A1 10-29-89 by Craig Carper }
{ written by Steve Horowitz; further development by Craig Carper }

{  CHANGES
		• (01-03-90 crc)	BEBounds: Added LockHands, UnlockHands.
			DrawLasso: Renamed local variable selRect to selBBox to avoid conflict with field name in
				BESelectRect.
			This file was changed to provide the implementation section only; it is included by the
				interface file BitEdit.p.
			Renamed fields of BitEditRecord to those used in interface file.
		• (12-12-89 crc)	BEInvalRect: Limit argument rectangle to it's intersection with rEdit.  BEInvalRect is
				called internally with this argument set to the selection, part of which may lie outside
				rEdit--causing an invalid update of the views.
			DragSelection: If dragging a copy, set the wasErased flag to true without erasing the original.
		• (12-05-89 crc)	BEObject: Eliminated "idle" and "select" parameter.  "select" was only used to prevent
			making an undo copy when dragging out a selection--this is inconsistent with other functions, and
			does not appear to introduce any anomilies.  "idle" was folded into the "flags" parameter.
			BEPaste: Changed so that if pasting into a moved selection, the selection is replaced without
				first putting it down.
			BESelect: Fixed to always save undo state before creating a new selection
		• (12-04-89 crc)	Many changes so Undo restores selections:
			BESetupUndo: Renamed SetupUndo, which is not exported.
			BEUndoBegin,BEUndoEnd: Replacements for BESetupUndo to allow freezing the
				undo state across multiple calls to the BitEdit package.  These calls are counting, so
				if nested, undo is not re-enabled untill first BEUndoBegin is matched by a BEUndoEnd.
		• (12-01-89 crc)	Collected all selection variables into a record structure.
		• (11-30-89 crc)	Implemented fShapeUnbounded in OvalDraw, RectDraw, and RndRectDraw.
		• (11-29-89 crc)
			IMPORTANT: These changes result in a 2.0 package that is INCOMPATABLE with the 1.0 version;
				some existing routines were eliminated, others had changes to their parameter list.  In any
				event, all clients must relink since this version of the package is INCOMPATABLE with
				previous versions of BEPackLib.a.
			BEPaintBucket: Fills with current pattern; non-color support (untested).
			BERotate: Fix for non-color support (untested).
			BESetupOffScreen: Renamed to BESetupUndo.
			BEScroll: Deleted (never implemented).
			BELasso: Deleted (subsumed by BESelect).
			BEDiagonalLine, BEFillOval, BEFillRect, BEFillRndRect, BECircle, BEFillCircle, BESquare,
			BEFillSquare, BERndSquare, BEFillRndSquare: Deleted (subsumed by other routines).
			BELine, BEOval, BERec, BEObject: Added flags parameter.
			BERndRect: Renamed BERoundRect, added flags parameter, moved BitEditHandle parameter to end of parameter list.
			BEFlipSelection, BERotateSelection, BENudgeSelection, BEDupSelection:
				Renamed to BEFlip, BERotate, BENudge, and BEDuplicate, respectively.
			BESelect: Added flags parameter.
			BERect: Added refCon field.
			BEPutSelBits: Renamed to BEPutSelection;
		• (11-26-89 crc)	BEColor, BEPaintBucket: New routines.
		• (11-01-89 crc)	BENudgeSelection: always draws image without ants, so caller can always copy
				the image without picking up the ants.
			BEObject: Invalidate pattern before and after drawing into the image, in case client uses
				the same pattern to draw in a different GDevice.
		• (10-31-89 crc)	CenterScrap: changed to align rects larger than the edit image with the top
				left; this allows pasting of larger PICTs.
			BEPaste: Expand clip rgn & vis rgn to bounds of selection before drawing PICT;
				eliminated redundant code to erase selection bits after creating a selection.
		• (10-30-89 crc)  BEPutSelBits: eliminated useless call to SetupRects.
				??? Is invalid for BEInvalRect to be called between LockHands and UnlockHands ???
			BEDupSelection: new routine.
			ConstrainNudge: new routine.
		• (10-29-89 crc)	MarqueeDance: Called by BEIdle in place of MarqueeDraw; allows selRect to remain
				partially outside the edit area (for rotate).  Uses selRect instead of selection rectangle,
				and allows ants to march out of the edit area.
			BESetSelect: Constrain argument rectangle to editRect, then further constrain the bottom right
				of selRect to be one pixel inside the bottom right of editRect.  Also, moved call to
				BEKillSelection ahead of DoQuickPortToDup, so ants are not copied.
			DragSelection: Allow marquee selection to be dragged beyond edge of edit rectangle as long as
				one edge of selection remains over the edit rectangle.
			SetupRects, SetUpSelBits: Undid change described below, as this caused partially offscreen
				selections to be scaled when copied to image.
		• (10-28-89 crc)	EraseUnderSelection: new routine, called by DragSelection, BENudgeSelection, and
				BERotateSelection.
			BERotateSelection, GetPix, SetPix, SetupExtract: new routines.
		• (10-27-89 crc)	SetupRects, SetUpSelBits: After calling StretchRect, intersect with bounds rect
				of bitmap so any selection established by BESetSelect remains withing the bitmap.
			BEFlipSelection:  New routine.
		• (10-02-89 crc)	BEDraw: Call BWOffScreen if offHandle is Nil.
				When drawing directly to the screen, don't need to lock & unlock offHandle.
			BEView: Moved initialization of offPix to BENew; eliminated call to ResetOffScreen
				since BEDraw will call it if necessary.
		• ( 9-29-89 crc)	BEActivate: On deactivate, discard offscreen bits & pixMap.  This significantly
				reduces the amount of memory used by the BitEdit record.
		• ( 9-23-89 crc)	BESquare, BEFillSquare, BERndSquare, BEFillRndSquare, BECircle,
				BEFillCircle, BEDiagonalLine: New exported routines.
			SquareRect, ClipAnchorRect, SquareDraw, FillSquareDraw, RndSquareDraw,
				FillRndSquareDraw, CircleDraw, FillCircleDraw, DiagonalLineDraw, WidthHeightRect: New
				routines.
			RectDraw, FillRectDraw, RndRectDraw, FillRndRectDraw, OvalDraw, FillOvalDraw: Constrained
				to keep within edit area.
		• ( 7-24-89 crc)	BEDraw: Constructs offscreen pixMap based on deepest device, not on the
			window's port (which always reflects the main screen).
			BEView: Do not initialize offPix (requires knowing deepest device).
			BWOffscreen: Eliminated GrafPort^ parameter (not used).
			ResetOffscreen: Takes GDHandle instead of a GrafPort.
		• ( 7-17-89 crc)	Note: The BitEdit package assumes that only PixMaps are edited in color
			grafports.  The package will fail if a BitMap is passed to BENew when the current port is
			a color graf port.  The package looks into the port's BitMap to find the bounds, for example,
			without checking to see if the port contains a PixMap instead.  This is probably due to the
			fact that BitEdit was first a BitMap only editor, and color was added later.
		• ( 5-31-89 crc)	Changed MyDisposPixMap to take a PixMapHandle instead of a VAR PixMapHandle,
			which bad news if the argument lies within an unlocked relocatable object.
			Changed GetBWTable parameter to not be VAR since it was not necessary.
		• ( 5-24-89 crc)	BEDraw calls ResetOffscreen if the port's color table has changed, which means
			that the inverse table no longer corresponds to the color table in offPix.
		• (crc)  Added ImageCt2DeviceCt to create a device color table from an image color table.
			BENew was incorrectly installing an image color table as a device color table.
		• (crc)  Added the internal routine CopyPixels for transferring pixels to off-screen bitmaps.
			This is to get around a bug (fixed in 32-bit QD) that causes CopyBits to colorize pixels.
	(Peter Alley)
		• Added CopySetToPort to BEPaste
		• Added BESetupOffScreen to the end of BENew so that undo before cut, copy or clear
			will not put garbage into the bitmap.
		• Added BESelection routine 8/5/88.
 }

{USES Memtypes, Quickdraw, OSIntf, ToolIntf, PackIntf}

CONST
	antSpeed = 6;

	{ Bit flags for shape drawing }
	fShapeUnframed = $1;	{ INTERNAL USE ONLY }

	{ Bit flags for MarqueeDraw }
	fUseSelection			= $00;	{ Keep the current selection rectangle }
	fSetSelection			= $01;	{ Set the selection rectangle }

	{ Bit flags interpreted by BEObject }
	fIdle						= $8000;

PROCEDURE FixRect(VAR r: Rect); EXTERNAL;
PROCEDURE GetSelPat(patIndex: INTEGER; VAR pat: Pattern); EXTERNAL;
PROCEDURE PaintBits(blacken, erase: BOOLEAN; stPt, endPt: Point; hbe: BEHandle); FORWARD;
PROCEDURE SetUpSelBits(r: Rect; hbe: BEHandle); FORWARD;
PROCEDURE SetupUndo(hbe: BEHandle); FORWARD;
PROCEDURE MarqueeDraw(r: Rect; flags: BEDrawFlags; hbe: BEHandle); FORWARD;
PROCEDURE DrawLasso(startPt: Point; hbe: BEHandle); FORWARD;
PROCEDURE DragSelection(startPt: Point; flags: BESelectFlags; hbe: BEHandle); FORWARD;
PROCEDURE AlignBits(hbe: BEHandle); FORWARD;
PROCEDURE CenterScrap(VAR r:Rect; hbe: BEHandle); FORWARD;
PROCEDURE HiliteLasso(hbe: BEHandle); FORWARD;
PROCEDURE TrimBBox(dstBuf: QDPtr; VAR dstRect: Rect; rowB: INTEGER); EXTERNAL;
PROCEDURE CalcEdges(srcBuf,dstBuf: QDPtr;
		        topVert,height,rowBytes: INTEGER;
		        lassoBlack: BOOLEAN); EXTERNAL;
PROCEDURE ClearHandle(block: Handle); EXTERNAL;
PROCEDURE LockHands(hbe: BEHandle); FORWARD;
PROCEDURE UnlockHands(hbe: BEHandle); FORWARD;
PROCEDURE SetupRects(VAR r1, r2: Rect; hbe: BEHandle); FORWARD;
FUNCTION  GetRgnMax: INTEGER; EXTERNAL;
FUNCTION	 NewPtrClr(logicalSize: Size): Ptr; EXTERNAL;
PROCEDURE ClipAnchorRect (VAR r: Rect; VAR rClip: Rect); FORWARD;
PROCEDURE SquareRect (VAR r: Rect); FORWARD;
PROCEDURE WidthHeightRect (VAR r: Rect; VAR pt: Point); FORWARD;
FUNCTION  UIntMul(i,j: INTEGER): LONGINT; INLINE
														{ move	(sp)+,d0 } $301F,
														{ mulu	(sp)+,d0 } $C0DF,
														{ move.l	d0,(sp)  } $2E80;
PROCEDURE CallDrawProc(r:Rect; flags: BEDrawFlags; hbe: BEHandle; proc: ProcPtr); INLINE
														{ move.l	(sp)+,a0 } $205F,
														{ jsr	(a0) } $4E90;

{ ---- Begin BitEdit Routines ---- }

PROCEDURE StretchRect(VAR r: Rect);

BEGIN
	r.right := r.right + 1;
	r.bottom := r.bottom + 1;
END;


PROCEDURE LockHands(hbe: BEHandle);
{ Lock down the handles to the bits and set the baseAddr field }
{ of the bitMap or pixMap (lock it if it's a pixMap) }

BEGIN
	HLock(Handle(hbe));

	WITH hbe^^ DO
		BEGIN
		IF dupHandle <> Nil THEN
			BEGIN
			HLock(dupHandle);
			IF isColor THEN
			{ lock the pixMap handle }
				BEGIN
				HLock(Handle(dupPix));
				dupPix^^.baseAddr := dupHandle^;
				END
			ELSE
				dupBits.baseAddr := dupHandle^;
			END;
		END; { WITH hbe^^ }

	WITH hbe^^.selection DO
		BEGIN
		IF selHandle <> Nil THEN
			BEGIN
			HLock(selHandle);
			IF hbe^^.isColor THEN
			{ lock the pixMap handle }
				BEGIN
				HLock(Handle(selPix));
				selPix^^.baseAddr := selHandle^;
				END
			ELSE
				selBits.baseAddr := selHandle^;
			END;

		IF maskHandle <> Nil THEN
			BEGIN
			HLock(maskHandle);
			IF hbe^^.isColor THEN
			{ lock the pixMap handle }
				BEGIN
				HLock(Handle(maskPix));
				maskPix^^.baseAddr := maskHandle^;
				END
			ELSE
				maskBits.baseAddr := maskHandle^;
			END;
		END; { WITH hbe^^.selection }
END; { LockHands }


PROCEDURE UnlockHands(hbe: BEHandle);
{ unlock the handles to the Bits and unlock the pixMap handle if we're in color }

BEGIN
	HUnlock(Handle(hbe));

	WITH hbe^^ DO
		BEGIN
		IF dupHandle <> Nil THEN
			BEGIN
			HUnlock(dupHandle);
			IF isColor THEN
				HUnlock(Handle(dupPix));
			END;
		END; { WITH hbe^^ }

	WITH hbe^^.selection DO
		BEGIN
		IF selHandle <> Nil THEN
			BEGIN
			HUnlock(selHandle);
			IF hbe^^.isColor THEN
				HUnlock(Handle(selPix));
			END;

		IF maskHandle <> Nil THEN
			BEGIN
			HUnlock(maskHandle);
			IF hbe^^.isColor THEN
				HUnlock(Handle(maskPix));
			END;
		END; { WITH hbe^^.selection }
END; { UnlockHands }


PROCEDURE SetupRects(VAR r1, r2: Rect; hbe: BEHandle);

VAR
	b: BOOLEAN;

BEGIN
	WITH hbe^^, hbe^^.selection DO
		BEGIN
		r1 := selRect;
		StretchRect(r1);

		IF maskHandle <> Nil THEN
			BEGIN
			IF isColor THEN
				r2 := maskPix^^.bounds
			ELSE
				r2 := maskBits.bounds;
			InsetRect(r2, 1, 1);
			END;
		END;
END; { SetupRects }


PROCEDURE GetMIndex(VAR mIndex: INTEGER);

VAR
	now: INTEGER;

BEGIN
	now := LoWrd(TickCount);
	now := now DIV antSpeed;
	mIndex := BAnd(now, 7);
END;

{ cc }
PROCEDURE CopyPixels (hpmSource, hpmDest: PixMapHandle);

BEGIN
	BlockMove (hpmSource^^.baseAddr, hpmDest^^.baseAddr,
				 INTEGER(BAND(hpmDest^^.rowBytes, $7FFF))*(hpmDest^^.bounds.bottom-hpmDest^^.bounds.top));
END;

PROCEDURE CSelToPort(hbe: BEHandle; r1: Rect; isDrag: BOOLEAN);

VAR
	r, srcRect: Rect;

BEGIN
	SetupRects(r, srcRect, hbe);
	IF isDrag THEN
		r := r1;
	WITH hbe^^ DO
		IF isColor THEN
			CopyBits(BitMapPtr(selection.selPix^)^, beGrafPtr^.portBits, selection.selPix^^.bounds, r, srcCopy, Nil)
		ELSE
			CopyBits(selection.selBits, beGrafPtr^.portBits, selection.selBits.bounds, r, srcCopy, Nil);
END;


PROCEDURE OSelToPort(hbe: BEHandle; r1: Rect; isDrag: BOOLEAN);

VAR
	r, srcRect: Rect;

BEGIN
	SetupRects(r, srcRect, hbe);
	IF isDrag THEN
		r := r1;
	WITH hbe^^ DO
		IF isColor THEN
			CopyBits(BitMapPtr(selection.selPix^)^, beGrafPtr^.portBits, srcRect, r, srcOr, Nil)
		ELSE
			CopyBits(selection.selBits, beGrafPtr^.portBits, srcRect, r, srcOr, Nil);
END;


PROCEDURE MaskToPort(hbe: BEHandle; r1: Rect; isDrag: BOOLEAN);

VAR
	r, srcRect: Rect;

BEGIN
	SetupRects(r, srcRect, hbe);
	IF isDrag THEN
		r := r1;
	WITH hbe^^ DO
		IF isColor THEN
			CopyBits(BitMapPtr(selection.maskPix^)^, beGrafPtr^.portBits, srcRect, r, srcBic, Nil)
		ELSE
			CopyBits(selection.maskBits, beGrafPtr^.portBits, srcRect, r, srcBic, Nil);
END;


PROCEDURE CSelToDup(hbe: BEHandle);

VAR
	r, srcRect: Rect;

BEGIN
	SetupRects(r, srcRect, hbe);
	WITH hbe^^ DO
		IF isColor THEN
			CopyBits(BitMapPtr(selection.selPix^)^, BitMapPtr(dupPix^)^, selection.selPix^^.bounds, r,
						srcCopy, Nil)
		ELSE
			CopyBits(selection.selBits, dupBits, selection.selBits.bounds, r, srcCopy, Nil);
END;


PROCEDURE OSelToDup(hbe: BEHandle);

VAR
	r, srcRect: Rect;

BEGIN
	SetupRects(r, srcRect, hbe);
	WITH hbe^^ DO
		IF isColor THEN
			CopyBits(BitMapPtr(selection.selPix^)^, BitMapPtr(dupPix^)^, srcRect, r, srcOr, Nil)
		ELSE
			CopyBits(selection.selBits, dupBits, srcRect, r, srcOr, Nil);
END;


PROCEDURE MaskToDup(hbe: BEHandle);

VAR
	r, srcRect: Rect;

BEGIN
	SetupRects(r, srcRect, hbe);
		WITH hbe^^ DO
		IF isColor THEN
			CopyBits(BitMapPtr(selection.maskPix^)^, BitMapPtr(dupPix^)^, srcRect, r, srcBic, Nil)
		ELSE
			CopyBits(selection.maskBits, dupBits, srcRect, r, srcBic, Nil);
END;


PROCEDURE DupToPort(hbe: BEHandle);

BEGIN
	WITH hbe^^ DO
		IF isColor THEN
			{ cc }
			CopyPixels (dupPix, CGrafPtr(beGrafPtr)^.portPixMap)
			{ CopyBits(BitMapPtr(dupPix^)^, beGrafPtr^.portBits, dupPix^^.bounds,
						cGrafPtr(beGrafPtr)^.portPixMap^^.bounds, srcCopy, Nil) }
		ELSE
			CopyBits(dupBits, beGrafPtr^.portBits, dupBits.bounds,
						beGrafPtr^.portBits.bounds, srcCopy, Nil);
END;


PROCEDURE DupToSel(hbe: BEHandle);

VAR
	r, srcRect: Rect;

BEGIN
	SetupRects(r, srcRect, hbe);
	WITH hbe^^ DO
		IF isColor THEN
			CopyBits(BitMapPtr(dupPix^)^, BitMapPtr(selection.selPix^)^, r, selection.selPix^^.bounds, srcCopy, Nil)
		ELSE
			CopyBits(dupBits, selection.selBits, r, selection.selBits.bounds, srcCopy, Nil);
END;

{ As modified by cc }
PROCEDURE OldPortToDup(hbe: BEHandle);

VAR
	{ cc } rgbFore, rgbBack, rgbZeros, rgbOnes: RGBColor;
	{ cc } thePort: GrafPtr;
	{ cc } theDevice: GDHandle;
	{ cc } pixelOnes, pixelZeros: LONGINT;
BEGIN
	WITH hbe^^ DO
		IF isColor THEN
			BEGIN
			{ Set foreground color to black to prevent unpredictable colorizing during
			  copy bits }
			GetForeColor (rgbFore);
			GetBackColor (rgbBack);
			Index2Color (0, rgbZeros);
			Index2Color (BSL(1,bePixelSize)-1, rgbOnes);
			RGBForeColor (rgbOnes);
			RGBBackColor (rgbZeros);
			pixelOnes := Color2Index (rgbOnes);
			pixelZeros := Color2Index (rgbZeros);
			theDevice := GetGDevice;
			GetPort (thePort);
			CopyBits(beGrafPtr^.portBits, BitMapPtr(dupPix^)^, cGrafPtr(beGrafPtr)^.portPixMap^^.bounds,
						dupPix^^.bounds, srcCopy, Nil);
			RGBForeColor (rgbFore);
			RGBBackColor (rgbBack);
			END
		ELSE
			CopyBits(beGrafPtr^.portBits, dupBits, beGrafPtr^.portBits.bounds, dupBits.bounds,
						srcCopy, Nil);
END;

PROCEDURE PortToDup(hbe: BEHandle);

BEGIN
	WITH hbe^^ DO
		IF isColor THEN
			{ cc }
			BEGIN
				{
				CopyPixels (CGrafPtr(beGrafPtr)^.portPixMap, dupPix)
				}
				OldPortToDup (hbe)
				{
				CopyBits(beGrafPtr^.portBits, BitMapPtr(dupPix^)^, cGrafPtr(beGrafPtr)^.portPixMap^^.bounds,
						dupPix^^.bounds, srcCopy, Nil)
				}
			END
		ELSE
			CopyBits(beGrafPtr^.portBits, dupBits, beGrafPtr^.portBits.bounds, dupBits.bounds,
						srcCopy, Nil);
END;


PROCEDURE EdgeToPort(hbe: BEHandle; mode: INTEGER);

VAR
	r, srcRect: Rect;

BEGIN
	SetupRects(r, srcRect, hbe);
	WITH hbe^^ DO
		IF isColor THEN
			CopyBits(BitMapPtr(selection.edgePix^)^, beGrafPtr^.portBits, srcRect, r, mode, Nil)
		ELSE
			CopyBits(selection.edgeBits, beGrafPtr^.portBits, srcRect, r, mode, Nil);
END;


PROCEDURE CopySelToPort(hbe: BEHandle);

VAR
	r: Rect;

BEGIN
	IF hbe^^.marquee THEN
		CSelToPort(hbe, r, False)
	ELSE
		BEGIN
		MaskToPort(hbe, r, False);
		OSelToPort(hbe, r, False);
		END;
END;


PROCEDURE DoQuickDupToPort(hbe: BEHandle);

BEGIN
	BEDrawBegin(hbe);
	LockHands(hbe);
	DupToPort(hbe);
	UnlockHands(hbe);
	BEDrawEnd(hbe);
END;


PROCEDURE DoQuickPortToDup(hbe: BEHandle);

BEGIN
	BEDrawBegin(hbe);
	PortToDup(hbe);
	BEDrawEnd(hbe);
END;


PROCEDURE MyDisposPixMap(p: PixMapHandle);

VAR
	n: Handle;

BEGIN
	IF p <> Nil THEN
		BEGIN
		n := NewHandle(0);
		p^^.pmTable := CTabHandle(n);
		DisposPixMap(p);
		END;
END;


PROCEDURE BEActivate(activate: BOOLEAN; hbe: BEHandle);
{ handle activate/deactivate events }

VAR
   hUse: BEHandle;

BEGIN
	IF NOT EmptyRect(hbe^^.selection.selRect) AND NOT activate THEN
		BEGIN
		LockHands(hbe);
		BEDrawBegin(hbe);
		CopySelToPort(hbe);
		BEDrawEnd(hbe);
		UnlockHands(hbe);

		BEInvalRect(hbe^^.beEditRect, hbe);
		END;

	hUse:= hbe;
	REPEAT
	IF activate THEN
		hUse^^.active := TRUE
	ELSE
		BEGIN
		hUse^^.active := FALSE;
		IF hbe^^.offHandle <> Nil THEN
			BEGIN
			DisposHandle(hbe^^.offHandle);
			hbe^^.offHandle := Nil;
			IF hbe^^.isColor THEN
				BEGIN
				DisposPixMap(hbe^^.offPix);
				hbe^^.offPix := Nil
				END;
			END;
		END;
	hUse := hUse^^.sibling;
	UNTIL hUse = hbe;
END; { BEActivate }


PROCEDURE BEDoDraw(draw: BOOLEAN; hbe: BEHandle);
{ turn drawing on and off }

VAR
   hUse: BEHandle;

BEGIN
	hUse := hbe;
	REPEAT
	IF draw THEN
		hUse^^.drawEnabled := True
	ELSE
		hUse^^.drawEnabled := False;
	hUse := hUse^^.sibling;
	UNTIL hUse = hbe;
END; { BEDoDraw }


PROCEDURE AlignBits(hbe: BEHandle);
{ aligns the big offscreen bitmap with the screen for maximum copybits speed }

VAR
	bitNum, width: INTEGER;

BEGIN
	WITH hbe^^ DO
	BEGIN
		bitNum := (beViewRect.left - beGrafPtr^.portBits.bounds.left) MOD 32;
		IF bitNum < 0 THEN bitNum := bitNum + 32;
		WITH offBits.bounds DO
		BEGIN
			width := right - left;
			left := beViewRect.left - bitNum;
			right := left + width;
		END;
	END;
END; { AlignBits }


PROCEDURE BEBitmap(VAR bm: Bitmap; hbe: BEHandle);
{ set up a bitmap according to a BitEdit record }

BEGIN
	WITH hbe^^, bm DO
		BEGIN
		baseAddr := POINTER(ORD(editHandle^) + editOffset);
		rowBytes := beRowBytes;
		bounds := beBounds;
		END;
END; { BEBitmap }


PROCEDURE BEPixmap(VAR bm: PixMapHandle; hbe: BEHandle);
{ set up a pixmap according to a BitEdit record }

BEGIN
	WITH hbe^^ DO
		BEGIN
		editPix^^.baseAddr := POINTER(ORD(editHandle^) + editOffset);
		bm := editPix;
		END;
END; { BEBitmap }


FUNCTION BEBit(pt: Point; hbe: BEHandle): BOOLEAN;
{ Get the value of the pixel at pt }

VAR
	myRGB: RGBColor;

BEGIN
	BEBit := FALSE;
	BEPtXForm(pt, hbe);
	WITH hbe^^ DO
		BEGIN
		IF PtInRect(pt, beEditRect) THEN
			BEGIN
			BEDrawBegin(hbe);
			IF isColor THEN
				BEGIN
				GetCPixel(pt.h, pt.v, myRGB);
				IF (myRGB.red = bePixColor.red) &
					(myRGB.green = bePixColor.green) &
					(myRGB.blue = bePixColor.blue) THEN
					BEBit := True;
				END
			ELSE
				BEBit := GetPixel(pt.h, pt.v);
			BEDrawEnd(hbe);
			END;
		END;
END; { BEBit }


PROCEDURE BEColor (pt: Point; VAR rgb: RGBColor; hbe: BEHandle);
{ Returns in rgb the color of the pixel at pt; returns white if pt is outside beEditRect or
  if the BitEdit object is not in color }

BEGIN
	WITH rgb DO
		BEGIN red := 0; green := 0; blue := 0; END;
	BEPtXForm (pt, hbe);
	IF PtInRect(pt,hbe^^.beEditRect) AND hbe^^.isColor THEN
		BEGIN
		BEDrawBegin (hbe);
		GetCPixel (pt.h, pt.v, rgb);
		BEDrawEnd (hbe);
		END;
END; { BEColor }


PROCEDURE BEPaintBucket (startPt: Point; hbe: BEHandle);
{ Paint the bit/pixel at point startPt (in window coordinates) and all adjacent bits/pixels of the same value
  with 1/the current color }

VAR
	bmMask:	BitMap;
	r:			Rect;
	port:		GrafPtr;
	rgbBlack:RGBColor;
	hpmPat:	PixMapHandle;
	bmPat:	BitMap;
	p:			Ptr;

BEGIN
	IF BESelection(hbe) THEN
		BEPutSelection (hbe)
	ELSE
		SetupUndo (hbe);

	{ Get the rectangle enclosing the image }
	r := hbe^^.beBounds;

	{ Setup the mask }
	bmMask.rowBytes := ((r.right - r.left) + 7) DIV 8;		{ minimum number of bytes }
	bmMask.rowBytes := BAND (bmMask.rowBytes+1, $FFFE);	{ make it an even number of bytes for SeedCFill }
	bmMask.bounds := r;
	bmMask.baseAddr := NewPtr (UIntMul(bmMask.rowBytes, (r.bottom - r.top)));
	hbe^^.error := MemError;
	If hbe^^.error <> NoErr THEN
		Exit(BEPaintBucket);

	{ Expand the pattern }
	IF hbe^^.isColor THEN
		BEGIN
		hpmPat := hbe^^.editPix;
		hbe^^.error := HandToHand (Handle (hpmPat));
		IF hbe^^.error <> NoErr THEN
			BEGIN
			DisposPtr (bmMask.baseAddr);
			Exit(BEPaintBucket);
			END;
		p := NewPtr (UIntMul(BAND (hpmPat^^.rowBytes, $7FFF), (r.bottom - r.top)));
		hpmPat^^.baseAddr := p;
		hbe^^.error := MemError;
		IF hbe^^.error <> NoErr THEN
			BEGIN
			DisposPtr (bmMask.baseAddr);
			HUnlock (Handle (hpmPat));
			DisposHandle (Handle (hpmPat));
			Exit(BEPaintBucket);
			END;
		END
	ELSE
		BEGIN
		bmPat.bounds := r;
		bmPat.rowBytes := hbe^^.beRowBytes;
		bmPat.baseAddr := NewPtr (UIntMul(bmPat.rowBytes, (r.bottom - r.top)));
		hbe^^.error := MemError;
		IF hbe^^.error <> NoErr THEN
			BEGIN
			DisposPtr (bmMask.baseAddr);
			Exit(BEPaintBucket);
			END;
		END;

	{ Create the pattern }
	LockHands(hbe);
	BEDrawBegin (hbe);
	IF hbe^^.isColor THEN
		BEGIN
		WITH rgbBlack DO
			BEGIN red := 0; green := 0; blue := 0; END;
		HLock (Handle (hpmPat));
		SetPortPix (hpmPat);
		RGBForeColor(hbe^^.bePixColor);
		hbe^^.bePixPat^^.patXValid := -1;
		FillCRect(r, hbe^^.bePixPat);
		RGBForeColor (rgbBlack);
		HUnlock (Handle (hpmPat));
		END
	ELSE
		BEGIN
		SetPortBits (bmPat);
		FillRect (r, hbe^^.beFillPat);
		END;
	BEDrawEnd (hbe);
	UnlockHands(hbe);

	{ Translate window point into bitmap local coordinates }
	BEPtXForm (startPt, hbe);

	LockHands(hbe);
	BEDrawBegin (hbe);
	GetPort (port);

	IF hbe^^.isColor THEN
		BEGIN
		{ Calculate the mask }
		SeedCFill (port^.portBits, bmMask, r, r, startPt.h, startPt.v, nil, 0);

		{ Spread the pattern }
		HLock (Handle (hpmPat));
		CopyMask (BitMapPtr(hpmPat^)^, bmMask, port^.portBits, r, r, r);
		HUnlock (Handle (hpmPat));

		{ Discard expanded pattern }
		DisposPtr (hpmPat^^.baseAddr);
		DisposHandle (Handle (hpmPat));
		END
	ELSE
		BEGIN
		SeedFill (port^.portBits.baseAddr, bmMask.baseAddr, hbe^^.beRowBytes, bmMask.rowBytes, (r.bottom-r.top),
						bmMask.rowBytes DIV 2, startPt.h, startPt.v);
		CopyMask (bmPat, bmMask, port^.portBits, r, r, r);

		{ Discard expanded pattern data }
		DisposPtr (bmPat.baseAddr);
		END;

	BEDrawEnd (hbe);
	UnlockHands(hbe);

	{ Force redraw }
	BEInvalRect (r, hbe);

	{ Discard mask }
	DisposPtr (bmMask.baseAddr);
END; { BEPaintBucket }


PROCEDURE DoSaveBits(hbe: BEHandle);

BEGIN
	WITH hbe^^ DO
		BEGIN
		IF isColor THEN
			savePixMap := cGrafPtr(beGrafPtr)^.portPixMap
		ELSE
			saveBitMap := beGrafPtr^.portBits;
		END;
END;


PROCEDURE DoRestoreBits(hbe: BEHandle);

BEGIN
	WITH hbe^^ DO
		BEGIN
		IF isColor THEN
			SetPortPix(savePixMap)
		ELSE
			SetPortBits(saveBitMap);
		END;
END;


FUNCTION  BEInSelection(pt: Point; hbe: BEHandle): BOOLEAN;
{ tell whether or not the given point is over a selection. }
{ returns False if there is no selection. }

VAR
	r: Rect;
	saveBits: PixMapHandle;
	tPt: Point;
	myC: RGBColor;

BEGIN
	WITH hbe^^, hbe^^.selection DO
		BEGIN
		BEInSelection := False;
		IF NOT EmptyRect(selRect) OR (maskHandle <> Nil) THEN
			BEGIN
			r := selRect;
			StretchRect(r);
			BEPtXForm(pt, hbe);
			IF marquee THEN
				BEGIN
				IF PtInRect(pt, r) THEN
					BEInSelection := True;
				END
			ELSE
				IF PtInRect(pt, r) THEN
					BEGIN
					DoSaveBits(hbe);
					tPt := r.topLeft;

					IF isColor THEN
						BEGIN
						maskPix^^.baseAddr := maskHandle^;
						SetPortPix(maskPix);
						SubPt(maskPix^^.bounds.topLeft, tPt);
						END
					ELSE
						BEGIN
						maskBits.baseAddr := maskHandle^;
						SetPortBits(maskBits);
						SubPt(maskBits.bounds.topLeft, tPt);
						END;

					SubPt(tPt, pt);
					pt.h := pt.h + 1;
					pt.v := pt.v + 1;

					IF isColor THEN
						BEGIN
						GetCPixel(pt.h, pt.v, myC);
						WITH myC DO
							IF (red <> $FFFF) | (blue <> $FFFF) | (green <> $FFFF) THEN
								BEInSelection := True;
						END
					ELSE
						IF GetPixel(pt.h, pt.v) THEN
							BEInSelection := True;
					DoRestoreBits(hbe);
					END;
			END;
		END;
END; { BEInSelection }


FUNCTION  BESelection(hbe: BEHandle): BOOLEAN;
{ tell whether or not there is a selection. }
{ returns False if there is no selection. }

BEGIN
	WITH hbe^^.selection DO
		BEGIN
		IF EmptyRect(selRect) AND (maskHandle = Nil) THEN
			BESelection := FALSE
		ELSE
			BESelection := TRUE;
		END;
END;	{ BESelection }


PROCEDURE GetBWTable(tempTable: CTabHandle);

VAR
	i: INTEGER;

BEGIN
	WITH tempTable^^ DO
		FOR i := 0 to ctSize DO
			BEGIN
			IF (ctTable[i].rgb.red <> $FFFF) OR
				(ctTable[i].rgb.green <> $FFFF) OR
				(ctTable[i].rgb.blue <> $FFFF) THEN
					BEGIN
					ctTable[i].rgb.red := 0;
					ctTable[i].rgb.green := 0;
					ctTable[i].rgb.blue := 0;
					END;
			END;
END;


FUNCTION myGetPixel(x, y: INTEGER; hbe: BEHandle): BOOLEAN;

VAR
	 myC: RGBColor;

BEGIN
	IF hbe^^.isColor THEN
		BEGIN
		myGetPixel := False;
		GetCPixel(x, y, myC);
		WITH myC DO
			IF (red <> $FFFF) OR (blue <> $FFFF) OR (green <> $FFFF) THEN
				myGetPixel := True;
		END
	ELSE
		myGetPixel := GetPixel(x,y);
END;


PROCEDURE BEBounds(VAR minimalRect: Rect; hbe: BEHandle);
{ returns the minimal bounds rect of the bitmap }

VAR
    x, y: INTEGER;
	 tempRect: Rect;

BEGIN
	LockHands(hbe);
	WITH hbe^^.beEditRect DO
		SetRect(minimalRect, right, bottom, left, top);
	tempRect := minimalRect;
	WITH hbe^^, beEditRect DO
		BEGIN
		BEDrawBegin(hbe);
		HideCursor;

	{ check for left boundary }
		y := top;
		x := left;
		WHILE y < bottom DO
			BEGIN
			WHILE x < right DO
				BEGIN
				IF myGetPixel(x, y, hbe) THEN
					IF minimalRect.left > x THEN minimalRect.left := x;
				x := x + 1;
				END;
			y := y + 1;
			x := left;
			END;

	{ check for right boundary }
		y := top;
		x := right - 1;
		WHILE y < bottom DO
			BEGIN
			WHILE x >= left DO
				BEGIN
				IF myGetPixel(x, y, hbe) THEN
					IF minimalRect.right < x THEN minimalRect.right := x;
				x := x - 1;
				END;
			y := y + 1;
			x := right - 1;
			END;

	{ check for top boundary }
		y := top;
		x := left;
		WHILE x < right DO
			BEGIN
			WHILE y < bottom DO
				BEGIN
				IF myGetPixel(x, y, hbe) THEN
					IF minimalRect.top > y THEN minimalRect.top := y;
				y := y + 1;
				END;
			x := x + 1;
			y := top;
			END;

	{ check for bottom boundary }
		y := bottom - 1;
		x := left;
		WHILE x < right DO
			BEGIN
			WHILE y >= top DO
				BEGIN
				IF myGetPixel(x, y, hbe) THEN
					IF minimalRect.bottom < y THEN minimalRect.bottom := y;
				y := y - 1;
				END;
			x := x + 1;
			y := bottom - 1;
			END;

		IF EqualRect(tempRect, minimalRect) THEN
			SetRect(minimalRect, 0, 0, 0, 0)
		ELSE
			BEGIN
			StretchRect(minimalRect);
			END;

		BEDrawEnd(hbe);
		ShowCursor;
		END;

UnlockHands(hbe);
END; { BEBounds }


PROCEDURE MySetDupBits(hbe: BEHandle);

BEGIN
	WITH hbe^^ DO
		BEGIN
		IF isColor THEN
			SetPortPix(dupPix)
		ELSE
			SetPortBits(dupBits);
		END;
END;


PROCEDURE BEClear(hbe: BEHandle);
{ Clears the selection }

VAR
	r, srcRect: Rect;

BEGIN
	IF NOT EmptyRect(hbe^^.selection.selRect) THEN
		BEGIN
		SetupRects(r, srcRect, hbe);
		LockHands(hbe);

		IF NOT hbe^^.selection.wasErased THEN
			BEGIN
			BEDrawBegin(hbe);
			CopySelToPort(hbe);
			IF hbe^^.marquee THEN
				BEGIN
				MySetDupBits(hbe);
				EraseRect(r);
				END
			ELSE
				MaskToDup(hbe);
			BEDrawEnd(hbe);
			END;

		UnlockHands(hbe);
		BEKillSelection(hbe);

		END;

END; { BEClear }


FUNCTION BEGetView(pt: Point; VAR hbe: BEHandle): BOOLEAN;

VAR
   found: Boolean;
	hUse: BEHandle;

BEGIN
	 hUse := hbe;
	 REPEAT
	    found := PtInRect(pt, hUse^^.beViewRect);
		 IF found THEN
		    hbe := hUse
		 ELSE
		    hUse:= hUse^^.sibling;
	 UNTIL hUse = hbe;
	 BEGetView := found;
END; { BEGetView }


PROCEDURE BEUpdateRect(r: Rect; hbe: BEHandle);
{ Redraws the given rectangle (in screen space) }

VAR
    theRect: rect;
	 hUse: BEHandle;
	 t: Boolean;

BEGIN
	hUse:= hbe;
	REPEAT
		IF SectRect(r, hUse^^.beViewRect, theRect) THEN
			BEGIN
				BEPtXForm(theRect.topLeft, hUse);
				BEPtXForm(theRect.botRight, hUse);
				InsetRect(theRect, -1, -1);
				t := SectRect(theRect, hUse^^.beEditRect, theRect);
				BEDraw(theRect, hUse);
			END;
		hUse:= hUse^^.sibling;
	UNTIL hUse = hbe;
END; { BEUpdateRect }


PROCEDURE BEInvalRect(r: Rect; hbe: BEHandle);

VAR
	hFirst: BEHandle;
	b: BOOLEAN;

BEGIN
	b := SectRect (r, hbe^^.beEditRect, r);
	hFirst := hbe;
	REPEAT
		BEDraw(r, hbe);
		hbe := hbe^^.sibling;
	UNTIL hFirst = hbe;
END; { BEInvalRect }


PROCEDURE PaintBits(blacken, erase: BOOLEAN; stPt, endPt: Point; hbe: BEHandle);

VAR
	testPt: Point;
	t: BOOLEAN;
	r: Rect;
	saveRgn: Handle;
	blackRGB: RGBColor;

BEGIN
	testPt := stPt;
	BEPtXForm(stPt, hbe);
	BEPtXForm(endPt, hbe);
	GetPenState(hbe^^.savePen);

	WITH blackRGB DO
		BEGIN
		red := 0; blue := 0; green := 0;
		END;

	IF (LongInt(stPt) <> LongInt(endPt)) |
	   (blacken <> BEBit(testPt, hbe)) | erase THEN
	  BEGIN
			BEDrawBegin(hbe);
			PenNormal;

			IF NOT blacken THEN PenMode(patBic);
			IF erase THEN penSize(hbe^^.beEraserSize.h, hbe^^.beEraserSize.v);

			IF hbe^^.isColor THEN
				RGBForeColor(hbe^^.bePixColor);

		{ draw the line in small bitmap }
			MoveTo(stPt.h, stPt.v);
			LineTo(endPt.h, endPt.v);

			IF hbe^^.isColor THEN
				RGBForeColor(blackRGB);

			BEDrawEnd(hbe);

		{ figure out what the smallest rectangle to copy is }
		   Pt2Rect(stPt, endPt, r);

		{ expand rect to enclose all pixels }
			StretchRect(r);

			IF erase THEN
				BEGIN
				r.bottom := r.bottom + (hbe^^.beEraserSize.v - 1);
				r.right := r.right + (hbe^^.beEraserSize.h - 1);
				END;

		{ do our own clipping }
			t := SectRect(hbe^^.beEditRect, r, r);

		{ Draw all the views - turn off region collection }
			saveRgn := hbe^^.beGrafPtr^.rgnSave;
			hbe^^.beGrafPtr^.rgnSave := Nil;

			BEInvalRect(r, hbe);

		{ turn region collection back on }
			hbe^^.beGrafPtr^.rgnSave := saveRgn;

	END;
	SetPenState(hbe^^.savePen);
END; { PaintBits }


PROCEDURE BWOffScreen (hbe: BEHandle);

VAR
	n: Handle;
	bitNum: INTEGER;

BEGIN
	WITH hbe^^,offBits DO
	BEGIN
	{ figure out rowbytes from rectangle }
		rowBytes := ((beViewRect.right - beViewRect.left) + 7) DIV 8;

	{ allocate out to long word boundary + 1 long word }
		rowBytes := rowBytes + rowBytes MOD 4 + 4;

	{ figure out which bit in a long word the left edge occupies }
		bitNum := (beViewRect.left - beGrafPtr^.portBits.bounds.left) MOD 32;

	{ make sure answer is positive }
		IF bitNum < 0 THEN bitNum := bitNum + 32;

		with bounds do
		 begin
			top := beViewRect.top;
			left := beViewRect.left - bitNum;
			bottom := beViewRect.bottom;
			right := left + 8 * rowBytes;
		 end;
	END;

	n := NewHandle(hbe^^.offBits.rowBytes * (hbe^^.beViewRect.bottom - hbe^^.beViewRect.top));
	hbe^^.error := MemError;
	If hbe^^.error <> NoErr THEN		{ if not enough memory set error field true and leave }
		Exit(BWOffScreen);

	hbe^^.offHandle := n;
END;


PROCEDURE ResetOffScreen(hgd: GDHandle; hbe: BEHandle);
{ routine to reset the pixmap to the new pixel depth of the screen }

VAR
	viewCTHndl, theHndl: Handle;
	mySize: LongInt;
	myErr: OSErr;
	np: PixMapHandle;

BEGIN
	{ if we already have an offBits, trash it }
	IF hbe^^.offHandle <> Nil THEN
		BEGIN
		DisposHandle(hbe^^.offHandle);
		IF hbe^^.isColor THEN
			DisposPixMap(hbe^^.offPix);
		END;

	IF NOT hbe^^.isColor THEN
		BEGIN
		BWOffScreen (hbe);
		Exit(ResetOffScreen);
		END;

	np := NewPixMap;
	hbe^^.error := MemError;
	IF hbe^^.error <> NoErr THEN
		Exit(ResetOffScreen);
	hbe^^.offPix := np;
	DisposHandle (Handle(hbe^^.offPix^^.pmTable));

	viewCTHndl := Handle (hgd^^.gdPMap^^.pmTable);
	myErr := HandToHand (viewCTHndl);

	WITH hbe^^.offPix^^ DO
	BEGIN
	{ figure out rowbytes from rectangle }
		rowBytes := ((((hgd^^.gdPMap^^.pixelSize * (hbe^^.beViewRect.right - hbe^^.beViewRect.left)) + 15))
							DIV 16) * 2;
		pixelSize := hgd^^.gdPMap^^.pixelSize;
		cmpSize := hgd^^.gdPMap^^.pixelSize;
		pmTable := CTabHandle(viewCTHndl);
		bounds := hbe^^.beViewRect;
	END;

	WITH hbe^^ DO
		mySize := UIntMul((beViewRect.bottom - beViewRect.top), offPix^^.rowBytes);

	theHndl := NewHandle(mySize);
	hbe^^.error := MemError;

	hbe^^.offHandle := theHndl;
	hbe^^.offPix^^.rowBytes := hbe^^.offPix^^.rowBytes + $8000;
END;


PROCEDURE DoMyMapRect(hbe: BEHandle; VAR r: Rect; srcRect: Rect);

VAR
  rvr: Rect;

BEGIN
	WITH srcRect DO
		BEGIN
		bottom := top + 1;
		right := left + 1;
		END;

	rvr.topLeft := hbe^^.beViewRect.topLeft;
	WITH rvr, hbe^^ DO
		BEGIN
		bottom := top + fatbitsSize.v + fatbitsDelta.v;
		right := left + fatbitsSize.h + fatbitsDelta.h;
		END;

	MapRect(r, srcRect, rvr);
END;


PROCEDURE BEDraw(r: Rect; hbe: BEHandle);

TYPE
	TAPat = RECORD
		CASE INTEGER OF
			0: (pat: Pattern);
			1: (lPat0, lPat1: LONGINT);
		END;

VAR
	myWhite: TAPat;
	bigR, bmr, rvr: Rect;
	bm: BitMap;
	pm: PixMapHandle;
	i: INTEGER;
	t: Boolean;
	saveDevice, maxDevice:	GDHandle;
	globalR:	Rect;

BEGIN
	IF hbe^^.drawEnabled THEN
		BEGIN
		GetPenState (hbe^^.savePen);
		GetClip (hbe^^.saveClip);

		HLock (Handle(hbe));
		HLock (hbe^^.editHandle);

		bigR := r;
		bmr.topLeft := hbe^^.beBounds.topLeft;

		DoMyMapRect(hbe, bigR, bmr);

		t := SectRect(hbe^^.beViewRect, hbe^^.realRect, bmr);
		ClipRect(bmr);

		myWhite.lPat0 := 0;
		myWhite.lPat1 := 0;

		IF LongInt(hbe^^.fatbitsDelta) <> 0 THEN
			{ Image to offscreen pixMap to avoid flicker }
			BEGIN

			{ Find deepest screen intersecting fatbits rectangle }
			IF hbe^^.isColor THEN
				BEGIN
				saveDevice := GetGDevice;
				globalR := hbe^^.beViewRect;
				LocalToGlobal (globalR.topLeft);
				LocalToGlobal (globalR.botRight);
				maxDevice := GetMaxDevice (globalR);
				SetGDevice (maxDevice);
				END;

			{ check to see if pixel depth of screen has changed }
			{ or if the port's color table has changed -- 5/24/89 crc }
			IF hbe^^.isColor THEN
				BEGIN
				IF (hbe^^.offPix = Nil)
					|
					(maxDevice^^.gdPMap^^.pixelSize <> hbe^^.offPix^^.pixelSize)
				   |
				   (maxDevice^^.gdPMap^^.pmTable^^.ctSeed <> hbe^^.offPix^^.pmTable^^.ctSeed)
				THEN
					ResetOffscreen (maxDevice, hbe);
				END
			ELSE
				IF (hbe^^.offHandle = Nil) THEN
					BWOffScreen (hbe);

			IF hbe^^.error <> NoErr THEN
				BEGIN
				HUnlock (Handle(hbe));
				HUnlock (hbe^^.editHandle);
				SetPenState(hbe^^.savePen);
				SetClip(hbe^^.saveClip);
				Exit(BEDraw);
				END;

			WITH hbe^^ DO
				BEGIN
				HLock (offHandle);
				IF isColor THEN
					HLock(Handle(offPix));

				IF isColor THEN
					offPix^^.baseAddr := offHandle^
				ELSE
					offBits.baseAddr := offHandle^;

				{ make a pixmap out of bitEdit record }
				IF isColor THEN
					BEGIN
					BEPixMap(pm, hbe);
					HLock(Handle(cGrafPtr(beGrafPtr)^.portPixMap))		{ crc: Why this? }
					END
				ELSE
					BEBitmap(bm, hbe);

				{ align the offscreen bitmaps }
				IF NOT isColor THEN
					AlignBits(hbe);
				END;

			DoSaveBits(hbe);

			{ enlarge bits into offscreen }
			IF hbe^^.isColor THEN
				CopyBits (BitMapPtr(pm^)^, BitMapPtr(hbe^^.offPix^)^, r, bigR, srcCopy, Nil)
			ELSE
				CopyBits (bm, hbe^^.offBits, r, bigR, srcCopy, Nil);

			{ draw the grid lines }
				PenNormal;
				PenMode(patBic);

			IF hbe^^.isColor THEN
				SetPortPix(hbe^^.offPix)
			ELSE
				SetPortBits(hbe^^.offBits);

			{ draw horizontals }
			i:= bigR.top;
			WHILE i <= bigR.bottom DO
				BEGIN
				MoveTo(bigR.left, i);
				LineTo(bigR.right, i);
				i := i + hbe^^.fatbitsSize.v + hbe^^.fatbitsDelta.v;
				END;

			{ draw verticals }
			i:= bigR.left;
			WHILE i <= bigR.right DO
				BEGIN
				MoveTo(i, bigR.top);
				LineTo(i, bigR.bottom);
				i := i + hbe^^.fatbitsSize.h + hbe^^.fatbitsDelta.h;
				END;

			{ Dim out the pixels }
			IF ((TAPat(hbe^^.fatbitsPat).lPat0 <> myWhite.lPat0) |
				(TAPat(hbe^^.fatbitsPat).lPat1 <> myWhite.lPat1)) THEN
				{ if pattern is white then don't paintrect }
					BEGIN
					PenPat(hbe^^.fatbitsPat);
					PenMode(patBic);
					PaintRect(bigR);
					END;

			DoRestoreBits(hbe);

			{ copy FatBits to screen }
			IF hbe^^.isColor THEN
				BEGIN
				SetGDevice (saveDevice);
				CopyBits(BitMapPtr(hbe^^.offPix^)^, hbe^^.beGrafPtr^.portBits,
							bigR, bigR, srcCopy, Nil);
				END
			ELSE
				CopyBits(hbe^^.offBits, hbe^^.beGrafPtr^.portBits, bigR, bigR, srcCopy, Nil);

			IF hbe^^.isColor THEN
				BEGIN
				HUnlock(Handle(hbe^^.offPix));
				HUnlock(Handle(cGrafPtr(hbe^^.beGrafPtr)^.portPixMap))
				END;
			HUnlock(hbe^^.offHandle);
			END
		ELSE
			{ no grid lines, just blit to screen }
			BEGIN
			IF hbe^^.isColor THEN
				BEGIN
				BEPixMap(pm, hbe);
				CopyBits(BitMapPtr(pm^)^, hbe^^.beGrafPtr^.portBits, r, bigR, srcCopy, Nil);
				END
			ELSE
				BEGIN
				BEBitmap(bm, hbe);
				CopyBits(bm, hbe^^.beGrafPtr^.portBits, r, bigR, srcCopy, Nil);
				END;

			{ Dim out the pixels }
			IF ((TAPat(hbe^^.fatbitsPat).lPat0 <> myWhite.lPat0) |
				(TAPat(hbe^^.fatbitsPat).lPat1 <> myWhite.lPat1)) THEN
				{ if pattern is white then don't paintrect }
					BEGIN
					PenPat(hbe^^.fatbitsPat);
					PenMode(patBic);
					PaintRect(bigR);
					END;
			END;

		SetPenState(hbe^^.savePen);
		SetClip(hbe^^.saveClip);
		HUnlock(hbe^^.editHandle);
		HUnlock(Handle(hbe));
		END;
END; { BEDraw }

FUNCTION AllocatePixMap (pmTable: CTabHandle; hbe: BEHandle): PixMapHandle;
{ Returns Nil if a PixMap cannot be allocated.  If pmTable is not Nil,
  it is used as the color table. }

VAR
	hpm: PixMapHandle;

BEGIN
	AllocatePixMap := Nil;

	hpm := NewPixMap;
	IF MemError = NoErr THEN
		BEGIN
		IF pmTable <> Nil THEN
			BEGIN
			DisposHandle (Handle(hpm^^.pmTable));
			hpm^^.pmTable := pmTable
			END;
		AllocatePixMap := hpm;
		END;
END; { AllocatePixMap }


PROCEDURE NewSelPixMaps (hbe: BEHandle);
{ This routine should be called once to allocate and partially initialize PixMaps
  for repeated use in selections.  This routine requires its argument handle to
  be locked! }

VAR
	hpm: PixMapHandle;
	myWhite, myBlack: RGBColor;
	pmTable: CTabHandle;

BEGIN
	WITH hbe^^.selection DO
		BEGIN
		selPix := AllocatePixMap(hbe^^.beColorTable, hbe);
		IF selPix = Nil THEN
			EXIT (NewSelPixMaps);

		selPix^^.pixelSize := hbe^^.bePixelSize;
		selPix^^.cmpSize := hbe^^.bePixelSize;

		maskPix := AllocatePixMap (Nil, hbe);
		IF maskPix = Nil THEN
			EXIT (NewSelPixMaps);

		maskPix^^.pixelSize := 1;
		maskPix^^.cmpSize := 1;

		WITH myBlack DO
			BEGIN
			red := 0; blue := 0; green := 0;
			END;

		WITH myWhite DO
			BEGIN
			red := $FFFF; blue := $FFFF; green := $FFFF;
			END;

		{ Is this handle zero length? }
		WITH maskPix^^.pmTable^^ DO
			BEGIN
			ctSeed := 1;
			ctFlags := $8000;
			ctSize := 1;
			ctTable[0].value := 0;
			ctTable[1].value := 0;
			ctTable[0].rgb := myWhite;
			ctTable[1].rgb := myBlack;
			END;

		pmTable := maskPix^^.pmTable;

		edgePix := AllocatePixMap (pmTable, hbe);
		IF edgePix = Nil THEN
			EXIT (NewSelPixMaps);

		edgePix^^.pixelSize := 1;
		edgePix^^.cmpSize := 1;
		END;

	WITH hbe^^.undoSelection DO
		BEGIN
		selPix := AllocatePixMap(hbe^^.beColorTable, hbe);
		IF selPix = Nil THEN
			EXIT (NewSelPixMaps);

		maskPix := AllocatePixMap (pmTable, hbe);
		IF maskPix = Nil THEN
			EXIT (NewSelPixMaps);

		edgePix := AllocatePixMap (pmTable, hbe);
		IF edgePix = Nil THEN
			EXIT (NewSelPixMaps);
		END;
END; { NewSelPixMaps }


PROCEDURE DisposeSelPixMaps (hbe: BEHandle);
BEGIN
	MyDisposPixMap (hbe^^.undoSelection.edgePix);
	MyDisposPixMap (hbe^^.undoSelection.maskPix);
	MyDisposPixMap (hbe^^.undoSelection.selPix);

	MyDisposPixMap (hbe^^.selection.edgePix);
	IF  hbe^^.selection.maskPix <> Nil THEN
		DisposPixMap   (hbe^^.selection.maskPix);
	MyDisposPixMap (hbe^^.selection.selPix);
END;


PROCEDURE KillSelection (VAR selection: BESelectRec);
{ NOTE: if the argument is referenced by a BEHandle, make sure the handle is locked }

BEGIN
	WITH selection DO
		BEGIN
		IF edgeHandle <> Nil THEN
			BEGIN
			DisposHandle(edgeHandle);
			edgeHandle := Nil;
			END;

		IF maskHandle <> Nil THEN
			BEGIN
			DisposHandle(maskHandle);
			maskHandle := Nil;
			END;

		IF selHandle <> Nil THEN
			BEGIN
			DisposHandle(selHandle);
			selHandle := Nil;
			END;

		SetRect(selRect, 0, 0, 0, 0);
		END;
END; { KillSelection }


PROCEDURE KillUndoSelection (hbe: BEHandle);

BEGIN
	HLock (Handle (hbe));
	KillSelection (hbe^^.undoSelection);
	HUnlock (Handle (hbe));
END; { KillUndoSelection }


PROCEDURE SetupUndoSelection (hbe: BEHandle);
{ Save a copy of the current selection for undo.  The current selection is not effected }

LABEL
	1, 2, 3;
VAR
	err:		OSErr;
	maskHandle, edgeHandle, selHandle: Handle;

BEGIN
	IF hbe^^.undoFreeze = 0 THEN
		BEGIN
		err := noErr;

		maskHandle := hbe^^.selection.maskHandle;
		IF maskHandle <> Nil THEN
			err := HandToHand (Handle(maskHandle));
		IF err <> noErr THEN
			GOTO 1;

		edgeHandle := hbe^^.selection.edgeHandle;
		IF edgeHandle <> Nil THEN
			err := HandToHand (Handle(edgeHandle));
		IF err <> noErr THEN
			GOTO 2;

		selHandle := hbe^^.selection.selHandle;
		IF selHandle <> Nil THEN
			err := HandToHand (Handle(selHandle));
		IF err <> noErr THEN
			GOTO 3;

		KillUndoSelection (hbe);

		hbe^^.undoSelection.maskHandle := maskHandle;
		hbe^^.undoSelection.edgeHandle := edgeHandle;
		hbe^^.undoSelection.selHandle  := selHandle;

		{ Update the bitMaps or pixMaps (note that all color tables are shared) }
		IF hbe^^.isColor THEN
			BEGIN
			hbe^^.undoSelection.maskPix^^ := hbe^^.selection.maskPix^^;
			hbe^^.undoSelection.edgePix^^ := hbe^^.selection.edgePix^^;
			hbe^^.undoSelection.selPix^^  := hbe^^.selection.selPix^^;
			END
		ELSE
			BEGIN
			hbe^^.undoSelection.maskBits := hbe^^.selection.maskBits;
			hbe^^.undoSelection.edgeBits := hbe^^.selection.edgeBits;
			hbe^^.undoSelection.selBits  := hbe^^.selection.selBits;
			END;

		hbe^^.undoSelection.selRect   := hbe^^.selection.selRect;
		hbe^^.undoSelection.wasErased := hbe^^.selection.wasErased;

		{ Normal exit }
		GOTO 1;

		{ Error exits }
		3:	DisposHandle(Handle(edgeHandle));
		2:	DisposHandle(Handle(maskHandle));

		{ Normal and error exit }
		1: hbe^^.error := err;
		END;

END; { SetupUndoSelection }


PROCEDURE BEKillSelection(hbe: BEHandle);
{ kills the selection rect without putting down the bits }

VAR
	r: Rect;
	n: Handle;

BEGIN
	IF BESelection(hbe) THEN
		BEGIN
		r := hbe^^.selection.selRect;
		StretchRect(r);

	{ Copy current selection for undo, then kill it }
		SetupUndo (hbe);
		HLock(Handle(hbe));
		KillSelection(hbe^^.selection);
		HUnlock(Handle(hbe));

	{ copy from dupBits to BEBits }
		DoQuickDupToPort(hbe);

	{ hide selection in all views }
		BEInvalRect(r, hbe);
		END;

END; { BEKillSelection }


PROCEDURE BEPutSelection(hbe: BEHandle);
{ puts the marquee selection bits or lasso bits down and then kills the selection }

BEGIN
	IF BESelection(hbe) THEN
		BEGIN
		BEUndoBegin (hbe);
		LockHands(hbe);

	{ plunk down bits }
		IF hbe^^.marquee THEN
			CSelToDup(hbe)
		ELSE
			BEGIN
			MaskToDup(hbe);
			OSelToDup(hbe);
			END;

		UnlockHands(hbe);

		BEKillSelection(hbe);
		BEUndoEnd (hbe);
		END;

END; { BEPutSelection }


PROCEDURE BEClick(pt: Point; hbe: BEHandle);
{ Toggle the bit at the given screen point.  If selection up hide it. }

VAR
	blacken: BOOLEAN;
	savePt: Point;

BEGIN
{ make sure there is no selection }
	IF BESelection(hbe) THEN
		BEPutSelection(hbe)
	ELSE
		SetupUndo(hbe);

	blacken := NOT BEBit(pt, hbe);

	LONGINT(savePt) := LONGINT(pt);

	PaintBits(blacken, False, savePt, pt, hbe);

	WHILE WaitMouseUp DO
		BEGIN
		GetMouse(pt);
		IF LONGINT(savePt) <> LONGINT(pt) THEN
			PaintBits(blacken, False, savePt, pt, hbe);

		LONGINT(savePt) := LONGINT(pt);
		END;

END; { BEClick }


PROCEDURE BEErase(pt: Point; hbe: BEHandle);
{ Erase the bit at the given screen point.  If selection up hide it. }

VAR
	savePort: GrafPtr;
	savePt: Point;
	hUse: BEHandle;

BEGIN
{ make sure there is no selection }
	IF BESelection(hbe) THEN
		BEPutSelection(hbe)
	ELSE
		SetupUndo(hbe);

	LONGINT(savePt) := LONGINT(pt);

	PaintBits(False, True, savePt, pt, hbe);

	WHILE WaitMouseUp DO
		BEGIN
		GetMouse(pt);
		IF LONGINT(savePt) <> LONGINT(pt) THEN
			PaintBits(False, True, savePt, pt, hbe);

		LONGINT(savePt) := LONGINT(pt);
		END;

END; { BEErase }


PROCEDURE BECopy(hbe: BEHandle);
{ Copy selection to scrap as bitmap picture }

VAR
    pict: PicHandle;
    x: LONGINT;
	 r, srcRect: Rect;

BEGIN
	IF NOT EmptyRect(hbe^^.selection.selRect) THEN
		BEGIN
		SetupRects(r, srcRect, hbe);

		BEDrawBegin(hbe);
		LockHands(hbe);

		pict := OpenPicture(r);
		CopySelToPort(hbe);
		ClosePicture;

		UnlockHands(hbe);
		BEDrawEnd(hbe);

		x := ZeroScrap;
		HLock(Handle(pict));
		x := PutScrap(GetHandleSize(Handle(pict)), 'PICT', POINTER(pict^));
		HUnlock(Handle(pict));
		KillPicture(pict);
		END;

END; { BECopy }


PROCEDURE BECut(hbe: BEHandle);
{ Cut the selection }

BEGIN
	BECopy(hbe);
	BEClear(hbe);
END; { BECut }


PROCEDURE BEDispose(hbe: BEHandle);
{ Dispose the bit edit record and the offscreen bitmaps }

BEGIN
	BEPutSelection(hbe);
	KillUndoSelection (hbe);

	IF hbe^^.offHandle <> Nil THEN
		BEGIN
		DisposHandle(hbe^^.offHandle);
		IF hbe^^.isColor THEN
			DisposPixMap(hbe^^.offPix);
		END;

	IF hbe^^.isColor THEN
		BEGIN
		DisposGDevice(hbe^^.beGDevice);
		MyDisposPixMap(hbe^^.dupPix);
		MyDisposPixMap(hbe^^.editPix);
		MyDisposPixMap(hbe^^.undoPix);
		DisposeSelPixMaps (hbe);
		END;

	DisposHandle(hbe^^.dupHandle);
	DisposHandle(hbe^^.undoHandle);
	DisposeRgn(hbe^^.saveClip);
	DisposeRgn(hbe^^.saveVis);
	DisposHandle(Handle(hbe));
END; { BEDispose }


PROCEDURE BEDrawBegin(hbe: BEHandle);
{ Prepares the grafPort for drawing into the bits offscreen }

VAR
    savePort: GrafPtr;
	 theDevice: GDHandle;
	 myPixel:RGBColor;

BEGIN
	GetPort(savePort);

	WITH hbe^^, beGrafPtr^ DO BEGIN
		SetPort(beGrafPtr);
		DoSaveBits(hbe);

		HLock(editHandle);
		IF hbe^^.isColor THEN
			BEGIN
			BEPixMap(cGrafPtr(hbe^^.beGrafPtr)^.portPixMap, hbe);
			HLock(Handle(cGrafPtr(hbe^^.beGrafPtr)^.portPixMap));
			END
		ELSE
			BEBitMap(portBits, hbe);

		GetPenState(savePen);

		GetClip(saveClip);
		ClipRect(beEditRect);

		CopyRgn(visRgn, saveVis);
		IF hbe^^.isColor THEN
			RectRgn(visRgn, cGrafPtr(hbe^^.beGrafPtr)^.portPixMap^^.bounds)
		ELSE
			RectRgn(visRgn, hbe^^.beGrafPtr^.portBits.bounds);
	END;

	SetPort(savePort);

	IF hbe^^.isColor THEN
		BEGIN
		theDevice := GetGDevice;
		SetGDevice(hbe^^.beGDevice);
		hbe^^.beGDevice := theDevice;
		GetCPixel(0,0, myPixel);
		SetCPixel(0,0, myPixel);
		END;

END; { BEDrawBegin }


PROCEDURE BEDrawEnd(hbe: BEHandle);
{ Releases the grafport for drawing into the bits }

VAR
    savePort: GrafPtr;
	 theDevice: GDHandle;
	 myPixel: RGBColor;

BEGIN
	IF hbe^^.isColor THEN
		BEGIN
		theDevice := GetGDevice;
		SetGDevice(hbe^^.beGDevice);
		hbe^^.beGDevice := theDevice;
		END;

	WITH hbe^^, beGrafPtr^ DO
		BEGIN
		GetPort(savePort);
		SetPort(beGrafPtr);

		IF hbe^^.isColor THEN
			HUnlock(Handle(cGrafPtr(hbe^^.beGrafPtr)^.portPixMap));
		DoRestoreBits(hbe);

		HUnlock(editHandle);

		SetPenState(savePen);
		SetClip(saveClip);

		CopyRgn(saveVis, visRgn);

		SetPort(savePort);

		IF hbe^^.isColor THEN
			BEGIN
			GetCPixel(0,0, myPixel);
			SetCPixel(0,0, myPixel);
			END;
		END;

END; { BEDrawEnd }


PROCEDURE BEIdle(hbe: BEHandle);
{ Dance the selection rectangle or lasso if active and selection is there }

BEGIN
	IF hbe^^.active THEN
		WITH hbe^^.selection DO
			BEGIN
			IF (maskHandle <> Nil) THEN
				HiliteLasso(hbe)
			ELSE
				IF (NOT EmptyRect(selRect)) THEN
					BEObject(selRect.topLeft, @MarqueeDraw, BOR (fUseSelection, fIdle), hbe);
			END;
END; { BEIdle }


PROCEDURE BEObject(startPt: Point; drawProc: ProcPtr; flags: BEDrawFlags; hbe: BEHandle);

VAR
	endPt, testPt: Point;
	r, saveRect, theRect: Rect;
	t, firstFlag: Boolean;
	idle:	Boolean;
	mIndex: INTEGER;
	myBlack: RGBColor;

BEGIN
{ invalidate our pattern in case it has been expanded using another gDevice }
	IF hbe^^.isColor THEN
		hbe^^.bePixPat^^.patXValid := -1;

	idle := BAND (flags, fIdle) <> 0;

{ make sure there is no selection }
	IF NOT idle THEN
		BEGIN
		IF BESelection(hbe) THEN
			BEPutSelection(hbe)
		ELSE
			SetupUndo(hbe);
		END;

	WITH myBlack DO
		BEGIN
		red := 0; blue := 0; green := 0;
		END;

	LockHands(hbe);

	IF idle THEN
		saveRect := hbe^^.selection.selRect
	ELSE
		SetRect(saveRect,0,0,0,0);

	firstFlag := True;
	BEPtXForm(startPt, hbe);
	testPt := startPt;

{ make a copy of bits }
	IF NOT idle THEN
		DoQuickPortToDup(hbe);

	PenNormal;
	WHILE idle | WaitMouseUp DO
		BEGIN
			GetMouse(endPt);
			BEPtXForm(endPt, hbe);

			IF idle THEN
				BEGIN
				startPt := hbe^^.selection.selRect.topLeft;
				endPt := hbe^^.selection.selRect.botRight;
				END;

		GetMIndex(mIndex);

		IF (LongInt(endPt) <> LongInt(testPt)) | firstFlag | (NOT EmptyRect(hbe^^.selection.selRect)
			& (mIndex <> hbe^^.oldIndex)) THEN
		BEGIN
			firstFlag := False;
			testPt := endPt;
			r.topLeft := startPt;
			r.botRight := endPt;

		{ refresh BEBits from dupBits or selBits }

			BEDrawBegin(hbe);
			IF idle THEN
				CSelToPort(hbe, r, False)
			ELSE
				DupToPort(hbe);

		{ draw shape }
			IF hbe^^.isColor THEN
				RGBForeColor(hbe^^.bePixColor);

			PenSize(hbe^^.bePenSize.h, hbe^^.bePenSize.v);
			CallDrawProc(r, flags, hbe, drawProc);

			IF hbe^^.isColor THEN
				RGBForeColor(myBlack);

			BEDrawEnd(hbe);

		{ figure out what the smallest rectangle to copy is }
			Pt2Rect(startPt, endPt, r);

		{ expand rect to enclose all pixels }
			r.bottom := r.bottom + 1 + hbe^^.bePenSize.v;
			r.right := r.right + 1 + hbe^^.bePenSize.h;

		{ find out what the big rect is }
			UnionRect(r, saveRect, theRect);

			saveRect := r;

		{ Draw all the views }
			BEInvalRect(theRect, hbe);

		   END; { of draw }

		IF idle THEN Leave;

	END; { stilldown loop }

	UnlockHands(hbe);

{ invalidate our pattern in case client uses it on another gDevice }
	IF hbe^^.isColor THEN
		hbe^^.bePixPat^^.patXValid := -1;
END; { BEObject }


PROCEDURE BELink(hbe1, hbe2: BEHandle);
{ Link in hbe1 with hbe2 in the circular list }

VAR
    hbe: BEHandle;

BEGIN
    hbe := hbe2^^.sibling;
    hbe2^^.sibling := hbe1^^.sibling;
    hbe1^^.sibling := hbe;
END; { BELink }

{ cc }
{ ImageCt2DeviceCt creates a device color table from an image color table.
  The device color table always contains 2**pixelDepth entries, and entry i
  contains the rgb color for pixel value i.  In other words, for a device
  color table, pixel value = table index.  For image color tables, this equivalence
  is not necessarily true.
}
FUNCTION ImageCt2DeviceCt (hctImage: CTabHandle; pixelDepth: INTEGER): CTabHandle;

VAR
	hctDevice:	CTabHandle;
	i:			INTEGER;

BEGIN
	hctDevice := CTabHandle (NewHandle (8+BSL(1,pixelDepth)*8));
	HLock (Handle (hctDevice));
	WITH hctDevice^^ DO
		BEGIN
		ctSeed := 0;
		ctFlags := $8000;
		ctSize := BSL(1,pixelDepth)-1;
		FOR i := ctSize DOWNTO 0 DO
			WITH ctTable[i] DO
				BEGIN
				value := 0;
				rgb.red := 0;
				rgb.green := 0;
				rgb.blue := 0;
				END;
		FOR i := hctImage^^.ctSize DOWNTO 0 DO
			WITH ctTable[hctImage^^.ctTable[i].value].rgb DO
				BEGIN
				red := hctImage^^.ctTable[i].rgb.red;
				green := hctImage^^.ctTable[i].rgb.green;
				blue := hctImage^^.ctTable[i].rgb.blue;
				END;
		END;
	HUnlock (Handle (hctDevice));
	ImageCt2DeviceCt := hctDevice;
END; { ImageCt2DeviceCt }


FUNCTION BENew(editHandle: Handle; colorTable: CTabHandle; offset: LongInt; rowBytes: integer; pixelSize: integer; bounds: Rect): BEHandle;
{ Allocate a record and offscreen pixmaps/bitmaps }
{ If there was not enough memory to allocate pixmaps/bitmaps then return Nil in BEHandle }

VAR
	hbe: BEHandle;
	p, u : Handle;
	err: OSErr;
	theDevice: GDHandle;
	ctabTemp: CTabHandle;

BEGIN
	hbe := BEHandle(NewHandle(SIZEOF(BERec)));
	err := MemError;
	IF err <> NoErr THEN
		BEGIN
		BENew := Nil;
		Exit(BeNew);
		END;

	HLock(Handle(hbe));

{ get storage for little offscreen bitmap }
	p := NewHandle(GetHandleSize(editHandle) - offset);
	err := MemError;
	If err <> NoErr THEN		{ if not enough memory set handle to Nil and leave }
		BEGIN
		DisposHandle(Handle(hbe));
		BENew := Nil;
		Exit(BeNew);
		END;

{ get storage for undo offscreen bitmap }
	u := NewHandle(GetHandleSize(editHandle) - offset);
	err := MemError;
	If err <> NoErr THEN		{ if not enough memory set handle to Nil and leave }
		BEGIN
		DisposHandle(Handle(hbe));
		BENew := Nil;
		Exit(BeNew);
		END;

{ can't do the NewPixMap calls within the WITH statement }
{ scramble bug, But its Locked so maybe we can?!?##!??}
	hbe^^.editHandle := editHandle;
	WITH hbe^^ DO BEGIN
		editOffset := offset;
		beRowBytes := rowBytes;

		bePixelSize := pixelSize;
		beColorTable := colorTable;

		GetPort(beGrafPtr);
		IF BAND(beGrafPtr^.portBits.rowBytes, $0000C000) = $0000C000 THEN
			isColor := True
		ELSE
			isColor := False;

		beBounds := bounds;
		offHandle := Nil;
		offPix := Nil;

		beEraserSize.v := 1;
		beEraserSize.h := 1;

		bePenSize.v := 1;
		bePenSize.h := 1;

{ setup offscreen pixMaps for objects/undo }
		IF hbe^^.isColor THEN
			BEGIN
			dupPix := NewPixMap;
			error := MemError;
			IF error <> NoErr THEN
				BEGIN
				DisposHandle(Handle(hbe));
				BENew := Nil;
				Exit(BeNew);
				END;
			DisposHandle(Handle(dupPix^^.pmTable));
			dupPix^^.rowBytes := beRowBytes;
			dupPix^^.bounds := beBounds;
			dupPix^^.pixelSize := bePixelSize;
			dupPix^^.cmpSize := bePixelSize;
			dupPix^^.pmTable := beColorTable;
			END
		ELSE
			BEGIN
			dupBits.rowBytes := beRowBytes;
			dupBits.bounds := beBounds;
			END;
		dupHandle := p;

		IF hbe^^.isColor THEN
			BEGIN
			undoPix := NewPixMap;
			error := MemError;
			IF error <> NoErr THEN
				BEGIN
				DisposHandle(Handle(hbe));
				BENew := Nil;
				Exit(BeNew);
				END;
			DisposHandle(Handle(undoPix^^.pmTable));
			undoPix^^.rowBytes := beRowBytes;
			undoPix^^.bounds := beBounds;
			undoPix^^.pixelSize := bePixelSize;
			undoPix^^.cmpSize := bePixelSize;
			undoPix^^.pmTable := beColorTable;
			END
		ELSE
			BEGIN
			undoBits.rowBytes := beRowBytes;
			undoBits.bounds := beBounds;
			END;
		undoHandle := u;

		IF hbe^^.isColor THEN
			BEGIN
			editPix := NewPixMap;
			error := MemError;
			IF error <> NoErr THEN
				BEGIN
				DisposHandle(Handle(hbe));
				BENew := Nil;
				Exit(BeNew);
				END;
			DisposHandle(Handle(editPix^^.pmTable));
			editPix^^.rowBytes := beRowBytes;
			editPix^^.bounds := beBounds;
			editPix^^.pixelSize := bePixelSize;
			editPix^^.cmpSize := bePixelSize;
			editPix^^.pmTable := beColorTable;
			END;

		WITH selection DO
			BEGIN
			selHandle := Nil;
			selPix := Nil;
			maskHandle := Nil;
			maskPix := Nil;
			edgeHandle := Nil;
			edgePix := Nil;
			SetRect(selRect, 0, 0, 0, 0);
			END;
		undoSelection := selection;
		IF hbe^^.isColor THEN
			BEGIN
			NewSelPixMaps (hbe);
			IF MemError <> noErr THEN
				BEGIN
				DisposeSelPixMaps (hbe);
				DisposHandle(Handle(hbe));
				BENew := Nil;
				Exit(BeNew);
				END;
			END;
		undoFreeze := 0;
		oldIndex := -1;

		error := noErr;
		active := False;
		drawEnabled := True;

		saveClip := NewRgn;
		saveVis := NewRgn;
		sibling := hbe;

		IF hbe^^.isColor THEN
			BEGIN
			beGDevice := NewGDevice(0,-1);
			error := MemError;
			IF error <> NoErr THEN
				BEGIN
				DisposHandle(Handle(hbe));
				BENew := Nil;
				Exit(BeNew);
				END;
			DisposHandle(Handle(beGDevice^^.gdPMap^^.pmTable));
			{ Since beGDevice is not locked, must not dereference it on lhs if rhs may move memory }
			ctabTemp := ImageCt2DeviceCt (beColorTable, bePixelSize);
			beGDevice^^.gdPMap^^.pmTable := ctabTemp;
			SetDeviceAttribute(beGDevice, GDDevType, True);
			WITH beGDevice^^ DO
				BEGIN
				gdID := 0;
				gdType := 0;
				gdSearchProc := Nil;
				gdCompProc := Nil;
				gdPMap^^.baseAddr := Nil;
				gdPMap^^.rowBytes := beRowBytes;
				gdPMap^^.bounds := beBounds;
				gdPMap^^.pmVersion := 0;
				gdPMap^^.packType := 0;
				gdPMap^^.packSize := 0;
				gdPMap^^.hRes := 72;
				gdPMap^^.vRes := 72;
				gdPMap^^.pixelType := 0;
				gdPMap^^.pixelSize := bePixelSize;
				gdPMap^^.cmpCount := 1;
				gdPMap^^.cmpSize := bePixelSize;
				gdPMap^^.planeBytes := 0;
				gdPMap^^.pmReserved := 0;
				gdNextGD := Nil;
				gdRect := beBounds;
				gdMode := 0;
				gdReserved := 0;
				END;

			theDevice := GetGDevice;
			SetGDevice(beGDevice);
			MakeITable(Nil, Nil, 0);
			SetGDevice(theDevice);
			END;
	END;
	HUnlock(Handle(hbe));

	{ Initialize the undo buffer with the current bits in case the user does an undo first. }
	SetupUndo (hbe);	{ 7/7/88 PEA }
	BENew := hbe;
END; { BENew }


PROCEDURE CenterScrap(VAR r:Rect; hbe: BEHandle);
{ Center scrap rectangle in current window if it will fit; if not, align it with the topLeft }

VAR
	width,height: INTEGER;

BEGIN
	WITH hbe^^ DO
		BEGIN
		width := r.right - r.left;
		height := r.bottom - r.top;

		WITH r DO
			BEGIN
			left := (beEditRect.left + beEditRect.right - width) DIV 2;
			top  := (beEditRect.top + beEditRect.bottom - height) DIV 2;
			IF (left < beEditRect.left) THEN
				left := beEditRect.left;
			IF (top < beEditRect.top) THEN
				top := beEditRect.top;
			right := left + width;
			bottom := top + height;
			END;
		END;
END; { CenterScrap }


PROCEDURE BEPaste(hbe: BEHandle);
{ paste the picture from the scrap into the selection.  If no selection make one }

VAR
    err, o: LongInt;
	 pHndl: PicHandle;

BEGIN
	IF NOT EmptyRect(hbe^^.selection.selRect) AND (NOT hbe^^.marquee) THEN
		BEPutSelection(hbe)
	ELSE
		SetupUndo(hbe);

	pHndl := PicHandle(NewHandle(0));
	err := GetScrap(Handle(pHndl), 'PICT', o);

	IF err >= 0 THEN
		BEGIN
		IF EmptyRect(hbe^^.selection.selRect) THEN
			BEGIN
			hbe^^.selection.selRect := pHndl^^.picFrame;
			CenterScrap(hbe^^.selection.selRect, hbe);
			hbe^^.selection.selRect.right := hbe^^.selection.selRect.right - 1;
			hbe^^.selection.selRect.bottom := hbe^^.selection.selRect.bottom - 1;
			hbe^^.marquee := True;
			SetUpSelBits(hbe^^.selection.selRect, hbe);
			IF hbe^^.error <> NoErr THEN
				BEGIN
				SetRect(hbe^^.selection.selRect, 0, 0, 0, 0);
				DisposHandle(Handle(pHndl));
				Exit(BEPaste);
				END;
			END;

		IF NOT EmptyRect(hbe^^.selection.selRect) THEN
			BEGIN
			hbe^^.selection.wasErased := True;

			LockHands(hbe);
			DoQuickPortToDup(hbe);

			BEDrawBegin(hbe);
			IF hbe^^.isColor THEN
				BEGIN
				SetPortPix(hbe^^.selection.selPix);
				ClipRect(hbe^^.selection.selPix^^.bounds);
				RectRgn(hbe^^.beGrafPtr^.visRgn, hbe^^.selection.selPix^^.bounds);
				EraseRect(hbe^^.selection.selPix^^.bounds);
				DrawPicture(pHndl, hbe^^.selection.selPix^^.bounds);
				END
			ELSE
				BEGIN
				SetPortBits(hbe^^.selection.selBits);
				ClipRect(hbe^^.selection.selBits.bounds);
				RectRgn(hbe^^.beGrafPtr^.visRgn, hbe^^.selection.selBits.bounds);
				EraseRect(hbe^^.selection.selBits.bounds);
				DrawPicture(pHndl, hbe^^.selection.selBits.bounds);
				END;
			BEDrawEnd(hbe);

			{ Put the bits down so that other views derived from the bits can be updated.
				PEA 4/1/88 }
			BEDrawBegin (hbe);
			CopySelToPort (hbe);
			BEDrawEnd (hbe);

			hbe^^.marquee := True;

			{ And paint them - PEA 4/1/88 }
			BEInvalRect (hbe^^.selection.selRect, hbe);

			UnlockHands(hbe);

			END
		ELSE
			SysBeep(20);
		END;
	DisposHandle(Handle(pHndl));

END; { BEPaste }


PROCEDURE BEPtUnXForm(VAR pt: Point; hbe: BEHandle);
{ Transforms the given fatbit point into screen point }

BEGIN
    WITH hbe^^ DO BEGIN
        pt.h := beViewRect.left + (pt.h - beEditRect.left) * (fatbitsDelta.h + fatbitsSize.h);
        pt.v := beViewRect.top + (pt.v - beEditRect.top) * (fatbitsDelta.v + fatbitsSize.v);
    END;
END; { BEPtUnXForm }


PROCEDURE BEPtXForm(VAR pt: Point; hbe: BEHandle);
{ Transforms the given screen point into fatbits point }

BEGIN
    WITH hbe^^ DO BEGIN
        pt.h := beEditRect.left + (pt.h - beViewRect.left) DIV (fatbitsDelta.h + fatbitsSize.h);
        pt.v := beEditRect.top + (pt.v - beViewRect.top) DIV (fatbitsDelta.v + fatbitsSize.v);
    END;
END; { BEPtXForm }


PROCEDURE LineDraw(r: Rect; flags: BEDrawFlags; hbe: BEHandle);
{ draw a line between the points specified in topleft and bottomright of rect,
  constrained to horizontal, vertical, or diagonal lines according to flags }

VAR
	hv:			Point;
	lineFlags:	BEDrawFlags;
BEGIN
	WITH r DO BEGIN
		WidthHeightRect (r, hv);
		lineFlags := BAND(flags, fShapeLineMask);
		IF lineFlags = fLineOnAxis THEN
			BEGIN
			IF hv.v > hv.h THEN
				right := left
			ELSE
				bottom := top;
			END
		ELSE IF lineFlags = fLineOnDiagonal THEN
			BEGIN
			IF hv.h > hv.v*2 THEN
				bottom := top
			ELSE IF hv.v > hv.h*2 THEN
				right := left
			ELSE
				SquareRect (r);
			END;
		MoveTo(left, top);
		LineTo(right, bottom);
	END;
END; { LineDraw }


PROCEDURE BELine(startPt: Point; flags: BEDrawFlags; hbe: BEHandle);
{ Drag out a line starting at given point }

BEGIN
    BEObject(startPt, @LineDraw, flags, hbe);
END; { BELine }


PROCEDURE OvalDraw( r: Rect; flags: BEDrawFlags; hbe: BEHandle );
{ Draw an oval in r; flags specify if framed, filled, or both, and if circle }

BEGIN
	IF BAND (flags, fShapeUnbounded) = 0 THEN
		ClipAnchorRect (r, hbe^^.beEditRect);
	IF BAND (flags, fShapeSymmetric)  <> 0 THEN
		SquareRect (r);
	FixRect(r);
	StretchRect(r);
	IF BAND (flags, fShapeFill) <> 0 THEN
		IF hbe^^.isColor THEN
			FillCOval(r, hbe^^.bePixPat)
		ELSE
			FillOval(r, hbe^^.beFillPat);
	IF BAND (flags, fShapeUnframed) = 0 THEN
		BEGIN
		PenSize(hbe^^.bePenSize.h, hbe^^.bePenSize.v);
		FrameOval(r);
		END;
END; { OvalDraw }


PROCEDURE BEOval(startPt: Point; flags: BEDrawFlags; hbe: BEHandle);
{ Drag out an oval starting at given point }

BEGIN
    BEObject(startPt, @OvalDraw, flags, hbe);
END; { BEOval }


PROCEDURE RndRectDraw( r: Rect; flags: BEDrawFlags; hbe: BEHandle );
{ Draw a round-cornered rect }

BEGIN
	IF BAND (flags, fShapeUnbounded) = 0 THEN
		ClipAnchorRect (r, hbe^^.beEditRect);
	IF BAND (flags, fShapeSymmetric) <> 0 THEN
		SquareRect (r);
	FixRect(r);
	StretchRect(r);
	IF BAND (flags, fShapeFill) <> 0 THEN
		IF hbe^^.isColor THEN
			FillCRoundRect(r, Point(hbe^^.beDrawParm).v, Point(hbe^^.beDrawParm).h, hbe^^.bePixPat)
		ELSE
			FillRoundRect(r, Point(hbe^^.beDrawParm).v, Point(hbe^^.beDrawParm).h, hbe^^.beFillPat);
	IF BAND (flags, fShapeUnframed) = 0 THEN
		BEGIN
		PenSize(hbe^^.bePenSize.h, hbe^^.bePenSize.v);
		FrameRoundRect(r, Point(hbe^^.beDrawParm).v, Point(hbe^^.beDrawParm).h);
		END;
END; { RndRectDraw }


PROCEDURE BERoundRect(startPt: Point; flags: BEDrawFlags; ovalWidth, ovalHeight: INTEGER; hbe: BEHandle);
{ Drag out a rect starting at given point }

BEGIN
	 Point(hbe^^.beDrawParm).v := ovalWidth;
	 Point(hbe^^.beDrawParm).h := ovalHeight;
    BEObject(startPt, @RndRectDraw, flags, hbe);
END; { BERoundRect }


PROCEDURE RectDraw(r: Rect; flags: BEDrawFlags; hbe: BEHandle);
{ Draw a rect }

BEGIN
	IF BAND (flags, fShapeUnbounded) = 0 THEN
		ClipAnchorRect (r, hbe^^.beEditRect);
	IF BAND (flags, fShapeSymmetric) <> 0 THEN
		SquareRect (r);
	FixRect(r);
	StretchRect(r);
	IF BAND (flags, fShapeFill) <> 0 THEN
		IF hbe^^.isColor THEN
			FillCRect(r, hbe^^.bePixPat)
		ELSE
			FillRect(r, hbe^^.beFillPat);
	IF BAND (flags, fShapeUnframed) = 0 THEN
		BEGIN
		PenSize(hbe^^.bePenSize.h, hbe^^.bePenSize.v);
		FrameRect(r);
		END;
END; { RectDraw }


PROCEDURE BERect(startPt: Point; flags: BEDrawFlags; hbe: BEHandle);
{ Drag out a rect starting at given point }

BEGIN
    BEObject(startPt, @RectDraw, flags, hbe);
END; { BERect }


PROCEDURE ClipAnchorRect (VAR r: Rect; VAR rClip: Rect);
{ Constrains the bottom right corner of r to lie within a rectangle
  defined by rClip shrunk on the bottom and right by one pixel.
	rClip is not changed.
}

BEGIN
	IF r.right >= rClip.right THEN
		r.right := rClip.right-1
	ELSE IF r.right < rClip.left THEN
		r.right := rClip.left;
	IF r.bottom >= rClip.bottom THEN
		r.bottom := rClip.bottom-1
	ELSE IF r.bottom < rClip.top THEN
		r.bottom := rClip.top;
END; { ClipAnchorRect }


PROCEDURE WidthHeightRect (VAR r: Rect; VAR pt: Point);

BEGIN
	pt.h := ABS (r.right - r.left);
	pt.v := ABS (r.bottom - r.top);
END; { WidthHeightRect }


PROCEDURE SquareRect (VAR r: Rect);
{ Convert a rectangle to the smallest contained square }

VAR
	height,width,hs,ws: INTEGER;
BEGIN
	IF r.right < r.left THEN
		BEGIN
		ws := -1;
		width := r.left - r.right;
		END
	ELSE
		BEGIN
		ws := 1;
		width := r.right - r.left;
		END;
	IF r.bottom < r.top THEN
		BEGIN
		hs := -1;
		height := r.top - r.bottom;
		END
	ELSE
		BEGIN
		hs := 1;
		height := r.bottom - r.top;
		END;

	IF width > height THEN
		r.right := r.left + (height*ws)
	ELSE
		r.bottom := r.top + (width*hs);
END; { SquareRect }


PROCEDURE MarqueeDraw(r: Rect; flags: BEDrawFlags; hbe: BEHandle);
{ If the fSetSelection bit of flags is false, uses the current selection rectangle rather than setting it;
  this allows the curent selection to remain partially outside the edit rectangle }

VAR
	mIndex: INTEGER;
	savePat: Pattern;
	tempRect: Rect;
	t: Boolean;
	myBlack: RGBColor;

BEGIN
	WITH myBlack DO
		BEGIN
		red := 0; blue := 0; green := 0;
		END;

	IF BAND (flags, fSquareMarquee)  <> 0 THEN
		SquareRect (r);
	FixRect(r);

	WITH hbe^^ DO
		BEGIN
		IF BAND (flags, fSetSelection) <> 0 THEN
			BEGIN
			t := SectRect(r, beEditRect, r);
			IF r.right = beEditRect.right THEN r.right := r.right - 1;
			IF r.bottom = beEditRect.bottom THEN r.bottom := r.bottom - 1;
			selection.selRect := r;
			END
		ELSE
			r := selection.selRect;
		StretchRect(r);

		GetMIndex(mIndex);

		IF NOT EmptyRect(selection.selRect) AND active THEN
			BEGIN
			PenSize(1,1);
			GetSelPat(mIndex, savePat);
			PenPat(savePat);
			IF isColor THEN
				RGBForeColor(myBlack);
			FrameRect(r);
			oldIndex := mIndex;
			END;
		END;
END; { MarqueeDraw }


PROCEDURE SetUpSelBits(r: Rect; hbe: BEHandle);

VAR
	p: Handle;
	pix: PixMapHandle;
	rb: INTEGER;
	b: BOOLEAN;

BEGIN
	HLock(Handle(hbe));
	WITH hbe^^.selection DO
		BEGIN
		IF hbe^^.marquee THEN
			BEGIN
			StretchRect(r);
			END;

		IF hbe^^.isColor THEN
		{ setup a temporary offscreen pixMap the size of the selection }
			WITH selPix^^ DO
				BEGIN
				rowBytes := ((((hbe^^.bePixelSize * (r.right - r.left)) + 15)) DIV 16) * 2;
				bounds := r;
				rb := rowBytes;
				rowBytes := rowBytes + $8000;
				END
		ELSE
			WITH selBits DO
				BEGIN
				rowBytes := (((r.right - r.left) + 15) DIV 16) * 2;
				bounds := r;
				rb := rowBytes;
				END;

		p := NewHandle(UIntMul((r.bottom - r.top), rb));
		hbe^^.error := MemError;
		If hbe^^.error <> NoErr THEN
			BEGIN
			HUnlock(Handle(hbe));
			Exit(SetUpSelBits);
			END;
		selHandle := p;
		END;

	HUnlock(Handle(hbe));
	LockHands(hbe);

{ copy from the dupBits to the selBits bitmap }
	IF hbe^^.marquee THEN
		DupToSel(hbe);

	UnlockHands(hbe);

END; { SetUpSelBits }


PROCEDURE EraseUnderSelection (hbe: BEHandle);
{ erase area under the selection prior to moving the selected bits }

VAR
	r, srcRect: Rect;

BEGIN
	IF NOT hbe^^.selection.wasErased THEN
		BEGIN
		LockHands(hbe);
		WITH hbe^^ DO
			BEGIN
			SetupRects(r, srcRect, hbe);

			BEDrawBegin(hbe);
			MySetDupBits(hbe);
			IF marquee THEN
				EraseRect(r)
			ELSE
				MaskToDup(hbe);
			BEDrawEnd(hbe);
			selection.wasErased := True;
			END;
		UnlockHands(hbe);
		END;
END; { EraseUnderSelection }


PROCEDURE DragSelection(startPt: Point; flags: BESelectFlags; hbe: BEHandle);

VAR
	testPt, deltaPt, endPt: Point;
	oldSelRect, r, cRect, srcRect: Rect;
	pt: Ptr;
	rLimit: Rect;
	vhSel: Point;

BEGIN
	SetupUndo(hbe);

	BEPtXForm(startPt, hbe);
	testPt := startPt;

	IF BAND(flags,fDragCopy) = 0 THEN
		EraseUnderSelection (hbe)
	ELSE IF hbe^^.selection.wasErased THEN
		BEGIN
	{ plunk down bits }
		IF hbe^^.marquee THEN
			CSelToDup(hbe)
		ELSE
			BEGIN
			MaskToDup(hbe);
			OSelToDup(hbe);
			END;
		END
	ELSE
		hbe^^.selection.wasErased := TRUE;

	SetupRects(r, srcRect, hbe);

	LockHands(hbe);

{ figure out bounding rect within which the drag point must remain }
	WITH hbe^^, cRect DO
		BEGIN
		IF marquee THEN
			BEGIN
			{ Allow selection to move beyond edit rectangle, as long as one edge is visible }
			vhSel.h := selection.selRect.right - selection.selRect.left;
			vhSel.v := selection.selRect.bottom - selection.selRect.top;
			rLimit.top := beEditRect.top - vhSel.v;
			rLimit.left := beEditRect.left - vhSel.h;
			rLimit.bottom := beEditRect.bottom + vhSel.v;
			rLimit.right := beEditRect.right + vhSel.h;
			END
		ELSE
			{ For lasso selections, keep entire selection within edit rectangle }
			rLimit := beEditRect;

		left := rLimit.left + (startPt.h - selection.selRect.left);
		top := rLimit.top + (startPt.v - selection.selRect.top);
		right := rLimit.right - (selection.selRect.right - startPt.h);
		bottom := rLimit.bottom - (selection.selRect.bottom - startPt.v);
		IF NOT marquee THEN
			bottom := bottom + 1;
		END;

{ draw image without ants on mousedown }
	BEDrawBegin(hbe);
	DupToPort(hbe);
	CopySelToPort(hbe);
	BEDrawEnd(hbe);

{ invalidate and redraw the rects }
	BEInvalRect(r, hbe);

	WHILE WaitMouseUp DO
		BEGIN
			GetMouse(endPt);
			BEPtXForm(endPt, hbe);

			endPt := Point(PinRect(cRect, endPt));

		IF (LongInt(endPt) <> LongInt(testPt)) THEN
			BEGIN { mouse moved }
			deltaPt := endPt;

		{ figure out distance moved }
			SubPt(testPt, deltaPt);

			oldSelRect := r;
			OffSetRect(r, deltaPt.h, deltaPt.v);

		{ get into BEBits and move the image }
			BEDrawBegin(hbe);

			UnionRect(oldSelRect, r, oldSelRect);

		{ copy from dupBits to BEBits }
			WITH hbe^^ DO
			IF isColor THEN
				{ cc }
				CopyPixels (dupPix, CGrafPtr(beGrafPtr)^.portPixMap)
				{ CopyBits(BitMapPtr(dupPix^)^, beGrafPtr^.portBits, oldSelRect, oldSelRect, srcCopy, Nil) }
			ELSE
				CopyBits(dupBits, beGrafPtr^.portBits, oldSelRect, oldSelRect, srcCopy, Nil);

			IF hbe^^.marquee THEN
				CSelToPort(hbe, r, True)
			ELSE
				BEGIN
				MaskToPort(hbe, r, True);
				OSelToPort(hbe, r, True);
				END;

			BEDrawEnd(hbe);

		{ invalidate and redraw the rects }
			BEInvalRect(oldSelRect, hbe);

			testPt := endPt;

			END; { mouse moved }

		END; { waitmouseup loop }

	r.bottom := r.bottom - 1;
	r.right := r.right - 1;
	hbe^^.selection.selRect := r;

	UnlockHands(hbe);

END; { DragSelection }


PROCEDURE BESelect(startPt: Point; flags: BESelectFlags; hbe: BEHandle);
{ Drag out a selection marquee or lasso starting at given point.
  The selection is moved if startPt is inside the current selection,
  even if that selection type is different than that specified in flags. }

VAR
	kind:	BESelectFlags;
BEGIN
	IF BEInSelection(startPt, hbe) THEN
		DragSelection(startPt, flags, hbe)
	ELSE
		BEGIN
		kind := BAND (flags, fSelectKindMask);
		IF kind = fSelectLasso THEN
			DrawLasso(startPt, hbe)
		ELSE
			BEGIN
			flags := BAND(flags, fSquareMarquee);
			BEObject(startPt, @MarqueeDraw, BOR(flags, fSetSelection), hbe);
			hbe^^.marquee := True;
			IF NOT EmptyRect(hbe^^.selection.selRect) THEN
				BEGIN
				SetUpSelBits(hbe^^.selection.selRect, hbe);
				IF hbe^^.error <> NoErr THEN
					SetRect(hbe^^.selection.selRect, 0, 0, 0, 0);
				END;
			hbe^^.selection.wasErased := False;
			END
		END;
END; { BESelect }


PROCEDURE ConstrainNudge (VAR deltaPt: Point; hbe: BEHandle);

VAR
	cRect: Rect;
	newPt: Point;

BEGIN
	WITH hbe^^, cRect DO
		BEGIN
		IF marquee THEN
			BEGIN
			{ Allow selection to move beyond edit rectangle, as long as one edge is visible }
			top := beEditRect.top - (selection.selRect.bottom - selection.selRect.top);
			left := beEditRect.left - (selection.selRect.right - selection.selRect.left);
			botRight := beEditRect.botRight;
			END
		ELSE
			BEGIN
			{ For lasso selections, keep entire selection within edit rectangle }
			topLeft := beEditRect.topLeft;
			right := beEditRect.right - (selection.selRect.right - selection.selRect.left);
			bottom := beEditRect.bottom - (selection.selRect.bottom - selection.selRect.top) + 1;
			END;

	{ constrain deltaPt }
		newPt := selection.selRect.topLeft;
		AddPt (deltaPt, newPt);
		newPt := Point(PinRect(cRect, newPt));
		deltaPt := newPt;
		SubPt (selection.selRect.topLeft, deltaPt);
		END;
END; { ConstrainNudge }


PROCEDURE BEDuplicate (deltaPt: Point; hbe: BEHandle);
{ Duplicate the selection.  DeltaPt is the preferred displacement and the minimal
  margin on the bottom and right.  It should be positive.
 }

VAR
	rInval:		Rect;
	origPt:		Point;

BEGIN
	IF BESelection(hbe) THEN
		BEGIN
		SetupUndo (hbe);

		LockHands(hbe);

	{ plunk down bits }
		IF hbe^^.marquee THEN
			CSelToDup(hbe)
		ELSE
			BEGIN
			MaskToDup(hbe);
			OSelToDup(hbe);
			END;

	{ draw image without ants }
		BEDrawBegin(hbe);
		DupToPort(hbe);

		rInval := hbe^^.selection.selRect;

	{ Move Selection }
	{ If too close to bottom, move to the top; if too close to right edge, move to left.
	  The term "deltaPt.v <> origPt.v" is for lasso selections (which are constrained to the
	  edit area; the second term is for marquee selections (which may overhang).
	}
		origPt := deltaPt;
		ConstrainNudge (deltaPt, hbe);
		WITH hbe^^, hbe^^.selection DO
			BEGIN
			IF (deltaPt.v <> origPt.v) OR ((selRect.top + deltaPt.v) > (beEditRect.bottom - deltaPt.v)) THEN
				deltaPt.v := (beEditRect.top + deltaPt.v) - selRect.top;
			IF (deltaPt.h <> origPt.h) OR ((selRect.left + deltaPt.h) > (beEditRect.right - deltaPt.h)) THEN
				deltaPt.h := (beEditRect.left + deltaPt.h) - selRect.left;
			OffsetRect (selRect, deltaPt.h, deltaPt.v);
			END;

		CopySelToPort (hbe);
		BEDrawEnd(hbe);

		hbe^^.selection.wasErased := True;

		UnlockHands(hbe);

		UnionRect (rInval, hbe^^.selection.selRect, rInval);
		StretchRect (rInval);
		BEInvalRect(rInval, hbe);
		END;
END; { BEDuplicate }


FUNCTION  BENudge(deltaPt: Point; hbe: BEHandle): BOOLEAN;

VAR
	oldSelRect, r, srcRect: Rect;

BEGIN
	BENudge := FALSE;
	IF BESelection(hbe) THEN
		BEGIN
		SetupUndo (hbe);

		EraseUnderSelection (hbe);

	{ draw image without ants }
		LockHands(hbe);
		BEDrawBegin(hbe);
		DupToPort(hbe);
		BEDrawEnd(hbe);
		UnlockHands(hbe);

		ConstrainNudge (deltaPt, hbe);

		IF LongInt(DeltaPt) <> 0 THEN
			BEGIN
			LockHands(hbe);

			SetupRects(r, srcRect, hbe);

		{ invalidate and redraw the rects }
			BEInvalRect(r, hbe);

			oldSelRect := r;
			OffSetRect(r, deltaPt.h, deltaPt.v);

		{ get into BEBits and move the image }
			BEDrawBegin(hbe);

			UnionRect(oldSelRect, r, oldSelRect);

		{ copy from dupBits to BEBits }
			WITH hbe^^ DO
			IF isColor THEN
				{ cc }
				CopyPixels (dupPix, CGrafPtr(beGrafPtr)^.portPixMap)
				{ CopyBits(BitMapPtr(dupPix^)^, beGrafPtr^.portBits, oldSelRect, oldSelRect, srcCopy, Nil) }
			ELSE
				CopyBits(dupBits, beGrafPtr^.portBits, oldSelRect, oldSelRect, srcCopy, Nil);

			IF hbe^^.marquee THEN
				CSelToPort(hbe, r, True)
			ELSE
				BEGIN
				MaskToPort(hbe, r, True);
				OSelToPort(hbe, r, True);
				END;

			BEDrawEnd(hbe);

		{ invalidate and redraw the rects }
			BEInvalRect(oldSelRect, hbe);

			r.bottom := r.bottom - 1;
			r.right := r.right - 1;
			hbe^^.selection.selRect := r;

			UnlockHands(hbe);
			BENudge := TRUE;
			END
		END;

END; { BENudge }


PROCEDURE BEFlip(vertical: BOOLEAN; hbe: BEHandle);
{ flip the selection; if there is no selection or a lasso selection, flip the entire image }
{ Note that a lasso selection is canceled before fliping the entire image }

VAR
	rInval, srcRect:	Rect;
	r:						Rect;
	rgbPt:				RGBColor;
	cpyHandle:			Handle;			{ copy of selection bits }
	cpyPix:				PixMapHandle;
	cpyBits:				BitMap;
	cpyRect, selRect: Rect;
	i, limit:			INTEGER;

BEGIN
	IF BESelection(hbe) AND hbe^^.marquee THEN
		BEGIN
		SetupUndo(hbe);

		SetupRects(rInval, srcRect, hbe);

		LockHands(hbe);

		WITH hbe^^ DO
			BEGIN
			{ Copy the selection }
			cpyHandle := selection.selHandle;
			error := HandToHand(cpyHandle);
			IF error <> NoErr THEN Exit(BEFlip);
			HLock (cpyHandle);

			IF isColor THEN
				BEGIN
				cpyPix := selection.selPix;
				error := HandToHand(Handle (cpyPix));
				IF error <> NoErr THEN
					BEGIN
					HUnlock (cpyHandle);
					DisposHandle (cpyHandle);
					Exit(BEFlip);
					END;
				HLock (Handle (cpyPix));
				cpyPix^^.baseAddr := cpyHandle^;
				r := selection.selPix^^.bounds;
				END
			ELSE
				BEGIN
				cpyBits := selection.selBits;
				cpyBits.baseAddr := cpyHandle^;
				r := selection.selBits.bounds;
				END;

			BEDrawBegin(hbe);

			IF vertical THEN
				limit := r.bottom - r.top
			ELSE
				limit := r.right - r.left;

			FOR i := 0 TO limit DO
				BEGIN
				IF vertical THEN
					BEGIN
					SetRect (cpyRect, r.left, r.top+i, r.right, r.top+i+1);
					SetRect (selRect, r.left, r.bottom-i-1, r.right, r.bottom-i);
					END
				ELSE
					BEGIN
					SetRect (cpyRect, r.left+i, r.top, r.left+i+1, r.bottom);
					SetRect (selRect, r.right-i-1, r.top, r.right-i, r.bottom);
					END;
				IF isColor THEN
					CopyBits(BitMapPtr(cpyPix^)^, BitMapPtr(selection.selPix^)^, cpyRect, selRect, srcCopy, Nil)
				ELSE
					CopyBits(cpyBits, selection.selBits, cpyRect, selRect, srcCopy, Nil);
				END;

			HUnlock (cpyHandle);
			DisposHandle (cpyHandle);
			IF isColor THEN
				BEGIN
				HUnlock (Handle (cpyPix));
				DisposHandle (Handle (cpyPix));
				END;

			DupToPort(hbe);
			CopySelToPort(hbe);

			BEDrawEnd(hbe);
			UnlockHands(hbe);
			END;

	{ invalidate and redraw the rects }
		BEInvalRect(rInval, hbe);

		END;
END; { BEFlip }


PROCEDURE SetupExtract (bm: BitMapPtr; pt: Point; VAR offset, mask, shift: INTEGER);
{	Returns offset, mask and shift.  Offset is the displacement from baseAddr to the byte containing
	the argument point.  Mask can be used to extract the pixel, and shift used to move the pixel
	value into the least significant bits of the byte.  This routine uses integer math to compute
	bit offsets, so the width of the BitMap should be less than 4K for 8-bit pixels.
}

VAR
	pixelSize:	INTEGER;
	rowBytes:	INTEGER;
	row:			INTEGER;
	rowOffset:	INTEGER;
	pixOffset:	INTEGER;
	bitOffset:	INTEGER;
	byteOffset:	INTEGER;
	pm:			PixMapPtr;

BEGIN
IF BAND (bm^.rowBytes, $8000) <> 0 THEN
	BEGIN
	pm := PixMapPtr (bm);
	pixelSize := pm^.pixelSize;
	END
ELSE
	pixelSize := 1;
rowBytes := BAND (bm^.rowBytes, $7FFF);
row := pt.v - bm^.bounds.top;
rowOffset := UIntMul (row, rowBytes);
pixOffset := pt.h - bm^.bounds.left;
bitOffset := UIntMul(pixOffset,pixelSize);
byteOffset := bitOffset DIV 8;
bitOffset := bitOffset MOD 8;
offset := rowOffset + byteOffset;
shift := (8 - pixelSize - bitOffset);
mask := BitShift(1, pixelSize) - 1;
mask := BitShift(mask, shift);
END;

FUNCTION GetPix (bm: BitMapPtr; pt: Point): INTEGER;

VAR
	byte: INTEGER;
	offset, mask, shift: INTEGER;
	p:	Ptr;

BEGIN
IF PtInRect (pt, bm^.bounds) THEN
	BEGIN
	SetupExtract (bm, pt, offset, mask, shift);
	p := POINTER(ORD(bm^.baseAddr) + offset);
	byte := p^;
	GetPix := BitShift (BAND (byte, mask), -shift);
	END
ELSE
	GetPix := 0;
END; { GetPix }


PROCEDURE SetPix (bm: BitMapPtr; pt: Point; VAR pixel: INTEGER);

VAR
	byte: INTEGER;
	offset, mask, shift: INTEGER;
	p:	Ptr;

BEGIN
IF PtInRect (pt, bm^.bounds) THEN
	BEGIN
	SetupExtract (bm, pt, offset, mask, shift);
	p := POINTER(ORD(bm^.baseAddr) + offset);
	byte := BAND (p^, BXOR ($FFFF, mask));
	pixel := BAND (BitShift (pixel,shift), mask);
	byte := BOR (byte, pixel);
	p^ := byte;
	END
END; { SetPix }


PROCEDURE BERotate(hbe: BEHandle);
{ Requires a selection in order to perform a rotate, since rotate may move some bits off the
  edit area--without a selection these bits are lost forever.  For now, this selection must be
  a rectangular marquee.
 }

VAR
	rInval, srcRect:	Rect;
	rectBounds:			Rect;
	rotHandle:			Handle;
	rotPix:				PixMapHandle;
	rotBits:				BitMap;
	hSel, vSel:			INTEGER;
	ptSel, ptRot:		Point;
	widthSel:			INTEGER;
	width, height:		INTEGER;
	pixel:				INTEGER;
	vhAdjust:			Point;

BEGIN
	IF NOT EmptyRect(hbe^^.selection.selRect) AND (hbe^^.marquee) THEN
		BEGIN
		SetupUndo(hbe);

		EraseUnderSelection (hbe);

		SetupRects(rInval, srcRect, hbe);
		LockHands(hbe);

		WITH hbe^^ DO
			BEGIN
			{ Copy the selection }
			IF isColor THEN
				BEGIN
				{ Make sure topLeft of selection is at (0,0) }
				OffsetRect (selection.selPix^^.bounds, -selection.selPix^^.bounds.left, -selection.selPix^^.bounds.top);

				rotPix := selection.selPix;
				error := HandToHand(Handle (rotPix));
				IF error <> NoErr THEN
					Exit(BERotate);
				HLock (Handle (rotPix));

				{ Bounds of selPix are (0,0,hbe,v); bounds of rotated image are (0,0,v,hbe) }
				WITH rotPix^^ DO
					BEGIN
					bounds.bottom := selection.selPix^^.bounds.right;
					bounds.right := selection.selPix^^.bounds.bottom;
					rowBytes := ((((pixelSize * (bounds.right - bounds.left)) + 15)) DIV 16) * 2;
					rotHandle := NewHandle (UIntMul((bounds.bottom - bounds.top), rowBytes));
					hbe^^.error := MemError;
					IF hbe^^.error <> NoErr THEN
						BEGIN
						HUnlock (Handle (rotPix));
						DisposHandle (Handle (rotPix));
						Exit(BERotate);
						END;
					rowBytes := rowBytes + $8000;
					HLock (rotHandle);
					baseAddr := rotHandle^;
					END;
				rectBounds := selection.selPix^^.bounds;
				END
			ELSE
				BEGIN
				{ Make sure topLeft of selection is at (0,0) }
				OffsetRect (selection.selBits.bounds, -selection.selBits.bounds.left, -selection.selBits.bounds.top);
				rotBits := selection.selBits;
				rotBits.bounds.bottom := selection.selBits.bounds.right;
				rotBits.bounds.right := selection.selBits.bounds.bottom;
				rotBits.rowBytes := (((rotBits.bounds.right - rotBits.bounds.left) + 15) DIV 16) * 2;
				rotHandle := NewHandle (UIntMul((rotBits.bounds.bottom - rotBits.bounds.top), rotBits.rowBytes));
				hbe^^.error := MemError;
				IF hbe^^.error <> NoErr THEN
					Exit(BERotate);
				HLock (rotHandle);
				rotBits.baseAddr := rotHandle^;
				rectBounds := selection.selBits.bounds;
				END;

			{ Map pixels from selection to rotated image }
			widthSel := rectBounds.right - rectBounds.left;
			FOR vSel := rectBounds.top TO rectBounds.bottom-1 DO
				FOR hSel := rectBounds.left TO rectBounds.right-1 DO
					BEGIN
					SetPt (ptSel, hSel, vSel);
					SetPt (ptRot, vSel, -hSel-1+widthSel);
					IF hbe^^.isColor THEN
						BEGIN
						pixel := GetPix (BitMapPtr (selection.selPix^), ptSel);
						SetPix (BitMapPtr (rotPix^), ptRot, pixel);
						END
					ELSE
						BEGIN
						pixel := GetPix (@selection.selBits, ptSel);
						SetPix (@rotBits, ptRot, pixel);
						END;
					END;

			{ Replace selection with rotated image }
			DisposHandle(selection.selHandle);
			selection.selHandle := rotHandle;
			IF isColor THEN
				BEGIN
				MyDisposPixMap(selection.selPix);
				selection.selPix := rotPix;
				END
			ELSE
				selection.selBits := rotBits;

			{ Adjust selRect to keep center of selection stationary }
			WITH selection.selRect DO
				BEGIN
				{ Note that former selection width is now bounds.bottom of rotPix or rotBit }
				width := right - left;
				height := bottom - top;
				top := top + (height - width) DIV 2;
				left := left + (width - height) DIV 2;
				bottom := top + width;
				right := left + height;

				{ But don't allow selection to move completely beyond the edit rectangle }
				IF bottom <= beEditRect.top THEN
					vhAdjust.v := (beEditRect.top+1) - bottom
				ELSE IF top >= beEditRect.bottom THEN
					vhAdjust.v := (beEditRect.bottom-1) - top
				ELSE
					vhAdjust.v := 0;
				IF right <= beEditRect.left THEN
					vhAdjust.h := (beEditRect.left+1) - right
				ELSE IF left >= beEditRect.right THEN
					vhAdjust.h := (beEditRect.right-1) - left
				ELSE
					vhAdjust.h := 0;
				OffsetRect (selection.selRect,  vhAdjust.h, vhAdjust.v);
				END;

			{ Must invalidate area under selection before and after rotation }
			UnionRect (rInval, selection.selRect, rInval);

			{ Copy rotated selection to image }
			BEDrawBegin(hbe);
			DupToPort(hbe);
			CopySelToPort(hbe);
			BEDrawEnd(hbe);

			UnlockHands(hbe);

			END;

	{ invalidate and redraw the rects }
		BEInvalRect(rInval, hbe);
	END;
END; { BERotate }


PROCEDURE BESetSelect(r: Rect; hbe: BEHandle);
{ sets the selection rectangle to the given rect, removing existing one if there }

VAR
	b:		BOOLEAN;

BEGIN
	IF BESelection(hbe) THEN
		BEKillSelection(hbe)
	ELSE
		SetupUndo(hbe);

	LockHands(hbe);
	DoQuickPortToDup(hbe);
	UnlockHands(hbe);

	WITH hbe^^ DO
		BEGIN
		BEPtXForm(r.topLeft, hbe);
		BEPtXForm(r.botRight, hbe);
		b := SectRect(r, beEditRect, r);
		IF r.right = beEditRect.right THEN r.right := r.right - 1;
		IF r.bottom = beEditRect.bottom THEN r.bottom := r.bottom - 1;
		selection.selRect := r;
		selection.wasErased := False;
		marquee := True;
		END;

	IF NOT EmptyRect(hbe^^.selection.selRect) THEN
		BEGIN
		SetUpSelBits(hbe^^.selection.selRect, hbe);
		IF hbe^^.error <> NoErr THEN
			SetRect(hbe^^.selection.selRect, 0, 0, 0, 0);
		END;

END; { BESetSelect }


PROCEDURE DrawLasso(startPt: Point; hbe: BEHandle);
{ routine to implement arbitrary selection }

VAR
	words: INTEGER;
	lassoRgn, tempRgn: RgnHandle;
	savePt, pt: Point;
	selBBox, r: Rect;
	s: Handle;
	tempBits, mTBits: BitMap;
	memLimit, theSize: LONGINT;
	i: INTEGER;
	tempTable, saveTable: CTabHandle;
	saveColor, myBlack: RGBColor;
	np: PixMapHandle;

BEGIN
{ make sure there is no selection }
	IF BESelection(hbe) THEN
		BEPutSelection(hbe)
	ELSE
		SetupUndo(hbe);

{ make a copy of the BEBits into dupBits buffer }
	LockHands(hbe);
	DoQuickPortToDup(hbe);
	UnlockHands(hbe);

	OpenRgn;
	ShowPen;

	savePt := startPt;

	WITH myBlack DO
		BEGIN
		red := 0; blue := 0; green := 0;
		END;

	saveColor := hbe^^.bePixColor;
	hbe^^.bePixColor := myBlack;
	PaintBits(True, False, savePt, startPt, hbe);

	memLimit := BitShift(CompactMem(maxSize), -1);
	IF memLimit > 15000 THEN memLimit := 15000;

	WHILE WaitMouseUp AND (GetRgnMax < memLimit) DO
		BEGIN
		GetMouse(pt);
		BEPtXForm(pt, hbe);
		pt := Point(PinRect(hbe^^.beEditRect, pt));
      IF pt.v = hbe^^.beEditRect.bottom THEN pt.v := pt.v - 1;
      IF pt.h = hbe^^.beEditRect.right  THEN pt.h := pt.h - 1;
		BEPtUnXform(pt, hbe);

		IF LONGINT(savePt) <> LONGINT(pt) THEN
			PaintBits(True, False, savePt, pt, hbe);

		LONGINT(savePt) := LONGINT(pt);
		END;

	PaintBits(True, False, savePt, startPt, hbe);
	hbe^^.bePixColor := saveColor;

{ got the region! }
	lassoRgn := NewRgn;
	CloseRgn(lassoRgn);

{ if it's empty, just blit bits back to screen }
	IF NOT EmptyRgn(lassoRgn) THEN
		BEGIN
	{ enclose all the pixels which we lassoed }
		tempRgn := NewRgn;
		CopyRgn(lassoRgn, tempRgn);
		OffSetRgn(tempRgn, 1, 0);
		UnionRgn(tempRgn, lassoRgn, lassoRgn);
		OffSetRgn(tempRgn, 0, 1);
		UnionRgn(tempRgn, lassoRgn, lassoRgn);
		OffSetRgn(tempRgn, -1, 0);
		UnionRgn(tempRgn, lassoRgn, lassoRgn);

	{ get the rectangle }
		selBBox := lassoRgn^^.rgnBBox;

		tempBits.bounds := selBBox;
		tempBits.rowBytes := ((((selBBox.right - selBBox.left) + 15)) DIV 16) * 2;
		tempBits.baseAddr := NewPtrClr(UIntMul((selBBox.bottom - selBBox.top),tempBits.rowBytes));
		hbe^^.error := MemError;
		IF hbe^^.error <> NoErr THEN
			Exit(DrawLasso);

		LockHands(hbe);

		WITH hbe^^ DO
			IF isColor THEN
				BEGIN
				tempTable := dupPix^^.pmTable;
				error := HandToHand(Handle(tempTable));
				IF error <> NoErr THEN Exit(DrawLasso);

				GetBWTable(tempTable);

				saveTable := dupPix^^.pmTable;
				dupPix^^.pmTable := tempTable;

			{ get a copy of the selected bits into temporary bitmap (black/white only) }
				CopyBits(BitMapPtr(dupPix^)^, tempBits, selBBox, selBBox, srcCopy, lassoRgn);

				dupPix^^.pmTable := saveTable;
				END
			ELSE
				CopyBits(dupBits, tempBits, selBBox, selBBox, srcCopy, lassoRgn);

		UnlockHands(hbe);

	{ trim the rectangle }
	{ must give TrimBBox a 0,0 upper coord }
		OffSetRect(selBBox, -tempBits.bounds.left, -tempBits.bounds.top);
		TrimBBox(tempBits.baseAddr, selBBox, tempBits.rowBytes);
		selBBox.bottom := selBBox.bottom + 1;

	{ now offset rectangle back to where it was }
		OffSetRect(selBBox, tempBits.bounds.left, tempBits.bounds.top);

	{ don't need tempBits ptr anymore }
		DisposPtr(tempBits.baseAddr);

		IF NOT EmptyRect(selBBox) THEN
			BEGIN

		{ flag that this is a lasso selection }
			hbe^^.marquee := False;

			WITH hbe^^.selection DO
				BEGIN
			{ it was not erased yet }
				wasErased := False;

			{ setup selRect the way that all routines expect it }
				selRect := selBBox;
				selRect.right := selRect.right - 1;
				selRect.bottom := selRect.bottom - 1;
				END;

		{ expand the bitmap into which we place the bits }
			r := selBBox;
			InsetRect(r, -1, -1);

		{ setup the selection bitmap }
			SetUpSelBits(r, hbe);
			IF hbe^^.error <> NoErr THEN
				BEGIN
				SetRect(hbe^^.selection.selRect, 0, 0, 0, 0);
				Exit(DrawLasso);
				END;

			LockHands(hbe);

		{ clear the bitmap because we use a maskRgn in copyBits }
			ClearHandle(hbe^^.selection.selHandle);

		{ put only selected bits into selBits bitmap }
			IF hbe^^.isColor THEN
				CopyBits(BitMapPtr(hbe^^.dupPix^)^, BitMapPtr(hbe^^.selection.selPix^)^, selBBox, selBBox,
							srcCopy, lassoRgn)
			ELSE
				CopyBits(hbe^^.dupBits, hbe^^.selection.selBits, selBBox, selBBox, srcCopy, lassoRgn);

			UnlockHands(hbe);

			IF hbe^^.isColor THEN
				WITH hbe^^, selection.maskPix^^ DO
					BEGIN
					rowBytes := ((((selection.selPix^^.bounds.right - selection.selPix^^.bounds.left) + 15))
									DIV 16) * 2;
					bounds := selection.selPix^^.bounds;
					theSize := UIntMul((bounds.bottom - bounds.top), rowBytes);
					rowBytes := rowBytes + $8000;
					words := ((bounds.right - bounds.left) + 15) DIV 16;
					tempBits.rowBytes := rowBytes - $8000;
					tempBits.bounds := bounds;
					mTBits.rowBytes := rowBytes - $8000;
					mTBits.bounds := bounds;
					END
			ELSE
				WITH hbe^^, selection.maskBits DO
					BEGIN
					bounds := selection.selBits.bounds;
					rowBytes := selection.selBits.rowBytes;
					theSize := GetHandleSize(selection.selHandle);
					words := ((bounds.right - bounds.left) + 15) DIV 16;
					tempBits.rowBytes := rowBytes;
					tempBits.bounds := bounds;
					mTBits.rowBytes := rowBytes;
					mTBits.bounds := bounds;
					END;

		{ make a bitmap for the mask }
			s := NewHandle(theSize);
			hbe^^.error := MemError;
			IF hbe^^.error <> NoErr THEN
				Exit(DrawLasso);

			hbe^^.selection.maskHandle := s;

			LockHands(hbe);

			tempBits.baseAddr := NewPtrClr(GetHandleSize(hbe^^.selection.maskHandle));
			hbe^^.error := MemError;
			IF hbe^^.error <> NoErr THEN SysBeep(20);

			mTBits.baseAddr := NewPtrClr(GetHandleSize(hbe^^.selection.maskHandle));
			hbe^^.error := MemError;
			IF hbe^^.error <> NoErr THEN SysBeep(20);

			WITH hbe^^ DO
				IF isColor THEN
					BEGIN
					saveTable := selection.selPix^^.pmTable;
					selection.selPix^^.pmTable := tempTable;
					CopyBits(BitMapPtr(selection.selPix^)^, tempBits, selection.selPix^^.bounds, tempBits.bounds, srcCopy, Nil);
					selection.selPix^^.pmTable := saveTable;
					END
				ELSE
					CopyBits(selection.selBits, tempBits, selection.selBits.bounds, tempBits.bounds, srcCopy, Nil);

		{ calculate the mask bits }
			CalcMask(tempBits.baseAddr, mTBits.baseAddr,  { srcPtr, dstPtr }
						tempBits.rowBytes, mTBits.rowBytes,  { srcRow, dstRow }
						tempBits.bounds.bottom - tempBits.bounds.top, words); { height, words }

			IF hbe^^.isColor THEN
				CopyBits(mTBits, BitMapPtr(hbe^^.selection.maskPix^)^, mTBits.bounds, hbe^^.selection.maskPix^^.bounds,
							srcCopy, Nil)
			ELSE
				CopyBits(mTBits, hbe^^.selection.maskBits, mTBits.bounds, hbe^^.selection.maskBits.bounds,
							srcCopy, Nil);

			DisposPtr(tempBits.baseAddr);
			DisposPtr(mTBits.baseAddr);

		{ make sure the mask is correct }
			BEDrawBegin(hbe);
			IF hbe^^.isColor THEN
				BEGIN
				SetPortPix(hbe^^.selection.maskPix);
				RectRgn(tempRgn, hbe^^.selection.maskPix^^.bounds);
				END
			ELSE
				BEGIN
				SetPortBits(hbe^^.selection.maskBits);
				RectRgn(tempRgn, hbe^^.selection.maskBits.bounds);
				END;

			DiffRgn(tempRgn, lassoRgn, tempRgn);
			EraseRgn(tempRgn);
			BEDrawEnd(hbe);

			DisposeRgn(tempRgn);

			s := NewHandle(GetHandleSize(hbe^^.selection.maskHandle));
			hbe^^.error := MemError;
			IF hbe^^.error <> NoErr THEN
				Exit(DrawLasso);

			hbe^^.selection.edgeHandle := s;

		{ setup a pixMap for the edges }
			IF hbe^^.isColor THEN
				WITH hbe^^.selection DO
					BEGIN
					edgePix^^.bounds    := maskPix^^.bounds;
					edgePix^^.rowBytes  := maskPix^^.rowBytes;
					END
			ELSE
				WITH hbe^^.selection DO
					BEGIN
					edgeBits.bounds   := maskBits.bounds;
					edgeBits.rowBytes := maskBits.rowBytes;
					END;

			UnlockHands(hbe);
			END; { selBBox not empty }

		IF hbe^^.isColor THEN
			DisposHandle(Handle(tempTable));
		END; { lassoRgn not empty }

	DisposeRgn(lassoRgn);

{ copy original bits back to screen to get rid of lasso }
	DoQuickDupToPort(hbe);

	BEInvalRect(hbe^^.beEditRect, hbe);

END; { DrawLasso }


PROCEDURE HiliteLasso(hbe: BEHandle);

VAR
	now, mIndex: INTEGER;
	r, srcRect: Rect;
	thePat: Pattern;

BEGIN
	GetMIndex(mIndex);

	SetupRects(r, srcRect, hbe);

	IF mIndex <> hbe^^.oldIndex THEN
		BEGIN
		hbe^^.oldIndex := mIndex;

		HLock(hbe^^.selection.edgeHandle);
		IF hbe^^.isColor THEN
			hbe^^.selection.edgePix^^.baseAddr := hbe^^.selection.edgeHandle^
		ELSE
			hbe^^.selection.edgeBits.baseAddr := hbe^^.selection.edgeHandle^;

	{ start with a clean bitmap }
		ClearHandle(hbe^^.selection.edgeHandle);

		LockHands(hbe);
		WITH hbe^^.selection DO
			IF hbe^^.isColor THEN
				CalcEdges(maskPix^^.baseAddr, edgePix^^.baseAddr, 0,
							maskPix^^.bounds.bottom - maskPix^^.bounds.top,
							(maskPix^^.rowBytes - $8000), True)
			ELSE
				CalcEdges(maskBits.baseAddr, edgeBits.baseAddr, 0,
							maskBits.bounds.bottom - maskBits.bounds.top,
							maskBits.rowBytes, True);
		UnlockHands(hbe);

		BEDrawBegin(hbe);
		GetSelPat(mIndex, thePat);
		PenPat(thePat);
		PenMode(patBic);
		EdgeToPort(hbe, srcBic);
		WITH hbe^^.selection DO
			IF hbe^^.isColor THEN
				BEGIN
				SetPortPix(edgePix);
				PaintRect(edgePix^^.bounds);
				END
			ELSE
				BEGIN
				SetPortBits(edgeBits);
				PaintRect(edgeBits.bounds);
				END;
		BEDrawEnd(hbe);

		BEDrawBegin(hbe);
		EdgeToPort(hbe, srcOr);
		BEDrawEnd(hbe);

		HUnlock(hbe^^.selection.edgeHandle);

		BEInvalRect(r, hbe);

		END;

END; { HiliteLasso }


PROCEDURE SetupUndo(hbe: BEHandle);
{ copy bit image to offscreen for undo }

VAR
	pm: PixMapHandle;
	bm: BitMap;

BEGIN
	IF hbe^^.undoFreeze = 0 THEN
		BEGIN
		IF BESelection(hbe) THEN
			BEGIN
			SetupUndoSelection(hbe);
			DoQuickDupToPort(hbe);
			END
		ELSE
			KillUndoSelection(hbe);

		HLock(hbe^^.undoHandle);
		HLock(Handle(hbe^^.editHandle));

	{ setup offscreen bitmap baseAddr }
		IF hbe^^.isColor THEN
			BEGIN
			hbe^^.undoPix^^.baseAddr := hbe^^.undoHandle^;
			BEPixmap(pm, hbe);
			HLock(Handle(cGrafPtr(hbe^^.beGrafPtr)^.portPixMap));
			END
		ELSE
			BEGIN
			hbe^^.undoBits.baseAddr := hbe^^.undoHandle^;
			BEBitmap(bm, hbe);
			END;

	{ copy from BitEdit record bits to offscreen buffer }
		BEDrawBegin(hbe);
		IF hbe^^.isColor THEN
			CopyBits(BitMapPtr(pm^)^, BitMapPtr(hbe^^.undoPix^)^, pm^^.bounds, hbe^^.undoPix^^.bounds,
						srcCopy, Nil)
		ELSE
			CopyBits(bm, hbe^^.undoBits, bm.bounds, hbe^^.undoBits.bounds, srcCopy, Nil);
		BEDrawEnd(hbe);

		IF hbe^^.isColor THEN
			HUnlock(Handle(cGrafPtr(hbe^^.beGrafPtr)^.portPixMap));
		HUnlock(hbe^^.undoHandle);
		HUnlock(Handle(hbe^^.editHandle));
		END;

END; { SetupUndo }


PROCEDURE BEUndoBegin(hbe: BEHandle);
{ Save an undo snapshot and retain it until the next call to BEUndoEnd }

BEGIN
	SetupUndo (hbe);
	hbe^^.undoFreeze := hbe^^.undoFreeze + 1;
END; { BEUndoBegin }


PROCEDURE BEUndoEnd(hbe: BEHandle);
{ Allow the next BitEdit routine to save a new undo state }

BEGIN
	IF hbe^^.undoFreeze > 0 THEN
		hbe^^.undoFreeze := hbe^^.undoFreeze - 1;
END; { BEUndoEnd }


PROCEDURE NoColorUndo(hbe: BEHandle);

VAR
	bm, tempBits: Bitmap;

BEGIN
{ setup undo bitmap baseAddr }
	HLock(hbe^^.undoHandle);
	hbe^^.undoBits.baseAddr := hbe^^.undoHandle^;

{ make a bitmap out of bitEdit record }
	HLock(Handle(hbe^^.editHandle));
	BEBitmap(bm, hbe);

{ set up a temporary bitmap }
	tempBits.baseAddr := NewPtr(GetHandleSize(Handle(hbe^^.undoHandle)));
	hbe^^.error := MemError;

	tempBits.bounds := hbe^^.undoBits.bounds;
	tempBits.rowBytes := hbe^^.undoBits.rowBytes;

	IF hbe^^.error = NoErr THEN
	{ swap the bits }
		BEGIN
			CopyBits(bm, tempBits, bm.bounds, tempBits.bounds, srcCopy, Nil);
			CopyBits(hbe^^.undoBits, bm, hbe^^.undoBits.bounds, bm.bounds, srcCopy, Nil);
			CopyBits(tempBits, hbe^^.undoBits, tempBits.bounds, hbe^^.undoBits.bounds, srcCopy, Nil);
		END
	ELSE
	{ copy from offscreen buffer to BitEdit record bits }
		CopyBits(hbe^^.undoBits, bm, hbe^^.undoBits.bounds, bm.bounds, srcCopy, Nil);

	HUnlock(hbe^^.undoHandle);
	HUnlock(Handle(hbe^^.editHandle));

	IF tempBits.baseAddr <> Nil THEN
		DisposPtr(tempBits.baseAddr);

END;


PROCEDURE BEUndo(hbe: BEHandle);
{ routine to undo the last change }

VAR
	bm, tempBits: PixMapHandle;
	p: Ptr;
	n: Handle;
	tempSel:	BESelectRec;

BEGIN
	IF BESelection (hbe) THEN
		BEGIN
	{ draw image without ants }
		LockHands(hbe);
		BEDrawBegin(hbe);
		DupToPort(hbe);
		BEDrawEnd(hbe);
		UnlockHands(hbe);
		END;

	IF NOT hbe^^.isColor THEN
		NoColorUndo(hbe)
	ELSE
		BEGIN
	{ setup undo bitmap baseAddr }
		HLock(hbe^^.undoHandle);
		hbe^^.undoPix^^.baseAddr := hbe^^.undoHandle^;

	{ make a bitmap out of BitEdit record }
		HLock(Handle(hbe^^.editHandle));
		BEPixmap(bm, hbe);
		HLock(Handle(cGrafPtr(hbe^^.beGrafPtr)^.portPixMap));

	{ set up a temporary bitmap }
		tempBits := NewPixMap;
		hbe^^.error := MemError;
		IF hbe^^.error <> NoErr THEN
			Exit(BEUndo);

		p := NewPtr(GetHandleSize(Handle(hbe^^.undoHandle)));
		hbe^^.error := MemError;
		IF hbe^^.error <> NoErr THEN
			BEGIN
			DisposPixMap (tempBits);
			Exit(BEUndo);
			END;
		DisposHandle(Handle(tempBits^^.pmTable));

		tempBits^^.baseAddr := p;
		tempBits^^.bounds := hbe^^.undoPix^^.bounds;
		tempBits^^.rowBytes := hbe^^.undoPix^^.rowBytes;
		tempBits^^.pixelSize := hbe^^.undoPix^^.pixelSize;
		tempBits^^.cmpSize := hbe^^.undoPix^^.cmpSize;
		tempBits^^.pmTable := hbe^^.undoPix^^.pmTable;

		IF hbe^^.error = NoErr THEN
		{ swap the bits }
			BEGIN
				CopyBits(BitMapPtr(bm^)^, BitMapPtr(tempBits^)^, bm^^.bounds, tempBits^^.bounds, srcCopy, Nil);
				CopyBits(BitMapPtr(hbe^^.undoPix^)^, BitMapPtr(bm^)^, hbe^^.undoPix^^.bounds, bm^^.bounds, srcCopy, Nil);
				CopyBits(BitMapPtr(tempBits^)^, BitMapPtr(hbe^^.undoPix^)^, tempBits^^.bounds, hbe^^.undoPix^^.bounds, srcCopy, Nil);
			END
			ELSE
		{ copy from offscreen buffer to BitEdit record bits }
				CopyBits(BitMapPtr(hbe^^.undoPix^)^, BitMapPtr(bm^)^, hbe^^.undoPix^^.bounds, bm^^.bounds, srcCopy, Nil);

		HUnlock(Handle(cGrafPtr(hbe^^.beGrafPtr)^.portPixMap));
		HUnlock(hbe^^.undoHandle);
		HUnlock(Handle(hbe^^.editHandle));

		IF tempBits^^.baseAddr <> Nil THEN
			BEGIN
			DisposPtr(tempBits^^.baseAddr);
			MyDisposPixMap(tempBits);
			END;
		END;

{ Restore selection }
	tempSel := hbe^^.undoSelection;
	hbe^^.undoSelection := hbe^^.selection;
	hbe^^.selection := tempSel;

{ Selection must be a lasso if a mask exists }
	hbe^^.marquee := (hbe^^.selection.maskHandle = Nil);

	IF BESelection (hbe) THEN
		BEGIN
		LockHands(hbe);
		BEDrawBegin(hbe);
		PortToDup(hbe);
		CopySelToPort(hbe);
		BEDrawEnd(hbe);
		UnlockHands(hbe);
		END;

	{ force all views to be redrawn }
	BEInvalRect(hbe^^.beEditRect, hbe);

END; { BEUndo }


PROCEDURE BEView(editRect: Rect; viewRect: Rect; bitsSize: Point; bitsDelta: Point; hbe: BEHandle);
{ Sets up a view of the bits }

BEGIN
	WITH hbe^^ DO BEGIN
		beEditRect := editRect;
		beViewRect := viewRect;
		fatbitsSize := bitsSize;
		fatbitsDelta := bitsDelta;
		StuffHex(@beFillPat, 'FFFFFFFFFFFFFFFF');
		StuffHex(@fatbitsPat, '0000000000000000');

		realRect := beEditRect;
		DoMyMapRect(hbe, realRect, realRect);
		END;
END; { BEView }
