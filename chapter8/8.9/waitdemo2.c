/* waitdemo2.c - shows how parent gets child status
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#define DELAY 5

void parent_code (int childpid);
void child_code (int delay);
unsigned int
generate_unsigned_int_random ()
{
  int fd;
  unsigned int random_num;

  fd = open ("/dev/urandom", O_RDONLY);
  if (fd == -1)
    {
      perror ("/dev/urandom open error");
      return 0;
    }

  if (read (fd, &random_num, sizeof (unsigned int)) != sizeof (unsigned int))
    {
      perror ("/dev/urandom read error");
      close (fd);
      return 0;
    }

  close (fd);

  return random_num;
}


void
sighandler (int signum)
{
  parent_code (signum);
  exit (1);
}

int
main ()
{
  int newpid;
  signal (SIGCHLD, sighandler);
  printf ("before: mypid is %d\n", getpid ());
  if ((newpid = fork ()) == -1)
    perror ("fork");
  else if (newpid == 0)
    child_code (generate_unsigned_int_random () % 5);
  else
    {
      while (1)
	{
	  printf ("waiting..\n");
	  sleep (1);
	}
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
