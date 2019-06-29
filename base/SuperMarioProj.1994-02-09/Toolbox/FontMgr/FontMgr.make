#
#	File:		FontMgr.make
#
#	Contains:	Makefile for the Font Manager.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM3>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency.
#	   <SM2>	11/11/92	RB		Removed PrNonPortable, since it is set in StandarEqu.d

PartialDir			=	{FontMgrDir}PartialFont:
PartialIfDir		=	{PartialDir}Interface:
PartialPtDir		=	{PartialDir}Prototype:
FontLibDir			=	{FontMgrDir}Library:
FontLibIfDir		=	{FontLibDir}Interface:
FontLibPtDir		=	{FontLibDir}Prototype:
FontCachDir			=	{FontMgrDir}DiskCache:
FontCachIfDir		=	{FontCachDir}Interface:
FontCachPtDir		=	{FontCachDir}Prototype:

FntIncOpts			= 	-i "{FontLibIfDir}"		∂
						-i "{FontLibPtDir}"		∂
						-i "{PartialIfDir}"		∂
						-i "{PartialPtDir}"		∂
						-i "{FontCachIfDir}"	∂
						-i "{FontCachPtDir}"



FontMgrObjs							=	"{ObjDir}fnt.c.o"							∂
										"{ObjDir}FSglue.c.o"						∂
										"{ObjDir}embeddedBitmap.c.o"				∂
										"{ObjDir}OutlineMetrics.a.o"				∂
										"{ObjDir}BassExtensions.a.o"				∂
										"{ObjDir}sfnt.c.o"							∂
										"{ObjDir}Bass_Allocate.a.o"					∂
										"{ObjDir}MacExtra.c.o"						∂
										"{ObjDir}MapString.c.o"						∂
										"{ObjDir}FontMath.c.o"						∂
										"{ObjDir}FontMath.a.o"						∂
										"{ObjDir}FontScaler.c.o"					∂
										"{ObjDir}Bass_Cache.c.o"					∂
										"{ObjDir}DoubleByteBassCache.c.o"			∂
										"{ObjDir}DoubleByteSupport.a.o"				∂
										"{ObjDir}sc.c.o"							∂
										"{ObjDir}PartialFontExtensions.c.o"			∂
										"{ObjDir}FragmentTable.c.o"					∂
										"{ObjDir}Fragment.c.o"						∂
										"{ObjDir}Assertion.c.o"						∂
										"{ObjDir}ZoneHandle.c.o"					∂
										"{ObjDir}StringUtility.c.o"					∂
										"{ObjDir}AddDiskCache.c.o"					∂
										"{ObjDir}DiskCacheExtensions.c.o"			∂
										"{ObjDir}DiskCacheList.c.o"					∂
										"{ObjDir}DiskCacheMap.c.o"					∂
										"{ObjDir}FontFolderExtension.a.o" 			∂
										"{ObjDir}FontMgr.a.o" 						


"{LibDir}FontMgr.lib"				ƒ	{FontMgrObjs}
	Lib	{StdLibOpts} {FontMgrObjs} -o "{Targ}"


"{ObjDir}FontFolderExtension.a.o"	ƒ 	"{IntAIncludes}LinkedPatchMacros.a"			∂
										"{AIncludes}Traps.a"						∂
										"{AIncludes}ToolUtils.a"					∂
										"{AIncludes}Folders.a"						∂
										"{IntAIncludes}ResourceMgrPriv.a"			∂
										"{FontMgrDir}FontFolderExtension.a"
	Asm {StdAOpts} -o "{Targ}" "{FontMgrDir}FontFolderExtension.a"


"{ObjDir}BassExtensions.a.o" 		ƒ 	"{IntAIncludes}LinkedPatchMacros.a"			∂
										"{IntAIncludes}fontPrivate.a"				∂
										"{AIncludes}QuickDraw.a"					∂
										"{IntAIncludes}ColorEqu.a"					∂
										"{AIncludes}SysEqu.a"						∂
										"{IntAIncludes}SysPrivateEqu.a"				∂
										"{AIncludes}SysErr.a"						∂
										"{AIncludes}Traps.a"						∂
										"{AIncludes}ToolUtils.a"					∂
										"{AIncludes}GestaltEqu.a"					∂
										"{FontMgrDir}SplineDefines.a"				∂
										"{FontMgrDir}BassExtensions.a"
	Asm {StdAOpts} -o "{Targ}" "{FontMgrDir}BassExtensions.a" -d NEED_JSTDTXMEAS=0 


"{ObjDir}Bass_Allocate.a.o" 		ƒ 	"{FontMgrDir}SplineDefines.a"				∂
										"{IntAIncludes}fontPrivate.a"				∂
										"{ColorQDDir}qdHooks.a"						∂
										"{AIncludes}QuickDraw.a"					∂
										"{AIncludes}Traps.a"						∂
										"{AIncludes}SysEqu.a"						∂
										"{IntAIncludes}MFPrivate.a"					∂
										"{FontMgrDir}Bass_Allocate.a"
	Asm {StdAOpts} -o "{Targ}" "{FontMgrDir}Bass_Allocate.a" -i "{ColorQDDir}"


"{ObjDir}Bass_Cache.c.o"			ƒ 	"{CIncludes}Types.h"						∂
										"{CIncludes}Memory.h"						∂
										"{CIncludes}QuickDraw.h"					∂
										"{CIncludes}FixMath.h"						∂
										"{CIncludes}ToolUtils.h"					∂
										"{CIncludes}Resources.h"					∂
										"{CIncludes}Script.h"						∂
										"{CIncludes}ShutDown.h"						∂
										"{CIncludes}Fonts.h"						∂
										"{CIncludes}GestaltEqu.h"					∂
										"{IntCIncludes}MFPrivate.h"					∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}FontScaler.h"					∂
										"{FontMgrDir}embeddedBitmap.h"				∂
										"{FontMgrDir}MacExtra.h"					∂
										"{FontMgrDir}privateFontScaler.h"			∂
										"{FontMgrDir}Bass_Cache.h" 					∂
										"{FontMgrDir}SplineError.h"					∂
										"{FontMgrDir}Bass_Cache.c"					∂
										"{FontMgrDir}DoubleByteBassCache.h" 		∂
										"{FontMgrDir}Bass_Cache.c"					∂
										"{PartialPtDir}PartialFontExtensions.proto"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{FontMgrDir}Bass_Cache.c"


"{ObjDir}DoubleByteBassCache.c.o"	ƒ 	"{CIncludes}Fonts.h"						∂
										"{CIncludes}Memory.h"						∂
										"{CIncludes}Script.h"						∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}FontScaler.h"					∂
										"{FontMgrDir}Bass_Cache.h"					∂
										"{FontMgrDir}DoubleByteBassCache.h" 		∂
										"{FontMgrDir}DoubleByteBassCache.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}DoubleByteBassCache.c" -i "{PartialIfDir}"


"{ObjDir}DoubleByteSupport.a.o"		ƒ	"{AIncludes}Script.a"						∂
										"{AIncludes}SysEqu.a"						∂
										"{AIncludes}ToolUtils.a"					∂
										"{FontMgrDir}DoubleByteSupport.a"
	Asm {StdAOpts} -o "{Targ}" "{FontMgrDir}DoubleByteSupport.a" 


"{ObjDir}FontMgr.a.o" 				ƒ	"{FontMgrDir}SplineDefines.a" 				∂
										"{IntAIncludes}fontPrivate.a"				∂
										"{AIncludes}QuickDraw.a"					∂
										"{IntAIncludes}ColorEqu.a"					∂
										"{ColorQDDir}qdHooks.a"						∂
										"{AIncludes}ToolUtils.a"					∂
										"{AIncludes}SysEqu.a"						∂
										"{IntAIncludes}SysPrivateEqu.a"				∂
										"{AIncludes}Traps.a"						∂
										"{AIncludes}Script.a"						∂
										"{IntAIncludes}ScriptPriv.a"				∂
										"{IntAIncludes}ResourceMgrPriv.a"			∂
										"{FontMgrDir}FontMgr.a"						∂
										"{FontMgrDir}FontMgrShell.a"
	Asm {StdAOpts} -o "{Targ}" "{FontMgrDir}FontMgrShell.a" -i "{ColorQDDir}" -d NOT_68000=1 -d HAS_COLOR=1 -i "{FontMgrDir}"


"{ObjDir}FontScaler.c.o"			ƒ	"{FontMgrDir}FontScaler.h" 					∂
										"{FontMgrDir}privateSFNT.h"					∂
										"{Cincludes}FixMath.h"						∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}FontMath.h"					∂
										"{FontMgrDir}privateFontScaler.h"			∂
										"{FontMgrDir}sc.h"							∂
										"{FontMgrDir}fnt.h"							∂
										"{FontMgrDir}sfnt.h"						∂
										"{FontMgrDir}FSglue.h"						∂
										"{FontMgrDir}embeddedBitmap.h"				∂
										"{FontMgrDir}FSError.h"						∂
										"{FontMgrDir}FontScaler.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}FontScaler.c" 


"{ObjDir}fnt.c.o" 					ƒ	"{FontMgrDir}fnt.h"							∂
										"{Cincludes}FixMath.h"						∂
										"{FontMgrDir}FontMath.h"					∂
										"{Cincludes}setjmp.h"						∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}sc.h"							∂
										"{FontMgrDir}FSError.h"						∂
										"{FontMgrDir}fnt.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}fnt.c" -d MR_MAC


"{ObjDir}FSglue.c.o" 				ƒ 	"{FontMgrDir}sc.h"							∂
										"{FontMgrDir}privateSFNT.h"					∂
										"{CIncludes}ToolUtils.h"					∂
										"{Cincludes}FixMath.h"						∂
										"{FontMgrDir}FontMath.h"					∂
										"{FontMgrDir}fnt.h"							∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}sfnt.h"						∂
										"{FontMgrDir}FSglue.h"						∂
										"{FontMgrDir}FontScaler.h"					∂
										"{FontMgrDir}FSError.h"						∂
										"{FontMgrDir}FSglue.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}FSglue.c" 


"{ObjDir}embeddedBitmap.c.o" 		ƒ 	"{FontMgrDir}sc.h"							∂
										"{FontMgrDir}FontMath.h"					∂
										"{FontMgrDir}fnt.h"							∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}sfnt.h"						∂
										"{FontMgrDir}FSglue.h"						∂
										"{FontMgrDir}embeddedBitmap.h"				∂
										"{FontMgrDir}FontScaler.h"					∂
										"{FontMgrDir}FSError.h"						∂
										"{FontMgrDir}embeddedBitmap.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}embeddedBitmap.c" 


"{ObjDir}MacExtra.c.o" 				ƒ 	"{FontMgrDir}FontScaler.h" 					∂
										"{FontMgrDir}privateSFNT.h"					∂
										"{FontMgrDir}MacExtra.h"					∂
										"{FontMgrDir}MapString.h"					∂
										"{FontMgrDir}sc.h"							∂
										"{FontMgrDir}fnt.h"							∂
										"{FontMgrDir}sfnt.h"						∂
										"{FontMgrDir}FSglue.h"						∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}FontMath.h"					∂
										"{FontMgrDir}FSError.h"						∂
										"{FontMgrDir}Bass_Cache.h"					∂
										"{Cincludes}SetJmp.h"						∂
										"{Cincludes}FixMath.h"						∂
										"{Cincludes}ToolUtils.h"					∂
										"{Cincludes}Memory.h"						∂
										"{Cincludes}Fonts.h"						∂
										"{Cincludes}Script.h"						∂
										"{FontMgrDir}MacExtra.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}MacExtra.c" -i "{PartialIfDir}" -i "{PartialPtDir}"


"{ObjDir}MapString.c.o" 			ƒ 	"{FontMgrDir}MapString.h" 					∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}MapString.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}MapString.c" 


"{ObjDir}OutlineMetrics.a.o"		 ƒ 	"{AIncludes}SysEqu.a"						∂
										"{IntAIncludes}SysPrivateEqu.a"				∂
										"{AIncludes}Traps.a"						∂
										"{AIncludes}QuickDraw.a"					∂
										"{AIncludes}ToolUtils.a"					∂
										"{IntAIncludes}FontPrivate.a"				∂
										"{FontMgrDir}SplineDefines.a"				∂
										"{FontMgrDir}OutlineMetrics.a"
	Asm {StdAOpts} -o "{Targ}" "{FontMgrDir}OutlineMetrics.a" 


"{ObjDir}sc.c.o" 					ƒ 	"{FontMgrDir}sc.h"							∂
										"{FontMgrDir}FSError.h"						∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}sc.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}sc.c" 


"{ObjDir}sfnt.c.o" 					ƒ	"{FontMgrDir}sfnt.h"						∂
										"{FontMgrDir}privateSFNT.h"					∂
										"{Cincludes}FixMath.h"						∂
										"{FontMgrDir}FontMath.h"					∂
										"{FontMgrDir}sfnt_enum.h"					∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}FontScaler.h"					∂
										"{FontMgrDir}FSError.h"						∂
										"{FontMgrDir}FSglue.h"						∂
										"{FontMgrDir}embeddedBitmap.h"				∂
										"{FontMgrDir}fnt.h"							∂
										"{FontMgrDir}sc.h"							∂
										"{FontMgrDir}MapString.h"					∂
										"{FontMgrDir}sfnt.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}sfnt.c" 


"{ObjDir}FontMath.c.o" 				ƒ	"{FontMgrDir}FontMath.h"					∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}FontMath.c"
	C {StdCOpts} -o "{Targ}" "{FontMgrDir}FontMath.c" 


"{ObjDir}FontMath.a.o" 				ƒ 	"{FontMgrDir}FontMath.a"
	Asm {StdAOpts} -o "{Targ}" {FntIncOpts} "{FontMgrDir}FontMath.a"


"{ObjDir}Assertion.c.o"				ƒ	"{FontLibDir}Source:Assertion.c"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{FontLibDir}Source:Assertion.c"


"{ObjDir}ZoneHandle.c.o"			ƒ	"{FontLibDir}Source:ZoneHandle.c"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{FontLibDir}Source:ZoneHandle.c"


"{ObjDir}StringUtility.c.o"			ƒ	"{FontLibDir}Source:StringUtility.c"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{FontLibDir}Source:StringUtility.c"


"{ObjDir}Fragment.c.o" 				ƒ 	"{PartialDir}Source:Fragment.c"				∂
										"{CIncludes}Memory.h"						∂
										"{CIncludes}Resources.h"					∂
										"{FontLibIfDir}Assertion.h"					∂
										"{FontLibIfDir}BaseIncludes.h"				∂
										"{FontLibIfDir}ZoneHandle.h"				∂
										"{FontLibPtDir}ZoneHandle.proto"			∂
										"{PartialIfDir}Fragment.h"					∂
										"{PartialPtDir}Fragment.proto"				∂
										"{PartialIfDir}PartialFontExtensions.h"		∂
										"{PartialPtDir}PartialFontExtensions.proto"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{PartialDir}Source:Fragment.c"


"{ObjDir}FragmentTable.c.o" 		ƒ 	"{PartialDir}Source:FragmentTable.c"		∂
										"{CIncludes}Memory.h"						∂
										"{CIncludes}Types.h"						∂
										"{FontLibIfDir}Assertion.h"					∂
										"{FontLibIfDir}BaseIncludes.h"				∂
										"{FontLibIfDir}ZoneHandle.h"				∂
										"{FontLibPtDir}ZoneHandle.proto"			∂
										"{PartialIfDir}FragmentTable.h"				∂
										"{PartialPtDir}FragmentTable.proto"			∂
										"{PartialIfDir}PartialFontExtensions.h"		∂
										"{PartialPtDir}PartialFontExtensions.proto"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{PartialDir}Source:FragmentTable.c" 


"{ObjDir}PartialFontExtensions.c.o"	ƒ 	"{PartialDir}Source:PartialFontExtensions.c" ∂
										"{CIncludes}Memory.h"						∂
										"{FontMgrDir}FSCdefs.h"						∂
										"{FontMgrDir}MapString.h"					∂
										"{FontMgrDir}sfnt.h"						∂
										"{FontMgrDir}sfnt_enum.h"					∂
										"{FontMgrDir}FontScaler.h"					∂
										"{FontMgrDir}Bass_Cache.h"					∂
										"{FontLibIfDir}Assertion.h"					∂
										"{FontLibIfDir}BaseIncludes.h"				∂
										"{FontLibIfDir}ZoneHandle.h"				∂
										"{FontLibPtDir}ZoneHandle.proto"			∂
										"{PartialIfDir}Fragment.h"					∂
										"{PartialPtDir}Fragment.proto"				∂
										"{PartialIfDir}FragmentTable.h"				∂
										"{PartialPtDir}FragmentTable.proto"			∂
										"{PartialIfDir}PartialFontExtensions.h"		∂
										"{PartialPtDir}PartialFontExtensions.proto"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{PartialDir}Source:PartialFontExtensions.c" -i "{FontMgrDir}"


"{ObjDir}AddDiskCache.c.o" 			ƒ 	"{FontCachDir}Source:AddDiskCache.c"		∂
										"{FontMgrDir}Bass_Cache.h"					∂
										"{FontCachIfDir}DiskCacheExtensions.h"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{FontCachDir}Source:AddDiskCache.c" -i "{FontMgrDir}"


"{ObjDir}DiskCacheExtensions.c.o" 	ƒ 	"{FontCachDir}Source:DiskCacheExtensions.c"	∂
										"{FontMgrDir}Bass_Cache.h"					∂
										"{FontCachIfDir}DiskCacheExtensions.h"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{FontCachDir}Source:DiskCacheExtensions.c" -i "{FontMgrDir}"


"{ObjDir}DiskCacheList.c.o" 		ƒ 	"{FontCachDir}Source:DiskCacheList.c"		∂
										"{FontCachIfDir}DiskCacheExtensions.h"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{FontCachDir}Source:DiskCacheList.c"


"{ObjDir}DiskCacheMap.c.o" 			ƒ 	"{FontCachDir}Source:DiskCacheMap.c"		∂
										"{FontMgrDir}Bass_Cache.h"					∂
										"{FontCachIfDir}DiskCacheExtensions.h"
	C {StdCOpts} -o "{Targ}" {FntIncOpts} "{FontCachDir}Source:DiskCacheMap.c" -i "{FontMgrDir}"



