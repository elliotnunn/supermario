/*
	File:		ROM_EM.r

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM2>	11/11/92	RB		Added the alertPositionMainScreen option to the dialogs.
		 <1>	 11/5/92	RB		first checked in

*/

/*

	File: ROM_EM.r
	
	Created by: Ricardo Batista
	
	© Copyright 1992 Apple Computer, Inc.  All Rights Reserved.
	
	Resources for the ROM pseudo-extensions Manager.
	
*/

#ifndef SystemSevenOrLater
#define SystemSevenOrLater 1
#endif


#include "systypes.r"
#include "types.r"



// if you change these id's, change them also in ROM_EM.c

#define		kEMID					-15020

#define		kEM_Dialog				-15020

#define		kNeedINITID				-15020
#define		kNeedNoINITID			-15021
#define		duplicateDlg			-15021


resource 'DITL' (kEM_Dialog, "EM Panel") {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{164, 239, 184, 299},
		Button {
			enabled,
			"Revert"
		},
		/* [2] */
		{44, 105, 152, 299},
		UserItem {
			enabled
		},
		/* [3] */
		{164, 104, 184, 164},
		Button {
			enabled,
			"All Off"
		},
		/* [4] */
		{164, 172, 184, 232},
		Button {
			enabled,
			"All On"
		},
		/* [5] */
		{5, 100, 18, 300},
		StaticText {
			enabled,
			"© Copyright 1992 Apple Computer, Inc."
		},
		/* [6] */
		{23, 100, 40, 300},
		StaticText {
			enabled,
			"       All Rights Reserved."
		}
	}
};


resource 'DITL' (duplicateDlg, "DuplicateFile") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{99, 192, 119, 250},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 67, 75, 253},
		StaticText {
			disabled,
			"An extension could not be moved because "
			"a duplicate already exists in the target"
			" folder."
		},
		/* [3] */
		{13, 13, 45, 45},
		Icon {
			disabled,
			0
		}
	}
};



data 'iNIT' (kNeedINITID, "Types need INIT") {
	$"494E 4954 6364 6576 5244 4556"                      /* INITcdevRDEV */
};

data 'iNIT' (kNeedNoINITID, "Types NO INIT required") {
	$"5052 4553 5052 4552 6164 6576 6665 7874"            /* PRESPRERadevfext */
	$"7363 7269 6362 6E64 6662 6E64 7462 6E64"            /* scricbndfbndtbnd */
	$"6464 6576 6170 7065 6763 3234 6164 7270"            /* ddevappegc24adrp */
	$"6462 6772 6466 696C 4150 504C"                      /* dbgrdfilAPPL */
};

resource 'DLOG' (kEM_Dialog, "EM Panel") {
	{98, 128, 296, 350},
	documentProc,
	invisible,
	goAway,
	0x0,
	kEM_Dialog,
	"ROM EM",
	alertPositionMainScreen
};


resource 'DLOG' (duplicateDlg, "DuplicateFile") {
	{84, 44, 221, 327},
	dBoxProc,
	visible,
	noGoAway,
	0x0,
	duplicateDlg,
	"DuplicateFile",
	alertPositionMainScreen
};


data 'FTyp' (kEMID, "Folder types") {
	$"6578 746E 6578 7444 6374 726C 6374 7244"            /* extnextDctrlctrD */
	$"6D61 6373 6D61 6344 7374 7274 7374 7244"            /* macsmacDstrtstrD */
};

resource 'fld#' (kEMID, "New folders") {
	{	/* array: 5 elements */
		/* [1] */
		'extD',
		inSystemFolder,
		"Extensions (disabled)",
		/* [2] */
		'ctrD',
		inSystemFolder,
		"Control Panels (disabled)",
		/* [3] */
		'macD',
		inSystemFolder,
		"System Extensions (disabled)",
		/* [4] */
		'strD',
		inSystemFolder,
		"Startup Items (disabled)"
	}
};

