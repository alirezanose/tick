#include "ui.h"

void input_buffer(char *buffer, int ch, size_t buffer_size, size_t *length)
{
    if (ch >= '0' && ch <= '9' && *length < buffer_size - 1) {
        buffer[*length] = ch;
        (*length)++;
        buffer[*length] = '\0';
    }

    if (ch == '\b' || ch == 127) {
        if (*length > 0) {
            (*length)--;
            buffer[*length] = '\0';
        }
    }
}

/* function input handler */
int input_handling(int ch, Timer *timer, size_t buffer_size, char *buffer, size_t *length)
{
    if (ch == 'q') {
        return -1;
    }

    if (ch == 'i') {
        timer->state = INPUT;
        keypad(stdscr, TRUE);
        echo();
        memset(buffer, 0, buffer_size);
        *length = 0;
        return 1;
    }

    if (timer->state == INPUT || timer->state == INPUT_ERROR) {

        /* tekan key apapun saat INPUT_ERROR → reset ke INPUT, proses key */
        if (timer->state == INPUT_ERROR) {
            timer->state = INPUT;
            memset(buffer, 0, buffer_size);
            *length = 0;
            /* teruskan key yang ditekan ke handler input normal */
        }

        if (ch == '\n') {
            errno = 0;
            char *endptr;
            long val = strtol(buffer, &endptr, 10);

            if (endptr == buffer || val <= 0) {
                /* validasi gagal: masuk INPUT_ERROR, JANGAN balik ke NORMAL */
                timer->state = INPUT_ERROR;
                /* buffer dikosongkan supaya siap ngetik ulang */
                memset(buffer, 0, buffer_size);
                *length = 0;
            } else {
                /* validasi berhasil: balik NORMAL, terapkan durasi */
                timer->state = NORMAL;
                noecho();
                keypad(stdscr, FALSE);
                timer_reset(timer, val);
            }
        } else if ((ch >= '0' && ch <= '9') || ch == '\b' || ch == 127) {
            input_buffer(buffer, ch, buffer_size, length);
        }
    }
    return 1;
}

/* function for init ui */
int ui_init(void)
{
    setlocale(LC_ALL, "");

    if (initscr() == NULL) {
        fprintf(stderr, "error call initscr");
        return -1;
    }
    cbreak();
    noecho();
    curs_set(0);
    timeout(200);

    return 0;
}

void ui_render(double elapsed, Timer *timer, const char *buffer)
{
    clear();

    int height, width;
    getmaxyx(stdscr, height, width);
    int center_y = height / 2;
    int center_x = width / 2;

    if (timer->state == INPUT || timer->state == INPUT_ERROR) {
        mvprintw(center_y - 3, center_x - 8, "set duration (seconds):");

        if (timer->state == INPUT_ERROR) {
            mvprintw(center_y - 1, center_x - 14, "! invalid: masukkan angka lebih dari 0");
            mvprintw(center_y + 1, center_x - 6, "[ tekan key untuk coba lagi ]");
        } else {
            mvprintw(center_y,     center_x - 6, "%s_", buffer);
            mvprintw(center_y + 2, center_x - 17, "ENTER: konfirmasi   BACKSPACE: hapus");
        }

        refresh();
        return;
    }

    int result_elapsed  = (int)elapsed;
    int result_remained = (int)timer_remaining(timer);
    int result = 0;  /* nilai default aman */

    if (timer->mode == MODE_STOPWATCH) {
        result = result_elapsed;
    } else if (timer->mode == MODE_COUNTDOWN) {
        if (timer_is_finished(timer)) {
            mvprintw(center_y + 5, center_x - 7, "time's up!");
            result = 0;
        } else {
            result = result_remained;
        }
    }

    int total_seconds = result;
    int hours   = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;

    /* mvprintw(center_y, center_x - 4, "%02d:%02d:%02d", hours, minutes, seconds); */
    ascii_time(
        hours,
        minutes,
        seconds,
        center_y - 2,
        center_x - 20
    );

    mvprintw(center_y + 5, center_x - 16, "SPACE: pause/resume   i: set time   q: quit");

    refresh();
}

void ui_shutdown(void)
{
    endwin();
}
