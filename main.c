#include <curses.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define DFL_SYMBOL 'o'
#define BLANK ' '

#define EPOLL_NUM 1
#define EPOLL_WAIT_MS 16
#define MOVE_INTERVAL_MS 50

#ifndef EVENTDEV
#define EVENTDEV "/dev/input/event0"
#endif

typedef struct {
  int x_pos;
  int y_pos;
  char symbol;
} ppball_t;

ppball_t ball = {0};
int done = 0;
int quit_requested = 0;
volatile sig_atomic_t tick = 0;

static int up_pressed = 0, down_pressed = 0, right_pressed = 0,
           left_pressed = 0;

void move_ball(int newxdir, int newydir) {
  mvaddch(ball.y_pos, ball.x_pos, BLANK);
  int maxline = LINES;
  int maxcol = COLS;
  if (ball.x_pos + newxdir < 0)
    ball.x_pos = maxcol - 1;
  else if (ball.x_pos + newxdir >= maxcol)
    ball.x_pos = 0;
  else
    ball.x_pos += newxdir;

  if (ball.y_pos + newydir < 0)
    ball.y_pos = maxline - 1;
  else if (ball.y_pos + newydir >= maxline)
    ball.y_pos = 0;
  else
    ball.y_pos += newydir;
  mvaddch(ball.y_pos, ball.x_pos, ball.symbol);
  refresh();
}

void handle_key_event(struct epoll_event *pevent) {
  struct input_event ev;
  ssize_t n;

  while ((n = read(pevent->data.fd, &ev, sizeof(ev))) == sizeof(ev)) {
    if (ev.type != EV_KEY)
      continue;
    switch (ev.code) {
    case KEY_Q:
      if (ev.value == 1)
        quit_requested = 1;
      else if (ev.value == 0 && quit_requested)
        done = 1;
      break;
    case KEY_W:
      up_pressed = (ev.value != 0);
      break;
    case KEY_S:
      down_pressed = (ev.value != 0);
      break;
    case KEY_A:
      left_pressed = (ev.value != 0);
      break;
    case KEY_D:
      right_pressed = (ev.value != 0);
      break;
    default:
      break;
    }
  }
  return;
}

void update_motion(void) {
  int dx = 0, dy = 0;
  if (left_pressed && !right_pressed)
    dx = -1;
  else if (!left_pressed && right_pressed)
    dx = 1;

  if (up_pressed && !down_pressed)
    dy = -1;
  else if (!up_pressed && down_pressed)
    dy = 1;

  if (dx != 0 || dy != 0)
    move_ball(dx, dy);
}

int setup_ticker(int n_msecs) {
  struct itimerval new_timeset;
  long n_sec, n_usecs;

  n_sec = n_msecs / 1000;
  n_usecs = (n_msecs % 1000) * 1000;

  new_timeset.it_interval.tv_sec = n_sec;
  new_timeset.it_interval.tv_usec = n_usecs;
  new_timeset.it_value.tv_sec = n_sec;
  new_timeset.it_value.tv_usec = n_usecs;
  return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

void on_tick(int sig) { tick = 1; }

void init(void) {
  // init curse
  initscr();
  cbreak();
  noecho();
  clear();
  curs_set(0);           // do not display cursor
  leaveok(stdscr, TRUE); // avoid cursor movement artifacts
  nodelay(stdscr, TRUE); // getch unblock

  // signal init
  signal(SIGINT, SIG_IGN);
  struct sigaction action = {0};
  action.sa_handler = on_tick;
  sigaction(SIGALRM, &action, NULL);

  // ball pos init
  ball.symbol = DFL_SYMBOL;
  ball.x_pos = COLS / 2;
  ball.y_pos = LINES / 2;

  move_ball(0, 0);
  if (setup_ticker(MOVE_INTERVAL_MS)) {
    perror("set ticker error");
    exit(-1);
  }
}

int main() {
  int epfd;
  int devfd;
  struct epoll_event events;
  struct epoll_event *monitor_events;
  int evtcnt;
  int ret = 0;
  sigset_t block_sigs;
  sigset_t old_sigs;
  sigemptyset(&block_sigs);
  sigaddset(&block_sigs, SIGALRM);

  devfd = open(EVENTDEV, O_RDONLY | O_NONBLOCK);
  epfd = epoll_create(EPOLL_NUM);

  if (devfd == -1 || epfd == -1) {
    perror("open fd error");
    return -1;
  }

  monitor_events = (struct epoll_event *)malloc(sizeof(*monitor_events));
  if (NULL == monitor_events) {
    perror("malloc error");
    return -1;
  }

  events.events = EPOLLIN;
  events.data.fd = devfd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, devfd, &events);

  init();
  while (done == 0) {
    evtcnt = epoll_wait(epfd, monitor_events, EPOLL_NUM, -1);
    if (evtcnt == 1) {
      handle_key_event(monitor_events);
    } else if (evtcnt == -1) {
      if (errno != EINTR) {
        perror("epoll_wait error");
        ret = -1;
        break;
      }
    }

    int ticktmp;
    sigprocmask(SIG_BLOCK, &block_sigs, &old_sigs);
    ticktmp = tick;
    tick=0;
    sigprocmask(SIG_SETMASK, &old_sigs, NULL);
    if (ticktmp == 1) {
      update_motion();
    }
 
  }

  endwin();
  close(epfd);
  close(devfd);
  return ret;
}
