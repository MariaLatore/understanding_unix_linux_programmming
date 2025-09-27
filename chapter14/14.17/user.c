#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include<string.h>

#define DBFILE "/etc/passwd"

static char *dbbuf = NULL;
static size_t dbbufsize = 0;
static pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;
typedef struct
{
  char name[BUFSIZ];
  char homedir[BUFSIZ];
  char shell[BUFSIZ];
} user_t;


void
update_database ()
{
  struct stat info;
  int fd = -1;


  if (0 > stat (DBFILE, &info))
    {
      fprintf (stderr, "cannot stat file info");
      exit (1);
    }
  dbbufsize = info.st_size + 1;	//+1 for ending

  printf ("UPDATE database\n");

  pthread_mutex_lock (&lk);
  dbbuf = (char *) realloc (dbbuf, dbbufsize);
  bzero (dbbuf, dbbufsize);
  fd = open (DBFILE, O_RDONLY);
  if (read (fd, dbbuf, dbbufsize) != info.st_size)
    {
      fprintf (stderr, "read from db file error");
      exit (2);
    }

  close (fd);

  //printf("####\n%s####\n", dbbuf);
  pthread_mutex_unlock (&lk);
  return;
}



int
getuser (char *name, user_t * ru)
{
  char buf[BUFSIZ] = { 0 };
  char modbuf[BUFSIZ] = { 0 };

  FILE *fp = NULL;
  char *token;
  int i, j;

  if (NULL == ru || NULL == dbbuf)
    {
      printf ("ru:%p, dbbuf: %p\n", ru, dbbuf);
      return -1;
    }


  pthread_mutex_lock (&lk);
  fp = fmemopen (dbbuf, dbbufsize, "r");
  if (NULL == fp)
    {
      puts ("error open /etc/passwd");
      pthread_mutex_unlock (&lk);
      return -1;
    }

  for (; NULL != fgets (buf, BUFSIZ, fp);
       bzero (buf, BUFSIZ), bzero (modbuf, BUFSIZ))
    {

      for (i = 0, j = 0; buf[i + 1] != 0; i++)
	{
	  if (buf[i] == ':' && buf[i + 1] == ':')
	    {
	      modbuf[j++] = buf[i];
	      modbuf[j++] = '#';
	    }
	  else
	    modbuf[j++] = buf[i];
	}


      token = strtok (modbuf, ":");	//name
      if (NULL == token)
	continue;
      if (strcmp (name, token) != 0)
	continue;
      else
	strncpy (ru->name, name, BUFSIZ);


      strtok (NULL, ":");	//password
      strtok (NULL, ":");	//UID
      strtok (NULL, ":");	//GID
      strtok (NULL, ":");	//GECOS
      token = strtok (NULL, ":");	//directory
      if (NULL == token)
	continue;
      else
	strncpy (ru->homedir, token, BUFSIZ);
      token = strtok (NULL, ":");	//shell
      if (NULL == token)
	continue;
      else
	{
	  strncpy (ru->shell, token, BUFSIZ);
	  fclose (fp);
	  pthread_mutex_unlock (&lk);
	  return 0;
	}

    }
  fclose (fp);
  pthread_mutex_unlock (&lk);
  return -1;
}
