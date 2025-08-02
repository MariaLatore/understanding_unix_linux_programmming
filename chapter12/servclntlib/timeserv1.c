#include <sys/socket.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "socklib.h"
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
	break;			//or die
      process_request (fd);	//chat with client
      close (fd);		//hang up when done
    }
  return 0;
}

void
process_request (int fd)
{
  time_t now;
  char *cp;

  time (&now);
  cp = ctime (&now);
  write (fd, cp, strlen (cp));
}
