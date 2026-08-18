#include "common.h"
#include "timer.h"
#include "ui.h"

#define BUFFERSIZE 8

int main()
{
    char buffer[BUFFERSIZE] = {0};
    size_t length = 0;
    double elapsed;

    Timer timer;

    timer.accumulated    = 0.0;
    timer.paused         = true;   /* mulai dalam kondisi pause */
    timer.mode           = MODE_COUNTDOWN;
    timer.state          = NORMAL;
    timer.target_duration = 10.0;

    /* init ui */
    if (ui_init() == -1) {
        return -1;
    }

    /* init timer */
    if (timer_init(&timer) == -1) {
        return -1;
    }

    int ch = 0;

    while (input_handling(ch, &timer, BUFFERSIZE, buffer, &length) != -1) {
        clock_gettime(CLOCK_MONOTONIC, &timer.current);

        timer_toggle(&timer, ch);

        timer_elapsed(&timer, &elapsed);

        /* render ui */
        ui_render(elapsed, &timer, buffer);

        ch = getch();
    }

    ui_shutdown();
    return 0;
}
