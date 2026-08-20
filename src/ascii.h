#ifndef ASCII_H_
#define ASCII_H_
#include "common.h"

#define ASCII_TIME_WIDTH 37
#define ASCII_TIME_HEIGHT 5
#define ASCII_DIGIT_WIDTH 5

void ascii_digit(int digit, int y, int x);
void ascii_colon(int y, int x);
void ascii_time(int hours, int minutes, int seconds, int y, int x);
int ascii_get_digit_x_offset(int digit_index);

#endif
