#include <curses.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <termios.h>
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

static int up_pressed = 0, down_pressed = 0, right_pressed = 0,
           left_pressed = 0;

static long now_ms(void) {
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (ts.tv_sec * 1000L) + (ts.tv_nsec / 1000000L);
}

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
  refresh();
  curs_set(0);            // do not display cursor
  leaveok(stdscr, TRUE);  // avoid cursor movement artifacts
  nodelay(stdscr, TRUE);  // getch unblock

  // signal init
  signal(SIGINT, SIG_IGN);

  // ball pos init
  ball.symbol = DFL_SYMBOL;
  ball.x_pos = COLS / 2;
  ball.y_pos = LINES / 2;

  move_ball(0, 0);
}

int main() {
  int epfd;
  int devfd;
  struct epoll_event events;
  struct epoll_event *monitor_events;
  int evtcnt;
  int ret = 0;
  long last_move_ms;

  devfd = open(EVENTDEV, O_RDONLY|O_NONBLOCK);
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
  last_move_ms = now_ms();
  while (done == 0) {
    evtcnt = epoll_wait(epfd, monitor_events, EPOLL_NUM, EPOLL_WAIT_MS);
    if (evtcnt == 1) {
      handle_key_event(monitor_events);
    } else if (evtcnt == -1) {
      perror("epoll_wait error");
      ret = -1;
      break;
    }

    if (now_ms() - last_move_ms >= MOVE_INTERVAL_MS) {
      update_motion();
      last_move_ms = now_ms();
    }
  }

  flushinp();
  tcflush(STDIN_FILENO, TCIFLUSH);
  endwin();
  close(epfd);
  close(devfd);
  return ret;
}
