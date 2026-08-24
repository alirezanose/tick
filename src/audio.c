#include "audio.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/* /\* logging just temp *\/ */

/* this is logging for development*/

/* #include <stdio.h> */
/* #include <time.h> */
/* #include <stdarg.h> */

/* static void log_audio(const char *format, ...){ */
/*     FILE *fp = fopen("tick.log", "a"); */
/*     if(!fp) return; */

/*     time_t now = time(NULL); */
/*     struct tm *t = localtime(&now); */
/*     char time_str[32]; */
/* 	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t); */
/*     fprintf(fp, "[%s]", time_str); */

/*     va_list args; */
/*     va_start(args, format); */
/*     vfprintf(fp, format, args); */
/*     va_end(args); */

/*     fprintf(fp, "\n"); */
/*     fclose(fp); */
/* } */

static bool sound_muted = false;

static const char *tick_sounds[3] =
{
    "sounds/tick1.wav",
    "sounds/tick2.wav",
    "sounds/tick3.wav"
};

static const char *alarm_sound = "sounds/alarm.wav";

static void play_wav_file(const char *filepath){
    if(sound_muted){
	/* log_audio("[audio] muted, skip: %s", filepath); */
	return;
    }

    /* check file exist */
    if(access(filepath, F_OK) != 0){
	/* log_audio("[audio error] file not found: %s", filepath); */
	return;
    }

    /* log_audio("[audio] executing aplay for: %s", filepath); */
    
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
    int random_idx = rand() % 3;
    play_wav_file(tick_sounds[random_idx]);
    /* log_audio("[audio] tick triggered, index : %d, file: %s", random_idx, tick_sounds[random_idx]); */
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
