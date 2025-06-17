/* ls2.c
 * 	purpose list contents of directory or directories
 * 	action  if no args, use . else list files in args
 * 	note    uses stat and pwd.h and grp.h
 *      BUG: try ls2 /tmp
 */
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_SUPPORTED_DIR_FIFO_DEPTH 256
#define MAX_FILE_NAME_LEN 256

void do_ls (char[]);
void dostat (char *dirname, char *filename);
void show_file_info (char *, struct stat *);
void mode_to_letters (int, char[]);
char *uid_to_name (uid_t);
char *gid_to_name (gid_t);
typedef struct
{
  char fifo[MAX_SUPPORTED_DIR_FIFO_DEPTH][MAX_FILE_NAME_LEN];
  int fifo_num;
  int head_index;
} dir_fifo_t;

int
dir_fifo_add (dir_fifo_t * dfifo, char *dirname)
{
  if (MAX_SUPPORTED_DIR_FIFO_DEPTH <= dfifo->fifo_num)
    {
      printf ("dir fifo size is %d, supported max dir fifo size is %d!\n",
	      dfifo->fifo_num, MAX_SUPPORTED_DIR_FIFO_DEPTH);
      return -1;
    }
  if (MAX_FILE_NAME_LEN <= strlen (dirname))
    {
      printf ("dir name %s len %ld, supported max file name %d!\n", dirname,
	      strlen (dirname), MAX_FILE_NAME_LEN);
      return -1;
    }
  int tail_index =
    (dfifo->head_index + dfifo->fifo_num) % MAX_SUPPORTED_DIR_FIFO_DEPTH;
  strcpy (dfifo->fifo[tail_index], dirname);
  dfifo->fifo_num++;
  return 0;
}

int
dir_fifo_remove (dir_fifo_t * dfifo, char buf[], int size)
{
  if (size <= strlen (dfifo->fifo[dfifo->head_index]))
    {
      printf ("buf size %d is smaller than file name %s len!\n", size,
	      dfifo->fifo[dfifo->head_index]);
      return -1;
    }
  strcpy (buf, dfifo->fifo[dfifo->head_index]);
  dfifo->head_index = (dfifo->head_index + 1) % MAX_SUPPORTED_DIR_FIFO_DEPTH;
  dfifo->fifo_num--;
  return 0;
}


void
do_ls_recurse (char dirname[])
{
  char name[MAX_FILE_NAME_LEN] = { 0 };
  char absolute_name[MAX_FILE_NAME_LEN] = { 0 };
  DIR *dirp = NULL;
  struct dirent *direntp = NULL;
  struct stat stat_info = { 0 };
  dir_fifo_t dirfifo;
  memset (&dirfifo, 0, sizeof (dirfifo));

  dir_fifo_add (&dirfifo, dirname);

  while (dirfifo.fifo_num > 0)
    {
      if (0 > dir_fifo_remove (&dirfifo, name, sizeof (name)))
	exit (1);
      do_ls (name);
      if (NULL == (dirp = opendir (name)))
	{
	  perror (name);
	  exit (1);
	}
      while (NULL != (direntp = readdir (dirp)))
	{
	  if (0 == strcmp (direntp->d_name, "."))
	    continue;
	  if (0 == strcmp (direntp->d_name, ".."))
	    continue;

	      snprintf (absolute_name, sizeof (absolute_name), "%s/%s", name,direntp->d_name);
	  if (-1 == stat (absolute_name, &stat_info))
	    {
	      perror (direntp->d_name);
	      continue;
	    }
	  if (S_ISDIR (stat_info.st_mode))
	    {
	      dir_fifo_add (&dirfifo, absolute_name);
	    }
	}
    }
}


void
do_ls (char dirname[])
/*
 * list files in directory called dirname
 */
{
  DIR *dir_ptr;			//the directory
  struct dirent *direntp;	//each entry
  if ((dir_ptr = opendir (dirname)) == NULL)
    fprintf (stderr, "ls1: cannot open %s\n", dirname);
  else
    {
      printf ("%s:\n", dirname);
      while ((direntp = readdir (dir_ptr)) != NULL)
	dostat (dirname, direntp->d_name);
      closedir (dir_ptr);
      putchar ('\n');
    }
}

void
dostat (char *dir_name, char *filename)
{
  struct stat info;
  char absolute_filename[256] = { 0 };
  snprintf (absolute_filename, sizeof (absolute_filename), "%s/%s", dir_name,
	    filename);
  if (stat (absolute_filename, &info) == -1)	//cannot stat
    perror (absolute_filename);	//say why
  else				//else show info
    show_file_info (filename, &info);
}

void
show_file_info (char *filename, struct stat *info_p)
/*
 * display the info about 'filename;, The info is stored in struct 
 * at info_p
 */
{
  char *uid_to_name (), *ctime (), *gid_to_name (), *filemode ();
  void mode_to_letters ();
  char modestr[11];
  mode_to_letters (info_p->st_mode, modestr);
  printf ("%s", modestr);
  printf ("%4d ", (int) info_p->st_nlink);
  printf ("%-8s ", uid_to_name (info_p->st_uid));
  printf ("%-8s ", gid_to_name (info_p->st_gid));
  printf ("%8ld ", (long) info_p->st_size);
  printf ("%.12s ", 4 + ctime (&info_p->st_mtime));
  printf ("%s\n", filename);
}

/*
 * utility functions
 */

/*
 * This function takes a mode value and a char array
 * and puts into the char array the file type and the
 * nine letters that correspond to the bits in mode.
 * NOTE: It does not code setuid, setgid, and sticky
 * codes
 */
void
mode_to_letters (int mode, char str[])
{
  strcpy (str, "----------");	//default=no perms
  if (S_ISDIR (mode))
    str[0] = 'd';		//directory
  if (S_ISCHR (mode))
    str[0] = 'c';		//char devices
  if (S_ISBLK (mode))
    str[0] = 'b';		//block devices
  if (mode & S_IRUSR)
    str[1] = 'r';		//3 bits for user
  if (mode & S_IWUSR)
    str[2] = 'w';
  if (mode & S_IXUSR)
    str[3] = 'x';
  if (mode & S_IRGRP)
    str[4] = 'r';		//3 bits for group
  if (mode & S_IWGRP)
    str[5] = 'w';
  if (mode & S_IXGRP)
    str[6] = 'x';
  if (mode & S_IROTH)
    str[7] = 'r';		//3 bits for othre
  if (mode & S_IWOTH)
    str[8] = 'w';
  if (mode & S_IXOTH)
    str[9] = 'x';
}

#include <pwd.h>
char *
uid_to_name (uid_t uid)
/*
 * returns pointer to username associated with uid, uses getpw()
 */
{
  struct passwd *getpwuid (), *pw_ptr;
  static char numstr[10];
  if ((pw_ptr = getpwuid (uid)) == NULL)
    {
      sprintf (numstr, "%d", uid);
      return numstr;
    }
  else
    return pw_ptr->pw_name;
}

#include <grp.h>
char *
gid_to_name (gid_t gid)
/*
 * returns pointer to group number gid. used getgrgid(3)
 */
{
  struct group *getgrgid (), *grp_ptr;
  static char numstr[10];
  if ((grp_ptr = getgrgid (gid)) == NULL)
    {
      sprintf (numstr, "%d", gid);
      return numstr;
    }
  else
    return grp_ptr->gr_name;
}


int
main (int ac, char *av[])
{
  char *opt = "R";
  int o;
  int recurse = 0;
  while (-1 != (o = getopt (ac, av, opt)))
    {
      switch (o)
	{
	case 'R':
	  recurse = 1;
	  break;
	default:
	  recurse = 0;
	  break;
	}
    }
  int para_index = optind;
  if (para_index == ac)
    {
      if (1 == recurse)
	do_ls_recurse (".");
      else
	do_ls (".");
    }
  else
    while (para_index < ac)
      {
	printf ("%s:\n", av[para_index]);
	if (1 == recurse)
	  do_ls_recurse (av[para_index]);
	else
	  do_ls (av[para_index]);
	para_index++;
      }
  return 0;
}
