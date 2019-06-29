#
#	File:		ListMgr.make
#
#	Contains:	Makefile for List Manager.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc.  All rights reserved.
#
#	Change History (most recent first):
#
#	   <SM4>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency.
#	   <SM3>	11/30/92	SWC		Changed PackMacs.a->Packages.a.
#	   <SM2>	11/20/92	RB		Changed SndLOpts to StdLOpts
#


"{RsrcDir}ListMgr.rsrc"			ƒƒ  "{ObjDir}IconLDEF.a.o"			
	Link {StdLOpts} {StdAlign} -o "{Targ}" -rt LDEF=19 "{ObjDir}IconLDEF.a.o" || Exit 1


"{RsrcDir}ListMgr.rsrc"			ƒƒ  "{ObjDir}ListMgrPACK.a.o"		
	Link {StdLOpts} {StdAlign} -o "{Targ}" -rt PACK=0 "{ObjDir}ListMgrPACK.a.o" || Exit 1


"{RsrcDir}ListMgr.rsrc"			ƒƒ  "{ObjDir}TextLDEF.a.o"
	Link {StdLOpts} {StdAlign} -o "{Targ}" -rt LDEF=0 "{ObjDir}TextLDEF.a.o" || Exit 1


"{ObjDir}IconLDEF.a.o"			ƒ 	"{ListMgrDir}IconLDEF.a"						∂
									"{AIncludes}SysEqu.a"							∂
									"{IntAIncludes}SysPrivateEqu.a"					∂
									"{AIncludes}QuickDraw.a"						∂
									"{AIncludes}Traps.a"							∂
									"{AIncludes}Packages.a"							∂
									"{AIncludes}Script.a"							∂
									"{IntAIncludes}IconUtilsPriv.a"
	Asm {StdAOpts} -o "{Targ}" "{ListMgrDir}IconLDEF.a"


"{ObjDir}ListMgrPACK.a.o"		ƒ 	"{ObjDir}StandardEqu.d"							∂
									"{AIncludes}Packages.a"							∂
									"{ListMgrDir}ListMgrPriv.a"						∂
									"{ListMgrDir}ListMgrPACK.a"
	Asm {StdAOpts} -o "{Targ}" "{ListMgrDir}ListMgrPACK.a"


"{ObjDir}TextLDEF.a.o"			ƒ 	"{AIncludes}SysEqu.a"							∂
									"{IntAIncludes}SysPrivateEqu.a"					∂
									"{AIncludes}Traps.a"							∂
									"{AIncludes}QuickDraw.a"						∂
									"{AIncludes}Packages.a"							∂
									"{ListMgrDir}TextLDEF.a"
	Asm {StdAOpts} -o "{Targ}" "{ListMgrDir}TextLDEF.a"



