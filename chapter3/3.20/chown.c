/*
 * udi_t type is defined in /usr/src/kernels/3.10.0-1160.108.1.el7.x86_64/include/linux/types.h:
 * 	typedef __kernel_uid32_t uid_t;
 * __kernel_uid32_t type is defined in  /usr/src/kernels/3.10.0-1160.108.1.el7.x86_64/include/uapi/asm-generic/posix_types.h:
 * 	typedef unsigned int __kernel_uid32_t;
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<pwd.h>
int
main (int ac, char *av[])
{
  if (ac < 3)
    {
      printf ("Usage: %s <username/usrid> <filename1> <filename2> ...\n",
	      av[0]);
      exit (1);
    }
  char *usr = av[1];

  struct passwd *pwdinfo = NULL;
  if (NULL == (pwdinfo = getpwnam (av[1])))
    {

      uid_t id = (unsigned int) atoi (usr);
      pwdinfo = getpwuid (id);
    }

  if (NULL == pwdinfo)
    {
      printf ("cannot find usr %s in the password database", av[1]);
      exit (1);
    }

  uid_t user_id = pwdinfo->pw_uid;
  int i;
  struct stat fileinfo = { 0 };
  for (i = 2; i < ac; i++)
    {
      if (0 > stat (av[i], &fileinfo))
	{
	  perror ("file stat error");
	  continue;
	}
      if (0 > chown (av[i], user_id, -1))
	{
	  perror ("chown error");
	  continue;
	}
    }

  return 0;

}
