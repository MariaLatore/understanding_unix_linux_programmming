#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>

#define MAX_FILE_NUM 10
#define oops(m,x)  {perror(m); exit(x);}

struct arg_set
{				/* 2 values in one arg */
  char *fname;			/* fle to examine */
  int count;			/* number of words */
};

void *count_words (void *a);

int
main (int ac, char *av[])
{
  if (ac <= 1)
    {
      printf ("%s file1 file2 ...\n", av[0]);
      exit (1);
    }

  char **file_name = &av[1];
  int file_num = ac - 1;
  int pipes[MAX_FILE_NUM][2] = { {0} };
  int i, j, k;
  char word_count[MAX_FILE_NUM][BUFSIZ] = { {0} };
  int total_words = 0;
  struct arg_set args[MAX_FILE_NUM] = { {0} };

  for (i = 0; i < file_num; i++)
    {
      if (0 > pipe (pipes[i]))
	{
	  perror ("create pipe");
	  exit (2);
	}

      args[i].fname = file_name[i];
      args[i].count = 0;
    }
  for (i = 0; i < file_num; i++)
    {
      switch (fork ())
	{
	case -1:
	  oops ("cannot fork", 3);
	case 0:

	  for (j = 0; j < file_num; j++)
	    {
	      close (pipes[j][0]);
	      if (j != i)
		close (pipes[j][1]);
	    }
	  count_words ((void *) &args[i]);
	  if (0 >=
	      write (pipes[i][1], &args[i].count, sizeof (args[i].count)))
	    perror ("write error");
	  return 0;
	default:
	  continue;
	}
    }
  for (j = 0; j < file_num; j++)
    {
      close (pipes[j][1]);
    }

  for (j = 0; j < file_num; j++)
    {
      for (k = 0; k < sizeof (args[i].count); k++)
	if (0 > read (pipes[j][0], &word_count[j][k], 1))
	  perror ("read error");
    }

  for (j = 0; j < file_num; j++)
    {
      close (pipes[j][0]);
      wait (NULL);
    }

  for (j = 0; j < file_num; j++)
    {
      printf ("file %s: %d\n", file_name[j], *(int *) word_count[j]);
      total_words += *(int *) word_count[j];
    }

  printf ("the total words: %d\n", total_words);

  return 0;
}

void *
count_words (void *a)
{

  struct arg_set *args = a;	//cast arg back to correct type
  FILE *fp;
  int c, prevc = '\0';

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
  return NULL;
}
