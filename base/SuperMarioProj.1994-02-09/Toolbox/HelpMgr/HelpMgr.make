#
#	File:		HelpMgr.make
#
#	Contains:	Makefile to build the resources needed for the helpmgr (balloon help).
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM4>	12/11/92	chp		Removed runtime library routines not needed when compiled for
#									68020 or better.
#	   <SM3>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency.
#	   <SM2>	 11/17/92	jy		changed BalloonPackObjs to BalloonWDEFObjs for WDEF link
#									into HelpMgr.rsrc

BalloonPackObjs					=	"{ObjDir}BalloonPack.a.o"						∂
									"{ObjDir}BalloonPack.p.o"						∂
									"{IfObjDir}interface.o"


BalloonWDEFObjs					=	"{ObjDir}BalloonWDEF.a.o"						∂
									"{ObjDir}BalloonWDEF.p.o"



"{LibDir}HelpMgr.lib"		ƒ	"{ObjDir}BalloonExtensions.a.o" 					∂
								"{ObjDir}Balloonptch28.p.o"
	Lib	{StdLibOpts} -o "{Targ}"	"{ObjDir}BalloonExtensions.a.o" "{ObjDir}Balloonptch28.p.o"

"{RsrcDir}HelpMgr.rsrc"			ƒƒ	{BalloonPackObjs}								
	Link {StdLOpts} {StdAlign} -rt PACK=14 -o "{Targ}" {BalloonPackObjs} || Exit 1


"{RsrcDir}HelpMgr.rsrc"			ƒƒ	{BalloonWDEFObjs}								
	Link {StdLOpts} {StdAlign} -rt WDEF=126 -o "{Targ}" {BalloonWDEFObjs} || Exit 1


"{RsrcDir}HelpMgr.rsrc"			ƒƒ	"{RIncludes}Types.r"							∂
									"{RIncludes}BalloonTypes.r"						∂
									"{HelpMgrDir}Balloon.r"
	Rez {StdROpts} -a -o "{Targ}" "{HelpMgrDir}Balloon.r" || Exit 1


"{ObjDir}BalloonPACK.a.o"		ƒ 	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}MFPrivate.a"						∂
									"{IntAIncludes}LayerEqu.a"						∂
									"{IntAIncludes}ScriptPriv.a"					∂
									"{IntAIncludes}InternalMacros.a"				∂
									"{AIncludes}Processes.a"						∂
									"{AIncludes}SysEqu.a"							∂
									"{IntAIncludes}BalloonsPriv.a"					∂
									"{IntAIncludes}DialogsPriv.a"					∂
									"{HelpMgrDir}BalloonPACK.a"
	Asm {StdAOpts} -o "{targ}" "{HelpMgrDir}BalloonPACK.a"


"{ObjDir}BalloonPACK.p.o"		ƒ	"{HelpMgrDir}BalloonPACK.p"						∂
									"{HelpMgrDir}BalloonDefs.p"						∂
									"{IntPInterfaces}MenuMgrPriv.p"					∂
									"{IntPInterfaces}MFPrivate.p"
	Pascal {StdPOpts} -o "{targ}" "{HelpMgrDir}BalloonPACK.p"


"{ObjDir}BalloonWDEF.a.o"		ƒ 	"{IntAIncludes}BalloonsPriv.a"					∂
									"{HelpMgrDir}BalloonWDEF.a"
	Asm {StdAOpts} -o "{targ}" "{HelpMgrDir}BalloonWDEF.a"


"{ObjDir}BalloonWDEF.p.o"		ƒ "{HelpMgrDir}BalloonWDEF.p"
	Pascal {StdPOpts} -o "{targ}" "{HelpMgrDir}BalloonWDEF.p"


"{ObjDir}BalloonExtensions.a.o"	ƒ 	"{ObjDir}StandardEqu.d"							∂
									"{AIncludes}GestaltEqu.a"						∂
									"{IntAIncludes}"IconUtilsPriv.a					∂
									"{IntAIncludes}BalloonsPriv.a"					∂
									"{HelpMgrDir}BalloonExtensions.a"				∂
									"{IntAIncludes}LinkedPatchMacros.a"
	Asm {StdAOpts} -o "{targ}" "{HelpMgrDir}BalloonExtensions.a"


"{ObjDir}Balloonptch28.p.o"		ƒ 	"{PInterfaces}Balloons.p"						∂
									"{HelpMgrDir}BalloonDefs.p"						∂
									"{IntPInterfaces}DialogsPriv.p"					∂
									"{IntPInterfaces}MenuMgrPriv.p"					∂
									"{IntPInterfaces}StandardEqu.p"					∂
									"{HelpMgrDir}Balloonptch28.p"
	Pascal {StdPOpts} -o "{targ}" "{HelpMgrDir}Balloonptch28.p"
