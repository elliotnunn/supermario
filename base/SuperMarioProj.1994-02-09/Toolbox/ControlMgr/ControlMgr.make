#
#	File:		ControlMgr.make
#
#	Contains:	Makefile for the Control Manager.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM2>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency.


ControlMgrObjs		=			"{ObjDir}ControlMgr.a.o"							∂
								"{ObjDir}ControlMgrExtensions.a.o"	


"{RsrcDir}ControlMgr.rsrc"			ƒƒ	"{ObjDir}ButtonCDEF.a.o"					
	Link {StdLOpts} {StdAlign} -rt CDEF=0 -o "{Targ}" "{ObjDir}ButtonCDEF.a.o" || Exit 1


"{RsrcDir}ControlMgr.rsrc"			ƒƒ	"{ObjDir}ScrollBarCDEF.a.o"					
	Link {StdLOpts} {StdAlign} -rt CDEF=1 -o "{Targ}" "{ObjDir}ScrollBarCDEF.a.o" || Exit 1


"{LibDir}ControlMgr.lib"			ƒ 	{ControlMgrObjs}
	Lib {StdLibOpts} -o "{Targ}"	{ControlMgrObjs}


"{ObjDir}ControlMgr.a.o"			ƒ	"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}LinkedPatchMacros.a"			∂
										"{IntAIncludes}ControlPriv.a"				∂
										"{ControlMgrDir}ControlMgr.a"
	Asm {StdAOpts} -o "{Targ}" "{ControlMgrDir}ControlMgr.a"


"{ObjDir}ControlMgrExtensions.a.o"	ƒ	"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}LinkedPatchMacros.a"			∂
										"{IntAIncludes}ControlPriv.a"				∂
										"{ControlMgrDir}ControlMgrExtensions.a"
	Asm {StdAOpts} -o "{Targ}" "{ControlMgrDir}ControlMgrExtensions.a"


"{ObjDir}ScrollBarCDEF.a.o"		ƒ	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}ColorEqu.a"						∂
									"{ControlMgrDir}ScrollBarCDEF.a"
	Asm {StdAOpts} -o "{Targ}" "{ControlMgrDir}ScrollBarCDEF.a"


"{ObjDir}ButtonCDEF.a.o"		ƒ	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}ColorEqu.a"						∂
									"{ControlMgrDir}ButtonCDEF.a"
	Asm {StdAOpts} -o "{Targ}" "{ControlMgrDir}ButtonCDEF.a"
