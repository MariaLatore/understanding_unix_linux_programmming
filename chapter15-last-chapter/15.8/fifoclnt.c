#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "mutual.h"

int
main ()
{
  int fd = open (FIFONAME, O_RDONLY);
  char buf[BUFSIZ] = { 0 };
  if (0 > fd)
    oops ("clnt open", 1);

  if (0 > read (fd, buf, BUFSIZ))
    oops ("clnt read", 2);

  close (fd);


  printf ("time: %s\n", buf);
  return 0;
}
