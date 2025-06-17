/* who2.c - read /etc/utmp and list info therein
 *        - suppresses empty records
 *        - formats time nicely
 */
#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

//#define SHOWHOST  /* include remote machine on output*/

void showtime(long);
void show_info(struct utmp*);
int am_i(const struct utmp *utmp_info)
{
	char path_buf[50];
	int path_len = 0;
	if(0 > (path_len = readlink("/proc/self/fd/0", path_buf, sizeof(path_buf))))
	{
		puts("error when reading link\n");
		exit(1);
	}
	else
	{
		path_len = path_len >= sizeof(path_buf)? sizeof(path_buf)-1: path_len;
	}
	path_buf[path_len] = 0;

	if(NULL == strstr(path_buf, utmp_info->ut_line))
		return 0;
	else
		return 1;
}

int main(int argc, char* argv[])
{
  struct utmp current_record;  //read info into here
  int         utmpfd;  //read from this descriptior
  int         reclen = sizeof(current_record);
  int         am_i_subfix = 0;
  if(3 < argc || 2 == argc)
  {
	puts("Usage: who [am i]\n");
	exit(1);
  }
  else if( 3 == argc )
  {
	if(0 == strcmp(argv[1], "am") && 0 == strcmp(argv[2], "i"))
		am_i_subfix = 1;
	else
	{
		puts("Usage: who [am i]\n");
		exit(1);
	}
  }

  if((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1){
    perror(UTMP_FILE); // UTMP_FILE is in utmp.h
    exit(1);
  }
  while(read(utmpfd, &current_record, reclen) == reclen)
  {  
	if( 0 == am_i_subfix || 1 == am_i(&current_record))
		show_info(&current_record);
  }
  close(utmpfd);
  return 0;  //went ok
}

/*
 * show info()
 *     displays contents of the utmp struct in human readable form
 *     * displays nothing if record has no user name
 */
void show_info(struct utmp *utbufp)
{
  if(utbufp->ut_type != USER_PROCESS)
    return;

  printf("%-8.8s", utbufp->ut_name);  //the logname
  printf(" ");  //a space
  printf("%-8.8s", utbufp->ut_line);  //the tty
  printf(" ");  //a space
  showtime(utbufp->ut_time);  //display time
#ifdef SHOWHOST
  printf("(%s)", utbufp->ut_host);  //the host
#endif
  printf("\n");  //new line
}

void showtime(long timeval)
/*
 *  displays time in a format fit for human consumption
 *  uses ctime to build a string then picks parts out of it
 *  Note: %12.12s prints a string 12 chars wide and LIMITS
 *  it to 12 chars.
 */
{
  char *cp;  //to hold address of time
  /* convert time to string
     string looks like
     Mon Feb 4 00:46:40 EST 1991
     0123456789012345 */
  cp  = ctime(&timeval);
  printf("%12.12s", cp+4);  //pick 12 chars from pos 4
}
