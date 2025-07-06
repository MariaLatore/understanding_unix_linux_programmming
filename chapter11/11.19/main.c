#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int
main ()
{
  char cmd[BUFSIZ] = { 0 };
  char result[BUFSIZ] = { 0 };
  pid_t pid = getpid ();	//typedef int pid_t
  sprintf (cmd, "ps|grep %d|awk \'{print $4}\'", pid);
  FILE *fp = popen (cmd, "r");
  if (NULL == fp)
    {
      perror ("popen failed");
      exit (1);
    }


  if (fgets (result, BUFSIZ, fp) == NULL)
    {
      printf ("fgets error!\n");
      exit (1);
    }
  fclose (fp);

  printf ("the program name: %s", result);
  return 0;
}
