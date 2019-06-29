#
#	File:		EditionMgr.make
#
#	Contains:	Makefile for the Edition Manager.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM4>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency.
#	   <SM3>	12-02-92	jmp		Added the -append option to the Rez command.
#	   <SM2>	11/12/92	kc		Add -i option to rez command.


# dpDispatcher.a.o must be first, dpDialogs should be last

EditionMgrObjs				=		"{ObjDir}dpDispatcher.a.o"						∂
									"{ObjDir}dpInit.p.o"							∂
									"{ObjDir}dpSection.p.o"							∂
									"{ObjDir}dpSectionIO.p.o"						∂
									"{ObjDir}dpMisc.p.o"							∂
									"{ObjDir}dpPubControlBlock.p.o"					∂
									"{ObjDir}dpPubIO.p.o"							∂
									"{ObjDir}dpEvents.p.o"							∂
									"{IfObjDir}interface.o"							∂
									"{ObjDir}dpMisc.a.o"							∂
									"{ObjDir}dpDialogs.p.o"


"{LibDir}EditionMgr.lib"		ƒ	"{ObjDir}dpPackEntry.a.o"
	Lib	{StdLibOpts} -o "{Targ}" "{ObjDir}dpPackEntry.a.o"


"{RsrcDir}EditionMgr.rsrc"		ƒƒ	"{EditionMgrDir}dpResources.r" 					∂
									"{StandardFileDir}StandardFileTypes.r"			∂
									"{RIncludes}Types.r" 							∂
									"{RIncludes}BalloonTypes.r"						
	Rez {StdROpts} -o "{Targ}" "{EditionMgrDir}dpResources.r"						∂
				   -i "{StandardFileDir}" -append || Exit 1


"{RsrcDir}EditionMgr.rsrc"		ƒƒ	{EditionMgrObjs}
	Link {StdLOpts} {StdAlign} -o "{Targ}" -rt PACK=11 -m PACKENTRY {EditionMgrObjs} || Exit 1


"{ObjDir}dpPackEntry.a.o"		ƒ	"{ObjDir}StandardEqu.d"							∂
									"{EditionMgrDir}dpPackEntry.a"					∂
									"{IntAIncludes}DispatchHelperPriv.a"
	Asm {StdAOpts} -o "{targ}" "{EditionMgrDir}dpPackEntry.a"


"{ObjDir}dpDispatcher.a.o"		ƒ	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}EditionsPrivate.a"				∂
									"{EditionMgrDir}dpCompileOptions.inc.a"			∂
									"{EditionMgrDir}dpDispatcher.a"
	Asm {StdAOpts} -o "{targ}" "{EditionMgrDir}dpDispatcher.a"


"{ObjDir}dpEvents.p.o"			ƒ	"{EditionMgrDir}dpEvents.inc.p"					∂
									"{EditionMgrDir}dpCompileOptions.inc.p"			∂
									"{IntPInterfaces}StandardEqu.p"					∂
									"{PInterfaces}Editions.p"						∂
									"{PInterfaces}Aliases.p"						∂
									"{IntPInterfaces}AppleEventsInternal.p"			∂
									"{IntPInterfaces}EditionsPrivate.p" 			∂
									"{IntPInterfaces}MFPrivate.p"					∂
									"{EditionMgrDir}dpMisc.p"						∂
									"{EditionMgrDir}dpPubControlBlock.p"			∂
									"{EditionMgrDir}dpPubIO.p"						∂
									"{EditionMgrDir}dpSection.p"					∂
									"{EditionMgrDir}dpEvents.p"
	Pascal {StdPOpts} -o "{targ}" "{EditionMgrDir}dpEvents.p"


"{ObjDir}dpInit.p.o"			ƒ	"{EditionMgrDir}dpInit.inc.p"					∂
									"{EditionMgrDir}dpCompileOptions.inc.p"			∂
									"{IntPInterfaces}StandardEqu.p"					∂
									"{PInterfaces}Editions.p"						∂
									"{PInterfaces}Aliases.p"						∂
									"{IntPInterfaces}EditionsPrivate.p" 			∂
									"{EditionMgrDir}dpMisc.p"						∂
									"{EditionMgrDir}dpPubControlBlock.p"			∂
									"{EditionMgrDir}dpInit.p"
	Pascal {StdPOpts} -o "{targ}" "{EditionMgrDir}dpInit.p"


"{ObjDir}dpMisc.a.o"			ƒ	"{ObjDir}StandardEqu.d"							∂
									"{IntAIncludes}EditionsPrivate.a"				∂
									"{EditionMgrDir}dpCompileOptions.inc.a"			∂
									"{EditionMgrDir}dpMisc.a"
	Asm {StdAOpts} -o "{targ}" "{EditionMgrDir}dpMisc.a"


"{ObjDir}dpMisc.p.o"			ƒ	"{EditionMgrDir}dpMisc.inc.p"					∂
									"{EditionMgrDir}dpCompileOptions.inc.p"			∂
									"{IntPInterfaces}StandardEqu.p"					∂
									"{PInterfaces}Editions.p"						∂
									"{IntPInterfaces}EditionsPrivate.p"				∂
									"{EditionMgrDir}dpMisc.p"
	Pascal {StdPOpts} -o "{Targ}" "{EditionMgrDir}dpMisc.p"


"{ObjDir}dpPubControlBlock.p.o"	ƒ	"{EditionMgrDir}dpPubControlBlock.inc.p"		∂
									"{EditionMgrDir}dpCompileOptions.inc.p"			∂
									"{IntPInterfaces}StandardEqu.p"					∂
									"{PInterfaces}Editions.p"						∂
									"{IntPInterfaces}EditionsPrivate.p" 	 		∂
									"{EditionMgrDir}dpMisc.p"						∂
									"{EditionMgrDir}dpPubIO.p"						∂
									"{EditionMgrDir}dpPubControlBlock.p"
	Pascal {StdPOpts} -o "{targ}" "{EditionMgrDir}dpPubControlBlock.p"


"{ObjDir}dpPubIO.p.o"			ƒ	"{EditionMgrDir}dpPubIO.inc.p"					∂
									"{EditionMgrDir}dpCompileOptions.inc.p"			∂
									"{IntPInterfaces}StandardEqu.p"					∂
									"{PInterfaces}Editions.p"						∂
									"{PInterfaces}Aliases.p"						∂
									"{IntPInterfaces}EditionsPrivate.p" 			∂
									"{IntPInterfaces}MFPrivate.p"					∂
									"{EditionMgrDir}dpMisc.p"						∂
									"{EditionMgrDir}dpEvents.p"						∂
									"{EditionMgrDir}dpSectionIO.p"					∂
									"{EditionMgrDir}dpSection.p"					∂
									"{EditionMgrDir}dpPubControlBlock.p"			∂
									"{EditionMgrDir}dpPubIO.p"
	Pascal {StdPOpts} -o "{targ}" "{EditionMgrDir}dpPubIO.p"


"{ObjDir}dpSection.p.o"			ƒ	"{EditionMgrDir}dpSection.inc.p"				∂
									"{EditionMgrDir}dpCompileOptions.inc.p"			∂
									"{IntPInterfaces}StandardEqu.p"					∂
									"{PInterfaces}Editions.p"						∂
									"{PInterfaces}Aliases.p" 						∂
									"{IntPInterfaces}EditionsPrivate.p" 			∂
									"{EditionMgrDir}dpMisc.p"						∂
									"{EditionMgrDir}dpPubControlBlock.p"			∂
									"{EditionMgrDir}dpEvents.p"						∂
									"{EditionMgrDir}dpPubIO.p"						∂
									"{EditionMgrDir}dpSection.p"
	Pascal {StdPOpts} -o "{targ}" "{EditionMgrDir}dpSection.p"


"{ObjDir}dpSectionIO.p.o"		ƒ	"{EditionMgrDir}dpSectionIO.inc.p"				∂
									"{EditionMgrDir}dpCompileOptions.inc.p"			∂
									"{IntPInterfaces}StandardEqu.p"					∂
									"{PInterfaces}Editions.p"						∂
									"{IntPInterfaces}EditionsPrivate.p" 			∂
									"{EditionMgrDir}dpMisc.p"						∂
									"{EditionMgrDir}dpSection.p"					∂
									"{EditionMgrDir}dpPubIO.p"						∂
									"{EditionMgrDir}dpSectionIO.p"
	Pascal {StdPOpts} -o "{targ}" "{EditionMgrDir}dpSectionIO.p"


"{ObjDir}dpDialogs.p.o"			ƒ	"{EditionMgrDir}dpDialogs.inc.p"				∂
									"{EditionMgrDir}dpCompileOptions.inc.p"			∂
									"{IntPInterfaces}StandardEqu.p"					∂
									"{PInterfaces}Editions.p"						∂
									"{PInterfaces}StandardFile.p"					∂
									"{PInterfaces}CTBUtilities.p"					∂
									"{IntPInterfaces}EditionsPrivate.p" 			∂
									"{IntPInterfaces}IconUtilsPriv.p" 				∂
									"{PInterfaces}Icons.p" 							∂
									"{IntPInterfaces}Layers.p" 						∂
									"{IntPInterfaces}DialogsPriv.p"					∂
									"{EditionMgrDir}dpMisc.p"						∂
									"{EditionMgrDir}dpPubIO.p"						∂
									"{EditionMgrDir}dpEvents.p"						∂
									"{EditionMgrDir}dpDialogs.p"
	Pascal {StdPOpts} -o "{targ}" "{EditionMgrDir}dpDialogs.p"


