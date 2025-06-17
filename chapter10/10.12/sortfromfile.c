#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>

int
main (int argc, char *argv[])
{
  if (2 != argc)
    {
      printf ("Usage: %s <sortfilename>\n", argv[0]);
      exit (1);
    }
  char *filename = argv[1];
  int pid;
  int fd;
  if ((pid = fork ()) == -1)
    {
      perror ("fork");
      exit (1);
    }

  if (pid == 0)
    {
      fd = open (filename, O_RDONLY);
      if (0 > fd)
	{
	  perror ("open sort source file");
	}
      else
	{
	  dup2 (fd, 0);
	  close (fd);
	}

      execlp ("sort", "sort", NULL);
      perror ("execlp");
      exit (1);
    }

  if (pid != 0)
    {
      wait (NULL);
      printf ("Done running sort\n");
    }
  return 0;
}
