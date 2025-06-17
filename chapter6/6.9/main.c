/* sigdemo2.c - shows how to ignore a signal
 *            - press Ctrl-\ to kill this one
 */

#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<termios.h>

void
tty_mode (int how)
{
  static struct termios ttystate;
  if (0 == how)
    tcgetattr (0, &ttystate);
  else if (1 == how)
    tcsetattr (0, TCSANOW, &ttystate);
}

int
main ()
{
  struct termios ttystate;
  tty_mode (0);
  tcgetattr (0, &ttystate);
  ttystate.c_lflag &= ~ISIG;
  tcsetattr (0, TCSANOW, &ttystate);
  puts ("you can't stop me!");
  while (1)
    {
      sleep (1);
      puts ("haha");
    }
  tty_mode (1);
}
