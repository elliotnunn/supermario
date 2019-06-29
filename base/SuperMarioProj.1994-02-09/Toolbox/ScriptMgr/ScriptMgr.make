#
#	File:		ScriptMgr.make
#
#	Contains:	Makefile to build the script manager library
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc.  All rights reserved.
#
#	Change History (most recent first):
#
#	   <SM2>	11/30/92	SWC		Changed PackMacs.a->Packages.a.


# These are postion depenedant

ScriptMgrObjects =					"{ObjDir}ScriptMgrHeader.a.o"					∂
									"{ObjDir}RomanUtil.a.o"							∂
									"{ObjDir}ScriptMgrMisc.a.o"						∂
									"{ObjDir}ScriptMgrUtilDate.a.o"					∂
									"{ObjDir}ScriptMgrUtilText.a.o"					∂
									"{ObjDir}ScriptMgrUtilNum.a.o"					∂
									"{ObjDir}ScriptMgrInit.a.o"						∂
									"{ObjDir}RomanNewJust.a.o"						∂
									"{ObjDir}ScriptMgrFindWord.c.o"					∂
									"{ObjDir}ScriptMgrTruncRepl.a.o"				∂
									"{ObjDir}ScriptMgrKbdMenu.a.o"					∂
									"{ObjDir}ScriptMgrSysMenuPatch.a.o"				∂
									"{ObjDir}DblByteCompat.a.o"						∂
									"{ObjDir}ScriptMgrExtensions.a.o"				∂
									"{ObjDir}ScriptMgrKeyGetSet.a.o"				∂
									"{ObjDir}ScriptMgrDispatch.a.o"					∂


"{LibDir}ScriptMgr.lib"	ƒ	{ScriptMgrObjects}
	Lib {StdLibOpts} -o "{Targ}" {ScriptMgrObjects}


"{ObjDir}ScriptMgrDispatch.a.o"		 ƒ	"{IntAIncludes}ScriptPriv.a" 				∂
										"{IntAIncludes}IntlUtilsPriv.a"				∂
										"{ObjDir}StandardEqu.d"						∂
										"{ScriptMgrDir}ScriptMgrDispatch.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrDispatch.a"


"{ObjDir}ScriptMgrHeader.a.o"		ƒ	"{IntAIncludes}ScriptPriv.a"				∂
										"{ObjDir}StandardEqu.d"						∂
										"{ScriptMgrDir}ScriptMgrHeader.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrHeader.a"


"{ObjDir}ScriptMgrInit.a.o"			ƒ	"{IntAIncludes}ScriptPriv.a" 				∂
										"{AIncludes}Packages.a"						∂
										"{ObjDir}StandardEqu.d"						∂
										"{ScriptMgrDir}ScriptMgrInit.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrInit.a"


"{ObjDir}ScriptMgrExtensions.a.o" 	ƒ	"{ScriptMgrDir}ScriptMgrExtensions.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrExtensions.a"


"{ObjDir}ScriptMgrKeyGetSet.a.o" 	ƒ	"{IntAIncludes}ScriptPriv.a" 				∂
										"{IntAIncludes}TSMPrivate.a"				∂
										"{IntAIncludes}MenuMgrPriv.a"				∂
										"{AIncludes}Components.a"					∂
										"{AIncludes}TextServices.a"					∂
										"{ObjDir}StandardEqu.d"						∂
										"{ScriptMgrDir}ScriptMgrKeyGetSet.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrKeyGetSet.a"


"{ObjDir}ScriptMgrMisc.a.o"			ƒ	"{IntAIncludes}ScriptPriv.a"				∂
										"{ObjDir}StandardEqu.d"						∂
										"{AIncludes}ApplDeskBus.a"					∂
										"{ScriptMgrDir}ScriptMgrMisc.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrMisc.a"


"{ObjDir}RomanUtil.a.o"				ƒ	"{IntAIncludes}ScriptPriv.a"				∂
										"{ObjDir}StandardEqu.d"						∂
										"{AIncludes}Packages.a"						∂
										"{ScriptMgrDir}RomanUtil.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}RomanUtil.a"


"{ObjDir}RomanNewJust.a.o"			ƒ	"{IntAIncludes}ScriptPriv.a"				∂
										"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}ColorEqu.a"					∂
										"{AIncludes}FixMath.a"						∂
										"{ScriptMgrDir}RomanNewJust.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}RomanNewJust.a"


"{ObjDir}ScriptMgrUtilDate.a.o"		ƒ	"{IntAIncludes}ScriptPriv.a"				∂
										"{ObjDir}StandardEqu.d"						∂
										"{AIncludes}Packages.a"						∂
										"{AIncludes}SANEMacs.a"						∂
										"{AIncludes}FixMath.a"						∂
										"{ScriptMgrDir}ScriptMgrUtilDate.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrUtilDate.a"


"{ObjDir}ScriptMgrUtilText.a.o"		ƒ	"{IntAIncludes}ScriptPriv.a"				∂
										"{ObjDir}StandardEqu.d"						∂
										"{ScriptMgrDir}ScriptMgrUtilText.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrUtilText.a"


"{ObjDir}ScriptMgrTruncRepl.a.o"	ƒ	"{IntAIncludes}ScriptPriv.a"				∂
										"{ObjDir}StandardEqu.d"						∂
										"{AIncludes}Packages.a"						∂
										"{ScriptMgrDir}ScriptMgrTruncRepl.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrTruncRepl.a"


"{ObjDir}ScriptMgrFindWord.c.o"		ƒ 	"{CIncludes}Types.h"						∂
										"{CIncludes}Script.h" 						∂
										"{IntCIncludes}ScriptPriv.h"				∂
										"{ScriptMgrDir}ScriptMgrFindWord.c"
	C {StdCOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrFindWord.c"


"{ObjDir}ScriptMgrKbdMenu.a.o"		ƒ 	"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}ScriptPriv.a"				∂
										"{IntAIncludes}IconUtilsPriv.a"				∂
										"{IntAIncludes}MenuMgrPriv.a"				∂
										"{AIncludes}Components.a"					∂
										"{AIncludes}TextServices.a"					∂
										"{AIncludes}Packages.a"						∂
										"{AIncludes}Icons.a"						∂
										"{ScriptMgrDir}ScriptMgrKbdMenu.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrKbdMenu.a"


"{ObjDir}ScriptMgrSysMenuPatch.a.o"	ƒ 	"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}ScriptPriv.a"				∂
										"{IntAIncludes}MFPrivate.a"					∂
										"{IntAIncludes}DialogsPriv.a"				∂
										"{IntAIncludes}MenuMgrPriv.a"				∂
										"{AIncludes}Components.a"					∂
										"{AIncludes}Icons.a"						∂
										"{ScriptMgrDir}ScriptMgrSysMenuPatch.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrSysMenuPatch.a"


"{ObjDir}DblByteCompat.a.o"			ƒ	"{AIncludes}SysEqu.a"						∂
										"{AIncludes}Script.a"						∂
										"{IntAIncludes}ScriptPriv.a"				∂
										"{ScriptMgrDir}DblByteCompat.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}DblByteCompat.a"


"{ObjDir}ScriptMgrUtilNum.a.o"		ƒ	"{IntAIncludes}ScriptPriv.a" 				∂
										"{ObjDir}StandardEqu.d"						∂
										"{AIncludes}Packages.a"						∂
										"{AIncludes}SANEMacs.a"	∂
										"{ScriptMgrDir}ScriptMgrUtilNum.a"
	Asm {StdAOpts} -o "{Targ}" "{ScriptMgrDir}ScriptMgrUtilNum.a"




