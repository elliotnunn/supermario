(*---------------------------------------------------------------------------*
 |                                                                           |
 |                            <<< Unmangler.p >>>                            |
 |                                                                           |
 |                         C++ Function Name Decoding                        |
 |                                                                           |
 |                 Copyright Apple Computer, Inc. 1988-1991                  |
 |                           All rights reserved.                            |
 |                                                                           |
 *---------------------------------------------------------------------------*)

UNIT Unmangler;

	INTERFACE
		
		USES Types;

		(*
		FUNCTION unmangle(dst: UNIV Ptr; src: UNIV Ptr; limit: LongInt): LongInt; C;
			{This function unmangles C++ mangled symbols (i.e. a symbol with a type signature).
			 The mangled C string is passed in “src” and the unmangled C string is returned in
			 “dst”.  Up to “limit” characters (not including terminating null) may be retured
			 in “dst”.
		 
			 The function returns,
		 
				 -1 ==> error, probably because symbol was not mangled, but looked like it was
					0 ==> symbol wasn't mangled; not copied either
					1 ==> symbol was mangled; unmangled result fit in buffer
					2 ==> symbol was mangled; unmangled result truncated to fit in buffer}
		*)
		
		FUNCTION Unmangle(dst: UNIV StringPtr; src: UNIV StringPtr; limit: LongInt): LongInt;
			{This function unmangles C++ mangled symbols (i.e. a symbol with a type signature).
			 The mangled Pascal string is passed in “src” and the unmangled Pascal string is
			 returned in “dst”.  Up to “limit” characters may be retured in “dst”.
			
			 The function returns,
			 
				 -1 ==> error, probably because symbol was not mangled, but looked like it was
					0 ==> symbol wasn't mangled; not copied either
					1 ==> symbol was mangled; unmangled result fit in buffer
					2 ==> symbol was mangled; unmangled result truncated to fit in buffer
					 
			 This function is identical to unmangle() above except that all the strings are
			 Pascal instead of C strings.
			}

END.
