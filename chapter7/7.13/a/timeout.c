#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "set_ticker.c"

void sighandler (int);

int pidtokill;

int
main (int argc, char *argv[])
{

  if (3 != argc)
    {
      printf ("Usage: %s <pid> <sleep miliseconds>\n", argv[0]);
      return 1;
    }

  pidtokill = atoi (argv[1]);
  int sleepmilisec = atoi (argv[2]);

  signal (SIGALRM, sighandler);
  set_ticker (sleepmilisec);

  pause ();
  return 0;
}

void
sighandler (int sig)
{

  if (0 > kill (pidtokill, SIGKILL))
    {
      perror ("kill()");
    }
  return;
}
