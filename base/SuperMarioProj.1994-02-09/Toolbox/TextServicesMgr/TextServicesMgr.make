#
#	File:		TextServicesMgr.make
#
#	Contains:	Makefile for the Text Services Manager.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#


TextServicesMgrObjs	=				"{ObjDir}TSMExtension.a.o"					∂
									"{ObjDir}TSMFunctions.c.o"					∂
									"{ObjDir}TSMUtility.c.o"					∂
									"{ObjDir}TSMInternalUtils.c.o"


"{LibDir}TextServicesMgr.lib" 	ƒ	{TextServicesMgrObjs}
	Lib {StdLibOpts} -o "{Targ}"	{TextServicesMgrObjs}


"{ObjDir}TSMExtension.a.o"	ƒ	"{ObjDir}StandardEqu.d"							∂
								"{AIncludes}TextServices.a"						∂
								"{IntAIncludes}TSMPrivate.a"					∂
								"{TextServicesDir}TSMExtension.a"
	Asm {StdAOpts} -o "{Targ}" "{TextServicesDir}TSMExtension.a"


{ObjDir}TSMFunctions.c.o	ƒ	"{TextServicesDir}TSMFunctions.c"				∂
								"{CIncludes}Components.h"						∂
								"{IntCIncludes}TSMPrivate.h"					∂
								"{CIncludes}TextServices.h"
	C {StdCOpts} -o "{Targ}" "{TextServicesDir}TSMFunctions.c"


"{ObjDir}TSMUtility.c.o"	ƒ	"{TextServicesDir}TSMUtility.c"					∂
								"{CIncludes}Script.h"							∂
								"{CIncludes}Components.h"						∂
								"{IntCIncludes}TSMPrivate.h"					∂
								"{CIncludes}TextServices.h"
	C {StdCOpts} -o "{Targ}" "{TextServicesDir}TSMUtility.c"


"{ObjDir}TSMInternalUtils.c.o"	ƒ	"{TextServicesDir}TSMInternalUtils.c"		∂
									"{CIncludes}Components.h"					∂
									"{IntCIncludes}TSMPrivate.h"				∂
									"{CIncludes}TextServices.h"
	C {StdCOpts} -o "{Targ}" "{TextServicesDir}TSMInternalUtils.c"

