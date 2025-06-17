/* hello5.c
 *     purpose bounce a message back and forth across the screen *     compile cc hello5.c -lcurses -o hello5
 */
#include <curses.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define ROW 30
#define PI 3.1415926

/*
 * v = A sin(2pi t/T)
 * => x = AT/2pi * (1 - cos(2pi t/T))
 * so when we choose T, we can calculate A
 * according to the width of the window
 * 
 * v = delta x / delta t
 * delta x = 1
 * => delta t = (4piA x/T - 4pi^2 x^2/T^2)^(-0.5) 
 */

float
calA (int winwidth, int Tus)
{
  float T = Tus / 1000.0;
  return PI * winwidth / T;
}

int
calspan (int pos, float A, int Tus)
{
  float T = Tus / 1000.0;
  float span =
    1.0 / sqrt (4 * PI * A * pos / T -
		4 * pow (PI, 2) * pow (pos, 2) / pow (T, 2));
  return (int) round (1000 * span);
}

static int T = 5000000;		//period is 1s

int
main ()
{
  char message[] = "Hello";
  char blank[] = "     ";
  int dir = +1;
  int pos;
  int usleepspan = 0;

  initscr ();
  clear ();
  int RIGHTEDGE = COLS - strlen (message);
  int LEFTEDGE = 0;
  float amplitude = calA (RIGHTEDGE - LEFTEDGE, T);
  pos = LEFTEDGE;
  while (1)
    {
      move (ROW, pos);
      addstr (message);		//draw string
      move (LINES - 1, COLS - 1);	//park the cursor
      refresh ();		//show string
      if (pos <= LEFTEDGE)
	usleepspan = calspan (pos + 1, amplitude, T);	//use last span at pos LEFTEDGE+1, or the span is too large at the edge, theorarily, delta t = delta x/ v, and v=0
      else if (pos >= RIGHTEDGE)
	usleepspan = calspan (pos - 1, amplitude, T);	//use last span, or the span is too large at the edge
      else
	usleepspan = calspan (pos, amplitude, T);
      usleep (usleepspan);
      move (ROW, pos);		//erase string
      addstr (blank);
      pos += dir;		//advance positiong
      if (pos >= RIGHTEDGE)	//check for bounce
	{
	  pos = RIGHTEDGE;
	  dir = -1;
	}
      if (pos <= LEFTEDGE)
	{
	  pos = LEFTEDGE;
	  dir = +1;
	}
    }
  return 0;
}
