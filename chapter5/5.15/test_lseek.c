#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
int
main (int argc, char *argv[])
{
  if (2 != argc)
    {
      printf ("Usage %s <filetotest>\n", argv[0]);
      exit (1);
    }
  int fd = open (argv[1], O_RDONLY);
  if (0 > fd)
    {
      perror ("open");
      exit (1);
    }

  if (0 > lseek (fd, 0, SEEK_END))
    {
      perror ("lseek");
      exit (1);
    }

  close (fd);
  return 0;
}
