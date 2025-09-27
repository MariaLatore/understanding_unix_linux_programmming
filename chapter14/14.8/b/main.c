#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_FILE_NUM 10
#define oops(m,x)  {perror(m); exit(x);}

void *count_words (void *);
struct arg_set
{				/* 2 values in one arg */
  char *fname;			/* fle to examine */
  int count;			/* number of words */
};

int
main (int ac, char *av[])
{
  struct arg_set args[MAX_FILE_NUM];
  char **file_name = &av[1];
  int file_num = ac - 1;
  int i;
  int total_words = 0;

  if (ac <= 1)
    {
      printf ("usage: %s file1 file2 ...\n", av[0]);
      exit (1);
    }

  for (i = 0; i < file_num; i++)
    {
      args[i].fname = file_name[i];
      args[i].count = 0;
    }

  for (i = 0; i < file_num; i++)
    count_words ((void *) &args[i]);

  for (i = 0; i < file_num; i++)
    {
      printf ("file %s: %d\n", args[i].fname, args[i].count);
      total_words += args[i].count;
    }

  printf ("total words: %d\n", total_words);
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
