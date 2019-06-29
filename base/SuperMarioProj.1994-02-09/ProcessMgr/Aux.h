/*
    C Interface to the Macintosh Libraries
    Copyright Apple Computer,Inc. 1987-9
    All rights reserved.
*/

/*
    These constants define selectors for the AUXDispatch call.  This call
should be used only after using Gestalt to determine that the application
is running under A/UX.
*/

#ifndef unix
pascal long	AUXDispatch(short selector, char *p)
	= {0xABF9};

typedef pascal long
(*AUXDispatch_ptr)(short selector, char *p);

#endif unix

#define AUX_HIGHEST          0  /* return highest available selector , p is not used */
#define AUX_GET_ERRNO        1  /* get pointer to errno , p points to (int *) */
#define AUX_GET_PRINTF       2  /* get pointer to printf() , p points to ((*int) ()) */
#define AUX_GET_SIGNAL       3  /* get pointer to signal() , p points to ((*int) ()) */
#define AUX_GET_TIMEOUT      4  /* return nice timeout value in clock ticks , p is not used */
#define AUX_SET_SELRECT      5  /* set a mouse movement rectangle to use in */
								/*  future select() calls.  p points to a Rect */
#define AUX_CHECK_KIDS       6  /* check to see if a given process has kids p points */
								/*  to an integer containing the process id */
#define AUX_POST_MODIFIED    7  /* post an event, with modifiers */

#define AUX_FIND_EVENT       8  /* search event queue for an event */

#define AUX_GET_UDEVFD       9  /* get pointer to udevfd */
								/* p points to (int *) */
#define AUX_GET_CDEVFD      10  /* get pointer to cdevfd */
								/* p points to (int *) */
#define AUX_GET_ENVIRON     11  /* get pointer to environ */
 								/* p points to (char ***) */
#define AUX_LOGOUT          12  /* call routine to do whatever it takes to logout */
#define AUX_SWITCH          13  /* process context switch */
#define AUX_GETTASK         14  /* get a/ux task descriptor */
#define AUX_GETANYEVENT     15  /* get any event from the event queue */
#define AUX_SETAUXMASK      16  /* a/ux events should be returned by GetOSEvent and */
								/* OSEventAvail, p is an int */
#define AUX_STOPTIMER       17  /* turn off the vbl and time mgr interrupt */
#define AUX_STARTTIMER      18  /* turn on the vbl and time mgr interrupt */
#define AUX_SETTIMEOUT      19  /* set the timeout value for GetOSEvent */
#define AUX_SETBOUNDS       20  /* set the mouse moved region for GetOSEvent */
#define AUX_KILL            21  /* kill the specified process, p contains a pid */
#define AUX_FORKEXEC        22  /* fork and exec a new coff binary */
#define AUX_REG_SIGIO       23  /* register a function and mask for SIGIO muxing */
#define AUX_UNREG_SIGIO     24  /* unregister a function for SIGIO muxing */
#define AUX_VFS_VREFNUM     25  /* return the VREFNUM used for "/" */
#define AUX_CLEANFS         26  /* clean fs shutdown for Login */
#define AUX_TRIM_CACHE      27  /* dispatch to compact the File Manager cache */
#define AUX_ID_TO_PATH      28  /* translate a CNID to a *valid* UNIX path */
#define AUX_FS_FREE_SPACE   29  /* get the # of free bytes for a filesystem */
#define AUX_CHECK_TB_LAUNCH 30  /* check if OK to launch the file */
#define AUX_FS_USED_SPACE   31  /* get the # of used bytes for a filesystem */
#define AUX_SAME_FS         32  /* returns true if 2 dirIDs on same FS */
#define AUX_MNT_CD          33  /* try to mount any available CD-ROMs */
#define AUX_RESTART_DLOG    34  /* post restart dialog but don't restart */
#define AUX_SHUTDOWN_DLOG   35  /* post shutdown dialog but don't shutdown */
#define AUX_SECONDARY_INIT  36  /* Perform secondary initializations. */
#define AUX_COFFFSSPEC      37  /* Get fsspec for a coff binary, p is the taskid    */
#define AUX_HOME_DIR        38  /* Get the cnid of the home directory */
#define AUX_GETUID          39  /* Get the effective user id */
#define AUX_POST_EVTREC     40  /* post an entire event record */
#define AUX_FSTYPE          41  /* return the UNIX file sys type for a dir */
#define AUX_MAP_ID          42  /* moral equiv to PBMapID() */
#define AUX_MAP_NAME        43  /* moral equiv to PBMapName() */
#define AUX_SAME_DIRS       44  /* returns TRUE if 2 dirs stat equal */
#define AUX_SYNC_DIR        45  /* Used to inform the File Mgr when to */
                                /* check dirs for open folders. */
#define AUX_BEGINAPPLICATION 46 /* Start up application */
#define AUX_ENABLECOFFLAUNCH 50	/* Enable launch of coff applications? */

#define MAX_SELECTOR    AUX_ENABLECOFFLAUNCH

typedef struct	
{
	EventRecord		event;
	long			timeout;
	RgnHandle		mouseBounds;
	Boolean			pullIt;
	Boolean			found;
	short			mask;
} GetAnyEventRec, *GetAnyEventPtr;

#define	FIRST_AUX_EVENT		16
#define	LAST_AUX_EVENT		18

enum
{
	auxAttachEvent = FIRST_AUX_EVENT,
	auxExitEvent,
	auxSelectEvent
};

typedef struct
{
	short			vrefnum;
	char			*name;
	short			pid;
} ForkExecRec, *ForkExecPtr;

typedef struct {
	struct FSSpec fsspec;
	long taskid;
} CoffName;
