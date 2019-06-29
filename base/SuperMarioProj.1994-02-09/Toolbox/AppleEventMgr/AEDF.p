{
	File:		AEDFWrapper.inc1.p

	Written by:	Ed Lai

	Copyright:	© 1990-1991 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <3>	 5/21/91	gbm		Stop using the "empty" units (MemTypes, etc...)
		 <2>	12/13/90	Lai		Separation of the main files into separate modules
			Summary of B3 Changes.
			In all calls that returns a descriptor, return 'NULL' with NIL handle if there is
				an error.
			Always pass descriptor internally as VAR to reduce code size, too many to list.
			Rename every internal AEXXXXXX call to AE_XXXXXX, too many to list.
			Split file up into smaller module, too many to list.
			The procedure RoundUp is removed, it is now done in line because it is faster and
				yet reduce code size slightly, too many to list.
			Coercion from 'exte' to numeric bug is fixed.
			Bug that SANE flags are affected by numeric coercion (hence if overflow occurs
				once, coercion will always fail afterwards) is fixed.
			Coercion from TEXT to numeric would check to see if string is too long and that
				there is no garbage at the end.
			Coercion from numeric to TEXT is now included.
			In coercion from Boolean, it is also possible to do it from 'true' 'fals' and
				'enum' with value 'true'/'fals'.
			In coercion from anything to AERecord, data is checked to see if it look like an
				AERecord at all. The procedure CheckIsRecord is added for this purpose.
			StdCoercion is rewrittento remove nest procedures.
			Long division is removed to reduce code size.
			Error handling by nested CheckErr procedure is replaced by GOTO to reduce code size,
				too many to list.
			The call CheckDesc is introduced to convert four letter type into enumerator to
				cut down 4 byte comparisions, also it provides a bottle neck so that the
				AE_GetXXXX can call the blocking handler.
			The MustHaveGlobal routine for the system global and application global has been
				consolidated to save code. The AEInit call has been incorported into
				MustHaveGlobal and the Init for system global is also incorported here to
				save code.
			The handler tables are now generated on demand rather than during initialization.
			In WriteData, use Ptr(-1) rather than NIL in dataPtr to denote delete to save code.
			Better factoring in AEGetArray and AEPutArray to reduce code size.
			GetHeaderInfo is now incorporated into FindDescOffset to save code, and all the
				information is in a record to avoid passing too many parameters.
			The waitList, the list of reply waiting for the answer message is now a linked
				list rather than an array in a handle.
			The message header has been changed to remove fields that are present for historical
				reason, places affected too numerous to list.
			The pre-B3 switching layer code has the bug that if the sender of the message
				has timed out or canceled, the server is not aware of it and still switch
				layer completely suprising the user. The only satisfactory way to solve the
				problem is to use a different strategy to switch layer. In stead of having
				the background application call set front process, it send an AppleEvent to
				the foreground process which would switch in the background application only
				if it is still in the wait reply loop.
			Fixes the problem that there is a pending update event, if the application has
				an idle proc but does not process the update event, the AppleEvent will never
				be seen. Get around the problem by using the update mask only on the every
				other call to WaitNextEvent.
			Now we do not return errUnknownSendMode when the send mode is kAECanSwitchLayer
				+ kAENeverInteract, or kAECanSwitchLayer + kAEAnyInteract.
			AE_DeleteItem, AE_DeleteKeyDesc, AE_PutKeyPtr, AE_PutPtr, AE_PutDesc, AE_PutKeyDesc,
				AE_GetNthDesc, AE_GetAttributePtr, AE_GetAttributeDesc, AE_SizeOfAttribute,
				AE_InstallEventHandler, AE_InstallCoercionHandler, AE_RemoveEventHandler,
				AE_RemoveCoercionHandler, AE_GetCoercionHandler are all procedures with
				only one line in it to call another common internal routine and the parameters
				stack is already set up in a form very close to wat the internal common routine
				expects. So they can easily be rewritten in assembler to improve both code size
				and speed. 
			GetSpecialProcOffset is rewritten in assembler to save code space.
			Add the 'errn' and 'errs' constant to interface file.
			In B2, in an 'aevt' 'ansr' came in that matches the message on the wait list but
				there is no memory to read it in, the message will be discarded, which means
				the message will be on the wait list for ever. In B3 it is marked and
				treated reply has arrived but attempting to access the content will result
				in an error.
			In B3, return ID of every process will start with the same value, now it would
				be different for every application.
			Now there will be no sound in the default notification when trying to interact with
				user.
			In AEInteractWithUser, if we switched layer then we would switch back to the message
				orginator only if AEM does the layer switching, we don't do it if the user switches
				it.
			ReplyFilter and ReceiptFilter are now combined so that it is possible to process
				AppleEvent when waiting for a receipt.
			Alias to FSS coercion now will not pop up a mount volume dialog if no interaction is
				spedified.
			
			Accessing the event handler and coercion routines now goes through the hash manager
				rather than by sequential search. All the routines assoicated with Install
				Get and Remove handlers are rewritten.
				
			
			AEDisposeDesc now checks if there is a special handler first so that applicatin
				can do special dispose, to be used for disposing token in object library.
			Blocking and unblocking handler are provided. When using a AE_GetXXXX, if reply
				has not yet arrived, blocking handler will be called if it is available.
				When a reply arrives, unblocking handler will be called if available.
			Add code for special handler that allows additional dispatching to support the
				object library call back.
			The attribute 'refc' is a LONGINT for use by the application, it is not send as
				part of the message.
			There is now a delete range call.
			There is now a create object call.
			The non-AppleEvent handler call is in B1 but there cannot not be installed, now
				it can.
				
		 <1>	  9/7/90	Lai		first checked in
		<0>	  9/7/90	Lai		First Checked in to BBS

	To Do:
}

{[a-,body+,h-,o=100,r+,rec+,t=4,u+,#+,j=20/57/1$,n-]}	{ Pasmat format control line }
{ AEDF.p }
{ Copyright © 1984-1990 by Apple Computer Inc.	All rights reserved. }


UNIT AEDF;

INTERFACE

USES
	Types, Memory, Errors,
	AppleEvents, AEHashTable, AEUtil, AEHandlerTable, AECoercion, AEDFWrapper;
	
CONST
	kAEProtocolMask		= $0000000F;					{ mask for sending protocols }
	kAEInteractMask 	= $00000070;					{ mask for interact level in send protocol }
	
	{ bit number in the AESendMode }
	kBitCanSwitchLayer	= 6;
	kBitDontReconnect	= 7;
	kBitPartOfReply		= 8;
	kBitWantReceipt		= 9;
	
	kBitAttnMsg			= 0;
	


	FUNCTION AE_ResumeTheCurrentEvent(VAR theAppleEvent, reply: AppleEvent; { VAR is for efficiency only }
								   dispatcher:EventHandlerProcPtr; 
								   handlerRefcon:Longint): OSErr; 
	
	FUNCTION AE_Send(VAR theAppleEvent: AppleEvent;{ VAR is for efficiency only }
					VAR reply: AppleEvent;
					sendMode: AESendMode;
					sendPriority: AESendPriority;
					timeOutInTicks: LONGINT;
					idleProc: IdleProcPtr;
					filterProc: EventFilterProcPtr): OSErr;
	

	FUNCTION AE_ProcessAppleEvent(VAR theEventRecord: EventRecord): OSErr;

IMPLEMENTATION

{$I AEDF.inc1.p}

END. { AEDF }
