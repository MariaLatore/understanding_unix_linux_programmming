/* shm_tc2.c - time client shared mem ver2: use semaphores for locking
 * program uses shared memory with key 99
 * program uses semaphore set with key 9900
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <unistd.h>
#include "mutual.h"

#define oops(m,x) { perror(m); exit(x); }
void release_lock (int semset_id);
void wait_and_lock (int semset_id);
void lock_operation (int fd, int op);


int
main ()
{
  int seg_id;
  char *mem_ptr, *ctime ();
  int lockfd;


/* create a shared memory segment */
  seg_id = shmget (TIME_MEM_KEY, SEG_SIZE, 0777);

  if (seg_id == -1)
    oops ("shmget", 1);

/* attach to it and get a pointer to where it attaches */
  mem_ptr = shmat (seg_id, NULL, 0);
  if (mem_ptr == (void *) -1)
    oops ("shmat", 2);

/* connect to semaphore set 9900 with 2 semaphores */
  lockfd = open (LOCKFILE, O_RDONLY);
  if (lockfd < 0)
    oops ("open", 6);
  wait_and_lock (lockfd);

  printf ("The time, direct from memory: ..%s", mem_ptr);
  release_lock (lockfd);
  shmdt (mem_ptr);		//detach, but not needed here
  close (lockfd);
  return 0;
}

/* build and execute a 2-element actions set:
 *   wait for 0 on n_writers AND increment n_readers
 */
void
wait_and_lock (int lockfd)
{
  lock_operation (lockfd, F_RDLCK);
}

/*
 * build and execute a 1-element action set:
 *  decrement num_readers
 */
void
release_lock (int lockfd)
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
