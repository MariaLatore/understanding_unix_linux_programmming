#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include "user.c"
#include "trim.c"
#define PORT 15000

int
main ()
{
  struct sockaddr_in servaddr;
  struct sockaddr_in clntaddr;
  socklen_t clnt_addr_size, optlen;
  int serv_sock;
  int clnt_sock;
  int option;
  char buffer[BUFSIZ];
  user_t userinfo = { {0} };

  serv_sock = socket (AF_INET, SOCK_STREAM, 0);
  if (-1 == serv_sock)
    {
      perror ("socket error");
      exit (-1);
    }
  optlen = sizeof (option);
  option = 1;
  setsockopt (serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *) &option, optlen);

  bzero (&servaddr, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  servaddr.sin_port = htons (PORT);

  if (-1 ==
      bind (serv_sock, (struct sockaddr *) &servaddr, sizeof (servaddr)))
    {
      perror ("bind  error");
      exit (-1);
    }

  if (listen (serv_sock, 1))
    {
      perror ("listen");
      exit (-1);
    }

  clnt_addr_size = sizeof (clntaddr);
  clnt_sock =
    accept (serv_sock, (struct sockaddr *) &clntaddr, &clnt_addr_size);
  FILE *sock_fp = fdopen (clnt_sock, "r+");
  if (NULL == sock_fp)
    {
      perror ("fdopen");
      exit (-1);
    }

  if (fgets (buffer, BUFSIZ, sock_fp) == NULL)
    {
      puts ("cannot get message");
      exit (-1);
    }
  else
    trim_string (buffer);

  if (getuser (buffer, &userinfo))
    {
      fputs ("cannot find the user\n", sock_fp);
    }
  else
    {
      snprintf (buffer, BUFSIZ, "user name %s, homedir %s, shell %s\n",
		userinfo.name, userinfo.homedir, userinfo.shell);
      fputs (buffer, sock_fp);
    }
  //client will read and wait until server close socket
  //so after run, it will enter time-wait phase, cannot bind to the same port again
  fclose (sock_fp);
  close (serv_sock);
  return 0;
}
