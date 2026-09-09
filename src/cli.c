#include "cli.h"
#include <ctype.h>

void cli_print_version(void){
    printf("tick v%s - Terminal Clock, Timer, Stopwatch & Pomodoro\n", TICK_VERSION);
}

void cli_print_help(const char *program_name){
    printf("Usage: %s [OPTIONS] [DURATION]\n\n", program_name);
    printf("Options:\n");
    printf("   -s, --stopwatch   Start directly in Stopwatch mode\n");
    printf("   -p, --pomodoro    Start directly in Pomodoro mode\n");
    printf("   -v, --version     Show version information\n");
    printf("   -h, --help        Show this help message\n\n");
    printf("Duration Examples:\n");
    printf("   %s 25m            25 minutes countdown\n", program_name);
    printf("   %s 1h30m          1 hour 30 minutes countdown\n", program_name);
    printf("   %s 90s            90 seconds countdown\n", program_name);
    printf("   %s 45             45 seconds countdown\n", program_name);
}

int cli_parse_duration(const char *str, double *out_seconds) {
    if (!str || *str == '\0') return -1;

    double total = 0.0;
    const char *p = str;
    bool has_any_value = false;

    while (*p != '\0') {
	if (!isdigit((unsigned char) *p)){
	    return -1;
	}

	char *endptr = NULL;
	long val = strtol(p, &endptr, 10);
	if(val < 0 || endptr == p) {
	    return -1;
	}

	p = endptr;
	if (*p == 'h' || *p == 'H') {
	    total += val * 3600.0;
	    p++;
	} else if (*p == 'm' || *p == 'M') {
	    total += val * 60.0;
	    p++;
	} else if (*p == 's' || *p == 'S') {
	    total += val * 1.0;
	    p++;
	} else if (*p == '\0') {
	    total += val * 1.0;
	} else {
	    return -1;
	}

	has_any_value = true;
    }

    if (!has_any_value || total <= 0.0){
	return -1;
    }

    if (out_seconds) {
	*out_seconds = total;
    }

    return 0;
}

int cli_parse_args(int argc, char *argv[], CliConfig *config) {
    if (!config) return -1;

    config->action = CLI_ACTION_RUN;
    config->mode = MODE_COUNTDOWN;
    config->countdown_duration = 900.0;
    config->duration_specified = false;

    for (int i = 1; i < argc; i++){
	const char *arg = argv[i];

	if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
	    cli_print_help(argv[0]);
	    config->action = CLI_ACTION_EXIT_SUCCESS;
	    return 0;
	}

	if (strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0) {
	    cli_print_version();
	    config->action = CLI_ACTION_EXIT_SUCCESS;
	    return 0;
	}

	if (strcmp(arg, "-s") == 0 || strcmp(arg, "--stopwatch") == 0) {
	    config->mode = MODE_STOPWATCH;
	    continue;
	}

	if (strcmp(arg, "-p") == 0 || strcmp(arg, "--pomodoro") == 0) {
	    config->mode = MODE_POMODORO;
	    continue;
	}

	if (arg[0] == '-') {
	    fprintf(stderr, "Error: Unknown option '%s'\n", arg);
	    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
	    config->action = CLI_ACTION_EXIT_ERROR;
	    return -1;
	}

	double duration = 0.0;
	if (cli_parse_duration(arg, &duration) == 0) {
	    config->countdown_duration = duration;
	    config->duration_specified = true;
	    config->mode = MODE_COUNTDOWN;
	} else {
	    fprintf(stderr, "Error: Invalid duration format '%s'\n", arg);
	    fprintf(stderr, "Example formats: 25m, 1h30m, 90s, 45\n");
	    config->action = CLI_ACTION_EXIT_ERROR;
	    return -1;
	}
    }

    return 0;
}
