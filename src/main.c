#include "common.h"
#include "timer.h"
#include "ui.h"
#include "audio.h"
#include "cli.h"
#include "config.h"
#include "notification.h"

static void app_init(App *app, const CliConfig *cli, const TickConfig *cfg, int argc) {
    timer_init(&app->timer);
    pomodoro_init(&app->pomo);

    /* CLI overrides configuration if specified, otherwise fallback to config defaults */
    app->mode = (argc == 1) ? cfg->default_mode : cli->mode;
    app->countdown_duration = cli->duration_specified ? cli->countdown_duration 
                                                      : cfg->default_countdown_duration;

    app->state = STATE_NORMAL;
    app->running = true;
    app->widget_mode = false;

    /* Apply pomodoro durations from configuration */
    app->pomo.focus_duration = cfg->pomo_focus_duration;
    app->pomo.short_break_duration = cfg->pomo_short_break_duration;
    app->pomo.long_break_duration = cfg->pomo_long_break_duration;

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

    if (!cfg->sound_enabled) {
        audio_toggle_mute();
    }
}

static void app_update(App *app, int *last_sec, double *elapsed) {
    clock_gettime(CLOCK_MONOTONIC, &app->timer.current);
    timer_elapsed(&app->timer, elapsed);
    int current_sec = (int)(*elapsed);

    /* Countdown alarm & notification */
    if (app->mode == MODE_COUNTDOWN && !app->timer.paused && timer_remaining(&app->timer) <= 0.0) {
        audio_play_alarm();
        notification_timer_finished();
        flash();
        app->timer.paused = true;
        *last_sec = -1;
    }

    /* Pomodoro transition alarm & notification */
    if (app->mode == MODE_POMODORO && !app->timer.paused && timer_remaining(&app->timer) <= 0.0) {
        audio_play_alarm();
        pomodoro_next_phase(&app->pomo);
        notification_pomodoro_phase(app->pomo.phase);
        flash();
        app->timer.paused = true;
        timer_reset(&app->timer, pomodoro_get_current_duration(&app->pomo));
        *last_sec = -1;
    }

    /* Mechanical tick audio on second transition */
    if (!app->timer.paused && current_sec != *last_sec) {
        audio_play_tick();
        *last_sec = current_sec;
    }
}

int main(int argc, char *argv[])
{
    TickConfig config;
    config_load_or_create(&config);
    
    CliConfig cli;
    if (cli_parse_args(argc, argv, &cli) != 0) {
        return 1;
    }
    if (cli.action == CLI_ACTION_EXIT_SUCCESS) {
        return 0;
    }

    App app;
    app_init(&app, &cli, &config, argc);
    
    audio_init();
    notification_init();
    notification_set_enabled(config.notification_enabled);

    if (ui_init() == -1) {
        return -1;
    }

    int last_sec = -1;
    while (app.running) {
        int ch = getch();
        input_handling(ch, &app);

        if (!app.running) {
            break;
        }

        double elapsed = 0.0;
        app_update(&app, &last_sec, &elapsed);

        ui_render(elapsed, &app);
    }
    
    audio_free();
    ui_shutdown();
    return 0;
}
