/*
				File:		AEDF.r
			
				Contains:	xxx put contents here xxx
			
				Written by:	xxx put writers here xxx
			
				Copyright:	© 1990, 1992 by Apple Computer, Inc., all rights reserved.
			
    This file is used in these builds: BigBang

				Change History (most recent first):
			
	   <SM2>	 11/2/92	kc		Add #if NewBuildSystem conditional to flag obsolete file.
		 <2>	 3/15/91	Lai		BM,#83128: Remove notification sicn for interacting with event
									from another process.
		 <1>	  9/7/90	Lai		first checked in
			
				To Do:
			*/


#if !NewBuildSystem /* This file is obsolete */

include $$Shell("ObjDir")"AppleEventMgr.PACK.rsrc" 'PACK' (8);

#endif