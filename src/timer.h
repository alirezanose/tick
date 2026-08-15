#ifndef TIMER_H_
#define TIMER_H_

#include "common.h"

/* struct mode */
typedef enum {
    MODE_STOPWATCH,
    MODE_COUNTDOWN
} TimerMode;

/* struct Stopwatch */

typedef struct {
    struct timespec segment_start;
    struct timespec current;
    TimerMode mode;
    bool paused;
    double accumulated;
    double target_duration;
} Timer;

/* function for stopwatch */

int timer_init(Timer *timer);
void timer_toggle(Timer *timer, const int ch);
void timer_elapsed(const Timer *timer, double *elapsed);
double timer_remaining(const Timer *timer);
bool timer_is_finished(const Timer *timer);

/* calculate function for stop wacth */
double differences(const struct timespec *segment_start,const struct timespec *current);

#endif
