/* hello5.c
 *     purpose bounce a message back and forth across the screen
 *     compile cc hello5.c -lcurses -o hello5
 */
#include <curses.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>

#define ROW 10
#define POSFACTOR 10		//1pixel = POSFACTOR * 1meter

/* a = g => v = g*t
 * x = 1/2 * g * t^2
 * 
 * v = delta x / delta t
 * delta x = 1
 * => delta t = 1 / (2*x*g)^0.5
 */

#define g 9.8

void
split_message (int y, int x, char *message)
{
  int len = strlen (message);
  int i;
  for (i = 0; i < len; i++)
    {
      int random_x = (rand () % 11) - 10;	//generate random num [-10,0]
      int random_y = (rand () % 6) - 3;	//generate random num [-2,2]
      //same (random_x, random_y) may appear more than once
      mvaddch (y + random_y, x + len + random_x, message[i]);
    }
  refresh ();
}

int
calspan (int pos)
{
  float span = 1.0 / sqrt (2 * pos * POSFACTOR * g);
  return (int) round (span * 1000000);
}


int
main ()
{
  char message[] = "Hello";
  char blank[] = "     ";
  int dir = +1;
  int pos = 0;


  int fd = open ("/dev/urandom", O_RDONLY);
  if (fd == -1)
    {
      perror ("Failed to open /dev/urandom");
      return 1;
    }

  unsigned int seed;
  if (read (fd, &seed, sizeof (seed)) != sizeof (seed))
    {
      perror ("Failed to read from /dev/urandom");
      close (fd);
      return 1;
    }
  close (fd);

  srand (seed);

  initscr ();
  clear ();
  int RIGHTEDGE = COLS - strlen (message);
  int LEFTEDGE = 0;
  pos = LEFTEDGE;

  while (1)
    {
      move (ROW, pos);
      addstr (message);		//draw string
      move (LINES - 1, COLS - 1);	//park the cursor
      refresh ();		//show string
      if (pos <= LEFTEDGE)
	usleep (calspan (pos + 1));
      else
	usleep (calspan (pos));
      move (ROW, pos);		//erase string
      addstr (blank);
      pos += dir;		//advance positiong
      if (pos >= RIGHTEDGE)	//check for bounce
	{

	  split_message (ROW, RIGHTEDGE, message);

	  while (1);
	}

    }
  endwin ();
  return 0;
}
