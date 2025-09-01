/******************************************************************
 * lclnt1.c
 * License server client version 1
 * link with lclnt_funcs1.o dgram.o
 */

#include "cclnt_funcs2.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void do_regular_work();

int main(int ac, char *av[]) {
  int cn;
  if (ac != 2) {
    printf("Usage: %s <cards num>\n", av[0]);
    exit(1);
  }
  cn = (int)strtod(av[1], NULL);

  setup(cn);
  if (get_cards() != 0)
    exit(0);

  do_regular_work();
  release_cards();
  shut_down();
  return 0;
}

/******************************************************************
 * do_regular_work the main work of the application goes here
 */
void do_regular_work() {
  printf("SuperSleep version 1.0 Running - Licensed Software\n");
  sleep(5); // cur patented sleep algorithm
}
