#ifndef UI_H_
#define UI_H_

#include "common.h"
#include "timer.h"

int ui_init(void);
void ui_render(double elapsed, Timer *timer, const char *buffer);
void ui_shutdown(void);
void input_buffer(char *buffer, int ch,size_t buffer_size, size_t *length);
int input_handling(int ch, Timer *timer, size_t buffer_size, char *buffer, size_t *length);

#endif
