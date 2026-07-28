#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <signal.h>

#define DFL_SYMBOL 'o'
#define BLANK  ' '

typedef struct {
	int x_pos;
	int y_pos;
	char symbol;
}ppball_t;

ppball_t ball = {0};
int done = 0;

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
	

int main(){
	//init curse
	initscr();
	cbreak();
	noecho();
	clear();
	refresh();
        curs_set(0); 
	signal(SIGINT, SIG_IGN);

	ball.symbol = DFL_SYMBOL;
	ball.x_pos = COLS/2;
	ball.y_pos = LINES/2;
	
	char c;
	move_ball(0,0);
	while((c = getch())){
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
		if(done) break;
	}

	endwin();
	return 0;

}
