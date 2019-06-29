#
#	File:		ColorQD.make
#
#	Contains:	Makefile for color quickdraw.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992-1993 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM2>	  9/9/93	pdw		Added slots.a to dependencies.
#	   <SM1>	 12/2/92			


PictUtilsDir	=	{ColorQDDir}PictUtilities:
QDPatchesDir	=	{ColorQDDir}Patches:

#include {PictUtilsDir}PictUtilities.Make


CQDObjs							=	"{ObjDir}CheckDevicesINIT.a.o"			∂
									"{ObjDir}CQD.a.o"						∂
									"{ObjDir}DeviceLoop.a.o"


"{RsrcDir}CQD.rsrc"				ƒ	"{ColorQDDir}CQD.r"
	Rez {StdROpts} -o "{Targ}" "{ColorQDDir}CQD.r" -i "{QDPatchesDir}"


"{LibDir}CQD.lib"				ƒ	{CQDObjs}
	Lib {StdLibOpts} -o "{Targ}" {CQDObjs}


"{ObjDir}CheckDevicesINIT.a.o"	ƒ	"{ColorQDDir}CheckDevicesINIT.a"
	Asm {StdAOpts} -o "{Targ}" "{ColorQDDir}CheckDevicesINIT.a"


"{ObjDir}DeviceLoop.a.o"		ƒ	 "{ObjDir}StandardEqu.d"				∂
									"{IntAIncludes}LinkedPatchMacros.a"		∂
									"{ColorQDDir}DeviceLoop.a"
	Asm {StdAOpts} -o "{Targ}" "{ColorQDDir}DeviceLoop.a"


"{ObjDir}CQD.a.o"				ƒ	"{IntAIncludes}ColorEqu.a"				∂
									"{AIncludes}Slots.a"					∂
									"{IntAIncludes}SlotMgrEqu.a"			∂
									"{AIncludes}QuickDraw.a"				∂
									"{AIncludes}SysEqu.a"					∂
									"{IntAIncludes}SysPrivateEqu.a"			∂
									"{AIncludes}syserr.a"					∂
									"{AIncludes}ToolUtils.a"				∂
									"{AIncludes}ROMEqu.a"					∂
									"{AIncludes}Video.a"					∂
									"{IntAIncludes}HardwarePrivateEqu.a"	∂
									"{AIncludes}HardwareEqu.a"				∂
									"{AIncludes}Palettes.a"					∂
									"{ColorQDDir}ANGLES.a"					∂
									"{ColorQDDir}Arith8Blt.a"				∂
									"{ColorQDDir}ARCS.a"					∂
									"{ColorQDDir}BITBLT.a"					∂
									"{ColorQDDir}BITMAPS.a"					∂
									"{ColorQDDir}cCrsrCore.a"				∂
									"{ColorQDDir}ColorMgr.a"				∂
									"{ColorQDDir}ColorAsm.a"				∂
									"{ColorQDDir}cqd.a"						∂
									"{ColorQDDir}DRAWARC.a"					∂
									"{ColorQDDir}DRAWLINE.a"				∂
									"{ColorQDDir}DRAWTEXT.a"				∂
									"{ColorQDDir}fasttraps.a"				∂
									"{ColorQDDir}gdevice.a"					∂
									"{ColorQDDir}GrafAsm.a"					∂
									"{ColorQDDir}GWorld.a"					∂
									"{ColorQDDir}LCURSOR.a"					∂
									"{ColorQDDir}LINES.a"					∂
									"{ColorQDDir}MaskAsm.a"					∂
									"{ColorQDDir}OVALS.a"					∂
									"{ColorQDDir}PACKRGN.a"					∂
									"{ColorQDDir}PaletteMgr.a"				∂
									"{QDPatchesDir}PaletteMgrPatch.a"		∂
									"{ColorQDDir}patterns.a"				∂
									"{ColorQDDir}PICTURES.a"				∂
									"{ColorQDDir}POLYGONS.a"				∂
									"{ColorQDDir}PUTLINE.a"					∂
									"{ColorQDDir}PUTOVAL.a"					∂
									"{ColorQDDir}PUTRGN.a"					∂
									"{ColorQDDir}QDHooks.a" 				∂
									"{ColorQDDir}QuickPolys.a" 				∂
									"{ColorQDDir}RECTS.a"					∂
									"{ColorQDDir}REGIONS.a"					∂
									"{ColorQDDir}RGNBLT.a"					∂
									"{ColorQDDir}RGNOP.a"					∂
									"{ColorQDDir}RRECTS.a"					∂
									"{ColorQDDir}ScaleBlt.a"				∂
									"{ColorQDDir}seekMask.a"				∂
									"{ColorQDDir}SEEKRGN.a"					∂
									"{ColorQDDir}SORTPOINTS.a"				∂
									"{ColorQDDir}STRETCH.A"					∂
									"{ColorQDDir}TEXT.a"					∂
									"{ColorQDDir}QDUtil.a"
	Asm {StdAOpts} -o "{Targ}" "{ColorQDDir}CQD.a" -i "{QDPatchesDir}"
