#ifndef CONFIG_H_
#define CONFIG_H_

#include "common.h"
#include "ui.h"

typedef struct {
    AppMode default_mode;
    bool sound_enabled;
    double default_countdown_duration;
    double pomo_focus_duration;
    double pomo_short_break_duration;
    double pomo_long_break_duration;
} TickConfig;

void config_init_defaults(TickConfig *cfg);

int config_load_or_create(TickConfig *cfg);

int config_parse_line(const char *line, TickConfig *cfg);

#endif
