/** cp1.c
 *    version 1 of cp - uses read and write with tunable buffer size
 *
 *    usage cp1 src dest
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>

#define BUFFERSIZE 4096
#define COPYMODE  0644

void oops (char *, char *);

int
main (int ac, char *av[])
{
  int in_fd, out_fd, n_chars;
  char buf[BUFFERSIZE];

  //check args
  if (ac != 3)
    {
      fprintf (stderr, "usage: %s source destination\n", *av);
      exit (1);
    }

  //get src file info
  struct stat fileinfo;
  if (0 > stat (av[1], &fileinfo))
    {
      perror ("cannot stat");
      exit (1);
    }

  //open files
  if ((in_fd = open (av[1], O_RDONLY)) == -1)
    oops ("Cannot open ", av[1]);
  if ((out_fd = creat (av[2], COPYMODE)) == -1)
    oops ("Cannot creat", av[2]);

  //copy files
  while ((n_chars = read (in_fd, buf, BUFFERSIZE)) > 0)
    if (write (out_fd, buf, n_chars) != n_chars)
      oops ("Write error to ", av[2]);
  if (n_chars == -1)
    oops ("Read error from ", av[1]);

  //close files
  if (close (in_fd) == -1 || close (out_fd) == -1)
    oops ("Error closing files", "");

  //restore the src file access time
  struct utimbuf restore_time;
  restore_time.actime = fileinfo.st_atime;
  restore_time.modtime = fileinfo.st_mtime;
  if (0 > utime (av[1], &restore_time))
    perror ("srcutime error");

  //change the dst file time stamp
  if (0 > utime (av[2], &restore_time))
    perror ("dst utime error");

  return 0;
}

void
oops (char *s1, char *s2)
{
  fprintf (stderr, "Error: %s ", s1);
  perror (s2);
  exit (1);
}
