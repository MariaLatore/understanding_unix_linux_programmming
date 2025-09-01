/******************************************************************
 * logfilec.c - logfile client - send messages to the logfile server
 *              usage: logfilec "a message here"
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>

#define SOCKET "/tmp/logfilesock"
#define oops(m,x) { perror(m); exit(x); }
#define USERNAMELEN 50
#define MAXMSGLEN 512

int
get_user (char user[], int buflen)
{
  int euid = geteuid ();
  char euid_str[32];
  int passwd_fd = open ("/etc/passwd", O_RDONLY);
  if (0 > passwd_fd)
    {
      puts ("open file error\n");
      exit (1);
    }
  char line_buf[256];
  int i = 0;
  char *token;
  char *delim = ":";
  int rc = -1;

  sprintf (euid_str, "%d", euid);
  while (0 < (rc = read (passwd_fd, &line_buf[i], 1)))
    {
      if ('\n' == line_buf[i])
	{
	  line_buf[++i] = 0;
	  if (NULL != strstr (line_buf, euid_str))
	    {
	      token = strtok (line_buf, delim);
	      strncpy (user, token, buflen);
	      return 0;
	    }
	  i = 0;
	}
      else
	i++;
    }
  return 1;
}


int
main (int ac, char *av[])
{
  int sock;
  struct sockaddr_un addr;
  socklen_t addrlen;
  char sockname[] = SOCKET;
  char *usrmsg = av[1];
  char username[USERNAMELEN];
  char msg[MAXMSGLEN];


  if (ac != 2)
    {
      fprintf (stderr, "usage: logfilec 'message'\n");
      exit (1);
    }
  sock = socket (PF_UNIX, SOCK_DGRAM, 0);
  if (sock == -1)
    oops ("socket", 2);

  addr.sun_family = AF_UNIX;
  strcpy (addr.sun_path, sockname);
  addrlen = strlen (sockname) + sizeof (addr.sun_family);
  if (get_user (username, USERNAMELEN))
    {
      fprintf (stderr, "cannot get username\n");
      exit (1);
    }

  snprintf (msg, MAXMSGLEN, "[%s] %s", username, usrmsg);

  if (sendto (sock, msg, strlen (msg), 0, (struct sockaddr *) &addr, addrlen)
      == -1)
    oops ("sendto", 3);
  return 0;
}
