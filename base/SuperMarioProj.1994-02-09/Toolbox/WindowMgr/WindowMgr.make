#
#	File:		WindowMgr.make
#
#	Contains:	Makefile for the Window Manager
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992-1993 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM3>	  9/9/93	pdw		Fixed up dependencies - slots.a and some others.
#	   <SM2>	 4/30/93	CSS		Fix dependecies to stop building WindowMgrExtensions.a just
#									because we change WindowMgr.a
#	   <SM2>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency.


WindowMgrObjs		=				"{ObjDir}WindowMgr.a.o"							∂
									"{ObjDir}WindowMgrExtensions.a.o"				∂
									"{ObjDir}LayerMgr.c.o"			


"{RsrcDir}WindowMgr.rsrc"			ƒƒ	"{WindowMgrDir}WindowMgr.r"						
	Rez {StdROpts} -a -o "{Targ}" "{WindowMgrDir}WindowMgr.r" || Exit 1

									
"{RsrcDir}WindowMgr.rsrc"			ƒƒ	"{ObjDir}StandardWDEF.a.o"
	Link {StdLOpts} {StdAlign} -rt WDEF=0 -o "{Targ}" "{ObjDir}StandardWDEF.a.o" || Exit 1


"{RsrcDir}WindowMgr.rsrc"			ƒƒ	"{ObjDir}RoundedWDEF.a.o"
	Link {StdLOpts} {StdAlign} -rt WDEF=1 -o "{Targ}" "{ObjDir}RoundedWDEF.a.o" || Exit 1


"{LibDir}WindowMgr.lib"				ƒ 	{WindowMgrObjs}
	Lib	{StdLibOpts} -o "{Targ}" {WindowMgrObjs}


"{ObjDir}LayerMgr.c.o"				ƒ	"{WindowMgrDir}LayerMgr.c"
	C {StdCOpts} -o "{Targ}" "{WindowMgrDir}LayerMgr.c"


"{ObjDir}RoundedWDEF.a.o"			ƒ	"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}ColorEqu.a"					∂
										"{WindowMgrDir}RoundedWDEF.a"
	Asm {StdAOpts} -o "{Targ}" "{WindowMgrDir}RoundedWDEF.a"


"{ObjDir}WindowMgr.a.o"				ƒ	"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}ColorEqu.a"					∂
										"{IntAIncludes}ScriptPriv.a"				∂
										"{AIncludes}Palettes.a"						∂
										"{IntAIncludes}PalettePriv.a"				∂
										"{AIncludes}Balloons.a"						∂
										"{IntAIncludes}BalloonsPriv.a"				∂
										"{IntAIncludes}ControlPriv.a"				∂
										"{AIncludes}Displays.a"						∂
										"{IntAIncludes}DisplaysPriv.a"				∂
										"{WindowMgrDir}WindowMgr.a"
	Asm {StdAOpts} -o "{Targ}" "{WindowMgrDir}WindowMgr.a"


"{ObjDir}WindowMgrExtensions.a.o"	ƒ	"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}LayerEqu.a"					∂
										"{AIncludes}Palettes.a"						∂
										"{IntAIncludes}PalettePriv.a"				∂
										"{IntAIncludes}LinkedPatchMacros.a"			∂
										"{WindowMgrDir}WindowMgrExtensions.a"
	Asm {StdAOpts} -o "{Targ}" "{WindowMgrDir}WindowMgrExtensions.a"


"{ObjDir}StandardWDEF.a.o"			ƒ	"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}ColorEqu.a"					∂
										"{WindowMgrDir}StandardWDEF.a"
	Asm {StdAOpts} -o "{Targ}" "{WindowMgrDir}StandardWDEF.a"
