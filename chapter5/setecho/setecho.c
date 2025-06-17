/*setecho.c
 *   usage: setecho [y|n]
 *   shows: how to read change, reset tty attributes
 */
#include<stdio.h>
#include<termios.h>
#include<stdlib.h>

#define oops(s,x) { perror(s); exit(x); }

int
main (int ac, char *av[])
{
  struct termios info;
  if (1 == ac)
    exit (0);

  if (-1 == tcgetattr (0, &info))	//get attribs
    oops ("tcgetattr", 1);

  if ('y' == av[1][0])
    info.c_lflag |= ECHO;	//turn on bit
  else
    info.c_lflag &= ~ECHO;	//turn off bit

  if (-1 == tcsetattr (0, TCSANOW, &info))	//set attribs
    oops ("tcsetattr", 2);
  return 0;
}
