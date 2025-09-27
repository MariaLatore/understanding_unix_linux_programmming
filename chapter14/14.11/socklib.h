#ifndef __SOCKLIB_H__
#define __SOCKLIB_H__

int make_server_socket (int portnum);
int make_server_socket_q (int portnum, int backlog);
int connect_to_server (char *host, int portnum);

#endif
