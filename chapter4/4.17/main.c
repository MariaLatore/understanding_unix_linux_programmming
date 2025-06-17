#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>

int
main (int argc, char *argv[])
{
  if (argc != 3)
    {
      printf ("Usage:%s <src> <dst>\n", argv[0]);
      exit (1);
    }

  char error_msg[100];
  if (-1 != link (argv[1], argv[2]))
    if (0 > unlink (argv[1]))
      {
	sprintf (error_msg,
		 "unlink original file %s error, new link %s has been created",
		 argv[1], argv[2]);
	perror (error_msg);
	exit (1);
      }
  return 0;
}
