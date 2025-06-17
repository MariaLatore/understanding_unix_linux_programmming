/* waitdemo1.c - shows how parent pauses until child finished
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#define DELAY 2

void child_code (int delay);
void parent_code ();

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


int
main (int argc, char *argv[])
{
  int newpid;
  if (2 != argc)
    {
      printf ("Usage:%s <pid counts>\n", argv[0]);
      return 1;
    }

  int pid_count = atoi (argv[1]);
  if (0 >= pid_count)
    {
      printf ("warning:pid count: %d\n", pid_count);
      pid_count = 1;
    }

  printf ("before: my pid is %d\n", getpid ());
  int i;
  for (i = 0; i < pid_count; i++)
    {
      if ((newpid = fork ()) == -1)
	perror ("fork");
      else if (newpid == 0)
	//child will call exit() in the child_code(),
	//so child never go into next loop
	child_code (generate_unsigned_int_random () % 5);
    }

  for (i = 0; i < pid_count; i++)
    parent_code (0);

  return 0;
}

/*
 * new process takes a nap and then exits
 */
void
child_code (int delay)
{
  printf ("child %d here, will sleep for %d seconds\n", getpid (), delay);
  sleep (delay);
  printf ("child done, about to exit\n");
  exit (17);
}

/*
 * parent waits for child then prints a message
 */
void
parent_code ()
{
  int wait_rv;			//return value from wait()
  int status = -1;
  wait_rv = wait (&status);
  if (0 >= wait_rv)
    perror ("wait");
  else
    printf ("done waiting for %d. Wait returned:0x%x \n", wait_rv, status);
}
