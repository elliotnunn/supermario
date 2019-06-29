/*
	File:		dpMacsBugTemplates.r

	Contains:	MacsBug templates for debugging the Edition Manager

	Usage:		Rez "dpMacsBugTemplates.r" -o "{SystemFolder}Debugger Prefs" -a 

	Written by:	Nick Kledzik

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 5/31/90	ngk		update
		 <2>	  1/6/90	ngk		Updated record definitions
	   <1.6>	 11/4/89	ngk		Updated again
	   <1.5>	 10/2/89	ngk		Updated new PubCB
	   <1.4>	  9/7/89	ngk		Updated to new PubCB.
	   <1.3>	 8/29/89	ngk		Updated to new PubCB. Add atb macros for all routines. Added
									parameter templates for all routines. Added templare for
									aliases.
	   <1.2>	  8/8/89	ngk		Updated to new PubCB. Added DataPubGlobals and Show macro.
	   <1.1>	 6/11/89	ngk		Changed case of fields to start lowercase Updated to new PubCB
									Added Alias manager types
	   <1.0>	 5/29/89	ngk		Updated SectionRecord

	To Do:
*/


type 'mxwt'{
	integer = $$CountOf(templates);					/* Number of templates               */
	array templates { pString;						/* Type name                         */
				      integer = $$CountOf(fields);  /* Number of fields in this template */
				      array fields { pString;	    /* Field name                        */
								     pString;       /* Field type                        */
								     integer;		/* Number of fields of this type     */
							       };
    			    };
};

type 'mxbm'{
	integer = $$CountOf(templates);					/* Number of templates   	*/
	array templates { 	pString;					/* Macro name  				*/
						pString;					/* Macro expansion 			*/
					};
};

resource 'mxwt' (211, "Editions Manager Records") {
	{
			
	"ECS", {
	 	"theFile",			"FSSpec",				1,
		"theFileScript",	"Word",					1,
	 	"thePart",			"Long",					1,
	 	"thePartName",		"pString",				32,
		"thePartScript",	"Word",					1,
		},
				
	"EditionInfoRecord", {
	 	"crDate",		"Long",					1,
	 	"mdDate",		"Long",					1,
	 	"fdCreator",	"Text",					4,
	 	"fdType",		"Text",					4,
	 	"container",	"ECS",					1
		},
		
	
	"EachAppsUsage", {
	 	"app",					"Long",				1,
	 	"useCount",				"Word",				1,
	 	"IOCount",				"Word",				1
		},
		
	"TotalAppUsage", {
	 	"totalUseCount",		"Word",				1,
	 	"totalIOCount",			"Word",				1,
		""	,					"Skip",				2,	
		"slotSize"	,			"Word",				1,	
		"maxSlots",				"Word",				1,		
		"lastUsedSlot",			"Word",				1,	
	 	"appUseCount[1]",		"EachAppsUsage",	1,
	 	"appUseCount[2]",		"EachAppsUsage",	1
		},
		
	"PubCBLink", {
	 	"nextPubCB",			"Long",				1,
	 	"prevPubCB",			"Long",				1
		},
		
	"PubCBRecord", {		
	 	"links",				"PubCBLink",		1,
		"usageInfo",			"^^TotalAppUsage",	1,
	 	"volumeInfo",			"Long",				1,
	 	"pubCNodeID",			"Long",				1,
	 	"lastVolMod",			"Long",				1,
	 	"lastDirMod",			"Long",				1,
	 	"oldPubCB",				"Long",				1,
	 	"publisherApp",			"Long",				1,
	 	"publisher",			"Long",				1,
	 	"publisherAlias",		"Long",				1,
		"publisherCount",		"Word",				1,
		"publisherKind",		"Byte",				1,
		"fileMissing",			"Byte",				1,
	 	"fileRefNum",			"Word",				1,
	 	"openMode",				"Word",				1,
	 	"fileMark",				"Long",				1,
	 	"rangeLockStart",		"Word",				1,
	 	"rangeLockLen",			"Word",				1,
	 	"allocMap",				"Long",				1,
	 	"formats",				"Long",				1,
	 	"info",					"EditionInfoRecord",1,
		},
		
	"FormatPositionInfo", {
	 	"format",				"Text",				4,
	 	"mark",					"Long",				1,
	 	"index",				"Word",				1,
	 	"length",				"Word",				1
		},
		
	"SIOCBRecord", {
	 	"ioRefNum",				"Long",					1,
	 	"ioProc",				"Long",					1,
	 	"section",				"Long",					1,
	 	"blockSize",			"Long",					1,
		""	,					"Skip",					2,	
		"slotSize"	,			"Word",					1,	
		"maxSlots",				"Word",					1,		
		"lastUsedSlot",			"Word",					1,	
	 	"eachFormat[1]",		"FormatPositionInfo",	1,
	 	"eachFormat[2]",		"FormatPositionInfo",	1
		},
		
	"SectionRecord", {
	 	"version",		"Byte",					1,
	 	"sectionType",	"Byte",					1,
	 	"mode",			"Word",					1,
	 	"mdDate",		"Long",					1,
	 	"sectionID",	"Long",					1,
	 	"refCon",		"Long",					1,
	 	"alias",		"Long",					1,
	 	"subPart",		"Long",					1,
	 	"nextSection",	"Long",					1,
	 	"controlBlock",	"Long",					1,
	 	"refNum",		"Long",					1
		},
	
	
	"FailInfo", {
	 	"regs",					"Long",				11,
	 	"returnTo",				"Long",				1,
	 	"errorPtr",				"Long",				1,
	 	"nextInfo",				"Long",				1,
		},
		
	"EdtnMgrGlobals", {
	 	"pubCBlinks",			"PubCBLink",		1,
	 	"perAppListHead",		"Long",				1,
	 	"nextPollTime",			"Long",				1,
	 	"A6Link",				"Long",				1,
	 	"gotoHere",				"Long",				1,
	 	"failureHandler",		"^FailInfo",		1,
		"lastPubUsed",			"Long",				1
		},
		
	"PerAppGlobals", {
	 	"nextApp",				"Long",				1,
	 	"appPSN",				"Long",				2,
		"signature",			"Text",				4,
	 	"sectionHandleListHead","Long",				1,
		"emulator",				"Long",				1,
	 	"appName",				"pString",			32,
		},
		
	"NewPublisherReply", {
	 	"canceled",				"Boolean",			1,
	 	"replacing",			"Boolean",			1,
		"usePart",				"Boolean",			1,
		"",						"Skip",				1,
	 	"preview",				"Long",				1,
	 	"previewFormat",		"Text",				4,
	 	"container",			"ECS",				1,
		},
		
	"NewSubscriberReply", {
	 	"canceled",				"Boolean",			1,
	 	"container",			"ECS",				1
		},
		
	"SectionOptionsReply", {
	 	"canceled",				"Boolean",			1,
	 	"changed",				"Boolean",			1,
	 	"sectionH",				"Long",				1,
	 	"action",				"Text",				4
		},

			
	/* maybe these last ones should be in 'AliasMacsBugTemplates.r' */
	"FSSpec", {
	 	"vRefNum",			"Word",					1,
	 	"parID",			"Long",					1,
	 	"name",				"pString",				64
		},
				
	"AliasRecord", {
	 	"userType",			"Text",				4,
	 	"aliasSize",		"Word",				1
		},
		
	"AliasRecordPriv", {
	 	"userType",			"Text",				4,
	 	"aliasSize",		"Word",				1,
	 	"aliasVersion",		"Word",				1,
	 	"aliasKind",		"Word",				1,
	 	"volumeName",		"pString",			28,
	 	"volumeCrDate",		"Long",				1,
	 	"volumeSig",		"Word",				1,
	 	"volumeType",		"Word",				1,
	 	"folderName",		"pString",			32,
	 	"parDirID",			"Long",				1,
	 	"fileName",			"pString",			32,
	 	"fileNum",			"Long",				1,
	 	"fileID",			"Long",				1,
	 	"fileCrDate",		"Long",				1,
	 	"fileType",			"Text",				4,
	 	"fdCreator",		"Text",				1,
	 	"nlvlFrom",			"Word",				1,
	 	"nlvlTo",			"Word",				1
		},

	},	
};

resource 'mxwt' (212, "Editions Mgr Parameters") {
	{
		
	"param.NewSection", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^Long",				1,
	 	"initialMode",			"Word",					1,
	 	"sectionID",			"Long",					1,
	 	"kind",					"Byte",					1,
	 	"",						"Skip",					1,
	 	"sectionDocument",		"^FSSpec",				1,
	 	"container",			"^ECS",					1
		},
		
	"param.GetSection", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^Long",				1,
	 	"resID",				"Word",					1,
	 	"sectionDocument",		"^FSSpec",				1
		},
		
	"param.SaveSection", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^^^SectionRecord",		1,
	 	"resID",				"Word",					1
		},
		
	"param.CloneSection", {
	 	"",						"Skip",					8,
	 	"clonedSectionH",		"^Long",				1,
	 	"sectionH",				"^Long",				1
		},
		
	"param.AssociateSection", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^^^SectionRecord",		1,
	 	"newSectionDocument",	"^FSSpec",				1
		},
		
	"param.DisposeSection", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^^^SectionRecord",		1
		},
		
	"param.GetEditionInfo", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^^^SectionRecord",		1,
	 	"pubInfo",				"^PubInfoRecord",		1
		},
		
	"param.GotoPublisherSection", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^^^SectionRecord",		1
		},
		
	"param.RegisterSection", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^^^SectionRecord",		1,
	 	"withPub",				"^ECS",					1
		},
		
	"param.UnRegisterSection", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^^^SectionRecord",		1,
		},
		
	"param.CreateEditionContainerFile", {
	 	"",						"Skip",					8,
	 	"publication",			"^FSSpec",				1,
	 	"fdCreator",			"Text",					4,
	 	"comments",				"^pString",				1
		},
		
	"param.OpenNewEdition", {
	 	"",						"Skip",					8,
	 	"refNum",				"^Long",				1,
	 	"sectionDocument",		"^FSSpec",				1,
	 	"fdCreator",			"Text",					4,
	 	"sectionH",				"^^^SectionRecord",		1,
		},
		
	"param.OpenEdition", {
	 	"",						"Skip",					8,
	 	"refNum",				"^Long",				1,
	 	"sectionH",				"^^^SectionRecord",		1
		},
		
	"param.CloseEdition", {
	 	"",						"Skip",					8,
	 	"refNum",				"Long",					1,
	 	"success",				"Boolean",				1
		},
		
	"param.ReadEdition", {
	 	"",						"Skip",					8,
	 	"len",					"Long",					1,
	 	"buffPtr",				"Long",					1,
	 	"whichFormat",			"Text",					4,
	 	"refNum",				"Long",					1
		},
		
	"param.WriteEdition", {
	 	"",						"Skip",					8,
	 	"len",					"Long",					1,
	 	"buffPtr",				"Long",					1,
	 	"whichFormat",			"Text",					4,
	 	"refNum",				"Long",					1
		},
		
	"param.GetEditionFormatMark", {
	 	"",						"Skip",					8,
	 	"mark",					"^Long",				1,
	 	"whichFormat",			"Text",					4,
	 	"refNum",				"Long",					1
		},
		
	"param.SetEditionFormatMark", {
	 	"",						"Skip",					8,
	 	"mark",					"Long",					1,
	 	"whichFormat",			"Text",					4,
	 	"refNum",				"Long",					1
		},
		
	"param.EditionHasFormat", {
	 	"",						"Skip",					8,
	 	"length",				"^Long",				1,
	 	"whichFormat",			"Text",					4,
	 	"refNum",				"Long",					1
		},
		
	"param.NewSubscriberDialog", {
	 	"",						"Skip",					8,
	 	"reply",				"^NewSubscriberReply",	1,
	 	"where.v",				"Word",					1,
	 	"where.h",				"Word",					1
		},
		
	"param.NewPublisherDialog", {
	 	"",						"Skip",					8,
	 	"reply",				"^NewPublisherReply",	1,
	 	"where.v",				"Word",					1,
	 	"where.h",				"Word",					1
		},
		
	"param.GetLastEditionContainerUsed", {
	 	"",						"Skip",					8,
	 	"publication",			"^ECS",					1
		},
		
	"param.IsRegisteredSection", {
	 	"",						"Skip",					8,
	 	"sectionH",				"^SectionH",			1
		},
		
		
	},
};


resource 'mxbm' (211, "Editions Mgr Macros") {
	{
		"ShowEMGlobals",
		"dm @(@2b6+80) EdtnMgrGlobals",
		
		"InitEditionPack",
		"pack11 D0.B=00",
		
		"NewSection",
		"pack11 D0.B=02",
		
		"RegisterSection",
		"pack11 D0.B=04",

		"UnRegisterSection",
		"pack11 D0.B=06",
		
		"IsRegisteredSection",
		"pack11 D0.B=08",
		
		"FindEditionContainerDialog",
		"pack11 D0.B=0A",
		
		"AssociateSection",
		"pack11 D0.B=0C",
		
		"CreateEditionContainerFile",
		"pack11 D0.B=0E",
		
		"DeleteEditionContainerFile",
		"pack11 D0.B=10",
		
		"OpenEdition",
		"pack11 D0.B=12",
		
		"OpenNewEdition",
		"pack11 D0.B=14",
		
		"CloseEdition",
		"pack11 D0.B=16",
		
		"EditionHasFormat",
		"pack11 D0.B=18",
		
		"ReadEdition",
		"pack11 D0.B=1A",
		
		"WriteEdition",
		"pack11 D0.B=1C",
		
		"GetEditionFormatMark",
		"pack11 D0.B=1E",
		
		"SetEditionFormatMark",
		"pack11 D0.B=20",
		
		"GetEditionInfo",
		"pack11 D0.B=22",
		
		"GotoPublisherSection",
		"pack11 D0.B=24",
		
		"GetLastEditionContainerUsed",
		"pack11 D0.B=26",
		
		"GetStandardFormats",
		"pack11 D0.B=28",
		
		"GetEditionOpenerProc",
		"pack11 D0.B=2A",
		
		"SetEditionOpenerProc",
		"pack11 D0.B=2C",
		
		"CallEditionOpenerProc",
		"pack11 D0.B=2E",
		
		"CallFormatIOProc",
		"pack11 D0.B=30",
		
		"NewSubscriberDialog",
		"pack11 D0.B=32",
		
		"NewSubscriberExpDialog",
		"pack11 D0.B=34",
		
		"NewPublisherDialog",
		"pack11 D0.B=36",
		
		"NewPublisherExpDialog",
		"pack11 D0.B=38",
		
		"SectionOptionsDialog",
		"pack11 D0.B=3A",
		
		"SectionOptionsExpDialog",
		"pack11 D0.B=3C",
		
	},
};


