#ifndef UI_H_
#define UI_H_

#include "common.h"
#include "timer.h"

int ui_init(void);
void ui_render(double elapsed, Timer *timer);
void ui_shutdown(void);

#endif
