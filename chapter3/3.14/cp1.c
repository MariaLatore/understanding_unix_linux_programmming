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
#include <dirent.h>

#define BUFFERSIZE 4096
#define COPYMODE  0644

typedef enum
{
  FILE_TYPE_NORMAL,
  FILE_TYPE_DIR,
} file_type_t;

void oops (char *, char *);

file_type_t
cp_filetype (char *filename)
{
  struct stat fileinfo = { 0 };
  if (-1 == stat (filename, &fileinfo))
    {
      return FILE_TYPE_NORMAL;	//file does not exist, we treat them as regular
    }

  if (S_ISDIR (fileinfo.st_mode))
    return FILE_TYPE_DIR;

  return FILE_TYPE_NORMAL;
}

int
do_cp (char *srcfile, char *dstfile)
{
  int in_fd, out_fd, n_chars;
  char buf[BUFFERSIZE];

  //open files
  if ((in_fd = open (srcfile, O_RDONLY)) == -1)
    oops ("Cannot open ", srcfile);
  if ((out_fd = creat (dstfile, COPYMODE)) == -1)
    oops ("Cannot creat", dstfile);

  //copy files
  while ((n_chars = read (in_fd, buf, BUFFERSIZE)) > 0)
    if (write (out_fd, buf, n_chars) != n_chars)
      oops ("Write error to ", dstfile);
  if (n_chars == -1)
    oops ("Read error from ", srcfile);

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

int
main (int ac, char *av[])
{
  //check args
  if (ac != 3)
    {
      fprintf (stderr, "usage: %s source destination\n", *av);
      exit (1);
    }
  file_type_t srctype = cp_filetype (av[1]);
  file_type_t dsttype = cp_filetype (av[2]);
  if (FILE_TYPE_DIR != srctype)
    return do_cp (av[1], av[2]);

  if (FILE_TYPE_DIR != dsttype)
    oops ("dst is not dir, but src is dir", NULL);

  DIR *dirp;
  struct dirent *direntp;
  char srcfile_completename[256];
  char dstfile_completename[256];

  if (NULL == (dirp = opendir (av[1])))
    {
      printf ("cannot open src dir %s", av[1]);
      perror ("open src dir");
      exit (1);
    }

  while (NULL != (direntp = readdir (dirp)))
    {
      snprintf (srcfile_completename,
		sizeof (srcfile_completename), "%s/%s", av[1],
		direntp->d_name);

      //omit the dirs in srcdir
      struct stat fileinfo;
      if (0 != stat (srcfile_completename, &fileinfo))
	oops ("open src file error", srcfile_completename);
      if (S_ISDIR (fileinfo.st_mode))
	continue;

      snprintf (dstfile_completename,
		sizeof (dstfile_completename), "%s/%s", av[2],
		direntp->d_name);

      do_cp (srcfile_completename, dstfile_completename);
    }

  return 0;
}
