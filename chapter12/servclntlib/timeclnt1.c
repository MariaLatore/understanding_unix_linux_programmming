#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "socklib.h"

void talk_with_server (int fd);

int
main ()
{
  int fd;
  fd = connect_to_server ("localhost", 15000);
  if (fd == -1)
    exit (1);
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
