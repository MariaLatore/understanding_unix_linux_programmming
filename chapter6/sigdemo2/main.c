/* sigdemo2.c - shows how to ignore a signal
 *            - press Ctrl-\ to kill this one
 */

#include<stdio.h>
#include<signal.h>
#include<unistd.h>

int
main ()
{
  signal (SIGINT, SIG_IGN);
  puts ("you can't stop me!");
  while (1)
    {
      sleep (1);
      puts ("haha");
    }
}
