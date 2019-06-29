/*
	File:		SplineError.h

	Contains:	TrueType error states

	Written by: Charlton E. Lui

	Copyright:	© 1989-1990 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 10/9/90	CL		Adding PRE_FLIGHT_ERR.
		 <8>	 9/16/90	CL		Added INIT_HEAPREC_ERR, GROW_HEAPREC_ERR
		 <7>	 8/29/90	CL		Added GROWZONE_HANDLE_ERR, NEWZONE_HANDLE_ERR, NEWCACHELIST_HANDLE_ERR,
									GROWCACHELIST_HANDLE_ERR.
		 <6>	 5/29/90	CL		Added NO_CACHE_ERR.
		 <5>	  5/3/90	RB		Adding in a resource error.
		 <4>	 4/11/90	CL		Took out old EASE headers…
		 <3>	 3/20/90	CL		Adding Disk Cache error codes…
		 <2>	 2/22/90	CL		Added new error codes for entry error and no_bits flag
	   <1.4>	 9/15/89	CEL		Added NIL_FONT_ERR flag.
	   <1.3>	 5/26/89	CEL		Integrated the new Font Scaler 1.0 into Spline Fonts
	   <1.2>	  5/3/89	CEL		Comment brackets were previously messed up from override
	  <•1.1>	  5/3/89	CEL		All kinks worked out, Blasting in bass files…
	   <1.0>	  5/2/89	CEL		Rolling in Bass for first time…

	To Do:
*/

/************/
/** ERRORS **/
/************/
#define NO_ERR						0x0000
#define NULL_KEY					0x0000


#define MISSING_CONTOURS			0x0009


/** EXTERNAL INTERFACE PACKAGE **/
#define KEY_ERR						0x0010

/** FONT EXECUTION **/
#define POINT_MIGRATION_ERR			0x0060
#define INTERPRETER_ERROR			0x0061
#define UNDEFINED_INSTRUCTION_ERR	0x0062
#define TRASHED_MEM_ERR				0x0063

/** Other Execution **/	
#define BAD_CHAR_ERR				0x0070
#define MEM_ERR 					0x0071
#define GROWZONE_HANDLE_ERR			0x0072
#define NEWZONE_HANDLE_ERR			0x0073
#define NEWCACHELIST_HANDLE_ERR		0x0073
#define GROW_HANDLE_ERR				0x0074
#define INIT_HEAPREC_ERR			0x0075
#define GROW_HEAPREC_ERR			0x0076
#define APP_HEAP_FULL				0x0077

/** SFNT DATA ERROR **/
#define SFNT_DATA_ERR				0x0090
#define POINTS_DATA_ERR				0x0091
#define INSTRUCTION_SIZE_ERR 		0x0092
#define CONTOUR_DATA_ERR			0x0093
#define SKIP_CHAR					0x0094
#define NIL_FONT_ERR				0x0096

/** BUFFER Execution **/
#define REHASH_ERR					0x00A0
#define CLIP_ERR					0x00A1
#define CLIPPED_OUT					0x00A2
#define NO_BITS						0x00A3


/** Resource errors **/
#define RESOURCE_ERR				0x00B0

/** CACHE errors **/
#define NO_CACHE_ERROR				0x00C0

/** PreFlight Execution **/
#define PRE_FLIGHT_ERR				0x00D0

/** Entry Offset errors **/
#define ENTRY_ERROR					0xFFFFFFFF
#define DISK_CACHE_ERROR			0xFFFF0001
/************/

extern foo();

/****************************************/
/************ For Debugging *************/
/****************************************/
#define DEBUG_ON
pascal 	Debug()						/* User break drop into Macsbug */
#ifdef	DEBUG_ON
extern	0xA9FF;
#else
{
	;
}	
#endif

#ifdef	LEAVEOUT
pascal 	void DebugStr( aString) char *aString; extern 0xABFF;
char 	*c2pstr();
#define BugInfo( aString) DebugStr( c2pstr(aString))
#endif
/****************************************/

