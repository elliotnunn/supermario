{[n+,u+,r+,d+,#+,j=13-/40/1o,t=2,o=95] PasMat formatting options}
{------------------------------------------------------------------------------
FILE TestPerf.p
 Copyright Apple Computer, Inc. 1985-1987
 All rights reserved.

NAME
  TestPerf

DESCRIPTION
  This small tool demonstrates the use of the performance analysis tools.
------------------------------------------------------------------------------}
PROGRAM TestPerf;

 {$SETC doPerform := true} {set to false to omit Perform tools when debugging. }

 {$Z*}

  USES MemTypes, 
		{$IFC doPerform}
		Perf,
		{$ENDC}
    {needed for waste procedures, but not for Perf.p:}
       QuickDraw;

		{$IFC doPerform}
  VAR
    ThePGlobals: TP2PerfGlobals;
		{$ENDC}
		
    {[j=0] PasMat formatting option}

    { These are some procedures that "waste" time (W) in different amounts. }

    { First, some procedures that waste time in CODE segments: }

    {$S SEG1}

  PROCEDURE W500A;

    VAR
      i: integer;
      junk, junk1, junk2: {integer} longint;

    BEGIN
      FOR i := 1 TO 500 DO
        BEGIN
        junk := 1;
        junk1 := junk * 5;
        junk2 := (junk + junk1) * 5;
        END;
    END;                               {W500A}

  PROCEDURE W100;

    VAR
      i: integer;
      junk, junk1, junk2: {integer} longint;

    BEGIN
      FOR i := 1 TO 100 DO
        BEGIN
        junk := 1;
        junk1 := junk * 5;
        junk2 := (junk + junk1) * 5;
        END;
    END;                               {W100}

  PROCEDURE W1500;

    VAR
      i: integer;
      junk, junk1, junk2: {integer} longint;

    BEGIN
      FOR i := 1 TO 1500 DO
        BEGIN
        junk := 1;
        junk1 := junk * 5;
        junk2 := (junk + junk1) * 5;
        END;
    END;                               {W1500}

  {$S SEG2}

  PROCEDURE Waste;

    VAR
      i: integer;
      junk, junk1, junk2: {integer} longint;

    BEGIN
      FOR i := 1 TO 1 DO
        BEGIN
        junk := 1;
        junk1 := junk * 5;
        junk2 := (junk + junk1) * 5;
        END;
    END;                               {Waste}

  PROCEDURE W500BwithAVeryLongName;

    VAR
      i: integer;
      junk, junk1, junk2: {integer} longint;

    BEGIN
      FOR i := 1 TO 500 DO
        BEGIN
        junk := 1;
        junk1 := junk * 5;
        junk2 := (junk + junk1) * 5;
        END;
    END;                               {W500BwithAVeryLongName}

  { Second, some procedures that waste time in ROM: }

  {$S ROMSEG1}

  PROCEDURE ROMW500A;

    VAR
      i: integer;
      junk, junk1, junk2: rect;
      dontCare: Boolean;

    BEGIN
      FOR i := 1 TO 500 DO
        BEGIN
        SetRect(junk, 100, 200, 300, 400);
        SetRect(junk1, 200, 300, 400, 500);
        dontCare := SectRect(junk, junk1, junk2);
        END;
    END;                               {ROMW500A}

  PROCEDURE ROMW100;

    VAR
      i: integer;
      junk, junk1, junk2: rect;
      dontCare: Boolean;

    BEGIN
      FOR i := 1 TO 100 DO
        BEGIN
        SetRect(junk, 100, 200, 300, 400);
        SetRect(junk1, 200, 300, 400, 500);
        dontCare := SectRect(junk, junk1, junk2);
        END;
    END;                               {ROMW100}

  PROCEDURE ROMW1500;

    VAR
      i: integer;
      junk, junk1, junk2: rect;
      dontCare: Boolean;

    BEGIN
      FOR i := 1 TO 1500 DO
        BEGIN
        SetRect(junk, 100, 200, 300, 400);
        SetRect(junk1, 200, 300, 400, 500);
        dontCare := SectRect(junk, junk1, junk2);
        END;
    END;                               {ROMW1500}

  {$S ROMSEG2}

  PROCEDURE ROMWaste;

    VAR
      i: integer;
      junk, junk1, junk2: rect;
      dontCare: Boolean;

    BEGIN
      FOR i := 1 TO 1 DO
        BEGIN
        SetRect(junk, 100, 200, 300, 400);
        SetRect(junk1, 200, 300, 400, 500);
        dontCare := SectRect(junk, junk1, junk2);
        END;
    END;                               {ROMWaste}

  PROCEDURE ROMW500B;

    VAR
      i: integer;
      junk, junk1, junk2: rect;
      dontCare: Boolean;

    BEGIN
      FOR i := 1 TO 500 DO
        BEGIN
        SetRect(junk, 100, 200, 300, 400);
        SetRect(junk1, 200, 300, 400, 500);
        dontCare := SectRect(junk, junk1, junk2);
        END;
    END;                               {ROMW500B}

  VAR
    repeats: integer;

  BEGIN

		{$IFC doPerform}
    ThePGlobals := NIL;
    IF NOT InitPerf(ThePGlobals,
										4 			{ms},
										2				{bytes},
										true,		{measure ROM code}
										true, 	{measure application code}
										'CODE',	{segments to measure}
										0,			{let the performance tools calculate the ROM id}
										'',			{let the performance tools find the ROM name}
                    false,	{don't measure RAM}
										0 			{low RAM},
										$1FFFFF	{high RAM (2M)},
       							16			{default} ) THEN
        BEGIN
        writeln('Errors during InitPerf.');
        Exit(TestPerf);
        END;
				
    IF PerfControl(ThePGlobals, true) THEN {turn on, throw away old state} ;
		{$ENDC}

    FOR repeats := 1 TO 5 DO
      BEGIN
      { waste some time in user Code/MUL4 }
      Waste;
      W100;
      W500A;
      W500BwithAVeryLongName;
      W1500;

      { waste some time in ROM calls: }
      ROMWaste;
      ROMW100;
      ROMW500A;
      ROMW500B;
      ROMW1500;
      END;

		{$IFC doPerform}
    IF PerfControl(ThePGlobals, false) THEN {turn off, throw away old state} ;

    IF PerfDump(ThePGlobals, 'Perform.out', false, 80) <> 0 THEN
      writeln('Errors during dump.');

    TermPerf(ThePGlobals);
		{$ENDC}

  END.
