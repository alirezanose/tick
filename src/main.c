#include "common.h"
#include "timer.h"
#include "ui.h"
#include "audio.h"

int main(void)
{
    double elapsed;
    Timer timer;

    timer.accumulated     = 0.0;
    timer.paused          = true;
    timer.mode            = MODE_COUNTDOWN;
    timer.state           = NORMAL;
    timer.target_duration = 900.0;
    timer.cursor_pos      = 0;
    timer.show_invalid_input = 0;

    /* init for sound */
    audio_init();

    /* init for render ui */
    if (ui_init() == -1) {
        return -1;
    }

    if (timer_init(&timer) == -1) {
        return -1;
    }

    int ch = 0;

    int last_sec = -1;

    while (input_handling(ch, &timer) != -1) {
        clock_gettime(CLOCK_MONOTONIC, &timer.current);

        timer_toggle(&timer, ch);

        timer_elapsed(&timer, &elapsed);

	int current_sec = (int)elapsed;

	if(!timer.paused && current_sec != last_sec){
	    audio_play_tick();
	    /* update last second */
	    last_sec = current_sec;
	}

	if(timer.mode == MODE_COUNTDOWN && !timer.paused && timer_remaining(&timer) <= 0.0){
	    audio_play_alarm();
	    timer.paused = true;
	}

        ui_render(elapsed, &timer);

        ch = getch();
    }
    
    audio_free();
    ui_shutdown();
    return 0;
}
