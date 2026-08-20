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

int input_handling(int ch, Timer *timer)
{
    if (timer->state == NORMAL) {
        if (ch == 'q') {
            return -1;
        }

	if(ch == 'r'){
	    timer_reset(timer, timer->target_duration);
	    return 1;
	}
	/* switch mode handling. TAB key*/
	if(ch == '\t'){
	    timer->mode = (timer->mode + 1) % 3;
	    timer_reset(timer, timer->target_duration);
	    return 1;
	}else if(ch == '1'){
	    timer->mode = MODE_COUNTDOWN;
	    timer_reset(timer, timer->target_duration);
	    return 1;
	}else if(ch == '2'){
	    timer->mode = MODE_STOPWATCH;
	    timer_reset(timer, timer->target_duration);
	    return 1;
	}else if(ch == '3'){
	    timer->mode = MODE_POMODORO;
	    timer_reset(timer, timer->target_duration);
	    return 1;
	}
	    /* end switch mode handling */
	    
        if (ch == 'i') {
            timer_seconds_to_digits((int)timer->target_duration, timer->edit_digits);
            timer->cursor_pos = 0;
            timer->state = INPUT;
            return 1;
        }

        if (timer->paused) {
            if (ch == KEY_UP) {
                timer_reset(timer, timer->target_duration + 5);
            } else if (ch == KEY_DOWN) {
                if (timer->target_duration > 5) {
                    timer_reset(timer, timer->target_duration - 5);
                }
            }
        }
    } else if (timer->state == INPUT) {
	/* assign false for invalid input */
	if(ch != -1 && ch != '\n' && ch != KEY_ENTER){
	    timer->show_invalid_input = false;   
	}
	
        if (ch >= '0' && ch <= '9') {
            timer->edit_digits[timer->cursor_pos] = ch - '0';
            if (timer->cursor_pos < 5) {
                timer->cursor_pos++;
            }
        } else if (ch == KEY_LEFT) {
            if (timer->cursor_pos > 0) {
                timer->cursor_pos--;
            }
        } else if (ch == KEY_RIGHT) {
            if (timer->cursor_pos < 5) {
                timer->cursor_pos++;
            }
        } else if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127) {
            if (timer->cursor_pos > 0) {
                timer->cursor_pos--;
            }
        } else if (ch == KEY_UP) {
            timer_adjust_edit_duration(timer, 5);
        } else if (ch == KEY_DOWN) {
            timer_adjust_edit_duration(timer, -5);
        } else if (ch == '\n' || ch == KEY_ENTER) {
            int total_sec = timer_digits_to_seconds(timer->edit_digits);
            if (total_sec > 0) {
                timer_reset(timer, total_sec);
		timer->state = NORMAL;
            }else {
		timer->show_invalid_input = true;
	    }

        } else if (ch == 27 || ch == 'q') { /* ESC or q cancels edit */
	    timer->state = NORMAL;
        }
    }

    return 1;
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

void ui_render_tabs(int y, TimerMode current_mode){
    char tab_bar[128];
    snprintf(tab_bar, sizeof(tab_bar),
	    "%s %s %s",
	    current_mode == MODE_COUNTDOWN ? "[ 1. Countdown ]" : " 1. Countdown ",
	    current_mode == MODE_STOPWATCH ? "[ 2. Stopwatch ]" : " 2. Stopwatch ",
	    current_mode == MODE_POMODORO ? "[ 3. Pomodoro ]" : " 3. Pomodoro "
	);

    ui_print_centered(y, tab_bar);
}



void ui_render(double elapsed, const Timer *timer)
{
    clear();

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
    ui_render_tabs(start_y - 4, timer->mode);
    
    if (timer->state == INPUT) {
        int hours   = timer->edit_digits[0] * 10 + timer->edit_digits[1];
        int minutes = timer->edit_digits[2] * 10 + timer->edit_digits[3];
        int seconds = timer->edit_digits[4] * 10 + timer->edit_digits[5];

	ui_print_centered(start_y - 2, "[ EDIT TIME ]");

        ascii_time(hours, minutes, seconds, start_y, start_x);

        /* Render cursor underline directly beneath active digit */
        mvprintw(start_y + 5, start_x + ascii_get_digit_x_offset(timer->cursor_pos), "^^^^^");

	
	if(timer->show_invalid_input == true){
	    ui_print_centered(start_y + 7, "invalid: duration must be > 0");
	}else{
	    ui_print_centered(start_y + 7, "[0-9] Type   [<-/->] Move   [^/v] +/-5s   [ENTER] Save   [ESC] Cancel");
	}
    } else {
        int total_seconds = 0;

        if (timer->mode == MODE_STOPWATCH) {
            total_seconds = (int)elapsed;
            if (timer->paused) {
		ui_print_centered(start_y - 2, "[ PAUSED ]");
            } else {
		ui_print_centered(start_y - 2, "[ RUNNING ]");
            }
        } else if (timer->mode == MODE_COUNTDOWN || timer->mode == MODE_POMODORO) {
            if (timer_is_finished(timer)) {
                total_seconds = 0;
		ui_print_centered(start_y - 2, ">> TIME'S UP! <<");
            } else {
                total_seconds = (int)timer_remaining(timer);
                if (timer->paused) {
		    ui_print_centered(start_y - 2, "[ PAUSED ]");
                } else {
		    ui_print_centered(start_y - 2, "[ RUNNING ]");
                }
            }
        }

        int hours   = total_seconds / 3600;
        int minutes = (total_seconds % 3600) / 60;
        int seconds = total_seconds % 60;

        ascii_time(hours, minutes, seconds, start_y, start_x);
	/* footer */
	ui_print_centered(start_y + 7, "[SPACE] Start/Pause [TAB] Mode [r] Reset  [i] Edit   [q] Quit");
    }

    refresh();
}


void ui_shutdown(void)
{
    endwin();
}
