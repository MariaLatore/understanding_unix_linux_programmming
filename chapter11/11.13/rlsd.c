/* rlsd.c - a remote ls server - with paranoia
 */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<netdb.h>
#include<time.h>
#include<string.h>
#include<ctype.h>

#include "trim.c"

#define PORTNUM 15000
#define oops(msg) {perror(msg); exit(1);}
#define HOSTLEN 1024

void
sanitize (char *str)
/*
 * it would be very bad if someone passed us an dirname like
 * "; rm *" and we naively created a command "ls ; rm *"
 *
 * so..we remove everything but slashes and alphanumerics
 * There are nicer solutions, see exercises
 */
{
  char *src, *dest;
  for (src = dest = str; *src; src++)
    if (*src == '/' || isalnum (*src))
      *dest++ = *src;
  *dest = '\0';
}

int
main (int ac, char *av[])
{
  struct sockaddr_in saddr;	//build our address here
  struct hostent *hp;		//this is part of our
  int sock_id, sock_fd;		//line id, file desc
  FILE *sock_fpi, *sock_fpo, *pipe_fp;	//streams for in and out
  char hostname[HOSTLEN];
  char dirname[BUFSIZ];		//from client
  int pipefd[2];		//for child parent communication
  pid_t pid;
  struct stat fileinfo;		//for check dirname
  int c;

/** Step 1: ask kernel for a socket **/
  sock_id = socket (PF_INET, SOCK_STREAM, 0);	// get a socket
  if (sock_id == -1)
    oops ("socket");

/** Step 2: bind address to socket. aDDRESS IS HOST, PORT **/
  bzero ((void *) &saddr, sizeof (saddr));	//clear out struct
  gethostname (hostname, HOSTLEN);	///where am I
  hp = gethostbyname (hostname);	//get info about host
  bcopy ((void *) hp->h_addr, (void *) &saddr.sin_addr, hp->h_length);
  saddr.sin_port = htons (PORTNUM);	//fill in socket port
  saddr.sin_family = AF_INET;	//fill in addr family

  if (bind (sock_id, (struct sockaddr *) &saddr, sizeof (saddr)) != 0)
    oops ("bind");

/** Step 3: allow incoming calls with Qsize=1 on socket **/
  if (listen (sock_id, 1) != 0)
    oops ("listen");

/*
 * main loop: accept(), write(), close()
 */
  while (1)
    {
      if (pipe (pipefd))
	oops ("pipe create");

      sock_fd = accept (sock_id, NULL, NULL);	//wait for call...
      if (sock_fd == -1)
	oops ("accept");

/* open reading direction as buffered stream */
      if ((sock_fpi = fdopen (sock_fd, "r")) == NULL)
	oops ("fdopen reading");

      if ((sock_fpo = fdopen (sock_fd, "w")) == NULL)
	oops ("fdopen writing");

      if (fgets (dirname, BUFSIZ - 5, sock_fpi) == NULL)
	oops ("reading dirname");
      trim_string (dirname);

      if ((pid = fork ()) == 0)
	{
	  close (pipefd[0]);
	  if (-1 == dup2 (pipefd[1], 1))
	    oops ("dup2");
	  close (pipefd[1]);
	  if (stat (dirname, &fileinfo) || !S_ISDIR (fileinfo.st_mode))
	    {
	      fprintf (stdout, "invalid dirname: %s\n", dirname);
	      return -1;
	    }
	  execlp ("ls", "ls", dirname, NULL);
	  oops ("should not run to here!");
	}
      close (pipefd[1]);

/* open writing direction as buffered stream */
      if ((pipe_fp = fdopen (pipefd[0], "r")) == NULL)
	oops ("fdopen pipe reading");

/* transfer data from ls to socket */
      while ((c = getc (pipe_fp)) != EOF)
	putc (c, sock_fpo);
      fclose (pipe_fp);
      fclose (sock_fpo);
      fclose (sock_fpi);
      wait (NULL);
    }
}
