#	MakeFile	-  Make File for Assembly Language 
#					Structured Macros Example.
#
#	Copyright Apple Computer, Inc. 1986-1990
#	All rights reserved.
#
#	This makefile builds:
#		Sample  - an example application.
#
# This file is a MPW Script file to build the sample program in
#    {MPW}Interfaces:AStructMacs
#

AOptions		= -w

AObjs			= Sample.a.o 

FlowCtlMacs.d	ƒ FlowCtlMacs.a
	asm FlowCtlMacs.a
	
ProgStrucMacs.d	ƒ ProgStrucMacs.a
	asm ProgStrucMacs.a

Sample.a.o		ƒƒ Sample.make FlowCtlMacs.d ProgStrucMacs.d

Sample			ƒƒ {AObjs} Sample.make
					Link -o {Targ} {AObjs}
					SetFile {Targ} -t APPL -c 'MOOS' -a B

Sample			ƒƒ Sample.r Sample.make
					Rez -rd -o {Targ} Sample.r -append
