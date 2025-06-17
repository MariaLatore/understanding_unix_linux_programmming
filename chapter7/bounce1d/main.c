/* bounce1d.c
 *     purpose animation with user controlled speed and direction
 *     note    the handler does the animation
 *             the main program reads keyboard input
 *     compile cc bounce1d.c set_ticker.c -lcurses -o bounce1d
 */
#include <stdio.h>
#include <curses.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
/* some gloal settings main and the handler use */

#define MESSAGE "hello"
#define BLANK   "     "

int row;			// current row
int col;			//current column
int dir;			//where we are gong

int set_ticker (int n_msecs);
int
main ()
{
  int delay;			//bigger => slower
  int ndelay;			//new delay
  int c;			//user input
  void move_msg (int);		//handler for timer

  initscr ();
  crmode ();
  noecho ();
  clear ();

  row = 10;			//sart here
  col = 0;
  dir = 1;			//add 1 to row number
  delay = 200;			//200ms = 0.2 seconds

  move (row, col);		//get into position
  addstr (MESSAGE);		//draw message
  signal (SIGALRM, move_msg);
  set_ticker (delay);

  while (1)
    {
      ndelay = 0;
      c = getch ();
      if (c == 'Q')
	break;
      if (c == ' ')
	dir = -dir;
      if (c == 'f' && delay > 2)
	ndelay = delay / 2;
      if (c == 's')
	ndelay = delay * 2;
      if (ndelay > 0)
	set_ticker (delay = ndelay);
    }
  endwin ();
  return 0;
}

void
move_msg (int signum)
{
  signal (SIGALRM, move_msg);	//reset, just in case
  move (row, col);
  addstr (BLANK);
  col += dir;			//move to new colum
  move (row, col);		//then set cursor
  addstr (MESSAGE);		//redo message
  refresh ();			//and show it
/*
 * now handle borders
 */
  if (dir == -1 && col <= 0)
    dir = 1;
  else if (dir == 1 && col + strlen (MESSAGE) >= COLS)
    dir = -1;
}

/* set_ticker( number_of_milliseconds )
 *     arranges for interval timer to issue SIGALRM's at regular intervals
 *     return -1 on error, 0 for ok
 *     arg in milliseconds, converted into whole seconds and microseconds
 *     note: set_ticker(0) turns off ticker
 */
int
set_ticker (int n_msecs)
{
  struct itimerval new_timeset;
  long n_sec, n_usecs;

  n_sec = n_msecs / 1000;	//int part
  n_usecs = (n_msecs % 1000) * 1000L;	//remainder

  new_timeset.it_interval.tv_sec = n_sec;	//set reload
  new_timeset.it_interval.tv_usec = n_usecs;	//new ticker value
  new_timeset.it_value.tv_sec = n_sec;	//store this
  new_timeset.it_value.tv_usec = n_usecs;	//and this

  return setitimer (ITIMER_REAL, &new_timeset, NULL);
}
