#include "timer.h"

double differences(const struct timespec *segment_start,const struct timespec *current) {
  double elapsed_time;
  
  elapsed_time = (current->tv_sec - segment_start->tv_sec) +
      (current->tv_nsec - segment_start->tv_nsec) / 1000000000.0;

  return elapsed_time;
  
}
