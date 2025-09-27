/* tbounce1d.c: controlled animation using two threads
 *        note: one thread handles animation
 *              other thread handles keyboard input
 *     compile: cc tbounce1d.c -lcurses -lpthread -o tbounce1d
 */
#include <stdio.h>
#include <curses.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* shared variables both threads use. These need a mutex. */
#define MESSAGE " hello "

struct arg_set
{
  int *row_p;
  int *col_p;
  int *dir_p;
  int *delay_p;
  char *msg;
};

void *moving_msg ();		//int curses and tty
int
main ()
{
  int ndelay;			//new delay
  int c;			//user input
  int row;			//current row
  int col;			//current column
  int dir;			//where we are going
  int delay;			//delay between moves
  pthread_t msg_thread;		//a thread
  struct arg_set args = { 0 };

  args.row_p = &row;
  args.col_p = &col;
  args.dir_p = &dir;
  args.delay_p = &delay;
  args.msg = MESSAGE;


  initscr ();
  crmode ();
  noecho ();
  clear ();

  row = 10;			//start here
  col = 0;
  dir = 1;			//add 1 to row number
  delay = 200;			//200ms = 0.2 seconds

  if (pthread_create (&msg_thread, NULL, moving_msg, (void *) &args))
    {
      fprintf (stderr, "error creating thread");
      endwin ();
      exit (0);
    }
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
	delay = ndelay;
    }
  pthread_cancel (msg_thread);
  endwin ();
  return 0;
}

void *
moving_msg (void *arg)
{
  struct arg_set *parg = (struct arg_set *) arg;
  char *msg = parg->msg;
  while (1)
    {
      usleep (*parg->delay_p * 1000);	//sleep a while)
      move (*parg->row_p, *parg->col_p);	//set cursor position
      addstr (msg);		//redo message
      refresh ();		//and show it

/* move to next column and check for bouncing */
      *parg->col_p += *parg->dir_p;

      if (*parg->col_p <= 0 && *parg->dir_p == -1)
	*parg->dir_p = 1;
      else if (*parg->col_p + strlen (msg) >= COLS && *parg->dir_p == 1)
	*parg->dir_p = -1;
    }
}
