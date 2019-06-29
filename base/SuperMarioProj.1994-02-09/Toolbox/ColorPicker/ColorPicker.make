#
#	File:		ColorPicker.make
#
#	Contains:	Makefile for the Color Picker Package.
#
#	Written by:	Kurt Clark, Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM2>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency. 
#


ColorPickerObjs					=	"{ObjDir}ColorPicker.a.o"						∂
									"{ObjDir}ColorPicker.p.o" 						∂
									"{ObjDir}ColorConvert.p.o"						∂
									"{ObjDir}ColorGlue.p.o"							∂
									"{IfObjDir}Interface.o"							∂
									"{PLibraries}PasLib.o"


"{RsrcDir}ColorPicker.rsrc"		ƒƒ	"{RIncludes}Types.r"							∂
									"{ColorPickerDir}ColorPicker.r"					∂
									"{ColorPickerDir}ColorPickerWedge.r"
	Rez -a {StdROpts} -o "{Targ}" "{ColorPickerDir}ColorPicker.r" "{ColorPickerDir}ColorPickerWedge.r" || Exit 1


"{RsrcDir}ColorPicker.rsrc"		ƒƒ	{ColorPickerObjs}
	Link {StdLOpts} {StdAlign} -o "{Targ}" {ColorPickerObjs} -rt PACK=12 || Exit 1


# Since these guys use the obsolete MemTypes, QuickDraw, OSIntf, ToolIntf, PackIntf
# they depend on a *huge* number of .p files.

MQOTPUnits						=	"{PInterfaces}MemTypes.p"						∂
									"{PInterfaces}Types.p"							∂
									"{PInterfaces}QuickDraw.p"						∂
									"{PInterfaces}OSIntf.p"							∂
									"{PInterfaces}Events.p"							∂
									"{PInterfaces}OSUtils.p"						∂
									"{PInterfaces}Files.p"							∂
									"{PInterfaces}Devices.p"						∂
									"{PInterfaces}DeskBus.p"						∂
									"{PInterfaces}DiskInit.p"						∂
									"{PInterfaces}Disks.p"							∂
									"{PInterfaces}Errors.p"							∂
									"{PInterfaces}Memory.p"							∂
									"{PInterfaces}OSEvents.p"						∂
									"{PInterfaces}Retrace.p"						∂
									"{PInterfaces}SegLoad.p"						∂
									"{PInterfaces}Serial.p"							∂
									"{PInterfaces}ShutDown.p"						∂
									"{PInterfaces}Slots.p"							∂
									"{PInterfaces}Sound.p"							∂
									"{PInterfaces}Start.p"							∂
									"{PInterfaces}Timer.p"							∂
									"{PInterfaces}ToolIntf.p"						∂
									"{PInterfaces}Controls.p"						∂
									"{PInterfaces}Desk.p"							∂
									"{PInterfaces}Windows.p"						∂
									"{PInterfaces}TextEdit.p"						∂
									"{PInterfaces}Dialogs.p"						∂
									"{PInterfaces}Fonts.p"							∂
									"{PInterfaces}Lists.p"							∂
									"{PInterfaces}Menus.p"							∂
									"{PInterfaces}Resources.p"						∂
									"{PInterfaces}Scrap.p"							∂
									"{PInterfaces}ToolUtils.p"						∂
									"{PInterfaces}PackIntf.p"						∂
									"{PInterfaces}Packages.p"

ColorConvertUnits				=	{MQOTPUnits}									∂
									"{PInterfaces}PaletteMgr.p"						∂
									"{PInterfaces}FixMath.p"						∂
									"{ColorPickerDir}ColorConvert.p"

ColorPickerUnits				=	{ColorConvertUnits}								∂
									"{PInterfaces}QDOffScreen.p"					∂
									"{PInterfaces}SysEqu.p"							∂
									"{PInterfaces}GestaltEqu.p"						∂
									"{ColorPickerDir}ColorPicker.p"

"{ObjDir}ColorConvert.p.o"		ƒ 	{ColorConvertUnits}
	Pascal {StdPOpts} -o "{targ}" "{ColorPickerDir}ColorConvert.p"   


"{ObjDir}ColorPicker.a.o"		ƒ 	"{AIncludes}Traps.a"							∂
									"{ColorPickerDir}ColorPicker.a"
	Asm {StdAOpts} -o "{Targ}" "{ColorPickerDir}ColorPicker.a" 


"{ObjDir}ColorPicker.p.o"		ƒ 	{ColorPickerUnits}
	Pascal {StdPOpts} -o "{targ}" "{ColorPickerDir}ColorPicker.p"   


"{ObjDir}ColorGlue.p.o"			ƒ 	{ColorPickerUnits}								∂
									"{ColorPickerDir}ColorGlue.p"
	Pascal {StdPOpts} -o "{targ}" "{ColorPickerDir}ColorGlue.p"   
