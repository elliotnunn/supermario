{[n+,u+,r+,d+,#+,j=13-/40/1o,t=2,o=95] PasMat formatting options}

{------------------------------------------------------------------------------

FILE ResEqual.p
 Copyright Apple Computer, Inc. 1986, 1987
 All rights reserved.

NAME
 ResEqual -- compare resources in two files

SYNOPSIS
 resequal [-p] file1 file2

DESCRIPTION
 "resequal" compares the resources in two files. If both files contain
 the same resources, and all of the resources are equal, it runs silently.
 It reports resources found in only one of the files, resources of
 different sizes, resources with different contents, and differing
 resource attributes. Output is written to standard output. The -p
 option writes progress information to diagnostic output.
------------------------------------------------------------------------------}
{$R-} 																 { Turn off range checking}
PROGRAM ResEqual;

	USES
		Memtypes, Quickdraw, OSIntf, ToolIntf, PackIntf, { Standard Includes}
		CursorCtl,												 { for the spinning cursor}
		Signal, 													 { to handle command-period}
		PasLibIntf, 											 { for standard I/O, etc.}
		IntEnv; 													 { for argV and argC}

{ CONST
	  Version 		 = '2.1'; 						   Current version, now in 'vers' resource}

	VAR

		file1Ref: 	 integer; 						 { the refnums for our files }
		file2Ref: 	 integer;
		curRes: 		 integer;
		file1Name:	 Str255;							 { the names of the files we're comparing}
		file2Name:	 Str255;
		quiet:			 Boolean; 						 { True ==> no info on Diagnostic file}
		progName: 	 Str255;							 { Program's file name}
		interrupted: Boolean; 						 { True ==> interrupted (Cmd "." pressed)}
		retCode:		 (RC_Normal, RC_ParmErrs, RC_DontMatch, RC_Abort); {Return codes}

		{[j=0] PasMat formatting option}

 {*----------------------------------*
 | Stop - terminate execution  |
 *----------------------------------*}

	PROCEDURE Stop(msg: Str255);

		BEGIN 														 {Stop}
			IF Length(msg) > 0 THEN
				BEGIN
				PLFlush (Output);
				WriteLn(Diagnostic);
				WriteLn(Diagnostic, msg);
				END;

			IF interrupted THEN IEexit(-9);
	{ don't worry about closing the files we opened.  The Shell
		will do so if appropriate.}
			IEexit(Ord(retCode)); 					 {exit, returning the appropriate status code}
		END;															 {Stop}

 {*--------------------------------------*
 | Intr - Process external interrupt	|
 | this routine is passed to IEsigset |
 *--------------------------------------*}

	PROCEDURE Intr;

		BEGIN 														 {Intr}
			interrupted := True;						 {we test this switch periodically}
		END;															 {Intr}

	{$S Init}

{*-------------------------------------------------------------*
 | SyntaxError - Report an error in parameters or options |
 *-------------------------------------------------------------*}

	PROCEDURE SyntaxError(suffix: Str255);

		BEGIN 														 {SyntaxError}
			PLFlush (Output);
			WriteLn(Diagnostic, '### ', progName, ' - ', suffix);
			Stop(Concat('# Usage: ', progName, ' file1 file2 [-p]'));
		END;															 {SyntaxError}

 {*-----------------------------------*
 | LetterOpt - Set a letter option |
 *-----------------------------------*}

	PROCEDURE LetterOpt(opt: Char; VAR argVIndex: integer);
 { note only one option is supported.  argVIndex is passed to
 this routine so options that have arguments can 'eat' them}

		BEGIN 														 {LetterOpt}
			CASE opt OF
				'p', 'P': quiet := False;
				OTHERWISE SyntaxError(Concat(ArgV^[argVIndex]^, ' <invalid option>'));
			END;														 {case}
		END;															 {LetterOpt}

 {*---------------------------*
 | Init - Tool initalization |
 *---------------------------*}

	PROCEDURE Init;

		VAR
			ioRslt, argVIndex, fileCount, holdIndex: integer;
			prevSig: SignalHandler;
			strH: StringHandle;
			versH: StringHandle;
			nextFileNamePtr: StringPtr;
			arg: Str255;
			versStr: Str255;

		BEGIN 														 {Init}
			retCode := RC_Normal;

			interrupted := False; 					 {becomes True when interrupted}
			prevSig := IEsignal(SIGINT, @Intr);

			quiet := True;
			progName := ArgV^[0]^;
			retCode := RC_ParmErrs;

			fileCount := 0;
			retCode := RC_ParmErrs;
			file1Ref := - 1;								 { so we tell if the open works}
			file2Ref := - 1;								 { as above}
			argVIndex := 1;
			WHILE argVIndex < ArgC DO 			 {ArgC is the number of args plus one}
				BEGIN
				arg := ArgV^[argVIndex]^;
				IF (Length(arg) <> 0) THEN
					BEGIN
					IF arg[1] = '-' THEN				 { we have an option }
						BEGIN
						holdIndex := argVIndex;
						LetterOpt(arg[2], argVIndex);
						IF argVIndex <> holdIndex THEN CYCLE; { skip the increment of argVIndex below}
						END
					ELSE												 { it must be a file to open}
						BEGIN
						fileCount := fileCount + 1;
						IF fileCount = 1 THEN
							nextFileNamePtr := @file1Name
						ELSE
							nextFileNamePtr := @file2Name;
						nextFileNamePtr^ := ArgV^[argVIndex]^;
						END;
					END;
				argVIndex := argVIndex + 1;
				END;
			IF fileCount <> 2 THEN SyntaxError('Invalid Parameters');
			curRes := CurResFile;

			IF NOT quiet THEN
				BEGIN
				WriteLn(Diagnostic);

				versH := StringHandle(Get1Resource('vers', 1)); {get the version from the 'vers' resource}

				IF versH <> nil THEN
					BEGIN
					versStr := StringPtr(Ord(versH^) + 6)^;
					ReleaseResource(Handle(versH));
					WriteLn(Diagnostic, progName, '  (Ver ', versStr, ') ');
					END
				ELSE
					WriteLn(Diagnostic, progName, '  (Ver **unavailable**) ');

				WriteLn(Diagnostic);
				WriteLn(Diagnostic);
				END;

			SetResLoad(False);							 { keep preloads from loading}
			file2Ref := OpenRFPerm(file2Name, 0, fsRdPerm);
			SetResLoad(True);
			IF file2Ref = - 1 THEN
				Stop(Concat('### ', progName, ' - ', 'could not open ', file2Name));
			SetResLoad(False);							 { keep preloads from loading}
			file1Ref := OpenRFPerm(file1Name, 0, fsRdPerm);
			SetResLoad(True);
			UseResFile(curRes); 						 { in case file2 opened, but file one doesn't}
			IF file1Ref = - 1 THEN
				Stop(Concat('### ', progName, ' - ', 'could not open ', file1Name));

			retCode := RC_Normal;

			RotateCursor(0);
			IF interrupted THEN Stop('');
		END;															 {Init}

	{$S Main}

 {*-------------------------------------------------*
	| ByteToStr - Convert a byte to a hex string	|
	*-------------------------------------------------*}

	FUNCTION ByteToStr(N: Byte): Str255;

		BEGIN 														 {ByteToStr}
			IF N = 0 THEN
				ByteToStr := '00'
			ELSE
				BEGIN 												 {convert to hex}
				ByteToStr[0] := chr(2);
				ByteToStr[1] := '0123456789ABCDEF'[BAND(BSR(N, 4), $F) + 1];
				ByteToStr[2] := '0123456789ABCDEF'[BAND(N, $F) + 1];
				END;
		END;

	PROCEDURE DumpMem(firstStart, secondStart: Ptr; offset, size: longint);

		PROCEDURE DumpPtr(theStart: Ptr);

			VAR
				count, len: integer;
				limit: longint;
				thePtr: Ptr;
				theStr: Str255;

			BEGIN
				limit := size - offset;
				IF limit > 16 THEN limit := 16;
				theStr := '';
				thePtr := theStart;
				FOR count := 1 TO limit DO
					BEGIN
					theStr := Concat(theStr, ByteToStr(thePtr^), ' ');
					thePtr := Ptr(Ord(thePtr) + 1);
					END;
				theStr := Concat(theStr, '    ');
				thePtr := theStart;
				len := Length(theStr) + limit;
				theStr[0] := chr(len);
				FOR count := len - limit + 1 TO len DO
					BEGIN
					IF (thePtr^ >= Ord(' ')) AND (thePtr^ <= Ord('z')) THEN
						theStr[count] := chr(thePtr^)
					ELSE
						theStr[count] := '.';
					thePtr := Ptr(Ord(thePtr) + 1);
					END;
				WriteLn(theStr);
			END;

		BEGIN
			WriteLn('Contents of resource in file 1 at offset ', offset: 1);
			DumpPtr(firstStart);
			WriteLn('Contents of resource in file 2 at offset ', offset: 1);
			DumpPtr(secondStart);
			WriteLn;
		END;

 {*--------------------------------------*
 | DoIt -- check resources in each file |
 *--------------------------------------*}

	PROCEDURE DoIt; 										 { the guts of our program--in a procedure so the
																				compiler can do register optimizations}

		VAR
			cursorCount: integer; 					 { for our spinning cursor}

			theType: ResType; 							 { parameters for GetResInfo calls}
			theID: integer;
			theName: Str255;

			theSize1, theSize2: longint;
			theAttr1, theAttr2: integer;

 {*-------------------------------------------*
 | CheckResources -- compare those resources |
 *-------------------------------------------*}

		PROCEDURE CheckResources;

			VAR
				typeIndex, resIndex: integer;  { counters for our getindXXXX calls}

				theRes1, theRes2: handle; 		 { handles for our resource compares}
				homeFile: integer;						 { what file is this resource in?}
				clearRes1, clearRes2: Boolean; { call emptyhandle if True}
				firstTime: Boolean; 					 { have we written anything yet?}
				res1State, res2State: SignedByte; { to save and restore MP flags}

			PROCEDURE TellIt(message: Str255);

				BEGIN
					IF firstTime THEN
						BEGIN
						WriteLn;
						WriteLn('File #1: ', file1Name);
						WriteLn('File #2: ', file2Name);
						WriteLn;
						firstTime := False;
						retCode := RC_DontMatch;
						END;
					WriteLn(message);
					WriteLn('Resource Type = "', theType, '"  ID = ', theID: 1);
				END;

			PROCEDURE TellAttr(theAttr: integer);
			
			{ These constants are found in the Resource Manager chapter of 
				Inside Macintosh Vol I, page 111.  Only bits 1-6 of the low-order
				byte of the integer are used for flags. The other bits of the
				integer are reserved or ignored.  }
			BEGIN
					IF BAND(theAttr, $2) <> 0 THEN Write('changed ');
					IF BAND(theAttr, $4) <> 0 THEN Write('preload ');
					IF BAND(theAttr, $8) <> 0 THEN Write('protected ');
					IF BAND(theAttr, $10) <> 0 THEN Write('locked ');
					IF BAND(theAttr, $20) <> 0 THEN Write('purgable ');
					IF BAND(theAttr, $40) <> 0 THEN Write('sysHeap ');
					IF BAND(theAttr, $7E) = 0 THEN Write('-No flags set-');
					WriteLn;
				END;

 {*-------------------------------------------------*
 | CheckEqual--compares contents of 2 handles 	|
 *-------------------------------------------------*}

			PROCEDURE CheckEqual(a, b: handle; size: longint);

				CONST
					maxBad = 10;

				VAR
					count: longint;
					source, target: Ptr;
					numBad: integer;

				BEGIN 												 {CheckEqual}
					numBad := 0;								 { no problems so far}
					source := a^;
					target := b^;
					count  := 0;
					WHILE count < size DO
						BEGIN
						IF source^ <> target^ THEN
							BEGIN
							IF numBad = 0 THEN TellIt('Resources have different contents');
							DumpMem(source, target, count, size);
							numBad := numBad + 1;
							IF numBad >= maxBad THEN LEAVE;
							source := Ptr(Ord(source) + 16);
							target := Ptr(Ord(target) + 16);
							count := count + 16;
							END
						ELSE
							BEGIN
							source := Ptr(Ord(source) + 1);
							target := Ptr(Ord(target) + 1);
							count := count + 1;
							END;
						END;
				END;													 {CheckEqual}

			BEGIN 													 { CheckResources}
				firstTime := True;
				UseResFile(file1Ref);
				FOR typeIndex := 1 TO Count1Types DO { countTypes counts Types in 1st file}
					BEGIN
					UseResFile(file1Ref);
					Get1IndType(theType, typeIndex); { get the next type}
					FOR resIndex := 1 TO Count1Resources(theType) DO
						BEGIN
						UseResFile(file1Ref);
						SetResLoad(False);				 { we want the info, but not the data right now}
						theRes1 := Get1IndResource(theType, resIndex);
						SetResLoad(True); 				 { so loadseg will really get our segments}
						UseResFile(curRes);

						cursorCount := cursorCount + 1;
						RotateCursor(cursorCount);

						IF interrupted THEN Stop(''); { in case the user hit command-period}

						{ we want the resource ID and name}
						GetResInfo(theRes1, theID, theType, theName);
						UseResFile(file2Ref); 		 { now we will try to find a match in the other file}
						SetResLoad(False);				 { we want the info, but not the data right now}
						theRes2 := Get1Resource(theType, theID); { resLoad is FALSE}
						SetResLoad(True); 				 { so loadseg will really get our segments}
						UseResFile(curRes); 			 { from our resource file}

						IF NOT quiet THEN
							BEGIN
							PLFlush (Output);
							WriteLn(Diagnostic, 'Checking resource ', theType, ' #', theID: 1);
							END;

						IF (theRes2 = NIL) THEN 	 {not in our file}
							BEGIN
							TellIt('In 1 but not in 2');
							WriteLn;
							END

						ELSE
							BEGIN
							theAttr1 := GetResAttrs(theRes1);
							theAttr2 := GetResAttrs(theRes2);
							{ Use the $7E constant since only bits 1-6 are significant.}
							IF BAND(theAttr1, $7E) <> BAND(theAttr2, $7E) THEN
								BEGIN
								TellIt('Resources have different flags');
								Write('Flags 1 = ');
								TellAttr(theAttr1);
								Write('Flags 2 = ');
								TellAttr(theAttr2);
								WriteLn;
								END;
							theSize1 := SizeResource(theRes1);
							theSize2 := SizeResource(theRes2);
							IF theSize1 <> theSize2 THEN
								BEGIN
								TellIt('Resources are different sizes');
								WriteLn('Size 1 = ', theSize1: 1, '  Size 2 = ', theSize2: 1);
								WriteLn;
								END
							ELSE
								BEGIN
								clearRes1 := longint(theRes1^) = 0;
								LoadResource(theRes1); { resload is TRUE, so get the data--add check for
																				reserr}
								IF ResError <> 0 THEN
									BEGIN
									Stop('Could not load resource');
									END;
								res1State := HGetState (theRes1); { save current MP flags}
								HNoPurge(theRes1);		{ don't let it be purged}
								clearRes2 := longint(theRes2^) = 0;
								LoadResource(theRes2); { as above}
								IF ResError <> 0 THEN
									BEGIN
									Stop('Could not load resource');
									END;
								res2State := HGetState (theRes2); { save current MP flags}
								MoveHHi(theRes2);
								HLock(theRes2); 	{ lock it down}
								MoveHHi(theRes1);
								HLock(theRes1); 	{ this one as well}
								CheckEqual(theRes1, theRes2, theSize1);
								HSetState(theRes1,Res1State);
								HSetState(theRes2,Res2State);
								IF clearRes1 THEN EmptyHandle(theRes1); { free memory if we loaded resource}
								IF clearRes2 THEN EmptyHandle(theRes2); { as above}
								END 									 {ELSE sizes equal}
							END;										 {ELSE res in both files}
						END;
					END;

				UseResFile(file2Ref);
				FOR typeIndex := 1 TO Count1Types DO { countTypes counts Types in 2nd file}
					BEGIN
					UseResFile(file2Ref);
					Get1IndType(theType, typeIndex); { get the next type}
					FOR resIndex := 1 TO Count1Resources(theType) DO
						BEGIN
						UseResFile(file2Ref);
						SetResLoad(False);				 { we want the info, but not the data right now}
						theRes2 := Get1IndResource(theType, resIndex);
						SetResLoad(True); 				 { so loadseg will really get our segments}
						UseResFile(curRes);

						cursorCount := cursorCount + 1;
						RotateCursor(cursorCount);

						IF interrupted THEN Stop(''); { in case the user hit command-period}

						{ we want the resource ID and name}
						GetResInfo(theRes2, theID, theType, theName);
						UseResFile(file1Ref); 		 { now we will try to find a match in the other file}
						SetResLoad(False);				 { we want the info, but not the data right now}
						theRes1 := Get1Resource(theType, theID); { resLoad is FALSE}
						SetResLoad(True); 				 { so loadseg will really get our segments}
						UseResFile(curRes); 			 { from our resource file}

						IF NOT quiet THEN
							BEGIN
							PLFlush (Output);
							WriteLn(Diagnostic, 'Checking resource ', theType, ' #', theID: 1);
							END;

						IF (theRes1 = NIL) THEN 	 {not in our file}
							BEGIN
							TellIt('In 2 but not in 1');
							WriteLn;
							END

						END;
					END;
			END;														 {CheckResources}

		BEGIN 														 {DoIt}
			cursorCount := 0; 							 { prepare to spin that cursor}
			CheckResources;
			Stop('');
		END;															 {DoIt}

 {*----------------------------*
 | ResEqual -- main program  |
 *----------------------------*}

	BEGIN 															 {ResEqual}
		Init; 														 { sets up world, opens our resource files}
		UnLoadSeg(@Init); 								 { release our initialization segment}
		DoIt; 														 { and call our routine}
	END.																 {ResEqual}
