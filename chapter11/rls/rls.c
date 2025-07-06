/* rls.c - a client for a remote directory listing service
 *         usage: rls host name directory
 */
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define oops(msg) { perror(msg); exit(1); }
#define PORTNUM 15000

int
main (int ac, char *av[])
{

  struct sockaddr_in servadd;	/* the number to call */
  struct hostent *hp;		// used to get number
  int sock_id;			//the socket and fd
  char buffer[BUFSIZ];		//to receive message
  int n_read;			//for message length

  if (ac != 3)
    {
      printf ("usage: %s <hostname|hostip> <dirname>\n", av[0]);
      exit (1);
    }

/** Step 1: Get a socket **/
  sock_id = socket (AF_INET, SOCK_STREAM, 0);	//get a line
  if (sock_id == -1)
    oops ("socket");		//or faile

/** Step 2: connect to server **/
  bzero (&servadd, sizeof (servadd));	//zero the address
  hp = gethostbyname (av[1]);	//lookup host's ip #
  if (hp == NULL)
    oops (av[1]);		//or die
  bcopy (hp->h_addr, (struct sockaddr *) &servadd.sin_addr, hp->h_length);

  servadd.sin_port = htons (PORTNUM);	//fill in port num
  servadd.sin_family = AF_INET;	//fill in socket type

  if (connect (sock_id, (struct sockaddr *) &servadd, sizeof (servadd)) != 0)
    oops ("connect");

/** Step 3: send directory name, then read back results **/
  if (write (sock_id, av[2], strlen (av[2])) == -1)
    oops ("write");
  if (write (sock_id, "\n", 1) == -1)
    oops ("write");

  while ((n_read = read (sock_id, buffer, BUFSIZ)) > 0)
    if (write (1, buffer, n_read) == -1)
      oops ("write");
  close (sock_id);
  return 0;
}
