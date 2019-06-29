{
	File:		ColorConvert.p

	Copyright:	© 1987-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 5/21/91	gbm		Stop using the "empty" units (MemTypes, etc...)
		 <2>	12/28/89	dba		got rid of obsolete compile-time options (options are controlled
									by the Makefile)
	   <1.1>	 7/24/89	CSD		FOR 6.0.4: Forcing submission to upgrade to the 32-Bit QuickDraw
									Color Picker.

	To Do:
}

UNIT ColorConvert;

INTERFACE

USES
	Types, QuickDraw, ToolUtils, FixMath;

CONST
	MaxSmallFract	= $0000FFFF;	{Maximum small fract value, as LONGINT}

TYPE
	{A SmallFract value is just the fractional part of a Fixed number,
		which is the low order word.  SmallFracts are used to save room,
		and to be compatible with Quickdraw's RGBColor.  They can be
		assigned directly to and from INTEGERs.}

	SmallFract		= INTEGER;		{Unsigned fraction between 0 and 1}

	{For developmental simplicity in switching between the HLS and HSV
		models, HLS is reordered into HSL.  Thus both models start with
		hue and saturation values; value/lightness/brightness is last.}

	HSVColor		= RECORD
		hue:		SmallFract;		{Fraction of circle, red at 0}
		saturation:	SmallFract;		{0-1, 0 for gray, 1 for pure color}
		value:		SmallFract;		{0-1, 0 for black, 1 for max intensity}
		END;
	HSLColor		= RECORD
		hue:		SmallFract;		{Fraction of circle, red at 0}
		saturation:	SmallFract;		{0-1, 0 for gray, 1 for pure color}
		lightness:	SmallFract;		{0-1, 0 for black, 1 for white}
		END;
	CMYColor		= RECORD
		cyan:		SmallFract;
		magenta:	SmallFract;
		yellow:		SmallFract;
		END;

FUNCTION  Max(a, b: Fixed): Fixed;
FUNCTION  Min(a, b: Fixed): Fixed;
FUNCTION  Fix2SmallFract(f: Fixed): SmallFract;
FUNCTION  SmallFract2Fix(s: SmallFract): Fixed;
PROCEDURE CMY2RGB(cColor: CMYColor; VAR rColor: RGBColor);
PROCEDURE RGB2CMY(rColor: RGBColor; VAR cColor: CMYColor);
PROCEDURE HSL2RGB(hColor: HSLColor; VAR rColor: RGBColor);
PROCEDURE RGB2HSL(rColor: RGBColor; VAR hColor: HSLColor);
PROCEDURE HSV2RGB(hColor: HSVColor; VAR rColor: RGBColor);
PROCEDURE RGB2HSV(rColor: RGBColor; VAR hColor: HSVColor);

IMPLEMENTATION

FUNCTION Max(a, b: Fixed): Fixed;
BEGIN
	IF a > b
		THEN Max := a
		ELSE Max := b;
END;

FUNCTION Min(a, b: Fixed): Fixed;
BEGIN
	IF a < b
		THEN Min := a
		ELSE Min := b;
END;

FUNCTION Fix2SmallFract(f: Fixed): SmallFract;
BEGIN
	IF f > MaxSmallFract
		THEN Fix2SmallFract := LoWrd(MaxSmallFract)
		ELSE Fix2SmallFract := f;
END;

FUNCTION SmallFract2Fix(s: SmallFract): Fixed;
BEGIN
	SmallFract2Fix := BAnd(s, MaxSmallFract);
END;

{---------------------------------------------------------------------------------
 CMY Model (cyan, magenta, yellow)
}

FUNCTION Complement(inColor: INTEGER): INTEGER;
BEGIN
	Complement := Fix2SmallFract(MaxSmallFract - SmallFract2Fix(inColor));
END;

PROCEDURE CMY2RGB(cColor: CMYColor; VAR rColor: RGBColor);
BEGIN
	WITH cColor, rColor DO
		BEGIN
		red := Complement(cyan);
		green := Complement(magenta);
		blue := Complement(yellow);
		END;
END;

PROCEDURE RGB2CMY(rColor: RGBColor; VAR cColor: CMYColor);
BEGIN
	WITH cColor, rColor DO
		BEGIN
		cyan := Complement(red);
		magenta := Complement(green);
		yellow := Complement(blue);
		END;
END;

{---------------------------------------------------------------------------------
 HSL Model (hue, saturation, lightness)
}

PROCEDURE HSL2RGB(hColor: HSLColor; VAR rColor: RGBColor);
VAR
	aThird:		Fixed;
	h:			Fixed;
	l, s, ls:	Fixed;
	m1, m2:		Fixed;
	
	FUNCTION Value(h: Fixed): SmallFract;
	VAR
		temp:	Fixed;
	BEGIN
		IF h >= $00010000 THEN
			h := h - $00010000
		ELSE IF h < 0 THEN
			h := h + $00010000;

		h := h * 6;
		CASE HiWrd(h) OF
			0:		temp := m1 + FixMul(m2 - m1, h);
			1,2:	temp := m2;
			3:		temp := m1 + FixMul(m2 - m1, $00040000 - h);
			4,5:	temp := m1;
			END;
		Value := Fix2SmallFract(temp);
	END;
		
BEGIN
	WITH hColor, rColor DO
		BEGIN
		l := SmallFract2Fix(lightness);
		s := SmallFract2Fix(saturation);
		ls := FixMul(l, s);

		IF l <= $00008000	{0.5}
			THEN m2 := l + ls
			ELSE m2 := l + s - ls;
		m1 := BSL(l, 1) - m2;

		IF saturation = 0 THEN	{achromatic case; no hue}
			BEGIN
			red := lightness;
			green := lightness;
			blue := lightness;
			END
		ELSE
			BEGIN	{chormatic case: there is hue}
			aThird := FixRatio(1, 3);
			h := SmallFract2Fix(hue);
			red := Value(h + aThird);
			green := Value(h);
			blue := Value(h - aThird);
			END;
		END;
END;

PROCEDURE RGB2HSL(rColor: RGBColor; VAR hColor: HSLColor);
VAR
	cr, cg, cb:	Fixed;
	r, g, b:	Fixed;
	h:			Fixed;
	maxRGB,
	minRGB:		Fixed;
	mSum,
	mDiff:		Fixed;
BEGIN
	WITH hColor, rColor DO
		BEGIN
		r := SmallFract2Fix(red);
		g := SmallFract2Fix(green);
		b := SmallFract2Fix(blue);

		{Determine the lightness.}
		maxRGB := Max(r, Max(g, b));
		minRGB := Min(r, Min(g, b));
		mSum := maxRGB + minRGB;
		mDiff := maxRGB - minRGB;

		{Average max and min to get lightness.}
		lightness := Fix2SmallFract(BSR(mSum, 1));

		{Determine the saturation.}
		IF maxRGB = minRGB THEN
			{achromatic case (hue is undefined)}
			saturation := 0
		ELSE
			BEGIN	{chromatic case}
			IF SmallFract2Fix(lightness) > $00008000 THEN	{0.5}
				mSum := $00020000 - mSum;
			saturation := Fix2SmallFract(FixDiv(mDiff, mSum));

			{Determine the hue.}
			cr := FixDiv(maxRGB - r, mDiff);
			cg := FixDiv(maxRGB - g, mDiff);
			cb := FixDiv(maxRGB - b, mDiff);

			IF r = maxRGB THEN
				{between yellow and magenta}
				h := cb - cg
			ELSE IF g = maxRGB THEN
				{between cyan and yellow}
				h := $00020000 + cr - cb
			ELSE
				{between magenta and cyan}
				h := $00040000 + cg - cr;

			h := h DIV 6;
			IF h < 0 THEN
				h := h + $00010000;
			hue := Fix2SmallFract(h);
			END;
		END;
END;

{---------------------------------------------------------------------------------
 HSV Model (hue, saturation, value)
}

PROCEDURE HSV2RGB(hColor: HSVColor; VAR rColor: RGBColor);
VAR
	kn, m:		INTEGER;
	segment:	INTEGER;
	subSeg:		Fixed;
	v, vs, vsf:	Fixed;
BEGIN
	WITH hColor, rColor DO
		IF saturation = 0 THEN
			BEGIN	{achromatic case: no hue}
			red := value;
			green := value;
			blue := value;
			END
		ELSE 
			BEGIN	{chromatic case}
			subSeg := SmallFract2Fix(hue) * 6;
			{Get the integer part in segment, and fraction in subSeg.}
			segment := HiWrd(subSeg);
			subSeg := BAnd(subSeg, $0000FFFF);

			{Compute out Smith's K/N and M as integers.}
			v := SmallFract2Fix(value);
			vs := FixMul(v, SmallFract2Fix(saturation));
			vsf := FixMul(vs, subSeg);
			IF ODD(segment)
				THEN kn := Fix2SmallFract(v - vsf)
				ELSE kn := Fix2SmallFract(v - vs + vsf);
			m := Fix2SmallFract(v - vs);

			{We have the RGB values, now figure out which is which.}
			CASE segment OF
				0,5:	red := value;
				1,4:	red := kn;
				2,3:	red := m;
				END;
			CASE segment OF
				1,2:	green := value;
				0,3:	green := kn;
				4,5:	green := m;
				END;
			CASE segment OF
				3,4:	blue := value;
				2,5:	blue := kn;
				0,1:	blue := m;
				END;
			END;	{end of chromatic case}
END;

PROCEDURE RGB2HSV(rColor: RGBColor; VAR hColor: HSVColor);
VAR
	cr, cg, cb:	Fixed;
	r, g, b:	Fixed;
	h:			Fixed;
	temp:		Fixed;
	v, vt:		Fixed;
BEGIN
	WITH hColor, rColor DO
		BEGIN
		r := SmallFract2Fix(red);
		g := SmallFract2Fix(green);
		b := SmallFract2Fix(blue);

		v := Max(r, Max(g, b));
		temp := Min(r, Min(g, b));
		vt := v - temp;
		value := Fix2SmallFract(v);

		IF v = 0
			THEN saturation := 0
			ELSE saturation := Fix2SmallFract(FixDiv(vt, v));

		{Determine the hue.}
		IF saturation = 0 THEN
			hue := 0
		ELSE
			BEGIN
			cr := FixDiv(v - r, vt);
			cg := FixDiv(v - g, vt);
			cb := FixDiv(v - b, vt);

			IF r = v THEN
				{between yellow and magenta}
				h := cb - cg
			ELSE IF g = v THEN
				{between cyan and yellow}
				h := $00020000 + cr - cb
			ELSE
				{between magenta and cyan}
				h := $00040000 + cg - cr;

			h := h DIV 6;
			IF h < 0 THEN
				h := h + $00010000;
			hue := Fix2SmallFract(h);
			END;
		END;
END;

END.

