/*
	File:		Data.c

	Contains:	Source to MPW tool that reads the code segment 1 from
				the resource file and writes it out to the data fork of
				another file.
				
	Written by:	George Norman - 7/30/86

	Copyright:	© 1986, 1992 by Apple Computer, Inc., all rights reserved.

	Change History (most recent first):

	   <SM5>	12-04-92	jmp		Fixed a 32-bit cleanliness problem and temporarily changed
									'decl' back to 'CODE' as I checked 3.2’s Linker into the
									project.
		 <4>	 3/31/92	JSM		Rolled this file into Reality.
		 <3>	 2/20/92	kc		Change DeclData resource type from 'CODE' to 'decl'. Remove code
									to skip resource header.
		 <2>	 2/13/92	RB		Need to call StripAddress on the resource pointer instead of
									anding the address with FF's.
		 <1>	 1/31/92	JSM		first checked in

*/

#include <stdio.h>
#include <fcntl.h>
#include <types.h>
#include <osutils.h>
#include <files.h>
#include <resources.h>
#include <memory.h>
#include <errno.h>


/******************************************************************
	Main
*******************************************************************/
main(argc,argv)

int	  argc;
char  *argv[];

{
  short		refnum;
  Handle	CodeHandle;		/* Handle to code resource */
  long		SizeCode;		/* Size of the code */
  Ptr		CodePtr;		/* Pointer to the code */
  int		FileDescript;	/* The file descriptor of the data file */
  unsigned	NumBWritten;	/* Number of bytes written per write */
  unsigned	Total;			/* Total number of bytes written */
		
  if (argc == 1)
    {
      fprintf(stderr,"### ERROR : No files specified.\n");
      fprintf(stderr,"### SYNTAX: Data  rsrcFileName  DataFileName\n");
      fprintf(stderr,"### DSCRPT: Copy code segment 1 to a data file.\n");
    }
  else if (argc != 3)
    {
      fprintf(stderr,"### ERROR : Wrong number of parameters specified.\n");
      fprintf(stderr,"### SYNTAX: Data  rsrcFileName  DataFileName\n");
      fprintf(stderr,"### DSCRPT: Copy code segment 1 to a data file.\n");
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
			   
			   FileDescript = creat(argv[2]);
			   if (FileDescript < 0)
                 fprintf(stderr,"### ERROR : Data file: %s can't be opened. err = %d.\n",argv[2],errno);
			   else
			     {
				   Total = 0;
				   while (Total < SizeCode)
				     {
					   NumBWritten = write(FileDescript,CodePtr,SizeCode);
					   if (NumBWritten < 0)
					     {
						   fprintf(stderr,"### ERROR : Write err = %d.\n",errno);
						   Total = SizeCode;
						 }
					   else
					     Total = Total + NumBWritten;
					 }
				   close(FileDescript);
				 }

			   CloseResFile(refnum);
			 }
	    }
	}
}

