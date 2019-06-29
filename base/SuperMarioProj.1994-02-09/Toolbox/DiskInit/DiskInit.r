/*

	File:		DiskInit.r

	Contains:	All resources used by the disk init PACK-2.

	Written by:	Philip Koch, June 1990.

	Copyright:	© 1990-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 3/27/92	DTY		Set resSysHeap bit on 'PACK' resource.
		 <8>	 2/10/92	JSM		Moved this file to DiskInit folder, keeping all the old
									revisions.
		 <7>	  7/2/91	JSM		Remove obsolete SystemSevenOrLater conditionals.
		 <6>	12/18/90	stb		& JDR; change SystemSevenOrBetter to SystemSevenOrLater
		 <5>	 10/5/90	dba		center the dialog
		 <4>	 8/27/90	PK		Add itNewFormat message for future volume formats.
		 <3>	 7/24/90	RLC		Update help strings to latest from J.Trumble.
		 <2>	 7/24/90	RLC		Change explicit #include of BalloonTypes.r to get it from
									{RIncludes}.
		 <1>	 6/21/90	PK		first checked in
*/

#include "Types.r"
#include "BalloonTypes.r"

	#define DiskInitID	-6079

	Include $$Shell("ObjDir")"DiskInit.a.rsrc" 'RSRC' (0) as 'PACK' (2, sysHeap, purgeable);
	Include $$Shell("ObjDir")"DiskInitHFS.a.rsrc" 'RSRC' (0) as 'FMTR' (DiskInitID);

	resource 'DLOG' (DiskInitID, purgeable) {
		{60, 120, 158, 405},
		dBoxProc, visible,
		noGoAway,
		0,
		DiskInitID,
		"",
		alertPositionMainScreen
	};

	#define HIDE	16384					/* Constant used by HideDItem and ShowDItem  */

	resource 'DITL' (DiskInitID) { {
		{10, 15, 42, 47}, Icon { disabled, -6079 },												// 1 Icon
		{69, HIDE+196, 90, HIDE+270}, Button { enabled, "Initialize" },							// 2 InitBut
		{69, HIDE+15, 90, HIDE+89}, Button { enabled, "Eject" },								// 3 EjectBut
		{69, HIDE+106, 90, HIDE+180}, Button { enabled, "OK" },									// 4 OkBut
		{29, HIDE+62, 45, HIDE+284}, StaticText { disabled, "Do you want to initialize it?" },	// 5 InitIt
		{29, HIDE+65, 45, HIDE+267}, EditText { enabled, "Untitled" },							// 6 NameText
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "This disk is unreadable:" },		// 7 UnReadable
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "This disk is damaged:" },			// 8 Damaged
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "This is not a Macintosh disk:" },	// 9 NotMacDisk
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "Please name this disk:" },			//10 NameDisk
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "Formatting disk…" },				//11 Formatting
		{29, HIDE+62, 54, HIDE+284}, StaticText { disabled, "" },								//12 UndoInitIt
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "Initialization failed!" },			//13 InitFailed
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "Initialize this disk?" },			//14 InitThis
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "This is a two-sided disk!" },		//15 2SidedDisk
		{69, HIDE+106, 90, HIDE+180}, Button { enabled, "One-Sided" },							//16 1SidedBut
		{69, HIDE+196, 90, HIDE+270}, Button { enabled, "Two-Sided" },							//17 2SidedBut
		{69, HIDE+15, 90, HIDE+89}, Button { enabled, "Cancel" },								//18 CancelBut
		{29, HIDE+62, 45, HIDE+284}, StaticText { disabled, "This disk is write-protected!" },	//19 WrProtected
		{7, HIDE+62, 54, HIDE+284}, StaticText { disabled, "^0" },								//20 ParamText
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "Verifying Format…" },				//21 Verifying
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "Creating Directory…" },				//22 CreatingDir
		{29, HIDE+62, 45, HIDE+284}, StaticText { disabled, "There are open files on this disk." },//23 FilesBusy
		{7, HIDE+62, 54, HIDE+284}, StaticText { disabled,										//24 GCRonMFM
			"This disk is improperly formatted for use in this drive. "
			"Do you want to initialize it?"
		},
		{7, HIDE+62, 54, HIDE+284}, StaticText { disabled,										//25 WarnMsg
			"This process will erase all information on this disk."
		},
		{69, HIDE+15, 90, HIDE+89}, Button { enabled, "Cancel" },								//26 WarnCancel
		{69, HIDE+196, 90, HIDE+270}, Button { enabled, "Erase" },								//27 WarnErase
		{10, HIDE+15, 42, HIDE+47}, Icon { disabled, 2 },										//28 WarnIcon
		{7, HIDE+62, 23, HIDE+284}, StaticText { disabled, "Re-Verifying Format…" },			//29 ReVerifying
		{69, HIDE+106, 90, HIDE+180}, Button { enabled, "OK" },									//30 FailedOkBut
		{7, HIDE+62, 54, HIDE+284}, StaticText { disabled,										//31 NewFormat
			"This disk uses a new format not supported by the system software on this Macintosh."
		},
		{0,0,0,0}, UserItem { disabled },														//32 UserItem
		{0,0,0,0}, HelpItem { disabled, HMScanhdlg { DiskInitID }}							//33 HelpMsgs
	} };

	resource 'ICON' (DiskInitID, purgeable) {
		$"7FFFFFF0 80800208 8000A004 80800202"
		$"80011002 80800202 80011002 80800202"
		$"8000A002 80800202 802AA802 80000002"
		$"80000002 80000002 80000002 82AAAA82"
		$"80000002 88000022 80000002 88000022"
		$"80000002 88000022 80000002 88000022"
		$"80000002 88000022 80000002 88000022"
		$"80000002 88000022 80000002 7FFFFFFC"
	};

	resource 'hdlg' (DiskInitID) {
		HelpMgrVersion,								// Help Version
		0,											// offset to 1st item
		0,											// options
		0,											// theProc
		0,											// variant
		HMSkipItem {},								// skip missing message
		{

			HMStringResItem {						//  1 - Icon
				{0,0}, {0,0,0,0},
				DiskInitID, 1,
				DiskInitID, 1,
				DiskInitID, 1,
				DiskInitID, 1
			},

			HMStringResItem {						//  2 - InitBut
				{0,0}, {0,0,0,0},
				DiskInitID, 2,
				DiskInitID, 2,
				DiskInitID, 2,
				DiskInitID, 2
			},

			HMStringResItem {						//  3 - EjectBut
				{0,0}, {0,0,0,0},
				DiskInitID, 3,
				DiskInitID, 3,
				DiskInitID, 3,
				DiskInitID, 3
			},

			HMStringResItem {						//  4 - OkBut
				{0,0}, {0,0,0,0},
				DiskInitID, 4,
				DiskInitID, 4,
				DiskInitID, 4,
				DiskInitID, 4
			},

			HMStringResItem {						//  5 - InitIt
				{8,20}, {0,0,0,0},
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5
			},

			HMStringResItem {						//  6 - NameText
				{8,20}, {0,0,0,0},
				DiskInitID, 6,
				DiskInitID, 6,
				DiskInitID, 6,
				DiskInitID, 6
			},

			HMStringResItem {						//  7 - UnReadable
				{8,20}, {0,0,0,0},
				DiskInitID, 7,
				DiskInitID, 7,
				DiskInitID, 7,
				DiskInitID, 7
			},

			HMStringResItem {						//  8 - Damaged
				{8,20}, {0,0,0,0},
				DiskInitID, 8,
				DiskInitID, 8,
				DiskInitID, 8,
				DiskInitID, 8
			},

			HMStringResItem {						//  9 - NotMacDisk
				{8,20}, {0,0,0,0},
				DiskInitID, 9,
				DiskInitID, 9,
				DiskInitID, 9,
				DiskInitID, 9
			},

			HMStringResItem {						// 10 - NameDisk
				{8,20}, {0,0,0,0},
				DiskInitID, 6,
				DiskInitID, 6,
				DiskInitID, 6,
				DiskInitID, 6
			},

			HMStringResItem {						// 11 - Formatting
				{0,0}, {0,0,0,0},
				DiskInitID, 0,						// no relinquish while formatting
				DiskInitID,0,
				DiskInitID,0,
				DiskInitID,0
			},

			HMStringResItem {						// 12 - UndoInitIt
				{0,0}, {0,0,0,0},
				DiskInitID, 0,						// this item zeroes out a portion of the screen
				DiskInitID,0,
				DiskInitID,0,
				DiskInitID,0
			},

			HMStringResItem {						// 13 - InitFailed
				{8,20}, {0,0,0,0},
				DiskInitID, 10,
				DiskInitID, 10,
				DiskInitID, 10,
				DiskInitID, 10
			},

			HMStringResItem {						// 14 - InitThis
				{8,20}, {0,0,0,0},
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5
			},

			HMStringResItem {						// 15 - 2SidedDisk
				{8,20}, {0,0,0,0},
				DiskInitID, 11,
				DiskInitID, 11,
				DiskInitID, 11,
				DiskInitID, 11
			},

			HMStringResItem {						// 16 - 1SidedBut
				{0,0}, {0,0,0,0},
				DiskInitID, 12,
				DiskInitID, 12,
				DiskInitID, 12,
				DiskInitID, 12
			},

			HMStringResItem {						// 17 - 2SidedBut
				{0,0}, {0,0,0,0},
				DiskInitID, 13,
				DiskInitID, 13,
				DiskInitID, 13,
				DiskInitID, 13
			},

			HMStringResItem {						// 18 - CancelBut
				{0,0}, {0,0,0,0},
				DiskInitID, 3,
				DiskInitID, 3,
				DiskInitID, 3,
				DiskInitID, 3
			},

			HMStringResItem {						// 19 - WrProtected
				{8,20}, {0,0,0,0},
				DiskInitID, 14,
				DiskInitID, 14,
				DiskInitID, 14,
				DiskInitID, 14
			},

			HMStringResItem {						// 20 - ParamText
				{23,20}, {0,0,0,0},
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5
			},

			HMStringResItem {						// 21 - Verifying
				{0,0}, {0,0,0,0},
				DiskInitID, 0,						// no relinquish verifying…
				DiskInitID,0,
				DiskInitID,0,
				DiskInitID,0
			},

			HMStringResItem {						// 22 - CreatingDir
				{0,0}, {0,0,0,0},
				DiskInitID, 0,						// no relinquish creating dir…
				DiskInitID,0,
				DiskInitID,0,
				DiskInitID,0
			},

			HMStringResItem {						// 23 - FilesBusy
				{8,20}, {0,0,0,0},
				DiskInitID, 15,
				DiskInitID, 15,
				DiskInitID, 15,
				DiskInitID, 15
			},

			HMStringResItem {						// 24 - GCRonMFM
				{23,20}, {0,0,0,0},
				DiskInitID, 16,
				DiskInitID, 16,
				DiskInitID, 16,
				DiskInitID, 16
			},

			HMStringResItem {						// 25 - WarnMsg
				{8,20}, {0,0,0,0},
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5
			},

			HMStringResItem {						// 26 - WarnCancel
				{0,0}, {0,0,0,0},
				DiskInitID, 3,
				DiskInitID, 3,
				DiskInitID, 3,
				DiskInitID, 3
			},

			HMStringResItem {						// 27 - WarnErase
				{0,0}, {0,0,0,0},
				DiskInitID, 2,
				DiskInitID, 2,
				DiskInitID, 2,
				DiskInitID, 2
			},

			HMStringResItem {						// 28 - WarnIcon
				{0,0}, {0,0,0,0},
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5,
				DiskInitID, 5
			},

			HMStringResItem {						// 29 - ReVerifying
				{0,0}, {0,0,0,0},
				DiskInitID, 0,						// no relinquish doing this…
				DiskInitID,0,
				DiskInitID,0,
				DiskInitID,0
			},

			HMStringResItem {						// 30 - FailedOkBut
				{0,0}, {0,0,0,0},
				DiskInitID, 10,
				DiskInitID, 10,
				DiskInitID, 10,
				DiskInitID, 10
			},

			HMStringResItem {						// 31 - NewFormat
				{8,20}, {0,0,0,0},
				DiskInitID, 9,
				DiskInitID, 9,
				DiskInitID, 9,
				DiskInitID, 9
			}
	}	};


	resource 'STR#'	(DiskInitID, purgeable) { {

		"This icon shows you the location of the disk you are about to erase or "
		"initialize.";																// 1. the disk icons

		/* [48] Erase disk/Disk initialization, FDHD floppy:1st dialog:Initialize button, Normal */
		"To erase or initialize the selected disk, click this button.";				// 2. init and erase buttons

		/* [44] Erase disk/Disk initialization, normal floppy:1st dialog:Cancel button, Normal */
		"If you decide not to erase or initialize the selected disk, click this "
		"button.";																	// 3. eject and cancel buttons

		/* [52] Erase disk/Disk initialization, normal floppy:3rd dialog: OK button, Normal */
		"After you have typed a name for the disk, click this button to initialize "
		"the disk.";																// 4. OK button typing disk name

		"";
		/* "TEMPORARY: Initializing the disk will erase all information (if any) "		// 5. msgs that ask to erase
		"on it.  Click Eject or Cancel to abort the operation."; */

		/* [51] Erase disk/Disk initialization, normal floppy:3rd dialog: Typing area, Normal */
		"Type a name for the disk in this box.";									// 6. name this disk txt and box

		"";
		/* "TEMPORARY: This is normal if the disk is brand new (blank), but can also "	// 7. "this disk is unreadable"
		"happen if I/O errors occur trying to read a formatted disk."; */

		"";
		/* "TEMPORARY: The directory information on this disk has been garbled, "		// 8. "this disk is damaged"
		"probably due to a software error.  It might be repairable by a "
		"utility such as Disk First Aid™.  The disk is not damaged physically."; */

		"";
		/* "TEMPORARY: The disk was not created on a Macintosh computer, and "			// 9. "this is not a mac disk"
		"therefore cannot be used except possibly by Apple File Exchange™ "
		"or a similar utility."; */

		"";
		/* "TEMPORARY: The disk could not be initialized.  Usually, this is because "	//10. "Initialization failed!"
		"the disk has a physical defect, although it might be due to a dirty or "
		"malfunctioning disk drive.  Click OK to continue."; */

		"";
		/* "TEMPORARY: You have tried to use a 2-sided disk in a 1-sided drive, which "//11. "This is a 2-sided disk!"
		"is not possible.  However, the disk can be re-initialized on just one "
		"side for use on this drive."; */

		/* [45] Erase disk/Disk initialization, normal floppy:1st dialog:One sided button, Normal */
		"To initialize the selected disk as a one-sided, 400K disk, click this "
		"button.";																	//12. 1-sided button

		/* [46] Erase disk/Disk initialization, normal floppy:1st dialog:Two sided button, Normal */
		"To initialize the selected disk as a two-sided, 800K disk, click this "
		"button.";																	//13. 2-sided button

		"";
		/* "TEMPORARY: The disk cannot be initialized until the small tab on the "		//14. "this disk is write protected!"
		"corner of the disk has been moved back to expose the square hole."; */

		"";
		/* "TEMPORARY: The disk cannot be initialized while some application is using "//15. "open files on this disk!"
		"a file on it."; */

		"";
		/* "TEMPORARY: It appears that this FDHD (1440K) disk was initialized on an "	//16. "improperly formatted!"
		"800K drive, which makes it unuseable on a FDHD drive.  Re-initializing "
		"it for 1440K may make it useable, though it will destroy its previous "
		"contents."; */
	}};
