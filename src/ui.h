#ifndef UI_H_
#define UI_H_

#include "common.h"
#include "timer.h"
#include "ascii.h"
#include "audio.h"
#include "pomodoro.h"

typedef enum {
    MODE_COUNTDOWN,
    MODE_STOPWATCH,
    MODE_POMODORO
} AppMode;
    
typedef enum {
    STATE_NORMAL,
    STATE_EDITING
} AppState;

typedef struct {
    int digits[6];
    int cursor_pos;
    bool show_invalid_input;
} Editor;

typedef struct {
    Timer timer;
    Pomodoro pomo;
    Editor editor;
    AppMode mode;
    AppState state;
    double countdown_duration;
    bool running;
} App;

int ui_init(void);
void ui_render(double elapsed, const App *app);
void ui_shutdown(void);
void input_handling(int ch, App *app);
void ui_render_tabs(int y, AppMode current_mode);

#endif
