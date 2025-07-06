#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define oops(msg,n) {perror(msg); exit(n);}

int
main ()
{
  int tosort[2];
  int fromsort[2];
  if (-1 == pipe (tosort) || -1 == pipe (fromsort))
    oops ("pipe", -1);

  pid_t pid;

  if ((pid = fork ()) == -1)
    oops ("fork", -2);

  if (pid == 0)
    {
      close (tosort[1]);
      close (fromsort[0]);
      if (dup2 (fromsort[1], 1) == -1)
	oops ("dup2", -3);
      if (dup2 (tosort[0], 0) == -1)
	oops ("dup2", -4);
      close (fromsort[1]);
      close (tosort[0]);
      execlp ("sort", "sort", NULL);
      oops ("Cannot run sort!", -5);
    }

  close (tosort[0]);
  close (fromsort[1]);

  FILE *fdout = fdopen (tosort[1], "w");
  FILE *fdin = fdopen (fromsort[0], "r");

  char buf[BUFSIZ] = { 0 };
  while (fgets (buf, BUFSIZ, stdin) != NULL)
    fputs (buf, fdout);

  fclose (fdout);

  printf ("#############\nsort result\n#############\n");

  while (fgets (buf, BUFSIZ, fdin) != NULL)
    fputs (buf, stdout);
  fclose (fdin);
  wait (NULL);
  return 0;
}
