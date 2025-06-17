#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<termios.h>
#include<unistd.h>
#include<sys/ioctl.h>

static char fill_char = 'A';


void
fillsc (char c)
{
  struct winsize w = { 0 };
  int i;
  char clrsc_cmd[] = "\033[2J\033[H";
  if (0 > ioctl (STDOUT_FILENO, TIOCGWINSZ, &w))
    {
      perror ("ioctl() error");
      exit (1);
    }
  write (STDOUT_FILENO, clrsc_cmd, sizeof (clrsc_cmd));
  for (i = 0; i < w.ws_row * w.ws_col; i++)
    putchar (c);
  fflush(stdout);//this is important
}


void
handsig (int signum)
{
  fillsc (fill_char);
  fill_char++;
  if ('Z' == fill_char)
    fill_char = 'A';
}

void
tty_mode (int how)
{
  static struct termios ttystate = { 0 };
  if (0 == how)
    tcgetattr (0, &ttystate);
  else if (1 == how)
    tcsetattr (0, TCSANOW, &ttystate);
}


int
main ()
{
  struct termios ttystate;
  int reply;

  signal (SIGWINCH, handsig);
  tty_mode (0);

  tcgetattr (0, &ttystate);
  ttystate.c_lflag &= ~ICANON;
  ttystate.c_lflag &= ~ECHO;
  ttystate.c_cc[VMIN] = 1;
  tcsetattr (0, TCSANOW, &ttystate);

  while (1)
    {
      reply = getchar ();
      if (reply == 'Q')
	break;
      else
	{
	  fill_char = 'A';
	  fillsc (fill_char);
	  fill_char++;
	}
    }

  tty_mode (1);
  char clrsc_cmd[] = "\033[2J\033[H";
  write (STDOUT_FILENO, clrsc_cmd, sizeof (clrsc_cmd));
  return 0;
}
