/* hello_multi.c - a multi-threaded hello world program */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define NUM 5

int
main ()
{

  pthread_t t1, t2, t3, t4;		//two threads

  void *print_msg (void *);

  pthread_create (&t1, NULL, print_msg, (void *) "hello");
  pthread_create (&t2, NULL, print_msg, (void *) "world\n");
  pthread_create (&t3, NULL, print_msg, (void *) "goodbye");
  pthread_create (&t4, NULL, print_msg, (void *) "world\n");
  pthread_join (t1, NULL);
  puts("t1 is joined");
  pthread_join (t2, NULL);
  puts("t2 is joined");
  pthread_join (t3, NULL);
  puts("t3 is joined");
  pthread_join (t4, NULL);
  puts("t4 is joined");
 
  return 0;
}

void *
print_msg (void *m)
{
  char *cp = (char *) m;
  int i;
  for (i = 0; i < strlen(cp); i++)
    {
      printf ("%s", cp);
      fflush (stdout);
      sleep (1);
    }
  return NULL;
}
