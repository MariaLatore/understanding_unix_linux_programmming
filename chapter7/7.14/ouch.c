/* sigdemo1.c - shows hos a signal handler works.
 *            - run this and press Ctrl-C a few times
 */
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
static long max_handle_time = 0;
static char *message;

int
main (int argc, char *argv[])
{
  if (3 != argc)
    {
      printf ("Usage: %s <message string>  <max CtrlC times>\n", argv[0]);
      return 2;
    }
  max_handle_time = strtol (argv[2], NULL, 0);
  message = argv[1];
  void f (int);			//declare the handler
  signal (SIGINT, f);		//install the handler
  while (1);
  return 0;
}

void
f (int signum)			//this function is called
{
  static int num = 0;
  num++;
  if (num >= max_handle_time)
    {
      printf ("%s\n", message);
      num = 0;
    }
}
