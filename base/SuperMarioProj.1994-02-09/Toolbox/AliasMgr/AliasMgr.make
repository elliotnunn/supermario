#
#	File:		AliasMgr.make
#
#	Contains:	Makefile for the Alias manager Library.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#


AliasMgrObjects			=	"{ObjDir}alDlogUtil.a.o"		∂
							"{ObjDir}alExt.c.o"				∂
							"{ObjDir}alFill.c.o"			∂
							"{ObjDir}alFind.c.o"			∂
							"{ObjDir}AliasMgr.a.o"			∂
							"{ObjDir}alUtil.c.o"			∂
							"{ObjDir}FolderMgr.a.o"


"{LibDir}AliasMgr.lib"			ƒ	{AliasMgrObjects}
	Lib {StdLibOpts} -o "{Targ}" {AliasMgrObjects} 


"{ObjDir}alDlogUtil.a.o"		ƒ	"{ObjDir}StandardEqu.d" 						∂
									"{AliasMgrDir}alDlogUtil.a"
	Asm {StdAOpts} -o "{Targ}" "{AliasMgrDir}alDlogUtil.a"


"{ObjDir}alExt.c.o"				ƒ 	"{CIncludes}Aliases.h" 							∂
									"{CIncludes}Memory.h" 							∂
									"{CIncludes}Files.h" 							∂
									"{CIncludes}errors.h" 							∂
									"{CIncludes}String.h" 							∂
									"{CIncludes}ToolUtils.h" 						∂
									"{CIncludes}OSUtils.h"							∂
									"{CIncludes}Resources.h"						∂
									"{AliasMgrDir}aliasPriv.h" 						∂
									"{AliasMgrDir}alExt.c"
	C {StdCOpts} -o "{Targ}" "{AliasMgrDir}alExt.c"


"{ObjDir}alFill.c.o"			ƒ	"{CIncludes}Aliases.h" 							∂
									"{CIncludes}Memory.h" 							∂
									"{CIncludes}files.h" 							∂
									"{CIncludes}errors.h" 							∂
									"{AliasMgrDir}aliasPriv.h" 						∂
									"{AliasMgrDir}alFill.c"
	C {StdCOpts} -o "{Targ}" "{AliasMgrDir}alFill.c"


"{ObjDir}alFind.c.o"			ƒ	"{CIncludes}Aliases.h" 							∂
									"{CIncludes}Memory.h" 							∂
									"{CIncludes}files.h" 							∂
									"{CIncludes}errors.h" 							∂
									"{CIncludes}String.h" 							∂
									"{CIncludes}ToolUtils.h" 						∂
									"{CIncludes}OSUtils.h"							∂
									"{CIncludes}StdLib.h"							∂
									"{AliasMgrDir}aliasPriv.h" 						∂
									"{AliasMgrDir}alFind.c"
	C {StdCOpts} -o "{Targ}" "{AliasMgrDir}alFind.c"


"{ObjDir}alUtil.c.o"			ƒ 	"{CIncludes}Aliases.h" 							∂
									"{CIncludes}Memory.h" 							∂
									"{CIncludes}errors.h" 							∂
									"{CIncludes}Dialogs.h" 							∂
									"{CIncludes}ToolUtils.h" 						∂
									"{AliasMgrDir}aliasPriv.h" 						∂
									"{AliasMgrDir}aliasDlogPriv.h"						∂
									"{AliasMgrDir}alUtil.c"
	C {StdCOpts} -o "{Targ}" "{AliasMgrDir}alUtil.c"


"{ObjDir}AliasMgr.a.o"			ƒ 	"{ObjDir}StandardEqu.d"		 					∂
									"{AIncludes}GestaltEqu.a"						∂
									"{AIncludes}AppleTalk.a" 						∂
									"{IntAIncludes}InternalMacros.a"				∂
									"{IntAIncludes}LinkedPatchMacros.a"				∂
									"{AliasMgrDir}AliasMgr.a"
	Asm {StdAOpts} -o "{Targ}" "{AliasMgrDir}AliasMgr.a"


"{ObjDir}FolderMgr.a.o"			ƒ 	"{AIncludes}SysEqu.a"							∂
									"{IntAIncludes}SysPrivateEqu.a"					∂
									"{AIncludes}ToolUtils.a" 							∂
									"{AIncludes}SysErr.a" 							∂
									"{AIncludes}Traps.a" 							∂
									"{IntAIncludes}InternalMacros.a" 				∂
									"{AIncludes}Folders.a"							∂
									"{AIncludes}GestaltEqu.a"						∂
									"{AliasMgrDir}FolderMgr.a"
	Asm {StdAOpts} -o "{Targ}" "{AliasMgrDir}FolderMgr.a"
