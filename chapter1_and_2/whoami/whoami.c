#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>

int main()
{
  int euid = geteuid();
  char euid_str[32];
  int passwd_fd = open("/etc/passwd", O_RDONLY);
  if(0 > passwd_fd)
  {
    puts("open file error\n");
    exit(1);
  }
  char line_buf[256];
  int i = 0;
  char *token;
  char *delim=":";
  int rc=-1;

  sprintf(euid_str, "%d", euid);
  while(0 < (rc=read(passwd_fd, &line_buf[i], 1)))
  {
    if('\n' == line_buf[i])
    {
      line_buf[++i] = 0;
      if(NULL!=strstr(line_buf, euid_str))
      {
	token = strtok(line_buf, delim);
	printf("%s\n",token);
	return 0;
      }
      i=0;
    }
    else
      i++;
  }
  return 1;
}
      
      
