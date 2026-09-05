#ifndef TIMER_H_
#define TIMER_H_

#include "common.h"

typedef struct {
    struct timespec segment_start;
    struct timespec current;
    bool paused;
    double accumulated;
    double target_duration;
} Timer;

/* Timer lifecycle & calculations */
int timer_init(Timer *timer);
void timer_reset(Timer *timer, double new_duration);
void timer_toggle(Timer *timer);
void timer_elapsed(const Timer *timer, double *elapsed);
double timer_remaining(const Timer *timer);
bool timer_is_finished(const Timer *timer);
double differences(const struct timespec *segment_start, const struct timespec *current);


/* Digit helper functions for live editing */
int timer_digits_to_seconds(const int digits[6]);
void timer_seconds_to_digits(int total_seconds, int digits[6]);

#endif
