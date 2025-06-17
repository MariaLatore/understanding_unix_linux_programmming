#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<string.h>

int
main (int argc, char *argv[])
{
  if (2 != argc)
    {
      printf ("Usage:%s <y|n>\n", argv[0]);
      exit (1);
    }

  struct stat fileinfo = { 0 };
  if (0 > fstat (0, &fileinfo))
    {
      perror ("fd 0");
      exit (1);
    }

  mode_t mode = fileinfo.st_mode;

  if (0 == strcmp (argv[1], "y"))
    {
      mode |= S_IWGRP;
    }
  else if (0 == strcmp (argv[1], "n"))
    {
      mode &= ~S_IWGRP;
    }
  else
    {
      printf ("Usage:%s <y|n>\n", argv[0]);
      exit (1);
    }
  if (0 > fchmod (0, mode))
    {
      perror ("fchmod");
      exit (1);
    }
  return 0;
}
