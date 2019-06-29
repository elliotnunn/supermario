/*
	File:		aliasDlogPriv.h

	Contains:	Private definitions for Alias Manager Dialog routines

	Written by:	Prashant Patel

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 7/19/90	PP		Remove Already Connected to Server dialog related constants.
		 <3>	 6/15/90	PP		Remove "SelectAlias" related constants.
		 <2>	 3/20/90	PP		Add support for "Already Connected to server" dialog.
	   <1.9>	11/28/89	prp		SwitchDisk dialog resources # is -16412 instead of -16396 due to
									a conflict.
	   <1.8>	11/27/89	prp		Add SwitchDisk dialog for Ejectable floppies.
	   <1.7>	11/16/89	prp		Cancel dialog is with 'Stop Search' button instead of
									cmd-period.
	   <1.6>	10/17/89	prp		NewSFPGetFile is now released. Removed it from this file.
	   <1.5>	10/14/89	ngk		Commented out NewStandardFile stuff, it is now in Packaages.h
	   <1.4>	10/13/89	prp		Add constants for Folder Selection dialog.
	   <1.3>	 10/2/89	prp		LongsHandle moved to aliasPriv.h.
	   <1.2>	 9/18/89	prp		Some constants are added.
	   <1.1>	  9/6/89	prp		Changes from CodeReview.
	   <1.0>	  8/7/89	prp		Initial Creation

	To Do:
*/

#ifndef REZ

// Switch Disk dialog item numbers
#define	 kI_CancelSwitchDisk 4		// cancel out of switch disk dialog
#define  kI_MountedEjectable 998	// fake item returned when correct volume mounted by name
#define  kI_BadEjectable 	997		// fake item returned when disk-inserted volume mount unsuccessful

#endif REZ

/* make only the following consts avaiable to resource files that include this file */
#define  kAlSwitchDiskID	-16412   		/* Switch disk dialog box */

