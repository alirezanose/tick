#ifndef UI_H_
#define UI_H_

#include "common.h"
#include "timer.h"
#include "ascii.h"

int ui_init(void);
void ui_render(double elapsed, const Timer *timer);
void ui_shutdown(void);
int input_handling(int ch, Timer *timer);

#endif
