#include "notification.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

static bool g_notification_supported = false;
static bool g_notification_enabled = true;

static bool command_exists(const char *cmd) {
    const char *path_env = getenv("PATH");
    if (!path_env) {
        return false;
    }

    char path_copy[2048];
    strncpy(path_copy, path_env, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';

    char *dir = strtok(path_copy, ":");
    while (dir != NULL) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        if (access(full_path, X_OK) == 0) {
            return true;
        }
        dir = strtok(NULL, ":");
    }
    return false;
}

void notification_init(void) {
    const char *display = getenv("DISPLAY");
    const char *wayland = getenv("WAYLAND_DISPLAY");
    if (!display && !wayland) {
        g_notification_supported = false;
        return;
    }

    g_notification_supported = command_exists("notify-send");
}

void notification_set_enabled(bool enabled) {
    g_notification_enabled = enabled;
}

bool notification_is_enabled(void) {
    return g_notification_supported && g_notification_enabled;
}

void notification_send(const char *title, const char *message) {
    if (!g_notification_supported || !g_notification_enabled) {
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        return;
    }

    if (pid == 0) {
        int null_fd = open("/dev/null", O_WRONLY);
        if (null_fd != -1) {
            dup2(null_fd, STDERR_FILENO);
            dup2(null_fd, STDOUT_FILENO);
            close(null_fd);
        }
        execlp("notify-send", "notify-send",
               "-a", "Tick",
               "-u", "normal",
               title,
               message,
               (char *)NULL);
        _exit(0);
    }
}

void notification_timer_finished(void) {
    notification_send("Timer Finished", "Countdown timer has ended!");
}

void notification_pomodoro_phase(PomoPhase phase) {
    if (phase == POMO_PHASE_SHORT_BREAK) {
        notification_send("Pomodoro Break", "Great job! Take a short break.");
    } else if (phase == POMO_PHASE_LONG_BREAK) {
        notification_send("Pomodoro Long Break", "Cycle completed! Enjoy your long break.");
    } else {
        notification_send("Pomodoro Focus", "Break is over! Time to focus.");
    }
}
