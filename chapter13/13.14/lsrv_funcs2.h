#ifndef __LSRV_FUNCS1_H__
#define __LSRV_FUNCS1_H__

#define RECLAIM_INTERVAL 5	// reclaim every 60 seconds
int setup ();

void free_all_tickets ();

void shut_down ();
void handle_request (char *req, struct sockaddr_in *client, socklen_t addlen);
char *do_hello (char *msg_p);
char *do_goodbye (char *msg_p);
void narrate (char *msg1, char *msg2, struct sockaddr_in *clientp);

void ticket_reclaim ();
#endif
