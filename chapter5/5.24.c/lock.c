#include <stdio.h>
#include<errno.h>
#include<unistd.h>
#include "filename.h"

int
lock ()
{
  int rv = 0;
  if (link (filename, lock_filename) == -1)
    rv = (errno == EEXIST ? 1 : 2);
  if (rv != 0)
    perror ("link file");
  return rv;
}

int
main ()
{
  return lock ();
}
