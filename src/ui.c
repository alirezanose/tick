#include "ui.h"

/* function for int ui */
void ui_init(void){
    
    if(initscr() == NULL){
	fprintf(stderr, "error call initscr");
	endwin();
    }
    cbreak();
    noecho();
    curs_set(0);
    timeout(200);
    
}

void ui_render(double elapsed){

    int total_seconds = (int)elapsed;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    int hours = total_seconds / 3600;

    mvprintw(10,20,"%02d:%02d:%02d",hours, minutes, seconds);
	
    refresh();

}

void ui_shutdown(void){

    endwin();
    
}

