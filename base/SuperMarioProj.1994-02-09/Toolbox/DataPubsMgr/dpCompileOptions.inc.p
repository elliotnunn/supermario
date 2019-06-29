{
	File:		dpCompileOptions.inc.p

	Contains:	converts comand line options to appropriate compiler inlines.

	Written by:	Nick Kledzik

	Copyright:	© 1990 by Apple Computer, Inc., all rights reserved.

    This file is used in these builds: BigBang

	Change History (most recent first):

		 <4>	10/31/90	ngk		No longer force macsbug symbols, if qNames is undefined. This
									allows "-mbg full" switch to work.
		 <3>	 10/8/90	ngk		turn off macsbug symbols, by default for beta builds
		 <2>	 3/25/90	ngk		turn on symbols, by default for alpha builds
		1.2	ngk 08/08/1989  		added qCatchFailures and more comments
		1.1	ngk 05/29/1989  		fixed bug of right-curly bracket in comment
		1.0	ngk 05/29/1989 			Changed from a USES dpCompileOptions.p to [$I dpCompileOptions.inc.p]

	To Do:
}


{
	qRangeCheck 	==> overflow checking of numbers, range checking of arrays
	qUnInit			==> all variables are initialized to $7267...
	qNames			==> all procedures and functions have MacsBug symbols 
	qCatchFailures	==> each failure handler breaks into MacsBug on failure
	
	qDebug			==> qRangeCheck, qUnInit, qNames, qCatchFailures
}


{ by defualt qDebug is False }
{$IFC UNDEFINED qDebug }		
	{$SETC qDebug=FALSE }
{$ENDC }


{ If qDebug is True, set the default for others }
{$IFC qDebug }						
	{$IFC UNDEFINED qRangeCheck}
		{$SETC qRangeCheck=TRUE }
	{$ENDC }
	{$IFC UNDEFINED qUnInit}
		{$SETC qUnInit=TRUE }
	{$ENDC }
	{$IFC UNDEFINED qNames}
		{$SETC qNames=TRUE }
	{$ENDC }
	{$IFC UNDEFINED qCatchFailures}
		{$SETC qCatchFailures=TRUE }
	{$ENDC }
{$ENDC }


{ Set the default for the undefined ones to be False }

{$IFC UNDEFINED qRangeCheck}
	{$SETC qRangeCheck=FALSE }
{$ENDC }

{$IFC qRangeCheck}
	{$R+}
	{$OV+}
{$ELSEC}
	{$R-}
	{$OV-}
{$ENDC}


{$IFC UNDEFINED qUnInit}
	{$SETC qUnInit=FALSE }
{$ENDC }

{$IFC qUnInit}
	{$Init+}
{$ELSEC}
	{$Init-}
{$ENDC}


{$IFC UNDEFINED qNames}
 	{ leave macsbug symbols alone }
{$ELSEC}
	{$IFC qNames}
		{$D+}
		{$N+}
	{$ELSEC}
		{$D-}
		{$N-}
	{$ENDC}
{$ENDC }


{$IFC UNDEFINED qCatchFailures}
	{$SETC qCatchFailures=FALSE }
{$ENDC }

