/*
	File:		ModalDialogMenuExtensions.c

	Contains:	C language portions of Modal Dialog Menu Access Facillity

	Written by:	Darin Adler

	Copyright:	© 1990-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		<23>	 11/3/92	DTY		Strip out unnecessary includes.
		<22>	 6/10/92	JSM		Moved this file to ModalDialogMenuExtensions.c from
									ModalDialogMenuPatches.c since it’s used by both the ROM and
									System, keeping all the old revisions.
		<21>	 5/29/92	DCL		Included TextUtils.p. EqualString was moved for the New Inside
									Mac.
		<20>	 3/23/92	JSM		OSEvents.h is obsolete, use Events.h.
		<19>	 2/20/91	KSM		gbm,#KSM-3: Don't check spareflag of a NIL window.
		<18>	 2/10/91	KSM		DBA,#80950: Fix to handle graying of Applications Menu properly.
		<17>	  1/5/91	fjs		(KSM) do not allow non-printing function keys
		<16>	12/18/90	KSM		<rlc> Fix bug where DialogMgr incorrectly sets up the EditField
									of the dialog record causing the Edit Menu to be incorrectly
									enabled.
		<15>	 12/3/90	RLC		<ksm> Change calls to IsWindowModal() to call the newer
									GetWindowModalClass instead.
		<14>	11/26/90	KSM		(dba+dh)Remove extraneous ifdefs around includes.  Also only
									analyze in foregnd.
		<13>	11/14/90	JL		Changing InvalidMenuBar back to InvalMenuBar because Invalid is
									misleading. All Inval… calls should be expanded to Invalidate if
									they are expanded at all.
		<12>	11/13/90	KSM		<dba>Fix bug where we checked spareFlag directly instead of
									calling IsWindowModal. IsWindowModal handles nil correctly, and
									is smarter about variation 1 of WDEF 0. Change
									SetSaveMenuEnableState to save the menu state, even if we are
									only setting up the system menus and not disabling the
									application menus. This fixes a bug where the system menus get
									permanently disabled.
		<11>	 11/6/90	KSM		<rlc>Revisit role of SetSaveMenuEnableState and ActiveWindowNeedsHelp
									to handle the additional case of apps using modal window kinds, but
									never calling _ModalDialog.
		<10>	10/29/90	KSM		<dba>Remove some dead code.  Update to look for the modal dialog bit
									before handling menus.  Enable edit menu iff edit text is active.
		 <9>	10/15/90	JSM		Really change InvalMenuBar to InvalidMenuBar.
		 <8>	10/15/90	JL		Changing InvalMenuBar to InvalidMenuBar.
		 <7>	 7/31/90	KSM		Fix edit menu analysis state handling.
		 <6>	 7/13/90	RLC		Fix bug in testing menuselect result, hiword will be zero, but
									entire longword might not be.
		 <5>	 7/13/90	KSM		Fix edit menu enabling check. Remove extra InvalMenuBar call in
									EnableTheEditMenu.
		 <4>	 7/10/90	dba		get rid of C warnings
		 <3>	  7/2/90	KSM		Add edit menu handling.
		 <2>	  6/8/90	KSM		Update to use new IsFrontWindow modal calls.
		 <1>	  6/1/90	KSM		First checked in.

	To Do:
		add a MENU resource to indicate names of Edit menu items
		find items from the Edit menu to enable them
		use knowledge about Dialog Manager to know if Edit menu items should be enabled
			note that we should *only* enable edit menus if the frontmost window is a
			windowKind == dialogKind and there is an appropriate active editText field
			(a visible TextEdit record that is currently active)
		enable help menu
		pass Cut, Copy, Paste, Clear, Select All through dialog filter as command keys
		make a separate patch to Dialog Manager to make it handle command keys
		use a global flag to prevent from saving the menu state twice
		??? save menu state in a global instead of returning it as a function result
		find all the *** and get rid of them
*/

/*
	Modal Dialog Menu Access
	Theory of Operation (Volume 1, Chapter 1, section 1)

	The state of the world is “normal.”
	STATE TRANSITION TABLE

	State	Event		Event		Event		MenuSelect	FlushEvents		SysBeep
			=MouseDn	=NullEvt	=Other		(from App)	(from anyone)	(from App)
	-----	---------	---------	---------	-----------	-------------	-------------
		0		1			0			0			0			0				0
		1		0			3			0			0			0				2
		2		0			3			0			0			0				2
		3		0			0			0			0			0				0

	What this means is:
	State 0: This is the normal state of this code.  If we see a mousedown in the menubar, we might
	want to handle the menuselect call, but the app might already be doing this.  So we save this
	event and move to state 1.

	State 1: We have seen a mousedown, but we are waiting to see if the application
	is handling menus during modal dialogs.  If we see the app call menuselect, we know it is
	handling menus and return to state 0.  If we see a flushevents call, we always go to state 0
	and forget any event information we saved coming to this state.  If we see a SysBeep, it might
	be that that ModalDialog filter proc is complaining we clicked out of the dialog, or we may just
	have gotten an extra beep. The SysBeep is "saved" and we go to state 2.  A null event assumes
	that the app had the change to handle the menus, but did not - so we assume it won't, go to
	state 3.

	State 2: We saw a sysbeep.  If we see a null event, we have "eaten" the sysbeep and go to state
	3.  If we see another SysBeep, we Beep (for the one that got us to this state) and save off the
	new SysBeep and stay in this state.

	State 3: We've done our normal code and go back to state 0.  This is implemented in the opposite
	order, in that, we set the state to zero as we proceed on into the regular code.

	------------------------------------------
	In general, new patches do the following:

	FlushEvents:	go to state zero
	MenuSelect:		go to state zero (if the app called it)
	SysBeep:		if state 1, don't beep and go to state 2
					if state 2, beep and stay in state 2
	FilterEvent		if state 0, mousedown goes to state 1
					if state 1, null event goes to state 3, otherwise state 0
					if state 2, null event goes to state 3, otherwise state 0
					if state 3, we'll be on our way to state 0 anyway.


*/

#include <Dialogs.h>
#include <Events.h>
#include <Memory.h>
#include <Menus.h>
#include <Processes.h>
#include <TextUtils.h>
#include <Windows.h>

#include <DialogsPriv.h>
#include <MenuMgrPriv.h>

#define kFunctionKey	0x10
#define kCut			0x78
#define kCopy			0x63
#define kPaste			0x76

// Menu Mgr. data structures

typedef struct
	{
	short		lastMenu;
	short		lastRight;
	short		reserved;
	}
MenuListHeader;

typedef struct
	{
	MenuHandle	handle;
	short		left;
	}
MenuListEntry;


// menu bar iteration

MenuListHeader** GetMenuList(void);
MenuListEntry* GetNextMenu(short* offset);


// other menu operations

Boolean MenuBarEmpty(void);
Boolean PtInMenuBar(Point);
Boolean IsThisASystemMenu(short menuID);
Boolean IsModal(WindowPtr);

MenuHandle FindMenuByTitle(const unsigned char* title);
MenuHandle FindAppleMenu(void);
long GetMenuFlags(MenuHandle);

void DisableAppMenus(void);
void EnableTheEditMenu(void);


// routines to save enable state of all menus in the menu bar

Handle SaveMenusEnableState(void);


// globals (implemented in assembly) to keep our state

enum
	{
	kApplicationHandlesMenus,
	kHandleSystemMenusDisabled,
	kHandleMenusEditMenuDisabled,
	kHandleMenusEditMenuEnabled,
	};
typedef short TAnalyzedWindowState;

typedef struct
	{
	MenuHandle	handle;
	long		savedEnableFlags;
	}
MenuStateEntry;

pascal WindowPtr GetAnalyzedWindow(void);
pascal void SetAnalyzedWindow(WindowPtr);

pascal TAnalyzedWindowState GetAnalyzedWindowState(void);
pascal void SetAnalyzedWindowState(TAnalyzedWindowState);

pascal Handle GetSavedMenuState(void);
pascal void SetSavedMenuState(Handle);


// call to enable all menus for us

pascal void ModalDialogMenuSetup(Boolean nowModal)
	= {0xAA67};


// heuristics used by the patches

pascal TAnalyzedWindowState ActiveWindowNeedsHelp(void);


// routines called by assembly language

pascal Handle SetSaveMenusEnableState(TAnalyzedWindowState state); 	// save state and enable selected menus
pascal void RestoreMenusEnableState(Handle state);
pascal void FilterEvent(EventRecord*);


// implementations

MenuListHeader** GetMenuList(void)
	{
	long result = (* (long*) 0xA1C);
	if (result & 1)
		return 0;
	else
		return (MenuListHeader**) result;
	}

MenuListEntry* GetNextMenu(short* offset)
	{
	MenuListHeader** list = GetMenuList();

	if (list == 0)
		return 0;
	*offset += sizeof(MenuListEntry);
	if (*offset > (**list).lastMenu)
		return 0;
	else
		return (MenuListEntry*) (((char*) *list) + *offset);
	}

Boolean MenuBarEmpty(void)
	{
	short offset = 0;
	return GetNextMenu(&offset) == 0;	// no next menu means no menus in bar
	}


Handle SaveMenusEnableState(void)
	{
	short offset;
	MenuListEntry* mlEntry;
	Handle handle;
	offset = 0;

	// First check to see if we already had saved the state (reentrant check)
	if (GetSavedMenuState())
		return 0;

	// make a handle for saved enable states

	handle = NewHandle(0);
	if (handle == 0)
		return 0;

	// traverse the menus, saving state

	while (mlEntry = GetNextMenu(&offset))
		{
		MenuStateEntry saved;
		MenuHandle menu;

		// make the saved state, and append it to the handle

		menu = mlEntry->handle;
		saved.handle = menu;
		saved.savedEnableFlags = (**menu).enableFlags;
		if (PtrAndHand((Ptr) &saved, handle, sizeof(MenuStateEntry)) != noErr)
			{
			DisposHandle(handle);
			return 0;
			}
		}

	// Be sure to save the state in our global
	SetSavedMenuState(handle);

	return handle;
	}

pascal void RestoreMenusEnableState(Handle state)
	{
	short offset = 0;
	MenuListEntry* mlEntry;

	// traverse the menus, restoring all the enable flags

	while (mlEntry = GetNextMenu(&offset))
		{
		MenuHandle handle = mlEntry->handle;
		MenuStateEntry* stateEntry = (MenuStateEntry*) *state;
		MenuStateEntry* afterLast = (MenuStateEntry*) (((char*) stateEntry) + GetHandleSize(state));

		// search through the state handle for saved state for this menu

		while (stateEntry < afterLast)
			{
			if (stateEntry->handle == handle)
				{
				// found it, jam it in enable flags and continue (unless system menu)

				if (!IsThisASystemMenu((**handle).menuID))
					(**handle).enableFlags = stateEntry->savedEnableFlags;
				break;
				}
			stateEntry++;
			}
		}

	DisposHandle(state);

	// Be sure to clean up our global
	SetSavedMenuState(0);
	ModalDialogMenuSetup(false);		// Tell system menus to return to normal state
	}


void DisableAppMenus(void)
	{
	short offset = 0;
	MenuListEntry* mlEntry;

	// traverse the menus, setting all the enable flags to zero (unless system menu)
	while (mlEntry = GetNextMenu(&offset))
		{
		if (!IsThisASystemMenu((**(mlEntry->handle)).menuID))
			(**(mlEntry->handle)).enableFlags = 0;
		}
	}


Boolean ThereIsAnActiveEditTextFieldInThisWindow(WindowPtr active)
	{
	if (((WindowPeek)active)->windowKind == dialogKind)
		{
		// For some reason, NewDialog does not set up the editField in the DialogRecord.
		// The next DialogMgr call does though, hence this peculiar call to GetDItem.
		// Additionally, call SelIText on a stattext field points editField to that
		// statText field, which belies the name editField.
		short kind;
		Handle h;
		Rect r;
		register short editField = ((DialogPeek)active)->editField;		// Get the edit field locally
		if (editField < 0) editField = 0;								// Force into range
		GetDItem(active, ++editField, &kind, &h, &r);					// Inc. editField before use
		// Check for editField erroneously pointing to a statText field (MPW 3.2 case)
		if ((editField < 0) || ((kind & 0x7f) == editText))
			if (((DialogPeek)active)->editField >= 0)
				if ((**(((DialogPeek)active)->textH)).active != 0)
					return true;
		}
	return false;
	}

pascal Handle SetSaveMenusEnableState(TAnalyzedWindowState state)
	{
	/*
		3 cases:	regular window (do nothing)
					modal window w/o _ModalDialog call (setup system menus)
					_ModalDialog called (setup all menus)
	*/
	Handle result = nil;

	if (state != kApplicationHandlesMenus)
		{
		// These 2 calls need to be in the opposite order in restore
		ModalDialogMenuSetup(true);		// System menus always need to be set up	<18>
		result = SaveMenusEnableState();
		if (result)
			{
			// we have some kind of modal situation; check if we should setup all menus
			if (state != kHandleSystemMenusDisabled)
				{
				// kHandleMenusEditMenuDisabled or kHandleMenusEditMenuEnabled: setup all the menus
				DisableAppMenus();
				if (state == kHandleMenusEditMenuEnabled)
					EnableTheEditMenu();
				}
			}
		}

	return result;
	}

void EnableTheEditMenu()
	{
	MenuListEntry* mlEntry;
	short offset = 0;

	while (mlEntry = GetNextMenu(&offset))
		{
		// Walk across the menus and look for Cut, Copy, and Paste (and maybe Undo) ...
		MenuHandle menu = mlEntry->handle;
		short items = CountMItems(menu);
		short i = (short) ((items - 2) + 1);		// Skip the last 2 items, since we are looking for a cluster of 3
		if (i > (3 + 1)) i = (3 + 1);				// Look at the first 3 items of each menu at most
		while (--i > 0)								// Start at the bottom of the menu, since cmd-X is usually item 3
			{
			short cmdChar;
			GetItemCmd(menu, i, &cmdChar);
			if (cmdChar == 'X')
				{
				long flagsToSet = (7 << i) + 1;		// Compute flagsToSet now before incrementing i
				GetItemCmd(menu, ++i, &cmdChar);	// Check the next item (should be Copy)
				if (cmdChar == 'C')					// We found cmd-C, find Paste
					{
					GetItemCmd(menu, ++i, &cmdChar);
					if (cmdChar == 'V')				// We found cmd-V! This is Edit menu
						(**menu).enableFlags |= flagsToSet;		// So, set the flags
					}
				 return;							// Once we find cmd-X, don’t look any more
				}
			}
		}
	}

MenuHandle FindMenuByTitle(const unsigned char* title)
	{
	short offset = 0;
	MenuListEntry* mlEntry;

	// traverse the menus, searching for the proper menu item

	while (mlEntry = GetNextMenu(&offset))
		{
		MenuHandle menu = mlEntry->handle;
		if (EqualString((**menu).menuData, title, false, false))
			return menu;
		}
	return 0;
	}

pascal TAnalyzedWindowState ActiveWindowNeedsHelp(void)
	{
	WindowPtr active;
	TAnalyzedWindowState oldState;
	TAnalyzedWindowState newState;
	ProcessSerialNumber ourPSN;
	ProcessSerialNumber currentPSN;
	Boolean inFront;

	if (MenuBarEmpty())		// short-circuit when there are no menus
		return false;

	// short circuit when current application is not frontmost since its menubar is not visible
	inFront = false;
	(void) GetFrontProcess(&ourPSN);
	currentPSN.highLongOfPSN = 0;
	currentPSN.lowLongOfPSN = kCurrentProcess;
	(void) SameProcess(&currentPSN, &ourPSN, &inFront);
	if (inFront == false)
		return false;

	active = FrontWindow();
	oldState = GetAnalyzedWindowState();

	if (active != GetAnalyzedWindow())
		{
		Boolean	calledModalDialog;

		// latch onto the active window

		SetAnalyzedWindow(active);
		SetAnalyzedWindowState(kApplicationHandlesMenus);
		if (active)
			{

			// <18> <KSM The rest of the code in this block is change>
			// Any window that is a modal dialog (WDEF 0 and variant=dBoxProc) window,
			// or was the the active window at the time _ModalDialog was called
			// requires us to disallow switching out (i.e., we must disable the Process menu).
			// IsModal() OR _ModalDialog implies set up System Menus for a modal dialog
			// But ONLY if _ModalDialog was called for this window should we check to see
			// if we should handle the menus on the application's behalf.
	
			// Find out if we called ModalDialog on this window
			calledModalDialog = ((((WindowPeek)active)->spareFlag & systemHandlesMenusMask) != 0);
	
			// Should we disable System Menus (either case)?
			if (IsModal(active) || calledModalDialog) 
				SetAnalyzedWindowState(kHandleSystemMenusDisabled);	// disable System Menus
	
			// Should we handle menus for the application (only if ModalDialog was called)?
			if (calledModalDialog)
				{
				// if application handles menus, Apple menu
				// (or at least About item) will be disabled
	
				MenuHandle appleMenu = FindAppleMenu();
				long appleMenuFlags = GetMenuFlags(appleMenu);
				if (appleMenu && (appleMenuFlags & 1) && (appleMenuFlags & 2))
					{
					// the application doesn’t handle menus, we will
					if (ThereIsAnActiveEditTextFieldInThisWindow(active))
						SetAnalyzedWindowState(kHandleMenusEditMenuEnabled);
					else
						SetAnalyzedWindowState(kHandleMenusEditMenuDisabled);
					// since we allow choices from the menu bar, unhilite the menu to make that obvious
					HiliteMenu(0);
					}
				}
			}
		}

	newState = GetAnalyzedWindowState();

	// if the state has changed, menu titles may have dimmed/undimmed so redraw them

	if (oldState != newState)
		InvalMenuBar();

	// return true if we are handling menus

	return newState;
	}

pascal void FilterEvent(EventRecord* event)
	{
	if ((event->what == nullEvent) || (event->what == updateEvt))
		(void) ActiveWindowNeedsHelp();
	else if (event->what == mouseDown)
		{
		if (PtInMenuBar(event->where))
			{
			TAnalyzedWindowState state = ActiveWindowNeedsHelp();
			if ((state == kHandleMenusEditMenuEnabled) || (state == kHandleMenusEditMenuDisabled))
				{
				long result;
				register short menuID;
				event->what = nullEvent;

				// track the menus

				result = MenuSelect(event->where);
				if (menuID = (short) (result >> 16))
					{
					short cmdChar;
					GetItemCmd(GetMHandle(menuID), (short) result, &cmdChar);
					// following is a compatibility hack for Word which does not respect enableFlags
 					if (cmdChar != 0)
						{
						EvQElPtr postedEvent;
						if (PPostEvent(keyDown, cmdChar, &postedEvent) != noErr)
							// if the event was not posted, unhilite the menu now
							HiliteMenu(0);
						else
							// otherwise, wait for MenuKey to unhilite it
							postedEvent->evtQModifiers = cmdKey;
						}
					}
				}
			}
		}
	else if (event->what == keyDown)
		{
		if ((event->modifiers & cmdKey) || ((char) event->message == kFunctionKey))
			{
			TAnalyzedWindowState state = ActiveWindowNeedsHelp();
			if ((state == kHandleMenusEditMenuEnabled) || (state == kHandleMenusEditMenuDisabled))
				{
				long result;
				unsigned char key = (char) event->message;

				if (key == kFunctionKey)
					{
					unsigned char virtualKey = (event->message & keyCodeMask) >> 8;
					
					switch (virtualKey)
						{
						case kCut:
							key = 'X';
							break;
							
						case kCopy:
							key = 'C';
							break;
							
						case kPaste:
							key = 'V';
							break;
							
						default:
							key = 0;
							break;
						}
					}
				
				// just hilite the menus when the relevant key is pressed
				
				if (key && (result = MenuKey(key)))
					{
					long dummy;
					Delay(8, &dummy);
					}
				HiliteMenu(0);
				}
			}
		}

	if (event->what != nullEvent)
		SetAnalyzedWindow((WindowPtr) 1);	// set to something that is never a window
	}

long GetMenuFlags(MenuHandle menu)
	{
	if (menu)
		{
		Handle savedFlags = GetSavedMenuState();
		if (savedFlags)
			{
			MenuStateEntry* stateEntry = (MenuStateEntry*) *savedFlags;
			MenuStateEntry* afterLast = (MenuStateEntry*) (((char*) stateEntry) + GetHandleSize(savedFlags));
	
			// search through the state handle for saved state for this menu
	
			while (stateEntry < afterLast)
				{
				if (stateEntry->handle == menu)
					// found it, return saved enable flags
					return stateEntry->savedEnableFlags;
				++stateEntry;
				}
			}
		return (**menu).enableFlags; // didn’t find a saved state, so return the flags from the menu
		}
	return 0; // didn’t find a menu, so return nothing enabled
	}

MenuHandle FindAppleMenu(void)
	{
	// *** use MENU resource
	return FindMenuByTitle("\p\024");
	}

Boolean PtInMenuBar(Point pt)
	{
	Rect mbRect;
	(void) GetMBARRect(&mbRect);
	return PtInRect(pt, &mbRect);
	}

Boolean IsThisASystemMenu(short menuID)
	{
	Boolean	isSys;
	if (IsSystemMenu(menuID, &isSys) != noErr)
		return 0;
	return isSys;
	}

Boolean IsModal(WindowPtr active)
	{
	short class;
	if (GetWindowModalClass(active,&class) != noErr)
		return false;
	return (class == dBoxProc);
	}
