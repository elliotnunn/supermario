/*
	File:		DatabaseAccessPriv.r

	Contains:	Private resource structures for the Database Access Manager.
				Contains definitions for the following resource types:
				
				'rtt#'		List of result handlers and the ids
							of the 'proc' resources they live in.

	Written by:	Jeff Miller

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 3/13/90	JSM		Update comments to reflect reality.
		 <1>	 2/23/90	JSM		First checked in.

	To Do:
*/

/* 'rtt#' - List of result handlers */
type 'rtt#' {
	/* array of IDs of 'proc' resources containing handlers */
	integer = $$CountOf(ResultHandlerArray);	/* array size */
	wide array ResultHandlerArray {
		integer;								/* id of 'proc' resource */
		
		/* array of DBTypes specifying what handlers are in this 'proc' */
		integer = $$CountOf(TypeArray);			/* array size */
		wide array TypeArray {
			literal longint;					/* DBType of handler */
		};
	};
	
};
