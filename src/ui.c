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



void ui_render(double elapsed, const App *app)
{
    /* clear(); */
    erase();
    
    int height, width;
    getmaxyx(stdscr, height, width);

    /* check size of terminal */
    if(width < MAX_TERM_WIDTH || height < MAX_TERM_HEIGHT){
	ui_print_centered(height / 2 - 1, "terminal to small");
	ui_print_centered(height / 2 + 1, "please resize");
	ui_print_centered(height / 2 + 3, "(min: 45x14)");
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
	    ui_print_centered(start_y + 7, "invalid: duration must be > 0");
	}else{
	    ui_print_centered(start_y + 7, "[0-9] Type   [<-/->] Move   [^/v] +/-5s   [ENTER] Save   [ESC] Cancel");
	}
    } else {
        int total_seconds = 0;

	/* variable for pomodoro get status text fucntion */
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
        }else if(app->mode == MODE_POMODORO){
	    total_seconds = (int)timer_remaining(&app->timer);
	    pomodoro_get_status_text(&app->pomo, pomo_status,
				     sizeof(pomo_status));
	    ui_print_centered(start_y - 2, pomo_status);
	}

        int hours   = total_seconds / 3600;
        int minutes = (total_seconds % 3600) / 60;
        int seconds = total_seconds % 60;

        ascii_time(hours, minutes, seconds, start_y, start_x);
	/* footer */
	char footer[160];
	snprintf(footer, sizeof(footer),
		 "[SPACE] Start/Pause [s] Sound: %s [TAB] Mode [r] Reset  [i] Edit   [q] Quit", audio_is_muted() ? "OFF" : "ON");
	ui_print_centered(start_y + 7, footer);
    }

    refresh();
}


void ui_shutdown(void)
{
    endwin();
}
