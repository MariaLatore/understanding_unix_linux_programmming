#include<stdio.h>
#include<errno.h>
#include<time.h>
#include<utime.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pthread.h>
/*
 * tries to make a link called <filename>.LCK
 * returns 0 if ok, 1 if already locked, 2 if other problem
 */
typedef char uint8_t;

static char lockfilename[] = "data.LCK";
static char filename[] = "data";


void *
task (void *sleep_sec)
{
  struct stat lock_file_info = { 0 };
  uint8_t span = (uint8_t) sleep_sec;
  struct utimbuf modifytime = { 0 };

  while (1)
    {
      sleep (span);
      if (stat (lockfilename, &lock_file_info) < 0)
	{
	  perror ("stat lock file error");
	  return NULL;
	}
      modifytime.actime = lock_file_info.st_atime;
      modifytime.modtime = time (NULL);	//time_t is long time
      if (utime (lockfilename, &modifytime) < 0)
	{
	  perror ("utime error");
	  return NULL;
	}
    }
}

int
lock_passwd (uint8_t updatespan)
{
  int rv = 0;
  if (link (filename, lockfilename) == -1)
    {
      rv = (errno == EEXIST ? 1 : 2);
      return rv;
    }
  pthread_attr_t attr;
  pthread_t id;
  if (pthread_attr_init (&attr) != 0)
    {
      perror ("pthread_attr_init error");
      return -1;
    }
  if (pthread_create (&id, &attr, task, (void *) updatespan) != 0)
    {
      perror ("pthread_create error");
      return -1;
    }
  return rv;
}


int
main ()
{
  int rc = lock_passwd (1);
  if (0 == rc)
    while (1);
  else
    perror ("lock_passwd error");
  return 0;
}
