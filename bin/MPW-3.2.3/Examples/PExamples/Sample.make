#
#	Apple Macintosh Developer Technical Support
#
#	MultiFinder-Aware Simple Sample Application
#
#	Sample
#
#	[P]Sample.make	-	Make Source
#
#	Copyright © Apple Computer, Inc. 1989-1990
#	All rights reserved.
#
#	Versions:	
#				1.00				08/88
#				1.01				11/88
#				1.02				04/89	MPW 3.1
#				1.03				02/90	MPW 3.2
#
#	Components:
#				Sample.p			Feb.  1, 1990
#				Sample.r			Feb.  1, 1990
#				Sample.h			Feb.  1, 1990
#				Sample.make			Feb.  1, 1990
#
#	Sample is an example application that demonstrates how to
#	initialize the commonly used toolbox managers, operate 
#	successfully under MultiFinder, handle desk accessories, 
#	and create, grow, and zoom windows.
#
#	It does not by any means demonstrate all the techniques 
#	you need for a large application. In particular, Sample 
#	does not cover exception handling, multiple windows/documents, 
#	sophisticated memory management, printing, or undo. All of 
#	these are vital parts of a normal full-sized application.
#
#	This application is an example of the form of a Macintosh 
#	application; it is NOT a template. It is NOT intended to be 
#	used as a foundation for the next world-class, best-selling, 
#	600K application. A stick figure drawing of the human body may 
#	be a good example of the form for a painting, but that does not 
#	mean it should be used as the basis for the next Mona Lisa.
#
#	We recommend that you review this program or TESample before 
#	beginning a new application.
#
# 	You can define {SymOptions} as "-sym on" or "-sym off" for use with SADE
POptions = {SymOptions}

PObjs		= Sample.p.o ∂
		"{Libraries}"Runtime.o ∂
		"{Libraries}"Interface.o ∂
		"{PLibraries}"PasLib.o

Sample		ƒƒ {PObjs} Sample.make
		Link -o {Targ} {PObjs} {SymOptions}
		SetFile {Targ} -t APPL -c 'MOOS' -a B

Sample		ƒƒ Sample.r Sample.h Sample.make
		Rez -rd -o {Targ} Sample.r -append

Sample.p.o	ƒƒ Sample.make
