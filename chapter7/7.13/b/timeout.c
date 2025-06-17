#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include "set_ticker.c"

void sighandler (int);

int pidtokill;
char *ptyfile;

int
main (int argc, char *argv[])
{

  if (4 != argc)
    {
      printf ("Usage: %s <pid> <sleep miliseconds> <pty file>\n", argv[0]);
      return 1;
    }

  pidtokill = atoi (argv[1]);
  int sleepmilisec = atoi (argv[2]);
  ptyfile = argv[3];

  signal (SIGALRM, sighandler);
  set_ticker (sleepmilisec);

  pause ();
  return 0;
}

void
sighandler (int sig)
{
  struct stat fileinfo;
  if (0 > stat (ptyfile, &fileinfo))
    {
      perror ("stat");
      return;
    }

  time_t now = time (NULL);
  int expire_time = 10;
  if (now - fileinfo.st_mtime < expire_time)
    {
      printf ("file is busy in the before %d secs, no kill!\n",
	      expire_time);
      return;
    }

  if (0 > kill (pidtokill, SIGKILL))
    {
      perror ("kill()");
    }
  return;
}
