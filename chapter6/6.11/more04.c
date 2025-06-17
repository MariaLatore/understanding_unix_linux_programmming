/* more02.c - version 0.2 of more
 *  read and print 24 lines then pause for a few special commands
 *  feature of version 0.2: reads from /dev/tty for commands
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>

#define PAGELEN 24
#define LINELEN 512

void do_more (FILE *);
int see_more (int);
int get_window_row_size ();
static int fd_tty = 0;
static int pagelen = PAGELEN;

void
tty_mode (int how)
{
  static struct termios ttystate;
  if (0 == how)
    tcgetattr (fd_tty, &ttystate);
  else if (1 == how)
    tcsetattr (fd_tty, TCSANOW, &ttystate);
}

void
handsig (int signum)
{
  tty_mode (1);
  exit (0);
}

int
main (int ac, char *av[])
{
  FILE *fp;
  struct termios ttystate;
  pagelen = get_window_row_size ();

  fd_tty = open ("/dev/tty", O_RDONLY);	//NEW:cmd stream
  if (fd_tty < 0)		//if open fails
    exit (1);			//no use in running

  tty_mode (0);
  tcgetattr (fd_tty, &ttystate);
  ttystate.c_lflag &= ~ECHO;
  ttystate.c_lflag &= ~ICANON;
  ttystate.c_cc[VMIN] = 1;
  tcsetattr (fd_tty, TCSANOW, &ttystate);

  signal (SIGINT, handsig);

  if (ac == 1)
    do_more (stdin);
  else
    while (--ac)
      if ((fp = fopen (*++av, "r")) != NULL)
	{
	  do_more (fp);
	  fclose (fp);
	}
      else
	break;
  tty_mode (1);
  return 0;
}

int
get_window_row_size ()
{
  struct winsize w = { 0 };
  if (0 > ioctl (STDOUT_FILENO, TIOCGWINSZ, &w))
    {
      perror ("ioctl() error");
      return PAGELEN;
    }
  return w.ws_row;
}

void
do_more (FILE * fp)
/*
 * read pagelen lines, then call see_more() for further instructions
 */
{
  char line[LINELEN];
  int num_of_lines = 0;
  int reply;

  while (fgets (line, LINELEN, fp))
    {				//more input
      if (num_of_lines == pagelen)
	{			//full screen
	  reply = see_more (fd_tty);	//NEW: pass FILE*
	  if (reply == 0)	//n: done
	    break;
	  num_of_lines -= reply;	//reset count
	}
      if (fputs (line, stdout) == EOF)	//show line
	exit (1);		//or die
      num_of_lines++;		//count it
    }
}

int
see_more (int fd)		//NEW:accept arg
/*
 * print message, wait for response, return # of lines to advance
 * q means no, space means yes, CR means one line
 */
{
  char c;
  int rc;
  int i;
  char see_more[] = "more? ";
  printf ("\033[7m %s\033[m", see_more);	//reverse on a vt100
  fflush (stdout);
  if (read (fd, &c, sizeof (c)) <= 0)
    rc = 0;
  if (c == 'q')			//q->N
    rc = 0;
  if (c == ' ')			//' ' => next page
    rc = pagelen;		// how many to show
  if (c == '\n')		//Enter key => 1 line
    rc = 1;
  putchar ('\r');
  for (i = 0; i < sizeof (see_more); i++)
    putchar (' ');
  putchar ('\r');
  return rc;
}
