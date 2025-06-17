/* sigdemo1.c - shows hos a signal handler works.
 *            - run this and press Ctrl-C a few times
 */
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>

int
main ()
{
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
  char reply;
  printf ("Interrupted! OK to quit (y/n)?");
  //fflush(stdout);
  reply = getchar ();
  if ('y' == reply)
    exit (0);
  else
    printf ("input: %c(%d)\n", reply, reply);
}
