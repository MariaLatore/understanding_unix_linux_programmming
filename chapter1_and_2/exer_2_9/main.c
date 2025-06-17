#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>

int main()
{
	int fd = open("file.txt", O_RDWR);
	if(0>lseek(fd, 100, SEEK_END))
	{
		puts("error when lseek");
		exit(1);
	}
	
        char buf[100]={0};
	int rd_len = 0;
	if(0> (rd_len = read(fd, buf, 50)))
	{
		puts("error when read");
		exit(1);
	}
	else
		printf("the read result %s, read len is %d\n", buf, rd_len);
        
        if(0>lseek(fd, 20000, SEEK_END))
	{
		puts("error when lseek");
		exit(1);
	}

	int wr_len=0;
        if(0> (wr_len=write(fd, "hello", sizeof("hello"))))
	{
		puts("error when write");
		exit(1);
	}
	else
		printf("have write %d bytes\n", wr_len);
	close(fd);
	return 0;
}

