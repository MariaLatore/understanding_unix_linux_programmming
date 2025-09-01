#include "csrv_funcs2.h"
#include "dgram.h"
#include <stdio.h>

#define SERVER_PORTNUM 2025
#define MSGLEN 128

int
main ()
{
  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof (client_addr);
  char buf[MSGLEN];
  int sock = setup ();
  int ret;

  while (1)
    {
      addrlen = sizeof (client_addr);
      ret = recvfrom (sock, buf, MSGLEN, 0, (struct sockaddr *) &client_addr,
		      &addrlen);

      if (ret != -1)
	{
	  buf[ret] = 0;
	  narrate ("GOT:", buf, &client_addr);
	  handle_request (buf, &client_addr, addrlen);
	}
      perror ("recvfrom");
    }
  shut_down ();
  return 0;
}
