#ifndef TIMER_H_
#define TIMER_H_

#include "common.h"

/* calculate function for stop wacth */
double differences(const struct timespec *segment_start,const struct timespec *current);

#endif
