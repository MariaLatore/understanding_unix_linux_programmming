/* twordcount4.c - threaded word counter for two files.
 *               - Version 4: condition variable allows counter
 *                            functions to report results early
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_FILE_NUM 10

struct arg_set
{				/* two values in one arg */
  char *fname;			/* file to examine */
  int count;			/* number of words */
  int id;
  struct arg_set **mailbox_p;
  pthread_mutex_t *lock_p;
  pthread_cond_t *flag_p;
};

int
main (int ac, char *av[])
{
  pthread_t t[MAX_FILE_NUM];
  struct arg_set args[MAX_FILE_NUM];
  void *count_words (void *);
  int reports_in = 0;
  int total_words = 0;
  int file_num = 0;
  char **file_name = &av[1];
  int i;
  struct arg_set *mailbox = NULL;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t flag = PTHREAD_COND_INITIALIZER;



  if (ac <= 1)
    {
      printf ("usage: %s file1 file2 ...\n", av[0]);
      exit (1);
    }
  file_num = ac - 1;
  file_name = &av[1];
  pthread_mutex_lock (&lock);	//lock the report box now

  for (i = 0; i < file_num; i++)
    {
      args[i].fname = file_name[i];
      args[i].count = 0;
      args[i].id = i;
      args[i].mailbox_p = &mailbox;
      args[i].lock_p = &lock;
      args[i].flag_p = &flag;
      pthread_create (&t[i], NULL, count_words, (void *) &args[i]);
    }

  while (reports_in < file_num)
    {
      printf ("MAIN: waiting for flag to go up\n");
      pthread_cond_wait (&flag, &lock);	//wait for notify
      printf ("MAIN: Wow! flag was raised, I have the lock\n");
      printf ("%7d: %s\n", mailbox->count, mailbox->fname);
      total_words += mailbox->count;
      for (i = 0; i < file_num; i++)
	if (mailbox == &args[i])
	  pthread_join (t[i], NULL);
      mailbox = NULL;
      pthread_cond_signal (&flag);
      reports_in++;
    }
  printf ("%7d: total words\n", total_words);
  return 0;
}

void *
count_words (void *a)
{
  struct arg_set *args = a;	//cast arg back to correct type
  FILE *fp;
  int c, prevc = '\0';
  int id = args->id;

  if ((fp = fopen (args->fname, "r")) != NULL)
    {
      while ((c = getc (fp)) != EOF)
	{
	  if (!isalnum (c) && isalnum (prevc))
	    args->count++;
	  prevc = c;
	}
      fclose (fp);
    }
  else
    perror (args->fname);
  printf ("COUNT%d: waiting to get lock\n", id);
  pthread_mutex_lock (args->lock_p);	//get the mailbox
  printf ("COUNT%d: have lock, storing data\n", id);
  if (*args->mailbox_p != NULL)
    {
      printf ("COUNT%d: mail_box not NULL, waiting main signal\n", id);
      pthread_cond_wait (args->flag_p, args->lock_p);
    }
  *args->mailbox_p = args;	//put ptr to our args there
  printf ("COUNT%d: raising flag\n", id);
  pthread_cond_signal (args->flag_p);	/* raise the flag */
  printf ("COUNT%d: unlocking box\n", id);
  pthread_mutex_unlock (args->lock_p);	/* release the mailbox */
  return NULL;
}
