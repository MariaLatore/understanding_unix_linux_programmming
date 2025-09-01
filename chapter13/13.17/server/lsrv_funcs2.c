/**************************************************************************
 * lsrv_funcs1.c
 * functions for the license server
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "dgram.h"
#include "lsrv_funcs2.h"

#define SERVER_PORTNUM 2020	//Our server's port number
#define MSGLEN 128		//Size OF OUR DATAGRAMS
#define TICKET_AVAIL 0		//Slot is avaliable for use
#define MAXUSERS 3		//Only 3 users for us
#define MAXIPADDRLEN 100
#define oops(x) {perror(x); exit(-1);}

/***************************************************************************
 * Important variables
 */
int ticket_array[MAXUSERS];	//Our ticket array
char ticket_ip_array[MAXUSERS][MAXIPADDRLEN];
int sd = -1;			//Our socket
int num_tickets_out = 0;	//Number of tickets outstanding

char *do_hello ();
char *do_goodbye ();
static char *do_validate (char *msg);

/*******************************************************************************
* setup() - initialize license server
*/
int
setup ()
{
  sd = make_dgram_server_socket (SERVER_PORTNUM);
  if (sd == -1)
    oops ("make socket");

  free_all_tickets ();
  return sd;
}

void
free_all_tickets ()
{
  int i;
  for (i = 0; i < MAXUSERS; i++)
    ticket_array[i] = TICKET_AVAIL;
}

/****************************************************************************
 * shut_down() - close down license server
 */
void
shut_down ()
{
  close (sd);
}

/*****************************************************************************
 * handle_request(request, clientaddr, addrlen)
 *  branch on code in request
 */
void
handle_request (char *req, struct sockaddr_in *client, socklen_t addlen)
{

  char *response;
  int ret;
  char ipaddr[MAXIPADDRLEN];
  strcpy (ipaddr, inet_ntoa (client->sin_addr));

/*act and compose a response */
  if (strncmp (req, "HELO", 4) == 0)
    response = do_hello (req, ipaddr);
  else if (strncmp (req, "GBYE", 4) == 0)
    response = do_goodbye (req);
  else if (strncmp (req, "VALD", 4) == 0)
    response = do_validate (req);
  else
    response = "FAIL invalid request";

/* send the response to the client */
  narrate ("SAID:", response, client);
  ret =
    sendto (sd, response, strlen (response), 0, (struct sockaddr *) client,
	    addlen);
  if (ret == -1)
    perror ("SERVER sendto failed");
}

/********************************************************************************
 * do_hello
 * Give out a ticket if any are avaliable
 * IN msg_p
 * Results: ptr to response
 *   NOTE: return is in static buffer overwritten by each call
 */
char *
do_hello (char *msg_p, char *ipaddr)
{
  int x;
  static char replybuf[MSGLEN];

  if (num_tickets_out >= MAXUSERS)
    return "FAIL no tickets available";

/* else find a free ticket and give it to client */
  for (x = 0; x < MAXUSERS && ticket_array[x] != TICKET_AVAIL; x++);

/* a sanity check - should never happen */
  if (x == MAXUSERS)
    {
      narrate ("database corrupt", "", NULL);
      return "FAIL database corrupt";
    }

/* Found a free ticket. Record "name" of user (pid) in array.
 *   generate ticket of form: pid.slot
 */
  if (1 != sscanf (msg_p + 5, "%d", &ticket_array[x]))	//get pid and ip in msg
    {
      narrate ("cannot parse hello msg", "", NULL);
      return "FAIL cannot parse request";
    }
  strcpy (ticket_ip_array[x], ipaddr);
  sprintf (replybuf, "TICK %d.%d:%s", ticket_array[x], x, ticket_ip_array[x]);
  num_tickets_out++;
  return replybuf;
}				/* do hello */

/*********************************************************************************
 * do_goodbye
 * Take back ticket client is returning
 * IN msg_p    message received from client
 * Results: ptr to response
 *   NOTE: return is in static buffer overwritten by each call
 *
 */
char *
do_goodbye (char *msg_p)
{
  int pid, slot;		//components of ticket
  char ipaddr[MAXIPADDRLEN] = { 0 };

/* The user's giving us back a ticket. First we need to get
 * the ticket out of the message, which looks like:
 *
 *    GBYE pid.slot
 */
  if ((sscanf ((msg_p + 5), "%d.%d:%s", &pid, &slot, ipaddr) != 3)
      || (ticket_array[slot] != pid)
      || (0 != strcmp (ipaddr, ticket_ip_array[slot])))
    {
      narrate ("Bogus ticket", msg_p + 5, NULL);
      return "FAIL invalid ticket";
    }

/* The ticket is valid. Release it. */
  ticket_array[slot] = TICKET_AVAIL;
  bzero (ticket_ip_array[slot], MAXIPADDRLEN);
  num_tickets_out--;
/* Return response */
  return "THNX See ya!";
}				/* do goodbye */

/*********************************************************************************
 * narrate() - chatty news for debugging and logging purposes
 */
void
narrate (char *msg1, char *msg2, struct sockaddr_in *clientp)
{
  fprintf (stderr, "\t\tSERVER: %s %s ", msg1, msg2);
  if (clientp)
    fprintf (stderr, "(%s:%d)", inet_ntoa (clientp->sin_addr),
	     ntohs (clientp->sin_port));
  putc ('\n', stderr);
}


/*********************************************************************************
 * ticket_reclaim
 * go through all tickets and reclaim ones belonging to dead processes
 * Results: none
 */
void
ticket_reclaim ()
{
  int i;
  char tick[BUFSIZ];
  for (i = 0; i < MAXUSERS; i++)
    {
      if ((ticket_array[i] != TICKET_AVAIL)
	  && (kill (ticket_array[i], 0) == -1) && (errno == ESRCH))
	{
/* Process is gone - free up slot */
	  sprintf (tick, "%d.%d:%s", ticket_array[i], i, ticket_ip_array[i]);
	  narrate ("freeing", tick, NULL);
	  ticket_array[i] = TICKET_AVAIL;
	  bzero (ticket_ip_array[i], MAXIPADDRLEN);
	  num_tickets_out--;
	}
    }
  alarm (RECLAIM_INTERVAL);	//reset alarm clock
}


/***********************************************************************************
 * do_validate
 * Validate client's ticket
 * IN msg_p    message received from client
 * Results: ptr to response
 *   NOTE: return is in static buffer overwritten by each call.
 */
static char *
do_validate (char *msg)
{
  int pid, slot;		//components of ticket
  char ipaddr[MAXIPADDRLEN];

/* msg looks like VALD pid.solt - parse it and validate */

  if (sscanf (msg + 5, "%d.%d:%s", &pid, &slot, ipaddr) == 3
      && ticket_array[slot] == pid
      && strcmp (ipaddr, ticket_ip_array[slot]) == 0)
    return ("GOOD Valid ticket");

/* bad ticket */
  narrate ("Bogus ticket", msg + 5, NULL);
  return ("FAIL invalid ticket");
}
