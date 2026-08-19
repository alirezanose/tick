#include "ui.h"

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

void ui_render(double elapsed, const Timer *timer)
{
    clear();

    int height, width;
    getmaxyx(stdscr, height, width);
    int center_y = height / 2;
    int center_x = width / 2;

    int start_y = center_y - 3;
    int start_x = center_x - 18;

    static const int digit_offsets[6] = {0, 6, 13, 19, 26, 32};

    if (timer->state == INPUT) {
        int hours   = timer->edit_digits[0] * 10 + timer->edit_digits[1];
        int minutes = timer->edit_digits[2] * 10 + timer->edit_digits[3];
        int seconds = timer->edit_digits[4] * 10 + timer->edit_digits[5];

        mvprintw(start_y - 2, center_x - 6, "[ EDIT TIME ]");

        ascii_time(hours, minutes, seconds, start_y, start_x);

        /* Render cursor underline directly beneath active digit */
        mvprintw(start_y + 5, start_x + digit_offsets[timer->cursor_pos], "^^^^^");


	if(timer->show_invalid_input == true){
	    mvprintw(start_y + 7, center_x - 15, "invalid : duration must be > 0");
	}else{
	    mvprintw(start_y + 7, center_x - 27,
		     "[0-9] Type   [<-/->] Move   [^/v] +/-5s   [ENTER] Save   [ESC] Cancel");
	}
    } else {
        int total_seconds = 0;

        if (timer->mode == MODE_STOPWATCH) {
            total_seconds = (int)elapsed;
            if (timer->paused) {
                mvprintw(start_y - 2, center_x - 5, "[ PAUSED ]");
            } else {
                mvprintw(start_y - 2, center_x - 5, "[ RUNNING ]");
            }
        } else if (timer->mode == MODE_COUNTDOWN) {
            if (timer_is_finished(timer)) {
                total_seconds = 0;
                mvprintw(start_y - 2, center_x - 8, ">> TIME'S UP! <<");
            } else {
                total_seconds = (int)timer_remaining(timer);
                if (timer->paused) {
                    mvprintw(start_y - 2, center_x - 5, "[ PAUSED ]");
                } else {
                    mvprintw(start_y - 2, center_x - 5, "[ RUNNING ]");
                }
            }
        }

        int hours   = total_seconds / 3600;
        int minutes = (total_seconds % 3600) / 60;
        int seconds = total_seconds % 60;

        ascii_time(hours, minutes, seconds, start_y, start_x);

        mvprintw(start_y + 6, center_x - 23,
                 "[SPACE] Start/Pause   [i] Edit   [q] Quit");
    }

    refresh();
}


void ui_shutdown(void)
{
    endwin();
}
