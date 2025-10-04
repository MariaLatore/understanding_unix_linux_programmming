/* selectdemo.c: watch for input on two devices AND timeout
 *        usage: selectdemo dev1 dev2 timeout
 *       action: reports on input from each file, and reports timeouts
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define oops(m,x) { perror(m); exit(x); }
void showdata (char *fname, int fd);
void dump (struct pollfd *fds);


int
main (int ac, char *av[])
{
  int fd1, fd2;			//the fds to watch
  int timeout;			//how long to wait
  int retval;			//return from select
  struct pollfd fds[2];
//lib/x86_64-redhat-linux6E/include/sys/poll.h:typedef unsigned long int nfds_t;
  nfds_t nfds = 2;

  if (ac != 4)
    {
      fprintf (stderr, "usage: %s file file timeout", av[0]);
      exit (1);
    }
  timeout = atoi (av[3]) * 1000;	//miliseconds

/* open files */
  if ((fd1 = open (av[1], O_RDONLY)) == -1)
    oops (av[1], 2);
  if ((fd2 = open (av[2], O_RDONLY)) == -1)
    oops (av[2], 3);

  fds[0].fd = fd1;
  fds[0].events = POLLIN;
  fds[1].fd = fd2;
  fds[1].events = POLLIN;

  while (1)
    {

      fds[0].revents = 0;
      fds[1].revents = 0;

      retval = poll (fds, nfds, timeout);
      if (retval == -1)
	oops ("select", 4);
      if (retval > 0)
	{

//check bits for each fd
	  if (fds[0].revents == POLLIN)
	    showdata (av[1], fd1);
	  if (fds[1].revents == POLLIN)
	    showdata (av[2], fd2);
	}
      else
	printf ("no input after %d secondes\n", atoi (av[3]));
    }
  return 0;
}

void
showdata (char *fname, int fd)
{
  char buf[BUFSIZ];
  int n;
  printf ("%s:", fname);
  fflush (stdout);
  n = read (fd, buf, BUFSIZ);
  if (n == -1)
    oops (fname, 5);
  write (1, buf, n);
  write (1, "\n", 1);
}


void
dump (struct pollfd *fds)
{
  printf
    ("after: fds[0].fd=%d, fds[0].events=0x%x, fds[0].revents=0x%x\nfds[1].fd=%d, fds[1].events=0x%x, fds[1].revents=0x%x\n",
     fds[0].fd, fds[0].events, fds[0].revents, fds[1].fd,
     fds[1].events, fds[1].revents);
}
