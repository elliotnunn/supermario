#
#	Macintosh Developer Technical Support
#
#	EditText Sample Control Panel Device
#
#	EditCdev
#
#	EditCdev.make	-	Make Source
#
#	Copyright © Apple Computer, Inc. 1988-1990
#	All rights reserved.
#
#	Versions:	1.1					7/88
#				1.0					6/88
#
#	Components:	EditCdev.c			Feb.  1, 1990
#				EditCdev.r			Feb.  1, 1990
#				EditCdev.make		Feb.  1, 1990
#
#	EditCdev demonstrates how to implement an editText item
#	in a Control Panel Device.  It utilizes the new undo, cut, copy,
#	paste, and delete messages that are sent to cdevs in
#	response to user menu selections.  How to handle private
#	storage is also covered.
#
# Enforce strict ptotype checking in ≥MPW 3.0 C
COptions = -r

SrcName			=	EditCdev
Lang			=	C
CdevName		=	{SrcName}

Objs				=	{SrcName}.{Lang}.o ∂
						"{Libraries}"Interface.o

{SrcName}.rsrc		ƒ	{SrcName}.r {CdevName}.make
		Rez -o {Targ} {SrcName}.r -t cdev -c hack

{CdevName}			ƒ	{SrcName}.rsrc {Objs} {CdevName}.make
		Duplicate -y {SrcName}.rsrc {Targ}
		Link -o {Targ} -rt cdev=-4064 -m TEXTCDEV {Objs} && ∂
			Setfile {CdevName} -a B && ∂
				Duplicate -y {CdevName} "{SystemFolder}"

