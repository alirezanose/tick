#include "common.h"
#include "timer.h"
#include "ui.h"
#define BUFFERSIZE 8

int main() {
    char buffer[8] = {0};
    size_t length = 0;
  /* struct timespec segment_start, current; */
    Timer timer;
  /* value for stopwach */
  timer.accumulated = 0.0;
  /* flag for stop */
  timer.paused = 1;

  /* default mode just temporary */
  timer.mode = MODE_COUNTDOWN;

  timer.state = NORMAL;

  timer.target_duration = 10.0;
    
  double elapsed;
  
    
    /* ini ui  */
  if(ui_init() == -1){
      return -1;
  }
    /* init stop watch */
  if (timer_init(&timer) == -1){
      return -1;
  }


  int ch = 0;
    
  while((input_handling(ch, &timer,BUFFERSIZE, buffer, &length)) != -1){
      
      clock_gettime(CLOCK_MONOTONIC, &timer.current);

      timer_toggle(&timer, ch);

      timer_elapsed(&timer, &elapsed);
      /* render the ui for stopwatch */
      ui_render(elapsed, &timer, buffer);
	
      ch = getch();    
  }

    ui_shutdown();
    /* buffer = NULL; */
    return 0;
}

