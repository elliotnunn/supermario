/*
	File:		International.r

	Contains:	Rez-format international resources for the System file

	Written by:	PKE	Peter Edberg
				SMB	Sue Bartalo

	Copyright:	© 1983-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<26>	 6/23/92	SMB		#1029263,<pke>: Set the 'smsfSynchUnstyledTE' bit in the Roman
									'itlb' script flags field to indicate that unstyled TE should
									synchronize the keyboard and font.
		<25>	 6/17/92	SMB		#1024950,<cv>: Since now using ParamText for About Keyboards
									dialog items, I added new STR# resources for each of the DITLs.
									Added a new DITL for the About Keyboards dialog to mention input
									methods (as well as keyboard layouts) when a 2-byte script is
									installed.  Updated all About Keyboards dialog items with
									Instructional Products suggestions.
									Added different default input method icons (again), a different
									default keyboard layout icon that is smaller and more rounded in
									appearance than the previous icon, and updated the US-System 6
									keyboard icons as well.
		<24>	 5/12/92	SMB		#1026017,<pke>: Added different default input method icon.
		<23>	 5/12/92	SMB		#1026017,<pke>: Added default input method icon. Updated System
									6 U.S. keyboard icons.  Removed '…' from About Keyboards
									strings.
		<22>	 4/16/92	JSM		Get rid of hasBalloonHelp (which was never used consistently
									anyway) and SixPack (which was never used at all) conditionals.
		<21>	 4/15/92	PKE		#1026910,<jh>: Delete unnecessary kcs8 resources, save 512
									bytes.
		<20>	  4/8/92	PKE		#1026586,<cv>: Add tentative keyboard icons for System 6 U.S.
									keyboard, ID 16383.
		<19>	 3/30/92	PKE		#1022875,<ha>: In KSWP, for Cmd-rightArrow, use new
									RomanIfOthers action instead of Roman (corresponds to KeyScript
									verb smKeyRoman instead of smRoman). This way, Cmd-rightArrow
									will not be stripped from the input queue unless it does
									something useful, which is only when there are other scripts
									installed besides Roman.
		<18>	12/11/91	SMB		#1014712 - For CubeE, in the About Keyboards… dialog for
									non-Roman scripts, the message about rotating keyboards within
									scripts should appear. Had to enlarge the DITL by 15 pixels for
									this.
		<17>	 9/25/91	PKE		For Cube-E (and Bruges): Update 'itlc' data to match latest
									template in SysTypes.r.
		<16>	 9/20/91	PKE		Remove CubeE conditional, since the new plan (according to
									Darin) is that everything for Cube-E goes in without
									conditionals. While I'm at it, remove some ">=$700"
									conditionals.
		<15>	 9/15/91	PKE		Oops, fix feature symbols to match latest BBSStartup.
		<14>	 9/15/91	PKE		Changes for Bruges/Cube-E: Turn off FontForce in default ‘itlc’
									resource. Fix ‘KSWP’ so arrow keys work on ADB keyboards too
									(Command-leftArrow selects system script, Command-rightArrow
									selects Roman).

		<13>	 2/26/91	PKE		smb,#perCindyR: Change resource name “US” to “U.S.”.
		<12>	 2/21/91	SMB		pke,#smb-2/21/91-1: Added smaller default keyboard icons
									(‘kcs#’,‘kcs4’,‘kcs8’ with ID -16491).
		<11>	 2/15/91	SMB		pke,#smb-2/15/91-1: Changing US 0 keyboard resources to smaller
									icons.
		<10>	  1/7/91	SMB		(JDR) approved the last change!
		 <9>	  1/7/91	SMB		Replacing the degree symbol as the default keyboard marker with
									null until it's demonstrated that a symbol should be used in the
									keyboard menu. Also removed the comment about the degree symbol
									from the DITL. Updated the DITL and DLOG for About Keyboards
									dialog.
		 <8>	12/14/90	PKE		(DC) Change abbreviated day and month strings in 'itl1' to match
									old 3-char form, so abbreviated dates don’t change on US system.
									Complete DITL text that explains switching among keyboards (the
									Cmd-Opt-space key combo needed to be finalized; other edits).
		 <7>	12/10/90	PKE		(VL) Add to KSWP resource: Cmd-Opt-space, for rotate to next
									keyboard in current script.
		 <6>	10/30/90	SMB		Changed data for US and default kcs’s.
		 <5>	  9/1/90	SMB		Added another About Keyboards DITL & DLOG for a system with only
									1 script. Also modified the other About msg.
		 <4>	 8/10/90	SMB		Added ‘About Keyboards…’ dialog (DITL & DLOG) and updated help
									messages.
		 <3>	  8/7/90	csd		Fixed problems caused by having double-/ comments in #defines.
		 <2>	  8/7/90	SMB		Added default keybd symbol and "About Keyboards…" to the
									Keyboard Menu STR# resource (although About is conditionalized
									out until later). Added balloon help 'hmnu' resource and indices
									for new help strings in Keyboard Menu STR# resource.
		 <1>	 7/23/90	PKE		New today, extracted from Sys.r. Added include of BalloonTypes.r
		 							since we’ll need it later. Note that real resource attributes are
									still set in Sys.r.

	Relevant comments from Sys.r (marker changed to <xNN>

	   <x163>	 7/17/90	PKE		‘kscn’ resource (see <x9.8>) has been superseded by keyboard icon
									suite (kcs#, etc. - see <x102>). Changed ‘kscn’ 0 back to ‘SICN’
									0 for backward compatibility.
	   <x158>	  7/7/90	PKE		Define “sysHeapFromNowOn” symbol to use as heap attribute for
									resources that did not have sysheap attribute in 6.x, but which
									should have sysheap attribute set for 7.0 and beyond (Darin gets
									credit for the symbol name). Use it for itlb, itl0, itl1, itl2,
									and itl4 (all ID=0) and PACK 6. For new 7.0 international
									resources, just set sysHeap attribute: itlm, kcs#, kcs4, kcs8,
									and STR# (-16491). This fixes a bug with saving handles to
									resources loaded at INIT time.
	   <x146>	 6/22/90	PKE		Updated 'itlm' data to use new script, language, and region
									names, and rearranged sorting order of scripts, languages, and
									regions.
	   <x121>	  6/1/90	SMB		Add STR# and default icon suites for Keyboard Menu.
	   <x117>	 5/29/90	PKE		Added bytes for valid style and alias style to ‘itlb’ type.
									Added list of additional separators to ‘itl1’. Both changes
									match new formats in SysTypes.r.
	   <x109>	 5/21/90	PKE		Added region code table to ‘itlm’ resource to match new format
									in SysTypes.r. Updated itlm data to use newer script, lang, and
									region codes (smTradChinese, etc.). Converted ‘itl1’ resource
									for 7.0 to new extended form with list of abbreviated day and
									month names, etc.
	   <x102>	 5/12/90	SMB		Adding the keyboard small icons (kcs#, kcs4, kcs8) for the US
									default keyboard (KCHR 0).
		<x97>	  5/2/90	PKE		Rearranged 'itlm' resource data to match new format.
		<x94>	 4/25/90	PKE		Add 'itlm' resource for multiscript mapping and sorting data.
		<x59>	 3/21/90	PKE		Added new info to US 'itlb' resource: font/size data, script
									local data size (needed for auto-initialization of script
									systems). Matches updated type in SysTypes.r. Also defined some
									FOND ID constants.
		<x19>	 1/17/90	PKE		Updated itlc resource data for new region code field added to
									itlc type in SysTypes.r.
		<x18>	 1/17/90	PKE		Updated itlc resource data to use MPW 3.1 itlc format, which is
									now in SysTypes.r.
	   <x9.8>	 9/18/89	PKE		For 7.0, changed type of keyboard/script small icon from 'SICN'
									to new 'kscn' to avoid resource ID conflicts.
				 3/12/89	PKE		Added script icon location at end of itlc (in previously
									reserved bytes)
				  3/8/89	PKE		Commented out itlr stuff

	To Do:
		• We can delete the kcs8 resources to save space, since they use the same colors as the
			kcs4 resources.

*/

#include "Types.r"
#include "SysTypes.r"

#include "BalloonTypes.r"

//__________________________________________________________________________________________________
// macros from Sys.r

#define IncSys Include $$Shell("ObjDir")
#define codeAs 'RSRC' (0) as

	#define sysHeapFromNowOn		sysHeap
	#define kKeybdMenuID			-16491												/* if this changes, update kKeyboardMenuID in ScriptPriv.a */
	#define kDefInputMethodIconID	-16492												/* if this changes, update kDefaultIMIconID in ScriptPriv.a <23> */

// STR# resource IDs
	#define	kKeybdMenuHelpID		kKeybdMenuID										/* <25> */
	#define kAboutKeybdRoman		kKeybdMenuID-1										/* <25> */
	#define kAboutKeybdIMs			kKeybdMenuID-2										/* <25> */
	#define	kAboutKeybdMulti		kKeybdMenuID-3										/* <25> */
	#define kKeybdMenuItemsID		kKeybdMenuID-4										/* if this changes, update kKeyboardMenuItemsID in ScriptPriv.a <25> */
	
// indices for strings in Keyboard Menu help STR# resource (kKeybdMenuHelpID)
	#define	kKMEnabledTitleIndex			1		/* Enabled keyboard Menu title			<2> */
	#define	kKMDisabledTitleIndex			2		/* Disabled keyboard Menu title			<2> */
	#define	kKMDisabledTitleModalIndex		3		/* Disabled kybd menu title w/ modal up	<2> */
	#define	kKMDisabledItemModalIndex		4		/* Disabled kybd menu item w/ modal up	<2> */
	#define	kKMEnabledItemIndex				5		/* Enabled keyboard Menu item 			<2> */
	#define	kKMDisabledItemIndex			6		/* Disabled keyboard Menu item			<2> */
	#define	kKMCheckedItemIndex				7		/* Checked keyboard menu item			<2> */
	#define	kKMOtherItemIndex				8		/* Default keyboard for a script		<2> */
	#define	kKMAboutItemIndex				9		/* About item 		 					<2> */
	#define	kKMDisabledAboutItemIndex		10		/* About item disabled					<2> */
	
	#define USname	"U.S."																	/*<13>*/
	
//__________________________________________________________________________________________________
// *************************************************************************************************
// IMPORTANT: the real attributes for all of the following are set in Sys.r.
// *************************************************************************************************

	resource 'itlc' (0, sysHeap, purgeable) {
		0,					// system script is Roman.
		2048,				// key cache size is 2K
		noFontForce,		// fontForce is off										<14>
		intlForce,			// intlForce is on
		noOldKeyboard,		// no old international keyboard.
		0,					// general flags (see smGenFlags info in ScriptEqu.a)
		40,					// keybd icon offset from end of menu bar				<x18>
		rightOffset,		// keybd icon at right end of menu bar					<x18>
		0,					// reserved for keybd icon data							<x18>
		verUS,				// preferred region code								<x19>
		directionLeftRight,	// default line direction								<17>
		$""					// reserved												<x18>
	};

	resource 'itlm' (0, sysHeap, purgeable) {
		$700,								// version
		$0000,								// format

		smUninterp,							// max script code for script->lang mapping
		langUnspecified,					// default lang code for unlisted scripts
		{									// script order and default lang table:
			smRoman,		langEnglish,
			smSlavic,		langCroatian,
			smGreek,		langGreek,
			smCyrillic,		langRussian,
			smGeorgian,		langGeorgian,
			smArmenian,		langArmenian,
			smArabic,		langArabic,
			smExtArabic,	langSindhi,
			smHebrew,		langHebrew,
			smGeez,			langAmharic,
			smDevanagari,	langHindi,
			smGurmukhi,		langPunjabi,
			smGujarati,		langGujarati,
			smOriya,		langOriya,
			smBengali,		langBengali,
			smTamil,		langTamil,
			smTelugu,		langTelugu,
			smKannada,		langKannada,
			smMalayalam,	langMalayalam,
			smSinhalese,	langSinhalese,
			smBurmese,		langBurmese,
			smKhmer,		langKhmer,
			smThai,			langThai,
			smLaotian,		langLao,
			smTibetan,		langTibetan,
			smMongolian,	langMongolian,
			smVietnamese,	langVietnamese,
			smTradChinese,	langTradChinese,
			smSimpChinese,	langSimpChinese,
			smJapanese,		langJapanese,
			smKorean,		langKorean,
			smRSymbol,		langHebrew,
			smUninterp,		langEnglish
		},

		langSimpChinese,					// max lang code for lang->script mapping
		smRoman,							// default script code for unlisted langs
		{									// lang order and parent script table:
			langEnglish,	smRoman,
			langFrench,		smRoman,
			langGerman,		smRoman,
			langItalian,	smRoman,
			langDutch,		smRoman,
			langSwedish,	smRoman,
			langSpanish,	smRoman,
			langDanish,		smRoman,
			langPortuguese,	smRoman,
			langNorwegian,	smRoman,
			langFinnish,	smRoman,
			langIcelandic,	smRoman,
			langMaltese,	smRoman,
			langTurkish,	smRoman,
			langLithuanian,	smRoman,
			langEstonian,	smRoman,
			langLettish,	smRoman,
			langLappish,	smRoman,
			langFaeroese,	smRoman,
			langCroatian,	smSlavic,
			langPolish,		smSlavic,
			langHungarian,	smSlavic,
			langGreek,		smGreek,
			langRussian,	smCyrillic,
			langArabic,		smArabic,
			langUrdu,		smArabic,
			langFarsi,		smArabic,
			langHebrew,		smHebrew,
			langHindi,		smDevanagari,
			langThai,		smThai,
			langTradChinese, smTradChinese,
			langSimpChinese, smSimpChinese,
			langJapanese,	smJapanese,
			langKorean,		smKorean,
		},

		verThailand,						// max region code for region->lang mapping
		langUnspecified,					// default lang code for unlisted regions
		{									// region order and parent lang table:
			verUS,			langEnglish,
			verBritain,		langEnglish,
			verAustralia,	langEnglish,
			verIreland,		langEnglish,
			verFrance,		langFrench,
			verFrBelgiumLux, langFrench,
			verFrCanada,	langFrench,
			verFrSwiss,		langFrench,
			verGermany,		langGerman,
			verGrSwiss,		langGerman,
			verItaly,		langItalian,
			verNetherlands,	langDutch,
			verSweden,		langSwedish,
			verSpain,		langSpanish,
			verDenmark,		langDanish,
			verPortugal,	langPortuguese,
			verNorway,		langNorwegian,
			verFinland,		langFinnish,
			verIceland,		langIcelandic,
			verMalta,		langMaltese,
			verTurkey,		langTurkish,
			verLithuania,	langLithuanian,
			verEstonia,		langEstonian,
			verLatvia,		langLettish,
			verLapland,		langLappish,
			verFaeroeIsl,	langFaeroese,
			verYugoCroatian, langCroatian,
			verPoland,		langPolish,
			verHungary,		langHungarian,
			verGreece,		langGreek,
			verRussia,		langRussian,
			verArabic,		langArabic,
			verPakistan,	langUrdu,
			verIran,		langFarsi,
			verIsrael,		langHebrew,
			verIndiaHindi,	langHindi,
			verThailand,	langThai,
			verTaiwan,		langTradChinese,
			verChina,		langSimpChinese,
			verJapan,		langJapanese,
			verKorea,		langKorean,
			verCyprus,		langUnspecified		// Hmm, 2 languages, which to use here?
		}
	};

#define Chicago 0										/* <x59> */
#define Geneva 3										/* <x59> */
#define Monaco 4										/* <x59> */

	resource 'itlb' (0, "Roman", sysHeapFromNowOn, purgeable) {
		0,			// itl0 ID
		0,			// itl1 ID
		0,			// itl2 ID
		$0107,		// script flags (see smScriptFlags info in ScriptEqu.a)
		0,			// itl4 ID
		0,			// optional itl5 ID (not used here).	<x163>
		0,			// language code
		0,			// numbers/dates
		0,			// KCHR ID
		0,			// ID of SICN or kcs#/kcs4/kcs8.		<x163>
		116,		// size of Roman local record, in bytes	<x59>
		Monaco,		// default monospace FOND ID			<x59>
		9,			// default monospace font size			<x59>
		Geneva,		// preferred FOND ID					<x59>
		12,			// preferred font size					<x59>
		Geneva,		// default small FOND ID				<x59>
		9,			// default small font size				<x59>
		Chicago,	// default system FOND ID				<x59>
		12,			// default system font size				<x59>
		Geneva,		// default application FOND ID			<x59>
		12,			// default application font size		<x59>
		Geneva,		// default Help Mgr FOND ID				<x59>
		9,			// default Help Mgr font size			<x59>
		$7F,		// valid styles for Roman				<x117>
		$02			// style set for alias = [italic]		<x117>
	};

	resource 'itl0' (0, USname, sysHeapFromNowOn, purgeable) {								/*<13>*/
		period, comma, semicolon, dollarsign, "\0x00", "\0x00",
		leadingZero, trailingZero, paren, leads, monDayYear,
		noCentury, noMonthLeadZero, noDayLeadZero, slash, twelveHour,
		noHoursLeadZero, minutesLeadZero, secondsLeadZero, " AM", " PM", ":",
		"\0x00", "\0x00", "\0x00", "\0x00", "\0x00", "\0x00", "\0x00", "\0x00",
		standard,
		verUs, 1,
	};
	resource 'INTL' (0, USname, purgeable) {												/*<13>*/
		period, comma, semicolon, dollarsign, "\0x00", "\0x00",
		leadingZero, trailingZero, paren, leads, monDayYear,
		noCentury, noMonthLeadZero, noDayLeadZero, slash, twelveHour,
		noHoursLeadZero, minutesLeadZero, secondsLeadZero, " AM", " PM", ":",
		"\0x00", "\0x00", "\0x00", "\0x00", "\0x00", "\0x00", "\0x00", "\0x00",
		standard,
		verUs, 1,
	};

	resource 'itl1' (0, USname, sysHeapFromNowOn, purgeable) {								/*<13>*/
		{ "Sunday"; "Monday"; "Tuesday"; "Wednesday"; "Thursday"; "Friday"; "Saturday"; },
		{
			"January"; "February"; "March"; "April"; "May"; "June";
			"July"; "August"; "September"; "October"; "November"; "December";
		},
		dayName, monDayYear, noDayLeadZero, 3,
		"", ", ", " ", ", ", "",
		verUs, 1,
		extFormat {
			$0700, $0001, 0,
			{},					// no extra day names (7 names are plenty, thank you)
			{},					// no extra month names
			{"Sun"; "Mon"; "Tue"; "Wed"; "Thu"; "Fri"; "Sat"},		// use old 3-char forms <8>
			{"Jan"; "Feb"; "Mar"; "Apr"; "May"; "Jun"; "Jul"; "Aug"; "Sep"; "Oct"; "Nov"; "Dec"},
			{"-"; "."}			// other reasonable date separators		<x117>
		},
	};
	resource 'INTL' (1, USname, purgeable) {												/*<13>*/
		{ "Sunday"; "Monday"; "Tuesday"; "Wednesday"; "Thursday"; "Friday"; "Saturday"; },
		{
			"January"; "February"; "March"; "April"; "May"; "June";
			"July"; "August"; "September"; "October"; "November"; "December";
		},
		dayName, monDayYear, noDayLeadZero, 3,
		"", ", ", " ", ", ", "",
		verUs, 1,
		DefaultReturn,
	};

	resource 'SICN' (0, sysHeapFromNowOn, purgeable) { {			// Roman script icon
		$"0000 0100 0380 07C0 0FE0 1FF0 3FF8 7FFC"
		$"3FF8 1FF0 0FE0 07C0 0380 0100 0000 0000"
	} };

	resource 'KSWP' (0, sysHeap) { {
		Rotate, 		$31, controlOff, optionOff, shiftOff, commandOn;	// space bar toggles script
		RotateKybd,		$31, controlOff, optionOn, shiftOff, commandOn;		// opt space bar toggles kybd	<7>
		System, 		$46, controlOff, optionOff, shiftOff, commandOn;	// Mac+ left arrow is system script
		RomanIfOthers,	$42, controlOff, optionOff, shiftOff, commandOn;	// Mac+ right arrow is Roman		<19>
		System, 		$7B, controlOff, optionOff, shiftOff, commandOn;	// ADB left arrow is system script	<14>
		RomanIfOthers,	$7C, controlOff, optionOff, shiftOff, commandOn;	// ADB right arrow is Roman			<14><19>
	} };

	resource 'kcs#' (0, sysHeap, purgeable) { {	/* array: 2 elements */							/* 	<11> */
		/* [1] */
		$"0000 0000 0000 0000 0000 FFFF AB01 FFFF"
		$"D501 FFFF AB01 FFFF 8001 FFFF 8001 FFFF",
		/* [2] */
		$"0000 0000 0000 0000 0000 FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	} };

	resource 'kcs4' (0, sysHeap, purgeable) {													/* 	<11> */
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0666 6663 3333 3330 0660 6060 0000 00C0"
		$"0666 6663 3333 3330 0606 0660 0000 00C0"
		$"0666 6663 3333 3330 0C00 0000 0000 00C0"
		$"0333 3333 3333 3330 0C00 0000 0000 00C0"
		$"0333 3333 3333 3330 0000 0000 0000 0000"
	};

#if 0
// don't need, has same colors as kcs4													/* 	<21> */
	resource 'kcs8' (0, sysHeap, purgeable) {
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"00EC ECEC ECEC ECD8 D8D8 D8D8 D8D8 D800"
		$"00EC EC00 EC00 EC00 0000 0000 0000 2B00"
		$"00EC ECEC ECEC ECD8 D8D8 D8D8 D8D8 D800"
		$"00EC 00EC 00EC EC00 0000 0000 0000 2B00"
		$"00EC ECEC ECEC ECD8 D8D8 D8D8 D8D8 D800"
		$"002B 0000 0000 0000 0000 0000 0000 2B00"
		$"00D8 D8D8 D8D8 D8D8 D8D8 D8D8 D8D8 D800"
		$"002B 0000 0000 0000 0000 0000 0000 2B00"
		$"00D8 D8D8 D8D8 D8D8 D8D8 D8D8 D8D8 D800"
	};
#endif

	resource 'kcs#' (16383, sysheap, purgeable) {										/*<20><25>*/
		{	/* array: 2 elements */
			/* [1] */
			$"0000 7FFC 6A04 7FFC 5604 7FFE 4102 7F32"
			$"4142 7F72 014A 014A 0132 0102 01FE",
			/* [2] */
			$"FFFE FFFE FFFE FFFE FFFF FFFF FFFF FFFF"
			$"FFFF FFFF FFFF 03FF 03FF 03FF 03FF 03FF"
		}
	};
	
	resource 'kcs4' (16383, sysheap, purgeable) {										/*<20><25>*/
		$"0000 0000 0000 0000 0666 6663 3333 3300"
		$"0660 6060 0000 0C00 0666 6663 3333 3300"
		$"0606 0660 0000 0C00 0666 666F FFFF FFF0"
		$"0C00 000F 0000 00F0 0333 333F 00FF 00F0"
		$"0C00 000F 0F00 00F0 0333 333F 0FFF 00F0"
		$"0000 000F 0F00 F0F0 0000 000F 0F00 F0F0"
		$"0000 000F 00FF 00F0 0000 000F 0000 00F0"
		$"0000 000F FFFF FFF0"
	};
	
#if 0
// don't need, has same colors as kcs4
	resource 'kcs8' (16383, sysheap, purgeable) {										/*<20><25>*/
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"00EC ECEC ECEC ECD8 D8D8 D8D8 D8D8 0000"
		$"00EC EC00 EC00 EC00 0000 0000 002B 0000"
		$"00EC ECEC ECEC ECD8 D8D8 D8D8 D8D8 0000"
		$"00EC 00EC 00EC EC00 0000 0000 002B 0000"
		$"00EC ECEC ECEC ECFF FFFF FFFF FFFF FF00"
		$"002B 0000 0000 00FF 0000 0000 0000 FF00"
		$"00D8 D8D8 D8D8 D8FF 0000 FFFF 0000 FF00"
		$"002B 0000 0000 00FF 00FF 0000 0000 FF00"
		$"00D8 D8D8 D8D8 D8FF 00FF FFFF 0000 FF00"
		$"0000 0000 0000 00FF 00FF 0000 FF00 FF00"
		$"0000 0000 0000 00FF 00FF 0000 FF00 FF00"
		$"0000 0000 0000 00FF 0000 FFFF 0000 FF00"
		$"0000 0000 0000 00FF 0000 0000 0000 FF00"
		$"0000 0000 0000 00FF FFFF FFFF FFFF FF"
	};
#endif

	// default small color icons for the scripts that don't include them					<x121>/* <12> */

	resource 'kcs#' (kKeybdMenuID, sysHeap, purgeable) { {	/* array: 2 elements */
		/* [1] */
		$"0000 0000 0000 0000 7FFE 4002 5552 4002"
		$"57EA 4002 7FFE",
		/* [2] */
		$"0000 0000 0000 7FFE FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF 7FFE"
	} };

	resource 'kcs4' (kKeybdMenuID, sysHeap, purgeable) {
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0DFF FFFF FFFF FFD0 0FCC CCCC CCCC CCF0"
		$"0FCF CFCF CFCF CCF0 0FCC CCCC CCCC CCF0"
		$"0FC3 CFFF FFFC FCF0 0FCC CCCC CCCC CCF0"
		$"0DFF FFFF FFFF FFD0"
	};

#if 0
// don't need, has same colors as kcs4													/* 	<21> */
	resource 'kcs8' (kKeybdMenuID, sysHeap, purgeable) {
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"00F9 FFFF FFFF FFFF FFFF FFFF FFFF F900"
		$"00FF 2B2B 2B2B 2B2B 2B2B 2B2B 2B2B FF00"
		$"00FF 2BFF 2BFF 2BFF 2BFF 2BFF 2B2B FF00"
		$"00FF 2B2B 2B2B 2B2B 2B2B 2B2B 2B2B FF00"
		$"00FF 2BD8 2BFF FFFF FFFF FF2B FF2B FF00"
		$"00FF 2B2B 2B2B 2B2B 2B2B 2B2B 2B2B FF00"
		$"00F9 FFFF FFFF FFFF FFFF FFFF FFFF F9"
	};
#endif

	resource 'kcs#' (kDefInputMethodIconID, sysHeap, purgeable) {								/* <23><24> */
		{	/* array: 2 elements */
		/* [1] */
		$"0000 0C3E 1212 2122 44C6 924A 4130 2490"
		$"1248 0924 1C92 3E02 7D24 3888 1050 0020",
		/* [2] */
		$"0C7F 1E7F 3F3F 7FFF FFFF FFFF FFFB 7FF8"
		$"3FFC 1FFE 3FFF 7FFF FFFE 7DFC 38F8 1070"
		}
	};

	resource 'kcs4' (kDefInputMethodIconID, sysHeap, purgeable) {							/* 	<23><24> */
		$"0000 0000 0000 0000 0000 FF00 00FF FFF0"
		$"000F CCF0 000F 00F0 00FC CCCF 00F0 00F0"
		$"0FCC CFCC FF00 0FF0 DCC3 CCFC CF00 F0F0"
		$"0FCC CCCF CCFF 0000 00FC CFCC FCCF 0000"
		$"000F CCFC CFCC F000 0000 FCCF CCFC CF00"
		$"000F 9FCC FCCF CCF0 00F9 99FC CCCC CCF0"
		$"0F99 9F0F CCFC CF00 00F9 F000 FCCC F000"
		$"000F 0000 0FCF 0000 0000 0000 00D0"
	};

#if 0
// don't need, has same colors as kcs4														/* 	<23><24> */
	resource 'kcs8' (kDefInputMethodIconID, sysHeap, purgeable) {
		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 FFFF 0000 0000 FFFF FFFF FF00"
		$"0000 00FF 2B2B FF00 0000 00FF 0000 FF00"
		$"0000 FF2B 2B2B 2BFF 0000 FF00 0000 FF00"
		$"00FF 2B2B 2BFF 2B2B FFFF 0000 00FF FF00"
		$"F92B 2BD8 2B2B FF2B 2BFF 0000 FF00 FF00"
		$"00FF 2B2B 2B2B 2BFF 2B2B FFFF 0000 0000"
		$"0000 FF2B 2BFF 2B2B FF2B 2BFF 0000 0000"
		$"0000 00FF 2B2B FF2B 2BFF 2B2B FF00 0000"
		$"0000 0000 FF2B 2BFF 2B2B FF2B 2BFF 0000"
		$"0000 00FF CBFF 2B2B FF2B 2BFF 2B2B FF00"
		$"0000 FFCB CBCB FF2B 2B2B 2B2B 2B2B FF00"
		$"00FF CBCB CBFF 00FF 2B2B FF2B 2BFF 0000"
		$"0000 FFCB FF00 0000 FF2B 2B2B FF00 0000"
		$"0000 00FF 0000 0000 00FF 2BFF 0000 0000"
		$"0000 0000 0000 0000 0000 F9"
	};
#endif

	resource 'STR#' (kKeybdMenuHelpID, sysHeap, purgeable) { {			// Balloon Help for Keyboard Menu					<25>
			// Keyboard, (Menu Title), Normal							(kKMEnabledTitleIndex)
		"Keyboard menu\n\nUse this menu to switch from one keyboard to another. This "
		"may also change the script you are using.";
		
			// Keyboard, (Menu Title), Dimmed							(kKMDisabledTitleIndex)
		"Keyboard menu\n\nUse this menu to switch from one keyboard to another. This "
		"may also change the script you are using. Not available because the "
		"keyboard layout cannot be changed.";

			// Keyboard, (Menu Title, Dimmed with Modal dialog up)		(kKMDisabledTitleModalIndex)
		"Keyboard menu\n\nThis menu is not available because it cannot be used with the dialog box "
		"on your screen.";

			// Keyboard, (Menu item, Dimmed with Modal dialog up)		(kKMDisabledItemModalIndex)
		"Keyboard menu\n\nThis item is not available because it cannot be used with the dialog box "
		"on your screen.";

			// Keyboard menu item, Normal/Selected						(kKMEnabledItemIndex)
		"Makes this keyboard layout active.";

			// Keyboard menu item, Disabled/Selected					(kKMDisabledItemIndex)
		"Makes this keyboard layout active. Not available now because the script of this keyboard "
		"cannot be used for this operation.";

			// Keyboard menu item, Checked								(kKMCheckedItemIndex)
		"Makes this keyboard layout active. Checked because this keyboard layout is now active.";

			// Keyboard menu item, Other								(kKMOtherItemIndex)
		"Makes this keyboard layout active. Marked because this keyboard layout is selected for "
		"the script system.";

			// Help, About keyboards..., Normal							(kKMAboutItemIndex)
		"Displays information about using hidden keystrokes to change your keyboard and script.";
	
			// Help, About help..., Dimmed								(kKMDisabledAboutItemIndex)
		"Displays information about using hidden keystrokes to change your keyboard and script. "
		"Not available because the About Keyboards dialog box is already open or because another "
		"dialog box is open.";
	} };

	resource 'STR#' (kAboutKeybdRoman, sysHeap, purgeable) { {			// About Keyboards for Roman-only system					<25>
		"The \0x12 indicates the active keyboard layout. To "
		"rotate to the next keyboard layout, press \0x11-Option-Space bar.";
	} };

	resource 'STR#' (kAboutKeybdIMs, sysHeap, purgeable) { {			// About Keyboards for system that includes 2-byte script	<25>
		"The \0x12 indicates the active keyboard layout or input method in "
		"the active script system.\0x0D\0x0D";

		"To rotate to the next keyboard layout or input method in the "
		"active script, press \0x11-Option-Space bar. To rotate to the "
		"preferred keyboard layout or input method in the next available "
		"script, press \0x11-Space bar." ;
	} };

	resource 'STR#' (kAboutKeybdMulti, sysHeap, purgeable) { {				// About Keyboards for multi-script w/o 2-byte script	<25>
		"The \0x12 indicates the active keyboard layout in the active script system.\0x0D\0x0D";
		
		"To rotate to the next keyboard layout in the active script, press "
		"\0x11-Option-Space bar. To rotate to the preferred keyboard layout "
		"in the next available script, press \0x11-Space bar.";
	} };

	resource 'STR#' (kKeybdMenuItemsID, sysHeap, purgeable) { {			// strings for menu items			<x121><25>
		"\0x00";														// marker for default keybd			<2><9>
		"About Keyboards";												//									<2><23>
//		"Next Script";													// currently not used
//		"Next Keyboard in Script";										// currently not used
	} };
		
	resource 'hmnu' (kKeybdMenuHelpID, sysheap) {						// balloon help strings for keyboard menu	<2>
		HelpMgrVersion,								/* Help Version */
		0,											/* options */
		0,											/* theProc */
		0,											/* variant */
		HMStringResItem {												// use missing msg
				kKeybdMenuHelpID,kKMEnabledItemIndex,	/* enabled msg */
				kKeybdMenuHelpID,kKMDisabledItemIndex,	/* disabled msg */
				kKeybdMenuHelpID,kKMCheckedItemIndex,	/* checked msg */
				kKeybdMenuHelpID,kKMOtherItemIndex		/* other marked msg : default kchr for a script */
			},
		{
			HMStringResItem {											// keyboard menu title
				kKeybdMenuHelpID,kKMEnabledTitleIndex,
				kKeybdMenuHelpID,kKMDisabledTitleIndex,
				kKeybdMenuHelpID,kKMDisabledTitleModalIndex,
				kKeybdMenuHelpID,kKMDisabledItemModalIndex
			},

			HMStringResItem {											// about keyboards… menu item
				kKeybdMenuHelpID,kKMAboutItemIndex,
				kKeybdMenuHelpID,kKMDisabledAboutItemIndex,
				0,0,
				0,0
			},

			HMSkipItem {												// disabled line
			},
		}
	};


	resource 'DLOG' (kAboutKeybdMulti, purgeable) {						// <4><18><25>
		{58, 16, 264, 484},
		dBoxProc,
		visible,
		noGoAway,
		0x0,
		kAboutKeybdMulti,
		"About…",
		alertPositionMainScreen
	};
	
	resource 'DLOG' (kAboutKeybdRoman, purgeable) {						// <5><25>
		{58, 18, 180, 458},
		dBoxProc,
		visible,
		noGoAway,
		0x0,
		kAboutKeybdRoman,
		"About…",
		alertPositionMainScreen
	};
	
	resource 'DLOG' (kAboutKeybdIMs, purgeable) {						// <25>
		{58, 16, 264, 484},
		dBoxProc,
		visible,
		noGoAway,
		0x0,
		kAboutKeybdIMs,
		"About…",
		alertPositionMainScreen
	};
	
	resource 'DITL' (kAboutKeybdMulti, purgeable) {						// <4><18><25>
		{	/* array DITLarray: 4 elements */
			/* [1] */
			{175, 367, 193, 447},
			Button {
				enabled,
				"OK"
			},
			/* [2] */
			{46, 15, 85, 447},
			StaticText {												// edited <8>
				disabled,
				"^0"													// now uses ParamText <25>
			},
			/* [3] */
			{96, 15, 159, 447},
			StaticText {												// edited <8>
				disabled,												
				"^1"													// now uses ParamText <25>
			},
			/* [4] */
			{14, 15, 46, 447},
			StaticText {
				disabled,
				"About Keyboards"										//	<23>
			}
		}
	};
	
	resource 'DITL' (kAboutKeybdRoman, purgeable) {						// <5>
		{	/* array DITLarray: 3 elements */
			/* [1] */
			{87, 342, 105, 422},
			Button {
				enabled,
				"OK"
			},
			/* [2] */
			{44, 17, 82, 422},
			StaticText {
				disabled,
				"^0"													// now uses ParamText <25>
			},
			/* [3] */
			{14, 17, 44, 422},
			StaticText {
				disabled,
				"About Keyboards"										//	<23>
			}
		}
	};
	
	resource 'DITL' (kAboutKeybdIMs, purgeable) {						// <25>
		{	/* array DITLarray: 4 elements */
			/* [1] */
			{175, 367, 193, 447},
			Button {
				enabled,
				"OK"
			},
			/* [2] */
			{46, 15, 85, 447},
			StaticText {													
				disabled,
				"^0"													// now uses ParamText <25>
			},
			/* [3] */
			{96, 15, 159, 447},
			StaticText {													
				disabled,													
				"^1"													// now uses ParamText <25> 
			},
			/* [4] */
			{14, 15, 46, 447},
			StaticText {
				disabled,
				"About Keyboards"					
			}
		}
	};
