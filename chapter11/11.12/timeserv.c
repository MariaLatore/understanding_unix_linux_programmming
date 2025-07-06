/* timeserv.c - a socket-based of day server */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"trim.c"

#define PORTNUM 13000		/* our time service phone number */
#define HOSTLEN 256
#define oops(msg)  { perror(msg); exit(1);}
#define MAX_VALID_IP 200
#define VALID_IP_FILE "ip.conf"

char valid_ip[MAX_VALID_IP][BUFSIZ] = { {0} };

int valid_ip_num = 0;

void
setup ()
{

  FILE *fp = fopen (VALID_IP_FILE, "r");
  int i;
  if (NULL == fp)
    {
      fprintf (stderr, "error: no ip.conf file");
      exit (-11);
    }

  for (i = 0; fgets (valid_ip[i], BUFSIZ, fp) != NULL; i++)
    {
      trim_string (valid_ip[i]);
      printf ("valid ip: %s\n", valid_ip[i]);
    }
  valid_ip_num = i;
  fclose (fp);
}


int
main (int ac, char *av[])
{
  struct sockaddr_in saddr;	//build our address here
  struct hostent *hp;		//this is part of our
  char hostname[HOSTLEN];	//address
  int sock_id, sock_fd;		//line id, file desc
  FILE *sock_fp;		//use socket as stream
  char *ctime ();		//convert secs to string
  time_t thetime;		//the time we report
/*
 * Step 0: setup
 */
  setup ();
  /*
   * Step 1: ask kernel for a socket
   */
  sock_id = socket (PF_INET, SOCK_STREAM, 0);	//get a socket
  if (sock_id == -1)
    oops ("socket");

  /*
   * Step 2: bind address to socket, Address is host,port
   */
  bzero ((void *) &saddr, sizeof (saddr));	//clear out struct
  gethostname (hostname, HOSTLEN);	//where am I
  printf ("host is %s\n", hostname);
  hp = gethostbyname (hostname);	//get info about host
  //fill in host part
  bcopy ((void *) hp->h_addr, (void *) &saddr.sin_addr, hp->h_length);
  saddr.sin_port = htons (PORTNUM);	//fill in socket port

  saddr.sin_family = AF_INET;	//fill in addr family

  if (bind (sock_id, (struct sockaddr *) &saddr, sizeof (saddr)) != 0)
    oops ("bind");

  /*
   * Step 3: allow incoming calls with Qsize = 1 on socket
   */

  if (listen (sock_id, 1) != 0)
    oops ("listen");

  /*
   * main loop: accetp(), write(), close()
   */

  struct sockaddr_in peeraddr = { 0 };
  socklen_t clnt_addr_len = 0;
  while (1)
    {
      sock_fd = accept (sock_id, (struct sockaddr *) &peeraddr, &clnt_addr_len);	/* wait for call */
      char *peeripaddr = inet_ntoa (peeraddr.sin_addr);
      int i;
      for (i = 0; i < valid_ip_num; i++)
	{
	  if (strcmp (peeripaddr, valid_ip[i]) == 0)
	    break;
	}

      if (i == valid_ip_num)
	{
	  close (sock_fd);
	  continue;
	}
      printf ("Wow! got a call from  %s!\n", peeripaddr);
      if (sock_fd == -1)
	oops ("accept");	//error get calls
      sock_fp = fdopen (sock_fd, "w");	//we'll write to the
      if (sock_fp == NULL)	//socket as astream
	oops ("fdopen");	//unless we can't
      thetime = time (NULL);	//get time and convert to string
      fprintf (sock_fp, "The time here is ..");
      fprintf (sock_fp, "%s", ctime (&thetime));
      fclose (sock_fp);		//release connection
    }
  return 0;
}
