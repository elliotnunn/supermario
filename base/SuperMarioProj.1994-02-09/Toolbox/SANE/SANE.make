#
#	File:		Sane.make
#
#	Contains:	Makefile for SANE.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992-1993 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM2>	  3/8/93	PN		From the conversation with Jon Okada. We should use InSane for
#									NonFPU pack 4 and 5 from now on. Delete pack4 and 5
#	   <SM2>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency.


"{RsrcDir}Sane.rsrc"			ƒƒ	"{ObjDir}BinDec.a.o"
	Link {StdLOpts} {StdAlign} -o "{Targ}" -rt PACK=7 "{ObjDir}BinDec.a.o" || Exit 1



"{ObjDir}BinDec.a.o"			ƒ	"{ObjDir}StandardEqu.d"							∂
									"{AIncludes}SANEMacs.a"							∂
									"{SANEDir}Str2Dec.a"							∂
									"{SANEDir}Dec2Str.a"							∂
									"{SANEDir}BinDec.a"
	Asm {StdAOpts} -o "{Targ}" "{SANEDir}BinDec.a"


