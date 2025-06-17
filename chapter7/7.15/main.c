/* bounce1d.c
 *     purpose animation with user controlled speed and direction
 *     note    the handler does the animation
 *             the main program reads keyboard input
 *     compile gcc -Wall -g main.c -lcurses -lrt
 */
#include <stdio.h>
#include <curses.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <aio.h>
/* some gloal settings main and the handler use */

#define MESSAGE "hello"
#define BLANK   "     "

int row;			// current row
int col;			//current column
int dir;			//where we are gong

int ndelay;			//new delay
int delay;			//bigger => slower
struct aiocb kbcbuf;		//an aio control buf
int done = 0;

int
main ()
{
  void move_msg (int);		//handler for timer

  void on_input (int sig);
  void setup_aio_buffer ();
  initscr ();
  crmode ();
  noecho ();
  clear ();


  row = 10;			//sart here
  col = 0;
  dir = 1;			//add 1 to row number
  delay = 200000;		//200ms = 0.2 seconds

  move (row, col);		//get into position
  addstr (MESSAGE);		//draw message
  signal (SIGIO, on_input);
  setup_aio_buffer ();		//initialize aio ctrl buff
  aio_read (&kbcbuf);		//place a read request


  nodelay (stdscr, TRUE);	//set to non-block mode

  while (1)
    {
      if (done)
	break;
      usleep (delay);
      move_msg (-1);
    }
  endwin ();
  return 0;
}

void
move_msg (int signum)
{
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

/* set members of struct.
 *  First specify args like those for read(fd, buf, num) and offset
 *  Then specify what to do (send signal) and what signal (SIGIO)
 */
void
setup_aio_buffer ()
{
  static char input[1];		//1 char of input

  kbcbuf.aio_fildes = 0;	//standard input
  kbcbuf.aio_buf = input;	//buffer
  kbcbuf.aio_nbytes = 1;	//number to read
  kbcbuf.aio_offset = 0;	//offset in file

//describe what to do when read is ready
  kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  kbcbuf.aio_sigevent.sigev_signo = SIGIO;	//send SIGIO
}

void
on_input (int sig)
{
  int c;
  char *cp = (char *) kbcbuf.aio_buf;	//cast to char

/*check for errors*/

  if (aio_error (&kbcbuf) != 0)
    perror ("reading failed");
  else
/*get number of chars read*/
  if (aio_return (&kbcbuf) == 1)
    {
      c = *cp;
      if (c == 'Q')
	done = 1;
      else if (c == ' ')
	dir = -dir;
      else if (c == 'f' && delay > 2)
	ndelay = delay / 2;
      else if (c == 's')
	ndelay = delay * 2;
      if (ndelay > 0)
	delay = ndelay;
    }
//place a new request
  aio_read (&kbcbuf);
}
