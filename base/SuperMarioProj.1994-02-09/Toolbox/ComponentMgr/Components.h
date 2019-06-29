/*
	File:		Component Manager.h

	Copyright:	© 1990-1991, 1993 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM4>	  8/5/93	JDR		integrate component mgr from Reality
	   <SM3>	 7/21/93	RC		House Cleaning from Reality
		 <4>	 7/20/93	JDR		Added some constants to match the public version of this header
									file.
		 <3>	 6/13/93	JDR		Rolled in the ComponentMgr sources from QuickTime 1.6.
		 <2>	12/12/91	DTY		Take the spaces out of the Component Manager files.

		<14>	 10/1/91	BAL		changed return value type for loadcomponent
		<13>	 10/1/91	BAL		Added prototypes for internal routines.
		<12>	 9/20/91	BAL		OpenDefaultComponent takes OSTypes.
		<11>	 9/17/91	BAL		Added cmpWantsRegisterMessage
		<10>	 8/28/91	BAL		Added OpenDefaultComponent
		 <9>	 7/18/91	dvb		Typo
		 <8>	 7/18/91	dvb		THINC_C == 1
		 <7>	 5/25/91	JB		OLD_THINK_C && TOOLBOX_LINKED
		 <6>	 5/17/91	dvb		Thing->Component
		 <5>	  5/6/91	dvb		Gestalt Selectors
		 <4>	  5/5/91	dvb		Make field names begin lower case!
		 <3>	  5/4/91	GW		Put in forPublicRelease check.
		 <2>	  5/4/91	PH		ComponentError -> ComponentResult
		 <1>	  5/3/91	BAL		first checked in
		<10>	 4/25/91	JB		Changing to new THINK C interface files
		 <9>	 4/24/91	BAL		Make CloseComponentResFile return an OSErr instead of a short.
		 <8>	 4/18/91	BAL		Removed self and when fields from the ComponentParams structure.
									RegisterComponent now takes a ComponentRoutine not a ComponentRoutine*; this
									was merely a typedef problem you still pass it a pointer to the
									routine.
		 <7>	 4/15/91	BAL		Removed the getComponentParentInstance message.
		 <6>	 4/15/91	BAL		Added some enums and renamed the params to CaptureComponent.
		 <5>	  4/7/91	BAL		Renamed ChainComponentCall to DelegateComponentCall as to Jed Harris'
									suggestion.
		 <4>	 3/28/91	dvb		Trivia
		 <3>	 3/28/91	BAL		Do some file management.  Added Load/UnloadComponent.  Added
									Capture/UncaptureComponent.  Added SetDefaultComponent.
		 <2>	 2/16/91	BAL		Added ComponentCallNow() macro
		 <3>	 12/30/90	bal		changed name to Component manager
		 <2>	 10/06/90	dvb		Make it work

	To Do:
*/

#ifdef firstHeaderFile
/************************************************************

Created: ∑$
 Components.h
 C Interface to the Macintosh Libraries


 Copyright Apple Computer, Inc. 1991, 1992
 All rights reserved

************************************************************/

#endif firstHeaderFile
#ifndef __COMPONENTS__
#define __COMPONENTS__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __MEMORY__
#include <Memory.h>
#endif

#ifndef kSmallestArray
	#define kSmallestArray 1
#endif

#define kAppleManufacturer 'appl'		/* Apple supplied components */
#define kComponentResourceType 'thng'	/* a components resource type */

enum {
 kAnyComponentType = 0,
 kAnyComponentSubType = 0,
 kAnyComponentManufacturer = 0,
 kAnyComponentFlagsMask = 0
};

enum {
	cmpWantsRegisterMessage = 1L<<31
};

#ifndef _PrivateThingManager_
typedef struct
	{
	OSType componentType;					/* A unique 4-byte code indentifying the command set */
	OSType componentSubType;				/* Particular flavor of this instance */
	OSType componentManufacturer;			/* Vendor indentification */
	unsigned long componentFlags;			/* 8 each for Component,Type,SubType,Manuf/revision */
	unsigned long componentFlagsMask;		/* Mask for specifying which flags to consider in search, zero during registration */
	} ComponentDescription;
#endif _PrivateThingManager_


typedef struct
	{
	OSType resType;							/* 4-byte code 	*/
	short resID;							/*  			*/
	} ResourceSpec;


struct ComponentResource {
	ComponentDescription cd;				/* Registration parameters */
	ResourceSpec component;					/* resource where Component code is found */
	ResourceSpec componentName;				/* name string resource */
	ResourceSpec componentInfo;				/* info string resource */
	ResourceSpec componentIcon;				/* icon resource */
};
typedef struct ComponentResource ComponentResource;
typedef ComponentResource *ComponentResourcePtr, **ComponentResourceHandle;

struct ComponentPlatformInfo
	{
	long	componentFlags;					/* flags of Component */
	ResourceSpec component;					/* resource where Component code is found */
	short	platformType;					/* gestaltSysArchitecture result */
	};
typedef struct ComponentPlatformInfo ComponentPlatformInfo;

struct ComponentResourceExtension
	{
	long	componentVersion;				/* version of Component */
	long	componentRegisterFlags;			/* flags for registration */
	short	componentIconFamily;			/* resource id of Icon Family */
	};
typedef struct ComponentResourceExtension ComponentResourceExtension;

struct ComponentPlatformInfoArray
	{
	long	count;
	ComponentPlatformInfo platformArray[1];
	};
typedef struct ComponentPlatformInfoArray ComponentPlatformInfoArray;

enum {	// Component Resource Extension flags
	componentDoAutoVersion = (1<<0),
	componentWantsUnregister = (1<<1),
	componentAutoVersionIncludeFlags = (1<<2),
	componentHasMutliplePlatforms = (1<<3)
	};


#ifndef _PrivateThingManager_
	typedef struct privateComponentRecord *Component;
	typedef struct privateComponentInstanceRecord *ComponentInstance;
	typedef struct privateRegisteredComponentRecord *RegisteredComponentPtr;
	typedef struct privateRegisteredComponentInstanceRecord *RegisteredComponentInstancePtr;
#endif _PrivateThingManager_


/*  Structure received by Component:		*/

typedef struct
	{
	unsigned char flags;				/* call modifiers: sync/async, deferred, immed, etc */
	unsigned char paramSize;			/* size in bytes of actual parameters passed to this call */
	short what;							/* routine selector, negative for Component management calls */
	long params[kSmallestArray];		/* actual parameters for the indicated routine */
	} ComponentParameters;


typedef long ComponentResult;

typedef pascal ComponentResult (*ComponentRoutine)
		(ComponentParameters *cp, Handle componentStorage );

typedef pascal ComponentResult (*ComponentFunction)();


#ifndef forPublicRelease
#ifndef tLINKED
	#define ComponentDispatch(name,returnType,selector) \
		pascal returnType name() = {0x7000+selector,0xA82A};

	#include "ComponentManagerSelectors.h"

	#undef ComponentDispatch
#endif tLINKED
#endif forPublicRelease

//∑#ifndef forPublicRelease
#define ComponentCallNow( callNumber, paramSize ) \
		{0x2f3c,paramSize,callNumber,0x7000,0xA82A}

/* the //$$$COMPONENTCALLNOW is so that MakeWarholRelease can figure out where to insert the ComponentCallNow macro */
//∑#endif
//$$$COMPONENTCALLNOW

/********************************************************
* Required Component routines
********************************************************/

#define kComponentOpenSelect	 -1			/* ComponentInstance for this open */
#define kComponentCloseSelect	 -2			/* ComponentInstance for this close */
#define kComponentCanDoSelect	 -3			/* selector # being queried */
#define kComponentVersionSelect	 -4			/* no params */
#define kComponentRegisterSelect -5			/* no params */
#define kComponentTargetSelect 	 -6			/* ComponentInstance for top of call chain */
#define kComponentUnregisterSelect 	-7			/* no params */

enum {				/* Set Default Component flags */
	defaultComponentIdentical		= 0,
	defaultComponentAnyFlags		= 1,
	defaultComponentAnyManufacturer	= 2,
	defaultComponentAnySubType		= 4
	};

#define defaultComponentAnyFlagsAnyManufacturer defaultComponentAnyFlags+defaultComponentAnyManufacturer
#define defaultComponentAnyFlagsAnyManufacturerAnySubType  defaultComponentAnyFlags+defaultComponentAnyManufacturer+defaultComponentAnySubType

#ifndef forPublicRelease
enum {				/* RegisterComponentResource flags */
	registerComponentGlobal			= 1,
	registerComponentNoDuplicates	= 2,
	registerComponentAfterExisting	= 4
	};
#endif

#ifdef __cplusplus
extern "C" {
#endif __cplusplus

/********************************************************
*														*
*  				APPLICATION LEVEL CALLS					*
*														*
********************************************************/

/********************************************************
* Component Database Add, Delete, and Query Routines
********************************************************/

pascal Component RegisterComponent(ComponentDescription *cd, ComponentRoutine componentEntryPoint, short global, Handle componentName, Handle componentInfo, Handle componentIcon);
pascal Component RegisterComponentResource(ComponentResourceHandle cr, short global);
pascal OSErr UnregisterComponent(Component aComponent);

pascal Component FindNextComponent(Component aComponent, ComponentDescription *looking);
pascal long CountComponents(ComponentDescription *looking);

pascal OSErr GetComponentInfo(Component aComponent, ComponentDescription *cd, Handle componentName, Handle componentInfo, Handle componentIcon);
pascal long GetComponentListModSeed( void );

/********************************************************
* Component Instance Allocation and dispatch routines
********************************************************/

pascal ComponentInstance OpenComponent(Component aComponent);
pascal OSErr CloseComponent(ComponentInstance aComponentInstance);

pascal OSErr GetComponentInstanceError(ComponentInstance aComponentInstance);

/* direct calls to the Components */

pascal long ComponentFunctionImplemented(ComponentInstance ci, short ftnNumber) = ComponentCallNow(-3,2);
pascal long GetComponentVersion(ComponentInstance ci) = ComponentCallNow(-4,0);
pascal long ComponentSetTarget(ComponentInstance ci, ComponentInstance target) = ComponentCallNow(-6,4);


/********************************************************
*														*
*  					CALLS MADE BY ComponentS		  		*
*														*
********************************************************/

/********************************************************
* Component Management routines
********************************************************/

pascal void SetComponentInstanceError(ComponentInstance aComponentInstance, OSErr theError);

pascal long GetComponentRefcon(Component aComponent);
pascal void SetComponentRefcon(Component aComponent, long theRefcon);

pascal short OpenComponentResFile(Component aComponent);
pascal OSErr CloseComponentResFile(short refnum);

/********************************************************
* Component Instance Management routines
********************************************************/

pascal Handle GetComponentInstanceStorage(ComponentInstance aComponentInstance);
pascal void SetComponentInstanceStorage(ComponentInstance aComponentInstance, Handle theStorage);

pascal long GetComponentInstanceA5(ComponentInstance aComponentInstance);
pascal void SetComponentInstanceA5(ComponentInstance aComponentInstance, long theA5);

pascal long CountComponentInstances(Component aComponent);

/* useful helper routines for convenient method dispatching */

pascal long CallComponentFunction(ComponentParameters *params, ComponentFunction func ) = {0x70FF,0xA82A};
pascal long CallComponentFunctionWithStorage(Handle storage, ComponentParameters *params, ComponentFunction func ) = {0x70FF,0xA82A};

pascal long DelegateComponentCall( ComponentParameters *originalParams, ComponentInstance ci ) = {0x7024,0xA82A};

pascal OSErr SetDefaultComponent( Component aComponent, short flags );
pascal ComponentInstance OpenDefaultComponent( OSType componentType, OSType componentSubType );

pascal Component CaptureComponent( Component capturedComponent, Component capturingComponent );
pascal OSErr UncaptureComponent( Component aComponent );

pascal long RegisterComponentResourceFile(short resRefNum, short global);

pascal OSErr GetComponentIconSuite(Component aComponent, Handle *iconSuite);

#ifndef forPublicRelease
/********************************************************
* Private component routines
********************************************************/

pascal long LoadComponent( Component aComponent );
pascal OSErr UnloadComponent( Component aComponent );
pascal OSErr DestroyComponent( Component aComponent );
pascal RegisteredComponentInstancePtr ValidComponentInstance(ComponentInstance aComponentInstance);
pascal RegisteredComponentPtr ValidComponent(Component aComponent);
pascal OSErr GrowRegisteredComponentInstanceTable( short growCount );
pascal OSErr GrowRegisteredComponentTable( short growCount );
pascal void InitComponentManager(Ptr *cutBackAddress);
pascal OSErr GetComponentInfoPrivate( Component aComponent,
				Handle  nameHand, Handle infoHand, Handle iconHand );
pascal short ResolveComponentPlatform(ComponentResource **htr, ComponentDescription *cd, ResourceSpec *rs);

#endif forPublicRelease

#ifdef __cplusplus
}
#endif __cplusplus

#endif __COMPONENTS__
