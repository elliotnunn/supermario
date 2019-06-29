#
#	File:		ToolboxEventMgr.make
#
#	Contains:	Makefile for the Toolbox Event Manager.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#

ToolboxEventMgrObjs	=				"{ObjDir}ToolboxEventMgr.a.o"					∂
									"{ObjDir}ToolboxEventMgrPatches.a.o"


"{LibDir}ToolboxEventMgr.lib"			ƒ	{ToolboxEventMgrObjs}
	Lib {StdLibOpts} -o "{Targ}" {ToolboxEventMgrObjs}


"{ObjDir}ToolboxEventMgrPatches.a.o"	ƒ	"{ToolboxEventDir}ToolboxEventMgrPatches.a"
	Asm {StdAOpts} -o "{Targ}" "{ToolboxEventDir}ToolboxEventMgrPatches.a"


"{ObjDir}ToolboxEventMgr.a.o"			ƒ	"{ObjDir}StandardEqu.d"							∂
											"{AIncludes}SCSI.a"						∂
											"{AIncludes}HardwareEqu.a"				∂
											"{ToolboxEventDir}ToolboxEventMgr.a"
	Asm {StdAOpts} -o "{Targ}" "{ToolboxEventDir}ToolboxEventMgr.a"