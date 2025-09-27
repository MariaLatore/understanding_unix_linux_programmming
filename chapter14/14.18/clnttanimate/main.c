/* tanimate.c: animate several strings using threads, curses, usleep()
 *
 * bigidea: one thread for each animated string
 *          one thread for keyboard control
 *          shared variables for communication
 * compile: cc tanimate.c -lcurses -lpthread -o tanimate
 *   to do: needs locks for shared variables
 *          nice to put screen handling in its own thread
 */
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dgram.h"

#define MAXMSG 10		//limit to number of strings
#define TUNIT 20000		//timeunits in microseconds
struct propset
{
  char *str;			//the message
  int row;			//the row
  int delay;			//delay in time units
  int dir;			//+1 or -1
  int clntsock;
  struct sockaddr_in saddr;
  int servCOLS;
  int servLINES;
};
int setup (int nstrings, char *strings[], struct propset props[]);
void *animate (void *arg);
struct sockaddr_in servaddr;

void
sighandler (int sig)
{
  int clntsock = make_dgram_client_socket ();
  char msgbuf[] = "CLEAR";
  if (0 >
      sendto (clntsock, msgbuf, strlen (msgbuf), 0,
	      (struct sockaddr *) &servaddr, sizeof (servaddr)))
    {
      fprintf (stderr, "error send msg to server\n");
      exit (3);
    }

  close (clntsock);
  signal (SIGINT, SIG_DFL);
  raise (SIGINT);
}

int
main (int ac, char *av[])
{
  int c;			//user input
  pthread_t thrds[MAXMSG];	//the threads
  struct propset props[MAXMSG];	//properties of string
  void *animate ();		//the function
  int num_msg;			//number of strings
  int i;

  if (ac <= 3)
    {
      printf ("usage: %s string ..\n", av[0]);
      exit (1);
    }
  num_msg = setup (ac - 3, av, props);
/* create all the threads */
  for (i = 0; i < num_msg; i++)
    if (pthread_create (&thrds[i], NULL, animate, &props[i]))
      {
	fprintf (stderr, "error creating thread");
	exit (0);
      }
/* process user input */
  while (1)
    {
      c = getchar ();
      if (c == 'Q')
	break;
      if (c == ' ')
	for (i = 0; i < num_msg; i++)
	  props[i].dir = -props[i].dir;
      if (c >= '0' && c <= '9')
	{
	  i = c - '0';
	  if (i < num_msg)
	    props[i].dir = -props[i].dir;
	}
    }

/* cancel all the threads */
  for (i = 0; i < num_msg; i++)
    pthread_cancel (thrds[i]);
  for (i = 0; i < num_msg; i++)
    close (props[i].clntsock);
  return 0;
}

int
setup (int nstrings, char *av[], struct propset props[])
{
  int num_msg = (nstrings > MAXMSG ? MAXMSG : nstrings);
  int i;
  char **strings = &av[3];
  int clntsock = make_dgram_client_socket ();
  int servCOLS = 0;
  int servLINES = 0;
  char msgbuf[] = "GETWINSIZE";
  make_internet_address (av[1], atoi (av[2]), &servaddr);
  /* get the server window size */
  if (0 > clntsock)
    {
      fprintf (stderr, "error make clnt socket\n");
      exit (5);
    }
  if (0 >
      sendto (clntsock, msgbuf, strlen (msgbuf), 0,
	      (struct sockaddr *) &servaddr, sizeof (servaddr)))
    {
      fprintf (stderr, "error send msg to server\n");
      exit (3);
    }
  if (0 < recvfrom (clntsock, msgbuf, sizeof (msgbuf), 0, NULL, NULL))
    {
      char *del = " ";
      char *rowstr;
      char *colstr;
      char msgbufbak[BUFSIZ];
      strcpy (msgbufbak, msgbuf);

      rowstr = strtok (msgbuf, del);
      colstr = strtok (NULL, del);
      if (NULL == rowstr || NULL == colstr)
	{
	  fprintf (stderr, "error parse server window size:%s\n", msgbuf);
	  exit (4);
	}
      servCOLS = atoi (colstr);
      servLINES = atoi (rowstr);

    }
  else
    {
      fprintf (stderr, "error recv msg from server\n");
      exit (6);
    }
  close (clntsock);

/* signal handler */
  signal (SIGINT, sighandler);

  fprintf (stderr, "cols:%d, lines:%d\n", servCOLS, servLINES);
/* assign rows and velocities to each string */
  srand (getpid ());
  for (i = 0; i < num_msg; i++)
    {
      props[i].str = strings[i];	//the message
      props[i].row = i;		//the row
      props[i].delay = 1 + (rand () % 15);	// a speed
      props[i].dir = ((rand () % 2) ? 1 : -1);	//+1 or -1
      props[i].servCOLS = servCOLS;
      props[i].servLINES = servLINES;
      props[i].clntsock = make_dgram_client_socket ();
      if (0 >= props[i].clntsock)
	{
	  fprintf (stderr, "error make clnt socket\n");
	  exit (2);
	}
      make_internet_address (av[1], atoi (av[2]), &props[i].saddr);

    }

  return num_msg;
}

/* the code that runs in each thread */
void *
animate (void *arg)
{
  struct propset *info = arg;	//pint to info block
  int len = strlen (info->str) + 2;	//+2 for padding
  int col = rand () % (info->servCOLS - len - 3);	//space for padding
  char msgbuf[BUFSIZ] = { 0 };

  while (1)
    {
      usleep (info->delay * TUNIT);
      snprintf (msgbuf, BUFSIZ, "DRAW %d %d  %s ", info->row, col, info->str);
      if (0 >
	  sendto (info->clntsock, msgbuf, strlen (msgbuf), 0,
		  (struct sockaddr *) &info->saddr, sizeof (info->saddr)))
	{
	  fprintf (stderr, "error send msg to server\n");
	  exit (3);
	}

      /* move item to next column and check for bouncing */
      col += info->dir;
      if (col <= 0 && info->dir == -1)
	info->dir = 1;
      else if (col + len >= info->servCOLS && info->dir == 1)
	info->dir = -1;
    }
}
