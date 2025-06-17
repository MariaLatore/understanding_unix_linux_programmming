/* waitdemo2.c - shows how parent gets child status
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#define DELAY 5

static int childcnt = 0;;

void parent_code (int childpid);
void child_code (int delay);
void
sighandler (int signum)
{
  static int sighandlecnt = 0;
  parent_code (signum);
  sighandlecnt++;
  if (sighandlecnt == childcnt)
    exit (1);
  else
    return;
}


int
main (int argc, char *argv[])
{
  int newpid;
  if (2 != argc)
    {
      printf ("Usage:%s <child pid count>\n", argv[0]);
      return 0;
    }

  childcnt = atoi (argv[1]);
  if (0 >= childcnt)
    {
      printf ("warning:input child count: %d\n", childcnt);
      childcnt = 1;
    }
  signal (SIGCHLD, sighandler);
  printf ("before: mypid is %d\n", getpid ());
  int i;
  for (i = 0; i < childcnt; i++)
    {
      if ((newpid = fork ()) == -1)
	perror ("fork");
      else if (newpid == 0)
	child_code (DELAY);
    }
  while (1)
    {
      printf ("waiting..\n");
      sleep (1);
    }
  return 0;
}

/*
 * new process takes a nap and then exits
 */
void
child_code (int delay)
{
  printf ("child %d here. will sleep for %d seconds\n", getpid (), delay);
  sleep (delay);
  printf ("child done, about to exit\n");
  exit (17);
}

/*
 * parent waits for child then prints a message
 */
void
parent_code (int childpid)
{
  int wait_rv;			//return value from wait()
  int child_status;
  int high_8, low_7, bit_7;

  wait_rv = wait (&child_status);
  printf ("done waiting for %d. Wait returned: %d\n", childpid, wait_rv);

  high_8 = child_status >> 8;	//1111 1111 0000 0000
  low_7 = child_status & 0x7F;	//000 0000 0111 1111
  bit_7 = child_status & 0x80;	//0000 000 1000 0000
  printf ("status: exit=%d, sig=%d, core=%d\n", high_8, low_7, bit_7);
}
