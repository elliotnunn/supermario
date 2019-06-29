#
#	File:		MenuMgr.make
#
#	Contains:	Makefile for the Menu Manager.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc.  All rights reserved.
#
#	Change History (most recent first):
#
#	   <SM3>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency.
#	   <SM2>	11/30/92	SWC		Changed PackMacs.a->Packages.a.


MenuMgrObjs			=				"{ObjDir}MenuMgr.a.o"							∂
									"{ObjDir}MenuDispatch.a.o"						∂
									"{ObjDir}MenuMgr.c.o"							∂
									"{ObjDir}SaveRestoreBits.a.o"					∂
									"{ObjDir}MenuMgrExtensions.a.o"					∂
									"{ObjDir}SystemMenusPatch.a.o"
			

"{RsrcDir}MenuMgr.rsrc"			ƒƒ	"{ObjDir}StandardMBDF.a.o"
	Link {StdLOpts} {StdAlign} -rt MBDF=0 -o "{Targ}" "{ObjDir}StandardMBDF.a.o" || Exit 1


"{RsrcDir}MenuMgr.rsrc"			ƒƒ	"{ObjDir}StandardMDEF.a.o"
	Link {StdLOpts} {StdAlign} -rt MDEF=0 -o "{Targ}" "{ObjDir}StandardMDEF.a.o" || Exit 1


"{LibDir}MenuMgr.lib"			ƒ	{MenuMgrObjs}
	Lib {StdLibOpts} -o "{Targ}" {MenuMgrObjs}


"{ObjDir}StandardMDEF.a.o"		ƒ	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}ColorEqu.a"						∂
									"{MenuMgrDir}StandardMDEF.a"
	Asm {StdAOpts} -o "{Targ}" "{MenuMgrDir}StandardMDEF.a"


"{ObjDir}StandardMBDF.a.o"		ƒ	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}ColorEqu.a"						∂
									"{MenuMgrDir}StandardMBDF.a"
	Asm {StdAOpts} -o "{Targ}" "{MenuMgrDir}StandardMBDF.a"


"{ObjDir}MenuMgr.a.o"			ƒ	"{MenuMgrDir}IncludeMenuMgr.a"					∂
									"{MenuMgrDir}MenuMgr.a"							∂
									"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}ColorEqu.a"						∂
									"{AIncludes}Packages.a"							∂
									"{MenuMgrDir}MenuDispatch.a"
	Asm {StdAOpts} -o "{Targ}" "{MenuMgrDir}IncludeMenuMgr.a"


"{ObjDir}MenuDispatch.a.o"		ƒ 	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}LinkedPatchMacros.a"				∂
									"{AIncludes}Menus.a"							∂
									"{IntAIncludes}MenuMgrPriv.a"					∂
									"{MenuMgrDir}MenuDispatch.a"
	Asm {StdAOpts} -o "{Targ}" "{MenuMgrDir}MenuDispatch.a"


"{ObjDir}MenuMgr.c.o"			ƒ	"{CIncludes}SysEqu.h"							∂
									"{CIncludes}Resources.h"						∂
									"{CIncludes}Types.h"							∂
									"{CIncludes}Files.h"							∂
									"{CIncludes}OSUtils.h"							∂
									"{CIncludes}SegLoad.h"							∂
									"{CIncludes}Memory.h"							∂
									"{CIncludes}Menus.h"							∂
									"{CIncludes}Quickdraw.h"						∂
									"{CIncludes}Script.h"							∂
									"{IntCIncludes}IntlUtilsPriv.h"					∂
									"{IntCIncludes}ScriptPriv.h"					∂
									"{CIncludes}Events.h"							∂
									"{CIncludes}Packages.h"							∂
									"{CIncludes}StandardFile.h"						∂
									"{CIncludes}Dialogs.h"							∂
									"{CIncludes}Windows.h"							∂
									"{CIncludes}Controls.h"							∂
									"{CIncludes}TextEdit.h"							∂
									"{MenuMgrDir}MenuMgr.c"
	C {StdCOpts} -o "{Targ}" "{MenuMgrDir}MenuMgr.c"


"{ObjDir}SaveRestoreBits.a.o"	ƒ 	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}LinkedPatchMacros.a"				∂
									"{AIncludes}Menus.a"							∂
									"{IntAIncludes}MenuMgrPriv.a"					∂
									"{IntAIncludes}LayerEqu.a"						∂
									"{IntAIncludes}InternalMacros.a"				∂
									"{MenuMgrDir}SaveRestoreBits.a"
	Asm {StdAOpts} -o "{Targ}" "{MenuMgrDir}SaveRestoreBits.a"


"{ObjDir}SystemMenusPatch.a.o"	ƒ 	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}LinkedPatchMacros.a"				∂
									"{AIncludes}Menus.a"							∂
									"{IntAIncludes}MenuMgrPriv.a"					∂
									"{MenuMgrDir}SystemMenusPatch.a"
	Asm {StdAOpts} -o "{Targ}" "{MenuMgrDir}SystemMenusPatch.a"


"{ObjDir}MenuMgrExtensions.a.o"	ƒ 	"{MenuMgrDir}MenuMgrExtensions.a"
	Asm {StdAOpts} -o "{Targ}" "{MenuMgrDir}MenuMgrExtensions.a"
