/* rotate.c: map a->b, b->c, .., z->a
 *   purpose: useful for showing tty modes
 */
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>


void
tty_mode (int how)
{
  static struct termios original_mode = { 0 };
  if (how == 0)
    tcgetattr (0, &original_mode);
  else
    tcsetattr (0, TCSANOW, &original_mode);
}

void
set_cr_no_echo_mode ()
{
  struct termios ttystate;
  tcgetattr (0, &ttystate);	//read curr, setting
  ttystate.c_lflag &= ~ICANON;	//no buffering
  ttystate.c_lflag &= ~ECHO;	//no echo either
  ttystate.c_cc[VMIN] = 1;	//get 1 char at a time
  tcsetattr (0, TCSANOW, &ttystate);	//install settings
}

int
main ()
{
  int c;
  tty_mode (0);
  set_cr_no_echo_mode ();
  signal (SIGINT, SIG_IGN);
  signal (SIGQUIT, SIG_IGN);
  static int total_chr_num = 0;
  while ((c = getchar ()) != EOF)
    {
      if (c == 'q')
	break;
      if (c == 'z')
	{
	  total_chr_num++;
	  putchar ('a');
	  continue;
	}

      if (islower (c))
	{
	  total_chr_num++;
	  putchar (++c);
	  continue;
	}

      if (c == 127 /*backspace */ )
	{
	  putchar ('\b');
	  putchar (' ');
	  putchar ('\b');
	  total_chr_num--;
	  continue;
	}

      if (c == 11 /*Ctrl+k */ )
	{
	  putchar ('\r');
	  while (total_chr_num > 0)
	    {
	      putchar (' ');
	      total_chr_num--;
	    }
	  putchar ('\r');
	  continue;
	}

      putchar (c);
      total_chr_num++;
    }
  tty_mode (1);
  return 0;
}
