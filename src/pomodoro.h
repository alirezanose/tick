#ifndef POMODORO_H_
#define POMODORO_H_

#include "common.h"

typedef enum {
    POMO_PHASE_FOCUS,
    POMO_PHASE_SHORT_BREAK,
    POMO_PHASE_LONG_BREAK
}PomoPhase;

typedef struct {
    PomoPhase phase;
    int cycle;
    double focus_duration;
    double short_break_duration;
    double long_break_duration;
}Pomodoro;

void pomodoro_init(Pomodoro *pomo);

void pomodoro_reset(Pomodoro *pomo);

double pomodoro_get_current_duration(const Pomodoro *pomo);

void pomodoro_next_phase(Pomodoro *pomo);

void pomodoro_get_status_text(const Pomodoro *pomo, char *buffer, size_t size);

#endif
