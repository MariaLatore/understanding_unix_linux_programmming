#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>

int
main ()
{
  int fd = open ("/dev/fd/0", O_RDWR);
  if (0 > fd)
    {
      perror ("open /dev/fd/0");
      exit (1);
    }
  struct termios settings;
  int result = tcgetattr (fd, &settings);
  if (0 > result)
    {
      perror ("get flag");
      exit (1);
    }
  settings.c_oflag |= OLCUC;
  result = tcsetattr (fd, TCSANOW, &settings);
  if (0 > result)
    {
      perror ("set flag");
      exit (1);
    }

  memset (&settings, 0, sizeof (settings));
  result = tcgetattr (fd, &settings);
  if (0 > result)
    {
      perror ("get flag again");
      exit (1);
    }
  printf ("get flag %d\n", settings.c_oflag & OLCUC);
  close (fd);
  return 0;
}
