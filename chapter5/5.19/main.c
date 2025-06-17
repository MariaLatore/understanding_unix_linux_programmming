#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int
main ()
{
  int fd = open ("/dev/pts/0", O_RDWR);
  int s = fcntl (fd, F_GETFL);
  char rec[] = "i love beauty!\n";
  int result;

  s |= O_APPEND;
  result = fcntl (fd, F_SETFL, s);
  if (result == -1)
    perror ("setting APPEND");
  else
    write (fd, rec, sizeof (rec));
  return 0;
}
