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
void process_request (int fd);

int
main ()
{
  int sock;			//socket and connection
  int port = PORTNUM;
  sock = make_dgram_server_socket (port);
  char buf[BUFSIZ] = { 0 };
  struct sockaddr_in clntaddr;
  socklen_t clntaddrlen = sizeof (clntaddr);
  struct sockaddr_in zero_addr;
  socklen_t zeroaddrlen = sizeof (zero_addr);
  // 初始化全零地址
  memset (&zero_addr, 0, sizeof (zero_addr));
  zero_addr.sin_family = AF_INET;	// IPv4
  zero_addr.sin_addr.s_addr = htonl (INADDR_ANY);	// 全零 IP（任意地址）
  zero_addr.sin_port = htons (0);	// 0 端口（无效端口）
  if (sock == -1)
    exit (1);

  while ((recvfrom
	  (sock, buf, BUFSIZ, 0, (struct sockaddr *) &clntaddr,
	   &clntaddrlen)) > 0)
    {
      printf ("TIME SERVER:recv from remote host\n");
      if (NULL != strstr (buf, "TIME"))
	{
	  if (0 > connect (sock, (struct sockaddr *) &clntaddr, clntaddrlen))
	    perror ("connect");
	  process_request (sock);	//chat with client
	}
      else
	printf ("TIME SERVER:unknown request %s\n", buf);

      bzero (buf, BUFSIZ);
      //below connect is critical, or the previous clnt addr is closed by clnt
      //and server will never read anything again
      connect (sock, (struct sockaddr *) &zero_addr, zeroaddrlen);

    }
  return 0;
}

void
process_request (int fd)
{
  int pid = fork ();
  switch (pid)
    {
    case -1:
      return;			//cannot proide service
    case 0:
      dup2 (fd, 1);		//child runs date
      close (fd);
      execl ("/bin/date", "date", NULL);
      puts ("execlp error");
      exit (3);			//or quits
    default:
      waitpid (pid, NULL, 0);	//parent wait for child
      printf ("TIME SERVER:wait child process finish\n");
      return;
    }
}
