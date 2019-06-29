#
#	Macintosh Developer Technical Support
#
#	Simple Color QuickDraw Sample Application
#
#	SillyBalls
#
#	SillyBalls.make	-	Make Source
#
#	Copyright © Apple Computer, Inc. 1988-1990
#	All rights reserved.
#
#	Versions:	1.1	(MPW 3.0 Final)	8/88
#
#	Components:	SillyBalls.p		Feb.  1, 1990
#				SillyBalls.make	Feb.  1, 1990
#
#	This is a very simple sample program that demonstrates how to use Color 
#	QuickDraw.  It is about two pages of code, and does nothing more than open
#	a color window and draw randomly colored ovals in the window.
#	
#	The purpose is to show how to get some initial results with Color QuickDraw.
#	It is a complete program and is very short to be as clear as possible.
#	
#	It does not have an Event Loop.  It is not fully functional in the sense that
#	it does not do all the things you would expect a well behaved Macintosh 
#	program to do, like size the window naturally, have an event loop, use menus, 
#	etc.
#
#	See Sample and TESample for the general structure and MultiFinder techniques that
#	we recommend that you use when building a new application.
#

PObjs		=	SillyBalls.p.o ∂
				"{Libraries}"Runtime.o ∂
				"{Libraries}"Interface.o ∂
				"{PLibraries}"PasLib.o

SillyBalls	ƒ	{PObjs} SillyBalls.make
				Link -o {Targ} {PObjs}
				SetFile {Targ} -t APPL -c '????'
