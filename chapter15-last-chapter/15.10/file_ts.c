/* file_ts.c - write the current date/time to a file
 *  usage: file_ts filename
 *  action: writes the current time/date to filename
 *  note: uses fcntl() - based locking
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/sem.h>
#include "mutual.h"
#define oops(m,x) { perror(m); exit(x); }
#define BUFLEN 10

int sem_id;
void set_sem_value (int sem_id, int semnum, int val);
void lock_operation (int fd, int op);
union semun
{
  int val;
  struct semid_ds *buf;
  ushort *array;
};



int
main (int ac, char *av[])
{

  int fd;
  time_t now;
  char *message;

  if (ac != 2)
    {
      fprintf (stderr, "usage: %s <file name>\n", av[0]);
      exit (-1);
    }

  /* 
   * sem 0 is n_readers
   * sem 1 is n_writers
   */
  sem_id = semget (TIME_SEM_KEY, 2, IPC_CREAT | IPC_EXCL | 0777);
  if (0 > sem_id)
    oops ("semget", 1);
  set_sem_value (sem_id, 0, 0);
  set_sem_value (sem_id, 1, 0);

  if ((fd = open (av[1], O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1)
    oops (av[1], 2);
  while (1)
    {
      time (&now);		//compute time
      message = ctime (&now);	//lock for writing

      printf ("SERVER: write lock\n");
      lock_operation (fd, F_WRLCK);

      if (lseek (fd, 0L, SEEK_SET) == -1)
	oops ("lseek", 3);
      if (write (fd, message, strlen (message)) == -1)
	oops ("write", 4);

      printf ("SERVER: wait 4 seconds to write\n");
      sleep (4);

      printf ("SERVER: write unlock\n");
      lock_operation (fd, F_UNLCK);	//unlock file
      sleep (1);		//wait for new time
    }
  return 0;
}


void
set_sem_value (int sem_id, int semnum, int val)
{
  union semun initval;
  initval.val = val;
  if (semctl (sem_id, semnum, SETVAL, initval) == -1)
    oops ("semctl", 4);
}

void
lock_operation (int fd, int op)
{
  struct sembuf actions[2] = {
    {0}
  };
  switch (op)
    {
    case F_WRLCK:
      actions[0].sem_num = 0;
      actions[0].sem_flg = SEM_UNDO;
      actions[0].sem_op = 0;

      actions[1].sem_num = 1;
      actions[1].sem_flg = SEM_UNDO;
      actions[1].sem_op = +1;
      if (semop (sem_id, actions, 2) == -1)
	oops ("semop: locking", 10);
      break;
    case F_UNLCK:
      actions[0].sem_num = 1;
      actions[0].sem_flg = SEM_UNDO;
      actions[0].sem_op = -1;
      if (semop (sem_id, actions, 1) == -1)
	oops ("semop: locking", 10);
      break;
    }
  return;

}
