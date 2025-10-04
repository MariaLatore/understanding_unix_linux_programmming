#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "mutual.h"


int
main ()
{
  int fd;
  pid_t pid;
  if (mkfifo (FIFONAME, 0644) != 0)
    oops ("mkfifo", 1);

  while (1)
    {
      if ((fd = open (FIFONAME, O_WRONLY)) == -1)
	oops ("open", 2);
      pid = fork ();
      switch (pid)
	{
	case -1:
	  oops ("fork", 4);
	case 0:
	  if (dup2 (fd, 1) < 0)
	    oops ("dup2", 3);
	  close (fd);
	  execlp ("date", "date", NULL);
	  oops ("could not be here", 4);
	default:
	  wait (NULL);
	  close (fd);
	  continue;
	}
    }

  close (fd);
  return 0;
}
