{
	File:		AECoercion.p

	Copyright:	© 1990-1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 5/29/92	DCL		Included TextUtils.p. NumToString was moved for the New Inside
									Mac.
		 <2>	 5/21/91	gbm		Stop using the "empty" units (MemTypes, etc...)
		 <1>	12/13/90	Lai		first checked in

	To Do:
}

UNIT AECoercion;

INTERFACE

USES
	Types, Memory, Packages, Errors, SANE, Aliases, Processes, TextUtils,
	AppleEvents, AEHashTable, AEUtil, AEHandlerTable;


	FUNCTION AE_CoercePtr(typeCode: DescType;
						  dataPtr: Ptr;
						  dataSize: LONGINT;
						  toType: DescType;
						  VAR result: AEDesc): OSErr;

	FUNCTION AE_CoerceDesc(VAR desc: AEDesc; { VAR is for efficiency only }
						  toType: DescType;
						  VAR result: AEDesc): OSErr;

	
IMPLEMENTATION

{$I AECoercion.inc1.p}

END. { AECoercion }