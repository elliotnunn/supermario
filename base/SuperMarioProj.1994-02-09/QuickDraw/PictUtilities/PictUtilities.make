#
#	File:		pu.make
#
#	Contains:	Makefile for the Picture Utilities Library.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#

PictUtilObjs					=	"{ObjDir}puDispatch.a.o"						∂
									"{ObjDir}puLowLevel.a.o"						∂
									"{ObjDir}puLowLevel.c.o" 						∂
									"{ObjDir}puPictures.c.o"						∂
									"{ObjDir}puPixMaps.c.o"							∂
									"{ObjDir}puPopular.a.o"							∂
									"{ObjDir}puPopular.c.o"							∂
									"{ObjDir}puMedian.a.o"							∂
									"{ObjDir}puMedian.c.o"							∂
									"{IfObjDir}interface.o"							


"{RsrcDir}PictUtilities.rsrc" ƒ	{PictUtilObjs}
	Link {StdLOpts} {StdAlign} -m PictUtilEntry -rt PACK=15 -o "{Targ}" {PictUtilObjs}


"{LibDir}PictUtilities.lib"	ƒ	"{ObjDir}puPackEntry.a.o"
	Lib	{StdLibOpts} -o "{Targ}" "{ObjDir}puPackEntry.a.o"


"{ObjDir}puDispatch.a.o"	ƒ "{PictUtilsDir}puDispatch.a"
	Asm {StdAOpts} -o "{Targ}" "{PictUtilsDir}puDispatch.a"


"{ObjDir}puLowLevel.a.o"	ƒ "{PictUtilsDir}puLowLevel.a"
	Asm {StdAOpts} -o "{Targ}" "{PictUtilsDir}puLowLevel.a"


"{ObjDir}puLowLevel.c.o" 	ƒ	"{PictUtilsDir}puPrivate.h"				∂
								"{PictUtilsDir}puLowLevel.c"
	C {StdCOpts} -o "{Targ}" "{PictUtilsDir}puLowLevel.c"


"{ObjDir}puPictures.c.o" 	ƒ	"{PictUtilsDir}puPrivate.h"				∂
								"{PictUtilsDir}puPictures.c"
	C {StdCOpts} -o "{Targ}" "{PictUtilsDir}puPictures.c"


"{ObjDir}puPixMaps.c.o" 	ƒ	"{PictUtilsDir}puPrivate.h"				∂
								"{PictUtilsDir}puPixMaps.c"
	C {StdCOpts} -o "{Targ}" "{PictUtilsDir}puPixMaps.c"


"{ObjDir}puPopular.a.o"		ƒ "{PictUtilsDir}puPopular.a"
	Asm {StdAOpts} -o "{Targ}" "{PictUtilsDir}puPopular.a"


"{ObjDir}puPopular.c.o" 	ƒ	"{PictUtilsDir}puPrivate.h"				∂
								"{PictUtilsDir}puPopular.c"
	C {StdCOpts} -o "{Targ}" "{PictUtilsDir}puPopular.c"


"{ObjDir}puMedian.a.o"		ƒ "{PictUtilsDir}puMedian.a"
	Asm {StdAOpts} -o "{Targ}" "{PictUtilsDir}puMedian.a"


"{ObjDir}puMedian.c.o" 		ƒ	"{PictUtilsDir}puPrivate.h"				∂
								"{PictUtilsDir}puMedian.c"
	C {StdCOpts} -o "{Targ}" "{PictUtilsDir}puMedian.c"


"{ObjDir}puPackEntry.a.o"	ƒ 	"{ObjDir}StandardEqu.d"					∂
								"{IntAIncludes}DispatchHelperPriv.a"	∂
								"{PictUtilsDir}puPackEntry.a"
	Asm {StdAOpts} -o "{Targ}" "{PictUtilsDir}puPackEntry.a"
