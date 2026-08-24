#include "timer.h"

void timer_reset(Timer *timer, double new_duration)
{
    timer->target_duration = new_duration;
    timer->accumulated     = 0.0;
    timer->paused          = true;

    clock_gettime(CLOCK_MONOTONIC, &timer->segment_start);
}

double timer_remaining(const Timer *timer)
{
    double elapsed;
    timer_elapsed(timer, &elapsed);
    return timer->target_duration - elapsed;
}

bool timer_is_finished(const Timer *timer)
{
    return timer_remaining(timer) <= 0;
}

void timer_elapsed(const Timer *timer, double *elapsed)
{
    if (timer->paused) {
        *elapsed = timer->accumulated;
    } else {
        *elapsed = timer->accumulated +
            differences(&timer->segment_start, &timer->current);
    }
}

void timer_toggle(Timer *timer, const int ch)
{
    if (timer->state != NORMAL) {
        return;
    }

    if (ch == ' ') {
        if (!timer->paused) {
            timer->accumulated += differences(&timer->segment_start, &timer->current);
        } else {
            timer->segment_start = timer->current;
        }
        timer->paused = !timer->paused;
    }
}

int timer_init(Timer *timer)
{
    /* init for pomodoro */
    pomodoro_init(&timer->pomo);
    
    if (clock_gettime(CLOCK_MONOTONIC, &timer->segment_start) == -1) {
        perror("cannot get time");
        endwin();
        return -1;
    }
    return 0;
}

double differences(const struct timespec *segment_start, const struct timespec *current)
{
    double elapsed_time;

    elapsed_time = (current->tv_sec - segment_start->tv_sec) +
        (current->tv_nsec - segment_start->tv_nsec) / 1000000000.0;

    return elapsed_time;
}

int timer_digits_to_seconds(const int digits[6])
{
    int hours   = digits[0] * 10 + digits[1];
    int minutes = digits[2] * 10 + digits[3];
    int seconds = digits[4] * 10 + digits[5];

    return (hours * 3600) + (minutes * 60) + seconds;
}

void timer_seconds_to_digits(int total_seconds, int digits[6])
{
    if (total_seconds < 0) {
        total_seconds = 0;
    }
    if (total_seconds > 359999) { /* Max 99:59:59 */
        total_seconds = 359999;
    }

    int hours   = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;

    digits[0] = hours / 10;
    digits[1] = hours % 10;
    digits[2] = minutes / 10;
    digits[3] = minutes % 10;
    digits[4] = seconds / 10;
    digits[5] = seconds % 10;
}

void timer_adjust_edit_duration(Timer *timer, int delta_seconds)
{
    int total_sec = timer_digits_to_seconds(timer->edit_digits);
    total_sec += delta_seconds;
    if (total_sec < 0) {
        total_sec = 0;
    }
    timer_seconds_to_digits(total_sec, timer->edit_digits);
}
