#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <string.h>
#include "dgram.h"


#define oops(m,x) { perror(m); exit(x); }

int make_dgram_server_socket (int);
int get_internet_address (char *, int, int *, struct sockaddr_in *);
void
animate_request (char *buf, int sock, struct sockaddr_in *clntaddr,
		 socklen_t clntaddrlen);
int
main (int ac, char *av[])
{
  int port;
  int sock;
  char buf[BUFSIZ];
  size_t msglen;
  struct sockaddr_in saddr;
  socklen_t saddrlen;

  if (ac == 1 || (port = atoi (av[1])) <= 0)
    {
      fprintf (stderr, "usage: %s <portnum>\n", av[0]);
      exit (1);
    }

  if ((sock = make_dgram_server_socket (port)) == -1)
    oops ("cannot make socket", 2);
/* set up curses */
  initscr ();
  crmode ();
  noecho ();
  clear ();
  move (LINES - 1, COLS - 1);
  refresh ();


  saddrlen = sizeof (saddr);
  while ((msglen =
	  recvfrom (sock, buf, BUFSIZ, 0, (struct sockaddr *) &saddr,
		    &saddrlen)) > 0)
    {
      buf[msglen] = 0;
      animate_request (buf, sock, &saddr, saddrlen);
    }
  endwin ();
  return 0;
}

void
animate_request (char *buf, int sock, struct sockaddr_in *clntaddr,
		 socklen_t clntaddrlen)
{
  char *del = " ";
  char *probe = NULL;
  char bufbak[BUFSIZ];
  char *next_pos;
  strcpy (bufbak, buf);
  next_pos = bufbak;

/* according to the man page, strtok has 3 pointers:
 *   search_start_point: it is the first byte when first call, and the byte after token_end in the following call.
 *   token_start: it search from search_start_point and set as the first non-delimeter byte position.
 *   token_end: it search from token_start and end at the first delimeter byte.
 */
  probe = strtok (next_pos, del);
  next_pos += strlen (probe) + 1;
  if (strcmp (probe, "CLEAR") == 0)
    {
      clear ();
      move (LINES - 1, COLS - 1);
      refresh ();
    }
  else if (strcmp (probe, "DRAW") == 0)
    {
      char *rowstr;
      char *colstr;
      char *dispstr;
      int row;
      int col;
      rowstr = strtok (next_pos, del);
      row = atoi (rowstr);
      if (rowstr == NULL || 0 > (row = atoi (rowstr)))
	{
	  fprintf (stderr, "dgserv: row parse error %s\n", buf);
	  return;
	}
      next_pos += strlen (rowstr) + 1;
      colstr = strtok (next_pos, del);
      if (colstr == NULL || 0 > (col = atoi (colstr)))
	{
	  fprintf (stderr, "dgserv: row parse error %s\n", buf);
	  return;
	}
      dispstr = next_pos + strlen (colstr) + 1;
      mvaddstr (row, col, dispstr);

      move (LINES - 1, COLS - 1);
      refresh ();
    }
  else if (strcmp (probe, "GETWINSIZE") == 0)
    {
      char msgbuf[BUFSIZ];
      snprintf (msgbuf, BUFSIZ, "%d %d", LINES, COLS);
      if (0 >
	  sendto (sock, msgbuf, strlen (msgbuf), 0,
		  (struct sockaddr *) clntaddr, clntaddrlen))
	{
	  fprintf (stderr, "dgserv: send window msg error %s\n", msgbuf);
	  return;
	}
    }
  else
    fprintf (stderr, "dgrecv: invalid request %s\n", buf);
}
