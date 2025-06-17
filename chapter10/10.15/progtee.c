#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

#define MAX_PROG_ARGNUM 10

int
main (int argc, char *argv[])
{

  if (argc == 1 || argc > MAX_PROG_ARGNUM + 1)
    {
      printf ("Usage: %s <prog> <prog arg1> ...(max %d args)\n", argv[0],
	      MAX_PROG_ARGNUM - 1);
      exit (1);
    }

  char *progargs[MAX_PROG_ARGNUM + 1] = { NULL };
  int i;
  int pipefd[2];
  int pid;
  char c;
  for (i = 1; i < argc; i++)
    progargs[i - 1] = argv[i];

  if (-1 == pipe (pipefd))
    {
      perror ("pipe error");
      exit (1);
    }

  if ((pid = fork ()) == -1)
    {
      perror ("fork error");
      exit (1);
    }

  if (0 < pid)
    {
      if (-1 == (dup2 (pipefd[0], 0)))
	{
	  perror ("dup2 error");
	  exit (1);
	}

      close (pipefd[0]);
      close (pipefd[1]);
      execvp (progargs[0], progargs);
      perror ("execvp error");
      exit (1);
    }

  close (pipefd[0]);
  while ((c = getchar ()) != EOF)
    {

      write (1, &c, sizeof (c));
      write (pipefd[1], &c, sizeof (c));
    }
  close (pipefd[1]);
  return 0;
}
