#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "socklib.c"

void process_request (int fd);

int
main ()
{
  int sock, fd;			//socket and connection
  int port = 15000;
  sock = make_server_socket (port);
  if (sock == -1)
    exit (1);

  while (1)
    {
      fd = accept (sock, NULL, NULL);	//take next call
      if (fd == -1)
	{
	  if (EINTR == errno)
	    continue;
	  break;		//or die
	}
      process_request (fd);	//chat with client
      close (fd);		//hang up when done
    }
  return 0;
}

void
process_request (int fd)
{
  int pid = fork ();
  switch (pid)
    {
    case -1:
      return;			//cannot proide service
    case 0:
      dup2 (fd, 1);		//child runs date
      close (fd);
      execl ("/bin/date", "date", NULL);
      puts ("execlp error");
      exit (3);			//or quits
    default:
      wait (NULL);		//parent wait for child
    }
}
