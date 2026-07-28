#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <signal.h>
#include <sys/time.h>

#define DFL_SYMBOL 'o'
#define BLANK  ' '

typedef struct {
	int x_pos;
	int y_pos;
	char symbol;
}ppball_t;

ppball_t ball = {0};
int done = 0;
int tick = 0;

void move_ball(int newxdir, int newydir){
	mvaddch(ball.y_pos, ball.x_pos, BLANK);
	int maxline = LINES;
	int maxcol = COLS;
	if(ball.x_pos + newxdir < 0) ball.x_pos = maxcol-1;
	else if(ball.x_pos + newxdir >= maxcol) ball.x_pos = 0;
	else ball.x_pos += newxdir;

	if(ball.y_pos + newydir < 0) ball.y_pos = maxline-1;
	else if(ball.y_pos + newydir >= maxline) ball.y_pos = 0;
	else ball.y_pos += newydir;
	mvaddch(ball.y_pos, ball.x_pos, ball.symbol);
	refresh();
}

void on_tick(int sig){
	tick = 1;
}
	

void on_input(){
	int c=ERR,now;
	while((now = getch())!=ERR){
		c = now;
	}
	switch(c){
			case 'q':
				done=1;break;
			case 'w':
				move_ball(0,-1);break;
			case 's':
				move_ball(0,1); break;
			case 'a':
				move_ball(-1,0); break;
			case 'd':
				move_ball(1,0); break;
			default:
				break;
	}
	return ;

}

int setup_ticker(int n_msecs){
	struct itimerval new_timeset;
	long n_sec, n_usecs;

	n_sec = n_msecs/1000;
	n_usecs = (n_msecs%1000)*1000;

	new_timeset.it_interval.tv_sec = n_sec;
	new_timeset.it_interval.tv_usec = n_usecs;
	new_timeset.it_value.tv_sec = n_sec;
	new_timeset.it_value.tv_usec = n_usecs;
	return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

void init(void){
	//init curse
	initscr();
	cbreak();
	noecho();
	clear();
	refresh();
        curs_set(0);  //do not display cursor 
	nodelay(stdscr, TRUE);  //getch unblock
	
	//signal init
	signal(SIGINT, SIG_IGN);
	signal(SIGALRM, on_tick);

	//ball pos init
	ball.symbol = DFL_SYMBOL;
	ball.x_pos = COLS/2;
	ball.y_pos = LINES/2;
	
	move_ball(0,0);

	//init ticker
	setup_ticker(10);
}

int main(){
	sigset_t block_sigs;
	sigset_t old_sigs;
	sigemptyset(&block_sigs);
	sigaddset(&block_sigs, SIGALRM);
	init();
	while(done == 0){
		if(tick == 1){
			sigprocmask(SIG_BLOCK, &block_sigs, &old_sigs);
			tick=0;
			on_input();
			sigprocmask(SIG_SETMASK, &old_sigs, NULL);
		}
	}
	
	endwin();
	return 0;

}
