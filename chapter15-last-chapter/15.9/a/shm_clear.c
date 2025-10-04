#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define TIME_MEM_KEY 99		//kind of like a port number
#define TIME_SEM_KEY 9900	//like a filename
#define SEG_SIZE ((size_t)100)	//size of segment0
int
main (int ac, char *av[])
{

  int seg_id = shmget (TIME_MEM_KEY, SEG_SIZE, 0777);
  int sem_id = semget (TIME_SEM_KEY, 2, 0777);
  if (-1 == sem_id)
    perror ("semget");

  if (-1 == shmctl (seg_id, IPC_RMID, NULL))
    perror ("shm clear shmctl");
  if (-1 == semctl (sem_id, 0, IPC_RMID, NULL))
    perror ("sem 0 clear shmctl");
  if (-1 == semctl (sem_id, 1, IPC_RMID, NULL))
    perror ("sem 1 clear shmctl");


  return 0;
}
