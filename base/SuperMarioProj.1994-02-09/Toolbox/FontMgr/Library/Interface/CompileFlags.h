/* ------------------------------------------------------------------------------------------

	Copyright © 1990-1991 by Apple Computer, Inc., all rights reserved.					

	File:		CompileFlags.h																																				
																							
	Author:		Simon Lee															

	Contains:	All the variables which specify the build environment of the project.																											
																											
	Revisions: (most recent first)														
																											
	ID		Date		Description

	<1>		8 Nov 90	Simon Lee - New today.
				
	To do:	
------------------------------------------------------------------------------------------ */


#ifndef __CompileFlags__
#define __CompileFlags__

#include	<Types.h>

#define	__debugOn__						// turn debugging on.
#define	__system6Compatible__			// maintain 6.x compatible.

#define	doubleTalkFileCreator	'doub'	// creator type of DoubleTalk.

#endif __CompileFlags__