/******************************************************************************************
 * lserv1.c
 * License server server program version 1
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/errno.h>
#include "lsrv_funcs2.h"
#define MSGLEN 128		//Size of our datagrams

int
main (int ac, char *av[])
{
  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof (client_addr);
  char buf[MSGLEN];
  int ret;
  int sock;
  unsigned time_left;

  sock = setup ();
  signal (SIGALRM, ticket_reclaim);	//version 2 addition
  alarm (RECLAIM_INTERVAL);	//after this delay

  while (1)
    {
      addrlen = sizeof (client_addr);
      ret =
	recvfrom (sock, buf, MSGLEN, 0, (struct sockaddr *) &client_addr,
		  &addrlen);
      if (ret != -1)
	{
	  buf[ret] = 0;
	  narrate ("GOT:", buf, &client_addr);
	  time_left = alarm (0);
	  handle_request (buf, &client_addr, addrlen);
	  alarm (time_left);
	}
      else if (errno != EINTR)
	perror ("recvfrom");
    }
}
