#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<error.h>
char error_buf[100] = { 0 };

int
main (int argc, char *argv[])
{
  char *optstr = "p";
  int opt;
  while (-1 != (opt = getopt (argc, argv, optstr)));
  if (1 == optind)
    if (0 > mkdir (argv[optind], ACCESSPERMS))
      {
	sprintf (error_buf, "mkdir %s error", argv[optind]);
	perror (error_buf);
	exit (1);
      }

  char *del = "/";
  char *token = strtok (argv[optind], del);
  struct stat file_info;
  while (NULL != token)
    {
      if (0 > stat (token, &file_info))
	if (0 > mkdir (token, ACCESSPERMS))
	  {
	    sprintf (error_buf, "mkdir %s error", token);
	    perror (error_buf);
	    exit (1);
	  }
      if (0 > chdir (token))
	{
	  sprintf (error_buf, "chdir %s error", token);
	  perror (error_buf);
	  exit (1);
	}
      token = strtok (NULL, del);
    }
  return 0;
}
