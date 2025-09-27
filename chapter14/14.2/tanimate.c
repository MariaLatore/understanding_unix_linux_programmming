/* tanimate.c: animate several strings using threads, curses, usleep()
 *
 * bigidea: one thread for each animated string
 *          one thread for keyboard control
 *          shared variables for communication
 * compile: cc tanimate.c -lcurses -lpthread -o tanimate
 *   to do: needs locks for shared variables
 *          nice to put screen handling in its own thread
 */
#include <stdio.h>
#include <fcntl.h>
#include <curses.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXMSG 10		//limit to number of strings
#define TUNIT 20000		//timeunits in microseconds
#define MAXMSGLEN 50


char print_msgs[MAXMSG][MAXMSGLEN] = { {0} };

struct propset
{
  char *str;			//the message
  int row;			//the row
  int delay;			//delay in time units
  int dir;			//+1 or -1
};
int setup (struct propset props[]);
void *animate (void *arg);


pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;

int
main (int ac, char *av[])
{
  int c;			//user input
  pthread_t thrds[MAXMSG];	//the threads
  struct propset props[MAXMSG];	//properties of string
  void *animate ();		//the function
  int num_msg;			//number of strings
  int i;

  num_msg = setup (props);
/* create all the threads */
  for (i = 0; i < num_msg; i++)
    if (pthread_create (&thrds[i], NULL, animate, &props[i]))
      {
	fprintf (stderr, "error creating thread");
	endwin ();
	exit (0);
      }
/* process user input */
  while (1)
    {
      c = getch ();
      if (c == 'Q')
	break;
      if (c == ' ')
	for (i = 0; i < num_msg; i++)
	  props[i].dir = -props[i].dir;
      if (c >= '0' && c <= '9')
	{
	  i = c - '0';
	  if (i < num_msg)
	    props[i].dir = -props[i].dir;
	}
    }

/* cancel all the threads */
  pthread_mutex_lock (&mx);
  for (i = 0; i < num_msg; i++)
    pthread_cancel (thrds[i]);
  endwin ();
  return 0;
}

int
setup (struct propset props[])
{
  int i;
  int num_msg;
  int fd;
  for (i = 0;
       i < MAXMSG && fgets (print_msgs[i], MAXMSGLEN, stdin) != NULL; i++);

  num_msg = i;

  fd = open ("/dev/tty", O_RDONLY);
  dup2 (fd, 0);

/* assign rows and velocities to each string */
  srand (getpid ());
  for (i = 0; i < num_msg; i++)
    {
      props[i].str = print_msgs[i];	//the message
      props[i].row = i;		//the row
      props[i].delay = 1 + (rand () % 15);	// a speed
      props[i].dir = ((rand () % 2) ? 1 : -1);	//+1 or -1
    }

/* set up curses */
  initscr ();
  crmode ();
  noecho ();
  clear ();
  mvprintw (LINES - 1, 0, "'Q' to quit, '0'..%d' to bounc", num_msg - 1);
  return num_msg;
}

/* the code that runs in each thread */
void *
animate (void *arg)
{
  struct propset *info = arg;	//pint to info block
  int len = strlen (info->str) + 2;	//+2 for padding
  int col = rand () % (COLS - len - 3);	//space for padding

  while (1)
    {
      usleep (info->delay * TUNIT);
      pthread_mutex_lock (&mx);	//only one thread
      move (info->row, col);	//can call curses
      addch (' ');		//at the same tiem
      addstr (info->str);	//Since I doubt it si
      addch (' ');		//reentrant
      move (LINES - 1, COLS - 1);	//park cursor
      refresh ();		//and show it
      pthread_mutex_unlock (&mx);	//done with curses

/* move item to next column and check for bouncing */
      col += info->dir;
      if (col <= 0 && info->dir == -1)
	info->dir = 1;
      else if (col + len >= COLS && info->dir == 1)
	info->dir = -1;
    }
}
