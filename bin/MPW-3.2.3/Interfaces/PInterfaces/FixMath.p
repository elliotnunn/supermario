{
Created: Thursday, March 15, 1990 at 8:43 AM
	FixMath.p
	Pascal Interface to Fixed Point Math

	Copyright Apple Computer, Inc.	1985-1990
	All rights reserved
	
CHANGE LOG:
	23 Oct 90  JPO	Changed functions Frac2X, Fix2X, X2Fix, and X2Frac
					  to in-line trap calls for non-mc68881 mode
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT FixMath;
	INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingFixMath}
{$SETC UsingFixMath := 1}

{$I+}
{$SETC FixMathIncludes := UsingIncludes}
{$SETC UsingIncludes := 1}
{$IFC UNDEFINED UsingTypes}
{$I $$Shell(PInterfaces)Types.p}
{$ENDC}
{$SETC UsingIncludes := FixMathIncludes}


FUNCTION Fix2Frac(x: Fixed): Fract;
	INLINE $A841;
FUNCTION Fix2Long(x: Fixed): LONGINT;
	INLINE $A840;
FUNCTION FixATan2(x: LONGINT;y: LONGINT): Fixed;
	INLINE $A818;
FUNCTION Long2Fix(x: LONGINT): Fixed;
	INLINE $A83F;
FUNCTION Frac2Fix(x: Fract): Fixed;
	INLINE $A842;
	
{$IFC OPTION(MC68881)}

FUNCTION Frac2X(x: Fract): Extended;
FUNCTION Fix2X(x: Fixed): Extended;
FUNCTION X2Fix(x: Extended): Fixed;
FUNCTION X2Frac(x: Extended): Fract;

{$ELSEC}

FUNCTION Frac2X(x: Fract): Extended;
	INLINE $A845;
FUNCTION Fix2X(x: Fixed): Extended;
	INLINE $A843;
FUNCTION X2Fix(x: Extended): Fixed;
	INLINE $A844;
FUNCTION X2Frac(x: Extended): Fract;
	INLINE $A846;

{$ENDC}

FUNCTION FracMul(x: Fract;y: Fract): Fract;
	INLINE $A84A;
FUNCTION FixDiv(x: Fixed;y: Fixed): Fixed;
	INLINE $A84D;
FUNCTION FracDiv(x: Fract;y: Fract): Fract;
	INLINE $A84B;
FUNCTION FracSqrt(x: Fract): Fract;
	INLINE $A849;
FUNCTION FracSin(x: Fixed): Fract;
	INLINE $A848;
FUNCTION FracCos(x: Fixed): Fract;
	INLINE $A847;

{$ENDC}    { UsingFixMath }

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

