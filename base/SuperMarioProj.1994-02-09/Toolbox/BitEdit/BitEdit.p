{__________________________________________________________________________________________________
	File:		BitEdit.p

	Contains:	xxx put contents here xxx

	Written by:	Craig Carper, Steve Horowitz

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<1+>	 1/22/90	JRM		update to Craig's "last" revision
	   <1.2>	10/16/89	CCH		Incorporating changes from Craig Carper.
	   <1.1>	10/11/89	JSM		Fixed BEDraw so that it doesn't indiscriminately call GetGDevice
									and SetGDevice without checking for color.
	   <1.0>	  8/3/89	CCH		Adding to EASE for Big Bang build as PACK ID=1.

	To Do:
__________________________________________________________________________________________________}

{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
  UNIT BitEdit;
  INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingBitEdit}
{$SETC UsingBitEdit := 1}

USES Types, Memory, Quickdraw, Events, OsUtils, Scrap, Windows, ToolUtils;

CONST
{ Bit flags for BESelectFlags }

fSelectKindMask = $3;	{ 2-bit field determines kind of selection: }
fSelectRect = $0;	{ 	Rectangular (marquee) Selection }
fSelectLasso = $1;	{ 	Free-form (lasso) Selection }
fSelectRectLasso = $2;	{ 	Rectangular selection the collapses into lasso selection }
fSelectRectShrink = $3;	{ 	Collapse to smallest enclosing rectangle }

{ BESelectFlags extended bit values }

fSquareMarquee = $4;	{ Drag out a square marquee }
fDragOnAxis = $8;	{ Move selection vertically or horizontally only }
fDragCopy = $10;	{ Drag a copy instead of moving the selection }

{ Bit flags for BEDrawFlags }

fShapeLineMask = $3;	{ 2-bit field specifying line angle constraints: }
fLineFree = $0;	{ 	Unconstrained lines }
fLineOnAxis = $1;	{ 	Vertical or horizontal lines only }
fLineOnDiagonal = $2;	{ 	Vertical, horizontal or 45 degree lines only }

{ Bit flags for shapes }

fShapeFillMask = $3;	{ 2-bit field specifying border and fill options: }
fShapeFrame = $0;	{ 	Frame only }
fShapeFill = $2;	{ 	Frame and filled }
fShapeFillOnly = $3;	{ 	Filled only }

{ BEDrawFlags extended bit values for shape routines }

fShapeSymmetric = $4;	{ Set for square rectangles or circular ovals }
fShapeUnbounded = $8;	{ Set to allow dragging out shape beyond bounds of image }

TYPE
BESelectFlags = integer;
BEDrawFlags = integer;
BESelectPtr = ^BESelectRec;
BESelectRec = RECORD	{ Selection State }
	selRect: Rect;	{ Rectangle for selection }
	wasErased: Boolean;	{ was the marquee rect erased already? }
	selHandle: Handle;	{ handle for selection bits }
	selPix: PixMapHandle;	{ pixMap for selection bits }
	selBits: BitMap;	{ bitMap for selection bits }
	maskHandle: Handle;	{ handle for mask bits }
	maskPix: PixMapHandle;	{ pixMap for mask bits }
	maskBits: BitMap;	{ bitMap for mask bits }
	edgeHandle: Handle;	{ handle for edge bits }
	edgePix: PixMapHandle;	{ pixMap for edge bits }
	edgeBits: BitMap;	{ bitMap for edge bits }
	END;

BEPtr = ^BERec;
BEHandle = ^BEPtr;
BERec = RECORD
	editHandle: Handle;	{ Handle to the bits }
	editPix: PixMapHandle;	{ PixMap with the BE bits }
	editOffset: LongInt;	{ Offset to bitmap in handle }
	beRowBytes: integer;	{ rowBytes of bitmap }
	beBounds: Rect;	{ bounds rect of bitmap }
	bePixelSize: integer;	{ bits per pixel }
	beColorTable: CTabHandle;	{ Handle to PixMap's color table, Nil if bitmap }
	bePixPat: PixPatHandle;	{ color pattern for shape drawing }
	bePixColor: RGBColor;	{ color to draw the pixels in }
	beFillPat: Pattern;	{ fill pattern for shapes }
	beEditRect: Rect;	{ rectangle for editing }
	beViewRect: Rect;	{ rectangle in which we are displayed }
	fatbitsSize: Point;	{ dimensions of the displayed fatBits pixels }
	fatbitsDelta: Point;	{ spacing between fatBits pixels }
	fatbitsPat: Pattern;	{ pattern for the pixels }
	sibling: BEHandle;	{ link to this relative for updates }
	beGrafPtr: GrafPtr;	{ GrafPort that owns me }
	savePen: PenState;	{ saved PenState over DrawBegin/End }
	savePixMap: PixMapHandle;	{ saved PixMap over DrawBegin/End }
	saveBitMap: BitMap;	{ saved bitMap over DrawBegin/End }
	saveClip: RgnHandle;	{ saved clipRgn over DrawBegin/End }
	saveVis: RgnHandle;	{ saved visRgn over DrawBegin/End }
	error: OSErr;	{ error result }
	undoFreeze: integer;	{ non-zero if undo snapshot should not be altered }
	realRect: Rect;	{ the actual exploded rectangle }
	beEraserSize: Point;	{ Size of the eraser }
	bePenSize: Point;	{ size of pen for shape drawing }
	beDrawParm: LongInt;	{ State information - Internal use }
	selection: BESelectRec;	{ Selection State }
	undoSelection: BESelectRec;	{ Selection state for undo }
	offHandle: Handle;	{ handle to large offscreen bits }
	offPix: PixMapHandle;	{ pixMap for large offscreen cGrafPort }
	offBits: BitMap;	{ bitMap for large offscreen cGrafPort }
	dupHandle: Handle;	{ handle for shape drawing }
	dupPix: PixMapHandle;	{ pixMap for shape drawing }
	dupBits: BitMap;	{ bitMap for shape drawing }
	undoHandle: Handle;	{ handle for undo bits }
	undoPix: PixMapHandle;	{ pixMap for undo bits }
	undoBits: BitMap;	{ bitMap for undo bits }
	oldIndex: integer;	{ index save for marquee }
	beGDevice: GDHandle;	{ handle to the offscreen gDevice }
	active: Boolean;	{ true if activated }
	drawEnabled: Boolean;	{ true if drawing enabled }
	isColor: Boolean;	{ is this a color BitEdit record? }
	marquee: Boolean;	{ true = marquee, false = lasso }
	refCon: LongInt;	{ For use by client }
	END;


{ Creation, activation and destruction routines }

FUNCTION BENew(editHandle: Handle; colorTable: CTabHandle; offset: LongInt; rowBytes: integer; pixelSize: integer; bounds: Rect): BEHandle;
PROCEDURE BEView(editRect: Rect; viewRect: Rect; bitsSize: Point; bitsDelta: Point; hbe: BEHandle);
PROCEDURE BEActivate(activate: Boolean; hbe: BEHandle);
PROCEDURE BEDispose(hbe: BEHandle);

{ Screen drawing and update routines }

PROCEDURE BEInvalRect(r: Rect; hbe: BEHandle);
PROCEDURE BEUpdateRect(r: Rect; hbe: BEHandle);
PROCEDURE BEDraw(r: Rect; hbe: BEHandle);
PROCEDURE BELink(hbe1: BEHandle; hbe2: BEHandle);

{ Scrap routines }

PROCEDURE BECopy(hbe: BEHandle);
PROCEDURE BECut(hbe: BEHandle);
PROCEDURE BEPaste(hbe: BEHandle);

{ Drawing routines }

PROCEDURE BEClick(pt: Point; hbe: BEHandle);
PROCEDURE BEErase(pt: Point; hbe: BEHandle);
PROCEDURE BEPaintBucket(startPt: Point; hbe: BEHandle);
PROCEDURE BELine(startPt: Point; flags: BEDrawFlags; hbe: BEHandle);
PROCEDURE BEOval(startPt: Point; flags: BEDrawFlags; hbe: BEHandle);
PROCEDURE BERect(startPt: Point; flags: BEDrawFlags; hbe: BEHandle);
PROCEDURE BERoundRect(startPt: Point; flags: BEDrawFlags; ovalWidth: integer; ovalHeight: integer; hbe: BEHandle);

{ Transformation Routines }

PROCEDURE BEFlip(vertical: Boolean; hbe: BEHandle);
PROCEDURE BERotate(hbe: BEHandle);

{ Inspection Routines }

FUNCTION BEBit(pt: Point; hbe: BEHandle): Boolean;
PROCEDURE BEColor(pt: Point; VAR rgb: RGBColor; hbe: BEHandle);

{ Selection Routines }

PROCEDURE BESelect(startPt: Point; flags: BESelectFlags; hbe: BEHandle);
PROCEDURE BESetSelect(r: Rect; hbe: BEHandle);
PROCEDURE BEClear(hbe: BEHandle);
PROCEDURE BEDuplicate(deltaPt: Point; hbe: BEHandle);
FUNCTION BENudge(deltaPt: Point; hbe: BEHandle): Boolean;
FUNCTION BEInSelection(pt: Point; hbe: BEHandle): Boolean;
FUNCTION BESelection(hbe: BEHandle): Boolean;
PROCEDURE BEKillSelection(hbe: BEHandle);
PROCEDURE BEPutSelection(hbe: BEHandle);
PROCEDURE BEIdle(hbe: BEHandle);

{ Undo Support }

PROCEDURE BEUndo(hbe: BEHandle);
PROCEDURE BEUndoBegin(hbe: BEHandle);
PROCEDURE BEUndoEnd(hbe: BEHandle);

{ Utility Routines }

FUNCTION BEGetView(pt: Point; VAR hbe: BEHandle): Boolean;
PROCEDURE BEDoDraw(draw: Boolean; hbe: BEHandle);
PROCEDURE BEPtUnXForm(VAR pt: Point; hbe: BEHandle);
PROCEDURE BEPtXForm(VAR pt: Point; hbe: BEHandle);

{ Low-level routines for do-it-yourselfers }

PROCEDURE BEBitMap(VAR bm: BitMap; hbe: BEHandle);
PROCEDURE BEPixMap(VAR bm: PixMapHandle; hbe: BEHandle);
PROCEDURE BEBounds(VAR minimalRect: Rect; hbe: BEHandle);
PROCEDURE BEDrawBegin(hbe: BEHandle);
PROCEDURE BEDrawEnd(hbe: BEHandle);
PROCEDURE BEObject(startPt: Point; drawProc: ProcPtr; flags: BEDrawFlags; hbe: BEHandle);

{$ENDC}

{ $IFC NOT UsingIncludes}
  IMPLEMENTATION
  {$I BitEdit.i}
  END.
{ $ENDC}