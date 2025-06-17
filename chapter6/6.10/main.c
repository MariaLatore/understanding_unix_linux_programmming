#include<stdio.h>
#include<signal.h>

#define TRUE 1
#define FALSE 0

static unsigned long max_prime_calculated = 0;
static unsigned long num_calculated = 0;


void
f (int signum)
{
  printf ("max prime calculated %lu, max num calculated %lu\n",
	  max_prime_calculated, num_calculated);
}

int
prime_or_not (unsigned long num)
{
  unsigned long i;
  if (2 == num || 0 == num || 1 == num)
    return FALSE;
  for (i = 2; i < num; i++)
    if (num % i == 0)
      return FALSE;
  return TRUE;
}



int
main ()
{
  signal (SIGINT, f);
  for (num_calculated = 0; num_calculated < (unsigned long) -1;
       num_calculated++)
    {
      if (TRUE == prime_or_not (num_calculated))
	max_prime_calculated = num_calculated;
    }
  return 0;
}
