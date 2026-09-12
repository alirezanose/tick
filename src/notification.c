#include "notification.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

static bool g_notification_supported = false;

void notification_init(void) {
    const char *display = getenv("DISPLAY");
    const char *wayland = getenv("WAYLAND_DISPLAY");
    if (!display && !wayland) {
	g_notification_supported = false;
	return;
    }

    if (system("which notify-send > /dev/null 2>&1") == 0) {
	g_notification_supported = true;
    } else {
	g_notification_supported = false;
    }
}

void notification_send(const char *title, const char *message) {
    if (!g_notification_supported) {
	return;
    }

    pid_t pid = fork();
    if (pid < 0) {
	return;
    }

    if (pid == 0) {
	execlp("notify-send", "notify-send",
	       "-a", "Tick",
	       "-i", "appointment-soon",
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
