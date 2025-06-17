#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "set_ticker.c"

#define MAX_PID_NUM 10
pid_t pid[MAX_PID_NUM];
int pidnum;
void sighandler (int);

int
main (int argc, char *argv[])
{

  if (argc < 3)
    {
      printf ("Usage: %s <interval milisec> <pid1> <pid2> ...\n", argv[0]);
      return -1;
    }

  if (argc > 12)
    {
      printf ("Signal should be less than %d", MAX_PID_NUM);
      return -1;
    }

  int span = atoi (argv[1]);
  int i;
  for (i = 2; i < argc; i++)
    pid[i - 2] = atoi (argv[i]);
  pidnum = argc - 2;
  signal (SIGALRM, sighandler);
  set_ticker (span);
  while(1);
  return 0;
}

void
sighandler (int sig)
{
  int i;
  for (i = 0; i < pidnum; i++)
    kill (pid[i], SIGINT);
}
