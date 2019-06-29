/*------------------------------------------------------------------------------
#	SIOW.h
#	Simple Inpue/Output Window header
#	Apple Copyright 1989-1990
#
#	The names for these constants have been selected to satisfy both the
#	standards set up by Apple for applications, and by ANSI for C programming.
#
#-----------------------------------------------------------------------------*/

#ifndef __SIOW__
#define __SIOW__

#ifndef __kPrefSize
#define __kPrefSize			122		/* Default max app size */
#endif
#ifndef __kMinSize
#define __kMinSize			38		/* Default min app size */
#endif
	
/* The following constants are used to identify menus and their items. */
/* NOTE: resources IDs 20000-20010 are reserved fur use by SIOW */

#define	__mApple			20000	/* Apple menu */
#define	__mFile				20001	/* File menu */
#define	__mEdit				20002	/* Edit menu */
#define __mFont				20003	/* Font menu */
#define __mSize				20004	/* Font menu */

#define	__rMenuBar			20000	/* application's menu bar */
#define	__rAboutAlert		20000	/* about alert */
#define	__rUserAlert		20001	/* user error alert */
#define __rSaveAlert		20002	/* save changes alert */
#define	__rDocWindow		20000	/* application's window */
#define	__rVScroll			20000	/* vertical scrollbar control */
#define	__rHScroll			20001	/* horizontal scrollbar control */
#define	__kErrStrings		20000	/* error string list */

#endif

