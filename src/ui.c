#include "ui.h"

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

void ui_render(double elapsed, Timer *timer){
    
    clear();
    
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

