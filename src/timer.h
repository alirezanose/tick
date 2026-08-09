#ifndef TIMER_H_
#define TIMER_H_

#include "common.h"

/* struct Stopwatch */

typedef struct {
    struct timespec segment_start;
    struct timespec current;
    bool paused;
    double accumulated;
} Stopwatch;

/* function for stopwatch */

int stopwatch_init(Stopwatch *stopwatch);
void stopwatch_toggle(Stopwatch *stopwatch, const int ch);
void stopwatch_elapsed(Stopwatch *stopwatch, double *elapsed);

/* calculate function for stop wacth */
double differences(const struct timespec *segment_start,const struct timespec *current);

#endif
