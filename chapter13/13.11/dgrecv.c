/************************************************************************
 * dgrecv.c - datagram receiver
 *            usage: dgrecv portnum
 *            action: listens at the specified port and reports messages
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "dgram.c"

#define oops(m,x) {perror(m); exit(x);}

int make_dgram_server_socket (int);
int get_internet_address (char *, int, int *, struct sockaddr_in *);
void say_who_called (struct sockaddr_in *);
static char timebuffer[80];
int gettime ();


int
main (int ac, char *av[])
{
  int port;			//use this port
  int sock;			//for this socket
  char buf[BUFSIZ];		//to receive data here
  size_t msglen;		//store its length here
  struct sockaddr_in saddr;	//put sender's address here
  socklen_t saddrlen;		//and its length here

  if (ac == 1 || (port = atoi (av[1])) <= 0)
    {
      fprintf (stderr, "usage: dgrecv portnumber\n");
      exit (1);
    }

/* get a socket and assign it a port number */
  if ((sock = make_dgram_server_socket (port)) == -1)
    oops ("cannot make socket", 2);

/* receive messages on that socket */
  saddrlen = sizeof (saddr);
  while ((msglen =
	  recvfrom (sock, buf, BUFSIZ, 0, (struct sockaddr *) &saddr,
		    &saddrlen)) > 0)
    {
      buf[msglen] = '\0';
      printf ("dgrecv: got a message: %s\n", buf);
      say_who_called (&saddr);
    }
  return 0;
}

void
say_who_called (struct sockaddr_in *addrp)
{
  char host[BUFSIZ];
  int port;
  static int msgnum = 0;
  gettime ();

  get_internet_address (host, BUFSIZ, &port, addrp);
  printf (" from: %s:%d\n", host, port);
  printf ("   at: %s\n", timebuffer);
  printf (" msg#: %d\n", msgnum++);
}


int
gettime ()
{
  // Get current time in seconds since epoch
  time_t rawtime;
  time (&rawtime);

  //                 // Convert to local time structure
  struct tm *timeinfo;
  timeinfo = localtime (&rawtime);


  // Format the time - the format string creates "Sun Aug 19 10:22:27 EDT 2001"
  // %a - Abbreviated weekday name (Sun)
  // %b - Abbreviated month name (Aug)
  // %d - Day of month (19)
  // %H:%M:%S - Time in 24-hour format (10:22:27)
  // %Z - Time zone name (EDT)
  // %Y - 4-digit year (2001)
  strftime (timebuffer, 80, "%a %b %d %H:%M:%S %Z %Y", timeinfo);

  return 0;
}
