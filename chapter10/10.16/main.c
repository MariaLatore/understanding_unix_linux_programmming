#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>

int
main (int argc, char *argv[])
{

  if (2 != argc)
    {
      printf ("usage: %s <fd(file descriptor)>\n", argv[0]);
      exit (1);
    }

  struct stat fileinfo;
  long int fd = strtol (argv[1], NULL, 0);


  if (0 != fstat (fd, &fileinfo))
    {
      perror ("fstat error");
      exit (1);
    }

  if (S_ISREG (fileinfo.st_mode))

    printf ("fd %ld is a regular file\n", fd);
  else

    printf ("fd %ld is not a regular file\n", fd);

  return 0;
}
