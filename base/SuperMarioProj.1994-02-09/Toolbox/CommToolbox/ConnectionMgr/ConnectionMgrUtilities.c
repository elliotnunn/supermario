/*
	File:		ConnectionMgrUtilities.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 3/14/90	BBH		first checked in

	To Do:
*/

/*
 *	CDEFExecuteDef takes the field defProc and jumps to it.  It calls ExecuteDef.
 * 	It wraps calls to ExecuteDef with calls to CRMToolPreflight and CRMToolPostflight.
 * 	These routines change the resource map so that curMap points to the system file
 * 	and the appropriate tool is the first resource file below the system file.  Also
 *	the CommToolbox is placed below the tool.  CRMToolPostflight undoes this.
 *
 •	Should we convert this routines to assembler? (BBH 4/6/89)
 */
pascal long CDEFExecuteDef( hConn, defProc, msg, p1, p2, p3 )
long hConn, defProc, p1, p2, p3;
short msg; 
{
	long	rval;
	CRMToolContext	context;							/* saved resource chain context */
	pascal long		(*callProc) (long, short, long, long, long);
	
	CRMToolPreflight((**(ConnHandle)hConn).procID, &context);			/* preflight the tool call */
	
	(long) callProc = defProc;
	rval = (*callProc) (hConn, msg, p1, p2, p3);		/* and away she goes			*/
	CRMToolPostflight(&context);						/* restore the environment		*/
	return(rval);										/* and send back the bodies		*/
}
