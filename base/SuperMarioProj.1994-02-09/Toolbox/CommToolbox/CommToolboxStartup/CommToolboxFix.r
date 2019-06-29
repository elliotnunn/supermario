/*
	File:		CommToolboxFix.r

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1991 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	  7/9/91	BH		first checked in

	To Do:
*/

#include "Types.r"
#include "SysTypes.r"

#define IncSys Include $$Shell("ObjDir")
#define codeAs 'RSRC' (0) as

IncSys "LinkedPatchLoaderINIT.rsrc" codeAs 'INIT' (1, locked);
IncSys "CommToolboxFix.rsrc";

#ifdef INITVERSION
resource 'vers' (1) {
	0x07, 
	0x00, 
	development, 
	0x01,
	verUS,
	"7.0d1",
	"7.0d1 - Copyright © 1991 Apple Computer, Inc.  All rights reserved."
};
#endif INITVERSION