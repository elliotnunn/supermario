{*-------------------------------------------------------------------------------*
 |																				 |
 |							  <<< Signal Manager >>>							 |
 |																				 |
 |					   Copyright Apple Computer, Inc. 1986, 1987, 1988			 |
 |							   All rights reserved. 							 |
 |																				 |
 *-------------------------------------------------------------------------------*}
{
	Signal Handling interface.
	This must be compatible with C's <signal.h>
}

UNIT Signal;

INTERFACE

TYPE
	SignalMap = 	INTEGER;
	SignalHandler = ^LONGINT;	{ Pointer to function }

CONST
	SIG_ERR =	   -1;			{ Returned by IEsignal on error }
	SIG_IGN =		0;
	SIG_DFL =		1;
	SIG_HOLD =		3;
	SIG_RELEASE =	5;

	SIGABRT =		$0001;
	SIGINT =		$0002;		{ Currently only SIGINT implemented }
	SIGFPE =		$0004;
	SIGILL =		$0008;
	SIGSEGV =		$0010;
	SIGTERM =		$0020;

{ Signal Handling Functions }

FUNCTION
	IEsignal(sigNum: LONGINT; sigHdlr: UNIV SignalHandler):
	SignalHandler; C;

FUNCTION
	IEraise(sigNum: LONGINT):
	LONGINT; C;

END.
