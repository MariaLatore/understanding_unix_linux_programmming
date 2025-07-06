#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define PORTNUM 15000

int
main ()
{
  int servsock, clntsock;
  struct sockaddr_in servaddr, clntaddr;
  int opt;
  socklen_t clntaddrlen, optlen;
  int num = 0;
  char buf[BUFSIZ];

  servsock = socket (PF_INET, SOCK_STREAM, 0);
  if (-1 == servsock)
    {
      perror ("socket error");
      exit (1);
    }
  opt = 1;
  optlen = sizeof (opt);
  setsockopt (servsock, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, optlen);

  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  servaddr.sin_port = htons (PORTNUM);
  servaddr.sin_family = AF_INET;

  if (-1 == bind (servsock, (struct sockaddr *) &servaddr, sizeof (servaddr)))
    {
      perror ("bind error");
      exit (2);
    }

  if (-1 == listen (servsock, 1))
    {
      perror ("listen error");
      exit (3);
    }

  clntaddrlen = sizeof (clntaddr);
  while (1)
    {

      clntsock =
	accept (servsock, (struct sockaddr *) &clntaddr, &clntaddrlen);
      if (-1 == clntsock)
	{
	  perror ("accept error");
	  exit (4);
	}

      snprintf (buf, BUFSIZ, "##############the number to beserved is %d##################\n", num);
      write (clntsock, buf, strlen (buf));
      close (clntsock);
      num++;
    }
  close (servsock);
  return 0;
}
