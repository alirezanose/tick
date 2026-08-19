#ifndef TIMER_H_
#define TIMER_H_

#include "common.h"

typedef enum {
    NORMAL,
    INPUT
} InputState;

typedef enum {
    MODE_STOPWATCH,
    MODE_COUNTDOWN
} TimerMode;

typedef struct {
    struct timespec segment_start;
    struct timespec current;
    TimerMode mode;
    InputState state;
    bool paused;
    double accumulated;
    double target_duration;
    int edit_digits[6];  /* [H1, H2, M1, M2, S1, S2] */
    int cursor_pos;      /* Active digit index: 0..5 */
    bool show_invalid_input;
} Timer;

/* Timer lifecycle & calculations */
int timer_init(Timer *timer);
void timer_toggle(Timer *timer, const int ch);
void timer_elapsed(const Timer *timer, double *elapsed);
double timer_remaining(const Timer *timer);
bool timer_is_finished(const Timer *timer);
double differences(const struct timespec *segment_start, const struct timespec *current);
void timer_reset(Timer *timer, double new_duration);

/* Digit helper functions for live editing */
int timer_digits_to_seconds(const int digits[6]);
void timer_seconds_to_digits(int total_seconds, int digits[6]);
void timer_adjust_edit_duration(Timer *timer, int delta_seconds);

#endif
