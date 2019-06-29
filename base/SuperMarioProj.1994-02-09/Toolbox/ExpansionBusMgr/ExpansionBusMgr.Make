#
#	File:		ExpansionBusMgr.make
#
#	Contains:	Expansion Bus Manager Make file
#
#	Written by:	Al Kossow
#
#	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#		 <1>	11/11/93	fau		first checked in

ExpansionBusMgrLibDir = {LibDir}

ExpansionBusObjs = 	"{ObjDir}ExpansionBusMgr.a.o" ∂
					"{ObjDir}ExpansionBusMgr.c.o" ∂
			 		"{ObjDir}ExpansionIntrs.c.o" ∂
			 		"{ObjDir}BusMgmt.c.o" ∂
			 		"{ObjDir}DeviceTree.c.o" ∂
			 		"{ObjDir}NuBus_BusMgr.c.o" ∂
			 		"{ObjDir}PCI_BusMgr.c.o"

ExpansionBusMgrCOpts =	-b2 {StdCOpts} -i {ExpansionBusMgrDir}
ExpansionBusMgrAOpts =	{StdAOpts}

"{ExpansionBusMgrLibDir}ExpansionBusMgr.lib" 	ƒ	{ExpansionBusObjs}
	Lib {LibWarnings} -o {Targ} {ExpansionBusObjs}

"{ObjDir}ExpansionBusMgr.a.o" ƒ		"{ObjDir}StandardEqu.d"					∂
									"{ExpansionBusMgrDir}ExpansionBusMgr.a"
	Asm -o {Targ} "{ExpansionBusMgrDir}ExpansionBusMgr.a" {ExpansionBusMgrAOpts}

"{ObjDir}ExpansionBusMgr.c.o" ƒ		"{ExpansionBusMgrDir}ExpansionMgrInternal.h" ∂
									"{ExpansionBusMgrDir}ExpansionBusMgr.c"
	C "{ExpansionBusMgrDir}ExpansionBusMgr.c" {ExpansionBusMgrCOpts} -o {Targ}

"{ObjDir}ExpansionIntrs.c.o" ƒ	"{ExpansionBusMgrDir}ExpansionMgrInternal.h" ∂
									"{ExpansionBusMgrDir}ExpansionIntrs.c"
	C "{ExpansionBusMgrDir}ExpansionIntrs.c" {ExpansionBusMgrCOpts} -o {Targ}

"{ObjDir}BusMgmt.c.o"	ƒ			"{ExpansionBusMgrDir}ExpansionMgrInternal.h" ∂
									"{ExpansionBusMgrDir}BusMgmt.c"
	C "{ExpansionBusMgrDir}BusMgmt.c" {ExpansionBusMgrCOpts} -o {Targ}

"{ObjDir}DeviceTree.c.o"	ƒ		"{ExpansionBusMgrDir}ExpansionMgrInternal.h" ∂
									"{ExpansionBusMgrDir}DeviceTree.c"
	C "{ExpansionBusMgrDir}DeviceTree.c" {ExpansionBusMgrCOpts} -o {Targ}

"{ObjDir}NuBus_BusMgr.c.o"	ƒ		"{ExpansionBusMgrDir}ExpansionMgrInternal.h" ∂
									"{ExpansionBusMgrDir}NuBus_BusMgr.c"
	C "{ExpansionBusMgrDir}NuBus_BusMgr.c" {ExpansionBusMgrCOpts} -o {Targ}

"{ObjDir}PCI_BusMgr.c.o"	ƒ		"{ExpansionBusMgrDir}ExpansionMgrInternal.h" ∂
									"{ExpansionBusMgrDir}PCI_BusMgr.c"
	C "{ExpansionBusMgrDir}PCI_BusMgr.c" {ExpansionBusMgrCOpts} -o {Targ}
