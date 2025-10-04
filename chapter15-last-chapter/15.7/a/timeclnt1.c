#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "dgram.h"
#include "mutual.h"

void talk_with_server (int fd);

int
main ()
{
  int fd;
  char hostname[BUFSIZ];
  struct sockaddr_in servaddr;
  gethostname (hostname, BUFSIZ);
  char request[] = "TIME";

  fd = make_dgram_client_socket ();
  if (fd == -1)
    exit (1);
  if (0 > make_internet_address (hostname, PORTNUM, &servaddr))
    {
      fprintf (stderr, "make internet address error\n");
      exit (3);
    }
  printf ("TIME CLIENT: host name %s\n", hostname);
  connect (fd, (struct sockaddr *) &servaddr, sizeof (servaddr));
  if ((write (fd, request, sizeof (request))) != sizeof (request))
    {
      fprintf (stderr, "sendto error\n");
      exit (2);
    }
  talk_with_server (fd);
  close (fd);
  return 0;
}

void
talk_with_server (int fd)
{
  char buf[BUFSIZ];
  int n;
  n = read (fd, buf, BUFSIZ);
  write (1, buf, n);
}
