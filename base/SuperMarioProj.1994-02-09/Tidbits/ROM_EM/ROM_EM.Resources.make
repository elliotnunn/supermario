#
#	File:		Resources.make
#
#	Contains:	Makefile to build the various resources.
#
#	Written by:	Kurt Clark
#
#	Copyright:	© 1992-1993 by Apple Computer, Inc.  All rights reserved.
#
#	Change History (most recent first):
#
#	   <SM3>	 3/31/93	kc		Remove build commands for RomResources.rsrc. It is now included
#									by the romlayout files.
#	   <SM2>	 2/22/93	CSS		Correct spelling of Resource (from Resoruce).


"{RsrcDir}ROMFonts.rsrc"		ƒ	"{ResourceDir}ROMFonts.r"
	Rez {StdROpts} "{ResourceDir}ROMFonts.r" -o "{Targ}"


"{RsrcDir}ROMApple2Fonts.rsrc"	ƒ	"{ResourceDir}ROMApple2Fonts.r"
	Rez {StdROpts} "{ResourceDir}ROMApple2Fonts.r" -o "{Targ}"


"{RsrcDir}MiscROMRsrcs.rsrc"	ƒ	"{ResourceDir}MiscROMRsrcs.r"
	Rez {StdROpts} "{ResourceDir}MiscROMRsrcs.r" -o "{Targ}"


"{RsrcDir}ROM_EM.rsrc"			ƒ	"{ResourceDir}ROM_EM.r"
	Rez {StdROpts} {ResourceDir}ROM_EM.r -o {Targ}


