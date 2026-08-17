#include "ui.h"

void input_buffer(char *buffer,int ch,size_t buffer_size, size_t *length ){

    if(ch >= '0' && ch <= '9' && *length < buffer_size -1){
	buffer[*length] = ch;
	(*length)++;
	buffer[*length] = '\0';
    }

    if(ch == '\b' || ch == 127){
	if(*length > 0){
	    (*length)--;
	    buffer[*length] = '\0';
	}
    }
}

/* function input handler */
int input_handling(int ch, Timer *timer, size_t buffer_size, char *buffer, size_t *length){
    
	if(ch == 'q'){
	    return -1;
	}

	
	if(ch == 'i'){
	    timer->state = INPUT;
	    keypad(stdscr, TRUE);
	    echo();
	    memset(buffer, 0, buffer_size);
	    *length = 0;
	    return 1;	    
	}

	if(timer->state == INPUT){
	    
	    if(ch == '\n'){
		timer->state = NORMAL;
		errno = 0;
		noecho();
		keypad(stdscr, FALSE);
		char *endptr;
		long val = strtol(buffer, &endptr, 10);

		if(endptr == buffer){
		    return -1;
		}else{
		    timer_reset(timer, val);
		}


	    }else if((ch >= '0' && ch <= '9') || ch == '\b' || ch == 127){
		input_buffer(buffer, ch, buffer_size, length);
	    }
	}
	return 1;
}

/* function for int ui */
int ui_init(void){
    
    if(initscr() == NULL){
	fprintf(stderr, "error call initscr");
	return -1;
    }
    cbreak();
    noecho();
    curs_set(0);
    timeout(200);
    
    return 0;
}

void ui_render(double elapsed, Timer *timer, const char *buffer){
    
    clear();

    if(timer->state == INPUT){
	mvprintw(8, 20, "set duration");
	mvprintw(10,20, "%s_", buffer);
	mvprintw(12,20, "ENTER: confirm BACKSPACE: for delete");
    }
    
    int result_elapsed = (int)elapsed;

    int result_remained = (int)timer_remaining(timer);

    int result;

    if(timer->mode == MODE_STOPWATCH){
	result = result_elapsed;
    }

    if(timer->mode == MODE_COUNTDOWN){
	if(timer_is_finished(timer)){
	    mvprintw(12,20,"times are gone");
	    result = 0;
	}else {
	    result = result_remained;
	}

    }

    int total_seconds = result;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;
    int hours = total_seconds / 3600;

    mvprintw(10,20,"%02d:%02d:%02d",hours, minutes, seconds);
    
    refresh();

}

void ui_shutdown(void){

    endwin();
    
}

