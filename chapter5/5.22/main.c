/* write0.c
 *
 *    purpose: send messages to another terminal
 *    method: open the other terminal for output then
 *            copy from stdin to that terminal
 *    shows: a terminal is just a file supporting regulart i/o
 *    usage: write0 ttyname
 */

#include<stdio.h>
#include<fcntl.h>		//open()
#include<stdlib.h>		//exit()
#include<unistd.h>		//write()
#include<string.h>		//strlen()
#include<utmp.h>
#include<sys/stat.h>

void
usrname2filename (char *usrname, char *filename, int filenamebufsz)
{

  int fd = open (UTMP_FILE, O_RDONLY);
  if (0 > fd)
    {
      perror (UTMP_FILE);
      exit (1);
    }

  struct utmp record = { 0 };
  int reclen = sizeof (record);
  struct stat fileinfo = { 0 };

  while (reclen == read (fd, &record, reclen))
    {
      if (USER_PROCESS != record.ut_type)
	continue;
      if (0 != strcmp (usrname, record.ut_user))
	continue;
      snprintf (filename, filenamebufsz, "/dev/%s", record.ut_line);
      if (0 > stat (filename, &fileinfo))
	continue;
      else
	return;
    }

  memset (filename, 0, filenamebufsz);
  return;
}

int
getmyname (char *name, int namebufsz)
{
  int euid = geteuid ();
  char euid_str[32];
  int passwd_fd = open ("/etc/passwd", O_RDONLY);
  if (0 > passwd_fd)
    {
      puts ("open file error\n");
      exit (1);
    }
  char line_buf[256];
  int i = 0;
  char *token;
  char *delim = ":";
  int rc = -1;

  sprintf (euid_str, "%d", euid);
  memset (name, 0, namebufsz);
  while (0 < (rc = read (passwd_fd, &line_buf[i], 1)))
    {
      if ('\n' == line_buf[i])
	{
	  line_buf[++i] = 0;
	  if (NULL != strstr (line_buf, euid_str))
	    {
	      token = strtok (line_buf, delim);
	      snprintf (name, namebufsz, "%s", token);
	      return 0;
	    }
	  i = 0;
	}
      else
	i++;
    }
  return 1;
}


int
main (int ac, char *av[])
{
  int fd;
  char buf[BUFSIZ];

  /*check args */
  if (ac != 2)
    {
      fprintf (stderr, "usage: write0 <usrname>\n");
      exit (1);
    }
  char filename[256] = { 0 };
  usrname2filename (av[1], filename, sizeof (filename));
  if (0 == strlen (filename))
    {
      printf
	("we cannot find the usr %s logged on this machine, may contact latter!\n",
	 av[1]);
      return 0;
    }

/*open devices*/
  fd = open (filename, O_WRONLY);
  if (fd == -1)
    {
      perror (av[1]);
      exit (1);
    }

/* loop until EOF on input */
  char *p;
  char myname[256] = { 0 };
  if (0 != getmyname (myname, sizeof (myname)))
    {
      perror ("cannot get my name");
      exit (1);
    }
  sprintf (buf, "hello, this is %s speaking:\n", myname);
  if (-1 == write (fd, buf, strlen (buf)))
    {
      perror ("write to usr file fail");
      exit (1);
    }
  while ((p = fgets (buf, BUFSIZ, stdin)) != NULL)
    if (write (fd, buf, strlen (buf)) == -1)
      break;
  close (fd);
  return 0;
}
