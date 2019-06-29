{__________________________________________________________________________________________________
	File:		BalloonDefs.p
	
	Contains:	Some Pascal definitions to share w/ BalloonPack.p & Balloonptch28.p
	
	Written by:	Randy Carr
	
	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.
	
    This file is used in these builds: BigBang

	Change History (most recent first):
	
		<13>	 1/24/91	RLC		<stb> Change use of kHMPicItem to kHMPictItem (as the use of pic
									has been changed to pict).
		<12>	12/14/90	RLC		<ngk> Add hmgSystemMenuID, hmgSystemMenuItem globals that are
									used in patch to _MenuSelect & _SystemMenu. And
									hmgInMenuSelectFlag global to get popup and hierarchical menus
									to work in modal dialogs.
		<11>	12/14/90	dba		<JDR> get rid of IntegerPtr, now in Types.p
		<9+>	10/17/90	RLC		<ksm> Were calling HMScanTemplate items internally instead of
									duplicating that code when scanning an 'hwin' resource (saves
									>720 bytes).  Remove reference to hmgContentWindow and
									hmgOurBalloon as HMScanTemplate sets its own last window check
									and that isn't the hmgContentWindow variable.
		 <9>	 9/30/90	RLC		Change a reference to hmgLastWindow to hmgContentWindow to keep
									myself from get confused from hmgLastWindowPtr references.
		 <8>	 9/24/90	RLC		Add hmgHelpWindow windowptr to the HMGlobalRecord to keep the
									preallocated window ptr around so that we can compare it to the
									window being removed to close the window vs. just hiding it.
		 <7>	 9/13/90	RLC		Add a permanent windowRecord to the globals so we don't always
									allocate on the heap and change the global handle to a ptr.
		 <6>	 8/31/90	RLC		Add a BalloonBulk & OSEventAvail reentrant flags and Closeview
									“we need to remove the balloon flag”
		<4+>	 8/20/90	RLC		Remove some unused Booleans from the global record.
		<3+>	 7/17/90	RLC		Remove references to hmgProcessFileName, as we don't really need
									it.
		<2+>	 7/16/90	RLC		Update global record as ProcessMgr calling interfaces changed.
		 <2>	 7/13/90	RLC		Move more common types & const declarations here and add changes
									to support context check access to globals in _ShowHide patch.
	
	To Do:
__________________________________________________________________________________________________}
CONST
	{
	Universal Help Mgr Resource BTSTing bits that correspond to the
	additive "Options for Help Manager resources in 'hmnu', 'hdlg', 'hrct', 'hovr', & 'hfdr' resources"

	hmDefaultOptions		EQU		0			; default options for help manager resources
	hmUseSubID				EQU		1			; treat resID's in resources as subID's of driver base ID (for Desk Accessories)
	hmAbsoluteCoords		EQU		2			; ignore window port origin and treat rectangles as absolute coords (local to window) 
	hmSaveBitsNoWindow		EQU		4			; don't create a window, just blast bits on screen. No update event is generated
	hmSaveBitsWindow		EQU		8			; create a window, but restore bits behind window when window goes away & generate update event
	hmMatchInTitle			EQU		16			; for hwin resources, match string anywhere in window title string
	}
	
	kBTSTUseSubID			= 0; 		{ treat resID's in resources as subID's of driver base ID (for Desk Accessories) }
	kBTSTAbsoluteCoords		= 1; 		{ ignore window port origin and treat rectangles as absolute coords (local to window)  }
	kBTSTSaveBitsNoWindow	= 2; 		{ don't create a window, just blast bits on screen. No update event is generated  }
	kBTSTSaveBitsWindow		= 3; 		{ create a window, but restore bits behind window when window goes away & generate update event }
	kBTSTMatchInTitle		= 4; 		{ for hwin resources, match string anywhere in window title string }

TYPE
	DITLItem = Record
				itmHndl : Handle;
				itmRect	: Rect;
				itmType	: SignedByte;
				itmData	: ARRAY [0..0] of SignedByte;
			END;
	pDITLItem	= ^DITLItem;
	hDITLItem	= ^pDITLItem;
	
	ItemList			= RECORD
				dlgMaxIndex : INTEGER;		{ num items -1 }
				DITLItems	: ARRAY [0..0] of DITLItem;
			END;
	pItemList	= ^ItemList;
	hItemList	= ^pItemList;
	
	SByteArray			= RECORD
				CASE Integer OF
					1: (SBArray : ARRAY [0..1] of SignedByte);
					2: (Int : INTEGER);
			END;
			
	HMHelpSlot			= RECORD
				slotPID			: ProcessSerialNumber;
				slotResID		: INTEGER;
				slotID			: INTEGER;
			END;
							
	HMHelpQueueRecord	= RECORD
				queueCount		: INTEGER;
				queueRecords	: ARRAY [1..1000] of HMHelpSlot;
			END;
			
	
	PHMHelpQueueRecord		= ^HMHelpQueueRecord;
	HHMHelpQueueRecord		= ^PHMHelpQueueRecord;
			
			
			
	HMFontRecord		= RECORD
				CASE INTEGER OF
					1: 	(	hmgFont					: INTEGER;
							hmgFontSize				: INTEGER;
						);
					2: 	(	hmgFontSizeLong			: LongInt;
						);
			END;

		
	HMGlobalRecord	= RECORD
				hmgItemRect				: Rect;			{Don't change the order of the record unless
														you update BalloonPackEqu.a (@ Assembly Equivalent globals)}
				hmgItemNum				: INTEGER;		
				hmgTitleBalloon			: INTEGER;		{flag telling whether a menu title balloon was just up}
				hmgState				: INTEGER;		{last state of item last checked}
				hmgWindow				: WindowPtr;
				hmgHelpWindow			: WindowPtr;
				hmgHelpWRecord			: WindowRecord;
				hmgMenuID				: INTEGER;
				hmgSavedBitsHandle		: PixMapHandle;
				hmgTix					: LongInt;
				
				hmgLastPartCode			: INTEGER;
				hmgLastWindowPtr		: WindowPtr;
				hmgLastMenuID			: INTEGER;
				hmgLastEnabled			: LongInt;
				hmgLastItem				: INTEGER;
				
				hmgLastWidth			: INTEGER;
				hmgLastHeight			: INTEGER;
				hmgLastLeft				: INTEGER;
				hmgLastTop				: INTEGER;

				hmgSlopRect				: Rect;
				
				hmgDelay				: INTEGER;
								
				hmgFontAndSize			: HMFontRecord;
				hmgBulkReentrantCount	: INTEGER;
				hmgOSEventReentrantCount : INTEGER;
				hmgCloseViewCount		: INTEGER;
				hmgLayer				: LayerPtr;
				hmgProcessName			: String[33];
				hmgMenuQueue			: HHMHelpQueueRecord;
				hmgDialogQueue			: HHMHelpQueueRecord;
				hmgTEHandle				: TEHandle;
				
				hmgSystemMenuID			: INTEGER;
				hmgSystemMenuItem		: INTEGER;
				hmgInMenuSelectFlag		: INTEGER;
				
				hmgWhatIs				: BOOLEAN;
				hmgKeepTEHandle			: BOOLEAN;
				hmgOurDialog			: BOOLEAN;
				hmgHasColorQD			: BOOLEAN;
			END;
			
	HMGlobalPtr		= ^HMGlobalRecord;


	HMMessageArray		= RECORD
				helpType	: INTEGER;
				CASE INTEGER OF
					kHMStringItem:	(	hmmEnabledString	: STR255;
										hmmDisabledString	: STR255;
										hmmCheckedString	: STR255;
										hmmOtherString		: STR255;
									);
					kHMPictItem:	(	hmmEnabledPict		: INTEGER;
										hmmDisabledPict		: INTEGER;
										hmmCheckedPict		: INTEGER;
										hmmOtherPict		: INTEGER;
									);
					kHMStringResItem:(	hmmEnabledResID		: INTEGER;
											hmmEnabledIndex	: INTEGER;
										hmmDisabledResID	: INTEGER;
											hmmDisabledIndex: INTEGER;
										hmmCheckedResID 	: INTEGER;
											hmmCheckedIndex	: INTEGER;
										hmmOtherResID		: INTEGER;
											hmmOtherIndex	: INTEGER;
									);

				END;
				
	HMhdlgRecord = RECORD
					version		: INTEGER;
					offset		: INTEGER;
					options		: LongInt;
					theProc		: INTEGER;
					variant		: INTEGER;
					count		: INTEGER;
					list		: ARRAY [0..1000] of HMMessageArray;
				END;
				
	HMhdlgRecordPtr		= ^HMhdlgRecord;
	HMhdlgRecordHdl		= ^HMhdlgRecordPtr;
	
	HMhfdrRecord = RECORD
					version		: INTEGER;
					options		: LongInt;
					theProc		: INTEGER;
					variant		: INTEGER;
					count		: INTEGER;
					list		: ARRAY [0..1000] of HMMessageArray;
				END;
				
	HMhfdrRecordPtr		= ^HMhfdrRecord;
	HMhfdrRecordHdl		= ^HMhfdrRecordPtr;
