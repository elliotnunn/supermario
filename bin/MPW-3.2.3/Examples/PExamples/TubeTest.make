#
#	Macintosh Developer Technical Support
#
#	Simple Color QuickDraw Animation Sample Application
#
#	TubeTest
#
#	TubeTest.make	-	Make Source
#
#	Copyright © Apple Computer, Inc. 1988-1990
#	All rights reserved.
#
#	Versions:	1.1	(MPW 3.0 Final)	8/88
#
#	Components:	TubeTest.p			Feb.   1, 1990
#				TubeTest.r			Feb.   1, 1990
#				TubeTest.make		Feb.   1, 1990
#
#	The TubeTest program is a simple demonstration of how to use the Palette 
#	Manager in a color program.  It has a special color palette that is associated
#	with the main window.  The colors are animated using the Palette Manager 
#	to give a flowing tube effect.  The program is very simple, and the Palette
#	Manager and drawing parts are put in separate subroutines to make it easier
#	to figure out what is happening.
#	
#	The program is still a complete Macintosh application with a Main Event Loop,
#	so there is the extra code to run the MEL.  
#	
#	There is a resource file that is necessary as well, to define the Menus, Window,
#	Dialog, and Palette resources used in the program.  
#
#	See Sample and TESample for the general structure and MultiFinder techniques that
#	we recommend that you use when building a new application.
#

PObjs			=	TubeTest.p.o ∂
					"{Libraries}"Runtime.o ∂
					"{Libraries}"Interface.o ∂
					"{PLibraries}"PasLib.o

TubeTest		ƒƒ	{PObjs} TubeTest.make
		Link -o {Targ} {PObjs}
		SetFile {Targ} -t APPL -c '????'

TubeTest		ƒƒ	TubeTest.r TubeTest.make
		Rez -rd -o {Targ} TubeTest.r -append

