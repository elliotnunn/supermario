/*
	File:		Sys.r

	Contains:	resources for the System file

	Written by:	Carl Hewitt

	Copyright:	© 1983-1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM3>	 8/25/93	BH		Added dialogs for use by manual-eject drive software. This may
									not be the most appropriate place for them, but they need a home
									for now.
	   <SM2>	 3/24/93	dwc		Changed to include AppleTalk resource AT58 instead of AT57.
	   <290>	 1/23/92	hsK		Deleted 'dtab' resource which is no more used by the Dictionary
									manager.
	   <289>	 1/18/92	DTY		Lock the print driver so that it will get loaded low in the
									System heap to reduce fragmentation.
	   <288>	 1/17/92	pvh		Add in AppleTalk version 57 resources in one happy .rsrc file.
									Includes all drivers and their shell thingies.
	   <287>	 1/14/92	pvh		Add Zydeco's real name to STR# -16395 so we can take out an
									Installer action item which modifies it on the fly.
	   <286>	  1/3/92	JSM		#1017240: Add STR -16391 with name of TrueType font cache file.
	   <285>	12/16/91	DTY		Don’t mark 'WDEF' 0 as purgeable. (Ask the 7 Up guys for the
									reason…)
	   <284>	 12/4/91	hsK		Changed the 'dtab' resource (dictionary attribute table)
									according to the new definition.
	   <283>	11/21/91	YK		Removed InputUtils.
	   <282>	11/19/91	DTY		Use a real date in the 'gbly' resource.
	   <281>	11/15/91	DTY		Change the resource ID of the 'gbly' and 'lodr' resources to
									match the ones in the Gibbly ERS.
	   <280>	11/12/91	DTY		Add a 'ptbl' resource which loads all the linked patches.
									Change resource type of linked patch loader to 'lodr'.
	   <279>	11/11/91	DTY		Rez LinkedPatchLoader.a.rsrc into 'pldr' 0.
	   <278>	 11/8/91	DTY		Added a temporary 'gbly' resource to the System until we can get
									Pete to add it to the BuildDisks script.
	   <277>	 11/2/91	DTY		FPHW.rsrc and ElemsHW.rsrc are compiled into 'RSRC' types
									instead of 'PACK's now. Change the IncSys for SANE to reflect
									this.
	   <276>	 11/1/91	JSM		Take out SANE stuff conditionalized ForROM, this file is not
									used in ROM builds.
	   <275>	10/28/91	SAM/KSM	Rolled in Regatta changes.
										Changed machine names and built-in video strings to reflect the
											offical product names.  We're GM!
										Changed seeding machine names to not include the code names.
										Changed ppat (16) to have the correct number of colors to
											satisfy General.  Changed all the new product names to strings
											that have the same length as the "real" ones will.
										Changed the Macintosh Zydeco name string to be the correct
											length for the install script.  (djw) re-worded bad battery
											warning string -16515,-16516.
										(ag) Revised low power strings again.
										Changed the IIci ROvrs for AppleTalk to appear as they did when
											7.0 shipped. Revised 'PAT ' (18).
										Adding strings for Eclpise33 (Zydeco)
										ROvring MPP driver on 67C machines (DRVR 9)
										Whats in a semi-colon? Huh?
										Removing all of the LocalTalk ROvrs and adding two IOP ROvrs
											(for 67C)
										Revised the Bad Battery strings.
										ROvr'ed atlk (1,3) and ltlk (0,1,2,3) on $67C. Adding bad
											battery strings -16514 & -16515. Added Portable desktop pat #18.
											Added EDiskShutdownPtch.a as an IncSys INIT (12). Included a
											placeholder 'ptch' (50).
										(ag) changed wording on STR#-16512. Changed network warning to
											default to sleep.
										Added atlk#10 to the list of 67C ROvrs.  Added some seeding
											support.
										Split off from 7.0 GM sources. Added Built-In video names for
											Spike, Eclipse, Tim, and Apollo.  Added “About this Macintosh…”
											(and responder) strings for Spike, Eclipse, Tim, and Apollo.
											Now including FPHW.rsrc & ElemsHW.rsrc (backpatching SANE) into
											the System build.  Cheesy software gets a cheesy Color Icon.
											Default 7.0 (high radiation) desktop PixPattern has been tweeked
											not not raditate as much.  Removed all the AppleTalk rsrcs from
											the ROVr list for IIci machines (except for lmgr & init 18) cuz
											the new disk based stuff will correctly determine which ATalk
											stuff is newer (ROM vs System).
	   <274>	10/23/91	JH		Changed Dictionary dimg resource to have an ID of -16385 which
									was officially obtained from the meanies.
	   <273>	10/23/91	YK		Added InputUtils code.
	   <272>	10/14/91	JSM		Don’t use hasDictionaryMgr conditional.
	   <271>	10/11/91	JH		Added DictionManager code and dtab resource.
	   <270>	 10/2/91	JSM		Use StandardFile.rsrc instead of StandardFile3.rsrc.
	   <269>	 9/19/91	PKE		(for Ian Hendry) For Cube-E, add 3 PICTs for use by Clock
									Manager (needed for Alarm Clock DA).
	   <268>	 9/17/91	DTY		Added 'font' to the 'fld#' resource.  This lets the Folder
									Manager know about the new Fonts Folder.
	   <267>	 6/11/91	gbm		Take out conditional for Regatta
	   <266>	 5/30/91	DTY		New 'pixs' resources for tuned up scroll bar CDEF. Make sure if
									you build with this version of Sys.r that you have version 36 of
									ScrollBarCDEF.a as well!
	   <265>	  4/2/91	SAM		Added Regatta conditionalized strings and cicn.
	   <264>	 3/23/91	EMT		Tune NBP Timeout and Retry parameters for PPC.
	   <263>	 3/21/91	DTY		Cindy got mad at me because the year and Apple Computer, Inc.
									are backwards from the “official” copyright format. She’s still
									going to get mad at me for not doing this for c3, but at least
									it’s getting done.
	   <262>	 3/20/91	ngk		Add back popup triangle PICT that Darin removed.
	   <261>	 3/13/91	DTY		Updated 'vers' 1 resource’s copyright notice.
	   <260>	  3/4/91	dba		dty: get rid of SysVers conditionals and the popup triangle
									PICT; get rid of sysHeapFromNowOn since it was for 6.X vs. 7.0
									convenience
	   <259>	 2/26/91	PKE		smb,#perCindyR: Change resource name “US” to “U.S.”.
	   <258>	 2/23/91	PKE		ngk,#perCSD: Clean up international resource names.
	   <257>	 2/22/91	ag		djw: hjr: Rolled in backlight support. new low power dialog
									string and backlight driver resource.
	   <256>	 2/21/91	csd		gbm, #GangOfFive: Added Geneva 12 to the System so that we have
									it on Install 1 for the Installer on the Plus.
	   <255>	 2/15/91	csd		dty,#81464: Changed the ppt# 0 resource to include the 7.0
									pattern set. Also changed ppat 16 (the default color desktop) to
									use a higher contrast dither which allows DragGrayOutline to
									show up on 24-bit color screens. Added the FRSV resource back in
									to be compatible with Font/DA Mover.
	   <254>	  2/1/91	RLC		<ngk> Add an IncSys to get 'dctb' -5696 and 'PICT' -5694 in the
									new design for the About Help… dialog.
	   <253>	 1/22/91	JNG		<BBH> Added a Dialog for the CommToolbox choose package.
	   <252>	 1/21/91	mjq		Include AppleTalk from the ObjDir folder instead of from the
									Misc folder.
	   <251>	 1/18/91	stb		& RLC; change dbex id
	   <250>	 1/18/91	stb		& RLC; add 'dbex' resource, the presence of which permits the
									user to disable extensions at startup time by holding down the
									shift key.
	   <249>	 1/17/91	csd		& KSM: Changed the name of the “Users & Groups” file to “Users &
									Groups Data File” so it’s different than the name of the control
									panel.
	   <248>	 1/12/91	kaz		Removing purge bit on popupCDEFMDEF 'proc'.
	   <247>	 1/12/91	gbm		<dba> Take out the obviously untested (wrong filenames and
									missing quotes) changes for AppleTalk from <245> until the
									makefile dependencies are fixed by mjq. Also fixed some typos
									from <246>, wondering the whole time why Eric would check this
									in untested (since <245> broke the build).  How the hell are we
									ever going to get b4 out the door if people aren’t testing their
									changes?!?
	   <246>	 1/11/91	EMT		<VC> change help strings for Prompt for user dialog
	   <245>	 1/11/91	mjq		Update AppleTalk resource locations for new AppleTalk Source
									build.
	   <244>	  1/9/91	EMT		<VC> Add dctb resource so that the promptforuser dialog looks
									nice.
	   <243>	  1/9/91	JDR		(KIP) Sound Manager's snth resources are now marked "locked."
	   <242>	  1/4/91	EMT		<VC> Trim the size of some dialog items as appropriate in the
									PromptForUser dialog.
	   <241>	12/27/90	csd		Changed the bundle to add a new icon (-16509) for the clipboard
									file and changed the FREF for files of type 'CLIP' to use it.
	   <240>	12/20/90	RLC		<VL> Add the IncSys's to get the Finder's Help File icon data
									back into this file.
	   <239>	12/15/90	csd		per Pace Bonner: add a STR to the System which contains the name
									of the file VM uses as a backing store.
	   <238>	12/14/90	stb		& gbm; remove AppleTalk 'STR '
	   <237>	12/13/90	JSM		<VL> Update help strings for PPC user dialog.
	   <236>	12/13/90	EMT		<JSM> Add param text to prompt for PPC user dialog
	   <235>	12/11/90	stb		& gbm; add 'STR ' -16507 for net booting
	   <234>	 11/9/90	VL		(dc) Deleted entries added in <228> as we don't need to draw
									titles in TrueGray anymore.
	   <233>	 11/4/90	dba		include Scheduler and DAHandler separately so we don’t waste so
									much space on people’s disks
	   <232>	 11/1/90	fjs		add color icons for the modem port and printer port
	   <231>	10/31/90	csd		Added 'atlk' 10 to the Mac32 Rov#.
	   <230>	10/31/90	RLC		Change help 'ICN#' resource ID to be -5696 instead of -5695.
									[This has been incorrect for quite some time.]
	   <229>	10/30/90	csd		Changed the directory where we get the ProcessMgr since we build it now.
	   <228>	10/30/90	VL		(jsm) Added new entries to ROv# resources to override
									RoundedWDEF (1) in SE, II, Portable and ci ROMs.
	   <227>	10/19/90	fjs		remove ppat 11, 12, 13
	   <226>	10/10/90	gbm		Mike really did forget...
	   <225>	 10/9/90	jmp		Oops, I forgot to rename various references to
									VideoExtensionIIci.p when I changed the name to
									BuiltInVideoExtension.p.
	   <224>	 10/8/90	VL		Changed 'pixs' resources so that it can handle 1-bit pixmap data
									by adding an additional field to contain the number of bits per
									pixel. This reduces the size of pixs significantly.
	   <223>	 10/6/90	jmp		Added a space in front of the “Memory Allocation for Built-In
									Video” strig so that it would look centered, and made the
									static-text item associated with this string 5 pixels larger to
									accomodate my change in the version number from 1.0 to 1.0.1
									(i.e., had to change VidExtVers in Build).
	   <222>	 9/27/90	csd		Added Responder.
	   <221>	 9/25/90	csd		Marked the 'pixs' and 'pmap' resources preload unti the defprocs
									can deal with not being able to get them.
	   <220>	 9/25/90	KIP		Convert Sound Mgr. to a linked patch. Also change ProcHelper and
									DispatchHelper back to a linked patch.
	   <219>	 9/21/90	gbm		Fixed spelling of purgeable.
	   <218>	 9/21/90	DDG		Used the new conditional for picture utilities.
	   <217>	 9/16/90	csd		Include the snd resource from PictWhapSound.rsrc.
	   <216>	 9/16/90	VL		Replaced old-style 'wctb' and 'cctb' with 7.0 extended ones.
	   <215>	 9/15/90	csd		Changed “A/ROSE Prep” to just “A/ROSE”.
	   <214>	 9/14/90	kaz		Marking CommToolbox 'cmtb' and 'proc' code resources as locked
									to load 'em in low in the system heap.
	   <212>	 9/10/90	EMT		Added BubbleHelp Strings for PPCToolBox PromptForUser Dialog
	   <211>	  9/7/90	Lai		Add include resources for AppleEvent Manager
	   <210>	  9/4/90	EMT		Change structure of 'ppci' resource
	   <209>	  9/3/90	gbm		add 'dcmp'(2), more commonly known as GreggyBits
	   <208>	  9/1/90	PKE		Add IncSys statements for Keyboard menu DLOG and DITL resources.
									They were not being included here, which is why they were not
									getting in.
	   <207>	 8/30/90	dba		put back in the resource override for WDEF 0, since we added a
									new window variant; fix Geneva back up
	   <206>	 8/29/90	stb		change FRSV to resf for 7.0 and later
	   <205>	 8/28/90	VL		Changed the size of the text box for the notification dialog so
									that it can accommodate 255 characters.
	   <204>	 8/27/90	RB		add TrueType glyphname resource at Font Mgr
	   <203>	 8/20/90	dvb		Remove "PixPat4Colors.r", 'coz colorpicker no longer needs it.
	   <202>	 8/20/90	RLC		Add back the IncSys's for the color help menu icons.
	   <201>	 8/20/90	gbm		mark a few thing not purgeable... I went a little purge crazy
									there...
	   <200>	 8/19/90	gbm		mark a few more things purgeable, take steps to speed up the
									build (a little)
	   <199>	 8/19/90	dba		got rid of old SixPack conditionals since we don’t build system
									versions older than SixPack; got rid of the forAppleTalk20
									switch since it is always true these days; made new defines so
									we can use dialog centering, and it will be ignored on 6.0
									builds; overhauled ROM resource overrides (still overriding KCHR
									0 too often); got rid of SwapHMMUPatch, SwapPMMUPatch,
									SonyEjectPatch, and FormatPatch since they are all now linked
									patches; set sysHeap bit on more resources, including all Comm.
									Toolbox resources; include the whole AliasMgr.rsrc file instead
									of including the resources one at a time, since it builds faster
									that way; used auto-centering on various dialogs and alerts, and
									made the corresponding changes to the code (got rid of centering
									code); removed the name from the Terminal Click sound, so it
									will not appear in lists of eligible beep sounds; declare the
									FOND for Geneva explicitly, instead of including it from
									SystemFonts.rsrc, also include Geneva 9 only, since Geneva 12 is
									in all ROMs; put the color apple cicn back in for 6.X builds, so
									6.X will still have a color apple menu title; formatted many
									color resources so that you can see what they are (check out the
									scroll bar arrows and the color apple); change the Parity alert
									button from Restart to Shut Down, since Restart just gives you
									the same alert again
	   <198>	 8/17/90	jmp		It was decided today that the LC video ROM would be named
									(internally) Macintosh B Built-In Video -- that there would be
									no Macintosh C Built-In Video (distinguishing V8 LCs from non-V8
									LCs). So, I removed the referenced to C.
	   <197>	 8/17/90	csd		Added more machine names to the STR# to correspond to the
									Gestalt machine IDs.
	   <196>	 8/17/90	DTY		Removed IncSys for Slot Manager since it’s a linked patch now.
	   <195>	 8/14/90	jmp		Added the "Macintosh II C Built-In Video" 'card' resource for
									V8, and the "Macintosh II C Built-In Video" 'STR#' translation
									resource.  This is for LC-V8s (Rexes?).  Also, changed the
									translated name from "Macintosh II Built-In Video" to "Macintosh
									Built-In Video" for Rexes only since Rexes are not going to be
									Macintosh II’s by name.
	   <194>	 8/14/90	DTY		Removed ptch 0 since TextEdit is a linked patch now.
	   <193>	 8/10/90	dba		add new strings for AppleTalk and A/ROSE, get rid of INIT 22
	   <192>	 8/10/90	S		Modified A String in STR# -16409.
	   <191>	 8/10/90	S		Added String resources to STR# -16409 resource in PPCToolBox.
	   <190>	  8/9/90	VL		Removed obsolete 'pixs' for the color scroll bar.
	   <189>	  8/9/90	DC		replace color apple cicn (for apple menu) with icon suite
	   <188>	  8/8/90	SAM		Including ptch 45 DispatchHelper & ProcHelper in the System.
									•••---> Temporary <--- ••• Remove this when sound becomes an
									Lptch.
	   <187>	  8/7/90	DTY		Removed ptch for ADBMgrPatch since it’s been converted to a
									linked patch.
	   <186>	  8/7/90	csd		Added some more resources to the Rov#s for Appletalk.
	   <185>	  8/7/90	SMB		Added IncSys for keyboard menu 'hmnu' found in
									international.rsrc.
	   <184>	  8/7/90	RLC		Remove 'ICN#' (-16490) warning from BalloonHelp.
	   <183>	  8/7/90	KIP		Change Sound Input driver res id to -16499.
	   <182>	  8/6/90	VC		Changed the bad-user and bad-pasword alert strings in the PPC
									log-in dialog.
	   <181>	  8/6/90	VL		Updated pixs resources for color scroll bars.
	   <180>	  8/5/90	csd		Include Geneva 9 from SystemFonts.rsrc since the Plus doesn’t
									have it in ROM.
	   <179>	  8/5/90	RLC		Change IncludeColorHelpIcons to zero to skip inclusion of 2
									color ics's.
	   <178>	  8/4/90	DTY		Got rid of 'pixs' resources -14336 & -14332 from Window Manager;
									they’re no longer used by the Standard WDEF. Modified 'pixs'
									-14335 & 'pixs' -14334.
	   <177>	  8/2/90	JSM		Read in all PPC Browser resources from a separate file.
	   <176>	 7/30/90	DDG		Updated the build commands for picture utilities.
	   <175>	 7/30/90	RLC		Remove some IncSys's to get rid of unused icons.
	   <174>	 7/30/90	dnf		Remove IncSys for File Manager and Btree Manger ptch resources
	   <173>	 7/24/90	RLC		Change references to BalloonTypes.r to look directly in
									{RIncludes} now.
	   <172>	 7/24/90	KIP		NEEDED FOR SIXPACK: Create 'proc' resources from
	   								SoundPFDProc.rsrc and SoundInputProc.rsrc.
	   <171>	 7/23/90	PKE		Moved the ‘resource’ and ‘data’ definitions for international
									resources that were defined here into a new file,
									International.r; replaced with IncSys statements here.
	   <170>	 7/23/90	dba		get rid of ptch 1
	   <169>	 7/23/90	dba		get rid of ptch 25 for 7.0; also get rid of ptch 16 for good
	   <168>	 7/20/90	DTY		Remove Bass 'ptch' resources since it’s a linked patch now.
	   <167>	 7/20/90	dba		get rid of PTCH -1 for 7.0
	   <166>	 7/20/90	csd		Added even more resources to the Mac32 ROv# for Appletalk.
	   <165>	 7/19/90	CCH		NEEDED FOR SIXPACK: Removing include of HwPriv ptch, since it is
									now a linked patch.
	   <164>	 7/19/90	PP		Remove obsolete AliasMgr DLOG and DITL resource         for
									Already Connected To Server dialog.
	   <164>	 7/19/90	PP		Remove obsolete AliasMgr DLOG and DITL resource for Already
									Connected To Server dialog.
	   <163>	 7/17/90	PKE		‘kscn’ resource (see <9.8>) has been superseded by keyboard icon
									suite (kcs#, etc. - see <102>). Changed ‘kscn’ 0 back to ‘SICN’
									0 for backward compatibility.
	   <162>	 7/16/90	csd		Added four more resources needed by Appletalk to the Mac32 ROv#.
	   <161>	 7/16/90	DDG		NEEDED FOR SIXPACK: Added the button CDEF back to the ‘ROv#’
									list for the Mac SE, since my patch didn’t work !! Maybe later I
									will come back and fix this patch if we REALLY need the ≈1000
									bytes that it saves.
	   <160>	 7/11/90	gbm		Fixed Darin “BoneHead” Adler’s glaring errors.  Maybe someday
									he’ll learn to build before he checks in files.
	   <159>	 7/11/90	dba		improve inclusion of ProcessMgr and VM, set system heap bit a
									lot more
	   <158>	  7/7/90	PKE		Define “sysHeapFromNowOn” symbol to use as heap attribute for
									resources that did not have sysheap attribute in 6.x, but which
									should have sysheap attribute set for 7.0 and beyond (Darin gets
									credit for the symbol name). Use it for itlb, itl0, itl1, itl2,
									and itl4 (all ID=0) and PACK 6. For new 7.0 international
									resources, just set sysHeap attribute: itlm, kcs#, kcs4, kcs8,
									and STR# (-16491). This fixes a bug with saving handles to
									resources loaded at INIT time.
	   <157>	  7/3/90	fjs		Die Hard II: Die Harder
	   <156>	  7/2/90	DTY		Removed IncSys for Resource Manager Extensions, since they’re in
									a linked patch.
	   <155>	  7/2/90	csd		Added resources needed by AppleTalk to the Mac32 ROv#.
	   <154>	  7/1/90	JSM		Change 'char' to 'TEXT' in the 'rtt#' resource.
	   <153>	  7/1/90	JSM		Updated some data types for SnarfMan in the 'rtt#' resource.
	   <152>	 6/29/90	DDG		NEEDED FOR SIXPACK: Removed the button CDEF from the ‘ROv#’ list
									for the Mac SE, because we now have a patch to fix the SE case.
	   <151>	 6/27/90	DC		Included label resources (IconUtils.rsrc)
	   <150>	 6/26/90	DTY		Removed IncSys for NotificationMgr, since it’s now a linked
									patch.
	   <149>	 6/26/90	JSM		Don't include 'ckid' resources from AppleTalk.rsrc and
									ADSP.rsrc.
	   <148>	 6/26/90	csd		get AppleTalk and ADSP resources from their own .rsrc files
									instead of System7.0Resources. Added a default, empty machine
									name STR .
	   <147>	 6/25/90	DTY		Removed 'ptch' for ScrollSpeedFix.a.rsrc since it’s now in an
									'lpch'.
	   <146>	 6/22/90	PKE		Updated 'itlm' data to use new script, language, and region
									names, and rearranged sorting order of scripts, languages, and
									regions.
	   <145>	 6/21/90	PK		Move disk init resources to DiskInit.rsrc
	   <144>	 6/21/90	DTY		Removed rules for Shutdown Manager INIT. (Shutdown Manager now a
									lpch in all systems.)
	   <143>	 6/20/90	ngk		Add PictButtonCDEF for system 7.0
	   <142>	 6/19/90	JSM		Center PPCBrowser dialog in the DLOG resource.
	   <141>	 6/19/90	VL		Remove IncSys for MiscPatches since MiscPatches is now a linked
									patch.
	   <140>	 6/19/90	DVB		Make 'general' patterns use clut8 colors.
	   <139>	 6/18/90	RLC		Remove a bunch of IncSys's for some BalloonPack resources.
	   <138>	 6/18/90	JBS		NEEDED FOR SIXPACK-Merged ci/erickson resources together with
									elsie resources.
	   <137>	 6/16/90	PK		Change Disk Init's DITL: support for sparing, UserItem to redraw
									button hilites, uses dialog mgr approved offset (not 1000) to
									show/hide items.
	   <136>	 6/15/90	PP		Remove 'SelectAlias' related obsolete resources from AliasMgr.
	   <135>	 6/15/90	DDG		Added the button CDEF to the ‘ROv#’ lists to fix a small bug in
									drawing text in buttons.
	   <134>	 6/14/90	JBS		NEEDED FOR SIXPACK-Removed Elsie Extension, Mapped all names to
									the same
	   <133>	 6/12/90	JSM		PPC Toolbox is now a linked patch. Get rid of IncSys ptch 33.
	   <132>	 6/12/90	PKE		NEEDED FOR SIXPACK: Override IIci and Portable ROM versions of
									MDEF 0 for SixPack as well as BigBang.
	   <131>	 6/11/90	DDG		NEEDED FOR SIXPACK: Changed the ID number of the resources used
									by the Disk Initialization package back to -6047 for 6.x systems.
									The ID number for the 7.0 system was NOT changed.
	   <130>	 6/11/90	DDG		NEEDED FOR SIXPACK: Changed the Macintosh XO string to just say
									macintosh. This means that we can do seed versions that can’t
									contain real product names without the extra step of changing all
									these little strings. Right before XO is actually shipped, we
									should change this to the real name of the computer.
	   <129>	 6/10/90	fjs		add a picture for drawing gray lines in dialogs and several
									pixel patterns for use as substitutes for the original QuickDraw
									patterns available as globals
	   <128>	 6/10/90	JSM		Include all the SnarfMan resources from just SnarfMan.a.rsrc.
	   <127>	  6/7/90	EMT		Layer Manager is now a linked patch. Get rid of IncSys ptch 17.
	   <126>	  6/7/90	VL		Help Mgr is now a linked patch. Get rid of IncSys ptch 28.
	   <125>	  6/7/90	DC		Modified existing ALRT, DLOG and WIND templates to for
									auto-positioning
	   <124>	  6/6/90	DTY		Add switch launch prevention 'proc'
	   <123>	  6/6/90	csd		add FREF for new System file type
	   <122>	  6/3/90	csd		Added new FRSV resource; changed System BNDL to use new System
									file icon.
	   <121>	  6/1/90	SMB		Add STR# and default icon suites for Keyboard Menu.
	   <120>	  6/1/90	RLC		Fix the bugs resulting from the Linker unable to do: Link -ra
									resSysheap,resLocked.
	   <119>	  6/1/90	RLC		Fixed IncSys includes for balloon pack resources.
	   <118>	 5/30/90	JBS		NEEDED FOR SIXPACK-Changed Elsie resources
	   <117>	 5/29/90	PKE		Added bytes for valid style and alias style to ‘itlb’ type.
									Added list of additional separators to ‘itl1’. Both changes
									match new formats in SysTypes.r.
	   <116>	 5/29/90	JSM		Update PPCBrowser resources for new interface.
	   <115>	 5/29/90	JBS		NEEDED FOR SIXPACK-Fixed Elsie video extension resource
									numbering
	   <114>	 5/29/90	DDG		NEEDED FOR SIXPACK: Changed all the sixpack conditionals from
									six-point-oh-seven to six-point-oh-six.
	   <113>	 5/25/90	NC		Added SnthLoading.r to System File.
	   <112>	 5/24/90	kaz		Removed Choose.r 'finf'
	   <111>	 5/23/90	csd		Changed the ctSeeds of cluts 5 and 9 so that they don’t match
									the standard system cluts.
	   <110>	 5/22/90	kaz		Changed StandardNBP.r 'STR#' to 'PICT'
	   <109>	 5/21/90	PKE		Added region code table to ‘itlm’ resource to match new format
									in SysTypes.r. Updated itlm data to use newer script, lang, and
									region codes (smTradChinese, etc.). Converted ‘itl1’ resource
									for 7.0 to new extended form with list of abbreviated day and
									month names, etc.
	   <108>	 5/18/90	csd		added clut 5 and clut 9 for drawing dimmed icons.
	   <107>	 5/18/90	fjs		mouse mapping init is banished to 6.x only
	   <107>	 5/18/90	fjs		mouse mapping INIT and resource are banished to 6.x
	   <106>	 5/15/90	JS		Added Elsie video extenion version
	   <105>	 5/15/90	JS		Added resources for Elsie video extension
	   <104>	 5/14/90	csd		Removed the splash screen init.
	   <103>	 5/13/90	PKE		Change ROv# resources for Portable and Mac32 so we use newly
									modified KCHR 0 in System file instead of old KCHR 0 in ROM.
	   <102>	 5/12/90	SMB		Adding the keyboard small icons (kcs#, kcs4, kcs8) for the US
									default keyboard (KCHR 0).
	   <101>	 5/10/90	DDG		Fixed the erickson string for the video card.
	   <100>	 5/10/90	JSM		AliasMgr is now a linked patch, don't include it as 'ptch' 20.
		<99>	  5/4/90	PP		Leave 'spoo' in 'fld#' until A10 is built and ready for
									installation.
		<98>	  5/3/90	PP		Change special folder names in 'fld#' resource.
		<97>	  5/2/90	PKE		Rearranged 'itlm' resource data to match new format.
		<96>	  5/2/90	JS		FOR 6.0.6-Added new 'card' resources for the ci video extension
		<95>	  5/1/90	BBM		add support for linked patches in system 6.0.6 or greater.
	   								and moved diskcache next to boot 2 resource
		<94>	 4/25/90	PKE		Add 'itlm' resource for multiscript mapping and sorting data.
		<93>	 4/25/90	S		To Change Constants in ppci resource.
		<92>	 4/24/90	DDG		Added back the ICN# 3 resource for system six. Also changed the
									FREFs and the BNDL slightly for system six (this does not affect
									system seven at all).
		<91>	 4/20/90	dba		put in the new apple for the menu bar; get rid of the two
									anti-aliased ones
		<90>	 4/19/90	csd		fixed the IDs for the CDEF 1 (scroll bar) owned resources.
		<89>	 4/19/90	DDG		Conditionalizing the new FREF for the help manager to only be
									included for system 7.0. Also added a string for the tiburon
									card.
		<88>	 4/19/90	dba		get rid of CheckDevicesINIT.a.rsrc on 7.0 systems; also use
									newer ParityINIT.a.rsrc instead of the two parity INITs on 7.0
									systems
	   								single ParityINIT instead of two ParityINITs for 7.0 systems
		<87>	 4/18/90	DDG		Added PatchTheBigFive.rsrc to the system file. Also made 7.0
									include PrintDriver.a.rsrc and 6.x include
									OldPrintDriver.a.rsrc.
		<86>	 4/15/90	dba		update dialog for INIT 12 (licensing splash screen)
		<85>	 4/14/90	csd		fixed the conditional for INIT 12 (licensing splash screen)
		<84>	 4/13/90	DDG		Removed the SnthLoading.rsrc file (it contains all the slods,
									which are no longer needed).
		<83>	 4/13/90	csd		added SplashScreenINIT and its ALRT and DITL.
		<82>	 4/12/90	DD		Added build instruction for system 7 to include
									DeCompressDefProc1.a.rsrc as 'dcmp' ID 1.
		<81>	 4/12/90	KO		Removed Button CDEF; changed Meter CDEF ID to 62, changed name
									of "SinDialog.r" to "SinHighLevel.r".
		<80>	 4/11/90	csd		added resources needed by color scroll bar and windows.
		<79>	 4/11/90	KSM		What happened to the submit?
	   <77+>	 4/11/90	KSM		Change IncSys's for BalloonPack resources.
		<77>	 4/11/90	BBH		eliminate inclusion of temporary CommToolbox resources (INIT 29,
									proc -32509, -32508, ctbp -32512, -32511, -32510)
		<76>	 4/11/90	dba		put back the STR resource containing “Spool Folder”, since
									PrintMonitor is still looking at it; add GenericIcons.rsrc;
									change IDs for DiskInit package resources for Nick K.
		<75>	 4/10/90	dba		marked the bootblocks resource protected to keep it safe from
									overzealous Finders; added the new secondary boot resource;
									moved the ".Print" driver back over to the 7.0 side because
									ProcessMgr.rsrc no longer includes its own ".Print"
		<73>	  4/9/90	NB		Moved "Spool Folder" string resource into a "< 7.00" condition.
									This is part of a several file patch to move the drivers into the
									extensions folder. The .Print driver will be rolled in next.
		<72>	  4/9/90	VL		Updated 'rtt#' for Database Access Manager.
		<71>	  4/5/90	CCH		Added Macintosh IIfx and XO to Product name resource.
		<70>	  4/4/90	KON		get rid of ptch 44, 35, 36
		<69>	 3/30/90	BBH		make cmtb resources load into system heap always
		<68>	 3/30/90	RLC		Conditionally add color icon family IncSys's for System 7.0 help
									file icon.
		<67>	 3/29/90	HJR		Change conditional in FKeys so that Dumpscreenfkey and
									printscreenfkey are no longer in 6.10 build.
		<66>	 3/29/90	KON		Added build commands for AllB&WQDPatch.a, ptch 44.
		<65>	 3/26/90	PKE		Include Script Mgr ptch 27 and ptch 39 for SysVers >= $607 (was
									SysVers >= $700). The source for these patches is now
									conditionalized appropriately.
		<64>	 3/23/90	NC		Added ptch 43 which now contains the Sound Manager Extensions.
		<63>	 3/22/90	csd		put in color icons (icl8, icl4, ics8, ics4) for the System File
									(Mac Plus) icon.
		<62>	 3/22/90	dba		bad doggie is not even strong enough; I had it right in change
									60
		<61>	 3/22/90	dba		syntax error; Darin is a bad doggie
		<60>	 3/22/90	dba		put in the lpch resources that I forgot in change 58
		<59>	 3/21/90	PKE		Added new info to US 'itlb' resource: font/size data, script
									local data size (needed for auto-initialization of script
									systems). Matches updated type in SysTypes.r. Also defined some
									FOND ID constants.
	   								(needed for auto-initialization of script systems). Matches
									updated type in SysTypes.r. Also defined some FOND ID constants.
		<58>	 3/21/90	dba		use linked patches, new boot blocks, and build MultiFinder, DA
									Handler and VM in
		<57>	 3/20/90	fjs		change the new mouse speed
		<56>	 3/20/90	S		To Change Default ppci parameters.
		<55>	 3/20/90	PP		Remove obsolete 'fld#' resource items and add "Already Connected
									to Server' Dialog to AliasMgr.
		<54>	 3/20/90	S		To Add a new PPC Resource ppci.
		<53>	 3/20/90	DDG		Changed sysvers conditionals from $610 to $607.
		<52>	 3/19/90	JSM		Add "<This Macintosh>" to PPC Browser STR#.
		<51>	 3/19/90	RLC		Include some new resources ('STR ' & 'ICN#') for Help Mgr.
		<50>	 3/18/90	DDG		Change the #ifdef statement for hasPopupCDEF and hasCommToolbox
									to be an #if statement. This ensures that these two features are
									only included on the proper systems.
		<49>	 3/17/90	BBH		oops - duplicated a line causing build to fail (StandardNBP STR#
									include line was duplicated)
		<48>	 3/17/90	BBH		add CommToolbox resources to system file
		<47>	 3/15/90	JSM		Add configuration resource for PPC Browser.
		<46>	 3/14/90	RLC		Add 'help' type to BNDL to get an icon.
		<45>	 3/12/90	JSM		Add even more resources for the Database Access Manager.
		<44>	  3/9/90	NJC		Added build rules for Sound Input Manager, Erickson Sound Input
									DRVR, CDEFs for the SinHighLevel Routines, and the standard
									Sound Input Dialog.
		<43>	  3/8/90	JSM		Change label for zone list in PPC Browser.
		<42>	  3/5/90	BBH		added resources for Popup CDEF
		<41>	  3/1/90	DC		Changed layerMgr.c.rsrc include to expect type 'ptch' and id 17.
									Changed sysobj.make to generate 'ptch' 17.
		<40>	 2/27/90	PKE		For 7.0, add new ScriptMgrROMPatch.rsrc as ‘ptch’ 39 (combines
									common Script Mgr patch code from IIci and Portable patch
									files).
		<39>	 2/23/90	JSM		Add some more resources and private .r file for the Database
									Access Manager.
		<38>	 2/21/90	DD		Added resource for DeCompressDefProc.a
		<37>	 2/19/90	HJR		Remove SERD from system which is now and will stay in ROM or the
									patch file as where it should remain.
		<36>	 2/19/90	JSM		Add resources for PPC Browser.
		<35>	 2/17/90	ngk		Split resources for 6.x and 7.x standard file into separate
									resource files
		<34>	 2/17/90	fjs		add new mcky resource more appropriate for large screens
		<33>	 2/16/90	DDG		Fixed the conditionals the sound manager extentions and the
									pictwhap FKEY so that they are included in the Sys610 Build.
		<32>	 2/15/90	JRM		Add version of standalone version of popup icon
		<31>	 2/12/90	JSM		Override ROM version of MDEF 0 on Mac32 and Portable.
		<30>	  2/8/90	RLC		Toss BalloonHelp icon in Standard file dialogs.
		<29>	  2/7/90	HJR		Needed for 6.0.5: Added AsyncIOPDriver.aii which replaces
									SERD=60/61 in ROM.
		<28>	  2/6/90	fjs		add dithered gray pattern to the pattern list
		<27>	  2/1/90	csd		Needed for 6.0.5: Added ALRT and DITL -16416 for the mismatched
									32-Bit QuickDraw alert.
		<26>	 1/24/90	dba		fix version string to mention 1990
		<25>	 1/24/90	dba		change names of some of the special folders
		<23>	 1/22/90	ngk		Moved volume icon over in PutFile to make room for border around
									file list
	    <21>	 1/22/90	JRM		add Mail Letter icon for Standard File
									add Control Panels folder
	    <20>	 1/22/90	PP		"Extension Folder" name is "Extensions Folder".
		<20>	 1/18/90	JSM		Add STR# -5728 for Database Access Manager.
		<19>	 1/17/90	PKE		Updated itlc resource data for new region code field added to
									itlc type in SysTypes.r.
		<18>	 1/17/90	PKE		Updated itlc resource data to use MPW 3.1 itlc format, which is
									now in SysTypes.r.
		<17>	 1/17/90	fjs		Properly conditionalize FKEYS for 6.0.5 vs. 7.0
		<16>	 1/16/90	KO		NEEDED FOR 6.0.5: Added Quickdraw patch file for ci
	    <14>	 1/10/90	RLC		Update and change WhatIs related resources to new format.
		<13>	  1/8/90	ngk		Update EditionMgr include statment. Fixed EditionFile SICN.
		<12>	  1/8/90	dba		added INIT icon into the bundle
		<11>	  1/5/90	CCH		NEEDED FOR 6.0.5:  Added Zone 5 to machine names list (STR#
									-16395).
		<10>	  1/4/90	csd		Fixed the BNDL so that it no longer refers to ICN# 4 which Darin
									nuked.
		 <9>	  1/3/90	dba		override the MBDF on Mac Plus and Mac SE
		 <8>	  1/3/90	RC		Update stdfile help strings (STR# -3999).
		 <7>	  1/3/90	CCH		Changed “Gestalt.a.rsrc” to “Gestalt.rsrc”.
		 <6>	  1/2/90	ngk		Fix DataPub.p.rsrc to explicitly include code resources
		 <5>	12/28/89	dba		create new #define (codeAs) so we don’t need 'RSRC' (0)
									sprinkled all over the file; change some code resources to be
									compiled as 'RSRC' (0) (includes changes to SysObj.Make); move
									patter and icon LDEF to appropriate places in the file and add
									more comments about them
		 <4>	12/25/89	dba		add in icons; get rid of lousy Finder icon; put conditionals in
									for Standard File help (no balloons in 6.0)
		 <3>	12/20/89	dba		move keyboard stuff to separate file
		 <2>	12/18/89	CCH		Now includes Rez text files, instead of resources files.
	  <14.1>	12/11/89	GMR		Added Sony Format patch (FormatPatch.a)
	  <14.0>	 12/6/89	RLC		Change overlay resource on Portable to use the RAM 'MBDF' proc
									for system 7.0.
	  <13.9>	 12/4/89	RLC		Fix include statement for BalloonHelp.
	  <13.8>	 12/1/89	RLC		Shift down each item in SFPutFile dialog and add some to height
									of DLOG frame.
	  <13.7>	11/30/89	RLC		Added hmnu, hdlg, & new STR# (-3999) to support Help Mgr in
									Standard File dialogs.
	  <13.6>	11/29/89	fjs		added include for icon list definition and a light gray 'PAT '
									for the chooser and the finder, added new and exciting color
									patterns (now there are 12), made default color pattern ('PPAT'
									16) a dithered gray.
	  <13.5>	11/29/89	GGD		NEEDED FOR 6.0.5: Enabled New ADB Manager for 6.0.5
	  <13.4>	11/28/89	prp		AliasMgr SwitchDisk dialog resources # changed due to a
									conflict.
	  <13.3>	11/28/89	CVC		Added alert resources for PPC.
	  <13.2>	11/27/89	prp		AliasMgr includes SwitchDisk Dialog resources.
	  <13.1>	11/21/89	EMT		NEEDED FOR 6.0.5: Added humane scrolling (ptch 9).
	  <13.0>	11/17/89	dba		got rid of FInit68K for System 7 because MultiFinder does its
									own launching
	  <12.9>	11/10/89	ngk		added icons for generic edtion files to Standard File icons
	  <12.8>	 11/7/89	csd		with dba: fix resource IDs of Standard File icons; get rid of
									Background Folder string
	  <12.7>	 11/2/89	RLC		Added new Balloonptch28 files.
	  <12.6>	10/31/89	dba		nuked INIT 4 in System 7
	  <12.5>	10/30/89	EMT		nuked INITs 0 & 1 in System 7
	  <12.4>	10/30/89	JRM		moved generic icons to PACK 3 range
	  <12.3>	10/25/89	BAL		Added GetPictUtil.rsrc as PACK 15
	  <12.2>	10/25/89	rwh		WOULD BE NICE FOR 6.0.5: added ptch38, backpatch of hwPriv trap.
	  <12.1>	10/15/89	BAL		Added 32-Bit Pict drawing patches for SE/Plus (35) and Exprit
									(36).
	  <12.0>	10/14/89	EMT		Cache is loaded by syspatch, don't need INIT 35.
	  <11.9>	10/13/89	prp		AliasMgr has additional resource for Folder Selection Dialog.
	  <11.8>	10/12/89	CVC		Added access control to ppc toolbox
	  <11.7>	10/11/89	dba		changed fonts so that ROM fonts are used in System 7.0
	  <11.6>	10/11/89	EMT		Included System7.0 Resources for BigBang and take fonts from
									SysStuffƒ just like in SCM build. Don't include AppleShare Rsrcs
									at all.
	  <11.5>	10/11/89	dba		change Picasso Macintosh icon to -16396; add anti-aliased apple
									icons
	  <11.4>	10/10/89	dba		no DAs for a 7.0 build; moved PPC to its place in alphabetical
									order; moved gray pattern in ppt# to middle of pattern list;
									added SlotMgr patch with Dave Wong
	  <11.3>	 10/6/89	JSM		Include SnarfMan as a PACK instead of a ptch.
	  <11.2>	 10/5/89	NJC		slod.rez has been changed to SnthLoading.r. SnthLoading.rsrc is
									now in the ObjFiles folder so people can do obj builds.
	  <11.1>	 10/5/89	PAT		"really" added new Finder icon and small icon (see v10.8 dba)
	  <11.0>	 10/4/89	ngk		added PupUpMenuSymbol sicn to standard file resources
	  <10.9>	 10/3/89	GMR		added new ADB ptch 34, for SE,II
	  <10.8>	 10/3/89	dba		added new Finder icon and small icon and reformatted things
	  <10.7>	 10/3/89	NJC		added #include for slod.rez which contains the sampled snth
									CPULoading values for each CPU.
	  <10.6>	 10/2/89	prp		added Finder 'extn' "Extension Folder' to 'fld#' resource.
	  <10.5>	 10/2/89	CVC		added resources required by the PPC Toolbox.
	  <10.4>	 9/28/89	CCH		added INCLUDE of :SysStuff:AppleShare Rsrcs conditionalized to
									SysVers >= $700. Also changed "Jaws32" to "Mac32".
	  <10.3>	 9/26/89	EMT		Corrected include of 'ptch' 0 to depend upon HasTearOffMenus
	  <10.2>	 9/25/89	BAL		added 32-Bit QuickDraw as ptch 32 for System 7.0
	  <10.1>	 9/21/89	RLC		Changed Balloon Help WDEF ID to 126.
	   <7.1>	 9/21/89	DAF		FOR 6.0.4 BUILD - added QDCheckDevices system init to build.
									added clut=127 (QD default colors) to System file. Overrode
									clut=127 in ROM for Mac II family and Aurora
	  <10.0>	 9/19/89	RLC		added even more resources for Balloon Help.
	   <9.8>	 9/18/89	PKE		For 7.0, changed type of keyboard/script small icon from 'SICN'
									to new 'kscn' to avoid resource ID conflicts.
	   <9.9>	 9/18/89	CVC		Integrate the PPC Toolbox as a ptch the first time.
	   <9.6>	 9/13/89	RLC		added new resources for Balloon Help.
	   <9.7>	 9/13/89	ngk		added open string to STR# -3997 for standard file
	   <9.5>	  9/6/89	dba		fixed color tables on ppat and ppt#, reformatted AppleTalk 2.0
	   <9.4>	  9/2/89	CCH		NEEDED FOR 6.0.5: added extra "}" at end of AppleTalk dialogs.
	   <9.3>	  9/1/89	CCH		NEEDED FOR 6.0.5: added AppleTalk dialogs for F-19.
	   <9.2>	 8/30/89	dba		replaced 0x… with $…; moved stuff into Types.r and SysTypes.r;
									simplified definitions for Color QuickDraw stuff (removing seed
									from 'clut' 127); locked the ROvr and ROv# resources; created a
									second definition for 'ppt#' to use until Rex can handle it;
									changed the color apple to have a more minimal rowBytes
	   <9.1>	 8/28/89	PKE		Add Script Manager extensions patch as 'ptch' 27 (SysVers >=
									$700 only).
	   <9.0>	 8/17/89	dba		got rid of “purgeable” on ROv# resources and fixed build
	   <8.9>	 8/17/89	dba		reorganization of this entire file; be careful not to roll this
									into 6.0.4 because some of the dialogs have been edited, and the
									attributes have been changed on some resources
	   <8.7>	 8/15/89	dba		NEEDED FOR 6.0.4: changed resource ID of QDCDInit from 14 to 6
									restrained myself as much as possible from gratuitous editing
	   <8.6>	 8/15/89	dba		NEEDED FOR 6.0.4: put back MBDF in Esprit, since recent bug
									fixes affected Color QuickDraw only; added semicolons to make
									7.0 build work again; edited Parity dialog to improve its
									appearance
	   <8.8>	 8/15/89	CCH		Changed references to {rsrc} to {ObjDir} and replaced references
									to {ColorPickerPack} with an equivalent expression.
	   <8.5>	 8/12/89	dba		NEEDED FOR 6.0.4: override MBDF in Aurora & Esprit ROMs (we blew
									it again, folks); also turned off WDEF 0 override on Aurora
	   <8.4>	 8/11/89	jbs		FOR 6.0.4! - Changed the DITL for the Aurora Extension to
									reflect the length of the version string (which is shorter now).
	   <8.3>	  8/9/89	prp		Preferences folder specialty is 'pref' and not 'temp'.
	   <8.2>	  8/8/89	PKE		NEEDED FOR 6.0.4: In ROv# 117 & 630, conditionalize WDEF 0
									override for SysVers >= $700 only. Change STR# -10 to -16395 and
									delete attributes. All of this per DBA and CSD.
	   <8.1>	  8/8/89	dba		NEEDED FOR 6.0.4: Neil sez: MIDI doesn’t work; take it out also,
									get rid of extraneous names in this file
	   <8.0>	  8/7/89	GMR		added new MiscPatches.a - Big Bang only patches for all ROMs.
	   <7.8>	  8/7/89	prp		AliasMgr has several additional resources to be included.
	   <7.9>	  8/7/89	JRM		added rom override (rov#) for WDEF 0 so that all systems can
									have icon in window title
	   <7.7>	  8/6/89	PAT		Changed default desktop pattern for Esprit (in accordance w/
									Product Marketing decision) - PAT ID=15
	   <7.6>	  8/5/89	PKE		NEEDED FOR 6.0.4: Fix conditionals for ROv# 890 & 1660; clean up
									KCHR conditionals per code review.
	   <7.5>	  8/4/89	NMB		Removed all Ginsu dependencies. Now the default printer (STR
									-8192) is undefined in a new system. An error is returned so the
									user knows to pick a driver.
	   <7.4>	  8/3/89	DAF		FOR 6.0.4 BUILD - added Mac IIci Monitor extension to system
									build.
	   <7.3>	  8/3/89	CCH		added BitEdit as 'PACK' #1.
	   <7.2>	 7/25/89	GMR		NEEDED FOR 6.0.4: added Sony Eject patch (SonyEjectPatch.a)
	   <7.0>	 7/24/89	CSD		added #includes for ColorPickerWedge.r and PixPat4colors.r for
									the Color Picker PACK (PACK12).
	   <6.9>	 7/24/89	GMR		added ALRT & DITL -16392 for parity disabled warnings, and
									IncSys calls for Parity Inits.
	   <6.8>	 7/20/89	RLC		added IncSys calls to include the system resources needed for
									Help Mgr.
	   <6.7>	 7/19/89	CCH		NEEDED FOR 6.0.4: Removed Init13.a from build.
	   <6.6>	  7/3/89	NJC		Sound Mgr Extensions (ptch 23) rolled in for real.
	   <6.5>	 6/30/89	CCH		added rsrc mgr extensions ('ptch' 21) to System 7.0
	   <6.4>	 6/29/89	RLC		Add HelpMgr BalloonPack code in as 'ptch' 28,SysHeap,Locked
	   <6.3>	 6/26/89	PKE		NEEDED FOR 6.0.4 ONLY: Aurora and Esprit have BigBang version of
									KCHR 0 in ROM, so override it with 6.0.4 version of KCHR 0.
	   <6.2>	 6/23/89	CCH		Made STR# SysHeap and Locked, and named it.
	   <6.1>	 6/21/89	NJC		added Sound Mgr 7.0 build support.
	   <6.0>	 6/19/89	MSH		Changed the id numbers of STR -2000 to -2002, ALRT -2000 to
									-2003, DITL -2000 to -2003, SICN -2000.
	   <5.9>	 6/10/89	MSH		Shortened STR -2002 to fit in Nmgr alert. Swapped positions of
									the Sleep and Cancel buttons in DITLs -2000, -2001, -2002.
	   <5.8>	  6/8/89	JRM		sf confitionals for big bang and 604
	   <5.7>	  6/8/89	CCH		Changed machine name from Aurora to release name in STR# -10.
	   <5.6>	  6/8/89	prp		Folder Mgr resources conditional for BigBang only.
	   <5.5>	  6/6/89	CCH		Changed Macintosh SE 030 to SE/30.
	   <5.4>	  6/5/89	JRM		new alerts, strings, icons for big bang standard file
	   <5.4>	  6/5/89	JRM		New standard file alerts, strings, and SICNs
	   <5.3>	  6/5/89	KSM		added big bang menu mgr patch.
	   <5.2>	  6/5/89	prp		Folder Mgr is added as part of Alias Mgr.
	   <5.1>	  6/1/89	CCH		added machine names for Esprit and Aurora to STR# -10.
	   <5.0>	  6/1/89	KSM		added tear off menu wdef.
	   <4.9>	  6/1/89	PKE		Discard INIT 5 for BigBang; the patches it contains have been
									rolled in and are conditionalized out for BigBang.
	   <4.8>	 5/31/89	CEL		added in SysFonts.r so Bass fonts will be rezed in for BigBang
	   <4.7>	 5/31/89	prp		added Alias Mgr Support
	   <4.6>	 5/30/89	dnf		Tweak 4.5 so it works
	   <4.5>	 5/30/89	dnf		Resource is now called HFS70.a.rsrc (ptch 18)
	   <4.4>	 5/25/89	MSH		Changed the wording of STR -2002.
	   <4.3>	 5/22/89	PKE		(Big Bang only) Modified KCHR ID=0 for option-shift entry of
									double acute accent, code $FD
	   <4.2>	 5/19/89	ngk		added include for DataPublication PACK and owned resources
	   <4.1>	 5/18/89	rwh		added ptch14SwapHMMU, ptch15SwapPMMU to the build
	   <4.0>	 5/15/89	EKN		added FileIDs 'ptch'.
	   <3.9>	 5/13/89	EMT		Fixed a typo.
	   <3.8>	 5/13/89	EMT		Don't use WDEF 127 until Big Bang.
	   <3.7>	 5/13/89	EMT		added Window Mgr extensions (Layers).
	   <3.6>	  5/8/89	NMB		last change was with override; comments got screwed up. Fixed
									them.
	  <•3.5>	  5/8/89	NMB		added patch support for PrGlue (ptch 16), and added Printing" at
									STR -8192. Chooser now writes the system printer into -8191.
	   <3.3>	  5/3/89	PAT		added new default desktop pattern for Esprit - PAT ID=15
	   <3.2>	  5/3/89	CEL		Adding in Bass for the first time
	   <3.1>	  5/2/89	CCH		added 'STR#' resource -10 for machine names.
	   <3.0>	 4/14/89	MSH		Locked low power message strings -2000 to -2003.
	   <2.9>	 4/11/89	JSM		Include SnarfMan 'ptch'.
	   <2.8>	 4/10/89	CCH		Took out FCQD stuff.
	   <2.7>	  4/6/89	CCH		Removed FCQD from BigBang build.
				 3/14/89	MSH		Added Esprit low power and AppleTalk alert resources
				 3/12/89	PKE		Added script icon location at end of itlc (in previously
									reserved bytes)
				  3/8/89	PKE		Commented out itlr stuff
				 2/24/89	PKE		Modified KCHR ID=0 to make it possible to enter additional
									upper-case accented forms with dead keys. added KMAP ID=5 for
									Zoots keyboard. Modified itlb ID=0 to correct flags.
	  <S570>	 9/16/88	EKH		String for the ":Background Folder".
	  <S568>	  9/9/88	ldc		added accented characters to KCHR resource
	 <PB570>	  9/8/88	BAL		added Full Color QuickDraw (FCQD) as ptch -24. Also added rom
									overides for WDEF=0, WDEF=1, CDEF=1.
	  <S522>	  7/6/88	ldc		added itlr
	  <S519>	  7/1/88	EKH		Removed the #include to include the AppleShare Resources.
	  <S497>	  6/7/88	med		Changed Script Mgr to ptch resource
	  <S478>	 4/26/88	EKH		Removed the names of the 'ptch' resource from Sys.r
	  <S475>	 4/20/88	JB		Shortened wording of message in DITL -6047, item 24
	  <S454>	  4/5/88	EKH		Added the WDEF ID=0 back into the system build.
	  <S414>	 2/29/88	JB		Renumbered dialog items for disk init package (see S413)
	  <S411>	 2/29/88	EKH		Changed the attributes of some resources as per Ed Tecot.
	  <S410>	 2/27/88	JB		Removed resource SonyFMTR (64k ROM format support) Updated
									dialog items for disk init package (see S409)
	  <S405>	 2/25/88	med		Made KCHR purgeable and unlocked.
	  <S397>	 2/16/88	EKH		Removed two more resources NBPC 1,2. NOTICED changes that Ed
									Tecot and Mark Davis made, they set KMAP resource purgeable.
	  <S395>	 2/14/88	EKH		Fixed mistake in S390 fix......
	  <S390>	 2/11/88	EKH		Removed selected resources.... They are commented out below:
									.MPP, .ATP, PACK 4,5,7, WDEF 0.
	  <S374>	 1/27/88	EKH		Added the new 'vers' resource format. (id = -2 now)
	  <S357>	 1/12/88	EMT		Final changes to Notification SICNs and dialog from HIG
	  <S355>	 1/11/88	EKH		Added new PTCH id = -1 resource, this resource is for the new
									"mini" system installation scripts. See AfterPatches.a for code.
	  <S352>	  1/6/88	EMT		More fixes to Notification Mgr dialog
	  <S349>	  1/5/88	EMT		Made Notification Mgr dialog handle bigger strings
	  <S342>	12/18/87	med		Added itl4 build, deleted INTL (2) build
	  <S337>	12/16/87	EMT		Added small icons for notification
	  <S327>	 12/8/87	jw		Added mac wavesynth.
	  <Syyy>	 12/4/87	EKH		&EMT, added PACK ID=12 include, Color Picker Package.
	  <Syyy>	 12/3/87	EKH		Added includes for font, DA, and sound resources. Added the new
									'vers' resource for finder 6.1 and on.
	  <S319>	11/30/87	jw		Added new 'snth's.
	PMAB318>	11/30/87	jw		Added new improved sound mgr.
	PMAB317>	11/28/87	EMT		Added Notification Mgr.
	  <S316>	11/27/87	EMT		Created.

	To Do:

*/

#include "Types.r"
#include "SysTypes.r"

#if hasBalloonHelp
	#include "BalloonTypes.r"
#endif

#if hasCommToolbox
	#include "CommToolboxPriv.r"
#endif

#if hasDataAccessMgr
	#include $$Shell("DataAccessMgr")"DatabaseAccessPriv.r"		// for 'rtt#' definition
#endif

//__________________________________________________________________________________________________
// conditionals (these migrate into the build system)

#ifndef wantSounds
	#define wantSounds true
#endif

//__________________________________________________________________________________________________
// macros

#define IncSys Include $$Shell("ObjDir")
#define codeAs 'RSRC' (0) as

//__________________________________________________________________________________________________
// ROM versions

	#define MacPlus		$0075
	#define MacSE		$0276
	#define MacII		$0178
	#define Portable	$037A
	#define	Mac32		$067C

//__________________________________________________________________________________________________
// version

	#define VersionString \
		"Macintosh System version " SysVersion "\n\n\n" \
		"© Apple Computer, Inc. 1983-1991\n" \
		"All rights reserved."

	resource 'STR ' (0) { VersionString };

	resource 'vers' (1) {
		LIntVers, LangInt, SysVersion, SysVersion", © Apple Computer, Inc. 1983-1991"
	};

	resource 'BNDL' (0) {
		'MACS', 0, {
			'FREF', { 0, 0; 1, 1; 2, 2; 3, 3; 4, 4; 5, 5; 6, 6 };
			'ICN#', { 3, 3; -16415, -16415; 5, -5696; -16494, -16494; -16509, -16509 };
		}
	};

	type 'MACS' (0) { pstring; };
	resource 'MACS' (0, purgeable) { VersionString };

	resource 'FREF' (6) { 'zsys', -16494, "" };	// System File for 7.0
	resource 'FREF' (0) { 'ZSYS', 3, "" };		// System File for 6.0.x & other systemish files
	resource 'FREF' (1) { 'FNDR', 3, "" };		// Finder
	resource 'FREF' (2) { 'INIT', -16415, "" };	// standard INIT
	resource 'FREF' (3) { 'CLIP', -16509, "" };	// Clipboard File
	resource 'FREF' (4) { 'FDOC', 3, "" };		// Finder Startup
	resource 'FREF' (5) { 'help', 5, "" };		// System Help ICN# is in Balloon.r

//__________________________________________________________________________________________________
// names

	type 'STR ' (-16096) { pstring[31]; };	// pad name to 31 characters to avoid file compaction
	resource 'STR ' (-16096, sysHeap) { "" };		// no user name by default

	type 'STR ' (-16413) { pstring[31]; };	// pad name to 31 characters to avoid file compaction
	resource 'STR ' (-16413, sysHeap) { "" };		// no machine name by default

	resource 'STR#' (-16395, sysHeap) { {
		"Macintosh",			// 0 unknown
		"Macintosh XL",
		"Macintosh 512Ke",
		"Macintosh Plus",
		"Macintosh SE",
		"Macintosh II",
		"Macintosh IIx",
		"Macintosh IIcx",
		"Macintosh SE/30",
		"Macintosh Portable",
		"Macintosh IIci",
		"",		// <199>
		"Macintosh IIfx",		// 13
		"",						// 14 not used
		"",						// 15 not used
		"",						// 16 not used
		"Macintosh Classic",	// 17 XO
		"Macintosh IIsi",		// 18 Erickson
		"Macintosh LC",			// 19 Elsie
		"Macintosh Quadra 900",	// 20 Eclipse
		"PowerBook 170",		// 21 Tim
		"Macintosh Quadra 700",	// 22 Spike
		"Macintosh Classic II",	// 23 Apollo
		"PowerBook 100",		// 24 Asahi
		"PowerBook 140",		// 25 TimLC
		"Macintosh Quadra 950",	// 26 Zydeco		<13> Leave 4 trailing spaces for the Installer
		"Macintosh",			// 27 DBLite
	} };

	type 'dbex' { integer=0; };		// nothing
	resource 'dbex' (-16385) { };	// doesn’t need to contain anything, just needs to be present
//__________________________________________________________________________________________________
// startup stuff

	IncSys "BootBlocks.a.rsrc" codeAs 'boot' (1, sysHeap, protected);
	IncSys "StartSystem.a.rsrc" codeAs 'boot' (2, sysHeap, locked);
	IncSys "BootCode.a.rsrc" codeAs 'boot' (3, sysHeap, locked);
	IncSys "LinkedPatchLoader.a.rsrc" codeAs 'lodr' (-16385, sysHeap, locked);
	IncSys "DiskCache.a.rsrc" codeAs 'ptch' (41, sysHeap, locked);
	resource 'STR ' (-16501, sysHeap, purgeable) { "A/ROSE" };
	resource 'STR ' (-16507, sysHeap, purgeable) { "NetBoot" };

	IncSys "BootAlerts.a.rsrc" codeAs 'DSAT' (0, sysHeap, locked);
	IncSys "UserAlerts.a.rsrc" codeAs 'DSAT' (2, sysHeap, locked);

	IncSys "KbdInstall.a.rsrc" codeAs 'ADBS' (2, locked);

	// CPU gibbly for System file.  This resource should be removed when the build scripts
	// can Rez gibblies into the System with a real time stamp.
	
	type 'gbly' {
		integer	GibblyVersion1 = 1;			// 'gbly' version
		longint;							// Gibbly timestamp
		integer = $$CountOf(BoxFlags);		// Number of CPUs supported
		array BoxFlags {
			integer;						// BoxFlag of supported CPU
		};
	};

	resource 'gbly' (-16385, sysHeap) {
		GibblyVersion1,
		BuildTime,							// Time stamp
		{									// Supported BoxFlags
			$00FE, $00FF, $0000, $0001,
			$0002, $0003, $0004, $0005,
			$0006, $0007, $0008, $0009,
			$000A, $000B, $000C, $000D,
			$000E, $000F, $0010, $0011,
			$0012, $0013, $0014, $0015
		};
	};

	// CubeE table of patches that the linked patch loader should load.

	type 'ptbl' {
		integer PatchTableVersion1 = 1;		// 'ptbl' version
		integer = $$CountOf(PatchRanges) - 1;	// # of ranges to load
		wide array PatchRanges {
			integer;						// Start of range to load
			integer;						// End of range to load (inclusive)
		};
	};
	
	resource 'ptbl' (0, sysHeap, locked) {
		PatchTableVersion1,
		{
			$0000,							// Start of range
			$FFFF							// End of range.  (System file loads them all.)
		};
	};
		
	// Picasso Macintosh icon (loaded by System Error Handler)

	resource 'cicn' (-16396, purgeable) {
		{0, 0, 32, 32}, 4,			// bounds, bits per pixel
	// mask
		$"0007FF80 00080000 00087E20 00080120"
		$"00080120 04080120 0C080120 08080120"
		$"66080120 99080120 8108FE20 80080020"
		$"80080020 4A001F20 34000020 00FC0020"
		$"03000000 0401FFF0 04000008 03E01FE4"
		$"00100552 001000A9 00200001 002001FE"
		$"00180000 00040000 0000C000 00012000"
		$"00025000 00008800 00000800 00001000",
	// black & white image
		$"0007FF80 00080000 00087E20 00080120"
		$"00080120 04080120 0C080120 08080120"
		$"66080120 99080120 8108FE20 80080020"
		$"80080020 4A001F20 34000020 00FC0020"
		$"03000000 0401FFF0 04000008 03E01FE4"
		$"00100552 001000A9 00200001 002001FE"
		$"00180000 00040000 0000C000 00012000"
		$"00025000 00008800 00000800 00001000",
	// color table
		{
			whiteRGB;				// white
			$2000, $2000, $2000;	// dark gray
			$C000, $C000, $0000;	// yellow
			$0000, $5000, $0000;	// green
			$E000, $0000, $0000;	// red
			$0000, $0000, $E000;	// blue
		},
	// 4-bit color image
		$"00000000000001111111111110000000"
		$"00000000000010000000000000000000"
		$"00000000000010000222222000100000"
		$"00000000000010000000000200100000"
		$"00000000000010000000000200100000"
		$"00000300000010000000000200100000"
		$"00003300000010000000000200100000"
		$"00003000000010000000000200100000"
		$"01100110000010000000000200100000"
		$"10011001000010000000000200100000"
		$"10000001000010002222222000100000"
		$"10000000000010000000000000100000"
		$"10000000000010000000000000100000"
		$"01001010000000000001111100100000"
		$"00110100000000000000000000100000"
		$"00000000444444000000000000100000"
		$"00000044000000000000000000000000"
		$"00000400000000011111111111110000"
		$"00000400000000000000000000001000"
		$"00000044444000000005555555500100"
		$"00000000000400000000050505050010"
		$"00000000000400000000000050505001"
		$"00000000004000000000000000000001"
		$"00000000004000000000000111111110"
		$"00000000000440000000000000000000"
		$"00000000000004000000000000000000"
		$"00000000000000001100000000000000"
		$"00000000000000010010000000000000"
		$"00000000000000100201000000000000"
		$"00000000000000002000100000000000"
		$"00000000000000000000100000000000"
		$"00000000000000000001000000000000"
	};

//__________________________________________________________________________________________________
// override ROM resources

	IncSys "ROvr.a.rsrc" codeAs 'ROvr' (0, locked);	// code to do override

	resource 'ROv#' (MacPlus, locked) { MacPlus, {
		'MBDF', 0;		// new MBDF for new Menu Mgr
		'MDEF',	0;		// new MDEF for new Menu Mgr
		'WDEF',	0;		// WDEF for new variant

		'DRVR', 2;		// new .Print driver
	} };

	resource 'ROv#' (MacSE, locked) { MacSE, {
		'CDEF', 0;		// new button CDEF fixes small bug when drawing text
		'CDEF', 1;		// why?
		'MBDF', 0;		// new MBDF for new Menu Mgr
		'MDEF',	0;		// new MDEF for new Menu Mgr
		'WDEF',	0;		// WDEF for new variant

		'KCHR',	0;		// new keyboard map

    	'DRVR', 40;		// AppleTalk
	} };

	resource 'ROv#' (MacII, locked) { MacII, {
		'CDEF', 0;		// new button CDEF fixes small bug when drawing text and for color
		'CDEF', 1;		// CDEF for color
		'MBDF', 0;		// new MBDF for new Menu Mgr
		'MDEF',	0;		// new MDEF for new Menu Mgr
		'WDEF',	0;		// WDEF for new variant

		'KCHR',	0;		// new keyboard map

		'clut',	127;	// 8 standard QuickDraw colors; green is fixed

    	'DRVR', 40;		// AppleTalk
	} };

	resource 'ROv#' (Portable, locked) { Portable, {
		'CDEF', 0;		// new button CDEF fixes small bug when drawing text and for color
		'CDEF', 1;		// why?
		'MBDF', 0;		// new MBDF for help
		'MDEF', 0;		// new MDEF for help and Standard File
		'WDEF',	0;		// WDEF for new variant

		'KCHR',	0;		// new keyboard map

    	'DRVR', 40;		// AppleTalk
	} };

	resource 'ROv#' (Mac32, locked) { Mac32, {
		'CDEF', 0;		// new button CDEF fixes small bug when drawing text and for color
		'CDEF', 1;		// CDEF for color
		'MBDF', 0;		// new MBDF fixes small bug for pop-up menus above menu bar and for help
		'MDEF', 0;		// new MDEF for help and Standard File
		'WDEF',	0;		// WDEF for color, new variant

		'KCHR',	0;		// new keyboard map

		'clut',	127;	// 8 standard QuickDraw colors; green is fixed

		'atlk', 1;		// AppleTalk
		'atlk', 3;		// AppleTalk
		'atlk', 10;		// EtherTalk driver (LC ROM only)
		'ltlk', 0;		// AppleTalk
		'ltlk', 1;		// AppleTalk
		'ltlk', 2;		// AppleTalk
		'ltlk', 3;		// AppleTalk
    	'DRVR', 10;		// AppleTalk
    	'DRVR', 40;		// AppleTalk
    	'DRVR', 9;		// AppleTalk
    	'INIT', 18;		// AppleTalk
    	'lmgr', 0;		// AppleTalk

		'iopc',	127;	// LocalTalk IOP (port B)									<7>
		'iopc',	128;	// LocalTalk IOP (port A)									<7>

	} };

//__________________________________________________________________________________________________
// Patch to prevent switch launching to System 7

	IncSys "PreventSwitchLaunch.a.rsrc" codeAs 'proc' (-16495, sysHeap, locked);		// <124>

//__________________________________________________________________________________________________
// fonts that cannot be removed from the System file

	resource 'resf' (-16493, sysHeap, purgeable) { {
		"Chicago", { 12, plain };
		"Geneva", { 9, plain, 12, plain };
		"Monaco", { 9, plain };
	} };
	resource 'FRSV' (1, purgeable) { { Chicago, 12; Geneva, 9; Geneva, 12; Monaco, 9; } };

//__________________________________________________________________________________________________
// patches

	IncSys "LinkedPatches.rsrc";

	IncSys "BeforePatches.a.rsrc" codeAs 'PTCH' (0, sysHeap, locked);

	IncSys "PatchPlusROM.a.rsrc" codeAs 'PTCH' (MacPlus, sysHeap, locked);
	IncSys "PatchSEROM.a.rsrc" codeAs 'PTCH' (MacSE, sysHeap, locked);
	IncSys "PatchIIROM.a.rsrc" codeAs 'PTCH' (MacII, sysHeap ,locked);
	IncSys "PatchPortableROM.a.rsrc" codeAs 'PTCH' (Portable, sysHeap, locked);
	IncSys "PatchIIciROM.a.rsrc" codeAs 'PTCH' (Mac32, sysHeap, locked);

//__________________________________________________________________________________________________
// INITs

	IncSys "ParityINIT.a.rsrc" codeAs 'INIT' (10, sysHeap, locked);

	IncSys "EDiskShutdownPtch.a.rsrc" codeAs 'INIT' (12, sysHeap, locked);
	
	
	resource 'mcky' (0, sysHeap, purgeable) { { 255, 255, 255, 255, 255, 255, 255, 255 } };
	resource 'mcky' (1, sysHeap, purgeable) { { 6, 255, 255, 255, 255, 255, 255, 255 } };
	resource 'mcky' (2, sysHeap, purgeable) { { 4, 10, 15, 255, 255, 255, 255, 255 } };
	resource 'mcky' (3, sysHeap, purgeable) { { 2, 8, 13, 17, 20, 255, 255, 255 } };
	resource 'mcky' (4, sysHeap, purgeable) { { 1, 4, 7, 10, 13, 15, 16, 255 } };
	resource 'mcky' (5, sysHeap, purgeable) { { 1, 2, 3, 5, 8, 13, 21, 34 } };
	resource 'mcky' (6, sysHeap, purgeable) { { 1, 2, 3, 4, 5, 9, 15, 21 } };

//__________________________________________________________________________________________________
// FKEYs

	IncSys "PictWhap.a.rsrc" codeAs 'FKEY' (3, sysHeap, purgeable);	// dump screen to PICT file
	IncSys "PictWhapSound.rsrc";									// snapshot sound
	resource 'STR ' (-16385, sysHeap, purgeable) { "Picture ^0" };	// filename to use
	// The PrintScreen FKEY is not in 7.0 because users will print screen shots with TeachText 7.0

//__________________________________________________________________________________________________
// Alias Mgr (includes Folder Mgr)

	IncSys "AliasMgr.rsrc";

// Folder Mgr folder list
	resource 'fld#' (0, sysHeap, purgeable) { {
		'prnt',	inSystemFolder, "PrintMonitor Documents";
		'strt',	inSystemFolder, "Startup Items";
		'amnu',	inSystemFolder, "Apple Menu Items";

		'extn',	inSystemFolder, "Extensions";
		'pref',	inSystemFolder, "Preferences";
		'ctrl',	inSystemFolder, "Control Panels";
		'font', inSystemFolder, "Fonts";
	}; };

//__________________________________________________________________________________________________
// AppleTalk

	// AppleTalk 58 resources
 	include $$Shell("Misc")"APTK58.rsrc" not 'ckid';
	
	// ^^ incorporates the following:
	// IncSys "AppleTalk.rsrc" not 'vers';
	// IncSys "ADSP.rsrc" not 'vers';
	// IncSys "Responder.rsrc" not 'vers';

	resource 'STR#' ($BFE8, sysheap, locked) { {
		"Your system has been placed in AppleTalk zone “",

		"”.  "
		"To select a different zone, please click the Network icon in the Control Panel, "
		"then click the selected AppleTalk connection icon.",

		"Access to your AppleTalk internet has now become available.  To use "
		"the internet, please click the Network icon in the Control Panel, "
		"then click the selected AppleTalk connection icon.",

		"An unexpected change in your AppleTalk internet has interrupted network access.  "
		"To resume network use, please click the "
		"Network icon in the Control Panel, "
		"then click the selected AppleTalk connection icon.",
	} };

	resource 'ALRT' ($BFE9, purgeable) {
		{80, 116, 205, 417}, $BFE9, beepStages, alertPositionMainScreen
	};
	resource 'DITL' ($BFE9, purgeable) { {
		{90, 116, 110, 196}, Button { enabled, "OK" };
		{13, 60, 85, 295}, StaticText { disabled,
			"A driver for the selected AppleTalk connection could not be found.  "
			"The built-in LocalTalk port will be used instead."
		};
	} };

	resource 'ALRT' ($BFEA, purgeable) {
		{80, 116, 205, 417}, $BFEA, beepStages, alertPositionMainScreen
	};
	resource 'DITL' ($BFEA, purgeable) { {
		{90, 116, 110, 196}, Button { enabled, "OK" };
		{13, 60, 85, 295}, StaticText { disabled,
			"An error occurred while trying to start up your AppleTalk connection.  "
			"The built-in LocalTalk port will be used instead."
		};
	} };

	resource 'ALRT' ($BFEB, purgeable) {
		{80, 116, 235, 416}, $BFEB, beepStages, alertPositionMainScreen
	};
	resource 'DITL' ($BFEB, purgeable) { {
		{123, 115, 145, 194}, Button { enabled, "OK" };
		{13, 60, 118, 294}, StaticText { disabled, "^0" };
	} };

	resource 'STR#' ($BFEC, purgeable) { {
		"Router",

		"The Router could not run.  "
		"A driver for one of the AppleTalk connections can not be found.  "
		"The built-in LocalTalk port will be used for your AppleTalk connection.",

		"The Router could not run.  "
		"An error occured while installing a driver for one of the AppleTalk connections.  "
		"The built-in LocalTalk port will be used for your AppleTalk connection.",

		"The Router could not run.  "
		"The Router file was not found.  "
		"Please reinstall the Router.",

		"The Router could not run.  "
		"The Router file is damaged.  "
		"Please reinstall the Router.",
	} };

//__________________________________________________________________________________________________
// Backlight Driver

	IncSys "Backlight.rsrc" not 'vers';

//__________________________________________________________________________________________________
// BitEdit

#if hasBitEdit
	IncSys "BitEdit.a.rsrc" codeAs 'PACK' (1, sysHeap, purgeable);
#endif

//__________________________________________________________________________________________________
// Clock Manager																added PICTs <269>

// Double Arrow (not selected)
resource 'PICT' (-16385) {
	89,
	{134, 272, 152, 283},
	$"1101 0100 0A00 8601 1000 9801 1B90 0002"
	$"00FE 0160 0110 0170 00FE 0160 0110 016B"
	$"0086 0110 0098 011B 0000 3F80 4040 8420"
	$"8E20 9F20 BFA0 8E20 8E20 8020 8020 8E20"
	$"8E20 BFA0 9F20 8E20 8420 4040 3F80 FF"
};

// Double Arrow (UP selected)
resource 'PICT' (-16386) {
	125,
	{188, 503, 206, 514},
	$"1101 0100 0A00 BC01 F700 CE02 0290 0004"
	$"00BC 01F0 00CE 0208 00BC 01F7 00CE 0202"
	$"00BC 01F7 00CE 0202 0000 007F 0000 00FF"
	$"8000 01F7 C000 01E3 C000 01C1 C000 0180"
	$"C000 01E3 C000 01E3 C000 01FF C000 0100"
	$"4000 011C 4000 011C 4000 017F 4000 013E"
	$"4000 011C 4000 0108 4000 0080 8000 007F"
	$"0000 FF"
};

// Double Arrow (DOWN selected)
resource 'PICT' (-16387) {
	89,
	{188, 520, 206, 531},
	$"1101 0100 0A00 BC02 0800 CE02 1390 0002"
	$"00BC 0208 00CE 0218 00BC 0208 00CE 0213"
	$"00BC 0208 00CE 0213 0000 3F80 4040 8420"
	$"8E20 9F20 BFA0 8E20 8E20 8020 FFE0 F1E0"
	$"F1E0 C060 E0E0 F1E0 FBE0 7FC0 3F80 FF"
};

//__________________________________________________________________________________________________
// Color Picker

	IncSys "ColorPicker.p.rsrc" codeAs 'PACK' (12, sysHeap, purgeable);
	#include $$Shell("ColorPicker")"ColorPicker.r"			// why are these includes???
	#include $$Shell("ColorPicker")"ColorPickerWedge.r"		// they should be in line to speed up Rez

//__________________________________________________________________________________________________
// Comm. Toolbox

	IncSys 	"CommToolboxUtilities.c.rsrc" codeAs CTB_CTBUtilitiesType (CTB_CTBUtilitiesID, sysHeap, locked);

	IncSys 	"CommResourceMgr.c.rsrc" codeAs CTB_CommResourceMgrType (CTB_CommResourceMgrID, sysHeap, locked);

	IncSys 	"ConnectionMgr.c.rsrc" codeAs CTB_ConnectionMgrType (CTB_ConnectionMgrID, sysHeap, locked);

	IncSys 	"TerminalMgr.c.rsrc" codeAs CTB_TerminalMgrType (CTB_TerminalMgrID, sysHeap, locked);
	IncSys 	"TerminalClick.r.rsrc" CTB_KeyclickType (CTB_KeyclickID) as CTB_KeyclickType (CTB_KeyclickID, sysHeap, purgeable);

	IncSys 	"FileTransferMgr.c.rsrc" codeAs CTB_FileTransferMgrType (CTB_FileTransferMgrID, sysHeap, locked);

	IncSys 	"DITL.p.rsrc" codeAs CTB_DITLPackType (CTB_DITLPackID, sysHeap, purgeable, locked);
	IncSys 	"CommToolboxLDEF.p.rsrc" codeAs 'LDEF' (CTB_ChooseDITLID, sysHeap, purgeable);

	IncSys	"Choose.p.rsrc" codeAs CTB_ChooseType (CTB_ChooseID, sysHeap, purgeable, locked);
	IncSys 	"ChooseHelp.r.rsrc" 'hdlg' (CTB_ChoosehdlgID) as 'hdlg' (CTB_ChoosehdlgID, sysHeap, purgeable);
	IncSys 	"ChooseHelp.r.rsrc" 'hmnu' (CTB_ChoosehmnuID) as 'hmnu' (CTB_ChoosehmnuID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'flst' (CTB_ChoosefinfID) as 'flst' (CTB_ChoosefinfID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'STR#' (CTB_ChooseSTRsID) as 'STR#' (CTB_ChooseSTRsID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'DLOG' (CTB_ChooseDLOGID) as 'DLOG' (CTB_ChooseDLOGID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'DITL' (CTB_ChooseDITLID) as 'DITL' (CTB_ChooseDITLID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'dctb' (CTB_ChoosedctbID) as 'dctb' (CTB_ChoosedctbID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'DLOG' (CTB_ChooseConfirmDLOGID) as 'DLOG' (CTB_ChooseConfirmDLOGID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'DITL' (CTB_ChooseConfirmDITLID) as 'DITL' (CTB_ChooseConfirmDITLID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'DLOG' (CTB_ChooseUnavailableDLOGID) as 'DLOG' (CTB_ChooseUnavailableDLOGID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'DITL' (CTB_ChooseUnavailableDITLID) as 'DITL' (CTB_ChooseUnavailableDITLID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'CNTL' (CTB_ChooseCNTLID) as 'CNTL' (CTB_ChooseCNTLID, sysHeap, purgeable);
	IncSys 	"Choose.r.rsrc" 'MENU' (CTB_ChooseMENUID) as 'MENU' (CTB_ChooseMENUID, sysHeap);

	IncSys 	"StandardNBP.p.rsrc" codeAs CTB_NuLookupType (CTB_NuLookupID, sysHeap, purgeable, locked);
	IncSys 	"StandardNBPHelp.r.rsrc" 'hdlg' (CTB_NulookuphdlgID) as 'hdlg' (CTB_NulookuphdlgID, sysHeap, purgeable);
	IncSys 	"StandardNBP.r.rsrc" 'DLOG' (CTB_NulookupDLOGID) as 'DLOG' (CTB_NulookupDLOGID, sysHeap, purgeable);
	IncSys 	"StandardNBP.r.rsrc" 'DITL' (CTB_NulookupDITLID) as 'DITL' (CTB_NulookupDITLID, sysHeap, purgeable);
	IncSys 	"StandardNBP.r.rsrc" 'dctb' (CTB_NulookupdctbID) as 'dctb' (CTB_NulookupdctbID, sysHeap, purgeable);
	IncSys 	"StandardNBP.r.rsrc" 'PICT' (CTB_NulookupPICTID) as 'PICT' (CTB_NulookupPICTID, sysHeap, purgeable);
	IncSys 	"StandardNBP.r.rsrc" CTB_NuLookupParmsType (CTB_NuLookupParmsID) as CTB_NuLookupParmsType (CTB_NuLookupParmsID, sysHeap, purgeable);
	IncSys 	"StandardNBPLDEF.p.rsrc" codeAS 'LDEF' (CTB_NulookupLDEFID, sysHeap, purgeable);

	IncSys 	"CommToolboxINIT.r.rsrc" CTB_CommFolderNameType (CTB_CommFolderNameID) as CTB_CommFolderNameType (CTB_CommFolderNameID, sysHeap, purgeable);
	IncSys 	"CommToolboxINIT.r.rsrc" CTB_MarkerType (CTB_CTB6MarkerID) as CTB_MarkerType (CTB_CTB6MarkerID, sysHeap, purgeable);
	IncSys 	"CommToolboxINIT.r.rsrc" CTB_MarkerType (CTB_CTB7MarkerID) as CTB_MarkerType (CTB_CTB7MarkerID, sysHeap, purgeable);
	IncSys 	"CommToolboxINIT.r.rsrc" CTB_ModemStringType (CTB_ModemStringID) as CTB_ModemStringType (CTB_ModemStringID, sysHeap, purgeable);
	IncSys 	"CommToolboxINIT.r.rsrc" CTB_ModemStringType (CTB_PrinterStringID) as CTB_ModemStringType (CTB_PrinterStringID, sysHeap, purgeable);

	IncSys 	"CommToolboxINIT.r.rsrc" CTB_ModemIconType (CTB_ModemIconID) as CTB_ModemIconType (CTB_ModemIconID, sysHeap, purgeable);
	IncSys 	"CommToolboxINIT.r.rsrc" CTB_Modem4ColorIconType (CTB_ModemIconID) as CTB_Modem4ColorIconType (CTB_ModemIconID, sysHeap, purgeable);
	IncSys 	"CommToolboxINIT.r.rsrc" CTB_Modem8ColorIconType (CTB_ModemIconID) as CTB_Modem8ColorIconType (CTB_ModemIconID, sysHeap, purgeable);

	IncSys 	"CommToolboxINIT.r.rsrc" CTB_PrinterIconType (CTB_PrinterIconID) as CTB_PrinterIconType (CTB_PrinterIconID, sysHeap, purgeable);
	IncSys 	"CommToolboxINIT.r.rsrc" CTB_Printer4ColorIconType (CTB_PrinterIconID) as CTB_Printer4ColorIconType (CTB_PrinterIconID, sysHeap, purgeable);
	IncSys 	"CommToolboxINIT.r.rsrc" CTB_Printer8ColorIconType (CTB_PrinterIconID) as CTB_Printer8ColorIconType (CTB_PrinterIconID, sysHeap, purgeable);

	IncSys 	"CommToolboxINIT.r.rsrc" 'vers' (CTB_VersionID) as CTB_VersionType (CTB_VersionID, sysHeap, purgeable);

//__________________________________________________________________________________________________
// control panels

// pattern used by Finder for control panel support (must have the same ID as the IconLDEF)
	resource 'PAT ' (19, sysHeap) { $"4000040040000400" };

// system patterns
	resource 'PAT#' (0, sysHeap, purgeable) { {
		$"FF FF FF FF FF FF FF FF";
		$"DD FF 77 FF DD FF 77 FF";
		$"DD 77 DD 77 DD 77 DD 77";
		$"AA 55 AA 55 AA 55 AA 55";
		$"55 FF 55 FF 55 FF 55 FF";
		$"AA AA AA AA AA AA AA AA";
		$"EE DD BB 77 EE DD BB 77";
		$"88 88 88 88 88 88 88 88";
		$"B1 30 03 1B D8 C0 0C 8D";
		$"80 10 02 20 01 08 40 04";
		$"FF 88 88 88 FF 88 88 88";
		$"FF 80 80 80 FF 08 08 08";
		$"80 00 00 00 00 00 00 00";
		$"80 40 20 00 02 04 08 00";
		$"82 44 39 44 82 01 01 01";
		$"F8 74 22 47 8F 17 22 71";
		$"55 A0 40 40 55 0A 04 04";
		$"20 50 88 88 88 88 05 02";
		$"BF 00 BF BF B0 B0 B0 B0";
		$"00 00 00 00 00 00 00 00";
		$"80 00 08 00 80 00 08 00";
		$"88 00 22 00 88 00 22 00";
		$"88 22 88 22 88 22 88 22";
		$"AA 00 AA 00 AA 00 AA 00";
		$"FF 00 FF 00 FF 00 FF 00";
		$"11 22 44 88 11 22 44 88";
		$"FF 00 00 00 FF 00 00 00";
		$"01 02 04 08 10 20 40 80";
		$"AA 00 80 00 88 00 80 00";
		$"FF 80 80 80 80 80 80 80";
		$"08 1C 22 C1 80 01 02 04";
		$"88 14 22 41 88 00 AA 00";
		$"40 A0 00 00 04 0A 00 00";
		$"03 84 48 30 0C 02 01 01";
		$"80 80 41 3E 08 08 14 E3";
		$"10 20 54 AA FF 02 04 08";
		$"77 89 8F 8F 77 98 F8 F8";
		$"00 08 14 2A 55 2A 14 08";
	} };

// system color patterns

// note: the colors used here are tuned for the 256-color standard color table
//   ??? should this be purgeable ???  I think so...  -gbm

	data 'ppt#' (0, sysHeap) {
	// number of patterns
		$"000C"
	// offsets to patterns
		$"00000032 000000E8 0000019E 00000254 0000030A 000003C0"
		$"00000476 0000052C 000005E2 00000698 0000074E 00000804"

	// pattern 1
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"43334333"
		$"33433343"
		$"43334333"
		$"33433343"
		$"43334333"
		$"33433343"
		$"43334333"
		$"33433343"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 0000 0000 0000"
		$"0002 0000 0000 DDDD"
		$"0003 4B8C A1FB A86B"
		$"0004 25A4 9057 9794"
		$"0005 3333 6666 3333"
		$"0006 9999 6666 2222"
		$"0007 FFFF 9999 9999"

	// pattern 2
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"22222322"
		$"22322222"
		$"22222232"
		$"23222222"
		$"22223222"
		$"22222223"
		$"22232222"
		$"32222222"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 0000 0000 0000"
		$"0002 9E66 AA03 D286"
		$"0003 4C0D 8A7A CF28"
		$"0004 FFFF FFFF 3333"
		$"0005 FFFF CCCC 9999"
		$"0006 FFFF 6666 0000"
		$"0007 CCCC 0000 0000"

	// pattern 3
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"22222222"
		$"22222222"
		$"22222222"
		$"22225222"
		$"22222222"
		$"22222222"
		$"22222222"
		$"52222222"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 0000 0000 0000"
		$"0002 5C10 5FFE 850D"
		$"0003 5ABA D368 FFFF"
		$"0004 B9CB 8C16 FFFF"
		$"0005 55A0 CCCC A6F3"
		$"0006 AAAA AAAA AAAA"
		$"0007 6666 CCCC CCCC"

	// pattern 4
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"10101010"
		$"01010101"
		$"10101010"
		$"01010101"
		$"10101010"
		$"01010101"
		$"10101010"
		$"01010101"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 A000 A000 A000"
		$"0001 6000 6000 6000"
		$"0002 0000 6666 0000"
		$"0003 0000 0000 DDDD"
		$"0004 0000 9999 FFFF"
		$"0005 FFFF 0000 9999"
		$"0006 CCCC 0000 0000"
		$"0007 FFFF FFFF 0000"

	// pattern 5
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"22222232"
		$"32322222"
		$"22222322"
		$"23232222"
		$"22222223"
		$"22322322"
		$"22222223"
		$"23223222"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 0000 0000 0000"
		$"0002 BE50 6574 7410"
		$"0003 A50D 67F6 6988"
		$"0004 0000 9999 FFFF"
		$"0005 FFFF 0000 9999"
		$"0006 CCCC 0000 0000"
		$"0007 FFFF FFFF 0000"

	// pattern 6
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"11112111"
		$"14111111"
		$"11111141"
		$"11111111"
		$"31111111"
		$"11141111"
		$"11111121"
		$"11111111"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 7286 7286 7286"
		$"0002 5BEF C86B B339"
		$"0003 DDDD 9D2E DCD3"
		$"0004 C5F5 D102 FFFF"
		$"0005 0000 0000 0000"
		$"0006 DDDD 0000 0000"
		$"0007 FFFF FFFF 0000"

	// pattern 7
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"32323236"
		$"23232323"
		$"32323232"
		$"23232323"
		$"32323232"
		$"23232323"
		$"32323232"
		$"23232323"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 0000 0000 0000"
		$"0002 5555 5555 5555"
		$"0003 AAAA AAAA AAAA"
		$"0004 0000 9999 FFFF"
		$"0005 FFFF 0000 9999"
		$"0006 DDDD 0000 0000"
		$"0007 FFFF FFFF 0000"

	// pattern 8
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"67777777"
		$"56666667"
		$"56666667"
		$"56665667"
		$"56676667"
		$"56666667"
		$"56666667"
		$"55555556"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 0000 0000 0000"
		$"0002 0000 6666 0000"
		$"0003 0000 0000 DDDD"
		$"0004 0000 9999 FFFF"
		$"0005 236F 764F 8D79"
		$"0006 3834 9423 AF28"
		$"0007 4C4D BB29 D5E4"

	// pattern 9
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"23232224"
		$"32322222"
		$"23242223"
		$"32424232"
		$"22242423"
		$"22224242"
		$"22232424"
		$"42323242"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 0000 0000 0000"
		$"0002 6560 6666 645A"
		$"0003 DFFF 7D6A 7F8E"
		$"0004 A6BC A793 A5E6"
		$"0005 FFFF 0000 9999"
		$"0006 DDDD 0000 0000"
		$"0007 FFFF FFFF 0000"

	// pattern A
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"22252325"
		$"45222222"
		$"22232523"
		$"52522242"
		$"22223252"
		$"22322325"
		$"25225252"
		$"22422223"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 0000 0000 0000"
		$"0002 BE50 826C 515E"
		$"0003 820E 835E 835E"
		$"0004 50D7 50D7 50D7"
		$"0005 9AF2 613D 5CCB"
		$"0006 DDDD 0000 0000"
		$"0007 E35D C55A 217B"

	// pattern B
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"77677767"
		$"67776777"
		$"77677767"
		$"67776777"
		$"77677767"
		$"67776777"
		$"77677767"
		$"67776777"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 FFFF FFFF FFFF"
		$"0001 0000 0000 0000"
		$"0002 0000 6666 0000"
		$"0003 0000 0000 DDDD"
		$"0004 0000 9999 FFFF"
		$"0005 FFFF 0000 9999"
		$"0006 DDDD 15E1 9B5F"
		$"0007 FFFF 40E4 BD2B"

	// pattern C
		$"0001 0000001C 0000004E 00000000 0000 00000000"
	// black & white pattern
		$"AA 55 AA 55 AA 55 AA 55"
	// pixmap (includes bounds and depth)
		$"00000000 8004 0000 0000 0008 0008"
		$"0000 0000 00000000"
		$"00480000 00480000"
		$"0000 0004 0001 0004"
		$"00000000 0000006E 00000000"
	// 4-bit color image
		$"24212427"
		$"33134244"
		$"43336221"
		$"43202432"
		$"33002616"
		$"42513424"
		$"41424135"
		$"64433342"
	// color table
		$"00000000 0000"
		$"0007"
		$"0000 D943 D943 D943"
		$"0001 B0D7 B0D7 B0D7"
		$"0002 9379 9379 9379"
		$"0003 DDDD A5C1 9F63"
		$"0004 8794 689A 7494"
		$"0005 535E 3B22 45DC"
		$"0006 81AF 36E0 2E31"
		$"0007 4074 6F28 00C0"
	};

//__________________________________________________________________________________________________
// Data Access Mgr

	IncSys "SnarfMan.a.rsrc" 'PACK' (13) as 'PACK' (13, sysHeap, purgeable);
	IncSys "SnarfMan.a.rsrc" 'proc' (-5728) as 'proc' (-5728, sysHeap, locked);	// result handlers
	IncSys "SnarfMan.a.rsrc" 'proc' (-5727) as 'proc' (-5727, sysHeap, locked);	// completion routine

	/* 'rtt#' specifying where result handlers are */
	resource 'rtt#' (-5728) { {
		-5728,							// id of 'proc'
		{
			'unkn',						// typeUnknown
			'colb',						// typeColBreak
			'rowb',						// typeRowBreak
			'bool',						// typeBoolean
			'shor',						// typeSMInt
			'long',						// typeInteger
			'sing',						// typeSMFloat
			'doub',						// typeFloat
			'TEXT',						// typeChar
			'vcha',						// typeVChar
			'date',						// typeDate
			'time',						// typeTime
			'tims',						// typeTimeStamp
			'deci',						// typeDecimal
			'mone'						// typeMoney
		}
	} };

	/* STR# for results handlers */
	resource 'STR#' (-5728, purgeable) { {
		"TRUE",							// typeBoolean - true
		"FALSE",						// typeBoolean - false
		"•Unknown Type•",				// typeUnknown
		"\t",							// typeColBreak
		"\n"							// typeRowBreak
	} };
	
//__________________________________________________________________________________________________
// Dictionary Mgr

	IncSys "DictionaryMgr.a.rsrc" 'dimg' (0) as 'dimg' (-16385, sysHeap, purgeable);
	
//__________________________________________________________________________________________________
// Dialog Mgr

	resource 'ICON' (0, purgeable){
		$"00FFFF00 01FFFF80 03FFFFC0 07FEFFE0"
		$"0FFC7FF0 1FEC67F8 3FC447FC 7FC447FE"
		$"FF4447FF FE4447FF FE4447FF FE4447FF"
		$"FE4447FF FE4447FF FE40071F FE00061F"
		$"FE00061F FE00043F FE00003F FE00007F"
		$"FE00007F FE0000FF FE0000FF FE0001FF"
		$"FE0001FF 7E0403FE 3F0207FC 1FFFFFF8"
		$"0FFFFFF0 07FFFFE0 03FFFFC0 01FFFF80"
	};

	resource 'ICON' (1, purgeable) {
		$"FFFFFFFF 807FFFFF 807FFFFF 807FFFFF"
		$"807FFFFF 807FC0FF 887F003F 887E001F"
		$"887C000F 80780007 80780007 80700003"
		$"8071DDC3 80700003 80700003 8071DD43"
		$"80700003 80700003 8071D703 80700003"
		$"87F00003 81F1EEC3 81F00007 81F00007"
		$"81F0000F 81E0001F 8F80007F 81FFFFFF"
		$"81FFFFFF 81FFFFFF 81FFFFFF FFFFFFFF"
	};

	resource 'ICON' (2, purgeable) {
		$"00018000 0003C000 0003C000 00066000"
		$"00066000 000C3000 000C3000 00181800"
		$"00199800 0033CC00 0033CC00 0063C600"
		$"0063C600 00C3C300 00C3C300 0183C180"
		$"0183C180 0303C0C0 0303C0C0 0603C060"
		$"06018060 0C018030 0C000030 18000018"
		$"18018018 3003C00C 3003C00C 60018006"
		$"60000006 C0000003 FFFFFFFF 7FFFFFFE"
	};

//__________________________________________________________________________________________________
// Disk Initialization

	IncSys "DiskInit.rsrc";

//__________________________________________________________________________________________________
// Edition Mgr

	IncSys "EditionMgr.rsrc";

//__________________________________________________________________________________________________
// Font Mgr

// name of TrueType Font Cache file

resource 'STR ' (-16391, purgeable) { "Font Cache" };

// FOND has flags, family number, unused fields, FOND version, association table, tables

#define FakeFONDFlags \
	proportionalFont, dontUseFractWidthTable, useIntegerExtra, useFractEnable, \
	canAdjustCharSpacing, noCharWidthTable, noImageHeightTable
#define FakeFONDFields  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#define FakeFONDTables { }, { }, { }, { }
#define glyphNameResID	-16488

// FONDs that point to the fonts in ROM (except Geneva 9 since Macintosh Plus doesn’t have it)

	resource 'FOND' (0, "Chicago", sysHeap) {
		FakeFONDFlags, 0, FakeFONDFields, 1, { 12, plain, 12; }, FakeFONDTables
	};

	resource 'FOND' (3, "Geneva", purgeable) {
		FakeFONDFlags, 3, FakeFONDFields, 1, { 9, plain, 393; 12, plain, 396; }, FakeFONDTables
	};
	data 'FONT' (384, "Geneva", purgeable) {};
	IncSys "SystemFonts.rsrc" 'FONT' (393) as 'FONT' (393, purgeable);
	IncSys "SystemFonts.rsrc" 'FONT' (396) as 'FONT' (396, purgeable);

	resource 'FOND' (4, "Monaco", purgeable) {
		FakeFONDFlags, 4, FakeFONDFields, 1, { 9, plain, 521; }, FakeFONDTables
	};

// used by PostScript drivers to create encoding vectors

resource 'STR#' (glyphNameResID, purgeable) { {
	".notdef", "null", "cr", "space",
	"exclam", "quotedbl", "numbersign", "dollar", "percent", "ampersand", "quotesingle",
	"parenleft", "parenright", "asterisk", "plus", "comma", "hyphen", "period", "slash",
	"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
	"colon", "semicolon", "less", "equal", "greater", "question", "at",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
	"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"bracketleft", "backslash", "bracketright", "asciicircum", "underscore", "grave",
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
	"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
	"braceleft", "bar", "braceright", "asciitilde",
	"Adieresis", "Aring", "Ccedilla", "Eacute", "Ntilde", "Odieresis", "Udieresis",
	"aacute", "agrave", "acircumflex", "adieresis", "atilde", "aring", "ccedilla",
	"eacute", "egrave", "ecircumflex", "edieresis", "iacute", "igrave", "icircumflex",
	"idieresis", "ntilde", "oacute", "ograve", "ocircumflex", "odieresis", "otilde",
	"uacute", "ugrave", "ucircumflex", "udieresis",
	"dagger", "degree", "cent", "sterling", "section", "bullet", "paragraph", "germandbls",
	"registered", "copyright", "trademark", "acute", "dieresis", "notequal", "AE", "Oslash",
	"infinity", "plusminus", "lessequal", "greaterequal", "yen", "mu", "partialdiff", "summation",
	"product", "pi", "integral", "ordfeminine", "ordmasculine", "Omega", "ae", "oslash",
	"questiondown", "exclamdown", "logicalnot", "radical", "florin", "approxequal", "Delta",
	"guillemotleft", "guillemotright", "ellipsis", "nobreakspace", "Agrave", "Atilde",
	"Otilde", "OE", "oe", "endash", "emdash", "quotedblleft", "quotedblright", "quoteleft",
	"quoteright", "divide", "lozenge", "ydieresis", "Ydieresis", "fraction", "currency",
	"guilsinglleft", "guilsinglright", "fi", "fl", "daggerdbl", "periodcentered", "quotesinglbase",
	"quotedblbase", "perthousand", "Acircumflex", "Ecircumflex", "Aacute", "Edieresis", "Egrave",
	"Iacute", "Icircumflex", "Idieresis", "Igrave", "Oacute", "Ocircumflex", "apple", "Ograve",
	"Uacute", "Ucircumflex", "Ugrave", "dotlessi", "circumflex", "tilde", "macron", "breve",
	"dotaccent", "ring", "cedilla", "hungarumlaut", "ogonek", "caron", "Lslash", "lslash",
	"Scaron", "scaron", "Zcaron", "zcaron", "brokenbar", "Eth", "eth", "Yacute", "yacute",
	"Thorn", "thorn", "minus", "multiply", "onesuperior", "twosuperior", "threesuperior", "onehalf",
	"onequarter", "threequarters", "franc", "Gbreve", "gbreve", "Idot", "Scedilla", "scedilla",
	"Cacute", "cacute", "Ccaron", "ccaron", "dmacron"
} };

//__________________________________________________________________________________________________
// Gestalt

	IncSys "Gestalt.rsrc" codeAs 'ptch' (5, sysHeap, locked);

//__________________________________________________________________________________________________
// pslt • Nubus pseudo-slot mapping definitions

#define gestaltMacII				6					// Temporary Crap!
#define gestaltMacIIx				7
#define gestaltMacIIcx				8
#define gestaltMacIIci				11
#define	gestaltMacIIfx				13
#define gestaltEclipse				14+6
#define gestaltEclipse33			20+6
#define gestaltSpike				16+6

resource 'pslt' (gestaltMacII, purgeable) {
	horizAscending,	
	0,
		{
		$9,1;
		$A,2;
		$B,3;
		$C,4;
		$D,5;
		$E,6
		}
};

resource 'pslt' (gestaltMacIIx, purgeable) {
	horizAscending,	
	0,
		{
		$9,1;
		$A,2;
		$B,3;
		$C,4;
		$D,5;
		$E,6;
		}
};

resource 'pslt' (gestaltMacIIcx, purgeable) {
	horizAscending,	
	0,
		{
		$9,1;
		$A,2;
		$B,3;
		}
};


resource 'pslt' (gestaltMacIIci, purgeable) {
	horizAscending,	
	0,
		{
		$C,1;
		$D,2;
		$E,3;
		}
};

resource 'pslt' (gestaltMacIIfx, purgeable) {
	horizAscending,	
	0,
		{
		$9,1;
		$A,2;
		$B,3;
		$C,4;
		$D,5;
		$E,6;
		}
};

resource 'pslt' (gestaltEclipse, purgeable) {
	vertAscending,	
	0,
		{
		$A,1;
		$B,2;
		$C,3;
		$D,4;
		$E,5;
		}
};

resource 'pslt' (gestaltSpike, purgeable) {
	vertDescending,	
	0,
		{
		$D,2;
		$E,1;
		}
};

resource 'pslt' (gestaltEclipse33, purgeable) {
	vertAscending,	
	0,
		{
		$A,1;
		$B,2;
		$C,3;
		$D,4;
		$E,5;
		}
};


//__________________________________________________________________________________________________
// Help Mgr

	IncSys "BalloonPack.a.rsrc" 'PACK' (14) as 'PACK' (14, sysHeap, purgeable);
	IncSys "BalloonPack.a.rsrc" 'WDEF' (126) as 'WDEF' (126, sysHeap, purgeable);

	IncSys "BalloonPack.a.rsrc" 'ICN#' (-5696) as 'ICN#' (-5696, sysHeap, purgeable);		// Finder's help icon
	IncSys "BalloonPack.a.rsrc" 'icl4' (-5696) as 'icl4' (-5696, sysHeap, purgeable);		// Finder's help icon
	IncSys "BalloonPack.a.rsrc" 'ics#' (-5696) as 'ics#' (-5696, sysHeap, purgeable);		// Finder's help icon
	IncSys "BalloonPack.a.rsrc" 'ics4' (-5696) as 'ics4' (-5696, sysHeap, purgeable);		// Finder's help icon
	IncSys "BalloonPack.a.rsrc" 'STR#' (-5696) as 'STR#' (-5696, sysHeap, purgeable);		// help misc strings
	IncSys "BalloonPack.a.rsrc" 'STR ' (-5696) as 'STR ' (-5696, sysHeap, purgeable);		// help misc strings
	IncSys "BalloonPack.a.rsrc" 'STR ' (-5695) as 'STR ' (-5695, sysHeap, purgeable);		// help misc strings
	IncSys "BalloonPack.a.rsrc" 'DLOG' (-5696) as 'DLOG' (-5696, sysHeap, purgeable);		// help menu about
	IncSys "BalloonPack.a.rsrc" 'DITL' (-5696) as 'DITL' (-5696, sysHeap, purgeable);		// help menu about
	IncSys "BalloonPack.a.rsrc" 'dctb' (-5696) as 'dctb' (-5696, sysHeap, purgeable);		// so that we get a color dialog

	IncSys "BalloonPack.a.rsrc" 'PICT' (-5696) as 'PICT' (-5696, sysHeap, purgeable);		// BWAboutPict PICT
	IncSys "BalloonPack.a.rsrc" 'PICT' (-5695) as 'PICT' (-5695, sysHeap, purgeable);		// BWAboutText PICT
	IncSys "BalloonPack.a.rsrc" 'PICT' (-5694) as 'PICT' (-5694, sysHeap, purgeable);		// BWBalloonIcon PICT

	IncSys "BalloonPack.a.rsrc" 'MENU' (-16490) as 'MENU' (-16490, sysHeap);
	IncSys "BalloonPack.a.rsrc" 'hmnu' (-16490) as 'hmnu' (-16490, sysHeap, purgeable);		// help menu strings
	IncSys "BalloonPack.a.rsrc" 'hmnu' (-16489) as 'hmnu' (-16489, sysHeap, purgeable);		// MF menu strings
	IncSys "BalloonPack.a.rsrc" 'ics#' (-16490) as 'ics#' (-16490, sysHeap);				// help menu ics#
	IncSys "BalloonPack.a.rsrc" 'ics8' (-16490) as 'ics8' (-16490, sysHeap, purgeable);		// help menu ics8
	IncSys "BalloonPack.a.rsrc" 'ics4' (-16490) as 'ics4' (-16490, sysHeap, purgeable);		// help menu ics4

//__________________________________________________________________________________________________
// Icon Utilities

	IncSys "IconUtils.rsrc";
	IncSys "GenericIcons.rsrc";

//__________________________________________________________________________________________________
// International (includes Script Mgr)

	IncSys "InternationalPACK.a.rsrc" codeAs 'PACK' (6, sysHeap, purgeable);	// Int'l Utilities package
	IncSys "ScriptMgrPatch.rsrc" codeAs 'ptch' (4, sysHeap, locked);
	IncSys "ScriptMgrROMPatch.rsrc" codeAs 'ptch' (39, sysHeap, locked);
	IncSys "ScriptMgrExtensions.rsrc" codeAs 'ptch' (27, sysHeap, locked);

	IncSys "International.rsrc" 'itlc' (0) as 'itlc' (0, sysHeap, purgeable);
	IncSys "International.rsrc" 'itlm' (0) as 'itlm' (0, sysHeap, purgeable);
	IncSys "International.rsrc" 'itlb' (0) as 'itlb' (0, "Roman", sysHeap, purgeable);
	IncSys "International.rsrc" 'itl0' (0) as 'itl0' (0, "U.S.", sysHeap, purgeable);							//	<259>
	IncSys "International.rsrc" 'INTL' (0) as 'INTL' (0, "U.S.", purgeable);	// ??? should this be sysHeap?		<259>
	IncSys "International.rsrc" 'itl1' (0) as 'itl1' (0, "U.S.", sysHeap, purgeable);							//	<259>
	IncSys "International.rsrc" 'INTL' (1) as 'INTL' (1, "U.S.", purgeable);	// ??? should this be sysHeap?		<259>
	IncSys "RomanITL2.a.rsrc" codeAs 'itl2' (0, "U.S.", sysHeap, purgeable);					//	<259>
	IncSys "itl4Roman.a.rsrc" codeAs 'itl4' (0, "U.S.", sysHeap, purgeable);					//	<258><259>

	IncSys "International.rsrc" 'SICN' (0) as 'SICN' (0, sysHeap, purgeable);
	IncSys "International.rsrc" 'KSWP' (0) as 'KSWP' (0, sysHeap);

	#include $$Shell("Keyboard")"Kbd.r"

	IncSys "International.rsrc" 'kcs#' (0) as 'kcs#' (0, sysHeap, purgeable);
	IncSys "International.rsrc" 'kcs4' (0) as 'kcs4' (0, sysHeap, purgeable);
	IncSys "International.rsrc" 'kcs8' (0) as 'kcs8' (0, sysHeap, purgeable);
	IncSys "International.rsrc" 'kcs#' (-16491) as 'kcs#' (-16491, sysHeap, purgeable);
	IncSys "International.rsrc" 'kcs4' (-16491) as 'kcs4' (-16491, sysHeap, purgeable);
	IncSys "International.rsrc" 'kcs8' (-16491) as 'kcs8' (-16491, sysHeap, purgeable);
	IncSys "International.rsrc" 'STR#' (-16491) as 'STR#' (-16491, sysHeap, purgeable);
	IncSys "International.rsrc" 'hmnu' (-16491) as 'hmnu' (-16491, sysHeap, purgeable);	// Keyboard menu help strings	<185>
	IncSys "International.rsrc" 'DLOG' (-16491) as 'DLOG' (-16491, sysHeap, purgeable);	// About Keyboards…				<208>
	IncSys "International.rsrc" 'DITL' (-16491) as 'DITL' (-16491, sysHeap, purgeable);	// About Keyboards…				<208>
	IncSys "International.rsrc" 'DLOG' (-16492) as 'DLOG' (-16492, sysHeap, purgeable);	// About Keyboards…				<208>
	IncSys "International.rsrc" 'DITL' (-16492) as 'DITL' (-16492, sysHeap, purgeable);	// About Keyboards…				<208>

//__________________________________________________________________________________________________
// List Mgr

	IncSys "ListMgrPACK.a.rsrc" codeAs 'PACK' (0, sysHeap, purgeable);	// List Mgr package
	IncSys "TextLDEF.a.rsrc" codeAs 'LDEF' (0, sysHeap, purgeable);

// LDEF for a list with icons (must have the same ID as the control panel pattern)
	IncSys "IconLDEF.a.rsrc" codeAs 'LDEF' (19, sysHeap, purgeable);

//__________________________________________________________________________________________________
// Menu Mgr

	IncSys "StandardMDEF.a.rsrc" codeAs 'MDEF' (0, sysHeap);
	IncSys "StandardMBDF.a.rsrc" codeAs 'MBDF' (0, sysHeap, locked);

// color apple icon for menu bar

	resource 'ics#' (-16386, sysHeap, purgeable) { {
		$"00 00 00 60 00 C0 00 80 0E 70 1F F8 3F E0 3F E0"
		$"3F E0 3F F0 3F F8 1F F8 1F F8 0F F0 06 60 00 00",

		$"00 00 00 60 00 C0 00 80 0E 70 1F F8 3F E0 3F E0"
		$"3F E0 3F F0 3F F8 1F F8 1F F8 0F F0 06 60 00 00"
	} };

	resource 'ics8' (-16386, sysHeap, purgeable) {
		$"00000000000000000000000000000000"
		$"000000000000000000E3E30000000000"
		$"0000000000000000E3E3000000000000"
		$"0000000000000000E300000000000000"
		$"00000000E3E3E30000E3E3E300000000"
		$"000000E3E3E3E3E3E3E3E3E3E3000000"
		$"00000505050505050505050000000000"
		$"00000505050505050505050000000000"
		$"00001717171717171717170000000000"
		$"00001717171717171717171700000000"
		$"0000D8D8D8D8D8D8D8D8D8D8D8000000"
		$"00000020202020202020202020000000"
		$"00000020202020202020202020000000"
		$"00000000ECECECECECECECEC00000000"
		$"0000000000ECEC0000ECEC0000000000"
		$"00000000000000000000000000000000"
	};

	resource'ics4'(-16386, sysHeap, purgeable){
		$"0000000000000000"
		$"0000000008800000"
		$"0000000088000000"
		$"0000000080000000"
		$"0000888008880000"
		$"0008888888888000"
		$"0011111111100000"
		$"0011111111100000"
		$"0022222222200000"
		$"0022222222220000"
		$"0033333333333000"
		$"0004444444444000"
		$"0004444444444000"
		$"0000666666660000"
		$"0000066006600000"
		$"0000000000000000"
	};

//__________________________________________________________________________________________________
// Notification Mgr

	resource 'DLOG' (1, sysHeap, purgeable) {
		{0, 0, 158, 448}, dBoxProc, invisible, noGoAway, 0, 1, "", alertPositionMainScreen
	};
	resource 'DITL' (1, sysHeap, purgeable) { {
		{130, 355, 150, 425}, Button { enabled, "OK" };
		{126, 351, 154, 429}, UserItem { disabled };
		{12, 68, 118, 430}, StaticText { disabled, "" };
		{8, 18, 40, 50}, Icon { disabled, 1 };
	} };

	resource 'SICN' (-16385, sysHeap, purgeable) { {	// alarm icon
		$"1084 0888 0000 0490 5005 07F0 1C1C 1084"
		$"1084 1084 1084 1104 1204 180C 07F0 0FF8"
	} };

//__________________________________________________________________________________________________
// Parity

	resource 'ALRT' (-16392) {
		{82, 140, 195, 500}, -16392, silentStages, alertPositionMainScreen
	};
	resource 'DITL' (-16392, purgeable) { {
		{80, 266, 100, 344}, Button { enabled, "Continue" };
		{80, 174, 100, 252}, Button { enabled, "Shut Down" };
		{9, 78, 65, 350}, StaticText { disabled,
			"Parity has been disabled because the parity circuitry is not functioning.  "
			"You may continue without parity checking."
		}
	} };

//__________________________________________________________________________________________________
// Power Mgr

	resource 'SICN' (-16386, sysHeap, locked) { {		// battery icon
		$"1084 0888 0000 0490 5005 0000 1C38 F7EF"
		$"8001 8801 9C39 8801 8001 8001 8001 FFFF"
	} };

	resource 'STR ' (-16386, sysHeap, locked) {
		"You are now running on reserve power.  "
		"You will be able to continue working for a short time.  "
		"If you can, plug in your power adapter to begin recharging the battery."
	};

	resource 'STR ' (-16387, sysHeap, locked) {
		"Only 50% of the battery’s reserve power remains.  "
		"Plug in your power adapter to begin recharging the battery as soon as possible."
	};

	resource 'STR ' (-16388, sysHeap, locked) {
		"Only 25% of the battery’s reserve power remains.  "
		"Please put your Macintosh to sleep and plug in your power adapter.  "
		"You might not be able to wake the computer again until the power adapter is connected."
	};

	resource 'STR ' (-16389, sysHeap, locked) {
		"No battery reserve power remains.  "
		"The Macintosh will go to sleep within 10 seconds to preserve the contents of memory.  "
		"Good Night."
	};

	resource 'STR ' (-16511, sysHeap, locked) {
		"You are now running on reserve power and your screen has been dimmed.  "
		"You will be able to continue working for a short time.  "
		"Please plug in your power adapter to begin recharging the battery."
	};
	resource 'STR ' (-16512, sysHeap, locked) {
		"Very little of the battery’s reserve power remains.  "
		"Please put your Macintosh to sleep and plug in your power adapter immediately.  "
		"Doing so will help extend the life of your battery."
	};
	resource 'STR ' (-16513, sysHeap, locked) {
		"No battery reserve power remains.  "
		"The Macintosh will go to sleep within 10 seconds to preserve the contents of memory.  Good Night."
	};
	resource 'STR ' (-16514, sysHeap, locked) {
		"Your battery is not charging.  "
		"Check the power adapter and electrical outlet connections."
	};
	resource 'STR ' (-16515, sysHeap, locked) {
		"Your battery has failed to charge and may need to be replaced.  "
		"Check the power adapter and electrical outlet connections."
	};

	resource 'STR ' (-16516, sysHeap, locked) {
		"Very little of the battery’s reserve power remains.  "
		"Please put your Macintosh to sleep and plug in your power adapter immediately."
	};


	resource 'ALRT' (-16386, sysHeap) {
		{82, 62, 226, 446}, -16386, silentStages, alertPositionMainScreen
	};
	resource 'DITL' (-16386, sysHeap) { {
		{110, 304, 130, 364}, Button { enabled, "Sleep" },
		{110, 224, 130, 284}, Button { enabled, "Cancel" },
		{10, 32, 42, 64}, Icon { disabled, 2 },
		{8, 112, 93, 374}, StaticText { disabled,
			"Putting the computer to sleep will cause you to lose some network services, "
			"including file servers.  "
			"Check your connections in the Chooser next time you wake the Macintosh."
		},
	} };

	resource 'ALRT' (-16387, sysHeap) {
		{82, 62, 226, 446}, -16387, silentStages, alertPositionMainScreen
	};
	resource 'DITL' (-16387, sysHeap) { {
		{110, 304, 130, 364}, Button { enabled, "Sleep" },
		{110, 224, 130, 284}, Button { enabled, "Cancel" },
		{10, 32, 42, 64}, Icon { disabled, 2 },
		{8, 112, 93, 374}, StaticText { disabled,
			"Putting the computer to sleep may cause you to lose some network services.  "
			"Check your connections in the Chooser next time you wake the Macintosh."
		},
	} };

	resource 'ALRT' (-16388, sysHeap) {
		{82, 62, 226, 446}, -16388, silentStages, alertPositionMainScreen
	};
	resource 'DITL' (-16388, sysHeap) { {
		{110, 304, 130, 364}, Button { enabled, "Sleep" },
		{110, 224, 130, 284}, Button { enabled, "Cancel" },
		{10, 32, 42, 64}, Icon { disabled, 2 },
		{8, 112, 93, 374}, StaticText { disabled,
			"AppleTalk is currently in use by an application.   "
			"Putting the computer to sleep will cause problems for the application, "
			"including possible data loss or system crash!"
		},
	} };

//__________________________________________________________________________________________________
// PPC

#define hOK				2
#define hCancel			3
#define hName 			4
#define hPassword		5
#define hGuest			6
#define hGuestDisabled	7
#define hRegistered		8

resource 'STR#' (-16409, sysHeap, purgeable) { {
	"PPCToolBox",
	// OK button
	"To confirm your entries and connect to the computer you selected, click this button.",

	// Cancel button
	"To close the dialog box without connecting to the computer, click this button.",

	// Name text box
	"Type your name here as it is registered on the computer you are connecting to.",

	// Password text box
	"Type your password exactly as it is registered on the computer you wish to access. "
	"Use uppercase and lowercase letters if necessary.",

	// Guest radio button
	"Click this button to connect to the computer as a guest. "
	"As a guest you do not have to enter your name or password.",

	// Guest radio button dimmed
	"Not available because the computer you’re connecting to does not allow guest access.",

	// Registered user radio button
	"Click this button to connect to the computer as a registered user. "
	"As such, you will have to enter your name and perhaps a password to connect.",
}};

	resource 'STR#' (-16410, sysHeap, purgeable) { {
		"Users & Groups Data File";
	} };

resource 'hdlg' (-16409, sysHeap, purgeable) {
	HelpMgrVersion,								// Help Version
	0,											// offset to 1st item
	0,											// options
	0,											// theProc
	0,											// variant
	HMSkipItem {},								// skip missing message
	{
		HMStringResItem {						// 1 - OK Button
			{0,0}, {0,0,0,0},
			-16409, hOK,
			-16409, hOK,						// enabled always
			-16409, hOK,
			-16409, hOK
		},

		HMStringResItem {						// 2 - Cancel button
			{0,0}, {0,0,0,0},
			-16409, hCancel,
			-16409, hCancel,					// enabled always
			-16409, hCancel,
			-16409, hCancel
		},
		HMSkipItem {},							// 3 - ""
		HMSkipItem {},							// 4 - "Name:"
		HMStringResItem {						// 5 - EditBox for user name
			{0,0}, {0,0,0,0},
			-16409, hName,
			-16409, hName,
			-16409, hName,
			-16409, hName
		},
		HMSkipItem {},							// 6 - "Password"
		HMStringResItem {						// 7 - EditBox for password
			{0,0}, {0,0,0,0},
			-16409, hPassword,
			-16409, hPassword,
			-16409, hPassword,
			-16409, hPassword
		},

		HMStringResItem {						// 8 - Guest radio button
			{0,0}, {0,0,0,0},
			-16409, hGuest,
			-16409, hGuestDisabled,
			-16409, hGuest,
			-16409, hGuest
		},

		HMStringResItem {						// 9 - Registered user radio button
			{0,0}, {0,0,0,0},
			-16409, hRegistered,
			-16409, hRegistered,				// enabled always
			-16409, hRegistered,
			-16409, hRegistered
		},
		// no help for other dialog items
}};

	resource 'ICN#' (-16411, sysHeap) { {
		$"00010000 00028000 00044000 00082000"
		$"00101000 00200800 00400400 00803F00"
		$"01004080 02008040 04013020 0801C810"
		$"100E7F8F 20023007 40010007 80008007"
		$"40006007 20001FE7 1000001F 0800000F"
		$"04028010 02028020 01028040 00828080"
		$"0047C100 00244200 00044400 0007C000"
		$"00054000 BFF93FFD 00028000 BFFC7FFD";

		$"00010000 00038000 0007C000 000FE000"
		$"001FF000 003FF800 007FFC00 00FFFE00"
		$"01FFFF00 03FFFF80 07FFFFC0 0FFFFFE0"
		$"1FFFFFFF 3FFFFFFF 7FFFFFFF FFFFFFFF"
		$"7FFFFFFF 3FFFFFFF 1FFFFFFF 0FFFFFFF"
		$"07FFFFF0 03FFFFE0 01FFFFC0 00FFFF80"
		$"007FFF00 003FFE00 000FFC00 0007F000"
		$"0003E000 BFFFFFFD BFFEFFFD BFFC7FFD"
	} };

	/* PromptForUser dialog */

	resource 'DLOG' (-16409, sysHeap) {
		{44, 70, 289, 441}, dBoxProc, invisible, noGoAway, 0, -16409, "", alertPositionMainScreen
	};
	resource 'DITL' (-16409, sysHeap) { {
		{205, 285, 225, 355}, Button { enabled, "OK" };
		{205, 55, 225, 125}, Button { enabled, "Cancel" };
		{40, 48, 73, 360}, StaticText { disabled, "Link to “^0” on “^1” as :" };
		{126, 63, 143, 142}, StaticText { disabled, "Name:" };
		{126, 144, 142, 356}, EditText { enabled, "" };
		{152, 63, 169, 136}, StaticText { disabled, "Password:" };
		{152, 144, 168, 204}, EditText { enabled, "" };
		{77, 63, 95, 120}, RadioButton { enabled, "Guest" };
		{95, 63, 113, 185}, RadioButton { enabled, "Registered User" };
		{152, 220, 169, 356}, StaticText { disabled, "" };
		{-1,-1,-1,-1}, HelpItem { disabled,HMScanhdlg{-16409}}
	} };

	/* color table for PromptForUser dialog */
	data 'dctb' (-16409) {
		$"0000 0000 0000 FFFF"                                /* ........ */
	};

	resource 'STR ' (-16409, sysHeap) { "(Clear text)" };
	resource 'STR ' (-16410, sysHeap) { "(Scrambled)" };

	resource 'ALRT' (-16411, sysHeap) {
		{53, 78, 198, 423}, -16411, beepStages, centerParentWindow
	};
	resource 'DITL' (-16411, sysHeap) { {
		{117, 132, 137, 212}, Button { enabled, "OK" };
		{10, 65, 107, 340}, StaticText { disabled, "^0" };
	} };

	resource 'STR#' (-16411, sysHeap, purgeable) { {
		"Sorry, this password was refused.  Please try again.";
		"Sorry, this user name was refused.  Please try another name.";
	} };

type 'ppci'
{
	byte;		// freePortMin
	byte;		// freePortMax
	byte;		// freeLocSessMin
	byte;		// freeLocSessMax
	byte;		// freeRemSessMin
	byte;		// freeRemSessMax
	byte;		// freeIPMSessMin
	byte;		// freeIPMSessMax
	byte;		// ADSP time out (6 = 1 sec)
	byte;		// ADSP Retries
	byte;		// NBP time out interval 8-tick units
	byte;		// NBP retries
	pstring;	// NBP Type of PPC Toolbox.
};

resource 'ppci' (-16409)
{
	1,2,			// Min,Max of PPC Ports
	1,2,			// Min,Max of Loc Sess
	1,2,			// Min,Max of Rem Sess
	0,1,			// Min,Max of IPM Sess
	30,				// 5 Seconds
	4,				// 4 Retries
	1,				// NBP Lookup Interval <264>
	18,				// NBP Lookup Count <264>
	"PPCToolBox"	// NBP Type of PPC Toolbox.
};

	/* PPC Browser resources */

	IncSys "PPCBrowser.a.rsrc" 'PACK' (9) as 'PACK' (9, sysHeap, purgeable);		// PPC Browser
	IncSys "PPCBrowser.a.rsrc" 'ppcc' (-5856) as 'ppcc' (-5856, sysHeap);			// PPC Browser configuration resource
	IncSys "PPCBrowser.a.rsrc" 'DLOG' (-5856) as 'DLOG' (-5856, sysHeap);			// PPC Browser dialog
	IncSys "PPCBrowser.a.rsrc" 'DITL' (-5856) as 'DITL' (-5856, sysHeap, purgeable);// PPC Browser dialog item list
	IncSys "PPCBrowser.a.rsrc" 'STR#' (-5856) as 'STR#' (-5856, sysHeap);			// PPC Browser strings
	IncSys "PPCBrowser.a.rsrc" 'hdlg' (-5856) as 'hdlg' (-5856, sysHeap, purgeable);// PPC Browser help dialog template

//__________________________________________________________________________________________________
// Printing

	IncSys "PrintDriver.a.rsrc" codeAs 'DRVR' (2, ".Print", sysHeap, locked, purgeable);

//__________________________________________________________________________________________________
// Process Mgr

	IncSys "Scheduler.rsrc";
	IncSys "DAHandler.rsrc";

//__________________________________________________________________________________________________
// SANE

	IncSys "FPHW.rsrc" 'RSRC' (0) as 'PACK' (4, sysHeap, locked);
	IncSys "ElemsHW.rsrc" 'RSRC' (0) as 'PACK' (5, sysHeap, locked);


//__________________________________________________________________________________________________
// QuickDraw

	resource 'STR ' (-16454, locked) { "8•24 GC" };	// name of the Tiburon patch file

	IncSys "QDciPatchROM.a.rsrc" codeAs 'ptch' (26, sysHeap, locked);
	IncSys "QuickDrawPatchII.rsrc" codeAs 'ptch' (32, sysHeap, locked);	 // 32-Bit QuickDraw
	IncSys "PictUtilities.rsrc" codeAs 'PACK' (15, sysHeap, purgeable);

// 8 standard QuickDraw colors
	resource 'clut' (127, sysHeap) { {
		blackRGB;
		$FC00, $F37D, $052F;
		$F2D7, $0856, $84EC;
		$DD6B, $08C2, $06A2;
		$0241, $AB54, $EAFF;
		$0000, $8000, $11B0;
		$0000, $0000, $D400;
		whiteRGB;
	}; };

	type 'clut' (5) {
		unsigned hex longint = 5;								/* ctSeed				*/
		hex integer = $8000;									/* ctFlags				*/
		integer = $$Countof(ColorSpec) - 1;						/* ctSize				*/
		wide array ColorSpec {
				integer = 0;									/* value				*/
				unsigned hex integer;							/* RGB:	red				*/
				unsigned hex integer;							/*		green			*/
				unsigned hex integer;							/*		blue			*/
		};
	};

	type 'clut' (9) {
		unsigned hex longint = 9;								/* ctSeed				*/
		hex integer = $8000;									/* ctFlags				*/
		integer = $$Countof(ColorSpec) - 1;						/* ctSize				*/
		wide array ColorSpec {
				integer = 0;									/* value				*/
				unsigned hex integer;							/* RGB:	red				*/
				unsigned hex integer;							/*		green			*/
				unsigned hex integer;							/*		blue			*/
		};
	};

// dimmed 4-bit
	resource 'clut' (5, sysHeap) { {
		$7FFF, $8000, $7FFF,
		$7C00, $737E, $0000,
		$7FFE, $0000, $0000,
		$DD6B, $08C2, $06A2,
		$F2D7, $0856, $84EC,
		$46E3, $0000, $A53E,
		$0000, $0000, $D400,
		$0241, $AB54, $EAFF,
		$1F21, $B793, $1431,
		$0000, $64AF, $11B0,
		$5600, $2C9D, $0524,
		$90D7, $7160, $3A34,
		$3FFD, $4004, $3FFD,
		$0000, $0003, $0000,
		$4000, $4000, $4000,
		$0000, $0000, $0000,
	} };

// dimmed 8-bit
	resource 'clut' (9, sysHeap) { {
		$7FFF, $8000, $7FFF,
		$7FFD, $8000, $4CC9,
		$7FFF, $7FFE, $1997,
		$7FFE, $8000, $0000,
		$7FFC, $8002, $0000,
		$7FFE, $8000, $0000,
		$7FFE, $4CCD, $7FFE,
		$7FFC, $4CCD, $4CCC,
		$7FFE, $4CCB, $1996,
		$7FFD, $4CCD, $0000,
		$7FFF, $4CCD, $0000,
		$7FFD, $4CCD, $0000,
		$7FFD, $199A, $7FFD,
		$7FFF, $1999, $4CCB,
		$7FFD, $1999, $1999,
		$7FFC, $199C, $0000,
		$7FFE, $199A, $0000,
		$7FFC, $199A, $0000,
		$7FFC, $0000, $7FFC,
		$7FFE, $0000, $4CCA,
		$7FFC, $0000, $1998,
		$7FFF, $0000, $0000,
		$7FFD, $0000, $0000,
		$7FFF, $0000, $0000,
		$7FFF, $0000, $7FFF,
		$7FFD, $0000, $4CC9,
		$FFFF, $3333, $9999,
		$FFFF, $3333, $6666,
		$FFFF, $3333, $3333,
		$FFFF, $3333, $0000,
		$FFFF, $0000, $FFFF,
		$FFFF, $0000, $CCCC,
		$FFFF, $0000, $9999,
		$FFFF, $0000, $6666,
		$FFFF, $0000, $3333,
		$FFFF, $0000, $0000,
		$4CCB, $8000, $7FFF,
		$4CC9, $8002, $4CC9,
		$4CCB, $7FFE, $1997,
		$4CCA, $8002, $0000,
		$4CCC, $8000, $0000,
		$4CCA, $8002, $0000,
		$4CCA, $4CCF, $7FFE,
		$4CCC, $4CCB, $4CCC,
		$4CCA, $4CCD, $1996,
		$4CC9, $4CCF, $0000,
		$4CCB, $4CCD, $0000,
		$4CC9, $4CCF, $0000,
		$4CC9, $199C, $7FFD,
		$4CCB, $1999, $4CCB,
		$4CC9, $199A, $1999,
		$4CCC, $199A, $0000,
		$4CCA, $199C, $0000,
		$4CCC, $1999, $0000,
		$4CCC, $0000, $7FFC,
		$4CCA, $0000, $4CCA,
		$4CCC, $0000, $1998,
		$4CCB, $0000, $0000,
		$CCCC, $6666, $3333,
		$CCCC, $6666, $0000,
		$CCCC, $3333, $FFFF,
		$CCCC, $3333, $CCCC,
		$CCCC, $3333, $9999,
		$CCCC, $3333, $6666,
		$CCCC, $3333, $3333,
		$CCCC, $3333, $0000,
		$CCCC, $0000, $FFFF,
		$CCCC, $0000, $CCCC,
		$CCCC, $0000, $9999,
		$CCCC, $0000, $6666,
		$CCCC, $0000, $3333,
		$CCCC, $0000, $0000,
		$1996, $8000, $7FFE,
		$1998, $7FFD, $4CCC,
		$1996, $7FFE, $1996,
		$1999, $7FFE, $0000,
		$1997, $8000, $0000,
		$1999, $7FFE, $0000,
		$1999, $4CCB, $7FFD,
		$1997, $4CCB, $4CCB,
		$1999, $4CCA, $1999,
		$1998, $4CCB, $0000,
		$1996, $4CCD, $0000,
		$1998, $4CCA, $0000,
		$1998, $1999, $7FFC,
		$1996, $199A, $4CCA,
		$1998, $1997, $1998,
		$1997, $199A, $0000,
		$1999, $1999, $0000,
		$1997, $199A, $0000,
		$1997, $0000, $7FFF,
		$1999, $0000, $4CC9,
		$9999, $6666, $9999,
		$9999, $6666, $6666,
		$9999, $6666, $3333,
		$9999, $6666, $0000,
		$9999, $3333, $FFFF,
		$9999, $3333, $CCCC,
		$9999, $3333, $9999,
		$9999, $3333, $6666,
		$9999, $3333, $3333,
		$9999, $3333, $0000,
		$9999, $0000, $FFFF,
		$9999, $0000, $CCCC,
		$9999, $0000, $9999,
		$9999, $0000, $6666,
		$9999, $0000, $3333,
		$9999, $0000, $0000,
		$0000, $7FFE, $7FFE,
		$0000, $7FFE, $4CCC,
		$0000, $7FFD, $1996,
		$0000, $8000, $0000,
		$0000, $8000, $0000,
		$0000, $8000, $0000,
		$0000, $4CCD, $7FFD,
		$0000, $4CCB, $4CCB,
		$0000, $4CCB, $1999,
		$0000, $4CCD, $0000,
		$0000, $4CCB, $0000,
		$0000, $4CCB, $0000,
		$0000, $199A, $7FFC,
		$0000, $1999, $4CCA,
		$0000, $1999, $1998,
		$0000, $199A, $0000,
		$6666, $9999, $3333,
		$6666, $9999, $0000,
		$6666, $6666, $FFFF,
		$6666, $6666, $CCCC,
		$6666, $6666, $9999,
		$6666, $6666, $6666,
		$6666, $6666, $3333,
		$6666, $6666, $0000,
		$6666, $3333, $FFFF,
		$6666, $3333, $CCCC,
		$6666, $3333, $9999,
		$6666, $3333, $6666,
		$6666, $3333, $3333,
		$6666, $3333, $0000,
		$6666, $0000, $FFFF,
		$6666, $0000, $CCCC,
		$6666, $0000, $9999,
		$6666, $0000, $6666,
		$6666, $0000, $3333,
		$6666, $0000, $0000,
		$0000, $8000, $7FFE,
		$0000, $7FFE, $4CCC,
		$0000, $7FFE, $1996,
		$0000, $8000, $0000,
		$0000, $7FFE, $0000,
		$0000, $8000, $0000,
		$0000, $4CCD, $7FFD,
		$0000, $4CCA, $4CCB,
		$0000, $4CCB, $1999,
		$0000, $4CCD, $0000,
		$0000, $4CCD, $0000,
		$0000, $4CCB, $0000,
		$0000, $199A, $7FFC,
		$0000, $199A, $4CCA,
		$3333, $9999, $9999,
		$3333, $9999, $6666,
		$3333, $9999, $3333,
		$3333, $9999, $0000,
		$3333, $6666, $FFFF,
		$3333, $6666, $CCCC,
		$3333, $6666, $9999,
		$3333, $6666, $6666,
		$3333, $6666, $3333,
		$3333, $6666, $0000,
		$3333, $3333, $FFFF,
		$3333, $3333, $CCCC,
		$3333, $3333, $9999,
		$3333, $3333, $6666,
		$3333, $3333, $3333,
		$3333, $3333, $0000,
		$3333, $0000, $FFFF,
		$3333, $0000, $CCCC,
		$3333, $0000, $9999,
		$3333, $0000, $6666,
		$3333, $0000, $3333,
		$3333, $0000, $0000,
		$0000, $8000, $7FFE,
		$0000, $7FFD, $4CCC,
		$0000, $7FFE, $1996,
		$0000, $8000, $0000,
		$0000, $8000, $0000,
		$0000, $8000, $0000,
		$0000, $4CCD, $7FFD,
		$0000, $4CCB, $4CCB,
		$0000, $4CCB, $1999,
		$0000, $4CCB, $0000,
		$0000, $CCCC, $3333,
		$0000, $CCCC, $0000,
		$0000, $9999, $FFFF,
		$0000, $9999, $CCCC,
		$0000, $9999, $9999,
		$0000, $9999, $6666,
		$0000, $9999, $3333,
		$0000, $9999, $0000,
		$0000, $6666, $FFFF,
		$0000, $6666, $CCCC,
		$0000, $6666, $9999,
		$0000, $6666, $6666,
		$0000, $6666, $3333,
		$0000, $6666, $0000,
		$0000, $3333, $FFFF,
		$0000, $3333, $CCCC,
		$0000, $3333, $9999,
		$0000, $3333, $6666,
		$0000, $3333, $3333,
		$0000, $3333, $0000,
		$0000, $0000, $FFFF,
		$0000, $0000, $CCCC,
		$0000, $0000, $9999,
		$0000, $0000, $6666,
		$0000, $0000, $3333,
		$EEEE, $0000, $0000,
		$DDDD, $0000, $0000,
		$BBBB, $0000, $0000,
		$AAAA, $0000, $0000,
		$8888, $0000, $0000,
		$7777, $0000, $0000,
		$5555, $0000, $0000,
		$4444, $0000, $0000,
		$2222, $0000, $0000,
		$1111, $0000, $0000,
		$0000, $6EED, $0000,
		$0000, $5DDF, $0000,
		$0000, $BBBB, $0000,
		$0000, $AAAA, $0000,
		$0000, $8888, $0000,
		$0000, $7777, $0000,
		$0000, $5555, $0000,
		$0000, $4444, $0000,
		$0000, $2222, $0000,
		$0000, $1111, $0000,
		$0000, $0000, $EEEE,
		$0000, $0000, $DDDD,
		$0000, $0000, $BBBB,
		$0000, $0000, $AAAA,
		$0000, $0000, $8888,
		$0000, $0000, $7777,
		$0000, $0000, $5555,
		$0000, $0000, $4444,
		$0000, $0000, $2222,
		$0000, $0000, $1111,
		$6EEE, $6EEE, $6EEE,
		$5DDD, $5DDD, $5DDD,
		$3BB8, $3BBF, $3BB8,
		$2AAA, $2AAA, $2AAA,
		$0885, $088C, $0885,
		$7777, $7777, $7777,
		$5555, $5555, $5555,
		$4444, $4444, $4444,
		$2222, $2222, $2222,
		$1111, $1111, $1111,
		$0000, $0000, $0000,
	} };

// default MakeITable queue sizes for 3, 4, and 5 bit inverse tables
	resource 'mitq' (0, sysHeap) { { $1300; $2200; $3C00; } };

// standard cursors
	resource 'CURS' (1, sysHeap) {
		$"0C60 0280 0100 0100 0100 0100 0100 0100"
		$"0100 0100 0100 0100 0100 0100 0280 0C60",

		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000",
		{4, 7}
	};

	resource 'CURS' (2, sysHeap) {
		$"0400 0400 0400 0400 0400 FFE0 0400 0400"
		$"0400 0400 0400 0400 0000 0000 0000 0000",

		$"0000 0000 0000 0000 0000 0000 0000 0000"
		$"0000 0000 0000 0000 0000 0000 0000 0000",
		{5, 5}
	};

	resource 'CURS' (3, sysHeap) {
		$"0000 07C0 0460 0460 0460 7C7C 4386 4286"
		$"4386 7C7E 3C7E 0460 0460 07E0 03E0 0000",

		$"0FC0 0FE0 0FF0 0FF0 FFFF FFFE FC7F FC7F"
		$"FC7F FFFF 7FFF 7FFF 0FF0 0FF0 07F0 03E0",
		{8, 8}
	};

	resource 'CURS' (4, sysHeap) {
		$"3F00 3F00 3F00 3F00 4080 8440 8440 8460"
		$"9C60 8040 8040 4080 3F00 3F00 3F00 3F00",

		$"3F00 3F00 3F00 3F00 7F80 FFC0 FFC0 FFC0"
		$"FFC0 FFC0 FFC0 7F80 3F00 3F00 3F00 3F00",
		{8, 8}
	};

// this picture is useful for drawing lines in dialog boxes
// note: the lines appear in smooth gray in color GrafPorts only

	data 'PICT' (11, sysHeap, purgeable) {
		$"00E4" 							// low word of picture size
		$"0000 0000 0001 0001"				// picture frame
		$"0011 02FF"						// version information
		$"0C00"								// header opCode
		$"FFFF FFFF"						// size = -1 for version 2 pictures
		$"0000 0000 0000 0000"				// fixed point bounding box
		$"0001 0000 0001 0000"
		$"FFFF FFFF"						// reserved
		$"001E"
		$"0001"								// clip region
		$"000A"								// region size
		$"0000 0000 0001 0001"
		$"0013"								// PenPixPat
		$"0001"								// pattern type is not a dither pat
		$"AA55 AA55 AA55 AA55"				// B & W pattern
		$"8004"								// row bytes with high bit set
		$"0000 0000 0008 0008"				// bounds
		$"0000"								// version
		$"0000"								// pack type
		$"0000 0000"						// pack size
		$"0048 0000"						// horizontal resolution
		$"0048 0000"						// vertical resolution
		$"0000"								// pixel type = chunky
		$"0004"								// pixel size
		$"0001"								// 1 component per pixel
		$"0004"								// component size
		$"0000 0000"						// no next plane
		$"002A 5734"						// pix map table
		$"0000 0000"						// reserved = 0
		$"0000 0415"						// color table seed
		$"0000"								// color table flags
		$"0007"								// 7 entries below
		$"0000 80E8 80E8 80E8"
		$"0001 7D00 7D00 7D00"
		$"0002 0000 64AF 11B0"
		$"0003 0000 0000 D400"
		$"0004 0241 AB54 EAFF"
		$"0005 F2D7 0856 84EC"
		$"0006 DD6B 08C2 06A2"
		$"0007 FC00 F37D 052F"
		$"1010 1010"						// pixel data
		$"0101 0101"
		$"1010 1010"
		$"0101 0101"
		$"1010 1010"
		$"0101 0101"
		$"1010 1010"
		$"0101 0101"
		$"0030"								// FrameRect
		$"0000 0000 0001 0001"				// rectangle
		$"00FF"								// end the picture
	};

//__________________________________________________________________________________________________
// Resource Mgr

	IncSys "DeCompressDefProc.a.rsrc" codeAs 'dcmp' (0, sysHeap, locked);
	IncSys "DeCompressDefProc1.a.rsrc" codeAs 'dcmp' (1, sysHeap, locked);
	IncSys "GreggyBitsDefProc.a.rsrc" codeAs 'dcmp' (2, sysHeap, locked);	// <209>

//__________________________________________________________________________________________________
// Sound Mgr

	IncSys "SinDrvr.a.rsrc" codeAs 'DRVR' (-16499, ".AppleSoundInput", sysHeap, locked);
	IncSys "SoundPFDProc.rsrc" codeAs 'proc' (-16498, sysHeap, purgeable, locked);
	IncSys "SoundInputProc.rsrc" codeAs 'proc' (-16497, sysHeap, purgeable, locked);
	IncSys "Meter.c.rsrc" codeAs 'CDEF' (62, sysHeap, purgeable);
	IncSys "SinHighLevel.rsrc";

	IncSys "Note.c.rsrc" codeAs 'snth' (2049, sysHeap, purgeable, locked);
	IncSys "Wave.c.rsrc" codeAs 'snth' (2051, sysHeap, purgeable, locked);
	IncSys "PartySamp.c.rsrc" codeAs 'snth' (2053, sysHeap, purgeable, locked);
	IncSys "mSamp.c.rsrc" codeAs 'snth' (4101, sysHeap, purgeable, locked);
	IncSys "mNote.c.rsrc" codeAs 'snth' (4097, sysHeap, purgeable, locked);
	IncSys "mWave.c.rsrc" codeAs 'snth' (4099, sysHeap, purgeable, locked);
	IncSys "MACE3.c.rsrc" codeAs 'snth' (11, sysHeap, purgeable, locked);
	IncSys "MACE6.c.rsrc" codeAs 'snth' (13, sysHeap, purgeable, locked);
	IncSys "SnthLoading.rsrc";	// BST#: table of indexes and SnthLoading values for Buffer Stuffers

//__________________________________________________________________________________________________
// Standard File

	IncSys "StandardFile.rsrc";	// see "{StandardFile}StandardFile.r" for details

//__________________________________________________________________________________________________
// Video

	IncSys "TFBDriver.a.rsrc" codeAs 'DRVR' (120, ".Display_Video_Apple_TFB", sysHeap, purgeable);

// Mac IIci (and Erickson, actually) video extension

	IncSys "BuiltInVideoExtension.p.rsrc" codeAs 'mntr' (-4096, purgeable);


	// Translation strings for the various Apple Built-In Video names

	type 'card' as 'STR ';
	resource 'card' (-4080, purgeable) { "Macintosh II Built-In Video" };
	resource 'card' (-4079, purgeable) { "Macintosh IIci Built-In Video" };
	resource 'card' (-4078, purgeable) { "Macintosh A Built-In Video" };
	resource 'card' (-4077, purgeable) { "Macintosh B Built-In Video" };
	resource 'card' (-4076, purgeable) { "Macintosh C Built-In Video" };
	resource 'card' (-4075, purgeable) { "Macintosh D Built-In Video" };
	resource 'card' (-4074, purgeable) { "Macintosh E Built-In Video" };
	resource 'card' (-4073, purgeable) { "Macintosh F Built-In Video" };
	resource 'card' (-4072, purgeable) { "Macintosh G Built-In Video" };

	resource 'STR#' (-4096, purgeable) {
		{
			"Macintosh IIci Built-In Video",
			"Macintosh II Built-In Video",
			"Macintosh A Built-In Video",
			"Macintosh II Built-In Video",
			"Macintosh B Built-In Video",
			"Macintosh Built-In Video",
			"Macintosh C Built-In Video",
			"Macintosh Quadra Built-In Video",
			"Macintosh D Built-In Video",
			"Macintosh PowerBook Built-In Video",
			"Macintosh E Built-In Video",
			"Macintosh Quadra Built-In Video",
			"Macintosh F Built-In Video",
			"Macintosh Classic II Built-In Video",
			"Macintosh G Built-In Video",
			"Macintosh Quadra Built-In Video",
		}
	};


	resource 'RECT' (-4096, purgeable) { { -133, 0,  0, 320} };
	resource 'DITL' (-4096, purgeable) { {
		{ 5, 1, 6, 319 }, UserItem { disabled };			// 1: separating line
		{53, 35, 65, 310 }, Control { enabled, -4080 };		// 2: black & white only
		{73, 35, 85, 310 }, Control { enabled, -4079 };		// 3: up to 4 colors/grays
		{93, 35, 105, 310 }, Control { enabled, -4078 };	// 4: up to 16 colors/grays
		{113, 35, 125, 310 }, Control { enabled, -4077 };	// 5: up to 256 colors/grays
		{ 14, 5, 90, 315 }, UserItem { disabled };			// 6: rectangle for two controls only
		{ 14, 5, 110, 315 }, UserItem { disabled };			// 7: rectangle for two controls only
		{ 14, 5, 130, 315 }, UserItem { disabled };			// 8: rectangle for two controls only
		{8, 15, 20, 215}, StaticText { disabled,
			" Memory Allocation For Built-In Video " VidExtVers
		};
		{23, 15, 47, 310}, StaticText { disabled,
			"Select the maximum number of colors/grays for which you want to reserve memory."
		};
	} };

	resource 'CNTL' (-4080, purgeable) {
		{53, 35, 65, 310}, 0, visible, 1, 0, radioButProcUseWFont, 0,
		"Black & White Only"
	};
	resource 'CNTL' (-4079, purgeable) {
		{73, 35, 85, 310}, 0, visible, 1, 0, radioButProcUseWFont, 0,
		"Up to 4 Colors/Grays"
	};
	resource 'CNTL' (-4078, purgeable) {
		{93, 35, 105, 310}, 0, visible, 1, 0, radioButProcUseWFont, 0,
		"Up to 16 Colors/Grays"
	};
	resource 'CNTL' (-4077, purgeable) {
		{113, 35, 125, 310}, 0, visible, 1, 0, radioButProcUseWFont, 0,
		"Up to 256 Colors/Grays"
	};

// string pieces for describing amount of memory used/freed by the selection
	resource 'STR#' (-4080, purgeable) { {
		"You will reserve an additional ",
		"K of memory for built-in video upon restart.",
		"You will gain an additional ",
		"K of memory for applications upon restart.",
	} };

// appended to the control title to indicate the active selection
	resource 'STR ' (-4080, purgeable) { " (current selection)" };

// alert warning that a restart is needed
	resource 'ALRT' (-4080, purgeable) { {50, 30, 165, 290}, -4080, silentStages, noAutoCenter };
	resource 'DITL' (-4080, purgeable) { {
		{78, 95, 98, 165}, Button { enabled, "OK" };
		{10, 72, 66, 250}, StaticText { disabled,
			"You have made changes that won’t take effect until Restart."
		};
	} };

//__________________________________________________________________________________________________
// EDisk
	resource 'STR#' (-4097, sysHeap, Locked) {
		{
			"The contents of the RAM Disk volume “^1” will be lost by shutting down.  "
			"Do you wish to continue?",
			"OK",
			"Cancel",
		}
	};
//__________________________________________________________________________________________________
// Virtual Memory

	include $$Shell("Misc")"VM.rsrc" not 'ckid';
	resource 'STR ' (-16508, sysheap, locked) { "VM Storage" };

//__________________________________________________________________________________________________
// Window Mgr (includes Control Mgr)

	IncSys "StandardWDEF.a.rsrc" codeAs 'WDEF' (0, sysHeap);
	IncSys "RoundedWDEF.a.rsrc" codeAs 'WDEF' (1, sysHeap, purgeable);
	IncSys "LayerWDEF.c.rsrc" codeAs 'WDEF' (127, sysHeap);

// standard window color table
	resource 'wctb' (0, sysHeap) { {
		wContentColor, whiteRGB;
		wFrameColor, blackRGB;
		wTextColor, blackRGB;
		wHiliteColor, blackRGB;
		wTitleBarColor, whiteRGB;
		wHiliteLight, whiteRGB;
		wHiliteDark, blackRGB;
		wTitleBarLight, whiteRGB;
		wTitleBarDark, blackRGB;
		wDialogLight, $CCCC, $CCCC, $FFFF;
		wDialogDark, blackRGB;
		wTingeLight, $CCCC, $CCCC, $FFFF;
		wTingeDark, $3333, $3333, $6666;
	}; };

	resource 'PAT ' (16, sysHeap, purgeable) { $"AA55AA55AA55AA55" };	// standard desktop pattern
	resource 'PAT ' (15, sysHeap, purgeable) { $"8800220088002200" };	// Portable desktop pattern
	resource 'PAT ' (18, sysHeap, purgeable) { $"0000400000000400" };	// New Portable desktop pattern (2 dots) <5>

// color desktop pattern = sparce pattern for portables with QCD that want ppats

data 'ppat' (18, sysheap, purgeable) {
	$"0001 0000 001C 0000 004E 0000 0000 0000"
	$"0000 0000 AA55 AA55 AA55 AA55 0000 0000"
	$"8004 0000 0000 0008 0008 0000 0000 0000"
	$"0000 0048 0000 0048 0000 0000 0004 0001"
	$"0004 0000 0000 0000 006E 0000 0000 0000"
	$"0000 0000 0000 0010 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0010 0000 0000 0000"
	$"08E4 0000 0007 0000 FFFF FFFF FFFF 0001"
	$"0000 0000 0000 0002 9E66 AA03 D286 0003"
	$"4C0D 8A7A CF28 0004 FFFF FFFF 3333 0005"
	$"FFFF CCCC 9999 0006 FFFF 6666 0000 0007"
	$"CCCC 0000 0000"
};


// color desktop pattern = dithered gray
	resource 'ppat' (16, sysHeap, purgeable) {
		$"AA55AA55AA55AA55",			// black & white pattern
		{0, 0, 8, 8}, 4,				// bounds, bits per pixel
	// 4-bit color image
		$"10101010"
		$"01010101"
		$"10101010"
		$"01010101"
		$"10101010"
		$"01010101"
		$"10101010"
		$"01010101",
	// color table
		{
			$A000, $A000, $A000;
			$6000, $6000, $6000;
			$0000, $64AF, $11B0;
			$0000, $0000, $D400;
			$0241, $AB54, $EAFF;
			$F2D7, $0856, $84EC;
			$DD6B, $08C2, $06A2;
			$FC00, $F37D, $052F;
		}
	};

// standard control color table
	resource 'cctb' (0, sysHeap) { {
		cFrameColor, blackRGB;
		cBodyColor, whiteRGB;
		cTextColor, blackRGB;
		cElevatorColor, whiteRGB;
		cFillPatColor,$5555,$5555,$5555;
		cArrowsLight,whiteRGB;
		cArrowsDark,blackRGB;
		cThumbLight,whiteRGB;
		cThumbDark,blackRGB;
		cHiliteLight,whiteRGB;
		cHiliteDark,blackRGB;
		cTitleBarLight,whiteRGB;
		cTitleBarDark,blackRGB;
		cTingeLight,$CCCC,$CCCC,$FFFF;
		cTingeDark,$3333,$3333,$6666;
	}; };

	IncSys "ButtonCDEF.a.rsrc" codeAs 'CDEF' (0, sysHeap, purgeable);

	IncSys "ScrollBarCDEF.a.rsrc" codeAs 'CDEF' (1, sysHeap, purgeable);
	resource 'PAT ' (17, sysHeap, purgeable) { $"8822882288228822" };	// scroll bar pattern

	data 'pixs' (-14335, sysHeap) {
		$"8006 0000 0000 000B 000B 0004"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
		$"AAAAAAAAAAAA"
	};

	data 'pixs' (-14334, sysHeap) {
		$"8006 0000 0000 000B 000B 0004"
		$"FFFFFFFFFFF0"
		$"F5555F555500"
		$"F5F55F55F500"
		$"F55F5F5F5500"
		$"F55555555500"
		$"FFFF555FFF00"
		$"F55555555500"
		$"F55F5F5F5500"
		$"F5F55F55F500"
		$"F5555F555500"
		$"F00000000000"
	};

	// Unhighlighted right arrow

	data 'pixs' (-10199, sysHeap) {
		$"8008 0000 0000 0010 0010"
		$"9999999999999999"
		$"9000000000000069" 
		$"9022222722222269"
		$"9022222772222269"
		$"9022222737222269"
		$"9027777733722269"
		$"9027333333372269"
		$"9027333333337269"
		$"9027333333372269" 
		$"9027777733722269"
		$"9022222737222269"
		$"9022222772222269"
		$"9022222722222269"
		$"9022222222222269"
		$"9666666666666669"
		$"9999999999999999"
	};

	//	Highlighted right arrow
	
	data 'pixs' (-10200, sysHeap) {
		$"8008 0000 0000 0010 0010"
		$"9999999999999999"
		$"9000000000000069"
		$"9022222922222269"
		$"9022222992222269"
		$"9022222999222269"
		$"9029999999922269"
		$"9029999999992269"
		$"9029999999999269"
		$"9029999999992269"
		$"9029999999922269"
		$"9022222999222269"
		$"9022222992222269"
		$"9022222922222269"
		$"9022222222222269"
		$"9666666666666669"
		$"9999999999999999"
	};
	
	//	Unhighlighted left arrow
	
	data 'pixs' (-10201, sysHeap) {
		$"8008 0000 0000 0010 0010"
		$"9999999999999999"
		$"9000000000000069"
		$"9022222272222269"
		$"9022222772222269"
		$"9022227372222269"
		$"9022273377777269"
		$"9022733333337269"
		$"9027333333337269"
		$"9022733333337269"
		$"9022273377777269"
		$"9022227372222269"
		$"9022222772222269"
		$"9022222272222269"
		$"9022222222222269"
		$"9666666666666669"
		$"9999999999999999"
	};
	
	// Highlighted left arrow
	
	data 'pixs' (-10202, sysHeap) {
		$"8008 0000 0000 0010 0010"
		$"9999999999999999"
		$"9000000000000069"
		$"9022222292222269"
		$"9022222992222269"
		$"9022229992222269"
		$"9022299999999269"
		$"9022999999999269"
		$"9029999999999269"
		$"9022999999999269"
		$"9022299999999269"
		$"9022229992222269"
		$"9022222992222269"
		$"9022222292222269"
		$"9022222222222269"
		$"9666666666666669"
		$"9999999999999999"
	};
	
	// Unhighlighted down arrow
	
	data 'pixs' (-10203, sysHeap) {
		$"8008 0000 0000 0010 0010"
		$"9999999999999999"
		$"9000000000000069"
		$"9022222222222269"
		$"9022277777222269"
		$"9022273337222269"
		$"9022273337222269"
		$"9022273337222269"
		$"9077773337777269"
		$"9027333333372269"
		$"9022733333722269"
		$"9022273337222269"
		$"9022227372222269"
		$"9022222722222269"
		$"9022222222222269"
		$"9666666666666669"
		$"9999999999999999"
	};
	
	// Highlighted down arrow
	
	data 'pixs' (-10204, sysHeap) {
		$"8008 0000 0000 0010 0010"
		$"9999999999999999"
		$"9000000000000069"
		$"9022222222222269"
		$"9022299999222269"
		$"9022299999222269"
		$"9022299999222269"
		$"9022299999222269"
		$"9099999999999269"
		$"9029999999992269"
		$"9022999999922269"
		$"9022299999222269"
		$"9022229992222269"
		$"9022222922222269"
		$"9022222222222269"
		$"9666666666666669"
		$"9999999999999999"
	};
	
	// Unhighlighted up arrow
	
	data 'pixs' (-10205, sysHeap) {
		$"8008 0000 0000 0010 0010"
		$"9999999999999999"
		$"9000000000000069"
		$"9022222222222269"
		$"9022222722222269"
		$"9022227372222269"
		$"9022273337222269"
		$"9022733333722269"
		$"9027333333372269"
		$"9077773337777269"
		$"9022273337222269"
		$"9022273337222269"
		$"9022273337222269"
		$"9022277777222269"
		$"9022222222222269"
		$"9666666666666669"
		$"9999999999999999"
	};
	
	//	Highlighted up arrow
	
	data 'pixs' (-10206, sysHeap) {
		$"8008 0000 0000 0010 0010"
		$"9999999999999999"
		$"9000000000000069"
		$"9022222222222269"
		$"9022222922222269"
		$"9022229992222269"
		$"9022299999222269"
		$"9022999999922269"
		$"9029999999992269"
		$"9099999999999269"
		$"9022299999222269"
		$"9022299999222269"
		$"9022299999222269"
		$"9022299999222269"
		$"9022222222222269"
		$"9666666666666669"
		$"9999999999999999"
	};
	
	//	Horizontal thumb
	
	data 'pixs' (-10207, sysHeap) {
		$"8008 0000 0000 000E 0010"
		$"8111111111111117"
		$"8144444444444447"
		$"8144444444444447"
		$"8144444444444447"
		$"8143151515151447"
		$"8143151515151447"
		$"8143151515151447"
		$"8143151515151447"
		$"8143151515151447"
		$"8143151515151447"
		$"8144444444444447"
		$"8144444444444447"
		$"8144444444444447"
		$"8777777777777777"
	};
	
	//	Vertical thumb
	
	data 'pixs' (-10208, sysHeap) {
		$"8008 0000 0000 0010 000E"
		$"8888888888888800"
		$"1111111111111700"
		$"1444444444444700"
		$"1444333333444700"
		$"1444111111444700"
		$"1444555555444700"
		$"1444111111444700"
		$"1444555555444700"
		$"1444111111444700"
		$"1444555555444700"
		$"1444111111444700"
		$"1444555555444700"
		$"1444111111444700"
		$"1444444444444700"
		$"1444444444444700"
		$"7777777777777700"
	};
	
	//	Colour table used by scroll bar to map tinge colours
	
	resource 'clut' (-10208, sysheap) {
		{
			65535, 65535, 65535,
			52428, 52428, 65535,
			56797, 56797, 56797,
			39321, 39321, 52428,
			43690, 43690, 43690,
			26214, 26214, 39321,
			30583, 30583, 30583,
			13107, 13107, 26214,
			21845, 21845, 21845,
			0, 0, 0
		}
	};
	
	data 'pmap' (-14336, preload, sysHeap) {
		$"00 00 00 00"
		$"80 06"
		$"00 00 00 00 00 0B 00 0B"
		$"00 00"
		$"00 00"
		$"00 00 00 00"
		$"00 48 00 00"
		$"00 48 00 00"
		$"00 00"
		$"00 04"
		$"00 01"
		$"00 04"
		$"00 00 00 00"
		$"00 00 00 00"
		$"00 00 00 00"
	};

	data 'pmap' (-10208, preload, sysHeap) {
		$"00 00 00 00"
		$"80 06"
		$"00 00 00 00 00 0B 00 0B"
		$"00 00"
		$"00 00"
		$"00 00 00 00"
		$"00 48 00 00"
		$"00 48 00 00"
		$"00 00"
		$"00 04"
		$"00 01"
		$"00 04"
		$"00 00 00 00"
		$"00 00 00 00"
		$"00 00 00 00"
	};

	IncSys "PictButtonCDEF.a.rsrc" codeAs 'CDEF' (61, sysHeap, purgeable);

//	popup CDEF (63) with hooks into MDEF 0
//	note that MDEF 0 should be compiled with hasPopupSupport flag
//	Note that the PopupCDEFMDEF.a.rsrc MDEF is used to expand the calcSizeMsg width
//	and is ONLY called by the Popup CDEF.  Since we store PC relative stuff,
//	_LoadResource won't help us, so we keep it around, but it's only 96 bytes.


	IncSys "PopupCDEF.c.rsrc" codeAs 'CDEF' (63, sysHeap, purgeable);
	IncSys "PopupCDEFMDEF.a.rsrc" codeAs 'proc' (-8224, sysHeap);
	
//	Is anyone using the PICT triangle?  The EditionMgr uses it for its OptionsDialog
	IncSys "PopupTriangle.r.rsrc" 'PICT' (-8224) as 'PICT' (-8224, sysHeap, purgeable);

//__________________________________________________________________________________________________
// other resources to make the file more reasonable

#if wantSounds
	#Include $$Shell("Misc")"SystemSounds.r";			// sounds
#endif

//__________________________________________________________________________________________________
// Apple Event Mgr

#if hasAppleEventMgr
	IncSys "AppleEventMgr.rsrc";
#endif

//__________________________________________________________________________________________________
// Manual-eject drive stuff
// This dialog and alert are used by the manual-eject drive software to communicate with the user
// when a dirty disk has been manually-ejected.  This may not be the best place for them, but they
// need to go somewhere for now.

#if hasManEject
resource 'DLOG' (-16413, sysheap, purgeable) {
	{73, 114, 159, 411},
	dBoxProc,
	visible,
	noGoAway,
	0x0,
	-16413,
	""
};

resource 'DITL' (-16414, sysheap) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{60, 229, 80, 287},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{9, 72, 54, 287},
		StaticText {
			disabled,
			"Failing to update your disk may result i"
			"n data loss."
		}
	}
};

resource 'DITL' (-16413, sysheap, purgeable) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{60, 229, 80, 287},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{9, 72, 54, 287},
		StaticText {
			disabled,
			"You may now safely remove the disk."
		},
		/* [3] */
		{9, 10, 41, 42},
		Icon {
			disabled,
			1
		}
	}
};

resource 'ALRT' (-16414, sysheap, purgeable) {
	{73, 114, 159, 411},
	-16414,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	}
};
#endif