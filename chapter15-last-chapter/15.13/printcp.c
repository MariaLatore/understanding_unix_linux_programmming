#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define oops(m,x) { perror(m); exit(x); }
#define FILE_SEM_NUM 99
void lock_operation (int fd, int operation);

int
main (int ac, char *av[])
{
  if (ac != 3)
    {
      fprintf (stderr, "usage: %s file1 file2\n", av[0]);
      exit (1);
    }


  int destfd = open (av[2], O_RDWR|O_CREAT|O_APPEND, 0777);
  int srcfd = open (av[1], O_RDONLY);
  char c;

  if (destfd < 0 || srcfd < 0)
    oops ("open", 2);
  printf ("pid %d locking file\n", getpid ());

  lock_operation (destfd, F_WRLCK);
  printf ("pid %d get the lock\n", getpid ());
  
  while (0 < read (srcfd, &c, 1))
    write (destfd, &c, 1);
  fsync (destfd);

  printf ("pid %d release lock\n", getpid ());
  lock_operation (destfd, F_UNLCK);
  close (destfd);
  close (srcfd);
  return 0;
}

void
lock_operation (int fd, int operation)
{

  struct flock lock;
  lock.l_whence = SEEK_SET;
  lock.l_start = lock.l_len = 0;
  lock.l_pid = getpid ();
  lock.l_type = operation;

  if (fcntl (fd, F_SETLKW, &lock) == -1)
    oops ("lock operation", 6);
}
