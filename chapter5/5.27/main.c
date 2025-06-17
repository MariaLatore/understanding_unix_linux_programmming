#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
char filename[] = "data";

void
turn_on (int fd)
{
  int s = fcntl (fd, F_GETFL);
  printf ("before,flag %d\n", s);
  s |= O_SYNC;
  printf ("after, flag %d\n", s);

  int result = fcntl (fd, F_SETFL, s);
  if (result == -1)
    perror ("setting SYNC");
}

void
turn_off (int fd)
{
  int s = fcntl (fd, F_GETFL);
  printf ("before,flag %d\n", s);
  s &= (~O_SYNC);
  printf ("after, flag %d\n", s);

  int result = fcntl (fd, F_SETFL, s);
  if (result == -1)
    perror ("setting SYNC");
}

int
main ()
{
  int fd = open (filename, O_RDWR|O_SYNC);
  if (fd < 0)
    {
      perror ("open file");
      return 1;
    }
  turn_on (fd);
  turn_off (fd);

  if (close (fd) < 0)
    perror ("close fd");
  return 0;
}
