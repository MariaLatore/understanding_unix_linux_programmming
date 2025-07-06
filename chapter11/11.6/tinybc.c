/** tinybc.c * a tiny calculator that uses dc to do its work
 * ** * demonstrates bidirectional pipes
 * ** * input looks like number op number which
 * ** * tinbv converts into number \n number \n op \n [p
 * ** * and passes result back to stdout
 * **
 * **
 * ** +---------------+ +--------+
 * ** stdin >0 >== pipetodc =====> |
 * ** | tinybc | | dc |
 * ** stdout <1 <== pipefromdc ===< |
 * ** +---------------+ +--------+
 * **
 * **
 * ** * program outline
 * ** a. get two pipes
 * ** b. fork (get another process)
 * ** c. int the dc-to-bc process.
 * ** connect stdin and out to pipes
 * ** then execl dc
 * ** d. in the tinybc-process, no plumbing to do
 * ** just talk to human via normal i/o
 * ** and send stuff via pipe
 * ** e, then close pipe and dc dies
 * ** * note: does not handle multiline answers
 * **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define oops(m, x) {perror (m); exit (x);}

void fatal (char mess[]);
void be_bc (int todc[2], int fromdc[2]);
void be_dc (int in[2], int out[2]);
void be_bc_c (int tobc[2], int frombc[2], char *arg);
int be_tinybc (int tobc[2], int frombc[2]);

int
main (int argc, char *argv[])
{
  char c;
  char *optstr = "c:";
  int tobc[2], frombc[2];
  int pid;

  if (argc > 1)
    {
      c = getopt (argc, argv, optstr);
      switch (c)
	{
	case 'c':
	  break;
	default:
	  fatal ("invalid argrment\n Arag: -c \"command\"");
	}

      if (pipe (tobc) == -1 || pipe (frombc) == -1)
	oops ("bc pipe failed", 11);
      if ((pid = fork ()) == -1)
	oops ("cannot for bc", 12);
      if (pid == 0)
	be_tinybc (tobc, frombc);
      else
	{
	  be_bc_c (tobc, frombc, optarg);

	  wait (NULL);		//wait for child
	}
      return 0;
    }
  return be_tinybc (NULL, NULL);
}

int
be_tinybc (int tobc[2], int frombc[2])
{
  if (tobc != NULL && frombc != NULL)
    {
      close (tobc[1]);
      close (frombc[0]);
      if (dup2 (tobc[0], 0) == -1)
	oops ("bc:cannot redirect stdin", 13);
      close (tobc[0]);
      if (dup2 (frombc[1], 1) == -1)
	oops ("bc:cannot redirect stdout", 14);
      close (frombc[1]);
    }
  else if (!(tobc == NULL && frombc == NULL))
    oops ("bc:invalid argument", 15);

  int pid, todc[2], fromdc[2];	//equipment
//make two pipes
  if (pipe (todc) == -1 || pipe (fromdc) == -1)
    oops ("pipe failed", 1);
  //get a process for user interface
  if ((pid = fork ()) == -1)
    oops ("cannot fork", 2);
  if (pid == 0)			//child is dc
    be_dc (todc, fromdc);
  else
    {
      be_bc (todc, fromdc);	//parent is ui
    }
  return 0;
}


void
be_dc (int in[2], int out[2])
	       /*set up stdin and stdout, then execl dc */
{
  // setup stdin fromm pipein
  close (in[1]);		//won't write here
  if (dup2 (in[0], 0) == -1)	//copy read end to 0
    oops ("dc: cannot redirect stdin", 3);
  close (in[0]);		//moved to fd 0
/*setup stdout to pipeout */
  if (dup2 (out[1], 1) == -1)	//dupe write end to 1
    oops ("dc: cannot redirect stdout", 4);
  close (out[1]);		//moved to fd 1
  close (out[0]);		//won't read from here
  /* now execl dc with the - option */
  execlp ("dc", "dc", "-", NULL);
  oops ("Cannot run dc", 5);
}


void
be_bc (int todc[2], int fromdc[2])
 /* read from stdin and convert into to RPN, send down pipe
    then read from other pipe and print to user
    Uses fdopen() to convert a file descriptor to a stream */
{
  int num1, num2;
  char operation[BUFSIZ], message[BUFSIZ];
  FILE *fpout, *fpin;
  /*setup */
  close (todc[0]);		//won't read from pipe to dc
  close (fromdc[1]);		//won't write to pipe from dc
  fpout = fdopen (todc[1], "w");	//convert file desc-
  fpin = fdopen (fromdc[0], "r");	//riptors to streams
  if (fpout == NULL || fpin == NULL)
    fatal ("Error converting pipes to stream");
  //main loop
  while (printf ("tinybc: "), fflush (stdout),
	 fgets (message, BUFSIZ, stdin) != NULL)
    {
      printf ("reseceive msg: %s\n", message);
      //parse input
      if (sscanf (message, "%d%[-+*_/^]%d", &num1, operation, &num2) != 3)
	{
	  printf ("syntax error\n");
	  continue;
	}

      if (fprintf (fpout, "%d\n%d\n%c\np\n", num1, num2, *operation) == EOF)
	fatal ("Error writing");
      fflush (fpout);
      if (fgets (message, BUFSIZ, fpin) == NULL)
	break;
      printf ("%d %c %d = %s", num1, *operation, num2, message);
    }

  fclose (fpout);		//close pipe
  fclose (fpin);		//dc will see EOF 
}

void
be_bc_c (int tobc[2], int frombc[2], char *arg)
{
  FILE *fpout, *fpin;
  char message[BUFSIZ];
  int i, j;
  //when bc output one more lines, this value should increase
  const int BC_OUTPUT_LINE_NUM = 3;
  close (tobc[0]);
  close (frombc[1]);
  fpout = fdopen (tobc[1], "w");
  fpin = fdopen (frombc[0], "r");
  if (fpout == NULL || fpin == NULL)
    fatal ("Error converting pipes in bc_c");
  //main loop
  for (i = 0; arg[i] != 0; i++)
    {
      fputc (arg[i], fpout);
      if (arg[i] == '\n')
	{
	  fflush (fpout);	//this is very important
	  for (j = 0; j < BC_OUTPUT_LINE_NUM; j++)
	    {
	      fgets (message, BUFSIZ, fpin);
	      printf ("%s", message);
	    }
	}
    }
  fclose (fpout);
  fclose (fpin);
}

void
fatal (char mess[])
{
  fprintf (stderr, "Error: %s\n", mess);
  exit (1);
}
