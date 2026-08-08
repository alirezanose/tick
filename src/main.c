#include "common.h"
#include "timer.h"

int main() {
    struct timespec segment_start, current;
    double accumulated = 0.0;
    double elapsed;
    
    /* my first curses  */
    initscr();

    int ch = 0;
    cbreak();
    noecho();
    curs_set(0);
    timeout(200);

    int paused = 1;

    if (clock_gettime(CLOCK_MONOTONIC, &segment_start) == -1) {
	perror("cannot get time");
	endwin();
	return -1;
    }

    while (ch != 'q') {

	clock_gettime(CLOCK_MONOTONIC, &current);
    
	if (ch == 32) {

	    if (paused == 0) {
		accumulated += differences(&segment_start, &current);
	
	    } else {
		segment_start = current;
	    }
	
	    paused = !paused;
	}

        if (paused) {
	    elapsed = accumulated;
        } else {
	    elapsed = accumulated + differences(&segment_start, &current);
        }
	
	clear();

	int total_seconds = (int)elapsed;
        int minutes = total_seconds / 60;
	int seconds = total_seconds % 60;
	
	mvprintw(10,20,"%02d:%02d", minutes, seconds);
    
	refresh();
	ch = getch();    
    }
    endwin();
}
