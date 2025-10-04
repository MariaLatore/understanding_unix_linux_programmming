/**********************************************************************************
 *  dgram.c
 *  support functions for datagram based programs
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "unixstream.h"
#include <netdb.h>
#include <string.h>

#define BACKLOG 1

int
make_unix_stream_server_socket (char *sockname)
{

  return make_unix_stream_server_socket_q (sockname, BACKLOG);
}

int
make_unix_stream_server_socket_q (char *sockname, int backlog)
{
  //struct sockaddr_un defined in un.h
  struct sockaddr_un saddr;	// build our address here
  int sock_id;			// the socket


  if (NULL == sockname)
    return -1;

  sock_id = socket (PF_UNIX, SOCK_STREAM, 0);	// get a socket
  if (sock_id == -1)
    {
      perror ("serv socket");
      return -1;
    }

  /** build address and bind it to socket **/
  make_unix_stream_address (sockname, &saddr);

  if (bind (sock_id, (struct sockaddr *) &saddr, sizeof (saddr)) != 0)
    {
      perror ("bind");
      return -1;
    }

  /* arrange for incoming calls */
  if (listen (sock_id, backlog) != 0)
    return -1;
  return sock_id;
}

int
make_unix_stream_address (char *sockname, struct sockaddr_un *addrp)
{
  if (NULL == sockname || NULL == addrp)
    return -1;
  addrp->sun_family = AF_UNIX;
  strcpy (addrp->sun_path, sockname);


  return 0;
}


int
connect_to_unix_stream_server (char *destsockname)
{
  int sock;
  struct sockaddr_un servaddr;
  socklen_t servaddrlen = sizeof (servaddr);

  if (NULL == destsockname)
    return -1;

  servaddr.sun_family = AF_UNIX;
  strcpy (servaddr.sun_path, destsockname);


  sock = socket (PF_UNIX, SOCK_STREAM, 0);
  if (-1 == sock)
    {
      perror ("clnt socket");
      return -1;
    }

  if (connect (sock, (struct sockaddr *) &servaddr, servaddrlen) != 0)
    {
      perror ("connect");
      return -1;
    }

  return sock;
}
