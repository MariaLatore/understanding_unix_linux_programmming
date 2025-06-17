/* whotfile.c
 *     purpose: show how to redirect output for another program
 *        idea: fork, then in the child, redirect output, then exec
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

int
main ()
{
  int pid;
  int fd;
  char *filename = "userlist";
  printf ("About to run who into a file\n");

/* create a new process or quit*/
  if ((pid = fork ()) == -1)
    {
      perror ("fork");
      exit (1);
    }
/*child does the work*/
  if (pid == 0)
    {
      close (1);		//close
      struct stat fileinfo;
      if (0 != stat (filename, &fileinfo))
	//file does not exist
	fd = creat (filename, 0644);
      else
	fd = open (filename, O_WRONLY | O_APPEND);	//then open
      if (fd != 1)
	printf ("opened fd is not 1");
      execlp ("who", "who", NULL);	//and run
      perror ("execlp");
      exit (1);
    }
/*parent waits then reports*/
  if (pid != 0)
    {
      wait (NULL);
      printf ("Done running who. results in userlist\n");
    }
  return 0;
}
