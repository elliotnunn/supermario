#
#	File:		ComponentMgr.make
#
#	Contains:	Makefile for the Component Manager.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992-1993 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM4>	  8/5/93	JDR		integrate Component Mgr from Reality, complete set of
#									dependancies now (which it never had)
#	   <SM3>	 7/21/93	RC		changed ThingManagerDisp to ThingManagerDispatcher since the two
#									files are the same and the first does not exist in Reality
#	   <SM2>	 6/14/93	kc		Roll in Ludwig.
#	   <LW2>	  4/9/93	fau		Added a DepVideo.h dependency to ThingManager.c.
#

ComponentMgrObjects 				=	"{ObjDir}ThingManagerDispatcher.a.o"										∂
										"{ObjDir}ThingManager.c.o"						∂
										"{ObjDir}ThingSearch.c.o"						∂
										"{ObjDir}ThingFiles.c.o"						∂

"{LibDir}ComponentMgr.lib"			ƒ	{ComponentMgrObjects}
	Lib {StdLibOpts} {ComponentMgrObjects} -o "{Targ}"


"{ObjDir}ThingManagerDispatcher.a.o" ƒ	"{ComponentMgrDir}ThingManagerDispatcher.a"		∂
										"{ComponentMgrDir}ComponentMgr.make"			∂
										"{ComponentMgrDir}ComponentManagerSelectors.h"	∂
										"{ObjDir}StandardEqu.d"							∂
										"{AIncludes}Components.a"						∂
										"{IntAIncludes}LinkedPatchMacros.a"
	Asm {StdAOpts} "{ComponentMgrDir}ThingManagerDispatcher.a" -o "{Targ}"


"{ObjDir}ThingManager.c.o" 			ƒ  	"{ComponentMgrDir}ThingManager.c" 				∂
										"{ComponentMgrDir}ComponentMgr.make"			∂
										"{CIncludes}Types.h"							∂
										"{CIncludes}Memory.h"							∂
										"{CIncludes}OSUtils.h"							∂
										"{CIncludes}Resources.h"						∂
										"{CIncludes}Files.h"							∂
										"{CIncludes}Errors.h"							∂
										"{CIncludes}GestaltEqu.h"						∂
										"{CIncludes}SysEqu.h"							∂
										"{CIncludes}Traps.h"							∂
										"{IntCIncludes}MemoryMgrPriv.h"					∂
										"{IntCIncludes}FigmentMemory.h"					∂
										"{IntCIncludes}ExpandMemPriv.h"					∂
										"{IntCIncludes}TrapsPrivate.h"					∂
										"{CIncludes}LowMem.h"							∂
										"{CIncludes}FragLoad.h"							∂
										"{ComponentMgrDir}Components.h"					∂
										"{ComponentMgrDir}ComponentManagerSelectors.h"	∂
										"{ComponentMgrDir}PrivateThingManager.h"
	C {StdCOpts} "{ComponentMgrDir}ThingManager.c" -o "{Targ}"


"{ObjDir}ThingSearch.c.o" 			ƒ  	"{ComponentMgrDir}ThingSearch.c" 				∂
										"{ComponentMgrDir}ComponentMgr.make"			∂
										"{ComponentMgrDir}ComponentManagerSelectors.h"	∂
										"{ComponentMgrDir}Components.h"					∂
										"{CIncludes}Resources.h"						∂
										"{CIncludes}Memory.h"							∂
										"{CIncludes}SysEqu.h"							∂
										"{CIncludes}Aliases.h"							∂
										"{CIncludes}Finder.h"							∂
										"{CIncludes}Errors.h"							∂
										"{CIncludes}Folders.h"
	C {StdCOpts} "{ComponentMgrDir}ThingSearch.c" -o "{Targ}"

"{ObjDir}ThingFiles.c.o" 			ƒ  	"{ComponentMgrDir}ThingFiles.c" 				∂
										"{ComponentMgrDir}ComponentMgr.make"			∂
										"{CIncludes}Files.h"							∂
										"{CIncludes}Resources.h"						∂
										"{CIncludes}Memory.h"							∂
										"{CIncludes}SysEqu.h"							∂
										"{CIncludes}TextUtils.h"						∂
										"{CIncludes}Errors.h"							∂
										"{CIncludes}Icons.h"							∂
										"{ComponentMgrDir}Components.h"					∂
										"{ComponentMgrDir}ComponentManagerSelectors.h"	∂
										"{ComponentMgrDir}PrivateThingManager.h"
	C {StdCOpts} "{ComponentMgrDir}ThingFiles.c" -o "{Targ}"

