#include "common.h"
#include "timer.h"

int main() {
  /* struct timespec segment_start, current; */
  Stopwatch stopwatch;
  /* value for stopwach */
  stopwatch.accumulated = 0.0;
  stopwatch.paused = 1;

  double elapsed;
  
    
    /* my first curses  */
    initscr();
    /* init stop watch */
    stopwatch_init(&stopwatch);
    /* flag for stop */
    stopwatch.paused = 1;

    int ch = 0;
    
    cbreak();
    noecho();
    curs_set(0);
    timeout(200);

    while (ch != 'q') {

	clock_gettime(CLOCK_MONOTONIC, &stopwatch.current);

        stopwatch_toggle(&stopwatch, ch);

	stopwatch_elapsed(&stopwatch, &elapsed);
	
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
