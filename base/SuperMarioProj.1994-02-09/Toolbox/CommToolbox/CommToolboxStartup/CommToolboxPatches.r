/*
	File:		CommToolboxPatches.r

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	  4/9/90	BBH		Example of creating standalone INIT which does a linked patch
									install.

	To Do:
*/

#define IncSys Include $$Shell("ObjDir")
#define codeAs 'RSRC' (0) as

IncSys "LinkedPatchLoaderINIT.rsrc" codeAs 'INIT' (1, locked);
IncSys "CommToolboxPatches.rsrc";
