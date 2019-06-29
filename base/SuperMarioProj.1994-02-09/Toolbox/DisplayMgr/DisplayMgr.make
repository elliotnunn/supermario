#
#	File:		DisplayMgr.make
#
#	Contains:	Display Manager Make file
#
#	Written by:	Ian Hendry
#
#	Copyright:	© 1993 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	  <SM10>	10/10/93	RC		Fixed a directory problem with this make file
#	   <SM9>	10/10/93	IH		Sync with Excelsior.
#		 <8>	 9/10/93	IH		Add DisplayManager.c dependency.
#	   <SM7>	08-17-93	jmp		Oops… in SuperMario we have “ObjDir”s not “Objects”.
#	   <SM6>	08-17-93	jmp		Updated a few of the dependency lists.
#		 <5>	  8/4/93	IH		#1101633,1098397,1101795,1101636,1102280 <KON>: Fix checkin 4 to
#									really have what it says it has (I checked it in by accident
#									before I finished making the changes). Add dependencies for
#									DisplayMgrCompatibility.c.
#		 <4>	  8/4/93	IH		#1101633,1098397,1101795,1101636 <KON>: Add dependencies for
#									DisplaysPriv.h and sync to SysObj.make.
#		 <3>	  6/1/93	IH		Add new dependencies.
#		 <2>	  4/8/93	IH		Add DisplayMgrUtils.c to build, also added some more .h
#									dependencies.
#		 <1>	  4/1/93	IH		first checked in
#
#


#
# When converting from Reality:
# remove all "Compiling" lines
# remove all " ; LogError" commands
#
ProcessMgrDir = {Sources}ProcessMgr:
DispalyMgrLibDir = {LibDir}

DisplayObjs =	"{ObjDir}DisplayMgr.a.o" ∂
				"{ObjDir}DisplayMgr.c.o" ∂
				"{ObjDir}DisplayMgrCompatibility.c.o" ∂
				"{ObjDir}DisplayMgrUtils.c.o" ∂
				"{ObjDir}DisplayMgrOverlap.c.o"

DisplayExtensionObjs = 	"{ObjDir}DisplayMgr.a.o" ∂
						"{ObjDir}DisplayMgr.c.o" ∂
						"{ObjDir}DisplayMgrCompatibility.c.o" ∂
						"{ObjDir}DisplayMgrUtils.c.o" ∂
						"{ObjDir}DeviceLoop.a.o"

DisplayMgrCOpts =	-b2 {StdCOpts} -i {DisplayMgrDir} -i {ProcessMgrDir}
DisplayMgrAOpts =	{StdAOpts}


"{DispalyMgrLibDir}DisplayMgr.lib" 	ƒ		{DisplayObjs}
	Lib {LibWarnings} -o {Targ} {DisplayObjs}


"{DispalyMgrLibDir}DisplayMgrExtensionLib.o" 	ƒ		{DisplayExtensionObjs}
	Lib {LibWarnings} -o {Targ} {DisplayExtensionObjs}


"{IntCIncludes}DisplaysPriv.h" ƒ ∂
								"{CIncludes}Components.h"				∂
								"{CIncludes}Displays.h"					∂
								"{CIncludes}Events.h"					∂
								"{IntCIncludes}Layers.h"				∂
								"{CIncludes}Processes.h"				∂
								"{CIncludes}Video.h"					∂
								"{CIncludes}Windows.h"					∂
								"{DisplayMgrDir}DisplayComponents.h"
	Setfile -m . {Targ}


"{ObjDir}DisplayMgr.a.o" ƒ		"{ObjDir}StandardEqu.d"					∂
								"{IntAIncludes}InternalMacros.a"		∂
								"{AIncludes}GestaltEqu.a"				∂
								"{AIncludes}Video.a"					∂
								"{IntAIncludes}LinkedPatchMacros.a"		∂
								"{AIncludes}Displays.a"					∂
								"{IntAIncludes}DisplaysPriv.a"			∂
								"{IntAIncludes}MenuMgrPriv.a"			∂
								"{IntAIncludes}HardwarePrivateEqu.a"	∂
								"{IntAIncludes}DepVideoEqu.a" 			∂
								"{IntAIncludes}TrapsPrivate.a" 			∂
								"{DisplayMgrDir}DisplayMgr.a"
	Asm -o {Targ} "{DisplayMgrDir}DisplayMgr.a" {DisplayMgrAOpts}


"{ObjDir}DisplayMgrUtils.c.o" ƒ	"{CIncludes}AppleEvents.h"				∂
								"{CIncludes}GestaltEqu.h"				∂
								"{CIncludes}Menus.h"					∂
								"{CIncludes}Processes.h"				∂
								"{CIncludes}QDOffscreen.h"				∂
								"{CIncludes}Resources.h"				∂
								"{CIncludes}Retrace.h"					∂
								"{CIncludes}ToolUtils.h"				∂
								"{CIncludes}SysEqu.h"					∂
								"{CIncludes}Errors.h"					∂
								"{CIncludes}Palettes.h"					∂
								"{CIncludes}Slots.h"					∂
								"{CIncludes}Video.h"					∂
								"{CIncludes}ROMDefs.h"					∂
								"{CIncludes}Devices.h"					∂
								"{IntCIncludes}MenuMgrPriv.h"			∂
								"{IntCIncludes}PalettesPriv.h"			∂
								"{IntCIncludes}ProcessesPriv.h"			∂
								"{ProcessMgrDir}Data.h"					∂
								"{CIncludes}Displays.h"					∂
								"{IntCIncludes}DisplaysPriv.h"			∂
								"{IntCIncludes}ExpandMemPriv.h"			∂
								"{DisplayMgrDir}DisplayMgrUtils.c"
	C "{DisplayMgrDir}DisplayMgrUtils.c" {DisplayMgrCOpts} -o {Targ}


"{ObjDir}DisplayMgrCompatibility.c.o" ƒ ∂
								"{CIncludes}GestaltEqu.h"				∂
								"{CIncludes}Notification.h"				∂
								"{CIncludes}QDOffscreen.h"				∂
								"{IntCIncludes}DisplaysPriv.h"			∂
								"{DisplayMgrDir}DisplayMgrCompatibility.c"
	C "{DisplayMgrDir}DisplayMgrCompatibility.c" {DisplayMgrCOpts} -o {Targ}

"{ObjDir}DisplayMgrOverlap.c.o" ƒ ∂
								"{CIncludes}Types.h"					∂
								"{CIncludes}GestaltEqu.h"				∂
								"{IntCIncludes}DisplaysPriv.h"			∂
								"{IntCIncludes}ExpandMemPriv.h"			∂
								"{DisplayMgrDir}DisplayMgrOverlap.c"
	C "{DisplayMgrDir}DisplayMgrOverlap.c" {DisplayMgrCOpts} -o {Targ}

"{ObjDir}DisplayMgr.c.o" ƒ		"{CIncludes}AppleEvents.h"				∂
								"{CIncludes}GestaltEqu.h"				∂
								"{CIncludes}Menus.h"					∂
								"{CIncludes}Processes.h"				∂
								"{CIncludes}QDOffscreen.h"				∂
								"{CIncludes}Resources.h"				∂
								"{CIncludes}Retrace.h"					∂
								"{CIncludes}ToolUtils.h"				∂
								"{CIncludes}SysEqu.h"					∂
								"{CIncludes}Errors.h"					∂
								"{CIncludes}Palettes.h"					∂
								"{CIncludes}Devices.h"					∂
								"{IntCIncludes}MenuMgrPriv.h"			∂
								"{IntCIncludes}PalettesPriv.h"			∂
								"{IntCIncludes}ProcessesPriv.h"			∂
								"{ProcessMgrDir}Data.h"					∂
								"{CIncludes}Displays.h"					∂
								"{IntCIncludes}DisplaysPriv.h"			∂
								"{IntCIncludes}ExpandMemPriv.h"			∂
								"{IntCIncludes}CrsrDevPriv.h"			∂
								"{DisplayMgrDir}DisplayMgr.c"
	C "{DisplayMgrDir}DisplayMgr.c" {DisplayMgrCOpts} -o {Targ}


