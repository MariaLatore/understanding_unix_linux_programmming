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

#define BUFFERSIZE 4096
#define COPYMODE  0644

typedef enum
{
  FILE_TYPE_NORMAL,
  FILE_TYPE_DIR,
} file_type_t;

void oops (char *, char *);

file_type_t
cp_dest_filetype (char *filename)
{
  struct stat fileinfo = { 0 };
  if (-1 == stat (filename, &fileinfo))
    {
      return FILE_TYPE_NORMAL;
    }

  if (S_ISDIR (fileinfo.st_mode))
    return FILE_TYPE_DIR;

  return FILE_TYPE_NORMAL;
}

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

  // judge the file type
  char dest_filename[256];
  file_type_t dst_type = cp_dest_filetype (av[2]);
  if (FILE_TYPE_NORMAL == dst_type)
    snprintf (dest_filename, sizeof (dest_filename), "%s", av[2]);
  else if (FILE_TYPE_DIR == dst_type)
    snprintf (dest_filename, sizeof (dest_filename), "%s/%s", av[2], av[1]);
  av[2] = dest_filename;

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

void
oops (char *s1, char *s2)
{
  fprintf (stderr, "Error: %s ", s1);
  perror (s2);
  exit (1);
}
