(*
 *	Reduce	- XFCN to compress runs of spaces and tabs to a space
 *					- Fully MPW 3.0 compatible
 *					- written by Dan Allen
 *
 *	Sample HyperTalk line:
 *
 *	put reduce(field 1) into field 1 -- reduce tabs & spaces
 *
 *)

{$R-}
{$Z+} (* has to have this or else put EntryPoint in Interface section *)

UNIT DummyUnit;


INTERFACE

USES
	Types,
	Memory,
	HyperXCmd;


IMPLEMENTATION


PROCEDURE MoveHHiTrap(h: Handle); INLINE $205F, $A064;


PROCEDURE EntryPoint(paramPtr: XCmdPtr);
CONST
	tab = 9;
	space = 32;
VAR
	h: 		Handle;		
	p,q:	Ptr;

BEGIN
	WITH paramPtr^ DO
		BEGIN
			IF paramCount <> 1 THEN EXIT(EntryPoint);
			MoveHHiTrap(params[1]);
			h := NewHandle(GetHandleSize(params[1]));
			IF h = NIL THEN EXIT(EntryPoint);
			p := params[1]^;
			q := h^;
			WHILE p^ <> 0 DO
				IF (p^ = tab) OR (p^ = space) THEN
					BEGIN
						REPEAT
							p := POINTER(ORD(p)+1);
						UNTIL (p^ <> tab) AND (p^ <> space);
						q^ := space;
						q := POINTER(ORD(q)+1);
					END
				ELSE
					BEGIN
						q^ := p^;
						p := POINTER(ORD(p)+1);
						q := POINTER(ORD(q)+1);
					END;
			q^ := 0;
			returnValue := h;
		END; { with }
END; { procedure }

END. { unit }
