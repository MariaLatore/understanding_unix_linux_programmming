#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_FILE_NUM 10
#define oops(m,x)  {perror(m); exit(x);}

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
  FILE *readfps[MAX_FILE_NUM] = { NULL };
  int i, j, k;
  char word_count[MAX_FILE_NUM][BUFSIZ] = { {0} };
  int total_words = 0;

  for (i = 0; i < file_num; i++)
    {
      if (0 > pipe (pipes[i]))
	{
	  perror ("create pipe");
	  exit (2);
	}
    }
  for (i = 0; i < file_num; i++)
    {
      switch (fork ())
	{
	case -1:
	  oops ("cannot fork", 3);
	case 0:
	  if (dup2 (pipes[i][1], 1) == -1)
	    oops ("could not redirect stdout", -5);

	  for (j = 0; j < file_num; j++)
	    {
	      close (pipes[j][0]);
	      close (pipes[j][1]);
	    }
	  execlp ("wc", "wc", "-w", file_name[i], NULL);
	  oops ("should not be here!", -1);
	default:
	  continue;
	}
    }
  for (j = 0; j < file_num; j++)
    {
      close (pipes[j][1]);
      readfps[j] = fdopen (pipes[j][0], "r");
      if (NULL == readfps[j])
	oops ("fdopen error", -2);
    }

  for (j = 0, k = 0; j < file_num; j++, k = 0)
    {
      while (EOF != (word_count[j][k++] = fgetc (readfps[j])));
      for (; k >= 0; k--)
	{
	  if (isalnum (word_count[j][k]))
	    break;
	}
      word_count[j][k + 1] = 0;
    }

  for (j = 0; j < file_num; j++)
    fclose (readfps[j]);

  for (j = 0; j < file_num; j++)
    {
      printf ("file %s: %s\n", file_name[j], word_count[j]);
      total_words += (int) strtol (word_count[j], NULL, 0);
    }

  printf ("the total words: %d\n", total_words);



  return 0;
}
