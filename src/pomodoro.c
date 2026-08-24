#include "pomodoro.h"

void pomodoro_init(Pomodoro *pomo){
    pomo->phase = POMO_PHASE_FOCUS;
    pomo->cycle = 1;
    pomo->focus_duration = 25.0 * 60.0;
    pomo->short_break_duration = 5.0 * 60.0;
    pomo->long_break_duration = 15.0 * 60.0;
}

void pomodoro_reset(Pomodoro *pomo){
    pomo->phase = POMO_PHASE_FOCUS;
    pomo->cycle = 1;
}

double pomodoro_get_current_duration(const Pomodoro *pomo){
    if(pomo->phase == POMO_PHASE_FOCUS){
	return pomo->focus_duration;
    }else if(pomo->phase == POMO_PHASE_SHORT_BREAK){
	return pomo->short_break_duration;
    }else{
	return pomo->long_break_duration;
    }
}

void pomodoro_next_phase(Pomodoro *pomo){
    if(pomo->phase == POMO_PHASE_FOCUS){
	if(pomo->cycle == 4){
	    pomo->phase = POMO_PHASE_LONG_BREAK;
	}else{
	    pomo->phase = POMO_PHASE_SHORT_BREAK;
	}
    }else if(pomo->phase == POMO_PHASE_SHORT_BREAK){
	pomo->cycle++;
	pomo->phase = POMO_PHASE_FOCUS;
    }else if(pomo->phase == POMO_PHASE_LONG_BREAK){
	pomo->cycle = 1;
	pomo->phase = POMO_PHASE_FOCUS;
    }    
}

void pomodoro_get_status_text(const Pomodoro *pomo, char *buffer, size_t size){
    if(pomo->phase == POMO_PHASE_FOCUS){
	snprintf(buffer, size, " [ POMODORO: FOCUS (%d/4) ] ", pomo->cycle);
    }else if(pomo->phase == POMO_PHASE_SHORT_BREAK){
	snprintf(buffer, size, " [ POMODORO: SHORT BREAK (%d/4) ] ", pomo->cycle);
    }else{
	snprintf(buffer, size, " [ POMODORO: LONG BREAK ] ");
    }
}
