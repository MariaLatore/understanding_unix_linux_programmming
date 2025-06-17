#include<stdio.h>
#include<sys/stat.h>
char lockfilename[] = "data.LCK";
int
main ()
{
  struct stat fileinfo = { 0 };
  if (0 > stat (lockfilename, &fileinfo))
    {
      perror ("stat error");
      return 1;
    }

  printf ("atime:%ld, mtime:%ld, ctime:%ld\n", fileinfo.st_atime,
	  fileinfo.st_mtime, fileinfo.st_ctime);
  return 0;
}
