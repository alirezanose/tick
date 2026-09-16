#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include "common.h"
#include "pomodoro.h"

void notification_init(void);
void notification_set_enabled(bool enabled);
bool notification_is_enabled(void);
void notification_send(const char *title, const char *message);

void notification_timer_finished(void);
void notification_pomodoro_phase(PomoPhase phase);

#endif
