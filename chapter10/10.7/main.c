#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int
main ()
{
  close (0);
  open ("/dev/tty", 0);
  printf ("hello world\n");
  return 0;
}
