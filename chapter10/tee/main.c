#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      printf ("Usage: %s <filename>\n", argv[0]);
      exit (1);
    }

  char *filename = argv[1];
  char c;
  int fd;

  fd = open (filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);


  while ((c = getchar ()) != EOF)
    {
      write (1, &c, sizeof (c));
      write (fd, &c, sizeof (c));
    }
  close (fd);
  return 0;
}
