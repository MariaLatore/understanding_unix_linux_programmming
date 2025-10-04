#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

#define oops(m,x) { perror(m); exit(x); }

int
main (int ac, char *av[])
{

  if (ac != 3)
    {
      fprintf (stderr, "usage: %s <semaphore key> <semaphore num, it is not important>\n", av[0]);
      exit (1);
    }

  int sem_key = atoi (av[1]);
  int sem_num = atoi (av[2]);
  int sem_id = semget (sem_key, sem_num, 0);
  if (sem_id < 0)
    oops ("semget error", 2);

  if (semctl (sem_id, 0, IPC_RMID, NULL) < 0)
    perror ("semctl");
  return 0;
}
