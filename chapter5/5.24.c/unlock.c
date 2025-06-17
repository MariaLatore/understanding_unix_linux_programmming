#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include"filename.h"

int
unlock_file ()
{
  struct stat fileinfo = { 0 };
  if (0 == stat (lock_filename, &fileinfo))
    {
      if (0 > unlink (lock_filename))
	{
	  perror ("unlink file");
	  return -1;
	}
    }

  return 0;
}

int
main ()
{
  return unlock_file ();
}
