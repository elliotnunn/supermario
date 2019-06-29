/*
	File:		ProcessMgrINIT.r

	Contains:	Resources used to build the ProcessMgrINIT

	Copyright:	© 1983-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	11/25/92	DRF		first checked in
*/

#include "Types.r"
#include "SysTypes.r"

#define IncSys Include $$Shell("ObjDir")

//	Version Resource

	resource 'vers' (1) {
		LIntVers, LangInt, SysVersion, SysVersion", © Apple Computer, Inc. 1983-1992"
	};

IncSys	"Scheduler.rsrc" 'scod';
