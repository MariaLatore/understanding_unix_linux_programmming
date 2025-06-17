/* smsh1.c  small-shell version 1
 *   first really useful version after prompting shell
 *   this one parses the command line into strings
 *   uses fork, exec, wait, and ignores signals
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include "smsh.h"

#define DFL_PROMPT "> "

void do_exit (char **);
int shell_process_id = 0;

int
main ()
{
  char *cmdline, *prompt, **arglist;
  int result;
  void setup ();

  prompt = DFL_PROMPT;
  setup ();

  while ((cmdline = next_cmd (prompt, stdin)) != NULL)
    {
      if ((arglist = splitline (cmdline)) != NULL)
	{
	  if (arglist[0] != NULL && strcmp (arglist[0], "exit") == 0)
	    do_exit (arglist);
	  else if (arglist[0] != NULL && strcmp (arglist[0], "if") == 0)
	    do_if ();
	  else if (arglist[0] != NULL && strcmp (arglist[0], "while") == 0)
	    do_while ();
	  else
	    result = execute (arglist);
	  freelist (arglist);
	}
      free (cmdline);
    }
  return result;
}

void
do_exit (char **arglist)
{
  int i;
  int len;
  int rc = 0;
  int exit_num = 0;
  if (arglist[1] == NULL)
    exit (0);
  if (arglist[1] != NULL && arglist[2] == NULL)
    {
      len = strlen (arglist[1]);
      for (i = 0; i < len; i++)
	{
	  if (!(isdigit (arglist[1][i])))
	    break;
	}
      rc = (i != len);
    }
  else
    rc = 1;

  if (rc)
    {
      printf ("Usage: %s <exit num>\n", arglist[0]);
      return;
    }

  exit_num = atoi (arglist[1]);
  exit (exit_num);
  return;
}

void
setup ()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
  signal (SIGINT, SIG_IGN);
  signal (SIGQUIT, SIG_IGN);
  shell_process_id = getpid ();
}

void
fatal (char *s1, char *s2, int n)
{
  fprintf (stderr, "Error: %s,%s\n", s1, s2);
  exit (n);
}
