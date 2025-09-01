#ifndef __LCNT_FUNCS1_H__
#define __LCNT_FUNCS1_H__

void license_setup ();
void shut_down ();
int get_ticket ();
int reget_ticket ();
int release_ticket ();
char *do_transaction (char *msg);
void narrate (char *msg1, char *msg2);

void syserr (char *msg1);
int validate_ticket ();

#endif
