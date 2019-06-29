/*
	File:		CRCPatch.c

	Contains:	Source to MPW tool that reads the code segment 1 from
				the resource file and calculates the CRC value.  It then
				patches the file and saves it.
				
	Written by:	George Norman - 7/30/86

	Copyright:	© 1986, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM4>	12-04-92	jmp		Fixed a 32-bit cleanliness problem and temporarily changed
									'decl' back to 'CODE' as I checked 3.2’s Linker into the
									project.
		 <3>	 3/31/92	JSM		Rolled this file into Reality.
		 <2>	 2/20/92	kc		Change resource type from 'CODE' to 'decl'.
		 <1>	 1/31/92	JSM		first checked in

*/

#include <stdio.h>
#include <types.h>
#include <osutils.h>
#include <files.h>
#include <resources.h>
#include <memory.h>
#include <errno.h>


pascal void debugger() extern 0xa9ff;


pascal void _CalcCRC (SizeCode,CodePtr,crc)
long		SizeCode;
Ptr			CodePtr;
long		*crc;
extern;

/******************************************************************
	Main
*******************************************************************/
main(argc,argv)

int	  argc;
char  *argv[];

{
  short		refnum;
  Handle	CodeHandle;		/* Handle to code resource */
  short		IOR;			/* IO Result */
  long		SizeCode;		/* Size of the code */
  Ptr		CodePtr;		/* Pointer to the code */
  long		crc;			/* the crc value */
		
  if (argc == 1)
    {
      fprintf(stderr,"### ERROR : No input file specified.\n");
      fprintf(stderr,"### SYNTAX: crcPatch  filename\n");
      fprintf(stderr,"### DSCRPT: Calculate and patch the crc value to code segment 1.\n");
    }
  else if (argc != 2)
    {
      fprintf(stderr,"### ERROR : Wrong number of parameters specified.\n");
      fprintf(stderr,"### SYNTAX: crcPatch  filename\n");
      fprintf(stderr,"### DSCRPT: Calculate and patch the crc value to code segment 1.\n");
    }
  else
    {
      refnum = openresfile(argv[1]);
	  if (refnum < 0 )
        fprintf(stderr,"### ERROR : Resource file: %s can't be opened. err = %d.\n",argv[1],refnum);
	  else
	    {
		   CodeHandle = Get1Resource('CODE',1);
		   
		   if (CodeHandle == nil)
             fprintf(stderr,"### ERROR : Code resource not available. Err = %d\n",errno);
		   else
		     {
			   LoadResource(CodeHandle);	// Make sure the resource is loaded.
			   
			   HNoPurge(CodeHandle);		// Make sure it stays loaded.
			   HLock(CodeHandle);			// Make sure it doesn’t move.
			   				
			   // Originally, this code subtracted 4 from SizeCode and added 4 to CodePtr
			   // due to the fact that the Linker would automatically put in space for a
			   // 'CODE' resource’s jump table.  With the 3.2 Linker, this is no longer done
			   // when the -rt option is specified.
			   //
			   SizeCode = GetHandleSize(CodeHandle);
			   CodePtr = (Ptr)(*CodeHandle); 
		
		       _CalcCRC(SizeCode,CodePtr,&crc);								/* Calc and patch the crc value */

			   ChangedResource(CodeHandle);
			   IOR = ResError();
			   if (IOR != 0)
				 fprintf(stderr,"### ERROR : ChangedResource. Err = %d\n",IOR);

			   WriteResource(CodeHandle);
			   IOR = ResError();
			   if (IOR != 0)
				 fprintf(stderr,"### ERROR : WriteResource. Err = %d\n",IOR);

			   CloseResFile(refnum);
			   IOR = ResError();
			   if (IOR != 0)
				 fprintf(stderr,"### ERROR : CloseResFile. Err = %d\n",IOR);
			 }
	    }
	}
}

