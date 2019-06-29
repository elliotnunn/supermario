#
#	File:		InSane.make
#
#	Contains:	Makefile for InSANE.
#
#	Written by:	Kurt Clark, Chas Spillar, and Tim Nichols
#
#	Copyright:	© 1992-1993 by Apple Computer, Inc., all rights reserved.
#
#	Change History (most recent first):
#
#	   <SM2>	  3/8/93	PN		From the conversation with Jon . We should use InSane for NonFPU
#									pack 4 and 5 from now on. Add make instruction to make
#									InSaneNFPU.rsrc for pack 4 & 5
#	   <SM2>	 12/2/92	kc		Added " || Exit 1" to commands with a double dependency.


"{RsrcDir}InSane.rsrc"				ƒƒ	"{ObjDir}ElemsHW.a.o"
	Link {StdLOpts} {StdAlign} -rt PACK=5 -o "{Targ}" "{ObjDir}ElemsHW.a.o" || Exit 1


"{RsrcDir}InSane.rsrc"				ƒƒ	"{ObjDir}FPHW.a.o"
	Link {StdLOpts} {StdAlign} -rt PACK=4 -o "{Targ}" "{ObjDir}FPHW.a.o" || Exit 1

"{RsrcDir}InSaneNFPU.rsrc"			ƒƒ	"{ObjDir}ELEMS020.a.o"	
	Link {StdLOpts} {StdAlign} -o "{Targ}" -rt PACK=5 "{ObjDir}ELEMS020.a.o" || Exit 1


"{RsrcDir}InSaneNFPU.rsrc"			ƒƒ	"{ObjDir}FP020.a.o"
	Link {StdLOpts} {StdAlign} -o "{Targ}" -rt PACK=4 "{ObjDir}FP020.a.o" || Exit 1


"{ObjDir}FPPrivTrap.a.o"			ƒ	"{ObjDir}StandardEqu.d"						∂
										"{IntAIncludes}UniversalEqu.a"				∂
										"{IntAIncludes}HardwarePrivateEqu.a"		∂
										"{InSaneDir}FPPrivTrap.a"
	Asm {StdAOpts} -o "{Targ}" "{InSaneDir}FPPrivTrap.a"


"{ObjDir}ElemsHW.a.o"				ƒ	"{InSaneDir}HWElemsEqus.a"					∂
										"{InSaneDir}HWElemsControl.a"				∂
										"{InSaneDir}HWElemsExp.a"					∂
										"{InSaneDir}HWElemsFinancial.a"				∂
										"{InSaneDir}HWElemsCommon.a"				∂
										"{InSaneDir}HWElemsLogs.a"					∂
										"{InSaneDir}HWElemsTg.a"					∂
										"{InSaneDir}HWElemsArcTg.a"					∂
										"{InSaneDir}HWElemsSinCos.a"				∂
										"{InSaneDir}HWElemsCoeffs.a"					
	Asm {StdAOpts} -d BackPatch=1 -o "{Targ}" "{InSaneDir}HWElemsControl.a"


"{RsrcDir}FPHW.rsrc"				ƒ	"{InSaneDir}FPHW.a"							∂
										"{InSaneDir}FPHWEqus.a"						∂
										"{AIncludes}ToolUtils.a"					∂
										"{AIncludes}TRAPS.a"						∂
										"{AIncludes}SANEMACS.a"						∂
										"{AIncludes}SANEMacs881.a"					∂
										"{InSaneDir}FPHWCtrl.a"						∂
										"{InSaneDir}FPHWArith.a"					∂
										"{InSaneDir}FPHWArith96.a"					∂
										"{InSaneDir}FPHWNonArith.a"					∂
										"{InSaneDir}FPHWB2DC.a"						
	Asm {StdAOpts} -d BackPatch=1 -o "{Targ}" "{InSaneDir}FPHW.a"


"{ObjDir}FPHW.a.o"					ƒ	"{InSaneDir}FPHW.a"
	Asm {StdAOpts} -d BackPatch=1 -o "{Targ}" "{InSaneDir}FPHW.a"
	
"{ObjDir}ELEMS020.a.o"			ƒ	"{ObjDir}StandardEqu.d"							∂
									"{AIncludes}SANEMACS.a"							∂
									"{InSaneDir}ELEMS020_1.a"						∂
									"{InSaneDir}ELEMS020_2.a"						∂
									"{InSaneDir}ELEMS020_3.a"						∂
									"{InSaneDir}ELEMS020_C.a"						∂
									"{InSaneDir}ELEMS020.a"
	Asm {StdAOpts} -o "{Targ}" "{InSaneDir}ELEMS020.a"


"{ObjDir}FP020.a.o"				ƒ	"{ObjDir}StandardEqu.d"							∂
									"{InSaneDir}FPEQUS.a"							∂
									"{AIncludes}SANEMACS.a"							∂
									"{InSaneDir}FPCTRL.a"							∂
									"{InSaneDir}FPOPS.a"							∂
									"{InSaneDir}FPBD.a"								∂
									"{InSaneDir}FP020.a"
	Asm {StdAOpts} -o "{Targ}" "{InSaneDir}FP020.a"
