/** cp1.c
 *    version 1 of cp - uses read and write with tunable buffer size
 *
 *    usage cp1 src dest
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 4096
#define COPYMODE  0644

void oops(char *, char *);

int main(int ac, char *av[])
{
  int in_fd, out_fd, n_chars;
  char buf[BUFFERSIZE];
  char reply=0;
  int in_para_idx=-1, out_para_idx=-1;

  //check args
  if(ac > 4 || (4 == ac && 0 != strcmp(av[1],"-i")))
  {
    fprintf(stderr, "usage: %s [-i] source destination\n", *av);
    exit(1);
  }
  else if(ac == 4)
  {
    in_para_idx = 2;
    out_para_idx = 3;

    out_fd = open(av[out_para_idx], O_RDONLY);
    if(0 < out_fd)
    {
        printf("dest fils %s exists, sure to copy?(y/n): ", av[out_para_idx]);
	reply= getchar();
        if('y' != reply)
	{
		close(out_fd);
 		return 0;
	}
	else
		close(out_fd);
    }
  }
  else
  {
    in_para_idx = 1;
    out_para_idx = 2;
  }

  //open files
  if((in_fd=open(av[in_para_idx], O_RDONLY)) == -1)
    oops("Cannot open ", av[in_para_idx]);
  if((out_fd=creat(av[out_para_idx], COPYMODE)) == -1)
    oops("Cannot creat", av[out_para_idx]);

  //copy files
  while((n_chars=read(in_fd, buf, BUFFERSIZE)) > 0)
    if(write(out_fd, buf, n_chars) != n_chars)
      oops("Write error to ", av[out_para_idx]);
  if(n_chars==-1)
    oops("Read error from ", av[in_para_idx]);

  //close files
  if(close(in_fd) == -1 || close(out_fd == -1))
     oops("Error closing files","");

     return 0;
}

void oops(char *s1, char *s2)
{
  fprintf(stderr, "Error: %s ", s1);
  perror(s2);
  exit(1);
}
