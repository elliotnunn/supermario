{
	File:		ColorGlue.p

	Copyright:	© 1987-1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 5/21/91	gbm		Stop using "empty" units like Memtypes...
		 <2>	12/28/89	dba		got rid of obsolete compile-time options (options are passed in
									by the Makefile)
	  <•1.1>	 7/24/89	CSD		FOR 6.0.4 Forcing submission to upgrade to the 32-Bit QuickDraw
									Color Picker.
}

UNIT ColorGlue;

INTERFACE

USES
	Types, Memory, Quickdraw,
	ColorConvert, ColorPicker;

FUNCTION  glueFix2SmallFract(f: Fixed): SmallFract;
FUNCTION  glueSmallFract2Fix(s: SmallFract): Fixed;
PROCEDURE glueCMY2RGB(cColor: CMYColor; VAR rColor: RGBColor);
PROCEDURE glueRGB2CMY(rColor: RGBColor; VAR cColor: CMYColor);
PROCEDURE glueHSL2RGB(hColor: HSLColor; VAR rColor: RGBColor);
PROCEDURE glueRGB2HSL(rColor: RGBColor; VAR hColor: HSLColor);
PROCEDURE glueHSV2RGB(hColor: HSVColor; VAR rColor: RGBColor);
PROCEDURE glueRGB2HSV(rColor: RGBColor; VAR hColor: HSVColor);
FUNCTION  glueGetColor(where: Point; prompt: Str255; inColor: RGBColor;
					VAR outColor: RGBColor): BOOLEAN;

IMPLEMENTATION

PROCEDURE Picker; EXTERNAL;		{First byte of the package}

{---------------------------------------------------------------------------------
 Glue routines for all the routines in the interface.  The dispatcher locks
 down the package, but has no chance to unlock it afterwards.  We get
 around this by dispatching to these routines, which just call the real
 ones, then unlock the package.  Internally, picker calls its own routines
 directly.
}

PROCEDURE glueUnlockPack;
BEGIN
	HUnlock(RecoverHandle(@Picker));
END;

FUNCTION  glueFix2SmallFract(f: Fixed): SmallFract;
BEGIN
	glueFix2SmallFract := Fix2SmallFract(f);
	glueUnlockPack;
END;

FUNCTION  glueSmallFract2Fix(s: SmallFract): Fixed;
BEGIN
	glueSmallFract2Fix := SmallFract2Fix(s);
	glueUnlockPack;
END;

PROCEDURE glueCMY2RGB(cColor: CMYColor; VAR rColor: RGBColor);
BEGIN
	CMY2RGB(cColor, rColor);
	glueUnlockPack;
END;

PROCEDURE glueRGB2CMY(rColor: RGBColor; VAR cColor: CMYColor);
BEGIN
	RGB2CMY(rColor, cColor);
	glueUnlockPack;
END;

PROCEDURE glueHSL2RGB(hColor: HSLColor; VAR rColor: RGBColor);
BEGIN
	HSL2RGB(hColor, rColor);
	glueUnlockPack;
END;

PROCEDURE glueRGB2HSL(rColor: RGBColor; VAR hColor: HSLColor);
BEGIN
	RGB2HSL(rColor, hColor);
	glueUnlockPack;
END;

PROCEDURE glueHSV2RGB(hColor: HSVColor; VAR rColor: RGBColor);
BEGIN
	HSV2RGB(hColor, rColor);
	glueUnlockPack;
END;

PROCEDURE glueRGB2HSV(rColor: RGBColor; VAR hColor: HSVColor);
BEGIN
	RGB2HSV(rColor, hColor);
	glueUnlockPack;
END;

FUNCTION glueGetColor(where: Point; prompt: Str255; inColor: RGBColor;
					VAR outColor: RGBColor): BOOLEAN;
BEGIN
	glueGetColor := GetColor(where, prompt, inColor, outColor);
	glueUnlockPack;
END;

END.
