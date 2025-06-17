#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>

char *filename = "data";
char *lock_filename = "data.LCK";

int
lock_file ()
{
  int rv = 0;
  if (link (filename, lock_filename) == -1)
    rv = (errno == EEXIST ? 1 : 2);
  return rv;
}

int
main ()
{
  int rc = lock_file ();
  if (1 == rc)
    {
      puts ("wait 1 s for unlink the file");
      sleep (1);
    }
  else if (2 == rc)
    {
      perror (" link file ");
      exit (1);
    }


  if (1 == rc)
    rc = lock_file ();

  if (0 != rc)
    {
      perror ("retry link file");
      exit (1);
    }

  int fd = open (filename, O_APPEND | O_WRONLY);
  if (0 > fd)
    {
      perror (" open file ");
      unlink (lock_filename);
      exit (1);
    }
  char buf[] = " Where is my Mrs.Right ? \n ";
  if (0 > write (fd, buf, sizeof (buf)))
    {
      perror (" write file ");
      unlink (lock_filename);
      exit (1);
    }

  unlink (lock_filename);
  return 0;
}
