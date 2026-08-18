#include "common.h"
#include "timer.h"
#include "ui.h"

int main(void)
{
    double elapsed;
    Timer timer;

    timer.accumulated     = 0.0;
    timer.paused          = true;
    timer.mode            = MODE_COUNTDOWN;
    timer.state           = NORMAL;
    timer.target_duration = 10.0;
    timer.cursor_pos      = 0;

    if (ui_init() == -1) {
        return -1;
    }

    if (timer_init(&timer) == -1) {
        return -1;
    }

    int ch = 0;

    while (input_handling(ch, &timer) != -1) {
        clock_gettime(CLOCK_MONOTONIC, &timer.current);

        timer_toggle(&timer, ch);

        timer_elapsed(&timer, &elapsed);

        ui_render(elapsed, &timer);

        ch = getch();
    }

    ui_shutdown();
    return 0;
}
