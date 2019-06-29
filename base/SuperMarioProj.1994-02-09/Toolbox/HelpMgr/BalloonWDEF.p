{
	File:		BalloonWDEF.p

	Contains:	BalloonWDEF - A small window defproc code resource for use by Help Mgr.

	Written by:	Randy Carr

	Copyright:	© 1989-1990, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	 3/23/92	JSM		OSEvents.p is obsolete, use Events.p.
		 <7>	 8/16/90	RLC		Change calls to private interface to use OsErr returned in
									interface.
		 <6>	 7/20/90	RLC		Shorten the code again.
		 <5>	 6/26/90	RLC		Shorten to the minimum.
		 <4>	  4/4/90	KSM		Changed references to 'ourPackNum' to be _Pack14.
		 <3>	 1/25/90	RLC		Tweak design of balloon.
		 <2>	 1/25/90	RLC		Put private interface calls right in code.
	   <1.2>	 8/29/89	RLC		Change routine to call the _Pack14 Help Mgr trap directly to
									save code.
	   <1.1>	 7/20/89	RLC		Changes uses interfaces for defproc.
	   <1.0>	 6/29/89	RLC		Created Today.
}
	
UNIT HMWindow;

INTERFACE

USES	Types, QuickDraw, ToolUtils, Fonts, Events, Controls,
		Windows, Dialogs, Files, Menus, Desk, Traps, OSUtils,
		Resources, Memory, Packages, Errors, Layers, Balloons;

	
FUNCTION  HMDrawBalloonFrame(rgn: RgnHandle): OsErr;
		INLINE $303C,$02FE,_Pack14;
		
	
FUNCTION  HMSetupBalloonRgns(	varCode: INTEGER;
								VAR structRgn: RgnHandle;
								VAR contentRgn: RgnHandle;
								VAR bounds: Rect): OsErr;
		INLINE $303C,$07FF,_Pack14;
	
	
FUNCTION  HMWindowDefProc(varCode: INTEGER; theWindow: WindowPtr;
					message: INTEGER; param: LONGINT): LONGINT;


{$D-}
{$R-}


IMPLEMENTATION	
	
		
	
FUNCTION  HMWindowDefProc(varCode: INTEGER; theWindow: WindowPtr;
				message: INTEGER; param: LONGINT): LONGINT;
	VAR	w			: WindowPeek;
		p			: Point;
		r			: Rect;
		rp			: RectPtr;			
			
	BEGIN
		HMWindowDefProc := 0;
		with WindowPeek(theWindow)^ do
			case message of
				wDraw:
					if visible then
						begin
							PenNormal;
							
							if HMDrawBalloonFrame(strucRgn)=noErr then;
							
							PenNormal; {restore to normal state}
						end;
				(*		
				wHit:	{*** Test: allow dragging if CurLayer and Point is set to Help Balloon}
					begin					
						p := Point(param);
					
						if PtInRgn(p, contRgn) then
							HMWindowDefProc := wInContent
								else if PtInRgn(p,strucRgn) then
									HMWindowDefProc := wInDrag
										else HMWindowDefProc := wNoHit;
					end;
				*)
				
				wCalcRgns: {figure out the balloon's position, tip shape, and rgns}
					begin
						r := theWindow^.portRect;
						
						with CGrafPtr(theWindow)^ do
							if portVersion < 0 then { color port }
								rp := @portPixMap^^.bounds
									else rp := @theWindow^.portBits.bounds;
								
						OffsetRect(r, -rp^.left, -rp^.top);
						
						if HMSetupBalloonRgns(varCode,strucRgn,contRgn,r)=noErr then;
						
					end;
			end;
	END;



END.


