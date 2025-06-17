/** ls1.c
 **   purpose  list contents of dirctory or directories
 **   action   if no args, use . else list files in args
 **/
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_FILE_NUM_CAN_PRINT 4000
static struct dirent *direntp_array[MAX_FILE_NUM_CAN_PRINT];
static enum
{
  DO_LS_NO_SORT,
  DO_LS_SORT,
  DO_LS_REVERSE_SORT,
} how_do_ls = DO_LS_SORT;


void do_ls (int ac, char *av[]);

int
main (int ac, char *av[])
{
  do_ls (ac, av);
  return 0;
}

int
file_comp (const void *a, const void *b)
{
  struct dirent **file_a = (struct dirent **) a;
  struct dirent **file_b = (struct dirent **) b;
  int result = strcmp ((*file_a)->d_name, (*file_b)->d_name);
  return how_do_ls == DO_LS_REVERSE_SORT ? -result : result;
}

void
do_ls (int ac, char *av[])
/*
 * list fiels in directory called dirname
 */
{
  DIR *dir_ptr;			/*the directory */
  int index, i;
  char *optstr = "qr";
  int o;
  char *dirname;

  while (-1 != (o = getopt (ac, av, optstr)))
    {
      switch (o)
	{
	case 'q':
	  how_do_ls = DO_LS_NO_SORT;
	  break;
	case 'r':
	  how_do_ls = DO_LS_REVERSE_SORT;
	  break;
	default:
	  how_do_ls = DO_LS_SORT;
	  break;
	}
    }
  if (optind == ac)
    dirname = ".";
  else
    dirname = av[optind];

  if ((dir_ptr = opendir (dirname)) == NULL)
    fprintf (stderr, "ls1: cannot open %s\n", dirname);
  else
    {
      for (index = 0; NULL != (direntp_array[index] = readdir (dir_ptr));
	   index++);
      if (DO_LS_NO_SORT != how_do_ls)
	qsort (direntp_array, index, sizeof (*direntp_array), file_comp);
      for (i = 0; i < index; i++)
	printf ("%s\n", direntp_array[i]->d_name);
      closedir (dir_ptr);
    }
}
