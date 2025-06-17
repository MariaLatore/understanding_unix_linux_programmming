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

void move_msg(int);

int row;			// current row
int col;			//current column
int dir;			//where we are gong

int
main ()
{
  row = 10;			//sart here
  col = 0;
  dir = 1;			//add 1 to row number

  while (1)
    move_msg (1);
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
