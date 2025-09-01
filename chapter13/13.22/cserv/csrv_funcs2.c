/**************************************************************************
 * lsrv_funcs1.c
 * functions for the license server
 */
#include "csrv_funcs2.h"
#include "dgram.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORTNUM 2020	// Our server's port number
#define MSGLEN 128		// Size OF OUR DATAGRAMS
#define CARDS_AVAIL 0		// Slot is avaliable for use
#define oops(x)                                                                \
  {                                                                            \
    perror(x);                                                                 \
    exit(-1);                                                                  \
  }
#define CARDSNUM 54
#define CARDSNAMEMAXLEN 10
#define CLNTADDRSTRLEN 50
#define DEBUGLOG(x)                                        \
  do {                                                     \
    printf ("num_cards_out %d, x %d\n", num_cards_out, x); \
  } while(0)


/***************************************************************************
 * Important variables
 */
int sd = -1;			// Our socket
int num_cards_out = 0;		// Number of tickets outstanding
/*
 * The cards format: num+color
 * H: heart
 * D: Diamond
 * S: Spade
 * C: Club
 * T: ten
 * LJ: little joker
 * BJ: big joker
 */
static char cards[CARDSNUM][CARDSNAMEMAXLEN] = {
  "1H", "1D", "1C", "1S", "2H", "2D", "2C", "2S", "3H", "3D", "3C",
  "3S", "4H", "4D", "4C", "4S", "5H", "5D", "5C", "5S", "6H", "6D",
  "6C", "6S", "7H", "7D", "7C", "7S", "8H", "8D", "8C", "8S", "9H",
  "9D", "9C", "9S", "TH", "TD", "TC", "TS", "JH", "JD", "JC", "JS",
  "QH", "QD", "QC", "QS", "KH", "KD", "KC", "KS", "LJ", "BJ"
};
static char cards_usage_array[CARDSNUM][CLNTADDRSTRLEN] = { {0} };

char *do_hello ();
char *do_goodbye ();
static void
shuffle ()
{
  char randombytes[CARDSNUM] = { -1 };
  int fd = open ("/dev/urandom", O_RDONLY);
  int i;
  char temp[CARDSNAMEMAXLEN] = { 0 };
  int randidx;
  if (fd == -1)
    {
      perror ("open /dev/urandom failed");
      exit (-2);
    }

  if (read (fd, randombytes, CARDSNUM) != CARDSNUM)
    {
      perror ("read /dev/urandom failed");
      exit (-3);
    }
  close (fd);

  for (i = CARDSNUM - 1; i >= 1; i--)
    {
      randidx = ((unsigned char) randombytes[i]) % (i + 1);
      // printf("radnidx %d i %d\n", randidx, i);
      strcpy (temp, cards[randidx]);
      strcpy (cards[randidx], cards[i]);
      strcpy (cards[i], temp);
    }

  puts ("after shuffle cards are:");
  for (i = 0; i < CARDSNUM; i++)
    {
      printf ("%s ", cards[i]);
      if ((i + 1) % 9 == 0)
	putchar ('\n');
    }
  fflush (stdout);
  return;
}

/*******************************************************************************
 * setup() - initialize license server
 */
int
setup ()
{
  shuffle ();
  sd = make_dgram_server_socket (SERVER_PORTNUM);
  if (sd == -1)
    oops ("make socket");

  return sd;
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

  /*act and compose a response */
  if (strncmp (req, "HELO", 4) == 0)
    response = do_hello (req, client);
  else if (strncmp (req, "GBYE", 4) == 0)
    response = do_goodbye (req, client);
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
do_hello (char *msg_p, struct sockaddr_in *client)
{
  int x, y;
  static char replybuf[MSGLEN];
  char clntaddr[CLNTADDRSTRLEN] = { 0 };
  int requirenum = -1;

  sprintf (replybuf, "CARD");

  /* Found a free ticket. Record "name" of user (pid) in array.
   *   generate ticket of form: pid.slot
   */
  if (sscanf (msg_p + 5, "%d", &requirenum) != 1 || NULL == client)
    {
      narrate ("parse hello error", "", NULL);
      return "FAIL parse hello request";
    }
  if (num_cards_out + requirenum > CARDSNUM)
    {
      DEBUGLOG (requirenum);
      return "FAIL no enough cards available";
    }

  snprintf (clntaddr, CLNTADDRSTRLEN, "%s:%d", inet_ntoa (client->sin_addr),
	    client->sin_port);
  /* else find a free ticket and give it to client */
  for (x = 0, y = requirenum; x < CARDSNUM; x++)
    {
      if (0 == strlen (cards_usage_array[x]))
	{
	  strncpy (cards_usage_array[x], clntaddr, CLNTADDRSTRLEN);
	  strcat (replybuf, " ");
	  strcat (replybuf, cards[x]);
	  num_cards_out++;
	  y--;
	  if (y == 0)
	    break;
	}
    }
  DEBUGLOG (requirenum);
  /* a sanity check - should never happen */
  if (x == CARDSNUM || y != 0)
    {
      narrate ("database corrupt", "", NULL);
      return "FAIL database corrupt";
    }

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
do_goodbye (char *msg_p, struct sockaddr_in *client)
{
  char clntaddr[CLNTADDRSTRLEN] = { 0 };
  char *retcards = NULL;
  int x;

  /* The user's giving us back a ticket. First we need to get
   * the ticket out of the message, which looks like:
   *
   *    GBYE pid.slot
   */
  if (NULL == msg_p || 0 == strlen (msg_p + 5) || NULL == client)
    {
      narrate ("cannot parse GBYE request", msg_p, NULL);
      return "FAIL parse GBYE request";
    }

  retcards = msg_p + 5;

  snprintf (clntaddr, CLNTADDRSTRLEN, "%s:%d", inet_ntoa (client->sin_addr),
	    client->sin_port);

  for (x = 0; x < CARDSNUM; x++)
    {
      if (NULL != strstr (retcards, cards[x]) &&
	  0 != strlen (cards_usage_array[x]))
	{
	  if (0 != strcmp (cards_usage_array[x], clntaddr))
	    {
	      narrate ("Clnt addr not match cards record", "", NULL);
	      break;
	    }
	  bzero (cards_usage_array[x], CLNTADDRSTRLEN);
	  num_cards_out--;
	}
    }
  /* a sanity check - should never happen */
  if (num_cards_out < 0 || x != CARDSNUM)
    {
      narrate ("database corrupt", "", NULL);
      return "FAIL database corrupt";
    }
  DEBUGLOG (0);

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
