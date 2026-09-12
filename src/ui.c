#include "ui.h"
#define MAX_TERM_WIDTH 45
#define MAX_TERM_HEIGHT 14

int ui_init(void)
{
    setlocale(LC_ALL, "");

    if (initscr() == NULL) {
        fprintf(stderr, "Error initializing ncurses screen\n");
        return -1;
    }

    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(100);

    return 0;
}

static void editor_adjust_duration(Editor *editor, int delta_seconds){
    int total_sec = timer_digits_to_seconds(editor->digits);
    total_sec += delta_seconds;
    if(total_sec < 0) total_sec = 0;
    timer_seconds_to_digits(total_sec, editor->digits);
}

void input_handling(int ch, App *app)
{
    if(ch == ERR) return;
    
    if (app->state == STATE_NORMAL) {
        if (ch == 'q') {
	    app->running = false;
            return;
        }

	if (ch == 'w' || ch == 'W') {
	    app->widget_mode = !app->widget_mode;
	    return;
	}

	if (ch == 'm' || ch == 'M' || ch == '?' || ch == 27) {
	    app->state = STATE_MENU;
	    return;
	}

	if (ch == ' ') {
	    timer_toggle(&app->timer);
	    return;
	}

	if(ch == 'r'){
	    if(app->mode == MODE_POMODORO){
		pomodoro_reset(&app->pomo);
		timer_reset(&app->timer, pomodoro_get_current_duration(&app->pomo));
	    }else if(app->mode == MODE_COUNTDOWN){
		timer_reset(&app->timer, app->countdown_duration);
	    }else{
		timer_reset(&app->timer, 0.0);
	    }
	    return;
	}

	if(ch == 's' || ch == 'S'){
	    audio_toggle_mute();
	    return;
	}
	/* switch mode handling. TAB key*/
	if(ch == '\t'){
	    app->mode = (app->mode + 1) % 3;
	    double dur = (app->mode == MODE_POMODORO)  ? pomodoro_get_current_duration(&app->pomo) :
	                 (app->mode == MODE_COUNTDOWN) ? app->countdown_duration : 0.0;
	    timer_reset(&app->timer, dur);
	    return;
	}else if(ch == '1'){
	    app->mode = MODE_COUNTDOWN;
	    timer_reset(&app->timer, app->countdown_duration);
	    return;
	}else if(ch == '2'){
	    app->mode = MODE_STOPWATCH;
	    timer_reset(&app->timer, 0.0);
	    return;
	}else if(ch == '3'){
	    app->mode = MODE_POMODORO;
	    timer_reset(&app->timer, pomodoro_get_current_duration(&app->pomo));
	    return;
	}
	    /* end switch mode handling TAB key*/
	    
        if (ch == 'i') {
	    if (app->widget_mode || app->mode != MODE_COUNTDOWN) {
		return;
	    }
            timer_seconds_to_digits((int)app->countdown_duration, app->editor.digits);
            app->editor.cursor_pos = 0;
            app->editor.show_invalid_input = false;
            app->state = STATE_EDITING;
            return;
        }

        if (app->timer.paused && app->mode == MODE_COUNTDOWN) {
            if (ch == KEY_UP) {
                app->countdown_duration += 5;
                timer_reset(&app->timer, app->countdown_duration);
            } else if (ch == KEY_DOWN) {
                if (app->countdown_duration > 5) {
                    app->countdown_duration -= 5;
                    timer_reset(&app->timer, app->countdown_duration);
                }
            }
        }
    } else if (app->state == STATE_EDITING) {
	/* assign false for invalid input */
	if(ch != -1 && ch != '\n' && ch != KEY_ENTER){
	    app->editor.show_invalid_input = false;   
	}
	
        if (ch >= '0' && ch <= '9') {
            app->editor.digits[app->editor.cursor_pos] = ch - '0';
            if (app->editor.cursor_pos < 5) {
                app->editor.cursor_pos++;
            }
        } else if (ch == KEY_LEFT) {
            if (app->editor.cursor_pos > 0) {
                app->editor.cursor_pos--;
            }
        } else if (ch == KEY_RIGHT) {
            if (app->editor.cursor_pos < 5) {
                app->editor.cursor_pos++;
            }
        } else if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127) {
            if (app->editor.cursor_pos > 0) {
                app->editor.cursor_pos--;
            }
        } else if (ch == KEY_UP) {
            editor_adjust_duration(&app->editor, 5);
        } else if (ch == KEY_DOWN) {
            editor_adjust_duration(&app->editor, -5);
        } else if (ch == '\n' || ch == KEY_ENTER) {
            int total_sec = timer_digits_to_seconds(app->editor.digits);
            if (total_sec > 0) {
                app->countdown_duration = (double)total_sec;
                timer_reset(&app->timer, app->countdown_duration);
		app->state = STATE_NORMAL;
            }else {
		app->editor.show_invalid_input = true;
	    }
        } else if (ch == 27 || ch == 'q') { /* ESC or q cancels edit */
	    app->state = STATE_NORMAL;
        }
    } else if (app->state == STATE_MENU) {
        if (ch == 's' || ch == 'S') {
            audio_toggle_mute();
            return;
        }
        if (ch == 27 || ch == 'm' || ch == 'M' || ch == '\n' || ch == KEY_ENTER || ch == ' ' || ch == 'q') {
            app->state = STATE_NORMAL;
        }
        return;
    }
}

static void ui_print_centered(int y, const char *text){
    int height, width;
    getmaxyx(stdscr, height, width);
    (void)height;

    int len = (int)strlen(text);
    int x = (width - len) / 2;
    if(x < 0) x = 0;

    mvprintw(y,x,"%s", text);
}

void ui_render_tabs(int y, AppMode current_mode){
    char tab_bar[128];
    snprintf(tab_bar, sizeof(tab_bar),
	    "%s %s %s",
	    current_mode == MODE_COUNTDOWN ? "[ 1. Countdown ]" : " 1. Countdown ",
	    current_mode == MODE_STOPWATCH ? "[ 2. Stopwatch ]" : " 2. Stopwatch ",
	    current_mode == MODE_POMODORO ? "[ 3. Pomodoro ]" : " 3. Pomodoro "
	);

    ui_print_centered(y, tab_bar);
}

static void ui_render_widget(double elapsed, const App *app) {
    int height, width;
    getmaxyx(stdscr, height, width);
    (void)width;

    int total_seconds = 0;
    char mode_str[64] = "";
    char status_str[32] = "";

    if (app->mode == MODE_STOPWATCH) {
        total_seconds = (int)elapsed;
        snprintf(mode_str, sizeof(mode_str), "[ STOPWATCH ]");
        snprintf(status_str, sizeof(status_str), app->timer.paused ? "[ PAUSED ]" : "[ RUNNING ]");
    } else if (app->mode == MODE_COUNTDOWN) {
        snprintf(mode_str, sizeof(mode_str), "[ COUNTDOWN ]");
        if (timer_is_finished(&app->timer)) {
            total_seconds = 0;
            snprintf(status_str, sizeof(status_str), "[ >> TIME'S UP! << ]");
        } else {
            total_seconds = (int)timer_remaining(&app->timer);
            snprintf(status_str, sizeof(status_str), app->timer.paused ? "[ PAUSED ]" : "[ RUNNING ]");
        }
    } else if (app->mode == MODE_POMODORO) {
        total_seconds = (int)timer_remaining(&app->timer);
        pomodoro_get_status_text(&app->pomo, mode_str, sizeof(mode_str));
        snprintf(status_str, sizeof(status_str), app->timer.paused ? "[ PAUSED ]" : "[ RUNNING ]");
    }

    int hours   = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;

    char line[160];
    snprintf(line, sizeof(line), "[ %02d:%02d:%02d ]   %s   %s",
             hours, minutes, seconds, mode_str, status_str);

    ui_print_centered(height / 2, line);
}

static void ui_render_too_small_guard(int width, int height) {
    const int box_width = 36;
    const int box_height = 7;

    if (width < box_width || height < box_height) {
        char msg[64];
        snprintf(msg, sizeof(msg), "Resize (%dx%d -> %dx%d)",
                 width, height, MAX_TERM_WIDTH, MAX_TERM_HEIGHT);
        ui_print_centered(height / 2, msg);
        return;
    }

    int start_y = (height - box_height) / 2;
    int start_x = (width - box_width) / 2;
    if (start_x < 0) start_x = 0;
    if (start_y < 0) start_y = 0;

    /* Draw box borders */
    mvaddch(start_y, start_x, ACS_ULCORNER);
    mvaddch(start_y, start_x + box_width - 1, ACS_URCORNER);
    mvaddch(start_y + box_height - 1, start_x, ACS_LLCORNER);
    mvaddch(start_y + box_height - 1, start_x + box_width - 1, ACS_LRCORNER);

    mvhline(start_y, start_x + 1, ACS_HLINE, box_width - 2);
    mvhline(start_y + box_height - 1, start_x + 1, ACS_HLINE, box_width - 2);
    mvvline(start_y + 1, start_x, ACS_VLINE, box_height - 2);
    mvvline(start_y + 1, start_x + box_width - 1, ACS_VLINE, box_height - 2);

    /* Box title */
    const char *title = " Terminal Too Small ";
    mvprintw(start_y, start_x + (box_width - (int)strlen(title)) / 2, "%s", title);

    /* Content */
    mvprintw(start_y + 2, start_x + 3, "Please expand your terminal");
    mvprintw(start_y + 3, start_x + 3, "Current : %2d x %2d", width, height);
    mvprintw(start_y + 4, start_x + 3, "Minimum : %2d x %2d", MAX_TERM_WIDTH, MAX_TERM_HEIGHT);
}

static void ui_render_menu_modal(void) {
    int height, width;
    getmaxyx(stdscr, height, width);

    const int box_width = 54;
    const int box_height = 15;

    int start_y = (height - box_height) / 2;
    int start_x = (width - box_width) / 2;
    if(start_x < 0) start_x = 0;
    if(start_y < 0) start_y = 0;

    /* Clear inside box area */
    for (int y = 0; y < box_height; y++) {
        mvhline(start_y + y, start_x, ' ', box_width);
    }

    /* Draw box borders */
    mvaddch(start_y, start_x, ACS_ULCORNER);
    mvaddch(start_y, start_x + box_width - 1, ACS_URCORNER);
    mvaddch(start_y + box_height - 1, start_x, ACS_LLCORNER);
    mvaddch(start_y + box_height - 1, start_x + box_width - 1, ACS_LRCORNER);

    mvhline(start_y, start_x + 1, ACS_HLINE, box_width - 2);
    mvhline(start_y + box_height - 1, start_x + 1, ACS_HLINE, box_width - 2);
    mvvline(start_y + 1, start_x, ACS_VLINE, box_height - 2);
    mvvline(start_y + 1, start_x + box_width - 1, ACS_VLINE, box_height - 2);

    /* Header Title */
    const char *title = "[ MENU / HELP ]";
    mvprintw(start_y, start_x + (box_width - (int)strlen(title)) / 2, "%s", title);

    /* Shortcut list */
    mvprintw(start_y + 2,  start_x + 4, "[SPACE]    Start / Pause");
    mvprintw(start_y + 3,  start_x + 4, "[TAB]      Switch Mode (Timer/Stopwatch/Pomo)");
    mvprintw(start_y + 4,  start_x + 4, "[1, 2, 3]  Direct Jump to Mode");
    mvprintw(start_y + 5,  start_x + 4, "[i]        Edit Countdown Duration");
    mvprintw(start_y + 6,  start_x + 4, "[w]        Toggle Mini Widget Mode");
    mvprintw(start_y + 7,  start_x + 4, "[UP / DN]  Adjust +/- 5 Seconds");
    mvprintw(start_y + 8,  start_x + 4, "[r]        Reset Current Timer / Cycle");
    mvprintw(start_y + 9,  start_x + 4, "[s]        Toggle Sound (Status: %s)", audio_is_muted() ? "OFF" : "ON");
    mvprintw(start_y + 10, start_x + 4, "[q]        Quit Application");

    /* Footer instruction */
    const char *footer_hint = "[ Press ESC or ENTER to Close ]";
    mvprintw(start_y + 12, start_x + (box_width - (int)strlen(footer_hint)) / 2, "%s", footer_hint);
}

void ui_render(double elapsed, const App *app)
{
    erase();
    
    int height, width;
    getmaxyx(stdscr, height, width);

    /* Guard jika terminal benar-benar terlalu sempit untuk apapun */
    if (width < 34 || height < 3) {
        ui_render_too_small_guard(width, height);
        refresh();
        return;
    }

    /* Mini Widget Mode (Manual Toggle atau Auto-Fallback jika tinggi < 14) */
    if (app->widget_mode || height < MAX_TERM_HEIGHT || width < MAX_TERM_WIDTH) {
        ui_render_widget(elapsed, app);
        if (app->state == STATE_MENU) {
            ui_render_menu_modal();
        }
        refresh();
        return;
    }
    
    int center_y = height / 2;
    int center_x = width / 2;

    int start_y = center_y - 2;
    int start_x = center_x - (ASCII_TIME_WIDTH / 2);

    /* render tabs */
    ui_render_tabs(start_y - 4, app->mode);
    
    if (app->state == STATE_EDITING) {
        int hours   = app->editor.digits[0] * 10 + app->editor.digits[1];
        int minutes = app->editor.digits[2] * 10 + app->editor.digits[3];
        int seconds = app->editor.digits[4] * 10 + app->editor.digits[5];

	ui_print_centered(start_y - 2, "[ EDIT TIME ]");

        ascii_time(hours, minutes, seconds, start_y, start_x);

        /* Render cursor underline directly beneath active digit */
        mvprintw(start_y + 5, start_x + ascii_get_digit_x_offset(app->editor.cursor_pos), "^^^^^");

	if(app->editor.show_invalid_input == true){
	    ui_print_centered(start_y + 7, "Invalid: duration must be > 0");
	}else{
	    ui_print_centered(start_y + 7, "[0-9] Type     [ENTER] Save     [ESC] Cancel");
	}
    } else {
        int total_seconds = 0;
	char pomo_status[64];

        if (app->mode == MODE_STOPWATCH) {
            total_seconds = (int)elapsed;
            if (app->timer.paused) {
		ui_print_centered(start_y - 2, "[ PAUSED ]");
            } else {
		ui_print_centered(start_y - 2, "[ RUNNING ]");
            }
        } else if (app->mode == MODE_COUNTDOWN) {
            if (timer_is_finished(&app->timer)) {
                total_seconds = 0;
		ui_print_centered(start_y - 2, ">> TIME'S UP! <<");
            } else {
                total_seconds = (int)timer_remaining(&app->timer);
		ui_print_centered(start_y - 2, app->timer.paused ? "[ PAUSED ]" : "[ RUNNING ]");
            }
        } else if (app->mode == MODE_POMODORO) {
	    total_seconds = (int)timer_remaining(&app->timer);
	    pomodoro_get_status_text(&app->pomo, pomo_status, sizeof(pomo_status));
	    ui_print_centered(start_y - 2, pomo_status);
	}

        int hours   = total_seconds / 3600;
        int minutes = (total_seconds % 3600) / 60;
        int seconds = total_seconds % 60;

        ascii_time(hours, minutes, seconds, start_y, start_x);
    }

    if (app->state == STATE_MENU) {
        ui_render_menu_modal();
    }

    refresh();
}

void ui_shutdown(void)
{
    endwin();
}
