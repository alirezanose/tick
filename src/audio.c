#include "audio.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

static bool sound_muted = false;

static const char *tick_sounds[7] =
{
    "sounds/tick1.wav",
    "sounds/tick2.wav",
    "sounds/tick3.wav",
    "sounds/tick4.wav",
    "sounds/tick5.wav",
    "sounds/tick6.wav",
    "sounds/tick7.wav"
};

static const char *alarm_sound = "sounds/alarm.wav";

static void play_wav_file(const char *filepath){
    if(sound_muted){
	return;
    }

    /* check file exist */
    if(access(filepath, F_OK) != 0){
	return;
    }

    pid_t pid = fork();
    if(pid == 0){
	/* child proces */
	int null_fd = open("/dev/null", O_WRONLY);
	if(null_fd != -1){
	    dup2(null_fd, STDERR_FILENO);
	    dup2(null_fd, STDOUT_FILENO);
	    close(null_fd);
	}

	execlp("aplay", "aplay", "-q", filepath, (char *)NULL);
	_exit(0);
    }else if(pid > 0){
	waitpid(-1, NULL, WNOHANG);
    }
}

void audio_init(void){
    sound_muted = false;
    srand((unsigned int)time(NULL));
}

void audio_play_tick(void){
    int random_idx = rand() % 7;
    play_wav_file(tick_sounds[random_idx]);
}

void audio_play_alarm(void){
    play_wav_file(alarm_sound);
}

void audio_toggle_mute(void){
    sound_muted = !sound_muted;
}

bool audio_is_muted(void){
    return sound_muted;
}

void audio_free(void){
    (void)0;
}
