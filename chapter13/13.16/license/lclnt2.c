/******************************************************************
 * lclnt1.c
 * License server client version 1
 * link with lclnt_funcs1.o dgram.o
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "lcnt_funcs2.h"
void do_regular_work ();

int
main (int ac, char *av[])
{
  setup ();
  if (get_ticket () != 0)
    exit (0);

  do_regular_work ();
  release_ticket ();
  shut_down ();
  return 0;
}

/******************************************************************
 * do_regular_work the main work of the application goes here
 */
void
do_regular_work ()
{
  printf ("SuperSleep version 1.0 Running - Licensed Software\n");
  int rc = 0;
  sleep (15);			//cur patented sleep algorithm
  rc = validate_ticket ();
  if (rc)
    {
      narrate ("validate ticket fail", "");
      rc = reget_ticket ();
      if (rc)
	{
	  narrate ("reget ticket fail, exit!", "");
	  exit (1);
	}
      else
	narrate ("reget ticket successfully", "");
    }
  sleep (15);
}
