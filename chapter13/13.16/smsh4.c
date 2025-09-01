/* smsh2.c - small-shell version2
 *     small shell that supports command line parsing
 *     and if..then..else.fi logic (by calling process())
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "smsh.h"
#include "lcnt_funcs2.h"
#include "varlib.h"

#define DEF_PROMPT "> "
void do_validate_ticket ();

int
main ()
{
  char *cmdline, *prompt, **arglist;
  int result, process (char **);
  void setup ();

  prompt = DEF_PROMPT;
  setup ();

  while ((cmdline = next_cmd (prompt, stdin)) != NULL)
    {
      do_validate_ticket ();
      if ((arglist = splitline (cmdline)) != NULL)
	{
	  result = process (arglist);
	  free (arglist);
	}
      free (cmdline);
    }
  return result;
}

void
setup ()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
  extern char **environ;
  license_setup ();
  if (get_ticket () != 0)
    fatal ("could not get ticket, exit", "", 1);
  VLenviron2table (environ);
  signal (SIGINT, SIG_IGN);
  signal (SIGQUIT, SIG_IGN);
}

void
fatal (char *s1, char *s2, int n)
{
  fprintf (stderr, "Error: %s,%s\n", s1, s2);
  exit (n);
}

void
do_validate_ticket ()
{
  int rc = 0;
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
}
