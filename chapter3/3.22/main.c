/*who3.c - who with buffered reads
 *       - surpresses empty records
 *       - formats time nicely
 *       - buffers input (using utmplib)
 */
#include <stdio.h>
#include <sys/types.h>
#include <utmp.h>
#include <fcntl.h>
#include <time.h>
#include "utmplib.c"
#include <stdlib.h>
#include <sys/stat.h>

#define SHOWHOST

void show_info (struct utmp *);
void showtime (time_t);
void getmodtime (const char *ptsname);

int
main ()
{
  struct utmp *utbufp,		//holds pointer to next rec
   *utmp_next ();		//returns pointer to next

  if (utmp_open (UTMP_FILE) == -1)
    {
      perror (UTMP_FILE);
      exit (1);
    }
  printf ("%-8.8s %-8.8s %-17.16s %-12.8s %-17.15s\n", "name", "tty",
	  "logintime", "host", "modtime");
  while ((utbufp = utmp_next ()) != ((struct utmp *) NULL))
    show_info (utbufp);
  utmp_close ();
  return 0;
}

/*
 * show info()
 *     displays contents of the utmp struct in human readable form
 *     * displays nothing if record has no user name
 */
void
show_info (struct utmp *utbufp)
{
  if (utbufp->ut_type != USER_PROCESS)
    return;

  printf ("%-8.8s", utbufp->ut_name);	//the logname
  printf (" ");			//a space
  printf ("%-8.8s", utbufp->ut_line);	//the tty
  printf (" ");			//a space
  showtime (utbufp->ut_time);	//display time
#ifdef SHOWHOST
  printf (" %-12.8s ", utbufp->ut_host);	//the host
#endif
  getmodtime (utbufp->ut_line);
  printf ("\n");		//new line
}

void
showtime (long timeval)
/*
 *  displays time in a format fit for human consumption
 *  uses ctime to build a string then picks parts out of it
 *  Note: %12.12s prints a string 12 chars wide and LIMITS
 *  it to 12 chars.
 */
{
  char *cp;			//to hold address of time
  /* convert time to string
     string looks like
     Mon Feb 4 00:46:40 EST 1991
     0123456789012345 */
  cp = ctime (&timeval);
  printf ("%-17.15s", cp + 4);	//pick 12 chars from pos 4
}


void
getmodtime (const char *ptsname)
{
  char namebuf[256] = { 0 };
  snprintf (namebuf, sizeof (namebuf), "/dev/%s", ptsname);
  struct stat fileinfo;
  if (0 > stat (namebuf, &fileinfo))
    {
      printf ("N.A.");
      return;
    }
  printf ("%-17.15s", 4 + ctime (&fileinfo.st_mtime));
}
