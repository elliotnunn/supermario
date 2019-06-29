#
#	File:		AppleEventMgr.make
#
#	Contains:	Makefile for the Apple Event Manager Package.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM2>	11/14/92	kc		Change link option used to name segment from -sg to -sn.

	
AppleEventMgrObjs				=	"{ObjDir}AEDFGlue.a.o"							∂
									"{ObjDir}AEHashTable.a.o"						∂
									"{ObjDir}AEUtil.p.o"							∂
									"{ObjDir}AEUtil.a.o"							∂
									"{ObjDir}AEHandlerTable.p.o"					∂
									"{ObjDir}AEHandlerTable.a.o"					∂
									"{ObjDir}AECoercion.p.o"						∂
									"{ObjDir}AEDFWrapper.p.o"						∂
									"{ObjDir}AEDFWrapper.a.o"						∂
									"{ObjDir}AEDF.p.o"								∂
									"{ObjDir}AEInteract.p.o"						∂
									"{IfObjDir}Interface.o"							∂
									"{PLibraries}SANELib.o" 						∂
									"{PLibraries}PasLib.o"


"{RsrcDir}AppleEventMgr.PACK.rsrc"	ƒ	{AppleEventMgrObjs}
	Link {StdLOpts} {StdAlign} -o "{Targ}" -rt PACK=8 -sn Main="AevtMgr" {AppleEventMgrObjs}


"{RsrcDir}AppleEventMgr.rsrc"	ƒ	{AppleEventMgrObjs}
	Link {StdLOpts} {StdAlign} -o "{Targ}" -rt PACK=8 -sn Main="AevtMgr" {AppleEventMgrObjs}


"{ObjDir}AEDFGlue.a.o"			ƒ	"{ObjDir}StandardEqu.d"							∂
									"{AppleEventDir}AEDFGlue.a"
	Asm {StdAOpts} -o "{Targ}" "{AppleEventDir}AEDFGlue.a"


"{ObjDir}AEHashTable.a.o"			ƒ	"{ObjDir}StandardEqu.d"						∂
									"{AppleEventDir}AEHashTable.a"
	Asm {StdAOpts} -o "{Targ}" "{AppleEventDir}AEHashTable.a"


"{ObjDir}AEUtil.a.o"			ƒ	"{ObjDir}StandardEqu.d"							∂
									"{AppleEventDir}AEUtil.a"
	Asm {StdAOpts} -o "{Targ}" "{AppleEventDir}AEUtil.a"


"{ObjDir}AEHandlerTable.a.o"	ƒ	"{ObjDir}StandardEqu.d"							∂
									"{AppleEventDir}AEHandlerTable.a"
	Asm {StdAOpts} -o "{Targ}" "{AppleEventDir}AEHandlerTable.a"


"{ObjDir}AEDFWrapper.a.o"		ƒ	"{ObjDir}StandardEqu.d"							∂
									"{AppleEventDir}AEDFWrapper.a"
	Asm {StdAOpts} -o "{Targ}" "{AppleEventDir}AEDFWrapper.a"


"{ObjDir}AEUtil.p.o"			ƒ	"{AppleEventDir}AEUtil.p"						∂
									"{AppleEventDir}AEUtil.inc1.p"					∂
									"{AppleEventDir}AEHashTable.p"					∂
									"{PInterfaces}AppleEvents.p"					∂
									"{IntPInterfaces}StandardEqu.p"		
	Pascal {StdPOpts} -o "{Targ}" "{AppleEventDir}AEUtil.p"


"{ObjDir}AEHandlerTable.p.o"	ƒ	"{AppleEventDir}AEHandlerTable.p"				∂
									"{AppleEventDir}AEHandlerTable.inc1.p"			∂
									"{AppleEventDir}AEUtil.inc1.p"					∂
									"{AppleEventDir}AEHashTable.p"					∂
									"{PInterfaces}AppleEvents.p"					∂
									"{IntPInterfaces}StandardEqu.p"
	Pascal {StdPOpts} -o "{Targ}" "{AppleEventDir}AEHandlerTable.p"


"{ObjDir}AECoercion.p.o"		ƒ	"{AppleEventDir}AECoercion.p"					∂
									"{AppleEventDir}AECoercion.inc1.p"				∂
									"{AppleEventDir}AEUtil.inc1.p"					∂
									"{AppleEventDir}AEHashTable.p"					∂
									"{AppleEventDir}AEHandlerTable.p"				∂
									"{PInterfaces}AppleEvents.p"					∂
									"{IntPInterfaces}StandardEqu.p"
	Pascal {StdPOpts} -o "{Targ}" "{AppleEventDir}AECoercion.p"


"{ObjDir}AEDFWrapper.p.o"		ƒ	"{AppleEventDir}AEDFWrapper.p"					∂
									"{AppleEventDir}AEDFWrapper.inc1.p"				∂
									"{AppleEventDir}AEUtil.inc1.p"					∂
									"{AppleEventDir}AEHashTable.p"					∂
									"{AppleEventDir}AEHandlerTable.p"				∂
									"{AppleEventDir}AECoercion.p"					∂
									"{PInterfaces}AppleEvents.p"					∂
									"{IntPInterfaces}StandardEqu.p"
	Pascal {StdPOpts} -o "{Targ}" "{AppleEventDir}AEDFWrapper.p"


"{ObjDir}AEDF.p.o"				ƒ	"{AppleEventDir}AEDF.p"							∂
									"{AppleEventDir}AEDF.inc1.p"					∂
									"{AppleEventDir}AEUtil.inc1.p"					∂
									"{AppleEventDir}AEHashTable.p"					∂
									"{AppleEventDir}AEHandlerTable.p"				∂
									"{AppleEventDir}AECoercion.p"					∂
									"{AppleEventDir}AEDFWrapper.p"					∂
									"{PInterfaces}AppleEvents.p"					∂
									"{IntPInterfaces}StandardEqu.p"
	Pascal {StdPOpts} -o "{Targ}" "{AppleEventDir}AEDF.p"


"{ObjDir}AEInteract.p.o"		ƒ	"{AppleEventDir}AEDF.p"							∂
									"{AppleEventDir}AEInteract.inc1.p"				∂
									"{AppleEventDir}AEUtil.inc1.p"					∂
									"{AppleEventDir}AEHashTable.p"					∂
									"{AppleEventDir}AECoercion.p"					∂
									"{AppleEventDir}AEDFWrapper.p"					∂
									"{AppleEventDir}AEDF.p"							∂
									"{PInterfaces}AppleEvents.p"					∂
									"{IntPInterfaces}StandardEqu.p"
	Pascal {StdPOpts} -o "{Targ}" "{AppleEventDir}AEInteract.p"


"{ObjDir}AEPackEntry.a.o"			ƒ	"{ObjDir}StandardEqu.d"						∂
									"{AppleEventDir}AEPackEntry.a"					∂
									"{IntAIncludes}DispatchHelperPriv.a"
	Asm {StdAOpts} -o "{Targ}" "{AppleEventDir}AEPackEntry.a"


"{ObjDir}AppleEventMgr.lib"			ƒ	"{ObjDir}AEPackEntry.a.o"
	Lib {StdLibOpts} -o "{Targ}" "{ObjDir}AEPackEntry.a.o"

