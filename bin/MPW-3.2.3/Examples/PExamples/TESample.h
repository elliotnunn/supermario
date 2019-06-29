/*------------------------------------------------------------------------------
#
#	Apple Macintosh Developer Technical Support
#
#	MultiFinder-Aware TextEdit Sample Application
#
#	TESample
#
#	TESample.h	-	Rez and C Include Source
#
#	Copyright © Apple Computer, Inc. 1989-1990
#	All rights reserved.
#
#	Versions:	
#				1.00				08/88
#				1.01				11/88
#				1.02				04/89	MPW 3.1
#				1.03				02/90	MPW 3.2
#
#	Components:
#				TESample.p			Feb.  1, 1990
#				TESampleGlue.a		Feb.  1, 1990
#				TESample.r			Feb.  1, 1990
#				TESample.h			Feb.  1, 1990
#				TESample.make		Feb.  1, 1990
#
#	TESample is an example application that demonstrates how 
#	to initialize the commonly used toolbox managers, operate 
#	successfully under MultiFinder, handle desk accessories and 
#	create, grow, and zoom windows. The fundamental TextEdit 
#	toolbox calls and TextEdit autoscroll are demonstrated. It 
#	also shows how to create and maintain scrollbar controls.
#
#	It does not by any means demonstrate all the techniques you 
#	need for a large application. In particular, Sample does not 
#	cover exception handling, multiple windows/documents, 
#	sophisticated memory management, printing, or undo. All of 
#	these are vital parts of a normal full-sized application.
#
#	This application is an example of the form of a Macintosh 
#	application; it is NOT a template. It is NOT intended to be 
#	used as a foundation for the next world-class, best-selling, 
#	600K application. A stick figure drawing of the human body may 
#	be a good example of the form for a painting, but that does not 
#	mean it should be used as the basis for the next Mona Lisa.
#
#	We recommend that you review this program or Sample before 
#	beginning a new application. Sample is a simple app. which doesn’t 
#	use TextEdit or the Control Manager.
#
------------------------------------------------------------------------------*/

/*	These #defines correspond to values in the Pascal source code.
	TESample.c and TESample.r include this file. */

/*	Determining an application's minimum size to request from MultiFinder depends
	on many things, each of which can be unique to an application's function,
	the anticipated environment, the developer's attitude of what constitutes
	reasonable functionality and performance, etc. Here is a list of some things to
	consider when determining the minimum size (and preferred size) for your
	application. The list is pretty much in order of importance, but by no means
	complete.
	
	1.	What is the minimum size needed to give almost 100 percent assurance
		that the application won't crash because it ran out of memory? This
		includes not only things that you do have direct control over such as
		checking for NIL handles and pointers, but also things that some
		feel are not so much under their control such as QuickDraw and the
		Segment Loader.
		
	2.	What kind of performance can a user expect from the application when
		it is running in the minimum memory configuration? Performance includes
		not only speed in handling data, but also things like how many documents
		can be opened, etc.
		
	3.	What are the typical sizes of scraps is [a boy dog] that a user might
		wish to work with when lauching or switching to your application? If
		the amount of memory is too small, the scrap may get lost [will have
		to be shot]. This can be quite frustrating to the user.
		
	4.	The previous items have concentrated on topics that tend to cause an
		increase in the minimum size to request from MultiFinder. On the flip
		side, however, should be the consideration of what environments the
		application may be running in. There may be a high probability that
		many users with relatively small memory configurations will want to
		avail themselves of your application. Or, many users might want to use it
		while several other, possibly related/complementary applications are
		running. If that is the case, it would be helpful to have a fairly
		small minimum size.
	
	What we did for TESample:
	
		We determined the smallest heap size that TESample could have and still
		run (22K). For the preferred size we added enough space to permit:
			a. a maximum size TextEdit text handle (32000 characters)
			b. a maximum usable TextEdit scrap (32000 characters)
			b. a maximum scrap as a result of Copy (32000 characters)
			d. a little performance cushion (see 2, above) (10K)
		Result: 122K for preferred size
		
		For the minimum size we took the 22K and then scaled down our requirements
		for a,b, and c above. We thought that providing 16K more would be lean
		and mean (see 4, above).
		Result: 38K for minimum size
*/
 
#define kPrefSize				122
#define kMinSize				38
	
/* The following constants are used to identify menus and their items. The menu IDs
   have an "m" prefix and the item numbers within each menu have an "i" prefix. */
#define	mApple					128		/* Apple menu */
#define	iAbout					1

#define	mFile					129		/* File menu */
#define	iNew					1
#define	iClose					4
#define	iQuit					12

#define	mEdit					130		/* Edit menu */
#define	iUndo					1
#define	iCut					3
#define	iCopy					4
#define	iPaste					5
#define	iClear					6

/*	1.01 - kTopLeft - This is for positioning the Disk Initialization dialogs. */

#define kDITop					0x0050
#define kDILeft					0x0070

/* 1.01 - changed constants to begin with 'k' for consistency, except for resource IDs */
/*	kTextMargin is the number of pixels we leave blank at the edge of the window. */
#define kTextMargin				2

/* kMaxOpenDocuments is used to determine whether a new document can be opened
   or created. We keep track of the number of open documents, and disable the
   menu items that create a new document when the maximum is reached. If the
   number of documents falls below the maximum, the items are enabled again. */
#define	kMaxOpenDocuments		1
	
/*	kMaxDocWidth is an arbitrary number used to specify the width of the TERec's
	destination rectangle so that word wrap and horizontal scrolling can be
	demonstrated. */
#define	kMaxDocWidth			576
	
/* kMinDocDim is used to limit the minimum dimension of a window when GrowWindow
	is called. */
#define	kMinDocDim				64

/*	kControlInvisible is used to 'turn off' controls (i.e., cause the control not
	to be redrawn as a result of some Control Manager call such as SetCtlValue)
	by being put into the contrlVis field of the record. kControlVisible is used
	the same way to 'turn on' the control. */
#define kControlInvisible		0
#define kControlVisible			0xFF

/*	kScrollbarAdjust and kScrollbarWidth are used in calculating
	values for control positioning and sizing. */
#define kScrollbarWidth			16
#define kScrollbarAdjust		(kScrollbarWidth - 1)

/*	kScrollTweek compensates for off-by-one requirements of the scrollbars
 to have borders coincide with the growbox. */
#define kScrollTweek			2
	
/*	kCrChar is used to match with a carriage return when calculating the
	number of lines in the TextEdit record. kDelChar is used to check for
	delete in keyDowns. */
#define kCrChar					13
#define kDelChar				8
	
/*	kButtonScroll is how many pixels to scroll horizontally when the button part
	of the horizontal scrollbar is pressed. */
#define kButtonScroll			4
	
/*	kMaxTELength is an arbitrary number used to limit the length of text in the TERec
	so that various errors won't occur from too many characters in the text. */
#define	kMaxTELength			32000

/* kSysEnvironsVersion is passed to SysEnvirons to tell it which version of the
   SysEnvRec we understand. */
#define	kSysEnvironsVersion		1

/* kOSEvent is the event number of the suspend/resume and mouse-moved events sent
   by MultiFinder. Once we determine that an event is an OSEvent, we look at the
   high byte of the message sent to determine which kind it is. To differentiate
   suspend and resume events we check the resumeMask bit. */
#define	kOSEvent				app4Evt	/* event used by MultiFinder */
#define	kSuspendResumeMessage	1		/* high byte of suspend/resume event message */
#define	kResumeMask				1		/* bit of message field for resume vs. suspend */
#define	kMouseMovedMessage		0xFA	/* high byte of mouse-moved event message */
#define	kNoEvents				0		/* no events mask */

/* 1.01 - kMinHeap - This is the minimum result from the following
	 equation:
			
			ORD(GetApplLimit) - ORD(ApplicZone)
			
	 for the application to run. It will insure that enough memory will
	 be around for reasonable-sized scraps, FKEYs, etc. to exist with the
	 application, and still give the application some 'breathing room'.
	 To derive this number, we ran under a MultiFinder partition that was
	 our requested minimum size, as given in the 'SIZE' resource. */
	 
#define	kMinHeap				 (29 * 1024)
	
/* 1.01 - kMinSpace - This is the minimum result from PurgeSpace, when called
	 at initialization time, for the application to run. This number acts
	 as a double-check to insure that there really is enough memory for the
	 application to run, including what has been taken up already by
	 pre-loaded resources, the scrap, code, and other sundry memory blocks. */
	 
#define	kMinSpace				(20 * 1024)

/*	kExtremeNeg and kExtremePos are used to set up wide open rectangles and regions. */
#define kExtremeNeg				-32768
#define kExtremePos				(32767 - 1)	/* required to address an old region bug */
	
/* kTESlop provides some extra security when pre-flighting edit commands. */
#define	kTESlop					1024

/* The following are indicies into STR# resources. */
#define	eWrongMachine			1
#define	eSmallSize				2
#define	eNoMemory				3
#define	eNoSpaceCut				4
#define	eNoCut					5
#define	eNoCopy					6
#define	eExceedPaste			7
#define	eNoSpacePaste			8
#define	eNoWindow				9
#define	eExceedChar				10
#define	eNoPaste				11

#define	rMenuBar	128				/* application's menu bar */
#define	rAboutAlert	128				/* about alert */
#define	rUserAlert	129				/* user error alert */
#define	rDocWindow	128				/* application's window */
#define	rVScroll	128				/* vertical scrollbar control */
#define	rHScroll	129				/* horizontal scrollbar control */
#define	kErrStrings	128				/* error string list */
