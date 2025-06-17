#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#define oops(m,x) {perror(m); exit(x);}

int
main (int argc, char *argv[])
{
  int pipe1to2[2];
  int pipe2to3[2];
  int pid;
  int pid2;

  if (argc != 4)
    {
      fprintf (stderr, "usage: %s cmd1 cmd2 cmd3\n", argv[0]);
      exit (1);
    }

  if (0 != pipe (pipe1to2))
    oops ("Cannot get a pipe1to2", 1);

  if (0 != pipe (pipe2to3))
    oops ("Cannot get a pipe2to3", 2);

  if ((pid = fork ()) == -1)
    oops ("Cannot fork", 3);

  if (pid == 0)			//child porcess1
    {
      close (pipe1to2[0]);	//close read
      if ((dup2 (pipe1to2[1], 1)) == -1)
	oops ("could not redirect stdout", 5);
      close (pipe1to2[1]);


      close (pipe2to3[0]);
      close (pipe2to3[1]);
      execlp (argv[1], argv[1], NULL);
      oops (argv[1], 4);
    }

  if ((pid2 = fork ()) == -1)
    oops ("Cannot fork", 7);
  if (pid2 == 0)		//child process2
    {
      //redirect stdin
      close (pipe1to2[1]);	//close write
      if ((dup2 (pipe1to2[0], 0) == -1))
	oops ("could not redirect stdin", 5);
      close (pipe1to2[0]);
      //redirect stdout
      close (pipe2to3[0]);	//close read
      if (dup2 (pipe2to3[1], 1) == -1)
	oops ("could not redirct stdout", 6);
      close (pipe2to3[1]);
      execlp (argv[2], argv[2], NULL);
      oops (argv[2], 7);
    }

  //parent process
  close (pipe1to2[0]);
  close (pipe1to2[1]);
  close (pipe2to3[1]);		//close write
  if (dup2 (pipe2to3[0], 0) == -1)
    oops ("could not redirct stdin", 8);
  close (pipe2to3[1]);
  execlp (argv[3], argv[3], NULL);
  oops (argv[3], 9);
  return 0;
}
