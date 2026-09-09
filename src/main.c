#include "common.h"
#include "timer.h"
#include "ui.h"
#include "audio.h"
#include "cli.h"

static void app_init(App *app, const CliConfig *config){
    timer_init(&app->timer);
    pomodoro_init(&app->pomo);

    app->mode = config->mode;
    app->countdown_duration = config->countdown_duration;
    app->state = STATE_NORMAL;
    app->running = true;

    double initial_duration = 0.0;
    if (app->mode == MODE_POMODORO) {
	initial_duration = pomodoro_get_current_duration(&app->pomo);
    } else if (app->mode == MODE_COUNTDOWN) {
	initial_duration = app->countdown_duration;
    }
    app->timer.target_duration = initial_duration;
    timer_reset(&app->timer, initial_duration);

    app->editor.cursor_pos = 0;
    app->editor.show_invalid_input = false;
    for (int i = 0; i < 6; i++) {
	app->editor.digits[i] = 0;
    }
}

int main(int argc, char *argv[])
{
    CliConfig cli;
    if (cli_parse_args(argc, argv, &cli) != 0) {
	return 1;
    }

    if (cli.action == CLI_ACTION_EXIT_SUCCESS) {
	return 0;
    }

    App app;
    app_init(&app, &cli);
    
    /* init for sound */
    audio_init();

    /* init for render ui */
    if (ui_init() == -1) {
        return -1;
    }

    int last_sec = -1;
       
    while (app.running) {
	int ch = getch();
	input_handling(ch, &app);

	if(!app.running){
	    break;
	}
	
        clock_gettime(CLOCK_MONOTONIC, &app.timer.current);

	double elapsed = 0.0;
	timer_elapsed(&app.timer, &elapsed);
	int current_sec = (int)elapsed;

	if(app.mode == MODE_COUNTDOWN && !app.timer.paused && timer_remaining(&app.timer) <= 0.0){
	    audio_play_alarm();
	    flash();
	    app.timer.paused = true;
	    last_sec = -1;
	}

	if(app.mode == MODE_POMODORO && !app.timer.paused && timer_remaining(&app.timer) <= 0.0){
	    audio_play_alarm();
	    flash();
	    app.timer.paused = true;
	    pomodoro_next_phase(&app.pomo);
	    timer_reset(&app.timer, pomodoro_get_current_duration(&app.pomo));
	    last_sec = -1;
	}

	if(!app.timer.paused && current_sec != last_sec){
	    audio_play_tick();
	    /* update last second */
	    last_sec = current_sec;
	}
	/* render */
        ui_render(elapsed, &app);
    }
    
    audio_free();
    ui_shutdown();
    return 0;
}
