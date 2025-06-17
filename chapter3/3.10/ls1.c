/** ls1.c
 **   purpose  list contents of dirctory or directories
 **   action   if no args, use . else list files in args
 **/
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_FILE_IN_DIR 4000
static struct dirent *direntp_array[MAX_FILE_IN_DIR];
static int direntp_arrsize = 0;

void do_ls (char[]);
static void do_column_print ();

int
main (int ac, char *av[])
{
  if (ac == 1)
    do_ls (".");
  else
    while (--ac)
      {
	printf ("%s:\n", *++av);
	do_ls (*av);
      }
  return 0;
}

void
do_ls (char dirname[])
/*
 * list fiels in directory called dirname
 */
{
  DIR *dir_ptr;			/*the directory */

  if ((dir_ptr = opendir (dirname)) == NULL)
    fprintf (stderr, "ls1: cannot open %s\n", dirname);
  else
    {
      for (direntp_arrsize = 0;
	   (direntp_array[direntp_arrsize] = readdir (dir_ptr)) != NULL;
	   direntp_arrsize++);
      do_column_print ();
      closedir (dir_ptr);
    }
}

static int
file_name_cmp (const void *a, const void *b)
{
  struct dirent **direntppa = (struct dirent **) a;
  struct dirent **direntppb = (struct dirent **) b;
  return strcmp ((*direntppa)->d_name, (*direntppb)->d_name);
}

static void
do_column_print ()
{
  //sort the file array
  qsort (direntp_array, direntp_arrsize, sizeof (*direntp_array),
	 file_name_cmp);

  //get the current window size
  struct winsize w;
  ioctl (STDOUT_FILENO, TIOCGWINSZ, &w);

  //get the max filenmae size
  int i;
  int max_filename_len = 0;
  for (i = 0; i < direntp_arrsize; i++)
    {
      if (strlen (direntp_array[i]->d_name) > max_filename_len)
	max_filename_len = strlen (direntp_array[i]->d_name);
    }

  //calculate the column num needed
  int column_num = w.ws_col / (max_filename_len + 2);

  //calculate the row num needed
  int row_num = direntp_arrsize / column_num;
  int tail = direntp_arrsize % column_num;
  row_num = tail == 0 ? row_num : row_num + 1;

  //print the files
  int row, col;
  int file_idx;
  for (row = 1; row <= row_num; row++)
    {
      for (col = 1; col <= column_num; col++)
	{
	  file_idx = 0;
	  int j;
	  for (j = 1; j < col; j++)
	    {

	      file_idx += j <= tail ? row_num : row_num - 1;
	    }
	  file_idx += row;
	  file_idx -= 1;
	  if (file_idx < direntp_arrsize)
	    for (i = 0; i < max_filename_len + 2; i++)
	      {
		if (i < strlen (direntp_array[file_idx]->d_name))
		  putchar (direntp_array[file_idx]->d_name[i]);
		else if (i == strlen (direntp_array[file_idx]->d_name))
		  putchar (0);
		else
		  putchar (' ');
	      }
	}
      putchar('\n');
    }
}
