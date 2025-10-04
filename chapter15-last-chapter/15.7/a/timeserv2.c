#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "dgram.h"
#include "mutual.h"

void process_request (int sock, struct sockaddr_in *clntaddr);

int
main ()
{
  int sock;			//socket and connection
  int port = PORTNUM;
  sock = make_dgram_server_socket (port);
  char buf[BUFSIZ] = { 0 };
  struct sockaddr_in clntaddr;
  socklen_t clntaddrlen = sizeof (clntaddr);
  if (sock == -1)
    exit (1);

  while ((recvfrom
	  (sock, buf, BUFSIZ, 0, (struct sockaddr *) &clntaddr,
	   &clntaddrlen)) > 0)
    {
      printf ("TIME SERVER:recv from remote host\n");
      if (NULL != strstr (buf, "TIME"))
	process_request (sock, &clntaddr);	//chat with client
      else
	printf ("TIME SERVER:unknown request %s\n", buf);

      bzero (buf, BUFSIZ);

    }
  return 0;
}

void
process_request (int sock, struct sockaddr_in *clntaddr)
{
  int pipes[2];
  char buf[BUFSIZ];
  int rc;

  if (0 > pipe (pipes))
    {
      perror ("pipe create");
      exit (5);
    }

  int pid = fork ();
  switch (pid)
    {
    case -1:
      return;			//cannot proide service
    case 0:
      if (0 > dup2 (pipes[1], 1))	//child runs date
	perror ("dup2");
      close (pipes[0]);
      close (pipes[1]);
      execl ("/usr/bin/date", "date", NULL);
      puts ("execlp error");
      exit (3);			//or quits
    default:
      close (pipes[1]);
      if (0 < (rc = read (pipes[0], buf, BUFSIZ)))
	{
	  printf ("TIME SERVER: date %s\n", buf);
	  sendto (sock, buf, strlen (buf), 0,
		  (struct sockaddr *) clntaddr, sizeof (struct sockaddr_in));

	}
      else
	{
	  printf ("read return %d\n readstr %s\n", rc, buf);
	  perror ("read");
	}

      close (pipes[0]);
      waitpid (pid, NULL, 0);
      printf ("TIME SERVER:wait child process finish\n");
    }
}
