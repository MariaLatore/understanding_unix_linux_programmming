#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXARGS 20		//cmdline args
#define ARGLEN  100		//token length

char *makestring (char *buf);
void execute (char *arglist[]);
void sigint_handler (int signo);
void sigquit_handler (int signo);

int
main ()
{
  char *arglist[MAXARGS + 1];	//an array of ptrs
  int numargs;			//index into array
  char argbuf[ARGLEN];		//read stuff here

  // Set up signal handlers to ignore SIGINT and SIGQUIT in the parent shell
  signal (SIGINT, sigint_handler);
  signal (SIGQUIT, sigquit_handler);

  numargs = 0;
  while (numargs < MAXARGS)
    {

      printf ("Arg[%d]? ", numargs);
      if (NULL == fgets (argbuf, ARGLEN, stdin)
	  || 0 == strcmp (argbuf, "exit\n"))
	exit (0);
      else if (*argbuf != '\n')
	arglist[numargs++] = makestring (argbuf);
      else
	{
	  if (numargs > 0)
	    {			//any args?
	      arglist[numargs] = NULL;	//close list
	      execute (arglist);	//do it
	      numargs = 0;	//and reset
	    }
	}
    }
  return 0;
}

void
execute (char *arglist[])
/*
 * use fork and execvp and wait to do it
 */
{
  int pid, exitstatus;		//of child
  pid = fork ();		//make new process
  switch (pid)
    {
    case -1:
      perror ("fork failed");
      exit (1);
    case 0:
      // Restore default signal handlers in the child process
      signal (SIGINT, SIG_DFL);
      signal (SIGQUIT, SIG_DFL);
      execvp (arglist[0], arglist);	//do it
      perror ("execvp failed");
      exit (1);
    default:
      while (wait (&exitstatus) != pid);
      printf ("child exited with status %d, %d\n",
	      exitstatus >> 8, exitstatus & 0377);
    }
}

char *
makestring (char *buf)
/*
 * trim off newline and create storage for the string
 */
{
  char *cp;
  buf[strlen (buf) - 1] = '\0';	//trim newline
  cp = malloc (strlen (buf) + 1);	//get memory
  if (cp == NULL)
    {
      fprintf (stderr, "no memory\n");
      exit (1);
    }
  strcpy (cp, buf);		//copy chars
  return cp;
}

void
sigint_handler (int signo)
{
  // Do nothing, just ignore the SIGINT signal
  return;
}

void
sigquit_handler (int signo)
{
  // Do nothing, just ignore the SIGQUIT signal
  return;
}
