#include "common.h"
#include "timer.h"
#include "ui.h"

int main() {
  /* struct timespec segment_start, current; */
  Stopwatch stopwatch;
  /* value for stopwach */
  stopwatch.accumulated = 0.0;
  stopwatch.paused = 1;
  /* flag for stop */
  stopwatch.paused = 1;
    
  double elapsed;
  
    
    /* ini ui  */
    ui_init();
    /* init stop watch */
    stopwatch_init(&stopwatch);


    int ch = 0;

    while (ch != 'q') {

	clock_gettime(CLOCK_MONOTONIC, &stopwatch.current);

        stopwatch_toggle(&stopwatch, ch);

	stopwatch_elapsed(&stopwatch, &elapsed);
	/* render the ui for stopwatch */
	ui_render(elapsed);
	
	ch = getch();    
    }

    ui_shutdown();
}
