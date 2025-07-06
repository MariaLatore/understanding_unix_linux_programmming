/**    tinybc.c  * a tiny calculator that uses dc to do its work
 **              * demonstrates bidirectional pipes
 **              * input looks like number op number which
 **              * tinbv converts into number \n number \n op \n [p
 **              * and passes result back to stdout
 **
 **
 **             +---------------+                 +--------+
 **    stdin   >0               <==>bipipe[0]     |        |
 **             |    tinybc     |                 |   dc   |
 **    stdout  <1               |     bipipe[1]<==>        |
 **             +---------------+                 +--------+
 **
 **
 **              * program outline
 **                  a. get two pipes
 **                  b. fork (get another process)
 **                  c. int the dc-to-bc process.
 **                         connect stdin and out to pipes
 **                         then execl dc
 **                  d. in the tinybc-process, no plumbing to do 
 **                         just talk to human via normal i/o
 **                          and send stuff via pipe
 **                  e, then close pipe and dc dies
 **             * note: does not handle multiline answers
 **/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/socket.h>

#define oops(m,x) {perror(m); exit(x);}

void fatal (char mess[]);
void be_bc (int fd);
void be_dc (int fd);
int
main ()
{
  int pid;			//equipment
  int bipipe[2];
//make two pipes
  if (socketpair (AF_UNIX, SOCK_STREAM, PF_UNSPEC, bipipe) == -1)
    oops ("pipe failed", 1);

//get a process for user interface
  if ((pid = fork ()) == -1)
    oops ("cannot fork", 2);
  if (pid == 0)			//child is dc
    be_dc (bipipe[0]);
  else
    {
      be_bc (bipipe[1]);	//parent is ui
      wait (NULL);		//wait for child
    }
  return 0;
}

void
be_dc (int fd)
/*
 * set up stdin and stdout, then execl dc
 */
{
/* setup stdin fromm pipein */
  if (dup2 (fd, 0) == -1)	//copy read end to 0
    oops ("dc: cannot redirect stdin", 3);

/* setup stdout to pipeout */
  if (dup2 (fd, 1) == -1)	//dupe write end to 1
    oops ("dc: cannot redirect stdout", 4);
  close (fd);		//moved to fd 1

/* now execl dc with the - option */
  execlp ("dc", "dc", "-", NULL);
  oops ("Cannot run dc", 5);
}

void
be_bc (int fd)
/*
 * read from stdin and convert into to RPN, send down pipe
 * then read from other pipe and print to user
 * Uses fdopen() to convert a file descriptor to a stream
 */
{
  int num1, num2;
  char operation[BUFSIZ], message[BUFSIZ];
  FILE *fpout, *fpin;

/* setup */

  fpout = fdopen (fd, "w");	//convert file desc-
  fpin = fdopen (fd, "r");	//riptors to streams
  if (fpout == NULL || fpin == NULL)
    fatal ("Error converting pipes to stream");

//main loop
  while (printf ("tinybc: "), fgets (message, BUFSIZ, stdin) != NULL)
    {
//parse input
      if (sscanf (message, "%d%[-+*/^]%d", &num1, operation, &num2) != 3)
	{
	  printf ("syntax error\n");
	  continue;
	}
      if (fprintf (fpout, "%d\n%d\n%c\np\n", num1, num2, *operation) == EOF)
	fatal ("Error writing");

      fflush (fpout);
      fflush (fpin);
      if (fgets (message, BUFSIZ, fpin) == NULL)
	break;
      printf ("%d %c %d = %s", num1, *operation, num2, message);
    }
  fclose (fpout);		//close pipe
  fclose (fpin);		//dc will see EOF
}

void
fatal (char mess[])
{
  fprintf (stderr, "Error: %s\n", mess);
  exit (1);
}
