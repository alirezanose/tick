#include "audio.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

static bool sound_muted = false;

static const char *tick_filenames[3] =
{
    "tick1.wav",
    "tick2.wav",
    "tick3.wav"
};

static const char *alarm_filename = "alarm.wav";

static bool resolve_sound_path(const char *filename, char *resolved_path, size_t max_len){
    snprintf(resolved_path, max_len, "sounds/%s", filename);
    if(access(resolved_path, F_OK) == 0) return true;

    snprintf(resolved_path, max_len, "/usr/share/tick/sounds/%s", filename);
    if(access(resolved_path, F_OK) == 0) return true;

    snprintf(resolved_path, max_len, "/usr/local/share/tick/sounds/%s", filename);
    if(access(resolved_path, F_OK) == 0) return true;

    return false;
}

static void play_wav_file(const char *filename){
    
    /* check file exist */
    char full_path[512];
    if(!resolve_sound_path(filename, full_path, sizeof(full_path))){
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

	execlp("aplay", "aplay", "-q", full_path, (char *)NULL);
	_exit(0);
    }
}

void audio_init(void){
    sound_muted = false;
    srand((unsigned int)time(NULL));

    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_NOCLDWAIT;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);
}

void audio_play_tick(void){
    if(sound_muted){
	return;
    }
    int random_idx = rand() % 3;
    play_wav_file(tick_filenames[random_idx]);
}

void audio_play_alarm(void){
    play_wav_file(alarm_filename);
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


