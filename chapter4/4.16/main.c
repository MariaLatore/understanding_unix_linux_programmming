#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>


int
main (int argc, char *argv[])
{
  char error_msg[100];
  if (3 != argc)
    {
      printf ("Usage:%s <src> <dst>\n", argv[0]);
      exit (1);
    }

  struct stat srcfile_info;
  struct stat dstfile_info;
  if (0 > stat (argv[1], &srcfile_info))
    {
      sprintf (error_msg, "src file %s stat error", argv[1]);
      perror (error_msg);
      exit (1);
    }
  if (!S_ISREG (srcfile_info.st_mode))
    {
      printf ("src %s is not a regular file\n", argv[1]);
      exit (1);
    }

  if (0 > stat (argv[2], &dstfile_info))
    {
      if (0 > rename (argv[1], argv[2]))
	{
	  sprintf (error_msg, "rename from %s to %s error", argv[1], argv[2]);
	  perror (error_msg);
	  exit (1);
	}
      return 0;
    }

  if (!S_ISDIR (dstfile_info.st_mode))
    {
      printf ("dst file %s exists!\n", argv[2]);
      exit (1);
    }

  char desired_file_name[256];
  sprintf (desired_file_name, "%s/%s", argv[2], argv[1]);
  if (0 > rename (argv[1], desired_file_name))
    {
      sprintf (error_msg, "mv file %s to dir %s error", argv[1], argv[2]);
      perror (error_msg);
      exit (1);
    }
  return 0;
}
