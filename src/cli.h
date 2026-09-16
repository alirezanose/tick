#ifndef CLI_H_
#define CLI_H_

#include "common.h"
#include "ui.h"

#define TICK_VERSION "1.2.0"

typedef enum {
    CLI_ACTION_RUN,
    CLI_ACTION_EXIT_SUCCESS,
    CLI_ACTION_EXIT_ERROR
} CliAction;

typedef struct {
    CliAction action;
    AppMode mode;
    double countdown_duration;
    bool duration_specified;
} CliConfig;

int cli_parse_duration(const char *str, double *out_seconds);
int cli_parse_args(int argc, char *argv[], CliConfig *config);
void cli_print_help(const char *program_name);
void cli_print_version(void);

#endif
