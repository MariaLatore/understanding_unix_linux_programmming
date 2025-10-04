/* file_tc.c - read the current date/time from a file
 * usage: file_tc filename
 * uses: fcntl()-based locking
 */

#include <stdio.h>
#include <sys/file.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "mutual.h"

#define oops(m, x) { perror(m); exit(x); }
#define BUFLEN 10


void lock_operation (int fd, int op);

int semset_id;


int
main (int ac, char *av[])
{
  int fd, nread;
  char buf[BUFLEN];

  /* 
   * sem 0 is n_readers
   * sem 1 is n_writers
   */
  semset_id = semget (TIME_SEM_KEY, 2, 0);
  if (0 > semset_id)
    oops ("clnt semget", 4);

  if (ac != 2)
    {
      fprintf (stderr, "usage: file_tc filename\n");
      exit (1);
    }

  if ((fd = open (av[1], O_RDONLY)) == -1)
    oops (av[1], 3);

  lock_operation (fd, F_RDLCK);
  while ((nread = read (fd, buf, BUFLEN)) > 0)
    write (1, buf, nread);
  printf ("CLIENT: wait 5 seconds before exit\n");
  sleep (5);
  lock_operation (fd, F_UNLCK);
  close (fd);
  return 0;
}


void
lock_operation (int fd, int op)
{
  struct sembuf actions[2] = { {0} };
  switch (op)
    {
    case F_RDLCK:
      actions[0].sem_num = 1;
      actions[0].sem_flg = SEM_UNDO;
      actions[0].sem_op = 0;

      actions[1].sem_num = 0;
      actions[1].sem_flg = SEM_UNDO;
      actions[1].sem_op = +1;
      if (semop (semset_id, actions, 2) == -1)
	oops ("semop: locking", 10);
      break;
    case F_UNLCK:
      actions[0].sem_num = 0;
      actions[0].sem_flg = SEM_UNDO;
      actions[0].sem_op = -1;
      if (semop (semset_id, actions, 1) == -1)
	oops ("semop: locking", 10);
      break;
    }
  return;

}
