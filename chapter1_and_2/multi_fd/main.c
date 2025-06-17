#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
  int fd1 = open("file.txt", O_RDONLY);
  int fd2 = open("file.txt", O_WRONLY);
  int fd3 = open("file.txt", O_RDONLY);
  char buf[50]={0};
  char buf2[]="testing 1 2 3...";
  int i;
  const int fd1_readlen = 20;

  if(0 > read(fd1, buf, fd1_readlen))
	printf("read fd1 error\n");
  else
  {
	for(i=0; i < fd1_readlen; i++)
	  putchar(buf[i]);
	putchar('\n');
  }

  if(0 > write(fd2, buf2, strlen(buf2)))
	printf("write fd2 error\n");

  if(0 > read(fd3, buf, fd1_readlen))
	printf("read fd1 error\n");
  else
  {
	for(i=0; i < fd1_readlen; i++)
	  putchar(buf[i]);
	putchar('\n');
  }

  close(fd1); close(fd2); close(fd3);
  return 0;

}
