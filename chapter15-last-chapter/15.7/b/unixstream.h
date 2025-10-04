#ifndef __DGRAM_H__
#define __DGRAM_H__

#include <sys/un.h>
int make_unix_stream_server_socket (char *sockname);
int make_unix_stream_server_socket_q (char *sockname, int backlog);
int make_unix_stream_address (char *sockname, struct sockaddr_un *addrp);
int connect_to_unix_stream_server (char *destsockname);

#endif
