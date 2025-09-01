#ifndef __DGRAM_H__
#define __DGRAM_H__

#include <netinet/in.h>

int make_internet_address ();
int make_dgram_server_socket (int portnum);
int make_dgram_client_socket ();
int
make_internet_address (char *hostname, int port, struct sockaddr_in *addrp);
int
get_internet_address (char *host, int len, int *portp,
		      struct sockaddr_in *addrp);

#endif
