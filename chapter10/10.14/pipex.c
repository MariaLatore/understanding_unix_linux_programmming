#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define oops(m,x) {perror(m); exit(x);}
#define MAX_CMD 10
void
close_unused_pipes (int pipes[][2], int pipenum)
{
  int i;
  for (i = 0; i < pipenum; i++)
    {
      close (pipes[i][0]);
      close (pipes[i][1]);
    }
  return;
}


int
main (int argc, char *argv[])
{

  int i;
  int pipes[MAX_CMD - 1][2];
  int pid[MAX_CMD];
  int cmdnum = argc - 1;
  int pipenum = cmdnum - 1;

  if (cmdnum > MAX_CMD || cmdnum < 2)
    {
      fprintf (stderr, "CMD number cannot less than 2 or more than %d\n",
	       MAX_CMD);
      exit (1);
    }

  for (i = 0; i < pipenum; i++)
    {
      if ((pipe (pipes[i])) != 0)
	{
	  perror ("create pipes");
	  printf ("create pipe %d error\n", i);
	  exit (1);
	}
    }

//first child process
  if ((pid[0] = fork ()) == -1)
    oops ("Cannot fork", 3);
  if (pid[0] == 0)
    {
      //redirect stdout  
      if ((dup2 (pipes[0][1], 1)) == -1)
	oops ("could not redirect stdout", 555);
      close_unused_pipes (pipes, pipenum);
      execlp (argv[1], argv[1], NULL);
      oops (argv[1], 1);
    }


  for (i = 1; i <= pipenum - 1; i++)
    {

      if ((pid[i] = fork ()) == -1)
	oops ("Cannot fork", i + 1);
      if (pid[i] == 0)		//child porcess1
	{

	  //redirect stdin
	  if ((dup2 (pipes[i - 1][0], 0) == -1))
	    oops ("could not redirect stdin", i + 1);
	  //redirect stdout
	  if (dup2 (pipes[i][1], 1) == -1)
	    oops ("could not redirct stdout", i+1);
	  close_unused_pipes (pipes, pipenum);
	  execlp (argv[i + 1], argv[i + 1], NULL);
	  oops (argv[i + 1], i + 1);
	}
    }

//parent process
//redirect stdin
  if ((dup2 (pipes[i - 1][0], 0) == -1))
    oops ("could not redirect stdin", 5);
  close_unused_pipes (pipes, pipenum);
  execlp (argv[i + 1], argv[i + 1], NULL);
  oops (argv[i + 1], i + 1);


  return 0;
}
