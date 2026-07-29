#include <curses.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

#define DFL_SYMBOL 'o'
#define BLANK ' '

#define EPOLL_NUM 2
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

void init(void) {
  // init curse
  initscr();
  cbreak();
  noecho();
  clear();
  curs_set(0);           // do not display cursor
  leaveok(stdscr, TRUE); // avoid cursor movement artifacts
  nodelay(stdscr, TRUE); // getch unblock

  // ball pos init
  ball.symbol = DFL_SYMBOL;
  ball.x_pos = COLS / 2;
  ball.y_pos = LINES / 2;

  move_ball(0, 0);
}

void setup_timer(struct itimerspec *ptsp, int ms) {
  ptsp->it_value.tv_sec = (ms / 1000);
  ptsp->it_value.tv_nsec = (ms % 1000) * 1000000;

  ptsp->it_interval.tv_sec = (ms / 1000);
  ptsp->it_interval.tv_nsec = (ms % 1000) * 1000000;
}

int main() {
  int epfd;
  int devfd;
  int timerfd;
  struct epoll_event events;
  struct epoll_event monitor_events[EPOLL_NUM] = {0};
  int evtcnt;
  struct itimerspec timerspec = {0};
  int ret = 0;

  devfd = open(EVENTDEV, O_RDONLY | O_NONBLOCK);
  if (devfd == -1) {
    perror("devfd open error");
    return -1;
  }

  timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (-1 == timerfd) {
    perror("timefd create error");
    return -1;
  }

  epfd = epoll_create(EPOLL_NUM);
  if (epfd == -1) {
    perror("ep fd open error");
    return -1;
  }

  events.events = EPOLLIN;
  events.data.fd = devfd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, devfd, &events);

  events.events = EPOLLIN;
  events.data.fd = timerfd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, timerfd, &events);

  init();
  setup_timer(&timerspec, MOVE_INTERVAL_MS);
  ret = timerfd_settime(timerfd, 0, &timerspec, NULL);
  if (-1 == ret) {
    perror("timer fd settime error");
    return -1;
  }

  while (done == 0) {
    int update = 0;
    evtcnt = epoll_wait(epfd, monitor_events, EPOLL_NUM, -1);
    if (evtcnt == -1) {
      if (errno != EINTR) {
        perror("epoll_wait error");
        ret = -1;
        break;
      }
    }
    for (int i = 0; i < evtcnt; i++) {
      if (monitor_events[i].data.fd == timerfd) {
        uint64_t expiration;
        unsigned int bytes = read(timerfd, &expiration, sizeof(expiration));
        if (bytes == sizeof(expiration))
          update = 1;
        else if (bytes == -1 && errno != EINTR) {
          ret = -1;
          done = 1;
        }
      } else if (monitor_events[i].data.fd == devfd)
        handle_key_event(&monitor_events[i]);
    }
    if (1 == update)
      update_motion();
  }

  endwin();
  close(epfd);
  close(devfd);
  close(timerfd);
  return ret;
}
