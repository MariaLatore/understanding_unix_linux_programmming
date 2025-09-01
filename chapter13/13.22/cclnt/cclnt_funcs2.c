/****************************************************************
 * cclnt_funcs1.c: functions for the client of the license server
 */

#include "cclnt_funcs2.h"
#include "dgram.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Important variables used throughout
 */
static int pid = -1; // Our PID
static int cardnum = 0;
static int sd = -1;               // Our communications socket
static struct sockaddr serv_addr; // Server address
static socklen_t serv_alen;       // length of address
static char cards_buf[128];       // Buffer to hold our cards
static int have_cards = 0;        // Set when we have a cards

#define MSGLEN 128          // Size of our datagrams
#define SERVER_PORTNUM 2020 // Our server's port number
#define HOSTLEN 512
#define oops(p)                                                                \
  {                                                                            \
    perror(p);                                                                 \
    exit(1);                                                                   \
  }

char *do_transaction();

/*
 * setup: get pid, socket, and address of license server
 * IN     no args
 * RET    nothing, dies on error
 * notes: assumes server is on the same host as client
 */
void setup(int cn) {
  char hostname[BUFSIZ];
  cardnum = cn;

  pid = getpid();                  // for ticks and msgs
  sd = make_dgram_client_socket(); // to talk to server
  if (sd == -1)
    oops("Cannot create socket");
  gethostname(hostname, HOSTLEN); // server on same host
  make_internet_address(hostname, SERVER_PORTNUM,
                        (struct sockaddr_in *)&serv_addr);
  serv_alen = sizeof(serv_addr);
}

void shut_down() { close(sd); }

/******************************************************************
 * get_cards
 * get a cards from the license server
 * REsults: 0 for success, -1 for failure
 */
int get_cards() {
  char *response;
  char buf[MSGLEN];
  ;

  if (have_cards) // don't be greedy
    return (0);

  sprintf(buf, "HELO %d", cardnum); // compose request

  if ((response = do_transaction(buf)) == NULL)
    return -1;

  /* parse the response and see if we got a cards.
   *   on success, the message is: TICK cards_string
   *   on failure, the message is: FAIL failure-msg
   */
  if (strncmp(response, "CARD", 4) == 0) {
    strcpy(cards_buf, response + 5); // grab cards-id
    have_cards = 1;
    narrate("got cards", cards_buf);
    return 0;
  }

  if (strncmp(response, "FAIL", 4) == 0)
    narrate("Could not get cards", response);
  else
    narrate("Unknown message:", response);
  return -1;
} /* get cards */

/*************************************************************************
 * release_cards
 * Give a cards back to the server
 * Results: 0 for success, -1 for failure
 */
int release_cards() {
  char buf[MSGLEN + 5];
  char *response;

  if (!have_cards) // don't have a cards
    return 0;      // noting to release

  sprintf(buf, "GBYE %s", cards_buf); // compose message
  if ((response = do_transaction(buf)) == NULL)
    return -1;

  /* examine response
   * success: THNX info-string
   * failure: FAIL error-string
   */
  if (strncmp(response, "THNX", 4) == 0) {
    narrate("released cards OK", "");
    return 0;
  }

  if (strncmp(response, "FAIL", 4) == 0)
    narrate("release failed", response + 5);
  else
    narrate("Unknown message:", response);
  return -1;
} /* release_cards */

/*****************************************************************************
 * do_transaction
 * Send a request to the server and get a response back
 * IN msg_p      message to send
 * Results: pointer to message string, or NULL for error
 *          NOTE: pointer returned is to static storage
 *          overwritten by each successive call.
 * note: for extra security, compare retaddr to serv_addr (why?)
 */
char *do_transaction(char *msg) {
  static char buf[MSGLEN];
  struct sockaddr retaddr;
  socklen_t addrlen = sizeof(retaddr);
  int ret;

  ret = sendto(sd, msg, strlen(msg), 0, &serv_addr, serv_alen);
  if (ret == -1) {
    syserr("sendto");
    return NULL;
  }

  /* Get the response back */
  ret = recvfrom(sd, buf, MSGLEN, 0, &retaddr, &addrlen);
  if (ret == -1) {
    syserr("recvfrom");
    return NULL;
  }

  /* Now return the message itself */
  return buf;
} /* do_transaction */

/********************************************************************************
 * narrate: print message to stderr for debugging and demo purposes
 * IN msg1, msg2: strings to print along with pid and title
 * RET      nothing, dies on error
 */
void narrate(char *msg1, char *msg2) {
  fprintf(stderr, "CLIENT [%d]: %s %s\n", pid, msg1, msg2);
}

void syserr(char *msg1) {
  char buf[MSGLEN];
  sprintf(buf, "CLIENT [%d]: %s", pid, msg1);
  perror(buf);
}
