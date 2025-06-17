#include<stdio.h>
#include<fcntl.h>		//for fcntl,open
#include<unistd.h>		//for close
#include<stdlib.h>		//for malloc
#include<string.h>		//for memset

int
main (int ac, char *av[])
{

  if (ac != 2)
    {
      printf ("Usage:%s <on/off>\n", av[0]);
      return 1;
    }

  int flag;
  if (strcmp (av[1], "on") == 0)
    flag = O_WRONLY | O_SYNC;
  else if (strcmp (av[1], "off") == 0)
    flag = O_WRONLY;
  else
    {
      printf ("Usage:%s <on/off>\n", av[0]);
      return 1;
    }
  int fd = open ("data", flag);	//O_WRONLY requires the file to exist
  if (0 > fd)
    {
      perror ("open failed");
      return 1;
    }

  int s;
  s = fcntl (fd, F_GETFL);
  printf ("original flag %d\n", s);

  const long int bufsz = 800000000;
  char *buf = (char *) malloc (bufsz);
  long int writtensz = 0;
  long int new_written = 0;
  int piece = 409600;
  memset (buf, 'c', bufsz);
  while (bufsz > writtensz)
    {
      new_written = write (fd, buf + writtensz, piece);
      if (new_written <= 0)
	{
	  printf ("written %ld bufsz %ld\n", writtensz, bufsz);
	  break;
	}
      else
	writtensz += new_written;

    }

  close (fd);
  return 0;
}
