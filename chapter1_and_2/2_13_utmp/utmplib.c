#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>
#include "utmplib.h"
#include <string.h>

static char utmpbuf[NRECS*UTSIZE];  //storage
static int num_recs;  //num stored
static int cur_rec;  //next to go
static int fd_utmp = -1;

int utmp_open(char *filename)
{
  fd_utmp = open(filename, O_RDONLY);  //open it
  cur_rec = num_recs = 0;  //no recs yet
  return fd_utmp;
}

struct utmp *utmp_next()
{
  struct utmp *recp;
  if(fd_utmp == -1)  //error?
    return NULLUT;
  if(cur_rec == num_recs && utmp_reload() == 0)  //any more?
    return NULLUT;  //get address of next record
  recp = (struct utmp *) &utmpbuf[cur_rec*UTSIZE];
  cur_rec++;
  return recp;
}

int utmp_reload()
/*
 *  read next bunch of records into buffer
 */
{
  int amt_read;  //read them in
  amt_read = read(fd_utmp, utmpbuf, NRECS*UTSIZE);  //how many did we get?
  if(amt_read == 0)
	return 0;
  num_recs = amt_read/UTSIZE;  //reset pointer
  cur_rec = 0;
  return num_recs;
}

void utmp_close()
{
  if(fd_utmp!=-1)  //don't close if not open
    close(fd_utmp);
}

void count_record_num(int *ret_total, int *ret_cur, struct utmp *cur_record)
{
	int fd = utmp_open(UTMP_FILE);
	int arrive_cur = 0;
	*ret_total = 0;
	*ret_cur = 0;
	struct utmp *rec=NULL;
	while(NULLUT != (rec = utmp_next()))
	{
		(*ret_total)++;
		if(0 == arrive_cur)
			(*ret_cur)++;
		if(0 == memcmp(cur_record, rec, sizeof(struct utmp)))
			arrive_cur= 1;
	}
	utmp_close(fd);
}


int utmp_seek(int record_offset, seek_utmp_base_t base)
{
        struct utmp cur_record={0};
	utmp_close();		
	int total_record_num=0, cur_record_num=0;
	int ret=0;
	if(0 == cur_rec && 0 == num_recs)
	{
		memcpy(&cur_record, &utmpbuf[cur_rec*UTSIZE], sizeof(struct utmp));
		count_record_num(&total_record_num, &cur_record_num, &cur_record);
		cur_record_num = 0;
	}
	else
	{
		memcpy(&cur_record, &utmpbuf[(cur_rec-1)*UTSIZE], sizeof(struct utmp));
		count_record_num(&total_record_num, &cur_record_num, &cur_record);
	}
	
	switch( base )
	{
		case SEEK_SET:
			if(record_offset > total_record_num || record_offset < 0)
			{
				printf("the offset %d is invalid, cur %d, base %d, max %d\n", record_offset, cur_record_num, base, total_record_num);
				record_offset = cur_record_num;
				ret=-1;
				break;
			}
			break;
		case SEEK_END:
			if(record_offset < -total_record_num || record_offset > 0)
			{
				printf("the offset %d is invalid, cur %d, base %d, max %d\n", record_offset, cur_record_num, base, total_record_num);
				record_offset = cur_record_num;
				ret=-1;
				break;
			}
			record_offset = total_record_num + record_offset;
			break;
		case SEEK_CUR:
			if(cur_record_num + record_offset > total_record_num || cur_record_num + record_offset < 0)
			{
				printf("the offset %d is invalid, cur %d,  base %d, max %d\n", record_offset, cur_record_num, base, total_record_num);
				record_offset = cur_record_num;
				ret=-1;
				break;
			}
			record_offset =  cur_record_num + record_offset;
			break;
		default:
			printf("the offset %d is invalid, base %d, max %d\n", record_offset, base, total_record_num);
			ret=-1;
			record_offset = cur_record_num;
			break;
	}
	utmp_open(UTMP_FILE);
	
	int i;
	for(i=0; i<record_offset; i++)
		utmp_next();
	return ret;
}	
		


			
			
			



