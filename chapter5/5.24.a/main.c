#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include<fcntl.h>

void *
owf (void *param)
{
  int id = *(int *) param;
  char *tstr;
  char *buf;
  if (1 == id)
    {
      tstr = "1";
      buf = "where is my Mrs.Right?\n";
    }
  else
    {
      tstr = "2";
      buf = "where is my Mr.Right?\n";
    }

  int fd = open ("data", O_WRONLY);
  if (0 > lseek (fd, 0, SEEK_END))
    fprintf (stderr, "lseek err %s\n", tstr);
  if (0 > write (fd, buf, strlen (buf)))
    fprintf (stderr, "fwrite err %s\n", tstr);

  if (0 > close (fd))
    fprintf (stderr, "close err %s\n", tstr);
  return NULL;
}



int
main ()
{
  pthread_t thread1, thread2;
  int id1 = 1, id2 = 2;
  void *res1, *res2;
  pthread_create (&thread1, NULL, &owf, &id1);
  pthread_create (&thread2, NULL, &owf, &id2);
  pthread_join (thread1, &res1);
  pthread_join (thread2, &res2);
  printf ("the result1 %s\n", (char *) res1);
  printf ("the result2 %s\n", (char *) res2);
  return 0;
}
