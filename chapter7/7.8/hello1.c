/* hello1.c
 *     purpose show the minimal calls needed to use curses
 *     outlineinitialize, draw stuff, wait for input, quit
 */

#include<stdio.h>
#include<curses.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>

char *msg = NULL;
char blank[256] = { 0 };

void handler (int);

int
main (int argc, char *argv[])
{
  int i;
  if (argc == 1)
    msg = "Hello world";
  else if (argc == 2)
    msg = argv[1];
  else
    {
      printf ("Usage: %s [messag]\n", argv[0]);
      return 1;
    }

  for (i = 0; i < strlen (msg); i++)
    blank[i] = ' ';

  initscr ();			//turn on curses

  //send requests
  clear ();			//clear screen

  i = 0;
  while (1)
    {
      handler (i);
      sleep (1);
      i = !i;
    }

  endwin ();			//turn off curses
  return 0;
}

void
handler (int i)
{
  const int row = 10;
  const int col = 20;
  if (0 == i)
    {
      move (row, col);
      addstr (msg);
      move (LINES - 1, 0);
      refresh ();
    }
  else
    {
      move (row, col);
      addstr (blank);
      move (LINES - 1, 0);
      refresh ();
    }
}
