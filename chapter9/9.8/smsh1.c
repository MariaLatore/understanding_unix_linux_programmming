/* smsh1.c  small-shell version 1
 *   first really useful version after prompting shell
 *   this one parses the command line into strings
 *   uses fork, exec, wait, and ignores signals
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "smsh.h"

#define DFL_PROMPT "> "

void
sighandler (int signum)
{

  int status;
  int pid;
  // 使用 WNOHANG 避免阻塞
  while ((pid = waitpid (-1, &status, WNOHANG)) > 0)
    printf ("success wait for %d, return status %d\n", pid, status);
}


int
main ()
{
  char *cmdline, *prompt, **arglist;
  int result;
  int back_ground;
  void setup ();

  prompt = DFL_PROMPT;
  setup ();

  while ((cmdline = next_cmd (prompt, stdin, &back_ground)) != NULL)
    {
      if ((arglist = splitline (cmdline)) != NULL)
	{
	  result = execute (arglist, back_ground);
	  freelist (arglist);
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
  signal (SIGCHLD, sighandler);
  signal (SIGINT, SIG_IGN);
  signal (SIGQUIT, SIG_IGN);
}

void
fatal (char *s1, char *s2, int n)
{
  fprintf (stderr, "Error: %s,%s\n", s1, s2);
  exit (n);
}
