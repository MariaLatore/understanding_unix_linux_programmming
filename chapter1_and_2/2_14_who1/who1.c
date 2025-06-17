/* who1.c - a first version of the who program
 *          open, read UTMP file, and show results
 */
#include <stdio.h>
#include <stdlib.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>

#define SHOWHOST  /* include remote machine on output*/

void show_info(struct utmp*);

char *ut_type_str[] = {
	[0] = "EMPTY",
	[1] = "RUN_LVL",
	[2] = "BOOT_TIME",
	[3] = "NEW_TIME",
	[4] = "OLD_TIME",
	[5] = "INIT_PROCESS",
	[6] = "LOGIN_PROCESS",
	[7] = "USER_PROCESS",
	[8] = "DEAD_PROCESS",
	[9] = "ACCOUNTING",
};

int main(int argc, char *argv[])
{
  struct utmp current_record;  //read info into here
  int         utmpfd;  //read from this descriptior
  int         reclen = sizeof(current_record);
  char        *filename;

  if(argc > 2)
  {
    printf("Usage: %s [filename](default utmp)\n", argv[0]);
    return 1;
  }

  if(argc == 1)
    filename = UTMP_FILE;
  else
    filename = argv[1];

  if((utmpfd = open(filename, O_RDONLY)) == -1){
    perror(filename); // UTMP_FILE is in utmp.h
    exit(1);
  }
  while(read(utmpfd, &current_record, reclen) == reclen)
    show_info(&current_record);
  close(utmpfd);
  return 0;  //went ok
}

/*
 * show info()
 *     displays contents of the utmp struct in human readable form
 *     *note* these sizes should not be hardwired
 */
void show_info(struct utmp *utbufp)
{
  printf("%-8.8s", utbufp->ut_name);  //the logname
  printf(" ");  //a space
  printf("%-8.8s", utbufp->ut_line);  //the tty
  printf(" ");  //a space
  printf("%-10d", utbufp->ut_time);  //login time
  printf(" ");  //a space
  printf("%-20.20s", ut_type_str[utbufp->ut_type]); //type
#ifdef SHOWHOST
  printf("%-15.15s", utbufp->ut_host);  //the host
#endif
  putchar(' ');
  printf("usr:%-15.15s", utbufp->ut_user);
  putchar(' ');
  printf("host:%-15.15s", utbufp->ut_host);
  putchar(' ');
  printf("\n");  //new line
}


