#
#	File:		StandardFile.make
#
#	Contains:	Makefile for the Standard File Package.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc.  All rights reserved.
#
#	Change History (most recent first):
#
#	   <SM3>	11/30/92	SWC		Changed PackMacs.a->Packages.a.
#	   <SM2>	11/12/92	kc		Add -i "{IntCIncludes}".
#

"{RsrcDir}StandardFile.rsrc"		ƒ	"{StandardFileDir}StandardFile.r"			∂
										"{StandardFileDir}StandardFileTypes.r"		∂
										"{RIncludes}Types.r" 						∂
										"{RIncludes}BalloonTypes.r"					∂
										"{IntRIncludes}IconUtilsPrivTypes.r"		∂
										"{IntCIncludes}FinderPriv.h"				∂
										"{RsrcDir}StandardFilePACK.rsrc"			∂
										"{RsrcDir}StandardFileLDEF.rsrc"
	Rez {StdROpts} -o "{Targ}" "{StandardFileDir}StandardFile.r" -i "{IntCIncludes}"


"{RsrcDir}StandardFilePACK.rsrc"		ƒ	"{ObjDir}StandardFilePACK.a.o"
	Link {StdLOpts} {StdAlign} -rt PACK=3 -o "{Targ}" "{ObjDir}StandardFilePACK.a.o"


"{RsrcDir}StandardFileLDEF.rsrc"		ƒ	"{ObjDir}StandardFileLDEF.a.o"
	Link {StdLOpts} {StdAlign} -rt LDEF=-4000 -o "{Targ}" "{ObjDir}StandardFileLDEF.a.o"


"{ObjDir}StandardFileLDEF.a.o"		ƒ 	"{StandardFileDir}StandardFileLDEF.a"		∂
										"{ObjDir}StandardEqu.d"						∂
										"{AIncludes}Packages.a"						∂
										"{StandardFileDir}StandardFilePriv.a"
	Asm {StdAOpts} -o "{Targ}" "{StandardFileDir}StandardFileLDEF.a"


"{ObjDir}StandardFilePACK.a.o"		ƒ	"{StandardFileDir}StandardFilePACK.a"		∂
										"{ObjDir}StandardEqu.d"						∂
										"{AIncludes}Folders.a"						∂
										"{AIncludes}Aliases.a"						∂
										"{AIncludes}Balloons.a"						∂
										"{AIncludes}CTBUtilities.a"					∂
										"{AIncludes}Icons.a"						∂
										"{AIncludes}StandardFile.a"					∂
										"{IntAIncludes}LayerEqu.a"					∂
										"{IntAIncludes}IntlUtilsPriv.a"				∂
										"{IntAIncludes}IconUtilsPriv.a"				∂
										"{IntAIncludes}FinderPriv.a"				∂
										"{IntAIncludes}DialogsPriv.a"				∂
										"{StandardFileDir}StandardFilePriv.a"
	Asm {StdAOpts} -o "{Targ}" "{StandardFileDir}StandardFilePACK.a"
