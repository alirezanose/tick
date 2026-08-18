#include "timer.h"

void timer_reset(Timer *timer, double new_duration)
{
    timer->target_duration = new_duration;
    timer->accumulated     = 0.0;
    timer->paused          = false;

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

/* toggle untuk pause/resume timer */
void timer_toggle(Timer *timer, const int ch)
{
    if (ch == 32) {
        if (timer->paused == 0) {
            timer->accumulated += differences(&timer->segment_start, &timer->current);
        } else {
            timer->segment_start = timer->current;
        }
        timer->paused = !timer->paused;
    }
}

int timer_init(Timer *timer)
{
    if (clock_gettime(CLOCK_MONOTONIC, &timer->segment_start) == -1) {
        perror("cannot get time");
        endwin();
        return -1;
    }
    return 0;
}

/* hitung selisih waktu antara dua timespec */
double differences(const struct timespec *segment_start, const struct timespec *current)
{
    double elapsed_time;

    elapsed_time = (current->tv_sec - segment_start->tv_sec) +
        (current->tv_nsec - segment_start->tv_nsec) / 1000000000.0;

    return elapsed_time;
}
