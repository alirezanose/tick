#include "config.h"
#include "cli.h"
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>


void config_init_defaults(TickConfig *cfg) {
    if (!cfg) return;
    cfg->default_mode = MODE_COUNTDOWN;
    cfg->sound_enabled = true;
    cfg->default_countdown_duration = 900.0;
    cfg->pomo_focus_duration = 1500.0;
    cfg->pomo_short_break_duration = 300.0;
    cfg->pomo_long_break_duration = 900.0;
}

static char *trim_whitespace(char *str) {
    if (!str) return NULL;
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
	*end = '\0';
	end--;
    }
    return str;
}

int config_parse_line(const char *line, TickConfig *cfg) {
    if (!line || !cfg) return -1;

    char buf[256];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *trimmed = trim_whitespace(buf);
    if (*trimmed == '\0' || *trimmed == '#' || *trimmed == ';') {
	return 0;
    }

    char *eq = strchr(trimmed, '=');
    if (!eq) return -1;

    *eq = '\0';
    char *key = trim_whitespace(trimmed);
    char *val = trim_whitespace(eq + 1);

    if (strcmp(key, "default_mode") == 0) {
	if (strcmp(val, "countdown") == 0) {
	    cfg->default_mode = MODE_COUNTDOWN;
	} else if (strcmp(val, "stopwatch") == 0) {
	    cfg->default_mode = MODE_STOPWATCH;
	} else if (strcmp(val, "pomodoro") == 0) {
	    cfg->default_mode = MODE_POMODORO;
	} else {
	    return -1;
	}
    } else if (strcmp(key, "sound") == 0) {
	if (strcmp(val, "true") == 0 || strcmp(val, "1") == 0 || strcmp(val, "yes") == 0) {
	    cfg->sound_enabled = true;
	} else if (strcmp(val, "false") == 0 || strcmp(val, "0") == 0 || strcmp(val, "no") == 0) {
	    cfg->sound_enabled = false;
	} else {
	    return -1;
	}
    } else if (strcmp(key, "countdown_duration") == 0) {
	double sec = 0.0;
	if (cli_parse_duration(val, &sec) == 0) {
	    cfg->default_countdown_duration = sec;
	} else {
	    return -1;
	}
    } else if (strcmp(key, "pomo_focus") == 0) {
	double sec = 0.0;
	if (cli_parse_duration(val, &sec) == 0) {
	    cfg->pomo_focus_duration = sec;
	} else {
	    return -1;
	}
    } else if (strcmp(key, "pomo_short_break") == 0) {
	double sec = 0.0;
	if (cli_parse_duration(val, &sec) == 0) {
	    cfg->pomo_short_break_duration = sec;
	} else {
	    return -1;
	}
    } else if (strcmp(key, "pomo_long_break") == 0) {
	double sec = 0.0;
	if (cli_parse_duration(val, &sec) == 0) {
	    cfg->pomo_long_break_duration = sec;
	} else {
	    return -1;
	}
    } else {
	return -1; /* Unknown key */
    }

    return 0;
}

static int get_config_paths(char *dir_out, size_t dir_len, char *file_out, size_t file_len) {
    const char *xdg = getenv("XDG_CONFIG_HOME");
    if (xdg && *xdg != '\0') {
	snprintf(dir_out, dir_len, "%s/tick", xdg);
    } else {
	const char *home = getenv("HOME");
	if (!home || *home == '\0') return -1;
	snprintf(dir_out, dir_len, "%s/.config/tick", home);
    }

    snprintf(file_out, file_len, "%s/config.ini", dir_out);
    return 0;
}

static void create_default_config_file(const char *dir_path, const char *file_path) {
    mkdir(dir_path, 0755);

    FILE *f = fopen(file_path, "w");
    if (!f) return;

    fprintf(f, "# Tick Configuration File (~/.config/tick/config.ini)\n\n");
    fprintf(f, "# Default mode when opening tick: countdown: countdown | stopwatch | pomodoro\n");
    fprintf(f, "default_mode = countdown\n\n");
    fprintf(f, "# Sound effects: true | false\n");
    fprintf(f, "sound = true\n\n");
    fprintf(f, "# Default countdown duration (e.g. 15m, 25m, 1h30m, 90s)\n");
    fprintf(f, "countdown_duration = 15m\n\n");
    fprintf(f, "# Pomodoro cycle durations\n");
    fprintf(f, "pomo_focus = 25m\n");
    fprintf(f, "pomo_short_break = 5m\n");
    fprintf(f, "pomo_long_break = 15m\n");

    fclose(f);
}

int config_load_or_create(TickConfig *cfg) {
    if (!cfg) return -1;
    config_init_defaults(cfg);

    char dir_path[512];
    char file_path[512];
    if (get_config_paths(dir_path, sizeof(dir_path), file_path, sizeof(file_path)) != 0) {
	return -1;
    }

    FILE *f = fopen(file_path, "r");
    if (!f) {
	create_default_config_file(dir_path, file_path);
	return 0;
    }

    char line[256];
    while(fgets(line, sizeof(line), f)) {
	config_parse_line(line,cfg);
    }

    fclose(f);
    return 0;
}
