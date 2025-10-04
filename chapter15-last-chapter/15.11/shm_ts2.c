/* shm_ts2.c - time server shared mem ver2 : use semaphores for locking
 * program uses shared memory with key 99
 * program uses semaphore set with key 9900
 */

#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include "mutual.h"

#define oops(m,x) { perror(m); exit(x); }

union semun
{
  int val;
  struct semid_ds *buf;
  ushort *array;
};
int seg_id;			//global for cleanup()
int lockfd;
void cleanup (int);
void wait_and_lock (int semset_id);
void release_lock (int semset_id);
void lock_operation (int fd, int op);


int
main ()
{
  char *mem_ptr, *ctime ();
  time_t now;
  int n;

/* create a shared memory segment */

  seg_id = shmget (TIME_MEM_KEY, SEG_SIZE, IPC_CREAT | 0777);
  if (seg_id == -1)
    oops ("shmget1", 1);

/* attach to it and get a pointer to where it attaches */
  mem_ptr = shmat (seg_id, NULL, 0);
  if (mem_ptr == (void *) -1)
    oops ("shmat", 2);

/* create a semset: key 9900, 2 semaphores, and mode rw-rw-rw */
  lockfd = open (LOCKFILE, O_WRONLY);
  if (lockfd < 0)
    oops ("open", 3);

  signal (SIGINT, cleanup);

/* run for a minute */
  for (n = 0; n < 60; n++)
    {
      time (&now);		//get the time
      printf ("\tshm_ts2 waiting for lock\n");
      wait_and_lock (lockfd);	//lock memory
      printf ("\tshm_ts2 updating memory\n");
      strcpy (mem_ptr, ctime (&now));	//write to mem
      sleep (5);
      release_lock (lockfd);	//unlock
      printf ("\tshm_ts2 released lock\n");
      sleep (1);		//wati a sec
    }

  cleanup (0);
  return 0;
}

void
cleanup (int n)
{
  shmctl (seg_id, IPC_RMID, NULL);	//rm shrd mem
  close (lockfd);
}

/*
 * build and execute a 2-element action set:
 *   wait for 0 on n_readers AND increment n_writers
 */
void
wait_and_lock (int semset_id)
{
  lock_operation (lockfd, F_WRLCK);
}

/*
 * build and execute a 1-element actions set:
 *   decrement num_writers
 */
void
release_lock (int semset_id)
{
  lock_operation (lockfd, F_UNLCK);
}


void
lock_operation (int fd, int op)
{
  struct flock lock;
  lock.l_whence = SEEK_SET;
  lock.l_start = lock.l_len = 0;
  lock.l_pid = getpid ();
  lock.l_type = op;

  if (fcntl (fd, F_SETLKW, &lock) == -1)
    oops ("lock operation", 6);
}
