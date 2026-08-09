#include "timer.h"

void stopwatch_elapsed(Stopwatch *stopwatch, double *elapsed) {
    
    if (stopwatch->paused) {
      *elapsed = stopwatch->accumulated;
    } else {
      *elapsed = stopwatch->accumulated +
	  differences(&stopwatch->segment_start, &stopwatch->current);
    }
}

/* togglw for stopwatch */
/* int ch for number of toogle */
void stopwatch_toggle(Stopwatch *stopwatch, const int ch) {
    
    if (ch == 32) {
	if (stopwatch->paused == 0) {
	   stopwatch->accumulated += differences(&stopwatch->segment_start, &stopwatch->current);
	} else {
	    stopwatch->segment_start = stopwatch->current;
	}	
	stopwatch->paused = !stopwatch->paused;
    }
}

int stopwatch_init(Stopwatch *stopwatch) {
  if (clock_gettime(CLOCK_MONOTONIC, &stopwatch->segment_start) == -1) {
    perror("cannot get time");
    endwin();
    return -1;
  }
  return 0;
}

double differences(const struct timespec *segment_start,const struct timespec *current) {
  double elapsed_time;
  
  elapsed_time = (current->tv_sec - segment_start->tv_sec) +
      (current->tv_nsec - segment_start->tv_nsec) / 1000000000.0;

  return elapsed_time;
  
}
