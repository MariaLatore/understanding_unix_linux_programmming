#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
void
error_handling (char *errmsg)
{
  perror (errmsg);
  exit (1);
}

void
oops (char *s1, char *s2)
{
  fprintf (stderr, "Error: %s ", s1);
  perror (s2);
  exit (1);
}

int copy1 (int ac, char *av[]);

int
main ()
{
  int fd;
  if (0 > mkdir ("demodir", ACCESSPERMS))
    error_handling ("mk demodir error");

  if (0 > (fd = creat ("demodir/y", ACCESSPERMS)))
    error_handling ("create y fail");
  else
    close (fd);

  if (0 > mkdir ("demodir/a", ACCESSPERMS))
    error_handling ("mkdir a error");

  if (0 > mkdir ("demodir/c", ACCESSPERMS))
    error_handling ("mkdir c error");

  if (0 > (fd = creat ("demodir/a/x", ACCESSPERMS)))
    error_handling ("create file x error");
  else
    close (fd);


  if (0 > mkdir ("demodir/c/d1", ACCESSPERMS))
    error_handling ("mkdir d1 error");

  if (0 > mkdir ("demodir/c/d2", ACCESSPERMS))
    error_handling ("mkdir d2 error");

  char *copy_arg[] = {
    "copy1",
    "demodir/a/x",
    "demodir/c/d2/xcopy"
  };
  if (0 > copy1 (3, copy_arg))
    error_handling ("cp x to xcopy error");

  if (0 > link ("demodir/a/x", "demodir/c/d1/xlink"))
    error_handling ("ln x to xlink error");

  return 0;
}

/** cp1.c
 *    version 1 of cp - uses read and write with tunable buffer size
 *
 *    usage cp1 src dest
 */

#define BUFFERSIZE 4096
#define COPYMODE  0644

int
copy1 (int ac, char *av[])
{
  int in_fd, out_fd, n_chars;
  char buf[BUFFERSIZE];
  //check args
  if (ac != 3)
    {
      fprintf (stderr, "usage: %s source destination\n", *av);
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
  return 0;
}
