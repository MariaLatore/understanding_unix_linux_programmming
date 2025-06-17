#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <curses.h>
#include <sys/time.h>

#define TEST_TIMES 5
#define STR_LEN 512
struct timeval response_time[TEST_TIMES] = { {0}, {0} };

void disp_result ();
int row = 0;
int col = 0;

unsigned int
generate_unsigned_int_random ()
{
  int fd;
  unsigned int random_num;

  fd = open ("/dev/urandom", O_RDONLY);
  if (fd == -1)
    {
      perror ("/dev/urandom open error");
      return 0;
    }

  if (read (fd, &random_num, sizeof (unsigned int)) != sizeof (unsigned int))
    {
      perror ("/dev/urandom read error");
      close (fd);
      return 0;
    }

  close (fd);

  return random_num;
}

int
main ()
{

  initscr ();
  crmode ();
  noecho ();
  clear ();
  refresh ();

  int i;
  char str[STR_LEN] = { 0 };
  struct timeval before, after;
  int testnum;
  unsigned int sleeptime;
  char c;
  for (i = 0; i < TEST_TIMES; i++)
    {
      sleeptime = ((generate_unsigned_int_random () % 10000) * 1000);
      usleep (sleeptime);
      testnum = generate_unsigned_int_random () % 10;
      snprintf (str, STR_LEN, "after sleep %-8u microsecs, please input %d",
		sleeptime, testnum);
      move ((++row) % LINES, col);
      addstr (str);
      refresh ();
      gettimeofday (&before, NULL);
      while ((c = getch ()))
	if (testnum == (c - '0'))
	  {
	    gettimeofday (&after, NULL);
	    break;
	  }
      response_time[i].tv_sec = after.tv_sec - before.tv_sec;
      if (after.tv_usec >= before.tv_usec)
	response_time[i].tv_usec = after.tv_usec - before.tv_usec;
      else
	{
	  response_time[i].tv_sec--;
	  response_time[i].tv_usec = after.tv_usec + 1000000 - before.tv_usec;
	}
      snprintf (str, STR_LEN, "timespan: %lusecs %lumicrocsecs",
		response_time[i].tv_sec, response_time[i].tv_usec);
      move ((++row) % LINES, col);
      addstr (str);
      refresh ();
    }

  snprintf (str, STR_LEN, "test end! Wait a little for result...");
  move ((++row) % LINES, col);
  addstr (str);
  refresh ();

  sleep (3);
  disp_result ();

  endwin ();
  return 0;
}

int
compare_time (const void *a, const void *b)
{
  struct timeval *timea = (struct time_val *) a;
  struct timeval *timeb = (struct time_val *) b;
  if (timea->tv_sec > timeb->tv_sec)
    return 1;
  if (timea->tv_sec == timeb->tv_sec && timea->tv_usec > timeb->tv_usec)
    return 1;
  if (timea->tv_sec == timeb->tv_sec && timea->tv_usec == timeb->tv_usec)
    return 0;
  return -1;
}

void
disp_result ()
{
  char str[STR_LEN];
  row += 2;
  qsort (response_time, TEST_TIMES, sizeof (struct timeval), compare_time);
  snprintf (str, STR_LEN, "min time used: %lu secs %lu micorsecs",
	    response_time[0].tv_sec, response_time[0].tv_usec);
  move ((++row) % LINES, col);
  addstr (str);

  snprintf (str, STR_LEN, "max time used: %lu secs %lu micorsecs",
	    response_time[TEST_TIMES - 1].tv_sec,
	    response_time[TEST_TIMES - 1].tv_usec);
  move ((++row) % LINES, col);
  addstr (str);

  struct timeval average = { 0 };
  int i;
  for (i = 0; i < TEST_TIMES; i++)
    {
      average.tv_usec +=
	response_time[i].tv_sec * 1000000 + response_time[i].tv_usec;
    }

  average.tv_usec = average.tv_usec / TEST_TIMES;
  average.tv_sec = average.tv_usec / 1000000;
  average.tv_usec = average.tv_usec % 1000000;

  snprintf (str, STR_LEN, "average time used: %lu secs %lu micorsecs",
	    average.tv_sec, average.tv_usec);
  move ((++row) % LINES, col);
  addstr (str);

  row++;
  snprintf (str, STR_LEN, "press any key to exit...");
  move ((++row) % LINES, col);
  addstr (str);

  refresh ();
  getch ();
}
