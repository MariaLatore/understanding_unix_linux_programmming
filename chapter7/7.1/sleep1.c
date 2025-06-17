/* sleep1.c
 *     purpose show how sleep works
 *     usage sleep1
 *     outline sets handler, sets alarm, pauses, then return
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define SHHHH

int
main ()
{
  void wakeup (int);
  void sigint_handle (int);

  printf ("about to sleep for 4 seconds\n");
  signal (SIGALRM, wakeup);	//catch it
  signal (SIGINT, sigint_handle);
  alarm (4);			//set clock
  pause ();			//freeze here
  printf ("Morning so soon?\n");	//back to work
  return 0;
}

void
wakeup (int signum)
{
#ifndef SHHHH
  printf ("Alarm received from kernel\n");
#endif
}

void
sigint_handle (int signum)
{
  puts ("Receive SIGINT from kernel\n");
}
