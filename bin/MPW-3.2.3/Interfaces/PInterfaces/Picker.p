{
Created: Friday, October 20, 1989 at 8:56 AM
	Picker.p
	Pascal Interface to the Macintosh Libraries

	Copyright Apple Computer, Inc.	1987-1989
	All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT Picker;
	INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingPicker}
{$SETC UsingPicker := 1}

{$I+}
{$SETC PickerIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingQuickdraw}
{$I $$Shell(PInterfaces)Quickdraw.p}
{$ENDC}
{$SETC UsingIncludes := PickerIncludes}

CONST

{

 }

MaxSmallFract = $0000FFFF;		{Maximum small fract value, as long}


TYPE

SmallFract = INTEGER;

HSVColor = RECORD
	hue: SmallFract;			{Fraction of circle, red at 0}
	saturation: SmallFract; 	{0-1, 0 for gray, 1 for pure color}
	value: SmallFract;			{0-1, 0 for black, 1 for max intensity}
	END;

{ For developmental simplicity in switching between the HLS and HSVmodels, HLS
 is reordered into HSL.  Thus both models start with hue and saturation values;
 value/lightness/brightness is last. }
HSLColor = RECORD
	hue: SmallFract;			{Fraction of circle, red at 0}
	saturation: SmallFract; 	{0-1, 0 for gray, 1 for pure color}
	lightness: SmallFract;		{0-1, 0 for black, 1 for white}
	END;

CMYColor = RECORD
	cyan: SmallFract;
	magenta: SmallFract;
	yellow: SmallFract;
	END;



FUNCTION Fix2SmallFract(f: Fixed): SmallFract;
	INLINE $3F3C,$0001,$A82E;
FUNCTION SmallFract2Fix(s: SmallFract): Fixed;
	INLINE $3F3C,$0002,$A82E;
PROCEDURE CMY2RGB(cColor: CMYColor;VAR rColor: RGBColor);
	INLINE $3F3C,$0003,$A82E;
PROCEDURE RGB2CMY(rColor: RGBColor;VAR cColor: CMYColor);
	INLINE $3F3C,$0004,$A82E;
PROCEDURE HSL2RGB(hColor: HSLColor;VAR rColor: RGBColor);
	INLINE $3F3C,$0005,$A82E;
PROCEDURE RGB2HSL(rColor: RGBColor;VAR hColor: HSLColor);
	INLINE $3F3C,$0006,$A82E;
PROCEDURE HSV2RGB(hColor: HSVColor;VAR rColor: RGBColor);
	INLINE $3F3C,$0007,$A82E;
PROCEDURE RGB2HSV(rColor: RGBColor;VAR hColor: HSVColor);
	INLINE $3F3C,$0008,$A82E;
FUNCTION GetColor(where: Point;prompt: Str255;inColor: RGBColor;VAR outColor: RGBColor): BOOLEAN;
	INLINE $3F3C,$0009,$A82E;

{$ENDC}    { UsingPicker }

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

