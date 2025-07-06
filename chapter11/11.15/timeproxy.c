#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define TCP_TIME_PORT 37
#define PORTNUM 13000		/* our time service phone number */
#define oops(msg) {perror(msg); exit(1);}

int
main ()
{

  int proxysock, clntsock, servsock;
  struct sockaddr_in proxyaddr = { 0 }, clntaddr =
  {
  0}, servaddr =
  {
  0};				//build our address here
  socklen_t clntaddrlen, optionlen;
  char c;
  int option;

  proxysock = socket (PF_INET, SOCK_STREAM, 0);
  if (-1 == proxysock)
    oops ("porxy sock error");
  optionlen = sizeof (option);
  option = 1;
  setsockopt (proxysock, SOL_SOCKET, SO_REUSEADDR, (void *) &option,
	      optionlen);

  proxyaddr.sin_family = AF_INET;
  proxyaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  proxyaddr.sin_port = htons (TCP_TIME_PORT);
  if (bind (proxysock, (struct sockaddr *) &proxyaddr, sizeof (proxyaddr)))

    oops ("proxy bind error");
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
  servaddr.sin_port = htons (PORTNUM);

  if (listen (proxysock, 1) == -1)
    oops ("proxy listen error");

  while (1)
    {
      clntaddrlen = sizeof (clntaddr);
      clntsock =
	accept (proxysock, (struct sockaddr *) &clntaddr, &clntaddrlen);
      if (-1 == clntsock)
	oops ("accept error");

      servsock = socket (PF_INET, SOCK_STREAM, 0);
      if (-1 == servsock)
	oops ("serv sock error");

      if (connect (servsock, (struct sockaddr *) &servaddr, sizeof (servaddr))
	  == -1)
	oops ("connect serv addr");

      while (read (servsock, &c, sizeof (c)))
	write (clntsock, &c, sizeof (c));
      close (servsock);
      close (clntsock);
    }



  return 0;
}
