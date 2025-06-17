#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>

void oops(char *s1, char *s2)
{
	fprintf(stderr, "Error: %s ", s1);
	perror(s2);
	exit(1);
}

int main(int argc, char* argv[])
{
	if(3 != argc)
	{
		printf("%s <filename> <tail line num>\n", argv[0]);
		exit(1);
	}

	int fd = open(argv[1], O_RDONLY);
	if(0 > fd)
		oops("cannot open", argv[1]);

	const int blocksize = 64;
	char block[blocksize];
	int newlinecnt=0;
	int tailnum = atoi(argv[2]);
	int offset = blocksize -1;
	int blockread = 0;
	int readsize = 0;
	int i;
	
	while(newlinecnt < tailnum)
	{
		blockread++;
		if(0 > lseek(fd, -blocksize*blockread,SEEK_END))
		{
			blockread--;
			break;
		}
		if(0 > (readsize = read(fd, block, blocksize)))
			oops("cannot read", argv[1]);
		if(0 == readsize)
		{
			offset=0;
			break;
		}


		for(offset=0; offset<readsize; offset++)
		{
			if('\n' == block[readsize-1-offset])
			{
				newlinecnt++;
				if(newlinecnt == tailnum)
					break;
			}
		}
				
	}		

	if(0 > lseek(fd, -blocksize*(blockread-1)-offset, SEEK_END))
		oops("cannot lseek 2", argv[1]);

	while(0 < (readsize = read(fd, block, blocksize)))
	{
		for(i=0; i<readsize; i++)
			putchar(block[i]);
	}
	return 0;
		
}
