/* sigdemo1.c - shows hos a signal handler works.
 *            - run this and press Ctrl-C a few times
 */
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
static long max_handle_time = 0;

int
main (int argc, char *argv[])
{
  if (2 != argc)
    {
      printf ("Usage: %s <max CtrlC times>\n", argv[0]);
      return 2;
    }
  max_handle_time = strtol (argv[1], NULL, 0);
  void f (int);			//declare the handler
  int i;
  signal (SIGINT, f);		//install the handler
  for (i = 0; i < 5; i++)
    {				//do something else
      printf ("hello\n");
      sleep (1);
    }
  return 0;
}

void
f (int signum)			//this function is called
{
  static int num = 0;
  static char ouch[] = { 'O', 'U', 'C', 'H' };
  int i;
  num++;
  for (i = 0; i < sizeof (ouch); i++)
    putchar (ouch[i]);
  for (i = 0; i < num; i++)
    putchar ('!');
  putchar ('\n');
  if (num >= max_handle_time)
    exit (0);
}
