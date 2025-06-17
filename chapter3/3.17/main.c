#include<stdio.h>
#include<err.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <filename>", argv[0]);
      exit (1);
    }

  int fd;
  if (0 > (fd = open (argv[1], O_RDONLY)))
    {
      perror ("cannot open file");
      err (1, "cannot open file %s", argv[1]);
      exit (1);
    }
  else
    printf ("the fd is %d\n", fd);

  char buf[256];
  if (-1 == read (fd, buf, 4))
    {
      perror ("cannot read file");
      exit (1);
    }
  else
    {
      buf[4] = 0;
      printf ("read 4 bytes:%s\n", buf);
    }

  sleep (20);

  if (-1 == read (fd, buf, 4))
    {
      perror ("cannot read file");
      exit (1);
    }
  else
    {
      buf[4] = 0;
      printf ("read anothre 4 bytes:%s\n", buf);
    }
  return 0;
}
