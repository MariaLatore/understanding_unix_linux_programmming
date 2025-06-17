#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
char filename[] = "data";
int
main ()
{
  int fd = open (filename, O_RDONLY);
  if (fd < 0)
    {
      perror ("open file");
      return 1;
    }
  int s = fcntl (fd, F_GETFL);
  printf ("before,flag %d\n", s);
  s |= O_SYNC;
  printf ("after, flag %d\n", s);

  int result = fcntl (fd, F_SETFL, s);
  if (result == -1)
    perror ("setting SYNC");

  if (close (fd) < 0)
    perror ("close fd");
  return 0;
}
