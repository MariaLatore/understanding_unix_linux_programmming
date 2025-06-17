/* listchars.c
 *     purpose: list individually all the chars seen on input
 *     output:  char and ascii code, one pair per line
 *     input:   sttdin, until the letter Q
 *     notes:   useful to show that buffering/editing exists
 */

#include<stdio.h>

int
main ()
{
  int c, n = 0;
  while ((c = getchar ()) != 'Q')
    printf ("char %3d is %c code %d\n", n++, c, c);
  return 0;
}
